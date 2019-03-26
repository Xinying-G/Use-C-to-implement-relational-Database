#include <cstdio>
#include <cstdlib>
#include <libpq-fe.h>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <wait.h>
#include "db.h"
#include "msg.h"

//----------------------------------------------------------------------------------------------------------------------

// Database setup

static const char* DDL[] = {
    "drop table if exists routing cascade",
    "drop table if exists message cascade",
    "drop table if exists member cascade",
    "create table member("
        "member_id serial primary key, "
        "name varchar not null, "
        "birth_date date)",
    "create table message("
        "message_id serial primary key, "
        "message_date date not null, "
        "message_text varchar not null)",
    "create table routing("
        "from_member_id int not null references member, "
        "to_member_id int not null references member, "
        "message_id int not null references message)"
};

static void setup_database(const char* host, const char* dbname, const char* user, const char* password)
{
    PGconn* connection = connect_to_database(host, dbname, user, password);
    begin_transaction(connection);
    int n = sizeof(DDL) / sizeof(const char*);
    for (int i = 0; i < n; i++) {
        update(connection, DDL[i], 0, NULL);
    }
    commit_transaction(connection);
    disconnect_from_database(connection);
}

//----------------------------------------------------------------------------------------------------------------------

// Processing input files

static const int MAX_LINE_SIZE = 200;

static int find_strings(char* line, int n, const char* positions[])
{
    // The string is a comma-separated list of values, and each value is either NULL or a double-quoted string.
    int i = 0;
    char* p = line;
    char* end = line + strlen(line);
    while (p < end) {
        if (!strncmp(p, "NULL", 4)) {
            positions[i++] = NULL;
            p += 4;
        } else if (*p == '"') {
            positions[i++] = p + 1;
            char* matching_quote = strchr(p + 1, '"');
            assert(matching_quote);
            *matching_quote = 0;
            p = matching_quote + 1;
        } else {
            p++;
        }
    }
    return i == n;
}

static void upload_member_rows(FILE* input, PGconn* connection)
{
    char line[MAX_LINE_SIZE];
    const char* fields[2];
    int expected_id = 0;
    int ok;
    do {
        ok =
            fscanf(input, "%s\n", line) != EOF &&
            find_strings(line, 2, fields);
        if (ok) {
            int member_id = add_member(connection, fields[0], fields[1]);
            if (member_id != ++expected_id) {
                fprintf(stderr, "Unexpected member id %d\n", member_id);
            }
        }
    } while (ok);
    //fclose(input);
}

static void upload_message_rows(FILE* input, PGconn* connection)
{
    char line[MAX_LINE_SIZE];
    const char* fields[2];
    int expected_id = 0;
    int ok;
    do {
        ok =
            fscanf(input, "%s\n", line) != EOF &&
            find_strings(line, 2, fields);
        if (ok) {
            int message_id = add_message(connection, fields[0], fields[1]);
            if (message_id != ++expected_id) {
                fprintf(stderr, "Unexpected message id %d\n", message_id);
            }
        }
    } while (ok);
}

static void upload_routing_rows(FILE* input, PGconn* connection)
{
    const int MESSAGE_IDS_BUFFER_SIZE = 1000;
    char line[MAX_LINE_SIZE];
    char from_member_id[MAX_LINE_SIZE];
    char to_member_id[MAX_LINE_SIZE];
    char message_ids[MESSAGE_IDS_BUFFER_SIZE];
    char* mp = message_ids;
    int message_id_count = 0;
    const char* fields[3];
    from_member_id[0] = 0;
    to_member_id[0] = 0;
    int x = 0;
    while (1) {
        int have_line =
            fscanf(input, "%s\n", line) != EOF &&
            find_strings(line, 3, fields);
        int new_members =
            have_line &&
            (strcmp(from_member_id, fields[0]) != 0 || strcmp(to_member_id, fields[1]) != 0);
        // mp > message_ids: We have seen message ids. This will be true, except for the first line.
        int old_members_done = (new_members && mp > message_ids) || !have_line;
        if (old_members_done) {
            *mp++ = '}';
            *mp++ = 0;
            int routing_rows = add_routing(connection, from_member_id, to_member_id, message_ids);
            if (routing_rows != message_id_count) {
                fprintf(stderr, "Posted %d message ids, inserted %d\n", message_id_count, routing_rows);
            }
        }
        if (new_members) {
            strcpy(from_member_id, fields[0]);
            strcpy(to_member_id, fields[1]);
            mp = message_ids;
            *mp++ = '{';
            message_id_count = 0;
        } else if (have_line) {
            *mp++ = ',';
        }
        if (have_line) {
            mp += sprintf(mp, "%s", fields[2]);
            message_id_count++;
        } else {
            break;
        }
        x++;
    }
}

//----------------------------------------------------------------------------------------------------------------------

// Exercising the message app

static int check(int test)
{
    return test ? 0 : 1;
}

static int test_q1(PGconn* connection)
{
    int errors = 0;
    char buffer[20] = "";
    int found = birth_date(connection, "Tweetii", buffer);
    errors += check(found);
    errors += check(!strcmp(buffer, "1989-10-01"));
    return errors;
}

static int test_q1_no_such_member(PGconn* connection)
{
    int errors = 0;
    char buffer[20] = "";
    int found = birth_date(connection, "notamember", buffer);
    errors += check(!found);
    return errors;
}

static int test_q7(PGconn* connection)
{
    int errors = 0;
    const int MAX_ROWS = 100;
    const int MAX_ROW_SIZE = 100;
    char buffer[MAX_ROWS * MAX_ROW_SIZE] = "";
    char* rows[100];
    for (int r = 0; r < MAX_ROWS; r++) {
        rows[r] = &buffer[r * MAX_ROW_SIZE];
    }
    int n_rows = senders_and_receivers(connection, "2016/05/17", MAX_ROWS, rows);
    int r = 0;
    errors += check(!strcmp(rows[r++], "Abderian,Abderian"));
    errors += check(!strcmp(rows[r++], "Abderian,Cephalophore"));
    errors += check(!strcmp(rows[r++], "Abderian,Cynosure"));
    errors += check(!strcmp(rows[r++], "Abderian,Froglet"));
    errors += check(!strcmp(rows[r++], "Abderian,Oscitate"));
    errors += check(!strcmp(rows[r++], "Oscitate,Bamboozled"));
    errors += check(!strcmp(rows[r++], "Oscitate,Flyaway"));
    errors += check(!strcmp(rows[r++], "Oscitate,Hindforemost"));
    errors += check(!strcmp(rows[r++], "Oscitate,Salpiglossis"));
    errors += check(!strcmp(rows[r++], "Squeezewas,Anchusa"));
    errors += check(!strcmp(rows[r++], "Squeezewas,Bamboozled"));
    errors += check(!strcmp(rows[r++], "Squeezewas,Botuliform"));
    errors += check(!strcmp(rows[r++], "Squeezewas,Moneyocracy"));
    errors += check(!strcmp(rows[r++], "Squeezewas,Normalcy"));
    errors += check(!strcmp(rows[r++], "Unguiferous,Flyaway"));
    errors += check(!strcmp(rows[r++], "Unguiferous,Moneyocracy"));
    errors += check(!strcmp(rows[r++], "Unguiferous,Tweetii"));
    errors += check(!strcmp(rows[r++], "Unguiferous,Unguiferous"));
    errors += check(r == n_rows);
    return errors;
}

static void test_msg(const char* host,
                     const char* dbname,
                     const char* user,
                     const char* password)
{
    PGconn* connection = connect_to_database(host, dbname, user, password);
    // upload_member_rows(fopen("member.csv", "r"), connection);
    // upload_message_rows(fopen("message.csv", "r"), connection);
    // upload_routing_rows(fopen("routing.csv", "r"), connection);
    FILE *file1 = fopen("member.csv", "r");
    FILE *file2 = fopen("message.csv", "r");
    FILE *file3 = fopen("routing.csv", "r");
    upload_member_rows(file1, connection);
    upload_message_rows(file2, connection);
    upload_routing_rows(file3, connection);
    fclose(file1);
    fclose(file2);
    fclose(file3);
    int errors = 0;
    errors += test_q1(connection);
    errors += test_q1_no_such_member(connection);
    errors += test_q7(connection);
    disconnect_from_database(connection);
    printf("Test of message API: %d errors\n", errors);
}

//----------------------------------------------------------------------------------------------------------------------

// Main

void usage()
{
    const char* USAGE = "a6 HOST DBNAME USER PASSWORD\n";
    fprintf(stderr, "%s\n", USAGE);
    exit(1);
}

int main(int argc, const char** argv)
{
    if (argc != 5) {
        usage();
    }
    int a = 1;
    const char* host = argv[a++];
    const char* dbname = argv[a++];
    const char* user = argv[a++];
    const char* password = argv[a++];
    setup_database(host, dbname, user, password);
    test_msg(host, dbname, user, password);
}
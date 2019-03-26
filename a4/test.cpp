#include <fstream>
#include "Database.h"
#include "unittest.h"

using namespace std;

static char *db_dir;

static Table *user;
static Table *routing;
static Table *message;

// ------------------------------------------------------------------------------------------

// UTILITIES

// Simplifies memory management of stack-based rows used in testing
class TestRow: public Row
{
public:
    TestRow(Table* table, const vector<string>& values)
        : Row(table)
    {
        auto n = values.size();
        for (unsigned i = 0; i < n; i++) {
            append(values.at(i));
        }
    }
};

static bool add(Table* table, const vector<string>& values)
{
    Row* row = new Row(table);
    try {
        unsigned long n = values.size();
        for (unsigned i = 0; i < n; i++) {
            row->append(values.at(i));
        }
        bool added = table->add(row);
        if (!added) {
            delete row;
        }
        return added;
    } catch (TableException& e) {
        delete row;
        throw e;
    }
}

static bool remove(Table* table, const vector<string>& values)
{
    TestRow row(table, values);
    return table->remove(&row);
}

static bool has(Table* table, const vector<string>& values)
{
    TestRow row(table, values);
    return table->has(&row);
}

static bool row_eq(Row* x, Row* y)
{
    const ColumnNames& xColumnNames = x->table()->columns();
    const ColumnNames& yColumnNames = y->table()->columns();
    unsigned long n = xColumnNames.size();
    assert(yColumnNames.size() == n);
    for (unsigned long i = 0; i < n; i++) {
        if (x->value(xColumnNames.at(i)) != y->value(yColumnNames.at(i))) {
            return false;
        }
    }
    return true;

}

static bool table_eq(Table* x, Table* y)
{
    if (x->columns().size() != y->columns().size()) {
        return false;
    }
    if (x->rows().size() != y->rows().size()) {
        return false;
    }
    // C++ sets are ordered
    auto i = x->rows().begin();
    auto j = y->rows().begin();
    auto i_end = x->rows().end();
    auto j_end = y->rows().end();
    while (i != i_end && j != j_end) {
        if (!row_eq(*i++, *j++)) {
            return false;
        }
    }
    return true;
}

void print_table(const char *label, Table *t)
{
    printf("%s:\n", label);
    auto columns = t->columns();
    bool first = true;
    for (auto column : columns) {
        if (first) {
            first = false;
        } else {
            printf("\t");
        }
        printf("%s", column.c_str());
    }
    printf("\n");
    auto n = columns.size();
    for (auto row : t->rows()) {
        for (unsigned long i = 0; i < n; i++) {
            if (i > 0) {
                printf("\t");
            }
            printf("%s", row->value(t->columns().at(i)).c_str());
        }
        printf("\n");
    }
}

static string strip_eol(string line)
{
    char c;
    int remove = 0;
    while ((c = line.at(line.size() - 1 - remove)) == '\r' || c == '\n') {
        remove++;
    }
    return line.substr(0, line.size() - remove);
}

// ------------------------------------------------------------------------------------------

// Loading the database from .csv files

static void load_table(Table *table, string db_dir, const string &filename)
{
    if (db_dir.at(db_dir.size() - 1) != '/') {
        db_dir += '/';
    }
    string user_path = db_dir + filename;
    ifstream input(user_path);
    string line;
    if (input.is_open()) {
        while (getline(input, line)) {
            line = strip_eol(line);
            vector<string> fields;
            size_t start_scan = 0;
            bool done = false;
            while (!done) {
                size_t after_field = line.find(',', start_scan + 1);
                if (after_field == string::npos) {
                    done = true;
                    after_field = line.size();
                }
                string field = line.substr(start_scan + 1, after_field - 2 - start_scan); // Skips quote marks
                fields.emplace_back(field);
                start_scan = after_field + 1;
            }
            add(table, fields);
        }
    } else {
        fprintf(stderr, "Can't open %s?!\n", user_path.c_str());
    }
}

static void import(const char *db_dir)
{
    user = Database::new_table("user", ColumnNames{"user_id", "username", "birth_date"});
    routing = Database::new_table("routing", ColumnNames{"from_user_id", "to_user_id", "message_id"});
    message = Database::new_table("message", ColumnNames{"message_id", "send_date", "text"});
    load_table(user, db_dir, "user.csv");
    load_table(routing, db_dir, "routing.csv");
    load_table(message, db_dir, "message.csv");
}

static void setup()
{
    Database::delete_all_tables();
    import(db_dir);
}

static void reset_database()
{
    Database::delete_all_tables();
}

static Table* IMPLEMENT_ME()
{
    return Database::new_table(Database::new_table_name(), ColumnNames{"dummy"});
}

//----------------------------------------------------------------------------------------------------------------------

// What are the names of users who have not sent a message to themselves?

static bool q0_predicate(Row *row)
{
    return row->value("from_user_id") == row->value("to_user_id");
}

static void test_q0()
{
    Table *q0 =
        diff(
            project(user, ColumnNames{"username"}),
            project(
                join(
                    rename(
                        select(routing, q0_predicate),
                        NameMap({{"from_user_id", "user_id"}})),
                    user),
                ColumnNames{"username"}));
    // print_table("q0", q0);
    Table *control0 = Database::new_table("control0", ColumnNames{"username"});
    add(control0, {"Intaglio"});
    add(control0, {"Unguiferous"});
    assert(table_eq(control0, q0));
}

//----------------------------------------------------------------------------------------------------------------------

// What is the birth date of Tweetii?

static bool q1_predicate(Row *row)
{
    return row->value("username") == "Tweetii";
}

static void test_q1()
{
    Table *q1 = project(select(user,q1_predicate), ColumnNames{"birth_date"});
    Table *control1 = Database::new_table("control1", ColumnNames{"birth_date"});
    add(control1, {"1984/02/28"});
    assert(table_eq(control1, q1));
}

//----------------------------------------------------------------------------------------------------------------------

// What are the send dates of messages sent by Zyrianyhippy?

static bool q2_predicate(Row *row)
{
    return row->value("username") == "Zyrianyhippy";
}

static void test_q2()
{
    
    Table *q2 = project(    
        join(    
        project(
            join(
                select(user,q2_predicate),rename(
                    routing,NameMap({{"from_user_id", "user_id"}}))),
                ColumnNames{"message_id"}),message),ColumnNames{"send_date"});


    Table *control2 = Database::new_table("control2", ColumnNames{"send_date"});
    add(control2, {"2015/01/09"});
    add(control2, {"2015/04/29"});
    add(control2, {"2015/12/25"});
    add(control2, {"2016/01/08"});
    add(control2, {"2016/02/09"});
    add(control2, {"2016/02/22"});
    add(control2, {"2016/03/25"});
    add(control2, {"2016/04/26"});
    add(control2, {"2016/09/05"});
    add(control2, {"2016/10/08"});
    add(control2, {"2017/01/10"});
    add(control2, {"2017/06/07"});
    add(control2, {"2017/08/05"});
    assert(table_eq(control2, q2));
}

//----------------------------------------------------------------------------------------------------------------------

// What are the usernames of members who received messages on their birthdays?

static bool q3_predicate(Row *row)
{
    // Date format is yyyy/mm/dd. substr(5, 5) extracts mm/dd.
    return row->value("birth_date").substr(5, 5) == row->value("send_date").substr(5, 5);
}

static void test_q3()
{
    Table *q3 = project(
        select(
            join(
                join(user,
                    rename(routing,NameMap({{"to_user_id", "user_id"}}))),message)
            ,q3_predicate),ColumnNames{"username"});

    Table *control3 = Database::new_table("control3", ColumnNames{"username"});
    add(control3, {"Moneyocracy"});
    assert(table_eq(control3, q3));
}

//----------------------------------------------------------------------------------------------------------------------

// What are the send dates of messages from Unquiferous to Froglet?

static bool q4_from_predicate(Row *row)
{
    return row->value("username") == "Unguiferous";
}

static bool q4_to_predicate(Row *row)
{
    return row->value("username") == "Froglet";
}

static void test_q4()
{
    Table *q4 = project(
        join(
            join(
                join(
                    rename(
                        select(user,q4_from_predicate),NameMap({{"user_id", "from_user_id"}}))
                    ,routing),        
            project(
                rename(select(user,q4_to_predicate),NameMap({{"user_id", "to_user_id"}})),
                ColumnNames{"to_user_id"})),message),ColumnNames{"send_date"}); 
      
    Table *control4 = Database::new_table("control4", ColumnNames{"send_date"});
    add(control4, {"2016/12/14"});
    assert(table_eq(control4, q4));
}

//----------------------------------------------------------------------------------------------------------------------

// Who sent messages on 2017/12/22?

static bool q5_predicate(Row *row)
{
    return row->value("send_date") == "2017/12/22";
}

static void test_q5()
{
    Table *q5 = project(
        join(
            rename(
                project(
                    join(
                        select(message,q5_predicate),routing),ColumnNames{"from_user_id"}),
                NameMap({{"from_user_id","user_id"}})),user),ColumnNames{"username"}); 

    Table *control5 = Database::new_table("control5", ColumnNames{"username"});
    add(control5, {"Moneyocracy"});
    assert(table_eq(control5, q5));
}

//----------------------------------------------------------------------------------------------------------------------

// What are the names of users who received messages from Bamboozled?

static bool q6_predicate(Row *row)
{
    return row->value("username") == "Bamboozled";
}

static void test_q6()
{
    Table *q6 = project(
        join(
            rename(
                project(
                    join(
                        rename(
                            project(
        select(user,q6_predicate),ColumnNames{"user_id"}),NameMap({{"user_id","from_user_id"}})),
            routing),ColumnNames{"to_user_id"}),NameMap({{"to_user_id","user_id"}})),
            user),ColumnNames{"username"});

    Table *control6 = Database::new_table("control6", ColumnNames{"username"});
    add(control6, {"Abderian"});
    add(control6, {"Anchusa"});
    add(control6, {"Bamboozled"});
    add(control6, {"Botuliform"});
    add(control6, {"Cynosure"});
    add(control6, {"Flyaway"});
    add(control6, {"Froglet"});
    add(control6, {"Hindforemost"});
    add(control6, {"Intaglio"});
    add(control6, {"Lucarne"});
    add(control6, {"Normalcy"});
    add(control6, {"Oscitate"});
    add(control6, {"Salpiglossis"});
    add(control6, {"Squeezewas"});
    add(control6, {"Tweetii"});
    add(control6, {"Unguiferous"});
    add(control6, {"Zyrianyhippy"});
    assert(table_eq(control6, q6));
}

//----------------------------------------------------------------------------------------------------------------------

// What are the names of senders and receivers of messages sent on 2015/09/26?

static bool q7_predicate(Row *row)
{
    return row->value("send_date") == "2015/09/26";
}

static void test_q7()
{
    Table *q7 = project(join(join(
        join(
            select(message,q7_predicate),routing),
                rename(user,
                    NameMap({{"user_id","from_user_id"},{"username","from_username"}}))),
                        project(
                    rename(user,NameMap({{"user_id","to_user_id"},{"username","to_username"}})),
        ColumnNames{"to_username","to_user_id"})),
    ColumnNames{"from_username", "to_username"});
    
    Table *control7 = Database::new_table("control7", ColumnNames{"from_username", "to_username"});
    add(control7, {"Abderian", "Anchusa"});
    add(control7, {"Anchusa", "Anchusa"});
    assert(table_eq(control7, q7));
}

//----------------------------------------------------------------------------------------------------------------------

void test_queries(int argc, const char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Specify the directory containing the .csv files as a command-line argument.\n");
        exit(1);
    }
    db_dir = strdup(argv[1]);
    BEFORE_ALL_TESTS(setup);
    AFTER_ALL_TESTS(reset_database);
    ADD_TEST(test_q0);
    ADD_TEST(test_q1);
    ADD_TEST(test_q2);
    ADD_TEST(test_q3);
    ADD_TEST(test_q4);
    ADD_TEST(test_q5);
    ADD_TEST(test_q6);
    ADD_TEST(test_q7);
    RUN_TESTS();
    free(db_dir);
}

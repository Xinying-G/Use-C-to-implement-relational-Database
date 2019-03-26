#include <iostream>
#include <fstream>
#include <cassert>
#include "Database.h"
#include "unittest.h"
#include "util.h"

using namespace std;

static char *db_dir;

static Table *user;
static Index* username_index;
static Table *routing;
static Table *message;

// ------------------------------------------------------------------------------------------

// Loading the database from .csv files

static string strip_eol(string line)
{
    char c;
    int remove = 0;
    while ((c = line.at(line.size() - 1 - remove)) == '\r' || c == '\n') {
        remove++;
    }
    return line.substr(0, line.size() - remove);
}

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
    username_index = user->add_index(ColumnNames{"username"});
}

static void setup()
{
    Database::delete_all();
    import(db_dir);
}

static void reset_database()
{
    Database::delete_all();
}

//----------------------------------------------------------------------------------------------------------------------

// What is the birth date of Tweetii?

static bool q1_predicate(const Row *row)
{
    return row->at(1) == "Tweetii";
}

static void test_q1()
{
    Table *control1 = Database::new_table("control1", ColumnNames{"birth_date"});
    add(control1, {"1984/02/28"});
    Iterator* q1 =
        project(
            select(table_scan(user),
                   q1_predicate),
            {2});  
    Iterator* c1 = table_scan(control1);
    CHECK(match(c1, q1));
    delete q1;
    delete c1;
}

//----------------------------------------------------------------------------------------------------------------------

// What are the send dates of messages sent by Zyrianyhippy?

static bool q2_predicate(const Row *row)
{
    return row->at(1) == "Zyrianyhippy";
}

static void test_q2_table_scan()
{
    Table *control2 = Database::new_table("control2_table_scan", ColumnNames{"send_date"});
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
    Iterator* c2 = table_scan(control2);
    Iterator* q2 = 
    unique(sort(        
        project(
            nested_loops_join(
                nested_loops_join(select(table_scan(user),q2_predicate), 
                    {0}, table_scan(routing), {0})
            ,{4}, table_scan(message), {0})
        ,{5}),{0}));
    CHECK(match(c2, q2));
    delete q2;
    delete c2;
}

static void test_q2_index_scan()
{
    Table *control2 = Database::new_table("control2_index_scan", ColumnNames{"send_date"});
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
    Iterator* c2 = table_scan(control2);
    
    Row username({"Zyrianyhippy"});
    Row* lo = &username;
    Row* hi = NULL;
    Index* q2_index = user->add_index(ColumnNames{"username"});

    Iterator* q2 =     
    unique(sort(        
        project(
            nested_loops_join(
                nested_loops_join(index_scan(q2_index,lo,hi), 
                    {0}, table_scan(routing), {0})
            ,{4}, table_scan(message), {0})
        ,{5}),{0}));

     // Use an index_scan on username_index.
    CHECK(match(c2, q2));
    delete q2;
    delete c2;
}

//----------------------------------------------------------------------------------------------------------------------

// What are the usernames of members who received messages on their birthdays?

static bool q3_predicate(const Row *row)
{
    // Date format is yyyy/mm/dd. substr(5, 5) extracts mm/dd.
    return row->at(2).substr(5, 5) == row->at(5).substr(5, 5);
}

static void test_q3()
{
    Table *control3 = Database::new_table("control3", ColumnNames{"username"});
    add(control3, {"Moneyocracy"});
    Iterator *q3 =  
    unique(        
        project(
            select(             
                nested_loops_join(
                    nested_loops_join(table_scan(user), 
                        {0}, table_scan(routing), {1})
                ,{4}, table_scan(message), {0}), q3_predicate)
        ,{1}));
    // print_iterator("iterator", q3);
    Iterator* c3 = table_scan(control3);
    CHECK(match(c3, q3));
    delete q3;
    delete c3;
}

//----------------------------------------------------------------------------------------------------------------------

// What are the send dates of messages from Unguiferous to Froglet?
static bool q4_predicate(const Row *row)
{
    // Date format is yyyy/mm/dd. substr(5, 5) extracts mm/dd.
    return row->at(1) == "Froglet";
}

static void test_q4()
{
    Table *control4 = Database::new_table("control4", ColumnNames{"send_date"});
    add(control4, {"2016/12/14"});
    Row unguiferous({"Unguiferous"});
    Row froglet({"Froglet"});

    Row* lo1 = &unguiferous;
    Row* hi1 = NULL;
    Row* lo2 = &froglet;
    Row* hi2 = NULL;
    Index* q4_index = user->add_index(ColumnNames{"username"});

    Iterator *q4 = 
    project(
    nested_loops_join(
    nested_loops_join(
    index_scan(q4_index,lo2,hi2),{0},    
     project(
        unique(    
        nested_loops_join(
            index_scan(q4_index,lo1,hi1),{0},
                table_scan(routing),{0}))
                                        ,{3,4})
     ,{0}),
    {3},table_scan(message),{0})
    ,{4});

    Iterator* c4 = table_scan(control4);
    CHECK(match(c4, q4));
    delete q4;
    delete c4;
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
    ADD_TEST(test_q1);
    ADD_TEST(test_q2_table_scan);
    ADD_TEST(test_q2_index_scan);
    ADD_TEST(test_q3);
    ADD_TEST(test_q4);
    RUN_TESTS();
    free(db_dir);
}

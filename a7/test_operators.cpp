#include <iostream>
#include <fstream>
#include <cassert>
#include "Database.h"
#include "unittest.h"
#include "util.h"
using namespace std;

// Use this macro to make sure that iterators can be reused
#define TWICE for (int __i = 0; __i < 2; __i++)

// ------------------------------------------------------------------------------------------

// Setup

static void cleanup()
{
    Database::delete_all();
}

//----------------------------------------------------------------------------------------------------------------------

// table_scan

void table_scan_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a"});
    Iterator* i = table_scan(t);
    CHECK(i->n_columns() == 1);
    TWICE {
        i->open();
        Row* row = i->next();
        CHECK(row == NULL);
        row = i->next();
        CHECK(row == NULL);
        i->close();
    };
    delete i;
}

void table_scan_no_next()
{
    Table* t = Database::new_table("t", ColumnNames{"a"});
    Iterator* i = table_scan(t);
    CHECK(i->n_columns() == 1);
    TWICE {
        i->open();
        i->close();
    };
    delete i;
}

void table_scan_non_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    add(t, {"1", "2"});
    add(t, {"3", "4"});
    Iterator* i = table_scan(t);
    CHECK(i->n_columns() == 2);
    
    TWICE {
        Row* row;
        i->open();
        row = i->next();
        CHECK(row->at(0) == "1");
        CHECK(row->at(1) == "2");
        row = i->next();
        CHECK(row->at(0) == "3");
        CHECK(row->at(1) == "4");
        row = i->next();
        CHECK(row == NULL);
        i->close();
    };
    delete i;
}

//----------------------------------------------------------------------------------------------------------------------

// index_scan

void index_scan_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    Index* tc = t->add_index(ColumnNames{"c"});
    TestRow x(t, {"10"});
    Iterator* i = index_scan(tc, &x);
    CHECK(i->n_columns() == 3);
    TWICE {
        i->open();
        Row* row = i->next();
        CHECK(row == NULL);
        row = i->next();
        CHECK(row == NULL);
        i->close();
    };
    delete i;
}

void index_scan_no_next()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    Index* tc = t->add_index(ColumnNames{"c"});
    TestRow x(t, {"10"});
    Iterator* i = index_scan(tc, &x);
    CHECK(i->n_columns() == 3);
    TWICE {
        i->open();
        i->close();
    };
    delete i;
}

void index_scan_non_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    add(t, {"a", "b", "30"});
    add(t, {"c", "d", "20"});
    add(t, {"e", "f", "10"});
    add(t, {"g", "h", "40"});
    Index* tc = t->add_index(ColumnNames{"c"});
    TestRow lo(t, {"15"});
    TestRow hi(t, {"35"});
    Iterator* i = index_scan(tc, &lo, &hi);
    Table* control = Database::new_table("control", ColumnNames{"a", "b", "c"});
    add(control, {"c", "d", "20"});
    add(control, {"a", "b", "30"});
    Iterator* control_iterator = table_scan(control);
    CHECK(i->n_columns() == 3);
    TWICE {
        CHECK(match(control_iterator, i));
    };
    delete i;
    delete control_iterator;
}

//----------------------------------------------------------------------------------------------------------------------

// select

bool c_between_15_and_35(const Row* row)
{
    const string& c = row->at(2);
    return c >= "15" and c <= "35";
}

void select_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    Iterator* i = select(table_scan(t), c_between_15_and_35);
    CHECK(i->n_columns() == 3);
    TWICE {
        i->open();
        Row* row = i->next();
        CHECK(row == NULL);
        row = i->next();
        CHECK(row == NULL);
        i->close();
    };
    delete i;
}

void select_no_next()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    Iterator* i = select(table_scan(t), c_between_15_and_35);
    CHECK(i->n_columns() == 3);
    TWICE {
        i->open();
        i->close();
    };
    delete i;
}

void select_non_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    add(t, {"a", "b", "30"});
    add(t, {"c", "d", "20"});
    add(t, {"e", "f", "10"});
    add(t, {"g", "h", "40"});
    Iterator* i = select(table_scan(t), c_between_15_and_35);
    Table* control = Database::new_table("control", ColumnNames{"a", "b", "c"});
    add(control, {"a", "b", "30"});
    add(control, {"c", "d", "20"});
    Iterator* control_iterator = table_scan(control);
    CHECK(i->n_columns() == 3);
    TWICE {
        CHECK(match(control_iterator, i));
    };
    delete i;
    delete control_iterator;
}

//----------------------------------------------------------------------------------------------------------------------

// project

void project_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    Iterator* i = project(table_scan(t), {2, 0});
    CHECK(i->n_columns() == 2);
    TWICE {
        i->open();
        Row* row = i->next();
        CHECK(row == NULL);
        row = i->next();
        CHECK(row == NULL);
        i->close();
    };
    delete i;
}

void project_no_next()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    Iterator* i = project(table_scan(t), {2, 0});
    CHECK(i->n_columns() == 2);
    TWICE {
        i->open();
        i->close();
    };
    delete i;
}

void project_non_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    add(t, {"a", "b", "30"});
    add(t, {"c", "d", "20"});
    add(t, {"e", "f", "10"});
    add(t, {"g", "h", "40"});
    add(t, {"c", "x", "20"});
    add(t, {"e", "y", "10"});
    Iterator* i = project(table_scan(t), {2, 0});
    Table* control = Database::new_table("control", ColumnNames{"c", "a"});
    add(control, {"30", "a"});
    add(control, {"20", "c"});
    add(control, {"10", "e"});
    add(control, {"40", "g"});
    add(control, {"20", "c"});
    add(control, {"10", "e"});
    Iterator* control_iterator = table_scan(control);
    CHECK(i->n_columns() == 2);
    TWICE {
        CHECK(match(control_iterator, i));
    };
    delete i;
    delete control_iterator;
}

//----------------------------------------------------------------------------------------------------------------------

// nested_loops_join

void nested_loops_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b", "c"});
    Table* s = Database::new_table("s", ColumnNames{"c", "d", "e"});
    Iterator* i = nested_loops_join(table_scan(r), {2}, table_scan(s), {0});
    CHECK(i->n_columns() == 5);
    TWICE {
        i->open();
        Row* row = i->next();
        CHECK(row == NULL);
        row = i->next();
        CHECK(row == NULL);
        i->close();
    };
    delete i;
}

void nested_loops_no_next()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b", "c"});
    Table* s = Database::new_table("s", ColumnNames{"c", "d", "e"});
    Iterator* i = nested_loops_join(table_scan(r), {2}, table_scan(s), {0});
    CHECK(i->n_columns() == 5);
    TWICE {
        i->open();
        i->close();
    };
    delete i;
}

void nested_loops_left_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b", "c"});
    Table* s = Database::new_table("s", ColumnNames{"c", "d", "e"});
    add(s, {"a", "12", "1"});
    add(s, {"a", "12", "2"});
    add(s, {"c", "56", "1"});
    add(s, {"c", "56", "2"});
    add(s, {"c", "56", "3"});
    add(s, {"d", "--", "-"});
    Iterator* i = nested_loops_join(table_scan(r), {2}, table_scan(s), {0});
    Table* control = Database::new_table("control", {"a", "b", "c", "d", "e"});
    Iterator* control_iterator = table_scan(control);
    CHECK(i->n_columns() == 5);
    TWICE {
        CHECK(match(control_iterator, i));
    };
    delete i;
    delete control_iterator;
}

void nested_loops_right_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b", "c"});
    add(r, {"1", "2", "a"});
    add(r, {"3", "4", "b"});
    add(r, {"5", "6", "c"});
    Table* s = Database::new_table("s", ColumnNames{"c", "d", "e"});
    Iterator* i = nested_loops_join(table_scan(r), {2}, table_scan(s), {0});
    Table* control = Database::new_table("control", {"a", "b", "c", "d", "e"});
    Iterator* control_iterator = table_scan(control);
    CHECK(i->n_columns() == 5);
    TWICE {
        CHECK(match(control_iterator, i));
    };
    delete i;
    delete control_iterator;
}

void nested_loops_both_non_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b", "c"});
    add(r, {"1", "2", "a"});
    add(r, {"3", "4", "b"});
    add(r, {"5", "6", "c"});
    Table* s = Database::new_table("s", ColumnNames{"c", "d", "e"});
    add(s, {"a", "12", "1"});
    add(s, {"a", "12", "2"});
    add(s, {"c", "56", "1"});
    add(s, {"c", "56", "2"});
    add(s, {"c", "56", "3"});
    add(s, {"d", "--", "-"});
    Iterator* i = nested_loops_join(table_scan(r), {2}, table_scan(s), {0});
    Table* control = Database::new_table("control", {"a", "b", "c", "d", "e"});
    add(control, {"1", "2", "a", "12", "1"});
    add(control, {"1", "2", "a", "12", "2"});
    add(control, {"5", "6", "c", "56", "1"});
    add(control, {"5", "6", "c", "56", "2"});
    add(control, {"5", "6", "c", "56", "3"});
    Iterator* control_iterator = table_scan(control);
    CHECK(i->n_columns() == 5);
    TWICE {
        CHECK(match(control_iterator, i));
    };
    delete i;
    delete control_iterator;
}

//----------------------------------------------------------------------------------------------------------------------

// sort

void sort_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    Iterator* i = sort(table_scan(t), {1, 2, 0});
    CHECK(i->n_columns() == 3);
    TWICE {
        i->open();
        Row* row = i->next();
        CHECK(row == NULL);
        row = i->next();
        CHECK(row == NULL);
        i->close();
    };
    delete i;
}

void sort_no_next()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    Iterator* i = sort(table_scan(t), {1, 2, 0});
    CHECK(i->n_columns() == 3);
    TWICE {
        i->open();
        i->close();
    };
    delete i;
}

void sort_non_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b", "c"});
    add(t, {"a", "b", "30"});
    add(t, {"c", "d", "20"});
    add(t, {"e", "f", "10"});
    add(t, {"g", "h", "40"});
    add(t, {"a", "b", "31"});
    add(t, {"c", "d", "21"});
    add(t, {"e", "f", "11"});
    add(t, {"g", "h", "41"});
    Iterator* i = sort(table_scan(t), {1, 2, 0});
    Table* control = Database::new_table("control", ColumnNames{"a", "b", "c"});
    add(control, {"a", "b", "30"});
    add(control, {"a", "b", "31"});
    add(control, {"c", "d", "20"});
    add(control, {"c", "d", "21"});
    add(control, {"e", "f", "10"});
    add(control, {"e", "f", "11"});
    add(control, {"g", "h", "40"});
    add(control, {"g", "h", "41"});
    Iterator* control_iterator = table_scan(control);
    CHECK(i->n_columns() == 3);
    TWICE {
        CHECK(match(control_iterator, i));
    };
    delete i;
    delete control_iterator;
}

//----------------------------------------------------------------------------------------------------------------------

// unique

void unique_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});

    Iterator* i = unique(table_scan(t));
    CHECK(i->n_columns() == 2);
    TWICE {
        i->open();
        Row* row = i->next();
        CHECK(row == NULL);
        row = i->next();
        CHECK(row == NULL);
        i->close();
    };
    delete i;
}

void unique_no_next()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    Iterator* i = unique(table_scan(t));
    CHECK(i->n_columns() == 2);
    TWICE {
        i->open();
        i->close();
    };
    delete i;
}

void unique_non_empty()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    add(t, {"1", "10"});
    add(t, {"2", "20"});
    add(t, {"2", "20"});
    add(t, {"1", "10"});
    add(t, {"1", "10"});
    add(t, {"1", "10"});
    add(t, {"3", "30"});
    Iterator* i = unique(table_scan(t));
    // print_iterator("unique non empty", i);
    Table* control = Database::new_table("control", ColumnNames{"a", "b"});
    add(control, {"1", "10"});
    add(control, {"2", "20"});
    add(control, {"1", "10"});
    add(control, {"3", "30"});
    Iterator* control_iterator = table_scan(control);
    // print_iterator("unique non empty", i);
    CHECK(i->n_columns() == 2);
    TWICE {
        CHECK(match(control_iterator, i));
    };
    delete i;
    delete control_iterator;
}

//----------------------------------------------------------------------------------------------------------------------

void test_operators(int argc, const char **argv)
{
    AFTER_TEST(cleanup);
    ADD_TEST(table_scan_empty);
    ADD_TEST(table_scan_no_next);
    ADD_TEST(table_scan_non_empty);
    ADD_TEST(index_scan_empty);
    ADD_TEST(index_scan_no_next);
    ADD_TEST(index_scan_non_empty);
    ADD_TEST(select_empty);
    ADD_TEST(select_no_next);
    ADD_TEST(select_non_empty);
    ADD_TEST(project_empty);
    ADD_TEST(project_no_next);
    ADD_TEST(project_non_empty);
    ADD_TEST(nested_loops_empty);
    ADD_TEST(nested_loops_no_next);
    ADD_TEST(nested_loops_left_empty);
    ADD_TEST(nested_loops_right_empty);
    ADD_TEST(nested_loops_both_non_empty);
    ADD_TEST(sort_empty);
    ADD_TEST(sort_no_next);
    ADD_TEST(sort_non_empty);
    ADD_TEST(unique_empty);
    ADD_TEST(unique_no_next);
    ADD_TEST(unique_non_empty);
    RUN_TESTS();
}

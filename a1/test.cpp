#include "Database.h"
#include "util.h"
#include "unittest.h"

using namespace std;

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

// ------------------------------------------------------------------------------------------

// TESTS

// Table creation

static void create_table_no_columns()
{
    try {
        Database::new_table("t", ColumnNames());
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void create_table_duplicate_columns()
{
    try {
        Database::new_table("t", ColumnNames{"a", "a"});
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void create_table_duplicate_names()
{
    Database::new_table("t", ColumnNames{"a", "b"});
    try {
        Database::new_table("t", ColumnNames{"a", "b"});
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void create_table_ok()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    assert(t->name() == "t");
    assert(t->columns() == ColumnNames{"a", "b"});

}

// Row addition

static void add_row_with_too_few_columns()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    try {
        add(t, {});
        fail();
    } catch (TableException& e) {
        // expected
    }
    try {
        add(t, {"1"});
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void add_row_with_too_many_columns()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    try {
        add(t, {"1", "2", "3"});
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void add_row_ok()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    assert(t->rows().empty());
    assert(add(t, {"1", "2"}));
    assert(t->rows().size() == 1);

    int count = 0;
    for (Row* row : t->rows()) {
        assert(row->value("a") == "1");
        assert(row->value("b") == "2");
        count++;
    }
    assert(count == 1);
    assert(add(t, {"3", "4"}));
    assert(t->rows().size() == 2);
    count = 0;
    bool saw12 = false;
    bool saw34 = false;
    for (Row* row : t->rows()) {
        saw12 = saw12 || (row->value("a") == "1" && row->value("b") == "2");
        saw34 = saw34 || (row->value("a") == "3" && row->value("b") == "4");
        count++;
    }
    assert(saw12);
    assert(saw34);
    assert(count == 2);
}

static void add_row_suppress_duplicates()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    assert(add(t, {"e", "f"}));
    assert(add(t, {"x", "y"}));
    assert(!add(t, {"e", "f"}));
    assert(add(t, {"e", "r"}));
    assert(add(t, {"s", "f"}));
    int ef = 0;
    int xy = 0;
    int er = 0;
    int sf = 0;
    int count = 0;
    for (Row* row : t->rows()) {
        if (row->value("a") == "e" && row->value("b") == "f") ef++;
        if (row->value("a") == "x" && row->value("b") == "y") xy++;
        if (row->value("a") == "e" && row->value("b") == "r") er++;
        if (row->value("a") == "s" && row->value("b") == "f") sf++;
        count++;
    }
    assert(count == 4);
    assert(ef == 1);
    assert(xy == 1);
    assert(er == 1);
    assert(sf == 1);
}

static void remove_from_empty_table()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    assert(!remove(t, {"1", "2"}));
}

static void remove_row_missing()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    assert(add(t, {"x", "y"}));
    assert(!remove(t, {"xx", "yy"}));
    int xy = 0;
    int count = 0;
    for (Row* row : t->rows()) {
        if (row->value("a") == "x" && row->value("b") == "y") {
            xy++;
        }
        count++;
    }
    assert(count == 1);
    assert(xy == 1);
}

static void remove_row_present()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    assert(add(t, {"1", "2"}));
    assert(add(t, {"3", "4"}));
    assert(add(t, {"5", "6"}));
    assert(add(t, {"7", "8"}));
    assert(!remove(t, {"1", "1"}));
    assert(remove(t, {"3", "4"}));
    assert(!remove(t, {"3", "4"}));
    assert(!remove(t, {"8", "7"}));
    assert(remove(t, {"7", "8"}));
    assert(!remove(t, {"2", "2"}));
    int r12 = 0;
    int r34 = 0;
    int r56 = 0;
    int r78 = 0;
    int count = 0;
    for (Row* row : t->rows()) {
        if (row->value("a") == "1" && row->value("b") == "2") {
            r12++;
        }
        if (row->value("a") == "3" && row->value("b") == "4") {
            r34++;
        }
        if (row->value("a") == "5" && row->value("b") == "6") {
            r56++;
        }
        if (row->value("a") == "7" && row->value("b") == "8") {
            r78++;
        }
        count++;
    }
    assert(count == 2);
    assert(r12 == 1);
    assert(r34 == 0);
    assert(r56 == 1);
    assert(r78 == 0);
}

static void remove_incompatible_row()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    assert(add(t, {"1", "2"}));
    assert(add(t, {"3", "4"}));
    assert(add(t, {"5", "6"}));
    assert(add(t, {"7", "8"}));
    try {
        remove(t, {"1"});
        fail();
    } catch (TableException& e) {
        // expected
    }
    try {
        remove(t, {"1", "2", "3"});
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void has_bad_row()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    assert(add(t, {"1", "2"}));
    assert(add(t, {"3", "4"}));
    assert(add(t, {"5", "6"}));
    assert(add(t, {"7", "8"}));
    try {
        has(t, {"1"});
        fail();
    } catch (TableException& e) {
        // expected
    }
    try {
        has(t, {"1", "2", "3"});
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void has_row_ok()
{
    Table* t = Database::new_table("t", ColumnNames{"a", "b"});
    assert(!has(t, {"1", "2"}));
    assert(add(t, {"1", "2"}));
    assert(add(t, {"3", "4"}));
    assert(add(t, {"5", "6"}));
    assert(add(t, {"7", "8"}));
    assert(has(t, {"1", "2"}));
    assert(has(t, {"3", "4"}));
    assert(has(t, {"5", "6"}));
    assert(has(t, {"7", "8"}));
    assert(!has(t, {"8", "7"}));
    assert(!has(t, {"5", "5"}));
}

void test_tables_and_rows(int argc, const char** argv)
{
    AFTER_TEST(reset_database);
    ADD_TEST(create_table_no_columns);
    ADD_TEST(create_table_duplicate_columns);
    ADD_TEST(create_table_duplicate_names);
    ADD_TEST(create_table_ok);
    ADD_TEST(add_row_with_too_few_columns);
    ADD_TEST(add_row_with_too_many_columns);
    ADD_TEST(add_row_ok);
    ADD_TEST(add_row_suppress_duplicates);
    ADD_TEST(remove_from_empty_table);
    ADD_TEST(remove_row_missing);
    ADD_TEST(remove_row_present);
    ADD_TEST(remove_incompatible_row);
    ADD_TEST(has_bad_row);
    ADD_TEST(has_row_ok);
    RUN_TESTS();
}

#include "Database.h"
#include "util.h"
#include "unittest.h"
#include "RelationalAlgebra.h"

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

// ------------------------------------------------------------------------------------------

// predicates used by select

static bool false_for_all(Row* row)
{
    return false;
}

static bool true_for_all(Row* row)
{
    return true;
}

static bool ra_eq_1(Row* row)
{
    return row->value("a") == "1";
}

// Union

static void union_incompatible()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b", "c"});
    try {
        onion(r, s);
        fail();
    } catch (UnionIncompatibilityException& e) {
        // expected
    }
}

static void union_incompatible_different_names()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"b", "a"});
    try {
        onion(r, s);
    } catch (UnionIncompatibilityException& e) {
        fail();
    }
}
static void union_compatible_both_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    Table* t = onion(r, s);
    assert(t->rows().empty());
}

static void union_compatible_one_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    Table* t = onion(r, s);
    assert(table_eq(r, t));
}

static void union_compatible_both_non_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "44"});
    add(r, {"1", "55"});
    add(r, {"2", "66"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    add(s, {"1", "44"});
    add(s, {"3", "77"});
    Table* control = Database::new_table("control", ColumnNames{"a", "b"});
    add(control, {"1", "44"});
    add(control, {"1", "55"});
    add(control, {"2", "66"});
    add(control, {"3", "77"});
    assert(table_eq(control, onion(r, s)));
}

// Intersect

static void intersect_incompatible()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b", "c"});
    try {
        intersect(r, s);
        fail();
    } catch (UnionIncompatibilityException& e) {
        // expected
    }
}

static void intersect_compatible_both_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    assert(table_eq(r, intersect(r, s)));
}

static void intersect_compatible_one_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "44"});
    add(r, {"1", "55"});
    add(r, {"2", "66"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    assert(table_eq(s, intersect(r, s)));
}

static void intersect_compatible_both_non_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "44"});
    add(r, {"1", "55"});
    add(r, {"2", "66"});
    add(r, {"2", "77"});
    add(r, {"3", "88"});
    add(r, {"3", "99"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    add(s, {"1", "44"});
    add(s, {"1", "555"});
    add(s, {"2", "33"});
    add(s, {"2", "77"});
    add(s, {"3", "77"});
    add(s, {"3", "88"});
    add(s, {"3", "99"});
    Table* control = Database::new_table("control", ColumnNames{"a", "b"});
    add(control, {"1", "44"});
    add(control, {"2", "77"});
    add(control, {"3", "88"});
    add(control, {"3", "99"});
    assert(table_eq(control, intersect(r, s)));
}

// diff

static void diff_incompatible()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b", "c"});
    try {
        diff(r, s);
        fail();
    } catch (UnionIncompatibilityException& e) {
        // expected
    }
}

static void diff_compatible_both_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    assert(table_eq(r, diff(r, s)));
}

static void diff_compatible_one_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "44"});
    add(r, {"1", "55"});
    add(r, {"2", "66"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    assert(table_eq(r, diff(r, s)));
}

static void diff_compatible_other_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    add(s, {"1", "44"});
    add(s, {"1", "55"});
    add(s, {"2", "66"});
    assert(table_eq(r, diff(r, s)));
}

static void diff_compatible_both_non_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "44"});
    add(r, {"1", "55"});
    add(r, {"2", "66"});
    Table* s = Database::new_table("s", ColumnNames{"a", "b"});
    add(s, {"1", "44"});
    add(s, {"3", "77"});
    Table* control = Database::new_table("control", ColumnNames{"a", "b"});
    add(control, {"1", "55"});
    add(control, {"2", "66"});
    assert(table_eq(control, diff(r, s)));
}

static void product_duplicate_columns()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"b", "c"});
    try {
        product(r, s);
        fail();
    } catch (TableException &exception1) {
        // expected
    }
}

static void product_both_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"c", "d"});
    assert(product(r, s)->rows().empty());
}

static void product_one_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "44"});
    Table* s = Database::new_table("s", ColumnNames{"c", "d"});
    assert(product(r, s)->rows().empty());
}

static void product_non_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "44"});
    add(r, {"2", "55"});
    Table* s = Database::new_table("s", ColumnNames{"c", "d"});
    add(s, {"3", "66"});
    add(s, {"4", "77"});
    Table* control = Database::new_table("control", ColumnNames{"a", "b", "c", "d"});
    add(control, {"1", "44", "3", "66"});
    add(control, {"1", "44", "4", "77"});
    add(control, {"2", "55", "3", "66"});
    add(control, {"2", "55", "4", "77"});
    assert(table_eq(control, product(r, s)));
}

// rename

static void rename_wrong()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    try {
        rename(r, NameMap({{"a", "x"}, {"c", "y"}}));
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void rename_too_many()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    try {
        rename(r, NameMap({{"a", "x"}, {"b", "y"}, {"c", "z"}}));
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void rename_ok()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "2"});
    add(r, {"3", "4"});
    Table* t = rename(r, NameMap({{"a", "x"}, {"b", "y"}}));
    Table* control = Database::new_table("control", ColumnNames{"x", "y"});
    add(control, {"1", "2"});
    add(control, {"3", "4"});
    assert(t->columns() == control->columns());
    assert(table_eq(control, t));
}

static void rename_nothing()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "2"});
    add(r, {"3", "4"});
    Table* t = rename(r, NameMap());
    assert(t->columns() == r->columns());
    assert(table_eq(r, t));
}

static void rename_some()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "2"});
    add(r, {"3", "4"});
    Table* t = rename(r, NameMap({{"a", "x"}}));
    Table* control = Database::new_table("control", ColumnNames{"x", "b"});
    add(control, {"1", "2"});
    add(control, {"3", "4"});
    assert(t->columns() == control->columns());
    assert(table_eq(control, t));
}

static void select_nothing()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "2"});
    add(r, {"3", "4"});
    assert(select(r, false_for_all)->rows().empty());
}

// select

static void select_all()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "2"});
    add(r, {"3", "4"});
    assert(table_eq(r, select(r, true_for_all)));
}

static void select_some()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "2"});
    add(r, {"3", "4"});
    Table* control = Database::new_table("control", ColumnNames{"a", "b"});
    add(control, {"1", "2"});
    assert(table_eq(control, select(r, ra_eq_1)));
}

// project

static void project_no_columns()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    try {
        project(r, ColumnNames{});
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void project_unknown_column()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    try {
        project(r, ColumnNames{"b", "c"});
        fail();
    } catch (TableException& e) {
        // expected
    }
}

static void project_no_duplicates()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b", "c"});
    add(r, {"1", "2", "3"});
    add(r, {"4", "5", "6"});
    add(r, {"7", "8", "9"});
    Table* control = Database::new_table("control", ColumnNames{"a", "b"});
    add(control, {"1", "2"});
    add(control, {"4", "5"});
    add(control, {"7", "8"});
    assert(table_eq(control, project(r, ColumnNames{"a", "b"})));
}

static void project_eliminate_duplicates()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b", "c"});
    add(r, {"1", "2", "3"});
    add(r, {"1", "2", "6"});
    add(r, {"1", "2", "9"});
    Table* control = Database::new_table("control", ColumnNames{"a", "b"});
    add(control, {"1", "2"});
    assert(table_eq(control, project(r, ColumnNames{"a", "b"})));
}

// join

static void join_no_join_columns()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"c", "d"});
    try {
        join(r, s);
        fail();
    } catch (JoinException& e) {
        // expected
    }
}

static void join_both_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    Table* s = Database::new_table("s", ColumnNames{"b", "c"});
    assert(join(r, s)->rows().empty());
}

static void join_one_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"1", "2"});
    Table* s = Database::new_table("s", ColumnNames{"b", "c"});
    assert(join(r, s)->rows().empty());
}

static void join_both_non_empty()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"10", "1"});
    add(r, {"11", "1"});
    add(r, {"0", "2"});
    add(r, {"10", "3"});
    add(r, {"11", "3"});
    Table* s = Database::new_table("s", ColumnNames{"b", "c"});
    add(s, {"1", "100"});
    add(s, {"1", "101"});
    add(s, {"3", "200"});
    add(s, {"3", "201"});
    add(s, {"3", "202"});
    add(s, {"4", "0"});
    Table* control = Database::new_table("control", ColumnNames{"a", "b", "c"});
    add(control, {"10", "1", "100"});
    add(control, {"10", "1", "101"});
    add(control, {"11", "1", "100"});
    add(control, {"11", "1", "101"});
    add(control, {"10", "3", "200"});
    add(control, {"10", "3", "201"});
    add(control, {"10", "3", "202"});
    add(control, {"11", "3", "200"});
    add(control, {"11", "3", "201"});
    add(control, {"11", "3", "202"});
    assert(table_eq(control, join(r, s)));
}

static void join_both_non_empty_2()
{
    Table* r = Database::new_table("r", ColumnNames{"a", "b"});
    add(r, {"10", "1"});
    add(r, {"10", "3"});
    add(r, {"11", "1"});
    add(r, {"11", "3"});
    add(r, {"0", "2"});
    Table* s = Database::new_table("s", ColumnNames{"a", "c"});
    add(s, {"10", "100"});
    add(s, {"10", "101"});
    add(s, {"10", "200"});
    add(s, {"11", "201"});
    Table* control = Database::new_table("control", ColumnNames{"a", "b", "c"});
    add(control, {"10", "1", "100"});
    add(control, {"10", "1", "101"});
    add(control, {"10", "1", "200"});
    add(control, {"10", "3", "100"});
    add(control, {"10", "3", "101"});
    add(control, {"10", "3", "200"});
    add(control, {"11", "1", "201"});
    add(control, {"11", "3", "201"});
    assert(table_eq(control, join(r, s)));
}

void test_relational_algebra(int argc, const char** argv)
{
    AFTER_TEST(reset_database);
    ADD_TEST(union_incompatible);
    ADD_TEST(union_incompatible_different_names);
    ADD_TEST(union_compatible_both_empty);
    ADD_TEST(union_compatible_one_empty);
    ADD_TEST(union_compatible_both_non_empty);
    ADD_TEST(intersect_incompatible);
    ADD_TEST(intersect_compatible_both_empty);
    ADD_TEST(intersect_compatible_one_empty);
    ADD_TEST(intersect_compatible_both_non_empty);
    ADD_TEST(diff_incompatible);
    ADD_TEST(diff_compatible_both_empty);
    ADD_TEST(diff_compatible_one_empty);
    ADD_TEST(diff_compatible_other_empty);
    ADD_TEST(diff_compatible_both_non_empty);
    ADD_TEST(product_duplicate_columns);
    ADD_TEST(product_both_empty);
    ADD_TEST(product_one_empty);
    ADD_TEST(product_non_empty);
    ADD_TEST(rename_nothing);
    ADD_TEST(rename_wrong);
    ADD_TEST(rename_some);
    ADD_TEST(rename_too_many);
    ADD_TEST(rename_ok);
    ADD_TEST(select_nothing);
    ADD_TEST(select_all);
    ADD_TEST(select_some);
    ADD_TEST(project_no_columns);
    ADD_TEST(project_unknown_column);
    ADD_TEST(project_no_duplicates);
    ADD_TEST(project_eliminate_duplicates);
    ADD_TEST(join_no_join_columns);
    ADD_TEST(join_both_empty);
    ADD_TEST(join_one_empty);
    ADD_TEST(join_both_non_empty);
    ADD_TEST(join_both_non_empty_2);
    RUN_TESTS();
}

#include "Database.h"

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

bool eq(Table* x, Table* y)
{
    if (x->columns() != y->columns()) {
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
        if (**i++ != **j++) {
            return false;
        }
    }
    return true;
}

void reset_database()
{
    Database::delete_all_tables();
}

void IMPLEMENT_ME()
{
    throw NotYetImplementedException();
}

Table* IMPLEMENT_THIS_QUERY()
{
    throw NotYetImplementedException();
}

NotYetImplementedException::NotYetImplementedException()
{}

const char *NotYetImplementedException::what() const throw()
{
    return "Not yet implemented!";
}

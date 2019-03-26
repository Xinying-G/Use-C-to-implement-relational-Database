#include "Database.h"
#include "util.h"

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

#include "ColumnNames.h"

int ColumnNames::position(const string &name) const
{
    unsigned long n = size();
    for (unsigned long i = 0; i < n; i++) {
        if (at(i) == name) {
            return (int) i;
        }
    }
    return -1;
}

ColumnNames::ColumnNames()
{}

ColumnNames::ColumnNames(const ColumnNames &columns)
        : vector<string>(columns)
{}

ColumnNames::ColumnNames(initializer_list<string> elements)
        : vector<string>(elements)
{}

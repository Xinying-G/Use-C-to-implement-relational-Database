#include <cassert>
#include <cstring>
#include "Database.h"

//----------------------------------------------------------------------------------

// Row

const Table *Row::table() const
{
    return _table;
}

const string &Row::value(const string &column) const
{
    assert(_table != NULL);
    const ColumnNames &columns = _table->columns();
    unsigned long n = columns.size();
    for (unsigned i = 0; i < n; i++) {
        if (columns.at(i) == column) {
            return at(i);
        }
    }
    throw RowException("Unknown column");
}

void Row::append(const string &value)
{
    emplace_back(value);
}

bool Row::is_intermediate_row() const
{
    return _table == NULL;
}

Row::Row(const Table *table)
        : _table(table)
{}

Row::Row()
        : _table(NULL)
{
}

Row::Row(const initializer_list<string>& values)
    : vector<string>(values),
      _table(NULL)
{}

Row::~Row()
{
    clear();
}

void Row::reclaim(Row* row)
{
    if (row && row->is_intermediate_row()) {
        // printf("delete row\n");
        delete row;
    }
}

//----------------------------------------------------------------------------------

// RowCompare

RowCompare RowCompare::only;

int RowCompare::operator()(Row* const &x, Row* const &y)
{
    unsigned n = (unsigned) x->size();
    for (unsigned i = 0; i < n; i++) {
        int comparison = strcmp(x->at(i).c_str(), y->at(i).c_str());
        if (comparison != 0) {
            return comparison < 0;
        }
    }
    return 0;
}

bool RowCompare::cmp(Row* const &x, Row* const &y)
{
    unsigned n = (unsigned) x->size();
    for (unsigned i = 0; i < n; i++) {
        int comparison = strcmp(x->at(i).c_str(), y->at(i).c_str());
        if (comparison < 0) {
            return true;
        }
    }
    return false;
}


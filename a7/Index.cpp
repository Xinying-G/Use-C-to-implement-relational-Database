#include "Table.h"
#include "Index.h"

void Index::put(const vector<string>& key, Row* value)
{
    insert(make_pair(key, value));
}

unsigned Index::n_columns()
{
    return _n_columns;
}

Index::Index(Table* table)
    : _n_columns((unsigned) table->columns().size())
{}

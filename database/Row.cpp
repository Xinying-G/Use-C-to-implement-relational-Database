#include "Database.h"
#include "util.h"

bool Row::operator==(const Row &that) const
{
    return (that.v == v && 
    	that.table()->name() == table()->name());
}

bool Row::operator!=(const Row& row) const
{
    return !operator==(row);
}

const Table *Row::table() const
{
    return _table;
}

const string &Row::value(const string &column) const
{
    int position = _table->columns().position(column);
    return v.at(position);
}

void Row::append(const string &value)
{
    v.push_back(value);
}

int Row::get_row_size()
{
	return v.size();
}

Row::Row(const Table *table)
        : _table(table)
{}

Row::Row(const Table *table, const Row &row):_table(table)
{
	v = row.v;
}

Row::~Row()
{
}

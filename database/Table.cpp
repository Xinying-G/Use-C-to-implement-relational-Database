#include <cstring>
#include "Database.h"

using namespace std;

const string &Table::name() const
{
    return _name;
}

const ColumnNames &Table::columns() const
{
    return _columns;
}

const RowSet& Table::rows()
{
    return _rows;
}

bool Table::add(Row* row)
{
    
    if (_rows.find(row) != _rows.end())
    {
        return false;
    }
    else if (row->get_row_size() == (int)_columns.size())
    {
        Row *new_row = new Row(row->table(),*row);
        delete row;
        _rows.insert(new_row);     
        return true;
    }

    else
    {
        throw TableException("Row cannot be add");
    }        
}

bool Table::remove(Row* row)
{
    if (row->get_row_size() != (int)_columns.size())
    {
        throw TableException("Imcompatible Row");
    }

    if(_rows.find(row) == _rows.end())
    {
        return false;
    }
    else 
    {

        set<Row*, RowCompare>::iterator it = _rows.find(row);
        Row *new_row = *it;
        if (it!= _rows.end())
        {
            _rows.erase(*it);
            delete (new_row);
        }        
        return true;
    }
}

bool Table::has(Row* row)
{
    if (row->get_row_size() != (int)_columns.size())
        throw TableException("Have bad row");
    return _rows.find(row) != _rows.end();
}

Table::Table(const string &name, const ColumnNames &columns)
    : _name(name),
      _columns(columns)
{
    if(_columns.empty())
        throw TableException("ColumnNames is empty");

    set<string> Y(_columns.begin(),_columns.end());

    if(_columns.size() != Y.size())
        throw TableException("ColumnNames is duplicated");
}

Table::~Table()
{ 
    for(set<Row*, RowCompare>::iterator it = _rows.begin(); it != _rows.end(); it++)
            delete *it;
}

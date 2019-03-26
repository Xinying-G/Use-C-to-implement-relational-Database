#ifndef TABLE_H
#define TABLE_H

#include <memory>
#include <set>
#include "Row.h"
#include "ColumnNames.h"

using namespace std;

class Index;

class Table
{
public:
    // The name of this Table
    const string &name() const;

    // The columns of this Table
    const ColumnNames &columns() const;

    // The contents of this Table
    RowList& rows();

    // Add the given row to the table, returning true if the row was added, false if not (because a matching row
    // is already present). Following a successful add (i.e., returning true), the row is owned by the table, and
    // must not be modified or deleted by the caller. Otherwise, it is the caller's responsibility to delete the row
    // eventually.
    void add(Row* row);

    Index* add_index(const ColumnNames& index_columns);

    // Create a table with the given name and column names
    Table(const string& name, const ColumnNames& columns);

    // Destroy this table
    ~Table();

private:
    string _name;
    ColumnNames _columns;
    RowList _rows;
    vector<Index*> _indexes;
};


#endif //TABLE_H

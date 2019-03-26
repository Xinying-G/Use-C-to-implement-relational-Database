#include <cstring>
#include <cassert>
#include "Table.h"
#include "Index.h"
#include "Row.h"
#include "dbexceptions.h"

using namespace std;

const string &Table::name() const
{
    return _name;
}

const ColumnNames &Table::columns() const
{
    return _columns;
}

RowList& Table::rows()
{
    return _rows;
}

void Table::add(Row* row)
{
    const ColumnNames& source_columns = row->table()->columns();
    const ColumnNames& target_columns = _columns;
    if (row->size() != _columns.size()) {
        throw TableException("row size is wrong");
    }
    if (source_columns.size() != target_columns.size()) {
        throw TableException("source and target metadata incompatible");
    }
    _rows.emplace_back(row);
}

Index* Table::add_index(const ColumnNames& index_columns)
{
    Index* index = new Index(this);
    unsigned n_key_columns = (unsigned) index_columns.size();
    unsigned key_positions[n_key_columns];
    unsigned k = 0;
    for (const string& column : index_columns) {
        int position = _columns.position(column);
        assert(position != -1);
        key_positions[k++] = (unsigned) position;
    }
    vector<string> key;
    for (Row* row : _rows) {
        key.clear();
        for (unsigned i = 0; i < n_key_columns; i++) {
            key.emplace_back(row->at(key_positions[i]));
        }
        index->put(key, row);
    }
    _indexes.emplace_back(index);
    return index;
}

Table::Table(const string &name, const ColumnNames &columns)
    : _name(name),
      _columns(columns)
{
    if (columns.empty()) {
        throw TableException("No columns");
    }
    auto n = columns.size();
    for (unsigned long i = 0; i < n; i++) {
        for (unsigned long j = i + 1; j < n; j++) {
            if (columns.at(i) == columns.at(j)) {
                throw TableException("Duplicate columns");
            }
        }
    }
}

Table::~Table()
{
    for (Index* index : _indexes) {
        delete index;
    }
    auto i = _rows.begin();
    while (i != _rows.end()) {
        delete *i;
        i = _rows.erase(i);
    }
    _rows.clear();
}

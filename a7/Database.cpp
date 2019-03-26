#include <sstream>
#include "Database.h"

unordered_map<string, Table*> Database::_tables;

Table* Database::new_table(const string &name, const ColumnNames &columns)
{
    if (_tables.find(name) != _tables.end()) {
        throw TableException("Table name already in use");
    }
    auto table = new Table(name, columns);
    _tables.insert({{name, table}});
    return table;
}

void Database::delete_all()
{
    auto i = _tables.begin();
    while (i != _tables.end()) {
        delete i++->second;
    }
    _tables.clear();
}

#include <sstream>
#include "Database.h"

unordered_map<string, Table*> Database::_tables;
unsigned Database::_table_name_counter = 0;

Table* Database::new_table(const string &name, const ColumnNames &columns)
{
    if (_tables.find(name) != _tables.end()) {
        throw TableException("Table name already in use");
    }
    auto table = new Table(name, columns);
    _tables.insert({{name, table}});
    return table;
}

string Database::new_table_name()
{
    string name;
    do {
        ostringstream stream;
        stream << "t_" << to_string(_table_name_counter++);
        name = stream.str();
    } while (_tables.find(name) != _tables.end());
    return name;

}

void Database::delete_all_tables()
{
    auto i = _tables.begin();
    while (i != _tables.end()) {
        delete i++->second;
    }
    _tables.clear();
}

#ifndef RA_C_DATABASE_H
#define RA_C_DATABASE_H

#include "Table.h"
#include "Row.h"
#include "RelationalAlgebra.h"
#include "dbexceptions.h"

class Database
{
public:
    // Returns a new, empty table, with the given name, and column names.
    static Table* new_table(const string &name, const ColumnNames &columns);

    // Delete all tables and rows, resulting an an empty database.
    static void delete_all_tables();

    static string new_table_name();

private:
    static unordered_map<string, Table*> _tables;
    static unsigned _table_name_counter;
};


#endif //RA_C_DATABASE_H

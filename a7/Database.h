#ifndef DATABASE_H
#define DATABASE_H

#include <unordered_map>
#include "Table.h"
#include "Index.h"
#include "Iterator.h"
#include "Row.h"
#include "ColumnNames.h"
#include "ColumnSelector.h"
#include "QueryProcessor.h"
#include "dbexceptions.h"

class Iterator;

class Database
{
public:
    // Returns a new, empty table, with the given name, and column names.
    static Table* new_table(const string &name, const ColumnNames &columns);

    // Delete all tables and rows, resulting an an empty database.
    static void delete_all();

private:
    static unordered_map<string, Table*> _tables;
};


#endif //DATABASE_H

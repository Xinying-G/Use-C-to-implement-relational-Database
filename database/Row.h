#ifndef RA_C_ROW_H
#define RA_C_ROW_H

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Table;

class Row
{
public:
    bool operator==(const Row &) const;

    bool operator!=(const Row &row) const;

    // This Row's table
    const Table *table() const;

    // The value for the given column in this Row
    const string &value(const string &column) const;

    const string &at(unsigned long);

    // Append a value to this Row
    void append(const string& value);

    int get_row_size();

    // Create a Row for the given Table
    Row(const Table *table);

    Row(const Table *table, const Row &);

    // Destroy this Row
    ~Row();

public:
    const Table *_table;
    vector<string> v;

};

#endif //RA_C_ROW_H

#ifndef ROW_H
#define ROW_H

#include <string>
#include <vector>

using namespace std;

class Table;

class Row: public vector<string>
{
public:
    // This Row's table
    const Table *table() const;

    // The value for the given column in this Row
    const string &value(const string &column) const;

    // Append a value to this Row
    void append(const string& value);

    // Create a Row for the given Table
    Row(const Table *table);

    bool is_intermediate_row() const;

    // Create a Row for a query processing result
    Row();

    // Create a Row literal
    Row(const initializer_list<string>& values);

    // Destroy this Row
    ~Row();

public:
    static void reclaim(Row*);

private:
    const Table *_table; // NULL for a query processing result
};

class RowCompare
{
public:
    static RowCompare only;

public:
    int operator()(Row* const &x, Row* const &y);
    bool cmp(Row* const &x, Row* const &y);
};

typedef bool (*RowPredicate)(const Row*);
class RowList: public vector<Row*> {};

#endif //ROW_H

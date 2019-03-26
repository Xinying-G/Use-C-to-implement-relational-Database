#ifndef INDEX_H
#define INDEX_H

#include <map>
#include <vector>

using namespace std;

class Row;

class Table;

class Index: public map<vector<string>, Row*>
{
public:
    void put(const vector<string>& key, Row* value);
    unsigned n_columns();
    Index(Table* table);

private:
    unsigned _n_columns;
};

#endif //INDEX_H

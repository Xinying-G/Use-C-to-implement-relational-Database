#ifndef UTIL_H
#define UTIL_H

#include "Row.h"

class Table;
class Iterator;

class TestRow: public Row
{
public:
    TestRow(Table* table, const vector<string>& values);
};

void add(Table* table, const vector<string>& values);
bool row_eq(const vector<string>* x, const vector<string>* y);
bool row_eq(const vector<string>* x, const vector<string>& y);
void done_with(Row* row);
bool match(Iterator* x, Iterator* y);
void print_iterator(const char* label, Iterator* input);

#define IMPLEMENT_ME 0

#endif //UTIL_H

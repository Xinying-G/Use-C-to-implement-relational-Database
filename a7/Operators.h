#ifndef OPERATORS_H
#define OPERATORS_H

#include <unordered_set>
#include "Iterator.h"
#include "Index.h"
#include "Row.h"
#include "ColumnSelector.h"

class Table;
class Row;

class TableIterator : public Iterator {
public:
    unsigned n_columns() override;
    void open() override;
    Row* next() override;
    void close() override;

public:
    explicit TableIterator(Table* table);

private:
    Table* _table;
    RowList::iterator _end;
    RowList::iterator _input;
};

class Select : public Iterator {
public:
    unsigned n_columns() override;
    void open() override;
    Row* next() override;
    void close() override;

public:
    Select(Iterator* input, RowPredicate predicate);
    ~Select();

private:
    Iterator* _input;
    RowPredicate _predicate;
};

class Project : public Iterator {
public:
    unsigned n_columns() override;
    void open() override;
    Row* next() override;
    void close() override;

public:
    Project(Iterator* input, const initializer_list<unsigned>& columns);
    ~Project();

private:
    Iterator* _input;
    ColumnSelector _column_selector;
};

class NestedLoopsJoin: public Iterator
{
public:
    unsigned n_columns() override;
    void open() override;
    Row* next() override;
    void close() override;

private:
    Row* join_rows_if_match(Row* left, Row* right);

public:
    NestedLoopsJoin(Iterator* left,
                    const initializer_list<unsigned>& left_join_columns,
                    Iterator* right,
                    const initializer_list<unsigned>& right_join_columns);
    ~NestedLoopsJoin();

private:
    Iterator* _left;
    Iterator* _right;
    ColumnSelector _left_join_columns;
    ColumnSelector _right_join_columns;
    Row* _left_row;
};

class IndexScan: public Iterator
{
public:
    unsigned n_columns() override;
    void open() override;
    Row* next() override;
    void close() override;

public:
    IndexScan(Index* index, Row* lo, Row* hi);

private:
    Index* _index;
    Row* _lo;
    Row* _hi;
    Index::iterator _input;
    Index::iterator _end;
};

class Sort: public Iterator
{
public:
    unsigned n_columns() override;
    void open() override;
    Row* next() override;
    void close() override;

public:
    Sort(Iterator* input, const initializer_list<unsigned>& sort_columns);
    ~Sort();

private:
    Iterator* _input;
    ColumnSelector _sort_columns;
    vector<Row*> _sorted;
    vector<Row*>::iterator _sorted_iterator;
};

class Unique: public Iterator
{
public:
    unsigned n_columns() override;
    void open() override;
    Row* next() override;
    void close() override;

public:
    explicit Unique(Iterator* input);
    ~Unique();

private:
    Iterator* _input;
    Row* _last_unique;
};

#endif //OPERATORS_H

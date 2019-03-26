#include "Operators.h"

Iterator* table_scan(Table* table)
{
    return new TableIterator(table);
}

Iterator* select(Iterator* input, RowPredicate predicate)
{
    return new Select(input, predicate);
}

Iterator* project(Iterator* input, initializer_list<unsigned> project_columns)
{
    return new Project(input, project_columns);
}

Iterator* nested_loops_join(Iterator* left,
                            const initializer_list<unsigned>& left_columns,
                            Iterator* right,
                            const initializer_list<unsigned>& right_columns)
{
    return new NestedLoopsJoin(left, left_columns, right, right_columns);
}

Iterator* index_scan(Index* index, Row* lo, Row* hi)
{
    return new IndexScan(index, lo, hi);
}

Iterator* sort(Iterator* input, const initializer_list<unsigned>& sort_columns)
{
    return new Sort(input, sort_columns);
}

Iterator* unique(Iterator* input)
{
    return new Unique(input);
}
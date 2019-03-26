#ifndef QUERYPROCESSOR_H
#define QUERYPROCESSOR_H

#include "Row.h"

class Iterator;
class Table;
class Index;

using namespace std;

/*
 * Return an iterator that scans that rows of the given table.
 */
Iterator* table_scan(Table* table);

/*
 * Return an iterator that scans the rows of the table identified by a search of the index.
 * The index scan begins at the first key >= lo, and ends at the last row <= hi. If hi is omitted,
 * then hi is assumed to be the same as lo, (i.e., the search is for a single key).
 */
Iterator* index_scan(Index* index, Row* lo, Row* hi = NULL);

/*
 * Return an iterator including only those input rows that satisfy the given predicate.
 */
Iterator* select(Iterator* input, RowPredicate predicate);

/*
 * Return an iterator whose rows contain only the columns specified in project_columns.
 * Duplicates are NOT eliminated.
 */
Iterator* project(Iterator* input, initializer_list<unsigned> project_columns);

/*
 * Return an iterator containing the join of rows in left and right. The join columns
 * of the left input are specified by left_columns, and those of the right input are specified
 * by right_columns. The output rows contain all the columns of the left input, followed by the
 * non-join columns of the right input. E.g., suppose that the left input has 5 columns, left_columns
 * is (0, 1); and the right input has 4 columns, with right_columns (2, 3). Then the output row has all
 * five left columns followed by the first two columns of the right input, (i.e., input columns 0 and 1).
 */
Iterator* nested_loops_join(Iterator* left,
                            const initializer_list<unsigned>& left_columns,
                            Iterator* right,
                            const initializer_list<unsigned>& right_columns);

/*
 * Return an iterator sorting by the columns specified in sort_columns.
 */
Iterator* sort(Iterator* input, const initializer_list<unsigned>& sort_columns);

/*
 * Return an iterator eliminating duplicates. This implementation assumes that the input is sorted, which
 * causes duplicates to be adjacent.
 */
Iterator* unique(Iterator* input);

#endif //QUERYPROCESSOR_H

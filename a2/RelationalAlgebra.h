#ifndef RA_C_RELATIONALALGEBRA_H
#define RA_C_RELATIONALALGEBRA_H

#include "Table.h"

typedef bool (*RowPredicate)(Row *);

typedef unordered_map<string, string> NameMap;

// Return the union of r and s.
// Throws UnionCompatibilityException if r and s don't have the same columns in the same order.
Table *onion(Table *r, Table *s);

// Return the intersection of r and s.
// Throws UnionCompatibilityException if r and s don't have the same columns in the same order.
Table *intersect(Table *r, Table *s);

// Return the difference of r and s, (r - s).
// Throws UnionCompatibilityException if r and s don't have the same columns in the same order.
Table *diff(Table *r, Table *s);

// Return the product of r and s.
// r and s must not have any columns with matching names. The output table columns will
// be the union of the columns in the input tables. E.g., the product of r(a, b) and s(c, d)
// will have columns (a, b, c, d)
// Throws TableException if the input tables do not have disjoint column names.
Table *product(Table *r, Table *s);

// Return the input table with some or all of the columns renamed according to the name_map.
// E.g. if the input has columns (a, b), then a name_map of {{"a", "x"}} will yield a table
// with columns named (x, b).
// Throws TableException if the name_map tries to rename columns that don't exist, or
// renames a column more than once.
Table *rename(Table *r, const NameMap &name_map);

// Return a table containing those rows of r for which the predicate evaluates to true.
Table *select(Table *r, RowPredicate predicate);

// Return a table containing the projection of each row on the given columns.
// Duplicates are (of course) eliminated.
// Throws TableException if columns is empty, or refers to a column that does not exist.
Table *project(Table *r, const ColumnNames &columns);

// Return the join of r and s.
// r and s must have at least one column in common. The common columns are the join columns.
// Throws JoinException if the inputs have no columns in common
Table *join(Table *r, Table *s);

#endif //RA_C_RELATIONALALGEBRA_H

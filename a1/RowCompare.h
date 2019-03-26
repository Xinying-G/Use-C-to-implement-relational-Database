#ifndef RA_ROWCOMPARE_H
#define RA_ROWCOMPARE_H

class Row;

// RowSet is a std::set<Row*, RowCompare>. RowCompare is used by std::set to implement the comparison of
// set elements to determine equality. In a set, elements are unique as determined by RowCompare.

class RowCompare
{
public:
    int operator()(Row* const &x, Row* const &y);
};

#endif //RA_ROWCOMPARE_H

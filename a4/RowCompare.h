#ifndef RA_ROWCOMPARE_H
#define RA_ROWCOMPARE_H

class Row;

class RowCompare
{
public:
    int operator()(Row* const &x, Row* const &y);
};

#endif //RA_ROWCOMPARE_H

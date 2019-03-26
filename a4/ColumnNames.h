#ifndef RA_C_COLUMNNAMES_H
#define RA_C_COLUMNNAMES_H

#include <vector>
#include <string>
#include <initializer_list>

using namespace std;

class ColumnNames : public vector<string>
{
public:
    int position(const string &name) const;

    ColumnNames();

    ColumnNames(const ColumnNames &columns);

    ColumnNames(initializer_list<string> elements);
};


#endif //RA_C_COLUMNNAMES_H

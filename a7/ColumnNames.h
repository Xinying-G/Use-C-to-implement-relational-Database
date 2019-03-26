#ifndef COLUMNNAMES_H
#define COLUMNNAMES_H

#include <vector>
#include <string>
#include <initializer_list>

using namespace std;

class ColumnNames : public vector<string>
{
public:
    int position(const string &name) const;

    ColumnNames(const initializer_list<string>& elements);
};


#endif //COLUMNNAMES_H

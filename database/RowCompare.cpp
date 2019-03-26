#include <cstring>
#include "RowCompare.h"
#include "Row.h"
#include "Table.h"

int RowCompare::operator()(Row* const &x, Row* const &y)
{
    for (const string &column : x->table()->columns()) {
        int comparison = strcmp(x->value(column).c_str(), y->value(column).c_str());
        if (comparison != 0) {
            return comparison < 0;
        }
    }
    return 0;
}

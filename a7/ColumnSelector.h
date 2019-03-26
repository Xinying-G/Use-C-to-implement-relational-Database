#ifndef COLUMNSELECTOR_H
#define COLUMNSELECTOR_H

#include <initializer_list>

using namespace std;

class ColumnSelector
{
public:
    unsigned n_columns() const;
    unsigned n_selected() const;
    unsigned n_unselected() const;
    unsigned selected(int i) const;
    unsigned unselected(int i) const;
    ColumnSelector(unsigned n_columns, const initializer_list<unsigned>& selected_positions);
    virtual ~ColumnSelector();

private:
    unsigned _n_columns;
    unsigned _n_selected;
    unsigned _n_unselected;
    unsigned *_selected;
    unsigned *_unselected;
};


#endif //COLUMNSELECTOR_H

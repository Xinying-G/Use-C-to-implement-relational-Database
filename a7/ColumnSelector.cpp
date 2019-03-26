#include <cassert>
#include "ColumnSelector.h"

unsigned ColumnSelector::n_columns() const
{
    return _n_columns;
}

unsigned ColumnSelector::n_selected() const
{
    return _n_selected;
}

unsigned ColumnSelector::n_unselected() const
{
    return _n_unselected;
}

unsigned ColumnSelector::selected(int i) const
{
    return _selected[i];
}

unsigned ColumnSelector::unselected(int i) const
{
    return _unselected[i];
}

ColumnSelector::ColumnSelector(unsigned n_columns, const initializer_list<unsigned>& selected_positions)
    : _n_columns(n_columns),
      _n_selected((unsigned) selected_positions.size()),
      _n_unselected(_n_columns - _n_selected),
      _selected(new unsigned[_n_selected]),
      _unselected(new unsigned[_n_unselected])
{
    assert(_n_selected <= _n_columns);
    // Initialize _selected
    {
        auto s = selected_positions.begin();
        unsigned i = 0;
        while (s != selected_positions.end()) {
            _selected[i++] = *s++;
        }
    }
    // Compute _unselected
    {
        unsigned u = 0;
        for (unsigned i = 0; i < _n_columns; i++) {
            bool i_selected = false;
            for (unsigned s = 0; s < _n_selected; s++) {
                i_selected = i_selected || _selected[s] == i;
            }
            if (!i_selected) {
                _unselected[u++] = i;
            }
        }
    }
}

ColumnSelector::~ColumnSelector()
{
    delete [] _selected;
    delete [] _unselected;
}

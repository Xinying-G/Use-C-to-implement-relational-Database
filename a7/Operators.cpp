#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include "QueryProcessor.h"
#include "Table.h"
#include "Index.h"
#include "Iterator.h"
#include "Row.h"
#include "ColumnSelector.h"
#include "Operators.h"
#include "util.h"

//----------------------------------------------------------------------

// TableIterator 

unsigned TableIterator::n_columns() 
{
    
    return _table->columns().size();
}

void TableIterator::open() 
{
    _end = _table->rows().end();
    _input = _table->rows().begin();
}

Row* TableIterator::next() 
{    
    if(_input != _end)
    {
      Row* row = *_input;
      _input++;
      return row;
    }
    else 
      return NULL;
    
}

void TableIterator::close() 
{
    
    _input = _end; 
}

TableIterator::TableIterator(Table* table)
    : _table(table)
{
  this->_table = table;  
}

//----------------------------------------------------------------------

// IndexScan

unsigned IndexScan::n_columns()
{    
    return _index->n_columns();
}

void IndexScan::open()
{
  _input = _index->lower_bound(*_lo);
  _end = _index->upper_bound(*_hi);
}

Row* IndexScan::next()
{
    Row* row; 
    if(_input != _end)
    {
      row = _input->second;
      _input++;
      return row;
    }
    else
      return NULL;
    
}

void IndexScan::close()
{
    _input == _end;
}

IndexScan::IndexScan(Index* index, Row* lo, Row* hi)
    : _index(index),
      _lo(lo),
      _hi(hi == NULL ? lo : hi)
{}

//----------------------------------------------------------------------

// SelectIterator 

unsigned Select::n_columns()
{
    return _input->n_columns();
}

void Select::open()
{
    _input->open();
}

Row* Select::next()
{
    Row* selected = NULL;
    Row* row;
    while((row = _input->next()) != 0 )
    {
      if(_predicate(row)){
        selected = row;
        break;
      }
      else
      {
        Row::reclaim(row);
      }

    }
    // Row::reclaim(row);
    return selected;
}

void Select::close()
{
    _input->close();
}

Select::Select(Iterator* input, RowPredicate predicate)
    : _input(input),
      _predicate(predicate)
{
}

Select::~Select()
{
    delete _input;
}

//----------------------------------------------------------------------

// ProjectIterator 

unsigned Project::n_columns()
{
    return _column_selector.n_selected();
}

void Project::open()
{
    _input->open();
}

Row* Project::next()
{
    Row* projected = NULL;
    Row* row = _input->next();
    if (row) {
        projected = new Row();
        for (unsigned i = 0; i < _column_selector.n_selected(); i++) {
            projected->append(row->at(_column_selector.selected(i)));
        }
        Row::reclaim(row);
    }
    return projected;
}

void Project::close()
{
    _input->close();
}

Project::Project(Iterator* input, const initializer_list<unsigned>& columns)
    : _input(input),
      _column_selector(input->n_columns(), columns)
{}

Project::~Project()
{
    delete _input;
}

//----------------------------------------------------------------------

// NestedLoopsJoinIterator

Row* NestedLoopsJoin::join_rows_if_match(Row* left, Row* right)
{
  Row* joined; 

    joined = new Row();
    for(unsigned i = 0; i < _left_join_columns.n_columns(); i++)
    {
      joined->append(left->at(i));
    }
    for(unsigned i = 0; i < _right_join_columns.n_unselected(); i++)
    {
      joined->append(right->at(_right_join_columns.unselected(i)));
    } 
  return joined;
}
unsigned NestedLoopsJoin::n_columns()
{
    return _left_join_columns.n_columns() 
      + _right_join_columns.n_columns()
      -_left_join_columns.n_selected();
}

void NestedLoopsJoin::open()
{
  _left->open();
  _right->open();
  _left_row = _left->next();
}

Row* NestedLoopsJoin::next()
{
  Row* last_left;
  Row* last_right;
  Row* joined;

  Row* left = _left_row;
  Row* right = new Row(); 
  Row* tmp = right;
  while(left != NULL && right != NULL)
  {
    right = _right->next();
    if(right != NULL)
    {
      bool match = false;
        
        if(left->at(_left_join_columns.selected(0))  == 
        right->at(_right_join_columns.selected(0)))
        {
          match = true;
        }

      if(match){
        break;
        }
      
      else
      {
        if(right != 0){
          Row::reclaim(right);
        }
      }
    }
    
    if(right == NULL)
    {
      last_left = left;
      left = _left->next();
      Row::reclaim(last_left);
      if(left != NULL)
      {
        _right->close();
        _right->open();
        right = _right->next();
      }
    }
  }

  _left_row = left;
  delete tmp;
  if(right != NULL && left != NULL)
  {
    joined = join_rows_if_match(left, right);
    Row::reclaim(right);
    return joined;
  }
  else {
    if(left == NULL && right != NULL){
      while(last_right = _right->next()){
        Row::reclaim(last_right);
      }
    }

    return NULL;
  }
}

void NestedLoopsJoin::close()
{
    _left->close();
    _right->close();
}

NestedLoopsJoin::NestedLoopsJoin(Iterator* left,
                                 const initializer_list<unsigned>& left_join_columns,
                                 Iterator* right,
                                 const initializer_list<unsigned>& right_join_columns)
    : _left(left),
      _right(right),
      _left_join_columns(left->n_columns(), left_join_columns),
      _right_join_columns(right->n_columns(), right_join_columns),
      _left_row(NULL)
{
    assert(_left_join_columns.n_selected() == _right_join_columns.n_selected());
}

NestedLoopsJoin::~NestedLoopsJoin()
{
    delete _left;
    delete _right;
    Row::reclaim(_left_row);
}

//----------------------------------------------------------------------

// Sort

unsigned Sort::n_columns() 
{
    return _input->n_columns();
}

void Sort::open() 
{
    _input->open();
    Row* row;
    Row* unarranged; 
    vector<Row*> _unarranged;
    map<Row*, Row*> map;

    while ((row = _input->next()) != 0) {
        unarranged = new Row();
        for (unsigned i = 0; i < _sort_columns.n_selected(); i++) {
            unarranged->append(row->at(_sort_columns.selected(i)));
        }

        _unarranged.push_back(unarranged);
        map.insert(make_pair(unarranged,row));
    }
    std::sort(_unarranged.begin(), _unarranged.end(), RowCompare());


    Row* arranged;
    for(_sorted_iterator = _unarranged.begin(); _sorted_iterator != _unarranged.end(); ++_sorted_iterator)
    {
        arranged = map.find(*_sorted_iterator)->second;
        _sorted.push_back(arranged);

    }

    for(vector<Row*>::iterator it = _unarranged.begin(); it != _unarranged.end(); ++it)
    {
      delete *it;
    }
    _sorted_iterator = _sorted.begin();
    
}

Row* Sort::next() 
{
    Row* sorted = NULL;
    if (_sorted_iterator != _sorted.end()) {
      sorted = *_sorted_iterator;
      _sorted_iterator++;
    }
    return sorted;
}

void Sort::close() 
{
    _input->close();
    _sorted.clear();
}

Sort::Sort(Iterator* input, const initializer_list<unsigned>& sort_columns)
    : _input(input),
      _sort_columns(input->n_columns(), sort_columns)
{}

Sort::~Sort()
{
    delete _input;
}

//----------------------------------------------------------------------

// Unique

unsigned Unique::n_columns()
{
    return _input->n_columns();
}

void Unique::open() 
{
    _input->open();
    _last_unique = new Row();
}

Row* Unique::next()
{
  Row* unique;
  while((unique = _input->next()) != 0)
  {
    if((*unique) != (*_last_unique))
    {
      *_last_unique = *unique;
      break;
    }
    else
    {
      Row::reclaim(unique);
    }
  }
  return unique;
}

void Unique::close() 
{
    delete _last_unique;
    _input->close();
}

Unique::Unique(Iterator* input)
    : _input(input),
      _last_unique(NULL)
{}

Unique::~Unique()
{
    delete _input;
}

#include <sstream>
#include <cstring>
#include "RelationalAlgebra.h"
#include "Database.h"
#include "unittest.h"
#include <algorithm>

bool isColumnNamesEqual(Table *r, Table *s);

Table *onion(Table *r, Table *s)
{

    if(!isColumnNamesEqual(r,s))
    {
        throw UnionIncompatibilityException("have different ColumnNames");
    }
    else
    {
        Table* onion = Database::new_table
            (Database::new_table_name(), r->columns());
        for(set<Row*, RowCompare>::iterator it = 
            r->rows().begin(); it != r->rows().end(); it++)
        {
            Row *row = new Row(onion,(*(*it)));
            if(!onion->add(row))
                delete row;
        }

        for(set<Row*, RowCompare>::iterator it = 
            s->rows().begin(); it != s->rows().end(); it++)
        {
            Row *row = new Row(onion,(*(*it)));
            if(!onion->add(row))
                delete row;       
        }
        return onion;        
    }
}

Table *intersect(Table *r, Table *s)
{
    if(!isColumnNamesEqual(r,s))
        throw UnionIncompatibilityException("have different ColumnNames");
    else
    {
        Table* intersect = Database::new_table
            (Database::new_table_name(), r->columns());

        for(set<Row*, RowCompare>::iterator it = 
            r->rows().begin(); it != r->rows().end(); it++)
        {
            if (s->has((*it)))
            {
                Row *row = new Row(intersect,(*(*it)));
                intersect->add(row);
            }
            
        }
        return intersect;
    }
}

Table *diff(Table *r, Table *s)
{
    if(!isColumnNamesEqual(r,s))
        throw UnionIncompatibilityException("have different ColumnNames");
    else
    {
        Table* intersect = Database::new_table
            (Database::new_table_name(), r->columns());

        for(set<Row*, RowCompare>::iterator it = 
            r->rows().begin(); it != r->rows().end(); it++)
        {
            if (!s->has((*it)))
            {
                Row *row = new Row(intersect,(*(*it)));
                intersect->add(row);
            }            
        }
        return intersect;            
    }
    
}

Table *product(Table *r, Table *s)
{
    
    for(unsigned i = 0; i < r->columns().size(); i++)
    {
        for(unsigned j = 0; j < s->columns().size(); j++)
        {
            if(r->columns()[i] == s->columns()[j])
            {
                throw TableException("Dupilicate columns");
            }

        }        
    }

    ColumnNames a; 
    for(unsigned k = 0; k < r->columns().size(); k++)
    {            
        a.push_back(r->columns()[k]);
    } 

    for(unsigned  l= 0; l < s->columns().size(); l++)
    {            
        a.push_back(s->columns()[l]);
    } 

    Table* product = Database::new_table
    (Database::new_table_name(), a);

    for(set<Row*, RowCompare>::iterator it1 = r->rows().begin(); 
        it1 != r->rows().end(); it1++)
    {
        for(set<Row*, RowCompare>::iterator it2 = s->rows().begin(); 
        it2 != s->rows().end(); it2++)
        {
            Row *row = new Row(product);
            for(int m = 0; m < (*it1)->get_row_size(); m++)
            {                
                row->append((*it1)->at(m));
            }
            for(int m = 0; m < (*it2)->get_row_size(); m++)
            {                
                row->append((*it2)->at(m));
            }

            if(!product->add(row))
                delete row;
        }
    }

    return product;     
}

Table *rename(Table *r, const NameMap &name_map)
{
    ColumnNames a = r->columns();

    for(auto it = name_map.begin(); 
        it != name_map.end(); it++)
    {
        bool finder = false;
        for(unsigned i = 0; i < a.size(); i++)
        {            
            
            if(a[i] == it->first)
            {                
                finder = true;
                a[i] = it->second;
            }
        } 
        if(finder == false)
            throw TableException("Name_map is wrong");
    }

    Table* rename = Database::new_table
    (Database::new_table_name(), a);

    for(set<Row*, RowCompare>::iterator it = r->rows().begin(); 
        it != r->rows().end(); it++)
    {
        Row *row = new Row(rename,(*(*it)));
        rename->add(row);
    }

    return rename;
}

Table *select(Table *r, RowPredicate predicate)
{
    Table* select = Database::new_table
    (Database::new_table_name(), r->columns());

    for(set<Row*, RowCompare>::iterator it = r->rows().begin(); 
        it != r->rows().end(); it++)
    {
        if(predicate((*it)))
        {
            Row *row = new Row(select,(*(*it)));
            select->add(row);
        }

    }

    return select;
}

Table *project(Table *r, const ColumnNames &columns)
{
    if(columns.empty())
        throw TableException("columns is empty");

    ColumnNames a;
    vector<int> positions; 
    for(unsigned i = 0; i < columns.size(); i++)
    {
        int position = r->columns().position(columns[i]);

        if(position == -1)
        {
            throw TableException("Project object can't find");
        }
        else
        {
            a.push_back(r->columns()[position]);
            positions.push_back(position);
        }
    }


    Table* project = Database::new_table
    (Database::new_table_name(), a);
    


    for(set<Row*, RowCompare>::iterator it = r->rows().begin(); 
        it != r->rows().end(); it++)
    {
        Row *row = new Row(project);

        for(unsigned i = 0; i < positions.size(); i++)
        {
            row -> append((*it)->at(positions[i]));
        }
                
        if(!project->add(row))
            delete row;             
    }

    return project;
}

Table *join(Table *r, Table *s)
{
    ColumnNames a;
    bool found = false;
    unsigned indexs;
    unsigned indexr;

    for(unsigned i = 0; i < r->columns().size(); i++)
    {
        for(unsigned j = 0; j < s->columns().size(); j++)
        {
            if(r->columns()[i] == s->columns()[j])
            {
                indexr = i;
                indexs = j;
                found = true;
            }

        }        
    }

    if(found == false)
        throw JoinException("Joint failed");

    for(unsigned i = 0; i < r->columns().size(); i++)
    {            
        a.push_back(r->columns()[i]);
    } 

    for(unsigned  j= 0; j < s->columns().size(); j++)
    {            
        if(j != indexs)
        {
            a.push_back(s->columns()[j]);
        }
        
    } 

    Table* join = Database::new_table
    (Database::new_table_name(), a);

    for(set<Row*, RowCompare>::iterator it1 = r->rows().begin(); 
        it1 != r->rows().end(); it1++)
    {
        for(set<Row*, RowCompare>::iterator it2 = s->rows().begin(); 
        it2 != s->rows().end(); it2++)
        {
            if ((*it1)->at(indexr) == (*it2)->at(indexs))
            {
                Row *row = new Row(join);

                for(int k = 0; k < (*it1)->get_row_size(); k++)
                {                
                    row->append((*it1)->at(k));
                }

                for(int m = 0; m < (*it2)->get_row_size(); m++)
                {                
                    if(m != (int)indexs)
                    {
                    row->append((*it2)->at(m));
                    }               
                }
                join->add(row);
            }
                       
        }
    }

    return join;
}

bool isColumnNamesEqual(Table *r, Table *s)
{
    set<string> rset(r->columns().begin(), r->columns().end());
    set<string> sset(s->columns().begin(), s->columns().end());
    return ((r->columns().size() == s->columns().size()) && rset == sset);
}

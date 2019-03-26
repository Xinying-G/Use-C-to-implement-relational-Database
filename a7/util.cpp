#include <cassert>
#include "util.h"
#include "dbexceptions.h"
#include "Table.h"
#include "Iterator.h"

TestRow::TestRow(Table* table, const vector<string>& values)
    : Row(table)
{
    auto n = values.size();
    for (unsigned i = 0; i < n; i++) {
        append(values.at(i));
    }
}

void add(Table* table, const vector<string>& values)
{
    Row* row = new Row(table);
    try {
        unsigned long n = values.size();
        for (unsigned i = 0; i < n; i++) {
            row->append(values.at(i));
        }
        table->add(row);
    } catch (TableException& e) {
        delete row;
        throw e;
    }
}

bool row_eq(const vector<string>* x, const vector<string>* y)
{
    assert(x->size() == y->size());
    for (unsigned long i = 0; i < x->size(); i++) {
        if (x->at(i) != y->at(i)) {
            return false;
        }
    }
    return true;

}

bool row_eq(const vector<string>* x, const vector<string>& y)
{
    assert(x->size() == y.size());
    for (unsigned long i = 0; i < x->size(); i++) {
        if (x->at(i) != y.at(i)) {
            return false;
        }
    }
    return true;

}

void done_with(Row* row)
{
    if (row->is_intermediate_row()) {
        delete row;
    }
}

bool match(Iterator* x, Iterator* y)
{
    bool match = true;
    if (x == NULL || y == NULL) {
        match = false;
    } else if (x->n_columns() != y->n_columns()) {
        match = false;
    } else {
        x->open();
        y->open();
        Row* x_row;
        Row* y_row;
        while (match && (x_row = x->next()) != NULL && (y_row = y->next()) != NULL) {
            if (!row_eq(x_row, y_row)) {
                match = false;
            }
            done_with(x_row);
            done_with(y_row);
        }
        match = match && x->next() == NULL && y->next() == NULL;
        x->close();
        y->close();
    }
    return match;
}

void print_iterator(const char* label, Iterator* input)
{
    printf("%s:\n", label);
    Row* row;
    int n_columns = input->n_columns();
    input->open();
    while ((row = input->next())) {
        for (int i = 0; i < n_columns; i++) {
            if (i > 0) {
                printf("\t");
            }
            printf("%s", row->at(i).c_str());
        }
        printf("\n");
        done_with(row);
    }
    input->close();
    delete input;
}



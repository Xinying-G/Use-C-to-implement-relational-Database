#ifndef RA_UTIL_H
#define RA_UTIL_H

class Table;

class NotYetImplementedException: public exception
{
public:
    NotYetImplementedException();

    const char *what() const throw();
};

void print_table(const char *label, Table *t);

bool eq(Table* x, Table* y);

void reset_database();

void IMPLEMENT_ME();

Table* IMPLEMENT_THIS_QUERY();

#endif //RA_UTIL_H

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

using namespace std;

class DBException : public exception
{
public:
    DBException(const string &message)
            : _message(message)
    {}

    const char *what() const throw()
    {
        return _message.c_str();
    }

private:
    const string _message;
};

class RowException : public DBException
{
public:
    RowException(const string &message) : DBException(message)
    {}
};

class TableException : public DBException
{
public:
    TableException(const string &message) : DBException(message)
    {}
};

#endif //EXCEPTIONS_H

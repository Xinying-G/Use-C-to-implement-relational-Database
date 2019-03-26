#ifndef RA_C_EXCEPTIONS_H
#define RA_C_EXCEPTIONS_H

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

class UnionIncompatibilityException : public DBException
{
public:
    UnionIncompatibilityException(const string &message) : DBException(message)
    {}
};

class JoinException : public DBException
{
public:
    JoinException(const string &message) : DBException(message)
    {}
};

#endif //RA_C_EXCEPTIONS_H

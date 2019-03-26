#ifndef UNITTEST_H
#define UNITTEST_H

#include <cstdio>
#include <cstring>
#include <memory>
#include <exception>
#include <vector>

using namespace std;

typedef void (*TestFunction)();

class Test
{
public:
    TestFunction function();

    const char* name();

    Test(const char* test_name, TestFunction test_function);

    ~Test();

private:
    const char* _name;
    TestFunction _function;
};

class TestSuite
{
public:
    void before_test(TestFunction before_test);

    void after_test(TestFunction after_test);

    void before_all(TestFunction before_all);

    void after_all(TestFunction after_all);

    void add(Test* test);

    void run_tests();

    void reset();

    TestSuite();

public:
    static TestSuite only;

private:
    vector<Test*> _tests;
    TestFunction _before_test;
    TestFunction _after_test;
    TestFunction _before_all;
    TestFunction _after_all;
};

class AssertionException: public exception
{
public:
    AssertionException(const char* file, int line)
        : _file(file),
          _line(line)
    {}

    const char *what() const throw()
    {
        static char buffer[100];
        sprintf(buffer, "Test assertion failed at %s:%d", _file, _line);
        return buffer;
    }

private:
    const char* _file;
    int _line;
};

#define CHECK(a) check(a, __FILE__, __LINE__)
#define FAILx() fail(__FILE__, __LINE__)

void check(bool a, const char* file, int line);
void fail(const char* file, int line);

#define BEFORE_TEST(f) TestSuite::only.before_test(f)
#define AFTER_TEST(f) TestSuite::only.after_test(f)
#define BEFORE_ALL_TESTS(f) TestSuite::only.before_all(f)
#define AFTER_ALL_TESTS(f) TestSuite::only.after_all(f)
#define ADD_TEST(t) TestSuite::only.add(new Test(#t, t))
#define RUN_TESTS() TestSuite::only.run_tests()

#endif //UNITTEST_H

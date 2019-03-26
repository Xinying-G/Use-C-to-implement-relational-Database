//
// Created by jao on 1/16/18.
//

#ifndef RA_C_TESTING_H
#define RA_C_TESTING_H

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
    AssertionException()
    {}

    const char *what() const throw()
    {
        return "Test assertion failed";
    }

};

void assert(bool a);

void fail();

#define BEFORE_TEST(f) TestSuite::only.before_test(f)
#define AFTER_TEST(f) TestSuite::only.after_test(f)
#define BEFORE_ALL_TESTS(f) TestSuite::only.before_all(f)
#define AFTER_ALL_TESTS(f) TestSuite::only.after_all(f)
#define ADD_TEST(t) TestSuite::only.add(new Test(#t, t))
#define RUN_TESTS() TestSuite::only.run_tests()

#endif //RA_C_TESTING_H

//
// Created by jao on 1/16/18.
//

#include "unittest.h"

TestSuite TestSuite::only;

typedef void (*TestFunction)();

TestFunction Test::function()
{
    return _function;
}

const char *Test::name()
{
    return _name;
}

Test::Test(const char *test_name, TestFunction test_function)
        : _name(strdup(test_name)),
          _function(test_function)
{}

Test::~Test()
{
    free((void*)_name);
}

void TestSuite::add(Test *test)
{
    _tests.emplace_back(test);
}

void TestSuite::run_tests()
{
    unsigned long ok = 0;
    try {
        if (_before_all) {
            _before_all();
        }
    } catch (exception &exception1) {
        printf("%c[%dmTest initialization failed!%c[%dm\n", 27, 7, 27, 0);
    }
    for (unsigned long i = 0; i < _tests.size(); i++) {
        Test* test = _tests.at(i);
        if (_before_test) {
            _before_test();
        }
        try {
            test->function()();
            printf(":-)  %2ld/%2ld -- %s: ok\n", i + 1, _tests.size(), test->name());
            ok++;
        } catch (exception &e) {
            printf("%c[%dm:-(%c[%dm", 27, 7, 27, 0);
            printf("  %2ld/%2ld -- %s: FAILED -- %s\n", i + 1, _tests.size(), test->name(), e.what());
        }
        if (_after_test) {
            _after_test();
        }
        delete test;
    }
    try {
        if (_after_all) {
            _after_all();
        }
    } catch (exception &exception1) {
        printf("%c[%dmTest shutdown failed!%c[%dm\n", 27, 7, 27, 0);
    }
    printf("SUMMARY:\n");
    printf("    %2ld tests\n", _tests.size());
    printf("    %2ld passed\n", ok);
    unsigned long failed = _tests.size() - ok;
    if (failed == 0) {
        printf("    %2ld failed\n", failed);
    } else {
        printf("    %c[%dm%2ld failed%c[%dm\n", 27, 7, failed, 27, 0);
    }
    reset();
}

void TestSuite::before_test(TestFunction before_test)
{
    _before_test = before_test;
}

void TestSuite::after_test(TestFunction after_test)
{
    _after_test = after_test;
}

void TestSuite::before_all(TestFunction before_all)
{
    _before_all = before_all;
}

void TestSuite::after_all(TestFunction after_all)
{
    _after_all = after_all;
}

void TestSuite::reset()
{
    _tests.clear();
    _before_test = nullptr;
    _after_test = nullptr;
    _before_all = nullptr;
    _after_all = nullptr;
}

TestSuite::TestSuite()
    : _before_test(nullptr),
      _after_test(nullptr),
      _before_all(nullptr),
      _after_all(nullptr)
{}

void check(bool a, const char* file, int line)
{
    if (!a) {
        throw AssertionException(file, line);
    }
}

void fail(const char* file, int line)
{
    throw AssertionException(file, line);
}

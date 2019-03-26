cmake_minimum_required(VERSION 3.9)
project(ra)

set(CMAKE_CXX_STANDARD 11)

add_executable(a1
        ColumnNames.cpp
        ColumnNames.h
        Database.cpp
        Database.h
        Row.cpp
        Row.h
        Table.cpp
        Table.h
        dbexceptions.h
        main.cpp
        test.cpp
        unittest.cpp
        unittest.h
        util.cpp
        util.h
        RowCompare.cpp
        RowCompare.h)

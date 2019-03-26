EXECUTABLE=a7

default: $(EXECUTABLE)

HEADERS = \
	ColumnNames.h \
	ColumnSelector.h \
	Database.h \
	Index.h \
	Iterator.h \
	Operators.h \
	QueryProcessor.h \
	Row.h \
	Table.h \
	dbexceptions.h \
	unittest.h \
	util.h

OBJECTS = \
	ColumnNames.o \
	ColumnSelector.o \
	Database.o \
	Index.o \
	main.o \
	Operators.o \
	QueryProcessor.o \
	Row.o \
	Table.o \
	test_operators.o \
	test_query_plans.o \
	unittest.o \
	util.o

CCFLAGS= -g -Wall -Wno-unused-function -O0 -std=c++11

CC=g++

ColumnNames.o: $(HEADERS)
ColumnSelector.o: $(HEADERS)
Database.o: $(HEADERS)
Index.o: $(HEADERS)
main.o: $(HEADERS)
Operators.o: $(HEADERS)
QueryProcessor.o: $(HEADERS)
Row.o: $(HEADERS)
Table.o: $(HEADERS)
test_operators.o: $(HEADERS)
test_query_plans.o: $(HEADERS)
unittest.o: $(HEADERS)
util.o: $(HEADERS)

.cpp.o: $(HEADERS)
	g++ $(CCFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	g++ $(CCFLAGS) $(OBJECTS) -o $(EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

HEADERS = \
	ColumnNames.h \
	Database.h \
	Row.h \
	RowCompare.h \
	Table.h \
	dbexceptions.h \
	unittest.h \
	util.h

OBJECTS = \
	ColumnNames.o \
	Database.o \
	Row.o \
	RowCompare.o \
	Table.o \
	main.o \
	test.o \
	unittest.o \
	util.o

default: a1

.cpp.o:
	g++ -std=c++11 -Wall -Wno-unused-function -c $< -o $@

a1: $(OBJECTS)
	g++ -std=c++11 -Wall -Wno-unused-function $(OBJECTS) -o a1

clean:
	rm -f $(OBJECTS) a1

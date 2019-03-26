HEADERS = \
	ColumnNames.h \
	Database.h \
	RelationalAlgebra.h \
	Row.h \
	RowCompare.h \
	Table.h \
	dbexceptions.h \
	unittest.h \
	util.h

OBJECTS = \
	ColumnNames.o \
	Database.o \
	RelationalAlgebra.o \
	Row.o \
	RowCompare.o \
	Table.o \
	main.o \
	test.o \
	unittest.o \
	util.o

default: a2

.cpp.o:
	g++ -std=c++11 -Wall -Wno-unused-function -c $< -o $@

a2: $(OBJECTS)
	g++ -std=c++11 -Wall -Wno-unused-function $(OBJECTS) -o a2

clean:
	rm -f $(OBJECTS) a2

HEADERS = \
	ColumnNames.h \
	Database.h \
	RelationalAlgebra.h \
	Row.h \
	RowCompare.h \
	dbexceptions.h \
	unittest.h \
	Table.h

OBJECTS = \
	ColumnNames.o \
	Database.o \
	RelationalAlgebra.o \
	Row.o \
	RowCompare.o \
	Table.o \
	main.o \
	test.o \
	unittest.o

EXECUTABLE = a4

default: $(EXECUTABLE)

ColumnNames.o: $(HEADERS)
RelationalAlgebra.o: $(HEADERS)
Table.o: $(HEADERS)
test_queries.o: $(HEADERS)
Database.o: $(HEADERS)
RowCompare.o: $(HEADERS)
test_ra.o: $(HEADERS)
main.o: $(HEADERS)
Row.o: $(HEADERS)
testing.o: $(HEADERS)
util.o: $(HEADERS)

.cpp.o:
	g++ -std=c++11 -Wall -Wno-unused-function -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	g++ -std=c++11 -Wall -Wno-unused-function $(OBJECTS) -o $(EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

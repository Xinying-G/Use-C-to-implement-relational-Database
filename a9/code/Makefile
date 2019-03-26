HEADERS = \
	db.h \
	txn.h

OBJECTS = \
	db.o \
	main.o \
	txn.o

default: a9

.cpp.o: $(HEADERS)
	gcc -I/usr/include/postgresql -Wall -Wno-unused-function -c -g $< -o $@

a9: $(OBJECTS)
	gcc -L/usr/lib/x86_64-linux-gnu -Wall -Wno-unused-function $(OBJECTS) -lpq -o a9

clean:
	rm -f $(OBJECTS) a9

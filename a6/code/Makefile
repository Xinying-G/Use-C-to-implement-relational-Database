HEADERS = \
	db.h \
	msg.h

OBJECTS = \
	db.o \
	main.o \
	msg.o

default: a6

.cpp.o: $(HEADERS)
	gcc -I/usr/include/postgresql -Wall -Wno-unused-function -c -g $< -o $@

a6: $(OBJECTS)
	gcc -L/usr/lib/x86_64-linux-gnu -Wall -Wno-unused-function $(OBJECTS) -lpq -o a6

clean:
	rm -f $(OBJECTS) a6

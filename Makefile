PROG=emptydir
CC=gcc
CFLAGS=-Wall
OBJS=
PREFIX=/usr/local/bin

default: build

build: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS) $(PROG).c

.c.o:
	$(CC) -c $(CFLAGS) $(LDFLAGS) $<

checkleaks: clean $(PROG)
	valgrind --leak-check=full ./$(PROG) $(ARGS)

clean:
	rm -f *.o $(PROG)

debug: clean $(OBJS)
	$(CC) $(CFLAGS) -g -o $(PROG) $(OBJS) $(PROG).c $(LDFLAGS)
	gdb ./$(PROG)

install: $(PROG)
	cp $(PROG) $(PREFIX)/$(PROG)

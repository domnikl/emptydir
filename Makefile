PROG=emptydir
CC=gcc
CFLAGS=-Wall
LIBS=
OBJS=

default: build

build: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(PROG).c $(LIBS)

.c.o:
	$(CC) -c $(CFLAGS) $(LIBS) $<

checkleaks: clean $(PROG)
	valgrind --leak-check=full ./$(PROG) $(ARGS)

clean:
	rm -f *.o $(PROG)

debug: clean $(OBJS)
	$(CC) $(CFLAGS) -g -o $(PROG) $(OBJS) $(PROG).c $(LIBS)
	gdb ./$(PROG)

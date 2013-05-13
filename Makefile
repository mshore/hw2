CFLAGS=-g -Wall
CC=gcc
SRCS=tokenizer.c shell.c
OBJS=tokenizer.o shell.o
LDFLAGS=-g
LIBS=

all:    test

$(SRCS):
	$(CC) $(CFLAGS) -c $*.c

test: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o test $(OBJS)

clean:
	rm -f *.o test

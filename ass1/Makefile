
CC = gcc
CFLAGS = -Wall -Werror -Wno-unused-function -g

COMMON = FlightDb.c AVLTree.c List.c Record.c

.PHONY: all
all: test

test: test.c $(COMMON)
	$(CC) $(CFLAGS) -o test test.c $(COMMON)

clean:
	rm -f test


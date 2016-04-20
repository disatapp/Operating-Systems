#name: Pavin Disatapundhu#e-mail: disatapp@onid.oregonstate.edu#class: CS344-400
#assignment: Homework #6, make

CC=gcc
CFLAGS = -Wall -Wshadow -std=c99 -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement

all: compute

compute: compute.o
	$(CC) $(CFLAGS) -o compute compute.o
	
compute.o: compute.c
	$(CC) $(CFLAGS) -c compute.c

clean:
	rm -f *.o compute

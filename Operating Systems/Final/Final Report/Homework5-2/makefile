#name: Pavin Disatapundhu#e-mail: disatapp@onid.oregonstate.edu#class: CS344-400
#assignment: Homework #5, make

CC=gcc
CFLAGS = -Wall -Wshadow -g -std=c99 -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement -lrt -lm 

all: primemproc primepthread

primemproc: primemproc.o
	$(CC) $(CFLAGS) -o primemproc primemproc.o
	
primemproc.o: primemproc.c
	$(CC) $(CFLAGS) -c primemproc.c
	
primepthread: primepthread.o
	$(CC) $(CFLAGS) -o primepthread primepthread.o
	
primepthread.o: primepthread.c
	$(CC) $(CFLAGS) -c primepthread.c

clean:
	rm -f *.o primemproc primepthread
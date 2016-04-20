#
#  $RCSfile$
#  $Revision$
#  $Author$
#  $Date$
#  $Log$
#

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall
PROGS=HW1Q8


all: $(PROGS)

HW1Q8: HW1Q8.o
	$(CC) $(CFLAGS) -std=c99 -o HW1Q8 HW1Q8.o -lm

HW1Q8.o: HW1Q8.c
	$(CC) $(CFLAGS) -std=c99 -c HW1Q8.c -lm


clean:
	rm -f $(PROGS) *.o *~


C=gcc
CFLAGS= -g -Wall
PROGS=myar sig_demo

all: ${PROGS}

myar: myar.o
	${CC} ${CFLAGS} -o myar myar.c

myar.o: myar.c
	${CC} ${CFLAGS} -c myar.c
sig_demo: sig_demo.o
	${CC} ${CFLAGS} -o sig_demo sig_demo.c

sig_demo.o: sig_demo.c
	${CC} ${CFLAGS} -c sig_demo.c

clean:
	rm -f ${PROGS} *.o *~ *.ar

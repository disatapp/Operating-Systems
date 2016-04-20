#name: Pavin Disatapundhu#e-mail: disatapp@onid.oregonstate.edu#class: CS344-400
#assignment: Homework #4, make

CC=gcc
CFLAGS = -Wall -Wshadow -std=c99 -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement 

all: pipeline uniqify

pipeline: pipeline.o
	$(CC) -g $(CFLAGS) -o pipeline pipeline.o
	
pipeline.o: pipeline.c
	$(CC) -g $(CFLAGS) -c pipeline.c

uniqify: uniqify.o
	$(CC) -g $(CFLAGS) -o uniqify uniqify.o
	
uniqify.o: uniqify.c
	$(CC) -g $(CFLAGS) -c uniqify.c

testu:
	./uniqify -n 5 < websters.txt > untest
	./uniqify.bash < websters.txt > unbashtest
	diff unbashtest untest

testp:
	./pipeline -f pitest < words.txt > pitest2
	./pipeline.bash -f pibashtest < words.txt > pibashtest2 
	diff pibashtest pitest
	diff pibashtest2 pitest2
	


test_all: test_pipeline test_uniquify

clean:
	rm -f $(PROGS) *.o *~ *#
	rm -f pibashtest pibashtest2 pitest pitest2
	rm -f unbashtest untest

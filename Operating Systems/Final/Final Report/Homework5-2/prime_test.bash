#!/bin/bash
#name: Pavin Disatapundhu
#e-mail: disatapp@onid.oregonstate.edu
#class: CS344-400
#assignment: Homework #5, prime_test.bash


test=( 1000 2000 4000 8000 16000 32000 64000 128000 256000 512000 )

echo "Testing primemproc.c"
for prime in ${!test[@]};
do
	for proc in {1..10}
	do
		printf "Testing %d numbers, with %d proc" $prime $proc
		time ./primemproc -q -m $prime -c $proc
	done
done

#http://www.linuxjournal.com/content/bash-arrays
echo "Testing primepthread.c"
for prime in ${!test[@]};
do
    for proc in {1..10}
    do
        printf "Testing %d numbers, with %d proc" $prime $proc
        time ./primepthread -q -m $prime -c $proc
    done
done

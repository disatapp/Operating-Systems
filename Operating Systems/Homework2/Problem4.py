# Pavin Disatapundhu
# disatapp@onid.oregonstate.edu
# CS344-400
# homework#2 Question4
import math
import sys

def findprime(prime):
    i = 0
    n = 2
    isprime = True
    while i < prime:
        for j in range(2,int(math.sqrt(n)+1)):
            if (n % j) == 0:
                isprime = False
        if isprime == True:
            i += 1
            if i == prime:
                nprime = n
        n += 1
        isprime = True
    print nprime

if __name__ == '__main__':
    if(len(sys.argv) == 2):
        prime = int(sys.argv[1])
        if 1 <= prime <= 2300:
            findprime(prime)
        else:
            sys.exit('Invalid: please enter a number between 1 and 2300')
    else:
        sys.exit('Invalid: please enter 2 arguments.')

#name: Pavin Disatapundhu
#e-mail: disatapp@onid.oregonstate.edu
#class: CS344-400
#assignment: Homework #6, report.py

import socket
import sys
import re
import argparse
import signal
import select
import string

MAXSIZE = 8192
HOST = "127.0.0.1"
PORT = 1234

#initialize signals
def intisig():
    signal.signal(signal.SIGINT, sighandle)
    signal.signal(signal.SIGHUP, sighandle)
    signal.signal(signal.SIGQUIT, sighandle)
    
    #http://pymotw.com/2/signal/

def sighandle(signum,stack):
    sys.exit
#http://itsjustsosimple.blogspot.com/2014/01/python-signal-handling-and-identifying.html

def command(mrecv, st):
    begin = mrecv.find(st)
    e = st[:1]+'/'+st[1:]
    finish = mrecv.find(e)
    return mrecv[len(st) + begin:finish]
#http://stackoverflow.com/questions/4666973/how-to-extract-a-substring-from-inside-a-string-in-python
#http://stackoverflow.com/questions/5254445/add-string-in-a-certain-position-in-python

#scan string for all the perfect numbers
def rangearr(mrecv):
    begin = [b.start() for b in re.finditer("<pn>", mrecv)]
    finish = [f.start() for f in re.finditer("</pn>", mrecv)]

    for b, f in zip(begin, finish):
        idx = int(mrecv[len("<pn>")+b:f])
        print idx,
#http://pymotw.com/2/re/
##https://docs.python.org/2/library/functions.html#zip

    
def main():
    intisig()
    #commandline
    parser = argparse.ArgumentParser(description='Report and kill.')
    parser.add_argument("-k",action ="store_true")
    args = parser.parse_args()
    #https://docs.python.org/2/howto/argparse.html#id1
            
    #connect to manage server
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect((HOST, PORT))
    except:
        print "Cannot connect!!!"
        sys.exit()
    
    #handshake recv the message
    mrecv1 = s.recv(MAXSIZE)
    cmd = command(mrecv1,"<message>")
    print cmd
    s.send("<?xml version='1.0' encoding='UTF-8'?><print><action>-p</action></print>")
    
    s.send("<?xml version='1.0' encoding='UTF-8'?><print><action>-s</action></print>")
    #get perfect numbers
    mrecv2 = s.recv(MAXSIZE)
    cmd = command(mrecv2,"<curmax>")
    print "Perfect number up to",cmd
    print "List Retrieved:",
    rangearr(mrecv2)
    
    if args.k:
        s.send("<?xml version='1.0' encoding='UTF-8'?><print><action>-k</action></print>")
        print "Exit program"

if __name__ == "__main__":
    main()

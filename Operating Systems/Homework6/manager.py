#name: Pavin Disatapundhu
#e-mail: disatapp@onid.oregonstate.edu
#class: CS344-400
#assignment: Homework #6, manager.py

import socket
import sys
import signal
import re
import select
import string


HOST = "127.0.0.1"
PORT = 1234
MAXSIZE = 8192

handle = []
calc = []
answer = []
allperfect = []

#initialize signals
def intisig():
    signal.signal(signal.SIGINT, sighandle)
    signal.signal(signal.SIGHUP, sighandle)
    signal.signal(signal.SIGQUIT, sighandle)
    #http://pymotw.com/2/signal/

#start server
def intiserver():
    msockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    msockfd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    msockfd.bind((HOST, PORT))
    msockfd.listen(5)
    return msockfd
#start handler for exiting program
def sighandle(signum,stack):
    for control in handle:
        control.send("<?xml version='1.0' encoding='UTF-8'?><server><action>-k</action></server>")
    print("Exiting")
    sys.exit()
#http://itsjustsosimple.blogspot.com/2014/01/python-signal-handling-and-identifying.html
    
#disconnect function
def disconn(s):
    if s in handle:
        handle.remove(s)
        return "compute.c (handle)"
    elif s in answer:
        answer.remove(s)
        return "report.py"
    elif s in calc:
        calc.remove(s)
        return "compute.c (main)"

#determine the action 
def command(mrecv, st):
    begin = mrecv.find(st)
    e = st[:1]+'/'+st[1:]
    finish = mrecv.find(e)
    return mrecv[len(st) + begin:finish]
#http://stackoverflow.com/questions/4666973/how-to-extract-a-substring-from-inside-a-string-in-python
#http://stackoverflow.com/questions/5254445/add-string-in-a-certain-position-in-python

#range
def rangearr(mrecv, arr):
    begin = [b.start() for b in re.finditer("<pn>", mrecv)]
    finish = [f.start() for f in re.finditer("</pn>", mrecv)]

    for b, f in zip(begin, finish):
        idx = int(mrecv[len("<pn>")+b:f])
        arr.remove(0)
        arr.append(idx)
#http://pymotw.com/2/re/
#https://docs.python.org/2/library/functions.html#zip
    
#-h = handler, -c = computer, -p = report
def selectconn(cmd,client):
    if cmd == "-h":
        handle.append(client)
    elif cmd == "-c":
        calc.append(client)
    elif cmd == "-p":
        answer.append(client)
    print "Command: ",cmd," has been received."

#get the max range
def getmax(i,j):
    while j > 0:
        j -= int(i) 
        i += 1
    return i

def connectionloop(msockfd):
    sockhost, sockport = msockfd.getsockname()
    arr = [0] * 6
    largest = 0
    flag = 0
    count = 1
    inputsock = [msockfd]
    print "IP =",sockhost
    print "port =",sockport
    while 1:
        #select a socket
        inputrdy, outputrdy, exceptrdy = select.select(inputsock,[],[])

        #http://stackoverflow.com/questions/20471816/how-does-the-select-function-in-the-select-module-of-python-exactly-work
        for s in inputrdy:
            #monitoring the control socket
            if s == msockfd:
                client, addr = msockfd.accept()
                inputsock.append(client)
                print "Client connected: Host =",sockhost,"Port =",sockport
            
                #send
                client.send("<?xml version='1.0' encoding='UTF-8'?><server><message>Connected to Server</message></server>")
                #recive the connection and format the action
                mrecv1 = client.recv(MAXSIZE)
                cmd = command(mrecv1,"<action>")
                selectconn(cmd,client)
            else:
                #recive the command
                mrecv2 = s.recv(MAXSIZE)
                cmd = command(mrecv2,"<action>")
                #deterimine what action to take based on input
                #-r = range, -k = kill, -rn = list perfect, -s = send
                if mrecv2:
                    #calculate the range
                    if cmd == "-r":
                        print "Calculate range for client."
                        cpu = int(command(mrecv2,"<cpu>"))*15
                        tmp = str(count)
                        count = str(getmax(int(count), int(cpu)))
                        print "Sending range",tmp,"-",count
                        tmp2 = count
                        s.send("<?xml version='1.0' encoding='UTF-8'?><range><min>"+tmp+"</min><max>"+tmp2+"</max></range>")
                        print "Range sent."

                    #list the array
                    if cmd == "-rn":
                        largest = int(command(mrecv2,"<curmax>"))
                        rangearr(mrecv2,arr)
                        print "Current max value:", largest
                        allperfect.extend(arr)
                        print "Printing number prefect : ",allperfect
                        #http://stackoverflow.com/questions/2612802/how-to-clone-or-copy-a-list-in-python
                    #send data
                    if cmd == "-s":
                        print "Reporting Values"
                        sendstr = "<?xml version='1.0' encoding='UTF-8'?><query><action>-rn</action><data><curmax>"+str(largest)+"</curmax><numbers>"
                        for x in xrange(len(allperfect)):
                            if allperfect[x] != 0: 
                                sendstr += "<pn>"+str(allperfect[x])+"</pn>"
                        sendstr += "</numbers></data></query>"
                        s.send(sendstr)
                    
                    #kill the program
                    if cmd == "-k":
                        for control in handle:
                            control.send("<?xml version='1.0' encoding='UTF-8'?><server><action>-k</action></server>")
                        print("Exiting")
                        sys.exit()
                        
                else:
                    con = disconn(s)
                    print "Disconnecting: No data recieved from ",con
                    s.close()
                    inputsock.remove(s)
            #http://stackoverflow.com/questions/20471816/how-does-the-select-function-in-the-select-module-of-python-exactly-work
        
def main():
    #signals
    intisig()
    #start the manage server
    msockfd = intiserver()
    #start connection loop
    print "Waiting for Client..."
    connectionloop(msockfd)
    #https://docs.python.org/2/howto/sockets.html

if __name__== "__main__":
    main()

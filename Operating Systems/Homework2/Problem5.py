# Pavin Disatapundhu
# disatapp@onid.oregonstate.edu
# CS344-400
# homework#2 Question5
import sys
import getopt
import subprocess

def main(argv):
    opts, args = getopt.getopt(argv, 'bdlprTtu', ['boot', 'dead', 'login', 'process','runlevel', 'mesg', 'time', 'users'])
    user = ''
    for opt, arg in opts:
        user += opt
    print unixpipe(user)

def unixpipe(clara):
    doctor = 'who '
    if clara:
        doctor += clara          
    pipes = subprocess.Popen( doctor, stdin= subprocess.PIPE, stdout = subprocess.PIPE, shell = True)
    display = pipes.communicate()[0]
    return display

if __name__ == '__main__':
    main(sys.argv[1:])

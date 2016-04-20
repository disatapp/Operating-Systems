# Pavin Disatapundhu
# disatapp@onid.oregonstate.edu
# CS344-400
# homework#2 Question2
import os
import sys
import getopt

def main(argv):
    opts, args = getopt.getopt(argv, 't:c:', ['term=', 'class='])
    termn = ""
    classn = ""
    for opt, arg in opts:
        if opt in ('-t', '--term'):
            termn = arg
        if opt in ('-c', '--class'):
            classn = arg
    if termn and classn:
        dircreate(termn, classn)
    else:
        sys.exit('Invalid argument')

def createdir(path):
    if not os.path.isdir(path):
        os.makedirs(path)
    else:
        print "The path " + path + " already exist."
            
def linkdir(sl,arg1,arg2):
    try:
        os.path.islink(sl)
        if not os.path.islink(sl):
            os.symlink(arg1,arg2)
    except OSError, e:
        sys.exit(str(e))
        
        
def dircreate(termn, classn):
    folders = "assigments examples exams lecture_notes submissions".split()
    homefolder = os.environ['HOME']+"/{0}/{1}".format(termn, classn)
    README = "/usr/local/classes/eecs/{0}/{1}/src".format(termn, classn)

    plp = homefolder + '/README.txt'
    slp = homefolder + '/src_class'


    #create dir
    createdir(homefolder)
    createdir(README)
    open(README + '/README.txt', 'a').close()
    
    #makedir
    for i in folders:
        newdir = homefolder + '/{0}' .format(i)
        if not os.path.isdir(newdir):
            os.mkdir(newdir)
        else:
            print "The path " + newdir + " already exist."
    
    linkdir(plp, README +'/README.txt',  plp)
    linkdir(slp, README , slp)



if __name__ == "__main__":
    main(sys.argv[1:])

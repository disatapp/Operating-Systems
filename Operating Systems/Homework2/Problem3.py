# Pavin Disatapundhu
# disatapp@onid.oregonstate.edu
# CS344-400
# homework#2 Question3
import os
import sys
import urllib2
import urlparse

def main(argv):
    win = ''
    wout = ''
    infolder = os.environ['HOME']+"/Downloads"
    if len(argv) == 3:
        win = argv[1]
        wout = argv[2]

    if not os.path.isdir(infolder):
        os.mkdir(infolder)
        print 'New download folder created!!'
    if win and wout:
        wget(win, wout, infolder)
    else:
        sys.exit('Invalid: please enter 2 arguments.')

def rephrase(url):
    if not urlparse.urlparse(url).scheme:
        url = "http://"+url
    return url

def wget(url, outfile, inf):
    fname = outfile
    try:
        w = urllib2.urlopen(rephrase(url))
        d = w.read()
        output = open(inf + "/" + outfile, "wb")
        output.write(d)
        print "Write success!! Find file in /User/<name>/Downloads folder"
    except urllib2.HTTPError, e:
        sys.exit(str(e))
    except urllib2.URLError, e:
        sys.exit(str(e))
 

if __name__ == "__main__":
    main(sys.argv)

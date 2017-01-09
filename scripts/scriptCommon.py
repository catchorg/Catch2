import os
import sys
import subprocess


catchPath = os.path.dirname(os.path.realpath( os.path.dirname(sys.argv[0])))

def getBuildExecutable():
    dir = os.environ.get('CATCH_DEV_OUT_DIR', "cmake-build-debug/SelfTest")
    return dir

def runAndCapture( args ):
    child = subprocess.Popen(" ".join( args ), shell=True, stdout=subprocess.PIPE)
    lines = []
    line = ""
    while True:
        out = child.stdout.read(1)
        if out == '' and child.poll() != None:
            break
        if out != '':
            if out == '\n':
                lines.append( line )
                line = ""
            else:
                line = line + out
    return lines

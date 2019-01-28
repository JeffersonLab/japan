#!/usr/bin/python

import os
import sys

presentcwd = os.getcwd()
os.chdir(sys.argv[1])


f = os.popen("git remote -v && git log -n 1 && git status -bs && echo \"  ROOT version\" `root-config --version` && echo \" `cmake --version`\" && echo \"\nGenerated at `date`\"")

boringstring = "";

if( f != 0):
    for line in f:
	boringstring += line
else:
    boringstring = "git information unavailable"

boringstring += "Source dir " + os.getcwd()
boringstring += "\nBuild  dir " + presentcwd + "\n"

newheadertext = """#ifndef __GITINFO_HH
#define __GITINFO_HH

/*
    Generated automatically by cmake process
*/

const char* const gGitInfo = R\"gitinfo(
""" + boringstring + """
)gitinfo\";

#endif//__GITINFO_HH
"""


os.chdir(presentcwd)

outdir = "include/"
if not os.path.exists(outdir):
    os.makedirs(outdir)

newheader = open( outdir + "/gitinfo.hh", "w")
newheader.write(newheadertext)
newheader.close()

print "Repository information\n", boringstring

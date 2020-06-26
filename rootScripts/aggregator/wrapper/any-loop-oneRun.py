import os
import sys
import csv
import subprocess
from argparse import ArgumentParser

parser     = ArgumentParser()
parser.add_argument("-f", "--conf", dest="devicelist", help="Device List and Analyses for camguin", required=True, metavar="CONF", default="input.txt")
parser.add_argument("-r", "--run", dest="runnumber", help="Run number", required=True, metavar="RUN", default="1")
parser.add_argument("-b", "--basename", dest="basename", help="Base ROOT File name", metavar="BASENAME", default="prexPrompt_pass2")
parser.add_argument("-F", "--fullruns", dest="fullruns", help="Do mini runs flag", metavar="FULLRUNS", default="0")
#parser.add_argument("-s", "--slug", dest="slug", help="slug flag", metavar="SLUG", default="-1")

args       = vars(parser.parse_args())
run        = int(args['runnumber'])
devicelist = args['devicelist']
basename   = args['basename']
fullruns   = args['fullruns']
#slug       = args['slug']
# Get slug: 
cmds = ['rcnd',str(run),'slug']
cond_out = "NULL"
cond_out = subprocess.Popen(cmds, stdout=subprocess.PIPE).stdout.read().strip().decode('ascii') # Needs to be decoded... be careful 
slug = float(cond_out)
print("The slug is slug: "+str(slug))

# Add a layer of for loop over the number of minirun - obtained from a new camguin method to ask how many miniruns
# minirun loop simply updates the MINIRUNNUM environment variable as it loops through.
# Alternate solution is to procedurally generate the cut string: "minirun==# && ok_cut==1",1 (where 1 is the overwrite cut telling Device_Error_Code to go away)
print("Extracting from run " + str(run))
cmds = ['root','-l','-q','-b','-L','getNMiniruns_japan.C("'+str(run)+'")']
nMiniRuns = 0
output = "NULL"
output = subprocess.Popen(cmds, stdout=subprocess.PIPE).stdout.read().strip().decode('ascii') # Needs to be decoded... be careful
print(output)
for each in output.split('\n'):
  #print(each)
  if each.split('=')[0] == "NMiniruns" and len(each.split('='))>1:
    nMiniRuns = int(each.split('=')[1])
    print("Found "+str(nMiniRuns)+" miniruns")
start = -1
if int(fullruns) == 0:
  start = 0
  for mini in range(start,nMiniRuns):
    print("Looking at mini run # = "+str(mini))
    #os.system("./wrapper.sh -f "+str(devicelist)+" -r "+str(run)+" -m "+str(mini)+" -s 000 -n "+str(slug)+" -b "+str(basename)+" ")
    os.system("root -l -b -q ../camDataFrame.C'(\""+str(run)+"\",\""+str(slug)+"\",\""+str(mini)+"\",\"000\",\""+str(devicelist)+"\",\""+str(basename)+"\")'")
    #echo -e "Done run: $RUNNUM, minirun: $MINIRUNNUM, slug: $NRUNS"
  print("\n\n\n\nCompletely Done with miniruns for run {}\n\n".format(run))
else: # Do full run only (obviously this can be editted to do both in one go... but people want them separate - once we get the agg-rootfile names done correctly we can handle this internally to camguin)
    print("Looking at Full run")
    #os.system("./wrapper.sh -f "+str(devicelist)+" -r "+str(run)+" -m "+str(-1)+" -s 000 -n "+str(slug)+" -b "+str(basename)+" ")
    os.system("root -l -b -q ../camDataFrame.C'(\""+str(run)+"\",\""+str(slug)+"\",\"-1\",\"000\",\""+str(devicelist)+"\",\""+str(basename)+"\")'")
    print("\n\n\n\nDone with run {}\n\n".format(run))

import os
import sys
import csv
import subprocess
from argparse import ArgumentParser

parser     = ArgumentParser()
parser.add_argument("-f", "--conf", dest="devicelist", help="Device List and Analyses for camguin", required=True, metavar="CONF", default="input.txt")
parser.add_argument("-r", "--runs", dest="runlist", help="Run List text file for camguin wrapper", required=True, metavar="RUNS", default="test")
parser.add_argument("-F", "--fullruns", dest="fullruns", help="Do mini runs flag", metavar="FULLRUNS", default="0")

args       = vars(parser.parse_args())
runlist    = args['runlist']
devicelist = args['devicelist']
fullruns = args['fullruns']

lines = []

try:
  with open(runlist) as runlistFile:
    for line in csv.reader(runlistFile, delimiter=','):
      lines.append(line[0])
except:
  print("ERROR: You forgot to have a runlist \"runlist/"+runlist+".txt\"\n")
  sys.exit()
    
#f=open("runlist/"+runlist+".txt")
#lines=[3348]
#lines=[3344,3343]#,3342,3324,3323,3322,3321,3320,3319,3318,3316,3308,3305,3289]
#lines=[3370,3369,3368,3366,3365,3364,3363,3358,3357,3356,3355,3354,3353,3352,3351,3350,3349,3348,3347,3346]

for line in lines:
  # FIXME need to obtain number of miniruns from a root macro looking at post pan file, then loop over them + the full run (changing the cut)
  # Add a layer of for loop over the number of minirun - obtained from a new camguin method to ask how many miniruns
  # minirun loop simply updates the MINIRUNNUM environment variable as it loops through.
  # Alternate solution is to procedurally generate the cut string: "minirun==# && ok_cut==1",1 (where 1 is the overwrite cut telling Device_Error_Code to go away)
  print("Extracting from run " + line)
  cmds = ['root','-l','-q','-b','-L','getNMiniruns_postpan.C("'+line+'")']
  nMiniRuns = -1
  output = "NULL"
  output = subprocess.Popen(cmds, stdout=subprocess.PIPE).stdout.read().strip().decode('ascii') # Needs to be decoded... be careful
  print(output)
  for each in output.split('\n'):
    print(each)
    if each.split('=')[0] == "NMiniruns" and len(each.split('='))>1:
      nMiniRuns = int(each.split('=')[1])
      print("Found "+str(nMiniRuns)+" miniruns")
  start = -1
  if int(fullruns) == 0:
    start = 0
    for mini in range(start,nMiniRuns):
      print("Looking at mini run # = "+str(mini))
      os.system("./wrapper.sh -f "+str(devicelist)+" -r "+str(line)+" -m "+str(mini)+" -s 000 -n 1") # FIXME This -n 1 should be the Slug Number
  else: # Do full run only (obviously this can be editted to do both in one go... but people want them separate - once we get the agg-rootfile names done correctly we can handle this internally to camguin)
      print("Looking at Full run")
      os.system("./wrapper.sh -f "+str(devicelist)+" -r "+str(line)+" -m "+str(-1)+" -s 000 -n 1")

#os.system("mv run_aggregator.root run_aggregator_"+runlist+"_regress.root")
#os.system("mv run_aggregator_"+runlist+"_regress.root ../")

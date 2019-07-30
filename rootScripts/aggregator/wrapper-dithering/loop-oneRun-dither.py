import os
import sys
import csv
import subprocess
from argparse import ArgumentParser

parser     = ArgumentParser()
parser.add_argument("-f", "--conf", dest="devicelist", help="Device List and Analyses for camguin", required=True, metavar="CONF", default="input.txt")
parser.add_argument("-r", "--run", dest="runnumber", help="Run number", required=True, metavar="RUN", default="1")
#parser.add_argument("-s", "--slug", dest="slug", help="slug flag", metavar="SLUG", default="-1")

args       = vars(parser.parse_args())
run        = int(args['runnumber'])
devicelist = args['devicelist']
fullruns   = args['fullruns']
#slug       = args['slug']
# Get slug: 
cmds = ['rcnd',str(run),'slug']
cond_out = "NULL"
cond_out = subprocess.Popen(cmds, stdout=subprocess.PIPE).stdout.read().strip().decode('ascii') # Needs to be decoded... be careful 
slug = int(cond_out)
print("The slug is slug: "+str(slug))

print("Extracting from run " + str(run))
  os.system("./wrapper_dithering.sh -f "+str(devicelist)+" -r "+str(run)+" -s 000 -n "+str(slug)) # FIXME backgrounding here so that it will do all miniruns at once

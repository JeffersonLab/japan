import os
import sys


runlist=sys.argv[1]

f=open("runlist/"+runlist+".txt")
lines=[3344,3343,3342,3324,3323,3322,3321,3320,3319,3318,3316,3308,3305,3289]
#lines=[3370,3369,3368,3366,3365,3364,3363,3358,3357,3356,3355,3354,3353,3352,3351,3350,3349,3348,3347,3346]

for line in lines:
  print(line)
  os.system("./wrapper.sh -f input.txt -r "+str(line)+" -s 000 -n 1")



os.system("mv run_aggregator.root run_aggregator_"+runlist+"uA_regress.root")
os.system("mv run_aggregator_"+runlist+"uA_regress.root ../")

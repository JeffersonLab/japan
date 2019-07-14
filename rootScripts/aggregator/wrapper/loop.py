import os
import sys


current=sys.argv[1]

if current=="scan":
  run=[2962, 2948, 2947, 2946, 2945]
if current=="pb208":
  run=[3134,3135,3136,3137,3138,3139,3140,3141,3142,3143]
if current=="50":
  run=[1315,1316,1317,1319,1320,1321,1352,1353,1369,1377,1457,1458,1459,1460]
if current=="30":
  run=[1354,1356,1357,1368,1370,1371,1372,1373,1374,1375,1376,1378,1379,1386,1387,1388,1391,1423]



for i in run:
  os.system("./wrapper.sh -f input.txt -r "+str(i)+" -s 000 -n 1")



os.system("mv run_aggregator.root run_aggregator_"+current+"uA_regress.root")
os.system("mv run_aggregator_"+current+"uA_regress.root ../")

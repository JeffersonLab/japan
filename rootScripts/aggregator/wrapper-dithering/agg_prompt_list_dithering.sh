#! /bin/sh
while IFS= read -r line; do
    runnum=$line;
    timenow=$(date +"%Y-%m%d-%H%M");
    ./aggregator_list_dithering.sh $runnum # >& ./LogFiles/Camguin_run$runnum\_$timenow.txt
done < $1


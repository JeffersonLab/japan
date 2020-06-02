#!/bin/bash


usage()
{ 
  [ $i -eq 2 ] && echo "Please enter the name of the aggregated rootfile."
	[ $1 -eq 3 ] && echo "Please enter input file name."
  [ $1 -eq 4 ] && echo "./twoDwrapper.sh -a run_aggregator_50uA_regress.root-f twoDinput.txt"
	exit $1
}

unset CONFIG RUNNUM NRUNS

while getopts ':a:f:h' c
do
  case $c in
    a) AGG=$OPTARG ;;
    f) CONFIG=$OPTARG ;;
    h) usage 4 ;;  
  esac
done

[ -z "$AGG" ] && usage 2 
[ -z "$CONFIG" ] && usage 3


n=1
while read line; do
  # reading each line
  root -q -b -L twoDagg.C"($line,\"$AGG\")"
  n=$((n+1))
done < $CONFIG


convert *.png ${AGG%%.root*}.pdf
rm *.png

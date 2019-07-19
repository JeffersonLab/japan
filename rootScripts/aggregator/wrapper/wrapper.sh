#!/bin/bash


usage()
{ 
	[ $1 -eq 2 ] && echo "Please enter runnumber."
  [ $1 -eq 3 ] && echo "Please enter splitnumber."
  [ $1 -eq 4 ] && echo "Please enter input file name."
  [ $1 -eq 5 ] && echo "Please enter number of miniruns."
  [ $1 -eq 6 ] && echo "./wrapper.sh -f input.txt -r 1296 -s 000 -n 1"
	exit $1
}

unset CONFIG RUNNUM NRUNS

while getopts ':r:s:f:n:h' c
do
  case $c in
    r) RUNNUM=$OPTARG ;;
    s) SPLITNUM=$OPTARG;;
    f) CONFIG=$OPTARG ;;
    n) NRUNS=$OPTARG ;;
    h) usage 5 ;;  
  esac
done

[ -z "$RUNNUM" ] &&  usage 2
[ -z "$SPLITNUM" ] && usage 3
[ -z "$CONFIG" ] && usage 4
[ -z "$NRUNS" ] && usage 5

while read line; do
  # reading each line
  root -q -b -L ../camguin.C"($line,$RUNNUM,$SPLITNUM,$NRUNS)"
done < $CONFIG

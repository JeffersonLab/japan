#!/bin/bash
CONFIG="default.cfg"
RUNNUM=0
NRUNS=1
PRINT=0
if [ $# -eq 0 ]
then
  echo "Please enter run number"
	read RUNNUM
else
  args=("$@")
  declare -i argN=1
  for arg in "$@"
  do
    case "$arg" in
      -r)
        RUNNUM=${args[$argN]}
        argN+=2
        ;;
      -f)
        CONFIG=${args[$argN]}
        argN+=2
        ;;
      -n)
        NRUNS=${args[$argN]}
        argN+=2
        ;;
      -P)
        PRINT=1
        argN+=1
        ;;
    esac
  done
#
#  if [ "$1" == "-r" ]
#  then
#    RUNNUM=$2
#    if [ "$3" == "-f" ]
#    then 
#      CONFIG=$4
#    fi
#  elif [ "$1" == "-f" ]
#  then 
#    CONFIG=$2
#    if [ "$3" == "-r" ]
#    then 
#      RUNNUM=$4
#    fi
#  fi
fi

if [ $# -gt 6 ]
then 
  if [ "$7" == "-P" ]
  then 
    PRINT=1
  else
	  echo " "
	  echo  "Should only enter parameters for runnumber and set-up"
	  echo "./panguin_wrapper -r RUNNUMBER -f CONFIG"
	  exit 1
  fi
fi

echo "CONFIG equals $CONFIG"
echo "RUNNUM equals $RUNNUM"
echo "NRUNS equals $NRUNS"

#touch sourceofanalysis.txt
#echo $PWD  ./haonline_tool -r $RUNNUM -s $CONFIG -o -y > "sourceofanalysis.txt"

if [ $PRINT -eq 1 ]
then
  root -L -l -p -q camguin.C'("postpan")' 
elif [ $PRINT -eq 0 ]
then
  root -L -l -p -q camguin.C'("postpan")' 
fi


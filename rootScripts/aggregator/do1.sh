#!/bin/bash

export ROOTSYS="/adaqfs/apps/ROOT/6.16-00"
export PATH="${ROOTSYS}/bin:${PATH}"
export LD_LIBRARY_PATH="${ROOTSYS}/lib:${LD_LIBRARY_PATH}"
export CAM_OUTPUTDIR="/adaqfs/home/apar/PREX/prompt/aggRootfiles/"
export CAM_OUTPUTDIR="plots/hist/" 

slug=`rcnd $1 slug`
mini=$2
input=$3
if [ $# -lt 1 ]
then
  echo "Must pass run number"
  exit
fi
if [ $# -lt 2 ]
then
  mini=-1
fi
if [ $# -lt 2 ]
then
  input="wrapper/inputs/cut_check_input_graphs.txt"
fi
export CAM_OUTPUTDIR="/chafs1/work1/apar/cameronc/plots/DF-plotter/"

root -l -q -b camDataFrame.C'("'$1'","'$slug'","'$mini'","000","'$input'","testHoldoff_120_pass2")' 
root -l -q -b camDataFrame.C'("'$1'","'$slug'","'$mini'","000","'$input'","testHoldoff_240_pass2")' 
root -l -q -b camDataFrame.C'("'$1'","'$slug'","'$mini'","000","'$input'","testHoldoff_480_pass2")' 
root -l -q -b camDataFrame.C'("'$1'","'$slug'","'$mini'","000","'$input'","testHoldoff_1000_pass2")' 

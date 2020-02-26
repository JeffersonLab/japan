#!/bin/bash

export ROOTSYS="/adaqfs/apps/ROOT/6.16-00"
export PATH="${ROOTSYS}/bin:${PATH}"
export LD_LIBRARY_PATH="${ROOTSYS}/lib:${LD_LIBRARY_PATH}"
export CAM_OUTPUTDIR="/adaqfs/home/apar/PREX/prompt/aggRootfiles/"
export CAM_OUTPUTDIR="plots/hist/" 

slug=`rcnd $1 slug`
export CAM_OUTPUTDIR="/chafs1/work1/apar/cameronc/plots/DF-plotter/"

root -l -q -b camDataFrame.C'("'$1'","'$slug'","-1","000","wrapper/inputs/cut_check_input_graphs.txt","testHoldoff_120_pass2")' 
root -l -q -b camDataFrame.C'("'$1'","'$slug'","-1","000","wrapper/inputs/cut_check_input_graphs.txt","testHoldoff_240_pass2")' 
root -l -q -b camDataFrame.C'("'$1'","'$slug'","-1","000","wrapper/inputs/cut_check_input_graphs.txt","testHoldoff_480_pass2")' 
root -l -q -b camDataFrame.C'("'$1'","'$slug'","-1","000","wrapper/inputs/cut_check_input_graphs.txt","testHoldoff_1000_pass2")' 

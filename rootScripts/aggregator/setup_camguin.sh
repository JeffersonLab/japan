#!/bin/bash
export ROOTSYS=/adaqfs/apps/ROOT/6.16-00
export PATH="${ROOTSYS}/bin:${PATH}"
export LD_LIBRARY_PATH="${ROOTSYS}/lib:${LD_LIBRARY_PATH}"
export RUNNUM=1
export NRUNS=1
export SPLITNUM=0
export MINIRUNNUM=-1
export CAM_DEBUG=0
export CAM_ALARM=1
export CAM_AGGREGATE=1
export CAM_POSTPAN=1
if [ -z "$CAM_OUTPUTDIR" ]; then
  export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/
fi
export POSTPAN_ROOTFILES=/chafs2/work1/apar/postpan-outputs/
#export DITHERING_ROOTFILES=/chafs2/work1/apar/DitherCorrection/
#export DITHERING_ROOTFILES_SLOPES=/chafs2/work1/apar/BMODextractor/
export LRB_ROOTFILES=/chafs2/work1/apar/LRBoutput/
export RCDB_CONNECTION=mysql://rcdb@hallcdb.jlab.org:3306/a-rcdb

#!/bin/bash


usage()
{ 
	[ $1 -eq 2 ] && echo "Please enter runnumber."
  [ $1 -eq 3 ] && echo "Please enter minirunnumber."
  [ $1 -eq 4 ] && echo "Please enter splitnumber."
  [ $1 -eq 5 ] && echo "Please enter input file name."
  [ $1 -eq 6 ] && echo "Please enter number of miniruns."
  [ $1 -eq 7 ] && echo "./wrapper.sh -f input.txt -r 1296 -s 000 -n 1"
	exit $1
}

source ../setup_camguin.sh

unset CONFIG RUNNUM NRUNS

while getopts ':r:m:s:f:n:h' c
do
  case $c in
    r) RUNNUM=$OPTARG ;;
    m) MINIRUNNUM=$OPTARG ;;
    s) SPLITNUM=$OPTARG;;
    f) CONFIG=$OPTARG ;;
    n) NRUNS=$OPTARG ;;
    h) usage 5 ;;  
  esac
done

[ -z "$RUNNUM" ] &&  usage 2
[ -z "$MINIRUNNUM" ] && usage 3
[ -z "$SPLITNUM" ] && usage 4
[ -z "$CONFIG" ] && usage 5
[ -z "$NRUNS" ] && usage 6

if [ $CONFIG = "PQB_input.txt" ];
then
  export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/PQB/
fi

if [ $CONFIG = "dithering_input.txt" ];
then
  export DITHERING_ROOTFILES=/chafs2/work1/apar/DitherCorrection/
  export DITHERING_ROOTFILES_SLOPES=/chafs2/work1/apar/BMODextractor/
  export DITHERING_ROOTFILES_SLOPES_RUN=/chafs1/work1/apar/cameronc/WAC/backups/dithering_slopes_backups
  export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/dithering/
fi

root -l -q -b ../camDataFrame.C"(\"$RUNNUM\",\"$NRUNS\",\"$MINIRUNNUM\",\"$SPLITNUM\",\"$CONFIG\")"
echo -e "Done with run: $RUNNUM, minirun: $MINIRUNNUM, slug: $NRUNS"

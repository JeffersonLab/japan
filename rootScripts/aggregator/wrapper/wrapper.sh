#!/bin/bash


usage()
{ 
	[ $1 -eq 2 ] && echo "Please enter runnumber."
  [ $1 -eq 3 ] && echo "Please enter minirunnumber."
  [ $1 -eq 4 ] && echo "Please enter splitnumber."
  [ $1 -eq 5 ] && echo "Please enter input file name."
  [ $1 -eq 6 ] && echo "Please enter number of miniruns."
  [ $1 -eq 7 ] && echo "Please enter the base ROOT file name."
  [ $1 -eq 8 ] && echo "./wrapper.sh -f input.txt -r 1296 -s 000 -n 1"
	exit $1
}

source ../setup_camguin.sh

unset CONFIG RUNNUM NRUNS

while getopts ':r:m:s:f:n:b:h' c
do
  case $c in
    r) RUNNUM=$OPTARG ;;
    m) MINIRUNNUM=$OPTARG ;;
    s) SPLITNUM=$OPTARG;;
    f) CONFIG=$OPTARG ;;
    n) NRUNS=$OPTARG ;;
    b) BASENAME=$OPTARG ;;
    h) usage 5 ;;  
  esac
done

[ -z "$RUNNUM" ] &&  usage 2
[ -z "$MINIRUNNUM" ] && usage 3
[ -z "$SPLITNUM" ] && usage 4
[ -z "$CONFIG" ] && usage 5
[ -z "$NRUNS" ] && usage 6
[ -z "$BASENAME" ] && usage 7

if [[ "$CONFIG" == "PQB_input.txt" ]];
then
  export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/PQB/
fi

if [[ "$CONFIG" == "burp_input.txt" ]];
then
  export QW_ROOTFILES=/aonl1/work1/quinn/
  #export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/burp/
fi

if [[ "$CONFIG" == "AT_input.txt" ]];
then
  echo "PREX Regression AT"
  export QW_ROOTFILES=/lustre/expphy/volatile/halla/parity/japanOutput/respin1-transverse-22March/
  export POSTPAN_ROOTFILES=/lustre/expphy/volatile/halla/parity/postpan_respin/
  export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/PREX_regression_AT/
fi

if [[ "$CONFIG" == "SAMs_input.txt" ]];
then
  echo "Doing SAM analysis"
  export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/SAMs/
  unset DITHERING_ROOTFILES
  unset DITHERING_ROOTFILES_SLOPES
fi

if [[ "$CONFIG" == "dithering_input."* ]];
then
  echo "Doing Dithering analysis"
  export DITHERING_ROOTFILES=/chafs2/work1/apar/DitherCorrection/
  export DITHERING_ROOTFILES_SLOPES=/chafs2/work1/apar/BMODextractor/
  export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/dithering/
  export DITHERING_STUB=""
fi

if [[ "$CONFIG" == "dithering_1X_input."* ]];
then
  echo "Doing Dithering_1X analysis"
  export DITHERING_ROOTFILES=/chafs2/work1/apar/DitherCorrection/
  export DITHERING_ROOTFILES_SLOPES=/chafs2/work1/apar/BMODextractor/
  export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/dithering_1X/
  export DITHERING_STUB="_1X"
fi

if [[ "$CONFIG" == "dithering_input_PREX_AT."* ]];
then
  echo "Doing Dithering_1X analysis"
  export DITHERING_ROOTFILES=/chafs2/work1/apar/DitherCorrection/
  export DITHERING_ROOTFILES_SLOPES=/chafs2/work1/apar/BMODextractor/
  export CAM_OUTPUTDIR=/chafs2/work1/apar/aggRootfiles/dithering_AT/
  export DITHERING_STUB="_AT"
  export POSTPAN_ROOTFILES=/lustre/expphy/volatile/halla/parity/postpan_respin/
fi

root -l -q -b ../camDataFrame.C"(\"$RUNNUM\",\"$NRUNS\",\"$MINIRUNNUM\",\"$SPLITNUM\",\"$CONFIG\",\"$BASENAME\")"
echo -e "Done run: $RUNNUM, minirun: $MINIRUNNUM, slug: $NRUNS"

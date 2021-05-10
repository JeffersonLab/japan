#! /bin/sh
if [[ "$CAM_OUTPUTDIR" == "" ]]; then
  # Default case, no environment set up yet
  export CAM_OUTPUTDIR="/chafs2/work1/apar/aggRootfiles"
fi
if [[ "$COILS" == "" ]]; then 
  export COILS="13746"
fi
# Default assumed outputs
CAM_OUTPUT="/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes${DITHERING_STUB}/dithering_slopes_${COILS}_slug*_run#.root"
NEW_CAM_OUTPUT="/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes${DITHERING_STUB}/"
if [ "$#" -lt 1 ]; then
  echo "Error: Need to pass a list file name"
  echo "Usage: ./smartHadd_miniruns_regression.sh list.txt (input file list, \\n separated) outputFileName (a name..., optional argument)"
else
  if [ "$#" -eq 3 ]; then
    # Special case, user defined output
    CAM_OUTPUT="${3}/dithering_slopes_${COILS}_slug*_run#.root"
    NEW_CAM_OUTPUT="${3}/"
  fi
  IFS=$'\n' read -d '' -r -a lines < ${1}
  # all lines
  echo "Hadding all runs: ${lines[@]}"
  #minirun_lines=("${lines[@]}")
  minirun_lines=()
  #run_lines=("${lines[@]}")
  num_lines=0
  num_miniruns=0

  # Do miniruns
  #for i in "${!lines[@]}"; do
  #  #  run_temp="${CAM_OUTPUT}/grandRootfile_${lines[$i]}/grand_aggregator.root"
  #  #run_lines[$i]=$run_temp
  #  num_lines=$i
  #done
  #
  #slug=`rcnd ${lines[0]} slug`
  name=dithering_slopes_full_CREX.root
  if [ "$#" -ge 2 ]; then
    name=$2
  fi
  root -l -b -q $JAPAN_DIR/rootScripts/merger/smartHadd_dit.C\(\"${1}\",\"${CAM_OUTPUT}\",\"${NEW_CAM_OUTPUT}/${name}\",\"run\",\"cyclenum\",\"dit\"\)

  # Add units
  #root -l -b -q $JAPAN_DIR/rootScripts/aggregator/wrapper/addUnits.C\(\"${NEW_CAM_OUTPUT}/${name}\",\"dit\"\)
fi

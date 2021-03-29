#! /bin/sh
if [[ "$CAM_OUTPUTDIR" == "" ]]; then
  # Default case, no environment set up yet
  export CAM_OUTPUTDIR="/chafs2/work1/apar/aggRootfiles"
fi
# Default assumed outputs
CAM_OUTPUT="${CAM_OUTPUTDIR}/slugRootfiles/grandRootfile_#/grand_aggregator.root"
NEW_CAM_OUTPUT="${CAM_OUTPUTDIR}/slugRootfiles/grandRootfile"
if [ "$#" -lt 1 ]; then
  echo "Error: Need to pass a list file name number \"slug##\".list"
  echo "Usage: ./smartHadd_slug_regression.sh list.txt (input file list, \\n separated) outputFileName (a name..., optional argument)"
else
  if [ "$#" -eq 3 ]; then
    # Special case, user defined output
    CAM_OUTPUT="${3}/slugRootfiles/grandRootfile_#/grand_aggregator.root"
    NEW_CAM_OUTPUT="${3}/slugRootfiles/grandRootfile"
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
  for i in "${!lines[@]}"; do
  #  run_temp="${CAM_OUTPUT}/grandRootfile_${lines[$i]}/grand_aggregator.root"
    #run_lines[$i]=$run_temp
    num_lines=$i
  done

  name="${lines[0]}-${lines[$num_lines]}"
  if [ "$#" -ge 2 ]; then
    name=$2
  fi
  root -l -b -q ${JAPAN_DIR}/rootScripts/merger/smartHadd.C\(\"${1}\",\"${CAM_OUTPUT}\",\"${NEW_CAM_OUTPUT}/grand_${name}.root\",\"n_runs\",\"run_number\",\"agg\"\)

  # Add units
  root -l -b -q ${JAPAN_DIR}/rootScripts/aggregator/wrapper/addUnits.C\(\"${NEW_CAM_OUTPUT}/grand_${name}.root\"\)
fi

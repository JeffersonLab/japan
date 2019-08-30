#! /bin/sh
ROOTFILEDIR="/chafs2/work1/apar/aggRootfiles/slugRootfiles/dithering/"
OUTPUTROOTFILEDIR="/chafs2/work1/apar/aggRootfiles/slugRootfiles/dithering/grandRootfile/"
#OUTPUTROOTFILEDIR="./"
if [ "$#" -lt 1 ]; then
  echo "Error: Need to pass a list file name number \"slug##\".list"
  echo "Usage: ./dithering_accumulate_aggFiles_list.sh list.txt (input file list, \\n separated)"
else
  IFS=$'\n' read -d '' -r -a lines < ${1}
  # all lines
  echo "Hadding all runs: ${lines[@]}"
  #minirun_lines=("${lines[@]}")
  minirun_lines=()
  run_lines=("${lines[@]}")
  num_lines=0
  num_miniruns=0

  # Do miniruns
  for i in "${!lines[@]}"; do
    run_temp="${ROOTFILEDIR}/grandRootfile_${lines[$i]}/grand_aggregator.root"
    run_lines[$i]=$run_temp
    num_lines=$i
  done

  hadd -f ${OUTPUTROOTFILEDIR}/grand_${lines[0]}-${lines[$num_lines]}.root ${run_lines[@]}

  # Add units
  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/grand_${lines[0]}-${lines[$num_lines]}.root\"\)

fi


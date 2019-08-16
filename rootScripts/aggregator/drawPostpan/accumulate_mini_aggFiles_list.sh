#! /bin/sh
ROOTFILEDIR="/chafs2/work1/apar/aggRootfiles"
OUTPUTROOTFILEDIR="/chafs2/work1/apar/aggRootfiles/slugRootfiles"
#OUTPUTROOTFILEDIR="./"
if [ "$#" -lt 1 ]; then
  echo "Error: Need to pass a list file name number \"slug##\".list"
  echo "Usage: ./accumulate_aggFiles_list.sh list.txt (input file list, \\n separated)"
else
  IFS=$'\n' read -d '' -r -a lines < run_list/${1}.list
  # all lines
  echo "Hadding all runs: ${lines[@]}"
  #minirun_lines=("${lines[@]}")
  minirun_lines=()
  run_lines=("${lines[@]}")
  num_miniruns=0

  # Do miniruns
  for i in "${!lines[@]}"; do
    run_temp="${ROOTFILEDIR}/run_aggregator_${lines[$i]}.root"
    run_lines[$i]=$run_temp
    shopt -s nullglob
    miniarray=(${ROOTFILEDIR}/minirun_aggregator_${lines[$i]}_*.root)
    shopt -u nullglob
    #"${miniarray[@]}"
    for j in "${!miniarray[@]}"; do
      echo ${miniarray[$j]}
      minirun_lines[$num_miniruns]=${miniarray[$j]}
      let num_miniruns++
    done
    #minirun_temp="${ROOTFILEDIR}/minirun_aggregator_${lines[$i]}.root"
    #minirun_lines[$i]=$minirun_temp
  done

  echo Looking at ${minirun_lines[@]}
  hadd -f ${OUTPUTROOTFILEDIR}/minirun_${1}.root ${minirun_lines[@]}
  hadd -f ${OUTPUTROOTFILEDIR}/run_${1}.root ${run_lines[@]}

  # Add units
  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/minirun_${1}.root\"\)
  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/run_${1}.root\"\)

fi


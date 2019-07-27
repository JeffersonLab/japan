#! /bin/sh
ROOTFILEDIR="/chafs2/work1/apar/aggRootfiles"
OUTPUTROOTFILEDIR="/chafs2/work1/apar/aggRootfiles/slugRootfiles"
if [ "$#" -lt 2 ]; then
  echo "Error: Need to pass a runlist and output rootfile name as arguments"
  echo "Usage: ./accumulate_aggFiles_list.sh list.txt (input file list, \\n separated,) output.root (what you want the output to be called)"
else
  IFS=$'\n' read -d '' -r -a lines < $1
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
  hadd -f ${OUTPUTROOTFILEDIR}/minirun_${2} ${minirun_lines[@]}
  hadd -f ${OUTPUTROOTFILEDIR}/run_${2} ${run_lines[@]}

  # Add units
  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/minirun_${2}\"\)
  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/run_${2}\"\)

fi


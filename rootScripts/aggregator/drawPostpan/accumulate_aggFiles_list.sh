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
  minirun_lines=("${lines[@]}")
  run_lines=("${lines[@]}")

  # Do miniruns
  for i in "${!lines[@]}"; do
    minirun_temp="${ROOTFILEDIR}/minirun_aggregator_${lines[$i]}.root"
    run_temp="${ROOTFILEDIR}/run_aggregator_${lines[$i]}.root"
    minirun_lines[$i]=$minirun_temp
    run_lines[$i]=$run_temp
  done

  hadd -f ${OUTPUTROOTFILEDIR}/minirun_${2} ${minirun_lines[@]}
  hadd -f ${OUTPUTROOTFILEDIR}/run_${2} ${run_lines[@]}

  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/minirun_${2}\"\)
  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/run_${2}\"\)

fi

# Add units

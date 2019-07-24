#! /bin/sh
ROOTFILEDIR="/chafs2/work1/apar/aggRootfiles"
if [ "$#" -lt 2 ]; then
  echo "Usage: ./accumulate_aggFiles_list.sh list.txt output.root"
  echo "Error: Need to pass a runlist and output rootfile name as arguments"
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

  hadd -f minirun_${2} ${minirun_lines[@]}
  hadd -f run_${2} ${run_lines[@]}

  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"minirun_${2}\"\)
  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"run_${2}\"\)

fi

# Add units

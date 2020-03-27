#! /bin/sh
ROOTFILEDIR="/chafs2/work1/apar/aggRootfiles/slugRootfiles/PQB/grandRootfile_#/grand_aggregator.root"
OUTPUTROOTFILEDIR="/chafs2/work1/apar/aggRootfiles/slugRootfiles/PQB/grandRootfile/"
#OUTPUTROOTFILEDIR="./"
if [ "$#" -lt 1 ]; then
  echo "Error: Need to pass a list file name number \"slug##\".list"
  echo "Usage: ./smartHadd_slug_PQB.sh list.txt (input file list, \\n separated) outputFileName (a name..., optional argument)"
else
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
  #  run_temp="${ROOTFILEDIR}/grandRootfile_${lines[$i]}/grand_aggregator.root"
    #run_lines[$i]=$run_temp
    num_lines=$i
  done

  name="${lines[0]}-${lines[$num_lines]}"
  if [ "$#" -ge 2 ]; then
    name=$2
  fi
  root -l -b -q ~/PREX/japan/rootScripts/merger/smartHadd.C\(\"${1}\",\"${ROOTFILEDIR}\",\"${OUTPUTROOTFILEDIR}/grand_${name}.root\",\"n_runs\",\"run_number\",\"agg\"\)

  # Add units
  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/grand_${name}.root\"\)
fi

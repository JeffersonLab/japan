#! /bin/sh
stub=""
if [ "$#" -ge 3 ]; then
  stub=$3
fi
ROOTFILEDIR="/chafs2/work1/apar/aggRootfiles/dithering${stub}/minirun_aggregator_#_#.root"
OUTPUTROOTFILEDIR="/chafs2/work1/apar/aggRootfiles/slugRootfiles/dithering${stub}/"
#OUTPUTROOTFILEDIR="./"
if [ "$#" -lt 1 ]; then
  echo "Error: Need to pass a list file name"
  echo "Usage: ./smartHadd_miniruns_dithering.sh list.txt (input file list, \\n separated) outputFileName (a name..., optional argument) stub (a stub argument for dithering analysis... i.e. \"_1X\")"
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

  slug=`rcnd ${lines[0]} slug`
  name=minirun_slug${slug}.root
  if [ "$#" -ge 2 ]; then
    name=$2
  fi
  root -l -b -q ~/PREX/japan/rootScripts/merger/smartHadd_miniruns.C\(\"${1}\",\"${ROOTFILEDIR}\",\"${OUTPUTROOTFILEDIR}/${name}\",\"run_number\",\"minirun_n\",\"agg\"\)

  # Add units
  root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/${name}\"\)
fi

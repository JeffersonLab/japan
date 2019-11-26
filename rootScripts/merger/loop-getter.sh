#! /bin/sh
ROOTFILEDIR="/chafs2/work1/apar/aggRootfiles"
#OUTPUTROOTFILEDIR="/chafs2/work1/apar/aggRootfiles/slugRootfiles"
OUTPUTROOTFILEDIR="./test/"
if [ "$#" -lt 1 ]; then
  echo "Error: Need to pass a list file name number \"slug##.txt\""
  echo "Usage: ./loop-getter.sh list.txt (input file list, \\n separated)"
else
  IFS=$'\n' read -d '' -r -a lines < ${1}
  # all lines
  echo "Hadding all runs: ${lines[@]}"
  #minirun_lines=("${lines[@]}")
  minirun_lines=()
  run_lines=("${lines[@]}")
  num_miniruns=0

  # Do miniruns
  for i in "${!lines[@]}"; do
    shopt -s nullglob
    miniarray=(${ROOTFILEDIR}/minirun_aggregator_${lines[$i]}_*.root)
    shopt -u nullglob
    for j in "${!miniarray[@]}"; do
      echo "Run ${lines[$i]}, minirun $j"
      root -l -b -q treeGetter.C\(${lines[$i]},$j,\"input.txt\"\)
      let num_miniruns++
    done
  done
fi


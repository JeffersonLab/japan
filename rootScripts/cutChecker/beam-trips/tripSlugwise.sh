#!/bin/bash

if [[ $# -lt 2 ]] ; then
  echo "Needs two arguments, starting slug and final slug (inclusive)"
  exit
fi
for (( i=$1; i<=$2; i++ )); do
  #./tripRunlist.sh ~/PREX/prompt/WAC/run_list/slug${i}.list
  ./tripRunlist.sh /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_run_list/crex-respin2/slugs/slug${i}.list
done

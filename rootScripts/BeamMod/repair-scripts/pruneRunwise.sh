#!/bin/bash
if [[ $# -lt 1 ]] ; then
  echo "Default assumes no coils, no postfix, and no prefix, otherwise can run as"
  echo "    ./pruneRunwise.sh segmentation-list-file coilStr (_13746) postfixStr (_A) prefixStr (AT_)"
  echo ""
  echo "And to fill one of those slots with an empty string feed it the word NONE in all caps"
  echo ""
  echo "Requires at least passing a prune run list (just run numbers)"
  exit
fi

coils=""
postfix=""
prefix=""
if [[ $# -ge 2 ]] ; then
  coils=$2
  if [[ $coils == "NONE" ]] ; then
    coils=""
  fi
  # Assume "_something"
fi
if [[ $# -ge 3 ]] ; then
  postfix=$3
  if [[ $postfix == "NONE" ]] ; then
    postfix=""
  fi
  # Assume "_something"
fi
if [[ $# -ge 4 ]] ; then
  prefix=$4
  if [[ $prefix == "NONE" ]] ; then
    prefix=""
  fi
  # Assume "something_"
fi

while IFS= read -r line; do
  echo "Pruning run $line, auto < > WAC"
  #echo "root -l -b -q CopyTree_runnum.C'(\"../bmodOutput/slopes_cyclewise${postfix}_1X/${prefix}dithering_slopes${coils}${postfix}_cyclewise.root\",'${line}')'"
  #root -l -b -q CopyTree_runnum.C'("../bmodOutput/slopes_cyclewise'${postfix}'_1X/'${prefix}'dithering_slopes'${coils}${postfix}'_cyclewise.root",'${line}')'
  #echo "root -l -b -q CopyTree_runnum.C'(\"../bmodOutput/slopes_run_avg${postfix}_1X/${prefix}dithering_slopes${coils}${postfix}_run_avg.root\",'${line}')'"
  #root -l -b -q CopyTree_runnum.C'("../bmodOutput/slopes_run_avg'${postfix}'_1X/'${prefix}'dithering_slopes'${coils}${postfix}'_run_avg.root",'${line}')'
  echo "root -l -b -q CopyTree_runnum.C'(\"../bmodOutput/slopes_run_avg${postfix}_1X/${prefix}dithering_slopes${coils}_run_avg.root\",'${line}')'"
  root -l -b -q CopyTree_runnum.C'("../bmodOutput/slopes_run_avg'${postfix}'_1X/'${prefix}'dithering_slopes'${coils}'_run_avg.root",'${line}')'
done < $1

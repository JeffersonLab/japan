#!/bin/bash

# Does NOT trim whitespace
#while IFS= read -r line; do

# Read a text file with format
# segment, firstrun, lastrun

if [[ $# -lt 1 ]] ; then
  echo "Default assumes no coils, no postfix, and no prefix, otherwise can run as"
  echo "    ./segmentRuns.sh segmentation-list-file coilStr (_13746) postfixStr (_A) prefixStr (AT_) analysis (segment,plots,evince)"
  echo ""
  echo "And to fill one of those slots with an empty string feed it the word NONE in all caps"
  echo ""
  echo "Requires at least passing a segmentation run list (assumes CSV syntax of Segment,InitialRun,FinalRun)"
  exit
fi

coils=""
postfix=""
prefix=""
ana="segment"
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
if [[ $# -ge 5 ]] ; then
  ana=$5
  if [[ $ana == "NONE" ]] ; then
    ana="segment"
  fi
  # Assume "something_"
fi

while IFS= read -r line; do
  #/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_respin_slug_list.sh $slug
  OLDIFS=$IFS
  IFS=','
  items=$line
  itemsarray=()
  for item in $items
  do
    item=`echo $item | xargs`
    itemsarray+=($item)
  done
  if [[ $ana == "segment" ]] ; then
    echo "Segmenting segment $i, auto < > WAC"
    echo "root -l -b -q SegmentTree_runnum.C'(\"../bmodOutput/slopes_cyclewise${postfix}_1X/${prefix}dithering_slopes${coils}${postfix}_cyclewise.root\",'${itemsarray[1]}','${itemsarray[0]}','${itemsarray[2]}')'"
    root -l -b -q SegmentTree_runnum.C'("../bmodOutput/slopes_cyclewise'${postfix}'_1X/'${prefix}'dithering_slopes'${coils}${postfix}'_cyclewise.root",'${itemsarray[1]}','${itemsarray[0]}','${itemsarray[2]}')'
    echo "root -l -b -q SegmentTree_runnum.C'(\"../bmodOutput/slopes_run_avg${postfix}_1X/${prefix}dithering_slopes${coils}${postfix}_runwise.root\",'${itemsarray[1]}','${itemsarray[0]}','${itemsarray[2]}')'"
    root -l -b -q SegmentTree_runnum.C'("../bmodOutput/slopes_run_avg'${postfix}'_1X/'${prefix}'dithering_slopes'${coils}${postfix}'_runwise.root",'${itemsarray[1]}','${itemsarray[0]}','${itemsarray[2]}')'
  elif [[ $ana == "plots" ]] ; then
    root -l -b -q plotAD_highlightedFlag.C'("../bmodOutput/slopes_run_avg'${postfix}'_1X/'${prefix}'dithering_slopes'${coils}${postfix}'_runwise.root","'${itemsarray[0]}'","0")'
    root -l -b -q plotAD_highlightedFlag.C'("../bmodOutput/slopes_cyclewise'${postfix}'_1X/'${prefix}'dithering_slopes'${coils}${postfix}'_cyclewise.root","'${itemsarray[0]}'","0")'
  elif [[ $ana == "evince" ]] ; then
    evince ../bmodOutput/slopes_cyclewise${postfix}_1X/${prefix}dithering_slopes${coils}${postfix}_cyclewise0_${itemsarray[0]}alphas-deltas.pdf &
    evince ../bmodOutput/slopes_run_avg${postfix}_1X/${prefix}dithering_slopes${coils}${postfix}_runwise0_${itemsarray[0]}alphas-deltas.pdf
  fi
done < $1

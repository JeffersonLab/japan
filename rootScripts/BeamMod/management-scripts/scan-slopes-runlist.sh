#!/bin/bash

# Blessed runlist with Production only, Good, Suspicious, and NeedCut runs
# Comes from /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_respin_slug_list.sh
# /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/crex-runlist/simple_list/blessed-prompt-crex-all.list
if [[ $# -lt 1 ]] ; then
  echo "Must pass in runlist"
  exit
fi

run=0.0
previous10run=5000.0
previous3run=5000.0
next10run=0.0
next3run=0.0
while read line; do
#while IFS= read -r line; do
  let line=`echo -n "${line//[[:space:]]/}"`
  #echo $line
  run=$line;
  #timenow=$(date +"%Y-%m%d-%H%M");

  run=$(printf "%.1f" "$(bc -l <<< 1.0*$run)")
  next10run=$(printf "%.1f" "$(bc -l <<< \(1.0*$previous10run\)+9.0)")
  #echo "Run $run and test against $next10run"
  if [[ $run > $next10run ]] ; then
    echo "10 step: Run $run vs. prior run $previous10run"
    #root -l -b -q plotAD_13746.C'("slopes_run_avg_1X/dithering-slopes-runwise.root","'${run}'","10")'
    root -l -b -q plotAD_13746.C'("slopes_run_avg_1X/AT-dithering-slopes-runwise.root","'${run}'","10")'
    previous10run=$run
  #else
    #echo "Skip $run 10"
  fi
  next3run=$(printf "%.1f" "$(bc -l <<< \(1.0*$previous3run\)+2.0)")
  if [[ $run > $next3run ]] ; then
    echo "3 step: Run $run vs. prior run $previous3run"
    #root -l -b -q plotAD_13746.C'("slopes_cyclewise_1X/dithering-slopes-cyclewise.root","'${run}'","5")'
    root -l -b -q plotAD_13746.C'("slopes_cyclewise_1X/AT-dithering-slopes-cyclewise.root","'${run}'","5")'
    previous3run=$run
  #else
    #echo "Skip $run 3"
  fi

done < $1
#for (( i=5000; i<=8560; i=$i+10 )); do

#for (( i=6344; i<=6408; i=$i+10 )); do
#  #/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_respin_slug_list.sh $slug
#  echo "Starting at run $i, auto < > WAC"
#
#  # Runlist check dithering does a check for missing runs, etc. -> csv file
#  # Designed to work with the prompt-era post-segmented/pruned dit slope files
#  #/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/dithering/runlist_check_dithering.sh $slug
#
#  # Before doing full scale slope determination do more precise runlist verification - suspicious and need cuts and include calibration runs?
#  # Done - Make a new runlist that is only Production && !Bad, etc.
#  # Done - For every 10 runs from 5000-8450 call this script.
#  # Done -  Make dit-slopes merger for run-list blessed dithering run slopes
#  #   OR more complicated: make a flag==0 setting script for runs NOT in the blessed runlist file (or in the BAD runlist file and non-Production)
#  # Done -  Add useful-analysis name to ....C
#  # Done -  Reset slugNum == "" if == "0" later on in ....C
#  #  Next do more intelligent supercycle remove
#  #  Next do segmentation careful study to make sure there are no intermediate runs/supercycles/alternative reasons to split or not split
#  #  Next do segmentation
#  root -l -b -q plotAD_13746.C'("slopes_run_avg_1X/dithering_slopes_runwise.root","'${i}'","10")'
#done
#for (( i=6344; i<=6408; i=$i+3 )); do
#  root -l -b -q plotAD_13746.C'("slopes_cyclewise_1X/dithering_slopes_cyclewise.root","'${i}'","5")'
#
#  #evince ~/PREX/prompt/hallaweb_online/dithering_1X_slug/slug_list/slug${slug}/dithering_slopes_13746_slug${slug}_alphas-deltas.pdf&
#  #evince ~/PREX/prompt/hallaweb_online/slug/slug_list/slug${slug}/summary_minirun_slug${slug}.pdf&
#  #sleep 3
##  /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/dithering/dither_slug_summary.sh $slug
##  /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/dithering/dither_slug_summary.sh $slug _1X
##  while IFS= read -r line
##  do
##    stub=/u/group/prex/analysis/www/crex/summary/all/run${line}_000/
##    echo "evince $stub"
##    evince $stub/run${line}_000_summary_bcm.pdf
##  done < "$input"
##  diff auto_run_list/slug${slug}.list run_list/slug${slug}.list
#done
##done < $1

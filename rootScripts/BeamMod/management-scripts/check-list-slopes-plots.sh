#!/bin/bash

# Blessed runlist with Production only, Good, Suspicious, and NeedCut runs
# Comes from /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_respin_slug_list.sh
# /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/crex-runlist/simple_list/Blessed-CREX-run-list.list

#while IFS= read -r line; do
  #run=$line;
  #timenow=$(date +"%Y-%m%d-%H%M");
#for (( i=5000; i<=8560; i=$i+10 )); do

cwd=`pwd`
#cd /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/
#source /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/configs/crex_ErrorFlag_dit_1X_runwise.sh
#cd $JAPAN_DIR/rootScripts/merger/
#./smartHadd_dit_slopes_by_run.sh /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_run_list/Blessed-AT-run-list.list
#cd /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/
#source /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/configs/crex_ErrorFlag_dit_1X_cyclewise.sh
#cd $JAPAN_DIR/rootScripts/merger/
#./smartHadd_dit_slopes_by_run.sh /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_run_list/Blessed-AT-run-list.list
cd $cwd

#declare -a 
slopePlots_runwise_list=()
#declare -a
slopePlots_cyclewise_list=()

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
    strrun=$(printf "%.0f" "$(bc -l <<< 1.0*$run)")
    slopePlots_runwise_list+=(/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_run_avg_1X/AT-dithering-slopes-runwise10_${strrun}alphas-deltas.pdf);
    evince /lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_run_avg_1X/AT-dithering-slopes-runwise10_${strrun}alphas-deltas.pdf
    previous10run=$run
  #else
    #echo "Skip $run 10"
  fi
  next3run=$(printf "%.1f" "$(bc -l <<< \(1.0*$previous3run\)+2.0)")
  if [[ $run > $next3run ]] ; then
    echo "3 step: Run $run vs. prior run $previous3run"
    strrun=$(printf "%.0f" "$(bc -l <<< 1.0*$run)")
    slopePlots_cyclewise_list+=(/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_cyclewise_1X/AT-dithering-slopes-cyclewise5_${strrun}alphas-deltas.pdf);
    previous3run=$run
  #else
    #echo "Skip $run 3"
  fi

done < $1

for slopefile in $slopePlots_runwise_list
do
  evince $slopefile
done

# Cyclewise - more difficult to parse by eye, just do it for cases that need additional input
#for slopefile in $slopePlots_cyclewise_list
#do
  #evince $slopefile
#done

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

shopt -s extglob
# find split file
slopePlots_runwise_list=$(ls -1 /lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_run_avg_1X/dithering-slopes-runwise10_*alphas-deltas.pdf);
slopePlots_cyclewise_list=$(ls -1 /lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_cyclewise_1X/dithering-slopes-cyclewise5_*alphas-deltas.pdf);
shopt -u extglob

for slopefile in $slopePlots_runwise_list
do
  evince $slopefile
done

# Cyclewise - more difficult to parse by eye, just do it for cases that need additional input
#for slopefile in $slopePlots_cyclewise_list
#do
  #evince $slopefile
#done

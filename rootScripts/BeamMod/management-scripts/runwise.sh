#!/bin/bash

# Blessed runlist with Production only, Good, Suspicious, and NeedCut runs
# Comes from /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_respin_slug_list.sh
# /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/crex-runlist/simple_list/blessed-prompt-crex-all.list

#while IFS= read -r line; do
  #run=$line;
  #timenow=$(date +"%Y-%m%d-%H%M");
#for (( i=5000; i<=8560; i=$i+10 )); do

cwd=`pwd`
#cd /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/
#source /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/configs/crex_ErrorFlag_dit_1X_runwise.sh
#cd $JAPAN_DIR/rootScripts/merger/
#./smartHadd_dit_slopes_by_run.sh /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_run_list/Blessed-AT-run-list.list dithering_slopes_runwise.root
#cd /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/
#source /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/configs/crex_ErrorFlag_dit_1X_cyclewise.sh
#cd $JAPAN_DIR/rootScripts/merger/
#./smartHadd_dit_slopes_by_run.sh /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_run_list/Blessed-AT-run-list.list dithering_slopes_cyclewise.root
cd $cwd

for (( i=6344; i<=6408; i=$i+10 )); do
  #/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_respin_slug_list.sh $slug
  echo "Starting at run $i, auto < > WAC"

  # Runlist check dithering does a check for missing runs, etc. -> csv file
  # Designed to work with the prompt-era post-segmented/pruned dit slope files
  #/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/dithering/runlist_check_dithering.sh $slug


  #  Next do segmentation
  root -l -b -q plotAD_13746.C'("slopes_run_avg_1X/dithering_slopes_runwise.root","'${i}'","10")'
done
for (( i=6344; i<=6408; i=$i+3 )); do
  root -l -b -q plotAD_13746.C'("slopes_cyclewise_1X/dithering_slopes_cyclewise.root","'${i}'","5")'

  #evince ~/PREX/prompt/hallaweb_online/dithering_1X_slug/slug_list/slug${slug}/dithering_slopes_13746_slug${slug}_alphas-deltas.pdf&
  #evince ~/PREX/prompt/hallaweb_online/slug/slug_list/slug${slug}/summary_minirun_slug${slug}.pdf&
  #sleep 3
#  /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/dithering/dither_slug_summary.sh $slug
#  /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/dithering/dither_slug_summary.sh $slug _1X
#  while IFS= read -r line
#  do
#    stub=/u/group/prex/analysis/www/crex/summary/all/run${line}_000/
#    echo "evince $stub"
#    evince $stub/run${line}_000_summary_bcm.pdf
#  done < "$input"
#  diff auto_run_list/slug${slug}.list run_list/slug${slug}.list
done
#done < $1

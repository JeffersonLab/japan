#!/bin/bash
#while IFS= read -r line; do
  #slug=$line;
  #timenow=$(date +"%Y-%m%d-%H%M");
#cd ~/PREX/prompt/bmodAna/
for (( i=100; i<=223; i++ )); do
#for (( i=4000; i<=4019; i++ )); do
  #~/PREX/prompt/WAC/hacks/fill_dithering.sh $i
  slug=$i
  #./auto_slug_list.sh $slug
  echo "Slug $slug, auto < > WAC"
  #input="run_list/slug${slug}.list"
  #~/PREX/prompt/WAC/dither_make_grand_plots.sh run_list/slug${slug}.list 100 _1X 
  #~/PREX/prompt/WAC/dither_make_grand_plots.sh run_list/slug${slug}.list

  # Runlist check dithering does a check for missing runs, etc. -> csv file
  # Designed to work with the prompt-era post-segmented/pruned dit slope files
  #~/PREX/prompt/WAC/dithering/runlist_check_dithering.sh $slug

  # Done - Before doing full scale slope determination do more precise runlist verification - suspicious and need cuts and include calibration runs?
  # Done - Make a new runlist that is only Production && !Bad, etc.
  # Done - For every 10 runs from 5000-8450 call this script.
  # Done -  Add useful-analysis name to ....C
  # Done -  Reset slugNum == "" if == "0" later on in ....C
  #  Next do more intelligent supercycle remove
  #  Next do segmentation careful study to make sure there are no intermediate runs/supercycles/alternative reasons to split or not split
  #  Next do segmentation

  root -l -b -q plotAD_13746.C'("slopes_run_avg_1X/dithering-slopes-runwise.root","","'$slug'")'
  root -l -b -q plotAD_13746.C'("slopes_cyclewise_1X/dithering-slopes-cyclewise.root","","'$slug'")'

  root -l -b -q plotAD_FullExpt.C'("slopes_run_avg_1X/dithering-slopes-runwise.root","","'$slug'")'
  root -l -b -q plotAD_FullExpt.C'("slopes_cyclewise_1X/dithering-slopes-cyclewise.root","","'$slug'")'

  #evince ~/PREX/prompt/hallaweb_online/dithering_1X_slug/slug_list/slug${slug}/dithering_slopes_13746_slug${slug}_alphas-deltas.pdf&
  #evince ~/PREX/prompt/hallaweb_online/slug/slug_list/slug${slug}/summary_minirun_slug${slug}.pdf&
  #sleep 3
#  ~/PREX/prompt/WAC/dithering/dither_slug_summary.sh $slug
#  ~/PREX/prompt/WAC/dithering/dither_slug_summary.sh $slug _1X
#  while IFS= read -r line
#  do
#    stub=/u/group/prex/analysis/www/crex/summary/all/run${line}_000/
#    echo "evince $stub"
#    evince $stub/run${line}_000_summary_bcm.pdf
#  done < "$input"
#  diff auto_run_list/slug${slug}.list run_list/slug${slug}.list
  #root -l -b -q plotAD_13746.C\(\"slopes_run_avg/dithering_slopes_13746_slug${slug}.root\"\);
  #/bin/cp --force ~/PREX/prompt/bmodAna/slopes_run_avg/dithering_slopes_13746_slug${slug}_alphas-deltas.pdf ~/PREX/prompt/hallaweb_online/dithering_slug/slug_list/slug${slug}/
  #root -l -b -q plotAD_13746.C\(\"slopes_run_avg_1X/dithering_slopes_13746_slug${slug}.root\"\);
  #/bin/cp --force ~/PREX/prompt/bmodAna/slopes_run_avg_1X/dithering_slopes_13746_slug${slug}_alphas-deltas.pdf ~/PREX/prompt/hallaweb_online/dithering_1X_slug/slug_list/slug${slug}/
  #root -l -b -q plotAD_13746.C\(\"slopes/dithering_slopes_13746_slug${slug}.root\",\"$1\"\);
  #/bin/cp --force ~/PREX/prompt/bmodAna/slopes/dithering_slopes_13746_slug${slug}_alphas-deltas.pdf ~/PREX/prompt/hallaweb_online/dithering_1X_slug/slug_list/slug${slug}/dithering_slopes_13746_slug${slug}_cyclewise_alphas-deltas.pdf
  #sleep 1
  ###~/PREX/prompt/agg-scripts/make_grand_plots.sh run_list/slug${slug}.list
  ###~/PREX/prompt/agg-scripts/PQB_make_grand_plots.sh run_list/slug${slug}.list
done
#done < $1

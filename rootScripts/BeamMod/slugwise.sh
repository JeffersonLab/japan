#!/bin/bash
#while IFS= read -r line; do
  #slug=$line;
  #timenow=$(date +"%Y-%m%d-%H%M");
#cd ~/PREX/prompt/bmodAna/
#for (( i=183; i<=185; i++ )); do
for (( i=106; i<=185; i++ )); do
  #~/PREX/prompt/WAC/hacks/fill_dithering.sh $i
  slug=$i
  #./auto_slug_list.sh $slug
  echo "Slug $slug, auto < > WAC"
  #input="run_list/slug${slug}.list"
  #~/PREX/prompt/WAC/dither_make_grand_plots.sh run_list/slug${slug}.list 100 _1X 
  #~/PREX/prompt/WAC/dither_make_grand_plots.sh run_list/slug${slug}.list
  ~/PREX/prompt/WAC/dithering/runlist_check_dithering.sh $slug
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

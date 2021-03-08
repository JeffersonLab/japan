#!/bin/bash

for (( i=5000; i<=9000; i=$i+1 )); do
  echo "Copy files for run $i, auto < > WAC"
  #cp slopes_run_avg_1X/dithering_slopes_13746_slug*_run${i}_alphas-deltas.pdf /u/group/prex/analysis/www/crex/summary_respin1/all/run${i}_000/dithering_slopes_13746_run${i}_runwise_alphas-deltas.pdf
  #cp slopes_run_avg_1X/BMOD_sensitivities_13746_run${i}.txt /u/group/prex/analysis/www/crex/summary_respin1/all/run${i}_000/BMOD_sensitivities_13746_run${i}_runwise.txt
  #cp slopes_cyclewise_1X/dithering_slopes_13746_slug*_run${i}_alphas-deltas.pdf /u/group/prex/analysis/www/crex/summary_respin1/all/run${i}_000/dithering_slopes_13746_run${i}_cyclewise_alphas-deltas.pdf
  #cp slopes_cyclewise_1X/BMOD_sensitivities_13746_run${i}.txt /u/group/prex/analysis/www/crex/summary_respin1/all/run${i}_000/BMOD_sensitivities_13746_run${i}_cyclewise.txt

  cp /lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/amplitude-check/run${i}*.pdf /u/group/prex/analysis/www/crex/summary_respin1/all/run${i}_000/
done

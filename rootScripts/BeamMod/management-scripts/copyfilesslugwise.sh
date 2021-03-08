#!/bin/bash

for (( i=100; i<=223; i=$i+1 )); do
  echo "Copy files for slug $i, auto < > WAC"
  cp slopes_run_avg_1X/dithering-slopes-runwise${i}_alphas-deltas.pdf /u/group/prex/analysis/www/crex/agg-respin1/ErrorFlag_slug/slug_list/slug${i}/
  cp slopes_run_avg_1X/dithering-slopes-runwise${i}_alphas-deltas_FullExpt.pdf /u/group/prex/analysis/www/crex/agg-respin1/ErrorFlag_slug/slug_list/slug${i}/
  cp slopes_cyclewise_1X/dithering-slopes-cyclewise${i}_alphas-deltas.pdf /u/group/prex/analysis/www/crex/agg-respin1/ErrorFlag_slug/slug_list/slug${i}/
  cp slopes_cyclewise_1X/dithering-slopes-cyclewise${i}_alphas-deltas_FullExpt.pdf /u/group/prex/analysis/www/crex/agg-respin1/ErrorFlag_slug/slug_list/slug${i}/
done

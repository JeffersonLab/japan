#!/bin/bash
stub="_run_avg_eigenvectors_double_sorted_part_avg_allbpms_1X"
#stub="_run_avg_eigenvectors_double_sorted_seg_avg_1X"
########cp -p /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_CREX_dithering_slopes_run_avg_basic.root /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_CREX_dithering_slopes_run_avg.root
########cp -p /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg_basic.root /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg.root
########cp -p /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_CREX_dithering_slopes_run_avg_basic.root /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_CREX_dithering_slopes_run_avg_pruned.root
########cp -p /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg_basic.root /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg_pruned.root
########
########while IFS= read -r line; do
########  echo "Pruning run $line, auto < > WAC"
########  echo "root -l -b -q CopyTree_runnum.C'(\"/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_CREX_dithering_slopes_run_avg_pruned.root\",'${line}')'"
########  root -l -b -q CopyTree_runnum.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_CREX_dithering_slopes_run_avg_pruned.root",'${line}')'
########done < ../results/pruneRuns_respin2.list
########
########while IFS= read -r line; do
########  echo "Pruning run $line, auto < > WAC"
########  echo "root -l -b -q CopyTree_runnum.C'(\"/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg_pruned.root\",'${line}')'"
########  root -l -b -q CopyTree_runnum.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg_pruned.root",'${line}')'
########done < ../results/AT-pruneRuns_respin2.list
########
########cp -p /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_CREX_dithering_slopes_run_avg_pruned.root /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_CREX_dithering_slopes_run_avg_segmented_pruned.root
########cp -p /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg_pruned.root /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg_segmented_pruned.root
########
########while IFS= read -r line; do
########  #/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_respin_slug_list.sh $slug
########  OLDIFS=$IFS
########  IFS=','
########  items=$line
########  itemsarray=()
########  for item in $items
########  do
########    item=`echo $item | xargs`
########    itemsarray+=($item)
########  done
########  echo "Segmenting segment $i, auto < > WAC"
########  echo "root -l -b -q SegmentTree_runnum.C'(\"/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_CREX_dithering_slopes_run_avg_segmented_pruned.root\",'${itemsarray[1]}','${itemsarray[0]}','${itemsarray[2]}')'"
########  root -l -b -q SegmentTree_runnum.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_CREX_dithering_slopes_run_avg_segmented_pruned.root",'${itemsarray[1]}','${itemsarray[0]}','${itemsarray[2]}')'
########done < ../results/contiguous-segmentRuns_respin2.list
########while IFS= read -r line; do
########  #/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_respin_slug_list.sh $slug
########  OLDIFS=$IFS
########  IFS=','
########  items=$line
########  itemsarray=()
########  for item in $items
########  do
########    item=`echo $item | xargs`
########    itemsarray+=($item)
########  done
########  echo "Segmenting segment $i, auto < > WAC"
########  echo "root -l -b -q SegmentTree_runnum.C'(\"/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg_segmented_pruned.root\",'${itemsarray[1]}','${itemsarray[0]}','${itemsarray[2]}')'"
########  root -l -b -q SegmentTree_runnum.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_AT_dithering_slopes_run_avg_segmented_pruned.root",'${itemsarray[1]}','${itemsarray[0]}','${itemsarray[2]}')'
########done < ../results/contiguous-AT-segmentRuns_respin2.list
########
########root -l -b -q Add_agg_rcdb_dit.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_CREX_dithering_slopes_run_avg_segmented_pruned.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_CREX_dithering_slopes_run_avg_rcdb_segmented_pruned.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/aggRootfiles/ErrorFlag/slugRootfiles/CREX-All-miniruns_units_maindet.root")'
########root -l -b -q Add_agg_rcdb_dit.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_AT_dithering_slopes_run_avg_segmented_pruned.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_AT_dithering_slopes_run_avg_rcdb_segmented_pruned.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/aggRootfiles/ErrorFlag/slugRootfiles/CREX-All-miniruns_units_maindet.root")'
########
########


cd /u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/workspace_plotting_macros/

root -l -b -q addMainDet_dit.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_CREX_dithering_slopes_run_avg_rcdb_segmented_pruned.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_CREX_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg.root","dit","12evMons")'
root -l -b -q addMainDet_dit.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_AT_dithering_slopes_run_avg_rcdb_segmented_pruned.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_AT_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg.root","dit","12evMons")'
#root -l -b -q addMainDet_dit.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_CREX_dithering_slopes_run_avg_rcdb_segmented_pruned.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_CREX_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg.root","dit","evMons")'
#root -l -b -q addMainDet_dit.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_AT_dithering_slopes_run_avg_rcdb_segmented_pruned.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_AT_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg.root","dit","evMons")'
hadd -f /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_All_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg.root /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_CREX_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg.root /lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes$stub/respin2_AT_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg.root

cd -

root -l -b -q Add_minirun_to_dit.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_All_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_All_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg_tag.root")'


root -l -b -q Expand_dit_to_mini.C'("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_All_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg_tag.root","/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes'$stub'/respin2_All_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg_tag_expanded.root","dit_part_avgd")'

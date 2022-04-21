#!/bin/bash

#  FIXME FIXME I need to update the singlePlotTable.C script to know that lagr_asym... is now eigen_lagr_asym!!! and likely also a lot of other variable names changed as I shifted into the aggregator world... I should probably rerun the ErrorFlag versions now too :|

cut=""
if [ $# -ge 1 ] ; then
  cut=$1
fi

vtime=""
if [ $# -ge 2 ] ; then
  vtime=$2
fi

mod_draw=""
if [ $# -ge 3 ] ; then
  mod_draw=$3
fi

null=""
if [ $# -ge 4 ] ; then
  null=$4
fi


 mv singles_plots_tables singles_plots_tables_save
 mkdir singles_plots_tables
 # Get main results
 root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","eigen_lagr_asym_manual_main_det","ppb","rcdb_slug<=185","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"Lagrange Apv")'
 root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","eigen_lagr_asym_us_dd","ppb"," rcdb_slug<=185","'$vtime'","'$cut'","part","'$null'_main_det_corrections",1,"Lagrange DD")'
 mv singles_plots_tables singles_plots_tables_pre_COVID
 mkdir singles_plots_tables
 # Do the pre and post cuts separately, move the output files by hand
 root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","eigen_lagr_asym_manual_main_det","ppb","rcdb_slug>=186","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"Lagrange Apv")'
 root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","eigen_lagr_asym_us_dd","ppb"," rcdb_slug>=186","'$vtime'","'$cut'","part","'$null'_main_det_corrections",1,"Lagrange DD")'
 mv singles_plots_tables singles_plots_tables_post_COVID
 mv singles_plots_tables_save singles_plots_tables


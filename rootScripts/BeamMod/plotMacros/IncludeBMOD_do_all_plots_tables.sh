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


 ######## Get main results
 #######root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","eigen_lagr_asym_manual_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"Lagrange Apv")'
 root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","255.70923+'$mod_draw'","eigen_lagr_asym_manual_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"Unblinded Lagrange Apv")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","eigen_lagr_asym_us_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",1,"Lagrange DD")'

 ######## Raw asym
 #######root -l -b -q singlePlotTable.C'("mini_raw_det_asyms_det_weighted","'$mod_draw'","asym_manual_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"Maindet weighted Raw Apv")'
 ######## Correction amount of asym
 #######root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_cor_det_asyms_det_weighted","'$mod_draw'","main_det_eigen_lagr_correction","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"Maindet weighted Lagrange Apv Correction")'
 #######
 ######## Compare techniques
 ########root -l -b -q singlePlotTable.C'("mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted","'$mod_draw'","eigen_reg_asym_manual_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"5BPM minirunwise evMon Regression Apv")'
 ########root -l -b -q singlePlotTable.C'("mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted","'$mod_draw'","eigen_reg_asym_manual_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"12BPM minirunwise evMon Regression Apv")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted","'$mod_draw'","eigen_reg_asym_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"5BPM Regression Apv")'
 #######root -l -b -q singlePlotTable.C'("mini_dit_part_avgd_det_asyms_det_weighted","'$mod_draw'","eigen_dit_asym_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"5BPM dit Apv")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","eigen_reg_asym_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"12BPM Regression Apv")'
 #######root -l -b -q singlePlotTable.C'("mini_regression_det_asyms_det_weighted","'$mod_draw'","reg_asym_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"Plain Regression Apv")'
 #######root -l -b -q singlePlotTable.C'("mini_overload_det_asyms_det_weighted","'$mod_draw'","overload_reg_asym_manual_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"Plain Overload Regression Apv")'
 ########root -l -b -q singlePlotTable.C'("mini_dit_plain_det_asyms_det_weighted","'$mod_draw'","eigen_dit_asym_manual_main_det","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"Plain Dithering Apv")'
#######
 ######## evMons
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon0","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon0")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon1","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon1")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon2","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon2")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon3","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon3")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon4","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon4")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon5","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon5")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon6","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon6")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon7","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon7")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon8","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon8")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon9","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon9")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon10","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon10")'
 #######root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon11","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff evMon11")'
 #######root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm4aX","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff BPM 4aX")'
 #######root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm4aY","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff BPM 4aY")'
 #######root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm4eX","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff BPM 4eX")'
 #######root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm4eY","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff BPM 4eY")'
 #######root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm12X","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff BPM 12X")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_an_us","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym BCM AN US")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_an_ds","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym BCM AN ds")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_dg_us","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym BCM DG US")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_dg_ds","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym BCM DG ds")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_cav4cQ","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym BCM Cav4CQ")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_an_us_an_ds_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym BCM AN DD")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_dg_us_dg_ds_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym BCM DG DD")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_cav4cQ_bcm_an_us_agg_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym bcm_cav4cQ_bcm_an_us_agg_dd")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_cav4cQ_bcm_an_ds_agg_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym bcm_cav4cQ_bcm_an_ds_agg_dd")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_cav4cQ_dg_ds_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym bcm_cav4cQ_dg_ds_dd")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_dg_us_an_us_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym bcm_dg_us_an_us_dd")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_dg_ds_an_ds_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym bcm_dg_ds_an_ds_dd")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_dg_us_an_ds_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym bcm_dg_us_an_ds_dd")'
 #######root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_dg_ds_an_us_dd","ppb","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"asym bcm_dg_ds_an_us_dd")'
#######
#######
 ######## Kinematics
 #######root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_targ_positionX","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff target positionX")'
 #######root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_targ_positionY","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff target positionY")'
 #######root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_targ_thetaX"   ,"nrad","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff target thetaX")'
 #######root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_targ_thetaY"   ,"nrad","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff target thetaY")'
 #######root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_dispE_bpm12X"  ,"dpp","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff dispersive E (bpm12X)")'



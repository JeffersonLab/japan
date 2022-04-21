#!/bin/bash
vtime=""
if [ $# -ge 1 ] ; then
  vtime=$1
fi

mod_draw=""
if [ $# -ge 2 ] ; then
  mod_draw=$2
fi

null=""
if [ $# -ge 3 ] ; then
  null=$3
fi


 # Get main results
 root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","lagr_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"Lagrange Apv")'
 root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","lagr_asym_us_dd","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",1,"Lagrange DD")'

 # Raw asym
 root -l -b -q singlePlotTable.C'("mini_raw_det_asyms_det_weighted","'$mod_draw'","asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"Maindet weighted Raw Apv")'
 # Correction amount of asym
 root -l -b -q singlePlotTable.C'("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","cor_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"Maindet weighted Lagrange Apv Correction")'
 
 # Compare techniques
 root -l -b -q singlePlotTable.C'("mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted","'$mod_draw'","reg_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"5BPM minirunwise evMon Regression Apv")'
 root -l -b -q singlePlotTable.C'("mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted","'$mod_draw'","reg_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"12BPM minirunwise evMon Regression Apv")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted","'$mod_draw'","reg_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"5BPM Regression Apv")'
 root -l -b -q singlePlotTable.C'("mini_dit_part_avgd_det_asyms_det_weighted","'$mod_draw'","dit_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"5BPM dit Apv")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted","'$mod_draw'","reg_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"12BPM Regression Apv")'
 root -l -b -q singlePlotTable.C'("mini_regression_det_asyms_det_weighted","'$mod_draw'","reg_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"Plain Regression Apv")'
 root -l -b -q singlePlotTable.C'("mini_overload_det_asyms_det_weighted","'$mod_draw'","reg_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"Plain Overload Regression Apv")'
 root -l -b -q singlePlotTable.C'("mini_dit_plain_det_asyms_det_weighted","'$mod_draw'","dit_asym_manual_main_det","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"Plain Dithering Apv")'

 # evMons
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon0","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon0")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon1","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon1")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon2","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon2")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon3","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon3")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon4","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon4")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon5","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon5")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon6","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon6")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon7","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon7")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon8","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon8")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon9","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon9")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon10","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon10")'
 root -l -b -q singlePlotTable.C'("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","'$mod_draw'","diff_evMon11","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff evMon11")'
 root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm4aX","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff BPM 4aX")'
 root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm4aY","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff BPM 4aY")'
 root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm4eX","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff BPM 4eX")'
 root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm4eY","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff BPM 4eY")'
 root -l -b -q singlePlotTable.C'("mini_BPMs_det_weighted","'$mod_draw'","diff_bpm12X","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff BPM 12X")'
 root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_an_us","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"asym BCM AN US")'
 root -l -b -q singlePlotTable.C'("mini_BCMs_det_weighted","'$mod_draw'","asym_bcm_an_ds","ppb","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"asym BCM AN ds")'



#!/bin/bash
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



 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_targ_positionX","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff target positionX")'
 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_targ_positionY","nm","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff target positionY")'
 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_targ_thetaX"   ,"nrad","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff target thetaX")'
 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_targ_thetaY"   ,"nrad","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff target thetaY")'
 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_'$cut'_diff_dispE_bpm12X"  ,"dpp","","'$vtime'","'$cut'","part","'$null'_main_det_corrections",0,"diff dispersive E (bpm12X)")'



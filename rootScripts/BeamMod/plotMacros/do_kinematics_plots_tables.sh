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


 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_ErrorFlag_diff_targ_positionX","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff target positionX")'
 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_ErrorFlag_diff_targ_positionY","nm","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff target positionY")'
 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_ErrorFlag_diff_targ_thetaX"   ,"nrad","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff target thetaX")'
 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_ErrorFlag_diff_targ_thetaY"   ,"nrad","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff target thetaY")'
 root -l -b -q singlePlotTable.C'("mini_kinematics_det_weighted","'$mod_draw'","_ErrorFlag_diff_dispE_bpm12X"  ,"dpp","","'$vtime'","ErrorFlag","part","'$null'_main_det_corrections",0,"diff dispersive E (bpm12X)")'



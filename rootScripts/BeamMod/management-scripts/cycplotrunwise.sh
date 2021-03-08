#!/bin/bash
export QW_ROOTFILES=/lustre19/expphy/volatile/halla/parity/crex-respin1/japanOutput/
for (( i=5408; i<=5460; i=$i+1 )); do
  if [ -f $QW_ROOTFILES/prexPrompt_pass2_${i}.000.root ] ; then
    echo "Starting at run $i, auto < > WAC"
    root -b -q -l "japan_plot_beammod_BPMS_amplitudecheck_crex.C($i, 0)"
  fi
done

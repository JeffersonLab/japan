#!/bin/tcsh
gojapan
cd /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod
set runNum=999999
if ($#>0) then
  set runNum=$1
endif
echo "Forked shell to do BMOD ana for run # $runNum"
root -l -b -q /adaqfs/home/apar/PREX/prompt/bmodAna/runBmodAna.C\(${runNum},\"/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/input_panguin_1X.txt\"\)

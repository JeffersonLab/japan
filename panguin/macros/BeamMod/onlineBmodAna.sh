#!/bin/tcsh
gojapan
cd /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod
set runNum=999999
if ($#>0) then
  set runNum=$1
endif
root -l -b -q /adaqfs/home/apar/PREX/prompt/bmodAna/bmodAna.C\(${runNum},\"/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/input_panguin.txt\"\)

#! /bin/sh
gojapan
SLOPEFILES="./slopes/"
OUTPUTROOTFILEDIR="./slugs/"
cd ~/cameronc/japan-feature-add-merger/rootScripts/BeamMod
if [ "$#" -lt 1 ]; then
  echo "Error: Need to pass a run number"
  echo "Usage: ./doRun ###"
else
  IFS=$'\n' read -d '' -r -a lines < ${1}.list
  echo "Hadding all runs: ${lines[@]}"
  run_lines=()
  num_runs=0

  for i in "${!lines[@]}"; do
    shopt -s nullglob
    splitarray=(${SLOPEFILES}/*${lines[$i]}*.root)
    shopt -u nullglob
    for j in "${!splitarray[@]}"; do
    #  echo ${splitarray[$j]}
    #  root -l -b -q histPlotter.C\(${1},3600,${j}\)
      run_lines[$num_runs]=${splitarray[$j]}
      let num_runs++
    done
  done

  echo Done with ${run_lines[@]}
  hadd -f ${OUTPUTROOTFILEDIR}/${1}.root ${run_lines[@]}
# root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/${1}.root\",\"dit\"\)
fi

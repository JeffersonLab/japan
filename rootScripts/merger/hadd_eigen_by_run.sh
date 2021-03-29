#! /bin/sh
ROOTFILEDIR="/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles${3}/"
OUTPUTROOTFILEDIR="/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles${3}/"
#ROOTFILEDIR="/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles_eigen_reg_parts/"
#OUTPUTROOTFILEDIR="/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles_eigen_reg_parts/"
#OUTPUTROOTFILEDIR="./"
if [ "$#" -lt 1 ]; then
  echo "Error: Need to pass a list file name number \"slug##\".list"
  echo "Usage: ./hadd_eigen_by_run.sh list.txt (input file list, \\n separated)"
else
  IFS=$'\n' read -d '' -r -a lines < ${1}
  # all lines
  echo "Hadding all runs: ${lines[@]}"
  #minirun_lines=("${lines[@]}")
  minirun_lines=()
  run_lines=("${lines[@]}")
  num_miniruns=0

  # Do runs
  for i in "${!lines[@]}"; do
    run_temp="${ROOTFILEDIR}/crexRespin1_eigen_${lines[$i]}.000.root"
    if [ ! -f $run_temp ] ; then
      echo "ERROR: Missing run ${lines[$i]}, please check it's outputs!!"
      unset 'run_lines[i]'
    else
      #echo "${lines[$i]}"
      run_lines[$i]=$run_temp
    fi
  done

  name=Full_mini_eigen_reg_allbpms_CREX.root
  if [ "$#" -ge 2 ]; then
    name=$2
  fi
  hadd -f ${OUTPUTROOTFILEDIR}/${name} ${run_lines[@]}

  # Add units
  #root -l -b -q ~/PREX/prompt/Aggregator/wrapper/addUnits.C\(\"${OUTPUTROOTFILEDIR}/grand_${name}.root\"\)

fi


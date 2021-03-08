#!/bin/bash

prefix=""
list="/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/WAC/auto_run_list/Blessed-CREX-run-list.list"
if [[ $# -ge 1 ]] ; then
  prefix=$1
fi
if [[ $# -ge 2 ]] ; then
  list=$2
fi

cd /u/group/halla/parity/software/japan_offline/prompt/prex-prompt
source /u/group/halla/parity/software/japan_offline/prompt/prex-prompt/PVDB/environment.sh
cd -

export JAPAN_DIR=/u/group/halla/parity/software/japan_offline/aggregator

cd $JAPAN_DIR/rootScripts/merger/

export DITHERING_STUB="_cyclewise_1X"
export COILS="13746"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_cyclewise.root
export DITHERING_STUB="_cyclewise_A_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_A_cyclewise.root
export DITHERING_STUB="_cyclewise_B_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_B_cyclewise.root
export DITHERING_STUB="_cyclewise_C_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_C_cyclewise.root
export DITHERING_STUB="_cyclewise_D_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_D_cyclewise.root

export COILS="13726"
export DITHERING_STUB="_cyclewise_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_cyclewise.root
export DITHERING_STUB="_cyclewise_A_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_A_cyclewise.root
export DITHERING_STUB="_cyclewise_B_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_B_cyclewise.root
export DITHERING_STUB="_cyclewise_C_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_C_cyclewise.root
export DITHERING_STUB="_cyclewise_D_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_D_cyclewise.root

export COILS="15746"
export DITHERING_STUB="_cyclewise_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_cyclewise.root
export DITHERING_STUB="_cyclewise_A_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_A_cyclewise.root
export DITHERING_STUB="_cyclewise_B_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_B_cyclewise.root
export DITHERING_STUB="_cyclewise_C_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_C_cyclewise.root
export DITHERING_STUB="_cyclewise_D_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_D_cyclewise.root


export COILS="13746"
export DITHERING_STUB="_run_avg_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_runwise.root
export DITHERING_STUB="_run_avg_A_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_A_runwise.root
export DITHERING_STUB="_run_avg_B_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_B_runwise.root
export DITHERING_STUB="_run_avg_C_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_C_runwise.root
export DITHERING_STUB="_run_avg_D_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13746_D_runwise.root

export COILS="13726"
export DITHERING_STUB="_run_avg_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_runwise.root
export DITHERING_STUB="_run_avg_A_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_A_runwise.root
export DITHERING_STUB="_run_avg_B_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_B_runwise.root
export DITHERING_STUB="_run_avg_C_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_C_runwise.root
export DITHERING_STUB="_run_avg_D_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_13726_D_runwise.root

export COILS="15746"
export DITHERING_STUB="_run_avg_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_runwise.root
export DITHERING_STUB="_run_avg_A_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_A_runwise.root
export DITHERING_STUB="_run_avg_B_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_B_runwise.root
export DITHERING_STUB="_run_avg_C_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_C_runwise.root
export DITHERING_STUB="_run_avg_D_1X"
./smartHadd_dit_slopes_by_run.sh $list ${prefix}dithering_slopes_15746_D_runwise.root

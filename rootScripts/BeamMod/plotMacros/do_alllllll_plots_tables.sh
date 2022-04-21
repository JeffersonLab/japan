#!/bin/bash

####./do_all_plots_tables.sh crex_part 
####./do_all_plots_tables.sh "crex_pitt" 
####./do_all_plots_tables.sh "crex_pitt" "" _NULL_both
##### To get the crex pitt data to work have to hadd IN and OUT together and use the special new script first, then just mv these crazy output names to the _NULL_both_wien suffix desired
####./do_all_plots_tables.sh "crex_pitt" "(-1*(rcdb_flip_state==2)+(rcdb_flip_state==1))*" _NULL_both
####rename \(-1\(rcdb_flip_state2\)\+\(rcdb_flip_state1\)\) wien singles_plots_tables/*
####./do_all_plots_tables.sh rcdb_ihwp 
####./do_all_plots_tables.sh rcdb_flip_state 
####./do_all_plots_tables.sh 'Entry$' "" _wienwise
####./do_all_plots_tables.sh rcdb_sign 
####./do_all_plots_tables.sh crex_slow_control 
####./do_all_plots_tables.sh crex_slow_control_simple 
####./do_all_plots_tables.sh rcdb_slug rcdb_sign*
####./do_kinematics_plots_tables.sh rcdb_slug rcdb_sign* _kinematics



#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD crex_part 
#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD "crex_pitt" 
#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD "crex_pitt" "" _NULL_both
#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD "crex_pitt" "(-1*(rcdb_flip_state==2)+(rcdb_flip_state==1))*" _NULL_both
#rename \(-1\(rcdb_flip_state2\)\+\(rcdb_flip_state1\)\) wien singles_plots_tables/*
#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD rcdb_ihwp 
#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD rcdb_flip_state 
#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD 'Entry$' "" _wienwise
#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD rcdb_sign 
#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD crex_slow_control 
#./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD crex_slow_control_simple 
./IncludeBMOD_do_all_plots_tables.sh IncludeBMOD rcdb_slug rcdb_sign*
## Added into main script now: ./IncludeBMOD_do_kinematics_plots_tables.sh IncludeBMOD rcdb_slug rcdb_sign* #_kinematics

# Special pre and post covid cuts for Ryan - slug averaging data with main dets only
#./AT_checking_do_all_plots_tables.sh IncludeBMOD rcdb_slug rcdb_sign*


####
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD crex_part 
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD "crex_pitt" 
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD "crex_pitt" "" _NULL_both
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD "crex_pitt" "(-1*(rcdb_flip_state==2)+(rcdb_flip_state==1))*" _NULL_both
####rename \(-1\(rcdb_flip_state2\)\+\(rcdb_flip_state1\)\) wien singles_plots_tables/*
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD rcdb_ihwp 
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD rcdb_flip_state 
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD 'Entry$' "" _wienwise
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD rcdb_sign 
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD crex_slow_control 
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD crex_slow_control_simple 
####./IncludeBMOD_do_all_plots_tables.sh OnlyBMOD rcdb_slug rcdb_sign*
####./IncludeBMOD_do_kinematics_plots_tables.sh OnlyBMOD rcdb_slug rcdb_sign* _kinematics

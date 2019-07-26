To reanalyze and make new run and minirun files for each run
1) Optional: Make your slug list, then if the run/minirun individual stub files haven't been made yet do:
 gojapan ; cd ../prompt/ ; ./agg_prompt_list.sh ~/PREX/prompt/collector/run_list/slug7.list
2) Once the aggregator is done running on all runs in a given slug you can hadd them together and add the units branch to them with 
 ~/PREX/prompt/accumulate_aggFiles_list.sh ~/PREX/prompt/Aggregator/drawPostpan/run_lists/slug11.list slug11.root
3) Then to make plots/csv file do
 [apar@adaq2 ~/PREX/prompt/Aggregator/drawPostpan]$ root -l -b -q plotAgg.C'("slugRootfiles/run_slug9.root","run_slug9")'

4) To look at the data by hand, find the output slug aggregated files and do: 
 root /chafs2/work1/apar/aggRootfiles/slugRootfiles/run_slug11.root
 root [1] agg->Draw("reg_asym_usl_mean:run_number","","*")

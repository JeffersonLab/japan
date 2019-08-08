To reanalyze and make new run and minirun files for each run

1) Optional: Make your slug list, then if the run/minirun individual stub files haven't been made yet do:

 gojapan ; cd ../prompt/ ; ./agg_prompt_list.sh ~/PREX/prompt/collector/run_list/slug7.list

2) Once the aggregator is done running on all runs in a given slug you can hadd them together and add the units branch to them with 

 ~/PREX/prompt/accumulate_mini_aggFiles_list.sh ~/PREX/prompt/Aggregator/drawPostpan/run_lists/slug11.list slug11.root

 - - - (NOTE: accumulate assumes minirun segmented output, but pre-slug 14 it was all one minirun in one, so use the accumulate_aggFiles_list.sh, and then HADD together if there is mixing (slug 14 and 15))

3) Then to make plots/csv file do

 - - - To append to the global aggregator (not advised, the file is too large) do 
 [apar@adaq2 ~/PREX/prompt/Aggregator/drawPostpan]$ setenv CAM_OUTPUTDIR /chafs2/work1/apar/aggRootfiles/slugRootfiles/grandRootfile 
 - - - To make a new grand aggregator that gets just one entry for the new slug do
 [apar@adaq2 ~/PREX/prompt/Aggregator/drawPostpan]$ setenv CAM_OUTPUTDIR ./

 - - - To make the plots do
 [apar@adaq2 ~/PREX/prompt/Aggregator/drawPostpan]$ root -l -b -q plotAgg.C'("aggRootfiles/slugRootfiles/minirun_slug","plots/summary_minirun_slug", slug, ihwp, wein, hrs)'
 The first parameter points to location of accumulated minirun files.
 The second parameter points to location of output plots and text file.
 The third parameter is the slug number and needs to be entered by user.
 Enter ihwp = 1 for in, 2 for out
 Enter wein = 1 for right, 2 for left
 Enter hrs = 0 for both, 1 for right only, 2 for left only

4) To upload the plots to hallaweb do
 mkdir hallaweb_sluglist/slug[number]
 cp plots/*[number]* hallaweb_sluglist/slug[number]

5) To look at the data by hand, find the output slug aggregated files and do: 
 root /chafs2/work1/apar/aggRootfiles/slugRootfiles/run_slug11.root
 root [1] agg->Draw("reg_asym_usl_mean:run_number","","*")

6) To Make grand aggregator plots do
 root -l -b -q grandAgg.C'("/chafs2/work1/apar/aggRootfiles/slugRootfiles/grandRootfile/grand_aggregator.root","outLocation...")'


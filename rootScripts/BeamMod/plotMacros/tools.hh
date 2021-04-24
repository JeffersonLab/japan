
void ToolBox::dit_slopes_averaging(TString averaging_timescale = "segment") {
  TChain* plain_dit = new TChain("dit");
  TChain* plain_AT_dit = new TChain("dit");
  TChain* dit = new TChain("dit");
  TChain* AT_dit = new TChain("dit");
  dit->AddFile(Form("respin1_data/static_part_avg_eigenvector_dit_slopes.root"));
  AT_dit->AddFile(Form("respin1_data/static_part_avg_eigenvector_AT_dit_slopes.root"));
  plain_dit->AddFile(Form("respin1_data/standard_bmod_dit_slopes.root"));
  plain_AT_dit->AddFile(Form("respin1_data/standard_bmod_AT_dit_slopes.root"));
  dit->BuildIndex("run","cyclenum");
  AT_dit->BuildIndex("run","cyclenum");

  std::vector<TString> detectors_dit = {
    "usl", "usr", "atl1", "atl2", "atr1", "atr2", "sam1", "sam2", "sam3", "sam4", "sam5", "sam6", "sam7", "sam8"
  };
  std::vector<TString> dit_cuts = {
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1", // Include a bonus cut at the end to be used for the global cut... 1 more entry than the numbe of detectors
  };
  std::vector<TString> detectors = {
    "usl", "usr", "us_avg" /*,"us_dd", ATs, SAMs*/
  };
  std::vector<TString> X_BPMs = {
    "bpm4aX",
    "bpm4eX",
    "bpm1X",
    "bpm16X",
  };
  std::vector<TString> Y_BPMs = {
    "bpm4aY",
    "bpm4eY",
    "bpm1Y",
    "bpm16Y",
  };
  std::vector<TString> E_BPMs = {
    "bpm11X",
    "bpm12X",
  };
  std::vector<TString> monitors = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  std::vector<TString> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  std::vector<TString> devices = {
    "bpm4aX",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm1X",
    "bpm1Y",
    "bpm11X",
    "bpm11Y",
    "bpm12X",
    "bpm12Y",
    "bpm16X",
    "bpm16Y"
  };
  std::vector<TString> devices5_dit = {
    "1X",
    "4aY",
    "4eX",
    "4eY",
    "12X"
  };
  std::vector<TString> devices5 = {
    "bpm1X",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm12X"
  };

  // FIXME temporary output rootfile name
  TString outFileName = Form("All_dit_outputs_doing_segment_averaging_of_slopes.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile newfile(outFileName, "recreate");
  newfile.cd();

  //TTree * out_tree_mini_eigen_reg_allbpms_tr     = new TTree("mini_eigen_reg_allbpms_tr_seg_avg",    "mini_eigen_reg_allbpms_tr_seg_avg");
  // Error: The tr branch never had it's combo branches changed by any sorting algorithm, so it's combos still live in the unsorted main branch
  //combo_segment_getter("dit_segment",mini,out_tree_mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr",cut,detectors,monitors,devices);

  // Start getting the averages
  // FIXME Using no cut (rcdb_run_type or run_flag or dit_flag) assumes that the dataset being fed in is a GOOD PRODUCTION and suitable for application to BMOD segments as well. For proof of concept comparisons the cut may not be that important, as we just want to compare eigen reg to eigen dit, etc. but for final numbers this will really matter.
  //TString cut = "dit_flag==1";
  //TString averaging_timescale = "dit_segment"; // Can also be run, rcdb_slug, rcdb_flip_state, or nothing (simply don't execute this script for nothing);
  // Part averages
  TTree * out_dit          = new TTree("dit_segmentwise_segment_averaged","dit_segmentwise_segment_averaged");
  TTree * out_AT_dit       = new TTree("dit_AT_segmentwise_segment_averaged","dit_AT_segmentwise_segment_averaged");
  TTree * out_dit_runwise     = new TTree("dit_runwise_segment_averaged","dit_runwise_segment_averaged");
  TTree * out_AT_dit_runwise  = new TTree("dit_AT_runwise_segment_averaged","dit_AT_runwise_segment_averaged");
  TTree * out_plain_dit    = new TTree("dit_plain_segmentwise_segment_averaged","dit_plain_segmentwise_segment_averaged");
  TTree * out_plain_AT_dit = new TTree("dit_plain_AT_segmentwise_segment_averaged","dit_plain_AT_segmentwise_segment_averaged");
  TTree * out_plain_dit_runwise    = new TTree("dit_plain_runwise_segment_averaged","dit_plain_runwise_segment_averaged");
  TTree * out_plain_AT_dit_runwise = new TTree("dit_plain_AT_runwise_segment_averaged","dit_plain_AT_runwise_segment_averaged");

  // Part averages
  //averaging_timescale = "crex_part";
  //cut = "dit_flag==1";
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  // FIXME This is literally just a test - not really interested in taking part-averages of slopes and combos without doing norm or using a useful timescale to get the slopes out
  combo_err_segment_getter(averaging_timescale,dit,out_dit,"#_#",detectors_dit,monitors5,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,AT_dit,out_AT_dit,"#_#",detectors_dit,monitors5,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,dit,out_dit_runwise,"#_#",detectors_dit,monitors5,dit_cuts,1);
  combo_err_segment_getter(averaging_timescale,AT_dit,out_AT_dit_runwise,"#_#",detectors_dit,monitors5,dit_cuts,1);
  combo_err_segment_getter(averaging_timescale,plain_dit,out_plain_dit,"#_#",detectors_dit,devices5_dit,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,plain_AT_dit,out_plain_AT_dit,"#_#",detectors_dit,devices5_dit,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,plain_dit,out_plain_dit_runwise,"#_#",detectors_dit,devices5_dit,dit_cuts,1);
  combo_err_segment_getter(averaging_timescale,plain_AT_dit,out_plain_AT_dit_runwise,"#_#",detectors_dit,devices5_dit,dit_cuts,1);
  out_dit->Write();
  out_AT_dit->Write();
  out_dit_runwise->Write();
  out_AT_dit_runwise->Write();
  out_plain_dit->Write();
  out_plain_AT_dit->Write();
  out_plain_dit_runwise->Write();
  out_plain_AT_dit_runwise->Write();
  delete out_dit;
  delete out_AT_dit;
  delete out_dit_runwise;
  delete out_AT_dit_runwise;
  delete out_plain_dit;
  delete out_plain_AT_dit;
  delete out_plain_dit_runwise;
  delete out_plain_AT_dit_runwise;

  // Loop over the prior file's existing TTrees and print all of those as well, uneditted.
  /*
  TFile infile(infilename);
  TKey *key;
  TIter nextkey(infile.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)infile.Get(key->GetName());
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!newfile.FindObject(key->GetName())) {
        newfile.cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    }
  }
  infile.Close();
  */

  newfile.Close();
}

void ToolBox::tg_err_averaging(TString averaging_timescale = "crex_part", Int_t draw_plots = 0) {
  TChain* mini                                  = new TChain("mini");
  TChain* agg_part_avgd_friendable              = new TChain("agg_part_avgd_friendable");
  TChain* agg_plain_friendable                  = new TChain("agg_plain_friendable");
  //TChain* mini_eigen_reg_allbpms_tr        = new TChain(Form("mini_eigen_reg_allbpms_tr"));     // Truncated one is not sorted... just deal with it I guess (could sort it later if urgently needed)
  TChain* mini_eigen_reg_5bpms_part_avg         = new TChain(Form("mini_eigen_reg_5bpms_part_avg")); // Unsorted versions have no _sorted in the name
  TChain* mini_reference_eigen_reg_5bpms        = new TChain(Form("mini_reference_eigen_reg_5bpms")); // Has the corrected asyms belonging to slopes of _sorted branch.
  TChain* mini_reference_eigen_reg_5bpms_sorted = new TChain(Form("mini_reference_eigen_reg_5bpms_sorted"));
  TChain* mini_regression                       = new TChain(Form("mini_regression"));
  TChain* mini_overload                         = new TChain(Form("mini_overload"));
  TChain* dit_part_avgd_friendable_slopes       = new TChain(Form("dit_part_avgd_friendable"));
  TChain* dit_plain_friendable_slopes           = new TChain(Form("dit_plain_friendable"));
  TChain* dit_part_avgd_friendable_sens         = new TChain(Form("dit_part_avgd_friendable"));
  TChain* dit_plain_friendable_sens             = new TChain(Form("dit_plain_friendable"));
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  TString mini_infilename = Form("respin1_data/static_part_avg_eigenvector_reg_slopes.root");
  TString agg_infilename = Form("respin1_data/aggregator_outputs_prod_only_mini_friendable.root");
  TString postpan_infilename = Form("respin1_data/mini_friended_5bpm_and_overload_postpan.root");
  TString bmod_slopes_infilename = Form("respin1_data/mini_friended_dit_slopes.root");
  TString bmod_sens_infilename = Form("respin1_data/mini_friended_dit_sensitivities.root");
  //TString infilename = Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted.root");
  mini->AddFile(mini_infilename);
  agg_part_avgd_friendable->AddFile(agg_infilename);
  agg_plain_friendable->AddFile(agg_infilename);
  //mini_eigen_reg_allbpms_tr->AddFile(infilename);
  //mini_eigen_reg_allbpms_sorted->AddFile(infilename);
  mini_eigen_reg_5bpms_part_avg->AddFile(mini_infilename);
  mini_reference_eigen_reg_5bpms->AddFile(mini_infilename);
  mini_reference_eigen_reg_5bpms_sorted->AddFile(mini_infilename);
  mini_regression->AddFile(postpan_infilename);
  mini_overload->AddFile(postpan_infilename);
  dit_part_avgd_friendable_slopes->AddFile(bmod_slopes_infilename);
  dit_plain_friendable_slopes->AddFile(bmod_slopes_infilename);
  dit_part_avgd_friendable_sens->AddFile(bmod_sens_infilename);
  dit_plain_friendable_sens->AddFile(bmod_sens_infilename);
  // Default outputs, unsorted:
  //mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  //mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->AddFriend(mini_regression,"mini_regression");
  mini->AddFriend(mini_overload,"mini_overload");
  mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  std::vector<TString> agg_cor_special_detectors = {
    "usl_reg_correction", "usr_reg_correction", "us_avg_reg_correction"// FIXME Need to update the parser to handle non-simple main_reg... // , "main_det_reg_correction", 
  };
  std::vector<TString> special_detectors = {
    "usl", "usr", "us_avg", "main_det" /*,"us_dd", ATs, SAMs*/
  };
  std::vector<TString> dit_special_detectors = { // FIXME FIXME Need to figure out a way to do averaging of usl + usr /2 to get us_avg or "main_det" dithing slopes -> correction ammounts...
    "usl", "usr", // "us_avg", "main_det"
  };
  std::vector<TString> special_differences_detectors = {
    "usl_dit_reg_diff", 
    "usr_dit_reg_diff", 
    "us_avg_dit_reg_diff", 
    //"main_det_eigen_reg_reg_diff",  // FIXME needs update
  };
  std::vector<TString> special_eigen_differences_detectors = {
    //"usl_eigen_reg_reg_diff", // FIXME FIXME part_avgd Aggregator used segment averaged slopes for eigen-regression!! This was a mistake
    "usl_dit_reg_diff",  // it is actually eigen dit for the eigen-agg rootfile (this will change in respin2)
    //"usl_dit_eigen_reg_diff", 
    //"usr_eigen_reg_reg_diff", 
    "usr_dit_reg_diff",  
    //"usr_dit_eigen_reg_diff", 
    //"us_avg_eigen_reg_reg_diff", 
    "us_avg_dit_reg_diff",  
    //"us_avg_dit_eigen_reg_diff", 
    //"main_det_eigen_reg_reg_diff",  // FIXME needs update to accomodate main_det parsing in complet titles here
    //"main_det_dit_reg_diff",        // FIXME needs update
    //"main_det_dit_eigen_reg_diff",  // FIXME needs update
  };
  /*
    asym_us_avg_eigen_reg_reg_diff_mean
    asym_us_avg_dit_reg_diff_mean
    asym_us_avg_dit_eigen_reg_diff_mean
    // Plain bmod and regs comparions: from agg_plain
    asym_us_avg_dit_reg_diff_mean
  */
  std::vector<TString> detectors = {
    //"usl",
    "usl", "usr", "us_avg" /*,"us_dd", ATs, SAMs*/
  };
  std::vector<TString> mon_cuts = {
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1", // Include 1 extra as the global 
  };
  std::vector<TString> cutGoodProduction = { // Must be == or > in length than the draws_piece1 first vector sent into the drawing scripts
    "rcdb_run_type == 1 && rcdb_run_flag == 1", // Include 1 extra as the global 
    "rcdb_run_type == 1 && rcdb_run_flag == 1", // Include 1 extra as the global 
  };
  std::vector<TString> null_cuts = {};

  std::vector<TString> special_eigen_differences_cuts = {
    //"rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1", // FIXME the eigen reg stuff in part_avgd aggregator was segment averages slopes! which is not wanted really
    "rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1",
    //"rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1",
    //"rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2",
    "rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2",
    //"rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2",
    //"rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0",
    "rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0",
    //"rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0",
    "rcdb_run_type == 1 && rcdb_run_flag == 1", // Include 1 extra as the global 
  };
  std::vector<TString> special_eigen_differences_arm_cuts = {
    "rcdb_arm_flag != 1",
    "rcdb_arm_flag != 1",
    "rcdb_arm_flag != 1",
    "rcdb_arm_flag != 2",
    "rcdb_arm_flag != 2",
    "rcdb_arm_flag != 2",
    "rcdb_arm_flag == 0",
    "rcdb_arm_flag == 0",
    "rcdb_arm_flag == 0",
    "", // Include 1 extra as the global 
  };
  std::vector<TString> arm_cuts = {
    "rcdb_arm_flag != 1",
    "rcdb_arm_flag != 2",
    "rcdb_arm_flag == 0",
    "", // Include 1 extra as the global 
  };
  std::vector<TString> cuts = {
    "rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2",
    "rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0",
    "rcdb_run_type == 1 && rcdb_run_flag == 1", // Include 1 extra as the global 
  };
  std::vector<TString> X_BPMs = {
    "bpm4aX",
    "bpm4eX",
    "bpm1X",
    "bpm16X",
  };
  std::vector<TString> Y_BPMs = {
    "bpm4aY",
    "bpm4eY",
    "bpm1Y",
    "bpm16Y",
  };
  std::vector<TString> E_BPMs = {
    "bpm11X",
    "bpm12X",
  };
  std::vector<TString> monitors = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  std::vector<TString> diff_vec = {"diff"};
  std::vector<TString> diff_yield_vec = {"diff","yield"};
  std::vector<TString> asym_vec = {"asym"};
  std::vector<TString> reg_cor_asym_vec = {"reg_asym","cor_asym"};
  std::vector<TString> reg_asym_vec = {"reg_asym"};
  std::vector<TString> dit_asym_vec = {"dit_asym"};
  std::vector<TString> asym_yield_vec = {"asym","yield"};
  //std::vector<TString> monitors5 = {"evMon0"};
  std::vector<TString> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  std::vector<TString> yield_bcm_devices = {
    "bcm_an_diff",
    "bcm_target",
    "bcm_an_us",
    "bcm_an_ds",
    "bcm_an_ds3",
    "bcm_dg_us",
    "bcm_dg_ds",
    "cav4bQ",
    "cav4cQ",
    "cav4dQ",
  };
  std::vector<TString> devices = {
    "bpm4aX",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm1X",
    "bpm1Y",
    "bpm11X",
    "bpm11Y",
    "bpm12X",
    "bpm12Y",
    "bpm16X",
    "bpm16Y",
  };
  std::vector<TString> yield_diff_BPMs = {
    "bpm4aX",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm1X",
    "bpm1Y",
    "bpm11X",
    "bpm11Y",
    "bpm12X",
    "bpm12Y",
    "bpm16X",
    "bpm16Y",
    "cav4bX",
    "cav4bY",
    "cav4cX",
    "cav4cY",
    "cav4dX",
    "cav4dY",
  };
  std::vector<TString> dit_slope_devices5 = {
    "1X",
    "4aY",
    "4eX",
    "4eY",
    "12X"
  };
  std::vector<TString> devices5 = {
    "bpm1X",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm12X"
  };

  // FIXME temporary output rootfile name
  TString outFileName = Form("processed_respin1_data/CREX_All_Slug_Avg_Outputs.root");
  //TString outFileName = Form("TEST_slug_avg_corrections_outputs.root");
  if (draw_plots == 1) {
    outFileName = "TEST_junk.root";
  }
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile newfile(outFileName, "recreate");
  newfile.cd();

  //TTree * out_tree_mini_eigen_reg_allbpms_tr     = new TTree("mini_eigen_reg_allbpms_tr_seg_avg",    "mini_eigen_reg_allbpms_tr_seg_avg");
  // Error: The tr branch never had it's combo branches changed by any sorting algorithm, so it's combos still live in the unsorted main branch
  //combo_segment_getter("dit_segment",mini,out_tree_mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr",cut,detectors,monitors,devices);

  // Start getting the averages
  // FIXME Using no cut (rcdb_run_type or run_flag or dit_flag) assumes that the dataset being fed in is a GOOD PRODUCTION and suitable for application to BMOD segments as well. For proof of concept comparisons the cut may not be that important, as we just want to compare eigen reg to eigen dit, etc. but for final numbers this will really matter.
  //TString cut = "dit_flag==1";

  // Corrections
  TTree * out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted = new TTree("mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted","mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted");
  TTree * out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted = new TTree("mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted","mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted");
  TTree * out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted = new TTree("mini_eigen_reg_5bpms_part_avg_corrections_det_weighted","mini_eigen_reg_5bpms_part_avg_corrections_det_weighted");
  TTree * out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted = new TTree("mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted","mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted");
  // FIXME postpan outputs don't have slopes in a useful format - for respin2 use dance for all analyses
  //TTree * out_tree_mini_regression_corrections_det_weighted = new TTree("mini_regression_corrections_det_weighted","mini_regression_corrections_det_weighted");
  //TTree * out_tree_mini_regression_corrections_mon_weighted = new TTree("mini_regression_corrections_mon_weighted","mini_regression_corrections_mon_weighted");
  //TTree * out_tree_mini_overload_corrections_det_weighted = new TTree("mini_overload_corrections_det_weighted","mini_overload_corrections_det_weighted");
  //TTree * out_tree_mini_overload_corrections_mon_weighted = new TTree("mini_overload_corrections_mon_weighted","mini_overload_corrections_mon_weighted");
  TTree * out_tree_mini_dit_plain_corrections_det_weighted = new TTree("mini_dit_plain_corrections_det_weighted","mini_dit_plain_corrections_det_weighted");
  TTree * out_tree_mini_dit_plain_corrections_mon_weighted = new TTree("mini_dit_plain_corrections_mon_weighted","mini_dit_plain_corrections_mon_weighted");
  TTree * out_tree_mini_dit_part_avgd_corrections_det_weighted = new TTree("mini_dit_part_avgd_corrections_det_weighted","mini_dit_part_avgd_corrections_det_weighted");
  TTree * out_tree_mini_dit_part_avgd_corrections_mon_weighted = new TTree("mini_dit_part_avgd_corrections_mon_weighted","mini_dit_part_avgd_corrections_mon_weighted");

  // Differences
  // Eigenvector bmod and regs comparison: from agg_part_avgd
  ////NO asym_us_avg_eigen_reg_reg_diff_mean
  ////asym_us_avg_dit_reg_diff_mean
  ////NO asym_us_avg_dit_eigen_reg_diff_mean
  // Plain bmod and regs comparions: from agg_plain
  ////asym_us_avg_dit_reg_diff_mean
  TTree * out_tree_mini_eigen_dit_reg_difference = new TTree("mini_eigen_dit_reg_difference","mini_eigen_dit_reg_difference");
  TTree * out_tree_mini_plain_dit_reg_difference = new TTree("mini_plain_dit_reg_difference","mini_plain_dit_reg_difference");
  

  // Asyms
  TTree * out_tree_mini_raw_det_asyms_det_weighted = new TTree("mini_raw_det_asyms_det_weighted","mini_raw_det_asyms_det_weighted");
  TTree * out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted = new TTree("mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted","mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted");
  TTree * out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted = new TTree("mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted","mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted");
  TTree * out_tree_burstwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted = new TTree("burstwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted","mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted");
  TTree * out_tree_mini_regression_det_asyms_det_weighted = new TTree("mini_regression_det_asyms_det_weighted","mini_regression_det_asyms_det_weighted");
  TTree * out_tree_mini_regression_det_asym_cors_det_weighted = new TTree("mini_regression_det_asym_cors_det_weighted","mini_regression_det_asym_cors_det_weighted");
  TTree * out_tree_mini_overload_det_asyms_det_weighted = new TTree("mini_overload_det_asyms_det_weighted","mini_overload_det_asyms_det_weighted");
  //TTree * out_tree_mini_overload_det_asym_cors_det_weighted = new TTree("mini_overload_det_asym_cors_det_weighted","mini_overload_det_asym_cors_det_weighted");
  TTree * out_tree_mini_dit_plain_det_asyms_det_weighted = new TTree("mini_dit_plain_det_asyms_det_weighted","mini_dit_plain_det_asyms_det_weighted");
  TTree * out_tree_mini_dit_part_avgd_det_asyms_det_weighted = new TTree("mini_dit_part_avgd_det_asyms_det_weighted","mini_dit_part_avgd_det_asyms_det_weighted");

  // Monitors
  TTree * out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted = new TTree("mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted","mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted");
  TTree * out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted = new TTree("mini_eigen_reg_5bpms_part_avg_monitors_det_weighted","mini_eigen_reg_5bpms_part_avg_monitors_det_weighted");
  TTree * out_tree_mini_BPMs_det_weighted = new TTree("mini_BPMs_det_weighted","mini_BPMs_det_weighted");
  TTree * out_tree_mini_BCMs_det_weighted = new TTree("mini_BCMs_det_weighted","mini_BCMs_det_weighted");

  TString draw = "0";
  TString draw_weighting_error = "0";
  TString drawn_channels_error = "0";

  if (draw_plots == 0){ // Just calculate the slug, etc. averaged quantities
  
  // Use main det asym error rescale-weighting - set to be = drawn_channels_error if no fancy weighting is desired

  // Main reference burst-wise eigenvector regression
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_5bpms.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_5bpms.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_5bpms.reg_asym_usl.err))/(ppb)";
  draw            = "-1.0*mini_reference_eigen_reg_5bpms_sorted.#_#.mean*mini_reference_eigen_reg_5bpms_sorted.diff_#.mean";
  drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted.#_#.mean*mini_reference_eigen_reg_5bpms_sorted.diff_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5,monitors5,cuts,{},0);
  out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted;
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*mini_reference_eigen_reg_5bpms_sorted.#_#.mean)+mini_reference_eigen_reg_5bpms_sorted.diff_#.err)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw                = "-1.0*mini_reference_eigen_reg_5bpms_sorted.#_#.mean*mini_reference_eigen_reg_5bpms_sorted.diff_#.mean";
  drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted.#_#.mean*mini_reference_eigen_reg_5bpms_sorted.diff_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5,monitors5,cuts,{},0);
  out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Main part-avgd eigenvector regression
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";
  draw            = "-1.0*mini_eigen_reg_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean";
  drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5,monitors5,cuts,{},0);
  out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted->Write();
  delete out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*mini_eigen_reg_5bpms_part_avg.#_#)+agg_part_avgd_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = "-1.0*mini_eigen_reg_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean";
  drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5,monitors5,cuts,{},0);
  out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted->Write();
  delete out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted;

  /* FIXME The standard Postpan outputs have the slopes stored in a matrix "coeff" which is difficult to parse without being really carefull...
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Standard regression corrections
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_regression.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_regression.reg_asym_usl.err))/(ppb)";
  draw            = "-1.0*mini_regression.#_#*agg_plain_friendable.diff_#_mean";
  drawn_channels_error = "abs(mini_regression.#_#*agg_plain_friendable.diff_#_mean_error)";
  //////////////combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_regression_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,devices5,devices5,cuts,{},0);
  //////////////out_tree_mini_regression_corrections_det_weighted->Write();
  delete out_tree_mini_regression_corrections_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*mini_regression.#_#)+agg_plain_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = "-1.0*mini_regression.#_#*agg_plain_friendable.diff_#_mean";
  drawn_channels_error = "abs(mini_regression.#_#*agg_plain_friendable.diff_#_mean_error)";
  //////////////combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_regression_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,devices5,devices5,cuts,{},0);
  //////////////out_tree_mini_regression_corrections_mon_weighted->Write();
  delete out_tree_mini_regression_corrections_mon_weighted;
  */

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Standard BMOD corrections
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(agg_plain_friendable.dit_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_plain_friendable.dit_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_plain_friendable.dit_asym_usl_mean_error))/(ppb)";
  draw            = "-1.0*dit_plain_friendable_slopes.#_#_mean*agg_plain_friendable.diff_#_mean";
  drawn_channels_error = "abs(dit_plain_friendable_slopes.#_#_mean*agg_plain_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_plain_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,dit_slope_devices5,devices5,cuts,{},0);
  out_tree_mini_dit_plain_corrections_det_weighted->Write();
  delete out_tree_mini_dit_plain_corrections_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*dit_plain_friendable_slopes.#_#_mean)+agg_plain_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = "-1.0*dit_plain_friendable_slopes.#_#_mean*agg_plain_friendable.diff_#_mean";
  drawn_channels_error = "abs(dit_plain_friendable_slopes.#_#_mean*agg_plain_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_plain_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,dit_slope_devices5,devices5,cuts,{},0);
  out_tree_mini_dit_plain_corrections_mon_weighted->Write();
  delete out_tree_mini_dit_plain_corrections_mon_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Part avgd eigenvector BMOD corrections
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(agg_part_avgd_friendable.dit_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_part_avgd_friendable.dit_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_part_avgd_friendable.dit_asym_usl_mean_error))/(ppb)";
  draw            = "-1.0*dit_part_avgd_friendable_slopes.#_#_mean*agg_part_avgd_friendable.diff_#_mean";
  drawn_channels_error = "abs(dit_part_avgd_friendable_slopes.#_#_mean*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,monitors5,monitors5,cuts,{},0);
  out_tree_mini_dit_part_avgd_corrections_det_weighted->Write();
  delete out_tree_mini_dit_part_avgd_corrections_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*dit_part_avgd_friendable_slopes.#_#_mean)+agg_part_avgd_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = "-1.0*dit_part_avgd_friendable_slopes.#_#_mean*agg_part_avgd_friendable.diff_#_mean";
  drawn_channels_error = "abs(dit_part_avgd_friendable_slopes.#_#_mean*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,monitors5,monitors5,cuts,{},0);
  out_tree_mini_dit_part_avgd_corrections_mon_weighted->Write();
  delete out_tree_mini_dit_part_avgd_corrections_mon_weighted;





  /////////// Differences between different analyses, etc.

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Difference between eigen regression and standard regression
  // Part avgd eigen reg weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";
  draw            = "agg_part_avgd_friendable.#_#_mean";
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_dit_reg_difference,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_differences_detectors,{},{},cuts,0);
  out_tree_mini_eigen_dit_reg_difference->Write();
  delete out_tree_mini_eigen_dit_reg_difference;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Difference between eigen regression and standard regression
  // Part avgd eigen reg weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_regression.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_regression.reg_asym_usl.err))/(ppb)";
  draw            = "agg_plain_friendable.#_#_mean";
  drawn_channels_error = "abs(agg_plain_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_plain_dit_reg_difference,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_differences_detectors,{},{},cuts,0);
  out_tree_mini_plain_dit_reg_difference->Write();
  delete out_tree_mini_plain_dit_reg_difference;



  /////////// Asyms, etc.

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Raw asyms
  // Main det signal weighted
  drawn_channels_error = "abs(#_#.err)";
  draw                 =     "#_#.mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_raw_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_raw_det_asyms_det_weighted->Write();
  delete out_tree_mini_raw_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Reference burstwise, (sorted, but it the corrected asyms etc. don't care about sort and live in non-sorted tree by themselves), eigenvector regression 
  // Main det signal weighted
  drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms.#_#.err)";
  draw                 =     "mini_reference_eigen_reg_5bpms.#_#.mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_5bpms.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_5bpms.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_5bpms.reg_asym_usl.err))/(ppb)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_cor_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Part avgd eigenvector regression (5bpms)
  // Main det signal weighted
  // This is the signal with which all other signals are averaged with (except self-weighted signals)
  drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg.#_#.err)";
  draw                 =     "mini_eigen_reg_5bpms_part_avg.#_#.mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_cor_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted->Write();
  delete out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Part avgd eigenvector regression (5bpms) - burstwise output this time
  // Main det signal weighted
  // This is the signal with which all other signals are averaged with (except self-weighted signals)
  drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg.#_#.err)";
  draw                 =     "mini_eigen_reg_5bpms_part_avg.#_#.mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";
  ////////////combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_burstwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_cor_asym_vec,special_detectors,{},null_cuts,cuts,1); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_burstwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted->Write();
  delete out_tree_burstwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Main standard 5bpm regression
  // For some reason the postpan analyzed data do not have the "correction value" included in their outputs (unlike Dance, which does...)
  // Correction amounts also exist in the agg tree... just not in a #_# easily parsed format of course
  // Main det signal weighted
  drawn_channels_error = "abs(mini_regression.#_#.err)";
  draw                 =     "mini_regression.#_#.mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_regression.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_regression.reg_asym_usl.err))/(ppb)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_regression_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_regression_det_asyms_det_weighted->Write();
  delete out_tree_mini_regression_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Main standard 5bpm regression - correction amount on the asym, from aggregator
  // Main det signal weighted
  drawn_channels_error = "abs(agg_plain_friendable.#_#_mean_error)";
  draw                 =     "agg_plain_friendable.#_#_mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_regression.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_regression.reg_asym_usl.err))/(ppb)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_regression_det_asym_cors_det_weighted,draw,draw_weighting_error,drawn_channels_error,asym_vec,agg_cor_special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_regression_det_asym_cors_det_weighted->Write();
  delete out_tree_mini_regression_det_asym_cors_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Overload standard regression
  // For some reason the postpan analyzed data do not have the "correction value" included in their outputs (unlike Dance, which does...)
  // Correction amounts also exist in the agg tree... just not in a #_# easily parsed format of course
  // Main det signal weighted
  drawn_channels_error = "abs(mini_overload.#_#.err)";
  draw                 =     "mini_overload.#_#.mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_overload.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_overload.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_overload.reg_asym_usl.err))/(ppb)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_overload_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_overload_det_asyms_det_weighted->Write();
  delete out_tree_mini_overload_det_asyms_det_weighted;

  /* FIXME just don't have overload regression in the aggregator ...
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Overload standard regression, asym correction amount from aggregator
  // Main det signal weighted
  drawn_channels_error = "abs(mini_overload.#_#.err)";
  draw                 =     "mini_overload.#_#.mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_overload.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_overload.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_overload.reg_asym_usl.err))/(ppb)";
  //////////combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_overload_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  //////////out_tree_mini_overload_det_asyms_det_weighted->Write();
  delete out_tree_mini_overload_det_asyms_det_weighted;
  */

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Standard BMOD analysis (segment averaged slopes, interpolate missing slopes with mini tree's help)
  // Asyms come from the standard "plain" Agg outputs
  // Correction amounts also exist in the agg tree... just not in a #_# easily parsed format of course
  // Main det signal weighted
  drawn_channels_error = "abs(agg_plain_friendable.#_#_mean_error)";
  draw                 =     "agg_plain_friendable.#_#_mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(agg_plain_friendable.dit_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_plain_friendable.dit_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_plain_friendable.dit_asym_usl_mean_error))/(ppb)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_plain_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_dit_plain_det_asyms_det_weighted->Write();
  delete out_tree_mini_dit_plain_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Part avgd eigen vector BMOD analysis (segment averaged slopes)
  // Asyms come from the "part avgd" Agg outputs
  // Correction amounts also exist in the agg tree... just not in a #_# easily parsed format of course
  // Main det signal weighted
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  draw                 =     "agg_part_avgd_friendable.#_#_mean"; // diff_evMon0.mean, etc.
  draw_weighting_error = "((rcdb_arm_flag==0)*(agg_part_avgd_friendable.dit_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_part_avgd_friendable.dit_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_part_avgd_friendable.dit_asym_usl_mean_error))/(ppb)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_dit_part_avgd_det_asyms_det_weighted->Write();
  delete out_tree_mini_dit_part_avgd_det_asyms_det_weighted;




  /////////// BPMs, monitors, etc.
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - regular eigenvector analysis, sorted
  // Main det signal weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";
  draw                 =     "mini_reference_eigen_reg_5bpms_sorted.#_#.mean"; // diff_evMon0.mean, etc.
  drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted,draw,draw_weighting_error,drawn_channels_error,diff_vec,monitors5,{},cutGoodProduction,{},0);
  out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted;
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - regular eigenvector analysis, sorted
  // Monitor signal weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";
  draw                 =     "agg_part_avgd_friendable.#_#_mean"; // diff_evMon0.mean, etc.
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted,draw,draw_weighting_error,drawn_channels_error,diff_vec,monitors5,{},cutGoodProduction,{},0);
  out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted->Write();
  delete out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted;
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - regular eigenvector analysis, sorted
  // Monitor signal weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";
  draw                 =     "agg_part_avgd_friendable.#_#_mean"; // diff_evMon0.mean, etc.
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_BPMs_det_weighted,draw,draw_weighting_error,drawn_channels_error,diff_yield_vec,yield_diff_BPMs,{},cutGoodProduction,{},0);
  out_tree_mini_BPMs_det_weighted->Write();
  delete out_tree_mini_BPMs_det_weighted;
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - regular eigenvector analysis, sorted
  // Monitor signal weighted
  draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";
  draw                 =     "agg_part_avgd_friendable.#_#_mean"; // diff_evMon0.mean, etc.
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_BCMs_det_weighted,draw,draw_weighting_error,drawn_channels_error,asym_yield_vec,yield_bcm_devices,{},cutGoodProduction,{},0);
  out_tree_mini_BCMs_det_weighted->Write();
  delete out_tree_mini_BCMs_det_weighted;
  
  // Loop over the prior file's existing TTrees and print all of those as well, uneditted.
  /*
  TFile infile(infilename);
  TKey *key;
  TIter nextkey(infile.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)infile.Get(key->GetName());
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!newfile.FindObject(key->GetName())) {
        newfile.cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    }
  }
  infile.Close();
  */
  }
  newfile.Close();
  if (draw_plots == 1){ // Instead use the multigraph command to make plots
    TFile data_file_cors("processed_respin1_data/practice/TEST_slug_avg_corrections_outputs.root","read");
    TFile data_file_asyms("processed_respin1_data/practice/TEST_slug_avg_asyms_outputs.root","read");

    TTree* mini_slugs /* Just use one of the available trees for rcdb etc.*/ = (TTree*)data_file_asyms.Get("mini_raw_det_asyms_det_weighted");
    out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted");
    out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted");
    out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_eigen_reg_5bpms_part_avg_corrections_det_weighted");
    out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted");
    out_tree_mini_dit_plain_corrections_det_weighted     = (TTree*)data_file_cors.Get("mini_dit_plain_corrections_det_weighted");
    out_tree_mini_dit_plain_corrections_mon_weighted     = (TTree*)data_file_cors.Get("mini_dit_plain_corrections_mon_weighted");
    out_tree_mini_dit_part_avgd_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_dit_part_avgd_corrections_det_weighted");
    out_tree_mini_dit_part_avgd_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_dit_part_avgd_corrections_mon_weighted");
    // Postpan corrections not possible due to bad format slopes storage - do again with Dance tool in respin2

    out_tree_mini_eigen_dit_reg_difference = (TTree*)data_file_cors.Get("mini_eigen_dit_reg_difference");
    out_tree_mini_plain_dit_reg_difference = (TTree*)data_file_cors.Get("mini_plain_dit_reg_difference");

    out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted         = (TTree*)data_file_asyms.Get("mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted");
    out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted = (TTree*)data_file_asyms.Get("mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted");
    out_tree_mini_regression_det_asyms_det_weighted                       = (TTree*)data_file_asyms.Get("mini_regression_det_asyms_det_weighted");
    out_tree_mini_regression_det_asym_cors_det_weighted                   = (TTree*)data_file_asyms.Get("mini_regression_det_asym_cors_det_weighted");
    out_tree_mini_overload_det_asyms_det_weighted                         = (TTree*)data_file_asyms.Get("mini_overload_det_asyms_det_weighted");
    out_tree_mini_dit_plain_det_asyms_det_weighted                        = (TTree*)data_file_asyms.Get("mini_dit_plain_det_asyms_det_weighted");
    out_tree_mini_dit_part_avgd_det_asyms_det_weighted                    = (TTree*)data_file_asyms.Get("mini_dit_part_avgd_det_asyms_det_weighted");


    out_tree_mini_raw_det_asyms_det_weighted                             = (TTree*)data_file_asyms.Get("mini_raw_det_asyms_det_weighted");
    out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted = (TTree*)data_file_asyms.Get("mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted");
    out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted         = (TTree*)data_file_asyms.Get("mini_eigen_reg_5bpms_part_avg_monitors_det_weighted");
    out_tree_mini_BPMs_det_weighted = (TTree*)data_file_asyms.Get("mini_BPMs_det_weighted");
    out_tree_mini_BCMs_det_weighted = (TTree*)data_file_asyms.Get("mini_BCMs_det_weighted");
    //////////out_tree_mini_eigen_reg_5bpms_part_avg_combos         = (TTree*)data_file.Get("mini_eigen_reg_5bpms_part_avg_combos");
    //////////out_tree_mini_reference_eigen_reg_5bpms_sorted_combos = (TTree*)data_file.Get("mini_reference_eigen_reg_5bpms_sorted_combos");


    mini_slugs->AddFriend(out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_plain_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_plain_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_part_avgd_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_part_avgd_corrections_mon_weighted);

    mini_slugs->AddFriend(out_tree_mini_eigen_dit_reg_difference);
    mini_slugs->AddFriend(out_tree_mini_plain_dit_reg_difference);

    mini_slugs->AddFriend(out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted);
    
    mini_slugs->AddFriend(out_tree_mini_regression_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_regression_det_asym_cors_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_overload_det_asyms_det_weighted);
    
    mini_slugs->AddFriend(out_tree_mini_dit_plain_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_part_avgd_det_asyms_det_weighted);

    mini_slugs->AddFriend(out_tree_mini_raw_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_BPMs_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_BCMs_det_weighted);


    TString corrections_pdfname = Form("TEST_corrections");
    TString ana = "Eigen BPMs combos";
    TText *label = new TText(0.0,0.005,ana.Data());
    label->SetTextFont(23);
    label->SetTextSize(12);
    label->SetNDC();

    std::ofstream outfile0;
    outfile0.open(Form("./processed_respin1_data/Results.csv"),std::ofstream::app);
    outfile0 << "Averaged outputs " << std::endl;
    outfile0 << "draw, "
             << "averaging, "
             << "draw_weighting_error, "
             << "cuts.at(icut), "
             << "avg_tmp1, "
             << "mean_err_tmp1" << std::endl
             << "     And bonus quantities: " 
             << "weighted_mean_stddev, "
             << "mean_red_chi2_err_tmp1, "
             << "mean_red_chi2_tmp1, "
             << "chi2_tmp1, "
             << "self_weighted_mean, "
             << "self_weighted_mean_err, "
             << "self_weighted_mean_stddev, "
             << "mean_self_red_chi2_tmp1, "
             << "sum_deviation_signal_self_sig2, "
             << "nen" << std::endl;
    outfile0.close();

    TVirtualPad* p1;

    
    TCanvas* c1_1 = new TCanvas();
    ana = "self-weighted BPM means (nano meters)"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c1_1->cd();
    c1_1->SetTitle(ana.Data());
    c1_1->SetName(ana.Data());
    c1_1->Divide(4,yield_diff_BPMs.size()/4);
    //c1_1->Divide(4,yield_diff_BPMs.size()/4);  // Ignore the Cavities
    //for (Int_t k = 0 ; k < devices.size() ; k++) {
      // Sign corrected
      // No error-rescaling
      // Utilize the slugwise main-det weighted data
      //p1=multiGraph(c1_1,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices.at(k).Data(),devices.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "abs(mini_BPMs_det_weighted.#_#_mean_self_err/1.0e-6)";
    draw                 =     "rcdb_sign*mini_BPMs_det_weighted.#_#_mean_self/1.0e-6"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_BPMs_det_weighted.#_#_mean_self_err/1.0e-6)";
    p1=manyGraph(c1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,diff_vec,yield_diff_BPMs,{},{},{},1); // empty draws_piece3 and cuts and cuts2 vectors
    //}

    c1_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    corrections_pdfname = "monitors_slug_avg";
    c1_1->SaveAs(Form("processed_respin1_data/%s.pdf(",corrections_pdfname.Data()));

    TCanvas* c1_2 = new TCanvas();
    ana = "main det weighted BPM means (nano meters)"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c1_2->cd();
    c1_2->SetTitle(ana.Data());
    c1_2->SetName(ana.Data());
    c1_2->Divide(4,yield_diff_BPMs.size()/4);
    //c1_2->Divide(4,yield_diff_BPMs.size()/4);  // Ignore the Cavities
    //for (Int_t k = 0 ; k < devices.size() ; k++) {
    // Sign corrected
    // No error-rescaling
    // Utilize the slugwise main-det weighted data
    //p1=multiGraph(c1_2,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices.at(k).Data(),devices.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_BPMs_det_weighted.#_#_mean_self/1.0e-6"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_BPMs_det_weighted.#_#_mean_self_err/1.0e-6)";
    p1=manyGraph(c1_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,diff_vec,yield_diff_BPMs,{},{},{},1); // empty draws_piece3 and cuts and cuts2 vectors
    //}

    c1_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_2->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_3 = new TCanvas();
    ana = "self weighted BCM means (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c1_3->cd();
    c1_3->SetTitle(ana.Data());
    c1_3->SetName(ana.Data());
    c1_3->Divide(4,yield_bcm_devices.size()/4);
    //c1_3->Divide(4,yield_bcm_devices.size()/4);  // Ignore the Cavities
    //for (Int_t k = 0 ; k < devices.size() ; k++) {
      // Sign corrected
      // No error-rescaling
      // Utilize the slugwise main-det weighted data
      //p1=multiGraph(c1_3,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices.at(k).Data(),devices.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "abs(mini_BCMs_det_weighted.#_#_mean_self_err/1.0e-9)";
    draw                 =     "rcdb_sign*mini_BCMs_det_weighted.#_#_mean_self/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_BCMs_det_weighted.#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c1_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,yield_bcm_devices,{},{},{},1); // empty draws_piece3 and cuts and cuts2 vectors
    //}

    c1_3->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_3->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_4 = new TCanvas();
    ana = "main det weighted BCM means (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c1_4->cd();
    c1_4->SetTitle(ana.Data());
    c1_4->SetName(ana.Data());
    c1_4->Divide(4,yield_bcm_devices.size()/4);
    //c1_4->Divide(4,yield_bcm_devices.size()/4);  // Ignore the Cavities
    //for (Int_t k = 0 ; k < devices.size() ; k++) {
    // Sign corrected
    // No error-rescaling
    // Utilize the slugwise main-det weighted data
    //p1=multiGraph(c1_4,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices.at(k).Data(),devices.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_BCMs_det_weighted.#_#_mean_self/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_BCMs_det_weighted.#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c1_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,yield_bcm_devices,{},{},{},1); // empty draws_piece3 and cuts and cuts2 vectors
    //}

    c1_4->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_4->SaveAs(Form("processed_respin1_data/%s.pdf)",corrections_pdfname.Data()));



    // Corrections
    corrections_pdfname = "corrections_slug_avg";

    TCanvas* c2_1 = new TCanvas();
    ana = "main det weighted - part-avg eigenvector regression - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c2_1->cd();
    c2_1->SetTitle(ana.Data());
    c2_1->SetName(ana.Data());
    c2_1->Divide(4,5); // FIXME
    //c2_1->Divide(4,special_detectors.size()/4);  // Ignore the Cavities
    //for (Int_t k = 0 ; k < devices.size() ; k++) {
      // Sign corrected
      // No error-rescaling
      // Utilize the slugwise main-det weighted data
      //p1=multiGraph(c2_1,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices.at(k).Data(),devices.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_eigen_reg_5bpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
    //}

    c2_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_1->SaveAs(Form("processed_respin1_data/%s.pdf(",corrections_pdfname.Data()));

    TCanvas* c2_2 = new TCanvas();
    ana = "main det weighted - burstwise eigenvector regression - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c2_2->cd();
    c2_2->SetTitle(ana.Data());
    c2_2->SetName(ana.Data());
    c2_2->Divide(4,5); // FIXME
    //c2_2->Divide(4,special_detectors.size()/4);  // Ignore the Cavities
    //for (Int_t k = 0 ; k < devices.size() ; k++) {
      // Sign corrected
      // No error-rescaling
      // Utilize the slugwise main-det weighted data
      //p1=multiGraph(c2_2,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices.at(k).Data(),devices.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted.#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
    //}

    c2_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_2->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_3 = new TCanvas();
    ana = "main det weighted - burstwise standard bmod - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c2_3->cd();
    c2_3->SetTitle(ana.Data());
    c2_3->SetName(ana.Data());
    c2_3->Divide(4,5); // FIXME
    //c2_3->Divide(4,special_detectors.size()/4);  // Ignore the Cavities
    //for (Int_t k = 0 ; k < devices.size() ; k++) {
      // Sign corrected
      // No error-rescaling
      // Utilize the slugwise main-det weighted data
      //p1=multiGraph(c2_3,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices.at(k).Data(),devices.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-6)";
    draw                 =     "rcdb_sign*mini_dit_plain_corrections_det_weighted.#_#_mean/1.0e-6"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_dit_plain_corrections_det_weighted.#_#_mean_err/1.0e-6)";
    p1=manyGraph(c2_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,dit_slope_devices5,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
    //}

    c2_3->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_3->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_4 = new TCanvas();
    ana = "main det weighted - burstwise eigenvector bmod - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c2_4->cd();
    c2_4->SetTitle(ana.Data());
    c2_4->SetName(ana.Data());
    c2_4->Divide(4,5); // FIXME
    //c2_4->Divide(4,special_detectors.size()/4);  // Ignore the Cavities
    //for (Int_t k = 0 ; k < devices.size() ; k++) {
      // Sign corrected
      // No error-rescaling
      // Utilize the slugwise main-det weighted data
      //p1=multiGraph(c2_4,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices.at(k).Data(),devices.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-6)";
    draw                 =     "rcdb_sign*mini_dit_part_avgd_corrections_det_weighted.#_#_mean/1.0e-6"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_dit_part_avgd_corrections_det_weighted.#_#_mean_err/1.0e-6)";
    p1=manyGraph(c2_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,monitors5,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
    //}

    c2_4->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_4->SaveAs(Form("processed_respin1_data/%s.pdf)",corrections_pdfname.Data()));



    // Multipletwise differences between analysis methods
    //                                                     
    
    corrections_pdfname = "disagreements_slug_avg_det_weighted";

    TCanvas* c2_dis_1 = new TCanvas();
    ana = "main det weighted difference between asyms, eigen vector based, eigen reg main det weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_1->cd();
    c2_dis_1->SetTitle(ana.Data());
    c2_dis_1->SetName(ana.Data());
    c2_dis_1->Divide(1,3);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_eigen_dit_reg_difference.#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_eigen_dit_reg_difference.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_differences_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c2_dis_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_1->SaveAs(Form("processed_respin1_data/%s.pdf(",corrections_pdfname.Data()));

    TCanvas* c2_dis_2 = new TCanvas();
    ana = "main det weighted difference between asyms, eigen vector based, eigen reg main det weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_2->cd();
    c2_dis_2->SetTitle(ana.Data());
    c2_dis_2->SetName(ana.Data());
    c2_dis_2->Divide(1,3);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_plain_dit_reg_difference.#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_plain_dit_reg_difference.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_differences_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c2_dis_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_2->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_3 = new TCanvas();
    ana = "main det weighted difference between asyms, eigen vector based, self weighted - distributional differences (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_3->cd();
    c2_dis_3->SetTitle(ana.Data());
    c2_dis_3->SetName(ana.Data());
    c2_dis_3->Divide(1,3);
    draw_weighting_error = "abs(mini_eigen_dit_reg_difference.#_#_mean_self_err/1.0e-9)";
    draw                 =     "rcdb_sign*mini_eigen_dit_reg_difference.#_#_mean_self/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_eigen_dit_reg_difference.#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c2_dis_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_differences_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c2_dis_3->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_3->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_4 = new TCanvas();
    ana = "main det weighted difference between asyms, eigen vector based, self weighted - distributional differences (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_4->cd();
    c2_dis_4->SetTitle(ana.Data());
    c2_dis_4->SetName(ana.Data());
    c2_dis_4->Divide(1,3);
    draw_weighting_error = "abs(mini_plain_dit_reg_difference.#_#_mean_self_err/1.0e-9)";
    draw                 =     "rcdb_sign*mini_plain_dit_reg_difference.#_#_mean_self/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_plain_dit_reg_difference.#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c2_dis_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_differences_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c2_dis_4->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_4->SaveAs(Form("processed_respin1_data/%s.pdf)",corrections_pdfname.Data()));



    // Corrected Asyms
    corrections_pdfname = "corrected_asyms_slug_avg";

    TCanvas* c3_1 = new TCanvas();
    ana = "main det weighted asyms - part avg eigenvector regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c3_1->cd();
    c3_1->SetTitle(ana.Data());
    c3_1->SetName(ana.Data());
    c3_1->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_1->SaveAs(Form("processed_respin1_data/%s.pdf(",corrections_pdfname.Data()));

    TCanvas* c3_2 = new TCanvas();
    ana = "main det weighted asyms - burstwise eigenvector regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c3_2->cd();
    c3_2->SetTitle(ana.Data());
    c3_2->SetName(ana.Data());
    c3_2->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_2->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_3 = new TCanvas();
    ana = "main det weighted asyms - burstwise standard regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c3_3->cd();
    c3_3->SetTitle(ana.Data());
    c3_3->SetName(ana.Data());
    c3_3->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_regression_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_regression_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_regression_det_asyms_det_weighted.reg_#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_regression_det_asyms_det_weighted.reg_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_3->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_3->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_4 = new TCanvas();
    ana = "main det weighted asyms - burstwise standard regression net corrections (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c3_4->cd();
    c3_4->SetTitle(ana.Data());
    c3_4->SetName(ana.Data());
    c3_4->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_regression_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_regression_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_regression_det_asym_cors_det_weighted.#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_regression_det_asym_cors_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,agg_cor_special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_4->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_4->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_5 = new TCanvas();
    ana = "main det weighted asyms - burstwise overloaded standard regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c3_5->cd();
    c3_5->SetTitle(ana.Data());
    c3_5->SetName(ana.Data());
    c3_5->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_overload_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_overload_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_overload_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_overload_det_asyms_det_weighted.reg_#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_overload_det_asyms_det_weighted.reg_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_5,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_5->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_5->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_6 = new TCanvas();
    ana = "main det weighted asyms - burstwise part avgd eigenvector dithering (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c3_6->cd();
    c3_6->SetTitle(ana.Data());
    c3_6->SetName(ana.Data());
    c3_6->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_6,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_6->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_6->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_7 = new TCanvas();
    ana = "main det weighted asyms - burstwise plain dithering (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c3_7->cd();
    c3_7->SetTitle(ana.Data());
    c3_7->SetName(ana.Data());
    c3_7->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_dit_plain_det_asyms_det_weighted.dit_#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_dit_plain_det_asyms_det_weighted.dit_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_7,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_7->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_7->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

//mini_dit_part_avgd_det_asyms_det_weighted
//mini_dit_plain_det_asyms_det_weighted

    TCanvas* c3_8 = new TCanvas();
    ana = "Raw main det weighted asyms - using part avgd eigenvector regression corrected asyms as weights (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c3_8->cd();
    c3_8->SetTitle(ana.Data());
    c3_8->SetName(ana.Data());
    c3_8->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     "rcdb_sign*mini_raw_det_asyms_det_weighted.#_#_mean/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_raw_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_8,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_8->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_8->SaveAs(Form("processed_respin1_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_9 = new TCanvas();
    ana = "Raw self weighted asyms - self weighted (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c3_9->cd();
    c3_9->SetTitle(ana.Data());
    c3_9->SetName(ana.Data());
    c3_9->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "abs(mini_raw_det_asyms_det_weighted.#_#_mean_self_err/1.0e-9)";
    draw                 =     "rcdb_sign*mini_raw_det_asyms_det_weighted.#_#_mean_self/1.0e-9"; // diff_evMon0.mean, etc.
    drawn_channels_error = "abs(mini_raw_det_asyms_det_weighted.#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c3_9,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_9->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_9->SaveAs(Form("processed_respin1_data/%s.pdf)",corrections_pdfname.Data()));

    data_file_cors.Close();
    data_file_asyms.Close();
  }
}

void ToolBox::eigenvector_averaging(TString averaging_timescale = "crex_part") {
  TChain* mini = new TChain("mini");
  //TChain* mini_eigen_reg_allbpms_tr        = new TChain(Form("mini_eigen_reg_allbpms_tr"));     // Truncated one is not sorted... just deal with it I guess (could sort it later if urgently needed)
  TChain* mini_eigen_reg_5bpms_part_avg    = new TChain(Form("mini_eigen_reg_5bpms_part_avg")); // Unsorted versions have no _sorted in the name
  TChain* mini_reference_eigen_reg_5bpms_sorted      = new TChain(Form("mini_reference_eigen_reg_5bpms_sorted"));
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  TString infilename = Form("respin1_data/static_part_avg_eigenvector_reg_slopes.root");
  //TString infilename = Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted.root");
  mini->AddFile(infilename);
  //mini_eigen_reg_allbpms_tr->AddFile(infilename);
  mini_eigen_reg_5bpms_part_avg->AddFile(infilename);
  mini_reference_eigen_reg_5bpms_sorted->AddFile(infilename);
  // Default outputs, unsorted:
  //mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  //mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->BuildIndex("run","mini");

  std::vector<TString> detectors = {
    "usl", "usr", "us_avg" /*,"us_dd", ATs, SAMs*/
  };
  std::vector<TString> mon_cuts = {
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1", // Include 1 extra as the global 
  };
  std::vector<TString> cuts = {
    "rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1",
    "rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2",
    "rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0",
    "rcdb_run_type == 1 && rcdb_run_flag == 1", // Include 1 extra as the global 
  };
  std::vector<TString> X_BPMs = {
    "bpm4aX",
    "bpm4eX",
    "bpm1X",
    "bpm16X",
  };
  std::vector<TString> Y_BPMs = {
    "bpm4aY",
    "bpm4eY",
    "bpm1Y",
    "bpm16Y",
  };
  std::vector<TString> E_BPMs = {
    "bpm11X",
    "bpm12X",
  };
  std::vector<TString> monitors = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  std::vector<TString> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  std::vector<TString> devices = {
    "bpm4aX",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm1X",
    "bpm1Y",
    "bpm11X",
    "bpm11Y",
    "bpm12X",
    "bpm12Y",
    "bpm16X",
    "bpm16Y"
  };
  std::vector<TString> devices5 = {
    "bpm1X",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm12X"
  };

  // FIXME temporary output rootfile name
  TString outFileName = Form("TEST_outputs.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile newfile(outFileName, "recreate");
  newfile.cd();

  //TTree * out_tree_mini_eigen_reg_allbpms_tr     = new TTree("mini_eigen_reg_allbpms_tr_seg_avg",    "mini_eigen_reg_allbpms_tr_seg_avg");
  // Error: The tr branch never had it's combo branches changed by any sorting algorithm, so it's combos still live in the unsorted main branch
  //combo_segment_getter("dit_segment",mini,out_tree_mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr",cut,detectors,monitors,devices);

  // Start getting the averages
  // FIXME Using no cut (rcdb_run_type or run_flag or dit_flag) assumes that the dataset being fed in is a GOOD PRODUCTION and suitable for application to BMOD segments as well. For proof of concept comparisons the cut may not be that important, as we just want to compare eigen reg to eigen dit, etc. but for final numbers this will really matter.
  //TString cut = "dit_flag==1";
  //TString averaging_timescale = "dit_segment"; // Can also be run, rcdb_slug, rcdb_flip_state, or nothing (simply don't execute this script for nothing);
  // Part averages
  TTree * out_tree_mini_eigen_reg_allbpms_sorted_part_pruned = new TTree("mini_eigen_reg_allbpms_sorted_pruned_part_avg","mini_eigen_reg_allbpms_sorted_pruned_part_avg");
  TTree * out_tree_mini_eigen_reg_5bpms_sorted_part_pruned   = new TTree("mini_eigen_reg_5bpms_sorted_pruned_part_avg",  "mini_eigen_reg_5bpms_sorted_pruned_part_avg");
  TTree * out_tree_mini_eigen_reg_allbpms_sorted_part = new TTree("mini_eigen_reg_allbpms_sorted_part_avg","mini_eigen_reg_allbpms_sorted_part_avg");
  TTree * out_tree_mini_eigen_reg_5bpms_sorted_part   = new TTree("mini_eigen_reg_5bpms_sorted_part_avg",  "mini_eigen_reg_5bpms_sorted_part_avg");

  // Part averages
  //averaging_timescale = "crex_part";
  //cut = "dit_flag==1";
  // FIXME This is literally just a test - not really interested in taking part-averages of slopes and combos without doing norm or using a useful timescale to get the slopes out
  combo_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_part_pruned,"mini_eigen_reg_5bpms_part_avg.#_#",detectors,monitors5,cuts,0);
  combo_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_part_pruned,"mini_reference_eigen_reg_5bpms_sorted.#_#",monitors5,devices5,mon_cuts,0);
  combo_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_part,"mini_eigen_reg_5bpms_part_avg.#_#",detectors,monitors5,cuts,0);
  combo_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_part,"mini_reference_eigen_reg_5bpms_sorted.#_#",monitors5,devices5,mon_cuts,0);
  out_tree_mini_eigen_reg_allbpms_sorted_part_pruned->Write();
  out_tree_mini_eigen_reg_5bpms_sorted_part_pruned->Write();
  out_tree_mini_eigen_reg_allbpms_sorted_part->Write();
  out_tree_mini_eigen_reg_5bpms_sorted_part->Write();
  delete out_tree_mini_eigen_reg_allbpms_sorted_part_pruned;
  delete out_tree_mini_eigen_reg_5bpms_sorted_part_pruned;
  delete out_tree_mini_eigen_reg_allbpms_sorted_part;
  delete out_tree_mini_eigen_reg_5bpms_sorted_part;

  // Loop over the prior file's existing TTrees and print all of those as well, uneditted.
  TFile infile(infilename);
  TKey *key;
  TIter nextkey(infile.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)infile.Get(key->GetName());
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!newfile.FindObject(key->GetName())) {
        newfile.cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    }
  }
  infile.Close();

  newfile.Close();
}

void ToolBox::manage_postpan_copy_and_friend_to_mini() {
  TTree* mini;
  TString mini_infilename = Form("respin1_data/static_part_avg_eigenvector_reg_slopes.root");
  TFile mini_file(mini_infilename);
  mini_file.GetObject("mini",mini);

  TTree* mini_postpan_regression;
  TTree* mini_postpan_overload;

  TString postpan_regression_infilename = Form("respin1_data/regress_postpan_full_CREX.root");
  TString postpan_overload_infilename = Form("respin1_data/regress_overload_full_CREX.root");
  TFile postpan_regression_in_file(postpan_regression_infilename);
  TFile postpan_overload_in_file(postpan_overload_infilename);
  postpan_regression_in_file.GetObject("mini",mini_postpan_regression);
  postpan_overload_in_file.GetObject("mini",mini_postpan_overload);
  TString outFileName = Form("mini_friended_5bpm_and_overload_postpan.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile* newfile = TFile::Open(outFileName, "recreate");
  newfile->cd();
  
  TTree* mini_out_postpan_regression = mini_postpan_regression->CloneTree(0);
  mini_out_postpan_regression->SetName("mini_regression");
  TTree* mini_out_postpan_overload = mini_postpan_overload->CloneTree(0);
  mini_out_postpan_overload->SetName("mini_overload");

  postpan_copy_and_friend_to_mini(mini,mini_postpan_regression,mini_out_postpan_regression);
  postpan_copy_and_friend_to_mini(mini,mini_postpan_overload,mini_out_postpan_overload);
  
  mini_out_postpan_regression->Write();
  delete mini_out_postpan_regression;

  mini_out_postpan_overload->Write();
  delete mini_out_postpan_overload;
  /*
  // Loop over the prior file's existing TTrees and print all of those as well, uneditted.
  TFile infile(mini_infilename);
  TKey *key;
  TIter nextkey(infile.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)infile.Get(key->GetName());
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!newfile.FindObject(key->GetName())) {
        newfile.cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    }
  }
  infile.Close();
  */

  newfile->Close();
}

void ToolBox::manage_dit_copy_and_friend_to_mini() {
  TTree* mini;
  TString mini_infilename = Form("respin1_data/static_part_avg_eigenvector_reg_slopes.root");
  TFile mini_file(mini_infilename);
  mini_file.GetObject("mini",mini);

  TTree* dit_in_part_avgd;
  TTree* dit_in_plain;
  //TTree* dit_in_AT_part_avgd;
  //TTree* dit_in_AT_plain;

  TString dit_part_avgd_infilename = Form("respin1_data/static_part_avg_eigenvector_dit_slopes.root");
  TString dit_plain_infilename = Form("respin1_data/standard_bmod_dit_slopes.root");
  //TString dit_AT_part_avgd_infilename = Form("respin1_data/static_part_avg_eigenvector_AT_dit_slopes.root");
  //TString dit_AT_plain_infilename = Form("respin1_data/standard_bmod_AT_dit_slopes.root");
  TFile dit_part_avgd_in_file(dit_part_avgd_infilename);
  TFile dit_plain_in_file(dit_plain_infilename);
  //TFile dit_part_avgd_in_file(dit_AT_part_avgd_infilename);
  //TFile dit_plain_in_file(dit_AT_plain_infilename);
  dit_part_avgd_in_file.GetObject("dit",dit_in_part_avgd);
  dit_plain_in_file.GetObject("dit",dit_in_plain);
  //dit_AT_part_avgd_in_file.GetObject("dit",dit_in_AT_part_avgd);
  //dit_AT_plain_in_file.GetObject("dit",dit_in_AT_plain);
  //dit_in_file.GetObject("dit",dit_in);
  TString outFileName = Form("mini_friended_dit_sensitivities.root");
  //TString outFileName = Form("mini_friended_sensitivities_dit.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile* newfile = TFile::Open(outFileName, "recreate");
  newfile->cd();
  
  TTree* dit_out_part_avgd = dit_in_part_avgd->CloneTree(0);
  dit_out_part_avgd->SetName("dit_part_avgd_friendable");
  TTree* dit_out_plain = dit_in_plain->CloneTree(0);
  dit_out_plain->SetName("dit_plain_friendable");
  TTree* dit_out_unfriended_part_avgd = dit_in_part_avgd->CloneTree(0);
  dit_out_unfriended_part_avgd->SetName("dit_part_avgd_unfriendable");
  TTree* dit_out_unfriended_plain = dit_in_plain->CloneTree(0);
  dit_out_unfriended_plain->SetName("dit_plain_unfriendable");

  dit_copy_and_friend_to_mini(mini,dit_in_part_avgd,dit_out_part_avgd,dit_out_unfriended_part_avgd);
  dit_copy_and_friend_to_mini(mini,dit_in_plain,dit_out_plain,dit_out_unfriended_plain);
  
  dit_out_part_avgd->Write();
  delete dit_out_part_avgd;
  dit_out_unfriended_part_avgd->Write();
  delete dit_out_unfriended_part_avgd;

  dit_out_plain->Write();
  delete dit_out_plain;
  dit_out_unfriended_plain->Write();
  delete dit_out_unfriended_plain;
  /*
  // Loop over the prior file's existing TTrees and print all of those as well, uneditted.
  TFile infile(mini_infilename);
  TKey *key;
  TIter nextkey(infile.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)infile.Get(key->GetName());
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!newfile.FindObject(key->GetName())) {
        newfile.cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    }
  }
  infile.Close();
  */

  newfile->Close();
}

void ToolBox::expand_dit_averaged_slopes() {
  TTree* mini;
  TString mini_infilename = Form("respin1_data/static_part_avg_eigenvector_reg_slopes.root");
  TFile mini_file(mini_infilename);
  mini_file.GetObject("mini",mini);

  TTree* dit_in_part_avgd;
  TTree* dit_in_plain;
  //TTree* dit_in_AT_part_avgd;
  //TTree* dit_in_AT_plain;

  TString dit_part_avgd_infilename = Form("respin1_data/All_dit_outputs_doing_segment_averaging_of_slopes.root");
  TString dit_plain_infilename = Form("respin1_data/All_dit_outputs_doing_segment_averaging_of_slopes.root");
  //TString dit_AT_part_avgd_infilename = Form("respin1_data/static_part_avg_eigenvector_AT_dit_slopes.root");
  //TString dit_AT_plain_infilename = Form("respin1_data/standard_bmod_AT_dit_slopes.root");
  TFile dit_part_avgd_in_file(dit_part_avgd_infilename);
  TFile dit_plain_in_file(dit_plain_infilename);
  //TFile dit_part_avgd_in_file(dit_AT_part_avgd_infilename);
  //TFile dit_plain_in_file(dit_AT_plain_infilename);
  dit_part_avgd_in_file.GetObject("dit_segmentwise_segment_averaged",dit_in_part_avgd);
  dit_plain_in_file.GetObject("dit_plain_segmentwise_segment_averaged",dit_in_plain);
  //dit_AT_part_avgd_in_file.GetObject("dit",dit_in_AT_part_avgd);
  //dit_AT_plain_in_file.GetObject("dit",dit_in_AT_plain);
  //dit_in_file.GetObject("dit",dit_in);
  TString outFileName = Form("mini_friended_dit_slopes.root");
  //TString outFileName = Form("mini_friended_sensitivities_dit.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile* newfile = TFile::Open(outFileName, "recreate");
  newfile->cd();
  
  TTree* dit_out_part_avgd = dit_in_part_avgd->CloneTree(0);
  dit_out_part_avgd->SetName("dit_part_avgd_friendable");
  TTree* dit_out_plain = dit_in_plain->CloneTree(0);
  dit_out_plain->SetName("dit_plain_friendable");

  dit_segmentwise_copy_and_friend_to_mini(mini,dit_in_part_avgd,dit_out_part_avgd);
  dit_segmentwise_copy_and_friend_to_mini(mini,dit_in_plain,dit_out_plain);
  
  dit_out_part_avgd->Write();
  delete dit_out_part_avgd;
  dit_out_plain->Write();
  delete dit_out_plain;
  /*
  // Loop over the prior file's existing TTrees and print all of those as well, uneditted.
  TFile infile(mini_infilename);
  TKey *key;
  TIter nextkey(infile.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)infile.Get(key->GetName());
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!newfile.FindObject(key->GetName())) {
        newfile.cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    }
  }
  infile.Close();
  */

  newfile->Close();
}

void ToolBox::manage_aggregator_friend_to_mini() {
  TTree* mini;
  TString mini_infilename = Form("respin1_data/static_part_avg_eigenvector_reg_slopes.root");
  TFile mini_file(mini_infilename);
  mini_file.GetObject("mini",mini);
  mini->BuildIndex("run","mini");

  TTree* agg_in_part_avgd;
  TString agg_part_avgd_infilename = Form("respin1_data/static_part_avg_eigenvector_aggregator_outputs.root");
  TFile agg_part_avgd_in_file(agg_part_avgd_infilename);
  agg_part_avgd_in_file.GetObject("agg",agg_in_part_avgd);

  TTree* agg_in_plain;
  TString agg_plain_infilename = Form("respin1_data/standard_bmod_aggregator_outputs.root");
  TFile agg_plain_in_file(agg_plain_infilename);
  agg_plain_in_file.GetObject("agg",agg_in_plain);

  TString outFileName = Form("mini_friended_agg.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile* newfile = TFile::Open(outFileName, "recreate");
  newfile->cd();
  
  TTree* agg_out_part_avgd = agg_in_part_avgd->CloneTree(0);
  agg_out_part_avgd->SetName("agg_part_avgd_friendable");
  TTree* agg_unfriended_part_avgd = (TTree*)agg_in_part_avgd->CloneTree(0);
  agg_unfriended_part_avgd->SetName("agg_part_avgd_unfriended");

  TTree* agg_out_plain = agg_in_plain->CloneTree(0);
  agg_out_plain->SetName("agg_plain_friendable");
  TTree* agg_unfriended_plain = (TTree*)agg_in_plain->CloneTree(0);
  agg_unfriended_plain->SetName("agg_plain_unfriended");

  aggregator_friend_to_mini(mini,agg_in_part_avgd,agg_out_part_avgd,agg_unfriended_part_avgd);
  aggregator_friend_to_mini(mini,agg_in_plain,agg_out_plain,agg_unfriended_plain);

  agg_out_part_avgd->Write();
  delete agg_out_part_avgd;

  agg_unfriended_part_avgd->Write();
  delete agg_unfriended_part_avgd;

  agg_out_plain->Write();
  delete agg_out_plain;

  agg_unfriended_plain->Write();
  delete agg_unfriended_plain;

  /*
  // Loop over the prior file's existing TTrees and print all of those as well, uneditted.
  TFile infile(mini_infilename);
  TKey *key;
  TIter nextkey(infile.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)infile.Get(key->GetName());
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!newfile.FindObject(key->GetName())) {
        newfile.cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    }
  }
  infile.Close();
  */

  newfile->Close();

}


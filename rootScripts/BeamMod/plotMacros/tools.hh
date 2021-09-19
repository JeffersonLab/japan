
void ToolBox::dit_slopes_averaging(TString averaging_timescale = "segment") {
  TChain* plain_cyclewise_dit  = new TChain("dit");
  TChain* plain_run_avg_dit    = new TChain("dit");
  TChain* eigen_seg_avg_dit    = new TChain("dit");
  TChain* eigen_part_avg_dit   = new TChain("dit");
  TChain* eigen_part_avg_allbpms_dit   = new TChain("dit");
  TChain* eigen_part_b_avg_dit = new TChain("dit");
  plain_cyclewise_dit  -> AddFile(Form("respin2_data/plain_dit_slopes_cyclewise.root"));
  plain_run_avg_dit    -> AddFile(Form("respin2_data/plain_dit_slopes_run_avg.root"));
  eigen_seg_avg_dit    -> AddFile(Form("respin2_data/eigen_seg_avg_dit_slopes_run_avg.root"));
  eigen_part_avg_dit   -> AddFile(Form("respin2_data/eigen_part_avg_dit_slopes_run_avg.root"));
  eigen_part_avg_allbpms_dit   -> AddFile(Form("respin2_data/eigen_part_avg_allbpms_dit_slopes_run_avg.root"));
  eigen_part_b_avg_dit -> AddFile(Form("respin2_data/eigen_part_b_avg_dit_slopes_run_avg.root"));

  plain_cyclewise_dit  -> BuildIndex("run","cyclenum");
  plain_run_avg_dit    -> BuildIndex("run","cyclenum");
  eigen_seg_avg_dit    -> BuildIndex("run","cyclenum");
  eigen_part_avg_dit   -> BuildIndex("run","cyclenum");
  eigen_part_avg_allbpms_dit   -> BuildIndex("run","cyclenum");
  eigen_part_b_avg_dit -> BuildIndex("run","cyclenum");

  std::vector<TString> detectors_dit = {
    "us_avg", "us_dd", "usl", "usr", "atl1", "atl2", "atr1", "atr2", "sam1", "sam2", "sam3", "sam4", "sam5", "sam6", "sam7", "sam8"
  };
  std::vector<TString> dit_cuts = {
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0",
    "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0",
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
    "us_avg", "us_dd", "usl", "usr", "atl1", "atl2", "atr1", "atr2" /*,"us_dd", ATs, SAMs*/
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
  std::vector<TString> monitors12 = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  std::vector<TString> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  std::vector<TString> devices12 = {
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
  TString outFileName = Form("processed_respin2_data/All_dit_outputs_doing_segment_averaging_of_slopes.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile newfile(outFileName, "recreate");
  newfile.cd();

  //TTree * out_tree_mini_eigen_reg_allbpms_tr     = new TTree("mini_eigen_reg_allbpms_tr_seg_avg",    "mini_eigen_reg_allbpms_tr_seg_avg");
  // Error: The tr branch never had it's combo branches changed by any sorting algorithm, so it's combos still live in the unsorted main branch
  //combo_segment_getter("dit_segment",mini,out_tree_mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr",cut,detectors,monitors12,devices12);

  // Start getting the averages
  // FIXME Using no cut (rcdb_run_type or run_flag or dit_flag) assumes that the dataset being fed in is a GOOD PRODUCTION and suitable for application to BMOD segments as well. For proof of concept comparisons the cut may not be that important, as we just want to compare eigen reg to eigen dit, etc. but for final numbers this will really matter.
  //TString cut = "dit_flag==1";
  //TString averaging_timescale = "dit_segment"; // Can also be run, rcdb_slug, rcdb_flip_state, or nothing (simply don't execute this script for nothing);
  // averages
  TTree * segmentwise_plain_cyclewise_dit  = new TTree("dit_segmentwise_plain_cyclewise_segment_averaged","dit_segmentwise_plain_cyclewise_segment_averaged");
  TTree * runwise_plain_cyclewise_dit      = new TTree("dit_runwise_plain_cyclewise_segment_averaged","dit_runwise_plain_cyclewise_segment_averaged");
  TTree * segmentwise_plain_run_avg_dit    = new TTree("dit_segmentwise_plain_run_avg_segment_averaged","dit_segmentwise_plain_run_avg_segment_averaged");
  TTree * runwise_plain_run_avg_dit        = new TTree("dit_runwise_plain_run_avg_segment_averaged","dit_runwise_plain_run_avg_segment_averaged");
  TTree * segmentwise_eigen_seg_avg_dit    = new TTree("dit_segmentwise_eigen_seg_avg_run_avg_segment_averaged","dit_segmentwise_eigen_seg_avg_run_avg_segment_averaged");
  TTree * runwise_eigen_seg_avg_dit        = new TTree("dit_runwise_eigen_seg_avg_run_avg_segment_averaged","dit_runwise_eigen_seg_avg_run_avg_segment_averaged");
  TTree * segmentwise_eigen_part_avg_dit   = new TTree("dit_segmentwise_eigen_part_avg_run_avg_segment_averaged","dit_segmentwise_eigen_part_avg_run_avg_segment_averaged");
  TTree * runwise_eigen_part_avg_dit       = new TTree("dit_runwise_eigen_part_avg_run_avg_segment_averaged","dit_runwise_eigen_part_avg_run_avg_segment_averaged");
  TTree * segmentwise_eigen_part_avg_allbpms_dit   = new TTree("dit_segmentwise_eigen_part_avg_allbpms_run_avg_segment_averaged","dit_segmentwise_eigen_part_avg_allbpms_run_avg_segment_averaged");
  TTree * runwise_eigen_part_avg_allbpms_dit       = new TTree("dit_runwise_eigen_part_avg_allbpms_run_avg_segment_averaged","dit_runwise_eigen_part_avg_allbpms_run_avg_segment_averaged");
  TTree * segmentwise_eigen_part_b_avg_dit = new TTree("dit_segmentwise_eigen_part_b_avg_run_avg_segment_averaged","dit_segmentwise_eigen_part_b_avg_run_avg_segment_averaged");
  TTree * runwise_eigen_part_b_avg_dit     = new TTree("dit_runwise_eigen_part_b_avg_run_avg_segment_averaged","dit_runwise_eigen_part_b_avg_run_avg_segment_averaged");

  // averages
  //averaging_timescale = "crex_part";
  //cut = "dit_flag==1";
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  // FIXME This is literally just a test - not really interested in taking part-averages of slopes and combos without doing norm or using a useful timescale to get the slopes out
  combo_err_segment_getter(averaging_timescale,plain_cyclewise_dit ,segmentwise_plain_cyclewise_dit ,"#_#",detectors_dit,devices5_dit,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,plain_cyclewise_dit ,runwise_plain_cyclewise_dit     ,"#_#",detectors_dit,devices5_dit,dit_cuts,1);
  combo_err_segment_getter(averaging_timescale,plain_run_avg_dit   ,segmentwise_plain_run_avg_dit   ,"#_#",detectors_dit,devices5_dit,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,plain_run_avg_dit   ,runwise_plain_run_avg_dit       ,"#_#",detectors_dit,devices5_dit,dit_cuts,1);
  combo_err_segment_getter(averaging_timescale,eigen_seg_avg_dit   ,segmentwise_eigen_seg_avg_dit   ,"#_#",detectors_dit,monitors5,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,eigen_seg_avg_dit   ,runwise_eigen_seg_avg_dit       ,"#_#",detectors_dit,monitors5,dit_cuts,1);
  combo_err_segment_getter(averaging_timescale,eigen_part_avg_dit  ,segmentwise_eigen_part_avg_dit  ,"#_#",detectors_dit,monitors5,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,eigen_part_avg_dit  ,runwise_eigen_part_avg_dit      ,"#_#",detectors_dit,monitors5,dit_cuts,1);
  combo_err_segment_getter(averaging_timescale,eigen_part_avg_allbpms_dit  ,segmentwise_eigen_part_avg_allbpms_dit  ,"#_#",detectors_dit,monitors12,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,eigen_part_avg_allbpms_dit  ,runwise_eigen_part_avg_allbpms_dit      ,"#_#",detectors_dit,monitors12,dit_cuts,1);
  combo_err_segment_getter(averaging_timescale,eigen_part_b_avg_dit,segmentwise_eigen_part_b_avg_dit,"#_#",detectors_dit,monitors5,dit_cuts,0);
  combo_err_segment_getter(averaging_timescale,eigen_part_b_avg_dit,runwise_eigen_part_b_avg_dit    ,"#_#",detectors_dit,monitors5,dit_cuts,1);

  segmentwise_plain_cyclewise_dit ->Write();
  runwise_plain_cyclewise_dit     ->Write();
  segmentwise_plain_run_avg_dit   ->Write();
  runwise_plain_run_avg_dit       ->Write();
  segmentwise_eigen_seg_avg_dit   ->Write();
  runwise_eigen_seg_avg_dit       ->Write();
  segmentwise_eigen_part_avg_dit  ->Write();
  runwise_eigen_part_avg_dit      ->Write();
  segmentwise_eigen_part_avg_allbpms_dit  ->Write();
  runwise_eigen_part_avg_allbpms_dit      ->Write();
  segmentwise_eigen_part_b_avg_dit->Write();
  runwise_eigen_part_b_avg_dit    ->Write();

  delete segmentwise_plain_cyclewise_dit ;
  delete runwise_plain_cyclewise_dit     ;
  delete segmentwise_plain_run_avg_dit   ;
  delete runwise_plain_run_avg_dit       ;
  delete segmentwise_eigen_seg_avg_dit   ;
  delete runwise_eigen_seg_avg_dit       ;
  delete segmentwise_eigen_part_avg_dit  ;
  delete runwise_eigen_part_avg_dit      ;
  delete segmentwise_eigen_part_avg_allbpms_dit  ;
  delete runwise_eigen_part_avg_allbpms_dit      ;
  delete segmentwise_eigen_part_b_avg_dit;
  delete runwise_eigen_part_b_avg_dit    ;

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

void ToolBox::tg_err_averaging(TString averaging_timescale = "crex_part", Int_t draw_plots = 0, TString mod_draw = "", TString mod_cut = "", TString DataSetCut = "ErrorFlag", TString type = "part", TString do_err = "", TString suffix = "") {
  TChain* mini                                  = new TChain("mini");
  TChain* agg_part_avgd_friendable              = new TChain(Form("agg_%s_%s_avgd%s_friendable",DataSetCut.Data(),type.Data(),do_err.Data()));
  TChain* agg_part_avgd_allbpms_friendable      = new TChain(Form("agg_%s_%s_avgd_allbpms%s_friendable",DataSetCut.Data(),type.Data(),do_err.Data()));
  TChain* agg_plain_friendable                  = new TChain("agg_plain_friendable");
  //TChain* mini_eigen_reg_allbpms_tr           = new TChain(Form("mini_eigen_reg_allbpms_tr"));     // Truncated one is not sorted... just deal with it I guess (could sort it later if urgently needed)
  // evMons stuff mostly unused it seems.... those plots should just be made separately in the obvious places
  TChain* mini_evMons_5bpms_part_avg            = new TChain(Form("mini_eigen_reg_5bpms_sorted_%s_avg",type.Data())); // Unsorted versions have no _sorted in the name
  TChain* mini_eigen_reg_5bpms_part_avg         = new TChain(Form("mini_eigen_reg_5bpms_%s_avg",type.Data())); // Unsorted versions have no _sorted in the name
  TChain* mini_eigen_lagr_5bpms_part_avg        = new TChain(Form("mini_eigen_lagr_5bpms_%s_avg",type.Data())); // Unsorted versions have no _sorted in the name
  TChain* mini_reference_eigen_reg_5bpms        = new TChain(Form("mini_eigen_reg_5bpms")); // Has the corrected asyms belonging to slopes of _sorted branch.
  TChain* mini_reference_eigen_reg_5bpms_sorted = new TChain(Form("mini_eigen_reg_5bpms_sorted"));
  TChain* mini_evMons_allbpms_part_avg          = new TChain(Form("mini_eigen_reg_allbpms_sorted_%s_avg",type.Data())); // Unsorted versions have no _sorted in the name
  TChain* mini_eigen_reg_allbpms_part_avg       = new TChain(Form("mini_eigen_reg_allbpms_%s_avg",type.Data())); // Unsorted versions have no _sorted in the name
  TChain* mini_eigen_lagr_allbpms_part_avg      = new TChain(Form("mini_eigen_lagr_allbpms_%s_avg",type.Data())); // Unsorted versions have no _sorted in the name
  TChain* mini_reference_eigen_reg_allbpms      = new TChain(Form("mini_eigen_reg_allbpms")); // Has the corrected asyms belonging to slopes of _sorted branch.
  TChain* mini_reference_eigen_reg_allbpms_sorted=new TChain(Form("mini_eigen_reg_allbpms_sorted"));
  TChain* mini_regression                       = new TChain(Form("mini"));
  TChain* mini_overload                         = new TChain(Form("mini"));
  TChain* dit_part_avgd_friendable_slopes       = new TChain(Form("dit_%s_avgd_friendable",type.Data()));
  // DOESN'T EXIST due to 5bpm limit on dithering
  // TChain* dit_part_avgd_allbpms_friendable_slopes=new TChain(Form("dit_%s_avgd_allbpms_friendable",type.Data()));
  TChain* dit_plain_friendable_slopes           = new TChain(Form("dit_plain_run_avg_friendable"));
  // unused sensitivities.... replace with friended stuff ---> Replace with outputs in workspace_plotting_macros/july_21_plots/residuals_*.root (move to processed respin2) ---> Do those in a separate list of TChain reads and such at the end of this massive function... for sanity purposes and reproducibility
  TChain* dit_part_avgd_friendable_sens         = new TChain(Form("dit_%s_avgd_tagged_friendable",type.Data()));  // Runwise expanded data
  TChain* dit_part_avgd_allbpms_friendable_sens = new TChain(Form("dit_%s_avgd_tagged_friendable",type.Data()));
  TChain* dit_plain_friendable_sens             = new TChain(Form("dit_plain_run_avg_friendable"));
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  TString mini_do_err = "";
  TString lagr_err = "_no_err";
  if (do_err == "_no_err") { 
    mini_do_err = "_noerr";
    lagr_err = "";
  }
  TString mini_infilename = Form("respin2_data/eigen%s_%s_avg_lagr_slopes.root",mini_do_err.Data(),type.Data()); //eigen_noerr_.... other options ; do_err="_noerr" for no err version lagrange sens calculated data
  TString mini_allbpms_infilename = Form("respin2_data/eigen%s_%s_avg_allbpms_lagr_slopes.root",mini_do_err.Data(),type.Data()); //eigen_noerr_.... other options ; do_err="_noerr" for no err version lagrange sens calculated data
  TString bigType = "Part";
  if (type == "part_b"){
    bigType = "Part_B";
  }
  if (type == "Seg"){
    bigType = "Segment";
  }
  TString mini_reference_evmon_infilename = Form("processed_respin2_data/%s_Avg_EigenCombos_double_sorted.root",bigType.Data()); //eigen_noerr_.... other options ; do_err="_noerr" for no err version lagrange sens calculated data
  TString agg_infilename = Form("processed_respin2_data/mini_friended_agg.root");
  TString regress_postpan_infilename = Form("respin2_data/regress_postpan_full_CREX.root");
  TString overload_postpan_infilename = Form("respin2_data/regress_overload_full_CREX.root");
  TString bmod_slopes_infilename = Form("processed_respin2_data/All_dit_outputs_doing_segment_averaging_of_slopes_mini_friended.root");
  // Replace this with new friended outputs
  TString bmod_sens_infilename = Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_run_avg_eigenvectors_double_sorted_%s_avg_1X/respin2_All_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg_tag_expanded.root",type.Data());
  TString bmod_allbpms_sens_infilename = Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_run_avg_eigenvectors_double_sorted_%s_avg_allbpms_1X/respin2_All_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg_tag_expanded.root",type.Data());
  mini->AddFile(mini_infilename);
  agg_part_avgd_friendable->AddFile(agg_infilename);
  agg_part_avgd_allbpms_friendable->AddFile(agg_infilename);
  agg_plain_friendable->AddFile(agg_infilename);
  //mini_eigen_reg_allbpms_tr->AddFile(infilename);
  //mini_eigen_reg_allbpms_sorted->AddFile(infilename);
  mini_evMons_5bpms_part_avg->AddFile(mini_reference_evmon_infilename);
  mini_evMons_allbpms_part_avg->AddFile(mini_reference_evmon_infilename);
  mini_eigen_reg_5bpms_part_avg->AddFile(mini_infilename);
  mini_eigen_reg_allbpms_part_avg->AddFile(mini_allbpms_infilename);
  mini_eigen_lagr_5bpms_part_avg->AddFile(mini_infilename);
  mini_eigen_lagr_allbpms_part_avg->AddFile(mini_allbpms_infilename);
  mini_reference_eigen_reg_5bpms->AddFile(mini_reference_evmon_infilename);
  mini_reference_eigen_reg_5bpms_sorted->AddFile(mini_reference_evmon_infilename);
  mini_reference_eigen_reg_allbpms->AddFile(mini_reference_evmon_infilename);
  mini_reference_eigen_reg_allbpms_sorted->AddFile(mini_reference_evmon_infilename);
  mini_regression->AddFile(regress_postpan_infilename);
  mini_overload->AddFile(overload_postpan_infilename);
  dit_part_avgd_friendable_slopes->AddFile(bmod_slopes_infilename);
  dit_plain_friendable_slopes->AddFile(bmod_slopes_infilename);
  dit_part_avgd_friendable_sens->AddFile(bmod_sens_infilename);
  dit_part_avgd_allbpms_friendable_sens->AddFile(bmod_allbpms_sens_infilename);
  //dit_plain_friendable_sens->AddFile(bmod_sens_infilename);
  // Default outputs, unsorted:
  //mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  //mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
  mini->AddFriend(mini_evMons_5bpms_part_avg,"mini_evMons_5bpms_part_avg");
  mini->AddFriend(mini_evMons_allbpms_part_avg,"mini_evMons_allbpms_part_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  mini->AddFriend(mini_eigen_lagr_5bpms_part_avg,"mini_eigen_lagr_5bpms_part_avg");
  mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->AddFriend(mini_regression,"mini_regression");
  mini->AddFriend(mini_overload,"mini_overload");
  mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  mini->AddFriend(dit_part_avgd_allbpms_friendable_sens,"dit_part_avgd_allbpms_friendable_sens"); // Give it a new name to avoid clashes
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  std::vector<TString> agg_cor_special_detectors = {
    "usl_reg_correction", "usr_reg_correction", "us_avg_reg_correction", "us_dd_reg_correction"// FIXME Need to update the parser to handle non-simple main_reg... // , "main_det_reg_correction", HAS NO main_det
  };
  std::vector<TString> AT_special_detectors = {
    "usl", "usr", "us_avg", "us_dd", "atl_dd","atr_dd", "atl_dd_atr_dd_avg", "main_det"  // used to comment out // "main_det", "us_dd"
  };
  std::vector<TString> special_detectors_plain = {
    "usl", "usr", "us_avg", "us_dd" // used to comment out // "main_det", "us_dd"
  };
  std::vector<TString> special_detectors_hardcoded = {
    "usl", "usr", "us_avg", "us_dd", "main_det" // used to comment out // "main_det", "us_dd"
  };
  std::vector<TString> special_detectors = {
    "usl", "usr", "us_avg", "us_dd", "manual_main_det" // used to comment out // "main_det", "us_dd"
  };
  //std::vector<TString> special_detectors = {
  //  /*"usl", "usr", */"us_avg"//, "us_dd" // used to comment out // "main_det", "us_dd"
  //};
  std::vector<TString> dit_special_detectors = special_detectors;
  //  USED TO BE { "usl", "usr"//, "us_avg", "us_dd" };
  std::vector<TString> coils = {
    "coil1","coil2","coil3","coil4","coil5","coil6","coil7"
  };
  std::vector<TString> special_differences_detectors = { // must come from first pass JAPAN plain aggregator
    "usl_dit_vs_cor_diff", 
    "usr_dit_vs_cor_diff", 
    "us_avg_dit_vs_cor_diff", 
    "us_dd_dit_vs_cor_diff", 
    // "main_det_dit_vs_cor_diff", FIXME not implemented by hand or in automation
  };
  std::vector<TString> special_eigen_dit_differences_detectors = {
    "usl_eigen_dit_eigen_reg_diff", 
    "usr_eigen_dit_eigen_reg_diff", 
    "us_avg_eigen_dit_eigen_reg_diff", 
    "us_dd_eigen_dit_eigen_reg_diff", 
    //"main_det_eigen_reg_reg_diff",  // FIXME not implemented by hand or in automation
  };
  std::vector<TString> special_eigen_lagr_differences_detectors = {
    "usl_eigen_lagr_reg_diff", 
    "usr_eigen_lagr_reg_diff", 
    "us_avg_eigen_lagr_reg_diff", 
    "us_dd_eigen_lagr_reg_diff", 
    "main_det_eigen_lagr_reg_diff",  // FIXME needs update
  };
  std::vector<TString> special_eigen_differences_detectors = {
    //"usl_eigen_reg_reg_diff", // FIXME FIXME respin1 part_avgd Aggregator used segment averaged slopes for eigen-regression!! This was a mistake
    "usl_dit_reg_diff",  // it is actually eigen dit for the eigen-agg rootfile (this will change in respin2)
    //"usl_dit_eigen_reg_diff", 
    //"usr_eigen_reg_reg_diff", 
    "usr_dit_reg_diff",  
    //"usr_dit_eigen_reg_diff", 
    //"us_avg_eigen_reg_reg_diff", 
    "us_avg_dit_reg_diff",  
    //"us_avg_dit_eigen_reg_diff", 
    //"us_dd_eigen_reg_reg_diff", 
    "us_dd_dit_reg_diff",  
    //"us_dd_dit_eigen_reg_diff", 
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
    "usl", "usr", "us_avg", "us_dd" // used to comment out // "main_det", "us_dd"
  };
  std::vector<TString> mon_cuts = {
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut, // Include 1 extra as the global 
  };
  std::vector<TString> cutGoodProduction = { // Must be == or > in length than the draws_piece1 first vector sent into the drawing scripts
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut, // Include 1 extra as the global 
  };
  std::vector<TString> null_cuts = {};
  std::vector<TString> only_mod_cuts = { // Lots of empty user passed cuts, for safety...
    "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut, "(1==1)"+mod_cut
  };

  std::vector<TString> special_eigen_differences_cuts = {
    //"rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1", // FIXME the eigen reg stuff in part_avgd aggregator was segment averages slopes! which is not wanted really
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1)"+mod_cut,
    //"(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1)"+mod_cut,
    //"(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2)"+mod_cut,
    //"(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2)"+mod_cut,
    //"(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    //"(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut, // Include 1 extra as the global 
  };
  std::vector<TString> special_eigen_differences_arm_cuts = {
    "(rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(1==1)"+mod_cut, // Include 1 extra as the global 
  };
  std::vector<TString> arm_cuts_plain = {
    "(rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(1==1)"+mod_cut, // Include 1 extra as the global 
  };
  std::vector<TString> arm_cuts = {
    "(rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_arm_flag == 0)"+mod_cut,
    "(1==1)"+mod_cut, // main_det cut here
    "(1==1)"+mod_cut, // Include 1 extra as the global 
  };
  //FIXME it would be nice to do cuts = Form("some cut from the user in plotMacros.C && this hard coded cut here")
  std::vector<TString> cuts_plain = {
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut, // Include 1 extra as the global 
  };
  std::vector<TString> cuts = {
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut, // main_det cut
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut, // Include 1 extra as the global 
  };
  std::vector<TString> AT_cuts = {
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut, // Include 1 extra as the global 
    "(rcdb_run_type == 1 && rcdb_run_flag == 1)"+mod_cut, // Include 1 extra as the global 
  };
  std::vector<TString> residuals_cuts = {
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 )"+mod_cut, // main_det case
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 )"+mod_cut, // Include 1 extra as the global 
  };
  std::vector<TString> residuals_cuts_runwise = {
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && minirun_n == 0 && rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && minirun_n == 0 && rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && minirun_n == 0 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && minirun_n == 0 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && minirun_n == 0 )"+mod_cut, // main_det case
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && flag==1 && dit_true_data==1 && minirun_n == 0 )"+mod_cut, // Include 1 extra as the global 
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
    "bpm11Y",
    "bpm12Y",
  };
  std::vector<TString> E_BPMs = {
    "bpm11X",
    "bpm12X",
  };
  std::vector<TString> empty_vec = {""};
  std::vector<TString> diff_vec = {"diff"};
  std::vector<TString> yield_vec = {"yield"};
  std::vector<TString> diff_yield_vec = {"diff","yield"};
  std::vector<TString> asym_vec = {"asym"};
  std::vector<TString> reg_cor_asym_vec = {"reg_asym","cor_asym"};
  std::vector<TString> lagr_cor_asym_vec = {"lagr_asym","cor_asym"};
  std::vector<TString> lagr_asym_vec = {"lagr_asym"};
  std::vector<TString> reg_asym_vec = {"reg_asym"};
  std::vector<TString> cor_asym_vec = {"cor_asym"};
  std::vector<TString> dit_asym_vec = {"dit_asym"};
  std::vector<TString> asym_yield_vec = {"asym","yield"};
  //std::vector<TString> monitors5 = {"evMon0"};
  std::vector<TString> monitors5_new = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new"};
  std::vector<TString> monitors12_new = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new","evMon5_new","evMon6_new","evMon7_new","evMon8_new","evMon9_new","evMon10_new","evMon11_new"};
  std::vector<TString> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  std::vector<TString> monitors12 = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};

  std::vector<TString> diff_kinematics = {
    "ErrorFlag_diff_targ_positionX_mean/1e-6",
    "ErrorFlag_diff_targ_positionY_mean/1e-6",
    "ErrorFlag_diff_targ_thetaX_mean/1e-6",
    "ErrorFlag_diff_targ_thetaY_mean/1e-6",
    "ErrorFlag_diff_dispE_bpm12X_mean/1e-6"
  };

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
  std::vector<TString> devices12 = {
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
  std::vector<TString> plot_yield_diff_BPMs = {
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
    "cav4dY" 
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
  //TString outFileName = Form("processed_respin2_data/TEST_new_weighting.root");
  //TString outFileName = Form("processed_respin2_data/CREX_All_Slug_Avg_Outputs.root");
  //TString outFileName = Form("processed_respin2_data/CREX_All_Slow_Controls_Avg_Outputs.root");
  //TString outFileName = Form("processed_respin2_data/CREX_All_Slow_Controls_Simple_Avg_Outputs.root");
  TString outFileName = Form("processed_respin2_data/CREX_All_%s_%s_Avg_Outputs%s.root",DataSetCut.Data(),averaging_timescale.Data(),suffix.Data()); /// FIXME no residuals generally...
  //TString outFileName = Form("processed_respin2_data/CREX_All_IHWP_Avg_Outputs.root");
  //TString outFileName = Form("processed_respin2_data/CREX_All_Wien_Avg_Outputs.root");
  //TString outFileName = Form("TEST_slug_avg_corrections_outputs.root");
  if (draw_plots == 1 || draw_plots == 3) {
    outFileName = "TEST_junk.root";
  }
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile newfile(outFileName, "recreate");
  newfile.cd();

  //TTree * out_tree_mini_eigen_reg_allbpms_tr     = new TTree("mini_eigen_reg_allbpms_tr_seg_avg",    "mini_eigen_reg_allbpms_tr_seg_avg");
  // Error: The tr branch never had it's combo branches changed by any sorting algorithm, so it's combos still live in the unsorted main branch
  //combo_segment_getter("dit_segment",mini,out_tree_mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr",cut,detectors,monitors12,devices12);

  // Start getting the averages
  // FIXME Using no cut (rcdb_run_type or run_flag or dit_flag) assumes that the dataset being fed in is a GOOD PRODUCTION and suitable for application to BMOD segments as well. For proof of concept comparisons the cut may not be that important, as we just want to compare eigen reg to eigen dit, etc. but for final numbers this will really matter.
  //TString cut = "dit_flag==1";

  // Corrections
  TTree * out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted = new TTree("mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted","mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted");
  TTree * out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted = new TTree("mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted","mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted");
  TTree * out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted = new TTree("mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted","mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted");
  TTree * out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_mon_weighted = new TTree("mini_reference_eigen_reg_allbpms_sorted_corrections_mon_weighted","mini_reference_eigen_reg_allbpms_sorted_corrections_mon_weighted");
  TTree * out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted = new TTree("mini_eigen_reg_5bpms_part_avg_corrections_det_weighted","mini_eigen_reg_5bpms_part_avg_corrections_det_weighted");
  TTree * out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted = new TTree("mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted","mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted");
  TTree * out_tree_mini_eigen_reg_allbpms_part_avg_corrections_det_weighted = new TTree("mini_eigen_reg_allbpms_part_avg_corrections_det_weighted","mini_eigen_reg_allbpms_part_avg_corrections_det_weighted");
  TTree * out_tree_mini_eigen_reg_allbpms_part_avg_corrections_mon_weighted = new TTree("mini_eigen_reg_allbpms_part_avg_corrections_mon_weighted","mini_eigen_reg_allbpms_part_avg_corrections_mon_weighted");
  TTree * out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted = new TTree("mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted","mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted");
  TTree * out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_mon_weighted = new TTree("mini_eigen_lagr_5bpms_part_avg_corrections_mon_weighted","mini_eigen_lagr_5bpms_part_avg_corrections_mon_weighted");
  TTree * out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted = new TTree("mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted","mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted");
  TTree * out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_mon_weighted = new TTree("mini_eigen_lagr_allbpms_part_avg_corrections_mon_weighted","mini_eigen_lagr_allbpms_part_avg_corrections_mon_weighted");
  TTree * out_tree_mini_regression_corrections_det_weighted = new TTree("mini_regression_corrections_det_weighted","mini_regression_corrections_det_weighted");
  TTree * out_tree_mini_regression_corrections_mon_weighted = new TTree("mini_regression_corrections_mon_weighted","mini_regression_corrections_mon_weighted");
  TTree * out_tree_mini_overload_corrections_det_weighted = new TTree("mini_overload_corrections_det_weighted","mini_overload_corrections_det_weighted");
  TTree * out_tree_mini_overload_corrections_mon_weighted = new TTree("mini_overload_corrections_mon_weighted","mini_overload_corrections_mon_weighted");
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
  TTree * out_tree_mini_eigen_allbpms_lagr_5bpms_dit_difference   = new TTree("mini_eigen_allbpms_lagr_5bpms_dit_difference","mini_eigen_allbpms_lagr_5bpms_dit_difference");
  TTree * out_tree_mini_eigen_allbpms_reg_5bpms_reg_difference    = new TTree("mini_eigen_allbpms_reg_5bpms_reg_difference","mini_eigen_allbpms_reg_5bpms_reg_difference");
  TTree * out_tree_mini_eigen_allbpms_lagr_allbpms_reg_difference = new TTree("mini_eigen_allbpms_lagr_allbpms_reg_difference","mini_eigen_allbpms_lagr_allbpms_reg_difference");
  TTree * out_tree_mini_eigen_allbpms_lagr_5bpms_reg_difference   = new TTree("mini_eigen_allbpms_lagr_5bpms_reg_difference","mini_eigen_allbpms_lagr_5bpms_reg_difference");
  TTree * out_tree_mini_eigen_allbpms_reg_5bpms_dit_difference    = new TTree("mini_eigen_allbpms_reg_5bpms_dit_difference","mini_eigen_allbpms_reg_5bpms_dit_difference");
  TTree * out_tree_mini_eigen_5bpms_dit_5bpms_reg_difference      = new TTree("mini_eigen_5bpms_dit_5bpms_reg_difference","mini_eigen_5bpms_dit_5bpms_reg_difference");
  TTree * out_tree_mini_plain_5bpms_dit_5bpms_reg_difference      = new TTree("mini_plain_5bpms_dit_5bpms_reg_difference","mini_plain_5bpms_dit_5bpms_reg_difference");
  

  // Asyms
  TTree * out_tree_mini_raw_det_asyms_det_weighted = new TTree("mini_raw_det_asyms_det_weighted","mini_raw_det_asyms_det_weighted");
  TTree * out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted = new TTree("mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted","mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted");
  TTree * out_tree_mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted = new TTree("mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted","mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted");
  TTree * out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted = new TTree("mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted","mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted");
  TTree * out_tree_mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted = new TTree("mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted","mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted");
  TTree * out_tree_mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted = new TTree("mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted","mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted");
  TTree * out_tree_mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted = new TTree("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted");
  //////////TTree * out_tree_burstwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted = new TTree("burstwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted","mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted");
  TTree * out_tree_mini_regression_det_asyms_det_weighted = new TTree("mini_regression_det_asyms_det_weighted","mini_regression_det_asyms_det_weighted");
  TTree * out_tree_mini_regression_det_asym_cors_det_weighted = new TTree("mini_regression_det_asym_cors_det_weighted","mini_regression_det_asym_cors_det_weighted");
  TTree * out_tree_mini_overload_det_asyms_det_weighted = new TTree("mini_overload_det_asyms_det_weighted","mini_overload_det_asyms_det_weighted");
  //TTree * out_tree_mini_overload_det_asym_cors_det_weighted = new TTree("mini_overload_det_asym_cors_det_weighted","mini_overload_det_asym_cors_det_weighted");
  TTree * out_tree_mini_dit_plain_det_asyms_det_weighted = new TTree("mini_dit_plain_det_asyms_det_weighted","mini_dit_plain_det_asyms_det_weighted");
  TTree * out_tree_mini_dit_part_avgd_det_asyms_det_weighted = new TTree("mini_dit_part_avgd_det_asyms_det_weighted","mini_dit_part_avgd_det_asyms_det_weighted");
  /* Special AT case for Ryan's needs
  TTree * out_tree_mini_dit_part_avgd_det_asyms_det_weighted = new TTree("mini_dit_part_avgd_det_asyms_det_weighted","mini_dit_part_avgd_det_asyms_det_weighted");
  TTree * out_tree_mini_reg_part_avgd_det_asyms_det_weighted = new TTree("mini_reg_part_avgd_det_asyms_det_weighted","mini_reg_part_avgd_det_asyms_det_weighted");
  End special AT case for Ryan's needs*/

  // Monitors
  TTree * out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted = new TTree("mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted","mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted");
  TTree * out_tree_mini_reference_eigen_reg_allbpms_sorted_monitors_det_weighted = new TTree("mini_reference_eigen_reg_allbpms_sorted_monitors_det_weighted","mini_reference_eigen_reg_allbpms_sorted_monitors_det_weighted");
  TTree * out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted = new TTree("mini_eigen_reg_5bpms_part_avg_monitors_det_weighted","mini_eigen_reg_5bpms_part_avg_monitors_det_weighted");
  TTree * out_tree_mini_eigen_reg_allbpms_part_avg_monitors_det_weighted = new TTree("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted","mini_eigen_reg_allbpms_part_avg_monitors_det_weighted");
  TTree * out_tree_mini_BPMs_det_weighted = new TTree("mini_BPMs_det_weighted","mini_BPMs_det_weighted");
  TTree * out_tree_mini_BCMs_det_weighted = new TTree("mini_BCMs_det_weighted","mini_BCMs_det_weighted");

  // Residuals (separately, using all new trees in a new chunk, for sanity and completeness)


  TString draw = "0";
  TString draw_weighting_error = "0";
  TString drawn_channels_error = "0";

  if (draw_plots == 0){ // Just calculate the slug, etc. averaged quantities
  
  // Use main det asym error rescale-weighting - set to be = drawn_channels_error if no fancy weighting is desired

  // Main reference burst-wise eigenvector regression
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_5bpms.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_5bpms.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_5bpms.reg_asym_usl.err))";
  draw            = Form("-1.0*%smini_reference_eigen_reg_5bpms_sorted.#_#.mean*mini_reference_eigen_reg_5bpms_sorted.diff_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted.#_#.mean*mini_reference_eigen_reg_5bpms_sorted.diff_#.err)";
  /* 8/5/2021 open end of comment for testing FIXME  */
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
  draw                = Form("-1.0*%smini_reference_eigen_reg_5bpms_sorted.#_#.mean*mini_reference_eigen_reg_5bpms_sorted.diff_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted.#_#.mean*mini_reference_eigen_reg_5bpms_sorted.diff_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5,monitors5,cuts,{},0);
  out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Main reference burst-wise eigenvector regression allbpms
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_allbpms.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_allbpms.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_allbpms.reg_asym_usl.err))";
  draw            = Form("-1.0*%smini_reference_eigen_reg_allbpms_sorted.#_#.mean*mini_reference_eigen_reg_allbpms_sorted.diff_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_reference_eigen_reg_allbpms_sorted.#_#.mean*mini_reference_eigen_reg_allbpms_sorted.diff_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors12,monitors12,cuts,{},0);
  out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
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
  draw_weighting_error = "abs((0*mini_reference_eigen_reg_allbpms_sorted.#_#.mean)+mini_reference_eigen_reg_allbpms_sorted.diff_#.err)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw                = Form("-1.0*%smini_reference_eigen_reg_allbpms_sorted.#_#.mean*mini_reference_eigen_reg_allbpms_sorted.diff_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_reference_eigen_reg_allbpms_sorted.#_#.mean*mini_reference_eigen_reg_allbpms_sorted.diff_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors12,monitors12,cuts,{},0);
  out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_mon_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_mon_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))";
  draw            = Form("-1.0*%smini_eigen_reg_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5_new,monitors5,cuts,{},0);
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
  draw            = Form("-1.0*%smini_eigen_reg_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5_new,monitors5,cuts,{},0);
  out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted->Write();
  delete out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Main part-avgd eigenvector regression allbpms
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_allbpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_allbpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_allbpms_part_avg.reg_asym_usl.err))";
  draw            = Form("-1.0*%smini_eigen_reg_allbpms_part_avg.#_#*agg_part_avgd_allbpms_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_reg_allbpms_part_avg.#_#*agg_part_avgd_allbpms_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_part_avg_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors12_new,monitors12,cuts,{},0);
  out_tree_mini_eigen_reg_allbpms_part_avg_corrections_det_weighted->Write();
  delete out_tree_mini_eigen_reg_allbpms_part_avg_corrections_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*mini_eigen_reg_allbpms_part_avg.#_#)+agg_part_avgd_allbpms_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = Form("-1.0*%smini_eigen_reg_allbpms_part_avg.#_#*agg_part_avgd_allbpms_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_reg_allbpms_part_avg.#_#*agg_part_avgd_allbpms_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_part_avg_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors12_new,monitors12,cuts,{},0);
  out_tree_mini_eigen_reg_allbpms_part_avg_corrections_mon_weighted->Write();
  delete out_tree_mini_eigen_reg_allbpms_part_avg_corrections_mon_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_eigen_lagr_5bpms_part_avg,"mini_eigen_lagr_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Main part-avgd eigenvector lagrangian regression
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_5bpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_5bpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_5bpms_part_avg.lagr_asym_usl.err))";
  draw            = Form("-1.0*%smini_eigen_lagr_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_lagr_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5_new,monitors5,cuts,{},0);
  out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted->Write();
  delete out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_eigen_lagr_5bpms_part_avg,"mini_eigen_lagr_5bpms_part_avg");
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
  draw_weighting_error = "abs((0*mini_eigen_lagr_5bpms_part_avg.#_#)+agg_part_avgd_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = Form("-1.0*%smini_eigen_lagr_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_lagr_5bpms_part_avg.#_#*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5_new,monitors5,cuts,{},0);
  out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_mon_weighted->Write();
  delete out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_mon_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Main part-avgd eigenvector allbpms lagrangian regression
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
  draw            = Form("-1.0*%smini_eigen_lagr_allbpms_part_avg.#_#*agg_part_avgd_allbpms_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_lagr_allbpms_part_avg.#_#*agg_part_avgd_allbpms_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors12_new,monitors12,cuts,{},0);
  out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted->Write();
  delete out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Monitor signal weighted allbpms (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*mini_eigen_lagr_allbpms_part_avg.#_#)+agg_part_avgd_allbpms_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = Form("-1.0*%smini_eigen_lagr_allbpms_part_avg.#_#*agg_part_avgd_allbpms_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_lagr_allbpms_part_avg.#_#*agg_part_avgd_allbpms_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors12_new,monitors12,cuts,{},0);
  out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_mon_weighted->Write();
  delete out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_mon_weighted;


  // The standard Postpan outputs have the slopes stored in a matrix "coeff" which is difficult to parse without being really carefull...
  // These slopes now live inside Aggregator output

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Standard regression corrections
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_regression.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_regression.reg_asym_usl.err))";
  draw            = Form("-1.0*%sagg_part_avgd_friendable.reg_asym_#_diff_#_slope*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_friendable.reg_asym_#_diff_#_slope*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_regression_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,devices5,devices5,cuts,{},0);
  out_tree_mini_regression_corrections_det_weighted->Write();
  delete out_tree_mini_regression_corrections_det_weighted;

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

  // Regular regression corrections
  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*agg_part_avgd_friendable.reg_asym_#_diff_#_slope)+agg_part_avgd_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = Form("-1.0*%sagg_part_avgd_friendable.reg_asym_#_diff_#_slope*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_friendable.reg_asym_#_diff_#_slope*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_regression_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,devices5,devices5,cuts,{},0);
  out_tree_mini_regression_corrections_mon_weighted->Write();
  delete out_tree_mini_regression_corrections_mon_weighted;


  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Overload regression corrections
  // Corrections per monitor
  // Main det weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_overload.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_overload.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_overload.reg_asym_usl.err))";
  draw            = Form("-1.0*%sagg_part_avgd_friendable.overload_reg_asym_#_diff_#_slope*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_friendable.overload_reg_asym_#_diff_#_slope*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_overload_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,devices12,devices12,cuts,{},0);
  out_tree_mini_overload_corrections_det_weighted->Write();
  delete out_tree_mini_overload_corrections_det_weighted;

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

  // Overload regression corrections
  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*agg_part_avgd_friendable.overload_reg_asym_#_diff_#_slope)+agg_part_avgd_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = Form("-1.0*%sagg_part_avgd_friendable.overload_reg_asym_#_diff_#_slope*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_friendable.overload_reg_asym_#_diff_#_slope*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_overload_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,devices12,devices12,cuts,{},0);
  out_tree_mini_overload_corrections_mon_weighted->Write();
  delete out_tree_mini_overload_corrections_mon_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(agg_plain_friendable.dit_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_plain_friendable.dit_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_plain_friendable.dit_asym_usl_mean_error))";
  draw            = Form("-1.0*%sdit_plain_friendable_slopes.#_#_mean*agg_plain_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(dit_plain_friendable_slopes.#_#_mean*agg_plain_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_plain_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,dit_slope_devices5,devices5,cuts,{},0);
  out_tree_mini_dit_plain_corrections_det_weighted->Write();
  delete out_tree_mini_dit_plain_corrections_det_weighted;

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

  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*dit_plain_friendable_slopes.#_#_mean)+agg_plain_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = Form("-1.0*%sdit_plain_friendable_slopes.#_#_mean*agg_plain_friendable.diff_#_mean",mod_draw.Data());
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
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(agg_part_avgd_friendable.eigen_dit_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_part_avgd_friendable.eigen_dit_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_part_avgd_friendable.eigen_dit_asym_usl_mean_error))";
  draw            = Form("-1.0*%sdit_part_avgd_friendable_slopes.#_#_mean*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(dit_part_avgd_friendable_slopes.#_#_mean*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_corrections_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,monitors5,monitors5,cuts,{},0);
  out_tree_mini_dit_part_avgd_corrections_det_weighted->Write();
  delete out_tree_mini_dit_part_avgd_corrections_det_weighted;

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

  // Monitor signal weighted (kind of like self, but no small-slope issues appear)
  draw_weighting_error = "abs((0*dit_part_avgd_friendable_slopes.#_#_mean)+agg_part_avgd_friendable.diff_#_mean_error)"; // The 0* section here is because I haven't figured out a convenient way to have multiple # # # set up that will let me pick which ones to use... i.e. I'm stuck with having exaclty these 3 #'s and their contents in this order...
  draw            = Form("-1.0*%sdit_part_avgd_friendable_slopes.#_#_mean*agg_part_avgd_friendable.diff_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(dit_part_avgd_friendable_slopes.#_#_mean*agg_part_avgd_friendable.diff_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_corrections_mon_weighted,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,monitors5,monitors5,cuts,{},0);
  out_tree_mini_dit_part_avgd_corrections_mon_weighted->Write();
  delete out_tree_mini_dit_part_avgd_corrections_mon_weighted;
  /* Open front of comment for testing */

  /* FIXME 8/5/2021 open end of comment for testing */


  /////////// Differences between different analyses, etc.

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Difference between allbpm lagr and 5bpm dit, all minirunwise differences (not multipletwise)
  // Part avgd allbpm lagr weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
  draw            = Form("%sagg_part_avgd_allbpms_friendable.eigen_lagr_asym_#_mean-%sagg_part_avgd_friendable.eigen_dit_asym_#_mean",mod_draw.Data(),mod_draw.Data());
  drawn_channels_error = "sqrt(abs(pow(agg_part_avgd_allbpms_friendable.eigen_lagr_asym_#_mean_error,2)-pow(agg_part_avgd_friendable.eigen_dit_asym_#_mean_error,2)))";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_allbpms_lagr_5bpms_dit_difference,draw,draw_weighting_error,drawn_channels_error,special_detectors_hardcoded,special_detectors_hardcoded,{},{},cuts,0);
  out_tree_mini_eigen_allbpms_lagr_5bpms_dit_difference->Write();
  delete out_tree_mini_eigen_allbpms_lagr_5bpms_dit_difference;

    // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
    delete mini;
    mini = new TChain("mini");
    mini->AddFile(mini_infilename);
    //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
    //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
    //mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
    //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
    //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
    mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
    mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
    //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
    //mini->AddFriend(mini_regression,"mini_regression");
    //mini->AddFriend(mini_overload,"mini_overload");
    //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
    //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
    //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
    //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
    mini->BuildIndex("run","mini");

    // Difference between eigen allbpm reg vs 5bpm reg
    // Part avgd eigen lagr weighted
    draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
    draw            = Form("%sagg_part_avgd_allbpms_friendable.eigen_reg_asym_#_mean-%sagg_part_avgd_friendable.eigen_reg_asym_#_mean",mod_draw.Data(),mod_draw.Data());
    drawn_channels_error = "sqrt(abs(pow(agg_part_avgd_allbpms_friendable.eigen_reg_asym_#_mean_error,2)-pow(agg_part_avgd_friendable.eigen_reg_asym_#_mean_error,2)))";
    combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_allbpms_reg_5bpms_reg_difference,draw,draw_weighting_error,drawn_channels_error,special_detectors_hardcoded,special_detectors_hardcoded,{},{},cuts,0);
    out_tree_mini_eigen_allbpms_reg_5bpms_reg_difference->Write();
    delete out_tree_mini_eigen_allbpms_reg_5bpms_reg_difference;

    // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
    delete mini;
    mini = new TChain("mini");
    mini->AddFile(mini_infilename);
    //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
    //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
    //mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
    //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
    //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
    mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
    mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
    //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
    //mini->AddFriend(mini_regression,"mini_regression");
    //mini->AddFriend(mini_overload,"mini_overload");
    //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
    //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
    //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
    //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
    mini->BuildIndex("run","mini");

    // Difference between eigen regression and eigen dithering - 5bpms
    // Part avgd eigen lagr weighted
    draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
    draw            = Form("%sagg_part_avgd_friendable.#_#_mean",mod_draw.Data());
    drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
    combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_5bpms_dit_5bpms_reg_difference,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_eigen_dit_differences_detectors,{},{},cuts_plain,0);
    out_tree_mini_eigen_5bpms_dit_5bpms_reg_difference->Write();
    delete out_tree_mini_eigen_5bpms_dit_5bpms_reg_difference;
  /* FIXME Add back in start of comment to open back up */

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  //mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Difference between eigen lagrange and eigen regression - allbpms
  // Part avgd eigen lagr weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
  draw            = Form("%sagg_part_avgd_allbpms_friendable.#_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_allbpms_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_allbpms_lagr_allbpms_reg_difference,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_eigen_lagr_differences_detectors,{},{},cuts,0);
  out_tree_mini_eigen_allbpms_lagr_allbpms_reg_difference->Write();
  delete out_tree_mini_eigen_allbpms_lagr_allbpms_reg_difference;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Difference between allbpm lagr and 5bpm reg, all minirunwise differences (not multipletwise)
  // Part avgd allbpm lagr weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
  draw            = Form("%sagg_part_avgd_allbpms_friendable.eigen_lagr_asym_#_mean-%sagg_part_avgd_friendable.eigen_reg_asym_#_mean",mod_draw.Data(),mod_draw.Data());
  drawn_channels_error = "sqrt(abs(pow(agg_part_avgd_allbpms_friendable.eigen_lagr_asym_#_mean_error,2)-pow(agg_part_avgd_friendable.eigen_reg_asym_#_mean_error,2)))";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_allbpms_lagr_5bpms_reg_difference,draw,draw_weighting_error,drawn_channels_error,special_detectors_hardcoded,special_detectors_hardcoded,{},{},cuts,0);
  out_tree_mini_eigen_allbpms_lagr_5bpms_reg_difference->Write();
  delete out_tree_mini_eigen_allbpms_lagr_5bpms_reg_difference;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Difference between allbpm reg and 5bpm dit, all minirunwise differences (not multipletwise)
  // Part avgd eigen lagr weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
  draw            = Form("%sagg_part_avgd_allbpms_friendable.eigen_reg_asym_#_mean-%sagg_part_avgd_friendable.eigen_dit_asym_#_mean",mod_draw.Data(),mod_draw.Data());
  drawn_channels_error = "sqrt(abs(pow(agg_part_avgd_allbpms_friendable.eigen_reg_asym_#_mean_error,2)-pow(agg_part_avgd_friendable.eigen_dit_asym_#_mean_error,2)))";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_allbpms_reg_5bpms_dit_difference,draw,draw_weighting_error,drawn_channels_error,special_detectors_hardcoded,special_detectors_hardcoded,{},{},cuts,0);
  out_tree_mini_eigen_allbpms_reg_5bpms_dit_difference->Write();
  delete out_tree_mini_eigen_allbpms_reg_5bpms_dit_difference;


  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Difference between plain dit and standard regression - 5bpms - and it is OLD cut data...
  // Part avgd eigen lagr weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
  draw            = Form("%sagg_plain_friendable.#_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_plain_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_plain_5bpms_dit_5bpms_reg_difference,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_differences_detectors,{},{},cuts_plain,0);
  out_tree_mini_plain_5bpms_dit_5bpms_reg_difference->Write();
  delete out_tree_mini_plain_5bpms_dit_5bpms_reg_difference;

  /* FIXME Add back in start of comment to open back up  */
  /* FIXME Add back in end of comment to open back up  */


  /////////// Asyms, etc.

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
  draw                 = Form("%s#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(#_#.err)";
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
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_5bpms.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_5bpms.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_5bpms.reg_asym_usl.err))";
  draw                 = Form("%smini_reference_eigen_reg_5bpms.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_cor_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Reference burstwise - allbpms, (sorted, but it the corrected asyms etc. don't care about sort and live in non-sorted tree by themselves), eigenvector regression 
  // Main det signal weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_allbpms.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_allbpms.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_allbpms.reg_asym_usl.err))";
  draw                 =     Form("%smini_reference_eigen_reg_allbpms.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_reference_eigen_reg_allbpms.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_cor_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))";
  draw                 =     Form("%smini_eigen_reg_5bpms_part_avg.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_cor_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted->Write();
  delete out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Part avgd eigenvector regression (allbpms)
  // Main det signal weighted
  // This is the signal with which all other signals are averaged with (except self-weighted signals)
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_allbpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_allbpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_allbpms_part_avg.reg_asym_usl.err))";
  draw                 =     Form("%smini_eigen_reg_allbpms_part_avg.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_reg_allbpms_part_avg.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_cor_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted->Write();
  delete out_tree_mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_eigen_lagr_5bpms_part_avg,"mini_eigen_lagr_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Part avgd eigenvector lagrange regression (5bpms)
  // Main det signal weighted
  // This is the signal with which all other signals are averaged with (except self-weighted signals)
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_5bpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_5bpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_5bpms_part_avg.lagr_asym_usl.err))";
  draw                 =     Form("%smini_eigen_lagr_5bpms_part_avg.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_lagr_5bpms_part_avg.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,lagr_cor_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted->Write();
  delete out_tree_mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  mini->AddFriend(mini_eigen_lagr_allbpms_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Part avgd eigenvector lagrange regression (allbpms)
  // Main det signal weighted
  // This is the signal with which all other signals are averaged with (except self-weighted signals)
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg.lagr_asym_usl.err))";
  draw                 =     Form("%smini_eigen_lagr_allbpms_part_avg.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_eigen_lagr_allbpms_part_avg.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,lagr_cor_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted->Write();
  delete out_tree_mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  //////////delete mini;
  //////////mini = new TChain("mini");
  //////////mini->AddFile(mini_infilename);
  //////////mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  ////////////mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  ////////////mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  ////////////mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  //////////mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  ////////////mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  ////////////mini->AddFriend(mini_regression,"mini_regression");
  ////////////mini->AddFriend(mini_overload,"mini_overload");
  ////////////mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  ////////////mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  ////////////mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  ////////////mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  //////////mini->BuildIndex("run","mini");

  //////////// Part avgd eigenvector regression (5bpms) - burstwise output this time
  //////////// Main det signal weighted
  //////////// This is the signal with which all other signals are averaged with (except self-weighted signals)
  //////////drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg.#_#.err)";
  //////////draw                 =     Form("%smini_eigen_reg_5bpms_part_avg.#_#.mean",mod_draw.Data());
  //////////draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  ////////////draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))";
  //////////combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_burstwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_cor_asym_vec,special_detectors,{},null_cuts,cuts,1); //null cut vector here means use draws_piece2 loop for cut definition instead
  //////////stwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted->Write();
  //////////delete out_tree_burstwise_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_regression.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_regression.reg_asym_usl.err))";
  draw                 =     Form("%smini_regression.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_regression.#_#.err)";
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
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Main standard 5bpm regression - correction amount on the asym, from aggregator
  // Main det signal weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_regression.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_regression.reg_asym_usl.err))";
  draw                 =     Form("%sagg_plain_friendable.#_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_plain_friendable.#_#_mean_error)";
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
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_overload.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_overload.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_overload.reg_asym_usl.err))";
  draw                 =     Form("%smini_overload.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_overload.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_overload_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_overload_det_asyms_det_weighted->Write();
  delete out_tree_mini_overload_det_asyms_det_weighted;

  // FIXME just don't have overload regression correction in the aggregator ... 
  // FIXME 07/20/2021 add in overload and regular postpan - slopes live in part_avgd aggregator
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  ////////////////////////delete mini;
  ////////////////////////mini = new TChain("mini");
  ////////////////////////mini->AddFile(mini_infilename);
  //////////////////////////mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //////////////////////////mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //////////////////////////mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //////////////////////////mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  ////////////////////////mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //////////////////////////mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //////////////////////////mini->AddFriend(mini_regression,"mini_regression");
  ////////////////////////mini->AddFriend(mini_overload,"mini_overload");
  //////////////////////////mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //////////////////////////mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //////////////////////////mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //////////////////////////mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  ////////////////////////mini->BuildIndex("run","mini");
  ////////////////////////
  ////////////////////////// Overload standard regression, asym correction amount from aggregator
  ////////////////////////// Main det signal weighted
  ////////////////////////drawn_channels_error = "abs(mini_overload.reg_asym_#.err)";
  ////////////////////////draw                 =     Form("%s(mini_overload.reg_asym_#.mean-mini_overload.asym_#.mean)",mod_draw.Data());
  ////////////////////////draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //////////////////////////draw_weighting_error = "((rcdb_arm_flag==0)*(mini_overload.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_overload.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_overload.reg_asym_usl.err))";
  ////////////////////////combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_overload_det_asym_cors_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  ////////////////////////out_tree_mini_overload_det_asym_cors_det_weighted->Write();
  ////////////////////////delete out_tree_mini_overload_det_asym_cors_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(agg_plain_friendable.reg_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_plain_friendable.reg_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_plain_friendable.reg_asym_usl_mean_error))";
  draw                 =     Form("%sagg_plain_friendable.#_#_mean",mod_draw.Data());
  // Because of issues with the dit dataset error bars just use regression errors here...
  drawn_channels_error = "abs(agg_plain_friendable.#_#_mean_error)";
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
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(agg_part_avgd_friendable.eigen_dit_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_part_avgd_friendable.eigen_dit_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_part_avgd_friendable.eigen_dit_asym_usl_mean_error))";
  draw                 =     Form("%sagg_part_avgd_friendable.eigen_#_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_friendable.eigen_#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  ///////////////combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_dit_part_avgd_det_asyms_det_weighted->Write();
  delete out_tree_mini_dit_part_avgd_det_asyms_det_weighted;

  /* FIXME Open front of comment for testing */

  /* FIXME FIXME This section here is special AT detector set and AT cuts for Ryan's purposes

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
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
  drawn_channels_error = "abs(agg_part_avgd_friendable.eigen_#_#_mean_error)";
  draw                 =     Form("%sagg_part_avgd_friendable.eigen_#_#_mean",mod_draw.Data());
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(agg_part_avgd_friendable.eigen_dit_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_part_avgd_friendable.eigen_dit_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_part_avgd_friendable.eigen_dit_asym_usl_mean_error))";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_asym_vec,AT_special_detectors,{},null_cuts,AT_cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  ///////////////combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_dit_part_avgd_det_asyms_det_weighted->Write();
  delete out_tree_mini_dit_part_avgd_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  //mini->AddFriend(mini_regression,"mini_regression");
  //mini->AddFriend(mini_overload,"mini_overload");
  //mini->AddFriend(dit_part_avgd_friendable_slopes,"dit_part_avgd_friendable_slopes");
  //mini->AddFriend(dit_plain_friendable_slopes,"dit_plain_friendable_slopes");
  //mini->AddFriend(dit_part_avgd_friendable_sens,"dit_part_avgd_friendable_sens");
  //mini->AddFriend(dit_plain_friendable_sens,"dit_plain_friendable_sens");
  mini->BuildIndex("run","mini");

  // Part avgd eigen vector regression analysis
  // Asyms come from the "part avgd" Agg outputs
  // Correction amounts also exist in the agg tree... just not in a #_# easily parsed format of course
  // Main det signal weighted
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  draw                 =     Form("%sagg_part_avgd_friendable.#_#_mean",mod_draw.Data());
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(agg_part_avgd_friendable.reg_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_part_avgd_friendable.reg_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_part_avgd_friendable.reg_asym_usl_mean_error))";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reg_part_avgd_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,AT_special_detectors,{},null_cuts,AT_cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  ///////////////combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reg_part_avgd_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},null_cuts,cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_reg_part_avgd_det_asyms_det_weighted->Write();
  delete out_tree_mini_reg_part_avgd_det_asyms_det_weighted;


  END special Ryan AT dets and cuts section */


  /////////// BPMs, monitors, etc.
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  /* FIXME 8/5/2021 open back of comment for testing */
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - regular eigenvector analysis, sorted - reference analysis
  // Main det signal weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))";
  draw                 =     Form("%smini_reference_eigen_reg_5bpms_sorted.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted,draw,draw_weighting_error,drawn_channels_error,diff_vec,monitors5,{},cutGoodProduction,{},0);
  out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted;
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  //mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - regular eigenvector analysis, sorted - reference analysis
  // Main det signal weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_allbpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_allbpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_allbpms_part_avg.reg_asym_usl.err))";
  draw                 =     Form("%smini_reference_eigen_reg_allbpms_sorted.#_#.mean",mod_draw.Data());
  drawn_channels_error = "abs(mini_reference_eigen_reg_allbpms_sorted.#_#.err)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reference_eigen_reg_allbpms_sorted_monitors_det_weighted,draw,draw_weighting_error,drawn_channels_error,diff_vec,monitors12,{},cutGoodProduction,{},0);
  out_tree_mini_reference_eigen_reg_allbpms_sorted_monitors_det_weighted->Write();
  delete out_tree_mini_reference_eigen_reg_allbpms_sorted_monitors_det_weighted;
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - 5bpms - regular eigenvector analysis, sorted - part avg eigenvectors
  // Main det signal weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))";
  draw                 =     Form("%sagg_part_avgd_friendable.#_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted,draw,draw_weighting_error,drawn_channels_error,diff_vec,monitors5,{},cutGoodProduction,{},0);
  out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted->Write();
  delete out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted;
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_allbpms_part_avg,"mini_eigen_reg_allbpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms,"mini_reference_eigen_reg_allbpms");
  //mini->AddFriend(mini_reference_eigen_reg_allbpms_sorted,"mini_reference_eigen_reg_allbpms_sorted");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - allbpms - regular eigenvector analysis, sorted - part avg eigenvectors
  // Main det signal weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_allbpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_allbpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_allbpms_part_avg.reg_asym_usl.err))";
  draw                 =     Form("%sagg_part_avgd_allbpms_friendable.#_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_allbpms_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_part_avg_monitors_det_weighted,draw,draw_weighting_error,drawn_channels_error,diff_vec,monitors12,{},cutGoodProduction,{},0);
  out_tree_mini_eigen_reg_allbpms_part_avg_monitors_det_weighted->Write();
  delete out_tree_mini_eigen_reg_allbpms_part_avg_monitors_det_weighted;
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - regular eigenvector analysis, sorted
  // Main det signal weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))";
  draw                 =     Form("%sagg_part_avgd_friendable.#_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_BPMs_det_weighted,draw,draw_weighting_error,drawn_channels_error,diff_yield_vec,yield_diff_BPMs,{},cutGoodProduction,{},0);
  out_tree_mini_BPMs_det_weighted->Write();
  delete out_tree_mini_BPMs_det_weighted;
  
  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  //mini->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms,"mini_reference_eigen_reg_5bpms");
  //mini->AddFriend(mini_reference_eigen_reg_5bpms_sorted,"mini_reference_eigen_reg_5bpms_sorted");
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->AddFriend(agg_part_avgd_allbpms_friendable,"agg_part_avgd_allbpms_friendable");
  //mini->AddFriend(agg_plain_friendable,"agg_plain_friendable");
  mini->BuildIndex("run","mini");

  // Main burst-wise eigenvector regression
  // Monitor signals directly - regular eigenvector analysis, sorted
  // Main det signal weighted
  draw_weighting_error = "agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_mean_error"; // Just use the aggregator's main_det error, no tricks
  //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))";
  draw                 =     Form("%sagg_part_avgd_friendable.#_#_mean",mod_draw.Data());
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_BCMs_det_weighted,draw,draw_weighting_error,drawn_channels_error,asym_yield_vec,yield_bcm_devices,{},cutGoodProduction,{},0);
  out_tree_mini_BCMs_det_weighted->Write();
  delete out_tree_mini_BCMs_det_weighted;
   /* Open front of comment for testing */
  }
  if  (draw_plots==2) {
  
  // Slope diff disagreements and residual sensitivities
  //
  /*
  TString residuals_infilename = "/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/workspace_plotting_macros/july_21_plots/residuals_part.root";
  TFile residuals_infile(residuals_infilename,"READ");
  TFile slopes_infile("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_part_avg/respin2-rcdb-parts-slowc-segments-part_avg_sens-eigen_reg_lagr.root","READ");

  TTree* residuals_dit                     = new TTree(Form("dit"),"dit");
  TChain* residuals_dit_13747               = new TChain(Form("dit_13746"));
  TChain* residuals_part_avgd_regression    = new TChain(Form("mini_eigen_reg_5bpms_%s_avg",type.Data()));
  TChain* residuals_part_avgd_lagrange      = new TChain(Form("mini_eigen_lagr_5bpms_%s_avg",type.Data()));
  TChain* residuals_reference_regression    = new TChain(Form("mini_reference_eigen_reg_5bpms_sorted"));
  TChain* residuals_segment_avgd_dit_slopes = new TChain(Form("dit_%s_avgd_friendable",type.Data()));

  residuals_infile.GetObject(Form("dit")                                      , residuals_dit                    );
  //residuals_dit                    ->AddFile(residuals_infilename);
  
  //residuals_infile.GetObject(Form("dit_13746")                                , residuals_dit_13747              );
  //slopes_infile.GetObject(Form("mini_eigen_reg_5bpms_%s_avg",type.Data())     , residuals_part_avgd_regression   );
  //slopes_infile.GetObject(Form("mini_eigen_lagr_5bpms_%s_avg",type.Data())    , residuals_part_avgd_lagrange     );
  //residuals_infile.GetObject(Form("mini_reference_eigen_reg_5bpms_sorted")    , residuals_reference_regression   );
  //residuals_infile.GetObject(Form("dit_%s_avgd_friendable",type.Data())       , residuals_segment_avgd_dit_slopes);
  residuals_dit_13747              ->AddFile(residuals_infilename);
  residuals_part_avgd_regression   ->AddFile(residuals_infilename);
  residuals_part_avgd_lagrange     ->AddFile(residuals_infilename);
  residuals_reference_regression   ->AddFile(residuals_infilename);
  residuals_segment_avgd_dit_slopes->AddFile(residuals_infilename);

  TTree * out_tree_residuals_run_dit_dit_diff_det_weighted = new TTree("residuals_run_dit_dit_diff_det_weighted","residuals_run_dit_dit_diff_det_weighted");
  TTree * out_tree_residuals_reg_dit_diff_det_weighted = new TTree("residuals_reg_dit_diff_det_weighted","residuals_reg_dit_diff_det_weighted");
  TTree * out_tree_residuals_lagr_dit_diff_det_weighted = new TTree("residuals_lagr_dit_diff_det_weighted","residuals_lagr_dit_diff_det_weighted");
  TTree * out_tree_residuals_lagr_reg_diff_det_weighted = new TTree("residuals_lagr_reg_diff_det_weighted","residuals_lagr_reg_diff_det_weighted");
  TTree * out_tree_residuals_avg_dit_res = new TTree("residuals_avg_dit_res","residuals_avg_dit_res");
  TTree * out_tree_residuals_reg_res = new TTree("residuals_reg_res","residuals_reg_res");
  TTree * out_tree_residuals_lagr_res = new TTree("residuals_lagr_res","residuals_lagr_res");

  residuals_dit->BuildIndex("run","minirun_n");

  //////// May need this if the corrected mini_eigen_reg_5bpms_part_avg given in the residuals file doesn't have maindet asyms??
  //////// residuals_dit->AddFriend(mini_eigen_reg_5bpms_part_avg,"mini_eigen_reg_5bpms_part_avg");
  // Must be taken care of inside the infile rootfile!!
  residuals_dit->AddFriend(residuals_dit_13747              );//,Form("dit_13746"));      
  residuals_dit->AddFriend(residuals_part_avgd_regression   );//,Form("mini_eigen_reg_5bpms_%s_avg",type.Data()));
  residuals_dit->AddFriend(residuals_part_avgd_lagrange     );//,Form("mini_eigen_lagr_5bpms_%s_avg",type.Data()));
  residuals_dit->AddFriend(residuals_reference_regression   );//,Form("mini_reference_eigen_reg_5bpms_sorted"));
  residuals_dit->AddFriend(residuals_segment_avgd_dit_slopes);//,Form("dit_%s_avgd_friendable",type.Data())); 
  */


  TString run_cycle = "run_avg";
  TString run_cycle_wise = "runwise";
  //TString run_cycle = "cyclewise";//"run_avg";
  //TString run_cycle_wise = "cyclewise";//"runwise";
  TString scale = type;
  TString bpms = "_allbpms";
  TString nbpms = "5bpms";
  if (bpms == "_allbpms") { // default case is "" for 5bpm suffix
    nbpms = "allbpms";
  }
  TString part_ana = "_eigenvectors_double_sorted_part_avg"+bpms; // FIXME hardcoded the analysis here!!


  //TTree* residuals_dit                     = new TTree(Form("dit"),"dit");
  //TString residuals_infilename = "/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/workspace_plotting_macros/july_21_plots/residuals_part.root";
  //TFile residuals_infile(residuals_infilename,"READ");
  //residuals_infile.GetObject(Form("dit"), residuals_dit);
  //TChain* residuals_dit           = new TChain(Form("dit_%s_avgd_tagged_friendable",type.Data()),"dit_with_all_the_friends");
  TChain* residuals_mini          = new TChain("mini");
  TChain* residuals_dit           = new TChain(Form("dit_%s_avgd_tagged_friendable",type.Data()),"dit_with_all_the_friends");
  TChain* dit_13746               = new TChain(Form("dit_%s_avgd_tagged_friendable",type.Data()));
  TChain* part_avgd_regression    = new TChain(Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  TChain* part_avgd_lagrange      = new TChain(Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  TChain* reference_regression    = new TChain(Form("mini_eigen_reg_%s_sorted",nbpms.Data()));
  TChain* segment_avgd_dit_slopes = new TChain(Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));



  TTree * out_tree_residuals_run_dit_dit_diff_det_weighted = new TTree("residuals_run_dit_dit_diff_det_weighted","residuals_run_dit_dit_diff_det_weighted");
  TTree * out_tree_residuals_reg_dit_diff_det_weighted = new TTree("residuals_reg_dit_diff_det_weighted","residuals_reg_dit_diff_det_weighted");
  TTree * out_tree_residuals_lagr_dit_diff_det_weighted = new TTree("residuals_lagr_dit_diff_det_weighted","residuals_lagr_dit_diff_det_weighted");
  TTree * out_tree_residuals_lagr_reg_diff_det_weighted = new TTree("residuals_lagr_reg_diff_det_weighted","residuals_lagr_reg_diff_det_weighted");
  TTree * out_tree_residuals_avg_dit_res = new TTree("residuals_avg_dit_res","residuals_avg_dit_res");
  TTree * out_tree_residuals_reg_res = new TTree("residuals_reg_res","residuals_reg_res");
  TTree * out_tree_residuals_lagr_res = new TTree("residuals_lagr_res","residuals_lagr_res");

  TTree * out_tree_slopes_run_dit       = new TTree("slopes_run_dit","slopes_run_dit");
  TTree * out_tree_slopes_part_avg_dit  = new TTree("slopes_part_avg_dit","slopes_part_avg_dit");
  TTree * out_tree_slopes_part_avg_reg  = new TTree("slopes_part_avg_reg","slopes_part_avg_reg");
  TTree * out_tree_slopes_part_avg_lagr = new TTree("slopes_part_avg_lagr","slopes_part_avg_lagr");

  
  /*
  residuals_dit          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_%s%s_1X/respin2_All_dithering_slopes_%s_rcdb_segmented_pruned_usavg_tag_expanded.root",run_cycle.Data(),part_ana.Data(),run_cycle.Data()));
  //residuals_dit          ->Add(Form("/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/processed_respin2_data/residuals_part.root"));
  // Assume run averaged analysis for now
  dit_13746              ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_%s%s_1X/respin2_All_dithering_slopes_%s_rcdb_segmented_pruned_usavg_tag_expanded.root",run_cycle.Data(),part_ana.Data(),run_cycle.Data()));
  part_avgd_regression   ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",do_err.Data(),type.Data(),type.Data())); // Make more of this
  part_avgd_lagrange     ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",do_err.Data(),type.Data(),type.Data())); // Make more of this
  reference_regression   ->Add(Form("/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/respin2_data/rcdb_burstwise_eigenvectors_sorted_sorted.root"));
  // Eigenvector BMOD only eved run-averaged analysis
  segment_avgd_dit_slopes->Add(Form("/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/processed_respin2_data/All_dit_outputs_doing_segment_averaging_of_slopes_mini_friended.root"));
  */

  residuals_mini         ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  residuals_dit          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_%s%s_1X/respin2_All_dithering_slopes_%s_rcdb_segmented_pruned_usavg_tag_expanded.root",run_cycle.Data(),part_ana.Data(),run_cycle.Data()));
  dit_13746              ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_%s%s_1X/respin2_All_dithering_slopes_%s_rcdb_segmented_pruned_usavg_tag_expanded.root",run_cycle.Data(),part_ana.Data(),run_cycle.Data()));
  part_avgd_regression   ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  part_avgd_lagrange     ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  reference_regression   ->Add(Form("respin2_data/rcdb_burstwise_eigenvectors_sorted_sorted.root"));
  // Eigenvector BMOD only ever did run-averaged analysis
  segment_avgd_dit_slopes->AddFile(Form("processed_respin2_data/All_dit_outputs_doing_segment_averaging_of_slopes_mini_friended.root"));
  // For safety build an index that assigns a run and cyclenum to define the entries (so we can compare different trees even with missing entries)
  //dit_13746->BuildIndex("run","cyclenum");
  //dit_15746->BuildIndex("run","cyclenum");
  //dit_13726->BuildIndex("run","cyclenum");
  // Set their names to be unique
  // Make them friends
  residuals_mini->AddFriend(residuals_dit         ,     "dit");
  residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
  residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));

  TString us_avg_frac = "";
  TString us_dd_frac = "";
  TString usl_frac = "";
  TString usr_frac = "";

  std::vector <TString> tmpDetNames;
  tmpDetNames = special_detectors_plain;//{"us_avg","us_dd","usl","usr"};

  std::vector <TString> monitors_choice;
  std::vector <TString> monitors_choice_new;

  Int_t nmons = 5;
  if ( bpms.Contains("allbpms") ) {
    nmons = 12;
    monitors_choice_new = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new","evMon5_new","evMon6_new","evMon7_new","evMon8_new","evMon9_new","evMon10_new","evMon11_new"};
    monitors_choice = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  }
  else {
    monitors_choice_new = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new"};
    monitors_choice = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  }
  TString tmpName = "";
  TString tmpErrName = "";
  TString tmpFrac = "";

  TString res_type = "_fractional"; // hardcoded to be fractional residuals

  for (Int_t idet = 0 ; idet < tmpDetNames.size() ; idet++) {
    for (Int_t coil = 1 ; coil <=7 ; coil++ ) {
      if (res_type == "_fractional") {
        tmpFrac = Form("/%s_coil%d",tmpDetNames.at(idet).Data(),coil);
        //us_avg_frac = Form("/us_avg_coil%d",coil);
        //us_dd_frac = Form("/us_dd_coil%d",coil);
        //usl_frac = Form("/usl_coil%d",coil);
        //usr_frac = Form("/usr_coil%d",coil);
      }
      // Math of fractional residual error...
      // sigma = sigma of ((difference of raw - correction)/raw) = fractional * sqrt(pow(raw err/raw,2) + pow(err of(correction error)/(correction),2))
      //      where that correction error = sqrt(sum(pow(slope * sens errors),2))
      //      and correction itself = (1 - frac residual)/frac_variable     (frac variable = 1/raw)

        // Run avg dit data 
        tmpName = Form("(dit_13746.%s_coil%d-(0",tmpDetNames.at(idet).Data(),coil);
        tmpErrName = Form("abs(1/dit_13746.%s_coil%d)*sqrt((pow(dit_13746.%s_coil%d_err*(1-dit_13746.%s_coil%d_residual),2))",
            tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil);
        for (Int_t imon = 0 ; imon < nmons ; imon++) {
          tmpName = Form("%s+(dit_13746.%s_evMon%d*dit_13746.evMon%d_coil%d)",tmpName.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
          tmpErrName = Form("%s+pow(dit_13746.%s_evMon%d*dit_13746.evMon%d_coil%d_err,2)",tmpErrName.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
        }
        if (nmons == 12) {
          dit_13746->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("dit_13746.%s_coil%d/100",tmpDetNames.at(idet).Data(),coil));
          dit_13746->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("dit_13746.%s_coil%d_err/100",tmpDetNames.at(idet).Data(),coil));
        }
        else {
          dit_13746->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s))%s",tmpName.Data(),tmpFrac.Data()));
          dit_13746->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s)",tmpErrName.Data()));
        }

        // Part avg reg data
        tmpName = Form("(dit_13746.%s_coil%d-((0",tmpDetNames.at(idet).Data(),coil);
        tmpErrName = Form("abs(1/dit_13746.%s_coil%d)*sqrt((pow(dit_13746.%s_coil%d_err*(1-mini_eigen_reg_%s_%s_avg.%s_coil%d_residual),2))+(0",
            tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil,nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),coil);
        for (Int_t imon = 0 ; imon < nmons ; imon++) {
          tmpName = Form("%s+(mini_eigen_reg_%s_%s_avg.%s_evMon%d_new*dit_13746.evMon%d_coil%d)",tmpName.Data(),nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
          tmpErrName = Form("%s+pow(mini_eigen_reg_%s_%s_avg.%s_evMon%d_new*dit_13746.evMon%d_coil%d_err,2)",tmpErrName.Data(),nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
        }
        part_avgd_regression->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s))/1e-3)%s",tmpName.Data(),tmpFrac.Data()));
        part_avgd_regression->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s)/1e-6)",tmpErrName.Data()));
        // FIXME FIXME FIXME FIXME I added a NEW 1/e-6 to the residual error calculation because it didn't match relative UNITS!!!!

        // Part avg lagr data
        tmpName = Form("(dit_13746.%s_coil%d-((0",tmpDetNames.at(idet).Data(),coil);
        tmpErrName = Form("abs(1/dit_13746.%s_coil%d)*sqrt((pow(dit_13746.%s_coil%d_err*(1-mini_eigen_lagr_%s_%s_avg.%s_coil%d_residual),2))+(0",
            tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil,nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),coil);
        for (Int_t imon = 0 ; imon < nmons ; imon++) {
          tmpName = Form("%s+(mini_eigen_lagr_%s_%s_avg.%s_evMon%d_new*dit_13746.evMon%d_coil%d)",tmpName.Data(),nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
          tmpErrName = Form("%s+pow(mini_eigen_lagr_%s_%s_avg.%s_evMon%d_new*dit_13746.evMon%d_coil%d_err,2)",tmpErrName.Data(),nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
        }
        part_avgd_lagrange->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s))/1e-3)%s",tmpName.Data(),tmpFrac.Data()));
        part_avgd_lagrange->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s)/1e-6)",tmpErrName.Data()));

        // Part avg segmentwise dit data
        tmpName = Form("(dit_13746.%s_coil%d-((0",tmpDetNames.at(idet).Data(),coil);
        tmpErrName = Form("abs(1/dit_13746.%s_coil%d)*sqrt((pow(dit_13746.%s_coil%d_err*(1-dit_%s_avgd%s_friendable.%s_coil%d_residual),2))+(0",
            tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil,scale.Data(),bpms.Data(),tmpDetNames.at(idet).Data(),coil);
        for (Int_t imon = 0 ; imon < nmons ; imon++) {
          tmpName = Form("%s+(dit_%s_avgd%s_friendable.%s_evMon%d_mean*dit_13746.evMon%d_coil%d)",tmpName.Data(),scale.Data(),bpms.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
          tmpErrName = Form("%s+pow(dit_%s_avgd%s_friendable.%s_evMon%d_mean*dit_13746.evMon%d_coil%d_err,2)",tmpErrName.Data(),scale.Data(),bpms.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
        }
        if (nmons == 12) {
          segment_avgd_dit_slopes->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s_coil%d/100",tmpDetNames.at(idet).Data(),coil));
          segment_avgd_dit_slopes->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s_coil%d_err/100",tmpDetNames.at(idet).Data(),coil));
        }
        else {
          segment_avgd_dit_slopes->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s)))%s",tmpName.Data(),tmpFrac.Data()));
          segment_avgd_dit_slopes->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s))",tmpErrName.Data()));
        }

      }
    }

    //reference_regression->SetAlias(Form("us_avg_coil%d_residual",coil),Form("(us_avg_coil%d-((mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon0*evMon0_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon1*evMon1_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon2*evMon2_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon3*evMon3_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon4*evMon4_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,us_avg_frac.Data()));
    //reference_regression->SetAlias(Form("us_dd_coil%d_residual",coil),Form("(us_dd_coil%d-((mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon0*evMon0_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon1*evMon1_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon2*evMon2_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon3*evMon3_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon4*evMon4_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,us_dd_frac.Data()));
    //reference_regression->SetAlias(Form("usl_coil%d_residual",coil),Form("(usl_coil%d-((mini_reference_eigen_reg_5bpms_sorted.usl_evMon0*evMon0_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usl_evMon1*evMon1_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usl_evMon2*evMon2_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usl_evMon3*evMon3_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usl_evMon4*evMon4_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,usl_frac.Data()));
    //reference_regression->SetAlias(Form("usr_coil%d_residual",coil),Form("(usr_coil%d-((mini_reference_eigen_reg_5bpms_sorted.usr_evMon0*evMon0_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usr_evMon1*evMon1_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usr_evMon2*evMon2_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usr_evMon3*evMon3_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usr_evMon4*evMon4_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,usr_frac.Data()));
    //reference_regression->SetAlias(Form("us_avg_coil%d_residual_err",coil),Form("abs(1/us_avg_coil%d)*sqrt((pow(us_avg_coil%d_err*(1-mini_reference_eigen_reg_5bpms_sorted.us_avg_coil%d_residual),2))+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon0_new*evMon0_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon1_new*evMon1_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon2_new*evMon2_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon3_new*evMon3_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon4_new*evMon4_coil%d_err,2))",coil,coil,coil,coil,coil,coil,coil,coil));
    //reference_regression->SetAlias(Form("us_dd_coil%d_residual_err",coil),Form("abs(1/us_dd_coil%d)*sqrt((pow(us_dd_coil%d_err*(1-mini_reference_eigen_reg_5bpms_sorted.us_dd_coil%d_residual),2))+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon0_new*evMon0_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon1_new*evMon1_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon2_new*evMon2_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon3_new*evMon3_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon4_new*evMon4_coil%d_err,2))",coil,coil,coil,coil,coil,coil,coil,coil));
    //reference_regression->SetAlias(Form("usl_coil%d_residual_err",coil),Form("abs(1/usl_coil%d)*sqrt((pow(usl_coil%d_err*(1-mini_reference_eigen_reg_5bpms_sorted.usl_coil%d_residual),2))+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon0_new*evMon0_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon1_new*evMon1_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon2_new*evMon2_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon3_new*evMon3_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon4_new*evMon4_coil%d_err,2))",coil,coil,coil,coil,coil,coil,coil,coil));
    //reference_regression->SetAlias(Form("usr_coil%d_residual_err",coil),Form("abs(1/usr_coil%d)*sqrt((pow(usr_coil%d_err*(1-mini_reference_eigen_reg_5bpms_sorted.usr_coil%d_residual),2))+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon0_new*evMon0_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon1_new*evMon1_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon2_new*evMon2_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon3_new*evMon3_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon4_new*evMon4_coil%d_err,2))",coil,coil,coil,coil,coil,coil,coil,coil));

  //// Net residual (average of all residuals, error weighted, over all coils per minirun)
  dit_13746->SetAlias("us_avg_net_residual","((pow(dit_13746.us_avg_coil1_residual_err,-2)*dit_13746.us_avg_coil1_residual)+(pow(dit_13746.us_avg_coil2_residual_err,-2)*dit_13746.us_avg_coil2_residual)+(pow(dit_13746.us_avg_coil3_residual_err,-2)*dit_13746.us_avg_coil3_residual)+(pow(dit_13746.us_avg_coil4_residual_err,-2)*dit_13746.us_avg_coil4_residual)+(pow(dit_13746.us_avg_coil5_residual_err,-2)*dit_13746.us_avg_coil5_residual)+(pow(dit_13746.us_avg_coil6_residual_err,-2)*dit_13746.us_avg_coil6_residual)+(pow(dit_13746.us_avg_coil7_residual_err,-2)*dit_13746.us_avg_coil7_residual))/(pow(dit_13746.us_avg_coil1_residual_err,-2)+pow(dit_13746.us_avg_coil2_residual_err,-2)+pow(dit_13746.us_avg_coil3_residual_err,-2)+pow(dit_13746.us_avg_coil4_residual_err,-2)+pow(dit_13746.us_avg_coil5_residual_err,-2)+pow(dit_13746.us_avg_coil6_residual_err,-2)+pow(dit_13746.us_avg_coil7_residual_err,-2))");
  dit_13746->SetAlias("us_dd_net_residual","((pow(dit_13746.us_dd_coil1_residual_err,-2)*dit_13746.us_dd_coil1_residual)+(pow(dit_13746.us_dd_coil2_residual_err,-2)*dit_13746.us_dd_coil2_residual)+(pow(dit_13746.us_dd_coil3_residual_err,-2)*dit_13746.us_dd_coil3_residual)+(pow(dit_13746.us_dd_coil4_residual_err,-2)*dit_13746.us_dd_coil4_residual)+(pow(dit_13746.us_dd_coil5_residual_err,-2)*dit_13746.us_dd_coil5_residual)+(pow(dit_13746.us_dd_coil6_residual_err,-2)*dit_13746.us_dd_coil6_residual)+(pow(dit_13746.us_dd_coil7_residual_err,-2)*dit_13746.us_dd_coil7_residual))/(pow(dit_13746.us_dd_coil1_residual_err,-2)+pow(dit_13746.us_dd_coil2_residual_err,-2)+pow(dit_13746.us_dd_coil3_residual_err,-2)+pow(dit_13746.us_dd_coil4_residual_err,-2)+pow(dit_13746.us_dd_coil5_residual_err,-2)+pow(dit_13746.us_dd_coil6_residual_err,-2)+pow(dit_13746.us_dd_coil7_residual_err,-2))");
  dit_13746->SetAlias("usl_net_residual","((pow(dit_13746.usl_coil1_residual_err,-2)*dit_13746.usl_coil1_residual)+(pow(dit_13746.usl_coil2_residual_err,-2)*dit_13746.usl_coil2_residual)+(pow(dit_13746.usl_coil3_residual_err,-2)*dit_13746.usl_coil3_residual)+(pow(dit_13746.usl_coil4_residual_err,-2)*dit_13746.usl_coil4_residual)+(pow(dit_13746.usl_coil5_residual_err,-2)*dit_13746.usl_coil5_residual)+(pow(dit_13746.usl_coil6_residual_err,-2)*dit_13746.usl_coil6_residual)+(pow(dit_13746.usl_coil7_residual_err,-2)*dit_13746.usl_coil7_residual))/(pow(dit_13746.usl_coil1_residual_err,-2)+pow(dit_13746.usl_coil2_residual_err,-2)+pow(dit_13746.usl_coil3_residual_err,-2)+pow(dit_13746.usl_coil4_residual_err,-2)+pow(dit_13746.usl_coil5_residual_err,-2)+pow(dit_13746.usl_coil6_residual_err,-2)+pow(dit_13746.usl_coil7_residual_err,-2))");
  dit_13746->SetAlias("usr_net_residual","((pow(dit_13746.usr_coil1_residual_err,-2)*dit_13746.usr_coil1_residual)+(pow(dit_13746.usr_coil2_residual_err,-2)*dit_13746.usr_coil2_residual)+(pow(dit_13746.usr_coil3_residual_err,-2)*dit_13746.usr_coil3_residual)+(pow(dit_13746.usr_coil4_residual_err,-2)*dit_13746.usr_coil4_residual)+(pow(dit_13746.usr_coil5_residual_err,-2)*dit_13746.usr_coil5_residual)+(pow(dit_13746.usr_coil6_residual_err,-2)*dit_13746.usr_coil6_residual)+(pow(dit_13746.usr_coil7_residual_err,-2)*dit_13746.usr_coil7_residual))/(pow(dit_13746.usr_coil1_residual_err,-2)+pow(dit_13746.usr_coil2_residual_err,-2)+pow(dit_13746.usr_coil3_residual_err,-2)+pow(dit_13746.usr_coil4_residual_err,-2)+pow(dit_13746.usr_coil5_residual_err,-2)+pow(dit_13746.usr_coil6_residual_err,-2)+pow(dit_13746.usr_coil7_residual_err,-2))");
  dit_13746->SetAlias("us_avg_net_residual_err","(pow(dit_13746.us_avg_coil1_residual_err,-2)+pow(dit_13746.us_avg_coil2_residual_err,-2)+pow(dit_13746.us_avg_coil3_residual_err,-2)+pow(dit_13746.us_avg_coil4_residual_err,-2)+pow(dit_13746.us_avg_coil5_residual_err,-2)+pow(dit_13746.us_avg_coil6_residual_err,-2)+pow(dit_13746.us_avg_coil7_residual_err,-2))");
  dit_13746->SetAlias("us_dd_net_residual_err","(pow(dit_13746.us_dd_coil1_residual_err,-2)+pow(dit_13746.us_dd_coil2_residual_err,-2)+pow(dit_13746.us_dd_coil3_residual_err,-2)+pow(dit_13746.us_dd_coil4_residual_err,-2)+pow(dit_13746.us_dd_coil5_residual_err,-2)+pow(dit_13746.us_dd_coil6_residual_err,-2)+pow(dit_13746.us_dd_coil7_residual_err,-2))");
  dit_13746->SetAlias("usl_net_residual_err","(pow(dit_13746.usl_coil1_residual_err,-2)+pow(dit_13746.usl_coil2_residual_err,-2)+pow(dit_13746.usl_coil3_residual_err,-2)+pow(dit_13746.usl_coil4_residual_err,-2)+pow(dit_13746.usl_coil5_residual_err,-2)+pow(dit_13746.usl_coil6_residual_err,-2)+pow(dit_13746.usl_coil7_residual_err,-2))");
  dit_13746->SetAlias("usr_net_residual_err","(pow(dit_13746.usr_coil1_residual_err,-2)+pow(dit_13746.usr_coil2_residual_err,-2)+pow(dit_13746.usr_coil3_residual_err,-2)+pow(dit_13746.usr_coil4_residual_err,-2)+pow(dit_13746.usr_coil5_residual_err,-2)+pow(dit_13746.usr_coil6_residual_err,-2)+pow(dit_13746.usr_coil7_residual_err,-2))");

  part_avgd_regression->SetAlias("us_avg_net_residual","((pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual))/(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("us_dd_net_residual","((pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual))/(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("usl_net_residual","((pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual))/(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("usr_net_residual","((pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual))/(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("us_avg_net_residual_err","(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("us_dd_net_residual_err","(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("usl_net_residual_err","(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("usr_net_residual_err","(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2))");

  part_avgd_lagrange->SetAlias("us_avg_net_residual","((pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual))/(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("us_dd_net_residual","((pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual))/(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("usl_net_residual","((pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual))/(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("usr_net_residual","((pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual))/(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("us_avg_net_residual_err","(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("us_dd_net_residual_err","(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("usl_net_residual_err","(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("usr_net_residual_err","(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2))");

  //reference_regression->SetAlias("us_avg_net_residual","sqrt(pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil1_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil2_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil3_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil4_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil5_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil6_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil7_residual,2))");
  //reference_regression->SetAlias("us_dd_net_residual","sqrt(pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil1_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil2_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil3_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil4_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil5_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil6_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil7_residual,2))");
  //reference_regression->SetAlias("usl_net_residual","sqrt(pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil1_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil2_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil3_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil4_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil5_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil6_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil7_residual,2))");
  //reference_regression->SetAlias("usr_net_residual","sqrt(pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil1_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil2_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil3_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil4_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil5_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil6_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil7_residual,2))");

  segment_avgd_dit_slopes->SetAlias("us_avg_net_residual","((pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual))/(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("us_dd_net_residual","((pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual))/(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("usl_net_residual","((pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual))/(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("usr_net_residual","((pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual))/(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("us_avg_net_residual_err","(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("us_dd_net_residual_err","(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("usl_net_residual_err","(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("usr_net_residual_err","(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err,-2))");


  // FIXME
  ////// Define slope differences here - chose one baseline (dit..._avgd_friendable perhaps?) and compare the slope of X other analyses to it
  for (Int_t xx = 0 ; xx<nmons ; xx++) {
    dit_13746->SetAlias(Form("us_avg_evMon%d_run_dit_dit_diff",xx),Form("dit_13746.us_avg_evMon%d-dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_evMon%d_mean",xx,xx));
    dit_13746->SetAlias(Form("us_dd_evMon%d_run_dit_dit_diff", xx),Form("dit_13746.us_dd_evMon%d-dit_"+ scale+"_avgd"+bpms+"_friendable.us_dd_evMon%d_mean",xx,xx));
    dit_13746->SetAlias(Form("usl_evMon%d_run_dit_dit_diff",   xx),Form("dit_13746.usl_evMon%d-dit_"+   scale+"_avgd"+bpms+"_friendable.usl_evMon%d_mean",xx,xx));
    dit_13746->SetAlias(Form("usr_evMon%d_run_dit_dit_diff",   xx),Form("dit_13746.usr_evMon%d-dit_"+   scale+"_avgd"+bpms+"_friendable.usr_evMon%d_mean",xx,xx));

    part_avgd_lagrange->SetAlias(Form("us_avg_evMon%d_lagr_dit_diff",xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon%d_new/1e-3)-dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_evMon%d_mean",xx,xx));
    part_avgd_lagrange->SetAlias(Form("us_dd_evMon%d_lagr_dit_diff", xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon%d_new/1e-3)-dit_"+ scale+"_avgd"+bpms+"_friendable.us_dd_evMon%d_mean",xx,xx));
    part_avgd_lagrange->SetAlias(Form("usl_evMon%d_lagr_dit_diff",   xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon%d_new/1e-3)-dit_"+   scale+"_avgd"+bpms+"_friendable.usl_evMon%d_mean",xx,xx));
    part_avgd_lagrange->SetAlias(Form("usr_evMon%d_lagr_dit_diff",   xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon%d_new/1e-3)-dit_"+   scale+"_avgd"+bpms+"_friendable.usr_evMon%d_mean",xx,xx));

    part_avgd_lagrange->SetAlias(Form("us_avg_evMon%d_lagr_reg_diff",xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon%d_new/1e-3)-(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon%d_new/1e-3)",xx,xx));
    part_avgd_lagrange->SetAlias(Form("us_dd_evMon%d_lagr_reg_diff", xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon%d_new/1e-3)-(mini_eigen_reg_"+nbpms+"_"+ scale+"_avg.us_dd_evMon%d_new/1e-3)",xx,xx));
    part_avgd_lagrange->SetAlias(Form("usl_evMon%d_lagr_reg_diff",   xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon%d_new/1e-3)-(mini_eigen_reg_"+nbpms+"_"+   scale+"_avg.usl_evMon%d_new/1e-3)",xx,xx));
    part_avgd_lagrange->SetAlias(Form("usr_evMon%d_lagr_reg_diff",   xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon%d_new/1e-3)-(mini_eigen_reg_"+nbpms+"_"+   scale+"_avg.usr_evMon%d_new/1e-3)",xx,xx));

    part_avgd_regression->SetAlias(Form("us_avg_evMon%d_reg_dit_diff",xx),Form("(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon%d_new/1e-3)-dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_evMon%d_mean",xx,xx));
    part_avgd_regression->SetAlias(Form("us_dd_evMon%d_reg_dit_diff", xx),Form("(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_evMon%d_new/1e-3)-dit_"+ scale+"_avgd"+bpms+"_friendable.us_dd_evMon%d_mean",xx,xx));
    part_avgd_regression->SetAlias(Form("usl_evMon%d_reg_dit_diff",   xx),Form("(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_evMon%d_new/1e-3)-dit_"+   scale+"_avgd"+bpms+"_friendable.usl_evMon%d_mean",xx,xx));
    part_avgd_regression->SetAlias(Form("usr_evMon%d_reg_dit_diff",   xx),Form("(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_evMon%d_new/1e-3)-dit_"+   scale+"_avgd"+bpms+"_friendable.usr_evMon%d_mean",xx,xx));
  }

  // The setbranchaddresses onto the residuals_mini tree apparently don't like being redone.... so just wipe residuals_mini and reset it...
  //   To execute these must be inside 1 level down folder (do processed_respin2_data folder, has links to above folders inside)

  /* Commenting block open out here */
  // Run dit - segment avgd dit slopes differences
  // Main det weighted
  draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
  draw                 = Form("dit_13746.#_#_run_dit_dit_diff");
  //draw                 = Form("%sdit_13746.#_#_run_dit_dit_diff",mod_draw.Data());
  drawn_channels_error = "1"; // no assumed error per slope diff? Does this really work?
  Printf("Run dit - segment avgd dit slopes differences");
  combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_residuals_run_dit_dit_diff_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,monitors_choice,residuals_cuts_runwise,{},0);
  out_tree_residuals_run_dit_dit_diff_det_weighted->Write();
  delete out_tree_residuals_run_dit_dit_diff_det_weighted;

  delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
  residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
  // Reg - dit slopes differences
  // Main det weighted
  draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
  //draw                 = Form("%smini_eigen_reg_%s_%s_avg.#_#_reg_dit_diff",mod_draw.Data(),nbpms.Data(),type.Data());
  draw                 = Form("mini_eigen_reg_%s_%s_avg.#_#_reg_dit_diff",nbpms.Data(),type.Data());
  //////// NO ERROR given for slope differences.... could do some error propagation with assumed 3% errors??? 
  //////// drawn_channels_error = "abs(mini_eigen_reg_%s_part_avg.#_#*_reg_dit_diff_error)";
  drawn_channels_error = "1"; // no assumed error per slope diff? Does this really work?
  Printf("Reg vs segment avgd dit slopes differences");
  combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_residuals_reg_dit_diff_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,monitors_choice,residuals_cuts,{},0);
  out_tree_residuals_reg_dit_diff_det_weighted->Write();
  delete out_tree_residuals_reg_dit_diff_det_weighted;

  delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
  residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
  // Lagr - dit slopes differences
  // Main det weighted
  draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
  //draw                 = Form("%smini_eigen_lagr_%s_%s_avg.#_#_lagr_dit_diff",mod_draw.Data(),nbpms.Data(),type.Data());
  draw                 = Form("mini_eigen_lagr_%s_%s_avg.#_#_lagr_dit_diff",nbpms.Data(),type.Data());
  //////// NO ERROR given for slope differences.... could do some error propagation with assumed 3% errors??? 
  //////// drawn_channels_error = "abs(mini_eigen_lagr_%s_part_avg.#_#*_lagr_dit_diff_error)";
  drawn_channels_error = "1"; // no assumed error per slope diff? Does this really work?
  Printf("Lagr vs segment avgd dit slopes differences");
  combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_residuals_lagr_dit_diff_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,monitors_choice,residuals_cuts,{},0);
  out_tree_residuals_lagr_dit_diff_det_weighted->Write();
  delete out_tree_residuals_lagr_dit_diff_det_weighted;


  // Slope differences
  delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
  residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
  // Lagr - reg slopes differences
  // Main det weighted
  draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
  //draw                 = Form("%smini_eigen_lagr_%s_%s_avg.#_#_lagr_reg_diff",mod_draw.Data(),nbpms.Data(),type.Data());
  draw                 = Form("mini_eigen_lagr_%s_%s_avg.#_#_lagr_reg_diff",nbpms.Data(),type.Data());
  //////// NO ERROR given for slope differences.... could do some error propagation with assumed 3% errors??? 
  //////// drawn_channels_error = "abs(mini_eigen_lagr_%s_part_avg.#_#*_lagr_reg_diff_error)";
  drawn_channels_error = "1"; // no assumed error per slope diff? Does this really work?
  Printf("Lagr vs reg slopes differences");
  combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_residuals_lagr_reg_diff_det_weighted,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,monitors_choice,residuals_cuts,{},0);
  out_tree_residuals_lagr_reg_diff_det_weighted->Write();
  delete out_tree_residuals_lagr_reg_diff_det_weighted;

  // Residuals
  delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
  residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
  // Residuals
  // Reg residuals 
  // Self err weighted
  //draw_weighting_error = Form("mini_eigen_reg_%s_%s_avg.#_#_residual_err",nbpms.Data(),type.Data());
  draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
  draw                 = Form("mini_eigen_reg_%s_%s_avg.#_#_residual",nbpms.Data(),type.Data());
  drawn_channels_error = Form("mini_eigen_reg_%s_%s_avg.#_#_residual_err",nbpms.Data(),type.Data()); 
  Printf("Reg self err weighted residuals");
  combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_residuals_reg_res,draw,draw_weighting_error,drawn_channels_error,special_detectors,coils,coils,residuals_cuts,{},0);
  out_tree_residuals_reg_res->Write();
  delete out_tree_residuals_reg_res;

  delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
  residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
  // Lagr residuals 
  // Self err weighted
  //draw_weighting_error = Form("mini_eigen_lagr_%s_%s_avg.#_#_residual_err",nbpms.Data(),type.Data());
  draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
  draw                 = Form("mini_eigen_lagr_%s_%s_avg.#_#_residual",nbpms.Data(),type.Data());
  drawn_channels_error = Form("mini_eigen_lagr_%s_%s_avg.#_#_residual_err",nbpms.Data(),type.Data()); 
  Printf("Lagr self err weighted residuals");
  combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_residuals_lagr_res,draw,draw_weighting_error,drawn_channels_error,special_detectors,coils,coils,residuals_cuts,{},0);
  out_tree_residuals_lagr_res->Write();
  delete out_tree_residuals_lagr_res;

  delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
  residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
  // Dit residuals 
  // Self err weighted
  Printf("Dit self err weighted residuals");
  //draw_weighting_error = Form("dit_%s_avgd%s_friendable.#_#_residual_err",type.Data(),bpms.Data());
  draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
  draw                 = Form("dit_%s_avgd%s_friendable.#_#_residual",type.Data(),bpms.Data());
  drawn_channels_error = Form("dit_%s_avgd%s_friendable.#_#_residual_err",type.Data(),bpms.Data()); 
  combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_residuals_avg_dit_res,draw,draw_weighting_error,drawn_channels_error,special_detectors,coils,coils,residuals_cuts,{},0);
  out_tree_residuals_avg_dit_res->Write();
  delete out_tree_residuals_avg_dit_res;


  // Slopes
  if (!bpms.Contains("allbpms")){
    delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
    residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
    residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
    residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
    residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
    residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
    // Dit slopes
    // Maindet err weighted
    draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
    draw                 = Form("dit_13746.#_#");
    drawn_channels_error = Form("1");
  Printf("Run dit slopes maindet err weighted");
    combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_slopes_run_dit,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,monitors_choice,cuts,{},0);
    out_tree_slopes_run_dit->Write();
    delete out_tree_slopes_run_dit;

    delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
    residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
    residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
    residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
    residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
    residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
    // Part avg dit slopes 
    // Self err weighted
    draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
    draw                 = Form("dit_%s_avgd%s_friendable.#_#_mean",type.Data(),bpms.Data());
    drawn_channels_error = Form("1");
  Printf("Part avgd dit slopes maindet err weighted");
    combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_slopes_part_avg_dit,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,monitors_choice,cuts,{},0);
    out_tree_slopes_part_avg_dit->Write();
    delete out_tree_slopes_part_avg_dit;
  }

  delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
  residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
  // Reg slopes
  // Maindet err weighted
  draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
  draw                 = Form("mini_eigen_reg_%s_%s_avg.#_#_new",nbpms.Data(),type.Data());
  drawn_channels_error = Form("1");
  Printf("Reg slopes maindet err weighted");
  combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_slopes_part_avg_reg,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,monitors_choice,cuts,{},0);
  out_tree_slopes_part_avg_reg->Write();
  delete out_tree_slopes_part_avg_reg;

  delete residuals_mini;
  residuals_mini           = new TChain("mini");
  residuals_mini          ->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",lagr_err.Data(),type.Data(),bpms.Data(),type.Data())); // Make more of this
  residuals_mini->AddFriend(dit_13746              ,     "dit_13746");
  residuals_mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),type.Data()));
  residuals_mini->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  residuals_mini->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",type.Data(),bpms.Data()));
  // Lagr slopes
  // Maindet err weighted
  draw_weighting_error = Form("((rcdb_arm_flag==0)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_%s_%s_avg.lagr_asym_usl.err))",nbpms.Data(),type.Data(),nbpms.Data(),type.Data(),nbpms.Data(),type.Data());
  draw                 = Form("mini_eigen_lagr_%s_%s_avg.#_#_new",nbpms.Data(),type.Data());
  drawn_channels_error = Form("1");
  Printf("Lagr slopes maindet err weighted");
  combo_tg_err_segment_getter(averaging_timescale,residuals_mini,out_tree_slopes_part_avg_lagr,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,monitors_choice,cuts,{},0);
  out_tree_slopes_part_avg_lagr->Write();
  delete out_tree_slopes_part_avg_lagr;
  /* Open comment out here */

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
    //TFile data_file_cors(Form("processed_respin2_data/CREX_All_%s_Null_Avg_Outputs.root",averaging_timescale.Data()),"read");
    //TFile data_file_asyms(Form("processed_respin2_data/CREX_All_%s_Null_Avg_Outputs.root",averaging_timescale.Data()),"read");
    TFile data_file_cors(Form("processed_respin2_data/CREX_All_%s_%s_Avg_Outputs%s.root",DataSetCut.Data(),averaging_timescale.Data(),suffix.Data()),"read");
    TFile data_file_asyms(Form("processed_respin2_data/CREX_All_%s_%s_Avg_Outputs%s.root",DataSetCut.Data(),averaging_timescale.Data(),suffix.Data()),"read");
    // Just use one of the available trees for rcdb etc.
    TTree* mini_slugs = (TTree*)data_file_asyms.Get("mini_raw_det_asyms_det_weighted");
    // FIXME BELOW FIXME Temporarily use the det .... tree as the Mini rcdb holding tree.... for now only
    out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted");
    out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted");
    out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted");
    out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_reference_eigen_reg_allbpms_sorted_corrections_mon_weighted");
    out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_eigen_reg_5bpms_part_avg_corrections_det_weighted");
    out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted");
    out_tree_mini_eigen_reg_allbpms_part_avg_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_eigen_reg_allbpms_part_avg_corrections_det_weighted");
    out_tree_mini_eigen_reg_allbpms_part_avg_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_eigen_reg_allbpms_part_avg_corrections_mon_weighted");
    out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted");
    out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_eigen_lagr_5bpms_part_avg_corrections_mon_weighted");
    out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted");
    out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_eigen_lagr_allbpms_part_avg_corrections_mon_weighted");
    out_tree_mini_regression_corrections_det_weighted     = (TTree*)data_file_cors.Get("mini_regression_corrections_det_weighted");
    out_tree_mini_regression_corrections_mon_weighted     = (TTree*)data_file_cors.Get("mini_regression_corrections_mon_weighted");
    out_tree_mini_overload_corrections_det_weighted     = (TTree*)data_file_cors.Get("mini_overload_corrections_det_weighted");
    out_tree_mini_overload_corrections_mon_weighted     = (TTree*)data_file_cors.Get("mini_overload_corrections_mon_weighted");
    out_tree_mini_dit_plain_corrections_det_weighted     = (TTree*)data_file_cors.Get("mini_dit_plain_corrections_det_weighted");
    out_tree_mini_dit_plain_corrections_mon_weighted     = (TTree*)data_file_cors.Get("mini_dit_plain_corrections_mon_weighted");
    out_tree_mini_dit_part_avgd_corrections_det_weighted = (TTree*)data_file_cors.Get("mini_dit_part_avgd_corrections_det_weighted");
    out_tree_mini_dit_part_avgd_corrections_mon_weighted = (TTree*)data_file_cors.Get("mini_dit_part_avgd_corrections_mon_weighted");
    // Postpan corrections not possible due to bad format slopes storage - do again with Dance tool in respin2

    out_tree_mini_eigen_allbpms_lagr_5bpms_dit_difference    = (TTree*)data_file_cors.Get("mini_eigen_allbpms_lagr_5bpms_dit_difference");
    out_tree_mini_eigen_allbpms_reg_5bpms_reg_difference     = (TTree*)data_file_cors.Get("mini_eigen_allbpms_reg_5bpms_reg_difference");
    out_tree_mini_eigen_allbpms_lagr_allbpms_reg_difference  = (TTree*)data_file_cors.Get("mini_eigen_allbpms_lagr_allbpms_reg_difference");
    out_tree_mini_eigen_allbpms_lagr_5bpms_reg_difference    = (TTree*)data_file_cors.Get("mini_eigen_allbpms_lagr_5bpms_reg_difference");
    out_tree_mini_eigen_allbpms_reg_5bpms_dit_difference     = (TTree*)data_file_cors.Get("mini_eigen_allbpms_reg_5bpms_dit_difference");
    out_tree_mini_eigen_5bpms_dit_5bpms_reg_difference       = (TTree*)data_file_cors.Get("mini_eigen_5bpms_dit_5bpms_reg_difference");
    out_tree_mini_plain_5bpms_dit_5bpms_reg_difference       = (TTree*)data_file_cors.Get("mini_plain_5bpms_dit_5bpms_reg_difference");

    out_tree_mini_raw_det_asyms_det_weighted                             = (TTree*)data_file_asyms.Get("mini_raw_det_asyms_det_weighted");
    out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted = (TTree*)data_file_asyms.Get("mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted");
    out_tree_mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted = (TTree*)data_file_asyms.Get("mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted");
    out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted         = (TTree*)data_file_asyms.Get("mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted");
    out_tree_mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted         = (TTree*)data_file_asyms.Get("mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted");
    out_tree_mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted         = (TTree*)data_file_asyms.Get("mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted");
    out_tree_mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted         = (TTree*)data_file_asyms.Get("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted");

    out_tree_mini_regression_det_asyms_det_weighted                       = (TTree*)data_file_asyms.Get("mini_regression_det_asyms_det_weighted");
    out_tree_mini_regression_det_asym_cors_det_weighted                   = (TTree*)data_file_asyms.Get("mini_regression_det_asym_cors_det_weighted");
    out_tree_mini_overload_det_asyms_det_weighted                         = (TTree*)data_file_asyms.Get("mini_overload_det_asyms_det_weighted");
    //out_tree_mini_overload_det_asym_cors_det_weighted                     = (TTree*)data_file_asyms.Get("mini_overload_det_asym_cors_det_weighted");
    out_tree_mini_dit_plain_det_asyms_det_weighted                        = (TTree*)data_file_asyms.Get("mini_dit_plain_det_asyms_det_weighted");
    out_tree_mini_dit_part_avgd_det_asyms_det_weighted                    = (TTree*)data_file_asyms.Get("mini_dit_part_avgd_det_asyms_det_weighted");
    /* Special case for Ryan's AT needs
    // FIXME Temporarily use the det .... tree as the Mini rcdb holding tree.... for now only
    TTree* mini_slugs = (TTree*)data_file_asyms.Get("mini_dit_part_avgd_det_asyms_det_weighted");
    out_tree_mini_dit_part_avgd_det_asyms_det_weighted                    = (TTree*)data_file_asyms.Get("mini_dit_part_avgd_det_asyms_det_weighted");
    out_tree_mini_reg_part_avgd_det_asyms_det_weighted                    = (TTree*)data_file_asyms.Get("mini_reg_part_avgd_det_asyms_det_weighted");
    End special case for Ryan's needs*/ 


    out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted = (TTree*)data_file_asyms.Get("mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted");
    out_tree_mini_reference_eigen_reg_allbpms_sorted_monitors_det_weighted = (TTree*)data_file_asyms.Get("mini_reference_eigen_reg_allbpms_sorted_monitors_det_weighted");
    out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted         = (TTree*)data_file_asyms.Get("mini_eigen_reg_5bpms_part_avg_monitors_det_weighted");
    out_tree_mini_eigen_reg_allbpms_part_avg_monitors_det_weighted         = (TTree*)data_file_asyms.Get("mini_eigen_reg_allbpms_part_avg_monitors_det_weighted");
    out_tree_mini_BPMs_det_weighted = (TTree*)data_file_asyms.Get("mini_BPMs_det_weighted");
    out_tree_mini_BCMs_det_weighted = (TTree*)data_file_asyms.Get("mini_BCMs_det_weighted");
    //////////out_tree_mini_eigen_reg_5bpms_part_avg_combos         = (TTree*)data_file.Get("mini_eigen_reg_5bpms_part_avg_combos");
    //////////out_tree_mini_reference_eigen_reg_5bpms_sorted_combos = (TTree*)data_file.Get("mini_reference_eigen_reg_5bpms_sorted_combos");


    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_5bpms_sorted_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_allbpms_sorted_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_5bpms_part_avg_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_5bpms_part_avg_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_allbpms_part_avg_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_allbpms_part_avg_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_lagr_5bpms_part_avg_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_lagr_allbpms_part_avg_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_regression_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_regression_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_overload_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_overload_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_plain_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_plain_corrections_mon_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_part_avgd_corrections_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_part_avgd_corrections_mon_weighted);

    mini_slugs->AddFriend(out_tree_mini_eigen_allbpms_lagr_5bpms_dit_difference);
    mini_slugs->AddFriend(out_tree_mini_eigen_allbpms_reg_5bpms_reg_difference);
    mini_slugs->AddFriend(out_tree_mini_eigen_allbpms_lagr_allbpms_reg_difference);
    mini_slugs->AddFriend(out_tree_mini_eigen_allbpms_lagr_5bpms_reg_difference);
    mini_slugs->AddFriend(out_tree_mini_eigen_allbpms_reg_5bpms_dit_difference);
    mini_slugs->AddFriend(out_tree_mini_eigen_5bpms_dit_5bpms_reg_difference);
    mini_slugs->AddFriend(out_tree_mini_plain_5bpms_dit_5bpms_reg_difference);

    mini_slugs->AddFriend(out_tree_mini_raw_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_regression_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_regression_det_asym_cors_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_overload_det_asyms_det_weighted);
    //mini_slugs->AddFriend(out_tree_mini_overload_det_asym_cors_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_plain_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_dit_part_avgd_det_asyms_det_weighted);

    /* Special AT case for ryan
    mini_slugs->AddFriend(out_tree_mini_dit_part_avgd_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reg_part_avgd_det_asyms_det_weighted);
    End special case */

    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_5bpms_sorted_monitors_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reference_eigen_reg_allbpms_sorted_monitors_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_5bpms_part_avg_monitors_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_eigen_reg_allbpms_part_avg_monitors_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_BPMs_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_BCMs_det_weighted);


    TString corrections_pdfname = Form("processed_respin2_data/CREX_All_%s_%s_Avg_Outputs%s",DataSetCut.Data(),averaging_timescale.Data(),suffix.Data());
    TString corrections_pdfname_mod_cut_suffix = "_"+mod_cut+"_"+DataSetCut;
    corrections_pdfname_mod_cut_suffix.ReplaceAll("&","").ReplaceAll("=","").ReplaceAll("|","").ReplaceAll(">","").ReplaceAll("<","").ReplaceAll(".","").ReplaceAll("!","").ReplaceAll(" ","");
    if (mod_cut == ""){
      corrections_pdfname_mod_cut_suffix = "";
    }

    TString ana = "Eigen BPMs combos";
    TText *label = new TText(0.0,0.005,ana.Data());
    label->SetTextFont(23);
    label->SetTextSize(12);
    label->SetNDC();

    std::ofstream outfile0;
    outfile0.open(Form("./processed_respin2_data/Results_%s_%s.csv",DataSetCut.Data(),averaging_timescale.Data()),std::ofstream::app);
    outfile0 << "Averaged outputs " << std::endl;
    outfile0 << "draw, "
             << "averaging, "
             << "draw_weighting_error, "
             << "cuts.at(icut), "
             << "avg_tmp1, "
             << "mean_err_tmp1,"
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


    // Corrections
    corrections_pdfname = type+"_avg_evMons_accumulated_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;

    corrections_pdfname = type+"_avg_5bpms_reg_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < monitors5_new.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors5_new.at(jmon)};
      TCanvas* c2_1_1_1 = new TCanvas();
      ana = "main det weighted - part-avg eigenvector regression - 5bpms - cor from " + monitors5_new.at(jmon) + " , ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_1_1_1->cd();
      c2_1_1_1->SetTitle(ana.Data());
      c2_1_1_1->SetName(ana.Data());
      c2_1_1_1->Divide(1,special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
      draw                 =     Form("%smini_eigen_reg_5bpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
      drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_1_1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_1_1_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_1_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == monitors5_new.size()-1) {
        //c2_1_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_1_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < monitors5_new.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors5_new.at(jmon)};
      TCanvas* c2_1_1_2 = new TCanvas();
      ana = "local err weighted - part-avg eigenvector regression - 5bpms - cor from " + monitors5_new.at(jmon) + " , ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_1_1_2->cd();
      c2_1_1_2->SetTitle(ana.Data());
      c2_1_1_2->SetName(ana.Data());
      c2_1_1_2->Divide(1,special_detectors.size());
      draw_weighting_error = "abs(mini_eigen_reg_5bpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      draw                 =     Form("%smini_eigen_reg_5bpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
      drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_1_1_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_1_1_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_1_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == monitors5_new.size()-1) {
        c2_1_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_1_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    /*TCanvas* c2_1_1_1 = new TCanvas();
    ana = "main det weighted - part-avg eigenvector regression - 5bpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c2_1_1_1->cd();
    c2_1_1_1->SetTitle(ana.Data());
    c2_1_1_1->SetName(ana.Data());
    c2_1_1_1->Divide(monitors5_new.size(),special_detectors.size());
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_eigen_reg_5bpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_1_1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors5_new,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

    c2_1_1_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_1_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
    */

    corrections_pdfname = type+"_avg_allbpms_reg_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < monitors12_new.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors12_new.at(jmon)};
      TCanvas* c2_1_2_1 = new TCanvas();
      ana = "main det weighted - part-avg eigenvector regression - allbpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_1_2_1->cd();
      c2_1_2_1->SetTitle(ana.Data());
      c2_1_2_1->SetName(ana.Data());
      c2_1_2_1->Divide(1,special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
      draw                 =     Form("%smini_eigen_reg_allbpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
      drawn_channels_error = "abs(mini_eigen_reg_allbpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_1_2_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
      c2_1_2_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_1_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == monitors12_new.size()-1) {
        //c2_1_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_1_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < monitors12_new.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors12_new.at(jmon)};
      TCanvas* c2_1_2_2 = new TCanvas();
      ana = "local err weighted - part-avg eigenvector regression - allbpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_1_2_2->cd();
      c2_1_2_2->SetTitle(ana.Data());
      c2_1_2_2->SetName(ana.Data());
      c2_1_2_2->Divide(1,special_detectors.size());
      draw_weighting_error = "abs(mini_eigen_reg_allbpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      draw                 =     Form("%smini_eigen_reg_allbpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
      drawn_channels_error = "abs(mini_eigen_reg_allbpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_1_2_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
      c2_1_2_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_1_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == monitors12_new.size()-1) {
        c2_1_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_1_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    corrections_pdfname = type+"_avg_5bpms_reference_reg_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < monitors5.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors5.at(jmon)};
      TCanvas* c2_2_1_1 = new TCanvas();
      ana = "main det weighted - burstwise reference eigenvector regression - 5bpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_2_1_1->cd();
      c2_2_1_1->SetTitle(ana.Data());
      c2_2_1_1->SetName(ana.Data());
      c2_2_1_1->Divide(1,special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
      draw                 =     Form("%smini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());  // 1/mm * mm = 1 -> ppb = 1/ppb
      drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_2_1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
      c2_2_1_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_2_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == monitors5.size()-1) {
        //c2_2_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_2_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < monitors5.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors5.at(jmon)};
      TCanvas* c2_2_1_2 = new TCanvas();
      ana = "local err weighted - burstwise reference eigenvector regression - 5bpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_2_1_2->cd();
      c2_2_1_2->SetTitle(ana.Data());
      c2_2_1_2->SetName(ana.Data());
      c2_2_1_2->Divide(1,special_detectors.size());
      draw_weighting_error = "abs(mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      draw                 =     Form("%smini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());  // 1/mm * mm = 1 -> ppb = 1/ppb
      drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_2_1_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
      c2_2_1_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_2_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == monitors5.size()-1) {
        c2_2_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_2_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    corrections_pdfname = type+"_avg_allbpms_reference_reg_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < monitors12.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors12.at(jmon)};
      TCanvas* c2_2_2_1 = new TCanvas();
      ana = "main det weighted - burstwise reference eigenvector regression - allbpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_2_2_1->cd();
      c2_2_2_1->SetTitle(ana.Data());
      c2_2_2_1->SetName(ana.Data());
      c2_2_2_1->Divide(1,special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
      draw                 =     Form("%smini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());  // 1/mm * mm = 1 -> ppb = 1/ppb
      drawn_channels_error = "abs(mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_2_2_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
      c2_2_2_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_2_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == monitors12.size()-1) {
        //c2_2_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_2_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < monitors12.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors12.at(jmon)};
      TCanvas* c2_2_2_2 = new TCanvas();
      ana = "local err weighted - burstwise reference eigenvector regression - allbpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_2_2_2->cd();
      c2_2_2_2->SetTitle(ana.Data());
      c2_2_2_2->SetName(ana.Data());
      c2_2_2_2->Divide(1,special_detectors.size());
      draw_weighting_error = "abs(mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      draw                 =     Form("%smini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());  // 1/mm * mm = 1 -> ppb = 1/ppb
      drawn_channels_error = "abs(mini_reference_eigen_reg_allbpms_sorted_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_2_2_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
      c2_2_2_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_2_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == monitors12.size()-1) {
        c2_2_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_2_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    corrections_pdfname = type+"_avg_5bpms_lagr_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < monitors5_new.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors5_new.at(jmon)};
      TCanvas* c2_2_3_1 = new TCanvas();
      ana = "main det weighted - part-avg eigenvector lagrange - 5bpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_2_3_1->cd();
      c2_2_3_1->SetTitle(ana.Data());
      c2_2_3_1->SetName(ana.Data());
      c2_2_3_1->Divide(1,special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
      draw                 =     Form("%smini_eigen_lagr_5bpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());  // 1/mm * mm = 1 -> ppb = 1/ppb
      drawn_channels_error = "abs(mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_2_3_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_2_3_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_2_3_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == monitors5_new.size()-1) {
        //c2_2_3_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_2_3_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < monitors5_new.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors5_new.at(jmon)};
      TCanvas* c2_2_3_2 = new TCanvas();
      ana = "local err weighted - part-avg eigenvector lagrange - 5bpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_2_3_2->cd();
      c2_2_3_2->SetTitle(ana.Data());
      c2_2_3_2->SetName(ana.Data());
      c2_2_3_2->Divide(1,special_detectors.size());
      draw_weighting_error = "abs(mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      draw                 =     Form("%smini_eigen_lagr_5bpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());  // 1/mm * mm = 1 -> ppb = 1/ppb
      drawn_channels_error = "abs(mini_eigen_lagr_5bpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_2_3_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_2_3_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_2_3_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == monitors5_new.size()-1) {
        c2_2_3_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_2_3_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    corrections_pdfname = type+"_avg_allbpms_lagr_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < monitors12_new.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors12_new.at(jmon)};
      TCanvas* c2_2_4_1 = new TCanvas();
      ana = "main det weighted - part-avg eigenvector lagrange - allbpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_2_4_1->cd();
      c2_2_4_1->SetTitle(ana.Data());
      c2_2_4_1->SetName(ana.Data());
      c2_2_4_1->Divide(1,special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
      draw                 =     Form("%smini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());  // 1/mm * mm = 1 -> ppb = 1/ppb
      drawn_channels_error = "abs(mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_2_4_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_2_4_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_2_4_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == monitors12_new.size()-1) {
        //c2_2_4_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_2_4_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < monitors12_new.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors12_new.at(jmon)};
      TCanvas* c2_2_4_2 = new TCanvas();
      ana = "local err weighted - part-avg eigenvector lagrange - allbpms - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_2_4_2->cd();
      c2_2_4_2->SetTitle(ana.Data());
      c2_2_4_2->SetName(ana.Data());
      c2_2_4_2->Divide(1,special_detectors.size());
      draw_weighting_error = "abs(mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      draw                 =     Form("%smini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());  // 1/mm * mm = 1 -> ppb = 1/ppb
      drawn_channels_error = "abs(mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.#_#_mean_err/1.0e-9)";
      p1=manyGraph(c2_2_4_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_2_4_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_2_4_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == monitors12_new.size()-1) {
        c2_2_4_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_2_4_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    corrections_pdfname = type+"_5bpms_plain_dit_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < dit_slope_devices5.size() ; jmon++) {
      std::vector<TString> tmpVec = {dit_slope_devices5.at(jmon)};
      TCanvas* c2_3_1_1 = new TCanvas();
      ana = "main det weighted - standard bmod - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_3_1_1->cd();
      c2_3_1_1->SetTitle(ana.Data());
      c2_3_1_1->SetName(ana.Data());
      c2_3_1_1->Divide(1,dit_special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-6)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-6)";
      draw                 =     Form("%smini_dit_plain_corrections_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());                         // ppm/um * mm = 1e-3 -> ppb = 1e-3/1e-9 = 1/1e-6
      drawn_channels_error = "abs(mini_dit_plain_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      p1=manyGraph(c2_3_1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_3_1_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_3_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == dit_slope_devices5.size()-1) {
        //c2_3_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_3_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < dit_slope_devices5.size() ; jmon++) {
      std::vector<TString> tmpVec = {dit_slope_devices5.at(jmon)};
      TCanvas* c2_3_1_2 = new TCanvas();
      ana = "local err weighted - standard bmod - cor per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_3_1_2->cd();
      c2_3_1_2->SetTitle(ana.Data());
      c2_3_1_2->SetName(ana.Data());
      c2_3_1_2->Divide(1,dit_special_detectors.size());
      draw_weighting_error = "abs(mini_dit_plain_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      draw                 =     Form("%smini_dit_plain_corrections_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());                         // ppm/um * mm = 1e-3 -> ppb = 1e-3/1e-9 = 1/1e-6
      drawn_channels_error = "abs(mini_dit_plain_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      p1=manyGraph(c2_3_1_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_3_1_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_3_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == dit_slope_devices5.size()-1) {
        c2_3_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_3_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    corrections_pdfname = type+"_avg_5bpms_eigen_dit_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < monitors5.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors5.at(jmon)};
      TCanvas* c2_3_2_1 = new TCanvas();
      ana = "main det weighted - eigenvector bmod - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_3_2_1->cd();
      c2_3_2_1->SetTitle(ana.Data());
      c2_3_2_1->SetName(ana.Data());
      c2_3_2_1->Divide(1,dit_special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-6)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-6)";
      draw                 =     Form("%smini_dit_part_avgd_corrections_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());
      drawn_channels_error = "abs(mini_dit_part_avgd_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      p1=manyGraph(c2_3_2_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_3_2_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_3_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == monitors5.size()-1) {
        //c2_3_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_3_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < monitors5.size() ; jmon++) {
      std::vector<TString> tmpVec = {monitors5.at(jmon)};
      TCanvas* c2_3_2_2 = new TCanvas();
      ana = "local err weighted - eigenvector bmod - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_3_2_2->cd();
      c2_3_2_2->SetTitle(ana.Data());
      c2_3_2_2->SetName(ana.Data());
      c2_3_2_2->Divide(1,dit_special_detectors.size());
      draw_weighting_error = "abs(mini_dit_part_avgd_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      draw                 =     Form("%smini_dit_part_avgd_corrections_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());
      drawn_channels_error = "abs(mini_dit_part_avgd_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      p1=manyGraph(c2_3_2_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,dit_special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_3_2_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_3_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == monitors5.size()-1) {
        c2_3_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_3_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    corrections_pdfname = type+"_5bpms_plain_reg_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < devices5.size() ; jmon++) {
      std::vector<TString> tmpVec = {devices5.at(jmon)};
      TCanvas* c2_3_3_1 = new TCanvas();
      ana = "main det weighted - plain regression - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_3_3_1->cd();
      c2_3_3_1->SetTitle(ana.Data());
      c2_3_3_1->SetName(ana.Data());
      c2_3_3_1->Divide(1,special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_regression_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_regression_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-6)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-6)";
      draw                 =     Form("%smini_regression_corrections_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());
      drawn_channels_error = "abs(mini_regression_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      p1=manyGraph(c2_3_3_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_3_3_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_3_3_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == devices5.size()-1) {
        //c2_3_3_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_3_3_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < devices5.size() ; jmon++) {
      std::vector<TString> tmpVec = {devices5.at(jmon)};
      TCanvas* c2_3_3_2 = new TCanvas();
      ana = "local err weighted - plain regression - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_3_3_2->cd();
      c2_3_3_2->SetTitle(ana.Data());
      c2_3_3_2->SetName(ana.Data());
      c2_3_3_2->Divide(1,special_detectors.size());
      draw_weighting_error = "abs(mini_regression_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      draw                 =     Form("%smini_regression_corrections_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());
      drawn_channels_error = "abs(mini_regression_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      p1=manyGraph(c2_3_3_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_3_3_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_3_3_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == devices5.size()-1) {
        c2_3_3_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_3_3_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    corrections_pdfname = type+"_allbpms_plain_reg_corrections_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jmon = 0 ; jmon < devices12.size() ; jmon++) {
      std::vector<TString> tmpVec = {devices12.at(jmon)};
      TCanvas* c2_3_4_1 = new TCanvas();
      ana = "main det weighted - overload regression - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_3_4_1->cd();
      c2_3_4_1->SetTitle(ana.Data());
      c2_3_4_1->SetName(ana.Data());
      c2_3_4_1->Divide(1,special_detectors.size());
      //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_overload_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_overload_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_overload_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-6)";
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-6)";
      draw                 =     Form("%smini_overload_corrections_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());
      drawn_channels_error = "abs(mini_overload_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      p1=manyGraph(c2_3_4_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_3_4_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jmon == 0) {
        c2_3_4_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      //else if (jmon == devices12.size()-1) {
        //c2_3_4_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      //}
      else {
        c2_3_4_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }
    for (Int_t jmon = 0 ; jmon < devices12.size() ; jmon++) {
      std::vector<TString> tmpVec = {devices12.at(jmon)};
      TCanvas* c2_3_4_2 = new TCanvas();
      ana = "local err weighted - overload regression - corrections per monitor, ppb"; // No more absolute value in this anymore (with the sorted slopes)
      c2_3_4_2->cd();
      c2_3_4_2->SetTitle(ana.Data());
      c2_3_4_2->SetName(ana.Data());
      c2_3_4_2->Divide(1,special_detectors.size());
      draw_weighting_error = "abs(mini_overload_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      draw                 =     Form("%smini_overload_corrections_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());
      drawn_channels_error = "abs(mini_overload_corrections_det_weighted.#_#_mean_err/1.0e-6)";
      p1=manyGraph(c2_3_4_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

      c2_3_4_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      //if (jmon == 0) {
        //c2_3_4_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      //}
      if (jmon == devices12.size()-1) {
        c2_3_4_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c2_3_4_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }



    // Multipletwise differences between analysis methods
    //                                                     
    
    corrections_pdfname = type+"_avg_evMons_accumulated_disagreements_"+ averaging_timescale +"_wise_det_weighted" +corrections_pdfname_mod_cut_suffix;

    TCanvas* c2_dis_1 = new TCanvas();
    ana = "difference between allbpm lagr vs. 5bpm dit asyms, main det weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_1->cd();
    c2_dis_1->SetTitle(ana.Data());
    c2_dis_1->SetName(ana.Data());
    c2_dis_1->Divide(1,special_detectors_hardcoded.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_lagr_5bpms_dit_difference.##_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_lagr_5bpms_dit_difference.##_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,empty_vec,special_detectors_hardcoded,special_detectors_hardcoded,{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));

    TCanvas* c2_dis_2 = new TCanvas();
    ana = "difference between allbpm lagr vs. 5bpm dit asyms, local err weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_2->cd();
    c2_dis_2->SetTitle(ana.Data());
    c2_dis_2->SetName(ana.Data());
    c2_dis_2->Divide(1,special_detectors_hardcoded.size());
    draw_weighting_error = "abs(mini_eigen_allbpms_lagr_5bpms_dit_difference.##_#_mean_err/1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_lagr_5bpms_dit_difference.##_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_lagr_5bpms_dit_difference.##_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,empty_vec,special_detectors_hardcoded,special_detectors_hardcoded,{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_3 = new TCanvas();
    ana = "difference between allbpm reg vs. 5bpm reg asyms, main det weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_3->cd();
    c2_dis_3->SetTitle(ana.Data());
    c2_dis_3->SetName(ana.Data());
    c2_dis_3->Divide(1,special_detectors_hardcoded.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_reg_5bpms_reg_difference.##_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_reg_5bpms_reg_difference.##_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,empty_vec,special_detectors_hardcoded,special_detectors_hardcoded,{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_3->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_3->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_4 = new TCanvas();
    ana = "difference between allbpm reg vs. 5bpm reg asyms, local err weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_4->cd();
    c2_dis_4->SetTitle(ana.Data());
    c2_dis_4->SetName(ana.Data());
    c2_dis_4->Divide(1,special_detectors_hardcoded.size());
    draw_weighting_error = "abs(mini_eigen_allbpms_reg_5bpms_reg_difference.##_#_mean_err/1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_reg_5bpms_reg_difference.##_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_reg_5bpms_reg_difference.##_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,empty_vec,special_detectors_hardcoded,special_detectors_hardcoded,{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_4->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_4->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_5 = new TCanvas();
    ana = "difference between allbpm lagr vs. allbpm reg asyms, main det weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_5->cd();
    c2_dis_5->SetTitle(ana.Data());
    c2_dis_5->SetName(ana.Data());
    c2_dis_5->Divide(1,special_eigen_lagr_differences_detectors.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_lagr_allbpms_reg_difference.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_lagr_allbpms_reg_difference.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_5,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_eigen_lagr_differences_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_5->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_5->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_6 = new TCanvas();
    ana = "difference between allbpm lagr vs. allbpm reg asyms, local err weighted - distributional differences (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_6->cd();
    c2_dis_6->SetTitle(ana.Data());
    c2_dis_6->SetName(ana.Data());
    c2_dis_6->Divide(1,special_eigen_lagr_differences_detectors.size());
    draw_weighting_error = "abs(mini_eigen_allbpms_lagr_allbpms_reg_difference.#_#_mean_err/1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_lagr_allbpms_reg_difference.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_lagr_allbpms_reg_difference.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_6,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_eigen_lagr_differences_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_6->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_6->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_7 = new TCanvas();
    ana = "difference between allbpm lagr vs. 5bpm reg asyms, main det weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_7->cd();
    c2_dis_7->SetTitle(ana.Data());
    c2_dis_7->SetName(ana.Data());
    c2_dis_7->Divide(1,special_detectors_hardcoded.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_lagr_5bpms_reg_difference.##_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_lagr_5bpms_reg_difference.##_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_7,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,empty_vec,special_detectors_hardcoded,special_detectors_hardcoded,{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_7->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_7->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_8 = new TCanvas();
    ana = "difference between allbpm lagr vs. 5bpm reg asyms, local err weighted - distributional differences (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_8->cd();
    c2_dis_8->SetTitle(ana.Data());
    c2_dis_8->SetName(ana.Data());
    c2_dis_8->Divide(1,special_detectors_hardcoded.size());
    draw_weighting_error = "abs(mini_eigen_allbpms_lagr_5bpms_reg_difference.##_#_mean_err/1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_lagr_5bpms_reg_difference.##_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_lagr_5bpms_reg_difference.##_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_8,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,empty_vec,special_detectors_hardcoded,special_detectors_hardcoded,{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_8->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_8->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dit_9 = new TCanvas();
    ana = "difference between allbpm reg vs. 5bpm dit asyms, main det weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dit_9->cd();
    c2_dit_9->SetTitle(ana.Data());
    c2_dit_9->SetName(ana.Data());
    c2_dit_9->Divide(1,special_detectors_hardcoded.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_reg_5bpms_dit_difference.##_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_reg_5bpms_dit_difference.##_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dit_9,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,empty_vec,special_detectors_hardcoded,special_detectors_hardcoded,{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dit_9->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dit_9->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_10 = new TCanvas();
    ana = "difference between allbpm reg vs. 5bpm dit asyms, local err weighted - distributional differences (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_10->cd();
    c2_dis_10->SetTitle(ana.Data());
    c2_dis_10->SetName(ana.Data());
    c2_dis_10->Divide(1,special_detectors_hardcoded.size());
    draw_weighting_error = "abs(mini_eigen_allbpms_reg_5bpms_dit_difference.##_#_mean_err/1.0e-9)";
    draw                 =     Form("%smini_eigen_allbpms_reg_5bpms_dit_difference.##_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_allbpms_reg_5bpms_dit_difference.##_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_10,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,empty_vec,special_detectors_hardcoded,special_detectors_hardcoded,{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_10->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_10->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_11 = new TCanvas();
    ana = "difference between 5bpm eigen dit vs. 5bpm reg asyms, main det weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_11->cd();
    c2_dis_11->SetTitle(ana.Data());
    c2_dis_11->SetName(ana.Data());
    c2_dis_11->Divide(1,special_eigen_dit_differences_detectors.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_eigen_5bpms_dit_5bpms_reg_difference.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_5bpms_dit_5bpms_reg_difference.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_11,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_eigen_dit_differences_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_11->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_11->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_12 = new TCanvas();
    ana = "difference between 5bpm eigen dit vs. 5bpm reg asyms, local err weighted - distributional differences (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_12->cd();
    c2_dis_12->SetTitle(ana.Data());
    c2_dis_12->SetName(ana.Data());
    c2_dis_12->Divide(1,special_eigen_dit_differences_detectors.size());
    draw_weighting_error = "abs(mini_eigen_5bpms_dit_5bpms_reg_difference.#_#_mean_err/1.0e-9)";
    draw                 =     Form("%smini_eigen_5bpms_dit_5bpms_reg_difference.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_5bpms_dit_5bpms_reg_difference.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_12,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_eigen_dit_differences_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_12->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_12->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_13 = new TCanvas();
    ana = "difference between plain 5bpm dit vs. 5bpm reg asyms, main det weighted - net disagreement (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_13->cd();
    c2_dis_13->SetTitle(ana.Data());
    c2_dis_13->SetName(ana.Data());
    c2_dis_13->Divide(1,special_differences_detectors.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_plain_5bpms_dit_5bpms_reg_difference.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_plain_5bpms_dit_5bpms_reg_difference.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_13,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_differences_detectors,{},{},arm_cuts_plain,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_13->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_13->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c2_dis_14 = new TCanvas();
    ana = "difference between plain 5bpm dit vs 5bpm reg asyms, local err weighted - distributional differences (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c2_dis_14->cd();
    c2_dis_14->SetTitle(ana.Data());
    c2_dis_14->SetName(ana.Data());
    c2_dis_14->Divide(1,special_differences_detectors.size());
    draw_weighting_error = "abs(mini_plain_5bpms_dit_5bpms_reg_difference.#_#_mean_err/1.0e-9)";
    draw                 =     Form("%smini_plain_5bpms_dit_5bpms_reg_difference.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_plain_5bpms_dit_5bpms_reg_difference.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c2_dis_14,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_differences_detectors,{},{},arm_cuts_plain,1); // empty draws_piece3 and cuts and cuts2 vectors
    c2_dis_14->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c2_dis_14->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));



    // Corrected Asyms
    corrections_pdfname = type+"_avg_evMons_accumulated_corrected_asyms_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;

    TCanvas* c3_0_main = new TCanvas();
    ana = "full main det signal - part avg eigenvector allbpm lagr (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_0_main->cd();
    c3_0_main->SetTitle(ana.Data());
    c3_0_main->SetName(ana.Data());
    // FIXME FIXME FIXME: When not doing slug timescale averaging... need to figure out how to avoid the "main_det" -> usl+r+avg transformation assuming slug level arm flags inside toolbox.hh # replacements and right here... to all 3 of these here.
    c3_0_main->Divide(1,1);
    draw_weighting_error =           "mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_manual_main_det_mean_err/(1.0e-9)";
    draw                 =    Form("%smini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_manual_main_det_mean/1.0e-9",mod_draw.Data()); // diff_evMon0.mean, etc.
    drawn_channels_error =       "abs(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_manual_main_det_mean_err/1.0e-9)";
    p1=manyGraph(c3_0_main,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_0_main->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_0_main->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));

    TCanvas* c3_0_cor_main = new TCanvas();
    ana = "full main det signal - total correction - maindet weighted - part avg eigenvector allbpm lagr (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_0_cor_main->cd();
    c3_0_cor_main->SetTitle(ana.Data());
    c3_0_cor_main->SetName(ana.Data());
    // FIXME FIXME FIXME: When not doing slug timescale averaging... need to figure out how to avoid the "main_det" -> usl+r+avg transformation assuming slug level arm flags inside toolbox.hh # replacements and right here... to all 3 of these here.
    c3_0_cor_main->Divide(1,1);
    draw_weighting_error =           "mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_manual_main_det_mean_err/(1.0e-9)";
    draw                 =    Form("%smini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.cor_asym_manual_main_det_mean/1.0e-9",mod_draw.Data()); // diff_evMon0.mean, etc.
    drawn_channels_error =       "abs(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.cor_asym_manual_main_det_mean_err/1.0e-9)";
    p1=manyGraph(c3_0_cor_main,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,cor_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_0_cor_main->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_0_cor_main->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_0_1 = new TCanvas();
    ana = "main det weighted asyms - part avg eigenvector allbpm lagr (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c3_0_1->cd();
    c3_0_1->SetTitle(ana.Data());
    c3_0_1->SetName(ana.Data());
    c3_0_1->Divide(2,(Int_t)ceil(special_detectors.size()/2.0));
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 = Form("%smini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_0_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,lagr_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c3_0_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_0_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_0_1_cor = new TCanvas();
    ana = "main det weighted asym total corrections - part avg eigenvector allbpm lagr (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c3_0_1_cor->cd();
    c3_0_1_cor->SetTitle(ana.Data());
    c3_0_1_cor->SetName(ana.Data());
    c3_0_1_cor->Divide(2,(Int_t)ceil(special_detectors.size()/2.0));
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 = Form("%smini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_0_1_cor,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,cor_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c3_0_1_cor->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_0_1_cor->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_0_2 = new TCanvas();
    ana = "main det weighted asyms - part avg eigenvecvtor 5bpm lagr (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c3_0_2->cd();
    c3_0_2->SetTitle(ana.Data());
    c3_0_2->SetName(ana.Data());
    c3_0_2->Divide(2,(Int_t)ceil(special_detectors.size()/2.0));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 = Form("%smini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_eigen_lagr_5bpms_part_avg_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_0_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,lagr_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c3_0_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_0_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_1_1 = new TCanvas();
    ana = "main det weighted asyms - part avg eigenvector allbpm reg (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c3_1_1->cd();
    c3_1_1->SetTitle(ana.Data());
    c3_1_1->SetName(ana.Data());
    c3_1_1->Divide(reg_asym_vec.size(),(Int_t)ceil(1.0*special_detectors.size()/reg_asym_vec.size()));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 = Form("%smini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c3_1_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_1_2 = new TCanvas();
    ana = "main det weighted asyms - part avg eigenvector 5bpm reg (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c3_1_2->cd();
    c3_1_2->SetTitle(ana.Data());
    c3_1_2->SetName(ana.Data());
    c3_1_2->Divide(reg_asym_vec.size(),(Int_t)ceil(1.0*special_detectors.size()/reg_asym_vec.size()));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 = Form("%smini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_1_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c3_1_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_1_1_cor = new TCanvas();
    ana = "main det weighted asym total corrections - part avg eigenvector allbpm reg (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c3_1_1_cor->cd();
    c3_1_1_cor->SetTitle(ana.Data());
    c3_1_1_cor->SetName(ana.Data());
    c3_1_1_cor->Divide(cor_asym_vec.size(),(Int_t)ceil(1.0*special_detectors.size()/cor_asym_vec.size()));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 = Form("%smini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_eigen_reg_allbpms_part_avg_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_1_1_cor,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,cor_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c3_1_1_cor->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_1_1_cor->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_1_2_cor = new TCanvas();
    ana = "main det weighted asym total corrections - part avg eigenvector 5bpm reg (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c3_1_2_cor->cd();
    c3_1_2_cor->SetTitle(ana.Data());
    c3_1_2_cor->SetName(ana.Data());
    c3_1_2_cor->Divide(cor_asym_vec.size(),(Int_t)ceil(1.0*special_detectors.size()/cor_asym_vec.size()));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 = Form("%smini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_1_2_cor,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,cor_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    c3_1_2_cor->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_1_2_cor->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    /* Begin special case for Ryan's AT needs
    TCanvas* c3_1 = new TCanvas();
    ana = "main det weighted asyms - standard regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_1->cd();
    c3_1->SetTitle(ana.Data());
    c3_1->SetName(ana.Data());
    c3_1->Divide(3,3);
    //c3_1->Divide(2,special_detectors.size()/2);
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    //draw                 = Form("%smini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_#_#_mean/1.0e-9",mod_draw.Data());
    //drawn_channels_error =       "abs(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_#_#_mean_err/1.0e-9)";
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reg_part_avgd_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reg_part_avgd_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reg_part_avgd_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 = Form("%smini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,AT_special_detectors,{},{},AT_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    //p1=manyGraph(c3_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,AT_special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
    //FIXME add ( above for AT case
    //c3_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_1_self = new TCanvas();
    ana = "self weighted asyms - part avg eigenvector regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_1_self->cd();
    c3_1_self->SetTitle(ana.Data());
    c3_1_self->SetName(ana.Data());
    c3_1_self->Divide(3,3);
    //c3_1_self->Divide(2,special_detectors.size()/2);
    //draw_weighting_error =       "abs(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_#_#_mean_self_err/1.0e-9)";
    //draw                 = Form("%smini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_#_#_mean_self/1.0e-9",mod_draw.Data());
    //drawn_channels_error =       "abs(mini_eigen_reg_5bpms_part_avg_det_asyms_det_weighted.reg_#_#_mean_self_err/1.0e-9)";
    draw_weighting_error =       "abs(mini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean_self_err/1.0e-9)";
    draw                 = Form("%smini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean_self/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c3_1_self,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,AT_special_detectors,{},{},AT_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    //p1=manyGraph(c3_1_self,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_1_self->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_1_self->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
    End special case */


    TCanvas* c3_2_1 = new TCanvas();
    ana = "main det weighted asyms - burstwise reference eigenvector allbpms regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_2_1->cd();
    c3_2_1->SetTitle(ana.Data());
    c3_2_1->SetName(ana.Data());
    c3_2_1->Divide(reg_asym_vec.size(),(Int_t)ceil(1.0*special_detectors.size()/reg_asym_vec.size()));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_2_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_2_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_2_2 = new TCanvas();
    ana = "main det weighted asyms - burstwise reference eigenvector 5bpms regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_2_2->cd();
    c3_2_2->SetTitle(ana.Data());
    c3_2_2->SetName(ana.Data());
    c3_2_2->Divide(reg_asym_vec.size(),(Int_t)ceil(1.0*special_detectors.size()/reg_asym_vec.size()));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_2_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_2_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_2_1_cor = new TCanvas();
    ana = "main det weighted asym net corrections - burstwise reference eigenvector allbpms regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_2_1_cor->cd();
    c3_2_1_cor->SetTitle(ana.Data());
    c3_2_1_cor->SetName(ana.Data());
    c3_2_1_cor->Divide(cor_asym_vec.size(),(Int_t)ceil(1.0*special_detectors.size()/cor_asym_vec.size()));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_reference_eigen_reg_allbpms_sorted_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_2_1_cor,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,cor_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_2_1_cor->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_2_1_cor->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_2_2_cor = new TCanvas();
    ana = "main det weighted asym net corrections - burstwise reference eigenvector 5bpms regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_2_2_cor->cd();
    c3_2_2_cor->SetTitle(ana.Data());
    c3_2_2_cor->SetName(ana.Data());
    c3_2_2_cor->Divide(cor_asym_vec.size(),(Int_t)ceil(1.0*special_detectors.size()/cor_asym_vec.size()));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_reference_eigen_reg_5bpms_sorted_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_2_2_cor,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,cor_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_2_2_cor->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_2_2_cor->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_3 = new TCanvas();
    ana = "main det weighted asyms - plain standard regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_3->cd();
    c3_3->SetTitle(ana.Data());
    c3_3->SetName(ana.Data());
    c3_3->Divide(2,(Int_t)ceil(special_detectors.size()/2.0));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_regression_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_regression_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_regression_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_regression_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_3->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_3->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_4 = new TCanvas();
    ana = "main det weighted asyms - plain standard regression net corrections (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_4->cd();
    c3_4->SetTitle(ana.Data());
    c3_4->SetName(ana.Data());
    c3_4->Divide(2,(Int_t)ceil(agg_cor_special_detectors.size()/2.0));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_regression_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_regression_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_regression_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_regression_det_asym_cors_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_regression_det_asym_cors_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,agg_cor_special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_4->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_4->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_5 = new TCanvas();
    ana = "main det weighted asyms - plain overloaded standard regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_5->cd();
    c3_5->SetTitle(ana.Data());
    c3_5->SetName(ana.Data());
    c3_5->Divide(2,(Int_t)ceil(special_detectors.size()/2.0));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_overload_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_overload_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_overload_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_overload_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_overload_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_5,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_5->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_5->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_6 = new TCanvas();
    ana = "main det weighted asyms - part avgd eigenvector dithering (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c3_6->cd();
    c3_6->SetTitle(ana.Data());
    c3_6->SetName(ana.Data());
    c3_6->Divide(2,(Int_t)ceil(special_detectors.size()/2.0));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_dit_part_avgd_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_dit_part_avgd_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_6,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,dit_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_6->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_6->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    /* Begin special AT case for Ryan's needs
    TCanvas* c3_6 = new TCanvas();
    ana = "main det weighted asyms - burstwise part avgd eigenvector dithering (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_6->cd();
    c3_6->SetTitle(ana.Data());
    c3_6->SetName(ana.Data());
    c3_6->Divide(3,3);
    //c3_6->Divide(2,special_detectors.size()/2.0);
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_6,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,AT_special_detectors,{},{},AT_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    //p1=manyGraph(c3_6,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_6->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_6->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_6_self = new TCanvas();
    ana = "self weighted asyms - burstwise part avgd eigenvector dithering (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_6_self->cd();
    c3_6_self->SetTitle(ana.Data());
    c3_6_self->SetName(ana.Data());
    c3_6_self->Divide(3,3);
    //c3_6_self->Divide(2,special_detectors.size()/2.0);
    //draw_weighting_error = "abs(mini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean_self_err/1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean_self/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c3_6_self,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,AT_special_detectors,{},{},AT_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors
    //p1=manyGraph(c3_6_self,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_6_self->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_6_self->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
    End special case */

    TCanvas* c3_7 = new TCanvas();
    ana = "main det weighted asyms - plain dithering (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_7->cd();
    c3_7->SetTitle(ana.Data());
    c3_7->SetName(ana.Data());
    c3_7->Divide(2,(Int_t)ceil(special_detectors.size()/2.0));
    //draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_plain_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-9)";
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_dit_plain_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_dit_plain_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_7,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,dit_asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_7->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_7->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_8 = new TCanvas();
    ana = "Raw main det weighted asyms - using part avgd eigenvector regression corrected asyms as weights (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_8->cd();
    c3_8->SetTitle(ana.Data());
    c3_8->SetName(ana.Data());
    c3_8->Divide(2,(Int_t)ceil(special_detectors.size()/2.0));
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_raw_det_asyms_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_raw_det_asyms_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_8,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_8->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_8->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_9 = new TCanvas();
    ana = "Raw self weighted asyms - self weighted (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_9->cd();
    c3_9->SetTitle(ana.Data());
    c3_9->SetName(ana.Data());
    c3_9->Divide(2,(Int_t)ceil(special_detectors.size()/2.0));
    draw_weighting_error = "abs(mini_raw_det_asyms_det_weighted.#_#_mean_self_err/1.0e-9)";
    draw                 =     Form("%smini_raw_det_asyms_det_weighted.#_#_mean_self/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_raw_det_asyms_det_weighted.#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c3_9,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,special_detectors,{},{},arm_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_9->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_9->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));


    // Monitors
    corrections_pdfname = type + "_avgd_evMons_accumulated_monitors_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    TCanvas* c1_1_1 = new TCanvas();
    ana = "self-weighted BPM means diffs (nano meters)"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c1_1_1->cd();
    c1_1_1->SetTitle(ana.Data());
    c1_1_1->SetName(ana.Data());
    c1_1_1->Divide(4,(Int_t)ceil(plot_yield_diff_BPMs.size()/4.0));
      // Sign corrected
      // No error-rescaling
      // Utilize the slugwise main-det weighted data
      //p1=multiGraph(c1_1_1,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices12.at(k).Data(),devices12.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "abs(mini_BPMs_det_weighted.#_#_mean_self_err/1.0e-6)";
    draw          =     Form("%smini_BPMs_det_weighted.#_#_mean_self/1.0e-6",mod_draw.Data());
    drawn_channels_error = "abs(mini_BPMs_det_weighted.#_#_mean_self_err/1.0e-6)";
    p1=manyGraph(c1_1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,diff_vec,plot_yield_diff_BPMs,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

    c1_1_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));

    TCanvas* c1_1_2 = new TCanvas();
    ana = "self-weighted BPM means yields (mm)"; // No more absolute value in this anymore (with the sorted slopes)
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c1_1_2->cd();
    c1_1_2->SetTitle(ana.Data());
    c1_1_2->SetName(ana.Data());
    c1_1_2->Divide(4,(Int_t)ceil(plot_yield_diff_BPMs.size()/4.0));
      // Sign corrected
      // No error-rescaling
      // Utilize the slugwise main-det weighted data
      //p1=multiGraph(c1_1_2,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices12.at(k).Data(),devices12.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "abs(mini_BPMs_det_weighted.#_#_mean_self_err)";
    draw          =     Form("mini_BPMs_det_weighted.#_#_mean_self");
    // no mod_draw... no need for sign correction...
    //draw          =     Form("%smini_BPMs_det_weighted.#_#_mean_self",mod_draw.Data());
    drawn_channels_error = "abs(mini_BPMs_det_weighted.#_#_mean_self_err)";
    p1=manyGraph(c1_1_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,yield_vec,plot_yield_diff_BPMs,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

    c1_1_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_2_1_12X = new TCanvas();
    ana = "main det weighted BPM means - diffs (nano meters)"; // No more absolute value in this anymore (with the sorted slopes)
    c1_2_1_12X->cd();
    c1_2_1_12X->SetTitle(ana.Data());
    c1_2_1_12X->SetName(ana.Data());
    c1_2_1_12X->Divide(1,1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_BPMs_det_weighted.diff_bpm12X_mean/1.0e-6",mod_draw.Data());
    drawn_channels_error = "abs(mini_BPMs_det_weighted.diff_bpm12X_mean_err/1.0e-6)";
    p1=manyGraph(c1_2_1_12X,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,diff_vec,diff_vec,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

    c1_2_1_12X->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_2_1_12X->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_2_1_4eX = new TCanvas();
    ana = "main det weighted BPM means - diffs (nano meters)"; // No more absolute value in this anymore (with the sorted slopes)
    c1_2_1_4eX->cd();
    c1_2_1_4eX->SetTitle(ana.Data());
    c1_2_1_4eX->SetName(ana.Data());
    c1_2_1_4eX->Divide(1,1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_BPMs_det_weighted.diff_bpm4eX_mean/1.0e-6",mod_draw.Data());
    drawn_channels_error = "abs(mini_BPMs_det_weighted.diff_bpm4eX_mean_err/1.0e-6)";
    p1=manyGraph(c1_2_1_4eX,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,diff_vec,diff_vec,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

    c1_2_1_4eX->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_2_1_4eX->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_2_1 = new TCanvas();
    ana = "main det weighted BPM means - diffs (nano meters)"; // No more absolute value in this anymore (with the sorted slopes)
    c1_2_1->cd();
    c1_2_1->SetTitle(ana.Data());
    c1_2_1->SetName(ana.Data());
    c1_2_1->Divide(4,(Int_t)ceil(plot_yield_diff_BPMs.size()/4.0));
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_BPMs_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());
    drawn_channels_error = "abs(mini_BPMs_det_weighted.#_#_mean_err/1.0e-6)";
    p1=manyGraph(c1_2_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,diff_vec,plot_yield_diff_BPMs,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

    c1_2_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_2_2 = new TCanvas();
    ana = "main det weighted BPM means - yields (mm)"; // No more absolute value in this anymore (with the sorted slopes)
    c1_2_2->cd();
    c1_2_2->SetTitle(ana.Data());
    c1_2_2->SetName(ana.Data());
    c1_2_2->Divide(4,(Int_t)ceil(plot_yield_diff_BPMs.size()/4.0));
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
    draw                 =     Form("mini_BPMs_det_weighted.#_#_mean");
    // no mod_draw... no need for sign correction...
    //draw                 =     Form("%smini_BPMs_det_weighted.#_#_mean",mod_draw.Data());
    drawn_channels_error = "abs(mini_BPMs_det_weighted.#_#_mean_err)";
    p1=manyGraph(c1_2_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,yield_vec,plot_yield_diff_BPMs,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
    c1_2_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_2_3 = new TCanvas();
    ana = "main det weighted 5bpm evMon means - diffs (nano meters)"; // No more absolute value in this anymore (with the sorted slopes)
    c1_2_3->cd();
    c1_2_3->SetTitle(ana.Data());
    c1_2_3->SetName(ana.Data());
    c1_2_3->Divide(1,monitors5.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_eigen_reg_5bpms_part_avg_monitors_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_reg_5bpms_part_avg_monitors_det_weighted.#_#_mean_err/1.0e-6)";
    p1=manyGraph(c1_2_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,diff_vec,monitors5,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
    c1_2_3->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_2_3->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_2_4 = new TCanvas();
    ana = "main det weighted allbpm evMon means - diffs (nano meters)"; // No more absolute value in this anymore (with the sorted slopes)
    c1_2_4->cd();
    c1_2_4->SetTitle(ana.Data());
    c1_2_4->SetName(ana.Data());
    c1_2_4->Divide(3,(Int_t)ceil(monitors12.size()/3.0));
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_eigen_reg_allbpms_part_avg_monitors_det_weighted.#_#_mean/1.0e-6",mod_draw.Data());
    drawn_channels_error = "abs(mini_eigen_reg_allbpms_part_avg_monitors_det_weighted.#_#_mean_err/1.0e-6)";
    p1=manyGraph(c1_2_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,diff_vec,monitors12,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors
    c1_2_4->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_2_4->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));


    TCanvas* c1_3 = new TCanvas();
    ana = "self weighted BCM means (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c1_3->cd();
    c1_3->SetTitle(ana.Data());
    c1_3->SetName(ana.Data());
    c1_3->Divide(4,(Int_t)ceil(yield_bcm_devices.size()/4.0));
    draw_weighting_error = "abs(mini_BCMs_det_weighted.#_#_mean_self_err/1.0e-9)";
    draw                 =     Form("%smini_BCMs_det_weighted.#_#_mean_self/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_BCMs_det_weighted.#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c1_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,yield_bcm_devices,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

    c1_3->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_3->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_4_targ = new TCanvas();
    ana = "main det weighted BCM means (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c1_4_targ->cd();
    c1_4_targ->SetTitle(ana.Data());
    c1_4_targ->SetName(ana.Data());
    c1_4_targ->Divide(1,1);
    // Sign corrected
    // No error-rescaling
    // Utilize the slugwise main-det weighted data
    //p1=multiGraph(c1_4_targ,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices12.at(k).Data(),devices12.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_BCMs_det_weighted.asym_bcm_target_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_BCMs_det_weighted.asym_bcm_target_mean_err/1.0e-9)";
    p1=manyGraph(c1_4_targ,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,asym_vec,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

    c1_4_targ->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_4_targ->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c1_4 = new TCanvas();
    ana = "main det weighted BCM means (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c1_4->cd();
    c1_4->SetTitle(ana.Data());
    c1_4->SetName(ana.Data());
    c1_4->Divide(4,(Int_t)ceil(yield_bcm_devices.size()/4.0));
    // Sign corrected
    // No error-rescaling
    // Utilize the slugwise main-det weighted data
    //p1=multiGraph(c1_4,p1,(TChain*)mini_slugs,k+1,Form("rcdb_slug:rcdb_sign*mini_BPMs_det_weighted.diff_%s_mean/(1.0e-6):mini_BPMs_det_weighted.diff_%s_mean_err/(1.0e-6)",devices12.at(k).Data(),devices12.at(k).Data()),cutGoodProduction.at(0),1);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_BCMs_det_weighted.#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_BCMs_det_weighted.#_#_mean_err/1.0e-9)";
    p1=manyGraph(c1_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,yield_bcm_devices,{},only_mod_cuts,{},1); // empty draws_piece3 and cuts and cuts2 vectors

    c1_4->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c1_4->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));

    data_file_cors.Close();
    data_file_asyms.Close();
  }
  if (draw_plots == 3){
    // Reproduced from section 2
    TFile data_file_slopes(Form("processed_respin2_data/CREX_All_%s_%s_Avg_Outputs%s.root",DataSetCut.Data(),averaging_timescale.Data(),suffix.Data()),"read");
    TFile data_file_corrections(Form("processed_respin2_data/CREX_All_%s_%s_Avg_Outputs_main_det_corrections.root",DataSetCut.Data(),averaging_timescale.Data()),"read"); // FIXME hardcoded here...

    TString run_cycle = "run_avg";
    TString run_cycle_wise = "runwise";
    //TString run_cycle = "cyclewise";//"run_avg";
    //TString run_cycle_wise = "cyclewise";//"runwise";
    TString scale = type;
    TString bpms = "_allbpms";
    TString nbpms = "5bpms";
    if (bpms == "_allbpms") { // default case is "" for 5bpm suffix
      nbpms = "allbpms";
    }
    TString part_ana = "_eigenvectors_double_sorted_part_avg"+bpms; // FIXME hardcoded the analysis here!!

    // Just use one of the available trees for rcdb etc.
    TTree* mini_slugs = (TTree*)data_file_corrections.Get("mini_raw_det_asyms_det_weighted");

    // Baseline asyms to use as weightings
    out_tree_mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted = (TTree*)data_file_corrections.Get("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted");
    mini_slugs->AddFriend(out_tree_mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted);

    TTree * out_tree_residuals_run_dit_dit_diff_det_weighted = (TTree*)data_file_slopes.Get("residuals_run_dit_dit_diff_det_weighted");
    TTree * out_tree_residuals_reg_dit_diff_det_weighted     = (TTree*)data_file_slopes.Get("residuals_reg_dit_diff_det_weighted");
    TTree * out_tree_residuals_lagr_dit_diff_det_weighted    = (TTree*)data_file_slopes.Get("residuals_lagr_dit_diff_det_weighted");
    TTree * out_tree_residuals_lagr_reg_diff_det_weighted    = (TTree*)data_file_slopes.Get("residuals_lagr_reg_diff_det_weighted");
    TTree * out_tree_residuals_avg_dit_res = (TTree*)data_file_slopes.Get("residuals_avg_dit_res");
    TTree * out_tree_residuals_reg_res     = (TTree*)data_file_slopes.Get("residuals_reg_res");
    TTree * out_tree_residuals_lagr_res    = (TTree*)data_file_slopes.Get("residuals_lagr_res");

    TTree * out_tree_slopes_run_dit        = (TTree*)data_file_slopes.Get("slopes_run_dit");
    TTree * out_tree_slopes_part_avg_dit   = (TTree*)data_file_slopes.Get("slopes_part_avg_dit");
    TTree * out_tree_slopes_part_avg_reg   = (TTree*)data_file_slopes.Get("slopes_part_avg_reg");
    TTree * out_tree_slopes_part_avg_lagr  = (TTree*)data_file_slopes.Get("slopes_part_avg_lagr");

    mini_slugs->AddFriend(out_tree_residuals_run_dit_dit_diff_det_weighted );
    mini_slugs->AddFriend(out_tree_residuals_reg_dit_diff_det_weighted     );
    mini_slugs->AddFriend(out_tree_residuals_lagr_dit_diff_det_weighted    );
    mini_slugs->AddFriend(out_tree_residuals_lagr_reg_diff_det_weighted    );
    mini_slugs->AddFriend(out_tree_residuals_avg_dit_res );
    mini_slugs->AddFriend(out_tree_residuals_reg_res     );
    mini_slugs->AddFriend(out_tree_residuals_lagr_res    );

    mini_slugs->AddFriend(out_tree_slopes_run_dit        );
    mini_slugs->AddFriend(out_tree_slopes_part_avg_dit   );
    mini_slugs->AddFriend(out_tree_slopes_part_avg_reg   );
    mini_slugs->AddFriend(out_tree_slopes_part_avg_lagr  );


    // Residuals
    // Reproduced from section 2
    std::vector <TString> monitors_choice;
    std::vector <TString> monitors_choice_new;

    Int_t nmons = 5;
    if ( bpms.Contains("allbpms") ) {
      nmons = 12;
      monitors_choice_new = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new","evMon5_new","evMon6_new","evMon7_new","evMon8_new","evMon9_new","evMon10_new","evMon11_new"};
      monitors_choice = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
    }
    else {
      monitors_choice_new = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new"};
      monitors_choice = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
    }

    TString corrections_pdfname = Form("processed_respin2_data/CREX_All_%s_%s_Avg_Outputs%s",DataSetCut.Data(),averaging_timescale.Data(),suffix.Data());
    TString corrections_pdfname_mod_cut_suffix = "_"+mod_cut;
    corrections_pdfname_mod_cut_suffix.ReplaceAll("&","").ReplaceAll("=","").ReplaceAll("|","").ReplaceAll(">","").ReplaceAll("<","").ReplaceAll(".","").ReplaceAll("!","").ReplaceAll(" ","");
    if (mod_cut == ""){
      corrections_pdfname_mod_cut_suffix = "";
    }

    TString ana = "Residuals";
    TText *label = new TText(0.0,0.005,ana.Data());
    label->SetTextFont(23);
    label->SetTextSize(12);
    label->SetNDC();

    std::ofstream outfile0;
    outfile0.open(Form("./processed_respin2_data/Results_%s_%s.csv",DataSetCut.Data(),averaging_timescale.Data()),std::ofstream::app);
    outfile0 << "Averaged outputs " << std::endl;
    outfile0 << "draw, "
             << "averaging, "
             << "draw_weighting_error, "
             << "cuts.at(icut), "
             << "avg_tmp1, "
             << "mean_err_tmp1,"
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

    // Slope differences (also want slopes by themselves and slope diffs / slope mean fraction, but doing a fraction is not possible with the current #_# looping technique.... would need 4 #'s at least or only zoom in to focus on 1 main_det...)
    corrections_pdfname = type + "_avgd_slope_diffs_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    TCanvas* c4_1_1 = new TCanvas();
    ana = "Run dit vs segment avgd dit slopes differences (local no weight, global maindet weight)";
    c4_1_1->cd();
    c4_1_1->SetTitle(ana.Data());
    c4_1_1->SetName(ana.Data());
    c4_1_1->Divide(special_detectors.size(),monitors_choice.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
    draw                 =     Form("residuals_run_dit_dit_diff_det_weighted.#_#_mean");
    drawn_channels_error = "abs(residuals_run_dit_dit_diff_det_weighted.#_#_mean_err)";
    p1=manyGraph(c4_1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,{},arm_cuts,{},1);
    c4_1_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c4_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));

    TCanvas* c4_1_2 = new TCanvas();
    ana = "Eigen reg vs segment avgd dit slopes differences (local no weight, global maindet weight)";
    c4_1_2->cd();
    c4_1_2->SetTitle(ana.Data());
    c4_1_2->SetName(ana.Data());
    c4_1_2->Divide(special_detectors.size(),monitors_choice.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
    draw                 =     Form("residuals_reg_dit_diff_det_weighted.#_#_mean");
    drawn_channels_error = "abs(residuals_reg_dit_diff_det_weighted.#_#_mean_err)";
    p1=manyGraph(c4_1_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,{},arm_cuts,{},1);
    c4_1_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c4_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c4_1_3 = new TCanvas();
    ana = "Eigen lagr vs segment avgd dit slopes differences (local no weight, global maindet weight)";
    c4_1_3->cd();
    c4_1_3->SetTitle(ana.Data());
    c4_1_3->SetName(ana.Data());
    c4_1_3->Divide(special_detectors.size(),monitors_choice.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
    draw                 =     Form("residuals_lagr_dit_diff_det_weighted.#_#_mean");
    drawn_channels_error = "abs(residuals_lagr_dit_diff_det_weighted.#_#_mean_err)";
    p1=manyGraph(c4_1_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,{},arm_cuts,{},1);
    c4_1_3->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c4_1_3->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c4_1_4 = new TCanvas();
    ana = "Eigen lagr vs eigen reg slopes differences (local no weight, global maindet weight)";
    c4_1_4->cd();
    c4_1_4->SetTitle(ana.Data());
    c4_1_4->SetName(ana.Data());
    c4_1_4->Divide(special_detectors.size(),monitors_choice.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
    draw                 =     Form("residuals_lagr_reg_diff_det_weighted.#_#_mean");
    drawn_channels_error = "abs(residuals_lagr_reg_diff_det_weighted.#_#_mean_err)";
    p1=manyGraph(c4_1_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,{},arm_cuts,{},1);
    c4_1_4->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c4_1_4->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));

    // Slopes themselves
    corrections_pdfname = type + "_avgd_slopes_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    TCanvas* c5_1_1 = new TCanvas();
    ana = "Eigen regression slopes (maindet weighted)";
    c5_1_1->cd();
    c5_1_1->SetTitle(ana.Data());
    c5_1_1->SetName(ana.Data());
    c5_1_1->Divide(special_detectors.size(),monitors_choice.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
    draw             =  Form("slopes_part_avg_reg.#_#_mean");
    drawn_channels_error = "abs(slopes_part_avg_reg.#_#_mean_err)";
    p1=manyGraph(c5_1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,{},arm_cuts,{},1);
    c5_1_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c5_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));

    TCanvas* c5_1_2 = new TCanvas();
    ana = "Eigen lagrange slopes (maindet weighted)";
    c5_1_2->cd();
    c5_1_2->SetTitle(ana.Data());
    c5_1_2->SetName(ana.Data());
    c5_1_2->Divide(special_detectors.size(),monitors_choice.size());
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
    draw             =  Form("slopes_part_avg_lagr.#_#_mean");
    drawn_channels_error = "abs(slopes_part_avg_lagr.#_#_mean_err)";
    p1=manyGraph(c5_1_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,{},arm_cuts,{},1);
    c5_1_2->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");

    if (!bpms.Contains("allbpms")){
      c5_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));


      TCanvas* c5_1_3 = new TCanvas();
      ana = "Runwise dithering slopes (maindet weighted)";
      c5_1_3->cd();
      c5_1_3->SetTitle(ana.Data());
      c5_1_3->SetName(ana.Data());
      c5_1_3->Divide(special_detectors.size(),monitors_choice.size());
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
      draw             =  Form("slopes_run_dit.#_#_mean");
      drawn_channels_error = "abs(slopes_run_dit.#_#_mean_err)";
      p1=manyGraph(c5_1_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,{},arm_cuts,{},1);
      c5_1_3->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      c5_1_3->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));

      TCanvas* c5_1_4 = new TCanvas();
      ana = "Part avg dithering slopes (maindet weighted)";
      c5_1_4->cd();
      c5_1_4->SetTitle(ana.Data());
      c5_1_4->SetName(ana.Data());
      c5_1_4->Divide(special_detectors.size(),monitors_choice.size());
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
      draw             =  Form("slopes_part_avg_dit.#_#_mean");
      drawn_channels_error = "abs(slopes_part_avg_dit.#_#_mean_err)";
      p1=manyGraph(c5_1_4,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,monitors_choice,{},arm_cuts,{},1);
      c5_1_4->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      c5_1_4->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
    }
    else {
      c5_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
    }


    // Residuals 
    corrections_pdfname = type + "_avgd_residuals_"+ averaging_timescale +"_wise" +corrections_pdfname_mod_cut_suffix;
    for (Int_t jcoil = 0 ; jcoil < coils.size() ; jcoil++) {
      std::vector<TString> tmpVec = {coils.at(jcoil)};
      TCanvas* c7_1_1 = new TCanvas();
      ana = "Eigen Segment avgd dit mean residuals for coil " + (TString)(jcoil+1) + " (local self weight, global maindet weight)";
      c7_1_1->cd();
      c7_1_1->SetTitle(ana.Data());
      c7_1_1->SetName(ana.Data());
      c7_1_1->Divide(1,special_detectors.size());
      //c7_1_1->Divide(special_detectors.size(),coils.size());
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
      //draw_weighting_error = "abs(residuals_avg_dit_res.#_#_mean_err)";
      draw                 =Form("residuals_avg_dit_res.#_#_mean_self");
      drawn_channels_error = "abs(residuals_avg_dit_res.#_#_mean_self_err)";
      p1=manyGraph(c7_1_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1);
      c7_1_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jcoil == 0) {
        c7_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf(",corrections_pdfname.Data()));
      }
      else {
        c7_1_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    for (Int_t jcoil = 0 ; jcoil < coils.size() ; jcoil++) {
      std::vector<TString> tmpVec = {coils.at(jcoil)};
      TCanvas* c7_2_1 = new TCanvas();
      ana = "Eigen Segment avgd dit residuals stddev for coil " + (TString)(jcoil+1) + " (local self weight, global maindet weight)";
      c7_2_1->cd();
      c7_2_1->SetTitle(ana.Data());
      c7_2_1->SetName(ana.Data());
      c7_2_1->Divide(1,special_detectors.size());
      //c7_2_1->Divide(special_detectors.size(),coils.size());
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
      //draw_weighting_error = "abs(residuals_avg_dit_res.#_#_mean_err)";
      draw                 =Form("residuals_avg_dit_res.#_#_stddev_self");
      drawn_channels_error = "abs(residuals_avg_dit_res.#_#_mean_self_err)";
      p1=manyGraph(c7_2_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1);
      c7_2_1->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      c7_2_1->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
    }

    for (Int_t jcoil = 0 ; jcoil < coils.size() ; jcoil++) {
      std::vector<TString> tmpVec = {coils.at(jcoil)};
      TCanvas* c7_1_2 = new TCanvas();
      ana = "Eigen Lagr mean residuals for coil " + (TString)(jcoil+1) + " (local self weight, global maindet weight)";
      c7_1_2->cd();
      c7_1_2->SetTitle(ana.Data());
      c7_1_2->SetName(ana.Data());
      c7_1_2->Divide(1,special_detectors.size());
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
      //draw_weighting_error = "abs(residuals_lagr_res.#_#_mean_err)";
      draw                 =Form("residuals_lagr_res.#_#_mean_self");
      drawn_channels_error = "abs(residuals_lagr_res.#_#_mean_self_err)";
      p1=manyGraph(c7_1_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1);
      c7_1_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      c7_1_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
    }

    for (Int_t jcoil = 0 ; jcoil < coils.size() ; jcoil++) {
      std::vector<TString> tmpVec = {coils.at(jcoil)};
      TCanvas* c7_2_2 = new TCanvas();
      ana = "Eigen Lagr residuals stddev for coil " + (TString)(jcoil+1) + " (local self weight, global maindet weight)";
      c7_2_2->cd();
      c7_2_2->SetTitle(ana.Data());
      c7_2_2->SetName(ana.Data());
      c7_2_2->Divide(1,special_detectors.size());
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
      //draw_weighting_error = "abs(residuals_lagr_res.#_#_mean_err)";
      draw                 =Form("residuals_lagr_res.#_#_stddev_self");
      drawn_channels_error = "abs(residuals_lagr_res.#_#_mean_self_err)";
      p1=manyGraph(c7_2_2,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1);
      c7_2_2->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      c7_2_2->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
    }

    for (Int_t jcoil = 0 ; jcoil < coils.size() ; jcoil++) {
      std::vector<TString> tmpVec = {coils.at(jcoil)};
      TCanvas* c7_1_3 = new TCanvas();
      ana = "Eigen reg mean residuals for coil " + (TString)(jcoil+1) + " (local self weight, global maindet weight)";
      c7_1_3->cd();
      c7_1_3->SetTitle(ana.Data());
      c7_1_3->SetName(ana.Data());
      c7_1_3->Divide(1,special_detectors.size());
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
      //draw_weighting_error = "abs(residuals_reg_res.#_#_mean_err)";
      draw                 =Form("residuals_reg_res.#_#_mean_self");
      drawn_channels_error = "abs(residuals_reg_res.#_#_mean_self_err)";
      p1=manyGraph(c7_1_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1);
      c7_1_3->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      c7_1_3->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
    }

    for (Int_t jcoil = 0 ; jcoil < coils.size() ; jcoil++) {
      std::vector<TString> tmpVec = {coils.at(jcoil)};
      TCanvas* c7_2_3 = new TCanvas();
      ana = "Eigen reg residuals stddev for coil " + (TString)(jcoil+1) + " (local self weight, global maindet weight)";
      c7_2_3->cd();
      c7_2_3->SetTitle(ana.Data());
      c7_2_3->SetName(ana.Data());
      c7_2_3->Divide(1,special_detectors.size());
      draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted.lagr_asym_usl_mean_err))";
      //draw_weighting_error = "abs(residuals_reg_res.#_#_mean_err)";
      draw                 =Form("residuals_reg_res.#_#_stddev_self");
      drawn_channels_error = "abs(residuals_reg_res.#_#_mean_self_err)";
      p1=manyGraph(c7_2_3,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,special_detectors,tmpVec,{},arm_cuts,{},1);
      c7_2_3->cd();
      label->SetText(0.0,0.005,ana.Data());
      label->Draw("same");
      if (jcoil == coils.size() -1 ) {
        c7_2_3->SaveAs(Form("processed_respin2_data/plots/%s.pdf)",corrections_pdfname.Data()));
      }
      else {
        c7_2_3->SaveAs(Form("processed_respin2_data/plots/%s.pdf",corrections_pdfname.Data()));
      }
    }

    data_file_slopes.Close();
    data_file_corrections.Close();
  }
}

void ToolBox::eigenvector_averaging(TString averaging_timescale = "crex_part") {
  TChain* mini = new TChain("mini");
  //TChain* mini_eigen_reg_allbpms_tr        = new TChain(Form("mini_eigen_reg_allbpms_tr"));     // Truncated one is not sorted... just deal with it I guess (could sort it later if urgently needed)
  TChain* mini_eigen_reg_5bpms_part_avg    = new TChain(Form("mini_eigen_reg_5bpms_part_avg")); // Unsorted versions have no _sorted in the name
  TChain* mini_reference_eigen_reg_5bpms_sorted      = new TChain(Form("mini_reference_eigen_reg_5bpms_sorted"));
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  TString infilename = Form("respin2_data/static_part_avg_eigenvector_reg_slopes.root");
  //TString infilename = Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted.root");
  mini->AddFile(infilename);
  //mini_eigen_reg_allbpms_tr->AddFile(infilename);
  mini_eigen_reg_5bpms_part_avg->AddFile(infilename);
  mini_reference_eigen_reg_5bpms_sorted->AddFile(infilename);
  // Default outputs, unsorted:
  //mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
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
  std::vector<TString> monitors5_new = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new"};
  std::vector<TString> monitors12_new = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new","evMon5_new","evMon6_new","evMon7_new","evMon8_new","evMon9_new","evMon10_new","evMon11_new"};
  std::vector<TString> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  std::vector<TString> monitors12 = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  std::vector<TString> devices12 = {
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
  //combo_segment_getter("dit_segment",mini,out_tree_mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr",cut,detectors,monitors12,devices12);

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
  TString mini_infilename = Form("respin2_data/rcdb_burstwise_eigenvectors_sorted_sorted.root");
  TFile mini_file(mini_infilename);
  mini_file.GetObject("mini",mini);

  TTree* mini_postpan_regression;
  TTree* mini_postpan_overload;

  TString postpan_regression_infilename = Form("respin2_data/regress_postpan_full_CREX.root");
  TString postpan_overload_infilename = Form("respin2_data/regress_overload_full_CREX.root");
  TFile postpan_regression_in_file(postpan_regression_infilename);
  TFile postpan_overload_in_file(postpan_overload_infilename);
  postpan_regression_in_file.GetObject("mini",mini_postpan_regression);
  postpan_overload_in_file.GetObject("mini",mini_postpan_overload);
  TString outFileName = Form("processed_respin2_data/mini_friended_5bpm_and_overload_postpan.root");
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
  // Generates mini-friended raw sensitivities data
  TTree* mini;
  TString mini_infilename = Form("respin2_data/rcdb_burstwise_eigenvectors_sorted_sorted.root");
  TFile mini_file(mini_infilename);
  mini_file.GetObject("mini",mini);

  TTree* dit_in_seg_avgd;
  TTree* dit_in_part_avgd;
  TTree* dit_in_part_b_avgd;
  TTree* dit_in_plain;

  TString dit_seg_avgd_infilename    = Form("respin2_data/eigen_seg_avg_dit_slopes_run_avg.root");
  TString dit_part_avgd_infilename   = Form("respin2_data/eigen_part_avg_dit_slopes_run_avg.root");
  TString dit_part_b_avgd_infilename = Form("respin2_data/eigen_part_b_avg_dit_slopes_run_avg.root");
  TString dit_plain_infilename       = Form("respin2_data/plain_dit_slopes_run_avg.root");

  TFile dit_seg_avgd_in_file(   dit_seg_avgd_infilename);
  TFile dit_part_avgd_in_file(  dit_part_avgd_infilename);
  TFile dit_part_b_avgd_in_file(dit_part_b_avgd_infilename);
  TFile dit_plain_in_file(      dit_plain_infilename);

  dit_seg_avgd_in_file.GetObject(   "dit",dit_in_seg_avgd);
  dit_part_avgd_in_file.GetObject(  "dit",dit_in_part_avgd);
  dit_part_b_avgd_in_file.GetObject("dit",dit_in_part_b_avgd);
  dit_plain_in_file.GetObject(      "dit",dit_in_plain);

  TString outFileName = Form("processed_respin2_data/mini_friended_run_avg_dit_sensitivities.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile* newfile = TFile::Open(outFileName, "recreate");
  newfile->cd();
  
  TTree* dit_out_seg_avgd    = dit_in_seg_avgd->CloneTree(0);
  TTree* dit_out_part_avgd   = dit_in_part_avgd->CloneTree(0);
  TTree* dit_out_part_b_avgd = dit_in_part_b_avgd->CloneTree(0);
  TTree* dit_out_plain       = dit_in_plain->CloneTree(0);
  dit_out_seg_avgd   ->SetName("dit_seg_avgd_friendable");
  dit_out_part_avgd  ->SetName("dit_part_avgd_friendable");
  dit_out_part_b_avgd->SetName("dit_part_b_avgd_friendable");
  dit_out_plain      ->SetName("dit_plain_friendable");
  TTree* dit_out_unfriended_seg_avgd    = dit_in_seg_avgd->CloneTree(0);
  TTree* dit_out_unfriended_part_avgd   = dit_in_part_avgd->CloneTree(0);
  TTree* dit_out_unfriended_part_b_avgd = dit_in_part_b_avgd->CloneTree(0);
  TTree* dit_out_unfriended_plain       = dit_in_plain->CloneTree(0);
  dit_out_unfriended_seg_avgd   ->SetName("dit_seg_avgd_unfriendable");
  dit_out_unfriended_part_avgd  ->SetName("dit_part_avgd_unfriendable");
  dit_out_unfriended_part_b_avgd->SetName("dit_part_b_avgd_unfriendable");
  dit_out_unfriended_plain      ->SetName("dit_plain_unfriendable");

  dit_copy_and_friend_to_mini(mini,dit_in_seg_avgd,    dit_out_seg_avgd,    dit_out_unfriended_seg_avgd);
  dit_copy_and_friend_to_mini(mini,dit_in_part_avgd,   dit_out_part_avgd,   dit_out_unfriended_part_avgd);
  dit_copy_and_friend_to_mini(mini,dit_in_part_b_avgd, dit_out_part_b_avgd, dit_out_unfriended_part_b_avgd);
  dit_copy_and_friend_to_mini(mini,dit_in_plain,       dit_out_plain,       dit_out_unfriended_plain);
  
  dit_out_seg_avgd->Write();
  dit_out_part_avgd->Write();
  dit_out_part_b_avgd->Write();
  dit_out_unfriended_seg_avgd->Write();
  dit_out_unfriended_part_avgd->Write();
  dit_out_unfriended_part_b_avgd->Write();
  dit_out_plain->Write();
  dit_out_unfriended_plain->Write();
  delete dit_out_seg_avgd;
  delete dit_out_part_avgd;
  delete dit_out_part_b_avgd;
  delete dit_out_unfriended_seg_avgd;
  delete dit_out_unfriended_part_avgd;
  delete dit_out_unfriended_part_b_avgd;
  delete dit_out_plain;
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
  TString mini_infilename = Form("respin2_data/rcdb_burstwise_eigenvectors_sorted_sorted.root");
  TFile mini_file(mini_infilename);
  mini_file.GetObject("mini",mini);

  TTree* dit_in_seg_avgd;
  TTree* dit_in_part_avgd;
  TTree* dit_in_part_avgd_allbpms;
  TTree* dit_in_part_b_avgd;
  TTree* dit_in_plain_cyclewise;
  TTree* dit_in_plain_run_avg;

  TString dit_infilename = Form("processed_respin2_data/All_dit_outputs_doing_segment_averaging_of_slopes.root");
  TFile dit_in_file(dit_infilename);
  dit_in_file.GetObject("dit_segmentwise_eigen_seg_avg_run_avg_segment_averaged",   dit_in_seg_avgd);
  dit_in_file.GetObject("dit_segmentwise_eigen_part_avg_run_avg_segment_averaged",  dit_in_part_avgd);
  dit_in_file.GetObject("dit_segmentwise_eigen_part_avg_allbpms_run_avg_segment_averaged",  dit_in_part_avgd_allbpms);
  dit_in_file.GetObject("dit_segmentwise_eigen_part_b_avg_run_avg_segment_averaged",dit_in_part_b_avgd);
  dit_in_file.GetObject("dit_segmentwise_plain_cyclewise_segment_averaged",         dit_in_plain_cyclewise);
  dit_in_file.GetObject("dit_segmentwise_plain_run_avg_segment_averaged",           dit_in_plain_run_avg);
  

  TString outFileName = Form("processed_respin2_data/All_dit_outputs_doing_segment_averaging_of_slopes_mini_friended.root");
  //TString outFileName = Form("mini_friended_sensitivities_dit.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile* newfile = TFile::Open(outFileName, "recreate");
  newfile->cd();
  
  TTree* dit_out_seg_avgd        = dit_in_seg_avgd->CloneTree(0);
  TTree* dit_out_part_avgd       = dit_in_part_avgd->CloneTree(0);
  TTree* dit_out_part_avgd_allbpms       = dit_in_part_avgd_allbpms->CloneTree(0);
  TTree* dit_out_part_b_avgd     = dit_in_part_b_avgd->CloneTree(0);
  TTree* dit_out_plain_cyclewise = dit_in_plain_cyclewise->CloneTree(0);
  TTree* dit_out_plain_run_avg   = dit_in_plain_run_avg->CloneTree(0);

  dit_out_seg_avgd        ->SetName("dit_seg_avgd_friendable");
  dit_out_part_avgd       ->SetName("dit_part_avgd_friendable");
  dit_out_part_avgd_allbpms       ->SetName("dit_part_avgd_allbpms_friendable");
  dit_out_part_b_avgd     ->SetName("dit_part_b_avgd_friendable");
  dit_out_plain_cyclewise ->SetName("dit_plain_cyclewise_friendable");
  dit_out_plain_run_avg   ->SetName("dit_plain_run_avg_friendable");

  dit_segmentwise_copy_and_friend_to_mini(mini,dit_in_seg_avgd,       dit_out_seg_avgd);
  dit_segmentwise_copy_and_friend_to_mini(mini,dit_in_part_avgd,      dit_out_part_avgd);
  dit_segmentwise_copy_and_friend_to_mini(mini,dit_in_part_avgd_allbpms,      dit_out_part_avgd_allbpms);
  dit_segmentwise_copy_and_friend_to_mini(mini,dit_in_part_b_avgd,    dit_out_part_b_avgd);
  dit_segmentwise_copy_and_friend_to_mini(mini,dit_in_plain_run_avg,  dit_out_plain_run_avg);
  dit_segmentwise_copy_and_friend_to_mini(mini,dit_in_plain_cyclewise,dit_out_plain_cyclewise);
  
  dit_out_seg_avgd        ->Write();
  dit_out_part_avgd       ->Write();
  dit_out_part_avgd_allbpms       ->Write();
  dit_out_part_b_avgd     ->Write();
  dit_out_plain_run_avg   ->Write();
  dit_out_plain_cyclewise ->Write();

  delete dit_out_seg_avgd;
  delete dit_out_part_avgd;
  delete dit_out_part_avgd_allbpms;
  delete dit_out_part_b_avgd;
  delete dit_out_plain_run_avg;
  delete dit_out_plain_cyclewise;
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
  TString mini_infilename = Form("respin2_data/rcdb_burstwise_eigenvectors_sorted_sorted.root");
  TFile mini_file(mini_infilename);
  mini_file.GetObject("mini",mini);
  mini->BuildIndex("run","mini");

  TTree* agg_in_ErrorFlag_part_avgd  ;
  TTree* agg_in_ErrorFlag_part_avgd_allbpms;
  TTree* agg_in_ErrorFlag_part_b_avgd;
  TTree* agg_in_ErrorFlag_seg_avgd   ;
  TTree* agg_in_IncludeBMOD_part_avgd;
  TTree* agg_in_OnlyBMOD_part_avgd   ;
  //TTree* agg_in_IncludeBMOD_part_avgd_allbpms;
  //TTree* agg_in_OnlyBMOD_part_avgd_allbpms   ;
  TTree* agg_in_ErrorFlag_part_avgd_no_err  ;
  TTree* agg_in_ErrorFlag_part_avgd_allbpms_no_err;
  TTree* agg_in_ErrorFlag_part_b_avgd_no_err;
  TTree* agg_in_ErrorFlag_seg_avgd_no_err   ;
  TTree* agg_in_IncludeBMOD_part_avgd_no_err;
  TTree* agg_in_OnlyBMOD_part_avgd_no_err   ;
  TTree* agg_in_IncludeBMOD_part_avgd_allbpms_no_err;
  TTree* agg_in_OnlyBMOD_part_avgd_allbpms_no_err   ;
  TString agg_ErrorFlag_part_avgd_infilename = Form("respin2_data/aggregator_from_ErrorFlag_part_avgd_drl_correction.root");
  TString agg_ErrorFlag_part_avgd_allbpms_infilename = Form("respin2_data/aggregator_from_ErrorFlag_part_avgd_allbpms_drl_correction.root");
  TString agg_ErrorFlag_part_b_avgd_infilename = Form("respin2_data/aggregator_from_ErrorFlag_part_b_avgd_drl_correction.root");
  TString agg_ErrorFlag_seg_avgd_infilename = Form("respin2_data/aggregator_from_ErrorFlag_seg_avgd_drl_correction.root");
  TString agg_IncludeBMOD_part_avgd_infilename = Form("respin2_data/aggregator_from_IncludeBMOD_part_avgd_drl_correction.root");
  TString agg_OnlyBMOD_part_avgd_infilename = Form("respin2_data/aggregator_from_OnlyBMOD_part_avgd_drl_correction.root");
  //TString agg_IncludeBMOD_part_avgd_allbpms_infilename = Form("respin2_data/aggregator_from_IncludeBMOD_part_avgd_allbpms_drl_correction.root");
  //TString agg_OnlyBMOD_part_avgd_allbpms_infilename = Form("respin2_data/aggregator_from_OnlyBMOD_part_avgd_allbpms_drl_correction.root");

  TString agg_ErrorFlag_part_avgd_no_err_infilename = Form("respin2_data/aggregator_from_ErrorFlag_part_avgd_drl_no_err_correction.root");
  TString agg_ErrorFlag_part_avgd_allbpms_no_err_infilename = Form("respin2_data/aggregator_from_ErrorFlag_part_avgd_allbpms_drl_no_err_correction.root");
  TString agg_ErrorFlag_part_b_avgd_no_err_infilename = Form("respin2_data/aggregator_from_ErrorFlag_part_b_avgd_drl_no_err_correction.root");
  TString agg_ErrorFlag_seg_avgd_no_err_infilename = Form("respin2_data/aggregator_from_ErrorFlag_seg_avgd_drl_no_err_correction.root");
  TString agg_IncludeBMOD_part_avgd_no_err_infilename = Form("respin2_data/aggregator_from_IncludeBMOD_part_avgd_drl_no_err_correction.root");
  TString agg_OnlyBMOD_part_avgd_no_err_infilename = Form("respin2_data/aggregator_from_OnlyBMOD_part_avgd_drl_no_err_correction.root");
  TString agg_IncludeBMOD_part_avgd_allbpms_no_err_infilename = Form("respin2_data/aggregator_from_IncludeBMOD_part_avgd_allbpms_drl_no_err_correction.root");
  TString agg_OnlyBMOD_part_avgd_allbpms_no_err_infilename = Form("respin2_data/aggregator_from_OnlyBMOD_part_avgd_allbpms_drl_no_err_correction.root");

  TFile agg_ErrorFlag_part_avgd_in_file(agg_ErrorFlag_part_avgd_infilename);
  TFile agg_ErrorFlag_part_avgd_allbpms_in_file(agg_ErrorFlag_part_avgd_allbpms_infilename);
  TFile agg_ErrorFlag_part_b_avgd_in_file(agg_ErrorFlag_part_b_avgd_infilename);
  TFile agg_ErrorFlag_seg_avgd_in_file(agg_ErrorFlag_seg_avgd_infilename);
  TFile agg_IncludeBMOD_part_avgd_in_file(agg_IncludeBMOD_part_avgd_infilename);
  TFile agg_OnlyBMOD_part_avgd_in_file(agg_OnlyBMOD_part_avgd_infilename);
  //TFile agg_IncludeBMOD_part_avgd_allbpms_in_file(agg_IncludeBMOD_part_avgd_allbpms_infilename);
  //TFile agg_OnlyBMOD_part_avgd_allbpms_in_file(agg_OnlyBMOD_part_avgd_allbpms_infilename);

  TFile agg_ErrorFlag_part_avgd_no_err_in_file(agg_ErrorFlag_part_avgd_no_err_infilename);
  TFile agg_ErrorFlag_part_avgd_allbpms_no_err_in_file(agg_ErrorFlag_part_avgd_allbpms_no_err_infilename);
  TFile agg_ErrorFlag_part_b_avgd_no_err_in_file(agg_ErrorFlag_part_b_avgd_no_err_infilename);
  TFile agg_ErrorFlag_seg_avgd_no_err_in_file(agg_ErrorFlag_seg_avgd_no_err_infilename);
  TFile agg_IncludeBMOD_part_avgd_no_err_in_file(agg_IncludeBMOD_part_avgd_no_err_infilename);
  TFile agg_OnlyBMOD_part_avgd_no_err_in_file(agg_OnlyBMOD_part_avgd_no_err_infilename);
  TFile agg_IncludeBMOD_part_avgd_allbpms_no_err_in_file(agg_IncludeBMOD_part_avgd_allbpms_no_err_infilename);
  TFile agg_OnlyBMOD_part_avgd_allbpms_no_err_in_file(agg_OnlyBMOD_part_avgd_allbpms_no_err_infilename);

  agg_ErrorFlag_part_avgd_in_file.GetObject("agg",agg_in_ErrorFlag_part_avgd);
  agg_ErrorFlag_part_avgd_allbpms_in_file.GetObject("agg",agg_in_ErrorFlag_part_avgd_allbpms);
  agg_ErrorFlag_part_b_avgd_in_file.GetObject("agg",agg_in_ErrorFlag_part_b_avgd);
  agg_ErrorFlag_seg_avgd_in_file.GetObject("agg",agg_in_ErrorFlag_seg_avgd);
  agg_IncludeBMOD_part_avgd_in_file.GetObject("agg",agg_in_IncludeBMOD_part_avgd);
  agg_OnlyBMOD_part_avgd_in_file.GetObject("agg",agg_in_OnlyBMOD_part_avgd);
  //agg_IncludeBMOD_part_avgd_allbpms_in_file.GetObject("agg",agg_in_IncludeBMOD_part_avgd_allbpms);
  //agg_OnlyBMOD_part_avgd_allbpms_in_file.GetObject("agg",agg_in_OnlyBMOD_part_avgd_allbpms);

  agg_ErrorFlag_part_avgd_no_err_in_file.GetObject("agg",agg_in_ErrorFlag_part_avgd_no_err);
  agg_ErrorFlag_part_avgd_allbpms_no_err_in_file.GetObject("agg",agg_in_ErrorFlag_part_avgd_allbpms_no_err);
  agg_ErrorFlag_part_b_avgd_no_err_in_file.GetObject("agg",agg_in_ErrorFlag_part_b_avgd_no_err);
  agg_ErrorFlag_seg_avgd_no_err_in_file.GetObject("agg",agg_in_ErrorFlag_seg_avgd_no_err);
  agg_IncludeBMOD_part_avgd_no_err_in_file.GetObject("agg",agg_in_IncludeBMOD_part_avgd_no_err);
  agg_OnlyBMOD_part_avgd_no_err_in_file.GetObject("agg",agg_in_OnlyBMOD_part_avgd_no_err);
  agg_IncludeBMOD_part_avgd_allbpms_no_err_in_file.GetObject("agg",agg_in_IncludeBMOD_part_avgd_allbpms_no_err);
  agg_OnlyBMOD_part_avgd_allbpms_no_err_in_file.GetObject("agg",agg_in_OnlyBMOD_part_avgd_allbpms_no_err);

  TTree* agg_in_plain;
  TString agg_plain_infilename = Form("respin2_data/aggregator_from_japan.root");
  TFile agg_plain_in_file(agg_plain_infilename);
  agg_plain_in_file.GetObject("agg",agg_in_plain);

  TString outFileName = Form("processed_respin2_data/mini_friended_agg.root");
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile* newfile = TFile::Open(outFileName, "recreate");
  newfile->cd();
  
  TTree* agg_out_ErrorFlag_part_avgd   = agg_in_ErrorFlag_part_avgd  ->CloneTree(0);
  TTree* agg_out_ErrorFlag_part_avgd_allbpms   = agg_in_ErrorFlag_part_avgd_allbpms  ->CloneTree(0);
  TTree* agg_out_ErrorFlag_part_b_avgd = agg_in_ErrorFlag_part_b_avgd->CloneTree(0);
  TTree* agg_out_ErrorFlag_seg_avgd    = agg_in_ErrorFlag_seg_avgd   ->CloneTree(0);
  TTree* agg_out_IncludeBMOD_part_avgd = agg_in_IncludeBMOD_part_avgd->CloneTree(0);
  TTree* agg_out_OnlyBMOD_part_avgd    = agg_in_OnlyBMOD_part_avgd   ->CloneTree(0);
  //TTree* agg_out_IncludeBMOD_part_avgd_allbpms = agg_in_IncludeBMOD_part_avgd_allbpms->CloneTree(0);
  //TTree* agg_out_OnlyBMOD_part_avgd_allbpms    = agg_in_OnlyBMOD_part_avgd_allbpms   ->CloneTree(0);
  TTree* agg_out_ErrorFlag_part_avgd_no_err   = agg_in_ErrorFlag_part_avgd_no_err  ->CloneTree(0);
  TTree* agg_out_ErrorFlag_part_avgd_allbpms_no_err   = agg_in_ErrorFlag_part_avgd_allbpms_no_err  ->CloneTree(0);
  TTree* agg_out_ErrorFlag_part_b_avgd_no_err = agg_in_ErrorFlag_part_b_avgd_no_err->CloneTree(0);
  TTree* agg_out_ErrorFlag_seg_avgd_no_err    = agg_in_ErrorFlag_seg_avgd_no_err   ->CloneTree(0);
  TTree* agg_out_IncludeBMOD_part_avgd_no_err = agg_in_IncludeBMOD_part_avgd_no_err->CloneTree(0);
  TTree* agg_out_OnlyBMOD_part_avgd_no_err    = agg_in_OnlyBMOD_part_avgd_no_err   ->CloneTree(0);
  TTree* agg_out_IncludeBMOD_part_avgd_allbpms_no_err = agg_in_IncludeBMOD_part_avgd_allbpms_no_err->CloneTree(0);
  TTree* agg_out_OnlyBMOD_part_avgd_allbpms_no_err    = agg_in_OnlyBMOD_part_avgd_allbpms_no_err   ->CloneTree(0);
  agg_out_ErrorFlag_part_avgd  ->SetName("agg_ErrorFlag_part_avgd_friendable");
  agg_out_ErrorFlag_part_avgd_allbpms  ->SetName("agg_ErrorFlag_part_avgd_allbpms_friendable");
  agg_out_ErrorFlag_part_b_avgd->SetName("agg_ErrorFlag_part_b_avgd_friendable");
  agg_out_ErrorFlag_seg_avgd   ->SetName("agg_ErrorFlag_seg_avgd_friendable");
  agg_out_IncludeBMOD_part_avgd->SetName("agg_IncludeBMOD_part_avgd_friendable");
  agg_out_OnlyBMOD_part_avgd   ->SetName("agg_OnlyBMOD_part_avgd_friendable");
  //agg_out_IncludeBMOD_part_avgd_allbpms->SetName("agg_IncludeBMOD_part_avgd_allbpms_friendable");
  //agg_out_OnlyBMOD_part_avgd_allbpms   ->SetName("agg_OnlyBMOD_part_avgd_allbpms_friendable");
  agg_out_ErrorFlag_part_avgd_no_err  ->SetName("agg_ErrorFlag_part_avgd_no_err_friendable");
  agg_out_ErrorFlag_part_avgd_allbpms_no_err  ->SetName("agg_ErrorFlag_part_avgd_allbpms_no_err_friendable");
  agg_out_ErrorFlag_part_b_avgd_no_err->SetName("agg_ErrorFlag_part_b_avgd_no_err_friendable");
  agg_out_ErrorFlag_seg_avgd_no_err   ->SetName("agg_ErrorFlag_seg_avgd_no_err_friendable");
  agg_out_IncludeBMOD_part_avgd_no_err->SetName("agg_IncludeBMOD_part_avgd_no_err_friendable");
  agg_out_OnlyBMOD_part_avgd_no_err   ->SetName("agg_OnlyBMOD_part_avgd_no_err_friendable");
  agg_out_IncludeBMOD_part_avgd_allbpms_no_err->SetName("agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable");
  agg_out_OnlyBMOD_part_avgd_allbpms_no_err   ->SetName("agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable");
  TTree* agg_unfriended_ErrorFlag_part_avgd   = (TTree*)agg_in_ErrorFlag_part_avgd  ->CloneTree(0);
  TTree* agg_unfriended_ErrorFlag_part_avgd_allbpms   = (TTree*)agg_in_ErrorFlag_part_avgd_allbpms  ->CloneTree(0);
  TTree* agg_unfriended_ErrorFlag_part_b_avgd = (TTree*)agg_in_ErrorFlag_part_b_avgd->CloneTree(0);
  TTree* agg_unfriended_ErrorFlag_seg_avgd    = (TTree*)agg_in_ErrorFlag_seg_avgd   ->CloneTree(0);
  TTree* agg_unfriended_IncludeBMOD_part_avgd = (TTree*)agg_in_IncludeBMOD_part_avgd->CloneTree(0);
  TTree* agg_unfriended_OnlyBMOD_part_avgd    = (TTree*)agg_in_OnlyBMOD_part_avgd   ->CloneTree(0);
  //TTree* agg_unfriended_IncludeBMOD_part_avgd_allbpms = (TTree*)agg_in_IncludeBMOD_part_avgd_allbpms->CloneTree(0);
  //TTree* agg_unfriended_OnlyBMOD_part_avgd_allbpms    = (TTree*)agg_in_OnlyBMOD_part_avgd_allbpms   ->CloneTree(0);
  TTree* agg_unfriended_ErrorFlag_part_avgd_no_err   = (TTree*)agg_in_ErrorFlag_part_avgd_no_err  ->CloneTree(0);
  TTree* agg_unfriended_ErrorFlag_part_avgd_allbpms_no_err   = (TTree*)agg_in_ErrorFlag_part_avgd_allbpms_no_err  ->CloneTree(0);
  TTree* agg_unfriended_ErrorFlag_part_b_avgd_no_err = (TTree*)agg_in_ErrorFlag_part_b_avgd_no_err->CloneTree(0);
  TTree* agg_unfriended_ErrorFlag_seg_avgd_no_err    = (TTree*)agg_in_ErrorFlag_seg_avgd_no_err   ->CloneTree(0);
  TTree* agg_unfriended_IncludeBMOD_part_avgd_no_err = (TTree*)agg_in_IncludeBMOD_part_avgd_no_err->CloneTree(0);
  TTree* agg_unfriended_OnlyBMOD_part_avgd_no_err    = (TTree*)agg_in_OnlyBMOD_part_avgd_no_err   ->CloneTree(0);
  TTree* agg_unfriended_IncludeBMOD_part_avgd_allbpms_no_err = (TTree*)agg_in_IncludeBMOD_part_avgd_allbpms_no_err->CloneTree(0);
  TTree* agg_unfriended_OnlyBMOD_part_avgd_allbpms_no_err    = (TTree*)agg_in_OnlyBMOD_part_avgd_allbpms_no_err   ->CloneTree(0);
  agg_unfriended_ErrorFlag_part_avgd  ->SetName("agg_ErrorFlag_part_avgd_unfriended");
  agg_unfriended_ErrorFlag_part_avgd_allbpms  ->SetName("agg_ErrorFlag_part_avgd_allbpms_unfriended");
  agg_unfriended_ErrorFlag_part_b_avgd->SetName("agg_ErrorFlag_part_b_avgd_unfriended");
  agg_unfriended_ErrorFlag_seg_avgd   ->SetName("agg_ErrorFlag_seg_avgd_unfriended");
  agg_unfriended_IncludeBMOD_part_avgd->SetName("agg_IncludeBMOD_part_avgd_unfriended");
  agg_unfriended_OnlyBMOD_part_avgd   ->SetName("agg_OnlyBMOD_part_avgd_unfriended");
  //agg_unfriended_IncludeBMOD_part_avgd_allbpms->SetName("agg_IncludeBMOD_part_avgd_allbpms_unfriended");
  //agg_unfriended_OnlyBMOD_part_avgd_allbpms   ->SetName("agg_OnlyBMOD_part_avgd_allbpms_unfriended");
  agg_unfriended_ErrorFlag_part_avgd_no_err  ->SetName("agg_ErrorFlag_part_avgd_no_err_unfriended");
  agg_unfriended_ErrorFlag_part_avgd_allbpms_no_err  ->SetName("agg_ErrorFlag_part_avgd_allbpms_no_err_unfriended");
  agg_unfriended_ErrorFlag_part_b_avgd_no_err->SetName("agg_ErrorFlag_part_b_avgd_no_err_unfriended");
  agg_unfriended_ErrorFlag_seg_avgd_no_err   ->SetName("agg_ErrorFlag_seg_avgd_no_err_unfriended");
  agg_unfriended_IncludeBMOD_part_avgd_no_err->SetName("agg_IncludeBMOD_part_avgd_no_err_unfriended");
  agg_unfriended_OnlyBMOD_part_avgd_no_err   ->SetName("agg_OnlyBMOD_part_avgd_no_err_unfriended");
  agg_unfriended_IncludeBMOD_part_avgd_allbpms_no_err->SetName("agg_IncludeBMOD_part_avgd_allbpms_no_err_unfriended");
  agg_unfriended_OnlyBMOD_part_avgd_allbpms_no_err   ->SetName("agg_OnlyBMOD_part_avgd_allbpms_no_err_unfriended");

  TTree* agg_out_plain = agg_in_plain->CloneTree(0);
  agg_out_plain->SetName("agg_plain_friendable");
  TTree* agg_unfriended_plain = (TTree*)agg_in_plain->CloneTree(0);
  agg_unfriended_plain->SetName("agg_plain_unfriended");

  aggregator_friend_to_mini(mini,agg_in_ErrorFlag_part_avgd  ,agg_out_ErrorFlag_part_avgd  ,agg_unfriended_ErrorFlag_part_avgd  );
  aggregator_friend_to_mini(mini,agg_in_ErrorFlag_part_avgd_allbpms  ,agg_out_ErrorFlag_part_avgd_allbpms  ,agg_unfriended_ErrorFlag_part_avgd_allbpms  );
  aggregator_friend_to_mini(mini,agg_in_ErrorFlag_part_b_avgd,agg_out_ErrorFlag_part_b_avgd,agg_unfriended_ErrorFlag_part_b_avgd);
  aggregator_friend_to_mini(mini,agg_in_ErrorFlag_seg_avgd   ,agg_out_ErrorFlag_seg_avgd   ,agg_unfriended_ErrorFlag_seg_avgd   );
  aggregator_friend_to_mini(mini,agg_in_IncludeBMOD_part_avgd,agg_out_IncludeBMOD_part_avgd,agg_unfriended_IncludeBMOD_part_avgd);
  aggregator_friend_to_mini(mini,agg_in_OnlyBMOD_part_avgd   ,agg_out_OnlyBMOD_part_avgd   ,agg_unfriended_OnlyBMOD_part_avgd   );
  //aggregator_friend_to_mini(mini,agg_in_IncludeBMOD_part_avgd_allbpms,agg_out_IncludeBMOD_part_avgd_allbpms,agg_unfriended_IncludeBMOD_part_avgd_allbpms);
  //aggregator_friend_to_mini(mini,agg_in_OnlyBMOD_part_avgd_allbpms   ,agg_out_OnlyBMOD_part_avgd_allbpms   ,agg_unfriended_OnlyBMOD_part_avgd_allbpms   );
  aggregator_friend_to_mini(mini,agg_in_ErrorFlag_part_avgd_no_err  ,agg_out_ErrorFlag_part_avgd_no_err  ,agg_unfriended_ErrorFlag_part_avgd_no_err  );
  aggregator_friend_to_mini(mini,agg_in_ErrorFlag_part_avgd_allbpms_no_err  ,agg_out_ErrorFlag_part_avgd_allbpms_no_err  ,agg_unfriended_ErrorFlag_part_avgd_allbpms_no_err  );
  aggregator_friend_to_mini(mini,agg_in_ErrorFlag_part_b_avgd_no_err,agg_out_ErrorFlag_part_b_avgd_no_err,agg_unfriended_ErrorFlag_part_b_avgd_no_err);
  aggregator_friend_to_mini(mini,agg_in_ErrorFlag_seg_avgd_no_err   ,agg_out_ErrorFlag_seg_avgd_no_err   ,agg_unfriended_ErrorFlag_seg_avgd_no_err   );
  aggregator_friend_to_mini(mini,agg_in_IncludeBMOD_part_avgd_no_err,agg_out_IncludeBMOD_part_avgd_no_err,agg_unfriended_IncludeBMOD_part_avgd_no_err);
  aggregator_friend_to_mini(mini,agg_in_OnlyBMOD_part_avgd_no_err   ,agg_out_OnlyBMOD_part_avgd_no_err   ,agg_unfriended_OnlyBMOD_part_avgd_no_err   );
  aggregator_friend_to_mini(mini,agg_in_IncludeBMOD_part_avgd_allbpms_no_err,agg_out_IncludeBMOD_part_avgd_allbpms_no_err,agg_unfriended_IncludeBMOD_part_avgd_allbpms_no_err);
  aggregator_friend_to_mini(mini,agg_in_OnlyBMOD_part_avgd_allbpms_no_err   ,agg_out_OnlyBMOD_part_avgd_allbpms_no_err   ,agg_unfriended_OnlyBMOD_part_avgd_allbpms_no_err   );
  aggregator_friend_to_mini(mini,agg_in_plain,agg_out_plain,agg_unfriended_plain);

  agg_out_ErrorFlag_part_avgd  ->Write("",TObject::kOverwrite);
  agg_out_ErrorFlag_part_avgd_allbpms  ->Write("",TObject::kOverwrite);
  agg_out_ErrorFlag_part_b_avgd->Write("",TObject::kOverwrite);
  agg_out_ErrorFlag_seg_avgd   ->Write("",TObject::kOverwrite);
  agg_out_IncludeBMOD_part_avgd->Write("",TObject::kOverwrite);
  agg_out_OnlyBMOD_part_avgd   ->Write("",TObject::kOverwrite);
  //agg_out_IncludeBMOD_part_avgd_allbpms->Write("",TObject::kOverwrite);
  //agg_out_OnlyBMOD_part_avgd_allbpms   ->Write("",TObject::kOverwrite);
  agg_out_ErrorFlag_part_avgd_no_err  ->Write("",TObject::kOverwrite);
  agg_out_ErrorFlag_part_avgd_allbpms_no_err  ->Write("",TObject::kOverwrite);
  agg_out_ErrorFlag_part_b_avgd_no_err->Write("",TObject::kOverwrite);
  agg_out_ErrorFlag_seg_avgd_no_err   ->Write("",TObject::kOverwrite);
  agg_out_IncludeBMOD_part_avgd_no_err->Write("",TObject::kOverwrite);
  agg_out_OnlyBMOD_part_avgd_no_err   ->Write("",TObject::kOverwrite);
  agg_out_IncludeBMOD_part_avgd_allbpms_no_err->Write("",TObject::kOverwrite);
  agg_out_OnlyBMOD_part_avgd_allbpms_no_err   ->Write("",TObject::kOverwrite);
  delete agg_out_ErrorFlag_part_avgd  ;
  delete agg_out_ErrorFlag_part_avgd_allbpms  ;
  delete agg_out_ErrorFlag_part_b_avgd;
  delete agg_out_ErrorFlag_seg_avgd   ;
  delete agg_out_IncludeBMOD_part_avgd;
  delete agg_out_OnlyBMOD_part_avgd   ;
  //delete agg_out_IncludeBMOD_part_avgd_allbpms;
  //delete agg_out_OnlyBMOD_part_avgd_allbpms   ;
  delete agg_out_ErrorFlag_part_avgd_no_err  ;
  delete agg_out_ErrorFlag_part_avgd_allbpms_no_err  ;
  delete agg_out_ErrorFlag_part_b_avgd_no_err;
  delete agg_out_ErrorFlag_seg_avgd_no_err   ;
  delete agg_out_IncludeBMOD_part_avgd_no_err;
  delete agg_out_OnlyBMOD_part_avgd_no_err   ;
  delete agg_out_IncludeBMOD_part_avgd_allbpms_no_err;
  delete agg_out_OnlyBMOD_part_avgd_allbpms_no_err   ;
  agg_unfriended_ErrorFlag_part_avgd  ->Write("",TObject::kOverwrite);
  agg_unfriended_ErrorFlag_part_avgd_allbpms  ->Write("",TObject::kOverwrite);
  agg_unfriended_ErrorFlag_part_b_avgd->Write("",TObject::kOverwrite);
  agg_unfriended_ErrorFlag_seg_avgd   ->Write("",TObject::kOverwrite);
  agg_unfriended_IncludeBMOD_part_avgd->Write("",TObject::kOverwrite);
  agg_unfriended_OnlyBMOD_part_avgd   ->Write("",TObject::kOverwrite);
  //agg_unfriended_IncludeBMOD_part_avgd_allbpms->Write("",TObject::kOverwrite);
  //agg_unfriended_OnlyBMOD_part_avgd_allbpms   ->Write("",TObject::kOverwrite);
  agg_unfriended_ErrorFlag_part_avgd_no_err  ->Write("",TObject::kOverwrite);
  agg_unfriended_ErrorFlag_part_avgd_allbpms_no_err  ->Write("",TObject::kOverwrite);
  agg_unfriended_ErrorFlag_part_b_avgd_no_err->Write("",TObject::kOverwrite);
  agg_unfriended_ErrorFlag_seg_avgd_no_err   ->Write("",TObject::kOverwrite);
  agg_unfriended_IncludeBMOD_part_avgd_no_err->Write("",TObject::kOverwrite);
  agg_unfriended_OnlyBMOD_part_avgd_no_err   ->Write("",TObject::kOverwrite);
  agg_unfriended_IncludeBMOD_part_avgd_allbpms_no_err->Write("",TObject::kOverwrite);
  agg_unfriended_OnlyBMOD_part_avgd_allbpms_no_err   ->Write("",TObject::kOverwrite);
  delete agg_unfriended_ErrorFlag_part_avgd  ;
  delete agg_unfriended_ErrorFlag_part_avgd_allbpms  ;
  delete agg_unfriended_ErrorFlag_part_b_avgd;
  delete agg_unfriended_ErrorFlag_seg_avgd   ;
  delete agg_unfriended_IncludeBMOD_part_avgd;
  delete agg_unfriended_OnlyBMOD_part_avgd   ;
  //delete agg_unfriended_IncludeBMOD_part_avgd_allbpms;
  //delete agg_unfriended_OnlyBMOD_part_avgd_allbpms   ;
  delete agg_unfriended_ErrorFlag_part_avgd_no_err  ;
  delete agg_unfriended_ErrorFlag_part_avgd_allbpms_no_err  ;
  delete agg_unfriended_ErrorFlag_part_b_avgd_no_err;
  delete agg_unfriended_ErrorFlag_seg_avgd_no_err   ;
  delete agg_unfriended_IncludeBMOD_part_avgd_no_err;
  delete agg_unfriended_OnlyBMOD_part_avgd_no_err   ;
  delete agg_unfriended_IncludeBMOD_part_avgd_allbpms_no_err;
  delete agg_unfriended_OnlyBMOD_part_avgd_allbpms_no_err   ;

  agg_out_plain->Write("",TObject::kOverwrite);
  delete agg_out_plain;
  agg_unfriended_plain->Write("",TObject::kOverwrite);
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


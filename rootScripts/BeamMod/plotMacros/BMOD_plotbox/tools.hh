
void ToolBox::tg_err_averaging(TString averaging_timescale = "crex_part", Int_t draw_plots = 0, TString mod_draw = "", TString mod_cut = "", TString type = "part", TString do_err = "", TString suffix = "") {
  TChain* mini                                  = new TChain("mini");
  TChain* agg_part_avgd_friendable              = new TChain(Form("agg_ErrorFlag_%s_avgd%s_friendable",type.Data(),do_err.Data()));
  TChain* agg_part_avgd_friendable_IncludeBMOD  = new TChain(Form("agg_IncludeBMOD_%s_avgd%s_friendable",type.Data(),do_err.Data()));
  TChain* agg_part_avgd_friendable_OnlyBMOD     = new TChain(Form("agg_OnlyBMOD_%s_avgd%s_friendable",type.Data(),do_err.Data()));
  // evMons stuff mostly unused it seems.... those plots should just be made separately in the obvious places
  // unused sensitivities.... replace with friended stuff ---> Replace with outputs in workspace_plotting_macros/july_21_plots/residuals_*.root (move to processed respin2) ---> Do those in a separate list of TChain reads and such at the end of this massive function... for sanity purposes and reproducibility
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  TString mini_do_err = "";
  if (do_err == "_no_err") { 
    mini_do_err = "_noerr";
  }
  TString mini_infilename = Form("../respin2_data/eigen%s_%s_avg_lagr_slopes.root",mini_do_err.Data(),type.Data()); //eigen_noerr_.... other options ; do_err="_noerr" for no err version lagrange sens calculated data
  TString bigType = "Part";
  if (type == "part_b"){
    bigType = "Part_B";
  }
  if (type == "Seg"){
    bigType = "Segment";
  }
  TString agg_infilename = Form("../processed_respin2_data/mini_friended_agg.root");
  mini->AddFile(mini_infilename);
  agg_part_avgd_friendable->AddFile(agg_infilename);
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->BuildIndex("run","mini");



  // Cuts and such
  
  std::vector<TString> AT_special_detectors = {
    "usl", "usr", "us_avg", "us_dd", "atl_dd","atr_dd", "atl_dd_atr_dd_avg", "main_det"  // used to comment out // "main_det", "us_dd"
  };
  std::vector<TString> special_detectors = {
    "usl", "usr", "us_avg", "us_dd" // used to comment out // "main_det", "us_dd"
  };
  std::vector<TString> null_cuts = {};
  //FIXME it would be nice to do cuts = Form("some cut from the user in plotMacros.C && this hard coded cut here")
  std::vector<TString> cuts = {
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 1)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag != 2)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
    "(rcdb_run_type == 1 && rcdb_run_flag == 1 && rcdb_arm_flag == 0)"+mod_cut,
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
  std::vector<TString> lagr_cor_asym_vec = {"lagr_asym","cor_asym"};
  std::vector<TString> reg_asym_vec = {"reg_asym"};
  std::vector<TString> cor_asym_vec = {"cor_asym"};
  std::vector<TString> dit_asym_vec = {"dit_asym"};
  std::vector<TString> asym_yield_vec = {"asym","yield"};
  //std::vector<TString> monitors5 = {"evMon0"};
  std::vector<TString> monitors5_new = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new"};
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
  TString outFileName = Form("output_data/CREX_All_%s_Avg_Outputs%s.root",averaging_timescale.Data(),suffix.Data()); /// FIXME no residuals generally...
  if (draw_plots == 1) {
    outFileName = "TEST_junk.root";
  }
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile newfile(outFileName, "recreate");
  newfile.cd();

  TTree * out_tree_mini_dit_part_avgd_det_asyms_det_weighted = new TTree("mini_dit_part_avgd_det_asyms_det_weighted","mini_dit_part_avgd_det_asyms_det_weighted");
  TTree * out_tree_mini_reg_part_avgd_det_asyms_det_weighted = new TTree("mini_reg_part_avgd_det_asyms_det_weighted","mini_reg_part_avgd_det_asyms_det_weighted");


  TString draw = "0";
  TString draw_weighting_error = "0";
  TString drawn_channels_error = "0";

  if (draw_plots == 0){ // Just calculate the slug, etc. averaged quantities
  

  // Part avgd eigen vector BMOD analysis (segment averaged slopes)
  // Asyms come from the "part avgd" Agg outputs
  // Correction amounts also exist in the agg tree... just not in a #_# easily parsed format of course
  // Main det signal weighted
  drawn_channels_error = "abs(agg_part_avgd_friendable.eigen_#_#_mean_error)";
  draw                 =     Form("%sagg_part_avgd_friendable.eigen_#_#_mean",mod_draw.Data());
  draw_weighting_error = "((rcdb_arm_flag==0)*(agg_part_avgd_friendable.eigen_dit_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_part_avgd_friendable.eigen_dit_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_part_avgd_friendable.eigen_dit_asym_usl_mean_error))"; // Set this to the lagrange analysis if you want to succeed.
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_dit_part_avgd_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,dit_asym_vec,AT_special_detectors,{},null_cuts,AT_cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_dit_part_avgd_det_asyms_det_weighted->Write();
  delete out_tree_mini_dit_part_avgd_det_asyms_det_weighted;

  // The setbranchaddresses onto the mini tree apparently don't like being redone.... so just wipe mini and reset it...
  delete mini;
  mini = new TChain("mini");
  mini->AddFile(mini_infilename);
  mini->AddFriend(agg_part_avgd_friendable,"agg_part_avgd_friendable");
  mini->BuildIndex("run","mini");

  // Part avgd eigen vector regression analysis (segment averaged slopes)
  // Asyms come from the "part avgd" Agg outputs
  // Correction amounts also exist in the agg tree... just not in a #_# easily parsed format of course
  // Main det signal weighted
  drawn_channels_error = "abs(agg_part_avgd_friendable.#_#_mean_error)";
  draw                 =     Form("%sagg_part_avgd_friendable.#_#_mean",mod_draw.Data());
  draw_weighting_error = "((rcdb_arm_flag==0)*(agg_part_avgd_friendable.reg_asym_us_avg_mean_error)+(rcdb_arm_flag==1)*(agg_part_avgd_friendable.reg_asym_usr_mean_error)+(rcdb_arm_flag==2)*(agg_part_avgd_friendable.reg_asym_usl_mean_error))";
  combo_tg_err_segment_getter(averaging_timescale,mini,out_tree_mini_reg_part_avgd_det_asyms_det_weighted,draw,draw_weighting_error,drawn_channels_error,reg_asym_vec,AT_special_detectors,{},null_cuts,AT_cuts,0); //null cut vector here means use draws_piece2 loop for cut definition instead
  out_tree_mini_reg_part_avgd_det_asyms_det_weighted->Write();
  delete out_tree_mini_reg_part_avgd_det_asyms_det_weighted;


  }
  if (draw_plots == 1){ // Instead use the multigraph command to make plots
    TFile data_file_cors(Form("output_data/CREX_All_%s_Avg_Outputs%s.root",averaging_timescale.Data(),suffix.Data()),"read");
    TFile data_file_asyms(Form("output_data/CREX_All_%s_Avg_Outputs%s.root",averaging_timescale.Data(),suffix.Data()),"read");
    TTree* mini_slugs = (TTree*)data_file_asyms.Get("mini_dit_part_avgd_det_asyms_det_weighted");
    out_tree_mini_dit_part_avgd_det_asyms_det_weighted                    = (TTree*)data_file_asyms.Get("mini_dit_part_avgd_det_asyms_det_weighted");
    out_tree_mini_reg_part_avgd_det_asyms_det_weighted                    = (TTree*)data_file_asyms.Get("mini_reg_part_avgd_det_asyms_det_weighted");
    mini_slugs->AddFriend(out_tree_mini_dit_part_avgd_det_asyms_det_weighted);
    mini_slugs->AddFriend(out_tree_mini_reg_part_avgd_det_asyms_det_weighted);


    TString corrections_pdfname = Form("CREX_All_%s_Avg_Outputs%s",averaging_timescale.Data(),suffix.Data());
    TString corrections_pdfname_mod_cut_suffix = "_"+mod_cut;
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
    outfile0.open(Form("./output_data/Results_%s.csv",averaging_timescale.Data()),std::ofstream::app);
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
/*
    TCanvas* c3_1 = new TCanvas();
    ana = "main det weighted asyms - standard regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    c3_1->cd();
    c3_1->SetTitle(ana.Data());
    c3_1->SetName(ana.Data());
    c3_1->Divide(3,3);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_reg_part_avgd_det_asyms_det_weighted.reg_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_reg_part_avgd_det_asyms_det_weighted.reg_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_reg_part_avgd_det_asyms_det_weighted.reg_asym_usl_mean_err))/(1.0e-9)";
    draw                 = Form("%smini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_1,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,AT_special_detectors,{},{},AT_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_1->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_1->SaveAs(Form("output_data/%s.pdf(",corrections_pdfname.Data()));
*/


    TCanvas* c3_1_self = new TCanvas();
    ana = "self weighted asyms - part avg eigenvector regression (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_1_self->cd();
    c3_1_self->SetTitle(ana.Data());
    c3_1_self->SetName(ana.Data());
    c3_1_self->Divide(3,3);
    draw_weighting_error =       "abs(mini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean_self_err/1.0e-9)";
    draw                 = Form("%smini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean_self/1.0e-9",mod_draw.Data());
    drawn_channels_error =       "abs(mini_reg_part_avgd_det_asyms_det_weighted.reg_#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c3_1_self,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,AT_special_detectors,{},{},AT_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_1_self->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_1_self->SaveAs(Form("%s.pdf",corrections_pdfname.Data()));

/*
    TCanvas* c3_6 = new TCanvas();
    ana = "main det weighted asyms - burstwise part avgd eigenvector dithering (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_6->cd();
    c3_6->SetTitle(ana.Data());
    c3_6->SetName(ana.Data());
    c3_6->Divide(3,3);
    //c3_6->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "((rcdb_arm_flag==0)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_us_avg_mean_err)+(rcdb_arm_flag==1)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usr_mean_err)+(rcdb_arm_flag==2)*(mini_dit_part_avgd_det_asyms_det_weighted.dit_asym_usl_mean_err))/(1.0e-9)";
    draw                 =     Form("%smini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean_err/1.0e-9)";
    p1=manyGraph(c3_6,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,AT_special_detectors,{},{},AT_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_6->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_6->SaveAs(Form("output_data/%s.pdf",corrections_pdfname.Data()));

    TCanvas* c3_6_self = new TCanvas();
    ana = "self weighted asyms - burstwise part avgd eigenvector dithering (ppb)"; // No more absolute value in this anymore (with the sorted slopes)
    //ana = "burstwise eigenvector regression - corrections per monitor (usl, r), ppb"; // No more absolute value in this anymore (with the sorted slopes)
    c3_6_self->cd();
    c3_6_self->SetTitle(ana.Data());
    c3_6_self->SetName(ana.Data());
    c3_6_self->Divide(3,3);
    //c3_6_self->Divide(2,special_detectors.size()/2);
    draw_weighting_error = "abs(mini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean_self_err/1.0e-9)";
    draw                 =     Form("%smini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean_self/1.0e-9",mod_draw.Data());
    drawn_channels_error = "abs(mini_dit_part_avgd_det_asyms_det_weighted.dit_#_#_mean_self_err/1.0e-9)";
    p1=manyGraph(c3_6_self,p1,averaging_timescale,(TChain*)mini_slugs,draw,draw_weighting_error,drawn_channels_error,asym_vec,AT_special_detectors,{},{},AT_cuts,1); // empty draws_piece3 and cuts and cuts2 vectors

    c3_6_self->cd();
    label->SetText(0.0,0.005,ana.Data());
    label->Draw("same");
    c3_6_self->SaveAs(Form("output_data/%s.pdf)",corrections_pdfname.Data()));
*/

    data_file_cors.Close();
    data_file_asyms.Close();
  }
}


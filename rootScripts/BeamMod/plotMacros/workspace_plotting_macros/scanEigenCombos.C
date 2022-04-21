void scanEigenCombos(TString bpms = "_allbpms", TString suffix = ""){ //TString input = "NULL",TString runNum = "", TString slugNum = "0"){}
  TString nbpms = "5bpms";
  if (bpms == "_allbpms") { // default case is "" for 5bpm suffix
    nbpms = "allbpms";
  }

  std::map<int,Double_t> runs;
  TString old_suffix = "";
  // Grab all of the relevant combos
  TChain* mini = new TChain("mini");
  TChain* mini_eigen_reg_5bpms                             = new TChain(Form("mini_eigen_reg_5bpms%s",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted                      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_segment_avg          = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_seg_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_slug_avg             = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_slug_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_part_avg             = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_part_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_part_b_avg           = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_part_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_sorted               = new TChain(Form("mini_eigen_reg_5bpms_sorted%s",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_sorted_segment_avg      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_seg_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_sorted_slug_avg      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_slug_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_sorted_part_avg      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_part_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_sorted_part_b_avg    = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_part_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms                           = new TChain(Form("mini_eigen_reg_allbpms%s",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted                    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_segment_avg           = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_seg_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_slug_avg           = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_slug_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_part_avg           = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_part_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_part_b_avg         = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_part_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_sorted             = new TChain(Form("mini_eigen_reg_allbpms_sorted%s",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_sorted_segment_avg    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_seg_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_sorted_slug_avg    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_slug_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_sorted_part_avg    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_part_avg",old_suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_sorted_part_b_avg  = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_part_avg",old_suffix.Data()));

  mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles/rcdb_eigenvectors_sorted.root");
  mini_eigen_reg_5bpms->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles/rcdb_eigenvectors_sorted.root");
  mini_eigen_reg_5bpms_sorted->AddFile("june_2_plots/Part_Avg_EigenCombos.root");
  mini_eigen_reg_5bpms_sorted_segment_avg->AddFile("june_2_plots/Segment_Avg_EigenCombos.root");
  mini_eigen_reg_5bpms_sorted_slug_avg->AddFile("june_2_plots/Slug_Avg_EigenCombos.root");
  mini_eigen_reg_5bpms_sorted_part_avg->AddFile("june_2_plots/Part_Avg_EigenCombos.root");
  mini_eigen_reg_5bpms_sorted_part_b_avg->AddFile("june_2_plots/Part_B_Avg_EigenCombos.root");
  mini_eigen_reg_5bpms_sorted_sorted->AddFile("june_2_plots/Part_Avg_EigenCombos_sorted.root");
  mini_eigen_reg_5bpms_sorted_sorted_segment_avg->AddFile("june_2_plots/Segment_Avg_EigenCombos_sorted.root");
  mini_eigen_reg_5bpms_sorted_sorted_slug_avg->AddFile("june_2_plots/Slug_Avg_EigenCombos_sorted.root");
  mini_eigen_reg_5bpms_sorted_sorted_part_avg->AddFile("june_2_plots/Part_Avg_EigenCombos_sorted.root");
  mini_eigen_reg_5bpms_sorted_sorted_part_b_avg->AddFile("june_2_plots/Part_B_Avg_EigenCombos_sorted.root");
  mini_eigen_reg_allbpms->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles/rcdb_eigenvectors_sorted.root");
  mini_eigen_reg_allbpms_sorted->AddFile("june_2_plots/Part_Avg_EigenCombos.root");
  mini_eigen_reg_allbpms_sorted_segment_avg->AddFile("june_2_plots/Segment_Avg_EigenCombos.root");
  mini_eigen_reg_allbpms_sorted_slug_avg->AddFile("june_2_plots/Slug_Avg_EigenCombos.root");
  mini_eigen_reg_allbpms_sorted_part_avg->AddFile("june_2_plots/Part_Avg_EigenCombos.root");
  mini_eigen_reg_allbpms_sorted_part_b_avg->AddFile("june_2_plots/Part_B_Avg_EigenCombos.root");
  mini_eigen_reg_allbpms_sorted_sorted->AddFile("june_2_plots/Part_Avg_EigenCombos_sorted.root");
  mini_eigen_reg_allbpms_sorted_sorted_segment_avg->AddFile("june_2_plots/Segment_Avg_EigenCombos_sorted.root");
  mini_eigen_reg_allbpms_sorted_sorted_slug_avg->AddFile("june_2_plots/Slug_Avg_EigenCombos_sorted.root");
  mini_eigen_reg_allbpms_sorted_sorted_part_avg->AddFile("june_2_plots/Part_Avg_EigenCombos_sorted.root");
  mini_eigen_reg_allbpms_sorted_sorted_part_b_avg->AddFile("june_2_plots/Part_B_Avg_EigenCombos_sorted.root");


  mini->AddFriend(mini_eigen_reg_5bpms,                             "mini_eigen_reg_5bpms");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted,                      "mini_eigen_reg_5bpms_sorted");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_segment_avg,          "mini_eigen_reg_5bpms_sorted_seg_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_slug_avg,             "mini_eigen_reg_5bpms_sorted_slug_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_part_avg,             "mini_eigen_reg_5bpms_sorted_part_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_part_b_avg,           "mini_eigen_reg_5bpms_sorted_part_b_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_sorted,               "mini_eigen_reg_5bpms_sorted_sorted");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_sorted_segment_avg,   "mini_eigen_reg_5bpms_sorted_sorted_seg_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_sorted_slug_avg,      "mini_eigen_reg_5bpms_sorted_sorted_slug_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_sorted_part_avg,      "mini_eigen_reg_5bpms_sorted_sorted_part_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_sorted_part_b_avg,    "mini_eigen_reg_5bpms_sorted_sorted_part_b_avg");
  mini->AddFriend(mini_eigen_reg_allbpms,                           "mini_eigen_reg_allbpms");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted,                    "mini_eigen_reg_allbpms_sorted");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_segment_avg,        "mini_eigen_reg_allbpms_sorted_seg_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_slug_avg,           "mini_eigen_reg_allbpms_sorted_slug_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_part_avg,           "mini_eigen_reg_allbpms_sorted_part_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_part_b_avg,         "mini_eigen_reg_allbpms_sorted_part_b_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_sorted,             "mini_eigen_reg_allbpms_sorted_sorted");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_sorted_segment_avg, "mini_eigen_reg_allbpms_sorted_sorted_seg_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_sorted_slug_avg,    "mini_eigen_reg_allbpms_sorted_sorted_slug_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_sorted_part_avg,    "mini_eigen_reg_allbpms_sorted_sorted_part_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_sorted_part_b_avg,  "mini_eigen_reg_allbpms_sorted_sorted_part_b_avg");
  mini->BuildIndex("run","mini");

  /*
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
  }; */
  std::vector<TString> devicesX = {
    "bpm4aX",
    "bpm4eX",
    "bpm1X",
    "bpm16X",
  };
  std::vector<TString> devicesY = {
    "bpm4aY",
    "bpm4eY",
    "bpm1Y",
    "bpm16Y",
    "bpm11Y",
    "bpm12Y",
  };
  std::vector<TString> devicesE = {
    "bpm11X",
    "bpm12X",
  };
  std::vector<TString> devicesX_mean = {
    "bpm4aX_mean",
    "bpm4eX_mean",
    "bpm1X_mean",
    "bpm16X_mean",
  };
  std::vector<TString> devicesY_mean = {
    "bpm4aY_mean",
    "bpm4eY_mean",
    "bpm1Y_mean",
    "bpm16Y_mean",
    "bpm11Y_mean",
    "bpm12Y_mean",
  };
  std::vector<TString> devicesE_mean = {
    "bpm11X_mean",
    "bpm12X_mean",
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
    "bpm16Y"
  };
  std::vector<TString> devices_mean = {
    "bpm4aX_mean",
    "bpm4aY_mean",
    "bpm4eX_mean",
    "bpm4eY_mean",
    "bpm1X_mean",
    "bpm1Y_mean",
    "bpm11X_mean",
    "bpm11Y_mean",
    "bpm12X_mean",
    "bpm12Y_mean",
    "bpm16X_mean",
    "bpm16Y_mean"
  };
  std::vector<TString> monitors = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  std::vector<TString> devices5 = {
    "bpm1X",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm12X"
  };
  std::vector<TString> devices5_mean = {
    "bpm1X_mean",
    "bpm4aY_mean",
    "bpm4eX_mean",
    "bpm4eY_mean",
    "bpm12X_mean"
  };
  std::vector<TString> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};

  TString cut = "rcdb_run_type==1 && rcdb_run_flag==1";

  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && run == 5408 && mini == 0";
  //cut = "rcdb_run_type==1 && rcdb_run_flag==1 && run== 6328 && mini == 0";
  //cut = "rcdb_run_type==1 && rcdb_run_flag==1 && run== 7627 && mini == 0";
  TString printstr = "Monitors vs. BPMs combos part 1,";
  for (Int_t j = 0; j < monitors.size() ; j++) {
    // Fill the Columns names
    printstr += monitors.at(j);
    printstr += ",";
  }
  Printf("%s",printstr.Data());
  for (Int_t i = 0 ; i < monitors.size() ; i++) {
    printstr = monitors.at(i);
    printstr += ",";
    for (Int_t j = 0; j < devices_mean.size() ; j++) {
      mini->Draw(Form("mini_eigen_reg_allbpms_sorted_sorted_part_avg.%s_%s",monitors.at(i).Data(),devices_mean.at(j).Data()),cut,"goff");
      printstr += (TString)Form("%0.2f,",mini->GetV1()[0]);
    }
    Printf("%s",printstr.Data());
  }

  //cut = "rcdb_run_type==1 && rcdb_run_flag==1 && run== 5408 && mini == 0";
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && run== 6328 && mini == 0";
  //cut = "rcdb_run_type==1 && rcdb_run_flag==1 && run== 7627 && mini == 0";
  printstr = "Monitors vs. BPMs combos part 2,";
  for (Int_t j = 0; j < devices_mean.size() ; j++) {
    // Fill the Columns names
    printstr += devices.at(j);
    printstr += ",";
  }
  Printf("%s",printstr.Data());
  for (Int_t i = 0 ; i < monitors.size() ; i++) {
    printstr = monitors.at(i);
    printstr += ",";
    for (Int_t j = 0; j < devices_mean.size() ; j++) {
      mini->Draw(Form("mini_eigen_reg_allbpms_sorted_sorted_part_avg.%s_%s",monitors.at(i).Data(),devices_mean.at(j).Data()),cut,"goff");
      printstr += (TString)Form("%0.2f,",mini->GetV1()[0]);
    }
    Printf("%s",printstr.Data());
  }

  //cut = "rcdb_run_type==1 && rcdb_run_flag==1 && run== 5408 && mini == 0";
  //cut = "rcdb_run_type==1 && rcdb_run_flag==1 && run== 6328 && mini == 0";
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && run== 7627 && mini == 0";
  printstr = "Monitors vs. BPMs combos part 3,";
  for (Int_t j = 0; j < devices_mean.size() ; j++) {
    // Fill the Columns names
    printstr += devices.at(j);
    printstr += ",";
  }
  Printf("%s",printstr.Data());
  for (Int_t i = 0 ; i < monitors.size() ; i++) {
    printstr = monitors.at(i);
    printstr += ",";
    for (Int_t j = 0; j < devices_mean.size() ; j++) {
      mini->Draw(Form("mini_eigen_reg_allbpms_sorted_sorted_part_avg.%s_%s",monitors.at(i).Data(),devices_mean.at(j).Data()),cut,"goff");
      printstr += (TString)Form("%0.2f,",mini->GetV1()[0]);
    }
    Printf("%s",printstr.Data());
  }

}

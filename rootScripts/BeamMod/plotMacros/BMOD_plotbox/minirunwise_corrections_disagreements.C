void minirunwise_corrections_disagreements(){ 


  TString scale = "part";
  TString nbpms = "allbpms";
  TString bpms  = "_allbpms";
  TString ana   = "_eigenvectors_double_sorted_part_avg_allbpms";
  TString run_cycle = "run_avg";
  TString run_cycle_wise = "runwise";

  TChain* mini                   = new TChain(Form("agg_ErrorFlag_part_avgd_allbpms_no_err_friendable"));
  TChain* mini_mons_ErrorFlag    = new TChain(Form("agg_ErrorFlag_part_avgd_allbpms_no_err_friendable"));
  TChain* mini_mons_IncludeBMOD  = new TChain(Form("agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable"));
  TChain* mini_mons_OnlyBMOD     = new TChain(Form("agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable"));
  TChain* part_avgd_regression   = new TChain(Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),scale.Data()));
  TChain* part_avgd_lagrange     = new TChain(Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),scale.Data()));
  
  mini                  ->AddFile(Form("/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/processed_respin2_data/mini_friended_agg.root"));
  mini_mons_ErrorFlag   ->AddFile(Form("/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/processed_respin2_data/mini_friended_agg.root"));
  mini_mons_IncludeBMOD ->AddFile(Form("/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/processed_respin2_data/mini_friended_agg.root"));
  mini_mons_OnlyBMOD    ->AddFile(Form("/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/processed_respin2_data/mini_friended_agg.root"));
  part_avgd_regression  ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_part_avg_allbpms/respin2-rcdb-parts-slowc-segments-part_avg_sens-eigen_reg_lagr.root"));
  part_avgd_lagrange    ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_part_avg_allbpms/respin2-rcdb-parts-slowc-segments-part_avg_sens-eigen_reg_lagr.root"));



  mini->AddFriend(mini                   ,Form("data_mini"));
  mini->AddFriend(mini_mons_ErrorFlag    ,Form("agg_ErrorFlag_part_avgd_allbpms_no_err_friendable"));
  mini->AddFriend(mini_mons_IncludeBMOD  ,Form("agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable"));
  mini->AddFriend(mini_mons_OnlyBMOD     ,Form("agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable"));
  mini->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_allbpms_part_avg"));
  mini->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_allbpms_part_avg"));
  mini->SetScanField(0);

  std::string cut = "(rcdb_run_type == 1 && rcdb_run_flag ==1 && rcdb_slug >= 100 && rcdb_slug<4000)"; // Ignore dit_true_data to do a "all data, regardless of if there is meaningful sensitivity underneath" analysis. Also, add in minirun_n == 0 (requires dit->minirun expand) to only get one minirun per run, otherwise it will automatically apply the sensitivities from the run_avg to every minirunwise-slope in the slope tree (which is... not exactly what we want).
  Int_t nmons = 12;

  for (Int_t xx = 0 ; xx<nmons ; xx++) {
    part_avgd_lagrange->SetAlias(Form("us_avg_evMon%d_lagr_reg_diff",xx),Form("((mini_eigen_lagr_allbpms_part_avg.us_avg_evMon%d_new/1e-3)-(mini_eigen_reg_allbpms_part_avg.us_avg_evMon%d_new/1e-3))",xx,xx));
    //part_avgd_lagrange->SetAlias(Form("us_dd_evMon%d_lagr_reg_diff", xx),Form("(mini_eigen_lagr_allbpms_part_avg.us_dd_evMon%d_new/1e-3)-(mini_eigen_reg_allbpms_part_avg.us_dd_evMon%d_new/1e-3)",xx,xx));
    //part_avgd_lagrange->SetAlias(Form("usl_evMon%d_lagr_reg_diff",   xx),Form("(mini_eigen_lagr_allbpms_part_avg.usl_evMon%d_new/1e-3)-(mini_eigen_reg_allbpms_part_avg.usl_evMon%d_new/1e-3)",xx,xx));
    //part_avgd_lagrange->SetAlias(Form("usr_evMon%d_lagr_reg_diff",   xx),Form("(mini_eigen_lagr_allbpms_part_avg.usr_evMon%d_new/1e-3)-(mini_eigen_reg_allbpms_part_avg.usr_evMon%d_new/1e-3)",xx,xx));
  }

  TCanvas * c1 = new TCanvas();
  c1->SaveAs("Outlier_checks/ErrorFlag_evMon_reg_v_lagr_diffs.pdf[");
  c1->SaveAs("Outlier_checks/IncludeBMOD_evMon_reg_v_lagr_diffs.pdf[");
  c1->SaveAs("Outlier_checks/OnlyBMOD_evMon_reg_v_lagr_diffs.pdf[");
  c1->SaveAs("Outlier_checks/IncludeBMOD_v_ErrorFlag_evMon_lagr_diffs.pdf[");
  c1->SaveAs("Outlier_checks/OnlyBMOD_v_ErrorFlag_evMon_lagr_diffs.pdf[");

  Printf("\n\n ErrorFlag Cut - regression vs. lagrange slopes * diffs compared to 20 percent of evMon0 stddev  \n\n");
  for (Int_t imon = 0 ; imon < nmons; imon++) {
    c1->Clear();
    gPad->SetLogy();
    Printf("evMon%d ErrorFlag Cut: Scanning the lagr slopes vs. reg slopes difference in correction -> Comparing to 0.1* the evMon0 main energy sensitive correction-minirunwise-stddev (difference in correction for evMon%d > 0.1* evMon_0 stddev is printed)",imon,imon);
    mini->Scan(Form("run_number:minirun_n:rcdb_slug:mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff:agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean:agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff"),(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff) > ( 0.1*0.015 ) )"); // 0.015 = 15,000 ppb which is stddev of corrections for evMon0 in all 3 parts of CREX
    mini->Draw("1e6*rcdb_sign*agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff) > ( -0.1*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("ErrorFlag Lagr vs. reg evMon%d",imon),Form("ErrorFlag Lagr vs. reg evMon%d cor diff",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/ErrorFlag_evMon_reg_v_lagr_diffs.pdf");
    mini->Draw("rcdb_sign*agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean/agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean_error","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff) > ( -0.1*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("ErrorFlag evMon%d pull",imon),Form("ErrorFlag evMon%d pull",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/ErrorFlag_evMon_reg_v_lagr_diffs.pdf");
  }
  c1->SaveAs("Outlier_checks/ErrorFlag_evMon_reg_v_lagr_diffs.pdf]");

  Printf("\n\n IncludeBMOD Cut - regression vs. lagrange slopes * diffs compared to 10 percent of evMon0 stddev  \n\n");
  for (Int_t imon = 0 ; imon < nmons; imon++) {
    c1->Clear();
    gPad->SetLogy();
    Printf("evMon%d IncludeBMOD Cut: Scanning the lagr slopes vs. reg slopes difference in correction -> Comparing to 0.1* the evMon0 main energy sensitive correction-minirunwise-stddev (difference in correction for evMon%d > 0.1* evMon_0 stddev is printed)",imon,imon);
    mini->Scan(Form("run_number:minirun_n:rcdb_slug:mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff:agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean:agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff"),(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs(agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff) > ( 0.1*0.015 ) )"); // 0.015 = 15,000 ppb which is stddev of corrections for evMon0 in all 3 parts of CREX
    mini->Draw("1e6*rcdb_sign*agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs(agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff) > ( -0.1*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("IncludeBMOD Lagr vs. reg evMon%d",imon),Form("IncludeBMOD Lagr vs. reg evMon%d cor diff",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/IncludeBMOD_evMon_reg_v_lagr_diffs.pdf");
    mini->Draw("rcdb_sign*agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean/agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean_error","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs(agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff) > ( -0.1*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("IncludeBMOD evMon%d pull",imon),Form("IncludeBMOD evMon%d pull",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/IncludeBMOD_evMon_reg_v_lagr_diffs.pdf");
  }
  c1->SaveAs("Outlier_checks/IncludeBMOD_evMon_reg_v_lagr_diffs.pdf]");

  Printf("\n\n OnlyBMOD Cut - regression vs. lagrange slopes * diffs compared to 10 percent of evMon0 stddev  \n\n");
  for (Int_t imon = 0 ; imon < nmons; imon++) {
    c1->Clear();
    gPad->SetLogy();
    Printf("evMon%d OnlyBMOD Cut: Scanning the lagr slopes vs. reg slopes difference in correction -> Comparing to 0.3* the evMon0 main energy sensitive correction-minirunwise-stddev (difference in correction for evMon%d > 0.3* evMon_0 stddev is printed)",imon,imon);
    mini->Scan(Form("run_number:minirun_n:rcdb_slug:mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff:agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean:agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff"),(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs(agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff) > ( 0.3*0.015 ) )"); // 0.015 = 15,000 ppb which is stddev of corrections for evMon0 in all 3 parts of CREX
    mini->Draw("1e6*rcdb_sign*agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs(agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff) > ( -0.1*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("OnlyBMOD Lagr vs. reg evMon%d",imon),Form("OnlyBMOD Lagr vs. reg evMon%d cor diff",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/OnlyBMOD_evMon_reg_v_lagr_diffs.pdf");
    mini->Draw("rcdb_sign*agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean/agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean_error","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs(agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff) > ( -0.1*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("OnlyBMOD evMon%d pull",imon),Form("OnlyBMOD evMon%d pull",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/OnlyBMOD_evMon_reg_v_lagr_diffs.pdf");
  }
  c1->SaveAs("Outlier_checks/OnlyBMOD_evMon_reg_v_lagr_diffs.pdf]");

  Printf("\n\n IncludeBMOD vs. ErrorFlag Cut - lagrange slopes * Cut Difference in diffs compared to 0.1 percent of evMon0 stddev \n\n");
  for (Int_t imon = 0 ; imon < nmons; imon++) {
    c1->Clear();
    gPad->SetLogy();
    Printf("evMon%d IncludeBMOD Cut vs. ErrorFlag Cut: Scanning the lagr slopes times the difference in minirunwise evMon diff mean correction -> Comparing to 0.001* the evMon0 main energy sensitive correction-minirunwise-stddev (difference in correction for evMon%d > 0.001* evMon_0 stddev is printed)",imon,imon);
    mini->Scan(Form("run_number:minirun_n:rcdb_slug:mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new:agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean:agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean:agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean:(agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new"),(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs((agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new) > ( 0.001*0.015 ) )"); // 0.015 = 15,000 ppb which is stddev of corrections for evMon0 in all 3 parts of CREX
    mini->Draw("1e6*rcdb_sign*(agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs((agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new) > ( -0.001*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("IncludeBMOD vs. ErrorFlag Lagr evMon%d",imon),Form("IncludeBMOD vs. ErrorFlag Lagr evMon%d cor diff",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/IncludeBMOD_v_ErrorFlag_evMon_lagr_diffs.pdf");
    mini->Draw("rcdb_sign*(agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)/(sqrt(abs(pow(agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean_error,2)-pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean_error,2))))","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs((agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new) > ( -0.001*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("IncludeBMOD vs. ErrorFlag evMon%d pull",imon),Form("IncludeBMOD vs. ErrorFlag evMon%d pull",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/IncludeBMOD_v_ErrorFlag_evMon_lagr_diffs.pdf");
  }
  c1->SaveAs("Outlier_checks/IncludeBMOD_v_ErrorFlag_evMon_lagr_diffs.pdf]");

  Printf("\n\n OnlyBMOD vs. ErrorFlag Cut - lagrange slopes * Cut Difference in diffs compared to 2 percent of evMon0 stddev \n\n");
  for (Int_t imon = 0 ; imon < nmons; imon++) {
    c1->Clear();
    gPad->SetLogy();
    Printf("evMon%d OnlyBMOD Cut vs. ErrorFlag Cut: Scanning the lagr slopes times the difference in minirunwise evMon diff mean correction -> Comparing to 0.02* the evMon0 main energy sensitive correction-minirunwise-stddev (difference in correction for evMon%d > 0.02* evMon_0 stddev is printed)",imon,imon);
    mini->Scan(Form("run_number:minirun_n:rcdb_slug:mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new:agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean:agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean:agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean:(agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new"),(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs((agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new) > ( 0.02*0.015 ) )"); // 0.015 = 15,000 ppb which is stddev of corrections for evMon0 in all 3 parts of CREX
    mini->Draw("1e6*rcdb_sign*(agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs((agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new) > ( -0.02*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("OnlyBMOD vs. ErrorFlag Lagr evMon%d",imon),Form("OnlyBMOD vs. ErrorFlag Lagr evMon%d cor diff",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/OnlyBMOD_v_ErrorFlag_evMon_lagr_diffs.pdf");
    mini->Draw("rcdb_sign*(agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)/(sqrt(abs(pow(agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean_error,2)-pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean_error,2))))","1/pow(agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.eigen_lagr_asym_main_det_mean_error,2)*("+(TString)cut+" && rcdb_arm_flag==0 && (1==0 || abs((agg_OnlyBMOD_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean-agg_ErrorFlag_part_avgd_allbpms_no_err_friendable.diff_evMon"+TString::Itoa(imon,10)+"_mean)*mini_eigen_lagr_allbpms_part_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new) > ( -0.02*0.015 ) ))");
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("OnlyBMOD vs. ErrorFlag evMon%d pull",imon),Form("OnlyBMOD vs. ErrorFlag evMon%d pull",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs("Outlier_checks/OnlyBMOD_v_ErrorFlag_evMon_lagr_diffs.pdf");
  }
  c1->SaveAs("Outlier_checks/OnlyBMOD_v_ErrorFlag_evMon_lagr_diffs.pdf]");

}

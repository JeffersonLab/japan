void scan_slugwise_corrections_diffs(TString cutana = "ErrorFlag"){

  TChain * mini = new TChain("mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted");
  TChain * lagr = new TChain("mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted");
  TChain * lagr_ErrorFlag = new TChain("mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted");
  TChain * reg = new TChain("mini_eigen_reg_allbpms_part_avg_corrections_det_weighted");
  mini->AddFile(Form("processed_respin2_data/results/CREX_All_%s_rcdb_slug_Avg_Outputs_main_det_corrections.root",cutana.Data()));
  lagr->AddFile(Form("processed_respin2_data/results/CREX_All_%s_rcdb_slug_Avg_Outputs_main_det_corrections.root",cutana.Data()));
  lagr_ErrorFlag->AddFile(Form("processed_respin2_data/results/CREX_All_ErrorFlag_rcdb_slug_Avg_Outputs_main_det_corrections.root"));
  reg->AddFile(Form("processed_respin2_data/results/CREX_All_%s_rcdb_slug_Avg_Outputs_main_det_corrections.root",cutana.Data()));
  mini->AddFriend(lagr);
  mini->AddFriend(reg);
  mini->AddFriend(lagr_ErrorFlag,"mini_eigen_lagr_ErrorFlag_allbpms_part_avg_corrections_det_weighted");
  mini->SetScanField(0);

  TCanvas * c1 = new TCanvas();
  c1->SaveAs(Form("Outlier_checks/%s_slugwise_corrections_plot.pdf[",cutana.Data()));

  for (Int_t imon = 0 ; imon < 12 ; imon++) {
    c1->Clear();
    mini->Draw(Form("(mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean-mini_eigen_reg_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean)/mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_stddev",imon,imon,imon));
    ((TH1F*)gROOT->FindObject("htemp"))->SetNameTitle(Form("evMon%d Lagr - reg / stddev",imon),Form("evMon%d Lagr - reg slugwise mean / slug's stddev",imon));
    gPad->Modified();
    gPad->Update();
    c1->SaveAs(Form("Outlier_checks/%s_slugwise_corrections_plot.pdf",cutana.Data()));
    c1->Clear();
    Int_t nen1 = mini->Draw(Form("mini_eigen_reg_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_stddev/mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_stddev:rcdb_slug",imon,imon),"","goff");
    TGraph* tge1 = new TGraph(nen1,mini->GetV2(),mini->GetV1());
    tge1->SetNameTitle(Form("evMon%d reg stddev / Lagr stddev",imon),Form("evMon%d reg stddev / Lagr slugwise mean's stddev",imon));
    tge1->Draw("ap*");
    gPad->Modified();
    gPad->Update();
    c1->SaveAs(Form("Outlier_checks/%s_slugwise_corrections_plot.pdf",cutana.Data()));
    if (cutana!="ErrorFlag"){
      c1->Clear();
      Printf("Slug, %s evMon%d correction, ErrorFlag correction, 100 x Difference/ErrorFlag val, ppm Difference of %s - ErrorFlag corrections, ratio of %s/ErrorFlag corrections",cutana.Data(),imon,cutana.Data(),cutana.Data());
      mini->Scan(Form("rcdb_slug:mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean:mini_eigen_lagr_ErrorFlag_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean:100.0*(mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean-mini_eigen_lagr_ErrorFlag_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean)/mini_eigen_lagr_ErrorFlag_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean:(mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean-mini_eigen_lagr_ErrorFlag_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean)/1e-6:mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean/mini_eigen_lagr_ErrorFlag_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_mean",imon,imon,imon,imon,imon,imon,imon,imon,imon));
      Int_t nen2 = mini->Draw(Form("mini_eigen_lagr_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_stddev/mini_eigen_lagr_ErrorFlag_allbpms_part_avg_corrections_det_weighted.manual_main_det_evMon%d_new_stddev:rcdb_slug",imon,imon),"","goff");
      TGraph* tge2 = new TGraph(nen1,mini->GetV2(),mini->GetV1());
      tge2->SetNameTitle(Form("evMon%d lagr %s stddev / ErrorFlag stddev",imon,cutana.Data()),Form("evMon%d %s stddev / ErrorFlag slugwise mean's stddev",imon,cutana.Data()));
      tge2->Draw("ap*");
      gPad->Modified();
      gPad->Update();
      c1->SaveAs(Form("Outlier_checks/%s_slugwise_corrections_plot.pdf",cutana.Data()));
    }
  }
  c1->SaveAs(Form("Outlier_checks/%s_slugwise_corrections_plot.pdf]",cutana.Data()));
}

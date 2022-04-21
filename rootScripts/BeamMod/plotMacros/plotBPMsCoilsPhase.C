void plotBPMsCoilsPhase(){

  TCanvas* c1 = new TCanvas();
  c1 -> Divide(7,3);

  TCanvas* c2 = new TCanvas();
  c2 -> Divide(7,2);

  TCanvas* c3 = new TCanvas();
  c3 -> Divide(7,2);

  TVirtualPad * p1;
  TGraph* g1;
  TH1F* hbuff;

  TChain* agg = new TChain("agg");
  agg->Add("/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/aggRootfiles/Coils_IncludeBMOD_drl_no_err_part_avg_allbpms/slugRootfiles/CREX-All-miniruns.root");

  std::vector<TString> Asyms = {"eigen_reg_asym_us_avg","eigen_lagr_asym_us_avg"};
  std::vector<TString> BPMs = {"diff_bpm4aX","diff_bpm4aY","diff_bpm12X"};
  std::vector<TString> coils = {"1","2","3","4","5","6","7"};
  for (Int_t i = 0 ; i < BPMs.size() ; i++) {
    for (Int_t j = 0 ; j < coils.size() ; j++) {
      p1 = c1 -> cd(i*coils.size() + j + 1);
      Int_t nen = agg->Draw(Form("OnlyBMOD_0Pi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),BPMs.at(i).Data()),"OnlyBMOD_yield_beam_mod_ramp_mean>50","");
      TGraph* local_g1 = new TGraph(nen,agg->GetV2(),agg->GetV1());
      if (local_g1 != 0) {
        local_g1->SetNameTitle(Form("StaringPoint_coil%s_%s",coils.at(j).Data(),BPMs.at(i).Data()),Form("Coil %s data only %s RMS;Run+0.1*Minirun;Coil %s - %s RMS",coils.at(j).Data(),BPMs.at(i).Data(),coils.at(j).Data(),BPMs.at(i).Data()));
        local_g1->SetMarkerColor(kWhite);
        local_g1->SetMarkerSize(0.01);
        local_g1->Draw("ap");
        p1->Modified();
        p1->Update();
      }
      agg->Draw(Form("OnlyBMOD_0Pi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),BPMs.at(i).Data()),Form("OnlyBMOD_0Pi_coil%s_yield_beam_mod_ramp_mean>50",coils.at(j).Data()),"*same");
      g1 = (TGraph*)p1->FindObject("Graph");
      if (g1 != 0) {
        g1->SetMarkerColor(kRed);
        g1->SetMarkerSize(0.1);
        g1->SetName(Form("0Pi_coil%s_%s",coils.at(j).Data(),BPMs.at(i).Data()));
      }
      agg->Draw(Form("OnlyBMOD_LowQuarterPi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),BPMs.at(i).Data()),Form("OnlyBMOD_LowQuarterPi_coil%s_yield_beam_mod_ramp_mean>50",coils.at(j).Data()),"*same");
      g1 = (TGraph*)p1->FindObject("Graph");
      if (g1 != 0) {
        g1->SetMarkerColor(kBlue);
        g1->SetMarkerSize(0.1);
        g1->SetName(Form("LowQuarterPi_coil%s_%s",coils.at(j).Data(),BPMs.at(i).Data()));
      }
      agg->Draw(Form("OnlyBMOD_HalfPi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),BPMs.at(i).Data()),Form("OnlyBMOD_HalfPi_coil%s_yield_beam_mod_ramp_mean>50",coils.at(j).Data()),"*same");
      g1 = (TGraph*)p1->FindObject("Graph");
      if (g1 != 0) {
        g1->SetMarkerColor(kBlack);
        g1->SetMarkerSize(0.1);
        g1->SetName(Form("HalfPi_coil%s_%s",coils.at(j).Data(),BPMs.at(i).Data()));
      }
    }
  }

  c1->SaveAs("coil_BPM_check.pdf");

  //agg->Draw("OnlyBMOD_HighQuarterPi_coil1_diff_bpm4aX_rms:run_number+0.1*minirun_n","OnlyBMOD_HighQuarterPi_coil1_yield_beam_mod_ramp_mean>50","*same");
  //agg->Draw("OnlyBMOD_1Pi_coil1_diff_bpm4aX_rms:run_number+0.1*minirun_n","OnlyBMOD_1Pi_coil1_yield_beam_mod_ramp_mean>50","*same");
  

  for (Int_t i = 0 ; i < Asyms.size() ; i++) {
    for (Int_t j = 0 ; j < coils.size() ; j++) {
      p1 = c2 -> cd(i*coils.size() + j + 1);
      Int_t nen = agg->Draw(Form("OnlyBMOD_0Pi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),Asyms.at(i).Data()),"OnlyBMOD_yield_beam_mod_ramp_mean>50 && rcdb_arm_flag==0 && rcdb_run_type == 1 && rcdb_run_flag == 1","");
      TGraph* local_g1 = new TGraph(nen,agg->GetV2(),agg->GetV1());
      if (local_g1 != 0) {
        local_g1->SetNameTitle(Form("StaringPoint_coil%s_%s",coils.at(j).Data(),Asyms.at(i).Data()),Form("Coil %s, %s RMS;Run+0.1*Minirun;Coil %s - %s RMS",coils.at(j).Data(),Asyms.at(i).Data(),coils.at(j).Data(),Asyms.at(i).Data()));
        local_g1->SetMarkerColor(kWhite);
        local_g1->SetMarkerSize(0.01);
        local_g1->Draw("ap");
        p1->Modified();
        p1->Update();
      }
      agg->Draw(Form("OnlyBMOD_0Pi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),Asyms.at(i).Data()),Form("OnlyBMOD_0Pi_coil%s_yield_beam_mod_ramp_mean>50 && rcdb_arm_flag==0 && rcdb_run_type == 1 && rcdb_run_flag == 1",coils.at(j).Data()),"*same");
      g1 = (TGraph*)p1->FindObject("Graph");
      if (g1 != 0) {
        g1->SetMarkerColor(kRed);
        g1->SetMarkerSize(0.1);
        g1->SetName(Form("0Pi_coil%s_%s",coils.at(j).Data(),Asyms.at(i).Data()));
      }
      agg->Draw(Form("OnlyBMOD_LowQuarterPi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),Asyms.at(i).Data()),Form("OnlyBMOD_LowQuarterPi_coil%s_yield_beam_mod_ramp_mean>50 && rcdb_arm_flag==0 && rcdb_run_type == 1 && rcdb_run_flag == 1",coils.at(j).Data()),"*same");
      g1 = (TGraph*)p1->FindObject("Graph");
      if (g1 != 0) {
        g1->SetMarkerColor(kBlue);
        g1->SetMarkerSize(0.1);
        g1->SetName(Form("LowQuarterPi_coil%s_%s",coils.at(j).Data(),Asyms.at(i).Data()));
      }
      agg->Draw(Form("OnlyBMOD_HalfPi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),Asyms.at(i).Data()),Form("OnlyBMOD_HalfPi_coil%s_yield_beam_mod_ramp_mean>50 && rcdb_arm_flag==0 && rcdb_run_type == 1 && rcdb_run_flag == 1",coils.at(j).Data()),"*same");
      g1 = (TGraph*)p1->FindObject("Graph");
      if (g1 != 0) {
        g1->SetMarkerColor(kBlack);
        g1->SetMarkerSize(0.1);
        g1->SetName(Form("HalfPi_coil%s_%s",coils.at(j).Data(),Asyms.at(i).Data()));
      }
    }
  }

  c2->SaveAs("coil_Asym_check.pdf(");

  for (Int_t i = 0 ; i < Asyms.size() ; i++) {
    for (Int_t j = 0 ; j < coils.size() ; j++) {
      p1 = c3 -> cd(i*coils.size() + j + 1);
      Int_t nen = agg->Draw(Form("OnlyBMOD_0Pi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),Asyms.at(i).Data()),"OnlyBMOD_yield_beam_mod_ramp_mean>50 && rcdb_arm_flag==0 && rcdb_run_type == 1 && rcdb_run_flag == 1 && run_number >= 7500","");
      TGraph* local_g1 = new TGraph(nen,agg->GetV2(),agg->GetV1());
      if (local_g1 != 0) {
        local_g1->SetNameTitle(Form("StaringPoint_coil%s_%s",coils.at(j).Data(),Asyms.at(i).Data()),Form("Coil %s, %s RMS;Run+0.1*Minirun;Coil %s - %s RMS",coils.at(j).Data(),Asyms.at(i).Data(),coils.at(j).Data(),Asyms.at(i).Data()));
        local_g1->SetMarkerColor(kWhite);
        local_g1->SetMarkerSize(0.01);
        local_g1->Draw("ap");
        p1->Modified();
        p1->Update();
      }
      agg->Draw(Form("OnlyBMOD_0Pi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),Asyms.at(i).Data()),Form("OnlyBMOD_0Pi_coil%s_yield_beam_mod_ramp_mean>50 && rcdb_arm_flag==0 && rcdb_run_type == 1 && rcdb_run_flag == 1 && run_number >= 7500",coils.at(j).Data()),"*same");
      g1 = (TGraph*)p1->FindObject("Graph");
      if (g1 != 0) {
        g1->SetMarkerColor(kRed);
        g1->SetMarkerSize(0.1);
        g1->SetName(Form("0Pi_coil%s_%s",coils.at(j).Data(),Asyms.at(i).Data()));
      }
      agg->Draw(Form("OnlyBMOD_LowQuarterPi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),Asyms.at(i).Data()),Form("OnlyBMOD_LowQuarterPi_coil%s_yield_beam_mod_ramp_mean>50 && rcdb_arm_flag==0 && rcdb_run_type == 1 && rcdb_run_flag == 1 && run_number >= 7500",coils.at(j).Data()),"*same");
      g1 = (TGraph*)p1->FindObject("Graph");
      if (g1 != 0) {
        g1->SetMarkerColor(kBlue);
        g1->SetMarkerSize(0.1);
        g1->SetName(Form("LowQuarterPi_coil%s_%s",coils.at(j).Data(),Asyms.at(i).Data()));
      }
      agg->Draw(Form("OnlyBMOD_HalfPi_coil%s_%s_rms:run_number+0.1*minirun_n",coils.at(j).Data(),Asyms.at(i).Data()),Form("OnlyBMOD_HalfPi_coil%s_yield_beam_mod_ramp_mean>50 && rcdb_arm_flag==0 && rcdb_run_type == 1 && rcdb_run_flag == 1 && run_number >= 7500",coils.at(j).Data()),"*same");
      g1 = (TGraph*)p1->FindObject("Graph");
      if (g1 != 0) {
        g1->SetMarkerColor(kBlack);
        g1->SetMarkerSize(0.1);
        g1->SetName(Form("HalfPi_coil%s_%s",coils.at(j).Data(),Asyms.at(i).Data()));
      }
    }
  }

  c3->SaveAs("coil_Asym_check.pdf)");


}

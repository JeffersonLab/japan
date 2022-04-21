void getCorrections(TString det = "us_avg"){
  TFile *_file0 = TFile::Open("respin2_data/eigen_noerr_part_avg_allbpms_lagr_slopes.root");
  TChain * mini = new TChain("mini");
  TChain * mini_eigen_lagr_allbpms_part_avg = new TChain("mini_eigen_lagr_allbpms_part_avg");
  TChain * agg = new TChain("agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable");
  mini                              -> Add("respin2_data/eigen_noerr_part_avg_allbpms_lagr_slopes.root");
  mini_eigen_lagr_allbpms_part_avg  -> Add("respin2_data/eigen_noerr_part_avg_allbpms_lagr_slopes.root");
  agg                               -> Add("processed_respin2_data/mini_friended_agg.root");
  mini->AddFriend(mini_eigen_lagr_allbpms_part_avg);
  mini->AddFriend(agg,"agg");

  TH1F * th1;
  std::vector<TString> monitors = {"diff_evMon0_new","diff_evMon1_new","diff_evMon2_new","diff_evMon3_new","diff_evMon4_new","diff_evMon5_new","diff_evMon6_new","diff_evMon7_new","diff_evMon8_new","diff_evMon9_new","diff_evMon10_new","diff_evMon11_new"};
  std::vector<TString> agg_monitors = {"diff_evMon0","diff_evMon1","diff_evMon2","diff_evMon3","diff_evMon4","diff_evMon5","diff_evMon6","diff_evMon7","diff_evMon8","diff_evMon9","diff_evMon10","diff_evMon11"};
  Double_t slope_1, slope_2, slope_3, bpm_rms_1, bpm_rms_2, bpm_rms_3, cor_rms_1, cor_rms_2, cor_rms_3;
  Printf("evMon #, slope 1, correction RMS 1, monitor RMS 1, Slope 2, correction RMS 2, monitor RMS 2, slope 3, correction RMS 3, monitor RMS 3");
  for ( Int_t i = 0 ; i < monitors.size() ; i++ ) {
    // BPM mean
    // Handled by my other scripts mini->Draw("diff_evMon0_new.mean","pow(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err,-2)","prof");
    // BPM mean err
    // Handled by my other scripts mini->Draw("diff_evMon0_new.err","pow(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err,-2)","prof");

    // BPM RMS
    // From the mini tree version
    //mini->Draw(Form("diff_evMon%d_new.rms/1e-3:crex_part",i),"pow(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err,-2)*(rcdb_arm_flag==0)","prof");
    // From the agg tree version
    mini->Draw(Form("agg.diff_evMon%d_rms/1e-3:crex_part",i),"pow(agg.eigen_lagr_asym_main_det_mean_error,-2)*(rcdb_arm_flag==0)","prof");
    th1 = (TH1F*)gROOT->FindObject("htemp");
    bpm_rms_1 = th1->GetBinContent(9);
    bpm_rms_2 = th1->GetBinContent(50);
    bpm_rms_3 = th1->GetBinContent(92);

    // Slope
    // From the mini tree version
    //mini->Draw(Form("mini_eigen_lagr_allbpms_part_avg.%s_evMon%d_new/(1e-3):crex_part",det.Data(),i),"pow(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err,-2)*(rcdb_arm_flag==0)","prof");
    // From the agg tree version
    mini->Draw(Form("mini_eigen_lagr_allbpms_part_avg.%s_evMon%d_new/(1e-3):crex_part",det.Data(),i),"pow(agg.eigen_lagr_asym_main_det_mean_error,-2)*(rcdb_arm_flag==0)","prof");
    th1 = (TH1F*)gROOT->FindObject("htemp");
    slope_1 = th1->GetBinContent(9);
    slope_2 = th1->GetBinContent(50);
    slope_3 = th1->GetBinContent(92);

    // Correction mean
    // Handled by my other scripts mini->Draw("diff_evMon0_new.mean*abs(mini_eigen_lagr_allbpms_part_avg.us_avg_evMon0_new/1e-6):crex_part","pow(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err,-2)","prof");

    // Correction err
    // Handled by my other scripts - the "self BPM err fit outputs!!" even the maindet weighted version is right here!!!
    // mini->Draw("diff_evMon0_new.err*abs(mini_eigen_lagr_allbpms_part_avg.us_avg_evMon0_new/1e-6):crex_part","pow(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err,-2)","prof");
    // Correction RMS
    // From the mini tree version
    //mini->Draw(Form("diff_evMon%d_new.rms*abs(mini_eigen_lagr_allbpms_part_avg.%s_evMon%d_new/1e-6):crex_part",i,det.Data(),i),"pow(mini_eigen_lagr_allbpms_part_avg.lagr_asym_us_avg.err,-2)*(rcdb_arm_flag==0)","prof");
    // From the agg tree version
    mini->Draw(Form("agg.diff_evMon%d_rms*abs(mini_eigen_lagr_allbpms_part_avg.%s_evMon%d_new/1e-6):crex_part",i,det.Data(),i),"pow(agg.eigen_lagr_asym_main_det_mean_error,-2)*(rcdb_arm_flag==0)","prof");
    th1 = (TH1F*)gROOT->FindObject("htemp");
    cor_rms_1 = th1->GetBinContent(9);
    cor_rms_2 = th1->GetBinContent(50);
    cor_rms_3 = th1->GetBinContent(92);

    Printf("%d, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f",i, slope_1,cor_rms_1,bpm_rms_1,slope_2,cor_rms_2,bpm_rms_2,slope_3,cor_rms_3,bpm_rms_3);
  }
}

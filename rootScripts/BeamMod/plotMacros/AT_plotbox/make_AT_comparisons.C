void make_AT_comparisons() {
  TChain * agg = new TChain("agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable");
  TChain * agg_5bpm = new TChain("agg_IncludeBMOD_part_avgd_no_err_friendable");
  agg->Add("../processed_respin2_data/mini_friended_agg.root");
  agg_5bpm->Add("../processed_respin2_data/mini_friended_agg.root");
  agg->AddFriend(agg_5bpm,"agg_5bpm");

  Int_t nent = agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->Draw("eigen_lagr_asym_atl1_mean:pow(eigen_lagr_asym_atl1_mean_error/eigen_lagr_asym_main_det_mean_error,2):eigen_lagr_asym_main_det_mean_error:run_number+0.1*minirun_n","rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag == 0 && rcdb_flip_state<=2 && ( !(run_number==6564 && minirun_n==4) && !(run_number==6567 && (minirun_n==2 || minirun_n==4)) && !(run_number==6571 && (minirun_n==3 || minirun_n==4)) && !(run_number==6593 && minirun_n==2) && !(run_number==6983 && minirun_n==8) && !(run_number==7149 && minirun_n==6) && !(run_number==7211 && minirun_n==4) && !(run_number==7889 && minirun_n==0) && !(run_number==7942 && minirun_n==5) && !(run_number==8036 && minirun_n==2) && !(run_number==8240 && minirun_n==1) && !(run_number==8549 && (minirun_n==0 || minirun_n==1 || minirun_n==4))) && rcdb_slug<=185","goff") ;
  TGraphErrors * atl1 = new TGraphErrors(nent,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV4(),agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV2(),0,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV3());

  atl1->Fit("pol0");
  TF1* atl1_eigen_lagr_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t atl1_eigen_lagr_scalefactor = sqrt(atl1_eigen_lagr_fit->GetParameter(0));

  nent = agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->Draw(Form("0:eigen_lagr_asym_atl1_mean:sqrt(%e)*eigen_lagr_asym_main_det_mean_error:run_number+0.1*minirun_n",atl1_eigen_lagr_scalefactor),"rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag == 0 && rcdb_flip_state<=2 && ( !(run_number==6564 && minirun_n==4) && !(run_number==6567 && (minirun_n==2 || minirun_n==4)) && !(run_number==6571 && (minirun_n==3 || minirun_n==4)) && !(run_number==6593 && minirun_n==2) && !(run_number==6983 && minirun_n==8) && !(run_number==7149 && minirun_n==6) && !(run_number==7211 && minirun_n==4) && !(run_number==7889 && minirun_n==0) && !(run_number==7942 && minirun_n==5) && !(run_number==8036 && minirun_n==2) && !(run_number==8240 && minirun_n==1) && !(run_number==8549 && (minirun_n==0 || minirun_n==1 || minirun_n==4))) && rcdb_slug<=185","goff") ;
  TGraphErrors * atl1_eigen_lagr_mean = new TGraphErrors(nent,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV4(),agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV2(),0,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV3());
  atl1_eigen_lagr_mean->Fit("pol0");

  Double_t atl1_eigen_lagr_mean     = atl1_eigen_lagr_fit->GetParameter(0);
  Double_t atl1_eigen_lagr_mean_err = atl1_eigen_lagr_fit->GetParError(0);



  Int_t nent = agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->Draw("agg_5bpm.eigen_reg_asym_atl1_mean:pow(agg_5bpm.eigen_reg_asym_atl1_mean_error/agg_5bpm.eigen_reg_asym_main_det_mean_error,2):agg_5bpm.eigen_reg_asym_main_det_mean_error:run_number+0.1*minirun_n","rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag == 0 && rcdb_flip_state<=2 && ( !(run_number==6564 && minirun_n==4) && !(run_number==6567 && (minirun_n==2 || minirun_n==4)) && !(run_number==6571 && (minirun_n==3 || minirun_n==4)) && !(run_number==6593 && minirun_n==2) && !(run_number==6983 && minirun_n==8) && !(run_number==7149 && minirun_n==6) && !(run_number==7211 && minirun_n==4) && !(run_number==7889 && minirun_n==0) && !(run_number==7942 && minirun_n==5) && !(run_number==8036 && minirun_n==2) && !(run_number==8240 && minirun_n==1) && !(run_number==8549 && (minirun_n==0 || minirun_n==1 || minirun_n==4))) && rcdb_slug<=185","goff") ;
  TGraphErrors * atl1 = new TGraphErrors(nent,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV4(),agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV2(),0,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV3());

  atl1->Fit("pol0");
  TF1* atl1_5bpm_reg_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t atl1_5bpm_reg_scalefactor = sqrt(atl1_5bpm_reg_fit->GetParameter(0));

  nent = agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->Draw(Form("0:agg_5bpm.eigen_reg_asym_atl1_mean:sqrt(%e)*agg_5bpm.eigen_reg_asym_main_det_mean_error:run_number+0.1*minirun_n",atl1_5bpm_reg_scalefactor),"rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag == 0 && rcdb_flip_state<=2 && ( !(run_number==6564 && minirun_n==4) && !(run_number==6567 && (minirun_n==2 || minirun_n==4)) && !(run_number==6571 && (minirun_n==3 || minirun_n==4)) && !(run_number==6593 && minirun_n==2) && !(run_number==6983 && minirun_n==8) && !(run_number==7149 && minirun_n==6) && !(run_number==7211 && minirun_n==4) && !(run_number==7889 && minirun_n==0) && !(run_number==7942 && minirun_n==5) && !(run_number==8036 && minirun_n==2) && !(run_number==8240 && minirun_n==1) && !(run_number==8549 && (minirun_n==0 || minirun_n==1 || minirun_n==4))) && rcdb_slug<=185","goff") ;
  TGraphErrors * atl1_5bpm_reg_mean = new TGraphErrors(nent,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV4(),agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV2(),0,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV3());
  atl1_5bpm_reg_mean->Fit("pol0");

  Double_t atl1_5bpm_reg_mean     = atl1_5bpm_reg_fit->GetParameter(0);
  Double_t atl1_5bpm_reg_mean_err = atl1_5bpm_reg_fit->GetParError(0);





  Int_t nent = agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->Draw("agg_5bpm.eigen_dit_asym_atl1_mean:pow(agg_5bpm.eigen_dit_asym_atl1_mean_error/agg_5bpm.eigen_dit_asym_main_det_mean_error,2):agg_5bpm.eigen_dit_asym_main_det_mean_error:run_number+0.1*minirun_n","rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag == 0 && rcdb_flip_state<=2 && ( !(run_number==6564 && minirun_n==4) && !(run_number==6567 && (minirun_n==2 || minirun_n==4)) && !(run_number==6571 && (minirun_n==3 || minirun_n==4)) && !(run_number==6593 && minirun_n==2) && !(run_number==6983 && minirun_n==8) && !(run_number==7149 && minirun_n==6) && !(run_number==7211 && minirun_n==4) && !(run_number==7889 && minirun_n==0) && !(run_number==7942 && minirun_n==5) && !(run_number==8036 && minirun_n==2) && !(run_number==8240 && minirun_n==1) && !(run_number==8549 && (minirun_n==0 || minirun_n==1 || minirun_n==4))) && rcdb_slug<=185","goff") ;
  TGraphErrors * atl1 = new TGraphErrors(nent,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV4(),agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV2(),0,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV3());

  atl1->Fit("pol0");
  TF1* atl1_5bpm_dit_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t atl1_5bpm_dit_scalefactor = sqrt(atl1_5bpm_dit_fit->GetParameter(0));

  nent = agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->Draw(Form("0:eigen_lagr_asym_atl1_mean:sqrt(%e)*eigen_lagr_asym_main_det_mean_error:run_number+0.1*minirun_n",atl1_5bpm_dit_scalefactor),"rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag == 0 && rcdb_flip_state<=2 && ( !(run_number==6564 && minirun_n==4) && !(run_number==6567 && (minirun_n==2 || minirun_n==4)) && !(run_number==6571 && (minirun_n==3 || minirun_n==4)) && !(run_number==6593 && minirun_n==2) && !(run_number==6983 && minirun_n==8) && !(run_number==7149 && minirun_n==6) && !(run_number==7211 && minirun_n==4) && !(run_number==7889 && minirun_n==0) && !(run_number==7942 && minirun_n==5) && !(run_number==8036 && minirun_n==2) && !(run_number==8240 && minirun_n==1) && !(run_number==8549 && (minirun_n==0 || minirun_n==1 || minirun_n==4))) && rcdb_slug<=185","goff") ;
  TGraphErrors * atl1_5bpm_dit_mean = new TGraphErrors(nent,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV4(),agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV2(),0,agg_IncludeBMOD_part_avgd_allbpms_no_err_friendable->GetV3());
  atl1_5bpm_dit_mean->Fit("pol0");

  Double_t atl1_5bpm_dit_mean     = atl1_5bpm_dit_fit->GetParameter(0);
  Double_t atl1_5bpm_dit_mean_err = atl1_5bpm_dit_fit->GetParError(0);


}

#include <iostream>
#include <fstream>
void slugwise_drl_diffs(TString cut_ana = "OnlyBMOD", TString type = "part", TString do_err = "_no_err", Int_t slugnum = 0) {
  Printf("\n\nAnalysis %s avgd, %s, %s data set comparison to ErrorFlag\n\n\n",type.Data(),do_err.Data(),cut_ana.Data());
  TString bpms = "_allbpms";
  TString fivebpms = "_5bpms";
  TString nbpms = "allbpms";

  // New way
  TChain * agg_part_avgd_friendable = new TChain(Form("agg"));
  agg_part_avgd_friendable->Add("/lustre19/expphy/volatile/halla/parity/crex-respin2/aggRootfiles/"+cut_ana+"_drl"+do_err+"_"+type+"_avg"+bpms+"/slugRootfiles/CREX-All-miniruns_units_maindet.root");

  TChain * agg_part_avgd_ErrorFlag_friendable = new TChain(Form("agg"));
  agg_part_avgd_ErrorFlag_friendable->Add("/lustre19/expphy/volatile/halla/parity/crex-respin2/aggRootfiles/ErrorFlag_drl"+do_err+"_"+type+"_avg"+bpms+"/slugRootfiles/CREX-All-miniruns_units_maindet.root");
  agg_part_avgd_friendable->AddFriend(agg_part_avgd_ErrorFlag_friendable,Form("agg_ErrorFlag_%s_avgd%s%s_friendable",type.Data(),bpms.Data(),do_err.Data()));

  TChain * agg_part_avgd_other_friendable = new TChain(Form("agg"));
  agg_part_avgd_other_friendable->Add("/lustre19/expphy/volatile/halla/parity/crex-respin2/aggRootfiles/"+cut_ana+"_drl"+do_err+"_"+type+"_avg"+bpms+"/slugRootfiles/CREX-All-miniruns_units_maindet.root");
  agg_part_avgd_friendable->AddFriend(agg_part_avgd_other_friendable,Form("agg_%s_%s_avgd%s%s_friendable",cut_ana.Data(),type.Data(),bpms.Data(),do_err.Data()));

  TChain * agg_part_avgd_fivebpms_ErrorFlag_friendable = new TChain(Form("agg"));
  agg_part_avgd_fivebpms_ErrorFlag_friendable->Add("/lustre19/expphy/volatile/halla/parity/crex-respin2/aggRootfiles/ErrorFlag_drl"+do_err+"_"+type+"_avg/slugRootfiles/CREX-All-miniruns_units_maindet.root");
  agg_part_avgd_friendable->AddFriend(agg_part_avgd_fivebpms_ErrorFlag_friendable,Form("agg_ErrorFlag_%s_avgd%s%s_friendable",type.Data(),fivebpms.Data(),do_err.Data()));

  TChain * agg_part_avgd_fivebpms_other_friendable = new TChain(Form("agg"));
  agg_part_avgd_fivebpms_other_friendable->Add("/lustre19/expphy/volatile/halla/parity/crex-respin2/aggRootfiles/"+cut_ana+"_drl"+do_err+"_"+type+"_avg/slugRootfiles/CREX-All-miniruns_units_maindet.root");
  agg_part_avgd_friendable->AddFriend(agg_part_avgd_fivebpms_other_friendable,Form("agg_%s_%s_avgd%s%s_friendable",cut_ana.Data(),type.Data(),fivebpms.Data(),do_err.Data()));

  // Old way - Minifriended procedure version
  //TChain * agg_part_avgd_friendable = new TChain(Form("agg_"+cut_ana+"_%s_avgd%s%s_friendable",type.Data(),bpms.Data(),do_err.Data()));
  //agg_part_avgd_friendable->Add("processed_respin2_data/mini_friended_agg.root");

  TChain * agg_plain_friendable = new TChain("agg_plain_friendable");
  agg_plain_friendable->Add("processed_respin2_data/mini_friended_agg.root");

  agg_part_avgd_friendable->SetScanField(0);
  agg_part_avgd_ErrorFlag_friendable->SetScanField(0);
  agg_part_avgd_other_friendable->SetScanField(0);
  agg_plain_friendable->SetScanField(0);

  TCut miniruns_cut = "(!(run_number==6564 && minirun_n==4) && !(run_number==6567 && (minirun_n==2 || minirun_n==4)) && !(run_number==6571 && (minirun_n==3 || minirun_n==4)) && !(run_number==6593 && minirun_n==2) && !(run_number==6983 && minirun_n==8) && !(run_number==7149 && minirun_n==6) && !(run_number==7211 && minirun_n==4) && !(run_number==7889 && minirun_n==0) && !(run_number==7942 && minirun_n==5) && !(run_number==8036 && minirun_n==2) && !(run_number==8240 && minirun_n==1) && !(run_number==8549 && (minirun_n==0 || minirun_n==1 || minirun_n==4)))";

  //TCut cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_flip_state>2";  // AT data only
  TCut cut = miniruns_cut + (TCut)Form("rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_flip_state<=2 && agg_%s_%s_avgd%s%s_friendable.eigen_lagr_asym_main_det_nentries>=5",cut_ana.Data(),type.Data(),bpms.Data(),do_err.Data());
  TCut comparable_cut = miniruns_cut + (TCut)Form("rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_flip_state<=2 && agg_%s_%s_avgd%s%s_friendable.eigen_lagr_asym_main_det_nentries>=5 && (agg_%s_%s_avgd%s%s_friendable.eigen_lagr_asym_main_det_nentries != agg_ErrorFlag_%s_avgd%s%s_friendable.eigen_lagr_asym_main_det_nentries) && (run_number!=6564 && minirun_n!=4)",cut_ana.Data(),type.Data(),bpms.Data(),do_err.Data(),cut_ana.Data(),type.Data(),bpms.Data(),do_err.Data(),type.Data(),bpms.Data(),do_err.Data());
  //TString suffix = do_err+"_"+cut_ana;
  TString suffix = do_err+"_"+cut_ana + "_Cleaned";
  TString fname = Form("plots/drl_diff_plots/drl_diffs_outputs_%s_avgd%s%s.txt",type.Data(),bpms.Data(),suffix.Data());
  if (slugnum != 0) {
    cut = (TCut)(Form("rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_flip_state<=2 && rcdb_slug == %d",slugnum));
    //suffix = do_err + "_" + cut_ana + (TString)Form("_slug%d",slugnum);
    suffix = do_err + "_" + cut_ana + (TString)Form("_slug%d",slugnum) + "_Cleaned";
    fname = Form("plots/drl_diff_plots/drl_diffs_outputs_%s_avgd%s%s_slug%d.txt",type.Data(),bpms.Data(),suffix.Data(),slugnum);
  }
  //TCut cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0 && rcdb_flip_state<=2"; // FIXME this explicitly assumes we are only looking a US_AVG
  //TCut cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0 && ((rcdb_slug != 142 && rcdb_slug != 188 && rcdb_slug != 189 && rcdb_slug != 190 && rcdb_slug != 192))";


  TVirtualPad * p1;
  Int_t nen = 0;
  //TCanvas* c_eigreg_eigdit = new TCanvas();
  TCanvas* c_eig_cut_eig_rms = new TCanvas();
  TCanvas* c_eig_cut_eig_rms_ErrorFlag = new TCanvas();
  TCanvas* c_eig_cut_eig_rms_diff = new TCanvas();
  TCanvas* c_eig_cut_eig_rms_ratio = new TCanvas();
  TCanvas* c_eig_cut_eig = new TCanvas();
  TCanvas* c_eig_cut_eig_maindet = new TCanvas();
  TCanvas* c_eiglagr_eigreg = new TCanvas();
  //TCanvas* c_eiglagr_eigdit = new TCanvas();
  //TCanvas* c_regreg_eigdit = new TCanvas();
  //TCanvas* c_regreg_eigreg = new TCanvas();

  //c_eigreg_eigdit->Divide(2,2);
  c_eig_cut_eig_rms->Divide(2,3);
  c_eig_cut_eig_rms_ErrorFlag->Divide(2,3);
  c_eig_cut_eig_rms_diff->Divide(2,3);
  c_eig_cut_eig_rms_ratio->Divide(2,3);
  c_eig_cut_eig->Divide(2,3);
  c_eig_cut_eig_maindet->Divide(2,3);
  c_eiglagr_eigreg->Divide(2,2);
  //c_eiglagr_eigdit->Divide(2,2);
  //c_regreg_eigdit->Divide(2,2);
  //c_regreg_eigreg->Divide(2,3);


  /*
     TCanvas* c_eigdit_RMS_1 = new TCanvas();
     TCanvas* c_eigdit_RMS_2 = new TCanvas();

     c_eigdit_RMS_1->Divide(2,2);
     c_eigdit_RMS_2->Divide(2,2);
     */

  gStyle->SetOptFit(1);
  std::ofstream outfile0;
  outfile0.open(fname,std::ofstream::app);

  /*
     if (!bpms.Contains("all")) {
     c_eigreg_eigdit->cd(1);
     gPad->SetLeftMargin(0.125);
     nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_main_det_eigen_dit_eigen_reg_diff_mean)/1e-9:asym_main_det_eigen_dit_eigen_reg_diff_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
     TGraphErrors* tg_eigreg_eigdit_multipletwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
     tg_eigreg_eigdit_multipletwise_self->SetNameTitle("Diff Multipletwise Eigen reg vs. Eigen dit self","Difference multipletwise of eigen reg vs. eigen dit, self weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
     tg_eigreg_eigdit_multipletwise_self->Fit("pol0");
     tg_eigreg_eigdit_multipletwise_self->Draw("ap");
     TF1* tg_eigreg_eigdit_multipletwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
     Printf("tg_eigreg_eigdit_multipletwise_self = %e +- %e",tg_eigreg_eigdit_multipletwise_self_fit->GetParameter(0),tg_eigreg_eigdit_multipletwise_self_fit->GetParError(0));
     outfile0 << "eigreg_eigdit_multipletwise_self = " << tg_eigreg_eigdit_multipletwise_self_fit->GetParameter(0) << " +- " << tg_eigreg_eigdit_multipletwise_self_fit->GetParError(0) << std::endl;

     c_eigreg_eigdit->cd(2);
     gStyle->SetOptStat(122221);
     gPad->SetLeftMargin(0.125);
     agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(asym_main_det_eigen_dit_eigen_reg_diff_mean)/asym_main_det_eigen_dit_eigen_reg_diff_mean_error",cut+((TCut)("abs(rcdb_sign*(asym_main_det_eigen_dit_eigen_reg_diff_mean)/asym_main_det_eigen_dit_eigen_reg_diff_mean_error)>4.0")));
     gPad->SetLogy(1);
     nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_main_det_eigen_dit_eigen_reg_diff_mean)/asym_main_det_eigen_dit_eigen_reg_diff_mean_error",cut,"");
     TH1F* tg_eigreg_eigdit_pull_self = (TH1F*)gPad->FindObject("htemp");
     if (tg_eigreg_eigdit_pull_self) {
     tg_eigreg_eigdit_pull_self->SetNameTitle("Diff Multipletwise Eigen reg vs. Eigen dit self Pull","Difference multipletwise pull of eigen reg vs. eigen dit, self weighted;multipletwise difference pull");
     Printf("tg_eigreg_eigdit_pull_self = %e +- %e",tg_eigreg_eigdit_pull_self->GetMean(),tg_eigreg_eigdit_pull_self->GetRMS());
     outfile0 << "eigreg_eigdit_pull_self = " << tg_eigreg_eigdit_pull_self->GetMean() << " +- " << tg_eigreg_eigdit_pull_self->GetRMS() << std::endl;
     }

     c_eigreg_eigdit->cd(3);
     gPad->SetLogy(0);
     gPad->SetLeftMargin(0.125);
     nen = agg_part_avgd_friendable->Draw("pow(asym_main_det_eigen_dit_eigen_reg_diff_mean_error/reg_asym_main_det_mean_error,2):reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
     TGraphErrors* tg_eigreg_eigdit_multipletwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
     tg_eigreg_eigdit_multipletwise_scalefactor->Fit("pol0");
     TF1* tg_eigreg_eigdit_multipletwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
     Double_t eigreg_eigdit_multipletwise_scalefactor = sqrt(tg_eigreg_eigdit_multipletwise_scalefactor_fit->GetParameter(0));

     nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_main_det_eigen_dit_eigen_reg_diff_mean)/1e-9:%e*reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",eigreg_eigdit_multipletwise_scalefactor),cut,"goff");
     TGraphErrors* tg_eigreg_eigdit_multipletwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
     tg_eigreg_eigdit_multipletwise_maindet->SetNameTitle("Diff Multipletwise Eigen reg vs. Eigen dit maindet","Difference multipletwise of eigen reg vs. eigen dit, maindet weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
     tg_eigreg_eigdit_multipletwise_maindet->Fit("pol0");
     tg_eigreg_eigdit_multipletwise_maindet->Draw("ap");
     TF1* tg_eigreg_eigdit_multipletwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
     Printf("tg_eigreg_eigdit_multipletwise_maindet = %e +- %e",tg_eigreg_eigdit_multipletwise_maindet_fit->GetParameter(0),tg_eigreg_eigdit_multipletwise_maindet_fit->GetParError(0));
     outfile0 << "eigreg_eigdit_multipletwise_maindet = " << tg_eigreg_eigdit_multipletwise_maindet_fit->GetParameter(0) << " +- " << tg_eigreg_eigdit_multipletwise_maindet_fit->GetParError(0) << std::endl;

     c_eigreg_eigdit->cd(4);
     gStyle->SetOptStat(122221);
     gPad->SetLeftMargin(0.125);
     nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_main_det_eigen_dit_eigen_reg_diff_mean)/(%e*reg_asym_main_det_mean_error)",eigreg_eigdit_multipletwise_scalefactor),cut,"");
     TH1F* tg_eigreg_eigdit_pull_maindet = (TH1F*) gPad->FindObject("htemp");
     if (tg_eigreg_eigdit_pull_maindet) {
     tg_eigreg_eigdit_pull_maindet->SetNameTitle("Diff Multipletwise Eigen reg vs. Eigen dit maindet Pull","Difference multipletwise pull of eigen reg vs. eigen dit, maindet weighted;multipletwise difference pull");
     Printf("tg_eigreg_eigdit_pull_maindet = %e +- %e",tg_eigreg_eigdit_pull_maindet->GetMean(),tg_eigreg_eigdit_pull_maindet->GetRMS());
     outfile0 << "eigreg_eigdit_pull_maindet = " << tg_eigreg_eigdit_pull_maindet->GetMean() << " +- " << tg_eigreg_eigdit_pull_maindet->GetRMS() << std::endl;
     }

  //c_eigreg_eigdit->SaveAs(Form("plots/drl_diff_plots/eigen_reg_eigen_dit_diffs_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));
  c_eigreg_eigdit->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf(",type.Data(),bpms.Data(),suffix.Data()));
  }
  */

  p1 = c_eig_cut_eig_rms->cd(1);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms/1e-6:run_number+0.1*minirun_n",cut,"goff");
  TGraph * tg1 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg1->SetNameTitle("agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms","agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms;Run+0.1*Minirun;RMS (ppm)");
  tg1->Draw("ap");
  p1->Modified();
  p1->Update();

  p1 = c_eig_cut_eig_rms->cd(2);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms/1e-6",cut,"");

  p1 = c_eig_cut_eig_rms->cd(3);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms/1e-6:run_number+0.1*minirun_n",cut,"");
  TGraph * tg3 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg3->SetNameTitle("agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms","agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms;Run+0.1*Minirun;RMS (ppm)");
  tg3->Draw("ap");

  p1 = c_eig_cut_eig_rms->cd(4);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms/1e-6",cut,"");

  p1 = c_eig_cut_eig_rms->cd(5);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms/1e-6:run_number+0.1*minirun_n",cut,"");
  TGraph * tg5 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg5->SetNameTitle("agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms","agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms;Run+0.1*Minirun;RMS (ppm)");
  tg5->Draw("ap");

  p1 = c_eig_cut_eig_rms->cd(6);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms/1e-6",cut,"");

  c_eig_cut_eig_rms->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf(",type.Data(),bpms.Data(),suffix.Data()));


  p1 = c_eig_cut_eig_rms_ErrorFlag->cd(1);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms/1e-6:run_number+0.1*minirun_n",cut,"");
  TGraph * tg2 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg2->SetNameTitle("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms","agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms;Run+0.1*Minirun;RMS (ppm)");
  tg2->Draw("ap");

  p1 = c_eig_cut_eig_rms_ErrorFlag->cd(2);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms/1e-6",cut,"");

  p1 = c_eig_cut_eig_rms_ErrorFlag->cd(3);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms/1e-6:run_number+0.1*minirun_n",cut,"");
  TGraph * tg4 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg4->SetNameTitle("agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms","agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms;Run+0.1*Minirun;RMS (ppm)");
  tg4->Draw("ap");

  p1 = c_eig_cut_eig_rms_ErrorFlag->cd(4);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms/1e-6",cut,"");

  p1 = c_eig_cut_eig_rms_ErrorFlag->cd(5);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms/1e-6:run_number+0.1*minirun_n",cut,"");
  TGraph * tg6 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg6->SetNameTitle("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms","agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms;Run+0.1*Minirun;RMS (ppm)");
  tg6->Draw("ap");

  p1 = c_eig_cut_eig_rms_ErrorFlag->cd(6);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms/1e-6",cut,"");


  c_eig_cut_eig_rms_ErrorFlag->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));

  p1 = c_eig_cut_eig_rms_diff->cd(1);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms)/1e-6:run_number+0.1*minirun_n",comparable_cut,"");
  TGraph * tg7 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg7->SetNameTitle("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms diff","agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms diff;Run+0.1*Minirun;RMS difference (ppm)");
  tg7->Draw("ap");
                                                                                                                            
  p1 = c_eig_cut_eig_rms_diff->cd(2);                                                                                                 
  gStyle->SetOptStat(1221);                                                                                                 
  p1->SetLeftMargin(0.125);                                                                                               
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms)/1e-6",comparable_cut,"");

  p1 = c_eig_cut_eig_rms_diff->cd(3);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms)/1e-6:run_number+0.1*minirun_n",comparable_cut,"");
  TGraph * tg9 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg9->SetNameTitle("agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms diff","agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms diff;Run+0.1*Minirun;RMS difference (ppm)");
  tg9->Draw("ap");
                                                                                                                               
  p1 = c_eig_cut_eig_rms_diff->cd(4);                                                                                                    
  gStyle->SetOptStat(1221);                                                                                                    
  p1->SetLeftMargin(0.125);                                                                                                  
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms)/1e-6",comparable_cut,"");

  p1 = c_eig_cut_eig_rms_diff->cd(5);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms)/1e-6:run_number+0.1*minirun_n",comparable_cut,"");
  TGraph * tg11 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg11->SetNameTitle("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms diff","agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms diff;Run+0.1*Minirun;RMS difference (ppm)");
  tg11->Draw("ap");
                                                                                                                           
  p1 = c_eig_cut_eig_rms_diff->cd(6);                                                                                                
  gStyle->SetOptStat(1221);                                                                                                
  p1->SetLeftMargin(0.125);                                                                                              
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms)/1e-6",comparable_cut,"");
  Printf("Difference in main det RMS between BMOD data set and standard ErrorFlag dataset, ppm, > 350 ppm difference");
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms)/1e-6",comparable_cut+((TCut)("abs(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms)/1e-6>350")),"");


  c_eig_cut_eig_rms_diff->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));


  p1 = c_eig_cut_eig_rms_diff->cd(1);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms/agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms):run_number+0.1*minirun_n",comparable_cut,"");
  TGraph * tg8 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg8->SetNameTitle("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms ratio","agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms ratio;Run+0.1*Minirun;RMS ratio");
  tg8->Draw("ap");
                                                                                                                            
  p1 = c_eig_cut_eig_rms_diff->cd(2);                                                                                                 
  gStyle->SetOptStat(1221);                                                                                                 
  p1->SetLeftMargin(0.125);                                                                                               
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms/agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_rms)",comparable_cut,"");

  p1 = c_eig_cut_eig_rms_diff->cd(3);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms/agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms):run_number+0.1*minirun_n",comparable_cut,"");
  TGraph * tg10 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg10->SetNameTitle("agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms ratio","agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms ratio;Run+0.1*Minirun;RMS ratio");
  tg10->Draw("ap");
                                                                                                                               
  p1 = c_eig_cut_eig_rms_diff->cd(4);                                                                                                    
  gStyle->SetOptStat(1221);                                                                                                    
  p1->SetLeftMargin(0.125);                                                                                                  
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms/agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_rms)",comparable_cut,"");

  p1 = c_eig_cut_eig_rms_diff->cd(5);
  gStyle->SetOptStat(1221);
  p1->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms/agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms):run_number+0.1*minirun_n",comparable_cut,"");
  TGraph * tg12 = new TGraph(nen,agg_part_avgd_friendable->GetV2(),agg_part_avgd_friendable->GetV1());
  tg12->SetNameTitle("agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms ratio","agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms ratio;Run+0.1*Minirun;RMS ratio");
  tg12->Draw("ap");
                                                                                                                           
  p1 = c_eig_cut_eig_rms_diff->cd(6);                                                                                                
  gStyle->SetOptStat(1221);                                                                                                
  p1->SetLeftMargin(0.125);                                                                                              
  nen = agg_part_avgd_friendable->Draw("(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms/agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_rms)",comparable_cut,"");


  c_eig_cut_eig_rms_diff->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));


  c_eig_cut_eig->cd(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean)/1e-9:sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2)))/1e-9:run_number+0.1*minirun_n",comparable_cut,"goff");
  TGraphErrors* tg_eiglagr_cut_eiglagr_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eiglagr_cut_eiglagr_minirunwise_self->SetNameTitle("Minirunwise diff "+cut_ana+" comparable_cut eigen lagr vs. ErrorFlag eigen lagr self","Minirunwise difference of "+cut_ana+" comparable_cut eigen lagr vs. ErrorFlag eigen lagr, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_eiglagr_cut_eiglagr_minirunwise_self->Fit("pol0");
  tg_eiglagr_cut_eiglagr_minirunwise_self->Draw("ap");
  TF1* tg_eiglagr_cut_eiglagr_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eiglagr_cut_eiglagr_minirunwise_self = %e +- %e",tg_eiglagr_cut_eiglagr_minirunwise_self_fit->GetParameter(0),tg_eiglagr_cut_eiglagr_minirunwise_self_fit->GetParError(0));
  outfile0 << "eiglagr_cut_eiglagr_minirunwise_self = " << tg_eiglagr_cut_eiglagr_minirunwise_self_fit->GetParameter(0) << " +- " << tg_eiglagr_cut_eiglagr_minirunwise_self_fit->GetParError(0) << std::endl;

  c_eig_cut_eig->cd(2);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  Printf("\n\n");
  Printf("Difference in main det mean value PULL (quadrature error difference) between BMOD data set and standard ErrorFlag dataset lagrange 12bpm data, pull > 4");
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2))) ",comparable_cut+((TCut)("abs(rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2))))>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2)))",comparable_cut,"");
  TH1F* tg_eiglagr_cut_eiglagr_pull_self = (TH1F*)gPad->FindObject("htemp");
  if (tg_eiglagr_cut_eiglagr_pull_self) {
    tg_eiglagr_cut_eiglagr_pull_self->SetNameTitle("Diff Multipletwise "+cut_ana+" comparable_cut eigen lagr vs. ErrorFlag eigen lagr self Pull","Difference multipletwise pull of "+cut_ana+" comparable_cut eigen lagr vs. ErrorFlag eigen lagr, self weighted;multipletwise difference pull");
    Printf("tg_eiglagr_cut_eiglagr_pull_self = %e +- %e",tg_eiglagr_cut_eiglagr_pull_self->GetMean(),tg_eiglagr_cut_eiglagr_pull_self->GetRMS());
    outfile0 << "eiglagr_cut_eiglagr_pull_self = " << tg_eiglagr_cut_eiglagr_pull_self->GetMean() << " +- " << tg_eiglagr_cut_eiglagr_pull_self->GetRMS() << std::endl;
  }

  c_eig_cut_eig->cd(3);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean)/1e-9:sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)))/1e-9:run_number+0.1*minirun_n",comparable_cut+(TCut)("rcdb_arm_flag>=0"),"goff");
  TGraphErrors* tg_eigdit_cut_eigdit_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigdit_cut_eigdit_minirunwise_self->SetNameTitle("Minirunwise diff "+cut_ana+" comparable_cut eigen dit vs. ErrorFlag eigen dit self","Minirunwise difference of "+cut_ana+" comparable_cut eigen dit vs. ErrorFlag eigen dit, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_eigdit_cut_eigdit_minirunwise_self->Fit("pol0");
  tg_eigdit_cut_eigdit_minirunwise_self->Draw("ap");
  TF1* tg_eigdit_cut_eigdit_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eigdit_cut_eigdit_minirunwise_self = %e +- %e",tg_eigdit_cut_eigdit_minirunwise_self_fit->GetParameter(0),tg_eigdit_cut_eigdit_minirunwise_self_fit->GetParError(0));
  outfile0 << "eigdit_cut_eigdit_minirunwise_self = " << tg_eigdit_cut_eigdit_minirunwise_self_fit->GetParameter(0) << " +- " << tg_eigdit_cut_eigdit_minirunwise_self_fit->GetParError(0) << std::endl;

  c_eig_cut_eig->cd(4);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  Printf("\n\n");
  Printf("Difference in main det mean value PULL (quadrature error difference) between BMOD data set and standard ErrorFlag dataset 5bpm dithering, pull > 4");
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)))",comparable_cut+((TCut)("abs(rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2))))>4 && rcdb_arm_flag>=0")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)))",comparable_cut+(TCut)("rcdb_arm_flag>=0"),"");
  TH1F* tg_eigdit_cut_eigdit_pull_self = (TH1F*)gPad->FindObject("htemp");
  if (tg_eigdit_cut_eigdit_pull_self) {
    tg_eigdit_cut_eigdit_pull_self->SetNameTitle("Diff Multipletwise "+cut_ana+" comparable_cut eigen dit vs. ErrorFlag eigen dit self Pull","Difference multipletwise pull of "+cut_ana+" comparable_cut eigen dit vs. ErrorFlag eigen dit, self weighted;multipletwise difference pull");
    Printf("tg_eigdit_cut_eigdit_pull_self = %e +- %e",tg_eigdit_cut_eigdit_pull_self->GetMean(),tg_eigdit_cut_eigdit_pull_self->GetRMS());
    outfile0 << "eigdit_cut_eigdit_pull_self = " << tg_eigdit_cut_eigdit_pull_self->GetMean() << " +- " << tg_eigdit_cut_eigdit_pull_self->GetRMS() << std::endl;
  }

  c_eig_cut_eig->cd(5);
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/1e-9:sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)))/1e-9:run_number+0.1*minirun_n",comparable_cut,"goff");
  TGraphErrors* tg_eigreg_cut_eigreg_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigreg_cut_eigreg_minirunwise_self->SetNameTitle("Minirunwise diff "+cut_ana+" comparable_cut eigen reg vs. ErrorFlag eigen reg self","Minirunwise difference of "+cut_ana+" comparable_cut eigen reg vs. ErrorFlag eigen reg, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_eigreg_cut_eigreg_minirunwise_self->Fit("pol0");
  tg_eigreg_cut_eigreg_minirunwise_self->Draw("ap");
  TF1* tg_eigreg_cut_eigreg_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eigreg_cut_eigreg_minirunwise_self = %e +- %e",tg_eigreg_cut_eigreg_minirunwise_self_fit->GetParameter(0),tg_eigreg_cut_eigreg_minirunwise_self_fit->GetParError(0));
  outfile0 << "eigreg_cut_eigreg_minirunwise_self = " << tg_eigreg_cut_eigreg_minirunwise_self_fit->GetParameter(0) << " +- " << tg_eigreg_cut_eigreg_minirunwise_self_fit->GetParError(0) << std::endl;

  c_eig_cut_eig->cd(6);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  Printf("\n\n");
  Printf("Difference in main det mean value PULL (quadrature error difference) between BMOD data set and standard ErrorFlag dataset 12bpm regression, pull > 4");
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)))",comparable_cut+((TCut)("abs(rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2))))>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)))",comparable_cut,"");
  TH1F* tg_eigreg_cut_eigreg_pull_self = (TH1F*) gPad->FindObject("htemp");
  if (tg_eigreg_cut_eigreg_pull_self) {
    tg_eigreg_cut_eigreg_pull_self->SetNameTitle("Diff Multipletwise "+cut_ana+" comparable_cut eigen reg vs. ErrorFlag eigen reg self Pull","Difference multipletwise pull of "+cut_ana+" comparable_cut eigen reg vs. ErrorFlag eigen reg, self weighted;multipletwise difference pull");
    Printf("tg_eigreg_cut_eigreg_pull_self = %e +- %e",tg_eigreg_cut_eigreg_pull_self->GetMean(),tg_eigreg_cut_eigreg_pull_self->GetRMS());
    outfile0 << "eigreg_cut_eigreg_pull_self = " << tg_eigreg_cut_eigreg_pull_self->GetMean() << " +- " << tg_eigreg_cut_eigreg_pull_self->GetRMS() << std::endl;
  }
  else {
    Printf("Failed reg v reg comparable_cut check pull plot");
  }

  c_eig_cut_eig->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));
  gPad->SetLogy(0);






  c_eig_cut_eig_maindet->cd(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2)))/agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2):agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",comparable_cut,"goff");
  TGraphErrors* tg_eiglagr_cut_eiglagr_multipletwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eiglagr_cut_eiglagr_multipletwise_scalefactor->Fit("pol0");
  TF1* tg_eiglagr_cut_eiglagr_multipletwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t eiglagr_cut_eiglagr_multipletwise_scalefactor = sqrt(tg_eiglagr_cut_eiglagr_multipletwise_scalefactor_fit->GetParameter(0));
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean)/1e-9:"+((TString)Form("%e",eiglagr_cut_eiglagr_multipletwise_scalefactor))+"*agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",comparable_cut,"goff");
  TGraphErrors* tg_eiglagr_cut_eiglagr_minirunwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eiglagr_cut_eiglagr_minirunwise_maindet->SetNameTitle("Minirunwise diff "+cut_ana+" comparable_cut eigen lagr vs. ErrorFlag eigen lagr maindet","Minirunwise difference of "+cut_ana+" comparable_cut eigen lagr vs. ErrorFlag eigen lagr, maindet weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_eiglagr_cut_eiglagr_minirunwise_maindet->Fit("pol0");
  tg_eiglagr_cut_eiglagr_minirunwise_maindet->Draw("ap");
  TF1* tg_eiglagr_cut_eiglagr_minirunwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eiglagr_cut_eiglagr_minirunwise_maindet = %e +- %e",tg_eiglagr_cut_eiglagr_minirunwise_maindet_fit->GetParameter(0),tg_eiglagr_cut_eiglagr_minirunwise_maindet_fit->GetParError(0));
  outfile0 << "eiglagr_cut_eiglagr_minirunwise_maindet = " << tg_eiglagr_cut_eiglagr_minirunwise_maindet_fit->GetParameter(0) << " +- " << tg_eiglagr_cut_eiglagr_minirunwise_maindet_fit->GetParError(0) << std::endl;

  c_eig_cut_eig_maindet->cd(2);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  Printf("\n\n");
  Printf("Difference in main det mean value PULL (quadrature error difference) between BMOD data set and standard ErrorFlag dataset 12bpm lagrange, MAINDET weighted, pull > 4");
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean)/("+((TString)Form("%e",eiglagr_cut_eiglagr_multipletwise_scalefactor))+"*agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error)",comparable_cut+((TCut)("abs(rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error,2))))>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean)/("+((TString)Form("%e",eiglagr_cut_eiglagr_multipletwise_scalefactor))+"*agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_lagr_asym_main_det_mean_error)",comparable_cut,"");
  TH1F* tg_eiglagr_cut_eiglagr_pull_maindet = (TH1F*)gPad->FindObject("htemp");
  if (tg_eiglagr_cut_eiglagr_pull_maindet) {
    tg_eiglagr_cut_eiglagr_pull_maindet->SetNameTitle("Diff Multipletwise "+cut_ana+" comparable_cut eigen lagr vs. ErrorFlag eigen lagr maindet Pull","Difference multipletwise pull of "+cut_ana+" comparable_cut eigen lagr vs. ErrorFlag eigen lagr, maindet weighted;multipletwise difference pull");
    Printf("tg_eiglagr_cut_eiglagr_pull_maindet = %e +- %e",tg_eiglagr_cut_eiglagr_pull_maindet->GetMean(),tg_eiglagr_cut_eiglagr_pull_maindet->GetRMS());
    outfile0 << "eiglagr_cut_eiglagr_pull_maindet = " << tg_eiglagr_cut_eiglagr_pull_maindet->GetMean() << " +- " << tg_eiglagr_cut_eiglagr_pull_maindet->GetRMS() << std::endl;
  }

  c_eig_cut_eig_maindet->cd(3);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)))/agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2):agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",comparable_cut,"goff");
  TGraphErrors* tg_eigdit_cut_eigdit_multipletwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigdit_cut_eigdit_multipletwise_scalefactor->Fit("pol0");
  TF1* tg_eigdit_cut_eigdit_multipletwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t eigdit_cut_eigdit_multipletwise_scalefactor = sqrt(tg_eigdit_cut_eigdit_multipletwise_scalefactor_fit->GetParameter(0));
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean)/1e-9:"+((TString)Form("%e",eigdit_cut_eigdit_multipletwise_scalefactor))+"*agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",comparable_cut,"goff");
  TGraphErrors* tg_eigdit_cut_eigdit_minirunwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigdit_cut_eigdit_minirunwise_maindet->SetNameTitle("Minirunwise diff "+cut_ana+" comparable_cut eigen dit vs. ErrorFlag eigen dit maindet","Minirunwise difference of "+cut_ana+" comparable_cut eigen dit vs. ErrorFlag eigen dit, maindet weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_eigdit_cut_eigdit_minirunwise_maindet->Fit("pol0");
  tg_eigdit_cut_eigdit_minirunwise_maindet->Draw("ap");
  TF1* tg_eigdit_cut_eigdit_minirunwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eigdit_cut_eigdit_minirunwise_maindet = %e +- %e",tg_eigdit_cut_eigdit_minirunwise_maindet_fit->GetParameter(0),tg_eigdit_cut_eigdit_minirunwise_maindet_fit->GetParError(0));
  outfile0 << "eigdit_cut_eigdit_minirunwise_maindet = " << tg_eigdit_cut_eigdit_minirunwise_maindet_fit->GetParameter(0) << " +- " << tg_eigdit_cut_eigdit_minirunwise_maindet_fit->GetParError(0) << std::endl;

  c_eig_cut_eig_maindet->cd(4);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  Printf("\n\n");
  Printf("Difference in main det mean value PULL (quadrature error difference) between BMOD data set and standard ErrorFlag dataset 5bpm dithering, MAINDET weighted, pull > 4");
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean)/("+((TString)Form("%e",eigdit_cut_eigdit_multipletwise_scalefactor))+"*agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error)",comparable_cut+((TCut)("abs(rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error,2))))>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean)/("+((TString)Form("%e",eigdit_cut_eigdit_multipletwise_scalefactor))+"*agg_ErrorFlag_"+type+"_avgd"+fivebpms+do_err+"_friendable.eigen_dit_asym_main_det_mean_error)",comparable_cut,"");

  TH1F* tg_eigdit_cut_eigdit_pull_maindet = (TH1F*)gPad->FindObject("htemp");
  if (tg_eigdit_cut_eigdit_pull_maindet) {
    tg_eigdit_cut_eigdit_pull_maindet->SetNameTitle("Diff Multipletwise "+cut_ana+" comparable_cut eigen dit vs. ErrorFlag eigen dit maindet Pull","Difference multipletwise pull of "+cut_ana+" comparable_cut eigen dit vs. ErrorFlag eigen dit, maindet weighted;multipletwise difference pull");
    Printf("tg_eigdit_cut_eigdit_pull_maindet = %e +- %e",tg_eigdit_cut_eigdit_pull_maindet->GetMean(),tg_eigdit_cut_eigdit_pull_maindet->GetRMS());
    outfile0 << "eigdit_cut_eigdit_pull_maindet = " << tg_eigdit_cut_eigdit_pull_maindet->GetMean() << " +- " << tg_eigdit_cut_eigdit_pull_maindet->GetRMS() << std::endl;
  }

  c_eig_cut_eig_maindet->cd(5);
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)))/agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2):agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",comparable_cut,"goff");
  TGraphErrors* tg_eigreg_cut_eigreg_multipletwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigreg_cut_eigreg_multipletwise_scalefactor->Fit("pol0");
  TF1* tg_eigreg_cut_eigreg_multipletwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t eigreg_cut_eigreg_multipletwise_scalefactor = sqrt(tg_eigreg_cut_eigreg_multipletwise_scalefactor_fit->GetParameter(0));
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/1e-9:"+((TString)Form("%e",eigreg_cut_eigreg_multipletwise_scalefactor))+"*agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",comparable_cut,"goff");
  TGraphErrors* tg_eigreg_cut_eigreg_minirunwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigreg_cut_eigreg_minirunwise_maindet->SetNameTitle("Minirunwise diff "+cut_ana+" comparable_cut eigen reg vs. ErrorFlag eigen reg maindet","Minirunwise difference of "+cut_ana+" comparable_cut eigen reg vs. ErrorFlag eigen reg, maindet weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_eigreg_cut_eigreg_minirunwise_maindet->Fit("pol0");
  tg_eigreg_cut_eigreg_minirunwise_maindet->Draw("ap");
  TF1* tg_eigreg_cut_eigreg_minirunwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eigreg_cut_eigreg_minirunwise_maindet = %e +- %e",tg_eigreg_cut_eigreg_minirunwise_maindet_fit->GetParameter(0),tg_eigreg_cut_eigreg_minirunwise_maindet_fit->GetParError(0));
  outfile0 << "eigreg_cut_eigreg_minirunwise_maindet = " << tg_eigreg_cut_eigreg_minirunwise_maindet_fit->GetParameter(0) << " +- " << tg_eigreg_cut_eigreg_minirunwise_maindet_fit->GetParError(0) << std::endl;

  c_eig_cut_eig_maindet->cd(6);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  Printf("\n\n");
  Printf("Difference in main det mean value PULL (quadrature error difference) between BMOD data set and standard ErrorFlag dataset 12bpm regression, MAINDET weighted, pull > 4");
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/("+((TString)Form("%e",eigreg_cut_eigreg_multipletwise_scalefactor))+"*agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error)",comparable_cut+((TCut)("abs(rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/sqrt(abs(pow(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2))))>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_"+cut_ana+"_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/("+((TString)Form("%e",eigreg_cut_eigreg_multipletwise_scalefactor))+"*agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error)",comparable_cut,"");
  TH1F* tg_eigreg_cut_eigreg_pull_maindet = (TH1F*) gPad->FindObject("htemp");
  if (tg_eigreg_cut_eigreg_pull_maindet) {
    tg_eigreg_cut_eigreg_pull_maindet->SetNameTitle("Diff Multipletwise "+cut_ana+" comparable_cut eigen reg vs. ErrorFlag eigen reg maindet Pull","Difference multipletwise pull of "+cut_ana+" comparable_cut eigen reg vs. ErrorFlag eigen reg, maindet weighted;multipletwise difference pull");
    Printf("tg_eigreg_cut_eigreg_pull_maindet = %e +- %e",tg_eigreg_cut_eigreg_pull_maindet->GetMean(),tg_eigreg_cut_eigreg_pull_maindet->GetRMS());
    outfile0 << "eigreg_cut_eigreg_pull_maindet = " << tg_eigreg_cut_eigreg_pull_maindet->GetMean() << " +- " << tg_eigreg_cut_eigreg_pull_maindet->GetRMS() << std::endl;
  }
  else {
    Printf("Failed reg v reg comparable_cut check pull plot");
  }

  c_eig_cut_eig_maindet->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));
  gPad->SetLogy(0);





  c_eiglagr_eigreg->cd(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/1e-9:asym_main_det_eigen_lagr_eigen_reg_diff_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_eiglagr_eigreg_multipletwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eiglagr_eigreg_multipletwise_self->SetNameTitle(cut_ana+" cut, Diff Multipletwise Eigen lagr vs. Eigen reg self",cut_ana+" cut, Difference multipletwise of eigen lagr vs. eigen reg, self weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
  tg_eiglagr_eigreg_multipletwise_self->Fit("pol0");
  tg_eiglagr_eigreg_multipletwise_self->Draw("ap");
  TF1* tg_eiglagr_eigreg_multipletwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf(cut_ana+" cut, tg_eiglagr_eigreg_multipletwise_self = %e +- %e",tg_eiglagr_eigreg_multipletwise_self_fit->GetParameter(0),tg_eiglagr_eigreg_multipletwise_self_fit->GetParError(0));
  outfile0 << cut_ana+" cut, eiglagr_eigreg_multipletwise_self = " << tg_eiglagr_eigreg_multipletwise_self_fit->GetParameter(0) << " +- " << tg_eiglagr_eigreg_multipletwise_self_fit->GetParError(0) << std::endl;

  c_eiglagr_eigreg->cd(2);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  Printf("\n\n");
  Printf("Difference in main det mean value PULL (quadrature error difference) of regression vs. lagrange datasets (same cut), pull > 4");
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/asym_main_det_eigen_lagr_eigen_reg_diff_mean_error",cut+((TCut)("abs(rcdb_sign*(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/asym_main_det_eigen_lagr_eigen_reg_diff_mean_error)>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/asym_main_det_eigen_lagr_eigen_reg_diff_mean_error",cut,"");
  TH1F* tg_eiglagr_eigreg_pull_self = (TH1F*)gPad->FindObject("htemp");
  if (tg_eiglagr_eigreg_pull_self) {
    tg_eiglagr_eigreg_pull_self->SetNameTitle(cut_ana+" cut, Diff Multipletwise Eigen lagr vs. Eigen reg self Pull",cut_ana+" cut, Difference multipletwise pull of eigen lagr vs. eigen reg, self weighted;multipletwise difference pull");
    Printf(cut_ana+" cut, tg_eiglagr_eigreg_pull_self = %e +- %e",tg_eiglagr_eigreg_pull_self->GetMean(),tg_eiglagr_eigreg_pull_self->GetRMS());
    outfile0 << cut_ana+" cut, eiglagr_eigreg_pull_self = " << tg_eiglagr_eigreg_pull_self->GetMean() << " +- " << tg_eiglagr_eigreg_pull_self->GetRMS() << std::endl;
  }

  c_eiglagr_eigreg->cd(3);
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(asym_main_det_eigen_lagr_eigen_reg_diff_mean_error/reg_asym_main_det_mean_error,2):reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_eiglagr_eigreg_multipletwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eiglagr_eigreg_multipletwise_scalefactor->Fit("pol0");
  TF1* tg_eiglagr_eigreg_multipletwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t eiglagr_eigreg_multipletwise_scalefactor = sqrt(tg_eiglagr_eigreg_multipletwise_scalefactor_fit->GetParameter(0));

  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/1e-9:%e*reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",eiglagr_eigreg_multipletwise_scalefactor),cut,"goff");
  TGraphErrors* tg_eiglagr_eigreg_multipletwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eiglagr_eigreg_multipletwise_maindet->SetNameTitle(cut_ana+" cut, Diff Multipletwise Eigen lagr vs. Eigen reg maindet",cut_ana+" cut, Difference multipletwise of eigen lagr vs. eigen reg, maindet weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
  tg_eiglagr_eigreg_multipletwise_maindet->Fit("pol0");
  tg_eiglagr_eigreg_multipletwise_maindet->Draw("ap");
  TF1* tg_eiglagr_eigreg_multipletwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf(cut_ana+" cut, tg_eiglagr_eigreg_multipletwise_maindet = %e +- %e",tg_eiglagr_eigreg_multipletwise_maindet_fit->GetParameter(0),tg_eiglagr_eigreg_multipletwise_maindet_fit->GetParError(0));
  outfile0 << cut_ana+" cut, eiglagr_eigreg_multipletwise_maindet = " << tg_eiglagr_eigreg_multipletwise_maindet_fit->GetParameter(0) << " +- " << tg_eiglagr_eigreg_multipletwise_maindet_fit->GetParError(0) << std::endl;

  c_eiglagr_eigreg->cd(4);
  gStyle->SetOptStat(122221);
  gPad->SetLogy(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/(%e*reg_asym_main_det_mean_error)",eiglagr_eigreg_multipletwise_scalefactor),cut,"");
  TH1F* tg_eiglagr_eigreg_pull_maindet = (TH1F*) gPad->FindObject("htemp");
  if (tg_eiglagr_eigreg_pull_maindet) {
    tg_eiglagr_eigreg_pull_maindet->SetNameTitle(cut_ana+" cut, Diff Multipletwise Eigen lagr vs. Eigen reg maindet Pull",cut_ana+" cut, Difference multipletwise pull of eigen lagr vs. eigen reg, maindet weighted;multipletwise difference pull");
    Printf("\n\n");
    Printf(cut_ana+" cut, tg_eiglagr_eigreg_pull_maindet = %e +- %e",tg_eiglagr_eigreg_pull_maindet->GetMean(),tg_eiglagr_eigreg_pull_maindet->GetRMS());
    outfile0 << cut_ana+" cut, eiglagr_eigreg_pull_maindet = " << tg_eiglagr_eigreg_pull_maindet->GetMean() << " +- " << tg_eiglagr_eigreg_pull_maindet->GetRMS() << std::endl;
  }
  c_eiglagr_eigreg->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf)",type.Data(),bpms.Data(),suffix.Data()));


  /*
     if (!bpms.Contains("all")) {
     c_eiglagr_eigdit->cd(1);
     gPad->SetLeftMargin(0.125);
     nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_main_det_eigen_lagr_eigen_dit_diff_mean)/1e-9:asym_main_det_eigen_lagr_eigen_dit_diff_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
     TGraphErrors* tg_eiglagr_eigdit_multipletwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
     tg_eiglagr_eigdit_multipletwise_self->SetNameTitle("Diff Multipletwise Eigen lagr vs. Eigen dit self","Difference multipletwise of eigen lagr vs. eigen dit, self weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
     tg_eiglagr_eigdit_multipletwise_self->Fit("pol0");
     tg_eiglagr_eigdit_multipletwise_self->Draw("ap");
     TF1* tg_eiglagr_eigdit_multipletwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
     Printf("tg_eiglagr_eigdit_multipletwise_self = %e +- %e",tg_eiglagr_eigdit_multipletwise_self_fit->GetParameter(0),tg_eiglagr_eigdit_multipletwise_self_fit->GetParError(0));
     outfile0 << "eiglagr_eigdit_multipletwise_self = " << tg_eiglagr_eigdit_multipletwise_self_fit->GetParameter(0) << " +- " << tg_eiglagr_eigdit_multipletwise_self_fit->GetParError(0) << std::endl;

     c_eiglagr_eigdit->cd(2);
     gStyle->SetOptStat(122221);
     gPad->SetLeftMargin(0.125);
     agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(asym_main_det_eigen_lagr_eigen_dit_diff_mean)/asym_main_det_eigen_lagr_eigen_dit_diff_mean_error",cut+((TCut)("abs(rcdb_sign*(asym_main_det_eigen_lagr_eigen_dit_diff_mean)/asym_main_det_eigen_lagr_eigen_dit_diff_mean_error)>4")));
     gPad->SetLogy(1);
     nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_main_det_eigen_lagr_eigen_dit_diff_mean)/asym_main_det_eigen_lagr_eigen_dit_diff_mean_error",cut,"");
     TH1F* tg_eiglagr_eigdit_pull_self = (TH1F*)gPad->FindObject("htemp");
     if (tg_eiglagr_eigdit_pull_self) {
     tg_eiglagr_eigdit_pull_self->SetNameTitle("Diff Multipletwise Eigen lagr vs. Eigen dit self Pull","Difference multipletwise pull of eigen lagr vs. eigen dit, self weighted;multipletwise difference pull");
     Printf("tg_eiglagr_eigdit_pull_self = %e +- %e",tg_eiglagr_eigdit_pull_self->GetMean(),tg_eiglagr_eigdit_pull_self->GetRMS());
     outfile0 << "eiglagr_eigdit_pull_self = " << tg_eiglagr_eigdit_pull_self->GetMean() << " +- " << tg_eiglagr_eigdit_pull_self->GetRMS() << std::endl;
     }

     c_eiglagr_eigdit->cd(3);
     gPad->SetLogy(0);
     gPad->SetLeftMargin(0.125);
     nen = agg_part_avgd_friendable->Draw("pow(asym_main_det_eigen_lagr_eigen_dit_diff_mean_error/reg_asym_main_det_mean_error,2):reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
     TGraphErrors* tg_eiglagr_eigdit_multipletwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
     tg_eiglagr_eigdit_multipletwise_scalefactor->Fit("pol0");
     TF1* tg_eiglagr_eigdit_multipletwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
     Double_t eiglagr_eigdit_multipletwise_scalefactor = sqrt(tg_eiglagr_eigdit_multipletwise_scalefactor_fit->GetParameter(0));

     nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_main_det_eigen_lagr_eigen_dit_diff_mean)/1e-9:%e*reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",eiglagr_eigdit_multipletwise_scalefactor),cut,"goff");
     TGraphErrors* tg_eiglagr_eigdit_multipletwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
     tg_eiglagr_eigdit_multipletwise_maindet->SetNameTitle("Diff Multipletwise Eigen lagr vs. Eigen dit maindet","Difference multipletwise of eigen lagr vs. eigen dit, maindet weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
     tg_eiglagr_eigdit_multipletwise_maindet->Fit("pol0");
     tg_eiglagr_eigdit_multipletwise_maindet->Draw("ap");
     TF1* tg_eiglagr_eigdit_multipletwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
     Printf("tg_eiglagr_eigdit_multipletwise_maindet = %e +- %e",tg_eiglagr_eigdit_multipletwise_maindet_fit->GetParameter(0),tg_eiglagr_eigdit_multipletwise_maindet_fit->GetParError(0));
     outfile0 << "eiglagr_eigdit_multipletwise_maindet = " << tg_eiglagr_eigdit_multipletwise_maindet_fit->GetParameter(0) << " +- " << tg_eiglagr_eigdit_multipletwise_maindet_fit->GetParError(0) << std::endl;

     c_eiglagr_eigdit->cd(4);
     gStyle->SetOptStat(122221);
     gPad->SetLogy(1);
     gPad->SetLeftMargin(0.125);
     nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_main_det_eigen_lagr_eigen_dit_diff_mean)/(%e*reg_asym_main_det_mean_error)",eiglagr_eigdit_multipletwise_scalefactor),cut,"");
     TH1F* tg_eiglagr_eigdit_pull_maindet = (TH1F*) gPad->FindObject("htemp");
     if (tg_eiglagr_eigdit_pull_maindet) {
     tg_eiglagr_eigdit_pull_maindet->SetNameTitle("Diff Multipletwise Eigen lagr vs. Eigen dit maindet Pull","Difference multipletwise pull of eigen lagr vs. eigen dit, maindet weighted;multipletwise difference pull");
     Printf("tg_eiglagr_eigdit_pull_maindet = %e +- %e",tg_eiglagr_eigdit_pull_maindet->GetMean(),tg_eiglagr_eigdit_pull_maindet->GetRMS());
     outfile0 << "eiglagr_eigdit_pull_maindet = " << tg_eiglagr_eigdit_pull_maindet->GetMean() << " +- " << tg_eiglagr_eigdit_pull_maindet->GetRMS() << std::endl;
     }
  //c_eiglagr_eigdit->SaveAs(Form("plots/drl_diff_plots/eigen_lagr_eigen_dit_diffs_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));
  c_eiglagr_eigdit->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));


  // Now do minirunwise regular regression vs. segment avg eigen dit

  c_regreg_eigdit->cd(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_main_det_eigen_dit_reg_diff_mean)/1e-9:asym_main_det_eigen_dit_reg_diff_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_regreg_eigdit_multipletwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regreg_eigdit_multipletwise_self->SetNameTitle("Diff Multipletwise Regular reg vs. Eigen dit self","Difference multipletwise of regular reg vs. eigen dit, self weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
  tg_regreg_eigdit_multipletwise_self->Fit("pol0");
  tg_regreg_eigdit_multipletwise_self->Draw("ap");
  TF1* tg_regreg_eigdit_multipletwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_regreg_eigdit_multipletwise_self = %e +- %e",tg_regreg_eigdit_multipletwise_self_fit->GetParameter(0),tg_regreg_eigdit_multipletwise_self_fit->GetParError(0));
  outfile0 << "regreg_eigdit_multipletwise_self = " << tg_regreg_eigdit_multipletwise_self_fit->GetParameter(0) << " +- " << tg_regreg_eigdit_multipletwise_self_fit->GetParError(0) << std::endl;

  c_regreg_eigdit->cd(2);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(asym_main_det_eigen_dit_reg_diff_mean)/asym_main_det_eigen_dit_reg_diff_mean_error",cut+((TCut)("abs(rcdb_sign*(asym_main_det_eigen_dit_reg_diff_mean)/asym_main_det_eigen_dit_reg_diff_mean_error)>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_main_det_eigen_dit_reg_diff_mean)/asym_main_det_eigen_dit_reg_diff_mean_error",cut,"");
  TH1F* tg_regreg_eigdit_pull_self = (TH1F*)gPad->FindObject("htemp");
  if (tg_regreg_eigdit_pull_self) {
    tg_regreg_eigdit_pull_self->SetNameTitle("Diff Multipletwise Regular reg vs. Eigen dit self Pull","Difference multipletwise pull of regular reg vs. eigen dit, self weighted;multipletwise difference pull");
    Printf("tg_regreg_eigdit_pull_self = %e +- %e",tg_regreg_eigdit_pull_self->GetMean(),tg_regreg_eigdit_pull_self->GetRMS());
    outfile0 << "regreg_eigdit_pull_self = " << tg_regreg_eigdit_pull_self->GetMean() << " +- " << tg_regreg_eigdit_pull_self->GetRMS() << std::endl;
  }


  c_regreg_eigdit->cd(3);
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(asym_main_det_eigen_dit_reg_diff_mean_error/reg_asym_main_det_mean_error,2):reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_regreg_eigdit_multipletwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regreg_eigdit_multipletwise_scalefactor->Fit("pol0");
  TF1* tg_regreg_eigdit_multipletwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t regreg_eigdit_multipletwise_scalefactor = sqrt(tg_regreg_eigdit_multipletwise_scalefactor_fit->GetParameter(0));

  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_main_det_eigen_dit_reg_diff_mean)/1e-9:%e*reg_asym_main_det_mean_error/1e-9:run_number+0.1*minirun_n",regreg_eigdit_multipletwise_scalefactor),cut,"goff");
  TGraphErrors* tg_regreg_eigdit_multipletwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regreg_eigdit_multipletwise_maindet->SetNameTitle("Diff Multipletwise Regular reg vs. Eigen dit maindet","Difference multipletwise of regular reg vs. eigen dit, maindet weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
  tg_regreg_eigdit_multipletwise_maindet->Fit("pol0");
  tg_regreg_eigdit_multipletwise_maindet->Draw("ap");
  TF1* tg_regreg_eigdit_multipletwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_regreg_eigdit_multipletwise_maindet = %e +- %e",tg_regreg_eigdit_multipletwise_maindet_fit->GetParameter(0),tg_regreg_eigdit_multipletwise_maindet_fit->GetParError(0));
  outfile0 << "regreg_eigdit_multipletwise_maindet = " << tg_regreg_eigdit_multipletwise_maindet_fit->GetParameter(0) << " +- " << tg_regreg_eigdit_multipletwise_maindet_fit->GetParError(0) << std::endl;

  c_regreg_eigdit->cd(4);
  gStyle->SetOptStat(122221);
  gPad->SetLogy(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_main_det_eigen_dit_reg_diff_mean)/(%e*reg_asym_main_det_mean_error)",regreg_eigdit_multipletwise_scalefactor),cut,"");
  TH1F* tg_regreg_eigdit_pull_maindet = (TH1F*) gPad->FindObject("htemp");
  if (tg_regreg_eigdit_pull_maindet) {
    tg_regreg_eigdit_pull_maindet->SetNameTitle("Diff Multipletwise Regular reg vs. Eigen dit maindet Pull","Difference multipletwise pull of regular reg vs. eigen dit, maindet weighted;multipletwise difference pull");
    Printf("tg_regreg_eigdit_pull_maindet = %e +- %e",tg_regreg_eigdit_pull_maindet->GetMean(),tg_regreg_eigdit_pull_maindet->GetRMS());
    outfile0 << "regreg_eigdit_pull_maindet = " << tg_regreg_eigdit_pull_maindet->GetMean() << " +- " << tg_regreg_eigdit_pull_maindet->GetRMS() << std::endl;
  }
  //c_regreg_eigdit->SaveAs(Form("plots/drl_diff_plots/regular_reg_eigen_dit_diffs_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));
  c_regreg_eigdit->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));


  // Now do dit vs. eigen dit comparison, maindet weighted and self weighted, both using quadrature error bar differences only
  agg_part_avgd_friendable->AddFriend(agg_plain_friendable);

  c_regreg_eigreg->cd(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean)/1e-9:sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean_error,2)))/1e-9:run_number+0.1*minirun_n",cut+"rcdb_arm_flag==0","goff");
  TGraphErrors* tg_regdit_eigdit_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regdit_eigdit_minirunwise_self->SetNameTitle("Minirunwise diff Regular dit vs. Eigen dit self","Minirunwise difference of regular dit vs. eigen dit, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_regdit_eigdit_minirunwise_self->Fit("pol0");
  tg_regdit_eigdit_minirunwise_self->Draw("ap");
  TF1* tg_regdit_eigdit_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_regdit_eigdit_minirunwise_self = %e +- %e",tg_regdit_eigdit_minirunwise_self_fit->GetParameter(0),tg_regdit_eigdit_minirunwise_self_fit->GetParError(0));
  outfile0 << "regdit_eigdit_minirunwise_self = " << tg_regdit_eigdit_minirunwise_self_fit->GetParameter(0) << " +- " << tg_regdit_eigdit_minirunwise_self_fit->GetParError(0) << std::endl;

  c_regreg_eigreg->cd(2);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean)/sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean_error,2)))",cut+((TCut)("abs(rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean)/sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean_error,2))))>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean)/sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean_error,2)))",cut,"");
  TH1F* tg_regdit_eigdit_pull_self = (TH1F*)gPad->FindObject("htemp");
  if (tg_regdit_eigdit_pull_self) {
    tg_regdit_eigdit_pull_self->SetNameTitle("Diff Multipletwise Regular dit vs. Eigen dit self Pull","Difference multipletwise pull of regular dit vs. eigen dit, self weighted;multipletwise difference pull");
    Printf("tg_regdit_eigdit_pull_self = %e +- %e",tg_regdit_eigdit_pull_self->GetMean(),tg_regdit_eigdit_pull_self->GetRMS());
    outfile0 << "regdit_eigdit_pull_self = " << tg_regdit_eigdit_pull_self->GetMean() << " +- " << tg_regdit_eigdit_pull_self->GetRMS() << std::endl;
  }

  c_regreg_eigreg->cd(3);
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean)/1e-9:sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean_error,2)))/1e-9:run_number+0.1*minirun_n",cut+((TCut)("rcdb_arm_flag==0 && abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean) < 1e-9 && abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean) > 1e-26")),"goff");
  TGraphErrors* tg_small_regdit_eigdit_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_small_regdit_eigdit_minirunwise_self->SetNameTitle("Small Minirunwise diff Regular dit vs. Eigen dit self","Tight cut - Minirunwise difference of regular dit vs. eigen dit, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_small_regdit_eigdit_minirunwise_self->Fit("pol0");
  tg_small_regdit_eigdit_minirunwise_self->Draw("ap");
  TF1* tg_small_regdit_eigdit_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_small_regdit_eigdit_minirunwise_self = %e +- %e",tg_small_regdit_eigdit_minirunwise_self_fit->GetParameter(0),tg_small_regdit_eigdit_minirunwise_self_fit->GetParError(0));
  outfile0 << "small_regdit_eigdit_minirunwise_self = " << tg_small_regdit_eigdit_minirunwise_self_fit->GetParameter(0) << " +- " << tg_small_regdit_eigdit_minirunwise_self_fit->GetParError(0) << std::endl;

  c_regreg_eigreg->cd(4);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean)/sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean_error,2)))",cut+((TCut)("rcdb_arm_flag==0 && abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean) < 1e-9 && abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean) > 1e-26 && abs(rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean)/sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean_error,2))))>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean)/sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean_error,2)))",cut+((TCut)("rcdb_arm_flag==0 && abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean) < 1e-9 && abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_dit_asym_us_avg_mean) > 1e-26")),"goff");
  if (nen != 0) {
    TH1F* tg_small_regdit_eigdit_pull_self = (TH1F*) gPad->FindObject("htemp");
    if (tg_small_regdit_eigdit_pull_self) {
      tg_small_regdit_eigdit_pull_self->SetNameTitle("Diff Multipletwise Regular dit vs. Eigen dit self Pull","Difference multipletwise pull of regular dit vs. eigen dit, self weighted;multipletwise difference pull");
      Printf("tg_small_regdit_eigdit_pull_self = %e +- %e",tg_small_regdit_eigdit_pull_self->GetMean(),tg_small_regdit_eigdit_pull_self->GetRMS());
      outfile0 << "small_regdit_eigdit_pull_self = " << tg_small_regdit_eigdit_pull_self->GetMean() << " +- " << tg_small_regdit_eigdit_pull_self->GetRMS() << std::endl;
    }
  }

  c_regreg_eigreg->cd(5);
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/1e-9:sqrt(abs(pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)))/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_small_regreg_eigreg_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_small_regreg_eigreg_minirunwise_self->SetNameTitle("Minirunwise diff Regular reg vs. Eigen reg self","Minirunwise difference of regular reg vs. eigen reg, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_small_regreg_eigreg_minirunwise_self->Fit("pol0");
  tg_small_regreg_eigreg_minirunwise_self->Draw("ap");
  TF1* tg_small_regreg_eigreg_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_small_regreg_eigreg_minirunwise_self = %e +- %e",tg_small_regreg_eigreg_minirunwise_self_fit->GetParameter(0),tg_small_regreg_eigreg_minirunwise_self_fit->GetParError(0));
  outfile0 << "small_regreg_eigreg_minirunwise_self = " << tg_small_regreg_eigreg_minirunwise_self_fit->GetParameter(0) << " +- " << tg_small_regreg_eigreg_minirunwise_self_fit->GetParError(0) << std::endl;

  c_regreg_eigreg->cd(6);
  gStyle->SetOptStat(122221);
  gPad->SetLeftMargin(0.125);
  agg_part_avgd_friendable->Scan("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:rcdb_sign*(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/sqrt(abs(pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)))",cut+((TCut)("abs(rcdb_sign*(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/sqrt(abs(pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2))))>4")));
  gPad->SetLogy(1);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.reg_asym_main_det_mean-agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean)/sqrt(abs(pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.reg_asym_main_det_mean_error,2)-pow(agg_ErrorFlag_"+type+"_avgd"+bpms+do_err+"_friendable.eigen_reg_asym_main_det_mean_error,2)))",cut,"goff");
  TH1F* tg_small_regreg_eigreg_pull_self = (TH1F*) gPad->FindObject("htemp");
  if (tg_small_regreg_eigreg_pull_self) {
    tg_small_regreg_eigreg_pull_self->SetNameTitle("Diff Multipletwise Regular reg vs. Eigen reg self Pull","Difference multipletwise pull of regular reg vs. eigen reg, self weighted;multipletwise difference pull");
    Printf("tg_small_regreg_eigreg_pull_self = %e +- %e",tg_small_regreg_eigreg_pull_self->GetMean(),tg_small_regreg_eigreg_pull_self->GetRMS());
    outfile0 << "small_regreg_eigreg_pull_self = " << tg_small_regreg_eigreg_pull_self->GetMean() << " +- " << tg_small_regreg_eigreg_pull_self->GetRMS() << std::endl;
  }

  //c_small_regreg_eigreg->SaveAs(Form("plots/drl_diff_plots/regular_vs_eigen_same_diffs_%s_avgd%s%s.pdf",type.Data(),bpms.Data(),suffix.Data()));
  c_regreg_eigreg->SaveAs(Form("plots/drl_diff_plots/dit_reg_lagr_compare_%s_avgd%s%s.pdf)",type.Data(),bpms.Data(),suffix.Data()));
  gPad->SetLogy(0);
  outfile0.close();
}*/

/*c_eigdit_RMS_1->cd(1);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_main_det_eigen_dit_eigen_reg_diff_rms:run_number+0.1*minirun_n",cut,"");
  c_eigdit_RMS_1->cd(2);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_main_det_eigen_dit_reg_diff_rms:run_number+0.1*minirun_n",cut,"");
  c_eigdit_RMS_1->cd(3);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_main_det_eigen_dit_eigen_reg_diff_rms:yield_bpm12X_mean",cut,"");
  c_eigdit_RMS_1->cd(4);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_main_det_eigen_dit_reg_diff_rms:yield_bpm12X_mean",cut,"");
  c_eigdit_RMS_1->SaveAs(Form("plots/drl_diff_plots/eigen_dit_reg_diff_RMSs_%s_avgd%s%s.pdf(",type.Data(),bpms.Data(),suffix.Data()));

  c_eigdit_RMS_2->cd(1);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_main_det_eigen_dit_eigen_reg_diff_rms:diff_bpm12X_rms",cut,"");
  c_eigdit_RMS_2->cd(2);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_main_det_eigen_dit_reg_diff_rms:diff_bpm12X_rms",cut,"");
  c_eigdit_RMS_2->cd(3);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_main_det_eigen_dit_eigen_reg_diff_rms:yield_bpm12X_rms",cut,"");
  c_eigdit_RMS_2->cd(4);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_main_det_eigen_dit_reg_diff_rms:yield_bpm12X_rms",cut,"");
  c_eigdit_RMS_2->SaveAs(Form("plots/drl_diff_plots/dit_reg_diff_RMSs_%s_avgd%s%s.pdf)",type.Data(),bpms.Data(),suffix.Data()));
  */

}

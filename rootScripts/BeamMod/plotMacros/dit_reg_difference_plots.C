#include <iostream>
#include <fstream>
void dit_reg_difference_plots() {
  TChain * agg_part_avgd_friendable = new TChain("agg_part_avgd_friendable");
  TChain * agg_plain_friendable = new TChain("agg_plain_friendable");
  agg_part_avgd_friendable->Add("respin1_data/aggregator_outputs_prod_only_mini_friendable.root");
  agg_plain_friendable->Add("respin1_data/aggregator_outputs_prod_only_mini_friendable.root");
  TCut cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0 && ((rcdb_slug != 142 && rcdb_slug != 188 && rcdb_slug != 189 && rcdb_slug != 190 && rcdb_slug != 192))";
  Int_t nen = 0;
  TCanvas* c1 = new TCanvas();
  TCanvas* c2 = new TCanvas();
  TCanvas* c3 = new TCanvas();
  TCanvas* c4 = new TCanvas();
  TCanvas* c5 = new TCanvas();
  c1->Divide(2,2);
  c2->Divide(2,2);
  c3->Divide(2,3);
  c4->Divide(2,2);
  c5->Divide(2,2);
  gStyle->SetOptFit(1);
  std::ofstream outfile0;
  outfile0.open("dit_reg_diffs_outputs.txt",std::ofstream::app);

  c1->cd(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_us_avg_dit_eigen_reg_diff_mean)/1e-9:asym_us_avg_dit_eigen_reg_diff_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_eigreg_eigdit_multipletwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigreg_eigdit_multipletwise_self->SetNameTitle("Diff Multipletwise Eigen reg vs. Eigen dit self","Difference multipletwise of eigen reg vs. eigen dit, self weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
  tg_eigreg_eigdit_multipletwise_self->Fit("pol0");
  tg_eigreg_eigdit_multipletwise_self->Draw("ap");
  TF1* tg_eigreg_eigdit_multipletwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eigreg_eigdit_multipletwise_self = %e +- %e",tg_eigreg_eigdit_multipletwise_self_fit->GetParameter(0),tg_eigreg_eigdit_multipletwise_self_fit->GetParError(0));
  outfile0 << "eigreg_eigdit_multipletwise_self = " << tg_eigreg_eigdit_multipletwise_self_fit->GetParameter(0) << " +- " << tg_eigreg_eigdit_multipletwise_self_fit->GetParError(0) << std::endl;

  c1->cd(2);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(dit_asym_us_avg_mean-eigen_reg_asym_us_avg_mean)/1e-9:sqrt(abs(pow(eigen_reg_asym_us_avg_mean_error,2)-pow(dit_asym_us_avg_mean_error,2)))/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_eigreg_eigdit_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigreg_eigdit_minirunwise_self->SetNameTitle("Minirunwise diff Eigen reg vs. Eigen dit self","Minirunwise difference of eigen reg vs. eigen dit, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_eigreg_eigdit_minirunwise_self->Fit("pol0");
  tg_eigreg_eigdit_minirunwise_self->Draw("ap");
  TF1* tg_eigreg_eigdit_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eigreg_eigdit_minirunwise_self = %e +- %e",tg_eigreg_eigdit_minirunwise_self_fit->GetParameter(0),tg_eigreg_eigdit_minirunwise_self_fit->GetParError(0));
  outfile0 << "eigreg_eigdit_minirunwise_self = " << tg_eigreg_eigdit_minirunwise_self_fit->GetParameter(0) << " +- " << tg_eigreg_eigdit_minirunwise_self_fit->GetParError(0) << std::endl;

  c1->cd(3);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(asym_us_avg_dit_eigen_reg_diff_mean_error/reg_asym_us_avg_mean_error,2):reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_eigreg_eigdit_multipletwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigreg_eigdit_multipletwise_scalefactor->Fit("pol0");
  TF1* tg_eigreg_eigdit_multipletwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t eigreg_eigdit_multipletwise_scalefactor = sqrt(tg_eigreg_eigdit_multipletwise_scalefactor_fit->GetParameter(0));

  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_us_avg_dit_eigen_reg_diff_mean)/1e-9:%f*reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",eigreg_eigdit_multipletwise_scalefactor),cut,"goff");
  TGraphErrors* tg_eigreg_eigdit_multipletwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigreg_eigdit_multipletwise_maindet->SetNameTitle("Diff Multipletwise Eigen reg vs. Eigen dit maindet","Difference multipletwise of eigen reg vs. eigen dit, maindet weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
  tg_eigreg_eigdit_multipletwise_maindet->Fit("pol0");
  tg_eigreg_eigdit_multipletwise_maindet->Draw("ap");
  TF1* tg_eigreg_eigdit_multipletwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eigreg_eigdit_multipletwise_maindet = %e +- %e",tg_eigreg_eigdit_multipletwise_maindet_fit->GetParameter(0),tg_eigreg_eigdit_multipletwise_maindet_fit->GetParError(0));
  outfile0 << "eigreg_eigdit_multipletwise_maindet = " << tg_eigreg_eigdit_multipletwise_maindet_fit->GetParameter(0) << " +- " << tg_eigreg_eigdit_multipletwise_maindet_fit->GetParError(0) << std::endl;

  c1->cd(4);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(sqrt(abs(pow(eigen_reg_asym_us_avg_mean_error,2)-pow(dit_asym_us_avg_mean_error,2)))/reg_asym_us_avg_mean_error,2):reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_eigreg_eigdit_minirunwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigreg_eigdit_minirunwise_scalefactor->Fit("pol0");
  TF1* tg_eigreg_eigdit_minirunwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t eigreg_eigdit_minirunwise_scalefactor = sqrt(tg_eigreg_eigdit_minirunwise_scalefactor_fit->GetParameter(0));

  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_us_avg_dit_eigen_reg_diff_mean)/1e-9:%f*reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",eigreg_eigdit_minirunwise_scalefactor),cut,"goff");
  TGraphErrors* tg_eigreg_eigdit_minirunwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_eigreg_eigdit_minirunwise_maindet->SetNameTitle("Minirunwise diff Eigen reg vs. Eigen dit maindet","Minirunwise difference of eigen reg vs. eigen dit, maindet weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_eigreg_eigdit_minirunwise_maindet->Fit("pol0");
  tg_eigreg_eigdit_minirunwise_maindet->Draw("ap");
  TF1* tg_eigreg_eigdit_minirunwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_eigreg_eigdit_minirunwise_maindet = %e +- %e",tg_eigreg_eigdit_minirunwise_maindet_fit->GetParameter(0),tg_eigreg_eigdit_minirunwise_maindet_fit->GetParError(0));
  outfile0 << "eigreg_eigdit_minirunwise_maindet = " << tg_eigreg_eigdit_minirunwise_maindet_fit->GetParameter(0) << " +- " << tg_eigreg_eigdit_minirunwise_maindet_fit->GetParError(0) << std::endl;
  c1->SaveAs("eigen_reg_eigen_dit_diffs.pdf");


  // Now do minirunwise regular regression vs. segment avg eigen dit

  c2->cd(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(asym_us_avg_dit_reg_diff_mean)/1e-9:asym_us_avg_dit_reg_diff_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_regreg_eigdit_multipletwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regreg_eigdit_multipletwise_self->SetNameTitle("Diff Multipletwise Regular reg vs. Eigen dit self","Difference multipletwise of regular reg vs. eigen dit, self weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
  tg_regreg_eigdit_multipletwise_self->Fit("pol0");
  tg_regreg_eigdit_multipletwise_self->Draw("ap");
  TF1* tg_regreg_eigdit_multipletwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_regreg_eigdit_multipletwise_self = %e +- %e",tg_regreg_eigdit_multipletwise_self_fit->GetParameter(0),tg_regreg_eigdit_multipletwise_self_fit->GetParError(0));
  outfile0 << "regreg_eigdit_multipletwise_self = " << tg_regreg_eigdit_multipletwise_self_fit->GetParameter(0) << " +- " << tg_regreg_eigdit_multipletwise_self_fit->GetParError(0) << std::endl;

  c2->cd(2);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(dit_asym_us_avg_mean-reg_asym_us_avg_mean)/1e-9:sqrt(abs(pow(reg_asym_us_avg_mean_error,2)-pow(dit_asym_us_avg_mean_error,2)))/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_regreg_eigdit_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regreg_eigdit_minirunwise_self->SetNameTitle("Minirunwise diff Regular reg vs. Eigen dit self","Minirunwise difference of regular reg vs. eigen dit, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_regreg_eigdit_minirunwise_self->Fit("pol0");
  tg_regreg_eigdit_minirunwise_self->Draw("ap");
  TF1* tg_regreg_eigdit_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_regreg_eigdit_minirunwise_self = %e +- %e",tg_regreg_eigdit_minirunwise_self_fit->GetParameter(0),tg_regreg_eigdit_minirunwise_self_fit->GetParError(0));
  outfile0 << "regreg_eigdit_minirunwise_self = " << tg_regreg_eigdit_minirunwise_self_fit->GetParameter(0) << " +- " << tg_regreg_eigdit_minirunwise_self_fit->GetParError(0) << std::endl;

  c2->cd(3);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(asym_us_avg_dit_reg_diff_mean_error/reg_asym_us_avg_mean_error,2):reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_regreg_eigdit_multipletwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regreg_eigdit_multipletwise_scalefactor->Fit("pol0");
  TF1* tg_regreg_eigdit_multipletwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t regreg_eigdit_multipletwise_scalefactor = sqrt(tg_regreg_eigdit_multipletwise_scalefactor_fit->GetParameter(0));

  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_us_avg_dit_reg_diff_mean)/1e-9:%f*reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",regreg_eigdit_multipletwise_scalefactor),cut,"goff");
  TGraphErrors* tg_regreg_eigdit_multipletwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regreg_eigdit_multipletwise_maindet->SetNameTitle("Diff Multipletwise Regular reg vs. Eigen dit maindet","Difference multipletwise of regular reg vs. eigen dit, maindet weighted;Run+0.1*Minirun;Difference multipletwise (ppb)");
  tg_regreg_eigdit_multipletwise_maindet->Fit("pol0");
  tg_regreg_eigdit_multipletwise_maindet->Draw("ap");
  TF1* tg_regreg_eigdit_multipletwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_regreg_eigdit_multipletwise_maindet = %e +- %e",tg_regreg_eigdit_multipletwise_maindet_fit->GetParameter(0),tg_regreg_eigdit_multipletwise_maindet_fit->GetParError(0));
  outfile0 << "regreg_eigdit_multipletwise_maindet = " << tg_regreg_eigdit_multipletwise_maindet_fit->GetParameter(0) << " +- " << tg_regreg_eigdit_multipletwise_maindet_fit->GetParError(0) << std::endl;

  c2->cd(4);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(sqrt(abs(pow(reg_asym_us_avg_mean_error,2)-pow(dit_asym_us_avg_mean_error,2)))/reg_asym_us_avg_mean_error,2):reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_regreg_eigdit_minirunwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regreg_eigdit_minirunwise_scalefactor->Fit("pol0");
  TF1* tg_regreg_eigdit_minirunwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t regreg_eigdit_minirunwise_scalefactor = sqrt(tg_regreg_eigdit_minirunwise_scalefactor_fit->GetParameter(0));

  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(asym_us_avg_dit_reg_diff_mean)/1e-9:%f*reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",regreg_eigdit_minirunwise_scalefactor),cut,"goff");
  TGraphErrors* tg_regreg_eigdit_minirunwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regreg_eigdit_minirunwise_maindet->SetNameTitle("Minirunwise diff Regular reg vs. Eigen dit maindet","Minirunwise difference of regular reg vs. eigen dit, maindet weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_regreg_eigdit_minirunwise_maindet->Fit("pol0");
  tg_regreg_eigdit_minirunwise_maindet->Draw("ap");
  TF1* tg_regreg_eigdit_minirunwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_regreg_eigdit_minirunwise_maindet = %e +- %e",tg_regreg_eigdit_minirunwise_maindet_fit->GetParameter(0),tg_regreg_eigdit_minirunwise_maindet_fit->GetParError(0));
  outfile0 << "regreg_eigdit_minirunwise_maindet = " << tg_regreg_eigdit_minirunwise_maindet_fit->GetParameter(0) << " +- " << tg_regreg_eigdit_minirunwise_maindet_fit->GetParError(0) << std::endl;
  c2->SaveAs("regular_reg_eigen_dit_diffs.pdf");


  // Now do dit vs. eigen dit comparison, maindet weighted and self weighted, both using quadrature error bar differences only
  agg_part_avgd_friendable->AddFriend(agg_plain_friendable);
  
  c3->cd(1);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean)/1e-9:sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_part_avgd_friendable.dit_asym_us_avg_mean_error,2)))/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_regdit_eigdit_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regdit_eigdit_minirunwise_self->SetNameTitle("Minirunwise diff Regular dit vs. Eigen dit self","Minirunwise difference of regular dit vs. eigen dit, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_regdit_eigdit_minirunwise_self->Fit("pol0");
  tg_regdit_eigdit_minirunwise_self->Draw("ap");
  TF1* tg_regdit_eigdit_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_regdit_eigdit_minirunwise_self = %e +- %e",tg_regdit_eigdit_minirunwise_self_fit->GetParameter(0),tg_regdit_eigdit_minirunwise_self_fit->GetParError(0));
  outfile0 << "regdit_eigdit_minirunwise_self = " << tg_regdit_eigdit_minirunwise_self_fit->GetParameter(0) << " +- " << tg_regdit_eigdit_minirunwise_self_fit->GetParError(0) << std::endl;

  c3->cd(2);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_part_avgd_friendable.dit_asym_us_avg_mean_error,2)))/reg_asym_us_avg_mean_error,2):reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_regdit_eigdit_minirunwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regdit_eigdit_minirunwise_scalefactor->Fit("pol0");
  TF1* tg_regdit_eigdit_minirunwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t regdit_eigdit_minirunwise_scalefactor = sqrt(tg_regdit_eigdit_minirunwise_scalefactor_fit->GetParameter(0));

  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean)/1e-9:%f*reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",regdit_eigdit_minirunwise_scalefactor),cut,"goff");
  TGraphErrors* tg_regdit_eigdit_minirunwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_regdit_eigdit_minirunwise_maindet->SetNameTitle("Minirunwise diff Regular dit vs. Eigen dit maindet","Minirunwise difference of regular dit vs. eigen dit, maindet weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_regdit_eigdit_minirunwise_maindet->Fit("pol0");
  tg_regdit_eigdit_minirunwise_maindet->Draw("ap");
  TF1* tg_regdit_eigdit_minirunwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_regdit_eigdit_minirunwise_maindet = %e +- %e",tg_regdit_eigdit_minirunwise_maindet_fit->GetParameter(0),tg_regdit_eigdit_minirunwise_maindet_fit->GetParError(0));
  outfile0 << "regdit_eigdit_minirunwise_maindet = " << tg_regdit_eigdit_minirunwise_maindet_fit->GetParameter(0) << " +- " << tg_regdit_eigdit_minirunwise_maindet_fit->GetParError(0) << std::endl;

  c3->cd(3);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean)/1e-9:sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_part_avgd_friendable.dit_asym_us_avg_mean_error,2)))/1e-9:run_number+0.1*minirun_n",cut+"abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean) < 1e-9 && abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean) > 1e-26","goff");
  TGraphErrors* tg_small_regdit_eigdit_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_small_regdit_eigdit_minirunwise_self->SetNameTitle("Small Minirunwise diff Regular dit vs. Eigen dit self","Tight cut - Minirunwise difference of regular dit vs. eigen dit, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_small_regdit_eigdit_minirunwise_self->Fit("pol0");
  tg_small_regdit_eigdit_minirunwise_self->Draw("ap");
  TF1* tg_small_regdit_eigdit_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_small_regdit_eigdit_minirunwise_self = %e +- %e",tg_small_regdit_eigdit_minirunwise_self_fit->GetParameter(0),tg_small_regdit_eigdit_minirunwise_self_fit->GetParError(0));
  outfile0 << "small_regdit_eigdit_minirunwise_self = " << tg_small_regdit_eigdit_minirunwise_self_fit->GetParameter(0) << " +- " << tg_small_regdit_eigdit_minirunwise_self_fit->GetParError(0) << std::endl;

  c3->cd(4);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(sqrt(abs(pow(agg_plain_friendable.dit_asym_us_avg_mean_error,2)-pow(agg_part_avgd_friendable.dit_asym_us_avg_mean_error,2)))/reg_asym_us_avg_mean_error,2):reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",cut+"abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean) < 1e-9 && abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean) > 1e-26","goff");
  TGraphErrors* tg_small_regdit_eigdit_minirunwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_small_regdit_eigdit_minirunwise_scalefactor->Fit("pol0");
  TF1* tg_small_regdit_eigdit_minirunwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t small_regdit_eigdit_minirunwise_scalefactor = sqrt(tg_small_regdit_eigdit_minirunwise_scalefactor_fit->GetParameter(0));

  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean)/1e-9:%f*reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",small_regdit_eigdit_minirunwise_scalefactor),cut+"abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean) < 1e-9 && abs(agg_plain_friendable.dit_asym_us_avg_mean-agg_part_avgd_friendable.dit_asym_us_avg_mean) > 1e-26","goff");
  TGraphErrors* tg_small_regdit_eigdit_minirunwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_small_regdit_eigdit_minirunwise_maindet->SetNameTitle("Small Minirunwise diff Regular dit vs. Eigen dit maindet","Tight cut - Minirunwise difference of regular dit vs. eigen dit, maindet weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_small_regdit_eigdit_minirunwise_maindet->Fit("pol0");
  tg_small_regdit_eigdit_minirunwise_maindet->Draw("ap");
  TF1* tg_small_regdit_eigdit_minirunwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_small_regdit_eigdit_minirunwise_maindet = %e +- %e",tg_small_regdit_eigdit_minirunwise_maindet_fit->GetParameter(0),tg_small_regdit_eigdit_minirunwise_maindet_fit->GetParError(0));
  outfile0 << "small_regdit_eigdit_minirunwise_maindet = " << tg_small_regdit_eigdit_minirunwise_maindet_fit->GetParameter(0) << " +- " << tg_small_regdit_eigdit_minirunwise_maindet_fit->GetParError(0) << std::endl;

  c3->cd(5);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("rcdb_sign*(agg_part_avgd_friendable.reg_asym_us_avg_mean-agg_part_avgd_friendable.eigen_reg_asym_us_avg_mean)/1e-9:sqrt(abs(pow(agg_part_avgd_friendable.reg_asym_us_avg_mean_error,2)-pow(agg_part_avgd_friendable.eigen_reg_asym_us_avg_mean_error,2)))/1e-9:run_number+0.1*minirun_n",cut,"goff");
  TGraphErrors* tg_small_regreg_eigreg_minirunwise_self = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_small_regreg_eigreg_minirunwise_self->SetNameTitle("Minirunwise diff Regular reg vs. Eigen reg self","Minirunwise difference of regular reg vs. eigen reg, self weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_small_regreg_eigreg_minirunwise_self->Fit("pol0");
  tg_small_regreg_eigreg_minirunwise_self->Draw("ap");
  TF1* tg_small_regreg_eigreg_minirunwise_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_small_regreg_eigreg_minirunwise_self = %e +- %e",tg_small_regreg_eigreg_minirunwise_self_fit->GetParameter(0),tg_small_regreg_eigreg_minirunwise_self_fit->GetParError(0));
  outfile0 << "small_regreg_eigreg_minirunwise_self = " << tg_small_regreg_eigreg_minirunwise_self_fit->GetParameter(0) << " +- " << tg_small_regreg_eigreg_minirunwise_self_fit->GetParError(0) << std::endl;

  c3->cd(6);
  gPad->SetLeftMargin(0.125);
  nen = agg_part_avgd_friendable->Draw("pow(sqrt(abs(pow(agg_part_avgd_friendable.eigen_reg_asym_us_avg_mean_error,2)-pow(agg_part_avgd_friendable.reg_asym_us_avg_mean_error,2)))/reg_asym_us_avg_mean_error,2):reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",cut,"goff"); // +"abs(agg_part_avgd_friendable.eigen_reg_asym_us_avg_mean-agg_part_avgd_friendable.reg_asym_us_avg_mean) < 1e-29 && abs(agg_part_avgd_friendable.eigen_reg_asym_us_avg_mean-agg_part_avgd_friendable.reg_asym_us_avg_mean) > -1e-26"
  TGraphErrors* tg_small_regreg_eigreg_minirunwise_scalefactor = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_small_regreg_eigreg_minirunwise_scalefactor->Fit("pol0");
  TF1* tg_small_regreg_eigreg_minirunwise_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t small_regreg_eigreg_minirunwise_scalefactor = sqrt(tg_small_regreg_eigreg_minirunwise_scalefactor_fit->GetParameter(0));

  nen = agg_part_avgd_friendable->Draw(Form("rcdb_sign*(agg_part_avgd_friendable.eigen_reg_asym_us_avg_mean-agg_part_avgd_friendable.reg_asym_us_avg_mean)/1e-9:%f*reg_asym_us_avg_mean_error/1e-9:run_number+0.1*minirun_n",small_regreg_eigreg_minirunwise_scalefactor),cut,"goff"); // +"abs(agg_part_avgd_friendable.eigen_reg_asym_us_avg_mean-agg_part_avgd_friendable.reg_asym_us_avg_mean) < 1e-29 && abs(agg_part_avgd_friendable.eigen_reg_asym_us_avg_mean-agg_part_avgd_friendable.reg_asym_us_avg_mean) > -1e-26"
  TGraphErrors* tg_small_regreg_eigreg_minirunwise_maindet = new TGraphErrors(nen,agg_part_avgd_friendable->GetV3(),agg_part_avgd_friendable->GetV1(),0,agg_part_avgd_friendable->GetV2());
  tg_small_regreg_eigreg_minirunwise_maindet->SetNameTitle("Minirunwise diff Regular reg vs. Eigen reg maindet","Minirunwise difference of regular reg vs. eigen reg, maindet weighted;Run+0.1*Minirun;Minirunwise difference (ppb)");
  tg_small_regreg_eigreg_minirunwise_maindet->Fit("pol0");
  tg_small_regreg_eigreg_minirunwise_maindet->Draw("ap");
  TF1* tg_small_regreg_eigreg_minirunwise_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Printf("tg_small_regreg_eigreg_minirunwise_maindet = %e +- %e",tg_small_regreg_eigreg_minirunwise_maindet_fit->GetParameter(0),tg_small_regreg_eigreg_minirunwise_maindet_fit->GetParError(0));
  outfile0 << "small_regreg_eigreg_minirunwise_maindet = " << tg_small_regreg_eigreg_minirunwise_maindet_fit->GetParameter(0) << " +- " << tg_small_regreg_eigreg_minirunwise_maindet_fit->GetParError(0) << std::endl;

  c3->SaveAs("regular_dit_eigen_dit_diffs.pdf");
  outfile0.close();

  c4->cd(1);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_us_avg_dit_eigen_reg_diff_rms:run_number+0.1*minirun_n",cut,"");
  c4->cd(2);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_us_avg_dit_reg_diff_rms:run_number+0.1*minirun_n",cut,"");
  c4->cd(3);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_us_avg_dit_eigen_reg_diff_rms:yield_bpm12X_mean",cut,"");
  c4->cd(4);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_us_avg_dit_reg_diff_rms:yield_bpm12X_mean",cut,"");
  c4->SaveAs("dit_reg_diff_RMSs.pdf(");

  c5->cd(1);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_us_avg_dit_eigen_reg_diff_rms:diff_bpm12X_rms",cut,"");
  c5->cd(2);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_us_avg_dit_reg_diff_rms:diff_bpm12X_rms",cut,"");
  c5->cd(3);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_us_avg_dit_eigen_reg_diff_rms:yield_bpm12X_rms",cut,"");
  c5->cd(4);
  gPad->SetLogy();
  agg_part_avgd_friendable->Draw("asym_us_avg_dit_reg_diff_rms:yield_bpm12X_rms",cut,"");
  c5->SaveAs("dit_reg_diff_RMSs.pdf)");

}

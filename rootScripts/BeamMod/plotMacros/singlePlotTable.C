#include <iostream>
#include <fstream>
void singlePlotTable(TString otherTree = "mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted", TString mod_draw = "", TString otherDraw = "lagr_asym_manual_main_det", TString units = "ppb", TString mod_cut = "", TString averaging_timescale = "crex_part", TString DataSetCut = "ErrorFlag", TString type = "part", TString suffix = "_main_det_corrections", Int_t pullZero = 1, TString title = "") { 
  // pullZero == 0 means the pull is vs. average, == 1 (true) is vs. 0
  // title = "12BPM Eigenvector Lagrange Maindet Apv") // for example
  Int_t diagnostic = 1; // Turn this on to show the print at bottom of each pdf page
  TCut cut = (TCut)mod_cut;
  // mod_draw should be "rcdb_sign*" for slug plots (only)
  // Suffix here also could be _NULL.... or _main_det_slopes_residuals
  //TFile data_file(Form("processed_respin2_data/results/CREX_All_%s_%s_Avg_Outputs%s.root",DataSetCut.Data(),averaging_timescale.Data(),suffix.Data()),"read");

  if (title == "") {
    title = otherTree + " " + otherDraw;
  }

  // Get trees
  //TTree * mini_lagr = new TTree("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted");
  //mini_lagr = (TTree*)data_file.Get("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted");
  TChain * mini_lagr = new TChain("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted");// = new TTree("mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted","mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted");
  mini_lagr->Add(Form("processed_respin2_data/results/CREX_All_%s_%s_Avg_Outputs%s.root",DataSetCut.Data(),averaging_timescale.Data(),suffix.Data()));
  mini_lagr->SetScanField(0);
  //TTree * mini_other = new TTree(otherTree,otherTree);
  //mini_other = (TTree*)data_file.Get(otherTree);
  TChain * mini_other = new TChain(otherTree);;
  mini_other->Add(Form("processed_respin2_data/results/CREX_All_%s_%s_Avg_Outputs%s.root",DataSetCut.Data(),averaging_timescale.Data(),suffix.Data()));
  mini_other->SetScanField(0);


  // Units
  mini_lagr->SetAlias("ppm","1e-6");
  mini_lagr->SetAlias("ppb","1e-9");
  mini_lagr->SetAlias("mm","1");
  mini_lagr->SetAlias("um","1e-3");
  mini_lagr->SetAlias("nm","1e-6");

  // Add friend
  mini_lagr->AddFriend(mini_other);//,otherTree);

  // Open output file

  TString corrections_pdfname_mod_cut_suffix = "_"+mod_cut;
  if (mod_cut == ""){
    corrections_pdfname_mod_cut_suffix = "";
  }
  corrections_pdfname_mod_cut_suffix.ReplaceAll("*","").ReplaceAll("&","").ReplaceAll("=","").ReplaceAll("|","").ReplaceAll(">","").ReplaceAll("<","").ReplaceAll(".","").ReplaceAll("!","").ReplaceAll(" ","");

  TString corrections_pdfname_mod_draw_suffix = "_"+mod_draw;
  if (mod_draw == ""){
    corrections_pdfname_mod_draw_suffix = "";
  }
  corrections_pdfname_mod_draw_suffix.ReplaceAll("*","").ReplaceAll("&","").ReplaceAll("=","").ReplaceAll("|","").ReplaceAll(">","").ReplaceAll("<","").ReplaceAll(".","").ReplaceAll("!","").ReplaceAll(" ","");

  TString corrections_texname = type+"AvgAllEvMonLagr_"+otherTree+"_"+otherDraw+"_"+ averaging_timescale +"_wise_" +DataSetCut+corrections_pdfname_mod_draw_suffix+corrections_pdfname_mod_cut_suffix+".tex";
  TString corrections_pdfname = type+"AvgAllEvMonLagr_"+otherTree+"_"+otherDraw+"_"+ averaging_timescale +"_wise_" +DataSetCut+corrections_pdfname_mod_draw_suffix+corrections_pdfname_mod_cut_suffix;

  std::ofstream outfile0;
  std::ofstream outfilecsv;
  outfile0.open("singles_plots_tables/"+corrections_texname,std::ofstream::out);
  outfilecsv.open("singles_plots_tables/csv_"+corrections_texname,std::ofstream::out);


  TVirtualPad * p1;
  Int_t nen = 0;
  TCanvas* c1 = new TCanvas(otherDraw,otherDraw,800,600); // Make 800,600 variable?


  if (diagnostic == 1) {
    TString ana = otherTree+" "+otherDraw+"Main det weighted";
    TText *label = new TText(0.0,0.005,ana.Data());
    label->SetTextFont(23);
    label->SetTextSize(12);
    label->SetNDC();
  }


  gStyle->SetOptFit(1);



  // Maindet weighted mean calculation and fit bar, self error bar plot
  p1 = c1->cd();
  p1->SetLogy(0);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("pow(%s.%s_mean_err/lagr_asym_manual_main_det_mean_err,2):lagr_asym_manual_main_det_mean_err/%s:%s",otherTree.Data(),otherDraw.Data(),units.Data(),averaging_timescale.Data()),cut,"goff");
  TGraphErrors* tg_maindet_scalefactor = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_maindet_scalefactor->Fit("pol0");
  TF1* tg_maindet_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t maindet_scalefactor = sqrt(tg_maindet_scalefactor_fit->GetParameter(0));

  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%e*lagr_asym_manual_main_det_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),maindet_scalefactor,units.Data(),averaging_timescale.Data()),cut,"goff");
  TGraphErrors* tg_self_maindet = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_self_maindet->SetNameTitle(Form("Maindet weighted %s",title.Data()),Form("Maindet weighted %s;%s;%s (%s)",title.Data(),averaging_timescale.Data(),title.Data(),units.Data()));
  tg_self_maindet->Fit("pol0");
  tg_self_maindet->Draw("ap");
  TF1* tg_self_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t self_maindet_mean = tg_self_maindet_fit->GetParameter(0);
  if (pullZero == 1) self_maindet_mean = 0;
  Printf("tg_self_maindet = %e +- %e",tg_self_maindet_fit->GetParameter(0),tg_self_maindet_fit->GetParError(0));


  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%s.%s_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),averaging_timescale.Data()),cut,"goff");
  TGraphErrors* tg_self_only_maindet = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_self_only_maindet->SetNameTitle(Form("Maindet weighted avg, local errors, %s",title.Data()),Form("Maindet weighted  avg, local errors%s;%s;%s (%s)",title.Data(),averaging_timescale.Data(),title.Data(),units.Data()));

  outfile0 << title << " " << averaging_timescale << ", time, Value, local error bar" << std::endl;
  outfilecsv << title << " " << averaging_timescale << ", time, Value, local error bar" << std::endl;
  for (Int_t i = 0 ; i < nen ; i++) {
    outfile0 << title << " " << averaging_timescale << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << mini_lagr->GetV2()[i] << std::endl;
    outfilecsv << title << " " << averaging_timescale << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << mini_lagr->GetV2()[i] << std::endl;
  }
  outfile0 << title << " maindet weighted average, " << tg_self_maindet_fit->GetParameter(0) << " +- " << tg_self_maindet_fit->GetParError(0) << std::endl;
  outfilecsv << title << " maindet weighted average, " << tg_self_maindet_fit->GetParameter(0) << " +- " << tg_self_maindet_fit->GetParError(0) << std::endl;
  c1->SaveAs(Form("singles_plots_tables/%s.pdf(",corrections_pdfname.Data()));
  c1->SaveAs(Form("singles_plots_tables/p0_%s.pdf",corrections_pdfname.Data()));


  p1 = c1->cd();
  gStyle->SetOptStat(122221);
  p1->SetLogy(1);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("%s(%s.%s_mean-%e*%s)/(%s.%s_mean_err)",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),self_maindet_mean,units.Data(),otherTree.Data(),otherDraw.Data()),cut,"");
  TH1F* th_pull_self_maindet = (TH1F*) p1->FindObject("htemp");
  if (th_pull_self_maindet) {
    th_pull_self_maindet->SetNameTitle(Form("Maindet weighted mean, self pull - %s",title.Data()),Form("Maindet weighted mean, self pull;Pull"));
    Printf("th_pull_self_maindet = %e +- %e",th_pull_self_maindet->GetMean(),th_pull_self_maindet->GetRMS());
    outfile0 << title << " maindet weighted mean, self pull = " << th_pull_self_maindet->GetMean() << " +- " << th_pull_self_maindet->GetRMS() << std::endl;
    outfilecsv << title << " maindet weighted mean, self pull = " << th_pull_self_maindet->GetMean() << " +- " << th_pull_self_maindet->GetRMS() << std::endl;
  }
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));
  c1->SaveAs(Form("singles_plots_tables/pull_%s.pdf",corrections_pdfname.Data()));





  // Self weighted
  p1 = c1->cd();
  p1->SetLogy(0);
  //p1->SetLeftMargin(0.125);
  //Printf("%s(%s.%s_mean)/%s:%s.%s_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),averaging_timescale.Data());
  //nen = mini_lagr->Scan(Form("%s(%s.%s_mean)/%s:%s.%s_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),averaging_timescale.Data()),"");
  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%s.%s_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),averaging_timescale.Data()),cut,"goff");
  TGraphErrors* tg_self = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_self->SetNameTitle(Form("Self weighted %s",title.Data()),Form("Self weighted %s;%s;%s (%s)",title.Data(),averaging_timescale.Data(),title.Data(),units.Data()));
  tg_self->Fit("pol0");
  tg_self->Draw("ap");
  TF1* tg_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t self_mean = tg_self_fit->GetParameter(0);
  if (pullZero == 1) self_mean = 0;
  Printf("tg_self = %e +- %e",tg_self_fit->GetParameter(0),tg_self_fit->GetParError(0));
  outfile0 << title << " " << averaging_timescale << ", time, Value, Value Error" << std::endl;
  outfilecsv << title << " " << averaging_timescale << ", time, Value, Value Error" << std::endl;
  for (Int_t i = 0 ; i < nen ; i++) {
    outfile0 << title << " " << averaging_timescale << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << mini_lagr->GetV2()[i] << std::endl;
    outfilecsv << title << " " << averaging_timescale << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << mini_lagr->GetV2()[i] << std::endl;
  }
  outfile0 << title << " total average, " << tg_self_fit->GetParameter(0) << " +- " << tg_self_fit->GetParError(0) << std::endl;
  outfilecsv << title << " total average, " << tg_self_fit->GetParameter(0) << " +- " << tg_self_fit->GetParError(0) << std::endl;
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));


  p1 = c1->cd();
  p1->SetLogy(0);
  gStyle->SetOptStat(122221);
  //p1->SetLeftMargin(0.125);
  //mini_lagr->Scan(Form("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:%s(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/asym_main_det_eigen_lagr_eigen_reg_diff_mean_error",mod_draw.Data()),cut+((TCut)((TString)Form("abs(%s(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/asym_main_det_eigen_lagr_eigen_reg_diff_mean_error)>2",mod_draw.Data()))));
  p1->SetLogy(1);
  nen = mini_lagr->Draw(Form("%s(%s.%s_mean-%e*%s)/%s.%s_mean_err",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),self_mean,units.Data(),otherTree.Data(),otherDraw.Data()),cut,"");
  TH1F* th_pull_self = (TH1F*)p1->FindObject("htemp");
  if (th_pull_self) {
    th_pull_self->SetNameTitle(Form("Self weighted pull - %s",title.Data()),Form("Self weighted pull;Pull"));
    Printf("th_pull_self = %e +- %e",th_pull_self->GetMean(),th_pull_self->GetRMS());
    outfile0 << title << " pull plot stats = " << th_pull_self->GetMean() << " +- " << th_pull_self->GetRMS() << std::endl;
    outfilecsv << title << " pull plot stats = " << th_pull_self->GetMean() << " +- " << th_pull_self->GetRMS() << std::endl;
  }
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));




  // Maindet weighted
  p1 = c1->cd();
  p1->SetLogy(0);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%e*lagr_asym_manual_main_det_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),maindet_scalefactor,units.Data(),averaging_timescale.Data()),cut,"goff");
  TGraphErrors* tg_maindet = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_maindet->SetNameTitle(Form("Maindet weighted %s",title.Data()),Form("Maindet weighted %s;%s;%s (%s)",title.Data(),averaging_timescale.Data(),title.Data(),units.Data()));
  tg_maindet->Fit("pol0");
  tg_maindet->Draw("ap");
  TF1* tg_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t maindet_mean = tg_maindet_fit->GetParameter(0);
  if (pullZero == 1) maindet_mean = 0;
  Printf("tg_maindet = %e +- %e",tg_maindet_fit->GetParameter(0),tg_maindet_fit->GetParError(0));
  outfile0 << title << " " << averaging_timescale << ", time, Value, Avg Weight" << std::endl;
  outfilecsv << title << " " << averaging_timescale << ", time, Value, Avg Weight" << std::endl;
  for (Int_t i = 0 ; i < nen ; i++) {
    outfile0 << title << " " << averaging_timescale << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << (1.0/pow(mini_lagr->GetV2()[i],2))/(1.0/pow(tg_maindet_fit->GetParError(0),2)) << std::endl;
    outfilecsv << title << " " << averaging_timescale << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << (1.0/pow(mini_lagr->GetV2()[i],2))/(1.0/pow(tg_maindet_fit->GetParError(0),2)) << std::endl;
  }
  outfile0 << title << " maindet weighted average, " << tg_maindet_fit->GetParameter(0) << " +- " << tg_maindet_fit->GetParError(0) << std::endl;
  outfilecsv << title << " maindet weighted average, " << tg_maindet_fit->GetParameter(0) << " +- " << tg_maindet_fit->GetParError(0) << std::endl;
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));


  p1 = c1->cd();
  gStyle->SetOptStat(122221);
  p1->SetLogy(1);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("%s(%s.%s_mean-%e*%s)/(%e*lagr_asym_manual_main_det_mean_err)",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),maindet_mean,units.Data(),maindet_scalefactor),cut,"");
  TH1F* th_pull_maindet = (TH1F*) p1->FindObject("htemp");
  if (th_pull_maindet) {
    th_pull_maindet->SetNameTitle(Form("Maindet weighted pull - %s",title.Data()),Form("Maindet weighted pull;Pull"));
    Printf("th_pull_maindet = %e +- %e",th_pull_maindet->GetMean(),th_pull_maindet->GetRMS());
    outfile0 << title << " maindet weighted pull (not meaningful) = " << th_pull_maindet->GetMean() << " +- " << th_pull_maindet->GetRMS() << std::endl;
    outfilecsv << title << " maindet weighted pull (not meaningful) = " << th_pull_maindet->GetMean() << " +- " << th_pull_maindet->GetRMS() << std::endl;
  }
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));



  // Unweighted
  p1 = c1->cd();
  p1->SetLogy(0);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("pow((%s.%s_mean_err)/(1.0),2):1.0/%s:%s",otherTree.Data(),otherDraw.Data(),units.Data(),averaging_timescale.Data()),cut,"goff");
  TGraphErrors* tg_unweighted_scalefactor = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_unweighted_scalefactor->Fit("pol0");
  TF1* tg_unweighted_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t unweighted_scalefactor = sqrt(tg_unweighted_scalefactor_fit->GetParameter(0));
  Printf("unweight scalefactor = %e",unweighted_scalefactor);

  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%e*1.0/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),unweighted_scalefactor,units.Data(),averaging_timescale.Data()),cut,"goff");
  TGraphErrors* tg_unweighted = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_unweighted->SetNameTitle(Form("Unweighted %s",title.Data()),Form("Unweighted %s;%s;%s (%s)",title.Data(),averaging_timescale.Data(),title.Data(),units.Data()));
  tg_unweighted->Fit("pol0");
  tg_unweighted->Draw("ap");
  TF1* tg_unweighted_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t unweighted_mean = tg_unweighted_fit->GetParameter(0);
  if (pullZero == 1) unweighted_mean = 0;
  Printf("tg_unweighted = %e +- %e",tg_unweighted_fit->GetParameter(0),tg_unweighted_fit->GetParError(0));
  outfile0 << title << " " << averaging_timescale << ", time, Value, Unweighted Weight" << std::endl;
  outfilecsv << title << " " << averaging_timescale << ", time, Value, Unweighted Weight" << std::endl;
  for (Int_t i = 0 ; i < nen ; i++) {
    outfile0 << title << " " << averaging_timescale << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << (1.0/pow(mini_lagr->GetV2()[i],2))/(1.0/pow(tg_unweighted_fit->GetParError(0),2)) << std::endl;
    outfilecsv << title << " " << averaging_timescale << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << (1.0/pow(mini_lagr->GetV2()[i],2))/(1.0/pow(tg_unweighted_fit->GetParError(0),2)) << std::endl;
  }
  outfile0 << title << " tg_unweighted = " << tg_unweighted_fit->GetParameter(0) << " +- " << tg_unweighted_fit->GetParError(0) << std::endl;
  outfilecsv << title << " tg_unweighted = " << tg_unweighted_fit->GetParameter(0) << " +- " << tg_unweighted_fit->GetParError(0) << std::endl;
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));



  p1 = c1->cd();
  gStyle->SetOptStat(122221);
  p1->SetLogy(1);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("%s(%s.%s_mean-%e*%s)/(%e*1.0)",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),unweighted_mean,units.Data(),unweighted_scalefactor),cut,"");
  TH1F* th_pull_unweighted = (TH1F*) p1->FindObject("htemp");
  if (th_pull_unweighted) {
    th_pull_unweighted->SetNameTitle(Form("Unweighted pull - %s",title.Data()),Form("Unweighted pull;Pull"));
    Printf("th_pull_unweighted = %e +- %e",th_pull_unweighted->GetMean(),th_pull_unweighted->GetRMS());
    outfile0 << title << " th_pull_unweighted = " << th_pull_unweighted->GetMean() << " +- " << th_pull_unweighted->GetRMS() << std::endl;
    outfilecsv << title << " th_pull_unweighted = " << th_pull_unweighted->GetMean() << " +- " << th_pull_unweighted->GetRMS() << std::endl;
  }
  c1->SaveAs(Form("singles_plots_tables/%s.pdf)",corrections_pdfname.Data()));

  //label->Draw("same");
  outfile0.close();
  outfilecsv.close();


}

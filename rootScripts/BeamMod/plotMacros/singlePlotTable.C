#include <iostream>
#include <fstream>
void singlePlotTable(TString otherTree = "mini_eigen_lagr_allbpms_part_avg_det_asyms_det_weighted", TString mod_draw = "", TString otherDraw = "lagr_asym_manual_main_det", TString units = "ppb", TString mod_cut = "", TString averaging_timescale = "crex_part", TString DataSetCut = "ErrorFlag", TString type = "part", TString suffix = "_main_det_corrections", Int_t pullZero = 1, TString title = "") { 
  TString averaging_timescale_xaxis = averaging_timescale;
  TString averaging_timescale_old_variable = averaging_timescale;
  TString lagr_suffix = suffix;
  TString timescale_plus = averaging_timescale;
  //if (DataSetCut == "ErrorFlag" && suffix.Contains("kinematics")) {
  //  lagr_suffix = suffix(11,suffix.Length());
  //}
  if (averaging_timescale.Contains("Entry")) {
    averaging_timescale_xaxis = "Wien";
    averaging_timescale_old_variable = "rcdb_flip_state";
    timescale_plus = "1+Entry$";//"1.0+rcdb_flip_state";
  }
  if (averaging_timescale.Contains("pitt")) {
    timescale_plus = "1.0+crex_pitt";
  }
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
  mini_lagr->Add(Form("processed_respin2_data/results/CREX_All_%s_%s_Avg_Outputs%s.root",DataSetCut.Data(),averaging_timescale_old_variable.Data(),lagr_suffix.Data()));
  mini_lagr->SetScanField(0);
  //TTree * mini_other = new TTree(otherTree,otherTree);
  //mini_other = (TTree*)data_file.Get(otherTree);
  TChain * mini_other = new TChain(otherTree);
  mini_other->Add(Form("processed_respin2_data/results/CREX_All_%s_%s_Avg_Outputs%s.root",DataSetCut.Data(),averaging_timescale_old_variable.Data(),suffix.Data()));
  mini_other->SetScanField(0);


  // Units
  mini_lagr->SetAlias("ppm","1e-6");
  mini_lagr->SetAlias("ppb","1e-9");
  mini_lagr->SetAlias("mm","1.0");
  mini_lagr->SetAlias("um","1e-3");
  mini_lagr->SetAlias("nm","1e-6");
  mini_lagr->SetAlias("murad","1e-3");
  mini_lagr->SetAlias("nrad","1e-6");
  mini_lagr->SetAlias("dpp","(um/murad)");

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
  corrections_pdfname_mod_draw_suffix.ReplaceAll("255.70923+","Unblinded_").ReplaceAll("*","").ReplaceAll("&","").ReplaceAll("=","").ReplaceAll("|","").ReplaceAll(">","").ReplaceAll("<","").ReplaceAll(".","").ReplaceAll("!","").ReplaceAll(" ","");

  TString corrections_texname = type+"AvgAllEvMonLagr_"+otherTree+"_"+otherDraw+"_"+ averaging_timescale_old_variable +"_wise_" +DataSetCut+corrections_pdfname_mod_draw_suffix+corrections_pdfname_mod_cut_suffix+suffix+".txt";
  TString corrections_csvname = type+"AvgAllEvMonLagr_"+otherTree+"_"+otherDraw+"_"+ averaging_timescale_old_variable +"_wise_" +DataSetCut+corrections_pdfname_mod_draw_suffix+corrections_pdfname_mod_cut_suffix+suffix+".csv";
  TString corrections_pdfname = type+"AvgAllEvMonLagr_"+otherTree+"_"+otherDraw+"_"+ averaging_timescale_old_variable +"_wise_" +DataSetCut+corrections_pdfname_mod_draw_suffix+corrections_pdfname_mod_cut_suffix+suffix;

  std::ofstream outfile0;
  std::ofstream outfilecsv;
  outfile0.open("singles_plots_tables/"+corrections_texname,std::ofstream::out);
  outfilecsv.open("singles_plots_tables/"+corrections_csvname,std::ofstream::out);


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

  mini_lagr->SetAlias("lagr_asym_manual_main_det_mean_err","eigen_lagr_asym_manual_main_det_mean_err");


  // Maindet weighted mean calculation and fit bar, self error bar plot
  p1 = c1->cd();
  p1->SetLeftMargin(0.125);
  p1->SetLogy(0);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("pow(%s.%s_mean_err/lagr_asym_manual_main_det_mean_err,2):lagr_asym_manual_main_det_mean_err/%s:%s",otherTree.Data(),otherDraw.Data(),units.Data(),timescale_plus.Data()),cut,"goff");
  TGraphErrors* tg_maindet_scalefactor = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_maindet_scalefactor->Fit("pol0");
  TF1* tg_maindet_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t maindet_scalefactor = sqrt(tg_maindet_scalefactor_fit->GetParameter(0));

  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%e*lagr_asym_manual_main_det_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),maindet_scalefactor,units.Data(),timescale_plus.Data()),cut,"goff");
  TGraphErrors* tg_self_maindet = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_self_maindet->SetNameTitle(Form("Maindet weighted %s",title.Data()),Form("Maindet weighted %s;%s;%s (%s)",title.Data(),averaging_timescale_xaxis.Data(),title.Data(),units.Data()));
  tg_self_maindet->SetMarkerStyle(20);
  tg_self_maindet->SetMarkerSize(1.0);
  tg_self_maindet->Fit("pol0");
  tg_self_maindet->Draw("ap");
  TF1* tg_self_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t self_maindet_mean = tg_self_maindet_fit->GetParameter(0);
  Double_t self_maindet_mean_legend = tg_self_maindet_fit->GetParameter(0);
  Double_t self_maindet_mean_err = tg_self_maindet_fit->GetParError(0);
  Double_t self_maindet_chi2 = tg_self_maindet_fit->GetChisquare();
  Double_t self_maindet_prob = tg_self_maindet_fit->GetProb();
  Int_t self_maindet_ndof = tg_self_maindet_fit->GetNDF();
  if (pullZero == 1) self_maindet_mean = 0;
  Printf("tg_self_maindet = %e +- %e",tg_self_maindet_fit->GetParameter(0),tg_self_maindet_fit->GetParError(0));
  std::map<Double_t,Double_t> weights;
  for (Int_t i = 0 ; i < nen ; i++) {
    weights[mini_lagr->GetV3()[i]] = pow(mini_lagr->GetV2()[i],-2)/pow(tg_self_maindet_fit->GetParError(0),-2);
  }

  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%s.%s_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),timescale_plus.Data()),cut,"goff");
  TGraphErrors* tg_self_only_maindet = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_self_only_maindet->SetNameTitle(Form("Maindet weighted avg, local errors, %s",title.Data()),Form("Maindet weighted avg, local errors - %s;%s;%s (%s)",title.Data(),averaging_timescale_xaxis.Data(),title.Data(),units.Data()));
  tg_self_only_maindet->SetMarkerStyle(20);
  tg_self_only_maindet->SetMarkerSize(1.0);
  tg_self_only_maindet->Draw("ap");
  
  p1->Modified();
  p1->Update();
  TLine *l=new TLine(p1->GetUxmin(),self_maindet_mean,p1->GetUxmax(),self_maindet_mean);
  l->SetLineColor(kRed);
  l->SetLineWidth(2);
  l->Draw();
  //gStyle->SetOptFit(11);
  //tg_self_only_maindet->Fit("pol0");
  //p1->Modified();
  //p1->Update();
  //TF1* tg_self_only_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  //TPaveStats* statbox = (TPaveStats *)tg_self_only_maindet->GetListOfFunctions()->FindObject("stats");
  //statbox->SetName("mystatbox");
  //TList *listoflines = statbox->GetListOfLines();
  //TText * tt1 = statbox->GetLineWith("ndf");
  //listoflines->Remove(tt1);
  //tg_self_only_maindet->GetListOfFunctions()->Remove(tg_self_only_maindet_fit);
  //tg_self_only_maindet->GetHistogram()->SetStats(0);
  //p1->Modified();

  //TLatex *myt = new TLatex(0,0,"Test = 10");
  //myt ->SetTextFont(42);
  //myt ->SetTextSize(0.04);
  //myt ->SetTextColor(kBlack);
  //listoflines->Add(myt);
  c1->cd();
  TText *label1;
  if (units == "dpp" || units == "nrad") {
    label1 = new TText(0.1,0.91,Form("Mean = %0.1e +- %0.0e %s",self_maindet_mean_legend,self_maindet_mean_err,units.Data()));
  }
  else {
    label1 = new TText(0.1,0.91,Form("Mean = %0.1f +- %0.1f %s",self_maindet_mean_legend,self_maindet_mean_err,units.Data()));
  }
  label1->SetTextFont(62);
  label1->SetTextSize(0.035);
  label1->SetTextColor(4);
  label1->SetNDC();
  label1->Draw("same");
  TText *label2 = new TText(0.45,0.91,Form("Chi2 / NDOF = %0.1f / %d",self_maindet_chi2,self_maindet_ndof));
  label2->SetTextFont(62);
  label2->SetTextSize(0.03);
  label2->SetNDC();
  label2->Draw("same");
  TText *label3 = new TText(0.7,0.91,Form("Probability = %0.2f",self_maindet_prob));
  label3->SetTextFont(62);
  label3->SetTextSize(0.03);
  label3->SetNDC();
  label3->Draw("same");
  //p1->Modified();
  //p1->Update();

  outfile0 << "\\begin{frame}[allowframebreaks]" << std::endl;
  outfile0 << "  \\frametitle{" << otherDraw << " " << averaging_timescale_xaxis << "}" << std::endl;

  outfile0 << "  \\begin{figure}" << std::endl;
  outfile0 << "    \\centering" << std::endl;
  outfile0 << "    \\includegraphics[page=1,width=0.9\\textwidth]{images/" << corrections_pdfname << ".pdf} " << std::endl;
  outfile0 << "      \\caption{Main detector weighted average " << otherDraw << " ( " << units << ") vs. " << averaging_timescale_xaxis << ".}" << std::endl;
  outfile0 << "    \\label{fig:" << otherDraw <<"_"<<averaging_timescale_xaxis << "_p0}" << std::endl;
  outfile0 << "  \\end{figure}" << std::endl;
  outfile0 << "  " << std::endl;
  outfile0 << "  \\begin{figure}" << std::endl;
  outfile0 << "    \\centering" << std::endl;
  outfile0 << "    \\includegraphics[page=2,width=0.9\\textwidth]{images/" << corrections_pdfname << ".pdf} " << std::endl;
  outfile0 << "      \\caption{Main detector weighted average " << otherDraw << " ( " << units << ") vs. " << averaging_timescale_xaxis << " pull.}" << std::endl;
  outfile0 << "    \\label{fig:" << otherDraw <<"_"<<averaging_timescale_xaxis << "_pull}" << std::endl;
  outfile0 << "  \\end{figure}" << std::endl;
  outfile0 << "  " << std::endl;
  outfile0 << "  \\begin{table} " << std::endl;
  outfile0 << "    \\caption{" << otherDraw << " vs. " << averaging_timescale_xaxis  << ", main-det weighted}"<< std::endl;
  outfile0 << "    \\begin{tabular}{cccc}"<< std::endl;
  outfile0 << "    \\cline{1-4}"<< std::endl;
  outfile0 << "    \\multicolumn{1}{|c|}{Averaging} &  \\multicolumn{1}{c|}{Value}  & \\multicolumn{1}{c|}{Error}  & \\multicolumn{1}{c|}{Weight into avg} \\\\ \\hline"<< std::endl;
  outfilecsv << title << " " << averaging_timescale_xaxis << ", time, Value, local error bar, weight into Maindet weighted mean" << std::endl;
  for (Int_t i = 0 ; i < nen ; i++) {
    outfile0 << "    \\multicolumn{1}{|c|}{" << averaging_timescale_xaxis << " " << mini_lagr->GetV3()[i] << "}  &  \\multicolumn{1}{c|}{" << mini_lagr->GetV1()[i] << "}  & \\multicolumn{1}{c|}{" << mini_lagr->GetV2()[i] << "}  & \\multicolumn{1}{c|}{" <<  100*weights.at(mini_lagr->GetV3()[i]) << "\\%} \\\\ \\hline"<< std::endl;
    outfilecsv << title << " " << averaging_timescale_xaxis << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << mini_lagr->GetV2()[i] << 100*weights.at(mini_lagr->GetV3()[i]) << "%" << std::endl;
  }
  outfile0 << "    \\multicolumn{1}{|c|}{Main det weighted average} &  \\multicolumn{1}{c|}{"<< tg_self_maindet_fit->GetParameter(0) << "\\pm " << tg_self_maindet_fit->GetParError(0) <<"}  & \\multicolumn{1}{c|}{Chi2/NDOF = " << self_maindet_chi2 << "/" <<  self_maindet_ndof << "}  & \\multicolumn{1}{c|}{Probability = " << self_maindet_prob << "} \\\\ \\hline"<< std::endl;
  outfile0 << "  \\end{tabular}"<< std::endl;
  outfile0 << "  \\end{table}"<< std::endl;
  outfile0 << "\\end{frame}"<< std::endl;
  outfile0 << title << " maindet weighted average, " << tg_self_maindet_fit->GetParameter(0) << " +- " << tg_self_maindet_fit->GetParError(0) << std::endl;
  outfilecsv << title << " maindet weighted average, " << tg_self_maindet_fit->GetParameter(0) << " +- " << tg_self_maindet_fit->GetParError(0) << std::endl;
  c1->SaveAs(Form("singles_plots_tables/%s.pdf(",corrections_pdfname.Data()));
  c1->SaveAs(Form("singles_plots_tables/p0_%s.pdf",corrections_pdfname.Data()));


  p1 = c1->cd();
  p1->SetLeftMargin(0.125);
  gStyle->SetOptStat(122221);
  p1->SetLogy(1);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("(%s%s.%s_mean-%e*%s)/(%s.%s_mean_err)",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),self_maindet_mean,units.Data(),otherTree.Data(),otherDraw.Data()),cut,"");
  TH1F* th_pull_self_maindet = (TH1F*) p1->FindObject("htemp");
  if (th_pull_self_maindet) {
    th_pull_self_maindet->SetNameTitle(Form("Maindet weighted mean, self pull"),Form("Maindet weighted mean, self pull - %s;Pull",title.Data()));
    Printf("th_pull_self_maindet = %e +- %e",th_pull_self_maindet->GetMean(),th_pull_self_maindet->GetRMS());
    outfile0 << title << " maindet weighted mean, self pull = " << th_pull_self_maindet->GetMean() << " +- " << th_pull_self_maindet->GetRMS() << std::endl;
    outfilecsv << title << " maindet weighted mean, self pull = " << th_pull_self_maindet->GetMean() << " +- " << th_pull_self_maindet->GetRMS() << std::endl;
  }
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));
  c1->SaveAs(Form("singles_plots_tables/pull_%s.pdf",corrections_pdfname.Data()));





  // Self weighted
  p1 = c1->cd();
  p1->SetLeftMargin(0.125);
  p1->SetLogy(0);
  //p1->SetLeftMargin(0.125);
  //Printf("%s(%s.%s_mean)/%s:%s.%s_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),timescale_plus.Data());
  //nen = mini_lagr->Scan(Form("%s(%s.%s_mean)/%s:%s.%s_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),timescale_plus.Data()),"");
  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%s.%s_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),timescale_plus.Data()),cut,"goff");
  TGraphErrors* tg_self = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_self->SetNameTitle(Form("Self weighted %s",title.Data()),Form("Self weighted %s;%s;%s (%s)",title.Data(),averaging_timescale_xaxis.Data(),title.Data(),units.Data()));
  tg_self->SetMarkerStyle(20);
  tg_self->SetMarkerSize(1.0);
  tg_self->Fit("pol0");
  tg_self->Draw("ap");
  TF1* tg_self_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t self_mean = tg_self_fit->GetParameter(0);
  if (pullZero == 1) self_mean = 0;
  Printf("tg_self = %e +- %e",tg_self_fit->GetParameter(0),tg_self_fit->GetParError(0));
  outfile0 << title << " " << averaging_timescale_xaxis << ", time, Value, Value Error" << std::endl;
  outfilecsv << title << " " << averaging_timescale_xaxis << ", time, Value, Value Error" << std::endl;
  for (Int_t i = 0 ; i < nen ; i++) {
    outfile0 << title << " " << averaging_timescale_xaxis << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << mini_lagr->GetV2()[i] << std::endl;
    outfilecsv << title << " " << averaging_timescale_xaxis << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << mini_lagr->GetV2()[i] << std::endl;
  }
  outfile0 << title << " total average, " << tg_self_fit->GetParameter(0) << " +- " << tg_self_fit->GetParError(0) << std::endl;
  outfilecsv << title << " total average, " << tg_self_fit->GetParameter(0) << " +- " << tg_self_fit->GetParError(0) << std::endl;
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));


  p1 = c1->cd();
  p1->SetLeftMargin(0.125);
  p1->SetLogy(0);
  gStyle->SetOptStat(122221);
  //p1->SetLeftMargin(0.125);
  //mini_lagr->Scan(Form("rcdb_slug:rcdb_flip_state:rcdb_ihwp:rcdb_sign:run_number:minirun_n:asym_main_det_nentries:%s(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/asym_main_det_eigen_lagr_eigen_reg_diff_mean_error",mod_draw.Data()),cut+((TCut)((TString)Form("abs(%s(asym_main_det_eigen_lagr_eigen_reg_diff_mean)/asym_main_det_eigen_lagr_eigen_reg_diff_mean_error)>2",mod_draw.Data()))));
  p1->SetLogy(1);
  nen = mini_lagr->Draw(Form("(%s%s.%s_mean-%e*%s)/%s.%s_mean_err",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),self_mean,units.Data(),otherTree.Data(),otherDraw.Data()),cut,"");
  TH1F* th_pull_self = (TH1F*)p1->FindObject("htemp");
  if (th_pull_self) {
    th_pull_self->SetNameTitle(Form("Self weighted pull"),Form("Self weighted pull - %s;Pull",title.Data()));
    Printf("th_pull_self = %e +- %e",th_pull_self->GetMean(),th_pull_self->GetRMS());
    outfile0 << title << " pull plot stats = " << th_pull_self->GetMean() << " +- " << th_pull_self->GetRMS() << std::endl;
    outfilecsv << title << " pull plot stats = " << th_pull_self->GetMean() << " +- " << th_pull_self->GetRMS() << std::endl;
  }
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));




  // Maindet weighted
  p1 = c1->cd();
  p1->SetLeftMargin(0.125);
  p1->SetLogy(0);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%e*lagr_asym_manual_main_det_mean_err/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),maindet_scalefactor,units.Data(),timescale_plus.Data()),cut,"goff");
  TGraphErrors* tg_maindet = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_maindet->SetNameTitle(Form("Maindet weighted %s",title.Data()),Form("Maindet weighted %s;%s;%s (%s)",title.Data(),averaging_timescale_xaxis.Data(),title.Data(),units.Data()));
  tg_maindet->SetMarkerStyle(20);
  tg_maindet->SetMarkerSize(1.0);
  tg_maindet->Fit("pol0");
  tg_maindet->Draw("ap");
  TF1* tg_maindet_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t maindet_mean = tg_maindet_fit->GetParameter(0);
  if (pullZero == 1) maindet_mean = 0;
  Printf("tg_maindet = %e +- %e",tg_maindet_fit->GetParameter(0),tg_maindet_fit->GetParError(0));
  outfile0 << title << " " << averaging_timescale_xaxis << ", time, Value, Avg Weight" << std::endl;
  outfilecsv << title << " " << averaging_timescale_xaxis << ", time, Value, Avg Weight" << std::endl;
  for (Int_t i = 0 ; i < nen ; i++) {
    outfile0 << title << " " << averaging_timescale_xaxis << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << (1.0/pow(mini_lagr->GetV2()[i],2))/(1.0/pow(tg_maindet_fit->GetParError(0),2)) << std::endl;
    outfilecsv << title << " " << averaging_timescale_xaxis << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << (1.0/pow(mini_lagr->GetV2()[i],2))/(1.0/pow(tg_maindet_fit->GetParError(0),2)) << std::endl;
  }
  outfile0 << title << " maindet weighted average, " << tg_maindet_fit->GetParameter(0) << " +- " << tg_maindet_fit->GetParError(0) << std::endl;
  outfilecsv << title << " maindet weighted average, " << tg_maindet_fit->GetParameter(0) << " +- " << tg_maindet_fit->GetParError(0) << std::endl;
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));


  p1 = c1->cd();
  p1->SetLeftMargin(0.125);
  gStyle->SetOptStat(122221);
  p1->SetLogy(1);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("(%s%s.%s_mean-%e*%s)/(%e*lagr_asym_manual_main_det_mean_err)",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),maindet_mean,units.Data(),maindet_scalefactor),cut,"");
  TH1F* th_pull_maindet = (TH1F*) p1->FindObject("htemp");
  if (th_pull_maindet) {
    th_pull_maindet->SetNameTitle(Form("Maindet weighted pull"),Form("Maindet weighted pull - %s;Pull",title.Data()));
    Printf("th_pull_maindet = %e +- %e",th_pull_maindet->GetMean(),th_pull_maindet->GetRMS());
    outfile0 << title << " maindet weighted pull (not meaningful) = " << th_pull_maindet->GetMean() << " +- " << th_pull_maindet->GetRMS() << std::endl;
    outfilecsv << title << " maindet weighted pull (not meaningful) = " << th_pull_maindet->GetMean() << " +- " << th_pull_maindet->GetRMS() << std::endl;
  }
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));



  // equal weighted
  p1 = c1->cd();
  p1->SetLeftMargin(0.125);
  p1->SetLogy(0);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("pow((%s.%s_mean_err)/(1.0),2):1.0/%s:%s",otherTree.Data(),otherDraw.Data(),units.Data(),timescale_plus.Data()),cut,"goff");
  TGraphErrors* tg_equal_weighted_scalefactor = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_equal_weighted_scalefactor->Fit("pol0");
  TF1* tg_equal_weighted_scalefactor_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t equal_weighted_scalefactor = sqrt(tg_equal_weighted_scalefactor_fit->GetParameter(0));
  Printf("equal_weight scalefactor = %e",equal_weighted_scalefactor);

  nen = mini_lagr->Draw(Form("%s(%s.%s_mean)/%s:%e*1.0/%s:%s",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),units.Data(),equal_weighted_scalefactor,units.Data(),timescale_plus.Data()),cut,"goff");
  TGraphErrors* tg_equal_weighted = new TGraphErrors(nen,mini_lagr->GetV3(),mini_lagr->GetV1(),0,mini_lagr->GetV2());
  tg_equal_weighted->SetNameTitle(Form("Equally weighted %s",title.Data()),Form("Equally weighted %s;%s;%s (%s)",title.Data(),averaging_timescale_xaxis.Data(),title.Data(),units.Data()));
  tg_equal_weighted->Fit("pol0");
  tg_equal_weighted->Draw("ap");
  TF1* tg_equal_weighted_fit = (TF1*)gROOT->GetFunction("pol0");
  Double_t equal_weighted_mean = tg_equal_weighted_fit->GetParameter(0);
  if (pullZero == 1) equal_weighted_mean = 0;
  Printf("tg_equal_weighted = %e +- %e",tg_equal_weighted_fit->GetParameter(0),tg_equal_weighted_fit->GetParError(0));
  outfile0 << title << " " << averaging_timescale_xaxis << ", time, Value, Equally weighted Weight" << std::endl;
  outfilecsv << title << " " << averaging_timescale_xaxis << ", time, Value, Equally weighted Weight" << std::endl;
  for (Int_t i = 0 ; i < nen ; i++) {
    outfile0 << title << " " << averaging_timescale_xaxis << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << (1.0/pow(mini_lagr->GetV2()[i],2))/(1.0/pow(tg_equal_weighted_fit->GetParError(0),2)) << std::endl;
    outfilecsv << title << " " << averaging_timescale_xaxis << "," << mini_lagr->GetV3()[i] << "," << mini_lagr->GetV1()[i] << "," << (1.0/pow(mini_lagr->GetV2()[i],2))/(1.0/pow(tg_equal_weighted_fit->GetParError(0),2)) << std::endl;
  }
  outfile0 << title << " tg_equal_weighted = " << tg_equal_weighted_fit->GetParameter(0) << " +- " << tg_equal_weighted_fit->GetParError(0) << std::endl;
  outfilecsv << title << " tg_equal_weighted = " << tg_equal_weighted_fit->GetParameter(0) << " +- " << tg_equal_weighted_fit->GetParError(0) << std::endl;
  c1->SaveAs(Form("singles_plots_tables/%s.pdf",corrections_pdfname.Data()));



  p1 = c1->cd();
  p1->SetLeftMargin(0.125);
  gStyle->SetOptStat(122221);
  p1->SetLogy(1);
  //p1->SetLeftMargin(0.125);
  nen = mini_lagr->Draw(Form("(%s%s.%s_mean-%e*%s)/(%e*1.0)",mod_draw.Data(),otherTree.Data(),otherDraw.Data(),equal_weighted_mean,units.Data(),equal_weighted_scalefactor),cut,"");
  TH1F* th_pull_equal_weighted = (TH1F*) p1->FindObject("htemp");
  if (th_pull_equal_weighted) {
    th_pull_equal_weighted->SetNameTitle(Form("Equally weighted pull"),Form("Equally weighted pull - %s;Pull",title.Data()));
    Printf("th_pull_equal_weighted = %e +- %e",th_pull_equal_weighted->GetMean(),th_pull_equal_weighted->GetRMS());
    outfile0 << title << " th_pull_equal_weighted = " << th_pull_equal_weighted->GetMean() << " +- " << th_pull_equal_weighted->GetRMS() << std::endl;
    outfilecsv << title << " th_pull_equal_weighted = " << th_pull_equal_weighted->GetMean() << " +- " << th_pull_equal_weighted->GetRMS() << std::endl;
  }
  c1->SaveAs(Form("singles_plots_tables/%s.pdf)",corrections_pdfname.Data()));

  //label->Draw("same");
  outfile0.close();
  outfilecsv.close();


}

#include <TMath.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TLine.h>
#include <TLegend.h>
#include <TVector2.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TProfile.h>
#include <sstream>
#include <iostream>
#include <TGraphErrors.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;
class TF1 *f1trap[42];
int DoublePeak() { 
  //MAKE SINGLE DIFFERENCE, DOUBLE DIFFERENCE, VERSUS PLOTS FOR BEAM DATA
  //gStyle->SetOptStat(0);
  //Change run number here 
  TFile *file1=new TFile("/adaq1/work1/apar/japanOutput/prexPrompt_pass2_3523.000.root");
  int run = 3523;
  Double_t trim_base[8] = {6831,6730,6967,6820,6895,6690,6870,6830};
  Double_t supercyc[7]={20.0,21.0,22.0,23.0,24.0,25.0,26.0};
  Double_t supercycslope[7]={147420.0,147421.0,147422.0,147423.0,147424.0,147425.0,147426.0};

  //  TGraphErrors *test;
  TTree *tree_R = (TTree*) file1->Get("mul");
  Double_t        CodaEventNumber;
  Double_t        ErrorFlag;
 
  //for some reason, these sam definitions have to be present for it to work??? 
  Double_t        yield_sam5_hw_sum;   
  Double_t        yield_sam6_hw_sum;  
  Double_t        yield_sam7_hw_sum;
  Double_t        yield_sam8_hw_sum;
  
  //Double_t        asym_cav4bQ_hw_sum;
  //Double_t        asym_cav4cQ_hw_sum;
  //Double_t        asym_cav4dQ_hw_sum;
  Double_t        asym_bcm_an_us;
  Double_t        asym_bcm_an_ds;
  Double_t        asym_bcm0l02;
  Double_t        asym_bcm_dg_us;
  Double_t        asym_bcm_dg_ds;
  Double_t        actual_pattern_polarity;
  Double_t        actual_previous_pattern_polarity;
  

  TBranch        *b_CodaEventNumber;
  //TBranch        *b_asym_cav4bQ;
  //TBranch        *b_asym_cav4cQ;
  //TBranch        *b_asym_cav4dQ;
  TBranch        *b_asym_bcm_an_us; //!
  TBranch        *b_asym_bcm_an_ds; //!  
  TBranch        *b_asym_bcm0l02;
  TBranch        *b_asym_bcm_dg_us;
  TBranch        *b_asym_bcm_dg_ds;
  TBranch        *b_ErrorFlag;
  TBranch        *b_actual_pattern_polarity;
  TBranch        *b_actual_previous_pattern_polarity;

  //first thing: name for future reference (anything)
  //second thing: leaf within a branch (Double_t name)
  //third thing: TBranch pointer ('annoying Root feature'- Victoria)
  tree_R->SetBranchAddress("CodaEventNumber", &CodaEventNumber, &b_CodaEventNumber);
  tree_R->SetBranchAddress("ErrorFlag", &ErrorFlag, &b_ErrorFlag);
 
  tree_R->SetBranchAddress("asym_bcm_an_us", &asym_bcm_an_us, &b_asym_bcm_an_us);
  tree_R->SetBranchAddress("asym_bcm_an_ds", &asym_bcm_an_ds, &b_asym_bcm_an_ds);
  tree_R->SetBranchAddress("asym_bcm0l02", &asym_bcm0l02, &b_asym_bcm0l02);
  tree_R->SetBranchAddress("asym_bcm_dg_us", &asym_bcm_dg_us, &b_asym_bcm_dg_us);
  tree_R->SetBranchAddress("asym_bcm_dg_ds", &asym_bcm_dg_ds, &b_asym_bcm_dg_ds);
  tree_R->SetBranchAddress("actual_pattern_polarity", &actual_pattern_polarity, &b_actual_pattern_polarity);
  tree_R->SetBranchAddress("actual_previous_pattern_polarity", &actual_previous_pattern_polarity, &b_actual_previous_pattern_polarity);
  cout << "working..." << endl;
  
  //cuts based on previous, current polarity 
  TString evcut_p0c0 = "CodaEventNumber>10 && actual_previous_pattern_polarity==0 && actual_pattern_polarity==0";
  TString evcut_p0c1 = "CodaEventNumber>10 && actual_previous_pattern_polarity==0 && actual_pattern_polarity==1";
  TString evcut_p1c0 = "CodaEventNumber>10 && actual_previous_pattern_polarity==1 && actual_pattern_polarity==0";
  TString evcut_p1c1 = "CodaEventNumber>10 && actual_previous_pattern_polarity==1 && actual_pattern_polarity==1";
  
  //apply cuts to the branches, write into histograms 
  TH1F *an_us00 = new TH1F("an_us00", "Prev. 0, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_an_us>>an_us00", evcut_p0c0, "goff");
  TH1F *an_us01 = new TH1F("an_us01", "Prev. 0, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_an_us>>an_us01", evcut_p0c1, "goff");
  TH1F *an_us10 = new TH1F("an_us10", "Prev. 1, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_an_us>>an_us10", evcut_p1c0, "goff");
  TH1F *an_us11 = new TH1F("an_us11", "Prev. 1, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_an_us>>an_us11", evcut_p1c1, "goff");

  //GetMean test
  float mean = an_us11->GetMean();
  cout << mean << endl;
  //plot the things
  TCanvas *doublepeak = new TCanvas("Double Peak Analysis");
  doublepeak->Divide(2, 3);
  doublepeak->cd(1);
  an_us00->SetLineColor(kRed);
  an_us00->SetLineWidth(2);
  an_us00->SetTitle("bcm_an_us");
  an_us00->Draw();
  an_us01->SetLineColor(kBlue);
  an_us01->SetLineWidth(2);
  an_us01->Draw("same");
  an_us10->SetLineColor(kGreen);
  an_us10->SetLineWidth(2);
  an_us10->Draw("same");
  an_us11->SetLineColor(kMagenta);
  an_us11->SetLineWidth(2);
  an_us11->Draw("same");
  //width, height
  gPad->BuildLegend(0.25, 0.21, 0.25, 0.21, Form("Legend: Run %d", run));


  //do the same for all BCMs (I could probably make this a loop but I most certainly will not at this hour)
  TH1F *an_ds00 = new TH1F("an_ds00", "Prev. 0, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_an_ds>>an_ds00", evcut_p0c0, "goff");
  TH1F *an_ds01 = new TH1F("an_ds01", "Prev. 0, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_an_ds>>an_ds01", evcut_p0c1, "goff");
  TH1F *an_ds10 = new TH1F("an_ds10", "Prev. 1, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_an_ds>>an_ds10", evcut_p1c0, "goff");
  TH1F *an_ds11 = new TH1F("an_ds11", "Prev. 1, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_an_ds>>an_ds11", evcut_p1c1, "goff");

  

  doublepeak->cd(2);//TCanvas *bcm_an_ds = new TCanvas("bcm_an_ds");
  an_ds00->SetLineColor(kRed);
  an_ds00->SetLineWidth(2);
  an_ds00->SetTitle("bcm_an_ds");
  an_ds00->Draw();
  an_ds01->SetLineColor(kBlue);
  an_ds01->SetLineWidth(2);
  an_ds01->Draw("same");
  an_ds10->SetLineColor(kGreen);
  an_ds10->SetLineWidth(2);
  an_ds10->Draw("same");
  an_ds11->SetLineColor(kMagenta);
  an_ds11->SetLineWidth(2);
  an_ds11->Draw("same");
  //width, height
  gPad->BuildLegend(0.25, 0.21, 0.25, 0.21, "Legend");

  //bcm0l02
  cout << tree_R << endl;
  TH1F *l02_00 = new TH1F("l02_00", "Prev. 0, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm0l02>>l02_00", evcut_p0c0, "goff");
  cout << tree_R << endl;
  TH1F *l02_01 = new TH1F("l02_01", "Prev. 0, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm0l02>>l02_01", evcut_p0c1, "goff");
  TH1F *l02_10 = new TH1F("l02_10", "Prev. 1, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm0l02>>l02_10", evcut_p1c0, "goff");
  TH1F *l02_11 = new TH1F("l02_11", "Prev. 1, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm0l02>>l02_11", evcut_p1c1, "goff");

  doublepeak->cd(3); //TCanvas *bcm_0l02 = new TCanvas("bcm_0l02");
  l02_00->SetLineColor(kRed);
  l02_00->SetLineWidth(2);
  l02_00->SetTitle("bcm_0l02");
  l02_00->Draw();
  l02_01->SetLineColor(kBlue);
  l02_01->SetLineWidth(2);
  l02_01->Draw("same");
  l02_10->SetLineColor(kGreen);
  l02_10->SetLineWidth(2);
  l02_10->Draw("same");
  l02_11->SetLineColor(kMagenta);
  l02_11->SetLineWidth(2);
  l02_11->Draw("same");
  //width, height
  gPad->BuildLegend(0.25, 0.21, 0.25, 0.21, "Legend");
  
 
  //dg_us
  TH1F *dg_us00 = new TH1F("dg_us00", "Prev. 0, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_dg_us>>dg_us00", evcut_p0c0, "goff");
  TH1F *dg_us01 = new TH1F("dg_us01", "Prev. 0, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_dg_us>>dg_us01", evcut_p0c1, "goff");
  TH1F *dg_us10 = new TH1F("dg_us10", "Prev. 1, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_dg_us>>dg_us10", evcut_p1c0, "goff");
  TH1F *dg_us11 = new TH1F("dg_us11", "Prev. 1, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_dg_us>>dg_us11", evcut_p1c1, "goff");

  doublepeak->cd(4); //TCanvas *bcm_dg_us = new TCanvas("bcm_dg_us");
  dg_us00->SetLineColor(kRed);
  dg_us00->SetLineWidth(2);
  dg_us00->SetTitle("bcm_dg_us");
  dg_us00->Draw();
  dg_us01->SetLineColor(kBlue);
  dg_us01->SetLineWidth(2);
  dg_us01->Draw("same");
  dg_us10->SetLineColor(kGreen);
  dg_us10->SetLineWidth(2);
  dg_us10->Draw("same");
  dg_us11->SetLineColor(kMagenta);
  dg_us11->SetLineWidth(2);
  dg_us11->Draw("same");
  //width, height
  gPad->BuildLegend(0.25, 0.21, 0.25, 0.21, "Legend");



  //dg_ds
  TH1F *dg_ds00 = new TH1F("dg_ds00", "Prev. 0, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_dg_ds>>dg_ds00", evcut_p0c0, "goff");
  TH1F *dg_ds01 = new TH1F("dg_ds01", "Prev. 0, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_dg_ds>>dg_ds01", evcut_p0c1, "goff");
  TH1F *dg_ds10 = new TH1F("dg_ds10", "Prev. 1, Curr. 0", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_dg_ds>>dg_ds10", evcut_p1c0, "goff");
  TH1F *dg_ds11 = new TH1F("dg_ds11", "Prev. 1, Curr. 1", 100, -0.002, 0.002);
  tree_R->Draw("asym_bcm_dg_ds>>dg_ds11", evcut_p1c1, "goff");

  doublepeak->cd(5);//TCanvas *bcm_dg_ds = new TCanvas("bcm_dg_ds");
  dg_ds00->SetLineColor(kRed);
  dg_ds00->SetLineWidth(2);
  dg_ds00->SetTitle("bcm_dg_ds");
  dg_ds00->Draw();
  dg_ds01->SetLineColor(kBlue);
  dg_ds01->SetLineWidth(2);
  dg_ds01->Draw("same");
  dg_ds10->SetLineColor(kGreen);
  dg_ds10->SetLineWidth(2);
  dg_ds10->Draw("same");
  dg_ds11->SetLineColor(kMagenta);
  dg_ds11->SetLineWidth(2);
  dg_ds11->Draw("same");
  //width, height
  gPad->BuildLegend(0.25, 0.21, 0.25, 0.21, "Legend");

  doublepeak->SaveAs(Form("doublepeak%d%s", run, ".pdf"));
  
  
  
  
  return 0;
				      
}


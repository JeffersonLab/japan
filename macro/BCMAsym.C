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
int BCMAsym() { 
  //MAKE SINGLE DIFFERENCE, DOUBLE DIFFERENCE, VERSUS PLOTS FOR BEAM DATA
  gStyle->SetOptStat(0);
 
  //TFile* f_outFile = new TFile("slop_plot_1474.root", "RECREATE");
  //Change run number here 
  TFile *file1=new TFile("/adaq1/work1/apar/japanOutput/prexPrompt_pass1_4290.000.root");
  int run = 3399;
  Double_t trim_base[8] = {6831,6730,6967,6820,6895,6690,6870,6830};
  Double_t supercyc[7]={20.0,21.0,22.0,23.0,24.0,25.0,26.0};
  Double_t supercycslope[7]={147420.0,147421.0,147422.0,147423.0,147424.0,147425.0,147426.0};
  const int n=7;//number of cycle
  const double trimmin=0.38;
  const double trimmax=0.7;
  const double bpmmax=3;
  const double bpmmin=1;

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

  //first thing: name for future reference (anything)
  //second thing: leaf within a branch (Double_t name)
  //third thing: TBranch pointer ('annoying Root feature'- Victoria)
  tree_R->SetBranchAddress("CodaEventNumber", &CodaEventNumber, &b_CodaEventNumber);
  tree_R->SetBranchAddress("ErrorFlag", &ErrorFlag, &b_ErrorFlag);
  //tree_R->SetBranchAddress("asym_cav4bQ", &asym_cav4bQ_hw_sum, &b_asym_cav4bQ);
  //tree_R->SetBranchAddress("asym_cav4cQ", &asym_cav4cQ_hw_sum, &b_asym_cav4cQ);
  //tree_R->SetBranchAddress("asym_cav4dQ", &asym_cav4dQ_hw_sum, &b_asym_cav4dQ);
  tree_R->SetBranchAddress("asym_bcm_an_us", &asym_bcm_an_us, &b_asym_bcm_an_us);
  tree_R->SetBranchAddress("asym_bcm_an_ds", &asym_bcm_an_ds, &b_asym_bcm_an_ds);
  tree_R->SetBranchAddress("asym_bcm0l02", &asym_bcm0l02, &b_asym_bcm0l02);
  tree_R->SetBranchAddress("asym_bcm_dg_us", &asym_bcm_dg_us, &b_asym_bcm_dg_us);
  tree_R->SetBranchAddress("asym_bcm_dg_ds", &asym_bcm_dg_ds, &b_asym_bcm_dg_ds);
  cout << "working..." << endl;
  
  //only selects events with no beam mod 
  TString evcut = "CodaEventNumber>10 && (ErrorFlag & 0x00009000)==0";
  
  TCanvas *single_diff = new TCanvas();
  gStyle->SetOptStat();
  single_diff->Divide(2, 2);
  single_diff->SetTitle("Single difference");
  single_diff->cd(1);
  tree_R->Draw("asym_bcm_an_us>>bin4(100, -0.002, 0.002)", evcut);
  single_diff->cd(2);
  tree_R->Draw("asym_bcm_an_ds>>bin5(100, -0.002, 0.002)", evcut);
  
  single_diff->cd(3);
  tree_R->Draw("asym_bcm_dg_us>>bin88(100, -0.002, 0.002)", evcut);
  single_diff->cd(4);
  tree_R->Draw("asym_bcm_dg_ds>>bin89(100, -0.002, 0.002)", evcut);
  single_diff->SaveAs(Form("singlediff%d%s", run, ".pdf"));
 

  TCanvas *versus_plots = new TCanvas();
  gStyle->SetOptStat();
  versus_plots->SetTitle("Versus Plots");
  versus_plots->Divide(3, 3);
  versus_plots->cd(1);
  tree_R->Draw("asym_bcm_an_us:asym_bcm_an_us", evcut); 
  versus_plots->cd(2);
  tree_R->Draw("asym_bcm_an_us:asym_bcm_an_ds", evcut);
  versus_plots->cd(3);
  tree_R->Draw("asym_bcm_an_us:asym_bcm0l02", evcut);
  versus_plots->cd(4);
  tree_R->Draw("asym_bcm_an_ds:asym_bcm_an_us", evcut);
  versus_plots->cd(5);
  tree_R->Draw("asym_bcm_an_ds:asym_bcm_an_ds", evcut);
  versus_plots->cd(6);
  tree_R->Draw("asym_bcm_an_ds:asym_bcm0l02", evcut);
  versus_plots->cd(7);
  tree_R->Draw("asym_bcm0l02:asym_bcm_an_us", evcut);
  versus_plots->cd(8);
  tree_R->Draw("asym_bcm0l02:asym_bcm_an_ds", evcut);
  versus_plots->cd(9);
  tree_R->Draw("asym_bcm0l02:asym_bcm0l02", evcut);
  

  TCanvas* double_diff = new TCanvas();
  gStyle->SetOptStat();
  double_diff->SetTitle("Double Difference Plots");
  double_diff->Divide(3, 1);
  double_diff->cd(1);
  tree_R->Draw("(asym_bcm_an_us-asym_bcm_an_ds)>>bin7(100, -0.0004, 0.0004)", evcut);
  
  double_diff->cd(2);
  tree_R->Draw("(asym_bcm_an_us-asym_bcm_dg_us)>>bin10(100, -0.0004, 0.0004)", evcut);
  double_diff->cd(3);
  tree_R->Draw("(asym_bcm_an_ds-asym_bcm_dg_ds)>>bin11(100, -0.0004, 0.0004)", evcut);
  double_diff->SaveAs(Form("doublediff%d%s", run, ".pdf"));
 
  
  
  return 0;
				      
}

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
void AsymTime() { 
  //PLOT ASYM VERSUS TIME FOR DOUBLE PEAK RUNS

  //Change run number here 
  //int numruns = 1;
  int run = 3901;
  TFile *file1 =new TFile(Form("/adaq1/work1/apar/japanOutput/prexPrompt_pass1_%d.000.root", run));
  //export files for means, errors
  ofstream meanfile;
  ofstream errfile;
  //make the trees
  TTree *tree_mul = (TTree*) file1->Get("mul");

  //define event cuts based on previous, current polarity 
  TString evcut_p0c0 = "ErrorFlag==0 && CodaEventNumber>10 && actual_previous_pattern_polarity==0 && actual_pattern_polarity==0";
  TString evcut_p0c1 = "ErrorFlag==0 && CodaEventNumber>10 && actual_previous_pattern_polarity==0 && actual_pattern_polarity==1";
  TString evcut_p1c0 = "ErrorFlag==0 && CodaEventNumber>10 && actual_previous_pattern_polarity==1 && actual_pattern_polarity==0";
  TString evcut_p1c1 = "ErrorFlag==0 && CodaEventNumber>10 && actual_previous_pattern_polarity==1 && actual_pattern_polarity==1";
  TString allcut = "ErrorFlag==0 && CodaEventNumber<130000 && CodaEventNumber>95000";
  //make plots versus time of things with no polarity cuts
  TCanvas *c_asym_bcm_an_us_all = new TCanvas("c_asym_bcm_an_us_all");
  tree_mul->Draw("asym_bcm_an_us:CodaEventNumber", allcut);
  TCanvas *c_asym_bcm_an_ds_all = new TCanvas("c_asym_bcm_an_ds_all");
  tree_mul->Draw("asym_bcm_an_ds:CodaEventNumber", allcut);

  TString cuts[4] = {evcut_p0c0, evcut_p0c1, evcut_p1c0, evcut_p1c1};
  cout << "event cuts defined...." <<endl;

  //make canvases
  TCanvas* c_asym_bcm_an_us = new TCanvas("asym_bcm_an_us");
  TCanvas *c_asym_bcm_an_ds = new TCanvas("asym_bcm_an_ds");
  TCanvas *c_asym_bcm0l02 = new TCanvas("asym_bcm0l02");
  TCanvas* c_asym_bcm_dg_us = new TCanvas("asym_bcm_dg_us");
  TCanvas *c_asym_bcm_dg_ds = new TCanvas("asym_bcm_dg_ds");
  TCanvas *canvases[5] = {c_asym_bcm_an_us, c_asym_bcm_an_ds, c_asym_bcm0l02, c_asym_bcm_dg_us, c_asym_bcm_dg_ds};
  TString asym_bcms[5] = {"asym_bcm_an_us", "asym_bcm_an_ds", "asym_bcm0l02", "asym_bcm_dg_us", "asym_bcm_dg_ds"};

  //open files, prepare to load mean and mean error
  meanfile.open (Form("mean%d.txt", run));
  float mean;
  float error;

  //start making the plots: asym versus CodaEventNumber
  for (int c=0; c<5; c++){ //5
    canvases[c]->Divide(2, 2);
    for (int i=0; i<4; i++) { //4
      canvases[c]->cd(i+1);
      tree_mul->Draw(Form("%s:CodaEventNumber", asym_bcms[c].Data()), cuts[i].Data());
     
      TH1F *htemp = new TH1F("htemp", "htemp", 100, -0.002, 0.002);
      tree_mul->Draw(Form("%s>>htemp", asym_bcms[c].Data()), cuts[i].Data(), "goff");
      mean = htemp->GetMean();
      error = htemp->GetMeanError();
      
      meanfile << asym_bcms[c] << " " << i << " "<< mean << " " << error <<"\n";
    }
  }
  cout << "done" << endl;
 
  //save canvases 
  
  c_asym_bcm_an_us->SaveAs(Form("asym_an_us_vtime%d.pdf", run));
  c_asym_bcm_an_ds->SaveAs(Form("asym_an_ds_vtime%d.pdf", run));
  c_asym_bcm0l02->SaveAs(Form("asym_0l02_vtime%d.pdf", run));
  c_asym_bcm_dg_us->SaveAs(Form("asym_dg_us_vtime%d.pdf", run));
  c_asym_bcm_dg_ds->SaveAs(Form("asym_dg_ds_vtime%d.pdf", run));
  


 
  
  



}

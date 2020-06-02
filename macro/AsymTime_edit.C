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
void AsymTime_edit(int runN=3543, TString ucut ="1") { 
  //To use this macro do .x AsymTime_edit.C(3543,"CodaEventNumber>10e3") and apply whatever run# and cut you want
  //PLOT ASYM VERSUS TIME FOR DOUBLE PEAK RUNS

  ucut = Form("%s&&ErrorFlag==0",ucut.Data());

 TFile *file1=new TFile(Form("/adaq1/work1/apar/japanOutput/prexPrompt_pass2_%d.000.root",runN));
 const int Nvar =4;
 TString vars[Nvar]={"asym_bcm_an_us","asym_bcm_an_ds","asym_bcm0l02","asym_bpm2i01WS"};
  
  //make the trees, begin to extract data
  
  TTree *tree_mul0 = (TTree*) file1->Get("mul");
   
  //make time plots
  TCanvas* c1 = new TCanvas("c1","c1",0,0,1200,700);
  c1->Divide(Nvar/2, 2);
  for(int i=0;  i<Nvar;i++){
    c1->cd(i+1);
    tree_mul0  ->Draw(Form("%s:CodaEventNumber", vars[i].Data()),ucut.Data());
  }
 
  //save canvases 
  /*
  c_asym_bcm_an_us->SaveAs("asym_an_us_vtime.pdf");
  c_asym_bcm_an_ds->SaveAs("asym_an_ds_vtime.pdf");
  c_asym_bcm0l02->SaveAs("asym_0l02_vtime.pdf");
  c_asym_bcm_dg_us->SaveAs("asym_dg_us_vtime.pdf");
  c_asym_bcm_dg_ds->SaveAs("asym_dg_ds_vtime.pdf");
  */


  
  
  



}

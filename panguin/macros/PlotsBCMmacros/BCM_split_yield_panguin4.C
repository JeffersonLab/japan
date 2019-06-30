
// Macro for SAM plots
//Auther: Devi Adhikari
//I have commented out the pdf outputs.
//You can uncomment if you want to produce plots as pdf file.
//Or you can also produce the plots into any other file format.
//To run this macro just type root-l SAM_pdf.C in your terminal.
//Then it will ask you to enter run number(only number).
//Type in run number you want to see and hit enter.
//This macro stands on its own.
//In order to configure to panguin some modifications are needed.
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <fstream>
using namespace std;

void BCM_split_yield_panguin4(){
  gStyle->SetOptStat(0);
  //gStyle->SetTitleYOffset(1.5);
  int run1=0;
  char buf[300];
  const int num=8;
  TString yieldb[9]={"","yield_bcm_an_ds.hw_sum","yield_bcm_an_ds3.hw_sum","yield_bcm_an_ds10.hw_sum","yield_bcm_an_us.hw_sum","yield_ch_battery_1.hw_sum*76.263e-6","yield_ch_battery_2.hw_sum*76.263e-6","yield_qwk_3_0.hw_sum*76.263e-6","yield_bcm_an_us.hw_sum*76.263e-6-yield_bcm_an_ds3.hw_sum*76.263e-6"};


  TTree* t = (TTree*)gDirectory->Get("mul");

  TH2D *hd[num*2+1];
  TPad *c4 = new TPad("BCM_Pedestal_Plots_yield_vs_time","BCM_Pedestal_Plots_yield_vs_time",0,0,1,1);
  c4->Divide(2,4);
  c4->Draw();
  for(int i=1;i<=num;i++){
    c4->cd(i);
    t->Draw(Form("%s:pattern_number>>hh%d",yieldb[i].Data(),i),"Entry$>3");
    hd[i] = (TH2D*)gDirectory->FindObject(Form("hh%d",i));
    hd[i]->SetDirectory(gROOT);
    hd[i]->Draw();
  
  }

  for(int i=1;i<=num;i++){
    c4->cd(i);
    TString title=Form("%s vs time",yieldb[i].Data());
    hd[i]->SetTitle(title);
    hd[i]->SetXTitle(Form("%s:pattern_number",yieldb[i].Data()));
    Printf("%s",Form("%f %f\n",hd[i]->GetMean(), hd[i]->GetRMS()));
    c4->cd(i);
    hd[i]->Draw();
  }
 // c4->SaveAs(Form("Plots/yield_vs_time_run%d.png",run1));
  //outfile.close()
}

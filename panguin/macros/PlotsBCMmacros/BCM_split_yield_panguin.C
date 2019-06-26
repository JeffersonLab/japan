
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

void BCM_split_yield_panguin(){
  gStyle->SetOptStat(2211);
  //gStyle->SetTitleYOffset(1.5);BCM_split_yield.C
  int run1=0;
  char buf[300];
  const int num=8;
  TString diffbnorm[9]={"","diff_bcm_an_ds.hw_sum/70","diff_bcm_an_ds3.hw_sum/70","diff_bcm_an_ds10.hw_sum/70","diff_bcm_an_us.hw_sum/70","diff_ch_battery_1.hw_sum*76.263e-6/5","diff_ch_battery_2.hw_sum*76.263e-6/5","diff_qwk_3_0.hw_sum*76.263e-6/5","(diff_bcm_an_us.hw_sum-diff_bcm_an_ds3.hw_sum)/70"};
  //TString diffb[9]={"","diff_bcm_an_ds","diff_bcm_an_ds3","diff_bcm_an_ds10","diff_bcm_an_us","diff_ch_battery_1","diff_ch_battery_2","diff_qwk_3_0","diff_bcm_an_us-diff_bcm_an_ds3"};

  TTree* t = (TTree*)gDirectory->Get("mul");
  //t->Add(Form("$QW_ROOTFILES/prexALL_%d.0*.root",run1));

  TH1D *hd[num*2+1];
  TPaveStats *st;
  TPad *c1 = new TPad("BCM_Pedestal_Plots","BCM_Pedestal_Plots",0,0,1,1);
  c1->Divide(2,4);
  c1->Draw();
  for(int i=1;i<=num;i++){
    c1->cd(i);
    t->Draw(Form("1e6*%s>>h%d",diffbnorm[i].Data(),i),"Entry$>3"); 
    hd[i] = (TH1D*)gDirectory->FindObject(Form("h%d",i));
    hd[i]->SetDirectory(gROOT);
    hd[i]->Draw();
    gPad->Update();
    st = (TPaveStats*)hd[i]->FindObject("stats");
    TGaxis::SetMaxDigits(3);
    st->SetX1NDC(0.7);
    st->SetX2NDC(0.9);
    st->SetY1NDC(0.9);
    st->SetY2NDC(0.7);
  }

  for(int i=1;i<=num;i++){
    c1->cd(i);
    TString title=Form("%s",diffbnorm[i].Data());
    hd[i]->SetTitle(title);
    hd[i]->SetXTitle(Form("%s(ppm)",diffbnorm[i].Data()));
    Printf("%s",Form("%f %f\n",hd[i]->GetMean(), hd[i]->GetRMS()));
    c1->cd(i);
    hd[i]->Draw();
  }

}

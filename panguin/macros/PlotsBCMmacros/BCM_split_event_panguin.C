
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

void BCM_split_event_panguin(){
  gStyle->SetTitleYOffset(1.5);
 // gStyle->SetOptStat(112211);
  gStyle->SetOptStat(2211);
  TGaxis::SetMaxDigits(3);
  gStyle->SetStatX(0.9);
  gStyle->SetStatY(0.9);
  gStyle->SetStatW(0.32);
  gStyle->SetStatH(0.18);
  int run1=0;
  char buf[300];
  const int num=8;
  TH1D *hp[num+1];
  TH2D *het[num+1];
  TString evtb[9]={"","bcm_an_ds.hw_sum","bcm_an_ds3.hw_sum","bcm_an_ds10.hw_sum","bcm_an_us.hw_sum","ch_battery_1*76.263e-6","ch_battery_2*76.263e-6","qwk_3_0*76.263e-6","bcm_an_us.hw_sum-bcm_an_ds3.hw_sum"}; 
// TString evtb[9]={"","bcm_an_ds.hw_sum/70","bcm_an_ds3.hw_sum/70","bcm_an_ds10.hw_sum/70","bcm_an_us.hw_sum/70","ch_battery_1*76.263e-6/5","ch_battery_2*76.263e-6/5","qwk_3_0*76.263e-6/5","bcm_an_us.hw_sum/70-bcm_an_ds3.hw_sum/70"};
 // TString evtb[9]={"","diff_bcm_an_ds.hw_sum_raw*76.263e-6","diff_bcm_an_ds3.hw_sum_raw*76.263e-6","diff_bcm_an_ds10.hw_sum_raw*76.263e-6","diff_bcm_an_us.hw_sum_raw*76.263e-6","diff_ch_battery_1.hw_sum_raw*76.263e-6","diff_ch_battery_2.hw_sum_raw*76.263e-6","diff_qwk_3_0.hw_sum_raw*76.263e-6","diff_bcm_an_us-diff_bcm_an_ds3.hw_sum_raw*76.263e-6"};
 


    TTree* t = (TTree*)gDirectory->Get("evt");
  //t->Add(Form("$QW_ROOTFILES/prexALL_%d.0*.root",run1));

  TH1D *hd[num+1];
  TPaveStats *st;
  TPad *c6 = new TPad("BCM_events_plots","BCM_events_plots",0,0,1,1);
  c6->Divide(2,4);
  c6->Draw();
  for(int i=1;i<=num;i++){
    c6->cd(i);
  //  t->Draw(Form("1e6*%s/5>>h%d",diffbnorm[i].Data(),i),"Entry$>3"); 
    t->Draw(Form("%s>>h%d",evtb[i].Data(),i),"Entry$>3");
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
    c6->cd(i);
    TString title=Form("%s evt",evtb[i].Data());
    hd[i]->SetTitle(title);
    hd[i]->SetXTitle(Form("%s",evtb[i].Data()));
    Printf("%s",Form("%f %f\n",hd[i]->GetMean(), hd[i]->GetRMS()));
    c6->cd(i);
    hd[i]->Draw();
  }

}




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

void BCM_split_yield_panguin2(){
  gStyle->SetOptStat(2211);
  //gStyle->SetTitleYOffset(1.5);
  int run1=0;
  char buf[300];
  const int num=8;
 
 // TString diffbnorm[9]={"","diff_bcm_an_ds.hw_sum/70","diff_bcm_an_ds3.hw_sum/70","diff_bcm_an_ds10.hw_sum/70","diff_bcm_an_us.hw_sum/70","diff_ch_battery_1.hw_sum*76.263e-6/5","diff_ch_battery_2.hw_sum*76.263e-6/5","diff_qwk_3_0.hw_sum*76.263e-6/5","(diff_bcm_an_us.hw_sum-diff_bcm_an_ds3.hw_sum)/70"};
  TString diffbnorm[9]={"","diff_bcm_an_ds.hw_sum","diff_bcm_an_ds3.hw_sum","diff_bcm_an_ds10.hw_sum","diff_bcm_an_us.hw_sum","diff_ch_battery_1.hw_sum*76.263e-6","diff_ch_battery_2.hw_sum*76.263e-6","diff_qwk_3_0.hw_sum*76.263e-6","diff_bcm_an_us.hw_sum-diff_bcm_an_ds3.hw_sum"};
  
  TTree* t = (TTree*)gDirectory->Get("mul");
  //t->Add(Form("$QW_ROOTFILES/prexALL_%d.0*.root",run1));

  TH1D *hd[num*2+1];
  TPaveStats *st;
  TPad *c2 = new TPad("BCM_Pedestal_Plots_uV","BCM_Pedestal_Plots_uV",0,0,1,1);
  c2->Divide(2,4);
  c2->Draw();
  for(int i=1;i<=num;i++){
    c2->cd(i);
    t->Draw(Form("1e6*%s>>h%d",diffbnorm[i].Data(),i+8),"Entry$>3"); 
    hd[i+8] = (TH1D*)gDirectory->FindObject(Form("h%d",i+8));
    hd[i+8]->SetDirectory(gROOT);
    hd[i+8]->Draw();
    gPad->Update();
    st = (TPaveStats*)hd[i+8]->FindObject("stats");
    TGaxis::SetMaxDigits(3);
    st->SetX1NDC(0.7);
    st->SetX2NDC(0.9);
    st->SetY1NDC(0.9);
    st->SetY2NDC(0.7);
  }

  for(int i=1;i<=num;i++){
    c2->cd(i);
    TString title=Form("%s",diffbnorm[i].Data());
    hd[i+8]->SetTitle(title);
    if (i== 4 || i == 5 || i ==6 || i ==7){
      hd[i+8]->SetXTitle(Form("%s(uV)",diffbnorm[i].Data()));
    }
    else {
      hd[i+8]->SetXTitle(Form("%s(uA)",diffbnorm[i].Data()));
    }
    Printf("%s",Form("%f %f\n",hd[i+8]->GetMean(), hd[i+8]->GetRMS()));
    //outfile << hd[i]->GetMean() <<"	"<< hd[i]->GetRMS() << endl;
    c2->cd(i);
    hd[i+8]->Draw();
  }

}

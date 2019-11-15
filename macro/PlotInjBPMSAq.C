// Injector BPMs Plot Macro 
// Source from Caryn Palatchi
// Adapted by Tao Ye for JAPAN 
// Last Update : 05-2019

#include "device_list_Caryn.h"

//FIXME : IHWPstatus should be able to obtain from slow tree. -TY

void PlotInjBPMSAq(vector<const char*> &vBPM , 
		 Int_t IHWPstatus, TString ucut);  // Generic function
void PlotInjBPMSAq(Int_t IHWPstatus, TString ucut); // interface to panguin
void PlotInjBPMSAq();// interface to summary plots

void PlotInjBPMSAq(Int_t docaryn=1, Int_t run_number=0, Int_t IHWPstatus=0, TString ucut="1"){
  TString rf_name =Form("$QW_ROOTFILES/prexPrompt_pass2_%d.000.root",run_number);
  TFile *rootfile = TFile::Open(rf_name);
  TString outputDir = "/adaqfs/home/apar/PREX/japan/plots/PosDiffoutputs";
  TCanvas *c4 = new TCanvas("c4","c4",2400,4800);
  c4->cd();
  PlotInjBPMSAq(IHWPstatus,ucut);
  plot_title = Form("%s/run%d_injector_BPM_Aq.png",outputDir.Data(),run_number);
  c4->SaveAs(output_path+plot_title);
}

void PlotInjBPMSAq( Int_t IHWPstatus, TString ucut){
  PlotInjBPMSAq(vInjBPM,IHWPstatus,ucut);
} 

void PlotInjBPMSAq(vector<const char*> &vBPM , 
		 Int_t IHWPstatus, TString ucut){

  //collect data points from first run file
  TTree* evt_tree = (TTree*)gROOT->FindObject("evt");
  TTree* pair_tree = (TTree*)gROOT->FindObject("pr");
  TTree* mul_tree = (TTree*)gROOT->FindObject("mul");

  Int_t ndeti = vBPM.size();
  Double_t* Aq= new Double_t[ndeti];
  Double_t* eAq= new Double_t[ndeti];
  Double_t* rmsAq= new Double_t[ndeti];
  Double_t* zero= new Double_t[ndeti];

  TPad *pad1= new TPad("pad1","pad1",0,0,1,1);
  pad1->Divide(1,2);
  pad1->Draw();
  TH1D* thishist;
  TString objnam;
  for(Int_t dd=0;dd<ndeti;dd++) {
    mul_tree->Draw(Form("1e6*asym_%sWS>>hista%d",vBPM[dd],dd),ucut,"goff");
    objnam = Form("hista%d",dd);
    thishist = (TH1D*)gDirectory->FindObject(objnam);
    Aq[dd] = thishist->GetMean();
    eAq[dd]= thishist->GetMeanError();
    rmsAq[dd]= thishist->GetRMS();
    zero[dd]=0;
    cout<<vBPM[dd]<<" Aq[ppm](RMS)+-error: "<<Aq[dd]<<" "<<rmsAq[dd]<<" "<<eAq[dd]<<endl;

  }

  const Int_t numchains=ndeti;
  Double_t chain[numchains];
  Double_t chainerr[numchains];
  for (Int_t i=0;i<numchains;i++){
    chain[i]=i+1;
    chainerr[i]=0.0;
  };

  TGraphErrors *Aqgraph;
  TGraphErrors *Aqrmsgraph;

  Aqgraph = new TGraphErrors(numchains,chain,&(Aq[0]),chainerr,&(eAq[0]));
  Aqrmsgraph = new TGraphErrors(numchains,chain,&(rmsAq[0]),chainerr,&(zero[0]));
  
  TMultiGraph *xygraph = new TMultiGraph();
  TMultiGraph *xyrmsgraph = new TMultiGraph();

  // TAxis *ax = Dygraph->GetHistogram()->GetXaxis();
  // Double_t x1 = ax->GetBinLowEdge(1);
  // Double_t x2 = ax->GetBinUpEdge(ax->GetNbins());
  Double_t x1 = chain[0]-0.5;
  Double_t x2 = chain[numchains-1]+0.5;

  TVirtualPad* pad_buff;


  pad1->cd(1);
  //  ax = Aqgraph->GetHistogram()->GetXaxis();
  //   x1 = ax->GetBinLowEdge(1);
  //   x2 = ax->GetBinUpEdge(ax->GetNbins());
  Aqgraph->GetHistogram()->GetXaxis()->Set(numchains,x1,x2);
  for(Int_t k=0;k<numchains;k++){
    //      l2timevschain->GetHistogram()->GetXaxis()->SetBinLabel(k+1,names[k].c_str());
    Aqgraph->GetHistogram()->GetXaxis()->SetBinLabel(k+1,vBPM[k]);
  }
  Aqgraph->GetHistogram()->GetXaxis()->SetLabelSize(0.07);

  Aqgraph->SetMarkerStyle(21);
  Aqgraph->SetMarkerColor(1);
  Aqgraph->SetLineColor(1);
  Aqgraph->Draw("AP");
  Aqgraph->SetTitle(Form("IHWP=%d,  %s ",IHWPstatus,ucut.Data()));
  Aqgraph->GetYaxis()->SetTitle("Aq ppm");
  pad1->SetGrid();

  pad1->cd(2);
  Aqrmsgraph->GetHistogram()->GetXaxis()->Set(numchains,x1,x2);
  for(Int_t k=0;k<numchains;k++){
    Aqrmsgraph->GetHistogram()->GetXaxis()->SetBinLabel(k+1,vBPM[k]);
  }
  Aqrmsgraph->GetHistogram()->GetXaxis()->SetLabelSize(0.07);

  Aqrmsgraph->SetMarkerStyle(21);
  Aqrmsgraph->SetMarkerColor(1);
  Aqrmsgraph->SetLineColor(1);
  Aqrmsgraph->Draw("AP");
  Aqrmsgraph->SetTitle(Form("IHWP=%d, %s ",IHWPstatus, ucut.Data()));
  Aqrmsgraph->GetYaxis()->SetTitle("RMS ppm");
  pad1->SetGrid();
}


#include "TString.h"
#include "TTree.h"
#include "TChain.h"



TGraphErrors gDraw(TTree *T, TString var, TString plot, Bool_t  good)
{
TString ok="";
Int_t nEntries=0;
if (good){
  ok += Form("%s_%s!=0 || %s_%s_error!=0", var.Data(), plot.Data(), var.Data(), plot.Data());
} else {
  ok += Form("%s_%s==0 && %s_%s_error==0", var.Data(), plot.Data(), var.Data(), plot.Data());
}

TString unit="1";
if (var.Contains("asym")){
	if (plot=="mean") unit="ppb";
        if (plot=="rms") unit="ppm";
}

nEntries=T->Draw(Form("run_number:%s_%s/%s:%s_%s_error/%s", var.Data(), plot.Data(), unit.Data(), var.Data(),plot.Data(), unit.Data()),ok.Data(), "goff");

Double_t run[nEntries];
for(int i=0; i<nEntries;i++){
  run[i]=i;
}

gStyle->SetOptFit(1);
TGraphErrors g(nEntries,run, T->GetV2(), 0, T->GetV3());
TAxis* a = g.GetXaxis();
a->SetNdivisions(-nEntries);
double* d=T->GetV1();
Int_t binindex= 0;
for(int i=0; i<nEntries;i++){
   binindex=a->FindBin(i);
   a->SetBinLabel(binindex, Form("%3.0f",d[i]));
}
Double_t par;
g.Fit("pol0");
g.SetTitle(Form("%s_%s/%s vs run", var.Data(), plot.Data(), unit.Data()));
g.SetMarkerStyle(20);
//TF1 *fit=g.GetFunction("pol0");
//printf("%3.1f",fit->GetParameter(0));
return g;
} 


void twoDagg(TString var, TString agg){


TChain *T=new TChain("agg");
T->Add(agg.Data());
TCanvas *c=new TCanvas("c", "c", 1200, 900);
c->Divide(2,2);

c->cd(1);
gPad->SetGrid();
TGraphErrors gmean= gDraw(T,var, "mean", true);
gmean.Draw("ap");
c->cd(2);
gPad->SetGrid();
TGraphErrors gmeanEC= gDraw(T,var, "mean", false);
gmeanEC.Draw("ap");
gmeanEC.SetMarkerColor(kRed);
gmeanEC.SetLineColor(kRed);
gmeanEC.SetMarkerStyle(47);
c->cd(3);
gPad->SetGrid();
TGraphErrors grms=gDraw(T,var, "rms", true);
grms.Draw("ap");
c->cd(4);
gPad->SetGrid();
TGraphErrors grmsEC=gDraw(T,var,"rms", false);
grmsEC.Draw("ap");
grmsEC.SetMarkerColor(kRed);
grmsEC.SetLineColor(kRed);
grmsEC.SetMarkerStyle(47);
c->Print(Form("%s.png",var.Data()));
return 0;
}



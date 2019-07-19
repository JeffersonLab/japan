#include "TString.h"
#include "TTree.h"
#include "TChain.h"


TGraphErrors gDraw(TTree *T, TString var, TString plot, Bool_t  good)
{
TString ok="";
Int_t nEntries=0;
if (good){
  ok += Form("%s_%s!=0 || %s_%s_error!=0",plot.Data(), var.Data(),plot.Data(), var.Data());
} else {
  ok += Form("%s_%s==0 && %s_%s_error==0", plot.Data(), var.Data(), plot.Data(), var.Data());
}

nEntries=T->Draw(Form("run_number:%s_%s:%s_%s_error", plot.Data(),var.Data(),plot.Data(),var.Data()),ok.Data(), "goff");
TGraphErrors g(nEntries, T->GetV1(), T->GetV2(), 0, T->GetV3());
g.SetTitle(Form("%s_%s vs run", plot.Data(), var.Data()));
g.SetMarkerStyle(20);
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



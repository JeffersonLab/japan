//////////////////////////////////////////////////////////////////////
//
// showIntegrated.C
//   Bryan Moffit - July 2005
//  
//  helper routine for PANGUIN.  Plots the integrated
//   convergence of a given quantity
//edit by Caryn Palatchi 3/8/2019 for japan

void showIntegrated(TString treename, TString given_quantity, 
		    TCut given_cut, UInt_t nbins=20) {

  gROOT->LoadMacro("~/PREX/japan/macro/Integrated.C");
  TTree *T = (TTree*)gROOT->FindObject(treename);
  TGraphErrors *gr = Integrated(T, given_quantity, given_cut, nbins);
  gr->SetMarkerStyle(20);
  gPad->SetGridy(1);
  gr->Draw("AP");

}

#include <sstream>
#include <string>
#include <vector>
#include "TTree.h"
#include "japantypes.hh"

void drawEVT(std::string title, std::string draws, std::string cuts,int binsx, double binlowx, double binhighx, int binsy, double binlowy, double binhighy){
  gStyle->SetOptStat(0);
  TTree *evtTree = (TTree*)gDirectory->Get("evt");
  // Make an instance of the relevant data type's struct
//  EVTTREE evtTreeVal;
//  evtTree->SetBranchAddress( Form("%s",draws.c_str()), &evtTreeVal );
  TH2F *h1=new TH2F("h1",title.c_str(),binsx,binlowx,binhighx,binsy,binlowy,binhighy);

//  Int_t numEntries = evtTree->GetEntries();
//
//  for (int j = 0; j < numEntries; j++) {
//    // Loop over the input file's entries and fill the new stuff with the results of one of the leaves
//    evtTree->GetEntry(j);
//    variable = evtTreeVal.hw_sum;
//  }
  // Draw your results
  h1->SetMarkerStyle(20);
  h1->SetMarkerSize(0.4);
  evtTree->Draw(Form("%s>>h1",draws.c_str()),Form("%s",cuts.c_str()));
  h1->Draw();
}

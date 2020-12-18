#include <iostream>
#include "TMath.h"
#include "TMatrixD.h"
#include "TMatrixDLazy.h"
#include "TVectorD.h"
#include "TDecompLU.h"
#include "TDecompSVD.h"
#include "TArrayD.h"
#include "TMatrixD.h"
#include "TMatrixDBase.h"
#include <TRandom.h>
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
#include <TChain.h>
#include <TString.h>
#include <TProfile.h>
#include <sstream>
#include <iostream>
#include <TGraphErrors.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <TEntryList.h>

// A ROOT macro to grab the "segment" leaf, if it exists, and to write it with the value in place, or the one desired by the user for cycles > user cycle input number
void AddSegVar(TString oldFileName = "test.root")//, Double_t cyclenumber = -1, Int_t userSegmentNum = 2)
{
  // Get old file, old tree and set top branch address
  //if(cyclenumber==-1){
    //cout << "No cyclenumber passed with macro call!" << endl;
    //return;
  //}
  //cout << "Updating segment number to " << userSegmentNum << " for cycles at and beyond cyclenumber " << cyclenumber << endl;
  Bool_t newFile = gSystem->AccessPathName(oldFileName);
  // Opposite return convention from the usual
  if (!newFile){

    TFile* oldfile = TFile::Open(oldFileName);
    TTree *oldtree;
    oldfile->GetObject("dit", oldtree);
    Int_t nentries = oldtree->GetEntries();
    if (oldtree->GetBranch("segment") && nentries > 0) {
      std::cout << "Segment branch exists already, quitting" << std::endl;
      return;
    }
    else {

    TFile newfile(Form("localTmp_AAA.root"), "recreate");
    auto newtree = oldtree->CloneTree(0);

    Int_t segment = 1;
    TLeaf* segmentL;
    // If segment exists address it, else create new
    if (oldtree->GetBranch("segment")) {
      newtree->SetBranchAddress("segment",&segment);
      segmentL = oldtree->GetLeaf("segment");
    }
    else {
      newtree->Branch("segment",&segment);
      segmentL = newtree->GetLeaf("segment");
    }

    TLeaf* cycNumL = oldtree->GetLeaf("cyclenum");
    Double_t tmpCycNum = 0.0;
    Int_t tmpSegmentNum = 1;
    for (auto i : ROOT::TSeqI(nentries)) {
      cycNumL->GetBranch()->GetEntry(i);
      tmpCycNum = cycNumL->GetValue();
      if (oldtree->GetBranch("segment")) {
        // Do nothing, rewrite existing value
        segmentL->GetBranch()->GetEntry(i);
        tmpSegmentNum = segmentL->GetValue();
      }
      else {
        // Just be default 1, add new branch in
        tmpSegmentNum = 1;
      }
      //if (abs(tmpSegmentNum) > 1000) {
        //tmpSegmentNum = 1;
      //}
      segment = tmpSegmentNum;
      //if (tmpCycNum>=cyclenumber) {
        //segment = userSegmentNum;
      //}
      oldtree->GetEntry(i);
      newtree->Fill();
    }
    newtree->Write("dit",TObject::kOverwrite);
    newfile.Close();
    oldfile->Close();
    delete oldfile;
    gSystem->Exec(Form("mv localTmp_AAA.root %s",oldFileName.Data()));
    }
  }
}

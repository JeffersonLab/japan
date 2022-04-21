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
void SegmentTree_runnum_eig(TString oldFileName = "test.root", Double_t runnumber = -1, Int_t userSegmentNum = 2, Double_t finalrunnumber = 999999)
{
  // Get old file, old tree and set top branch address
  if(runnumber==-1){
    cout << "No runnumber passed with macro call!" << endl;
    return;
  }
  cout << "Updating segment number to " << userSegmentNum << " for cycles at and beyond runnumber " << runnumber << endl;
  Bool_t newFile = gSystem->AccessPathName(oldFileName);
  // Opposite return convention from the usual
  if (!newFile){

    TFile* oldFile = TFile::Open(oldFileName);
    TTree *oldtree;
    oldFile->GetObject("mini", oldtree);
    Int_t nentries = oldtree->GetEntries();

    TFile newFile(Form("localTmp_%d.root",(Int_t)runnumber), "recreate");
    auto newtree = oldtree->CloneTree(0);

    Int_t segment = 1;
    TLeaf* segmentL;
    if (oldtree->GetBranch("dit_segment")) {
      newtree->SetBranchAddress("dit_segment",&segment);
      segmentL = oldtree->GetLeaf("dit_segment");
    }
    else {
      newtree->Branch("dit_segment",&segment);
      segmentL = newtree->GetLeaf("dit_segment");
    }

    TLeaf* runNumL = oldtree->GetLeaf("run");
    Double_t tmprunNum = 0.0;
    Int_t tmpSegmentNum = 1;
    for (auto i : ROOT::TSeqI(nentries)) {
      runNumL->GetBranch()->GetEntry(i);
      tmprunNum = runNumL->GetValue();
      if (oldtree->GetBranch("dit_segment")) {
        segmentL->GetBranch()->GetEntry(i);
        tmpSegmentNum = segmentL->GetValue();
      }
      else {
        tmpSegmentNum = 1;
      }
      // I made some glitch with newtree vs. old tree
      if (abs(tmpSegmentNum) > 1000) {
        tmpSegmentNum = 1;
      }
      segment = tmpSegmentNum;
      if (tmprunNum>=runnumber && tmprunNum<=finalrunnumber) {
        segment = userSegmentNum;
      }
      oldtree->GetEntry(i);
      newtree->Fill();
    }
    newtree->Write("mini",TObject::kOverwrite);
    TKey *key;
    TIter nextkey(oldFile->GetListOfKeys(),kIterBackward);                           
    while ((key = (TKey*)nextkey())) {                                             
      const char *classname = key->GetClassName();                                 
      TClass *cl = gROOT->GetClass(classname);                                     
      if (!cl) continue;
      if (cl->InheritsFrom(TTree::Class())) {                                      
        TTree *T = (TTree*)oldFile->Get(key->GetName());                             
        // Avoid writing the data of a TTree more than once.                       
        // Note this assume that older cycles are (as expected) older              
        // snapshots of the TTree meta data.
        if (!newFile.FindObject(key->GetName())) {
          newFile.cd();
          if ((TString)key->GetName() == "mini") {
            continue;
          } // Skip re-writing mini
          TTree *newT = T->CloneTree(-1,"fast");
          newT->Write();
        }
      }
    }

    newFile.Close();
    oldFile->Close();
    delete oldFile;
    gSystem->Exec(Form("mv localTmp_%d.root %s",(Int_t)runnumber,oldFileName.Data()));
  }
}

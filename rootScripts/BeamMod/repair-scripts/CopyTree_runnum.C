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

void CopyTree_runnum(TString oldFileName = "test.root", Double_t runnum = -1) //Double_t cyclenumber = -1)
{
  // Get old file, old tree and set top branch address
  if(runnum==-1){
    cout << "No runnum passed with macro call!" << endl;
    return;
  }
  Int_t oldflag = -1;
  Int_t oldalldetsflag = -1;
  Int_t newflag = -1;
  Int_t newalldetsflag = -1;
  cout << "Clearing prior instances of runnum " << runnum << endl;
  Bool_t newFile = gSystem->AccessPathName(oldFileName);
  if (!newFile){
    TFile* oldfile = TFile::Open(oldFileName);
    TTree *oldtree;
    oldfile->GetObject("dit", oldtree);
    Int_t nentries = oldtree->GetEntries();

    TFile newfile(Form("localTmp_%d.root",(Int_t)runnum), "recreate");
    auto newtree = oldtree->CloneTree(0);
    TLeaf* cycNumL = oldtree->GetLeaf("run");
    Double_t tmpCycNum = 0.0;
    oldtree->SetBranchAddress("flag",&oldflag);
    oldtree->SetBranchAddress("alldetsflag",&oldalldetsflag);
    newtree->SetBranchAddress("flag",&newflag);
    newtree->SetBranchAddress("alldetsflag",&newalldetsflag);
    for (auto i : ROOT::TSeqI(nentries)) {
      cycNumL->GetBranch()->GetEntry(i);
      tmpCycNum = cycNumL->GetValue();
      // old option was to remove the cycle by continuing BEFORE GetEntry and fill. Now do Get entry first and set flag = 0 and then fill: continue;
      oldtree->GetEntry(i);
      if (tmpCycNum==runnum) 
      {
        Printf("Replacing old flag value of %d with new flag 0",oldflag);
        newflag=0;
        newalldetsflag=0;
      }
      else {
        newflag=oldflag;
        newalldetsflag=oldalldetsflag;
      }
      newtree->Fill();
    }
    newtree->Write("dit",TObject::kOverwrite);
    newfile.Close();
    oldfile->Close();
    delete oldfile;
    gSystem->Exec(Form("mv localTmp_%d.root %s",(Int_t)runnum,oldFileName.Data()));
  }
}

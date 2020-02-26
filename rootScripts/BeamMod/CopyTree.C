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

void CopyTree(TString oldFileName = "test.root", Double_t cyclenumber = -1)
{
  // Get old file, old tree and set top branch address
  if(cyclenumber==-1){
    cout << "No cyclenumber passed with macro call!" << endl;
    return;
  }
  cout << "Clearing prior instances of cyclenumber " << cyclenumber << endl;
  Bool_t newFile = gSystem->AccessPathName(oldFileName);
  if (!newFile){
    TFile* oldfile = TFile::Open(oldFileName);
    TTree *oldtree;
    oldfile->GetObject("dit", oldtree);
    Int_t nentries = oldtree->GetEntries();

    TFile newfile(Form("localTmp_%d.root",(Int_t)cyclenumber), "recreate");
    auto newtree = oldtree->CloneTree(0);
    TLeaf* cycNumL = oldtree->GetLeaf("cyclenum");
    Double_t tmpCycNum = 0.0;
    for (auto i : ROOT::TSeqI(nentries)) {
      cycNumL->GetBranch()->GetEntry(i);
      tmpCycNum = cycNumL->GetValue();
      if (tmpCycNum==cyclenumber) continue;
      oldtree->GetEntry(i);
      newtree->Fill();
    }
    newtree->Write("dit",TObject::kOverwrite);
    newfile.Close();
    oldfile->Close();
    delete oldfile;
    gSystem->Exec(Form("mv localTmp_%d.root %s",(Int_t)cyclenumber,oldFileName.Data()));
  }
}

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

//using namespace std; 

class BMOD{
  public:
    TString slopeFilename = "test.root";
    Int_t nBPM;
    Int_t nDet;
    std::map<std::string,std::vector<std::string>> parameterVectors;
    void parseTextFile(std::string);
    void edittree(TString);
    ~BMOD();
    Int_t getData(Int_t);
};

BMOD::~BMOD(){
};

void BMOD::parseTextFile(std::string fileName = "input.txt"){
  Printf("Reading input config file");
  std::string line;
  std::vector<std::string> tokens;
  std::string token1;
  std::string token2;
  std::vector<std::string> details;
  ifstream ifinfile(fileName);
  if (ifinfile.is_open()){
    while(getline(ifinfile,line)){
//      Printf("Reading line %s",line.c_str());
      std::string token;
      std::istringstream tokenStream(line);
      Int_t tokenizer = 0;
      while(getline(tokenStream,token,':')){
//        Printf("Reading token %s",token.c_str());
        if (tokenizer == 0) {
//          Printf("Token 1 = %s",token.c_str());
          token1 = (std::string)token;
        }
        else {
//          Printf("Token 2 = %s",token.c_str());
          token2 = (std::string)token;
        }
        tokenizer++;
        tokens.push_back(token);
      }
      if (tokens.size() < 2){
        Printf("Error: invalid input file");
        continue;
      }
      if (tokens.size() == 2){
        std::string detail;
        std::istringstream detailsStream(tokens.at(1));
        while(getline(detailsStream,detail,',')){
//          Printf("Reading detail %s",detail.c_str());
          details.push_back(detail);
        }
        if (details.size() > 0) {
//          Printf("Adding details to parameter vector %s",tokens[0].c_str());
          parameterVectors.insert(std::pair<std::string,std::vector<std::string>>(tokens.at(0),details));
        }
      }
      else {
        Printf("Invalid entry in input file: %s",line.c_str());
      }
      tokens.clear();
      details.clear();
    }
    ifinfile.close();
  }
}

void BMOD::edittree(TString oldFileName = "test.root")
{
  nBPM = parameterVectors["BPMs"].size();
  nDet = parameterVectors["Detectors"].size();
  // Get old file, old tree and set top branch address
  Bool_t newFile = gSystem->AccessPathName(oldFileName);
  if (!newFile){
    TFile* oldfile = TFile::Open(oldFileName);
    TTree *oldtree;
    oldfile->GetObject("dit", oldtree);
    const auto nentries = oldtree->GetEntries();

    TFile newfile(Form("localTmp.root"), "recreate");
    auto newtree = oldtree->CloneTree(0);
    TLeaf* cycNumL = oldtree->GetLeaf("cyclenum");
    Double_t tmpCycNum = 0.0;

    Int_t flag = 1;
    newtree->SetBranchAddress("flag",&flag);

    std::vector<std::vector<std::vector<Double_t>>> tmpVecss;
    std::vector<std::vector<Double_t>> tmpVecs;
    std::vector<std::vector<Double_t>> tmpMeans;
    std::vector<std::vector<Double_t>> tmpRMSs;
    std::vector<Double_t> tmpVec;
    std::vector<Double_t> tmpVec2;
    // FIXME Technically nDet and nBPM are 0 unless you've executed "calculateSensitivities" at least once...
    for(int idet=0;idet<nDet;idet++){
      tmpVecs.clear();
      tmpVec2.clear();
      for(int ibpm=0;ibpm<nBPM;ibpm++){
        tmpVec.clear();
        for (Int_t j = 0; j<nentries ; j++){
          tmpVec.push_back(0.0);
        }
        tmpVecs.push_back(tmpVec);
        tmpVec2.push_back(0.0);
      }
      tmpVecss.push_back(tmpVecs);
      tmpMeans.push_back(tmpVec2);
      tmpRMSs.push_back(tmpVec2);
    }
    for (auto i : ROOT::TSeqI(nentries)) {
      cycNumL->GetBranch()->GetEntry(i);
      tmpCycNum = cycNumL->GetValue();
      oldtree->GetEntry(i);
      //{do stuff}
      if (parameterVectors.count("BPMs Slope Names") == 0 || parameterVectors["BPMs Slope Names"].size() == 0) {
        Printf("ERROR: No \"BPMs Slope Names\" listed for Dithering Analysis. Please add a line to input file with an entry \"BPMs Slope Names\"=comma separated list of BPM slope names\"");
        return;
      }
      if (parameterVectors.count("Detectors") == 0 || parameterVectors["Detectors"].size() == 0) {
        Printf("ERROR: No \"Detectors\" listed for Dithering Analysis. Please add a line to input file with an entry \"Detectors=comma separated list of Detector device names\"");
        return;
      }
      for(int idet=0;idet<nDet;idet++){
        for(int ibpm=0;ibpm<nBPM;ibpm++){
          TLeaf* tmpLeaf = oldtree->GetLeaf(Form("%s_%s",parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm).c_str()));
          tmpLeaf->GetBranch()->GetEntry(i);
          tmpVecss.at(idet).at(ibpm).at(i) = tmpLeaf->GetValue();
        }
      }
    }
    for(int idet=0;idet<nDet;idet++){
      for(int ibpm=0;ibpm<nBPM;ibpm++){
        tmpMeans.at(idet).at(ibpm) = TMath::Mean(tmpVecss.at(idet).at(ibpm).begin(),tmpVecss.at(idet).at(ibpm).end());
        tmpRMSs.at(idet).at(ibpm)  = TMath::RMS(tmpVecss.at(idet).at(ibpm).begin(),tmpVecss.at(idet).at(ibpm).end());
      }
    }
    for (Int_t i=0 ; i<nentries ; i++) {
      cycNumL->GetBranch()->GetEntry(i);
      tmpCycNum = cycNumL->GetValue();
      oldtree->GetEntry(i);
      // FIXME how many detectors' slopes we actually look at is an important question: ignore ATs? Only look at SAMs for CREX?
      for(int idet=0;idet<nDet;idet++){
        for(int ibpm=0;ibpm<nBPM;ibpm++){
          if ( parameterVectors.count("Flag Sigma Cut") != 0 && abs(tmpVecss.at(idet).at(ibpm).at(i)-tmpMeans.at(idet).at(ibpm)) > atof((parameterVectors["Flag Sigma Cut"].at(0).c_str()))*tmpRMSs.at(idet).at(ibpm)) {
          //  Printf("Cycle hit %f sigma cut, flag = 0",atof((parameterVectors["Flag Sigma Cut"].at(0).c_str())));
          //  Printf("Slope %f doesn't fit within %f of mean %f",tmpVecss.at(idet).at(ibpm).at(i),atof((parameterVectors["Flag Sigma Cut"].at(0).c_str()))*tmpRMSs.at(idet).at(ibpm),tmpMeans.at(idet).at(ibpm));
            flag = 0;
          }
          //else {
          //  Printf("Slope OK");
          //  Printf("Slope %f fits within %f of mean %f",tmpVecss.at(idet).at(ibpm).at(i),atof((parameterVectors["Flag Sigma Cut"].at(0).c_str()))*tmpRMSs.at(idet).at(ibpm),tmpMeans.at(idet).at(ibpm));
          //}
        }
      }
      newtree->Fill();
      flag = 1;
    }
    newtree->Write("dit",TObject::kOverwrite);
    newfile.Close();
    oldfile->Close();
    delete oldfile;
    gSystem->Exec(Form("mv localTmp.root %s",oldFileName.Data()));
  }
  else {
    Printf("Error, %s doesn't exist",oldFileName.Data());
  }
}


int bmodRMS(std::string inputFile = "input.txt", std::string sluglist = "NULL"){
  Printf("Starting BMOD Analysis");
  TStopwatch tsw;
  tsw.Start();
  BMOD * bmod = new BMOD();
  bmod->parseTextFile(inputFile);
  bmod->slopeFilename = Form("%s.root",sluglist.substr(0,sluglist.find(".")).c_str());
  bmod->edittree(bmod->slopeFilename);
  cout << "-- BMOD Analysis done in "; tsw.Print(); cout << endl;
  return 0;
}

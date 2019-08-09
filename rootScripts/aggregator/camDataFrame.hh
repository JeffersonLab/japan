#ifndef __CAMDF__
#define __CAMDF__
#include "camguin.hh"

#include <TH1.h>
#include <TROOT.h>
#include <TLeaf.h>
using namespace std;
void writeCorrectionMeanRmsDF_h(TString tree = "mul", TString draw = "asym_vqwk_04_0ch0", TString units = "", TString cut = "defaultCut", Int_t overWriteCut = 0,  Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString name = draw;
  TString mean = draw + "_correction_mean";
  TString mean_error  = draw + "_correction_mean_error";
  TString rms = draw + "_correction_rms";
  TString rms_error = draw + "_correction_rms_error";
  TString nEntries = draw + "_correction_nentries";
  if (tree == "reg"){
    draw = "reg_" + draw + "-" + draw; // Make the actual draw command now
  }
  if (tree == "dit"){
    draw = "dit_" + draw + "-" + draw; // Make the actual draw command now
  }
  Double_t data_mean = 0;
  Double_t data_mean_error = 0;
  Double_t data_rms  = 0;
  Double_t data_rms_error  = 0;
  Int_t n_entries    = 0;
  TChain * Tree = getTree_h(tree, runNumber, minirunNumber, splitNumber, nRuns);
  // Technically for correction you need a smarter way to decide both object's OK/device error/error flag cuts
  cut = getCuts_h(cut, overWriteCut, "NULL"); // FIXME in postPan era, branchToCheck in cuts method is not even used, but when it is, it will need to be parsed correctly - probably just ignore this from now on and make Device_Error_Code something done explicitly (since so many variables in Japan actually aren't devices)
  //Int_t n_entries = Tree->Draw(Form("%s%s",draw.Data(),units.Data()),Form("%s",cut.Data()));
  //TH1 * hMeanRms = (TH1*)gROOT->FindObject("htemp");
  ROOT::RDataFrame df(*Tree);
  auto cutData = df.Filter(cut.Data());
  auto hMeanRms = cutData.Define(name.Data(),draw.Data()).Histo1D(name.Data());
  if (hMeanRms==0)
  {
    Printf("Error, writeMeanRms_h: Histogram failed");
  }
  else {
    data_mean = hMeanRms->GetMean(1);
    data_mean_error = hMeanRms->GetMeanError(1);
    data_rms = hMeanRms->GetRMS(1);
    data_rms_error = hMeanRms->GetRMSError(1);
    n_entries = hMeanRms->GetEntries();
  }

  if (debug>1) Printf("Run %d mean %s: %f+-%f",runNumber,(const char*)mean,data_mean,data_mean_error);
  if (debug>1) Printf("Run %d rms %s: %f+-%f",runNumber,(const char*)rms,data_rms,data_rms_error);
  if (aggregatorStatus){
    writeFile_h(mean,data_mean,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(mean_error,data_mean_error,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms,data_rms,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms_error,data_rms_error,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(nEntries,n_entries,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)mean,data_mean);
    Printf("%s=%f",(const char*)mean_error,data_mean_error);
    Printf("%s=%f",(const char*)rms,data_rms);
    Printf("%s=%f",(const char*)rms_error,data_rms_error);
    Printf("%s=%d",(const char*)nEntries,n_entries);
  }
}

void writeCombinedMeanRmsDF_h(TString tree = "mul", TString draw = "asym_vqwk_04_0ch0", TString name = "", TString cut = "defaultCut", Int_t overWriteCut = 0,  Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber     = getRunNumber_h(runNumber);
  splitNumber   = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns         = getNruns_h(nRuns);
  TString mean  = name + "_mean";
  TString mean_error = name + "_mean_error";
  TString rms   = name + "_rms";
  TString rms_error  = name + "_rms_error";
  TString nEntries   = name + "_nentries";
  Double_t data_mean = 0;
  Double_t data_mean_error = 0;
  Double_t data_rms  = 0;
  Double_t data_rms_error  = 0;
  Int_t n_entries    = 0;
  TChain * Tree = getTree_h(tree, runNumber, minirunNumber, splitNumber, nRuns);
  TChain * FriendTree = (TChain*)Tree->GetFriend(tree);
  Printf("Name = %s",FriendTree->GetName());
  //Tree->AddFriend(FriendTree);
  cut = getCuts_h(cut, overWriteCut, "NULL"); // FIXME in postPan era, branchToCheck in cuts method is not even used, but when it is, it will need to be parsed correctly - probably just ignore this from now on and make Device_Error_Code something done explicitly (since so many variables in Japan actually aren't devices)
  //Int_t n_entries = Tree->Draw(Form("%s",draw.Data()),Form("%s",cut.Data()));
  //TH1 * hMeanRms = (TH1*)gROOT->FindObject("htemp");
  ROOT::RDataFrame df(*Tree);
  auto cutData = df.Filter(cut.Data());
  auto hMeanRms = cutData.Define(name.Data(),draw.Data()).Histo1D(name.Data());
  if (hMeanRms==0)
  {
    Printf("Error, writeCombinedMeanRms_h: Histogram failed");
  }
  else {
    data_mean = hMeanRms->GetMean(1);
    data_mean_error = hMeanRms->GetMeanError(1);
    data_rms = hMeanRms->GetRMS(1);
    data_rms_error = hMeanRms->GetRMSError(1);
    n_entries = hMeanRms->GetEntries();
  }
  if (debug>1) Printf("Run %d mean %s: %f+-%f",runNumber,(const char*)mean,data_mean,data_mean_error);
  if (debug>1) Printf("Run %d rms %s: %f+-%f",runNumber,(const char*)rms,data_rms,data_rms_error);
  if (aggregatorStatus){
    writeFile_h(mean,data_mean,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(mean_error,data_mean_error,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms,data_rms,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms_error,data_rms_error,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(nEntries,n_entries,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)mean,data_mean);
    Printf("%s=%f",(const char*)mean_error,data_mean_error);
    Printf("%s=%f",(const char*)rms,data_rms);
    Printf("%s=%f",(const char*)rms_error,data_rms_error);
    Printf("%s=%d",(const char*)nEntries,n_entries);
  }
}

#endif // __CAMDF__

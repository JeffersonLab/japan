/*****************************************************************************
File Name: Open.C

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This macro opens the root output file for the given run number
              and then friends the mulc and mulc_lrd trees to the mul tree
              for ease of creating plots.

Last Modified: June 28, 2019
*****************************************************************************/

TFile* Open(int runnum, TString filestem = "") {

  TFile* myfile;
  TTree* mymul;
  TTree* mymulc;
  TTree* mymulc_lrb;

  const char* FILE_PATH = "$QW_ROOTFILES";    //path to folder that contains rootfiles

  if(filestem != "") {
    myfile = new TFile(Form("%s/%s",FILE_PATH,filestem.Data()));
    if (!(myfile->IsOpen())) {
      cout << "File not found" << endl;
      return NULL;
    }
  }

  while(1) {
    myfile = new TFile(Form("%s/prexPrompt_pass2_%d.000.root",FILE_PATH,runnum));
    if (myfile->IsOpen()) {break;}
    myfile = new TFile(Form("%s/prexPrompt_pass1_%d.000.root",FILE_PATH,runnum));
    if (myfile->IsOpen()) {break;}
    myfile = new TFile(Form("%s/prexALL_%d.000.root",FILE_PATH,runnum));
    if (myfile->IsOpen()) {break;}
    cout << endl << "No file found for run " << runnum << " in path " << FILE_PATH << endl << endl;
    return NULL;
  }

  mymul = (TTree*)gROOT->FindObject("mul");
  mymulc = (TTree*)gROOT->FindObject("mulc");
  mymulc_lrb = (TTree*)gROOT->FindObject("mulc_lrb");

  mymul->AddFriend(mymulc);
  mymul->AddFriend(mymulc_lrb);

  return myfile;

}

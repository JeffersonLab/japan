/*****************************************************************************
File Name: Open.C

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This macro opens the root output file for the given run number
              and then friends the mulc and mulc_lrd trees to the mul tree
              for ease of creating plots.

Examples: 'Open(2964)' will open the segment zero of the default rootfile
               for run 2964 in the $QW_ROOTFILES directory.

          'Open(1234,"junk_",0)' will open the file "junk_1234.000.root"
               in the $QW_ROOTFILES directory.

          'TFile* _run1234 = Open(1234,"junk_",0)' will open the file
               "junk_1234.000.root" in the $QW_ROOTFILES directory as the
               "_run1234" pointer, so you can swtich between open files
               by doing "_run1234->cd()".

Last Modified: July 1, 2019
*****************************************************************************/

TFile* Open(int runnum, TString filestem = "", Int_t segment=0) {

  TFile* myfile;
  TTree* mymul;
  TTree* mymulc;
  TTree* mymulc_lrb;

  const char* FILE_PATH = "$QW_ROOTFILES";    //path to folder that contains rootfiles

  TString filename;

  if(filestem != "") {
    filename = Form("%s/%s%d.%03d.root",FILE_PATH,
		    filestem.Data(),runnum,segment);
    myfile = new TFile(filename);
    if (!(myfile->IsOpen())) {
      std::cout << "File, "<< filename <<", not found" << std::endl;
      return NULL;
    }
  } else {
    TString stemlist[4] = {"prexPrompt_pass2_",
			   "prexPrompt_pass1_", 
			   "prexALL_",
			   "prexinj_"};
    for  (int i=0; i<4; i++){
      filename = Form("%s/%s%d.%03d.root",FILE_PATH,
		      stemlist[i].Data(),runnum,segment);
      myfile = new TFile(filename);
      if (myfile->IsOpen()) {break;}
    }
  }
  if (myfile->IsOpen()) {
    std::cout << "Opened file "<< filename << std::endl;
  } else {
    std::cout << "No file found for run " << runnum << " in path " 
	      << FILE_PATH << std::endl;
    return NULL;
  }

  mymul = (TTree*)gROOT->FindObject("mul");
  mymulc = (TTree*)gROOT->FindObject("mulc");
  mymulc_lrb = (TTree*)gROOT->FindObject("mulc_lrb");

  mymul->AddFriend(mymulc);
  mymul->AddFriend(mymulc_lrb);
  
  return myfile;
}

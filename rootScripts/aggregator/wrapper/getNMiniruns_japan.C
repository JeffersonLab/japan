void getNMiniruns_japan(TString runNumber = "0") {
  Int_t miniruns = -1;
  // Get the Japan file, open it, find minirun, find its final entry, get that value, return
  //Open(runNumber); // FIXME For JAPAN version of code - code below
  TFile *fin;
  const char* FILE_PATH = "$QW_ROOTFILES";    //path to folder that contains rootfiles

  TString filename;
  TString stemlist[5] = {"prexPrompt_pass2_",
    "prexPrompt_pass1_", 
    "prexALL_",
    "prexALLminusR_",
    "prexinj_"};
  for  (int i=0; i<5; i++){
    filename = Form("%s/%s%s.000.root",FILE_PATH,
        stemlist[i].Data(),runNumber.Data());
    fin = new TFile(filename);
    if (fin->IsOpen()) {break;}
  }
  if (fin->IsOpen()) {
    std::cerr << "Opened file "<< filename << std::endl;
  } else {
    std::cerr << "No file found for run " << runNumber << " in path " 
	      << FILE_PATH << std::endl;
    return NULL;
  }
  TTree *T=(TTree*)fin->Get("burst");
  miniruns = T->Scan("BurstCounter","");
  Printf("NMiniruns=%d",miniruns);
}

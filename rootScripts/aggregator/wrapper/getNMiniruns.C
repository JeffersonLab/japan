void getNMiniruns(TString runNumber = "0") {
  Int_t miniruns = -1;
  // Get the post pan file, open it, find minirun, find its final entry, get that value, return
  //Open(runNumber); // FIXME For JAPAN version of code
  TString postpanBaseDir = gSystem->Getenv("POSTPAN_ROOTFILES");
  TString fnm = postpanBaseDir + "/prexPrompt_"+runNumber+"_000_regress_postpan.root";
  TFile *fin = TFile::Open(fnm.Data(),"READ");
  TTree *T=(TTree*)fin->Get("mini");
  miniruns = T->Scan("minirun","");
  Printf("NMiniruns=%d",miniruns);
}

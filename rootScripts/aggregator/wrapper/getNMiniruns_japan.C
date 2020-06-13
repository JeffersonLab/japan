void getNMiniruns_postpan(TString runNumber = "0") {
  Int_t miniruns = -1;
  // Get the post pan file, open it, find minirun, find its final entry, get that value, return
  //Open(runNumber); // FIXME For JAPAN version of code
  TString fnm = "$QW_ROOTFILES/prexPrompt_pass1_"+runNumber+".000.root";
  TFile *fin = TFile::Open(fnm.Data(),"READ");
  TTree *T=(TTree*)fin->Get("burst");
  miniruns = T->Scan("BurstCounter","");
  Printf("NMiniruns=%d",miniruns);
}

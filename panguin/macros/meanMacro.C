void meanMacro(char* rn) {

  TFile *_file0 = TFile::Open(Form("prexRespin1_%s.root",rn));
  TTree* evt = (TTree*)gROOT->FindObject("evt");
  evt->Draw(Form("bcm_an_ds"), Form("ErrorFlag==0 && bcm_an_ds3.Device_Error_Code == 0"));
  TH1* htemp = (TH1*)gROOT->FindObject("htemp");

  if (htemp != NULL) {
    double mean = htemp->GetMean();
    cout << endl << mean << endl << endl;
  }

}

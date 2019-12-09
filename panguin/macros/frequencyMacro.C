void frequencyMacro(char* rn) {

  TFile *_file0 = TFile::Open(Form("prexRespin1_%s.root",rn));
  TTree* evt = (TTree*)gROOT->FindObject("evt");

  evt->Scan("bcm_an_ds3.num_samples:4e6/clk4mhz_1","CodaEventNumber==100");

}

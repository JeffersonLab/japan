//#include <string>
//Gives FFTs of BCM data/BCM asymmetries on one canvas 

void FFT_Tree(int run, int run2, string tree_input) {

 //setup tree, pull and cut data 
  TFile *file = new TFile(Form("/adaq1/work1/apar/japanOutput/prexPrompt_pass1_%d.000.root", run));
  TFile *file2 = new TFile(Form("/adaq1/work1/apar/japanOutput/prexPrompt_pass1_%d.000.root", run2));
  TTree *tree = (TTree*) file->Get(Form("%s", tree_input.c_str())); 
  TTree *tree2 = (TTree*) file2->Get(Form("%s", tree_input.c_str())); 
  TString TStree = tree_input;
  TString TSevt = "evt";
  TString TSmul = "mul";
  TString TSpair = "pr";
  string plot1;
  string plot2;
  string plot3;
  string plot4;
  Double_t frequency=0;
  if (TStree.CompareTo(TSevt) == 0) {
    frequency = 240*4;
    plot1 = "bcm_an_us";
    plot2 = "bcm_dg_us";
    plot3 = "bcm_an_ds";
    plot4 = "bcm_dg_ds";
    cout << "event tree..." << endl;
  }
  else if (TStree.CompareTo(TSmul) == 0) {
    frequency = 30*4;
    plot1 = "asym_bcm_an_us";
    plot2 = "asym_bcm_dg_us";
    plot3 = "asym_bcm_an_ds";
    plot4 = "asym_bcm_dg_ds";
  }
  else if (TStree.CompareTo(TSpair) == 0) {
    frequency = 240*4/2;
  }
  else {
    cout << "Tree name not recognized. Please specify frequency in file." << endl;
  }
  cout << "Calculating FFT with frequency of " << frequency << endl;

  
  //initialize important variables 
  TString evcut = "ErrorFlag==0";
  int numevents = tree->Draw(Form("%s", plot1.c_str()), evcut, "goff"); 
  int numevents2 = tree2->Draw(Form("%s", plot1.c_str()), evcut, "goff"); 
  cout << "got number of events.." << endl;
  //float dataarray[4][numevents*4];
  //float dataarray2[4][numevents2*4];
  string plots[4] = {plot1, plot2, plot3, plot4};
  cout << "initialized variables...." << endl;

  /*
  TCanvas *rawhist = new TCanvas();
  rawhist->Divide(2, 2);
  for (int n=0; n<4; n++) {
    rawhist->cd(n+1);
    tree->Draw(Form("%s", plots[n].c_str()), evcut);
  }
  */

  
  //make and fill the data histogram
  TCanvas *fftdata = new TCanvas();
  //fftdata->Divide(2, 2);
  TH1D *datahist1 = new TH1D("datahist", plot1.c_str(), numevents*4, 0, numevents);
  TH1D *datahist2 = new TH1D("datahist", plot2.c_str(), numevents*4, 0, numevents);
  TH1D *datahist3 = new TH1D("datahist", plot3.c_str(), numevents*4, 0, numevents);
  TH1D *datahist4 = new TH1D("datahist", plot4.c_str(), numevents*4, 0, numevents);
  TH1D *datahist1_t2 = new TH1D("datahist", plot1.c_str(), numevents2*4, 0, numevents2);
  TH1D *datahist2_t2 = new TH1D("datahist", plot2.c_str(), numevents2*4, 0, numevents2);
  TH1D *datahist3_t2 = new TH1D("datahist", plot3.c_str(), numevents2*4, 0, numevents2);
  TH1D *datahist4_t2 = new TH1D("datahist", plot4.c_str(), numevents2*4, 0, numevents2);
  TH1D *datahist[4] = {datahist1, datahist2, datahist3, datahist4};
  TH1D *datahist_t2[4] = {datahist1_t2, datahist2_t2, datahist3_t2, datahist4_t2};
  cout << "created histograms..." << endl;
  for(int canv=0; canv<1; canv++) {
    tree->Draw(Form("%s.block0:%s.block1:%s.block2:%s.block3", plots[canv].c_str(), plots[canv].c_str(), plots[canv].c_str(), plots[canv].c_str()), evcut, "goff");
    for(int event=0; event<numevents; event++) {
      datahist[canv]->SetBinContent(event*4, tree->GetV1()[event]);
      datahist[canv]->SetBinContent(event*4+1, tree->GetV2()[event]);
      datahist[canv]->SetBinContent(event*4+2, tree->GetV3()[event]);
      datahist[canv]->SetBinContent(event*4+3, tree->GetV4()[event]);
      cout << event << endl;
    }
  }
  cout << "filled histogram 1..." << endl;
  for (int canv=0; canv<1; canv++) {
    tree2->Draw(Form("%s.block0:%s.block1:%s.block2:%s.block3", plots[canv].c_str(), plots[canv].c_str(), plots[canv].c_str(), plots[canv].c_str()), evcut, "goff");
    for(int event=0; event<numevents2; event++) {
      datahist_t2[canv]->SetBinContent(event*4, tree2->GetV1()[event]);
      datahist_t2[canv]->SetBinContent(event*4+1, tree2->GetV2()[event]);
      datahist_t2[canv]->SetBinContent(event*4+2, tree2->GetV3()[event]);
      datahist_t2[canv]->SetBinContent(event*4+3, tree2->GetV4()[event]);
    }
    fftdata->cd(canv+1);
    datahist[canv]->Draw();
    datahist_t2[canv]->SetLineColor(kRed);
    datahist_t2[canv]->Draw("same");
  }
  cout << "filled histogram 2..." << endl;
  //do the transforms
  TH1 *ffthist1 = NULL;
  TH1 *ffthist2 = NULL;
  TH1 *ffthist3 = NULL;
  TH1 *ffthist4 = NULL;
  TH1 *ffthist1_t2 = NULL;
  TH1 *ffthist2_t2 = NULL;
  TH1 *ffthist3_t2 = NULL;
  TH1 *ffthist4_t2 = NULL;
  TH1 *ffthist[4] = {ffthist1, ffthist2, ffthist3, ffthist4};
  TH1 *ffthist_t2[4] = {ffthist1_t2, ffthist2_t2, ffthist3_t2, ffthist4_t2};
  Double_t nbins;
  Double_t nbins2;
  Double_t scale = 1.0/sqrt(numevents);
  Double_t scale2 = 1.0/sqrt(numevents2);
  
  for (int canv=0; canv<1; canv++) {
    ffthist[canv] = datahist[canv]->FFT(ffthist[canv], "MAG");
    ffthist_t2[canv] = datahist_t2[canv]->FFT(ffthist_t2[canv], "MAG");
    ffthist[canv]->SetTitle("Magnitude of Transform");
    ffthist_t2[canv]->SetTitle("Magnitude of Transform");
  }
  cout << "done transforms..." << endl;
  TAxis *xa1 = ffthist[0]->GetXaxis();
  TAxis *xa1_t2 = ffthist_t2[0]->GetXaxis();
  /*
  TAxis *xa2 = ffthist[1]->GetXaxis();
  TAxis *xa3 = ffthist[2]->GetXaxis();
  TAxis *xa4 = ffthist[3]->GetXaxis();
  TAxis *xa2_t2 = ffthist_t2[1]->GetXaxis();
  TAxis *xa3_t2 = ffthist_t2[2]->GetXaxis();
  TAxis *xa4_t2 = ffthist_t2[3]->GetXaxis();
  */
  nbins = xa1->GetNbins();
  nbins2 = xa1_t2->GetNbins();
  
 
  TH1D *scaled1 = new TH1D("scaled1", Form("%s", plot1.c_str()) ,nbins,1,frequency); 
  TH1D *scaled1_t2 = new TH1D("scaled1", Form("%s", plot1.c_str()) ,nbins2,1,frequency); 
  TH1D *scaled2 = new TH1D("scaled2", Form("%s", plot2.c_str()) ,nbins,1,frequency); 
  TH1D *scaled3 = new TH1D("scaled3", Form("%s", plot3.c_str()) ,nbins,1,frequency);
  TH1D *scaled4 = new TH1D("scaled4", Form("%s", plot4.c_str()) ,nbins,1,frequency); 
  TH1D *scaled2_t2 = new TH1D("scaled2", Form("%s", plot2.c_str()) ,nbins2,1,frequency); 
  TH1D *scaled3_t2 = new TH1D("scaled3", Form("%s", plot3.c_str()) ,nbins2,1,frequency);
  TH1D *scaled4_t2 = new TH1D("scaled4", Form("%s", plot4.c_str()) ,nbins2,1,frequency); 
  TH1D *scaled[4] = {scaled1, scaled2, scaled3, scaled4};
  TH1D *scaled_t2[4] = {scaled1_t2, scaled2_t2, scaled3_t2, scaled4_t2};
  
  
  for(int canv=0; canv<1; canv++) {
    for(int event=0; event<nbins; event++ ) {
      scaled[canv]->SetBinContent(event+1, scale*ffthist[canv]->GetBinContent(event+1));
    }
    for(int event=0; event<nbins2; event++ ) {
      scaled_t2[canv]->SetBinContent(event+1, scale2*ffthist_t2[canv]->GetBinContent(event+1));
    }
    scaled[canv]->GetXaxis() -> SetTitle("Frequency (Hz)");
    scaled[canv]->GetYaxis() -> SetTitle("Amplitude");
    scaled_t2[canv]->GetXaxis() -> SetTitle("Frequency (Hz)");
    scaled_t2[canv]->GetYaxis() -> SetTitle("Amplitude");
  }

  TAxis *xb1 = scaled[0]->GetXaxis();
  /*
  TAxis *xb2 = scaled[1]->GetXaxis();
  TAxis *xb3 = scaled[2]->GetXaxis();
  TAxis *xb4 = scaled[3]->GetXaxis();
  */
  TAxis *xb1_t2 = scaled_t2[0]->GetXaxis();
  /*
  TAxis *xb2_t2 = scaled_t2[1]->GetXaxis();
  TAxis *xb3_t2 = scaled_t2[2]->GetXaxis();
  TAxis *xb4_t2 = scaled_t2[3]->GetXaxis();
  */
  xb1 -> SetRangeUser(xa1->GetBinLowEdge(1), xa1->GetBinUpEdge(nbins/2.0));
  /*
  xb2 -> SetRangeUser(xa2->GetBinLowEdge(1), xa2->GetBinUpEdge(nbins/2.0));
  xb3 -> SetRangeUser(xa3->GetBinLowEdge(1), xa3->GetBinUpEdge(nbins/2.0));
  xb4 -> SetRangeUser(xa4->GetBinLowEdge(1), xa4->GetBinUpEdge(nbins/2.0));
  */
  xb1_t2 -> SetRangeUser(xa1_t2->GetBinLowEdge(1), xa1_t2->GetBinUpEdge(nbins2/2.0));
  /*
  xb2_t2 -> SetRangeUser(xa2_t2->GetBinLowEdge(1), xa2_t2->GetBinUpEdge(nbins2/2.0));
  xb3_t2 -> SetRangeUser(xa3_t2->GetBinLowEdge(1), xa3_t2->GetBinUpEdge(nbins2/2.0));
  xb4_t2 -> SetRangeUser(xa4_t2->GetBinLowEdge(1), xa4_t2->GetBinUpEdge(nbins2/2.0));
  */

  TCanvas *c_fft = new TCanvas("c_fft", "FFT Canvas");
  //c_fft->Divide(2, 2);
  for (int canv=0; canv<1; canv++) {
    c_fft->cd(canv+1);
    scaled[canv]->GetXaxis()->SetRangeUser(0, frequency/2);
    if (TStree.CompareTo(TSevt) == 0) {
      scaled[canv]->SetMaximum(12);
    }
    scaled[canv]->SetLineWidth(3);
    scaled[canv]->Draw();
    scaled_t2[canv]->SetLineColor(kRed);
    scaled_t2[canv]->Draw("same");
  }

  c_fft->SaveAs(Form("FFT%d_%stree.pdf", run, tree_input.c_str()));




  

  


}
  
  
 

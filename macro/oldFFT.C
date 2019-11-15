//How to use:
//root [0] .L FFT.C
//root [1] FFT(run_number, "variable")
//run_number: number of the run to be analyzed
//variable: relevant parameter in event tree 
void FFT(int run, string param) {
  
  //setup tree, canvas
  TFile *file = new TFile(Form("/adaq1/work1/apar/japanOutput/prexPrompt_pass2_%d.000.root", run));
  TTree *tree_evt = (TTree*) file->Get("evt");
  //TEntryList *elist = (TEntryList*) file->Get("elist");
  //tree_evt->SetEntryList(elist);
  int numevents = tree_evt->Draw(Form("%s", param.c_str()), "", "goff");
  
  TCanvas *a = new TCanvas();
  TH1F *htemp = new TH1F("htemp", "test", numevents, -10000, 10000);
  tree_evt->Draw(Form("%s>>htemp", param.c_str()), "", "goff");
  //set x axis range 
  int lowbin = 0;
  int highbin = numevents;
  int ymax = htemp->GetMaximum();
  cout << ymax << endl;
  for (int i=0; i<numevents; i++) {
    if (htemp->GetBinContent(i) > ymax/10 && abs(htemp->GetXaxis()->GetBinLowEdge(i)) > 10) {
      lowbin = i;
      break;
    }
  }
  for(int i=numevents; i>0; i=i-1) {
    if (htemp->GetBinContent(i) > ymax/10) {
      highbin = i;
      break;
    }
  }
  
  htemp->GetXaxis()->SetRange(lowbin, highbin);
  double lowx = htemp->GetXaxis()->GetBinLowEdge(lowbin);
  double highx = htemp->GetXaxis()->GetBinUpEdge(highbin);
  cout << lowx << endl;
  cout << highx << endl;
  htemp->Draw();
  
  //pull out each sum block, fill corresponding arrays
  tree_evt->Draw(Form("%s.block0:%s.block1:%s.block2:%s.block3", param.c_str(), param.c_str(), param.c_str(), param.c_str()), "", "goff");
  Double_t time[numevents*4];
  Double_t dataarray[numevents*4];
  
  for(int i=0; i<numevents; i++) {
    dataarray[i*4] = tree_evt->GetV1()[i];
    dataarray[i*4+1] = tree_evt->GetV2()[i];
    dataarray[i*4+2] = tree_evt->GetV3()[i];
    dataarray[i*4+3] = tree_evt->GetV4()[i];
    datahist->Fill(tree_evt->GetV1()[i]);
    datahist->Fill(tree_evt->GetV2()[i]);
    datahist->Fill(tree_evt->GetV3()[i]);
    datahist->Fill(tree_evt->GetV4()[i]);
    time[i*4] = i;
    time[i*4+1] = i+0.25;
    time[i*4+2] = i+0.5;
    time[i*4+3] = i+0.75;
  }
  
  TGraph *testgraph = new TGraph(numevents*4, time, dataarray);
  datahist->Draw();
  TH1D *datahist = new TH1D("testh", "data histogram", numevents*4, 0, numevents);
  //do the transform
  TCanvas *c_fft = new TCanvas("c_fft", "FFT Canvas");
  c_fft->SetLogy();
  TH1 *ffthist = NULL;
  TVirtualFFT::SetTransform(0);
  ffthist = datahist->FFT(ffthist, "MAG");
  ffthist->SetTitle("Magnitude of transform");

  
  //set up scaling factors
  TAxis *xa = ffthist->GetXaxis();
  Double_t nbins = xa->GetNbins();
  Double_t frequency = 480;
  Double_t scale = 1.0/sqrt(numevents);

  TH1D *h3 = new TH1D("h3", "FFT Magnitude" ,nbins,1,frequency);  
  for(Int_t n = 0;n<nbins;n++){
    h3->SetBinContent(n+1,scale*ffthist->GetBinContent(n+1)); 
  }
  h3 -> GetXaxis() -> SetTitle("Frequency (Hz)");
  h3 -> GetYaxis() -> SetTitle("Amplitude");
  //h3->Draw();
  
  TAxis *xb = h3 -> GetXaxis();
  xb -> SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins()/2.0));
  h3->SetLineColor(kBlue);
  h3 -> Draw();
  
  /*
  //A function to sample
  
  TCanvas *c1_2 = new TCanvas;
  TCanvas *test = new TCanvas;
  TF1 *fsin = new TF1("fsin", "cos(100*x)+sin(200*x)+cos(20*x)+sin(459*x)", 0, 1); //time: 1 second
  fsin->Draw();
    
  Int_t n=1000;
  TH1D *hsin = new TH1D("hsin", "hsin", n, 0, 1);
  Double_t x;
    
  //Fill the histogram with function values
  for (Int_t i=0; i<=n; i++){
     x = (Double_t(i)/n)*(1);
     hsin->SetBinContent(i+1, fsin->Eval(x));
  }
  hsin->Draw("same");
  fsin->GetXaxis()->SetLabelSize(0.05);
  fsin->GetYaxis()->SetLabelSize(0.05);
   
  c1_2->cd();
  //Compute the transform and look at the magnitude of the output
  TH1 *hm =0;
  TVirtualFFT::SetTransform(0);
  hm = hsin->FFT(hm, "MAG");
  hm->SetTitle("Magnitude of the 1st transform");
  hm->Draw();
  
  TAxis *xa = hm->GetXaxis();
  Double_t nbins = xa->GetNbins();
  cout << nbins << endl;
  Double_t frequency = 1000;
  Double_t scale = 1.0/sqrt(1000);
  
  TH1D *h3 = new TH1D("h3","test hist" ,nbins,1,frequency/0.16);  
  for(Int_t n = 0;n<nbins;n++){
    h3->SetBinContent(n,scale*hm->GetBinContent(n+1)); 
    if(n<500) {
      cout << "bin is: " << n << endl;
      cout << "value is: " << scale*hm->GetBinContent(n+1) << endl;
    }
  }
  h3 -> GetXaxis() -> SetTitle("Frequency (Hz)");
  h3 -> GetYaxis() -> SetTitle("Amplitude");
  h3->Draw();
  
  TAxis *xb = h3 -> GetXaxis();
  xb -> SetRangeUser(xa->GetBinLowEdge(1),xa->GetBinUpEdge(xa->GetNbins()/2.0));
  h3->SetLineColor(kBlue);
  h3 -> Draw();
  */
  
  

  
}

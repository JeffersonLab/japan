//How to use:
//root [0] .L FFT.C
//root [1] FFT(run_number, "variable")
//run_number: number of the run to be analyzed
//variable: relevant parameter in event tree 
void FFT(int run, string param) {
  
  //setup tree, pull and cut data, export files for frequencies 
  TFile *file = new TFile(Form("/adaq1/work1/apar/japanOutput/prexPrompt_pass1_%d.000.root", run));
  TTree *tree_evt = (TTree*) file->Get("evt"); 
  TString evcut = "ErrorFlag==0";
  int numevents = tree_evt->Draw(Form("%s", param.c_str()), evcut, "goff");
  ofstream freqfile;
  Double_t frequency = 240*4; //frequency of sampling
  //mul tree is always 30 Hz
  //pr tree is half the rate of evt tree 

  //draw the raw data histogram (not versus time)
  TCanvas *rawdata = new TCanvas();
  tree_evt->Draw(Form("%s", param.c_str()), evcut);
 
  //pull out each sum block, fill corresponding arrays
  tree_evt->Draw(Form("%s.block0:%s.block1:%s.block2:%s.block3", param.c_str(), param.c_str(), param.c_str(), param.c_str()), evcut, "goff");
  Double_t dataarray[numevents*4];  
  for(int i=0; i<numevents; i++) {
    dataarray[i*4] = tree_evt->GetV1()[i];
    dataarray[i*4+1] = tree_evt->GetV2()[i];
    dataarray[i*4+2] = tree_evt->GetV3()[i];
    dataarray[i*4+3] = tree_evt->GetV4()[i];
  }
  
  //make and fill the data histogram, send relevant frequencies to a file
  TCanvas *fftdata = new TCanvas();
  TH1D *datahist = new TH1D("datahist", "Data to FFT", numevents*4, 0, numevents);

  //freqfile.open (Form("freq%d_%s.txt", run, param.c_str()));
  for (int i=0; i<numevents*4; i++) {    
    if(dataarray[i] == 0) {
      break;
    }
    datahist->SetBinContent(i, dataarray[i]);   
  }

  datahist->Draw();
  
  
 
  //do the transform
  TH1 *ffthist = NULL;
  TVirtualFFT::SetTransform(0);
  ffthist = datahist->FFT(ffthist, "MAG");
  ffthist->SetTitle("Magnitude of transform");


  
  //set up scaling factors
  TAxis *xa = ffthist->GetXaxis();
  Double_t nbins = xa->GetNbins();
  Double_t scale = 1.0/sqrt(numevents);
  cout << nbins << endl;
  TH1D *h3 = new TH1D("h3", Form("%s", param.c_str()) ,nbins,1,frequency);  
  for(Int_t n = 0;n<nbins;n++){
    h3->SetBinContent(n+1,scale*ffthist->GetBinContent(n+1)); 
    /*
    if (h3->GetBinContent(n+1) >0.4  && n<(nbins/2)) {
      double freq = ((n+1)/nbins)*frequency;
      freqfile << freq << "\n";
      cout << freq << endl;
    }
    */
  }

  
  h3 -> GetXaxis() -> SetTitle("Frequency (Hz)");
  h3 -> GetYaxis() -> SetTitle("Amplitude");

  
  TAxis *xb = h3 -> GetXaxis();
  xb -> SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins()/2.0));
  
  //draw everything, save canvas as a pdf 
  TCanvas *c_fft = new TCanvas("c_fft", "FFT Canvas");
  c_fft->Divide(2, 2);
  c_fft->cd(1)->SetLogy();
  h3->SetLineColor(kBlue);
  h3 -> Draw();

  c_fft->cd(2);
  h3->SetMaximum(10);
  h3->Draw();

  TH1D *h3scaled = (TH1D*) h3->Clone();
  h3scaled->GetXaxis()->SetRangeUser(0, frequency/2);
  c_fft->cd(3)->SetLogy();
  h3scaled->Draw();

  c_fft->cd(4);
  h3scaled->Draw();
  //gPad->BuildLegend(0.25, 0.21, 0.25, 0.21, "Data");
  c_fft->SaveAs(Form("FFT%d_%s.pdf", run, param.c_str()));
  

  
}

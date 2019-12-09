//How to use:
//root [0] .L FFT.C
//root [1] FFT(run_number, "variable")
//run_number: number of the run to be analyzed
//variable: relevant parameter in event tree 
void FFT_DD(int run, string param) {
  
  //setup tree, pull and cut data, export files for frequencies 
  TFile *file = new TFile(Form("/adaq1/work1/apar/japanOutput/prexPrompt_pass1_%d.000.root", run));
  TTree *tree_evt = (TTree*) file->Get("evt");
  TString evcut = "ErrorFlag==0 && CodaEventNumber<130000 && CodaEventNumber>95000";
  ofstream freqfile;
  Double_t frequency = 240*4; //frequency of sampling
  //mul tree is always 30 Hz
  //pr tree is half the rate of evt tree 

  //set up variables for an_ds-dg_ds and dg_us - dg_ds
  string andgds = "bcm_an_ds-bcm_dg_ds";
  string dgusds = "bcm_dg_us-bcm_dg_ds";
  int numevents = tree_evt->Draw(Form("%s", param.c_str()), evcut, "goff");
  int num_andgds = tree_evt->Draw("bcm_an_ds-bcm_dg_ds", evcut, "goff");
  int num_dgusds = tree_evt->Draw("bcm_dg_us-bcm_dg_ds", evcut, "goff");

  //draw the raw data histogram (not versus time)
  TCanvas *rawdata = new TCanvas();
  tree_evt->Draw(Form("%s", param.c_str()), evcut);
  cout << "drawn" << endl;
  //pull out each sum block, fill corresponding arrays
  tree_evt->Draw(Form("%s.block0:%s.block1:%s.block2:%s.block3", param.c_str(), param.c_str(), param.c_str(), param.c_str()), evcut, "goff");
  Double_t dataarray[numevents*4];  
  for(int i=0; i<numevents; i++) {
    dataarray[i*4] = tree_evt->GetV1()[i];
    dataarray[i*4+1] = tree_evt->GetV2()[i];
    dataarray[i*4+2] = tree_evt->GetV3()[i];
    dataarray[i*4+3] = tree_evt->GetV4()[i];
  }

  tree_evt->Draw(Form("%s.block0:%s.block1:%s.block2:%s.block3", andgds.c_str(), andgds.c_str(), andgds.c_str(), andgds.c_str()), evcut, "goff");
  Double_t array_andgds[num_andgds*4];  
  for(int i=0; i<num_andgds; i++) {
    array_andgds[i*4] = tree_evt->GetV1()[i];
    array_andgds[i*4+1] = tree_evt->GetV2()[i];
    array_andgds[i*4+2] = tree_evt->GetV3()[i];
    array_andgds[i*4+3] = tree_evt->GetV4()[i];
  }

  tree_evt->Draw(Form("%s.block0:%s.block1:%s.block2:%s.block3", dgusds.c_str(), dgusds.c_str(), dgusds.c_str(), dgusds.c_str()), evcut, "goff");
  Double_t array_dgusds[num_dgusds*4];  
  for(int i=0; i<num_dgusds; i++) {
    array_dgusds[i*4] = tree_evt->GetV1()[i];
    array_dgusds[i*4+1] = tree_evt->GetV2()[i];
    array_dgusds[i*4+2] = tree_evt->GetV3()[i];
    array_dgusds[i*4+3] = tree_evt->GetV4()[i];
  }
  
  //make and fill the data histogram, send relevant frequencies to a file
  TCanvas *fftdata = new TCanvas();
  TH1D *datahist = new TH1D("datahist", "Data to FFT", numevents*4, 0, numevents);
  TH1D *datahist_andgds = new TH1D("datahist_andgds", "ANDGDS", num_andgds*4, 0, num_andgds);
  TH1D *datahist_dgusds = new TH1D("datahist_dgusds", "DGUSDS", num_dgusds*4, 0, num_dgusds);
  //freqfile.open (Form("freq%d_%s.txt", run, param.c_str()));
  for (int i=0; i<numevents*4; i++) {    
    if(dataarray[i] == 0) {
      break;
    }
    datahist->SetBinContent(i, dataarray[i]);   
  }

  for (int i=0; i<num_andgds*4; i++) {    
    if(array_andgds[i] == 0) {
      break;
    }
    datahist_andgds->SetBinContent(i, array_andgds[i]);   
  }

  for (int i=0; i<num_dgusds*4; i++) {    
    if(array_dgusds[i] == 0) {
      break;
    }
    datahist_dgusds->SetBinContent(i, array_dgusds[i]);   
  }
  datahist->Draw();
  
  
 
  //do the transform
  TH1 *ffthist = NULL;
  TVirtualFFT::SetTransform(0);
  ffthist = datahist->FFT(ffthist, "MAG");
  ffthist->SetTitle("Magnitude of transform");

  TH1 *ffthist_andgds = NULL;
  TVirtualFFT::SetTransform(0);
  ffthist_andgds = datahist_andgds->FFT(ffthist_andgds, "MAG");
  ffthist_andgds->SetTitle("Magnitude of transform");

  TH1 *ffthist_dgusds = NULL;
  TVirtualFFT::SetTransform(0);
  ffthist_dgusds = datahist_dgusds->FFT(ffthist_dgusds, "MAG");
  ffthist_dgusds->SetTitle("Magnitude of transform");

  
  //set up scaling factors
  TAxis *xa = ffthist->GetXaxis();
  Double_t nbins = xa->GetNbins();
  Double_t scale = 1.0/sqrt(numevents);

  TAxis *xa_andgds = ffthist_andgds->GetXaxis();
  Double_t nbins_andgds = xa_andgds->GetNbins();
  Double_t scale_andgds = 1.0/sqrt(num_andgds);

  TAxis *xa_dgusds = ffthist_dgusds->GetXaxis();
  Double_t nbins_dgusds = xa_dgusds->GetNbins();
  Double_t scale_dgusds = 1.0/sqrt(num_dgusds);

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

  TH1D *h3_andgds = new TH1D("h3_andgds", "bcm_an_ds-bcm_dg_ds",nbins_andgds,1,frequency);  
  for(Int_t n = 0;n<nbins_andgds;n++){
    h3_andgds->SetBinContent(n+1,scale_andgds*ffthist_andgds->GetBinContent(n+1)); 
  }

  TH1D *h3_dgusds = new TH1D("h3_dgusds", "bcm_dg_us-bcm_dg_ds" ,nbins_dgusds,1,frequency);  
  for(Int_t n = 0;n<nbins_dgusds;n++){
    h3_dgusds->SetBinContent(n+1,scale_dgusds*ffthist_dgusds->GetBinContent(n+1)); 
  }

  
  h3 -> GetXaxis() -> SetTitle("Frequency (Hz)");
  h3 -> GetYaxis() -> SetTitle("Amplitude");

  
  TAxis *xb = h3 -> GetXaxis();
  xb -> SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins()/2.0));
  TAxis *xb_andgds = h3_andgds -> GetXaxis();
  xb_andgds -> SetRangeUser(xa_andgds->GetBinLowEdge(1), xa_andgds->GetBinUpEdge(xa_andgds->GetNbins()/2.0));
  TAxis *xb_dgusds = h3_dgusds -> GetXaxis();
  xb_dgusds -> SetRangeUser(xa_dgusds->GetBinLowEdge(1), xa_dgusds->GetBinUpEdge(xa_dgusds->GetNbins()/2.0));

  //draw everything, save canvas as a pdf 
  TCanvas *c_fft = new TCanvas("c_fft", "FFT Canvas");
  c_fft->Divide(2, 2);
  c_fft->cd(1)->SetLogy();
  h3->SetLineColor(kBlue);
  h3->SetLineWidth(3);
  h3 -> Draw();
  h3_andgds->SetLineColor(kRed);
  h3_andgds->SetLineWidth(2);
  h3_andgds->Draw("same");
  h3_dgusds->SetLineColor(kGreen);
  h3_dgusds->Draw("same");

  c_fft->cd(2);
  h3->SetMaximum(10);
  h3->Draw();
  h3_andgds->Draw("same");
  h3_dgusds->Draw("same");

  TH1D *h3scaled = (TH1D*) h3->Clone();
  TH1D *h3scaled_andgds = (TH1D*) h3_andgds->Clone();
  TH1D *h3scaled_dgusds = (TH1D*) h3_dgusds->Clone();
  h3scaled->GetXaxis()->SetRangeUser(0, 100);
  c_fft->cd(3)->SetLogy();
  h3scaled->Draw();
  h3scaled_andgds->Draw("same");
  h3scaled_dgusds->Draw("same");
  c_fft->cd(4);
  h3scaled->Draw();
  h3scaled_andgds->Draw("same");
  h3scaled_dgusds->Draw("same");
  gPad->BuildLegend(0.25, 0.21, 0.25, 0.21, "Data");
  c_fft->SaveAs(Form("FFT%d_%s.pdf", run, param.c_str()));
  

  
}

void pullAq(Int_t flip_state = 1, TString pass = "respin2", TString ana = "Aq") {

  TCanvas * c1 = new TCanvas();
  c1->Divide(3,2);
  //TFile * infile = new TFile("/chafs1/work1/apar/cameronc/rootfiles/respin1_prune_rootfiles/test.root");
  TTree *mul_tree;
  //infile->GetObject("mul_trip", mul_tree);
  TString draw = "asym_bcm_an_us.hw_sum/1e-9";
  TString name = "Aq (ppb)";
  TString title = "Aq";
  TString cut = "(rcdb_run_type == 1 && rcdb_run_flag == 1) ";

  if (ana == "BPM12X") {
    draw = "diff_bpm12X.hw_sum/1e-6";
    name = "12X (nm)";
    title = "BPM12X";
  }
  if (ana == "BPM1X") {
    draw = "diff_bpm1X.hw_sum/1e-6";
    name = "1X (nm)";
    title = "BPM1X";
  }
  if (ana == "BPM4eX") {
    draw = "diff_bpm4eX.hw_sum/1e-6";
    name = "4eX (nm)";
    title = "BPM4eX";
  }
  if (ana == "BPM4eY") {
    draw = "diff_bpm4eY.hw_sum/1e-6";
    name = "4eY (nm)";
    title = "BPM4eY";
  }
  if (ana == "raw_us_avg") {
    draw = "asym_us_avg.hw_sum/1e-9";
    name = "Raw US Avg (ppb, 2 arm running only)";
    title = "rawUSAvg";
    cut = "(" + cut + " && rcdb_arm_flag==0)";
  }
  if (ana == "reg_us_avg") {
    draw = "cor_asym_us_avg.hw_sum/1e-9";
    name = "Reg US Avg (ppb, 2 arm running only)";
    title = "regUSAvg";
    cut = "(" + cut + " && rcdb_arm_flag==0)";
  }

  std::vector<Double_t> a_means;
  std::vector<Double_t> a_meanerrs;
  std::vector<Double_t> b_means;
  std::vector<Double_t> b_meanerrs;
  std::vector<Double_t> c_means;
  std::vector<Double_t> c_meanerrs;
  std::vector<Double_t> bins_a;
  std::vector<Double_t> bins_b;
  std::vector<Double_t> bins_c;

  //for (Int_t i = 5785; i < 5820; i++) {
  for (Int_t i = 5408; i <= 8558; i++) {
    TFile * infile_run = new TFile(Form("%s_rootfiles/trip_%d.root",pass.Data(),i));
    infile_run->GetObject("mul_trip", mul_tree);
    if (!mul_tree) { continue; }
    Int_t nData_a = mul_tree->Draw(Form("rcdb_sign*%s",draw.Data()),Form("%s && ErrorFlag==0 && SinceLastTripEnd<250 && rcdb_flip_state==%d && run_number==%d",cut.Data(),flip_state,i),"goff");
    if (nData_a != 0) {
      TH1F* a_th1 = (TH1F*)gROOT->FindObject("htemp");
      Double_t a_tmpMean = 0.0;
      Double_t a_tmpMeanErr = 0.0;
      a_tmpMean = a_th1->GetMean();
      a_tmpMeanErr = a_th1->GetMeanError();
      a_means.push_back(a_tmpMean);
      a_meanerrs.push_back(a_tmpMeanErr);
      bins_a.push_back(i);
    }
    Int_t nData_b = mul_tree->Draw(Form("rcdb_sign*%s",draw.Data()),Form("%s && ErrorFlag==0 && SinceLastTripEnd>=250 && SinceLastTripEnd<500 && rcdb_flip_state==%d && run_number==%d",cut.Data(),flip_state,i),"goff");
    if (nData_a != 0 && nData_b != 0) {
      TH1F* b_th1 = (TH1F*)gROOT->FindObject("htemp");
      Double_t b_tmpMean = 0.0;
      Double_t b_tmpMeanErr = 0.0;
      b_tmpMean = b_th1->GetMean();
      b_tmpMeanErr = b_th1->GetMeanError();
      b_means.push_back(b_tmpMean);
      b_meanerrs.push_back(b_tmpMeanErr);
      bins_b.push_back(i);
    }
    Int_t nData_c = mul_tree->Draw(Form("rcdb_sign*%s",draw.Data()),Form("%s && ErrorFlag==0 && SinceLastTripEnd>=500 && SinceLastTripEnd<750 && rcdb_flip_state==%d && run_number==%d",cut.Data(),flip_state,i),"goff");
    if (nData_a != 0 && nData_b != 0 && nData_c != 0) {
      TH1F* c_th1 = (TH1F*)gROOT->FindObject("htemp");
      Double_t c_tmpMean = 0.0; 
      Double_t c_tmpMeanErr = 0.0;
      c_tmpMean = c_th1->GetMean();
      c_tmpMeanErr = c_th1->GetMeanError();
      c_means.push_back(c_tmpMean);
      c_meanerrs.push_back(c_tmpMeanErr);
      bins_c.push_back(i);
    }
    infile_run->Close();
    delete infile_run;
  } 
  TGraphErrors * tga = new TGraphErrors(bins_a.size(),&bins_a[0],&a_means[0],0,&a_meanerrs[0]);
  TGraphErrors * tgb = new TGraphErrors(bins_b.size(),&bins_b[0],&b_means[0],0,&b_meanerrs[0]);
  TGraphErrors * tgc = new TGraphErrors(bins_c.size(),&bins_c[0],&c_means[0],0,&c_meanerrs[0]);
  gStyle->SetOptFit(1111);
  c1->cd(1);
  tga->SetNameTitle("New 250",Form("New %s data, 250 multiplets post trip cut;Run Number;%s",name.Data(),name.Data()));
  tga->SetMarkerStyle(20);
  tga->SetMarkerSize(0.5);
  tga->Fit("pol0","QW","");
  tga->Draw("ap");
  gPad->Update();
  //tga->SetStats(1);
  //(tga->GetListOfFunctions()->FindObject("stats"))->Draw();
  c1->cd(2);
  tgb->SetNameTitle("Standard 250",Form("Standard %s data, 250 multiplets afeter new trip cut;Run Number;%s",name.Data(),name.Data()));
  tgb->SetMarkerStyle(20);
  tgb->SetMarkerSize(0.5);
  tgb->Fit("pol0","QW","");
  tgb->Draw("ap");
  gPad->Update();
  //tgb->SetStats(1);
  //(tgb->GetListOfFunctions()->FindObject("stats"))->Draw();
  c1->cd(3);
  tgc->SetMarkerStyle(20);
  tgc->SetNameTitle("Standard 500",Form("Standard %s data, 250 multiplets, 500 after new trip cut;Run Number;%s",name.Data(),name.Data()));
  tgc->SetMarkerSize(0.5);
  tgc->Fit("pol0","QW","");
  tgc->Draw("ap");
  gPad->Update();
  //tgc->SetStats(1);
  //(tgc->GetListOfFunctions()->FindObject("stats"))->Draw();

  TH1F* h1c_vals = new TH1F("h1c_vals","Standard data, 250 multiplets, 500 after new trip cut, runwise pull plot",50,0,0);
  for (Int_t j = 0 ; j < bins_c.size(); j++) {
    h1c_vals->Fill(c_means[j],1);
  }
  Double_t default_mean = h1c_vals->GetMean();
  TH1F* h1a = new TH1F("h1a","New data, 250 multiplets, after new trip cut, runwise pull plot",50,0,0);
  TH1F* h1b = new TH1F("h1b","Standard data, 250 multiplets, 250 after new trip cut, runwise pull plot",50,0,0);
  TH1F* h1c = new TH1F("h1c","Standard data, 250 multiplets, 500 after new trip cut, runwise pull plot",50,0,0);
  for (Int_t j = 0 ; j < bins_c.size(); j++) {
    if (abs((a_means[j]-default_mean)/a_meanerrs[j]) > 100) {
      Printf("A) Run %d has a major issue",(Int_t)bins_a.at(j));
      Printf("A) First data = %f +- %f, vs. %f",a_means[j],default_mean,a_meanerrs[j]);
    }
    else {
      h1a->Fill((a_means[j]-default_mean)/a_meanerrs[j],1);
    }
    if (abs((b_means[j]-default_mean)/b_meanerrs[j]) > 100) {
      Printf("B) Run %d has a major issue",(Int_t)bins_b.at(j));
      Printf("B) First data = %f +- %f, vs. %f",b_means[j],default_mean,b_meanerrs[j]);
    }
    else {
      h1b->Fill((b_means[j]-default_mean)/b_meanerrs[j],1);
    }
    h1c->Fill((c_means[j]-default_mean)/c_meanerrs[j],1);
  }
  c1->cd(4);
  ((TVirtualPad*)c1->cd(4))->SetLogy();
  h1a->Draw();
  c1->cd(5);
  ((TVirtualPad*)c1->cd(5))->SetLogy();
  h1b->Draw();
  c1->cd(6);
  ((TVirtualPad*)c1->cd(6))->SetLogy();
  h1c->Draw();

  c1->SaveAs(Form("plots/test_pulls_wienstate%d_%s_%s.pdf",flip_state,title.Data(),pass.Data()));
}

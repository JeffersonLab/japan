void drawProf(Int_t flip_state = 1, TString ana = "Aq") {
  gStyle->SetOptStat(1221);
  TString treename = "mul_trip";
  TChain *mul_trip = new TChain(treename);
  mul_trip->SetCacheSize(20*1024*1024);
  mul_trip->Add("respin2_rootfiles/test.root");
  mul_trip->Add("respin2_rootfiles/test_1.root");
  mul_trip->Add("respin2_rootfiles/test_2.root");
  mul_trip->LoadTree(-1);
  /*
  TFile *_file0 = TFile::Open("respin1_rootfiles/test.root");
  TTree *mul_trip;
  _file0->GetObject("mul_trip", mul_trip);
  */
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

  TCanvas * c1 = new TCanvas();
  c1->Divide(2,3);
  c1->cd(1);
  mul_trip->Draw(Form("rcdb_sign*%s:SinceLastTripEnd>>respin2_ht1",draw.Data()),Form("%s && rcdb_flip_state == %d && ErrorFlag==0 && SinceLastTripEnd<=250",cut.Data(),flip_state),"goff");
  TH2* respin2_ht1 = (TH2*)gROOT->FindObject("respin2_ht1");
  respin2_ht1->SetNameTitle("Respin2 Candle",Form("Respin2, new %s data, all CREX wien state %d, post trip, candle;SinceLastTripEnd (multiplets);%s",name.Data(),flip_state,name.Data()));
  respin2_ht1->Draw("candlex3");
  c1->cd(2);
  mul_trip->Draw(Form("rcdb_sign*%s:SinceLastTripEnd>>respin2_ht2",draw.Data()),Form("%s && rcdb_flip_state == %d && ErrorFlag==0 && SinceLastTripEnd<=250",cut.Data(),flip_state),"prof");
  TProfile* respin2_ht2 = (TProfile*)gROOT->FindObject("respin2_ht2");
  respin2_ht2->SetNameTitle("Respin2 Prof",Form("Respin2, new %s data, all CREX wien state %d, post trip, prof;SinceLastTripEnd (multiplets);%s",name.Data(),flip_state,name.Data()));
  respin2_ht2->Rebin(4);
  respin2_ht2->Draw();


  mul_trip = new TChain(treename);
  mul_trip->SetCacheSize(20*1024*1024);
  mul_trip->Add("respin1_rootfiles/test.root");
  mul_trip->Add("respin1_rootfiles/test_1.root");
  mul_trip->Add("respin1_rootfiles/test_2.root");
  mul_trip->LoadTree(-1);
  c1->cd(3);
  mul_trip->Draw(Form("rcdb_sign*%s:SinceLastTripEnd>>respin1_ht1",draw.Data()),Form("%s && rcdb_flip_state == %d && ErrorFlag==0 && SinceLastTripEnd<=250",cut.Data(),flip_state),"goff");
  TH2* respin1_ht1 = (TH2*)gROOT->FindObject("respin1_ht1");
  respin1_ht1->SetNameTitle("Respin1 Candle",Form("Respin1, new %s data, all CREX wien state %d, post trip, candle;SinceLastTripEnd (multiplets);%s",name.Data(),flip_state,name.Data()));
  respin1_ht1->Draw("candlex3");
  c1->cd(4);
  mul_trip->Draw(Form("rcdb_sign*%s:SinceLastTripEnd>>respin1_ht2",draw.Data()),Form("%s && rcdb_flip_state == %d && ErrorFlag==0 && SinceLastTripEnd<=250",cut.Data(),flip_state),"prof");
  TProfile* respin1_ht2 = (TProfile*)gROOT->FindObject("respin1_ht2");
  respin1_ht2->SetNameTitle("Respin1 Prof",Form("Respin1, new %s data, all CREX wien state %d, post trip, prof;SinceLastTripEnd (multiplets);%s",name.Data(),flip_state,name.Data()));
  respin1_ht2->Rebin(4);
  respin1_ht2->Draw();



  /*
  TFile *_file1 = TFile::Open("prompt_rootfiles/test.root");
  _file1->cd();
  _file1->GetObject("mul_trip", mul_trip);
  */
  mul_trip = new TChain(treename);
  mul_trip->SetCacheSize(20*1024*1024);
  mul_trip->Add("prompt_rootfiles/test.root");
  mul_trip->Add("prompt_rootfiles/test_1.root");
  mul_trip->Add("prompt_rootfiles/test_2.root");
  mul_trip->LoadTree(-1);
  c1->cd(5);
  mul_trip->Draw(Form("rcdb_sign*%s:-1.0*UntilNextTripEnd>>prompt_ht1",draw.Data()),Form("%s && rcdb_flip_state == %d && NonTripErrorFlag==0 && UntilNextTripEnd<=250",cut.Data(),flip_state),"goff");
  TH2* prompt_ht1 = (TH2*)gROOT->FindObject("prompt_ht1");
  prompt_ht1->SetNameTitle("Prompt Candle",Form("Prompt, new %s data, all CREX wien state %d, post trip, candle;UntilNextTripEnd (multiplets);%s",name.Data(),flip_state,name.Data()));
  prompt_ht1->Draw("candlex3");
  c1->cd(6);
  mul_trip->Draw(Form("rcdb_sign*%s:-1.0*UntilNextTripEnd>>prompt_ht2",draw.Data()),Form("%s && rcdb_flip_state == %d && NonTripErrorFlag==0 && UntilNextTripEnd<=250",cut.Data(),flip_state),"prof");
  TProfile* prompt_ht2 = (TProfile*)gROOT->FindObject("prompt_ht2");
  prompt_ht2->SetNameTitle("Prompt Prof",Form("Prompt, new %s data, all CREX wien state %d, post trip, prof;UntilNextTripEnd (multiplets);%s",name.Data(),flip_state,name.Data()));
  prompt_ht2->Rebin(4);
  prompt_ht2->Draw();
  c1->SaveAs(Form("plots/prof_CREX_wienstate%d_signed_%s.pdf",flip_state,title.Data()));
}

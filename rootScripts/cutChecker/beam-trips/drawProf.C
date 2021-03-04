void drawProf(Int_t flip_state = 1) {
  TFile *_file0 = TFile::Open("respin1_rootfiles/test.root");
  TTree *mul_trip;
  _file0->GetObject("mul_trip", mul_trip);
  TCanvas * c1 = new TCanvas();
  c1->Divide(2,2);
  c1->cd(1);
  mul_trip->Draw(Form("rcdb_sign*asym_bcm_an_us.hw_sum/1e-9:SinceLastTripEnd>>respin1_ht1"),Form("rcdb_flip_state == %d && ErrorFlag==0 && SinceLastTripEnd<=250",flip_state),"goff");
  TH2* respin1_ht1 = (TH2*)gROOT->FindObject("respin1_ht1");
  respin1_ht1->SetNameTitle("Respin1 Candle",Form("Respin1, new Aq data, all CREX wien state %d, post trip, candle;SinceLastTripEnd (multiplets);Aq (ppb)",flip_state));
  respin1_ht1->Draw("candlex3");
  c1->cd(2);
  mul_trip->Draw("rcdb_sign*asym_bcm_an_us.hw_sum/1e-9:SinceLastTripEnd>>respin1_ht2",Form("rcdb_flip_state == %d && ErrorFlag==0 && SinceLastTripEnd<=250",flip_state),"prof");
  TProfile* respin1_ht2 = (TProfile*)gROOT->FindObject("respin1_ht2");
  respin1_ht2->SetNameTitle("Respin1 Prof",Form("Respin1, new Aq data, all CREX wien state %d, post trip, prof;SinceLastTripEnd (multiplets);Aq (ppb)",flip_state));
  respin1_ht2->Rebin(4);
  respin1_ht2->Draw();



  TFile *_file1 = TFile::Open("prompt_rootfiles/test.root");
  _file1->cd();
  _file1->GetObject("mul_trip", mul_trip);
  c1->cd(3);
  mul_trip->Draw(Form("rcdb_sign*asym_bcm_an_us.hw_sum/1e-9:-1.0*UntilNextTripEnd>>prompt_ht1"),Form("rcdb_flip_state == %d && NonTripErrorFlag==0 && UntilNextTripEnd<=250",flip_state),"goff");
  TH2* prompt_ht1 = (TH2*)gROOT->FindObject("prompt_ht1");
  prompt_ht1->SetNameTitle("Prompt Candle",Form("Prompt, new Aq data, all CREX wien state %d, post trip, candle;UntilNextTripEnd (multiplets);Aq (ppb)",flip_state));
  prompt_ht1->Draw("candlex3");
  c1->cd(4);
  mul_trip->Draw("rcdb_sign*asym_bcm_an_us.hw_sum/1e-9:-1.0*UntilNextTripEnd>>prompt_ht2",Form("rcdb_flip_state == %d && NonTripErrorFlag==0 && UntilNextTripEnd<=250",flip_state),"prof");
  TProfile* prompt_ht2 = (TProfile*)gROOT->FindObject("prompt_ht2");
  prompt_ht2->SetNameTitle("Prompt Prof",Form("Prompt, new Aq data, all CREX wien state %d, post trip, prof;UntilNextTripEnd (multiplets);Aq (ppb)",flip_state));
  prompt_ht2->Rebin(4);
  prompt_ht2->Draw();
  c1->SaveAs(Form("plots/profAq_CREX_wienstate%d.pdf",flip_state));
}

void checkOutlierRun(Int_t run = 6564,Int_t event = 0) {

  TChain* evt = new TChain("evt");
  TChain* mul = new TChain("mul");
  TChain* mulc_lrb_alldet_burst = new TChain("mulc_lrb_alldet_burst");

  evt->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/japanOutput/prexPrompt_pass2_%d.000.root",run));
  mul->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/japanOutput/prexPrompt_pass2_%d.000.root",run));
  mulc_lrb_alldet_burst->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/japanOutput/prexPrompt_pass2_%d.000.root",run));

  mul->AddFriend(mulc_lrb_alldet_burst);


  TCanvas * c1 = new TCanvas();
  c1->Divide(3,2);

  Int_t span = 10000;
  if (event == -1) {
    span = 500000;
  }
  c1->cd(1);
  evt->Draw("bpm4eX:CodaEventNumber",Form("(ErrorFlag&0xda7e6bff)==0 && (CodaEventNumber>%d - %d) && (CodaEventNumber<%d + %d)",event,span,event,span),"");
  
  c1->cd(2);
  evt->Draw("bpm4eY:CodaEventNumber",Form("(ErrorFlag&0xda7e6bff)==0 && (CodaEventNumber>%d - %d) && (CodaEventNumber<%d + %d)",event,span,event,span),"");
  
  c1->cd(3);
  evt->Draw("bpm12X:CodaEventNumber",Form("(ErrorFlag&0xda7e6bff)==0 && (CodaEventNumber>%d - %d) && (CodaEventNumber<%d + %d)",event,span,event,span),"");

  c1->cd(4);
  evt->Draw("usl:CodaEventNumber",Form("(ErrorFlag&0xda7e6bff)==0 && (CodaEventNumber>%d - %d) && (CodaEventNumber<%d + %d)",event,span,event,span),"");

  c1->cd(5);
  evt->Draw("usr:CodaEventNumber",Form("(ErrorFlag&0xda7e6bff)==0 && (CodaEventNumber>%d - %d) && (CodaEventNumber<%d + %d)",event,span,event,span),"");

  c1->cd(6);
  mul->Draw("mulc_lrb_alldet_burst.cor_asym_us_avg:CodaEventNumber",Form("(ErrorFlag&0xda7e6bff)==0 && (CodaEventNumber>%d - %d) && (CodaEventNumber<%d + %d)",event,span,event,span),"");

  Short_t mini = 0;
  mul->SetBranchAddress("BurstCounter",&mini);
  mul->BuildIndex("CodaEventNumber");
  mul->GetEntry(mul->GetEntryNumberWithIndex(event));

  c1->SaveAs(Form("Mul_Outlier_checks/Check_Run_%d_mini_%d_Event_%d.pdf",run,mini,event));

}

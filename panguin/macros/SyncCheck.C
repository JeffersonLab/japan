void SyncCheck(){
  TPad *a1 = new TPad("a1", "a1", 0,0,1,1);
  a1->Draw();

  const int Nstats = 6;
  TString statStr[Nstats];
  TString statStrNumbers[Nstats];

  TTree* r = (TTree*)gROOT->FindObject("evt");
  TH1D* hist = new TH1D("CodaEventNumber", "CodaEventNumber", 100, 0, 10e9);
 
  r->Project(hist->GetName(), "CodaEventNumber", "1");
  Int_t tot = hist->GetEntries();
  statStr[0] = Form("Total Number of events");
  statStrNumbers[0] = Form(" = %d", tot);

  r->Project(hist->GetName(), "CodaEventNumber", "(lhrs_flexio_input&0x1)-reported_helicity != 0 && reported_helicity>=-1");
  Int_t l_heli = hist->GetEntries();
  Double_t per_l_heli= l_heli*100./tot;
  statStr[1] = Form("LHRS helicity mismatches");
  statStrNumbers[1] = Form(" = %d (%.2f%%)", l_heli, per_l_heli);

  r->Project(hist->GetName(), "CodaEventNumber", "((rhrs_flexio_input&0x40)==0)-(reported_helicity==1) != 0 && reported_helicity>=-1");
  Int_t r_heli = hist->GetEntries();
  Double_t per_r_heli= r_heli*100./tot;
  statStr[2] = Form("RHRS helicity mismatches");
  statStrNumbers[2] = Form(" = %d (%.2f%%)", r_heli, per_r_heli);

  r->Project(hist->GetName(), "CodaEventNumber", "((rhrs_flexio_input&0x20)==0)-(pattern_phase==1) != 0");
  Int_t inj_sync = hist->GetEntries();
  Double_t per_inj_sync= inj_sync*100./tot;
  statStr[3] = Form("Inj loss sync events");
  statStrNumbers[3] = Form(" = %d (%.2f%%)", inj_sync, per_inj_sync);

  r->Project(hist->GetName(), "CodaEventNumber", "((ch_flexio_input&0x20)==0x20)-(pattern_phase==1) != 0");
  Int_t ch_sync = hist->GetEntries();
  Double_t per_ch_sync= ch_sync*100./tot;
  statStr[4] = Form("CH loss sync events");
  statStrNumbers[4] = Form(" = %d (%.2f%%)", ch_sync, per_ch_sync);

  Int_t sum = l_heli + r_heli + inj_sync + ch_sync;
  Double_t persum= sum*100./tot;
  statStr[5] = Form("Total loss sync events");
  statStrNumbers[5] = Form(" = %d (%.2f%%)", sum, persum);

  TLatex text;
  text.SetTextSize(0.08);
  a1->cd(); 
  Float_t iniY = 0.90, diffY = 0.05;
  text.DrawLatex(0.10, 0.93, "Sync Stats, Full Run"); 
  text.SetTextSize(0.06);
  for (Int_t j = 0 ; j < Nstats ; j++){
    text.DrawLatex(0.10, iniY - (j*2.5+1)*diffY, statStr[j]); 
    text.DrawLatex(0.10, iniY - (j*2.5+2)*diffY, statStrNumbers[j]); 
  }
  if(hist) delete hist;
}

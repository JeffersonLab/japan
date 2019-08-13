void GetCountCutStats(Int_t nUserEvents = 0){
  TPad *a1 = new TPad("a1", "a1", 0,0,1,1);
  a1->Draw();

  const int Nstats = 8;
  TString statStr[Nstats];
  TString statStrNumbers[Nstats];
  Int_t cut[Nstats];
  Double_t percut[Nstats];

  TTree* r = (TTree*)gROOT->FindObject("evt");
  TH1D* hist = new TH1D("CodaEventNumber", "CodaEventNumber", 100, 0, 10e9);
 
  statStr[0] = Form("Number of Previous Events Checked");
  r->Project(hist->GetName(), "CodaEventNumber", "1");
  Int_t tot = hist->GetEntries();
  Int_t nEvents = 0;
  if (nUserEvents == 0) {
    nEvents = tot;
  }
  else {
    nEvents = nUserEvents;
  }
  r->Project(hist->GetName(), "CodaEventNumber", Form("Entry$>=(Entries$-%d)",nEvents));
  tot = hist->GetEntries();
  statStrNumbers[0] = Form(" = %d", tot);

  statStr[1] = Form("EventCut Failing events");
  r->Project(hist->GetName(), "CodaEventNumber", Form("ErrorFlag!=0 && Entry$>=(Entries$-%d)",nEvents));
  cut[1] = hist->GetEntries();
  percut[1]= cut[1]*100./tot;
  statStrNumbers[1] = Form(" = %d (%.2f%%)", cut[1], percut[1]);

  statStr[2] = Form("Number of Missed Helicity Triggers");
  r->Project(hist->GetName(), "CodaEventNumber", Form("ch_clock_4mhz<500 && Entry$>=(Entries$-%d)",nEvents));
  cut[2] = hist->GetEntries();
  percut[2]= cut[2]*100./tot;
  statStrNumbers[2] = Form(" = %d (%.2f%%)", cut[2], percut[2]);

  statStr[3] = Form("Detector Saturation Failing Events");
  r->Project(hist->GetName(), "CodaEventNumber", Form("(usl.Device_Error_Code&0x800)!=0 || (dsl.Device_Error_Code&0x800)!=0 || (usr.Device_Error_Code&0x800)!=0 || (dsr.Device_Error_Code&0x800)!=0 && Entry$>=(Entries$-%d)",nEvents));
  cut[3] = hist->GetEntries();
  percut[3]= cut[3]*100./tot;
  statStrNumbers[3] = Form(" = %d (%.2f%%)", cut[3], percut[3]);

  statStr[4] = Form("Beam Trip Failing Events");
  r->Project(hist->GetName(), "CodaEventNumber", Form("(ErrorFlag&0x8000000)!=0 && Entry$>=(Entries$-%d)",nEvents));
  cut[4] = hist->GetEntries();
  percut[4]= cut[4]*100./tot;
  statStrNumbers[4] = Form(" = %d (%.2f%%)", cut[4], percut[4]);

  statStr[5] = Form("Beam Stability cut Failing Events");
  r->Project(hist->GetName(), "CodaEventNumber", Form("(ErrorFlag&0x1000000)!=0 && Entry$>=(Entries$-%d)",nEvents));
  cut[5] = hist->GetEntries();
  percut[5]= cut[5]*100./tot;
  statStrNumbers[5] = Form(" = %d (%.2f%%)", cut[5], percut[5]);

  statStr[6] = Form("Stability Cut Failing Events (beam + others)");
  r->Project(hist->GetName(), "CodaEventNumber", Form("(ErrorFlag&0x1000000)!=0 && Entry$>=(Entries$-%d)",nEvents));
  cut[6] = hist->GetEntries();
  percut[6]= cut[6]*100./tot;
  statStrNumbers[6] = Form(" = %d (%.2f%%)", cut[6], percut[6]);

  statStr[7] = Form("BPM Cut Failing Events");
  r->Project(hist->GetName(), "CodaEventNumber", Form("(ErrorFlag&0x400)!=0 && Entry$>=(Entries$-%d)",nEvents));
  cut[7] = hist->GetEntries();
  percut[7]= cut[7]*100./tot;
  statStrNumbers[7] = Form(" = %d (%.2f%%)", cut[7], percut[7]);

  TLatex text;
  text.SetTextSize(0.08);
  a1->cd(); 
  Float_t iniY = 0.90, diffY = 0.045;
  if (nUserEvents == 0) {
    text.DrawLatex(0.10, 0.93, Form("Total Run: %d events",nEvents)); 
  }
  else {
    text.DrawLatex(0.10, 0.93, Form("Event Stats, %d events",nEvents)); 
  }
  text.SetTextSize(0.05);
  for (Int_t j = 0 ; j < Nstats ; j++){
    text.DrawLatex(0.10, iniY - (j*2.5+1)*diffY, statStr[j]); 
    text.DrawLatex(0.10, iniY - (j*2.5+2)*diffY, statStrNumbers[j]); 
  }
  if(hist) delete hist;
}


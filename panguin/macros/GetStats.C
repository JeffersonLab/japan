void GetStats(){
  TPad *a1 = new TPad("a1", "a1", 0,0,1,1);
  a1->Draw();

  const int Nstats = 6;
  TString statStr[Nstats];
  TString statStrNumbers[Nstats];

  TTree* r = (TTree*)gROOT->FindObject("evt");
  TH1D* hist = new TH1D("CodaEventNumber", "CodaEventNumber", 100, 0, 10e9);
 
  r->Project(hist->GetName(), "CodaEventNumber", "");
  Int_t tot = hist->GetEntries();
  statStr[0] = Form("Total Number of events");
  statStrNumbers[0] = Form(" = %d", tot);

  r->Project(hist->GetName(), "CodaEventNumber", "ErrorFlag==0");
  Int_t ok = hist->GetEntries();
  Double_t perok= ok*100./tot;
  statStr[1] = Form("EventCut passing events");
  statStrNumbers[1] = Form(" = %d (%.2f%%)", ok, perok);

  r->Project(hist->GetName(), "CodaEventNumber", "bcm_dg_ds<60");
  Int_t low = hist->GetEntries();
  Double_t perlow= low*100./tot;
  statStr[2] = Form("Current < 60 #muA events");
  statStrNumbers[2] = Form(" = %d (%.2f%%)", low, perlow);

  r->Project(hist->GetName(), "CodaEventNumber", "bpm11XP>50000 || bpm11XM>50000 || bpm11YP>50000 || bpm11YM>50000");
  Int_t bpmsat = hist->GetEntries();
  Double_t perbpmsat= bpmsat*100./tot;
  statStr[3] = Form("BPM11 wire saturation events");
  statStrNumbers[3] = Form(" = %d (%.2f%%)", bpmsat, perbpmsat);

  r->Project(hist->GetName(), "cav4cQ.hw_sum", "ErrorFlag==0");
  Int_t nent = hist->GetEntries();
  Double_t avgCurrent = hist->GetMean();
  Double_t totalCharge = avgCurrent*(1/1.0e6)*(nent/240);
  statStr[4] = Form("Total acc Q this run (1.2C = slug)");
  statStrNumbers[4] = Form(" = %.2f C", totalCharge);

  r->Project(hist->GetName(), "CodaEventNumber", "");
  Double_t goodTime = ((nent/240.0)/60.0);
  Double_t totalTime = ((hist->GetEntries()/240.0)/60.0);
  statStr[5] = Form("Good beam time / total time");
  statStrNumbers[5] = Form(" = %.2f ABU minutes / %0.2f mins ",goodTime,totalTime);

  TLatex text;
  text.SetTextSize(0.08);
  a1->cd(); 
  Float_t iniY = 0.90, diffY = 0.05;
  text.DrawLatex(0.10, 0.93, "Event Stats, Full Run"); 
  text.SetTextSize(0.06);
  for (Int_t j = 0 ; j < Nstats ; j++){
    text.DrawLatex(0.10, iniY - (j*2.5+1)*diffY, statStr[j]); 
    text.DrawLatex(0.10, iniY - (j*2.5+2)*diffY, statStrNumbers[j]); 
  }
}

#include "/adaqfs/home/apar/PREX/japan/rootScripts/Open.C"
void GetStatsManually(Int_t runNum = 999999){
  const int Nstats = 6;
  TString statStr[Nstats];
  TString statStrNumbers[Nstats];

  if (runNum == 999999) {
    Open(runNum,"",-1);
  }
  else {
    Open(runNum);
  }
  TTree* r = (TTree*)gROOT->FindObject("evt");
  TString string = "";
  if (!r) {
    string = "No Root file for this run";
    gSystem->Exec(Form("echo \"%s\" > /adaqfs/home/apar/scripts/stats.dat",string.Data()));
    return;
  }
  else {
    TH1D* hist = new TH1D("CodaEventNumber", "CodaEventNumber", 100, 0, 10e9);

    r->Project(hist->GetName(), "CodaEventNumber", "", "goff");
    Int_t tot = hist->GetEntries();
    statStr[0] = Form("Total Number of events");
    statStrNumbers[0] = Form(" = %d", tot);

    r->Project(hist->GetName(), "CodaEventNumber", "ErrorFlag==0||(ErrorFlag & 0xda7e6bff)==0","goff");
    Int_t ok = hist->GetEntries();
    Double_t perok= ok*100./tot;
    statStr[1] = Form("EventCut passing events");
    statStrNumbers[1] = Form(" = %d (%.2f%%)", ok, perok);

    r->Project(hist->GetName(), "CodaEventNumber", "bcm_dg_ds<50","goff");
    Int_t low = hist->GetEntries();
    Double_t perlow= low*100./tot;
    statStr[2] = Form("Current < 50 #muA events");
    statStrNumbers[2] = Form(" = %d (%.2f%%)", low, perlow);

    r->Project(hist->GetName(), "CodaEventNumber", "bpm12XP>50000 || bpm12XM>50000 || bpm12YP>50000 || bpm12YM>50000","goff");
    Int_t bpmsat = hist->GetEntries();
    Double_t perbpmsat= bpmsat*100./tot;
    statStr[3] = Form("BPM12 wire saturation events");
    statStrNumbers[3] = Form(" = %d (%.2f%%)", bpmsat, perbpmsat);

    r->Project(hist->GetName(), "bcm_an_us.hw_sum", "bcm_an_us>0.1");
    Int_t nentFULL = hist->GetEntries();
    Double_t avgCurrentFULL = hist->GetMean();
    Double_t totalChargeFULL = avgCurrentFULL*(1/1.0e6)*(nentFULL/120);
    r->Project(hist->GetName(), "bcm_an_us.hw_sum", "ErrorFlag==0||(ErrorFlag & 0xda7e6bff)==0","goff");
    Int_t nent = hist->GetEntries();
    Double_t avgCurrent = hist->GetMean();
    Double_t totalCharge = avgCurrent*(1/1.0e6)*(nent/120);
    statStr[4] = Form("Total Good Q this run (2C = slug)");
    statStrNumbers[4] = Form(" = %.3f C ABU / %.3f C no cuts", totalCharge,totalChargeFULL);

    r->Project(hist->GetName(), "CodaEventNumber", "","goff");
    Double_t goodTime = ((nent/120.0)/60.0);
    Double_t totalTime = ((hist->GetEntries()/120.0)/60.0);
    statStr[5] = Form("Good beam time / total time");
    statStrNumbers[5] = Form(" = %.2f ABU minutes / %0.2f mins ",goodTime,totalTime);

    //if (totalTime > 35.0) {
    //  gSystem->Exec("python3 /adaqfs/home/apar/alarms/AlarmHandlerGUI/alert.py -a 6");
    //  Printf("The run has laster 35 minutes, please start a new one to avoid CODA file splits\n");
    //}

    for (Int_t j = 0 ; j < Nstats ; j++){
      string = string + "\n\t" + statStr[j] + statStrNumbers[j];
    }
    gSystem->Exec(Form("echo \"%s\" > /adaqfs/home/apar/scripts/stats.dat",string.Data()));
    gSystem->Exec(Form("echo \"%s\" > /adaqfs/home/apar/scripts/stats/stats_%d.dat",string.Data(),runNum));
    if(hist) delete hist;
  }
}

////////realtimeBmodAna.C

void realtimeBmodAna() {
  gStyle->SetOptStat(1221);
  TTree* tree_R = (TTree*)gDirectory->Get("evt_bmw");
  int events = tree_R->GetEntries();
//  tree_R->Draw(">>elist1","beam_mod_ramp>0 && bmwobj>0","eventlist");
//  TEventList *elist1 = (TEventList*)gDirectory->Get("elist1");
//  tree_R->SetEventList(elist1);

  //tree_R->Draw(">>elist1","beam_mod_ramp>0 && bmwobj>0","entrylist");
  //TEntryList *elist1 = (TEntryList*)gDirectory->Get("elist1");
  //tree_R->SetEntryList(elist1);
  
  int nonzero = tree_R->Draw("bmwcycnum","beam_mod_ramp>0 && bmwobj>0","goff");
  //if (nonzero>50) { //then we have enough data to do a full cycle analysis most likely
  TLeaf *l_bmwcycnum = tree_R->GetLeaf("bmwcycnum");
  l_bmwcycnum->GetBranch()->GetEntry(nonzero-1);
  Double_t currentCyclenum = 0.0;
  currentCyclenum = (Double_t)l_bmwcycnum->GetValue();
  //for (Int_t i=0;i<nonzero;i++){
  //  l_bmwcycnum->GetBranch()->GetEntry(i);
  //  if ((Double_t)l_bmwcycnum->GetValue()>0){
  //    currentCyclenum = (Double_t)l_bmwcycnum->GetValue();
  //  }
  //}
  TString s = gSystem->GetFromPipe("cat /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/last_cycle_num.txt");
  Double_t lastCyclenum = s.Atof();
  Printf("currentCyclenum = %f, lastCyclenum = %f, %d Total entries, %d entries with BMW active",currentCyclenum,lastCyclenum,events,nonzero);
  TString runFile = gDirectory->GetName();
  TString runNum = "0";
  if (runFile.Contains("999999")){
    Printf("Preparing to do online rootfile BMOD analysis");
    runNum = "999999";
  }
  else {
    runNum = runFile(runFile.Length()-13,4);
    Printf("Preparing to do run %s BMOD analysis",runNum.Data());
  }
  if ((lastCyclenum < currentCyclenum-1)){ 
    // The goal of this if condition is to prevent the analysis from running once every 10 seconds in panguin. Ideally we just check if the file was editted in the last 5 minutes. If no then analyze
    Printf("Running BMOD Analysis from inside PANGUIN on realtime root file");
    gSystem->Exec(Form("echo %f > /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/last_cycle_num.txt",currentCyclenum));
    gSystem->Exec(Form("/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/onlineRunAvgBmodAna_1X.sh"));
  }
  else if (runNum!="999999"){
    // The goal of this if condition is to prevent the analysis from running once every 10 seconds in panguin. Ideally we just check if the file was editted in the last 5 minutes. If no then analyze
    Printf("Running BMOD Analysis from inside PANGUIN");
    gSystem->Exec(Form("echo %f > /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/last_cycle_num.txt",currentCyclenum));
    gSystem->Exec(Form("/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/onlineRunAvgBmodAna_1X.sh %s",runNum.Data()));
  }
  else {
    Printf("No fresh cycle, wait for more data");
  }
 // }
 // else {
 //   Printf("Not enough data in this cycle to analyze, wait for more data");
 // }
}

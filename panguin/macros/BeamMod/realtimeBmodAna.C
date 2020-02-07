////////realtimeBmodAna.C
///Panguin gui compatible diagnostic plots for shift taking online replay 

void realtimeBmodAna() {
  gStyle->SetOptStat(1221);
  TTree* tree_R = (TTree*)gDirectory->Get("evt");
  int events = tree_R->GetEntries();
  tree_R->Draw(">>elist1","","entrylist");
  TEntryList *elist1 = (TEntryList*)gDirectory->Get("elist1");
  //tree_R->Draw(">>elist2","bmwcycnum>0","entrylist");
  tree_R->Draw(">>elist2","beam_mod_ramp>0 && bmwobj==7","entrylist");
  TEntryList *elist2 = (TEntryList*)gDirectory->Get("elist2");

  tree_R->SetEntryList(elist1);
  TLeaf *l_CEN = tree_R->GetLeaf("CodaEventNumber");
  l_CEN->GetBranch()->GetEntry(events-1);
  Double_t currentCEN = (Double_t)l_CEN->GetValue();

  tree_R->SetEntryList(elist2);
  TLeaf *l_bmwcycnum = tree_R->GetLeaf("bmwcycnum");
  TLeaf *l_bmwCEN = tree_R->GetLeaf("CodaEventNumber");
  int nonzero = tree_R->Draw("bmwcycnum","beam_mod_ramp>0 && bmwobj==7","goff");
  //if (nonzero>50) { //then we have enough data to do a full cycle analysis most likely
    l_bmwcycnum->GetBranch()->GetEntry(nonzero-1);
    l_bmwCEN->GetBranch()->GetEntry(nonzero-1);
    Double_t currentCyclenum = 0.0;
    for (Int_t i=0;i<nonzero;i++){
      l_bmwcycnum->GetBranch()->GetEntry(i);
      if ((Double_t)l_bmwcycnum->GetValue()>0){
        currentCyclenum = (Double_t)l_bmwcycnum->GetValue();
      }
    }
    Double_t lastCEN = (Double_t)l_bmwCEN->GetValue();
    TString s = gSystem->GetFromPipe("cat /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/last_cycle_num.txt");
    Double_t lastCyclenum = s.Atof();
    Printf("currentCyclenum = %f, lastCEN = %f, lastCyclenum = %f, currentCEN = %f, %d Total entries, %d entries with BMW active",currentCyclenum,lastCEN,lastCyclenum,currentCEN,events,nonzero);
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
    //if (runNum!="999999" || ((lastCyclenum < currentCyclenum) && (currentCEN > 15*120 + lastCEN))) 
    if (runNum!="999999" || ((lastCyclenum < currentCyclenum) && (currentCEN > -1*120 + lastCEN))) { // Give it a -second to start analysis... it's in evt_bmw tree, so it needs exactly the right number of events to work
      //FIXME If you get problems with GOOD cycles being ignore by this script, then uncomment the line below and remove the 15 second time check condition... it should be fine
      //if (runNum!="999999" || ((lastCyclenum < currentCyclenum)))  

      // The goal of this if condition is to prevent the analysis from running once every 10 seconds in panguin. Ideally we just check if the file was editted in the last 5 minutes. If no then analyze
      Printf("Running BMOD Analysis from inside PANGUIN");
      gSystem->Exec(Form("echo %f > /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/last_cycle_num.txt",currentCyclenum));
      gSystem->Exec(Form("/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/onlineRunAvgBmodAna.sh %s",runNum.Data()));
    }
    else {
      Printf("No fresh cycle, wait for more data");
    }
 // }
 // else {
 //   Printf("Not enough data in this cycle to analyze, wait for more data");
 // }
}

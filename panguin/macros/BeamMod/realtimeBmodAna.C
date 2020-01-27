////////realtimeBmodAna.C
///Panguin gui compatible diagnostic plots for shift taking online replay 

// #include "plotAD.C"
void realtimeBmodAna() {
  gStyle->SetOptStat(1221);
//plotAD(999999,1357642);
  TTree* tree_R = (TTree*)gDirectory->Get("evt");
  int events = tree_R->GetEntries();
  tree_R->Draw(">>elist1","","entrylist");  //picks out unique cycle numbers
  TEntryList *elist1 = (TEntryList*)gDirectory->Get("elist1");
  tree_R->Draw(">>elist2","bmwcycnum>0","entrylist");  //picks out unique cycle numbers
  TEntryList *elist2 = (TEntryList*)gDirectory->Get("elist2");

  tree_R->SetEntryList(elist1);
  TLeaf *l_CEN = tree_R->GetLeaf("CodaEventNumber");
  l_CEN->GetBranch()->GetEntry(events-1);
  Double_t currentCEN = (Double_t)l_CEN->GetValue();

  tree_R->SetEntryList(elist2);
  TLeaf *l_bmwcycnum = tree_R->GetLeaf("bmwcycnum");
  TLeaf *l_bmwCEN = tree_R->GetLeaf("CodaEventNumber");
  int nonzero = tree_R->Draw("bmwcycnum","bmwcycnum>1","goff");
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
  if ((lastCyclenum < currentCyclenum) && (currentCEN > 15*120 + lastCEN)) {
    // FIXME the goal of this if condition is to prevent the analysis from running once every 10 seconds in panguin. Ideally we just check if the file was editted in the last 5 minutes. If no then analyze
    Printf("Running BMOD Analysis from inside PANGUIN");
    gSystem->Exec(Form("echo %d > /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/last_cycle_num.txt",(Int_t)currentCyclenum));
    gSystem->Exec(Form("/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/onlineBmodAna.sh"));
  }
}
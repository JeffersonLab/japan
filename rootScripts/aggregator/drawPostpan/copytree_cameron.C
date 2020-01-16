void copytree_cameron(TString oldFileName = "grand_aggregator.root" , int entry = -1, int goal = 1)
{
     // Get old file, old tree and set top branch address
     if(entry==-1){
       cout << "No entry passed with macro call!" << endl;
       return;
     }
     cout << "ignoring entry" << entry << endl;
     TFile oldfile(oldFileName);
     TTree *oldtree;
     oldfile.GetObject("agg", oldtree);
     const auto nentries = oldtree->GetEntries();
     
     TFile newfile("grand_prototype.root", "recreate");
     auto newtree = oldtree->CloneTree(0);
     if (goal == 1){
       for (auto i : ROOT::TSeqI(nentries)) {
         if (i==entry) continue;
         oldtree->GetEntry(i);
         newtree->Fill();
       }
     }
     else if (goal == 2) {
       // Move final entry to beginning
       oldtree->GetEntry(nentries-1);
       newtree->Fill();
       for (auto i : ROOT::TSeqI(nentries-1)) {
         oldtree->GetEntry(i);
         newtree->Fill();
       }
     }
     newtree->Write("agg",TObject::kOverwrite);
     newfile.Close();
 }                                                                           

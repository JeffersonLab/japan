void copytree()
{
     // Get old file, old tree and set top branch address
     
     TFile oldfile("/chafs2/work1/apar/aggRootfiles/slugRootfiles/grandRootfile/grand_aggregator.root");
     TTree *oldtree;
     oldfile.GetObject("agg", oldtree);
     const auto nentries = oldtree->GetEntries();
     
     TFile newfile("plots/grand_prototype.root", "recreate");
     auto newtree = oldtree->CloneTree(0);
     for (auto i : ROOT::TSeqI(nentries)) {
      if (i>=43-27) continue;
     	oldtree->GetEntry(i);
      newtree->Fill();
     }
     newtree->Write("agg",TObject::kOverwrite);
     newfile.Close();
 }                                                                           

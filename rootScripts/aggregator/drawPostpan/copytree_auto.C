void copytree_auto(int slug = -1)
{
     // Get old file, old tree and set top branch address
     if(slug==-1){
       cout << "No slug passed with macro call!" << endl;
       return;
     }
     cout << "ignoring slug" << slug << endl;
     slug=slug-27;
     TFile oldfile("/chafs2/work1/apar/aggRootfiles/slugRootfiles/grandRootfile/grand_aggregator.root");
     TTree *oldtree;
     oldfile.GetObject("agg", oldtree);
     const auto nentries = oldtree->GetEntries();
     
     TFile newfile("plots/grand_prototype.root", "recreate");
     auto newtree = oldtree->CloneTree(0);
     for (auto i : ROOT::TSeqI(nentries)) {
      if (i==slug) continue;
     	oldtree->GetEntry(i);
      newtree->Fill();
     }
     newtree->Write("agg",TObject::kOverwrite);
     newfile.Close();
 }                                                                           

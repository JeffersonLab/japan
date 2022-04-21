/*
author: Cameron Clarke
last update: February 2021

*/

void Add_crex_segment_eig(TString alt_ana = ""){ //(int start, int end)
  // FIXME replace with read-from-dit tree
  //TFile eig_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_AT.root"));
  TFile eig_input(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles%s//rcdb_segment_eigenvectors_sorted.root",alt_ana.Data()));
  TFile output(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles%s//rcdb_segment_parts_eigenvectors_sorted.root",alt_ana.Data()),"recreate");
  //TFile output(Form("rootfiles/rcdb_AT_eigenvectors.root"),"recreate");

  TTree *mini_tree;
  //TTree *eig_reg_tree_5_tr;
  eig_input.GetObject("mini", mini_tree);

  TChain* out_mini_tree = (TChain*)mini_tree -> CloneTree(0);

  Int_t run, burst_counter;
  mini_tree->SetBranchAddress("run",&run);
  mini_tree->SetBranchAddress("mini",&burst_counter);

  std::string index1 = "run";
  mini_tree->BuildIndex("run","mini");
  TTreeIndex *mini_indexedTree = (TTreeIndex *) mini_tree->GetTreeIndex();

  Long64_t nEntries = mini_tree->GetEntries();
  Double_t current_part = 0;
  out_mini_tree->Branch("crex_part",&current_part);

  for(int ievt=0;ievt<nEntries;ievt++){
    mini_tree -> GetEntry(ievt);
    if (run < 6328) {
      current_part = 1.0;
    }
    if (run >= 6328 && run < 7500) {
      current_part = 2.0;
    }
    if (run >= 7500) {
      current_part = 3.0;
    }
    out_mini_tree -> Fill();
  }

  out_mini_tree -> Write("mini",TObject::kOverwrite);

  TKey *key;
  TIter nextkey(eig_input.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)eig_input.Get(key->GetName());
      if ((TString)key->GetName() == "mini") {
        // Skip the tree we've been working with
        continue;
      }
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!output.FindObject(key->GetName())) {
        output.cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    }
  }
  eig_input.Close();

  output.Close();
}

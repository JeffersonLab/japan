/*
author: Cameron Clarke
last update: March 16 2021

*/

void Add_crex_segment_dit(TString ana = "_eigenvectors_sorted_part_avg"){ //(int start, int end)
  // FIXME replace with read-from-dit tree
  //TFile dit_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_AT.root"));
  TFile dit_input(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_run_avg%s_1X/dithering_slopes_13746%s_rcdb_runwise.root",ana.Data(),ana.Data()));
  TFile output(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_run_avg%s_1X/dithering_slopes_13746%s_rcdb_parts_runwise.root",ana.Data(),ana.Data()),"recreate");
  //TFile output(Form("rootfiles/rcdb_AT_eigenvectors.root"),"recreate");

  TTree *dit_tree;
  dit_input.GetObject("dit", dit_tree);

  TChain* out_dit_tree = (TChain*)dit_tree -> CloneTree(0);

  Int_t run, burst_counter;
  dit_tree->SetBranchAddress("run",&run);

  std::string index1 = "run";
  dit_tree->BuildIndex("run");
  TTreeIndex *dit_indexedTree = (TTreeIndex *) dit_tree->GetTreeIndex();

  Long64_t nEntries = dit_tree->GetEntries();
  Double_t current_part = 0;
  out_dit_tree->Branch("crex_part",&current_part);

  for(int ievt=0;ievt<nEntries;ievt++){
    dit_tree -> GetEntry(ievt);
    if (run < 6328) {
      current_part = 1.0;
    }
    if (run >= 6328 && run < 7500) {
      current_part = 2.0;
    }
    if (run >= 7500) {
      current_part = 3.0;
    }
    out_dit_tree -> Fill();
  }

  out_dit_tree -> Write("dit",TObject::kOverwrite);

  TKey *key;
  TIter nextkey(dit_input.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)dit_input.Get(key->GetName());
      if ((TString)key->GetName() == "dit") {
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
  dit_input.Close();

  output.Close();
}

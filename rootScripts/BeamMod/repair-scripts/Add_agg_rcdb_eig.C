/*
author: Cameron Clarke
last update: February 2021

*/

void Add_agg_rcdb_eig(){ //(int start, int end)
  // FIXME replace with read-from-agg tree
  //TFile eig_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_AT.root"));
  TFile eig_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_CREX.root"));
  TFile agg_input(Form("dataRootfiles/CREX-All-miniruns.root"));
  TFile output(Form("rootfiles/rcdb_eigenvectors.root"),"recreate");
  //TFile output(Form("rootfiles/rcdb_AT_eigenvectors.root"),"recreate");

  TTree *agg_tree;
  TTree *mini_tree;
  TTree *eig_reg_tree_all;
  TTree *eig_reg_tree_all_tr;
  TTree *eig_reg_tree_5;
  TTree *eig_reg_tree_5_tr;
  agg_input.GetObject("agg", agg_tree);
  eig_input.GetObject("mini", mini_tree);
  eig_input.GetObject("mini_eigen_reg_allbpms", eig_reg_tree_all);
  eig_input.GetObject("mini_eigen_reg_allbpms_tr", eig_reg_tree_all_tr);
  eig_input.GetObject("mini_eigen_reg_5bpms", eig_reg_tree_5);
  eig_input.GetObject("mini_eigen_reg_5bpms_tr", eig_reg_tree_5_tr);

  TChain* out_mini_tree           = (TChain*)mini_tree           -> CloneTree(0);
  TChain* out_eig_reg_tree_all    = (TChain*)eig_reg_tree_all    -> CloneTree(0);
  TChain* out_eig_reg_tree_all_tr = (TChain*)eig_reg_tree_all_tr -> CloneTree(0);
  TChain* out_eig_reg_tree_5      = (TChain*)eig_reg_tree_5      -> CloneTree(0);
  TChain* out_eig_reg_tree_5_tr   = (TChain*)eig_reg_tree_5_tr   -> CloneTree(0);

  Int_t run, burst_counter;
  mini_tree->SetBranchAddress("run",&run);
  mini_tree->SetBranchAddress("mini",&burst_counter);

  // Add new branches
  // 
  // - Loop over all "rcdb_" branches in agg tree
  // - - agg->SetBranchAddress(name,placeholder_agg);
  // - - make clone/copies of target trees
  // - - eig_reg_tree_all->Branch(name,placeholder_new);
  // - Build index on run, minirun for all trees
  // - Loop over all entries in mini/eig/reg/lagr
  // - - Find corresponding indexed entry in agg
  // - - Set new = old values
  // - Write tree
  //
  // Adding new branches?
  //eig_reg_tree_all->Branch();

  std::string index1 = "run_number";
  std::string index2 = "minirun_n";
  agg_tree->BuildIndex(index1.c_str(),index2.c_str());
  mini_tree->BuildIndex("run","mini");
  TTreeIndex *mini_indexedTree = (TTreeIndex *) mini_tree->GetTreeIndex();
  TTreeIndex *agg_indexedTree = (TTreeIndex *) agg_tree->GetTreeIndex();

  TObjArray *var_list=agg_tree->GetListOfBranches();
  TIter var_iter(var_list);
  std::vector<Double_t> old_vals;
  std::vector<Double_t> new_vals;
  std::vector<TString> names;
  Int_t nRCDB = 0;
  
  while (auto *var= var_iter.Next()){
    TString name(var->GetName());
    if (name.Contains("rcdb_")) {
      names.push_back(name);
      //double old_tmpval = 0.0;
      ////double new_tmpval = 0.0;
      old_vals.push_back(0.0);
      new_vals.push_back(0.0);
      nRCDB++;
    }
  }

  for (int m = 0 ; m < nRCDB ; m++) {
    // Add new branch to out_mini_tree
    agg_tree->SetBranchAddress(names[m],&old_vals[m]);
    out_mini_tree->Branch(names[m],&new_vals[m]);
  }

  Long64_t nEntries = mini_tree->GetEntries();
  Int_t last_run = -1;

  for(int ievt=0;ievt<nEntries;ievt++){
    mini_tree           -> GetEntry(ievt);
    eig_reg_tree_all    -> GetEntry(ievt);
    eig_reg_tree_all_tr -> GetEntry(ievt);
    eig_reg_tree_5      -> GetEntry(ievt);
    eig_reg_tree_5_tr   -> GetEntry(ievt);
    agg_tree->GetEntryWithIndex((Double_t)run,(Double_t)burst_counter);
    // For each branch in rcdb branches list set new = old
    //////// lagr_tree->GetEntry(ievt);
    for (int o = 0 ; o < nRCDB ; o++) {
      new_vals.at(o) = old_vals.at(o);
      //Printf("RCDB data %d = %f",o,old_vals.at(o));
    }
    out_mini_tree           -> Fill();
    out_eig_reg_tree_all    -> Fill();
    out_eig_reg_tree_all_tr -> Fill();
    out_eig_reg_tree_5      -> Fill();
    out_eig_reg_tree_5_tr   -> Fill();
  }

  //output.cd();
  out_mini_tree           -> Write("mini",TObject::kOverwrite);
  out_eig_reg_tree_all    -> Write("mini_eigen_reg_allbpms",TObject::kOverwrite);
  out_eig_reg_tree_all_tr -> Write("mini_eigen_reg_allbpms_tr",TObject::kOverwrite);
  out_eig_reg_tree_5      -> Write("mini_eigen_reg_5bpms",TObject::kOverwrite);
  out_eig_reg_tree_5_tr   -> Write("mini_eigen_reg_5bpms_tr",TObject::kOverwrite);
  //output.Close();
  //delete output;
  //delete agg_tree;
  //delete mini_tree;
  //delete eig_reg_tree_all;
  //delete eig_reg_tree_all_tr;
  //delete eig_reg_tree_5;
  //delete eig_reg_tree_5_tr;
}

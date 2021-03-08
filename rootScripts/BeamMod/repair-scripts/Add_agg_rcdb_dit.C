/*
author: Cameron Clarke
last update: February 2021

*/

void Add_agg_rcdb_dit(TString scale = "run_avg"){ //(int start, int end)
  // FIXME replace with read-from-agg tree
  //TFile eig_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_AT.root"));
  TFile * dit_input;
  TFile * output;
  if (scale == "run_avg") {
    dit_input = new TFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_run_avg_1X/dithering_slopes_13746_runwise.root"));
    output = new TFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_run_avg_1X/dithering_slopes_13746_rcdb_runwise.root"),"recreate");
  }
  else {
    dit_input = new TFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_cyclewise_1X/dithering_slopes_13746_cyclewise.root"));
    output = new TFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_cyclewise_1X/dithering_slopes_13746_rcdb_cyclewise.root"),"recreate");
  }
  //Double_t mini_index = -1.0; // for runwise aggregator Not commonly used
  Double_t mini_index = 0.0; // for minirun aggregator

  TTree *dit_tree;
  TChain* agg_tree = new TChain("agg");
  agg_tree->Add("/lustre19/expphy/volatile/halla/parity/crex-respin1/aggRootfiles/ErrorFlag_dit_1X/slugRootfiles/CREX-All-miniruns.root");
  dit_input->GetObject("dit", dit_tree);

  TChain* out_dit_tree = (TChain*)dit_tree -> CloneTree(0);

  Int_t run;
  Double_t cycle;
  dit_tree->SetBranchAddress("run",&run);
  dit_tree->SetBranchAddress("cyclenum",&cycle);
  //dit_tree->SetBranchAddress("minirun_n",&burst_counter); // No miniruns

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
  dit_tree->BuildIndex("run"); // No second index 
  TTreeIndex *dit_indexedTree = (TTreeIndex *) dit_tree->GetTreeIndex();
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
    // Add new branch to out_dit_tree
    agg_tree->SetBranchAddress(names[m],&old_vals[m]);
    out_dit_tree->Branch(names[m],&new_vals[m]);
  }

  Long64_t nEntries = dit_tree->GetEntries();
  Int_t last_run = -1;

  for(int ievt=0;ievt<nEntries;ievt++){
    dit_tree       -> GetEntry(ievt);
    agg_tree->GetEntryWithIndex((Double_t)run,mini_index); // Use minirun 0 for the index. If using run aggregator then use minirun_num == -1.0
    // For each branch in rcdb branches list set new = old
    //////// lagr_tree->GetEntry(ievt);
    for (int o = 0 ; o < nRCDB ; o++) {
      new_vals.at(o) = old_vals.at(o);
      //Printf("RCDB data %d = %f",o,old_vals.at(o));
    }
    out_dit_tree   -> Fill();
  }

  output->cd();
  out_dit_tree     -> Write("dit",TObject::kOverwrite);
  //output.Close();
  //delete output;
  //delete agg_tree;
  //delete mini_tree;
}

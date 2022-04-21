/*
author: Cameron Clarke
Last update: May 2021

*/

void Expand_dit_to_mini(TString infile, TString outfile, TString treename){
  // FIXME replace with read-from-agg tree
  //TFile eig_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_AT.root"));
  TFile * dit_input;
  TFile * output;
  dit_input = new TFile(infile);
  output = new TFile(outfile,"recreate");

  TTree *dit_tree;
  TChain* agg_tree = new TChain("agg_plain_friendable");
  agg_tree->Add("/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/processed_respin2_data/mini_friended_agg.root"); // Use mini friended Agg file to skip a few steps of verifying data quality :)
  dit_input->GetObject("dit", dit_tree);
  dit_tree ->SetName(treename+"_tagged_friendable");

  TChain* out_dit_tree = (TChain*)dit_tree -> CloneTree(0);

  Int_t    dit_run      = 0;
  Double_t dit_cycle    = 0;
  Double_t agg_run  = 0;
  Double_t agg_mini = 0;
  dit_tree->SetBranchAddress("run",&dit_run);
  dit_tree->SetBranchAddress("cyclenum",&dit_cycle);
  agg_tree->SetBranchAddress("run_number",&agg_run);
  agg_tree->SetBranchAddress("minirun_n",&agg_mini);
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
  
  names.push_back("minirun_n");
  old_vals.push_back(0.0);
  new_vals.push_back(0.0);
  nRCDB++;
  /*while (auto *var= var_iter.Next()){
    TString name(var->GetName());
    if (name.Contains("rcdb_")) {
      names.push_back(name);
      //double old_tmpval = 0.0;
      ////double new_tmpval = 0.0;
      old_vals.push_back(0.0);
      new_vals.push_back(0.0);
      nRCDB++;
    }
  }*/

  Double_t clean_data = 0;
  out_dit_tree->Branch("dit_true_data",&clean_data);

  for (int m = 0 ; m < nRCDB ; m++) {
    // Add new branch to out_dit_tree
    agg_tree->SetBranchAddress(names[m],&old_vals[m]);
    out_dit_tree->Branch(names[m],&new_vals[m]);
  }

  dit_tree->Draw(">>elist",Form("flag==1 && rcdb_run_flag == 1 && rcdb_run_type == 1"),"entrylist");
  TEntryList* elist = (TEntryList*)gDirectory->Get("elist");
  dit_tree->SetEntryList(elist);
  dit_tree->BuildIndex("run"); // No second index 
  dit_indexedTree = (TTreeIndex *) dit_tree->GetTreeIndex();
  agg_indexedTree = (TTreeIndex *) agg_tree->GetTreeIndex();

  Long64_t nEntries_dit = dit_tree->GetEntries();
  Long64_t nEntries_agg = agg_tree->GetEntries();
  Int_t last_run = -1;

  // Loop over the aggregator tree
  for(int ievt=0;ievt<nEntries_agg;ievt++){
    agg_tree -> GetEntry(ievt);
    Int_t goodbytes = dit_tree -> GetEntry(dit_tree->GetEntryNumberWithIndex((Int_t)agg_run)); // FIXME Add in minirun index for minirun tagged outputs... nested minirun distance loop below here...
    clean_data = 1;
    if (goodbytes==0) {
      clean_data = 0;
      Int_t distance = 1;
      // Look behind us
      while (goodbytes == 0 && distance < 501) {
        goodbytes = dit_tree -> GetEntry(dit_tree->GetEntryNumberWithIndex((Int_t)agg_run-distance));
        distance++;
      }
      // If nothing is behind us then look ahead of us (should only apply to slug 100 or so)
      if (distance >= 500) {
        distance = 0;
        while (goodbytes == 0 && distance < 501) {
          goodbytes = dit_tree -> GetEntry(dit_tree->GetEntryNumberWithIndex((Int_t)agg_run+distance));
          distance++;
        }
      }
    }
    // For each branch in rcdb branches list set new = old
    //////// lagr_tree->GetEntry(ievt);
    for (int o = 0 ; o < nRCDB ; o++) {
      new_vals.at(o) = old_vals.at(o);
      //Printf("RCDB data %d = %f",o,old_vals.at(o));
    }
    out_dit_tree -> Fill();
  }

  output->cd();
  out_dit_tree   -> Write(treename+"_tagged_friendable",TObject::kOverwrite);
  //output.Close();
  //delete output;
  //delete agg_tree;
  //delete mini_tree;
}

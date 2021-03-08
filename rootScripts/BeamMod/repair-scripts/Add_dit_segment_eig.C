/*
author: Cameron Clarke
last update: February 2021

*/

void Add_dit_segment_eig(){ //(int start, int end)
  // FIXME replace with read-from-dit tree
  //TFile eig_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_AT.root"));
  TFile eig_input(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_eigenvectors_sorted.root"));
  TFile dit_input(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/bmodOutput/slopes_run_avg_1X/dithering_slopes_13746_runwise.root"));
  TFile output(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted.root"),"recreate");
//  // Versions of eigenvectors ouputs : double sorted here
//  TFile eig_input(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_eigenvectors_sorted_sorted.root"));
//  TFile output(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted_sorted.root"),"recreate");
  //TFile output(Form("rootfiles/rcdb_AT_eigenvectors.root"),"recreate");

  TTree *dit_tree;
  TTree *mini_tree;
  TTree *eig_reg_tree_all;
  TTree *eig_reg_tree_all_sorted;
  TTree *eig_reg_tree_all_tr;
  TTree *eig_reg_tree_5;
  TTree *eig_reg_tree_5_sorted;
  //TTree *eig_reg_tree_5_tr;
  dit_input.GetObject("dit", dit_tree);
  eig_input.GetObject("mini", mini_tree);
  eig_input.GetObject("mini_eigen_reg_allbpms", eig_reg_tree_all);
  eig_input.GetObject("mini_eigen_reg_allbpms_sorted", eig_reg_tree_all_sorted);
  eig_input.GetObject("mini_eigen_reg_allbpms_tr", eig_reg_tree_all_tr);
  eig_input.GetObject("mini_eigen_reg_5bpms", eig_reg_tree_5);
  eig_input.GetObject("mini_eigen_reg_5bpms_sorted", eig_reg_tree_5_sorted);
  //eig_input.GetObject("mini_eigen_reg_5bpms_tr", eig_reg_tree_5_tr);

  TChain* out_mini_tree             = (TChain*)mini_tree           -> CloneTree(0);
  TChain* out_eig_reg_tree_all      = (TChain*)eig_reg_tree_all    -> CloneTree(0);
  TChain* out_eig_reg_tree_all_sorted = (TChain*)eig_reg_tree_all_sorted -> CloneTree(0);
  TChain* out_eig_reg_tree_all_tr   = (TChain*)eig_reg_tree_all_tr -> CloneTree(0);
  TChain* out_eig_reg_tree_5        = (TChain*)eig_reg_tree_5      -> CloneTree(0);
  TChain* out_eig_reg_tree_5_sorted = (TChain*)eig_reg_tree_5_sorted     -> CloneTree(0);
  //TChain* out_eig_reg_tree_5_tr   = (TChain*)eig_reg_tree_5_tr   -> CloneTree(0);

  Int_t run, burst_counter;
  mini_tree->SetBranchAddress("run",&run);
  mini_tree->SetBranchAddress("mini",&burst_counter);

  // Add new branche

  std::string index1 = "run";
  dit_tree->BuildIndex(index1.c_str());
  mini_tree->BuildIndex("run","mini");
  TTreeIndex *mini_indexedTree = (TTreeIndex *) mini_tree->GetTreeIndex();
  TTreeIndex *dit_indexedTree = (TTreeIndex *) dit_tree->GetTreeIndex();

  TObjArray *var_list=dit_tree->GetListOfBranches();
  TIter var_iter(var_list);
  std::vector<Int_t> int_old_vals;
  std::vector<Int_t> int_new_vals;
  std::vector<TString> int_old_names;
  std::vector<TString> int_new_names;
  Int_t int_nSEG = 0;
  std::vector<Double_t> old_vals;
  std::vector<Double_t> new_vals;
  std::vector<TString> old_names;
  std::vector<TString> new_names;
  Int_t nSEG = 0;
  
  while (auto *var= var_iter.Next()){
    TString name(var->GetName());
    if (name.Contains("segment") || name == "flag" || name == "alldetsflag") {
      int_old_names.push_back(name);
      int_new_names.push_back("dit_"+name);
      //double old_tmpval = 0.0;
      ////double new_tmpval = 0.0;
      int_old_vals.push_back(0.0);
      int_new_vals.push_back(0.0);
      int_nSEG++;
    }
    else if (name == "cyclenum") {
      old_names.push_back(name);
      new_names.push_back("dit_"+name);
      //double old_tmpval = 0.0;
      ////double new_tmpval = 0.0;
      old_vals.push_back(0.0);
      new_vals.push_back(0.0);
      nSEG++;
    }
  }

  for (int m = 0 ; m < int_nSEG ; m++) {
    // Add new branch to out_mini_tree
    dit_tree->SetBranchAddress(int_old_names[m],&int_old_vals[m]);
    out_mini_tree->Branch(int_new_names[m],&int_new_vals[m]);
  }
  for (int m = 0 ; m < nSEG ; m++) {
    // Add new branch to out_mini_tree
    dit_tree->SetBranchAddress(old_names[m],&old_vals[m]);
    out_mini_tree->Branch(new_names[m],&new_vals[m]);
  }

  Long64_t nEntries = mini_tree->GetEntries();
  Int_t last_run = -1;

  for(int ievt=0;ievt<nEntries;ievt++){
    mini_tree           -> GetEntry(ievt);
    eig_reg_tree_all    -> GetEntry(ievt);
    eig_reg_tree_all_sorted -> GetEntry(ievt);
    eig_reg_tree_all_tr -> GetEntry(ievt);
    eig_reg_tree_5      -> GetEntry(ievt);
    eig_reg_tree_5_sorted -> GetEntry(ievt);
    //eig_reg_tree_5_tr   -> GetEntry(ievt);
    dit_tree->GetEntryWithIndex((Double_t)run,(Double_t)burst_counter);
    // For each branch in rcdb branches list set new = old
    //////// lagr_tree->GetEntry(ievt);
    for (int o = 0 ; o < int_nSEG ; o++) {
      int_new_vals.at(o) = int_old_vals.at(o);
      //Printf("RCDB data %d = %f",o,old_vals.at(o));
    }
    for (int o = 0 ; o < nSEG ; o++) {
      new_vals.at(o) = old_vals.at(o);
      //Printf("RCDB data %d = %f",o,old_vals.at(o));
    }
    out_mini_tree             -> Fill();
    out_eig_reg_tree_all      -> Fill();
    out_eig_reg_tree_all_sorted -> Fill();
    out_eig_reg_tree_all_tr   -> Fill();
    out_eig_reg_tree_5        -> Fill();
    out_eig_reg_tree_5_sorted -> Fill();
    //out_eig_reg_tree_5_tr   -> Fill();
  }

  //output.cd();
  out_mini_tree             -> Write("mini",TObject::kOverwrite);
  out_eig_reg_tree_all      -> Write("mini_eigen_reg_allbpms",TObject::kOverwrite);
  out_eig_reg_tree_all_sorted -> Write("mini_eigen_reg_allbpms_sorted",TObject::kOverwrite);
  out_eig_reg_tree_all_tr   -> Write("mini_eigen_reg_allbpms_tr",TObject::kOverwrite);
  out_eig_reg_tree_5        -> Write("mini_eigen_reg_5bpms",TObject::kOverwrite);
  out_eig_reg_tree_5_sorted -> Write("mini_eigen_reg_5bpms_sorted",TObject::kOverwrite);
  //out_eig_reg_tree_5_tr   -> Write("mini_eigen_reg_5bpms_tr",TObject::kOverwrite);
  //output.Close();
  //delete output;
  //delete dit_tree;
  //delete mini_tree;
  //delete eig_reg_tree_all;
  //delete eig_reg_tree_all_sorted;
  //delete eig_reg_tree_all_tr;
  //delete eig_reg_tree_5;
  //delete eig_reg_tree_5_sorted;
  ////delete eig_reg_tree_5_tr;
}

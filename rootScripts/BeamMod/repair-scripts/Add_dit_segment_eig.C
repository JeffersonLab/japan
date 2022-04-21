/*
author: Cameron Clarke
last update: July 7 2021

*/

void Add_dit_segment_eig(TString dit_inputfilename, TString input, TString outputfile, TString treename = "mini"){ 
  // FIXME replace with read-from-dit tree
  //TFile eig_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_AT.root"));
  TFile eig_input(input);
  TFile dit_input(dit_inputfilename);
  // FIXME FIXME I need to do segment stuff first!!! FIXME TFile dit_input(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_run_avg_1X/respin2_CREX_dithering_slopes_run_avg_basic.root")); // FIXME this is just grabbing the old normal dithering data - it's just grabbing flag and segment from the tree - but be careful
  TFile output(outputfile,"recreate");
//  // Versions of eigenvectors ouputs : double sorted here
//  TFile eig_input(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_eigenvectors_sorted_sorted.root"));
//  TFile output(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted_sorted.root"),"recreate");
  //TFile output(Form("rootfiles/rcdb_AT_eigenvectors.root"),"recreate");

  TTree *dit_tree;
  TTree *mini_tree;
  //TTree *eig_reg_tree_5_tr;
  dit_input.GetObject("dit", dit_tree);
  eig_input.GetObject(treename, mini_tree);

  TChain* out_mini_tree             = (TChain*)mini_tree           -> CloneTree(0);

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
      int_old_vals.push_back(0);
      int_new_vals.push_back(0);
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
    if (out_mini_tree->GetBranch(int_new_names[m])) {
      out_mini_tree->SetBranchAddress(int_new_names[m],&int_new_vals[m]);
    }
    else {
      out_mini_tree->Branch(int_new_names[m],&int_new_vals[m]);
    }
  }
  for (int m = 0 ; m < nSEG ; m++) {
    // Add new branch to out_mini_tree
    dit_tree->SetBranchAddress(old_names[m],&old_vals[m]);
    if (out_mini_tree->GetBranch(new_names[m])) {
      out_mini_tree->SetBranchAddress(new_names[m],&new_vals[m]);
    }
    else {
      out_mini_tree->Branch(new_names[m],&new_vals[m]);
    }
  }

  Long64_t nEntries = mini_tree->GetEntries();
  Int_t last_run = -1;

  Int_t runStepper = 0;
  Int_t goodRun = 0;
  for(int ievt=0;ievt<nEntries;ievt++){
    mini_tree           -> GetEntry(ievt);
    //eig_reg_tree_5_tr   -> GetEntry(ievt);
    goodRun = dit_tree->GetEntryNumberWithIndex((Double_t)run);
    while (goodRun == -1) {
      // In this loop, if there is no corresponding entry in the tree being copied from, we run forward and grab the next available entry in the future to take its value -> FIXME Note that this means that any run existing in mini tree that is at the end of a segment but that doesn't exist explicitly in the dithering tree will be placed in THE WRONG segment label! -> I can instead loop over the contiguous list segment text file...... but does it really matter? I will use slugs and pitts for any meaningful calculations in the future, except for "segment averaged slopes" that I can also check from the updated CorrecTree outputs (that now contain explicit slope values).
      //Printf("Checking run %d from entry %d",run,goodRun);
      runStepper++;
      goodRun = dit_tree->GetEntryNumberWithIndex((Double_t)(run+runStepper));
    }
    //Printf("Got run %d from entry %d",run,goodRun);
    dit_tree->GetEntry(goodRun);
    runStepper = 0;

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
  }

  out_mini_tree             -> Write(treename,TObject::kOverwrite);

  TKey *key;
  TIter nextkey(eig_input.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)eig_input.Get(key->GetName());
      if ((TString)key->GetName() == treename) {
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
  dit_input.Close();
  output.Close();
}

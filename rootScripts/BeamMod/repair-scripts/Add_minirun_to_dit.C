
/*
author: Cameron Clarke
last update: May 2021

*/

void Add_minirun_to_dit(TString infile, TString outfile){
  // FIXME replace with read-from-agg tree
  //TFile eig_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_AT.root"));
  TFile * dit_input;
  TFile * output;
  dit_input = new TFile(infile);
  output = new TFile(outfile,"recreate");
  //Double_t mini_index = -1.0; // for runwise aggregator Not commonly used
  Double_t mini_index = 0.0; // for minirun aggregator

  TTree *dit_tree;
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

  dit_tree->BuildIndex("run"); // No second index 
  TTreeIndex *dit_indexedTree = (TTreeIndex *) dit_tree->GetTreeIndex();

  Double_t dit_minirun_n = 0.0;
  out_dit_tree->Branch("dit_minirun_n",&dit_minirun_n);

  Long64_t nEntries = dit_tree->GetEntries();
  Int_t last_run = -1;
  Int_t found = 0;
  Short_t BurstCounter = -1;
  Double_t mul_cyclenum = -1;
  TEntryList* elist;

  for(int ievt=0;ievt<nEntries;ievt++){
    dit_tree -> GetEntry(ievt);
    Printf("Trying run %d, /lustre19/expphy/volatile/halla/parity/crex-respin2/japanOutput/prexPrompt_pass2_%d.000.root",run,run);
    TTree* mul_tree;
    TFile* mul_input = TFile::Open(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/japanOutput/prexPrompt_pass2_%d.000.root",run));
    if (!mul_input) {
      continue;
    }
    mul_input->GetObject("mul",mul_tree);
    if (!mul_tree) {
      continue;
    }
    mul_tree->Draw(">>elist","yield_bmwcycnum>0","entrylist");
    elist = (TEntryList*)gDirectory->Get("elist");
    mul_tree->SetEntryList(elist);
    //TLeaf *l_BurstCounter = mul_tree->GetLeaf("BurstCounter");
    //TLeaf *l_CycleNum     = mul_tree->GetLeaf("yield_bmwcycnum");
    int nent = mul_tree->Draw("BurstCounter:yield_bmwcycnum","yield_bmwcycnum>0","goff");
    BurstCounter = -1;
    mul_cyclenum = -1;
    mul_tree->SetBranchAddress("BurstCounter",&BurstCounter);
    mul_tree->SetBranchAddress("yield_bmwcycnum",&mul_cyclenum);
    found = 0;
    for (Int_t i = 0 ; i < nent; i++) {
      mul_tree->GetEntry(elist->GetEntry(i));
      //l_BurstCounter->GetBranch()->GetEntry(elist->GetEntry(i));
      //l_CycleNum->GetBranch()->GetEntry(elist->GetEntry(i));
      //mul_cyclenum = (Double_t)l_CycleNum->GetValue();
      if (cycle == mul_cyclenum){
        Printf("Found cyclenum = %d at burst %d",(Int_t)mul_cyclenum,(Int_t)BurstCounter);
        found = 1;
        break;
      }
    }
    // For each branch in rcdb branches list set new = old
    //////// lagr_tree->GetEntry(ievt);
    if (found == 1) {
      dit_minirun_n = (Double_t)BurstCounter;
      //dit_minirun_n = (Double_t)l_BurstCounter->GetValue();
    }
    else {
      // Should be impossible...
      dit_minirun_n = -1;
    }
    out_dit_tree -> Fill();
    //delete mul_input;
    mul_input->Close();
  }

  output->cd();
  out_dit_tree   -> Write("dit",TObject::kOverwrite);
  //output.Close();
  //delete output;
  //delete agg_tree;
  //delete mini_tree;
}

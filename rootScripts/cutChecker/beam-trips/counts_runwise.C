void counts_runwise() {

  TCanvas * c1 = new TCanvas();
  c1->Divide(3,2);
  TTree *r1_mul_tree;
  TTree *pr_mul_tree;

  for (Int_t i = 6600; i < 8558; i++) {
    TFile * r1_infile_run = new TFile(Form("/chafs1/work1/apar/cameronc/rootfiles/respin1_prune_rootfiles/trip_%d.root",i));
    TFile * pr_infile_run = new TFile(Form("/chafs1/work1/apar/cameronc/rootfiles/prompt_prune_rootfiles/trip_%d.root",i));
    r1_infile_run->GetObject("mul_trip", r1_mul_tree);
    pr_infile_run->GetObject("mul_trip", pr_mul_tree);
    if (!pr_mul_tree && r1_mul_tree) { Printf("Respin 1 exists, prompt doesn't exist - run %d",i);}
    if (pr_mul_tree && !r1_mul_tree) { Printf("Respin 1 doesn't exist, prompt does exist - run %d",i);}
    if (!pr_mul_tree || !r1_mul_tree) { continue; }
    Int_t r1_nData = r1_mul_tree->Draw(Form("asym_bcm_an_us.hw_sum/1e-9"),Form(""),"goff");
    Int_t pr_nData = pr_mul_tree->Draw(Form("asym_bcm_an_us.hw_sum/1e-9"),Form(""),"goff");
    if (r1_nData-50!=pr_nData) {
      Printf("Run %d has %d more entries in prompt than respin1",i,r1_nData-pr_nData);
    }
    delete r1_infile_run;
    delete pr_infile_run;
  }
}

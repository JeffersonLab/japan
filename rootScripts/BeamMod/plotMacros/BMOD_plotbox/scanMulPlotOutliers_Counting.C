void scanMulPlotOutliers_Counting(Double_t outlier = 5.0) {
  TChain* mul_prune = new TChain("mul_prune");
  mul_prune->Add("/lustre19/expphy/volatile/halla/parity/crex-respin2/pruned_lagr_analysis/All_Lagr_Production.root");
  mul_prune->SetScanField(0);
  std::map<Double_t,Int_t> duplicate_slug;
  std::map<Double_t,Int_t> duplicate_run;
  Double_t slug = 0;
  Double_t run = 0;
  Double_t priorrun = 0;
  Double_t event = 0;
  Short_t mini = 0; 
  mul_prune->Draw(">>elist1",Form("(ErrorFlag&0xda7e6bff)==0 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && rcdb_slug < 4000 && abs(eigen_lagr_asym_us_avg/1e-6)>(%f*780)",outlier),"entrylist"); //,1000000,1000000);
  TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist1"));
  mul_prune->SetEntryList(elist);
  mul_prune->SetBranchAddress("rcdb_slug",&slug);
  mul_prune->SetBranchAddress("run_number",&run);
  mul_prune->SetBranchAddress("CodaEventNumber",&event);
  mul_prune->SetBranchAddress("BurstCounter",&mini);
  Printf("Cut %0.0f sigma done",outlier);
  for (Int_t i = 0 ; i < elist->GetN() ; i++) {
    //Printf("Entry %d",i);
    mul_prune->GetEntry(elist->Next());
    //mul_prune->GetEntry(mul_prune->GetEntryNumber(i));
    Printf("Run %0.0f, slug %0.0f, CEN %0.0f",run,slug,event);
    if (duplicate_slug.count(slug) == 0) {
      duplicate_slug[slug] = 1;
    }
    else {
      duplicate_slug[slug] = duplicate_slug.at(slug) + 1;
    }

    if (priorrun == run) {
      if (duplicate_run.count(run) == 0) {
        duplicate_run[run] = 1;
      }
      else {
        duplicate_run[run] = duplicate_run.at(run) + 1;
      }
    }
    priorrun = run;
  }
  Printf("Looking at runs with more than 1 outlier in them:");
  for ( const auto &rn : duplicate_run ) {
    if (rn.second > 1) {
      Printf("Run %0.0f, %d",rn.first,rn.second);
    }
  }
  Printf("Number of outliers per slug:");
  for ( const auto &sl : duplicate_slug ) {
    Printf("Slug %0.0f, %d",sl.first,sl.second);
  }
}

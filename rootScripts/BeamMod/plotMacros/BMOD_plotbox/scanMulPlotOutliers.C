void scanMulPlotOutliers(Double_t outlier = 5.0) {
  TChain* mul_prune = new TChain("mul_prune");
  mul_prune->Add("/lustre19/expphy/volatile/halla/parity/crex-respin2/pruned_lagr_analysis/All_Lagr_Production.root");
  mul_prune->SetScanField(0);
  mul_prune->Scan("rcdb_slug:run_number:BurstCounter:CodaEventNumber:eigen_lagr_asym_us_avg",Form("(ErrorFlag&0xda7e6bff)==0 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && rcdb_slug < 4000 && abs(eigen_lagr_asym_us_avg/1e-6)>(%f*780)",outlier));

}

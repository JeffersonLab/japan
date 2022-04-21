void compareMulPlots(Int_t run = 8558) {
  TChain * mul_prune = new TChain("mul_prune");
  mul_prune->Add(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/pruned_lagr_analysis/prune_%d.root",run));


  //gStyle->SetOptStat(112211);
  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(1);
  TCanvas * c1 = new TCanvas();
  c1->SetLogy();
  //mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv_lin(5000,-4000,4000)","(ErrorFlag)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  mul_prune->Draw("rcdb_sign*asym_us_avg/1e-6>>Apv(1000,-15000,15000)","(ErrorFlag&0xda7e6bff)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  TH1F* Apv = (TH1F*) gROOT->FindObject("Apv");
  Apv->SetNameTitle("Apv, Raw (red) vs. Corrected (blue)","Ap, Raw (red) vs. Corrected (blue);asym (ppm)");
  Printf("raw rms = %f",Apv->GetRMS());
  Apv->SetLineColor(kRed);
  Apv->SetMaximum(1.5e3);//SetRangeUser(0,1.2e3);
  //Apv->GetYaxis()->SetLimits(0,1.3e3);//SetRangeUser(0,1.2e3);
  c1->Modified();
  c1->Update();
  mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6","(ErrorFlag&0xda7e6bff)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))","same");
  TH1F* htemp1 = (TH1F*) gROOT->FindObject("htemp");

  Printf("corrected rms = %f",htemp1->GetRMS());
  c1->SaveAs(Form("IncludeBMOD_Mul_plot_%d.pdf",run));
}

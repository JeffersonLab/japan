void makeMulPlots() {
  TChain * mul_prune = new TChain("mul_prune");
  mul_prune->Add("/lustre19/expphy/volatile/halla/parity/crex-respin2/pruned_lagr_analysis/All_Lagr_Production.root");


  gStyle->SetOptStat(112211);
  gStyle->SetOptFit(1);
  TCanvas * c1 = new TCanvas();
  c1->SetLogy(0);
  //mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv_lin(5000,-4000,4000)","(ErrorFlag)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv_lin(1000,-4000,4000)","(ErrorFlag&0xda7e6bff)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  TH1F* Apv_lin = (TH1F*) gROOT->FindObject("Apv_lin");
  Apv_lin->SetLineWidth(3);
  Apv_lin->SetNameTitle("Apv > 135 uA, needed cut, linear","Apv, > 135 uA cut out, needed miniruns cut;asym (ppm)");
  Apv_lin->Fit("gaus");

  c1->SaveAs("IncludeBMOD_Mul_plots_lin.pdf");
  c1->SaveAs("IncludeBMOD_Mul_plots.pdf(");


  gStyle->SetOptStat(112211);
  gStyle->SetOptFit(1);
  c1->SetLogy();
  //mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv(1000,-25000,25000)","(ErrorFlag)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv(1000,-25000,25000)","(ErrorFlag&0xda7e6bff)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  TH1F* Apv = (TH1F*) gROOT->FindObject("Apv");
  Apv->SetNameTitle("Apv > 135 uA, needed cut","Apv, > 135 uA cut out, needed miniruns cut;asym (ppm)");
  Apv->Fit("gaus");

  c1->SaveAs("IncludeBMOD_Mul_plots.pdf");


  gStyle->SetOptStat(112211);
  gStyle->SetOptFit(1);
  c1->SetLogy();
  //mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv(1000,-25000,25000)","(ErrorFlag)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_dd/1e-6>>Apv_dd(1000,-25000,25000)","(ErrorFlag&0xda7e6bff)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  TH1F* DD = (TH1F*) gROOT->FindObject("Apv_dd");
  DD->SetNameTitle("DD > 135 uA, needed cut","DD, > 135 uA cut out, needed miniruns cut;asym (ppm)");
  DD->Fit("gaus");

  c1->SaveAs("IncludeBMOD_Mul_plots.pdf");


  gStyle->SetOptStat(112211);
  gStyle->SetOptFit(1);
  c1->SetLogy();
  //mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv(1000,-25000,25000)","(ErrorFlag)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  mul_prune->Draw("rcdb_sign*eigen_reg_asym_us_avg/1e-6>>Apv_reg(1000,-25000,25000)","(ErrorFlag&0xda7e6bff)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  TH1F* Reg = (TH1F*) gROOT->FindObject("Apv_reg");
  Reg->SetNameTitle("Apv Regression > 135 uA, needed cut","Apv Regression, > 135 uA cut out, needed miniruns cut;asym (ppm)");
  Reg->Fit("gaus");

  c1->SaveAs("IncludeBMOD_Mul_plots.pdf");


  c1->Clear();
  c1->SetLogy();
  //mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv2(1000,-25000,25000)","(ErrorFlag)==0 && rcdb_slug<4000 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv2(1000,-25000,25000)","(ErrorFlag&0xda7e6bff)==0 && rcdb_slug<4000 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1 && (!(run_number==6564 && BurstCounter==4) && !(run_number==6567 && (BurstCounter==2 || BurstCounter==4)) && !(run_number==6571 && (BurstCounter==3 || BurstCounter==4)) && !(run_number==6593 && BurstCounter==2) && !(run_number==6983 && BurstCounter==8) && !(run_number==7149 && BurstCounter==6) && !(run_number==7211 && BurstCounter==4) && !(run_number==7889 && BurstCounter==0) && !(run_number==7942 && BurstCounter==5) && !(run_number==8036 && BurstCounter==2) && !(run_number==8240 && BurstCounter==1) && !(run_number==8549 && (BurstCounter==0 || BurstCounter==1 || BurstCounter==4)))");
  TH1F* Apv2 = (TH1F*) gROOT->FindObject("Apv2");
  Apv2->SetNameTitle("Apv all currents, needed cut","Apv, all currents, needed miniruns cut;asym (ppm)");
  Apv2->Fit("gaus");

  c1->SaveAs("IncludeBMOD_Mul_plots.pdf");


  c1->Clear();
  c1->SetLogy();
  //mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv3(1000,-25000,25000)","(ErrorFlag)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1");
  mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv3(1000,-25000,25000)","(ErrorFlag&0xda7e6bff)==0 && rcdb_slug<4000 && yield_bcm_an_us>135 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1");
  TH1F* Apv3 = (TH1F*) gROOT->FindObject("Apv3");
  Apv3->SetNameTitle("Apv > 135 uA, no cut","Apv, > 135 uA cut out, no miniruns cut;asym (ppm)");
  Apv3->Fit("gaus");

  c1->SaveAs("IncludeBMOD_Mul_plots.pdf");


  c1->Clear();
  c1->SetLogy();
  //mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv4(1000,-25000,25000)","(ErrorFlag)==0 && rcdb_slug<4000 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1");
  mul_prune->Draw("rcdb_sign*eigen_lagr_asym_us_avg/1e-6>>Apv4(1000,-25000,25000)","(ErrorFlag&0xda7e6bff)==0 && rcdb_slug<4000 && rcdb_arm_flag==0 && rcdb_run_flag == 1 && rcdb_run_type == 1");
  TH1F* Apv4 = (TH1F*) gROOT->FindObject("Apv4");
  Apv4->SetNameTitle("Apv all currents, no cut","Apv, all currents, no miniruns cut;asym (ppm)");
  Apv4->Fit("gaus");

  c1->SaveAs("IncludeBMOD_Mul_plots.pdf)");

}

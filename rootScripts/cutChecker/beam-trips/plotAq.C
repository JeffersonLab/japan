void plotAq(Int_t flip_state=1) {

  TCanvas * c1 = new TCanvas();
  TString treename = "mul_trip";
  TChain *mul_tree = new TChain(treename);
  mul_tree->SetCacheSize(20*1024*1024);
  mul_tree->Add("respin2_rootfiles/test.root");
  mul_tree->Add("respin2_rootfiles/test_1.root");
  mul_tree->Add("respin2_rootfiles/test_2.root");
  mul_tree->LoadTree(-1);
  /*
  TFile * infile = new TFile("/chafs1/work1/apar/cameronc/rootfiles/respin1_prune_rootfiles/test.root");
  TTree *mul_tree;
  //TTree *eig_reg_tree_5_tr;
  infile->GetObject("mul_trip", mul_tree);
  */

  std::vector<Double_t> means;
  std::vector<Double_t> meanerrs;
  std::vector<Double_t> bins;

  Int_t nBins = 25;
  for (Int_t i = 0; i < nBins; i++) {
    mul_tree->Draw(Form("rcdb_sign*asym_bcm_an_us.hw_sum/1e-9"),Form("ErrorFlag==0 && SinceLastTripEnd>=%0.1f && SinceLastTripEnd<%0.1f && rcdb_flip_state==%d",i*250.0/nBins,(i+1)*250.0/nBins,flip_state),"goff");
    means.push_back(((TH1*)gROOT->FindObject("htemp"))->GetMean());
    meanerrs.push_back(((TH1*)gROOT->FindObject("htemp"))->GetMeanError());
    bins.push_back((Double_t)(i+0.5)*250.0/nBins);
  }
  TGraphErrors * tg1 = new TGraphErrors(nBins,&bins[0],&means[0],0,&meanerrs[0]);
  tg1->SetMarkerStyle(20);
  tg1->SetMarkerSize(0.5);
  tg1->Fit("pol0","QW","");
  tg1->Draw("ap");
  c1->SaveAs(Form("plots/Aq_SinceLastTripEnd_new_wienstate%d.pdf",flip_state));
}

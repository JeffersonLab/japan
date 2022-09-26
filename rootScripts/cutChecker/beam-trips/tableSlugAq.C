#include <fstream>
void tableSlugAq(Int_t flip_state = 1) {

  TCanvas * c1 = new TCanvas();
  c1->Divide(2,2);
  TString treename = "mul_trip";
  TChain *mul_tree = new TChain(treename);
  mul_tree->SetCacheSize(20*1024*1024);
  mul_tree->Add("respin2_rootfiles/test.root");
  mul_tree->Add("respin2_rootfiles/test_1.root");
  mul_tree->Add("respin2_rootfiles/test_2.root");
  mul_tree->LoadTree(-1);
  /*
  TFile * respin2_infile = new TFile("/chafs1/work1/apar/cameronc/rootfiles/respin2_prune_rootfiles/test.root");
  //TFile * respin1_infile = new TFile("/chafs1/work1/apar/cameronc/rootfiles/respin1_prune_rootfiles/test.root");
  //TFile * prompt_infile = new TFile("/chafs1/work1/apar/cameronc/rootfiles/prompt_prune_rootfiles/test.root");
  TTree *mul_tree;
  respin1_infile->GetObject("mul_trip", mul_tree);
  */

  std::vector<Double_t> oldPostTrip_means;
  std::vector<Double_t> oldPostTrip_meanerrs;
  std::vector<Double_t> newData_means;
  std::vector<Double_t> newData_meanerrs;
  std::vector<Double_t> newCut_means;
  std::vector<Double_t> newCut_meanerrs;
  std::vector<Double_t> oldCut_means;
  std::vector<Double_t> oldCut_meanerrs;
  std::vector<Double_t> bins;

  ofstream outfile0;
  outfile0.open(Form("plots/slugs_wienstate%d_respin2.csv",flip_state));

  Int_t nSlugs = 223;
  Int_t ndata = 0;
  outfile0 << "Slug, new cut mean, new cut mean error, old cut mean, old cut mean error, old cut post trip 250 mean, old cut post trip 250 mean error, new data only mean, new data only mean error" << std::endl;
  for (Int_t i = 100; i <= nSlugs; i++) {
    ndata = mul_tree->Draw(Form("rcdb_sign*asym_bcm_an_us.hw_sum/1e-9"),Form("ErrorFlag==0 && rcdb_slug==%d && rcdb_flip_state==%d",i,flip_state),"goff");
    TH1* newCutH1 = (TH1*)gROOT->FindObject("htemp");
    if (ndata !=0) {
      Double_t tmpNewCut_mean=0;
      Double_t tmpNewCut_meanerr=0;
      tmpNewCut_mean = newCutH1->GetMean();
      tmpNewCut_meanerr = newCutH1->GetMeanError();
      Printf("Slug %d: New cut mean = %f +- %f",i,tmpNewCut_mean,tmpNewCut_meanerr);
      outfile0 << i << "," << tmpNewCut_mean << "," << tmpNewCut_meanerr << "," ;
      newCut_means.push_back(tmpNewCut_mean);
      newCut_meanerrs.push_back(tmpNewCut_meanerr);
    }
    mul_tree->Draw(Form("rcdb_sign*asym_bcm_an_us.hw_sum/1e-9"),Form("ErrorFlag==0 && SinceLastTripEnd>250 && rcdb_slug==%d && rcdb_flip_state==%d",i,flip_state),"goff");
    TH1* oldCutH1 = (TH1*)gROOT->FindObject("htemp");
    if (ndata !=0) {
      Double_t tmpOldCut_mean=0;
      Double_t tmpOldCut_meanerr=0;
      tmpOldCut_mean = oldCutH1->GetMean();
      tmpOldCut_meanerr = oldCutH1->GetMeanError();
      Printf("Slug %d: Old cut mean = %f +- %f",i,tmpOldCut_mean,tmpOldCut_meanerr);
      outfile0 << tmpOldCut_mean << "," << tmpOldCut_meanerr << "," ;
      oldCut_means.push_back(tmpOldCut_mean);
      oldCut_meanerrs.push_back(tmpOldCut_meanerr);
    }
    mul_tree->Draw(Form("rcdb_sign*asym_bcm_an_us.hw_sum/1e-9"),Form("ErrorFlag==0 && SinceLastTripEnd>=250 && SinceLastTripEnd<500 && rcdb_slug==%d && rcdb_flip_state==%d",i,flip_state),"goff");
    TH1* oldPostTripH1 = (TH1*)gROOT->FindObject("htemp");
    if (ndata !=0) {
      Double_t tmpOldPostTrip_mean=0;
      Double_t tmpOldPostTrip_meanerr=0;
      tmpOldPostTrip_mean = oldPostTripH1->GetMean();
      tmpOldPostTrip_meanerr = oldPostTripH1->GetMeanError();
      Printf("Slug %d: Old Cut post trip 250 only mean = %f +- %f",i,tmpOldPostTrip_mean,tmpOldPostTrip_meanerr);
      outfile0 << tmpOldPostTrip_mean << "," << tmpOldPostTrip_meanerr << ",";
      oldPostTrip_means.push_back(tmpOldPostTrip_mean);
      oldPostTrip_meanerrs.push_back(tmpOldPostTrip_meanerr);
    }
    mul_tree->Draw(Form("rcdb_sign*asym_bcm_an_us.hw_sum/1e-9"),Form("ErrorFlag==0 && SinceLastTripEnd<250 && rcdb_slug==%d && rcdb_flip_state==%d",i,flip_state),"goff");
    TH1* newDataH1 = (TH1*)gROOT->FindObject("htemp");
    if (ndata !=0) {
      Double_t tmpNewData_mean=0;
      Double_t tmpNewData_meanerr=0;
      tmpNewData_mean = newDataH1->GetMean();
      tmpNewData_meanerr = newDataH1->GetMeanError();
      Printf("Slug %d: New data only mean = %f +- %f",i,tmpNewData_mean,tmpNewData_meanerr);
      outfile0 << tmpNewData_mean << "," << tmpNewData_meanerr << std::endl ;
      newData_means.push_back(tmpNewData_mean);
      newData_meanerrs.push_back(tmpNewData_meanerr);
      bins.push_back((Double_t)i);
    }
  }
  gStyle->SetOptFit(1111);
  TGraphErrors * tg1 = new TGraphErrors(bins.size(),&bins[0],&newData_means[0],0,&newData_meanerrs[0]);
  TGraphErrors * tg2 = new TGraphErrors(bins.size(),&bins[0],&oldPostTrip_means[0],0,&oldPostTrip_meanerrs[0]);
  TGraphErrors * tg3 = new TGraphErrors(bins.size(),&bins[0],&oldCut_means[0],0,&oldCut_meanerrs[0]);
  TGraphErrors * tg4 = new TGraphErrors(bins.size(),&bins[0],&newCut_means[0],0,&newCut_meanerrs[0]);
  c1->cd(1)->SetGrid();
  tg1->SetNameTitle("New Data 250 post cut included alone","New Data 250 post cut included alone;slug;Aq (ppb)");
  tg1->SetMarkerStyle(20);
  tg1->SetMarkerSize(0.5);
  tg1->Fit("pol0","QW","");
  tg1->Draw("ap");
  c1->cd(2)->SetGrid();
  tg2->SetNameTitle("Old Cut initial 250 post trip cut alone","Old Cut initial 250 post trip cut alone;slug;Aq (ppb)");
  tg2->SetMarkerStyle(20);
  tg2->SetMarkerSize(0.5);
  tg2->Fit("pol0","QW","");
  tg2->Draw("ap");
  c1->cd(3)->SetGrid();
  tg3->SetNameTitle("Old Cut all data","Old Cut all data;slug;Aq (ppb)");
  tg3->SetMarkerStyle(20);
  tg3->SetMarkerSize(0.5);
  tg3->Fit("pol0","QW","");
  tg3->Draw("ap");
  c1->cd(4)->SetGrid();
  tg4->SetNameTitle("New Cut all data","New Cut all data;slug;Aq (ppb)");
  tg4->SetMarkerStyle(20);
  tg4->SetMarkerSize(0.5);
  tg4->Fit("pol0","QW","");
  tg4->Draw("ap");
  c1->SaveAs(Form("plots/test_slugs_wien%d_respin2.pdf",flip_state));
  outfile0.close();
}

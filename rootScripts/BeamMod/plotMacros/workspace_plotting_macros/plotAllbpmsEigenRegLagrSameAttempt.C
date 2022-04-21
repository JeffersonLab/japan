#include <fstream>
#include <sstream>
#include <iostream>

/*TVirtualPad* loopMultiGraph(TPad* c1, TVirtualPad* p1, TChain* dit, TChain* slopes, int subpad = 1, std::string draw = "cyclenum", std::string cut = "", TString gh = "g", Int_t color = 4){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  TMultiGraph* mg = new TMultiGraph();
  TGraph* eq;

  //if (subpad == 1){
  //  dit->Scan("dit_13746.usl_1X:dit_15746.usl_1X:dit_13726.usl_1X:cyclenum","segment<4");
  //}
  //

  // less than OR both, as ONE is guaranteed to be == 0
  if (gh == "g"){
    dit->Draw(">>elist",Form("%s",cut.c_str()),"entrylist");
    TEntryList* elist = (TEntryList*)gDirectory->Get("elist");
    dit->SetEntryList(elist);
    for (entry=start;entry<end:entry++) {

      Int_t nen_eq = dit->Draw(Form("%s:cyclenum",draw.c_str()),Form("(%s)",cut.c_str()),"goff");//lp
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      eq = new TGraph(nen_eq,dit->GetV2(),dit->GetV1());
      //eq->SetNameTitle(Form("eq_%s_%d",draw.c_str(),subpad),Form("eq_%s_%d",draw.c_str(),subpad));
      eq->SetLineColor(color);
      eq->SetMarkerColor(color);
      eq->SetMarkerStyle(20);
      eq->SetMarkerSize(0.2);
    }
    gStyle->SetOptFit(1);
    mg->Fit("pol0");
    mg->SetNameTitle(Form("%s",draw.c_str()),Form("%s;Cycle Number;%s",draw.c_str(),draw.c_str()));
    mg->Draw("ap");
  }
  if (gh == "h"){
    gStyle->SetOptStat(1221);
    Int_t nen_eq = dit->Draw(Form("%s",draw.c_str()),Form("(%s)",cut.c_str()));
    ((TH1*)gROOT->FindObject("htemp"))->SetNameTitle(Form("%s",draw.c_str()),Form("%s;Counts",draw.c_str()));
  }
  return p1;
}*/

TVirtualPad* multiGraph(TPad* c1, TVirtualPad* p1, TChain* dit, int subpad = 1, TString draw = "run", std::string cut = "", TString gh = "g", Int_t color = 4, TString drawopt = ""){
  //if (drawopt == "") {
    //p1=c1->cd(subpad);
    //p1->SetGrid();
  //}

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  TGraph* eq;
  TGraphErrors* tgeq;

  //if (subpad == 1){
  //  dit->Scan("dit_13746.usl_1X:dit_15746.usl_1X:dit_13726.usl_1X:run","segment<4");
  //}
  //

  // less than OR both, as ONE is guaranteed to be == 0
  if (gh == "g"){
    Int_t nen_eq = dit->Draw(Form("%s:run",draw.Data()),Form("(%s)",cut.c_str()),"goff");//lp
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.Data(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.Data(),subpad));
    eq = new TGraph(nen_eq,dit->GetV2(),dit->GetV1());
    //eq->SetNameTitle(Form("eq_%s_%d",draw.Data(),subpad),Form("eq_%s_%d",draw.Data(),subpad));
    eq->SetLineColor(color);
    eq->SetMarkerColor(color);
    eq->SetMarkerStyle(20);
    eq->SetMarkerSize(0.2);
    gStyle->SetOptFit(1);
    //eq->Fit("pol0");
    eq->SetNameTitle(Form("%s %s",draw.Data(),drawopt.Data()),Form("%s;Run;%s",draw.Data(),draw.Data()));
    if (drawopt == "") {
      eq->Draw("ap");
    }
    else {
      eq->Draw(drawopt);
    }
  }
  if (gh == "ge"){
    Int_t nen_tgeq = dit->Draw(Form("%s:run:%s_err",draw.Data(),draw.Data()),Form("(%s)",cut.c_str()),"goff");//lp
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.Data(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.Data(),subpad));
    tgeq = new TGraphErrors(nen_tgeq,dit->GetV2(),dit->GetV1(),0,dit->GetV3());
    //tgeq->SetNameTitle(Form("eq_%s_%d",draw.Data(),subpad),Form("eq_%s_%d",draw.Data(),subpad));
    tgeq->SetLineColor(color);
    tgeq->SetMarkerColor(color);
    tgeq->SetMarkerStyle(20);
    tgeq->SetMarkerSize(0.2);
    gStyle->SetOptFit(1);
    tgeq->Fit("pol0");
    tgeq->SetNameTitle(Form("%s_err",draw.Data()),Form("%s;Run;%s",draw.Data(),draw.Data()));
    if (drawopt == "") {
      tgeq->Draw("ape");
    }
    else {
      tgeq->Draw(drawopt);
    }
    p1->Update();
    p1->Modified();
    c1->Update();
    c1->Modified();
  }
  if (gh == "h"){
    gStyle->SetOptStat(1221);
    Int_t nen_eq = dit->Draw(Form("%s",draw.Data()),Form("(%s)",cut.c_str()));
    ((TH1*)gROOT->FindObject("htemp"))->SetNameTitle(Form("%s_hist",draw.Data()),Form("%s;Counts",draw.Data()));
  }
  if (gh == "he"){
    gStyle->SetOptStat(1221);
    //Int_t nen_eq = dit->Draw(Form("%s",draw.Data()),Form("pow(%s_err,-2)*(%s && %s_err!=0)",draw.Data(),cut.c_str(),draw.Data()));
    Int_t nen_eq = dit->Draw(Form("%s",draw.Data()),Form("(%s)/pow(%s_err,2)",cut.c_str(),draw.Data()));
    // Get htemp's Mean, RMS
    std::ofstream outfile0;
    outfile0.open(Form("august_30_plots/residuals.txt"),std::ofstream::out | std::ofstream::app);
    outfile0 << draw << "," << ((TH1*)gROOT->FindObject("htemp"))->GetMean() << "," << ((TH1*)gROOT->FindObject("htemp"))->GetMeanError() << "," << ((TH1*)gROOT->FindObject("htemp"))->GetRMS() << std::endl; 
    outfile0.close();
    ((TH1*)gROOT->FindObject("htemp"))->SetNameTitle(Form("%s_hist_err",draw.Data()),Form("%s;Error Weighted Counts",draw.Data()));
    //((TH1*)gROOT->FindObject("htemp"))->SetNameTitle(Form("%s_hist",draw.Data()),Form("%s;Counts",draw.Data()));
  }
  return p1;
}

TVirtualPad* highlightedGraph(TCanvas* c1, TVirtualPad* p1, TChain* dit, int subpad = 1, std::string draw = "run:cyclenum", std::string cut = "", std::string cut2 = "0",std::string drawopts = "*", int color = 4, std::string cut3 = "0", int color3=2, std::string cut4 = "") {
  if (cut4 != "") {
    cut = cut+"&&"+cut4;
    cut2 = cut2+"&&"+cut4;
    if (cut3!= "") {
      cut3 = cut3+"&&"+cut4;
    }
  }

  p1=c1->cd(subpad);
  p1->SetGrid();
  dit->Draw(draw.c_str(),cut.c_str(),drawopts.c_str());

  if (p1->FindObject("Graph")) {
    ((TGraph*)p1->FindObject("Graph"))->SetName("full");
    dit->Draw(draw.c_str(),Form("%s",cut2.c_str()),Form("%ssame",drawopts.c_str()));
    if (p1->FindObject("Graph")) {
      ((TGraph*)p1->FindObject("Graph"))->SetName("color");
      ((TGraph*)p1->FindObject("color"))->SetMarkerColor(color);
    }
    if (cut3 != "") {
      dit->Draw(draw.c_str(),Form("%s",cut3.c_str()),Form("%ssame",drawopts.c_str()));
      if (p1->FindObject("Graph")) {
        ((TGraph*)p1->FindObject("Graph"))->SetName("color3");
        ((TGraph*)p1->FindObject("color3"))->SetMarkerColor(color3);
      }
      dit->Draw(draw.c_str(),Form("(%s) && (%s)",cut2.c_str(),cut3.c_str()),Form("%ssame",drawopts.c_str()));
      if (p1->FindObject("Graph")) {
        ((TGraph*)p1->FindObject("Graph"))->SetName("color2and3");
        ((TGraph*)p1->FindObject("color2and3"))->SetMarkerColor(color+color3);
      }
    }
  }
  if (p1->FindObject("htemp")) {
    ((TH1*)p1->FindObject("htemp"))->SetTitle(draw.c_str());
  }
  return p1;
}

TVirtualPad* optimizeMultiGraph(TPad* c1, TVirtualPad* p1, TChain* dit, int subpad = 1, std::string draw = "cyclenum", std::string cut = ""){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  TMultiGraph* mg = new TMultiGraph();
  TGraph* eq3542;
  TGraph* lt3542;
  TGraph* gt3542;
  TGraph* eq5342;
  TGraph* lt5342;
  TGraph* gt5342;
  TGraph* eq3524;
  TGraph* lt3524;
  TGraph* gt3524;

  //if (subpad == 1){
  //  dit->Scan("dit_13746.usl_1X:dit_15746.usl_1X:dit_13726.usl_1X:cyclenum","segment<4");
  //}
  //

  // less than OR both, as ONE is guaranteed to be == 0
  Int_t nen_eq3542 = dit->Draw(Form("dit_13746.%s:cyclenum",draw.c_str()),Form("(%s && abs(dit_13746.%s)>0.001)",cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13746 is ideal\n",nen_eq3542);
  if (nen_eq3542 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    eq3542 = new TGraph(nen_eq3542,dit->GetV2(),dit->GetV1());
    eq3542->SetNameTitle(Form("eq3542_%s_%d",draw.c_str(),subpad),Form("eq3542_%s_%d",draw.c_str(),subpad));
    eq3542->SetLineColor(4);
    eq3542->SetMarkerColor(4);
    eq3542->SetMarkerStyle(20);
    eq3542->SetMarkerSize(0.01);
    mg->Add(eq3542,"l");
    //legend->AddEntry(eq3542,"13746 non ideal","lp");
  }
  Int_t nen_lt3542 = dit->Draw(Form("dit_13746.%s:cyclenum",draw.c_str()),Form("((abs(dit_13746.%s) < 0.1+abs(dit_15746.%s)) || (abs(dit_13746.%s) < 0.1+abs(dit_13726.%s))) && (%s && abs(dit_13746.%s)>0.001)",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13746 is ideal\n",nen_lt3542);
  if (nen_lt3542 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("lt3542_%s_%d",draw.c_str(),subpad));
    //lt3542 = (TGraph*)p1->FindObject(Form("lt3542_%s_%d",draw.c_str(),subpad));
    lt3542 = new TGraph(nen_lt3542,dit->GetV2(),dit->GetV1());
    lt3542->SetNameTitle(Form("lt3542_%s_%d",draw.c_str(),subpad),Form("lt3542_%s_%d",draw.c_str(),subpad));
    lt3542->SetLineColor(4);
    lt3542->SetMarkerColor(4);
    lt3542->SetMarkerStyle(20);
    lt3542->SetMarkerSize(0.5);
    mg->Add(lt3542,"p");
    legend->AddEntry(lt3542,"13746 ideal","lp");
  }
  Int_t nen_gt3542 = dit->Draw(Form("dit_13746.%s:cyclenum",draw.c_str()),Form("((abs(dit_13746.%s) >= 0.1+abs(dit_15746.%s)) && (abs(dit_13746.%s) >= 0.1+abs(dit_13726.%s))) && (%s && abs(dit_13746.%s)>0.001)",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13746 is not ideal\n",nen_gt3542);
  if (nen_gt3542 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("gt3542_%s_%d",draw.c_str(),subpad));
    //gt3542 = (TGraph*)p1->FindObject(Form("gt3542_%s_%d",draw.c_str(),subpad));
    gt3542 = new TGraph(nen_gt3542,dit->GetV2(),dit->GetV1());
    gt3542->SetNameTitle(Form("gt3542_%s_%d",draw.c_str(),subpad),Form("gt3542_%s_%d",draw.c_str(),subpad));
    gt3542->SetLineColor(4);
    gt3542->SetMarkerColor(4);
    gt3542->SetMarkerStyle(24);
    gt3542->SetMarkerSize(0.5);
    mg->Add(gt3542,"p");
    legend->AddEntry(gt3542,"13746 non-ideal","lp");
  }
  Int_t nen_eq5342 = dit->Draw(Form("dit_15746.%s:cyclenum",draw.c_str()),Form("(%s && abs(dit_15746.%s)>0.001)",cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 15746 is ideal\n",nen_eq5342);
  if (nen_eq5342 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq5342_%s_%d",draw.c_str(),subpad));
    //eq5342 = (TGraph*)p1->FindObject(Form("eq5342_%s_%d",draw.c_str(),subpad));
    eq5342 = new TGraph(nen_eq5342,dit->GetV2(),dit->GetV1());
    eq5342->SetNameTitle(Form("eq5342_%s_%d",draw.c_str(),subpad),Form("eq5342_%s_%d",draw.c_str(),subpad));
    eq5342->SetLineColor(46);
    eq5342->SetMarkerColor(46);
    eq5342->SetMarkerStyle(20);
    eq5342->SetMarkerSize(0.01);
    mg->Add(eq5342,"l");
  }
  Int_t nen_lt5342 = dit->Draw(Form("dit_15746.%s:cyclenum",draw.c_str()),Form("((abs(dit_15746.%s) < 0.1+abs(dit_13746.%s)) || (abs(dit_15746.%s) < 0.1+abs(dit_13726.%s))) && (%s && abs(dit_15746.%s)>0.001)",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 15746 is ideal\n",nen_lt5342);
  if (nen_lt5342 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("lt5342_%s_%d",draw.c_str(),subpad));
    //lt5342 = (TGraph*)p1->FindObject(Form("lt5342_%s_%d",draw.c_str(),subpad));
    lt5342 = new TGraph(nen_lt5342,dit->GetV2(),dit->GetV1());
    lt5342->SetNameTitle(Form("lt5342_%s_%d",draw.c_str(),subpad),Form("lt5342_%s_%d",draw.c_str(),subpad));
    lt5342->SetLineColor(46);
    lt5342->SetMarkerColor(46);
    lt5342->SetMarkerStyle(20);
    lt5342->SetMarkerSize(0.5);
    mg->Add(lt5342,"p");
    legend->AddEntry(lt5342,"15746 ideal");
  }
  Int_t nen_gt5342 = dit->Draw(Form("dit_15746.%s:cyclenum",draw.c_str()),Form("((abs(dit_15746.%s) >= 0.1+abs(dit_13746.%s)) && (abs(dit_15746.%s) >= 0.1+abs(dit_13726.%s))) && (%s && abs(dit_15746.%s)>0.001)",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 15746 is not ideal\n",nen_gt5342);
  if (nen_gt5342 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("gt5342_%s_%d",draw.c_str(),subpad));
    //gt5342 = (TGraph*)p1->FindObject(Form("gt5342_%s_%d",draw.c_str(),subpad));
    gt5342 = new TGraph(nen_gt5342,dit->GetV2(),dit->GetV1());
    gt5342->SetNameTitle(Form("gt5342_%s_%d",draw.c_str(),subpad),Form("gt5342_%s_%d",draw.c_str(),subpad));
    gt5342->SetLineColor(46);
    gt5342->SetMarkerColor(46);
    gt5342->SetMarkerStyle(24);
    gt5342->SetMarkerSize(0.5);
    mg->Add(gt5342,"p");
    legend->AddEntry(gt5342,"15746 non-ideal");
  }
  Int_t nen_eq3524 = dit->Draw(Form("dit_13726.%s:cyclenum",draw.c_str()),Form("(%s && abs(dit_13726.%s)>0.001)",cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13726 is ideal\n",nen_eq3524);
  if (nen_eq3524 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3524_%s_%d",draw.c_str(),subpad));
    //eq3524 = (TGraph*)p1->FindObject(Form("eq3524_%s_%d",draw.c_str(),subpad));
    eq3524 = new TGraph(nen_eq3524,dit->GetV2(),dit->GetV1());
    eq3524->SetNameTitle(Form("eq3524_%s_%d",draw.c_str(),subpad),Form("eq3524_%s_%d",draw.c_str(),subpad));
    eq3524->SetLineColor(30);
    eq3524->SetMarkerColor(30);
    eq3524->SetMarkerStyle(20);
    eq3524->SetMarkerSize(0.01);
    mg->Add(eq3524,"l");
  }
  Int_t nen_lt3524 = dit->Draw(Form("dit_13726.%s:cyclenum",draw.c_str()),Form("((abs(dit_13726.%s) < 0.1+abs(dit_15746.%s)) || (abs(dit_13726.%s) < 0.1+abs(dit_13746.%s))) && (%s && abs(dit_13726.%s)>0.001)",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13726 is ideal\n",nen_lt3524);
  if (nen_lt3524 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("lt3524_%s_%d",draw.c_str(),subpad));
    //lt3524 = (TGraph*)p1->FindObject(Form("lt3524_%s_%d",draw.c_str(),subpad));
    lt3524 = new TGraph(nen_lt3524,dit->GetV2(),dit->GetV1());
    lt3524->SetNameTitle(Form("lt3524_%s_%d",draw.c_str(),subpad),Form("lt3524_%s_%d",draw.c_str(),subpad));
    lt3524->SetLineColor(30);
    lt3524->SetMarkerColor(30);
    lt3524->SetMarkerStyle(20);
    lt3524->SetMarkerSize(0.5);
    mg->Add(lt3524,"p");
    legend->AddEntry(lt3524,"13726 ideal");
  }
  Int_t nen_gt3524 = dit->Draw(Form("dit_13726.%s:cyclenum",draw.c_str()),Form("((abs(dit_13726.%s) >= 0.1+abs(dit_15746.%s)) && (abs(dit_13726.%s) >= 0.1+abs(dit_13746.%s))) && (%s && abs(dit_13726.%s)>0.001)",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13726 is not ideal\n",nen_gt3524);
  if (nen_gt3524 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("gt3524_%s_%d",draw.c_str(),subpad));
    //gt3524 = (TGraph*)p1->FindObject(Form("gt3524_%s_%d",draw.c_str(),subpad));
    gt3524 = new TGraph(nen_gt3524,dit->GetV2(),dit->GetV1());
    gt3524->SetNameTitle(Form("gt3524_%s_%d",draw.c_str(),subpad),Form("gt3524_%s_%d",draw.c_str(),subpad));
    gt3524->SetLineColor(30);
    gt3524->SetMarkerColor(30);
    gt3524->SetMarkerStyle(24);
    gt3524->SetMarkerSize(0.5);
    mg->Add(gt3524,"p");
    legend->AddEntry(gt3524,"13726 non-ideal");
  }
  //p1->Clear();

  mg->SetNameTitle(Form("%s comparisons",draw.c_str()),Form("%s comparisons;Cycle Number;%s",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  if (subpad%2==0) {
    legend->Draw();
  }
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

void plotAllbpmsEigenResiduals(TString scale = "part", TString ana = "_eigenvectors_double_sorted", TString type = "_fractional", TString do_err = "_err", TString bpms = ""){ // If "ana" == "" then does the plain BPMs case
  TString nbpms = "5bpms";
  if (bpms == "_allbpms") { // default case is "" for 5bpm suffix
    nbpms = "allbpms";
  }
  if (ana != "") {
    ana = ana+"_"+scale+"_avg"+bpms;
  }
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* dit                     = new TChain(Form("dit_%s_avgd_tagged_friendable",scale.Data()),"dit_with_all_the_friends");
  TChain* dit_13746               = new TChain(Form("dit_%s_avgd_tagged_friendable",scale.Data()));
  TChain* part_avgd_regression    = new TChain(Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),scale.Data()));
  TChain* part_avgd_lagrange      = new TChain(Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),scale.Data()));
  TChain* reference_regression    = new TChain(Form("mini_eigen_reg_%s_sorted",nbpms.Data()));
  TChain* segment_avgd_dit_slopes = new TChain(Form("dit_%s_avgd%s_friendable",scale.Data(),bpms.Data()));
  
  TString run_cycle = "run_avg";
  TString run_cycle_wise = "runwise";
  //TString run_cycle = "cyclewise";//"run_avg";
  //TString run_cycle_wise = "cyclewise";//"runwise";
  dit                    ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_%s%s_1X/respin2_All_dithering_slopes_%s_rcdb_segmented_pruned_usavg_tag_expanded.root",run_cycle.Data(),ana.Data(),run_cycle.Data()));
  // Assume run averaged analysis for now
  dit_13746              ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_%s%s_1X/respin2_All_dithering_slopes_%s_rcdb_segmented_pruned_usavg_tag_expanded.root",run_cycle.Data(),ana.Data(),run_cycle.Data()));
  part_avgd_regression   ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",do_err.Data(),scale.Data(),bpms.Data(),scale.Data())); // Make more of this
  part_avgd_lagrange     ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles%s_eigen_%s_avg%s/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",do_err.Data(),scale.Data(),bpms.Data(),scale.Data())); // Make more of this
  //part_avgd_regression   ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_%s_avg/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",scale.Data(),scale.Data())); // Make more of this
  //part_avgd_lagrange     ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_%s_avg/respin2-rcdb-parts-slowc-segments-%s_avg_sens-eigen_reg_lagr.root",scale.Data(),scale.Data())); // Make more of this
  reference_regression   ->AddFile(Form("../respin2_data/rcdb_burstwise_eigenvectors_sorted_sorted.root"));
  // Eigenvector BMOD only ever did run-averaged analysis
  segment_avgd_dit_slopes->AddFile(Form("../processed_respin2_data/All_dit_outputs_doing_segment_averaging_of_slopes_mini_friended.root"));
  // For safety build an index that assigns a run and cyclenum to define the entries (so we can compare different trees even with missing entries)
  //dit_13746->BuildIndex("run","cyclenum");
  //dit_15746->BuildIndex("run","cyclenum");
  //dit_13726->BuildIndex("run","cyclenum");
  // Set their names to be unique
  // Make them friends
  dit->AddFriend(dit_13746              ,     "dit_13746");
  dit->AddFriend(part_avgd_regression   ,Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),scale.Data()));
  dit->AddFriend(part_avgd_lagrange     ,Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),scale.Data()));
  dit->AddFriend(reference_regression   ,Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  dit->AddFriend(segment_avgd_dit_slopes,Form("dit_%s_avgd%s_friendable",scale.Data(),bpms.Data()));

  TString us_avg_frac = "";
  TString us_dd_frac = "";
  TString usl_frac = "";
  TString usr_frac = "";
  Int_t nmons = 5;
  if ( bpms.Contains("allbpms") ) {
    nmons = 12;
  }
  std::vector <TString> tmpDetNames;
  tmpDetNames = {"us_avg","us_dd","usl","usr"};
  TString tmpName = "";
  TString tmpErrName = "";
  TString tmpFrac = "";
  for (Int_t idet = 0 ; idet < tmpDetNames.size() ; idet++) {
    for (Int_t coil = 1 ; coil <=7 ; coil++ ) {
      if (type == "_fractional") {
        tmpFrac = Form("/%s_coil%d",tmpDetNames.at(idet).Data(),coil);
        //us_avg_frac = Form("/us_avg_coil%d",coil);
        //us_dd_frac = Form("/us_dd_coil%d",coil);
        //usl_frac = Form("/usl_coil%d",coil);
        //usr_frac = Form("/usr_coil%d",coil);
      }
      // Math of fractional residual error...
      // sigma = sigma of ((difference of raw - correction)/raw) = fractional * sqrt(pow(raw err/raw,2) + pow(err of(correction error)/(correction),2))
      //      where that correction error = sqrt(sum(pow(slope * sens errors),2))
      //      and correction itself = (1 - frac residual)/frac_variable     (frac variable = 1/raw)

        // Run avg dit data 
        tmpName = Form("(%s_coil%d-(0",tmpDetNames.at(idet).Data(),coil);
        tmpErrName = Form("abs(1/%s_coil%d)*sqrt((pow(%s_coil%d_err*(1-dit_13746.%s_coil%d_residual),2))",
            tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil);
        for (Int_t imon = 0 ; imon < nmons ; imon++) {
          tmpName = Form("%s+(dit_13746.%s_evMon%d*evMon%d_coil%d)",tmpName.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
          tmpErrName = Form("%s+pow(dit_13746.%s_evMon%d*evMon%d_coil%d_err,2)",tmpErrName.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
        }
        if (nmons == 12) {
          dit_13746->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s_coil%d/100",tmpDetNames.at(idet).Data(),coil));
          dit_13746->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s_coil%d_err/100",tmpDetNames.at(idet).Data(),coil));
        }
        else {
          dit_13746->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s))%s",tmpName.Data(),tmpFrac.Data()));
          dit_13746->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s)",tmpErrName.Data()));
        }

        // Part avg reg data
        tmpName = Form("(%s_coil%d-((0",tmpDetNames.at(idet).Data(),coil);
        tmpErrName = Form("abs(1/%s_coil%d)*sqrt((pow(%s_coil%d_err*(1-mini_eigen_reg_%s_%s_avg.%s_coil%d_residual),2))+(0",
            tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil,nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),coil);
        for (Int_t imon = 0 ; imon < nmons ; imon++) {
          tmpName = Form("%s+(mini_eigen_reg_%s_%s_avg.%s_evMon%d_new*evMon%d_coil%d)",tmpName.Data(),nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
          tmpErrName = Form("%s+pow(mini_eigen_reg_%s_%s_avg.%s_evMon%d_new*evMon%d_coil%d_err,2)",tmpErrName.Data(),nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
        }
        part_avgd_regression->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s))/1e-3)%s",tmpName.Data(),tmpFrac.Data()));
        part_avgd_regression->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s)/1e-6)",tmpErrName.Data()));
        // FIXME FIXME FIXME FIXME I added a NEW 1/e-6 to the residual error calculation because it didn't match relative UNITS!!!!

        // Part avg lagr data
        tmpName = Form("(%s_coil%d-((0",tmpDetNames.at(idet).Data(),coil);
        tmpErrName = Form("abs(1/%s_coil%d)*sqrt((pow(%s_coil%d_err*(1-mini_eigen_lagr_%s_%s_avg.%s_coil%d_residual),2))+(0",
            tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil,nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),coil);
        for (Int_t imon = 0 ; imon < nmons ; imon++) {
          tmpName = Form("%s+(mini_eigen_lagr_%s_%s_avg.%s_evMon%d_new*evMon%d_coil%d)",tmpName.Data(),nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
          tmpErrName = Form("%s+pow(mini_eigen_lagr_%s_%s_avg.%s_evMon%d_new*evMon%d_coil%d_err,2)",tmpErrName.Data(),nbpms.Data(),scale.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
        }
        part_avgd_lagrange->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s))/1e-3)%s",tmpName.Data(),tmpFrac.Data()));
        part_avgd_lagrange->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s)/1e-6)",tmpErrName.Data()));

        // Part avg segmentwise dit data
        tmpName = Form("(%s_coil%d-((0",tmpDetNames.at(idet).Data(),coil);
        tmpErrName = Form("abs(1/%s_coil%d)*sqrt((pow(%s_coil%d_err*(1-dit_%s_avgd%s_friendable.%s_coil%d_residual),2))+(0",
            tmpDetNames.at(idet).Data(),coil,tmpDetNames.at(idet).Data(),coil,scale.Data(),bpms.Data(),tmpDetNames.at(idet).Data(),coil);
        for (Int_t imon = 0 ; imon < nmons ; imon++) {
          tmpName = Form("%s+(dit_%s_avgd%s_friendable.%s_evMon%d_mean*evMon%d_coil%d)",tmpName.Data(),scale.Data(),bpms.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
          tmpErrName = Form("%s+pow(dit_%s_avgd%s_friendable.%s_evMon%d_mean*evMon%d_coil%d_err,2)",tmpErrName.Data(),scale.Data(),bpms.Data(),tmpDetNames.at(idet).Data(),imon,imon,coil);
        }
        if (nmons == 12) {
          segment_avgd_dit_slopes->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s_coil%d/100",tmpDetNames.at(idet).Data(),coil));
          segment_avgd_dit_slopes->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s_coil%d_err/100",tmpDetNames.at(idet).Data(),coil));
        }
        else {
          segment_avgd_dit_slopes->SetAlias(Form("%s_coil%d_residual",tmpDetNames.at(idet).Data(),coil),Form("%s)))%s",tmpName.Data(),tmpFrac.Data()));
          segment_avgd_dit_slopes->SetAlias(Form("%s_coil%d_residual_err",tmpDetNames.at(idet).Data(),coil),Form("%s))",tmpErrName.Data()));
        }

      }
    }

    //reference_regression->SetAlias(Form("us_avg_coil%d_residual",coil),Form("(us_avg_coil%d-((mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon0*evMon0_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon1*evMon1_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon2*evMon2_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon3*evMon3_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon4*evMon4_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,us_avg_frac.Data()));
    //reference_regression->SetAlias(Form("us_dd_coil%d_residual",coil),Form("(us_dd_coil%d-((mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon0*evMon0_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon1*evMon1_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon2*evMon2_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon3*evMon3_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon4*evMon4_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,us_dd_frac.Data()));
    //reference_regression->SetAlias(Form("usl_coil%d_residual",coil),Form("(usl_coil%d-((mini_reference_eigen_reg_5bpms_sorted.usl_evMon0*evMon0_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usl_evMon1*evMon1_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usl_evMon2*evMon2_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usl_evMon3*evMon3_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usl_evMon4*evMon4_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,usl_frac.Data()));
    //reference_regression->SetAlias(Form("usr_coil%d_residual",coil),Form("(usr_coil%d-((mini_reference_eigen_reg_5bpms_sorted.usr_evMon0*evMon0_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usr_evMon1*evMon1_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usr_evMon2*evMon2_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usr_evMon3*evMon3_coil%d)+(mini_reference_eigen_reg_5bpms_sorted.usr_evMon4*evMon4_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,usr_frac.Data()));
    //reference_regression->SetAlias(Form("us_avg_coil%d_residual_err",coil),Form("abs(1/us_avg_coil%d)*sqrt((pow(us_avg_coil%d_err*(1-mini_reference_eigen_reg_5bpms_sorted.us_avg_coil%d_residual),2))+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon0_new*evMon0_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon1_new*evMon1_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon2_new*evMon2_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon3_new*evMon3_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_avg_evMon4_new*evMon4_coil%d_err,2))",coil,coil,coil,coil,coil,coil,coil,coil));
    //reference_regression->SetAlias(Form("us_dd_coil%d_residual_err",coil),Form("abs(1/us_dd_coil%d)*sqrt((pow(us_dd_coil%d_err*(1-mini_reference_eigen_reg_5bpms_sorted.us_dd_coil%d_residual),2))+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon0_new*evMon0_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon1_new*evMon1_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon2_new*evMon2_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon3_new*evMon3_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.us_dd_evMon4_new*evMon4_coil%d_err,2))",coil,coil,coil,coil,coil,coil,coil,coil));
    //reference_regression->SetAlias(Form("usl_coil%d_residual_err",coil),Form("abs(1/usl_coil%d)*sqrt((pow(usl_coil%d_err*(1-mini_reference_eigen_reg_5bpms_sorted.usl_coil%d_residual),2))+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon0_new*evMon0_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon1_new*evMon1_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon2_new*evMon2_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon3_new*evMon3_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usl_evMon4_new*evMon4_coil%d_err,2))",coil,coil,coil,coil,coil,coil,coil,coil));
    //reference_regression->SetAlias(Form("usr_coil%d_residual_err",coil),Form("abs(1/usr_coil%d)*sqrt((pow(usr_coil%d_err*(1-mini_reference_eigen_reg_5bpms_sorted.usr_coil%d_residual),2))+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon0_new*evMon0_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon1_new*evMon1_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon2_new*evMon2_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon3_new*evMon3_coil%d_err,2)+pow(mini_reference_eigen_reg_5bpms_sorted.usr_evMon4_new*evMon4_coil%d_err,2))",coil,coil,coil,coil,coil,coil,coil,coil));

  //// Net residual (average of all residuals, error weighted, over all coils per minirun)
  dit_13746->SetAlias("us_avg_net_residual","((pow(dit_13746.us_avg_coil1_residual_err,-2)*dit_13746.us_avg_coil1_residual)+(pow(dit_13746.us_avg_coil2_residual_err,-2)*dit_13746.us_avg_coil2_residual)+(pow(dit_13746.us_avg_coil3_residual_err,-2)*dit_13746.us_avg_coil3_residual)+(pow(dit_13746.us_avg_coil4_residual_err,-2)*dit_13746.us_avg_coil4_residual)+(pow(dit_13746.us_avg_coil5_residual_err,-2)*dit_13746.us_avg_coil5_residual)+(pow(dit_13746.us_avg_coil6_residual_err,-2)*dit_13746.us_avg_coil6_residual)+(pow(dit_13746.us_avg_coil7_residual_err,-2)*dit_13746.us_avg_coil7_residual))/(pow(dit_13746.us_avg_coil1_residual_err,-2)+pow(dit_13746.us_avg_coil2_residual_err,-2)+pow(dit_13746.us_avg_coil3_residual_err,-2)+pow(dit_13746.us_avg_coil4_residual_err,-2)+pow(dit_13746.us_avg_coil5_residual_err,-2)+pow(dit_13746.us_avg_coil6_residual_err,-2)+pow(dit_13746.us_avg_coil7_residual_err,-2))");
  dit_13746->SetAlias("us_dd_net_residual","((pow(dit_13746.us_dd_coil1_residual_err,-2)*dit_13746.us_dd_coil1_residual)+(pow(dit_13746.us_dd_coil2_residual_err,-2)*dit_13746.us_dd_coil2_residual)+(pow(dit_13746.us_dd_coil3_residual_err,-2)*dit_13746.us_dd_coil3_residual)+(pow(dit_13746.us_dd_coil4_residual_err,-2)*dit_13746.us_dd_coil4_residual)+(pow(dit_13746.us_dd_coil5_residual_err,-2)*dit_13746.us_dd_coil5_residual)+(pow(dit_13746.us_dd_coil6_residual_err,-2)*dit_13746.us_dd_coil6_residual)+(pow(dit_13746.us_dd_coil7_residual_err,-2)*dit_13746.us_dd_coil7_residual))/(pow(dit_13746.us_dd_coil1_residual_err,-2)+pow(dit_13746.us_dd_coil2_residual_err,-2)+pow(dit_13746.us_dd_coil3_residual_err,-2)+pow(dit_13746.us_dd_coil4_residual_err,-2)+pow(dit_13746.us_dd_coil5_residual_err,-2)+pow(dit_13746.us_dd_coil6_residual_err,-2)+pow(dit_13746.us_dd_coil7_residual_err,-2))");
  dit_13746->SetAlias("usl_net_residual","((pow(dit_13746.usl_coil1_residual_err,-2)*dit_13746.usl_coil1_residual)+(pow(dit_13746.usl_coil2_residual_err,-2)*dit_13746.usl_coil2_residual)+(pow(dit_13746.usl_coil3_residual_err,-2)*dit_13746.usl_coil3_residual)+(pow(dit_13746.usl_coil4_residual_err,-2)*dit_13746.usl_coil4_residual)+(pow(dit_13746.usl_coil5_residual_err,-2)*dit_13746.usl_coil5_residual)+(pow(dit_13746.usl_coil6_residual_err,-2)*dit_13746.usl_coil6_residual)+(pow(dit_13746.usl_coil7_residual_err,-2)*dit_13746.usl_coil7_residual))/(pow(dit_13746.usl_coil1_residual_err,-2)+pow(dit_13746.usl_coil2_residual_err,-2)+pow(dit_13746.usl_coil3_residual_err,-2)+pow(dit_13746.usl_coil4_residual_err,-2)+pow(dit_13746.usl_coil5_residual_err,-2)+pow(dit_13746.usl_coil6_residual_err,-2)+pow(dit_13746.usl_coil7_residual_err,-2))");
  dit_13746->SetAlias("usr_net_residual","((pow(dit_13746.usr_coil1_residual_err,-2)*dit_13746.usr_coil1_residual)+(pow(dit_13746.usr_coil2_residual_err,-2)*dit_13746.usr_coil2_residual)+(pow(dit_13746.usr_coil3_residual_err,-2)*dit_13746.usr_coil3_residual)+(pow(dit_13746.usr_coil4_residual_err,-2)*dit_13746.usr_coil4_residual)+(pow(dit_13746.usr_coil5_residual_err,-2)*dit_13746.usr_coil5_residual)+(pow(dit_13746.usr_coil6_residual_err,-2)*dit_13746.usr_coil6_residual)+(pow(dit_13746.usr_coil7_residual_err,-2)*dit_13746.usr_coil7_residual))/(pow(dit_13746.usr_coil1_residual_err,-2)+pow(dit_13746.usr_coil2_residual_err,-2)+pow(dit_13746.usr_coil3_residual_err,-2)+pow(dit_13746.usr_coil4_residual_err,-2)+pow(dit_13746.usr_coil5_residual_err,-2)+pow(dit_13746.usr_coil6_residual_err,-2)+pow(dit_13746.usr_coil7_residual_err,-2))");
  dit_13746->SetAlias("us_avg_net_residual_err","(pow(dit_13746.us_avg_coil1_residual_err,-2)+pow(dit_13746.us_avg_coil2_residual_err,-2)+pow(dit_13746.us_avg_coil3_residual_err,-2)+pow(dit_13746.us_avg_coil4_residual_err,-2)+pow(dit_13746.us_avg_coil5_residual_err,-2)+pow(dit_13746.us_avg_coil6_residual_err,-2)+pow(dit_13746.us_avg_coil7_residual_err,-2))");
  dit_13746->SetAlias("us_dd_net_residual_err","(pow(dit_13746.us_dd_coil1_residual_err,-2)+pow(dit_13746.us_dd_coil2_residual_err,-2)+pow(dit_13746.us_dd_coil3_residual_err,-2)+pow(dit_13746.us_dd_coil4_residual_err,-2)+pow(dit_13746.us_dd_coil5_residual_err,-2)+pow(dit_13746.us_dd_coil6_residual_err,-2)+pow(dit_13746.us_dd_coil7_residual_err,-2))");
  dit_13746->SetAlias("usl_net_residual_err","(pow(dit_13746.usl_coil1_residual_err,-2)+pow(dit_13746.usl_coil2_residual_err,-2)+pow(dit_13746.usl_coil3_residual_err,-2)+pow(dit_13746.usl_coil4_residual_err,-2)+pow(dit_13746.usl_coil5_residual_err,-2)+pow(dit_13746.usl_coil6_residual_err,-2)+pow(dit_13746.usl_coil7_residual_err,-2))");
  dit_13746->SetAlias("usr_net_residual_err","(pow(dit_13746.usr_coil1_residual_err,-2)+pow(dit_13746.usr_coil2_residual_err,-2)+pow(dit_13746.usr_coil3_residual_err,-2)+pow(dit_13746.usr_coil4_residual_err,-2)+pow(dit_13746.usr_coil5_residual_err,-2)+pow(dit_13746.usr_coil6_residual_err,-2)+pow(dit_13746.usr_coil7_residual_err,-2))");

  part_avgd_regression->SetAlias("us_avg_net_residual","((pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual))/(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("us_dd_net_residual","((pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual))/(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("usl_net_residual","((pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual))/(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("usr_net_residual","((pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual)+(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2)*mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual))/(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("us_avg_net_residual_err","(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("us_dd_net_residual_err","(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("usl_net_residual_err","(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2))");
  part_avgd_regression->SetAlias("usr_net_residual_err","(pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)+pow(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2))");

  part_avgd_lagrange->SetAlias("us_avg_net_residual","((pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual))/(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("us_dd_net_residual","((pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual))/(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("usl_net_residual","((pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual))/(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("usr_net_residual","((pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual)+(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2)*mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual))/(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("us_avg_net_residual_err","(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("us_dd_net_residual_err","(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("usl_net_residual_err","(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual_err,-2))");
  part_avgd_lagrange->SetAlias("usr_net_residual_err","(pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual_err,-2)+pow(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual_err,-2))");

  //reference_regression->SetAlias("us_avg_net_residual","sqrt(pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil1_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil2_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil3_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil4_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil5_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil6_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_avg_coil7_residual,2))");
  //reference_regression->SetAlias("us_dd_net_residual","sqrt(pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil1_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil2_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil3_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil4_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil5_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil6_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.us_dd_coil7_residual,2))");
  //reference_regression->SetAlias("usl_net_residual","sqrt(pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil1_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil2_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil3_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil4_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil5_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil6_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil7_residual,2))");
  //reference_regression->SetAlias("usr_net_residual","sqrt(pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil1_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil2_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil3_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil4_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil5_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil6_residual,2)+pow(mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil7_residual,2))");

  segment_avgd_dit_slopes->SetAlias("us_avg_net_residual","((pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual))/(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("us_dd_net_residual","((pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual))/(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("usl_net_residual","((pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual))/(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("usr_net_residual","((pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual)+(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err,-2)*dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual))/(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("us_avg_net_residual_err","(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("us_dd_net_residual_err","(pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("usl_net_residual_err","(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err,-2))");
  segment_avgd_dit_slopes->SetAlias("usr_net_residual_err","(pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err,-2)+pow(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err,-2))");


  // FIXME
  ////// Define slope differences here - chose one baseline (dit..._avgd_friendable perhaps?) and compare the slope of X other analyses to it
  for (Int_t xx = 0 ; xx<nmons ; xx++) {
    dit_13746->SetAlias(Form("us_avg_evMon%d_run_dit_dit_diff",xx),Form("dit_13746.us_avg_evMon%d-dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_evMon%d_mean",xx,xx));
    dit_13746->SetAlias(Form("us_dd_evMon%d_run_dit_dit_diff", xx),Form("dit_13746.us_dd_evMon%d-dit_"+ scale+"_avgd"+bpms+"_friendable.us_dd_evMon%d_mean",xx,xx));
    dit_13746->SetAlias(Form("usl_evMon%d_run_dit_dit_diff",   xx),Form("dit_13746.usl_evMon%d-dit_"+   scale+"_avgd"+bpms+"_friendable.usl_evMon%d_mean",xx,xx));
    dit_13746->SetAlias(Form("usr_evMon%d_run_dit_dit_diff",   xx),Form("dit_13746.usr_evMon%d-dit_"+   scale+"_avgd"+bpms+"_friendable.usr_evMon%d_mean",xx,xx));

    part_avgd_lagrange->SetAlias(Form("us_avg_evMon%d_lagr_dit_diff",xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon%d_new/1e-3)-dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_evMon%d_mean",xx,xx));
    part_avgd_lagrange->SetAlias(Form("us_dd_evMon%d_lagr_dit_diff", xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon%d_new/1e-3)-dit_"+ scale+"_avgd"+bpms+"_friendable.us_dd_evMon%d_mean",xx,xx));
    part_avgd_lagrange->SetAlias(Form("usl_evMon%d_lagr_dit_diff",   xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon%d_new/1e-3)-dit_"+   scale+"_avgd"+bpms+"_friendable.usl_evMon%d_mean",xx,xx));
    part_avgd_lagrange->SetAlias(Form("usr_evMon%d_lagr_dit_diff",   xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon%d_new/1e-3)-dit_"+   scale+"_avgd"+bpms+"_friendable.usr_evMon%d_mean",xx,xx));

    part_avgd_lagrange->SetAlias(Form("us_avg_evMon%d_lagr_reg_diff",xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon%d_new/1e-3)-(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon%d_new/1e-3)",xx,xx));
    part_avgd_lagrange->SetAlias(Form("us_dd_evMon%d_lagr_reg_diff", xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon%d_new/1e-3)-(mini_eigen_reg_"+nbpms+"_"+ scale+"_avg.us_dd_evMon%d_new/1e-3)",xx,xx));
    part_avgd_lagrange->SetAlias(Form("usl_evMon%d_lagr_reg_diff",   xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon%d_new/1e-3)-(mini_eigen_reg_"+nbpms+"_"+   scale+"_avg.usl_evMon%d_new/1e-3)",xx,xx));
    part_avgd_lagrange->SetAlias(Form("usr_evMon%d_lagr_reg_diff",   xx),Form("(mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon%d_new/1e-3)-(mini_eigen_reg_"+nbpms+"_"+   scale+"_avg.usr_evMon%d_new/1e-3)",xx,xx));

    part_avgd_regression->SetAlias(Form("us_avg_evMon%d_reg_dit_diff",xx),Form("(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon%d_new/1e-3)-dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_evMon%d_mean",xx,xx));
    part_avgd_regression->SetAlias(Form("us_dd_evMon%d_reg_dit_diff", xx),Form("(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_evMon%d_new/1e-3)-dit_"+ scale+"_avgd"+bpms+"_friendable.us_dd_evMon%d_mean",xx,xx));
    part_avgd_regression->SetAlias(Form("usl_evMon%d_reg_dit_diff",   xx),Form("(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_evMon%d_new/1e-3)-dit_"+   scale+"_avgd"+bpms+"_friendable.usl_evMon%d_mean",xx,xx));
    part_avgd_regression->SetAlias(Form("usr_evMon%d_reg_dit_diff",   xx),Form("(mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_evMon%d_new/1e-3)-dit_"+   scale+"_avgd"+bpms+"_friendable.usr_evMon%d_mean",xx,xx));
  }

  // Cut include "run == localtree.run" to make the comparisons only for runs with the sensitivity data valid (skip the no-bmod runs for this calculation)
  std::string cut = "(rcdb_run_type == 1 && rcdb_run_flag ==1 && flag==1 && rcdb_slug >= 100 && rcdb_slug<4000 && dit_true_data == 1)"; // Ignore dit_true_data to do a "all data, regardless of if there is meaningful sensitivity underneath" analysis. Also, add in minirun_n == 0 (requires dit->minirun expand) to only get one minirun per run, otherwise it will automatically apply the sensitivities from the run_avg to every minirunwise-slope in the slope tree (which is... not exactly what we want).


  // Do some Tree->SetAlias() around everywhere
  //std::string cut3 = "((dit_13746.usl_coil2-((dit_13746.usl_1X*dit_13746.1X_coil2)+(dit_13746.usl_4aY*dit_13746.4aY_coil2)+(dit_13746.usl_4eX*dit_13746.4eX_coil2)+(dit_13746.usl_4eY*dit_13746.4eY_coil2)+(dit_13746.usl_12X*dit_13746.12X_coil2))) < (dit_15746.usl_coil2-((dit_15746.usl_1X*dit_15746.1X_coil2)+(dit_15746.usl_4aY*dit_15746.4aY_coil2)+(dit_15746.usl_4eX*dit_15746.4eX_coil2)+(dit_15746.usl_4eY*dit_15746.4eY_coil2)+(dit_15746.usl_12X*dit_15746.12X_coil2))))";

  TString modanaz = "";
  if (type == "_fractional") {
    modanaz = "Fractional ";
  }

  std::ofstream outfile0;
  outfile0.open(Form("august_30_plots/residuals.txt"),std::ofstream::out | std::ofstream::app);
  outfile0 << "Analysis and draw,Residual TH1 weighted mean,Residual TH1 error,Residual RMS" << std::endl; 
  outfile0.close();

  TString anaz = "US Avg, DD "+modanaz+"Residual sensitivities for coil set 13746, "+scale+"-avgd eigenvector run_avg bmod slope calculation (only good run_avg sensitivity data included)";
  std::string pdfname = Form("august_30_plots/method_comparison_%s%s%s_%s%s.pdf",run_cycle_wise.Data(),type.Data(),ana.Data(),nbpms.Data(),do_err.Data());
  std::string pdfresidualsname = Form("august_30_plots/method_comparison_all_eigen_residuals_%s%s%s_%s%s.pdf",run_cycle_wise.Data(),type.Data(),ana.Data(),nbpms.Data(),do_err.Data());
  std::string pdfslopesname = Form("august_30_plots/method_comparison_all_eigen_slopes_%s%s%s_%s%s.pdf",run_cycle_wise.Data(),type.Data(),ana.Data(),nbpms.Data(),do_err.Data());

  TText *label = new TText(0.0,0.005,anaz.Data());
  label->SetTextFont(23);
  label->SetTextSize(14);
  label->SetNDC();

  TVirtualPad* p1;
  /*
  FIXME FIXME TEMPORARY COMMENT OUT AUG 30
  TCanvas* c1_1 = new TCanvas();
  c1_1->cd();
  c1_1->SetTitle(anaz.Data());
  c1_1->SetName(anaz.Data());
  c1_1->Divide(4,7);
  p1=multiGraph(c1_1,p1,dit,1, "dit_13746.us_avg_coil1_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4); // miniprinted cut needs to be "run" == "run_number" if you want to only look at runs that had BMOD data and only look at the BMOD data inside of those runs (otherwise you'll multi-count those runs).
  p1=multiGraph(c1_1,p1,dit,2, "dit_13746.us_avg_coil1_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_1,p1,dit,3, "dit_13746.us_dd_coil1_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_1,p1,dit,4, "dit_13746.us_dd_coil1_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_1,p1,dit,5, "dit_13746.us_avg_coil2_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",2);
  p1=multiGraph(c1_1,p1,dit,6, "dit_13746.us_avg_coil2_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",2);
  p1=multiGraph(c1_1,p1,dit,7, "dit_13746.us_dd_coil2_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",2); // Add minirun_n == 0 cut to dithering run_avg or segment averaged slopes (since they don't care about minirun number and that would be duplicating a run needlessly, right?)
  p1=multiGraph(c1_1,p1,dit,8, "dit_13746.us_dd_coil2_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",2);
  p1=multiGraph(c1_1,p1,dit,9, "dit_13746.us_avg_coil3_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_1,p1,dit,10,"dit_13746.us_avg_coil3_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_1,p1,dit,11,"dit_13746.us_dd_coil3_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_1,p1,dit,12,"dit_13746.us_dd_coil3_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_1,p1,dit,13,"dit_13746.us_avg_coil4_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_1,p1,dit,14,"dit_13746.us_avg_coil4_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_1,p1,dit,15,"dit_13746.us_dd_coil4_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_1,p1,dit,16,"dit_13746.us_dd_coil4_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_1,p1,dit,17,"dit_13746.us_avg_coil5_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",2);
  p1=multiGraph(c1_1,p1,dit,18,"dit_13746.us_avg_coil5_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",2);
  p1=multiGraph(c1_1,p1,dit,19,"dit_13746.us_dd_coil5_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",2);
  p1=multiGraph(c1_1,p1,dit,20,"dit_13746.us_dd_coil5_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",2);
  p1=multiGraph(c1_1,p1,dit,21,"dit_13746.us_avg_coil6_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_1,p1,dit,22,"dit_13746.us_avg_coil6_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_1,p1,dit,23,"dit_13746.us_dd_coil6_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_1,p1,dit,24,"dit_13746.us_dd_coil6_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_1,p1,dit,25,"dit_13746.us_avg_coil7_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_1,p1,dit,26,"dit_13746.us_avg_coil7_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_1,p1,dit,27,"dit_13746.us_dd_coil7_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_1,p1,dit,28,"dit_13746.us_dd_coil7_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  c1_1->cd();
  label->Draw("same");
  c1_1->SaveAs(Form("%s(",pdfresidualsname.c_str()));
  c1_1->SaveAs(Form("%s(",pdfname.c_str()));

  TCanvas* c1_lr_c1_1 = new TCanvas();
  c1_lr_c1_1->cd();
  c1_lr_c1_1->SetTitle(anaz.Data());
  c1_lr_c1_1->SetName(anaz.Data());
  c1_lr_c1_1->Divide(4,7);
  //p1=highlightedGraph(c1_lr_c1_1,p1,dit_13746,1,"(dit_13746.usl_coil2-((dit_13746.usl_1X*dit_13746.1X_coil2)+(dit_13746.usl_4aY*dit_13746.4aY_coil2)+(dit_13746.usl_4eX*dit_13746.4eX_coil2)+(dit_13746.usl_4eY*dit_13746.4eY_coil2)+(dit_13746.usl_12X*dit_13746.12X_coil2))):dit_13746.cyclenum", cut,"(dit_13746.flag==0)","*",2,cut3,4,"(dit_13746.usl_coil2-((dit_13746.usl_1X*dit_13746.1X_coil2)+(dit_13746.usl_4aY*dit_13746.4aY_coil2)+(dit_13746.usl_4eX*dit_13746.4eX_coil2)+(dit_13746.usl_4eY*dit_13746.4eY_coil2)+(dit_13746.usl_12X*dit_13746.12X_coil2))) != 1.0e6");
  p1=multiGraph(c1_lr_c1_1,p1,dit,1, "dit_13746.usl_coil1_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4); // miniprinted cut needs to be "run" == "run_number" if you want to only look at runs that had BMOD data and only look at the BMOD data inside of those runs (otherwise you'll multi-count those runs).
  p1=multiGraph(c1_lr_c1_1,p1,dit,2, "dit_13746.usl_coil1_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,3, "dit_13746.usr_coil1_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,4, "dit_13746.usr_coil1_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,5, "dit_13746.usl_coil2_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",2);
  p1=multiGraph(c1_lr_c1_1,p1,dit,6, "dit_13746.usl_coil2_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",2);
  p1=multiGraph(c1_lr_c1_1,p1,dit,7, "dit_13746.usr_coil2_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",2); // Add minirun_n == 0 cut to dithering run_avg or segment averaged slopes (since they don't care about minirun number and that would be duplicating a run needlessly, right?)
  p1=multiGraph(c1_lr_c1_1,p1,dit,8, "dit_13746.usr_coil2_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",2);
  p1=multiGraph(c1_lr_c1_1,p1,dit,9, "dit_13746.usl_coil3_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,10,"dit_13746.usl_coil3_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,11,"dit_13746.usr_coil3_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,12,"dit_13746.usr_coil3_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,13,"dit_13746.usl_coil4_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,14,"dit_13746.usl_coil4_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,15,"dit_13746.usr_coil4_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,16,"dit_13746.usr_coil4_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,17,"dit_13746.usl_coil5_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",2);
  p1=multiGraph(c1_lr_c1_1,p1,dit,18,"dit_13746.usl_coil5_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",2);
  p1=multiGraph(c1_lr_c1_1,p1,dit,19,"dit_13746.usr_coil5_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",2);
  p1=multiGraph(c1_lr_c1_1,p1,dit,20,"dit_13746.usr_coil5_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",2);
  p1=multiGraph(c1_lr_c1_1,p1,dit,21,"dit_13746.usl_coil6_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,22,"dit_13746.usl_coil6_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,23,"dit_13746.usr_coil6_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,24,"dit_13746.usr_coil6_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,25,"dit_13746.usl_coil7_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,26,"dit_13746.usl_coil7_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,27,"dit_13746.usr_coil7_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_lr_c1_1,p1,dit,28,"dit_13746.usr_coil7_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);
  //p1=highlightedGraph(c1_lr_c1_1,p1,dit,5,"run:cyclenum",cut,"(flag==0)","*",2,"(flag==1)",4); // Red err cutout
  //p1=highlightedGraph(c1_lr_c1_1,p1,dit,6,"segment:cyclenum",cut,"(flag==0)","*",2,"(flag==1)",4);
  //p1=highlightedGraph(c1_lr_c1_1,p1,dit,9,"run:cyclenum",cut,"(flag==2)","*",2); // Black
  //p1=highlightedGraph(c1_lr_c1_1,p1,dit,10,"segment:cyclenum",cut,"(flag==2)","*",2); // Black
  c1_lr_c1_1->cd();
  label->Draw("same");
  c1_lr_c1_1->SaveAs(Form("%s",pdfresidualsname.c_str()));
  c1_lr_c1_1->SaveAs(Form("%s",pdfname.c_str()));

  anaz = "US Avg, DD "+modanaz+"Residual sensitivities for "+scale+"-avgd eigenvector regression slopes (only good run_avg sensitivity data included)";
  TCanvas* c1_2 = new TCanvas();
  c1_2->cd();
  c1_2->SetTitle(anaz.Data());
  c1_2->SetName(anaz.Data());
  c1_2->Divide(4,7);
  p1=multiGraph(c1_2,p1,dit,1, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,2, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2,p1,dit,3, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,4, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2,p1,dit,5, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual",cut + " && rcdb_arm_flag==0","ge",2);
  p1=multiGraph(c1_2,p1,dit,6, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual",cut + " && rcdb_arm_flag==0","he",2);
  p1=multiGraph(c1_2,p1,dit,7, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual" ,cut + " && rcdb_arm_flag==0","ge",2);
  p1=multiGraph(c1_2,p1,dit,8, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual" ,cut + " && rcdb_arm_flag==0","he",2);
  p1=multiGraph(c1_2,p1,dit,9, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,10,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2,p1,dit,11,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,12,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2,p1,dit,13,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,14,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2,p1,dit,15,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,16,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2,p1,dit,17,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual",cut + " && rcdb_arm_flag==0","ge",2);
  p1=multiGraph(c1_2,p1,dit,18,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual",cut + " && rcdb_arm_flag==0","he",2);
  p1=multiGraph(c1_2,p1,dit,19,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual" ,cut + " && rcdb_arm_flag==0","ge",2);
  p1=multiGraph(c1_2,p1,dit,20,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual" ,cut + " && rcdb_arm_flag==0","he",2);
  p1=multiGraph(c1_2,p1,dit,21,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,22,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2,p1,dit,23,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,24,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2,p1,dit,25,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,26,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2,p1,dit,27,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2,p1,dit,28,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  c1_2->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c1_2->SaveAs(Form("%s",pdfresidualsname.c_str()));
  c1_2->SaveAs(Form("%s",pdfname.c_str()));

  anaz = "USL, R "+modanaz+"Residual sensitivities for "+scale+"-avgd eigenvector regression slopes (only good run_avg sensitivity data included)";
  TCanvas* c1_lr_c1_2 = new TCanvas();
  c1_lr_c1_2->cd();
  c1_lr_c1_2->SetTitle(anaz.Data());
  c1_lr_c1_2->SetName(anaz.Data());
  c1_lr_c1_2->Divide(4,7);
  p1=multiGraph(c1_lr_c1_2,p1,dit,1, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,2, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,3, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,4, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,5, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","ge",2);
  p1=multiGraph(c1_lr_c1_2,p1,dit,6, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","he",2);
  p1=multiGraph(c1_lr_c1_2,p1,dit,7, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","ge",2);
  p1=multiGraph(c1_lr_c1_2,p1,dit,8, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","he",2);
  p1=multiGraph(c1_lr_c1_2,p1,dit,9, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,10,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,11,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,12,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,13,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,14,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,15,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,16,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,17,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","ge",2);
  p1=multiGraph(c1_lr_c1_2,p1,dit,18,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","he",2);
  p1=multiGraph(c1_lr_c1_2,p1,dit,19,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","ge",2);
  p1=multiGraph(c1_lr_c1_2,p1,dit,20,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","he",2);
  p1=multiGraph(c1_lr_c1_2,p1,dit,21,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,22,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,23,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,24,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,25,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,26,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,27,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2,p1,dit,28,"mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","he",4);
  c1_lr_c1_2->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c1_lr_c1_2->SaveAs(Form("%s",pdfresidualsname.c_str()));
  c1_lr_c1_2->SaveAs(Form("%s",pdfname.c_str()));

  anaz = "US Avg, DD "+modanaz+"Residual sensitivities for "+scale+"-avgd eigenvector lagrange slopes (only good run_avg sensitivity data included)";
  TCanvas* c1_2_lagr = new TCanvas();
  c1_2_lagr->cd();
  c1_2_lagr->SetTitle(anaz.Data());
  c1_2_lagr->SetName(anaz.Data());
  c1_2_lagr->Divide(4,7);
  p1=multiGraph(c1_2_lagr,p1,dit,1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil1_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2_lagr,p1,dit,3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil1_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2_lagr,p1,dit,5, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual",cut + " && rcdb_arm_flag==0","ge",2);
  p1=multiGraph(c1_2_lagr,p1,dit,6, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil2_residual",cut + " && rcdb_arm_flag==0","he",2);
  p1=multiGraph(c1_2_lagr,p1,dit,7, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual" ,cut + " && rcdb_arm_flag==0","ge",2);
  p1=multiGraph(c1_2_lagr,p1,dit,8, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil2_residual" ,cut + " && rcdb_arm_flag==0","he",2);
  p1=multiGraph(c1_2_lagr,p1,dit,9, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,10,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil3_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2_lagr,p1,dit,11,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,12,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil3_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2_lagr,p1,dit,13,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,14,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil4_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2_lagr,p1,dit,15,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,16,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil4_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2_lagr,p1,dit,17,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual",cut + " && rcdb_arm_flag==0","ge",2);
  p1=multiGraph(c1_2_lagr,p1,dit,18,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil5_residual",cut + " && rcdb_arm_flag==0","he",2);
  p1=multiGraph(c1_2_lagr,p1,dit,19,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual" ,cut + " && rcdb_arm_flag==0","ge",2);
  p1=multiGraph(c1_2_lagr,p1,dit,20,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil5_residual" ,cut + " && rcdb_arm_flag==0","he",2);
  p1=multiGraph(c1_2_lagr,p1,dit,21,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,22,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil6_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2_lagr,p1,dit,23,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,24,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil6_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2_lagr,p1,dit,25,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual",cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,26,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_coil7_residual",cut + " && rcdb_arm_flag==0","he",4);
  p1=multiGraph(c1_2_lagr,p1,dit,27,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual" ,cut + " && rcdb_arm_flag==0","ge",4);
  p1=multiGraph(c1_2_lagr,p1,dit,28,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_coil7_residual" ,cut + " && rcdb_arm_flag==0","he",4);
  c1_2_lagr->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c1_2_lagr->SaveAs(Form("%s",pdfresidualsname.c_str()));
  c1_2_lagr->SaveAs(Form("%s",pdfname.c_str()));

  anaz = "USL, R "+modanaz+"Residual sensitivities for "+scale+"-avgd eigenvector lagrange slopes (only good run_avg sensitivity data included)";
  TCanvas* c1_lr_c1_2_lagr = new TCanvas();
  c1_lr_c1_2_lagr->cd();
  c1_lr_c1_2_lagr->SetTitle(anaz.Data());
  c1_lr_c1_2_lagr->SetName(anaz.Data());
  c1_lr_c1_2_lagr->Divide(4,7);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual",Form("%s && rcdb_arm_flag!=1 && mini_eigen_lagr_%s_%s_avg.usl_coil1_residual_err>1e-9",cut.c_str(),nbpms.Data(),scale.Data()),"ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil1_residual",Form("%s && rcdb_arm_flag!=1 && mini_eigen_lagr_%s_%s_avg.usl_coil1_residual_err>1e-9",cut.c_str(),nbpms.Data(),scale.Data()),"he",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,5, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","ge",2);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,6, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","he",2);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,7, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","ge",2);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,8, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","he",2);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,9, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,10,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,11,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,12,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,13,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,14,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,15,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,16,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,17,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","ge",2);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,18,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","he",2);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,19,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","ge",2);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,20,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","he",2);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,21,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,22,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,23,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,24,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,25,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,26,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,27,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_lr_c1_2_lagr,p1,dit,28,"mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","he",4);
  c1_lr_c1_2_lagr->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c1_lr_c1_2_lagr->SaveAs(Form("%s",pdfresidualsname.c_str()));
  c1_lr_c1_2_lagr->SaveAs(Form("%s",pdfname.c_str()));
  FIXME FIXME TEMPORARY COMMENT OUT AUG 30
  */

  /*
  anaz = "USL, R "+modanaz+"Residual sensitivities for minirunwise eigenvector regression slopes (only good run_avg sensitivity data included)";
  TCanvas* c1_3 = new TCanvas();
  c1_3->cd();
  c1_3->SetTitle(anaz.Data());
  c1_3->SetName(anaz.Data());
  c1_3->Divide(4,7);
  p1=multiGraph(c1_3,p1,dit,1, "mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_3,p1,dit,2, "mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_3,p1,dit,3, "mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_3,p1,dit,4, "mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_3,p1,dit,5, "mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","ge",2);
  p1=multiGraph(c1_3,p1,dit,6, "mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","he",2);
  p1=multiGraph(c1_3,p1,dit,7, "mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","ge",2);
  p1=multiGraph(c1_3,p1,dit,8, "mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","he",2);
  p1=multiGraph(c1_3,p1,dit,9, "mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_3,p1,dit,10,"mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_3,p1,dit,11,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_3,p1,dit,12,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_3,p1,dit,13,"mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_3,p1,dit,14,"mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_3,p1,dit,15,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_3,p1,dit,16,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_3,p1,dit,17,"mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","ge",2);
  p1=multiGraph(c1_3,p1,dit,18,"mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","he",2);
  p1=multiGraph(c1_3,p1,dit,19,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","ge",2);
  p1=multiGraph(c1_3,p1,dit,20,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","he",2);
  p1=multiGraph(c1_3,p1,dit,21,"mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_3,p1,dit,22,"mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_3,p1,dit,23,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_3,p1,dit,24,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","he",4);
  p1=multiGraph(c1_3,p1,dit,25,"mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","ge",4);
  p1=multiGraph(c1_3,p1,dit,26,"mini_reference_eigen_reg_"+nbpms+"_sorted.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","he",4);
  p1=multiGraph(c1_3,p1,dit,27,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","ge",4);
  p1=multiGraph(c1_3,p1,dit,28,"mini_reference_eigen_reg_"+nbpms+"_sorted.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","he",4);
  c1_3->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c1_3->SaveAs(Form("%s",pdfresidualsname.c_str()));*/

  /* 
  FIXME FIXME TEMPORARY COMMENT OUT AUG 30

  anaz = "US Avg, DD "+modanaz+"Residual sensitivities for segment averaged (averaged run_avg slope calculations) "+scale+"-avgd eigenvector dithering slopes (only good run_avg sensitivity data included)";
  TCanvas* c1_4 = new TCanvas();
  c1_4->cd();
  c1_4->SetTitle(anaz.Data());
  c1_4->SetName(anaz.Data());
  c1_4->Divide(4,7);
  dit->SetScanField(0);
  p1=multiGraph(c1_4,p1,dit,1, "dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual) < 0.5)","g",4);
  p1=multiGraph(c1_4,p1,dit,2, "dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil1_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_4,p1,dit,3, "dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_4,p1,dit,4, "dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil1_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_4,p1,dit,5, "dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",2);
  p1=multiGraph(c1_4,p1,dit,6, "dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil2_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",2);
  p1=multiGraph(c1_4,p1,dit,7, "dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",2);
  p1=multiGraph(c1_4,p1,dit,8, "dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil2_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",2);
  p1=multiGraph(c1_4,p1,dit,9, "dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_4,p1,dit,10,"dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil3_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_4,p1,dit,11,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_4,p1,dit,12,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil3_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_4,p1,dit,13,"dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_4,p1,dit,14,"dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil4_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_4,p1,dit,15,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_4,p1,dit,16,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil4_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_4,p1,dit,17,"dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",2);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual) < 0.5)","g",2);
  p1=multiGraph(c1_4,p1,dit,18,"dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",2);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual) < 0.5)","h",2);
  p1=multiGraph(c1_4,p1,dit,19,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",2);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual) < 0.5)","g",2);
  p1=multiGraph(c1_4,p1,dit,20,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",2);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual) < 0.5)","h",2);
  p1=multiGraph(c1_4,p1,dit,21,"dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual) < 0.5)","g",4);
  p1=multiGraph(c1_4,p1,dit,22,"dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual) < 0.5)","h",4);
  p1=multiGraph(c1_4,p1,dit,23,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual) < 0.5)","g",4);
  p1=multiGraph(c1_4,p1,dit,24,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual) < 0.5)","h",4);
  p1=multiGraph(c1_4,p1,dit,25,"dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_4,p1,dit,26,"dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil7_residual",cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  p1=multiGraph(c1_4,p1,dit,27,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","ge",4);
  p1=multiGraph(c1_4,p1,dit,28,"dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil7_residual" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","he",4);
  c1_4->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c1_4->SaveAs(Form("%s",pdfresidualsname.c_str()));
  c1_4->SaveAs(Form("%s",pdfname.c_str()));

  anaz = "USL, R "+modanaz+"Residual sensitivities for segment averaged (averaged run_avg slope calculations) "+scale+"-avgd eigenvector dithering slopes (only good run_avg sensitivity data included)";
  TCanvas* clr_c1_4 = new TCanvas();
  clr_c1_4->cd();
  clr_c1_4->SetTitle(anaz.Data());
  clr_c1_4->SetName(anaz.Data());
  clr_c1_4->Divide(4,7);
  dit->SetScanField(0);
  p1=multiGraph(clr_c1_4,p1,dit,1, "dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual) < 0.5)","g",4);
  p1=multiGraph(clr_c1_4,p1,dit,2, "dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);
  p1=multiGraph(clr_c1_4,p1,dit,3, "dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);
  p1=multiGraph(clr_c1_4,p1,dit,4, "dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil1_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);
  p1=multiGraph(clr_c1_4,p1,dit,5, "dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",2);
  p1=multiGraph(clr_c1_4,p1,dit,6, "dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil2_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",2);
  p1=multiGraph(clr_c1_4,p1,dit,7, "dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",2);
  p1=multiGraph(clr_c1_4,p1,dit,8, "dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil2_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",2);
  p1=multiGraph(clr_c1_4,p1,dit,9, "dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4);
  p1=multiGraph(clr_c1_4,p1,dit,10,"dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil3_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);
  p1=multiGraph(clr_c1_4,p1,dit,11,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);
  p1=multiGraph(clr_c1_4,p1,dit,12,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil3_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);
  p1=multiGraph(clr_c1_4,p1,dit,13,"dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4);
  p1=multiGraph(clr_c1_4,p1,dit,14,"dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil4_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);
  p1=multiGraph(clr_c1_4,p1,dit,15,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);
  p1=multiGraph(clr_c1_4,p1,dit,16,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil4_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);
  p1=multiGraph(clr_c1_4,p1,dit,17,"dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",2);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual) < 0.5)","g",2);
  p1=multiGraph(clr_c1_4,p1,dit,18,"dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",2);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual) < 0.5)","h",2);
  p1=multiGraph(clr_c1_4,p1,dit,19,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",2);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual) < 0.5)","g",2);
  p1=multiGraph(clr_c1_4,p1,dit,20,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",2);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual) < 0.5)","h",2);
  p1=multiGraph(clr_c1_4,p1,dit,21,"dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual) < 0.5)","g",4);
  p1=multiGraph(clr_c1_4,p1,dit,22,"dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual) < 0.5)","h",4);
  p1=multiGraph(clr_c1_4,p1,dit,23,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual) < 0.5)","g",4);
  p1=multiGraph(clr_c1_4,p1,dit,24,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual) < 0.5)","h",4);
  p1=multiGraph(clr_c1_4,p1,dit,25,"dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",4);
  p1=multiGraph(clr_c1_4,p1,dit,26,"dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",4);
  p1=multiGraph(clr_c1_4,p1,dit,27,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",4);
  p1=multiGraph(clr_c1_4,p1,dit,28,"dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",4);
  clr_c1_4->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  clr_c1_4->SaveAs(Form("%s",pdfresidualsname.c_str()));
  clr_c1_4->SaveAs(Form("%s",pdfname.c_str()));


  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual > 0.5");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual) > 0.5)",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual > 0.5");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual) > 0.5)",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual > 0.5");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual) > 0.5)",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual > 0.5");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual) > 0.5)",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual > 0.5");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err:usr_coil6_err/usr_coil6:evMon0_coil6_err/evMon0_coil6:evMon1_coil6_err/evMon1_coil6:evMon2_coil6_err/evMon2_coil6:evMon3_coil6_err/evMon3_coil6:evMon4_coil6_err/evMon4_coil6"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual) > 0.5)",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual > 0.5");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual) > 0.5)",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual > 0.5");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual) > 0.5)",cut.c_str()));

  Printf("Again, but with a _err > 1.0 cut away instead this time");
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual > 0.5 && dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err < 1.0");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual) > 0.5) && dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil1_residual_err < 1.0",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual > 0.5 && dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err < 1.0");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual) > 0.5) && dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual_err < 1.0",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual > 0.5 && dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err < 1.0");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual) > 0.5) && dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual_err < 1.0",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual > 0.5 && dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err < 1.0");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual) > 0.5) && dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual_err < 1.0",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual > 0.5 && dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err < 1.0");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err:usr_coil6_err/usr_coil6:evMon0_coil6_err/evMon0_coil6:evMon1_coil6_err/evMon1_coil6:evMon2_coil6_err/evMon2_coil6:evMon3_coil6_err/evMon3_coil6:evMon4_coil6_err/evMon4_coil6"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual) > 0.5) && dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual_err < 1.0",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual > 0.5 && dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err < 1.0");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual) > 0.5) && dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil7_residual_err < 1.0",cut.c_str()));
  Printf("dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual > 0.5 && dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err < 1.0");
  dit->Scan(Form("run:cyclenum:segment:flag:rcdb_slug:minirun_n:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual:dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err"),Form("%s  && rcdb_arm_flag!=1 && minirun_n == 0 && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual) > 0.5) && dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil7_residual_err < 1.0",cut.c_str()));


  // Slopes plots

  anaz = "USL, R run_avg "+scale+"-avgd eigenvector dithering slopes (only good run_avg sensitivity data included)";
  Double_t splitting = 2.0;
  if (nmons <= 5) {
    splitting = 1.0;
  }
  TCanvas* c_slopes_dit_13746 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_slopes_dit_13746->cd();
  c_slopes_dit_13746->SetTitle(anaz.Data());
  c_slopes_dit_13746->SetName(anaz.Data());
  c_slopes_dit_13746->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_slopes_dit_13746,p1,dit,4*imon+1, Form("dit_13746.us_avg_evMon%d",imon),cut + " && rcdb_arm_flag==0 && minirun_n == 0");
    p1=multiGraph(c_slopes_dit_13746,p1,dit,4*imon+2, Form("dit_13746.us_dd_evMon%d",imon) ,cut + " && rcdb_arm_flag==0 && minirun_n == 0");
    p1=multiGraph(c_slopes_dit_13746,p1,dit,4*imon+3, Form("dit_13746.usl_evMon%d",imon)   ,cut + " && rcdb_arm_flag!=1 && minirun_n == 0");
    p1=multiGraph(c_slopes_dit_13746,p1,dit,4*imon+4, Form("dit_13746.usr_evMon%d",imon)   ,cut + " && rcdb_arm_flag!=2 && minirun_n == 0");
  }
  c_slopes_dit_13746->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_slopes_dit_13746->SaveAs(Form("%s",pdfname.c_str()));
  c_slopes_dit_13746->SaveAs(Form("%s(",pdfslopesname.c_str()));

  if (nmons > 5) {
    TCanvas* c_slopes_dit_13746_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_slopes_dit_13746_2->cd();
    c_slopes_dit_13746_2->SetTitle(anaz.Data());
    c_slopes_dit_13746_2->SetName(anaz.Data());
    c_slopes_dit_13746_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_slopes_dit_13746_2,p1,dit,4*imon+1, Form("dit_13746.us_avg_evMon%d",imon+(Int_t)ceil(nmons/splitting)),cut + " && rcdb_arm_flag==0 && minirun_n == 0");
      p1=multiGraph(c_slopes_dit_13746_2,p1,dit,4*imon+2, Form("dit_13746.us_dd_evMon%d",imon+(Int_t)ceil(nmons/splitting)) ,cut + " && rcdb_arm_flag==0 && minirun_n == 0");
      p1=multiGraph(c_slopes_dit_13746_2,p1,dit,4*imon+3, Form("dit_13746.usl_evMon%d",imon+(Int_t)ceil(nmons/splitting))   ,cut + " && rcdb_arm_flag!=1 && minirun_n == 0");
      p1=multiGraph(c_slopes_dit_13746_2,p1,dit,4*imon+4, Form("dit_13746.usr_evMon%d",imon+(Int_t)ceil(nmons/splitting))   ,cut + " && rcdb_arm_flag!=2 && minirun_n == 0");
    }
    c_slopes_dit_13746_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_slopes_dit_13746_2->SaveAs(Form("%s",pdfname.c_str()));
    c_slopes_dit_13746_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "USL, R "+scale+"-avgd eigenvector definitions, run-avg BMOD then segment averaged dithering slopes";
  TCanvas* c_slopes_dit_avgd = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_slopes_dit_avgd->cd();
  c_slopes_dit_avgd->SetTitle(anaz.Data());
  c_slopes_dit_avgd->SetName(anaz.Data());
  c_slopes_dit_avgd->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_slopes_dit_avgd,p1,dit,4*imon+1, "dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_evMon"+TString::Itoa(imon,10)+"_mean",cut + " && rcdb_arm_flag==0");
    p1=multiGraph(c_slopes_dit_avgd,p1,dit,4*imon+2, "dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_evMon"+TString::Itoa(imon,10)+"_mean" ,cut + " && rcdb_arm_flag==0");
    p1=multiGraph(c_slopes_dit_avgd,p1,dit,4*imon+3, "dit_"+scale+"_avgd"+bpms+"_friendable.usl_evMon"+TString::Itoa(imon,10)+"_mean"   ,cut + " && rcdb_arm_flag!=1");
    p1=multiGraph(c_slopes_dit_avgd,p1,dit,4*imon+4, "dit_"+scale+"_avgd"+bpms+"_friendable.usr_evMon"+TString::Itoa(imon,10)+"_mean"   ,cut + " && rcdb_arm_flag!=2");
  }
  c_slopes_dit_avgd->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_slopes_dit_avgd->SaveAs(Form("%s",pdfname.c_str()));
  c_slopes_dit_avgd->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    anaz = "USL, R "+scale+"-avgd eigenvector definitions, run-avg BMOD then segment averaged dithering slopes";
    TCanvas* c_slopes_dit_avgd_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_slopes_dit_avgd_2->cd();
    c_slopes_dit_avgd_2->SetTitle(anaz.Data());
    c_slopes_dit_avgd_2->SetName(anaz.Data());
    c_slopes_dit_avgd_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_slopes_dit_avgd_2,p1,dit,4*imon+1, "dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_mean",cut + " && rcdb_arm_flag==0");
      p1=multiGraph(c_slopes_dit_avgd_2,p1,dit,4*imon+2, "dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_mean" ,cut + " && rcdb_arm_flag==0");
      p1=multiGraph(c_slopes_dit_avgd_2,p1,dit,4*imon+3, "dit_"+scale+"_avgd"+bpms+"_friendable.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_mean"   ,cut + " && rcdb_arm_flag!=1");
      p1=multiGraph(c_slopes_dit_avgd_2,p1,dit,4*imon+4, "dit_"+scale+"_avgd"+bpms+"_friendable.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_mean"   ,cut + " && rcdb_arm_flag!=2");
    }
    c_slopes_dit_avgd_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_slopes_dit_avgd_2->SaveAs(Form("%s",pdfname.c_str()));
    c_slopes_dit_avgd_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }
  FIXME FIXME TEMPORARY COMMENT OUT AUG 30
  */

  anaz = "USL, R "+scale+"-avgd eigenvector definitions, lagrange and regression slopes";
  TCanvas* c_slopes_lagr_reg = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_slopes_lagr_reg->cd();
  c_slopes_lagr_reg->SetTitle(anaz.Data());
  c_slopes_lagr_reg->SetName(anaz.Data());
  Double_t splitting = 2.0;
  if (nmons <= 5) {
    splitting = 1.0;
  }
  c_slopes_lagr_reg->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=c_slopes_lagr_reg->cd(4*imon+1);
    p1->SetGrid();
    p1=multiGraph(c_slopes_lagr_reg,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new/1e-3",cut + " && rcdb_arm_flag==0","g",36+2*imon);
    p1=multiGraph(c_slopes_lagr_reg,p1,dit,4*imon+1, "mini_eigen_reg_" +nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new/1e-3",cut + " && rcdb_arm_flag==0","g",36+2*imon,"ap same");
    p1=c_slopes_lagr_reg->cd(4*imon+2);
    p1->SetGrid();
    p1=multiGraph(c_slopes_lagr_reg,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_new/1e-3" ,cut + " && rcdb_arm_flag==0","g",36+2*imon);
    p1=multiGraph(c_slopes_lagr_reg,p1,dit,4*imon+2, "mini_eigen_reg_" +nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_new/1e-3" ,cut + " && rcdb_arm_flag==0","g",36+2*imon,"ap same");
    p1=c_slopes_lagr_reg->cd(4*imon+3);
    p1->SetGrid();
    p1=multiGraph(c_slopes_lagr_reg,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=1","g",36+2*imon);
    p1=multiGraph(c_slopes_lagr_reg,p1,dit,4*imon+3, "mini_eigen_reg_" +nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=1","g",36+2*imon,"ap same");
    p1=c_slopes_lagr_reg->cd(4*imon+4);
    p1->SetGrid();
    p1=multiGraph(c_slopes_lagr_reg,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=2","g",36+2*imon);
    p1=multiGraph(c_slopes_lagr_reg,p1,dit,4*imon+4, "mini_eigen_reg_" +nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=2","g",36+2*imon,"ap same");
  }
  c_slopes_lagr_reg->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  //c_slopes_lagr_reg->SaveAs(Form("%s(",pdfname.c_str()));
  //c_slopes_lagr_reg->SaveAs(Form("%s(",pdfslopesname.c_str()));
  c_slopes_lagr_reg->SaveAs(Form("%s",pdfname.c_str()));
  c_slopes_lagr_reg->SaveAs(Form("%s",pdfslopesname.c_str()));

  /*
  FIXME FIXME TEMPORARY COMMENT OUT AUG 30
  if (nmons > 5) {
    anaz = "USL, R "+scale+"-avgd eigenvector definitions, lagrange and regression slopes";
    TCanvas* c_slopes_lagr_reg_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_slopes_lagr_reg_2->cd();
    c_slopes_lagr_reg_2->SetTitle(anaz.Data());
    c_slopes_lagr_reg_2->SetName(anaz.Data());
    c_slopes_lagr_reg_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_slopes_lagr_reg_2,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3",cut + " && rcdb_arm_flag==0","g",36+2*imon);
      p1=multiGraph(c_slopes_lagr_reg_2,p1,dit,4*imon+1, "mini_eigen_reg_" +nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3",cut + " && rcdb_arm_flag==0","g",36+2*imon,"ap same");
      p1=multiGraph(c_slopes_lagr_reg_2,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3" ,cut + " && rcdb_arm_flag==0","g",36+2*imon);
      p1=multiGraph(c_slopes_lagr_reg_2,p1,dit,4*imon+2, "mini_eigen_reg_" +nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3" ,cut + " && rcdb_arm_flag==0","g",36+2*imon,"ap same");
      p1=multiGraph(c_slopes_lagr_reg_2,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=1","g",36+2*imon);
      p1=multiGraph(c_slopes_lagr_reg_2,p1,dit,4*imon+3, "mini_eigen_reg_" +nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=1","g",36+2*imon,"ap same");
      p1=multiGraph(c_slopes_lagr_reg_2,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=2","g",36+2*imon);
      p1=multiGraph(c_slopes_lagr_reg_2,p1,dit,4*imon+4, "mini_eigen_reg_" +nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=2","g",36+2*imon,"ap same");
    }
    c_slopes_lagr_reg_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_slopes_lagr_reg_2->SaveAs(Form("%s",pdfname.c_str()));
    c_slopes_lagr_reg_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "USL, R "+scale+"-avgd eigenvector definitions, lagrange slopes";
  TCanvas* c_slopes_lagr = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_slopes_lagr->cd();
  c_slopes_lagr->SetTitle(anaz.Data());
  c_slopes_lagr->SetName(anaz.Data());
  c_slopes_lagr->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_slopes_lagr,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new/1e-3",cut + " && rcdb_arm_flag==0");
    p1=multiGraph(c_slopes_lagr,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_new/1e-3" ,cut + " && rcdb_arm_flag==0");
    p1=multiGraph(c_slopes_lagr,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=1");
    p1=multiGraph(c_slopes_lagr,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=2");
  }
  c_slopes_lagr->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_slopes_lagr->SaveAs(Form("%s",pdfname.c_str()));
  c_slopes_lagr->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    anaz = "USL, R "+scale+"-avgd eigenvector definitions, lagrange slopes";
    TCanvas* c_slopes_lagr_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_slopes_lagr_2->cd();
    c_slopes_lagr_2->SetTitle(anaz.Data());
    c_slopes_lagr_2->SetName(anaz.Data());
    c_slopes_lagr_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_slopes_lagr_2,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3",cut + " && rcdb_arm_flag==0");
      p1=multiGraph(c_slopes_lagr_2,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3" ,cut + " && rcdb_arm_flag==0");
      p1=multiGraph(c_slopes_lagr_2,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=1");
      p1=multiGraph(c_slopes_lagr_2,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=2");
    }
    c_slopes_lagr_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_slopes_lagr_2->SaveAs(Form("%s",pdfname.c_str()));
    c_slopes_lagr_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "USL, R "+scale+"-avgd eigenvector definitions, regression slopes";
  TCanvas* c_slopes_reg = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_slopes_reg->cd();
  c_slopes_reg->SetTitle(anaz.Data());
  c_slopes_reg->SetName(anaz.Data());
  c_slopes_reg->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_slopes_reg,p1,dit,4*imon+1, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_new/1e-3",cut + " && rcdb_arm_flag==0");
    p1=multiGraph(c_slopes_reg,p1,dit,4*imon+2, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_new/1e-3" ,cut + " && rcdb_arm_flag==0");
    p1=multiGraph(c_slopes_reg,p1,dit,4*imon+3, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=1");
    p1=multiGraph(c_slopes_reg,p1,dit,4*imon+4, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=2");
  }
  c_slopes_reg->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_slopes_reg->SaveAs(Form("%s",pdfname.c_str()));
  c_slopes_reg->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    anaz = "USL, R "+scale+"-avgd eigenvector definitions, regression slopes";
    TCanvas* c_slopes_reg_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_slopes_reg_2->cd();
    c_slopes_reg_2->SetTitle(anaz.Data());
    c_slopes_reg_2->SetName(anaz.Data());
    c_slopes_reg_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_slopes_reg_2,p1,dit,4*imon+1, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3",cut + " && rcdb_arm_flag==0");
      p1=multiGraph(c_slopes_reg_2,p1,dit,4*imon+2, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3" ,cut + " && rcdb_arm_flag==0");
      p1=multiGraph(c_slopes_reg_2,p1,dit,4*imon+3, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=1");
      p1=multiGraph(c_slopes_reg_2,p1,dit,4*imon+4, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_new/1e-3"   ,cut + " && rcdb_arm_flag!=2");
    }
    c_slopes_reg_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_slopes_reg_2->SaveAs(Form("%s)",pdfname.c_str()));
    c_slopes_reg_2->SaveAs(Form("%s)",pdfslopesname.c_str()));
  }


  // Slope diff plots 

  anaz = "US avg, dd run_avg dit vs segment avg dit slopes differences, "+scale+"-avgd eigenvector";
  TCanvas* c_diffslopes_dit_13746 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_diffslopes_dit_13746->cd();
  c_diffslopes_dit_13746->SetTitle(anaz.Data());
  c_diffslopes_dit_13746->SetName(anaz.Data());
  c_diffslopes_dit_13746->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_diffslopes_dit_13746,p1,dit,4*imon+1, "dit_13746.us_avg_evMon"+TString::Itoa(imon,10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag==0 && minirun_n == 0","g");
    p1=multiGraph(c_diffslopes_dit_13746,p1,dit,4*imon+2, "dit_13746.us_avg_evMon"+TString::Itoa(imon,10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag==0 && minirun_n == 0","h");
    p1=multiGraph(c_diffslopes_dit_13746,p1,dit,4*imon+3, "dit_13746.us_dd_evMon"+TString::Itoa(imon,10)+"_run_dit_dit_diff" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","g");
    p1=multiGraph(c_diffslopes_dit_13746,p1,dit,4*imon+4, "dit_13746.us_dd_evMon"+TString::Itoa(imon,10)+"_run_dit_dit_diff" ,cut + " && rcdb_arm_flag==0 && minirun_n == 0","h");
  }
  c_diffslopes_dit_13746->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_diffslopes_dit_13746->SaveAs(Form("%s",pdfname.c_str()));
  c_diffslopes_dit_13746->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    TCanvas* c_diffslopes_dit_13746_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_diffslopes_dit_13746_2->cd();
    c_diffslopes_dit_13746_2->SetTitle(anaz.Data());
    c_diffslopes_dit_13746_2->SetName(anaz.Data());
    c_diffslopes_dit_13746_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_diffslopes_dit_13746_2,p1,dit,4*imon+1, "dit_13746.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag==0 && minirun_n == 0","g");
      p1=multiGraph(c_diffslopes_dit_13746_2,p1,dit,4*imon+2, "dit_13746.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag==0 && minirun_n == 0","h");
      p1=multiGraph(c_diffslopes_dit_13746_2,p1,dit,4*imon+3, "dit_13746.us_dd_evMon" +TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag==0 && minirun_n == 0","g");
      p1=multiGraph(c_diffslopes_dit_13746_2,p1,dit,4*imon+4, "dit_13746.us_dd_evMon" +TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag==0 && minirun_n == 0","h");
    }
    c_diffslopes_dit_13746_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_diffslopes_dit_13746_2->SaveAs(Form("%s",pdfname.c_str()));
    c_diffslopes_dit_13746_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "USL, R run_avg dit vs segment avg dit slopes differences, "+scale+"-avgd eigenvector";
  TCanvas* c_lr_diffslopes_dit_13746 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_lr_diffslopes_dit_13746->cd();
  c_lr_diffslopes_dit_13746->SetTitle(anaz.Data());
  c_lr_diffslopes_dit_13746->SetName(anaz.Data());
  c_lr_diffslopes_dit_13746->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_lr_diffslopes_dit_13746,p1,dit,4*imon+1, "dit_13746.usl_evMon"+TString::Itoa(imon,10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","g");
    p1=multiGraph(c_lr_diffslopes_dit_13746,p1,dit,4*imon+2, "dit_13746.usl_evMon"+TString::Itoa(imon,10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","h");
    p1=multiGraph(c_lr_diffslopes_dit_13746,p1,dit,4*imon+3, "dit_13746.usr_evMon"+TString::Itoa(imon,10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","g");
    p1=multiGraph(c_lr_diffslopes_dit_13746,p1,dit,4*imon+4, "dit_13746.usr_evMon"+TString::Itoa(imon,10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","h");
  }
  c_lr_diffslopes_dit_13746->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_lr_diffslopes_dit_13746->SaveAs(Form("%s",pdfname.c_str()));
  c_lr_diffslopes_dit_13746->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    anaz = "USL, R run_avg dit vs segment avg dit slopes differences, "+scale+"-avgd eigenvector";
    TCanvas* c_lr_diffslopes_dit_13746_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_lr_diffslopes_dit_13746_2->cd();
    c_lr_diffslopes_dit_13746_2->SetTitle(anaz.Data());
    c_lr_diffslopes_dit_13746_2->SetName(anaz.Data());
    c_lr_diffslopes_dit_13746_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_lr_diffslopes_dit_13746_2,p1,dit,4*imon+1, "dit_13746.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","g");
      p1=multiGraph(c_lr_diffslopes_dit_13746_2,p1,dit,4*imon+2, "dit_13746.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","h");
      p1=multiGraph(c_lr_diffslopes_dit_13746_2,p1,dit,4*imon+3, "dit_13746.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","g");
      p1=multiGraph(c_lr_diffslopes_dit_13746_2,p1,dit,4*imon+4, "dit_13746.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_run_dit_dit_diff",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","h");
    }
    c_lr_diffslopes_dit_13746_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_lr_diffslopes_dit_13746_2->SaveAs(Form("%s",pdfname.c_str()));
    c_lr_diffslopes_dit_13746_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "US avg, dd lagr vs segment avg dit slopes differences, "+scale+"-avgd eigenvector";
  TCanvas* c_diffslopes_lagr = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_diffslopes_lagr->cd();
  c_diffslopes_lagr->SetTitle(anaz.Data());
  c_diffslopes_lagr->SetName(anaz.Data());
  c_diffslopes_lagr->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_diffslopes_lagr,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag==0","g");
    p1=multiGraph(c_diffslopes_lagr,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag==0","h");
    p1=multiGraph(c_diffslopes_lagr,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_lagr_dit_diff" ,cut + " && rcdb_arm_flag==0","g");
    p1=multiGraph(c_diffslopes_lagr,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_lagr_dit_diff" ,cut + " && rcdb_arm_flag==0","h");
  }
  c_diffslopes_lagr->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_diffslopes_lagr->SaveAs(Form("%s",pdfname.c_str()));
  c_diffslopes_lagr->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    TCanvas* c_diffslopes_lagr_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_diffslopes_lagr_2->cd();
    c_diffslopes_lagr_2->SetTitle(anaz.Data());
    c_diffslopes_lagr_2->SetName(anaz.Data());
    c_diffslopes_lagr_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_diffslopes_lagr_2,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag==0","g");
      p1=multiGraph(c_diffslopes_lagr_2,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag==0","h");
      p1=multiGraph(c_diffslopes_lagr_2,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_dit_diff" ,cut + " && rcdb_arm_flag==0","g");
      p1=multiGraph(c_diffslopes_lagr_2,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_dit_diff" ,cut + " && rcdb_arm_flag==0","h");
    }
    c_diffslopes_lagr_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_diffslopes_lagr_2->SaveAs(Form("%s",pdfname.c_str()));
    c_diffslopes_lagr_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "USL, R lagr vs segment avg dit slopes differences, "+scale+"-avgd eigenvector";
  TCanvas* c_lr_diffslopes_lagr = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_lr_diffslopes_lagr->cd();
  c_lr_diffslopes_lagr->SetTitle(anaz.Data());
  c_lr_diffslopes_lagr->SetName(anaz.Data());
  c_lr_diffslopes_lagr->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_lr_diffslopes_lagr,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag!=1","g");
    p1=multiGraph(c_lr_diffslopes_lagr,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag!=1","h");
    p1=multiGraph(c_lr_diffslopes_lagr,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag!=2","g");
    p1=multiGraph(c_lr_diffslopes_lagr,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag!=2","h");
  }
  c_lr_diffslopes_lagr->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_lr_diffslopes_lagr->SaveAs(Form("%s",pdfname.c_str()));
  c_lr_diffslopes_lagr->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    TCanvas* c_lr_diffslopes_lagr_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_lr_diffslopes_lagr_2->cd();
    c_lr_diffslopes_lagr_2->SetTitle(anaz.Data());
    c_lr_diffslopes_lagr_2->SetName(anaz.Data());
    c_lr_diffslopes_lagr_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_lr_diffslopes_lagr_2,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag!=1","g");
      p1=multiGraph(c_lr_diffslopes_lagr_2,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag!=1","h");
      p1=multiGraph(c_lr_diffslopes_lagr_2,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag!=2","g");
      p1=multiGraph(c_lr_diffslopes_lagr_2,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_dit_diff",cut + " && rcdb_arm_flag!=2","h");
    }
    c_lr_diffslopes_lagr_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_lr_diffslopes_lagr_2->SaveAs(Form("%s",pdfname.c_str()));
    c_lr_diffslopes_lagr_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "US avg, dd lagr vs reg slopes differences, "+scale+"-avgd eigenvector";
  TCanvas* c_diffslopes_lagr_reg = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_diffslopes_lagr_reg->cd();
  c_diffslopes_lagr_reg->SetTitle(anaz.Data());
  c_diffslopes_lagr_reg->SetName(anaz.Data());
  c_diffslopes_lagr_reg->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_diffslopes_lagr_reg,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag==0","g");
    p1=multiGraph(c_diffslopes_lagr_reg,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag==0","h");
    p1=multiGraph(c_diffslopes_lagr_reg,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff" ,cut + " && rcdb_arm_flag==0","g");
    p1=multiGraph(c_diffslopes_lagr_reg,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff" ,cut + " && rcdb_arm_flag==0","h");
  }
  c_diffslopes_lagr_reg->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_diffslopes_lagr_reg->SaveAs(Form("%s",pdfname.c_str()));
  c_diffslopes_lagr_reg->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    TCanvas* c_diffslopes_lagr_reg_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_diffslopes_lagr_reg_2->cd();
    c_diffslopes_lagr_reg_2->SetTitle(anaz.Data());
    c_diffslopes_lagr_reg_2->SetName(anaz.Data());
    c_diffslopes_lagr_reg_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_diffslopes_lagr_reg_2,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag==0","g");
      p1=multiGraph(c_diffslopes_lagr_reg_2,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag==0","h");
      p1=multiGraph(c_diffslopes_lagr_reg_2,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_reg_diff" ,cut + " && rcdb_arm_flag==0","g");
      p1=multiGraph(c_diffslopes_lagr_reg_2,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_reg_diff" ,cut + " && rcdb_arm_flag==0","h");
    }
    c_diffslopes_lagr_reg_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_diffslopes_lagr_reg_2->SaveAs(Form("%s",pdfname.c_str()));
    c_diffslopes_lagr_reg_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "USL, R lagr vs reg slopes differences, "+scale+"-avgd eigenvector";
  TCanvas* c_lr_diffslopes_lagr_reg = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_lr_diffslopes_lagr_reg->cd();
  c_lr_diffslopes_lagr_reg->SetTitle(anaz.Data());
  c_lr_diffslopes_lagr_reg->SetName(anaz.Data());
  c_lr_diffslopes_lagr_reg->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_lr_diffslopes_lagr_reg,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag!=1","g");
    p1=multiGraph(c_lr_diffslopes_lagr_reg,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag!=1","h");
    p1=multiGraph(c_lr_diffslopes_lagr_reg,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag!=2","g");
    p1=multiGraph(c_lr_diffslopes_lagr_reg,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag!=2","h");
  }
  c_lr_diffslopes_lagr_reg->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_lr_diffslopes_lagr_reg->SaveAs(Form("%s",pdfname.c_str()));
  c_lr_diffslopes_lagr_reg->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    TCanvas* c_lr_diffslopes_lagr_reg_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_lr_diffslopes_lagr_reg_2->cd();
    c_lr_diffslopes_lagr_reg_2->SetTitle(anaz.Data());
    c_lr_diffslopes_lagr_reg_2->SetName(anaz.Data());
    c_lr_diffslopes_lagr_reg_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_lr_diffslopes_lagr_reg_2,p1,dit,4*imon+1, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag!=1","g");
      p1=multiGraph(c_lr_diffslopes_lagr_reg_2,p1,dit,4*imon+2, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag!=1","h");
      p1=multiGraph(c_lr_diffslopes_lagr_reg_2,p1,dit,4*imon+3, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag!=2","g");
      p1=multiGraph(c_lr_diffslopes_lagr_reg_2,p1,dit,4*imon+4, "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_lagr_reg_diff",cut + " && rcdb_arm_flag!=2","h");
    }
    c_lr_diffslopes_lagr_reg_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_lr_diffslopes_lagr_reg_2->SaveAs(Form("%s",pdfname.c_str()));
    c_lr_diffslopes_lagr_reg_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "US avg, dd reg vs segment avg dit slopes differences, "+scale+"-avgd eigenvector";
  TCanvas* c_diffslopes_reg = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_diffslopes_reg->cd();
  c_diffslopes_reg->SetTitle(anaz.Data());
  c_diffslopes_reg->SetName(anaz.Data());
  c_diffslopes_reg->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_diffslopes_reg,p1,dit,4*imon+1, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_reg_dit_diff",cut + " && rcdb_arm_flag==0","g");
    p1=multiGraph(c_diffslopes_reg,p1,dit,4*imon+2, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa(imon,10)+"_reg_dit_diff",cut + " && rcdb_arm_flag==0","h");
    p1=multiGraph(c_diffslopes_reg,p1,dit,4*imon+3, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_reg_dit_diff" ,cut + " && rcdb_arm_flag==0","g");
    p1=multiGraph(c_diffslopes_reg,p1,dit,4*imon+4, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa(imon,10)+"_reg_dit_diff" ,cut + " && rcdb_arm_flag==0","h");
  }
  c_diffslopes_reg->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_diffslopes_reg->SaveAs(Form("%s",pdfname.c_str()));
  c_diffslopes_reg->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    TCanvas* c_diffslopes_reg_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_diffslopes_reg_2->cd();
    c_diffslopes_reg_2->SetTitle(anaz.Data());
    c_diffslopes_reg_2->SetName(anaz.Data());
    c_diffslopes_reg_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_diffslopes_reg_2,p1,dit,4*imon+1, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_reg_dit_diff",cut + " && rcdb_arm_flag==0","g");
      p1=multiGraph(c_diffslopes_reg_2,p1,dit,4*imon+2, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_avg_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_reg_dit_diff",cut + " && rcdb_arm_flag==0","h");
      p1=multiGraph(c_diffslopes_reg_2,p1,dit,4*imon+3, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_reg_dit_diff" ,cut + " && rcdb_arm_flag==0","g");
      p1=multiGraph(c_diffslopes_reg_2,p1,dit,4*imon+4, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.us_dd_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_reg_dit_diff" ,cut + " && rcdb_arm_flag==0","h");
    }
    c_diffslopes_reg_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_diffslopes_reg_2->SaveAs(Form("%s",pdfname.c_str()));
    c_diffslopes_reg_2->SaveAs(Form("%s",pdfslopesname.c_str()));
  }

  anaz = "USL, R reg vs segment avg dit slopes differences, "+scale+"-avgd eigenvector";
  TCanvas* c_lr_diffslopes_reg = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c_lr_diffslopes_reg->cd();
  c_lr_diffslopes_reg->SetTitle(anaz.Data());
  c_lr_diffslopes_reg->SetName(anaz.Data());
  c_lr_diffslopes_reg->Divide(4,(Int_t)ceil(nmons/splitting));
  for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
    p1=multiGraph(c_lr_diffslopes_reg,p1,dit,4*imon+1, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_reg_dit_diff",cut + " && rcdb_arm_flag!=1","g");
    p1=multiGraph(c_lr_diffslopes_reg,p1,dit,4*imon+2, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa(imon,10)+"_reg_dit_diff",cut + " && rcdb_arm_flag!=1","h");
    p1=multiGraph(c_lr_diffslopes_reg,p1,dit,4*imon+3, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_reg_dit_diff",cut + " && rcdb_arm_flag!=2","g");
    p1=multiGraph(c_lr_diffslopes_reg,p1,dit,4*imon+4, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa(imon,10)+"_reg_dit_diff",cut + " && rcdb_arm_flag!=2","h");
  }
  c_lr_diffslopes_reg->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c_lr_diffslopes_reg->SaveAs(Form("%s",pdfname.c_str()));
  c_lr_diffslopes_reg->SaveAs(Form("%s",pdfslopesname.c_str()));

  if (nmons > 5) {
    TCanvas* c_lr_diffslopes_reg_2 = new TCanvas();
    //TPad* cp1 = new TPad("test","",0,0.35,1,1);
    c_lr_diffslopes_reg_2->cd();
    c_lr_diffslopes_reg_2->SetTitle(anaz.Data());
    c_lr_diffslopes_reg_2->SetName(anaz.Data());
    c_lr_diffslopes_reg_2->Divide(4,(Int_t)ceil(nmons/splitting));
    for (Int_t imon = 0 ; imon < (Int_t)ceil(nmons/splitting); imon++) {
      p1=multiGraph(c_lr_diffslopes_reg_2,p1,dit,4*imon+1, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_reg_dit_diff",cut + " && rcdb_arm_flag!=1","g");
      p1=multiGraph(c_lr_diffslopes_reg_2,p1,dit,4*imon+2, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_reg_dit_diff",cut + " && rcdb_arm_flag!=1","h");
      p1=multiGraph(c_lr_diffslopes_reg_2,p1,dit,4*imon+3, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_reg_dit_diff",cut + " && rcdb_arm_flag!=2","g");
      p1=multiGraph(c_lr_diffslopes_reg_2,p1,dit,4*imon+4, "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_evMon"+TString::Itoa((imon+(Int_t)ceil(nmons/splitting)),10)+"_reg_dit_diff",cut + " && rcdb_arm_flag!=2","h");
    }
    c_lr_diffslopes_reg_2->cd();
    label->SetText(0.0,0.005,anaz.Data());
    label->Draw("same");
    c_lr_diffslopes_reg_2->SaveAs(Form("%s",pdfname.c_str()));
    c_lr_diffslopes_reg_2->SaveAs(Form("%s)",pdfslopesname.c_str()));
  }

  // Net residuals

  anaz = "USL,R "+modanaz+"Net (sqrt(Sum over coils("+modanaz+"Residuals per coil^2))) Residual sensitivities for all slopes (only good run_avg sensitivity data included)";
  TCanvas* c3 = new TCanvas();
  c3->cd();
  c3->SetTitle(anaz.Data());
  c3->SetName(anaz.Data());
  c3->Divide(4,3);
  p1=multiGraph(c3,p1,dit,1,  "dit_13746.usl_net_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit,2,  "dit_13746.usl_net_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit,3,  "dit_13746.usr_net_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit,4,  "dit_13746.usr_net_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit,5,  "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_net_residual",cut + " && rcdb_arm_flag!=1","ge",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit,6,  "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usl_net_residual",cut + " && rcdb_arm_flag!=1","he",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit,7,  "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_net_residual",cut + " && rcdb_arm_flag!=2","ge",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit,8,  "mini_eigen_reg_"+nbpms+"_"+scale+"_avg.usr_net_residual",cut + " && rcdb_arm_flag!=2","he",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit,9,  "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_net_residual",cut + " && rcdb_arm_flag!=1","ge",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit,10,  "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usl_net_residual",cut + " && rcdb_arm_flag!=1","he",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit,11,  "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_net_residual",cut + " && rcdb_arm_flag!=2","ge",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit,12,  "mini_eigen_lagr_"+nbpms+"_"+scale+"_avg.usr_net_residual",cut + " && rcdb_arm_flag!=2","he",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  c3->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");

  c3->SaveAs(Form("%s",pdfresidualsname.c_str()));
  c3->SaveAs(Form("%s",pdfname.c_str()));


  anaz = "Cont. USL,R "+modanaz+"Net (sqrt(Sum over coils("+modanaz+"Residuals per coil^2))) Residual sensitivities for all slopes (only good run_avg sensitivity data included)";
  TCanvas* c4 = new TCanvas();
  c4->cd();
  c4->SetTitle(anaz.Data());
  c4->SetName(anaz.Data());
  c4->Divide(4,2);
  FIXME FIXME TEMPORARY COMMENT OUT AUG 30
  */
  /*
  p1=multiGraph(c4,p1,dit,1,  "mini_reference_eigen_reg_"+nbpms+"_sorted.usl_net_residual",cut + " && rcdb_arm_flag!=1","g",1);
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c4,p1,dit,2, "mini_reference_eigen_reg_"+nbpms+"_sorted.usl_net_residual",cut + " && rcdb_arm_flag!=1","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  p1=multiGraph(c4,p1,dit,3, "mini_reference_eigen_reg_"+nbpms+"_sorted.usr_net_residual",cut + " && rcdb_arm_flag!=2","g",1);
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c4,p1,dit,4, "mini_reference_eigen_reg_"+nbpms+"_sorted.usr_net_residual",cut + " && rcdb_arm_flag!=2","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  */
  /*
  FIXME FIXME TEMPORARY COMMENT OUT AUG 30
  
  p1=multiGraph(c4,p1,dit,1, "dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_net_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",1);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual) < 0.5) && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual) < 0.5)","g",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c4,p1,dit,2, "dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_net_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",1);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil5_residual) < 0.5) && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_avg_coil6_residual) < 0.5)","h",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  p1=multiGraph(c4,p1,dit,3, "dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_net_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",1);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual) < 0.5) && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual) < 0.5)","g",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c4,p1,dit,4, "dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_net_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",1);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil5_residual) < 0.5) && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.us_dd_coil6_residual) < 0.5)","h",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  p1=multiGraph(c4,p1,dit,5, "dit_"+scale+"_avgd"+bpms+"_friendable.usl_net_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","ge",1);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual) < 0.5) && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual) < 0.5)","g",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c4,p1,dit,6, "dit_"+scale+"_avgd"+bpms+"_friendable.usl_net_residual",cut + " && rcdb_arm_flag!=1 && minirun_n == 0","he",1);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil5_residual) < 0.5) && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usl_coil6_residual) < 0.5)","h",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  p1=multiGraph(c4,p1,dit,7, "dit_"+scale+"_avgd"+bpms+"_friendable.usr_net_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","ge",1);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual) < 0.5) && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual) < 0.5)","g",1);
  p1->SetLogy(0);
  p1->SetGrid(1,0);
  p1=multiGraph(c4,p1,dit,8, "dit_"+scale+"_avgd"+bpms+"_friendable.usr_net_residual",cut + " && rcdb_arm_flag!=2 && minirun_n == 0","he",1);// && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil5_residual) < 0.5) && (abs(dit_"+scale+"_avgd"+bpms+"_friendable.usr_coil6_residual) < 0.5)","h",1);
  p1->SetLogy(1);
  p1->SetGrid(0,0);
  c4->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");

  c4->SaveAs(Form("%s)",pdfresidualsname.c_str()));
  c4->SaveAs(Form("%s)",pdfname.c_str()));
FIXME FIXME TEMPORARY COMMENT OUT AUG 30
*/

  TFile* outfile = new TFile("residuals_"+scale+"_"+nbpms+".root","RECREATE");
  outfile->cd();
  dit                    ->Write("dit");
  dit_13746              ->Write("dit_13746");
  part_avgd_regression   ->Write(Form("mini_eigen_reg_%s_%s_avg",nbpms.Data(),scale.Data()));
  part_avgd_lagrange     ->Write(Form("mini_eigen_lagr_%s_%s_avg",nbpms.Data(),scale.Data()));
  reference_regression   ->Write(Form("mini_reference_eigen_reg_%s_sorted",nbpms.Data()));
  segment_avgd_dit_slopes->Write(Form("dit_%s_avgd%s_friendable",scale.Data(),bpms.Data()));
  outfile->Close();

  gSystem->Exec("mv august_30_plots/residuals.txt august_30_plots/residuals_"+scale+"_"+nbpms+do_err+".txt");

}

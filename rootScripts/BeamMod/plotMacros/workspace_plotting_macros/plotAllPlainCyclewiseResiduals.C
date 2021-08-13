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

TVirtualPad* multiGraph(TPad* c1, TVirtualPad* p1, TChain* dit, int subpad = 1, std::string draw = "run+0.1*minirun_n", std::string cut = "", TString gh = "g", Int_t color = 4){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  TGraph* eq;

  //if (subpad == 1){
  //  dit->Scan("dit_13746.usl_1X:dit_15746.usl_1X:dit_13726.usl_1X:run+0.1*minirun_n","segment<4");
  //}
  //

  // less than OR both, as ONE is guaranteed to be == 0
  if (gh == "g"){
    Int_t nen_eq = dit->Draw(Form("%s:run+0.1*minirun_n",draw.c_str()),Form("(%s)",cut.c_str()),"goff");//lp
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    eq = new TGraph(nen_eq,dit->GetV2(),dit->GetV1());
    //eq->SetNameTitle(Form("eq_%s_%d",draw.c_str(),subpad),Form("eq_%s_%d",draw.c_str(),subpad));
    eq->SetLineColor(color);
    eq->SetMarkerColor(color);
    eq->SetMarkerStyle(20);
    eq->SetMarkerSize(0.2);
    gStyle->SetOptFit(1);
    eq->Fit("pol0");
    eq->SetNameTitle(Form("%s",draw.c_str()),Form("%s;Run+0.1*Minirun;%s",draw.c_str(),draw.c_str()));
    eq->Draw("ap");
  }
  if (gh == "h"){
    gStyle->SetOptStat(1221);
    Int_t nen_eq = dit->Draw(Form("%s",draw.c_str()),Form("(%s)",cut.c_str()));
    ((TH1*)gROOT->FindObject("htemp"))->SetNameTitle(Form("%s",draw.c_str()),Form("%s;Counts",draw.c_str()));
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

void plotAllPlainCyclewiseResiduals(TString ana = "", TString type = "_fractional"){ // If "ana" == "" then does the plain BPMs case
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* dit_cyclewise = new TChain("dit_plain_cyclewise_tagged_friendable");
  TChain* dit_cyclewise_13746 = new TChain("dit_plain_cyclewise_tagged_friendable");
  TChain* dit_runwise_13746 = new TChain("dit_plain_run_avg_tagged_friendable");
  TChain* reg_plain_regression = new TChain("agg_plain_friendable");
  TChain* segment_avgd_dit_slopes = new TChain("dit_plain_cyclewise_friendable");
  
  //TString run_cycle = "run_avg";
  //TString run_cycle_wise = "runwise";
  TString run_cycle = "cyclewise";
  TString run_cycle_wise = "cyclewise";
  dit_cyclewise          ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_%s%s_1X/respin2_All_dithering_slopes_%s_rcdb_segmented_pruned_usavg_tag_expanded.root",run_cycle.Data(),ana.Data(),run_cycle.Data()));
  //dit_cyclewise_13746 = (TChain*)dit_cyclewise->CloneTree();//CopyTree();
  //dit_cyclewise_13746->SetName("dit_cyclewise_13746");
  //dit_cyclewise_13746->SetAlias("usl_coil2_residual","(usl_coil2-((usl_1X*1X_coil2)+(usl_4aY*4aY_coil2)+(usl_4eX*4eX_coil2)+(usl_4eY*4eY_coil2)+(usl_12X*12X_coil2)))");
  // Assume run averaged analysis for now
  dit_cyclewise_13746    ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_%s%s_1X/respin2_All_dithering_slopes_%s_rcdb_segmented_pruned_usavg_tag_expanded.root",run_cycle.Data(),ana.Data(),run_cycle.Data()));
  dit_runwise_13746      ->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/slopes_run_avg_1X/respin2_All_dithering_slopes_run_avg_rcdb_segmented_pruned_usavg_tag_expanded.root"));
  reg_plain_regression   ->AddFile(Form("/u/group/halla/parity/software/japan_offline/bmodAna/rootScripts/BeamMod/plotMacros/processed_respin2_data/mini_friended_agg.root"));
  // Eigenvector BMOD only ever did run-averaged analysis
  segment_avgd_dit_slopes ->AddFile(Form("../processed_respin2_data/All_dit_outputs_doing_segment_averaging_of_slopes_mini_friended.root"));
  // For safety build an index that assigns a run and cyclenum to define the entries (so we can compare different trees even with missing entries)
  //dit_cyclewise_13746->BuildIndex("run","cyclenum");
  //dit_15746->BuildIndex("run","cyclenum");
  //dit_13726->BuildIndex("run","cyclenum");
  // Set their names to be unique
  // Make them friends
  dit_cyclewise->AddFriend(dit_cyclewise_13746,"dit_cyclewise_13746");
  dit_cyclewise->AddFriend(dit_runwise_13746,"dit_runwise_13746");
  dit_cyclewise->AddFriend(reg_plain_regression   ,"reg_plain_friendable");
  dit_cyclewise->AddFriend(segment_avgd_dit_slopes,"dit_plain_friendable");

  TString usl_frac = "";
  TString usr_frac = "";
  for (Int_t coil = 1 ; coil <=7 ; coil++ ) {
    if (type == "_fractional") {
      usl_frac = Form("/usl_coil%d",coil);
      usr_frac = Form("/usr_coil%d",coil);
    }
    dit_cyclewise_13746->SetAlias(Form("usl_coil%d_residual",coil),Form("(usl_coil%d-((dit_cyclewise_13746.usl_1X*1X_coil%d)+(dit_cyclewise_13746.usl_4aY*4aY_coil%d)+(dit_cyclewise_13746.usl_4eX*4eX_coil%d)+(dit_cyclewise_13746.usl_4eY*4eY_coil%d)+(dit_cyclewise_13746.usl_12X*12X_coil%d)))%s",coil,coil,coil,coil,coil,coil,usl_frac.Data()));
    dit_cyclewise_13746->SetAlias(Form("usr_coil%d_residual",coil),Form("(usr_coil%d-((dit_cyclewise_13746.usr_1X*1X_coil%d)+(dit_cyclewise_13746.usr_4aY*4aY_coil%d)+(dit_cyclewise_13746.usr_4eX*4eX_coil%d)+(dit_cyclewise_13746.usr_4eY*4eY_coil%d)+(dit_cyclewise_13746.usr_12X*12X_coil%d)))%s",coil,coil,coil,coil,coil,coil,usr_frac.Data()));
    dit_runwise_13746->SetAlias(Form("usl_coil%d_residual",coil),Form("(usl_coil%d-((dit_runwise_13746.usl_1X*1X_coil%d)+(dit_runwise_13746.usl_4aY*4aY_coil%d)+(dit_runwise_13746.usl_4eX*4eX_coil%d)+(dit_runwise_13746.usl_4eY*4eY_coil%d)+(dit_runwise_13746.usl_12X*12X_coil%d)))%s",coil,coil,coil,coil,coil,coil,usl_frac.Data()));
    dit_runwise_13746->SetAlias(Form("usr_coil%d_residual",coil),Form("(usr_coil%d-((dit_runwise_13746.usr_1X*1X_coil%d)+(dit_runwise_13746.usr_4aY*4aY_coil%d)+(dit_runwise_13746.usr_4eX*4eX_coil%d)+(dit_runwise_13746.usr_4eY*4eY_coil%d)+(dit_runwise_13746.usr_12X*12X_coil%d)))%s",coil,coil,coil,coil,coil,coil,usr_frac.Data()));
    reg_plain_regression->SetAlias(Form("usl_coil%d_residual",coil),Form("(usl_coil%d-((reg_plain_friendable.reg_asym_usl_diff_bpm1X_slope*1X_coil%d)+(reg_plain_friendable.reg_asym_usl_diff_bpm4aY_slope*4aY_coil%d)+(reg_plain_friendable.reg_asym_usl_diff_bpm4eX_slope*4eX_coil%d)+(reg_plain_friendable.reg_asym_usl_diff_bpm4eY_slope*4eY_coil%d)+(reg_plain_friendable.reg_asym_usl_diff_bpm12X_slope*12X_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,usl_frac.Data()));
    reg_plain_regression->SetAlias(Form("usr_coil%d_residual",coil),Form("(usr_coil%d-((reg_plain_friendable.reg_asym_usr_diff_bpm1X_slope*1X_coil%d)+(reg_plain_friendable.reg_asym_usr_diff_bpm4aY_slope*4aY_coil%d)+(reg_plain_friendable.reg_asym_usr_diff_bpm4eX_slope*4eX_coil%d)+(reg_plain_friendable.reg_asym_usr_diff_bpm4eY_slope*4eY_coil%d)+(reg_plain_friendable.reg_asym_usr_diff_bpm12X_slope*12X_coil%d))/1e-3)%s",coil,coil,coil,coil,coil,coil,usr_frac.Data()));
    segment_avgd_dit_slopes->SetAlias(Form("usl_coil%d_residual",coil),Form("(usl_coil%d-((dit_plain_friendable.usl_1X_mean*1X_coil%d)+(dit_plain_friendable.usl_4aY_mean*4aY_coil%d)+(dit_plain_friendable.usl_4eX_mean*4eX_coil%d)+(dit_plain_friendable.usl_4eY_mean*4eY_coil%d)+(dit_plain_friendable.usl_12X_mean*12X_coil%d)))%s",coil,coil,coil,coil,coil,coil,usl_frac.Data()));
    segment_avgd_dit_slopes->SetAlias(Form("usr_coil%d_residual",coil),Form("(usr_coil%d-((dit_plain_friendable.usr_1X_mean*1X_coil%d)+(dit_plain_friendable.usr_4aY_mean*4aY_coil%d)+(dit_plain_friendable.usr_4eX_mean*4eX_coil%d)+(dit_plain_friendable.usr_4eY_mean*4eY_coil%d)+(dit_plain_friendable.usr_12X_mean*12X_coil%d)))%s",coil,coil,coil,coil,coil,coil,usr_frac.Data()));
  }
  dit_cyclewise_13746->SetAlias("usl_net_residual","sqrt(pow(dit_cyclewise_13746.usl_coil1_residual,2)+pow(dit_cyclewise_13746.usl_coil2_residual,2)+pow(dit_cyclewise_13746.usl_coil3_residual,2)+pow(dit_cyclewise_13746.usl_coil4_residual,2)+pow(dit_cyclewise_13746.usl_coil5_residual,2)+pow(dit_cyclewise_13746.usl_coil6_residual,2)+pow(dit_cyclewise_13746.usl_coil7_residual,2))");
  dit_cyclewise_13746->SetAlias("usr_net_residual","sqrt(pow(dit_cyclewise_13746.usr_coil1_residual,2)+pow(dit_cyclewise_13746.usr_coil2_residual,2)+pow(dit_cyclewise_13746.usr_coil3_residual,2)+pow(dit_cyclewise_13746.usr_coil4_residual,2)+pow(dit_cyclewise_13746.usr_coil5_residual,2)+pow(dit_cyclewise_13746.usr_coil6_residual,2)+pow(dit_cyclewise_13746.usr_coil7_residual,2))");
  dit_runwise_13746->SetAlias("usl_net_residual","sqrt(pow(dit_runwise_13746.usl_coil1_residual,2)+pow(dit_runwise_13746.usl_coil2_residual,2)+pow(dit_runwise_13746.usl_coil3_residual,2)+pow(dit_runwise_13746.usl_coil4_residual,2)+pow(dit_runwise_13746.usl_coil5_residual,2)+pow(dit_runwise_13746.usl_coil6_residual,2)+pow(dit_runwise_13746.usl_coil7_residual,2))");
  dit_runwise_13746->SetAlias("usr_net_residual","sqrt(pow(dit_runwise_13746.usr_coil1_residual,2)+pow(dit_runwise_13746.usr_coil2_residual,2)+pow(dit_runwise_13746.usr_coil3_residual,2)+pow(dit_runwise_13746.usr_coil4_residual,2)+pow(dit_runwise_13746.usr_coil5_residual,2)+pow(dit_runwise_13746.usr_coil6_residual,2)+pow(dit_runwise_13746.usr_coil7_residual,2))");
  reg_plain_regression->SetAlias("usl_net_residual","sqrt(pow(reg_plain_friendable.usl_coil1_residual,2)+pow(reg_plain_friendable.usl_coil2_residual,2)+pow(reg_plain_friendable.usl_coil3_residual,2)+pow(reg_plain_friendable.usl_coil4_residual,2)+pow(reg_plain_friendable.usl_coil5_residual,2)+pow(reg_plain_friendable.usl_coil6_residual,2)+pow(reg_plain_friendable.usl_coil7_residual,2))");
  reg_plain_regression->SetAlias("usr_net_residual","sqrt(pow(reg_plain_friendable.usr_coil1_residual,2)+pow(reg_plain_friendable.usr_coil2_residual,2)+pow(reg_plain_friendable.usr_coil3_residual,2)+pow(reg_plain_friendable.usr_coil4_residual,2)+pow(reg_plain_friendable.usr_coil5_residual,2)+pow(reg_plain_friendable.usr_coil6_residual,2)+pow(reg_plain_friendable.usr_coil7_residual,2))");
  segment_avgd_dit_slopes->SetAlias("usl_net_residual","sqrt(pow(dit_plain_friendable.usl_coil1_residual,2)+pow(dit_plain_friendable.usl_coil2_residual,2)+pow(dit_plain_friendable.usl_coil3_residual,2)+pow(dit_plain_friendable.usl_coil4_residual,2)+pow(dit_plain_friendable.usl_coil5_residual,2)+pow(dit_plain_friendable.usl_coil6_residual,2)+pow(dit_plain_friendable.usl_coil7_residual,2))");
  segment_avgd_dit_slopes->SetAlias("usr_net_residual","sqrt(pow(dit_plain_friendable.usr_coil1_residual,2)+pow(dit_plain_friendable.usr_coil2_residual,2)+pow(dit_plain_friendable.usr_coil3_residual,2)+pow(dit_plain_friendable.usr_coil4_residual,2)+pow(dit_plain_friendable.usr_coil5_residual,2)+pow(dit_plain_friendable.usr_coil6_residual,2)+pow(dit_plain_friendable.usr_coil7_residual,2))");

  // Cut include "run == localtree.run" to make the comparisons only for runs with the sensitivity data valid (skip the no-bmod runs for this calculation)
  std::string cut = "(rcdb_run_type == 1 && rcdb_run_flag ==1 && flag==1 && rcdb_slug >= 100 && rcdb_slug<4000 && dit_true_data == 1)"; // Ignore dit_true_data to do a "all data, regardless of if there is meaningful sensitivity underneath" analysis


  // Do some Tree->SetAlias() around everywhere
  //std::string cut3 = "((dit_cyclewise_13746.usl_coil2-((dit_cyclewise_13746.usl_1X*dit_cyclewise_13746.1X_coil2)+(dit_cyclewise_13746.usl_4aY*dit_cyclewise_13746.4aY_coil2)+(dit_cyclewise_13746.usl_4eX*dit_cyclewise_13746.4eX_coil2)+(dit_cyclewise_13746.usl_4eY*dit_cyclewise_13746.4eY_coil2)+(dit_cyclewise_13746.usl_12X*dit_cyclewise_13746.12X_coil2))) < (dit_15746.usl_coil2-((dit_15746.usl_1X*dit_15746.1X_coil2)+(dit_15746.usl_4aY*dit_15746.4aY_coil2)+(dit_15746.usl_4eX*dit_15746.4eX_coil2)+(dit_15746.usl_4eY*dit_15746.4eY_coil2)+(dit_15746.usl_12X*dit_15746.12X_coil2))))";

  TString modanaz = "";
  if (type == "_fractional") {
    modanaz = "Fractional ";
  }

  TString anaz = "USL, R "+modanaz+"Residual sensitivities for coil set 13746, plain BPMs cyclewise bmod slope calculation (only good cyclewise sensitivity data included)";
  std::string pdfname = Form("july_14_plots/method_comparison_plain_residuals_%s%s.pdf",run_cycle_wise.Data(),type.Data());

  TText *label = new TText(0.0,0.005,anaz.Data());
  label->SetTextFont(23);
  label->SetTextSize(14);
  label->SetNDC();

  TVirtualPad* p1;
  TCanvas* c1_1 = new TCanvas();
  c1_1->cd();
  c1_1->SetTitle(anaz.Data());
  c1_1->SetName(anaz.Data());
  c1_1->Divide(4,7);
  //p1=highlightedGraph(c1_1,p1,dit_cyclewise_13746,1,"(dit_cyclewise_13746.usl_coil2-((dit_cyclewise_13746.usl_1X*dit_cyclewise_13746.1X_coil2)+(dit_cyclewise_13746.usl_4aY*dit_cyclewise_13746.4aY_coil2)+(dit_cyclewise_13746.usl_4eX*dit_cyclewise_13746.4eX_coil2)+(dit_cyclewise_13746.usl_4eY*dit_cyclewise_13746.4eY_coil2)+(dit_cyclewise_13746.usl_12X*dit_cyclewise_13746.12X_coil2))):dit_cyclewise_13746.cyclenum", cut,"(dit_cyclewise_13746.flag==0)","*",2,cut3,4,"(dit_cyclewise_13746.usl_coil2-((dit_cyclewise_13746.usl_1X*dit_cyclewise_13746.1X_coil2)+(dit_cyclewise_13746.usl_4aY*dit_cyclewise_13746.4aY_coil2)+(dit_cyclewise_13746.usl_4eX*dit_cyclewise_13746.4eX_coil2)+(dit_cyclewise_13746.usl_4eY*dit_cyclewise_13746.4eY_coil2)+(dit_cyclewise_13746.usl_12X*dit_cyclewise_13746.12X_coil2))) != 1.0e6");
  p1=multiGraph(c1_1,p1,dit_cyclewise,1, "dit_cyclewise_13746.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","g",4); // miniprinted cut needs to be "run" == "run_number" if you want to only look at runs that had BMOD data and only look at the BMOD data inside of those runs (otherwise you'll multi-count those runs).
  p1=multiGraph(c1_1,p1,dit_cyclewise,2, "dit_cyclewise_13746.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,3, "dit_cyclewise_13746.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,4, "dit_cyclewise_13746.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,5, "dit_cyclewise_13746.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","g",2);
  p1=multiGraph(c1_1,p1,dit_cyclewise,6, "dit_cyclewise_13746.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","h",2);
  p1=multiGraph(c1_1,p1,dit_cyclewise,7, "dit_cyclewise_13746.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","g",2);
  p1=multiGraph(c1_1,p1,dit_cyclewise,8, "dit_cyclewise_13746.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","h",2);
  p1=multiGraph(c1_1,p1,dit_cyclewise,9, "dit_cyclewise_13746.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,10,"dit_cyclewise_13746.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,11,"dit_cyclewise_13746.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,12,"dit_cyclewise_13746.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,13,"dit_cyclewise_13746.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,14,"dit_cyclewise_13746.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,15,"dit_cyclewise_13746.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,16,"dit_cyclewise_13746.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,17,"dit_cyclewise_13746.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","g",2);
  p1=multiGraph(c1_1,p1,dit_cyclewise,18,"dit_cyclewise_13746.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","h",2);
  p1=multiGraph(c1_1,p1,dit_cyclewise,19,"dit_cyclewise_13746.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","g",2);
  p1=multiGraph(c1_1,p1,dit_cyclewise,20,"dit_cyclewise_13746.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","h",2);
  p1=multiGraph(c1_1,p1,dit_cyclewise,21,"dit_cyclewise_13746.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,22,"dit_cyclewise_13746.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,23,"dit_cyclewise_13746.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,24,"dit_cyclewise_13746.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,25,"dit_cyclewise_13746.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,26,"dit_cyclewise_13746.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,27,"dit_cyclewise_13746.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1,p1,dit_cyclewise,28,"dit_cyclewise_13746.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","h",4);
  //p1=highlightedGraph(c1_1,p1,dit_cyclewise,5,"run:cyclenum",cut,"(flag==0)","*",2,"(flag==1)",4); // Red err cutout
  //p1=highlightedGraph(c1_1,p1,dit_cyclewise,6,"segment:cyclenum",cut,"(flag==0)","*",2,"(flag==1)",4);
  //p1=highlightedGraph(c1_1,p1,dit_cyclewise,9,"run:cyclenum",cut,"(flag==2)","*",2); // Black
  //p1=highlightedGraph(c1_1,p1,dit_cyclewise,10,"segment:cyclenum",cut,"(flag==2)","*",2); // Black
  c1_1->cd();
  label->Draw("same");
  c1_1->SaveAs(Form("%s(",pdfname.c_str()));

  anaz = "USL, R "+modanaz+"Residual sensitivities for coil set 13746, plain BPMs run_avg bmod slope calculation (only good cyclewise sensitivity data included)";
  TCanvas* c1_1r = new TCanvas();
  c1_1r->cd();
  c1_1r->SetTitle(anaz.Data());
  c1_1r->SetName(anaz.Data());
  c1_1r->Divide(4,7);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,1, "dit_runwise_13746.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","g",4); // miniprinted cut needs to be "run" == "run_number" if you want to only look at runs that had BMOD data and only look at the BMOD data inside of those runs (otherwise you'll multi-count those runs).
  p1=multiGraph(c1_1r,p1,dit_cyclewise,2, "dit_runwise_13746.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,3, "dit_runwise_13746.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,4, "dit_runwise_13746.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,5, "dit_runwise_13746.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","g",2);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,6, "dit_runwise_13746.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","h",2);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,7, "dit_runwise_13746.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","g",2);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,8, "dit_runwise_13746.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","h",2);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,9, "dit_runwise_13746.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,10,"dit_runwise_13746.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,11,"dit_runwise_13746.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,12,"dit_runwise_13746.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,13,"dit_runwise_13746.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,14,"dit_runwise_13746.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,15,"dit_runwise_13746.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,16,"dit_runwise_13746.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,17,"dit_runwise_13746.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","g",2);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,18,"dit_runwise_13746.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","h",2);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,19,"dit_runwise_13746.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","g",2);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,20,"dit_runwise_13746.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","h",2);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,21,"dit_runwise_13746.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,22,"dit_runwise_13746.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,23,"dit_runwise_13746.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,24,"dit_runwise_13746.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,25,"dit_runwise_13746.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,26,"dit_runwise_13746.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,27,"dit_runwise_13746.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_1r,p1,dit_cyclewise,28,"dit_runwise_13746.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","h",4);
  c1_1r->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c1_1r->SaveAs(Form("%s",pdfname.c_str()));

  anaz = "USL, R "+modanaz+"Residual sensitivities for plain BPM regression slopes (only good cyclewise sensitivity data included)";
  TCanvas* c1_2 = new TCanvas();
  c1_2->cd();
  c1_2->SetTitle(anaz.Data());
  c1_2->SetName(anaz.Data());
  c1_2->Divide(4,7);
  p1=multiGraph(c1_2,p1,dit_cyclewise,1, "reg_plain_friendable.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,2, "reg_plain_friendable.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,3, "reg_plain_friendable.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,4, "reg_plain_friendable.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,5, "reg_plain_friendable.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","g",2);
  p1=multiGraph(c1_2,p1,dit_cyclewise,6, "reg_plain_friendable.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","h",2);
  p1=multiGraph(c1_2,p1,dit_cyclewise,7, "reg_plain_friendable.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","g",2);
  p1=multiGraph(c1_2,p1,dit_cyclewise,8, "reg_plain_friendable.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","h",2);
  p1=multiGraph(c1_2,p1,dit_cyclewise,9, "reg_plain_friendable.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,10,"reg_plain_friendable.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,11,"reg_plain_friendable.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,12,"reg_plain_friendable.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,13,"reg_plain_friendable.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,14,"reg_plain_friendable.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,15,"reg_plain_friendable.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,16,"reg_plain_friendable.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,17,"reg_plain_friendable.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","g",2);
  p1=multiGraph(c1_2,p1,dit_cyclewise,18,"reg_plain_friendable.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","h",2);
  p1=multiGraph(c1_2,p1,dit_cyclewise,19,"reg_plain_friendable.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","g",2);
  p1=multiGraph(c1_2,p1,dit_cyclewise,20,"reg_plain_friendable.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","h",2);
  p1=multiGraph(c1_2,p1,dit_cyclewise,21,"reg_plain_friendable.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,22,"reg_plain_friendable.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,23,"reg_plain_friendable.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,24,"reg_plain_friendable.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,25,"reg_plain_friendable.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,26,"reg_plain_friendable.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,27,"reg_plain_friendable.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_2,p1,dit_cyclewise,28,"reg_plain_friendable.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","h",4);
  c1_2->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c1_2->SaveAs(Form("%s",pdfname.c_str()));

  anaz = "USL, R "+modanaz+"Residual sensitivities for segment averaged (averaging over run_avg slope calculations) plain BPM dithering slopes (only good cyclewise sensitivity data included)";
  TCanvas* c1_4 = new TCanvas();
  c1_4->cd();
  c1_4->SetTitle(anaz.Data());
  c1_4->SetName(anaz.Data());
  c1_4->Divide(4,7);
  p1=multiGraph(c1_4,p1,dit_cyclewise,1, "dit_plain_friendable.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,2, "dit_plain_friendable.usl_coil1_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,3, "dit_plain_friendable.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,4, "dit_plain_friendable.usr_coil1_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,5, "dit_plain_friendable.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","g",2);
  p1=multiGraph(c1_4,p1,dit_cyclewise,6, "dit_plain_friendable.usl_coil2_residual",cut + " && rcdb_arm_flag!=1","h",2);
  p1=multiGraph(c1_4,p1,dit_cyclewise,7, "dit_plain_friendable.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","g",2);
  p1=multiGraph(c1_4,p1,dit_cyclewise,8, "dit_plain_friendable.usr_coil2_residual",cut + " && rcdb_arm_flag!=2","h",2);
  p1=multiGraph(c1_4,p1,dit_cyclewise,9, "dit_plain_friendable.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,10,"dit_plain_friendable.usl_coil3_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,11,"dit_plain_friendable.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,12,"dit_plain_friendable.usr_coil3_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,13,"dit_plain_friendable.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,14,"dit_plain_friendable.usl_coil4_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,15,"dit_plain_friendable.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,16,"dit_plain_friendable.usr_coil4_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,17,"dit_plain_friendable.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","g",2);
  p1=multiGraph(c1_4,p1,dit_cyclewise,18,"dit_plain_friendable.usl_coil5_residual",cut + " && rcdb_arm_flag!=1","h",2);
  p1=multiGraph(c1_4,p1,dit_cyclewise,19,"dit_plain_friendable.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","g",2);
  p1=multiGraph(c1_4,p1,dit_cyclewise,20,"dit_plain_friendable.usr_coil5_residual",cut + " && rcdb_arm_flag!=2","h",2);
  p1=multiGraph(c1_4,p1,dit_cyclewise,21,"dit_plain_friendable.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,22,"dit_plain_friendable.usl_coil6_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,23,"dit_plain_friendable.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,24,"dit_plain_friendable.usr_coil6_residual",cut + " && rcdb_arm_flag!=2","h",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,25,"dit_plain_friendable.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,26,"dit_plain_friendable.usl_coil7_residual",cut + " && rcdb_arm_flag!=1","h",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,27,"dit_plain_friendable.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","g",4);
  p1=multiGraph(c1_4,p1,dit_cyclewise,28,"dit_plain_friendable.usr_coil7_residual",cut + " && rcdb_arm_flag!=2","h",4);
  c1_4->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c1_4->SaveAs(Form("%s",pdfname.c_str()));

  anaz = "USL, R cyclewise plain BPM dithering slopes (only good cyclewise sensitivity data included)";
  TCanvas* c2 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2->cd();
  c2->SetTitle(anaz.Data());
  c2->SetName(anaz.Data());
  c2->Divide(2,5);
  p1=multiGraph(c2,p1,dit_cyclewise,1, "dit_cyclewise_13746.usl_1X" ,cut + " && rcdb_arm_flag!=1");
  p1=multiGraph(c2,p1,dit_cyclewise,2, "dit_cyclewise_13746.usr_1X" ,cut + " && rcdb_arm_flag!=2");
  p1=multiGraph(c2,p1,dit_cyclewise,3, "dit_cyclewise_13746.usl_4aY",cut + " && rcdb_arm_flag!=1");
  p1=multiGraph(c2,p1,dit_cyclewise,4, "dit_cyclewise_13746.usr_4aY",cut + " && rcdb_arm_flag!=2");
  p1=multiGraph(c2,p1,dit_cyclewise,5, "dit_cyclewise_13746.usl_4eX",cut + " && rcdb_arm_flag!=1");
  p1=multiGraph(c2,p1,dit_cyclewise,6, "dit_cyclewise_13746.usr_4eX",cut + " && rcdb_arm_flag!=2");
  p1=multiGraph(c2,p1,dit_cyclewise,7, "dit_cyclewise_13746.usl_4eY",cut + " && rcdb_arm_flag!=1");
  p1=multiGraph(c2,p1,dit_cyclewise,8, "dit_cyclewise_13746.usr_4eY",cut + " && rcdb_arm_flag!=2");
  p1=multiGraph(c2,p1,dit_cyclewise,9, "dit_cyclewise_13746.usl_12X",cut + " && rcdb_arm_flag!=1");
  p1=multiGraph(c2,p1,dit_cyclewise,10,"dit_cyclewise_13746.usr_12X",cut + " && rcdb_arm_flag!=2");
  c2->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");
  c2->SaveAs(Form("%s",pdfname.c_str()));

  anaz = "USL,R "+modanaz+"Net (sqrt(Sum over coils("+modanaz+"Residuals per coil^2))) Residual sensitivities for all slopes (only good cyclewise sensitivity data included)";
  TCanvas* c3 = new TCanvas();
  c3->cd();
  c3->SetTitle(anaz.Data());
  c3->SetName(anaz.Data());
  c3->Divide(4,4);
  p1=multiGraph(c3,p1,dit_cyclewise,1,  "dit_cyclewise_13746.usl_net_residual" ,cut + " && rcdb_arm_flag!=1","g",1);
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit_cyclewise,2,  "dit_cyclewise_13746.usl_net_residual" ,cut + " && rcdb_arm_flag!=1","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit_cyclewise,3,  "dit_cyclewise_13746.usr_net_residual" ,cut + " && rcdb_arm_flag!=2","g",1);
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit_cyclewise,4,  "dit_cyclewise_13746.usr_net_residual" ,cut + " && rcdb_arm_flag!=2","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit_cyclewise,5,  "dit_runwise_13746.usl_net_residual"   ,cut + " && rcdb_arm_flag!=1","g",1); // NOT including the minirun_n == 0 cut (as run_avg case does) will mean that all multi-good-cycles runs will be multi-represented in the plot here (As desired)
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit_cyclewise,6,  "dit_runwise_13746.usl_net_residual"   ,cut + " && rcdb_arm_flag!=1","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit_cyclewise,7,  "dit_runwise_13746.usr_net_residual"   ,cut + " && rcdb_arm_flag!=2","g",1);
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit_cyclewise,8,  "dit_runwise_13746.usr_net_residual"   ,cut + " && rcdb_arm_flag!=2","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit_cyclewise,9,  "reg_plain_friendable.usl_net_residual",cut + " && rcdb_arm_flag!=1","g",1);
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit_cyclewise,10, "reg_plain_friendable.usl_net_residual",cut + " && rcdb_arm_flag!=1","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit_cyclewise,11, "reg_plain_friendable.usr_net_residual",cut + " && rcdb_arm_flag!=2","g",1);
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit_cyclewise,12, "reg_plain_friendable.usr_net_residual",cut + " && rcdb_arm_flag!=2","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit_cyclewise,13, "dit_plain_friendable.usl_net_residual",cut + " && rcdb_arm_flag!=1","g",1);
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit_cyclewise,14, "dit_plain_friendable.usl_net_residual",cut + " && rcdb_arm_flag!=1","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  p1=multiGraph(c3,p1,dit_cyclewise,15, "dit_plain_friendable.usr_net_residual",cut + " && rcdb_arm_flag!=2","g",1);
  p1->SetLogy();
  p1->SetGrid(1,0);
  p1=multiGraph(c3,p1,dit_cyclewise,16, "dit_plain_friendable.usr_net_residual",cut + " && rcdb_arm_flag!=2","h",1);
  p1->SetLogy();
  p1->SetGrid(0,0);
  c3->cd();
  label->SetText(0.0,0.005,anaz.Data());
  label->Draw("same");

  c3->SaveAs(Form("%s)",pdfname.c_str()));


  //c8->SaveAs(Form("%s)",pdfname.c_str()));
}

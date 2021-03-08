TVirtualPad* multiGraph(TPad* c1, TVirtualPad* p1, TChain* dit, int subpad = 1, std::string draw = "cyclenum", std::string cut = ""){
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

void plotResiduals(){ //std::string input = "NULL",std::string runNum = "", std::string slugNum = "0"){}
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* dit = new TChain("dit");
  TChain* dit_13746 = new TChain("dit");
  TChain* dit_15746 = new TChain("dit");
  TChain* dit_13726 = new TChain("dit");
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  dit->AddFile("bmodOutput/slopes_cyclewise_1X/dithering_slopes_13746_cyclewise.root");
  //dit_13746 = (TChain*)dit->CloneTree();//CopyTree();
  //dit_13746->SetName("dit_13746");
  //dit_13746->SetAlias("usl_coil2_residual","(usl_coil2-((usl_1X*1X_coil2)+(usl_4aY*4aY_coil2)+(usl_4eX*4eX_coil2)+(usl_4eY*4eY_coil2)+(usl_12X*12X_coil2)))");
  dit_13746->AddFile("bmodOutput/slopes_cyclewise_1X/dithering_slopes_13746_cyclewise.root");
  dit_15746->AddFile("bmodOutput/slopes_cyclewise_1X/dithering_slopes_15746_cyclewise.root");
  dit_13726->AddFile("bmodOutput/slopes_cyclewise_1X/dithering_slopes_13726_cyclewise.root");
  // For safety build an index that assigns a run and cyclenum to define the entries (so we can compare different trees even with missing entries)
  dit_13746->BuildIndex("run","cyclenum");
  dit_15746->BuildIndex("run","cyclenum");
  dit_13726->BuildIndex("run","cyclenum");
  // Set their names to be unique
  // Make them friends
  //dit_13746->AddFriend(dit_13746,"dit_13746");
  //dit_13746->AddFriend(dit_15746,"dit_15746");
  //dit_13746->AddFriend(dit_13726,"dit_13726");
  dit->AddFriend(dit_13746,"dit_13746");
  dit->AddFriend(dit_15746,"dit_15746");
  dit->AddFriend(dit_13726,"dit_13726");

  //dit->SetAlias("usl_coil2_residual","(usl_coil2-((usl_1X*1X_coil2)+(usl_4aY*4aY_coil2)+(usl_4eX*4eX_coil2)+(usl_4eY*4eY_coil2)+(usl_12X*12X_coil2)))");
  dit_13746->SetAlias("usl_coil3_residual","(dit_13746.usl_coil3-((dit_13746.usl_1X*1X_coil3)+(dit_13746.usl_4aY*4aY_coil3)+(dit_13746.usl_4eX*4eX_coil3)+(dit_13746.usl_4eY*4eY_coil3)+(dit_13746.usl_12X*12X_coil3)))");
  dit_15746->SetAlias("usl_coil3_residual","(dit_15746.usl_coil3-((dit_15746.usl_1X*1X_coil3)+(dit_15746.usl_4aY*4aY_coil3)+(dit_15746.usl_4eX*4eX_coil3)+(dit_15746.usl_4eY*4eY_coil3)+(dit_15746.usl_12X*12X_coil3)))");
  dit_13726->SetAlias("usl_coil3_residual","(dit_13726.usl_coil3-((dit_13726.usl_1X*1X_coil3)+(dit_13726.usl_4aY*4aY_coil3)+(dit_13726.usl_4eX*4eX_coil3)+(dit_13726.usl_4eY*4eY_coil3)+(dit_13726.usl_12X*12X_coil3)))");
  dit_13746->SetAlias("usl_coil5_residual","(dit_13746.usl_coil5-((dit_13746.usl_1X*1X_coil5)+(dit_13746.usl_4aY*4aY_coil5)+(dit_13746.usl_4eX*4eX_coil5)+(dit_13746.usl_4eY*4eY_coil5)+(dit_13746.usl_12X*12X_coil5)))");
  dit_15746->SetAlias("usl_coil5_residual","(dit_15746.usl_coil5-((dit_15746.usl_1X*1X_coil5)+(dit_15746.usl_4aY*4aY_coil5)+(dit_15746.usl_4eX*4eX_coil5)+(dit_15746.usl_4eY*4eY_coil5)+(dit_15746.usl_12X*12X_coil5)))");
  dit_13726->SetAlias("usl_coil5_residual","(dit_13726.usl_coil5-((dit_13726.usl_1X*1X_coil5)+(dit_13726.usl_4aY*4aY_coil5)+(dit_13726.usl_4eX*4eX_coil5)+(dit_13726.usl_4eY*4eY_coil5)+(dit_13726.usl_12X*12X_coil5)))");
  dit_13746->SetAlias("usl_coil2_residual","(dit_13746.usl_coil2-((dit_13746.usl_1X*1X_coil2)+(dit_13746.usl_4aY*4aY_coil2)+(dit_13746.usl_4eX*4eX_coil2)+(dit_13746.usl_4eY*4eY_coil2)+(dit_13746.usl_12X*12X_coil2)))");
  dit_15746->SetAlias("usl_coil2_residual","(dit_15746.usl_coil2-((dit_15746.usl_1X*1X_coil2)+(dit_15746.usl_4aY*4aY_coil2)+(dit_15746.usl_4eX*4eX_coil2)+(dit_15746.usl_4eY*4eY_coil2)+(dit_15746.usl_12X*12X_coil2)))");
  dit_13726->SetAlias("usl_coil2_residual","(dit_13726.usl_coil2-((dit_13726.usl_1X*1X_coil2)+(dit_13726.usl_4aY*4aY_coil2)+(dit_13726.usl_4eX*4eX_coil2)+(dit_13726.usl_4eY*4eY_coil2)+(dit_13726.usl_12X*12X_coil2)))");
  dit_13746->SetAlias("usl_coil4_residual","(dit_13746.usl_coil4-((dit_13746.usl_1X*1X_coil4)+(dit_13746.usl_4aY*4aY_coil4)+(dit_13746.usl_4eX*4eX_coil4)+(dit_13746.usl_4eY*4eY_coil4)+(dit_13746.usl_12X*12X_coil4)))");
  dit_15746->SetAlias("usl_coil4_residual","(dit_15746.usl_coil4-((dit_15746.usl_1X*1X_coil4)+(dit_15746.usl_4aY*4aY_coil4)+(dit_15746.usl_4eX*4eX_coil4)+(dit_15746.usl_4eY*4eY_coil4)+(dit_15746.usl_12X*12X_coil4)))");
  dit_13726->SetAlias("usl_coil4_residual","(dit_13726.usl_coil4-((dit_13726.usl_1X*1X_coil4)+(dit_13726.usl_4aY*4aY_coil4)+(dit_13726.usl_4eX*4eX_coil4)+(dit_13726.usl_4eY*4eY_coil4)+(dit_13726.usl_12X*12X_coil4)))");

  dit_13746->SetAlias("usr_coil3_residual","(dit_13746.usr_coil3-((dit_13746.usr_1X*1X_coil3)+(dit_13746.usr_4aY*4aY_coil3)+(dit_13746.usr_4eX*4eX_coil3)+(dit_13746.usr_4eY*4eY_coil3)+(dit_13746.usr_12X*12X_coil3)))");
  dit_15746->SetAlias("usr_coil3_residual","(dit_15746.usr_coil3-((dit_15746.usr_1X*1X_coil3)+(dit_15746.usr_4aY*4aY_coil3)+(dit_15746.usr_4eX*4eX_coil3)+(dit_15746.usr_4eY*4eY_coil3)+(dit_15746.usr_12X*12X_coil3)))");
  dit_13726->SetAlias("usr_coil3_residual","(dit_13726.usr_coil3-((dit_13726.usr_1X*1X_coil3)+(dit_13726.usr_4aY*4aY_coil3)+(dit_13726.usr_4eX*4eX_coil3)+(dit_13726.usr_4eY*4eY_coil3)+(dit_13726.usr_12X*12X_coil3)))");
  dit_13746->SetAlias("usr_coil5_residual","(dit_13746.usr_coil5-((dit_13746.usr_1X*1X_coil5)+(dit_13746.usr_4aY*4aY_coil5)+(dit_13746.usr_4eX*4eX_coil5)+(dit_13746.usr_4eY*4eY_coil5)+(dit_13746.usr_12X*12X_coil5)))");
  dit_15746->SetAlias("usr_coil5_residual","(dit_15746.usr_coil5-((dit_15746.usr_1X*1X_coil5)+(dit_15746.usr_4aY*4aY_coil5)+(dit_15746.usr_4eX*4eX_coil5)+(dit_15746.usr_4eY*4eY_coil5)+(dit_15746.usr_12X*12X_coil5)))");
  dit_13726->SetAlias("usr_coil5_residual","(dit_13726.usr_coil5-((dit_13726.usr_1X*1X_coil5)+(dit_13726.usr_4aY*4aY_coil5)+(dit_13726.usr_4eX*4eX_coil5)+(dit_13726.usr_4eY*4eY_coil5)+(dit_13726.usr_12X*12X_coil5)))");
  dit_13746->SetAlias("usr_coil2_residual","(dit_13746.usr_coil2-((dit_13746.usr_1X*1X_coil2)+(dit_13746.usr_4aY*4aY_coil2)+(dit_13746.usr_4eX*4eX_coil2)+(dit_13746.usr_4eY*4eY_coil2)+(dit_13746.usr_12X*12X_coil2)))");
  dit_15746->SetAlias("usr_coil2_residual","(dit_15746.usr_coil2-((dit_15746.usr_1X*1X_coil2)+(dit_15746.usr_4aY*4aY_coil2)+(dit_15746.usr_4eX*4eX_coil2)+(dit_15746.usr_4eY*4eY_coil2)+(dit_15746.usr_12X*12X_coil2)))");
  dit_13726->SetAlias("usr_coil2_residual","(dit_13726.usr_coil2-((dit_13726.usr_1X*1X_coil2)+(dit_13726.usr_4aY*4aY_coil2)+(dit_13726.usr_4eX*4eX_coil2)+(dit_13726.usr_4eY*4eY_coil2)+(dit_13726.usr_12X*12X_coil2)))");
  dit_13746->SetAlias("usr_coil4_residual","(dit_13746.usr_coil4-((dit_13746.usr_1X*1X_coil4)+(dit_13746.usr_4aY*4aY_coil4)+(dit_13746.usr_4eX*4eX_coil4)+(dit_13746.usr_4eY*4eY_coil4)+(dit_13746.usr_12X*12X_coil4)))");
  dit_15746->SetAlias("usr_coil4_residual","(dit_15746.usr_coil4-((dit_15746.usr_1X*1X_coil4)+(dit_15746.usr_4aY*4aY_coil4)+(dit_15746.usr_4eX*4eX_coil4)+(dit_15746.usr_4eY*4eY_coil4)+(dit_15746.usr_12X*12X_coil4)))");
  dit_13726->SetAlias("usr_coil4_residual","(dit_13726.usr_coil4-((dit_13726.usr_1X*1X_coil4)+(dit_13726.usr_4aY*4aY_coil4)+(dit_13726.usr_4eX*4eX_coil4)+(dit_13726.usr_4eY*4eY_coil4)+(dit_13726.usr_12X*12X_coil4)))");

  std::string cut = "(flag==1 && slug >= 100)";
  // Do some Tree->SetAlias() around everywhere
  //std::string cut3 = "((dit_13746.usl_coil2-((dit_13746.usl_1X*dit_13746.1X_coil2)+(dit_13746.usl_4aY*dit_13746.4aY_coil2)+(dit_13746.usl_4eX*dit_13746.4eX_coil2)+(dit_13746.usl_4eY*dit_13746.4eY_coil2)+(dit_13746.usl_12X*dit_13746.12X_coil2))) < (dit_15746.usl_coil2-((dit_15746.usl_1X*dit_15746.1X_coil2)+(dit_15746.usl_4aY*dit_15746.4aY_coil2)+(dit_15746.usl_4eX*dit_15746.4eX_coil2)+(dit_15746.usl_4eY*dit_15746.4eY_coil2)+(dit_15746.usl_12X*dit_15746.12X_coil2))))";

  std::string ana = "Compare coil sets 13746, 15746, and 13726 residual sensitivities against non-included coils";
  std::string pdfname = Form("test_residuals.pdf");

  TText *label = new TText(0.0,0.005,ana.c_str());
  label->SetTextFont(23);
  label->SetTextSize(14);
  label->SetNDC();

  TVirtualPad* p1;
  TCanvas* c1 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1->cd();
  c1->SetTitle(ana.c_str());
  c1->SetName(ana.c_str());
  c1->Divide(2,4);
  //p1=highlightedGraph(c1,p1,dit_13746,1,"(dit_13746.usl_coil2-((dit_13746.usl_1X*dit_13746.1X_coil2)+(dit_13746.usl_4aY*dit_13746.4aY_coil2)+(dit_13746.usl_4eX*dit_13746.4eX_coil2)+(dit_13746.usl_4eY*dit_13746.4eY_coil2)+(dit_13746.usl_12X*dit_13746.12X_coil2))):dit_13746.cyclenum", cut,"(dit_13746.flag==0)","*",2,cut3,4,"(dit_13746.usl_coil2-((dit_13746.usl_1X*dit_13746.1X_coil2)+(dit_13746.usl_4aY*dit_13746.4aY_coil2)+(dit_13746.usl_4eX*dit_13746.4eX_coil2)+(dit_13746.usl_4eY*dit_13746.4eY_coil2)+(dit_13746.usl_12X*dit_13746.12X_coil2))) != 1.0e6");
  p1=multiGraph(c1,p1,dit,1,"usl_coil3_residual",cut);
  p1=multiGraph(c1,p1,dit,2,"usr_coil3_residual",cut);
  p1=multiGraph(c1,p1,dit,3,"usl_coil5_residual",cut);
  p1=multiGraph(c1,p1,dit,4,"usr_coil5_residual",cut);
  p1=multiGraph(c1,p1,dit,5,"usl_coil2_residual",cut);
  p1=multiGraph(c1,p1,dit,6,"usr_coil2_residual",cut);
  p1=multiGraph(c1,p1,dit,7,"usl_coil4_residual",cut);
  p1=multiGraph(c1,p1,dit,8,"usr_coil4_residual",cut);
  //p1=highlightedGraph(c1,p1,dit,5,"run:cyclenum",cut,"(flag==0)","*",2,"(flag==1)",4); // Red err cutout
  //p1=highlightedGraph(c1,p1,dit,6,"segment:cyclenum",cut,"(flag==0)","*",2,"(flag==1)",4);
  //p1=highlightedGraph(c1,p1,dit,9,"run:cyclenum",cut,"(flag==2)","*",2); // Black
  //p1=highlightedGraph(c1,p1,dit,10,"segment:cyclenum",cut,"(flag==2)","*",2); // Black
  c1->cd();
  label->Draw("same");
  c1->SaveAs(Form("%s(",pdfname.c_str()));

  TCanvas* c2 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2->cd();
  c2->SetTitle(ana.c_str());
  c2->SetName(ana.c_str());
  c2->Divide(2,5);
  p1=multiGraph(c2,p1,dit,1,"usl_1X",cut);
  p1=multiGraph(c2,p1,dit,2,"usr_1X",cut);
  p1=multiGraph(c2,p1,dit,3,"usl_4aY",cut);
  p1=multiGraph(c2,p1,dit,4,"usr_4aY",cut);
  p1=multiGraph(c2,p1,dit,5,"usl_4eX",cut);
  p1=multiGraph(c2,p1,dit,6,"usr_4eX",cut);
  p1=multiGraph(c2,p1,dit,7,"usl_4eY",cut);
  p1=multiGraph(c2,p1,dit,8,"usr_4eY",cut);
  p1=multiGraph(c2,p1,dit,9,"usl_12X",cut);
  p1=multiGraph(c2,p1,dit,10,"usr_12X",cut);

  c2->SaveAs(Form("%s)",pdfname.c_str()));


  //c8->SaveAs(Form("%s)",pdfname.c_str()));
}

TVirtualPad* multiGraph(TPad* c1, TVirtualPad* p1, TChain* dit, int subpad = 1, std::string draw = "cyclenum", std::string cut = ""){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  TMultiGraph* mg = new TMultiGraph();
  TGraph* eq3542;
  TGraph* A_eq3542;
  TGraph* B_eq3542;
  TGraph* C_eq3542;
  TGraph* D_eq3542;

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
    mg->Add(eq3542,"lp");
    legend->AddEntry(eq3542,"13746 normal BPMs","lp");
  }
  Int_t nen_A_eq3542 = dit->Draw(Form("dit_A_13746.%s:cyclenum",draw.c_str()),Form("(%s && abs(dit_A_13746.%s)>0.001)",cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13746 is ideal\n",nen_A_eq3542);
  if (nen_A_eq3542 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("A_eq3542_%s_%d",draw.c_str(),subpad));
    //A_eq3542 = (TGraph*)p1->FindObject(Form("A_eq3542_%s_%d",draw.c_str(),subpad));
    A_eq3542 = new TGraph(nen_A_eq3542,dit->GetV2(),dit->GetV1());
    A_eq3542->SetNameTitle(Form("A_eq3542_%s_%d",draw.c_str(),subpad),Form("A_eq3542_%s_%d",draw.c_str(),subpad));
    A_eq3542->SetLineColor(46);
    A_eq3542->SetMarkerColor(46);
    A_eq3542->SetMarkerStyle(20);
    A_eq3542->SetMarkerSize(0.01);
    mg->Add(A_eq3542,"lp");
    legend->AddEntry(A_eq3542,"13746 A BPMs","lp");
  }
  Int_t nen_B_eq3542 = dit->Draw(Form("dit_B_13746.%s:cyclenum",draw.c_str()),Form("(%s && abs(dit_B_13746.%s)>0.001)",cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13746 is ideal\n",nen_B_eq3542);
  if (nen_B_eq3542 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("B_eq3542_%s_%d",draw.c_str(),subpad));
    //B_eq3542 = (TGraph*)p1->FindObject(Form("B_eq3542_%s_%d",draw.c_str(),subpad));
    B_eq3542 = new TGraph(nen_B_eq3542,dit->GetV2(),dit->GetV1());
    B_eq3542->SetNameTitle(Form("B_eq3542_%s_%d",draw.c_str(),subpad),Form("B_eq3542_%s_%d",draw.c_str(),subpad));
    B_eq3542->SetLineColor(31);
    B_eq3542->SetMarkerColor(31);
    B_eq3542->SetMarkerStyle(20);
    B_eq3542->SetMarkerSize(0.01);
    mg->Add(B_eq3542,"lp");
    legend->AddEntry(B_eq3542,"13746 B BPMs","lp");
  }
  Int_t nen_C_eq3542 = dit->Draw(Form("dit_C_13746.%s:cyclenum",draw.c_str()),Form("(%s && abs(dit_C_13746.%s)>0.001)",cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13746 is ideal\n",nen_C_eq3542);
  if (nen_C_eq3542 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("C_eq3542_%s_%d",draw.c_str(),subpad));
    //C_eq3542 = (TGraph*)p1->FindObject(Form("C_eq3542_%s_%d",draw.c_str(),subpad));
    C_eq3542 = new TGraph(nen_C_eq3542,dit->GetV2(),dit->GetV1());
    C_eq3542->SetNameTitle(Form("C_eq3542_%s_%d",draw.c_str(),subpad),Form("C_eq3542_%s_%d",draw.c_str(),subpad));
    C_eq3542->SetLineColor(41);
    C_eq3542->SetMarkerColor(41);
    C_eq3542->SetMarkerStyle(20);
    C_eq3542->SetMarkerSize(0.01);
    mg->Add(C_eq3542,"lp");
    legend->AddEntry(C_eq3542,"13746 C BPMs","lp");
  }
  Int_t nen_D_eq3542 = dit->Draw(Form("dit_D_13746.%s:cyclenum",draw.c_str()),Form("(%s && abs(dit_D_13746.%s)>0.001)",cut.c_str(),draw.c_str()),"goff");//lp
  printf("Plotting %d good entries where 13746 is ideal\n",nen_D_eq3542);
  if (nen_D_eq3542 > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("D_eq3542_%s_%d",draw.c_str(),subpad));
    //D_eq3542 = (TGraph*)p1->FindObject(Form("D_eq3542_%s_%d",draw.c_str(),subpad));
    D_eq3542 = new TGraph(nen_D_eq3542,dit->GetV2(),dit->GetV1());
    D_eq3542->SetNameTitle(Form("D_eq3542_%s_%d",draw.c_str(),subpad),Form("D_eq3542_%s_%d",draw.c_str(),subpad));
    D_eq3542->SetLineColor(8);
    D_eq3542->SetMarkerColor(8);
    D_eq3542->SetMarkerStyle(20);
    D_eq3542->SetMarkerSize(0.01);
    mg->Add(D_eq3542,"lp");
    legend->AddEntry(D_eq3542,"13746 D BPMs","lp");
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

void plotSlopes(){ //std::string input = "NULL",std::string runNum = "", std::string slugNum = "0"){}
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* dit = new TChain("dit");
  TChain* dit_13746 = new TChain("dit");
  TChain* dit_A_13746 = new TChain("dit");
  TChain* dit_B_13746 = new TChain("dit");
  TChain* dit_C_13746 = new TChain("dit");
  TChain* dit_D_13746 = new TChain("dit");
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  dit->AddFile("bmodOutput/slopes_cyclewise_1X/dithering_slopes_13746_cyclewise.root");
  //dit_13746 = (TChain*)dit->CloneTree();//CopyTree();
  //dit_13746->SetName("dit_13746");
  //dit_13746->SetAlias("usl_coil2_residual","(usl_coil2-((usl_1X*1X_coil2)+(usl_4aY*4aY_coil2)+(usl_4eX*4eX_coil2)+(usl_4eY*4eY_coil2)+(usl_12X*12X_coil2)))");
  dit_13746->AddFile("bmodOutput/slopes_cyclewise_1X/dithering_slopes_13746_cyclewise.root");
  dit_A_13746->AddFile("bmodOutput/slopes_cyclewise_A_1X/dithering_slopes_13746_A_cyclewise.root");
  dit_B_13746->AddFile("bmodOutput/slopes_cyclewise_B_1X/dithering_slopes_13746_B_cyclewise.root");
  dit_C_13746->AddFile("bmodOutput/slopes_cyclewise_C_1X/dithering_slopes_13746_C_cyclewise.root");
  dit_D_13746->AddFile("bmodOutput/slopes_cyclewise_D_1X/dithering_slopes_13746_D_cyclewise.root");
  // For safety build an index that assigns a run and cyclenum to define the entries (so we can compare different trees even with missing entries)
  dit_13746->BuildIndex("run","cyclenum");
  dit_A_13746->BuildIndex("run","cyclenum");
  dit_B_13746->BuildIndex("run","cyclenum");
  dit_C_13746->BuildIndex("run","cyclenum");
  dit_D_13746->BuildIndex("run","cyclenum");
  // Set their names to be unique
  // Make them friends
  //dit_13746->AddFriend(dit_13746,"dit_13746");
  //dit_13746->AddFriend(dit_15746,"dit_15746");
  //dit_13746->AddFriend(dit_13726,"dit_13726");
  dit->AddFriend(dit_13746,"dit_13746");
  dit->AddFriend(dit_A_13746,"dit_A_13746");
  dit->AddFriend(dit_B_13746,"dit_B_13746");
  dit->AddFriend(dit_C_13746,"dit_C_13746");
  dit->AddFriend(dit_D_13746,"dit_D_13746");

  //dit->SetAlias("usl_coil2_residual","(usl_coil2-((usl_1X*1X_coil2)+(usl_4aY*4aY_coil2)+(usl_4eX*4eX_coil2)+(usl_4eY*4eY_coil2)+(usl_12X*12X_coil2)))");
  dit_13746->SetAlias("usl_PX","dit_13746.usl_1X");
  dit_13746->SetAlias("usl_MX","dit_13746.usl_4eX");
  dit_13746->SetAlias("usl_PY","dit_13746.usl_4aY");
  dit_13746->SetAlias("usl_MY","dit_13746.usl_4eY");
  dit_13746->SetAlias("usl_EX","dit_13746.usl_12X");
  dit_13746->SetAlias("usr_PX","dit_13746.usr_1X");
  dit_13746->SetAlias("usr_MX","dit_13746.usr_4eX");
  dit_13746->SetAlias("usr_PY","dit_13746.usr_4aY");
  dit_13746->SetAlias("usr_MY","dit_13746.usr_4eY");
  dit_13746->SetAlias("usr_EX","dit_13746.usr_12X");

  std::string cut = "(flag==1 && slug >= 100)";
  // Do some Tree->SetAlias() around everywhere
  //std::string cut3 = "((dit_13746.usl_coil2-((dit_13746.usl_1X*dit_13746.1X_coil2)+(dit_13746.usl_4aY*dit_13746.4aY_coil2)+(dit_13746.usl_4eX*dit_13746.4eX_coil2)+(dit_13746.usl_4eY*dit_13746.4eY_coil2)+(dit_13746.usl_12X*dit_13746.12X_coil2))) < (dit_15746.usl_coil2-((dit_15746.usl_1X*dit_15746.1X_coil2)+(dit_15746.usl_4aY*dit_15746.4aY_coil2)+(dit_15746.usl_4eX*dit_15746.4eX_coil2)+(dit_15746.usl_4eY*dit_15746.4eY_coil2)+(dit_15746.usl_12X*dit_15746.12X_coil2))))";

  std::string ana = "Compare coil sets 13746, 15746, and 13726 slopes vs. eachother";
  std::string pdfname = Form("test_slopes.pdf");

  TText *label = new TText(0.0,0.005,ana.c_str());
  label->SetTextFont(23);
  label->SetTextSize(14);
  label->SetNDC();

  TVirtualPad* p1;
  /*TCanvas* c1 = new TCanvas();
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
  c1->SaveAs(Form("%s(",pdfname.c_str()));*/

  TCanvas* c2 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2->cd();
  c2->SetTitle(ana.c_str());
  c2->SetName(ana.c_str());
  c2->Divide(2,5);
  p1=multiGraph(c2,p1,dit,1,"usl_PX",cut);
  p1=multiGraph(c2,p1,dit,2,"usr_PX",cut);
  p1=multiGraph(c2,p1,dit,3,"usl_MX",cut);
  p1=multiGraph(c2,p1,dit,4,"usr_MX",cut);
  p1=multiGraph(c2,p1,dit,5,"usl_PY",cut);
  p1=multiGraph(c2,p1,dit,6,"usr_PY",cut);
  p1=multiGraph(c2,p1,dit,7,"usl_MY",cut);
  p1=multiGraph(c2,p1,dit,8,"usr_MY",cut);
  p1=multiGraph(c2,p1,dit,9,"usl_EX",cut);
  p1=multiGraph(c2,p1,dit,10,"usr_EX",cut);
  c2->cd();
  label->Draw("same");

  c2->SaveAs(Form("%s",pdfname.c_str()));


  //c8->SaveAs(Form("%s)",pdfname.c_str()));
}

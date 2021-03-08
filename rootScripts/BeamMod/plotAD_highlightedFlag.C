TVirtualPad* highlightedGraph(TCanvas* c1, TVirtualPad* p1, TChain* dit, int subpad = 1, std::string draw = "run:cyclenum", std::string cut = "", std::string cut2 = "0",std::string drawopts = "*", int color = 4, std::string cut3 = "0", int color3=2, std::string cut4 = "") {
  if (cut4 != "") {
    cut = cut+cut4;
    cut2 = cut2+cut4;
    if (cut3!= "") {
      cut3 = cut3+cut4;
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

void plotAD_highlightedFlag(std::string input = "NULL",std::string runNum = "", std::string slugNum = "0"){
  std::map<int,Double_t> runs;
  TChain* dit = new TChain("dit");
  dit->AddFile(input.c_str());
  std::string cut = "flag==1";
  if (slugNum != "0") {
    cut = Form("(%s && slug==%s)",cut.c_str(),slugNum.c_str());
  }
  std::string ana = Form("%s BMOD Outputs",input.c_str());
  if (runNum != "" && slugNum == "0" && ((TString)runNum).Atoi() >= 1000) {
    cut = Form("run == %s && flag==1",runNum.c_str());
    ana = Form("Run %s BMOD Outputs",runNum.c_str());
  }
  // FIXME cut2 has been superceded by a hardcoded flag==0 cut....
  std::string cut2 = Form("(%s && (slug==%s))",cut.c_str(),slugNum.c_str());
  std::string cut3 = Form("(slug==%s)",slugNum.c_str());
  if (runNum == "" && slugNum == "0") {
    cut2 = Form("%s",cut.c_str());
    cut3 = "";
  }

  if (runNum != "" && ((TString)runNum).Atoi() < 1000) {
    cut = Form("(segment == %s || (segment == %d))",runNum.c_str(),((TString)runNum).Atoi()+1);
    cut2 = Form("(segment == %s)",runNum.c_str());
    cut3 = Form("(segment == %s)",runNum.c_str());
    ana = Form("Segments %s (blue/purple), and %d (black/red), and neighbors (red) and all flag==0 (red/purple) BMOD Outputs",runNum.c_str(),((TString)runNum).Atoi()+1);
    // Explicitly remove the flag == 0 data from determining Axis, and ONLY plot segment == arg from the user
    //cut = Form("(%s && segment == %s)",cut.c_str(),runNum.c_str());
    //cut2 = cut;
    //cut3 = "";
    //ana = Form("Segment %s BMOD Outputs, red = cut out or neighboring segment",runNum.c_str());
  }
  if (runNum != "" && slugNum != "0") {
    dit->Draw(">>elist",Form("run>=%s",runNum.c_str()),"entrylist");  //picks out unique cycle numbers
    TEntryList *elist = (TEntryList*)gDirectory->Get("elist");
    //dit->SetEntryList(elist);
    TLeaf *l_flags = dit->GetLeaf("flag");
    TLeaf *l_runs = dit->GetLeaf("run");
    int nonzero = dit->Draw("run","flag==1","goff");
    int nGood  = 0;
    int nTotal = 0;
    Double_t flagi = 0.0;
    Double_t runi  = 0.0;
    //Printf("%lld entries",elist->GetN());
    while (nGood <= 2*std::atoi(slugNum.c_str()) && nTotal<elist->GetN()) {  
      //dit->GetEntry(nTotal);
      l_flags->GetBranch()->GetEntry(elist->GetEntry(nTotal));
      l_runs->GetBranch()->GetEntry(elist->GetEntry(nTotal));
      flagi = l_flags->GetValue(); //(Double_t)(((l_flags->GetBranch())->GetEntry(nGood))->GetValue());
      runi  = l_runs->GetValue();
      runs[nTotal] = runi;
      //Printf("ngood = %d, ntotal = %d, flag = %f",nGood,nTotal,flagi);
      if (flagi == 1) {
        nGood++;
      }
      nTotal++;
    }
    if (nGood!=0 && nTotal!=0) {
      nGood = nTotal/2;
      //nGood = std::atoi(slugNum.c_str());
    }
    // Treat slugNum as "number of runs to examine")
    // Cut for only doing 2 color version, always ignoring flag==0
    //cut = Form("flag==1 && (run>=%d && run<=%d)",(Int_t)runs[0],(Int_t)runs[runs.size()-1]);
    
    cut = Form("(run>=%d && run<=%d)",(Int_t)runs[0],(Int_t)runs[runs.size()-1]);
    cut2 = Form("(%s && (run>=%d && run<%d))",cut.c_str(),(Int_t)runs[0],(Int_t)runs[runs.size()/2]);
    cut3 = Form("(run>=%d && run<%d)",(Int_t)runs[0],(Int_t)runs[runs.size()/2]);
    ana = Form("Runs %d to %d (blue/purple), to %d (black), flag==0 (red/purple) BMOD Outputs",(Int_t)runs[0],(Int_t)runs[nGood-1],(Int_t)runs[runs.size()-1]);
    //cut = Form("flag==1 && (run>=%s && run<=(%s+(2*%d)+1))",runNum.c_str(),runNum.c_str(),nGood);
    //cut2 = Form("(%s && (run>=%s && run<(%s+%d)))",cut.c_str(),runNum.c_str(),runNum.c_str(),nGood);
    //cut3 = Form("(run>=%s && run<(%s+%d))",runNum.c_str(),runNum.c_str(),nGood);
    //ana = Form("Runs %s to %d (blue), to %d BMOD Outputs",runNum.c_str(),(std::atoi(runNum.c_str())+(nGood-1)),(std::atoi(runNum.c_str())+2*(nGood)+1));
  }

  if ( slugNum == "0" ){ 
    slugNum = "";
  }

  std::string pdfname = Form("%s%d_%dalphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),std::atoi(slugNum.c_str()),std::atoi(runNum.c_str()));

  if (((TString)pdfname).Contains(Form("_run%d",std::atoi(runNum.c_str())))) {
    pdfname = Form("%s%d_alphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),std::atoi(slugNum.c_str()));
  }

  TText *label = new TText(0.0,0.005,ana.c_str());
  label->SetTextFont(23);
  label->SetTextSize(14);
  label->SetNDC();

  TVirtualPad* p1;
  TCanvas* c1 = new TCanvas();
  c1->SetTitle(ana.c_str());
  c1->SetName(ana.c_str());
  c1->Divide(3,2);
  p1=highlightedGraph(c1,p1,dit,1,"alpha13_4eX1X:cyclenum", cut,"(flag==0)","*",2,cut3,4," && alpha13_4eX1X != 0");
  p1=highlightedGraph(c1,p1,dit,2,"alpha46_4eY4aY:cyclenum",cut,"(flag==0)","*",2,cut3,4," && alpha46_4eY4aY != 0");
  p1=highlightedGraph(c1,p1,dit,3,"delta13_12X4eX:cyclenum",cut,"(flag==0)","*",2,cut3,4," && delta13_12X4eX != 0");
  p1=highlightedGraph(c1,p1,dit,4,"delta46_12X4eY:cyclenum",cut,"(flag==0)","*",2,cut3,4," && delta46_12X4eY != 0");
  p1=highlightedGraph(c1,p1,dit,5,"run:cyclenum",cut,"(flag==0)","*",2,cut3,4);
  p1=highlightedGraph(c1,p1,dit,6,"segment:cyclenum",cut,"(flag==0)","*",2,cut3,4);
  c1->cd();
  label->Draw("same");
  c1->SaveAs(Form("%s(",pdfname.c_str()));
  TCanvas* c2 = new TCanvas();
  c2->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c2,p1,dit,1,"usl_1X:cyclenum", cut,"(flag==0)","*",2,cut3,4," && usl_1X != 0");
  p1=highlightedGraph(c2,p1,dit,2,"usl_4aY:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usl_4aY != 0");
  p1=highlightedGraph(c2,p1,dit,3,"usl_4eX:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usl_4eX != 0");
  p1=highlightedGraph(c2,p1,dit,4,"usl_4eY:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usl_4eY != 0");
  p1=highlightedGraph(c2,p1,dit,5,"usl_12X:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usl_12X != 0");
  p1=highlightedGraph(c2,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,cut3,4);
  c2->cd();
  label->Draw("same");
  c2->SaveAs(Form("%s",pdfname.c_str()));
  TCanvas* c3 = new TCanvas();
  c3->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c3,p1,dit,1,"usr_1X:cyclenum", cut,"(flag==0)","*",2,cut3,4," && usr_1X != 0" );
  p1=highlightedGraph(c3,p1,dit,2,"usr_4aY:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usr_4aY != 0");
  p1=highlightedGraph(c3,p1,dit,3,"usr_4eX:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usr_4eX != 0");
  p1=highlightedGraph(c3,p1,dit,4,"usr_4eY:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usr_4eY != 0");
  p1=highlightedGraph(c3,p1,dit,5,"usr_12X:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usr_12X != 0");
  p1=highlightedGraph(c3,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,cut3,4);
  c3->cd();
  label->Draw("same");
  c3->SaveAs(Form("%s",pdfname.c_str()));
  TCanvas* c4 = new TCanvas();
  c4->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c4,p1,dit,1,"usl_coil1:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usl_coil1 != 0");
  p1=highlightedGraph(c4,p1,dit,2,"usl_coil3:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usl_coil3 != 0");
  p1=highlightedGraph(c4,p1,dit,3,"usl_coil4:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usl_coil4 != 0");
  p1=highlightedGraph(c4,p1,dit,4,"usl_coil6:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usl_coil6 != 0");
  p1=highlightedGraph(c4,p1,dit,5,"usl_coil7:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usl_coil7 != 0");
  p1=highlightedGraph(c4,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,cut3,4);
  c4->cd();
  label->Draw("same");
  c4->SaveAs(Form("%s",pdfname.c_str()));
  TCanvas* c5 = new TCanvas();
  c5->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
//p1=highlightedGraph(c5,p1,dit,1,"usr_coil1:cyclenum",cut,cut2,"*",4); // This one just does 2 cuts directly, coloring over cut 1 in "same*" with cut 2 ( in color 4)
//p1=highlightedGraph(c5,p1,dit,2,"usr_coil3:cyclenum",cut,cut2,"*",4);
//p1=highlightedGraph(c5,p1,dit,3,"usr_coil4:cyclenum",cut,cut2,"*",4);
//p1=highlightedGraph(c5,p1,dit,4,"usr_coil6:cyclenum",cut,cut2,"*",4);
//p1=highlightedGraph(c5,p1,dit,5,"usr_coil7:cyclenum",cut,cut2,"*",4);
//p1=highlightedGraph(c5,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,cut3,4);
  p1=highlightedGraph(c5,p1,dit,1,"usr_coil1:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usr_coil1 != 0");
  p1=highlightedGraph(c5,p1,dit,2,"usr_coil3:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usr_coil3 != 0");
  p1=highlightedGraph(c5,p1,dit,3,"usr_coil4:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usr_coil4 != 0");
  p1=highlightedGraph(c5,p1,dit,4,"usr_coil6:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usr_coil6 != 0");
  p1=highlightedGraph(c5,p1,dit,5,"usr_coil7:cyclenum",cut,"(flag==0)","*",2,cut3,4," && usr_coil7 != 0");
  p1=highlightedGraph(c5,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,cut3,4);
  c5->cd();
  label->Draw("same");
  c5->SaveAs(Form("%s",pdfname.c_str()));
  TCanvas* c6 = new TCanvas();
  c6->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c6,p1,dit,1,"1X_coil1:cyclenum", cut,"(flag==0)","*",2,cut3,4," && 1X_coil1 != 0");
  p1=highlightedGraph(c6,p1,dit,2,"1X_coil3:cyclenum", cut,"(flag==0)","*",2,cut3,4," && 1X_coil3 != 0");
  p1=highlightedGraph(c6,p1,dit,3,"1X_coil7:cyclenum", cut,"(flag==0)","*",2,cut3,4," && 1X_coil7 != 0");
  p1=highlightedGraph(c6,p1,dit,4,"4eX_coil1:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 4eX_coil1 != 0");
  p1=highlightedGraph(c6,p1,dit,5,"4eX_coil3:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 4eX_coil3 != 0");
  p1=highlightedGraph(c6,p1,dit,6,"4eX_coil7:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 4eX_coil7 != 0");
  c6->cd();
  label->Draw("same");
  c6->SaveAs(Form("%s",pdfname.c_str()));
  TCanvas* c7 = new TCanvas();
  c7->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c7,p1,dit,1,"4aY_coil4:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 4aY_coil4 != 0");
  p1=highlightedGraph(c7,p1,dit,2,"4aY_coil6:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 4aY_coil6 != 0");
  p1=highlightedGraph(c7,p1,dit,3,"4aY_coil7:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 4aY_coil7 != 0");
  p1=highlightedGraph(c7,p1,dit,4,"4eY_coil4:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 4eY_coil4 != 0");
  p1=highlightedGraph(c7,p1,dit,5,"4eY_coil6:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 4eY_coil6 != 0");
  p1=highlightedGraph(c7,p1,dit,6,"4eY_coil7:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 4eY_coil7 != 0");
  c7->cd();
  label->Draw("same");
  c7->SaveAs(Form("%s",pdfname.c_str()));
  TCanvas* c8 = new TCanvas();
  c8->Divide(3,2);
  p1=highlightedGraph(c8,p1,dit,1,"12X_coil1:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 12X_coil1 != 0");
  p1=highlightedGraph(c8,p1,dit,2,"12X_coil3:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 12X_coil3 != 0");
  p1=highlightedGraph(c8,p1,dit,3,"12X_coil4:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 12X_coil4 != 0");
  p1=highlightedGraph(c8,p1,dit,4,"12X_coil6:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 12X_coil6 != 0");
  p1=highlightedGraph(c8,p1,dit,5,"12X_coil7:cyclenum",cut,"(flag==0)","*",2,cut3,4," && 12X_coil7 != 0");
  p1=highlightedGraph(c8,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,cut3,4);
  c8->cd();
  label->Draw("same");
  c8->SaveAs(Form("%s)",pdfname.c_str()));
}

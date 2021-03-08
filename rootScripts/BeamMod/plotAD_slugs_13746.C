TVirtualPad* highlightedGraph(TCanvas* c1, TVirtualPad* p1, TChain* dit, int subpad = 1, std::string draw = "run:cyclenum", std::string cut = "", std::string cut2 = "0",std::string drawopts = "*", int color = 4, std::string cut3 = "0", int color3=2) {
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
      dit->Draw(draw.c_str(),Form("%s && %s",cut2.c_str(),cut3.c_str()),Form("%ssame",drawopts.c_str()));
      if (p1->FindObject("Graph")) {
        ((TGraph*)p1->FindObject("Graph"))->SetName("color2and3");
        ((TGraph*)p1->FindObject("color2and3"))->SetMarkerColor(color+color3);
      }
    }
  }
  return p1;
}

void plotAD_slugs_13746(std::string input = "NULL",std::string runNum = "", std::string slugNum = ""){
  TChain* dit = new TChain("dit");
  std::string cut = "flag==1";
  if (runNum != "" && slugNum == "") {
    cut = Form("run == %s && flag==1",runNum.c_str());
  }
  dit->AddFile(input.c_str());
  TVirtualPad* p1;
  TCanvas* c1 = new TCanvas();
  c1->Divide(3,2);
  p1=highlightedGraph(c1,p1,dit,1,"alpha13_4eX1X:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c1,p1,dit,2,"alpha46_4eY4aY:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c1,p1,dit,3,"delta13_12X4eX:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c1,p1,dit,4,"delta46_12X4eY:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c1,p1,dit,5,"run:cyclenum",cut,"(flag==0)","*",2,Form("(slug==%s)",slugNum.c_str()),4);
  p1=highlightedGraph(c1,p1,dit,6,"segment:cyclenum",cut,Form("(%s && slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  c1->SaveAs(Form("%s%s_%salphas-deltas.pdf(",input.substr(0,input.find(".root")).c_str(),slugNum.c_str(),runNum.c_str()));
  TCanvas* c2 = new TCanvas();
  c2->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c2,p1,dit,1,"usl_1X:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c2,p1,dit,2,"usl_4aY:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c2,p1,dit,3,"usl_4eX:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c2,p1,dit,4,"usl_4eY:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c2,p1,dit,5,"usl_12X:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c2,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,Form("(slug==%s)",slugNum.c_str()),4);
  c2->SaveAs(Form("%s%s_%salphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),slugNum.c_str(),runNum.c_str()));
  TCanvas* c3 = new TCanvas();
  c3->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c3,p1,dit,1,"usr_1X:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c3,p1,dit,2,"usr_4aY:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c3,p1,dit,3,"usr_4eX:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c3,p1,dit,4,"usr_4eY:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c3,p1,dit,5,"usr_12X:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c3,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,Form("(slug==%s)",slugNum.c_str()),4);
  c3->SaveAs(Form("%s%s_%salphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),slugNum.c_str(),runNum.c_str()));
  TCanvas* c4 = new TCanvas();
  c4->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c4,p1,dit,1,"usl_coil1:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c4,p1,dit,2,"usl_coil3:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c4,p1,dit,3,"usl_coil4:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c4,p1,dit,4,"usl_coil6:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c4,p1,dit,5,"usl_coil7:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c4,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,Form("(slug==%s)",slugNum.c_str()),4);
  c4->SaveAs(Form("%s%s_%salphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),slugNum.c_str(),runNum.c_str()));
  TCanvas* c5 = new TCanvas();
  c5->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c5,p1,dit,1,"usr_coil1:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c5,p1,dit,2,"usr_coil3:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c5,p1,dit,3,"usr_coil4:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c5,p1,dit,4,"usr_coil6:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c5,p1,dit,5,"usr_coil7:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c5,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,Form("(slug==%s)",slugNum.c_str()),4);
  c5->SaveAs(Form("%s%s_%salphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),slugNum.c_str(),runNum.c_str()));
  TCanvas* c6 = new TCanvas();
  c6->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c6,p1,dit,1,"1X_coil1:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c6,p1,dit,2,"1X_coil3:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c6,p1,dit,3,"1X_coil7:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c6,p1,dit,4,"4eX_coil1:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c6,p1,dit,5,"4eX_coil3:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c6,p1,dit,6,"4eX_coil7:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  c6->SaveAs(Form("%s%s_%salphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),slugNum.c_str(),runNum.c_str()));
  TCanvas* c7 = new TCanvas();
  c7->Divide(3,2);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  p1=highlightedGraph(c7,p1,dit,1,"4aY_coil4:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c7,p1,dit,2,"4aY_coil6:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c7,p1,dit,3,"4aY_coil7:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c7,p1,dit,4,"4eY_coil4:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c7,p1,dit,5,"4eY_coil6:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c7,p1,dit,6,"4eY_coil7:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  c7->SaveAs(Form("%s%s_%salphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),slugNum.c_str(),runNum.c_str()));
  TCanvas* c8 = new TCanvas();
  c8->Divide(3,2);
  p1=highlightedGraph(c8,p1,dit,1,"12X_coil1:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c8,p1,dit,2,"12X_coil3:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c8,p1,dit,3,"12X_coil4:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c8,p1,dit,4,"12X_coil6:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c8,p1,dit,5,"12X_coil7:cyclenum",cut,Form("%s && (slug==%s)",cut.c_str(),slugNum.c_str()),"*",4);
  p1=highlightedGraph(c8,p1,dit,6,"run:cyclenum",cut,"(flag==0)","*",2,Form("(slug==%s)",slugNum.c_str()),4);
  c8->SaveAs(Form("%s%s_%salphas-deltas.pdf)",input.substr(0,input.find(".root")).c_str(),slugNum.c_str(),runNum.c_str()));
}

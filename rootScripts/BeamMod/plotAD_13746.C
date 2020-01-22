void plotAD_13746(std::string input = "NULL",std::string runNum = ""){
  TChain* dit = new TChain("dit");
  std::string cut = "";
  if (runNum != "") {
    cut = Form("run == %s",runNum.c_str());
  }
  dit->AddFile(input.c_str());
  TCanvas* c1 = new TCanvas();
  c1->cd();
  dit->Draw("alpha13_4eX4aX:cyclenum",cut.c_str(),"*");
  c1->SaveAs(Form("%s_%salphas-deltas.pdf(",input.substr(0,input.find(".root")).c_str(),runNum.c_str()));
  TCanvas* c2 = new TCanvas();
  c2->cd();
  dit->Draw("alpha46_4eY4aY:cyclenum",cut.c_str(),"*");
  c2->SaveAs(Form("%s_%salphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),runNum.c_str()));
  TCanvas* c3 = new TCanvas();
  c3->cd();
  dit->Draw("delta13_11X12X4eX:cyclenum",cut.c_str(),"*");
  c3->SaveAs(Form("%s_%salphas-deltas.pdf",input.substr(0,input.find(".root")).c_str(),runNum.c_str()));
  TCanvas* c4 = new TCanvas();
  c4->cd();
  dit->Draw("delta46_11X12X4eY:cyclenum",cut.c_str(),"*");
  c4->SaveAs(Form("%s_%salphas-deltas.pdf)",input.substr(0,input.find(".root")).c_str(),runNum.c_str()));
}

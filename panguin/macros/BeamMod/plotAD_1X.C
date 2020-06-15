#include "realtimeBmodAna.C"
void plotAD_1X(Int_t runnum = 999999, Long_t coils = 1357642){
  realtimeBmodAna();
  gStyle->SetOptStat(0);
  TPad *c2 = new TPad("cBMWPlotSens","cBMWPlotSens",0,0,1,1);
  c2->SetGrid();
  Int_t slug_number = 0;
  if(runnum == 999999){
    TString tmpRunFile = gDirectory->GetName();
    TString tmpRunNum = "0";
    if (tmpRunFile.Contains("999999")){
      Printf("Doing online rootfile BMOD analysis");
      tmpRunNum = "999999";
      TString s2 = gSystem->GetFromPipe("~/scripts/getSlugNumber");
      slug_number = s2.Atoi();
    }
    else {
      tmpRunNum = tmpRunFile(tmpRunFile.Length()-13,4);
      Printf("Doing run %s BMOD analysis",tmpRunNum.Data());
      runnum = tmpRunNum.Atoi();
      slug_number = gSystem->GetFromPipe(Form("rcnd %d slug",runnum)).Atoi();
    }
  }
  else {
    slug_number = gSystem->GetFromPipe(Form("rcnd %d slug",runnum)).Atoi();
  }
  TChain* dit = new TChain("dit");
  Printf("Current run %d Alphas and Deltas",runnum);
  Printf("Getting alphas and deltas from /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/slopes_1X/dithering_slopes_%ld_slug%d.root",coils,slug_number);
  dit->AddFile(Form("/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/slopes_1X/dithering_slopes_%ld_slug%d.root",coils,slug_number));
  //dit->AddFile(Form("/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/slopes_1X/tonight.root",coils,slug_number));
  
  //dit->Draw("alpha35_4eXaX:cyclenum:runnum>>run", "", "goff");
  //TGraph *g=new TGraph(dit->GetV2(), dit->GetV3());
  c2->Divide(4,3);
  c2->Draw();
  c2->cd(1)->SetGrid();
  //dit->Draw("alpha35_4eX1X:scandata1","","*");
  //dit->Draw("alpha35_4eX1X:scandata2","","*");
  dit->Draw("alpha35_4eX1X:cyclenum","","*");
  c2->cd(2)->SetGrid();
  dit->Draw("alpha15_4eX1X:cyclenum","","*");
  c2->cd(3)->SetGrid();
  dit->Draw("delta35_11X12X1X:cyclenum","","*");
  c2->cd(4)->SetGrid();
  dit->Draw("delta15_11X12X1X:cyclenum","","*");
  c2->cd(5)->SetGrid();
  dit->Draw("alpha13_4eX1X:cyclenum","","*");
  c2->cd(6)->SetGrid();
  dit->Draw("alpha46_4eY4aY:cyclenum","","*");
  c2->cd(7)->SetGrid();
  dit->Draw("delta13_11X12X1X:cyclenum","","*");
  c2->cd(8)->SetGrid();
  dit->Draw("delta46_11X12X4aY:cyclenum","","*");
  c2->cd(9)->SetGrid();
  dit->Draw("alpha26_4eY4aY:cyclenum","","*");
  c2->cd(10)->SetGrid();
  dit->Draw("alpha24_4eY4aY:cyclenum","","*");
  c2->cd(11)->SetGrid();
  dit->Draw("run:cyclenum","","*");
  c2->cd(12)->SetGrid();
  dit->Draw("11X12X_coil7:cyclenum","","*");
  //c2->SaveAs(Form("%s_alphas-deltas.pdf",input.substr(0,input.find(".root")).c_str()));
}

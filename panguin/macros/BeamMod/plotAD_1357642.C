#include "realtimeBmodAna.C"
void plotAD_1357642(Int_t runnum = 999999, Long_t coils = 1357642){
  //realtimeBmodAna();
  gStyle->SetOptStat(0);
  TCanvas* c2 = new TCanvas();
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
  Printf("Getting alphas and deltas from /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/slopes/dithering_slopes_%ld_slug%d.root",coils,slug_number);
  dit->AddFile(Form("/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/slopes/dithering_slopes_%ld_slug%d.root",coils,slug_number));
  c2->Divide(4,3);
  c2->Draw();
  c2->cd(1);
  //dit->Draw("alpha35_4eX4aX:scandata1","","*");
  //dit->Draw("alpha35_4eX4aX:scandata2","","*");
  dit->Draw("alpha35_4eX4aX:cyclenum","","*");
  c2->cd(2);
  dit->Draw("alpha15_4eX4aX:cyclenum","","*");
  c2->cd(3);
  dit->Draw("delta35_11X12X4aX:cyclenum","","*");
  c2->cd(4);
  dit->Draw("delta15_11X12X4aX:cyclenum","","*");
  c2->cd(5);
  dit->Draw("alpha13_4eX4aX:cyclenum","","*");
  c2->cd(6);
  dit->Draw("alpha46_4eY4aY:cyclenum","","*");
  c2->cd(7);
  dit->Draw("delta13_11X12X4aX:cyclenum","","*");
  c2->cd(8);
  dit->Draw("delta46_11X12X4aY:cyclenum","","*");
  c2->cd(9);
  dit->Draw("alpha26_4eY4aY:cyclenum","","*");
  c2->cd(10);
  dit->Draw("alpha24_4eY4aY:cyclenum","","*");
  c2->cd(11);
  dit->Draw("delta26_11X12X4aY:cyclenum","","*");
  c2->cd(12);
  dit->Draw("delta24_11X12X4aY:cyclenum","","*");
  c2->SaveAs(Form("/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/slopes/dithering_alphas-deltas_%ld_%d.pdf",coils,slug_number));
}

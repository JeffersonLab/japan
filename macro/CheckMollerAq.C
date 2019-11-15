void CheckMollerAq(int run_num, TString ucut="1")
{
 TFile *rootfile = TFile::Open(Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num));
  TTree *mul= (TTree*)rootfile->Get("mul");

  TString mycut = Form("%s&&yield_bcm_dg_us>0",ucut.Data());

TCanvas *c1 = new TCanvas("c1","c1",50,50,1700,1000);
 c1->Divide(2,2);
 c1->cd(1);
 mul->Draw("yield_bcm_dg_us:CodaEventNumber",mycut.Data());
 c1->cd(2);
 mul->Draw("asym_bcm_dg_us/ppm:CodaEventNumber",mycut.Data());
 c1->cd(3);
 mul->Draw("yield_bcm_dg_us",mycut.Data());
   c1->cd(4);
   mul->Draw("asym_bcm_dg_us/ppm",mycut.Data());


}

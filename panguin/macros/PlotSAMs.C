void PlotSAMs(TString type="evt"){
  gStyle->SetOptStat(0);
  TTree* tree = (TTree*)gDirectory->Get(type);
  TString evcut = "CodaEventNumber>10";

  TPad *tPad = new TPad("tPad","",0,0,1,1);
  tPad->Divide(2,4);
  tPad->Draw();

  TGraph *tGraph[8];

  for(int i=0;i<8;i++){
    tPad->cd(i+1);
    int npt = tree->Draw(Form("sam%d:CodaEventNumber>>h%d",i+1,i),
			 evcut,"goff");
    tGraph[i] = new TGraph(npt,tree->GetV2(),tree->GetV1());
    tGraph[i]->SetLineStyle(1);
    tGraph[i]->SetLineColor(4);
    tGraph[i]->Draw("AL");
    tGraph[i]->SetTitle(Form("SAM %d",i+1));
  }
}
  

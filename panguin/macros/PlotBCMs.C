void PlotBCMs(TString type="evt"){
  gStyle->SetOptStat(0);
  TTree* tree = (TTree*)gDirectory->Get(type);
  TString evcut = "CodaEventNumber>10";

  TPad *tPad = new TPad("tPad","",0,0,1,1);
  tPad->Divide(2,2);
  tPad->Draw();

  TString device[4]={"bcm_dg_us","bcm_dg_ds","bcm_an_ds3","bcm_an_ds10"};

  TGraph *tGraph[4];

  for(int i=0;i<4;i++){
    tPad->cd(i+1);
    int npt= tree->Draw(Form("%s:CodaEventNumber>>h%d",device[i].Data(),i),
	       evcut,"goff");
    tGraph[i] = new TGraph(npt,tree->GetV2(),tree->GetV1());
    tGraph[i]->SetLineStyle(1);
    tGraph[i]->SetLineColor(4);
    tGraph[i]->Draw("AL");
    tGraph[i]->SetTitle(Form(" %s",device[i].Data() ));
  }
}

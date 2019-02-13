void PlotBPMs(TString type="evt", TString ref="CodaEventNumber"){
  gStyle->SetOptStat(0);
  TTree* tree = (TTree*)gDirectory->Get(type);
  TString evcut = "CodaEventNumber>10";

  TPad *tPad = new TPad("tPad","",0,0,0.66,1);
  tPad->Divide(2,2);
  tPad->Draw();

  TPad *tPad2 = new TPad("tPad2","",0.66,0,1,1);
  tPad2->Divide(1,2);
  tPad2->Draw();

  tPad2->cd(1);
  tree->Draw("bpm4aY:bpm4aX","ErrorFlag==0","COL");
  tPad2->cd(2);
  tree->Draw("bpm4eY:bpm4eX","ErrorFlag==0","COL");

  TString wire[4]={"bpm4aX","bpm4aY","bpm4eX","bpm4eY"};

  TGraph *tGraph[4];

  for(int i=0;i<4;i++){
    tPad->cd(i+1);
    int npt= tree->Draw(Form("%s:CodaEventNumber>>h%d",wire[i].Data(),i),
	       evcut,"goff");

    tGraph[i] = new TGraph(npt,tree->GetV2(),tree->GetV1());
    tGraph[i]->SetLineStyle(1);
    tGraph[i]->SetLineColor(4);
    tGraph[i]->Draw("AL");
    tGraph[i]->SetTitle(Form(" %s",wire[i].Data() ));
  }

}
  

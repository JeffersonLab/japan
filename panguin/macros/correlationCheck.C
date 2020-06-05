///Panguin gui compatible diagnostic plots for shift taking online replay 
void correlationCheck() {
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(0);
  TTree* tree_R = (TTree*)gDirectory->Get("mul");
  
  const Int_t nVar = 5;
  TString drawBPMs[nVar] = {"diff_bpm1X","diff_bpm4aY","diff_bpm4eX","diff_bpm4eY","diff_bpm12X"};
  Double_t iv_mean[nVar];
  Double_t iv_rms[nVar];

  TPad *cPlot = new TPad("cPlot","cPlot",0,0,1,1);
  cPlot->Divide(nVar,nVar);
  cPlot->Draw();
  TVirtualPad *pad_buff;


  for(int i=0;i<nVar;i++){
    tree_R->Draw(Form("%s>>hiv%d",drawBPMs[i].Data(),i),"ErrorFlag==0","goff");
    TH1D *hbuff = (TH1D*)gDirectory->FindObject(Form("hiv%d",i));
    if(hbuff){
      iv_mean[i] = hbuff->GetMean();
      iv_rms[i] = hbuff->GetRMS();
    }
    else{
      iv_mean[i] = 0.0;
      iv_rms[i] = 0.0;
    }
    //delete hbuff;
  }

  TGraph * tGraphPlot[nVar*nVar];  //plots one super cycle
  for (int i=0;i<nVar; i++){
    for (int j=0;j<nVar; j++) {
      pad_buff=cPlot->cd(i+nVar*j+1);
      tree_R->Draw(Form("%s:%s",drawBPMs[i].Data(),drawBPMs[j].Data()),"ErrorFlag==0");
      TGraph * tg = (TGraph*)pad_buff->FindObject("Graph");
      Double_t corD = 0.0;
      TLatex * cor;
      if (tg) {
        corD = tg->GetCorrelationFactor();
        cor = new TLatex(0,0,Form("Cor factor = %0.4f",corD));
        cor->SetTextFont(42);
        cor->SetTextSize(0.055);
        if (corD>0.95 || corD<-0.95) {
          cor->SetTextColor(kRed);
        }
      }
      //delete tg;
      tree_R->Draw(Form("%s:%s",drawBPMs[i].Data(),drawBPMs[j].Data()),"ErrorFlag==0","prof");
      TH1F * th1 = (TH1F*)pad_buff->FindObject("htemp");
      if (th1) {
        th1->SetTitle(Form("%s vs %s",drawBPMs[i].Data(),drawBPMs[j].Data()));
        th1->Fit("pol1","QR","",
            iv_mean[j]-2*iv_rms[j],
            iv_mean[j]+2*iv_rms[j]);
        pad_buff->Update();
        TF1* f1 = th1->GetFunction("pol1");
        if (f1) {
          Double_t slope = f1->GetParameter(1);
          TPaveStats* lst = (TPaveStats*)pad_buff->GetPrimitive("stats");
          lst->SetName("MyStats");
          lst->SetOptFit(1);
          lst->SetOptStat(0);
          if(slope<0){
            lst->SetX2NDC(1.0);
            lst->SetY2NDC(0.9);
            lst->SetX1NDC(0.5);
            lst->SetY1NDC(0.6);
          }
          else{
            lst->SetX2NDC(0.5);
            lst->SetY2NDC(0.6);
            lst->SetX1NDC(0.0);
            lst->SetY1NDC(0.9);
          }
          if (cor) {
            if (drawBPMs[i] != drawBPMs[j]) {
              lst->GetListOfLines()->Add(cor);
              th1->SetStats(0);
              pad_buff->Modified();
            }
          }
          lst->Draw();
        }
      }
    }
  }

  gStyle->SetOptFit(0);
  gStyle->SetOptStat(1);
}

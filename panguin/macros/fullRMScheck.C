////////BeamModCycle.C
///Panguin gui compatible diagnostic plots for shift taking online replay 

void fullRMScheck(TString type = "burst_lrb_std", Double_t refRMSlrRaw = 1700.0, Double_t refRMSlrReg = 1050.0, Double_t refRMScomboRaw = 1100.0, Double_t refRMScomboReg = 776.0, Double_t burstLen = 9000.0) {
  gStyle->SetOptStat(0);
  TTree* tree_R = (TTree*)gDirectory->Get(type);
  //tree_R->AddFriend((TTree*)gDirectory->Get("burst"));

  int num = tree_R->Draw("MYp[0]","","goff"); // Number of miniruns
  if(num<1) {
    TPad *cPlot = new TPad("cPlot","cPlot",0,0,1,1);
    cPlot->Divide(1,3);
    cPlot->Draw();
    TLatex line1;
    TLatex line2;
    TLatex line3;
    line1.SetTextSize(.3);
    line1.SetTextAlign(12);
    line2.SetTextSize(.2);
    line2.SetTextAlign(12);
    line3.SetTextSize(.15);
    line3.SetTextAlign(12);

    cPlot->cd(1);
    line1.DrawLatex(.25,.5,"No Miniruns");

    cPlot->cd(2);
    line2.DrawLatex(.25,.5,"Completed Yet");

    cPlot->cd(3);
    line3.DrawLatex(.12,.75,"(Wait for 5 minutes of good data)");
    return 0;
  }
  
  TPad *cPlot = new TPad("cPlot","cPlot",0,0,1,1);
  cPlot->Divide(2,2);
  cPlot->Draw();

  // I want 4 sub-canvases of TGraphErrors multi-graphs
  // Each one does a different thing, raw/regressed, means/RMSs
  //   both left and right in one plot, purple and green
  //   red will be a reference line indicating the max expected level
  //
  TString drawRaw[4] = {"dMY[0]*1e6","dMY[1]*1e6","dMY[2]*1e6","dMY[3]*1e6"};
  TString drawReg[4] = {"dMYp[0]*1e6","dMYp[1]*1e6","dMYp[2]*1e6","dMYp[3]*1e6"};
  TMultiGraph *tMGraphPlot[4];  //plots one super cycle
  TGraph *tGraphPlotRaw[4];  //plots one super cycle
  TGraph *tGraphPlotReg[4];  //plots one super cycle
  TGraph *tGraphPlotRef[4];  //plots one super cycle
  for (int i=0;i<4; i++){
    int nptRaw = tree_R->Draw(Form("%s:0:Entry$:0",drawRaw[i].Data()),"","goff");
    tGraphPlotRaw[i]= new TGraphErrors(nptRaw,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
    tGraphPlotRaw[i]->SetLineStyle(1);
    if (i == 0 || i == 2){
      tGraphPlotRaw[i]->SetLineColor(3);
      tGraphPlotRaw[i]->SetMarkerColor(3);
    }
    else {
      tGraphPlotRaw[i]->SetLineColor(4);
      tGraphPlotRaw[i]->SetMarkerColor(4);
    }
    tGraphPlotRaw[i]->SetMarkerStyle(20);
    tGraphPlotRaw[i]->SetMarkerSize(1.0);
    int nptReg = tree_R->Draw(Form("%s:0:Entry$:0",drawReg[i].Data()),"","goff");
    tGraphPlotReg[i]= new TGraphErrors(nptReg,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
    tGraphPlotReg[i]->SetLineStyle(1);
    if (i == 0 || i == 2){
      tGraphPlotReg[i]->SetLineColor(3);
      tGraphPlotReg[i]->SetMarkerColor(3);
    }
    else {
      tGraphPlotReg[i]->SetLineColor(4);
      tGraphPlotReg[i]->SetMarkerColor(4);
    }
    tGraphPlotReg[i]->SetMarkerStyle(20);
    tGraphPlotReg[i]->SetMarkerSize(1.0);

    int nptRMSRef = 0;
    if (i==0){
      nptRMSRef = tree_R->Draw(Form("%f:0:Entry$:0",refRMSlrRaw),"","goff");
    }
    if (i==1){
      nptRMSRef = tree_R->Draw(Form("%f:0:Entry$:0",refRMScomboRaw),"","goff");
    }
    if (i==2){
      nptRMSRef = tree_R->Draw(Form("%f:0:Entry$:0",refRMSlrReg),"","goff");
    }
    if (i==3){
      nptRMSRef = tree_R->Draw(Form("%f:0:Entry$:0",refRMScomboReg),"","goff");
    }
    tGraphPlotRef[i]= new TGraphErrors(nptRMSRef,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
    tGraphPlotRef[i]->SetLineStyle(2);
    tGraphPlotRef[i]->SetMarkerColor(2);
    tGraphPlotRef[i]->SetLineColor(2);
    
  }
  for (int i=0;i<4; i++){
    tMGraphPlot[i] = new TMultiGraph();
    if (i==0){
      tMGraphPlot[i]->SetTitle("Raw US L and R main detector RMSs");
    }
    if (i==1){
      tMGraphPlot[i]->SetTitle("Regressed US L and R main detector RMSs");
    }
    if (i==2){
      tMGraphPlot[i]->SetTitle("Raw US combo main detector RMSs");
    }
    if (i==3){
      tMGraphPlot[i]->SetTitle("Regressed US combo main detector RMSs");
    }
    tMGraphPlot[i]->GetXaxis()->SetTitle("Burst Number");
    tMGraphPlot[i]->GetYaxis()->SetTitle("RMS (ppm)");
    if (i == 0) {
      tMGraphPlot[i]->Add(tGraphPlotRaw[0],"P");
      tMGraphPlot[i]->Add(tGraphPlotRaw[1],"P");
    }
    if (i == 1) {
      tMGraphPlot[i]->Add(tGraphPlotReg[0],"P");
      tMGraphPlot[i]->Add(tGraphPlotReg[1],"P");
    }
    if (i == 2) {
      tMGraphPlot[i]->Add(tGraphPlotRaw[2],"P");
      tMGraphPlot[i]->Add(tGraphPlotRaw[3],"P");
    }
    if (i == 3) {
      tMGraphPlot[i]->Add(tGraphPlotReg[2],"P");
      tMGraphPlot[i]->Add(tGraphPlotReg[3],"P");
    }
    tMGraphPlot[i]->Add(tGraphPlotRef[i],"LP");
    cPlot->cd(i+1);
    tMGraphPlot[i]->Draw("A");
  }
  cPlot->cd(1);
  //auto legend = new TLegend(0.1,0.7,0.4,0.9);
  auto legend1 = new TLegend(0.6,0.7,0.9,0.925);
  legend1->SetHeader("L/R Main Dets","C");
  legend1->SetNColumns(1);
  legend1->AddEntry(tGraphPlotReg[0],"Left","p");
  legend1->AddEntry(tGraphPlotReg[1],"Right","p");
  legend1->AddEntry(tGraphPlotRef[0],"Max RMS","lp");
  legend1->SetTextSize(.06);
  legend1->Draw();

  cPlot->cd(3);
  //auto legend = new TLegend(0.1,0.7,0.4,0.9);
  auto legend2 = new TLegend(0.6,0.7,0.9,0.925);
  legend2->SetHeader("Combo Main Dets","C");
  legend2->SetNColumns(1);
  legend2->AddEntry(tGraphPlotReg[2],"Avg","p");
  legend2->AddEntry(tGraphPlotReg[3],"DD","p");
  legend2->AddEntry(tGraphPlotRef[0],"Max RMS","lp");
  legend2->SetTextSize(.06);
  legend2->Draw();

  // FIXME wants:
  // * Conditional warning text about too large RMSs
  // * Better red line
  // * Description of what is shown and what it should be
  //
  /*
  cPlot->cd(4);
  TLatex Line1;
  TLatex Line2;
  TLatex Line3;
  Line1.SetTextSize(.2);
  Line1.SetTextAlign(12);
  Line2.SetTextSize(.15);
  Line2.SetTextAlign(12);
  Line3.SetTextSize(.3);
  Line3.SetTextAlign(12);

  cPlot->cd(4);
  Line1.DrawLatex(.3,.25,"Stability Errors!");
  Line2.DrawLatex(.25,.5,"If RMS exceeds line");
  Line3.DrawLatex(.12,.75,"Call RC now!");
  */
}

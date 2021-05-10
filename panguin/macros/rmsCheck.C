////////BeamModCycle.C
///Panguin gui compatible diagnostic plots for shift taking online replay 

void rmsCheck(TString type = "burst_lrb_std", Double_t refRMSraw = 1900.0, Double_t refRMSreg = 1125.0, Double_t burstLen = 9000.0) {
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
  //TString drawRawMean[2] = {"burst.asym_usl.hw_sum","burst.asym_usr.hw_sum"};
  //TString drawRawRMS[2]  = {"burst.asym_usl.hw_sum_err*1e6*sqrt(burst.asym_usl.num_samples)","burst.asym_usr.hw_sum_err*1e6*sqrt(burst.asym_usr.num_samples)"};
  //TString drawRawMean[2] = {"MY[0]*1e6","MY[1]*1e6"};
  //TString drawRawRMS[2]  = {"dMY[0]*1e6","dMY[1]*1e6"};
  //TString drawRegMean[2] = {"MYp[0]*1e6","MYp[1]*1e6"};
  //TString drawRegRMS[2]  = {"dMYp[0]*1e6","dMYp[1]*1e6"};
  TString drawUSL[4] = {"MY[0]*1e6","dMY[0]*1e6","MYp[0]*1e6","dMYp[0]*1e6"};
  TString drawUSLerr[4] = {Form("dMY[0]*1e6/sqrt(%f)",burstLen),"0",Form("dMYp[0]*1e6/sqrt(%f)",burstLen),"0"};
  TString drawUSR[4] = {"MY[1]*1e6","dMY[1]*1e6","MYp[1]*1e6","dMYp[1]*1e6"};
  TString drawUSRerr[4] = {Form("dMY[1]*1e6/sqrt(%f)",burstLen),"0",Form("dMY[1]*1e6/sqrt(%f)",burstLen),"0"};
  TMultiGraph *tMGraphPlot[4];  //plots one super cycle
  TGraph *tGraphPlotL[4];  //plots one super cycle
  TGraph *tGraphPlotR[4];  //plots one super cycle
  TGraph *tGraphPlotRef[4];  //plots one super cycle
  for (int i=0;i<4; i++){
    int nptL = tree_R->Draw(Form("%s:%s:Entry$:0",drawUSL[i].Data(),drawUSLerr[i].Data()),"","goff");
    //int npt = tree_R->Draw(Form("%s:burst.BurstCounter>>hc%d",draw[i].Data(),i),"","goff");
    tGraphPlotL[i]= new TGraphErrors(nptL,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
    tGraphPlotL[i]->SetLineStyle(1);
    tGraphPlotL[i]->SetLineColor(3);
    tGraphPlotL[i]->SetMarkerStyle(20);
    tGraphPlotL[i]->SetMarkerSize(0.5);
    tGraphPlotL[i]->SetMarkerColor(3);

    int nptR = tree_R->Draw(Form("%s:%s:Entry$:0",drawUSR[i].Data(),drawUSRerr[i].Data()),"","goff");
    tGraphPlotR[i]= new TGraphErrors(nptR,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
    tGraphPlotR[i]->SetLineStyle(1);
    tGraphPlotR[i]->SetLineColor(4);
    tGraphPlotR[i]->SetMarkerStyle(20);
    tGraphPlotR[i]->SetMarkerSize(0.5);
    tGraphPlotR[i]->SetMarkerColor(4);

    if (i==1 || i==3){
      int nptRMSRef = 0;
      if (i==1){
        nptRMSRef = tree_R->Draw(Form("%f:0:Entry$:0",refRMSraw),"","goff");
      }
      if (i==3){
        nptRMSRef = tree_R->Draw(Form("%f:0:Entry$:0",refRMSreg),"","goff");
      }
      tGraphPlotRef[i]= new TGraphErrors(nptRMSRef,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
      tGraphPlotRef[i]->SetLineStyle(2);
      tGraphPlotRef[i]->SetMarkerColor(2);
      tGraphPlotRef[i]->SetLineColor(2);
    }

    tMGraphPlot[i] = new TMultiGraph();
    if (i==0){
      tMGraphPlot[i]->SetTitle("Raw US L and R main detector Means");
    }
    if (i==1){
      tMGraphPlot[i]->SetTitle("Raw US L and R main detector RMSs");
    }
    if (i==2){
      tMGraphPlot[i]->SetTitle("Regressed US L and R main detector Means");
    }
    if (i==3){
      tMGraphPlot[i]->SetTitle("Regressed US L and R main detector RMSs");
    }
    tMGraphPlot[i]->GetXaxis()->SetTitle("Burst Number");
    tMGraphPlot[i]->GetYaxis()->SetTitle("Mean (ppm)");
    tMGraphPlot[i]->Add(tGraphPlotL[i],"P");
    tMGraphPlot[i]->Add(tGraphPlotR[i],"P");
    if (i==1 || i==3){
      tMGraphPlot[i]->Add(tGraphPlotRef[i],"LP");
      tMGraphPlot[i]->GetYaxis()->SetTitle("RMS (ppm)");
    }
    cPlot->cd(i+1);
    tMGraphPlot[i]->Draw("A");
  }
  
  cPlot->cd(3);
  //auto legend = new TLegend(0.1,0.7,0.4,0.9);
  auto legend1 = new TLegend(0.7,0.7,0.9,0.925);
  legend1->SetHeader("Main Dets","C");
  legend1->SetNColumns(1);
  legend1->AddEntry(tGraphPlotL[0],"Left","p");
  legend1->AddEntry(tGraphPlotR[1],"Right","p");
  legend1->AddEntry(tGraphPlotRef[3],"Max RMS","lp");
  legend1->Draw();

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

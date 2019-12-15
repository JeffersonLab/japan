////////realtimeBmodAna.C
///Panguin gui compatible diagnostic plots for shift taking online replay 

void realtimeBmodAna(Int_t nMins = 5, Int_t runNum = 999999,TString config = "/chafs2/work1/apar/japan-bmodAna/rootScripts/BeamMod/inputs/input_day1-CREX.txt"){
  gStyle->SetOptStat(0);
  //TTree* tree_R = (TTree*)gDirectory->Get(mul); // FIXME Use BMOD Slopes to do regression on the fly!
  //tree_R->AddFriend((TTree*)gDirectory->Get("burst"));

  TDatime *Ttime = new TDatime();
  Int_t time = Ttime->GetTime();
  if (((Int_t)time)%(nMins*60) > 0 && ((Int_t)time)%(nMins*60) < 1200) { // 15 second window, once every n minutes
    Printf("Running BMOD Analysis from inside PANGUIN");
    gSystem->Exec(Form("root -l -b -q ~/PREX/prompt/bmodAna/bmodAna.C'(%d,\"%s\")'",runNum,config.Data()));
  }

//  TPad *cPlot = new TPad("cPlot","cPlot",0,0,1,1);
//  cPlot->Divide(2,2);
//  cPlot->Draw();

  /*
  TString drawUSL[4] = {"MY[0]*1e6","dMY[0]*1e6","MYp[0]*1e6","dMYp[0]*1e6"};
  TString drawUSLerr[4] = {Form("dMY[0]*1e6/sqrt(%f)",burstLen),Form("dMYp[0]*1e6/sqrt(%f)",burstLen),"0","0"};
  TString drawUSR[4] = {"MY[2]*1e6","dMY[2]*1e6","MYp[2]*1e6","dMYp[2]*1e6"};
  TString drawUSRerr[4] = {Form("dMY[2]*1e6/sqrt(%f)",burstLen),Form("dMY[2]*1e6/sqrt(%f)",burstLen),"0","0"};
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
  */
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

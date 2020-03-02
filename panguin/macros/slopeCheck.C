///Panguin gui compatible diagnostic plots for shift taking online replay 
//#include "GetStats.C"
void slopeCheck(TString type = "burst_lrb_std", Double_t refRMS4eX = 10, Double_t refRMS12X = -40, Double_t burstLen = 9000.0) {
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
  cPlot->Divide(3,2);
  cPlot->Draw();

  // I want 5 sub-canvases of TGraphErrors multi-graphs
  //
  TString drawSlopesL[5] = {"A[0][0]","A[1][0]","A[2][0]","A[3][0]","A[4][0]"};
  TString drawSlopesLerr[5] = {"dA[0][0]","dA[1][0]","dA[2][0]","dA[3][0]","dA[4][0]"};
  TString drawSlopesR[5] = {"A[0][1]","A[1][1]","A[2][1]","A[3][1]","A[4][1]"};
  TString drawSlopesRerr[5] = {"dA[0][1]","dA[1][1]","dA[2][1]","dA[3][1]","dA[4][1]"};
  TMultiGraph *tMGraphPlot[5];  //plots one super cycle
  TGraph *tGraphPlotL[5];  //plots one super cycle
  TGraph *tGraphPlotR[5];  //plots one super cycle
  TGraph *tGraphPlotRef[5];  //plots one super cycle
  for (int i=0;i<5; i++){
    int nptL = tree_R->Draw(Form("%s*1000:%s*1000:Entry$:0",drawSlopesL[i].Data(),drawSlopesLerr[i].Data()),"","goff");
    //int npt = tree_R->Draw(Form("%s:burst.BurstCounter>>hc%d",draw[i].Data(),i),"","goff");
    tGraphPlotL[i]= new TGraphErrors(nptL,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
    tGraphPlotL[i]->SetLineStyle(1);
    tGraphPlotL[i]->SetLineColor(3);
    tGraphPlotL[i]->SetMarkerStyle(20);
    tGraphPlotL[i]->SetMarkerSize(0.5);
    tGraphPlotL[i]->SetMarkerColor(3);

    int nptR = tree_R->Draw(Form("%s*1000:%s*1000:Entry$:0",drawSlopesR[i].Data(),drawSlopesRerr[i].Data()),"","goff");
    //int npt = tree_R->Draw(Form("%s:burst.BurstCounter>>hc%d",draw[i].Data(),i),"","goff");
    tGraphPlotR[i]= new TGraphErrors(nptR,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
    tGraphPlotR[i]->SetLineStyle(1);
    tGraphPlotR[i]->SetLineColor(4);
    tGraphPlotR[i]->SetMarkerStyle(20);
    tGraphPlotR[i]->SetMarkerSize(0.5);
    tGraphPlotR[i]->SetMarkerColor(4);

    if (i==2 || i==4){
      int nptRMSRef = 0;
      if (i==2){
        nptRMSRef = tree_R->Draw(Form("%f:0:Entry$:0",refRMS4eX),"","goff");
      }
      if (i==4){
        nptRMSRef = tree_R->Draw(Form("%f:0:Entry$:0",refRMS12X),"","goff");
      }
      tGraphPlotRef[i]= new TGraphErrors(nptRMSRef,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
      tGraphPlotRef[i]->SetLineStyle(2);
      tGraphPlotRef[i]->SetMarkerColor(2);
      tGraphPlotRef[i]->SetLineColor(2);
    }

    tMGraphPlot[i] = new TMultiGraph();
    if (i==0){
      tMGraphPlot[i]->SetTitle("1X Slopes");
    }
    if (i==1){
      tMGraphPlot[i]->SetTitle("4aY Slopes");
    }
    if (i==2){
      tMGraphPlot[i]->SetTitle("4eX Slopes");
    }
    if (i==3){
      tMGraphPlot[i]->SetTitle("4eY Slopes");
    }
    if (i==4){
      tMGraphPlot[i]->SetTitle("12X Slopes");
    }
    tMGraphPlot[i]->GetXaxis()->SetTitle("Burst Number");
    tMGraphPlot[i]->GetYaxis()->SetTitle("Slopes");
    tMGraphPlot[i]->Add(tGraphPlotL[i],"P");
    tMGraphPlot[i]->Add(tGraphPlotR[i],"P");
    if (i==2 || i==4){
      tMGraphPlot[i]->Add(tGraphPlotRef[i],"LP");
//      tMGraphPlot[i]->GetYaxis()->SetTitle("Expected");
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
  legend1->AddEntry(tGraphPlotRef[4],"Expected","lp");
  legend1->SetTextSize(.09);
  legend1->Draw();

  cPlot->cd(6);

  GetStats();

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

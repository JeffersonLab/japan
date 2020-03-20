///Panguin gui compatible diagnostic plots for shift taking online replay 
//#include "GetStats.C"
#include <iostream>
#include <fstream>
void longSlopeCheck(TString type = "burst_lrb_std", Double_t refRMS4eX = 10, Double_t refRMS12X = -40, Double_t burstLen = 9000.0) {
  gStyle->SetOptStat(0);
  TTree* tree_R = (TTree*)gDirectory->Get(type);
  TChain* tree_agg = new TChain("agg");
  gSystem->Exec("export runNum=`~/scripts/getRunNumber`; export runNum2=`echo $runNum-30 | bc`; python /adaqfs/home/apar/pvdb/prex/examples/make_run_list.py --run=$runNum2-$runNum --type=Production --current=20 --target=48Ca --slug=`~/scripts/getSlugNumber`");
  //gSystem->Exec("export runNum=`~/scripts/getRunNumber`; export runNum2=`echo $runNum-100 | bc`; python /adaqfs/home/apar/pvdb/prex/examples/make_run_list.py --run=$runNum2-$runNum --type=Production --current=20 --target=48Ca --slug=149");
  std::ifstream* infile = new std::ifstream("/adaqfs/home/apar/PREX/japan/panguin/list.txt");
  TString currentRunNum = gSystem->GetFromPipe("/adaqfs/home/apar/scripts/getRunNumber");
  std::vector<Int_t> listOfFileIndices;
  std::string line;
  if (infile->is_open()) {
    while(getline(*infile,line)){
      listOfFileIndices.push_back(atoi(line.c_str()));
    }
  }
  for (Int_t i = listOfFileIndices.size()-1; i > listOfFileIndices.size()-3 || i > 0; i-- ){ // Just do 3  most recent runs
    Printf("Trying run %d",listOfFileIndices.at(i));
    std::string tmpInfile = "/adaqfs/home/apar/PREX/prompt/aggRootfiles/minirun_aggregator_#_*.root";
    std::string infile1 = tmpInfile.substr(0,tmpInfile.find("#"));
    std::string infile2 = tmpInfile.substr(tmpInfile.find("#")+1,tmpInfile.size()-infile1.size()-1);
    tmpInfile = Form("%s%d%s",infile1.c_str(),listOfFileIndices.at(i),infile2.c_str());
    tree_agg->Add(tmpInfile.c_str());
  }
  
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
  TString drawSlopesAggL[5] = {"reg_asym_usl_diff_bpm1X_slope","reg_asym_usl_diff_bpm4aY_slope","reg_asym_usl_diff_bpm4eX_slope","reg_asym_usl_diff_bpm4eY_slope","reg_asym_usl_diff_bpm12X_slope"};
  TString drawSlopesAggR[5] = {"reg_asym_usr_diff_bpm1X_slope","reg_asym_usr_diff_bpm4aY_slope","reg_asym_usr_diff_bpm4eX_slope","reg_asym_usr_diff_bpm4eY_slope","reg_asym_usr_diff_bpm12X_slope"};
  TString drawSlopesL[5] = {"A[0][0]","A[1][0]","A[2][0]","A[3][0]","A[4][0]"};
  TString drawSlopesLerr[5] = {"dA[0][0]","dA[1][0]","dA[2][0]","dA[3][0]","dA[4][0]"};
  TString drawSlopesR[5] = {"A[0][1]","A[1][1]","A[2][1]","A[3][1]","A[4][1]"};
  TString drawSlopesRerr[5] = {"dA[0][1]","dA[1][1]","dA[2][1]","dA[3][1]","dA[4][1]"};
  TMultiGraph *tMGraphPlot[5];  //plots one super cycle
  TGraph *tGraphPlotAggL[5];  //plots one super cycle
  TGraph *tGraphPlotAggR[5];  //plots one super cycle
  TGraph *tGraphPlotL[5];  //plots one super cycle
  TGraph *tGraphPlotR[5];  //plots one super cycle
  //TGraph *tGraphPlotRef[5];  //plots one super cycle
  for (int i=0;i<5; i++){
    int nptAggL = tree_agg->Draw(Form("%s*1000:0:run_number+0.1*minirun_n:0",drawSlopesAggL[i].Data()),"","goff");
    //int npt = tree_R->Draw(Form("%s:burst.BurstCounter>>hc%d",draw[i].Data(),i),"","goff");
    tGraphPlotAggL[i]= new TGraphErrors(nptAggL,tree_agg->GetV3(),tree_agg->GetV1(),tree_agg->GetV4(),tree_agg->GetV2());
    tGraphPlotAggL[i]->SetLineStyle(1);
    tGraphPlotAggL[i]->SetLineColor(3);
    tGraphPlotAggL[i]->SetMarkerStyle(20);
    tGraphPlotAggL[i]->SetMarkerSize(0.5);
    tGraphPlotAggL[i]->SetMarkerColor(3);

    int nptL = tree_R->Draw(Form("%s*1000:%s*1000:%s+0.1*Entry$:0",drawSlopesL[i].Data(),drawSlopesLerr[i].Data(),currentRunNum.Data()),"","goff");
    //int npt = tree_R->Draw(Form("%s:burst.BurstCounter>>hc%d",draw[i].Data(),i),"","goff");
    tGraphPlotL[i]= new TGraphErrors(nptL,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
    tGraphPlotL[i]->SetLineStyle(1);
    tGraphPlotL[i]->SetLineColor(3);
    tGraphPlotL[i]->SetMarkerStyle(20);
    tGraphPlotL[i]->SetMarkerSize(0.5);
    tGraphPlotL[i]->SetMarkerColor(3);

    int nptAggR = tree_agg->Draw(Form("%s*1000:0:run_number+0.1*minirun_n:0",drawSlopesAggR[i].Data()),"","goff");
    //int npt = tree_R->Draw(Form("%s:burst.BurstCounter>>hc%d",draw[i].Data(),i),"","goff");
    tGraphPlotAggR[i]= new TGraphErrors(nptAggR,tree_agg->GetV3(),tree_agg->GetV1(),tree_agg->GetV4(),tree_agg->GetV2());
    tGraphPlotAggR[i]->SetLineStyle(1);
    tGraphPlotAggR[i]->SetLineColor(4);
    tGraphPlotAggR[i]->SetMarkerStyle(20);
    tGraphPlotAggR[i]->SetMarkerSize(0.5);
    tGraphPlotAggR[i]->SetMarkerColor(4);

    int nptR = tree_R->Draw(Form("%s*1000:%s*1000:%s+0.1*Entry$:0",drawSlopesR[i].Data(),drawSlopesRerr[i].Data(),currentRunNum.Data()),"","goff");
    //int npt = tree_R->Draw(Form("%s:burst.BurstCounter>>hc%d",draw[i].Data(),i),"","goff");
    tGraphPlotR[i]= new TGraphErrors(nptR,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
    tGraphPlotR[i]->SetLineStyle(1);
    tGraphPlotR[i]->SetLineColor(4);
    tGraphPlotR[i]->SetMarkerStyle(20);
    tGraphPlotR[i]->SetMarkerSize(0.5);
    tGraphPlotR[i]->SetMarkerColor(4);

    /*if (i==2 || i==4){
      int nptRMSRef = 0;
      if (i==2){
        nptRMSRef = tree_R->Draw(Form("%f:0:%s+0.1*Entry$:0",refRMS4eX,currentRunNum.Data()),"","goff");
      }
      if (i==4){
        nptRMSRef = tree_R->Draw(Form("%f:0:%s+0.1*Entry$:0",refRMS12X,currentRunNum.Data()),"","goff");
      }
      tGraphPlotRef[i]= new TGraphErrors(nptRMSRef,tree_R->GetV3(),tree_R->GetV1(),tree_R->GetV4(),tree_R->GetV2());
      tGraphPlotRef[i]->SetLineStyle(2);
      tGraphPlotRef[i]->SetMarkerColor(2);
      tGraphPlotRef[i]->SetLineColor(2);
    }*/

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
    tMGraphPlot[i]->GetXaxis()->SetTitle("Run Number + 0.1*Burst Number");
    tMGraphPlot[i]->GetYaxis()->SetTitle("Slopes");
    tMGraphPlot[i]->Add(tGraphPlotAggL[i],"P");
    tMGraphPlot[i]->Add(tGraphPlotAggR[i],"P");
    tMGraphPlot[i]->Add(tGraphPlotL[i],"P");
    tMGraphPlot[i]->Add(tGraphPlotR[i],"P");
    //if (i==2 || i==4){
    //  tMGraphPlot[i]->Add(tGraphPlotRef[i],"LP");
    //}
    cPlot->cd(i+1);
    tMGraphPlot[i]->Draw("A");
  }
  
  cPlot->cd(3);
  //auto legend = new TLegend(0.1,0.7,0.4,0.9);
  //auto legend1 = new TLegend(0.7,0.7,0.9,0.925);
  auto legend1 = new TLegend(0.1,0.4,0.45,0.6);
  legend1->SetHeader("Main Dets","C");
  legend1->SetNColumns(1);
  legend1->AddEntry(tGraphPlotL[0],"Left","p");
  legend1->AddEntry(tGraphPlotR[1],"Right","p");
  //legend1->AddEntry(tGraphPlotRef[4],"Expected","lp");
  legend1->SetTextSize(.09);
  legend1->Draw();

  cPlot->cd(6);

  GetStats();
  infile->close();

}

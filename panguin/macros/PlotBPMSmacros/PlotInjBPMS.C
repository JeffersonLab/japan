// Injector BPMs Plot Macro 
// Source from Caryn Palatchi
// Adapted by Tao Ye for JAPAN 
// Adapted by KDP and PK for panguin
// Last Update : 05-2019

//nclude "device_list.h"
// vector<const char*> vBPM = {"bpm2i01","bpm2i02","bpm1i02","bpm1i04",
//                                "bpm1i06", "bpm0i01","bpm0i01a","bpm0i02",
//                                "bpm0i02a","bpm0i05","bpm0i07","bpm0l01",
//                                "bpm0l02","bpm0l03","bpm0l04","bpm0l05",
//                                "bpm0l06","bpm0l07","bpm0l10", "bpm0r02",
//                                "bpm0r05"};

vector<const char*> vBPMall = {"bpm2i01","bpm2i02","bpm1i02","bpm1i04",
                               "bpm1i06", "bpm0i01","bpm0i01a","bpm0i02",
                               "bpm0i02a","bpm0i05","bpm0i07","bpm0l01",
			    "bpm0l02","bpm0l03","bpm0l04","bpm0l05","bpm0l06",
			    "bpm0l07","bpm0l08","bpm0l09","bpm0l10","bpm0r03",
			    "bpm0r05"};

void PlotInjBPMS(TString ucut){

  const int intper = 9000;
  //collect data points from first run file
  TTree* evt_tree = (TTree*)gROOT->FindObject("evt");
  TTree* pair_tree = (TTree*)gROOT->FindObject("pr");
  TTree* mul_tree = (TTree*)gROOT->FindObject("mul");

  Int_t ndeti = vBPMall.size();
  Double_t* Aq= new Double_t[ndeti];
  Double_t* eAq= new Double_t[ndeti];
  Double_t* rmsAq= new Double_t[ndeti];
  Double_t* Dx= new Double_t[ndeti];
  Double_t* Dy= new Double_t[ndeti];
  Double_t* eDx= new Double_t[ndeti];
  Double_t* eDy= new Double_t[ndeti];
  Double_t* rmsDx= new Double_t[ndeti];
  Double_t* rmsDy= new Double_t[ndeti];
  Double_t* zero= new Double_t[ndeti];

  Int_t nument = mul_tree->GetEntries();

  //TString lcut = ucut +  Form(" && Entry$>(%d-%d)",nument,intper) ;
  TString lcut = ucut;

  TPad *pad1= new TPad("pad1","pad1",0,0,1,1);
  pad1->Divide(2,2);
  pad1->Draw();
  TH1D* thishist;
  TString objnam;
  for(Int_t dd=0;dd<ndeti;dd++) {
    mul_tree->Draw(Form("1e6*asym_%sWS>>hista%d",vBPMall[dd],dd),lcut,"goff");
    objnam = Form("hista%d",dd);
    thishist = (TH1D*)gDirectory->FindObject(objnam);
    Aq[dd] = thishist->GetMean();
    eAq[dd]= thishist->GetMeanError();
    rmsAq[dd]= thishist->GetRMS();    
    zero[dd]=0;
    // delete thishist;
    mul_tree->Draw(Form("1e3*diff_%sX>>histx%d",vBPMall[dd],dd),lcut,"goff");
    objnam = Form("histx%d",dd);
    thishist = (TH1D*)gDirectory->FindObject(objnam);
    Dx[dd] = thishist->GetMean();
    eDx[dd]= thishist->GetMeanError();
    rmsDx[dd]= thishist->GetRMS();
    // delete thishist;
    mul_tree->Draw(Form("1e3*diff_%sY>>histy%d",vBPMall[dd],dd),lcut,"goff");
    objnam = Form("histy%d",dd);
    thishist = (TH1D*)gDirectory->FindObject(objnam);
    Dy[dd] = thishist->GetMean();
    eDy[dd]= thishist->GetMeanError();
    rmsDy[dd]= thishist->GetRMS();
    // delete thishist;
    // cout<<vBPMall[dd]<<" Aq[ppm](RMS)+-error,Dx[nm](RMS)+-err,Dy[nm](RMS)+-err: "<<Aq[dd]<<" "<<rmsAq[dd]<<" "<<eAq[dd]<<" "<<1e3*Dx[dd]<<" "<<1e3*rmsDx[dd]<<" "<<1e3*eDx[dd]<<" "<<1e3*Dy[dd]<<" "<<1e3*rmsDy[dd]<<" "<<1e3*eDy[dd]<<endl;

  }

  const Int_t numchains=ndeti;
  Double_t chain[numchains];
  Double_t chainerr[numchains];
  for (Int_t i=0;i<numchains;i++){
    chain[i]=i+1;
    chainerr[i]=0.0;
  };

  TGraphErrors *Dxgraph;
  TGraphErrors *Dygraph;
  TGraphErrors *Dxrmsgraph;
  TGraphErrors *Dyrmsgraph;
  TGraphErrors *Aqgraph;
  TGraphErrors *Aqrmsgraph;

  Dxgraph = new TGraphErrors(numchains,chain,&(Dx[0]),chainerr,&(eDx[0]));
  Dygraph= new TGraphErrors(numchains,chain,&(Dy[0]),chainerr,&(eDy[0]));
  Dxrmsgraph = new TGraphErrors(numchains,chain,&(rmsDx[0]),chainerr,&(zero[0]));
  Dyrmsgraph= new TGraphErrors(numchains,chain,&(rmsDy[0]),chainerr,&(zero[0]));
  Aqgraph= new TGraphErrors(numchains,chain,&(Aq[0]),chainerr,&(eAq[0]));
  Aqrmsgraph = new TGraphErrors(numchains,chain,&(rmsAq[0]),chainerr,&(zero[0]));

  
  TMultiGraph *xygraph = new TMultiGraph();
  TMultiGraph *xyrmsgraph = new TMultiGraph();

  // TAxis *ax = Dygraph->GetHistogram()->GetXaxis();
  // Double_t x1 = ax->GetBinLowEdge(1);
  // Double_t x2 = ax->GetBinUpEdge(ax->GetNbins());
  Double_t x1 = chain[0]-0.5;
  Double_t x2 = chain[numchains-1]+0.5;

  TVirtualPad* pad_buff;

  pad_buff= pad1->cd(1);
  Dygraph->SetMarkerStyle(21);
  Dygraph->SetMarkerColor(3);
  Dygraph->SetLineColor(3);

  Dxgraph->SetMarkerStyle(21);
  Dxgraph->SetMarkerColor(4);
  Dxgraph->SetLineColor(4);

  xygraph->Add(Dygraph,"p");
  xygraph->Add(Dxgraph,"p");
  xygraph->Draw("AP");

  TLegend *leg_xygraph = new TLegend(0.9,0.7,0.99,0.99);
  leg_xygraph->AddEntry(Dygraph,"Dy");
  leg_xygraph->AddEntry(Dxgraph,"Dx");
  leg_xygraph->Draw("same");

  xygraph->SetTitle(Form("Diff: Cut=, %s ",ucut.Data()));
  xygraph->GetYaxis()->SetTitle(" um, blue=Dx, green=Dy");
  xygraph->GetHistogram()->GetXaxis()->Set(numchains,x1,x2);
  for(Int_t k=0;k<numchains;k++){
    xygraph->GetHistogram()->GetXaxis()->SetBinLabel(k+1,vBPMall[k]);
  }
  xygraph->GetHistogram()->GetXaxis()->SetLabelSize(0.05);
  xygraph->GetHistogram()->GetXaxis()->LabelsOption("v");
  pad_buff->SetGrid();

  pad_buff = pad1->cd(3);
  Dyrmsgraph->SetMarkerStyle(21);
  Dyrmsgraph->SetMarkerColor(3);
  Dyrmsgraph->SetLineColor(3);

  Dxrmsgraph->SetMarkerStyle(21);
  Dxrmsgraph->SetMarkerColor(4);
  Dxrmsgraph->SetLineColor(4);

  xyrmsgraph->Add(Dxrmsgraph,"p");
  xyrmsgraph->Add(Dyrmsgraph,"p");
  xyrmsgraph->Draw("AP");

  TLegend *leg_xyrmsgraph = new TLegend(0.9,0.7,0.99,0.99);
  leg_xyrmsgraph->AddEntry(Dyrmsgraph,"Dy RMS");
  leg_xyrmsgraph->AddEntry(Dxrmsgraph,"Dx RMS");
  leg_xyrmsgraph->Draw("same");

  xyrmsgraph->SetTitle(Form("Diff RMS: Cut=  %s ",ucut.Data()));
  xyrmsgraph->GetYaxis()->SetTitle("RMS um, blue=Dx, green=Dy");
  xyrmsgraph->GetHistogram()->GetXaxis()->Set(numchains,x1,x2);
  for(Int_t k=0;k<numchains;k++){
    xyrmsgraph->GetHistogram()->GetXaxis()->SetBinLabel(k+1,vBPMall[k]);
  }
  xyrmsgraph->GetHistogram()->GetXaxis()->SetLabelSize(0.05);
  xyrmsgraph->GetHistogram()->GetXaxis()->LabelsOption("v");
  pad_buff->SetGrid();

  pad_buff= pad1->cd(2);
  Aqgraph->SetMarkerStyle(21);
  Aqgraph->SetMarkerColor(4);
  Aqgraph->SetLineColor(4);
  Aqgraph->Draw("AP");

  Aqgraph->SetTitle(Form("Aq: Cut = %s ",ucut.Data()));
  Aqgraph->GetYaxis()->SetTitle(" ppm");
  Aqgraph->GetHistogram()->GetXaxis()->Set(numchains,x1,x2);
  for(Int_t k=0;k<numchains;k++){
    Aqgraph->GetHistogram()->GetXaxis()->SetBinLabel(k+1,vBPMall[k]);
  }
  Aqgraph->GetHistogram()->GetXaxis()->SetLabelSize(0.05);
  Aqgraph->GetHistogram()->GetXaxis()->LabelsOption("v");
  pad_buff->SetGrid();

  pad_buff = pad1->cd(4);
  Aqrmsgraph->SetMarkerStyle(21);
  Aqrmsgraph->SetMarkerColor(4);
  Aqrmsgraph->SetLineColor(4);
  Aqrmsgraph->Draw("AP");

  Aqrmsgraph->SetTitle(Form("Aq RMS: Cut =  %s ",ucut.Data()));
  Aqrmsgraph->GetYaxis()->SetTitle("RMS ppm");
  Aqrmsgraph->GetHistogram()->GetXaxis()->Set(numchains,x1,x2);
  for(Int_t k=0;k<numchains;k++){
    Aqrmsgraph->GetHistogram()->GetXaxis()->SetBinLabel(k+1,vBPMall[k]);
  }
  Aqrmsgraph->GetHistogram()->GetXaxis()->SetLabelSize(0.05);
  Aqrmsgraph->GetHistogram()->GetXaxis()->LabelsOption("v");
  pad_buff->SetGrid();

}


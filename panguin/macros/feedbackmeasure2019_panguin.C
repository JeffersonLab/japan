void feedbackmeasure2019_panguin(TString mon, TString ucut="1", Int_t N = 0, Double_t PITAslp = 0, Int_t ev0=0, Int_t freq = 120, Int_t helpattern =8 )
{//N is interval in number of events
  //freq is helicity flip rate: i.e. 30Hz. 
  //helpattern =2 for pairs, =4 for quad, =8 for octet.
  //mon is actual full name "asym_bpm1i02WS" or "diff_bpm1i02WS". 
  //usage example
  //be sure to do evt_scanclean[0]==4,1,etc. with the [0] or else will get wrong. 
  //also be sure to do evt_scandata1[0]>.... or else will get wrong. 
  //.x macro/makefeedbackplot2018.C(4883,"evt_scanclean[0]==1&&ok_cut&&avg_qpd1sum>260&&abs(asym_qpd1sum)<50e3",60*30,1698*0.525/100.)

  TTree *m = (TTree*)gROOT->FindObject("mul");
  TPad *pad1 = new TPad("pad1", "pad1", 0, 0, 1, 1);
  pad1->Divide(3,2);
  pad1->Draw();
  TVirtualPad* pad_buff;
  pad_buff = pad1->cd(1);

  m->Draw("CodaEventNumber>>h1",ucut.Data());
  TH1 *h1 = (TH1*)gROOT->FindObject("h1");
  Int_t Nevents=helpattern*h1->GetEntries();
  printf("%i \n", Nevents);
  if (ev0==0){
   ev0 =floor(h1->GetMean()-Nevents/2);
  }

  const Int_t Ntemp=floor(Nevents/N);
  const Int_t Nintervals = Ntemp;
  Double_t Aq[Nintervals];
  Double_t AqRMS[Nintervals];
  Double_t eAq[Nintervals];
  Double_t PITA[Nintervals];
  Double_t Aqcorr[Nintervals];
  Double_t Aqavg[Nintervals];
  Double_t AqRMSavg[Nintervals];
  Double_t x[Nintervals];
  Double_t ex[Nintervals];
  Double_t dset[Nintervals];
  Double_t edset[Nintervals];
  Double_t N_1[Nintervals];
  Double_t rootN[Nintervals];
  Double_t N_1m[Nintervals];
  Double_t rootNm[Nintervals];
  TH1* thishist;
  TString objnam;

  pad_buff = pad1->cd(1);
  
  for(Int_t ii=0; ii<Nintervals; ii++ ){
    m->Draw(Form("%s*1e6>>hAq%d",mon.Data(),ii),Form("CodaEventNumber>(%d+%d)&&CodaEventNumber<(%d+%d)&&%s",ev0,ii*N,ev0,(ii+1)*N,ucut.Data()));
    objnam=Form("hAq%d",ii);
    thishist = (TH1*)gROOT->FindObject(objnam);
    Aq[ii]=thishist->GetMean();
    AqRMS[ii]=thishist->GetMeanError();
    eAq[ii]=thishist->GetMeanError();
    printf("%f %i\n", AqRMS[ii], ii);
    delete thishist;
    x[ii]=(ev0+(ii+0.5)*N)/freq/60;
    ex[ii]=0;
    dset[ii]=ii;
    edset[ii]=0;
    PITA[ii]=-Aq[ii]/PITAslp;
    printf("%f PITA\n", PITA[ii]);
    if(ii>1){
    Aqcorr[ii]=Aq[ii];//+PITA[ii-1]*(PITAslp);
    Aqavg[ii]=(Aqcorr[ii]+Aqavg[ii-1]*(ii))/(ii+1);
    AqRMSavg[ii]=(AqRMS[ii]+AqRMSavg[ii-1]*(ii))/(ii+1);
    N_1[ii]=AqRMSavg[ii]/(ii+1-1);
    rootN[ii]=AqRMSavg[ii]/sqrt(ii+1-1);
    N_1m[ii]=-N_1[ii];
    rootNm[ii]=-rootN[ii];
    //    N_1[ii]=AqRMS[ii]/(ii+1-1);
    // rootN[ii]=AqRMS[ii]/sqrt(ii+1-1);
    // N_1m[ii]=-AqRMS[ii]/(ii+1-1);
    //rootNm[ii]=-AqRMS[ii]/sqrt(ii+1-1);
    }
    if(ii==1){
      Aqcorr[ii]=Aq[ii];//+PITA[ii-1]*(PITAslp);
      Aqavg[ii]=Aqcorr[ii];
      AqRMSavg[ii]=AqRMS[ii];
      N_1[ii]=AqRMS[ii];
      rootN[ii]=AqRMS[ii];
      N_1m[ii]=-AqRMS[ii];
      rootNm[ii]=-AqRMS[ii];
    }
      if(ii==0){
	Aqcorr[ii]=Aq[ii];
	Aqavg[ii]=Aq[ii];
	AqRMSavg[ii]=AqRMS[ii];
        N_1[ii]=AqRMS[ii];
        rootN[ii]=AqRMS[ii];
        N_1m[ii]=-AqRMS[ii];
        rootNm[ii]=-AqRMS[ii];
      }
  }



  // TGraphErrors *gcorr = new TGraphErrors(Nintervals-1, &(dset[1]), &(Aqcorr[1]), &(edset[1]),&(AqRMS[1]));
 TGraphErrors *gcorr = new TGraphErrors(Nintervals-1, &(dset[1]), &(Aqcorr[1]), &(edset[1]),&(eAq[1]));
  
  TGraph *gcorrpnts = new TGraph(Nintervals-1, &(dset[1]), &(Aqcorr[1]));
  TGraphErrors *gavg = new TGraphErrors(Nintervals-1, &(dset[1]),  &(Aqavg[1]),  &(edset[1]),  &(rootN[1]));
  TGraph *gavgpnts = new TGraph(Nintervals-1, &(dset[1]),  &(Aqavg[1]));
  TGraph *gN_1 = new TGraph(Nintervals-1, &(dset[1]), &(N_1[1]));
  TGraph *grootN =  new TGraph(Nintervals-1, &(dset[1]), &(rootN[1]));
  TGraph *gN_1m = new TGraph(Nintervals-1, &(dset[1]), &(N_1m[1]));
  TGraph *grootNm =  new TGraph(Nintervals-1, &(dset[1]), &(rootNm[1]));

  TGraphErrors *g =new TGraphErrors(Nintervals,x,Aq,ex,AqRMS);
  TGraph *gPITA = new TGraph(Nintervals,x,PITA);
  // m->Draw(Form("%6.8f*asym_bcm1+%6.8f*asym_qpd1sum:CodaEventNumber",noisepower,pitapower),ucut.Data());
  g->SetMarkerStyle(6);
  g->SetLineStyle(1);
  g->SetLineColor(2);
  g->SetLineWidth(2);
  g->SetMarkerColor(2);
  g->SetTitle(Form("%s vs time(min)",mon.Data()));
  g->Draw("APL");
  m->Draw(Form("%s*1e6:CodaEventNumber/%d/60",mon.Data(),freq),ucut.Data(),"same");
  //g->SetMinimum(-AqRMS[0]);
  //g->SetMaximum(AqRMS[0]);
  g->Draw("PL&&same");
  
  pad_buff = pad1->cd(4);
  gPITA->SetTitle("Correcting Voltage vs time(min)");
  gPITA->SetLineColor(4);
  gPITA->SetLineWidth(4);
  gPITA->SetMarkerColor(4);
  gPITA->Draw("APL");

  pad_buff = pad1->cd(6);
  gcorr->SetMarkerStyle(22);
  gcorr->SetLineStyle(1);
  gcorr->SetMarkerColor(4);
  gcorr->SetMarkerSize(2);
  gcorr->SetLineWidth(1);
  gcorr->SetTitle(Form("%s vs interval#",mon.Data()));
  gcorr->Draw("APL");

  pad_buff = pad1->cd(3);
  gcorrpnts->SetMarkerStyle(22);
  gcorrpnts->SetLineStyle(1);
  gcorrpnts->SetMarkerColor(4);
  gcorrpnts->SetMarkerSize(2);
  gcorrpnts->SetLineWidth(1);
  gcorrpnts->SetTitle(Form("%s vs interval#",mon.Data()));
  gcorrpnts->Draw("APL"); 


  pad_buff = pad1->cd(2);
  //c1->cd(2);
  //  gcorr->SetMarkerStyle(22);
  // gcorr->SetMarkerColor(4);
  //gcorr->SetLineColor(4);
  //gcorr->SetLineWidth(2);
  //gcorr->Draw("APL");

  gavg->SetMarkerStyle(21);
  gavg->SetLineStyle(1);
  gavg->SetMarkerColor(4);
  gavg->SetLineWidth(1);
  gavg->SetTitle(Form("Accumulated avg. %s vs interval#",mon.Data()));
  gavg->Draw("APL");

  gN_1->SetLineColor(2);
  gN_1->SetMarkerColor(2);
  gN_1->Draw("PL&&same");
  gN_1->SetLineWidth(2);
  gN_1m->SetLineColor(2);
  gN_1m->SetMarkerColor(2);
  gN_1m->SetLineWidth(2);
  gN_1m->Draw("PL&&same");

  //  grootN->SetLineColor(5);
  // grootN->SetMarkerColor(5);
  grootN->SetLineStyle(2);
  grootN->Draw("PL&&same");
 grootNm->SetLineStyle(2);
  //  grootNm->SetLineColor(5);
  // grootNm->SetMarkerColor(5);
  grootNm->Draw("PL&&same");

  //g->DrawCopy("same");
  //  m->Draw("asym_qpd1sum:CodaEventNumber",ucut.Data(),"same");
  // c1->cd(1)->SetLogy(1);
  // g->Fit("gaus");
  // TF1 *f = g->GetFunction("gaus");
  //f->SetLineColor(2);
  //f->SetLineWidth(1);

  pad_buff = pad1->cd(5);
 gavgpnts->SetMarkerStyle(21);
  gavgpnts->SetLineStyle(1);
  gavgpnts->SetMarkerColor(4);
  gavgpnts->SetLineWidth(1);
  gavgpnts->SetTitle(Form("Accumulated avg. %s vs interval#",mon.Data()));
  gavgpnts->SetMinimum(-AqRMSavg[Nintervals-1]);
  gavgpnts->SetMaximum(AqRMSavg[Nintervals-1]);
  // gavgpnts->SetMinimum(-0.1);
  // gavgpnts->SetMaximum(0.1);

  gavgpnts->Draw("APL");


  gN_1->SetLineColor(2);
  gN_1->SetMarkerColor(2);
  gN_1->Draw("PL&&same");
  gN_1->SetLineWidth(2);
  gN_1m->SetLineColor(2);
  gN_1m->SetMarkerColor(2);
  gN_1m->SetLineWidth(2);
  gN_1m->Draw("PL&&same");

 grootN->SetLineStyle(2);
  grootN->Draw("PL&&same");
 grootNm->SetLineStyle(2);
  grootNm->Draw("PL&&same");

 

}

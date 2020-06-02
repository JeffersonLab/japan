#include <fstream>
void pullUtil(TH1*,TCanvas*);
void DrawOneChannel(TString, TCut ,TTree*, string, TCanvas*);
void Init(TTree*);
TF1 *fg, *fconst;
ofstream ofile;

void drawPostPan(string rootFnm, string dataTitle, string inputList){
  TFile *fin = TFile::Open(rootFnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("T");
  Init(t);

  //Think about saving these to a rootfile directly
  //TFile *fout = new TFile(Form("o_postPanPlots_%s.root",title.c_str()),"RECREATE");

  TCanvas *c1 = new TCanvas("c1","c1",1800,900);

  c1->Print(Form("y_postPanPlots_%s.pdf[",dataTitle.c_str()),"pdf");
  std::fstream infile(inputList);
  string draw;
  ofile.open(Form("o_%s.txt",dataTitle.c_str()),std::ofstream::out);
  while ( std::getline(infile,draw) ) 
  {
    c1->Clear();
    DrawOneChannel(draw,"",t,dataTitle,c1);
  }
  c1->Print(Form("y_postPanPlots_%s.pdf]",dataTitle.c_str()),"pdf");
  ofile<<endl;
  ofile.close();
  //fout->Close();

  fin->Close();
}

void DrawOneChannel(TString channelName, TCut userCut,TTree *t, string dataTitle, TCanvas *c1){

  //TCanvas *c1 = new TCanvas(Form("c1_%s",channelName.Data()),channelName,1200,600);

  Int_t npt = t->Draw("run:Entry$",userCut,"goff");
  double* dummy = t->GetV1();
  double* run =new double[npt];
  for(int ipt=0;ipt<npt;ipt++)
    run[ipt] = *(dummy+ipt);
  
  t->Draw("mini:Entry$",userCut,"goff");
  double* mini = new double[npt];
  dummy = t->GetV1();
  for(int ipt=0;ipt<npt;ipt++)
    mini[ipt] = *(dummy+ipt);

  TH1D *h;
  npt=t->Draw(channelName+":Entry$",userCut,"goff");
  double* mean = new double[npt];
  double* ent = new double[npt];
  dummy = t->GetV1();
  h = new TH1D(Form("h_%s",channelName.Data()),
	       Form("%s: %s",dataTitle.c_str(),channelName.Data()),
	       npt+2,0,npt+2);
  double min(99999999),max(-99999999);
  for(int ipt=0;ipt<npt;ipt++){
    h->SetBinContent(ipt+2,*(dummy+ipt));
    if(min > *(dummy+ipt))
      min = *(dummy+ipt);
    if(max < *(dummy+ipt))
      max = *(dummy+ipt);
  }

  h->SetMarkerStyle(20);
  //h->GetXaxis()->SetTitle("Run.Mini");
  TH1D *hCorr = new TH1D(Form("hCorr_%s",channelName.Data()),
			 Form("%s: 1D Corr %s",dataTitle.c_str(),channelName.Data()),
			 40, 0,-1);//on the fly limits on the x axis
  for(int ipt=0;ipt<npt;ipt++){
    h->GetXaxis()->SetBinLabel(ipt+2,Form("%d.%d",(int)run[ipt],(int)mini[ipt]));
    hCorr -> Fill(*(dummy+ipt));
  }

  TPaveText *pt = new TPaveText(0.6,0.8,0.89,0.89,"blNDC");  
  pt->SetBorderSize(0);
  pt->SetFillColor(0);
  pt->SetFillStyle(3155);
  pt->SetTextColor(4);

  if(channelName.Contains("-")){
    min -= 0.05*abs(min);
    max += 0.10*abs(max);
    h->GetYaxis()->SetRangeUser(min, max);
    h->SetMarkerStyle(20);
    c1->Clear();
    TPad *p1 = new TPad(Form("p1_%s",h->GetTitle()),"",0,0,0.7,1);
    p1->SetRightMargin(0.05);
    p1->Draw();
    TPad *p2 = new TPad(Form("p2_%s",h->GetTitle()),"",0.7,0,1,1);
    p2->SetLeftMargin(0.02);
    p2->Draw();

    p1->cd();
    h->Fit(fconst->GetName(),"Q","",1,npt+1);
    p1->SetGridx(1);
    p1->SetGridy(1);
    h->GetXaxis()->SetLabelSize(0.04);
    h->GetXaxis()->LabelsOption("v");
    h->GetXaxis()->SetTitle("");
    h->SetMarkerSize(2);
    h->DrawCopy("P");
    h->SetStats(0);
    h->DrawCopy("P");
    pt->AddText(Form("#chi^2 /ndf : %5.2f/%d",fconst->GetChisquare(),fconst->GetNDF()));
    pt->AddText(Form("p0 : %5.2f #pm %5.2f",fconst->GetParameter(0),fconst->GetParError(0)));
    pt->Draw();

    p2->cd();
    hCorr->DrawCopy();
    hCorr->SetStats(2200);
    hCorr->DrawCopy();

    cout<<channelName<<":\t"<<fconst->GetParameter(0)<<"\t+\\-\t"<<hCorr->GetRMS()<<endl;
    ofile<<fconst->GetParameter(0)<<","<<hCorr->GetRMS()<<",";
    
  }else if(channelName.Contains("mean")){
    TString channel_err = channelName;
    channel_err.ReplaceAll("mean","err");

    t->Draw(channel_err+":Entry$",userCut,"goff");
    dummy = t->GetV1();
    for(int ipt=0;ipt<npt;ipt++){
      h->SetBinError(ipt+2,*(dummy+ipt));
      double val1 = h->GetBinContent(ipt+2) - *(dummy+ipt);
      double val2 = h->GetBinContent(ipt+2) + *(dummy+ipt);
      if(min > val1)
	min = val1;
      if(max < val2 )
	max = val2;
    }

    min -= 0.05*abs(min);
    max += 0.10*abs(max);
    h->GetYaxis()->SetRangeUser(min, max);
    pullUtil(h,c1); //This does all the plotting
  }else{
    min -= 0.05*abs(min);
    max += 0.10*abs(max);
    h->GetYaxis()->SetRangeUser(min, max);
    h->SetMarkerStyle(20);
    h->Fit(fconst->GetName(),"Q","",1,npt+1);
    cout<<channelName<<":\t"<<fconst->GetParameter(0)<<"\t+\\-\t"<<fconst->GetParError(0)<<endl;
    ofile<<fconst->GetParameter(0)<<","<<fconst->GetParError(0)<<",";
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    h->GetXaxis()->SetLabelSize(0.04);
    h->GetXaxis()->LabelsOption("v");
    h->GetXaxis()->SetTitle("");
    h->SetMarkerSize(2);
    h->DrawCopy("P");
    h->SetStats(0);
    h->DrawCopy("P");
    pt->AddText(Form("#chi^2 /ndf : %5.2f/%d",fconst->GetChisquare(),fconst->GetNDF()));
    pt->AddText(Form("p0 : %5.2f #pm %5.2f",fconst->GetParameter(0),fconst->GetParError(0)));
    pt->Draw();
  }

  c1->Print(Form("y_postPanPlots_%s.pdf",dataTitle.c_str()),"pdf");
}


void Init(TTree *t){
  fg=new TF1("fg","gaus");
  fconst = new TF1("fconst","pol0");
  TString dv_list[]=
    {"asym_usl",
     "asym_usr",
     "asym_dsl",
     "asym_dsr",
     "asym_us_avg",
     "asym_us_dd",
     "asym_ds_avg",
     "asym_ds_dd",
     "asym_left_avg",
     "asym_left_dd",
     "asym_right_avg",
     "asym_right_dd",
     "asym_bcm_an_ds3"};

  TString iv_list[]=
    {"diff_bpm4aX",
     "diff_bpm4aY",
     "diff_bpm4eX",
     "diff_bpm4eY",
     "diff_bpm12X"};
  Int_t nIV = sizeof(iv_list)/sizeof(*iv_list);
  Int_t nDV = sizeof(dv_list)/sizeof(*dv_list);

  TString dv_name, iv_name;
  for(Int_t idv=0;idv<nDV;idv++){
    for(Int_t iiv=0;iiv<nIV;iiv++){
      dv_name = dv_list[idv];
      iv_name = iv_list[iiv];
      dv_name.ReplaceAll("asym_","");
      iv_name.ReplaceAll("diff_","");
      TString coeff_name = Form("slope[%d][%d]",idv,iiv);
      t->SetAlias(Form("%s_%s",dv_name.Data(),iv_name.Data()),
		  coeff_name);
    }
  }
}

void pullUtil(TH1 *h,TCanvas *c1){
  c1->Clear();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);

  double ySep = 0.3;
  double xSep = 0.65;

  TPad *p1 = new TPad(Form("p1_%s",h->GetTitle()),"",0,ySep,xSep,1);
  p1->Draw();
  TPad *p2 = new TPad(Form("p2_%s",h->GetTitle()),"",0,0,xSep,ySep);
  p2->Draw();
  TPad *p3 = new TPad(Form("p3_%s",h->GetTitle()),"",xSep,0,1,1);
  p3->Draw();

  p1->cd();
  p1->UseCurrentStyle();
  p1->SetBottomMargin(0);
  int npt = h->GetXaxis()->GetNbins();
  h->Fit(fconst->GetName(),"Q","",1,npt-1);
  cout<<h->GetTitle()<<" :\t"<<fconst->GetParameter(0)<<"\t+\\-\t"<<fconst->GetParError(0)<<endl;
  ofile<<fconst->GetParameter(0)<<","<<fconst->GetParError(0)<<",";
  h->DrawCopy();
  p1->SetGridx(1);
  p1->SetGridy(1);
  
  const int nBin = h->GetXaxis()->GetNbins();
  double xlow = h->GetBinCenter(1) - h->GetBinWidth(1)/2;
  double xhigh = h->GetBinCenter(nBin) + h->GetBinWidth(nBin)/2;
  TH1D *hPull = new TH1D(Form("hPull_%s",h->GetName()),"",nBin,xlow,xhigh);
  TH1D *hPull1D = new TH1D(Form("hPull1D_%s",h->GetName()),"1D pull distribution",40,-8,8);
  for(int i=1;i<=nBin;i++){
    if(h->GetBinError(i) == 0 ) continue;
    double val = (h->GetBinContent(i) - fconst->Eval(h->GetBinCenter(i))) / h->GetBinError(i);
    hPull -> SetBinContent(i, val);
    hPull -> GetXaxis() -> SetBinLabel(i, h->GetXaxis()->GetBinLabel(i));
    hPull1D -> Fill(val);
  }
  p2->cd();
  p2->SetTopMargin(0);
  p2->SetBottomMargin(0.3);
  hPull->GetYaxis()->SetLabelSize(0.1);
  hPull->GetXaxis()->SetLabelSize(0.1);
  hPull->SetFillColor(kGreen-3);
  hPull->SetStats(0);
  hPull->GetXaxis()->LabelsOption("v");
  hPull->DrawCopy("b");
  p2->SetGridx(1);
  p2->SetGridy(1);

  p3->cd();
  gStyle->SetOptFit(1);
  gStyle->SetOptStat("MRou");
  p3->UseCurrentStyle();
  fg->SetParameters(hPull1D->GetMaximum(),0,1);
  hPull1D->Fit(fg->GetName(),"Q");
  hPull1D->DrawCopy();
  cout<<h->GetTitle()<<" (pull)\t"<<hPull1D->GetMean()<<"\t+\\-\t"<<hPull1D->GetRMS()<<endl;
  ofile<<hPull1D->GetMean()<<","<<hPull1D->GetRMS()<<",";

}

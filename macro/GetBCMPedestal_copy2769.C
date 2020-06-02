// GetBCMPedestal.C
// 	jaPAN Macro Script - BCM Pedestal Scan
// 	based on UNSER readout
//	author: Tao Ye
//      modified: July 14, 2019 by Caryn

void LoadStyle();
void GetBCMPedestal_copy2769(int run_num=2769,
			     Double_t lowlimit=5,Double_t uplimit=80,
		    TCut user_cut ="1");

void GetBCMPedestal_copy2769(int run_num=2769,
			     Double_t lowlimit,Double_t uplimit,
		    TCut user_cut){
  LoadStyle();
  char* qwrootfile_path = getenv("QW_ROOTFILES");
  TString rf_name =Form("%s/prexPrompt_pass2_%d.000.root",
			qwrootfile_path,run_num);
  TFile *rootfile = TFile::Open(rf_name);  
  if(rootfile ==NULL){
    cout << rf_name 
	 << " doesn't exist !!" << endl;
    return;
  }
  TTree *tree= (TTree*)rootfile->Get("evt");
  TString device_name[] =
    {"bcm_an_us", "bcm_an_ds",
     "bcm_an_ds3","bcm_an_ds10",
     "bcm_dg_us","bcm_dg_ds"};

  const int nbcm= sizeof(device_name)/sizeof(*device_name);

  TCanvas *c_bcm = new TCanvas("c_bcm","c_bcm",1000,500);
  c_bcm->Divide(2,1);
  
  TF1 *f_zero = new TF1("f_zero","0",0,100);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);
  
  TString branch_name;
  TString num_samples_name;
  // run 1404
  // TString beam_evtcut[] ={ 
  //   "Entry$>1e3 && Entry$<27e3",
  //   "Entry$>30e3 && Entry$<42e3",
  //   "Entry$>70e3 && Entry$<88e3"};
  // TString pedestal_evtcut[] ={
  //   "Entry$>45e3 && Entry$<50e3",
  //   "Entry$>45e3 && Entry$<50e3",
  //   "Entry$>45e3 && Entry$<50e3" };

  // run 1329
  // TString beam_evtcut[] ={ 
  //   "Entry$>10 && Entry$<3e3",
  //   "Entry$>4e3 && Entry$<6e3",
  //   "Entry$>7.5e3 && Entry$<10e3"};
  // TString pedestal_evtcut[] ={
  //   "Entry$>11e3 && Entry$<13e3",
  //   "Entry$>11e3 && Entry$<13e3",
  //   "Entry$>11e3 && Entry$<13e3"};

  //  run 1280
  // TString beam_evtcut[] ={ 
  //   "Entry$>10 && Entry$<2.5e3",
  //   "Entry$>8.5e3 && Entry$<10e3",
  //   "Entry$>15e3 && Entry$<17e3"};
  // TString pedestal_evtcut[] ={
  //   "Entry$>4e3 && Entry$<6e3",
  //   "Entry$>11e3 && Entry$<13e3",
  //   "Entry$>18e3 && Entry$<20e3"};

  //  run 2769
  TCut beam_evtcut[] ={ 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7",
    "cleandata && scandata1>0 && scandata2==8"};

 TCut unser_evtcut[] ={ 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7",
    "cleandata && scandata1>0 && scandata2==8"};

  //  TCut unser_evtcut[] ={
  //   "cleandata && scandata1>0 && scandata2==1&& (CodaEventNumber>9300)",
  //  "cleandata && scandata1>0 && scandata2==2&& (CodaEventNumber<34200 ||CodaEventNumber>37300)",
  //   "cleandata && scandata1>0 && scandata2==3&& (CodaEventNumber<63500 ||CodaEventNumber>66500)",
  //   "cleandata && scandata1>0 && scandata2==4&& (CodaEventNumber<88700 ||CodaEventNumber>92200)",
  //  "cleandata && scandata1>0 && scandata2==5&& (CodaEventNumber<110500 ||CodaEventNumber>113500)",
  //   "cleandata && scandata1>0 && scandata2==6&& (CodaEventNumber<133500 ||CodaEventNumber>13700)",
  //   "cleandata && scandata1>0 && scandata2==7&& (CodaEventNumber<152e3 ||CodaEventNumber>155e3)",
  //   "cleandata && scandata1>0 && scandata2==8&&CodaEventNumber<172e3"
  // };

 
  
  //  TCut pedestal_evtcut[] ={
  //    "cleandata && scandata1==0 && (scandata2==1)&&CodaEventNumber<16200",
  //  "cleandata && scandata1==0 && (scandata2==2 || scandata2==1)&& (CodaEventNumber>23500 ||CodaEventNumber<48200)",
  //  "cleandata && scandata1==0 && (scandata2==3 || scandata2==2)&& (CodaEventNumber>51500 ||CodaEventNumber<72500)",
  //  "cleandata && scandata1==0 && (scandata2==4 || scandata2==5)&&(CodaEventNumber>81300 || CodaEventNumber>98300)",
  //  "cleandata && scandata1==0 && (scandata2==5 || scandata2==6)&&(CodaEventNumber>100300 || CodaEventNumber>122500)",
  //  "cleandata && scandata1==0 && (scandata2==6 || scandata2==7)&&(CodaEventNumber>127200 || CodaEventNumber>142000)",
  //   "cleandata && scandata1==0 && (scandata2==7 || scandata2==8)&&(CodaEventNumber>144.8e3 || CodaEventNumber>163e3)",
  //   "cleandata && scandata1==0 && (scandata2==8)&&CodaEventNumber>166e3"};

  TCut pedestal_evtcut[] ={
    "cleandata && scandata1==0 && (scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==2 || scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==3 || scandata2==2)",
    "cleandata && scandata1==0 && (scandata2==4 || scandata2==5)",
    "cleandata && scandata1==0 && (scandata2==5 || scandata2==6)",
    "cleandata && scandata1==0 && (scandata2==6 || scandata2==7)",
    "cleandata && scandata1==0 && (scandata2==7 || scandata2==8)",
    "cleandata && scandata1==0 && (scandata2==8)"};



  const int ndata = sizeof(beam_evtcut)/sizeof(*beam_evtcut);
  double bcm_mean[ndata]; 
  double bcm_error[ndata];
  double unser_mean[ndata]; 
  double unser_error[ndata];
  double bcm_res[ndata]; // residual

  TString gfit_title;
  TString gres_title;
  TH1D *h_stat;
  double ped[nbcm];
  double gain[nbcm];
  double slope[nbcm];

  TGraphErrors *g_res;
  TGraphErrors *g_fit;  
  TGraphErrors *g_res_ref;
  TGraphErrors *g_fit_ref;
  TMultiGraph *mg_res;
  TMultiGraph *mg_fit;

  TF1 *f_fit = new TF1("f_fit","[1]*x+[0]",-10,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Slope");
  double init_par[2] = { 1e3,1e-3};
  double fit_up, fit_low;
  fit_up = uplimit; // hardcoded upper limit 200 uA
  fit_low = lowlimit;

  TString my_cut;
  for(int i=0;i<ndata;i++){
    my_cut = unser_evtcut[i]+user_cut;
    tree->Draw("unser",
	       my_cut,"goff");
    h_stat =(TH1D*)gDirectory->FindObject("htemp");
    unser_mean[i] = h_stat->GetMean();
    unser_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
    my_cut = pedestal_evtcut[i];
    tree->Draw("unser",
	       my_cut,"goff");
    h_stat =(TH1D*)gDirectory->FindObject("htemp");
    unser_mean[i]  = unser_mean[i] - h_stat->GetMean(); // pedestal subtraction
    cout << unser_mean[i]
	 << "+/-"
	 << unser_error[i]
	 << endl;
  }

  for(int ibcm=0;ibcm<nbcm;ibcm++){
    branch_name = Form("%s.hw_sum_raw/%s.num_samples",
		       device_name[ibcm].Data(),device_name[ibcm].Data());

      for(int i=0;i<ndata;i++){
	my_cut = beam_evtcut[i];
	tree->Draw(branch_name,
		  my_cut,"goff");
	h_stat =(TH1D*)gDirectory->FindObject("htemp");
	bcm_mean[i] = h_stat->GetMean();
	bcm_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      c_bcm->cd(1);

      g_fit = new TGraphErrors(ndata,unser_mean,bcm_mean,unser_error,bcm_error);
      g_fit->SetMarkerStyle(20);
      g_fit->Draw("AP");
      gfit_title = Form("%s; UNSER(uA); Raw ADC per Sample",branch_name.Data());
      g_fit->SetTitle(gfit_title.Data());
      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      if(branch_name.Contains("ds10"))
	 g_fit->Fit("f_fit","QR","",0,30);
      else
	g_fit->Fit("f_fit","QR","",fit_low,fit_up);

      ped[ibcm] = f_fit->GetParameter(0);
      slope[ibcm] = f_fit->GetParameter(1);
      gain[ibcm]=1/slope[ibcm];

      for(int i=0;i<ndata;i++){
	bcm_res[i] = (bcm_mean[i]-ped[ibcm])*gain[ibcm] - unser_mean[i];
	bcm_error[i] = bcm_error[i] *gain[ibcm];
      }
      
      c_bcm->cd(2);
      g_res = new TGraphErrors(ndata,unser_mean,bcm_res,unser_error,bcm_error);
      g_res->SetMarkerStyle(20);
      g_res->Draw("AP");
      gres_title = Form("%s;UNSER(uA); Residual(uA) ",branch_name.Data());
      g_res->SetTitle(gres_title.Data());
      f_zero->Draw("same");

    c_bcm->SaveAs(Form("run%d_%s_ped_fit.png",run_num,device_name[ibcm].Data()));
  } // End of BCMs loop
  gSystem->Exec(Form("convert $(ls -rt *bcm*fit.png) run%d_bcm_pedestal_fit.pdf",run_num));
  gSystem->Exec("rm *bcm*.png");

  for(int ibcm=0;ibcm<nbcm;ibcm++){
      printf("%s, %.2f, %f \n",
	     device_name[ibcm].Data(),
	     ped[ibcm],
	     gain[ibcm]);
  }
  rootfile->Close();
}

void LoadStyle(){
  gROOT->SetStyle("Plain");
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.3);
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(1011);
  gStyle->SetStatX(0.7);
  gStyle->SetStatY(0.9);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadColor(39); 
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadRightMargin(0.01);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetLabelSize(0.035,"x");
  gStyle->SetLabelSize(0.035,"y");
  gStyle->SetTitleSize(0.06,"hxyz");
  gROOT->ForceStyle();  
}

void LoadStyle();
void GetBCMPedestal_run3395_Caryn(int run_num=3395,
		    Double_t lowlimit=5,
		    Double_t uplimit=200,
		    TCut user_cut=""){
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
     "bcm_dg_us","bcm_dg_ds",
     "cav4cQ"};

  const int nbcm= sizeof(device_name)/sizeof(*device_name);

  TCanvas *c_bcm = new TCanvas("c_bcm","c_bcm",1000,500);
  c_bcm->Divide(2,1);
  
  TF1 *f_zero = new TF1("f_zero","0",0,100);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);
  
  TString branch_name;
  TString num_samples_name;

  //  run 3395
  TCut beam_evtcut[] ={ 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==7",
    "cleandata && scandata1>0 && scandata2==8",
    "cleandata && scandata1>0 && scandata2==9"
};


// TCut pedestal_evtcut[] ={
//     "cleandata && scandata1==0 && (scandata2==1)",
//     "cleandata && scandata1==0 && (scandata2==2 || scandata2==1)",
//     "cleandata && scandata1==0 && (scandata2==3 || scandata2==2)",
//     "cleandata && scandata1==0 && (scandata2==4 || scandata2==3)",
//     "cleandata && scandata1==0 && (scandata2==5 || scandata2==4)",
//     "cleandata && scandata1==0 && (scandata2==6 || scandata2==5)",
//     "cleandata && scandata1==0 && (scandata2==7 || scandata2==6)",
//     "cleandata && scandata1==0 && (scandata2==8 || scandata2==7)",
//     "cleandata && scandata1==0 && (scandata2==9 || scandata2==8)",
//     "cleandata && scandata1==0 && (scandata2==10 || scandata2==9)",
//     "cleandata && scandata1==0 && (scandata2==11 || scandata2==10)"};

  TCut pedestal_evtcut[] ={
    " scandata2==1 && (CodaEventNumber>86000&&CodaEventNumber<87100)",
    "scandata2==4 &&cleandata &&CodaEventNumber>121.5e3&&CodaEventNumber<123.3e3",
    "scandata2==4 && CodaEventNumber>131.6e3&&CodaEventNumber<133.2e3",
    "scandata2==7 &&CodaEventNumber>162.2e3&&CodaEventNumber<163.6e3",
    "scandata2==8 &&CodaEventNumber>177.8e3&&CodaEventNumber<179.4e3",
    "scandata2==9 &&CodaEventNumber>197.5e3"};

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
  fit_up = uplimit; 
  fit_low = lowlimit;

  TString my_cut;
  for(int i=0;i<ndata;i++){
    my_cut = beam_evtcut[i]+user_cut;
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
	 g_fit->Fit("f_fit","QR","",0,35);
      elseif(branch_name.Contains("ds3"))
	 g_fit->Fit("f_fit","QR","",50,75);
      else
	g_fit->Fit("f_fit","QR","",fit_low,fit_up);

      ped[ibcm] = f_fit->GetParameter(0);
      slope[ibcm]=f_fit->GetParameter(1);
      gain[ibcm] = 1/slope[ibcm];
      
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
  gSystem->Exec(Form("convert $(ls -rt *bcm*fit.png) run%d_bcm_pedestal_fit_Caryn.pdf",run_num));
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

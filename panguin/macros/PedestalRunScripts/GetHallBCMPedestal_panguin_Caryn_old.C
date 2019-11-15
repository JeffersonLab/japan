// jaPAN Macro Script - BCM Pedestal Scan for Counting House
// based on UNSER readout
void GetHallBCMPedestal_panguin_Caryn(TString user_cut ="1",TString scan_data = "unser", double MIN = -1000, double MAX = 1000,Int_t myii=0){
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
  
  //T
 
  TTree *tree= (TTree*)gROOT->FindObject("evt");

  //TString device_name[] = devnam;

  //  TString device_name[] ={Form("%s",devnam.Data())};
   TString device_name[] =  {"bcm_an_us", "bcm_an_ds",
			     "bcm_an_ds3","bcm_an_ds10",
			     "bcm_dg_us","bcm_dg_ds"};
  //   "hallc_bcm_1","hallc_bcm_2",
  //   "cav4bx","cav4by","cav4bq",
  //   "cav4cx","cav4cy","cav4cq",
  //   "cav4dx","cav4dy","cav4dq"};

  // TString device_name[] =
  //   {"bcm_an_us", "bcm_an_ds",
  //    "bcm_an_ds3",
  //    "bcm_dg_us","bcm_dg_ds"};

  const int nbcm= sizeof(device_name)/sizeof(*device_name);

  TPad *c_bcm = new TPad("c_bcm","c_bcm",0,0,1,1);
  c_bcm->Divide(1,2);
  c_bcm->Draw();
  TVirtualPad* c_fit = c_bcm->cd(1);
  TVirtualPad* c_res = c_bcm->cd(2);
  c_fit->Divide(nbcm,1);
  c_res->Divide(nbcm,1);



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
    //    "Entry$>10 && Entry$<2.5e3",
    //   "Entry$>8.5e3 && Entry$<10e3",
    //   "Entry$>15e3 && Entry$<17e3"};
  // TString pedestal_evtcut[] ={
    //    "Entry$>4e3 && Entry$<6e3",
    //    "Entry$>11e3 && Entry$<13e3",
    //    "Entry$>18e3 && Entry$<20e3"};

  TString beam_evtcut[] ={ 
    "cleandata==1&&scandata2==1",
    "cleandata==1&&scandata2==2",
    "cleandata==1&&scandata2==3",
    "cleandata==1&&scandata2==4",
    "cleandata==1&&scandata2==5",
    "cleandata==1&&scandata2==6",
    "cleandata==1&&scandata2==7",
    "cleandata==1&&scandata2==8",
    "cleandata==1&&scandata2==9",
    "cleandata==1&&scandata2==10"   };
  TString pedestal_unser_evtcut[] ={
    "cleandata==1&&scandata2==100",
    "cleandata==1&&scandata2==200",
    "cleandata==1&&scandata2==300",
    "cleandata==1&&scandata2==400",
    "cleandata==1&&scandata2==500",
    "cleandata==1&&scandata2==600",
    "cleandata==1&&scandata2==700",
    "cleandata==1&&scandata2==800",
    "cleandata==1&&scandata2==900",
    "cleandata==1&&scandata2==1000"};


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

	char outfilename[255];

  TGraphErrors *g_bcm_res;
  TGraphErrors *g_bcm_fit;  
  TGraphErrors *g_bcm_res_ref;
  TGraphErrors *g_bcm_fit_ref;
  TMultiGraph *mg_bcm_res;
  TMultiGraph *mg_bcm_fit;

  TF1 *f_fit = new TF1("f_fit","1.0/[1]*x+[0]",-10,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Gain");
  double init_par[2] = { 1e3,1e-3};

  TString my_cut;
  for(int i=0;i<ndata;i++){
    my_cut = beam_evtcut[i];
    tree->Draw(Form("%s>>h_unser_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_unser_%d_%d",i,myii));
    unser_mean[i] = h_stat->GetMean();
    unser_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
    //cout << unser_mean[i] << endl;
    my_cut = pedestal_unser_evtcut[i];
    tree->Draw(Form("%s>>h_ped_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_ped_%d_%d",i,myii));
    unser_mean[i]  = unser_mean[i] - h_stat->GetMean(); // pedestal subtraction
    //cout << unser_mean[i] << endl;
  }

  for(int ibcm=0;ibcm<nbcm;ibcm++){
    branch_name = Form("%s.hw_sum_raw/%s.num_samples",
		       device_name[ibcm].Data(),device_name[ibcm].Data());

      for(int i=0;i<ndata;i++){
	my_cut = beam_evtcut[i];
	tree->Draw(Form("%s>>h_bcm%d_%d_%d",branch_name.Data(),ibcm,i,myii),
		  my_cut.Data(),"goff");
	h_stat =(TH1D*)gDirectory->FindObject(Form("h_bcm%d_%d_%d",ibcm,i,myii));
	bcm_mean[i] = h_stat->GetMean();
	bcm_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      c_fit->cd(ibcm+1);

      g_bcm_fit = new TGraphErrors(ndata,unser_mean,bcm_mean,unser_error,bcm_error);
      g_bcm_fit->SetMarkerStyle(20);
      g_bcm_fit->Draw("AP");
      gfit_title = Form("%s; %s; Raw ADC per Sample",device_name[ibcm].Data(),scan_data.Data());
      g_bcm_fit->SetTitle(gfit_title.Data());
      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      g_bcm_fit->Fit("f_fit","QR","",MIN,MAX);

      ped[ibcm] = f_fit->GetParameter(0);
      gain[ibcm] = f_fit->GetParameter(1);
      
      for(int i=0;i<ndata;i++){
	bcm_res[i] = (bcm_mean[i]-ped[ibcm])*gain[ibcm] - unser_mean[i];
	bcm_error[i] = bcm_error[i] *gain[ibcm];
      }
      
      c_res->cd(ibcm+1);
      g_bcm_res = new TGraphErrors(ndata,unser_mean,bcm_res,unser_error,bcm_error);
      g_bcm_res->SetMarkerStyle(20);
      g_bcm_res->Draw("AP");
      gres_title = Form("%s;%s; Residual(uA) ",device_name[ibcm].Data(),scan_data.Data());
      g_bcm_res->SetTitle(gres_title.Data());
      f_zero->Draw("same");

  } // End of BCMs loop



  delete  tree,  device_name, nbcm,f_zero, branch_name, num_samples_name, beam_evtcut,pedestal_unser_evtcut, ndata, bcm_mean, bcm_error,  unser_mean, unser_error, bcm_res, gfit_title, gres_title, ped, gain, f_fit, init_par,my_cut;

}

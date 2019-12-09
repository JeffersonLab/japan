// GetDetectorPedestal.C
// 	jaPAN ROTO Macro - Detector Pedestal Script
// 	based on BCM** readout.
//
// **Note: This also means a BCM needs to be calibrated with pedestals,
//	   therefore this function should be called at the second pass after
// 	   beamline calibration.
//
//	Revised: Tao Ye  
//	Date: July 2019

void LoadStyle();
void GetDetectorPedestal(int run_num,
			 TString bcm_name = "bcm_an_ds3",
			 Double_t lowlimit=30,
			 TCut user_cut ="1");


void GetDetectorPedestal(int run_num,
			 TString bcm_name,
			 Double_t lowlimit,
			 TCut user_cut){
  
  LoadStyle();
  TFile *rootfile = TFile::Open(Form("$QW_ROOTFILES/prexPrompt_pass2_%d.000.root",run_num));
  TTree *tree= (TTree*)rootfile->Get("evt");

  TString outputDir = "/adaqfs/home/apar/PREX/japan/plots/pedestalOutputs";

  //---------------------------

  TCut beam_evtcut[] ={ 
    "cleandata && ErrorFlag==0 && scandata1>0 && scandata2==1",
    "cleandata && ErrorFlag==0 && scandata1>0 && scandata2==2"};

  const int ndata = sizeof(beam_evtcut)/sizeof(*beam_evtcut);

  TString device_name[] =
    {"sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8",
     "usl","dsl","atl1","atl2",
     "usr","dsr","atr1","atr2"};

  const int nDET= sizeof(device_name)/sizeof(*device_name);

  TCanvas *c_det = new TCanvas("c_det","c_det",1000,500);
  c_det->Divide(2,1);

  TF1 *f_zero = new TF1("f_zero","0",0,10000);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);

  double det_mean[ndata]; 
  double det_error[ndata];
  double bcm_mean[ndata]; 
  double bcm_error[ndata];
  double det_res[ndata]; // residual

  TString gfit_title;
  TString gres_title;
  TH1D *h_stat;
  double ped[nDET];
  double slope[nDET];
  double gain[nDET];

  char outfilename[255];
  sprintf(outfilename,"%s/run%d_detector_pedestal_fit_taoye.txt",
	  outputDir.Data(),run_num);
  printf("Writing output to %s\n",outfilename);

  //FILE *outfile = fopen(outfilename, "w"); 
  ofstream outfile;
  outfile.open(outfilename);

  TGraphErrors *g_res;
  TGraphErrors *g_fit;  
  TGraphErrors *g_res_ref;
  TGraphErrors *g_fit_ref;
  TMultiGraph *mg_res;
  TMultiGraph *mg_fit;

  TString branch_name;
  TString num_samples_name;

  TF1 *f_fit = new TF1("f_fit","[1]*x+[0]",-10,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Slope");
  double init_par[2] = { 1e3,1e-3};
  double fit_up, fit_low;
  fit_up = 100; // 100 uA
  fit_low = lowlimit;
  for(int i=0;i<ndata;i++){
    tree->Draw(bcm_name,
	       beam_evtcut[i]+user_cut,"goff");
    h_stat =(TH1D*)gDirectory->FindObject("htemp");

    bcm_mean[i] = h_stat->GetMean();
    bcm_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());

    cout << bcm_mean[i] 
	 << "+/-"
	 << bcm_error[i] 
	 << endl;
  }

  for(int idet=0;idet<nDET;idet++){
    branch_name = Form("%s.hw_sum_raw/%s.num_samples",
		       device_name[idet].Data(),device_name[idet].Data());
      for(int i=0;i<ndata;i++){

	tree->Draw(branch_name,
		   beam_evtcut[i]+user_cut,
		   "goff");
	h_stat =(TH1D*)gDirectory->FindObject("htemp");
	det_mean[i] = h_stat->GetMean();
	det_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      c_det->cd(1);

      g_fit = new TGraphErrors(ndata,bcm_mean,det_mean,bcm_error,det_error);
      g_fit->SetMarkerStyle(20);
      g_fit->Draw("AP");
      gfit_title = Form("%s; %s; DET(ADC)",branch_name.Data(),bcm_name.Data());
      g_fit->SetTitle(gfit_title.Data());
      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      g_fit->Fit("f_fit","QR","",fit_low,fit_up);

      ped[idet] = f_fit->GetParameter(0);
      slope[idet] = f_fit->GetParameter(1);
      // gain[idet] = 7.629e-5; // 76.29 uV/ADC

      for(int i=0;i<ndata;i++){
	det_res[i] = det_mean[i] - f_fit->Eval(bcm_mean[i]);
      }
      
      c_det->cd(2);
      g_res = new TGraphErrors(ndata,bcm_mean,det_res,bcm_error,det_error);
      g_res->SetMarkerStyle(20);
      g_res->Draw("AP");
      gres_title = Form("%s;%s; Residual(ADC) ",branch_name.Data(),bcm_name.Data());
      g_res->SetTitle(gres_title.Data());
      f_zero->Draw("same");
      c_det->SaveAs(Form("%s/run%d_%s_ped_fit.png",outputDir.Data(),run_num,device_name[idet].Data()));
  } // End of device loop

  gSystem->Exec(Form("convert $(ls -rt %s/*fit.png) %s/run%d_detector_pedestal_fit_taoye.pdf",
		     outputDir.Data(),
		     outputDir.Data(),run_num));

  gSystem->Exec(Form("rm %s/run%d_*fit.png",outputDir.Data(),run_num));

  for(int idet=0;idet<nDET;idet++){
      printf("%s, %.2f, 76.29e-6 \n",
	     device_name[idet].Data(),
	     ped[idet]);
      outfile<<Form("%s, %.2f, 76.29e-6\n ",
		    device_name[idet].Data(),
		    ped[idet]);

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

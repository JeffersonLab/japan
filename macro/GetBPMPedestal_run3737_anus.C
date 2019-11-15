// GetBPMPedestal.C
//	date: July, 2019
void LoadStyle();

void GetBPMPedestal_run3737_anus(int run_num=3737, 
				 TString bcm_name="(bcm_an_us.hw_sum_raw/bcm_an_us.num_samples--669.87)*0.005919",
		    Double_t lowlimit=10,
		    Double_t upperlimit=75,
		    TCut user_cut="" ){
  LoadStyle();
  char* qwrootfile_path = getenv("QW_ROOTFILES");
  TString rf_name =Form("%s/prexALL_%d.000.root",
			qwrootfile_path,run_num);
  TFile *rootfile = TFile::Open(rf_name);
  if(rootfile ==NULL){
    cout << rf_name 
	 << " doesn't exist !!" << endl;
    return;
  }
    
  TString outputdir = "~/PREX/japan/plots/pedestalOutputs";

  TTree *tree= (TTree*)rootfile->Get("evt");

  //TString device_name[] ={"1"};
   TString device_name[] ={"4a","4e",
  			  "4ac","4ec",
			   "11","12","16","1"};
  TString ch_name[4]={"XP","XM","YP","YM"};
  const int nBPM= sizeof(device_name)/sizeof(*device_name);
  const int nbpm= sizeof(device_name)/sizeof(*device_name);

  TCanvas *c1 = new TCanvas("c1","c1",1500,600);
  c1->Divide(1,2);
  TVirtualPad* c_fit = c1->cd(1);
  TVirtualPad* c_res = c1->cd(2);
  c_fit->Divide(5,1);
  c_res->Divide(5,1);
  
  TF1 *f_zero = new TF1("f_zero","0",0,100);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);
  
  TString branch_name;
  TString num_samples_name;

  //  run 3396
  TCut beam_evtcut[] ={ 
    //    "cleandata && scandata1>0 && scandata2==1",
    //   "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1==600",
    "cleandata && scandata1==652",
    "cleandata && scandata1==700"};

  const int ndata = sizeof(beam_evtcut)/sizeof(*beam_evtcut);

  double adc_mean[5][ndata]; // 4 channels  + 1channel for wire sum
  double adc_error[5][ndata];
  double adc_res[5][ndata]; // residual

  double bcm_mean[ndata]; 
  double bcm_error[ndata];

  TString gfit_title[5];
  TString gres_title[5];
  TH1D *h_stat;
  double ped[nBPM][5];
  double slope[nBPM][5];

  TGraphErrors *g_res[5];
  TGraphErrors *g_fit[5];  
  TGraphErrors *g_res_ref[5];
  TGraphErrors *g_fit_ref[5];
  TMultiGraph *mg_res[5];
  TMultiGraph *mg_fit[5];

  TF1 *f_fit = new TF1("f_fit","[1]*x+[0]",-10,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Slope");
  double init_par[2] = { 1e3,1e4};

  double fit_up, fit_low;
  fit_up = upperlimit;
  fit_low = lowlimit;

  TCut my_cut;
  for(int i=0;i<ndata;i++){
    my_cut = beam_evtcut[i] + user_cut;
    tree->Draw(bcm_name,
	       my_cut,"goff");
    h_stat =(TH1D*)gDirectory->FindObject("htemp");
    bcm_mean[i] = h_stat->GetMean();
    bcm_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
    cout << "beam current(uA) : " 
	 << bcm_mean[i]
	 << "+/-" 
	 << bcm_error[i]
	 << endl;
    // my_cut = pedestal_evtcut[i];
    // tree->Draw(Form("unser>>h_ped_%d",i),
    // 	       my_cut.Data(),"goff");
    // h_stat =(TH1D*)gDirectory->FindObject(Form("h_ped_%d",i));
    // unser_mean[i]  = unser_mean[i] - h_stat->GetMean(); // pedestal subtraction
    // unser_mean[i]  = unser_mean[i]; // pedestal subtraction
    // cout << unser_mean[i] << endl;
  }

  for(int ibpm=0;ibpm<nBPM;ibpm++){
    for(int ich=0;ich<5;ich++){
      if(ich==4) { // WireSum
	// TString wiresum_text = "0.0+";
	// for(int ich=0;ich<4;ich++){
	//    wiresum_text += Form("+bpm%s%s.hw_sum_raw/bpm%s%s.num_samples",
	// 			device_name[ibpm].Data(),ch_name[ich].Data(),
	// 			device_name[ibpm].Data(),ch_name[ich].Data());
	// }
	// tree->SetAlias("WireSum", wiresum_text);
	// branch_name = "WireSum";
	branch_name = Form("bpm%sWS.hw_sum", device_name[ibpm].Data());
      }
      else{
	branch_name = Form("bpm%s%s.hw_sum_raw/bpm%s%s.num_samples",
			   device_name[ibpm].Data(),ch_name[ich].Data(),
			   device_name[ibpm].Data(),ch_name[ich].Data());
      }

      for(int i=0;i<ndata;i++){
	my_cut = beam_evtcut[i]+user_cut;
	tree->Draw(branch_name,
		   my_cut,"goff");
	h_stat =(TH1D*)gDirectory->FindObject("htemp");
	adc_mean[ich][i] = h_stat->GetMean();
	adc_error[ich][i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      c_fit->cd(ich+1);
      
      g_fit[ich] = new TGraphErrors(ndata,
				    bcm_mean,adc_mean[ich],
				    bcm_error,adc_error[ich]);
      g_fit[ich]->SetMarkerStyle(20);
      g_fit[ich]->Draw("AP");
      gfit_title[ich] = Form("%s;%s; Raw ADC",
			     branch_name.Data(),bcm_name.Data());
      g_fit[ich]->SetTitle(gfit_title[ich].Data());
       f_fit->SetParameters(init_par);
       if(ibpm==nBPM-1)
      	{
      	  double init_parminus[2] ={ -1e3,-1e4};
        f_fit->SetParameters(init_parminus);
      	}
      f_fit->SetLineColor(kRed);
      g_fit[ich]->Fit("f_fit","QR","",fit_low,fit_up);
      // f_fit->Draw("same");

      ped[ibpm][ich] = f_fit->GetParameter(0);
      slope[ibpm][ich] = f_fit->GetParameter(1);
      
      for(int i=0;i<ndata;i++)
	adc_res[ich][i] = adc_mean[ich][i] - f_fit->Eval(bcm_mean[i]);
      
      c_res->cd(ich+1);
      g_res[ich] = new TGraphErrors(ndata,
				    bcm_mean,adc_res[ich],
				    bcm_error,adc_error[ich]);
      g_res[ich]->SetMarkerStyle(20);
      g_res[ich]->Draw("AP");

      gres_title[ich] = Form("%s;%s; Residual ",
			     branch_name.Data(),bcm_name.Data());
      g_res[ich]->SetTitle(gres_title[ich].Data());
      f_zero->Draw("same");
    } // End ofW Wires loop
    c1->SaveAs(Form("%s/run%d_bpm%s_ped_fit.png",outputdir.Data(),run_num,device_name[ibpm].Data()));
  } // End of BPMs loop
  gSystem->Exec(Form("convert $(ls -rt %s/*bpm*fit.png) %s/run%d_bpm_pedestal_fit_anus.pdf",outputdir.Data(),outputdir.Data(),run_num));
  gSystem->Exec(Form("rm %s/run%d*bpm*.png",outputdir.Data(),run_num));

  for(int ibpm=0;ibpm<nBPM;ibpm++){
    for(int ich=0;ich<4;ich++){
      printf("bpm%s%s, %.2f, %.1f \n",
	      device_name[ibpm].Data(),
	      ch_name[ich].Data(),
	      ped[ibpm][ich],
	      1.0);
    }
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

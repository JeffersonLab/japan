// jaPAN Macro Script - SAM Pedestal Script
// based on UNSER readout

void GetAllPedestals_Caryn_run4652_dgds(int run_num = 4652,TString user_cut ="cleandata",TString scan_data ="(bcm_dg_ds.hw_sum_raw/bcm_dg_ds.num_samples--351.06)*0.002318" ,double MIN=5,double MAX=75,Int_t myii=0){


  // user_cut = Form("%s&&bmwobj==-1",user_cut.Data());

  gROOT->SetStyle("Plain");
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.3);
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(1011);
  gStyle->SetStatX(1.0);//0.7
  gStyle->SetStatY(0.4);//0.9
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
  gStyle->SetTitleSize(0.04,"hxyz");
  gROOT->ForceStyle();  

  //  TString rf_name =Form("$QW_ROOTFILES/prexPrompt_pass2_%d.000.root",run_num);
    TFile *rootfile = TFile::Open(Form("$QW_ROOTFILES/prexPrompt_pass1_%d.000.root",run_num));
  //  TFile *rootfile = TFile::Open(Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num));
  TTree *tree= (TTree*)rootfile->Get("evt");
  //  user_cut += "&&ErrorFlag==0";

	TString outputDir = "/adaqfs/home/apar/PREX/japan/plots/pedestalOutputs";
  //---------------------------

 tree->SetAlias("beam_current", scan_data);
  tree->Draw(Form("beam_current>>htemp%d",myii),user_cut.Data(),"goff");
  TH1D* htemp = (TH1D*)gDirectory->FindObject(Form("htemp%d",myii));
  double max = htemp->GetXaxis()->GetXmax();
  cout<<max<<endl;
  int nbinx = (int)(10*max); // to one-tenth precision.

  vector<double> vec_scandata;
  TH1D *hsd = new TH1D(Form("hsd%d",myii),"scan data",nbinx,-0.05,max-0.05); 
  tree->Draw(Form("beam_current>>hsd%d",myii),Form("%s&&scandata1!=0",user_cut.Data()),"goff");
  int bin_content; 
  double bin_center;
  double tempbincenter;
  for(int ibin=0;ibin<nbinx;ibin++){
    bin_content = hsd->GetBinContent(ibin+1); // Histogram bin number starts from 1
    // tempbincenter = hsd->GetBinCenter(ibin+1);
    // cout<<tempbincenter<<endl;
    if(bin_content>0){
      bin_center = hsd->GetBinCenter(ibin+1);
      vec_scandata.push_back(bin_center);
    }
  }
  const int ndata = vec_scandata.size();
  

  TString device_name[] =
    {"sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8",
    "usl","dsl","usr","dsr",
     "atl1","atl2","atr1","atr2",
     "bcm_an_us","bcm_an_ds","bcm_an_ds3","bcm_an_ds10","bcm_dg_us","bcm_dg_ds","cav4cQ"};


  //     "bcm_dg_us","cav4cQ"};
     //"cav4bQ","cav4bXI","cav4bYI",
  //     "cav4cQ","cav4cXI","cav4cYI"};//,
     // "cav4dQ","cav4dXI","cav4dYI"};
  //          "bcm_an_us","bcm_an_ds","bcm_an_ds3","bcm_dg_us","bcm_dg_ds"};

  const int nDET= sizeof(device_name)/sizeof(*device_name);

  TCanvas *c_det = new TCanvas("c_det","c_det",1000,500);
  c_det->Divide(2,1);

  TF1 *f_zero = new TF1("f_zero","0",0,10000);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);

  // const int ndata = sizeof(beam_evtcut)/sizeof(*beam_evtcut);
  double det_mean[ndata]; 
  double det_error[ndata];
  double unser_mean[ndata]; 
  double unser_error[ndata];
  double det_res[ndata]; // residual

  TString gfit_title;
  TString gres_title;
  TH1D *h_stat;
  double ped[nDET];
  double slope[nDET];
  double gain[nDET];

	char outfilename[255];
	sprintf(outfilename,"%s/run%d_detsamcavbcm_pedestal_fit_pedsearch1070.txt",
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
  double init_par[2] = { 1e3,1e-4};

  TString my_cut;
  int Np = 0;
  for(int i=0;i<ndata;i++){
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0&&%s",i+1,user_cut.Data());
    tree->Draw(Form("%s>>h_unser_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_unser_%d_%d",i,myii));
    unser_mean[i] = h_stat->GetMean();
    if(unser_mean[i]!=0)
      {
	Np=Np+1;
	cout<<Np<<endl;
      }
    //  cout<<unser_mean[i]<<endl;
    unser_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1==0&&%s",i+1,user_cut.Data());
    tree->Draw(Form("%s>>h_ped_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_ped_%d_%d",i,myii));
    if(scan_data=="unser"){
      unser_mean[i]  = unser_mean[i] - h_stat->GetMean(); // pedestal subtraction
    }
  }

  for(int idet=0;idet<nDET;idet++){
    branch_name = Form("%s.hw_sum_raw/%s.num_samples",
		       device_name[idet].Data(),device_name[idet].Data());

      for(int i=0;i<ndata;i++){
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0&&%s",i+1,user_cut.Data());

	tree->Draw(Form("%s>>h_det%d_%d_%d",branch_name.Data(),idet,i,myii),
		  my_cut.Data(),"goff");
	h_stat =(TH1D*)gDirectory->FindObject(Form("h_det%d_%d_%d",idet,i,myii));
	det_mean[i] = h_stat->GetMean();
	det_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      c_det->cd(1);

      // g_fit = new TGraphErrors(ndata,unser_mean,det_mean,unser_error,det_error); 
      g_fit = new TGraphErrors(Np,unser_mean,det_mean,unser_error,det_error);
      g_fit->SetMarkerStyle(20);
      g_fit->Draw("AP");
      gfit_title = Form("%s; %s; DET(ADC)",branch_name.Data(),scan_data.Data());
      g_fit->SetTitle(gfit_title.Data());
      //      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      g_fit->Fit("f_fit","Q","",MIN,MAX);
      //      cout<<(MAX+5)*100<<endl;
      // g_fit->GetXaxis()->SetRange(10,(MAX+5)*100);

      ped[idet] = f_fit->GetParameter(0);
      slope[idet] = f_fit->GetParameter(1);
      if(device_name[idet]=="cav4cQ" || device_name[idet]=="cav4cXI" || device_name[idet]=="cav4cYI"||device_name[idet]=="bcm_an_us"||device_name[idet]=="bcm_an_ds"||device_name[idet]=="bcm_dg_us"||device_name[idet]=="bcm_dg_ds"||device_name[idet]=="bcm_an_ds3"||device_name[idet]=="bcm_an_ds10"){
        gain[idet] = 1/slope[idet];      
      }
      else{
      gain[idet] = 76.293e-6;  
      }

      for(int i=0;i<ndata;i++){
	det_res[i] = det_mean[i] - f_fit->Eval(unser_mean[i]);
      }
      
      c_det->cd(2);
      // g_res = new TGraphErrors(ndata,unser_mean,det_res,unser_error,det_error);
      g_res = new TGraphErrors(Np,unser_mean,det_res,unser_error,det_error);
      g_res->SetMarkerStyle(20);
      //      g_res->GetXaxis()->SetRange(10,(MAX+5)*100);
      g_res->Draw("AP");
      gres_title = Form("%s;%s; Residual(ADC) ",branch_name.Data(),scan_data.Data());
      g_res->SetTitle(gres_title.Data());
      f_zero->Draw("same");
      // g_res->SetMaximum(100);
      //   g_res->SetMinimum(-100);
      c_det->SaveAs(Form("%s/run%d_%s_ped_fit.png",outputDir.Data(),run_num,device_name[idet].Data()));
  } // End of device loop
  gSystem->Exec(Form("convert $(ls -rt %s/run%d_*fit.png) %s/run%d_det_pedestal_fit_dgds_10to70.pdf",outputDir.Data(),run_num,outputDir.Data(),run_num));
  gSystem->Exec(Form("rm %s/run%d_*fit.png",outputDir.Data(),run_num));

  for(int idet=0;idet<nDET;idet++){
      printf("%s, %.2f, %f \n",
	     device_name[idet].Data(),
	     ped[idet],
	     gain[idet]);
      outfile<<Form("%s, %.2f, %f ",
	     device_name[idet].Data(),
	     ped[idet],
	     gain[idet])<<endl;
  }
  rootfile->Close();

  delete rootfile, tree, outputDir, device_name, nDET,f_zero,  ndata, det_mean, det_error,  unser_mean, unser_error, det_res, gfit_title, gres_title, ped, slope, outfilename, outfile, f_fit, init_par,my_cut,branch_name,num_samples_name,htemp, hsd, max, nbinx, vec_scandata, hsd, bin_content, bin_center,slope;

}

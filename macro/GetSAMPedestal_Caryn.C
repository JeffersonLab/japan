// jaPAN Macro Script - SAM Pedestal Script
// based on UNSER readout

void GetSAMPedestal_Caryn(int run_num = 1329,TString user_cut ="1",TString scan_data = "unser",double MIN=-1000,double MAX=1000,Int_t myii=0){


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

  //  TString rf_name =Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num);
  TFile *rootfile = TFile::Open(Form("$QW_ROOTFILES/prexPrompt_pass2_%d.000.root",run_num));
  TTree *tree= (TTree*)rootfile->Get("evt");
  //  user_cut += "&&ErrorFlag==0";

	TString outputDir = "/adaqfs/home/apar/PREX/japan/plots/pedestalOutputs";
  //---------------------------

 tree->SetAlias("beam_current", scan_data);
  tree->Draw(Form("beam_current>>htemp%d",myii),user_cut.Data(),"goff");
  TH1D* htemp = (TH1D*)gDirectory->FindObject(Form("htemp%d",myii));
  double max = htemp->GetXaxis()->GetXmax();
  int nbinx = (int)(10*max); // to one-tenth precision.

  vector<double> vec_scandata;
  TH1D *hsd = new TH1D(Form("hsd%d",myii),"scan data",nbinx,-0.05,max-0.05); 
  tree->Draw(Form("beam_current>>hsd%d",myii),Form("%s&&scandata1!=0",user_cut.Data()),"goff");
  int bin_content; 
  double bin_center;
  for(int ibin=0;ibin<nbinx;ibin++){
    bin_content = hsd->GetBinContent(ibin+1); // Histogram bin number starts from 1
    if(bin_content>0){
      bin_center = hsd->GetBinCenter(ibin+1);
      vec_scandata.push_back(bin_center);
    }
  }
  const int ndata = vec_scandata.size();

  TString device_name[] =
    {"sam1","sam2","sam3","sam4",
     "sam5","sam6","sam7","sam8"};

  const int nSAM= sizeof(device_name)/sizeof(*device_name);

  TCanvas *c_sam = new TCanvas("c_sam","c_sam",1000,500);
  c_sam->Divide(2,1);

  TF1 *f_zero = new TF1("f_zero","0",0,10000);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);

  //  const int ndata = sizeof(beam_evtcut)/sizeof(*beam_evtcut);
  double sam_mean[ndata]; 
  double sam_error[ndata];
  double unser_mean[ndata]; 
  double unser_error[ndata];
  double sam_res[ndata]; // residual

  TString gfit_title;
  TString gres_title;
  TH1D *h_stat;
  double ped[nSAM];
  double slope[nSAM];

	char outfilename[255];
	sprintf(outfilename,"%s/run%d_sam_pedestal_fit.txt",
			outputDir.Data(),run_num);
	printf("Writing output to %s\n",outfilename);
	//FILE *outfile = fopen(outfilename, "w"); 
	ofstream outfile;
	outfile.open(outfilename);

  TGraphErrors *g_res;
  TGraphErrors *g_fit;  
  TGraphErrors *g_res_ref;
  TGraphErrors *g_fit_ref;

  TString branch_name;
  TString num_samples_name;

  TF1 *f_fit = new TF1("f_fit","[1]*x+[0]",-10,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Slope");
  double init_par[2] = { 1e3,1e-3};

  TString my_cut;
  for(int i=0;i<ndata;i++){
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0",i+1);
    tree->Draw(Form("%s>>h_unser_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_unser_%d_%d",i,myii));
    unser_mean[i] = h_stat->GetMean();
    unser_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1==0",i+1);
    tree->Draw(Form("%s>>h_ped_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_ped_%d_%d",i,myii));
    if(scan_data=="unser"){
   unser_mean[i]  = unser_mean[i] - h_stat->GetMean(); // pedestal subtraction
    }
  }

  for(int isam=0;isam<nSAM;isam++){
    branch_name = Form("%s.hw_sum_raw/%s.num_samples",
		       device_name[isam].Data(),device_name[isam].Data());

      for(int i=0;i<ndata;i++){
	my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0",i+1);
	tree->Draw(Form("%s>>h_det%d_%d_%d",branch_name.Data(),isam,i,myii),
		  my_cut.Data(),"goff");
	h_stat =(TH1D*)gDirectory->FindObject(Form("h_det%d_%d_%d",isam,i,myii));
	sam_mean[i] = h_stat->GetMean();
	sam_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      c_sam->cd(1);

      g_fit = new TGraphErrors(ndata,unser_mean,sam_mean,unser_error,sam_error);
      g_fit->SetMarkerStyle(20);
      g_fit->Draw("AP");
      gfit_title = Form("%s; %s; SAM(ADC)",branch_name.Data(),scan_data.Data());
      g_fit->SetTitle(gfit_title.Data());
      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      g_fit->Fit("f_fit","Q","",MIN,MAX);

      ped[isam] = f_fit->GetParameter(0);
      slope[isam] = f_fit->GetParameter(1);
      
      for(int i=0;i<ndata;i++){
	sam_res[i] = sam_mean[i] - f_fit->Eval(unser_mean[i]);
      }
      
      c_sam->cd(2);
      g_res = new TGraphErrors(ndata,unser_mean,sam_res,unser_error,sam_error);
      g_res->SetMarkerStyle(20);
      g_res->Draw("AP");
      gres_title = Form("%s;%s; Residual(ADC) ",branch_name.Data(),scan_data.Data());
      g_res->SetTitle(gres_title.Data());
      f_zero->Draw("same");

      c_sam->SaveAs(Form("%s/run%d_%s_ped_fit.png",outputDir.Data(),run_num,device_name[isam].Data()));
  } // End of device loop
  gSystem->Exec(Form("convert %s/*sam*fit.png %s/run%d_sam_pedestal_fit.pdf",outputDir.Data(),outputDir.Data(),run_num));
  gSystem->Exec(Form("rm %s/run%d_*sam*.png",outputDir.Data(),run_num));

  for(int isam=0;isam<nSAM;isam++){
      printf("%s, %.2f, %f \n",
	     device_name[isam].Data(),
	     ped[isam],
	     slope[isam]);
      outfile<<Form("%s, %.2f, %f ",
	     device_name[isam].Data(),
	     ped[isam],
	     slope[isam])<<endl;
  }
  rootfile->Close();

  delete rootfile, tree, outputDir, device_name, nSAM,f_zero, ndata, sam_mean, sam_error,  unser_mean, unser_error, sam_res, gfit_title, gres_title, ped, slope, outfilename, outfile, f_fit, init_par,my_cut,htemp, hsd, max, nbinx, vec_scandata, hsd, bin_content, bin_center,slope;

}

// jaPAN Macro Script - SAM Pedestal Script
// based on UNSER readout

void GetSAMPedestal1537_panguin_Caryn(TString user_cut ="1",TString scan_data = "unser",double MIN=-1000,double MAX=1000,Int_t myii=0){



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


  TTree *tree= (TTree*)gROOT->FindObject("evt");

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
    {"sam1","sam5","sam3","sam7"};
     // "usr","dsr","atr1","atr2"};

  const int nDET= sizeof(device_name)/sizeof(*device_name);

  TPad *c_sam = new TPad("c_sam","c_sam",0,0,1,1);
  c_sam->Divide(1,2);
  c_sam->Draw();
  TVirtualPad* csam_fit = c_sam->cd(1);
  TVirtualPad* csam_res = c_sam->cd(2);
  csam_fit->Divide(nDET,1);
  csam_res->Divide(nDET,1);


  TF1 *f_zero = new TF1("f_zero","0",0,10000);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);

  //  const int ndata = sizeof(beam_evtcut)/sizeof(*beam_evtcut);
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


  TGraphErrors *g_sam_res;
  TGraphErrors *g_sam_fit;  
  TGraphErrors *g_sam_res_ref;
  TGraphErrors *g_sam_fit_ref;
  TMultiGraph *mg_sam_res;
  TMultiGraph *mg_sam_fit;

  TString branch_name;
  TString num_samples_name;

  TF1 *f_fit = new TF1("f_fit","[1]*x+[0]",-10,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Slope");
  double init_par[2] = { 1e3,1e-3};

  TString my_cut;
  for(int i=0;i<ndata;i++){
    // my_cut = beam_evtcut[i];
   my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0",i+1);
    tree->Draw(Form("%s>>h_unser_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_unser_%d_%d",i,myii));
    unser_mean[i] = h_stat->GetMean();
    unser_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
    // my_cut = pedestal_unser_evtcut[i];
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1==0",i+1);
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
	//	my_cut = beam_evtcut[i];
	my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0",i+1);
	tree->Draw(Form("%s>>h_det%d_%d_%d",branch_name.Data(),idet,i,myii),
		  my_cut.Data(),"goff");
	h_stat =(TH1D*)gDirectory->FindObject(Form("h_det%d_%d_%d",idet,i,myii));
	det_mean[i] = h_stat->GetMean();
	det_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      csam_fit->cd(idet+1);

      g_sam_fit = new TGraphErrors(ndata,unser_mean,det_mean,unser_error,det_error);
      g_sam_fit->SetMarkerStyle(20);
      g_sam_fit->Draw("AP");
      gfit_title = Form("%s; %s; DET(ADC)",device_name[idet].Data(),scan_data.Data());
      g_sam_fit->SetTitle(gfit_title.Data());
      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      g_sam_fit->Fit("f_fit","Q","",MIN,MAX);

      ped[idet] = f_fit->GetParameter(0);
      slope[idet] = f_fit->GetParameter(1);
      
      for(int i=0;i<ndata;i++){
	det_res[i] = det_mean[i] - f_fit->Eval(unser_mean[i]);
      }
      
      csam_res->cd(idet+1);
      g_sam_res = new TGraphErrors(ndata,unser_mean,det_res,unser_error,det_error);
      g_sam_res->SetMarkerStyle(20);
      g_sam_res->Draw("AP");
      gres_title = Form("%s;%s; Residual(ADC) ",device_name[idet].Data(),scan_data.Data());
      g_sam_res->SetTitle(gres_title.Data());
      f_zero->Draw("same");

  
  } // End of device loop



  delete tree,  device_name, nDET,f_zero,  ndata, det_mean, det_error,  unser_mean, unser_error, det_res, gfit_title, gres_title, ped, slope, f_fit, init_par,my_cut,branch_name,num_samples_name;

    delete htemp, hsd, max, nbinx, vec_scandata, hsd, bin_content, bin_center,slope;

}

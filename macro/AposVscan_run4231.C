void AposVscan_run4231(int run_num = 4231,TCut user_cut ="cleandata&&ErrorFlag==0&&scandata1!=-385")
{
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

 TFile *rootfile = TFile::Open(Form("$QW_ROOTFILES/prexPrompt_pass1_%d.000.root",run_num));
  TTree *tree= (TTree*)rootfile->Get("mul");



 TString device_name[] =
   {"2i02X","2i02Y","0l01X","0l01Y",
   "11X","11Y","4aX","4aY","4eX","4eY"};

 const int nbpm= sizeof(device_name)/sizeof(*device_name);
TCanvas *c_bpm = new TCanvas("c_bpm","c_bpm",1200,900);
  c_bpm->Divide(nbpm/2,2);


TCut beam_evtcut[] ={ 
    "scandata2==1615&&scandata1==768",
    "scandata1==-2385",
    "scandata2==-385&&scandata1==768"};

const int ndata = sizeof(beam_evtcut)/sizeof(*beam_evtcut);
 double bpm_mean[ndata]; 
  double bpm_error[ndata];
double V_mean[ndata]={1615,-2385,-385}; 
double V_error[ndata]={0,0,0};

 TString gfit_title;
  TH1D *h_stat;
  double ped[nbpm];
  double slope[nbpm];
 TGraphErrors *g_fit;  

TF1 *f_fit = new TF1("f_fit","[1]*x+[0]",-10e3,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Slope");


 TString my_cut;
 TString branch_name;
 for(int ibpm=0;ibpm<nbpm;ibpm++){
    branch_name = Form("1e3*diff_bpm%s/um",
		       device_name[ibpm].Data());
    cout<<device_name[ibpm].Data()<<endl;
      for(int i=0;i<ndata;i++){
       my_cut = beam_evtcut[i]+user_cut;
	tree->Draw(branch_name,
		  my_cut,"goff");
	h_stat =(TH1D*)gDirectory->FindObject("htemp");
	bpm_mean[i] = h_stat->GetMean();
	bpm_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }

     c_bpm->cd(ibpm+1);

      g_fit = new TGraphErrors(ndata,V_mean,bpm_mean,V_error,bpm_error);
      g_fit->SetMarkerStyle(20);
      g_fit->Draw("AP");
      gfit_title = Form("%s; AposV(counts); nm",branch_name.Data());
      g_fit->SetTitle(gfit_title.Data());
      //      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      g_fit->Fit("f_fit","QR");
    ped[ibpm] = f_fit->GetParameter(0);
      slope[ibpm]=f_fit->GetParameter(1);

 }
 for(int ibpm=0;ibpm<nbpm;ibpm++){
      printf("%s, %.2f, %f \n",
	     device_name[ibpm].Data(),
	     ped[ibpm],
	     slope[ibpm]);
  }
  rootfile->Close();


}

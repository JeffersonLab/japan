
void PITAscan_color_run3752(int run_num = 3752, TString mainBCM = "bcm_dg_ds", TString ucut = "abs(asym_bcm_an_ds/ppm)<20000&&yield_bcm_an_ds>67"){
//void IAScan_DET(){


  gROOT->SetStyle("Plain");
  gStyle->SetStatH(0.15);
  gStyle->SetStatW(0.37);
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(111);
  gStyle->SetStatX(0.9);
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
  gStyle->SetLabelSize(0.03,"x");
  gStyle->SetLabelSize(0.03,"y");
  gStyle->SetTitleSize(0.04,"hxyz");
  gStyle->SetTitleOffset(1.3,"y");
  gROOT->ForceStyle();  

  TString mydir = "~/PREX/japan/plots/parityOutputs";
  
  //  TString rf_name =Form("$QW_ROOTFILES/prexPrompt_pass2_%d.000.root",run_num);
   TString rf_name =Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num);

  TFile *rootfile = TFile::Open(rf_name);
  TTree *tree= (TTree*)rootfile->Get("mul");
  tree->AddFriend("mulc_lrb");


   TString device_name[] =
     {"asym_atl1","asym_atl2"};
   //,"cor_atl1","cor_atl2","cor_atr1","cor_atr2"};


  TString bcm_name[]=
    {"asym_bcm_dg_us","asym_cav4cQ","asym_bpm4aWS","asym_bpm4eWS","asym_bpm11WS"};

  const int nDET = sizeof(device_name)/sizeof(*device_name);
 
  const int nBCM = sizeof(bcm_name)/sizeof(*bcm_name);

  TCanvas *c_col = new TCanvas("c_col","c_col",0,0,1200,600);
  //  c_col->Divide(nDET/2,2);
 c_col->Divide(2,2);

  //  TCanvas *c_res = new TCanvas("c_res","c_res",50,50,1700,1000);
  //  c_res->Divide(nDET/2,2);

 c_col->cd(1);
 tree->Draw("(asym_bcm_dg_us-asym_bcm_an_ds)/ppm:yield_bcm_an_us:asym_bcm_an_ds/ppm",ucut.Data(),"colz");
   c_col->cd(3);
 tree->Draw("asym_bcm_dg_us/ppm:yield_bcm_an_us:asym_bcm_an_ds/ppm",ucut.Data(),"colz");
 c_col->cd(2);
 tree->Draw("(asym_bcm_dg_ds-asym_bcm_an_ds)/ppm:yield_bcm_an_us:asym_bcm_an_ds/ppm",ucut.Data(),"colz");
   c_col->cd(4);
 tree->Draw("asym_bcm_dg_ds/ppm:yield_bcm_an_us:asym_bcm_an_ds/ppm",ucut.Data(),"colz");
 

TCanvas *c_col2 = new TCanvas("c_col2","c_col2",0,0,1200,600);
  //  c_col->Divide(nDET/2,2);
 c_col2->Divide(2,2);

c_col2->cd(1);
 tree->Draw("(asym_bcm_an_us-asym_bcm_an_ds)/ppm:yield_bcm_an_us:asym_bcm_an_ds/ppm",ucut.Data(),"col2z");
   c_col2->cd(3);
 tree->Draw("asym_bcm_an_us/ppm:yield_bcm_an_us:asym_bcm_an_ds/ppm",ucut.Data(),"colz");
 c_col2->cd(2);
 tree->Draw("(asym_cav4cQ-asym_bcm_an_ds)/ppm:yield_bcm_an_us:asym_bcm_an_ds/ppm",ucut.Data(),"colz");
   c_col2->cd(4);
 tree->Draw("asym_cav4cQ/ppm:yield_bcm_an_us:asym_bcm_an_ds/ppm",ucut.Data(),"colz");
 

// c_col->cd(3);
 // tree->Draw("(asym_cav4cQ-asym_bcm_an_ds)/ppm:yield_bcm_an_us:asym_bcm_an_ds",ucut.Data(),"colz");
 //  c_col->cd(6);
 // tree->Draw("asym_cav4cQ/ppm:yield_bcm_an_us:asym_bcm_an_ds",ucut.Data(),"colz");



 // tree->Draw("asym_bcm_dg_us:yield_bcm_an_us:asym_bcm0l02",ucut.Data(),"colz");

  TH2D *h_det[nDET];


  TH2D *mybcm;
  TH2D *myE;
  TH2D *h_bcm[nBCM];
  TH2D *mybcmres;
  TH2D *myEres;
  TH2D *h_bcmres[nBCM];
  TH1D *h_scandata;
  TString branch_name;
  double Asam[nDET];
  double Asamoffs[nDET];
  double Aq[nBCM];
  double Aqoffs[nBCM];
  double sam_dev[nDET];
  double bcm_dev[nBCM];
  TString my_cut = Form("%s",ucut.Data());

  
  c_col->SaveAs(Form("%s/PITAscan_color_run%d.pdf",mydir.Data(),run_num));
  // c_res->SaveAs(Form("%s/PITAscan_color_run%d.pdf",mydir.Data(),run_num));


  //   gSystem->Exec(Form("pdfunite %s/PITAscan_run%d*.pdf %s/run%d_PITAscanall_atpeds3705maps.pdf",mydir.Data(),run_num,mydir.Data(),run_num));
  //  gSystem->Exec(Form("rm -rf PITA*run%d*.pdf",run_num));

 rootfile->Close();

}

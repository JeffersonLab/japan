
void IAScan_BPMs(int run_num = 3431){
//void IAScan_BPMs(){

  gROOT->SetStyle("Plain");
  gStyle->SetStatH(0.15);
  gStyle->SetStatW(0.25);
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(111);
  gStyle->SetStatX(0.8);
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
  
  TString rf_name =Form("$QW_ROOTFILES/prexTest_%d.000.root",run_num);
  TFile *rootfile = TFile::Open(rf_name);
  TTree *tree= (TTree*)rootfile->Get("mul");
  tree->AddFriend("mulc_lrb");

  TString device_name[] =
    {"asym_bpm4aWS","asym_bpm4eWS","asym_bpm12WS","asym_bpm16WS"};

  TString bcm_name[]=
    {"asym_bcm_an_ds","asym_bcm_an_ds3","asym_bcm_dg_ds","asym_bcm_an_us","asym_bcm_dg_us"};

  const int nDET = sizeof(device_name)/sizeof(*device_name);
  const int nBCM = sizeof(bcm_name)/sizeof(*bcm_name);

  TCanvas *c_bpm = new TCanvas("c_bpm","c_bpm",800,600);
  c_bpm->Divide(2,2);

  TH2D *h_det[nDET];
  TH2D *h_bcm[nBCM];
  TH1D *h_scandata;
  TString branch_name;
  double Abpm[nDET];
  double Aq;
  double bpm_dev[nDET];
  TString my_cut = Form("cleandata&&ErrorFlag==0");

  for(int idet=0;idet<nDET;idet++){
      c_bpm->cd(idet+1);
      branch_name = Form("(%s-asym_bcm_an_ds)/ppm",device_name[idet].Data());
      tree->Draw(Form("%s:scandata1>>h_det%d",branch_name.Data(),idet),my_cut.Data(),"prof");
      h_det[idet] =(TH2D*)gDirectory->FindObject(Form("h_det%d",idet));
      h_det[idet]->SetDirectory(gROOT);
      TString yTitle = device_name[idet];
      h_det[idet]->SetYTitle(Form("%s",yTitle.Data()));
      h_det[idet]->SetXTitle("scandata1");
      h_det[idet]->SetLineColor(kBlue);
      h_det[idet]->Fit("pol1");
      TF1 *fit = h_det[idet]->GetFunction("pol1");
      fit->SetLineColor(kRed);
      Abpm[idet] = fit->GetMaximum();
      cout<< Abpm[idet] << endl;
   }
    c_bpm->SaveAs(Form("bpm_run%d.pdf",run_num));

TCanvas *c_bcm = new TCanvas("c_bcm","c_bcm",800,600);
   c_bcm->Divide(2,2);
   for(int ibcm=0;ibcm<nBCM-1;ibcm++){
      c_bcm->cd(ibcm+1);
      branch_name = Form("%s/ppm",bcm_name[ibcm].Data());
      tree->Draw(Form("%s:scandata1>>h_%d",branch_name.Data(),ibcm),my_cut.Data(),"prof");
      h_bcm[ibcm] = (TH2D*)gDirectory->FindObject(Form("h_%d",ibcm));
      h_bcm[ibcm]->SetDirectory(gROOT);
      TString yTitle = bcm_name[ibcm];
      h_bcm[ibcm]->SetYTitle(Form("%s",yTitle.Data()));
      h_bcm[ibcm]->SetXTitle("scandata1");
      h_bcm[ibcm]->SetLineColor(kBlue);
      h_bcm[ibcm]->Fit("pol1");
      TF1 *fitbcm = h_bcm[ibcm]->GetFunction("pol1");

      fitbcm->SetLineColor(kRed);
      Aq = fitbcm->GetMaximum();
      cout<< Aq << endl;
   }
    c_bcm->SaveAs(Form("bcm_run%d.pdf",run_num));
    gSystem->Exec(Form("pdfunite *.pdf plots/run%d_IAscan_bpm.pdf",run_num));
    gSystem->Exec(Form("rm -rf *.pdf"));
      for(int i=0;i<nDET;i++){
	bpm_dev[i] = Abpm[i]/Aq*100;
        cout<< Form("%s deviation",device_name[i].Data())<<"\t"<<bpm_dev[i]<<"%"<<endl;
      }
 rootfile->Close();

}

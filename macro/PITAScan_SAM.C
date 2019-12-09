
void PITAScan_SAM(int run_num = 3431){
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
  gStyle->SetLabelSize(0.035,"x");
  gStyle->SetLabelSize(0.035,"y");
  gStyle->SetTitleSize(0.06,"hxyz");
  gROOT->ForceStyle();  
  
  TString rf_name =Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num);
  TFile *rootfile = TFile::Open(rf_name);
  TTree *tree= (TTree*)rootfile->Get("mul");
  tree->AddFriend("mulc_lrb");

  TString device_name[] =
      {"cor_sam1","cor_sam2","cor_sam3","cor_sam4",
       "cor_sam5","cor_sam6","cor_sam7","cor_sam8"};
    // {"asym_sam1","asym_sam2","asym_sam3","asym_sam4",
    //  "asym_sam5","asym_sam6","asym_sam7","asym_sam8"};
    // {"asym_usl","asym_dsl","asym_usr","asym_dsr"};
  TString bcm_name[]=
    {"asym_bcm_an_ds3","asym_bcm_dg_ds","asym_bcm_an_us","asym_bcm_dg_us","asym_cav4Q"};

  const int nDET = sizeof(device_name)/sizeof(*device_name);
  // const int nDET2 = sizeof(detnam)/sizeof(*detnam);
  const int nBCM = sizeof(bcm_name)/sizeof(*bcm_name);

  TCanvas *c_sam_det = new TCanvas("c_sam_det","c_sam_det",1200,800);
  c_sam_det->Divide(nDET/2,2);

  // TCanvas *c_det = new TCanvas("c_det","c_det",1200,600);
  //c_sam_det->Divide(n,2);


  TH2D *h_det[nDET];
  TH2D *mybcm;
  TH2D *h_bcm[nBCM];
  TH1D *h_scandata;
  TString branch_name;
  double Asam[nDET];
  double Aq[nBCM];
  double sam_dev[nDET];
  double bcm_dev[nBCM];
  TString my_cut = Form("cleandata&&ErrorFlag==0");

  


  for(int idet=0;idet<nDET;idet++){
      c_sam_det->cd(idet+1);
      //      branch_name = Form("(%s+asym_bcm_an_ds3-asym_bcm_an_ds)/ppm",device_name[idet].Data());
      branch_name = Form("(%s)/ppm",device_name[idet].Data());
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
      Asam[idet] = fit->GetParameter(1);
      cout<< Asam[idet] << endl;
   }
    c_sam_det->SaveAs(Form("bpm_run%d.pdf",run_num));

TCanvas *c_bcm = new TCanvas("c_bcm","c_bcm",1200,600);
   c_bcm->Divide(nBCM/2+1,2);


  c_bcm->cd(1);
      //      branch_name = Form("(%s+asym_bcm_an_ds3-asym_bcm_an_ds)/ppm",device_name[idet].Data());
    //  branch_name = Form("(%s)/ppm","asym_bcm_an_ds");
      tree->Draw(Form("asym_bcm_an_ds/ppm:scandata1>>mybcm"),my_cut.Data(),"prof");
      mybcm =(TH2D*)gDirectory->FindObject("mybcm");
      mybcm->SetDirectory(gROOT);
      TString yTitle = "asym_bcm_an_ds";
      mybcm->SetYTitle(Form("%s",yTitle.Data()));
      mybcm->SetXTitle("scandata1");
      mybcm->SetLineColor(kBlue);
      mybcm->Fit("pol1");
      TF1 *fit =mybcm->GetFunction("pol1");
      fit->SetLineColor(kRed);
      double myslope = fit->GetParameter(1);
      cout<< myslope << endl;


   for(int ibcm=0;ibcm<nBCM-1;ibcm++){
      c_bcm->cd(ibcm+1+1);
      branch_name = Form("%s/ppm-asym_bcm_an_ds/ppm",bcm_name[ibcm].Data());
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
      Aq[ibcm] = fitbcm->GetParameter(1);
      cout<< Aq[ibcm] << endl;
   }




    c_bcm->SaveAs(Form("bpm_run%d.pdf",run_num));
    gSystem->Exec(Form("pdfunite *.pdf ~/PREX/japan/plots/parityOutputs/run%d_PITAscan_sam.pdf",run_num));
    gSystem->Exec(Form("rm -rf *.pdf"));


    cout<<"asym_bcm_an_ds Slope: "<<myslope<<endl;

   for(int ibcm=0;ibcm<nBCM-1;ibcm++){
	bcm_dev[ibcm] = Aq[ibcm]/myslope*100;
        cout<< Form("%s deviation",bcm_name[ibcm].Data())<<"\t"<<bcm_dev[ibcm]<<"%"<<endl;
   }


      for(int i=0;i<nDET;i++){
	sam_dev[i] = Asam[i]/myslope*100;
        cout<< Form("%s deviation",device_name[i].Data())<<"\t"<<sam_dev[i]<<"%"<<endl;
      }
 rootfile->Close();

}

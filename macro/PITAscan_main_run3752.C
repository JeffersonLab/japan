
void PITAscan_main_run3752(int run_num = 3752, TString mainBCM = "bcm_an_ds", TString ucut = "cleandata||(scandata1==6&&CodaEventNumber>94e3)"){
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
  gStyle->SetTitleSize(0.05,"hxyz");
  gStyle->SetTitleOffset(1.3,"y");
  gROOT->ForceStyle();  

  TString mydir = "~/PREX/japan/plots/parityOutputs";
  
  //  TString rf_name =Form("$QW_ROOTFILES/prexPrompt_pass2_%d.000.root",run_num);
     TString rf_name =Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num);

  TFile *rootfile = TFile::Open(rf_name);
  TTree *tree= (TTree*)rootfile->Get("mul");
  tree->AddFriend("mulc_lrb");

    TString device_name2[] =
    //  {"cor_sam1","cor_sam2","cor_sam3","cor_sam4",
    //  "cor_sam5","cor_sam6","cor_sam7","cor_sam8"
    //  {"asym_sam1","asym_sam2","asym_sam3","asym_sam4",
    //   "asym_sam5","asym_sam6","asym_sam7","asym_sam8"};
      {"asym_usl","asym_dsl","asym_usr","asym_dsr"};
    //,"asym_usl+asym_bcm_dg_ds"};
    //,"asym_dsl+asym_bcm_dg_ds","asym_usr+asym_bcm_dg_ds","asym_dsr+asym_bcm_dg_ds"};

 TString device_name3[] =
   //     {"cor_sam1","cor_sam2","cor_sam3","cor_sam4",
   //  "cor_sam5","cor_sam6","cor_sam7","cor_sam8",
   { "asym_sam1","asym_sam2","asym_sam3","asym_sam4",
     "asym_sam5","asym_sam6","asym_sam7","asym_sam8"};

   TString device_name[] =
     {"asym_atl1","asym_atl2","asym_atr1","asym_atr2"};
   //,"cor_atl1","cor_atl2","cor_atr1","cor_atr2"};


  TString bcm_name[]=
    {"asym_bcm_an_us","asym_bcm_an_ds","asym_bcm_dg_us","asym_cav4cQ"};

  const int nDET = sizeof(device_name)/sizeof(*device_name);
  const int nDET2 = sizeof(device_name2)/sizeof(*device_name2);
  const int nDET3 = sizeof(device_name3)/sizeof(*device_name3);

  const int nBCM = sizeof(bcm_name)/sizeof(*bcm_name);

  TCanvas *c_sam_det = new TCanvas("c_sam_det","c_sam_det",200,200,1700,1000);
  c_sam_det->Divide(nDET/2,2);

  //  TCanvas *c_sam_det2 = new TCanvas("c_sam_det2","c_sam_det2",150,150,1700,1000);
  // c_sam_det2->Divide(nDET2/2,2);

  // TCanvas *c_sam_det3 = new TCanvas("c_sam_det3","c_sam_det3",100,100,1700,1000);
  /// c_sam_det2->Divide(nDET3/2,2);


  TCanvas *c_res = new TCanvas("c_res","c_res",50,50,1700,1000);
  c_res->Divide(nDET/2,2);

  // TCanvas *c_res2 = new TCanvas("c_res2","c_res2",40,40,1700,1000);
  //  c_res2->Divide(nDET2/2,2);

  // TCanvas *c_res3 = new TCanvas("c_res3","c_res3",30,30,1700,1000);
  //  c_res3->Divide(nDET3/2,2);


  // TCanvas *c_det = new TCanvas("c_det","c_det",1200,600);
  //c_sam_det->Divide(n,2);



  TH2D *h_det[nDET];
  TH2D *h_det2[nDET2];
  TH2D *h_det3[nDET3];

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

  


  for(int idet=0;idet<nDET;idet++){
      c_sam_det->cd(idet+1);
      //      branch_name = Form("(%s+asym_bcm_an_ds3-asym_bcm_an_ds)/ppm",device_name[idet].Data());
      branch_name = Form("(%s)/ppm",device_name[idet].Data());
      tree->Draw(Form("%s:scandata1>>h_det%d",branch_name.Data(),idet),my_cut.Data(),"prof");
      h_det[idet] =(TH2D*)gDirectory->FindObject(Form("h_det%d",idet));
      h_det[idet]->SetDirectory(gROOT);
      TString yTitle = device_name[idet];
    h_det[idet]->SetTitle(Form("%s",yTitle.Data()));
      h_det[idet]->SetYTitle(Form("%s",yTitle.Data()));
      h_det[idet]->SetXTitle("scandata1");
      h_det[idet]->SetLineColor(kBlue);
     h_det[idet]->SetMarkerStyle(20);
      h_det[idet]->Fit("pol1");
      TF1 *fit = h_det[idet]->GetFunction("pol1");
      fit->SetLineColor(kRed);
      Asam[idet] = fit->GetParameter(1);
      Asamoffs[idet] = fit->GetParameter(0);
      cout<< Asam[idet] << endl;
   }
  c_sam_det->SaveAs(Form("%s/PITAscan_run%d.pdf",mydir.Data(),run_num));


 for(int idet=0;idet<nDET;idet++){
      c_res->cd(idet+1);
      //      branch_name = Form("(%s+asym_bcm_an_ds3-asym_bcm_an_ds)/ppm",device_name[idet].Data());
      branch_name = Form("(%s)/ppm",device_name[idet].Data());
      tree->Draw(Form("%s-%6.5f*scandata1-%6.5f:scandata1>>h_det%d",branch_name.Data(),Asam[idet],Asamoffs[idet],idet),my_cut.Data(),"prof");
      h_det[idet] =(TH2D*)gDirectory->FindObject(Form("h_det%d",idet));
      h_det[idet]->SetDirectory(gROOT);
      TString yTitle = device_name[idet];
      h_det[idet]->SetTitle(Form("%s residual",yTitle.Data()));
      h_det[idet]->SetYTitle(Form("%s",yTitle.Data()));
      h_det[idet]->SetXTitle("scandata1");
      h_det[idet]->SetLineColor(kBlue);
      h_det[idet]->SetMarkerStyle(20);
      //      h_det[idet]->Fit("pol1");
      //   TF1 *fit = h_det[idet]->GetFunction("pol1");
      //  fit->SetLineColor(kRed);
      // Asam[idet] = fit->GetParameter(1);
      //  cout<< Asam[idet] << endl;
   }
  c_res->SaveAs(Form("%s/PITAscan_run%d_res.pdf",mydir.Data(),run_num));


  TCanvas *c_bcm = new TCanvas("c_bcm","c_bcm",100,100,1700,1000);
   c_bcm->Divide(nBCM/2+2,2);

  TCanvas *c_bcmres = new TCanvas("c_bcmres","c_bcmres",50,50,1700,1000);
   c_bcmres->Divide(nBCM/2+2,2);


  c_bcm->cd(1);
      //      branch_name = Form("(%s+asym_bcm_an_ds3-asym_bcm_an_ds)/ppm",device_name[idet].Data());
    //  branch_name = Form("(%s)/ppm","asym_bcm_an_ds");
  tree->Draw(Form("asym_%s/ppm:scandata1>>mybcm",mainBCM.Data()),my_cut.Data(),"prof");
      mybcm =(TH2D*)gDirectory->FindObject("mybcm");
      mybcm->SetDirectory(gROOT);
      TString yTitle = Form("asym_%s",mainBCM.Data());
    mybcm->SetTitle(Form("%s",yTitle.Data()));
      mybcm->SetYTitle(Form("%s",yTitle.Data()));
      mybcm->SetXTitle("scandata1");
      mybcm->SetLineColor(kBlue);
      mybcm->SetMarkerStyle(20);
      mybcm->Fit("pol1");
      TF1 *fit =mybcm->GetFunction("pol1");
      fit->SetLineColor(kRed);
      double myslope = fit->GetParameter(1);
     double myoffs = fit->GetParameter(0);
      cout<< myslope << endl;

  c_bcm->cd(2);
      //      branch_name = Form("(%s+asym_bcm_an_ds3-asym_bcm_an_ds)/ppm",device_name[idet].Data());
    //  branch_name = Form("(%s)/ppm","asym_bcm_an_ds");
  tree->Draw("diff_bpm11X/um:scandata1>>myE",my_cut.Data(),"prof");
      myE =(TH2D*)gDirectory->FindObject("myE");
      myE->SetDirectory(gROOT);
      TString yETitle = "diff_bpm11X";
    myE->SetTitle(Form("%s",yETitle.Data()));
      myE->SetYTitle(Form("%s",yETitle.Data()));
      myE->SetXTitle("scandata1");
      myE->SetLineColor(kBlue);
      myE->SetMarkerStyle(20);
      myE->Fit("pol1");
      TF1 *fitE =myE->GetFunction("pol1");
      fit->SetLineColor(kRed);
      double myEslope = fitE->GetParameter(1);
      double myEoffs = fitE->GetParameter(0);
      cout<< myEslope << endl;


   for(int ibcm=0;ibcm<nBCM;ibcm++){
      c_bcm->cd(ibcm+1+2);
      branch_name = Form("%s/ppm-0*asym_%s/ppm",bcm_name[ibcm].Data(),mainBCM.Data());
      tree->Draw(Form("%s:scandata1>>h_%d",branch_name.Data(),ibcm),my_cut.Data(),"prof");
      h_bcm[ibcm] = (TH2D*)gDirectory->FindObject(Form("h_%d",ibcm));
      h_bcm[ibcm]->SetDirectory(gROOT);
      TString yTitle = bcm_name[ibcm];
     h_bcm[ibcm]->SetTitle(Form("%s",yTitle.Data()));
      h_bcm[ibcm]->SetYTitle(Form("%s",yTitle.Data()));
      h_bcm[ibcm]->SetXTitle("scandata1");
      h_bcm[ibcm]->SetLineColor(kBlue);
     h_bcm[ibcm]->SetMarkerStyle(20);
      h_bcm[ibcm]->Fit("pol1");
      TF1 *fitbcm = h_bcm[ibcm]->GetFunction("pol1");
      fitbcm->SetLineColor(kRed);
      Aq[ibcm] = fitbcm->GetParameter(1);
      Aqoffs[ibcm] = fitbcm->GetParameter(0);
      cout<< Aq[ibcm] << endl;
   }

 c_bcmres->cd(1);
      //      branch_name = Form("(%s+asym_bcmres_an_ds3-asym_bcmres_an_ds)/ppm",device_name[idet].Data());
    //  branch_name = Form("(%s)/ppm","asym_bcmres_an_ds");
 tree->Draw(Form("(asym_%s/ppm-%6.5f*scandata1-%6.5f):scandata1>>mybcmres",mainBCM.Data(),myslope,myoffs),my_cut.Data(),"prof");
      mybcmres =(TH2D*)gDirectory->FindObject("mybcmres");
      mybcmres->SetDirectory(gROOT);
      yTitle = Form("asym_%s Residual",mainBCM.Data());
     mybcmres->SetTitle(Form("%s",yTitle.Data()));
      mybcmres->SetYTitle(Form("%s",yTitle.Data()));
      mybcmres->SetXTitle("scandata1");
      mybcmres->SetLineColor(kBlue);
      mybcmres->SetMarkerStyle(20);

  c_bcmres->cd(2);
      //      branch_name = Form("(%s+asym_bcmres_an_ds3-asym_bcmres_an_ds)/ppm",device_name[idet].Data());
    //  branch_name = Form("(%s)/ppm","asym_bcmres_an_ds");
  tree->Draw(Form("(diff_bpm11X/um-%6.5f*scandata1-%6.5f):scandata1>>myEres",myEslope,myEoffs),my_cut.Data(),"prof");
      myEres =(TH2D*)gDirectory->FindObject("myEres");
      myEres->SetDirectory(gROOT);
      yETitle = "diff_bpm11X";
       myEres->SetTitle(Form("%s Residual",yETitle.Data()));
      myEres->SetYTitle(Form("%s Residual",yETitle.Data()));
      myEres->SetXTitle("scandata1");
      myEres->SetLineColor(kBlue);
      myEres->SetMarkerStyle(20);

   for(int ibcm=0;ibcm<nBCM;ibcm++){
      c_bcmres->cd(ibcm+1+2);
      branch_name = Form("(%s/ppm-%6.5f*scandata1-%6.5f)",bcm_name[ibcm].Data(),Aq[ibcm],Aqoffs[ibcm]);
      tree->Draw(Form("%s:scandata1>>h_%d",branch_name.Data(),ibcm),my_cut.Data(),"prof");
      h_bcmres[ibcm] = (TH2D*)gDirectory->FindObject(Form("h_%d",ibcm));
      h_bcmres[ibcm]->SetDirectory(gROOT);
      yTitle = bcm_name[ibcm];
  h_bcmres[ibcm]->SetTitle(Form("%s Residual",yTitle.Data()));
      h_bcmres[ibcm]->SetYTitle(Form("%s Residual",yTitle.Data()));
      h_bcmres[ibcm]->SetXTitle("scandata1");
      h_bcmres[ibcm]->SetLineColor(kBlue);
     h_bcmres[ibcm]->SetMarkerStyle(20);
   }


   c_bcm->SaveAs(Form("%s/PITAscan_run%d_bcm.pdf",mydir.Data(),run_num));
   c_bcmres->SaveAs(Form("%s/PITAscan_run%d_bcmres.pdf",mydir.Data(),run_num));
   gSystem->Exec(Form("pdfunite %s/PITAscan_run%d*.pdf %s/run%d_PITAscanat_andspeds.pdf",mydir.Data(),run_num,mydir.Data(),run_num));
   gSystem->Exec(Form("rm -rf %s/PITA*run%d*.pdf",mydir.Data(),run_num));

    cout<<Form("asym_%s pita-slope: ",mainBCM.Data())<<myslope<<endl;

   for(int ibcm=0;ibcm<nBCM-1;ibcm++){
	bcm_dev[ibcm] = Aq[ibcm]/myslope*100;
        cout<< Form("%s slope(%s of pita-slope):",bcm_name[ibcm].Data(),"%")<<"\t"<<bcm_dev[ibcm]<<"%"<<endl;
   }


      for(int i=0;i<nDET;i++){
	sam_dev[i] = Asam[i]/myslope*100;
        cout<< Form("%s slope(%s of pita-slope):",device_name[i].Data(),"%")<<"\t"<<sam_dev[i]<<"%"<<endl;
      }
 rootfile->Close();

}

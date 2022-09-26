void GetBCMResolution(Int_t runnumber,TString treeName){

  gStyle->SetStatW(0.35);
  gStyle->SetStatH(0.15);
  TFile *postpanFile = TFile::Open(Form("./postpan/japanOutput/prexRespin1_%d_regress_bcm_%s.root",runnumber,treeName.Data()));
  TTree *reg = (TTree*)postpanFile->Get("reg");
  
  TString bcm_name[] = {"asym_bcm_an_us","asym_bcm_an_ds","asym_bcm_an_ds3",
  			"asym_bcm_dg_us","asym_bcm_dg_ds"};

  TF1* f1 = new TF1("f1","gaus",-1,1);

  Double_t c_width = 400;
  TCanvas *c_sam = new TCanvas("c_sam","c_sam",2*c_width,2*c_width);
  c_sam->Divide(2,2);
  
  c_sam->cd(1);
  reg->Draw("reg_asym_sam2+reg_asym_sam4","ok_cut");
  c_sam->cd(2);
  reg->Draw("reg_asym_sam6+reg_asym_sam8","ok_cut");
  c_sam->cd(3);
  reg->Draw("reg_asym_sam2+reg_asym_sam6:reg_asym_sam4+reg_asym_sam8","ok_cut");
  c_sam->cd(4);
  reg->Draw("reg_asym_sam2+reg_asym_sam6-(reg_asym_sam4+reg_asym_sam8) >>hsamresol","ok_cut");
  double mean = hsamresol->GetMean();
  double rms = hsamresol->GetRMS();
  Double_t sam_resolution = (1e6*rms)/4.0;
  c_sam->SaveAs(Form("run%d_sam_resolution.png",runnumber));

  TCanvas *c_dd = new TCanvas("c_dd","c_dd",3*c_width,2*c_width);
  c_dd->Divide(3,2);
  Double_t sam_bcm_rms[5];
  Double_t sam_bcm_slope[5];
  Double_t sam_bcm_slope_err[5];
  for(Int_t ibcm=0;ibcm<5;ibcm++){
    char* this_bcm  = bcm_name[ibcm].Data();
    TVirtualPad* c_buff;
    c_dd->cd(1);
    reg->Draw("(reg_asym_sam2+reg_asym_sam6+reg_asym_sam4+reg_asym_sam8)/4","ok_cut");
    c_dd->cd(2);
    reg->Draw(this_bcm,"ok_cut");
    c_buff = c_dd->cd(3);
    reg->Draw(Form("(reg_asym_sam2+reg_asym_sam6+reg_asym_sam4+reg_asym_sam8)/4:%s",this_bcm),"ok_cut","prof");
    TProfile* prof_buff = (TProfile*)c_buff->FindObject("htemp");
    prof_buff->Draw();gPad->Update();
    ((TPaveStats*)prof_buff->FindObject("stats"))->SetOptStat(0);
    ((TPaveStats*)prof_buff->FindObject("stats"))->SetOptFit(1);
    Double_t mean = prof_buff->GetMean(1);
    Double_t rms = prof_buff->GetRMS(1);
    TF1* fpol1 = new TF1("fpol1","pol1",-1,1);
    prof_buff->Fit("fpol1","QR","",mean-3*rms , mean+3*rms);
    c_buff->Modified();
    sam_bcm_slope[ibcm] = fpol1->GetParameter(1);
    sam_bcm_slope_err[ibcm] = fpol1->GetParError(1);
    c_dd->cd(4);
    reg->Draw(Form("(reg_asym_sam2+reg_asym_sam6+reg_asym_sam4+reg_asym_sam8)/4:%s",this_bcm),"ok_cut","");
    c_dd->cd(5);
    reg->Draw(Form("(reg_asym_sam2+reg_asym_sam6+reg_asym_sam4+reg_asym_sam8)/4-%s : %s",this_bcm,this_bcm),"ok_cut","");
    c_buff = c_dd->cd(6);
    reg->Draw(Form("(reg_asym_sam2+reg_asym_sam6+reg_asym_sam4+reg_asym_sam8)/4-%lf*%s",1.0,this_bcm),"ok_cut");
    h_buff = (TH1D*)c_buff->FindObject("htemp");
    Double_t rms = h_buff->GetRMS();
    sam_bcm_rms[ibcm] = 1e6*rms;
    c_dd->SaveAs(Form("run%d_%s_sam_dd.png",runnumber,this_bcm));
  }

  for(int ibcm=0;ibcm<5;ibcm++){
    cout <<"(asym_sam2 +asym_sam6 +asym_sam4 +asym_sam8)/4 - "  
	 << bcm_name[ibcm] << ":\t" << sam_bcm_rms[ibcm]<<"\t"
	 << sam_bcm_slope[ibcm] << "+/-" << sam_bcm_slope_err[ibcm]<<  endl;
  }


  Double_t bcmdd_rms[3];

  TCanvas *c_bcm = new TCanvas("c_bcm","c_bcm",3*c_width,c_width);
  c_bcm->Divide(3,1);
  c_bcm->cd(1);
  reg->Draw("asym_bcm_an_us - asym_bcm_an_ds>>bcm1(100,-2e-4,2e-4)","ok_cut");
  double mean = bcm1->GetMean();
  double rms = bcm1->GetRMS();
  bcmdd_rms[0] = 1e6*rms;

  c_bcm->cd(2);
  reg->Draw("asym_bcm_an_us - asym_bcm_an_ds3>>bcm2(100,-2e-4,2e-4)","ok_cut");
  double mean = bcm2->GetMean();
  double rms = bcm2->GetRMS();
  bcmdd_rms[1] = 1e6*rms;

  c_bcm->cd(3);
  reg->Draw("asym_bcm_dg_us - asym_bcm_dg_ds>>bcm3(100,-2e-4,2e-4)","ok_cut");
  double mean = bcm3->GetMean();
  double rms = bcm3->GetRMS();
  bcmdd_rms[2] = 1e6*rms;

  c_bcm->SaveAs(Form("run%d_bcm_dd.png",runnumber));  

  cout << "asym_bcm_an_us - asym_bcm_an_ds" << ":\t" << bcmdd_rms[0] <<endl;
  cout << "asym_bcm_an_us - asym_bcm_an_ds3" << ":\t" << bcmdd_rms[1] <<endl;
  cout << "asym_bcm_dg_us - asym_bcm_dg_ds" << ":\t" << bcmdd_rms[2]  <<endl;

  cout << " SAM resolution (ppm) : "  << endl;
  cout << sam_resolution << endl;
  Double_t sam_noise;
  cout << " SAM noise (ppm) : "  << endl;
  cout << sqrt( 0.5 * ( sam_bcm_rms[0]**2 + sam_bcm_rms[1]**2 - 2*(sam_resolution**2) -bcmdd_rms[0]**2 )) << endl;
  cout << sqrt( 0.5 * ( sam_bcm_rms[0]**2 + sam_bcm_rms[2]**2 - 2*(sam_resolution**2) -bcmdd_rms[1]**2 )) << endl;
  cout << sqrt( 0.5 * ( sam_bcm_rms[3]**2 + sam_bcm_rms[4]**2 - 2*(sam_resolution**2) -bcmdd_rms[2]**2 )) << endl;

  sam_noise = 0.5 * ( sam_bcm_rms[0]**2 + sam_bcm_rms[1]**2 - 2*(sam_resolution**2) -bcmdd_rms[0]**2 );
  cout << bcm_name[0] << " resolution : " ;
  cout << sqrt( sam_bcm_rms[0]**2 - sam_noise - sam_resolution**2 ) << endl;
  cout << bcm_name[1] << " resolution : " ;
  cout << sqrt( sam_bcm_rms[1]**2 - sam_noise - sam_resolution**2 ) << endl;

  sam_noise = 0.5 * ( sam_bcm_rms[0]**2 + sam_bcm_rms[2]**2 - 2*(sam_resolution**2) -bcmdd_rms[1]**2 );
  cout << bcm_name[0] << " resolution : " ;
  cout << sqrt( sam_bcm_rms[0]**2 - sam_noise - sam_resolution**2 ) << endl;
  cout << bcm_name[2] << " resolution : " ;
  cout << sqrt( sam_bcm_rms[2]**2 - sam_noise - sam_resolution**2 ) << endl;

  sam_noise = 0.5 * ( sam_bcm_rms[3]**2 + sam_bcm_rms[4]**2 - 2*(sam_resolution**2) -bcmdd_rms[2]**2 );
  cout << bcm_name[3] << " resolution : " ;
  cout << sqrt( sam_bcm_rms[3]**2 - sam_noise - sam_resolution**2 ) << endl;
  cout << bcm_name[4] << " resolution : " ;
  cout << sqrt( sam_bcm_rms[4]**2 - sam_noise - sam_resolution**2 ) << endl;

}

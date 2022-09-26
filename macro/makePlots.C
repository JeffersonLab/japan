void makePlots(TString expt = "CREX", TString ana = "unser", TString version = "new", TString type = "orig", Double_t errorThreshold = 10.0 /*Needs to be ~0.02 for old error bar type of runs, and needs to be ~4.0 for ALL type analysis*/) {
  TString rf_name =Form("./%s_%s_pedestals_%s_%s.root",version.Data(),ana.Data(),expt.Data(),type.Data());
  TFile *rootfile = TFile::Open(rf_name);  
  if(rootfile == NULL){
    cout << rf_name 
      << " doesn't exist !!" << endl;
    return;
  }
  TString outputDir = "./expt-plots/";
  TTree *ped = (TTree*)rootfile->Get("ped");

  TString rf_name2 =Form("./%s_unser_pedestals_%s_orig.root",version.Data(),expt.Data());
  TFile *rootfile2 = TFile::Open(rf_name2);  
  TTree *ped2 = NULL;
  if(rootfile2 == NULL){
    cout << rf_name2
      << " doesn't exist !!" << endl;
  }
  else { 
    ped2 = (TTree*)rootfile2->Get("ped");
  }

  std::vector<TString> devices =
  {"bcm_an_us", "bcm_an_ds",
    "bcm_an_ds3","bcm_an_ds10",
    "bcm_dg_us","bcm_dg_ds","cav4bQ",
    "cav4cQ","cav4dQ","bcm_dg_usc","bcm_dg_dsc",
    "cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
  if (expt == "PREX") { //PREX 
    devices =
    {"bcm_an_us", "bcm_an_ds",
      "bcm_an_ds3","bcm_an_ds10",
      "bcm_dg_us","bcm_dg_ds","cav4bQ",
      "cav4cQ","cav4dQ",
      "cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
  }
  if (ana == "all"){
    devices = 
    {"bcm_an_us", "bcm_an_ds",
      "bcm_an_ds3","bcm_an_ds10",
      "bcm_dg_us","bcm_dg_ds","cav4bQ",
      "cav4cQ","cav4dQ","bcm_dg_usc","bcm_dg_dsc",
      "cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI",
      "usl","usr","dsl","dsr",
      "atl1","atr1","atl2","atr2",
      "sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8"};
    if (expt == "PREX") { //PREX 
      devices =
      {"bcm_an_us", "bcm_an_ds",
        "bcm_an_ds3","bcm_an_ds10",
        "bcm_dg_us","bcm_dg_ds","cav4bQ",
        "cav4cQ","cav4dQ",
        "cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI",
        "usl","usr","dsl","dsr",
        "atl1","atr1","atl2","atr2",
        "sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8"};
    }
  }

  FILE *pFile;
  pFile = fopen(Form("%s/%s_%s_pedestals_%s_average_%s.txt",outputDir.Data(),version.Data(),ana.Data(),expt.Data(),type.Data()),"w+");

  TCanvas *c1 = new TCanvas("c1","c1",3000,1600);
  c1->Divide(3,2);
  Double_t tg_ped     = 0.0;
  Double_t tg_ped_err = 0.0;
  Double_t tg_unser_ped     = 0.0;
  Double_t tg_unser_ped_err = 0.0;
  Double_t th_ped     = 0.0;
  Double_t th_ped_err = 0.0;
  Double_t gain       = 0.0;

  fprintf(pFile, "Device Name, p0 Fit Pedestal, p0 Fit Pedestal Error, TH1 weighted mean Pedestal, TH1 weighted mean Pedestal Error, Gain scale factor\n");
  printf("Device Name, p0 Fit Pedestal, p0 Fit Pedestal Error, TH1 weighted mean Pedestal, TH1 weighted mean Pedestal Error, Gain scale factor\n");

  TGraph* g_buff;
  TVirtualPad* pad_buff;

  TString cut = "1==1";
  for (Int_t i = 0 ; i<devices.size(); i++ ) {
    cut = Form("%s_pedestal_error>%f && %s_pedestal_error<1000000.0",devices[i].Data(),errorThreshold,devices[i].Data());
    gStyle->SetOptStat(112211);
    pad_buff = c1->cd(1);
    Int_t nruns = ped->Draw(Form("%s_pedestal",devices[i].Data()),Form("%s_pedestal_error*(%s && %s_valid)",devices[i].Data(),cut.Data(),devices[i].Data()));
    if (nruns > 0) {
      th_ped     = ((TH1D*)gROOT->FindObject("htemp"))->GetMean();
      th_ped_err = ((TH1D*)gROOT->FindObject("htemp"))->GetMeanError();
      ((TH1D*)gROOT->FindObject("htemp"))->SetNameTitle("W-Mean",Form("%s_pedestal, error weighted",devices[i].Data()));
    }
    else {
      th_ped     = 0.0;
      th_ped_err = 0.0;
    }
    pad_buff = c1->cd(2);
    nruns = ped->Draw(Form("%s_pedestal:%s_pedestal_error:run_number",devices[i].Data(),devices[i].Data()),Form("%s && %s_valid",cut.Data(),devices[i].Data()),"goff");
    if (nruns > 0) {
      TGraphErrors* tgbuf = new TGraphErrors(nruns, ped->GetV3(),ped->GetV1(),0,ped->GetV2());
      tgbuf->SetNameTitle(Form("%s %s pedestal vs. run",devices[i].Data(),ana.Data()),Form("%s %s pedestal vs. run",devices[i].Data(),ana.Data()));
      tgbuf->Fit("pol0","QR","",0,10000);
      TF1* p0 = (TF1*)gROOT->FindObject("pol0");
      tg_ped     = p0->GetParameter(0);
      tg_ped_err = p0->GetParError(0);
      gStyle->SetOptFit(1);
      tgbuf->SetMarkerStyle(20);
      tgbuf->SetMarkerSize(1);
      tgbuf->Draw("AP");
    }
    else {
      tg_ped     = 0.0;
      tg_ped_err = 0.0;
    }
    pad_buff = c1->cd(3);
    nruns = ped->Draw(Form("%s_pedestal_error:run_number",devices[i].Data()),Form("%s_pedestal_error",devices[i].Data()),"*");
    if (nruns > 0) {
      g_buff = (TGraph*)pad_buff->FindObject("Graph");
      if (g_buff!=0) {
        g_buff->SetNameTitle("GraphAll",Form("%s_pedestal_error vs. Run, red = cut out",devices[i].Data()));
      }
      ((TH1D*)gROOT->FindObject("htemp"))->SetNameTitle("htempAll",Form("%s_pedestal_error vs. Run, red = cut out",devices[i].Data()));
      ped->Draw(Form("%s_pedestal_error:run_number",devices[i].Data()),Form("%s_pedestal_error<%f || %s_pedestal_error>1000000.0 || (%s_valid != 1)",devices[i].Data(),errorThreshold,devices[i].Data(),devices[i].Data()),"*same");
      g_buff = (TGraph*)pad_buff->FindObject("Graph");
      if(g_buff!=0){
        g_buff->SetName("GraphCut");
        g_buff->SetMarkerColor(kRed);
        g_buff->SetMarkerSize(1.0);
        // g_buff->SetMarkerStyle(20);
      }
      gStyle->SetMarkerColor(kBlack);
    }
    //c1->SaveAs(Form("%s/ped_fit_%s_%s_witherr_%s_%s.png",outputDir.Data(),ana.Data(),devices[i].Data(),expt.Data(),version.Data()));

    pad_buff = c1->cd(4);
    //gStyle->SetStats(1);
    gStyle->SetOptStat(112211);
    nruns = ped->Draw(Form("%s_gain",devices[i].Data()),Form("%s && %s_valid",cut.Data(),devices[i].Data()));
    if (nruns > 0) {
      gain = ((TH1D*)gROOT->FindObject("htemp"))->GetMean();
      ((TH1D*)gROOT->FindObject("htemp"))->SetNameTitle("Gains",Form("%s_gain",devices[i].Data()));
    }
    else {
      gain = 0.0;
    }

    pad_buff = c1->cd(5);
    if (ana == "unser") {
      nruns = ped->Draw(Form("(%s_pedestal-%f)/(%f):run_number",devices[i].Data(),tg_ped,tg_ped_err),Form("%s && %s_valid",cut.Data(),devices[i].Data()),"*");
      if (nruns > 0) {
        g_buff = (TGraph*)pad_buff->FindObject("Graph");
        if (g_buff!=0) {
          g_buff->SetName("GraphAll2");
        }
        ((TH1D*)gROOT->FindObject("htemp"))->SetNameTitle("htemp2",Form("%s_pedestal pull vs. fit pedestal (%0.2f), using fit mean error",devices[i].Data(),tg_ped));
      }
    }
    else if (ped2!=NULL && !(!ped2->GetListOfBranches()->FindObject(Form("%s_pedestal",devices[i].Data())))) {
      nruns = ped2->Draw(Form("%s_pedestal:%s_pedestal_error:run_number",devices[i].Data(),devices[i].Data()),Form("%s && %s_valid",cut.Data(),devices[i].Data()),"goff");
      if (nruns > 0) {
        TGraphErrors* tgbuf = new TGraphErrors(nruns, ped2->GetV3(),ped2->GetV1(),0,ped2->GetV2());
        tgbuf->SetNameTitle(Form("%s %s unser pedestal vs. run",devices[i].Data(),ana.Data()),Form("%s %s unser pedestal vs. run",devices[i].Data(),ana.Data()));
        tgbuf->Fit("pol0","QR","",0,10000);
        TF1* p0 = (TF1*)gROOT->FindObject("pol0");
        tg_unser_ped     = p0->GetParameter(0);
        tg_unser_ped_err = p0->GetParError(0);
        gStyle->SetOptFit(1);
        tgbuf->SetMarkerStyle(20);
        tgbuf->SetMarkerSize(1);
        tgbuf->Draw("AP");
      }
      else {
        tg_unser_ped     = 0.0;
        tg_unser_ped_err = 0.0;
      }

      nruns = ped->Draw(Form("(%s_pedestal-%f)/(%s_pedestal_error):run_number",devices[i].Data(),tg_unser_ped,devices[i].Data()),Form("%s && %s_valid",cut.Data(),devices[i].Data()),"*");
      if (nruns > 0) {
        g_buff = (TGraph*)pad_buff->FindObject("Graph");
        if (g_buff!=0) {
          g_buff->SetName("GraphAll2");
        }
        ((TH1D*)gROOT->FindObject("htemp"))->SetNameTitle("htemp2",Form("%s_pedestal pull vs. global unser ped (%0.2f) fit, using each point's error",devices[i].Data(),tg_unser_ped));
      }
    }
    else {
      pad_buff->Clear();
    }

    pad_buff = c1->cd(6);
    nruns = ped->Draw(Form("(%s_pedestal-%f)/(%s_pedestal_error):run_number",devices[i].Data(),tg_ped,devices[i].Data()),Form("%s && %s_valid",cut.Data(),devices[i].Data()),"*");
    if (nruns > 0) {
      g_buff = (TGraph*)pad_buff->FindObject("Graph");
      if (g_buff!=0) {
        g_buff->SetName("GraphAll3");
      }
      ((TH1D*)gROOT->FindObject("htemp"))->SetNameTitle("htemp3",Form("%s_pedestal pull vs. fit pedestal (%0.2f), using each point's error",devices[i].Data(),tg_ped));
    }

    c1->SaveAs(Form("%s/ped_fit_%s_%s_witherr_%s_%s.png",outputDir.Data(),ana.Data(),devices[i].Data(),expt.Data(),version.Data()));

    fprintf(pFile, "%s,%.2f,%.2f,%.2f,%.2f,%f\n",
        devices[i].Data(), tg_ped, tg_ped_err, th_ped, th_ped_err, gain);
    printf("%s,%.2f,%.2f,%.2f,%.2f,%f\n",
        devices[i].Data(), tg_ped, tg_ped_err, th_ped, th_ped_err, gain);
  } // End of BCMs loop
  gSystem->Exec(Form("convert $(ls -rt %s/ped_fit_%s_*_witherr_%s_%s.png) %s/ped_fit_%s_witherr_%s_%s_%s.pdf",outputDir.Data(),ana.Data(),expt.Data(),version.Data(),outputDir.Data(),ana.Data(),expt.Data(),version.Data(),type.Data()));
  gSystem->Exec(Form("rm %s/ped_fit_%s_*_witherr_%s_%s.png",outputDir.Data(),ana.Data(),expt.Data(),version.Data()));
  fclose(pFile);

}

void addMainDet_dit(TString fnm, TString foutname = "test.root", TString tree = "dit", TString bpms = "evMons") {
  TFile *fin = TFile::Open(fnm.Data(),"READ");
  TTree *T=(TTree*)fin->Get(tree);
  TFile *fout = TFile::Open(foutname,"RECREATE");
  if (!T) {Printf("No tree %s",tree.Data());}
  TTree *out = (TTree*)(T->CloneTree(0));

  // Generate us_avg and us_dd (l+r)/2 and (l-r)/2 redefined slopes and sensitivities (all 7 coils)

  std::vector<TString> dets   = {"usl","usr","us_avg","us_dd"};
  std::vector<TString> monitors;
  if (bpms == "bpms") {
    monitors  = {"1X","4aY","4eX","4eY","12X"};
  }
  if (bpms == "evMons") {
    monitors  = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  }
  if (bpms == "12evMons") {
    monitors  = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  }
  std::vector<TString> coils  = {"coil1","coil2","coil3","coil4","coil5","coil6","coil7"};
  std::vector<Double_t> usl_slopes(monitors.size());
  std::vector<Double_t> usl_sens(7);
  std::vector<Double_t> usl_sens_err(7);
  std::vector<Double_t> usr_slopes(monitors.size());
  std::vector<Double_t> usr_sens(7);
  std::vector<Double_t> usr_sens_err(7);
  std::vector<Double_t> us_avg_slopes(monitors.size());
  std::vector<Double_t> us_avg_sens(7);
  std::vector<Double_t> us_avg_sens_err(7);
  std::vector<Double_t> us_dd_slopes(monitors.size());
  std::vector<Double_t> us_dd_sens(7);
  std::vector<Double_t> us_dd_sens_err(7);

  for (Int_t i = 0 ; i < monitors.size() ; i++) {
    Printf("Set branch address %s_%s",dets.at(0).Data(),monitors.at(i).Data());
    T->SetBranchAddress(Form("%s_%s",dets.at(0).Data(),monitors.at(i).Data()), &usl_slopes.at(i));
    T->SetBranchAddress(Form("%s_%s",dets.at(1).Data(),monitors.at(i).Data()), &usr_slopes.at(i));
    out->Branch(Form("%s_%s",dets.at(2).Data(),monitors.at(i).Data()), &us_avg_slopes.at(i));
    out->Branch(Form("%s_%s",dets.at(3).Data(),monitors.at(i).Data()), &us_dd_slopes.at(i));
  }
  for (Int_t i = 0 ; i < 7 ; i++) {
    T->SetBranchAddress(Form("%s_%s",dets.at(0).Data(),coils.at(i).Data()), &usl_sens.at(i));
    T->SetBranchAddress(Form("%s_%s_err",dets.at(0).Data(),coils.at(i).Data()), &usl_sens_err.at(i));
    T->SetBranchAddress(Form("%s_%s",dets.at(1).Data(),coils.at(i).Data()), &usr_sens.at(i));
    T->SetBranchAddress(Form("%s_%s_err",dets.at(1).Data(),coils.at(i).Data()), &usr_sens_err.at(i));
    out->Branch(Form("%s_%s",dets.at(2).Data(),coils.at(i).Data()), &us_avg_sens.at(i));
    out->Branch(Form("%s_%s_err",dets.at(2).Data(),coils.at(i).Data()), &us_avg_sens_err.at(i));
    out->Branch(Form("%s_%s",dets.at(3).Data(),coils.at(i).Data()), &us_dd_sens.at(i));
    out->Branch(Form("%s_%s_err",dets.at(3).Data(),coils.at(i).Data()), &us_dd_sens_err.at(i));
  }

  Double_t rcdb_arm_flag = 0.0;
  T->SetBranchAddress(Form("rcdb_arm_flag"), &rcdb_arm_flag);

  for (auto i=0;i <T->GetEntries();i++){
    T->GetEntry(i);
    for (Int_t i = 0 ; i < monitors.size() ; i++) {
      us_avg_slopes.at(i) = (usl_slopes.at(i) + usr_slopes.at(i))/2;
      us_dd_slopes.at(i)  = (usl_slopes.at(i) - usr_slopes.at(i))/2;
    }
    for (Int_t i = 0 ; i < 7 ; i++) {
      us_avg_sens.at(i) = (usl_sens.at(i) + usr_sens.at(i))/2;
      us_dd_sens.at(i)  = (usl_sens.at(i) - usr_sens.at(i))/2;

      us_avg_sens_err.at(i) = sqrt(pow(usl_sens_err.at(i),2) + pow(usr_sens_err.at(i),2))/2;
      us_dd_sens_err.at(i)  = sqrt(pow(usl_sens_err.at(i),2) + pow(usr_sens_err.at(i),2))/2;
    }
    out->Fill();
  }

  out->Write(tree, TObject::kOverwrite);
  fin->Close();
  fout->Close();
}

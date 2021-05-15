void plotErrors_all(){ //std::string input = "NULL",std::string runNum = "", std::string slugNum = "0"){}
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* agg = new TChain("agg");
  TChain* mini = new TChain("mini");
  TChain* mini_eigen_reg_allbpms    = new TChain("mini_eigen_reg_allbpms");
  TChain* mini_eigen_reg_allbpms_tr = new TChain("mini_eigen_reg_allbpms_tr");
  TChain* mini_eigen_reg_5bpms      = new TChain("mini_eigen_reg_5bpms");
  TChain* mini_eigen_reg_5bpms_tr   = new TChain("mini_eigen_reg_5bpms_tr");
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  //agg->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/aggRootfiles/ErrorFlag/slugRootfiles/CREX-All-miniruns.root");
  mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  mini_eigen_reg_allbpms->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  mini_eigen_reg_allbpms_tr->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  mini_eigen_reg_5bpms->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  mini_eigen_reg_5bpms_tr->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  //agg->BuildIndex("agg.run_number","agg.minirun_n");
  mini->BuildIndex("run","mini");
  mini->AddFriend(mini_eigen_reg_allbpms,"mini_eigen_reg_allbpms");
  mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
  mini->AddFriend(mini_eigen_reg_5bpms,"mini_eigen_reg_5bpms");
  mini->AddFriend(mini_eigen_reg_5bpms_tr,"mini_eigen_reg_5bpms_tr");
  //mini->AddFriend(agg,"agg");

  std::vector<std::string> X_BPMs = {
    "bpm4aX",
    "bpm4eX",
    "bpm1X",
    "bpm16X",
  };
  std::vector<std::string> Y_BPMs = {
    "bpm4aY",
    "bpm4eY",
    "bpm1Y",
    "bpm16Y",
  };
  std::vector<std::string> E_BPMs = {
    "bpm11X",
    "bpm12X",
  };
  std::vector<std::string> devices = {
    "bpm4aX",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm1X",
    "bpm1Y",
    "bpm11X",
    "bpm11Y",
    "bpm12X",
    "bpm12Y",
    "bpm16X",
    "bpm16Y"
  };
  std::vector<std::string> monitors = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  std::vector<std::string> devices5 = {
    "bpm1X",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm12X"
  };
  std::vector<std::string> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};

  TGraphErrors* tge1;
  TGraphErrors* tge2;
  TGraphErrors* tge3;

  std::string cut = "run!=7760 && mini_eigen_reg_allbpms.reg_asym_us_avg.rms/ppm < 1000 && mini_eigen_reg_allbpms.reg_asym_us_avg.rms/ppm > 675";
  // Do some Tree->SetAlias() around everywhere

  std::string pdfname = Form("test_plotErrors_allBPMs.pdf");

  std::string ana = "Eigen BPMs combos";
  TText *label = new TText(0.0,0.005,ana.c_str());
  label->SetTextFont(23);
  label->SetTextSize(12);
  label->SetNDC();

  TVirtualPad* p1;

  TCanvas* c1 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1->cd();
  c1->SetTitle(ana.c_str());
  c1->SetName(ana.c_str());
  c1->Divide(3,12);
  cut = "run!=7760 && mini_eigen_reg_allbpms.reg_asym_us_avg.rms/ppm < 1000 && mini_eigen_reg_allbpms.reg_asym_us_avg.rms/ppm > 675";
  for (Int_t j = 0 ; j < monitors.size() ; j++) {
    p1=c1->cd(3*j+1);
    mini->Draw(Form("mini_eigen_reg_allbpms.diff_%s.mean/nm:run+0.1*mini",monitors.at(j).c_str()),cut.c_str());
    //((TH1*)p1->FindObject("htemp"))->SetTitle(Form("mini_eigen_reg_allbpms.diff_%s.mean/nm;run+0.1*mini;mini_eigen_reg_allbpms.diff_%s.mean/nm",monitors.at(j).c_str(),monitors.at(j).c_str()));
    //((TH1*)p1->FindObject("htemp"))->Draw();
    p1=c1->cd(3*j+2);
    mini->Draw(Form("mini_eigen_reg_allbpms.diff_%s.rms/um:run+0.1*mini",monitors.at(j).c_str()),cut.c_str());
    //((TH1*)p1->FindObject("htemp"))->SetTitle(Form("mini_eigen_reg_allbpms.diff_%s.rms/um;run+0.1*mini;mini_eigen_reg_allbpms.diff_%s.rms/um",monitors.at(j).c_str(),monitors.at(j).c_str()));
    //((TH1*)p1->FindObject("htemp"))->Draw();
    p1=c1->cd(3*j+3);
    int tge1ne = mini->Draw(Form("run+0.1*mini:mini_eigen_reg_allbpms.diff_%s.mean/nm:mini_eigen_reg_allbpms.diff_%s.err/nm",monitors.at(j).c_str(),monitors.at(j).c_str()),cut.c_str(),"goff");
    tge1 = new TGraphErrors(tge1ne,mini->GetV1(),mini->GetV2(),0,mini->GetV3());
    tge1->SetNameTitle(Form("mini_eigen_reg_allbpms.diff_%s",monitors.at(j).c_str()),Form("mini_eigen_reg_allbpms.diff_%s/nm;run+0.1*mini;mini_eigen_reg_allbpms.diff_%s/nm",monitors.at(j).c_str(),monitors.at(j).c_str()));
    tge1->Fit("pol0");
    gStyle->SetOptFit(1);
    tge1->Draw("alp");
  }

  c1->cd();
  ana = "CREX Eigen Vector Monitor's values";
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c1->SaveAs(Form("%s(",pdfname.c_str()));

  TCanvas* c2 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2->cd();
  c2->SetTitle(ana.c_str());
  c2->SetName(ana.c_str());
  c2->Divide(3,12);
  cut = "run!=7760 && mini_eigen_reg_allbpms.reg_asym_us_avg.rms/ppm < 1000 && mini_eigen_reg_allbpms.reg_asym_us_avg.rms/ppm > 675";
  for (Int_t j = 0 ; j < monitors.size() ; j++) {
    p1=c2->cd(3*j+1);
    mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s*mini_eigen_reg_allbpms.diff_%s.mean/ppb:run+0.1*mini",monitors.at(j).c_str(),monitors.at(j).c_str()),cut.c_str());
    //((TH1*)p1->FindObject("htemp"))->SetTitle(Form("mini_eigen_reg_allbpms.diff_%s correction /ppb;run+0.1*mini;mini_eigen_reg_allbpms.diff_%s correction /ppb",monitors.at(j).c_str(),monitors.at(j).c_str()));
    //((TH1*)p1->FindObject("htemp"))->Draw();
    p1=c2->cd(3*j+2);
    mini->Draw(Form("abs(mini_eigen_reg_allbpms.us_avg_%s)*mini_eigen_reg_allbpms.diff_%s.rms/ppm:run+0.1*mini",monitors.at(j).c_str(),monitors.at(j).c_str()),cut.c_str());
    //((TH1*)p1->FindObject("htemp"))->SetTitle(Form("mini_eigen_reg_allbpms.diff_%s correction rms /ppm;run+0.1*mini;mini_eigen_reg_allbpms.diff_%s correction rms /ppm",monitors.at(j).c_str(),monitors.at(j).c_str()));
    //((TH1*)p1->FindObject("htemp"))->Draw();
    p1=c2->cd(3*j+3);
    int tge2ne = mini->Draw(Form("run+0.1*mini:mini_eigen_reg_allbpms.us_avg_%s*mini_eigen_reg_allbpms.diff_%s.mean/ppb:mini_eigen_reg_allbpms.us_avg_%s*mini_eigen_reg_allbpms.diff_%s.err/ppb",monitors.at(j).c_str(),monitors.at(j).c_str(),monitors.at(j).c_str(),monitors.at(j).c_str()),cut.c_str(),"goff");
    tge2 = new TGraphErrors(tge2ne,mini->GetV1(),mini->GetV2(),0,mini->GetV3());
    tge2->SetNameTitle(Form("mini_eigen_reg_allbpms.diff_%s us_avg correction (ppb)",monitors.at(j).c_str()),Form("mini_eigen_reg_allbpms.diff_%s us_avg slope;run+0.1*mini;mini_eigen_reg_allbpms.diff_%s us_avg correction (ppb)",monitors.at(j).c_str(),monitors.at(j).c_str()));
    tge2->Fit("pol0");
    gStyle->SetOptFit(1);
    tge2->Draw("alp");
  }

  c2->cd();
  ana = "CREX Eigen Vector Monitor's slope for us_avg value (correction factors)";
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c2->SaveAs(Form("%s",pdfname.c_str()));

  TCanvas* c3 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c3->cd();
  c3->SetTitle(ana.c_str());
  c3->SetName(ana.c_str());
  c3->Divide(1,3);
  cut = "run!=7760 && mini_eigen_reg_allbpms.reg_asym_us_avg.rms/ppm < 1000 && mini_eigen_reg_allbpms.reg_asym_us_avg.rms/ppm > 675";
  p1=c3->cd(1);
  mini->Draw(Form("mini_eigen_reg_allbpms.reg_asym_us_avg.mean/ppb:run+0.1*mini"),cut.c_str());
  p1=c3->cd(2);
  mini->Draw(Form("mini_eigen_reg_allbpms.reg_asym_us_avg.rms/ppm:run+0.1*mini"),cut.c_str());
  p1=c3->cd(3);
  int tge3ne = mini->Draw(Form("run+0.1*mini:mini_eigen_reg_allbpms.reg_asym_us_avg.mean/ppb:mini_eigen_reg_allbpms.reg_asym_us_avg.err/ppb"),cut.c_str(),"goff");
  tge3 = new TGraphErrors(tge3ne,mini->GetV1(),mini->GetV2(),0,mini->GetV3());
  tge3->SetNameTitle(Form("mini_eigen_reg_allbpms.reg_asym_us_avg result"),Form("mini_eigen_reg_allbpms.reg_asym_us_avg result;run+0.1*mini;mini_eigen_reg_allbpms.reg_asym_us_avg.mean/ppb (err/ppb)"));
  tge3->Fit("pol0");
  gStyle->SetOptFit(1);
  tge3->Draw("alp");
  c3->cd();
  ana = "CREX us_avg from Eigen Vector regression";
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");

  c3->SaveAs(Form("%s)",pdfname.c_str()));
}

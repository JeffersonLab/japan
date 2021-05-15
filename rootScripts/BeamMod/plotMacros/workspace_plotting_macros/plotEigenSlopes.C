TVirtualPad* vs_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::string cut = "", std::vector<std::string> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    if (cut == "") {
      nen = mini->Draw(Form("(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).c_str()),Form("run!=7760"),"goff");
    }
    else {
      // No more abs cut here...
      nen = mini->Draw(Form("(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).c_str()),Form("run!=7760 && %s",cut.c_str()),"goff");
      //nen = mini->Draw(Form("abs(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).c_str()),Form("run!=7760 && %s",cut.c_str()),"goff");
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d",vs.at(i).c_str(),draw.c_str(),subpad),Form("%s_%s_%d",vs.at(i).c_str(),draw.c_str(),subpad));
      //if (cut == "") {
        tmpG->SetLineColor(startcolor+2*i);
        tmpG->SetMarkerColor(startcolor+2*i);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.01);
      mg->Add(tmpG,"lp");
      legend->AddEntry(tmpG,(TString)vs.at(i),"lp");
    }
  }

  mg->SetNameTitle(Form("%s slopes",draw.c_str()),Form("%s slopes;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  if (subpad%3==0 || cut=="rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0") {
    legend->Draw();
  }
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}


TVirtualPad* vs_ranked_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::string cut = "", std::vector<std::string> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  std::string localCut = cut;

  Int_t nen=0;
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    localCut = cut;
    if (localCut == "") {
      localCut = Form("(run>5000 && run != 7760)");
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s%s_%s) > abs(%s%s_%s))",localCut.c_str(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).c_str(),draw.substr(draw.find(".")+1,draw.length()).c_str(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(k).c_str(),draw.substr(draw.find(".")+1,draw.length()).c_str());
        }
      }
      Printf("%s",localCut.c_str());
      nen = mini->Draw(Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).c_str(),draw.substr(draw.find(".")+1,draw.length()).c_str()),Form("%s",localCut.c_str()),"goff");
      Printf("%s",Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).c_str(),draw.substr(draw.find(".")+1,draw.length()).c_str()));
    }
    else {
      localCut = Form("(%s)",localCut.c_str());
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s%s_%s) > abs(%s%s_%s))",localCut.c_str(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).c_str(),draw.substr(draw.find(".")+1,draw.length()).c_str(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(k).c_str(),draw.substr(draw.find(".")+1,draw.length()).c_str());
        }
      }
      Printf("%s",localCut.c_str());
      nen = mini->Draw(Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).c_str(),draw.substr(draw.find(".")+1,draw.length()).c_str()),Form("%s",localCut.c_str()),"goff");
      // No more absolute values in here
      //nen = mini->Draw(Form("abs(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).c_str(),draw.substr(draw.find(".")+1,draw.length()).c_str()),Form("%s",localCut.c_str()),"goff");
      Printf("%s",Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).c_str(),draw.substr(draw.find(".")+1,draw.length()).c_str()));
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d",vs.at(i).c_str(),draw.c_str(),subpad),Form("%s_%s_%d",vs.at(i).c_str(),draw.c_str(),subpad));
      //if (localCut == "") {
        tmpG->SetLineColor(startcolor+2*i);
        tmpG->SetMarkerColor(startcolor+2*i);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.2);
      //tmpG->SetMarkerSize(0.01);
      mg->Add(tmpG,"p");
      legend->AddEntry(tmpG,(TString)vs.at(i),"p");
    }
  }

  mg->SetNameTitle(Form("%s BPM combo's ranked largest contribution",draw.c_str()),Form("%s BPM combo's ranked largest contribution;Run+0.1*Minirun;%s",draw.c_str(),draw.c_str()));
  mg->Draw("ap");

  //if (subpad%3==0 || localCut=="") {
  legend->Draw();
  //}
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}


TVirtualPad* vs_ranked_weighted_slope_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string tree = "mini", std::string detector = "us_avg", std::string device = "bpm12X", std::vector<std::string> vs = {""}, std::string cut = "", Int_t startcolor=30){ 
  // Ana == 
  // slope - calculate the amount of BPM Q in detector P, find the evMon that has the most Q in P and plot it
  // err   - calculate the uncertainty on correction to detector from evMonitor
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  std::string localCut = cut;

  Int_t nen=0;
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    localCut = cut;
    if (localCut == "") {
      localCut = Form("(run>5000 && run != 7760)");
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s.%s_%s*%s.%s_%s) > abs(%s.%s_%s*%s.%s_%s))",localCut.c_str(),tree.c_str(),detector.c_str(),vs.at(i).c_str(),tree.c_str(),vs.at(i).c_str(),device.c_str(),tree.c_str(),detector.c_str(),vs.at(k).c_str(),tree.c_str(),vs.at(k).c_str(),device.c_str());
        }
      }
      Printf("%s",localCut.c_str());
      nen = mini->Draw(Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.c_str(),detector.c_str(),vs.at(i).c_str(),tree.c_str(),vs.at(i).c_str(),device.c_str()),Form("%s",localCut.c_str()),"goff");
      Printf("%s",Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.c_str(),detector.c_str(),vs.at(i).c_str(),tree.c_str(),vs.at(i).c_str(),device.c_str()));
    }
    else {
      localCut = Form("(%s)",localCut.c_str());
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s.%s_%s*%s.%s_%s) > abs(%s.%s_%s*%s.%s_%s))",localCut.c_str(),tree.c_str(),detector.c_str(),vs.at(i).c_str(),tree.c_str(),vs.at(i).c_str(),device.c_str(),tree.c_str(),detector.c_str(),vs.at(k).c_str(),tree.c_str(),vs.at(k).c_str(),device.c_str());
        }
      }
      Printf("%s",localCut.c_str());
      nen = mini->Draw(Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.c_str(),detector.c_str(),vs.at(i).c_str(),tree.c_str(),vs.at(i).c_str(),device.c_str()),Form("%s",localCut.c_str()),"goff");
      // No more absolute values 
      // nen = mini->Draw(Form("abs(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.c_str(),detector.c_str(),vs.at(i).c_str(),tree.c_str(),vs.at(i).c_str(),device.c_str()),Form("%s",localCut.c_str()),"goff");
      Printf("%s",Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.c_str(),detector.c_str(),vs.at(i).c_str(),tree.c_str(),vs.at(i).c_str(),device.c_str()));
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s.%s_%s_%d",tree.c_str(),vs.at(i).c_str(),device.c_str(),subpad),Form("%s.%s_%s_%d",tree.c_str(),vs.at(i).c_str(),device.c_str(),subpad));
      //if (localCut == "") {
        tmpG->SetLineColor(startcolor+2*i);
        tmpG->SetMarkerColor(startcolor+2*i);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.2);
      //tmpG->SetMarkerSize(0.01);
      mg->Add(tmpG,"p");
      legend->AddEntry(tmpG,Form("%s_%s",detector.c_str(),vs.at(i).c_str()),"p");
    }
  }

  mg->SetNameTitle(Form("%s's ranked EigVecMon with largest amount of %s BPM slope;Run+0.1*Minirun;%s",detector.c_str(),device.c_str(),device.c_str()),Form("%s's ranked EigVecMon with largest amount of %s BPM slope;Run+0.1*Minirun;%s_monitor_%s",detector.c_str(),device.c_str(),detector.c_str(),device.c_str()));
  mg->Draw("ap");

  //if (subpad%3==0 || localCut=="") {
  legend->Draw();
  //}
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* highlightedGraph(TCanvas* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "run:cyclenum", std::string cut = "", std::string cut2 = "0",std::string drawopts = "*", int color = 4, std::string cut3 = "0", int color3=2, std::string cut4 = "") {
  if (cut4 != "") {
    cut = cut+"&&"+cut4;
    cut2 = cut2+"&&"+cut4;
    if (cut3!= "") {
      cut3 = cut3+"&&"+cut4;
    }
  }

  p1=c1->cd(subpad);
  p1->SetGrid();
  mini->Draw(draw.c_str(),cut.c_str(),drawopts.c_str());

  if (p1->FindObject("Graph")) {
    ((TGraph*)p1->FindObject("Graph"))->SetName("full");
    mini->Draw(draw.c_str(),Form("%s",cut2.c_str()),Form("%ssame",drawopts.c_str()));
    if (p1->FindObject("Graph")) {
      ((TGraph*)p1->FindObject("Graph"))->SetName("color");
      ((TGraph*)p1->FindObject("color"))->SetMarkerColor(color);
    }
    if (cut3 != "") {
      mini->Draw(draw.c_str(),Form("%s",cut3.c_str()),Form("%ssame",drawopts.c_str()));
      if (p1->FindObject("Graph")) {
        ((TGraph*)p1->FindObject("Graph"))->SetName("color3");
        ((TGraph*)p1->FindObject("color3"))->SetMarkerColor(color3);
      }
      mini->Draw(draw.c_str(),Form("(%s) && (%s)",cut2.c_str(),cut3.c_str()),Form("%ssame",drawopts.c_str()));
      if (p1->FindObject("Graph")) {
        ((TGraph*)p1->FindObject("Graph"))->SetName("color2and3");
        ((TGraph*)p1->FindObject("color2and3"))->SetMarkerColor(color+color3);
      }
    }
  }
  if (p1->FindObject("htemp")) {
    ((TH1*)p1->FindObject("htemp"))->SetTitle(draw.c_str());
  }
  return p1;
}


/*
TFile *_file0 = TFile::Open("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root")
mini->AddFriend(mini_eigen_reg_allbpms)
mini->AddFriend(mini_eigen_reg_5bpms)
 mini->Scan("run:mini:abs(mini_eigen_reg_5bpms.us_avg_evMon0/(ppm/um)):abs(mini_eigen_reg_5bpms.usl_evMon0/(ppm/um)):abs(mini_eigen_reg_5bpms.usl_evMon1/(ppm/um)):abs(mini_eigen_reg_5bpms.usl_evMon2/(ppm/um)):abs(mini_eigen_reg_5bpms.usl_evMon3/(ppm/um)):abs(mini_eigen_reg_5bpms.usl_evMon4/(ppm/um))")
mini->Scan("run:mini:mini_eigen_reg_5bpms.evMon0_bpm1X")
mini->Scan("run:mini:mini_eigen_reg_allbpms.evMon0_bpm1X")
mini->Scan("run:mini:mini_eigen_reg_5bpms.evMon0_bpm1X")
mini->Scan("run:mini:mini_eigen_reg_5bpms.evMon0_bpm1X:mini_eigen_reg_5bpms.evMon0_bpm4aY:mini_eigen_reg_5bpms.evMon0_bpm4eX:mini_eigen_reg_5bpms.evMon0_bpm4eY:mini_eigen_reg_5bpms.evMon0_bpm12X")
mini->Scan("run:mini:mini_eigen_reg_5bpms.evMon1_bpm1X:mini_eigen_reg_5bpms.evMon1_bpm4aY:mini_eigen_reg_5bpms.evMon1_bpm4eX:mini_eigen_reg_5bpms.evMon1_bpm4eY:mini_eigen_reg_5bpms.evMon1_bpm12X")
*/

void plotEigenSlopes(){ //std::string input = "NULL",std::string runNum = "", std::string slugNum = "0"){}
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* mini = new TChain("mini");
  TChain* mini_eigen_reg_allbpms    = new TChain("mini_eigen_reg_allbpms_sorted"); // Unsorted versions have no _sorted in the name
  TChain* mini_eigen_reg_allbpms_tr = new TChain("mini_eigen_reg_allbpms_tr");     // Truncated one is not sorted... just deal with it I guess (could sort it later if urgently needed)
  TChain* mini_eigen_reg_5bpms      = new TChain("mini_eigen_reg_5bpms_sorted");
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_eigenvectors_sorted.root");
  mini_eigen_reg_allbpms->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_eigenvectors_sorted.root");
  mini_eigen_reg_allbpms_tr->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_eigenvectors_sorted.root");
  mini_eigen_reg_5bpms->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_eigenvectors_sorted.root");
  // Default outputs, unsorted:
  //mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  mini->AddFriend(mini_eigen_reg_allbpms,"mini_eigen_reg_allbpms");
  mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
  mini->AddFriend(mini_eigen_reg_5bpms,"mini_eigen_reg_5bpms");
  mini->BuildIndex("run","mini");

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
  std::vector<std::string> monitors1(monitors.begin()+0,monitors.begin()+3);
  std::vector<std::string> monitors2(monitors.begin()+3,monitors.begin()+6);
  std::vector<std::string> monitors3(monitors.begin()+6,monitors.begin()+9);
  std::vector<std::string> monitors4(monitors.begin()+9,monitors.begin()+12);
  std::vector<std::string> devices5 = {
    "bpm1X",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm12X"
  };
  std::vector<std::string> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  std::vector<std::string> monitors5_1 = {"evMon0"};
  std::vector<std::string> monitors5_2 = {"evMon1"};
  std::vector<std::string> monitors5_3 = {"evMon2"};
  std::vector<std::string> monitors5_4 = {"evMon3"};
  std::vector<std::string> monitors5_5 = {"evMon4"};
  std::vector<std::vector<std::string>> monitors5_all = {monitors5_1,monitors5_2,monitors5_3,monitors5_4,monitors5_5};


  std::string cut = "";
  // Do some Tree->SetAlias() around everywhere

  std::string pdfname = Form("test_EigenSlopes.pdf");

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
  c1->Divide(3,6);
  cut = "run<=6332";
  for (Int_t j = 0 ; j < monitors5_all.size() ; j++) {
    cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1 && run<=6332";
    p1=vs_multiGraph(c1,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms.usl"),cut,monitors5_all.at(j),36+j);
    //p1=vs_multiGraph(c1,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms.usl_%s*mini_eigen_reg_5bpms.%s",monitors5.at(j).c_str(),monitors5.at(j).c_str()),cut,devices5,36);
    cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2 && run<=6332";
    p1=vs_multiGraph(c1,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms.usr"),cut,monitors5_all.at(j),36+j);
    cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0 && run<=6332";
    p1=vs_multiGraph(c1,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms.us_avg"),cut,monitors5_all.at(j),36+j);
  }
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1 && run<=6332";
  p1=vs_multiGraph(c1,p1,mini,16,Form("mini_eigen_reg_5bpms.usl"),cut,monitors5,36);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2 && run<=6332";
  p1=vs_multiGraph(c1,p1,mini,17,Form("mini_eigen_reg_5bpms.usr"),cut,monitors5,36);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0 && run<=6332";
  p1=vs_multiGraph(c1,p1,mini,18,Form("mini_eigen_reg_5bpms.us_avg"),cut,monitors5,36);

  c1->cd();
  ana = "CREX Part 1 Eigen Vector Monitor BPMs slopes"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c1->SaveAs(Form("%s(",pdfname.c_str()));
  c1->SaveAs(Form("5BPM_eigen_slopes.pdf("));

  TCanvas* c2 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2->cd();
  c2->SetTitle(ana.c_str());
  c2->SetName(ana.c_str());
  c2->Divide(3,6);
  cut = "run>6332 && run<7500";
  for (Int_t j = 0 ; j < monitors5_all.size() ; j++) {
    cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1 && run>6332 && run<7500";
    p1=vs_multiGraph(c2,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms.usl"),cut,monitors5_all.at(j),36+j);
    cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2 && run>6332 && run<7500";
    p1=vs_multiGraph(c2,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms.usr"),cut,monitors5_all.at(j),36+j);
    cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0 && run>6332 && run<7500";
    p1=vs_multiGraph(c2,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms.us_avg"),cut,monitors5_all.at(j),36+j);
  }
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1 && run>6332 && run<7500";
  p1=vs_multiGraph(c2,p1,mini,16,Form("mini_eigen_reg_5bpms.usl"),cut,monitors5,36);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2 && run>6332 && run<7500";
  p1=vs_multiGraph(c2,p1,mini,17,Form("mini_eigen_reg_5bpms.usr"),cut,monitors5,36);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0 && run>6332 && run<7500";
  p1=vs_multiGraph(c2,p1,mini,18,Form("mini_eigen_reg_5bpms.us_avg"),cut,monitors5,36);

  c2->cd();
  ana = "CREX Part 2 Eigen Vector Monitor BPMs slopes"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c2->SaveAs(Form("%s",pdfname.c_str()));
  c2->SaveAs(Form("5BPM_eigen_slopes.pdf"));

  TCanvas* c3 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c3->cd();
  c3->SetTitle(ana.c_str());
  c3->SetName(ana.c_str());
  c3->Divide(3,6);
  cut = "run>7500 && run!=7760";
  for (Int_t j = 0 ; j < monitors5_all.size() ; j++) {
    cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1 && run>7500";
    p1=vs_multiGraph(c3,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms.usl"),cut,monitors5_all.at(j),36+j);
    cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2 && run>7500";
    p1=vs_multiGraph(c3,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms.usr"),cut,monitors5_all.at(j),36+j);
    cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0 && run>7500";
    p1=vs_multiGraph(c3,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms.us_avg"),cut,monitors5_all.at(j),36+j);
  }
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1 && run>7500";
  p1=vs_multiGraph(c3,p1,mini,16,Form("mini_eigen_reg_5bpms.usl"),cut,monitors5,36);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2 && run>7500";
  p1=vs_multiGraph(c3,p1,mini,17,Form("mini_eigen_reg_5bpms.usr"),cut,monitors5,36);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0 && run>7500";
  p1=vs_multiGraph(c3,p1,mini,18,Form("mini_eigen_reg_5bpms.us_avg"),cut,monitors5,36);
  c3->cd();
  ana = "CREX Part 3 Eigen Vector Monitor BPMs slopes"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c3->SaveAs(Form("%s",pdfname.c_str()));
  c3->SaveAs(Form("5BPM_eigen_slopes.pdf)"));

  TCanvas* c4 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c4->cd();
  c4->SetTitle(ana.c_str());
  c4->SetName(ana.c_str());
  c4->Divide(3,4);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0";
    //p1=vs_multiGraph(c4,p1,mini,3*j+1,Form("mini_eigen_reg_allbpms.us_avg_%s*mini_eigen_reg_allbpms.%s",monitors.at(j).c_str(),monitors.at(j).c_str()),cut,X_BPMs,36);
    //p1=vs_multiGraph(c4,p1,mini,3*j+2,Form("mini_eigen_reg_allbpms.us_avg_%s*mini_eigen_reg_allbpms.%s",monitors.at(j).c_str(),monitors.at(j).c_str()),cut,Y_BPMs,36);
    //p1=vs_multiGraph(c4,p1,mini,3*j+3,Form("mini_eigen_reg_allbpms.us_avg_%s*mini_eigen_reg_allbpms.%s",monitors.at(j).c_str(),monitors.at(j).c_str()),cut,E_BPMs,36);
  p1=vs_multiGraph(c4,p1,mini,1,Form("mini_eigen_reg_allbpms.usl"),cut,monitors1,36);
  p1=vs_multiGraph(c4,p1,mini,2,Form("mini_eigen_reg_allbpms.usr"),cut,monitors1,36);
  p1=vs_multiGraph(c4,p1,mini,3,Form("mini_eigen_reg_allbpms.us_avg"),cut,monitors1,36);
  p1=vs_multiGraph(c4,p1,mini,4,Form("mini_eigen_reg_allbpms.usl"),cut,monitors2,36);
  p1=vs_multiGraph(c4,p1,mini,5,Form("mini_eigen_reg_allbpms.usr"),cut,monitors2,36);
  p1=vs_multiGraph(c4,p1,mini,6,Form("mini_eigen_reg_allbpms.us_avg"),cut,monitors2,36);
  p1=vs_multiGraph(c4,p1,mini,7,Form("mini_eigen_reg_allbpms.usl"),cut,monitors3,36);
  p1=vs_multiGraph(c4,p1,mini,8,Form("mini_eigen_reg_allbpms.usr"),cut,monitors3,36);
  p1=vs_multiGraph(c4,p1,mini,9,Form("mini_eigen_reg_allbpms.us_avg"),cut,monitors3,36);
  p1=vs_multiGraph(c4,p1,mini,10,Form("mini_eigen_reg_allbpms.usl"),cut,monitors4,36);
  p1=vs_multiGraph(c4,p1,mini,11,Form("mini_eigen_reg_allbpms.usr"),cut,monitors4,36);
  p1=vs_multiGraph(c4,p1,mini,12,Form("mini_eigen_reg_allbpms.us_avg"),cut,monitors4,36);
  c4->cd();
  ana = "CREX All runs, sets of 3 Eigen Vector Monitors slopes";
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c4->SaveAs(Form("%s",pdfname.c_str()));
  c4->SaveAs(Form("AllBPM_eigen_slopes.pdf("));

  TCanvas* c5 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c5->cd();
  c5->SetTitle(ana.c_str());
  c5->SetName(ana.c_str());
  c5->Divide(3,4);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0";
  c5->cd(1);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(0).c_str()),cut.c_str(),"p");
  c5->cd(2);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(1).c_str()),cut.c_str(),"p");
  c5->cd(3);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(2).c_str()),cut.c_str(),"p");
  c5->cd(4);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(3).c_str()),cut.c_str(),"p");
  c5->cd(5);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(4).c_str()),cut.c_str(),"p");
  c5->cd(6);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(5).c_str()),cut.c_str(),"p");
  c5->cd(7);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(6).c_str()),cut.c_str(),"p");
  c5->cd(8);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(7).c_str()),cut.c_str(),"p");
  c5->cd(9);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(8).c_str()),cut.c_str(),"p");
  c5->cd(10);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(9).c_str()),cut.c_str(),"p");
  c5->cd(11);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(10).c_str()),cut.c_str(),"p");
  c5->cd(12);
  mini->Draw(Form("mini_eigen_reg_allbpms.us_avg_%s/(ppm/um):run+0.1*mini",monitors.at(11).c_str()),cut.c_str(),"p");
  c5->cd();
  ana = "CREX All runs, Eigen Vector Monitor BPMs us_avg slopes, evMon 0 to 11, left to right, etc.";
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c5->SaveAs(Form("%s)",pdfname.c_str()));
  c5->SaveAs(Form("AllBPM_eigen_slopes.pdf)"));

}

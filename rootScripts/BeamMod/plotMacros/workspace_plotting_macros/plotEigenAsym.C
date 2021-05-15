TVirtualPad* master_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::string cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.8,0.50,0.95);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  if (((TString)cut).Contains("rcdb_sign")) { 
    nen = mini->Draw(Form("rcdb_sign*((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.mean)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.mean)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.mean))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.c_str(),"goff");
  }
  else {
    nen = mini->Draw(Form("((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.mean)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.mean)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.mean))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.c_str(),"goff");
  }
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV3(),mini->GetV1(),0,mini->GetV2());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.c_str(),subpad),Form("%s_%d",draw.c_str(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.01);
    mg->Add(tmpG,"lp");
    legend->AddEntry(tmpG,((TString)draw)(((TString)draw).Last('_')+1,((TString)draw).Length())+" all eigen reg","lp");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s grand reg avg",draw.c_str()),Form("%s grand reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* applied_master_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::vector<std::string> monitors = {""}, std::string cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.8,0.50,0.95);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  // FIXME ADD SLOPES UNITS
  std::string us_avg_slopes = draw + ".us_avg_" + monitors.at(0) + "*" + draw + ".diff_" + monitors.at(0) + ".mean";
  std::string usl_slopes = draw + ".usl_" + monitors.at(0) + "*" + draw + ".diff_" + monitors.at(0) + ".mean";
  std::string usr_slopes = draw + ".usr_" + monitors.at(0) + "*" + draw + ".diff_" + monitors.at(0) + ".mean";
  for (Int_t i = 1 ; i < monitors.size() ; i++) {
    us_avg_slopes = us_avg_slopes + "+" + draw + ".us_avg_" + monitors.at(i) + "*" + draw + ".diff_" + monitors.at(i) + ".mean";
    usl_slopes = usl_slopes + "+" + draw + ".usl_" + monitors.at(i) + "*" + draw + ".diff_" + monitors.at(i) + ".mean";
    usr_slopes = usr_slopes + "+" + draw + ".usr_" + monitors.at(i) + "*" + draw + ".diff_" + monitors.at(i) + ".mean";
  }

  Int_t nen=0;
  if (((TString)cut).Contains("rcdb_sign")) { 
    //nen = mini->Draw(Form("rcdb_sign*((rcdb_arm_flag==0)*(mini.asym_us_avg.mean+%s)+(rcdb_arm_flag==1)*(mini.asym_usr.mean+%s)+(rcdb_arm_flag==2)*(mini.asym_usl.mean+%s))/(ppb):(rcdb_arm_flag==0)+(rcdb_arm_flag==1)+(rcdb_arm_flag==2):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str()),cut.c_str(),"goff");
    nen = mini->Draw(Form("rcdb_sign*((rcdb_arm_flag==0)*(asym_us_avg.mean-(%s))+(rcdb_arm_flag==1)*(asym_usr.mean-(%s))+(rcdb_arm_flag==2)*(asym_usl.mean-(%s)))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.c_str(),"goff");

  }
  else {
    //nen = mini->Draw(Form("((rcdb_arm_flag==0)*(mini.asym_us_avg.mean+%s)+(rcdb_arm_flag==1)*(mini.asym_usr.mean+%s)+(rcdb_arm_flag==2)*(mini.asym_usl.mean+%s))/(ppb):(rcdb_arm_flag==0)+(rcdb_arm_flag==1)+(rcdb_arm_flag==2):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str()),cut.c_str(),"goff");
    nen = mini->Draw(Form("((rcdb_arm_flag==0)*(mini.asym_us_avg.mean-(%s))+(rcdb_arm_flag==1)*(mini.asym_usr.mean-(%s))+(rcdb_arm_flag==2)*(mini.asym_usl.mean-(%s)))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.c_str(),"goff");
  }
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV3(),mini->GetV1(),0,mini->GetV2());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.c_str(),subpad),Form("%s_%d",draw.c_str(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.01);
    mg->Add(tmpG,"lp");
    legend->AddEntry(tmpG,((TString)draw)(((TString)draw).Last('_')+1,((TString)draw).Length())+" all eigen reg","lp");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s grand reg avg",draw.c_str()),Form("%s grand reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* applied_segment_master_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::vector<std::string> monitors = {""}, std::string cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.8,0.50,0.95);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  // FIXME ADD SLOPES UNITS
  std::string us_avg_slopes = draw + ".us_avg_" + monitors.at(0) + "*" + draw + ".diff_" + monitors.at(0) + ".mean";
  std::string usl_slopes = draw + ".usl_" + monitors.at(0) + "*" + draw + ".diff_" + monitors.at(0) + ".mean";
  std::string usr_slopes = draw + ".usr_" + monitors.at(0) + "*" + draw + ".diff_" + monitors.at(0) + ".mean";
  for (Int_t i = 1 ; i < monitors.size() ; i++) {
    us_avg_slopes = us_avg_slopes + "+" + draw + ".us_avg_" + monitors.at(i) + "*" + draw + ".diff_" + monitors.at(i) + ".mean";
    usl_slopes = usl_slopes + "+" + draw + ".usl_" + monitors.at(i) + "*" + draw + ".diff_" + monitors.at(i) + ".mean";
    usr_slopes = usr_slopes + "+" + draw + ".usr_" + monitors.at(i) + "*" + draw + ".diff_" + monitors.at(i) + ".mean";
  }

  Int_t nen=0;
  if (((TString)cut).Contains("rcdb_sign")) { 
    //nen = mini->Draw(Form("rcdb_sign*((rcdb_arm_flag==0)*(mini.asym_us_avg.mean+%s)+(rcdb_arm_flag==1)*(mini.asym_usr.mean+%s)+(rcdb_arm_flag==2)*(mini.asym_usl.mean+%s))/(ppb):(rcdb_arm_flag==0)+(rcdb_arm_flag==1)+(rcdb_arm_flag==2):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str()),cut.c_str(),"goff");
    nen = mini->Draw(Form("rcdb_sign*((rcdb_arm_flag==0)*(asym_us_avg.mean-(%s))+(rcdb_arm_flag==1)*(asym_usr.mean-(%s))+(rcdb_arm_flag==2)*(asym_usl.mean-(%s)))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.c_str(),"goff");

  }
  else {
    //nen = mini->Draw(Form("((rcdb_arm_flag==0)*(mini.asym_us_avg.mean+%s)+(rcdb_arm_flag==1)*(mini.asym_usr.mean+%s)+(rcdb_arm_flag==2)*(mini.asym_usl.mean+%s))/(ppb):(rcdb_arm_flag==0)+(rcdb_arm_flag==1)+(rcdb_arm_flag==2):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str()),cut.c_str(),"goff");
    nen = mini->Draw(Form("((rcdb_arm_flag==0)*(mini.asym_us_avg.mean-(%s))+(rcdb_arm_flag==1)*(mini.asym_usr.mean-(%s))+(rcdb_arm_flag==2)*(mini.asym_usl.mean-(%s)))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.c_str(),"goff");
  }
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV3(),mini->GetV1(),0,mini->GetV2());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.c_str(),subpad),Form("%s_%d",draw.c_str(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.01);
    mg->Add(tmpG,"lp");
    legend->AddEntry(tmpG,((TString)draw)(((TString)draw).Last('_')+1,((TString)draw).Length())+" all eigen reg","lp");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s grand reg avg",draw.c_str()),Form("%s grand reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* corrected_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::string slope = "test", std::string cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.75,0.50,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  if (((TString)cut).Contains("rcdb_sign")) { 
    nen = mini->Draw(Form("rcdb_sign*((%s.mean/nm)*(%s/(ppm/um))):((%s.err/nm)*(%s/(ppm/um))):run+0.1*mini",draw.c_str(),slope.c_str(),draw.c_str(),slope.c_str()),cut.c_str(),"goff");
  }
  else {
    nen = mini->Draw(Form("((%s.mean/nm)*(%s/(ppm/um))):((%s.err/nm)*(%s/(ppm/um))):run+0.1*mini",draw.c_str(),slope.c_str(),draw.c_str(),slope.c_str()),cut.c_str(),"goff");
  }
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV3(),mini->GetV1(),0,mini->GetV2());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.c_str(),subpad),Form("%s_%d",draw.c_str(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.01);
    mg->Add(tmpG,"lp");
    legend->AddEntry(tmpG,((TString)draw)(((TString)draw).First(".")+1,((TString)draw).Length())+" * "+((TString)slope)(((TString)slope).First(".")+1,((TString)slope).Length()),"lp");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s reg avg",draw.c_str()),Form("%s reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::string cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  if (((TString)cut).Contains("rcdb_sign")) { 
    nen = mini->Draw(Form("rcdb_sign*(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.c_str(),draw.c_str()),cut.c_str(),"goff");
  }
  else {
    nen = mini->Draw(Form("(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.c_str(),draw.c_str()),cut.c_str(),"goff");
  }
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV3(),mini->GetV1(),0,mini->GetV2());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.c_str(),subpad),Form("%s_%d",draw.c_str(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.01);
    mg->Add(tmpG,"lp");
    legend->AddEntry(tmpG,(TString)draw,"lp");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s reg avg",draw.c_str()),Form("%s reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

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

void plotEigenAsym(){ //std::string input = "NULL",std::string runNum = "", std::string slugNum = "0"){}
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* mini = new TChain("mini");
  TChain* mini_eigen_reg_allbpms           = new TChain("mini_eigen_reg_allbpms");
  TChain* mini_eigen_reg_allbpms_sorted    = new TChain("mini_eigen_reg_allbpms_sorted"); // Unsorted versions have no _sorted in the name
  TChain* mini_eigen_reg_allbpms_tr        = new TChain("mini_eigen_reg_allbpms_tr");     // Truncated one is not sorted... just deal with it I guess (could sort it later if urgently needed)
  TChain* mini_eigen_reg_5bpms             = new TChain("mini_eigen_reg_5bpms");
  TChain* mini_eigen_reg_5bpms_sorted      = new TChain("mini_eigen_reg_5bpms_sorted");
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles/rcdb_segment_eigenvectors_sorted.root");
  mini_eigen_reg_allbpms->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles/rcdb_segment_eigenvectors_sorted.root");
  mini_eigen_reg_allbpms_sorted->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles/rcdb_segment_eigenvectors_sorted.root");
  mini_eigen_reg_allbpms_tr->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles/rcdb_segment_eigenvectors_sorted.root");
  mini_eigen_reg_5bpms->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles/rcdb_segment_eigenvectors_sorted.root");
  mini_eigen_reg_5bpms_sorted->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles/rcdb_segment_eigenvectors_sorted.root");
  // Default outputs, unsorted:
  //mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  mini->AddFriend(mini_eigen_reg_allbpms,"mini_eigen_reg_allbpms");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted,"mini_eigen_reg_allbpms_sorted");
  mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
  mini->AddFriend(mini_eigen_reg_5bpms,"mini_eigen_reg_5bpms");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted,"mini_eigen_reg_5bpms_sorted");
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

  std::string correctionpdfname = Form("Eigen_regressed_corrections_asyms.pdf");
  std::string nullpdfname = Form("Eigen_regressed_NULL_asyms.pdf");
  std::string signedpdfname = Form("Eigen_regressed_signed_asyms.pdf");

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
  c1->Divide(1,3);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1";
  p1=multiGraph(c1,p1,mini,1,Form("mini_eigen_reg_5bpms.reg_asym_usl"),cut,44);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2";
  p1=multiGraph(c1,p1,mini,2,Form("mini_eigen_reg_5bpms.reg_asym_usr"),cut,44);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0";
  p1=multiGraph(c1,p1,mini,3,Form("mini_eigen_reg_5bpms.reg_asym_us_avg"),cut,44);

  c1->cd();
  ana = "CREX 5BPM Eigen Vector regression results"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c1->SaveAs(Form("%s(",nullpdfname.c_str()));

  TCanvas* c2 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2->cd();
  c2->SetTitle(ana.c_str());
  c2->SetName(ana.c_str());
  c2->Divide(1,3);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1";
  p1=multiGraph(c2,p1,mini,1,Form("mini_eigen_reg_allbpms.reg_asym_usl"),cut,48);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2";
  p1=multiGraph(c2,p1,mini,2,Form("mini_eigen_reg_allbpms.reg_asym_usr"),cut,48);
  cut = "rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0";
  p1=multiGraph(c2,p1,mini,3,Form("mini_eigen_reg_allbpms.reg_asym_us_avg"),cut,48);

  c2->cd();
  ana = "CREX All BPM Eigen Vector regression results"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c2->SaveAs(Form("%s)",nullpdfname.c_str()));

  TCanvas* c3 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c3->cd();
  c3->SetTitle(ana.c_str());
  c3->SetName(ana.c_str());
  c3->Divide(1,3);
  cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1)";
  p1=multiGraph(c3,p1,mini,1,Form("mini_eigen_reg_5bpms.reg_asym_usl"),cut,44);
  cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2)";
  p1=multiGraph(c3,p1,mini,2,Form("mini_eigen_reg_5bpms.reg_asym_usr"),cut,44);
  cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0)";
  p1=multiGraph(c3,p1,mini,3,Form("mini_eigen_reg_5bpms.reg_asym_us_avg"),cut,44);

  c3->cd();
  ana = "CREX 5BPM Eigen Vector regression results"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c3->SaveAs(Form("%s(",signedpdfname.c_str()));

  TCanvas* c4 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c4->cd();
  c4->SetTitle(ana.c_str());
  c4->SetName(ana.c_str());
  c4->Divide(1,3);
  cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1)";
  p1=multiGraph(c4,p1,mini,1,Form("mini_eigen_reg_allbpms.reg_asym_usl"),cut,48);
  cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2)";
  p1=multiGraph(c4,p1,mini,2,Form("mini_eigen_reg_allbpms.reg_asym_usr"),cut,48);
  cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0)";
  p1=multiGraph(c4,p1,mini,3,Form("mini_eigen_reg_allbpms.reg_asym_us_avg"),cut,48);

  c4->cd();
  ana = "CREX All BPM Eigen Vector regression results"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c4->SaveAs(Form("%s",signedpdfname.c_str()));

  TCanvas* c5 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c5->cd();
  c5->SetTitle(ana.c_str());
  c5->SetName(ana.c_str());
  c5->Divide(2,2);
  cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1)";
  p1=master_multiGraph(c5,p1,mini,1,Form("mini_eigen_reg_5bpms"),cut,38);
  p1=master_multiGraph(c5,p1,mini,2,Form("mini_eigen_reg_allbpms"),cut,39);
  p1=applied_segment_master_multiGraph(c5,p1,mini,3,Form("mini_eigen_reg_5bpms"),monitors5,cut,38);
  p1=applied_segment_master_multiGraph(c5,p1,mini,4,Form("mini_eigen_reg_allbpms"),monitors,cut,39);

  c5->cd();
  ana = "CREX All BPM Eigen Vector regression results, including L, R, and Both arm runs, comparing 5 to all bpm eigen regression"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c5->SaveAs(Form("%s)",signedpdfname.c_str()));

  TCanvas* c6 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c6->cd();
  c6->SetTitle(ana.c_str());
  c6->SetName(ana.c_str());
  c6->Divide(3,5);
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1)";
    p1=corrected_multiGraph(c6,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms_sorted.diff_evMon%d",j),Form("mini_eigen_reg_5bpms_sorted.usl_evMon%d",j),cut,48);
    cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2)";
    p1=corrected_multiGraph(c6,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms_sorted.diff_evMon%d",j),Form("mini_eigen_reg_5bpms_sorted.usr_evMon%d",j),cut,48);
    cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0)";
    p1=corrected_multiGraph(c6,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms_sorted.diff_evMon%d",j),Form("mini_eigen_reg_5bpms_sorted.us_avg_evMon%d",j),cut,48);
  }

  c6->cd();
  ana = "CREX All BPM Eigen Vector regression corrections 0-5"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c6->SaveAs(Form("%s(",correctionpdfname.c_str()));

  TCanvas* c7 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c7->cd();
  c7->SetTitle(ana.c_str());
  c7->SetName(ana.c_str());
  c7->Divide(3,6);
  for (Int_t j = 0 ; j < monitors.size()/2 ; j++) {
    cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1)";
    p1=corrected_multiGraph(c7,p1,mini,3*j+1,Form("mini_eigen_reg_allbpms_sorted.diff_evMon%d",j),Form("mini_eigen_reg_allbpms_sorted.usl_evMon%d",j),cut,38);
    cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2)";
    p1=corrected_multiGraph(c7,p1,mini,3*j+2,Form("mini_eigen_reg_allbpms_sorted.diff_evMon%d",j),Form("mini_eigen_reg_allbpms_sorted.usr_evMon%d",j),cut,38);
    cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0)";
    p1=corrected_multiGraph(c7,p1,mini,3*j+3,Form("mini_eigen_reg_allbpms_sorted.diff_evMon%d",j),Form("mini_eigen_reg_allbpms_sorted.us_avg_evMon%d",j),cut,38);
  }

  c7->cd();
  ana = "CREX All BPM Eigen Vector regression corrections 0-5"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c7->SaveAs(Form("%s",correctionpdfname.c_str()));

  TCanvas* c8 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c8->cd();
  c8->SetTitle(ana.c_str());
  c8->SetName(ana.c_str());
  c8->Divide(3,6);
  for (Int_t j = monitors.size()/2 ; j < monitors.size() ; j++) {
    cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=1)";
    p1=corrected_multiGraph(c8,p1,mini,3*j+1-18,Form("mini_eigen_reg_allbpms_sorted.diff_evMon%d",j),Form("mini_eigen_reg_allbpms_sorted.usl_evMon%d",j),cut,38);
    cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag!=2)";
    p1=corrected_multiGraph(c8,p1,mini,3*j+2-18,Form("mini_eigen_reg_allbpms_sorted.diff_evMon%d",j),Form("mini_eigen_reg_allbpms_sorted.usr_evMon%d",j),cut,38);
    cut = "rcdb_sign*(rcdb_run_flag==1 && rcdb_run_type==1 && rcdb_arm_flag==0)";
    p1=corrected_multiGraph(c8,p1,mini,3*j+3-18,Form("mini_eigen_reg_allbpms_sorted.diff_evMon%d",j),Form("mini_eigen_reg_allbpms_sorted.us_avg_evMon%d",j),cut,38);
  }

  c8->cd();
  ana = "CREX All BPM Eigen Vector regression corrections 6-11"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c8->SaveAs(Form("%s)",correctionpdfname.c_str()));

}

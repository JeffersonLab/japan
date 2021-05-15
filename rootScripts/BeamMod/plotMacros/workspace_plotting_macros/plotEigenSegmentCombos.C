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

TVirtualPad* combo_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::string cut = "", std::vector<std::string> vs = {""}, Int_t startcolor=30){
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
      nen = mini->Draw(Form("(%s_%s):run+0.1*mini",draw.c_str(),vs.at(i).c_str()),Form("run!=7760"),"goff");
    }
    else {
      // No more abs cut here...
      nen = mini->Draw(Form("(%s_%s):run+0.1*mini",draw.c_str(),vs.at(i).c_str()),Form("run!=7760 && %s",cut.c_str()),"goff");
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

  mg->SetNameTitle(Form("%s BPM combination composition",draw.c_str()),Form("%s BPM combination composition;Run+0.1*Minirun;%s composition",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  if (subpad%3==0 || cut=="rcdb_arm_flag==0") {
    legend->Draw();
  }
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* combo_segment_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::string cut = "", std::vector<std::string> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  std::map<Int_t,Int_t> segments;
  Int_t segment;
  mini->SetBranchAddress("dit_segment",&segment);
  for (Int_t nent = 0 ; nent<mini->GetEntries() ; nent++) {
    mini->GetEntry(nent);
    if (segments.count(segment) == 0) {
      segments[segment] = 1;
    }
    else {
      segments[segment] = segments.at(segment) + 1;
    }
  }

  std::vector<std::map<Int_t,Double_t>> avg_combos;
  std::vector<std::vector<Double_t>> avg_combos_miniruns;
  std::vector<Double_t>  xaxis;
  Int_t nen=0;
  std::vector<Int_t> totalEntries;
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    Int_t local_totalEntries = 0;
    std::vector<Double_t> tmp_avg;
    std::map<Int_t,Double_t> tmp_avg_combo;
    for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    //for (Int_t iSeg = 0; iSeg<segments.size() ; iSeg++) 
      //Printf("Adding %d events for segment %d",iter->second,iter->first);
      if (cut == "") {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw.c_str(),vs.at(i).c_str(),i,iter->first),Form("dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw.c_str(),vs.at(i).c_str()),Form("dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",iter->first,iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw.c_str(),vs.at(i).c_str(),iter->first);
      }
      else {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw.c_str(),vs.at(i).c_str(),i,iter->first),Form("%s && dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",cut.c_str(),iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw.c_str(),vs.at(i).c_str()),Form("%s && dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",cut.c_str(),iter->first,iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut %s && dit_segment==%d",nen,draw.c_str(),vs.at(i).c_str(),cut.c_str(),iter->first);
      }
      if (nen != iter->second && nen!=0) {
        //Printf("Error, number of entries passing cut here = %d != above %d",nen,iter->second);
      }
      if (nen != 0) {
        //TGraph* g1 = (TGraph*)p1->FindObject("Graph");
        //g1->SetName(Form("g1_%d_%d",i,iter->first));
        //if (g1 != 0) {
          //Printf("g1 Works!");
        //}
        //TH2* ah1 = (TH2*)gROOT->Get("htemp");
        TH1* ah1;
        ah1 = (TH1*)(p1->FindObject("htemp"));
        //ah1 = (TH1*)(p1->FindObject(Form("h1_%d_%d",i,iter->first)));
        //if (ah1 != 0) {
          //ah1->SetName(Form("h1_%d_%d",i,iter->first));
        //}
        if (nen != 0 && ah1 != 0) {
          Double_t avg_tmp1 = 0.0;
          avg_tmp1 = (Double_t)(ah1->GetMean(1));
          //Printf("Mean = %f, entries = %d",avg_tmp1,nen);
          tmp_avg_combo[iter->first] = avg_tmp1;
          if (cut == "") {
            nen = mini->Draw(Form("run+0.1*mini>>h2_%d_%d",i,iter->first),Form("dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",iter->first,iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw.c_str(),vs.at(i).c_str(),iter->first);
          }
          else {
            nen = mini->Draw(Form("run+0.1*mini>>h2_%d_%d",i,iter->first),Form("%s && dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",cut.c_str(),iter->first,iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut %s && dit_segment==%d",nen,draw.c_str(),vs.at(i).c_str(),cut.c_str(),iter->first);
          }
          std::vector<Double_t> tmp_xaxis(mini->GetV1(), mini->GetV1() + mini->GetSelectedRows() % mini->GetEstimate());
          for (Int_t j = 0 ; j < nen ; j++){
            tmp_avg.push_back(avg_tmp1);
            if (i == 0) {
              Double_t local_xaxis = 0.0;
              local_xaxis = tmp_xaxis.at(j);
              //Printf("Adding run+0.1*minirun = %0.1f",tmp_xaxis.at(j));
              xaxis.push_back(local_xaxis);
            }
          }
          local_totalEntries += nen;
          //ah1->Delete();
        }
        //else {
          //Printf("outside cut %s",cut.c_str());
        //}
      }
      else {
        //Printf("outside cut %s",cut.c_str());
      }
    }
    totalEntries.push_back(local_totalEntries);
    avg_combos_miniruns.push_back(tmp_avg);
    avg_combos.push_back(tmp_avg_combo);
  }
  //nen = mini->Draw(Form("run+0.1*mini"),cut.c_str(),"goff");
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(totalEntries.at(i),&xaxis[0],&(avg_combos_miniruns.at(i))[0]);
      //Printf("X axis has %d entries, Y has %d entries",xaxis.size(),avg_combos_miniruns.at(i).size());
      //tmpG = new TGraph(totalEntries,(const Double_t *)mini->GetV1(),(const Double_t *)(avg_combos_miniruns.at(i)));
      //tmpG = new TGraph(totalEntries.at(i),(const Double_t *)xaxis,(const Double_t *)(avg_combos_miniruns.at(i)));
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d_avg",vs.at(i).c_str(),draw.c_str(),subpad),Form("%s_%s_%d_avg",vs.at(i).c_str(),draw.c_str(),subpad));
      //if (cut == "") {
        tmpG->SetLineColor(startcolor+2*i);
        tmpG->SetMarkerColor(startcolor+2*i);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.4);
      mg->Add(tmpG,"p");
      legend->AddEntry(tmpG,(TString)vs.at(i),"lp");
    }
  }

  mg->SetNameTitle(Form("%s BPM combination composition segment averaged",draw.c_str()),Form("%s BPM combination composition segment averaged;Run+0.1*Minirun;%s composition segment averaged",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  if (subpad%3==0 || cut=="rcdb_arm_flag==0") {
    legend->Draw();
  }
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

  mg->SetNameTitle(Form("%s BPM combination composition",draw.c_str()),Form("%s BPM combination composition;Run+0.1*Minirun;%s composition",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  if (subpad%3==0 || cut=="rcdb_arm_flag==0") {
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
mini->AddFriend(mini_eigen_reg_allbpms_sorted)
mini->AddFriend(mini_eigen_reg_5bpms_sorted)
 mini->Scan("run:mini:abs(mini_eigen_reg_5bpms_sorted.us_avg_evMon0/(ppm/um)):abs(mini_eigen_reg_5bpms_sorted.usl_evMon0/(ppm/um)):abs(mini_eigen_reg_5bpms_sorted.usl_evMon1/(ppm/um)):abs(mini_eigen_reg_5bpms_sorted.usl_evMon2/(ppm/um)):abs(mini_eigen_reg_5bpms_sorted.usl_evMon3/(ppm/um)):abs(mini_eigen_reg_5bpms_sorted.usl_evMon4/(ppm/um))")
mini->Scan("run:mini:mini_eigen_reg_5bpms_sorted.evMon0_bpm1X")
mini->Scan("run:mini:mini_eigen_reg_allbpms_sorted.evMon0_bpm1X")
mini->Scan("run:mini:mini_eigen_reg_5bpms_sorted.evMon0_bpm1X")
mini->Scan("run:mini:mini_eigen_reg_5bpms_sorted.evMon0_bpm1X:mini_eigen_reg_5bpms_sorted.evMon0_bpm4aY:mini_eigen_reg_5bpms_sorted.evMon0_bpm4eX:mini_eigen_reg_5bpms_sorted.evMon0_bpm4eY:mini_eigen_reg_5bpms_sorted.evMon0_bpm12X")
mini->Scan("run:mini:mini_eigen_reg_5bpms_sorted.evMon1_bpm1X:mini_eigen_reg_5bpms_sorted.evMon1_bpm4aY:mini_eigen_reg_5bpms_sorted.evMon1_bpm4eX:mini_eigen_reg_5bpms_sorted.evMon1_bpm4eY:mini_eigen_reg_5bpms_sorted.evMon1_bpm12X")
*/

void plotEigenSegmentCombos(TString suffix = ""){ //std::string input = "NULL",std::string runNum = "", std::string slugNum = "0"){}
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* mini = new TChain("mini");
  TChain* mini_eigen_reg_allbpms_sorted    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s",suffix.Data())); // Unsorted versions have no _sorted in the name
  TChain* mini_eigen_reg_allbpms_tr        = new TChain(Form("mini_eigen_reg_allbpms_tr"));     // Truncated one is not sorted... just deal with it I guess (could sort it later if urgently needed)
  TChain* mini_eigen_reg_5bpms_sorted      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s",suffix.Data()));
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  mini->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted%s.root",suffix.Data()));
  mini_eigen_reg_allbpms_sorted->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted%s.root",suffix.Data()));
  mini_eigen_reg_allbpms_tr->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted%s.root",suffix.Data()));
  mini_eigen_reg_5bpms_sorted->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted%s.root",suffix.Data()));
  // Default outputs, unsorted:
  //mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted,"mini_eigen_reg_allbpms_sorted");
  mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
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
  std::vector<std::string> devices5 = {
    "bpm1X",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm12X"
  };
  std::vector<std::string> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};


  // FIXME Using no cut (rcdb_run_type or run_flag or dit_flag) assumes that the dataset being fed in is a GOOD PRODUCTION and suitable for application to BMOD segments as well. For proof of concept comparisons the cut may not be that important, as we just want to compare eigen reg to eigen dit, etc. but for final numbers this will really matter.
  std::string cut = "";
  // Do some Tree->SetAlias() around everywhere

  std::string pdfname = Form("test_Segment_EigenCombos%s.pdf",suffix.Data());

  std::string ana = "Eigen BPMs combos";
  TText *label = new TText(0.0,0.005,ana.c_str());
  label->SetTextFont(23);
  label->SetTextSize(12);
  label->SetNDC();

  TVirtualPad* p1;

  /*TCanvas* c9 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c9->cd();
  c9->SetTitle(ana.c_str());
  c9->SetName(ana.c_str());
  c9->Divide(2,3);
  cut = "run<=6332";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    p1=combo_segment_multiGraph(c9,p1,mini,j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).c_str()),cut,devices5,36);
  }

  c9->cd();
  ana = "CREX Part 1 Eigen Vector Monitor BPMs compositions"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");

  c9->SaveAs(Form("%s(",pdfname.c_str()));
  c9->SaveAs(Form("5BPM_evMon_segmented_compositions%s(.pdf",suffix.Data()));
  */

  TCanvas* c10 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c10->cd();
  c10->SetTitle(ana.c_str());
  c10->SetName(ana.c_str());
  c10->Divide(2,3);
  cut = "";
  //cut = "run>6332 && run<7500";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    p1=combo_segment_multiGraph(c10,p1,mini,j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).c_str()),cut,devices5,36);
  }

  c10->cd();
  ana = "CREX Part 2 Eigen Vector Monitor BPMs compositions"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c10->SaveAs(Form("%s",pdfname.c_str()));
  c10->SaveAs(Form("5BPM_evMon_segmented_compositions%s.pdf",suffix.Data()));

  /*
  TCanvas* c11 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c11->cd();
  c11->SetTitle(ana.c_str());
  c11->SetName(ana.c_str());
  c11->Divide(2,3);
  cut = "run>7500 && run!=7760";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    p1=combo_segment_multiGraph(c11,p1,mini,j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).c_str()),cut,devices5,36);
  }
  c11->cd();
  ana = "CREX Part 3 Eigen Vector Monitor BPMs compositions"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c11->SaveAs(Form("%s",pdfname.c_str()));
  c11->SaveAs(Form("5BPM_evMon_segmented_compositions%s).pdf",suffix.Data()));
  */

  TCanvas* c12 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c12->cd();
  c12->SetTitle(ana.c_str());
  c12->SetName(ana.c_str());
  c12->Divide(3,6);
  cut = "";
  for (Int_t j = 0 ; j < monitors.size()/2 ; j++) {
    p1=combo_segment_multiGraph(c12,p1,mini,3*j+1,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).c_str()),cut,X_BPMs,36);
    p1=combo_segment_multiGraph(c12,p1,mini,3*j+2,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).c_str()),cut,Y_BPMs,36);
    p1=combo_segment_multiGraph(c12,p1,mini,3*j+3,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).c_str()),cut,E_BPMs,36);
  }
  c12->cd();
  ana = "CREX All runs, 0-5 Eigen Vector Monitor BPMs us_avg slopes compositions (signed values)";
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c12->SaveAs(Form("%s",pdfname.c_str()));
  c12->SaveAs(Form("AllBPM_evMon_segmented_compositions%s.pdf(",suffix.Data()));

  TCanvas* c13 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c13->cd();
  c13->SetTitle(ana.c_str());
  c13->SetName(ana.c_str());
  c13->Divide(3,6);
  cut = "";
  for (Int_t j = monitors.size()/2 ; j < monitors.size() ; j++) {
    p1=combo_segment_multiGraph(c13,p1,mini,3*j+1-18,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).c_str()),cut,X_BPMs,36);
    p1=combo_segment_multiGraph(c13,p1,mini,3*j+2-18,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).c_str()),cut,Y_BPMs,36);
    p1=combo_segment_multiGraph(c13,p1,mini,3*j+3-18,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).c_str()),cut,E_BPMs,36);
  }
  c13->cd();
  ana = "CREX All runs, 6-11 Eigen Vector Monitor BPMs us_avg slopes compositions (signed values)";
  label->SetText(0.0,0.005,ana.c_str());
  label->Draw("same");
  c13->SaveAs(Form("%s)",pdfname.c_str()));
  c13->SaveAs(Form("AllBPM_evMon_segmented_compositions%s.pdf)",suffix.Data()));

}

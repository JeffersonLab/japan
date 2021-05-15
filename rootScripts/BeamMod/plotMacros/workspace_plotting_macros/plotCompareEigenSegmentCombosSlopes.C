TVirtualPad* multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  if (((TString)cut).Contains("rcdb_sign")) { 
    nen = mini->Draw(Form("rcdb_sign*(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.c_str(),draw.c_str()),cut.Data(),"goff");
  }
  else {
    nen = mini->Draw(Form("(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.c_str(),draw.c_str()),cut.Data(),"goff");
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

TVirtualPad* combo_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", std::vector<TString> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;
  TString x_axis_name = "run+0.1*mini";
  if ((TString)mini->GetName() == "dit") {
    x_axis_name = "run";
  }

  Int_t nen=0;
  for (Int_t i = vs.size()-1 ; i >= 0 ; i--) {
    if (cut == "") {
      nen = mini->Draw(Form("(%s_%s):%s",draw.c_str(),vs.at(i).Data(),x_axis_name.Data()),Form("run!=7760"),"goff");
    }
    else {
      // No more abs cut here...
      nen = mini->Draw(Form("(%s_%s):%s",draw.c_str(),vs.at(i).Data(),x_axis_name.Data()),Form("run!=7760 && %s",cut.Data()),"goff");
      //nen = mini->Draw(Form("abs(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).Data()),Form("run!=7760 && %s",cut.Data()),"goff");
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d_combo",vs.at(i).Data(),((TString)draw).Data(),subpad),Form("%s_%s_%d",vs.at(i).Data(),((TString)draw).Data(),subpad));
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
      //if (cut == "") {}
      mg->Add(tmpG,"p");
      legend->AddEntry(tmpG,(TString)vs.at(i),"lp");
    }
  }

  if (((TString)draw).Contains("evMon")) {
    mg->SetNameTitle(Form("%s BPM combination composition_combo %d",draw.c_str(),subpad),Form("%s BPM combination composition;Run+0.1*Minirun;%s composition",draw.c_str(),draw.c_str()));
  }
  else {
    mg->SetNameTitle(Form("%s slopes vs evMons_combo %d",draw.c_str(),subpad),Form("%s slopes vs, evMons;Run+0.1*Minirun;%s slopes",draw.c_str(),draw.c_str()));
  }
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

TVirtualPad* combo_segment_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", TString averaging = "dit_segment", std::vector<TString> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();
  Printf("test1");

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  TString x_axis_name = "run+0.1*mini";
  if ((TString)mini->GetName() == "dit") {
    x_axis_name = "run";
  }
  std::map<Int_t,Int_t> segments;
  Int_t segment;
  Int_t segment_run;
  Double_t double_segment;
  Printf("test2");
  if (((TString)averaging).Contains("segment")){
  Printf("test3");
    mini->SetBranchAddress(Form("%s",averaging.Data()),&segment);
    Printf("set branch addreses %s to an integer",averaging.Data());
    mini->Draw(Form(">>elist_%s_%d",draw.c_str(),subpad), Form("%s",cut.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(segment) == 0) {
        segments[segment] = 1;
      }
      else {
        segments[segment] = segments.at(segment) + 1;
      }
    }
    mini->SetEntryList(0);
    delete elist;
  } 
  else if (((TString)averaging).Contains("run")){
  Printf("test3 - run");
    mini->SetBranchAddress(Form("%s",averaging.Data()),&segment_run);
    Printf("set branch addreses %s to an integer",averaging.Data());
    mini->Draw(Form(">>elist_%s_%d",draw.c_str(),subpad), Form("%s",cut.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(segment_run) == 0) {
        segments[segment_run] = 1;
      }
      else {
        segments[segment_run] = segments.at(segment_run) + 1;
      }
    }
    mini->SetEntryList(0);
    delete elist;
  } 
  else {
  Printf("test4");
    mini->SetBranchAddress(Form("%s",averaging.Data()),&double_segment);
    mini->Draw(Form(">>elist_%s_%d",draw.c_str(),subpad), Form("%s",cut.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(double_segment) == 0) {
        segments[double_segment] = 1;
      }
      else {
        segments[double_segment] = segments.at(double_segment) + 1;
      }
    }
    mini->SetEntryList(0);
    delete elist;
  }

  std::vector<std::map<Int_t,Double_t>> avg_combos;
  std::vector<std::vector<Double_t>> avg_combos_miniruns;
  std::vector<Double_t>  xaxis;
  Int_t nen=0;
  std::vector<Int_t> totalEntries;

  for (Int_t i = vs.size()-1 ; i >= 0 ; i--) {
    Int_t local_totalEntries = 0;
    std::vector<Double_t> tmp_avg;
    std::map<Int_t,Double_t> tmp_avg_combo;
    for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    //for (Int_t iSeg = 0; iSeg<segments.size() ; iSeg++) 
      //Printf("Adding %d events for segment %d",iter->second,iter->first);
  //Printf("test5");
      // FIXME generate a mapout textfile
      if (cut == "") {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw.c_str(),vs.at(i).Data(),i,iter->first),Form("dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw.c_str(),vs.at(i).Data()),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",averaging.Data(),iter->first,averaging.Data(),iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw.c_str(),vs.at(i).Data(),iter->first);
      }
      else {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw.c_str(),vs.at(i).Data(),i,iter->first),Form("%s && dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",cut.Data(),iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw.c_str(),vs.at(i).Data()),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut.Data(),averaging.Data(),iter->first,averaging.Data(),iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut %s && dit_segment==%d",nen,draw.c_str(),vs.at(i).Data(),cut.Data(),iter->first);
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
  //Printf("test5.1");
        TH1* ah1;
        ah1 = (TH1*)(p1->FindObject("htemp"));
        //ah1 = (TH1*)(p1->FindObject(Form("h1_%d_%d",i,iter->first)));
        //if (ah1 != 0) {
          //ah1->SetName(Form("h1_%d_%d",i,iter->first));
        //}
        if (nen != 0 && ah1 != 0) {
  //Printf("test5.2");
          Double_t avg_tmp1 = 0.0;
          avg_tmp1 = (Double_t)(ah1->GetMean(1));
          //Printf("Mean = %f, entries = %d",avg_tmp1,nen);
          tmp_avg_combo[iter->first] = avg_tmp1;
          if (cut == "") {
            nen = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name.Data(),i,iter->first),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",averaging.Data(),iter->first,averaging.Data(),iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw.c_str(),vs.at(i).Data(),iter->first);
          }
          else {
            nen = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name.Data(),i,iter->first),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut.Data(),averaging.Data(),iter->first,averaging.Data(),iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut %s && dit_segment==%d",nen,draw.c_str(),vs.at(i).Data(),cut.Data(),iter->first);
          }
          std::vector<Double_t> tmp_xaxis(mini->GetV1(), mini->GetV1() + mini->GetSelectedRows() % mini->GetEstimate());
  //Printf("test5.3");
          // FIXME Print a mapfile for eigenvector based slopes (assume eigenvectors exist in mulc tree)
          for (Int_t j = 0 ; j < nen ; j++){
            tmp_avg.push_back(avg_tmp1);
            if (i == 0) {
  //Printf("test5.4");
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
          //Printf("outside cut %s",cut.Data());
        //}
      }
      else {
        Printf("outside cut %s",cut.Data());
      }
    }
  Printf("test6");
    totalEntries.push_back(local_totalEntries);
    avg_combos_miniruns.push_back(tmp_avg);
    avg_combos.push_back(tmp_avg_combo);
  }
  //nen = mini->Draw(Form("run+0.1*mini"),cut.Data(),"goff");
  for (Int_t i = vs.size()-1 ; i >= 0 ; i--) {
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
  Printf("test7");
      tmpG = new TGraph(totalEntries.at(i),&xaxis[0],&(avg_combos_miniruns.at(i))[0]);
      //Printf("X axis has %d entries, Y has %d entries",xaxis.size(),avg_combos_miniruns.at(i).size());
      //tmpG = new TGraph(totalEntries,(const Double_t *)mini->GetV1(),(const Double_t *)(avg_combos_miniruns.at(i)));
      //tmpG = new TGraph(totalEntries.at(i),(const Double_t *)xaxis,(const Double_t *)(avg_combos_miniruns.at(i)));
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d_avg_%s_combo_seg",vs.at(i).Data(),draw.c_str(),subpad,cut.Data()),Form("%s_%s_%d_avg",vs.at(i).Data(),draw.c_str(),subpad));
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

  Printf("test8");
  // Assume evMon are compositions, else doing a straight slope
  if (((TString)draw).Contains("evMon")) {
  Printf("test9");
    mg->SetNameTitle(Form("%s BPM combination composition %s averaged %d %s_combo_seg",draw.c_str(),averaging.Data(),subpad,cut.Data()),Form("%s BPM combination composition %s averaged;Run+0.1*Minirun;%s composition %s averaged",draw.c_str(),averaging.Data(),draw.c_str(),averaging.Data()));
  }
  else {
  Printf("test10");
    mg->SetNameTitle(Form("%s slopes vs evMons, %s averaged %d %s_combo_seg",draw.c_str(),averaging.Data(),subpad,cut.Data()),Form("%s slopes vs, evMons, %s averaged;Run+0.1*Minirun;%s slopes averaged",draw.c_str(),averaging.Data(),draw.c_str()));
  }
  mg->Draw("alp");
  //if (subpad%3==0 || cut=="rcdb_arm_flag==0") {
    legend->Draw();
  //}
  p1->Update();
  p1->Modified();
  return p1;
}

TVirtualPad* vs_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", std::vector<TString> vs = {""}, Int_t startcolor=30){
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
      nen = mini->Draw(Form("(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).Data()),Form("run!=7760"),"goff");
    }
    else {
      // No more abs cut here...
      nen = mini->Draw(Form("(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).Data()),Form("run!=7760 && %s",cut.Data()),"goff");
      //nen = mini->Draw(Form("abs(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).Data()),Form("run!=7760 && %s",cut.Data()),"goff");
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d",vs.at(i).Data(),draw.c_str(),subpad),Form("%s_%s_%d",vs.at(i).Data(),draw.c_str(),subpad));
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


TVirtualPad* vs_ranked_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", std::vector<TString> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  TString localCut = cut;

  Int_t nen=0;
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    localCut = cut;
    if (localCut == "") {
      localCut = Form("(run>5000 && run != 7760)");
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s%s_%s) > abs(%s%s_%s))",localCut.Data(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(k).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str());
        }
      }
      Printf("%s",localCut.Data());
      nen = mini->Draw(Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()),Form("%s",localCut.Data()),"goff");
      Printf("%s",Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()));
    }
    else {
      localCut = Form("(%s)",localCut.Data());
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s%s_%s) > abs(%s%s_%s))",localCut.Data(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(k).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str());
        }
      }
      Printf("%s",localCut.Data());
      nen = mini->Draw(Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()),Form("%s",localCut.Data()),"goff");
      // No more absolute values in here
      //nen = mini->Draw(Form("abs(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()),Form("%s",localCut.Data()),"goff");
      Printf("%s",Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()));
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d",vs.at(i).Data(),draw.c_str(),subpad),Form("%s_%s_%d",vs.at(i).Data(),draw.c_str(),subpad));
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


TVirtualPad* vs_ranked_weighted_slope_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, TString tree = "mini", TString detector = "us_avg", TString device = "bpm12X", std::vector<TString> vs = {""}, TString cut = "", Int_t startcolor=30){ 
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

  TString localCut = cut;

  Int_t nen=0;
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    localCut = cut;
    if (localCut == "") {
      localCut = Form("(run>5000 && run != 7760)");
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s.%s_%s*%s.%s_%s) > abs(%s.%s_%s*%s.%s_%s))",localCut.Data(),tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data(),tree.Data(),detector.Data(),vs.at(k).Data(),tree.Data(),vs.at(k).Data(),device.Data());
        }
      }
      Printf("%s",localCut.Data());
      nen = mini->Draw(Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()),Form("%s",localCut.Data()),"goff");
      Printf("%s",Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()));
    }
    else {
      localCut = Form("(%s)",localCut.Data());
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s.%s_%s*%s.%s_%s) > abs(%s.%s_%s*%s.%s_%s))",localCut.Data(),tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data(),tree.Data(),detector.Data(),vs.at(k).Data(),tree.Data(),vs.at(k).Data(),device.Data());
        }
      }
      Printf("%s",localCut.Data());
      nen = mini->Draw(Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()),Form("%s",localCut.Data()),"goff");
      // No more absolute values 
      // nen = mini->Draw(Form("abs(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()),Form("%s",localCut.Data()),"goff");
      Printf("%s",Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()));
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s.%s_%s_%d",tree.Data(),vs.at(i).Data(),device.Data(),subpad),Form("%s.%s_%s_%d",tree.Data(),vs.at(i).Data(),device.Data(),subpad));
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
      legend->AddEntry(tmpG,Form("%s_%s",detector.Data(),vs.at(i).Data()),"p");
    }
  }

  mg->SetNameTitle(Form("%s's ranked EigVecMon with largest amount of %s BPM slope;Run+0.1*Minirun;%s",detector.Data(),device.Data(),device.Data()),Form("%s's ranked EigVecMon with largest amount of %s BPM slope;Run+0.1*Minirun;%s_monitor_%s",detector.Data(),device.Data(),detector.Data(),device.Data()));
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

TVirtualPad* highlightedGraph(TCanvas* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "run:cyclenum", TString cut = "", TString cut2 = "0",TString drawopts = "*", int color = 4, TString cut3 = "0", int color3=2, TString cut4 = "") {
  if (cut4 != "") {
    cut = cut+"&&"+cut4;
    cut2 = cut2+"&&"+cut4;
    if (cut3!= "") {
      cut3 = cut3+"&&"+cut4;
    }
  }

  p1=c1->cd(subpad);
  p1->SetGrid();
  mini->Draw(draw.c_str(),cut.Data(),drawopts.Data());

  if (p1->FindObject("Graph")) {
    ((TGraph*)p1->FindObject("Graph"))->SetName("full");
    mini->Draw(draw.c_str(),Form("%s",cut2.Data()),Form("%ssame",drawopts.Data()));
    if (p1->FindObject("Graph")) {
      ((TGraph*)p1->FindObject("Graph"))->SetName("color");
      ((TGraph*)p1->FindObject("color"))->SetMarkerColor(color);
    }
    if (cut3 != "") {
      mini->Draw(draw.c_str(),Form("%s",cut3.Data()),Form("%ssame",drawopts.Data()));
      if (p1->FindObject("Graph")) {
        ((TGraph*)p1->FindObject("Graph"))->SetName("color3");
        ((TGraph*)p1->FindObject("color3"))->SetMarkerColor(color3);
      }
      mini->Draw(draw.c_str(),Form("(%s) && (%s)",cut2.Data(),cut3.Data()),Form("%ssame",drawopts.Data()));
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

void plotCompareEigenSegmentCombosSlopes(TString suffix = ""){ //TString input = "NULL",TString runNum = "", TString slugNum = "0"){}
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* dit = new TChain("dit");
  TChain* mini = new TChain("mini");
  TChain* mini_eigen_reg_allbpms_sorted    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s",suffix.Data())); // Unsorted versions have no _sorted in the name
  TChain* mini_eigen_reg_allbpms_tr        = new TChain(Form("mini_eigen_reg_allbpms_tr"));     // Truncated one is not sorted... just deal with it I guess (could sort it later if urgently needed)
  TChain* mini_eigen_reg_5bpms_part_avg_static      = new TChain(Form("mini_eigen_reg_5bpms_part_avg%s",suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s",suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_part_avg    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_part_avg",suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_part_avg      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_part_avg",suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_seg_avg    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_seg_avg",suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_seg_avg      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_seg_avg",suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_slug_avg    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_slug_avg",suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_slug_avg      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_slug_avg",suffix.Data()));
  TChain* mini_eigen_reg_allbpms_sorted_run_avg    = new TChain(Form("mini_eigen_reg_allbpms_sorted%s_run_avg",suffix.Data()));
  TChain* mini_eigen_reg_5bpms_sorted_run_avg      = new TChain(Form("mini_eigen_reg_5bpms_sorted%s_run_avg",suffix.Data()));
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  /*mini->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted%s.root",suffix.Data()));
  mini_eigen_reg_allbpms_sorted->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted%s.root",suffix.Data()));
  mini_eigen_reg_allbpms_tr->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted%s.root",suffix.Data()));
  mini_eigen_reg_5bpms_sorted->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_eigenvectors_sorted%s.root",suffix.Data()));
  // Default outputs, unsorted:
  //mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//Full_mini_eigen_reg_allbpms_CREX.root");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted,"mini_eigen_reg_allbpms_sorted");
  mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted,"mini_eigen_reg_5bpms_sorted");*/
  //dit->AddFile(Form("march_10_rootfiles/dit_alldet_slopes_run_avg_eigenvectors_sorted_seg_avg_1X%s.root",suffix.Data()));
  dit->AddFile(Form("march_17_rootfiles/dit_alldet_slopes_run_avg_eigenvectors_sorted_part_avg_1X%s.root",suffix.Data()));
  //dit->AddFile(Form("march_17_rootfiles/dit_alldet_slopes_run_avg_eigenvectors_sorted_seg_avg_1X%s.root",suffix.Data()));
  mini->AddFile(Form("march_17_rootfiles/rcdb_segment_parts_eigenvectors_sorted%s.root",suffix.Data()));
  //mini->AddFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin1/LagrangeOutput/rootfiles//rcdb_segment_parts_eigenvectors_sorted%s.root",suffix.Data()));
  //mini->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  //mini_eigen_reg_allbpms_sorted->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  //mini_eigen_reg_allbpms_tr->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  mini_eigen_reg_5bpms_part_avg_static->AddFile(Form("march_17_rootfiles/rcdb_segment_parts_eigenvectors_sorted%s.root",suffix.Data()));
  mini_eigen_reg_5bpms_sorted->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  mini_eigen_reg_allbpms_sorted_part_avg->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  mini_eigen_reg_5bpms_sorted_part_avg->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  mini_eigen_reg_allbpms_sorted_seg_avg->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  mini_eigen_reg_5bpms_sorted_seg_avg->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  mini_eigen_reg_allbpms_sorted_slug_avg->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  mini_eigen_reg_5bpms_sorted_slug_avg->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Part_Seg_Slug_Avgs-NormedEigenVector-Calculation%s.root",suffix.Data()));
  mini_eigen_reg_allbpms_sorted_run_avg->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Run_Avg-NormedEigenVector-Calculation%s.root",suffix.Data()));
  mini_eigen_reg_5bpms_sorted_run_avg->AddFile(Form("march_17_rootfiles/Segment_Avg_EigenCombos_Run_Avg-NormedEigenVector-Calculation%s.root",suffix.Data()));
  // Default outputs, unsorted:
  //mini->AddFriend(mini_eigen_reg_allbpms_sorted,"mini_eigen_reg_allbpms_sorted");
  //mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
  mini->AddFriend(mini_eigen_reg_5bpms_part_avg_static,"mini_eigen_reg_5bpms_part_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted,"mini_eigen_reg_5bpms_sorted");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_part_avg,"mini_eigen_reg_allbpms_sorted_part_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_part_avg,"mini_eigen_reg_5bpms_sorted_part_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_seg_avg,"mini_eigen_reg_allbpms_sorted_seg_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_seg_avg,"mini_eigen_reg_5bpms_sorted_seg_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_slug_avg,"mini_eigen_reg_allbpms_sorted_slug_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_slug_avg,"mini_eigen_reg_5bpms_sorted_slug_avg");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_run_avg,"mini_eigen_reg_allbpms_sorted_run_avg");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted_run_avg,"mini_eigen_reg_5bpms_sorted_run_avg");
  dit->BuildIndex("run","cyclenum");
  mini->BuildIndex("run","mini");

  std::vector<TString> X_BPMs = {
    "bpm4aX",
    "bpm4eX",
    "bpm1X",
    "bpm16X",
  };
  std::vector<TString> Y_BPMs = {
    "bpm4aY",
    "bpm4eY",
    "bpm1Y",
    "bpm16Y",
  };
  std::vector<TString> E_BPMs = {
    "bpm11X",
    "bpm12X",
  };
  std::vector<TString> devices = {
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
  std::vector<TString> monitors = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  std::vector<TString> devices5 = {
    "bpm1X",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm12X"
  };
  std::vector<TString> devices5_mean = {
    "bpm1X_mean",
    "bpm4aY_mean",
    "bpm4eX_mean",
    "bpm4eY_mean",
    "bpm12X_mean"
  };
  std::vector<TString> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};


  // FIXME Using no cut (rcdb_run_type or run_flag or dit_flag) assumes that the dataset being fed in is a GOOD PRODUCTION and suitable for application to BMOD segments as well. For proof of concept comparisons the cut may not be that important, as we just want to compare eigen reg to eigen dit, etc. but for final numbers this will really matter.
  TString cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  // Do some Tree->SetAlias() around everywhere

  TString pdfname = Form("outputs_Segment_Avg_EigenCombos%s.pdf",suffix.Data());

  TString ana = "Eigen BPMs combos";
  TText *label = new TText(0.0,0.005,ana);
  label->SetTextFont(23);
  label->SetTextSize(12);
  label->SetNDC();

  TVirtualPad* p1;

  /*TCanvas* c9 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c9->cd();
  c9->SetTitle(ana);
  c9->SetName(ana);
  c9->Divide(2,3);
  cut = "run<=6332";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    p1=combo_segment_multiGraph(c9,p1,mini,j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,devices5,36);
  }

  c9->cd();
  ana = "CREX Part 1 Eigen Vector Monitor BPMs compositions"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");

  c9->SaveAs(Form("%s(",pdfname.Data()));
  c9->SaveAs(Form("5BPM_evMon_segmented_compositions%s(.pdf",suffix.Data()));
  */


  //FIXME temporarily remove the run/burst averaged eigen value slopes for sanity
  /*
  TCanvas* c0 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c0->cd();
  c0->SetTitle(ana);
  c0->SetName(ana);
  c0->Divide(3,2);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
  p1=combo_segment_multiGraph(c0,p1,mini,1,Form("1e3*mini_eigen_reg_5bpms_sorted.usl"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
  p1=combo_segment_multiGraph(c0,p1,mini,2,Form("1e3*mini_eigen_reg_5bpms_sorted.usr"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  p1=combo_segment_multiGraph(c0,p1,mini,3,Form("1e3*mini_eigen_reg_5bpms_sorted.us_avg"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
  p1=combo_segment_multiGraph(c0,p1,mini,4,Form("1e3*mini_eigen_reg_5bpms_sorted.usl"),cut,"rcdb_slug",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
  p1=combo_segment_multiGraph(c0,p1,mini,5,Form("1e3*mini_eigen_reg_5bpms_sorted.usr"),cut,"rcdb_slug",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  p1=combo_segment_multiGraph(c0,p1,mini,6,Form("1e3*mini_eigen_reg_5bpms_sorted.us_avg"),cut,"rcdb_slug",monitors5,36);
  c0->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Segment and Slug Averaged"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c0->SaveAs(Form("%s(",pdfname.Data()));
  c0->SaveAs(Form("5BPM_evMon_segmented_slopes%s.pdf",suffix.Data()));
  */

  // FIXME Eigen regression slopes section
  TCanvas* c1 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1->cd();
  //c1->SetTitle(ana);
  //c1->SetName(ana);
  c1->Divide(3,3);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500";
  //for (Int_t j = 0 ; j < monitors5.size() ; j++) {
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
  p1=combo_multiGraph(c1,p1,mini,1,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
  p1=combo_multiGraph(c1,p1,mini,2,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  p1=combo_multiGraph(c1,p1,mini,3,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
  p1=combo_segment_multiGraph(c1,p1,mini,4,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
  p1=combo_segment_multiGraph(c1,p1,mini,5,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  p1=combo_segment_multiGraph(c1,p1,mini,6,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
  p1=combo_segment_multiGraph(c1,p1,mini,7,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
  p1=combo_segment_multiGraph(c1,p1,mini,8,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  p1=combo_segment_multiGraph(c1,p1,mini,9,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,"dit_segment",monitors5,36);
  c1->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c1->SaveAs(Form("%s(",pdfname.Data()));
  c1->SaveAs(Form("5BPM_evMon_segmented_eigen_reg_slopes%s.pdf(",suffix.Data()));
  
  TCanvas* c1_left = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1_left->cd();
  //c1_left->SetTitle(ana);
  //c1_left->SetName(ana);
  c1_left->Divide(3,3);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500";
  //for (Int_t j = 0 ; j < monitors5.size() ; j++) {
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==1";
  p1=combo_multiGraph(c1_left,p1,mini,1,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==2";
  p1=combo_multiGraph(c1_left,p1,mini,2,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==3";
  p1=combo_multiGraph(c1_left,p1,mini,3,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==1";
  p1=combo_segment_multiGraph(c1_left,p1,mini,4,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==2";
  p1=combo_segment_multiGraph(c1_left,p1,mini,5,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==3";
  p1=combo_segment_multiGraph(c1_left,p1,mini,6,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==1";
  p1=combo_segment_multiGraph(c1_left,p1,mini,7,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==2";
  p1=combo_segment_multiGraph(c1_left,p1,mini,8,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==3";
  p1=combo_segment_multiGraph(c1_left,p1,mini,9,Form("1e3*mini_eigen_reg_5bpms_part_avg.usl"),cut,"dit_segment",monitors5,36);

  c1_left->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged, usl, 3 parts of CREX"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c1_left->SaveAs(Form("%s",pdfname.Data()));
  c1_left->SaveAs(Form("5BPM_evMon_segmented_eigen_reg_slopes%s.pdf",suffix.Data()));
  
  TCanvas* c1_right = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1_right->cd();
  c1_right->SetTitle(ana);
  c1_right->SetName(ana);
  c1_right->Divide(3,3);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500";
  //for (Int_t j = 0 ; j < monitors5.size() ; j++) {
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==1";
  p1=combo_multiGraph(c1_right,p1,mini,1,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==2";
  p1=combo_multiGraph(c1_right,p1,mini,2,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==3";
  p1=combo_multiGraph(c1_right,p1,mini,3,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==1";
  p1=combo_segment_multiGraph(c1_right,p1,mini,4,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==2";
  p1=combo_segment_multiGraph(c1_right,p1,mini,5,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==3";
  p1=combo_segment_multiGraph(c1_right,p1,mini,6,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==1";
  p1=combo_segment_multiGraph(c1_right,p1,mini,7,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==2";
  p1=combo_segment_multiGraph(c1_right,p1,mini,8,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==3";
  p1=combo_segment_multiGraph(c1_right,p1,mini,9,Form("1e3*mini_eigen_reg_5bpms_part_avg.usr"),cut,"dit_segment",monitors5,36);

  c1_right->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged, usr, 3 parts of CREX"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c1_right->SaveAs(Form("%s",pdfname.Data()));
  c1_right->SaveAs(Form("5BPM_evMon_segmented_eigen_reg_slopes%s.pdf",suffix.Data()));

  TCanvas* c1_avg = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1_avg->cd();
  c1_avg->SetTitle(ana);
  c1_avg->SetName(ana);
  c1_avg->Divide(3,3);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500";
  //for (Int_t j = 0 ; j < monitors5.size() ; j++) {
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==1";
  p1=combo_multiGraph(c1_avg,p1,mini,1,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==2";
  p1=combo_multiGraph(c1_avg,p1,mini,2,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==3";
  p1=combo_multiGraph(c1_avg,p1,mini,3,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==1";
  p1=combo_segment_multiGraph(c1_avg,p1,mini,4,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==2";
  p1=combo_segment_multiGraph(c1_avg,p1,mini,5,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==3";
  p1=combo_segment_multiGraph(c1_avg,p1,mini,6,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==1";
  p1=combo_segment_multiGraph(c1_avg,p1,mini,7,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==2";
  p1=combo_segment_multiGraph(c1_avg,p1,mini,8,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==3";
  p1=combo_segment_multiGraph(c1_avg,p1,mini,9,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_avg"),cut,"dit_segment",monitors5,36);

  c1_avg->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged, us_avg, 3 parts of CREX"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c1_avg->SaveAs(Form("%s",pdfname.Data()));
  c1_avg->SaveAs(Form("5BPM_evMon_segmented_eigen_reg_slopes%s.pdf",suffix.Data()));
  c1_avg->Clear();
  delete c1_avg;

  TCanvas* c1_dd = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1_dd->cd();
  c1_dd->SetTitle(ana);
  c1_dd->SetName(ana);
  c1_dd->Divide(3,3);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500";
  //for (Int_t j = 0 ; j < monitors5.size() ; j++) {
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==1";
  p1=combo_multiGraph(c1_dd,p1,mini,1,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_dd"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==2";
  p1=combo_multiGraph(c1_dd,p1,mini,2,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_dd"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==3";
  p1=combo_multiGraph(c1_dd,p1,mini,3,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_dd"),cut,monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==1";
  p1=combo_segment_multiGraph(c1_dd,p1,mini,4,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_dd"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==2";
  p1=combo_segment_multiGraph(c1_dd,p1,mini,5,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_dd"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==3";
  p1=combo_segment_multiGraph(c1_dd,p1,mini,6,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_dd"),cut,"run",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==1";
  p1=combo_segment_multiGraph(c1_dd,p1,mini,7,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_dd"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==2";
  p1=combo_segment_multiGraph(c1_dd,p1,mini,8,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_dd"),cut,"dit_segment",monitors5,36);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0 && crex_part==3";
  p1=combo_segment_multiGraph(c1_dd,p1,mini,9,Form("1e3*mini_eigen_reg_5bpms_part_avg.us_dd"),cut,"dit_segment",monitors5,36);

  c1_dd->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged, us_dd, 3 parts of CREX"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c1_dd->SaveAs(Form("%s",pdfname.Data()));
  c1_dd->SaveAs(Form("5BPM_evMon_segmented_eigen_reg_slopes%s.pdf)",suffix.Data()));

  TCanvas* c2 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2->cd();
  c2->SetTitle(ana);
  c2->SetName(ana);
  c2->Divide(2,2);
  //cut = "run>6332 && run<7500";
  //for (Int_t j = 0 ; j < monitors5.size() ; j++) {
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
  p1=combo_multiGraph(c2,p1,dit,1,Form("usl"),cut,monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
  p1=combo_multiGraph(c2,p1,dit,2,Form("usr"),cut,monitors5,36);
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  //p1=combo_multiGraph(c2,p1,dit,6,Form("us_avg"),cut,monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
  p1=combo_segment_multiGraph(c2,p1,dit,3,Form("usl"),cut,"segment",monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
  p1=combo_segment_multiGraph(c2,p1,dit,4,Form("usr"),cut,"segment",monitors5,36);
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  //p1=combo_segment_multiGraph(c2,p1,dit,3,Form("us_avg"),cut,"segment",monitors5,36);

  c2->cd();
  ana = "CREX Eigen Vector Monitor BMOD Slopes, Runwise and Runwise-Segment Averaged"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c2->SaveAs(Form("%s",pdfname.Data()));
  c2->SaveAs(Form("5BPM_evMon_segmented_bmod_slopes%s.pdf(",suffix.Data()));


  TCanvas* c2_left = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2_left->cd();
  c2_left->SetTitle(ana);
  c2_left->SetName(ana);
  c2_left->Divide(3,2);
  //cut = "run>6332 && run<7500";
  //for (Int_t j = 0 ; j < monitors5.size() ; j++) {
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==1";
  p1=combo_multiGraph(c2_left,p1,dit,1,Form("usl"),cut,monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==2";
  p1=combo_multiGraph(c2_left,p1,dit,2,Form("usl"),cut,monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==3";
  p1=combo_multiGraph(c2_left,p1,dit,3,Form("usl"),cut,monitors5,36);
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  //p1=combo_multiGraph(c2_left,p1,dit,6,Form("us_avg"),cut,monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==1";
  p1=combo_segment_multiGraph(c2_left,p1,dit,4,Form("usl"),cut,"segment",monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==2";
  p1=combo_segment_multiGraph(c2_left,p1,dit,5,Form("usl"),cut,"segment",monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part==3";
  p1=combo_segment_multiGraph(c2_left,p1,dit,6,Form("usl"),cut,"segment",monitors5,36);
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  //p1=combo_segment_multiGraph(c2_left,p1,dit,3,Form("us_avg"),cut,"segment",monitors5,36);
  c2_left->cd();
  ana = "CREX Eigen Vector Monitor BMOD Slopes, Runwise and Runwise-Segment Averaged, usl, 3 parts of CREX"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c2_left->SaveAs(Form("%s",pdfname.Data()));
  c2_left->SaveAs(Form("5BPM_evMon_segmented_bmod_slopes%s.pdf",suffix.Data()));

  TCanvas* c2_right = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2_right->cd();
  c2_right->SetTitle(ana);
  c2_right->SetName(ana);
  c2_right->Divide(3,2);
  //cut = "run>6332 && run<7500";
  //for (Int_t j = 0 ; j < monitors5.size() ; j++) {
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==1";
  p1=combo_multiGraph(c2_right,p1,dit,1,Form("usr"),cut,monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==2";
  p1=combo_multiGraph(c2_right,p1,dit,2,Form("usr"),cut,monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==3";
  p1=combo_multiGraph(c2_right,p1,dit,3,Form("usr"),cut,monitors5,36);
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  //p1=combo_multiGraph(c2_right,p1,dit,6,Form("us_avg"),cut,monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==1";
  p1=combo_segment_multiGraph(c2_right,p1,dit,4,Form("usr"),cut,"segment",monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==2";
  p1=combo_segment_multiGraph(c2_right,p1,dit,5,Form("usr"),cut,"segment",monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part==3";
  p1=combo_segment_multiGraph(c2_right,p1,dit,6,Form("usr"),cut,"segment",monitors5,36);
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  //p1=combo_segment_multiGraph(c2_right,p1,dit,3,Form("us_avg"),cut,"segment",monitors5,36);
  c2_right->cd();
  ana = "CREX Eigen Vector Monitor BMOD Slopes, Runwise and Runwise-Segment Averaged, usr, 3 parts of CREX"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c2_right->SaveAs(Form("%s",pdfname.Data()));
  c2_right->SaveAs(Form("5BPM_evMon_segmented_bmod_slopes%s.pdf)",suffix.Data()));


  // FIXME Monitors COMBOs section
  TCanvas* c3 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c3->cd();
  c3->SetTitle(ana);
  c3->SetName(ana);
  c3->Divide(4,5);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500 && rcdb_run_type==1 && rcdb_run_flag==1";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    //p1=combo_segment_multiGraph(c3,p1,mini,0+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c3,p1,mini,4*j+1,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    //p1=combo_segment_multiGraph(c3,p1,mini,5+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c3,p1,mini,4*j+2,Form("mini_eigen_reg_5bpms_sorted_seg_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    //p1=combo_segment_multiGraph(c3,p1,mini,10+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c3,p1,mini,4*j+3,Form("mini_eigen_reg_5bpms_sorted_run_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    //p1=combo_segment_multiGraph(c3,p1,mini,10+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c3,p1,mini,4*j+4,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,devices5,36);
  }

  c3->cd();
  ana = "CREX Eigen Vector Monitor BPMs compositions, all averaging time scales"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c3->SaveAs(Form("%s",pdfname.Data()));
  c3->SaveAs(Form("5BPM_evMon_crex_parts_compositions%s.pdf(",suffix.Data()));

  TCanvas* c3_part = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c3_part->cd();
  c3_part->SetTitle(ana);
  c3_part->SetName(ana);
  c3_part->Divide(3,5);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500 && rcdb_run_type==1 && rcdb_run_flag==1";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==1";
    //p1=combo_segment_multiGraph(c3_part,p1,mini,0+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c3_part,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==2";
    //p1=combo_segment_multiGraph(c3_part,p1,mini,5+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c3_part,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==3";
    //p1=combo_segment_multiGraph(c3_part,p1,mini,10+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c3_part,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }

  c3_part->cd();
  ana = "CREX Eigen Vector Monitor BPMs compositions, 3 parts - averaging over the part, renorm the combos"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c3_part->SaveAs(Form("%s",pdfname.Data()));
  c3_part->SaveAs(Form("5BPM_evMon_crex_parts_compositions%s.pdf",suffix.Data()));

  TCanvas* c3_seg = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c3_seg->cd();
  c3_seg->SetTitle(ana);
  c3_seg->SetName(ana);
  c3_seg->Divide(3,5);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500 && rcdb_run_type==1 && rcdb_run_flag==1";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==1";
    p1=combo_multiGraph(c3_seg,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms_sorted_seg_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    //p1=combo_multiGraph(c3_seg,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==2";
    p1=combo_multiGraph(c3_seg,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms_sorted_seg_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    //p1=combo_multiGraph(c3_seg,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==3";
    p1=combo_multiGraph(c3_seg,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms_sorted_seg_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    //p1=combo_multiGraph(c3_seg,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }

  c3_seg->cd();
  ana = "CREX Eigen Vector Monitor BPMs compositions, 3 parts - averaging over the segment, renorm the combos"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c3_seg->SaveAs(Form("%s",pdfname.Data()));
  c3_seg->SaveAs(Form("5BPM_evMon_crex_parts_compositions%s.pdf",suffix.Data()));

  TCanvas* c3_run = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c3_run->cd();
  c3_run->SetTitle(ana);
  c3_run->SetName(ana);
  c3_run->Divide(3,5);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500 && rcdb_run_type==1 && rcdb_run_flag==1";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==1";
    p1=combo_multiGraph(c3_run,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms_sorted_run_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    //p1=combo_multiGraph(c3_run,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==2";
    p1=combo_multiGraph(c3_run,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms_sorted_run_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    //p1=combo_multiGraph(c3_run,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==3";
    p1=combo_multiGraph(c3_run,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms_sorted_run_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    //p1=combo_multiGraph(c3_run,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }

  c3_run->cd();
  ana = "CREX Eigen Vector Monitor BPMs compositions, 3 parts - averaging over the run, renorm the combos"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c3_run->SaveAs(Form("%s",pdfname.Data()));
  c3_run->SaveAs(Form("5BPM_evMon_crex_parts_compositions%s.pdf",suffix.Data()));

  TCanvas* c3_burst = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c3_burst->cd();
  c3_burst->SetTitle(ana);
  c3_burst->SetName(ana);
  c3_burst->Divide(3,5);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500 && rcdb_run_type==1 && rcdb_run_flag==1";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==1";
    p1=combo_multiGraph(c3_burst,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,devices5,36);
    //p1=combo_multiGraph(c3_burst,p1,mini,3*j+1,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==2";
    p1=combo_multiGraph(c3_burst,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,devices5,36);
    //p1=combo_multiGraph(c3_burst,p1,mini,3*j+2,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
    cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==3";
    p1=combo_multiGraph(c3_burst,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,devices5,36);
    //p1=combo_multiGraph(c3_burst,p1,mini,3*j+3,Form("mini_eigen_reg_5bpms_sorted_part_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }

  c3_burst->cd();
  ana = "CREX Eigen Vector Monitor BPMs compositions, 3 parts - burstwise, renorm the combos"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c3_burst->SaveAs(Form("%s)",pdfname.Data()));
  c3_burst->SaveAs(Form("5BPM_evMon_crex_parts_compositions%s.pdf)",suffix.Data()));

  // FIXME COMMENT March 15th
  /*
  TCanvas* c4 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c4->cd();
  c4->SetTitle(ana);
  c4->SetName(ana);
  c4->Divide(5,3);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500 && rcdb_run_type==1 && rcdb_run_flag==1";
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==1";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    //p1=combo_segment_multiGraph(c4,p1,mini,0+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c4,p1,mini,0+j+1,Form("mini_eigen_reg_5bpms_sorted_seg_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==2";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    //p1=combo_segment_multiGraph(c4,p1,mini,5+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c4,p1,mini,5+j+1,Form("mini_eigen_reg_5bpms_sorted_seg_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==3";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    //p1=combo_segment_multiGraph(c4,p1,mini,10+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c4,p1,mini,10+j+1,Form("mini_eigen_reg_5bpms_sorted_seg_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }

  c4->cd();
  ana = "CREX Eigen Vector Monitor BPMs compositions, 95 segments"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c4->SaveAs(Form("%s",pdfname.Data()));
  c4->SaveAs(Form("5BPM_evMon_crex_segs_compositions%s.pdf",suffix.Data()));


  TCanvas* c5 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c5->cd();
  c5->SetTitle(ana);
  c5->SetName(ana);
  c5->Divide(5,3);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500 && rcdb_run_type==1 && rcdb_run_flag==1";
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==1";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    //p1=combo_segment_multiGraph(c5,p1,mini,0+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c5,p1,mini,0+j+1,Form("mini_eigen_reg_5bpms_sorted_slug_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==2";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    //p1=combo_segment_multiGraph(c5,p1,mini,5+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c5,p1,mini,5+j+1,Form("mini_eigen_reg_5bpms_sorted_slug_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }
  cut = "rcdb_run_type==1 && rcdb_run_flag==1 && crex_part==3";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    //p1=combo_segment_multiGraph(c5,p1,mini,10+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"crex_part",devices5,36);
    p1=combo_multiGraph(c5,p1,mini,10+j+1,Form("mini_eigen_reg_5bpms_sorted_slug_avg.%s",monitors5.at(j).Data()),cut,devices5_mean,36);
  }

  c5->cd();
  ana = "CREX Eigen Vector Monitor BPMs compositions, 123 slugs"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c5->SaveAs(Form("%s",pdfname.Data()));
  c5->SaveAs(Form("5BPM_evMon_crex_slugs_compositions%s.pdf",suffix.Data()));

  TCanvas* c10 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c10->cd();
  c10->SetTitle(ana);
  c10->SetName(ana);
  c10->Divide(5,3);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  //cut = "run>6332 && run<7500 && rcdb_run_type==1 && rcdb_run_flag==1";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    p1=combo_segment_multiGraph(c10,p1,mini,0+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"dit_segment",devices5,36);
  }
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    p1=combo_segment_multiGraph(c10,p1,mini,5+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"rcdb_slug",devices5,36);
  }
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    p1=combo_segment_multiGraph(c10,p1,mini,10+j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,"run",devices5,36);
  }

  c10->cd();
  ana = "CREX Eigen Vector Monitor BPMs compositions"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c10->SaveAs(Form("%s",pdfname.Data()));
  c10->SaveAs(Form("5BPM_evMon_segmented_compositions%s.pdf",suffix.Data()));
  */

  /*
  TCanvas* c11 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c11->cd();
  c11->SetTitle(ana);
  c11->SetName(ana);
  c11->Divide(2,3);
  cut = "run>7500 && run!=7760 && rcdb_run_type==1 && rcdb_run_flag==1";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    p1=combo_segment_multiGraph(c11,p1,mini,j+1,Form("mini_eigen_reg_5bpms_sorted.%s",monitors5.at(j).Data()),cut,devices5,36);
  }
  c11->cd();
  ana = "CREX Part 3 Eigen Vector Monitor BPMs compositions"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c11->SaveAs(Form("%s",pdfname.Data()));
  c11->SaveAs(Form("5BPM_evMon_segmented_compositions%s).pdf",suffix.Data()));


  TCanvas* c12 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c12->cd();
  c12->SetTitle(ana);
  c12->SetName(ana);
  c12->Divide(3,6);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  for (Int_t j = 0 ; j < monitors.size()/2 ; j++) {
    p1=combo_segment_multiGraph(c12,p1,mini,3*j+1,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).Data()),cut,"dit_segment",X_BPMs,36);
    p1=combo_segment_multiGraph(c12,p1,mini,3*j+2,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).Data()),cut,"dit_segment",Y_BPMs,36);
    p1=combo_segment_multiGraph(c12,p1,mini,3*j+3,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).Data()),cut,"dit_segment",E_BPMs,36);
  }
  c12->cd();
  ana = "CREX All runs, 0-5 Eigen Vector Monitor BPMs us_avg slopes compositions (signed values)";
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c12->SaveAs(Form("%s",pdfname.Data()));
  c12->SaveAs(Form("AllBPM_evMon_segmented_compositions%s.pdf(",suffix.Data()));

  TCanvas* c13 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c13->cd();
  c13->SetTitle(ana);
  c13->SetName(ana);
  c13->Divide(3,6);
  cut = "rcdb_run_type==1 && rcdb_run_flag==1";
  for (Int_t j = monitors.size()/2 ; j < monitors.size() ; j++) {
    p1=combo_segment_multiGraph(c13,p1,mini,3*j+1-18,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).Data()),cut,"dit_segment",X_BPMs,36);
    p1=combo_segment_multiGraph(c13,p1,mini,3*j+2-18,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).Data()),cut,"dit_segment",Y_BPMs,36);
    p1=combo_segment_multiGraph(c13,p1,mini,3*j+3-18,Form("mini_eigen_reg_allbpms_sorted.%s",monitors.at(j).Data()),cut,"dit_segment",E_BPMs,36);
  }
  c13->cd();
  ana = "CREX All runs, 6-11 Eigen Vector Monitor BPMs us_avg slopes compositions (signed values)";
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c13->SaveAs(Form("%s)",pdfname.Data()));
  c13->SaveAs(Form("AllBPM_evMon_segmented_compositions%s.pdf)",suffix.Data()));
  */

}

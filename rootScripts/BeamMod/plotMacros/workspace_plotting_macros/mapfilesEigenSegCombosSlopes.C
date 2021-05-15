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
    Printf("set branch address %s to an integer",averaging.Data());
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

  for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    std::ofstream outfile0;
    outfile0.open(Form("maps/out_%s_%d.map",averaging.Data(),iter->first),std::ofstream::app);
    outfile0 << std::endl;
    if ( vs.at(0).Contains("evMon") ){
      outfile0 << "[asym:@eigen_dit_asym_" << draw.c_str() << "]" << std::endl;
    }
    else {
      outfile0 << "[asym:@dit_asym_" << draw.c_str() << "]" << std::endl;
    }
    outfile0 << "asym:" << draw.c_str() << ",1.0" << std::endl;
    outfile0.close();
  }
  for (Int_t i = vs.size()-1 ; i >= 0 ; i--) {
    Int_t local_totalEntries = 0;
    std::vector<Double_t> tmp_avg;
    std::map<Int_t,Double_t> tmp_avg_combo;
    for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    //for (Int_t iSeg = 0; iSeg<segments.size() ; iSeg++) 
      //Printf("Adding %d events for segment %d",iter->second,iter->first);
  //Printf("test5");
      // FIXME generate a mapout textfile
      std::ofstream outfile0;
      outfile0.open(Form("maps/out_%s_%d.map",averaging.Data(),iter->first),std::ofstream::app);
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
          outfile0 << "diff:";
          if (!vs.at(i).Contains("evMon")) {
            outfile0 << "bpm" << vs.at(i);
          }
          else {
            outfile0 << "diff_" << vs.at(i);
          }
          outfile0 << "," << 1e-3*-1.0*avg_tmp1 << std::endl;
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
      outfile0.close();
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

void mapfilesEigenSegCombosSlopes(TString out_ana = "evMon" , TString suffix = ""){ //TString input = "NULL",TString runNum = "", TString slugNum = "0"){}
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* plain_dit = new TChain("dit");
  TChain* AT_plain_dit = new TChain("dit");
  TChain* dit = new TChain("dit");
  TChain* AT_dit = new TChain("dit");
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
  AT_dit->AddFile(Form("march_17_rootfiles/AT-dit_alldet_slopes_run_avg_eigenvectors_sorted_part_avg_1X%s.root",suffix.Data()));
  plain_dit->AddFile(Form("march_17_rootfiles/dit_alldet_slopes_run_avg_1X%s.root",suffix.Data()));
  AT_plain_dit->AddFile(Form("march_17_rootfiles/AT-dit_alldet_slopes_run_avg_1X%s.root",suffix.Data()));
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
  AT_dit->BuildIndex("run","cyclenum");
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
  std::vector<TString> dit_plain_name_devices5 = {
    "1X",
    "4aY",
    "4eX",
    "4eY",
    "12X"
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


  TCanvas* c2 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2->Divide(1,1);
  TString localDraw = "usl";
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
  if (out_ana == "evMon") {
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("usl"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("atl1"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("atl2"),cut,"segment",monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("usr"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("atr1"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("atr2"),cut,"segment",monitors5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1";
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("sam1"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("sam2"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("sam3"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("sam4"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("sam5"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("sam6"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("sam7"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,dit,1,Form("sam8"),cut,"segment",monitors5,36);
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  //p1=combo_segment_multiGraph(c2,p1,dit,3,Form("us_avg"),cut,"segment",monitors5,36);
  // AT versions
  cut = "flag == 1 && rcdb_run_type==-1 && rcdb_run_flag==-1 && rcdb_arm_flag!=1";
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("usl"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("atl1"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("atl2"),cut,"segment",monitors5,36);
  cut = "flag == 1 && rcdb_run_type==-1 && rcdb_run_flag==-1 && rcdb_arm_flag!=2";
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("usr"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("atr1"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("atr2"),cut,"segment",monitors5,36);
  cut = "flag == 1 && rcdb_run_type==-1 && rcdb_run_flag==-1";
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("sam1"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("sam2"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("sam3"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("sam4"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("sam5"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("sam6"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("sam7"),cut,"segment",monitors5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_dit,1,Form("sam8"),cut,"segment",monitors5,36);
  }


  // Plain stuff
  if (out_ana == "plain") {
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("usl"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("atl1"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("atl2"),cut,"segment",dit_plain_name_devices5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("usr"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("atr1"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("atr2"),cut,"segment",dit_plain_name_devices5,36);
  cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1";
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("sam1"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("sam2"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("sam3"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("sam4"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("sam5"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("sam6"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("sam7"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,plain_dit,1,Form("sam8"),cut,"segment",dit_plain_name_devices5,36);
  //cut = "flag == 1 && rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag==0";
  //p1=combo_segment_multiGraph(c2,p1,plain_dit,3,Form("us_avg"),cut,"segment",dit_plain_name_devices5,36);
  // AT versions
  cut = "flag == 1 && rcdb_run_type==-1 && rcdb_run_flag==-1 && rcdb_arm_flag!=1";
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("usl"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("atl1"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("atl2"),cut,"segment",dit_plain_name_devices5,36);
  cut = "flag == 1 && rcdb_run_type==-1 && rcdb_run_flag==-1 && rcdb_arm_flag!=2";
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("usr"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("atr1"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("atr2"),cut,"segment",dit_plain_name_devices5,36);
  cut = "flag == 1 && rcdb_run_type==-1 && rcdb_run_flag==-1";
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("sam1"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("sam2"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("sam3"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("sam4"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("sam5"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("sam6"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("sam7"),cut,"segment",dit_plain_name_devices5,36);
  p1=combo_segment_multiGraph(c2,p1,AT_plain_dit,1,Form("sam8"),cut,"segment",dit_plain_name_devices5,36);
  }
}

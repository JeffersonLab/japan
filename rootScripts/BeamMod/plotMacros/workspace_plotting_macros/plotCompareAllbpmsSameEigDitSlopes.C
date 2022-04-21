TVirtualPad* combo_segment_compare_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw_reg = "cyclenum", std::string draw_dit = "cyclenum", TString cut_reg = "", TString cut_dit = "", TString averaging = "dit_segment", TString vs = "evMon0", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();
  Printf("test1");

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;
  TGraph *tmpG_dit;

  TString x_axis_name = "run+0.1*mini";
  TString x_axis_name_dit = "run";
  if ((TString)mini->GetName() == "dit") {
    x_axis_name = "run";
  }
  std::map<Int_t,Int_t> segments;
  Int_t segment;
  Int_t run_segment;
  Double_t double_segment;
  TString dit_averaging = "dit_segment";
  if (averaging == "dit_segment") {
    dit_averaging = "dit_segment";
  }
  else {
    dit_averaging = averaging;
  }
  if (((TString)averaging).Contains("segment")){
    mini->SetBranchAddress(Form("%s",averaging.Data()),&segment);
    mini->Draw(Form(">>elist_%s_%d",draw_reg.c_str(),subpad), Form("%s",cut_reg.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw_reg.c_str(),subpad));
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
    mini->SetBranchAddress(Form("%s",averaging.Data()),&run_segment);
    mini->Draw(Form(">>elist_%s_%d",draw_reg.c_str(),subpad), Form("%s",cut_reg.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw_reg.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(run_segment) == 0) {
        segments[run_segment] = 1;
      }
      else {
        segments[run_segment] = segments.at(run_segment) + 1;
      }
    }
    mini->SetEntryList(0);
    delete elist;
  } 
  else {
    mini->SetBranchAddress(Form("%s",averaging.Data()),&double_segment);
    mini->Draw(Form(">>elist_%s_%d",draw_reg.c_str(),subpad), Form("%s",cut_reg.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw_reg.c_str(),subpad));
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
  }

  std::vector<std::map<Int_t,Double_t>> avg_combos;
  std::vector<std::vector<Double_t>> avg_combos_miniruns;
  std::vector<std::vector<Double_t>> avg_combos_miniruns_dit;
  std::vector<Double_t>  xaxis;
  std::vector<Double_t>  xaxis_dit;
  Int_t nen=0;
  Int_t nen_dit=0;
  Int_t totalEntries;
  Int_t totalEntries_dit;

  //for (Int_t i = 0 ; i < vs.size() ; i++) {
    Int_t local_totalEntries = 0;
    Int_t local_totalEntries_dit = 0;
    std::vector<Double_t> tmp_avg;
    std::vector<Double_t> tmp_avg_dit;
    std::map<Int_t,Double_t> tmp_avg_combo;
    for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    //for (Int_t iSeg = 0; iSeg<segments.size() ; iSeg++) 
      //Printf("Adding %d events for segment %d",iter->second,iter->first);
      if (cut_reg == "") {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw_reg.c_str(),vs.at(i).Data(),i,iter->first),Form("dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw_reg.c_str(),vs.Data()),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",averaging.Data(),iter->first,averaging.Data(),iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw_reg.c_str(),vs.Data(),iter->first);
      }
      else {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw_reg.c_str(),vs.Data(),i,iter->first),Form("%s && dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",cut_reg.Data(),iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw_reg.c_str(),vs.Data()),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut_reg.Data(),averaging.Data(),iter->first,averaging.Data(),iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut %s && dit_segment==%d",nen,draw_reg.c_str(),vs.Data(),cut_reg.Data(),iter->first);
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

          if (cut_dit == "") {
            //nen_dit = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw_dit.c_str(),vs.at(i).Data(),i,iter->first),Form("dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",iter->first,iter->first));
            nen_dit = mini->Draw(Form("(%s_%s)",draw_dit.c_str(),vs.Data()),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first));
            //Printf("nen_dit = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen_dit,draw_dit.c_str(),vs.Data(),iter->first);
          }
          else {
            //nen_dit = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw_dit.c_str(),vs.Data(),i,iter->first),Form("%s && dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",cut_dit.Data(),iter->first,iter->first));
            //Printf("nen_dit = mini->Draw(Form(\"(%s_%s)\", cut = ,Form(\"%s && %s>=(%d-0.1) && %s<=(%d+0.1)\"",draw_dit.c_str(),vs.Data(),cut_dit.Data(),dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first);
            nen_dit = mini->Draw(Form("(%s_%s)",draw_dit.c_str(),vs.Data()),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut_dit.Data(),dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first));
            //Printf("nen_dit = %d, Draw run+0.1*mini:(%s_%s), cut_dit %s && dit_segment==%d",nen_dit,draw_dit.c_str(),vs.Data(),cut_dit.Data(),iter->first);
          }
          //local_totalEntries_dit += nen_dit;
          TH1* ah2;
          ah2 = (TH1*)(p1->FindObject("htemp"));
          Double_t avg_tmp1_dit = 0.0;
          if (!ah2 || nen_dit == 0) {
            avg_tmp1_dit = 0.0;
          }
          else {
            avg_tmp1_dit = (Double_t)(ah2->GetMean(1));
          }
          for (Int_t j = 0 ; j < nen_dit ; j++) {
            tmp_avg_dit.push_back(avg_tmp1_dit);
          }
          // Do X axis
          if (cut_reg == "") {
            nen = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name.Data(),0,iter->first),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",averaging.Data(),iter->first,averaging.Data(),iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw_reg.c_str(),vs.Data(),iter->first);
          }
          else {
            nen = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name.Data(),0,iter->first),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut_reg.Data(),averaging.Data(),iter->first,averaging.Data(),iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut_reg %s && dit_segment==%d",nen,draw_reg.c_str(),vs.Data(),cut_reg.Data(),iter->first);
          }
          std::vector<Double_t> tmp_xaxis(mini->GetV1(), mini->GetV1() + mini->GetSelectedRows() % mini->GetEstimate());
          for (Int_t j = 0 ; j < nen ; j++){
            tmp_avg.push_back(avg_tmp1);
          //  if (i == 0) {
              Double_t local_xaxis = 0.0;
              local_xaxis = tmp_xaxis.at(j);
              //Printf("Adding run+0.1*minirun = %0.1f",tmp_xaxis.at(j));
              xaxis.push_back(local_xaxis);
            //}
          }
          local_totalEntries += nen;
          // X axis for mini
          if (cut_dit == "") {
            nen_dit = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name_dit.Data(),0,iter->first),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first));
            //Printf("nen_dit = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen_dit,draw_dit.c_str(),vs.Data(),iter->first);
          }
          else {
            nen_dit = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name_dit.Data(),0,iter->first),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut_dit.Data(),dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first));
            //Printf("nen_dit = %d, Draw run+0.1*mini:(%s_%s), cut_dit %s && dit_segment==%d",nen_dit,draw_dit.c_str(),vs.Data(),cut_dit.Data(),iter->first);
          }
          std::vector<Double_t> tmp_xaxis_dit(mini->GetV1(), mini->GetV1() + mini->GetSelectedRows() % mini->GetEstimate());
          for (Int_t j = 0 ; j < nen_dit ; j++){
              Double_t local_xaxis = 0.0;
              local_xaxis = tmp_xaxis_dit.at(j);
              //Printf("Adding run+0.1*minirun = %0.1f",tmp_xaxis.at(j));
              xaxis_dit.push_back(local_xaxis);
            //}
          }
          local_totalEntries_dit += nen_dit;
          //ah1->Delete();
        }
        //else {
          //Printf("outside cut %s",cut_reg.Data());
        //}
      }
      else {
        //Printf("outside cut %s",cut_reg.Data());
      }
    }
  Printf("test6");
    totalEntries = local_totalEntries;
    totalEntries_dit = local_totalEntries_dit;
    avg_combos_miniruns.push_back(tmp_avg);
    avg_combos_miniruns_dit.push_back(tmp_avg_dit);
    avg_combos.push_back(tmp_avg_combo);
  //}
  //nen = mini->Draw(Form("run+0.1*mini"),cut_reg.Data(),"goff");
  //for (Int_t i = 0 ; i < vs.size() ; i++) {
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (totalEntries > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw_reg.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw_reg.c_str(),subpad));
  Printf("test7");
      tmpG = new TGraph(totalEntries,&xaxis[0],&(avg_combos_miniruns.at(0))[0]);
      tmpG_dit = new TGraph(totalEntries_dit,&xaxis_dit[0],&(avg_combos_miniruns_dit.at(0))[0]);
      //Printf("X axis has %d entries, Y has %d entries",xaxis.size(),avg_combos_miniruns.at(i).size());
      //tmpG = new TGraph(totalEntries,(const Double_t *)mini->GetV1(),(const Double_t *)(avg_combos_miniruns.at(i)));
      //tmpG = new TGraph(totalEntries.at(i),(const Double_t *)xaxis,(const Double_t *)(avg_combos_miniruns.at(i)));
      mgs.push_back(tmpG);
      mgs.push_back(tmpG_dit);
      tmpG->SetNameTitle(Form("%s_%s_%d_avg_combo_seg",vs.Data(),draw_reg.c_str(),subpad),Form("%s_%s_%d_avg",vs.Data(),draw_reg.c_str(),subpad));
      //if (cut_reg == "") {}
        tmpG->SetLineColor(startcolor);
        tmpG->SetMarkerColor(startcolor);
      tmpG_dit->SetNameTitle(Form("%s_%s_%d_avg_combo_seg_dit",vs.Data(),draw_dit.c_str(),subpad),Form("%s_%s_%d_avg",vs.Data(),draw_dit.c_str(),subpad));
      //if (cut_dit == "") {
        tmpG_dit->SetLineColor(startcolor+10);
        tmpG_dit->SetMarkerColor(startcolor+10);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.4);
      mg->Add(tmpG,"p");
      tmpG_dit->SetMarkerStyle(20);
      tmpG_dit->SetMarkerSize(0.4);
      mg->Add(tmpG_dit,"p");
      legend->AddEntry(tmpG,(TString)vs+" reg","lp");
      legend->AddEntry(tmpG_dit,(TString)vs+" lagr","lp");
    }
  //}

  Printf("test8");
  // Assume evMon are compositions, else doing a straight slope
  if (((TString)draw_reg).Contains("evMon")) {
  Printf("test9");
    mg->SetNameTitle(Form("%s BPM combination composition %s averaged %d %s_combo_seg",draw_reg.c_str(),averaging.Data(),subpad,cut_reg.Data()),Form("%s BPM combination composition %s averaged;Run+0.1*Minirun;%s composition %s averaged",draw_reg.c_str(),averaging.Data(),draw_reg.c_str(),averaging.Data()));
  }
  else {
  Printf("test10");
    mg->SetNameTitle(Form("%s slopes vs evMons, %s averaged %d %s_combo_seg",draw_reg.c_str(),averaging.Data(),subpad,cut_reg.Data()),Form("%s slopes vs, evMons, %s averaged;Run+0.1*Minirun;%s slopes averaged",draw_reg.c_str(),averaging.Data(),draw_reg.c_str()));
  }
  mg->Draw("alp");
  //if (subpad%3==0 || cut_reg=="rcdb_arm_flag==0") {
    legend->Draw();
  //}
  return p1;
}

void plotCompareAllbpmsSameEigDitSlopes(TString suffix = ""){ //TString input = "NULL",TString runNum = "", TString slugNum = "0"){}
  TString old_suffix = "";
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* mini = new TChain("mini");
  TChain* mini_eigen_reg_allbpms_sorted_part_avg    = new TChain(Form("mini_eigen_reg_allbpms_part_avg"));
  TChain* mini_eigen_lagr_allbpms_sorted_part_avg    = new TChain(Form("mini_eigen_lagr_allbpms_part_avg"));
  mini                                    ->  AddFile(Form("../respin2_data/eigen_noerr_part_avg_allbpms_lagr_slopes.root"));
  mini_eigen_reg_allbpms_sorted_part_avg  ->  AddFile(Form("../respin2_data/eigen_noerr_part_avg_allbpms_lagr_slopes.root"));
  mini_eigen_lagr_allbpms_sorted_part_avg ->  AddFile(Form("../respin2_data/eigen_noerr_part_avg_allbpms_lagr_slopes.root"));
  // Default outputs, unsorted:
  mini->AddFriend(mini_eigen_reg_allbpms_sorted_part_avg,"mini_eigen_reg_allbpms_part_avg");
  mini->AddFriend(mini_eigen_lagr_allbpms_sorted_part_avg,"mini_eigen_lagr_allbpms_part_avg");
  //mini_eigen_lagr_allbpms_sorted_part_avg->AddFriend(mini,"mini_base");
  //mini_eigen_lagr_allbpms_sorted_part_avg->BuildIndex("mini_base.run","mini_base.mini");
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
  std::vector<TString> monitors = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new","evMon5_new","evMon6_new","evMon7_new","evMon8_new","evMon9_new","evMon10_new","evMon11_new"};
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
  std::vector<TString> monitors5 = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new"};
  std::vector<TString> monitorsA = {"evMon0_new","evMon1_new","evMon2_new","evMon3_new","evMon4_new","evMon5_new"};
  std::vector<TString> monitorsB = {"evMon6_new","evMon7_new","evMon8_new","evMon9_new","evMon10_new","evMon11_new"};


  // FIXME Using no cut (rcdb_run_type or run_flag or dit_flag) assumes that the dataset being fed in is a GOOD PRODUCTION and suitable for application to BMOD segments as well. For proof of concept comparisons the cut may not be that important, as we just want to compare eigen reg to eigen dit, etc. but for final numbers this will really matter.
  TString cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1";
  TString cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1";
  // Do some Tree->SetAlias() around everywhere

  TString pdfname = Form("outputs_Segment_Avg_EigenCombos_compare%s.pdf",suffix.Data());

  TString ana = "Eigen BPMs combos";
  TText *label = new TText(0.0,0.005,ana);
  label->SetTextFont(23);
  label->SetTextSize(12);
  label->SetNDC();

  TVirtualPad* p1;

  // FIXME Eigen regression slopes section
  
  /*
  TCanvas* c1 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1->cd();
  //c1->SetTitle(ana);
  //c1->SetName(ana);
  c1->Divide(2,5);
  //cut = "run>6332 && run<7500";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
    p1=combo_segment_compare_multiGraph(c1,p1,mini,0+2*j+1,Form("1e3*mini_eigen_reg_allbpms_part_avg.usl"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usl"),cut_reg,cut_dit,"run",monitors5.at(j),36+2*j);
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
    p1=combo_segment_compare_multiGraph(c1,p1,mini,0+2*j+2,Form("1e3*mini_eigen_reg_allbpms_part_avg.usr"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usr"),cut_reg,cut_dit,"run",monitors5.at(j),36+2*j);
  }

  c1->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c1->SaveAs(Form("%s(",pdfname.Data()));
  c1->SaveAs(Form("august_30_plots/12BPM_evMon_segmented_eigen_reg_slopes_compare%s.pdf",suffix.Data()));

  TCanvas* c1_left = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1_left->cd();
  //c1_left->SetTitle(ana);
  //c1_left->SetName(ana);
  c1_left->Divide(3,5);
  //cut = "run>6332 && run<7500";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part == 1";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part == 1";
    p1=combo_segment_compare_multiGraph(c1_left,p1,mini,0+3*j+1,Form("1e3*mini_eigen_reg_allbpms_part_avg.usl"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usl"),cut_reg,cut_dit,"run",monitors5.at(j),36+2*j);
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part == 2";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part == 2";
    p1=combo_segment_compare_multiGraph(c1_left,p1,mini,0+3*j+2,Form("1e3*mini_eigen_reg_allbpms_part_avg.usl"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usl"),cut_reg,cut_dit,"run",monitors5.at(j),36+2*j);
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part == 3";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1 && crex_part == 3";
    p1=combo_segment_compare_multiGraph(c1_left,p1,mini,0+3*j+3,Form("1e3*mini_eigen_reg_allbpms_part_avg.usl"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usl"),cut_reg,cut_dit,"run",monitors5.at(j),36+2*j);
  }

  c1_left->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c1_left->SaveAs(Form("%s",pdfname.Data()));
  c1_left->SaveAs(Form("august_30_plots/12BPM_evMon_segmented_eigen_reg_slopes_compare_left%s.pdf",suffix.Data()));
  
  TCanvas* c1_right = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c1_right->cd();
  //c1_right->SetTitle(ana);
  //c1_right->SetName(ana);
  c1_right->Divide(3,5);
  //cut = "run>6332 && run<7500";
  for (Int_t j = 0 ; j < monitors5.size() ; j++) {
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part == 1";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part == 1";
    p1=combo_segment_compare_multiGraph(c1_right,p1,mini,0+3*j+1,Form("1e3*mini_eigen_reg_allbpms_part_avg.usr"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usr"),cut_reg,cut_dit,"run",monitors5.at(j),36+2*j);
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part == 2";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part == 2";
    p1=combo_segment_compare_multiGraph(c1_right,p1,mini,0+3*j+2,Form("1e3*mini_eigen_reg_allbpms_part_avg.usr"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usr"),cut_reg,cut_dit,"run",monitors5.at(j),36+2*j);
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part == 3";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2 && crex_part == 3";
    p1=combo_segment_compare_multiGraph(c1_right,p1,mini,0+3*j+3,Form("1e3*mini_eigen_reg_allbpms_part_avg.usr"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usr"),cut_reg,cut_dit,"run",monitors5.at(j),36+2*j);
  }

  c1_right->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c1_right->SaveAs(Form("%s",pdfname.Data()));
  c1_right->SaveAs(Form("august_30_plots/12BPM_evMon_segmented_eigen_reg_slopes_compare_right%s.pdf",suffix.Data()));
  */
  
  TCanvas* c2 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c2->cd();
  //c2->SetTitle(ana);
  //c2->SetName(ana);
  c2->Divide(2,6);
  //cut = "run>6332 && run<7500";
  for (Int_t j = 0 ; j < monitorsA.size() ; j++) {
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
    p1=combo_segment_compare_multiGraph(c2,p1,mini,0+2*j+1,Form("1e3*mini_eigen_reg_allbpms_part_avg.usl"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usl"),cut_reg,cut_dit,"dit_segment",monitorsA.at(j),36+2*j);
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
    p1=combo_segment_compare_multiGraph(c2,p1,mini,0+2*j+2,Form("1e3*mini_eigen_reg_allbpms_part_avg.usr"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usr"),cut_reg,cut_dit,"dit_segment",monitorsA.at(j),36+2*j);
  }

  c2->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c2->SaveAs(Form("august_30_plots/%s(",pdfname.Data()));
  c2->SaveAs(Form("august_30_plots/12BPM_evMon_segmented_eigen_reg_slopes_compare_segs%s.pdf(",suffix.Data()));

  TCanvas* c3 = new TCanvas();
  //TPad* cp1 = new TPad("test","",0,0.35,1,1);
  c3->cd();
  //c3->SetTitle(ana);
  //c3->SetName(ana);
  c3->Divide(2,6);
  //cut = "run>6332 && run<7500";
  for (Int_t j = 0 ; j < monitorsB.size() ; j++) {
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=1";
    p1=combo_segment_compare_multiGraph(c3,p1,mini,0+2*j+1,Form("1e3*mini_eigen_reg_allbpms_part_avg.usl"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usl"),cut_reg,cut_dit,"dit_segment",monitorsB.at(j),30+2*j);
    cut_reg = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
    cut_dit = "rcdb_run_type==1 && rcdb_run_flag==1 && rcdb_arm_flag!=2";
    p1=combo_segment_compare_multiGraph(c3,p1,mini,0+2*j+2,Form("1e3*mini_eigen_reg_allbpms_part_avg.usr"),Form("1e3*mini_eigen_lagr_allbpms_part_avg.usr"),cut_reg,cut_dit,"dit_segment",monitorsB.at(j),30+2*j);
  }

  c3->cd();
  ana = "CREX Eigen Vector Monitor Eigen Regression Slopes, Burstwise and Run and Segment Averaged"; // No more absolute value in this anymore (with the sorted slopes)
  label->SetText(0.0,0.005,ana);
  label->Draw("same");
  c3->SaveAs(Form("august_30_plots/%s)",pdfname.Data()));
  c3->SaveAs(Form("august_30_plots/12BPM_evMon_segmented_eigen_reg_slopes_compare_segs%s.pdf)",suffix.Data()));

}

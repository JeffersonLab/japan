#ifndef __CAMDF__
#define __CAMDF__
using namespace ROOT;

// Best in root >= 6.16

class Channel{
  public: 
    /* Minirun update:
    std::vector <TH1D> histos;
    TString name;
    TString draw;
    std::vector <Int_t> miniruns;
    std::vector <Double_t> means;
    std::vector <Double_t> meanErrs;
    std::vector <Double_t> rmss;
    std::vector <Double_t> rmsErrs;
    std::vector <Double_t> nentries;*/
    void getData();
    void getSlowData();
    void storeData(TTree *);
    ROOT::RDF::RResultPtr<TH1D> histo;
    TString name;
    TString branchName;
    TString type = "meanrms";
    TString draw;
    //Int_t minirun;
    Double_t singleEntry = -1.0e6; // For single entry getting
    Double_t slope = -1.0e6; // For Slope getting
    Double_t slopeError = -1.0e6;
    Double_t avg = -1.0e6; // For normal meanrms getting
    Double_t avgErr = -1.0e6;
    Double_t rms = -1.0e6;
    Double_t rmsErr = -1.0e6;
    Double_t nEntries = 0;
};
/* I would like to define the channels in a loop over input.txt and apply all filter/defines to dataframe, but I don't know the dataframe.filter's data type... just do it in line
void Channel::fill_channels(TString inputFile = "input.txt"){
  return;
}; */

void Channel::getData(){
  if (histo){
    avg = histo->GetMean();
    avgErr = histo->GetMeanError();
    rms = histo->GetRMS();
    rmsErr = histo->GetRMSError();
    nEntries = histo->GetEntries();
  }
  else {
    if (debug>1) {
      Printf("Error, histogram for %s empty",name.Data());
    }
  }
};

void Channel::getSlowData(){
  if (histo){
    singleEntry = histo->GetBinCenter(histo->GetMaximumBin());
  }
  else {
    if (debug>1) {
      Printf("Error, histogram for %s empty",name.Data());
    }
  }
};

void Channel::storeData(TTree * outputTree){
  if (type == "meanrms"){
    outputTree->Branch(Form("%s_mean",name.Data()),&avg);
    outputTree->Branch(Form("%s_mean_error",name.Data()),&avgErr);
    outputTree->Branch(Form("%s_rms",name.Data()),&rms);
    outputTree->Branch(Form("%s_rms_error",name.Data()),&rmsErr);
    outputTree->Branch(Form("%s_nentries",name.Data()),&nEntries);
  }
  if (type == "slow"){
    outputTree->Branch(Form("%s",name.Data()),&singleEntry);
  }
  if (type == "slopes"){
    outputTree->Branch(Form("%s_slope",name.Data()),&slope);
    outputTree->Branch(Form("%s_slope_error",name.Data()),&slopeError);
  }
};

class Source {
  public:
    TString run,nruns,split,minirun,input;
    Source(TString run_n, TString n_runs, TString n_minirun, TString n_split, TString in): run(run_n), nruns(n_runs), minirun(n_minirun), split(n_split), input(in) {}
    RDataFrame readSource();
    void printInfo() { std::cout << "Processing run  " << run  << ". " << std::endl;} 
    void drawAll();
    void getSlopes(std::vector<Channel>&, Int_t, Int_t, Int_t, Double_t);
    Channel GetChannelByName(TString name);    
};

void Source::getSlopes(std::vector<Channel> &channels, Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  Double_t data_slope = 0;
  Double_t data_slope_error = 0;
  Printf("Getting slopes");

  TString ditSlopeFileNamebase = gSystem->Getenv("DITHERING_ROOTFILES_SLOPES");
  TString ditSlopeFileName = ditSlopeFileNamebase + "/dit_slopes_slug" + nRuns + ".root";
  if( !gSystem->AccessPathName(ditSlopeFileName) ) {
    Printf("Getting dithering slopes from %s",ditSlopeFileName.Data());
    TChain *ditTree = new TChain("dit");
    ditTree->Add(ditSlopeFileName);
    TLeaf *ditRunNum = ditTree->GetLeaf("cycnum_usl");
    TObjArray *slopesList = ditTree->GetListOfLeaves();
    TString outname = "";
    /*for (Int_t a = 0; a<ditTree->GetEntries(); a++){
      ditTree->GetEntry(a);
      if (debug>3) Printf("Entry number %d, run number %d",a,(Int_t)ditRunNum->GetValue(0));
      TIter slopesIter(slopesList);
      while (TLeaf *slopes=(TLeaf*)slopesIter.Next()){
        if (debug>4) Printf("Checking dither slope %s",((TString)slopes->GetName()).Data());
        if ((TString)slopes->GetName() != "cycnum_usl" && (Int_t)ditRunNum->GetValue(0) == runNumber){
          outname = "dit_"+(TString)slopes->GetName()+"_slope";
          if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),(Double_t)slopes->GetValue(0));
          Channel tmpChan;
          tmpChan.type = "slopes";
          tmpChan.name = "dit_"+(TString)slopes->GetName();
          tmpChan.slope = (Double_t)slopes->GetValue(0);
          tmpChan.slopeError = 0.0; // Dithering has no errors?
          channels.push_back(tmpChan);
        }
      }
    }*/ // Loop Over Entries - Ideal per/cycle way of doing things FIXME
    TIter slopesIter(slopesList);
    while (TLeaf *slopes=(TLeaf*)slopesIter.Next()){
      if (debug>4) Printf("Checking dither slope %s",((TString)slopes->GetName()).Data());
      if ((TString)slopes->GetName() != "cycnum_usl"){
        ditTree->Draw(Form("%s",slopes->GetName()),"1==1","goff");
        TH1* tmpHist = (TH1*)gROOT->FindObject("htemp");
        outname = "dit_"+(TString)slopes->GetName()+"_slope";
        if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),(Double_t)tmpHist->GetMean());
        Channel tmpChan;
        tmpChan.type = "slopes";
        tmpChan.name = "dit_"+(TString)slopes->GetName();
        tmpChan.slope = (Double_t)tmpHist->GetMean();
        tmpChan.slopeError = (Double_t)tmpHist->GetMeanError(); // Dithering has no errors?
        channels.push_back(tmpChan);
      }
    }
  }
  TString ditRunSlopeFileNamebase = gSystem->Getenv("DITHERING_ROOTFILES_SLOPES_RUN");
  TString ditRunSlopeFileName = ditRunSlopeFileNamebase + "/dit_slopes_slug" + nRuns + ".root";
  if( !gSystem->AccessPathName(ditRunSlopeFileName) ) {
    Printf("Getting dithering slopes from %s",ditRunSlopeFileName.Data());
    TChain *ditTree = new TChain("dit");
    ditTree->Add(ditRunSlopeFileName);
    TLeaf *ditRunNum = ditTree->GetLeaf("run");
    TObjArray *slopesList = ditTree->GetListOfLeaves();
    TString outname = "";
    for (Int_t a = 0; a<ditTree->GetEntries(); a++){
      ditTree->GetEntry(a);
      if (debug>3) Printf("Entry number %d, run number %d",a,(Int_t)ditRunNum->GetValue(0));
      TIter slopesIter(slopesList);
      while (TLeaf *slopes=(TLeaf*)slopesIter.Next()){
        if (debug>4) Printf("Checking dither slope %s",((TString)slopes->GetName()).Data());
        if ((TString)slopes->GetName() != "run" && (Int_t)ditRunNum->GetValue(0) == runNumber){
          outname = "dit_"+(TString)slopes->GetName()+"_slope";
          if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),(Double_t)slopes->GetValue(0));
          Channel tmpChan;
          tmpChan.type = "slopes";
          tmpChan.name = "dit_run_"+(TString)slopes->GetName();
          tmpChan.slopeError = 0.0001; // Dithering has no errors?
          if (abs((Double_t)slopes->GetValue(0))<100.0) { 
            tmpChan.slope = (Double_t)slopes->GetValue(0);
            tmpChan.slopeError = -1.0e6;
          }
          channels.push_back(tmpChan);
        }
      }
    } // Loop Over Entries - Ideal per/cycle way of doing things FIXME
  }
  if(minirunNumber<0){
    TString lrbFileNameBase = gSystem->Getenv("LRB_ROOTFILES");
    TString lrbFileName = lrbFileNameBase + "/blueR"+runNumber+".000new.slope.root";  
    TFile f(lrbFileName);

    std::map<TString, Int_t> IVname;
    std::map<TString, Int_t> DVname;

    TH1D* m=(TH1D*) f.Get("IVname");
    for (auto i=0; i<m->GetEntries();i++){
      TString ivname=m->GetXaxis()->GetBinLabel(i+1);
      IVname[ivname]=i;
    }

    TH1D* n=(TH1D*) f.Get("DVname");
    for (auto i=0; i<n->GetEntries(); i++){
      TString dvname=n->GetXaxis()->GetBinLabel(i+1);
      DVname[dvname]=i;
    }


    TMatrixT<double> slopes=*(TMatrixT<double>*) f.Get("slopes");
    TMatrixT<double> sigSlopes=*(TMatrixT<double>*) f.Get("sigSlopes");
    for (auto& i: DVname){ 
      for (auto& j: IVname){
        Channel tmpChan;
        tmpChan.type = "slopes";
        tmpChan.name = "cor_"+i.first+"_"+j.first;
        tmpChan.slope = slopes(j.second,i.second);
        tmpChan.slopeError = sigSlopes(j.second,i.second);
        channels.push_back(tmpChan);
      }
    }
  }
  if (minirunNumber>=0){
    Printf("Getting postpan slopes");
    TString lpostpanFileNameBase = gSystem->Getenv("POSTPAN_ROOTFILES");
    TString lpostpanFileName = lpostpanFileNameBase + "/prexPrompt_" + runNumber + "_000_regress_postpan.root";
    TFile f(lpostpanFileName);

    TTreeReader theReader("mini", &f);
    TTreeReaderArray<double> slope(theReader,"coeff");
    TTreeReaderArray<double> slope_err(theReader,"err_coeff");

    std::vector<TString> ivname= *(std::vector<TString>*) f.Get("IVNames");
    std::vector<TString> dvname= *(std::vector<TString>*) f.Get("DVNames");
    std::vector<TString> comb;

    for (auto &i:dvname){
      for (auto &j:ivname){
        comb.push_back(i+"_"+j);
      }
    }

    Int_t mini=0;
    while(theReader.Next()){
      Int_t count=0;
      for (auto i=0; i<slope.GetSize(); i++){
        if (mini!=minirunNumber) continue;
        Channel tmpChan;
        tmpChan.type = "slopes";
        tmpChan.name = "reg_"+comb.at(count);
        tmpChan.slope = slope[i];
        tmpChan.slopeError = slope_err[i];
        channels.push_back(tmpChan);
        count++;
      }
      mini++; 
    }
  }	
};

RDataFrame Source::readSource(){
  EnableImplicitMT();

  /*--------------------------------------------*/

  TStopwatch tsw;
  TStopwatch tswAll;
  tsw.Start();
  tswAll.Start();
  if (debug > 1) { cout << "Beginning TChain Setup --"; tsw.Print(); cout << endl;}
  tsw.Start();
  TChain * mul_tree      = new TChain("mul");
  TChain * slow_tree     = new TChain("slow");
  TChain * reg_tree      = new TChain("reg");
  TChain * dit_tree      = new TChain("dit");
  TChain * mini_tree     = new TChain("mini");
  TChain * mulc_tree     = new TChain("mulc");
  TChain * mulc_lrb_tree = new TChain("mulc_lrb");

  mul_tree->Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root",run.Data(),split.Data()));
  slow_tree->Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root",run.Data(),split.Data()));
  reg_tree->Add(Form("/chafs2/work1/apar/postpan-outputs/prexPrompt_%s_%s_regress_postpan.root", run.Data(),split.Data()));
  TString ditheringFileNameDF = gSystem->Getenv("DITHERING_ROOTFILES");
  if (ditheringFileNameDF != ""){
    Printf("Looking for Dithering corrected files in %s",ditheringFileNameDF.Data());
    dit_tree->Add(Form("%s/prexPrompt_dither_%s_000.root", ditheringFileNameDF.Data(), run.Data()));
  }
  mini_tree->Add(Form("/chafs2/work1/apar/postpan-outputs/prexPrompt_%s_%s_regress_postpan.root", run.Data(),split.Data()));
  mulc_tree->Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root", run.Data(),split.Data()));
  mulc_lrb_tree->Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root", run.Data(),split.Data()));

  mul_tree->AddFriend(reg_tree);
  if (ditheringFileNameDF != ""){
    Printf("Obtained Dithering corrected files in %s",ditheringFileNameDF.Data());
    mul_tree->AddFriend(dit_tree);
  }
  mul_tree->AddFriend(mulc_tree);
  mul_tree->AddFriend(mulc_lrb_tree);

  //miniruns = mini_tree->Scan("minirun",""); // FIXME for later minirun looping addition

  RDataFrame d(*mul_tree);//,device_list);
  RDataFrame slow(*slow_tree);
  if (debug > 1) { cout << "Filtering through ErrorFlag==0 --"; tsw.Print(); cout << endl; }
  tsw.Start();
  auto d_good=d.Filter("reg.ok_cut==1");
  //if (ditheringFileNameDF != ""){
  //  d_good=d.Filter("dit.ErrorFlag==0");
  //}

  if (debug > 1) { cout << "Filtered through ErrorFlag==0 --"; tsw.Print(); cout << endl;}
  tsw.Start();
  //std::vector<TInterface<TFilterBase> ROOT::Experimental::TDF::TInterface< Proxied >::Filter> minirunCuts;
  //std::vector<ROOT::RDF::RInterface<Filter> minirunCuts;
  //for (Int_t k = 0; k<miniruns; k++) {
  //  minirunCuts.push_back(d_good.Filter(Form("reg.minirun==%d",k)));
  //}

  if (debug > 1) { cout << "Done with Tree cuts --"; tsw.Print(); cout << endl;}
  tsw.Start();

  std::vector<Channel> channels;

  // Getting device list
  string line;
  std::vector<string> tokens;
  string typedefault = "meanrms";
  ifstream infile(input);
  if (infile.is_open()){
    while(getline(infile,line)){
      string token;
      string name;
      string device;
      string type;
      std::istringstream tokenStream(line);
      while(getline(tokenStream,token,',')){
        tokens.push_back(token);
      }
      if (tokens.size() > 0){
        name = tokens.at(0);
      }
      if (tokens.size() > 1){
        device = tokens.at(1);
        if (name == "same"){
          name = device;
        }
      }
      else { 
        Printf("Error: invalid input file");
        return 1; 
      }
      if (tokens.size() > 2){
        type=tokens.at(2);
      }
      else {
        type=typedefault;
      }
      tokens.clear();
      Channel tmpChan;
      tmpChan.name = name;
      tmpChan.branchName = "agg_"+name;
      tmpChan.draw = device;
      tmpChan.type = type;
      try{
        if (minirun != "-1" && (tmpChan.type != "slopes" && tmpChan.type != "slow")){
          if (debug > 1) Printf("Executing \"tmpChan.histo = d_good.Define("+tmpChan.branchName+","+tmpChan.draw+").Filter(Form(\"reg.minirun==%s\".Histo1D("+tmpChan.branchName+")",minirun.Data());
          tmpChan.histo = d_good.Define(tmpChan.branchName.Data(),tmpChan.draw.Data()).Filter(Form("reg.minirun==%s",minirun.Data())).Histo1D(tmpChan.branchName.Data());
          channels.push_back(tmpChan);
          if (debug > 1) {cout << "Done Getting Histo1D for " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;}
          tsw.Start();
        }
        else if (tmpChan.type != "slopes" && tmpChan.type != "slow") {
          tmpChan.histo = d_good.Define(tmpChan.branchName.Data(),tmpChan.draw.Data()).Histo1D(tmpChan.branchName.Data());
          channels.push_back(tmpChan);
          if (debug > 1) {cout << "Done Getting Histo1D for " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;}
          tsw.Start();
        }
        if (tmpChan.type == "slow"){
          tmpChan.histo = slow.Histo1D(tmpChan.name.Data()); // FIXME want try/except to fill a histo that returns -1e6 mean -1e6 rms
          channels.push_back(tmpChan);
          if (debug > 1) {cout << "Done Getting Histo1D for " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;}
          tsw.Start();
        }
      }
      catch (...){
        Printf("Channel %s not available",tmpChan.draw.Data());
        channels.push_back(tmpChan);
      }
      if (tmpChan.type == "slopes") {
        getSlopes(channels, run.Atoi(), minirun.Atoi(), split.Atoi(), nruns.Atof());
        if (debug > 1) {cout << "Done Getting " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;}
        tsw.Start();
      }
    }
  }

  cout << "Done with getting All Histos --"; tsw.Print();
  tsw.Start();

  for (auto &tmpChan:channels){
    if (tmpChan.type=="meanrms"){
      tmpChan.getData();
      if (debug > 1) {cout<< tmpChan.name << " Mean = :" << tmpChan.avg<< " --"; tsw.Print(); cout <<std::endl;}
      tsw.Start();
    }
    if (tmpChan.type=="slow"){
      tmpChan.getSlowData();
      if (debug > 1) {cout<< tmpChan.name << " Single Entry = :" << tmpChan.singleEntry<< " --"; tsw.Print(); cout <<std::endl;}
      tsw.Start();
    }
    if (tmpChan.type=="slopes"){
      if (debug > 1) {cout<< tmpChan.name << " Slope = :" << tmpChan.slope<< " --"; tsw.Print(); cout <<std::endl;}
      tsw.Start();
    }
  }
  cout << "Done with getting data --"; tsw.Print(); cout << endl;
  tsw.Start();

  Double_t tmpRunN = run.Atof();
  Double_t tmpNRuns = nruns.Atof();
  Double_t tmpSplitN = split.Atof();
  Double_t tmpMinirunN = minirun.Atof();

  TString outputDir = getOutputDir_h();
  TString aggregatorFileName = Form("%s/aggregator.root",outputDir.Data()); // FIXME, this is very specific, and doesn't allow for aggregating over slugs, for instance
  // Store all trees
  if (tmpMinirunN <= -1) {
    aggregatorFileName = Form("%s/run_aggregator_%d.root",outputDir.Data(),(Int_t)tmpRunN);
  }
  else{
    aggregatorFileName = Form("%s/minirun_aggregator_%d_%d.root",outputDir.Data(),(Int_t)tmpRunN,(Int_t)tmpMinirunN);
  }
  TFile *aggregatorFile = new TFile(aggregatorFileName,"UPDATE");
  aggregatorFile->cd();
  TTree * outputTree = new TTree("agg","Aggregator Tree");
  // Intentionally not using a struct here to match prior aggregator output definition 
  // and to simplify life for other people, and to allow for non-mean kinds of variables to be used
  outputTree->Branch("run_number", &tmpRunN);
  outputTree->Branch("n_runs", &tmpNRuns);
  outputTree->Branch("split_n", &tmpSplitN);
  outputTree->Branch("minirun_n", &tmpMinirunN);

  if (debug > 1) {cout << "Done setting up output tree --"; tsw.Print(); cout << endl;}
  tsw.Start();

  //for (auto tmpChan:channels) {
  for (Int_t loop = 0 ; loop<channels.size() ; loop++) {
    channels.at(loop).storeData(outputTree);
  }

  outputTree->Fill();
  outputTree->Write("agg",TObject::kOverwrite);
  aggregatorFile->Close();

  cout << "Done with ALL, run " << run << " and minirun " << minirun << " --"; tswAll.Print(); cout << endl;
  tswAll.Start();
  return d;
}
#endif // __CAMDF__

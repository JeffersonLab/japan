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
    void storeData(TTree *);
    ROOT::RDF::RResultPtr<TH1D> histo;
    TString name;
    TString type = "meanrms";
    TString draw;
    //Int_t minirun;
    Double_t avg;
    Double_t avgErr;
    Double_t rms;
    Double_t rmsErr;
    Double_t nEntries;
};
/* I would like to define the channels in a loop over input.txt and apply all filter/defines to dataframe, but I don't know the dataframe.filter's data type... just do it in line
void Channel::fill_channels(TString inputFile = "input.txt"){
  return;
}; */

void Channel::getData(){
  avg = histo->GetMean();
  avgErr = histo->GetMeanError();
  rms = histo->GetRMS();
  rmsErr = histo->GetRMSError();
  nEntries = histo->GetEntries();
};

void Channel::storeData(TTree * outputTree){
  if (type=="meanrms"){
    outputTree->Branch(Form("%s_avg",name.Data()),&avg);
    outputTree->Branch(Form("%s_avg_error",name.Data()),&avgErr);
    outputTree->Branch(Form("%s_rms",name.Data()),&rms);
    outputTree->Branch(Form("%s_rms_error",name.Data()),&rmsErr);
    outputTree->Branch(Form("%s_nentries",name.Data()),&nEntries);
  }
};

class Source {
  public:
    TString run,nruns,split,minirun,input;
    Source(TString run_n, TString n_runs, TString n_minirun, TString n_split, TString in): run(run_n), nruns(n_runs), minirun(n_minirun), split(n_split), input(in) {}
    RDataFrame readSource();
    void printInfo() { std::cout << "Processing run  " << run  << ". " << std::endl;} 
    void drawAll();
    Channel GetChannelByName(TString name);    
};

RDataFrame Source::readSource(){
  EnableImplicitMT();

  /*--------------------------------------------*/

  TStopwatch tsw;
  TStopwatch tswAll;
  tsw.Start();
  tswAll.Start();
  cout << "Beginning TChain Setup --"; tsw.Print(); cout << endl;
  tsw.Start();
  TChain * mul_tree      = new TChain("mul");
  TChain * reg_tree      = new TChain("reg");
  TChain * mini_tree     = new TChain("mini");
  TChain * mulc_tree     = new TChain("mulc");
  TChain * mulc_lrb_tree = new TChain("mulc_lrb");

  mul_tree->Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root",run.Data(),split.Data()));
  reg_tree->Add(Form("/chafs2/work1/apar/postpan-outputs/prexPrompt_%s_%s_regress_postpan.root", run.Data(),split.Data()));
  mini_tree->Add(Form("/chafs2/work1/apar/postpan-outputs/prexPrompt_%s_%s_regress_postpan.root", run.Data(),split.Data()));
  mulc_tree->Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root", run.Data(),split.Data()));
  mulc_lrb_tree->Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root", run.Data(),split.Data()));

  mul_tree->AddFriend(reg_tree);
  mul_tree->AddFriend(mulc_tree);
  mul_tree->AddFriend(mulc_lrb_tree);

  //miniruns = mini_tree->Scan("minirun",""); // FIXME for later minirun looping addition

  RDataFrame d(*mul_tree);//,device_list);
  cout << "Filtering through reg.ok_cut==1 --"; tsw.Print(); cout << endl;
  tsw.Start();
  auto d_good=d.Filter("reg.ok_cut==1");

  cout << "Filtered through reg.ok_cut==1 --"; tsw.Print(); cout << endl;
  tsw.Start();
  //std::vector<TInterface<TFilterBase> ROOT::Experimental::TDF::TInterface< Proxied >::Filter> minirunCuts;
  //std::vector<ROOT::RDF::RInterface<Filter> minirunCuts;
  //for (Int_t k = 0; k<miniruns; k++) {
  //  minirunCuts.push_back(d_good.Filter(Form("reg.minirun==%d",k)));
  //}

  cout << "Done with Tree cuts --"; tsw.Print(); cout << endl;
  tsw.Start();

  //cout << "Executing Tree Traversal --"; tsw.Print(); cout << endl;
  //tsw.Start();
  //auto mean=d_good.Mean(); // This is non-lazy, avoid!

  //std::vector<ROOT::RDF::RResultPtr<TH1D>> histoVec;
  std::vector<Channel> channels;

  /* Old non-OOP way
  for (auto &device:device_list){
    //auto mean=d_good.Mean(device);
    //ROOT::RDF::RResultPtr<TH1D> tmpHisto1D = d_good.Histo1D(device);
    //tmpHisto1D->SetName(device);
    //histoVec.push_back(tmpHisto1D);
    string tmpStr(device.size(),'\0');
    std::replace_copy(device.begin(), device.end(),tmpStr.begin(),'.','_');
    cout << "Alias name = " << tmpStr << endl;
    // Replace this with a method that takes the number of miniruns, the minirunCuts list of TDFs, and the device_list info for Channel initialization
    // Maybe do histo2d here and splice the reg.minirun stuff internally while defining them in the dedicated method
    histoVec.push_back(d_good.Define(tmpStr+"_agg",device).Histo1D(tmpStr+"_agg"));//:reg.minirun")); // for 2D do this slice
    cout << "Done Getting Histo1D for " << device << " --"; tsw.Print(); cout << endl;
    tsw.Start();
    //    std::cout<< device << std::endl;
  }
  */

  /* Getting device list. This can be vectorised.*/
  string line;
  std::vector<string> tokens;
  string typedefault = "meanrms";
  ifstream infile("input.txt");
  if (infile.is_open()){
    while(getline(infile,line)){
      string token;
      string device;
      string type;
      std::istringstream tokenStream(line);
      while(getline(tokenStream,token,',')){
        tokens.push_back(token);
      }
      if (tokens.size() > 0){
        device = tokens.at(0);
      }
      else { 
        Printf("Error: invalid input file");
        return 1; 
      }
      if (tokens.size() > 1){
        type=tokens.at(1);
      }
      else {
        type=typedefault;
      }
      tokens.clear();
      string tmpStr(device.size(),'\0');
      std::replace_copy(device.begin(), device.end(),tmpStr.begin(),'.','_');
      cout << "Alias name = " << tmpStr << endl;
      Channel tmpChan;
      tmpChan.name = tmpStr;
      tmpChan.draw = device;
      if (minirun != "-1"){
        tmpChan.histo = d_good.Define(tmpStr,device).Filter(Form("reg.minirun==%s",minirun.Data())).Histo1D(tmpStr);
      }
      else {
        tmpChan.histo = d_good.Define(tmpStr,device).Histo1D(tmpStr);
      }
      channels.push_back(tmpChan);
      cout << "Done Getting Histo1D for " << device << " --"; tsw.Print(); cout << endl;
      tsw.Start();
    }
  }

  cout << "Done with getting All Histos --"; tsw.Print(); cout << endl << endl << endl;
  tsw.Start();

  for (auto &tmpChan:channels){
    tmpChan.getData();
    cout<< tmpChan.name << " Mean = :" << tmpChan.avg<< " --"; tsw.Print(); cout <<std::endl;
    tsw.Start();
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

  cout << "Done setting up output tree --"; tsw.Print(); cout << endl;
  tsw.Start();

  for (auto tmpChan:channels) {
    tmpChan.storeData(outputTree);
  }

  outputTree->Fill();
  outputTree->Write("agg");
  aggregatorFile->Close();

  cout << "Done with ALL, run " << run << " and minirun " << minirun << " --"; tswAll.Print(); cout << endl;
  tswAll.Start();
  return d;
}

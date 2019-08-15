using namespace ROOT;

// Best in root >= 6.16

class Channel{
  public: 
    std::vector <TH1D> histos;
    TString name;
    TString draw;
    std::vector <Int_t> miniruns;
    std::vector <Double_t> means;
    std::vector <Double_t> meanErrs;
    std::vector <Double_t> rmss;
    std::vector <Double_t> rmsErrs;
    std::vector <Double_t> nentries;
};

class Source {
  public:
    TString run,split,miniruns,input;
    Source(TString n_run, TString n_split, TString in): run(n_run), split(n_split), input(in) {}
    RDataFrame readSource();
    void printInfo() { std::cout << "Processing run  " << run  << ". " << std::endl;} 
    void drawAll();
    Channel GetChannelByName(TString name);    

};

RDataFrame Source::readSource(){
  EnableImplicitMT();

  /* Getting device list. This can be vectorised.*/
  std::vector<string> device_list;
  string device;
  ifstream infile("input.txt");
  Int_t count=0;
  if (infile.is_open()){
    while(getline(infile,device)){
      device_list.push_back(device);
      count++;
    }
  }
  /*--------------------------------------------*/


  TStopwatch tsw;
  tsw.Start();
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

  miniruns = mini_tree->Scan("minirun","");

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

  std::vector<ROOT::RDF::RResultPtr<TH1D>> histoVec;

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

  for (auto &analysis:histoVec){
    //auto mean=d_good.Mean(device);
    /* Manually, non-lazy
    auto histMean = d_good.Histo1D(device)->GetMean();
    auto histMeanErr = d_good.Histo1D(device)->GetMeanError();
    auto histRMS = d_good.Histo1D(device)->GetRMS();
    auto histRMSErr = d_good.Histo1D(device)->GetRMSError();
    auto histNentries = d_good.Histo1D(device)->GetEntries();
    */
    // Lazy
    auto histMean = analysis->GetMean();
    auto histMeanErr = analysis->GetMeanError();
    auto histRMS = analysis->GetRMS();
    auto histRMSErr = analysis->GetRMSError();
    auto histNentries = analysis->GetEntries();
    //cout << "Done Getting analysis results for " << analysis->GetName() << " --"; tsw.Print(); cout << std::endl;
    //tsw.Start();
    //std::cout<< "Mean = " << analysis->GetName() << ":" << histMean<< std::endl;
    cout<< "Mean = :" << histMean<< " --"; tsw.Print(); cout <<std::endl;
    tsw.Start();
  }
  cout << "Done with getting data --"; tsw.Print(); cout << endl;
  tsw.Start();

  //std::cout<< typeid(histMean).name() << std::endl;
  cout << "Done with printing data --"; tsw.Print(); cout << endl;
  tsw.Start();
  return d;

}

/*
   Int_t miniagg(TString runlist, TString varlist, TString output){

   TString filename1="/adaqfs/home/apar/pking/example_correlator_outputblueR1296new.slope.root";
   TString filename2="/chafs2/work1/apar/japanOutput/prexALL_2332.000.root";
   TString varname="";

   ROOT::RDataFrame d=ROOT::RDataFrame("mul",filename2.Data());

//displayColumns(d);
std::vector<TString> name;
name.push_back("asym_sam1");
name.push_back("asym_sam2");

std::vector<Float_t> mean;
std::vector<Float_t> err_mean;
std::vector<Float_t> rms;
std::vector<Float_t> err_rms;

for (auto it=name.begin();it!=name.end();it++){
auto h=d.Histo1D(Form("%s.hw_sum",(*it).Data());
mean.push_back(h->GetMean());
err_mean.push_back(h->GetMeanError());
rms.push_back(h->GetRMS());
err_rms.push_back(h->GetRMSError());
h->SetDirectory(0);
}

ROOT::RDataFrame summary(1);
auto newsum=summary.Define("h1", [&means]() {return means[0];});
auto newsum1=newsum.Define("h1_t", [&means]() {return means[1];});
auto newsum2=newsum1.Define("h2", [&means]() {return means[2];});
auto newsum3=newsum2.Define("h2_t", [&means]() {return means[3];});

newsum3.Snapshot("tree", "tree.root");

return 0;
}
*/

using namespace ROOT;


class Channel{


};

class Source {
  public:
    TString run,split,input;
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
  TStopwatch tswAll;
  tswAll.Start();
  cout << "Beginning TChain Setup --"; tsw.Print(); cout << endl;
  tsw.Start();
  TChain mul_tree("mul");
  TChain reg_tree("reg");
  TChain mulc_tree("mulc");
  TChain mulc_lrb_tree("mulc_lrb");

  mul_tree.Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root",run.Data(),split.Data()));
  reg_tree.Add(Form("/chafs2/work1/apar/postpan-outputs/prexPrompt_%s_%s_regress_postpan.root", run.Data(),split.Data()));
  mulc_tree.Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root", run.Data(),split.Data()));
  mulc_lrb_tree.Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root", run.Data(),split.Data()));

  mul_tree.AddFriend(&reg_tree);
  mul_tree.AddFriend(&mulc_tree);

  RDataFrame d(mul_tree,device_list);
  cout << "Filtering through reg.ok_cut==1 --"; tsw.Print(); cout << endl;
  tsw.Start();
  auto d_good=d.Filter("reg.ok_cut==1");

  cout << "Done with Tree cut --"; tsw.Print(); cout << endl;
  tsw.Start();

  //cout << "Executing Tree Traversal --"; tsw.Print(); cout << endl;
  //tsw.Start();
  //auto mean=d_good.Mean();

  std::vector<ROOT::RDF::RResultPtr<TH1D>> histoVec;

  for (auto &device:device_list){
    //auto mean=d_good.Mean(device.c_str());
    // Non-lazy, classic T->Draw() technique
    mul_tree.Draw(device.c_str(),"reg.ok_cut==1");
    /*
    ROOT::RDF::RResultPtr<TH1D> tmpHisto1D = d_good.Histo1D(device.c_str());
    tmpHisto1D->SetName(device.c_str());
    histoVec.push_back(tmpHisto1D);
    */
    //cout << "Done Getting Histo1D for " << device.c_str() << " --"; tsw.Print(); cout << endl;
    //tsw.Start();
    //cout << "Done Getting analysis for " << (Double_t)((TH1*)gROOT->FindObject("htemp"))->GetMean() << " --"; tsw.Print(); cout << endl;
    //tsw.Start();
  }

//  for (auto &analysis:histoVec){
//    //auto mean=d_good.Mean(device.c_str());
//    /* Manually, non-lazy
//    auto histMean = d_good.Histo1D(device.c_str())->GetMean();
//    auto histMeanErr = d_good.Histo1D(device.c_str())->GetMeanError();
//    auto histRMS = d_good.Histo1D(device.c_str())->GetRMS();
//    auto histRMSErr = d_good.Histo1D(device.c_str())->GetRMSError();
//    auto histNentries = d_good.Histo1D(device.c_str())->GetEntries();
//    */
//    // Lazy
//    auto histMean = analysis->GetMean();
//    auto histMeanErr = analysis->GetMeanError();
//    auto histRMS = analysis->GetRMS();
//    auto histRMSErr = analysis->GetRMSError();
//    auto histNentries = analysis->GetEntries();
//    cout << "Done Getting analysis results for " << analysis->GetName() << " --"; tsw.Print(); cout << std::endl;
//    tsw.Start();
//    std::cout<< "Mean = " << analysis->GetName() << ":" << histMean<< std::endl;
//  }
  cout << "Done with getting data --"; tsw.Print(); cout << endl;
  tsw.Start();

  //std::cout<< typeid(histMean).name() << std::endl;
  cout << "Done with printing data --"; tsw.Print(); cout << endl;
  tsw.Start();
  cout << "Done with ALL --"; tswAll.Print(); cout << endl;
  tswAll.Start();

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

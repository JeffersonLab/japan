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

time_t current=time(NULL);
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
printf("%s", ctime(&current));

TChain mul_tree("mul");
TChain reg_tree("reg");
TChain mulc_tree("mulc");

mul_tree.Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root",run.Data(),split.Data()));
reg_tree.Add(Form("/chafs2/work1/apar/postpan-outputs/prexPrompt_%s_%s_regress_postpan.root", run.Data(),split.Data()));
mulc_tree.Add(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_%s.%s.root", run.Data(),split.Data()));

mul_tree.AddFriend(&reg_tree);
mul_tree.AddFriend(&mulc_tree);

RDataFrame d(mul_tree,device_list);
auto d_good=d.Filter("ok_cut==1");

printf("%s", ctime(&current));

for (auto &device:device_list){
     auto mean=d_good.Mean(device.c_str());
     std::cout<< device.c_str() << ":" << *mean<< std::endl;
     printf("%s", ctime(&current));
}

//std::cout<< typeid(mean).name() << std::endl;
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

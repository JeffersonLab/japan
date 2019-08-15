#include "draw_miniagg.h"



int draw_miniagg(){


//TString mul= "/chafs2/work1/apar/japanOutput/prexPrompt_pass1_3454.000.root";
//TString reg="/chafs2/work1/apar/postpan-outputs/prexPrompt_3454_000_regress_postpan.root";

TString input= "input.txt";


Source source("3994","000",input.Data());  // These labels need to be passed as strings. Otherwise it's difficult to convert them back to filenames.
auto df_source=source.readSource();




return 0;
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

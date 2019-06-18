#include "TFile.h"
#include "TTree.h"


void displayColumns(ROOT::RDataFrame d){

auto colNames=d.GetColumnNames();
std::cout<< "hello";
for (auto &&colName:colNames){
     std::cout<<colName<<std::endl;
}

}


Int_t miniagg(TString runlist, TString varlist, TString output){

TString filename1="/adaqfs/home/apar/pking/example_correlator_outputblueR1296new.slope.root";
TString filename2="/chafs2/work1/apar/japanOutput/prexALL_2332.000.root";
TString varname="";
Int_t nRuns=20;

ROOT::RDataFrame d=ROOT::RDataFrame("mul",filename2.Data());


std::vector<TString> name;
name.push_back("asym_sam1");
name.push_back("asym_sam2");

std::vector<Float_t> means;

for (auto it=name.begin();it!=name.end();it++){
Float_t sum=0;
Int_t n=0;
d.Foreach([&sum, &n](double b){++n; sum+=b; }, {Form("%s.hw_sum",(*it).Data())});
means.push_back(sum/n);
}


ROOT::RDataFrame summary(1);
auto newsum=summary.Define(name[0].Data(), [&means]() {return means[0];});
  
newsum=newsum.Define(name[1].Data(), [&means]() {return means[1];});

newsum.Snapshot("tree", "tree.root");

return 0;
}


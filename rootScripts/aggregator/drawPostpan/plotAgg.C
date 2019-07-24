#include "plotAgg.h"
#include "TTree.h"

void plotAgg(TString filename, TString output){

Source source(filename, "agg");
source.printInfo();
source.drawAll();
TCanvas c("c", "c", 800,600);
c.Print(output+".pdf[", "pdf");
std::ofstream out;
out.open(Form("%s.txt",output.Data()));
out << Form("%32s \t %32s \t %32s", "Device", "Value", "Value Error") <<endl;
for (auto iter=source.list.begin(); iter!= source.list.end(); iter++){
  
    iter->draw(source.T,output);
    TString summary= Form("%32s \t %32.8f \t %32.8f\n", (iter->name).Data(), iter->value, iter->value_err);
    out<< summary.Data() ;

}

c.Print(output+".pdf]","pdf");
out.close();
std::cout<< "All done drawing " << source.list.size() << "channels";

}

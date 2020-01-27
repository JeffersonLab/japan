#include "grandAgg.h"
#include "TTree.h"


void grandAgg(TString filename, TString output, Int_t min = 0, Int_t max = 10000){

Source source(filename, "agg",output);
source.printInfo();
TCanvas c("c", "c", 800,600);
c.Print(output+".pdf[", "pdf");
if (min !=0 || max != 10000) {
  source.drawAllRange(min,max);
}
else{
  source.drawAll();
}

std::ofstream out;
out.open(Form("%s.txt",output.Data()));   
out<< "Units:\n";
out<< "Yields: bpm (mm), bcm (uA), sam/det (mV_uA)\n";
out<< "Asym: bcm/sam/det mean (ppb), bcm/sam/det width (ppm) Diff: bpm (nm) \n"; 
out << "Slopes: All Devices (ppb/nm) \n"; 
out << "======\n"<< "Yields\n" << "======\n";



for (auto iter=source.list.begin(); iter!= source.list.end(); iter++){
  
    auto name=iter->name;
    if (!name.Contains("yield_")) continue;    
    
    if(name.Contains("_mean")){
    name.ReplaceAll("_mean","");
    Channel rms=source.GetChannelByName(name+"_rms"); 
    TString summary= Form("%28s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f\n", name.Data(), iter->value, iter->value_err,rms.value, rms.value_err);
    
    out<< summary.Data() ;
    }
}

out << "=====================\n"<< "Asymmetry/Differences\n" << "=====================\n";
for (auto iter=source.list.begin();iter!=source.list.end(); iter++){

    auto name=iter->name;
    if(name.Contains("_slope")||name.Contains("_pslope")) continue;
    if(!name.Contains("asym_")&&!name.Contains("diff_")) continue;
    if(name.Contains("_avg") || name.Contains("_da") || name.Contains("_dd")) continue;
    if(name.Contains("reg_") || name.Contains("correction_")) continue;
    if(name.Contains("_mean")){
      name.ReplaceAll("_mean", "");
      Channel rms=source.GetChannelByName(name+"_rms");
      TString summary = Form("%28s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f\n", name.Data(), iter->value, iter->value_err,rms.value, rms.value_err);
      
      out<< summary.Data();
    }
}


out << "=================\n"<<"Combined Elements\n"<<"=================\n";

for (auto iter=source.list.begin();iter!=source.list.end(); iter++){
    auto name=iter->name;
    if(name.Contains("_slope")||name.Contains("_pslope")) continue;
    if(!name.Contains("_avg")&&!name.Contains("_da")&&!name.Contains("_dd")) continue;
    if(name.Contains("reg_")||name.Contains("correction_")) continue;  
    if(name.Contains("_mean")){
      name.ReplaceAll("_mean", "");
      Channel rms=source.GetChannelByName(name+"_rms");
      TString summary = Form("%28s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f\n", name.Data(), iter->value, iter->value_err,rms.value, rms.value_err);
      out<< summary.Data();
    }
}








out << "======================================\n"<<"Japan Regressed Asymmetry/Difference\n" << "=====================================\n";
for (auto iter=source.list.begin();iter!=source.list.end(); iter++){
    auto name=iter->name;
    if (name.Contains("_slope")||name.Contains("_pslope")) continue;
    if(!name.Contains("cor_")) continue;
    if(name.Contains("_mean")){
      name.ReplaceAll("_mean", "");
      Channel rms=source.GetChannelByName(name+"_rms");
      TString summary = Form("%28s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f\n", name.Data(), iter->value, iter->value_err,rms.value, rms.value_err);
      out<< summary.Data();
    }
}

out << "=======================================\n"<<"Postpan Regressed Asymmetry/Difference\n" << "=======================================\n";
for (auto iter=source.list.begin();iter!=source.list.end(); iter++){
    auto name=iter->name;
    if (name.Contains("_slope")||name.Contains("_pslope")) continue;
    if(!name.Contains("reg_asym_")) continue;
    if(name.Contains("_mean")){
      name.ReplaceAll("_mean", "");
      Channel rms=source.GetChannelByName(name+"_rms");
      TString summary = Form("%28s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f\n", name.Data(), iter->value, iter->value_err,rms.value, rms.value_err);
      out<< summary.Data();
    }
}


out << "=======================================\n"<<"Postpan Asymmetry/Difference Corrections\n" << "=======================================\n";
for (auto iter=source.list.begin();iter!=source.list.end(); iter++){
    auto name=iter->name;
    if(name.Contains("_slope")||name.Contains("_pslope")) continue;
    if(!name.Contains("correction_")) continue;
    if(name.Contains("_mean")){
      name.ReplaceAll("_mean", "");
      Channel rms=source.GetChannelByName(name+"_rms");
      TString summary = Form("%28s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f\n", name.Data(), iter->value, iter->value_err,rms.value, rms.value_err);
      out<< summary.Data();
    }
}


out << "=======================================\n"<<"Japan Slopes\n" << "=======================================\n";
for (auto iter=source.list.begin();iter!=source.list.end(); iter++){
    auto name=iter->name;
    if(!name.Contains("_slope")) continue;
    if (name.Contains("reg_")) continue;
    name.ReplaceAll("_slope", "");
    TString summary = Form("%28s| \t Mean:%12.4f+/-%12.4f\n", name.Data(), iter->value, iter->value_err);
    out<< summary.Data();
    
}


out << "=======================================\n"<<"Postpan Slopes\n" << "=======================================\n";
for (auto iter=source.list.begin();iter!=source.list.end(); iter++){
    auto name=iter->name;
    if(!name.Contains("_slope")) continue;
    if (!name.Contains("reg_")) continue;
    name.ReplaceAll("_slope", "");
    TString summary = Form("%45s| \t Mean:%12.4f+/-%12.4f\n", name.Data(), iter->value, iter->value_err);
    out<< summary.Data();
    
}






c.Print(output+".pdf]","pdf");
out.close();
std::cout<< "All done drawing " << source.list.size() << "channels" << std::endl;

}

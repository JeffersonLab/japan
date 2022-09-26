#include "plotAgg.h"
#include "TTree.h"


void plotAgg(TString filename, TString output, Int_t slug, Int_t ihwp, Int_t wein, Int_t hrs){

Source source(filename, "agg",output, slug, ihwp, wein, hrs);
source.printInfo();
TCanvas c("c", "c", 800,600);
c.Print(output+slug+".pdf[", "pdf");
source.drawAll();


std::ofstream out;
std::ofstream outLinear;
std::map<Int_t,TString> map_ihwp={ {1,"In"}, {2,"Out"} };
std::map<Int_t, TString> map_wein={ {1,"Right"}, {2,"Left"} };
std::map<Int_t, TString> map_hrs={ {0, "Both"}, {1,"Right Only"}, {2,"Left Only"} };
out.open(Form("%s%i.txt",output.Data(),slug));
outLinear.open(Form("%s_linear%i.txt",output.Data(),slug));
out<< "Slug: "<< slug<<"\t IHWP: "<<map_ihwp[ihwp]<<"\t Wein: "<< map_wein[wein]<< "\t HRS: "<<map_hrs[hrs]<< "\n";   
outLinear<<"Slug\t"<<slug<<"\n";
outLinear<<"IHWP\t"<<map_ihwp[ihwp]<<"\n";
outLinear<<"Wein\t"<<map_wein[wein]<<"\n";
outLinear<<"HRS\t"<<map_hrs[hrs]<<"\n";   
out<< "Units:\n";
out<< "Yields: bpm (mm), bcm (uA), sam/det (mV_uA)\n";
out<< "Asym: bcm/sam/det mean (ppb), bcm/sam/det width (ppm) Diff: bpm (nm) \n"; 
out << "Slopes: All Devices (ppb/nm) \n"; 
out << "======\n"<< "Yields\n" << "======\n";



for (auto iter=source.list.begin(); iter!= source.list.end(); iter++){
  
    auto linearname=iter->name;
    
    if(linearname.Contains("_mean")){
    linearname.ReplaceAll("_mean","");
    Channel linearrms=source.GetChannelByName(linearname+"_rms"); 
    TString linearSummary= Form("%28s\t%12.4f\t%12.4f\t%12.4f\t%12.4f\n", linearname.Data(), iter->value, iter->value_err,linearrms.value, linearrms.value_err);
    
    outLinear<<linearSummary.Data() ;
    }

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






c.Print(output+slug+".pdf]","pdf");
out.close();
outLinear.close();
std::cout<< "All done drawing " << source.list.size() << "channels" << std::endl;

}

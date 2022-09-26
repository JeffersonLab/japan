#include "grandAgg.h"
#include "TTree.h"


void grandAgg(TString filename, TString output, Int_t signCorrect = 0, Int_t min = 0, Int_t max = 10000){

Source source(filename, "agg",output,signCorrect);
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
std::ofstream outLinear;
std::map<Int_t,TString> map_ihwp={ {1,"In"}, {2,"Out"} };
std::map<Int_t, TString> map_wein={ {1,"Right"}, {2,"Left"} };
std::map<Int_t, TString> map_hrs={ {0, "Both"}, {1,"Right Only"}, {2,"Left Only"} };
out.open(Form("%s.txt",output.Data()));   
outLinear.open(Form("%s_linear.txt",output.Data()));
out<< "Units:\n";
out<< "Yields: bpm (mm), bcm (uA), sam/det (mV_uA)\n";
out<< "Asym: bcm/sam/det mean (ppb), bcm/sam/det width (ppm) Diff: bpm (nm) \n"; 
out << "Slopes: All Devices (ppb/nm) \n"; 
out << "======\n"<< "Yields\n" << "======\n";

outLinear<< "Units:\n";
outLinear<< "Yields: bpm (mm), bcm (uA), sam/det (mV_uA)\n";
outLinear<< "Asym: bcm/sam/det mean (ppb), bcm/sam/det width (ppm) Diff: bpm (nm) \n"; 
outLinear << "Table format: Name\tRange\tmean\tmean_err\trms\trms_err\tmean_chi2\tmean_ndof\n";


for (auto iter=source.list.begin(); iter!= source.list.end(); iter++){
  
    auto linearname=iter->name;
    
    if(linearname.Contains("_mean")){
      linearname.ReplaceAll("_mean","");
      Channel linearrms=source.GetChannelByName(linearname+"_rms"); 
      //TString linearSummary= Form("%28s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\n", linearname.Data(), iter->value, iter->value_err,linearrms.value, linearrms.value_err);
      TString linearSummary= Form("%28s\t%10s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\t%3e\t%5d\n", linearname.Data(),"ALL      ", iter->value, iter->value_err,linearrms.value, linearrms.value_err,iter->value_Chi2,iter->value_NDOF);
      linearSummary       += Form("%28s\t%10s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\t%3e\t%5d\n", "","WIEN L   ", iter->value_L, iter->value_L_err,linearrms.value_L, linearrms.value_L_err,iter->value_L_Chi2,iter->value_L_NDOF);
      linearSummary       += Form("%28s\t%10s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\t%3e\t%5d\n", "","WIEN R   ", iter->value_R, iter->value_R_err,linearrms.value_R, linearrms.value_R_err,iter->value_R_Chi2,iter->value_R_NDOF);
      linearSummary       += Form("%28s\t%10s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\t%3e\t%5d\n", "","IHWP IN  ", iter->value_IN, iter->value_IN_err,linearrms.value_IN, linearrms.value_IN_err,iter->value_IN_Chi2,iter->value_IN_NDOF);
      linearSummary       += Form("%28s\t%10s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\t%3e\t%5d\n", "","IHWP OUT ", iter->value_OUT, iter->value_OUT_err,linearrms.value_OUT, linearrms.value_OUT_err,iter->value_OUT_Chi2,iter->value_OUT_NDOF);
      linearSummary       += Form("%28s\t%10s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\t%3e\t%5d\n", "","IN Left  ", iter->value_L_IN, iter->value_L_IN_err,linearrms.value_L_IN, linearrms.value_L_IN_err,iter->value_L_IN_Chi2,iter->value_L_IN_NDOF);
      linearSummary       += Form("%28s\t%10s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\t%3e\t%5d\n", "","OUT Left ", iter->value_L_OUT, iter->value_L_OUT_err,linearrms.value_L_OUT, linearrms.value_L_OUT_err,iter->value_L_OUT_Chi2,iter->value_L_OUT_NDOF);
      linearSummary       += Form("%28s\t%10s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\t%3e\t%5d\n", "","IN Right ", iter->value_R_IN, iter->value_R_IN_err,linearrms.value_R_IN, linearrms.value_R_IN_err,iter->value_R_IN_Chi2,iter->value_R_IN_NDOF);
      linearSummary       += Form("%28s\t%10s\t%12.3f\t%12.3f\t%12.3f\t%12.3f\t%3e\t%5d\n", "","OUT Right", iter->value_R_OUT, iter->value_R_OUT_err,linearrms.value_R_OUT, linearrms.value_R_OUT_err,iter->value_R_OUT_Chi2,iter->value_R_OUT_NDOF);

      outLinear<<linearSummary.Data() ;
    }

    auto name=iter->name;

    if (!name.Contains("yield_")) continue;    
    
    if(name.Contains("_mean")){
    name.ReplaceAll("_mean","");
    Channel rms=source.GetChannelByName(name+"_rms"); 
    TString summary= Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", name.Data(),"ALL       ", iter->value, iter->value_err,rms.value, rms.value_err,iter->value_Chi2,iter->value_NDOF);
    summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN L    ",iter->value_L, iter->value_L_err,rms.value_L, rms.value_L_err,iter->value_L_Chi2,iter->value_L_NDOF);
    summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN R    ",iter->value_R, iter->value_R_err,rms.value_R, rms.value_R_err,iter->value_R_Chi2,iter->value_R_NDOF);
    summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP IN   ",iter->value_IN, iter->value_IN_err,rms.value_IN, rms.value_IN_err,iter->value_IN_Chi2,iter->value_IN_NDOF);
    summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP OUT  ",iter->value_OUT, iter->value_OUT_err,rms.value_OUT, rms.value_OUT_err,iter->value_OUT_Chi2,iter->value_OUT_NDOF);
    summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Left   ",iter->value_L_IN, iter->value_L_IN_err,rms.value_L_IN, rms.value_L_IN_err,iter->value_L_IN_Chi2,iter->value_L_IN_NDOF);
    summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Left  ",iter->value_L_OUT, iter->value_L_OUT_err,rms.value_L_OUT, rms.value_L_OUT_err,iter->value_L_OUT_Chi2,iter->value_L_OUT_NDOF);
    summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Right  ",iter->value_R_IN, iter->value_R_IN_err,rms.value_R_IN, rms.value_R_IN_err,iter->value_R_IN_Chi2,iter->value_R_IN_NDOF);
    summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Right ",iter->value_R_OUT, iter->value_R_OUT_err,rms.value_R_OUT, rms.value_R_OUT_err,iter->value_R_OUT_Chi2,iter->value_R_OUT_NDOF);
    
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
      TString summary= Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", name.Data(),"ALL       ", iter->value, iter->value_err,rms.value, rms.value_err,iter->value_Chi2,iter->value_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN L    ",iter->value_L, iter->value_L_err,rms.value_L, rms.value_L_err,iter->value_L_Chi2,iter->value_L_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN R    ",iter->value_R, iter->value_R_err,rms.value_R, rms.value_R_err,iter->value_R_Chi2,iter->value_R_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP IN   ",iter->value_IN, iter->value_IN_err,rms.value_IN, rms.value_IN_err,iter->value_IN_Chi2,iter->value_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP OUT  ",iter->value_OUT, iter->value_OUT_err,rms.value_OUT, rms.value_OUT_err,iter->value_OUT_Chi2,iter->value_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Left   ",iter->value_L_IN, iter->value_L_IN_err,rms.value_L_IN, rms.value_L_IN_err,iter->value_L_IN_Chi2,iter->value_L_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Left  ",iter->value_L_OUT, iter->value_L_OUT_err,rms.value_L_OUT, rms.value_L_OUT_err,iter->value_L_OUT_Chi2,iter->value_L_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Right  ",iter->value_R_IN, iter->value_R_IN_err,rms.value_R_IN, rms.value_R_IN_err,iter->value_R_IN_Chi2,iter->value_R_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Right ",iter->value_R_OUT, iter->value_R_OUT_err,rms.value_R_OUT, rms.value_R_OUT_err,iter->value_R_OUT_Chi2,iter->value_R_OUT_NDOF);
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
      TString summary= Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", name.Data(),"ALL       ", iter->value, iter->value_err,rms.value, rms.value_err,iter->value_Chi2,iter->value_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN L    ",iter->value_L, iter->value_L_err,rms.value_L, rms.value_L_err,iter->value_L_Chi2,iter->value_L_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN R    ",iter->value_R, iter->value_R_err,rms.value_R, rms.value_R_err,iter->value_R_Chi2,iter->value_R_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP IN   ",iter->value_IN, iter->value_IN_err,rms.value_IN, rms.value_IN_err,iter->value_IN_Chi2,iter->value_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP OUT  ",iter->value_OUT, iter->value_OUT_err,rms.value_OUT, rms.value_OUT_err,iter->value_OUT_Chi2,iter->value_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Left   ",iter->value_L_IN, iter->value_L_IN_err,rms.value_L_IN, rms.value_L_IN_err,iter->value_L_IN_Chi2,iter->value_L_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Left  ",iter->value_L_OUT, iter->value_L_OUT_err,rms.value_L_OUT, rms.value_L_OUT_err,iter->value_L_OUT_Chi2,iter->value_L_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Right  ",iter->value_R_IN, iter->value_R_IN_err,rms.value_R_IN, rms.value_R_IN_err,iter->value_R_IN_Chi2,iter->value_R_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Right ",iter->value_R_OUT, iter->value_R_OUT_err,rms.value_R_OUT, rms.value_R_OUT_err,iter->value_R_OUT_Chi2,iter->value_R_OUT_NDOF);
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
      TString summary= Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", name.Data(),"ALL       ", iter->value, iter->value_err,rms.value, rms.value_err,iter->value_Chi2,iter->value_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN L    ",iter->value_L, iter->value_L_err,rms.value_L, rms.value_L_err,iter->value_L_Chi2,iter->value_L_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN R    ",iter->value_R, iter->value_R_err,rms.value_R, rms.value_R_err,iter->value_R_Chi2,iter->value_R_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP IN   ",iter->value_IN, iter->value_IN_err,rms.value_IN, rms.value_IN_err,iter->value_IN_Chi2,iter->value_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP OUT  ",iter->value_OUT, iter->value_OUT_err,rms.value_OUT, rms.value_OUT_err,iter->value_OUT_Chi2,iter->value_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Left   ",iter->value_L_IN, iter->value_L_IN_err,rms.value_L_IN, rms.value_L_IN_err,iter->value_L_IN_Chi2,iter->value_L_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Left  ",iter->value_L_OUT, iter->value_L_OUT_err,rms.value_L_OUT, rms.value_L_OUT_err,iter->value_L_OUT_Chi2,iter->value_L_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Right  ",iter->value_R_IN, iter->value_R_IN_err,rms.value_R_IN, rms.value_R_IN_err,iter->value_R_IN_Chi2,iter->value_R_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Right ",iter->value_R_OUT, iter->value_R_OUT_err,rms.value_R_OUT, rms.value_R_OUT_err,iter->value_R_OUT_Chi2,iter->value_R_OUT_NDOF);
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
      TString summary= Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", name.Data(),"ALL       ", iter->value, iter->value_err,rms.value, rms.value_err,iter->value_Chi2,iter->value_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN L    ",iter->value_L, iter->value_L_err,rms.value_L, rms.value_L_err,iter->value_L_Chi2,iter->value_L_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN R    ",iter->value_R, iter->value_R_err,rms.value_R, rms.value_R_err,iter->value_R_Chi2,iter->value_R_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP IN   ",iter->value_IN, iter->value_IN_err,rms.value_IN, rms.value_IN_err,iter->value_IN_Chi2,iter->value_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP OUT  ",iter->value_OUT, iter->value_OUT_err,rms.value_OUT, rms.value_OUT_err,iter->value_OUT_Chi2,iter->value_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Left   ",iter->value_L_IN, iter->value_L_IN_err,rms.value_L_IN, rms.value_L_IN_err,iter->value_L_IN_Chi2,iter->value_L_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Left  ",iter->value_L_OUT, iter->value_L_OUT_err,rms.value_L_OUT, rms.value_L_OUT_err,iter->value_L_OUT_Chi2,iter->value_L_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Right  ",iter->value_R_IN, iter->value_R_IN_err,rms.value_R_IN, rms.value_R_IN_err,iter->value_R_IN_Chi2,iter->value_R_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Right ",iter->value_R_OUT, iter->value_R_OUT_err,rms.value_R_OUT, rms.value_R_OUT_err,iter->value_R_OUT_Chi2,iter->value_R_OUT_NDOF);
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
      TString summary= Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", name.Data(),"ALL       ", iter->value, iter->value_err,rms.value, rms.value_err,iter->value_Chi2,iter->value_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN L    ",iter->value_L, iter->value_L_err,rms.value_L, rms.value_L_err,iter->value_L_Chi2,iter->value_L_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN R    ",iter->value_R, iter->value_R_err,rms.value_R, rms.value_R_err,iter->value_R_Chi2,iter->value_R_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP IN   ",iter->value_IN, iter->value_IN_err,rms.value_IN, rms.value_IN_err,iter->value_IN_Chi2,iter->value_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP OUT  ",iter->value_OUT, iter->value_OUT_err,rms.value_OUT, rms.value_OUT_err,iter->value_OUT_Chi2,iter->value_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Left   ",iter->value_L_IN, iter->value_L_IN_err,rms.value_L_IN, rms.value_L_IN_err,iter->value_L_IN_Chi2,iter->value_L_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Left  ",iter->value_L_OUT, iter->value_L_OUT_err,rms.value_L_OUT, rms.value_L_OUT_err,iter->value_L_OUT_Chi2,iter->value_L_OUT_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Right  ",iter->value_R_IN, iter->value_R_IN_err,rms.value_R_IN, rms.value_R_IN_err,iter->value_R_IN_Chi2,iter->value_R_IN_NDOF);
      summary       += Form("%28s|%10s| \t Mean:%12.4f+/-%12.4f \tWidth:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Right ",iter->value_R_OUT, iter->value_R_OUT_err,rms.value_R_OUT, rms.value_R_OUT_err,iter->value_R_OUT_Chi2,iter->value_R_OUT_NDOF);
      out<< summary.Data();
    }
}


out << "=======================================\n"<<"Japan Slopes\n" << "=======================================\n";
for (auto iter=source.list.begin();iter!=source.list.end(); iter++){
    auto name=iter->name;
    if(!name.Contains("_slope")) continue;
    if (name.Contains("reg_")) continue;
    name.ReplaceAll("_slope", "");
    TString summary= Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", name.Data(),"ALL       ", iter->value, iter->value_err,iter->value_Chi2,iter->value_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN L    ",iter->value_L, iter->value_L_err,iter->value_L_Chi2,iter->value_L_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN R    ",iter->value_R, iter->value_R_err,iter->value_R_Chi2,iter->value_R_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP IN   ",iter->value_IN, iter->value_IN_err,iter->value_IN_Chi2,iter->value_IN_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP OUT  ",iter->value_OUT, iter->value_OUT_err,iter->value_OUT_Chi2,iter->value_OUT_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Left   ",iter->value_L_IN, iter->value_L_IN_err,iter->value_L_IN_Chi2,iter->value_L_IN_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Left  ",iter->value_L_OUT, iter->value_L_OUT_err,iter->value_L_OUT_Chi2,iter->value_L_OUT_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Right  ",iter->value_R_IN, iter->value_R_IN_err,iter->value_R_IN_Chi2,iter->value_R_IN_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Right ",iter->value_R_OUT, iter->value_R_OUT_err,iter->value_R_OUT_Chi2,iter->value_R_OUT_NDOF);
    out<< summary.Data();
    
}


out << "=======================================\n"<<"Postpan Slopes\n" << "=======================================\n";
for (auto iter=source.list.begin();iter!=source.list.end(); iter++){
    auto name=iter->name;
    if(!name.Contains("_slope")) continue;
    if (!name.Contains("reg_")) continue;
    name.ReplaceAll("_slope", "");
    TString summary= Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", name.Data(),"ALL       ", iter->value, iter->value_err,iter->value_Chi2,iter->value_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN L    ",iter->value_L, iter->value_L_err,iter->value_L_Chi2,iter->value_L_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","WIEN R    ",iter->value_R, iter->value_R_err,iter->value_R_Chi2,iter->value_R_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP IN   ",iter->value_IN, iter->value_IN_err,iter->value_IN_Chi2,iter->value_IN_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IHWP OUT  ",iter->value_OUT, iter->value_OUT_err,iter->value_OUT_Chi2,iter->value_OUT_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Left   ",iter->value_L_IN, iter->value_L_IN_err,iter->value_L_IN_Chi2,iter->value_L_IN_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Left  ",iter->value_L_OUT, iter->value_L_OUT_err,iter->value_L_OUT_Chi2,iter->value_L_OUT_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","IN Right  ",iter->value_R_IN, iter->value_R_IN_err,iter->value_R_IN_Chi2,iter->value_R_IN_NDOF);
    summary       += Form("%45s|%10s| \t Mean:%12.4f+/-%12.4f \t Chi2/NDOF:   %3e /%5d\n", "","OUT Right ",iter->value_R_OUT, iter->value_R_OUT_err,iter->value_R_OUT_Chi2,iter->value_R_OUT_NDOF);
    out<< summary.Data();
    
}






c.Print(output+".pdf]","pdf");
out.close();
outLinear.close();
std::cout<< "All done drawing " << source.list.size() << "channels" << std::endl;

}

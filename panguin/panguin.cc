#include "panguinOnline.hh"
#include <TApplication.h>
#include <TString.h>
#include <TROOT.h>
#include <iostream>

using namespace std;

void Usage();
void online(TString type="standard",UInt_t run=0,Bool_t printonly=kFALSE);

int main(int argc, char **argv){
  TString type="default";
  UInt_t run=0;
  Bool_t printonly=kFALSE;
  Bool_t showedUsage=kFALSE;

  TApplication theApp("App",&argc,argv,NULL,-1);

  for(Int_t i=1;i<theApp.Argc();i++) {
    TString sArg = theApp.Argv(i);
    if(sArg=="-f") {
      type = theApp.Argv(++i);
      cout << " File specifier: "
	   <<  type << endl;
    } else if (sArg=="-r") {
      run = atoi(theApp.Argv(++i));
      cout << " Runnumber: "
	   << run << endl;
    } else if (sArg=="-P") {
      printonly = kTRUE;
      cout <<  " PrintOnly" << endl;
    } else if (sArg=="-h") {
      if(!showedUsage) Usage();
      showedUsage=kTRUE;
      return 0;
    } else {
      cerr << "\"" << sArg << "\"" << " not recognized.  Ignored." << endl;
      if(!showedUsage) Usage();
      showedUsage=kTRUE;
    }
  }

  online(type,run,printonly);
  theApp.Run();

  return 0;
}


void online(TString type,UInt_t run,Bool_t printonly){

  if(printonly) {
    if(!gROOT->IsBatch()) {
      gROOT->SetBatch();
    }
  }

  OnlineConfig *fconfig = new OnlineConfig(type);

  if(!fconfig->ParseConfig()) {
    gApplication->Terminate();
  }

  if(run!=0) fconfig->OverrideRootFile(run);

  new OnlineGUI(*fconfig,printonly);
}

void Usage(){
  cerr << "Usage: online [-r] [-f] [-P]" << endl;
  cerr << "Options:" << endl;
  cerr << "  -r : runnumber" << endl;
  cerr << "  -f : configuration file" << endl;
  cerr << "  -P : Only Print Summary Plots" << endl;
  cerr << endl;
}


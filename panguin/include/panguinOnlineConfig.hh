#ifndef panguinOnlineConfig_h
#define panguinOnlineConfig_h 1

#include <utility>
#include <fstream>
#include <vector>
#include <TString.h>
#include "TCut.h"

static TString guiDirectory = "macros";

class OnlineConfig {
  // Class that takes care of the config file
private:
  TString confFileName;                   // config filename
  std::ifstream *fConfFile;                    // original config file
  void ParseFile();
  std::vector < std::vector <TString> > sConfFile;  // the config file, in memory
  // pageInfo is the vector of the pages containing the sConfFile index
  //   and how many commands issued within that page (title, 1d, etc.)
  TString rootfilename;  //  Just the name
  TString goldenrootfilename; // Golden rootfile for comparisons
  TString protorootfile; // Prototype for getting the rootfilename
  TString guicolor; // User's choice of background color
  TString plotsdir; // Where to store sample plots.. automatically stored as .jpg's).
  std::vector < std::pair <UInt_t,UInt_t> > pageInfo; 
  std::vector <TCut> cutList; 
  std::vector <UInt_t> GetDrawIndex(UInt_t);
  Bool_t fFoundCfg;
  Bool_t fMonitor;
  int fVerbosity;
  int hist2D_nBinsX,hist2D_nBinsY;
  TString fPlotFormat;
  int fRunNumber;

public:
  OnlineConfig();
  OnlineConfig(TString);
  int GetRunNumber(){return fRunNumber;}
  TString GetConfFileName(){return confFileName;}
  void Get2DnumberBins(int &nX, int &nY){nX = hist2D_nBinsX; nY = hist2D_nBinsY;}
  void SetVerbosity(int ver){fVerbosity=ver;}
  TString GetPlotFormat(){return fPlotFormat;}
  Bool_t ParseConfig();
  TString GetRootFile() { return rootfilename; };
  TString GetGoldenFile() { return goldenrootfilename; };
  TString GetGuiColor() { return guicolor; };
  TString GetPlotsDir() { return plotsdir; };
  TCut GetDefinedCut(TString ident);
  std::vector <TString> GetCutIdent();
  // Page utilites
  UInt_t  GetPageCount() { return pageInfo.size(); };
  std::pair <UInt_t,UInt_t> GetPageDim(UInt_t);
  Bool_t IsLogy(UInt_t page);
  TString GetPageTitle(UInt_t);
  UInt_t GetDrawCount(UInt_t);           // Number of histograms in a page
  std::vector <TString> GetDrawCommand(UInt_t,UInt_t);
  std::vector <TString> SplitString(TString,TString);
  void OverrideRootFile(UInt_t);
  Bool_t IsMonitor() { return fMonitor; };
};

#endif //panguinOnlineConfig_h

/********************************************************************
File Name: QwCorrelator.h

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This is the header file of the QwCorrelator class,
              which is a child of the VQwDataHandler class.  The
              functionality of this class is derived from
              LinRegBlue.

Last Modified: August 1, 2018 1:43 PM
********************************************************************/

#ifndef QWCORRELATOR_H_
#define QWCORRELATOR_H_

// Parent Class
#include "VQwDataHandler.h"

// LinRegBlue Correlator Class
#include "LinReg_Bevington_Pebay.h"

// Forward declarations
class TH1D;
class TH2D;
class QwRootFile;

class QwCorrelator : public VQwDataHandler, public MQwDataHandlerCloneable<QwCorrelator>
{
 public:
  /// \brief Constructor with name
  QwCorrelator(const TString& name);
  QwCorrelator(const QwCorrelator& name);
  virtual ~QwCorrelator();

 private:
  static bool fPrintCorrelations;
 public:
  static void DefineOptions(QwOptions &options);
  void ProcessOptions(QwOptions &options);

  void ParseConfigFile(QwParameterFile& file);

  Int_t LoadChannelMap(const std::string& mapfile);

  /// \brief Connect to Channels (asymmetry/difference only)
  Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);

  void ProcessData();
  void CalcCorrelations();

  /// \brief Construct the tree branches
  void ConstructTreeBranches(
      QwRootFile *treerootfile,
      const std::string& treeprefix = "",
      const std::string& branchprefix = "");
  /// \brief Fill the tree branches
  void FillTreeBranches(QwRootFile *treerootfile) { };

  /// \brief Construct the histograms in a folder with a prefix
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  /// \brief Fill the histograms
  void  FillHistograms();

  void ClearEventData();
  void AccumulateRunningSum(VQwDataHandler &value, Int_t count = 0, Int_t ErrorMask = 0xFFFFFFF);

 protected:

  Int_t fBlock;

  bool fDisableHistos;
  
  std::vector< TString > fIndependentFull;
  std::vector< TString > fDependentFull;
    
  //  Using the fDependentType and fDependentName from base class, but override the IV arrays
  std::vector< EQwHandleType > fIndependentType;
  std::vector< std::string > fIndependentName;

  std::vector< const VQwHardwareChannel* > fIndependentVar;
  std::vector< Double_t > fIndependentValues;

  std::string fAlphaOutputFileBase;
  std::string fAlphaOutputFileSuff;
  std::string fAlphaOutputPath;
  TFile* fAlphaOutputFile;

  TTree* fTree;

  std::string fAliasOutputFileBase;
  std::string fAliasOutputFileSuff;
  std::string fAliasOutputPath;		

  int fTotalCount;
  int fGoodCount;

  int fErrCounts_EF;
  std::vector<int> fErrCounts_IV;
  std::vector<int> fErrCounts_DV;

  unsigned int fGoodEvent;

 private:
		
  int nP, nY;

  // histograms
  enum {mxHA=4};
  TH1D* hA[mxHA];

  // monitoring histos for iv & dv
  std::vector<TH1D> fH1iv;
  std::vector<TH1D> fH1dv;
  std::vector<std::vector<TH2D>> fH2iv;
  std::vector<std::vector<TH2D>> fH2dv;

  LinRegBevPeb linReg;

  void exportAlphas(std::vector < TString > ivName, std::vector < TString > dvName);
  void exportAlias(TString outPath, TString macroName, std::vector < TString > ivName, std::vector < TString > dvName);

  // Default constructor
  QwCorrelator();

};


#endif //QWCORRELATOR_H_

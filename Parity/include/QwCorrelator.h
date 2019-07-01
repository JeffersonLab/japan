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

class QwCorrelator : public VQwDataHandler, public MQwDataHandlerCloneable<QwCorrelator>
{
 public:
  /// \brief Constructor with name
  QwCorrelator(const TString& name);
  virtual ~QwCorrelator();

  void ParseConfigFile(QwParameterFile& file);

  Int_t LoadChannelMap(const std::string& mapfile);

  /// \brief Connect to Channels (asymmetry/difference only)
  Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);

  void ProcessData();
  void CalcCorrelations();
		
 protected:

  size_t fBlock;

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
  TTree* fAlphaOutputTree;

  std::string fAliasOutputFileBase;
  std::string fAliasOutputFileSuff;
  std::string fAliasOutputPath;		

  Int_t fTotalCount;
  Int_t fGoodCount;
  std::vector< Int_t > fErrCounts_IV;
  std::vector< Int_t > fErrCounts_DV;

 private:
		
  TString fNameNoSpaces;
  int nP, nY;

  // histograms
  enum {mxHA=4};
  TH1D* hA[mxHA];

  // monitoring histos for iv & dv
  TH1D **fH1iv, **fH1dv;
  TH2D **fH2iv, **fH2dv;

  LinRegBevPeb linReg;

  void init(std::vector<std::string> ivName, std::vector<std::string> dvName);
  void initHistos(std::vector<std::string> ivName, std::vector<std::string> dvName);
  void addEvent(double *Pvec, double *Yvec);
  void exportAlphas(std::vector < TString > ivName, std::vector < TString > dvName);
  void exportAlias(TString outPath, TString macroName, std::vector < TString > ivName, std::vector < TString > dvName);

  // Default constructor
  QwCorrelator();

};


#endif //QWCORRELATOR_H_

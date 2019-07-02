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
#include "QwkRegBlueCorrelator.h"

class QwCorrelator : public VQwDataHandler, public MQwDataHandlerCloneable<QwCorrelator>{
 public:
  /// \brief Constructor with name
  QwCorrelator(const TString& name);

  void ParseConfigFile(QwParameterFile& file);

  Int_t LoadChannelMap(const std::string& mapfile);
  	
  void readConfig(const char * configFName);
  	
  /// \brief Connect to Channels (asymmetry/difference only)
  Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);
		
  void unpackEvent();

  void ProcessData();
  void CalcCorrelations();
		
 protected:
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

  std::string fAliasOutputFileBase;
  std::string fAliasOutputFileSuff;
  std::string fAliasOutputPath;		

  Int_t fTotalCount;
  Int_t fGoodCount;
  Int_t fErrCounts_EF;
  std::vector< Int_t > fErrCounts_IV;
  std::vector< Int_t > fErrCounts_DV;

 private:
		
  //Default Constructor
  QwCorrelator():corA("input") { };
		
  QwkRegBlueCorrelator corA;
		
};


#endif //QWCORRELATOR_H_

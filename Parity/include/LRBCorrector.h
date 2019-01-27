/********************************************************************
File Name: LRBCorrector.h

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This is the header file of the LRBCorrector class,
              which is a child of the VQwDataHandler class.  The
              functionality of this class is derived from
              LinRegBlue.

Last Modified: August 1, 2018 1:41 PM
********************************************************************/

#ifndef LRBCORRECTOR_H_
#define LRBCORRECTOR_H_

// Parent Class
#include "VQwDataHandler.h"


//Formerly LRBRegression
class LRBCorrector : public VQwDataHandler {
  
  public:
    
    LRBCorrector(QwOptions &options, QwHelicityPattern& helicitypattern, const TString &run = "0");
    
    /// \brief Define the configuration options
    void static DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);

    Int_t LoadChannelMap(const std::string& mapfile);

    Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);
    
    void CalcOneOutput(const VQwHardwareChannel* dv, VQwHardwareChannel* output,
                       std::vector< const VQwHardwareChannel* > &ivs,
                       std::vector< Double_t > &sens);

    void ProcessData();

    void LinearRegression(EQwRegType type);
    
  protected:
    
    LRBCorrector() { }

    bool fEnableCorrection;

    std::string outPath;

    /// Single event pointer
    QwSubsystemArrayParity* fSubsystemArray;
    /// Helicity pattern pointer
    QwHelicityPattern* fHelicityPattern;

    /// Regression dependent and independent variable map
    std::string fRegressionMapFile;

    std::vector< EQwRegType > fIndependentType;
    std::vector< std::string > fIndependentName;
    
    std::vector< const VQwHardwareChannel* > fIndependentVar;
    std::vector< Double_t > fIndependentValues;

    std::vector< std::vector< Double_t > > fSensitivity;
    
};


#endif // LRBCORRECTOR_H_

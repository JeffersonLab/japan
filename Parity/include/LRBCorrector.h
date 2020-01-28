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
#include "LinReg_Bevington_Pebay.h"

// ROOT headers
#include <TVectorD.h>
#include <TMatrixD.h>


class LRBCorrector : public VQwDataHandler, public MQwDataHandlerCloneable<LRBCorrector>{
 public:
    /// \brief Constructor with name
    LRBCorrector(const TString& name);

    void ParseConfigFile(QwParameterFile& file);

    Int_t LoadChannelMap(const std::string& mapfile);

    Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);
    
    void ProcessData();
    Int_t AddTwoBursts(Short_t, Short_t);

    void UpdateBurstCounter(Short_t burstcounter){
      if (burstcounter<fLastCycle){
        fBurstCounter=burstcounter;
      } else if (fLastCycle==1){
        //  If we have a single cycle of slopes, don't throw a warning.
        fBurstCounter = 0;
      } else {
        fBurstCounter = fLastCycle-1;
        QwWarning << "LRBCorrector, " << GetName() 
          << ": Burst counter, " << burstcounter
          << ", is greater than the stored number of sets of slopes.  "
          << "Using the last set of slopes (cycle=" << fLastCycle
          << ")" << QwLog::endl;
      }
    };

  protected:
    LRBCorrector() { }

    std::string fAlphaFileBase;
    std::string fAlphaFileSuff;
    std::string fAlphaFilePath;

    std::vector< EQwHandleType > fIndependentType;
    std::vector< std::string > fIndependentName;
    std::vector< std::string > fIndependentFull;
    //TH1D  IVname;1  names of IVs
    //TH1D  DVname;1  names of IVs

    std::vector< const VQwHardwareChannel* > fIndependentVar;
    std::vector< Double_t > fIndependentValues;

    Short_t fLastCycle;
    std::map<Short_t,std::vector<std::vector<Double_t>>> fSensitivity;
    std::map<Short_t,Int_t> fNstat; 

    std::map<TString, std::map<Short_t,Double_t>> fStats; // Burstwise filled vectors, map of name vs. LinReg Vector
    std::map<TString, std::map<Short_t,TVectorD>> fVecs;  // Burstwise filled vectors, map of name vs. LinReg Vector
    std::map<TString, std::map<Short_t,TMatrixD>> fMats;  // Burstwise filled vectors, map of name vs. LinReg Matrix
    TString fStatNames[1] = {
      "MyStat"};
    TString fVecNames[12]  = {
      "IV_mean",
      "DV_mean",
      "DV_mean_prime",
      "IV_sigma",
      "DV_sigma",
      "DV_sigma_prime",
      "DV_rawVariance",
      "IV_rawVariance",
      "DV_rawVariance_prime",
      "DV_normVariance",
      "IV_normVariance",
      "DV_normVariance_prime"};
    TString fMatNames[16]  = {
      "slopes",
      "sigSlopes",
      "IV_IV_correlation",
      "IV_DV_correlation",
      "DV_DV_correlation",
      "DV_DV_correlation_prime",
      "IV_IV_rawVariance",
      "IV_DV_rawVariance",
      "DV_DV_rawVariance",
      "DV_DV_rawVariance_prime",
      "IV_IV_normVariance",
      "IV_DV_normVariance",
      "DV_DV_normVariance",
      "DV_DV_normVariance_prime",
      "A_xy",
      "A_yx"};

    std::map<Short_t,LinRegBevPeb> fLinRegs;
    
};


#endif // LRBCORRECTOR_H_

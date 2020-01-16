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


class LRBCorrector : public VQwDataHandler, public MQwDataHandlerCloneable<LRBCorrector>{
 public:
    /// \brief Constructor with name
    LRBCorrector(const TString& name);

    void ParseConfigFile(QwParameterFile& file);

    Int_t LoadChannelMap(const std::string& mapfile);

    Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);
    
    void ProcessData();

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

    std::vector< const VQwHardwareChannel* > fIndependentVar;
    std::vector< Double_t > fIndependentValues;

    Short_t fLastCycle;
    std::map<Short_t,std::vector<std::vector<Double_t>>> fSensitivity;
    
};


#endif // LRBCORRECTOR_H_

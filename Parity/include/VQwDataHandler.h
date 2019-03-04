/*****************************************************************************
File Name: VQwDataHandler.h

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This is the header file to the VQwDataHandler class.  This
              class acts as a base class to all classes which need
              to access data from multiple subsystems

Last Modified: August 1, 2018 1:39 PM
*****************************************************************************/

#ifndef VQWDATAHANDLER_H_
#define VQWDATAHANDLER_H_

// Qweak headers
//#include "QwHelicityPattern.h"
#include "QwSubsystemArrayParity.h"
#include "VQwHardwareChannel.h"
#include "QwFactory.h"

class QwHelicityPattern;

class VQwDataHandler{

  public:
    
    enum EQwHandleType {
      kHandleTypeUnknown=0, kHandleTypeMps, kHandleTypeAsym, kHandleTypeDiff
    };
    
    typedef std::vector< VQwHardwareChannel* >::iterator Iterator_HdwChan;
    typedef std::vector< VQwHardwareChannel* >::const_iterator ConstIterator_HdwChan;

    void ProcessData();

    virtual ~VQwDataHandler();

    TString GetDataHandlerName(){return fName;}

    void AccumulateRunningSum(VQwDataHandler &value);
    void CalculateRunningAverage();
    void PrintValue() const;
    void FillDB(QwParityDB *db, TString datatype){};

    // Fill the vector for this subsystem
    void FillTreeVector(std::vector<Double_t> &values) const;

    void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values);

    void get_run_label(TString x) {
      run_label = x;
    }

    virtual void ProcessOptions(QwOptions &options) = 0;

    virtual Int_t LoadChannelMap(const std::string& mapfile) = 0;

  protected:
    
    VQwDataHandler() { }
    
    Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);
    
    std::pair<EQwHandleType,std::string> ParseHandledVariable(const std::string& variable);

   void CalcOneOutput(const VQwHardwareChannel* dv, VQwHardwareChannel* output,
                       std::vector< const VQwHardwareChannel* > &ivs,
                       std::vector< Double_t > &sens);

    //Bool_t PublishInternalValue(const TString &name, const TString &desc, const VQwHardwareChannel *value) const;
    //Bool_t PublishByRequest(TString device_name);

 protected:
    //***************[Variables]***************
   TString fName;

    UInt_t fErrorFlag;

    TString run_label;

    std::string fCorrelatorMapFile;

    std::vector< EQwHandleType > fDependentType;
    std::vector< std::string > fDependentName;

    std::vector< const VQwHardwareChannel* > fDependentVar;
    std::vector< Double_t > fDependentValues;

    std::vector< VQwHardwareChannel* > fOutputVar;
    std::vector< Double_t > fOutputValues;

    std::string ParseSeparator;  // Used as space between tokens in ParseHandledVariable

};


#endif // VQWDATAHANDLER_H_

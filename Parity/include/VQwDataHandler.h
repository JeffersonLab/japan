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


class QwParameterFile;
class QwRootFile;
class QwHelicityPattern;
class QwPromptSummary;

class VQwDataHandler:  virtual public VQwDataHandlerCloneable {

  public:
  
    enum EQwHandleType {
      kHandleTypeUnknown=0, kHandleTypeMps, kHandleTypeAsym, kHandleTypeDiff
    };
    
    typedef std::vector< VQwHardwareChannel* >::iterator Iterator_HdwChan;
    typedef std::vector< VQwHardwareChannel* >::const_iterator ConstIterator_HdwChan;

    VQwDataHandler(const TString& name):fName(name),fPrefix(""),fKeepRunningSum(kFALSE) {}
    VQwDataHandler(const VQwDataHandler &source);

    virtual void ParseConfigFile(QwParameterFile& file);

    void SetPointer(QwHelicityPattern *ptr){fHelicityPattern = ptr;};
    void SetPointer(QwSubsystemArrayParity *ptr){fSubsystemArray = ptr;};

    Int_t ConnectChannels(QwSubsystemArrayParity& yield, QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff){
      return this->ConnectChannels(asym, diff);
    }

    virtual void ProcessData();

    virtual void FinishDataHandler(){};

    virtual void CalcCorrelations(){};

    virtual ~VQwDataHandler();

    TString GetDataHandlerName(){return fName;}

    void ClearEventData();

    void AccumulateRunningSum(VQwDataHandler &value);
    void CalculateRunningAverage();
    void PrintValue() const;
    void FillDB(QwParityDB *db, TString datatype){};

    void WritePromptSummary(QwPromptSummary *ps, TString type);

    void ConstructTreeBranches(
        QwRootFile *treerootfile,
        const std::string& treeprefix = "",
        const std::string& branchprefix = "");
    void FillTreeBranches(QwRootFile *treerootfile);

    // Fill the vector for this subsystem
    void FillTreeVector(std::vector<Double_t> &values) const;

    void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values);

    void SetRunLabel(TString x) {
      run_label = x;
    }

    Int_t LoadChannelMap(){return this->LoadChannelMap(fMapFile);}
    virtual Int_t LoadChannelMap(const std::string& mapfile){return 0;};

  protected:
    
    VQwDataHandler() { }
    
    virtual Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);
    
    std::pair<EQwHandleType,std::string> ParseHandledVariable(const std::string& variable);

   void CalcOneOutput(const VQwHardwareChannel* dv, VQwHardwareChannel* output,
                       std::vector< const VQwHardwareChannel* > &ivs,
                       std::vector< Double_t > &sens);

    //Bool_t PublishInternalValue(const TString &name, const TString &desc, const VQwHardwareChannel *value) const;
    //Bool_t PublishByRequest(TString device_name);

 protected:
   //
   Int_t fPriority; ///  When a datahandler array is processed, handlers with lower priority will be processed before handlers with higher priority

    //***************[Variables]***************
   TString fName;
   std::string fMapFile;
   std::string fTreeName;
   std::string fTreeComment;

   std::string fPrefix;

   TString run_label;

   /// Single event pointer
   QwSubsystemArrayParity* fSubsystemArray;
   /// Helicity pattern pointer
   QwHelicityPattern* fHelicityPattern;

   std::vector< EQwHandleType > fDependentType;
   std::vector< std::string > fDependentName;

   std::vector< const VQwHardwareChannel* > fDependentVar;
   std::vector< Double_t > fDependentValues;

   std::vector< VQwHardwareChannel* > fOutputVar;
   std::vector< Double_t > fOutputValues;

   std::string ParseSeparator;  // Used as space between tokens in ParseHandledVariable

 protected:
   Bool_t fKeepRunningSum;
};

#endif // VQWDATAHANDLER_H_

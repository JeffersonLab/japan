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
#include "QwHelicityPattern.h"
#include "QwSubsystemArrayParity.h"
#include "VQwHardwareChannel.h"
#include "QwFactory.h"
#include "MQwPublishable.h"

class QwParameterFile;
class QwRootFile;
class QwPromptSummary;
class QwDataHandlerArray;

class VQwDataHandler:  virtual public VQwDataHandlerCloneable, public MQwPublishable_child<QwDataHandlerArray,VQwDataHandler> {

  public:
  
    enum EQwHandleType {
      kHandleTypeUnknown=0, kHandleTypeMps, kHandleTypeAsym, kHandleTypeDiff, kHandleTypeYield
    };
    
    typedef std::vector< VQwHardwareChannel* >::iterator Iterator_HdwChan;
    typedef std::vector< VQwHardwareChannel* >::const_iterator ConstIterator_HdwChan;

    VQwDataHandler(const TString& name);
    VQwDataHandler(const VQwDataHandler &source);

    virtual void ParseConfigFile(QwParameterFile& file);

    void SetPointer(QwHelicityPattern *ptr){
      fHelicityPattern = ptr;
      fErrorFlagPtr = ptr->GetEventcutErrorFlagPointer();
    };
    void SetPointer(QwSubsystemArrayParity *ptr){
      fSubsystemArray = ptr;
      fErrorFlagPtr = ptr->GetEventcutErrorFlagPointer();
    };

    virtual Int_t ConnectChannels(QwSubsystemArrayParity& yield, QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff){
      return this->ConnectChannels(asym, diff);
    }

    // Subsystems with support for subsystem arrays should override this
    virtual Int_t ConnectChannels(QwSubsystemArrayParity& detectors) { return 0; }

    Int_t ConnectChannels(QwHelicityPattern& helicitypattern) {
      return this->ConnectChannels(
          helicitypattern.GetYield(),
          helicitypattern.GetAsymmetry(),
          helicitypattern.GetDifference());
    }

    virtual void ProcessData();

    virtual void UpdateBurstCounter(Short_t burstcounter){fBurstCounter=burstcounter;};

    virtual void FinishDataHandler(){
      CalculateRunningAverage();
    };

    virtual ~VQwDataHandler();

    TString GetName(){return fName;}

    virtual void ClearEventData();

    void InitRunningSum();
    void AccumulateRunningSum();
    virtual void AccumulateRunningSum(VQwDataHandler &value, Int_t count = 0, Int_t ErrorMask = 0xFFFFFFF);
    void CalculateRunningAverage();
    void PrintValue() const;
    void FillDB(QwParityDB *db, TString datatype){};

    void WritePromptSummary(QwPromptSummary *ps, TString type);

    virtual void ConstructTreeBranches(
        QwRootFile *treerootfile,
        const std::string& treeprefix = "",
        const std::string& branchprefix = "");
    virtual void FillTreeBranches(QwRootFile *treerootfile);

    /// \brief Construct the histograms in a folder with a prefix
    virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) { };
    /// \brief Fill the histograms
    virtual void  FillHistograms() { };

    // Fill the vector for this subsystem
    void FillTreeVector(std::vector<Double_t> &values) const;

    void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values);

    void SetRunLabel(TString x) {
      run_label = x;
    }

    Int_t LoadChannelMap(){return this->LoadChannelMap(fMapFile);}
    virtual Int_t LoadChannelMap(const std::string& mapfile){return 0;};

    /// \brief Publish all variables of the subsystem
    virtual Bool_t PublishInternalValues() const;
    /// \brief Try to publish an internal variable matching the submitted name
    virtual Bool_t PublishByRequest(TString device_name);

  protected:
    
    VQwDataHandler() { }
    
    virtual Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);
    
    void SetEventcutErrorFlagPointer(const UInt_t* errorflagptr) {
      fErrorFlagPtr = errorflagptr;
    }
    UInt_t GetEventcutErrorFlag() const {
      if (fErrorFlagPtr)
        return *fErrorFlagPtr;
      else
        return -1;
    };

    std::pair<EQwHandleType,std::string> ParseHandledVariable(const std::string& variable);

   void CalcOneOutput(const VQwHardwareChannel* dv, VQwHardwareChannel* output,
                       std::vector< const VQwHardwareChannel* > &ivs,
                       std::vector< Double_t > &sens);

 protected:
   //
   Int_t fPriority; ///  When a datahandler array is processed, handlers with lower priority will be processed before handlers with higher priority

   Short_t fBurstCounter;

    //***************[Variables]***************
   TString fName;
   std::string fMapFile;
   std::string fTreeName;
   std::string fTreeComment;

   std::string fPrefix;

   TString run_label;

   /// Error flag pointer
   const UInt_t* fErrorFlagPtr;

   /// Single event pointer
   QwSubsystemArrayParity* fSubsystemArray;
   /// Helicity pattern pointer
   QwHelicityPattern* fHelicityPattern;

   std::vector< std::string > fDependentFull;
   std::vector< EQwHandleType > fDependentType;
   std::vector< std::string > fDependentName;

   std::vector< const VQwHardwareChannel* > fDependentVar;
   std::vector< Double_t > fDependentValues;

   std::vector< VQwHardwareChannel* > fOutputVar;
   std::vector< Double_t > fOutputValues;

   std::vector<std::vector<TString> > fPublishList;

   std::string ParseSeparator;  // Used as space between tokens in ParseHandledVariable

 protected:
   Bool_t fKeepRunningSum;
   Bool_t fRunningsumFillsTree;
   VQwDataHandler *fRunningsum;
};

#endif // VQWDATAHANDLER_H_

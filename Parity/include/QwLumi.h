/**********************************************************\
* File: QwLumi.h                                          *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwLUMI__
#define __QwLUMI__

// System headers
#include <vector>

// ROOT headers
#include "TTree.h"

// Qweak headers
#include "VQwSubsystemParity.h"

#include "QwIntegrationPMT.h"
#include "QwCombinedPMT.h"
#include "QwTypes.h"
#include "QwScaler_Channel.h"


class QwLumiDetectorID{

 public:

  QwLumiDetectorID():fSubbankIndex(-1),fWordInSubbank(-1),
    fTypeID(kQwUnknownPMT),fIndex(-1),
    fSubelement(999999),fmoduletype(""),fdetectorname("")
    {};

  int fSubbankIndex;
  int fWordInSubbank; //first word reported for this channel in the subbank
                      //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)
  // The first word of the subbank gets fWordInSubbank=0

  EQwPMTInstrumentType fTypeID;     // type of detector
  int fIndex;      // index of this detector in the vector containing all the detector of same type
  UInt_t fSubelement; // some detectors have many subelements (eg stripline have 4 antenas) some have only one sub element(eg lumis have one channel)

  TString fmoduletype; // eg: VQWK, SCALER
  TString fdetectorname;
  TString fdetectortype; // IntegrationPMT,fLumiCounter .. this string is encoded by fTypeID
  std::vector<TString> fCombinedChannelNames;
  std::vector<Double_t> fWeight;
  void Print() const;

};


/*****************************************************************
*  Class:
******************************************************************/
class QwLumi : public VQwSubsystemParity, public MQwSubsystemCloneable<QwLumi> {
  /////  
  friend class QwCombinedPMT;

 private:
  /// Private default constructor (not implemented, will throw linker error on use)
  QwLumi();

 public:
  /// Constructor with name
  QwLumi(const TString& name)
  : VQwSubsystem(name),VQwSubsystemParity(name),bNormalization(kFALSE)
  {
    fTargetCharge.InitializeChannel("q_targ","derived");
  };
  /// Copy constructor
  QwLumi(const QwLumi& source)
  : VQwSubsystem(source),VQwSubsystemParity(source),
    fIntegrationPMT(source.fIntegrationPMT),fCombinedPMT(source.fCombinedPMT),
    fScalerPMT(source.fScalerPMT),
    fLumiDetectorID(source.fLumiDetectorID)
  { }
  /// Virtual destructor
  virtual ~QwLumi() { };


  /* derived from VQwSubsystem */

  /// \brief Define options function
  static void DefineOptions(QwOptions &options);


  void ProcessOptions(QwOptions &options);//Handle command line options
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t LoadEventCuts(TString filename);//derived from VQwSubsystemParity
  Bool_t ApplySingleEventCuts();//derived from VQwSubsystemParity
  void IncrementErrorCounters();
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliures
  UInt_t GetEventcutErrorFlag();//return the error flag
  //update the error flag in the subsystem level from the top level routines related to stability checks. This will uniquely update the errorflag at each channel based on the error flag in the corresponding channel in the ev_error subsystem
  void UpdateErrorFlag(const VQwSubsystem *ev_error);

  void AccumulateRunningSum(VQwSubsystem* value);
  //remove one entry from the running sums for devices
  void DeaccumulateRunningSum(VQwSubsystem* value);
  void CalculateRunningAverage();

  Int_t ProcessConfigurationBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t ProcessEvBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  PrintDetectorID() const;

  void  ClearEventData();
  Bool_t IsGoodEvent();

  void  ProcessEvent();
  void  ExchangeProcessedData();
  void  ProcessEvent_2();

  Bool_t PublishInternalValues() const;
  Bool_t PublishByRequest(TString device_name);

  void DoNormalization(Double_t factor=1.0);

  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void RandomizeEventData(int helicity = 0, double time = 0.0);
  void EncodeEventData(std::vector<UInt_t> &buffer);

  VQwSubsystem&  operator=  (VQwSubsystem *value);
  VQwSubsystem&  operator+= (VQwSubsystem *value);
  VQwSubsystem&  operator-= (VQwSubsystem *value);
  void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
  void Normalize(VQwDataElement* denom);
  void Scale(Double_t factor);

  using VQwSubsystem::ConstructHistograms;
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file);
  void  FillTreeVector(std::vector<Double_t> &values) const;

#ifdef __USE_DATABASE__
  void  FillDB(QwParityDB *db, TString datatype);
  void  FillErrDB(QwParityDB *db, TString datatype);
#endif
 
  const QwIntegrationPMT* GetChannel(const TString name) const;
  const QwIntegrationPMT* GetIntegrationPMT(const TString name) const;
  const QwCombinedPMT* GetCombinedPMT(const TString name) const;

  Bool_t Compare(VQwSubsystem *source);

  void PrintValue() const;
  void PrintInfo() const;
  void WritePromptSummary(QwPromptSummary *ps, TString type);

  std::vector<TString> fgDetectorTypeNames;


/////
 protected: 
 
 EQwPMTInstrumentType GetDetectorTypeID(TString name);
 
 Int_t GetDetectorIndex(EQwPMTInstrumentType TypeID, TString name);
 //when the type and the name is passed the detector index from appropriate vector will be returned
 //for example if TypeID is IntegrationPMT  then the index of the detector from fIntegrationPMT vector for given name will be returnd.

 std::vector <QwIntegrationPMT>      fIntegrationPMT;  
 std::vector <QwCombinedPMT>         fCombinedPMT;
 std::vector <QwSIS3801D24_Channel>  fScalerPMT;

 std::vector <QwLumiDetectorID>      fLumiDetectorID;

 protected:
   QwBeamCharge   fTargetCharge;
   Bool_t bIsExchangedDataValid;
   Bool_t bNormalization;

/////
 private:
 Int_t fQwLumiErrorCount;

 static const Bool_t bDEBUG=kFALSE;

};




#endif

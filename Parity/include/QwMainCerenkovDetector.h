/**********************************************************\
* File: QwMainCerenkovDetector.h                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

///
/// \ingroup QwAnalysis_ADC

#ifndef __QWMAINCERENKOVDETECTOR__
#define __QWMAINCERENKOVDETECTOR__

// System headers
#include <vector>

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwIntegrationPMT.h"
#include "QwCombinedPMT.h"


// Forward declarations
class QwBlinder;
class QwMainCerenkovDetectorID;


class QwMainCerenkovDetector:
    public VQwSubsystemParity,
    public MQwSubsystemCloneable<QwMainCerenkovDetector>
{
  friend class QwCombinedPMT;
  /******************************************************************
   *  Class: QwMainCerenkovDetector
   *
   *
   ******************************************************************/
 private:
  /// Private default constructor (not implemented, will throw linker error on use)
  QwMainCerenkovDetector();

 public:
  /// Constructor with name
  QwMainCerenkovDetector(const TString& name)
  : VQwSubsystem(name),VQwSubsystemParity(name),bNormalization(kFALSE)
  {
    fTargetCharge.InitializeChannel("q_targ","derived");
    fTargetX.InitializeChannel("x_targ","derived");
    fTargetY.InitializeChannel("y_targ","derived");
    fTargetXprime.InitializeChannel("xp_targ","derived");
    fTargetYprime.InitializeChannel("yp_targ","derived");
    fTargetEnergy.InitializeChannel("e_targ","derived");
  };
  /// Copy constructor
  QwMainCerenkovDetector(const QwMainCerenkovDetector& source)
  : VQwSubsystem(source),VQwSubsystemParity(source),
    fIntegrationPMT(source.fIntegrationPMT),
    fCombinedPMT(source.fCombinedPMT)
  { }
  /// Virtual destructor
  virtual ~QwMainCerenkovDetector() { };

  /*  Member functions derived from VQwSubsystemParity. */

  /// \brief Define options function
  static void DefineOptions(QwOptions &options);


  void ProcessOptions(QwOptions &options);//Handle command line options
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t LoadEventCuts(TString filename);
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  void IncrementErrorCounters();
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  UInt_t GetEventcutErrorFlag();//return the error flag

  //update the error flag in the subsystem level from the top level routines related to stability checks. This will uniquely update the errorflag at each channel based on the error flag in the corresponding channel in the ev_error subsystem
  void UpdateErrorFlag(const VQwSubsystem *ev_error);


  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ClearEventData();
  Bool_t IsGoodEvent();

  void  ProcessEvent();
  void  ExchangeProcessedData();
  void  ProcessEvent_2();


  Bool_t PublishInternalValues() const;

  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity = 0, Double_t time = 0.0);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ConstructHistograms(TDirectory *folder){
    TString tmpstr("");
    ConstructHistograms(folder,tmpstr);
  };

  using VQwSubsystem::ConstructHistograms;
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  using VQwSubsystem::ConstructBranchAndVector;
  void ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void ConstructBranch(TTree *tree, TString &prefix);
  void ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file );

  void  FillTreeVector(std::vector<Double_t> &values) const;
#ifdef __USE_DATABASE__
  void  FillDB(QwParityDB *db, TString datatype);
  void  FillErrDB(QwParityDB *db, TString datatype);
#endif // __USE_DATABASE__

  const QwIntegrationPMT* GetChannel(const TString name) const;

  Bool_t Compare(VQwSubsystem* source);


  /// \brief Blind the asymmetry
  void Blind(const QwBlinder *blinder);
  /// \brief Blind the difference using the yield
  void Blind(const QwBlinder *blinder, const VQwSubsystemParity* subsys);


  VQwSubsystem&  operator=  ( VQwSubsystem *value);
  VQwSubsystem&  operator+= ( VQwSubsystem *value);
  VQwSubsystem&  operator-= ( VQwSubsystem *value);


  void Sum(VQwSubsystem* value1, VQwSubsystem* value2);
  void Difference(VQwSubsystem* value1, VQwSubsystem* value2);
  void Ratio(VQwSubsystem* numer, VQwSubsystem* denom);
  void Scale(Double_t factor);
  void Normalize(VQwDataElement* denom);

  void AccumulateRunningSum(VQwSubsystem* value);
  //remove one entry from the running sums for devices
  void DeaccumulateRunningSum(VQwSubsystem* value);
  void CalculateRunningAverage();

  const QwIntegrationPMT* GetIntegrationPMT(const TString name) const;
  const QwCombinedPMT* GetCombinedPMT(const TString name) const;

  void DoNormalization(Double_t factor=1.0);

  Bool_t ApplyHWChecks(){//Check for harware errors in the devices
    Bool_t status = kTRUE;
    for (size_t i=0; i<fIntegrationPMT.size(); i++){
      status &= fIntegrationPMT.at(i).ApplyHWChecks();
    }
    return status;
  };

  void  PrintValue() const;
  void  WritePromptSummary(QwPromptSummary *ps, TString type);
  void  PrintInfo() const;
  void  PrintDetectorID() const;


 protected:
  Bool_t fDEBUG;

 EQwPMTInstrumentType GetDetectorTypeID(TString name);

 // when the type and the name is passed the detector index from appropriate vector
 // will be returned. For example if TypeID is IntegrationPMT  then the index of
 // the detector from fIntegrationPMT vector for given name will be returnd.
 Int_t GetDetectorIndex(EQwPMTInstrumentType TypeID, TString name);

  std::vector <QwIntegrationPMT> fIntegrationPMT;
  std::vector <QwCombinedPMT> fCombinedPMT;
  std::vector <QwMainCerenkovDetectorID> fMainDetID;

 protected:
  QwBeamCharge   fTargetCharge;
  QwBeamPosition fTargetX;
  QwBeamPosition fTargetY;
  QwBeamAngle    fTargetXprime;
  QwBeamAngle    fTargetYprime;
  QwBeamEnergy   fTargetEnergy;

  Bool_t bIsExchangedDataValid;

  Bool_t bNormalization;

 private:

  static const Bool_t bDEBUG=kFALSE;
  Int_t fMainDetErrorCount;

};


class QwMainCerenkovDetectorID
{
 public:
  QwMainCerenkovDetectorID():fSubbankIndex(-1),fWordInSubbank(-1),
    fTypeID(kQwUnknownPMT),fIndex(-1),
    fSubelement(kInvalidSubelementIndex),fmoduletype(""),fdetectorname("")
    {};

  int fSubbankIndex;
  int fWordInSubbank; //first word reported for this channel in the subbank
                      //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)
  // The first word of the subbank gets fWordInSubbank=0

  EQwPMTInstrumentType fTypeID;     // type of detector
  int fIndex;      // index of this detector in the vector containing all the detector of same type
  UInt_t fSubelement; // some detectors have many subelements (eg stripline have 4 antenas)
                      // some have only one sub element(eg lumis have one channel)

  TString fmoduletype; // eg: VQWK, SCALER
  TString fdetectorname;
  TString fdetectortype; // stripline, IntegrationPMT, ... this string is encoded by fTypeID

  std::vector<TString> fCombinedChannelNames;
  std::vector<Double_t> fWeight;

  void Print() const;

};


#endif














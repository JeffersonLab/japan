/**********************************************************\
* File: QwBeamLine.h                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwBEAMLINE__
#define __QwBEAMLINE__

// System headers
#include <vector>

// ROOT headers
#include "TTree.h"
#include "TString.h"

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwTypes.h"
#include "QwBPMStripline.h"
#include "VQwBCM.h"
#include "QwBCM.h"
#include "QwBPMCavity.h"
#include "QwCombinedBCM.h"
#include "QwCombinedBPM.h"
#include "QwEnergyCalculator.h"
#include "QwHaloMonitor.h"
#include "QwQPD.h"
#include "QwLinearDiodeArray.h"
#include "VQwClock.h"
#include "QwBeamDetectorID.h"


/*****************************************************************
*  Class:
******************************************************************/
class QwBeamLine : public VQwSubsystemParity, public MQwSubsystemCloneable<QwBeamLine> {

 private:
  /// Private default constructor (not implemented, will throw linker error on use)
  QwBeamLine();

 public:
  /// Constructor with name
  QwBeamLine(const TString& name)
  : VQwSubsystem(name),VQwSubsystemParity(name)
  { };
  /// Copy constructor
  QwBeamLine(const QwBeamLine& source)
  : VQwSubsystem(source),VQwSubsystemParity(source),
    fQPD(source.fQPD),
    fLinearArray(source.fLinearArray),
    fCavity(source.fCavity),
    fHaloMonitor(source.fHaloMonitor),
    fECalculator(source.fECalculator),
    fBeamDetectorID(source.fBeamDetectorID)
  { this->CopyTemplatedDataElements(&source); }
  /// Virtual destructor
  virtual ~QwBeamLine() { };

  void   CopyTemplatedDataElements(const VQwSubsystem *source);

  /* derived from VQwSubsystem */
  
  void   ProcessOptions(QwOptions &options);//Handle command line options
  Int_t  LoadChannelMap(TString mapfile);
  Int_t  LoadInputParameters(TString pedestalfile);
  Int_t  LoadEventCuts(TString filename);//derived from VQwSubsystemParity
  Int_t  LoadGeometryDefinition(TString mapfile);
  Int_t  LoadMockDataParameters(TString mapfile);
  void   AssignGeometry(QwParameterFile* mapstr, VQwBPM * bpm);

  Bool_t ApplySingleEventCuts();//derived from VQwSubsystemParity
  void   IncrementErrorCounters();
  void   PrintErrorCounters() const;// report number of events failed due to HW and event cut faliures
  UInt_t GetEventcutErrorFlag();//return the error flag

  UInt_t UpdateErrorFlag();//Update and return the error flags

  //update the error flag in the subsystem level from the top level routines related to stability checks. This will uniquely update the errorflag at each channel based on the error flag in the correspondinsize_t GetNumberOfElements() {return fElement.size();};g channel in the ev_error subsystem
  void   UpdateErrorFlag(const VQwSubsystem *ev_error);

  Int_t  ProcessConfigurationBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t  ProcessEvBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words);
  void   PrintDetectorID() const;

  void   ClearEventData();
  void   ProcessEvent();

  Bool_t PublishInternalValues() const;
  Bool_t PublishByRequest(TString device_name);


 public:
  size_t GetNumberOfElements();
  void   RandomizeEventData(int helicity = 0, double time = 0.0);
  void   SetRandomEventAsymmetry(Double_t asymmetry);
  void   EncodeEventData(std::vector<UInt_t> &buffer);

  VQwSubsystem&  operator=  (VQwSubsystem *value);
  VQwSubsystem&  operator+= (VQwSubsystem *value);
  VQwSubsystem&  operator-= (VQwSubsystem *value);
  void   Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void   Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void   Ratio(VQwSubsystem *numer, VQwSubsystem *denom);

  void   Scale(Double_t factor);

  void   AccumulateRunningSum(VQwSubsystem* value);
  //remove one entry from the running sums for devices
  void   DeaccumulateRunningSum(VQwSubsystem* value);


  void   CalculateRunningAverage();

  using  VQwSubsystem::ConstructHistograms;
  void   ConstructHistograms(TDirectory *folder, TString &prefix);
  void   FillHistograms();

  using  VQwSubsystem::ConstructBranchAndVector;
  void   ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void   ConstructBranch(TTree *tree, TString &prefix);
  void   ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file );
  void   FillTreeVector(std::vector<Double_t> &values) const;

#ifdef __USE_DATABASE__
  void   FillDB(QwParityDB *db, TString datatype);
  void   FillErrDB(QwParityDB *db, TString datatype);
#endif // __USE_DATABASE__

  Bool_t Compare(VQwSubsystem *source);

  void   PrintValue() const;
  void   PrintInfo() const;
  void   WritePromptSummary(QwPromptSummary *ps, TString type);

  VQwDataElement* GetElement(QwBeamDetectorID det_id);
  VQwDataElement* GetElement(EQwBeamInstrumentType TypeID, TString name);
  VQwDataElement* GetElement(EQwBeamInstrumentType TypeID, Int_t index);
  const  VQwDataElement* GetElement(EQwBeamInstrumentType TypeID, Int_t index) const;

  const  VQwHardwareChannel* GetChannel(EQwBeamInstrumentType TypeID, Int_t index, TString device_prop) const;

  VQwBPM* GetBPMStripline(const TString name);
  VQwBCM* GetBCM(const TString name);
  VQwClock* GetClock(const TString name);
  QwBPMCavity* GetBPMCavity(const TString name);
  VQwBCM* GetCombinedBCM(const TString name);
  VQwBPM* GetCombinedBPM(const TString name);
  QwEnergyCalculator* GetEnergyCalculator(const TString name);
  QwHaloMonitor* GetScalerChannel(const TString name);
  const QwBPMCavity* GetBPMCavity(const TString name) const;
  const VQwBPM* GetBPMStripline(const TString name) const;
  const VQwBCM* GetBCM(const TString name) const;
  const VQwClock* GetClock(const TString name) const;
  const VQwBCM* GetCombinedBCM(const TString name) const;
  const VQwBPM* GetCombinedBPM(const TString name) const;
  const QwEnergyCalculator* GetEnergyCalculator(const TString name) const;
  const QwHaloMonitor* GetScalerChannel(const TString name) const;


/////
protected:

  ///  \brief Adds a new element to a vector of data elements, and returns
  ///  the index of that element within the array.
  template <typename TT>
  Int_t AddToElementList(std::vector<TT> &elementlist, QwBeamDetectorID &detector_id);
  
  Int_t GetDetectorIndex(EQwBeamInstrumentType TypeID, TString name) const;
  //when the type and the name is passed the detector index from appropriate vector will be returned
  //for example if TypeID is bcm  then the index of the detector from fBCM vector for given name will be returnd.

  std::vector <VQwBPM_ptr> fStripline;
  std::vector <VQwBPM_ptr> fBPMCombo;

  std::vector <VQwBCM_ptr> fBCM;
  std::vector <VQwBCM_ptr> fBCMCombo;

  std::vector <VQwClock_ptr> fClock;

  std::vector <QwQPD> fQPD;
  std::vector <QwLinearDiodeArray> fLinearArray;
  std::vector <QwBPMCavity> fCavity;
  std::vector <QwHaloMonitor> fHaloMonitor;


  std::vector <QwEnergyCalculator> fECalculator;
  std::vector <QwBeamDetectorID> fBeamDetectorID;

/////
private:
  // std::vector<TString> DetectorTypes;// for example could be BCM, LUMI,BPMSTRIPLINE, etc..
  Int_t fQwBeamLineErrorCount;


  static const Bool_t bDEBUG=kFALSE;

};

#endif

/**********************************************************\
* File: QwHelicityPattern.h                                *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/
#ifndef __QwHelicityPattern__
#define __QwHelicityPattern__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwSubsystemArrayParity.h"
#include "QwEPICSEvent.h"
#include "QwBlinder.h"
//#include "VQwDataHandler.h"
#include "QwCorrelator.h"
#include "QwCombiner.h"
#include "LRBCorrector.h"

// Forward declarations
class QwHelicity;

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwHelicityPattern {
  /******************************************************************
   *  Class: QwHelicityPattern
   *
   *
   ******************************************************************/
 private:
  /// Private default constructor (not implemented, will throw linker error on use)
  QwHelicityPattern();
 public:
  /// Constructor with subsystem array
  QwHelicityPattern(QwSubsystemArrayParity &event, const TString &run = "0");
  /// \brief Copy constructor by reference
  QwHelicityPattern(const QwHelicityPattern& source);
  /// Virtual destructor
  virtual ~QwHelicityPattern() { };

  /// \brief Define the configuration options
  static void DefineOptions(QwOptions &options);
  /// \brief Process the configuration options
  void ProcessOptions(QwOptions &options);

  void  LoadEventData(QwSubsystemArrayParity &event);
  Bool_t HasDataLoaded() const { return fIsDataLoaded; };

  Bool_t IsCompletePattern() const;

  Bool_t IsEndOfBurst(){
    //  Is this the end of a burst?
    return (fBurstLength > 0 && fCurrentPatternNumber % fBurstLength == 0);
  }

  void  CalculateAsymmetry();
  void GetTargetChargeStat(Double_t & asym, Double_t & error, Double_t & width);//retrieves the target charge asymmetry,asymmetry error ,asymmetry width

  /// Enable/disable alternate asymmetry calculation
  void  EnableAlternateAsymmetry(const Bool_t flag = kTRUE) { fEnableAlternateAsym = flag; };
  /// Disable alternate asymmetry calculation
  void  DisableAlternateAsymmetry() { fEnableAlternateAsym = kFALSE; };
  /// Status of alternate asymmetry calculation flag
  Bool_t IsAlternateAsymEnabled() { return fEnableAlternateAsym; };

  /// Enable/disable burst sum calculation
  void  EnableBurstSum(const Bool_t flag = kTRUE) { fEnableBurstSum = flag; };
  /// Disable burst sum calculation
  void  DisableBurstSum() { fEnableBurstSum = kFALSE; };
  /// Status of burst sum calculation flag
  Bool_t IsBurstSumEnabled() { return fEnableBurstSum; };

  /// Enable/disable running sum calculation
  void  EnableRunningSum(const Bool_t flag = kTRUE) { fEnableRunningSum = flag; };
  /// Disable running sum calculation
  void  DisableRunningSum() { fEnableRunningSum = kFALSE; };
  /// Status of running sum calculation flag
  Bool_t IsRunningSumEnabled() { return fEnableRunningSum; };

  /// Enable/disable storing pattern differences
  void  EnableDifference(const Bool_t flag = kTRUE) { fEnableDifference = flag; };
  /// Disable storing pattern differences
  void  DisableDifference() { fEnableDifference = kFALSE; };
  /// Status of storing pattern differences flag
  Bool_t IsDifferenceEnabled() { return fEnableDifference; };

#ifdef __USE_DATABASE__
  /// Update the blinder status with new external information
  void UpdateBlinder(QwParityDB* db){
    fBlinder.Update(db);
  };
#endif

  /// Update the blinder status with new external information
  void UpdateBlinder(const QwSubsystemArrayParity& detectors) {
    fBlinder.Update(detectors);
  };
  /// Update the blinder status with new external information
  void UpdateBlinder(const QwEPICSEvent& epics) {
    fBlinder.Update(epics);
  };

  // wish these could be const references, but ConstructBranchAndVector messes with object
  QwSubsystemArrayParity& GetBurstYield()      { return fBurstYield; };
  QwSubsystemArrayParity& GetBurstDifference() { return fBurstDifference; };
  QwSubsystemArrayParity& GetBurstAsymmetry()  { return fBurstAsymmetry; };

  void  AccumulateBurstSum();
  void  AccumulateRunningBurstSum();
  void  AccumulateRunningSum(){AccumulateRunningSum(*this);};
  void  AccumulateRunningSum(QwHelicityPattern &entry);

  void  CalculateBurstAverage();
  void  CalculateRunningBurstAverage();
  void  CalculateRunningAverage();

  void  PrintRunningBurstAverage() const;
  void  PrintRunningAverage() const;
  void  PrintBurstAverage() const;

  void  ConstructHistograms(){ConstructHistograms((TDirectory*)NULL);};
  void  ConstructHistograms(TDirectory *folder);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile &trim_tree);
  void  FillTreeVector(std::vector<Double_t> &values) const;

#ifdef __USE_DATABASE__
  void  FillDB(QwParityDB *db);
  void  FillErrDB(QwParityDB *db);
#endif

  void  WritePromptSummary(QwPromptSummary *ps);

  Bool_t IsGoodAsymmetry();
  UInt_t GetEventcutErrorFlag() const{
    return fAsymmetry.GetEventcutErrorFlag();
  };

  void  ClearEventData();
  void  ClearBurstSum();
  void  ClearRunningSum();

  void  Print() const;

  void get_run_label(TString x) {
    run_label = x;
  }

  void ProcessDataHandlerEntry();
  void FinishDataHandler();

  LRBCorrector& return_regress_from_LRB() {
    return regress_from_LRB;
  }
  QwCombiner& return_regression() {
    return regression;
  }
  QwCombiner& return_running_regression() {
    return running_regression;
  }

 protected:
  Bool_t fDEBUG;

  //  QwHelicity* fHelicitySubsystem;

  std::vector<QwSubsystemArrayParity> fEvents;
  std::vector<Bool_t> fEventLoaded;
  std::vector<Int_t> fHelicity;// this is here up to when we code the Helicity decoding routine
  std::vector<Int_t> fEventNumber;
  Int_t fCurrentPatternNumber;
  Int_t fPatternSize;
  Int_t fQuartetNumber;

  // Blinding strategy
  QwBlinder fBlinder;

  /// This indicates if the subsystem arrays are missing the helicity object.
  /// It is updated once during initialization and once when processing the first event
  Bool_t fHelicityIsMissing;
  /// This is true if any of the helicity objects of this pattern have indicated that
  /// we should ignore the helicity.  It is updated every event and reset by ClearEventData.
  Bool_t fIgnoreHelicity;

  // Yield and asymmetry of a single helicity pattern
  QwSubsystemArrayParity fYield;
  QwSubsystemArrayParity fAsymmetry;
  // Alternate asymmetry calculations
  Bool_t fEnableAlternateAsym;
  QwSubsystemArrayParity fAsymmetry1;
  QwSubsystemArrayParity fAsymmetry2;

  // Burst sum/difference of the yield and asymmetry
  Int_t fBurstLength;
  Bool_t fEnableBurstSum;
  Bool_t fPrintBurstSum;
  QwSubsystemArrayParity fBurstYield;
  QwSubsystemArrayParity fBurstDifference;
  QwSubsystemArrayParity fBurstAsymmetry;
  QwSubsystemArrayParity fRunningBurstYield;
  QwSubsystemArrayParity fRunningBurstDifference;
  QwSubsystemArrayParity fRunningBurstAsymmetry;

  // Running sum/average of the yield and asymmetry
  Bool_t fEnableRunningSum;
  Bool_t fPrintRunningSum;

  Bool_t fEnableDifference;
  QwSubsystemArrayParity fDifference;
  QwSubsystemArrayParity fAlternateDiff;
  QwSubsystemArrayParity fPositiveHelicitySum;
  QwSubsystemArrayParity fNegativeHelicitySum;

  Long_t fLastWindowNumber;
  Long_t fLastPatternNumber;
  Int_t  fLastPhaseNumber;

  Bool_t fPatternIsGood;

  TString run_label;

  QwCorrelator correlator;
  LRBCorrector regress_from_LRB;
  QwCombiner regression;
  QwCombiner running_regression;

  // Flag to indicate that the pattern contains data
  Bool_t fIsDataLoaded;
  void SetDataLoaded(Bool_t flag) { fIsDataLoaded = flag; };

  friend class VQwDataHandler;
  friend class QwCombiner;
  friend class QwCorrelator;
  friend class LRBCorrector;

};


#endif

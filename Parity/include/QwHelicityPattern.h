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
#include <iostream>
#include <fstream>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwSubsystemArrayParity.h"
#include "QwEPICSEvent.h"
#include "QwBlinder.h"

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

  Bool_t PairAsymmetryIsGood();
  Bool_t NextPairIsComplete();
  void   CalculatePairAsymmetry();
  void   ClearPairData(){
    fPairYield.ClearEventData();
    fPairDifference.ClearEventData();
    fPairAsymmetry.ClearEventData();
  }

  Bool_t IsCompletePattern() const;

  void PrintIndexMapFile(Int_t runNum){
    if ( fPrintIndexFile && (fGoodPatterns < fBurstMinGoodPatterns) ) {
      // Print a text map file to max_burst_index.####.conf
      Int_t maxBurst = 0;
      if (fBurstCounter==0) {
        // It's a single burst run anyway
        maxBurst = 0;
      }
      else {
        // It's a multi burst run and we want to merge the final two bursts
        maxBurst = fBurstCounter-1;
      }
      QwWarning << "Printing max_burst_index." << runNum << ".conf file with " << maxBurst << " max burst number" << QwLog::endl;
      std::ofstream output;
      output.open(Form("max_burst_index.%d.conf",runNum));
      output<< "max-burst-index=" << maxBurst << std::endl; // Print the current index before incrementing further, this will be the max index and the next pass will overflow this one instead of having another
      output.close();
    }
  }

  Bool_t IsEndOfBurst(){
    //  Is this the end of a burst? And is this not the final burst?
    return (( fBurstLength > 0 && fGoodPatterns >= fBurstLength ) && ( fBurstCounter<fMaxBurstIndex ));
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

  /// Enable/disable storing pair differences
  void  EnablePairs(const Bool_t flag = kTRUE) { fEnablePairs = flag; };
  /// Disable storing pair differences
  void  DisablePairs() { fEnablePairs = kFALSE; };
  /// Status of storing pair differences flag
  Bool_t IsPairsEnabled() { return fEnablePairs; };

#ifdef __USE_DATABASE__
  /// Update the blinder status with new external information
  void UpdateBlinder(QwParityDB* db){
    fBlinder.Update(db);
  };
#endif
  /// Update the blinder status using a random number generator
  void UpdateBlinder(){
    fBlinder.Update();
  };
  /// Update the blinder status with new external information
  void UpdateBlinder(const QwSubsystemArrayParity& detectors) {
    fBlinder.Update(detectors);
  };
  /// Update the blinder status with new external information
  void UpdateBlinder(const QwEPICSEvent& epics) {
    fBlinder.Update(epics);
  };

  // wish these could be const references, but ConstructBranchAndVector messes with object
  QwSubsystemArrayParity& GetYield()      { return fYield; };
  QwSubsystemArrayParity& GetDifference() { return fDifference; };
  QwSubsystemArrayParity& GetAsymmetry()  { return fAsymmetry; };

  // wish these could be const references, but ConstructBranchAndVector messes with object
  QwSubsystemArrayParity& GetPairYield()      { return fPairYield; };
  QwSubsystemArrayParity& GetPairDifference() { return fPairDifference; };
  QwSubsystemArrayParity& GetPairAsymmetry()  { return fPairAsymmetry; };

  void  AccumulateRunningSum(QwHelicityPattern &entry, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void  AccumulatePairRunningSum(QwHelicityPattern &entry);

  void  CalculateRunningAverage();

  void  PrintValue() const;

  void  ConstructObjects(){ConstructObjects((TDirectory*)NULL);};
  void  ConstructObjects(TDirectory *folder);

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
  UInt_t GetEventcutErrorFlag() const {
    return fAsymmetry.GetEventcutErrorFlag();
  };
  const UInt_t* GetEventcutErrorFlagPointer() const {
    return fAsymmetry.GetEventcutErrorFlagPointer();
  };

  Bool_t HasBurstData(){return fGoodPatterns>0;};
  void  IncrementBurstCounter(){
    if (fBurstCounter < fMaxBurstIndex) {
      fBurstCounter++;
    }
    // Else we just park here and don't try to increment any more. This is a parameter from command line or map file
  }
  Short_t GetBurstCounter() const {return fBurstCounter;}
  void  ClearEventData();

  void  Print() const;

 protected:

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
  QwSubsystemArrayParity fDifference;
  QwSubsystemArrayParity fAsymmetry;
  // Alternate asymmetry calculations
  Bool_t fEnableAlternateAsym;
  QwSubsystemArrayParity fAsymmetry1;
  QwSubsystemArrayParity fAsymmetry2;

  // Yield and asymmetry of a single helicity pair
  Bool_t fEnablePairs;
  QwSubsystemArrayParity fPairYield;
  QwSubsystemArrayParity fPairDifference;
  QwSubsystemArrayParity fPairAsymmetry;

  // Burst sum/difference of the yield and asymmetry
  Int_t fBurstLength;
  Int_t fMaxBurstIndex;
  Bool_t fPrintIndexFile;
  Int_t fBurstMinGoodPatterns;
  Int_t fGoodPatterns;
  Short_t fBurstCounter;
  Bool_t fEnableBurstSum;
  Bool_t fPrintBurstSum;

  // Running sum/average of the yield and asymmetry
  Bool_t fEnableRunningSum;
  Bool_t fPrintRunningSum;

  Bool_t fEnableDifference;
  QwSubsystemArrayParity fAlternateDiff;
  QwSubsystemArrayParity fPositiveHelicitySum;
  QwSubsystemArrayParity fNegativeHelicitySum;

  Long_t fLastWindowNumber;
  Long_t fLastPatternNumber;
  Int_t  fLastPhaseNumber;

  size_t  fNextPair;
  Bool_t fPairIsGood;

  Bool_t fPatternIsGood;

  TString run_label;

  // Flag to indicate that the pattern contains data
  Bool_t fIsDataLoaded;
  void SetDataLoaded(Bool_t flag) { fIsDataLoaded = flag; };

  friend class QwDataHandlerArray;
};


#endif

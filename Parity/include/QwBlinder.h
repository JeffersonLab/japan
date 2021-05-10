/*!
 * \file   QwBlinder.h
 * \brief  A class for blinding data, adapted from G0 blinder class.
 *
 * \author Peiqing Wang
 * \date   2010-04-14
 */

#ifndef __QWBLINDER__
#define __QWBLINDER__

// System headers
#include <vector>
#include <iostream>
#include <iomanip>

// ROOT headers
#include <TString.h>
#include <TObjString.h>
#include <TMD5.h>

// Qweak headers
#include "QwSubsystemArrayParity.h"
#include "QwEPICSEvent.h"
#include "QwTypes.h"

// Forward declarations
#ifdef __USE_DATABASE__
class QwParityDB;
#endif

// Backup type definition for ULong64_t; needed with some older ROOT versions.
#if !defined(ULong64_t)
#if defined(R__WIN32) && !defined(__CINT__)
typedef unsigned __int64   ULong64_t; // Portable unsigned long integer 8 bytes
#else
typedef unsigned long long ULong64_t; // Portable unsigned long integer 8 bytes
#endif
#endif


/**
 * \class QwBlinder
 * \ingroup QwAnalysis
 *
 * \brief Class for blinding data, adapted from G0 blinder class
 *
 * 1. Asymmetry blinding scheme:
 * \f[
 *   Asym_{blinded} = (Asym_{actual} + fBlindingOffset) * fBlindingFactor
 * \f]
 * where \f$ fBlindingOffset = F \times sign(\lambda/2) \f$, F is an encrypted factor
 * with |F| < 0.06 ppm.
 * This offset fBlindingOffset will be applied on the block and blocksum of the asymmetry.
 *
 * 2. Difference blinding scheme:
 * For blinding the helicity correlated differences of the detectors, we'd have to do:
 * \f[
 *   Diff_{blinded} = (Diff_{raw} + Yield_{raw} * fBlindingOffset) * fBlindingFactor)
 * \f]
 * where \f$ Asym_{raw} = Diff_{raw} / Yield_{raw} \f$ is the unblinded asymmetry,
 * and \f$ Asym_{blinded} = Diff_{blinded} / Yield_{blinded} \f$ the blinded asymmetry.
 * Blinding the differences allows that the difference can be written to the output
 * ROOT files without compromising the blinding.
 *
 */
class QwBlinder {

 public:

  /// Available blinding strategies
  enum EQwBlindingStrategy {
    kDisabled,
    kAdditive,
    kMultiplicative,
    kAdditiveMultiplicative
  };
  /// Status of the blinding process or intermediate steps of the process  
  enum EQwBlinderStatus {
    kIndeterminate = 0,
    kNotBlindable,
    kBlindable,
    kBlindableFail
  };
  static const TString fStatusName[4];
  
  ///  Error flag value 
  static const UInt_t kErrorFlag_BlinderFail = 0x200;
  constexpr static const Double_t kValue_BlinderFail   = -1.0;

  static void DefineOptions(QwOptions &options);



    /// \brief Default constructor with optional database
    QwBlinder(const EQwBlindingStrategy blinding_strategy = kAdditive);
    /// \brief Default destructor
    virtual ~QwBlinder();

    /// \brief Update the status with new external information
    void ProcessOptions(QwOptions& options);
    /// \brief Update the status using a random number
    void Update();
    /// \brief Update the status with new external information
    void Update(QwParityDB* db);
    /// \brief Update the status with new external information
    void Update(const QwSubsystemArrayParity& detectors);
    /// \brief Update the status with new external information
    void Update(const QwEPICSEvent& epics);

    void ClearEventData(){
      fBeamIsPresent = kTRUE;
    };


    void  WriteFinalValuesToDB(QwParityDB* db);
    void  PrintCountersValues(std::vector<Int_t> fCounters, TString counter_type);
    void  PrintFinalValues(Int_t kVerbosity=1);


#ifdef __USE_DATABASE__
    /// Write to the database
    void FillDB(QwParityDB *db, TString datatype);
    void FillErrDB(QwParityDB *db, TString datatype);
#endif // __USE_DATABASE__

    /// Modifies the device error code variable passed to it, if the blinder is
    /// not okay.
    void ModifyThisErrorCode(UInt_t &errorcode) const {
      errorcode |= kErrorFlag_BlinderFail;
    };

    /// Asymmetry blinding
    void  BlindValue(Double_t& value) const {
      switch (fBlindingStrategy) {
        case kAdditive:
          value += fBlindingOffset; break;
        case kMultiplicative:
          value *= fBlindingFactor; break;
        case kAdditiveMultiplicative:
          value = (value + fBlindingOffset) * fBlindingFactor; break;
        default: break;
      }
    };
    /// Asymmetry unblinding
    void  UnBlindValue(Double_t& value) const {
      switch (fBlindingStrategy) {
        case kAdditive:
          value -= fBlindingOffset; break;
        case kMultiplicative:
          value /= fBlindingFactor; break;
        case kAdditiveMultiplicative:
          value = value / fBlindingFactor - fBlindingOffset; break;
        default: break;
      }
    };

    /// Difference blinding
    void  BlindValue(Double_t& value, const Double_t& yield) const {
      switch (fBlindingStrategy) {
      case kAdditive:
	value += yield * fBlindingOffset; break;
      case kMultiplicative:
	value *= fBlindingFactor; break;
      case kAdditiveMultiplicative:
	value = (value + fBlindingOffset * yield) * fBlindingFactor; break;
      default: break;
      }
    };
    /// Difference unblinding
    void  UnBlindValue(Double_t& value, const Double_t& yield) const {
      switch (fBlindingStrategy) {
        case kAdditive:
          value -= yield * fBlindingOffset; break;
        case kMultiplicative:
          value /= fBlindingFactor; break;
        case kAdditiveMultiplicative:
          value = value / fBlindingFactor - yield * fBlindingOffset; break;
        default: break;
      }
    };

    /// Blind the asymmetry of an array of subsystems
    void  Blind(QwSubsystemArrayParity& diff) {
      if (CheckBlindability(fPatternCounters)!=kNotBlindable)
	diff.Blind(this);
    };
    /// Blind the pair asymmetry 
    /// and only check fBlindingStrategy to avoid  overcounting fPatternCounters
    void  BlindPair(QwSubsystemArrayParity& diff) {
      if (CheckBlindability(fPairCounters)!=kNotBlindable)
	diff.Blind(this);
    };

    /// Unblind the asymmetry of an array of subsystems
    void  UnBlind(QwSubsystemArrayParity& diff) {
      diff.UnBlind(this);
    };

    /// Blind the difference of an array of subsystems
    void  Blind(QwSubsystemArrayParity& diff, const QwSubsystemArrayParity& yield) {
      if (CheckBlindability(fPatternCounters)!=kNotBlindable)
	diff.Blind(this, yield);
    };
    /// Blind the pair difference of an array of subsystems
    void  BlindPair(QwSubsystemArrayParity& diff, const QwSubsystemArrayParity& yield) {
      if (CheckBlindability(fPairCounters)!=kNotBlindable)
	diff.Blind(this, yield);
    };

    /// Unblind the difference of an array of subsystems
    void  UnBlind(QwSubsystemArrayParity& diff, const QwSubsystemArrayParity& yield) {
      diff.UnBlind(this, yield);
    };

    const Bool_t& IsBlinderOkay() const {return fBlinderIsOkay;};

    void ConstructObjects(TDirectory *folder, TString &prefix) {
      if (folder != NULL) folder->cd();
      const TObjString* seed = new TObjString(fSeed);
      folder->WriteTObject(seed, prefix + "seed", "WriteDelete");
      const TObjString* seedID = new TObjString(Form("%u",fSeedID));
      folder->WriteTObject(seedID, prefix + "seedID", "WriteDelete");
      const TObjString* strategy = new TObjString(Form("%u", fBlindingStrategy));
      folder->WriteTObject(strategy, prefix + "strategy", "WriteDelete");
      const TObjString* max_asymmetry = new TObjString(Form("%f",fMaximumBlindingAsymmetry));
      folder->WriteTObject(max_asymmetry, prefix + "max_asymmetry", "WriteDelete");
      const TObjString* max_factor = new TObjString(Form("%f",fMaximumBlindingFactor));
      folder->WriteTObject(max_factor, prefix + "max_factor", "WriteDelete");
      const TObjString* checksum = new TObjString(fChecksum.c_str());
      folder->WriteTObject(checksum, prefix + "checksum", "WriteDelete");
    };

 private:
    ///  Indicates the first value recieved of the blindability of the target 
    EQwBlinderStatus fTargetBlindability_firstread;
    EQwBlinderStatus fTargetBlindability;
    Bool_t fTargetPositionForced;
    EQwWienMode fWienMode_firstread;
    EQwWienMode fWienMode;
    Int_t fIHWPPolarity_firstread;
    Int_t fIHWPPolarity;
    Bool_t fSpinDirectionForced;
    void SetTargetBlindability(EQwBlinderStatus status);
    void SetWienState(EQwWienMode wienmode);
    void SetIHWPPolarity(Int_t ihwppolarity);

    //  Target position look-up index for PREX/CREX.
    //  Index value of -1 is for the PREX positons
    //  CREX index values in date order go from "min" to "Max" and must correspond to cases in the QwBlinder::Update(const QwEPICSEvent& epics) function.
    Int_t  fCREXTargetIndex;
    Int_t  kCREXTgtIndexMin = 1;
    Int_t  kCREXTgtIndexMax = 2;

    Double_t fBeamCurrentThreshold;
    Bool_t fBeamIsPresent;

    EQwBlinderStatus CheckBlindability(std::vector<Int_t> &fCounters);
    Bool_t fBlinderIsOkay;

    


 private:

    /// Private copy constructor
    QwBlinder (const QwBlinder& __attribute__((unused)) blinder): fBlindingStrategy(kDisabled) { };
    /// Private assignment operator
    const QwBlinder& operator= (const QwBlinder& __attribute__((unused)) blinder) { return *this; };

    //  Variables and functions used in blinding the detector asymmetries
    EQwBlindingStrategy fBlindingStrategy; /// Blinding strategy
    Double_t fBlindingOffset; /// The term to be added to detector asymmetries
    Double_t fBlindingOffset_Base; /// The term to be added to detector asymmetries, before polarity correction
    Double_t fBlindingFactor; /// The factor to be mutliplied to detector asymmetries


    static const Double_t kDefaultMaximumBlindingAsymmetry; /// Default maximum blinding asymmetry (in ppm)
    static const Double_t kDefaultMaximumBlindingFactor;    /// Default maximum blinding factor (in fraction from identity)

    Double_t fMaximumBlindingAsymmetry; /// Maximum blinding asymmetry (in ppm)
    Double_t fMaximumBlindingFactor;    /// Maximum blinding factor (in fraction from identity)

    UInt_t fSeedID;      /// ID of seed used (seeds.seed_id)
    TString fSeed;       /// Seed string (seeds.seed)
    static const TString kDefaultSeed; /// Default seed

    std::vector<UChar_t> fDigest;         /// Checksum in raw hex
    std::string          fChecksum;       /// Checksum in ASCII hex

    std::vector<double> fTestValues;        /// Vector of test values, original
    std::vector<double> fBlindTestValues;   /// Vector of test values, after blinding
    std::vector<double> fUnBlindTestValues; /// Vector of test values, after unblinding

    void InitBlinders(const UInt_t seed_id); /// Initializes fBlindingFactor from fSeed.

    void InitTestValues(const int n);    /// Initializes the test values: fTestNumber, fTestValue,
                                         /// fBlindTestValue, if fBlindingFactor is set.
    Bool_t CheckTestValues();            /// Recomputes fBlindTestValue to check for memory errors


    Int_t UseMD5(const TString &barestring);          /// Returns an integer from a string using MD5

    Int_t UseStringManip(const TString &barestring);  /// Returns an integer from a string using
                                                      /// a character manipulation algorithm

    Int_t UsePseudorandom(const TString &barestring); /// Returns an integer from a string using
                                                      /// a version of the helicity bit pseudorandom algorithm.


    ///  Reads the seed with specified id from the database object
    Int_t ReadSeed(QwParityDB* db, const UInt_t seed_id);

    ///  Reads the seed from the database object
    Int_t ReadSeed(QwParityDB* db);

    ///  Read the seed string generated utilizing a random number generator
    Int_t ReadRandomSeed();

    void WriteChecksum(QwParityDB* db);     ///  Writes fSeedID and fBFChecksum to DB for this analysis ID
    void WriteTestValues(QwParityDB* db);   ///  Writes fTestNumber and fBlindTestValue to DB for this analysis ID

    std::vector<UChar_t> GenerateDigest(const TString& input) const;

    std::vector<Int_t> fPatternCounters; ///< Counts the number of events in each failure mode
    std::vector<Int_t> fPairCounters; ///< Counts the number of helicity pairs in each failure mode
};

#endif //__QWBLINDER__

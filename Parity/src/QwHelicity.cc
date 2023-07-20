/**********************************************************\
* File: QwHelicity.C                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwHelicity.h"

// System headers
#include <stdexcept>

// ROOT headers
#include "TRegexp.h"
#include "TMath.h"

// Qweak headers
#include "QwHistogramHelper.h"
#ifdef __USE_DATABASE__
#define MYSQLPP_SSQLS_NO_STATICS
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__
#include "QwLog.h"

extern QwHistogramHelper gQwHists;
//**************************************************//

// Register this subsystem with the factory
RegisterSubsystemFactory(QwHelicity);


/// Default helicity bit pattern of 0x69 represents a -++-+--+ octet
/// (event polarity listed in reverse time order), where the LSB
/// of the bit pattern is the first event of the pattern.
const std::vector<UInt_t> QwHelicity::kDefaultHelicityBitPattern{0x69};

//**************************************************//
/// Constructor with name
QwHelicity::QwHelicity(const TString& name)
: VQwSubsystem(name),
  VQwSubsystemParity(name),
  fInputReg_HelPlus(kDefaultInputReg_HelPlus),
  fInputReg_HelMinus(kDefaultInputReg_HelMinus),
  fInputReg_PatternSync(kDefaultInputReg_PatternSync),
  fInputReg_PairSync(0),
  fHelicityBitPattern(kDefaultHelicityBitPattern),
  kPatternCounter(-1), kMpsCounter(-1), kPatternPhase(-1),
  fMinPatternPhase(1), fUsePredictor(kTRUE), fIgnoreHelicity(kFALSE),
  fEventNumberFirst(-1),fPatternNumberFirst(-1),
  fSuppressMPSErrorMsgs(kFALSE)
{
  ClearErrorCounters();
  // Default helicity delay to two patterns.
  fHelicityDelay = 2;
  // Default the EventType flags to HelPlus=1 and HelMinus=4
  // These are only used in Moller decoding mode.
  kEventTypeHelPlus  = 4;
  kEventTypeHelMinus = 1;
  //
  fEventNumberOld=-1; fEventNumber=-1;
  fPatternPhaseNumberOld=-1; fPatternPhaseNumber=-1;
  fPatternNumberOld=-1;  fPatternNumber=-1;
  kUserbit=-1;
  fActualPatternPolarity=kUndefinedHelicity;
  fDelayedPatternPolarity=kUndefinedHelicity;
  fHelicityReported=kUndefinedHelicity;
  fHelicityActual=kUndefinedHelicity;
  fHelicityDelayed=kUndefinedHelicity;
  fHelicityBitPlus=kFALSE;
  fHelicityBitMinus=kFALSE;
  n_ranbits = 0;
  fGoodHelicity=kFALSE;
  fGoodPattern=kFALSE;
  fHelicityDecodingMode=-1;

  fInputReg_FakeMPS = kDefaultInputReg_FakeMPS;
}

//**************************************************//
/// Copy constructor
// FIXME this is pretty horrible as a copy constructor, but it gets around
// all of the copy protection built into the helicity subsystem.  I can't be
// bothered to clean it up right now... (wdc)
QwHelicity::QwHelicity(const QwHelicity& source)
: VQwSubsystem(source.GetName()),
  VQwSubsystemParity(source.GetName()),
  fInputReg_HelPlus(source.fInputReg_HelPlus),
  fInputReg_HelMinus(source.fInputReg_HelMinus),
  fInputReg_PatternSync(source.fInputReg_PatternSync),
  fInputReg_PairSync(source.fInputReg_PairSync),
  //fHelicityBitPattern(source.fHelicityBitPattern),
  kPatternCounter(source.kPatternCounter),
  kMpsCounter(source.kMpsCounter),
  kPatternPhase(source.kPatternPhase),
  fMinPatternPhase(1), fUsePredictor(kTRUE), fIgnoreHelicity(kFALSE),
  fEventNumberFirst(-1),fPatternNumberFirst(-1),
  fSuppressMPSErrorMsgs(kFALSE)
{
  fHelicityBitPattern = source.fHelicityBitPattern; 
  //std::cout << source.fHelicityBitPattern.size() << " " << fHelicityBitPattern.size() << std::endl;
  ClearErrorCounters();
  // Default helicity delay to two patterns.
  fHelicityDelay = 2;
  // Default the EventType flags to HelPlus=1 and HelMinus=4
  // These are only used in Moller decoding mode.
  kEventTypeHelPlus  = 4;
  kEventTypeHelMinus = 1;
  //
  fEventNumberOld=-1; fEventNumber=-1;
  fPatternPhaseNumberOld=-1; fPatternPhaseNumber=-1;
  fPatternNumberOld=-1;  fPatternNumber=-1;
  kUserbit=-1;
  fActualPatternPolarity=kUndefinedHelicity;
  fDelayedPatternPolarity=kUndefinedHelicity;
  fHelicityReported=kUndefinedHelicity;
  fHelicityActual=kUndefinedHelicity;
  fHelicityDelayed=kUndefinedHelicity;
  fHelicityBitPlus=kFALSE;
  fHelicityBitMinus=kFALSE;
  fGoodHelicity=kFALSE;
  fGoodPattern=kFALSE;
  fHelicityDecodingMode=-1;

  fInputReg_FakeMPS = source.fInputReg_FakeMPS;

  this->fWord.resize(source.fWord.size());
  for(size_t i=0;i<this->fWord.size();i++)
    {
      this->fWord[i].fWordName=source.fWord[i].fWordName;
      this->fWord[i].fModuleType=source.fWord[i].fModuleType;
      this->fWord[i].fWordType=source.fWord[i].fWordType;
    }
  fNumMissedGates = source.fNumMissedGates;
  fNumMissedEventBlocks = source.fNumMissedEventBlocks;
  fNumMultSyncErrors = source.fNumMultSyncErrors;
  fNumHelicityErrors = source.fNumHelicityErrors;
  fEventNumberFirst = source.fEventNumberFirst;
  fPatternNumberFirst = source.fPatternNumberFirst;
  fEventType = source.fEventType;
  fIgnoreHelicity = source.fIgnoreHelicity;
  fRandBits = source.fRandBits;
  fUsePredictor = source.fUsePredictor;
  fHelicityInfoOK = source.fHelicityInfoOK;
  fPatternPhaseOffset = source.fPatternPhaseOffset;
  fMinPatternPhase = source.fMinPatternPhase;
  fMaxPatternPhase = source.fMaxPatternPhase;
  fHelicityDelay = source.fHelicityDelay;
  iseed_Delayed = source.iseed_Delayed;
  iseed_Actual = source.iseed_Actual;
  n_ranbits = source.n_ranbits;
  fEventNumber = source.fEventNumber;
  fEventNumberOld = source.fEventNumberOld;
  fPatternPhaseNumber = source.fPatternPhaseNumber;
  fPatternPhaseNumberOld = source.fPatternPhaseNumberOld;
  fPatternNumber = source.fPatternNumber;
  fPatternNumberOld = source.fPatternNumberOld;

  this->kUserbit = source.kUserbit;
  this->fIgnoreHelicity = source.fIgnoreHelicity;
}

//**************************************************//
void QwHelicity::DefineOptions(QwOptions &options)
{
  options.AddOptions("Helicity options")
      ("helicity.seed", po::value<int>(),
          "Number of bits in random seed");
  options.AddOptions("Helicity options")
      ("helicity.bitpattern", po::value<std::string>(),
          "Helicity bit pattern: 0x1 (pair), 0x9 (quartet), 0x69 (octet), 0x666999 (hexo-quad), 0x66669999 (octo-quad)");
  options.AddOptions("Helicity options")
      ("helicity.patternoffset", po::value<int>(),
          "Set 1 when pattern starts with 1 or 0 when starts with 0");
  options.AddOptions("Helicity options")
      ("helicity.patternphase", po::value<int>(),
          "Maximum pattern phase");
  options.AddOptions("Helicity options")
      ("helicity.delay", po::value<int>(),
          "Default delay is 2 patterns, set at the helicity map file.");
  options.AddOptions("Helicity options")
      ("helicity.toggle-mode", po::value<bool>()->default_bool_value(false),
          "Activates helicity toggle-mode, overriding the 'delay', 'patternphase', 'bitpattern', and 'seed' options.");
}

//**************************************************//

void QwHelicity::ProcessOptions(QwOptions &options)
{
  // Read the cmd options and override channel map settings
  QwMessage << "QwHelicity::ProcessOptions" << QwLog::endl;
  if (options.HasValue("helicity.patternoffset")) {
    if (options.GetValue<int>("helicity.patternoffset") == 1
     || options.GetValue<int>("helicity.patternoffset") == 0) {
      fPatternPhaseOffset = options.GetValue<int>("helicity.patternoffset");
      QwMessage << " Pattern Phase Offset = " << fPatternPhaseOffset << QwLog::endl;
    } else QwError << "Pattern phase offset should be 0 or 1!" << QwLog::endl;
  }

  if (options.HasValue("helicity.patternphase")) {
    if (options.GetValue<int>("helicity.patternphase") % 2 == 0) {
      fMaxPatternPhase = options.GetValue<int>("helicity.patternphase");
      QwMessage << " Maximum Pattern Phase = " << fMaxPatternPhase << QwLog::endl;
    } else QwError << "Pattern phase should be an even integer!" << QwLog::endl;
  }

  if (options.HasValue("helicity.seed")) {
    if (options.GetValue<int>("helicity.seed") == 24
     || options.GetValue<int>("helicity.seed") == 30) {
      QwMessage << " Random Bits = " << options.GetValue<int>("helicity.seed") << QwLog::endl;
      fRandBits = options.GetValue<int>("helicity.seed");
    } else QwError << "Number of random seed bits should be 24 or 30!" << QwLog::endl;
  }

  if (options.HasValue("helicity.delay")) {
    QwMessage << " Helicity Delay = " << options.GetValue<int>("helicity.delay") << QwLog::endl;
    SetHelicityDelay(options.GetValue<int>("helicity.delay"));
  }

  if (options.HasValue("helicity.bitpattern")) {
    QwMessage << " Helicity Pattern =" 
	      << options.GetValue<std::string>("helicity.bitpattern") 
	      << QwLog::endl;
    std::string hex = options.GetValue<std::string>("helicity.bitpattern");
    //UInt_t bits = QwParameterFile::GetUInt(hex);
    SetHelicityBitPattern(hex);
  } else {
    BuildHelicityBitPattern(fMaxPatternPhase);
  }

  if (options.GetValue<bool>("helicity.toggle-mode")) {
    fHelicityDelay   = 0;
    fUsePredictor    = kFALSE;
    fMaxPatternPhase = 2;
    fHelicityBitPattern = kDefaultHelicityBitPattern;
  }

  //  If we have the default Helicity Bit Pattern & a large fMaxPatternPhase,
  //  try to recompute the Helicity Bit Pattern.
  if (fMaxPatternPhase > 8 && fHelicityBitPattern == kDefaultHelicityBitPattern) {
    BuildHelicityBitPattern(fMaxPatternPhase);
  }

  //  Here we're going to try to get the "online" option which
  //  is defined by QwEventBuffer.
  if (options.HasValue("online")){
    fSuppressMPSErrorMsgs = options.GetValue<bool>("online");
  } else {
    fSuppressMPSErrorMsgs = kFALSE;
  }
}


Bool_t QwHelicity::IsContinuous()
{
  Bool_t results=kFALSE;
  if(IsGoodPatternNumber()&&IsGoodEventNumber()&&IsGoodPhaseNumber()){
    results=kTRUE;
  } else {
    //  Results is already false, so just set the error flag value.
    fErrorFlag = kErrorFlag_Helicity + kGlobalCut + kEventCutMode3;
  }
  return results;
}


Bool_t QwHelicity::IsGoodPatternNumber()
{
  Bool_t results;
  
  if((fPatternNumber == fPatternNumberOld) && (fPatternPhaseNumber == fPatternPhaseNumberOld+1))//same pattern new phase
       results = kTRUE; //got same pattern
  else if((fPatternNumber == fPatternNumberOld + 1) && (fPatternPhaseNumber == fMinPatternPhase))
       results=kTRUE; //new pattern
  else results=kFALSE; //wrong pattern

  if(!results) {
    QwWarning << "QwHelicity::IsGoodPatternNumber:  This is not a good pattern number. New = "<< fPatternNumber << " Old = " <<  fPatternNumberOld << QwLog::endl;
    //Print();
  }

  return results;
}


Bool_t QwHelicity::IsGoodEventNumber()
{
  Bool_t results;
  if(fEventNumber == fEventNumberOld + 1)
    results= kTRUE;
  else
    results= kFALSE;

  if(!results) {
    QwWarning << "QwHelicity::IsGoodEventNumber: \n this is not a good event number indeed:" << QwLog::endl;
    Print();
  }
  return results;
}


Bool_t QwHelicity::IsGoodPhaseNumber()
{
  Bool_t results;

  if((fPatternPhaseNumber == fMaxPatternPhase)  && (fPatternNumber == fPatternNumberOld )) //maximum phase of old pattern
     results = kTRUE;
  else if((fPatternPhaseNumber == fPatternPhaseNumberOld+1) && (fPatternNumber == fPatternNumberOld))
    results = kTRUE;
  else if((fPatternPhaseNumber == fMinPatternPhase) && (fPatternNumber == fPatternNumberOld + 1))
    results= kTRUE;
  else
    results = kFALSE;

  if(fPatternPhaseNumber>fMaxPatternPhase)
    results=kFALSE;

  if(!results) {
    QwWarning << "QwHelicity::IsGoodPhaseNumber:  not a good phase number \t"
	      <<  "Phase: " << fPatternPhaseNumber << " out of "
	      <<  fMaxPatternPhase
	      <<  "(was "  << fPatternPhaseNumberOld << ")"
	      <<  "\tPattern #" << fPatternNumber << "(was "
	      <<  fPatternNumberOld  << ")"
	      <<  QwLog::endl; //Paul's modifications
    Print();
  }

  return results;
}


Bool_t QwHelicity::IsGoodHelicity()
{
  fGoodHelicity = kTRUE;
  if (!fIgnoreHelicity  && fHelicityReported!=fHelicityDelayed){
    /**We are not ignoring the helicity, and the helicities do not match.
       Check phase number to see if its a new pattern.*/
    fGoodHelicity=kFALSE;
    fNumHelicityErrors++;
    fErrorFlag = kErrorFlag_Helicity + kGlobalCut + kEventCutMode3;
    if(fPatternPhaseNumber == fMinPatternPhase) {
      //first event in a new pattern
      QwError << "QwHelicity::IsGoodHelicity : The helicity reported in event "
	      << fEventNumber
	      << " is not what we expect from the randomseed. Not a good event nor pattern"
	      << QwLog::endl;
    } else {
      QwError << "QwHelicity::IsGoodHelicity - The helicity reported in event "
	      << fEventNumber
	      << " is not what we expect according to pattern structure. Not a good event nor pattern"
	      << QwLog::endl;
    }
  }
  if(!fGoodHelicity) {
    fHelicityReported=kUndefinedHelicity;
    fHelicityActual=kUndefinedHelicity;
    fHelicityDelayed=kUndefinedHelicity;
    //Have to start over again
    ResetPredictor();
  }
  
  return fGoodHelicity;
}


void QwHelicity::ClearEventData()
{
  SetDataLoaded(kFALSE);
  for (size_t i=0;i<fWord.size();i++)
    fWord[i].ClearEventData();

  /**Reset data by setting the old event number, pattern number and pattern phase 
     to the values of the previous event.*/
  if (fEventNumberFirst==-1 && fEventNumberOld!= -1){
    fEventNumberFirst = fEventNumberOld;
  }
  if (fPatternNumberFirst==-1 && fPatternNumberOld!=-1 
      && fPatternNumber==fPatternNumberOld+1){
    fPatternNumberFirst = fPatternNumberOld;
  }

  fEventNumberOld = fEventNumber;
  fPatternNumberOld = fPatternNumber;
  fPatternPhaseNumberOld = fPatternPhaseNumber;

  //fIgnoreHelicity = kFALSE;

  /**Clear out helicity variables */
  fHelicityReported = kUndefinedHelicity;
  fHelicityActual = kUndefinedHelicity;
  fHelicityDelayed= kUndefinedHelicity;
  fHelicityBitPlus = kFALSE;
  fHelicityBitMinus = kFALSE;
  // be careful: it is not that I forgot to reset fActualPatternPolarity
  // or fDelayedPatternPolarity. One doesn't want to do that here.
  /** Set the new event number and pattern number to -1. If we are not reading these correctly
      from the data stream, -1 will allow us to identify that.*/
  fEventNumber = -1;
  fPatternPhaseNumber = -1;
  fPatternNumber = -1;
  return;
}

Int_t QwHelicity::ProcessConfigurationBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  //stub function
  // QwError << " this function QwHelicity::ProcessConfigurationBuffer does nothing yet " << QwLog::endl;
  return 0;
}

Int_t QwHelicity::LoadInputParameters(TString pedestalfile)
{
  return 0;
}


Bool_t QwHelicity::ApplySingleEventCuts(){
  //impose single event cuts //Paul's modifications

  return kTRUE;
}

void QwHelicity::IncrementErrorCounters()
{
  /// TODO:  Implement  QwHelicity::IncrementErrorCounters()
}

void QwHelicity::PrintErrorCounters() const{
  // report number of events failed due to HW and event cut faliure
  QwMessage << "\n*********QwHelicity Error Summary****************"
	    << QwLog::endl;
  QwMessage << "First helicity gate counter:  "
	    << fEventNumberFirst
	    << "; last helicity gate counter:  "
	    << fEventNumber
	    << QwLog::endl;
  QwMessage << "First pattern counter:  "
	    << fPatternNumberFirst
	    << "; last pattern counter:  "
	    << fPatternNumber
	    << QwLog::endl;
  QwMessage << "Missed " << fNumMissedGates << " helicity gates in "
	    << fNumMissedEventBlocks << " blocks of missed events."
	    << QwLog::endl;
  QwMessage << "Number of multiplet-sync-bit errors:  "
	    << fNumMultSyncErrors
	    << QwLog::endl;
  QwMessage << "Number of helicity prediction errors: "
	    << fNumHelicityErrors
	    << QwLog::endl;
  QwMessage <<"---------------------------------------------------\n"
	    << QwLog::endl;
}

UInt_t QwHelicity::GetEventcutErrorFlag(){//return the error flag
  return fErrorFlag;
}

void QwHelicity::ProcessEventUserbitMode()
{

  /** In this version of the code, the helicity is extracted for a userbit configuration.
      This is not what we plan to have for Qweak but it was done for injector tests and 
      so is usefull to have as another option to get helicity information. */
  
  Bool_t ldebug=kFALSE;
  UInt_t userbits;
  static UInt_t lastuserbits  = 0xFF;
  UInt_t scaleroffset=fWord[kScalerCounter].fValue/32;

  if(scaleroffset==1 || scaleroffset==0) {
    userbits = (fWord[kUserbit].fValue & 0xE0000000)>>28;

    //  Now fake the input register, MPS coutner, QRT counter, and QRT phase.
    fEventNumber=fEventNumberOld+1;

    lastuserbits = userbits;

    if (lastuserbits==0xFF) {
      fPatternPhaseNumber    = fMinPatternPhase;
    } else {
      if ((lastuserbits & 0x8) == 0x8) {
	//  Quartet bit is set.
	fPatternPhaseNumber    = fMinPatternPhase;  // Reset the QRT phase
	fPatternNumber=fPatternNumberOld+1;     // Increment the QRT counter
      } else {
	fPatternPhaseNumber=fPatternPhaseNumberOld+1;       // Increment the QRT phase
      }

      fHelicityReported=0;

      if ((lastuserbits & 0x4) == 0x4){ //  Helicity bit is set.
	fHelicityReported    |= 1; // Set the InputReg HEL+ bit.
	fHelicityBitPlus=kTRUE;
	fHelicityBitMinus=kFALSE;
      } else {
	fHelicityReported    |= 0; // Set the InputReg HEL- bit.
	fHelicityBitPlus=kFALSE;
	fHelicityBitMinus=kTRUE;
      }
    }
  } else {
    QwError << " QwHelicity::ProcessEvent finding a missed read event in the scaler" << QwLog::endl;
    if(ldebug) {
      std::cout << " QwHelicity::ProcessEvent :" << scaleroffset << " events were missed \n";
      std::cout << " before manipulation \n";
      Print();
    }
    //there was more than one event since the last reading of the scalers
    //ie we should read only one event at the time,
    //if not something is wrong
    fEventNumber=fEventNumberOld+scaleroffset;
    Int_t localphase=fPatternPhaseNumberOld;
    Int_t localpatternnumber=fPatternNumberOld;
    for (UInt_t i=0;i<scaleroffset;i++) {
      fPatternPhaseNumber=localphase+1;
      if(fPatternPhaseNumber>fMaxPatternPhase) {
	fPatternNumber=localpatternnumber+fPatternPhaseNumber/fMaxPatternPhase;
	fPatternPhaseNumber=fPatternPhaseNumber-fMaxPatternPhase;
	localpatternnumber=fPatternNumber;
      }
      localphase=fPatternPhaseNumber;
    }
    //Reset helicity predictor because we are not sure of what we are doing
    fHelicityReported=-1;
    ResetPredictor();
    if(ldebug) {
      std::cout << " after manipulation \n";
      Print();
    }
  }
  return;
}


void QwHelicity::ProcessEventInputRegisterMode()
{
  static Bool_t firstevent   = kTRUE;
  static Bool_t firstpattern = kTRUE;
  static Bool_t fake_the_counters=kFALSE;
  UInt_t thisinputregister=fWord[kInputRegister].fValue;

  if (firstpattern){
    //  If any of the special counters are negative or zero, setup to
    //  generate the counters internally.
    fake_the_counters |= (kPatternCounter<=0)
      || ( kMpsCounter<=0) || (kPatternPhase<=0);
  }
  
  if (CheckIORegisterMask(thisinputregister,fInputReg_FakeMPS))
    fIgnoreHelicity = kTRUE;
  else 
    fIgnoreHelicity = kFALSE;

  /** If we get junk for the mps and pattern information from the run
      we can enable fake counters for mps, pattern number and pattern
      phase to get the job done.
  */
  if (!fake_the_counters){
    /**
       In the Input Register Mode,
       the event number is obtained straight from the wordkMPSCounter.
    */
    fEventNumber=fWord[kMpsCounter].fValue;
    // When we have the minimum phase from the pattern phase word
    // and the input register minimum phase bit is set
    // we can select the second pattern as below.
    if(fWord[kPatternPhase].fValue - fPatternPhaseOffset == 0)
      if (firstpattern && CheckIORegisterMask(thisinputregister,fInputReg_PatternSync)){
	firstpattern   = kFALSE;
      }
    
    // If firstpattern is still TRUE, we are still searching for the first
    // pattern of the data stream. So set the pattern number = 0
    if (firstpattern)
      fPatternNumber      = -1;
    else {
      fPatternNumber      = fWord[kPatternCounter].fValue;
      fPatternPhaseNumber = fWord[kPatternPhase].fValue - fPatternPhaseOffset + fMinPatternPhase;
    }
  } else {
    //  Use internal variables for all the counters.
    fEventNumber = fEventNumberOld+1;
    if (CheckIORegisterMask(thisinputregister,fInputReg_PatternSync)) {
      fPatternPhaseNumber = fMinPatternPhase;
      fPatternNumber      = fPatternNumberOld + 1;
    } else  {
      fPatternPhaseNumber = fPatternPhaseNumberOld + 1;
      fPatternNumber      = fPatternNumberOld;
    }
  }


  if (firstevent){
    firstevent = kFALSE;
  } else if(fEventNumber!=(fEventNumberOld+1)){
    Int_t nummissed(fEventNumber - (fEventNumberOld+1));
    if (!fSuppressMPSErrorMsgs){
      QwError << "QwHelicity::ProcessEvent read event# ("
	      << fEventNumber << ") is not  old_event#+1; missed "
	      << nummissed << " gates" << QwLog::endl;
    }
    fNumMissedGates += nummissed;
    fNumMissedEventBlocks++;
    fErrorFlag = kErrorFlag_Helicity + kGlobalCut + kEventCutMode3;
  }

  if (CheckIORegisterMask(thisinputregister,fInputReg_PatternSync) && fPatternPhaseNumber != fMinPatternPhase){
    //  Quartet bit is set.
    QwError << "QwHelicity::ProcessEvent:  The Multiplet Sync bit is set, but the Pattern Phase is (" 
	    << fPatternPhaseNumber << ") not "
	    << fMinPatternPhase << "!  Please check the fPatternPhaseOffset in the helicity map file." << QwLog::endl;
    fNumMultSyncErrors++;
    fErrorFlag = kErrorFlag_Helicity + kGlobalCut + kEventCutMode3;
  }

  fHelicityReported=0;

  /**
     Extract the reported helicity from the input register for each event.
  */

  if (CheckIORegisterMask(thisinputregister,fInputReg_HelPlus)
      && CheckIORegisterMask(thisinputregister,fInputReg_HelMinus) ){
    //  Both helicity bits are set.
    QwError << "QwHelicity::ProcessEvent:  Both the H+ and H- bits are set: thisinputregister==" 
	    << thisinputregister << QwLog::endl;
    fHelicityReported = kUndefinedHelicity;
    fHelicityBitPlus  = kFALSE;
    fHelicityBitMinus = kFALSE;
  } else if (CheckIORegisterMask(thisinputregister,fInputReg_HelPlus)){ //  HelPlus bit is set.
    fHelicityReported    |= 1; // Set the InputReg HEL+ bit.
    fHelicityBitPlus  = kTRUE;
    fHelicityBitMinus = kFALSE;
  } else {
    fHelicityReported    |= 0; // Set the InputReg HEL- bit.
    fHelicityBitPlus  = kFALSE;
    fHelicityBitMinus = kTRUE;
  }
  
  return;
}

void QwHelicity::ProcessEventInputMollerMode()
{
  static Bool_t firstpattern = kTRUE;

  if(firstpattern && fWord[kPatternCounter].fValue > fPatternNumberOld){
    firstpattern = kFALSE;
  }
  
  fEventNumber=fWord[kMpsCounter].fValue;
  if(fEventNumber!=(fEventNumberOld+1)){
    Int_t nummissed(fEventNumber - (fEventNumberOld+1));
    QwError << "QwHelicity::ProcessEvent read event# ("
	    << fEventNumber << ") is not  old_event#+1; missed "
	    << nummissed << " gates" << QwLog::endl;
    fNumMissedGates += nummissed;
    fNumMissedEventBlocks++;
  }
  if (firstpattern){
    fPatternNumber      = -1;
    fPatternPhaseNumber = fMinPatternPhase;
  } else {
    fPatternNumber = fWord[kPatternCounter].fValue;
    if (fPatternNumber > fPatternNumberOld){
      //  We are at a new pattern!
      fPatternPhaseNumber  = fMinPatternPhase;
    } else {
      fPatternPhaseNumber  = fPatternPhaseNumberOld + 1;
    }
  }
  
  if (fEventType == kEventTypeHelPlus)       fHelicityReported=1;
  else if (fEventType == kEventTypeHelMinus) fHelicityReported=0;
  //  fHelicityReported = (fEventType == 1 ? 0 : 1);

  if (fHelicityReported == 1){
    fHelicityBitPlus=kTRUE;
    fHelicityBitMinus=kFALSE;
  } else {
    fHelicityBitPlus=kFALSE;
    fHelicityBitMinus=kTRUE;
  }
  return;
}


void  QwHelicity::ProcessEvent()
{
  Bool_t ldebug = kFALSE;
  fErrorFlag = 0;

  if (! HasDataLoaded()) return;

  switch (fHelicityDecodingMode)
    {
    case kHelUserbitMode :
      ProcessEventUserbitMode();
      break;
    case kHelInputRegisterMode :
      ProcessEventInputRegisterMode();
      break;
    case kHelInputMollerMode :
      ProcessEventInputMollerMode();
      break;
    default:
      QwError << "QwHelicity::ProcessEvent no instructions on how to decode the helicity !!!!" << QwLog::endl;
      abort();
      break;
    }

  if(fHelicityBitPlus==fHelicityBitMinus)
    fHelicityReported=-1;
    
  // Predict helicity if delay is non zero.
  if(fUsePredictor && !fIgnoreHelicity){
    PredictHelicity();
  } else {
    // Else use the reported helicity values.
    fHelicityActual  = fHelicityReported;
    fHelicityDelayed = fHelicityReported;

    if(fPatternPhaseNumber== fMinPatternPhase){
      fPreviousPatternPolarity = fActualPatternPolarity;
      fActualPatternPolarity   = fHelicityReported;
      fDelayedPatternPolarity  = fHelicityReported;
    } 
   
  }

  if(ldebug){
    std::cout<<"\nevent number= "<<fEventNumber<<std::endl;
    std::cout<<"pattern number = "<<fPatternNumber<<std::endl;
    std::cout<<"pattern phase = "<<fPatternPhaseNumber<<std::endl;
    std::cout<<"max pattern phase = "<<fMaxPatternPhase<<std::endl;
    std::cout<<"min pattern phase = "<<fMinPatternPhase<<std::endl;


  }
  //std::cout << fPatternPhaseNumber << " " << fHelicityReported << " LOOK HERE !!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  return;

}


void QwHelicity::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> localbuffer;
  localbuffer.clear();

  // Userbit mode
  switch (fHelicityDecodingMode) {
  case kHelUserbitMode: {
    UInt_t userbit = 0x0;
    if (fPatternPhaseNumber == fMinPatternPhase) userbit |= 0x80000000;
    if (fHelicityDelayed == 1)    userbit |= 0x40000000;

    // Write the words to the buffer
    localbuffer.push_back(0x1); // cleandata
    localbuffer.push_back(0xa); // scandata1
    localbuffer.push_back(0xa); // scandata2
    localbuffer.push_back(0x0); // scalerheader
    localbuffer.push_back(0x20); // scalercounter (32)
    localbuffer.push_back(userbit); // userbit

    for (int i = 0; i < 64; i++) localbuffer.push_back(0x0); // (not used)
    break;
  }
  case kHelInputRegisterMode: {
    UInt_t input_register = 0x0;
    if (fHelicityDelayed == 1) input_register |= fInputReg_HelPlus;
    if (fHelicityDelayed == 0) input_register |= fInputReg_HelMinus;
    if (fPatternPhaseNumber == fMinPatternPhase) input_register |= fInputReg_PatternSync;

    // Write the words to the buffer
    localbuffer.push_back(input_register); // input_register
    localbuffer.push_back(0x0); // output_register
    localbuffer.push_back(fEventNumber); // mps_counter
    localbuffer.push_back(fPatternNumber); // pat_counter
    localbuffer.push_back(fPatternPhaseNumber - fMinPatternPhase + fPatternPhaseOffset); // pat_phase

    for (int i = 0; i < 17; i++) localbuffer.push_back(0x0); // (not used)
    break;
  }
  default:
    QwWarning << "QwHelicity::EncodeEventData: Unsupported helicity encoding!" << QwLog::endl;
    break;
  }

  // If there is element data, generate the subbank header
  std::vector<UInt_t> subbankheader;
  std::vector<UInt_t> rocheader;
  if (localbuffer.size() > 0) {

    // Form CODA subbank header
    subbankheader.clear();
    subbankheader.push_back(localbuffer.size() + 1);	// subbank size
    subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
		// subbank tag | subbank type | event number

    // Form CODA bank/roc header
    rocheader.clear();
    rocheader.push_back(subbankheader.size() + localbuffer.size() + 1);	// bank/roc size
    rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
		// bank tag == ROC | bank type | event number

    // Add bank header, subbank header and element data to output buffer
    buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
    buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
    buffer.insert(buffer.end(), localbuffer.begin(), localbuffer.end());
  }
}

void QwHelicity::Print() const
{
  QwOut << "===========================\n"
	<< "This event: Event#, Pattern#, PatternPhase#="
	<< fEventNumber << ", "
	<< fPatternNumber << ", "
	<< fPatternPhaseNumber << QwLog::endl;
  QwOut << "Previous event: Event#, Pattern#, PatternPhase#="
	<< fEventNumberOld << ", "
	<< fPatternNumberOld << ", "
	<< fPatternPhaseNumberOld << QwLog::endl;
  QwOut << "delta = \n(fEventNumberOld)-(fMaxPatternPhase)x(fPatternNumberOld)-(fPatternPhaseNumberOld)= "
	<< ((fEventNumberOld)-(fMaxPatternPhase)*(fPatternNumberOld)-(fPatternPhaseNumberOld)) << QwLog::endl;
  QwOut << "Helicity Reported, Delayed, Actual ="
	<< fHelicityReported << ","
	<< fHelicityDelayed << ","
	<< fHelicityActual << QwLog::endl;
  QwOut << "===" << QwLog::endl;
  return;
}


Int_t QwHelicity::LoadChannelMap(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  Int_t wordsofar=0;
  Int_t bankindex=-1;

  fPatternPhaseOffset=1;//Phase number offset is set to 1 by default and will be set to 0 if phase number starts from 0


  // Default value for random seed is 30 bits
  fRandBits = 30;


  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  mapstr.EnableGreediness();
  mapstr.SetCommentChars("!");

  UInt_t value = 0;
  TString valuestr;
  
  while (mapstr.ReadNextLine()){
    RegisterRocBankMarker(mapstr);

    if (mapstr.PopValue("patternphase",value)) {
      fMaxPatternPhase=value;
      //QwMessage << " fMaxPatternPhase " << fMaxPatternPhase << QwLog::endl;
    }
    if (mapstr.PopValue("patternbits",valuestr)) {
      SetHelicityBitPattern(valuestr);
    }
    if (mapstr.PopValue("inputregmask_fakemps",value)) {
      fInputReg_FakeMPS = value;
    }
    if (mapstr.PopValue("inputregmask_helicity",value)) {
      fInputReg_HelPlus  = value;
      fInputReg_HelMinus = 0;
    }
    if (mapstr.PopValue("inputregmask_helplus",value)) {
      fInputReg_HelPlus = value;
    }
    if (mapstr.PopValue("inputregmask_helminus",value)) {
      fInputReg_HelMinus = value;
    }
    if (mapstr.PopValue("inputregmask_pattsync",value)) {
      fInputReg_PatternSync = value;
    }
    if (mapstr.PopValue("inputregmask_pairsync",value)) {
      fInputReg_PairSync = value;
    }
    if (mapstr.PopValue("fakempsbit",value)) {
      fInputReg_FakeMPS = value;
      QwWarning << " fInputReg_FakeMPS 0x" << std::hex << fInputReg_FakeMPS << std::dec << QwLog::endl;
    }
    if (mapstr.PopValue("numberpatternsdelayed",value)) {
      SetHelicityDelay(value);
    }
    if (mapstr.PopValue("randseedbits",value)) {
      if (value==24 || value==30)
	fRandBits = value;
    }
    if (mapstr.PopValue("patternphaseoffset",value)) {
      fPatternPhaseOffset=value;
    }
    if (mapstr.PopValue("helpluseventtype",value)) {
      kEventTypeHelPlus = value;
    }
    if (mapstr.PopValue("helminuseventtype",value)) {
      kEventTypeHelMinus = value;
    }
    if (mapstr.PopValue("helicitydecodingmode",valuestr)) {
      if (valuestr=="InputRegisterMode") {
	QwMessage << " **** Input Register Mode **** " << QwLog::endl;
	fHelicityDecodingMode=kHelInputRegisterMode;
      } else if (valuestr=="UserbitMode"){
	QwMessage << " **** Userbit Mode **** " << QwLog::endl;
	fHelicityDecodingMode=kHelUserbitMode;
      } else if (valuestr=="HelLocalyMadeUp"){
	QwMessage << "**** Helicity Locally Made Up ****" << QwLog::endl;
	fHelicityDecodingMode=kHelLocalyMadeUp;
      } else if (valuestr=="InputMollerMode") {
	QwMessage << "**** Input Moller Mode ****" << QwLog::endl;
	fHelicityDecodingMode=kHelInputMollerMode;
      } else {
	QwError  << "The helicity decoding mode read in file " << mapfile
		 << " is not recognized in function QwHelicity::LoadChannelMap \n"
		 << " Quiting this execution." << QwLog::endl;
      }
    }

    if(bankindex!=GetSubbankIndex(fCurrentROC_ID,fCurrentBank_ID)) { 
      bankindex=GetSubbankIndex(fCurrentROC_ID,fCurrentBank_ID);
      if ((bankindex+1)>0){
	UInt_t numbanks = UInt_t(bankindex+1);
	if (fWordsPerSubbank.size()<numbanks){
	  fWordsPerSubbank.resize(numbanks,
				  std::pair<Int_t, Int_t>(fWord.size(),fWord.size()));
	}
      }
      wordsofar=0;
    }
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    //  Break this line into tokens to process it.
    TString modtype = mapstr.GetTypedNextToken<TString>();	// module type
    Int_t modnum = mapstr.GetTypedNextToken<Int_t>();	//slot number
    /* Int_t channum = */ mapstr.GetTypedNextToken<Int_t>();	//channel number /* unused */
    TString dettype = mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
    dettype.ToLower();
    TString namech  = mapstr.GetTypedNextToken<TString>();  //name of the detector
    namech.ToLower();
    TString keyword = mapstr.GetTypedNextToken<TString>();
    keyword.ToLower();
    // Notice that "namech" and "keyword" are now forced to lower-case.

    if(modtype=="SKIP"){
      if (modnum<=0) wordsofar+=1;
      else           wordsofar+=modnum;
    } else if(modtype!="WORD"|| dettype!="helicitydata") {
      QwError << "QwHelicity::LoadChannelMap:  Unknown detector type: "
	      << dettype  << ", the detector " << namech << " will not be decoded "
	      << QwLog::endl;
      continue;
    } else {
      QwWord localword;
      localword.fSubbankIndex=bankindex;
      localword.fWordInSubbank=wordsofar;
      wordsofar+=1;
      // I assume that one data = one word here. But it is not always the case, for
      // example the triumf adc gives 6 words per channel
      localword.fModuleType=modtype;
      localword.fWordName=namech;
      localword.fWordType=dettype;
      fWord.push_back(localword);
      fWordsPerSubbank.at(bankindex).second = fWord.size();
      
      // Notice that "namech" is in lower-case, so these checks
      // should all be in lower-case
      switch (fHelicityDecodingMode)
	{
	case kHelUserbitMode :
	  if(namech.Contains("userbit")) kUserbit=fWord.size()-1;
	  if(namech.Contains("scalercounter")) kScalerCounter=fWord.size()-1;
	  break;
	case kHelInputRegisterMode :
	  if(namech.Contains("input_register")) kInputRegister= fWord.size()-1;
	  if(namech.Contains("mps_counter")) kMpsCounter= fWord.size()-1;
	  if(namech.Contains("pat_counter")) kPatternCounter= fWord.size()-1;
	  if(namech.Contains("pat_phase")) kPatternPhase= fWord.size()-1;
	  break;
	case kHelInputMollerMode :
	  if(namech.Contains("mps_counter")) {
	    kMpsCounter= fWord.size()-1;
	  }
	  if(namech.Contains("pat_counter")) {
	    kPatternCounter = fWord.size()-1;
	  }
	  break;
	}
    }
  }
  

  if(ldebug) {
    std::cout << "Done with Load map channel \n";
    for(size_t i=0;i<fWord.size();i++)
      fWord[i].PrintID();
    std::cout << " kUserbit=" << kUserbit << "\n";  
  }
  ldebug=kFALSE;
  
  if (fHelicityDecodingMode==kHelInputMollerMode){
    // Check to be sure kEventTypeHelPlus and kEventTypeHelMinus are both defined and not equal
    if (kEventTypeHelPlus != kEventTypeHelMinus
	&& kEventTypeHelPlus>0 && kEventTypeHelPlus<15
	&& kEventTypeHelMinus>0 && kEventTypeHelMinus<15) {
      // Everything is okay
      QwDebug << "QwHelicity::LoadChannelMap:"
	      << "  We are in Moller Helicity Mode, with HelPlusEventType = "
	      << kEventTypeHelPlus
	      << "and HelMinusEventType = " << kEventTypeHelMinus
	      << QwLog::endl;
    } else {
      QwError << "QwHelicity::LoadChannelMap:"
	      << "  We are in Moller Helicity Mode, and the HelPlus and HelMinus event types are not set properly."
	      << "  HelPlusEventType = "  << kEventTypeHelPlus
	      << ", HelMinusEventType = " << kEventTypeHelMinus
	      << ".  Please correct the helicity map file!"
	      << QwLog::endl;
      exit(65);
    }
  }

std::cout << fHelicityBitPattern.size() << std::endl;
  
  mapstr.Close(); // Close the file (ifstream)
  return 0;
}


Int_t QwHelicity::LoadEventCuts(TString filename){
  return 0;
}

Int_t QwHelicity::ProcessEvBuffer(UInt_t event_type, const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG = kFALSE;

  if (((0x1 << (event_type - 1)) & this->GetEventTypeMask()) == 0)
    return 0;
  fEventType = event_type;

  Int_t index = GetSubbankIndex(roc_id,bank_id);
  if (index >= 0 && num_words > 0) {
    SetDataLoaded(kTRUE);
    //  We want to process this ROC.  Begin loopilooping through the data.
    QwDebug << "QwHelicity::ProcessEvBuffer:  "
	    << "Begin processing ROC" << roc_id
	    << " and subbank " << bank_id
	    << " number of words=" << num_words << QwLog::endl;

    for(Int_t i=fWordsPerSubbank.at(index).first; i<fWordsPerSubbank.at(index).second; i++) {
      if(fWord[i].fWordInSubbank+1<= (Int_t) num_words) {
	fWord[i].fValue=buffer[fWord[i].fWordInSubbank];
      } else {
	QwWarning << "QwHelicity::ProcessEvBuffer:  There is not enough word in the buffer to read data for "
		  << fWord[i].fWordName << QwLog::endl;
	QwWarning << "QwHelicity::ProcessEvBuffer:  Words in this buffer:" << num_words
		  << " trying to read word number =" << fWord[i].fWordInSubbank << QwLog::endl;
      }
    }
    if(lkDEBUG) {
      QwDebug << "QwHelicity::ProcessEvBuffer:  Done with Processing this event" << QwLog::endl;
      for(size_t i=0;i<fWord.size();i++) {
	std::cout << " word number = " << i << " ";
	fWord[i].Print();
      }
    }
  }
  lkDEBUG=kFALSE;
  return 0;
}


Int_t QwHelicity::GetHelicityReported()
{
  return fHelicityReported;
}

Int_t QwHelicity::GetHelicityActual()
{
  return fHelicityActual;
}

Int_t QwHelicity::GetHelicityDelayed()
{
  return fHelicityDelayed;
}

Long_t QwHelicity::GetPatternNumber()
{
  return  fPatternNumber;
}

Long_t QwHelicity::GetEventNumber()
{
  return fEventNumber;
}

Int_t QwHelicity::GetPhaseNumber()
{
  return fPatternPhaseNumber;
}

void QwHelicity::SetEventPatternPhase(Int_t event, Int_t pattern, Int_t phase)
{
  fEventNumber = event;
  fPatternNumber = pattern;
  fPatternPhaseNumber = phase;
}

void QwHelicity::SetFirstBits(UInt_t nbits, UInt_t seed)
{
  // This gives the predictor a quick start
  UShort_t firstbits[nbits];
  for (unsigned int i = 0; i < nbits; i++) firstbits[i] = (seed >> i) & 0x1;
  // Set delayed seed
  iseed_Delayed = GetRandomSeed(firstbits);
  // Progress actual seed by the helicity delay
  iseed_Actual = iseed_Delayed;
  for (int i = 0; i < fHelicityDelay; i++) GetRandbit(iseed_Actual);
}

void QwHelicity::SetHistoTreeSave(const TString &prefix)
{
  Ssiz_t len;
  if (TRegexp("diff_").Index(prefix,&len) == 0
   || TRegexp("asym[1-9]*_").Index(prefix,&len) == 0)
    fHistoType = kHelNoSave;
  else if (TRegexp("yield_").Index(prefix,&len) == 0)
    fHistoType = kHelSavePattern;
  else
    fHistoType = kHelSaveMPS;
}

void  QwHelicity::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  SetHistoTreeSave(prefix);
  if (folder != NULL) folder->cd();
  TString basename;
  size_t index=0;

  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSavePattern)
    {
      fHistograms.resize(1+fWord.size(), NULL);
      basename="pattern_polarity";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	basename="hel_"+fWord[i].fWordName;
	fHistograms[index]   = gQwHists.Construct1DHist(basename);
	index+=1;
      }
    }
  else if(fHistoType==kHelSaveMPS)
    {
      fHistograms.resize(4+fWord.size(), NULL);
      //eventnumber, patternnumber, helicity, patternphase + fWord.size
      basename=prefix+"delta_event_number";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      basename=prefix+"delta_pattern_number";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      basename=prefix+"pattern_phase";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      basename=prefix+"helicity";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	basename=prefix+fWord[i].fWordName;
	fHistograms[index]   = gQwHists.Construct1DHist(basename);
	index+=1;
      }
    }
  else
    QwError << "QwHelicity::ConstructHistograms this prefix--" << prefix << "-- is not unknown:: no histo created" << QwLog::endl;

  return;
}

void  QwHelicity::FillHistograms()
{
  //  Bool_t localdebug=kFALSE;

  size_t index=0;
  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSavePattern)
    {
      QwDebug << "QwHelicity::FillHistograms helicity info " << QwLog::endl;
      QwDebug << "QwHelicity::FillHistograms  pattern polarity=" << fActualPatternPolarity << QwLog::endl;
      if (fHistograms[index]!=NULL)
	fHistograms[index]->Fill(fActualPatternPolarity);
      index+=1;
      
      for (size_t i=0; i<fWord.size(); i++){
	if (fHistograms[index]!=NULL)
	  fHistograms[index]->Fill(fWord[i].fValue);
	index+=1;	
	QwDebug << "QwHelicity::FillHistograms " << fWord[i].fWordName << "=" << fWord[i].fValue << QwLog::endl;
      }
    }
  else if(fHistoType==kHelSaveMPS)
    {
      QwDebug << "QwHelicity::FillHistograms mps info " << QwLog::endl;
      if (fHistograms[index]!=NULL)
	fHistograms[index]->Fill(fEventNumber-fEventNumberOld);
      index+=1;
      if (fHistograms[index]!=NULL)
	fHistograms[index]->Fill(fPatternNumber-fPatternNumberOld);
      index+=1;
      if (fHistograms[index]!=NULL)
	fHistograms[index]->Fill(fPatternPhaseNumber);
      index+=1;
      if (fHistograms[index]!=NULL)
	fHistograms[index]->Fill(fHelicityActual);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	if (fHistograms[index]!=NULL)
	  fHistograms[index]->Fill(fWord[i].fValue);
	index+=1;
	QwDebug << "QwHelicity::FillHistograms " << fWord[i].fWordName << "=" << fWord[i].fValue << QwLog::endl;
      }
    }

  return;
}


void  QwHelicity::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  SetHistoTreeSave(prefix);


  fTreeArrayIndex  = values.size();
  TString basename;
  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSaveMPS)
    {
      // basename = "actual_helicity";    //predicted actual helicity before being delayed.
      // values.push_back(0.0);
      // tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "delayed_helicity";   //predicted delayed helicity
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "reported_helicity";  //delayed helicity reported by the input register.
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "pattern_phase";
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "pattern_number";
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "pattern_seed";
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "event_number";
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      for (size_t i=0; i<fWord.size(); i++)
	{
	  basename = fWord[i].fWordName;
	  values.push_back(0.0);
	  tree->Branch(basename, &(values.back()), basename+"/D");
	}
    }
  else if(fHistoType==kHelSavePattern)
    {
      basename = "actual_helicity";    //predicted actual helicity before being delayed.
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "actual_pattern_polarity";
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "actual_previous_pattern_polarity";
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "delayed_pattern_polarity";
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "pattern_number";
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      basename = "pattern_seed";
      values.push_back(0.0);
      tree->Branch(basename, &(values.back()), basename+"/D");
      //
      for (size_t i=0; i<fWord.size(); i++)
	{
	  basename = fWord[i].fWordName;
	  values.push_back(0.0);
	  tree->Branch(basename, &(values.back()), basename+"/D");
	}
    }

  return;
}

void  QwHelicity::ConstructBranch(TTree *tree, TString &prefix)
{
  TString basename;

  SetHistoTreeSave(prefix);
  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSaveMPS)
    {
      // basename = "actual_helicity";    //predicted actual helicity before being delayed.
      // tree->Branch(basename, &fHelicityActual, basename+"/I");
      //
      basename = "delayed_helicity";   //predicted delayed helicity
      tree->Branch(basename, &fHelicityDelayed, basename+"/I");
      //
      basename = "reported_helicity";  //delayed helicity reported by the input register.
      tree->Branch(basename, &fHelicityReported, basename+"/I");
      //
      basename = "pattern_phase";
      tree->Branch(basename, &fPatternPhaseNumber, basename+"/I");
      //
      basename = "pattern_number";
      tree->Branch(basename, &fPatternNumber, basename+"/I");
      //
      basename = "pattern_seed";
      tree->Branch(basename, &fPatternSeed, basename+"/I");
      //
      basename = "event_number";
      tree->Branch(basename, &fEventNumber, basename+"/I");
    }
  else if(fHistoType==kHelSavePattern)
    {
      basename = "actual_helicity";    //predicted actual helicity before being delayed.
      tree->Branch(basename, &fHelicityActual, basename+"/I");
      //
      basename = "actual_pattern_polarity";
      tree->Branch(basename, &fActualPatternPolarity, basename+"/I");
      //
      basename = "actual_previous_pattern_polarity";
      tree->Branch(basename, &fPreviousPatternPolarity, basename+"/I");
      //
      basename = "delayed_pattern_polarity";
      tree->Branch(basename, &fDelayedPatternPolarity, basename+"/I");
      //
      basename = "pattern_number";
      tree->Branch(basename, &fPatternNumber, basename+"/I");
      //
      basename = "pattern_seed";
      tree->Branch(basename, &fPatternSeed, basename+"/I");

      for (size_t i=0; i<fWord.size(); i++)
	{
	  basename = fWord[i].fWordName;
	  tree->Branch(basename, &fWord[i].fValue, basename+"/I");
	}
    }

  return;
}

void  QwHelicity::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file)
{
  TString basename;

  SetHistoTreeSave(prefix);
  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSaveMPS)
    {
      // basename = "actual_helicity";    //predicted actual helicity before being delayed.
      // tree->Branch(basename, &fHelicityActual, basename+"/I");
      //
      basename = "delayed_helicity";   //predicted delayed helicity
      tree->Branch(basename, &fHelicityDelayed, basename+"/I");
      //
      basename = "reported_helicity";  //delayed helicity reported by the input register.
      tree->Branch(basename, &fHelicityReported, basename+"/I");
      //
      basename = "pattern_phase";
      tree->Branch(basename, &fPatternPhaseNumber, basename+"/I");
      //
      basename = "pattern_number";
      tree->Branch(basename, &fPatternNumber, basename+"/I");
      //
      basename = "pattern_seed";
      tree->Branch(basename, &fPatternSeed, basename+"/I");
      //
      basename = "event_number";
      tree->Branch(basename, &fEventNumber, basename+"/I");
    }
  else if(fHistoType==kHelSavePattern)
    {
      basename = "actual_helicity";    //predicted actual helicity before being delayed.
      tree->Branch(basename, &fHelicityActual, basename+"/I");
      //
      basename = "actual_pattern_polarity";
      tree->Branch(basename, &fActualPatternPolarity, basename+"/I");
      //
      basename = "actual_previous_pattern_polarity";
      tree->Branch(basename, &fPreviousPatternPolarity, basename+"/I");
      //
      basename = "delayed_pattern_polarity";
      tree->Branch(basename, &fDelayedPatternPolarity, basename+"/I");
      //
      basename = "pattern_number";
      tree->Branch(basename, &fPatternNumber, basename+"/I");
      //
      basename = "pattern_seed";
      tree->Branch(basename, &fPatternSeed, basename+"/I");

      for (size_t i=0; i<fWord.size(); i++)
	{
	  basename = fWord[i].fWordName;
	  tree->Branch(basename,&fWord[i].fValue, basename+"/I");
	}

    }


  return;
}

void  QwHelicity::FillTreeVector(std::vector<Double_t> &values) const
{

  size_t index=fTreeArrayIndex;
  if(fHistoType==kHelSaveMPS)
    {
      // values[index++] = fHelicityActual;
      values[index++] = fHelicityDelayed;
      values[index++] = fHelicityReported;
      values[index++] = fPatternPhaseNumber;
      values[index++] = fPatternNumber;
      values[index++] = fPatternSeed;
      values[index++] = fEventNumber;
      for (size_t i=0; i<fWord.size(); i++)
	values[index++] = fWord[i].fValue;
    }
  else if(fHistoType==kHelSavePattern)
    {
      values[index++] = fHelicityActual;
      values[index++] = fActualPatternPolarity;
      values[index++] = fPreviousPatternPolarity;
      values[index++] = fDelayedPatternPolarity;
      values[index++] = fPatternNumber;
      values[index++] = fPatternSeed;
      for (size_t i=0; i<fWord.size(); i++){
	values[index++] = fWord[i].fValue;
      }
    }

  return;
}

#ifdef __USE_DATABASE__
void  QwHelicity::FillDB(QwParityDB *db, TString type)
{
  if (type=="yield" || type=="asymmetry")
    return;

  db->Connect();
  mysqlpp::Query query = db->Query();

  db->Disconnect();
}



void  QwHelicity::FillErrDB(QwParityDB *db, TString type)
{
  return;
}
#endif // __USE_DATABASE__


UInt_t QwHelicity::GetRandbit(UInt_t& ranseed){
  Bool_t status = false;

  if (fRandBits == 24)
    status = GetRandbit24(ranseed);
  if (fRandBits == 30)
    status = GetRandbit30(ranseed);

  return status;
}

UInt_t QwHelicity::GetRandbit24(UInt_t& ranseed)
{
  /** This is a 24 bit random bit generator according to the "new" algorithm
     described in "G0 Helicity Digital Controls" by E. Stangland, R. Flood, H. Dong.


     The helicity board uses a maximum-length linear feedback shift registers
     for the generation of a pseudo-random sequence of bits.  The length of the
     register (24 bits or 30 bits) defines the length before a sequence is
     repeated: 2^n - 1.

     For a mathematical introduction to the generation of pseudo-random numbers
     with maximum-length linear feedback shift registers (LFSR), see the
     following web references:
       http://en.wikipedia.org/wiki/Linear_feedback_shift_register
       http://www.newwaveinstruments.com/resources/articles/m_sequence_linear_feedback_shift_register_lfsr.htm

     In particular, the used solutions are to place XNOR taps at the bits
      24 stages, 4 taps:  (47 sets)
       [24, 23, 21, 20]
      30 stages, 4 taps:  (104 sets)
       [30, 29, 28, 7]

     The 24 stage solution we use has been mirrored by transforming [m, A, B, C]
     into [m, m-C, m-B, m-A].  This goes through the sequence in the opposite
     direction.
   */

  const UInt_t IB1 = 1;                     //Bit 1 of shift register 000000000000000000000001
  const UInt_t IB3 = 4;                     //Bit 3 of shift register 000000000000000000000100
  const UInt_t IB4 = 8;                     //Bit 4 of shift register 000000000000000000001000
  const UInt_t IB24 = 8388608;               //Bit 24 of shift register 100000000000000000000000
  const UInt_t MASK = IB1+IB3+IB4+IB24;     //Sum of the four feedback bits is 100000000000000000001101

  UInt_t result; //The generated pattern

  if(ranseed<=0)
    {
      QwError << "ranseed must be greater than zero!" << QwLog::endl;
      result = 0;
    }

  if(ranseed & IB24) // if bit 24 of ranseed = 1, then output 1
    {
      ranseed = (((ranseed^MASK) << 1)|IB1);
      result = 1;
    }
  else
    {
      ranseed <<= 1;
      result = 0;
    }
  return(result);

}


UInt_t QwHelicity::GetRandbit30(UInt_t& ranseed)
{
  /* For an explanation of the algorithm, see above in GetRandbit24() */

  UInt_t bit7    = (ranseed & 0x00000040) != 0;
  UInt_t bit28   = (ranseed & 0x08000000) != 0;
  UInt_t bit29   = (ranseed & 0x10000000) != 0;
  UInt_t bit30   = (ranseed & 0x20000000) != 0;

  UInt_t result = (bit30 ^ bit29 ^ bit28 ^ bit7) & 0x1;

  if(ranseed<=0) {
    QwError << "ranseed must be greater than zero!" << QwLog::endl;
    result = 0;
  }
  ranseed =  ( (ranseed << 1) | result ) & 0x3FFFFFFF;

  return(result);
}


UInt_t QwHelicity::GetRandomSeed(UShort_t* first24randbits)
{
  Bool_t ldebug=0;
  QwDebug << " Entering QwHelicity::GetRandomSeed \n";

  /**  This the random seed generator used in G0 (L.Jianglai)
      Here we get the 24 random bits and derive the randome seed from that.
      randome seed                      : b24 b23 b22.....b2 b1
      first 24 random bit from this seed: h1 h2 h3 ....h23 h24
      we have,
      b23 = h1, b22 = h2,... b5 = h20,
      h21^b24 = b4 , h3^b24^b23 = b3 ,h23^b23^b22 = b2, h24^b22^b24 = b1.
      Thus by using h1,...h24, we can derive the b24,..b1 of the randseed.
  */

  UShort_t b[25];
  UInt_t ranseed = 0;

  if(ldebug)
    {
     for(size_t i=0;i<25;i++)
       std::cout << i << " : " << first24randbits[i] << "\n";
    }

  for(size_t i=24;i>=5;i--)   b[i]= first24randbits[24-i+1]; //fill h24..h5

  // fill b[4] to b[1]
  b[4] = first24randbits[21]^b[24]; //h21^b24 = b4
  b[3] = first24randbits[22]^b[24]^b[23]; //h22^b24^b23 = b3
  b[2] = first24randbits[23]^b[23]^b[22];// h23^b23^b22 = b2
  b[1] = first24randbits[24]^b[21]^b[22]^b[24];// h24^b22^b24 = b1

  ///assign the values in the h aray and into the sead
  for(size_t i=24;i>=1;i--)  ranseed = (ranseed << 1) | (b[i]&1);

  ranseed = ranseed&0xFFFFFF; //put a mask

  QwDebug << " seed =" << ranseed <<QwLog::endl;
  QwDebug << " Exiting QwHelicity::GetRandomSeed \n";


  return ranseed;

}


void QwHelicity::RunPredictor()
{
  Int_t ldebug = kFALSE;

  if(ldebug)  std::cout  << "Entering QwHelicity::RunPredictor for fEventNumber, " << fEventNumber
			 << ", fPatternNumber, " << fPatternNumber
			 <<  ", and fPatternPhaseNumber, " << fPatternPhaseNumber << std::endl;

    /**Update the random seed if the new event is from a different pattern.
       Check the difference between old pattern number and the new one and
       to see how many patterns we have missed or skipped. Then loop back
       to get the correct pattern polarities.
    */


    for (int i = 0; i < fPatternNumber - fPatternNumberOld; i++) //got a new pattern
      {
	fPreviousPatternPolarity = fActualPatternPolarity;
	fActualPatternPolarity   = GetRandbit(iseed_Actual);
	fDelayedPatternPolarity  = GetRandbit(iseed_Delayed);
	QwDebug << "Predicting : seed actual, delayed: " <<  iseed_Actual
			    << ":" << iseed_Delayed <<QwLog::endl;
      }

  /**Predict the helicity according to pattern
     Defined patterns:
     Pair:    +-       or -+
     Quartet: +--+     or -++-
     Octet:   +--+-++- or -++-+--+
     Symmetric octet:  +-+--+-+ or -+-++-+-
     Octo-quad: +--++--++--++--+-++--++--++--++-
  */

  Int_t localphase = fPatternPhaseNumber-fMinPatternPhase;//Paul's modifications

  Int_t localbit,indexnum,shiftnum;
  indexnum = TMath::FloorNint(localphase/32.);
  shiftnum = localphase - indexnum*32;
  //std::cout << localphase << " " << indexnum << " " << shiftnum << " "<< fHelicityBitPattern.size() << std::endl;
  localbit = ((fHelicityBitPattern.at(indexnum))>>shiftnum)&0x1;
  //std::cout<< localphase << " " << indexnum << " " << shiftnum <<std::hex << fHelicityBitPattern.at(indexnum) << std::dec << " " << localbit << std::endl;
  // Use the stored helicity bit pattern to calculate the helicity of this window
  if (localbit == (fHelicityBitPattern[0] & 0x1)) {
    fHelicityActual  = fActualPatternPolarity;
    fHelicityDelayed = fDelayedPatternPolarity;
  } else {
    fHelicityActual  = fActualPatternPolarity ^ 0x1;
    fHelicityDelayed = fDelayedPatternPolarity ^ 0x1;
  }
  // Past highest pattern phase
  if (localphase > fMaxPatternPhase)
    ResetPredictor();

  if(ldebug){
    std::cout << "Predicted Polarity ::: Delayed ="
	      << fDelayedPatternPolarity << " Actual ="
	      << fActualPatternPolarity << "\n";
    std::cout << "Predicted Helicity ::: Delayed Helicity=" << fHelicityDelayed
	      << " Actual Helicity=" << fHelicityActual << " Reported Helicity=" << fHelicityReported << "\n";
    QwError << "Exiting QwHelicity::RunPredictor " << QwLog::endl;

  }

  return;
}


Bool_t QwHelicity::CollectRandBits()
{
  Bool_t status = false;

  if (fRandBits == 24)
    status = CollectRandBits24();
  if (fRandBits == 30)
    status = CollectRandBits30();

  return status;
}



Bool_t QwHelicity::CollectRandBits24()
{
    //routine to collect 24 random bits before getting the randseed for prediction
    Bool_t  ldebug = kFALSE;
    const UInt_t ranbit_goal = 24;

  QwDebug << "QwHelicity::Entering CollectRandBits24...." << QwLog::endl;


  if (n_ranbits==ranbit_goal)    return kTRUE;

  if(n_ranbits<ranbit_goal&&fPatternPhaseNumber==fMinPatternPhase)
    {
      QwMessage << "Collecting information from event #" << fEventNumber << " to generate helicity seed"
                << "(need 24 bit, so far got " << n_ranbits << " bits )" << QwLog::endl;
    }


  static UShort_t first24bits[25]; //array to store the first 24 bits

  fGoodHelicity = kFALSE; //reset before prediction begins
  if(IsContinuous())
    {
      if((fPatternPhaseNumber==fMinPatternPhase)&& (fPatternNumber>=0))
	{
	  first24bits[n_ranbits+1] = fHelicityReported;
	  n_ranbits ++;
	  if(ldebug)
	    {
	      std::cout << " event number" << fEventNumber << ", fPatternNumber"
		        << fPatternNumber << ", n_ranbit" << n_ranbits
		        << ", fHelicityReported" << fHelicityReported << "\n";
	    }

	  if(n_ranbits == ranbit_goal ) //If its the 24th consecative random bit,
	    {
	       if(ldebug)
		 {
		   std::cout << "Collected 24 random bits. Get the random seed for the predictor." << "\n";
		   for(UInt_t i=0;i<ranbit_goal;i++) std::cout << " i:bit =" << i << ":" << first24bits[i] << "\n";
		 }
	      iseed_Delayed = GetRandomSeed(first24bits);
	      //This random seed will predict the helicity of the event (24+fHelicityDelay) patterns  before;
	      // run GetRandBit 24 times to get the delayed helicity for this event
	       QwDebug << "The reported seed 24 patterns ago = " << iseed_Delayed << "\n";

	      for(UInt_t i=0;i<ranbit_goal;i++) fDelayedPatternPolarity =GetRandbit(iseed_Delayed);
	      fHelicityDelayed = fDelayedPatternPolarity;
	      //The helicity of the first phase in a pattern is
	      //equal to the polarity of the pattern

	      //Check whether the reported helicity is the same as the helicity predicted by the random seed

	      if(fHelicityDelay >=0){
		iseed_Actual = iseed_Delayed;
		for(Int_t i=0; i<fHelicityDelay; i++)
		  {
		    QwDebug << "Delaying helicity " << QwLog::endl;
		    fPreviousPatternPolarity = fActualPatternPolarity;
		    fActualPatternPolarity = GetRandbit(iseed_Actual);
		  }
	      }
	      else
		{
		  QwError << "QwHelicity::CollectRandBits  We cannot handle negative delay(prediction) in the reported helicity. Exiting." << QwLog::endl;
		  ResetPredictor();
		}

	      fHelicityActual =  fActualPatternPolarity;
	    }
	}
    }
  else // while collecting the seed, we encounter non continuous events.
    {
      ResetPredictor();
      QwError << "QwHelicity::CollectRandBits, while collecting the seed, we encountered non continuous events: need to reset the seed collecting " << QwLog::endl
	      << " event number=" << fEventNumber << ", fPatternNumber="
	      << fPatternNumber << ",  fPatternPhaseNumber=" << fPatternPhaseNumber << QwLog::endl;
    }

      //else n randbits have been set to zero in the error checking routine
      //start over from the next pattern
  QwDebug << "QwHelicity::CollectRandBits24 => Done collecting ..." << QwLog::endl;

  return kFALSE;

}


Bool_t QwHelicity::CollectRandBits30()
{
  /** Starting to collect 30 bits/helicity state to get the
      random seed for the 30 bit helicity predictor.
      These bits (1/0) are the reported helicity states of the first event
      of each new pattern ot the so called pattern polarity.*/

  //  Bool_t  ldebug = kFALSE;
  const UInt_t ranbit_goal = 30;

  /** If we have finished collecting the bits then ignore the rest of this funciton and return true.
      No need to recollect!*/
  if (n_ranbits == ranbit_goal)    return kTRUE;

  /** If we are still collecting the bits, make sure we collect them from only the
      events with the minimum pattern phase.*/

  if (n_ranbits < ranbit_goal && fPatternPhaseNumber == fMinPatternPhase) {
    QwMessage << "Collecting information (";
    if (fHelicityReported == 1) QwMessage << "+";
    else                        QwMessage << "-";
    QwMessage << ") from event #" << fEventNumber << " to generate helicity seed ";
    QwMessage << "(need " << ranbit_goal << " bit, so far got " << n_ranbits << " bits )" << QwLog::endl;
  }

  /** If the events are continuous, start to make the ranseed for the helicity
      pattern we are getting which is the delayed helicity.*/

  fGoodHelicity = kFALSE; //reset before prediction begins

  if(IsContinuous()) {
    /**  Make sure we are at the beging of a valid pattern. */
    if((fPatternPhaseNumber==fMinPatternPhase)&& (fPatternNumber>=0)) {
      iseed_Delayed = ((iseed_Delayed << 1)&0x3FFFFFFF)|fHelicityReported;
      QwDebug << "QwHelicity:: CollectRandBits30:  Collecting randbit " << n_ranbits << ".." << QwLog::endl;
      n_ranbits++;

      /** If we got the 30th bit,*/
      if(n_ranbits == ranbit_goal){
	QwDebug << "QwHelicity:: CollectRandBits30:  done Collecting 30 randbits" << QwLog::endl;

	/** set the polarity of the current pattern to be equal to the reported helicity,*/
	fDelayedPatternPolarity = fHelicityReported;
	QwDebug << "QwHelicity:: CollectRandBits30:  delayedpatternpolarity =" << fDelayedPatternPolarity << QwLog::endl;

	/** then use it as the delayed helicity, */
	fHelicityDelayed = fDelayedPatternPolarity;

	/**if the helicity is delayed by a positive number of patterns then loop the delayed ranseed backward to get the ranseed
	   for the actual helicity */
	if(fHelicityDelay >=0){
	  iseed_Actual = iseed_Delayed;
	  for(Int_t i=0; i<fHelicityDelay; i++) {
	    /**, get the pattern polarity for the actual pattern using that actual ranseed.*/
	    fPreviousPatternPolarity = fActualPatternPolarity;
	    fActualPatternPolarity = GetRandbit(iseed_Actual);
	  }
	} else {
	  /** If we have a negative delay. Reset the predictor.*/
	  QwError << "QwHelicity::CollectRandBits30:  We cannot handle negative delay(prediction) in the reported helicity. Exiting." << QwLog::endl;
	  ResetPredictor();
	}
	/** If all is well so far, set the actual pattern polarity as the actual helicity.*/
	fHelicityActual =  fActualPatternPolarity;
      }
    }
  } else {
    /** while collecting the seed, we encounter non continuous events.Discard bit. Reset the predition*/
    ResetPredictor();
    QwWarning << "QwHelicity::CollectRandBits30:  While collecting the seed, we encountered non continuous events: Need to reset the seed collecting " << QwLog::endl;
    QwDebug   << " event number=" << fEventNumber << ", fPatternNumber="<< fPatternNumber << ",  fPatternPhaseNumber=" << fPatternPhaseNumber << QwLog::endl;
  }
  return kFALSE;
}


void QwHelicity::PredictHelicity()
{
   Bool_t ldebug=kFALSE;

   if(ldebug)  std::cout << "Entering QwHelicity::PredictHelicity \n";

   /**Routine to predict the true helicity from the delayed helicity.
      Helicities are usually delayed by 8 events or 2 quartets. This delay
      can now be set as a cmd line option.
   */

   if(CollectRandBits()) {
     /**After accumulating 24/30 helicity bits, iseed is up-to-date.
	If nothing goes wrong, n-ranbits will stay as 24/30
	Reset it to zero if something goes wrong.
     */

     if(ldebug)  std::cout << "QwHelicity::PredictHelicity=>Predicting the  helicity \n";
     RunPredictor();

     /** If not good helicity, start over again by resetting the predictor. */
     if(!IsGoodHelicity())
       ResetPredictor();
   }

   if(ldebug)  std::cout << "n_ranbit exiting the function = " << n_ranbits << "\n";

   return;
}



void QwHelicity::SetHelicityDelay(Int_t delay)
{
  /**Sets the number of bits the helicity reported gets delayed with.
     We predict helicity only if there is a non-zero pattern delay given. */

  if(delay>=0){
    fHelicityDelay = delay;
    if(delay == 0){
      QwWarning << "QwHelicity : SetHelicityDelay ::  helicity delay is set to 0."
		<< " Disabling helicity predictor and using reported helicity information." 
		<< QwLog::endl;
      fUsePredictor = kFALSE;
    }
    else
      fUsePredictor = kTRUE; 
  }
  else
    QwError << "QwHelicity::SetHelicityDelay We cannot handle negative delay in the prediction of delayed helicity. Exiting.." << QwLog::endl;

  return;
}


void QwHelicity::SetHelicityBitPattern(TString hex)
{
  fHelicityBitPattern.clear();
  fHelicityBitPattern = kDefaultHelicityBitPattern;
    /*  // Set the helicity pattern bits
  if (parity(bits) == 0)
    fHelicityBitPattern = bits;
  else QwError << "What, exactly, are you trying to do ?!?!?" << QwLog::endl;
    */
  // Notify the user
  QwMessage << " fPatternBits 0x" ;
  for (int i = fHelicityBitPattern.size()-1;i>=0; i--){
    QwMessage << std::hex << fHelicityBitPattern[i] << " ";
  }
  QwMessage << std::dec << QwLog::endl;
}

void QwHelicity::ResetPredictor()
{
  /**Start a new helicity prediction sequence.*/

  QwWarning << "QwHelicity::ResetPredictor:  Resetting helicity prediction!" << QwLog::endl;
  n_ranbits = 0;
  fGoodHelicity = kFALSE;
  fGoodPattern = kFALSE;
  return;
}



VQwSubsystem&  QwHelicity::operator=  (VQwSubsystem *value)
{
  Bool_t ldebug = kFALSE;
  if(Compare(value))
    {

       //QwHelicity* input= (QwHelicity*)value;
      VQwSubsystem::operator=(value);
      QwHelicity* input= dynamic_cast<QwHelicity*>(value);

      for(size_t i=0;i<input->fWord.size();i++)
	this->fWord[i].fValue=input->fWord[i].fValue;
      this->fHelicityActual = input->fHelicityActual;
      this->fPatternNumber  = input->fPatternNumber;
      this->fPatternSeed    = input->fPatternSeed;
      this->fPatternPhaseNumber=input->fPatternPhaseNumber;
      this->fEventNumber=input->fEventNumber;
      this->fActualPatternPolarity=input->fActualPatternPolarity;
      this->fDelayedPatternPolarity=input->fDelayedPatternPolarity;
      this->fPreviousPatternPolarity=input->fPreviousPatternPolarity;
      this->fHelicityReported=input->fHelicityReported;
      this->fHelicityActual=input->fHelicityActual;
      this->fHelicityDelayed=input->fHelicityDelayed;
      this->fHelicityBitPlus=input->fHelicityBitPlus;
      this->fHelicityBitMinus=input->fHelicityBitMinus;
      this->fGoodHelicity=input->fGoodHelicity;
      this->fGoodPattern=input->fGoodPattern;
      this->fIgnoreHelicity = input->fIgnoreHelicity;

      this->fErrorFlag = input->fErrorFlag;
      this->fEventNumberFirst     = input->fEventNumberFirst;
      this->fPatternNumberFirst   = input->fPatternNumberFirst;
      this->fNumMissedGates       = input->fNumMissedGates;
      this->fNumMissedEventBlocks = input->fNumMissedEventBlocks;
      this->fNumMultSyncErrors    = input->fNumMultSyncErrors;
      this->fNumHelicityErrors    = input->fNumHelicityErrors;

      if(ldebug){
	std::cout << "QwHelicity::operator = this->fPatternNumber=" << this->fPatternNumber << std::endl;
	std::cout << "input->fPatternNumber=" << input->fPatternNumber << "\n";
      }
    }

  return *this;
}

VQwSubsystem&  QwHelicity::operator+=  (VQwSubsystem *value)
{
  //  Bool_t localdebug=kFALSE;
  QwDebug << "Entering QwHelicity::operator+= adding " << value->GetName() << " to " << this->GetName() << " " << QwLog::endl;

  //this routine is most likely to be called during the computatin of assymetry
  //this call doesn't make too much sense for this class so the following lines
  //are only use to put safe gards testing for example if the two instantiation indeed
  // refers to elements in the same pattern.
  CheckPatternNum(value);
  MergeCounters(value);
  return *this;
}

void QwHelicity::CheckPatternNum(VQwSubsystem *value)
{
  //  Bool_t localdebug=kFALSE;
  if(Compare(value)) {
    QwHelicity* input= dynamic_cast<QwHelicity*>(value);
    QwDebug << "QwHelicity::MergeCounters: this->fPatternNumber=" << this->fPatternNumber 
	    << ", input->fPatternNumber=" << input->fPatternNumber << QwLog::endl;

    this->fErrorFlag |= input->fErrorFlag;
    
    //  Make sure the pattern number and poalrity agree!
    if(this->fPatternNumber!=input->fPatternNumber)
      this->fPatternNumber=-999999;
    if(this->fActualPatternPolarity!=input->fActualPatternPolarity)
      this->fPatternNumber=-999999;
    if (this->fPatternNumber==-999999){
      this->fErrorFlag |= kErrorFlag_Helicity + kGlobalCut + kEventCutMode3;
    }
  }
}

void QwHelicity::MergeCounters(VQwSubsystem *value)
{
  //  Bool_t localdebug=kFALSE;
  if(Compare(value)) {
    QwHelicity* input= dynamic_cast<QwHelicity*>(value);

    fEventNumber = (fEventNumber == 0) ? input->fEventNumber :
      std::min(fEventNumber, input->fEventNumber);
    for (size_t i=0; i<fWord.size(); i++) {
      fWord[i].fValue =  (fWord[i].fValue == 0) ? input->fWord[i].fValue :
	std::min( fWord[i].fValue, input->fWord[i].fValue);
    }
  }
}

void QwHelicity::Ratio(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  // this is stub function defined here out of completion and uniformity between each subsystem
  *this =  value1;
  CheckPatternNum(value2);
  MergeCounters(value2);
}

void  QwHelicity::AccumulateRunningSum(VQwSubsystem* value, Int_t count, Int_t ErrorMask){
  if (Compare(value)) {
    MergeCounters(value);
    QwHelicity* input = dynamic_cast<QwHelicity*>(value);
    fPatternNumber = (fPatternNumber <=0 ) ? input->fPatternNumber :
      std::min(fPatternNumber, input->fPatternNumber);
    //  Keep track of the various error quantities, so we can print 
    //  them at the end.
    fNumMissedGates       = input->fNumMissedGates;
    fNumMissedEventBlocks = input->fNumMissedEventBlocks;
    fNumMultSyncErrors    = input->fNumMultSyncErrors;
    fNumHelicityErrors    = input->fNumHelicityErrors;
  }
}

Bool_t QwHelicity::Compare(VQwSubsystem *value)
{
  Bool_t res=kTRUE;
  if(typeid(*value)!=typeid(*this)) {
    res=kFALSE;
  } else {
    QwHelicity* input= dynamic_cast<QwHelicity*>(value);
    if(input->fWord.size()!=fWord.size()) {
      res=kFALSE;
    }
  }
  return res;
}

UInt_t QwHelicity::BuildHelicityBitPattern(Int_t patternsize){
  UInt_t bitpattern = 0;
  //  Standard helicity board patterns (last to first):
  //  Pair, quad, octet: -++-+--+ : 0x69
  //  Hexo-quad:         -++--++--++-+--++--++--+ : 0x666999
  //  Octo-quad:         -++--++--++--++-+--++--++--++--+ : 0x66669999
  //
  //std::cout << fHelicityBitPattern.size() << std::endl;
  fHelicityBitPattern.clear();
  if (patternsize<8){
    fHelicityBitPattern = kDefaultHelicityBitPattern;
  } else if (patternsize%2==0 && patternsize>0 && patternsize < 129){
    int num_int = TMath::CeilNint(patternsize/32.);
    //std::cout << num_int << " " << patternsize << " " << TMath::Ceil(patternsize/32.) << std::endl;
    fHelicityBitPattern.resize(num_int);
    bitpattern = 0x69969669;
    fHelicityBitPattern[0] = bitpattern;
    for (int i = 1; i<num_int; i++){
      fHelicityBitPattern[i] = ~fHelicityBitPattern[i-1];
    }
    /*if (patternsize%8==0){
    Int_t halfshift = patternsize/2;
    for (Int_t i=0; i<(patternsize/8); i++){
      bitpattern += (0x9<<(i*4));
      bitpattern += (0x6<<(halfshift+i*4));
    }
    */
  } else {
    QwError << "QwHelicity::BuildHelicityBitPattern: "
	    << "Unable to build standard bit pattern for pattern size of "
	    << patternsize << ".  Try a pattern of 0x69."
	    << QwLog::endl;
    fHelicityBitPattern = kDefaultHelicityBitPattern;
  }
  //std::cout << fHelicityBitPattern.size() << std::endl;
  /*QwMessage << "QwHelicity::BuildHelicityBitPattern: "
	  << "Built pattern 0x" << std::hex << bitpattern
	  << std::dec << " for pattern size "
	  << patternsize << "." << QwLog::endl;
	*/
  QwMessage << "QwHelicity::BuildHelicityBitPattern: "
          << "Built pattern 0x";
  for (int i = fHelicityBitPattern.size()-1;i>=0; i--){
    QwMessage << std::hex << fHelicityBitPattern[i] << " ";
  }
  QwMessage << std::dec << "for pattern size "
          << patternsize << "." << QwLog::endl;
  //  Now set the bit pattern.
  //  SetHelicityBitPattern(bitpattern);
  return fHelicityBitPattern[0];
}

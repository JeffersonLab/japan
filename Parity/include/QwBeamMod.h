/**********************************************************\
* File: QwBeamMod.h                                       *
*                                                         *
* Author: Joshua Hoskins                                  *
* Time-stamp: 052510                                      *
***********************************************************
*                                                         *
* Time-Stamp: 101910                                      *
*                                                         *
* Added support of QwWord                                 *
*                                                         *
\**********************************************************/

#ifndef __QwBEAMMOD__
#define __QwBEAMMOD__

// System headers
#include <vector>

// ROOT headers
#include "TTree.h"

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwVQWK_Channel.h"
#include "QwWord.h"


//enum EBeamInstrumentType{kBPMStripline = 0,
//			 kBCM,
//			 kCombinedBCM,
//			 kCombinedBPM};

// this emun vector needs to be coherent with the DetectorTypes declaration in the QwBeamLine constructor

class QwModChannelID;

/*****************************************************************
*  Class:
******************************************************************/
class QwBeamMod: public VQwSubsystemParity, public MQwSubsystemCloneable<QwBeamMod> {

 private:
  ///
  QwBeamMod();

 public:
  /// Constructor with name
  QwBeamMod(const TString& name)
  : VQwSubsystem(name),VQwSubsystemParity(name)
    {
      // these declaration need to be coherent with the enum vector EBeamInstrumentType
      fgModTypeNames.push_back(""); // Need to define these wrt to our detector types.
      fgModTypeNames.push_back("");
      fgModTypeNames.push_back("");
      fgModTypeNames.push_back("");
      for(size_t i=0;i<fgModTypeNames.size();i++)
        fgModTypeNames[i].ToLower();
      fFFB_holdoff_Counter=0;
      fFFB_Flag=kTRUE;
      fRampChannelIndex = -1;
      fPatternWordIndex = -1;
    };
  /// Copy constructor
  QwBeamMod(const QwBeamMod& source)
  : VQwSubsystem(source),VQwSubsystemParity(source),
    fModChannel(source.fModChannel),fWord(source.fWord),
    fMonitorNames(source.fMonitorNames),fMonitors(source.fMonitors),
    fBPMPosition(source.fBPMPosition)
  { }
  /// Virtual destructor
  virtual ~QwBeamMod() {};

  std::vector<TString> fgModTypeNames;
  /* derived from VQwSubsystem */

  //Handle command line options
  void ProcessOptions(QwOptions &options);
  void AccumulateRunningSum(VQwSubsystem*);
  //remove one entry from the running sums for devices
  void DeaccumulateRunningSum(VQwSubsystem* value){
  };

  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadEventCuts(TString filename);//derived from VQwSubsystemParity
  Int_t LoadGeometry(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);


  Bool_t ApplySingleEventCuts();//derived from VQwSubsystemParity
  void IncrementErrorCounters();
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliures
  UInt_t GetEventcutErrorFlag();//return the error flag

  //update the error flag in the subsystem level from the top level routines related to stability checks. This will uniquely update the errorflag at each channel based on the error flag in the corresponding channel in the ev_error subsystem
  void UpdateErrorFlag(const VQwSubsystem *ev_error);

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
//  void  PrintDetectorID();

  void  ClearEventData();

  void  ProcessEvent();
  void  ExchangeProcessedData();
  void  ProcessEvent_2();

  VQwSubsystem&  operator=  (VQwSubsystem *value);
  VQwSubsystem&  operator+= (VQwSubsystem *value);
  VQwSubsystem&  operator-= (VQwSubsystem *value);


  void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);

  void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);

  void Scale(Double_t factor);

  void CalculateRunningAverage();
  void PrintModChannelID();

  using VQwSubsystem::ConstructHistograms;
  void ConstructHistograms(TDirectory *folder, TString &prefix);
  void FillHistograms();

  using VQwSubsystem::ConstructBranchAndVector;
  void ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void ConstructBranch(TTree *tree, TString& prefix) { };
  void ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file) { };
  void FillTreeVector(std::vector<Double_t> &values) const;

#ifdef __USE_DATABASE__
  void FillDB_MPS(QwParityDB *db, TString datatype);
  void FillDB(QwParityDB *db, TString datatype);
  void FillErrDB(QwParityDB *db, TString datatype);
#endif // __USE_DATABASE__
  void WritePromptSummary(QwPromptSummary *ps, TString type);

  Bool_t Compare(VQwSubsystem *source);

  void Print();

  void AtEndOfEventLoop();
  void AnalyzeOpticsPlots();
  void ResizeOpticsDataContainers(Int_t);
  void ClearVectors();

  Double_t GetAmplitudeSign(Double_t, Double_t, Double_t, Double_t);

 protected:
 Int_t GetDetectorTypeID(TString name);
 Int_t GetDetectorIndex(Int_t TypeID, TString name);    // when the type and the name is passed the detector index from appropriate vector will be returned
 Int_t fTreeArrayIndex; 						        // for example if TypeID is bcm  then the index of the detector from fBCM vector for given name will be returnd.


 std::vector <QwVQWK_Channel> fModChannel;
 std::vector <QwModChannelID> fModChannelID;
 std::vector <QwWord> fWord;
 std::vector < std::pair<Int_t, Int_t> > fWordsPerSubbank;



 private:

 UInt_t fFFB_Index;
 UInt_t fFFB_holdoff;
 UInt_t fFFB_holdoff_Counter;
 UInt_t fFFB_ErrorFlag;
 Bool_t fFFB_Flag;
 static const Bool_t bDEBUG=kFALSE;

 static const Int_t fNumberPatterns = 5;

 // List of monitor channels

 std::vector<TString> fMonitorNames;
 std::vector<QwVQWK_Channel> fMonitors;
 std::vector<Double_t> fBPMPosition;

 std::vector <std::vector <Double_t> > fAmplitude;
 std::vector <std::vector <Double_t> > fOffset;
 std::vector <std::vector <Double_t> > fPhase;

 std::vector <std::vector <Double_t> > fAmplitudeError;
 std::vector <std::vector <Double_t> > fOffsetError;
 std::vector <std::vector <Double_t> > fPhaseError;

 std::vector <std::vector <Double_t> > fChisquare;
 std::vector <std::vector <Int_t> > fNFitPoints;

 Int_t fRampChannelIndex;
 Int_t fPatternWordIndex;

};



class QwModChannelID
{
 public:

  QwModChannelID(Int_t subbankid, Int_t wordssofar,TString name,
		   TString modtype ,QwBeamMod * obj);


/*     QwModChannelID():fSubbankIndex(-1),fWordInSubbank(-1),fTypeID(-1),fIndex(-1), */
/*     fSubelement(999999),fmoduletype(""),fmodulename("") */
/*     {}; */

  Int_t fSubbankIndex;        //Generated from ROCID(readout CPU) & BankID(corespondes to internal headers to ID differnt types of data..ex. F1TDC)
  Int_t fWordInSubbank;
  //first word reported for this channel in the subbank
  //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)

  // The first word of the subbank gets fWordInSubbank=0

  TString fmoduletype; // eg: VQWK, SCALER
  TString fmodulename;
 // TString fdetectortype;

  Int_t  kUnknownDeviceType;
  Int_t  fTypeID;           // type of detector eg: lumi or stripline, etc..
  Int_t  fIndex;            // index of this detector in the vector containing all the detector of same type

  void Print();

};



#endif

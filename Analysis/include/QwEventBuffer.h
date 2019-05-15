/**********************************************************\
* File: QwEventBuffer.h                                    *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-22 15:40>                           *
\**********************************************************/

#ifndef __QWEVENTBUFFER__
#define __QWEVENTBUFFER__


#include <string>
#include <vector>
#include "Rtypes.h"
#include "TString.h"
#include "TStopwatch.h"

#include "THaCodaData.h"

#include "MQwCodaControlEvent.h"
#include "QwParameterFile.h"

#include <unordered_map>

class QwOptions;
class QwEPICSEvent;
class VQwSubsystem;
class QwSubsystemArray;

//////////////////////////////////////////////////////////////////////


///
/// \ingroup QwAnalysis
class QwEventBuffer: public MQwCodaControlEvent{
 public:
  static void DefineOptions(QwOptions &options);
  static void SetDefaultDataDirectory(const std::string& dir) {
	fDefaultDataDirectory = dir;
  }
  static void SetDefaultDataFileStem(const std::string& stem) {
	fDefaultDataFileStem = stem;
  }
  static void SetDefaultDataFileExtension(const std::string& extension) {
	fDefaultDataFileExtension = extension;
  }

 public:
  static const Int_t kRunNotSegmented;
  static const Int_t kNoNextDataFile;
  static const Int_t kFileHandleNotConfigured;

  static const UInt_t kNullDataWord;



 public:
  QwEventBuffer();
  virtual ~QwEventBuffer() {
    // Delete event stream
    if (fEvStream != NULL) {
      delete fEvStream;
      fEvStream = NULL;
    }
    // Delete run list file
    if (fRunListFile != NULL) {
      delete fRunListFile;
      fRunListFile = NULL;
    }
  };

  /// \brief Sets internal flags based on the QwOptions
  void ProcessOptions(QwOptions &options);

  void PrintRunTimes();

  /// \brief Returns a string like <run#> or <run#>.<file#>
  TString GetRunLabel() const;
  /// \brief Return true if file segments are being separated for
  //analysis
  Bool_t AreRunletsSplit() const   {
    return (fRunIsSegmented && !fChainDataFiles);
  };
  /// \brief Return CODA file run number
  Int_t GetRunNumber() const {return fCurrentRun;};
  /// \brief Return CODA file segment number
  Int_t GetSegmentNumber() const {
    return fRunSegments.size() ? *fRunSegmentIterator : 0;
  };

  std::pair<UInt_t, UInt_t> GetEventRange() const {
    return fEventRange;
  };

  /// \brief Opens the event stream (file or ET) based on the internal flags
  Int_t OpenNextStream();
  /// \brief Closes a currently open event stream.
  Int_t CloseStream();

  void    SetDataDirectory(const TString datadir){fDataDirectory = datadir;}


  const TString&  GetDataFile() const {return fDataFile;};
  const TString&  GetDataDirectory() const {return fDataDirectory;};

  Int_t ReOpenStream();

  Int_t OpenDataFile(UInt_t current_run, Short_t seg);
  Int_t OpenDataFile(UInt_t current_run, const TString rw = "R");
  Int_t OpenDataFile(const TString filename, const TString rw = "R");
  Int_t CloseDataFile();

  Int_t OpenETStream(TString computer, TString session, int mode, const TString stationname="");
  Int_t CloseETStream();

  Bool_t IsPhysicsEvent() {
    // fEvtType is an unsigned integer, hence always positive
    return ((fIDBankNum == 0xCC) && ( /* fEvtType >= 0 && */ fEvtType <= 15));
  };

  Int_t GetEventNumber() { return fEvtNumber; };

  Bool_t GetNextEventRange();
  Bool_t GetNextRunRange();
  Bool_t GetNextRunNumber();

  Int_t GetNextEvent();

  Int_t  GetEvent();
  Int_t  WriteEvent(int* buffer);

  Bool_t IsOnline(){return fOnline;};

  Bool_t IsROCConfigurationEvent(){
    return (fEvtType>=0x90 && fEvtType<=0xaf);
  };

  Bool_t IsEPICSEvent(){
    //  What are the correct codes for our EPICS events?
    //return (fEvtType>=160 && fEvtType<=170);// epics event type is only with tag="160"
    return (fEvtType>=160 && fEvtType<=190);// epics event type is only with tag="180" from July 2010 running
  };

  Bool_t FillSubsystemConfigurationData(QwSubsystemArray &subsystems);
  Bool_t FillSubsystemData(QwSubsystemArray &subsystems);

  Bool_t FillEPICSData(QwEPICSEvent &epics);

  template < class T > Bool_t FillObjectWithEventData(T &t);


  Int_t EncodeSubsystemData(QwSubsystemArray &subsystems);
  Int_t EncodePrestartEvent(int runnumber, int runtype = 0);
  Int_t EncodeGoEvent();
  Int_t EncodePauseEvent();
  Int_t EncodeEndEvent();

  void ResetFlags();

 private:
  //  These methods will be removed from a future version
  void ClearEventData(std::vector<VQwSubsystem*> &subsystems);

  Bool_t FillSubsystemConfigurationData(std::vector<VQwSubsystem*> &subsystems);
  Bool_t FillSubsystemData(std::vector<VQwSubsystem*> &subsystems);

 protected:
  ///
  Bool_t fOnline;
  TString fETHostname;
  TString fETSession;
  TString fETStationName;

  Bool_t fAllowLowSubbankIDs;

  Bool_t fChainDataFiles;
  std::pair<Int_t, Int_t> fRunRange;
  std::string fRunListFileName;
  QwParameterFile* fRunListFile;
  std::vector<Int_t> fRunRangeMinList, fRunRangeMaxList;

  std::pair<UInt_t, UInt_t> fEventRange;
  std::string fEventListFileName;
  QwParameterFile* fEventListFile;
  std::vector<UInt_t> fEventList;

  std::pair<Int_t, Int_t> fSegmentRange;

 protected:

  static std::string fDefaultDataDirectory;
  static std::string fDefaultDataFileStem;
  static std::string fDefaultDataFileExtension;

  TString fDataFileStem;
  TString fDataFileExtension;

  TString fDataDirectory;
  TString fDataFile;

  // UInt_t fRunNumber;


 protected:
  Int_t  GetFileEvent();
  Int_t  GetEtEvent();

  Int_t WriteFileEvent(int* buffer);

  Bool_t DataFileIsSegmented();

  Int_t CloseThisSegment();
  Int_t OpenNextSegment();

  void DecodeEventIDBank(UInt_t *buffer);
  Bool_t DecodeSubbankHeader(UInt_t *buffer);

  const TString&  DataFile(const UInt_t run, const Short_t seg);

  //  void SetEventLength(const ULong_t tmplength) {fEvtLength = tmplength;};
  //  void SetEventType(const UInt_t tmptype) {fEvtType = tmptype;};
  //  void SetWordsSoFar(const ULong_t tmpwords) {fWordsSoFar = tmpwords;};



 protected:
  enum CodaStreamMode{fEvStreamNull, fEvStreamFile, fEvStreamET} fEvStreamMode;
  THaCodaData *fEvStream; //  Pointer to a THaCodaFile or THaEtClient

  Int_t fCurrentRun;

  Bool_t fRunIsSegmented;


  std::vector<Int_t>           fRunSegments;
  std::vector<Int_t>::iterator fRunSegmentIterator;


 protected:
  Bool_t fPhysicsEventFlag;

  UInt_t fEvtLength;
  UInt_t fWordsSoFar;

  UInt_t fEvtType;

  UInt_t fEvtTag;
  UInt_t fBankDataType;
  UInt_t fIDBankNum;

  UInt_t fEvtNumber;   ///< CODA event number; only defined for physics events
  UInt_t fEvtClass;
  UInt_t fStatSum;

  Double_t fCleanParameter[3]; ///< Scan data/clean data from the green monster

  UInt_t fFragLength;
  BankID_t fSubbankTag;
  UInt_t fSubbankType;
  UInt_t fSubbankNum;
  ROCID_t fROC;

  TStopwatch fRunTimer;      ///<  Timer used for runlet processing loop
  TStopwatch fStopwatch;     ///<  Timer used for internal timing

 protected:
  ///  Methods and data members needed to find marker words
  typedef ULong64_t RocBankLabel_t;
  std::unordered_map<RocBankLabel_t, std::vector<UInt_t> > fMarkerList;
  std::unordered_map<RocBankLabel_t, std::vector<UInt_t> > fOffsetList;

  Int_t CheckForMarkerWords(QwSubsystemArray &subsystems);
  RocBankLabel_t fThisRocBankLabel;
  UInt_t FindMarkerWord(UInt_t markerID, UInt_t* buffer, UInt_t num_words);
  UInt_t GetMarkerWord(UInt_t markerID);

 protected:
  UInt_t     fNumPhysicsEvents;
  UInt_t     fStartingPhysicsEvent;

};

template < class T > Bool_t QwEventBuffer::FillObjectWithEventData(T &object){
  ///  Template to fill any object with data from a CODA event.
  ///
  ///  The classes for which this template can be specialized
  ///  must have the following three methods defined:
  ///
  ///  - Bool_t T::CanUseThisEventType(const UInt_t event_type);
  ///  - Bool_t T::ClearEventData(const UInt_t event_type);
  ///  - Int_t  T::ProcessBuffer(const UInt_t event_type,
  ///       const ROCID_t roc_id, const BankID_t bank_id, 
  ///       const UInt_t banktype, UInt_t* buffer, UInt_t num_words);
  ///
  Bool_t okay = kFALSE;
  UInt_t *localbuff = (UInt_t*)(fEvStream->getEvBuffer());

  if (fFragLength==1 && localbuff[fWordsSoFar]==kNullDataWord){
    fWordsSoFar += fFragLength;
  } else if (object.CanUseThisEventType(fEvtType)){
    //  Clear the old event information from the object
    object.ClearEventData(fEvtType);
    //  Loop through the data buffer in this event.
    if (fBankDataType == 0x10){
      //  This bank is subbanked; loop through subbanks
      while ((okay = DecodeSubbankHeader(&localbuff[fWordsSoFar]))){
	//  If this bank has further subbanks, restart the loop.
	if (fSubbankType == 0x10) continue;
	//  If this bank only contains the word 'NULL' then skip
	//  this bank.
	if (fFragLength==1 && localbuff[fWordsSoFar]==kNullDataWord){
	  fWordsSoFar += fFragLength;
	  continue;
	}
	object.ProcessBuffer(fEvtType, fROC, fSubbankTag, fSubbankType,
			     &localbuff[fWordsSoFar],
			     fFragLength);
	fWordsSoFar += fFragLength;
      }
    } else {
      //  This is a single bank of some type
      object.ProcessBuffer(fEvtType, 0, fBankDataType,
			   &localbuff[fWordsSoFar],
			   fEvtLength);
    }
  }
  return okay;
}



#endif

/**********************************************************\
* File: QwBeamDetectorID.h                                *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QWBEAMDETECTORID__
#define __QWBEAMDETECTORID__

// ROOT headers
#include "Rtypes.h"
#include "TString.h"

// Qweak headers
#include "QwTypes.h"

//  Forward declarations
class QwParameterFile;

/*****************************************************************
*  Class:
******************************************************************/
class QwBeamDetectorID {
public:
  QwBeamDetectorID(Int_t subbankid, Int_t offset,TString name, TString dettype,
		   TString modtype);
  QwBeamDetectorID(Int_t subbankid, QwParameterFile &paramfile);
  QwBeamDetectorID(const QwBeamDetectorID& input);
  ~QwBeamDetectorID(){};

  Bool_t ReportInitErrors() const;
  void   Print() const;

public:
  Int_t   fSubbankIndex;
  Int_t   fWordInSubbank;
  //first word reported for this channel in the subbank
  //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)

  // The first word of the subbank gets fWordInSubbank=0

  TString fmoduletype; // eg: VQWK, SCALER
  TString fdetectorname;
  TString fdetectortype; // stripline, bcm, ... this string is encoded by fTypeID

  EQwBeamInstrumentType   fTypeID; // type of detector eg: bcm or stripline, etc..
  Int_t   fIndex;            // index of this detector in the vector containing all the detector of same type
  UInt_t  fSubelement;       // some detectors have many subelements (eg stripline have 4 antenas) some have only one sub element(eg lumis have one channel)
  TString fChannelName;
  TString fSubelementName;

private:
  QwBeamDetectorID();

};

#endif

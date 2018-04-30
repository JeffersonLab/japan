/**********************************************************\
* File: MQwV775TDC.h                                       *
*                                                          *
* Author: P. M. King                                       *
*         J. H. Lee                                        *
* Time-stamp: <2010-04-12 12:12>                           *
\**********************************************************/


#ifndef __MQwV775TDC__
#define __MQwV775TDC__

#include "Rtypes.h"
#include <cstdlib>
#include <iostream>

///
/// \ingroup QwAnalysis
class MQwV775TDC{
  /******************************************************************
   *  Class: MQwV775TDC
   *         Mix-in class containing decoding functions for the V775TDC
   *         32-bit dataword.
   *         The functions in this class will decode a single word
   *         of V775TDC data and provide the components of the word
   *         through member functions.
   *
   *         The decoding of the CAEN V792 ADC is nearly identical;
   *         the 792 does not use the "DataValidBit", so it is
   *         disabled in this version to allow this class to work
   *         for both the ADC and TDC.
   ******************************************************************/
 public:
  MQwV775TDC();
  ~MQwV775TDC();

  void DecodeTDCWord(UInt_t &word, const UInt_t roc_id=0);
  
  Bool_t IsValidDataword()    {return fV775ValidFlag;};
  Bool_t IsHeaderword()       {return fV775HeaderFlag;};

  UInt_t GetTDCSlotNumber()   {return fV775SlotNumber;};
  UInt_t GetTDCChannelNumber(){return fV775ChannelNumber;};
  UInt_t GetTDCData()         {return fV775Dataword;};
  UInt_t GetTDCMaxChannels()  {return fV775MaxChannelsPerModule;};

  UInt_t GetTDCEventNumber()  {return fV775EventNumber;};
  UInt_t GetTDCTriggerTime();

  void SetReferenceParameters(Double_t mindiff, Double_t maxdiff,
			      Double_t offset, Double_t shift){
    //  Do nothing, but keep this function to behave like
    //  the MQwF1TDC class.
  };

  Double_t SubtractReference(Double_t rawtime, Double_t reftime);
  Bool_t CheckDataIntegrity(const UInt_t roc_id, UInt_t *buffer, UInt_t num_words);
  void   PrintTDCHeader(Bool_t flag) ; 
  void   PrintTDCData(Bool_t flag) ; 

 private:
  static const UInt_t kV775Mask_SlotNumber;
  static const UInt_t kV775Mask_WordType;

  static const UInt_t kV775Mask_CrateNumber;
  static const UInt_t kV775Mask_HitChannels;

  static const UInt_t kV775Mask_EventCounter;

  static const UInt_t kV775Mask_ChannelNumber;
  static const UInt_t kV775Mask_DataValidBit;
  static const UInt_t kV775Mask_UnderthresholdBit;
  static const UInt_t kV775Mask_OverflowBit;
  static const UInt_t kV775Mask_Dataword;

  static const UInt_t kV775WordType_NotValid;
  static const UInt_t kV775WordType_Header;
  static const UInt_t kV775WordType_Tail;
  static const UInt_t kV775WordType_Datum;

  Bool_t fV775ValidFlag;
  Bool_t fV775HeaderFlag;

  UInt_t fV775SlotNumber;
  UInt_t fV775ChannelNumber;
  UInt_t fV775Dataword;
  UInt_t fV775MaxChannelsPerModule;
  UInt_t fV775EventNumber;

};

#endif

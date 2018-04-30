
/**********************************************************\
* File: MQwF1TDC.h                                         *
*                                                          *
* Author: P. M. King                                       *
*         J. H. Lee                                        *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/


#ifndef __MQwF1TDC__
#define __MQwF1TDC__

#include "Rtypes.h"
#include "QwTypes.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>

///
/// \ingroup QwAnalysis
class MQwF1TDC{
  /******************************************************************
   *  Class: MQwF1TDC
   *         Mix-in class containing decoding functions for the F1TDC
   *         32-bit dataword.
   *         The functions in this class will decode a single word
   *         of F1TDC data and provide the components of the word
   *         through member functions.
   ******************************************************************/
 
  
 public:
  MQwF1TDC();
  ~MQwF1TDC();

  friend std::ostream& operator<<(std::ostream& os, const MQwF1TDC &f1tdc);

  void DecodeTDCWord(UInt_t &word, const UInt_t roc_id);

  Bool_t IsValidDataword() const;
  const Bool_t& IsHeaderword()        const {return fF1HeaderFlag;};
 
  const UInt_t& GetTDCSlotNumber()    const {return fF1SlotNumber;};
  const UInt_t& GetTDCChannelNumber() const {return fF1ChannelNumber;};
  const UInt_t& GetTDCChipAddress()   const {return fF1ChipAddress;};
  const UInt_t& GetTDCChannelAddress()const {return fF1ChannelAddress;};

  const UInt_t& GetTDCData()          const {return fF1Dataword;};
  const UInt_t& GetTDCMaxChannels()   const {return fF1MaxChannelsPerModule;};
  
 
  UInt_t GetTDCEventNumber()       const {return GetTDCHeaderEventNumber();};
  UInt_t GetTDCTriggerTime()       const {return GetTDCHeaderTriggerTime();};


  void SetTDCMaxChannels(const UInt_t in) {fF1MaxChannelsPerModule = in;};





  
  /* Double_t SubtractReference(Double_t rawtime, Double_t reftime); */
  /* Double_t ActualTimeDifference(Double_t raw_time, Double_t ref_time); */
  /* Bool_t CheckDataIntegrity(const UInt_t roc_id, UInt_t *buffer, UInt_t num_words); */
  void   PrintTDCHeader(Bool_t flag);
  void   PrintTDCData(Bool_t flag);
  void   Print(Bool_t flag);
  


  UInt_t GetTDCHeaderEventNumber() const {return fF1HeaderEventNumber;};
  UInt_t GetTDCHeaderTriggerTime() const {return fF1HeaderTriggerTime;};


  Bool_t IsValidDataSlot()         const {return fF1ValidDataSlotFlag;};
 
  Bool_t IsHeaderXorSetup()        const {return fF1HeaderXorSetupFlag;};
  Bool_t IsNotHeaderTrigFIFO()     const {return !fF1HeaderTrigFIFOFlag;};
  Bool_t IsOverFlowEntry()         const {return fF1OverFlowEntryFlag;};

  Bool_t IsHitFIFOOverFlow()       const {return fF1HitFIFOFlag;};
  Bool_t IsOutputFIFOOverFlow()    const {return fF1OutputFIFOFlag;};
  Bool_t IsResolutionLock()        const {return fF1ResolutionLockFlag;};

  Bool_t IsFakeData()              const {return fF1FakeDataFlag;};

 private:


  //  *********  DATA and Header words ************
  // Bit 31 - 0 bit (32 bit)
  // 31 - 24 bit
  static const UInt_t kF1Mask_SlotNumber;
  static const UInt_t kF1Mask_ResolutionLockFlag;
  static const UInt_t kF1Mask_OutputFIFOFlag;
  static const UInt_t kF1Mask_HitFIFOFlag;
  // F1TDC chip output 24-bit words.
  // 23 bit for all
  static const UInt_t kF1Mask_HeaderFlag;



  //  *********  DATA words ************
  // 22    bit  : 1 Bit Fake Data Flag
  //              0 > a real data
  //              1 > a fake data 
  // This bit is not described in the F1TDC manual (1.2), I think,
  // is introduced by after private communcation with Ed Jastrzembski. 
  // Thursday, September 16 14:04:18 EDT 2010, jhlee
  static const UInt_t kF1Mask_FakeDataFlag;

  // 16-21 bit : upper 3 Bit Chip address, lower 3 Bit Channel address
  //             the total 6 Bit channel number 0-63
  static const UInt_t kF1Mask_ChannelNumber;
  static const UInt_t kF1Mask_ChipAddress;
  static const UInt_t kF1Mask_ChannelAddress;

  // 15- 0 bit : 16 Bit time
  static const UInt_t kF1Mask_Dataword;
  

  //  *********  Header words ************
  // 22    bit : 1 Bit Trigger FIFO overflow
  static const UInt_t kF1Mask_HeaderTrigFIFOFlag;
  
  // 21-16 bit : 6 Bit Event Number
  static const UInt_t kF1Mask_HeaderEventNumber;

  // 15- 8 bit : 9 Bit Trigger Time
  static const UInt_t kF1Mask_HeaderTriggerTime;

  // 7     bit : 1 Bit Xor Setup register
  static const UInt_t kF1Mask_HeaderXorSetupFlag;

  // 6 - 0 bit : upper 3 Bit Chip address, lower 3 Bit Channel address
  //             the total 6 Bit channel number 0-63
  static const UInt_t kF1Mask_HeaderChannelNumber;
  static const UInt_t kF1Mask_HeaderChipAddress;
  static const UInt_t kF1Mask_HeaderChannelAddress;
  
  
  //  static const UInt_t offset;
  UInt_t fF1ROCNumber;
  UInt_t fF1SlotNumber;

  Bool_t fF1HeaderFlag;              // true(1) if word is 0 (header) and false(0) 
                                     // if word is 1 (data)
  Bool_t fF1HitFIFOFlag;             // true(1) if word is 1 
  Bool_t fF1OutputFIFOFlag;          // true(1) if word is 1
  Bool_t fF1ResolutionLockFlag;      // true(1) if word is 1

 
  Bool_t fF1FakeDataFlag;            // true(1) if word is 1 (Fake)
  UInt_t fF1ChannelNumber;
  UInt_t fF1ChipAddress;
  UInt_t fF1ChannelAddress;
  UInt_t fF1Dataword;



  Bool_t fF1HeaderTrigFIFOFlag;
  UInt_t fF1HeaderEventNumber;
  UInt_t fF1HeaderTriggerTime;
  Bool_t fF1HeaderXorSetupFlag;
 
  
  UInt_t fF1MaxChannelsPerModule;
  Bool_t fF1OverFlowEntryFlag;
  Bool_t fF1ValidDataSlotFlag;  

  // Slot 1 - 21 indicates valid data
  // Slot 0  is the tag for a "filler" word. This is a non-valid data word that is
  //         inserted to make the block of data output from the module consist of
  //         an "even" number of words.
  // Slot 30 : the module will return 30 when the data is not valid
  // 
  //           Tempoararily comment out, because I have no idea how
  //           to check this inside MQwF1TDC so we don't interrupt 
  //           MQwV775TDC. I think "IsSlotRegistered()" can do instead of this
  //                   

  void   PrintHitFIFOStatus(const UInt_t roc_id);
  void   PrintOutputFIFOStatus(const UInt_t roc_id);
  void   PrintResolutionLockStatus(const UInt_t roc_id);

 

};

#endif

#ifndef __QwF1TDContainer__
#define __QwF1TDContainer__
/**
 *  \file   QwF1TDContainer.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Tuesday, September 14 23:04:45 EDT 2010
 */

#include <iostream>
#include <math.h>


#include "QwTypes.h"
#include "MQwF1TDC.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TArrayD.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"

#include "TROOT.h"
#include "TFile.h"
#include "TH2F.h"



/**
 *  \class QwF1TDC
 *  \ingroup QwAnalysis
 *
 *  \brief one F1TDC configuration and reference signal(s) holder
 *
 */

#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif


class QwF1TDC :  public TObject
{

 public:
  QwF1TDC();
  QwF1TDC(const Int_t roc, const Int_t slot);
  virtual ~QwF1TDC();
  friend std::ostream& operator<<(std::ostream& os, const QwF1TDC &f1tdc);

  Int_t  GetROCNumber()          const {return fROC;};
  Int_t  GetSlotNumber()         const {return fSlot;};
  Int_t  GetChannelNumber()      const {return fChannelNumber;};

  Bool_t IsReferenceSlot()       const {return fReferenceSlotFlag;};
  Bool_t IsNormResolution()      const {return fF1TDCNormResFlag;};
  Bool_t IsSyncMode()            const {return fF1TDCSyncFlag;};

  UInt_t GetF1TDC_refcnt()       const {return fF1TDC_refcnt;};
  UInt_t GetF1TDC_hsdiv()        const {return fF1TDC_hsdiv;};
  UInt_t GetF1TDC_refclkdiv()    const {return fF1TDC_refclkdiv;};
  UInt_t GetF1TDC_trigwin()      const {return fF1TDC_trigwin;};
  UInt_t GetF1TDC_triglat()      const {return fF1TDC_triglat;};

  Double_t GetF1TDC_tframe()     const {return fF1TDC_tframe_ns;};
  Double_t GetF1TDC_full_range() const {return fF1TDC_full_range_ns;};
  Double_t GetF1TDC_window()     const {return fF1TDC_window_ns;};
  Double_t GetF1TDC_latency()    const {return fF1TDC_latency_ns;}; 
  Double_t GetF1TDC_resolution() const {return fF1TDC_resolution_ns;};
  Double_t GetF1TDC_bin_size()   const {return fF1TDC_resolution_ns;};

  Double_t GetF1TDC_t_offset()   const {return fF1TDC_t_offset;};

  Double_t GetF1TDC_trig_t_offset()   const {return fF1TDC_trig_t_offset;};
  Double_t GetF1TDC_trig_resolution() const {return fF1TDC_trig_resolution_ns;};

  Int_t   GetF1TDCIndex()        const {return fF1TDCIndex;};
  Int_t   GetF1BankIndex()       const {return fF1BankIndex;};
  const TString GetF1SystemName()      const {return fSystemName;};

  void SetROCNumber       (const Int_t roc)     {fROC = roc;};
  void SetSlotNumber      (const Int_t slot)    {fSlot = slot;};
  void SetReferenceSlot   (const Bool_t reflag) {fReferenceSlotFlag = reflag;};


  void SetF1TDCIndex  (const Int_t tdc_index)   {fF1TDCIndex = tdc_index;};
  void SetF1BankIndex (const Int_t bank_index)  {fF1BankIndex = bank_index;};
  void SetF1SystemName(const TString name)      {fSystemName = name;};
  
  void SetF1TDCBuffer(UInt_t *buffer, UInt_t num_words);
  const UInt_t * GetF1TDCBuffer() const {return fBuffer;};

  void PrintF1TDCBuffer();
  void PrintF1TDCConfigure();

  void PrintChannelErrorCounter(Int_t channel);
  void PrintErrorCounter();
  void PrintTotalErrorCounter();

  void PrintContact();

  TString GetChannelErrorCounter(Int_t channel);
  TString GetErrorCounter();
  TString GetTotalErrorCounter();

  void AddSEU(Int_t channel) {fF1TDC_SEU_counter[channel]++;};
  void AddSYN(Int_t channel) {fF1TDC_SYN_counter[channel]++;};
  void AddEMM(Int_t channel) {fF1TDC_EMM_counter[channel]++;};
  void AddTFO(Int_t channel) {fF1TDC_TFO_counter[channel]++;};
  void AddRLF(Int_t channel) {fF1TDC_RLF_counter[channel]++;};
  void AddHFO(Int_t channel) {fF1TDC_HFO_counter[channel]++;};
  void AddOFO(Int_t channel) {fF1TDC_OFO_counter[channel]++;};
  void AddFDF(Int_t channel) {fF1TDC_FDF_counter[channel]++;};
  void AddS30(Int_t channel) {fF1TDC_S30_counter[channel]++;};


  UInt_t GetSEU(Int_t channel) const {return fF1TDC_SEU_counter[channel];};
  UInt_t GetSYN(Int_t channel) const {return fF1TDC_SYN_counter[channel];};
  UInt_t GetEMM(Int_t channel) const {return fF1TDC_EMM_counter[channel];};
  UInt_t GetTFO(Int_t channel) const {return fF1TDC_TFO_counter[channel];};

  UInt_t GetRLF(Int_t channel) const {return fF1TDC_RLF_counter[channel];};   // Resolution Lock Fail couter
  UInt_t GetHFO(Int_t channel) const {return fF1TDC_HFO_counter[channel];};   // Hit    Fifo Overflow counter
  UInt_t GetOFO(Int_t channel) const {return fF1TDC_OFO_counter[channel];};   // Output Fifo Overflow coutner;

  UInt_t GetFDF(Int_t channel) const {return fF1TDC_FDF_counter[channel];};
  UInt_t GetS30(Int_t channel) const {return fF1TDC_S30_counter[channel];};

  UInt_t GetTotalSEU();
  UInt_t GetTotalSYN();
  UInt_t GetTotalEMM();
  UInt_t GetTotalTFO();

  UInt_t GetTotalRLF();
  UInt_t GetTotalHFO();
  UInt_t GetTotalOFO();

  UInt_t GetTotalFDF();
  UInt_t GetTotalS30();

  UInt_t GetTotalRFM() {return fF1TDC_RFM_counter;};

  void ResetCounters();

  //void SetReferenceSignals(Int_t chan_row, Double_t val);

  Double_t ReferenceSignalCorrection(Double_t raw_time, Double_t ref_time);


 public:
  Int_t fROC;          // F1TDC physical roc number, not index 
                       // 4  : ROC 4   
                       // 9  : ROC 9
                       // 10 : ROC 10

  Int_t fSlot;         // F1TDC slot number 
                       // QwAnalysis' slot range 0-20. But, the physical VME
                       // slot range is 1-21. QwAlaysis's slot range is acceptable
                       // if we carefully match them with each other as follows:
                       //  QwAnalysis slot 0, 1, 2, .... , 2
                       //  VME        slot 1, 2, 3, .... , 21
                       // Inside codes, we did the following way:
                       //  QwAnalysis slot 0, 1, 2, .... , 20
                       //     VME     slot    1, 2, 3, .... , 21
                       // It is acceptable if we don't use the VME slot 21 as
                       // we do.
                       // Friday, July 30 11:26:36 EDT 2010, jhlee

 
  
  Int_t   fChannelNumber;
  UInt_t   *fBuffer;


  UInt_t   fF1TDC_refcnt;
  UInt_t   fF1TDC_hsdiv;
  UInt_t   fF1TDC_refclkdiv;
  UInt_t   fF1TDC_trigwin;
  UInt_t   fF1TDC_triglat;
  
  Double_t fF1TDC_tframe_ns;
  Double_t fF1TDC_full_range_ns;
  //  Double_t fF1TDC_bin_size_ns;
  Double_t fF1TDC_window_ns;
  Double_t fF1TDC_latency_ns;
  Double_t fF1TDC_resolution_ns;

  Double_t fF1TDC_t_offset;

  Double_t fF1TDC_trig_resolution_ns;
  Double_t fF1TDC_trig_t_offset;


  Double_t fF1TDCFactor;


  // We don't look at these errors upto "channels",
  // and we care these numbers per each F1TDC.
  // There is no reason to track them upto channel levels.
  // Do we have?
  // Monday, September 13 14:55:10 EDT 2010, jhlee

  UInt_t *fF1TDC_SEU_counter;  // Single Event Upset counter
  UInt_t *fF1TDC_EMM_counter;  // Event number MisMatch counter
  UInt_t *fF1TDC_SYN_counter;  // SYNc  issue counter
  UInt_t *fF1TDC_TFO_counter;  // Trigger Fifo Overflow counter


  UInt_t *fF1TDC_RLF_counter;   // Resolution Lock Fail couter
  UInt_t *fF1TDC_HFO_counter;   // Hit    Fifo Overflow counter
  UInt_t *fF1TDC_OFO_counter;   // Output Fifo Overflow coutner;
  UInt_t *fF1TDC_FDF_counter;   // Fake Data Flag counter 
  UInt_t *fF1TDC_S30_counter;   // Slot 30 counter

  Bool_t   fReferenceSlotFlag; //! 
  TArrayD  *fReferenceSignals; //!

  Bool_t   fF1TDCNormResFlag;
  Bool_t   fF1TDCSyncFlag;


  Int_t    fF1TDCIndex;  // keep the tdcindex info from GetTDCIndex() function
  Int_t    fF1BankIndex; // This is the Bank index, which
                         // is the redefined index to access "bank=0x0901"
                         // defined in map file (e.g. qweak_R2.map)
 
 private:

  //TString  fF1ErrorSummaryString;

  TString  fSystemName;
  static const Int_t fWordsPerBuffer;
  static const Int_t fMaxF1TDCChannelNumber;
  
  UInt_t  fF1TDC_RFM_counter;  // ReFerence signal Miss counter

  UInt_t GetTotal(UInt_t* error_counter);

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwF1TDC,1);
  #endif

};

/**
 *  \class QwF1TDContainer
 *  \ingroup QwAnalysis
 *
 *  \brief  F1TDCs configuration and reference siganls container
 * 
 *  This would be used to be a configuration and reference signal(s)
 *  container for F1TDCs which are used in one subsystem. The subsystem
 *  has one QwF1TDContaier that have DetectorType, Region, and QwF1TDC(s)
 *
 */

// TODO
// * Reference signal(s) holder?
// * How to let a shift crew to recognize F1TDC problems
// * Is it valuable to save these errors into ROOT file?
// 

class QwF1TDContainer :  public TObject
{
  
 public:
  Int_t                    fNQwF1TDCs;
  TObjArray               *fQwF1TDCList; 
 
 public:
  QwF1TDContainer();
  virtual ~QwF1TDContainer();

  // friend std::ostream& operator<<(std::ostream& os, const QwF1TDContainer &container);

  void SetSystemName(const TString name);// {fSystemName = name;};
  const TString GetSystemName() const {return fSystemName;};

  void AddQwF1TDC(QwF1TDC *in);
  void Print(const Option_t* options = 0) const;

  Int_t              GetSize()         const {return fNQwF1TDCs;};
  Int_t              Size()            const {return fNQwF1TDCs;};
  Int_t              HowManyF1TDCs()   const {return fNQwF1TDCs;};
  EQwDetectorType    GetDetectorType() const {return fDetectorType;}
  EQwRegionID        GetRegion()       const {return fRegion;};

  QwF1TDC* GetF1TDC(Int_t roc, Int_t slot);
  QwF1TDC* GetF1TDCwithIndex(Int_t tdc_index);
  QwF1TDC* GetF1TDCwithBankIndexSLOT(Int_t bank_index, Int_t slot);

  Double_t GetF1TDCOneResolution() const {return fF1TDCOneResolutionNS;};
  Double_t DoneF1TDCsConfiguration();

  Double_t ReturnTimeCalibration(Double_t time_arb_unit) {return fF1TDCOneResolutionNS*time_arb_unit;};

  //
  // This function will be removed after several tests
  // in order to reduce 'call' per event/hit
  // Saturday, January 21 15:33:40 EST 2012, jhlee

  /// Double_t GetF1TDCResolution();

  Int_t GetF1TDCChannelNumber();
  const TH2F* GetF1TDCErrorHist();

  
  void AddSYN(Int_t roc, Int_t slot, Int_t channel);
  void AddEMM(Int_t roc, Int_t slot, Int_t channel);
  void AddSEU(Int_t roc, Int_t slot, Int_t channel);
  void AddTFO(Int_t roc, Int_t slot, Int_t channel);

  void AddRLF(Int_t roc, Int_t slot, Int_t channel);
  void AddHFO(Int_t roc, Int_t slot, Int_t channel);
  void AddOFO(Int_t roc, Int_t slot, Int_t channel);

  void AddFDF(Int_t roc, Int_t slot, Int_t channel);
  void AddS30(Int_t roc, Int_t slot, Int_t channel);

 
  Bool_t  CheckDataIntegrity(const UInt_t roc_id, UInt_t *buffer, UInt_t num_words);

  const MQwF1TDC GetF1TDCDecoder() const {return fF1TDCDecoder;};
  
  Double_t ReferenceSignalCorrection(Double_t raw_time, Double_t ref_time, Int_t bank_index, Int_t slot);

  void PrintErrorSummary();
  void WriteErrorSummary(Bool_t hist_flag=true);
  //  void WriteErrorSummaryToDedicatedRootFile(QwRootFile* rootfile, Bool_t hist_flag=true);
  // a stupid boost prevents this function, use a generic way to access root files internally. 
  // Tuesday, December 13 12:27:39 EST 2011, jhlee

 public:

  EQwDetectorType    fDetectorType;
  EQwRegionID        fRegion;
  TString            fSystemName;
  
  TH2F              *fError2DHist;

private:


  MQwF1TDC fF1TDCDecoder;

  Bool_t fLocalF1RawDecodeDebug;
  Bool_t fLocalF1DecodeDebug;

  Bool_t fLocalDebug;
  Bool_t fLocalF1ErrorDebug;
  
  Double_t fF1TDCOneResolutionNS;
  Double_t GetF1TDCsResolution();


  TList* GetErrorSummary();

  TString PrintNoF1TDC(Int_t roc, Int_t slot);
  TString PrintNoF1TDC(Int_t tdc_index);
  Bool_t  CheckRegisteredF1(Int_t roc, Int_t slot);
  Double_t GetF1TDCTriggerRollover(); 

  Double_t fF1TDCTriggerRollover; 

  void SetErrorHistOptions();

  // this is a temp solution to exclude
  // 31 MHz clock input in QwDriftChamber
  // in order to reduce unreasonable error number
  //, but it doesn't work with other subsystem
  // Wednesday, September 29 10:49:51 EDT 2010, jhlee

  Bool_t CheckSlot20Chan30(Int_t slot, Int_t chan);

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwF1TDContainer,1);
  #endif

};





class F1TDCReferenceSignal :  public TObject
{

 public:
  F1TDCReferenceSignal();
  F1TDCReferenceSignal(const Int_t bank_index, const Int_t slot, const Int_t channel);
  F1TDCReferenceSignal(const Int_t bank_index, const Int_t slot, const Int_t channel, const TString name);
  virtual ~F1TDCReferenceSignal() {};
  friend std::ostream& operator<<(std::ostream& os, const F1TDCReferenceSignal &f1tdcref);

  Int_t         GetSlotNumber()    const {return fSlot;};
  Int_t         GetChannelNumber() const {return fChannelNumber;};
  Int_t         GetBankIndex()     const {return fBankIndex;};
  const TString GetRefSignalName() const {return fRefSignalName;};

  const Double_t GetRefTimeAU()    const {return fRefTimeArbUnit;};



  const UInt_t   GetNoRefTimeAU()  const {return fNoRefTimeArbUnit;};


  void SetSlotNumber  (const Int_t slot)       {fSlot = slot;};
  void SetChanNumber  (const Int_t chan)       {fChannelNumber = chan;};    
  void SetBankIndex   (const Int_t bank_index) {fBankIndex = bank_index;};
  void SetRefSignalName  (const TString name)  {fRefSignalName = name;};

  Bool_t SetRefTimeAU   (const Double_t ref_time);// { if  fFirstHitFlag  = false;fRefTimeArbUnit = ref_time;};
  
  Bool_t HasFirstHit() {return fFirstHitFlag; };
  Bool_t HasRefTime(); 
  const UInt_t GetCounter() const {return fCounter;};



  void PrintCounterSummary();

  void ClearEventData() { Clear(); };

 private:

  Int_t fSlot;         // F1TDC slot number 
                       // QwAnalysis' slot range 0-20. But, the physical VME
                       // slot range is 1-21. QwAlaysis's slot range is acceptable
                       // if we carefully match them with each other as follows:
                       //  QwAnalysis slot 0, 1, 2, .... , 2
                       //  VME        slot 1, 2, 3, .... , 21
                       // Inside codes, we did the following way:
                       //  QwAnalysis slot 0, 1, 2, .... , 20
                       //     VME     slot    1, 2, 3, .... , 21
                       // It is acceptable if we don't use the VME slot 21 as
                       // we do.
                       // Friday, July 30 11:26:36 EDT 2010, jhlee

   
  Int_t    fChannelNumber;

  Int_t    fBankIndex; // This is the Bank index, which
                         // is the redefined index to access "bank=0x0901"
                         // defined in map file (e.g. qweak_R2.map)
 
  TString  fRefSignalName;

  Double_t  fRefTimeArbUnit;
  Bool_t    fFirstHitFlag;
  UInt_t    fCounter;

  TString            fSystemName;

  static const UInt_t fNoRefTimeArbUnit; 
  Bool_t    fRefTimeFlag;

  void Clear() { fRefTimeArbUnit = (Double_t) fNoRefTimeArbUnit; fFirstHitFlag = false;};

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(F1TDCReferenceSignal,1);
  #endif

};



class F1TDCReferenceContainer :  public TObject
{
  
 public:
  Int_t                    fNF1TDCReferenceSignals;
  TObjArray               *fF1TDCReferenceSignalsList; 
 
 public:
  F1TDCReferenceContainer();
  virtual ~F1TDCReferenceContainer();

  // friend std::ostream& operator<<(std::ostream& os, const QwF1TDContainer &container);

  void SetSystemName(const TString name);//{fSystemName = name;};
  const TString GetSystemName() const {return fSystemName;};

  void AddF1TDCReferenceSignal(F1TDCReferenceSignal *in);
  /* void Print(const Option_t* options = 0) const; */

  Int_t              GetSize()         const {return fNF1TDCReferenceSignals;};
  Int_t              Size()            const {return fNF1TDCReferenceSignals;};

  void PrintCounters();

  //  F1TDCReferenceSignal* GetReferenceSignal(Int_t bank_index, Int_t slot, Int_t chan);
  void SetReferenceSignal(Int_t bank_index, Int_t slot, Int_t chan, UInt_t data, Bool_t debug=false);
  

  Double_t GetReferenceTimeAU  (Int_t bank_index, TString name);
  // Double_t GetNoReferneceTimeAU(Int_t bank_index, TString name);

  void     ClearEventData();
  // we need Double_t to return a negative refererence time so that
  // we can distinguish them with "zero" reference time. 
private:

  TString            fSystemName;
  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(F1TDCReferenceContainer,1);
  #endif

};


#endif

/**********************************************************\
* File: QwPMT_Channel.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QwPMT_CHANNEL__
#define __QwPMT_CHANNEL__

#include <vector>
#include "TTree.h"

//jpan: Mersenne Twistor: A 623-diminsionally equidistributed
//uniform pseudorandom number generator
#include "TRandom3.h"

#include "VQwDataElement.h"



///
/// \ingroup QwTracking
class QwPMT_Channel: public VQwDataElement {
  /******************************************************************
   *  Class: QwPMT_Channel
   *
   ******************************************************************/
 public:
  /// Default constructor
  QwPMT_Channel() {
    // Prepare the random number generator.
    gRandom->SetSeed();
  };
  /// Copy constructor
  QwPMT_Channel(const QwPMT_Channel& source)
  : VQwDataElement(source)
  { };
  /// Constructor with name
  QwPMT_Channel(TString name) {
    InitializeChannel(name);
  };
  /// Virtual destructor
  virtual ~QwPMT_Channel() { };

  void  InitializeChannel(TString name){
    SetElementName(name);
    ClearEventData();
  };

  void  ClearEventData();
  void  RandomizeEventData(int helicity, int SlotNum, int ChanNum);
  void  EncodeEventData(std::vector<UInt_t> &TrigBuffer);
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement=0){return 0;};

  void     SetValue(Double_t data) { fValue = data; };
  Double_t GetValue() const        { return fValue; };
  void SetSubbankID(const Int_t bank_index)         { fCrate = bank_index; };
  void SetModule(const Int_t slot_num)              { fModule = slot_num; };
  Int_t           GetSubbankID()     const { return fCrate; };
  Int_t           GetModule()        const { return fModule; };

  void  ProcessEvent();

  QwPMT_Channel& operator=  (const QwPMT_Channel &value);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  void PrintValue() const;
  void PrintInfo() const;
  void PrintErrorCounters() const {};


 protected:


 private:
  static const Bool_t kDEBUG;

  /*  ADC Calibration                     */
  static const Double_t kPMT_VoltsPerBit;

  /*  Channel information data members    */


  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  /*  Event data members */
  Double_t fValue;

  Int_t fCrate;                      ///< ROC number
  Int_t fModule;                     ///< slot number
};



#endif

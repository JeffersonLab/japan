
#ifndef __QwVQWK_COMBINEDPMT__
#define __QwVQWK_COMBINEDPMT__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwVQWK_Channel.h"
#include "QwIntegrationPMT.h"

// Forward declarations
class QwBlinder;
class QwDBInterface;

class QwCombinedPMT : public VQwDataElement {
/////
 public:
  QwCombinedPMT(){
    InitializeChannel("", "derived");
  };

  QwCombinedPMT(TString name){
    InitializeChannel(name, "derived");
  };

  QwCombinedPMT(TString subsystemname, TString name){
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name, "derived");
  };
  QwCombinedPMT(const QwCombinedPMT& source)
  : VQwDataElement(source),
    fCalibration(source.fCalibration),
    fElement(source.fElement),
    fWeights(source.fWeights),
    fSumADC(source.fSumADC)
  { }
  virtual ~QwCombinedPMT() { };

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave); 
  void  LinkChannel(TString name);

  const QwVQWK_Channel* GetChannel(const TString name) const {
    if (fSumADC.GetElementName() == name) return fSumADC.GetChannel(name);
    else return 0;
  };

  void Add(QwIntegrationPMT* pmt, Double_t weight);

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);
  void  ClearEventData();

  void PrintErrorCounters();

  void CalculateSumAndAverage();

  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity);
  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  void SetSingleEventCuts(UInt_t errorflag, Double_t LL, Double_t UL, Double_t stability);

  void SetDefaultSampleSize(Int_t sample_size);
  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    fSumADC.SetEventCutMode(bcuts);
  }
  UInt_t GetEventcutErrorFlag(){//return the error flag
    return fSumADC.GetEventcutErrorFlag();
  }

  void IncrementErrorCounters(){
    fSumADC.IncrementErrorCounters();
  }

  UInt_t UpdateErrorFlag();
  void   UpdateErrorFlag(const QwCombinedPMT *ev_error);

  void PrintInfo() const;
  void PrintValue() const;

  QwCombinedPMT& operator=  (const QwCombinedPMT &value);
  QwCombinedPMT& operator+= (const QwCombinedPMT &value);
  QwCombinedPMT& operator-= (const QwCombinedPMT &value);
  void Sum(QwCombinedPMT &value1, QwCombinedPMT &value2);
  void Difference(QwCombinedPMT &value1, QwCombinedPMT &value2);
  void Ratio(QwCombinedPMT &numer, QwCombinedPMT &denom);
  void Scale(Double_t factor);
  void Normalize(VQwDataElement* denom);
  void AccumulateRunningSum(const QwCombinedPMT& value);
  void DeaccumulateRunningSum(QwCombinedPMT& value);
  void CalculateRunningAverage();

  void SetBlindability(Bool_t isblindable){fSumADC.SetBlindability(isblindable);};
  void SetNormalizability(Bool_t isnormalizable){fSumADC.SetNormalizability(isnormalizable);}; 

  /// \brief Blind the asymmetry
  void Blind(const QwBlinder *blinder);
  /// \brief Blind the difference using the yield
  void Blind(const QwBlinder *blinder, const QwCombinedPMT& yield);

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  std::vector<QwDBInterface>  GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();

 protected:

 private:

  Int_t fDataToSave;
  Double_t fCalibration;
  Double_t fULimit, fLLimit;
  Double_t fSequenceNo_Prev;

  Bool_t fGoodEvent; /// used to validate sequence number in the IsGoodEvent()

  std::vector <QwIntegrationPMT*> fElement;
  std::vector <Double_t> fWeights;

  QwIntegrationPMT  fSumADC;
  //QwIntegrationPMT  fAvgADC;

  Int_t fDevice_flag; /// sets the event cut level for the device
                      /// fDevice_flag=1 Event cuts & HW check,
                      /// fDevice_flag=0 HW check, fDevice_flag=-1 no check

  Int_t fDeviceErrorCode; /// keep the device HW status using a unique code
                          /// from the QwVQWK_Channel::fDeviceErrorCode

  Bool_t bEVENTCUTMODE; /// If this set to kFALSE then Event cuts do not depend
                        /// on HW ckecks. This is set externally through the
                        /// qweak_beamline_eventcuts.map

  const static  Bool_t bDEBUG=kFALSE; /// debugging display purposes

};

#endif

/**********************************************************\
* File: QwIntegratedRasterChannel.h                       *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwIntegratedRasterChannel__
#define __QwIntegratedRasterChannel__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwParameterFile.h"
#include "VQwDataElement.h"
#include "VQwHardwareChannel.h"

// Forward declarations
class QwDBInterface;

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
template<typename T>
class QwIntegratedRasterChannel : public VQwDataElement{
/////
 public:
  QwIntegratedRasterChannel() { };
  QwIntegratedRasterChannel(TString name){
    InitializeChannel(name,"raw");
  };
  QwIntegratedRasterChannel(TString subsystemname, TString name){
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name,"raw");
  };
  virtual ~QwIntegratedRasterChannel() { };

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave);
  void  ClearEventData();


  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity = 0, double time = 0);
  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  UseExternalRandomVariable();
  void  SetExternalRandomVariable(Double_t random_variable);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  void IncrementErrorCounters(){fTriumf_ADC.IncrementErrorCounters();};
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  UInt_t GetEventcutErrorFlag(){//return the error flag
    return fTriumf_ADC.GetEventcutErrorFlag();
  }
  UInt_t UpdateErrorFlag() {return GetEventcutErrorFlag();};
  void UpdateErrorFlag(const QwIntegratedRasterChannel *ev_error){
    return fTriumf_ADC.UpdateErrorFlag(ev_error->fTriumf_ADC);
  }

  Int_t SetSingleEventCuts(Double_t mean = 0, Double_t sigma = 0);//two limts and sample size
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  void SetSingleEventCuts(UInt_t errorflag, Double_t min = 0, Double_t max = 0, Double_t stability = 0);

  void SetDefaultSampleSize(Int_t sample_size);
  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    fTriumf_ADC.SetEventCutMode(bcuts);
  }

  void PrintValue() const;
  void PrintInfo() const;

  
  QwIntegratedRasterChannel& operator=  (const QwIntegratedRasterChannel &value);
  QwIntegratedRasterChannel& operator+= (const QwIntegratedRasterChannel &value);
  QwIntegratedRasterChannel& operator-= (const QwIntegratedRasterChannel &value);
  void Sum(QwIntegratedRasterChannel &value1, QwIntegratedRasterChannel &value2);
  void Difference(QwIntegratedRasterChannel &value1, QwIntegratedRasterChannel &value2);
  void Ratio(QwIntegratedRasterChannel &numer, QwIntegratedRasterChannel &denom);
  void Scale(Double_t factor);

  void AccumulateRunningSum(const QwIntegratedRasterChannel& value);
  void CalculateRunningAverage();

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  std::vector<QwDBInterface>    GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();

  const VQwDataElement* GetElement() const {
    return const_cast<QwIntegratedRasterChannel*>(this)->GetElement();
  };


/////
 protected:

/////
 private:

  Double_t fPedestal;
  Double_t fCalibration;
  Double_t fULimit, fLLimit;
  Bool_t fGoodEvent;//used to validate sequence number in the IsGoodEvent()




  T fTriumf_ADC;

  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode

  const static  Bool_t bDEBUG=kFALSE;//debugging display purposes
  Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts do not depend on HW ckecks. This is set externally through the qweak_beamline_eventcuts.map


};

#endif

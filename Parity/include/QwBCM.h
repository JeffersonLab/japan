/**********************************************************\
* File: QwBCM.h                                            *
*                                                          *
* Author:                                                  *
* Time-stamp:                                              *
\**********************************************************/

#ifndef __QWBCM__
#define __QWBCM__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

#include "QwParameterFile.h"
#include "VQwDataElement.h"
#include "VQwHardwareChannel.h"
#include "VQwBCM.h"

// Forward declarations
class QwDBInterface;
class QwErrDBInterface;

template<typename T> class QwCombinedBCM;

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
template<typename T> class QwBCM : public VQwBCM {
/////
  friend class QwCombinedBCM<T>;
 public:
  QwBCM(): VQwBCM(fBeamCurrent) { };
  QwBCM(TString name): VQwBCM(fBeamCurrent,name) {
    InitializeChannel(name,"raw");
  };
  QwBCM(TString subsystemname, TString name)
  : VQwBCM(fBeamCurrent,name) {
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname,name,"raw");
  };
  QwBCM(TString subsystemname, TString name, TString type, TString clock = "")
  : VQwBCM(fBeamCurrent,name) {
    fBeamCurrent.SetExternalClockName(clock.Data());
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname,name,type,"raw");
  };
  QwBCM(const QwBCM& source)
  : VQwBCM(source),
    fBeamCurrent(source.fBeamCurrent)
  { }
  virtual ~QwBCM() { };

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave);
  void  InitializeChannel(TString subsystem, TString name, TString type,
      TString datatosave);
  void  ClearEventData();

  void LoadChannelParameters(QwParameterFile &paramfile){
    fBeamCurrent.LoadChannelParameters(paramfile);
  };

  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);

  void  SetResolution(Double_t resolution){
    fResolution = resolution;
  }

  void  ApplyResolutionSmearing();
  void  FillRawEventData();


//-----------------------------------------------------------------------------------------------
  void  RandomizeEventData(int helicity = 0, double time = 0);
  void  LoadMockDataParameters(QwParameterFile &paramfile);
//-----------------------------------------------------------------------------------------------

  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  UseExternalRandomVariable();
  void  SetExternalRandomVariable(Double_t random_variable);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  void IncrementErrorCounters();
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  UInt_t GetEventcutErrorFlag(){//return the error flag
    return fBeamCurrent.GetEventcutErrorFlag();
  }

  void UpdateErrorFlag(const VQwBCM *ev_error);

  UInt_t GetErrorCode() const {return (fBeamCurrent.GetErrorCode());}; 


  Int_t SetSingleEventCuts(Double_t mean = 0, Double_t sigma = 0);//two limts and sample size
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  void SetSingleEventCuts(UInt_t errorflag, Double_t min = 0, Double_t max = 0, Double_t stability = 0, Double_t burplevel = 0);

  void SetDefaultSampleSize(Int_t sample_size);
  void SetEventCutMode(Int_t bcuts) {
    fBeamCurrent.SetEventCutMode(bcuts);
  }

  void PrintValue() const;
  void PrintInfo() const;

protected:
  VQwHardwareChannel* GetCharge() {
    return &fBeamCurrent;
  };
public:

  // These are for the clocks
  std::string GetExternalClockName() { return fBeamCurrent.GetExternalClockName(); };
  Bool_t NeedsExternalClock() { return fBeamCurrent.NeedsExternalClock(); };
  void SetExternalClockPtr( const VQwHardwareChannel* clock) {fBeamCurrent.SetExternalClockPtr(clock);};
  void SetExternalClockName( const std::string name) { fBeamCurrent.SetExternalClockName(name);};
  Double_t GetNormClockValue() { return fBeamCurrent.GetNormClockValue();}

  // Implementation of Parent class's virtual operators
  VQwBCM& operator=  (const VQwBCM &value);
  VQwBCM& operator+= (const VQwBCM &value);
  VQwBCM& operator-= (const VQwBCM &value);

  // This is used only by a QwComboBCM. It is placed here since in QwBeamLine we do
  // not readily have the appropriate template every time we want to use this
  // function.
  virtual void SetBCMForCombo(VQwBCM* bcm, Double_t weight, Double_t sumqw ) {
    std::cerr<<"SetBCMForCombo for QwCombinedBCM<T> not defined!!\n";
  };

  // This class specific operators
  QwBCM& operator=  (const QwBCM &value);
  QwBCM& operator+= (const QwBCM &value);
  QwBCM& operator-= (const QwBCM &value);
  void Ratio(const VQwBCM &numer, const VQwBCM &denom);
  void Ratio(const QwBCM &numer, const QwBCM &denom);
  void Scale(Double_t factor);

  void AccumulateRunningSum(const VQwBCM&, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void DeaccumulateRunningSum(VQwBCM& value, Int_t ErrorMask=0xFFFFFFF);
  void CalculateRunningAverage();

  Bool_t CheckForBurpFail(const VQwDataElement *ev_error);

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  std::vector<QwDBInterface> GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();

  Double_t GetValue();
  Double_t GetValueError();
  Double_t GetValueWidth();


/////
 protected:
  T fBeamCurrent;

/////
 private:
 Double_t fResolution;

};


#endif // __QWBCM__

/********************************************************\
* File: QwClock.h                                        *
*                                                        *
* Author: Juan Carlos Cornejo <cornejo@jlab.org>         *
* Time-stamp: <2011-06-16>                               *
\********************************************************/

#ifndef __QWCLOCK__
#define __QWCLOCK__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

#include "QwParameterFile.h"
#include "VQwDataElement.h"
#include "VQwHardwareChannel.h"
#include "VQwClock.h"

// Forward declarations
#ifdef __USE_DATABASE__
class QwDBInterface;
#endif // __USE_DATABASE__

/*****************************************************************
*  Class:
*  \brief QwClock is a standard clock with the calibration factor
*  representing the frequency of this clock.
*
*  Other channels may use this clock to normalize their units with
*  time. Initially designed for Scalers, but could be used by
*  other hardware elements.
******************************************************************/
template<typename T>
class QwClock : public VQwClock {
/////
 public:
  QwClock() { };
  QwClock(TString subsystemname, TString name, TString type = ""){
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name, "raw", type);
  };
  QwClock(const QwClock& source)
  : VQwClock(source),
    fPedestal(source.fPedestal),fCalibration(source.fCalibration),
    fULimit(source.fULimit),fLLimit(source.fLLimit),
    fClock(source.fClock),
    fNormalizationValue(source.fNormalizationValue)
  { }
  virtual ~QwClock() { };

  void LoadChannelParameters(QwParameterFile &paramfile){
    fClock.LoadChannelParameters(paramfile);
  };

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString subsystem, TString name, TString datatosave, TString type = "");
  void  ClearEventData();


  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  void IncrementErrorCounters(){fClock.IncrementErrorCounters();}

  Bool_t CheckForBurpFail(const QwClock *ev_error){
    return fClock.CheckForBurpFail(&(ev_error->fClock));
  }


  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  UInt_t GetEventcutErrorFlag(){//return the error flag
    return fClock.GetEventcutErrorFlag();
  }
  UInt_t UpdateErrorFlag() {return GetEventcutErrorFlag();};
  void UpdateErrorFlag(const QwClock *ev_error){
    fClock.UpdateErrorFlag(ev_error->fClock);
  }

  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  void SetSingleEventCuts(UInt_t errorflag, Double_t min = 0, Double_t max = 0, Double_t stability = 0, Double_t burplevel = 0);

  void SetDefaultSampleSize(Int_t sample_size);
  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    fClock.SetEventCutMode(bcuts);
  }

  void PrintValue() const;
  void PrintInfo() const;

  // Implementation of Parent class's virtual operators
  VQwClock& operator=  (const VQwClock &value);
  VQwClock& operator+= (const VQwClock &value);
  VQwClock& operator-= (const VQwClock &value);

  // This class specific operators
  QwClock& operator=  (const QwClock &value);
  QwClock& operator+= (const QwClock &value);
  QwClock& operator-= (const QwClock &value);
  void Ratio(const VQwClock &numer, const VQwClock &denom);
  void Ratio(const QwClock &numer, const QwClock &denom);
  void Scale(Double_t factor);

  void AccumulateRunningSum(const VQwClock& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void DeaccumulateRunningSum(VQwClock& value, Int_t ErrorMask=0xFFFFFFF);
  void CalculateRunningAverage();

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void  FillTreeVector(std::vector<Double_t> &values) const;

#ifdef __USE_DATABASE__
  std::vector<QwDBInterface> GetDBEntry();
#endif // __USE_DATABASE__

  // These are related to those hardware channels that need to normalize
  // to an external clock
  Double_t GetNormClockValue() { return fNormalizationValue;};
  Double_t GetStandardClockValue() { return fCalibration; };

  const VQwHardwareChannel* GetTime() const {
    return &fClock;
  };


/////
 protected:

/////
 private:
  Double_t fPedestal;
  Double_t fCalibration;
  Double_t fULimit, fLLimit;

  T fClock;

  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode

  const static  Bool_t bDEBUG=kFALSE;//debugging display purposes
  Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts do not depend on HW ckecks. This is set externally through the qweak_beamline_eventcuts.map

  Double_t fNormalizationValue;

};

#endif // __QWCLOCK__

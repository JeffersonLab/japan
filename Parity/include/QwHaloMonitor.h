/**********************************************************\
* File: QwHaloMonitor.h                                   *
*                                                         *
* Author:B. Waidyawansa                                   *
* Time-stamp:24-june-2010                                 *
\**********************************************************/

#ifndef __QwHALO_MONITOR__
#define __QwHALO_MONITOR__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwParameterFile.h"
#include "QwScaler_Channel.h"
#include "VQwHardwareChannel.h"

// Forward declarations
#ifdef __USE_DATABASE__
class QwDBInterface;
#endif // __USE_DATABASE__

/*****************************************************************
*  Class: QwHaloMonitor handles the halo counters. This use
          QwSIS3801_Channel scaler channels.
******************************************************************/
///
/// \ingroup QwAnalysis_BL
class  QwHaloMonitor : public VQwDataElement{
/////

 public:
  QwHaloMonitor() { };
  QwHaloMonitor(TString name){
    InitializeChannel(name);
  };
  QwHaloMonitor(TString subsystemname, TString name){
    InitializeChannel(subsystemname, name);
  };
  QwHaloMonitor(const QwHaloMonitor& source)
  : VQwDataElement(source),
    fHalo_Counter(source.fHalo_Counter)
  { }
  virtual ~QwHaloMonitor() { };

  void  InitializeChannel(TString name);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name);
  void  ClearEventData();

  void LoadChannelParameters(QwParameterFile &paramfile){
    fHalo_Counter.LoadChannelParameters(paramfile);
  }
  std::string GetExternalClockName() { return fHalo_Counter.GetExternalClockName(); };
  Bool_t NeedsExternalClock() { return fHalo_Counter.NeedsExternalClock(); };
  void SetExternalClockPtr( const VQwHardwareChannel* clock) {fHalo_Counter.SetExternalClockPtr(clock);};
  void SetExternalClockName( const std::string name) { fHalo_Counter.SetExternalClockName(name);};
  Double_t GetNormClockValue() { return fHalo_Counter.GetNormClockValue();}



  void  PrintErrorCounters();//This will display the error summary for each device
  void  UpdateHWErrorCount();//Update error counter for HW faliure

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,UInt_t index=0);
  void  ProcessEvent();

  QwHaloMonitor& operator=  (const QwHaloMonitor &value);
  QwHaloMonitor& operator+= (const QwHaloMonitor &value);
  QwHaloMonitor& operator-= (const QwHaloMonitor &value);
  void Sum(QwHaloMonitor &value1, QwHaloMonitor &value2);
  void Difference(QwHaloMonitor &value1, QwHaloMonitor &value2);
  void Ratio(QwHaloMonitor &numer, QwHaloMonitor &denom);
  void Offset(Double_t Offset);
  void Scale(Double_t Offset);

  void     SetPedestal(Double_t ped) { fHalo_Counter.SetPedestal(ped); };
  void     SetCalibrationFactor(Double_t factor) { fHalo_Counter.SetCalibrationFactor(factor); };
  void AccumulateRunningSum(const QwHaloMonitor& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void DeaccumulateRunningSum(QwHaloMonitor& value, Int_t ErrorMask=0xFFFFFFF);
  void CalculateRunningAverage();

  Bool_t ApplySingleEventCuts();//check values read from modules are at desired level
  void IncrementErrorCounters(){fHalo_Counter.IncrementErrorCounters();};
  UInt_t GetEventcutErrorFlag(){return fHalo_Counter.GetEventcutErrorFlag();};

  Bool_t CheckForBurpFail(const VQwDataElement *ev_error);

  UInt_t UpdateErrorFlag() {return GetEventcutErrorFlag();};
  void UpdateErrorFlag(const QwHaloMonitor *ev_error){
    fHalo_Counter.UpdateErrorFlag(ev_error->fHalo_Counter);
  };

  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  Bool_t ApplyHWChecks();
  void SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability, Double_t burplevel){
    QwError<<"***************************"<<QwLog::endl;
    fHalo_Counter.SetSingleEventCuts(errorflag,min,max,stability,burplevel);
  };
  void SetEventCutMode(Int_t bcuts){
    fHalo_Counter.SetEventCutMode(bcuts);
  }


  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  VQwHardwareChannel* GetScaler(){
    return &fHalo_Counter;
  };

  const VQwHardwareChannel* GetScaler() const{
    return const_cast<QwHaloMonitor*>(this)->GetScaler();
  };

  Double_t GetValue() {
    return fHalo_Counter.GetValue();
  }

  void  PrintValue() const;
  void  PrintInfo() const;

#ifdef __USE_DATABASE__
  std::vector<QwDBInterface> GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();
#endif // __USE_DATABASE__

 protected:

 private:
  static const Bool_t kDEBUG;

  QwSIS3801D24_Channel fHalo_Counter;

  Int_t  fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode
  Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts do not depend on HW ckecks. This is set externally through the qweak_beamline_eventcuts.map

};


#endif

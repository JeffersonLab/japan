/**********************************************************\
* File: QwBPMStripline.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwBPMSTRIPLINE__
#define __QwBPMSTRIPLINE__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "VQwBPM.h"
#include "QwParameterFile.h"
#include "QwUtil.h"

// Forward declarations
#ifdef __USE_DATABASE__
class QwDBInterface;
class QwErrDBInterface;
#endif // __USE_DATABASE__
class QwPromptSummary;

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL

template<typename T>
class QwBPMStripline : public VQwBPM {
  template <typename TT> friend class QwCombinedBPM;
  friend class QwEnergyCalculator;

 public:
  static UInt_t  GetSubElementIndex(TString subname);

 public:
  QwBPMStripline() { };
  QwBPMStripline(TString name) {
    InitializeChannel(name);
    fRotationAngle = 45.0;
    SetRotation(fRotationAngle);
    bRotated=kTRUE;
  };
  QwBPMStripline(TString subsystemname, TString name) {
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name);
    fRotationAngle = 45.0;
    SetRotation(fRotationAngle);
    bRotated=kTRUE;
  };
  QwBPMStripline(TString subsystemname, TString name, TString type) {
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name, type);
    fRotationAngle = 45.0;
    SetRotation(fRotationAngle);
    bRotated=kTRUE;
  };
  QwBPMStripline(const QwBPMStripline& source)
  : VQwBPM(source),
    fEffectiveCharge(source.fEffectiveCharge),fEllipticity(source.fEllipticity)
  {
    QwCopyArray(source.fWire, fWire);
    QwCopyArray(source.fRelPos, fRelPos);
    QwCopyArray(source.fAbsPos, fAbsPos);
  }
  virtual ~QwBPMStripline() { };

  void    InitializeChannel(TString name);
  // new routine added to update necessary information for tree trimming
  void    InitializeChannel(TString subsystem, TString name);
  void    InitializeChannel(TString subsystem, TString name, TString type);
  void    ClearEventData();

  void    SetMockDataAsDiff();

  void LoadChannelParameters(QwParameterFile &paramfile){
    for(Short_t i=0;i<4;i++)
      fWire[i].LoadChannelParameters(paramfile);
  }

  Int_t   ProcessEvBuffer(UInt_t* buffer,
			UInt_t word_position_in_buffer,UInt_t indexnumber);
  void    ProcessEvent();
  void    PrintValue() const;
  void    PrintInfo() const;
  void    WritePromptSummary(QwPromptSummary *ps, TString type);


  const VQwHardwareChannel* GetPosition(EBeamPositionMonitorAxis axis) const {
    if (axis<0 || axis>2){
      TString loc="QwBPMStripline::GetPosition for "
        +this->GetElementName()+" failed for axis value "+Form("%d",axis);
      throw std::out_of_range(loc.Data());
    }
    return &fAbsPos[axis];
  }
  const VQwHardwareChannel* GetEffectiveCharge() const {return &fEffectiveCharge;}
  const VQwHardwareChannel* GetEllipticity() const {return &fEllipticity;}

  TString GetSubElementName(Int_t subindex);
  void    GetAbsolutePosition();

  Bool_t  ApplyHWChecks();//Check for harware errors in the devices
  Bool_t  ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  //void    SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX);
  /*   /\*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel *\/ */
  void    SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t min, Double_t max, Double_t stability, Double_t burplevel);
  //void    SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t min, Double_t max, Double_t stability, Double_t burplevel){return;};
  void    SetEventCutMode(Int_t bcuts);
  void    IncrementErrorCounters();
  void    PrintErrorCounters() const;   // report number of events failed due to HW and event cut faliure
  UInt_t  GetEventcutErrorFlag();
  UInt_t  UpdateErrorFlag();

  Bool_t  CheckForBurpFail(const VQwDataElement *ev_error);
  void    UpdateErrorFlag(const VQwBPM *ev_error);


  void    SetDefaultSampleSize(Int_t sample_size);
  void    SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY);
  void    RandomizeEventData(int helicity = 0, double time = 0.0);
  void    LoadMockDataParameters(QwParameterFile &paramfile);
  void    ApplyResolutionSmearing();
  void    ApplyResolutionSmearing(EBeamPositionMonitorAxis iaxis);
  void    FillRawEventData();
  void    EncodeEventData(std::vector<UInt_t> &buffer);
  void    SetSubElementPedestal(Int_t j, Double_t value);
  void    SetSubElementCalibrationFactor(Int_t j, Double_t value);

  void    Ratio(VQwBPM &numer, VQwBPM &denom);
  void    Ratio(QwBPMStripline &numer, QwBPMStripline &denom);
  void    Scale(Double_t factor);

  VQwBPM& operator=  (const VQwBPM &value);
  VQwBPM& operator+= (const VQwBPM &value);
  VQwBPM& operator-= (const VQwBPM &value);

  virtual QwBPMStripline& operator=  (const QwBPMStripline &value);
  virtual QwBPMStripline& operator+= (const QwBPMStripline &value);
  virtual QwBPMStripline& operator-= (const QwBPMStripline &value);

  void    AccumulateRunningSum(const QwBPMStripline& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void    AccumulateRunningSum(const VQwBPM& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void    DeaccumulateRunningSum(VQwBPM& value, Int_t ErrorMask=0xFFFFFFF);
  void    DeaccumulateRunningSum(QwBPMStripline& value, Int_t ErrorMask=0xFFFFFFF);

  void    CalculateRunningAverage();

  void    ConstructHistograms(TDirectory *folder, TString &prefix);
  void    FillHistograms();

  void    ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void    ConstructBranch(TTree *tree, TString &prefix);
  void    ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void    FillTreeVector(std::vector<Double_t> &values) const;


#ifdef __USE_DATABASE__
  std::vector<QwDBInterface> GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();
#endif // __USE_DATABASE__

 protected:
  ///  Should be used inside the QwCombinedBPM::GetProjectedPosition function to assign the
  ///  BPM's X and Y values based on the slope and intercept of the combinedBPM.
  VQwHardwareChannel* GetPosition(EBeamPositionMonitorAxis axis) {
    if (axis<0 || axis>2){
      TString loc="QwBPMStripline::GetPosition for "
        +this->GetElementName()+" failed for axis value "+Form("%d",axis);
      throw std::out_of_range(loc.Data());
    }
    return &fAbsPos[axis];
  }

  VQwHardwareChannel* GetSubelementByName(TString ch_name);


  /////
 private:
  /* /\*  Position calibration factor, transform ADC counts in mm *\/ */
  /* static Double_t kQwStriplineCalibration; */
  /* static Double_t fRelativeGains[2]; */
  /* Rotation factor for the BPM which antenna are at 45 deg */
  static const Double_t kRotationCorrection;
  static const TString subelement[4];



 protected:
  T fWire[4];
  T fRelPos[2];

  //  These are the "real" data elements, to which the base class
  //  fAbsPos_base and fEffectiveCharge_base are pointers.
  T fAbsPos[2];
  T fEffectiveCharge;
  T fEllipticity;

private:
  // Functions to be removed
  void    SetEventData(Double_t* block, UInt_t sequencenumber);
  std::vector<T> fBPMElementList;
  void    MakeBPMList();




};


#endif

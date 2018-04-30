/**********************************************************\
* File: QwBPMStripline.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwVQWK_CAVITY__
#define __QwVQWK_CAVITY__

// System headers
#include <vector>

// ROOT headres
#include <TTree.h>

// Qweak headers
#include "QwVQWK_Channel.h"
#include "VQwBPM.h"
#include "QwParameterFile.h"

// Forward declarations
class QwDBInterface;
class QwErrDBInterface;

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL

class QwBPMCavity : public VQwBPM {
  template <typename TT> friend class QwCombinedBPM;
  friend class QwEnergyCalculator;

 public:
  static UInt_t  GetSubElementIndex(TString subname);

 public:
  QwBPMCavity() { };
  QwBPMCavity(TString name):VQwBPM(name){
    InitializeChannel(name);
    bRotated=kTRUE;
  };
  QwBPMCavity(TString subsystemname, TString name)
  : VQwBPM(name) {
	  SetSubsystemName(subsystemname);
	  InitializeChannel(subsystemname, name);
	  bRotated=kTRUE;
  };
  QwBPMCavity(const QwBPMCavity& source)
  : VQwBPM(source),
    fWire(source.fWire),fRelPos(source.fRelPos),fAbsPos(source.fAbsPos),
    fEffectiveCharge(source.fEffectiveCharge)
  { }
  virtual ~QwBPMCavity() { };

  void    InitializeChannel(TString name);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name);
  void    ClearEventData();

  void LoadChannelParameters(QwParameterFile &paramfile){
    for(Short_t i=0;i<2;i++)
      fWire[i].LoadChannelParameters(paramfile);
  }

  Int_t   ProcessEvBuffer(UInt_t* buffer,
			UInt_t word_position_in_buffer,UInt_t indexnumber);
  void    ProcessEvent();
  void    PrintValue() const;
  void    PrintInfo() const;

  const VQwHardwareChannel* GetPosition(EBeamPositionMonitorAxis axis) const {
    if (axis<0 || axis>2){
      TString loc="QwBPMCavity::GetPosition for "
        +this->GetElementName()+" failed for axis value "+Form("%d",axis);
      throw std::out_of_range(loc.Data());
    }
    return &fAbsPos[axis];
  }
  const VQwHardwareChannel* GetEffectiveCharge() const {return &fEffectiveCharge;}

  TString GetSubElementName(Int_t subindex);
  void    GetAbsolutePosition();

  Bool_t  ApplyHWChecks();//Check for harware errors in the devices
  Bool_t  ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  //void    SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX);
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  //void    SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t min, Double_t max, Double_t stability);
  void    SetEventCutMode(Int_t bcuts);
  void IncrementErrorCounters();
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  UInt_t  GetEventcutErrorFlag();
  UInt_t  UpdateErrorFlag();
  void UpdateErrorFlag(const VQwBPM *ev_error);

  void    SetDefaultSampleSize(Int_t sample_size);
  void    SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY);
  void    RandomizeEventData(int helicity = 0, double time = 0.0);
  void    SetEventData(Double_t* block, UInt_t sequencenumber);
  void    EncodeEventData(std::vector<UInt_t> &buffer);
  void    SetSubElementPedestal(Int_t j, Double_t value);
  void    SetSubElementCalibrationFactor(Int_t j, Double_t value);

  void    Ratio(QwBPMCavity &numer, QwBPMCavity &denom);
  void    Scale(Double_t factor);

  VQwBPM& operator=  (const VQwBPM &value);
  VQwBPM& operator+= (const VQwBPM &value);
  VQwBPM& operator-= (const VQwBPM &value);

  virtual QwBPMCavity& operator=  (const QwBPMCavity &value);
  virtual QwBPMCavity& operator+= (const QwBPMCavity &value);
  virtual QwBPMCavity& operator-= (const QwBPMCavity &value);

  void    AccumulateRunningSum(const VQwBPM &value);
  void    AccumulateRunningSum(const QwBPMCavity &value);
  void    DeaccumulateRunningSum(VQwBPM &value);
  void    DeaccumulateRunningSum(QwBPMCavity &value);
  void    CalculateRunningAverage();

  void    ConstructHistograms(TDirectory *folder, TString &prefix);
  void    FillHistograms();

  void    ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void    FillTreeVector(std::vector<Double_t> &values) const;
  void    ConstructBranch(TTree *tree, TString &prefix);
  void    ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);


  std::vector<QwDBInterface> GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();

  protected:
  VQwHardwareChannel* GetSubelementByName(TString ch_name);

  /////
 private:
  /*  Position calibration factor, transform ADC counts in mm */
  static const Double_t kQwCavityCalibration;
  /* Rotation factor for the BPM which antenna are at 45 deg */
  static const Double_t kRotationCorrection;
  static const TString subelement[3];



 protected:
  Bool_t   bRotated;
  QwVQWK_Channel fWire[2];
  QwVQWK_Channel fRelPos[2];

  //  These are the "real" data elements, to which the base class
  //  fAbsPos_base and fEffectiveCharge_base are pointers.
  QwVQWK_Channel fAbsPos[2];
  QwVQWK_Channel fEffectiveCharge;


 private:
  // Functions to be removed
  void    MakeBPMCavityList();
  std::vector<QwVQWK_Channel> fBPMElementList;

};


#endif

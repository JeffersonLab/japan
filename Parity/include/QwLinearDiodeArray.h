/**********************************************************\
* File: QwLinearDiodeArray.h                              *
*                                                         *
* Author: B.Waidyawansa                                   *
* Time-stamp: 09-14-2010                                  *
\**********************************************************/

#ifndef __QwLinearDiodeArray__
#define __QwLinearDiodeArray__

// System headers
#include <vector>

// ROOT headers
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

class QwLinearDiodeArray : public VQwBPM {

 public:
  static UInt_t  GetSubElementIndex(TString subname);

  QwLinearDiodeArray() {
  };
  QwLinearDiodeArray(TString name):VQwBPM(name){
  };
  QwLinearDiodeArray(TString subsystemname, TString name):VQwBPM(name){
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name);
  };    
  QwLinearDiodeArray(const QwLinearDiodeArray& source)
  : VQwBPM(source),
    fEffectiveCharge(source.fEffectiveCharge)
  {
    for (size_t i = 0; i < 2; i++) {
      fRelPos[i] = source.fRelPos[i];
      fAbsPos[i] = source.fAbsPos[i];
    }
    for (size_t i = 0; i < 8; i++) {
      fPhotodiode[i] = source.fPhotodiode[i];
    }
  }
  virtual ~QwLinearDiodeArray() { };
  
  void    InitializeChannel(TString name);
  // new routine added to update necessary information for tree trimming
  void    InitializeChannel(TString subsystem, TString name);
  void    ClearEventData();

  void LoadChannelParameters(QwParameterFile &paramfile){
    for(size_t i=0;i<kMaxElements;i++)
      fPhotodiode[i].LoadChannelParameters(paramfile);
  }

  Int_t   ProcessEvBuffer(UInt_t* buffer,
			UInt_t word_position_in_buffer,UInt_t indexnumber);
  void    ProcessEvent();
  void    PrintValue() const;
  void    PrintInfo() const;

  const VQwHardwareChannel* GetPosition(EBeamPositionMonitorAxis axis) const {
    if (axis<0 || axis>2){
      TString loc="QwLinearDiodeArray::GetPosition for "
        +this->GetElementName()+" failed for axis value "+Form("%d",axis);
      throw std::out_of_range(loc.Data());
    }
    return &fAbsPos[axis];
  }
  const VQwHardwareChannel* GetEffectiveCharge() const {return &fEffectiveCharge;}

  TString GetSubElementName(Int_t subindex);
  UInt_t  SetSubElementName(TString subname);
  void    GetAbsolutePosition();

  Bool_t  ApplyHWChecks();//Check for harware errors in the devices
  Bool_t  ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  //void    SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX);
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  //void    SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t min, Double_t max, Double_t stability);
  void    SetEventCutMode(Int_t bcuts);
  void IncrementErrorCounters();
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  UInt_t GetEventcutErrorFlag();
  UInt_t UpdateErrorFlag();
  void UpdateErrorFlag(const VQwBPM *ev_error);

  void    SetDefaultSampleSize(Int_t sample_size);
  void    SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY);
  void    RandomizeEventData(int helicity = 0, double time = 0.0);
  void    SetEventData(Double_t* block, UInt_t sequencenumber);
  void    EncodeEventData(std::vector<UInt_t> &buffer);
  void    SetSubElementPedestal(Int_t j, Double_t value);
  void    SetSubElementCalibrationFactor(Int_t j, Double_t value);

  void    Ratio(QwLinearDiodeArray &numer, QwLinearDiodeArray &denom);
  void    Scale(Double_t factor);


  VQwBPM& operator=  (const VQwBPM &value);
  VQwBPM& operator+= (const VQwBPM &value);
  VQwBPM& operator-= (const VQwBPM &value);

  virtual QwLinearDiodeArray& operator=  (const QwLinearDiodeArray &value);
  virtual QwLinearDiodeArray& operator+= (const QwLinearDiodeArray &value);
  virtual QwLinearDiodeArray& operator-= (const QwLinearDiodeArray &value);

  void    AccumulateRunningSum(const QwLinearDiodeArray& value);
  void    AccumulateRunningSum(const VQwBPM& value);
  void    DeaccumulateRunningSum(QwLinearDiodeArray& value);
  void    DeaccumulateRunningSum(VQwBPM& value);
  void    CalculateRunningAverage();

  void    ConstructHistograms(TDirectory *folder, TString &prefix);
  void    FillHistograms();

  void    ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void    ConstructBranch(TTree *tree, TString &prefix);
  void    ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void    FillTreeVector(std::vector<Double_t> &values) const;



  std::vector<QwDBInterface> GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();

  void    MakeLinearArrayList();

  protected:
  VQwHardwareChannel* GetSubelementByName(TString ch_name);

  /////
 private:
  static const size_t kMaxElements;
  static const TString subelement[8]; 

  /*  Position calibration factor, transform ADC counts in mm */
  static const Double_t kQwLinearDiodeArrayPadSize;



 protected:
  // std::vector<QwVQWK_Channel> fPhotodiode;
  QwVQWK_Channel fPhotodiode[8];
  QwVQWK_Channel fRelPos[2];

  //  These are the "real" data elements, to which the base class
  //  fAbsPos_base and fEffectiveCharge_base are pointers.
  QwVQWK_Channel fAbsPos[2];
  QwVQWK_Channel fEffectiveCharge;

  std::vector<QwVQWK_Channel> fLinearArrayElementList;

};


#endif

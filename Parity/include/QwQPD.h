/**********************************************************\
* File: QwQPD.h                                           *
*                                                         *
* Author: B.Waidyawansa                                   *
* Time-stamp: 09-14-2010                                  *
\**********************************************************/

#ifndef __QwQPD__
#define __QwQPD__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwVQWK_Channel.h"
#include "VQwBPM.h"
#include "QwParameterFile.h"
#include "QwUtil.h"

// Forward declarations
#ifdef __USE_DATABASE__
class QwDBInterface;
class QwErrDBInterface;
#endif // __USE_DATABASE__

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL

class QwQPD : public VQwBPM {

 public:
  static UInt_t  GetSubElementIndex(TString subname);

  QwQPD() {
  };
  QwQPD(TString name):VQwBPM(name){
    InitializeChannel(name);
  };
  QwQPD(TString subsystemname, TString name):VQwBPM(name){
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name);
    fQwQPDCalibration[0] = 1.0;
    fQwQPDCalibration[1] = 1.0;
  };
  QwQPD(const QwQPD& source)
  : VQwBPM(source),
    fEffectiveCharge(source.fEffectiveCharge)
  {
    QwCopyArray(source.fPhotodiode, fPhotodiode);
    QwCopyArray(source.fRelPos, fRelPos);
    QwCopyArray(source.fAbsPos, fAbsPos);
  }
  virtual ~QwQPD() { };

  void    InitializeChannel(TString name);
  // new routine added to update necessary information for tree trimming
  void    InitializeChannel(TString subsystem, TString name);

  void LoadChannelParameters(QwParameterFile &paramfile){
    for(Short_t i=0;i<4;i++)
      fPhotodiode[i].LoadChannelParameters(paramfile);
  }

  void    GetCalibrationFactors(Double_t AlphaX, Double_t AlphaY);

  void    ClearEventData();
  Int_t   ProcessEvBuffer(UInt_t* buffer,
			UInt_t word_position_in_buffer,UInt_t indexnumber);
  void    ProcessEvent();

  const VQwHardwareChannel* GetPosition(EBeamPositionMonitorAxis axis) const {
    if (axis<0 || axis>2){
      TString loc="QwQPD::GetPosition for "
        +this->GetElementName()+" failed for axis value "+Form("%d",axis);
      throw std::out_of_range(loc.Data());
    }
    return &fAbsPos[axis];
  }
  const VQwHardwareChannel* GetEffectiveCharge() const {return &fEffectiveCharge;}

  TString GetSubElementName(Int_t subindex);
  void    GetAbsolutePosition(){};

  Bool_t  ApplyHWChecks();//Check for harware errors in the devices
  Bool_t  ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  //void    SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX);
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  void    SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t min, Double_t max, Double_t stability, Double_t burplevel);
  void    SetEventCutMode(Int_t bcuts);
  void IncrementErrorCounters();
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  UInt_t  GetEventcutErrorFlag();
  UInt_t  UpdateErrorFlag();
  void UpdateErrorFlag(const VQwBPM *ev_error);

  Bool_t  CheckForBurpFail(const VQwDataElement *ev_error);

  void    SetDefaultSampleSize(Int_t sample_size);
  void    SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY);
  void    RandomizeEventData(int helicity = 0, double time = 0.0);
  void    SetEventData(Double_t* block, UInt_t sequencenumber);
  void    EncodeEventData(std::vector<UInt_t> &buffer);
  void    SetSubElementPedestal(Int_t j, Double_t value);
  void    SetSubElementCalibrationFactor(Int_t j, Double_t value);

  void    Ratio(QwQPD &numer, QwQPD &denom);
  void    Scale(Double_t factor);

  VQwBPM& operator=  (const VQwBPM &value);
  VQwBPM& operator+= (const VQwBPM &value);
  VQwBPM& operator-= (const VQwBPM &value);

  virtual QwQPD& operator=  (const QwQPD &value);
  virtual QwQPD& operator+= (const QwQPD &value);
  virtual QwQPD& operator-= (const QwQPD &value);

  void    AccumulateRunningSum(const QwQPD& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void    AccumulateRunningSum(const VQwBPM& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void    DeaccumulateRunningSum(VQwBPM &value, Int_t ErrorMask=0xFFFFFFF);
  void    DeaccumulateRunningSum(QwQPD& value, Int_t ErrorMask=0xFFFFFFF);
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

  void    MakeQPDList();


  protected:
  VQwHardwareChannel* GetSubelementByName(TString ch_name);


  /////
 private:


  static const TString subelement[4];
  /*  Position calibration factor, transform ADC counts in mm */
  Double_t fQwQPDCalibration[2];


 protected:
  std::array<QwVQWK_Channel,4> fPhotodiode;//[4];
  std::array<QwVQWK_Channel,2> fRelPos;//[2];

  //  These are the "real" data elements, to which the base class
  //  fAbsPos_base and fEffectiveCharge_base are pointers.
  std::array<QwVQWK_Channel,2> fAbsPos;//[2];
  QwVQWK_Channel fEffectiveCharge;

  std::vector<QwVQWK_Channel> fQPDElementList;

};


#endif

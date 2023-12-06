/**********************************************************\
* File: QwCombinedBPM.h                                   *
*                                                         *
* Author:B. Waidyawansa                                   *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwCOMBINEDBPM__
#define __QwCOMBINEDBPM__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "VQwHardwareChannel.h"
#include "VQwBPM.h"
#include "QwUtil.h"

// Forward declarations
#ifdef __USE_DATABASE__
class QwDBInterface;
#endif // __USE_DATABASE__
class QwParameterFile;

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
template<typename T>
class QwCombinedBPM : public VQwBPM {
  friend class QwEnergyCalculator;

  /////
 public:

//-------------------------------------------------------------------------
  size_t GetNumberOfElements() {return fElement.size();};
  TString GetSubElementName(Int_t index) {return fElement.at(index)->GetElementName();};
//-------------------------------------------------------------------------

  QwCombinedBPM(){
  };
  QwCombinedBPM(TString name):VQwBPM(name){
    InitializeChannel(name);
  };
  QwCombinedBPM(TString subsystem, TString name): VQwBPM(name){
    InitializeChannel(subsystem, name);
  };

  QwCombinedBPM(TString subsystem, TString name, TString type): VQwBPM(name){
    InitializeChannel(subsystem, name,type);
  };
  QwCombinedBPM(const QwCombinedBPM& source)
  : VQwBPM(source),
    fEffectiveCharge(source.fEffectiveCharge)
  {
    QwCopyArray(source.fSlope, fSlope);
    QwCopyArray(source.fIntercept, fIntercept);
    QwCopyArray(source.fMinimumChiSquare, fMinimumChiSquare);
    QwCopyArray(source.fAbsPos, fAbsPos);
  }
  virtual ~QwCombinedBPM() { };

  using VQwBPM::EBeamPositionMonitorAxis;

  void    InitializeChannel(TString name);
  // new routine added to update necessary information for tree trimming
  void    InitializeChannel(TString subsystem, TString name);
  void    InitializeChannel(TString subsystem, TString name, TString type) {
    SetModuleType(type);
    InitializeChannel(subsystem, name);
  }

  void    SetMockDataAsDiff();

  void    LoadChannelParameters(QwParameterFile &paramfile){};
  void    ClearEventData();
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

  const VQwHardwareChannel* GetSlope(EBeamPositionMonitorAxis axis) const{
    if (axis<0 || axis>2){
      TString loc="QwLinearDiodeArray::GetPosition for "
        +this->GetElementName()+" failed for axis value "+Form("%d",axis);
      throw std::out_of_range(loc.Data());
    }
    return &fSlope[axis];
  }
  const VQwHardwareChannel* GetEffectiveCharge() const {
    return &fEffectiveCharge;
  }


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


  void    SetBPMForCombo(const VQwBPM* bpm, Double_t charge_weight,  Double_t x_weight, Double_t y_weight,Double_t sumqw);

  void    Ratio(QwCombinedBPM &numer, QwCombinedBPM &denom);
  void    Ratio(VQwBPM &numer, VQwBPM &denom);
  void    Scale(Double_t factor);

  VQwBPM& operator=  (const VQwBPM &value);
  VQwBPM& operator+= (const VQwBPM &value);
  VQwBPM& operator-= (const VQwBPM &value);

  virtual QwCombinedBPM& operator=  (const QwCombinedBPM &value);
  virtual QwCombinedBPM& operator+= (const QwCombinedBPM &value);
  virtual QwCombinedBPM& operator-= (const QwCombinedBPM &value);

  void    AccumulateRunningSum(const VQwBPM& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void    AccumulateRunningSum(const QwCombinedBPM& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void    DeaccumulateRunningSum(VQwBPM& value, Int_t ErrorMask=0xFFFFFFF);
  void    DeaccumulateRunningSum(QwCombinedBPM& value, Int_t ErrorMask=0xFFFFFFF);
  void    CalculateRunningAverage();

  void    ConstructHistograms(TDirectory *folder, TString &prefix);
  void    FillHistograms();

  void    ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void    ConstructBranch(TTree *tree, TString &prefix);
  void    ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void    FillTreeVector(std::vector<Double_t> &values) const;

//------------------------------------------------------------------------------------------------
  void    RandomizeEventData(int helicity = 0, double time = 0.0);
  void    SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY,
                                   Double_t meanXslope, Double_t sigmaXslope, Double_t meanYslope, Double_t sigmaYslope);
  void    GetProjectedPosition(VQwBPM *device);
  void    LoadMockDataParameters(QwParameterFile &paramfile);
//------------------------------------------------------------------------------------------------

  VQwHardwareChannel* GetAngleX(){ //At present this returns the slope not the angle
    return &fSlope[0];
  };

  const VQwHardwareChannel* GetAngleX() const { //At present this returns the slope not the angle
    return const_cast<QwCombinedBPM*>(this)->GetAngleX();
  };

  VQwHardwareChannel* GetAngleY(){//At present this returns the slope not the angle
    return &fSlope[1];
  };

  const VQwHardwareChannel* GetAngleY() const { //At present this returns the slope not the angle
    return const_cast<QwCombinedBPM*>(this)->GetAngleY();
  };


#ifdef __USE_DATABASE__
  std::vector<QwDBInterface> GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();
#endif // __USE_DATABASE__

 protected:
  VQwHardwareChannel* GetSubelementByName(TString ch_name);

  /* Functions for least square fit */
  void     CalculateFixedParameter(std::vector<Double_t> fWeights, Int_t pos);
  Double_t SumOver( std::vector <Double_t> weight , std::vector <T> val);
  void     LeastSquareFit(VQwBPM::EBeamPositionMonitorAxis axis, std::vector<Double_t> fWeights) ; //bbbbb



  /////
 private:
  Bool_t fixedParamCalculated;

  //used for least squares fit
  Double_t erra[2],errb[2],covab[2];
  Double_t A[2], B[2], D[2], m[2];
  Double_t chi_square[2];
  Double_t fSumQweights;

  std::vector <const VQwBPM*> fElement;
  std::vector <Double_t> fQWeights;
  std::vector <Double_t> fXWeights;
  std::vector <Double_t> fYWeights;


 protected:
  /* This channel contains the beam slope w.r.t the X & Y axis at the target */
  std::array<T,2> fSlope;//[2];

  /* This channel contains the beam intercept w.r.t the X & Y axis at the target */
  std::array<T,2> fIntercept;//[2];

  /*This channel gives the minimum chisquare value for the fit over target bpms*/
  std::array<T,2> fMinimumChiSquare;//[2];

  //  These are the "real" data elements, to which the base class
  //  fAbsPos_base and fEffectiveCharge_base are pointers.
  std::array<T,2> fAbsPos;//[2];
  T fEffectiveCharge;

private:
  // Functions to be removed
  void    MakeBPMComboList();
  std::vector<T> fBPMComboElementList;

};



#endif

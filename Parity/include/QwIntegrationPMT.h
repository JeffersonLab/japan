/**********************************************************\
* File: QwIntegrationPMT.h                                *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwMollerADC_IntegrationPMT__
#define __QwMollerADC_IntegrationPMT__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwMollerADC_Channel.h"
#include "QwParameterFile.h"

// Forward declarations
class QwBlinder;
class QwDBInterface;


/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
class QwIntegrationPMT : public VQwDataElement{
/////
 public:
  QwIntegrationPMT() { 
    InitializeChannel("","raw");
  };
  QwIntegrationPMT(TString name){
    InitializeChannel(name,"raw");
  };
  QwIntegrationPMT(TString subsystemname, TString name){
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name,"raw");
  };
  QwIntegrationPMT(const QwIntegrationPMT& source)
  : VQwDataElement(source),
    fPedestal(source.fPedestal),
    fCalibration(source.fCalibration),
    fTriumf_ADC(source.fTriumf_ADC),
    fIsBlindable(source.fIsBlindable),
    fIsNormalizable(source.fIsNormalizable)
  { }
  virtual ~QwIntegrationPMT() { };

  void    LoadChannelParameters(QwParameterFile &paramfile){};

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave); 
  // same purpose as above but this was needed to accormodate combinedPMT. Unlike Beamline combined devices where they have MollerADC channels, Combined PMT has integration PMT 
  void  InitializeChannel(TString subsystem, TString module, TString name, TString datatosave); 
  void SetElementName(const TString &name) { fElementName = name; fTriumf_ADC.SetElementName(name);};

  const QwMollerADC_Channel* GetChannel(const TString name) const {
    if (fTriumf_ADC.GetElementName() == name) return &fTriumf_ADC;
    else return 0;
  };



  void  ClearEventData();
  void PrintErrorCounters();
/********************************************************/

  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity = 0, double time = 0.0);
  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);
void RandomizeMollerEvent(int helicity, const QwBeamCharge& charge, const QwBeamPosition& xpos, const QwBeamPosition& ypos, const QwBeamAngle& xprime, const QwBeamAngle& yprime, const QwBeamEnergy& energy);


  void  UseExternalRandomVariable();
  void  SetExternalRandomVariable(Double_t random_variable);

  Double_t GetValue();
  Double_t GetValue(Int_t blocknum);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  void IncrementErrorCounters(){
    fTriumf_ADC.IncrementErrorCounters();
  }
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  Int_t SetSingleEventCuts(Double_t, Double_t);//set two limts
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  void SetSingleEventCuts(UInt_t errorflag, Double_t LL, Double_t UL, Double_t stability, Double_t burplevel);
  void SetDefaultSampleSize(Int_t sample_size);
  void SetSaturationLimit(Double_t saturation_volt );
  UInt_t GetEventcutErrorFlag(){//return the error flag
    return fTriumf_ADC.GetEventcutErrorFlag();
  }

  Bool_t CheckForBurpFail(const VQwDataElement *ev_error);
  
  UInt_t UpdateErrorFlag() {return GetEventcutErrorFlag();};
  void UpdateErrorFlag(const QwIntegrationPMT *ev_error);

  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    fTriumf_ADC.SetEventCutMode(bcuts);
  }

  void SetBlindability(Bool_t isblindable){fIsBlindable=isblindable;};
  void SetNormalizability(Bool_t isnormalizable){fIsNormalizable = isnormalizable;};

  /// \brief Blind the asymmetry
  void Blind(const QwBlinder *blinder);
  /// \brief Blind the difference using the yield
  void Blind(const QwBlinder *blinder, const QwIntegrationPMT& yield);

  void PrintValue() const;
  void PrintInfo() const;

/*   Double_t GetRawBlockValue(size_t blocknum) */
/*            {return fTriumf_ADC.GetRawBlockValue(blocknum);}; */



  QwIntegrationPMT& operator=  (const QwIntegrationPMT &value);
  QwIntegrationPMT& operator+= (const QwIntegrationPMT &value);
  QwIntegrationPMT& operator-= (const QwIntegrationPMT &value);
  void Ratio(QwIntegrationPMT &numer, QwIntegrationPMT &denom);
  void Scale(Double_t factor);
  void Normalize(VQwDataElement* denom);
  void AccumulateRunningSum(const QwIntegrationPMT& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
  void DeaccumulateRunningSum(QwIntegrationPMT& value, Int_t ErrorMask=0xFFFFFFF);
  void CalculateRunningAverage();

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void SetNormRate(Double_t value)      {fNormRate  = value;};
  void SetVoltPerHz(Double_t value)     {fVoltPerHz = value;};

  void SetAsymmetry(Double_t value)     {fAsym      = value;};
  void SetCoefficientCx(Double_t value) {fCoeff_x   = value;};
  void SetCoefficientCy(Double_t value) {fCoeff_y   = value;};
  void SetCoefficientCxp(Double_t value){fCoeff_xp  = value;};
  void SetCoefficientCyp(Double_t value){fCoeff_yp  = value;};
  void SetCoefficientCe(Double_t value) {fCoeff_e   = value;};

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  std::vector<QwDBInterface> GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();

 protected:


 private:

  Double_t fPedestal;
  Double_t fCalibration;
  Double_t fNormRate;
  Double_t fVoltPerHz;
  Double_t fAsym;
  Double_t fCoeff_x;
  Double_t fCoeff_y;
  Double_t fCoeff_xp;
  Double_t fCoeff_yp;
  Double_t fCoeff_e;

  Bool_t fGoodEvent;//used to validate sequence number in the IsGoodEvent()

  QwMollerADC_Channel fTriumf_ADC;

  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwMollerADC_Channel::fDeviceErrorCode

  Bool_t fIsBlindable; //
  Bool_t fIsNormalizable;

  const static  Bool_t bDEBUG=kFALSE;//debugging display purposes
  Bool_t bEVENTCUTMODE; //global switch to turn event cuts ON/OFF
};



#endif

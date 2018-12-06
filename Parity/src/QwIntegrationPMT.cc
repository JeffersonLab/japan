/**********************************************************\
* File: QwIntegrationPMT.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwIntegrationPMT.h"

// System headers
#include <stdexcept>

// Qweak headers
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif
#include "QwVQWK_Channel.h"
#include "QwParameterFile.h"

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetPedestal(Double_t pedestal)
{
  fPedestal = pedestal;
  fADC.SetPedestal(fPedestal);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetCalibrationFactor(Double_t calib)
{
  fCalibration = calib;
  fADC.SetCalibrationFactor(fCalibration);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fADC.InitializeChannel(name,datatosave);
  SetElementName(name);
  SetBlindability(kTRUE);
  SetNormalizability(kTRUE);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::InitializeChannel(TString subsystem, TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fADC.InitializeChannel(subsystem,"QwIntegrationPMT", name, datatosave);
  SetElementName(name);
  SetBlindability(kTRUE);
  SetNormalizability(kTRUE);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::InitializeChannel(TString subsystem, TString module, TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fADC.InitializeChannel(subsystem,module, name, datatosave);
  SetElementName(name);
  SetBlindability(kTRUE);
  SetNormalizability(kTRUE);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::ClearEventData()
{
  fADC.ClearEventData();
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::PrintErrorCounters()
{
  fADC.PrintErrorCounters();
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::UseExternalRandomVariable()
{
  fADC.UseExternalRandomVariable();
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetExternalRandomVariable(double random_variable)
{
  fADC.SetExternalRandomVariable(random_variable);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fADC.SetRandomEventDriftParameters(amplitude, phase, frequency);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fADC.AddRandomEventDriftParameters(amplitude, phase, frequency);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fADC.SetRandomEventParameters(mean, sigma);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fADC.SetRandomEventAsymmetry(asymmetry);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::RandomizeEventData(int helicity, double time)
{
  fADC.RandomizeEventData(helicity, time);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::RandomizeMollerEvent(
        int helicity,
        const QwBeamCharge& charge,
        const QwBeamPosition& xpos,
        const QwBeamPosition& ypos,
        const QwBeamAngle& xprime,
        const QwBeamAngle& yprime,
        const QwBeamEnergy& energy)
{
  T temp(fADC);
  fADC.ClearEventData();

  temp.AssignScaledValue(xpos, fCoeff_x);
  fADC += temp;

  temp.AssignScaledValue(ypos, fCoeff_y);
  fADC += temp;

  temp.AssignScaledValue(xprime, fCoeff_xp);
  fADC += temp;

  temp.AssignScaledValue(yprime, fCoeff_yp);
  fADC += temp;

  temp.AssignScaledValue(energy, fCoeff_e);
  fADC += temp;

  //fADC.AddChannelOffset(fBaseRate * (1+helicity*fAsym));
  fADC.AddChannelOffset(1.0+helicity*fAsym);

  fADC *= charge;
  fADC.Scale(fNormRate*fVoltPerHz);  //  After this Scale function, fADC should be the detector signal in volts.
  fADC.ForceMapfileSampleSize();
  //  Double_t voltage_width = sqrt(fADC.GetValue()*window_length/fVoltPerHz)/(window_length/fVoltPerHz);
  Double_t voltage_width = sqrt( fADC.GetValue() / (fADC.GetNumberOfSamples()* T::kTimePerSample/Qw::sec/fVoltPerHz) );
  fADC.SmearByResolution(voltage_width);
  fADC.SetRawEventData();
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fADC.SetHardwareSum(hwsum, sequencenumber);
}

template<typename T>
Double_t QwIntegrationPMT<T>::GetValue()
{
  return fADC.GetValue();
}

template<typename T>
Double_t QwIntegrationPMT<T>::GetValue(Int_t blocknum)
{
  return fADC.GetValue(blocknum);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fADC.SetEventData(block, sequencenumber);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fADC.EncodeEventData(buffer);
}

/********************************************************/
template<typename T>
void  QwIntegrationPMT<T>::ProcessEvent()
{
  ApplyHWChecks();//first apply HW checks and update HW  error flags.
  fADC.ProcessEvent();
}

/********************************************************/
template<typename T>
Bool_t QwIntegrationPMT<T>::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=fADC.ApplyHWChecks();//will check for consistancy between HWSUM and SWSUM also check for sample size
  eventokay=(deviceerror & 0x0);//if no HW error return true

  return eventokay;
}

/********************************************************/
template<typename T>
Int_t QwIntegrationPMT<T>::SetSingleEventCuts(Double_t LL /* = 0 */, Double_t UL /* = 0 */) // FIXME no default arguments originally, but not allowed
{
  //std::vector<Double_t> & dEventCuts){//two limts and sample size
  fADC.SetSingleEventCuts(LL,UL);
  return 1; // FIXME return value of ADC return?
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetSingleEventCuts(
        UInt_t errorflag,
        Double_t LL /* = 0 */, // FIXME originally no default arguments
        Double_t UL /* = 0 */,
        Double_t stability /* = 0 */)
{
  //set the unique tag to identify device type (bcm,bpm & etc)
  errorflag|=kPMTErrorFlag;
  QwMessage<<"QwIntegrationPMT Error Code passing to QwVQWK_Ch "<<errorflag<<QwLog::endl;
  fADC.SetSingleEventCuts(errorflag,LL,UL,stability);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::SetDefaultSampleSize(Int_t sample_size)
{
 fADC.SetDefaultSampleSize((size_t)sample_size);
}

/********************************************************/
template<typename T>
Bool_t QwIntegrationPMT<T>::ApplySingleEventCuts()
{
  //std::cout<<" QwBCM::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;

  if (fADC.ApplySingleEventCuts()){
    status=kTRUE;
    //std::cout<<" BCM Sample size "<<fADC.GetNumberOfSamples()<<std::endl;
  }
  else{
    status&=kFALSE;//kTRUE;//kFALSE;
  }

  return status;
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::PrintErrorCounters() const{
  // report number of events failed due to HW and event cut faliure
  fADC.PrintErrorCounters();
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::UpdateErrorFlag(const QwIntegrationPMT* ev_error)
{
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwIntegrationPMT<T>::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
	fADC.UpdateErrorFlag(ev_error->fADC);
      }
    } else {
      TString loc="Standard exception from QwIntegrationPMT<T>::UpdateErrorFlag :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }
}

/********************************************************/
template<typename T>
Int_t QwIntegrationPMT<T>::ProcessEvBuffer(
        UInt_t* buffer,
        UInt_t word_position_in_buffer,
        UInt_t subelement)
{
  fADC.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
}

/********************************************************/
template<typename T>
QwIntegrationPMT<T>& QwIntegrationPMT<T>::operator= (const QwIntegrationPMT<T> &value)
{
//   std::cout<<" Here in QwIntegrationPMT<T>::operator= \n";
  if (GetElementName()!="")
    {
      this->fADC=value.fADC;
      this->fPedestal=value.fPedestal;
      this->fCalibration=value.fCalibration;
    }
//   std::cout<<" to be copied \n";
//   value.Print();
//   std::cout<<" copied \n";
//   this->Print();

  return *this;
}

/********************************************************/
template<typename T>
QwIntegrationPMT<T>& QwIntegrationPMT<T>::operator+= (const QwIntegrationPMT<T> &value)
{
  if (GetElementName()!="")
    {
      this->fADC+=value.fADC;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}

/********************************************************/
template<typename T>
QwIntegrationPMT<T>& QwIntegrationPMT<T>::operator-= (const QwIntegrationPMT<T> &value)
{
  if (GetElementName()!="")
    {
      this->fADC-=value.fADC;
      this->fPedestal-=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::Sum(QwIntegrationPMT<T> &value1, QwIntegrationPMT<T> &value2){
  *this =  value1;
  *this += value2;
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::Difference(QwIntegrationPMT<T> &value1, QwIntegrationPMT<T> &value2){
  *this =  value1;
  *this -= value2;
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::Ratio(QwIntegrationPMT<T> &numer, QwIntegrationPMT<T> &denom)
{
  //  std::cout<<"QwIntegrationPMT<T>::Ratio element name ="<<GetElementName()<<" \n";
  if (GetElementName()!="")
    {
      //  std::cout<<"here in \n";
      this->fADC.Ratio(numer.fADC,denom.fADC);
      this->fPedestal=0;
      this->fCalibration=0;
    }
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::Scale(Double_t factor)
{
  fADC.Scale(factor);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::Normalize(VQwDataElement* denom)
{
  if (fIsNormalizable) {
    T* denom_ptr = dynamic_cast<T*>(denom);
    T vqwk_denom(*denom_ptr);
    fADC.DivideBy(vqwk_denom);
  }
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::PrintValue() const
{
  fADC.PrintValue();
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::PrintInfo() const
{
  //std::cout<<"QwVQWK_Channel Info " <<std::endl;
  //std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<IntegrationPMT_Running_AVG<<std::endl;
  std::cout<<"QwVQWK_Channel Info " <<std::endl;
  fADC.PrintInfo();
  std::cout<< "Blindability is "    << (fIsBlindable?"TRUE":"FALSE") 
	   <<std::endl;
  std::cout<< "Normalizability is " << (fIsNormalizable?"TRUE":"FALSE")
	   <<std::endl;
  std::cout << "fNormRate=" << fNormRate << "fVoltPerHz=" << fVoltPerHz 
            << " Asym=" << fAsym << " C_x=" << fCoeff_x << " C_y=" << fCoeff_y 
            << " C_xp=" << fCoeff_xp << " C_yp=" << fCoeff_yp 
            << " C_e=" << fCoeff_e << std::endl;
}

/********************************************************/
template<typename T>
void  QwIntegrationPMT<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fADC.ConstructHistograms(folder, prefix);
    }
}

/********************************************************/
template<typename T>
void  QwIntegrationPMT<T>::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fADC.FillHistograms();
    }
}

/********************************************************/
template<typename T>
void  QwIntegrationPMT<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fADC.ConstructBranchAndVector(tree, prefix,values);
    }
}

/********************************************************/
template<typename T>
void  QwIntegrationPMT<T>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fADC.ConstructBranch(tree, prefix);
    }
}

/********************************************************/
template<typename T>
void  QwIntegrationPMT<T>::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
   TString devicename;
   devicename=GetElementName();
   devicename.ToLower();
   if (GetElementName()==""){
     //  This channel is not used, so skip filling the histograms.
   } else {
     if (modulelist.HasValue(devicename)){
       fADC.ConstructBranch(tree, prefix);
       QwMessage <<"QwIntegrationPMT Tree leave added to "<<devicename<<QwLog::endl;
       }

   }
}


/********************************************************/
template<typename T>
void  QwIntegrationPMT<T>::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {
    fADC.FillTreeVector(values);
  }
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::CalculateRunningAverage()
{
  fADC.CalculateRunningAverage();
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::AccumulateRunningSum(const QwIntegrationPMT<T>& value)
{
  fADC.AccumulateRunningSum(value.fADC);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::DeaccumulateRunningSum(QwIntegrationPMT<T>& value)
{
  fADC.DeaccumulateRunningSum(value.fADC);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::Blind(const QwBlinder *blinder)
{
  if (fIsBlindable)  fADC.Blind(blinder);
}

/********************************************************/
template<typename T>
void QwIntegrationPMT<T>::Blind(const QwBlinder *blinder, const QwIntegrationPMT<T>& yield)
{
  if (fIsBlindable)  fADC.Blind(blinder, yield.fADC);
}

#ifdef __USE_DATABASE__

/********************************************************/
template<typename T>
std::vector<QwDBInterface> QwIntegrationPMT<T>::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  fADC.AddEntriesToList(row_list);
  return row_list;

}

/********************************************************/
template<typename T>
std::vector<QwErrDBInterface> QwIntegrationPMT<T>::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();
  fADC.AddErrEntriesToList(row_list);
  return row_list;
}

#endif

template class QwIntegrationPMT<QwVQWK_Channel>;

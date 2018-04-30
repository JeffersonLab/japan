/**********************************************************\
* File: QwIntegratedRasterChannel.h                       *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwIntegratedRasterChannel.h"

// System headers
#include <stdexcept>

// Qweak database headers
#include "QwDBInterface.h"

// Qweak types that we want to use in this template
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"

/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::SetPedestal(Double_t pedestal)
{
	fPedestal=pedestal;
	fTriumf_ADC.SetPedestal(fPedestal);
	return;
}

template<typename T>
void QwIntegratedRasterChannel<T>::SetCalibrationFactor(Double_t calib)
{
	fCalibration=calib;
	fTriumf_ADC.SetCalibrationFactor(fCalibration);
	return;
}
/********************************************************/
template<typename T>
void  QwIntegratedRasterChannel<T>::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(name,datatosave);
  SetElementName(name);
  //set default limits to event cuts
  fLLimit=0;//init two timits
  fULimit=0;//init two timits
  return;
}
/********************************************************/
template<typename T>
void  QwIntegratedRasterChannel<T>::InitializeChannel(TString subsystem, TString name, TString datatosave){
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(subsystem, "QwIntegratedRasterChannel", name, datatosave);
  SetElementName(name);
  //set default limits to event cuts
  fLLimit=0;//init two timits
  fULimit=0;//init two timits
  return;  
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::ClearEventData()
{
  fTriumf_ADC.ClearEventData();
  return;
}


/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::UseExternalRandomVariable()
{
  fTriumf_ADC.UseExternalRandomVariable();
  return;
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::SetExternalRandomVariable(double random_variable)
{
  fTriumf_ADC.SetExternalRandomVariable(random_variable);
  return;
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.SetRandomEventDriftParameters(amplitude, phase, frequency);
  return;
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.AddRandomEventDriftParameters(amplitude, phase, frequency);
  return;
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fTriumf_ADC.SetRandomEventParameters(mean, sigma);
  return;
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fTriumf_ADC.SetRandomEventAsymmetry(asymmetry);
  return;
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::RandomizeEventData(int helicity, double time)
{
  fTriumf_ADC.RandomizeEventData(helicity, time);
  return;
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fTriumf_ADC.SetHardwareSum(hwsum, sequencenumber);
  return;
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fTriumf_ADC.SetEventData(block, sequencenumber);
  return;
}
/********************************************************/
template<typename T>
void QwIntegratedRasterChannel<T>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fTriumf_ADC.EncodeEventData(buffer);
}
/********************************************************/
template<typename T>
void  QwIntegratedRasterChannel<T>::ProcessEvent()
{
  ApplyHWChecks();//first apply HW checks and update HW  error flags. Calling this routine either in ApplySingleEventCuts or here do not make any difference for a BCM but do for a BPMs because they have derrived devices.
  fTriumf_ADC.ProcessEvent();
  return;
}
/********************************************************/
template<typename T>
Bool_t QwIntegratedRasterChannel<T>::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  fDeviceErrorCode=fTriumf_ADC.ApplyHWChecks();//will check for HW consistancy and return the error code (=0 is HW good)
  fEventIsGood=(fDeviceErrorCode & 0x0);//if no HW error return true


  return fEventIsGood;
}
/********************************************************/

template<typename T>
Int_t QwIntegratedRasterChannel<T>::SetSingleEventCuts(Double_t LL, Double_t UL){
  fTriumf_ADC.SetSingleEventCuts(LL,UL);
  return 1;
}

/********************************************************/

template<typename T>
void QwIntegratedRasterChannel<T>::SetSingleEventCuts(UInt_t errorflag, Double_t LL, Double_t UL, Double_t stability){
  //set the unique tag to identify device type (bcm,bpm & etc)
  //errorflag|=kBCMErrorFlag;
  QwMessage<<"QwIntegratedRasterChannel<T> Error Code passing to QwVQWK_Ch "<<errorflag<<QwLog::endl;
  fTriumf_ADC.SetSingleEventCuts(errorflag,LL,UL,stability);

}

template<typename T>
void QwIntegratedRasterChannel<T>::SetDefaultSampleSize(Int_t sample_size){
  fTriumf_ADC.SetDefaultSampleSize((size_t)sample_size);
}


/********************************************************/
template<typename T>
Bool_t QwIntegratedRasterChannel<T>::ApplySingleEventCuts(){
  //std::cout<<" QwIntegratedRasterChannel<T>::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;


  if (fTriumf_ADC.ApplySingleEventCuts()){
    status=kTRUE;
  }
  else{

    if (bDEBUG) std::cout<<" evnt cut failed:-> set limit "<<fULimit;
    status&=kFALSE;
  }
  fDeviceErrorCode|=fTriumf_ADC.GetEventcutErrorFlag();//retrun the error flag for event cuts


  return status;

}

/********************************************************/

template<typename T>
void QwIntegratedRasterChannel<T>::PrintErrorCounters() const{// report number of events failed due to HW and event cut faliure
  fTriumf_ADC.PrintErrorCounters();
}



template<typename T>
Int_t QwIntegratedRasterChannel<T>::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  fTriumf_ADC.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
}
/********************************************************/
template<typename T>
QwIntegratedRasterChannel<T>& QwIntegratedRasterChannel<T>::operator= (const QwIntegratedRasterChannel<T> &value)
{
//   std::cout<<" Here in QwIntegratedRasterChannel<T>::operator= \n";
  if (GetElementName()!="")
    {
      this->fTriumf_ADC=value.fTriumf_ADC;
      this->fPedestal=value.fPedestal;
      this->fCalibration=value.fCalibration;
    }
//   std::cout<<" to be copied \n";
//   value.Print();
//   std::cout<<" copied \n";
//   this->Print();

  return *this;
}

template<typename T>
QwIntegratedRasterChannel<T>& QwIntegratedRasterChannel<T>::operator+= (const QwIntegratedRasterChannel<T> &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC+=value.fTriumf_ADC;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}

template<typename T>
QwIntegratedRasterChannel<T>& QwIntegratedRasterChannel<T>::operator-= (const QwIntegratedRasterChannel<T> &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC-=value.fTriumf_ADC;
      this->fPedestal-=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}


template<typename T>
void QwIntegratedRasterChannel<T>::Sum(QwIntegratedRasterChannel<T> &value1, QwIntegratedRasterChannel<T> &value2){
  *this =  value1;
  *this += value2;
}

template<typename T>
void QwIntegratedRasterChannel<T>::Difference(QwIntegratedRasterChannel<T> &value1, QwIntegratedRasterChannel<T> &value2){
  *this =  value1;
  *this -= value2;
}

template<typename T>
void QwIntegratedRasterChannel<T>::Ratio(QwIntegratedRasterChannel<T> &numer, QwIntegratedRasterChannel<T> &denom)
{
  *this =  numer;
  return;
}

template<typename T>
void QwIntegratedRasterChannel<T>::Scale(Double_t factor)
{
  fTriumf_ADC.Scale(factor);
  return;
}

template<typename T>
void QwIntegratedRasterChannel<T>::CalculateRunningAverage() {
  fTriumf_ADC.CalculateRunningAverage();
}

template<typename T>
void QwIntegratedRasterChannel<T>::AccumulateRunningSum(const QwIntegratedRasterChannel<T> &value) {
  fTriumf_ADC.AccumulateRunningSum(value.fTriumf_ADC);
}


template<typename T>
void QwIntegratedRasterChannel<T>::PrintValue() const
{
  fTriumf_ADC.PrintValue();
}

template<typename T>
void QwIntegratedRasterChannel<T>::PrintInfo() const
{
  std::cout << "QwVQWK_Channel Info " << std::endl;
  fTriumf_ADC.PrintInfo();
}

/********************************************************/
template<typename T>
void  QwIntegratedRasterChannel<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fTriumf_ADC.ConstructHistograms(folder, prefix);
    }
  return;
}

template<typename T>
void  QwIntegratedRasterChannel<T>::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fTriumf_ADC.FillHistograms();
    }


  return;
}

template<typename T>
void  QwIntegratedRasterChannel<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip
  } else
    {    
    TString thisprefix=prefix;
    if(prefix=="asym_"){
      if (GetElementName()=="qwk_raster_x"||GetElementName()=="qwk_raster_y"||GetElementName()=="qwk_raster_sumxy") thisprefix="diff_";
    }
      fTriumf_ADC.ConstructBranchAndVector(tree, thisprefix,values);
    }
  return;
}

template<typename T>
void  QwIntegratedRasterChannel<T>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.ConstructBranch(tree, prefix);
      // this functions doesn't do anything yet
    }
  return;
}

template<typename T>
void  QwIntegratedRasterChannel<T>::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;

  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {

      //QwMessage <<" QwIntegratedRasterChannel<T> "<<devicename<<QwLog::endl;
      if (modulelist.HasValue(devicename)){
	fTriumf_ADC.ConstructBranch(tree, prefix);
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
      }
      // this functions doesn't do anything yet
    }
  return;
}

template<typename T>
void  QwIntegratedRasterChannel<T>::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.FillTreeVector(values);
      // this functions doesn't do anything yet
    }
  return;
}

template<typename T>
std::vector<QwDBInterface> QwIntegratedRasterChannel<T>::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  fTriumf_ADC.AddEntriesToList(row_list);
  return row_list;
}


template<typename T>
std::vector<QwErrDBInterface> QwIntegratedRasterChannel<T>::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  fTriumf_ADC.AddErrEntriesToList(row_list);
  return row_list;
}


template class QwIntegratedRasterChannel<QwVQWK_Channel>;

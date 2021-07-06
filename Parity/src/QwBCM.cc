/**********************************************************\
* File: QwBCM.h                                          *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBCM.h"

// System headers
#include <stdexcept>

// Qweak database headers
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif // __USE_DATABASE__

//  Qweak types that we want to use in this template
#include "QwVQWK_Channel.h"
#include "QwADC18_Channel.h"
#include "QwScaler_Channel.h"
#include "QwMollerADC_Channel.h"
/********************************************************/
template<typename T>
void QwBCM<T>::SetPedestal(Double_t pedestal)
{
  fBeamCurrent.SetPedestal(pedestal);
}

template<typename T>
void QwBCM<T>::SetCalibrationFactor(Double_t calibration)
{
  fBeamCurrent.SetCalibrationFactor(calibration);
}
/********************************************************/
template<typename T>
void QwBCM<T>::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  SetResolution(0.0);
  fBeamCurrent.InitializeChannel(name,datatosave);
  this->SetElementName(name);
}
/********************************************************/
template<typename T>
void QwBCM<T>::InitializeChannel(TString subsystem, TString name, TString datatosave){
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  SetResolution(0.0);
  fBeamCurrent.InitializeChannel(subsystem, "QwBCM", name, datatosave);
  SetElementName(name);
}
/********************************************************/
template<typename T>
void QwBCM<T>::InitializeChannel(TString subsystem, TString name, TString type, TString datatosave){
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  SetResolution(0.0);
  SetModuleType(type);
  fBeamCurrent.InitializeChannel(subsystem, "QwBCM", name, datatosave);
  SetElementName(name);
}
/********************************************************/
template<typename T>
void QwBCM<T>::LoadMockDataParameters(QwParameterFile &paramfile) {
/*
  Bool_t ldebug=kFALSE;
  Double_t asym=0.0, mean=0.0, sigma=0.0;
*/
  Double_t res=0.0;

  if (paramfile.GetLine().find("resolution")!=std::string::npos){
    paramfile.GetNextToken();
    res = paramfile.GetTypedNextToken<Double_t>();
    this->SetResolution(res);
  } else {
/*
    asym    = paramfile.GetTypedNextToken<Double_t>();
    mean    = paramfile.GetTypedNextToken<Double_t>(); 
    sigma   = paramfile.GetTypedNextToken<Double_t>();
    
    if (ldebug==1) {
      std::cout << "#################### \n";
      std::cout << "asym, mean, sigma \n" << std::endl;
      std::cout << asym                   << " / "
	        << mean                   << " / "
	        << sigma                  << " / "
                << std::endl;
    }
    this->SetRandomEventParameters(mean, sigma);
    this->SetRandomEventAsymmetry(asym);
*/
    //std::cout << "In QwBCM: ChannelName = " << GetElementName() << std::endl;
    fBeamCurrent.SetMockDataAsDiff();
    fBeamCurrent.LoadMockDataParameters(paramfile);    
  }
}
/********************************************************/
template<typename T>
void QwBCM<T>::ClearEventData()
{
  fBeamCurrent.ClearEventData();
}

/********************************************************/
template<typename T>
void QwBCM<T>::UseExternalRandomVariable()
{
  fBeamCurrent.UseExternalRandomVariable();
}
/********************************************************/
template<typename T>
void QwBCM<T>::SetExternalRandomVariable(double random_variable)
{
  fBeamCurrent.SetExternalRandomVariable(random_variable);
}
/********************************************************/
template<typename T>
void QwBCM<T>::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fBeamCurrent.SetRandomEventDriftParameters(amplitude, phase, frequency);
}
/********************************************************/
template<typename T>
void QwBCM<T>::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fBeamCurrent.AddRandomEventDriftParameters(amplitude, phase, frequency);
}
/********************************************************/
template<typename T>
void QwBCM<T>::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fBeamCurrent.SetRandomEventParameters(mean, sigma);
}
/********************************************************/
template<typename T>
void QwBCM<T>::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fBeamCurrent.SetRandomEventAsymmetry(asymmetry);
}
/********************************************************/
template<typename T>
void QwBCM<T>::ApplyResolutionSmearing(){
   fBeamCurrent.SmearByResolution(fResolution);
}
template<typename T>
void QwBCM<T>::FillRawEventData()
{
  fBeamCurrent.SetRawEventData();
}

template<typename T>
void QwBCM<T>::RandomizeEventData(int helicity, double time)
{
  fBeamCurrent.RandomizeEventData(helicity, time);
  fBeamCurrent.SetRawEventData();

}
/********************************************************/
template<typename T>
void QwBCM<T>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fBeamCurrent.EncodeEventData(buffer);
}

/********************************************************/
template<typename T>
void QwBCM<T>::ProcessEvent()
{
  this->ApplyHWChecks();//first apply HW checks and update HW  error flags. Calling this routine either in ApplySingleEventCuts or here do not make any difference for a BCM but do for a BPMs because they have derrived devices.
  fBeamCurrent.ProcessEvent();
}
/********************************************************/
template<typename T>
Bool_t QwBCM<T>::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=fBeamCurrent.ApplyHWChecks();//will check for HW consistancy and return the error code (=0 is HW good)
  eventokay=(deviceerror & 0x0);//if no HW error return true

  return eventokay;
}
/********************************************************/

template<typename T>
Int_t QwBCM<T>::SetSingleEventCuts(Double_t LL, Double_t UL){//std::vector<Double_t> & dEventCuts){//two limts and sample size
  fBeamCurrent.SetSingleEventCuts(LL,UL);
  return 1;
}

template<typename T>
void QwBCM<T>::SetSingleEventCuts(UInt_t errorflag, Double_t LL, Double_t UL, Double_t stability, Double_t burplevel){
  //set the unique tag to identify device type (bcm,bpm & etc)
  errorflag|=kBCMErrorFlag;
  QwMessage<<"QwBCM Error Code passing to QwVQWK_Ch "<<errorflag<<" "<<stability<<QwLog::endl;
  //QwError<<"***************************"<<typeid(fBeamCurrent).name()<<QwLog::endl;
  fBeamCurrent.SetSingleEventCuts(errorflag,LL,UL,stability,burplevel);

}

template<typename T>
void QwBCM<T>::SetDefaultSampleSize(Int_t sample_size){
  fBeamCurrent.SetDefaultSampleSize((size_t)sample_size);
}


/********************************************************/
template<typename T>
Bool_t QwBCM<T>::ApplySingleEventCuts()
{
  //std::cout<<" QwBCM::SingleEventCuts() "<<std::endl;
  Bool_t status = kTRUE;

  if (fBeamCurrent.ApplySingleEventCuts()) {
    status = kTRUE;
  } else {
    status &= kFALSE;
  }
  return status;
}

/********************************************************/

template<typename T>
void QwBCM<T>::IncrementErrorCounters()
{// report number of events failed due to HW and event cut faliure
  fBeamCurrent.IncrementErrorCounters();
}

template<typename T>
void QwBCM<T>::PrintErrorCounters() const
{// report number of events failed due to HW and event cut faliure
  fBeamCurrent.PrintErrorCounters();
}

/********************************************************/
template<typename T>
void QwBCM<T>::UpdateErrorFlag(const VQwBCM *ev_error){
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwBCM::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
        const QwBCM<T>* value_bcm = dynamic_cast<const QwBCM<T>* >(ev_error);
	fBeamCurrent.UpdateErrorFlag(value_bcm->fBeamCurrent);
      }
    } else {
      TString loc="Standard exception from QwBCM::UpdateErrorFlag :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl; 
  }
};


/********************************************************/
template<typename T>
Int_t QwBCM<T>::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  fBeamCurrent.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
}
/********************************************************/
template<typename T>
QwBCM<T>& QwBCM<T>::operator= (const QwBCM<T> &value)
{
//   std::cout<<" Here in QwBCM::operator= \n";
  if (this->GetElementName()!="")
    {
      this->fBeamCurrent=value.fBeamCurrent;
    }
//   std::cout<<" to be copied \n";
//   value.Print();
//   std::cout<<" copied \n";
//   this->Print();

  return *this;
}

template<typename T>
VQwBCM& QwBCM<T>::operator= (const VQwBCM &value)
{
  try {
    if(typeid(value)==typeid(*this)) {
      // std::cout<<" Here in QwBCM::operator= \n";
      if (this->GetElementName()!="") {
        const QwBCM<T>* value_bcm = dynamic_cast<const QwBCM<T>* >(&value);
        QwBCM<T>* this_cast = dynamic_cast<QwBCM<T>* >(this);
        this_cast->fBeamCurrent= value_bcm->fBeamCurrent;
      }
    } else {
      TString loc="Standard exception from QwBCM::operato= :"+
        value.GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }
//   std::cout<<" to be copied \n";
//   value.Print();
//   std::cout<<" copied \n";
//   this->Print();

  return *this;
}

template<typename T>
QwBCM<T>& QwBCM<T>::operator+= (const QwBCM<T> &value)
{
  if (this->GetElementName()!="")
    {
      this->fBeamCurrent+=value.fBeamCurrent;
    }
  return *this;
}

template<typename T>
VQwBCM& QwBCM<T>::operator+= (const VQwBCM &value)
{
  try {
    if(typeid(value)==typeid(*this)) {
      // std::cout<<" Here in QwBCM::operator+= \n";
      if (this->GetElementName()!="") {
        const QwBCM<T>* value_bcm = dynamic_cast<const QwBCM<T>* >(&value);
        QwBCM<T>* this_cast = dynamic_cast<QwBCM<T>* >(this);
        this_cast->fBeamCurrent+=value_bcm->fBeamCurrent;
      }
    } else {
      TString loc="Standard exception from QwBCM::operator+= :"+
        value.GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }

 return *this;
}

template<typename T>
VQwBCM& QwBCM<T>::operator-= (const VQwBCM &value)
{
  if (this->GetElementName()!="")
    {
      const QwBCM<T>* value_bcm = dynamic_cast<const QwBCM<T>* >(&value);
      QwBCM<T>* this_cast = dynamic_cast<QwBCM<T>* >(this);
      this_cast->fBeamCurrent-=value_bcm->fBeamCurrent;
    }
  return *this;
}

template<typename T>
QwBCM<T>& QwBCM<T>::operator-= (const QwBCM<T> &value)
{
  try {
    if(typeid(value)==typeid(*this)) {
      // std::cout<<" Here in QwBCM::operator-= \n";
      if (this->GetElementName()!="") {
        const QwBCM<T>* value_bcm = dynamic_cast<const QwBCM<T>* >(&value);
        QwBCM<T>* this_cast = dynamic_cast<QwBCM<T>* >(this);
        this_cast->fBeamCurrent-=value_bcm->fBeamCurrent;
      }
    } else {
      TString loc="Standard exception from QwBCM::operator-= :"+
        value.GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }
  return *this;
}

template<typename T>
void QwBCM<T>::Ratio(const VQwBCM &numer, const VQwBCM &denom)
{
  Ratio(*dynamic_cast<const QwBCM<T>* >(&numer),
      *dynamic_cast<const QwBCM<T>* >(&denom));
}

template<typename T>
void QwBCM<T>::Ratio(const QwBCM<T> &numer, const QwBCM<T> &denom)
{
  //  std::cout<<"QwBCM::Ratio element name ="<<GetElementName()<<" \n";
  if (this->GetElementName()!="")
    {
      //  std::cout<<"here in \n";
      this->fBeamCurrent.Ratio(numer.fBeamCurrent,denom.fBeamCurrent);
    }
}

template<typename T>
void QwBCM<T>::Scale(Double_t factor)
{
  fBeamCurrent.Scale(factor);
}

template<typename T>
void QwBCM<T>::CalculateRunningAverage()
{
  fBeamCurrent.CalculateRunningAverage();
}

template<typename T>
Bool_t QwBCM<T>::CheckForBurpFail(const VQwDataElement *ev_error){
  Short_t i=0;
  Bool_t burpstatus = kFALSE;
  //QwError << "************* " << this->GetElementName() << "  <<<this, event>>>  " << ev_error->GetElementName() << " *****************" << QwLog::endl;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      //std::cout<<" Here in VQwBCM::CheckForBurpFail \n";
      if (this->GetElementName()!="") {
        const QwBCM<T>* value_bcm = dynamic_cast<const QwBCM<T>* >(ev_error);
        burpstatus |= fBeamCurrent.CheckForBurpFail(&(value_bcm->fBeamCurrent)); 
      }
    } else {
      TString loc="Standard exception from QwBCM::CheckForBurpFail :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }
  return burpstatus;
}

template<typename T>
void QwBCM<T>::AccumulateRunningSum(const VQwBCM& value, Int_t count, Int_t ErrorMask) {
  fBeamCurrent.AccumulateRunningSum(
      dynamic_cast<const QwBCM<T>* >(&value)->fBeamCurrent, count, ErrorMask);
}

template<typename T>
void QwBCM<T>::DeaccumulateRunningSum(VQwBCM& value, Int_t ErrorMask) {
  fBeamCurrent.DeaccumulateRunningSum(dynamic_cast<QwBCM<T>* >(&value)->fBeamCurrent, ErrorMask);
}
template<typename T>
void QwBCM<T>::PrintValue() const
{
  fBeamCurrent.PrintValue();
}

template<typename T>
void QwBCM<T>::PrintInfo() const
{
  std::cout << "QwVQWK_Channel Info " << std::endl;
  fBeamCurrent.PrintInfo();
}

/********************************************************/
template<typename T>
void QwBCM<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (this->GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fBeamCurrent.ConstructHistograms(folder, prefix);
    }
}

template<typename T>
void QwBCM<T>::FillHistograms()
{
  if (this->GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fBeamCurrent.FillHistograms();
    }
}

template<typename T>
void QwBCM<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip
  } else
    {
      fBeamCurrent.ConstructBranchAndVector(tree, prefix,values);
    }
}

template<typename T>
void  QwBCM<T>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fBeamCurrent.ConstructBranch(tree, prefix);
      // this functions doesn't do anything yet
    }
}

template<typename T>
void  QwBCM<T>::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;

  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {

      //QwMessage <<" QwBCM "<<devicename<<QwLog::endl;
      if (modulelist.HasValue(devicename)){
	fBeamCurrent.ConstructBranch(tree, prefix);
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
      }
      // this functions doesn't do anything yet
    }
}

template<typename T>
void QwBCM<T>::FillTreeVector(std::vector<Double_t> &values) const
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fBeamCurrent.FillTreeVector(values);
      // this functions doesn't do anything yet
    }
}

#ifdef __USE_DATABASE__
template<typename T>
std::vector<QwDBInterface> QwBCM<T>::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  fBeamCurrent.AddEntriesToList(row_list);
  return row_list;
}

template<typename T>
std::vector<QwErrDBInterface> QwBCM<T>::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  fBeamCurrent.AddErrEntriesToList(row_list);
  return row_list;
}
#endif // __USE_DATABASE__

template<typename T>
Double_t QwBCM<T>::GetValue()
{
  return fBeamCurrent.GetValue();
}


template<typename T>
Double_t QwBCM<T>::GetValueError()
{
  return fBeamCurrent.GetValueError();
}

template<typename T>
Double_t QwBCM<T>::GetValueWidth()
{
  return fBeamCurrent.GetValueWidth();
}

template class QwBCM<QwVQWK_Channel>;
template class QwBCM<QwADC18_Channel>;
template class QwBCM<QwSIS3801_Channel>;
template class QwBCM<QwSIS3801D24_Channel>;
template class QwBCM<QwMollerADC_Channel>;
/********************************************************\
* File: QwClock.h                                        *
*                                                        *
* Author: Juan Carlos Cornejo <cornejo@jlab.org>         *
* Time-stamp: <2011-06-16>                               *
\********************************************************/

#include "QwClock.h"

// System headers
#include <stdexcept>

// Qweak database headers
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif // __USE_DATABASE__

//  Qweak types that we want to use in this template
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"

/********************************************************/
template<typename T>
void QwClock<T>::SetPedestal(Double_t pedestal)
{
  fPedestal=pedestal;
  fClock.SetPedestal(fPedestal);
}

template<typename T>
void QwClock<T>::SetCalibrationFactor(Double_t calib)
{
  fCalibration=calib;
  fClock.SetCalibrationFactor(fCalibration);
}
/********************************************************/
template<typename T>
void  QwClock<T>::InitializeChannel(TString subsystem, TString name, TString datatosave, TString type){
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  if (type.Length() > 0)
    SetModuleType(type);
  fClock.InitializeChannel(subsystem, "QwClock", name, datatosave);
  fClock.SetNeedsExternalClock(kFALSE);
  SetElementName(name);
  //set default limits to event cuts
  fLLimit=0;//init two timits
  fULimit=0;//init two timits
}
/********************************************************/
template<typename T>
void QwClock<T>::ClearEventData()
{
  fClock.ClearEventData();
  return;
}
/********************************************************/
//template<typename T>
// void QwClock<T>::SetEventData(Double_t* block, UInt_t sequencenumber)
// {
//   fClock.SetEventData(block, sequencenumber);
//   return;
// }
/********************************************************/
// template<typename T>
// void QwClock<T>::SetEventNumber(int event)
// {
//   fClock.SetEventNumber(event);
//   return;
// };
/********************************************************/
template<typename T>
void QwClock<T>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fClock.EncodeEventData(buffer);
}
/********************************************************/
template<typename T>
void QwClock<T>::ProcessEvent()
{
  return;
}
/********************************************************/
template<typename T>
Bool_t QwClock<T>::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=fClock.ApplyHWChecks();//will check for HW consistancy and return the error code (=0 is HW good)
  eventokay=(deviceerror & 0x0);//if no HW error return true

  return eventokay;
}
/********************************************************/

template<typename T>
void QwClock<T>::SetSingleEventCuts(UInt_t errorflag, Double_t LL, Double_t UL, Double_t stability, Double_t burplevel){
  //set the unique tag to identify device type (bcm,bpm & etc)
  errorflag|=kBCMErrorFlag;
  QwMessage<<"QwClock Error Code passing to QwVQWK_Ch "<<errorflag<<QwLog::endl;
  fClock.SetSingleEventCuts(errorflag,LL,UL,stability,burplevel);

}

template<typename T>
void QwClock<T>::SetDefaultSampleSize(Int_t sample_size){
  fClock.SetDefaultSampleSize((size_t)sample_size);
}


/********************************************************/
template<typename T>
Bool_t QwClock<T>::ApplySingleEventCuts(){
  //std::cout<<" QwClock::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;

  if (fClock.ApplySingleEventCuts()){
    status=kTRUE;
  }
  else{

    if (bDEBUG) std::cout<<" evnt cut failed:-> set limit "<<fULimit<<" harware sum  "<<fClock.GetValue();
    status&=kFALSE;
  }
  return status;
}

/********************************************************/

template<typename T>
void QwClock<T>::PrintErrorCounters() const{// report number of events failed due to HW and event cut faliure
  fClock.PrintErrorCounters();
}



template<typename T>
Int_t QwClock<T>::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  UInt_t words_read = fClock.ProcessEvBuffer(buffer,word_position_in_buffer);
  this->ApplyHWChecks();
  return words_read;
}
/********************************************************/
template<typename T>
QwClock<T>& QwClock<T>::operator= (const QwClock<T> &value)
{
//   std::cout<<" Here in QwClock::operator= \n";
  if (this->GetElementName()!="")
    {
      this->fClock=value.fClock;
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
VQwClock& QwClock<T>::operator= (const VQwClock &value)
{
  try {
    if(typeid(value)==typeid(*this)) {
      // std::cout<<" Here in QwClock::operator= \n";
      if (this->GetElementName()!="") {
        const QwClock<T>* value_bcm = dynamic_cast<const QwClock<T>* >(&value);
        QwClock<T>* this_cast = dynamic_cast<QwClock<T>* >(this);
        this_cast->fClock= value_bcm->fClock;
        this_cast->fPedestal=value_bcm->fPedestal;
        this_cast->fCalibration=value_bcm->fCalibration;
      }
    } else {
      TString loc="Standard exception from QwClock::operato= :"+
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
QwClock<T>& QwClock<T>::operator+= (const QwClock<T> &value)
{
  if (this->GetElementName()!="")
    {
      this->fClock+=value.fClock;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}

template<typename T>
VQwClock& QwClock<T>::operator+= (const VQwClock &value)
{
  try {
    if(typeid(value)==typeid(*this)) {
      // std::cout<<" Here in QwClock::operator+= \n";
      if (this->GetElementName()!="") {
        const QwClock<T>* value_bcm = dynamic_cast<const QwClock<T>* >(&value);
        QwClock<T>* this_cast = dynamic_cast<QwClock<T>* >(this);
        this_cast->fClock+=value_bcm->fClock;
        this_cast->fPedestal+=value_bcm->fPedestal;
        this_cast->fCalibration=0;
      }
    } else {
      TString loc="Standard exception from QwClock::operator+= :"+
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
VQwClock& QwClock<T>::operator-= (const VQwClock &value)
{
  if (this->GetElementName()!="")
    {
      const QwClock<T>* value_bcm = dynamic_cast<const QwClock<T>* >(&value);
      QwClock<T>* this_cast = dynamic_cast<QwClock<T>* >(this);
      this_cast->fClock-=value_bcm->fClock;
      this_cast->fPedestal-=value_bcm->fPedestal;
      this_cast->fCalibration=0;
    }
  return *this;
}

template<typename T>
QwClock<T>& QwClock<T>::operator-= (const QwClock<T> &value)
{
  try {
    if(typeid(value)==typeid(*this)) {
      // std::cout<<" Here in QwClock::operator-= \n";
      if (this->GetElementName()!="") {
        const QwClock<T>* value_bcm = dynamic_cast<const QwClock<T>* >(&value);
        QwClock<T>* this_cast = dynamic_cast<QwClock<T>* >(this);
        this_cast->fClock-=value_bcm->fClock;
        this_cast->fPedestal-=value_bcm->fPedestal;
        this_cast->fCalibration=0;
      }
    } else {
      TString loc="Standard exception from QwClock::operator-= :"+
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
void QwClock<T>::Ratio(const VQwClock &numer, const VQwClock &denom)
{
  Ratio(*dynamic_cast<const QwClock<T>* >(&numer),
      *dynamic_cast<const QwClock<T>* >(&denom));
}

template<typename T>
void QwClock<T>::Ratio(const QwClock<T> &numer, const QwClock<T> &denom)
{
  //  std::cout<<"QwClock::Ratio element name ="<<GetElementName()<<" \n";
  if (this->GetElementName()!="")
    {
      //  std::cout<<"here in \n";
      this->fClock.Ratio(numer.fClock,denom.fClock);
      this->fPedestal=0;
      this->fCalibration=0;
    }
  return;
}

template<typename T>
void QwClock<T>::Scale(Double_t factor)
{
  fClock.Scale(factor);
  return;
}

template<typename T>
void QwClock<T>::CalculateRunningAverage(){
  fClock.CalculateRunningAverage();
}

template<typename T>
void QwClock<T>::AccumulateRunningSum(const VQwClock& value, Int_t count, Int_t ErrorMask) {
  fClock.AccumulateRunningSum(
      dynamic_cast<const QwClock<T>* >(&value)->fClock, count, ErrorMask);
}

template<typename T>
void QwClock<T>::DeaccumulateRunningSum(VQwClock& value, Int_t ErrorMask) {
  fClock.DeaccumulateRunningSum(
      dynamic_cast<const QwClock<T>* >(&value)->fClock, ErrorMask);
}

template<typename T>
void QwClock<T>::PrintValue() const
{
  fClock.PrintValue();
}

template<typename T>
void QwClock<T>::PrintInfo() const
{
  std::cout << "QwVQWK_Channel Info " << std::endl;
  fClock.PrintInfo();
}

/********************************************************/
template<typename T>
void QwClock<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (this->GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fClock.ConstructHistograms(folder, prefix);
    }
  return;
}

template<typename T>
void QwClock<T>::FillHistograms()
{
  if (this->GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fClock.FillHistograms();
    }


  return;
}

template<typename T>
void QwClock<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip
  } else
    {
      fClock.ConstructBranchAndVector(tree, prefix,values);
    }
  return;
}

template<typename T>
void  QwClock<T>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fClock.ConstructBranch(tree, prefix);
      // this functions doesn't do anything yet
    }
  return;
}

template<typename T>
void  QwClock<T>::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;

  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {

      //QwMessage <<" QwClock "<<devicename<<QwLog::endl;
      if (modulelist.HasValue(devicename)){
	fClock.ConstructBranch(tree, prefix);
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
      }
      // this functions doesn't do anything yet
    }
  return;
}

template<typename T>
void QwClock<T>::FillTreeVector(std::vector<Double_t> &values) const
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fClock.FillTreeVector(values);
      // this functions doesn't do anything yet
    }
  return;
}

#ifdef __USE_DATABASE__
/********************************************************/
template<typename T>
std::vector<QwDBInterface> QwClock<T>::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  fClock.AddEntriesToList(row_list);
  return row_list;
}
#endif // __USE_DATABASE__

template class QwClock<QwVQWK_Channel>; 
template class QwClock<QwSIS3801_Channel>; 
template class QwClock<QwSIS3801D24_Channel>; 

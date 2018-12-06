#include "QwCombinedPMT.h"

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
void QwCombinedPMT<T>::Add(QwIntegrationPMT<T>* pmt, Double_t weight)
{
  //std::cout<<"QwCombinedPMT: Got "<<pmt->GetElementName()<<"  and weight ="<<weight<<"\n";
  fElement.push_back(pmt);
  fWeights.push_back(weight);
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::InitializeChannel(TString name, TString datatosave)
{
  SetElementName(name);
  //SetPedestal(0.);
  //SetCalibrationFactor(1.);
  if (datatosave=="raw") fDataToSave=kRaw;
  else
    if (datatosave=="derived") fDataToSave=kDerived;

  fSumADC.InitializeChannel(name, datatosave);
  SetBlindability(kTRUE);
  return;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::InitializeChannel(TString subsystemname, TString name, TString datatosave)
{
  SetElementName(name);
  //SetPedestal(0.);
  //SetCalibrationFactor(1.);
  if (datatosave=="raw") fDataToSave=kRaw;
  else
    if (datatosave=="derived") fDataToSave=kDerived;

  fSumADC.InitializeChannel(subsystemname, "QwCombinedPMT", name, datatosave);
  SetBlindability(kTRUE);

  return;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::LinkChannel(TString name)
{
  Bool_t local_debug = false;
  SetElementName(name);
//  TString sumstr = name+TString("_sum");
  TString sumstr = name+TString("");
  fSumADC.SetElementName(sumstr);
//   TString avgstr = name+TString("_avg");
//   fAvgADC.SetElementName(avgstr);
  if(local_debug) std::cout<<"linked combined PMT channel "<< GetElementName()<<std::endl;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::ClearEventData()
{
  fSumADC.ClearEventData();
  //fAvgADC.ClearEventData();
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{

}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fSumADC.SetEventData(block, sequencenumber);
//  fAvgADC.SetEventData(block, sequencenumber);
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::CalculateSumAndAverage()
{
  Bool_t ldebug = kFALSE;
  Double_t  total_weights=0.0;

  fSumADC.ClearEventData();
  static QwIntegrationPMT<T> tmpADC("tmpADC");

  for (size_t i=0;i<fElement.size();i++)
    {
      //std::cout<<"=========fElement["<<i<<"]=========="<<std::endl;
      //fElement[i]->Print();
      tmpADC = *(fElement[i]);
      //std::cout<<"=========tmpADC========="<<std::endl;
      //tmpADC->Print();
      tmpADC.Scale(fWeights[i]);
      fSumADC += tmpADC;
      total_weights += fWeights[i];
    }

//   fAvgADC = fSumADC;
//   if (total_weights!=0.0)
//     fAvgADC.Scale(1/total_weights);

   if (total_weights!=0.0)
     fSumADC.Scale(1/total_weights);


  if (ldebug)
    {
      std::cout<<"QwCombinedPMT<T>::CalculateAverage()"<<std::endl;
//      fAvgADC.PrintInfo();
      fSumADC.PrintInfo();

//      std::cout<<"QwCombinedPMT: "<<GetElementName()
//      <<"\nweighted average of hardware sums = "<<fAvgADC.GetValue()<<"\n";
      std::cout<<"QwCombinedPMT: "<<GetElementName()
      <<"\nweighted average of hardware sums = "<<fSumADC.GetValue()<<"\n";

      for (size_t i=0;i<4;i++)
        {
          std::cout<<"weighted average of block["<<i<<"] = "<<fSumADC.GetValue(i+1)<<"\n";
        }
    }

}

/********************************************************/
template<typename T>
Bool_t QwCombinedPMT<T>::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  return eventokay;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::SetSingleEventCuts(
        UInt_t errorflag,
        Double_t LL /* = 0 */,  // FIXME this function had default arguments but not allowed
        Double_t UL /* = 0 */,
        Double_t stability /* = 0 */)
{
  //set the unique tag to identify device type (Int.PMT & Comb. PMT)
  //errorflag|=kPMTErrorFlag;
  QwMessage<<"QwCombinedPMT Error Code passing to QwIntegrationPMT "<<errorflag<<QwLog::endl;
  fSumADC.SetSingleEventCuts(errorflag,LL,UL,stability);
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::ProcessEvent()
{
//Calculate the weigted averages of the hardware sum and each of the four blocks.
  CalculateSumAndAverage();
  //fSumADC.ProcessEvent(); //This is not necessary for combined devices-Rakitha 11-15-2010
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::SetDefaultSampleSize(Int_t sample_size)
{
  fSumADC.SetDefaultSampleSize((size_t)sample_size);
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::PrintErrorCounters() const
{
  // report number of events failed due to HW and event cut faliure
  fSumADC.PrintErrorCounters();
}

/********************************************************/
template<typename T>
UInt_t QwCombinedPMT<T>::UpdateErrorFlag()
{
  for (size_t i=0;i<fElement.size();i++) {
    fSumADC.UpdateErrorFlag(fElement[i]);
  }
  return GetEventcutErrorFlag();
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::UpdateErrorFlag(const QwCombinedPMT *ev_error)
{
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwCombinedPMT<T>::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
	fSumADC.UpdateErrorFlag(&(ev_error->fSumADC));
      }
    } else {
      TString loc="Standard exception from QwCombinedPMT<T>::UpdateErrorFlag :"+
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
Bool_t QwCombinedPMT<T>::ApplySingleEventCuts()
{
  return fSumADC.ApplySingleEventCuts();
}

/********************************************************/
template<typename T>
Int_t QwCombinedPMT<T>::ProcessEvBuffer(
        UInt_t* buffer,
        UInt_t word_position_in_buffer,
        UInt_t subelement)
{
  return 0;
}

/********************************************************/
template<typename T>
QwCombinedPMT<T>& QwCombinedPMT<T>::operator= (const QwCombinedPMT<T> &value)
{
  //std::cout<<"Calling QwCombinedPMT<T>::operator="<<std::endl;
  if ( (GetElementName()!="") && (this->fElement.size()==value.fElement.size()) )
    {
      for (size_t i=0; i<value.fElement.size(); i++)
        {
          //std::cout<<"value.fWeight["<<i<<"]="<<value.fWeights[i]<<std::endl;
          //std::cout<<"=========value.fElement["<<i<<"]=========="<<std::endl;
          //value.fElement[i]->Print();
          (this->fElement[i]) = (value.fElement[i]);
          this->fWeights[i] = value.fWeights[i];
          //std::cout<<"this->fWeight["<<i<<"]="<<fWeights[i]<<std::endl;
          //std::cout<<"=========this->fElement["<<i<<"]=========="<<std::endl;
          //fElement[i]->Print();
        }

      this->fSumADC=value.fSumADC;
//      this->fAvgADC=value.fAvgADC;

      //std::cout<<"value.fSumADC"<<std::endl;
      //value.fSumADC.Print();
      //std::cout<<"this->fSumADC"<<std::endl;
      //fSumADC.Print();
    }

  return *this;
}

/********************************************************/
template<typename T>
QwCombinedPMT<T>& QwCombinedPMT<T>::operator+= (const QwCombinedPMT<T> &value)
{
  //std::cout<<"Calling QwCombinedPMT<T>::operator+="<<std::endl;
  if ( (GetElementName()!="") && (this->fElement.size()==value.fElement.size()) )
    {
      for (size_t i=0; i<value.fElement.size(); i++)
        {
          *(this->fElement[i]) += *(value.fElement[i]);
          this->fWeights[i] += value.fWeights[i];
        }

      this->fSumADC+=value.fSumADC;
//      this->fAvgADC+=value.fAvgADC;
    }

  return *this;
}

/********************************************************/
template<typename T>
QwCombinedPMT<T>& QwCombinedPMT<T>::operator-= (const QwCombinedPMT<T> &value)
{
  //std::cout<<"Calling QwCombinedPMT<T>::operator-="<<std::endl;
  if ( (GetElementName()!="") && (this->fElement.size()==value.fElement.size()) )
    {
      for (size_t i=0; i<value.fElement.size(); i++)
        {
          *(this->fElement[i]) -= *(value.fElement[i]);
          this->fWeights[i] -= value.fWeights[i];
        }

      this->fSumADC-=value.fSumADC;
//      this->fAvgADC-=value.fAvgADC;
      //value.fSumADC.Print();
      //fSumADC.Print();
    }

  return *this;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::Sum(QwCombinedPMT<T> &value1, QwCombinedPMT<T> &value2)
{
  //std::cout<<"Calling QwCombinedPMT<T>::Sum"<<std::endl;
  this->fSumADC =  value1.fSumADC;
//  this->fAvgADC =  value1.fAvgADC;
  this->fSumADC += value2.fSumADC;
//  this->fAvgADC += value2.fAvgADC;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::AccumulateRunningSum(const QwCombinedPMT<T>& value)
{
  fSumADC.AccumulateRunningSum(value.fSumADC);
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::DeaccumulateRunningSum(QwCombinedPMT<T>& value)
{
  fSumADC.DeaccumulateRunningSum(value.fSumADC);
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::Difference(QwCombinedPMT<T> &value1, QwCombinedPMT<T> &value2)
{
  //std::cout<<"Calling QwCombinedPMT<T>::Difference="<<std::endl;
  this->fSumADC =  value1.fSumADC;
//  this->fAvgADC =  value1.fAvgADC;
  this->fSumADC -= value2.fSumADC;
//  this->fAvgADC -= value2.fAvgADC;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::Ratio(QwCombinedPMT<T> &numer, QwCombinedPMT<T> &denom)
{
  //std::cout<<"Calling QwCombinedPMT<T>::Ratio"<<std::endl;
  fSumADC.Ratio(numer.fSumADC,denom.fSumADC);
//  fAvgADC.Ratio(numer.fAvgADC,denom.fAvgADC);
  return;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::Scale(Double_t factor)
{
  fSumADC.Scale(factor);
//  fAvgADC.Scale(factor);
  return;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::Normalize(VQwDataElement* denom)
{
  fSumADC.Normalize(denom);

  return;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::CalculateRunningAverage()
{
  fSumADC.CalculateRunningAverage();
//  fAvgADC.CalculateRunningAverage();
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::Blind(const QwBlinder *blinder)
{
  fSumADC.Blind(blinder);
//  fAvgADC.Blind(blinder);
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::Blind(const QwBlinder *blinder, const QwCombinedPMT<T>& yield)
{
  fSumADC.Blind(blinder, yield.fSumADC);
//  fAvgADC.Blind(blinder, yield.fAvgADC);
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::PrintValue() const
{
  fSumADC.PrintValue();
//  fAvgADC.PrintValue();
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::PrintInfo() const
{
  fSumADC.PrintInfo();
//  fAvgADC.PrintInfo();
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      TString sumprefix = prefix+TString("");
      fSumADC.ConstructHistograms(folder, sumprefix);
//      TString avgprefix = prefix+TString("");
//      fAvgADC.ConstructHistograms(folder, avgprefix);
    }
  return;

}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fSumADC.FillHistograms();
//      fAvgADC.FillHistograms();
    }
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip
    }
  else
    {
      TString sumprefix =  prefix+"";
      fSumADC.ConstructBranchAndVector(tree, sumprefix,values);

    }
  return;
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip
    }
  else
    {
      TString sumprefix =  prefix+"";
      fSumADC.ConstructBranch(tree, sumprefix);
    }
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile &modulelist)
{
  TString devicename;
  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip
    }
  else
    {
      if (modulelist.HasValue(devicename)){
	TString sumprefix =  prefix+"";
	fSumADC.ConstructBranch(tree, sumprefix);
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
      }
    }
}

/********************************************************/
template<typename T>
void QwCombinedPMT<T>::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  } else {
    fSumADC.FillTreeVector(values);
//  fAvgADC.FillTreeVector(values);
  }
}

#ifdef __USE_DATABASE__

/********************************************************/
template<typename T>
std::vector<QwDBInterface>  QwCombinedPMT<T>::GetDBEntry()
{
  return fSumADC.GetDBEntry();
}

/********************************************************/
template<typename T>
std::vector<QwErrDBInterface>  QwCombinedPMT<T>::GetErrDBEntry()
{
  return fSumADC.GetErrDBEntry();
}

#endif


template class QwCombinedPMT<QwVQWK_Channel>;

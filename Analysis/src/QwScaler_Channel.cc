/**********************************************************\
* File: QwScaler_Channel.cc                              *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#include <boost/algorithm/string.hpp>

#include "QwScaler_Channel.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

#include <QwLog.h>


const Bool_t VQwScaler_Channel::kDEBUG = kFALSE;


/********************************************************/
void  VQwScaler_Channel::InitializeChannel(TString name, TString datatosave)
{
  fNormChannelPtr = NULL;
  fNeedsExternalClock = kFALSE;
  fIsDifferentialScaler = false;

  SetElementName(name);
  SetDataToSave(datatosave);
  SetNumberOfDataWords(1);  //Scaler - single word, 32 bits
  SetNumberOfSubElements(1);

  //  Default mockdata parameters
  SetRandomEventParameters(300.0, 50.0);

  fHeader = 0;
  fValue_Raw_Old = 0;
  fValue_Raw  = 0;
  fValue      = 0.0;
  fValueM2    = 0.0;
  fValueError = 0.0;
  fPedestal   = 0.0;
  fCalibrationFactor = 1.0;

  fClockNormalization = 1.0;

  fTreeArrayIndex = 0;
  fTreeArrayNumEntries =0;

  fNumEvtsWithHWErrors=0;//init error counters
  fNumEvtsWithEventCutsRejected=0;//init error counters

  fErrorFlag = 0;
  fErrorConfigFlag=0;
  fGoodEventCount = 0;
  return;
};

/********************************************************/

void VQwScaler_Channel::InitializeChannel(TString subsystem, TString instrumenttype, TString name, TString datatosave){
  InitializeChannel(name,datatosave);
  SetSubsystemName(subsystem);
  SetModuleType(instrumenttype);
}

void VQwScaler_Channel::ClearEventData()
{
  fHeader      = 0;
  fValue_Raw   = 0;
  fValue       = 0.0;
  fValueM2     = 0.0;
  fValueError  = 0.0;

  fGoodEventCount  = 0;
  fErrorFlag = 0;
}

void VQwScaler_Channel::RandomizeEventData(int helicity, double time)
{
  // Calculate drift (if time is not specified, it stays constant at zero)
  Double_t drift = 0.0;
  for (UInt_t i = 0; i < fMockDriftFrequency.size(); i++) {
    drift += fMockDriftAmplitude[i] * sin(2.0 * Qw::pi * fMockDriftFrequency[i] * time + fMockDriftPhase[i]);
  }

  Double_t value = fMockGaussianMean * (1 + helicity * fMockAsymmetry)
    + fMockGaussianSigma * GetRandomValue()
    + drift;

  fValue     = value;
  fValue_Raw = Int_t(value / fCalibrationFactor + fPedestal);
}

void VQwScaler_Channel::SmearByResolution(double resolution)
{
  fValue = resolution*GetRandomValue();
  fValueM2 = 0.0;
  SetRawEventData();
}


/*!  Static member function to return the word offset within a data buffer
 *   given the module number index and the channel number index.
 *   @param scalerindex   Scaler index within this buffer; counts from 1
 *   @param wordindex  Word index within this scaler; counts from 1
 *   @param header  Number of header words; normally is 1
 *   @return   The number of words offset to the beginning of this
 *             scaler word from the beginning of the buffer.
 */
Int_t VQwScaler_Channel::GetBufferOffset(Int_t scalerindex, Int_t wordindex, UInt_t header)
{
  Int_t offset = -1;
  Int_t kMaxWords = 32; // usually the scalers have 32 data words starting from 0

  if (scalerindex<0 ){
    QwError << "QwScaler_Channel::GetBufferOffset:  Invalid scaler index,"
	    << scalerindex
	    << ".  Must be zero or greater."
	    << QwLog::endl;
  } else if (scalerindex<0 || wordindex>kMaxWords){
    QwError << "QwScaler_Channel::GetBufferOffset:  Invalid word index,"
	    << wordindex
	    << ".  Must be in range [0," << kMaxWords << "]."
	    << QwLog::endl;
  } else {
    offset = (header + kMaxWords)*scalerindex + header + wordindex ;
  }
  return offset;
}


void VQwScaler_Channel::SetEventData(Double_t value){

  this->fValue     = value;
  this->fValue_Raw = (UInt_t)value;
  //std::cout<<"fValue is set to: value = "<<value<<std::endl;

}

void VQwScaler_Channel::LoadChannelParameters(QwParameterFile &paramfile){
  std::string varvalue;
  if (paramfile.ReturnValue("normclock",varvalue)){
    boost::to_lower(varvalue);
    SetExternalClockName(varvalue);
    fNeedsExternalClock = kTRUE;
  }
};

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Fill in with zero.
    buffer.push_back( 0 );
  } else {
    buffer.push_back( ((this->fValue_Raw<<data_shift)&data_mask) );
    //std::cout<<"this->fValue="<<this->fValue<<std::endl;
  }
}


template<unsigned int data_mask, unsigned int data_shift>
Int_t QwScaler_Channel<data_mask,data_shift>::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,
							      UInt_t index)
{
  UInt_t words_read = 0;
  if (IsNameEmpty()){
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
      words_read = fNumberOfDataWords;
  } else if (num_words_left >= fNumberOfDataWords) {
    fHeader    = (buffer[0] & ~data_mask);
    fValue_Raw = ((buffer[0] & data_mask) >> data_shift);
    fValue     = fCalibrationFactor * (Double_t(fValue_Raw) - Double_t(fValue_Raw_Old) - fPedestal);
    words_read = fNumberOfDataWords;

    // Store old raw value for differential scalers
    if (IsDifferentialScaler())
      fValue_Raw_Old = fValue_Raw;
    else
      fValue_Raw_Old = 0;

  } else {
    //QwError << "QwScaler_Channel::ProcessEvBuffer: Not enough words!"<< QwLog::endl;
  }
  return words_read;
}


void VQwScaler_Channel::ProcessEvent()
{
  if (NeedsExternalClock()){
    if(fNormChannelPtr){
      Double_t time = fNormChannelPtr->GetValue();
      //QwError << "VQwScaler_Channel::ProcessEvent() "<<GetElementName()<<" "<< fValue_Raw<< " "<< fValue<<" "<<fCalibrationFactor<<" "<< fPedestal<<QwLog::endl;
      fValue = fCalibrationFactor * (Double_t(fValue_Raw)/time - fPedestal);
    } else {
      QwWarning << "VQwScaler_Channel::ProcessEvent:  "
		<< "Missing the reference clock, "
		<< fNormChannelName
		<< ", for data element "
		<< GetElementName()
		<< QwLog::endl;
    }
  }
}


void VQwScaler_Channel::PrintValue() const
{
  QwMessage << std::setprecision(4)
            << std::setw(18) << std::left << GetSubsystemName()  << " "
            << std::setw(18) << std::left << GetModuleType()     << " "
            << std::setw(18) << std::left << GetElementName()    << " "
	    << std::setw(12) << std::left << GetValue()          << "  +/-  "
	    << std::setw(12) << std::left << GetValueError()     << "  sig  "
            << std::setw(12) << std::left << GetValueWidth()     << " "
            << std::setw(12) << std::left << GetGoodEventCount() << " "
	    << QwLog::endl;
}

void VQwScaler_Channel::PrintInfo() const
{
  QwMessage << "***************************************" << QwLog::endl;
  QwMessage << "QwScaler channel: " << GetElementName()
            << QwLog::endl;
}

void VQwScaler_Channel::ConstructHistograms(TDirectory *folder, TString &prefix){
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {
    //  Now create the histograms.
    TString basename = prefix + GetElementName();

    fHistograms.resize(1, NULL);
    size_t index=0;
    fHistograms[index]   = gQwHists.Construct1DHist(basename);
    index += 1;
  }
}

void  VQwScaler_Channel::FillHistograms()
{
  size_t index = 0;
  if (IsNameEmpty()) {
    //  This channel is not used, so skip creating the histograms.
  } else {
    if (index < fHistograms.size() && fHistograms[index] != NULL  && fErrorFlag==0)
      fHistograms[index]->Fill(this->fValue);
    index += 1;
  }
}


template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    //  Decide what to store based on prefix
    SetDataToSaveByPrefix(prefix);

    TString basename = prefix(0, (prefix.First("|") >= 0)? prefix.First("|"): prefix.Length()) + GetElementName();
    fTreeArrayIndex  = values.size();

    TString list;
    values.push_back(0.0);
    list = "value/D";
    if (fDataToSave == kMoments) {
      values.push_back(0.0);
      list += ":value_m2/D";
      values.push_back(0.0);
      list += ":value_err/D";
      values.push_back(0.0);
      list += ":num_samples/D";
    }
    values.push_back(0.0);
    list += ":Device_Error_Code/D";
    if(fDataToSave==kRaw){
      values.push_back(0.0);
      list += ":raw/D";
      if ((~data_mask) != 0){
	values.push_back(0.0);
	list += ":header/D"; 
      }
    }
    //std::cout << basename <<": first==" << fTreeArrayIndex << ", last==" << values.size() << std::endl;
    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    if (gQwHists.MatchDeviceParamsFromList(basename.Data()))
      tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
}

void  VQwScaler_Channel::ConstructBranch(TTree *tree, TString &prefix)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();

    tree->Branch(basename, &fValue, basename+"/D");
  }
}

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::FillTreeVector(std::vector<Double_t> &values) const
{
  //std::cout<<"inside QwScaler_Channel::FillTreeVector"<< std::endl;
  if (IsNameEmpty()) {
    //  This channel is not used, so skip setting up the tree.
  } else if (fTreeArrayNumEntries < 0) {
    QwError << "VQwScaler_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	    << fTreeArrayNumEntries << QwLog::endl;
  } else if (fTreeArrayNumEntries == 0) {
    static bool warned = false;
    if (!warned) {
      QwError << "VQwScaler_Channel::FillTreeVector:  fTreeArrayNumEntries=="
              << fTreeArrayNumEntries << " (no branch constructed?)" << QwLog::endl;
      QwError << "Offending element is " << GetElementName() << QwLog::endl;
      warned = true;
    }
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries) {
    QwError << "VQwScaler_Channel::FillTreeVector:  values.size()=="
	    << values.size() << " name: " << fElementName
	    << "; fTreeArrayIndex+fTreeArrayNumEntries=="
            << fTreeArrayIndex << '+' << fTreeArrayNumEntries << '='
	    << fTreeArrayIndex+fTreeArrayNumEntries
	    << QwLog::endl;
  } else {
    size_t index = fTreeArrayIndex;
    values[index++] = this->fValue;
    if (fDataToSave == kMoments) {
      values[index++] = fValueM2;
      values[index++] = fValueError;
      values[index++] = fGoodEventCount;
    }
    values[index++] = this->fErrorFlag;
    if(fDataToSave==kRaw){
      values[index++] = this->fValue_Raw;
      if ((~data_mask) != 0){
	values[index++] = this->fHeader;
      }

    }
    //std::cout << fElementName <<": first==" << fTreeArrayIndex << ", last==" << index << std::endl;
    //std::cout<<"value: "<< this->fValue << std::endl;
    //std::cout <<"index: " << index  << std::endl;
  }
  
}


void VQwScaler_Channel::AssignValueFrom(const VQwDataElement* valueptr){
  const VQwScaler_Channel* tmpptr;
  tmpptr = dynamic_cast<const VQwScaler_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this = *tmpptr;
  } else {
    TString loc="Standard exception from VQwScaler_Channel::AssignValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void VQwScaler_Channel::AddValueFrom(const  VQwHardwareChannel* valueptr)
{
  const VQwScaler_Channel* tmpptr;
  tmpptr = dynamic_cast<const VQwScaler_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this += *tmpptr;
  } else {
    TString loc="Standard exception from VQwScaler_Channel::AddValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void VQwScaler_Channel::SubtractValueFrom(const  VQwHardwareChannel* valueptr)
{
  const VQwScaler_Channel* tmpptr;
  tmpptr = dynamic_cast<const VQwScaler_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this -= *tmpptr;
  } else {
    TString loc="Standard exception from VQwScaler_Channel::SubtractValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void VQwScaler_Channel::MultiplyBy(const VQwHardwareChannel* valueptr)
{
  const VQwScaler_Channel* tmpptr;
  tmpptr = dynamic_cast<const VQwScaler_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this *= *tmpptr;
  } else {
    TString loc="Standard exception from VQwScaler_Channel::MultiplyBy = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}

void VQwScaler_Channel::DivideBy(const VQwHardwareChannel* valueptr)
{
  const VQwScaler_Channel* tmpptr;
  tmpptr = dynamic_cast<const VQwScaler_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this /= *tmpptr;
  } else {
    TString loc="Standard exception from VQwScaler_Channel::DivideBy = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}

VQwScaler_Channel& VQwScaler_Channel::operator=(const VQwScaler_Channel &value)
{
  if(this == &value) return *this;
  if (!IsNameEmpty()) {
    VQwHardwareChannel::operator=(value);
    this->fHeader     = value.fHeader;
    this->fValue_Raw  = value.fValue_Raw;
    this->fValue      = value.fValue;
    this->fValueError = value.fValueError;
    this->fValueM2    = value.fValueM2;
  }
  return *this;
}

void VQwScaler_Channel::AssignScaledValue(const VQwScaler_Channel &value,
				    Double_t scale)
{
  if (!IsNameEmpty()) {
    this->fValue      = value.fValue * scale;
    this->fValueError = value.fValueError;
    this->fValueM2    = value.fValueM2 * scale * scale;
    this->fErrorFlag  = value.fErrorFlag;//error code is updated.
    this->fGoodEventCount = value.fGoodEventCount;
  }
  return;
}


VQwScaler_Channel& VQwScaler_Channel::operator+= (const VQwScaler_Channel &value)
{
  if (!IsNameEmpty()){
    this->fValue           += value.fValue;
    this->fValueM2          = 0.0;
    this->fErrorFlag       |= value.fErrorFlag;//error code is ORed.
  }
  return *this;
}

VQwScaler_Channel& VQwScaler_Channel::operator-= (const VQwScaler_Channel &value)
{
  if (!IsNameEmpty()){
    this->fValue           -= value.fValue;
    this->fValueM2          = 0.0;
    this->fErrorFlag       |= (value.fErrorFlag);//error code is ORed.
  }
  return *this;
}

VQwScaler_Channel& VQwScaler_Channel::operator*= (const VQwScaler_Channel &value)
{
  if (!IsNameEmpty()){
    this->fValue     *= value.fValue;
    fHeader           = 0;
    fValue_Raw        = 0;
    this->fValueM2    = 0.0;
    this->fErrorFlag |= (value.fErrorFlag);//error code is ORed.
  }
  return *this;
}

VQwHardwareChannel& VQwScaler_Channel::operator+=(const VQwHardwareChannel *source)
{
  const VQwScaler_Channel* tmpptr;
  tmpptr = dynamic_cast<const VQwScaler_Channel*>(source);
  if (tmpptr!=NULL){
    *this += *tmpptr;
  } else {
    TString loc="Standard exception from VQwScaler_Channel::operator+= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}
VQwHardwareChannel& VQwScaler_Channel::operator-=(const VQwHardwareChannel *source)
{
  const VQwScaler_Channel* tmpptr;
  tmpptr = dynamic_cast<const VQwScaler_Channel*>(source);
  if (tmpptr!=NULL){
    *this -= *tmpptr;
  } else {
    TString loc="Standard exception from VQwScaler_Channel::operator-= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}
VQwHardwareChannel& VQwScaler_Channel::operator*=(const VQwHardwareChannel *source)
{
  const VQwScaler_Channel* tmpptr;
  tmpptr = dynamic_cast<const VQwScaler_Channel*>(source);
  if (tmpptr!=NULL){
    *this *= *tmpptr;
  } else {
    TString loc="Standard exception from VQwScaler_Channel::operator*= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}
VQwHardwareChannel& VQwScaler_Channel::operator/=(const VQwHardwareChannel *source)
{
  const VQwScaler_Channel* tmpptr;
  tmpptr = dynamic_cast<const VQwScaler_Channel*>(source);
  if (tmpptr!=NULL){
    *this /= *tmpptr;
  } else {
    TString loc="Standard exception from VQwScaler_Channel::operator/= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}

void VQwScaler_Channel::Sum(VQwScaler_Channel &value1, VQwScaler_Channel &value2)
{
  *this =  value1;
  *this += value2;
}

void VQwScaler_Channel::Difference(VQwScaler_Channel &value1, VQwScaler_Channel &value2){
  *this =  value1;
  *this -= value2;
}

void VQwScaler_Channel::Ratio(const VQwScaler_Channel &numer, const VQwScaler_Channel &denom)
{
  if (!IsNameEmpty()){
    *this  = numer;
    *this /= denom;
    
    //  Set the raw values to zero.
    fHeader    = 0;
    fValue_Raw = 0;
    
    // Remaining variables
    fGoodEventCount  = denom.fGoodEventCount;
    fErrorFlag = (numer.fErrorFlag|denom.fErrorFlag);//error code is ORed.    
  }
}

VQwScaler_Channel& VQwScaler_Channel::operator/= (const VQwScaler_Channel &denom)
{
  //  In this function, leave the "raw" variables untouched.
  Double_t ratio;
  Double_t variance;
  if (!IsNameEmpty()){
    // The variances are calculated using the following formula:
    //   Var[ratio] = ratio^2 (Var[numer] / numer^2 + Var[denom] / denom^2)
    //
    // This requires that both the numerator and denominator are non-zero!
    //
    if (this->fValue != 0.0 && denom.fValue != 0.0){
      ratio = (this->fValue) / (denom.fValue);
      variance =  ratio * ratio *
	(this->fValueM2 / this->fValue / this->fValue
	 + denom.fValueM2 / denom.fValue / denom.fValue);
      fValue   = ratio;
      fValueM2 = variance;
    } else if (this->fValue == 0.0) {
      fValue   = 0.0;
      fValueM2 = 0.0;
    } else {
      QwVerbose << "Attempting to divide by zero in " 
		<< GetElementName() << QwLog::endl;
      fValue   = 0.0;
      fValueM2 = 0.0;
    }

    // Remaining variables
    //  Don't change fGoodEventCount.
    //  'OR' the device error codes together.
    fErrorFlag |= denom.fErrorFlag;
  }

  // Nanny
  if (fValue != fValue)
    QwWarning << "Angry Nanny: NaN detected in " << GetElementName() << QwLog::endl;
  return *this;
}

void VQwScaler_Channel::Product(VQwScaler_Channel &numer, VQwScaler_Channel &denom)
{
  if (!IsNameEmpty()){
    fValue = numer.fValue * denom.fValue;
    fHeader    = 0;
    fValue_Raw = 0;
    
    // Remaining variables
    fGoodEventCount  = denom.fGoodEventCount;
    fErrorFlag = (numer.fErrorFlag|denom.fErrorFlag);//error code is ORed.
  }
}


void VQwScaler_Channel::AddChannelOffset(Double_t offset)
{
  if (!IsNameEmpty())
    {
      fValue += offset;
    }
}


void VQwScaler_Channel::Scale(Double_t scale)
{
  if (!IsNameEmpty())
    {
      fValue   *= scale;
      fValueM2 *= scale * scale;
    }
}

void VQwScaler_Channel::DivideBy(const VQwScaler_Channel &denom)
{
  *this /= denom;
}

/********************************************************/
Int_t VQwScaler_Channel::ApplyHWChecks() {
  //  fErrorFlag=0;
  if (bEVENTCUTMODE>0){//Global switch to ON/OFF event cuts set at the event cut file
    //check for the hw_sum is zero
    if (GetRawValue()==0){
      fErrorFlag|=kErrorFlag_ZeroHW;
    }
  }
  return fErrorFlag;
}



Bool_t VQwScaler_Channel::ApplySingleEventCuts()
{
  //std::cout << "Here in VQwScaler_Channel: "<< std::endl; 
  Bool_t status;
  //QwError<<" Single Event Check ! "<<QwLog::endl;
  if (bEVENTCUTMODE>=2){//Global switch to ON/OFF event cuts set at the event cut file
    //std::cout << "Upper : " << fULimit << " , Lower: " << fLLimit << std::endl;
    if (fULimit <  fLLimit){
      // std::cout << "First" << std::endl;
      status=kTRUE;
    } else  if (GetValue()<=fULimit && GetValue()>=fLLimit){
      //std::cout << "Second" << std::endl;
      //QwError<<" Single Event Cut passed "<<GetElementName()<<" "<<GetValue()<<QwLog::endl;
      if (fErrorFlag !=0)
	status=kFALSE;
      else
	status=kTRUE;
    }
    else{
      //std::cout << "Third" << std::endl;
      //QwError<<" Single Event Cut Failed "<<GetElementName()<<" "<<GetValue()<<QwLog::endl;
      if (GetValue()> fULimit)
	fErrorFlag|=kErrorFlag_EventCut_U;
      else
	fErrorFlag|=kErrorFlag_EventCut_L;
      status=kFALSE;
    }

    if (bEVENTCUTMODE==3){
      status=kTRUE; //Update the event cut fail flag but pass the event.
    }

  }
  else{
    status=kTRUE;
    fErrorFlag=0;
  }


  return status;  
}

void VQwScaler_Channel::IncrementErrorCounters()
{
  if ( (kErrorFlag_ZeroHW &  fErrorFlag)==kErrorFlag_ZeroHW){
    fNumEvtsWithHWErrors++; //increment the hw error counter
  }
  if ( ((kErrorFlag_EventCut_L &  fErrorFlag)==kErrorFlag_EventCut_L) 
       || ((kErrorFlag_EventCut_U &  fErrorFlag)==kErrorFlag_EventCut_U)){
    fNumEvtsWithEventCutsRejected++; //increment the event cut error counter
  }
}


void VQwScaler_Channel::AccumulateRunningSum(const VQwScaler_Channel& value, Int_t count, Int_t ErrorMask)
{

  if(count==0){
    count = value.fGoodEventCount;
  }
  
  // Moment calculations
  Int_t n1 = fGoodEventCount;
  Int_t n2 = count;

  // If there are no good events, check whether device HW is good
  if (n2 == 0 && value.fErrorFlag == 0) {
    n2 = 1;
  }

  // If a single event is removed from the sum, check all but stability fail flags
  if (n2 == -1) {
    if ((value.fErrorFlag & ErrorMask) == 0) {
      n2 = -1;
    } else {
      n2 = 0;
    }
  }

  if (ErrorMask ==  kPreserveError){
    //n = 1;
    if (n2 == 0) {
      n2 = 1;
    }
    if (count == -1) {
      n2 = -1;
    }
  }

  // New total number of good events
  Int_t n = n1 + n2;

  // Set up variables
  Double_t M11 = fValue;
  Double_t M12 = value.fValue;
  Double_t M22 = value.fValueM2;
  if (n2 == 0) {
    // no good events for addition
    return;
  } else if (n2 == -1) {
    // simple version for removal of single event from the sum
    fGoodEventCount--;
    if (n > 1) {
      fValue -= (M12 - M11) / n;
      fValueM2 -= (M12 - M11)
        * (M12 - fValue); // note: using updated mean
    } else if (n == 1) {
      fValue -= (M12 - M11) / n;
      fValueM2 -= (M12 - M11)
        * (M12 - fValue); // note: using updated mean
      if (fabs(fValueM2) < 10.*std::numeric_limits<double>::epsilon())
        fValueM2 = 0; // rounding
    } else if (n == 0) {
      fValue -= M12;
      fValueM2 -= M22;
      if (fabs(fValue) < 10.*std::numeric_limits<double>::epsilon())
        fValue = 0; // rounding
      if (fabs(fValueM2) < 10.*std::numeric_limits<double>::epsilon())
        fValueM2 = 0; // rounding
    } else {
      QwWarning << "Running sum has deaccumulated to negative good events." << QwLog::endl;
    }
  } else if (n2 == 1) {
    // simple version for addition of single event
    fGoodEventCount++;
    fValue += (M12 - M11) / n;
    fValueM2 += (M12 - M11) * (M12 - fValue); // note: using updated mean
  } else if (n2 > 1) {
    // general version for addition of multi-event sets
    fGoodEventCount += n2;
    fValue += n2 * (M12 - M11) / n;
    fValueM2 += M22 + n1 * n2 * (M12 - M11) * (M12 - M11) / n;
  }

  // Nanny
  if (fValue != fValue)
    QwWarning << "Angry Nanny: NaN detected in " << GetElementName() << QwLog::endl;
}

void VQwScaler_Channel::CalculateRunningAverage()
{
  //  See notes in QwVQWK_Channel;  we are using:
  //         error = sqrt(M2)/n,
  //  or alternately we could use the unbiased estimator for both
  //  the sigma and error on the mean:
  //          error = sqrt(M2)/(n-1)
  if(fGoodEventCount > 0) {
    fValueError = sqrt(fValueM2) / fGoodEventCount;
  } else {
    fValueError = 0.0;
  }

}

void  VQwScaler_Channel::PrintErrorCounters() const{
    if (fNumEvtsWithHWErrors>0)
      QwMessage << "QwScaler_Channel " << GetElementName()
		<< " had " << fNumEvtsWithHWErrors
		<< " events with a hardware faliure."
		<< QwLog::endl;

    if (fNumEvtsWithEventCutsRejected>0)
      QwMessage << "QwScaler_Channel " << GetElementName()
		<< " had " << fNumEvtsWithEventCutsRejected
		<< " events rejected by Event Cuts."
		<< QwLog::endl;
  }

void VQwScaler_Channel::ScaledAdd(Double_t scale, const VQwHardwareChannel *value){

    const VQwScaler_Channel* input = dynamic_cast<const VQwScaler_Channel*>(value);

    // follows same steps as += but w/ scaling factor
    if (input!=NULL && !IsNameEmpty()){
        this->fValue  += scale * input->fValue;
        this->fValueM2 = 0.0;
	this->fErrorFlag |= (input->fErrorFlag);
    }
}


template<>
VQwHardwareChannel* QwScaler_Channel<0x00ffffff,0>::Clone(VQwDataElement::EDataToSave datatosave) const{
  return new QwScaler_Channel(*this, datatosave);
};
template<>
VQwHardwareChannel* QwScaler_Channel<0xffffffff,0>::Clone(VQwDataElement::EDataToSave datatosave) const{
  return new QwScaler_Channel(*this, datatosave);
};

//  These explicit class template instantiations should be the
//  last thing in this file.  This list should cover all of the
//  types that are typedef'ed in the header file.
template class QwScaler_Channel<0x00ffffff,0>;  // QwSIS3801D24_Channel
template class QwScaler_Channel<0xffffffff,0>;  // QwSIS3801_Channel, etc.



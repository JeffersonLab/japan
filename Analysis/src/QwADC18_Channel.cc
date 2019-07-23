#include "QwADC18_Channel.h"

// System headers
#include <stdexcept>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwBlinder.h"
#include "QwHistogramHelper.h"
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif

const Bool_t QwADC18_Channel::kDEBUG = kFALSE;

const Int_t  QwADC18_Channel::kHeaderWordsPerBank = 1;
const Int_t  QwADC18_Channel::kFooterWordsPerBank = 1;
const Int_t  QwADC18_Channel::kHeaderWordsPerModule = 12;
const Int_t  QwADC18_Channel::kFooterWordsPerModule = 1;
const Int_t  QwADC18_Channel::kDataWordsPerChannel  = 3;
const Int_t  QwADC18_Channel::kMaxChannels          = 4;

const UInt_t QwADC18_Channel::mask31x   = 0x80000000;   // Header bit mask
const UInt_t QwADC18_Channel::mask3029x = 0x60000000;   // Channel number mask
const UInt_t QwADC18_Channel::mask2625x = 0x06000000;   // Divider value mask
const UInt_t QwADC18_Channel::mask2422x = 0x01c00000;   // Data type mask
const UInt_t QwADC18_Channel::mask21x   = 0x00200000;   // Data type 0 value sign mask
const UInt_t QwADC18_Channel::mask200x  = 0x001fffff;   // Data type 0 value field mask
const UInt_t QwADC18_Channel::mask2118x = 0x003c0000;   // Data types 1-2 sample number mask
const UInt_t QwADC18_Channel::mask170x  = 0x0003ffff;   // Data types 1-2 value field mask
const UInt_t QwADC18_Channel::mask150x  = 0x0000ffff;   // Data type 4 value field mask



const Double_t QwADC18_Channel::kTimePerSample = 2.0 * Qw::us; // FIXME

/*!  Conversion factor to translate the average bit count in an ADC
 *   channel into average voltage.
 *   The base factor is roughly 76 uV per count, and zero counts corresponds
 *   to zero voltage.
 *   Store as the exact value for 20 V range, 18 bit ADC.
 */
const Double_t QwADC18_Channel::kADC18_VoltsPerBit = (20./(1<<18)); // FIXME

/*!  Static member function to return the word offset within a data buffer
 *   given the module number index and the channel number index.
 *   @param moduleindex   Module index within this buffer; counts from zero
 *   @param channelindex  Channel index within this module; counts from -1 (header)
 *   @return   The number of words offset to the beginning of this
 *             channel's data from the beginning of the ADC18 buffer.
 */
Int_t QwADC18_Channel::GetBufferOffset(Int_t moduleindex, Int_t channelindex)
{
  Int_t offset = -1;
  if (moduleindex<0 ){
    QwError << "QwADC18_Channel::GetBufferOffset:  Invalid module index,"
            << moduleindex
            << ".  Must be zero or greater."
            << QwLog::endl;
  } else if (channelindex<-1 || channelindex>kMaxChannels){
    QwError << "QwADC18_Channel::GetBufferOffset:  Invalid channel index,"
            << channelindex
            << ".  Must be in range [-1," << kMaxChannels << "]."
            << QwLog::endl;
  } else {
    if (channelindex == -1) {
      // Header
      offset = kHeaderWordsPerBank
             + moduleindex * (kHeaderWordsPerModule + kMaxChannels * kDataWordsPerChannel + kFooterWordsPerModule);
    } else {
      // Data word
      offset = kHeaderWordsPerBank
             + moduleindex * (kHeaderWordsPerModule + kMaxChannels * kDataWordsPerChannel + kFooterWordsPerModule)
             + kHeaderWordsPerModule
             + channelindex * kDataWordsPerChannel;
    }
  }
  return offset;
}


/********************************************************/
Int_t QwADC18_Channel::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;
  Bool_t bStatus;
  if (bEVENTCUTMODE>0){//Global switch to ON/OFF event cuts set at the event cut file

    if (bDEBUG)
      QwWarning<<" QwQWVK_Channel "<<GetElementName()<<"  "<<GetNumberOfSamples()<<QwLog::endl;

    // Sample size check
    bStatus = MatchNumberOfSamples(fNumberOfSamples_map);//compare the default sample size with no.of samples read by the module
    if (!bStatus) {
      fErrorFlag |= kErrorFlag_sample;
    }

    //check sequence number
    fSequenceNo_Prev++;
    if (fSequenceNo_Counter==0 || GetSequenceNumber()==0){//starting the data run
      fSequenceNo_Prev=GetSequenceNumber();
    }

    if (!MatchSequenceNumber(fSequenceNo_Prev)){//we have a sequence number error
      fEventIsGood=kFALSE;
      fErrorFlag|=kErrorFlag_Sequence;
      if (bDEBUG)       QwWarning<<" QwQWVK_Channel "<<GetElementName()<<" Sequence number previous value = "<<fSequenceNo_Prev<<" Current value= "<< GetSequenceNumber()<<QwLog::endl;
    }
  }
  else {
    fGoodEventCount = 1;
    fErrorFlag = 0;
  }

  return fErrorFlag;
}


/********************************************************/
void QwADC18_Channel::IncrementErrorCounters(){
  if ( (kErrorFlag_sample &  fErrorFlag)==kErrorFlag_sample)
    fErrorCount_sample++; //increment the hw error counter
  if ( (kErrorFlag_SW_HW &  fErrorFlag)==kErrorFlag_SW_HW)
    fErrorCount_SW_HW++; //increment the hw error counter
  if ( (kErrorFlag_Sequence &  fErrorFlag)==kErrorFlag_Sequence)
    fErrorCount_Sequence++; //increment the hw error counter
  if ( (kErrorFlag_SameHW &  fErrorFlag)==kErrorFlag_SameHW)
    fErrorCount_SameHW++; //increment the hw error counter
  if ( (kErrorFlag_ZeroHW &  fErrorFlag)==kErrorFlag_ZeroHW)
    fErrorCount_ZeroHW++; //increment the hw error counter
  if ( ((kErrorFlag_EventCut_L &  fErrorFlag)==kErrorFlag_EventCut_L) 
       || ((kErrorFlag_EventCut_U &  fErrorFlag)==kErrorFlag_EventCut_U)){
    fNumEvtsWithEventCutsRejected++; //increment the event cut error counter
  }
}

/********************************************************/

void QwADC18_Channel::InitializeChannel(TString name, TString datatosave)
{
  SetElementName(name);
  SetDataToSave(datatosave);
  SetNumberOfDataWords(1);
  SetNumberOfSubElements(1);

  kFoundPedestal = 0;
  kFoundGain = 0;

  fPedestal            = 0.0;
  fCalibrationFactor   = 1.0;

  fTreeArrayIndex      = 0;
  fTreeArrayNumEntries = 0;

  ClearEventData();

  fPreviousSequenceNumber = 0;
  fNumberOfSamples_map    = 0;
  fNumberOfSamples        = 0;

  // Use internal random variable by default
  fUseExternalRandomVariable = false;

  // Mock drifts
  fMockDriftAmplitude.clear();
  fMockDriftFrequency.clear();
  fMockDriftPhase.clear();

  // Mock asymmetries
  fMockAsymmetry     = 0.0;
  fMockGaussianMean  = 0.0;
  fMockGaussianSigma = 0.0;

  // Event cuts
  fULimit=-1;
  fLLimit=1;
  fNumEvtsWithEventCutsRejected = 0;

  fErrorFlag=0;               //Initialize the error flag
  fErrorConfigFlag=0;         //Initialize the error config. flag

  //init error counters//
  fErrorCount_sample     = 0;
  fErrorCount_SW_HW      = 0;
  fErrorCount_Sequence   = 0;
  fErrorCount_SameHW     = 0;
  fErrorCount_ZeroHW     = 0;
  fErrorCount_HWSat      = 0;

  fRunningSum            = 0;

  fADC_Same_NumEvt       = 0;
  fSequenceNo_Prev       = 0;
  fSequenceNo_Counter    = 0;

  fGoodEventCount        = 0;

  bEVENTCUTMODE          = 0;

  //std::cout<< "name = "<<name<<" error count same _HW = "<<fErrorCount_SameHW <<std::endl;
  return;
}

/********************************************************/

void QwADC18_Channel::InitializeChannel(TString subsystem, TString instrumenttype, TString name, TString datatosave){
  InitializeChannel(name,datatosave);
  SetSubsystemName(subsystem);
  SetModuleType(instrumenttype);
  //PrintInfo();
}

void QwADC18_Channel::LoadChannelParameters(QwParameterFile &paramfile){
  UInt_t value = 0;
  if (paramfile.ReturnValue("sample_size",value)){
    SetDefaultSampleSize(value);
  } else {
    QwWarning << "ADC18 Channel "
	      << GetElementName()
	      << " cannot set the default sample size."
	      << QwLog::endl;
  }
};


void QwADC18_Channel::ClearEventData()
{
  fDiff_Raw    = 0;
  fBase_Raw    = 0;
  fPeak_Raw    = 0;
  fValue       = 0.0;
  fValueM2     = 0.0;
  fValueError  = 0.0;
  fSequenceNumber   = 0;
  fNumberOfSamples  = 0;
  fGoodEventCount   = 0;
  fErrorFlag = 0;
}

void QwADC18_Channel::RandomizeEventData(int helicity, double time)
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
  fSequenceNumber = 0;
  fNumberOfSamples = fNumberOfSamples_map;
}

void QwADC18_Channel::SmearByResolution(double resolution)
{
  fValue = resolution*GetRandomValue();
  fValueM2 = 0.0;
  fNumberOfSamples = fNumberOfSamples_map;
  SetRawEventData();
}

// SetEventData() is used by the mock data generator to turn "model"
// data values into their equivalent raw data.  It should be used
// nowhere else.  -- pking, 2010-09-16

void QwADC18_Channel::SetEventData(Double_t value)
{
  fValue     = value;
  fDiff_Raw = (UInt_t) value;
  fPeak_Raw = (UInt_t) value;
  fBase_Raw = (UInt_t) 0;
  fSequenceNumber = 0;
  fNumberOfSamples = fNumberOfSamples_map;
}

void QwADC18_Channel::SetRawEventData()
{
  fNumberOfSamples = fNumberOfSamples_map;
  fDiff_Raw = Int_t(fValue / fCalibrationFactor + fPedestal) * fNumberOfSamples;
  fPeak_Raw = Int_t(fValue / fCalibrationFactor + fPedestal) * fNumberOfSamples;
};

// FIXME here goes the encoding of raw data into CODA blocks
void QwADC18_Channel::EncodeEventData(std::vector<UInt_t> &buffer)
{
  UInt_t localbuf[kDataWordsPerChannel] = {0};

  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Fill in with zero.
    localbuf[0] = 0;
  } else {
    localbuf[0] |=   mask31x & (0 << 31);
    localbuf[0] |= mask3029x & (0 << 29);
    localbuf[0] |= mask2625x & (0 << 25);
    UInt_t exp_dtype = 1;
    switch (exp_dtype) {
      case 0:
        break;
      case 1:
      case 2:
        localbuf[0] |= mask2422x & (2 << 22);
        localbuf[0] |=  mask170x & fPeak_Raw;
        break;
      case 4:
        break;
      default:
        QwError << "QwADC18_Channel::EncodeEventData: Unknown data type" << QwLog::endl;
    }
  }

  for (Int_t i = 0; i < kDataWordsPerChannel; i++) {
    buffer.push_back(localbuf[i]);
  }
}

Bool_t QwADC18_Channel::IsHeaderWord(UInt_t rawd) const
{
  return ((rawd & mask31x) != 0);
}

Int_t QwADC18_Channel::ProcessDataWord(UInt_t rawd)
{
  // "Actual" values from data word
  UInt_t act_dtype  = (rawd & mask2422x) >> 22;
  UInt_t act_chan   = (act_dtype != 4) ?
                      ((rawd & mask3029x) >> 29) : 0;
  UInt_t act_dvalue = (rawd & mask2625x) >> 25;
  UInt_t act_snum   = (act_dtype == 1 || act_dtype == 2) ?
                      ((rawd & mask2118x) >> 18) : 0;

  // Interpret by data type
  UInt_t value_raw = 0;
  switch (act_dtype) {
    case 0: // Diff word
      static UInt_t prev_dvalue = act_dvalue;
      if (act_dvalue != prev_dvalue) {
        QwError << "QwADC18_Channel::ProcessEvBuffer: Number of samples changed " << act_dvalue << " " << prev_dvalue << QwLog::endl;
        return 0;
      }
      value_raw = rawd & mask200x;
      if (rawd & mask21x) value_raw = -((~value_raw & 0x1fffffff) + 1);
      fNumberOfSamples = (1 << act_dvalue);
      return value_raw;
      break;
    case 1: // Peak word
    case 2: // Base word
      if (act_snum != fNumberOfSamples) {
        QwError << "QwADC18_Channel::ProcessEvBuffer: Number of samples changed " << act_snum << " " << fNumberOfSamples << QwLog::endl;
        return 0;
      }
      if (act_dvalue != 0) {
        QwError << "QwADC18_Channel::ProcessEvBuffer: Divider value non-zero 0x" << std::hex << rawd << std::dec << QwLog::endl;
        return 0;
      }
      return rawd & mask170x;
      break;
    case 4: // DAC word
      if (act_dvalue != 0) {
        QwError << "QwADC18_Channel::ProcessEvBuffer: Divider value non-zero 0x" << std::hex << rawd << std::dec << QwLog::endl;
        return 0;
      }
      return rawd & mask150x;
      break;
    default:
      QwError << "QwADC18_Channel::ProcessEvBuffer: Unknown data type 0x" << std::hex << rawd << std::dec << QwLog::endl;
      return 0;
  }
}

// FIXME here goes the decoding of raw data from CODA blocks
Int_t QwADC18_Channel::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
{
  Bool_t debug = false;

  // Print buffer
  if (debug) {
    QwOut << GetElementName() << " : " << QwLog::endl << std::hex;
    Int_t n = 25;
    for (size_t i = 0; i < num_words_left; i++) {
      QwOut << "0x" << std::setfill('0') << std::setw(8) << buffer[i] << " ";
      if (i % n == n - 1) QwOut << QwLog::endl;
    }
    QwOut << std::dec << std::setfill(' ') << std::setw(0) << QwLog::endl;
  }

  UInt_t words_read = 0;
  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
    words_read = kDataWordsPerChannel;
  } else if (num_words_left >= fNumberOfDataWords) {

    // Is this a header word?
    if (IsHeaderWord(buffer[0])) {

      // Debug output
      if (debug) {
        QwOut << " : header " << std::hex;
        UInt_t n = kHeaderWordsPerModule;
        for (size_t i = 0; i < n && i < num_words_left; i++) {
          QwOut << "0x" << std::setfill('0') << std::setw(8) << buffer[i] << " ";
        }
        QwOut << std::dec << std::setfill(' ') << std::setw(0) << QwLog::endl;
      }

      // Check if enough words left
      if (num_words_left < kHeaderWordsPerModule) {
        QwError << "QwADC18_Channel::ProcessEvBuffer: Not enough words left!" << QwLog::endl;
        return num_words_left;
      }

      // FIXME Catch 0xfa180bad

      // Header word: read DAC value
      fValue_Raw = ProcessDataWord(buffer[kHeaderWordsPerModule-1]);

      words_read = kHeaderWordsPerModule;

    } else {

      // Debug output
      if (debug) {
        QwOut << " : channel " << std::hex;
        UInt_t n = kDataWordsPerChannel;
        for (size_t i = 0; i < n && i < num_words_left; i++) {
          QwOut << "0x" << std::setfill('0') << std::setw(8) << buffer[i] << " ";
        }
        QwOut << std::dec << std::setfill(' ') << std::setw(0) << QwLog::endl;
      }

      // Check if enough words left
      if (num_words_left < kDataWordsPerChannel) {
        QwError << "QwADC18_Channel::ProcessEvBuffer: Not enough words left!" << QwLog::endl;
        return num_words_left;
      }

      // Data channel words: read diff, peak, base
      fDiff_Raw = ProcessDataWord(buffer[0]);
      fPeak_Raw = ProcessDataWord(buffer[1]);
      fBase_Raw = ProcessDataWord(buffer[2]);

      words_read = kDataWordsPerChannel;
    }

  } else {
    QwError << "QwADC18_Channel::ProcessEvBuffer: Not enough words!" << QwLog::endl;
  }

  return words_read;
}



void QwADC18_Channel::ProcessEvent()
{
  if (fNumberOfSamples == 0 && fDiff_Raw == 0) {
    //  There isn't valid data for this channel.  Just flag it and move on.
    fValue = 0.0;
    fValueM2 = 0.0;
    fErrorFlag |= kErrorFlag_sample;
  } else if (fNumberOfSamples == 0) {
    //  This is probably a more serious problem.
    QwWarning << "QwADC18_Channel::ProcessEvent:  Channel "
              << GetElementName()
              << " has fNumberOfSamples == 0 but has valid data in the value filed."
              << "Flag this as an error."
              << QwLog::endl;
    fValue = 0.0;
    fValueM2 = 0.0;
    fErrorFlag |= kErrorFlag_sample;
  } else {
    fValue = fCalibrationFactor * ( (Double_t(fDiff_Raw) / fNumberOfSamples) - fPedestal );
    fValueM2 = 0.0; // second moment is zero for single events
  }
}

Double_t QwADC18_Channel::GetAverageVolts() const
{
  return fValue * kADC18_VoltsPerBit / fNumberOfSamples;
}

void QwADC18_Channel::PrintInfo() const
{
  QwMessage<<"***************************************"<<QwLog::endl;
  QwMessage<<"Subsystem "<<GetSubsystemName()<<QwLog::endl;
  QwMessage<<"Beam Instrument Type: "<<GetModuleType()<<QwLog::endl;
  QwMessage<<"QwADC18 channel: "<<GetElementName()<<QwLog::endl;
  QwMessage<<"fPedestal= "<< fPedestal<<QwLog::endl;
  QwMessage<<"fCalibrationFactor= "<<fCalibrationFactor<<QwLog::endl;
  QwMessage<<"fSequenceNumber= "<<fSequenceNumber<<QwLog::endl;
  QwMessage<<"fNumberOfSamples= "<<fNumberOfSamples<<QwLog::endl;
  QwMessage<<"fDiff_Raw= "<<fDiff_Raw<<QwLog::endl;
  QwMessage<<"fPeak_Raw= "<<fPeak_Raw<<QwLog::endl;
  QwMessage<<"fBase_Raw= "<<fBase_Raw<<QwLog::endl;
  QwMessage<<"fValue = "<<std::setprecision(8) <<fValue << QwLog::endl;
}

void  QwADC18_Channel::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (IsNameEmpty()){
    //  This channel is not used, so skip filling the histograms.
  } else {
    //  Now create the histograms.
    if (prefix == TString("asym_")
       || prefix == TString("diff_")
       || prefix == TString("yield_"))
      fDataToSave=kDerived;

    TString basename, fullname;
    basename = prefix + GetElementName();

    if(fDataToSave==kRaw)
      {
	fHistograms.resize(2, NULL);
	size_t index=0;
	fHistograms[index++] = gQwHists.Construct1DHist(basename);
	fHistograms[index++] = gQwHists.Construct1DHist(basename+Form("_raw"));
      }
    else if(fDataToSave==kDerived)
      {
	fHistograms.resize(1, NULL);
	Int_t index=0;
	fHistograms[index++] = gQwHists.Construct1DHist(basename);
      }
    else
      {
	// this is not recognized
      }
  }
}

void  QwADC18_Channel::FillHistograms()
{
  Int_t index=0;

  if (IsNameEmpty())
    {
      //  This channel is not used, so skip creating the histograms.
    } else
      {
	if(fDataToSave==kRaw)
	  {
	    if (fHistograms[index] != NULL && (fErrorFlag)==0)
	      fHistograms[index++]->Fill(GetValue());
	    if (fHistograms[index] != NULL && (fErrorFlag)==0)
	      fHistograms[index++]->Fill(GetRawValue());
	  }
	else if(fDataToSave==kDerived)
	  {
	    if (fHistograms[index] != NULL && (fErrorFlag)==0)
	      fHistograms[index++]->Fill(GetValue());
	  }
    }
}

void  QwADC18_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
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
    }

    values.push_back(0.0);
    list += ":Device_Error_Code/D";

    if (fDataToSave == kRaw){
      values.push_back(0.0);
      list += ":raw/D";
      values.push_back(0.0);
      list += ":diff/D";
      values.push_back(0.0);
      list += ":peak/D";
      values.push_back(0.0);
      list += ":base/D";
    }

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    if (gQwHists.MatchDeviceParamsFromList(basename.Data()))
      tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
}

void  QwADC18_Channel::ConstructBranch(TTree *tree, TString &prefix)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    tree->Branch(basename, &fValue, basename+"/D");
  }
}


void  QwADC18_Channel::FillTreeVector(std::vector<Double_t> &values) const
{
  if (IsNameEmpty()) {
    //  This channel is not used, so skip setting up the tree.
  } else if (fTreeArrayNumEntries < 0) {
    QwError << "QwADC18_Channel::FillTreeVector:  fTreeArrayNumEntries=="
            << fTreeArrayNumEntries << QwLog::endl;
  } else if (fTreeArrayNumEntries == 0) {
    static bool warned = false;
    if (!warned) {
      QwError << "QwADC18_Channel::FillTreeVector:  fTreeArrayNumEntries=="
              << fTreeArrayNumEntries << " (no branch constructed?)" << QwLog::endl;
      QwError << "Offending element is " << GetElementName() << QwLog::endl;
      warned = true;
    }
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries) {
    QwError << "QwADC18_Channel::FillTreeVector:  values.size()=="
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
    }
    values[index++] = this->fErrorFlag;
    if(fDataToSave==kRaw){
      values[index++] = this->fValue_Raw;
      values[index++] = this->fDiff_Raw;
      values[index++] = this->fPeak_Raw;
      values[index++] = this->fBase_Raw;
    }
  }
}


QwADC18_Channel& QwADC18_Channel::operator= (const QwADC18_Channel &value)
{
  if (this == &value) return *this;

  if (!IsNameEmpty()) {
    VQwHardwareChannel::operator=(value);
    this->fDiff_Raw   = value.fDiff_Raw;
    this->fPeak_Raw   = value.fPeak_Raw;
    this->fBase_Raw   = value.fBase_Raw;
    this->fValue_Raw  = value.fValue_Raw;
    this->fValue      = value.fValue;
    this->fValueError = value.fValueError;
    this->fValueM2    = value.fValueM2;
  }
  return *this;
}

void QwADC18_Channel::AssignScaledValue(const QwADC18_Channel &value,
				 Double_t scale)
{
  if(this == &value) return;

  if (!IsNameEmpty()) {
    this->fValue      = value.fValue * scale;
    this->fValueError = value.fValueError;
    this->fValueM2    = value.fValueM2 * scale * scale;
    this->fErrorFlag  = value.fErrorFlag;//error code is updated.
    this->fGoodEventCount = value.fGoodEventCount;
  }
}

void QwADC18_Channel::AssignValueFrom(const  VQwDataElement* valueptr)
{
  const QwADC18_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwADC18_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this = *tmpptr;
  } else {
    TString loc="Standard exception from QwADC18_Channel::AssignValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void QwADC18_Channel::AddValueFrom(const  VQwHardwareChannel* valueptr)
{
  const QwADC18_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwADC18_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this += *tmpptr;
  } else {
    TString loc="Standard exception from QwADC18_Channel::AddValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void QwADC18_Channel::SubtractValueFrom(const  VQwHardwareChannel* valueptr)
{
  const QwADC18_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwADC18_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this -= *tmpptr;
  } else {
    TString loc="Standard exception from QwADC18_Channel::SubtractValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void QwADC18_Channel::MultiplyBy(const VQwHardwareChannel* valueptr)
{
  const QwADC18_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwADC18_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this *= *tmpptr;
  } else {
    TString loc="Standard exception from QwADC18_Channel::MultiplyBy = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void QwADC18_Channel::DivideBy(const VQwHardwareChannel* valueptr)
{
  const QwADC18_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwADC18_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this /= *tmpptr;
  } else {
    TString loc="Standard exception from QwADC18_Channel::DivideBy = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}


const QwADC18_Channel QwADC18_Channel::operator+ (const QwADC18_Channel &value) const
{
  QwADC18_Channel result = *this;
  result += value;
  return result;
}

QwADC18_Channel& QwADC18_Channel::operator+= (const QwADC18_Channel &value)
{
  if (!IsNameEmpty()) {
    this->fValue           += value.fValue;
    this->fValueM2          = 0.0;
    this->fErrorFlag       |= value.fErrorFlag;//error code is ORed.
  }
  return *this;
}

const QwADC18_Channel QwADC18_Channel::operator- (const QwADC18_Channel &value) const
{
  QwADC18_Channel result = *this;
  result -= value;
  return result;
}

QwADC18_Channel& QwADC18_Channel::operator-= (const QwADC18_Channel &value)
{
  if (!IsNameEmpty()){
    this->fValue           -= value.fValue;
    this->fValueM2          = 0.0;
    this->fErrorFlag       |= (value.fErrorFlag);//error code is ORed.
  }
  return *this;
}

const QwADC18_Channel QwADC18_Channel::operator* (const QwADC18_Channel &value) const
{
  QwADC18_Channel result = *this;
  result *= value;
  return result;
}

QwADC18_Channel& QwADC18_Channel::operator*= (const QwADC18_Channel &value)
{
  if (!IsNameEmpty()){
    this->fValue     *= value.fValue;
    this->fDiff_Raw   = 0;
    this->fPeak_Raw   = 0;
    this->fBase_Raw   = 0;
    this->fValueM2    = 0.0;
    this->fErrorFlag |= (value.fErrorFlag);//error code is ORed.
  }
  return *this;
}

VQwHardwareChannel& QwADC18_Channel::operator+=(const VQwHardwareChannel *source)
{
  const QwADC18_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwADC18_Channel*>(source);
  if (tmpptr!=NULL){
    *this += *tmpptr;
  } else {
    TString loc="Standard exception from QwADC18_Channel::operator+= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}

VQwHardwareChannel& QwADC18_Channel::operator-=(const VQwHardwareChannel *source)
{
  const QwADC18_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwADC18_Channel*>(source);
  if (tmpptr!=NULL){
    *this -= *tmpptr;
  } else {
    TString loc="Standard exception from QwADC18_Channel::operator-= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}

VQwHardwareChannel& QwADC18_Channel::operator*=(const VQwHardwareChannel *source)
{
  const QwADC18_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwADC18_Channel*>(source);
  if (tmpptr!=NULL){
    *this *= *tmpptr;
  } else {
    TString loc="Standard exception from QwADC18_Channel::operator*= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}

VQwHardwareChannel& QwADC18_Channel::operator/=(const VQwHardwareChannel *source)
{
  const QwADC18_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwADC18_Channel*>(source);
  if (tmpptr!=NULL){
    *this /= *tmpptr;
  } else {
    TString loc="Standard exception from QwADC18_Channel::operator/= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}


void QwADC18_Channel::Sum(const QwADC18_Channel &value1, const QwADC18_Channel &value2)
{
  *this  = value1;
  *this += value2;
}

void QwADC18_Channel::Difference(const QwADC18_Channel &value1, const QwADC18_Channel &value2)
{
  *this  = value1;
  *this -= value2;
}

void QwADC18_Channel::Ratio(const QwADC18_Channel &numer, const QwADC18_Channel &denom)
{
  if (!IsNameEmpty()) {
    *this  = numer;
    *this /= denom;

    //  Set the raw values to zero.
    fDiff_Raw = 0;
    fPeak_Raw = 0;
    fBase_Raw = 0;

    // Remaining variables
    fGoodEventCount  = denom.fGoodEventCount;
    fErrorFlag = (numer.fErrorFlag|denom.fErrorFlag);//error code is ORed.
  }
}

QwADC18_Channel& QwADC18_Channel::operator/= (const QwADC18_Channel &denom)
{
  //  In this function, leave the "raw" variables untouched.
  Double_t ratio;
  Double_t variance;
  if (!IsNameEmpty()) {
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

//--------------------------------------------------------------------------------------------

void QwADC18_Channel::ArcTan(const QwADC18_Channel &value)
{
  if (!IsNameEmpty()) {
    this->fValue = atan(value.fValue);
  }
  // FIXME stuff missing here
}

//--------------------------------------------------------------------------------------------
void QwADC18_Channel::Product(const QwADC18_Channel &value1, const QwADC18_Channel &value2)
{
  if (!IsNameEmpty()){
    fValue = value1.fValue * value2.fValue;
    fDiff_Raw = 0;
    fPeak_Raw = 0;
    fBase_Raw = 0;

    // Remaining variables
    fGoodEventCount  = value2.fGoodEventCount;
    fErrorFlag = (value1.fErrorFlag | value2.fErrorFlag);//error code is ORed.
  }
}

/**
This function will add a offset to the hw_sum and add the same offset for blocks.
 */
void QwADC18_Channel::AddChannelOffset(Double_t offset)
{
  if (!IsNameEmpty())
    {
      fValue += offset;
    }
}

void QwADC18_Channel::Scale(Double_t scale)
{
  if (!IsNameEmpty())
    {
      fValue   *= scale;
      fValueM2 *= scale * scale;
    }
}

void QwADC18_Channel::DivideBy(const QwADC18_Channel &denom)
{
  *this /= denom;
}


/** Moments and uncertainty calculation on the running sums and averages
 * The calculation of the first and second moments of the running sum is not
 * completely straightforward due to numerical instabilities associated with
 * small variances and large average values.  The naive computation taking
 * the difference of the square of the average and the average of the squares
 * leads to the subtraction of two very large numbers to get a small number.
 *
 * Alternative algorithms (including for higher order moments) are supplied in
 *  Pebay, Philippe (2008), "Formulas for Robust, One-Pass Parallel Computation
 *  of Covariances and Arbitrary-Order Statistical Moments", Technical Report
 *  SAND2008-6212, Sandia National Laboratories.
 *  http://infoserve.sandia.gov/sand_doc/2008/086212.pdf
 *
 * In the following formulas the moments \f$ M^1 \f$ and \f$ M^2 \f$ are defined
 * \f{eqnarray}
 *     M^1 & = & \frac{1}{n} \sum^n y \\
 *     M^2 & = & \sum^n (y - \mu)
 * \f}
 *
 * Recurrence relations for the addition of a single event:
 * \f{eqnarray}
 *     M^1_n & = & M^1_{n-1} + \frac{y - M^1_{n-1}}{n} \\
 *     M^2_n & = & M^2_{n-1} + (y - M^1_{n-1})(y - M^1_n)
 * \f}
 *
 * For the addition of an already accumulated sum:
 * \f{eqnarray}
 *     M^1 & = & M^1_1 + n_2 \frac{M^1_2 - M^1_1}{n} \\
 *     M^2 & = & M^2_1 + M^2_2 + n_1 n_2 \frac{(M^1_2 - M^1_1)^2}{n}
 * \f}
 *
 * In these recursive formulas we start from \f$ M^1 = y \f$ and \f$ M^2 = 0 \f$.
 *
 * To calculate the mean and standard deviation we use
 * \f{eqnarray}
 *          \mu & = & M^1 \\
 *     \sigma^2 & = & \frac{1}{n} M^2
 * \f}
 * The standard deviation is a biased estimator, but this is what ROOT uses.
 * Better would be to divide by \f$ (n-1) \f$.
 *
 * We use the formulas provided there for the calculation of the first and
 * second moments (i.e. average and variance).
 */
/**
 * Accumulate the running moments M1 and M2
 * @param value Object (single event or accumulated) to add to running moments
 * @param count Number of good events in value
 */
void QwADC18_Channel::AccumulateRunningSum(const QwADC18_Channel& value, Int_t count, Int_t ErrorMask)
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

  if (ErrorMask ==  kPreserveError){
    //n = 1;
    if (n2 == 0) {
      n2 = 1;
    }
    if (count == -1) {
      n2 = -1;
    }
  }

  Int_t n = n1 + n2;

  // Set up variables
  Double_t M11 = fValue;
  Double_t M12 = value.fValue;
  Double_t M22 = value.fValueM2;
  if (n2 == 0) {
    // no good events for addition
    return;
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


void QwADC18_Channel::CalculateRunningAverage()
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


void QwADC18_Channel::PrintValue() const
{
  QwMessage << std::setprecision(8)
            << std::setw(18) << std::left << GetSubsystemName()    << ""
            << std::setw(18) << std::left << GetModuleType()       << ""
            << std::setw(18) << std::left << GetElementName()      << ""
            << std::setw(12) << std::left << GetValue()            << " +/- "
            << std::setw(12) << std::left << GetValueError()       << " sig "
	    << std::setw(12) << std::left << GetValueWidth()       << ""
            << std::setw(12) << std::left << fGoodEventCount       << ""
            << QwLog::endl;
}

std::ostream& operator<< (std::ostream& stream, const QwADC18_Channel& channel)
{
  stream << channel.GetValue();
  return stream;
}

/**
 * Blind this channel as an asymmetry
 * @param blinder Blinder
 */
void QwADC18_Channel::Blind(const QwBlinder *blinder)
{
  if (!IsNameEmpty()) {
    if (blinder->IsBlinderOkay() && ((fErrorFlag)==0) ){
      blinder->BlindValue(fValue);
    } else {
      blinder->ModifyThisErrorCode(fErrorFlag);
      fValue = QwBlinder::kValue_BlinderFail;
    }
  }
}

/**
 * Blind this channel as a difference with specified yield
 * @param blinder Blinder
 * @param yield Corresponding yield
 */
void QwADC18_Channel::Blind(const QwBlinder *blinder, const QwADC18_Channel& yield)
{
  if (!IsNameEmpty()) {
    if (blinder->IsBlinderOkay() && ((fErrorFlag) ==0) ){
      blinder->BlindValue(fValue, yield.fValue);
    } else {
      blinder->ModifyThisErrorCode(fErrorFlag);//update the HW error code
      fValue = QwBlinder::kValue_BlinderFail * yield.fValue;
    }
  }
}

Bool_t QwADC18_Channel::MatchSequenceNumber(size_t seqnum)
{
  Bool_t status = kTRUE;
  if (!IsNameEmpty()){
    status = (fSequenceNumber == seqnum);
    }
  return status;
}

Bool_t QwADC18_Channel::MatchNumberOfSamples(size_t numsamp)
{
  Bool_t status = kTRUE;
  if (!IsNameEmpty()){
    status = (fNumberOfSamples==numsamp);
    if (! status){
      if (bDEBUG)
	QwError << "QwADC18_Channel::MatchNumberOfSamples:  Channel "
		<< GetElementName()
		<< " had fNumberOfSamples==" << fNumberOfSamples
		<< " and was supposed to have " << numsamp
		<< std::endl;
    }
  }
  return status;
}

//only check to see HW_Sum is within these given limits
Bool_t QwADC18_Channel::ApplySingleEventCuts(Double_t LL,Double_t UL)
{
  Bool_t status = kFALSE;

  if (UL < LL){
    status=kTRUE;
  } else  if (GetValue()<=UL && GetValue()>=LL){
    if ((fErrorFlag & kPreserveError)!=0)
      status=kTRUE;
    else
      status=kFALSE;//If the device HW is failed
  }
  std::cout<<(this->fErrorFlag & kPreserveError)<<std::endl;
  return status;
}

Bool_t QwADC18_Channel::ApplySingleEventCuts()//This will check the limits and update event_flags and error counters
{
  Bool_t status;

  if (bEVENTCUTMODE>=2){//Global switch to ON/OFF event cuts set at the event cut file

    if (fULimit < fLLimit){
      status=kTRUE;
    } else  if (GetValue()<=fULimit && GetValue()>=fLLimit){
      if ((fErrorFlag)==0)
	status=kTRUE;
      else
	status=kFALSE;//If the device HW is failed
    }
    else{
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
    //fErrorFlag=0;//we need to keep the device error codes 
  }

  return status;
}

void  QwADC18_Channel::PrintErrorCounterHead()
{
  TString message;
  message  = Form("%30s","Device name");
  message += Form("%9s", "HW Sat");
  message += Form("%9s", "Sample");
  message += Form("%9s", "SW_HW");
  message += Form("%9s", "Sequence");
  message += Form("%9s", "SameHW");
  message += Form("%9s", "ZeroHW");
  message += Form("%9s", "EventCut");
  QwMessage << "---------------------------------------------------------------------------------------------" << QwLog::endl;
  QwMessage << message << QwLog::endl; 
  QwMessage << "---------------------------------------------------------------------------------------------" << QwLog::endl;
}

void  QwADC18_Channel::PrintErrorCounterTail()
{
  QwMessage << "---------------------------------------------------------------------------------------------" << QwLog::endl;
}

void  QwADC18_Channel::PrintErrorCounters() const
{
  TString message;
  if (fErrorCount_sample || fErrorCount_SW_HW || fErrorCount_Sequence || fErrorCount_SameHW || fErrorCount_ZeroHW || fErrorCount_HWSat || fNumEvtsWithEventCutsRejected) {
    message  = Form("%30s", GetElementName().Data());
    message += Form("%9d", fErrorCount_HWSat);
    message += Form("%9d", fErrorCount_sample);
    message += Form("%9d", fErrorCount_SW_HW);
    message += Form("%9d", fErrorCount_Sequence);
    message += Form("%9d", fErrorCount_SameHW);
    message += Form("%9d", fErrorCount_ZeroHW);
    message += Form("%9d", fNumEvtsWithEventCutsRejected);

    if((fDataToSave == kRaw) && (!kFoundPedestal||!kFoundGain)){
      message += " >>>>> No Pedestal or Gain in map file";
    }

    QwMessage << message << QwLog::endl;
  }
}

void QwADC18_Channel::ScaledAdd(Double_t scale, const VQwHardwareChannel *value)
{
  const QwADC18_Channel* input = dynamic_cast<const QwADC18_Channel*>(value);

  // follows same steps as += but w/ scaling factor
  if(input!=NULL && !IsNameEmpty()){
    //     QwWarning << "Adding " << input->GetElementName()
    // 	      << " to " << GetElementName()
    // 	      << " with scale factor " << scale
    // 	      << QwLog::endl;
    //     PrintValue();
    //     input->PrintValue();
    this->fDiff_Raw = 0;
    this->fPeak_Raw = 0;
    this->fBase_Raw = 0;
    this->fValue += scale * input->fValue;
    this->fValueM2 = 0.0;
    this->fNumberOfSamples += input->fNumberOfSamples;
    this->fSequenceNumber  =  0;
    this->fErrorFlag       |= (input->fErrorFlag);   
  }
  //   QwWarning << "Finsihed with addition"  << QwLog::endl;
  //   PrintValue();
}

#ifdef __USE_DATABASE__
void QwADC18_Channel::AddErrEntriesToList(std::vector<QwErrDBInterface> &row_list)
{
  // TString message;
  // message  = Form("%30s",GetElementName().Data());
  // message += Form("%9d", fErrorCount_HWSat);
  // message += Form("%9d", fErrorCount_sample);
  // message += Form("%9d", fErrorCount_SW_HW);
  // message += Form("%9d", fErrorCount_Sequence);
  // message += Form("%9d", fErrorCount_SameHW);
  // message += Form("%9d", fErrorCount_ZeroHW);
  // message += Form("%9d", fNumEvtsWithEventCutsRejected);
  // QwMessage << message << QwLog::endl;

  // kErrorFlag_ADC18_Sat   =0x1;    //ADC18 Saturation Cut. Currently saturation limit is set to +/-8.5V
  // kErrorFlag_sample     =0x2;    //If sample size mis-matches with the default value in the map file.
  // kErrorFlag_Sequence   =0x8;    //If the ADC sequence number is not incrementing properly
  // kErrorFlag_SameHW     =0x10;   //If ADC value keep returning the same value
  // kErrorFlag_ZeroHW     =0x20;   //Check to see ADC is returning zero



  // kErrorFlag_EventCut_L =0x40;   //Flagged if lower limit of the event cut has failed
  // kErrorFlag_EventCut_U =0x80;   //Flagged if upper limit of the event cut has failed
  // >>>>>>  fNumEvtsWithEventCutsRejected

  // outside QwADC18_Channel
  // kErrorFlag_BlinderFail = 0x0200;// in Decimal  512 to identify the blinder fail flag
  // kStabilityCutError     = 0x10000000;// in Decimal 2^28 to identify the stability cut failure

  // This is my modified mysql DB, Thursday, December  8 16:40:36 EST 2011, jhlee
  // Error code must be matched to MySQL DB
  //
  // mysql> select * from error_code;
  // +---------------+------------------------------+
  // | error_code_id | quantity                     |
  // +---------------+------------------------------+
  // |             1 | kErrorFlag_ADC18_Sat         |
  // |             2 | kErrorFlag_sample            |
  // |             3 | kErrorFlag_SW_HW             |
  // |             4 | kErrorFlag_Sequence          |
  // |             5 | kErrorFlag_SameHW            |
  // |             6 | kErrorFlag_ZeroHW            |
  // |             7 | kErrorFlag_EventCut_Rejected |
  // |             8 | kErrorFlag_EventCut_L        |
  // |             9 | kErrorFlag_EventCut_U        |
  // |            10 | kErrorFlag_BlinderFail       |
  // |            11 | kStabilityCutError           |
  // +---------------+------------------------------+
  // 11 rows in set (0.00 sec)


  QwErrDBInterface row;
  TString name    = GetElementName();
  
  row.Reset();
  row.SetDeviceName(name);
  row.SetErrorCodeId(1); 
  row.SetN(fErrorCount_HWSat);
  row_list.push_back(row);
  
  row.Reset();
  row.SetDeviceName(name);
  row.SetErrorCodeId(2);
  row.SetN(fErrorCount_sample);
  row_list.push_back(row);
  
  row.Reset();
  row.SetDeviceName(name);
  row.SetErrorCodeId(3);
  row.SetN(fErrorCount_SW_HW);
  row_list.push_back(row);
  
  
  row.Reset();
  row.SetDeviceName(name);
  row.SetErrorCodeId(4);
  row.SetN(fErrorCount_Sequence);
  row_list.push_back(row);
  
  
  row.Reset();
  row.SetDeviceName(name);
  row.SetErrorCodeId(5); 
  row.SetN(fErrorCount_SameHW);
  row_list.push_back(row);
  
  row.Reset();
  row.SetDeviceName(name);
  row.SetErrorCodeId(6); 
  row.SetN(fErrorCount_ZeroHW);
  row_list.push_back(row);


  row.Reset();
  row.SetDeviceName(name);
  row.SetErrorCodeId(7); 
  row.SetN(fNumEvtsWithEventCutsRejected);
  row_list.push_back(row);
  return;
  
}
#endif

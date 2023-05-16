#include "QwMollerADC_Channel.h"
 
// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwBlinder.h"
#include "QwHistogramHelper.h"
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif

const Bool_t QwMollerADC_Channel::kDEBUG = kFALSE;

const Int_t  QwMollerADC_Channel::kWordsPerChannel = 30;
const Int_t  QwMollerADC_Channel::kMaxChannels     = 8;

const Double_t QwMollerADC_Channel::kTimePerSample = (2.0/30.0) * Qw::us; //2.0 originally

/*!  Conversion factor to translate the average bit count in an ADC
 *   channel into average voltage.
 *   The base factor is roughly 76 uV per count, and zero counts corresponds
 *   to zero voltage.
 *   Store as the exact value for 20 V range, 18 bit ADC.
 */
const Double_t QwMollerADC_Channel::kMollerADC_VoltsPerBit = (20./(1<<18));

/*!  Static member function to return the word offset within a data buffer
 *   given the module number index and the channel number index.
 *   @param moduleindex   Module index within this buffer; counts from zero
 *   @param channelindex  Channel index within this module; counts from zero
 *   @return   The number of words offset to the beginning of this
 *             channel's data from the beginning of the MollerADC buffer.
 */
Int_t QwMollerADC_Channel::GetBufferOffset(Int_t moduleindex, Int_t channelindex){
    Int_t offset = -1;
    if (moduleindex<0 ){
      QwError << "QwMollerADC_Channel::GetBufferOffset:  Invalid module index,"
              << moduleindex
              << ".  Must be zero or greater."
              << QwLog::endl;
    } else if (channelindex<0 || channelindex>kMaxChannels){
      QwError << "QwMollerADC_Channel::GetBufferOffset:  Invalid channel index,"
              << channelindex
              << ".  Must be in range [0," << kMaxChannels << "]."
              << QwLog::endl;
    } else {
      offset = ( (moduleindex * kMaxChannels) + channelindex )
        * kWordsPerChannel;
    }
    return offset;
  }


/********************************************************/
Int_t QwMollerADC_Channel::ApplyHWChecks()
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

    // Check SW and HW return the same sum
    bStatus = (GetRawHardwareSum() == GetRawSoftwareSum());
    //fEventIsGood = bStatus;
    if (!bStatus) {
      fErrorFlag |= kErrorFlag_SW_HW;
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

    fSequenceNo_Counter++;

    //Checking for HW_sum is returning same value.
    if (fPrev_HardwareBlockSum != GetRawHardwareSum()){
      //std::cout<<" BCM hardware sum is different  "<<std::endl;
      fPrev_HardwareBlockSum = GetRawHardwareSum();
      fADC_Same_NumEvt=0;
    }else
      fADC_Same_NumEvt++;//hw_sum is same increment the counter

    //check for the hw_sum is giving the same value
    if (fADC_Same_NumEvt>0){//we have ADC stuck with same value
      if (bDEBUG) QwWarning<<" BCM hardware sum is same for more than  "<<fADC_Same_NumEvt<<" time consecutively  "<<QwLog::endl;
      fErrorFlag|=kErrorFlag_SameHW;
    }

    //check for the hw_sum is zero
    if (GetRawHardwareSum()==0){
      fErrorFlag|=kErrorFlag_ZeroHW;
    }
    if (!fEventIsGood)    
      fSequenceNo_Counter=0;//resetting the counter after ApplyHWChecks() a failure

    if ((TMath::Abs(GetRawHardwareSum())*kMollerADC_VoltsPerBit/fNumberOfSamples) > GetMollerADCSaturationLimt()){
      if (bDEBUG) 
        QwWarning << this->GetElementName()<<" "<<GetRawHardwareSum() << "Saturating MollerADC invoked! " <<TMath::Abs(GetRawHardwareSum())*kMollerADC_VoltsPerBit/fNumberOfSamples<<" Limit "<<GetMollerADCSaturationLimt() << QwLog::endl;
      fErrorFlag|=kErrorFlag_VQWK_Sat; 
    }

  }
  else {
    fGoodEventCount = 1;
    fErrorFlag = 0;
  }

  return fErrorFlag;
}


/********************************************************/
void QwMollerADC_Channel::IncrementErrorCounters(){
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
  if ( (kErrorFlag_VQWK_Sat &  fErrorFlag)==kErrorFlag_VQWK_Sat)
    fErrorCount_HWSat++; //increment the hw saturation error counter
  if ( ((kErrorFlag_EventCut_L &  fErrorFlag)==kErrorFlag_EventCut_L) 
       || ((kErrorFlag_EventCut_U &  fErrorFlag)==kErrorFlag_EventCut_U)){
    fNumEvtsWithEventCutsRejected++; //increment the event cut error counter
  }
}

/********************************************************/

void QwMollerADC_Channel::InitializeChannel(TString name, TString datatosave)
{
  SetElementName(name);
  SetDataToSave(datatosave);
  SetNumberOfDataWords(6);
  SetNumberOfSubElements(5);

  kFoundPedestal = 0;
  kFoundGain = 0;

  fPedestal            = 0.0;
  fCalibrationFactor   = 1.0;

  fBlocksPerEvent      = 4;

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

  fADC_Same_NumEvt       = 0;
  fSequenceNo_Prev       = 0;
  fSequenceNo_Counter    = 0;
  fPrev_HardwareBlockSum = 0.0;

  fGoodEventCount        = 0;

  bEVENTCUTMODE          = 0;

  return;
}

/********************************************************/

void QwMollerADC_Channel::InitializeChannel(TString subsystem, TString instrumenttype, TString name, TString datatosave){
  InitializeChannel(name,datatosave);
  SetSubsystemName(subsystem);
  SetModuleType(instrumenttype);
  //PrintInfo();
}

void QwMollerADC_Channel::LoadChannelParameters(QwParameterFile &paramfile){
  UInt_t value = 0;
  if (paramfile.ReturnValue("sample_size",value)){
    SetDefaultSampleSize(value);
  } else {
    QwWarning << "MollerADC Channel "
              << GetElementName()
              << " cannot set the default sample size."
              << QwLog::endl;
  }
};


void QwMollerADC_Channel::ClearEventData()
{
  for (Int_t i = 0; i < fBlocksPerEvent; i++) {
    fBlock_raw[i] = 0;
    fBlockSumSq_raw[i] = 0;
    fBlock_min[i] = 0;
    fBlock_max[i] = 0;
    fBlock[i] = 0.0;
    fBlockM2[i] = 0.0;
    fBlockError[i] = 0.0;
  }
  fHardwareBlockSum_raw = 0;
  fSoftwareBlockSum_raw = 0;
  fHardwareBlockSum   = 0.0;
  fHardwareBlockSumM2 = 0.0;
  fHardwareBlockSumError = 0.0;
  fSequenceNumber   = 0;
  fNumberOfSamples  = 0;
  fGoodEventCount   = 0;
  fErrorFlag=0;
  return;
}

void QwMollerADC_Channel::RandomizeEventData(int helicity, double time)
{
  // updated to calculate the drift for each block individually
  Double_t drift = 0.0;
  for (Int_t i = 0; i < fBlocksPerEvent; i++){
    drift = 0.0;
    if (i >= 1){
      time += (fNumberOfSamples_map/4.0)*kTimePerSample;
    }
    for (UInt_t i = 0; i < fMockDriftFrequency.size(); i++) {
      drift += fMockDriftAmplitude[i] * sin(2.0 * Qw::pi * fMockDriftFrequency[i] * time + fMockDriftPhase[i]);
      //std::cout << "Drift: " << drift << std::endl;
    }
  }

  // Calculate signal
  fHardwareBlockSum = 0.0;
  fHardwareBlockSumM2 = 0.0; // second moment is zero for single events
  fBlock_max[4] = kMinInt;
  fBlock_min[4] = kMaxInt;

  for (Int_t i = 0; i < fBlocksPerEvent; i++) {
    double tmpvar = GetRandomValue();

    fBlock[i] = fMockGaussianMean + drift;

    if (fCalcMockDataAsDiff) {
      fBlock[i] += helicity*fMockAsymmetry;
    } else {
      fBlock[i] *= 1.0 + helicity*fMockAsymmetry;
    }
    fBlock[i] += fMockGaussianSigma*tmpvar*sqrt(fBlocksPerEvent);
    fBlockM2[i] = 0.0; // second moment is zero for single events
    fHardwareBlockSum += fBlock[i];

  }
  fHardwareBlockSum /= fBlocksPerEvent;
  fSequenceNumber = 0;
  fNumberOfSamples = fNumberOfSamples_map;
  //  SetEventData(block);
  //  delete block;
  return;
}

void QwMollerADC_Channel::SmearByResolution(double resolution){

  fHardwareBlockSum   = 0.0;
  fHardwareBlockSumM2 = 0.0; // second moment is zero for single events
  for (Int_t i = 0; i < fBlocksPerEvent; i++) {

    fBlock[i] += resolution*sqrt(fBlocksPerEvent) * GetRandomValue();
 
    fBlockM2[i] = 0.0; // second moment is zero for single events
    fHardwareBlockSum += fBlock[i];
  }
  // std::cout << std::endl;
  fHardwareBlockSum /= fBlocksPerEvent;

  fNumberOfSamples = fNumberOfSamples_map;
  // SetRawEventData();
  return;
}

void QwMollerADC_Channel::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  Double_t* block = new Double_t[fBlocksPerEvent];
  for (Int_t i = 0; i < fBlocksPerEvent; i++){
    block[i] = hwsum / fBlocksPerEvent;
  }
  SetEventData(block);
  delete block;
  return;
}


// SetEventData() is used by the mock data generator to turn "model"
// data values into their equivalent raw data.  It should be used
// nowhere else.  -- pking, 2010-09-16

void QwMollerADC_Channel::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fHardwareBlockSum = 0.0;
  fHardwareBlockSumM2 = 0.0; // second moment is zero for single events
  for (Int_t i = 0; i < fBlocksPerEvent; i++) {
    fBlock[i] = block[i];
    fBlockM2[i] = 0.0; // second moment is zero for single events
    fHardwareBlockSum += block[i];
  }
  fHardwareBlockSum /= fBlocksPerEvent;

  fSequenceNumber = sequencenumber;
  fNumberOfSamples = fNumberOfSamples_map;

//  Double_t thispedestal = 0.0;
//  thispedestal = fPedestal * fNumberOfSamples;

  SetRawEventData();
  return;
}

void QwMollerADC_Channel::SetRawEventData(){
  fNumberOfSamples = fNumberOfSamples_map;
  fHardwareBlockSum_raw = 0;
//  Double_t hwsum_test = 0.0;
//  std::cout <<  "*******In QwMollerADC_Channel::SetRawEventData for channel:\t" << this->GetElementName() << std::endl;
  for (Int_t i = 0; i < fBlocksPerEvent; i++) 
    {
     fBlock_raw[i] = Int_t((fBlock[i] / fCalibrationFactor + fPedestal) * fNumberOfSamples / (fBlocksPerEvent * 1.0));
     fHardwareBlockSum_raw += fBlock_raw[i];
     
    double_t block = fBlock[i] / fCalibrationFactor;
    double_t sigma = fMockGaussianSigma / fCalibrationFactor;
    fBlockSumSq_raw[i] = (sigma*sigma + block*block)*fNumberOfSamples_map / (fBlocksPerEvent * 1.0);
    fBlock_min[i] = (block - 3.0 * sigma) * double_t(fNumberOfSamples_map) / (fBlocksPerEvent * 1.0);
    fBlock_max[i] = (block + 3.0 * sigma) * double_t(fNumberOfSamples_map) / (fBlocksPerEvent * 1.0);
    
    fBlockSumSq_raw[4] += fBlockSumSq_raw[i];
    fBlock_min[4] = TMath::Min(fBlock_min[i],fBlock_min[4]);
    fBlock_max[4] = TMath::Max(fBlock_max[i],fBlock_max[4]);
    }



  fSoftwareBlockSum_raw = fHardwareBlockSum_raw;

  return;
}

void QwMollerADC_Channel::EncodeEventData(std::vector<UInt_t> &buffer)
{
  Long_t localbuf[kWordsPerChannel] = {0};

  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
  } else {
    //    localbuf[4] = 0;
    for (Int_t i = 0; i < 4; i++) {
      localbuf[i*5] = fBlock_raw[i];
      localbuf[i*5+1] = fBlockSumSq_raw[i] & 0xffffffff;
      localbuf[i*5+2] = fBlockSumSq_raw[i] >> 32;
      localbuf[i*5+3] = fBlock_min[i];
      localbuf[i*5+4] = fBlock_max[i];
      
      //        localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
    }
    // The following causes many rounding errors and skips due to the check
    // that fHardwareBlockSum_raw == fSoftwareBlockSum_raw in IsGoodEvent().
    localbuf[20] = fHardwareBlockSum_raw;
    localbuf[21] = fBlockSumSq_raw[4] & 0xffffffff;
    localbuf[22] = fBlockSumSq_raw[4] >> 32;
    localbuf[23] = fBlock_min[4];
    localbuf[24] = fBlock_max[4];
    localbuf[25] = (fNumberOfSamples << 16 & 0xFFFF0000)
                | (fSequenceNumber  << 8  & 0x0000FF00);

    for (Int_t i = 0; i < kWordsPerChannel; i++){
        buffer.push_back(localbuf[i]);
    }
  }
}



Int_t QwMollerADC_Channel::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
{
  UInt_t words_read = 0;
  UInt_t localbuf[kWordsPerChannel] = {0};
  // The conversion from UInt_t to Double_t discards the sign, so we need an intermediate
  // static_cast from UInt_t to Int_t.
  Int_t localbuf_signed[kWordsPerChannel] = {0};

  if (IsNameEmpty()){
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
    words_read = fNumberOfDataWords;
  } else if (num_words_left >= fNumberOfDataWords)
    {
      for (Int_t i=0; i<kWordsPerChannel; i++){
        localbuf[i] = buffer[i];
        localbuf_signed[i] = static_cast<Int_t>(localbuf[i]);
      }

      fSoftwareBlockSum_raw = 0;
      for (Int_t i=0; i<fBlocksPerEvent; i++){
        fBlock_raw[i] = localbuf_signed[i*5];
        fBlockSumSq_raw[i] = localbuf_signed[i*5+1];
        fBlockSumSq_raw[i] += Long64_t (localbuf_signed[i*5+2]) << 32;
        fBlock_min[i] = localbuf_signed[i*5+3];
        fBlock_max[i] = localbuf_signed[i*5+4];
        fSoftwareBlockSum_raw += fBlock_raw[i];
      }
      fHardwareBlockSum_raw = localbuf_signed[20];

      /*  Permanent change in the structure of the 6th word of the ADC readout.
       *  The upper 16 bits are the number of samples, and the upper 8 of the
       *  lower 16 are the sequence number.  This matches the structure of
       *  the ADC readout in block read mode, and now also in register read mode.
       *  P.King, 2007sep04.
       */
      fSequenceNumber   = (localbuf[25]>>8)  & 0xFF;
      fNumberOfSamples  = (localbuf[25]>>16) & 0xFFFF;

      words_read = fNumberOfDataWords;

    } else
      {
        std::cerr << "QwMollerADC_Channel::ProcessEvBuffer: Not enough words!"
                  << std::endl;
      }
  return words_read;
}



void QwMollerADC_Channel::ProcessEvent()
{
  if (fNumberOfSamples == 0 && fHardwareBlockSum_raw == 0) {
    //  There isn't valid data for this channel.  Just flag it and
    //  move on.
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
      fBlock[i] = 0.0;
      fBlockM2[i] = 0.0;
    }
    fHardwareBlockSum = 0.0;
    fHardwareBlockSumM2 = 0.0;
    fErrorFlag |= kErrorFlag_sample;
  } else if (fNumberOfSamples == 0) {
    //  This is probably a more serious problem.
    QwWarning << "QwMollerADC_Channel::ProcessEvent:  Channel "
              << this->GetElementName().Data()
              << " has fNumberOfSamples==0 but has valid data in the hardware sum.  "
              << "Flag this as an error."
              << QwLog::endl;
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
      fBlock[i] = 0.0;
      fBlockM2[i] = 0.0;
    }
    fHardwareBlockSum = 0.0;
    fHardwareBlockSumM2 = 0.0;
    fErrorFlag|=kErrorFlag_sample;
  } else {
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
      fBlock[i] = fCalibrationFactor * ( (1.0 * fBlock_raw[i] * fBlocksPerEvent / fNumberOfSamples) - fPedestal );
      fBlockM2[i] = 0.0; // second moment is zero for single events
    }
    fHardwareBlockSum = fCalibrationFactor * ( (1.0 * fHardwareBlockSum_raw / fNumberOfSamples) - fPedestal );
    fHardwareBlockSumM2 = 0.0; // second moment is zero for single events
  }
  return;
}

Double_t QwMollerADC_Channel::GetAverageVolts() const
{
  //Double_t avgVolts = (fBlock[0]+fBlock[1]+fBlock[2]+fBlock[3])*kMollerADC_VoltsPerBit/fNumberOfSamples;
  Double_t avgVolts = fHardwareBlockSum * kMollerADC_VoltsPerBit / fNumberOfSamples;
  //std::cout<<"QwMollerADC_Channel::GetAverageVolts() = "<<avgVolts<<std::endl;
  return avgVolts;

}

void QwMollerADC_Channel::PrintInfo() const
{
  std::cout<<"***************************************"<<"\n";
  std::cout<<"Subsystem "<<GetSubsystemName()<<"\n"<<"\n";
  std::cout<<"Beam Instrument Type: "<<GetModuleType()<<"\n"<<"\n";
  std::cout<<"QwMollerADC channel: "<<GetElementName()<<"\n"<<"\n";
  std::cout<<"fPedestal= "<< fPedestal<<"\n";
  std::cout<<"fCalibrationFactor= "<<fCalibrationFactor<<"\n";
  std::cout<<"fBlocksPerEvent= "<<fBlocksPerEvent<<"\n"<<"\n";
  std::cout<<"fSequenceNumber= "<<fSequenceNumber<<"\n";
  std::cout<<"fNumberOfSamples= "<<fNumberOfSamples<<"\n";
  std::cout<<"fBlock_raw ";

  for (Int_t i = 0; i < fBlocksPerEvent; i++)
    std::cout << " : " << fBlock_raw[i];
  std::cout<<"\n";
  std::cout<<"fHardwareBlockSum_raw= "<<fHardwareBlockSum_raw<<"\n";
  std::cout<<"fSoftwareBlockSum_raw= "<<fSoftwareBlockSum_raw<<"\n";
  std::cout<<"fBlock ";
  for (Int_t i = 0; i < fBlocksPerEvent; i++)
    std::cout << " : " <<std::setprecision(8) << fBlock[i];
  std::cout << std::endl;

  std::cout << "fHardwareBlockSum = "<<std::setprecision(8) <<fHardwareBlockSum << std::endl;
  std::cout << "fHardwareBlockSumM2 = "<<fHardwareBlockSumM2 << std::endl;
  std::cout << "fHardwareBlockSumError = "<<fHardwareBlockSumError << std::endl;

  return;
}

void  QwMollerADC_Channel::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (IsNameEmpty()){
    //  This channel is not used, so skip filling the histograms.
  } else {
    //  Now create the histograms.
    SetDataToSaveByPrefix(prefix);

    TString basename = prefix + GetElementName();

    if(fDataToSave==kRaw)
      {
        fHistograms.resize(8+2+1, NULL);
        size_t index=0;
        for (Int_t i=0; i<fBlocksPerEvent; i++){
          fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_block%d_raw",i));
          fHistograms[index+1] = gQwHists.Construct1DHist(basename+Form("_block%d",i));
          index += 2;
        }
        fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_hw_raw"));
        fHistograms[index+1] = gQwHists.Construct1DHist(basename+Form("_hw"));
        index += 2;
        fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_sw-hw_raw"));
      }
    else if(fDataToSave==kDerived)
      {
        fHistograms.resize(4+1+1, NULL);
        Int_t index=0;
        for (Int_t i=0; i<fBlocksPerEvent; i++){
          fHistograms[index] = gQwHists.Construct1DHist(basename+Form("_block%d",i));
          index += 1;
        }
        fHistograms[index] = gQwHists.Construct1DHist(basename+Form("_hw"));
        index += 1;
        fHistograms[index] = gQwHists.Construct1DHist(basename+Form("_dev_err"));
        index += 1;
      }
    else
      {
        // this is not recognized
      }

  }
}

void  QwMollerADC_Channel::FillHistograms()
{
  Int_t index=0;
  
  if (IsNameEmpty())
    {
      //  This channel is not used, so skip creating the histograms.
    } else
      {
        if(fDataToSave==kRaw)
          {
            for (Int_t i=0; i<fBlocksPerEvent; i++)
              {
                if (fHistograms[index] != NULL && (fErrorFlag)==0)
                  fHistograms[index]->Fill(this->GetRawBlockValue(i));
                if (fHistograms[index+1] != NULL && (fErrorFlag)==0)
                  fHistograms[index+1]->Fill(this->GetBlockValue(i));
                index+=2;
              }
            if (fHistograms[index] != NULL && (fErrorFlag)==0)
              fHistograms[index]->Fill(this->GetRawHardwareSum());
            if (fHistograms[index+1] != NULL && (fErrorFlag)==0)
              fHistograms[index+1]->Fill(this->GetHardwareSum());
            index+=2;
            if (fHistograms[index] != NULL && (fErrorFlag)==0)
              fHistograms[index]->Fill(this->GetRawSoftwareSum()-this->GetRawHardwareSum());
          }
        else if(fDataToSave==kDerived)
          {
            for (Int_t i=0; i<fBlocksPerEvent; i++)
              {
                if (fHistograms[index] != NULL && (fErrorFlag)==0)
                  fHistograms[index]->Fill(this->GetBlockValue(i));
                index+=1;
              }
            if (fHistograms[index] != NULL && (fErrorFlag)==0)
              fHistograms[index]->Fill(this->GetHardwareSum());
            index+=1; 
            if (fHistograms[index] != NULL){
              if ( (kErrorFlag_sample &  fErrorFlag)==kErrorFlag_sample)
                fHistograms[index]->Fill(kErrorFlag_sample);
              if ( (kErrorFlag_SW_HW &  fErrorFlag)==kErrorFlag_SW_HW)
                fHistograms[index]->Fill(kErrorFlag_SW_HW);
              if ( (kErrorFlag_Sequence &  fErrorFlag)==kErrorFlag_Sequence)
                fHistograms[index]->Fill(kErrorFlag_Sequence);
              if ( (kErrorFlag_ZeroHW &  fErrorFlag)==kErrorFlag_ZeroHW)
                fHistograms[index]->Fill(kErrorFlag_ZeroHW);
              if ( (kErrorFlag_VQWK_Sat &  fErrorFlag)==kErrorFlag_VQWK_Sat)
                fHistograms[index]->Fill(kErrorFlag_VQWK_Sat);
              if ( (kErrorFlag_SameHW &  fErrorFlag)==kErrorFlag_SameHW)
                fHistograms[index]->Fill(kErrorFlag_SameHW);
            }
            
          }
 
    }
}

void  QwMollerADC_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  //  This channel is not used, so skip setting up the tree.
  if (IsNameEmpty()) return;

  //  Decide what to store based on prefix
  SetDataToSaveByPrefix(prefix);

  TString basename = prefix(0, (prefix.First("|") >= 0)? prefix.First("|"): prefix.Length()) + GetElementName();
  fTreeArrayIndex  = values.size();

  TString list = "";

  bHw_sum =     gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "hw_sum");
  bHw_sum_raw = gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "hw_sum_raw");
  bBlock =     gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "block");
  bBlock_raw = gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "block_raw");
  bNum_samples = gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "num_samples");
  bDevice_Error_Code = gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "Device_Error_Code");
  bSequence_number = gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "sequence_number");

  if (bHw_sum) {
    values.push_back(0.0);
    list += "hw_sum/D";
    if (fDataToSave == kMoments) {
      values.push_back(0.0);
      list += ":hw_sum_m2/D";
      values.push_back(0.0);
      list += ":hw_sum_err/D";
    }
  }

  if (bBlock) {
    values.push_back(0.0);
    list += ":block0/D";
    values.push_back(0.0);
    list += ":block1/D";
    values.push_back(0.0);
    list += ":block2/D";
    values.push_back(0.0);
    list += ":block3/D";
  }

  if (bNum_samples) {
    values.push_back(0.0);
    list += ":num_samples/D";
  }

  if (bDevice_Error_Code) {
    values.push_back(0.0);
    list += ":Device_Error_Code/D";
  }

  if (fDataToSave == kRaw) {
    if (bHw_sum_raw) {
      values.push_back(0.0);
      list += ":hw_sum_raw/D";
    }
    if (bBlock_raw) {
      values.push_back(0.0);
      list += ":block0_raw/D";
      values.push_back(0.0);
      list += ":block1_raw/D";
      values.push_back(0.0);
      list += ":block2_raw/D";
      values.push_back(0.0);
      list += ":block3_raw/D";
    }

    for(int i = 0; i < 4; i++){
     if (bBlock_raw) {
      values.push_back(0.0);
      list += Form(":SumSq1_%d/D",i);
      values.push_back(0.0);
      list += Form(":SumSq2_%d/D",i);
      values.push_back(0.0);
      list += Form(":RawMin_%d/D",i);
      values.push_back(0.0);
      list += Form(":RawMax_%d/D",i);
     }
    }

    if (bSequence_number) {
      values.push_back(0.0);
      list += ":sequence_number/D";
    }
  }

  fTreeArrayNumEntries = values.size() - fTreeArrayIndex;

  if (gQwHists.MatchDeviceParamsFromList(basename.Data())
    && (bHw_sum || bBlock || bNum_samples || bDevice_Error_Code ||
        bHw_sum_raw || bBlock_raw || bSequence_number)) {

    // This is for the RT mode
    if (list == "hw_sum/D")
      list = basename+"/D";

    if (kDEBUG)
      QwMessage << "base name " << basename << " List " << list << QwLog::endl;

    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }

  if (kDEBUG) {
    std::cerr << "QwMollerADC_Channel::ConstructBranchAndVector: fTreeArrayIndex==" << fTreeArrayIndex
              << "; fTreeArrayNumEntries==" << fTreeArrayNumEntries
              << "; values.size()==" << values.size()
              << "; list==" << list
              << std::endl;
  }
}

void  QwMollerADC_Channel::ConstructBranch(TTree *tree, TString &prefix)
{
  //  This channel is not used, so skip setting up the tree.
  if (IsNameEmpty()) return;

  TString basename = prefix + GetElementName();
  tree->Branch(basename,&fHardwareBlockSum,basename+"/D");
  if (kDEBUG){
    std::cerr << "QwMollerADC_Channel::ConstructBranchAndVector: fTreeArrayIndex==" << fTreeArrayIndex
              << "; fTreeArrayNumEntries==" << fTreeArrayNumEntries
              << std::endl;
  }
}


void  QwMollerADC_Channel::FillTreeVector(std::vector<Double_t> &values) const
{
  if (IsNameEmpty()) {
    //  This channel is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries <= 0) {
    if (bDEBUG) std::cerr << "QwMollerADC_Channel::FillTreeVector:  fTreeArrayNumEntries=="
              << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries){
    if (bDEBUG) std::cerr << "QwMollerADC_Channel::FillTreeVector:  values.size()=="
              << values.size()
              << "; fTreeArrayIndex+fTreeArrayNumEntries=="
              << fTreeArrayIndex+fTreeArrayNumEntries
              << std::endl;
  } else {

    UInt_t index = fTreeArrayIndex;

    // hw_sum
    if (bHw_sum) {
      values[index++] = this->GetHardwareSum();
      if (fDataToSave == kMoments) {
        values[index++] = this->GetHardwareSumM2();
        values[index++] = this->GetHardwareSumError();
      }
    }

    if (bBlock) {
      for (Int_t i = 0; i < fBlocksPerEvent; i++) {
        // blocki
        values[index++] = this->GetBlockValue(i);
      }
    }

    // num_samples
    if (bNum_samples)
      values[index++] =
          (fDataToSave == kMoments)? this->fGoodEventCount: this->fNumberOfSamples;

    // Device_Error_Code
    if (bDevice_Error_Code)
      values[index++] = this->fErrorFlag;

    if (fDataToSave == kRaw)
      {
        // hw_sum_raw
        if (bHw_sum_raw)
          values[index++] = this->GetRawHardwareSum();

        if (bBlock_raw) {
          for (Int_t i = 0; i < fBlocksPerEvent; i++) {
            // blocki_raw
            values[index++] = this->GetRawBlockValue(i);
          }
        }

        for(int i = 0; i < 4; i++){
        values[index++] = fBlockSumSq_raw[i] & 0xffffffff;
        values[index++] = fBlockSumSq_raw[i] >> 32;
        values[index++] = fBlock_min[i];
        values[index++] = fBlock_max[i];
        }
        // sequence_number
        if (bSequence_number)
          values[index++] = this->fSequenceNumber;
      }
  }
}

QwMollerADC_Channel& QwMollerADC_Channel::operator= (const QwMollerADC_Channel &value)
{
  if(this ==&value) return *this;

  if (!IsNameEmpty()) {
    VQwHardwareChannel::operator=(value);
    for (Int_t i=0; i<fBlocksPerEvent; i++){
      this->fBlock_raw[i] = value.fBlock_raw[i];
      this->fBlock[i]     = value.fBlock[i];
      this->fBlockM2[i]   = value.fBlockM2[i];
      this->fBlockSumSq_raw[i] = value.fBlockSumSq_raw[i];
      this->fBlock_min[i]     = value.fBlock_min[i];
      this->fBlock_max[i]     = value.fBlock_max[i];
    }
    this->fHardwareBlockSum_raw = value.fHardwareBlockSum_raw;
    this->fSoftwareBlockSum_raw = value.fSoftwareBlockSum_raw;
    this->fHardwareBlockSum = value.fHardwareBlockSum;
    this->fHardwareBlockSumM2 = value.fHardwareBlockSumM2;
    this->fHardwareBlockSumError = value.fHardwareBlockSumError;
    this->fNumberOfSamples = value.fNumberOfSamples;
    this->fSequenceNumber  = value.fSequenceNumber;
   

  }
  return *this;
}

void QwMollerADC_Channel::AssignScaledValue(const QwMollerADC_Channel &value,
                                 Double_t scale)
{
  if(this == &value) return;

  if (!IsNameEmpty()) {
    for (Int_t i=0; i<fBlocksPerEvent; i++){
      this->fBlock_raw[i] = value.fBlock_raw[i];   // Keep this?
      this->fBlock[i]   = value.fBlock[i]   * scale;
      this->fBlockM2[i] = value.fBlockM2[i] * scale * scale;
      this->fBlockSumSq_raw[i] = value.fBlockSumSq_raw[i];
      this->fBlock_min[i]     = value.fBlock_min[i];
      this->fBlock_max[i]     = value.fBlock_max[i];

    }
    this->fHardwareBlockSum_raw = value.fHardwareBlockSum_raw;  // Keep this?
    this->fSoftwareBlockSum_raw = value.fSoftwareBlockSum_raw;  // Keep this?
    this->fHardwareBlockSum   = value.fHardwareBlockSum * scale;
    this->fHardwareBlockSumM2 = value.fHardwareBlockSumM2 * scale * scale;
    this->fHardwareBlockSumError = value.fHardwareBlockSumError;   // Keep this?
    this->fGoodEventCount  = value.fGoodEventCount;
    this->fNumberOfSamples = value.fNumberOfSamples;
    this->fSequenceNumber  = value.fSequenceNumber;
    this->fErrorFlag       = value.fErrorFlag;
  }
}

void QwMollerADC_Channel::AssignValueFrom(const  VQwDataElement* valueptr)
{
  const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this = *tmpptr;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::AssignValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void QwMollerADC_Channel::AddValueFrom(const  VQwHardwareChannel* valueptr)
{
  const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this += *tmpptr;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::AddValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void QwMollerADC_Channel::SubtractValueFrom(const  VQwHardwareChannel* valueptr)
{
  const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this -= *tmpptr;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::SubtractValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void QwMollerADC_Channel::MultiplyBy(const VQwHardwareChannel* valueptr)
{
  const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this *= *tmpptr;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::MultiplyBy = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}
void QwMollerADC_Channel::DivideBy(const VQwHardwareChannel* valueptr)
{
  const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this /= *tmpptr;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::DivideBy = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}


const QwMollerADC_Channel QwMollerADC_Channel::operator+ (const QwMollerADC_Channel &value) const
{
  QwMollerADC_Channel result = *this;
  result += value;
  return result;
}

QwMollerADC_Channel& QwMollerADC_Channel::operator+= (const QwMollerADC_Channel &value)
{

  if (!IsNameEmpty()) {
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
      this->fBlock[i] += value.fBlock[i];
      this->fBlock_raw[i] += value.fBlock_raw[i];

      this->fBlock_min[i] = TMath::Min(fBlock_min[i],value.fBlock_min[i]);
      this->fBlock_max[i] = TMath::Max(fBlock_max[i],value.fBlock_max[i]);    
      this->fBlockSumSq_raw[i] += value.fBlockSumSq_raw[i];
      this->fBlockM2[i] = 0.0;
    }
    this->fHardwareBlockSum_raw = value.fHardwareBlockSum_raw;
    this->fSoftwareBlockSum_raw = value.fSoftwareBlockSum_raw;
    this->fHardwareBlockSum    += value.fHardwareBlockSum;
    this->fHardwareBlockSumM2   = 0.0;
    this->fNumberOfSamples     += value.fNumberOfSamples;
    this->fSequenceNumber       = 0;
    this->fErrorFlag            |= (value.fErrorFlag);

  }

  return *this;
}

const QwMollerADC_Channel QwMollerADC_Channel::operator- (const QwMollerADC_Channel &value) const
{
  QwMollerADC_Channel result = *this;
  result -= value;
  return result;
}

QwMollerADC_Channel& QwMollerADC_Channel::operator-= (const QwMollerADC_Channel &value)
{
  if (!IsNameEmpty()){
    for (Int_t i=0; i<fBlocksPerEvent; i++){
      this->fBlock[i] -= value.fBlock[i];
      this->fBlock_raw[i] = 0;
      this->fBlockSumSq_raw[i] = value.fBlockSumSq_raw[i];
      this->fBlock_min[i] = TMath::Min(fBlock_min[i],value.fBlock_min[i]);
      this->fBlock_max[i] = TMath::Max(fBlock_max[i],value.fBlock_max[i]);  
      this->fBlockM2[i] = 0.0;
    }
    this->fHardwareBlockSum_raw = 0;
    this->fSoftwareBlockSum_raw = 0;
    this->fHardwareBlockSum    -= value.fHardwareBlockSum;
    this->fHardwareBlockSumM2   = 0.0;
    this->fNumberOfSamples     += value.fNumberOfSamples;
    this->fSequenceNumber       = 0;
    this->fErrorFlag           |= (value.fErrorFlag);
  }

  return *this;
}

const QwMollerADC_Channel QwMollerADC_Channel::operator* (const QwMollerADC_Channel &value) const
{
  QwMollerADC_Channel result = *this;
  result *= value;
  return result;
}

QwMollerADC_Channel& QwMollerADC_Channel::operator*= (const QwMollerADC_Channel &value)
{
  if (!IsNameEmpty()){
    for (Int_t i=0; i<fBlocksPerEvent; i++){
      this->fBlock[i] *= value.fBlock[i];
      this->fBlock_raw[i] *= value.fBlock_raw[i];
      if(fBlockSumSq_raw[i] != 0){
        this->fBlockSumSq_raw[i] *= value.fBlockSumSq_raw[i];
        this->fBlock_min[i] *= value.fBlock_min[i];
        this->fBlock_max[i] *= value.fBlock_max[i];
      } else {
        this->fBlockSumSq_raw[i] = value.fBlockSumSq_raw[i];
        this->fBlock_min[i] = value.fBlock_min[i];
        this->fBlock_max[i] = value.fBlock_max[i];
      }
      this->fBlockM2[i] = 0.0;
    }
    this->fHardwareBlockSum_raw *= value.fHardwareBlockSum_raw;
    this->fSoftwareBlockSum_raw *= value.fSoftwareBlockSum_raw;
    this->fHardwareBlockSum     *= value.fHardwareBlockSum;
    this->fHardwareBlockSumM2    = 0.0;
    this->fNumberOfSamples      *= value.fNumberOfSamples;
    this->fSequenceNumber        = 0;
    this->fErrorFlag            |= (value.fErrorFlag);
  }

  return *this;
}

VQwHardwareChannel& QwMollerADC_Channel::operator+=(const VQwHardwareChannel *source)
{
  const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(source);
  if (tmpptr!=NULL){
    *this += *tmpptr;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::operator+= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}
VQwHardwareChannel& QwMollerADC_Channel::operator-=(const VQwHardwareChannel *source)
{
  const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(source);
  if (tmpptr!=NULL){
    *this -= *tmpptr;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::operator-= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}
VQwHardwareChannel& QwMollerADC_Channel::operator*=(const VQwHardwareChannel *source)
{
  const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(source);
  if (tmpptr!=NULL){
    *this *= *tmpptr;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::operator*= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}
VQwHardwareChannel& QwMollerADC_Channel::operator/=(const VQwHardwareChannel *source)
{
  const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(source);
  if (tmpptr!=NULL){
    *this /= *tmpptr;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::operator/= "
        +source->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
  return *this;
}


void QwMollerADC_Channel::Sum(const QwMollerADC_Channel &value1, const QwMollerADC_Channel &value2)
{
  *this  = value1;
  *this += value2;
}

void QwMollerADC_Channel::Difference(const QwMollerADC_Channel &value1, const QwMollerADC_Channel &value2)
{
  *this  = value1;
  *this -= value2;
}

void QwMollerADC_Channel::Ratio(const QwMollerADC_Channel &numer, const QwMollerADC_Channel &denom)
{
  if (!IsNameEmpty()) {
    *this  = numer;
    *this /= denom;

    //  Set the raw values to zero.
    for (Int_t i = 0; i < fBlocksPerEvent; i++) fBlock_raw[i] = 0;
    fHardwareBlockSum_raw = 0;
    fSoftwareBlockSum_raw = 0;
    // Remaining variables
    fNumberOfSamples      = denom.fNumberOfSamples;
    fSequenceNumber       = 0;
    fGoodEventCount       = denom.fGoodEventCount;
    fErrorFlag            = (numer.fErrorFlag|denom.fErrorFlag);
  }
}

QwMollerADC_Channel& QwMollerADC_Channel::operator/= (const QwMollerADC_Channel &denom)
{
  //  In this function, leave the "raw" variables untouched.
  //  
  Double_t ratio;
  Double_t variance;
  if (!IsNameEmpty()) {
    // The variances are calculated using the following formula:
    //   Var[ratio] = ratio^2 (Var[numer] / numer^2 + Var[denom] / denom^2)
    //
    // This requires that both the numerator and denominator are non-zero!
    //
    for (Int_t i = 0; i < 4; i++) {
      if (this->fBlock[i] != 0.0 && denom.fBlock[i] != 0.0){
        ratio    = (this->fBlock[i]) / (denom.fBlock[i]);
        variance =  ratio * ratio *
           (this->fBlockM2[i] / this->fBlock[i] / this->fBlock[i]
          + denom.fBlockM2[i] / denom.fBlock[i] / denom.fBlock[i]);
        fBlock[i]   = ratio;
        fBlockM2[i] = variance;
      } else if (this->fBlock[i] == 0.0) {
        this->fBlock[i]   = 0.0;
        this->fBlockM2[i] = 0.0;
      } else {
        QwVerbose << "Attempting to divide by zero block in " 
                  << GetElementName() << QwLog::endl;
        fBlock[i]   = 0.0;
        fBlockM2[i] = 0.0;
      }
    }
    if (this->fHardwareBlockSum != 0.0 && denom.fHardwareBlockSum != 0.0){
      ratio    =  (this->fHardwareBlockSum) / (denom.fHardwareBlockSum);
      variance =  ratio * ratio *
        (this->fHardwareBlockSumM2 / this->fHardwareBlockSum / this->fHardwareBlockSum
         + denom.fHardwareBlockSumM2 / denom.fHardwareBlockSum / denom.fHardwareBlockSum);
      fHardwareBlockSum   = ratio;
      fHardwareBlockSumM2 = variance;
    } else if (this->fHardwareBlockSum == 0.0) {
      fHardwareBlockSum   = 0.0;
      fHardwareBlockSumM2 = 0.0;
    } else {
      QwVerbose << "Attempting to divide by zero sum in " 
                << GetElementName() << QwLog::endl;
      fHardwareBlockSumM2 = 0.0;
    }
    // Remaining variables
    //  Don't change fNumberOfSamples, fSequenceNumber, fGoodEventCount,
    //  'OR' the HW error codes in the fErrorFlag values together.
    fErrorFlag |= (denom.fErrorFlag);//mix only the hardware error codes
  }

  // Nanny
  if (fHardwareBlockSum != fHardwareBlockSum)
    QwWarning << "Angry Nanny: NaN detected in " << GetElementName() << QwLog::endl;

  return *this;
}

//--------------------------------------------------------------------------------------------

void QwMollerADC_Channel::ArcTan(const QwMollerADC_Channel &value)
{
  if (!IsNameEmpty()) {
    this->fHardwareBlockSum = 0.0;
    for (Int_t i=0; i<fBlocksPerEvent; i++) {
      this->fBlock[i] = atan(value.fBlock[i]);
      this->fHardwareBlockSum += this->fBlock[i];
    }
    this->fHardwareBlockSum /= fBlocksPerEvent;
  }

  return;
}

//--------------------------------------------------------------------------------------------
void QwMollerADC_Channel::Product(const QwMollerADC_Channel &value1, const QwMollerADC_Channel &value2)
{
  if (!IsNameEmpty()){
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
      this->fBlock[i] = (value1.fBlock[i]) * (value2.fBlock[i]);
      this->fBlock_raw[i] = 0;
      // For a single event the second moment is still zero
      this->fBlockM2[i] = 0.0;
      if(value1.fBlockSumSq_raw[i] != 0){
        this->fBlockSumSq_raw[i] = value1.fBlockSumSq_raw[i];
        this->fBlock_min[i]     = value1.fBlock_min[i];
        this->fBlock_max[i]     = value1.fBlock_max[i];
      } else {
        this->fBlockSumSq_raw[i] = value2.fBlockSumSq_raw[i];
        this->fBlock_min[i]     = value2.fBlock_min[i];
        this->fBlock_max[i]     = value2.fBlock_max[i];
      }
    }

    // For a single event the second moment is still zero
    this->fHardwareBlockSumM2 = 0.0;

    this->fSoftwareBlockSum_raw = 0;
    this->fHardwareBlockSum_raw = value1.fHardwareBlockSum_raw * value2.fHardwareBlockSum_raw;
    this->fHardwareBlockSum = value1.fHardwareBlockSum * value2.fHardwareBlockSum;
    this->fNumberOfSamples = value1.fNumberOfSamples;
    this->fSequenceNumber  = 0;
    this->fErrorFlag       = (value1.fErrorFlag|value2.fErrorFlag);
  }
  return;
}

/**
This function will add a offset to the hw_sum and add the same offset for blocks.
 */
void QwMollerADC_Channel::AddChannelOffset(Double_t offset)
{
  if (!IsNameEmpty()){
    fHardwareBlockSum += offset;
    for (Int_t i=0; i<fBlocksPerEvent; i++) 
      fBlock[i] += offset;
  }
  return;
}

void QwMollerADC_Channel::Scale(Double_t scale)
{
  if (!IsNameEmpty()){
      for (Int_t i = 0; i < fBlocksPerEvent; i++) {
        fBlock[i] *= scale;
        fBlockM2[i] *= scale * scale;
      }
      fHardwareBlockSum *= scale;
      fHardwareBlockSumM2 *= scale * scale;
    }
}


void QwMollerADC_Channel::DivideBy(const QwMollerADC_Channel &denom)
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
void QwMollerADC_Channel::AccumulateRunningSum(const QwMollerADC_Channel& value, Int_t count, Int_t ErrorMask)
{
  /*
    note:
    The AccumulateRunningSum is called on a dedicated subsystem array object and
    for the standard running avg computations we only need value.fErrorFlag==0
    events to be included in the running avg. So the "berror" conditions is only
    used for the stability check purposes.

    The need for this check below came due to fact that when routine
    DeaccumulateRunningSum is called the errorflag is updated with
    the kBeamStabilityError flag (+ configuration flags for global errors) and
    need to make sure we remove this flag and any configuration flags before
    checking the (fErrorFlag != 0) condition
    
    See how the stability check is implemented in the QwEventRing class

    Rakitha
  */

  if(count==0){
    count = value.fGoodEventCount;
  }

  Int_t n1 = fGoodEventCount;
  Int_t n2 = count;

  // If there are no good events, check the error flag
  if (n2 == 0 && (value.fErrorFlag == 0)) {
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
  Double_t M11 = fHardwareBlockSum;
  Double_t M12 = value.fHardwareBlockSum;
  Double_t M22 = value.fHardwareBlockSumM2;

  //if(this->GetElementName() == "bcm_an_ds3" && ErrorMask == kPreserveError){QwError << "count=" << fGoodEventCount << "  n=" << n << QwLog::endl;    }
  if (n2 == 0) {
    // no good events for addition
    return;
  } else if (n2 == -1) {
    // simple version for removal of single event from the sum
    fGoodEventCount--;
    if (n > 1) {
      fHardwareBlockSum -= (M12 - M11) / n;
      fHardwareBlockSumM2 -= (M12 - M11)
        * (M12 - fHardwareBlockSum); // note: using updated mean
      // and for individual blocks
      for (Int_t i = 0; i < 4; i++) {
        M11 = fBlock[i];
        M12 = value.fBlock[i];
        M22 = value.fBlockM2[i];
        fBlock[i] -= (M12 - M11) / n;
        fBlockM2[i] -= (M12 - M11) * (M12 - fBlock[i]); // note: using updated mean
      }
    } else if (n == 1) {
      fHardwareBlockSum -= (M12 - M11) / n;
      fHardwareBlockSumM2 -= (M12 - M11)
        * (M12 - fHardwareBlockSum); // note: using updated mean
      if (fabs(fHardwareBlockSumM2) < 10.*std::numeric_limits<double>::epsilon())
        fHardwareBlockSumM2 = 0; // rounding
      // and for individual blocks
      for (Int_t i = 0; i < 4; i++) {
        M11 = fBlock[i];
        M12 = value.fBlock[i];
        M22 = value.fBlockM2[i];
        fBlock[i] -= (M12 - M11) / n;
        fBlockM2[i] -= (M12 - M11) * (M12 - fBlock[i]); // note: using updated mean
        if (fabs(fBlockM2[i]) < 10.*std::numeric_limits<double>::epsilon())
          fBlockM2[i] = 0; // rounding
      }
    } else if (n == 0) {
      fHardwareBlockSum -= M12;
      fHardwareBlockSumM2 -= M22;
      if (fabs(fHardwareBlockSum) < 10.*std::numeric_limits<double>::epsilon())
        fHardwareBlockSum = 0; // rounding
      if (fabs(fHardwareBlockSumM2) < 10.*std::numeric_limits<double>::epsilon())
        fHardwareBlockSumM2 = 0; // rounding
      // and for individual blocks
      for (Int_t i = 0; i < 4; i++) {
        M11 = fBlock[i];
        M12 = value.fBlock[i];
        M22 = value.fBlockM2[i];
        fBlock[i] -= M12;
        fBlockM2[i] -= M22;
        if (fabs(fBlock[i]) < 10.*std::numeric_limits<double>::epsilon())
          fBlock[i] = 0; // rounding
        if (fabs(fBlockM2[i]) < 10.*std::numeric_limits<double>::epsilon())
          fBlockM2[i] = 0; // rounding
      }
    } else {
      QwWarning << "Running sum has deaccumulated to negative good events." << QwLog::endl;
    }
  } else if (n2 == 1) {
    // simple version for addition of single event
    fGoodEventCount++;
    fHardwareBlockSum += (M12 - M11) / n;
    fHardwareBlockSumM2 += (M12 - M11)
         * (M12 - fHardwareBlockSum); // note: using updated mean
    // and for individual blocks
    for (Int_t i = 0; i < 4; i++) {
      M11 = fBlock[i];
      M12 = value.fBlock[i];
      M22 = value.fBlockM2[i];
      fBlock[i] += (M12 - M11) / n;
      fBlockM2[i] += (M12 - M11) * (M12 - fBlock[i]); // note: using updated mean
    }
  } else if (n2 > 1) {
    // general version for addition of multi-event sets
    fGoodEventCount += n2;
    fHardwareBlockSum += n2 * (M12 - M11) / n;
    fHardwareBlockSumM2 += M22 + n1 * n2 * (M12 - M11) * (M12 - M11) / n;
    // and for individual blocks
    for (Int_t i = 0; i < 4; i++) {
      M11 = fBlock[i];
      M12 = value.fBlock[i];
      M22 = value.fBlockM2[i];
      fBlock[i] += n2 * (M12 - M11) / n;
      fBlockM2[i] += M22 + n1 * n2 * (M12 - M11) * (M12 - M11) / n;
    }
  }

  // Nanny
  if (fHardwareBlockSum != fHardwareBlockSum)
    QwWarning << "Angry Nanny: NaN detected in " << GetElementName() << QwLog::endl;
}


void QwMollerADC_Channel::CalculateRunningAverage()
{
  if (fGoodEventCount <= 0)
    {
      for (Int_t i = 0; i < fBlocksPerEvent; i++) {
        fBlockError[i] = 0.0;
      }
      fHardwareBlockSumError = 0.0;
    }
  else
    {
      // We use a biased estimator by dividing by n.  Use (n - 1) to get the
      // unbiased estimator for the standard deviation.
      //
      // Note we want to calculate the error here, not sigma:
      //    sigma = sqrt(M2 / n);
      //    error = sigma / sqrt (n) = sqrt(M2) / n;
      for (Int_t i = 0; i < fBlocksPerEvent; i++)
        fBlockError[i] = sqrt(fBlockM2[i]) / fGoodEventCount;
      fHardwareBlockSumError = sqrt(fHardwareBlockSumM2) / fGoodEventCount;

      // Stability check 83951872
      if ((fStability>0) &&( (fErrorConfigFlag & kStabilityCut) == kStabilityCut)) {
        // check to see the channel has stability cut activated in the event cut file
	if (GetValueWidth() > fStability){
	  // if the width is greater than the stability required flag the event
	  fErrorFlag = kBeamStabilityError;
	} else
	  fErrorFlag = 0;
      }
    }
}


void QwMollerADC_Channel::PrintValue() const
{
  QwMessage << std::setprecision(8)
            << std::setw(18) << std::left << GetSubsystemName()    << " "
            << std::setw(18) << std::left << GetModuleType()       << " "
            << std::setw(18) << std::left << GetElementName()      << " "
            << std::setw(12) << std::left << GetHardwareSum()      << " +/- "
            << std::setw(12) << std::left << GetHardwareSumError() << " sig "
            << std::setw(12) << std::left << GetHardwareSumWidth() << " "
            << std::setw(10) << std::left << GetGoodEventCount()   << " "
            << std::setw(12) << std::left << GetBlockValue(0)      << " +/- "
            << std::setw(12) << std::left << GetBlockErrorValue(0) << " "
            << std::setw(12) << std::left << GetBlockValue(1)      << " +/- "
            << std::setw(12) << std::left << GetBlockErrorValue(1) << " "
            << std::setw(12) << std::left << GetBlockValue(2)      << " +/- "
            << std::setw(12) << std::left << GetBlockErrorValue(2) << " "
            << std::setw(12) << std::left << GetBlockValue(3)      << " +/- "
            << std::setw(12) << std::left << GetBlockErrorValue(3) << " "
            << std::setw(12) << std::left << fGoodEventCount       << " "
            << QwLog::endl;
  /*
    //for Debudding
            << std::setw(12) << std::left << fErrorFlag << " err "
            << std::setw(12) << std::left << fErrorConfigFlag << " c-err "

  */
}

std::ostream& operator<< (std::ostream& stream, const QwMollerADC_Channel& channel)
{
  stream << channel.GetHardwareSum();
  return stream;
}

/**
 * Blind this channel as an asymmetry
 * @param blinder Blinder
 */
void QwMollerADC_Channel::Blind(const QwBlinder *blinder)
{
  if (!IsNameEmpty()) {
    if (blinder->IsBlinderOkay() && ((fErrorFlag)==0) ){
      for (Int_t i = 0; i < fBlocksPerEvent; i++)
        blinder->BlindValue(fBlock[i]);
      blinder->BlindValue(fHardwareBlockSum);
    } else {
      blinder->ModifyThisErrorCode(fErrorFlag);
      for (Int_t i = 0; i < fBlocksPerEvent; i++)
	fBlock[i] = QwBlinder::kValue_BlinderFail;
      fHardwareBlockSum =  QwBlinder::kValue_BlinderFail;
    }
  }
  return;
}

/**
 * Blind this channel as a difference with specified yield
 * @param blinder Blinder
 * @param yield Corresponding yield
 */
void QwMollerADC_Channel::Blind(const QwBlinder *blinder, const QwMollerADC_Channel& yield)
{
  if (!IsNameEmpty()) {
    if (blinder->IsBlinderOkay() && ((fErrorFlag) ==0) ){
      for (Int_t i = 0; i < fBlocksPerEvent; i++)
        blinder->BlindValue(fBlock[i], yield.fBlock[i]);
      blinder->BlindValue(fHardwareBlockSum, yield.fHardwareBlockSum);
    } else {
      blinder->ModifyThisErrorCode(fErrorFlag);//update the HW error code
      for (Int_t i = 0; i < fBlocksPerEvent; i++)
	fBlock[i] = QwBlinder::kValue_BlinderFail * yield.fBlock[i];
      fHardwareBlockSum = QwBlinder::kValue_BlinderFail * yield.fHardwareBlockSum;
    }
  }
  return;
}

Bool_t QwMollerADC_Channel::MatchSequenceNumber(size_t seqnum)
{

  Bool_t status = kTRUE;
  if (!IsNameEmpty()){
    status = (fSequenceNumber==seqnum);
    }
  return status;
}

Bool_t QwMollerADC_Channel::MatchNumberOfSamples(size_t numsamp)
{
  Bool_t status = kTRUE;
  if (!IsNameEmpty()){
    status = (fNumberOfSamples==numsamp);
    if (! status){
      if (bDEBUG)
        std::cerr << "QwMollerADC_Channel::MatchNumberOfSamples:  Channel "
                << GetElementName()
                << " had fNumberOfSamples==" << fNumberOfSamples
                << " and was supposed to have " << numsamp
                << std::endl;
      //      PrintChannel();
    }
  }
  return status;
}

Bool_t QwMollerADC_Channel::ApplySingleEventCuts(Double_t LL,Double_t UL)//only check to see HW_Sum is within these given limits
{
  Bool_t status = kFALSE;

  if (UL < LL){
    status=kTRUE;
  } else  if (GetHardwareSum()<=UL && GetHardwareSum()>=LL){
    if ((fErrorFlag & kPreserveError)!=0)
      status=kTRUE;
    else
      status=kFALSE;//If the device HW is failed
  }
  std::cout<<(this->fErrorFlag & kPreserveError)<<std::endl;
  return status;
}

Bool_t QwMollerADC_Channel::ApplySingleEventCuts()//This will check the limits and update event_flags and error counters
{
  Bool_t status;

  if (bEVENTCUTMODE>=2){//Global switch to ON/OFF event cuts set at the event cut file

    if (fULimit < fLLimit){
      status=kTRUE;
    } else  if (GetHardwareSum()<=fULimit && GetHardwareSum()>=fLLimit){
      if ((fErrorFlag)==0)
        status=kTRUE;
      else
        status=kFALSE;//If the device HW is failed
    }
    else{
      if (GetHardwareSum()> fULimit)
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

void  QwMollerADC_Channel::PrintErrorCounterHead()
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
  return;
}

void  QwMollerADC_Channel::PrintErrorCounterTail()
{
  QwMessage << "---------------------------------------------------------------------------------------------" << QwLog::endl;
  return;
}

void  QwMollerADC_Channel::PrintErrorCounters() const
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
  return;
}

void QwMollerADC_Channel::ScaledAdd(Double_t scale, const VQwHardwareChannel *value)
{
  const QwMollerADC_Channel* input = dynamic_cast<const QwMollerADC_Channel*>(value);
  
  // follows same steps as += but w/ scaling factor
  if(input!=NULL && !IsNameEmpty()){
    //     QwWarning << "Adding " << input->GetElementName()
    //               << " to " << GetElementName()
    //               << " with scale factor " << scale
    //               << QwLog::endl;
    //     PrintValue();
    //     input->PrintValue();
    for(Int_t i = 0; i < fBlocksPerEvent; i++){
      this -> fBlock[i] += scale * input->fBlock[i];
      this->fBlock_raw[i] = 0;
      this -> fBlockM2[i] = 0.0;
      this->fBlockSumSq_raw[i] = input->fBlockSumSq_raw[i];
      this->fBlock_min[i]     = input->fBlock_min[i];
      this->fBlock_max[i]     = input->fBlock_max[i];
    }
    this->fHardwareBlockSum_raw = 0;
    this->fSoftwareBlockSum_raw = 0;
    this -> fHardwareBlockSum += scale * input->fHardwareBlockSum;
    this -> fHardwareBlockSumM2 = 0.0;
    this -> fNumberOfSamples += input->fNumberOfSamples;
    this -> fSequenceNumber  =  0;
    this -> fErrorFlag       |= (input->fErrorFlag);   
  }
  //   QwWarning << "Finsihed with addition"  << QwLog::endl;
  //   PrintValue();
}

void QwMollerADC_Channel::CopyParameters(const VQwHardwareChannel* valueptr){
    const QwMollerADC_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwMollerADC_Channel*>(valueptr);
  if (tmpptr!=NULL){
    fNumberOfSamples = tmpptr->fNumberOfSamples;
    fNumberOfSamples_map = tmpptr->fNumberOfSamples_map;
    fMockGaussianSigma = tmpptr->fMockGaussianSigma;
  } else {
    TString loc="Standard exception from QwMollerADC_Channel::CopyParameters"
        +valueptr->GetElementName()+" "
        +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  }
};

#ifdef __USE_DATABASE__
void QwMollerADC_Channel::AddErrEntriesToList(std::vector<QwErrDBInterface> &row_list)
{

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

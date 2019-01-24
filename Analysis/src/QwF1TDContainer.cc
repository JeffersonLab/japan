
#include "QwF1TDContainer.h"

#include "QwColor.h"
#include "QwLog.h"

#include "TROOT.h"
#include "TFile.h"


/**
 *  \file   QwF1TDContainer.cc
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Tuesday, September 14 23:04:36 EDT 2010
 */

//
//
//
//  QwF1TDC
//
//
//
//


const Int_t QwF1TDC::fWordsPerBuffer = 16;
const Int_t QwF1TDC::fMaxF1TDCChannelNumber = 64;

// prefare when I decide to use Stream...
//
// QwF1TDC::QwF1TDC()
// {
//   fROC                 = -1;
//   fSlot                = -1;

//   fChannelNumber       = 0;

//   fF1TDC_refcnt        = 0;
//   fF1TDC_hsdiv         = 0;
//   fF1TDC_refclkdiv     = 0;
//   fF1TDC_trigwin       = 0;
//   fF1TDC_triglat       = 0;
  
//   fF1TDC_tframe_ns     = 0.0;
//   fF1TDC_full_range_ns = 0.0;
//   fF1TDC_window_ns     = 0.0;
//   fF1TDC_latency_ns    = 0.0;
//   fF1TDC_resolution_ns = 0.0;
  
//   fF1TDC_t_offset      = 0.0;

//   fF1TDC_SEU_counter   = NULL;
//   fF1TDC_EMM_counter   = NULL;
//   fF1TDC_SYN_counter   = NULL;
//   fF1TDC_TFO_counter   = NULL;

//   fF1TDC_RLF_counter   = NULL;
//   fF1TDC_HFO_counter   = NULL;
//   fF1TDC_OFO_counter   = NULL;

//   fF1TDC_FDF_counter   = NULL;
//   fF1TDC_S30_counter   = NULL;


//   fReferenceSignals    = NULL;

//   fReferenceSlotFlag   = false;
//   fF1TDCNormResFlag    = true;
//   fF1TDCSyncFlag       = true;

//   fF1TDCFactor         = 0.0;

//   fF1TDCIndex          = -1;
//   fF1BankIndex         = -1;

//   fBuffer = NULL;

// }

QwF1TDC::QwF1TDC()
{
  fROC                 = -1;
  fSlot                = -1;

  fChannelNumber       = 0;

  fF1TDC_refcnt        = 0;
  fF1TDC_hsdiv         = 0;
  fF1TDC_refclkdiv     = 0;
  fF1TDC_trigwin       = 0;
  fF1TDC_triglat       = 0;
  
  fF1TDC_tframe_ns     = 0.0;
  fF1TDC_full_range_ns = 0.0;
  fF1TDC_window_ns     = 0.0;
  fF1TDC_latency_ns    = 0.0;
  fF1TDC_resolution_ns = 0.0;
  
  fF1TDC_t_offset      = 0.0;

  fF1TDC_SEU_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_EMM_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_SYN_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_TFO_counter = new UInt_t[fMaxF1TDCChannelNumber];

  fF1TDC_RLF_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_HFO_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_OFO_counter = new UInt_t[fMaxF1TDCChannelNumber];

  fF1TDC_FDF_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_S30_counter = new UInt_t[fMaxF1TDCChannelNumber];

  fF1TDC_RFM_counter = 0;

  fReferenceSignals    = NULL;

  fReferenceSlotFlag   = false;
  fF1TDCNormResFlag    = true;
  fF1TDCSyncFlag       = true;

  fF1TDCFactor         = 0.0;

  fF1TDCIndex          = -1;
  fF1BankIndex         = -1;

  fBuffer = new UInt_t[fWordsPerBuffer];

}



QwF1TDC::QwF1TDC(const Int_t roc, const Int_t slot)
{
  fROC                 = roc;
  // fROCIndex            = -1;
  fSlot                = slot;

  fChannelNumber       = 0;

  fF1TDC_refcnt        = 0;
  fF1TDC_hsdiv         = 0;
  fF1TDC_refclkdiv     = 0;
  fF1TDC_trigwin       = 0;
  fF1TDC_triglat       = 0;
  
  fF1TDC_tframe_ns     = 0.0;
  fF1TDC_full_range_ns = 0.0;
  fF1TDC_window_ns     = 0.0;
  fF1TDC_latency_ns    = 0.0;
  fF1TDC_resolution_ns = 0.0;

  fF1TDC_t_offset      = 0.0;

  fF1TDC_SEU_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_EMM_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_SYN_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_TFO_counter = new UInt_t[fMaxF1TDCChannelNumber];

  fF1TDC_RLF_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_HFO_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_OFO_counter = new UInt_t[fMaxF1TDCChannelNumber];

  fF1TDC_FDF_counter = new UInt_t[fMaxF1TDCChannelNumber];
  fF1TDC_S30_counter = new UInt_t[fMaxF1TDCChannelNumber];

  fF1TDC_RFM_counter = 0;

  fReferenceSignals    = NULL;

  fReferenceSlotFlag   = false;
  fF1TDCNormResFlag    = true;
  fF1TDCSyncFlag       = true;

  fF1TDCFactor         = 0.0;

  fF1TDCIndex          = -1;
  fF1BankIndex         = -1;

  fBuffer = new UInt_t[fWordsPerBuffer];
 
}


QwF1TDC::~QwF1TDC()
{
  delete [] fBuffer;
  delete [] fReferenceSignals;
  //delete [] fF1TDCReferenceChannels;
  delete [] fF1TDC_SEU_counter;
  delete [] fF1TDC_EMM_counter;
  delete [] fF1TDC_SYN_counter;
  delete [] fF1TDC_TFO_counter;

  delete [] fF1TDC_RLF_counter;
  delete [] fF1TDC_HFO_counter;
  delete [] fF1TDC_OFO_counter;

  delete [] fF1TDC_FDF_counter;
  delete [] fF1TDC_S30_counter;

}

void
QwF1TDC::SetF1TDCBuffer(UInt_t *buffer, UInt_t num_words)
{
  UInt_t i = 0;
  Int_t j  = 0;
  UInt_t start_index = 0;

  for(i=0; i<num_words; i++ ) {
    if(buffer[i] == (UInt_t) fSlot) start_index = i;
  }


  // reassign buffer with 16 bits trim, because f1 configuration register is 16 bits
  Int_t F1_16Bits = 0xFFFF;
  Int_t F1_9Bits  = 0x01FF;
  
  Double_t buffer_range = (Double_t) F1_16Bits + 1.0;
  Double_t trig_range   = (Double_t) F1_9Bits  + 1.0;

  for(j=0; j<fWordsPerBuffer; j++) {
    // exclude the first buffer, because it contains "slot" number.
    fBuffer[j] = buffer[start_index+1+j] & F1_16Bits; 
  }


  // decide Normal or High Resolution Mode 
  if ( fBuffer[1] & 0x8000 ) {
    fF1TDCFactor = 0.5;
    fF1TDCNormResFlag = false;
  }
  else {
    fF1TDCFactor = 1.0;
    fF1TDCNormResFlag = true;
  }
  

  // check whether Synchronous mode or not
  if( fBuffer[15] & 0x4 ) {
    fF1TDCSyncFlag = true;
  } 
  else {
    fF1TDCSyncFlag = false;
  }
  // calculate resolution_ns or bin_size_ns and full_range_ns
  Double_t f1tdc_internal_reference_clock_ns = 25.0; // 40MHz
  // decide max channel number of F1TDC according to "resolution mode"

  fChannelNumber   = (Int_t) (fF1TDCFactor*fMaxF1TDCChannelNumber);

  // get refcnt and calculate tframe_ns
  fF1TDC_refcnt    = (fBuffer[7]>>6) & 0x1FF;
  fF1TDC_tframe_ns = (Double_t)(f1tdc_internal_reference_clock_ns * (fF1TDC_refcnt +2 ));


  // calculate refclkdiv
  UInt_t exponent = 0;
  exponent = ( fBuffer[10] >> 8 ) & 0x7;
  
  fF1TDC_refclkdiv = 1;
  for(UInt_t ii = 0; ii < exponent; ii++) fF1TDC_refclkdiv = 2 * fF1TDC_refclkdiv;

  // get hsdiv
  fF1TDC_hsdiv = fBuffer[10] & 0xFF;

 
  fF1TDC_resolution_ns = fF1TDCFactor * (f1tdc_internal_reference_clock_ns/152.0) * ( (Double_t) fF1TDC_refclkdiv )/( (Double_t) fF1TDC_hsdiv );
  fF1TDC_full_range_ns = buffer_range * fF1TDC_resolution_ns;

  // get trigwin and triglat
  fF1TDC_trigwin    = fBuffer[8] & F1_16Bits;
  fF1TDC_triglat    = fBuffer[9] & F1_16Bits; 	

  // calculate trigger window_ns and trigger latency_ns
  fF1TDC_window_ns  = ((Double_t)fF1TDC_trigwin) * fF1TDC_resolution_ns / fF1TDCFactor;
  fF1TDC_latency_ns = ((Double_t)fF1TDC_triglat) * fF1TDC_resolution_ns / fF1TDCFactor;

  // calculate trigger resolution_ns
  fF1TDC_trig_resolution_ns = fF1TDC_resolution_ns * 128.0; // buffer_range / trig_range = 65536 /512 = 128

  // calculate the rollover counter (t_offset) if synchronoues mode is used
  if(fF1TDCSyncFlag) {
    fF1TDC_t_offset      = fF1TDC_tframe_ns / fF1TDC_resolution_ns;
    fF1TDC_trig_t_offset = fF1TDC_tframe_ns / fF1TDC_trig_resolution_ns;
  }
  else {
    fF1TDC_t_offset      = buffer_range; // no offset (rollover)
    fF1TDC_trig_t_offset = trig_range; 
  }

  return;
}


Double_t 
QwF1TDC::ReferenceSignalCorrection(Double_t raw_time, Double_t ref_time)
{


  Double_t actual_time_difference = 0.0;
  

  // If we don't have the reference signal in the case that 
  // it is recorded as "0.0" or it isn't recorded at the same time when the raw_time is recoreded
  // return zero corrected time, because without a reference time, raw_time has no meaning
  // Friday, January 20 13:04:33 EST 2012 , jhlee

  if (ref_time !=0.0) {
    
    Double_t trigger_window        = (Double_t) fF1TDC_trigwin;
    Double_t time_offset           = (Double_t) fF1TDC_t_offset; // effective total bin numbers
    Double_t time_condition        = 0.0;
    Double_t local_time_difference = 0.0;
 
    local_time_difference = raw_time - ref_time; 

    if(local_time_difference != 0.0) {

      time_condition = fabs(local_time_difference); 
      // maximum value is trigger_window -1, 
      if(time_condition < trigger_window) {
	// there is no ROLLEVENT within trigger window
	actual_time_difference = local_time_difference;
      }
      else {
	// there is an ROLLOVER event within trigger window
	if (local_time_difference > 0.0) {
	  // ref_time is in after ROLLOVER event
	  actual_time_difference =  local_time_difference - time_offset;
	}
	else {
	  // we already excluded local_time_diffrence == 0 case.
	  // ref_time is in before ROLLOVER event
	  actual_time_difference = local_time_difference + time_offset;
	}
      }
    }
    else {
      // raw_time is the same as ref_time
      // std::cout << "QwF1TDC::ReferenceSignalCorrection " 
      // 	      << local_time_difference
      // 	      << " ref " << ref_time
      // 	      << " raw " << raw_time
      // 	      << std::endl;
      actual_time_difference = local_time_difference;
    }
  
  } 
  else {
    fF1TDC_RFM_counter++;
  }
  // Double_t trigger_window = (Double_t) fF1TDC_trigwin;
  // Double_t time_offset    = (Double_t) fF1TDC_t_offset;


  // Double_t time_condition = 0.0;
  // Double_t local_time_difference = 0.0;
  // Double_t actual_time_difference = 0.0;

  // local_time_difference = raw_time - ref_time; 
  
  // if(local_time_difference == 0.0) {
  //   // raw_time is the same as ref_time
  //   // std::cout << "QwF1TDC::ReferenceSignalCorrection " 
  //   // 	      << local_time_difference
  //   // 	      << " ref " << ref_time
  //   // 	      << " raw " << raw_time
  //   // 	      << std::endl;
  //   actual_time_difference = local_time_difference;
  // }
  // else {
  //   time_condition = fabs(local_time_difference); 
  //   // maximum value is trigger_window -1, 
  //   if(time_condition < trigger_window) {
  //     // there is no ROLLEVENT within trigger window
  //     actual_time_difference = local_time_difference;
  //   }
  //   else {
  //     // there is an ROLLOVER event within trigger window
  //     if (local_time_difference > 0.0) {
  // 	// ref_time is in after ROLLOVER event
  // 	actual_time_difference =  local_time_difference - time_offset;
  //     }
  //     else {
  // 	// we already excluded local_time_diffrence == 0 case.
  // 	// ref_time is in before ROLLOVER event
  // 	actual_time_difference = local_time_difference + time_offset;
  //     }
      
  //   }
  // }
  return actual_time_difference;
}



void
QwF1TDC::PrintF1TDCBuffer()
{
  Int_t i = 0;
  
  printf("System %s F1TDC ROC %2d, Slot %2d TDC id %2d Bank id %2d\n", 
	 fSystemName.Data(), fROC, fSlot, fF1TDCIndex, fF1BankIndex);
  for(i=0; i<fWordsPerBuffer; i++) {
    printf("0x%x ", fBuffer[i]);
  }
  printf("\n");

  return;
}


void
QwF1TDC::PrintF1TDCConfigure()
{

  printf("%s, ROC%2d slot%2d ", GetF1SystemName().Data(), GetROCNumber(), GetSlotNumber());
  if(IsNormResolution()) printf("Normal Resolution mode ");
  else                   printf("High Resolution mMode ");
  
  printf(" with the total channel number %d\n", fChannelNumber);

  if(IsSyncMode())       {
    printf("Synchronous mode ");
    printf(" with the rollover counter data %8.2f and trigger time %6.2f \n", fF1TDC_t_offset, fF1TDC_trig_t_offset);
  }
  else {
    printf("Non Synchronouse mode \n");
    printf(" with the full range %8.1f\n", fF1TDC_t_offset);
  }
  
  printf("refcnt    = %5d, tframe(ns) = %5.4f\n", fF1TDC_refcnt, fF1TDC_tframe_ns);
  printf("refclkdiv = %5d, hsdiv   = %6d, bin_size(ns) = %10.5f, full_range(ns) = %8.2f\n",
	 fF1TDC_refclkdiv, fF1TDC_hsdiv, fF1TDC_resolution_ns, fF1TDC_full_range_ns);
  printf("trigwin   = %5d, triglat = %6d, window  (ns) = %10.5f, latency   (ns) = %8.2f\n",
	 fF1TDC_trigwin, fF1TDC_triglat, fF1TDC_window_ns, fF1TDC_latency_ns);
  printf("trig_bin_size(ns) = %10.5f\n", fF1TDC_trig_resolution_ns);
  
  printf("\n");

  return;
}



void
QwF1TDC::ResetCounters()
{
  Int_t i = 0;

  fF1TDC_RFM_counter = 0;
  
  for(i=0; i<fMaxF1TDCChannelNumber; i++) 
    {
      fF1TDC_SEU_counter[i] = 0;
      fF1TDC_SYN_counter[i] = 0;
      fF1TDC_EMM_counter[i] = 0;
      
      fF1TDC_TFO_counter[i] = 0;
      
      fF1TDC_RLF_counter[i] = 0;
      fF1TDC_HFO_counter[i] = 0;
      fF1TDC_OFO_counter[i] = 0;
      fF1TDC_FDF_counter[i] = 0;
      fF1TDC_S30_counter[i] = 0;
    }
  return;
}


UInt_t
QwF1TDC::GetTotal(UInt_t *error_counter)
{
  UInt_t error = 0;
  Int_t i = 0;

  for(i=0; i<fMaxF1TDCChannelNumber; i++) 
    {
      error += error_counter[i];
    }
  return error;
}

UInt_t
QwF1TDC::GetTotalSEU()
{
  return GetTotal(fF1TDC_SEU_counter);
}

UInt_t
QwF1TDC::GetTotalSYN()
{
  return GetTotal(fF1TDC_SYN_counter);
}

UInt_t 
QwF1TDC::GetTotalEMM()
{
  return GetTotal(fF1TDC_EMM_counter);
}

UInt_t 
QwF1TDC::GetTotalTFO()
{
  return GetTotal(fF1TDC_TFO_counter);
}

UInt_t 
QwF1TDC::GetTotalRLF()
{
  return GetTotal(fF1TDC_RLF_counter);
}

UInt_t 
QwF1TDC::GetTotalHFO()
{
  return GetTotal(fF1TDC_HFO_counter);
}

UInt_t 
QwF1TDC::GetTotalOFO()
{
  return GetTotal(fF1TDC_OFO_counter);
}

UInt_t 
QwF1TDC::GetTotalFDF()
{
  return GetTotal(fF1TDC_FDF_counter);
}

UInt_t
QwF1TDC::GetTotalS30()
{
  return GetTotal(fF1TDC_S30_counter);
}


void
QwF1TDC::PrintChannelErrorCounter(Int_t channel)
{
  TString system_name = this->GetF1SystemName();
  if(system_name.Contains("R2") || system_name.Contains("R3")){
    QwMessage << "System " << std::setw(4) << system_name
            << " QwF1TDC obj at " << this
            << *this
            << " CH " <<  std::setw(2) << channel
	    << " OFO " << this->GetOFO(channel)
	    << " RLF " << this->GetRLF(channel)
	    << " TFO " << this->GetTFO(channel)
	    << " EMM " << this->GetEMM(channel)
	    << " SEU " << this->GetSEU(channel)
	    << " FDF " << this->GetFDF(channel)
            << " SYN " << this->GetSYN(channel)
            << " HFO " << this->GetHFO(channel)
    //              << " S30 " << this->GetS30(channel)
            << QwLog::endl;
  }
  return;
}



void
QwF1TDC::PrintErrorCounter()
{
  QwVerbose << "System " << this->GetF1SystemName()
            << " QwF1TDC object at " << this
            << *this
            << " " << QwLog::endl;

  Int_t i = 0;

  for(i=0; i<fMaxF1TDCChannelNumber; i++) 
    {
      PrintChannelErrorCounter(i);
    }
  return;
}



void
QwF1TDC::PrintTotalErrorCounter()
{
  QwMessage << "System " << this->GetF1SystemName()
            << " QwF1TDC object at " << this
            << *this
            << " "
	    << " OFO " << this->GetTotalOFO()
	    << " RLF " << this->GetTotalRLF()
	    << " TFO " << this->GetTotalTFO()
	    << " EMM " << this->GetTotalEMM()
	    << " SEU " << this->GetTotalSEU()
	    << " FDF " << this->GetTotalFDF()
	    << " SYN " << this->GetTotalSYN()
            << " RFM " << this->GetTotalRFM()
            << " HFO " << this->GetTotalHFO()
    //              << " S30 " << this->GetTotalS30()
            << QwLog::endl;
  return;
}



TString
QwF1TDC::GetChannelErrorCounter(Int_t channel)
{
  TString error_counter;
  error_counter =  Form("Error Counter F1TDC ROC%2d", fROC);
  error_counter += Form(" Slot%2d", fSlot);
  error_counter += Form(" Index%2d", fF1TDCIndex);
  error_counter += " OFO : ";
  error_counter += this->GetOFO(channel);
  error_counter += " RLF : ";
  error_counter += this->GetRLF(channel);
  error_counter += " TFO : ";
  error_counter += this->GetTFO(channel);
  error_counter += " EMM : ";
  error_counter += this->GetEMM(channel);
  error_counter += " SEU : ";
  error_counter += this->GetSEU(channel);
  error_counter += " FDC : " ;
  error_counter += this->GetFDF(channel);
  error_counter += " SYN : ";
  error_counter += this->GetSYN(channel);
  error_counter += " RFM : ";
  error_counter += this->GetTotalRFM();
  error_counter += " HFO : ";
  error_counter += this->GetHFO(channel);
  //  error_counter += " S30 : ";
  // error_counter += this->GetS30(channel);

  return error_counter;
}



TString
QwF1TDC::GetErrorCounter()
{
  TString error_counter;

  error_counter =  Form("Error Counter F1TDC ROC%2d", fROC);
  error_counter += Form(" Slot%2d", fSlot);
  error_counter += Form(" Index%2d", fF1TDCIndex);
  Int_t i = 0;

  for(i=0; i<fMaxF1TDCChannelNumber; i++) 
    {
      error_counter += GetChannelErrorCounter(i);
    }
  return error_counter;
}


TString
QwF1TDC::GetTotalErrorCounter()
{
  TString error_counter;

  error_counter =  Form("Error Counter F1TDC ROC%2d", fROC);
  error_counter += Form(" Slot%2d", fSlot);
  error_counter += Form(" Index%2d", fF1TDCIndex);
  error_counter += " OFO : ";
  error_counter += this->GetTotalOFO();
  error_counter += " RLF : ";
  error_counter += this->GetTotalRLF();
  error_counter += " TFO : ";
  error_counter += this->GetTotalTFO();
  error_counter += " EMM : ";
  error_counter += this->GetTotalEMM();
  error_counter += " SEU : ";
  error_counter += this->GetTotalSEU();
  error_counter += " FDC : " ;
  error_counter += this->GetTotalFDF();
  error_counter += " SYN : ";
  error_counter += this->GetTotalSYN();
  error_counter += " HFO : ";
  error_counter += this->GetTotalHFO();
  //  error_counter += " S30 : ";
  // error_counter += this->GetTotalS30();
  return error_counter;
}


void
QwF1TDC::PrintContact()
{
  printf("-------------------------------------------------------------------------------- \n");
  printf("%s : Roc%2d, slot%d is trouble now, please contact jhlee or rakithab immediately\n", 
	 GetF1SystemName().Data(), GetROCNumber(), GetSlotNumber());
  printf("-------------------------------------------------------------------------------- \n");
  return;
}

// void 
// SetRefernceSignals(Int_t chan, Double_t val)
// {
//   if( IsReFerenceSlot() ) 
//     {
//       fF1TDCReferenceChannels[chan] = val;
//     }
//   return;
// }


std::ostream& operator<< (std::ostream& os, const QwF1TDC &f1tdc)
{
  os << " ROC ";
  os << std::setw(2) << f1tdc.fROC;
  os << " Slot ";
  os << std::setw(2) << f1tdc.fSlot;
  os << " TDC idx ";
  os << std::setw(2) << f1tdc.fF1TDCIndex;
  os << " Bank idx ";
  os << std::setw(2) << f1tdc.fF1BankIndex;

  return os;
}




//----------------------------------
//
//
//
//  QwF1TDContainer
//
//
//
//
//----------------------------------






// QwF1TDContainer::QwF1TDContainer()
// {
//   fQwF1TDCList = NULL;
//   fError2DHist = NULL;
//   fNQwF1TDCs = 0;
//   fDetectorType = kTypeNull;
//   fRegion       = kRegionIDNull;


//   fLocalF1RawDecodeDebug = false; // Before checking "subsystem"
//   fLocalF1DecodeDebug    = false; //  After cheking "subsystem"
//   //level 0

//   fLocalDebug = false; // level 1 // not well defined...
//   fLocalDebug2 = false;// level 2// not well defined...
  
// }


QwF1TDContainer::QwF1TDContainer()
{
  fQwF1TDCList = new TObjArray();
 
  fQwF1TDCList -> Clear();
  fQwF1TDCList -> SetOwner(kTRUE);

  fError2DHist = new TH2F;
  fError2DHist -> SetBins(3,0,3,2,0,2);

  //FIXME: this is no longer supported in rt6: do we need this functionality?
  //fError2DHist -> SetBit(TH1::kCanRebin);
  fError2DHist -> SetStats(0);

  fNQwF1TDCs = 0;
  fDetectorType = kTypeNull;
  fRegion       = kRegionIDNull;


  fLocalF1RawDecodeDebug = false; // Before checking "subsystem"
  fLocalF1DecodeDebug    = false; //  After cheking "subsystem"
  //level 0

  fLocalDebug        = false; // level 1 // not well defined...
  fLocalF1ErrorDebug = false; // Error logs
  
}


QwF1TDContainer::~QwF1TDContainer()
{
  if(fQwF1TDCList) delete fQwF1TDCList; fQwF1TDCList = NULL;
  if(fError2DHist) delete fError2DHist; fError2DHist = NULL;
}



void 
QwF1TDContainer::AddQwF1TDC(QwF1TDC *in)
{
  Int_t pos = 0;

  pos = fQwF1TDCList -> AddAtFree(in);
  if(fLocalDebug) {
    printf("AddQwF1TDC at pos %d\n", pos);
  }

  fNQwF1TDCs++;
  return;
}


QwF1TDC *
QwF1TDContainer::GetF1TDC(Int_t roc, Int_t slot)
{
  Int_t roc_num  = 0;
  Int_t slot_num = 0;
  
  //  Int_t unique_id = 0;


  QwF1TDC* F1 = NULL;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;


  while ( (obj = next()) )
    {
      
      F1 = (QwF1TDC*) obj;
      //      unique_id = F1->GetUniqueID();
      roc_num  = F1->GetROCNumber();
      slot_num = F1->GetSlotNumber();
      if((roc_num == roc) && (slot_num == slot) ) {
	//	if(fLocalDebug) {
	//	  std::cout << "System " << F1->GetF1SystemName()
	//		    << " QwF1TDContainer::GetF1TDC F1TDC address at" << F1 << std::endl;
	//	}
	return F1;
      }
    }

  return NULL;
}


QwF1TDC *
QwF1TDContainer::GetF1TDCwithIndex(Int_t tdc_index)
{
  Int_t f1_idx = 0;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL; 
  QwF1TDC* F1  = NULL;

  while ( (obj = next()) )
    {
      F1     = (QwF1TDC*) obj;
      f1_idx = F1->GetF1TDCIndex();
      if( f1_idx == tdc_index ) { 
	// if(fLocalDebug) {
	//   std::cout << "System " << F1->GetF1SystemName()
	// 	    << " QwF1TDContainer::GetF1TDCIndex F1TDC address at" << F1 << std::endl;
	// }
	return F1;
      }
    }

  return NULL;
}

QwF1TDC *
QwF1TDContainer::GetF1TDCwithBankIndexSLOT(Int_t bank_index, Int_t slot)
{
  Int_t bank_idx = 0;
  Int_t slot_num = 0;
  
  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;
  QwF1TDC* F1  = NULL;

  while ( (obj = next()) )
    {
      F1       = (QwF1TDC*) obj;
      bank_idx = F1->GetF1BankIndex();
      slot_num = F1->GetSlotNumber();
      if((bank_idx == bank_index) && (slot_num == slot) ) {
	// if(fLocalDebug) {
	//   std::cout << "System " << F1->GetF1SystemName()
	// 	    << " QwF1TDContainer::GetF1TDC F1TDC address at" << F1 << std::endl;
	// }
	return F1;
      }
    }

  return NULL;
}


void
QwF1TDContainer::AddSYN(Int_t roc, Int_t slot, Int_t channel)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);
  
  if(F1) {
    F1->AddSYN(channel);
    if(fLocalF1ErrorDebug) F1->PrintChannelErrorCounter(channel);
  }
  else {
    if(fLocalF1ErrorDebug) std::cout << "QwF1TDContainer::AddSYN : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}

void
QwF1TDContainer::AddEMM(Int_t roc, Int_t slot, Int_t channel)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddEMM(channel);
    if(fLocalF1ErrorDebug) F1->PrintChannelErrorCounter(channel);
  }
  else {
    if(fLocalF1ErrorDebug) std::cout << "QwF1TDContainer::AddEMM : " << PrintNoF1TDC(roc,slot)  << std::endl;
  }
  return;
}


void
QwF1TDContainer::AddSEU(Int_t roc, Int_t slot, Int_t channel)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddSEU(channel);
    if(fLocalF1ErrorDebug) F1->PrintChannelErrorCounter(channel);
  }
  else {
    if(fLocalF1ErrorDebug) std::cout << "QwF1TDContainer::AddSEU : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}


void
QwF1TDContainer::AddTFO(Int_t roc, Int_t slot, Int_t channel)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddTFO(channel);
    if(fLocalF1ErrorDebug) F1->PrintChannelErrorCounter(channel);
  }
  else {
    if(fLocalF1ErrorDebug) std::cout << "QwF1TDContainer::AddTFO : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}


void
QwF1TDContainer::AddRLF(Int_t roc, Int_t slot, Int_t channel)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddRLF(channel);
    if(fLocalF1ErrorDebug) F1->PrintChannelErrorCounter(channel);
  }
  else {
    if(fLocalF1ErrorDebug) std::cout << "QwF1TDContainer::AddRLF : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}


void
QwF1TDContainer::AddHFO(Int_t roc, Int_t slot, Int_t channel)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddHFO(channel);
    if(fLocalF1ErrorDebug) F1->PrintChannelErrorCounter(channel);
  }
  else {
    if(fLocalF1ErrorDebug) std::cout << "QwF1TDContainer::AddHFO : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}

void
QwF1TDContainer::AddOFO(Int_t roc, Int_t slot, Int_t channel)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddOFO(channel);
    if(fLocalF1ErrorDebug) F1->PrintChannelErrorCounter(channel);
  }
  else {
    if(fLocalF1ErrorDebug) std::cout << "QwF1TDContainer::AddOFO : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}


void
QwF1TDContainer::AddFDF(Int_t roc, Int_t slot, Int_t channel)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddFDF(channel);
    if(fLocalF1ErrorDebug) F1->PrintChannelErrorCounter(channel);
  }
  else {
    if(fLocalF1ErrorDebug) std::cout << "QwF1TDContainer::AddFDF : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}



void
QwF1TDContainer::AddS30(Int_t roc, Int_t slot, Int_t channel)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);
  if(F1) {
    F1->AddS30(channel);
    if(fLocalF1ErrorDebug) F1->PrintChannelErrorCounter(channel);
  }
  else {
    if(fLocalF1ErrorDebug) std::cout << "QwF1TDContainer::AddS30 : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}



Double_t
QwF1TDContainer::ReferenceSignalCorrection(
					   Double_t raw_time, 
					   Double_t ref_time, 
					   Int_t bank_index, 
					   Int_t slot_num
					   )
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDCwithBankIndexSLOT(bank_index, slot_num);

  if(F1) {
    return F1->ReferenceSignalCorrection(raw_time, ref_time);
  }
  else {
    //   printf("There is no F1TDC with Bank index %2d slot %2d\n", bank_index, slot_num);
    return -1.0;
  }
}


void 
QwF1TDContainer::SetSystemName(const TString name)
{
  // Types are defined in QwType.h
  if(fSystemName.IsNull()) {
    fSystemName = name;
    if(fSystemName == "R1") {
      fDetectorType = kTypeSciFiber;
      fRegion       = kRegionID1;
    }
    else if(fSystemName == "R2") {
      fDetectorType = kTypeDriftHDC;
      fRegion       = kRegionID2;
    }
    else if(fSystemName == "R3") {
      fDetectorType = kTypeDriftVDC;
      fRegion       = kRegionID3;
    }

    else if(fSystemName == "MD" ) {
      fDetectorType = kTypeCerenkov;
      fRegion       = kRegionIDCer;
    }
    else if(fSystemName == "TS" ) {
      fDetectorType = kTypeTrigscint;
      fRegion       = kRegionIDTrig;
    }
    else if(fSystemName == "FPS") {
      fDetectorType = kTypeScanner;
      fRegion       = kRegionIDScanner;
    }
    else {
      fDetectorType = kTypeNull;
      fRegion       = kRegionIDNull;
    }
  }
  else {
    QwWarning << "QwF1TDContainer::SetSystemName "
              << fSystemName 
              << " is already registered."
              << QwLog::endl;
  }
  
  fError2DHist -> SetNameTitle(
			       Form("%s_F1ErrorHist",fSystemName.Data()),
			       Form("%s F1TDC Board Error Status Histogram", fSystemName.Data())
			       );
  return;
}

void
QwF1TDContainer::Print(const Option_t* options) const
{

  Int_t size = 0; 
  size = this->GetSize();

  QwMessage << "\nQwF1TDContainer::Print() "
            << " QwF1TDContainer in System : "  
            << this->GetSystemName()
            << ", DetectorType " 
	    << this->GetDetectorType()
	    << ", RegionType "   
            << this->GetRegion()
            << ", How many F1TDCs are : " 
            << size
            << QwLog::endl;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj;
  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      QwMessage << "F1TDC object " << F1 << QwLog::endl;
      F1 -> PrintF1TDCBuffer();
      F1 -> PrintF1TDCConfigure();
    }

  return;
}


TString
QwF1TDContainer::PrintNoF1TDC(Int_t roc, Int_t slot)
{
  TString tmp = Form("There is no F1TDC with ROC%2d and SLOT%2d in System %s\n", 
		     roc, slot, GetSystemName().Data());
  return tmp;

}

TString
QwF1TDContainer::PrintNoF1TDC(Int_t tdc_index)
{
  TString tmp = Form("There is no F1TDC with TDCINDEX%2d in System %s\n", 
		     tdc_index, GetSystemName().Data());

  return tmp;
}

// // This function will be removed after several tests..
// //
// Double_t
// QwF1TDContainer::GetF1TDCResolution()
// {

//   // F1TDC resolution must be the same
//   // among VME crates and among F1TDC boards
//   // We cannot change it on each F1TDC board.QwF1TDContainer::GetF1TDCResolution()
//   // Thus, this function return one value of them.
//   // Wednesday, September  1 16:52:05 EDT 2010, jhlee

//   Double_t old_r = 0.0;
//   Double_t new_r = 0.0;
//   Int_t cnt      = 0;

//   TObjArrayIter next(fQwF1TDCList);
//   TObject* obj = NULL;

//   while ( (obj = next()) )
//     {
//       QwF1TDC* F1 = (QwF1TDC*) obj;
//       new_r = F1->GetF1TDC_resolution();
//       if(cnt not_eq 0) {
// 	if(old_r not_eq new_r) {
// 	  F1->PrintContact();
// 	  printf("%s : QwF1TDContainer::GetF1TDCResolution(): F1TDC configurations are corrupted!\n", 
// 		 GetSystemName().Data());
// 	  F1->PrintF1TDCConfigure();

// 	  return 0.0;
// 	}

//       }
//       old_r = new_r;
//       cnt++;
//     }

//   return old_r;
// }


Double_t
QwF1TDContainer::GetF1TDCsResolution()
{

  // F1TDC resolution must be the same
  // among VME crates and among F1TDC boards
  // We cannot change it on each F1TDC board.QwF1TDContainer::GetF1TDCResolution()
  // Thus, this function return one value of them.
  // Wednesday, September  1 16:52:05 EDT 2010, jhlee

  Double_t old_r = 0.0;
  Double_t new_r = 0.0;
  Int_t cnt      = 0;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;

  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      new_r = F1->GetF1TDC_resolution();
      if(cnt not_eq 0) {
	if(old_r not_eq new_r) {
	  F1->PrintContact();
	  printf("%s : QwF1TDContainer::GetF1TDCResolution(): F1TDC configurations are corrupted!\n", 
		 GetSystemName().Data());
	  F1->PrintF1TDCConfigure();

	  return 0.0;
	}

      }
      old_r = new_r;
      cnt++;
    }

  return old_r;
}



Double_t
QwF1TDContainer::GetF1TDCTriggerRollover()
{

  Double_t old_r = 0.0;
  Double_t new_r = 0.0;
  Int_t cnt      = 0;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;

  if(fLocalDebug) printf("\nHow many F1TDC in %s : %d\n", GetSystemName().Data(), fQwF1TDCList->GetEntriesFast());

  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      new_r = F1->GetF1TDC_trig_t_offset();
      //      printf("QwF1TDContainer::GetF1TDCTriggerRollover():: cnt %d, new %f , old %f\n", cnt, new_r, old_r);
      if(cnt not_eq 0) {
       	if(old_r not_eq new_r) {
	  F1->PrintContact();
	  printf("%s : QwF1TDContainer::GetF1TDCTriggerRollover(): F1TDC configurations are corrupted!\n", 
		 GetSystemName().Data());
	  F1->PrintF1TDCConfigure();
	  return 0.0;
      	}

      }
      old_r = new_r;
      cnt++;
    }

  return old_r;
}




Int_t
QwF1TDContainer::GetF1TDCChannelNumber()
{

  // F1TDC max channel number must be the same
  // among VME crates and among F1TDC boards
  // We cannot change it on each F1TDC board.
  // Thus, this function return one value of them. (32 or 64)
  // Friday, September  3 13:09:01 EDT 2010, jhlee

  Int_t old_c = 0;
  Int_t new_c = 0;
  Int_t cnt   = 0;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;

  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      new_c = F1->GetChannelNumber();
      //  printf("QwF1TDContainer::GetF1TDCChannelNumber() cnt %d, new %d , old %d\n", cnt, new_c, old_c);
      if(cnt not_eq 0) {
	if(old_c not_eq new_c) {
	  F1->PrintContact();
	  printf("%s : QwF1TDContainer::GetF1TDCChannelNumber(): F1TDC configurations are corrupted!\n", 
		 GetSystemName().Data());
	  F1->PrintF1TDCConfigure();
	  return 0;
	}

      }
      old_c = new_c;
      cnt++;
    }
  
  return old_c;
}



void
QwF1TDContainer::PrintErrorSummary()
{
  printf("-----------------------\n");
  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;
  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      
      F1 -> PrintTotalErrorCounter();
    }
  printf("-----------------------\n");
  return;
}


TList *
QwF1TDContainer::GetErrorSummary()
{

  TList *error_list = new TList;
  error_list->SetOwner(true);
  
  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;

  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      TString error = F1->GetErrorCounter();
      error_list -> Add(new TObjString(error));
    }

  return error_list;
}


// Check Trigger Time Mismatch
//       Event Number Mismatch
//       Trigger FIFO overflow
// if    one of them is "false", return false
//
// Count the following 
//   OFO : Output FIFO Overflow
//   RLF : Resolution Lock Failed
//   TFO : Trigger FIFO Overflow
//   EMM : Event(Trigger) Number Mismatch
//   SEU : Single Event Upset
//   FDF : Fake Data Flag
//   SYN : Trigger Time mismatch
//   HFO : Hit FIFO Overflow
//   S30 : Slot 30 dataword when the F1 chip has no response within "a reasonable time"
    
Bool_t 
QwF1TDContainer::CheckDataIntegrity(const ROCID_t roc_id, UInt_t *buffer, UInt_t num_words)
{

 
  
  // three counter flags
  Bool_t hit_fifo_overflow_flag    = kFALSE;
  Bool_t output_fifo_overflow_flag = kFALSE;
  Bool_t chip_resolution_lock_flag = kTRUE;

  // real DataIntegrity Check
  Bool_t event_ok_flag              = kFALSE;
  Bool_t trig_time_ok_flag          = kFALSE;
  Bool_t trig_fifo_ok_flag          = kFALSE;

  // data integrity flags
  Bool_t data_integrity_flag        = kFALSE;
  Bool_t data_integrity_unlock_flag = kTRUE; // unlock first


  // SEU
  Bool_t xor_setup_flag     = kFALSE;
  Bool_t fake_data_flag     = kFALSE;

  
  UInt_t  slot_number         = 0;
  UInt_t  channel_number      = 0;
  UInt_t  chip_address        = 0;
  UInt_t  channel_address     = 0;

  UInt_t reference_trig_time = 0;
  UInt_t reference_event_num = 0;


  Double_t trigger_rollover = 0.0;
  Int_t    rounded_trigger_rollover = 0;
  Int_t    diff_trigger_time = 0;

  trigger_rollover         = GetF1TDCTriggerRollover();
  rounded_trigger_rollover = (Int_t) trigger_rollover;

  //  fLocalDebug = true;
  if(fLocalDebug) printf("roc_id %d trigger rollover %f, %d\n", roc_id, trigger_rollover, rounded_trigger_rollover);

  
  const Int_t valid_trigger_time_offset[3] = {0, 1, rounded_trigger_rollover};
  

  if(fLocalF1DecodeDebug) {
    QwMessage << "\n\n" << QwLog::endl;
    QwMessage << GetSystemName()
              << " ROC " << roc_id
              << " num_words (buffer size) " << num_words
              << QwLog::endl;
  }

  Int_t subsystem_cnt = 0;
  TString roc_idx;

  if(fLocalF1RawDecodeDebug) {
    printf("%s at %s nwrds %d\n", 
	   __PRETTY_FUNCTION__, GetSystemName().Data(), num_words);
  }

  for (UInt_t i=0; i<num_words ; i++) 
    {
      fF1TDCDecoder.DecodeTDCWord(buffer[i], roc_id); 
      // without any further process
      // check the decoding....
      
      if(fLocalF1RawDecodeDebug) {
	std::cout << "[" << std::setw(3) << i
		  << "," << std::setw(3) << i
		  << "] "
		  << fF1TDCDecoder << std::endl;
      }    


      slot_number     = fF1TDCDecoder.GetTDCSlotNumber();

      if(fF1TDCDecoder.IsValidDataSlot()) {
	// Check F1TDC slot, provided by buffer[i], is valid (1<=slot<=21)
	//, because sometimes, slot 30 data (junk) is placed in the first one
	// or two buffers.

	chip_address    = fF1TDCDecoder.GetTDCChipAddress();
	channel_address = fF1TDCDecoder.GetTDCChannelAddress();
	channel_number  = fF1TDCDecoder.GetTDCChannelNumber();


	// if(fLocalDebug) {
	//   // this is for tracking down missing wires in Region 2
	//   // a quick and dirty way to check
	//   // Monday, December 20 10:38:38 EST 2010, jhlee
	//      it turned out "loosing cabling....
	//      Saturday, January  8 01:54:02 EST 2011, jhlee
	  
	//   if( (slot_number==10) and (roc_id==3) ) {
	//     if( (channel_number == 19) ||  (channel_number == 21) || (channel_number == 23) || (channel_number == 25) 
	// 	|| (channel_number == 27) ||  (channel_number == 29) || (channel_number == 31) || (channel_number == 17) 
	// 	) 
	//       {
	// 	printf("Roc %2d Slot%2d Ch %2d \n", roc_id, slot_number, channel_number);
	//       }
	//   }
	// }
	
	roc_idx = Form("R%2d-S%2d", roc_id, slot_number);

	// We use the multiblock data transfer for F1TDC, thus
	// we must get the event number and the trigger time from the first buffer
	// (buffer[0]), and these valuse can be used to check "data" integrity
	// over all F1TDCs.
	// Each Subsystem uses the same buffer[0] even if their interesting F1
	// is not in the SLOT 4

	if ( i ==0 ) {
	  if ( fF1TDCDecoder.IsHeaderword() ) {
	    reference_event_num = fF1TDCDecoder.GetTDCEventNumber();
	    reference_trig_time = fF1TDCDecoder.GetTDCTriggerTime();
	    
	    trig_fifo_ok_flag = fF1TDCDecoder.IsNotHeaderTrigFIFO();
	    if(not trig_fifo_ok_flag) {
	      // temp solution ....
	      if(not CheckSlot20Chan30(slot_number, channel_number)) {
		this -> AddTFO(roc_id, slot_number, channel_number);
		  fError2DHist -> Fill(roc_idx.Data(), "TFO",1);
		}
	    }
	    
	  }
	  else {
	    printf("The first word of F1TDC must be header word. Check CODA stream and QwF1TDContainer::CheckDataIntegrity fucntion.\n");
	    return false;
	  }
	}

	// Check F1TDC status is inside a subsystem
    
	if( CheckRegisteredF1(roc_id, slot_number) ) { 
      
	  // Check F1TDC data, provided by buffer[i], is valid in each subsystem
	  //	  subsystem_cnt = 0;

	  if(fLocalF1DecodeDebug) {
	    
	    std::cout << "[" << std::setw(3) << i
		      << "," << std::setw(3) << subsystem_cnt 
		      << "] "
		      << fF1TDCDecoder << std::endl;
	  }               
	  subsystem_cnt++;
	  // Both header and data words have HFO, OFO, and RLF
	  hit_fifo_overflow_flag    = fF1TDCDecoder.IsHitFIFOOverFlow();
	  output_fifo_overflow_flag = fF1TDCDecoder.IsOutputFIFOOverFlow();
	  chip_resolution_lock_flag = fF1TDCDecoder.IsResolutionLock();
	  
	
	  if(hit_fifo_overflow_flag) {
	    if(not CheckSlot20Chan30(slot_number, channel_number)) {
		this->AddHFO(roc_id, slot_number, channel_number);
		// one event creates the maximum 9 HFOs
		// but the number is within the range of 1 - 9
		
		fError2DHist -> Fill(roc_idx.Data(), "HFO",1);
		
		if(fLocalDebug) {
		  std::cout << "There is the Hit FIFO Overflow on the F1TDC board at"
			    << " ROC "  << roc_id
			    << " Slot " << slot_number
			    << " Ch " << std::setw(3) << channel_number
			    << "[" << chip_address
			    << "," << channel_address
			    << "]\n";
		}
	      }
	  }
	  
	  if(output_fifo_overflow_flag)  {
	    if(not CheckSlot20Chan30(slot_number, channel_number)) {
	      this->AddOFO(roc_id, slot_number, channel_number);
	      fError2DHist -> Fill(roc_idx.Data(), "OFO",1);
	      if(fLocalDebug) {
		std::cout << "There is the Output FIFO Overflow on the F1TDC board at"
			  << " ROC "  << roc_id
			  << " Slot " << slot_number
			  << " Ch " << std::setw(3) << channel_number
			  << "[" << chip_address
			  << "," << channel_address
			  << "]\n";
		
	      }
	    }
	  }
	  
	  if(not chip_resolution_lock_flag) {
	    if(not CheckSlot20Chan30(slot_number, channel_number)) {
	      this->AddRLF(roc_id, slot_number, channel_number);
	      fError2DHist -> Fill(roc_idx.Data(), "RLF",1);
	      if(fLocalDebug) {
		std::cout << "There is the Resolution Lock Failed on the F1TDC board at"
			  << " ROC "  << roc_id
			  << " Slot " << slot_number
			  << " Ch " << std::setw(3) << channel_number
			  << "[" << chip_address
			  << "," << channel_address
			  << "]\n";
	      }
	    }
	  }
	  
	  // check only header word

	  if ( fF1TDCDecoder.IsHeaderword() ) {
	    
	    
	    xor_setup_flag    = fF1TDCDecoder.IsHeaderXorSetup();
	    trig_fifo_ok_flag = fF1TDCDecoder.IsNotHeaderTrigFIFO();
	    event_ok_flag     = ( reference_event_num==fF1TDCDecoder.GetTDCHeaderEventNumber() );
	    diff_trigger_time = abs( Long_t(reference_trig_time) - Long_t(fF1TDCDecoder.GetTDCHeaderTriggerTime()) );

	    trig_time_ok_flag = 
	      (diff_trigger_time == valid_trigger_time_offset[0])  
	      || 
	      (diff_trigger_time == valid_trigger_time_offset[1]) 
	      ||
	      (diff_trigger_time == valid_trigger_time_offset[2]);
	    
	    // trig_time_ok_flag = abs( reference_trig_time-fF1TDCDecoder.GetTDCHeaderTriggerTime() ) <= valid_trigger_time_offset;
	 
	    // if no SEU, check Trigger FIFO Overflow, Trigger Time, and Event Number

	    if (xor_setup_flag) {

	      if(not trig_fifo_ok_flag) {
		if(not CheckSlot20Chan30(slot_number, channel_number)) {
		  this -> AddTFO(roc_id, slot_number, channel_number);
		  fError2DHist -> Fill(roc_idx.Data(), "TFO",1);
		  if(fLocalDebug) {
		    std::cout << "There is the Trigger FIFO overflow at"
			      << " ROC "  << roc_id
			      << " Slot " << slot_number
			      << " Ch " << std::setw(3) << channel_number
			      << "[" << chip_address
			      << "," << channel_address
			      << "]\n";
		    
		  }
		}
	      }
	      
	      // Trigger Time difference of up to 1 count among the chips is acceptable
	      // For the Trigger Time, this assumes that an external SYNC_RESET signal has
	      // been successfully applied at the start of the run

	      if (not trig_time_ok_flag) {
		if(not CheckSlot20Chan30(slot_number, channel_number)) {
		  this->AddSYN(roc_id, slot_number, channel_number);
		  fError2DHist -> Fill(roc_idx.Data(), "SYN",1);
		  if(fLocalDebug) {
		    std::cout << "There is the Trigger Time Mismatch on the F1TDC board at"
			      << " ROC "  << roc_id
			      << " Slot " << slot_number
			      << " Ch " << std::setw(3) << channel_number
			      << "[" << chip_address
			      << "," << channel_address
			      << "]\n";
		  }
		}
		
	      }
	      // Any difference in the Event Number among the chips indicates a serious error
	      // that requires a reset of the board.
	    
	      if (not event_ok_flag) {
		if(not CheckSlot20Chan30(slot_number, channel_number)) {
		  this->AddEMM(roc_id, slot_number, channel_number);
		  fError2DHist -> Fill(roc_idx.Data(), "EMM",1);
		  if(fLocalDebug) {
		    std::cout << "There is the Event Number Mismatch issue on the F1TDC board at"
			      << " ROC "  << roc_id
			      << " Slot " << slot_number
			      << " Ch " << std::setw(3) << channel_number
			      << "[" << chip_address
			      << "," << channel_address
			      << "]\n";
		  }
		}
	      }
	      
	      
	      if(data_integrity_unlock_flag) { 
		// check data_integrity_flag = false once (not initial value),
		// if so, we skip the further process to check
		// data integrity, because it is alreay invalid
		// data we have.

		data_integrity_flag = (event_ok_flag) && (trig_time_ok_flag) && (trig_fifo_ok_flag) ;
		if(data_integrity_flag) data_integrity_unlock_flag = true;    // true  -> not lock -> check it again
		else                    data_integrity_unlock_flag = false;   // false -> lock  -> no check it again -> return false;
	      }
	    } // if(xor_setup_flag) {
	 
	 
	    // ignore SEU first, then we count them how frequently they are,
	    // because there is no way to seperate SEU and the data integrity.
	    // data integrity is valid within all buffer, but SEU is valid 
	    // for only when it is (header and dataword)

	    else {
	      if(not CheckSlot20Chan30(slot_number, channel_number)) {
		this->AddSEU(roc_id, slot_number, channel_number);
		fError2DHist -> Fill(roc_idx.Data(), "SEU",1);
		if (fLocalDebug) {
		  std::cout << "There is the Single Event Upset (SEU) on the F1TDC board at"
			    << " ROC "  << roc_id
			    << " Slot " << slot_number
			    << " Ch " << std::setw(3) << channel_number
			    << "[" << chip_address
			    << "," << channel_address
			    << "]\n";
		}
	      }
	    }
	  } //   if ( fF1TDCDecoder.IsHeaderword() ) {
	  else {
	    //	    fF1TDCDecoder.Print(true);
	    fake_data_flag = fF1TDCDecoder.IsFakeData();
	    if(fake_data_flag) {
	      if(not CheckSlot20Chan30(slot_number, channel_number)) {
		this->AddFDF(roc_id, slot_number, channel_number);
		fError2DHist -> Fill(roc_idx.Data(), "FDF",1);
		if(fLocalDebug) {
		  std::cout << "There is the Fake Data on the F1TDC board at"
			    << " ROC "  << roc_id
			    << " Slot " << slot_number
			    << " Ch " << std::setw(3) << channel_number
			    << "[" << chip_address
			    << "," << channel_address
			    << "]\n";
		}
	      }
	    }

	    // dataword
	
	    //	    if(!fF1TDCDecoder.IsOverFlowEntry()) fF1TDCDecoder.PrintTDCData(fLocalDebug);
	  }//;;
	  
	} // if( CheckRegisteredF1(roc_id, slot_number) ) {




      }//if(fF1TDCDecoder.IsValidDataSlot()) {
      else {

	//	slot_number = fF1TDCDecoder.GetTDCSlotNumber();
	if( slot_number == 0 ) {
	  if(fLocalDebug)  {
	    std::cout << "Slot " << slot_number << " is a filler word," 
		      << " then we ignore it." << std::endl;
	  }
	}
	else if( slot_number == 30 ) {
	  // Slot 30 is not in the list of F1TDCs in a subsystem.
	  // we cannot add this counter into QwF1TDC.
	  // Now just leave what I wrote for a possible future release.
	  // Tuesday, September 21 15:19:34 EDT 2010, jhlee
	  // this->AddS30(roc_id, slot_number); 
	  if(fLocalDebug)  {
	    std::cout << "Slot " << slot_number << " is a junk  word," 
		      << " then we ignore it." << std::endl;
	  }
	}
	else {
	  std::cout << "Slot " << slot_number << " is not in the reasonable slot," 
		    << " then we ignore it, but it is better to check what it is going on CODA stream." << std::endl;
	}
      }
   


    }//for (UInt_t i=0; i<num_words ; i++) {
  
  if(fLocalF1RawDecodeDebug) printf("\n");
  return (data_integrity_flag); 
}

void
QwF1TDContainer::SetErrorHistOptions()
{
  const char* opt = "TEXT";
  fError2DHist->SetMarkerSize(1.4);
  fError2DHist->SetOption(opt);
  fError2DHist->LabelsDeflate("X");
  fError2DHist->LabelsDeflate("Y");
  fError2DHist->LabelsOption("a", "X");
  fError2DHist->LabelsOption("a", "Y");
  return;
}


// send the historgram to a subsystem, then
// the subsystem can send it to ROOT file(s)
const TH2F*
QwF1TDContainer::GetF1TDCErrorHist()
{
  this->SetErrorHistOptions();
  return fError2DHist;
}



// direct write histogram into ROOT file(s)
void 
QwF1TDContainer::WriteErrorSummary(Bool_t hist_flag)
{
  if (fError2DHist->GetEntries() not_eq 0) {

    TSeqCollection *file_list = gROOT->GetListOfFiles();
  
    if (file_list) {

      Int_t size = file_list->GetSize();
    
      TString error_summary_name = this-> GetSystemName();
      error_summary_name += "_F1TDCs_Status";

      for (Int_t i=0; i<size; i++) 
	{
	  TFile *file = (TFile*) file_list->At(i);
	  if(hist_flag) {
	    TString hist_name = fError2DHist->GetName();
	    TH2F *error_hist = (TH2F*) file->FindObjectAny(hist_name);
	    if (not error_hist)  {
	      this->SetErrorHistOptions();
	      file->WriteObject(fError2DHist, hist_name);
	    }
	  }
	  else {
	    TList *error_summary = (TList*) file->FindObjectAny(error_summary_name);
	    if (not error_summary) {
	      file->WriteObject(this->GetErrorSummary(), error_summary_name);
	    }
	  }
	  if(fLocalDebug) {
	    std::cout << "i " << i
		      << " size " << size
		      << " error_summary_name " << error_summary_name
		      << std::endl;
	  }
	}
    }
  }
  return;
}



// void 
// QwF1TDContainer::WriteErrorSummaryToDedicatedRootFile(QwRootFile* rootfile, Bool_t hist_flag)
// {
//   if (fError2DHist->GetEntries() not_eq 0) {

//     TString error_summary_name = this-> GetSystemName();
//     error_summary_name += "_F1TDCs_Status";

//     if(hist_flag) {
//       TString hist_name = fError2DHist->GetName();
//       this->SetErrorHistOptions();
//       rootfile->WriteObject(fError2DHist, hist_name);
//     }
//     else {
//       rootfile->WriteObject(this->GetErrorSummary(), error_summary_name);
//     }
//   }
//   return;
// }


Bool_t 
QwF1TDContainer::CheckRegisteredF1(Int_t roc, Int_t slot)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);
  
  if(F1) return true;
  else   return false;
}


Bool_t 
QwF1TDContainer::CheckSlot20Chan30(Int_t slot, Int_t chan)
{
  return ( (slot==20) and (chan==30) );
}

//
// return F1TDC resolution for possible further check
// 
Double_t 
QwF1TDContainer::DoneF1TDCsConfiguration()
{
  //
  // reduce call "GetF1TDCsResolution" inside subsystem...
  //
  fF1TDCOneResolutionNS = this->GetF1TDCsResolution();
  return fF1TDCOneResolutionNS;
}




//----------------------------------
//
//
//
//  F1TDCReferenceSignal
//
//
//
//
//----------------------------------



const UInt_t F1TDCReferenceSignal::fNoRefTimeArbUnit = 0xFFFFF; // whatever number which is out of the F1TDC hardware range FFFF.


F1TDCReferenceSignal::F1TDCReferenceSignal()
{
  fSlot          = -1;
  fChannelNumber = -1;
  fBankIndex     = -1;
  fRefSignalName = "";
  fSystemName    = "";
  Clear();
}



F1TDCReferenceSignal::F1TDCReferenceSignal(
					   const Int_t bank_index, 
					   const Int_t slot, 
					   const Int_t channel
					   )
{
  fSlot          = slot;
  fChannelNumber = channel;
  fBankIndex     = bank_index;
  fRefSignalName = "";
  fSystemName    = "";
  Clear();

}



F1TDCReferenceSignal::F1TDCReferenceSignal(
					   const Int_t bank_index, 
					   const Int_t slot, 
					   const Int_t channel,
					   const TString name
					   )
{
  fSlot          = slot;
  fChannelNumber = channel;
  fBankIndex     = bank_index;
  fRefSignalName = name;
  fSystemName    = "";
  fCounter       = 0;
  Clear();
} 






std::ostream& operator<< (std::ostream& os,  const F1TDCReferenceSignal &f1tdcref)
{
  os << " Name ";
  os << std::setw(22) << f1tdcref.fRefSignalName;
  os << " Bank idx ";
  os << std::setw(2) << f1tdcref.fBankIndex;
  os << " Slot ";
  os << std::setw(2) << f1tdcref.fSlot;
  os << " Chan ";
  os << std::setw(2) << f1tdcref.fChannelNumber;
  os << " RefTime (a.u.) ";
  os << std::setw(8) << f1tdcref.fRefTimeArbUnit;
  os << " Counter ";
  os << std::setw(12) << f1tdcref.fCounter;

  return os;
}


Bool_t 
F1TDCReferenceSignal::SetRefTimeAU(const Double_t ref_time) 
{
  // always save the first hit as the reference signal
  Bool_t status = false;
  if( not HasFirstHit() ) { 
    // save the reference fime
    fRefTimeArbUnit = ref_time; 
    //   fRefTimeFlag = true;
    fFirstHitFlag = true;
    status = true;
    fCounter++;
  }
  return status;
};



Bool_t 
F1TDCReferenceSignal::HasRefTime() 
{
  if( (Double_t) fNoRefTimeArbUnit == fRefTimeArbUnit) {
    fRefTimeFlag = false;
  }
  else {
    fRefTimeFlag = true;
  }
  return fRefTimeFlag;
};


void 
F1TDCReferenceSignal::PrintCounterSummary()
{
  std::cout << " Name " << std::setw(20) << fRefSignalName
	    << " Bank idx " << std::setw(2) <<  fBankIndex
	    << " Slot "     << std::setw(2) <<  fSlot
            << " Chan "     << std::setw(2) <<  fChannelNumber
	    << " Counter "  << std::setw(20) << fCounter
	    << std::endl;
  return;
};



//----------------------------------
//
//
//
//  F1TDCReferenceContainer
//
//
//
//
//----------------------------------


F1TDCReferenceContainer::F1TDCReferenceContainer()
{
  fF1TDCReferenceSignalsList = new TObjArray();
 
  fF1TDCReferenceSignalsList -> Clear();
  fF1TDCReferenceSignalsList -> SetOwner(kTRUE);


  fNF1TDCReferenceSignals = 0;

}


F1TDCReferenceContainer::~F1TDCReferenceContainer()
{
  if(fF1TDCReferenceSignalsList) {
    delete fF1TDCReferenceSignalsList; 
    fF1TDCReferenceSignalsList = NULL;
  }
}



void 
F1TDCReferenceContainer::AddF1TDCReferenceSignal(F1TDCReferenceSignal *in)
{
  /* Int_t pos = */ fF1TDCReferenceSignalsList -> AddAtFree(in);
  //  printf("AddF1TDCReferenceSignal at pos %d\n", pos);
  std::cout << *in << std::endl;
  fNF1TDCReferenceSignals++;
  return;
}



// F1TDCReferenceSignal *
// F1TDCReferenceContainer::GetReferenceSignal(Int_t bank_index, 
// 					    Int_t slot, 
// 					    Int_t chan
// 					    )
// {
//   Int_t bank_idx = 0;
//   Int_t slot_num = 0;
//   Int_t chan_num = 0;
//   // Int_t time_au  = 0;
  
//   TObjArrayIter next(fF1TDCReferenceSignalsList);
//   TObject* obj = NULL;
//   F1TDCReferenceSignal* F1RefSignal  = NULL;

//   while ( (obj = next()) )
//     {
//       F1RefSignal = (F1TDCReferenceSignal*) obj;

//       bank_idx    = F1RefSignal->GetBankIndex();
//       slot_num    = F1RefSignal->GetSlotNumber();
//       chan_num    = F1RefSignal->GetChannelNumber();
//       //     time_au     = F1RefSignal->
//       if( (bank_idx == bank_index) and (slot_num == slot) and (chan_num == chan) ) {
// 	if ( F1RefSignal->HasFirstHit() ) return NULL;
// 	else                              return F1RefSignal;
//       }
//     }

//   return NULL;
// }



void
F1TDCReferenceContainer::SetReferenceSignal(Int_t bank_index, 
					    Int_t slot, 
					    Int_t chan,
					    UInt_t data,
					    Bool_t debug
					    )
{
  Int_t bank_idx = 0;
  Int_t slot_num = 0;
  Int_t chan_num = 0;
  // Int_t time_au  = 0;
  
  TObjArrayIter next(fF1TDCReferenceSignalsList);
  TObject* obj = NULL;
  F1TDCReferenceSignal* F1RefSignal  = NULL;

  while ( (obj = next()) )
    {
      F1RefSignal = (F1TDCReferenceSignal*) obj;

      bank_idx    = F1RefSignal->GetBankIndex();
      slot_num    = F1RefSignal->GetSlotNumber();
      chan_num    = F1RefSignal->GetChannelNumber();

      if( (bank_idx == bank_index) and (slot_num == slot) and (chan_num == chan) ) {
	if ( not F1RefSignal->HasFirstHit() ) {
	  F1RefSignal -> SetRefTimeAU (data);
	  if(debug) std::cout << *F1RefSignal << std::endl;
	}
      }
    }

  return;
}


Double_t
F1TDCReferenceContainer::GetReferenceTimeAU(
					    Int_t bank_index, 
					    TString name 
					    )
{
  Int_t   bank_idx = 0;
  TString ref_name = "";
  
  TObjArrayIter next(fF1TDCReferenceSignalsList);
  TObject* obj = NULL;
  F1TDCReferenceSignal* F1RefSignal  = NULL;

  while ( (obj = next()) )
    {
      F1RefSignal = (F1TDCReferenceSignal*) obj;

      bank_idx = F1RefSignal->GetBankIndex();
      ref_name = F1RefSignal->GetRefSignalName(); 

      if( (bank_idx == bank_index) and (ref_name == name) ) {
	// find the reference signals,
	if(F1RefSignal->HasRefTime()) {
	  // return a ref time if the reference time is filled up
	  // can be zero. 
	  return F1RefSignal->GetRefTimeAU();
	}
	//	return F1RefSignal->GetRefTimeAU();
      }
    }

  return 0.0; // they cannot find a reference signal for bank_index and name.
}



void
F1TDCReferenceContainer::ClearEventData()
{
  
  TObjArrayIter next(fF1TDCReferenceSignalsList);
  TObject* obj = NULL;
  F1TDCReferenceSignal* F1RefSignal  = NULL;

  while ( (obj = next()) )
    {
      F1RefSignal = (F1TDCReferenceSignal*) obj;
      F1RefSignal->ClearEventData();
    }

  return;
}

void
F1TDCReferenceContainer::PrintCounters()
{
  TObjArrayIter next(fF1TDCReferenceSignalsList);
  TObject* obj = NULL;
  F1TDCReferenceSignal* F1RefSignal  = NULL;


  std::cout << "F1Reference Signal Counters at System " << fSystemName << std::endl;
  while ( (obj = next()) )
    {
      F1RefSignal = (F1TDCReferenceSignal*) obj;
      F1RefSignal->PrintCounterSummary();
    }
  return;
};


void
F1TDCReferenceContainer::SetSystemName(const TString name)
{
  // Types are defined in QwType.h
  if(fSystemName.IsNull()) {
    fSystemName = name;
  }
  else {
    std::cout << "F1TDCReferenceContainer::SetSystemName " 
	      << fSystemName 
	      << " is already registered."
	      << std::endl;
  }
  return;
}



// Double_t
// F1TDCReferenceContainer::GetNoReferenceTimeAU(
// 					      Int_t bank_index, 
// 					      TString name 
// 					      )
// {
//   Int_t   bank_idx = 0;
//   TString ref_name = "";
  
//   TObjArrayIter next(fF1TDCReferenceSignalsList);
//   TObject* obj = NULL;
//   F1TDCReferenceSignal* F1RefSignal  = NULL;

//   while ( (obj = next()) )
//     {
//       F1RefSignal = (F1TDCReferenceSignal*) obj;

//       bank_idx = F1RefSignal->GetBankIndex();
//       ref_name = F1RefSignal->GetRefSignalName(); 

//       if( (bank_idx == bank_index) and (ref_name == name) ) {
// 	return F1RefSignal->GetNoRefTimeAU();
//       }
//     }

//   return 0.0;
// }

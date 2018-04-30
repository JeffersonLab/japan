/**********************************************************\
* File: MQwF1TDC.C                                         *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha,                *
*         J. H. Lee                                        *
* Time-stamp: <2010-04-12 12:12>                           *
\**********************************************************/

#include "MQwF1TDC.h"
#include "QwColor.h"
#include "QwLog.h"

#include <math.h>


const UInt_t MQwF1TDC::kF1Mask_SlotNumber           = 0xf8000000;
const UInt_t MQwF1TDC::kF1Mask_ResolutionLockFlag   = 0x04000000;
const UInt_t MQwF1TDC::kF1Mask_OutputFIFOFlag       = 0x02000000;
const UInt_t MQwF1TDC::kF1Mask_HitFIFOFlag          = 0x01000000;

const UInt_t MQwF1TDC::kF1Mask_HeaderFlag           = 0x00800000;

const UInt_t MQwF1TDC::kF1Mask_FakeDataFlag         = 0x00400000;
const UInt_t MQwF1TDC::kF1Mask_ChannelNumber        = 0x003f0000;
const UInt_t MQwF1TDC::kF1Mask_ChipAddress          = 0x00380000;
const UInt_t MQwF1TDC::kF1Mask_ChannelAddress       = 0x00070000;

const UInt_t MQwF1TDC::kF1Mask_Dataword             = 0x0000ffff;

const UInt_t MQwF1TDC::kF1Mask_HeaderTrigFIFOFlag   = 0x00400000;
const UInt_t MQwF1TDC::kF1Mask_HeaderEventNumber    = 0x003f0000;
const UInt_t MQwF1TDC::kF1Mask_HeaderTriggerTime    = 0x0000ff80;
const UInt_t MQwF1TDC::kF1Mask_HeaderXorSetupFlag   = 0x00000040;
const UInt_t MQwF1TDC::kF1Mask_HeaderChannelNumber  = 0x0000003f;
const UInt_t MQwF1TDC::kF1Mask_HeaderChipAddress    = 0x00000038;
const UInt_t MQwF1TDC::kF1Mask_HeaderChannelAddress = 0x00000007;



MQwF1TDC::MQwF1TDC()
{ 
  fF1ROCNumber          = 0;
  fF1SlotNumber         = 0;

  fF1HeaderFlag         = kFALSE;

  fF1HitFIFOFlag        = kFALSE;
  fF1OutputFIFOFlag     = kFALSE;
  fF1ResolutionLockFlag = kFALSE;

  fF1FakeDataFlag       = kFALSE;
  fF1ChannelNumber      = 0;
  fF1Dataword           = 0;


  fF1HeaderTrigFIFOFlag = kFALSE;
  fF1HeaderEventNumber  = 0;
  fF1HeaderTriggerTime  = 0;
  fF1HeaderXorSetupFlag = kFALSE;

  fF1MaxChannelsPerModule = 64; 

  // This initial fF1MaxChannelsPerModule 64
  // is used to "resize" a vector in RegisterSlotNumber() function
  // in each Subsystem, before one can access the real F1TDC 
  // configuration from CODA buffer. And it is a constant value (64)
  // and is totally independent upon the real F1TDC configuration. 
  // The real maximum channel number can be access via F1TDContainer
  // class of each subsystem.

  // However, Qweak uses only the Normal Resolution configuration. 
  // Thus, it is always 64 channels we uses. 
  // If someone wants to use the High Resolution Mode of F1TDC, 
  // it would be better to change this number to 32 by hand.
  // Friday, September  3 13:50:49 EDT 2010, jhlee

  fF1OverFlowEntryFlag  = kFALSE;
  fF1ValidDataSlotFlag  = kFALSE;
}

MQwF1TDC::~MQwF1TDC() { }


void MQwF1TDC::DecodeTDCWord(UInt_t &word, const UInt_t roc_id)
{

  fF1ROCNumber  = roc_id;
  fF1SlotNumber = (word & kF1Mask_SlotNumber)>>27;

  if( fF1SlotNumber>=1 && fF1SlotNumber<=21 ) fF1ValidDataSlotFlag = kTRUE;
  else                                        fF1ValidDataSlotFlag = kFALSE;

  
  fF1HeaderFlag         = ((word & kF1Mask_HeaderFlag)==0); 
  // TRUE if the mask bit IS NOT set
  
  // These three flags should be TRUE if their mask bit IS set
  fF1HitFIFOFlag        = ((word & kF1Mask_HitFIFOFlag       )!=0);
  fF1OutputFIFOFlag     = ((word & kF1Mask_OutputFIFOFlag    )!=0); 
  fF1ResolutionLockFlag = ((word & kF1Mask_ResolutionLockFlag)!=0);
  
  if (fF1HeaderFlag){
    //  This is a header word.
    fF1Dataword           = 0;
    fF1HeaderTrigFIFOFlag = ((word & kF1Mask_HeaderTrigFIFOFlag)!=0);
    fF1HeaderEventNumber  = ( word & kF1Mask_HeaderEventNumber )>>16;
    fF1HeaderTriggerTime  = ( word & kF1Mask_HeaderTriggerTime )>>7;
    fF1HeaderXorSetupFlag = ((word & kF1Mask_HeaderXorSetupFlag)!=0);
    fF1ChannelNumber      = ( word & kF1Mask_HeaderChannelNumber );
    fF1ChipAddress        = ( word & kF1Mask_HeaderChipAddress)>>3;
    fF1ChannelAddress     = ( word & kF1Mask_HeaderChannelAddress);
  } 
  else {
    // This is a data word.
    fF1FakeDataFlag   = ((word & kF1Mask_FakeDataFlag)!=0); // This flag should be TRUE if their mask bit IS set
    fF1ChannelNumber  = ( word & kF1Mask_ChannelNumber )>>16;
    fF1ChipAddress    = ( word & kF1Mask_ChipAddress )>>19;
    fF1ChannelAddress = ( word & kF1Mask_ChannelAddress )>>16;
    fF1Dataword       = ( word & kF1Mask_Dataword );

    if(fF1Dataword == 65535) fF1OverFlowEntryFlag = kTRUE;
    else                     fF1OverFlowEntryFlag = kFALSE;
    // skip to record overflow dataword entry (65535, 0xFFFF)
    fF1HeaderEventNumber   = 0;
    fF1HeaderTriggerTime   = 0;
    // std::cout << "fake flag " << fF1FakeDataFlag
    // 	      << " channel: " << fF1ChannelNumber 
    // 	      << " raw time: " << fF1Dataword 
    // 	      << std::endl;
  }
  return;
}

std::ostream& operator<< (std::ostream& os, const MQwF1TDC &f1tdc)
{
  if(f1tdc.fF1HeaderFlag) {
    os << "<<<< Header:"; 
  }
  else {
    os << ">>>>  DATA :";
  }

  os << " Ch" << std::setw(3) << f1tdc.fF1ChannelNumber;
  os << "[" << f1tdc.fF1ChipAddress;
  os << "," << f1tdc.fF1ChannelAddress;
  os << "]";

  if(f1tdc.fF1HeaderFlag) {
    os << " Xor " << f1tdc.fF1HeaderXorSetupFlag
       << " tOF " << f1tdc.fF1HeaderTrigFIFOFlag;
  }
  else {
     os << " - DATA " 
       << f1tdc.fF1FakeDataFlag
       << " - ";
   }
  
  os << "(hitOF,outOF,resLK)("
     << f1tdc.fF1HitFIFOFlag
     << f1tdc.fF1OutputFIFOFlag
     << f1tdc.fF1ResolutionLockFlag
     << ")";
  os << " ROC" << std::setw(2) << f1tdc.fF1ROCNumber;

  Int_t slot = 0;
  slot = f1tdc.fF1SlotNumber;

  os << " Slot" << std::setw(2) << slot;
    
  if(f1tdc.fF1HeaderFlag) {
    os << " EvtN" << std::setw(2) << f1tdc.fF1HeaderEventNumber;
    os << " TriT" << std::setw(4) << f1tdc.fF1HeaderTriggerTime;
  }
  else {
    os << " RawT " << std::setw(10) << f1tdc.fF1Dataword;
  }
  
  if(slot == 0) {
    os << ": a filler word";
  }
  if( (not f1tdc.fF1HeaderFlag) and (f1tdc.fF1FakeDataFlag) ){
    os << ": --> fake data";
  }
  
    //  os << std::endl;
  return os;
}



void MQwF1TDC::Print(Bool_t flag)
{
  if (! this) return; // do nothing if this is a null object

  if(flag) {
    std::cout << *this << std::endl;
  }
  return;
}


void MQwF1TDC::PrintTDCHeader(Bool_t flag)
{
  if (! this) return; // do nothing if this is a null object

  if(flag) {
    std::cout << *this << std::endl;
  }
  return;
}


void MQwF1TDC::PrintTDCData(Bool_t flag)
{
  if (! this) return; // do nothing if this is a null object
  
  if(flag) {
    std::cout << *this << std::endl;
  }
  return;
}


// Double_t MQwF1TDC::SubtractReference(Double_t rawtime, Double_t reftime)
// {
//   //  Note that this produces the opposite sign of the corrected time
//   //  as compared to the UInt_t version from revision 423 shown above.
//   //
//   //  For Region 3, according to the UInt_t version from revision 423,
//   //  the internal parameters should be:
//   //      fMinDiff   = -30000
//   //      fMaxDiff   =  30000
//   //      fOffset    =  64495
//   //      fTimeShift =   8929
//   //
//   Double_t real_time = rawtime - reftime;  

//   if( real_time < fMinDiff ) {
//     real_time += fOffset;
//   }
//   else if( real_time > fMaxDiff) {
//     real_time -= fOffset;
//   }
//   real_time = real_time + fTimeShift;
//   return real_time;
// }



// Double_t MQwF1TDC::ActualTimeDifference(Double_t raw_time, Double_t ref_time)
// {
 
//   Double_t trigger_window = 12896.0;
//   Double_t time_offset    = 65341.0;

//   Double_t time_condition = 0.0;
//   Double_t local_time_difference = 0.0;
//   Double_t actual_time_difference = 0.0;

//   local_time_difference = raw_time - ref_time; 
  
//   if(local_time_difference == 0.0) {
//     // raw_time is the same as ref_time
//     actual_time_difference = local_time_difference;
//   }
//   else {
//     time_condition = fabs(local_time_difference); 
//     // maximum value is trigger_window -1, 
//     // thus 12896-1 =  12895 - 0 = 12985
//     if(time_condition < trigger_window) {
//       // there is no ROLLEVENT within trigger window
//       actual_time_difference = local_time_difference;
//     }
//     else {
//       // there is an ROLLOVER event within trigger window
//       if (local_time_difference > 0.0) {
// 	// ref_time is in after ROLLOVER event
// 	actual_time_difference =  local_time_difference - time_offset;
//       }
//       else {
// 	// we already excluded local_time_diffrence == 0 case.
// 	// ref_time is in before ROLLOVER event
// 	actual_time_difference = local_time_difference + time_offset;
//       }
      
//     }
//   }
//   return actual_time_difference;
// }



void MQwF1TDC::PrintResolutionLockStatus(const UInt_t roc_id)
{
  if (not fF1ResolutionLockFlag) {
    QwWarning << "F1TDC board RESOULTION LOCK FAIL at Ch " 
	      << GetTDCChannelNumber() << " ROC " << roc_id
	      << " Slot " << GetTDCSlotNumber() << QwLog::endl;
  }
  return;
}



void MQwF1TDC::PrintHitFIFOStatus(const UInt_t roc_id)
{
  if (fF1HitFIFOFlag) {
    QwWarning << "F1TDC board HIT FIFO FULL at Ch "
              << GetTDCChannelNumber() << " ROC " << roc_id
              << " Slot " << GetTDCSlotNumber() << QwLog::endl;
  }
  return;
}



void MQwF1TDC::PrintOutputFIFOStatus(const UInt_t roc_id)
{
  if (fF1OutputFIFOFlag) {
    QwWarning << "F1TDC board OUTPUT FIFO FULL at Ch "
              << GetTDCChannelNumber() << " ROC " << roc_id
              << " Slot " << GetTDCSlotNumber() << QwLog::endl;
  }
  return;
}


Bool_t MQwF1TDC::IsValidDataword() const
{
  // fF1ValidDataSlotFlag = TRUE,
  // fF1ResolutionFlag    = TRUE,  
  // fF1HeaderFlag        = FALSE, 
  // fF1OverFlowEntry     = FALSE, 
  // fF1FakeDataWord      = FALSE, then it is a valid data word.
  if( fF1ValidDataSlotFlag && fF1ResolutionLockFlag && !fF1HeaderFlag && !fF1OverFlowEntryFlag && !fF1FakeDataFlag)
    //  if( fF1ValidDataSlotFlag && fF1ResolutionLockFlag && !fF1HeaderFlag)
    return kTRUE;
  else                                             
    return kFALSE; 
}


// Bool_t MQwF1TDC::CheckDataIntegrity(const UInt_t roc_id, UInt_t *buffer, UInt_t num_words)
// {
//   UInt_t reference_trig_time = 0;
//   UInt_t reference_event_num = 0;

//   const Int_t valid_trigger_time_offset = 1;
  
//   Bool_t event_ok_flag       = kFALSE;
//   Bool_t trig_time_ok_flag   = kFALSE;
//   Bool_t data_integrity_flag = kFALSE;

//   Bool_t temp_print_flag = false;

//   for (UInt_t i=0; i<num_words ; i++) {

//     DecodeTDCWord(buffer[i], roc_id); 

 
//     // We use the multiblock data transfer for F1TDC, thus
//     // we must get the event number and the trigger time from the first buffer
//     // (buffer[0]), and these valuse can be used to check "data" integrity
//     // over all F1TDCs
//     if ( i==0 ) {//;
//       if ( IsHeaderword() ) {//;;
// 	reference_event_num = GetTDCEventNumber();
// 	reference_trig_time = GetTDCTriggerTime();
// 	PrintTDCHeader(temp_print_flag);
//       }//;;
//       else {//;;
// 	QwWarning << QwColor(Qw::kRed)
// 		  << "The first word of F1TDC must be header word. "
// 		  << "Something wrong into this CODA stream.\n";
// 	QwWarning << QwLog::endl;
// 	return false;
//       }//;;
//     }//;
//     else {//;
//       if ( IsHeaderword() ) {//;;
// 	// Check date integrity, if it is fail, we skip this whole buffer to do further process 
// 	event_ok_flag     = ( reference_event_num == GetTDCHeaderEventNumber() );
// 	trig_time_ok_flag = abs( reference_trig_time - GetTDCHeaderTriggerTime() ) <= valid_trigger_time_offset;
// 	PrintTDCHeader(temp_print_flag);

// 	// If SEU exists, Xor Setup Register bit is changing from 1 to 0, I think.
// 	// Thus, it sets the trigger time 0 and the event number 0.
// 	// For example, 
// 	// Ch  8 Xor 0 tOF 0(hitOF,outOF,resLK)(001) ROC  9 Slot 10 EvtN 0 TriT   0
// 	// And it is the source of the trigger time and the event number differences
// 	// within the same ROC. In the CheckDataIntegrity routine, I decided
// 	// to skip such a event. 
// 	// Sunday, August  8 03:42:48 EDT 2010, jhlee
	
// 	if (IsHeaderXorSetup()) {//;;;
// 	  // Trigger Time difference of up to 1 count among the chips is acceptable
// 	  // For the Trigger Time, this assumes that an external SYNC_RESET signal has
// 	  // been successfully applied at the start of the run
// 	  if (not trig_time_ok_flag) {
// 	    if(temp_print_flag){
// 	      QwWarning  << QwColor(Qw::kBlue)
// 			 << "There is the no SYNC_RESET on the F1TDC board at"
// 			 << " Ch "   << fF1ChannelNumber
// 			 << " ROC "  << fF1ROCNumber 
// 			 << " Slot " << fF1SlotNumber <<"\n";
// 	      QwWarning  << QwColor(Qw::kBlue)
// 			 << "This event is excluded from the ROOT data stream.\n";
// 	      QwWarning << QwColor(Qw::kRed) 
// 			<< "Please, send an email to (a) Qweak DAQ expert(s) if you have time.\n";
// 	      QwWarning  << QwLog::endl;
// 	    }
// 	  }
// 	  // Any difference in the Event Number among the chips indicates a serious error
// 	  // that requires a reset of the board.
	  
// 	  if (not event_ok_flag) {
// 	    if(temp_print_flag){
// 	      QwWarning << QwColor(Qw::kRed)
// 			<< "There is the Event Number Mismatch issue on the F1TDC board at"
// 			<< " ROC "  << fF1ROCNumber
// 			<< " Slot " << fF1SlotNumber << "\n";
// 	      QwWarning << QwColor(Qw::kRed) 
// 			<< "This event is excluded from the ROOT data stream.\n";
// 	      QwWarning << QwColor(Qw::kRed) 
// 			<< "Please, send an email to (a) Qweak DAQ expert(s) if you have time.\n";
// 	      QwWarning << QwLog::endl;
// 	    }
// 	  }
	  
// 	  data_integrity_flag = (event_ok_flag) && (trig_time_ok_flag) && IsNotHeaderTrigFIFO() ;

// 	  if (not data_integrity_flag)  return data_integrity_flag; //false
// 	  // we stop check data, because all other next buffers 
// 	  // is useless and we don't need to check them in order to save some time.
// 	} //;;;
// 	else {//;;;

// 	  if (temp_print_flag) {

// 	    // I don't include the SEU in the CheckDataIntegrity.
// 	    // At this moment, I have no idea how I handle during data processes.
// 	    // Sunday, August  8 04:02:17 EDT 2010, jhlee
	    
// 	    QwWarning << QwColor(Qw::kRed)
// 		      << "There is the Single Event Upset (SEU) on the F1TDC board at"
// 		      << " ROC "  << fF1ROCNumber
// 		      << " Slot " << fF1SlotNumber << "\n";
// 	    QwWarning << QwColor(Qw::kRed) 
// 		      << "Please, send an email to (a) Qweak DAQ expert(s) if you have time.\n";
// 	    QwWarning << QwLog::endl;
// 	  }
// 	}//;;;
//       }//;;
//       else { //;; // dataword
// 	if(!fF1OverFlowEntryFlag) PrintTDCData(temp_print_flag);
//       }//;;
//     }//;
//   }//for (UInt_t i=0; i<num_words ; i++) {
  
  
//   return (data_integrity_flag); 
  
// }


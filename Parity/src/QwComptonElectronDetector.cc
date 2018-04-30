/**************************************************
 * \file	QwComptonElectronDetector.cc
 * \brief	Implementation of the analysis of Compton electron detector data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * \D. Dutta
 *
 * \V. Tvaskis
 * I have addet hits selections procedure, to select best hits, which corresponds
 * to the best track (track with the best Chi2). Tracking procedure is also included here.
 * Also, Angular distribution of the tracks is added.
 *
 * \A. Narayan 
 ****************************************************/

/*****************************************************
 * The QwComptonElectronDetector class is defined as a parity subsystem that
 * contains all data modules of the electron detector (V1495, ...).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *****************/
#include "QwComptonElectronDetector.h"
#include "QwSubsystemArrayParity.h"
#include "MQwCodaControlEvent.h"

// System headers
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include <math.h>

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"

using namespace std;

// Ugly, should go inside functions, seems undefined for running sum
Int_t notGood=0,myrun;

// Register this subsystem with the factory
RegisterSubsystemFactory(QwComptonElectronDetector);


/*****************************************************************
 * Load the channel map
 * @param mapfile Map file
 * @return Zero if successful
 *****************************************************************/
Int_t QwComptonElectronDetector::LoadChannelMap(TString mapfile)
{
  TString varname, varvalue;
  Int_t fdettype;
  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t currentsubbankindex=-1;


  QwParameterFile mapstr(mapfile.Data());  // Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment();    // Remove everything after a comment character.
    mapstr.TrimWhitespace(); // Get rid of leading and trailing whitespace (spaces or tabs).
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=", varname, varvalue)) {
      // This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);
      if (varname == "roc") {
	currentrocread=value;
        RegisterROCNumber(value,0);
      } else if (varname == "bank") {
        currentbankread=value;
        RegisterSubbank(value);
	if(currentsubbankindex!=GetSubbankIndex(currentrocread,currentbankread))
	  currentsubbankindex=GetSubbankIndex(currentrocread,currentbankread);
      }
    } else {
      //  Break this line into tokens to process it.
      TString modtype = mapstr.GetTypedNextToken<TString>();
      UInt_t modnum   = mapstr.GetTypedNextToken<UInt_t>();
      UInt_t channum __attribute__ ((unused)) // unused but function call is still required
                      = mapstr.GetTypedNextToken<UInt_t>(); //if commented, 'dettype' is not evaluated properly.
      TString dettype = mapstr.GetTypedNextToken<TString>();
      TString name    = mapstr.GetTypedNextToken<TString>();
      Int_t plane     = mapstr.GetTypedNextToken<Int_t>();
      Int_t stripnum  = mapstr.GetTypedNextToken<Int_t>();
      //  Push a new record into the element array
      if (modtype == "V1495") {
	if (dettype == "eaccum") {
          // Register data channel type
          fMapping[currentsubbankindex] = kV1495Accum;
          fdettype = 1;
	  ///the assignment of fdettype is arbitrarily made but is honoured subsequently
	  ///due to the numbers assigned for fdettype, the first index of vector 'fSubbankIndex' holds data
	  ///in the order:: 0:esingle; 1:eaccum; 2:escalers; 3:eaccum_v2; 4:eDAQ_info
	  ///the assignment of fdettype carries the signature of which bank was created first and not which should have appeared first
 	  if (fdettype >= (Int_t) fSubbankIndex.size())
	    fSubbankIndex.resize(fdettype+1);
          if (modnum >= fSubbankIndex[fdettype].size()) {
            fSubbankIndex[fdettype].push_back(modnum);
            fSubbankIndex[fdettype][modnum] = currentsubbankindex;
          }
	  if (plane >= (Int_t) fStripsRaw.size())
	    fStripsRaw.resize(plane);
	  if (stripnum >= (Int_t) fStripsRaw[plane-1].size())
	    fStripsRaw[plane-1].push_back(0);
	  // plane goes from 1 - 4 instead of 0 - 3,

	  if (plane >= (Int_t) fStrips.size())
	    fStrips.resize(plane);
	  if (stripnum >= (Int_t) fStrips[plane-1].size())
            fStrips[plane-1].push_back(0);
        }
	
	else if (dettype == "escalers") {
          // Register data channel type
          fMapping[currentsubbankindex] = kV1495Scaler;
          fdettype = 2;
 	  if (fdettype >= (Int_t) fSubbankIndex.size())
	    fSubbankIndex.resize(fdettype+1);
          if (modnum >= fSubbankIndex[fdettype].size()) {
            fSubbankIndex[fdettype].push_back(modnum);
            fSubbankIndex[fdettype][modnum] = currentsubbankindex;
          }
	  if (plane >= (Int_t) fStripsRawScal.size())
	    fStripsRawScal.resize(plane);
	  if (stripnum >= (Int_t) fStripsRawScal[plane-1].size())
	    fStripsRawScal[plane-1].push_back(0);
	}

	else if (dettype == "eaccum_v2") {
          // Register data channel type
          fMapping[currentsubbankindex] = kV1495Accum_v2;
          fdettype = 3;
 	  if (fdettype >= (Int_t) fSubbankIndex.size())
	    fSubbankIndex.resize(fdettype+1);
          if (modnum >= fSubbankIndex[fdettype].size()) {
            fSubbankIndex[fdettype].push_back(modnum);
            fSubbankIndex[fdettype][modnum] = currentsubbankindex;
          }
	  if (plane >= (Int_t) fStripsRaw_v2.size())
	    fStripsRaw_v2.resize(plane);
	  if (stripnum >= (Int_t) fStripsRaw_v2[plane-1].size())
	    fStripsRaw_v2[plane-1].push_back(0);
	}

	else if (dettype == "esingle") {
          // Register data channel type
          fMapping[currentsubbankindex] = kV1495Single;
          fdettype = 0;
	  if (fdettype >= (Int_t) fSubbankIndex.size())
	    fSubbankIndex.resize(fdettype+1);
          if (modnum >= fSubbankIndex[fdettype].size()) {
            fSubbankIndex[fdettype].push_back(modnum);
            fSubbankIndex[fdettype][modnum] = currentsubbankindex;
          }
	  if (plane >= (Int_t) fStripsRawEv.size())
	    fStripsRawEv.resize(plane);
	  if (stripnum >= (Int_t) fStripsRawEv[plane-1].size())
	    fStripsRawEv[plane-1].push_back(0);

	  if (plane >= (Int_t) fStripsEv.size())
	    fStripsEv.resize(plane);
	  if (stripnum >= (Int_t) fStripsEv[plane-1].size())
            fStripsEv[plane-1].push_back(0);
	} // end of switch (dettype)
	else {
	  notGood=notGood+1;
	  cout<<"***found an undefined data type "<<notGood<<" times"<<endl;
	  cout<<"dettype: "<<dettype<<endl;
	}
      } // end of switch (modtype)
    } // end of if for token line
  } // end of while over parameter file  
  return 0;
}

//*****************************************************************
Int_t QwComptonElectronDetector::LoadEventCuts(TString & filename)
{
  return 0;
}

//*****************************************************************
Int_t QwComptonElectronDetector::LoadInputParameters(TString pedestalfile)
{
  // Open the file
  QwParameterFile mapstr(pedestalfile.Data());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    else {
      TString varname = mapstr.GetTypedNextToken<TString>();  // name of the channel
      varname.ToLower();
      varname.Remove(TString::kBoth,' ');
      /* Double_t varcal = */ mapstr.GetTypedNextToken<Double_t>(); // value of the calibration factor
    }
  } // end of loop reading all lines of the pedestal file
  return 0;
}

/**
 * Process the event buffer for this subsystem
 * @param roc_id ROC ID
 * @param bank_id Subbank ID
 * @param buffer Buffer to read from
 * @param num_words Number of words left in buffer
 * @return Number of words read
 */
//*****************************************************************
Int_t QwComptonElectronDetector::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  const Int_t numberofWordPerEv = 5;
  UInt_t words_read = 0;
  UInt_t bitwise_mask = 0;
  div_t div_output;
  //UInt_t accum_count = 0;
  // Get the subbank index (or -1 when no match)
  Int_t subbank = GetSubbankIndex(roc_id, bank_id);
  if (subbank >= 0 && num_words > 0) {
    //  We want to process this ROC.  Begin looping through the data.
    switch (fMapping[subbank]) {
    case kV1495Accum:
      {	
	for (Int_t k = 0; k < NModules; k++) {
	  if (fSubbankIndex[1][k]==subbank) {
	    // sub-bank 0x0204, accum mode data from strips 0-31 of planes 1 thru 4
	    if (num_words > 0) { //!?this has already been checked in this function. remove this if?

	      for (Int_t i = 0; i < StripsPerModule; i++) { // loop all words in bank
		Int_t j = k*StripsPerModule+i;
		// accum_count = (buffer[i] & 0xff000000) >> 24;
		// //!what value is assigned to fStripsRaw[][] when the accum_counts is greater than 5?
		// if (accum_count < 5)fStripsRaw[0][j] = accum_count;
		// accum_count = (buffer[i] & 0x00ff0000) >> 16;
		// if (accum_count < 5)fStripsRaw[1][j] = accum_count;
		// accum_count = (buffer[i] & 0x0000ff00) >> 8;
		// if (accum_count < 5)fStripsRaw[2][j] = accum_count;
		// accum_count = (buffer[i] & 0x000000ff);
		// if (accum_count < 5)fStripsRaw[3][j] = accum_count;
		
		fStripsRaw[0][j] = (buffer[i] & 0xff000000) >> 24;
		fStripsRaw[1][j] = (buffer[i] & 0x00ff0000) >> 16;
		fStripsRaw[2][j] = (buffer[i] & 0x0000ff00) >> 8;
		fStripsRaw[3][j] = (buffer[i] & 0x000000ff);
		words_read++;
	      }
	    }
	    if (num_words != words_read) {
	      QwError << "QwComptonElectronDetector: There were "
		      << num_words - words_read
		      << " leftover words after decoding everything we recognize in accum data"
		      << std::hex
		      << " in ROC " << roc_id << ", bank " << bank_id << "."
		      << std::dec
		      << QwLog::endl;
	    }
	  }
	} // for (Int_t k = 0; k < NModules; k++) 
	break;
      }
      
    case kV1495Single:
      {	
	for (Int_t jj = 0; jj < NModules; jj++) {
	  if (fSubbankIndex[0][jj]==subbank) {
	    if (num_words > 0) {
	      div_output = div(num_words,numberofWordPerEv);
	      Int_t numbOfEvDataSet = div_output.quot;

	      Int_t evHitMap[numbOfEvDataSet][NPlanes][StripsPerPlane];

	      for (Int_t m = 0; m < numbOfEvDataSet; m++) {
		for (Int_t i = 0; i <NPlanes; i++) {
		  for (Int_t j = 0; j < StripsPerPlane; j++) {
		    evHitMap[m][i][j] = 0;
		  }
		}
	      }

	      if (div_output.rem) { 
		printf("\n\n***Error***:Event Data suspicious. Report to an expert:\n");
		printf(",found %d number of words which is not a multiple of 5!\n\n",num_words);
	      }///This should not happen until the event word readout format is changed in DAQ

	      for (Int_t m = 0; m < numbOfEvDataSet; m++) {
		for (Int_t i = 0; i < NPlanes; i++) {
		  Int_t bufferIndex = m*(NPlanes+1) + i;
		  for (Int_t j = 0; j < StripsPerModule; j++) {
		    Int_t k = jj*StripsPerModule + j; 
		    bitwise_mask = (0x1 << j);
		    evHitMap[m][i][k] = (buffer[bufferIndex] & bitwise_mask) >> j;
		  }
		  words_read++;
		}	
		//Int_t ExtraWord = buffer[NPlanes*(m+1)];//diagnostic word, ignore warning, unused currently
		words_read++;
	      }
	      for (Int_t k = 0; k < StripsPerPlane; k++) {
		for (Int_t i = 0; i < NPlanes; i++) {
		  for (Int_t m = 0; m < numbOfEvDataSet; m++) {
		    fStripsRawEv[i][k] += evHitMap[m][i][k]; 
		  }
		}
	      }
	      
	      if (num_words != words_read) {
		QwError << "QwComptonElectronDetector: There were "
			<< num_words - words_read
			<< " leftover words after decoding everything recognizable in event data."
			<< QwLog::endl;
	      }
	    }
	  }
	}
	break;
      }///end of case V1495Singles
      
    case kV1495Scaler:
      {
	for (Int_t k = 0; k < NModules; k++) {
	  if (fSubbankIndex[2][k]==subbank) {
	    // sub-bank 0x020A, V1495 SCALER data from strips 0-31 of planes 1 thru 4
	    if (num_words > 0) {		
	      //  We want to process this ROC.  Begin looping through the data.
	      for (Int_t i = 0; i < StripsPerModule; i++) { // loop all words in bank
		Int_t j = k*StripsPerModule + i;
		fStripsRawScal[0][j] = (buffer[i] & 0xff000000) >> 24;
		fStripsRawScal[1][j] = (buffer[i] & 0x00ff0000) >> 16;
		fStripsRawScal[2][j] = (buffer[i] & 0x0000ff00) >> 8;
		fStripsRawScal[3][j] = (buffer[i] & 0x000000ff);
		words_read++;
		//if (fStripsRawScal[0][j] > 0) printf("\nfStripsRawScal[%d][%d]:%f",i,j,fStripsRawScal[i][j]);
	      }
	    }
	    if (num_words != words_read) {
	      QwError << "QwComptonElectronDetector: There were "
		      << num_words - words_read
		      << " leftover words after decoding everything we recognize in v1495 scaler data"
		      << std::hex
		      << " in ROC " << roc_id << ", bank " << bank_id << "."
		      << std::dec
		      << QwLog::endl;
	    }	      
	  } // if (fSubbankIndex[1][k]==subbank) 
	} // for (Int_t k = 0; k < NModules; k++)  
	break;
      }
     
    case kV1495Accum_v2:
      {	
	for (Int_t k = 0; k < NModules; k++) {
	  if (fSubbankIndex[3][k]==subbank) {
	    // sub-bank 0x020D, accum mode V2 data from strips 0-31 of planes 1 thru 4
	    if (num_words > 0) { //!?this has already been checked in this function. remove this if?
	      //!the condition that total no.of counts be less than '5' is not executed for v2 data(just to see this side for the story, if needed)
	      for (Int_t i = 0; i < StripsPerModule; i++) { // loop all words in bank
		Int_t j = k*StripsPerModule+i;
		fStripsRaw_v2[0][j] = (buffer[i] & 0xff000000) >> 24;
		fStripsRaw_v2[1][j] = (buffer[i] & 0x00ff0000) >> 16;
		fStripsRaw_v2[2][j] = (buffer[i] & 0x0000ff00) >> 8;
		fStripsRaw_v2[3][j] = (buffer[i] & 0x000000ff);
		words_read++;
	      }
	    }
	    if (num_words != words_read) {
	      QwError << "QwComptonElectronDetector: There were "
		      << num_words - words_read
		      << " leftover words after decoding everything we recognize in version 2 accum data"
		      << std::hex
		      << " in ROC " << roc_id << ", bank " << bank_id << "."
		      << std::dec
		      << QwLog::endl;
	    }
	  }
	} // for (Int_t k = 0; k < NModules; k++) 
	break;
      }
      
    case kUnknown: // Unknown data channel type
    default:
      {
	QwError << "QwComptonElectronDetector: Unknown data channel type for ROC " 
		<< roc_id << ", bank 0x" <<std::hex<<bank_id << std::dec<<QwLog::endl;
	break;
      }
    }
  }
  return words_read;
}

/*** Process the single event cuts * @return */
//*****************************************************************
Bool_t QwComptonElectronDetector::SingleEventCuts()
{
  return IsGoodEvent();
}

/*** Process this event */
//*****************************************************************
void  QwComptonElectronDetector::ProcessEvent()
{
  //this was not being used till revision 4846 so is being removed
  return;
}

/**
 * Process the configuration buffer for this subsystem
 * @param roc_id ROC ID
 * @param bank_id Subbank ID
 * @param buffer Buffer to read from
 * @param num_words Number of words left in buffer
 * @return Number of words read
 */
//*****************************************************************
Int_t QwComptonElectronDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  // sub-bank 0x207 & 0x208, eDAQinfo for this slave board for all planes and strips
  ///the total number of words in these subbanks was fixed at 8, though the meaning carried by a word has
  ///..changed during different revisions (may need fine tuning later!)

  ///Based on the last edited firmware, the data structure of the DAQ info bank is:
  // Identifier beginning word
  // Firmware Revision Number
  // A mask
  // B mask
  // D mask
  // E mask
  // PWTL(8 bits) | PWDL(8 bits) | HOLDOFF(8 bits) | PL_Delay(8 bits)
  //Rejection Width(16 bits) | Accum trigger condition(8 bits) | Event trigger condition(8 bits)
  ///This will not be the same for all runs in even run 2, leave apart run 1
  
  Int_t subBankiterate = bank_id==0x207 ? 0 : 32;
  Int_t subBankmodule = bank_id==0x207 ? 0 : 1;

  slave_header.push_back(buffer[0]);///expect this to be always BX000000 for slaveX
  firmwareRevision.push_back(buffer[1]);
  portAmask.push_back(buffer[2]);
  portBmask.push_back(buffer[3]);
  portDmask.push_back(buffer[4]);
  portEmask.push_back(buffer[5]);
  widthInfo.push_back(buffer[6]);
  trigInfo.push_back (buffer[7]);

  UInt_t bitwise_mask=0;
  for (Int_t s = 0; s < StripsPerModule; s++) {
    bitwise_mask = (0x1 << s);
    fPlane1Mask[s+subBankiterate] = ((portAmask.back() & bitwise_mask) >> s);
  }

  for (Int_t s = 0; s < StripsPerModule; s++) {
    bitwise_mask = (0x1 << s);
    fPlane2Mask[s+subBankiterate] = ((portBmask.back() & bitwise_mask) >> s);
  }

  for (Int_t s = 0; s < StripsPerModule; s++) {
    bitwise_mask = (0x1 << s);
    fPlane3Mask[s+subBankiterate] = ((portDmask.back() & bitwise_mask) >> s);
  }

  for (Int_t s = 0; s < StripsPerModule; s++) {
    bitwise_mask = (0x1 << s);
    fPlane4Mask[s+subBankiterate] = ((portEmask.back() & bitwise_mask) >> s);
  }
   
  fFirmwareRevision[subBankmodule] = firmwareRevision.back();
   
  fPWTL[subBankmodule]          =(widthInfo.back() & 0xff000000) >> 24;
  fPWTL2[subBankmodule]         =(widthInfo.back() & 0x00ff0000) >> 16;
  fHoldOff[subBankmodule]       =(widthInfo.back() & 0x0000ff00) >> 8;
  fPipelineDelay[subBankmodule] =(widthInfo.back() & 0x000000ff);
   

  fMinWidth[subBankmodule] = (trigInfo.back() & 0x00ff0000) >> 16;
  fAcTrig[subBankmodule]   = (trigInfo.back() & 0x0000ff00) >> 8;
  fEvTrig[subBankmodule]   = (trigInfo.back() & 0x000000ff);

  subBankiterate = subBankiterate + 32;//when this process is called 2nd time,     
  return 0;
}

/**
 * Check whether this is a good event @return True if the event is good
 * !! The definition of 'good event' for the following function is that
 * it should have information for every strip (4planes x 96strips = 384)
 * I can't see why is this useful
 */
//*****************************************************************
Bool_t QwComptonElectronDetector::IsGoodEvent()
{
  Bool_t eventokay = kTRUE;
  Int_t nchan=0;
  for (Int_t i=0; i<NPlanes; i++)
    nchan += fStripsRaw[i].size();
  eventokay &= (nchan == 384);
  return eventokay;
}

/**
 * Clear the event data in this subsystem
 */
//*****************************************************************
void QwComptonElectronDetector::ClearEventData()
{
  for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){

      fStripsRaw[i][j] = 0;
      fStripsRaw_v2[i][j] = 0;
      fStrips[i][j] = 0;
      fStripsRawEv[i][j] = 0;
      fStripsEv[i][j] = 0;
      fStripsRawScal[i][j] = 0;
    }
  }
  
  // fStripsEvBest1 = 1000000;
  // fStripsEvBest2 = 1000000;
  // fStripsEvBest3 = 1000000;
  // edet_x2 = 1000000; 
  // edet_TotalNumberTracks = 1000000;
  // edet_angle = 1000000;
   
  fGoodEventCount = 0;
}

//*****************************************************************
VQwSubsystem&  QwComptonElectronDetector::operator=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    VQwSubsystem::operator=(value);
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (Int_t i = 0; i < NPlanes; i++){
      for (Int_t j = 0; j < StripsPerPlane; j++) {
        this->fStripsRaw[i][j] = input->fStripsRaw[i][j];
        this->fStrips[i][j] = input->fStrips[i][j];
        this->fStripsRawEv[i][j] = input->fStripsRawEv[i][j];
        this->fStripsRawScal[i][j] = input->fStripsRawScal[i][j];
        this->fStripsRaw_v2[i][j] = input->fStripsRaw_v2[i][j];
      }
    }
    this->fPlane1Mask.resize(input->fPlane1Mask.size());
    this->fPlane2Mask.resize(input->fPlane2Mask.size());
    this->fPlane3Mask.resize(input->fPlane3Mask.size());
    this->fPlane4Mask.resize(input->fPlane4Mask.size());
    this->fEvTrig.resize(input->fEvTrig.size());
    this->fAcTrig.resize(input->fAcTrig.size());
    this->fMinWidth.resize(input->fMinWidth.size());
    this->fFirmwareRevision.resize(input->fFirmwareRevision.size());
    this->fPWTL.resize(input->fPWTL.size());
    this->fPWTL2.resize(input->fPWTL2.size());
    this->fHoldOff.resize(input->fHoldOff.size());
    this->fPipelineDelay.resize(input->fPipelineDelay.size());

    for (UInt_t j = 0; j < input->fPlane1Mask.size(); j++)
      this->fPlane1Mask[j] = input->fPlane1Mask[j];
    for (UInt_t j = 0; j < input->fPlane2Mask.size(); j++)
      this->fPlane2Mask[j] = input->fPlane2Mask[j];
    for (UInt_t j = 0; j < input->fPlane3Mask.size(); j++)
      this->fPlane3Mask[j] = input->fPlane3Mask[j];
    for (UInt_t j = 0; j < input->fPlane4Mask.size(); j++)
      this->fPlane4Mask[j] = input->fPlane4Mask[j];

    for (UInt_t j = 0; j < input->fFirmwareRevision.size(); j++)
      this->fFirmwareRevision[j] = input->fFirmwareRevision[j];

    for (UInt_t j = 0; j < input->fPWTL.size(); j++)
      this->fPWTL[j] = input->fPWTL[j];

    for (UInt_t j = 0; j < input->fPWTL2.size(); j++)
      this->fPWTL2[j] = input->fPWTL2[j];

    for (UInt_t j = 0; j < input->fHoldOff.size(); j++)
      this->fHoldOff[j] = input->fHoldOff[j];

    for (UInt_t j = 0; j < input->fPipelineDelay.size(); j++)
      this->fPipelineDelay[j] = input->fPipelineDelay[j];

    for (UInt_t j = 0; j < input->fEvTrig.size(); j++)
      this->fEvTrig[j] = input->fEvTrig[j];
    for (UInt_t j = 0; j < input->fAcTrig.size(); j++)
      this->fAcTrig[j] = input->fAcTrig[j];
    for (UInt_t j = 0; j < input->fMinWidth.size(); j++)
      this->fMinWidth[j] = input->fMinWidth[j];
    
    this->fIsConfigOnly = input->fIsConfigOnly;
  }
  return *this;
}

VQwSubsystem&  QwComptonElectronDetector::operator+=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (Int_t i = 0; i < NPlanes; i++){
      for (Int_t j = 0; j < StripsPerPlane; j++){
        this->fStripsRaw[i][j] += input->fStripsRaw[i][j];      
	//this->fStrips[i][j] += input->fStrips[i][j];
        this->fStripsRawEv[i][j] += input->fStripsRawEv[i][j];
        this->fStripsRawScal[i][j] += input->fStripsRawScal[i][j];
        this->fStripsRaw_v2[i][j] += input->fStripsRaw_v2[i][j];
      }
    }
  }
  return *this;
}

VQwSubsystem&  QwComptonElectronDetector::operator-=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (Int_t i = 0; i < NPlanes; i++){
      for (Int_t j = 0; j < StripsPerPlane; j++) {
        this->fStripsRaw[i][j] -= input->fStripsRaw[i][j];
	//this->fStrips[i][j] -= input->fStrips[i][j];
        this->fStripsRawEv[i][j] -= input->fStripsRawEv[i][j];
        this->fStripsRawScal[i][j] -= input->fStripsRawScal[i][j];
        this->fStripsRaw_v2[i][j] -= input->fStripsRaw_v2[i][j];
      }
    }
  }
  return *this;
}

VQwSubsystem&  QwComptonElectronDetector::operator*=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (Int_t i = 0; i < NPlanes; i++){
      for (Int_t j = 0; j < StripsPerPlane; j++){
        this->fStripsRaw[i][j] *= input->fStripsRaw[i][j];
	//this->fStrips[i][j] *= input->fStrips[i][j];
        this->fStripsRawEv[i][j] *= input->fStripsRawEv[i][j];
        this->fStripsRawScal[i][j] *= input->fStripsRawScal[i][j];
        this->fStripsRaw_v2[i][j] *= input->fStripsRaw_v2[i][j];
      }
    }
  }
  return *this;
}

void  QwComptonElectronDetector::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this += value2;
  }
}

void  QwComptonElectronDetector::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this -= value2;
  }
}

void QwComptonElectronDetector::Ratio(VQwSubsystem *numer, VQwSubsystem *denom)
{
  if (Compare(numer) && Compare(denom)) {
    QwComptonElectronDetector* innumer = dynamic_cast<QwComptonElectronDetector*> (numer);
    QwComptonElectronDetector* indenom = dynamic_cast<QwComptonElectronDetector*> (denom);

    for (Int_t i = 0; i < NPlanes; i++){
      for (Int_t j = 0; j < StripsPerPlane; j++){        
	if(indenom->fStripsRaw[i][j] > 0) {
          fStripsRaw[i][j] =(innumer->fStripsRaw[i][j]/indenom->fStripsRaw[i][j]);
          //fStrips[i][j] =(innumer->fStrips[i][j]/indenom->fStrips[i][j]);
          fStripsRawEv[i][j] =(innumer->fStripsRawEv[i][j]/indenom->fStripsRawEv[i][j]);
          fStripsRawScal[i][j] =(innumer->fStripsRawScal[i][j]/indenom->fStripsRawScal[i][j]);
          fStripsRaw_v2[i][j] =(innumer->fStripsRaw_v2[i][j]/indenom->fStripsRaw_v2[i][j]);
	}	  
	else {
	  this->fStripsRaw[i][j]=0;
	}	     
      }
    }
  }
}

void QwComptonElectronDetector::Scale(Double_t factor)
{
  for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStripsRaw[i][j] *= static_cast<double>(factor);
      this->fStrips[i][j] *= static_cast<double>(factor);
      this->fStripsRawEv[i][j] *= static_cast<double>(factor);
      this->fStripsRawScal[i][j] *= static_cast<double>(factor);
      this->fStripsRaw_v2[i][j] *= static_cast<double>(factor);
      // TODO !!converting Double to Int (ok now) may not be okay later!
    }
  }
}

Bool_t QwComptonElectronDetector::Compare(VQwSubsystem *value)
{
  // Immediately fail on null objects
  if (value == 0) return kFALSE;

  // Continue testing on actual object
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;
  } else {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    if (input->fStripsRaw.size() != fStripsRaw.size()) {
      result = kFALSE;
    }
    // if (input->fStrips.size() != fStrips.size()) {
    //   result = kFALSE;
    // }
    if (input->fStripsRawEv.size() != fStripsRawEv.size()) {
      result = kFALSE;
    }
    if (input->fStripsRawScal.size() != fStripsRawScal.size()) {
      result = kFALSE;
    }
    if (input->fStripsRaw_v2.size() != fStripsRaw_v2.size()) {
      result = kFALSE;
    }
  }
  return result;
}

void  QwComptonElectronDetector::AccumulateRunningSum(VQwSubsystem* value)
{
  if (Compare(value)) {
    // Optional event selection...
    fGoodEventCount++;
    *this  += value;
  }
}

void  QwComptonElectronDetector::CalculateRunningAverage()
{
  if (fGoodEventCount <= 0) { 
    Scale(0);
  } else {
    Scale(1.0/fGoodEventCount);
  }
}


//*****************************************************************
/**
 * Construct the histograms
 * @param folder Folder in which the histograms will be created
 * @param prefix Prefix with information about the type of histogram
 */
void  QwComptonElectronDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{/// not adding the version-2 accum data to the histograms, only interested in adding it to tree
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  //  Go into subdirectory if it exists
  TDirectory* eDetfolder = folder->GetDirectory("Compton_Electron");
  if (eDetfolder == 0) eDetfolder = folder->mkdir("Compton_Electron");

  //  Now create the histograms.
  TString basename = prefix + GetSubsystemName();

  eDetfolder->cd();
  for (Int_t i=0; i<NPlanes; i++){
    TString histname = Form("Compton_eDet_Accum_Raw_Plane%d",i+1);
    fHistograms.push_back(gQwHists.Construct1DHist(prefix+histname));//1st histogram
    histname = Form("Compton_eDet_Accum_v2_Plane%d",i+1);
    fHistograms.push_back(gQwHists.Construct1DHist(prefix+histname));
    histname = Form("Compton_eDet_Evt_Raw_Plane%d",i+1);
    fHistograms.push_back(gQwHists.Construct1DHist(prefix+histname));//3rd histogram
    histname = Form("Compton_eDet_Evt_Plane%d",i+1);
    fHistograms.push_back(gQwHists.Construct1DHist(prefix+histname));
    histname = Form("Compton_eDet_Scal_Raw_Plane%d",i+1);
    fHistograms.push_back(gQwHists.Construct1DHist(prefix+histname));//5th histogram
  }
}

void  QwComptonElectronDetector::FillHistograms()
{
  for (Int_t i = 0; i < NPlanes; i++) {
    Int_t ii = 5*i;
    //apology for the hardcoding; had to do so in keeping with DaveG's philosophy of least change.
    //the 5 corresponds to the 5 histograms that are being declared above in the 'for' loop.
    ///The order in which the histograms are filled below should be the same in which they were created above
    for (Int_t j = 0; j < StripsPerPlane; j++) {
      Int_t nthStrip = j + 1;

      if (fHistograms[ii] != NULL) {
  	for (Int_t n=0; n<fStripsRaw[i][j]; n++) ///Accumulation raw data
	  fHistograms[ii]->Fill(nthStrip);	//!!Why does any of the following 2 method not work?
      }	//      fHistograms[4*i]->Fill(j+1,fStripsRaw[i][j]); 
	//      fHistograms[4*i]->SetBinContent(j+1,fStripsRaw[i][j]);
      if (fHistograms[ii+1] != NULL) {
	for (Int_t n=0; n<fStrips[i][j]; n++) ///Accumulation data
	  fHistograms[ii+1]->Fill(nthStrip);
      }

      if (fHistograms[ii+2] != NULL) {
	for (Int_t n = 0; n < fStripsRawEv[i][j]; n++) ///Event raw data
	  fHistograms[ii+2]->Fill(nthStrip);
      }

      if (fHistograms[ii+3] != NULL) {
	for (Int_t n=0; n<fStripsEv[i][j]; n++) ///Event data
	  fHistograms[ii+3]->Fill(nthStrip);
      }
      
      if (fHistograms[ii+4] != NULL) {
  	for (Int_t n=0; n<fStripsRawScal[i][j]; n++){ ///v1495 Scaler raw data
	  fHistograms[ii+4]->Fill(nthStrip);
	}
      }
    }
  }
}

void  QwComptonElectronDetector::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  //  SetHistoTreeSave(prefix);
  fTreeArrayIndex = values.size();

  if(prefix=="conf") {
    fIsConfigOnly = kTRUE;
    /// v1495 DAQ info for this slave
    for (Int_t i = 0; i < NPlanes; i++) {
      TString basename = Form("v1495InfoPlane%d",i+1);
      TString valnames = "";
      Double_t* valstart = &(values.back());
      for (Int_t j = 0; j < StripsPerPlane; j++) {
        valnames += TString(":") + Form("p%ds%dMask",i+1,j+1) + "/D";
        values.push_back(0.0);
      }
      valnames.Remove(0,1); // remove first ':' character
      tree->Branch(basename, valstart+1, valnames);
    }

    for (Int_t k = 0; k < NModules; k++) {
      TString basename = Form("slave%d_acTrig",k+1);
      Double_t* valstart = &(values.back());     
      values.push_back(0.0);
      tree->Branch(basename, valstart+1);
    }
    for (Int_t k = 0; k < NModules; k++) {
      TString basename = Form("slave%d_evTrig",k+1);
      Double_t* valstart = &(values.back());     
      values.push_back(0.0);
      tree->Branch(basename, valstart+1);
    }
    for (Int_t k = 0; k < NModules; k++) {
      TString basename = Form("slave%d_minWidth",k+1);
      Double_t* valstart = &(values.back());     
      values.push_back(0.0);
      tree->Branch(basename, valstart+1);
    }
    for (Int_t k = 0; k < NModules; k++) {
      TString basename = Form("slave%d_firmWareRevision",k+1);
      Double_t* valstart = &(values.back());     
      values.push_back(0.0);
      tree->Branch(basename, valstart+1);
    }
    for (Int_t k = 0; k < NModules; k++) {
      TString basename = Form("slave%d_PWTL",k+1);
      Double_t* valstart = &(values.back());     
      values.push_back(0.0);
      tree->Branch(basename, valstart+1);
    }
    for (Int_t k = 0; k < NModules; k++) {
      TString basename = Form("slave%d_PWTL2",k+1);
      Double_t* valstart = &(values.back());     
      values.push_back(0.0);
      tree->Branch(basename, valstart+1);
    }
    for (Int_t k = 0; k < NModules; k++) {
      TString basename = Form("slave%d_HoldOff",k+1);
      Double_t* valstart = &(values.back());     
      values.push_back(0.0);
      tree->Branch(basename, valstart+1);
    }
    for (Int_t k = 0; k < NModules; k++) {
      TString basename = Form("slave%d_PipelineDelay",k+1);
      Double_t* valstart = &(values.back());     
      values.push_back(0.0);
      tree->Branch(basename, valstart+1);
    }
    return;
  }

  for (Int_t i = 0; i < NPlanes; i++) {
    //!!iteration of this loop from '1' instead of '0' is for Qweak run-1 when the first plane was inactive
    /// Note that currently the plane number in these tree-leafs correspond to the C++ counting(from 0)
    /// Event branch for this plane
    TString basename = prefix + Form("p%dRawEv",i+1);
    TString valnames = "";
    Double_t* valstart = &(values.back());
    for (Int_t j = 0; j < StripsPerPlane; j++) {
      valnames += TString(":") + prefix + Form("p%ds%dRawEv",i+1,j+1) + "/D";
      values.push_back(0.0);
    }
    valnames.Remove(0,1); // remove first ':' character
    tree->Branch(basename, valstart+1, valnames);

    /// Accumulator branch for this plane
    basename = prefix + Form("p%dRawAc",i+1);
    valnames = "";
    valstart = &(values.back());
    for (Int_t j = 0; j < StripsPerPlane; j++) {
      valnames += TString(":") + prefix + Form("p%ds%dRawAc",i+1,j+1) + "/D";
      values.push_back(0.0);
    }
    valnames.Remove(0,1); // remove first ':' character
    tree->Branch(basename, valstart+1, valnames);

    /// Accumulator2 branch for this plane
    basename = prefix + Form("p%dRawAc_v2",i+1);
    valnames = "";
    valstart = &(values.back());
    for (Int_t j = 0; j < StripsPerPlane; j++) {
      valnames += TString(":") + prefix + Form("p%ds%dRawAc_v2",i+1,j+1) + "/D";
      values.push_back(0.0);
    }
    valnames.Remove(0,1); // remove first ':' character
    tree->Branch(basename, valstart+1, valnames);

    /// v1495 scaler branch for this plane
    basename = prefix + Form("p%dRawV1495Scaler",i+1);
    valnames = "";
    valstart = &(values.back());
    for (Int_t j = 0; j < StripsPerPlane; j++) {
      valnames += TString(":") + prefix + Form("p%ds%dRawV1495Scaler",i+1,j+1) + "/D";
      values.push_back(0.0);
    }
    valnames.Remove(0,1); // remove first ':' character
    tree->Branch(basename, valstart+1, valnames);
  }
  ///Notice-2: the pattern that for every plane the branch is created for all different types of data before 
  ///...creating the same for the next plane
}

void  QwComptonElectronDetector::FillTreeVector(std::vector<Double_t> &values) const
{
  ///The 'values' needs to be a Double_t due to the inner structure in the hierarchy 
  ///The 'values' should be filled in exactly the same order in which they were created above
  /// ...below we follow the pattern as in Notice-2
  size_t index = fTreeArrayIndex;

  if(fIsConfigOnly) {
    for (UInt_t j = 0; j < fPlane1Mask.size(); j++)
      values[index++] = fPlane1Mask.at(j);/// v1495 plane 1 mask info
    for (UInt_t j = 0; j < fPlane2Mask.size(); j++)
      values[index++] = fPlane2Mask.at(j);/// v1495 plane 2 mask info
    for (UInt_t j = 0; j < fPlane3Mask.size(); j++)
      values[index++] = fPlane3Mask.at(j);/// v1495 plane 3 mask info
    for (UInt_t j = 0; j < fPlane4Mask.size(); j++)
      values[index++] = fPlane4Mask.at(j);/// v1495 plane 4 mask info

    for (UInt_t j = 0; j < fAcTrig.size(); j++) {//!following vectors have same size hence this cheating allowed
      values[index++] = fAcTrig.at(j);/// v1495 accum trigger info
    }
    for (UInt_t j = 0; j < fEvTrig.size(); j++) {//!following vectors have same size hence this cheating allowed
      values[index++] = fEvTrig.at(j);/// v1495 event trigger info
    }
    for (UInt_t j = 0; j < fMinWidth.size(); j++) {//!following vectors have same size hence this cheating allowed
      values[index++] = fMinWidth.at(j);/// v1495 minimum width info
    }    
    for (UInt_t j = 0; j < fFirmwareRevision.size(); j++) {//!following vectors have same size hence this cheating allowed
      values[index++] = fFirmwareRevision.at(j);/// v1495 event trigger info
    }
    for (UInt_t j = 0; j < fPWTL.size(); j++) {//!following vectors have same size hence this cheating allowed
      values[index++] = fPWTL.at(j);/// v1495 event trigger info
    }
    for (UInt_t j = 0; j < fPWTL2.size(); j++) {//!following vectors have same size hence this cheating allowed
      values[index++] = fPWTL2.at(j);/// v1495 event trigger info
    }
    for (UInt_t j = 0; j < fHoldOff.size(); j++) {//!following vectors have same size hence this cheating allowed
      values[index++] = fHoldOff.at(j);/// v1495 event trigger info
    }
    for (UInt_t j = 0; j < fPipelineDelay.size(); j++) {//!following vectors have same size hence this cheating allowed
      values[index++] = fPipelineDelay.at(j);/// v1495 event trigger info
    }
    return;
  }

  for (Int_t i = 0; i < NPlanes; i++) {
    for (Int_t j = 0; j < StripsPerPlane; j++)
      values[index++] = fStripsRawEv[i][j];/// Event Raw
    for (Int_t j = 0; j < StripsPerPlane; j++)
      values[index++] = fStripsRaw[i][j];/// Accum Raw
    for (Int_t j = 0; j < StripsPerPlane; j++)
      values[index++] = fStripsRaw_v2[i][j];/// Accum Raw
    for (Int_t j = 0; j < StripsPerPlane; j++)
      values[index++] = fStripsRawScal[i][j];/// v1495 Scaler Raw
  }
}

/**
 * Construct the tree
 * @param folder Folder in which the tree will be created
 * @param prefix Prefix with information about the type of histogram
 **/
void  QwComptonElectronDetector::ConstructTree(TDirectory *folder, TString &prefix)
{
  TString basename = GetSubsystemName();
  TString vnameh;
  TString vnamet;
  folder->cd();
  fTree = new TTree("ComptonElectron", "Compton Electron Detector");
  fTree->Branch("nevents",&fTree_fNEvents,"nevents/I");
  for (Int_t i=0; i< NPlanes; i++){
    vnameh = Form("Plane%d_Evt",i);
    vnamet = Form("Plane%d_Evt/I",i);
    fComptonElectronVector.push_back(0);
    fTree->Branch(vnameh,&(fComptonElectronVector[i]),vnamet);
  }
  return;
}

/*** Delete the tree */
void  QwComptonElectronDetector::DeleteTree()
{
  delete fTree;
  return;
}

/*** Fill the tree with data */
void  QwComptonElectronDetector::FillTree()
{
  for (Int_t i=0; i< NPlanes; i++)
    fComptonElectronVector[i] = 0;

  fTree_fNEvents = GetNumberOfEvents();
  for (Int_t i=0; i< NPlanes; i++){
    for (Int_t j=0; j<StripsPerPlane; j++){
      if (fStripsEv[i][j] != 0)
	fComptonElectronVector[i] += 1;
    }
  }
  fTree->Fill();

  return;
}


//*****************************************************************
void  QwComptonElectronDetector::PrintValue() const
{//!?what does this do?
  string path = getenv_safe_string("QW_TMP") + "/" + "edet_asym";
  stringstream ss; ss << myrun;
  string str = ss.str();
  path += "_" + str + ".txt";

  static Int_t edet_count = 0;
  edet_count++;
  
  ofstream myfile;
  if(edet_count == 1) {
    QwOut << " " << QwLog::endl;
    QwOut << " Address =  " << path << QwLog::endl;
    QwOut << "Run Number = " << myrun << " Good Events = " << fGoodEventCount << QwLog::endl;    
    QwOut << " " << QwLog::endl;
    myfile.open (path.c_str());
  } 
  else { 
    myfile.open (path.c_str(),ios::app);
  }
  
  Int_t nchan =0;
  for (Int_t i=0; i<NPlanes; i++)
    nchan += fStripsRaw[i].size();
  QwOut << " there were " << nchan << " strips registered" << QwLog::endl;
  
  for (Int_t j=0; j<StripsPerPlane; j++) {
    myfile <<  j << " " << fGoodEventCount*fStripsRaw[0][j] << " " << fGoodEventCount*fStripsRaw[1][j] << " " <<
      fGoodEventCount*fStripsRaw[2][j] << " " << fGoodEventCount*fStripsRaw[3][j] << endl;
  }
  myfile.close();  
  return;
}
/**********
 *The condition that accum_count < 5 is there to ensure that spurious noise 
 *..events do not infiltrate the analyzed data. '5' already implies 5kHz per strip.
 *..since the overall Compton rate is 100kHz (integrated over all strips), with 50
 *..active strips, one can expect an average rate of 2kHz.
 *I should ideally be throwing out the entire event(MPS) if one strip shows spurious data
 *********/

/**********************************************************\
* File: VQwSubsystem.C                                     *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

/*------------------------------------------------------------------------*//*!

 \defgroup QwAnalysis QwAnalysis

 \section myoverview Overview

   Qweak Analysis Framework

   Each subsystem will have a class derived from "VQwSubsystem", and
   will be responsible for decoding of it's own data stream and any
   special event processing required. QwSubsystemArray will handle
   mutiple "VQwSubsystem" objects and one call on the QwSubsystemArray
   will handle all the calls to that method in each subsystem.  Each
   susbsytem will also own the histograms and ntupling functions used
   for its data.

*//*-------------------------------------------------------------------------*/

#include "VQwSubsystem.h"

// Qweak headers
#include "QwLog.h"
#include "QwSubsystemArray.h"
#include "QwParameterFile.h"


Int_t ERROR = -1;


Int_t VQwSubsystem::LoadDetectorMaps(QwParameterFile& file)
{
  Bool_t local_debug = false;

  file.RewindToFileStart();

  while (file.ReadNextLine()) {
    // Trim comments and whitespace
    file.TrimComment('!');
    file.TrimComment('#');
    file.TrimWhitespace();

    // Find key-value pairs
    std::string key, value;
    if (file.HasVariablePair("=", key, value)) {
      if ( value.size() > 0) {

	// If-Ordering Optimization for parity
	// Beamline     1423
	// MainDetector 123
	// Lumi         123
	// Helicity     1
	// Scanner      12
	// Beammod      1
	//              1(6),2(4),3(3),4(1)
	//              map, param, eventcut, geom

	// Map file definition
	if (key == "map" ) {
	  LoadChannelMap(value);
	  //	  fDetectorMapsNames.push_back(value);
	}
	// Parameter file definition
	else if (key == "param" ) {
	  LoadInputParameters(value); 
	  // fDetectorMapsNames.push_back(value);
	}
	// Event cut file definition
	else if (key == "eventcut") {
	  LoadEventCuts(value);
	  // fDetectorMapsNames.push_back(value);
	}
	// Geometry file definition
	else if (key == "geom" ) {
	  LoadGeometryDefinition(value);
	  // fDetectorMapsNames.push_back(value);
	}
        // Crosstalk file definition
        else if (key == "cross" ) {
          LoadCrosstalkDefinition(value);
          // fDetectorMapsNames.push_back(value);
        }
 	//Event type mask
	else if (key == "mask") {
	  SetEventTypeMask(file.GetUInt(value));
	}
      }
      
    } // end of HasVariablePair
  } // end of while 
  
  
  //
  // The above approach that fDetectorMapsNames.push_back(value) in VQwSubsystem doesn't work, because it reads the following...
  //
  // >>> VQwSubsystem::LoadDetectorMaps Subsytem Main Detector uses the following map files : 
  //   --->    1/3 :        qweak_maindet.map
  //   --->    2/3 : qweak_maindet_pedestal.map
  //   --->    3/3 : qweak_maindet_eventcuts.in

  //
  // So, fDetectorMapsNams.push_back will be called LoadChannelMap(), LoadInputParameter(), LoadEventCuts(),
  // and  LoadGeometryDefinition() in each subsystem.
  //
  // >>> VQwSubsystem::LoadDetectorMaps Subsytem Main Detector uses the following map files : 
  //   --->    1/3 : /home/jhlee/QwAnalysis/trunk/Parity/prminput/qweak_maindet.10213-.map
  //   --->    2/3 : /home/jhlee/QwAnalysis/trunk/Parity/prminput/qweak_maindet_pedestal.10229-.map
  //   --->    3/3 : /home/jhlee/QwAnalysis/trunk/Parity/prminput/qweak_maindet_eventcuts.in
  //
  // Friday, March 18 15:32:09 EDT 2011, jhlee

  PrintDetectorMaps(local_debug);

  return 0;
}

/**
 * Get the sibling of this subsystem with the specified name.  If no parents is
 * defined, an error is printed by GetParent().  If no sibling with that name
 * exists, the null pointer is returned.
 *
 * @param name Name of the sibling subsystem
 * @return Pointer to the sibling subsystem
 */
VQwSubsystem* VQwSubsystem::GetSibling(const std::string& name) const
{
  // Get the parent and check for existence
  QwSubsystemArray* parent = GetParent();
  if (parent != 0)
    // Return the subsystem with name in the parent
    return parent->GetSubsystemByName(name);
  else
    return 0; // GetParent() prints error already
}



void VQwSubsystem::ClearAllBankRegistrations()
{
  fBank_IDs.clear();
  fROC_IDs.clear();
  fCurrentROC_ID    = kNullROCID;
  fCurrentBank_ID   = kNullBankID;
}

Int_t VQwSubsystem::GetSubbankIndex(const ROCID_t roc_id, const BankID_t bank_id) const
{
  //  Bool_t lDEBUG=kTRUE;
  Int_t index = -1;
  Int_t roc_index = FindIndex(fROC_IDs, roc_id);//will return the vector index for the Roc from the vector fROC_IDs.
  // std::cout << "------------- roc_index" << roc_index <<std::endl;
  if (roc_index>=0){
    index = FindIndex(fBank_IDs[roc_index],bank_id);
    // std::cout << " Find Index " << index
    // 	      << " roc index " << roc_index
    // 	      << " index " << index <<std::endl;

    if (index>=0){
      for (Int_t i=0; i<roc_index; i++){
	index += (fBank_IDs[i].size());
	// std::cout << " i " << i
	// 	  << " fBank_IDs[i].size() " << fBank_IDs[i].size()
	// 	  << " index " << index
	// 	  << std::endl;
      }
    }
  }
  // std::cout << "return:index " << index << std::endl;
  return index;
}

Int_t VQwSubsystem::RegisterROCNumber(const ROCID_t roc_id, const BankID_t bank_id)
{
  Int_t stat      = 0;
  Int_t roc_index = 0;
  roc_index = FindIndex(fROC_IDs, roc_id);

  //will return the vector index for this roc_id on the vector fROC_IDs
  if (roc_index==-1){
    fROC_IDs.push_back(roc_id); // new ROC number is added.
    roc_index = (fROC_IDs.size() - 1);
    std::vector<BankID_t> tmpvec(1,bank_id);
    fBank_IDs.push_back(tmpvec);
    fMarkerWords.resize(fROC_IDs.size());
    fMarkerWords.at(roc_index).resize(1);
  } else {
    Int_t bank_index = FindIndex(fBank_IDs[roc_index],bank_id);
    if (bank_index==-1) { // if the bank_id is not registered then register it.
      fBank_IDs[roc_index].push_back(bank_id);
      fMarkerWords.at(roc_index).resize(fBank_IDs.at(roc_index).size());
    } else {
      //  This subbank in this ROC has already been registered!
      QwError << std::hex << "VQwSubsystem::RegisterROCNumber:  "
              << "This subbank (0x" << bank_id << ") "
              << "in this ROC (0x" << roc_id << ") "
              << "has already been registered!"
              << std::dec << QwLog::endl;
      stat = ERROR;
    }
  }
  if (stat!=-1){
    fCurrentROC_ID    = roc_id;
    fCurrentBank_ID   = bank_id;
  } else {
    fCurrentROC_ID    = kNullROCID;
    fCurrentBank_ID   = kNullBankID;
  }
  return stat;
}

Int_t VQwSubsystem::RegisterSubbank(const BankID_t bank_id)
{
  Int_t stat = 0;
  if (fCurrentROC_ID != kNullROCID){
    stat = RegisterROCNumber(fCurrentROC_ID, bank_id);
    fCurrentBank_ID   = bank_id;
  } else {
    //  There is not a ROC registered yet!
    QwError << std::hex << "VQwSubsystem::RegisterSubbank:  "
            << "This subbank (" << bank_id << ") "
            << "does not have an associated ROC!  "
            << "Add a 'ROC=#' line to the map file."
            << std::dec << QwLog::endl;
    stat = ERROR;
    fCurrentROC_ID  = kNullROCID;
    fCurrentBank_ID = kNullBankID;
  }
  return stat;
}


Int_t VQwSubsystem::RegisterMarkerWord(const UInt_t markerword)
{
  static BankID_t bankIDmask = 0xffffffff;
  Int_t stat = 0;
  if (fCurrentROC_ID != kNullROCID){
    Int_t roc_index = FindIndex(fROC_IDs, fCurrentROC_ID);
    Int_t bank_index = FindIndex(fBank_IDs[roc_index],(fCurrentBank_ID&bankIDmask));
    fMarkerWords.at(roc_index).at(bank_index).push_back(markerword);
    BankID_t tmpbank = markerword;
    tmpbank = ((tmpbank)<<32) + (fCurrentBank_ID&bankIDmask);
    RegisterSubbank(tmpbank);
  } else {
    //  There is not a ROC registered yet!
    QwError << std::hex << "VQwSubsystem::RegisterSubbank:  "
            << "This Marker word (" << markerword << ") "
            << "does not have an associated ROC!  "
            << "Add a 'ROC=#' line to the map file."
            << std::dec << QwLog::endl;
    stat = ERROR;
    fCurrentROC_ID  = kNullROCID;
    fCurrentBank_ID = kNullBankID;
  }
  return stat;
}

void VQwSubsystem::RegisterRocBankMarker(QwParameterFile &mapstr){
  UInt_t value = 0;
  if (mapstr.PopValue("roc",value)) {
    RegisterROCNumber(value,0);
  }
  if (mapstr.PopValue("bank",value)) {
    RegisterSubbank(value);
  }
  if (mapstr.PopValue("markerword",value)) {
    RegisterMarkerWord(value);
  }
}

void VQwSubsystem::PrintInfo() const
{
  std::cout << "Name of this subsystem: " << fSystemName << std::endl;
  for (size_t roc_index = 0; roc_index < fROC_IDs.size(); roc_index++) {
    std::cout << "ROC" << std::dec << fROC_IDs[roc_index] << ": ";
    for (size_t bank_index = 0; bank_index < fBank_IDs[roc_index].size(); bank_index++)
      std::cout << std::hex << "0x" << fBank_IDs[roc_index][bank_index] << " ";
    std::cout << std::dec << std::endl;
  }
  std::cout << "in array " << std::hex << GetParent() << std::dec << std::endl;
}


VQwSubsystem& VQwSubsystem::operator=(VQwSubsystem *value)
{
  this->fIsDataLoaded = value->fIsDataLoaded;
  return *this;
}



std::vector<TString> VQwSubsystem::GetParamFileNameList()
{
  return fDetectorMapsNames;
}



std::map<TString, TString> VQwSubsystem::GetDetectorMaps()
{
  return fDetectorMaps;
}



void VQwSubsystem::PrintDetectorMaps(Bool_t status) const
{
  Bool_t local_debug = false;
  if (status) {
    QwMessage << " >>> VQwSubsystem::LoadDetectorMaps Subsystem " << fSystemName
              << " uses the following map files:" << QwLog::endl;

    Int_t index = 0;
    size_t total = fDetectorMaps.size();

    if (total != 0) {

      for (std::map<TString,TString>::const_iterator ii = fDetectorMaps.begin();
           ii != fDetectorMaps.end(); ++ii) {

        index++;
        TString name = (*ii).first;
        TString all  = (*ii).second;
        QwMessage << "   ---> " << index << "/" << total << ": " << name << QwLog::endl;
        if (local_debug)
          QwMessage << "   " << all << QwLog::endl;

      }

    } else {
      QwMessage << "   ---> No map files" << QwLog::endl;
    }
  }
}

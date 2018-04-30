/**
 * \file  QwMollerDetector.cc
 *
 * \brief Implementation of the analysis of Moller data
 *
 * \author  Andrew Kubera
 * \date  2010-06-07
 * \ingroup QwMoller
 *
 *
 */

#include "QwMollerDetector.h"

// System headers
#include <stdexcept>
#include <iomanip>

// Qweak headers
#include "QwLog.h"
#include "QwParameterFile.h"
#include "QwScaler_Channel.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwMollerDetector);

/**
 * Load the channel map
 * @param mapfile Map file
 * @return Zero if successful
 */

Int_t QwMollerDetector::LoadChannelMap(TString mapfile)
{
  TString varname, varvalue;
  TString modtype, dettype, name, keyword;
  Int_t modnum, channum;
  Int_t currentrocread = 0;
  Int_t currentbankread = 0;
  Int_t wordsofar = 0;
  Int_t currentsubbankindex = -1;
  
  QwParameterFile mapstr(mapfile.Data());  // Open the file
  fDetectorMapsNames.push_back(mapstr.GetParamFilename());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing whitespace (spaces or tabs).
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
        if(currentsubbankindex != GetSubbankIndex(currentrocread,currentbankread)){
          currentsubbankindex = GetSubbankIndex(currentrocread,currentbankread);
        }
      }
    } else {
      Bool_t lineok = kTRUE;

      //  Break this line into tokens to process it.
      modtype   = mapstr.GetTypedNextToken<TString>();
      modnum    = mapstr.GetTypedNextToken<Int_t>();
      channum   = mapstr.GetTypedNextToken<Int_t>();
      dettype   = mapstr.GetTypedNextToken<TString>();
      name      = mapstr.GetTypedNextToken<TString>();
      dettype.ToLower();
      name.ToLower();

      QwMollerChannelID newChannelID;
      newChannelID.fModuleType    = modtype;
      newChannelID.fModuleNumber  = modnum;
      newChannelID.fChannelName   = name;
      newChannelID.fDetectorType  = dettype;
      newChannelID.fChannelNumber = channum;
      newChannelID.fWordInSubbank = wordsofar;
       
      if (modtype == "SIS3801"){
        wordsofar += 1;
      }else if(modtype == "SIS7200"){
        wordsofar += 1;
      }else if(modtype == "VQWK"){
        wordsofar += 6;
      }else {
        std::cerr << "Unknown module type: " << modtype << ". Skipping channel "  << name << '.' << std::endl;
        lineok = kFALSE;
      }

      if (name.Length() == 0){
        lineok = kFALSE;
      }

//    add new modules until current number (modnum) is reached 
      std::size_t chan_size;
      chan_size = fSTR7200_Channel.size();
      while ((Int_t) chan_size <= modnum) {
        std::vector<QwSTR7200_Channel> new_module;
        fSTR7200_Channel.push_back(new_module);
      }

      //change this line if names are supposed to be exclusive, as of now, 
      newChannelID.fIndex = -1; // GetChannelIndex(name, modnum);

      if (newChannelID.fIndex == -1 && lineok){
        QwSTR7200_Channel localSTR7200_Channel(newChannelID.fChannelName);
        fSTR7200_Channel[modnum].push_back(localSTR7200_Channel);
//        fSTR7200_Channel[modnum][fMollerChannelID.size() - 1].SetDefaultSampleSize(fSample_size);
        newChannelID.fIndex = fSTR7200_Channel[modnum].size() - 1;
        //std::cout << name << ':' << newChannelID.fIndex << ':' << wordsofar << '\n';
      }

      //  Push a new record into the element array
      if(lineok){
        fMollerChannelID.push_back(newChannelID);
      }
    }
  } // end looping over parameter file

  fPrevious_STR7200_Channel = fSTR7200_Channel;

//  for (size_t i = 0; i < fMollerChannelID.size(); i++){
//     std::cout << i << ": " << fMollerChannelID[i].fChannelName << ' ' << fMollerChannelID[i].fChannelNumber << ' ' << fMollerChannelID[i].fIndex << std::endl;
//   }

  return 0;
}

void QwMollerDetector::ProcessOptions(QwOptions &){}
Int_t QwMollerDetector::LoadInputParameters(TString){ return 0;}
void QwMollerDetector::ClearEventData(){}


Int_t QwMollerDetector::ProcessConfigurationBuffer(UInt_t, UInt_t, UInt_t*, UInt_t){
  return 0;
}

Int_t QwMollerDetector::ProcessConfigurationBuffer(UInt_t ev_type, UInt_t, UInt_t, UInt_t*, UInt_t)
{
 return 0;
}

Int_t QwMollerDetector::ProcessEvBuffer(UInt_t ev_type, UInt_t roc_id, UInt_t bank_id, UInt_t *buffer, UInt_t num_words)
{
  return ProcessEvBuffer(roc_id, bank_id, buffer, num_words);
}

Int_t QwMollerDetector::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t *buffer, UInt_t num_words)
{
  Int_t index = 0; // GetSubbankIndex(roc_id, bank_id);

  if (index >= 0 && num_words > 0){
    //we want to process this ROC

    //Loop through all the channels as defined in map file.. should be 96 in standard 3 module, 32 channel Moller configuration
    for(size_t i = 0; i < fMollerChannelID.size(); i++){

      Int_t wordsLeft = num_words - fMollerChannelID[i].fWordInSubbank;

//       std::cout << std::dec << fMollerChannelID[i].fChannelName << ' ' << fMollerChannelID[i].fIndex << " in module " << fMollerChannelID[i].fModuleNumber << " has data " << std::hex << buffer[fMollerChannelID[i].fWordInSubbank] << std::dec << std::endl;

      UInt_t mod_index = fMollerChannelID[i].fModuleNumber;
      Int_t chan_index = fMollerChannelID[i].fIndex;
//      char *junk = new char[2];
      if (mod_index < fSTR7200_Channel.size()) {
        //mod_index should be less than 3, next_module is reference to vector where the Channel should be located
     //   std::cout << std::hex << buffer[fMollerChannelID[i].fWordInSubbank] << '\n';
        //std::cin.getline(junk, 1);
//         std::cout << '(' << mod_index << ',' << chan_index << ')' << buffer[fMollerChannelID[i].fWordInSubbank] - fSTR7200_Channel[mod_index][chan_index].GetValue() << ' ' << std::endl;

        fSTR7200_Channel[mod_index][chan_index].ProcessEvBuffer(&(buffer[fMollerChannelID[i].fWordInSubbank]), wordsLeft);
      } else {
        std::cout << "Problem reading buffer, incorrect structure of map file?" << std::endl;
      }

    }
  }
//  std::cout << fSTR7200_Channel[2][0].GetValue() << std::endl;
//  print();
  return 0;
}



void QwMollerDetector::ProcessEvent()
{
  for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for(size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      fSTR7200_Channel[i][j].ProcessEvent();
      QwSTR7200_Channel tempscaler(fSTR7200_Channel[i][j]);
      tempscaler = fSTR7200_Channel[i][j];
      fSTR7200_Channel[i][j] -= fPrevious_STR7200_Channel[i][j];
      fPrevious_STR7200_Channel[i][j] = tempscaler;
      // Store a temproary copy of this channel's raw value as a scaler channel
      // Subtract the corresponding fPrevious_STR7200_Channel from this scaler channel
      // Put the temprary copy into the fPrevious_STR7200_Channel
    }
  }
}


void QwMollerDetector::ConstructHistograms(TDirectory* folder, TString& prefix){
  for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for(size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      fSTR7200_Channel[i][j].ConstructHistograms(folder, prefix);
    }
  }
}

void QwMollerDetector::FillHistograms(){
  for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for(size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      fSTR7200_Channel[i][j].FillHistograms();
    }
  }
}

void QwMollerDetector::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values){
  for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for(size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      fSTR7200_Channel[i][j].ConstructBranchAndVector(tree, prefix, values);
    }
  }
}

void QwMollerDetector::FillTreeVector(std::vector<Double_t> &values) const {
  for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for(size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      fSTR7200_Channel[i][j].FillTreeVector(values);
    }
  }
}

VQwSubsystem&  QwMollerDetector::operator=(VQwSubsystem *value){
  // std::cout << "QwMollerDetector assignment (operator=)" << std::endl;
  if(Compare(value)){
    //VQwSubsystem::operator=(value);
    QwMollerDetector* input = dynamic_cast<QwMollerDetector *> (value);
    for(size_t i = 0; i < input->fSTR7200_Channel.size(); i++){
      for(size_t j = 0; j < input->fSTR7200_Channel[i].size(); j++){
        this->fSTR7200_Channel[i][j] = input->fSTR7200_Channel[i][j];
      }
    }
  }
  return *this; 
}

VQwSubsystem&  QwMollerDetector::operator+=(VQwSubsystem *value){
  //std::cout << "QwMollerDetector addition assignment (operator+=)" << std::endl;
  if(Compare(value)){
    QwMollerDetector* input = dynamic_cast<QwMollerDetector *> (value);
    for(size_t i = 0; i < input->fSTR7200_Channel.size(); i++){
      for(size_t j = 0; j < input->fSTR7200_Channel[i].size(); j++){
        this->fSTR7200_Channel[i][j] += input->fSTR7200_Channel[i][j];
        //std::cout << "+= " << this->fSTR7200_Channel[i][j].GetValue() << std::endl;
      }
    }
  }
  return *this;
}

VQwSubsystem&  QwMollerDetector::operator-=(VQwSubsystem *value){
  //std::cout << "QwMollerDetector subtraction assignment (operator-=)" << std::endl;
  if(Compare(value)){
    QwMollerDetector* input = dynamic_cast<QwMollerDetector *> (value);
    for(size_t i = 0; i < input->fSTR7200_Channel.size(); i++){
      for(size_t j = 0; j < input->fSTR7200_Channel[i].size(); j++){
        this->fSTR7200_Channel[i][j] -= input->fSTR7200_Channel[i][j];
      }
    }
  }
  return *this;
}

void QwMollerDetector::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2){
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this += value2;
  }
}

void QwMollerDetector::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2){
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this -= value2;
  }
}

void QwMollerDetector::Ratio(VQwSubsystem  *value1, VQwSubsystem  *value2){
  if (Compare(value1) && Compare(value2)) {
    QwMollerDetector* v1 = dynamic_cast<QwMollerDetector *> (value1);
    QwMollerDetector* v2 = dynamic_cast<QwMollerDetector *> (value2);

    for(size_t i = 0; i < v1->fSTR7200_Channel.size(); i++){
      for(size_t j = 0; j < v1->fSTR7200_Channel[i].size(); j++){
        fSTR7200_Channel[i][j].Ratio(v1->fSTR7200_Channel[i][j],v2->fSTR7200_Channel[i][j]);
      }
    }
  }
  return;
}

void QwMollerDetector::Scale(Double_t factor){
  for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for(size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      fSTR7200_Channel[i][j].Scale(factor);
    }
  }
}

void QwMollerDetector::AccumulateRunningSum(VQwSubsystem* value){
  if (Compare(value)) {
    QwMollerDetector* v = dynamic_cast<QwMollerDetector*>(value);

    for (size_t i = 0; i < fSTR7200_Channel.size(); i++){
      for (size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
        fSTR7200_Channel[i][j].AccumulateRunningSum(v->fSTR7200_Channel[i][j]);
      }
    }

  }
}

void QwMollerDetector::CalculateRunningAverage(){
  for (size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for (size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      fSTR7200_Channel[i][j].CalculateRunningAverage();
    }
  }
}

Int_t QwMollerDetector::LoadEventCuts(TString filename){return 0;}

Bool_t QwMollerDetector::ApplySingleEventCuts(){
  std::cout << "QwMoller::ApplySingleEventCuts() ";
  Bool_t test = kTRUE, test_1 = kTRUE;

  for (size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for (size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      test_1 = fSTR7200_Channel[i][j].ApplySingleEventCuts();
      test &= test_1;
      if(!test_1 && bDEBUG){
        std::cout << "***** QwMoller::SingleEventCuts()->Channel[" << i << "][" << j << "]:" << fSTR7200_Channel[i][j].GetElementName() << std::endl;
      }
    }
  }
  return test;
}

void QwMollerDetector::PrintErrorCounters() const{
  std::cout << "***************QwMoller Error Summary****************" << std::endl;
  for (size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for (size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      fSTR7200_Channel[i][j].PrintErrorCounters();
    }
  }
  std::cout << "total failed events: " << fQwMollerErrorCount << std::endl;
  std::cout << "************End QwMoller Error Summary***************" << std::endl;
}

UInt_t QwMollerDetector::GetEventcutErrorFlag(){
  return 0;
}

float* QwMollerDetector::GetRawChannelArray(){
  size_t len = 0;
  for (size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for (size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      len++;
    }
  }
  float *result = new float[len];
  
  //float result[96];

  int n = 0;
  for (size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for (size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
      result[n+j] = fSTR7200_Channel[i][j].GetValue();
    }
    n=fSTR7200_Channel[i].size();
  }
 
  return result;
}

Int_t QwMollerDetector::GetChannelIndex(TString channelName, UInt_t module_number)
{
  Bool_t ldebug=kFALSE;

  channelName.ToLower();

  if (ldebug){
    std::cout << "QwMollerDetector::GetDetectorIndex" << std::endl;
    std::cout << "module_number: " << module_number << " name=" << channelName << std::endl;
  }

  Int_t result = -1;
  for(size_t i = 0; i < fMollerChannelID.size(); i++){
    QwMollerChannelID *nextChannel = &fMollerChannelID[i];
    //std::cout << ' '<< nextChannel->fChannelName << '=' << channelName << ':' << (nextChannel->fChannelName == channelName) << std::endl;
    if (nextChannel->fChannelName == channelName && nextChannel->fModuleNumber == module_number){
      result = nextChannel->fIndex;
      break;
    }
  }

  return result;
}


Bool_t QwMollerDetector::Compare(VQwSubsystem *source){
  //std::cout << "Beginning QwMollerDetector::Compare" << std::endl;
  
  if (source == 0) return kFALSE;

  Bool_t result = kTRUE;
  if(typeid(*source) != typeid(*this)){
    result = kFALSE;
    std::cout << " Type mismatch! This is bypassed for now but should be fixed eventually." << std::endl;
  } else { //same type, test for # of modules
    QwMollerDetector* input = dynamic_cast<QwMollerDetector*>(source);
    if(input->fSTR7200_Channel.size() != fSTR7200_Channel.size()){
      result = kFALSE;
      std::cout << " Not the same number of Modules" << std::endl;
    }else { //same # modules, loop through and make sure each one has same amount of channels
      for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
        if(input->fSTR7200_Channel[i].size() != fSTR7200_Channel[i].size()){
          result = kFALSE;
          std::cout << " Different number of channels in module " << i << std::endl;
        }
      }
    }
  }
  return result;
}

void QwMollerDetector::print(){
  std::cout << " " << fSTR7200_Channel.size() << std::endl;
  UInt_t max = 0;
  for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
    UInt_t next = fSTR7200_Channel[i].size();
    if (next > max){
      max = next;
    }
  }

  for(size_t i = 0; i < max; i++){
    std::cout << fMollerChannelID[i].fChannelName << ":\t" << (fMollerChannelID[i].fChannelName.Length() < 14 ? "\t" : "");
    for(size_t j = 0; j < fSTR7200_Channel.size(); j++){

      if ( i < fSTR7200_Channel[j].size()){
        std::cout << "0x" << std::hex << (int)fSTR7200_Channel[j][i].GetValue() << std::dec;
      } else {
        std::cout << "            ";
      }
      std::cout << "\t\t";
    }
    std::cout << std::endl;
  }

}

void QwMollerDetector::PrintValue() const {
  for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
    for(size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
         fSTR7200_Channel[i][j].PrintValue();
    }
  }
}

/*
 * QwAlarmHandler.cc
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 *
 *  Last Modified: August 1, 2018 1:45 PM
 */

#include "QwAlarmHandler.h"

// Qweak headers
#include "VQwDataElement.h"
#include "QwVQWK_Channel.h"
#include "QwParameterFile.h"
#include "QwHelicityPattern.h"

#define MYSQLPP_SSQLS_NO_STATICS
#ifdef __USE_DATABASE__
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__


// Register this handler with the factory
RegisterHandlerFactory(QwAlarmHandler);


/// \brief Constructor with name
QwAlarmHandler::QwAlarmHandler(const TString& name):VQwDataHandler(name)
{
  ParseSeparator = "_";
  //fKeepRunningSum = kTRUE;
}

QwAlarmHandler::QwAlarmHandler(const QwAlarmHandler &source):VQwDataHandler(source)
{
}

//  Just use the base class version for now....

void QwAlarmHandler::ParseConfigFile(QwParameterFile& file)
{
  VQwDataHandler::ParseConfigFile(file);
  //name        = MyAlarmHandler
  //priority    = 20
  //map         = prex_alarm_handler.map
  file.PopValue("alarm-output-file",fAlarmOutputFile);
  file.PopValue("alarm-loop-N-update",fAlarmNupdate);
  file.PopValue("alarm-active",fAlarmActive);
  // Check for and process key-value pairs
  //file.PopValue("new-key-word",fsomething); // These need = signs in map files
}

std::pair<std::string,std::string> QwAlarmHandler::ParseAlarmMapVariable(const std::string& variable, char delim) {
  std::pair<std::string,std::string> type_name;
  size_t len = variable.length();
  size_t pos1 = variable.find_first_of(delim);
  size_t pos2 = variable.find_first_not_of(delim,pos1);
  if (pos1 == string::npos) {
    type_name.first  = "NULL";
    type_name.second = "NULL";
  } else {
    string type = variable.substr(0,pos1);
    string name = variable.substr(pos2,len-pos2);
    type_name.first = type;
    type_name.second = name;
  }
  return type_name;
}

//******************************************************************************************************************************************************

/** Load the channel map
 *
 * @param mapfile Filename of map file
 * @return Zero when success
 */
Int_t QwAlarmHandler::LoadChannelMap(const std::string& mapfile)
{
  // Open the file
  QwParameterFile map(mapfile);

  // Read the sections of dependent variables
  std::pair<EQwHandleType,std::string> type_name;
  std::pair<std::string,std::string> tmpPair;

  // Add independent variables and sensitivities
  while (map.ReadNextLine()) {
    // Throw away comments, whitespace, empty lines
    map.TrimComment();
    map.TrimWhitespace();
    if (map.LineIsEmpty()) continue;
    // Get first token: label (dv or iv), second token is the name like "asym_blah"
//  Type, Channel, Ana, tree, channel, highhigh, high, low, lowlow, ringLength, pat-tolerance
//  Kind=Main-Det Chan=usr Tree=mul Channel=asym_usr HighHigh=1000 High=100 Low=-100 LowLow=-1000 Ring-Length=200 Tolerance=2
    alarmObject tmpAlarmObject;
    // Do while tmpVal = map.GetNextToken parse
    // Do map[tmpVal.type] = tmpVal.value
    std::string tmpToken = map.GetNextToken(" ");
    while ( tmpToken != "" ) {
      tmpPair = ParseAlarmMapVariable(tmpToken,'='); // This refers to the object under scrutiny, finds type of object - asym, yield, diff, etc. Check on yields FIXME
      if (tmpPair.first=="Channel") {
        type_name = ParseHandledVariable(tmpPair.second);
        tmpAlarmObject.alarmParameterMapStr["Analysis"] = tmpPair.second;
        tmpAlarmObject.analysisType = type_name.first;
        tmpAlarmObject.alarmParameterMapStr["Channel-Name"] = type_name.second;
        tmpAlarmObject.alarmParameterMapStr["Type-Name"] = tmpPair.second;
      }
      else if (tmpPair.first == "Kind") {
        tmpAlarmObject.alarmParameterMapStr[tmpPair.first] = tmpPair.second;
      }
      else if (tmpPair.first == "Chan") {
        tmpAlarmObject.alarmParameterMapStr[tmpPair.first] = tmpPair.second;
      }
      else if (tmpPair.first == "Error-Code") {
        tmpAlarmObject.alarmParameterMapStr[tmpPair.first] = tmpPair.second;
      }
      else {
        tmpAlarmObject.alarmParameterMap[tmpPair.first] = std::stod(tmpPair.second);
      }
      tmpToken = map.GetNextToken(" ");
    }
    if (!tmpAlarmObject.alarmParameterMap.count("Ring-Length")) {
      tmpAlarmObject.alarmParameterMap["Ring-Length"] = 1e9;
    }
    if (!tmpAlarmObject.alarmParameterMap.count("Tolerance")) {
      tmpAlarmObject.alarmParameterMap["Tolerance"] = 0;
    }

    /* tmpAlarmObject.type         = map.GetNextToken(" ");
    tmpAlarmObject.channel      = map.GetNextToken(" ");
    tmpAlarmObject.ana          = map.GetNextToken(" ");
    tmpAlarmObject.tree         = map.GetNextToken(" ");
    std::string localAna_token = map.GetNextToken(" ");
    // Parse current token into independent variable type and name
    type_name = ParseHandledVariable(localAna_token); // This refers to the object under scrutiny, finds type of object - asym, yield, diff, etc. Check on yields FIXME
    tmpAlarmObject.analysisType = type_name.first;
    tmpAlarmObject.analysisName = type_name.second;
    tmpAlarmObject.highHigh     = map.GetNextToken(" ");
    tmpAlarmObject.high         = map.GetNextToken(" ");
    tmpAlarmObject.low          = map.GetNextToken(" ");
    tmpAlarmObject.lowLow       = map.GetNextToken(" ");
    tmpAlarmObject.ringLength   = map.GetNextToken(" "); 
    tmpAlarmObject.tolerance    = map.GetNextToken(" "); // This is all hardcoded.... how to do with keywords? FIXME
    */
    // Default Initializations
    tmpAlarmObject.alarmStatus  = "OK";
    tmpAlarmObject.Nviolated    = 0;
    tmpAlarmObject.NsinceLastViolation = 1e9;
    tmpAlarmObject.value = NULL;
    tmpAlarmObject.eventcutErrorFlag = NULL;
    fAlarmObjectList.push_back(tmpAlarmObject);

    /*else if (primary_token == "treetype") {
      QwMessage << "Tree Type read, ignoring." << QwLog::endl;
    }
    else {
   	  QwError << "Function LoadChannelMap in QwCorrelator.cc read in invalid primary_token." << QwLog::endl;
    }*/
  }
  return 0;
}

/** Connect to the dependent and independent channels
 *
 * @param asym Asymmetry event structure
 * @param diff Difference event structure
 * @return Zero on success
 */
Int_t QwAlarmHandler::ConnectChannels(
    QwSubsystemArrayParity& yield,
    QwSubsystemArrayParity& asym,
    QwSubsystemArrayParity& diff)
{
  /// Fill vector of pointers to the relevant data elements
  for (size_t anaInd = 0; anaInd < fAlarmObjectList.size(); anaInd++) {
    // Get the dependent variables
    if (fAlarmObjectList.at(anaInd).analysisType==kHandleTypeMps || fAlarmObjectList.at(anaInd).alarmParameterMapStr.count("Channel-Name") == 0){
      //  Quietly ignore the MPS type when we're connecting the asym & diff
      continue;
    }
    const VQwHardwareChannel* ana_ptr = NULL;
    const UInt_t* eventcut = NULL;
    switch (fAlarmObjectList.at(anaInd).analysisType) {
      case kHandleTypeYield:
        SetEventcutErrorFlagPointer(yield.GetEventcutErrorFlagPointer());
        ana_ptr = yield.ReturnInternalValue(fAlarmObjectList.at(anaInd).alarmParameterMapStr.at("Channel-Name"));
        eventcut = yield.GetEventcutErrorFlagPointer();
        break;
      case kHandleTypeAsym:
        SetEventcutErrorFlagPointer(asym.GetEventcutErrorFlagPointer());
        ana_ptr = asym.ReturnInternalValue(fAlarmObjectList.at(anaInd).alarmParameterMapStr.at("Channel-Name"));
        eventcut = asym.GetEventcutErrorFlagPointer();
        break;
      case kHandleTypeDiff:
        SetEventcutErrorFlagPointer(diff.GetEventcutErrorFlagPointer());
        ana_ptr = diff.ReturnInternalValue(fAlarmObjectList.at(anaInd).alarmParameterMapStr.at("Channel-Name"));
        eventcut = diff.GetEventcutErrorFlagPointer();
        break;
      default:
        QwWarning << "Independent variable for AlarmHandler has unknown type."
          << QwLog::endl;
        break;
    }
    if (ana_ptr != NULL) {
      fAlarmObjectList.at(anaInd).value = ana_ptr;
      //QwError << "ana_ptr = " << ana_ptr <<QwLog::endl;
      //QwError << "fAlarmObjectList.at(" << anaInd << ").value = " <<  fAlarmObjectList.at(anaInd).value <<QwLog::endl;
      fAlarmObjectList.at(anaInd).eventcutErrorFlag = eventcut;
    } else {
      fAlarmObjectList.at(anaInd).value = NULL;
      //QwWarning << "Independent variable " << fAlarmObjectList.at(anaInd).alarmParameterMapStr.at("Channel-Name") << " missing in alarm map "
      //  << QwLog::endl;
    }
  }
  return 0;
}

/** Connect to the dependent and independent channels
 *
 * @param event Helicity event structure
 * @return Zero on success
 */
/*Int_t QwAlarmHandler::ConnectChannels(QwSubsystemArrayParity& event)
{
  // Return if correction is not enabled

  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fAlarmObjectList.size(); dv++) {
    // Get the dependent variables

    const VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    QwVQWK_Channel* vqwk = NULL;
    string name = " s";
    string calc = "calc_";

    if (fAnalysisType.at(dv)==kHandleTypeAsym || fAnalysisType.at(dv)==kHandleTypeDiff){
      //  Quietly skip the asymmetry or difference types.
      continue;
    } else if(fAnalysisType.at(dv) != kHandleTypeMps){
      QwWarning << "QwAlarmHandler::ConnectChannels(QwSubsystemArrayParity& event):  Dependent variable, "
                << fAnalysisName.at(dv)
	              << ", for MPS alarm handler does not have MPS type, type=="
	              << fAnalysisType.at(dv) << "."<< QwLog::endl;
      continue;
    } else {
      if(fAnalysisName.at(dv).at(0) == '@' ){
        name = fAnalysisName.at(dv).substr(1,fAnalysisName.at(dv).length());
        new_vqwk = new QwVQWK_Channel(name, VQwDataElement::kDerived);
      } else {
        dv_ptr = event.RequestExternalPointer(fAnalysisName.at(dv));

        vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
        name = vqwk.GetElementName().Data();
        new_vqwk = new QwVQWK_Channel(*vqwk, VQwDataElement::kDerived);
        new_vqwk.SetElementName(name);
      }
      new_vqwk.SetSubsystemName(fName);
    }

    // alias
    if(new_vqwk==NULL){
      QwWarning << "Dependent variable " << fAnalysisName.at(dv) << " could not be found, "
                << "or is not a VQWK channel." << QwLog::endl;
      continue; 
    } else {
      //QwMessage << "dv: " << new_vqwk.GetElementName() << QwLog::endl;
      // pair creation
      fAnalysisType.push_back(fAnalysisType.at(dv));
      fDependentVar.push_back(vqwk);
      fOutputVar.push_back(new_vqwk);
      //fDependentVar.push_back(std::make_pair(vqwk, new_vqwk));
    }
  }
  return 0; // FIXME this won't work, and the pointers are all wrong anyway... 
}*/

void QwAlarmHandler::ProcessData() {
 // for (size_t i = 0; i < fDependentVar.size(); ++i) {
 //   *(fOutputVar.at(i)) = *(fDependentVar[i]);
 // }
 // for (size_t i = 0; i < fDependentValues.size(); ++i) {
 //   fValue.at(i) = fDependentValues[i];
 // }
  fCounter++;
  if (fAlarmActive) {
    CheckAlarms();
    if (fCounter % fAlarmNupdate == 0) {
      UpdateAlarmFile();
    }
  }
}  // FIXME do I even need a process data method? Probably not

/* Want to define new methods that will evaluate alarm status of config file variables (and combined variables -> correlations)
 * Per multiplet calculate the asym, diffs, and yields, and compare these results to user set defined high or low values (if defined)
 * Per multiplet check device_error_code and check eventCuts failed due to device in question and compare with tolerances defined by user
 * Per failed low/high limit and per failed error_code and eventCut increment fail count++ and check if count > user tolerance
 * If > tolerance then update alarm status with status (degree of failure, or Not OK status) and print to alarm.csv file the limits and value
 *    for the alarm handler GUI to pick up and parse
 * Else if status changes to OK then print updated status to file too
 * Else if no change then don't change the text file
 *
 * Probably need to interface with event cuts definitions in order to access their number-coding
 * Probably need to define either some sort of event ring or counts decay procedure (+1 for bad, -1 for good...) to allow ignoring old failed events
 * Probably need to define my own map nomenclature to hold stuff like what alarm.csv needs
 *   Parameter List:
 *
 *   Japan Main Detectors,usr,asym_mean,Alarm Status,OK/saturated/high/low/nsamples/highhigh/lowlow/invalid/trip/eventcuter (I'm the cause)
 *   Japan Main Detectors,usr,asym_mean,Analysis,mean/rms/eventcuts/device_error_code
 *   Japan Main Detectors,usr,asym_mean,High,10000    
 *   Japan Main Detectors,usr,asym_mean,Value,500
 *   Japan Main Detectors,usr,asym_mean,Low,50
 *   Japan Main Detectors,usr,asym_mean,Alarm Type,Japan
 *   Japan Main Detectors,usr,asym_mean,Variable Name,yield_usr
 *   Japan Main Detectors,usr,asym_mean,Tree,mul
 *   Japan Main Detectors,usr,asym_mean,Pattern Tolerance,2
 *
 *   Type, Channel, Ana, tree, channel, highhigh, high, low, lowlow, pat tol
 *   J-m-d, usr, asym_mean, mul, usr, 1000, 100, -100, -1000, 2
 * 
 *
 *  Make sure I update "canContain" method in /analysis/src/qwdatahandlerarray to include alarmhandler
 */
  /* FIXME Available VQwHardwareChannel methods
  size_t GetNumberOfDataWords() {return fNumberOfDataWords;}
  size_t GetNumberOfSubelements() {return fNumberOfSubElements;};
  
  Int_t GetRawValue() const       {return this.GetRawValue(0);};
  Double_t GetValue() const       {return this->GetValue(0);};
  Double_t GetValueM2() const     {return this->GetValueM2(0);};
  Double_t GetValueError() const  {return this->GetValueError(0);};
  Double_t GetValueWidth() const  {return this->GetValueWidth(0);};
  virtual Int_t GetRawValue(size_t element) const = 0;
  virtual Double_t GetValue(size_t element) const = 0;
  virtual Double_t GetValueM2(size_t element) const = 0;
  virtual Double_t GetValueError(size_t element) const = 0;
  Double_t GetValueWidth(size_t element) const {*/

void QwAlarmHandler::CheckAlarms() {
  // If user-name-of-variable exists then grab it, grab its value from memory, and then compare to the upper and lower limits defined by user (if they were defined) 
  std::string tmpAlarmStat = "OK";
  for ( size_t numAna = 0; numAna < fAlarmObjectList.size() ; numAna++ ) {
    if (fAlarmObjectList.at(numAna).value != NULL){
      //QwWarning << "fAlarmObjectList.at("<<numAna<<").value == " << fAlarmObjectList.at(numAna).value  <<QwLog::endl;
      //QwWarning << "fAlarmObjectList.at("<<numAna<<").value->GetValue() == " << fAlarmObjectList.at(numAna).value->GetValue()  <<QwLog::endl;
      if ( fAlarmObjectList.at(numAna).alarmParameterMapStr.count("Error-Code") != 0 
          && ((TString)fAlarmObjectList.at(numAna).alarmParameterMapStr.at("Error-Code")).IsHex() && ((std::stoul(fAlarmObjectList.at(numAna).alarmParameterMapStr.at("Error-Code"),nullptr,16)) & *fAlarmObjectList.at(numAna).eventcutErrorFlag) != 0 ) { 
        fAlarmObjectList.at(numAna).Nviolated++;
        fAlarmObjectList.at(numAna).NsinceLastViolation = 0;
        tmpAlarmStat = "Error-Code";
      }
      else if (fAlarmObjectList.at(numAna).alarmParameterMap.count("Exactly") != 0 
          && fAlarmObjectList.at(numAna).value->GetValue() != fAlarmObjectList.at(numAna).alarmParameterMap.at("Exactly"))  {
        fAlarmObjectList.at(numAna).Nviolated++;
        fAlarmObjectList.at(numAna).NsinceLastViolation = 0;
        tmpAlarmStat = "Not-Exactly";
      }
      else if ( fAlarmObjectList.at(numAna).alarmParameterMap.count("HighHigh") != 0 
          && fAlarmObjectList.at(numAna).value->GetValue() >= fAlarmObjectList.at(numAna).alarmParameterMap.at("HighHigh") ) { 
        fAlarmObjectList.at(numAna).Nviolated++;
        fAlarmObjectList.at(numAna).NsinceLastViolation = 0;
        tmpAlarmStat = "HighHigh";
      }
      else if ( fAlarmObjectList.at(numAna).alarmParameterMap.count("High") != 0 
          && fAlarmObjectList.at(numAna).value->GetValue() >= fAlarmObjectList.at(numAna).alarmParameterMap.at("High") ) {
        fAlarmObjectList.at(numAna).Nviolated++;
        fAlarmObjectList.at(numAna).NsinceLastViolation = 0;
        tmpAlarmStat = "High";
      }
      else if ( fAlarmObjectList.at(numAna).alarmParameterMap.count("LowLow") != 0 
          && fAlarmObjectList.at(numAna).value->GetValue() <= fAlarmObjectList.at(numAna).alarmParameterMap.at("LowLow") ) {
        fAlarmObjectList.at(numAna).Nviolated++;
        fAlarmObjectList.at(numAna).NsinceLastViolation = 0;
        tmpAlarmStat = "LowLow";
      }
      else if ( fAlarmObjectList.at(numAna).alarmParameterMap.count("Low") != 0 
          &&  fAlarmObjectList.at(numAna).value->GetValue() <= fAlarmObjectList.at(numAna).alarmParameterMap.at("Low") ) {
        fAlarmObjectList.at(numAna).Nviolated++;
        fAlarmObjectList.at(numAna).NsinceLastViolation = 0;
        tmpAlarmStat = "Low";
      }
      else {
        fAlarmObjectList.at(numAna).NsinceLastViolation++;
      }
      if ( fAlarmObjectList.at(numAna).Nviolated > 0 && fAlarmObjectList.at(numAna).NsinceLastViolation > fAlarmObjectList.at(numAna).alarmParameterMap.at("Ring-Length") ) { 
        fAlarmObjectList.at(numAna).Nviolated--;
      }
      if ( fAlarmObjectList.at(numAna).Nviolated > fAlarmObjectList.at(numAna).alarmParameterMap.at("Tolerance") ) {
        fAlarmObjectList.at(numAna).alarmStatus = tmpAlarmStat;
      }
      else {
        fAlarmObjectList.at(numAna).alarmStatus = "OK";
      }
    }
    else {
      QwError << "Null: fAlarmObjectList.at("<<numAna<<").value == NULL" <<QwLog::endl;
    }
  }
}

void QwAlarmHandler::UpdateAlarmFile(){
  std::ofstream file_out;
  // Format of alarmObject struct contents
  // std::map <std::string,std::string> alarmParameterMap;
  // VQwDataHandler::EQwHandleType analysisType;
  // const VQwHardwareChannel* value;
  // UInt_t eventcutErrorFlag;
  // std::string alarmStatus;
  // int Nviolated; // Vector of 0's for history tracking
  // int NsinceLastViolation; // Vector of 0's for history tracking

  file_out.open(fAlarmOutputFile,std::ofstream::trunc);
  for (size_t ite = 0 ; ite<fAlarmObjectList.size(); ite++){
    if (fAlarmObjectList.at(ite).value != NULL && fAlarmObjectList.at(ite).alarmParameterMapStr.count("Kind") && fAlarmObjectList.at(ite).alarmParameterMapStr.count("Chan") && fAlarmObjectList.at(ite).alarmParameterMapStr.count("Analysis")) {
      if (fAlarmObjectList.at(ite).value != 0) { // Check if non-trivial value object...
        file_out<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Kind")<<","<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Chan")<<","<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Analysis")<<","<<"Value"<<","<<fAlarmObjectList.at(ite).value->GetValue()<<std::endl;
      }
      else continue;
      if (fAlarmObjectList.at(ite).alarmStatus != "") { // Check if non-trivial value object...
        file_out<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Kind")<<","<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Chan")<<","<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Analysis")<<","<<"Alarm Status"<<","<<fAlarmObjectList.at(ite).alarmStatus<<std::endl;
      }
      else continue;
      if (fAlarmObjectList.at(ite).alarmParameterMapStr.count("Error-Code")) { // Check if non-trivial value object...
        file_out<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Kind")<<","<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Chan")<<","<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Analysis")<<","<<"Error-Code"<<","<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Error-Code")<<std::endl;
      }
    }
    else continue;
    for (auto jte : fAlarmObjectList.at(ite).alarmParameterMap){ // Loop through parameter list
      file_out<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Kind")<<","<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Chan")<<","<<fAlarmObjectList.at(ite).alarmParameterMapStr.at("Analysis")<<","<<jte.first<<","<<jte.second<<std::endl;
    }
  }
  file_out.close();
}

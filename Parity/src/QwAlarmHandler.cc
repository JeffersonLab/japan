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
  //ParseSeparator = ":";
  //fKeepRunningSum = kTRUE;
}

QwAlarmHandler::QwAlarmHandler(const QwAlarmHandler &source):VQwDataHandler(source)
{
}

/// Destructor
QwAlarmHandler::~QwAlarmHandler()
{
  Iterator_HdwChan element;
  for (element = fOutputVar.begin(); element != fOutputVar.end(); element++) {
    if (*element != NULL){
       delete *element;
    }
  }
  fOutputVar.clear();
}


//  Just use the base class version for now....

void ParseConfigFile(QwParameterFile& file)
{
  file.RewindToFileStart();
  file.EnableGreediness();
  while (file.ReadNextLine()) {
    QwMessage << file.GetLine() << QwLog::endl;
  }
  // Check for and process key-value pairs
  //file.PopValue("new-key-word",fsomething); // These need = signs in map files
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

  // Add independent variables and sensitivities
  while (map.ReadNextLine()) {
    // Throw away comments, whitespace, empty lines
    map.TrimComment();
    map.TrimWhitespace();
    if (map.LineIsEmpty()) continue;
    // Get first token: label (dv or iv), second token is the name like "asym_blah"
//  Type, Channel, Ana, tree, channel, highhigh, high, low, lowlow, RingLength, pat-tolerance
//  main-det usr mean mul asym_usr 1000 100 -100 -1000 200 2
    fType.push_back(map.GetNextToken(" "));
    fChannel.push_back(map.GetNextToken(" "));
    fAna.push_back(map.GetNextToken(" "));
    fTree.push_back(map.GetNextToken(" "));
    std::string localAna_token = map.GetNextToken(" ");
    // Parse current token into independent variable type and name
    type_name = ParseHandledVariable(localAna_token); // This refers to the object under scrutiny, finds type of object - asym, yield, diff, etc. Check on yields FIXME
    fAnalysisType.push_back(type_name.first);
    fAnalysisName.push_back(type_name.second);
    fHighHigh.push_back(map.GetNextToken(" "));
    fHigh.push_back(map.GetNextToken(" "));
    fLow.push_back(map.GetNextToken(" "));
    fLowLow.push_back(map.GetNextToken(" "));
    fRingLength.push_back(map.GetNextToken(" ")); 
    fTolerance.push_back(map.GetNextToken(" ")); // This is all hardcoded.... FIXME
    fNviolated.push_back(0);

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
  // Return if correction is not enabled

  /// Fill vector of pointers to the relevant data elements
  for (size_t anaInd = 0; anaInd < fAnalysisName.size(); anaInd++) {
    // Get the dependent variables
    if (fAnalysisType.at(anaInd)==kHandleTypeMps){
      //  Quietly ignore the MPS type when we're connecting the asym & diff
      continue;
    }
    const VQwHardwareChannel* ana_ptr = 0;
    switch (fAnalysisType.at(anaInd)) {
      case kHandleTypeYield:
        ana_ptr = yield.ReturnInternalValue(fAnalysisName.at(anaInd));
        break;
      case kHandleTypeAsym:
        ana_ptr = asym.ReturnInternalValue(fAnalysisName.at(anaInd));
        break;
      case kHandleTypeDiff:
        ana_ptr = diff.ReturnInternalValue(fAnalysisName.at(anaInd));
        break;
      default:
        QwWarning << "Independent variable for combiner has unknown type."
          << QwLog::endl;
        break;
    }
    if (ana_ptr) {
      //QwMessage << " anaInd: " << fAnalysisName.at(anaInd).at(anaInd) << " (sens = " << fSensitivity.at(anaInd).at(anaInd) << ")" << QwLog::endl;
      fValue.push_back(ana_ptr);
    } else {
      QwWarning << "Independent variable " << fAnalysisName.at(anaInd) << " for combiner of "
        << QwLog::endl;
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
  for (size_t dv = 0; dv < fAnalysisName.size(); dv++) {
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
        dv_ptr = event.ReturnInternalValue(fAnalysisName.at(dv));

        vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
        name = vqwk->GetElementName().Data();
        new_vqwk = new QwVQWK_Channel(*vqwk, VQwDataElement::kDerived);
        new_vqwk->SetElementName(name);
      }
      new_vqwk->SetSubsystemName(fName);
    }

    // alias
    if(new_vqwk==NULL){
      QwWarning << "Dependent variable " << fAnalysisName.at(dv) << " could not be found, "
                << "or is not a VQWK channel." << QwLog::endl;
      continue; 
    } else {
      //QwMessage << "dv: " << new_vqwk->GetElementName() << QwLog::endl;
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
  CheckAlarms();
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
  
  Int_t GetRawValue() const       {return this->GetRawValue(0);};
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
  // if user-name-of-variable exists then grab it, grab its value from memory, and then compare to the upper and lower limits defined by user (if they were defined) 
  std::string tmpAlarmStat = "";
  for ( size_t numAna = 0; numAna < fAnalysisName.size() ; numAna++ ) {
    if ( fHighHigh.at(numAna) != "NULL" && fValue.at(numAna)->GetValue() >= std::stod(fHighHigh.at(numAna)) ) { 
      fNviolated.at(numAna)++;
      fNsinceLastViolation.at(numAna) = 0;
      tmpAlarmStat = "HighHigh";
    }
    else if ( fHigh.at(numAna) != "NULL" && fValue.at(numAna)->GetValue() >= std::stod(fHigh.at(numAna)) ) {
      fNviolated.at(numAna)++;
      fNsinceLastViolation.at(numAna) = 0;
      tmpAlarmStat = "High";
    }
    else if ( fLowLow.at(numAna) != "NULL" && fValue.at(numAna)->GetValue() <= std::stod(fLowLow.at(numAna)) ) {
      fNviolated.at(numAna)++;
      fNsinceLastViolation.at(numAna) = 0;
      tmpAlarmStat = "LowLow";
    }
    else if ( fLow.at(numAna) != "NULL" &&  fValue.at(numAna)->GetValue() <= std::stod(fLow.at(numAna)) ) {
      fNviolated.at(numAna)++;
      fNsinceLastViolation.at(numAna) = 0;
      tmpAlarmStat = "Low";
    }
    fNsinceLastViolation.at(numAna)++;
    if ( fNsinceLastViolation.at(numAna) < stoi(fRingLength.at(numAna)) ) { 
      fNviolated.at(numAna)--;
    }
    if ( fNviolated.at(numAna) > stoi(fTolerance.at(numAna)) ) {
      fAlarmStatus.at(numAna) = tmpAlarmStat;
    }
    else {
      fAlarmStatus.at(numAna) = "OK";
    }
  }
}

void QwAlarmHandler::UpdateAlarmFile() {
  int test1 = 0;
}

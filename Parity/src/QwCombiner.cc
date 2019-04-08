/*
 * QwCombiner.cc
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 *
 *  Last Modified: August 1, 2018 1:45 PM
 */

#include "QwCombiner.h"

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
RegisterHandlerFactory(QwCombiner);


/// \brief Constructor with name
QwCombiner::QwCombiner(const string& name):VQwDataHandler(name)
{
  ParseSeparator = ":";
}


/** Constructor with single event and helicity pattern
 *
 * @param options Options object
 * @param event Single event
 * @param helicitypattern Helicity pattern
 */
QwCombiner::QwCombiner(
    QwOptions &options,
    QwSubsystemArrayParity& event,
    QwHelicityPattern& helicitypattern)
{
  ParseSeparator = ":";
  ProcessOptions(options);
  fSubsystemArray = &event;
  fHelicityPattern = &helicitypattern;
  //  LoadChannelMap(fMapFile);
  QwSubsystemArrayParity& asym = helicitypattern.fAsymmetry;
  QwSubsystemArrayParity& diff = helicitypattern.fDifference;
  ConnectChannels(event,asym,diff);
}

/** Constructor with single event
 * 
 * @param options Options object
 * @param event Single event
 * 
 */
QwCombiner::QwCombiner(QwOptions &options, QwSubsystemArrayParity& event)
{
  fHelicityPattern = NULL;
  ParseSeparator = ":";
  ProcessOptions(options);
  fSubsystemArray = &event;
  //  LoadChannelMap(fMapFile);
  ConnectChannels(event);
}

/** Constructor with only helicity pattern
 * 
 * @param options Obtions object
 * @param helicitypattern Helicity Pattern
 * 
 */
QwCombiner::QwCombiner(QwOptions &options, QwHelicityPattern& helicitypattern)
{
  fSubsystemArray = NULL;
  ParseSeparator = ":";
  ProcessOptions(options);
  //  LoadChannelMap(fMapFile);
  fHelicityPattern = &helicitypattern;
  QwSubsystemArrayParity& asym = helicitypattern.fAsymmetry;
  QwSubsystemArrayParity& diff = helicitypattern.fDifference;
  ConnectChannels(asym,diff);
}

/** Constructor with options
 * 
 * @param options Obtions object
 * 
 */
QwCombiner::QwCombiner(QwOptions &options)
{
  ProcessOptions(options);
  //  LoadChannelMap(fMapFile);
} 


QwCombiner::QwCombiner(const QwCombiner &source)
{
  fMapFile = source.fMapFile;
  fErrorFlag = source.fErrorFlag;
  this->fDependentVar.resize(source.fDependentVar.size());
  fDependentType.resize(source.fDependentVar.size());
  fOutputVar.resize(source.fDependentVar.size());
  for (size_t i = 0; i < this->fDependentVar.size(); i++) {
    const QwVQWK_Channel* vqwk = dynamic_cast<const QwVQWK_Channel*>(source.fOutputVar[i]);
    this->fDependentVar[i] = NULL;
    this->fOutputVar[i] = new QwVQWK_Channel(*vqwk, VQwDataElement::kDerived);
    fDependentType[i] = source.fDependentType[i];
  }
}

/// Destructor
QwCombiner::~QwCombiner()
{
  Iterator_HdwChan element;
  for (element = fOutputVar.begin(); element != fOutputVar.end(); element++) {
    if (*element != NULL){
       delete *element;
    }
  }
  fOutputVar.clear();
}

/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwCombiner::DefineOptions(QwOptions &options){}

/**
 * Process configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwCombiner::ProcessOptions(QwOptions &options){}

/*  Just use the base class version for now....
 *
 * void LoadDetectorMaps(QwParameterFile& file)
 * {
 *   VQwDataHandler::LoadDetectorMaps(file);
 *   file.PopValue("slope-path", outPath);
 * }
 */

/** Load the channel map
 *
 * @param mapfile Filename of map file
 * @return Zero when success
 */
Int_t QwCombiner::LoadChannelMap(const std::string& mapfile)
{
  // Return if correctiion is not enabled

  // Open the file
  QwParameterFile map(mapfile);

  // Read the sections of dependent variables
  bool keep_header = true;
  std::string section_name;
  QwParameterFile* section = 0;
  std::pair<EQwHandleType,std::string> type_name;
  while ((section = map.ReadNextSection(section_name,keep_header))) {

    // Store index to the current position in the dv vector
    size_t current_dv_start = fDependentName.size();

    // Add dependent variables from the section header
    section->ReadNextLine();
    if (section->LineHasSectionHeader()) {
      section->TrimSectionHeader();
      section->TrimWhitespace();
      // Parse section header into tokens separated by a comma
      std::string current_token;
      std::string previous_token;
      do {
        previous_token = current_token;
        current_token = section->GetNextToken(",");
        if (current_token.size() == 0) continue;
        // Parse current token into dependent variable type and name
        type_name = ParseHandledVariable(current_token);
        fDependentType.push_back(type_name.first);
        fDependentName.push_back(type_name.second);
        // Resize the vectors of sensitivities and independent variables
        fSensitivity.resize(fDependentName.size());
        fIndependentType.resize(fDependentName.size());
        fIndependentName.resize(fDependentName.size());
      } while (current_token.size() != 0);
    } else QwError << "Section does not start with header." << QwLog::endl;

    // Add independent variables and sensitivities
    while (section->ReadNextLine()) {
      // Throw away comments, whitespace, empty lines
      section->TrimComment();
      section->TrimWhitespace();
      if (section->LineIsEmpty()) continue;
      // Get first token: independent variable
      std::string current_token = section->GetNextToken(",");
      // Parse current token into independent variable type and name
      type_name = ParseHandledVariable(current_token);
      // Loop over dependent variables to set sensitivities
      for (size_t dv = current_dv_start; dv < fDependentName.size(); dv++) {
        Double_t sensitivity = atof(section->GetNextToken(",").c_str());
        fSensitivity.at(dv).push_back(sensitivity);
        fIndependentType.at(dv).push_back(type_name.first);
        fIndependentName.at(dv).push_back(type_name.second);
      }
    }
  }
  return 0;
}

/** Connect to the dependent and independent channels
 *
 * @param event Helicity event structure
 * @param asym Asymmetry event structure
 * @param diff Difference event structure
 * @return Zero on success
 */
Int_t QwCombiner::ConnectChannels(
    QwSubsystemArrayParity& event,
    QwSubsystemArrayParity& asym,
    QwSubsystemArrayParity& diff)
{
  // Return if correction is not enabled

  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fDependentName.size(); dv++) {
    // Get the dependent variables

    VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    QwVQWK_Channel* vqwk = NULL;
    string name = "";
    string calc = "calc_";
    
    if(fDependentName.at(dv).at(0) == '@' ){
        name = fDependentName.at(dv).substr(1,fDependentName.at(dv).length());
    }else{
      switch (fDependentType.at(dv)) {
        case kHandleTypeMps:
          dv_ptr = event.ReturnInternalValueForFriends(fDependentName.at(dv));
          break;
        case kHandleTypeAsym:
          dv_ptr = asym.ReturnInternalValueForFriends(fDependentName.at(dv));
          break;
        case kHandleTypeDiff:
          dv_ptr = diff.ReturnInternalValueForFriends(fDependentName.at(dv));
          break;
        default:
          QwWarning << "Dependent variable for combiner has unknown type."
                    << QwLog::endl;
          break;
      }

      vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
      name = vqwk->GetElementName().Data();
      name.insert(0, calc);
      new_vqwk = new QwVQWK_Channel(*vqwk, VQwDataElement::kDerived);
      new_vqwk->SetElementName(name);
    }

    // alias
    if(fDependentName.at(dv).at(0) == '@'){
      //QwMessage << "dv: " << name << QwLog::endl;
      new_vqwk = new QwVQWK_Channel(name, VQwDataElement::kDerived);
    }
    // defined type
    else if(dv_ptr!=NULL){
      //QwMessage << "dv: " << fDependentName.at(dv) << QwLog::endl;
    }else {
      QwWarning << "Dependent variable " << fDependentName.at(dv) << " could not be found, "
                << "or is not a VQWK channel." << QwLog::endl;
      continue; 
    }

    // pair creation
    if(new_vqwk != NULL){
      fDependentType.push_back(fDependentType.at(dv));
      fDependentVar.push_back(vqwk);
      fOutputVar.push_back(new_vqwk);
      //fDependentVar.push_back(std::make_pair(vqwk, new_vqwk));
    }

    // Add independent variables
    fIndependentVar.resize(fDependentVar.size());
    for (size_t iv = 0; iv < fIndependentName.at(dv).size(); iv++) {
      // Get the independent variables
      const VQwHardwareChannel* iv_ptr = 0;
      switch (fIndependentType.at(dv).at(iv)) {
        case kHandleTypeMps:
          iv_ptr = event.ReturnInternalValue(fIndependentName.at(dv).at(iv));
          break;
        case kHandleTypeAsym:
          iv_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).at(iv));
          break;
        case kHandleTypeDiff:
          iv_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).at(iv));
          break;
        default:
          QwWarning << "Independent variable for combiner has unknown type."
                    << QwLog::endl;
          break;
      }
      if (iv_ptr) {
        //QwMessage << " iv: " << fIndependentName.at(dv).at(iv) << " (sens = " << fSensitivity.at(dv).at(iv) << ")" << QwLog::endl;
        fIndependentVar.back().push_back(iv_ptr);
        //fIndependentVar.back().push_back(std::make_pair(fSensitivity.at(dv).at(iv), iv_ptr));
      } else {
        QwWarning << "Independent variable " << fIndependentName.at(dv).at(iv) << " for combiner of "
                  << "dependent variable " << fDependentName.at(dv) << " could not be found."
                  << QwLog::endl;
      }
    }
  }
  
  return 0;

}
/** Connect to the dependent and independent channels
 *
 * @param asym Asymmetry event structure
 * @param diff Difference event structure
 * @return Zero on success
 */
Int_t QwCombiner::ConnectChannels(
    QwSubsystemArrayParity& asym,
    QwSubsystemArrayParity& diff)
{
  // Return if correction is not enabled

  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fDependentName.size(); dv++) {
    // Get the dependent variables

    VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    QwVQWK_Channel* vqwk = NULL;
    string name = "";
    string calc = "calc_";
    
    if (fDependentType.at(dv)==kHandleTypeMps){
      //  Quietly ignore the MPS type when we're connecting the asym & diff
      continue;
    } else if(fDependentName.at(dv).at(0) == '@' ){
        name = fDependentName.at(dv).substr(1,fDependentName.at(dv).length());
    }else{
      switch (fDependentType.at(dv)) {
        case kHandleTypeAsym:
          dv_ptr = asym.ReturnInternalValueForFriends(fDependentName.at(dv));
          break;
        case kHandleTypeDiff:
          dv_ptr = diff.ReturnInternalValueForFriends(fDependentName.at(dv));
          break;
        default:
          QwWarning << "QwCombiner::ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff):  Dependent variable, "
		                << fDependentName.at(dv)
		                << ", for asym/diff combiner does not have proper type, type=="
		                << fDependentType.at(dv) << "."<< QwLog::endl;
          break;
      }

      vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
      name = vqwk->GetElementName().Data();
      name.insert(0, calc);
      new_vqwk = new QwVQWK_Channel(*vqwk, VQwDataElement::kDerived);
      new_vqwk->SetElementName(name);
    }

    // alias
    if(fDependentName.at(dv).at(0) == '@'){
      //QwMessage << "dv: " << name << QwLog::endl;
      new_vqwk = new QwVQWK_Channel(name, VQwDataElement::kDerived);
    }
    // defined type
    else if(dv_ptr!=NULL){
      //QwMessage << "dv: " << fDependentName.at(dv) << QwLog::endl;
    }else {
      QwWarning << "Dependent variable " << fDependentName.at(dv) << " could not be found, "
                << "or is not a VQWK channel." << QwLog::endl;
      continue; 
    }

    // pair creation
    if(new_vqwk != NULL){
      fDependentType.push_back(fDependentType.at(dv));
      fDependentVar.push_back(vqwk);
      fOutputVar.push_back(new_vqwk);
      //fDependentVar.push_back(std::make_pair(vqwk, new_vqwk));
    }

    // Add independent variables
    fIndependentVar.resize(fDependentVar.size());
    for (size_t iv = 0; iv < fIndependentName.at(dv).size(); iv++) {
      // Get the independent variables
      const VQwHardwareChannel* iv_ptr = 0;
      switch (fIndependentType.at(dv).at(iv)) {
        case kHandleTypeAsym:
          iv_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).at(iv));
          break;
        case kHandleTypeDiff:
          iv_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).at(iv));
          break;
        default:
          QwWarning << "Independent variable for combiner has unknown type."
                    << QwLog::endl;
          break;
      }
      if (iv_ptr) {
        //QwMessage << " iv: " << fIndependentName.at(dv).at(iv) << " (sens = " << fSensitivity.at(dv).at(iv) << ")" << QwLog::endl;
        fIndependentVar.back().push_back(iv_ptr);
      } else {
        QwWarning << "Independent variable " << fIndependentName.at(dv).at(iv) << " for combiner of "
                  << "dependent variable " << fDependentName.at(dv) << " could not be found."
                  << QwLog::endl;
      }
    }
  }
  
  return 0;

}

/** Connect to the dependent and independent channels
 *
 * @param event Helicity event structure
 * @return Zero on success
 */
Int_t QwCombiner::ConnectChannels(QwSubsystemArrayParity& event)
{
  // Return if correction is not enabled

  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fDependentName.size(); dv++) {
    // Get the dependent variables

    VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    QwVQWK_Channel* vqwk = NULL;
    string name = " s";
    string calc = "calc_";

    if (fDependentType.at(dv)==kHandleTypeAsym || fDependentType.at(dv)==kHandleTypeDiff){
      //  Quietly skip the asymmetry or difference types.
      continue;
    } else if(fDependentType.at(dv) != kHandleTypeMps){
      QwWarning << "QwCombiner::ConnectChannels(QwSubsystemArrayParity& event):  Dependent variable, "
                << fDependentName.at(dv)
	              << ", for MPS combiner does not have MPS type, type=="
	              << fDependentType.at(dv) << "."<< QwLog::endl;
      continue;
    } else {
      if(fDependentName.at(dv).at(0) == '@' ){
        name = fDependentName.at(dv).substr(1,fDependentName.at(dv).length());
        new_vqwk = new QwVQWK_Channel(name, VQwDataElement::kDerived);
      } else {
        dv_ptr = event.ReturnInternalValueForFriends(fDependentName.at(dv));

        vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
        name = vqwk->GetElementName().Data();
        name.insert(0,calc);
        new_vqwk = new QwVQWK_Channel(*vqwk, VQwDataElement::kDerived);
        new_vqwk->SetElementName(name);
      }
    }

    // alias
    if(new_vqwk==NULL){
      QwWarning << "Dependent variable " << fDependentName.at(dv) << " could not be found, "
                << "or is not a VQWK channel." << QwLog::endl;
      continue; 
    } else {
      //QwMessage << "dv: " << new_vqwk->GetElementName() << QwLog::endl;
      // pair creation
      fDependentType.push_back(fDependentType.at(dv));
      fDependentVar.push_back(vqwk);
      fOutputVar.push_back(new_vqwk);
      //fDependentVar.push_back(std::make_pair(vqwk, new_vqwk));
    }

    // Add independent variables
    fIndependentVar.resize(fDependentVar.size());
    for (size_t iv = 0; iv < fIndependentName.at(dv).size(); iv++) {
      // Get the independent variables
      const VQwHardwareChannel* iv_ptr = 0;
      if(fIndependentType.at(dv).at(iv) == kHandleTypeMps){
        iv_ptr = event.ReturnInternalValue(fIndependentName.at(dv).at(iv));
    	} else {
        QwWarning << "Independent variable for MPS combiner has unknown type."
                  << QwLog::endl;
      }
      if (iv_ptr) {
        //QwMessage << " iv: " << fIndependentName.at(dv).at(iv) << " (sens = " << fSensitivity.at(dv).at(iv) << ")" << QwLog::endl;
        fIndependentVar.back().push_back(iv_ptr);
      } else {
        QwWarning << "Independent variable " << fIndependentName.at(dv).at(iv) << " for combiner of "
                  << "dependent variable " << fDependentName.at(dv) << " could not be found."
                  << QwLog::endl;
      }
    }
  }
  
  return 0;
}

void QwCombiner::ProcessData() {
  // Return if correction is not enabled
  if (! true){
    QwDebug << "QwCombiner is not enabled!" << QwLog::endl;
    return;
  }

  // Get error flag from QwHelicityPattern
  if (fHelicityPattern != NULL){
    fErrorFlag = fHelicityPattern->GetEventcutErrorFlag();
  } else if (fSubsystemArray != NULL){
    fErrorFlag = fSubsystemArray->GetEventcutErrorFlag();
  } else {
    QwError << "QwCombiner::LinearProcessData: Can't set fErrorFlag" << QwLog::endl;
    fErrorFlag = 0;
  }
 
  for (size_t i = 0; i < fDependentVar.size(); ++i) {
    CalcOneOutput(fDependentVar[i], fOutputVar[i], fIndependentVar[i], fSensitivity[i]);
  }
  
}


/*
 * QwCombiner.cc
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 *
 *  Last Modified: August 1, 2018 1:45 PM
 */

#include "QwCombiner.h"
#include "MQwPublishable.h"
#include "VQwSubsystem.h"
#include <iostream>
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "VQwDataElement.h"
#include "QwVQWK_Channel.h"
#include "QwParameterFile.h"
#include "QwHelicityPattern.h"

#define MYSQLPP_SSQLS_NO_STATICS
#ifdef __USE_DATABASE__
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__

#include "QwPromptSummary.h"

// Register this handler with the factory
RegisterHandlerFactory(QwCombiner);


/// \brief Constructor with name
QwCombiner::QwCombiner(const TString& name)
: VQwDataHandler(name)
{
  ParseSeparator = ":";
  fKeepRunningSum = kTRUE;
  fErrorFlagMask = 0;
  fErrorFlagPointer = 0;
}

QwCombiner::QwCombiner(const QwCombiner &source)
: VQwDataHandler(source)
{
  fErrorFlagMask = 0;
  fErrorFlagPointer = 0;
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


/*  Just use the base class version for now....
 *
 * void ParseConfigFile(QwParameterFile& file)
 * {
 *   VQwDataHandler::ParseConfigFile(file);
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
  // Open the file
  QwParameterFile map(mapfile);

  // Read the preamble
  QwParameterFile* preamble = map.ReadSectionPreamble();
  TString mask;
  if (preamble->FileHasVariablePair("=", "mask", mask)) {
    fErrorFlagMask = QwParameterFile::GetUInt(mask);
  }


  // Read the sections of dependent variables
  bool keep_header = true;
  std::string section_name;
  QwParameterFile* section = 0;
  std::pair<EQwHandleType,std::string> type_name;
  while ((section = map.ReadNextSection(section_name,keep_header))) {
    if(section_name=="PUBLISH") continue;

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
 
  TString varvalue; 
  // Now load the variables to publish
  std::vector<std::vector<TString> > fPublishList;
  map.RewindToFileStart();
  QwParameterFile *section2;
  std::vector<TString> publishinfo;
  while ((section2=map.ReadNextSection(varvalue))) {
    if (varvalue == "PUBLISH") {
      fPublishList.clear();
      while (section2->ReadNextLine()) {
        section2->TrimComment(); // Remove everything after a comment character
        section2->TrimWhitespace(); // Get rid of leading and trailing spaces
        for (int ii = 0; ii < 4; ii++) {
          varvalue = section2->GetTypedNextToken<TString>();
          if (varvalue.Length()) {
            publishinfo.push_back(varvalue);
          }
        }
        if (publishinfo.size() == 4)
          fPublishList.push_back(publishinfo);
        publishinfo.clear();
      }
    }
    delete section2;
  }
  // Print list of variables to publish
  if (fPublishList.size()>0){
    QwMessage << "Variables to publish:" << QwLog::endl;
    for (size_t jj = 0; jj < fPublishList.size(); jj++){
      QwMessage << fPublishList.at(jj).at(0) << " " << fPublishList.at(jj).at(1) << " " << fPublishList.at(jj).at(2) << " " << fPublishList.at(jj).at(3) << QwLog::endl;
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

    const VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    const QwVQWK_Channel* vqwk = NULL;
    string name = "";
    string calc = "calc_";
    
    if (fDependentType.at(dv)==kHandleTypeMps){
      //  Quietly ignore the MPS type when we're connecting the asym & diff
      continue;
    } else if(fDependentName.at(dv).at(0) == '@' ){
        name = fDependentName.at(dv).substr(1,fDependentName.at(dv).length());
    }else{
      dv_ptr = this->RequestExternalPointer(fDependentFull.at(dv));
      if (dv_ptr==NULL){
	switch (fDependentType.at(dv)) {
        case kHandleTypeAsym:
          dv_ptr = asym.RequestExternalPointer(fDependentName.at(dv));
          break;
        case kHandleTypeDiff:
          dv_ptr = diff.RequestExternalPointer(fDependentName.at(dv));
          break;
        default:
          QwWarning << "QwCombiner::ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff):  Dependent variable, "
		                << fDependentName.at(dv)
		                << ", for asym/diff combiner does not have proper type, type=="
		                << fDependentType.at(dv) << "."<< QwLog::endl;
          break;
	}
      }

      vqwk = dynamic_cast<const QwVQWK_Channel*>(dv_ptr);
      name = vqwk->GetElementName().Data();
      name.insert(0, calc);
      new_vqwk = new QwVQWK_Channel(*vqwk, VQwDataElement::kDerived);
      new_vqwk->SetElementName(name);
      new_vqwk->SetSubsystemName(fName);
    }

    // alias
    if(fDependentName.at(dv).at(0) == '@'){
      //QwMessage << "dv: " << name << QwLog::endl;
      new_vqwk = new QwVQWK_Channel(name, VQwDataElement::kDerived);
      new_vqwk->SetSubsystemName(fName);
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
      iv_ptr = RequestExternalPointer(fIndependentName.at(dv).at(iv));
      if (iv_ptr == NULL){
	switch (fIndependentType.at(dv).at(iv)) {
        case kHandleTypeAsym:
          iv_ptr = asym.RequestExternalPointer(fIndependentName.at(dv).at(iv));
          break;
        case kHandleTypeDiff:
          iv_ptr = diff.RequestExternalPointer(fIndependentName.at(dv).at(iv));
          break;
        default:
          QwWarning << "Independent variable for combiner has unknown type."
                    << QwLog::endl;
          break;
	}
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
  
  // Store error flag pointer
  QwMessage << "Using asymmetry error flag" << QwLog::endl;
  fErrorFlagPointer = asym.GetEventcutErrorFlagPointer();

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

    const VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    const QwVQWK_Channel* vqwk = NULL;
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
        dv_ptr = event.RequestExternalPointer(fDependentName.at(dv));

        vqwk = dynamic_cast<const QwVQWK_Channel*>(dv_ptr);
        name = vqwk->GetElementName().Data();
        name.insert(0,calc);
        new_vqwk = new QwVQWK_Channel(*vqwk, VQwDataElement::kDerived);
        new_vqwk->SetElementName(name);
      }
      new_vqwk->SetSubsystemName(fName);
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
        iv_ptr = event.RequestExternalPointer(fIndependentName.at(dv).at(iv));
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
  
  // Store error flag pointer
  QwMessage << "Using event error flag" << QwLog::endl;
  fErrorFlagPointer = event.GetEventcutErrorFlagPointer();

  return 0;
}

void QwCombiner::ProcessData()
{
  if (fErrorFlagMask!=0 && fErrorFlagPointer!=NULL) {
    if ((*fErrorFlagPointer & fErrorFlagMask)!=0) {
      //QwMessage << "0x" << std::hex << *fErrorFlagPointer << " passed mask " << "0x" << fErrorFlagMask << std::dec << QwLog::endl;
      for (size_t i = 0; i < fDependentVar.size(); ++i) {
        CalcOneOutput(fDependentVar[i], fOutputVar[i], fIndependentVar[i], fSensitivity[i]);
      }
    //} else {
      //QwMessage << "0x" << std::hex << *fErrorFlagPointer << " failed mask " << "0x" << fErrorFlagMask << std::dec << QwLog::endl;
    }
  }
  else{
    for (size_t i = 0; i < fDependentVar.size(); ++i) {
      CalcOneOutput(fDependentVar[i], fOutputVar[i], fIndependentVar[i], fSensitivity[i]);
    }
  }
}

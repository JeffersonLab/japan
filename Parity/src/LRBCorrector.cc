/********************************************************************
File Name: LRBCorrector.cc

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This is the implementaion file of the LRBCorrector
              class, which is a child of the VQwDataHandler class.
              The functionality of this class is derived from
              LinRegBlue.

Last Modified: August 1, 2018 1:41 PM
********************************************************************/

#include <iostream>

#include "QwHelicityPattern.h"

#include "LRBCorrector.h"

// Qweak headers
#include "VQwDataElement.h"
#include "QwVQWK_Channel.h"
#include "QwParameterFile.h"
#define MYSQLPP_SSQLS_NO_STATICS
#ifdef __USE_DATABASE__
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__

#include <TFile.h>
#include <TH2.h>
#include <TTree.h> 

#include <TChain.h>
#include <TObjArray.h>
#include <TEventList.h> 

#include <TMatrixD.h>

// Register this handler with the factory
RegisterHandlerFactory(LRBCorrector);

/// \brief Constructor with name
LRBCorrector::LRBCorrector(const TString& name):VQwDataHandler(name)
{
  ParseSeparator = "_";
  fKeepRunningSum = kTRUE;
}

void LRBCorrector::ParseConfigFile(QwParameterFile& file)
{
  VQwDataHandler::ParseConfigFile(file);
  file.PopValue("slope-file-base", fAlphaFileBase);
  file.PopValue("slope-file-suff", fAlphaFileSuff);
  file.PopValue("slope-path", fAlphaFilePath);
}

Int_t LRBCorrector::LoadChannelMap(const std::string& mapfile)
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
    string primary_token = map.GetNextToken(" ");
    string current_token = map.GetNextToken(" ");
    // Parse current token into independent variable type and name
    type_name = ParseHandledVariable(current_token);

    if (primary_token == "iv") {
      fIndependentType.push_back(type_name.first);
      fIndependentName.push_back(type_name.second);
    }
    else if (primary_token == "dv") {
      fDependentType.push_back(type_name.first);
      fDependentName.push_back(type_name.second);
    }
    else if (primary_token == "treetype") {
      QwMessage << "Tree Type read, ignoring." << QwLog::endl;
    }
    else {
      QwError << "Function LoadChannelMap in LRBCorrector.cc read in invalid primary_token." << QwLog::endl;
    }
  }

  // Resize sensitivity with all zeros
  fSensitivity.resize(fDependentType.size());
  for (size_t i = 0; i != fDependentType.size(); i++)
    fSensitivity.at(i).resize(fIndependentType.size());

  // Construct slope file name
  std::string SlopeFileName = fAlphaFileBase + run_label.Data() + fAlphaFileSuff;
  std::string SlopeFilePath = fAlphaFilePath + "/";
  std::string SlopeFile = SlopeFilePath + SlopeFileName;
  TString corFileName(SlopeFile);

  QwMessage << "Trying to open " << corFileName << QwLog::endl;
  TFile* corFile = new TFile(corFileName);
  if (! corFile->IsOpen()) {
    QwWarning << "Failed to open " << corFileName << ", slopes NOT found" << QwLog::endl;
    return 0;
  }

  // Slope matrix
  TMatrixD *alphasM = (TMatrixD *) corFile->Get("slopes");
  if (alphasM == 0) {
    QwWarning << "Slope matrix is null" << QwLog::endl;
    corFile->Close();
    return 0;
  }
  if (alphasM->GetNrows() != Int_t(fIndependentType.size())) {
    QwWarning << "Slope matrix has wrong number of rows: "
              << alphasM->GetNrows() << " != " << fIndependentType.size()
              << QwLog::endl;
    corFile->Close();
    return 0;
  }
  if (alphasM->GetNcols() != Int_t(fDependentType.size())) {
    QwWarning << "Slope matrix has wrong number of cols: "
              << alphasM->GetNcols() << " != " << fDependentType.size()
              << QwLog::endl;
    corFile->Close();
    return 0;
  }

  // DV names
  TH1 *dvnames = (TH1 *) corFile->Get("DVname");
  if (dvnames == 0) {
    QwWarning << "DV names matrix is null" << QwLog::endl;
    corFile->Close();
    return 0;
  }
  for (size_t i = 0; i != fDependentName.size(); i++) {
    TString name = dvnames->GetXaxis()->GetBinLabel(i+1);
    name.Remove(0, name.First("_") + 1);
    if (fDependentName[i] != name) {
      QwWarning << "DV name expected differs from found: "
                << fDependentName[i] << " != " << name
                << QwLog::endl;
      corFile->Close();
      return 0;
    }
  }

  // IV names
  TH1 *ivnames = (TH1 *) corFile->Get("IVname");
  if (ivnames == 0) {
    QwWarning << "IV names matrix is null" << QwLog::endl;
    corFile->Close();
    return 0;
  }
  for (size_t i = 0; i != fIndependentName.size(); i++) {
    TString name = ivnames->GetXaxis()->GetBinLabel(i+1);
    name.Remove(0, name.First("_") + 1);
    if (fIndependentName[i] != name) {
      QwWarning << "IV name expected differs from found: "
                << fIndependentName[i] << " != " << name
                << QwLog::endl;
      corFile->Close();
      return 0;
    }
  }

  // Assign sensitivities
  for (Int_t i = 0; i < ivnames->GetXaxis()->GetNbins(); ++i) {
    for (Int_t j = 0; j < dvnames->GetXaxis()->GetNbins(); ++j) {
      fSensitivity[j].push_back(-1.0*(*alphasM)(i,j));
    }
  }

  corFile->Close();
  return 0;
}


Int_t LRBCorrector::ConnectChannels(
    QwSubsystemArrayParity& asym,
    QwSubsystemArrayParity& diff)
{
  VQwDataHandler::ConnectChannels(asym, diff);

  // Add independent variables
  for (size_t iv = 0; iv < fIndependentName.size(); iv++) {
    // Get the independent variables
    const VQwHardwareChannel* iv_ptr = 0;
    switch (fIndependentType.at(iv)) {
      case kHandleTypeAsym:
        iv_ptr = asym.ReturnInternalValue(fIndependentName.at(iv));
        break;
      case kHandleTypeDiff:
        iv_ptr = diff.ReturnInternalValue(fIndependentName.at(iv));
        break;
      default:
        QwWarning << "Independent variable for corrector has unknown type."
                  << QwLog::endl;
        break;
    }
    if (iv_ptr) {
      //QwMessage << " iv: " << fIndependentName.at(iv) /*<< " (sens = " << fSensitivity.at(dv).at(iv) << ")"*/ << QwLog::endl;
      fIndependentVar.push_back(iv_ptr);
    } else {
      QwWarning << "Independent variable " << fIndependentName.at(iv) << " could not be found."
                << QwLog::endl;
    }
  }

  QwMessage << "In LRBCorrector::ConnectChannels; Number of IVs: " << fIndependentVar.size()
            << " Number of DVs: " << fDependentVar.size() << QwLog::endl;
  return 0;
}


void LRBCorrector::ProcessData() {
  for (size_t i = 0; i < fDependentVar.size(); ++i) {
    CalcOneOutput(fDependentVar[i], fOutputVar[i], fIndependentVar, fSensitivity[i]);
  }
}

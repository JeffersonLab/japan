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
using namespace std;

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
  file.PopValue("slope-path", outPath);
}

Int_t LRBCorrector::LoadChannelMap(const std::string& mapfile)
{
  string TmpFilePath = run_label.Data();
  fMapFile = "blueR" + TmpFilePath + "new.slope.root";
  string MapFilePath = outPath + "/";
  string tmp = MapFilePath + fMapFile;
  TString corFileName(tmp.c_str());
  QwMessage << "Trying to open " << corFileName << QwLog::endl;
  TFile*  corFile=new TFile(corFileName);
  if( !corFile->IsOpen()) {
    printf("Failed to open %s, slopes NOT found\n",corFile->GetName());
    return 0;
  }

  TMatrixD *alphasM=0;
  alphasM=(TMatrixD *) corFile->Get("slopes");
  assert(alphasM);

  TH1 *dvnames = (TH1 *) corFile->Get("DVname");
  assert(dvnames);
  TH1 *ivnames = (TH1 *) corFile->Get("IVname");
  assert(ivnames);

  pair<EQwHandleType, string> type_name_dv;
  pair<EQwHandleType, string> type_name_iv;

  //  Loop through ivnames to get IV type and name
  //    Loop over # of dep variables
  //      Push-back the sensitiivity, IV type and IVnames into their respective vectors for each DV

  for (Int_t i = 0; i < dvnames->GetXaxis()->GetNbins(); ++i){
    type_name_dv = ParseHandledVariable(dvnames->GetXaxis()->GetBinLabel(i+1));
    fDependentType.push_back(type_name_dv.first);
    fDependentName.push_back(type_name_dv.second);
  }

  fSensitivity.resize(fDependentType.size());

  for (Int_t i = 0; i < ivnames->GetXaxis()->GetNbins(); ++i) {
    type_name_iv = ParseHandledVariable(ivnames->GetXaxis()->GetBinLabel(i+1));
    fIndependentType.push_back(type_name_iv.first);
    fIndependentName.push_back(type_name_iv.second);
    for (Int_t j = 0; j < dvnames->GetXaxis()->GetNbins(); ++j) {
      fSensitivity[j].push_back(-1.0*(*alphasM)(i,j));
    }
  }

  //printf("opened %s, slopes found, dump:\n",corFile->GetName());
  //alphasM->Print();
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
    //QwMessage << "fInpedententType[" << iv << "] = " << fIndependentType.at(iv) 
    //          << "; fInpedententName[" << iv << "] = " << fIndependentName.at(iv)
    //          << QwLog::endl;
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

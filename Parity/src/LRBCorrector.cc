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

//#include "LinReg_Bevington_Pebay.h"
#include "LRBCorrector.h"

// Qweak headers
#include "QwDataHandlerArray.h"
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
#include <TVector.h>

// Register this handler with the factory
RegisterHandlerFactory(LRBCorrector);

/// \brief Constructor with name
LRBCorrector::LRBCorrector(const TString& name):VQwDataHandler(name),
						fLastCycle(0)
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
      fIndependentFull.push_back(current_token);
    }
    else if (primary_token == "dv") {
      fDependentType.push_back(type_name.first);
      fDependentName.push_back(type_name.second);
      fDependentFull.push_back(current_token);
    }
    else if (primary_token == "treetype") {
      QwMessage << "Tree Type read, ignoring." << QwLog::endl;
    }
    else {
      QwError << "Function LoadChannelMap in LRBCorrector.cc read in invalid primary_token." << QwLog::endl;
    }
  }

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

  // Slope matrix
  TKey* key = corFile->GetKey("slopes");
  if (key == 0) {
    QwWarning << "No slope matrix found" << QwLog::endl;
    corFile->Close();
    return 0;
  }
  QwMessage << "Slope matrix has " << key->GetCycle() << " cycle(s)." << QwLog::endl;
  fLastCycle = key->GetCycle(); // last cycle
  for (Short_t cycle=1; cycle<=fLastCycle; cycle++){
    for (Int_t iMat = 0; iMat < nMats ; iMat++) {
      TKey* key_cycle = corFile->GetKey(fMatNames[iMat], cycle);
      TMatrixD *alphasM((TMatrixD *) key_cycle->ReadObj());
      if (alphasM == 0) {
        QwWarning << fMatNames[iMat] << " matrix is null" << QwLog::endl;
        corFile->Close();
        return 0;
      }/*
      if (alphasM->GetNrows() != Int_t(fIndependentType.size()) && alphasM->GetNrows() != Int_t(fDependentType.size())) {
        QwWarning << fMatNames[iMat] << " matrix has wrong number of rows: "
          << alphasM->GetNrows() << " != " << fIndependentType.size() << " or " << fDependentType.size()
          << QwLog::endl;
        corFile->Close();
        return 0;
      }
      if (alphasM->GetNcols() != Int_t(fDependentType.size()) && alphasM->GetNcols() != Int_t(fDependentType.size())) {
        QwWarning << fMatNames[iMat] << " matrix has wrong number of cols: "
          << alphasM->GetNcols() << " != " << fIndependentType.size() << " or " << fDependentType.size()
          << QwLog::endl;
        corFile->Close();
        return 0;
      }*/
      fMats[fMatNames[iMat]].insert({cycle,*alphasM});
    }
    for (Int_t iVec = 0; iVec < nVecs ; iVec++) {
      TKey* key_cycle = corFile->GetKey(fVecNames[iVec], cycle);
      TVectorD *alphasV((TVectorD *) key_cycle->ReadObj());
      if (alphasV == 0) {
        QwWarning << fVecNames[iVec] << " vector is null" << QwLog::endl;
        corFile->Close();
        return 0;
      }
      //if (alphasV->GetNrows() != Int_t(fIndependentType.size()) && alphasV->GetNrows() != Int_t(fDependentType.size())) {
      //  QwWarning << fVecNames[iVec] << " vector has wrong number of rows: "
      //    << alphasV->GetNrows() << " != " << fIndependentType.size() << " or " << fDependentType.size()
      //    << QwLog::endl;
      //  corFile->Close();
      //  return 0;
      //}
      fVecs[fVecNames[iVec]].insert({cycle,*alphasV});
    }
    for (Int_t iStat = 0; iStat < nStats ; iStat++) {
      TKey* key_cycle = corFile->GetKey(fStatNames[iStat], cycle);
      TMatrixD *alphasS((TMatrixD *) key_cycle->ReadObj());
      if (alphasS == 0) {
        QwWarning << fStatNames[iStat] << " vector is null" << QwLog::endl;
        corFile->Close();
        return 0;
      }
      fStats[fStatNames[iStat]].insert({cycle,alphasS->operator()(0,0)});
        //(*alphasS)(0,0);
    }
  }

  fLinRegs.resize(fLastCycle);
  for (Short_t cycle=1; cycle<=fLastCycle; cycle++){
    if (fLastCycle > 1) { // Then we have multiple bursts to look at
      LinRegBevPeb tmpLinReg;
      fLinRegs.push_back(tmpLinReg);
      QwMessage << "We have multiple bursts, filling a set of LinReg objects" << QwLog::endl;
      fillLinRegObject(fLinRegs.at(cycle-1),cycle);
    }
  }
  if ( fLastCycle > GetParent()->GetMaxBurstIndex()+1 ) {
    QwMessage << "Merging the final two bursts, #" << fLastCycle-1 << " and #" << fLastCycle <<QwLog::endl;
    AddTwoBursts(fLastCycle-1,fLastCycle); 
  }

  // Assign slopes after doing the merge above
  QwMessage << "Populating LRBCorrector slopes" << QwLog::endl;
  for (Short_t cycle=1; cycle<=fLastCycle; cycle++){
    // Assign sensitivities
    fSensitivity[cycle].resize(fDependentType.size());
    for (size_t i = 0; i != fDependentType.size(); ++i) {
      fSensitivity[cycle].at(i).resize(fIndependentType.size());
      for (size_t j = 0; j != fIndependentType.size(); ++j) {
        fSensitivity[cycle].at(i).at(j) = -1.0*( fMats.at("slopes").at(cycle).operator()(j,i));
      }
    }
  }

  corFile->Close();
  return 0;
}

void LRBCorrector::fillLinRegObject(LinRegBevPeb& value,Short_t cycle)
{
  //value.nP = fVecs.at("IV_mean").at(cycle).nCols();
  //value.nY = fVecs.at("DV_mean").at(cycle).nCols();
  value.nP = fIndependentName.size();
  value.nY = fDependentName.size();
  value.init();
  // Lowercase p = prime = corrected
  // S = sigma = second moment
  // R = raw correlations
  // M = mean = first moment
  // V = variance = sigma squared
  // P = independent variable
  // Y =   dependent variable
  value.mMP = fVecs.at("IV_mean").at(cycle);
  value.mMY = fVecs.at("DV_mean").at(cycle);
  value.mMYp = fVecs.at("DV_mean_prime").at(cycle);

  value.mVPP = fMats.at("IV_IV_rawVariance").at(cycle);
  value.mVPY = fMats.at("IV_DV_rawVariance").at(cycle);
  value.mVYP.Transpose(value.mVPY);
  value.mVYY = fMats.at("DV_DV_rawVariance").at(cycle);
  value.mVYYp = fMats.at("DV_DV_rawVariance_prime").at(cycle);

  value.mVP = fVecs.at("IV_rawVariance").at(cycle);
  value.mVY = fVecs.at("DV_rawVariance").at(cycle);
  value.mVYp = fVecs.at("DV_rawVariance_prime").at(cycle);

  value.mSPP = fMats.at("IV_IV_normVariance").at(cycle);
  value.mSPY = fMats.at("IV_DV_normVariance").at(cycle);
  value.mSYP.Transpose(value.mSPY);
  value.mSYY = fMats.at("DV_DV_normVariance").at(cycle);
  value.mSYYp = fMats.at("DV_DV_normVariance_prime").at(cycle);

  value.Axy = fMats.at("slopes").at(cycle);
  value.Ayx.Transpose(value.Axy);
  value.dAxy = fMats.at("sigSlopes").at(cycle);
  value.dAyx.Transpose(value.dAxy);

  value.mSP = fVecs.at("IV_sigma").at(cycle);
  value.mSY = fVecs.at("DV_sigma").at(cycle);
  value.mSYp = fVecs.at("DV_sigma_primt").at(cycle);

  value.mRPP = fMats.at("IV_IV_correlation").at(cycle);
  value.mRPY = fMats.at("IV_DV_correlation").at(cycle);
  value.mRYP.Transpose(value.mRPY);
  value.mRYY = fMats.at("DV_DV_correlation").at(cycle);
  value.mRYYp = fMats.at("DV_DV_correlation_prime").at(cycle);

  value.fGoodEventNumber = fStats.at("MyStats").at(cycle);
  value.fErrorFlag = 0;
  //QwMessage << fGoodEventNumber << QwLog::endl;

}

void LRBCorrector::updateFromLinRegs(Short_t cycle)
{
  // Lowercase p = prime = corrected
  // S = sigma = second moment
  // R = raw correlations
  // M = mean = first moment
  // V = variance = sigma squared
  // P = independent variable
  // Y =   dependent variable
  fVecs.at("IV_mean").at(cycle+1) = fLinRegs.at(cycle).mMP;
  fVecs.at("DV_mean").at(cycle+1) = fLinRegs.at(cycle).mMY;
  fVecs.at("DV_mean_prime").at(cycle+1) = fLinRegs.at(cycle).mMYp;

  fMats.at("IV_IV_rawVariance").at(cycle+1) = fLinRegs.at(cycle).mVPP;
  fMats.at("IV_DV_rawVariance").at(cycle+1) = fLinRegs.at(cycle).mVPY;
  fMats.at("DV_DV_rawVariance").at(cycle+1) = fLinRegs.at(cycle).mVYY;
  fMats.at("DV_DV_rawVariance_prime").at(cycle+1) = fLinRegs.at(cycle).mVYYp;

  fVecs.at("IV_rawVariance").at(cycle+1) = fLinRegs.at(cycle).mVP;
  fVecs.at("DV_rawVariance").at(cycle+1) = fLinRegs.at(cycle).mVY;
  fVecs.at("DV_rawVariance_prime").at(cycle+1) = fLinRegs.at(cycle).mVYp;

  fMats.at("IV_IV_normVariance").at(cycle+1) = fLinRegs.at(cycle).mSPP;
  fMats.at("IV_DV_normVariance").at(cycle+1) = fLinRegs.at(cycle).mSPY;
  fMats.at("DV_DV_normVariance").at(cycle+1) = fLinRegs.at(cycle).mSYY;
  fMats.at("DV_DV_normVariance_prime").at(cycle+1) = fLinRegs.at(cycle).mSYYp;

  fMats.at("slopes").at(cycle+1) = fLinRegs.at(cycle).Axy;
  fMats.at("sigSlopes").at(cycle+1) = fLinRegs.at(cycle).dAxy;

  fVecs.at("IV_sigma").at(cycle+1) = fLinRegs.at(cycle).mSP;
  fVecs.at("DV_sigma").at(cycle+1) = fLinRegs.at(cycle).mSY;
  fVecs.at("DV_sigma_prime").at(cycle+1) = fLinRegs.at(cycle).mSYp;

  fMats.at("IV_IV_correlation").at(cycle+1) = fLinRegs.at(cycle).mRPP;
  fMats.at("IV_DV_correlation").at(cycle+1) = fLinRegs.at(cycle).mRPY;
  fMats.at("DV_DV_correlation").at(cycle+1) = fLinRegs.at(cycle).mRYY;
  fMats.at("DV_DV_correlation_prime").at(cycle+1) = fLinRegs.at(cycle).mRYYp;

  fStats.at("MyStats").at(cycle+1) = fLinRegs.at(cycle).fGoodEventNumber;
  //QwMessage << fGoodEventNumber << QwLog::endl;

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
    iv_ptr = this->RequestExternalPointer(fIndependentFull.at(iv));
    if (iv_ptr==NULL){
      switch (fIndependentType.at(iv)) {
      case kHandleTypeAsym:
        iv_ptr = asym.RequestExternalPointer(fIndependentName.at(iv));
        break;
      case kHandleTypeDiff:
        iv_ptr = diff.RequestExternalPointer(fIndependentName.at(iv));
        break;
      default:
        QwWarning << "Independent variable for corrector has unknown type."
                  << QwLog::endl;
        break;
      }
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
            << ", Number of DVs: " << fDependentVar.size() << QwLog::endl;
  return 0;
}


void LRBCorrector::ProcessData() {
  Short_t cycle = fBurstCounter+1;
  if (fSensitivity.count(cycle) == 0) return;
  for (size_t i = 0; i < fDependentVar.size(); ++i) {
    CalcOneOutput(fDependentVar[i], fOutputVar[i], fIndependentVar, fSensitivity[cycle][i]);
  }
}

Int_t LRBCorrector::AddTwoBursts(Short_t i1 = 0, Short_t i2 = 0) {
  if (i1 == i2) { return 0 ; }
  // Else go to the last two bursts and make both equal to the result of adding the two together
  // (because I'm paranoid that the i2th one may still get used somehow, even with max-burst-index set)
  fLinRegs.at(i1) = fLinRegs.at(i1)+fLinRegs.at(i2);
  fLinRegs.at(i1).solve();
  fLinRegs.at(i2) = fLinRegs.at(i1);
  updateFromLinRegs(i1);
  updateFromLinRegs(i2);
  QwMessage << "Merged Bursts LinRebBevPeb object calculated: " << QwLog::endl;
  fLinRegs.at(i1).print();
  return 1;
}

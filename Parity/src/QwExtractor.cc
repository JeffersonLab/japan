/*
 * QwExtractor.cc
 *
 *  Created on: Nov 22, 2019
 *      Author: cameronc137
 *
 */

#include "QwExtractor.h"

// System includes
#include <utility>

// ROOT headers
#include "TFile.h"

// Qweak headers
#include "QwOptions.h"
#include "QwHelicityPattern.h"
#include "VQwDataElement.h"
#include "QwVQWK_Channel.h"
#include "QwParameterFile.h"
#include "QwRootFile.h"

#define MYSQLPP_SSQLS_NO_STATICS
#ifdef __USE_DATABASE__
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__


// Register this handler with the factory
RegisterHandlerFactory(QwExtractor);


/// \brief Constructor with name
QwExtractor::QwExtractor(const TString& name)
: VQwDataHandler(name)
{
  ParseSeparator = ":";
  fKeepRunningSum = kTRUE;
  fTreeName = "bmw";
  fTreeName = "BMOD Extractor";
  fErrorFlagMask = 0x9000;
  fErrorFlagPointer = 0;

}

QwExtractor::QwExtractor(const QwExtractor &source)
: VQwDataHandler(source)
{
  fErrorFlagMask = 0x9000;
  fErrorFlagPointer = 0;
}

/// Destructor
QwExtractor::~QwExtractor() {}

Int_t QwExtractor::LoadChannelMap(const std::string& mapfile) {return 0;}

/** Connect to the dependent and independent channels
 *
 * @param even Helicity event structure
 * @return Zero on success
 */
Int_t QwExtractor::ConnectChannels(QwSubsystemArrayParity& event)
{
  // Keep a pointer to the source Detectors/RingOutput
  //fSourcePointer = &event;
  SetPointer(&event);
  fSourceCopy = QwSubsystemArrayParity(event);
  // Make a copy of RingOutput
  // Normal ConnectChannels for the test variable/ErrorFlag
  // Store error flag pointer
  QwMessage << "Using event error flag" << QwLog::endl;
  fErrorFlagPointer = fSourceCopy.GetEventcutErrorFlagPointer();
  return 0;
}

void QwExtractor::ConstructTreeBranches(
    QwRootFile *treerootfile,
    const std::string& treeprefix,
    const std::string& branchprefix)
{
  // Check if tree name is specified
  if (fTreeName == "") {
    QwWarning << "QwCorrelator: no tree name specified, use 'tree-name = value'" << QwLog::endl;
    return;
  }

  // Construct tree name and create new tree
  const std::string name = treeprefix + fTreeName;
  // Call BuildTree with "bmw" prefix
  treerootfile->NewTree(name, fTreeComment.c_str());
  fTree = treerootfile->GetTree(name);

}

void QwExtractor::ProcessData()
//void QwExtractor::ProcessDataHandlerEntry()
{
  if (fErrorFlagMask!=0 && fErrorFlagPointer!=NULL) {
    if ((*fErrorFlagPointer & fErrorFlagMask)!=0) {
      //QwMessage << "0x" << std::hex << *fErrorFlagPointer << " passed mask " << "0x" << fErrorFlagMask << std::dec << QwLog::endl;
      //CalcOneOutput(fDependentVar[i], fOutputVar[i], fIndependentVar[i], fSensitivity[i]);
      // If test passes then local RingOutput = *fSource one
      fLocalFlag = 1;
      fSourceCopy = QwSubsystemArrayParity(*fSourcePointer);
  //} else {
      //QwMessage << "0x" << std::hex << *fErrorFlagPointer << " failed mask " << "0x" << fErrorFlagMask << std::dec << QwLog::endl;
    }
  }
  else{
    //CalcOneOutput(fDependentVar[i], fOutputVar[i], fIndependentVar[i], fSensitivity[i]);
    // If test passes then local RingOutput = *fSource one
    fLocalFlag = 1;
    fSourceCopy = QwSubsystemArrayParity(*fSourcePointer);
  }
}

//void QwExtractor::ProcessData() {}

void QwExtractor::FillTreeBranches(QwRootFile *treerootfile)
{
  if (fTreeName.size()>0 && fLocalFlag ==1 ){
    treerootfile->FillTreeBranches(fSourceCopy);
    treerootfile->FillTree(fTreeName);
  }
}


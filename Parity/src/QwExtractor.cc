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
  fTreeName = "";
  fTreeComment = "Extractor";
  fCut = 0;
  fErrorFlagMask = 0x0;
  fErrorFlagPointer = 0;

}

QwExtractor::QwExtractor(const QwExtractor &source)
: VQwDataHandler(source)
{
  fCut = 0;
  fErrorFlagMask = 0x9000;
  fErrorFlagPointer = 0;
}

/// Destructor
QwExtractor::~QwExtractor() {delete fSourceCopy;}

void QwExtractor::ParseConfigFile(QwParameterFile& file)
{
  VQwDataHandler::ParseConfigFile(file);
  file.PopValue("cut-logic",    fCut);
  file.PopValue("tree-name",    fTreeName);
  file.PopValue("branch-prefix",fBranchprefix);
  file.PopValue("tree-prefix",  fTreeprefix);
  file.PopValue("tree-comment", fTreeComment);
  file.PopValue("error-mask",   fErrorFlagMask);

}

Int_t QwExtractor::LoadChannelMap(const std::string& mapfile) {return 0;}

/** Connect to the dependent and independent channels
 *
 * @param even Helicity event structure
 * @return Zero on success
 */

Int_t QwExtractor::ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff)
{
  // Keep a pointer to the source Detectors/RingOutput
  //fSourcePointer = &event;
  SetPointer(&asym);
  fSourceCopy = new QwSubsystemArrayParity(asym);
  // Make a copy of RingOutput
  // Normal ConnectChannels for the test variable/ErrorFlag
  // Store error flag pointer
  QwMessage << "Using event error flag" << QwLog::endl;
  fErrorFlagPointer = asym.GetEventcutErrorFlagPointer();
  return 0;
}

Int_t QwExtractor::ConnectChannels(QwSubsystemArrayParity& event)
{
  // Keep a pointer to the source Detectors/RingOutput
  //fSourcePointer = &event;
  SetPointer(&event);
  fSourceCopy = new QwSubsystemArrayParity(event);
  // Make a copy of RingOutput
  // Normal ConnectChannels for the test variable/ErrorFlag
  // Store error flag pointer
  QwMessage << "Using event error flag" << QwLog::endl;
  fErrorFlagPointer = event.GetEventcutErrorFlagPointer();
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
  if (treeprefix!="") { fTreeprefix = treeprefix; }
  fTreeName = fTreeprefix + fTreeName;
  treerootfile->ConstructTreeBranches(fTreeName, fTreeComment.c_str(), *fSourceCopy);
  //fTree = treerootfile->GetTree(fTreeName);
}

void QwExtractor::ProcessData()
{
  fLocalFlag = 0;
  if (fErrorFlagPointer!=NULL) {
    if (fCut==1 && (*fErrorFlagPointer & fErrorFlagMask)!=0) {
      // We are doing a == test, selecting activated bits
      //QwMessage << "Cut in, 0x" << std::hex << *fErrorFlagPointer << " passed mask " << "0x" << std::hex << fErrorFlagMask << std::dec << QwLog::endl;
      fLocalFlag = 1;
      fSourceCopy->operator=(*fSourcePointer);
    }
    else if (fCut==0 && (*fErrorFlagPointer & fErrorFlagMask)==0) {
      // We are doing an != test, not selecting activated bits
      //QwMessage << "Cut out, 0x" << std::hex << *fErrorFlagPointer << " passed mask " << "0x" << std::hex << fErrorFlagMask << std::dec << QwLog::endl;
      fLocalFlag = 1;
      fSourceCopy->operator=(*fSourcePointer);
    }
  // else {
  //    QwMessage << "0x" << std::hex << *fErrorFlagPointer << " failed mask " << "0x" << std::hex << fErrorFlagMask << std::dec << QwLog::endl;
  //  }
  }
  else{
    // Trivial case for safety
    fLocalFlag = 1;
    fSourceCopy->operator=(*fSourcePointer);
  }
}

void QwExtractor::FillTreeBranches(QwRootFile *treerootfile)
{
  if (fTreeName.size()>0 && fLocalFlag == 1 ){
    //QwMessage << fLocalFlag << " passed mask " << "0x" << std::hex<< fErrorFlagMask << std::dec << QwLog::endl;
    treerootfile->FillTreeBranches(*fSourceCopy);
    treerootfile->FillTree(fTreeName);
  }
  //else {
  //  QwMessage << fLocalFlag << " failed mask " << "0x" << fErrorFlagMask << std::dec << QwLog::endl;
  //}
}


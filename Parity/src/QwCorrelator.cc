/********************************************************************
File Name: QwCorrelator.cc

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This is the implementation file of the QwCorrelator
              class, which is a child of the VQwDataHandler class.
              The functionality of this class is derived from
              LinRegBlue.

Last Modified: August 1, 2018 1:43 PM
********************************************************************/

#include "QwCorrelator.h"

// System includes
#include <utility>

// ROOT headers
#include "TFile.h"
#include "TH2D.h"

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
RegisterHandlerFactory(QwCorrelator);

// Static members
bool QwCorrelator::fPrintCorrelations = false;

QwCorrelator::QwCorrelator(const TString& name)
: VQwDataHandler(name),
  fBlock(-1),
  fDisableHistos(true),
  fAlphaOutputFileBase("blueR"),
  fAlphaOutputFileSuff("new.slope.root"),
  fAlphaOutputPath("."),
  fAlphaOutputFile(0),
  fTree(0),
  fAliasOutputFileBase("regalias_"),
  fAliasOutputFileSuff(""),
  fAliasOutputPath("."),
  nP(0),nY(0),
  fH1iv(0),fH1dv(0),
  fH2iv(0),fH2dv(0)
{
  ParseSeparator = "_";
  fTotalCount = 0;
  fGoodCount  = 0;
  fGoodEvent = 0;
  fErrCounts_EF = 0;
}

QwCorrelator::~QwCorrelator()
{
  // Close output file
  if (fAlphaOutputFile) {
    fAlphaOutputFile->Write();
    fAlphaOutputFile->Close();
  }

  // Delete histograms if previously allocated
  if (fH1iv) {
    delete[] fH1iv;
    delete[] fH2iv;
    delete[] fH1dv;
    delete[] fH2dv;
  }
}

void QwCorrelator::DefineOptions(QwOptions &options)
{
  options.AddOptions()("print-correlations",
      po::value<bool>(&fPrintCorrelations)->default_bool_value(false),
      "print correlations after determining them");
}

void QwCorrelator::ProcessOptions(QwOptions &options)
{
}

void QwCorrelator::ParseConfigFile(QwParameterFile& file)
{
  VQwDataHandler::ParseConfigFile(file);
  file.PopValue("slope-file-base", fAlphaOutputFileBase);
  file.PopValue("slope-file-suff", fAlphaOutputFileSuff);
  file.PopValue("slope-path", fAlphaOutputPath);
  file.PopValue("alias-file-base", fAliasOutputFileBase);
  file.PopValue("alias-file-suff", fAliasOutputFileSuff);
  file.PopValue("alias-path", fAliasOutputPath);
  file.PopValue("disable-histos", fDisableHistos);
  file.PopValue("block", fBlock);
  if (fBlock >= 4)
    QwWarning << "QwCorrelator: expect 0 <= block <= 3 but block = "
              << fBlock << QwLog::endl;
}

void QwCorrelator::ProcessData()
{
  // Add to total count
  fTotalCount++;

  // Start as good event
  fGoodEvent = 0;

  // Event error flag
  fGoodEvent |= GetEventcutErrorFlag();
  if ( GetEventcutErrorFlag() != 0) fErrCounts_EF++;
  // Dependent variable error codes
  for (size_t i = 0; i < fDependentVar.size(); ++i) {
    fGoodEvent |= fDependentVar.at(i)->GetErrorCode();
    fDependentValues.at(i) = (fDependentVar[i]->GetValue(fBlock+1));
    if (fDependentVar.at(i)->GetErrorCode() !=0)  (fErrCounts_DV.at(i))++;
  }
  // Independent variable error codes
  for (size_t i = 0; i < fIndependentVar.size(); ++i) {
    fGoodEvent |= fIndependentVar.at(i)->GetErrorCode();
    fIndependentValues.at(i) = (fIndependentVar[i]->GetValue(fBlock+1));
    if (fIndependentVar.at(i)->GetErrorCode() !=0)  (fErrCounts_IV.at(i))++;
  }

  // If good, process event
  if (fGoodEvent == 0) {
    fGoodCount++;

    TVectorD P(fIndependentValues.size(), fIndependentValues.data());
    TVectorD Y(fDependentValues.size(),   fDependentValues.data());
    linReg += std::make_pair(P, Y);
  }
}

void QwCorrelator::ClearEventData()
{
  linReg.clear();
}

void QwCorrelator::AccumulateRunningSum(VQwDataHandler &value, Int_t count, Int_t ErrorMask)
{
  QwCorrelator* correlator = dynamic_cast<QwCorrelator*>(&value);
  if (correlator) {
    linReg += correlator->linReg;
  } else {
    QwWarning << "QwCorrelator::AccumulateRunningSum "
              << "can only accept other QwCorrelator objects."
              << QwLog::endl;
  }
}

void QwCorrelator::CalcCorrelations()
{
  // Print entry summary
  QwVerbose << "QwCorrelator: "
            << "total entries: " << fTotalCount << ", "
            << "good entries: " << fGoodCount
            << QwLog::endl;
  // and warn if zero
  if (fTotalCount > 100 && fGoodCount == 0) {
    QwWarning << "QwCorrelator: "
              << "< 1% good events, "
              << fGoodCount << " of " << fTotalCount
              << QwLog::endl;
  }

  // Event error flag
  if (fErrCounts_EF > 0) {
    QwVerbose << "   Entries failed due to error flag: "
              << fErrCounts_EF << QwLog::endl;
  }
  // Dependent variable error codes
  for (size_t i = 0; i < fDependentVar.size(); ++i) {
    if (fErrCounts_DV.at(i) > 0) {
      QwVerbose << "   Entries failed due to " << fDependentVar.at(i)->GetElementName()
                << ": " <<  fErrCounts_DV.at(i) << QwLog::endl;
    }
  }
  // Independent variable error codes
  for (size_t i = 0; i < fIndependentVar.size(); ++i) {
    if (fErrCounts_IV.at(i) > 0) {
      QwVerbose << "   Entries failed due to " << fIndependentVar.at(i)->GetElementName()
                << ": " <<  fErrCounts_IV.at(i) << QwLog::endl;
    }
  }

  if (! linReg.failed()) {

    if (fPrintCorrelations) {
      linReg.printSummaryP();
      linReg.printSummaryY();
    }

    linReg.solve();

    if (fPrintCorrelations) {
      linReg.printSummaryAlphas();
      linReg.printSummaryMeansWithUnc();
      linReg.printSummaryMeansWithUncCorrected();
    }
  }

  // Fill tree
  fTree->Fill();

  // Write alpha file
  std::string SlopeFileName = fAlphaOutputFileBase + run_label.Data() + fAlphaOutputFileSuff;
  std::string SlopeFilePath = fAlphaOutputPath + "/";
  std::string SlopeFile = SlopeFilePath + SlopeFileName;
  exportAlphas(fIndependentFull, fDependentFull);

  // Write macro file
  std::string MacroFileName = fAliasOutputFileBase + run_label.Data() + fAliasOutputFileSuff;
  std::string MacroFilePath = fAliasOutputPath + "/";
  exportAlias(TString(MacroFilePath), TString(MacroFileName), fIndependentFull, fDependentFull);
}


/** Load the channel map
 *
 * @param mapfile Filename of map file
 * @return Zero when success
 */
Int_t QwCorrelator::LoadChannelMap(const std::string& mapfile)
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
      QwError << "LoadChannelMap in QwCorrelator read invalid primary_token " << primary_token << QwLog::endl;
    }
  }

  return 0;
}


Int_t QwCorrelator::ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff)
{
  SetEventcutErrorFlagPointer(asym.GetEventcutErrorFlagPointer());

  // Return if correlator is not enabled

  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fDependentName.size(); dv++) {
    // Get the dependent variables

    VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    QwVQWK_Channel* vqwk = NULL;
    string name = "";
    string reg = "reg_";
    
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
		                << ", for asym/diff correlator does not have proper type, type=="
		                << fDependentType.at(dv) << "."<< QwLog::endl;
          break;
        }

      vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
      name = vqwk->GetElementName().Data();
      name.insert(0, reg);
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
    if(vqwk != NULL){
      // fDependentVarType.push_back(fDependentType.at(dv));
      fDependentVar.push_back(vqwk);
    }

  }
  
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
        QwWarning << "Independent variable for correlator has unknown type."
                  << QwLog::endl;
        break;
    }
    if (iv_ptr) {
      fIndependentVar.push_back(iv_ptr);
    } else {
      QwWarning << "Independent variable " << fIndependentName.at(iv) << " for correlator could not be found."
                << QwLog::endl;
    }
      
  }
  fIndependentValues.resize(fIndependentVar.size());
  fDependentValues.resize(fDependentVar.size());
 
  init(fIndependentName, fDependentName);

  fErrCounts_IV.resize(fIndependentVar.size(),0);
  fErrCounts_DV.resize(fDependentVar.size(),0);

  return 0;
}


void QwCorrelator::init(std::vector<std::string> ivName, std::vector<std::string> dvName)
{
  // Create ROOT file
  std::string SlopeFileName = fAlphaOutputFileBase + run_label.Data() + fAlphaOutputFileSuff;
  std::string SlopeFilePath = fAlphaOutputPath + "/";
  std::string SlopeFile = SlopeFilePath + SlopeFileName;

  fAlphaOutputFile = new TFile(SlopeFile.c_str(), "RECREATE", "correlation coefficients");
  if (! fAlphaOutputFile->IsWritable()) {
    QwError << "QwCorrelator could not create output file " << SlopeFile << QwLog::endl;
    delete fAlphaOutputFile;
    fAlphaOutputFile = 0;
  }

  nP = ivName.size();
  nY = dvName.size();

  linReg.setDims(nP, nY);
  linReg.init();
}

void QwCorrelator::ConstructTreeBranches(
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
  treerootfile->NewTree(name, fTreeComment.c_str());
  fTree = treerootfile->GetTree(name);

  // Set up branches
  fTree->Branch(TString(branchprefix + "total_count"), &fTotalCount);
  fTree->Branch(TString(branchprefix + "good_count"),  &fGoodCount);

  fTree->Branch(TString(branchprefix + "n"), &(linReg.fGoodEventNumber));
  fTree->Branch(TString(branchprefix + "ErrorFlag"), &(linReg.fErrorFlag));

  fTree->Branch(TString(branchprefix + "A"),    "TMatrixD", &(linReg.mA));
  fTree->Branch(TString(branchprefix + "dA"),   "TMatrixD", &(linReg.mAsig));

  fTree->Branch(TString(branchprefix + "VPP"),  "TMatrixD", &(linReg.mVPP));
  fTree->Branch(TString(branchprefix + "VPY"),  "TMatrixD", &(linReg.mVPY));
  fTree->Branch(TString(branchprefix + "VYY"),  "TMatrixD", &(linReg.mVYY));
  fTree->Branch(TString(branchprefix + "VYYp"), "TMatrixD", &(linReg.mVYYprime));

  fTree->Branch(TString(branchprefix + "SPP"),  "TMatrixD", &(linReg.sigXX));
  fTree->Branch(TString(branchprefix + "SPY"),  "TMatrixD", &(linReg.sigXY));
  fTree->Branch(TString(branchprefix + "SYY"),  "TMatrixD", &(linReg.sigYY));
  fTree->Branch(TString(branchprefix + "SYYp"), "TMatrixD", &(linReg.sigYYprime));

  fTree->Branch(TString(branchprefix + "RPP"),  "TMatrixD", &(linReg.mRPP));
  fTree->Branch(TString(branchprefix + "RPY"),  "TMatrixD", &(linReg.mRPY));
  fTree->Branch(TString(branchprefix + "RYY"),  "TMatrixD", &(linReg.mRYY));
  fTree->Branch(TString(branchprefix + "RYYp"), "TMatrixD", &(linReg.mRYYprime));

  fTree->Branch(TString(branchprefix + "MP"),   "TVectorD", &(linReg.mMP));
  fTree->Branch(TString(branchprefix + "MY"),   "TVectorD", &(linReg.mMY));
  fTree->Branch(TString(branchprefix + "MYp"),  "TVectorD", &(linReg.mMYprime));

  fTree->Branch(TString(branchprefix + "dMP"),   "TVectorD", &(linReg.sigX));
  fTree->Branch(TString(branchprefix + "dMY"),   "TVectorD", &(linReg.sigY));
  fTree->Branch(TString(branchprefix + "dMYp"),  "TVectorD", &(linReg.sigYprime));
}

/// \brief Construct the histograms in a folder with a prefix
void QwCorrelator::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  // Skip if disabled
  if (fDisableHistos) return;

  // Go to directory
  TString name(fName);
  name.ReplaceAll(" ","_");
  folder->mkdir(name)->cd();

  //..... 1D,  iv
  fH1iv = new TH1D*[nP];
  for(int i=0;i<nP;i++) {
    TH1D* h = fH1iv[i] = new TH1D(
        Form("P%d",i),
        Form("iv P%d=%s, pass=%s ;iv=%s (ppm)",i,fIndependentName[i].c_str(),fName.Data(),fIndependentName[i].c_str()),
        128,0.,0.);
    h->GetXaxis()->SetNdivisions(4);
  }

  //..... 2D,  iv correlations
  Double_t x1 = 0;
  fH2iv = new TH2D*[nP*nP]; // not all are used
  for(int i=0;i<nP;i++) {
    for(int j=i+1;j<nP;j++) {
      TH2D* h = fH2iv[i*nP+j] = new TH2D(
          Form("P%d_P%d",i,j),
          Form("iv correlation  P%d_P%d, pass=%s ;P%d=%s (ppm);P%d=%s   (ppm)  ",
              i,j,fName.Data(),i,fIndependentName[i].c_str(),j,fIndependentName[j].c_str()),
          64,-x1,x1,
          64,-x1,x1);
      h->GetXaxis()->SetTitleColor(kBlue);
      h->GetYaxis()->SetTitleColor(kBlue);
      h->GetXaxis()->SetNdivisions(4);
      h->GetYaxis()->SetNdivisions(4);
    }
  }

  //..... 1D,  dv
  fH1dv = new TH1D*[nY];
  for(int i=0;i<nY;i++) {
    TH1D* h = fH1dv[i] = new TH1D(
        Form("Y%d",i),
        Form("dv Y%d=%s, pass=%s ;dv=%s (ppm)",i,fDependentName[i].c_str(),fName.Data(),fDependentName[i].c_str()),
        128,0.,0.);
    h->GetXaxis()->SetNdivisions(4);
  }

  //..... 2D,  dv-iv correlations
  Double_t y1 = 0;
  fH2dv = new TH2D*[nP*nY]; // not all are used
  for(int i=0;i<nP;i++) {
    for(int j=0;j<nY;j++) {
      TH2D* h = fH2dv[i*nY+j] = new TH2D(
          Form("P%d_Y%d",i,j),
          Form("iv-dv correlation  P%d_Y%d, pass=%s ;P%d=%s (ppm);Y%d=%s   (ppm)  ",
              i,j,fName.Data(),i,fIndependentName[i].c_str(),j,fDependentName[j].c_str()),
          64,-x1,x1,
          64,-y1,y1);
      h->GetXaxis()->SetTitleColor(kBlue);
      h->GetYaxis()->SetTitleColor(kBlue);
      h->GetXaxis()->SetNdivisions(4);
      h->GetYaxis()->SetNdivisions(4);
    }
  }

  // store list of names to be archived
  hA[0] = new TH1D("NamesIV",Form("IV name list nIV=%d",nP),nP,0,1);
  for(int i=0;i<nP;i++)
    hA[0]->Fill(fIndependentName[i].c_str(),1.*i);
  hA[1] = new TH1D("NamesDV",Form("DV name list nIV=%d",nY),nY,0,1);
  for(int i=0;i<nY;i++)
    hA[1]->Fill(fDependentName[i].c_str(),i*1.);
}

/// \brief Fill the histograms
void QwCorrelator::FillHistograms()
{
  // Skip if disabled
  if (fDisableHistos) return;

  // Skip if bad event
  if (fGoodEvent != 0) return;

  // Fill histograms
  for (size_t i = 0; i < fIndependentValues.size(); i++) {
    fH1iv[i]->Fill(fIndependentValues[i]);
    for (size_t j = i+1; j < fIndependentValues.size(); j++)
      fH2iv[i*fIndependentValues.size()+j]->Fill(fIndependentValues[i], fIndependentValues[j]);
  }
  for (size_t j = 0; j < fDependentValues.size(); j++) {
    fH1dv[j]->Fill(fDependentValues[j]);
    for (size_t i = 0; i < fDependentValues.size(); i++)
      fH2dv[i*fDependentValues.size()+j]->Fill(fIndependentValues[i], fDependentValues[j]);
  }
}

void QwCorrelator::exportAlphas(
    std::vector < TString > ivName,
    std::vector < TString > dvName)
{
  // Ensure in output file
  if (fAlphaOutputFile) fAlphaOutputFile->cd();

  // Write objects
  linReg.mA.Write("slopes");
  linReg.mAsig.Write("sigSlopes");

  linReg.mRPP.Write("IV_IV_correlation");
  linReg.mRPY.Write("IV_DV_correlation");
  linReg.mRYY.Write("DV_DV_correlation");
  linReg.mRYYprime.Write("DV_DV_correlation_prime");

  linReg.mMP.Write("IV_mean");
  linReg.mMY.Write("DV_mean");
  linReg.mMYprime.Write("DV_mean_prime");

  // number of events
  TMatrixD Mstat(1,1);
  Mstat(0,0)=linReg.getUsedEve();
  Mstat.Write("MyStat");

  //... IVs
  TH1D hiv("IVname","names of IVs",nP,-0.5,nP-0.5);
  for (int i=0;i<nP;i++) hiv.Fill(ivName[i],i);
  hiv.Write();

  //... DVs
  TH1D hdv("DVname","names of IVs",nY,-0.5,nY-0.5);
  for (int i=0;i<nY;i++) hdv.Fill(dvName[i],i);
  hdv.Write();

  // sigmas
  linReg.sigX.Write("IV_sigma");
  linReg.sigY.Write("DV_sigma");
  linReg.sigYprime.Write("DV_sigma_prime");

  // raw covariances
  linReg.mVPP.Write("IV_IV_rawVariance");
  linReg.mVPY.Write("IV_DV_rawVariance");
  linReg.mVYY.Write("DV_DV_rawVariance");
  linReg.mVYYprime.Write("DV_DV_rawVariance_prime");
  TVectorD mVY2(TMatrixDDiag(linReg.mVYY));
  mVY2.Write("DV_rawVariance");
  TVectorD mVP2(TMatrixDDiag(linReg.mVPP));
  mVP2.Write("IV_rawVariance");
  TVectorD mVY2prime(TMatrixDDiag(linReg.mVYYprime));
  mVY2prime.Write("DV_rawVariance_prime");

  // normalized covariances
  linReg.sigXX.Write("IV_IV_normVariance");
  linReg.sigXY.Write("IV_DV_normVariance");
  linReg.sigYY.Write("DV_DV_normVariance");
  linReg.sigYYprime.Write("DV_DV_normVariance_prime");
  TVectorD sigY2(TMatrixDDiag(linReg.sigYY));
  sigY2.Write("DV_normVariance");
  TVectorD sigX2(TMatrixDDiag(linReg.sigXX));
  sigX2.Write("IV_normVariance");
  TVectorD sigY2prime(TMatrixDDiag(linReg.sigYYprime));
  sigY2prime.Write("DV_normVariance_prime");

  linReg.Axy.Write("A_xy");
  linReg.Ayx.Write("A_yx");
}


void QwCorrelator::exportAlias(
    TString outPath,
    TString macroName,
    std::vector < TString > Pname,
    std::vector < TString > Yname)
{
  FILE *fd=fopen(outPath+macroName+".C","w");
  if (fd == 0) {
    QwError << "QwCorrelator could not create alias file " << outPath+macroName+".C" << QwLog::endl;
    return;
  }
  fprintf(fd,"void %s() {\n",macroName.Data());
  fprintf(fd,"  TTree* tree = (TTree*) gDirectory->Get(\"mul\");\n");
  for (int iy = 0; iy <nY; iy++) {
    fprintf(fd,"  tree->SetAlias(\"reg_%s\",\n         \"%s",Yname[iy].Data(),Yname[iy].Data());
    for (int j = 0; j < nP; j++) {
      double val= -linReg.mA(j,iy);
      if(val>0)  fprintf(fd,"+");
      fprintf(fd,"%.4e*%s",val,Pname[j].Data());
    }
    fprintf(fd,"\");\n");

  }
  fprintf(fd,"}\n");
  fclose(fd);
}


#ifndef __CAMREG__
#define __CAMREG__
#include "../camguin.hh"
using namespace std;
void regress_h(TString tree = "mul", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = 0, Int_t nRuns = -1, TString regInput = "regressionInput.txt", char delim = ' ', TString filename = "NULL"){
  Double_t speed = 0.66;
  TString fit = "parity";
  Double_t nonLinearFit = 0.0; // 1.0 = nonLinear fit with fit parameter uncertaintites included in weight
  Int_t passLimitValue = 1;
  Double_t fitCriteria = 0.01;
  Double_t parAvg = 10.0;
  Double_t parameterLimitRMS = 0.000001;
  Double_t parameterLimitValue = 2.0;
  Double_t parRms2 = 10.0;
  Int_t parameterLimits = 0;

  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  vector<vector<string>> textFile = textFileParse_h(regInput,delim);
  TTree * oldTree;
  TChain *chain = new TChain(tree);
  TFile * candidateFile = new TFile(filename.Data(),"READ");
  if (filename != "NULL" && candidateFile->GetListOfKeys()->Contains(tree)){
    chain->Add(filename);
    oldTree = chain;
  }
  else {
    oldTree = getTree_h(tree, runNumber, minirunNumber, splitNumber, nRuns, "NULL");
  } 
  if (oldTree==0){
    Printf("Root file does not exist");
    return;
  }
  TTreeReader oldTreeReader(oldTree);
  TTree * newTree = new TTree("reg"+tree,"Regressed "+tree+" tree");
  TFile * outFile = new TFile(Form("outputReg_%s_%d.%d.%03d.root",(const char*)tree,runNumber,minirunNumber,splitNumber),"RECREATE");
  TDirectory *folder = outFile->mkdir("histos_"+tree);
  outFile->cd();
  gSystem->Exec("mkdir plots");

  Double_t data     = 0.0;
  Int_t    n_data   = 0;
  vector<Double_t> newRegressedValues;
  vector<Double_t> parameters;
  vector<Double_t> parametersLowerLimit;
  vector<Double_t> parametersUpperLimit;
  vector<Double_t> weighting;
  vector<Double_t> oldManipulatedValues;
  vector<Double_t> oldManipulatedErrors;
  vector<Double_t> oldManipulatedUncertainties;
  vector<Double_t> oldRespondingUncertainties;
  //vector<Double_t> oldRespondingValues;
  //vector<Double_t> oldRespondingErrors;
  vector<TString> newRegressedBranchList;
  vector<TString> oldManipulatedDataBranchList;
  vector<TString> oldManipulatedErrorBranchList;
  vector<TString> oldManipulatedUncertaintiesBranchList;
  vector<TString> oldRespondingDataBranchList;
  vector<TString> oldRespondingErrorBranchList;
  vector<TString> oldRespondingUncertaintiesBranchList;
  TString errorBranchName  = "ErrorFlag"; // == 0 means no error, means its ok
  Double_t oldTreeErrorFlag = 0.0;
  TString okFlagReg  = "ok_cut"; // == 1 means true, means its ok
  Double_t newRegressedValuesOkCut = 1.0;
  Bool_t resp  = false;
  Int_t nresp  = 0;
  Int_t fitN = 0;
  Bool_t manip = false;
  Int_t nmanip = 0;
  Int_t nmanipInputs = 0;
  if (debug > -1) Printf("Data structures initialized");
  for (UInt_t listEntryN = 0; listEntryN < textFile.size(); listEntryN++){
    if (textFile[listEntryN][0] == "Global"){
      if (debug > 2) Printf("Branch %s",textFile[listEntryN][1].c_str());
      errorBranchName = textFile[listEntryN][1];
      resp = false;
      manip = false;
      continue;
    }
    if (textFile[listEntryN][0] == "Speed"){
      speed = stof(textFile[listEntryN][1]);
      continue;
    }
    if (textFile[listEntryN][0] == "Parameter-Limits"){
      parameterLimits = stod(textFile[listEntryN][1]);
      continue;
    }
    if (textFile[listEntryN][0] == "Fit-Type"){
      fit = textFile[listEntryN][1];
      continue;
    }
    if (textFile[listEntryN][0] == "Non-Linear-Fit"){
      nonLinearFit = stof(textFile[listEntryN][1]);
      continue;
    }
    if (textFile[listEntryN][0] == "Fit-Pass-Limit"){
      passLimitValue = (Int_t)stof(textFile[listEntryN][1]);
      continue;
    }
    if (textFile[listEntryN][0] == "Fit-Stability-Criteria"){
      fitCriteria = stof(textFile[listEntryN][1]);
      continue;
    }
    if (textFile[listEntryN][0] == "Fit-Runaway-Scale-RMS-Criteria"){
      parameterLimitRMS = stof(textFile[listEntryN][1]);
      continue;
    }
    if (textFile[listEntryN][0] == "Fit-Runaway-Scale-Value-Criteria"){
      parameterLimitValue = stof(textFile[listEntryN][1]);
      continue;
    }
    if (textFile[listEntryN][0] == "Responding"){
      resp = true;
      manip = false;
      continue;
    }
    if (textFile[listEntryN][0] == "Manipulated"){
      manip = true;
      resp = false;
      continue;
    }
    if (resp){
      if (debug > 2) Printf("Responding Branch %s",textFile[listEntryN][0].c_str());
      if (textFile[listEntryN][1]!="NULL"){
        oldRespondingDataBranchList.push_back(textFile[listEntryN][0]+textFile[listEntryN][1]);
      }
      else {
        oldRespondingDataBranchList.push_back(textFile[listEntryN][0]);
      }
      //oldRespondingValues.push_back(0.0); 
      if (textFile[listEntryN][2]!="NULL"){
        oldRespondingErrorBranchList.push_back(textFile[listEntryN][0]+textFile[listEntryN][2]);
      }
      else {
        oldRespondingErrorBranchList.push_back(errorBranchName);
      }
      //oldRespondingErrors.push_back(0.0); 
      newRegressedBranchList.push_back("reg_"+textFile[listEntryN][0]);
      newRegressedValues.push_back(0.0); 
      listEntryN++; // Read next line for uncertainty data
      if (textFile[listEntryN][0]=="File" && textFile[listEntryN][2]!="NULL"){
        oldRespondingUncertaintiesBranchList.push_back(textFile[listEntryN][1]+textFile[listEntryN][2]);
        oldRespondingUncertainties.push_back(0.0);
      }
      else if (textFile[listEntryN][0]=="File" && textFile[listEntryN][2]=="NULL") {
        oldRespondingUncertaintiesBranchList.push_back(textFile[listEntryN][1]);
        oldRespondingUncertainties.push_back(0.0);
      }
      else if (textFile[listEntryN][0]=="User"){
        oldRespondingUncertaintiesBranchList.push_back(textFile[listEntryN][0]);
        oldRespondingUncertainties.push_back(stof(textFile[listEntryN][1].c_str()));
      }
      nresp++;
    }
    if (manip){
      if (debug > 2) Printf("Manipulated Branch %s",textFile[listEntryN][0].c_str());
      if (textFile[listEntryN][1]!="NULL"){
        oldManipulatedDataBranchList.push_back(textFile[listEntryN][0]+textFile[listEntryN][1]);
      }
      else {
        oldManipulatedDataBranchList.push_back(textFile[listEntryN][0]);
      }
      if (textFile[listEntryN][0] == "constant"){
        oldManipulatedValues.push_back(1.0); // The physics asymmetry is asymmetry*1 (the number 1 is the type of value it is, an exact scalar)
        oldManipulatedErrors.push_back(0.0); // The physics asymmetry applies for all Global Cuts passing entries
        parameters.push_back(stof(textFile[listEntryN][3])); // Push back the physics asymmetry value placeholder at nmanip+1 position... assume it is trivially 0 for first pass
        weighting.push_back(stof(textFile[listEntryN][4])); // Push back the physics asymmetry relative weighting factor for uncertainty calculations
        if (parameterLimits) {
          parametersLowerLimit.push_back(stof(textFile[listEntryN][5])); 
          parametersUpperLimit.push_back(stof(textFile[listEntryN][6])); 
        }
        nmanipInputs=nmanip; // This is our constant term
      }
      else {
        oldManipulatedValues.push_back(0.0); 
        if (textFile[listEntryN][2]!="NULL"){
          oldManipulatedErrorBranchList.push_back(textFile[listEntryN][0]+textFile[listEntryN][2]);
        }
        else {
          oldManipulatedErrorBranchList.push_back(errorBranchName);
        }
        oldManipulatedErrors.push_back(0.0); 
        parameters.push_back(stof(textFile[listEntryN][3])); // Initial parameter correlation slope guess for iterating fit
        weighting.push_back(stof(textFile[listEntryN][4])); // Initial weighting guess for errors
        if (parameterLimits) {
          parametersLowerLimit.push_back(stof(textFile[listEntryN][5])); 
          parametersUpperLimit.push_back(stof(textFile[listEntryN][6])); 
        }
      }
      listEntryN++; // Read next line for uncertainty data
      if (textFile[listEntryN][0]=="File" && textFile[listEntryN][2]!="NULL") {
        oldManipulatedUncertaintiesBranchList.push_back(textFile[listEntryN][1]+textFile[listEntryN][2]);
        oldManipulatedUncertainties.push_back(0.0);
      }
      else if (textFile[listEntryN][0]=="File" && textFile[listEntryN][2]=="NULL") {
        oldManipulatedUncertaintiesBranchList.push_back(textFile[listEntryN][1]);
        oldManipulatedUncertainties.push_back(0.0);
      }
      else if (textFile[listEntryN][0]=="User"){
        oldManipulatedUncertaintiesBranchList.push_back(textFile[listEntryN][0]);
        oldManipulatedUncertainties.push_back(stof(textFile[listEntryN][1].c_str()));
      }
      nmanip++;
    }
    if (debug > 1) Printf("Branch %d initialized",listEntryN);
  }

  vector<TTreeReaderValue<Double_t>> oldRespondingValuesReader;
  vector<TTreeReaderValue<Double_t>> oldRespondingErrorsReader;
  vector<TTreeReaderValue<Double_t>> oldRespondingUncertaintiesReader;
  vector<TTreeReaderValue<Double_t>> oldManipulatedValuesReader;
  vector<TTreeReaderValue<Double_t>> oldManipulatedErrorsReader;
  vector<TTreeReaderValue<Double_t>> oldManipulatedUncertaintiesReader;

  for(Int_t iBranch = 0; iBranch < oldManipulatedDataBranchList.size(); iBranch++) {
    if (oldManipulatedDataBranchList[iBranch]!="constant"){
      TTreeReaderValue<Double_t> temp1(oldTreeReader,oldManipulatedDataBranchList[iBranch]);
      TTreeReaderValue<Double_t> temp2(oldTreeReader,oldManipulatedErrorBranchList[iBranch]);
      if (oldManipulatedUncertaintiesBranchList[iBranch]=="User"){
        TTreeReaderValue<Double_t> temp3(oldTreeReader,oldManipulatedDataBranchList[iBranch]); // trivially just put something here to avoid errors, doesn't get used in this if case later...
        oldManipulatedUncertaintiesReader.push_back(temp3);
      }
      else {
        TTreeReaderValue<Double_t> temp3(oldTreeReader,oldManipulatedUncertaintiesBranchList[iBranch]);
        oldManipulatedUncertaintiesReader.push_back(temp3);
      }
      oldManipulatedValuesReader.push_back(temp1);
      oldManipulatedErrorsReader.push_back(temp2);
      //oldTree->SetBranchAddress(oldManipulatedDataBranchList[iBranch],&oldManipulatedValues[iBranch]);
      //oldTree->SetBranchAddress(oldManipulatedErrorBranchList[iBranch],&oldManipulatedErrors[iBranch]);
    }
  }
  for(Int_t iBranch = 0; iBranch < oldRespondingDataBranchList.size(); iBranch++) {
    TTreeReaderValue<Double_t> temp1(oldTreeReader,oldRespondingDataBranchList[iBranch]);
    TTreeReaderValue<Double_t> temp2(oldTreeReader,oldRespondingErrorBranchList[iBranch]);
    if (oldRespondingUncertaintiesBranchList[iBranch]=="User"){
      TTreeReaderValue<Double_t> temp3(oldTreeReader,oldRespondingDataBranchList[iBranch]); // trivially just put something here to avoid errors, doesn't get used in this if case later...
      oldRespondingUncertaintiesReader.push_back(temp3);
    }
    else {
      TTreeReaderValue<Double_t> temp3(oldTreeReader,oldRespondingUncertaintiesBranchList[iBranch]);
      oldRespondingUncertaintiesReader.push_back(temp3);
    }
    oldRespondingValuesReader.push_back(temp1);
    oldRespondingErrorsReader.push_back(temp2);
    //oldTree->SetBranchAddress(oldRespondingDataBranchList[iBranch],&oldRespondingValues[iBranch]);
    //oldTree->SetBranchAddress(oldRespondingErrorBranchList[iBranch],&oldRespondingErrors[iBranch]);
    newTree->Branch(          newRegressedBranchList[iBranch],&newRegressedValues[iBranch]);
  }
  TTreeReaderValue<Double_t> oldTreeErrorFlagValue(oldTreeReader,errorBranchName); // Constraint: all input ROOT files must have a parallel branch that denotes whether the event is to be taken seriously or not -> 0 == good event
  //oldTree->SetBranchAddress(errorBranchName,&oldTreeErrorFlag);
  newTree->Branch(okFlagReg,&newRegressedValuesOkCut);
  newTree->SetBranchStatus("*",1);
  oldTree->SetBranchStatus("*",1);
  if (debug > -1) Printf("Branches initialized");

  Double_t rate = 1.0e9;
  Double_t integrationFreq = 240.0;
  Double_t uncertainty = 1.0/sqrt(rate/integrationFreq);

  //FIXME make these into branches that get stored in output tree instead of large vectors here
  vector<Double_t> f; // The functional value integrated over all data entries
  vector<Double_t> s; // The sigma value integrated over all data entries
  vector<Double_t> chi2;
  Double_t chi2sum;
  Double_t fi = 0.0; // The functional value per data entry
  Double_t si2 = 1.0-nonLinearFit + nonLinearFit*0;//FIXME-needsWork uncertainty*uncertainty;// The total sigma value per data entry
  Double_t si = 0.0; // The total sigma value per data entry
  Double_t chi2i = 0.0;
  vector<Double_t> dfi;   // The first derivative of the functional value per data entry
  vector<Double_t> dsi;   // The first derivative of the sigma value per data entry
  vector<Double_t> dsi2;  // The first derivative of the sigma value per data entry
  vector<vector<Double_t>> ddfi;  // The second derivative of the functional value per data entry
  vector<vector<Double_t>> ddsi;  // The second derivative of the sigma value per data entry
  vector<vector<Double_t>> ddsi2; // The second derivative of the sigma value per data entry
  vector<vector<vector<Double_t>>> dddfi; // The third derivative of the functional value per data entry

  vector<Double_t> beta; // The x^2 slope vector
  vector<Double_t> betai; // The x^2 slope vector
  vector<vector<Double_t>> alpha; // The x^2 curvature matrix
  vector<vector<Double_t>> alphai; // The x^2 curvature matrix
  vector<vector<Double_t>> covariance; // The Covariance matrix = alpha^-1
  vector<vector<Double_t>> covariancei; // The Covariance matrix = alpha^-1
  vector<Double_t> delta_parameters; // The change in parameter vector = alpha^-1*beta
  vector<Double_t> placeholder;
  vector<vector<Double_t>> placeholder2;
  vector<vector<vector<Double_t>>> placeholder3;
  if (debug > -1) Printf("Empty matrices");
  for (Int_t l = 0; l<nmanip; l++){
    placeholder.push_back(0.0); // Make a vector of 0s
    dfi.push_back(0.0);
    dsi.push_back(0.0);
    dsi2.push_back(0.0);
    beta.push_back(0.0);
    betai.push_back(0.0);
    delta_parameters.push_back(0.0);
  }
  for (Int_t k = 0; k<nmanip; k++){
    placeholder2.push_back(placeholder); // Make a matrix of vectors of 0s
    ddfi.push_back(placeholder);
    ddsi.push_back(placeholder);
    ddsi2.push_back(placeholder);
    alpha.push_back(placeholder);
    alphai.push_back(placeholder);
    covariance.push_back(placeholder);
  }
  for (Int_t j = 0; j<nmanip; j++){
    dddfi.push_back(placeholder2); // Make a 3-tensor of matrices of vectors of 0s
    placeholder3.push_back(placeholder2);
  }

  //Int_t errorFlag = 0;
  Int_t numEntries = oldTree->GetEntries();
  newTree->SetEntries(numEntries); // Set the total number of entries to match
  Int_t numErrorEntries = 0;
  if (debug > -1) Printf("Will loop over %d entries",numEntries);
  if (numEntries>100000) numEntries=100000;
  Int_t eventN = 0;
  Int_t iterateAgain = 1;
  Int_t passLimit = passLimitValue;
  while(iterateAgain || fitN<nresp){
    if (debug > -1) Printf("Looping over %d entries",numEntries);
    //while(iterateAgain!=0)
    //while(oldTreeReader.Next() && oldTreeReader.GetCurrentEntry() > (iterateAgain-1)*numEntries && oldTreeReader.GetCurrentEntry() < iterateAgain*numEntries)
    while(oldTreeReader.Next() && oldTreeReader.GetCurrentEntry()<numEntries){
      //for (int i = 0; i < numEntries; i++)  // Loop over the input file's entries
      //oldTree->GetEntry(eventN);
      if (*oldTreeErrorFlagValue!=0.0){
        newRegressedValuesOkCut = 0.0;
      }
      //else {
      //  if (debug > 2) Printf("error encountered in event %d, global error = %f",eventN,*oldTreeErrorFlagValue);
      //  newRegressedValuesOkCut = 0.0;
      //}
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        if (debug > 3) Printf("Looping, j = %d",j);
        if (j<nmanip && j!=nmanipInputs){
          oldManipulatedValues[j]=*oldManipulatedValuesReader[j]*weighting[j];
          oldManipulatedErrors[j]=*oldManipulatedErrorsReader[j];
          if (oldManipulatedErrors[j] != 0.0) {
            newRegressedValuesOkCut = 0.0;
            if (debug > 2) Printf("Error in event manipulated variable %s, event %d",(const char*)oldManipulatedDataBranchList[j],eventN);
            continue;
          }
          if (oldManipulatedUncertaintiesBranchList[j]!="User"){
            oldManipulatedUncertainties[j]=*oldManipulatedUncertaintiesReader[j];
          } // use the ROOT file supplied branch with uncertainties in it
        }
        else {
          oldManipulatedValues[j]=1.0;// User input values here - this is the asymmetry/constant term
        }
      }
      // Define the function here
      if ( fit=="linear" || fit=="parity" ) {
        for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
          fi += parameters[j]*oldManipulatedValues[j]; // Functional form of f
          dfi[j] = 1*oldManipulatedValues[j]; // Functional form of first derivative of f
          for (Int_t k = 0 ; k<nmanip ; k++){ // Loop over fit parameters k
            ddfi[j][k] = 0; // Functional form of second derivative of f
            for (Int_t l = 0 ; l<nmanip ; l++){ // Loop over fit parameters l
              dddfi[j][k][l] = 0; // Functional form of third derivative of f
            } // end l
          } // end k
        } // end j (f)
      }
      if ( fit=="dipole" ) {
        Double_t Q2 = oldManipulatedValues[0];
        Double_t in = 1-0.5*parameters[1]*Q2;
        fi = parameters[0]*pow(in,-2.0);
        dfi[0] = fi/parameters[0];
        dfi[1] = parameters[0]*pow(in,-3.0)*Q2;
        ddfi[0][0] = 0.0;
        ddfi[0][1] = dfi[1]/parameters[0];
        ddfi[1][0] = dfi[1]/parameters[0];
        ddfi[1][1] = 1.5*parameters[0]*pow(in,-4.0)*pow(Q2,2.0);
        dddfi[0][0][0] = 0.0;
        dddfi[0][0][1] = 0.0;
        dddfi[0][1][0] = 0.0;
        dddfi[0][1][1] = ddfi[1][1]/parameters[0];
        dddfi[1][1][0] = ddfi[1][1]/parameters[0];
        dddfi[1][1][1] = 3.0*parameters[0]*pow(in,-5.0)*pow(Q2,3.0);
        dddfi[1][0][0] = 0.0;
        dddfi[1][0][1] = ddfi[1][1]/parameters[0];
      }
      if((*oldRespondingErrorsReader[fitN])!=0.0){ 
        if (debug > 3) Printf("Entry %d has an error in input %s",eventN,(const char*)oldRespondingDataBranchList[fitN]);
        newRegressedValuesOkCut = 0.0;
      }
      if (oldRespondingUncertaintiesBranchList[fitN]!="User"){
        oldRespondingUncertainties[fitN]=*oldRespondingUncertaintiesReader[fitN];
      } // use the ROOT file supplied branch with uncertainties in it
      f.push_back(fi);
      newRegressedValues[fitN] = *oldRespondingValuesReader[fitN] - fi + (parameters[nmanip]*1); // Regressed asymmetry of main detector [fitN]
      if (newRegressedValuesOkCut==0.0){ 
        numErrorEntries++;
        newTree->GetBranch(newRegressedBranchList[fitN])->Fill(); // Empty event here with error flag set to not ok
        newTree->GetBranch(okFlagReg)->Fill(); // Empty event here with error flag set to not ok
        newTree->SetEntries(numErrorEntries+eventN);
        //newTree->Fill(); // Empty event here with error flag set to not ok

        newRegressedValuesOkCut = 1.0;
        //eventN++;
        continue; 
      }
      si2 += oldRespondingUncertainties[fitN]*oldRespondingUncertainties[fitN];
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        si2 += oldManipulatedUncertainties[j]*oldManipulatedUncertainties[j];
        si2 += nonLinearFit*covariance[j][j]*dfi[j]*dfi[j];
        for (Int_t k = j+1; k<nmanip ; k++){ // Loop over fit parameters k
          si2  += nonLinearFit*2*dfi[j]*dfi[k]*covariance[j][k];
        }
        si = sqrt(si2);
        //chi2i += (oldRespondingValues[fitN]-fi)*(oldRespondingValues[fitN]-fi)/(si2);
      }
      if (si2 == 0){
        chi2i += (*oldRespondingValuesReader[fitN]-fi)*(*oldRespondingValuesReader[fitN]-fi);
      }
      else if (si2 > 0.0){
        chi2i += (*oldRespondingValuesReader[fitN]-fi)*(*oldRespondingValuesReader[fitN]-fi)/(si2);
      }
      else {
        continue;
      }
      newTree->GetBranch(newRegressedBranchList[fitN])->Fill();
      newTree->GetBranch(okFlagReg)->Fill();
      newTree->SetEntries(numErrorEntries+eventN);
      //newTree->Fill();
      s.push_back(si);
      chi2.push_back(chi2i);
      chi2sum+=chi2i;
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        for (Int_t k = 0; k<nmanip ; k++){ // Loop over fit parameters k
          dsi2[j] += 2*covariance[k][k]*dfi[k]*ddfi[k][j];
          for (Int_t l = k+1; l<nmanip ; l++){ // Loop over fit parameters l
            dsi2[j] += 2*covariance[k][l]*((ddfi[j][k]*dfi[l])+(ddfi[j][l]*dfi[k]));
          }
          dsi[j] = dsi2[j]/(2*si);
        }
      }
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        for (Int_t k = 0; k<nmanip ; k++){ // Loop over fit parameters k
          for (Int_t l = 0; l<nmanip ; l++){ // Loop over fit parameters l
            ddsi2[j][k] += 2*covariance[l][l]*((ddfi[l][k]*dfi[j])+(ddfi[j][k]*dfi[l]));
            for (Int_t m = l+1; m<nmanip ; m++){
              ddsi2[j][k] += 2*covariance[l][m]*((dddfi[j][k][l]*dfi[m])+(ddfi[l][j]*ddfi[m][k])+(dddfi[j][m][k]*dfi[l])+(ddfi[j][m]*ddfi[l][k]));
            }
            ddsi[j][k] = ((ddsi2[j][k]/(2*si))-(dsi2[j]*dsi2[k]/(4*si*si*si)));
          }
        }
      }
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        betai[j] += 0.5*(2*(((*oldRespondingValuesReader[fitN]-fi)*(*oldRespondingValuesReader[fitN]-fi)/(si*si*si))*dsi[j])+(2*(*oldRespondingValuesReader[fitN]-fi)*dfi[j]/(si*si)));
        // This factor of 0.5 is not clear to me from the basic math derivation but it appears to be necessary
        //betai[j] += (2*(*oldRespondingValuesReader[fitN]-fi)*dfi[j]/(si*si));
        beta[j] += betai[j];
        for (Int_t k = 0; k<nmanip ; k++){ // Loop over fit parameters k
          alphai[j][k] += 0.5*(4*(dsi[j]*(*oldRespondingValuesReader[fitN]-fi)*dfi[k]/(si*si*si))+(6*(*oldRespondingValuesReader[fitN]-fi)*(*oldRespondingValuesReader[fitN]-fi)*dsi[j]*dsi[k]/(si*si*si*si))+(-2*(*oldRespondingValuesReader[fitN]-fi)*(*oldRespondingValuesReader[fitN]-fi)*ddsi[j][k]/(si*si*si))+(4*(*oldRespondingValuesReader[fitN]-fi)*dsi[k]*dfi[j]/(si*si*si))+(2*(dfi[j]*dfi[k]/(si*si)))+(-2*(*oldRespondingValuesReader[fitN]-fi)*ddfi[j][k]/(si*si)));
          //alphai[j][k] += 0.5*(4*(dsi[j]*(*oldRespondingValuesReader[fitN]-fi)*dfi[k]/(si*si*si))+(2*(dfi[j]*dfi[k]/(si*si)))+(-2*(*oldRespondingValuesReader[fitN]-fi)*ddfi[j][k]/(si*si)));
          alpha[j][k] += alphai[j][k];
          if (debug > 4) Printf("Beta = %f, Alpha = %f",betai[j],alpha[j][k]);
        }
      }
      if (debug > 3) Printf("Functional value at event %d = %f, sigma = %f, chi2 = %f",eventN,fi,si,chi2i);

      // End storing values
      fill_n(dsi.begin(),nmanip,0.0);
      fill_n(dsi2.begin(),nmanip,0.0);
      fill_n(dfi.begin(),nmanip,0.0);
      fill_n(betai.begin(),nmanip,0.0);
      fill_n(alphai.begin(),nmanip,placeholder);
      fill_n(ddsi.begin(),nmanip,placeholder);
      fill_n(ddsi2.begin(),nmanip,placeholder);
      fill_n(ddfi.begin(),nmanip,placeholder);
      fill_n(dddfi.begin(),nmanip,placeholder2);
      //errorFlag = 0;
      fi=0.0;
      chi2i=0.0;
      si=0.0;
      si2 = 1.0-nonLinearFit + nonLinearFit*0;//FIXME-needsWork uncertainty*uncertainty;// The total sigma value per data entry
      newRegressedValuesOkCut = 1.0; // Reset true value

      //{}
      if (debug > 4) Printf("Done entry %d",eventN);
      eventN++;
    }
    eventN=0;
    // invert alpha
    //covariance = inverse_h(alpha,covariance);
    //
    //if (covariance[0][0]==-999999.0){
    //  Printf("Error, uninvertable matrix");
    //  return;
    //}

    TMatrixD * alphaM = new TMatrixD(nmanip,nmanip);
    TMatrixD * covarianceM = new TMatrixD(nmanip,nmanip);
    for (Int_t j = 0 ; j < nmanip ; j++){
      for (Int_t k = 0 ; k < nmanip ; k++){
        (*alphaM)[j][k] = alpha[j][k];
      }
    }

    covarianceM = &alphaM->Invert();

    for (Int_t j = 0 ; j < nmanip ; j++){
      for (Int_t k = 0 ; k < nmanip ; k++){
        covariance[j][k] = (*alphaM)[j][k];
      }
    }

    // multiply epsilon*beta = a vector
    for (Int_t j = 0 ; j<beta.size(); j++){
      for (Int_t k = 0 ; k<beta.size(); k++){
        delta_parameters[j]+=covariance[j][k]*beta[k];
      }
    }
    iterateAgain = 0;
    if (passLimit == 1){ // Last pass, go for it
      Printf("Last pass of fit, speed = 1");
      speed=0.5*1.0;
    }
    if (passLimit>0){
      if (debug > -1) {
        Printf("Change in parameter vector from initial guess: ");
        displayVector_h(delta_parameters);
        Printf("Original set of parameters: ");
        displayVector_h(parameters);
      }


      for (Int_t j = 0 ; j<delta_parameters.size(); j++){
        parAvg+=(parameters[j]+delta_parameters[j])/parameters[j];
      }
      parAvg = parAvg/nmanip;
      for (Int_t j = 0 ; j<delta_parameters.size(); j++){
        parRms2+=(((parameters[j]+delta_parameters[j])/parameters[j])-parAvg)*(((parameters[j]+delta_parameters[j])/parameters[j])-parAvg);
      }
      parRms2 = sqrt(parRms2/(nmanip-1));
      Printf("Relative change in all parameters avg = %f, stddev = %f",parAvg,parRms2);
      for (Int_t j = 0 ; j<parameters.size(); j++){
        Printf("Relative change in parameter %d = %f",j,(delta_parameters[j]+parameters[j])/parameters[j]);
        if (abs(abs((parameters[j]+delta_parameters[j])/parameters[j])-1)>fitCriteria){
        //if (!isnormal(parameters[j]) || !isnormal(delta_parameters[j]) || abs(abs((parameters[j]+delta_parameters[j])/parameters[j])-1)>fitCriteria){
          Printf("\nRe-fitting, needs to be less change in parameters between subsequent fits to be convergent \n");
          iterateAgain = 1;
          numErrorEntries=0;
          oldTreeReader.Restart();
          //newTree->Reset();
          newTree->GetBranch(newRegressedBranchList[fitN])->Reset(); 
          newTree->GetBranch(okFlagReg)->Reset(); 
          newTree->SetEntries(0);
        }
        if (parRms2<parameterLimitRMS && parAvg > parameterLimitValue){
          parameters[j]=(parameters[j]+speed*delta_parameters[j])/(parAvg*parAvg);
          Printf("\nData out of control, redoing but normalized \n");
        /*  iterateAgain = 1;
          numErrorEntries=0;
          oldTreeReader.Restart();
          //newTree->Reset();
          newTree->GetBranch(newRegressedBranchList[fitN])->Reset(); 
          newTree->GetBranch(okFlagReg)->Reset(); 
          newTree->SetEntries(0);*/
        }
        else{
          parameters[j]=parameters[j]+speed*delta_parameters[j];
          if (parameterLimits){
            if (parameters[j]>parametersUpperLimit[j]){
              parameters[j]=parametersUpperLimit[j];
              Printf("\nUpper limit encountered on parameter %d \n",j);
            }
            if (parameters[j]<parametersLowerLimit[j]){
              parameters[j]=parametersLowerLimit[j];
              Printf("\nLower limit encountered on parameter %d \n",j);
            }
          }
        }
        //if (parameters[j]>0.2) parameters[j]=0.2;
        //if (parameters[j]<-0.2) parameters[j]=-0.2;
      }

      if (parRms2<0.001 && parAvg > 2.0) {
        for (Int_t j = 0 ; j<parameters.size(); j++){
          Printf("\nParameters exceed runaway false stability limit\n");
          parameters[j]=1.0e-9*parameters[j];
        }
      }
      parAvg = 0.0;
      parRms2 = 0.0;

      if (debug > -1) {
        Printf("Iterate again: New set of parameters for another fit: ");
        displayVector_h(parameters);
        Printf("Chi2 = %f, reduced Chi2 = %f",chi2sum,chi2sum/(numEntries-nmanip));
      }
    }
    else {
      iterateAgain = 0;
    }
    Printf("Fit pass %d completed",passLimitValue-passLimit);
    passLimit = passLimit - 1;
    if (iterateAgain == 0){
      if (debug > -1) {
        Printf("Final set of parameters for fit: ");
        displayVector_h(parameters);
        Printf("Final Chi2 = %f, reduced Chi2 = %f",chi2sum,chi2sum/(numEntries-nmanip));
      }
      if (debug > 2) {
        Printf("Alpha matrix: ");
        displayMatrix_h(alpha,-2);
        Printf("Beta vector: ");
        displayVector_h(beta);
      }
      // Fill a histogram with yi - fi + last parameter*1 weighted by error on that
      //gROOT->SetBatch(kTRUE);
      //gROOT->SetBatch(kFALSE);

      if (folder != NULL) folder->cd();
      
      TCanvas * c1 = new TCanvas();
      c1->SetLogy();

      newTree->Draw(Form("%s",(const char*)newRegressedBranchList[fitN]),(const char*)okFlagReg);
      TH1 *h1 = (TH1*)gROOT->FindObject("htemp");
      h1->Write(Form("reg_%s_histogram",(const char*)newRegressedBranchList[fitN]));
      c1->SaveAs(Form("plots/reg_%s_%s_%d.%d.%03d.pdf",(const char*)tree,(const char*)oldRespondingDataBranchList[fitN],runNumber,minirunNumber,splitNumber));

      TCanvas * c1_2 = new TCanvas();
      c1_2->SetLogy();

      newTree->Draw(Form("%s",(const char*)newRegressedBranchList[fitN]),(const char*)okFlagReg);
      TH1 *h1_2 = (TH1*)gROOT->FindObject("htemp");
      h1_2->SetName("h1");
      TH1 *h2_3 = (TH1*)rebinTH1_h(h1_2,"clean",1,2,1000); // example use case of rebinTH1_h method
      TH1 *h2_2 = (TH1*)h2_3->Clone(); // example use case of rebinTH1_h method
      h2_3->Delete();
      TString h2_name = h2_2->GetName();
      newTree->Draw(Form("%s>>%s",(const char*)newRegressedBranchList[fitN],(const char*)h2_name),(const char*)okFlagReg); // Manual
      h2_2->Write(Form("reg_rebin_%s_histogram",(const char*)newRegressedBranchList[fitN]));
      c1_2->SaveAs(Form("plots/reg_rebin_%s_%s_%d.%d.%03d.pdf",(const char*)tree,(const char*)oldRespondingDataBranchList[fitN],runNumber,minirunNumber,splitNumber));

      TCanvas * c2 = new TCanvas();
      c2->SetLogy();

      oldTree->Draw(Form("%s",(const char*)oldRespondingDataBranchList[fitN]),Form("%s==0",(const char*)errorBranchName));
      TH1 *h1old = (TH1*)gROOT->FindObject("htemp");
      h1old->Write(Form("orig_%s_histogram",(const char*)oldRespondingDataBranchList[fitN]));
      c2->SaveAs(Form("plots/orig_%s_%s_%d.%d.%03d.pdf",(const char*)tree,(const char*)oldRespondingDataBranchList[fitN],runNumber,minirunNumber,splitNumber));

      TCanvas * c2_2 = new TCanvas();
      c2_2->SetLogy();

      oldTree->Draw(Form("%s",(const char*)oldRespondingDataBranchList[fitN]),Form("%s==0",(const char*)errorBranchName));
      TH1 *h1_2old = (TH1*)gROOT->FindObject("htemp");
      h1_2old->SetName("h1old");
      TH1 *h2_3old = (TH1*)rebinTH1_h(h1_2old,"clean",1,2,1000); // example use case of rebinTH1_h method
      TH1 *h2_2old = (TH1*)h2_3old->Clone(); // example use case of rebinTH1_h method
      h2_3old->Delete();
      TString h2old_name = h2_2old->GetName();
      oldTree->Draw(Form("%s>>%s",(const char*)oldRespondingDataBranchList[fitN],(const char*)h2old_name),Form("%s==0",(const char*)errorBranchName)); // Manual
      h2_2old->Write(Form("orig_rebin_%s_histogram",(const char*)oldRespondingDataBranchList[fitN]));
      c2_2->SaveAs(Form("plots/orig_rebin_%s_%s_%d.%d.%03d.pdf",(const char*)tree,(const char*)oldRespondingDataBranchList[fitN],runNumber,minirunNumber,splitNumber));

      outFile->cd();

      if (debug > -1) {
        Printf("Covariance matrix: ");
        displayMatrix_h(covariance,-2);
        if (fit=="linear"||fit=="parity"){
          for (Int_t j = 0 ; j<parameters.size(); j++){
            Printf("Parameter %s +- error = %5.3e +- %5.3e",(const char*)oldManipulatedDataBranchList[j],parameters[j],sqrt(covariance[j][j]));
          }
        }
        if (fit=="dipole"){
          Printf("Par: Normalization = %5.3e +- %5.3e",parameters[0],sqrt(covariance[0][0]));
          Printf("Par: Radius = %5.3e +- %5.3e",sqrt(-1*6*parameters[1]),1.224*sqrt(covariance[1][1]));
        }
        Printf("Regressed reg %s average %5.3e +- %5.3e, std dev %5.3e +- %5.3e",(const char*)oldRespondingDataBranchList[fitN],h1->GetMean(),h1->GetMeanError(),h1->GetRMS(),h1->GetRMSError());
        Printf("Regressed old %s average %5.3e +- %5.3e, std dev %5.3e +- %5.3e",(const char*)oldRespondingDataBranchList[fitN],h1old->GetMean(),h1old->GetMeanError(),h1old->GetRMS(),h1old->GetRMSError());
        Printf("Regressed rebinned reg %s average %5.3e +- %5.3e, std dev %5.3e +- %5.3e",(const char*)oldRespondingDataBranchList[fitN],h2_2->GetMean(),h2_2->GetMeanError(),h2_2->GetRMS(),h2_2->GetRMSError());
        Printf("Regressed rebinned old %s average %5.3e +- %5.3e, std dev %5.3e +- %5.3e",(const char*)oldRespondingDataBranchList[fitN],h2_2old->GetMean(),h2_2old->GetMeanError(),h2_2old->GetRMS(),h2_2old->GetRMSError());
        Printf("Number of entries = %d, entries with errors = %d, percent good entries = %f%%",numEntries,numErrorEntries,100.0*(numEntries-numErrorEntries)/numEntries);
      }
      // Go back to the top and loop over the next responding variable
      fitN++;
      passLimit = passLimitValue;
      iterateAgain    = 1;
      numErrorEntries = 0;
      oldTreeReader.Restart();
      //newTree->Write();
      //newTree->Reset();
      //newTree->SetEntries(0);
      // Close out on final pass
      if (fitN==nresp){
        //outFile->Close();
        iterateAgain=0;
      }
      else {
        newTree->SetEntries(0);
      }
    }
    chi2sum = 0;

    // Reset
    chi2.clear();
    f.clear();
    s.clear();
    fill_n(beta.begin(),nmanip,0.0);
    fill_n(delta_parameters.begin(),nmanip,0.0);
    fill_n(alpha.begin(),nmanip,placeholder);

  }
  newTree->Write();
  outFile->Close();
  if (aggregatorStatus){
  //writeFile_h("test_n_data",n_data,runNumber,minirunNumber, splitNumber,nRuns);
  }
}
#endif // __CAMREG__

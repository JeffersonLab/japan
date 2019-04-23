#ifndef __CAMREG__
#define __CAMREG__
#include "../camguin.hh"
#include <vector>
#include <TString.h>
#include <iostream>
#include <string>
#include <TChain.h>
using namespace std;
void regress_h(TString tree = "mul", Int_t runNumber = 0, Int_t nRuns = -1, TString regInput = "regressionInput.txt", char delim = ' '){
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  vector<vector<string>> textFile = textFileParse_h(regInput,delim);
  TTree * oldTree = getTree_h(tree, runNumber, nRuns, "NULL");
  TTree * newTree = new TTree("reg"+tree,"Regressed "+tree+" tree");
  if (oldTree==0){
    Printf("Root file does not exist");
    return;
  }

  Double_t data     = 0.0;
  Int_t    n_data   = 0;
  vector<Double_t> newRegressedValues;
  vector<Double_t> parameters;
  vector<Double_t> weighting;
  vector<Double_t> oldManipulatedValues;
  vector<Double_t> oldManipulatedErrors;
  vector<Double_t> oldRespondingValues;
  vector<Double_t> oldRespondingErrors;
  vector<TString> newRegressedBranchList;
  vector<TString> oldManipulatedDataBranchList;
  vector<TString> oldManipulatedErrorBranchList;
  vector<TString> oldRespondingDataBranchList;
  vector<TString> oldRespondingErrorBranchList;
  TString errorBranchName  = "ErrorFlag";
  TString oldTreeErrorFlag = "0";
  Bool_t resp  = false;
  Int_t nresp  = 0;
  Bool_t manip = false;
  Int_t nmanip = 0;
  if (debug > 2) Printf("Data structures initialized");
  for (UInt_t listEntryN = 0; listEntryN < textFile.size(); listEntryN++){
    if (textFile[listEntryN][0] == "Global"){
      if (debug > 2) Printf("Branch %s",textFile[listEntryN][1].c_str());
      errorBranchName = textFile[listEntryN][1];
      resp = false;
      manip = false;
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
      if (debug > 2) Printf("Branch %s",textFile[listEntryN][0].c_str());
      oldRespondingDataBranchList.push_back(textFile[listEntryN][0]+textFile[listEntryN][1]);
      oldRespondingValues.push_back(0.0); 
      oldRespondingErrorBranchList.push_back(textFile[listEntryN][0]+textFile[listEntryN][2]);
      oldRespondingErrors.push_back(0.0); 
      newRegressedBranchList.push_back(textFile[listEntryN][0]);
      newRegressedValues.push_back(0.0); 
      nresp++;
    }
    if (manip){
      if (debug > 2) Printf("Branch %s",textFile[listEntryN][0].c_str());
      oldManipulatedDataBranchList.push_back(textFile[listEntryN][0]+textFile[listEntryN][1]);
      oldManipulatedValues.push_back(0.0); 
      oldManipulatedErrorBranchList.push_back(textFile[listEntryN][0]+textFile[listEntryN][2]);
      oldManipulatedErrors.push_back(0.0); 
      parameters.push_back(stof(textFile[listEntryN][3])); // Initial parameter correlation slope guess for iterating fit
      weighting.push_back(stof(textFile[listEntryN][4])); // Initial weighting guess for errors
      nmanip++;
    }
    if (debug > 1) Printf("Branch %d initialized",listEntryN);
  }
  oldManipulatedValues.push_back(1.0); // The physics asymmetry is asymmetry*1 (the number 1 is the type of value it is, an exact scalar)
  oldManipulatedErrors.push_back(0.0); // The physics asymmetry applies for all Global Cuts passing entries
  parameters.push_back(0.0); // Push back the physics asymmetry value placeholder at nmanip+1 position... assume it is trivially 0 for first pass
  weighting.push_back(1.0); // Push back the physics asymmetry relative weighting factor for uncertainty calculations
  nmanip++; // The physics asymmetry value adds ++ to the parameters of the fit
  for(Int_t iBranch = 0; iBranch < oldManipulatedDataBranchList.size(); iBranch++) {
    oldTree->SetBranchAddress(oldManipulatedDataBranchList[iBranch],&oldManipulatedValues[iBranch]);
    oldTree->SetBranchAddress(oldManipulatedErrorBranchList[iBranch],&oldManipulatedErrors[iBranch]);
  }
  for(Int_t iBranch = 0; iBranch < oldRespondingDataBranchList.size(); iBranch++) {
    oldTree->SetBranchAddress(oldRespondingDataBranchList[iBranch],&oldRespondingValues[iBranch]);
    oldTree->SetBranchAddress(oldRespondingErrorBranchList[iBranch],&oldRespondingErrors[iBranch]);
    newTree->Branch(          newRegressedBranchList[iBranch],&newRegressedValues[iBranch]);
  }
  oldTree->SetBranchAddress(errorBranchName,&oldTreeErrorFlag);
  newTree->SetBranchStatus("*",1);
  oldTree->SetBranchStatus("*",1);

  Double_t rate = 1.0e9;
  Double_t integrationFreq = 240.0;
  Double_t uncertainty = 1.0/sqrt(rate/integrationFreq);

  vector<Double_t> f; // The functional value integrated over all data entries
  vector<Double_t> s; // The sigma value integrated over all data entries
  vector<Double_t> chi2i;
  Double_t fi = 0.0; // The functional value per data entry
  Double_t si2 = uncertainty*uncertainty;// The total sigma value per data entry
  Double_t si = 0.0; // The total sigma value per data entry
  Double_t chi2 = 0.0;
  vector<Double_t> dfi;   // The first derivative of the functional value per data entry
  vector<Double_t> dsi;   // The first derivative of the sigma value per data entry
  vector<Double_t> dsi2;  // The first derivative of the sigma value per data entry
  vector<vector<Double_t>> ddfi;  // The second derivative of the functional value per data entry
  vector<vector<Double_t>> ddsi;  // The second derivative of the sigma value per data entry
  vector<vector<Double_t>> ddsi2; // The second derivative of the sigma value per data entry
  vector<vector<vector<Double_t>>> dddfi; // The third derivative of the functional value per data entry

  vector<Double_t> beta; // The x^2 slope vector
  vector<vector<Double_t>> alpha; // The x^2 curvature matrix
  vector<vector<Double_t>> covariance; // The Covariance matrix = alpha^-1
  vector<Double_t> delta_parameters; // The change in parameter vector = alpha^-1*beta
  vector<Double_t> placeholder;
  vector<vector<Double_t>> placeholder2;
  for (Int_t j = 0; j<nmanip; j++){
    for (Int_t k = 0; j<nmanip; k++){
      for (Int_t l = 0; j<nmanip; l++){
        placeholder.push_back(0.0); // Make a vector of 0s
      }
      placeholder2.push_back(placeholder); // Make a matrix of vectors of 0s
      placeholder.clear();
    }
    dfi.push_back(0.0);
    dsi.push_back(0.0);
    dsi2.push_back(0.0);
    beta.push_back(0.0);
    delta_parameters.push_back(0.0);
    ddfi.push_back(placeholder);
    ddsi.push_back(placeholder);
    ddsi2.push_back(placeholder);
    alpha.push_back(placeholder);
    covariance.push_back(placeholder);
    dddfi.push_back(placeholder2); // Make a 3-tensor of matrices of vectors of 0s
    placeholder2.clear();
  }

  Int_t fitN = 0;
  Int_t errorFlag = 0;
  Int_t numEntries = oldTree->GetEntries();
  for (int i = 0; i < numEntries; i++) { // Loop over the input file's entries
    oldTree->GetEntry(i);
    if (oldTreeErrorFlag==0){
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        if(oldManipulatedErrors[j]==0){
          fi += parameters[j]*oldManipulatedValues[j]; // Functional form of f
          dfi[j] = 1*oldManipulatedValues[j]; // Functional form of first derivative of f
          for (Int_t k = 0 ; k<nmanip ; k++){ // Loop over fit parameters k
            ddfi[j][k] = 0; // Functional form of second derivative of f
            for (Int_t l = 0 ; l<nmanip ; l++){ // Loop over fit parameters l
              dddfi[j][k][l] = 0; // Functional form of third derivative of f
            }
          } // end l
        } // end k
        else {
          errorFlag = 1;
          continue; // Abort this entry
        }
      } // end j (f)
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        if(oldManipulatedErrors[j]==0){
          si2 += covariance[j][j]*dfi[j]*dfi[j];
          for (Int_t k = j+1; k<nmanip ; k++){ // Loop over fit parameters k
            si2  += 2*dfi[j]*dfi[k]*covariance[j][k];
          }
          si = sqrt(si2);
          chi2 += (oldRespondingValues[fitN]-fi)*(oldRespondingValues[fitN]-fi)/(si2);
        }
      }
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        if(oldManipulatedErrors[j]==0){
          for (Int_t k = 0; k<nmanip ; k++){ // Loop over fit parameters k
            dsi2[j] += 2*covariance[k][k]*dfi[k]*ddfi[k][j];
            for (Int_t l = k+1; l<nmanip ; l++){ // Loop over fit parameters l
              dsi2[j] += 2*covariance[k][l]*((ddfi[j][k]*dfi[l])+(ddfi[j][l]*dfi[k]));
            }
            dsi[j] = dsi2[j]/(2*si);
          }
        }
      }
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        if(oldManipulatedErrors[j]==0){
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
      }
      for (Int_t j = 0 ; j<nmanip ; j++){ // Loop over fit parameters j
        if(oldManipulatedErrors[j]==0){
          beta[j] += (2*((oldRespondingValues[fitN]-fi)*(oldRespondingValues[fitN]-fi)/(si*si*si))*dsi[j])-(2*(oldRespondingValues[fitN]-fi)*dfi[j]/(si*si));
          for (Int_t k = 0; k<nmanip ; k++){ // Loop over fit parameters k
            alpha[j][k] += 0.5*(-4*(dsi[j]*(oldRespondingValues[fitN]-fi)*dfi[k]/(si*si*si))+(6*(oldRespondingValues[fitN]-fi)*(oldRespondingValues[fitN]-fi)*dsi[j]*dsi[k]/(si*si*si*si))+(-2*(oldRespondingValues[fitN]-fi)*(oldRespondingValues[fitN]-fi)*ddsi[j][k]/(si*si*si))+(4*(oldRespondingValues[fitN]-fi)*dsi[k]*dfi[j]/(si*si*si))+(2*(dfi[j]*dfi[k]/(si*si)))+(-2*(oldRespondingValues[fitN]-fi)*ddfi[j][k]/(si*si)));
          }
        }
      }
      /*
      // invert alpha
      displayMatrix_h(alpha);
      if (!inverse_h(alpha,covariance)){
        Printf("Error, uninvertable matrix");
        return;
      }
      displayMatrix_h(covariance);
      // multiply epsilon*beta = a vector
      for (Int_t j = 0 ; j<beta.size(); j++){
        for (Int_t k = 0 ; k<beta.size(); k++){
          delta_parameters[j]+=covariance[j][k]*beta[k];
        }
      }
      // End storing values
      if (errorFlag==0){
        f.push_back(fi);
      }
      errorFlag = 0;
      fi=0.0;
      si2=0.0;
      */
    }
  }
  //writeFile_h("test_n_data",n_data,runNumber,nRuns);
  chi2=0.0;
}
#endif // __CAMREG__

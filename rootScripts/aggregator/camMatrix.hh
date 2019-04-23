#ifndef __CAMMATRIX__
#define __CAMMATRIX__
#include "camguin.hh"
#include <vector>
#include <TString.h>
#include <iostream>
#include <string>
using namespace std;
void displayMatrix_h(vector<vector<Double_t>> input){
  Printf("(");
  for (Int_t j = 0; j<input.size(); j++){
    Printf("(");
    for (Int_t k = 0; k<input[j].size(); k++){
      Printf("\t%f\t",input[j][k]);
    }
    Printf(")\n");
  }
  Printf(")");
}

void cofactor_h(vector<vector<Double_t>> input, vector<vector<Double_t>> temp, Int_t inRow, Int_t inColumn, Int_t length){
  Int_t i = 0;
  Int_t j = 0;

  for (Int_t row = 0; row<length; row++){
    for (Int_t column = 0; column<length; column++){
      if (row!=inRow && column !=inColumn){
        temp[i][j++] = input[row][column];
        if (j == length-1){
          j=0;
          i++;
        }
      }
    }
  }
}

Double_t determinant_h(vector<vector<Double_t>> input, Int_t subLength = 0){
  Double_t determinant = 0.0;
  if (input.size()==1){
    return input[0][0]; // Trivial scalar
  }
  vector<vector<Double_t>> temp; // For Cofactors
  vector<Double_t> temp2;
  for (Int_t j = 0; j<input.size() ; j++){
    for (Int_t k = 0; k<input.size() ; k++){
      temp2.push_back(0.0);
    }
    temp.push_back(temp2);
    temp2.clear();
  }
  Int_t sign = 1;                // For sign multiplier
  for (Int_t i = 0 ; i < (input.size()-subLength) ; i++){
    cofactor_h(input,temp,0,i,input.size()-subLength);
    subLength++;
    determinant+=sign*input[0][i]*determinant_h(temp,subLength);
  }
  return determinant;
}

void adjoint_h(vector<vector<Double_t>> input, vector<vector<Double_t>> adjoint){
  if (input.size() == 1){
    adjoint[0][0] = input[0][0];
    return;
  }
  Int_t sign = 1;
  vector<vector<Double_t>> temp;
  vector<Double_t> temp2;
  for (Int_t j = 0; j<input.size() ; j++){
    for (Int_t k = 0; k<input.size() ; k++){
      temp2.push_back(0.0);
    }
    temp.push_back(temp2);
    temp2.clear();
  }
  for (Int_t j = 0; j<input.size() ; j++){
    for (Int_t k = 0; k<input.size() ; k++){
      cofactor_h(input,temp,j,k,input.size());
      sign = ((j+k)%2==0)? 1: -1;
      adjoint[k][j]=(sign)*(determinant_h(temp,1));
    }
  }
}

Bool_t inverse_h(vector<vector<Double_t>> input, vector<vector<Double_t>> output){
  Double_t determinant = determinant_h(input,0);
  if (determinant == 0.0){
    Printf("Error: Singular curvature matrix, cannot find inverse");
    return false;
  }
  if (input.size()!=input[0].size()){
    Printf("Error: Non-square matrix, cannot find inverse");
    return false;
  }
  adjoint_h(input, output);
  for (Int_t j = 0; j<input.size(); j++){
    for (Int_t k = 0; k<input[j].size(); k++){
      output[j][k] = output[j][k]/determinant;
    }
  }
  return true;
}
#endif // __CAMMATRIX__

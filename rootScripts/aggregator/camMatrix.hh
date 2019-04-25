#ifndef __CAMMATRIX__
#define __CAMMATRIX__
#include "camguin.hh"
#include <vector>
#include <TString.h>
#include <iostream>
#include <string>
// Utilizing code from: https://www.geeksforgeeks.org/adjoint-inverse-matrix/
using namespace std;
void displayVector_h(vector<Double_t> input){
  cout<<"{";
  for (Int_t j = 0; j<input.size(); j++){
    cout<<" "<<Form("%5.3e",input[j]);
    if (j<input.size()-1){
      cout<<"\n";
    }
  }
  cout<<" }\n";
}

void displayMatrix_h(vector<vector<Double_t>> input, Int_t size = -1){
  Int_t sizej = size;
  if (size == -1){ // Default matrix printing, else user set size Square matrix
    size = input.size();
  }
  Bool_t triangle = false;
  if (size == -2){ // Default matrix printing, but only upper triangular part
    size = input.size();
    triangle=true;
  }
  cout<<"{";
  for (Int_t j = 0; j<size; j++){
    if (sizej < 0){
      sizej = input[j].size();
    }
    cout<<"{";
    if (triangle == false){
      for (Int_t k = 0; k<sizej; k++){
        cout<<" "<<Form("%5.3e",input[j][k])<<" ";
      }
    }
    else {
      for (Int_t k = 0; k<sizej; k++){
        if (k>=j){
          cout<<" "<<Form("%5.3e",input[j][k])<<" ";
        }
        else {
          cout<<" "<<Form("%5.3e",0.0)<<" ";
        }
      }
    }
    if (j<input.size()-1){
      cout<<"}\n";
    }
  }
  cout<<"}}\n";
}

vector<vector<Double_t>> cofactor_h(vector<vector<Double_t>> input, vector<vector<Double_t>> temp, Int_t inRow, Int_t inColumn, Int_t size){
  if (debug > 2) {
    Printf("Input matrix to be cofactored:");
    displayMatrix_h(input,size);
  }
  Int_t i = 0;
  Int_t j = 0;

  for (Int_t row = 0; row<size; row++){
    for (Int_t column = 0; column<size; column++){
      if (row!=inRow && column !=inColumn){
        temp[i][j++] = input[row][column];
        if (debug > 3) Printf("Cofactoring input[%d][%d] = %f ",row,column,input[row][column]);
        if (j == size-1){
          j=0;
          i++;
        }
      }
    }
  }
  if (debug > 3) {
    Printf("Temp cofactored matrix:");
    displayMatrix_h(temp,size);
  }
  return temp;
}

Double_t determinant_h(vector<vector<Double_t>> input, Int_t size){
  if (debug>2) {
    Printf("Matrix to have %dx%d determinant taken:",size,size);
    displayMatrix_h(input,size);
  }
  Double_t determinant = 0.0;
  if (size==1){
    if (debug>2) Printf("Determinant = %f, size = %d",input[0][0], size);
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
  for (Int_t i = 0 ; i < size ; i++){
    if (debug > 2) Printf("Determinant computation %f%% done",100.0*i/size); 
    temp=cofactor_h(input,temp,0,i,size);
    determinant+=sign*input[0][i]*determinant_h(temp,size-1);
    sign=-1*sign;
  }
  if (debug>2) Printf("Determinant = %f, size = %d",determinant, size);
  return determinant;
}

vector<vector<Double_t>> adjoint_h(vector<vector<Double_t>> input, vector<vector<Double_t>> adjoint){
  if (debug > 2) {
    Printf("Matrix to have its adjoint taken:");
    displayMatrix_h(input);
  }
  if (input.size() == 1){
    adjoint[0][0] = input[0][0];
    return adjoint;
  }
  Int_t sign = 1;
  vector<vector<Double_t>> temp;
  vector<Double_t> temp2;
  for (Int_t j = 0; j<input.size()-1 ; j++){
    for (Int_t k = 0; k<input.size()-1 ; k++){
      temp2.push_back(0.0);
    }
    temp.push_back(temp2);
    temp2.clear();
  }
  for (Int_t j = 0; j<input.size() ; j++){
    if (debug > 2) Printf("Adjoint computation %f%% done",(100.0*j/input.size())); 
    for (Int_t k = 0; k<input.size() ; k++){
      temp=cofactor_h(input,temp,j,k,input.size());
      if (debug > 3) {
        Printf("Temporary adjointing matrix:");
        displayMatrix_h(temp,temp.size());
      }
      sign = ((j+k)%2==0)? 1: -1;
      adjoint[k][j]=(sign)*(determinant_h(temp,input.size()-1));
    }
  }
  return adjoint;
}

vector<vector<Double_t>> inverse_h(vector<vector<Double_t>> input, vector<vector<Double_t>> output){
  if (debug > 2) {
    Printf("Matrix to be inverted:");
    displayMatrix_h(input);
  }
  Double_t determinant = determinant_h(input,input.size());
  if (debug > 2) {
    Printf("Matrix determinant = %f",determinant);
  }
  vector<vector<Double_t>> errorOut;
  errorOut.push_back({-999999.0});
  if (determinant == 0.0){
    Printf("Error: Singular curvature matrix, cannot find inverse");
    return errorOut;
  }
  if (input.size()!=input[0].size()){
    Printf("Error: Non-square matrix, cannot find inverse");
    return errorOut;
  }
  output = adjoint_h(input, output);
  for (Int_t j = 0; j<input.size(); j++){
    for (Int_t k = 0; k<input[j].size(); k++){
      output[j][k] = output[j][k]/determinant;
    }
  }
  return output;
}
#endif // __CAMMATRIX__

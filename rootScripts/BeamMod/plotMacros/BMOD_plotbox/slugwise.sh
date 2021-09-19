#!/bin/bash

root -l -b -q slugwise_drl_diffs.C'("part","_no_err","OnlyBMOD",0)'
root -l -b -q slugwise_drl_diffs.C'("part","_no_err","IncludeBMOD",0)'
####root -l -b -q slugwise_drl_residuals_slopes.C'("part","_eigenvectors_double_sorted","_fractional","","",0)'
####root -l -b -q slugwise_drl_residuals_slopes.C'("part","_eigenvectors_double_sorted","_fractional","","_allbpms",0)'
####for (( i=100; i<=223; i++)); do
####  slug=$i
####  echo "Slug $slug"
####  #root -l -b -q slugwise_drl_diffs.C'("part","_no_err","_allbpms",'$slug')'
####  #root -l -b -q slugwise_drl_diffs.C'("part","_no_err","",'$slug')'
####  root -l -b -q slugwise_drl_residuals_slopes.C'("part","_eigenvectors_double_sorted","_fractional","","_allbpms",'$slug')'
####  root -l -b -q slugwise_drl_residuals_slopes.C'("part","_eigenvectors_double_sorted","_fractional","","",'$slug')'
####done

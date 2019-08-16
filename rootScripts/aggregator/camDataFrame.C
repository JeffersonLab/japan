#include "camguin.hh"
#include "camIO.hh"
#include "camHist.hh"
#include "camDataFrame.hh"
#include "camAna.hh"
#include "camMatrix.hh"
#include "regressor/camReg.hh"
#include "camDataFrame.hh"
int camDataFrame(TString run_n = "3103", TString n_runs = "0", TString n_minirun = "-1", TString n_split = "000", TString input = "wrapper/dataFrame_input.txt"){
  Source source(run_n,n_runs,n_minirun,n_split,input.Data());  // These labels need to be passed as strings. Otherwise it's difficult to convert them back to filenames.
  auto df_source=source.readSource();
  return 0;
} 

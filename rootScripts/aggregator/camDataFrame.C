#include "camguin.hh"
#include "camDataFrame.hh"
int camDataFrame(TString run_n = "3103", TString n_runs = "0", TString n_minirun = "-1", TString n_split = "000", TString input = "wrapper/dataFrame_input.txt",TString base_name = "prexPrompt_pass2"){
  Source source(run_n,n_runs,n_minirun,n_split,input.Data(),base_name.Data());  // These labels need to be passed as strings. Otherwise it's difficult to convert them back to filenames.
  auto df_source=source.readSource();
  return 0;
} 

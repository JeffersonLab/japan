#include "camguin.hh"
using namespace std;
void camguin(TString ana = "help", TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCuts", Int_t overWriteCut = 0, TString histMode = "defaultHist", Int_t runNumber = 0, Int_t nRuns = -1){
  if( 
       ana == "Integral" 
    || ana == "integral" 
    || ana == "Sum" 
    || ana == "sum" 
    || ana == "INT" 
    || ana == "Int" 
    || ana == "int"){
    //writeInt_loop_h( tree, branch, leaf, cut, runNumber, nRuns );
    writeInt_leafHist_h( tree, branch, leaf, cut, runNumber, nRuns );
  }
  else if( 
       ana == "intloop"){
    writeInt_loop_h( tree, branch, leaf, cut, runNumber, nRuns );
  }
  else if ( 
       ana == "rms" 
    || ana == "Rms" 
    || ana == "width" 
    || ana == "Width" 
    || ana == "RMS" 
    || ana == "Stddev" 
    || ana == "stddev" 
    || ana == "Stdv" 
    || ana == "stdv" 
    || ana == "Sigma" 
    || ana == "sigma"
    || ana == "Sig"
    || ana == "sig"){
    writeRMS_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, nRuns );
  }
  else if (
       ana == "Mean"
    || ana == "mean"
    || ana == "MEAN"
    || ana == "Average"
    || ana == "average"
    || ana == "Avg"
    || ana == "avg"){
    writeMean_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, nRuns );
  }
  else if (
       ana == "meanrms"
    || ana == "meanRMS"
    || ana == "MeanRms"
    || ana == "meanRms"
    || ana == "mean_rms"
    || ana == "mean and rms"
    || ana == "mean&&rms"){
    writeMeanRms_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, nRuns );
  }
  else if (
       ana == "Display"
    || ana == "display"
    || ana == "Disp"
    || ana == "disp"
    || ana == "Draw"
    || ana == "draw"
    || ana == "D"
    || ana == "d"
    || ana == "Print"
    || ana == "print"
    || ana == "P"
    || ana == "p"){
    TH1* h1 = getHistogram_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, nRuns );
    gROOT->SetBatch(kFALSE);
    h1->Draw();
  }
  else if (
       ana == "redraw"
    || ana == "RePrint"
    || ana == "Reprint"
    || ana == "rePrint"
    || ana == "RP"
    || ana == "Rp"
    || ana == "rP"
    || ana == "rp"
    || ana == "reprint"){
    TH1* h1 = getHistogram_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, nRuns );
    gROOT->SetBatch(kFALSE);
    h1->Draw();
  }
  else
  {
    Printf("Standard form: ./camguin.C(string \"type of analysis\" (rms), string \"tree\" (mul), string \"branch\" (asym_vqwk_04_0ch0), string \"leaf\" (hw_sum), string \"cuts\" (defaultCuts), int overWriteCut (0, boolean to overwrite default cuts), string \"histMode\" (defaultHist, doesn't rebin), int runNumber ($RUNNUM), int nRuns ($NRUNS))");
  }
}

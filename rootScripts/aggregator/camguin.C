#include "camguin.hh"
#include "camIO.hh"
#include "camHist.hh"
#include "camDataFrame.hh"
#include "camAna.hh"
#include "camMatrix.hh"
#include "regressor/camReg.hh"
using namespace std;
void camguin(TString ana = "help", TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCuts", Int_t overWriteCut = 0, TString histMode = "defaultHist", Int_t stabilityRing = 0, Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  getDebug_h();
  getAlarmStatus_h();
  getAggregatorStatus_h();
  if (debug>0) Printf("Analysis: %s",(const char*) ana);
  if( 
       ana == "Integral" 
    || ana == "integral" 
    || ana == "Sum" 
    || ana == "sum"){
    //writeInt_loop_h( tree, branch, leaf, cut, runNumber, minirunNumber, splitNumber, nRuns );
    writeInt_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, minirunNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with integral ana");
  }
  else if( 
       ana == "intloop"){
    writeInt_loop_h( tree, branch, leaf, cut, overWriteCut, runNumber, minirunNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with intloop ana");
  }
  else if( 
       ana == "nevents"
    || ana == "NeventsCuts"
    || ana == "eventscuts"
    || ana == "cutevents"
    || ana == "cutEvents"
    || ana == "CutEvents"
    || ana == "Nevents"
    || ana == "events"
    || ana == "Events"
    || ana == "NEvents"){
    writeNEvents_Loop_h( tree, branch, leaf, cut, overWriteCut, stabilityRing, runNumber, minirunNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with nEvents loop ana");
  }
  else if( 
       ana == "last"
    || ana == "lastloop"
    || ana == "getlast"
    || ana == "getLast"){
    writeLast_Loop_h( tree, branch, leaf, runNumber, minirunNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with getLast ana");
  }
  else if( 
       ana == "eventsloopn"
    || ana == "eventloopn"
    || ana == "eventloopN"
    || ana == "eventLoopN"
    || ana == "eventsLoopN"
    || ana == "eventsloopN"
    || ana == "EventLoop"){
    writeEventLoopN_Loop_h( tree, branch, leaf, cut, overWriteCut, stabilityRing, runNumber, minirunNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with EventLoopNs loop ana");
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
    writeRMS_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, minirunNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with rms ana");
  }
  else if (
       ana == "Mean"
    || ana == "mean"
    || ana == "MEAN"
    || ana == "Average"
    || ana == "average"
    || ana == "Avg"
    || ana == "avg"){
    writeMean_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, minirunNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with mean ana");
  }
  else if (
       ana == "lmeanrms"
    || ana == "lmeanRMS"
    || ana == "lMeanRms"
    || ana == "lmeanRms"
    || ana == "lmean_rms"
    || ana == "lmean and rms"
    || ana == "lmean&&rms"){
    writeMeanRms_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, minirunNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with leaf meanrms ana");
  }
  else if (
       ana == "meanrms"
    || ana == "meanRMS"
    || ana == "MeanRms"
    || ana == "meanRms"
    || ana == "mean_rms"
    || ana == "mean and rms"
    || ana == "mean&&rms"){
    // Leaf here is actually units, everything to be drawn is the "branch"
    if(tree=="muls")
      writeMeanRms_muls(tree, branch, leaf, cut, overWriteCut, runNumber, minirunNumber, splitNumber, nRuns );
    else
      writeMeanRms_h( tree, branch, leaf, cut, overWriteCut, runNumber, minirunNumber, splitNumber, nRuns );
    // FIXME Removed histMode input from this version of writeMeanRMS since we aren't doing intelligent branch parsing. This is the ideal Aggregator method, so don't make it any fancier
    if (debug>1) Printf("Done with meanrms ana");
  }
  else if (
      ana == "slope"){
    writeSlope_h( runNumber, minirunNumber, splitNumber, nRuns);
  }
  else if (
       ana == "combinermeanrms"
    || ana == "combinermeanRms"
    || ana == "combinerMeanRms"
    || ana == "combiner"
    || ana == "combine"
    || ana == "combo"){
    writeCombinedMeanRms_h( tree, branch, leaf, cut, overWriteCut, runNumber, minirunNumber, splitNumber, nRuns ); // FIXME Removed histMode input from this version of writeMeanRMS since we aren't doing intelligent branch parsing. This is the ideal Aggregator method, so don't make it any fancier - this combo one does the branch as draw and then prints the leaf
    if (debug>1) Printf("Done with correction ana");
  }
  else if (
       ana == "combinermeanrmsdf"
    || ana == "combinermeanRmsDF"
    || ana == "combinerMeanRmsDF"
    || ana == "combinerDF"
    || ana == "combineDF"
    || ana == "comboDF"){
    writeCombinedMeanRmsDF_h( tree, branch, leaf, cut, overWriteCut, runNumber, minirunNumber, splitNumber, nRuns ); // FIXME Removed histMode input from this version of writeMeanRMS since we aren't doing intelligent branch parsing. This is the ideal Aggregator method, so don't make it any fancier - this combo one does the branch as draw and then prints the leaf
    if (debug>1) Printf("Done with correction DataFrame ana");
  }
  else if (
       ana == "correctionmeanrms"
    || ana == "correctionmeanRms"
    || ana == "correctionMeanRms"
    || ana == "correction"
    || ana == "corr"
    || ana == "cor"){
    writeCorrectionMeanRms_h( tree, branch, leaf, cut, overWriteCut, runNumber, minirunNumber, splitNumber, nRuns ); // FIXME Removed histMode input from this version of writeMeanRMS since we aren't doing intelligent branch parsing. This is the ideal Aggregator method, so don't make it any fancier
    if (debug>1) Printf("Done with correction ana");
  }
  else if (
       ana == "correctionmeanrmsdf"
    || ana == "correctionmeanRmsDF"
    || ana == "correctionMeanRmsDF"
    || ana == "correctionDF"
    || ana == "corrDF"
    || ana == "corDF"){
    writeCorrectionMeanRmsDF_h( tree, branch, leaf, cut, overWriteCut, runNumber, minirunNumber, splitNumber, nRuns ); // FIXME Removed histMode input from this version of writeMeanRMS since we aren't doing intelligent branch parsing. This is the ideal Aggregator method, so don't make it any fancier
    if (debug>1) Printf("Done with correction DataFrame ana");
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
    TH1* h1 = getHistogram_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, minirunNumber, splitNumber, nRuns );
    gROOT->SetBatch(kFALSE);
    h1->Draw();
    if (debug>1) Printf("Done with draw ana");
    //gROOT->SetBatch(kTRUE);
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
    TH1* h1 = getHistogram_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, minirunNumber, splitNumber, nRuns );
    gROOT->SetBatch(kFALSE);
    h1->Draw();
    if (debug>1) Printf("Done with redraw ana");
    //gROOT->SetBatch(kTRUE);
  }
  else if (
      ana == "writePostPan"
   || ana == "postpan"){
    writePostPanFile_h(runNumber, splitNumber);
  }
  else if (
      ana == "regress"
   || ana == "regressor"
   || ana == "regression"
   || ana == "reg"){
    regress_h( tree, runNumber, minirunNumber, splitNumber, nRuns, "regressor/regressionInput.txt", ' ');
  }
/*  else if (
      ana == "writeValue"
   || ana == "writeNumber"
   || ana == "write"){
    writeFile_h(tree, branch, runNumber, minirunNumber, splitNumber, nRuns)
  }*/
  else
  {
    Printf("Standard form: ./camguin.C(string \"type of analysis\" (rms), string \"tree\" (mul), string \"branch\" (asym_vqwk_04_0ch0), string \"leaf\" (hw_sum), string \"cuts\" (defaultCuts), int overWriteCut (0, boolean to overwrite default cuts), string \"histMode\" (defaultHist, doesn't rebin), int runNumber ($RUNNUM), double nRuns ($NRUNS), double data (0.0))");
  }
  if (debug>2) Printf("Done with camguin.C");
  gApplication->Terminate();
}

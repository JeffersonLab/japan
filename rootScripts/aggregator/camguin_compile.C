#include "camguin.hh"
#include "camIO.hh"
#include "camHist.hh"
#include "camAna.hh"
#include "camMatrix.hh"
#include "regressor/camReg.hh"
using namespace std;
//void camguin(TString ana = "help", TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCuts", Int_t overWriteCut = 0, TString histMode = "defaultHist", Int_t stabilityRing = 0, Int_t runNumber = 0, Int_t splitNumber = -1, Int_t nRuns = -1){
int main(int argc, char **argv) {
  TString ana        = Form("%s", argv[1]);
  Printf("ana == %s",(const char*)ana);
  TString tree       = Form("%s", argv[2]);
  Printf("tree == %s",(const char*)tree);
  TString branch     = Form("%s", argv[3]);
  Printf("branch == %s",(const char*)branch);
  TString leaf       = Form("%s", argv[4]);
  Printf("leaf == %s",(const char*)leaf);
  TString cut        = Form("%s", argv[5]);
  Printf("cut == %s",(const char*)cut);
  Int_t overWriteCut  = atoi(argv[6]);
  Printf("overWriteCut == %d",overWriteCut);
  TString histMode   = Form("%s", argv[7]);
  Printf("histMode == %s",(const char*)histMode);
  Int_t stabilityRing = atoi(argv[8]);
  Printf("stabilityRing == %d",stabilityRing);
  Int_t runNumber     = atoi(argv[9]);
  Printf("runNumber == %d",runNumber);
  Int_t splitNumber   = atoi(argv[10]);
  Printf("splitNumber == %d",splitNumber);
  Int_t nRuns         = atoi(argv[11]);
  Printf("nRuns == %d",nRuns);
  Double_t data         = atoi(argv[12]);
  Printf("data == %f",data);

  /*TString ana = "help";
  TString tree = "mul"; 
  TString branch = "asym_vqwk_04_0ch0";
  TString leaf = "hw_sum";
  TString cut = "defaultCuts";
  Int_t overWriteCut = 0;
  TString histMode = "defaultHist";
  Int_t stabilityRing = 0;
  Int_t runNumber = 0;
  Int_t splitNumber = -1;
  Int_t nRuns = -1;*/

  getDebug_h();
  getAlarmStatus_h();
  getAggregatorStatus_h();
  if (debug>0) Printf("Analysis: %s",(const char*) ana);
  if( 
       ana == "Integral" 
    || ana == "integral" 
    || ana == "Sum" 
    || ana == "sum"){
    //writeInt_loop_h( tree, branch, leaf, cut, runNumber, splitNumber, nRuns );
    writeInt_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with integral ana");
  }
  else if( 
       ana == "intloop"){
    writeInt_loop_h( tree, branch, leaf, cut, overWriteCut, runNumber, splitNumber, nRuns );
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
    writeNEvents_Loop_h( tree, branch, leaf, cut, overWriteCut, stabilityRing, runNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with nEvents loop ana");
  }
  else if( 
       ana == "eventsloopn"
    || ana == "eventloopn"
    || ana == "eventloopN"
    || ana == "eventLoopN"
    || ana == "eventsLoopN"
    || ana == "eventsloopN"
    || ana == "EventLoop"){
    writeEventLoopN_Loop_h( tree, branch, leaf, cut, overWriteCut, stabilityRing, runNumber, splitNumber, nRuns );
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
    writeRMS_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, splitNumber, nRuns );
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
    writeMean_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with mean ana");
  }
  else if (
       ana == "meanrms"
    || ana == "meanRMS"
    || ana == "MeanRms"
    || ana == "meanRms"
    || ana == "mean_rms"
    || ana == "mean and rms"
    || ana == "mean&&rms"){
    writeMeanRms_leafHist_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, splitNumber, nRuns );
    if (debug>1) Printf("Done with meanrms ana");
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
    TH1* h1 = getHistogram_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, splitNumber, nRuns );
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
    TH1* h1 = getHistogram_h( tree, branch, leaf, cut, overWriteCut, histMode, runNumber, splitNumber, nRuns );
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
   || ana == "reg"){
    regress_h( tree, runNumber, splitNumber, nRuns, "regressor/regressionInput.txt", ' ');
  }
/*  else if (
      ana == "writeValue"
   || ana == "writeNumber"
   || ana == "write"){
    writeFile_h(tree, branch, runNumber, splitNumber, nRuns)
  }*/
  else
  {
    Printf("Standard form: ./camguin.C(string \"type of analysis\" (rms), string \"tree\" (mul), string \"branch\" (asym_vqwk_04_0ch0), string \"leaf\" (hw_sum), string \"cuts\" (defaultCuts), int overWriteCut (0, boolean to overwrite default cuts), string \"histMode\" (defaultHist, doesn't rebin), int runNumber ($RUNNUM), int nRuns ($NRUNS), double data (0.0))");
  }
  if (debug>2) Printf("Done with camguin.C");
  return 1;
}

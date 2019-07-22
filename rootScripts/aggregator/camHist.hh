#ifndef __CAMHIST__
#define __CAMHIST__
#include "camguin.hh"

#include <TH1.h>
#include <TROOT.h>
#include <TLeaf.h>
using namespace std;
TH1 * rebinTH1_h(TH1 * h1, TString mode = "manual", Int_t lookAbove = 0, Int_t lookAround = 0, Int_t nBinsX = 0, Int_t sampleFactor = 0, Double_t lowBinValX = 0.0, Double_t highBinValX = 0.0){

  //gROOT->SetBatch(kTRUE);

  if (sampleFactor == 0){
    sampleFactor = 1;
  }
  if (nBinsX == 0){
    nBinsX = (Int_t)(h1->GetNbinsX()/sampleFactor);
  }
  if (lowBinValX == 0.0 && highBinValX == 0.0){
    lowBinValX  = h1->GetBinCenter(h1->FindFirstBinAbove(lookAbove,TH1::kXaxis) - lookAround);
    highBinValX = h1->GetBinCenter(h1->FindLastBinAbove(lookAbove,TH1::kXaxis)  + lookAround);
    if (debug>2) Printf("Low: %f",lowBinValX);//  = h1->GetBinContent(h1->FindFirstBinAbove(lookAbove,1) - lookAround);
    if (debug>2) Printf("high: %f",highBinValX);//highBinValX = h1->GetBinContent(h1->FindLastBinAbove(lookAbove,1)  + lookAround);
  }

  TString title    = h1->GetTitle();
  TString xTitle   = h1->GetXaxis()->GetTitle();
  TString yTitle   = h1->GetYaxis()->GetTitle();
  TString name     = h1->GetName();
  Int_t old_nBinsX = h1->GetNbinsX();

  TH1 *h2 = new TH1F((const char*)(name+"_rebin"),(const char*)("Rebinned: "+title),nBinsX,lowBinValX,highBinValX);
  if (debug>2) Printf("name = %s_rebin, title = Rebinned: %s, nBinsX = %d, lowBinValX = %f, highBinValX = %f",(const char*)name,(const char*)title,nBinsX,lowBinValX,highBinValX);
  h2->GetXaxis()->SetTitle(xTitle);h2->GetXaxis()->CenterTitle();
  h2->GetYaxis()->SetTitle(yTitle);h2->GetYaxis()->CenterTitle();

  // Generate empty Hist
  if (mode == "clean"){
    if (debug > 2) Printf("clean\n");
    h2->GetXaxis()->SetCanExtend(false);
  }
  else if (mode == "manual"){
    if (debug > 2) Printf("manual\n");
    h2->GetXaxis()->SetCanExtend(true);
  }
  // Loop rebin, down or upsample
  else if (mode == "loop"){
    if (debug > 2) Printf("loop\n");
    for (Int_t bin = 0; bin < old_nBinsX; bin++) {
      h2->Fill(h1->GetBinCenter(bin));
    }
  }
  // Auto rebin, down sample by factor sampleFactor
  else if (mode == "auto"){
    if (debug > 2) Printf("auto\n");
    h2 = h1->Rebin(sampleFactor,name+"_rebin");
  }

  return h2;
}

TString getCuts_h(TString cut = "defaultCut", Int_t overWriteCut = 0, TString branchToCheck = "asym_vqwk_04_0ch0"){
  TString defaultCut = "ErrorFlag==0"; // Default just errflag
  //TString defaultCut = "(ErrorFlag==0 && "+branchToCheck+".Device_Error_Code==0)"; // Make the default JAPAN cut on the user's given branch (assumes its a device ... excplicitly use noCut for non-device branches
  if (cut == "defaultCut" || cut == "default" || cut == "def" || cut == "defaultCuts" || cut == "prex" || cut == "PREX"){
    if (debug > 1) Printf("Cut = %s",(const char*)defaultCut);
    return defaultCut; // Just return the default JAPAN cut
  }
  if (cut == "noCut" || cut == "1"){
    return "1"; // Just return true
  }
  cut.ToLower();
  if (cut == "minirun" || cut == "miniruns"){ // Flag word by itself, then check env variable
    Int_t minirunNumber = getMinirunNumber_h(-2); // Check the minirun number
    if (minirunNumber >= 0){
      cut = Form("(ErrorFlag==0 && minirun==%i)",minirunNumber);
      //cut = Form("(ok_cut==1 && minirun==%d)",minirunNumber);
      //cut = Form("(ok_cut==1 && minirun==%d)",minirunNumber); // Use native ok_cut version of ErrorFlag instead of assuming that friending with "mul" tree will work FIXME this assumes we want ErrorFlag==0 and leaves no room for creativity
      return cut;
    }
    else {
      Printf("Note: invalid minirun number, using ok_cut instead on full run");
      cut = "(ok_cut==1)";
      return cut;
    }
  } // Else, if you want "minirun==#" that will still work by hand, this if is just a flag to check env variable method instead...
  if (overWriteCut == 1){
    return cut; // Just return the user's cut
  }
  cut = "(" + cut + ")&&" + defaultCut; // Add the user cut to default JAPAN cut
  return cut;
}

TH1 * getHistogram_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber           = getRunNumber_h(runNumber);
  splitNumber         = getSplitNumber_h(splitNumber);
  minirunNumber       = getMinirunNumber_h(minirunNumber);
  nRuns               = getNruns_h(nRuns);
  cut                 = getCuts_h(cut,overWriteCut,branch);
  // Make an instance of the relevant data source 
  TLeaf   *Leaf       = getLeaf_h(tree,branch,leaf,runNumber,minirunNumber,splitNumber,nRuns);
  if (!Leaf){
    return 0;
  }
  TString leafName = "NULL";
  if (leaf==branch || branch=="" || branch=="NULL" || leaf=="" || leaf=="NULL") // Just use the leaf name
  {
    Printf("Looking for leaf instead\n");
    leafName = (TString)Leaf->GetName();
  }
  else
  {
    leafName = branch+"."+(TString)Leaf->GetName();
  }
  TBranch *Branch     = Leaf->GetBranch();
  TTree   *Tree       = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  gROOT->SetBatch(kTRUE);
  if (debug > 2) Printf("Leaf name: %s",(const char*)leafName);
  Tree->Draw(Form("%s",(const char*)leafName),(const char*)cut,"");
  if (debug > 2) Printf("%s->Draw(\"%s\",\"%s\",\"\")",(const char*)tree,(const char*)leafName,(const char*) cut);
  TH1 *h1 = (TH1*)gROOT->FindObject("htemp");
  if (debug > 2) Printf("Histogram mean = %f",h1->GetMean());
  TH1 *h2 = new TH1F();
  
  if (mode == "defaultHist" || mode == "default" || mode == "normal"){
    if (debug > 1) Printf("Run %d histogram of branch %s returned",runNumber,(const char*)leafName);
    return h1;
  }
  else if (mode == "clean" || mode == "manual"){
    h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
    TString h2_name = h2->GetName();
    Tree->Draw(Form("%s>>%s",(const char*)leafName,(const char*)h2_name),cut,""); // Manual
  }
  else if (mode == "auto" || mode == "loop"){
    h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
  }

  if (debug > 1) Printf("Run %d histogram of branch %s returned",runNumber,(const char*)leafName);
  return h2; 
}

TH1 * getWeightedHistogram_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString weight = "1", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber           = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns               = getNruns_h(nRuns);
  cut                 = getCuts_h(cut,overWriteCut,branch);
  // Make an instance of the relevant data source 
  TLeaf   *Leaf       = getLeaf_h(tree,branch,leaf,runNumber,minirunNumber,splitNumber,nRuns);
  if (!Leaf){
    return 0;
  }
  TString leafName = "NULL";
  if (leaf==branch || branch=="" || branch =="NULL" || leaf=="" || leaf=="NULL")
  {
    Printf("Looking for leaf instead\n");
    leafName = (TString)Leaf->GetName();
  }
  else
  {
    leafName = branch+"."+(TString)Leaf->GetName();
  }
  TBranch *Branch     = Leaf->GetBranch();
  TTree   *Tree       = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  gROOT->SetBatch(kTRUE);
  if (debug>2) Printf("Leaf name: %s",(const char*)leafName);
  Tree->Draw(Form("%s",(const char*)leafName),weight+"*("+cut+")","");
  if (debug>2) Printf("Tree->Draw(\"%s>>h1,%s,\"\")",(const char*)leafName,(const char*) cut);
  TH1 *h1 = (TH1*)gROOT->FindObject("htemp");
  if (debug>2) Printf("Histogram mean = %f",h1->GetMean());
  TH1 *h2 = new TH1F();
  
  if (mode == "defaultHist" || mode == "default" || mode == "normal"){
    if (debug>1) Printf("Run %d histogram of branch %s returned",runNumber,(const char*)leafName);
    return h1;
  }
  else if (mode == "clean" || mode == "manual"){
    h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
    TString h2_name = h2->GetName();
    Tree->Draw(Form("%s>>%s",(const char*)leafName,(const char*)h2_name),weight+"*("+cut+")",""); // Manual
  }
  else if (mode == "auto" || mode == "loop"){
    h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
  }

  if (debug>1) Printf("Run %d histogram of branch %s returned",runNumber,(const char*)leafName);
  return h2; 
}

void writeInt_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString weight = "NULL";
  TString integral = "NULL";
  if (leaf==branch || branch == "" || branch == "NULL")
  {
    weight   = leaf;
    integral = "integral_" + leaf;
  }
  else if (leaf==branch || leaf == "" || leaf == "NULL")
  {
    weight   = branch;
    integral = "integral_" + branch;
  }
  else
  {
    weight   = branch+"."+leaf;
    integral = "integral_" + branch + "_" + leaf;
  }
  Double_t data_integral = 0.0;
  TH1 * h1_int = getWeightedHistogram_h(tree,branch,leaf,weight,cut,overWriteCut,mode,runNumber,minirunNumber,splitNumber,nRuns);
  if (h1_int ==0)
  { 
    Printf("Error, Histogram failed");
  }
  else {
    data_integral = h1_int->Integral();
  }
  if (debug>1) Printf("Run %d integral %s: %f",runNumber,(const char*)integral,data_integral);
  if (aggregatorStatus){
    writeFile_h(integral,data_integral,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    Printf("%s=%f",(const char*)integral,data_integral); // Then the alarm handler wants to receive the output in stdout
  }
}


// NEW FIXME


void writeMeanRms_h(TString tree = "mul", TString draw = "asym_vqwk_04_0ch0", TString units = "", TString cut = "defaultCut", Int_t overWriteCut = 0,  Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString mean = draw + "_mean";
  TString mean_error  = draw + "_mean_error";
  TString rms = draw + "_rms";
  TString rms_error = draw + "_rms_error";
  TString nEntries = draw + "_nentries";
  Double_t data_mean = 0;
  Double_t data_mean_error = 0;
  Double_t data_rms = 0;
  Double_t data_rms_error = 0;
  TChain * Tree = getTree_h(tree, runNumber, minirunNumber, splitNumber, nRuns);
  cut = getCuts_h(cut, overWriteCut, "NULL"); // FIXME in postPan era, branchToCheck in cuts method is not even used, but when it is, it will need to be parsed correctly - probably just ignore this from now on and make Device_Error_Code something done explicitly (since so many variables in Japan actually aren't devices)
  Int_t n_entries = Tree->Draw(Form("%s%s",draw.Data(),units.Data()),Form("%s",cut.Data()));
  //Int_t n_entries = Tree->Draw("yield_bcm_an_ds","ErrorFlag==0 && minirun==0");
  TH1 * hMeanRms = (TH1*)gROOT->FindObject("htemp");
  if (hMeanRms==0)
  {
    Printf("Error, writeMeanRms_h: Histogram failed");
  }
  else {
    data_mean = hMeanRms->GetMean(1);
    data_mean_error = hMeanRms->GetMeanError(1);
    data_rms = hMeanRms->GetRMS(1);
    data_rms_error = hMeanRms->GetRMSError(1);
  }

  if (debug>1) Printf("Run %d mean %s: %f+-%f",runNumber,(const char*)mean,data_mean,data_mean_error);
  if (debug>1) Printf("Run %d rms %s: %f+-%f",runNumber,(const char*)rms,data_rms,data_rms_error);
  if (aggregatorStatus){
    writeFile_h(mean,data_mean,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(mean_error,data_mean_error,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms,data_rms,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms_error,data_rms_error,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(nEntries,n_entries,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)mean,data_mean);
    Printf("%s=%f",(const char*)mean_error,data_mean_error);
    Printf("%s=%f",(const char*)rms,data_rms);
    Printf("%s=%f",(const char*)rms_error,data_rms_error);
    Printf("%s=%d",(const char*)nEntries,n_entries);
  }
}




// END NEW FIXME

/*

void writeSlopes_h(TString tree = "mul", TString draw = "asym_vqwk_04_0ch0", TString units = "", TString cut = "defaultCut", Int_t overWriteCut = 0,  Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString slope = draw + "_slope";
  TString slope_error  = draw + "_slope_error";
  Double_t data_slope = 0;
  Double_t data_slope_error = 0;
  Double_t data_rms = 0;
  Double_t data_rms_error = 0;
  TChain * Tree = getTree_h(tree, runNumber, minirunNumber, splitNumber, nRuns);
  cut = getCuts_h(cut, overWriteCut, "NULL"); // FIXME in postPan era, branchToCheck in cuts method is not even used, but when it is, it will need to be parsed correctly - probably just ignore this from now on and make Device_Error_Code something done explicitly (since so many variables in Japan actually aren't devices)
  Int_t n_entries = Tree->Draw(Form("%s%s",draw.Data(),units.Data()),Form("%s",cut.Data()));
  //Int_t n_entries = Tree->Draw("yield_bcm_an_ds","ErrorFlag==0 && minirun==0");
  TH1 * hMeanRms = (TH1*)gROOT->FindObject("htemp");
  if (hMeanRms==0)
  {
    Printf("Error, writeMeanRms_h: Histogram failed");
  }
  else {
    data_mean = hMeanRms->GetMean(1);
    data_mean_error = hMeanRms->GetMeanError(1);
    data_rms = hMeanRms->GetRMS(1);
    data_rms_error = hMeanRms->GetRMSError(1);
  }

  if (debug>1) Printf("Run %d mean %s: %f+-%f",runNumber,(const char*)mean,data_mean,data_mean_error);
  if (debug>1) Printf("Run %d rms %s: %f+-%f",runNumber,(const char*)rms,data_rms,data_rms_error);
  if (aggregatorStatus){
    writeFile_h(mean,data_mean,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(mean_error,data_mean_error,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)mean,data_mean);
    Printf("%s=%f",(const char*)mean_error,data_mean_error);
    Printf("%s=%f",(const char*)rms,data_rms);
    Printf("%s=%f",(const char*)rms_error,data_rms_error);
    Printf("%s=%d",(const char*)nEntries,n_entries);
  }
}


*/




void writeMeanRms_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString mean = "NULL";
  TString mean_error  = "NULL";
  TString rms = "NULL";
  TString rms_error = "NULL";
  if (leaf==branch || branch == "" || branch == "NULL")
  {
    mean = "mean_" + leaf;
    mean_error = "mean_" + leaf + "_error";
    rms = "rms_" + leaf;
    rms_error = "rms_" + leaf + "_error";
  }
  else if (leaf==branch || leaf == "" || leaf == "NULL")
  {
    mean = "mean_" + branch;
    mean_error = "mean_" + branch + "_error";
    rms = "rms_" + branch;
    rms_error = "rms_" + branch + "_error";
  }
  else
  {
    mean = "mean_" + branch + "_" + leaf;
    mean_error = "mean_" + branch + "_" + leaf + "_error";
    rms = "rms_" + branch + "_" + leaf;
    rms_error = "rms_" + branch + "_" + leaf + "_error";
  }
  Double_t data_mean = 0;
  Double_t data_mean_error = 0;
  Double_t data_rms = 0;
  Double_t data_rms_error = 0;
  TH1 * hMeanRms = getHistogram_h(tree,branch,leaf,cut,overWriteCut,mode,runNumber,minirunNumber,splitNumber,nRuns);
  if (hMeanRms==0)
  {
    Printf("Error, writeMeanRms_leafHist_h: Histogram failed");
  }
  else {
    data_mean = hMeanRms->GetMean(1);
    data_mean_error = hMeanRms->GetMeanError(1);
    data_rms = hMeanRms->GetRMS(1);
    data_rms_error = hMeanRms->GetRMSError(1);
  }

  if (debug>1) Printf("Run %d mean %s: %f+-%f",runNumber,(const char*)mean,data_mean,data_mean_error);
  if (debug>1) Printf("Run %d rms %s: %f+-%f",runNumber,(const char*)rms,data_rms,data_rms_error);
  if (aggregatorStatus){
    writeFile_h(mean,data_mean,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(mean_error,data_mean_error,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms,data_rms,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms_error,data_rms_error,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)mean,data_mean);
    Printf("%s=%f",(const char*)mean_error,data_mean_error);
    Printf("%s=%f",(const char*)rms,data_rms);
    Printf("%s=%f",(const char*)rms_error,data_rms_error);
  }
}

void writeMean_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString mean = "NULL";
  TString mean_error  = "NULL";
  if (leaf==branch || branch == "" || branch == "NULL")
  {
    mean = "mean_" + leaf;
    mean_error = "mean_" + leaf + "_error";
  }
  else if (leaf==branch || leaf == "" || leaf == "NULL")
  {
    mean = "mean_" + branch;
    mean_error = "mean_" + branch + "_error";
  }
  else
  {
    mean = "mean_" + branch + "_" + leaf;
    mean_error = "mean_" + branch + "_" + leaf + "_error";
  }
  Double_t data_mean = 0;
  Double_t data_mean_error = 0;
  TH1 * hMean = getHistogram_h(tree,branch,leaf,cut,overWriteCut,mode,runNumber,minirunNumber,splitNumber,nRuns);
  if (hMean==0)
  {
    Printf("Error, Histogram failed");
  }
  else {
    data_mean = hMean->GetMean(1);
    data_mean_error = hMean->GetMeanError(1);
  }

  if (debug>1) Printf("Run %d mean %s: %f+-%f",runNumber,(const char*)mean,data_mean,data_mean_error);
  if (aggregatorStatus){
    writeFile_h(mean,data_mean,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(mean_error,data_mean_error,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)mean,data_mean); 
    Printf("%s=%f",(const char*)mean_error,data_mean_error); 
  }
}

void writeRMS_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString rms = "NULL";
  TString rms_error = "NULL";
  if (leaf==branch || branch == "" || branch == "NULL")
  {
    rms = "rms_" + leaf;
    rms_error = "rms_" + leaf + "_error";
  }
  else if (leaf==branch || leaf == "" || leaf == "NULL")
  {
    rms = "rms_" + branch;
    rms_error = "rms_" + branch + "_error";
  }
  else
  {
    if (debug>3) Printf("defining strings for printing data type");
    rms = "rms_" + branch + "_" + leaf;
    rms_error = "rms_" + branch + "_" + leaf + "_error";
  }
  Double_t data_rms = 0;
  Double_t data_rms_error = 0;
  TH1 * hRms = getHistogram_h(tree,branch,leaf,cut,overWriteCut,mode,runNumber,minirunNumber,splitNumber,nRuns);
  if (hRms==0)
  {
    Printf("Error, Histogram failed");
  }
  else {
    data_rms = hRms->GetRMS(1);
    data_rms_error = hRms->GetRMSError(1);
  }

  if (debug>1) Printf("Run %d rms %s: %f+-%f",runNumber,(const char*)rms,data_rms,data_rms_error);
  if (aggregatorStatus){
    writeFile_h(rms,data_rms,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms_error,data_rms_error,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)rms,data_rms); 
    Printf("%s=%f",(const char*)rms_error,data_rms_error); 
  }
}
#endif // __CAMHIST__

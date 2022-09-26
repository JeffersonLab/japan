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
  cut.ToLower();
  if (cut == "defaultcut" || cut == "default" || cut == "def" || cut == "defaultcuts" || cut == "prex" ){
    if (debug > 1) Printf("Cut = %s",(const char*)defaultCut);
    return defaultCut; // Just return the default JAPAN cut
  }
  if (cut == "nocut" || cut == "1"){
    return "1"; // Just return true
  }
  if (cut == "minirun" || cut == "miniruns"){ // Flag word by itself, then check env variable
    Int_t minirunNumber = getMinirunNumber_h(-2); // Check the minirun number
    if (minirunNumber >= 0){
      cut = Form("(ok_cut==1 && minirun==%i)",minirunNumber);
      //cut = Form("(ok_cut==1 && minirun==%d)",minirunNumber);
      //cut = Form("(ok_cut==1 && minirun==%d)",minirunNumber); // Use native ok_cut version of ErrorFlag instead of assuming that friending with "mul" tree will work FIXME this assumes we want ErrorFlag==0 and leaves no room for creativity
      return cut;
    }
    else {
      Printf("Note: invalid minirun number, using ErrorFlag instead on full run");
      cut = "(ErrorFlag==0)";
      return cut;
    }
  } // Else, if you want "minirun==#" that will still work by hand, this if is just a flag to check env variable method instead...
  if (overWriteCut == 1){
    return cut; // Just return the user's cut
  }
  cut = "(" + cut + ")&&" + defaultCut; // Add the user cut to default JAPAN cut
  return cut;
}

TH1 * getHistogram_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
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

TH1 * getWeightedHistogram_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString weight = "1", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
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

void writeInt_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
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

void writeMeanRms_muls(TString tree = "muls", TString branch_title = "asym_vqwk_04_0ch0", TString units = "", TString cut = "defaultCut", Int_t overWriteCut = 0,  Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString mean = branch_title + "_mean";
  TString mean_error  = branch_title + "_mean_error";
  TString rms = branch_title + "_rms";
  TString rms_error = branch_title + "_rms_error";
  TString nEntries = branch_title + "_nentries";
  Double_t data_mean = 0;
  Double_t data_mean_error = 0;
  Double_t data_rms = 0;
  Double_t data_rms_error = 0;
  TChain* mulsChain = getMuls(tree, runNumber, minirunNumber, splitNumber, nRuns);
  cut = getCuts_h(cut, overWriteCut, "NULL"); 
  TBranch* this_branch = mulsChain->GetBranch(branch_title);
  TLeaf* mean_leaf= this_branch->GetLeaf("hw_sum");
  TLeaf* m2_leaf = this_branch->GetLeaf("hw_sum_m2");
  TLeaf* err_leaf = this_branch->GetLeaf("hw_sum_err");
  TLeaf* nevent_leaf = this_branch->GetLeaf("num_samples");
  TLeaf* unit_leaf;
  double scale_factor = 1.0;
  if(units!=NULL || units!=""){
    TBranch* unit_branch = mulsChain->GetBranch("units");
    if(unit_branch!=NULL)
      unit_leaf= unit_branch->GetLeaf(units);
  }
    
  mulsChain->GetEntry(0);
  if(unit_leaf!=NULL)
    scale_factor=unit_leaf->GetValue();
  Int_t n_entries = nevent_leaf->GetValue();
  if (n_entries==0)
  {
    Printf("Error, writeMeanRms_muls: Histogram failed");
  }
  else {
    data_mean = mean_leaf->GetValue()/scale_factor;
    data_mean_error = err_leaf->GetValue()/scale_factor;
    data_rms = TMath::Sqrt((m2_leaf->GetValue()/nevent_leaf->GetValue()))/scale_factor;
    data_rms_error = 0.0;
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

// NEW FIXME


void writeMeanRms_h(TString tree = "mul", TString draw = "asym_vqwk_04_0ch0", TString units = "", TString cut = "defaultCut", Int_t overWriteCut = 0,  Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
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

void writeCorrectionMeanRms_h(TString tree = "mul", TString draw = "asym_vqwk_04_0ch0", TString units = "", TString cut = "defaultCut", Int_t overWriteCut = 0,  Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString mean = draw + "_correction_mean";
  TString mean_error  = draw + "_correction_mean_error";
  TString rms = draw + "_correction_rms";
  TString rms_error = draw + "_correction_rms_error";
  TString nEntries = draw + "_correction_nentries";
  if (tree == "reg"){
    draw = "reg_" + draw + "-" + draw; // Make the actual draw command now
  }
  if (tree == "dit"){
    draw = "dit_" + draw + "-" + draw; // Make the actual draw command now
  }
  Double_t data_mean = 0;
  Double_t data_mean_error = 0;
  Double_t data_rms = 0;
  Double_t data_rms_error = 0;
  TChain * Tree = getTree_h(tree, runNumber, minirunNumber, splitNumber, nRuns);
  // Technically for correction you need a smarter way to decide both object's OK/device error/error flag cuts
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

void writeCombinedMeanRms_h(TString tree = "mul", TString draw = "asym_vqwk_04_0ch0", TString name = "", TString cut = "defaultCut", Int_t overWriteCut = 0,  Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber     = getRunNumber_h(runNumber);
  splitNumber   = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns         = getNruns_h(nRuns);
  TString mean  = name + "_mean";
  TString mean_error = name + "_mean_error";
  TString rms   = name + "_rms";
  TString rms_error  = name + "_rms_error";
  TString nEntries   = name + "_nentries";
  Double_t data_mean = 0;
  Double_t data_mean_error = 0;
  Double_t data_rms  = 0;
  Double_t data_rms_error  = 0;
  TChain * Tree = getTree_h(tree, runNumber, minirunNumber, splitNumber, nRuns);
  cut = getCuts_h(cut, overWriteCut, "NULL"); // FIXME in postPan era, branchToCheck in cuts method is not even used, but when it is, it will need to be parsed correctly - probably just ignore this from now on and make Device_Error_Code something done explicitly (since so many variables in Japan actually aren't devices)
  Int_t n_entries = Tree->Draw(Form("%s",draw.Data()),Form("%s",cut.Data()));
  //Int_t n_entries = Tree->Draw("yield_bcm_an_ds","ErrorFlag==0 && minirun==0");
  TH1 * hMeanRms = (TH1*)gROOT->FindObject("htemp");
  if (hMeanRms==0)
  {
    Printf("Error, writeCombinedMeanRms_h: Histogram failed");
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


void writeSlope_h(Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  Double_t data_slope = 0;
  Double_t data_slope_error = 0;

  TString ditSlopeFileNamebase = gSystem->Getenv("DITHERING_ROOTFILES_SLOPES");
  TString ditSlopeFileName = ditSlopeFileNamebase + "/dit_slopes_slug" + nRuns + ".root";
  if( !gSystem->AccessPathName(ditSlopeFileName) ) {
    Printf("Getting dithering slopes from %s",ditSlopeFileName.Data());
    TChain *ditTree = new TChain("dit");
    ditTree->Add(ditSlopeFileName);
    TLeaf *ditRunNum = ditTree->GetLeaf("run");
    TObjArray *slopesList = ditTree->GetListOfLeaves();
    TString outname = "";
    for (Int_t a = 0; a<ditTree->GetEntries(); a++){
      ditTree->GetEntry(a);
      if (debug>3) Printf("Entry number %d, run number %d",a,(Int_t)ditRunNum->GetValue(0));
      TIter slopesIter(slopesList);
      while (TLeaf *slopes=(TLeaf*)slopesIter.Next()){
      //for (Int_t c = 0; c<slopesList->GetEntries(); c++){
        if (debug>4) Printf("Checking dither slope %s",((TString)slopes->GetName()).Data());
        if ((TString)slopes->GetName() != "run" && (Int_t)ditRunNum->GetValue(0) == runNumber){
        //if ((TString)((TLeaf*)slopesList.at(c))->GetName() != "run" && ditRunNum->GetValue(0) == runNumber){
          //writeFile_h("dit_"+(TString)((TLeaf*)slopesList.at(c))->GetName()+"_slope",(Double_t)(TLeaf*)slopesList.at(c)->GetValue(0),runNumber,minirunNumber,splitNumber,nRuns);
          outname = "dit_"+(TString)slopes->GetName()+"_slope";
          if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),(Double_t)slopes->GetValue(0));
          writeFile_h("dit_"+(TString)slopes->GetName()+"_slope",(Double_t)slopes->GetValue(0),runNumber,minirunNumber,splitNumber,nRuns);
        }
      }
    }
  }
  if(minirunNumber<0){
    TString lrbFileNameBase = gSystem->Getenv("LRB_ROOTFILES");
    TString lrbFileName = lrbFileNameBase + "/blueR"+runNumber+".000new.slope.root";  
    TFile f(lrbFileName);

    std::map<TString, Int_t> IVname;
    std::map<TString, Int_t> DVname;

    TH1D* m=(TH1D*) f.Get("IVname");
    for (auto i=0; i<m->GetEntries();i++){
      TString ivname=m->GetXaxis()->GetBinLabel(i+1);
      IVname[ivname]=i;
    }

    TH1D* n=(TH1D*) f.Get("DVname");
    for (auto i=0; i<n->GetEntries(); i++){
      TString dvname=n->GetXaxis()->GetBinLabel(i+1);
      DVname[dvname]=i;
    }


    TMatrixT<double> slopes=*(TMatrixT<double>*) f.Get("slopes");
    TMatrixT<double> sigSlopes=*(TMatrixT<double>*) f.Get("sigSlopes");
    for (auto& i: DVname){ 
      for (auto& j: IVname){
        writeFile_h("cor_"+i.first+"_"+j.first+ "_slope", slopes(j.second,i.second),runNumber, minirunNumber, splitNumber, nRuns);
        writeFile_h("cor_"+i.first+"_"+j.first+ "_slope_error", sigSlopes(j.second,i.second),runNumber, minirunNumber, splitNumber, nRuns);
      }
    }
  }
  if (minirunNumber>=0){
    TString lpostpanFileNameBase = gSystem->Getenv("POSTPAN_ROOTFILES");
    TString lpostpanFileName = lpostpanFileNameBase + "/prexPrompt_" + runNumber + "_000_regress_postpan.root";
    TFile f(lpostpanFileName);

    TTreeReader theReader("mini", &f);
    TTreeReaderArray<double> slope(theReader,"coeff");
    TTreeReaderArray<double> slope_err(theReader,"err_coeff");

    std::vector<TString> ivname= *(std::vector<TString>*) f.Get("IVNames");
    std::vector<TString> dvname= *(std::vector<TString>*) f.Get("DVNames");
    std::vector<TString> comb;

    for (auto &i:dvname){
      for (auto &j:ivname){
        comb.push_back(i+"_"+j);
      }
    }

    Int_t mini=0;
    while(theReader.Next()){
      Int_t count=0;
      for (auto i=0; i<slope.GetSize(); i++){
        if (mini!=minirunNumber) continue;
        writeFile_h("reg_"+comb.at(count)+ "_slope", slope[i],runNumber, minirunNumber, splitNumber, nRuns);
        writeFile_h("reg_"+comb.at(count)+ "_slope_error", slope_err[i], runNumber, minirunNumber, splitNumber, nRuns);
        count++;
      }
      mini++; 
    }
  }	
}

// END NEW FIXME


void writeMeanRms_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
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

void writeMean_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
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

void writeRMS_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
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

#ifndef __CAMANA__
#define __CAMANA__
#include "camguin.hh"
#include <TLeaf.h>
using namespace std;
Int_t writeNEvents_Loop_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, Int_t stabilityRingLength = 0, Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  // Any branch will do, we are just counting the number of events that pass the global EventCuts, not device error codes too
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString channel = tree + "_" + branch + "_" + leaf;
  // Make an instance of the relevant data source 
  TLeaf   *Leaf   = getLeaf_h(tree,branch,leaf,runNumber,minirunNumber,splitNumber,nRuns);
  if (!Leaf){
    Printf("Event counting terminated, branch not found");
    return 0;
  }
  TString leafName = "NULL";
  if (leaf==branch)
  {
    leafName = (TString)Leaf->GetName();
  }
  else
  {
    leafName = branch+"."+(TString)Leaf->GetName();
  }
  TBranch *Branch = Leaf->GetBranch();
  TTree   *Tree   = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  Int_t    cutValue = 0;
  TLeaf   *CutLeaf;
  TLeaf   *CutBranch;
  TString defaultCut = "(ErrorFlag==0 && "+branch+".Device_Error_Code==0)"; // Make the default JAPAN cut on the user's given branch (assumes its a device ... excplicitly use noCut for non-device branches
  if (cut == "defaultCut" || cut == "default" || cut == "def" || cut == "defaultCuts" || cut == "prex" || cut == "PREX"){
    if (debug > 1) Printf("Cut == %s",(const char*)defaultCut);
    CutBranch = Tree->GetLeaf("ErrorFlag");
    CutLeaf   = Branch->GetLeaf("Device_Error_Code");
    cutValue = 0;
  }
  else {
    CutLeaf = Tree->GetLeaf(tree,cut); // Assume the user adds to ErrorFlag with a passed single branch (non-device) that they want to compare to == 0 for "good" cuts
    cutValue = 0;
  }
  if (cut == "noCut" || cut == "1"){
    cutValue = 1;
  }
  if (overWriteCut == 1){ // Don't include ErrorFlag cut - user overwrote
    CutLeaf = Branch->GetLeaf(cut);
    CutBranch = CutLeaf;
    cutValue = 0;
  }
  // End cut setup

  Int_t    n_data = 0;
  Int_t    n_reverse_data = 0;
  Int_t    n_reverse_data_good = 0;
  Int_t    stabilityRingStart = 0;
  TString  number_total_events = channel+"_number_total_events";
  TString  number_good_events = channel+"_number_good_events";
  TString  number_stabilityRingStart = channel+"_number_stability_ring_start"; // Entry number that begins good cut passing stability ring of length stabilityRing
  TString  number_stabilityRingLength = channel+"_number_stability_ring_length"; // length of stabilityRing

  for (int j = 0; j < numEntries; j++) 
  { // Loop over the input file's entries
    Tree->GetEntry(j);
    if ( cutValue || (CutBranch->GetValue(0) == 0 && CutLeaf->GetValue(0) == 0) ){ // cutValue = 1 iff overwrote cuts with 1, branch==leaf if overwritecuts==1, branch and leaf defined by default cuts or with added on top of non-deviceerrorcode leaf
      n_data+=1;
    }
  }
  if (aggregatorStatus){
    writeFile_h(number_total_events,numEntries,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(number_good_events,n_data,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (stabilityRingLength!=0){
    for (int k = numEntries-1; k > 0 ; k--)
    { // Loop over input file's entries in reverse
      Tree->GetEntry(k);
      n_reverse_data++;
      if ( cutValue || (CutBranch->GetValue(0) == 0 && CutLeaf->GetValue(0) == 0) ){ // cutValue = 1 iff overwrote cuts with 1, branch==leaf if overwritecuts==1, branch and leaf defined by default cuts or with added on top of non-deviceerrorcode leaf
        n_reverse_data_good++;
      }
      if (n_reverse_data_good>=stabilityRingLength || n_reverse_data>numEntries)
      {
        break;
      }
    }
    stabilityRingStart=numEntries-n_reverse_data+1; // +1 since events start on 1
    if (aggregatorStatus){
      writeFile_h(number_stabilityRingStart,stabilityRingStart,runNumber,minirunNumber,splitNumber,nRuns);
      writeFile_h(number_stabilityRingLength,stabilityRingLength,runNumber,minirunNumber,splitNumber,nRuns);
    }
    if (alarmStatus){
      // Then the alarm handler wants to receive the output in stdout
      Printf("%s=%i",(const char*)number_stabilityRingStart,stabilityRingStart);
      Printf("%s=%i",(const char*)number_stabilityRingLength,stabilityRingLength);
    }
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%i",(const char*)number_total_events,numEntries);
    Printf("%s=%i",(const char*)number_good_events,n_data);
  }
  return stabilityRingStart;
}

Int_t writeEventLoopN_Loop_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, Int_t stabilityRingLength = 0, Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  // Any branch will do, we are just counting the number of events that pass the global EventCuts, not device error codes too
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString channel = tree + "_" + branch + "_" + leaf;
  // Make an instance of the relevant data source 
  TLeaf   *Leaf   = getLeaf_h(tree,branch,leaf,runNumber,minirunNumber,splitNumber,nRuns);
  if (!Leaf){
    Printf("Event counting terminated, branch not found");
    return 0;
  }
  TString leafName = "NULL";
  if (leaf==branch)
  {
    leafName = (TString)Leaf->GetName();
  }
  else
  {
    leafName = branch+"."+(TString)Leaf->GetName();
  }
  TBranch *Branch = Leaf->GetBranch();
  TTree   *Tree   = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  Int_t    cutValue = 0;
  TLeaf   *CutLeaf;
  TLeaf   *CutBranch;
  TString defaultCut = "(ErrorFlag==0 && "+branch+".Device_Error_Code==0)"; // Make the default JAPAN cut on the user's given branch (assumes its a device ... excplicitly use noCut for non-device branches
  if (cut == "defaultCut" || cut == "default" || cut == "def" || cut == "defaultCuts" || cut == "prex" || cut == "PREX"){
    if (debug > 1) Printf("Cut == %s",(const char*)defaultCut);
    CutBranch = Tree->GetLeaf("ErrorFlag");
    CutLeaf   = Branch->GetLeaf("Device_Error_Code");
    cutValue = 0;
  }
  else {
    CutLeaf = Tree->GetLeaf(tree,cut); // Assume the user adds to ErrorFlag with a passed single branch (non-device) that they want to compare to == 0 for "good" cuts
    cutValue = 0;
  }
  if (cut == "noCut" || cut == "1"){
    cutValue = 1;
  }
  if (overWriteCut == 1){ // Don't include ErrorFlag cut - user overwrote
    CutLeaf = Branch->GetLeaf(cut);
    CutBranch = CutLeaf;
    cutValue = 0;
  }
  // End cut setup

  Int_t    n_reverse_data = 0;
  Int_t    n_reverse_data_good = 0;
  Int_t    stabilityRingStart = 0;
  TString  number_total_events = channel+"_number_total_events";
  TString  number_good_events = channel+"_number_good_events";
  TString  number_stabilityRingStart = channel+"_number_stability_ring_start"; // Entry number that begins good cut passing stability ring of length stabilityRing
  TString  number_stabilityRingLength = channel+"_number_stability_ring_length"; // length of stabilityRing

  if (aggregatorStatus){
    writeFile_h(number_total_events,numEntries,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (stabilityRingLength!=0){
    for (int k = numEntries-1; k > 0 ; k--)
    { // Loop over input file's entries in reverse
      Tree->GetEntry(k);
      n_reverse_data++;
      if ( cutValue || (CutBranch->GetValue(0) == 0 && CutLeaf->GetValue(0) == 0) ){ // cutValue = 1 iff overwrote cuts with 1, branch==leaf if overwritecuts==1, branch and leaf defined by default cuts or with added on top of non-deviceerrorcode leaf
        n_reverse_data_good++;
      }
      if (n_reverse_data_good>=stabilityRingLength || n_reverse_data>numEntries)
      {
        break;
      }
    }
    stabilityRingStart=numEntries-n_reverse_data+1; // +1 since events start on 1
    if (aggregatorStatus){
      writeFile_h(number_stabilityRingStart,stabilityRingStart,runNumber,minirunNumber,splitNumber,nRuns);
      writeFile_h(number_stabilityRingLength,stabilityRingLength,runNumber,minirunNumber,splitNumber,nRuns);
    }
    if (alarmStatus){
      // Then the alarm handler wants to receive the output in stdout
      Printf("%s=%i",(const char*)number_stabilityRingStart,stabilityRingStart);
      Printf("%s=%i",(const char*)number_stabilityRingLength,stabilityRingLength);
    }
  }
  return stabilityRingStart;
}

void writeLast_Loop_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString channel = tree + "_" + branch + "_" + leaf;
  // Make an instance of the relevant data source 
  TLeaf   *Leaf   = getLeaf_h(tree,branch,leaf,runNumber,minirunNumber,splitNumber,nRuns);
  if (!Leaf){
    return;
  }
  TString leafName = "NULL";
  if (leaf==branch)
  {
    leafName = (TString)Leaf->GetName();
    channel = tree+"_"+branch;
  }
  else
  {
    leafName = branch+"."+(TString)Leaf->GetName();
  }
  TBranch *Branch = Leaf->GetBranch();
  TTree   *Tree   = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  Double_t data    = 0.0;
  TString analysis = channel+"_value";

  if (numEntries==0){
    data = 0.0;
  }
  else{
    Tree->GetEntry(numEntries-1);
    data=Leaf->GetValue(0);
  }
  if (aggregatorStatus){
    writeFile_h(analysis,data,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)analysis,data);
  }
}

void writeMean_Loop_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString channel = tree + "_" + branch + "_" + leaf;
  // Make an instance of the relevant data source 
  TLeaf   *Leaf   = getLeaf_h(tree,branch,leaf,runNumber,minirunNumber,splitNumber,nRuns);
  if (!Leaf){
    return;
  }
  TString leafName = "NULL";
  if (leaf==branch)
  {
    leafName = (TString)Leaf->GetName();
  }
  else
  {
    leafName = branch+"."+(TString)Leaf->GetName();
  }
  TBranch *Branch = Leaf->GetBranch();
  TTree   *Tree   = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  Int_t    cutValue = 0;
  TLeaf   *CutLeaf;
  TLeaf   *CutBranch;
  TString defaultCut = "(ErrorFlag==0 && "+branch+".Device_Error_Code==0)"; // Make the default JAPAN cut on the user's given branch (assumes its a device ... excplicitly use noCut for non-device branches
  if (cut == "defaultCut" || cut == "default" || cut == "def" || cut == "defaultCuts" || cut == "prex" || cut == "PREX"){
    if (debug > 1) Printf("Cut == %s",(const char*)defaultCut);
    CutBranch = Tree->GetLeaf("ErrorFlag");
    CutLeaf   = Branch->GetLeaf("Device_Error_Code");
    cutValue = 0;
  }
  else {
    CutLeaf = Tree->GetLeaf(tree,cut); // Assume the user adds to ErrorFlag with a passed single branch (non-device) that they want to compare to == 0 for "good" cuts
    cutValue = 0;
  }
  if (cut == "noCut" || cut == "1"){
    cutValue = 1;
  }
  if (overWriteCut == 1){ // Don't include ErrorFlag cut - user overwrote
    CutLeaf = Branch->GetLeaf(cut);
    CutBranch = CutLeaf;
    cutValue = 0;
  }
  // End cut setup

  Double_t data     = 0.0;
  Int_t    n_data   = 0;
  Double_t data_mean = 0.0;
  TString  analysis = "mean_"+channel;

  for (int j = 0; j < numEntries; j++) 
  { // Loop over the input file's entries
    Tree->GetEntry(j);
    if ( cutValue || (CutBranch->GetValue(0) == 0 && CutLeaf->GetValue(0) == 0) ){ // cutValue = 1 iff overwrote cuts with 1, branch==leaf if overwritecuts==1, branch and leaf defined by default cuts or with added on top of non-deviceerrorcode leaf
      data+=Leaf->GetValue(0);
      n_data+=1;
    }
  }
  data_mean = data/(1.0*n_data);
  if (aggregatorStatus){
    writeFile_h(analysis,data_mean,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)analysis,data_mean);
  }
}

void writeInt_loop_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  TString channel = tree + "_" + branch + "_" + leaf;
  // Make an instance of the relevant data source 
  TLeaf   *Leaf   = getLeaf_h(tree,branch,leaf,runNumber,minirunNumber,splitNumber,nRuns);
  if (!Leaf){
    return;
  }
  TString leafName = "NULL";
  if (leaf==branch)
  {
    leafName = (TString)Leaf->GetName();
  }
  else
  {
    leafName = branch+"."+(TString)Leaf->GetName();
  }
  TBranch *Branch = Leaf->GetBranch();
  TTree   *Tree   = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  Int_t    cutValue = 0;
  TLeaf   *CutLeaf;
  TLeaf   *CutBranch;
  TString defaultCut = "(ErrorFlag==0 && "+branch+".Device_Error_Code==0)"; // Make the default JAPAN cut on the user's given branch (assumes its a device ... excplicitly use noCut for non-device branches
  if (cut == "defaultCut" || cut == "default" || cut == "def" || cut == "defaultCuts" || cut == "prex" || cut == "PREX"){
    if (debug > 1) Printf("Cut == %s",(const char*)defaultCut);
    CutBranch = Tree->GetLeaf("ErrorFlag");
    CutLeaf   = Branch->GetLeaf("Device_Error_Code");
    cutValue = 0;
  }
  else {
    CutLeaf = Tree->GetLeaf(tree,cut); // Assume the user adds to ErrorFlag with a passed single branch (non-device) that they want to compare to == 0 for "good" cuts
    cutValue = 0;
  }
  if (cut == "noCut" || cut == "1"){
    cutValue = 1;
  }
  if (overWriteCut == 1){ // Don't include ErrorFlag cut - user overwrote
    CutLeaf = Branch->GetLeaf(cut);
    CutBranch = CutLeaf;
    cutValue = 0;
  }
  // End cut setup

  Double_t data     = 0.0;
  Int_t    n_data   = 0;
  TString  analysis = "integral_"+channel;

  for (int j = 0; j < numEntries; j++) 
  { // Loop over the input file's entries
    Tree->GetEntry(j);
    if ( cutValue || (CutBranch->GetValue(0) == 0 && CutLeaf->GetValue(0) == 0) ){ // cutValue = 1 iff overwrote cuts with 1, branch==leaf if overwritecuts==1, branch and leaf defined by default cuts or with added on top of non-deviceerrorcode leaf
      data+=Leaf->GetValue(0);
      n_data+=1;
    }
  }
  if (aggregatorStatus){
    writeFile_h(analysis,data,runNumber,minirunNumber,splitNumber,nRuns);
  }
  if (alarmStatus){
    // Then the alarm handler wants to receive the output in stdout
    Printf("%s=%f",(const char*)analysis,data);
  }
}

/*
void writeMean_leafHist_h(TString mode = "default", TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);

  // Make an instance of the relevant data source 
  TLeaf   *Leaf   = getLeaf_h(tree,branch,leaf,runNumber,minirunNumber,splitNumber,nRuns);
  if (!Leaf){
    return 0;
  }
  TString leafName = branch+"."+(TString)Leaf->GetName();
  TBranch *Branch = Leaf->GetBranch();
  TTree   *Tree   = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  TString channel = tree + "_" + branch + "_" + leaf;
  Double_t data_mean = 0.0;
  Double_t data_rms = 0.0;
  TString  mean = "mean_"+channel;
  TString  rms = "rms_"+channel;

  gROOT->SetBatch(kTRUE);
  Printf("Leaf name: %s",(const char*)leafName);
  Tree->Draw(Form("%s>>h1",(const char*)leafName));
  TH1 *h1 = (TH1*)gDirectory->Get("h1");
  
  if (mode == "default"){
	  data_mean = h1->GetMean();
	  data_rms = h1->GetRMS();
  }
  else if (mode == "clean" || mode == "manual"){
    TH1 *h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
    TString h2_name = h2->GetName();
    Tree->Draw(Form("%s>>%s",(const char*)leafName,(const char*)h2_name)); // Manual
	  data_mean = h2->GetMean();
	  data_rms = h2->GetRMS();
  }
  else if (mode == "auto" || mode == "loop"){
    TH1 *h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
	  data_mean = h2->GetMean();
	  data_rms = h2->GetRMS();
  }

  Printf("Run %d mean %s: %f",runNumber,(const char*)mean,data_mean);
  Printf("Run %d rms %s: %f",runNumber,(const char*)rms,data_rms);
  if (aggregatorStatus){
    writeFile_h(mean,data_mean,runNumber,minirunNumber,splitNumber,nRuns);
    writeFile_h(rms,data_rms,runNumber,minirunNumber,splitNumber,nRuns);
  //writeFile_h("test",1.0,runNumber,minirunNumber,splitNumber,nRuns);
  }
}*/
#endif // __CAMANA__

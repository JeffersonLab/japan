using namespace std;
void writeNEvents_Loop_h(TString tree = "mul", TString branch = "ErrorFlag", Int_t runNumber = 0, Int_t nRuns = -1){
  // Any branch will do, we are just counting the number of events that pass the global EventCuts, not device error codes too
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  // Make an instance of the relevant data source 
  TChain  * Chain   = getTree_h(tree, runNumber, nRuns);
  TLeaf *ErrorFlag = getBranchLeaf_h(tree,branch,runNumber,nRuns);
  if (!ErrorFlag){
    return 0;
  }
  TTree   *Tree   = ErrorFlag->GetBranch()->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  Int_t    n_data   = 0;
  TString  number_total_events = "number_total_events";
  TString  number_good_events = "number_good_events";

  for (int j = 0; j < numEntries; j++) 
  { // Loop over the input file's entries
    Tree->GetEntry(j);
    if ( ErrorFlag->GetValue(0) == 0 ){
      n_data+=1;
    }
  }
  writeFile_h(number_total_events,numEntries,runNumber,nRuns);
  writeFile_h(number_good_events,n_data,runNumber,nRuns);
}
void writeMean_Loop_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", Int_t runNumber = 0, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  TString channel = tree + "_" + branch + "_" + leaf;
  // Make an instance of the relevant data source 
  TLeaf   *Leaf   = getLeaf_h(tree,branch,leaf,runNumber,nRuns);
  if (!Leaf){
    return 0;
  }
  TBranch *Branch = Leaf->GetBranch();
  TTree   *Tree   = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  Double_t data     = 0.0;
  Int_t    n_data   = 0;
  Double_t data_mean = 0.0;
  TString  analysis = "mean_"+channel;

  for (int j = 0; j < numEntries; j++) 
  { // Loop over the input file's entries
    Tree->GetEntry(j);
    data+=Leaf->GetValue(0);
    n_data+=1;
  }
  data_mean = data/(1.0*n_data);
  writeFile_h(analysis,data_mean,runNumber,nRuns);
}

void writeInt_loop_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t runNumber = 0, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  TString channel = tree + "_" + branch + "_" + leaf;
  // Make an instance of the relevant data source 
  TLeaf   *Leaf   = getLeaf_h(tree,branch,leaf);
  TBranch *Branch = Leaf->GetBranch();
  TTree   *Tree   = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  TLeaf   *CutLeaf;
  TLeaf   *CutBranch;
  if (cut == "defaultCut" || cut == "default" || cut == "defaultCuts" || cut == "prex" || cut == "PREX"){
    CutBranch = Tree->GetLeaf("ErrorFlag");
    CutLeaf   = Branch->GetLeaf("Device_Error_Code");
  }
  else {
    CutBranch = Tree->GetLeaf(tree,cut); // Assume the user passed a single branch (non-device) that they want to compare to == 0 for "good" cuts
  }

  Double_t data     = 0.0;
  Int_t    n_data   = 0;
  TString  analysis = "integral_"+channel;

  for (int j = 0; j < numEntries; j++) 
  { // Loop over the input file's entries
    Tree->GetEntry(j);
    if ( (cut == "noCut") || (cut != "noCut" && CutBranch->GetValue(0) == 0) ){
      data+=Leaf->GetValue(0);
      n_data+=1;
    }
  }
  writeFile_h(analysis,data,runNumber,nRuns);
}
/*
void writeMean_leafHist_h(TString mode = "default", TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", Int_t runNumber = 0, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);

  // Make an instance of the relevant data source 
  TLeaf   *Leaf   = getLeaf_h(tree,branch,leaf,runNumber,nRuns);
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
  writeFile_h(mean,data_mean,runNumber,nRuns);
  writeFile_h(rms,data_rms,runNumber,nRuns);
  //writeFile_h("test",1.0,runNumber,nRuns);
}*/

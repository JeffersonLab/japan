#ifndef __CAMIO__
#define __CAMIO__
#include "camguin.hh"
#include <utility>
#include <vector>
#include <TString.h>
#include "TCut.h"
#include <list>
#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <TSystem.h>
#include <TChain.h>
#include <TFile.h>
using namespace std;
Int_t getAggregatorStatus_h(){
// Get environment variable agg status
  if (debug>0) Printf("Aggregator Status: %d",aggregatorStatus);
  if ( aggregatorStatus == -1 ) 
  { 
    TString aggStatusStr = gSystem->Getenv("CAM_AGGREGATE");
    aggregatorStatus = aggStatusStr.Atoi();
  }
  if (aggregatorStatus<0){
    Printf("Error: Aggregator Status given (%d) invalid, must be an integer >= 0",aggregatorStatus);
    return 0;
  }
  if (debug>0) Printf("Aggregator Status: %d",aggregatorStatus);
  return aggregatorStatus;
}

Int_t getAlarmStatus_h(){
// Get environment variable alarm status (if 1 then print to stdout)
  if (debug>0) Printf("Alarm Status: %d",alarmStatus);
  if ( alarmStatus == -1 ) 
  { 
    TString alarmStr = gSystem->Getenv("CAM_ALARM");
    alarmStatus = alarmStr.Atoi();
  }
  if (alarmStatus<0){
    Printf("Error: Alarm Status given (%d) invalid, must be an integer >= 0",alarmStatus);
    return 0;
  }
  if (debug>0) Printf("Alarm Status: %d",alarmStatus);
  return alarmStatus;
}

Int_t getDebug_h(){
// Get environment variable debug level
  if (debug>0) Printf("Debug Level: %d",debug);
  if ( debug == -1 ) 
  { 
    TString debugStr = gSystem->Getenv("CAM_DEBUG");
    debug = debugStr.Atoi();
  }
  if (debug<0){
    Printf("Error: Debug Level given (%d) invalid, must be an integer >= 0",debug);
    return 0;
  }
  if (debug>0) Printf("Debug Level: %d",debug);
  return debug;
}

Int_t getRunNumber_h(Int_t runNumber = 0){
// Get environment variable run number
  if (debug>0) Printf("Run number: %d",runNumber);
  if ( runNumber == 0 ) 
  { 
    TString run = gSystem->Getenv("RUNNUM");
    runNumber = run.Atoi();
  }
  if (runNumber<=0){
    Printf("Error: Run Number given (%d) invalid, must be an integer > 0",runNumber);
    return 0;
  }
  if (debug>0) Printf("Run number: %d",runNumber);
  return runNumber;
}

Int_t getSplitNumber_h(Int_t splitNumber = -1){
// Get environment variable split number
  if ( splitNumber == -1 ) 
  { 
    TString split = gSystem->Getenv("SPLITNUM");
    splitNumber = split.Atoi();
  }
  if (splitNumber<0){
    Printf("Error: Split Number given (%d) invalid, must be an integer >= 0",splitNumber);
    return 0;
  }
  if (debug>0) Printf("Split number: %d",splitNumber);
  return splitNumber;
}

Int_t getNruns_h(Int_t n_runs = -1){
// Get environment variable number of runs to chain
  if ( n_runs == -1 ) 
  { 
    TString nRuns = gSystem->Getenv("NRUNS");
    n_runs = nRuns.Atoi();
  }
  if (n_runs<0){
    Printf("Error: Number of Runs given (%d) invalid, must be an integer > 0 \n Tip: n_runs = 1 means you will only use 1 run, = 2 will TChain a second one on)",n_runs);
    return 0;
  }
  if (debug>0) Printf("Number of Runs: %d",n_runs);
  return n_runs;
}

void getAggregateVars_h(TTree * rootTree, std::vector<TString>* aggVars, std::vector<Double_t>* oldValues, std::vector<Double_t>* newValues) 
{

  // Utility to find all of the variables (leaf's/branches) within a
  // Specified TTree and put them within the aggVars vector.
  // It is possible to generalize this up a level to loop over 
  // a vector full of TTrees, but for now lets assume just one tree
  //std::vector<TString> aggVars;
  //aggVars->clear();

  if (debug>2) Printf("Reading tree %s",(const char*)rootTree->GetName());
  TObjArray *branchList = rootTree->GetListOfBranches();
  TIter next(branchList);
  TBranch *brc;

  while((brc=(TBranch*)next())!=0) {
    TString found = brc->GetName();
    // Not sure if the line below is so smart...
    aggVars->push_back(found);
    if (debug>1) Printf("In branch %s",(const char*)found);
    oldValues->push_back(-1.0e6); // Add the other vectors simulatneously to avoid mis-mapping
    newValues->push_back(-1.0e6);
  }

  for(auto iBranch = aggVars->begin(); iBranch != aggVars->end(); iBranch++) {
    //if (debug>1) Printf("In branch %d : %s",iBranch,(const char*)&aggVars[iBranch]);
  }
}
void addAggregateVars_h(TString varName, std::vector<TString>* aggVars, std::vector<Double_t>* oldValues, std::vector<Double_t>* newValues){

  if (debug>1) Printf("Push back %s",(const char*)varName);
  aggVars->push_back(varName);
  oldValues->push_back(-1.0e6); // Add the other vectors simulatneously to avoid mis-mapping
  newValues->push_back(-1.0e6);
}

vector<vector<string>> textFileParse_h(TString fileName, char delim = ',')
{
  vector<vector<string> > filearray;   // the 2D array
  string line;                     // the contents
  string word;                     // one word at at time
  vector<string> words;            // array of values for one line only

  if ( !gSystem->AccessPathName(fileName.Data()) ) {
    if (debug>0) Printf("Found file name: %s",(const char*)fileName);
    ifstream in(fileName.Data());

    filearray.clear();
    //getline(in,line); // Uncomment to skip header line

    while ( getline(in,line) )    // get next line in file
    {
      words.clear();
      stringstream ss(line);

      while ( getline(ss,word,delim) ){
        // break line into comma delimitted fields
        words.push_back(word); 
      }
      filearray.push_back(words);  // add the 1D array to the 2D array
    }
    in.close();
    for (Int_t i = 0 ; i < words.size() ; i++){
      words[i].clear();
    }
    words.clear();
    return filearray;
  }
  else{
    Printf("File not found: %s",(const char*)fileName);
    return filearray;
  }
}

TChain * getTree_h(TString tree = "mul", Int_t runNumber = 0, Int_t splitNumber = -1, Int_t n_runs = -1, TString filenamebase = "NULL"){

  TString filename = "NULL";
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  n_runs    = getNruns_h(n_runs);
  if (filenamebase == "NULL"){
    filenamebase = gSystem->Getenv("QW_ROOTFILES");
  }
  TString fileNameBase  = filenamebase; // placeholder string
  if (debug>4) Printf("Tree to add to chain = %s",(const char*)tree);
  TChain * newTChain = new TChain(tree);
  TChain *friendTChain = new TChain("mul");

  Bool_t foundFile = false;

  for(Int_t i = 0; i < (n_runs); i++){

    TString daqConfigs[4] = {"prexPrompt_pass2","prexCH","prexINJ","prex_tedf"}; // Potentially replace this with a config file read in array or map;
    TString analyses[3] = {".","_regress_prFIXME.","_regress_mul."};
    TString suffix[2] = {"root",Form("%03d.root",splitNumber)};
    // FIXME remove this "FIXME" once there is a non-degeneracy in the tree names between the regress_pr and _mul root file's tree names
    if (debug>0) Printf("Looping over candidate rootfile prefixes and suffixes");
    for(Int_t ana=0;ana<3;ana++){
      for(Int_t j=0;j<4;j++){
        for(Int_t suf=0;suf<2;suf++){
          filenamebase = Form("%s/%s_%d%s%s",(const char *)fileNameBase,(const char *)daqConfigs[j],runNumber+i,(const char*)analyses[ana],(const char*) suffix[suf]);
          filename     = filenamebase;
          if (debug>1) Printf("Trying file name: %s",(const char*)filenamebase);
          if ( !gSystem->AccessPathName(filename.Data()) ) {
            if (debug>1) Printf("Found file name: %s",(const char*)filenamebase);
            foundFile = true;
            j=6; // Exit loop
            suf=3;
          }
        }
      }
      //filenamebase = getRootFile_h(runNumber+i);
      filename     = filenamebase;
      filenamebase.Remove(filenamebase.Last('.'),5);

      int split = 0;
      while ( !gSystem->AccessPathName(filename.Data()) ) {
        TFile * candidateFile = new TFile(filename.Data(),"READ");
        if (candidateFile->GetListOfKeys()->Contains(tree)){
          if (debug>0) Printf("File added to Chain: \"%s\"",(const char*)filename);
          newTChain->Add(filename);
          friendTChain->Add(filename);
          newTChain->AddFriend(friendTChain);
        }
        else {
          if (debug>1) Printf("File %s doesn't contain tree: \"%s\"",(const char*)filename,(const char*)tree);
        }
        split++;
        filename = filenamebase + "_" + Form("%i",split) + ".root";
        candidateFile->Close();
      }
    }
  }
  if (!foundFile){
    Printf("Rootfile not found in %s with runs from %d to %d, split %03d, check your config and rootfiles",(const char*)fileNameBase,runNumber,runNumber+n_runs-1, splitNumber);
    return 0;
  }
  if (debug>3) Printf("TChain total N Entries: %d",(int)newTChain->GetEntries());
  return newTChain;
}

TLeaf * getBranchLeaf_h(TString tree = "mul", TString branchleaf = "ErrorFlag", Int_t runNumber = 0, Int_t splitNumber = -1, Int_t nRuns = -1, TString filenamebase = "NULL"){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  nRuns     = getNruns_h(nRuns);
  TChain *Chain = getTree_h(tree, runNumber, splitNumber, nRuns, filenamebase);
  if (!Chain){
    return 0;
  }
  TLeaf * BranchLeaf = Chain->GetLeaf(branchleaf);
  return BranchLeaf;
}

TBranch * getBranch_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", Int_t runNumber = 0, Int_t splitNumber = -1, Int_t nRuns = -1, TString filenamebase = "NULL"){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  nRuns     = getNruns_h(nRuns);
  TChain * Chain = getTree_h(tree, runNumber, splitNumber, nRuns, filenamebase);
  if (!Chain){
    return 0;
  }
  TBranch * Branch = Chain->GetBranch(branch);
  return Branch;
}

TLeaf * getLeaf_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0",TString leaf = "hw_sum", Int_t runNumber = 0, Int_t splitNumber = -1, Int_t nRuns = -1, TString filenamebase = "NULL"){
  if (debug>2) Printf("Looking for leaf: \"%s\"",(const char*)(tree+"."+branch+"."+leaf));
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  nRuns     = getNruns_h(nRuns);
  TChain * Chain = getTree_h(tree, runNumber, splitNumber, nRuns, filenamebase);
  if (!Chain){
    Printf("Error, tree %s missing",(const char*)(tree));
    return 0;
  }
  TBranch * Branch = Chain->GetBranch(branch);
  if (!Branch){ // If the branch doesn't exist assume the user wants to get a leaf instead
    TLeaf * Leaf = Chain->GetLeaf(branch);
    //getBranchLeaf_h(tree,leaf,runNumber,splitNumber,nRuns,filenamebase);
    if (!Leaf){
      Leaf = Chain->GetLeaf(leaf); //Try again
      if (!Leaf){ 
        Printf("Error, branch %s missing",(const char*)(tree+"_"+branch));
        return 0;
      }
    }
    return Leaf;
  }
  TLeaf * Leaf = Branch->GetLeaf(leaf);
  if (!Leaf){
    Printf("Error, leaf %s missing",(const char*)(tree+"_"+branch+"_"+leaf));
    return 0;
  }
  return Leaf;
}

void writeAlarmFile_h(){
  // Store all trees
  TString alarmFileName = "~/bin/alarm.csv";
  TString pwd           = gSystem->Getenv("PWD");
  Bool_t newFile        = gSystem->AccessPathName(pwd+"/"+alarmFileName); // Opposite return convention
  std::ofstream file_in;
  std::ofstream file_out;
  if (!newFile) file_in.open(alarmFileName,std::ofstream::in);

  vector<string> placeholder; // placeholder
  vector<vector<string> > filearray;   // the 2D array
  vector<vector<string> > filearraycopy;   // the 2D array
  if (newFile) {
    if (debug>2) Printf("New alarm file started");
  }
  else{
    filearray = textFileParse_h(alarmFileName,fAlarmData.delim);
    if (debug>2) Printf("Old alarm file read");
    file_in.close();
  }

  // The file takes the form:
  // 0, type0_0, type1_0, type2_0, type3_0, type4_0
  // 1, type0_0, type1_0, type2_0, type3_1, type4_0
  // 2, type0_0, type1_0, type2_0, type3_2, type4_0
  // 3, type0_0, type1_0, type2_1, type3_0, type4_0
  // 4, type0_0, type1_0, type2_1, type3_1, type4_0
  // 5, type0_0, type1_0, type2_1, type3_2, type4_0
  // 6, type0_0, type1_1, type2_0, type3_0, type4_0
  // 7, type0_0, type1_1, type2_0, type3_1, type4_0
  // 8, type0_0, type1_1, type2_0, type3_2, type4_0
  // 9, type0_1, type1_0, type2_0, type3_0, type4_0
  // 10,type0_1, type1_0, type2_0, type3_1, type4_0
  //
  // At any given moment the user is assumed to know what the starting and ending indices are for their chose edit of the GUI
  // or for their chosen analysis

  if(fAlarmData.changeIndex==0){
    // Proceed in normal edit or add mode
    if(debug>3) Printf("Editing filearray");
    placeholder.push_back(fAlarmData.type[0]);
    placeholder.push_back(fAlarmData.type[1]);
    placeholder.push_back(fAlarmData.type[2]);
    placeholder.push_back(fAlarmData.name);
    placeholder.push_back(fAlarmData.value);
    if (newFile || fAlarmData.indexStart>filearray.size()-1){ // Then add
      if(debug>3) Printf("Editing filearray - adding new entry"); // FIXME add a new entry into the array (not at the end) somehow - do push_back() and then move()
      filearray.push_back(placeholder);
    }
    else{ // Then edit
      if(debug>3) Printf("Editing filearray - editing entry");
      filearray[fAlarmData.indexStart]=placeholder;
    }
    placeholder.clear();
  }
  else if(fAlarmData.changeIndex==999999){
    if(debug>3) Printf("Editing filearray - deleting entry");
    // Delete mode
    filearray.erase(filearray.begin()+fAlarmData.indexStart,filearray.begin()+fAlarmData.indexEnd+1); // Blast away all from start to end indices
  }
  else if(fAlarmData.changeIndex<0){ // Assume that the user has requested exactly the correct number of indices to move up or down
    if(debug>3) Printf("Editing filearray - swap up");
    // Swap locations mode
    Int_t sizeIndex = fAlarmData.indexEnd-fAlarmData.indexStart;
    if((fAlarmData.changeIndex+fAlarmData.indexStart)<0){
      file_out.close();
      return; // If the user sets it too far back continue
    }
    filearraycopy = filearray;
    for (Int_t i = 0 ; i > fAlarmData.changeIndex ; i--){    
      filearray[fAlarmData.indexEnd+i]=filearraycopy[fAlarmData.indexStart-1+i];
    }
    for (Int_t i = fAlarmData.indexStart ; i < fAlarmData.indexEnd+1 ; i++){
      filearray[fAlarmData.changeIndex+i]=filearraycopy[i]; // Swap remaining contents into gap
    }
  }
  else if(fAlarmData.changeIndex>0){ // Assume that the user has requested exactly the correct number of indices to move up or down
    if(debug>3) Printf("Editing filearray - swap down");
    // Swap locations mode
    Int_t sizeIndex = fAlarmData.indexEnd-fAlarmData.indexStart;
    if((fAlarmData.changeIndex+fAlarmData.indexEnd)>filearray.size()){
      file_out.close();
      return; // If the user sets it too far forward continue
    }
    filearraycopy = filearray;
    for (Int_t i = 0 ; i < fAlarmData.changeIndex ; i++){    
      filearray[fAlarmData.indexStart+i]=filearraycopy[fAlarmData.indexEnd+1+i];
    }
    for (Int_t i = fAlarmData.indexStart ; i < fAlarmData.indexEnd+1 ; i++){ // Swap remaining contents into gap
      filearray[fAlarmData.changeIndex+i]=filearraycopy[i];
    }
  }
  if(debug>3) Printf("Printing new version of alarm file");
  if(alarmStatus){
    file_out.open(alarmFileName,std::ofstream::trunc);
    for (size_t i = 0 ; i<filearray.size(); i++){
      file_out<<filearray[i][0];
      for (size_t j = 1 ; j<filearray[i].size(); j++){
        file_out<<fAlarmData.delim<<filearray[i][j];
      }
      file_out<<std::endl;
    }
    file_out.close();
  }
}

void writeFile_h(TString valueName = "value", Double_t new_value = 0.0, Int_t new_runNumber = 0, Int_t new_splitNumber = -1, Int_t new_nRuns = -1){
  // Store all trees
  TString aggregatorFileName = "run_aggregator.root";
  TString pwd                = gSystem->Getenv("PWD");
  Bool_t newFile             = gSystem->AccessPathName(pwd+"/"+aggregatorFileName); // Opposite return convention
  TFile *aggregatorFile      = new TFile(aggregatorFileName,"UPDATE");
  aggregatorFile->cd();
  TTree *oldTree;
  TTree *newTree;

  // If the user adds an entry or branch then behave differently
  Bool_t newBranch = false;

  // Get environment variables
  new_runNumber = getRunNumber_h(new_runNumber);
  new_splitNumber = getSplitNumber_h(new_splitNumber);
  new_nRuns     = getNruns_h(new_nRuns);

  // Placeholder variables for reading of root file
  // Store the existing data in here per event
  std::vector<Double_t> tempValues;
  // Variables to be assigned to branch addresses
  std::vector<Double_t> oldValues;
  std::vector<Double_t> newValues;
  // List of branch names, append with new user additions
  std::vector<TString> branchList;

  if (newFile) {
    // Write a new file
    oldTree = new TTree("agg","Aggregator Tree");
    if (debug>0) Printf("Making new aggregator tree");
    branchList.push_back("run_number");
    branchList.push_back("n_runs");
    branchList.push_back("split_n");
    newValues.push_back( -1.0e6); // Vectors have to be initialized, and I don't know how many entries will come, so go for all of them
    newValues.push_back( -1.0e6);
    newValues.push_back( -1.0e6);
    oldValues.push_back( -1.0e6); 
    oldValues.push_back( -1.0e6); 
    oldValues.push_back( -1.0e6); 
    tempValues.push_back(-1.0e6);
    tempValues.push_back(-1.0e6);
    tempValues.push_back(-1.0e6); 
  }
  else {
    // Open existing file 
    oldTree = (TTree*) aggregatorFile->Get("agg");
    //oldTree->SetName("old_agg");
    if (!oldTree) {
      Printf("ERROR, tree agg is dead");
    }
    if (debug>0) Printf("Updating tree %s",(const char*)oldTree->GetName());
    TObjArray *aggVars = oldTree->GetListOfBranches();
    if (debug>3) Printf("N entries = %d",aggVars->GetEntries());
    for ( Int_t b = 0; b<aggVars->GetEntries(); b++){
      TString found = (TString)(((TBranch*)(aggVars->At(b)))->GetName());
      if (debug>2) Printf("In branch %s",(const char*)found);
      branchList.push_back(found);
      newValues.push_back(-1.0e6);
      oldValues.push_back(-1.0e6);
      tempValues.push_back(-1.0e6);
    }
    for(Int_t iBranch = 0; iBranch < branchList.size(); iBranch++) {
      if (debug>4) Printf("In branch %d : %s",iBranch,(const char*)branchList[iBranch]);
    }
    if (debug>1) Printf("Got agg contents");
  }

  // Maybe do this here...
  //newTree = (TTree*) aggregatorFile->Get("agg");
  newTree = new TTree("agg","Aggregator Tree");

  // Check to see if the value passed by the user to store exists yet, if not add it
  if (!(std::find(branchList.begin(),branchList.end(),valueName)!=branchList.end())){
    if (debug>1) Printf("User adding new branch: %s",(const char*)valueName);
    //addAggregateVars_h(valueName,&branchList,&newValues,&oldValues);
    branchList.push_back(valueName);
    newValues.push_back(-1.0e6);
    oldValues.push_back(-1.0e6);
    tempValues.push_back(-1.0e6);
    newBranch = true;
  }
  // Loop over branches and assign their addresses to old and new tree
  for (size_t k = 0; k < branchList.size(); k++){
    if (debug>2) Printf("Assigning values to be saved, Iteration %zu, branch name: %s, manual blank initialization value: %f",k,(const char*) branchList[k],oldValues[k]);
    // If this is a new file then generate new branches for old and new
  	if (newFile || (newBranch && (branchList[k]==valueName))){
      oldTree->Branch(          branchList[k],&oldValues[k]); // Initialize a new branch, for placeholder purposes
  	}
  	else {
  	  oldTree->SetBranchAddress(branchList[k],&oldValues[k]); // Set old branch addresses
  	}
    newTree->Branch(            branchList[k],&newValues[k]); // Make new branches
  }
  newTree->SetBranchStatus("*",1);
  oldTree->SetBranchStatus("*",1);

  // Loop over all entries so far, find the one that I am adding if it is there, append otherwise, print all prior values in their respective places with appropriate placeholder values
  Int_t  numEntries    = oldTree->GetEntries();
  Int_t  entryN        = 0;     // Looping variable
  // List of conditions
  Bool_t userAddedNewEntry  = true; // Assume we are adding a new entry
  Bool_t writeEntry         = false;
  Bool_t editEntry          = false;
  Bool_t RunNCheck          = false;
  Bool_t NRunsCheck         = false;
  Bool_t SplitNumberCheck   = false;
  Bool_t userAddedNewBranch = newBranch;
  Bool_t loopEnd            = false;

  // 2) If userEdittedOldEntry then numEntries--
  //      How to tell: if branchName == user branch name
  //      Also means user didn't add a new entry
  // 1) If userAddedNewEntry then numEntries..
  //      either case write userPassedVariables to newValues[]
  //      for non-userPassedVariables pass oldValues[] to newValues[]
  // 3) If copyOldEntry then continue
  //      for all variables copy oldValues[] to newValues[]

  if (debug>3) Printf("Looking at %d entries",numEntries);
  while (entryN<=numEntries) {
    if (debug>3) Printf("Examining Entry Number %d",entryN);
	  oldTree->GetEntry(entryN);
	  //newTree->GetEntry(entryN);
    
	  // Set the "old" values to placeholder values
    for (size_t l = 0; l < branchList.size(); l++){
      if (debug>2) Printf("NOTE: Examining branch %s = %f (old value)",(const char*) branchList[l],oldValues[l]);
	    if (userAddedNewEntry && entryN==numEntries) {
	      // Case 1
	  	  // We are appending a new value to the end, or initializing an empty new root file
        if (debug>1) Printf("User adding new value to root file: branch %s, value (new = %f, old = %f) runnumber %d",(const char*)valueName,new_value,oldValues[l],new_runNumber);
  		  writeEntry = true;
  	  }
    }
    for (size_t l = 0; l < branchList.size(); l++){
	    // Check to see if we are on the requested new_runNumber, and if it is unique then behave differently
      if (branchList[l] == "run_number" && oldValues[l]==(Double_t)new_runNumber){
        if (debug>3) Printf("Looking at entry# %d, run_number %d",entryN,new_runNumber);
        RunNCheck=true;
      }
      if (branchList[l] == "n_runs" && oldValues[l]==(Double_t)new_nRuns){
        if (debug>3) Printf("Looking at entry# %d, n_runs %d",entryN,new_nRuns);
        NRunsCheck=true;
      }
      if (branchList[l] == "split_n" && oldValues[l]==(Double_t)new_splitNumber){
        if (debug>3) Printf("Looking at entry# %d, split_n %d",entryN,new_splitNumber);
        SplitNumberCheck=true;
      }
      if (SplitNumberCheck && NRunsCheck && RunNCheck){
        if (debug>3) Printf("Looking at entry# %d, editing it with updated values",entryN);
		    numEntries--;
		    userAddedNewEntry = false;
		    writeEntry        = true;
		    editEntry         = true;
        break;
      }
    }
    for (size_t l = 0; l < branchList.size(); l++){
      // If the user is currently writing an entry then assume all other values besides run_number and n_runs are not specified and leave them as oldValues initialization
  	  if (writeEntry){
  	    if ( branchList[l] == "run_number" ) { 
          if (debug > 3) Printf("NOTE: RunNumber %d getting written by user",new_runNumber);
  	      tempValues[l] = (Double_t)new_runNumber;
  	    }
        else if ( branchList[l] == "n_runs" ) {
          if (debug > 3) Printf("NOTE: new_nRuns %d getting written by user",new_nRuns);
          tempValues[l] = (Double_t)new_nRuns;
        }
  	    else if ( branchList[l] == "split_n" ) {
          if (debug > 3) Printf("NOTE: new_splitNumber %d getting written by user",new_splitNumber);
  	      tempValues[l] = (Double_t)new_splitNumber;
  	    }
  	    else if ( branchList[l] == valueName ) {
          if (debug > 3) Printf("NOTE: %s branch = %f getting written by user",(const char*) valueName,new_value);
          tempValues[l] = (Double_t)new_value;
	      }
	  	  else {
          if (debug > 3) Printf("NOTE: %s branch = %f getting written by user",(const char*) branchList[l],oldValues[l]);
          if (userAddedNewBranch && !editEntry){
            tempValues[l] = -1.0e6; //oldValues[l] has been replaced with the prior entry, and because this new branch has no value in the tree its just that prior value
          }
          else {
            tempValues[l] = oldValues[l];// has been replaced with the prior entry, and because this new branch has no value in the tree its just that prior value
          }
  		  }
        if (debug > 2) Printf("Saving new values, Branch name %s, value %f",(const char*)branchList[l],oldValues[l]);
      }
      else {
        if (debug > 2) Printf("Saving old values, Branch name %s, value %f",(const char*)branchList[l],oldValues[l]);
	      // Otherwise just save the oldValues
        tempValues[l] = oldValues[l];
	    }
	    newValues[l] = tempValues[l];
      if (debug > 1) Printf("Saving %s = %f, overwriting %f",(const char*)branchList[l],tempValues[l],oldValues[l]);
      oldValues[l] = -1.0e6;
	  }
    // Reset the triggers for writing
    writeEntry = false; 
    editEntry = false;
    NRunsCheck=false;
    RunNCheck=false;
    SplitNumberCheck = false;
 	  // And then be done writing the user passed input
    if (newFile || entryN<=numEntries){
	    newTree->Fill();  
    }
	  entryN++;
  }
  if (newFile) {
    newTree->Write("agg");
  }
  else {
    newTree->Write("agg",TObject::kOverwrite);
  }
  if (debug>0) newTree->Scan();
  aggregatorFile->Close();
}

string replaceStrChar(string str, const string& replace, char ch) {

  // set our locator equal to the first appearance of any character in replace
  size_t found = str.find_first_of(replace);

  while (found != string::npos) { // While our position in the sting is in range.
    str[found] = ch; // Change the character at position.
    found = str.find_first_of(replace, found+1); // Relocate again.
  }

  return str; // return our new string.
}

string stripStrChar(string str, const string& replace) {

  // set our locator equal to the first appearance of any character in replace
  size_t found = str.find_first_of(replace);

  if (debug > 3) Printf("Str: %s",str.c_str());
  while (found != string::npos) { // While our position in the sting is in range.
    for (size_t pos = 0; pos<str.size() - found; pos++){
      str[found+pos] = str[found+pos+1]; // Change the character at position.
    }
    //str[str.size()-1]='\0';
    str = str.substr(0, str.length()-1);
    found = str.find_first_of(replace, found); // Relocate again.
  }

  return str; // return our new string.
}


void writePostPanFile_h(Int_t runNumber = 1369, Int_t splitNumber = -1, TString filename = "run1369_summary.txt"){
  runNumber= getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  filename = Form("txt_out/run%d_regression_summary.txt",runNumber);
  char delim = '\t';
  vector<vector<string>> contents = textFileParse_h(filename,delim);
  int miniRun = 0;
  bool header = false;
  bool print  = false;
  double data = -1.0e6;
  string type = "type";
  string channel = "null";
  vector <string> manip;
  vector <double> numbers;

  for (size_t k = 0; k < contents.size(); k++){
    if (debug > 3) Printf("Contents pre trim: %s",contents[k][0].c_str());
    contents[k][0]=stripStrChar(contents[k][0]," ");
    if (debug > 3) Printf("Contents post trim: %s",contents[k][0].c_str());
    if (contents[k][0].substr(0,2)=="--"){
      //header line
      if (contents[k][0].substr(0,11)=="--Mini-run:"){
        string val = contents[k][0].substr(11,contents[k][0].size()-11);
        if (debug > 3) Printf("Mini-run: %s",val.c_str());
        miniRun=stof(val);
      }
      if (debug > 3) Printf("Type: %s",contents[k][0].substr(2,7-2).c_str());
      TString* testType = (TString*)contents[k][0].substr(2,7-2).c_str();;
      if (debug > 3) Printf("Compare: %s",(const char*)testType);
      if (!strcmp((const char*)testType,"Mini-") || !strcmp((const char*)testType,"Slope") || !strcmp((const char*)testType,"Noise") || !strcmp((const char*)testType,"IVRMS")){
        print  = false;
        header = true;
        manip.clear();
        numbers.clear();
        type = contents[k][0].substr(2,7-2);
        if (!strcmp((const char*)testType,"Mini-")){
          type = "Regressed";
        }
        if (debug > 3) Printf("Type %s",type.c_str());
      }
      continue;
    }
    if (header == true){
      // These are the column titles
      print  = true;
      header = false;
      for (size_t j = 0; j < contents[k].size(); j++){
        contents[k][j]=stripStrChar(contents[k][j]," ");
        TString* columnTitle = (TString*)contents[k][j].substr(0,contents[k][j].size()).c_str();

        if (strcmp((const char*)columnTitle,"")){ // if compare == true then return value is false and we skip onwards
          TString* columnTitle;
          TString* columnCompare = (TString*)contents[k][j].substr(contents[k][j].size()-5,5).c_str();
          if (debug > 3) Printf("ColumnCompare: %s",(const char*)columnCompare);
          if (!strcmp((const char*)columnCompare,"(ppm)")){
            columnTitle = (TString*)contents[k][j].substr(0,contents[k][j].size()-5).c_str();
            if (debug > 3) Printf("Saved Truncated Title: %s",(const char*)columnTitle);
          }
          else {
            columnTitle = (TString*)contents[k][j].substr(0,contents[k][j].size()).c_str();
            if (debug > 3) Printf("Title: %s",(const char*)columnTitle);
          }
          manip.push_back((const char*)columnTitle);
        }
      }
      continue;
    }
    else if (header==false) {
      // We are in the matrix of data
      // These are the row titles
      Int_t offset = 0;
      channel = contents[k][0];
      TString* testType2 = (TString*)type.c_str();
      //if (!strcmp((const char*)testType2,"IVRMS")){
      //  channel = "IVRMS";
      //  offset = -1;
      //}
      for (size_t j = 1+offset; j < contents[k].size(); j++){
        if (strcmp(contents[k][j].c_str()," ")){ // if compare == true then return value is false and we skip onwards
          // These are the matrix entries
          // contents[k][j]=stripStrChar(contents[k][j]," "); // atof strips front whitespace and ignores trailing whitespace
          string val = contents[k][j];
          if (debug > 3) Printf("Number %s",val.c_str());
          numbers.push_back(stof(val));
        }
      }
      // Add a new row to the data matrix
    }
    if (print){
      // Print one row at a time
      for (size_t b = 0; b < manip.size(); b++){
        if (aggregatorStatus){
          writeFile_h(channel+"_"+manip[b]+"_"+type,numbers[b],runNumber,miniRun);
        }
      }
      numbers.clear();
    }
  }

  for (Int_t i = 0 ; i < contents.size() ; i++){
    contents[i].clear();
  }
  contents.clear();
}
#endif // __CAMIO__

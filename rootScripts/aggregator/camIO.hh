#include <utility>
#include <vector>
#include <TString.h>
#include "TCut.h"
#include <list>
#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;
Int_t getRunNumber_h(Int_t runNumber = 0){
// Get environment variable run number
  if ( runNumber == 0 ) 
  { 
    TString run = gSystem->Getenv("RUNNUM");
    runNumber = run.Atoi();
  }
  if (runNumber<=0){
    Printf("Error: Run Number given (%d) invalid, must be an integer > 0",runNumber);
    return 0;
  }
  //Printf("Run number: %d\n",runNumber);
  return runNumber;
}

Int_t getNruns_h(Int_t n_runs = -1){
// Get environment variable run number
  if ( n_runs == -1 ) 
  { 
    TString nRuns = gSystem->Getenv("NRUNS");
    n_runs = nRuns.Atoi();
  }
  if (n_runs<=0){
    Printf("Error: Number of Runs given (%d) invalid, must be an integer > 0 \n Tip: n_runs = 1 means you will only use 1 run, = 2 will TChain a second one on)",n_runs);
    return 0;
  }
  //Printf("Number of Runs: %d\n",n_runs);
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

  //Printf("Reading tree %s\n",(const char*)rootTree->GetName());
  TObjArray *branchList = rootTree->GetListOfBranches();
  TIter next(branchList);
  TBranch *brc;

  while((brc=(TBranch*)next())!=0) {
    TString found = brc->GetName();
    // Not sure if the line below is so smart...
    aggVars->push_back(found);
    //Printf("In branch %s\n",(const char*)found);
    oldValues->push_back(-999999.0); // Add the other vectors simulatneously to avoid mis-mapping
    newValues->push_back(-999999.0);
  }

  for(Int_t iBranch = 0; iBranch < aggVars->size(); iBranch++) {
    //Printf("In branch %d : %s\n",iBranch,(const char*)&aggVars[iBranch]);
  }
}
void addAggregateVars_h(TString varName, std::vector<TString>* aggVars, std::vector<Double_t>* oldValues, std::vector<Double_t>* newValues){

  //Printf("Push back %s",(const char*)varName);
  aggVars->push_back(varName);
  oldValues->push_back(-999999.0); // Add the other vectors simulatneously to avoid mis-mapping
  newValues->push_back(-999999.0);
}

      
TChain * getTree_h(TString tree = "mul", Int_t runNumber = 0, Int_t n_runs = -1, TString filenamebase = "Rootfiles/"){

  TString filename = "NULL";
  runNumber = getRunNumber_h(runNumber);
  n_runs    = getNruns_h(n_runs);
  filenamebase = gSystem->Getenv("QW_ROOTFILES");
  TString fileNameBase  = filenamebase; // placeholder string
  TChain *chain = new TChain(tree);
  Bool_t foundFile = false;

  for(Int_t i = 0; i < (n_runs); i++){

    TString daqConfigs[4] = {"CH","INJ","ALL","_tedf"};
    for(Int_t j=0;j<4;j++){
      filenamebase = Form("%s/prex%s_%d.root",(const char *)fileNameBase,(const char *)daqConfigs[j],runNumber+i);
      filename     = filenamebase;
      //Printf("Trying file name: %s\n",(const char*)filenamebase);
      if ( !gSystem->AccessPathName(filename.Data()) ) {
        //Printf("Found file name: %s\n",(const char*)filenamebase);
        foundFile = true;
        j=5; // Exit loop
      }
    }
    //filenamebase = getRootFile_h(runNumber+i);
    filename     = filenamebase;
    filenamebase.Remove(filenamebase.Last('.'),5);
    
    int split = 0;
    while ( !gSystem->AccessPathName(filename.Data()) ) {
      //Printf("File added to Chain: \"%s\"\n",(const char*)filename);
      chain->Add(filename);
      split++;
      filename = filenamebase + "_" + split + ".root";
    }
  }
  if (!foundFile){
    Printf("Rootfile not found in %s with runs from %d to %d, check your config and rootfiles",(const char*)fileNameBase,runNumber,runNumber+n_runs-1);
    return 0;
  }
  //Printf("N Entries: %d",(int)chain->GetEntries());
  return chain;
}
TBranch * getBranch_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", Int_t runNumber = 0, Int_t nRuns = -1, TString filenamebase = "Rootfiles/"){
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  TChain  * Chain   = getTree_h(tree, runNumber, nRuns, filenamebase);
  if (!Chain){
    return 0;
  }
  TBranch * Branch  = Chain->GetBranch(branch);
  return Branch;
}

TLeaf * getLeaf_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0",TString leaf = "hw_sum", Int_t runNumber = 0, Int_t nRuns = -1, TString filenamebase = "Rootfiles/"){
  //Printf("Found leaf: \"%s\"\n",(const char*)(tree+"."+branch+"."+leaf));
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  TChain  * Chain   = getTree_h(tree, runNumber, nRuns, filenamebase);
  if (!Chain){
    Printf("Error, tree %s missing",(const char*)(tree));
    return 0;
  }
  TBranch * Branch  = Chain->GetBranch(branch);
  if (!Branch){
    Printf("Error, branch %s missing",(const char*)(tree+"_"+branch));
    return 0;
  }
  TLeaf   * Leaf    = Branch->GetLeaf(leaf);
  if (!Leaf){
    Printf("Error, leaf %s missing",(const char*)(tree+"_"+branch+"_"+leaf));
    return 0;
  }
  return Leaf;
}

void writeFile_h(TString valueName = "value", Double_t new_value = 0.0, Int_t new_runNumber = 0, Int_t new_nRuns = -1){
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
  new_nRuns     = getNruns_h(new_nRuns);

  // Placeholder variables for reading of root file
  // Store the existing data in here per event
  std::vector<Double_t> tempValues;
  // Variables to be assigned to branch addresses
  std::vector<Double_t> oldValues;
  std::vector<Double_t> newValues;
  // List of branch names, append with new user additions
  std::vector<TString> branchList;
  std::vector<TBranch*>newBranches;

  if (newFile) {
    // Write a new file
    oldTree = new TTree("agg","Aggregator Tree");
    //Printf("Making new aggregator tree");
    branchList.push_back("run_number");
    branchList.push_back("n_runs");
    newValues.push_back( -999999.0); // Vectors have to be initialized, and I don't know how many entries will come, so go for all of them
    newValues.push_back( -999999.0);
    tempValues.push_back(-999999.0);
    oldValues.push_back( -999999.0); 
    oldValues.push_back( -999999.0); 
    tempValues.push_back(-999999.0); 
  }
  else {
    // Open existing file 
    oldTree = (TTree*) aggregatorFile->Get("agg");
	  if (!oldTree) {
      Printf("ERROR, tree agg is dead");
    }
    //Printf("Making new aggregator tree");
    //Printf("Reading tree %s",(const char*)oldTree->GetName());
    TObjArray *aggVars = oldTree->GetListOfBranches();
    //Printf("N entries = %d",aggVars->GetEntries());
    for ( Int_t b = 0; b<aggVars->GetEntries(); b++){
      TString found = (TString)(((TBranch*)(aggVars->At(b)))->GetName());
      //Printf("In branch %s",(const char*)found);
      branchList.push_back(found);
      newValues.push_back(-999999.0);
      oldValues.push_back(-999999.0);
      tempValues.push_back(-999999.0);
    }
    for(Int_t iBranch = 0; iBranch < branchList.size(); iBranch++) {
      //Printf("In branch %d : %s",iBranch,(const char*)branchList[iBranch]);
    }
    //Printf("Got agg contents");
  }

  // Maybe do this here...
  //newTree = (TTree*) aggregatorFile->Get("agg");
  newTree = new TTree("agg","Aggregator Tree");

  // Check to see if the value passed by the user to store exists yet, if not add it
  if (!(std::find(branchList.begin(),branchList.end(),valueName)!=branchList.end())){
    //Printf("User adding new branch: %s",(const char*)valueName);
    //addAggregateVars_h(valueName,&branchList,&newValues,&oldValues);
    branchList.push_back(valueName);
    newValues.push_back(-999999.0);
    oldValues.push_back(-999999.0);
    tempValues.push_back(-999999.0);
    newBranch = true;
  }
  // Loop over branches and assign their addresses to old and new tree
  for (Int_t k = 0; k < branchList.size(); k++){
    //Printf("Assigning values to be saved, Iteration %d, branch name: %s, manual blank initialization value: %f",k,(const char*) branchList[k],oldValues[k]);
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
  Bool_t nRunsCheck         = false;
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

  //Printf("Looking at %d entries",numEntries);
  while (entryN<=numEntries) {
    //Printf("Examining Entry Number %d",entryN);
	  oldTree->GetEntry(entryN);
	  //newTree->GetEntry(entryN);
    
	  // Loop over all branches (FIXME (A) for the "new" user added value maybe initialize it differently?)
	  // Set the "old" values to placeholder values
    for (Int_t l = 0; l < branchList.size(); l++){
      //Printf("NOTE: Examining branch %s = %f (old value)",(const char*) branchList[l],oldValues[l]);
	    if (userAddedNewEntry && entryN==numEntries) {
	      // Case 1
	  	  // We are appending a new value to the end, or initializing an empty new root file
        //Printf("User adding new value to root file: branch %s, value (new = %f, old = %f) runnumber %d",(const char*)valueName,new_value,oldValues[l],new_runNumber);
  		  writeEntry = true;
  	  }
	    // Check to see if we are on the requested new_runNumber, and if it is unique then behave differently
	    if ( (branchList[l] == "run_number") && (oldValues[l]==(Double_t)new_runNumber) ){
	    	// Case 2
        // We are replacing a prior entry
        // Keep track of it being editted since it could also be a new branch situation
        //Printf("User editting value in root file: branch %s, value (new = %f, old = %f) runnumber %d",(const char*)valueName,new_value,oldValues[l],new_runNumber);
        //
        nRunsCheck = true; // Loop through again and check for nRuns being duplicated too
	    }
      if ( nRunsCheck && (branchList[l] == "n_runs") && (oldValues[l]==(Double_t)new_nRuns) ){ // FIXME this relies on run_number being first in the list of things being checked in order for it to work, maybe set l=0 and add nRunsheck== false to run_number check?
		    userAddedNewEntry = false;
		    writeEntry        = true;
		    editEntry         = true;
        nRunsCheck        = false; // Reset so further runs can be analyzed too
		    numEntries--;
      }
    }
    for (Int_t l = 0; l < branchList.size(); l++){
      // If the user is currently writing an entry then assume all other values besides run_number and n_runs are not specified and leave them as oldValues initialization
  	  if (writeEntry){
  	    if ( branchList[l] == "run_number" ) { 
          //Printf("NOTE: RunNumber %d getting written by user",new_runNumber);
  	      tempValues[l] = (Double_t)new_runNumber;
  	    }
  	    else if ( branchList[l] == "n_runs" ) {
          //Printf("NOTE: new_nRuns %d getting written by user",new_nRuns);
  	      tempValues[l] = (Double_t)new_nRuns;
  	    }
  	    else if ( branchList[l] == valueName ) {
          //Printf("NOTE: %s branch = %f getting written by user",(const char*) valueName,new_value);
          tempValues[l] = (Double_t)new_value;
	      }
	  	  else {
          //Printf("NOTE: %s branch = %f getting written by user",(const char*) branchList[l],oldValues[l]);
          if (userAddedNewBranch && !editEntry){
            tempValues[l] = -999999.0; //oldValues[l] has been replaced with the prior entry, and because this new branch has no value in the tree its just that prior value
          }
          else {
            tempValues[l] = oldValues[l];// has been replaced with the prior entry, and because this new branch has no value in the tree its just that prior value
          }
  		  }
        //Printf("Saving new values, Branch name %s, value %f",(const char*)branchList[l],oldValues[l]);
      }
      else {
        //Printf("Saving old values, Branch name %s, value %f",(const char*)branchList[l],oldValues[l]);
	      // Otherwise just save the oldValues
        tempValues[l] = oldValues[l];
	    }
	    newValues[l] = tempValues[l];
      //Printf("Saving %s = %f, overwriting %f",(const char*)branchList[l],tempValues[l],oldValues[l]);
      oldValues[l] = -999999.0;
	  }
    // Reset the triggers for writing
    writeEntry = false; 
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
    newTree->Write("agg",TObject::kWriteDelete,0);
  }
  //newTree->Scan();
  aggregatorFile->Close();
}

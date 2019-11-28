class TreeGetter{
  public:
    Int_t runNumber = 0;
    Int_t miniRunNumber = -1;
    std::string infile = "NULL";
    std::string baseinfile = "NULL";
    std::string outfile = "NULL";
    TFile *fileInput;
    TFile *fileBaseInput;
    TFile *fileOutput;
    std::vector<TChain*> trees;
    std::vector<TChain*> baseTrees;
    std::map<std::string,std::vector<std::string>> parameterVectors;
    void parseTextFile(std::string);
    // Need to do this in a more intelligent way...
    void eraseOldDuplicate(TChain*, TString, TString, Double_t, Double_t);
    Int_t getData();
    //Int_t getBaseData(Int_t);
    Int_t getMergeData();
    Int_t getBaseData();
    Int_t smartAdd(std::string);
    void stripData();
    void mergeData();
};

void TreeGetter::parseTextFile(std::string fileName = "input.txt"){
  Printf("Reading input config file");
  std::string line;
  std::vector<std::string> tokens;
  std::string token1;
  std::string token2;
  std::vector<std::string> details;
  ifstream ifinfile(fileName);
  if (ifinfile.is_open()){
    while(getline(ifinfile,line)){
//      Printf("Reading line %s",line.c_str());
      std::string token;
      std::istringstream tokenStream(line);
      Int_t tokenizer = 0;
      while(getline(tokenStream,token,'=')){
//        Printf("Reading token %s",token.c_str());
        if (tokenizer == 0) {
//          Printf("Token 1 = %s",token.c_str());
          token1 = (std::string)token;
        }
        else {
//          Printf("Token 2 = %s",token.c_str());
          token2 = (std::string)token;
        }
        tokenizer++;
        tokens.push_back(token);
      }
      if (tokens.size() < 2){
        Printf("Error: invalid input file");
        continue;
      }
      if (tokens.size() == 2){
        std::string detail;
        std::istringstream detailsStream(tokens.at(1));
        while(getline(detailsStream,detail,',')){
//          Printf("Reading detail %s",detail.c_str());
          details.push_back(detail);
        }
        if (details.size() > 0) {
//          Printf("Adding details to parameter vector %s",tokens[0].c_str());
          parameterVectors.insert(std::pair<std::string,std::vector<std::string>>(tokens.at(0),details));
        }
      }
      else {
        Printf("Invalid entry in input file: %s",line.c_str());
      }
      tokens.clear();
      details.clear();
    }
    ifinfile.close();
  }
}

Int_t TreeGetter::getData() { 
  Printf("Getting input file for run %d, minirun %d",runNumber, miniRunNumber);
  if (parameterVectors.count("ROOT input path") != 0 ) {
    infile = parameterVectors["ROOT input path"].at(0);
    if (!((TString)parameterVectors["ROOT input path"].at(0)).Contains("#") || parameterVectors["ROOT input path"].at(0).size() == 0) {
      Printf("Error, please include a \"#\" symbol to be replaced by the run number in \"ROOT input path\" in %s",infile.c_str());
      return 1;
    }
    std::string infile1 = infile.substr(0,infile.find("#"));
    std::string infile2 = infile.substr(infile.find("#")+1,infile.size()-infile1.size()-1);
    if (miniRunNumber == -1) {
      infile = Form("%s%d%s",infile1.c_str(),runNumber,infile2.c_str());
      fileInput = TFile::Open(infile.c_str());
    }
    else {
      std::string infile3 = infile2.substr(0,infile2.find("#"));
      std::string infile4 = infile2.substr(infile2.find("#")+1,infile2.size()-infile3.size()-1);
      infile = Form("%s%d%s%d%s",infile1.c_str(),runNumber,infile3.c_str(),miniRunNumber,infile4.c_str());
      fileInput = TFile::Open(infile.c_str());
    }
    // Else get default QW_ROOTFILES data to try
    if(!fileInput){
      infile = Form("$QW_ROOTFILES/prexPrompt_pass1_%d.000.root",runNumber);
      fileInput = TFile::Open(infile.c_str());
      if(!fileInput){
        std::cout << infile << " doesn't exist!!! Trying quick prompt" << std::endl;
        infile = Form("$QW_ROOTFILES/quick_%d.000.root",runNumber);
        fileInput=TFile::Open(infile.c_str());
        if(!fileInput){
          std::cout << infile << " doesn't exist!!!" << std::endl;
          return 1;
        }
      }
    }
    Printf("Got datafile %s",infile.c_str());
  }
  else {
    Printf("ERROR: No \"ROOT input path\" parameter in %s",infile.c_str());
    return 1;
  }
  return 0;
}

void TreeGetter::stripData(){
  if (parameterVectors.count("Trees") == 0){
    Printf("No Trees names given, aborting");
    return;
  }
  trees.clear();
  for (int n = 0 ; n < parameterVectors["Trees"].size() ; n++) {
    Printf("Getting tree \"%s\"",parameterVectors["Trees"].at(n).c_str());
    trees.push_back((TChain*)fileInput->Get(parameterVectors["Trees"].at(n).c_str()));
  }
  if (parameterVectors.count("Rootfile Output Path") == 0) {
    Printf("ERROR: No \"Rootfile Output Path\" listed. Using \"./ instead");
    outfile = Form("./%s_pruning.root",infile.substr(infile.find_last_of("/")+1,infile.size()-5).c_str());
  }
  else {
    outfile = Form("%s%s_pruning.root",parameterVectors["Rootfile Output Path"].at(0).c_str(),infile.substr(infile.find_last_of("/")+1,infile.size()-5).c_str());
  }
  fileOutput = TFile::Open(outfile.c_str(),"recreate");
  fileOutput->cd();
  for (int n = 0 ; n < parameterVectors["Trees"].size() ; n++) {
    trees.at(n)->CloneTree()->Write();
  }
  Printf("Done getting trees\n");
}

Int_t TreeGetter::getBaseData() { 
  Printf("Getting base tree for run");
  if (parameterVectors.count("ROOT base input path") != 0 ) {
    baseinfile = parameterVectors["ROOT base input path"].at(0);
    //if (!((TString)parameterVectors["ROOT base input path"].at(0)).Contains("#") || parameterVectors["ROOT base input path"].at(0).size() == 0) {
    //  Printf("Error, please include a \"#\" symbol to be replaced by the run number in \"ROOT base input path\" in %s",baseinfile.c_str());
    //  return 1;
    //}
    //std::string infile1 = baseinfile.substr(0,baseinfile.find("#"));
    //std::string infile2 = baseinfile.substr(baseinfile.find("#")+1,baseinfile.size()-infile1.size()-1);
    //baseinfile = Form("%s%d%s",infile1.c_str(),runNumber,infile2.c_str());
    fileBaseInput = TFile::Open(baseinfile.c_str());
    if (fileBaseInput) {
      Printf("Got base datafile %s",baseinfile.c_str());
    }
    else {
      Printf("No file %s",baseinfile.c_str());
    }
  }
  else {
    Printf("ERROR: No \"ROOT base input path\" parameter");
    return 1;
  }
  return 0;
}

Int_t TreeGetter::getMergeData() {
  if (parameterVectors.count("Trees") == 0){
    Printf("No Trees names given, aborting");
    return 1;
  }
  trees.clear();
  for (int n = 0 ; n < parameterVectors["Trees"].size() ; n++) {
    Printf("Getting Input tree \"%s\"",parameterVectors["Trees"].at(n).c_str());
    trees.push_back((TChain*)fileInput->Get(parameterVectors["Trees"].at(n).c_str()));
  }
  Printf("Done getting trees\n");
  return 0;
}

void TreeGetter::eraseOldDuplicate(TChain* tree, TString indicator1 = "run_number", TString indicator2 = "minirun_n", Double_t indicatorNumber1 = -1, Double_t indicatorNumber2 = -1)
{
  // Get old file, old tree and set top branch address
  if(indicatorNumber1==-1){
    cout << "No indicator given in parameter input file!" << endl;
    return;
  }
  if (!fileBaseInput) {
    Printf("Base Input File doesn't exist yet");
    return;
  }
  TChain* baseTree = (TChain*)fileBaseInput->Get(tree->GetName());
  //TTree* baseTree;
  //fileBaseInput->ls();
  //fileBaseInput->GetObject(tree->GetName(), baseTree);
  if (!baseTree) { 
    Printf("No tree named %s in %s",tree->GetName(),baseinfile.c_str());
    return; 
  }
  const auto nentries = baseTree->GetEntries();
  if (nentries>0){
    // Then the base file to append into is NOT empty and we should remove prior instances of the entry we are here to update
    Printf("Checking for deleting prior instance of %s = %f to make room",indicator1.Data(),indicatorNumber1);
    auto newtree = baseTree->CloneTree(0);
    TLeaf* indicatorNumL1 = baseTree->GetLeaf(indicator1);
    TLeaf* indicatorNumL2 = baseTree->GetLeaf(indicator2);
    Double_t tmpIndicatorNum1 = -1.0;
    Double_t tmpIndicatorNum2 = -1.0;
    Bool_t editted = false;
    for (auto i : ROOT::TSeqI(nentries)) {
      //indicatorNumL1->GetBranch()->GetEntry(i);
      baseTree->GetEntry(i);
      //newtree->GetEntry(i);
      tmpIndicatorNum1 = indicatorNumL1->GetValue();
      tmpIndicatorNum2 = indicatorNumL2->GetValue();
      Printf("run %f, minirun %f",tmpIndicatorNum1,tmpIndicatorNum2);
      Printf("run %f, minirun %f",indicatorNumber1,indicatorNumber2);
      if (tmpIndicatorNum1==indicatorNumber1 && (tmpIndicatorNum2==indicatorNumber2 || indicatorNumber2==-1.0)) {
        editted = true;
        Printf("Deleted an entry");
        continue;
      }
      tree->GetEntry(i);
      newtree->Fill();
    }
    if (editted) {
      TFile* newfile = TFile::Open(Form("localTmp_%f_%f.root",indicatorNumber1,indicatorNumber2), "recreate");
      newfile->cd();
      //newtree->Write(treeName,TObject::kOverwrite);
      newtree->Write();
      newfile->Close();
      gSystem->Exec(Form("mv localTmp_%f_%f.root %s",indicatorNumber1,indicatorNumber2,baseinfile.c_str()));
      delete newfile;
    }
    //delete indicatorNumL1;
    //delete indicatorNumL2;
  }
  delete baseTree;
}

void TreeGetter::mergeData() {
  Int_t worked = 0;
  if (parameterVectors.count("Trees") == 0){
    Printf("No Trees names given, aborting");
    return;
  }
  getBaseData();
  getMergeData();
  Printf("Merging into %s",baseinfile.c_str());
  outfile = Form("%s_%d.root",baseinfile.substr(0,infile.size()-5).c_str(),runNumber);
  fileOutput = TFile::Open(outfile.c_str(),"recreate");
  //for (int n = 0 ; n < trees.size() ; n++) {
  for (int n = 0 ; n < parameterVectors["Trees"].size() ; n++) {
    //FIXME add new parameters to vector for branch naming
    //eraseOldDuplicate(trees.at(n), "run_number", "minirun_n", runNumber, -1);
    Printf("Smart adding tree \"%s\"",parameterVectors["Trees"].at(n).c_str());
    // FIXME new things here, also conditions on appending new entries
    // Loop over new input file's entries
    // If the new input file's entry to be editted exists already in the old input file 
    //    then replace the previously existing entry in the old input file one with the new one in the new input file
    // Else 
    //    just add a new entry at the end
    // In both cases 
    //    write the branches existing in the union of both inputs
    //    For each branch that didn't have an entry fill with -1e6
    worked = smartAdd(parameterVectors["Trees"].at(n));
    if (!worked) {
      Printf("ERROR: Missing tree");
      continue;
    }
  }
  if (worked) {
    gSystem->Exec(Form("mv %s %s",outfile.c_str(),baseinfile.c_str()));
    //Printf("Done merging trees\n");
  }
  if (fileBaseInput) fileBaseInput->Close();
  if (fileInput) fileInput->Close();
  if (fileOutput) fileOutput->Close();
  delete fileBaseInput;
  delete fileInput;
  delete fileOutput;
}

Int_t TreeGetter::smartAdd(std::string tree) {
  Double_t ind1, ind2;
  std::string index1 = "run_number";
  std::string index2 = "minirun_n";
  if (parameterVectors.count("Index 1") == 1) {
    index1 = parameterVectors["Index 1"].at(0);
  }
  if (parameterVectors.count("Index 2") == 1) {
    index2 = parameterVectors["Index 2"].at(0);
  }
  if ((!fileBaseInput || !fileBaseInput->GetListOfKeys()->Contains(tree.c_str())) && fileInput) {
    // Move the input data into the output file name (could do with mv instead...)
    Printf("Starting a fresh output file");
    if (!fileInput){
      Printf("ERROR: no fileInput");
      return 0;
    }
    if (!fileInput->Get(tree.c_str())){
      Printf("ERROR: no fileInput tree: %s",tree.c_str());
      return 0;
    }
    //TChain *inTree = (TChain*)((TChain*)fileInput->Get(tree.c_str()))->Clone(tree.c_str());
    TChain *inTree = (TChain*)fileInput->Get(tree.c_str());
    TChain *outTree = (TChain*)inTree->CopyTree("","",0,0);
    if (!outTree->GetListOfBranches()->FindObject(index1.c_str())) {
      ind1 = (Double_t)runNumber;
      outTree->Branch(index1.c_str(),&ind1);
    }
    if (!outTree->GetListOfBranches()->FindObject(index2.c_str())) {
      ind2 = (Double_t)miniRunNumber;
      outTree->Branch(index2.c_str(),&ind2);
    }
    for(int i=0; i< inTree->GetEntries(); i++){
      inTree->GetEntry(i);
      outTree->Fill();
    }
    //((TChain*)fileInput->Get(tree.c_str()))->Scan();
    fileOutput->cd();
    outTree->Write();
//    outTree->Scan();
    delete inTree;
    delete outTree;
    return 1;
  }
  else if (fileInput) {
    // Make a friended tree and piecewise copy its entries into the output tree/file
    //TChain *baseTree = (TChain*) fileBaseInput->Get(tree.c_str())->Clone("Base Tree");
    TChain *baseTree = (TChain*) fileBaseInput->Get(tree.c_str());
    // FIXME if there is a run_number and minirun_n branch then we are in an already made output file and can continue, otherwise we are in a raw japan tree and need to make our own output run_number branch -> FIXME
    //baseTree->SetBranchAddress(index1.c_str(),&ind1);
    //baseTree->SetBranchAddress(index2.c_str(),&ind2);
    //baseTree->BuildIndex(index1.c_str(),index2.c_str());
//    baseTree->Scan();
//    Printf("Scanned base tree");

    if (!fileInput){
      Printf("ERROR: no fileInput");
      return 0;
    }
    if (!fileInput->Get(tree.c_str())){
      Printf("ERROR: no fileInput tree: %s",tree.c_str());
      return 0;
    }
    TChain *inTree = (TChain*) fileInput->Get(tree.c_str());
    //TChain *inTree = (TChain*) fileInput->Get(tree.c_str())->Clone();
    //inTree->SetBranchAddress(index1.c_str(),&ind1);
    //inTree->SetBranchAddress(index2.c_str(),&ind2);
    // FIXME if the index1 and index2 variables don't exist then try to generate new ones for indexing purposes
    // FIXME if the index1 and index2's match across all trees then just addFriend them together into one mega-inTree, but if they don't then do the AddFile thing too (maybe avoid this...)
    inTree->BuildIndex(index1.c_str(),index2.c_str());
//    inTree->Scan();
//    Printf("Scanned input tree");

    // Works... non-ideal
    //baseTree->AddFriend(inTree,"b");
    //baseTree->AddFriend(inTree);

    TChain *tmpTree = new TChain(tree.c_str());// = (TChain*) baseTree->CopyTree("","",0,0);
    tmpTree->AddFile(baseinfile.c_str());
    tmpTree->AddFriend(inTree);
    //outTree->RemoveFriend(inTree);
    //outTree->AddFriend(inTree);
    // FIXME Try to figure out how to do a lateral tree merging thing here
    tmpTree->AddFile(infile.c_str());
    // FIXME IF no index1 branch exists then make my own and use runNumber as the indexer 
    // FIXME IF no index2 branch exists then make my own and use miniRunNumber as the indexer (defaults -1)
    //if (0 != tmpTree->SetBranchAddress(index1.c_str(),&ind1)) {
    if (!tmpTree->GetListOfBranches()->FindObject(index1.c_str())) {
      ind1 = (Double_t)runNumber;
      tmpTree->Branch(index1.c_str(),&ind1);
    }
    if (!tmpTree->GetListOfBranches()->FindObject(index2.c_str())) {
      ind2 = (Double_t)miniRunNumber;
      tmpTree->Branch(index2.c_str(),&ind2);
    }
    tmpTree->BuildIndex(index1.c_str(),index2.c_str());
    Printf("Number of entries = %lld",tmpTree->GetEntries());
//    tmpTree->Scan();
//    Printf("Scanned temporary copy of baseTree with friends");
    TChain *outTree = (TChain*) tmpTree->CopyTree("","",0,0);
    std::map<std::pair<Int_t,Int_t>,Bool_t> deleted;
    for(Int_t i=0; i<tmpTree->GetEntries(); i++){
      tmpTree->GetEntry(i);
      std::pair <Int_t,Int_t> pr = {(Int_t)ind1,(Int_t)ind2};
      deleted[pr] = false;
    }
    for(Int_t i=0; i<tmpTree->GetEntries(); i++){
     // deleted[i] = false;
      tmpTree->GetEntry(i);
      std::pair <Int_t,Int_t> pr = {(Int_t)ind1,(Int_t)ind2};
      //Printf("Checking entry, ind1 = %d, ind2 = %d, deleted status == %d",(Int_t)ind1, (Int_t)ind2, deleted[pr]);
      if (deleted[pr] == false && -1 != baseTree->GetEntryNumberWithIndex((Int_t)ind1,(Int_t)ind2) && -1 != inTree->GetEntryNumberWithIndex((Int_t)ind1,(Int_t)ind2)) { 
        deleted[pr] = true;
        Printf("Skipping entry, ind1 = %d, ind2 = %d",(Int_t)ind1, (Int_t)ind2);
        continue; 
      }
      tmpTree->GetEntry(i);
      outTree->Fill();
    }
    /*for(int i=0; i< inTree->GetEntries(); i++){
//baseTree->GetEntry(i);
      inTree->GetEntry(i);
      // FIXME maybe do entry replacement here with a cut on runNumber?
      //inTree->GetEntryWithIndex((Int_t)ind1,(Int_t)ind2);
      outTree->Fill();
    }*/
    outTree->BuildIndex(index1.c_str(),index2.c_str());
    outTree->SetName(tree.c_str());
    fileOutput->cd();
    outTree->Write();
//    outTree->Scan();
    // Don't delete these trees? The file closure deletes them for me? But the tree-Loop??
    delete baseTree;
    delete inTree;
    delete outTree;
    return 1;
  }
  else {
    Printf("ERROR: No input file");
    return 0;
  }
}

int treeGetter(Int_t runNo = 4199, Int_t miniRunNo = -1, std::string inputFile = "input.xt", TString sluglist = "NULL"){
  if (runNo < 1000 && sluglist != "NULL") { // Assume its a slug - this is for the stripper mostly
    std::string runnum;
    ifstream ifinfile(sluglist);
    if (ifinfile.is_open()){
      while(getline(ifinfile,runnum)){
        TreeGetter * getter = new TreeGetter();
        getter->parseTextFile(inputFile);
        getter->runNumber = runNo;
        getter->miniRunNumber = miniRunNo;
        getter->getData();
        if (getter->parameterVectors.count("Merge") != 0){
          getter->mergeData();
        }
        else if (getter->parameterVectors.count("Strip") != 0){
          getter->stripData();
        }
      }
    }
  }
  else {
    TreeGetter * getter = new TreeGetter();
    getter->parseTextFile(inputFile);
    getter->runNumber = runNo;
    getter->miniRunNumber = miniRunNo;
    getter->getData();
    if (getter->parameterVectors.count("Merge") != 0){
      getter->mergeData();
    }
    else if (getter->parameterVectors.count("Strip") != 0){
      // Exclusive if/else if
      getter->stripData();
    }
  }
  return 0;
}

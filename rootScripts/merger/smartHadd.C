class TreeGetter{
  public:
    std::string outputFileName = "NULL";
    std::vector<Int_t> listOfFileIndices;
    std::vector<std::string> infile;
    std::string outfile = "NULL";
    std::vector<TFile *> fileInput;
    TFile *fileOutput;
    std::map<std::string,std::vector<std::string>> parameterVectors;
    Int_t parseTextFile(std::string);
    // Need to do this in a more intelligent way...
    Int_t getData();
    Int_t getListOfFiles(std::string);
    Int_t getMergeData();
    Int_t smartAdd(std::string);
    Int_t expandData();
};

Int_t TreeGetter::getListOfFiles(std::string fileName = "slug_list.txt") {
  std::string line;
  ifstream ifinfile(fileName);
  if (ifinfile.is_open()){
    while(getline(ifinfile,line)){
      listOfFileIndices.push_back(atoi(line.c_str()));
    }
  }
  else {
    Printf("Error, no file index list %s",fileName.c_str());
    return 1;
  }
  return 0;
}

Int_t TreeGetter::parseTextFile(std::string fileName = "input.txt"){
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
        return 1;
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
        return 1;
      }
      tokens.clear();
      details.clear();
    }
    ifinfile.close();
  }
  return 0;
}

Int_t TreeGetter::getData() { 
  if (parameterVectors.count("ROOT input path") != 0 ) {
    for (Int_t i = 0; i < listOfFileIndices.size() ; i++ ){
      std::string tmpInfile = "";
      tmpInfile = parameterVectors["ROOT input path"].at(0);
      if (!((TString)parameterVectors["ROOT input path"].at(0)).Contains("#") || parameterVectors["ROOT input path"].at(0).size() == 0) {
        Printf("Error, please include a \"#\" symbol to be replaced by the run number in \"ROOT input path\" in %s",tmpInfile.c_str());
        return 1;
      }
      TFile* tmpFileInput;
      std::string infile1 = tmpInfile.substr(0,tmpInfile.find("#"));
      std::string infile2 = tmpInfile.substr(tmpInfile.find("#")+1,tmpInfile.size()-infile1.size()-1);
      tmpInfile = Form("%s%d%s",infile1.c_str(),listOfFileIndices.at(i),infile2.c_str());
      tmpFileInput = TFile::Open(tmpInfile.c_str());
      // Else get default QW_ROOTFILES data to try
      if(!tmpFileInput){
        std::cout << tmpInfile << " doesn't exist!!!" << std::endl;
        return 1;
      }
      Printf("Got datafile %s",tmpInfile.c_str());
      infile.push_back(tmpInfile);
      fileInput.push_back(tmpFileInput);
    }
  }
  else {
    Printf("ERROR: No \"ROOT input path\" parameter in input config file");
    return 1;
  }
  return 0;
}

Int_t TreeGetter::expandData(){
  if (parameterVectors.count("Trees") == 0){
    Printf("No Trees names given, aborting");
    return 1;
  }
  if (parameterVectors.count("Rootfile Output Path") == 0) {
    outfile = Form("%s",outputFileName.c_str());
  }
  else {
    outfile = Form("%s",parameterVectors["Rootfile Output Path"].at(0).c_str());
    //outfile = Form("%s%s",parameterVectors["Rootfile Output Path"].at(0).c_str(),infile.at(0).substr(infile.at(0).find_last_of("/")+1,infile.at(0).size()-5).c_str());
  }
  fileOutput = TFile::Open(outfile.c_str(),"recreate");
  fileOutput->cd();
  for (int n = 0; n < parameterVectors["Trees"].size() ; n++) {
    TList* treeList = new TList;
    // Start at the end of the list and go backwards. Sort below undoes this, but this ensures the newest branch list dominates MergeTrees.
    for (Int_t f = listOfFileIndices.size()-1 ; f >= 0; f--) {
      treeList->Add((TChain*)fileInput.at(f)->Get(parameterVectors["Trees"].at(n).c_str()));
    }
    TChain* tmpTree = (TChain*)TChain::MergeTrees(treeList);
    std::string index1 = "run_number";
    std::string index2 = "minirun_n";
    // FIXME Needs optional index sorting
    if (parameterVectors.count("Index 1") == 1) {
      index1 = parameterVectors["Index 1"].at(0);
    }
    if (parameterVectors.count("Index 2") == 1) {
      index2 = parameterVectors["Index 2"].at(0);
    }
    tmpTree->BuildIndex(index1.c_str(),index2.c_str());
    TTreeIndex *indexedTree = (TTreeIndex *) tmpTree->GetTreeIndex();
    TChain* outTree = (TChain*)tmpTree->CloneTree(0);
    Long64_t nentries = tmpTree->GetEntries();
    for (Long64_t l = 0; l < nentries ; l++) {
      tmpTree->GetEntry(indexedTree->GetIndex()[l]);
      outTree->Fill();
    }
    outTree->Write("", TObject::kOverwrite);
  }
  //if (fileInput) fileInput->Close();
  if (fileOutput) fileOutput->Close();
  //delete fileInput;
  delete fileOutput;
  return 0;
}

// The final entry in listOfFiles will be used to determine the device list/branch list
int smartHadd(std::string listOfFiles = "slug_list.txt", std::string inputFile = "/adaqfs/home/apar/PREX/japan/rootScripts/merger/input_smartHadd.txt", std::string outputFileName = "grand_aggregator.root") {
  Printf("This Smart hadd replacement will do a standard tree TChaining, on multiple trees within one ROOT file");
  Printf("\tArg 1: Default = \"./slug_list.txt\" - The input list of integers, representing the input file indices");
  Printf("\t       You should fill a file with a list of the indices, in preferred order, but index sorting is done later");
  Printf("\t       Note that the final index is the file which will be used to determine the final Tree's branch list.");
  Printf("\t       Any missing branches from initial indexed files will be filled with zeros.");
  Printf("\tArg 2: Default = \"/adaqfs/home/apar/PREX/japan/rootScripts/merger/input_smartHadd.txt\" - The config file path");
  Printf("\t       You should fill an input text file, the 3rd argument, with several optional arguments:");
  Printf("\t\t\"ROOT input path\":      The input path from which you would like to select data, with a # symbol in place of the integer identifier");
  Printf("\t\t\"Rootfile Output Path\": Can be used to override argument 3, the output path destination");
  Printf("\t\t\"Trees\":                The list of Tree Names you would like to hadd independently");
  Printf("\t\t\"Index 1\":              The major sorting Tree Index, default is \"run_number\" - must be an integer, or round to a unique indexable integer");
  Printf("\t\t\"Index 2\":              The minor sorting Tree Index, default is \"minirun_n\" - must be an integer, or round to a unique indexable integer");
  Printf("\tArg 3: Default = \"./grand_aggregator.root\" - The output file path\n");
  TreeGetter * getter = new TreeGetter();
  Int_t works = 0;
  works = getter->parseTextFile(inputFile);
  getter->outputFileName = outputFileName;
  works = getter->getListOfFiles(listOfFiles);
  works = getter->getData();
  works = getter->expandData();
  if (works == 1){
    Printf("Failed");
    return 1;
  }
  Printf("\nDone :)\n");
  return 0;
}

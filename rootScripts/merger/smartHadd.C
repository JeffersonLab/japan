class TreeGetter{
  public:
    std::string inputFolderName = "NULL";
    std::string index1name      = "NULL";
    std::string index2name      = "NULL";
    std::string treeName        = "NULL";
    std::string outputFileName  = "NULL";
    std::vector<Int_t> listOfFileIndices;
    std::vector<std::string> infile;
    std::string outfile = "NULL";
    std::vector<TFile *> fileInput;
    TFile *fileOutput;
    // Need to do this in a more intelligent way...
    Int_t getData();
    Int_t getListOfFiles(std::string);
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

Int_t TreeGetter::getData() { 
  for (Int_t i = 0; i < listOfFileIndices.size() ; i++ ){
    std::string tmpInfile = "";
    tmpInfile = inputFolderName;
    if (!((TString)inputFolderName).Contains("#") || inputFolderName.size() == 0) {
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
  return 0;
}

Int_t TreeGetter::expandData(){
  outfile = Form("%s",outputFileName.c_str());
  fileOutput = TFile::Open(outfile.c_str(),"recreate");
  fileOutput->cd();
  TList* treeList = new TList;
  // Start at the end of the list and go backwards. Sort below undoes this, but this ensures the newest branch list dominates MergeTrees.
  //for (Int_t f = listOfFileIndices.size()-1 ; f >= 0; f--) {
  //  treeList->Add((TChain*)fileInput.at(f)->Get(treeName.c_str()));
  //}
  treeList->Add((TChain*)fileInput.at(listOfFileIndices.size()-1)->Get(treeName.c_str()));
  for (Int_t f = 0 ; f < listOfFileIndices.size()-1; f++) {
    treeList->Add((TChain*)fileInput.at(f)->Get(treeName.c_str()));
  }
  TChain* tmpTree = (TChain*)TChain::MergeTrees(treeList);
  std::string index1 = index1name;
  std::string index2 = index2name;
  tmpTree->BuildIndex(index1.c_str(),index2.c_str());
  TTreeIndex *indexedTree = (TTreeIndex *) tmpTree->GetTreeIndex();
  TChain* outTree = (TChain*)tmpTree->CloneTree(0);
  Long64_t nentries = tmpTree->GetEntries();
  for (Long64_t l = 0; l < nentries ; l++) {
    tmpTree->GetEntry(indexedTree->GetIndex()[l]);
    outTree->Fill();
  }
  outTree->Write("", TObject::kOverwrite);
  //if (fileInput) fileInput->Close();
  if (fileOutput) fileOutput->Close();
  //delete fileInput;
  delete fileOutput;
  return 0;
}

// The final entry in listOfFiles will be used to determine the device list/branch list
int smartHadd(std::string listOfFiles = "slug_list.txt", std::string inputFolderName = "/chafs2/work1/apar/aggRootfiles/slugRootfiles/", std::string outputFileName = "grand_aggregator.root", std::string index1 = "n_runs", std::string index2 = "run_number", std::string tree = "agg") {
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
  getter->inputFolderName = inputFolderName;
  getter->treeName        = tree;
  getter->index1name      = index1;
  getter->index2name      = index2;
  getter->outputFileName  = outputFileName;
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

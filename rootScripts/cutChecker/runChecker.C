#include <iostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <TEntryList.h>

//using namespace std; 

class Run{
  public:
    Int_t runNumber;
    Int_t numEntries;
    std::vector<Double_t> cycleNumbers;
    std::vector<Double_t> metricsData;
    std::vector<Double_t> metricsDataErrors;
};

class Checker{
  public:
    Int_t runNumber = 0;
    std::string infile = "NULL";
    TFile *file1;
    TChain *tree;
    ofstream outfile0;
    std::vector<Run*> runs;
    std::map<std::string,std::vector<std::string>> parameterVectors;
    void parseTextFile(std::string);
    Int_t getData(Int_t);
    Int_t analyzeRun(Run*);
    Int_t DoRun(Int_t);
    void printRunData();
    ~Checker();
};

Checker::~Checker(){
  outfile0.close();
};

Int_t Checker::getData(Int_t runNo) { 
  Printf("Getting Checker Data for run %d",runNo);
  runNumber = runNo; 
  if (parameterVectors.count("ROOT input path") != 0 ) {
    infile = parameterVectors["ROOT input path"].at(0);
    if (!((TString)parameterVectors["ROOT input path"].at(0)).Contains("#") || parameterVectors["ROOT input path"].at(0).size() == 0) {
      Printf("Error, please include a \"#\" symbol to be replaced by the run number in \"ROOT input path\" in %s",infile.c_str());
      return 1;
    }

    //Printf("Size of input file name %s = %d",infile.c_str(),infile.size());
    std::string infile1 = infile.substr(0,infile.find("#"));
    //Printf("Size of input file name part 1 %s = %d",infile1.c_str(),infile1.size());
    std::string infile2 = infile.substr(infile.find("#")+1,infile.size()-infile1.size()-1);
    //Printf("Size of input file name part 2 %s = %d",infile2.c_str(),infile2.size());
    //infile = Form("$QW_ROOTFILES/prexPrompt_pass1_%d.000.root",runNumber);
    infile = Form("%s%d%s",infile1.c_str(),runNumber,infile2.c_str());
    file1 = TFile::Open(infile.c_str());
    if(!file1){
      infile = Form("$QW_ROOTFILES/prexPrompt_pass1_%d.000.root",runNumber);
      file1 = TFile::Open(infile.c_str());
      if(!file1){
        std::cout << infile << " doesn't exist!!! Trying quick prompt" << std::endl;
        infile = Form("$QW_ROOTFILES/quick_%d.000.root",runNumber);
        file1=TFile::Open(infile.c_str());
        if(!file1){
          std::cout << infile << " doesn't exist!!!" << std::endl;
          return 1;
        }
      }
    }
  }
  else {
    Printf("ERROR: No \"ROOT input path\" parameter in %s",infile.c_str());
    return 1;
  }
  return 0;
}

void Checker::parseTextFile(std::string fileName = "input.txt"){
  // Getting device list
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
      while(getline(tokenStream,token,'?')){
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
        //return 1; 
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

Int_t Checker::analyzeRun(Run * tmpRun){
  if (parameterVectors.count("Tree") == 0){
    Printf("Looking in \"mul\" tree for Checker data");
    std::vector<std::string> tmpStrVec;
    std::string tmpStr = "mul";
    tmpStrVec.push_back(tmpStr);
    parameterVectors["Tree"] = tmpStrVec;
  }
  if (parameterVectors.count("Draws") == 0 || parameterVectors.count("Cuts") == 0 || ( parameterVectors["Draws"].size() != parameterVectors["Cuts"].size())){
    Printf("Error, non-matching vector of Draws vs. Cuts");
    return 2;
  }
  //FIXME this assumes only one tree requested
  tree = (TChain*) file1->Get(parameterVectors["Tree"].at(0).c_str());
  Int_t nonzero = 0;
  Int_t nEntries = tree->GetEntries();
  tmpRun->numEntries = nEntries;
  if (nEntries == 0) {
    return 1;
  }
  for (Int_t i = 0 ; i<parameterVectors["Draws"].size(); i++) {
    Printf("Counting entries that pass cut %s",parameterVectors["Cuts"].at(i).c_str());
    nonzero = tree->Draw(Form("%s",parameterVectors["Draws"].at(i).c_str()),Form("%s",parameterVectors["Cuts"].at(i).c_str()),"goff");  //picks out unique cycle numbers
    tmpRun->metricsData.push_back(1.0*nonzero/nEntries);
    tmpRun->metricsDataErrors.push_back(sqrt(nonzero)/nEntries);
  }
  return 0;
}

Int_t Checker::DoRun(Int_t num) {
  Run * tmpRun = new Run();
  tmpRun->runNumber = num;
  Int_t ret = 0;
  ret = this->getData(num);
  if (ret != 0){
    return ret;
  }
  ret = this->analyzeRun(tmpRun);
  if (ret != 0){
    return ret;
  }
  (this->runs).push_back(tmpRun);
  return ret;
}

void Checker::printRunData() {
  double tab_width = 50;
  if (parameterVectors.count("Output path") == 0) {
    Printf("No output path given, using: ./Runs.csv");
    outfile0.open(Form("./Runs.csv"));
  }
  else {
    outfile0.open(Form("%s/Runs.csv",parameterVectors["Output path"].at(0).c_str()));
  }
  outfile0
    << setw(8) << setiosflags(ios::left) 
    << "Run Number,"
    << setw(8) << setiosflags(ios::left) 
    << "N Entries,"
    << setw(tab_width) << setiosflags(ios::left) 
    << "Draws,"
    << setw(tab_width) << setiosflags(ios::left) 
    << "Cuts,"
    << setw(14) << setiosflags(ios::left) 
    << "Ratio Passing Cut,"
    << setw(14) << setiosflags(ios::left) 
    << "Ratio Error" << std::endl;
  for (Int_t i = 0 ; i<runs.size() ; i++) {
    Printf("Printing data for run %d",runs.at(i)->runNumber);
    //for (Int_t draw = 0 ; draw<parameterVectors["Draws"].size(); draw++){
    //  Printf("Draw # %d = \"%s\"",draw,parameterVectors["Cuts"].at(draw).c_str());
    //}
    for (Int_t draw = 0 ; draw<parameterVectors["Draws"].size(); draw++){
      outfile0
        << setw(8) << setiosflags(ios::left) 
        << runs.at(i)->runNumber << ","
        << setw(8) << setiosflags(ios::left) 
        << runs.at(i)->numEntries << ","
        << setw(tab_width) << setiosflags(ios::left) 
        << parameterVectors["Draws"].at(draw) << ","
        << setw(tab_width) << setiosflags(ios::left) 
        << parameterVectors["Cuts"].at(draw) << ","
        << setw(14) << setiosflags(ios::left) 
        << runs.at(i)->metricsData.at(draw) << ","
        << setw(14) << setiosflags(ios::left) 
        << runs.at(i)->metricsDataErrors.at(draw) << std::endl;
    }
    outfile0 << std::endl;
  }
}

void runChecker(Int_t runNo = 4199, std::string inputFile = "input.txt", TString sluglist = "NULL"){
  Printf("Starting runCheckeralysis");
  Checker * runChecker = new Checker();
  runChecker->parseTextFile(inputFile);
  // List of runs
  if (runChecker->parameterVectors.count("Runs") != 0 && runChecker->parameterVectors.at("Runs").size() > 1) {
    Printf("Doing a list of runs");
    for (std::string &num:runChecker->parameterVectors["Runs"]){
      runChecker->DoRun(std::atoi(num.c_str()));
    }
  }
  // Just one run
  else if (runChecker->parameterVectors.count("Runs") != 0 && (runChecker->parameterVectors.at("Runs").at(0).find("-") == std::string::npos)) {
    Printf("Doing just one run");
    runChecker->DoRun(std::atoi(runChecker->parameterVectors.at("Runs").at(0).c_str()));
  }
  // '-' ranged list of runs
  else if (runChecker->parameterVectors.count("Runs") != 0) {
    Printf("Doing a run-range");
    Printf("Doing runs %d to %d",std::atoi(runChecker->parameterVectors.at("Runs").at(0).substr(0,runChecker->parameterVectors.at("Runs").at(0).find("-")).c_str()),std::atoi(runChecker->parameterVectors.at("Runs").at(0).substr(runChecker->parameterVectors.at("Runs").at(0).find("-")+1).c_str()));
    Int_t skipSize = 1;
    if (runChecker->parameterVectors.count("Run Skip Size") != 0 ) {
      skipSize = std::atoi(runChecker->parameterVectors["Run Skip Size"].at(0).c_str());
    }
    for (Int_t k = std::atoi(runChecker->parameterVectors.at("Runs").at(0).substr(0,runChecker->parameterVectors.at("Runs").at(0).find("-")).c_str()) ; k < std::atoi(runChecker->parameterVectors.at("Runs").at(0).substr(runChecker->parameterVectors.at("Runs").at(0).find("-")+1).c_str()) ; k=k+skipSize ) {
      runChecker->DoRun(k);
    }
  }
  runChecker->printRunData();
}

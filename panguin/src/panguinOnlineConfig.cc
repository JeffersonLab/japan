#include "panguinOnlineConfig.hh"
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <TMath.h>
#include <unistd.h>
#include <dirent.h>

using namespace std;

OnlineConfig::OnlineConfig() 
{
  // Constructor.  Without an argument, will use default "standard" config
  fMonitor = kFALSE;
  fVerbosity = 0;
  OnlineConfig("standard");
}

OnlineConfig::OnlineConfig(TString anatype): 
  confFileName(anatype),fVerbosity(0)
{
  // Constructor.  Takes the config anatype as the only argument.
  //  Loads up the configuration file, and stores it's contents for access.
  
  //confFileName += ".cfg";//Not sure what this would be needed DELETEME cg
  fMonitor = kFALSE;
  fFoundCfg = kFALSE;

  fConfFile = new ifstream(confFileName.Data());
  if ( ! (*fConfFile) ) {    cerr << "OnlineConfig() WARNING: config file " << confFileName.Data()
         << " does not exist" << endl;
    cerr << " Checking the " << guiDirectory << " directory" << endl;
    confFileName.Prepend(guiDirectory+"/");
    fConfFile = new ifstream(confFileName.Data());
    if ( ! (*fConfFile) ) {
      confFileName = guiDirectory+"/default.cfg";
      cout << "OnlineConfig()  Trying " << confFileName.Data() 
	   << " as default configuration." << endl
	   << " (May be ok.)" << endl;
      fConfFile = new ifstream(confFileName.Data());
      if ( ! (*fConfFile) ) {
	cerr << "OnlineConfig() WARNING: no file "
	     << confFileName.Data() <<endl;
	cerr << "You need a configuration to run.  Ask an expert."<<endl;
	fFoundCfg = kFALSE;
	//      return;
      } else {
	fFoundCfg = kTRUE;
      }
    } else {
      fFoundCfg = kTRUE;
    }
  } else {
    fFoundCfg = kTRUE;
  }

  if(fFoundCfg) {
    clog << "GUI Configuration loading from " 
	 << confFileName.Data() << endl;
  }

  ParseFile();

  fConfFile->close();
  delete fConfFile;

}

void OnlineConfig::ParseFile() 
{
  // Reads in the Config File, and makes the proper calls to put
  //  the information contained into memory.

  if(!fFoundCfg) {
    return;
  }

  TString comment = "#";
  vector<TString> strvect;
  TString sinput, sline;
  while (sline.ReadLine(*fConfFile)) {
    if(sline.Contains(comment)) continue;
    strvect = SplitString(sline," ");
    sConfFile.push_back(strvect);
  }

  if(fVerbosity>=1){
    cout << "OnlineConfig::ParseFile()\n";
    for(UInt_t ii=0; ii<sConfFile.size(); ii++) {
      cout << "Line " << ii << endl << "  ";
      for(UInt_t jj=0; jj<sConfFile[ii].size(); jj++) 
	cout << sConfFile[ii][jj] << " ";
      cout << endl;
    }
  }

  cout << "     " << sConfFile.size() << " lines read from " 
       << confFileName << endl;
  
}

Bool_t OnlineConfig::ParseConfig() 
{
  //  Goes through each line of the config [must have been ParseFile()'d]
  //   and interprets.

  if(!fFoundCfg) {
    return kFALSE;
  }

  UInt_t command_cnt=0;
  UInt_t j=0;
  // If statement for each high level command (cut, newpage, etc)
  for(UInt_t i=0;i<sConfFile.size();i++) {
    // "newpage" command
    if(sConfFile[i][0] == "newpage") {
      // sConfFile[i] is first of pair
      for(j=i+1;j<sConfFile.size();j++) {
	if(sConfFile[j][0] != "newpage") {
	  // Count how many commands within the page
	  command_cnt++;
	} else break;
      }
      pageInfo.push_back(make_pair(i,command_cnt));
      i += command_cnt;
      command_cnt=0;
    }
    if(sConfFile[i][0] == "watchfile") {
      fMonitor = kTRUE;
    }
    if(sConfFile[i][0] == "definecut") {
      if(sConfFile[i].size()>3) {
	cerr << "cut command has too many arguments" << endl;
	continue;
      }
      TCut tempCut(sConfFile[i][1],sConfFile[i][2]);
      //      cutList.push_back(make_pair(sConfFile[i][1],sConfFile[i][2]));
      cutList.push_back(tempCut);
    }
    if(sConfFile[i][0] == "rootfile") {
      if(sConfFile[i].size() != 2) {
	cerr << "WARNING: rootfile command does not have the "
	     << "correct number of arguments"
	     << endl;
	continue;
      }
      if(!rootfilename.IsNull()) {
	cerr << "WARNING: too many rootfile's defined. " 
	     << " Will only use the first one." 
	     << endl;
	continue;
      }
      rootfilename = sConfFile[i][1];
    }
    if(sConfFile[i][0] == "goldenrootfile") {
      if(sConfFile[i].size() != 2) {
	cerr << "WARNING: goldenfile command does not have the "
	     << "correct number of arguments"
	     << endl;
	continue;
      }
      if(!goldenrootfilename.IsNull()) {
	cerr << "WARNING: too many goldenrootfile's defined. " 
	     << " Will only use the first one." 
	     << endl;
	continue;
      }
      goldenrootfilename = sConfFile[i][1];
    }
    if(sConfFile[i][0] == "protorootfile") {
      if(sConfFile[i].size() != 2) {
	cerr << "WARNING: protorootfile command does not have the "
	     << "correct number of arguments"
	     << endl;
	continue;
      }
      if(!protorootfile.IsNull()) {
	cerr << "WARNING: too many protorootfile's defined. " 
	     << " Will only use the first one." 
	     << endl;
	continue;
      }
      protorootfile = sConfFile[i][1];
    }
    if(sConfFile[i][0] == "guicolor") {
      if(sConfFile[i].size() != 2) {
	cerr << "WARNING: guicolor command does not have the "
	     << "correct number of arguments (needs 1)"
	     << endl;
	continue;
      }
      if(!guicolor.IsNull()) {
	cerr << "WARNING: too many guicolor's defined. " 
	     << " Will only use the first one." 
	     << endl;
	continue;
      }
      guicolor = sConfFile[i][1];
    }
    if(sConfFile[i][0] == "plotsdir") {
      if(sConfFile[i].size() != 2) {
	cerr << "WARNING: plotsdir command does not have the "
	     << "correct number of arguments (needs 1)"
	     << endl;
	continue;
      }
      if(!plotsdir.IsNull()) {
	cerr << "WARNING: too many plotdir's defined. " 
	     << " Will only use the first one." 
	     << endl;
	continue;
      }
      plotsdir = sConfFile[i][1];
    }

  }

  if(fVerbosity>=3){
    cout << "OnlineConfig::ParseConfig()\n";
    for(UInt_t i=0; i<GetPageCount(); i++) {
      cout << "Page " << i << " (" << GetPageTitle(i) << ")"
	   << " will draw " << GetDrawCount(i) 
	   << " histograms." << endl;
    }
  }

  cout << "Number of pages defined = " << GetPageCount() << endl;
  cout << "Number of cuts defined = " << cutList.size() << endl;

  if (fMonitor) 
    cout << "Will periodically update plots" << endl;
  if(!goldenrootfilename.IsNull()) {
    cout << "Will compare chosen histrograms with the golden rootfile: " 
	 << endl 
	 << goldenrootfilename << endl;
  }

  return kTRUE;

}

TCut OnlineConfig::GetDefinedCut(TString ident) {
  // Returns the defined cut, according to the identifier

  for(UInt_t i=0; i<cutList.size(); i++) {
    if((TString)cutList[i].GetName() == ident.Data()) {
      TCut tempCut = cutList[i].GetTitle();
      return tempCut;
    }
  }
  return "";
}

vector <TString> OnlineConfig::GetCutIdent() {
  // Returns a vector of the cut identifiers, specified in config
  vector <TString> out;

  for(UInt_t i=0; i<cutList.size(); i++) {
    out.push_back(cutList[i].GetName());
  }

  return out;
}

Bool_t OnlineConfig::IsLogy(UInt_t page) {
  // Check if last word on line is "logy"


  UInt_t page_index = pageInfo[page].first;
  Int_t word_index = sConfFile[page_index].size()-1;
  if (word_index <= 0) return kFALSE;
  TString option = sConfFile[page_index][word_index];  
  if(option == "logy") {
    printf("\nFound a logy!!!\n\n");
    return kTRUE;
  }
  if(fVerbosity>=1){
    cout << "OnlineConfig::IsLogy()     " << option << " " << page_index << " " << word_index 
	 << " " << sConfFile[page_index].size() << endl;
    for (Int_t i= 0; i < sConfFile[page_index].size(); i++) {
      cout << sConfFile[page_index][i] << " ";
    }
  }

  return kFALSE;

}


pair <UInt_t, UInt_t> OnlineConfig::GetPageDim(UInt_t page) 
{
  // If defined in the config, will return those dimensions
  //  for the indicated page.  Otherwise, will return the
  //  calculated dimensions required to fit all histograms.

  pair <UInt_t, UInt_t> outDim;

  // This is the page index in sConfFile.
  UInt_t page_index = pageInfo[page].first;
  
  UInt_t size1 = 2;
  if (IsLogy(page)) size1 = 3;  // last word is "logy"
  
  // If the dimensions are defined, return them.
  if(sConfFile[page_index].size()>size1-1) {
    if(sConfFile[page_index].size() == size1) {
      outDim = make_pair(UInt_t(atoi(sConfFile[page_index][1])),
			 UInt_t(atoi(sConfFile[page_index][1])));
      return outDim;
    } else if (sConfFile[page_index].size() == size1+1) {
      outDim = make_pair(UInt_t(atoi(sConfFile[page_index][1])),
			 UInt_t(atoi(sConfFile[page_index][2])));
      return outDim;
    } else {
      cout << "Warning: newpage command has too many arguments. "
	   << "Will automatically determine dimensions of page."
	   << endl;
    }
  }
  
  // If not defined, return the "default."
  UInt_t draw_count = GetDrawCount(page);
  UInt_t dim = UInt_t(TMath::Nint(sqrt(double(draw_count+1))));
  outDim = make_pair(dim,dim);

  return outDim;
}

TString OnlineConfig::GetPageTitle(UInt_t page) 
{
  // Returns the title of the page.
  //  if it is not defined in the config, then return "Page #"

  TString title;

  UInt_t iter_command = pageInfo[page].first+1;

  for(UInt_t i=0; i<pageInfo[page].second; i++) { // go through each command
    if(sConfFile[iter_command+i][0] == "title") { 
      // Combine the strings, and return it
      for (UInt_t j=1; j<sConfFile[iter_command+i].size(); j++) {
	title += sConfFile[iter_command+i][j];
	title += " ";
      }
      title.Chop();
      return title;
    }
  }
  title = "Page "; title += page;
  return title;
}

vector <UInt_t> OnlineConfig::GetDrawIndex(UInt_t page) 
{
  // Returns an index of where to find the draw commands within a page
  //  within the sConfFile vector

  vector <UInt_t> index;
  UInt_t iter_command = pageInfo[page].first+1;

  for(UInt_t i=0; i<pageInfo[page].second; i++) {
    if(sConfFile[iter_command+i][0] != "title") {
      index.push_back(iter_command+i);
    }
  }

  return index;
}

UInt_t OnlineConfig::GetDrawCount(UInt_t page) 
{
  // Returns the number of histograms that have been request for this page
  UInt_t draw_count=0;

  for(UInt_t i=0; i<pageInfo[page].second; i++) {
    if(sConfFile[pageInfo[page].first+i+1][0] != "title") draw_count++;
  }

  return draw_count;

}

vector <TString> OnlineConfig::GetDrawCommand(UInt_t page, UInt_t nCommand)
{
  // Returns the vector of strings pertaining to a specific page, and 
  //   draw command from the config.
  // Return vector of TStrings:
  //  0: variable
  //  1: cut
  //  2: type
  //  3: title
  //  4: treename

  vector <TString> out_command(5);
  vector <UInt_t> command_vector = GetDrawIndex(page);
  UInt_t index = command_vector[nCommand];

  if(fVerbosity > 1){
    cout<<__PRETTY_FUNCTION__<<"\t"<<__LINE__<<endl;
    cout << "OnlineConfig::GetDrawCommand(" << page << "," 
	 << nCommand << ")" << endl;
  }

  for(UInt_t i=0; i<out_command.size(); i++) {
    out_command[i] = "";
  }

  // First line is the variable
  if(sConfFile[index].size()>=1) {
    out_command[0] = sConfFile[index][0];
  }

  if(sConfFile[index].size()>=2) {
    if((sConfFile[index][1] != "-type") &&
       (sConfFile[index][1] != "-title") &&
       (sConfFile[index][1] != "-tree"))
      out_command[1] = sConfFile[index][1];
  }

  // Now go through the rest of that line..
  for (UInt_t i=1; i<sConfFile[index].size(); i++) {
    if(sConfFile[index][i]=="-type") {
      out_command[2] = sConfFile[index][i+1];
      i = i+1;
    } else if(sConfFile[index][i]=="-title") {
      // Put the entire title, surrounded by quotes, as one TString
      TString title;
      UInt_t j=0;
      for(j=i+1; j<sConfFile[index].size(); j++) {
	TString word = sConfFile[index][j];
	if( (word.BeginsWith("\"")) && (word.EndsWith("\"")) ) {
	  title = word.ReplaceAll("\"","");
	  out_command[3] = title;
	  i = j;
	  break;
	} else if(word.BeginsWith("\"")) {
	  title = word.ReplaceAll("\"","");
	} else if(word.EndsWith("\"")) {
	  title += " " + word.ReplaceAll("\"","");
	  out_command[3] = title;
	  i = j;
	  break;
	} else {
	  title += " " + word;
	}
      }
    } else if(sConfFile[index][i]=="-tree") {
      out_command[4] = sConfFile[index][i+1];
      i = i+1;
    }
  }

  if(fVerbosity>=1){
    cout << sConfFile[index].size() << ": ";
    for(UInt_t i=0; i<sConfFile[index].size(); i++) {
      cout << sConfFile[index][i] << " ";
    }
    cout << endl;
    for(UInt_t i=0; i<out_command.size(); i++) {
      cout << i << ": " << out_command[i] << endl;
    }
  }

  return out_command;
}

vector <TString> OnlineConfig::SplitString(TString instring,TString delim) 
{
  // Utility to split up a string on the deliminator.
  //  returns a vector of strings.

  vector <TString> v;

  TString remainingString = instring;
  TString tempstring = instring;
  int i;

  while (remainingString.Index(delim) != -1) {
    i = remainingString.Index(delim);
    tempstring.Remove(i);
    v.push_back(tempstring);
    remainingString.Remove(0,i+1);
    while(remainingString.Index(delim) == 0) {
      remainingString.Remove(0,1);
    }
    tempstring = remainingString;
  }

  while(tempstring.EndsWith(delim)) {
    tempstring.Chop();
  }
     
  if(!tempstring.IsNull()) v.push_back(tempstring);

  return v;
}

void OnlineConfig::OverrideRootFile(UInt_t runnumber) 
{
  // Override the ROOT file defined in the cfg file If
  // protorootfile is used, construct filename using it, otherwise
  // uses a helper macro "GetRootFileName.C(UInt_t runnumber)
  cout<< "Root file defined before was: "<<rootfilename<<endl;
  if(!protorootfile.IsNull()) {
    char runnostr[10];
    sprintf(runnostr,"%04i",runnumber);
    protorootfile.ReplaceAll("XXXXX",runnostr);
    rootfilename = protorootfile;
  } else {
    string fnmRoot="/adaq1/work1/apar/japanOutput";
    if(getenv("QW_ROOTFILES"))
      fnmRoot = getenv("QW_ROOTFILES");
    else
      cout<<"QW_ROOTFILES env variable was not found going with default: "<< fnmRoot<<endl;

    cout << " Looking for file with runnumber "<<runnumber<<" in "<<fnmRoot<<endl;

    DIR *dirSearch;
    struct dirent *entSearch;
    const string daqConfigs[3] = {"CH","INJ","ALL"};
    int found=0;
    string partialname = "";
    if ((dirSearch = opendir (fnmRoot.c_str())) != NULL) {
      while ((entSearch = readdir (dirSearch)) != NULL) {
	for(int i=0;i<3;i++){
	  partialname = Form("prex%s_%d.root",daqConfigs[i].c_str(),runnumber);
	  if(fMonitor)
	    partialname = Form("prex%s_%d.adaq3",daqConfigs[i].c_str(),runnumber);

	  std::string fullname = entSearch->d_name;
	  if(fullname.find(partialname) != std::string::npos){
	    rootfilename = fnmRoot + "/" + fullname;
	    found++;
	  }
	}
	if(found) break;
      }
      closedir (dirSearch);
    }

    if(found)
      cout<<"\t found file "<< rootfilename<<endl;
    else{
      cout<<"double check your configurations and files. Quitting"<<endl;
      exit(1);
    }
  }
}


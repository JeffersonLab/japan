///  @file  QwHistogramHelper.cc
///  @brief Handler class for histogram/tree trim parameter files and
///         histogram and Tree branch creation
///
///  This class was originally written to be instantiated as a
///  global object, which all classes containing histograms would
///  use as an external object.
///  It could be repurposed to have a single instance per class,
///  but that would probably involve increasing the number of
///  histogram parameter files.

#include "QwHistogramHelper.h"

// Qweak headers
#include "QwLog.h"

///  Globally defined instance of the QwHistogramHelper class.
QwHistogramHelper gQwHists;


const Double_t QwHistogramHelper::fInvalidNumber  = -1.0e7;
const TString QwHistogramHelper::fInvalidName = "EmptyEmpty";

void QwHistogramHelper::DefineOptions(QwOptions &options)
{
  // Define the histogram and tree options
  options.AddOptions()
    ("enable-tree-trim", po::value<bool>()->default_bool_value(false),
     "enable trimmed trees");

  options.AddOptions()
    ("enable-histo-trim", po::value<bool>()->default_bool_value(false),
     "enable trimmed histograms");

  options.AddOptions()(
		       "tree-trim-file",  
		       po::value<string>()->default_value("tree_trim.in"), 
		       "trimmed tree file name"
		       );
  
  // What about QwTracking ? 
  // Monday, October 18 23:19:09 EDT 2010, jhlee
  options.AddOptions()(
		       "histo-trim-file", 
		       po::value<string>()->default_value("parity_hist.in"),
		       "trimmed histo file name"
		       );

}

void QwHistogramHelper::ProcessOptions(QwOptions &options)
{
  //enable the tree trim when  --enable-tree-trim in offline mode or --enable-mapfile for real time mode
  fTrimDisable =!( options.GetValue<bool>("enable-tree-trim") || options.GetValue<bool>("enable-mapfile"));
  fTrimHistoEnable = options.GetValue<bool>("enable-histo-trim") || options.GetValue<bool>("enable-mapfile");
  
  if (fTrimDisable)
    QwMessage <<"tree-trim is disabled"<<QwLog::endl;
  else
    QwMessage <<"tree-trim is enabled"<<QwLog::endl;

  if (fTrimHistoEnable)
    QwMessage <<"RT Mode: histo-trim is enabled "<<QwLog::endl;
  else
    QwMessage <<"histo-trim is disabled "<<QwLog::endl;

  // Process trim file options
  if (options.HasValue("tree-trim-file"))
    LoadTreeParamsFromFile(options.GetValue<string>("tree-trim-file"));
  if (options.HasValue("histo-trim-file"))
    LoadHistParamsFromFile(options.GetValue<string>("histo-trim-file"));
}


const QwHistogramHelper::HistParams QwHistogramHelper::GetHistParamsFromLine(QwParameterFile &mapstr)
{
  ///  Decodes the histogram parameters from the current line of
  ///  a QwParameter file.
  ///  If the line cannot be decoded, the name is returned as
  ///  fInvalidName.
  HistParams tmpstruct;
  tmpstruct.name_title = fInvalidName;

  std::string tmpname = mapstr.GetTypedNextToken<std::string>();
  std::string tmptype = mapstr.GetTypedNextToken<std::string>();
  if (tmptype != "TH1F" && tmptype != "TH2F"
   && tmptype != "TProfile" && tmptype != "TProfile2D"){
    QwError << "QwHistogramHelper::GetHistParamsFromFile:  Unrecognized histogram type: "
	      << tmptype << " (tmpname==" << tmpname <<")"<<  QwLog::endl;
  } else {
    tmpstruct.expression = tmpname;
    tmpstruct.name_title = tmpname;
    tmpstruct.type       = tmptype;
    tmpstruct.x_nbins    = mapstr.GetTypedNextToken<Int_t>();
    tmpstruct.x_min      = mapstr.GetTypedNextToken<Float_t>();
    tmpstruct.x_max      = mapstr.GetTypedNextToken<Float_t>();
    if (tmptype == "TH2F") {
      tmpstruct.y_nbins  = mapstr.GetTypedNextToken<Int_t>();
      tmpstruct.y_min    = mapstr.GetTypedNextToken<Float_t>();
      tmpstruct.y_max    = mapstr.GetTypedNextToken<Float_t>();
    } else if (tmptype == "TProfile") {
      tmpstruct.y_nbins  = 0;
      tmpstruct.y_min    = mapstr.GetTypedNextToken<Float_t>();
      tmpstruct.y_max    = mapstr.GetTypedNextToken<Float_t>();
    } else {
      tmpstruct.y_nbins  = 0;
      tmpstruct.y_min    = 0.0;
      tmpstruct.y_max    = 0.0;
    }
    tmpstruct.xtitle     = mapstr.GetTypedNextToken<TString>();
    tmpstruct.ytitle     = mapstr.GetTypedNextToken<TString>();

    std::string tmpmin = mapstr.GetTypedNextToken<std::string>();
    if (tmpmin.find_first_not_of("-+1234567890.eE") == std::string::npos) {
      //  tmpmin is a number
      tmpstruct.min = atof(tmpmin.c_str());
    } else {
      tmpstruct.min = fInvalidNumber;
    }
    std::string tmpmax = mapstr.GetTypedNextToken<std::string>();
    if (tmpmax.find_first_not_of("-+1234567890.eE") == std::string::npos) {
      //  tmpmax is a number
      tmpstruct.max = atof(tmpmax.c_str());
    } else {
      tmpstruct.max = fInvalidNumber;
    }
  }
  return tmpstruct;
}



void  QwHistogramHelper::LoadHistParamsFromFile(const std::string& filename)
{
  fInputFile = filename;

  fDEBUG = 0;
  //fDEBUG = 1;

  if (fDEBUG) std::cout<< "file name "<<fInputFile<<std::endl;
  //Important to empty the fHistParams to reload the real time histo difinition file
  if (fTrimHistoEnable)
    fHistParams.clear();
  
  // Open the file
  QwParameterFile mapstr(filename);
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment();    // Remove everything after a comment character.
    mapstr.TrimWhitespace(); // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty()) continue;

    HistParams tmpstruct = GetHistParamsFromLine(mapstr);
    if (tmpstruct.name_title != fInvalidName){
      fHistParams.push_back(tmpstruct);
      if (fDEBUG) {
        QwMessage << fHistParams.back() << QwLog::endl;
      }
    }
  }

  // Sort the histogram parameter definitions
  sort(fHistParams.begin(), fHistParams.end());
}


void  QwHistogramHelper::PrintHistParams() const 
{
  for (std::vector<HistParams>::const_iterator h = fHistParams.begin();
       h != fHistParams.end(); ++h) {
    QwMessage << *h << QwLog::endl;
  }
}


void  QwHistogramHelper::LoadTreeParamsFromFile(const std::string& filename)
{
  TString devicename;
  TString moduletype;
  TString subsystemname;
  QwParameterFile *section;
  QwParameterFile *module;
  std::vector<TString> TrimmedList;//stores the list of elements for each module
  std::vector<std::vector<TString> > ModulebyTrimmedList;//stores the list of elements for each module
  std::vector<TString> ModuleList;//stores the list of modules for each subsystem
  fDEBUG = 0;
  //fDEBUG = 1;  
  if (fTrimDisable)
    return;
  QwMessage << "Tree trim definition file for Offline Engine"<< QwLog::endl;
  QwParameterFile mapstr(filename.c_str());  //Open the file
 
  fTreeTrimFileLoaded=!mapstr.IsEOF();  
  fSubsystemList.clear();
  fModuleList.clear();
  fVQWKTrimmedList.clear();
  
  while ( (section=mapstr.ReadNextSection(subsystemname)) ){
    if (subsystemname=="DEVICELIST")//done with VQWK element trimming
      break;
    fSubsystemList.push_back(subsystemname);
    QwMessage <<"Subsystem found "<<subsystemname<<QwLog::endl;  

    ModuleList.clear();
    ModulebyTrimmedList.clear();
    while ( (module=section->ReadNextModule(moduletype)) ){
 
      ModuleList.push_back(moduletype);
      QwMessage <<"Module found "<<moduletype<<QwLog::endl;
      TrimmedList.clear();
      while (module->ReadNextLine()){
	module->TrimComment('#');   // Remove everything after a '#' character.
	module->TrimWhitespace();   // Get rid of leading and trailing spaces.
	if (module->LineIsEmpty())  continue;
	devicename=(module->GetLine()).c_str();
	TrimmedList.push_back(devicename);
	if (fDEBUG) {
	QwMessage <<"data element "<<devicename<<QwLog::endl;	
	}
      }
      ModulebyTrimmedList.push_back(TrimmedList);
	  

    }
    fModuleList.push_back(ModuleList);
    fVQWKTrimmedList.push_back(ModulebyTrimmedList);

    
  }
  //Start decoding the device list in the section [DEVICELIST]
  fTreeParams.clear();
  while (section->ReadNextLine()){
    section->TrimComment('#');   // Remove everything after a '#' character.
    section->TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (section->LineIsEmpty())  continue;
    devicename=(section->GetLine()).c_str();
    fTreeParams.push_back(std::pair<TString,TRegexp>(devicename,devicename));
    if (fDEBUG)
      QwMessage <<"device name "<<devicename<<QwLog::endl;	
    
  }

  //exit(1);

}


const QwHistogramHelper::HistParams QwHistogramHelper::GetHistParamsFromList(const TString& histname)
{
  HistParams tmpstruct, matchstruct;
  tmpstruct.name_title = fInvalidName;

  std::vector<int> matches;
  for (size_t i = 0; i < fHistParams.size(); i++) {
    if (DoesMatch(histname,fHistParams.at(i).expression)) {
      matchstruct = fHistParams.at(i);
      if (tmpstruct.name_title == fInvalidName) {
        tmpstruct = matchstruct;
        tmpstruct.name_title = histname;
        matches.push_back(i);
        break; // enabled (to get warnings for multiple definitions, disable)
      } else if (tmpstruct.nbins == matchstruct.nbins
              && tmpstruct.min   == matchstruct.min
              && tmpstruct.max   == matchstruct.max
              && tmpstruct.x_nbins == matchstruct.x_nbins
              && tmpstruct.x_min   == matchstruct.x_min
              && tmpstruct.x_max   == matchstruct.x_max
              && tmpstruct.y_nbins == matchstruct.y_nbins
              && tmpstruct.y_min   == matchstruct.y_min
              && tmpstruct.y_max   == matchstruct.y_max) {
        //matches.push_back(i); // disabled (to enable, also remove break above)
      }
    }
  }

  // Warn when multiple identical matches were found
  if (matches.size() > 1) {
    QwWarning << "Multiple identical matches for histogram " << histname << ":" << QwLog::endl;
    for (size_t i = 0; i < matches.size(); i++) {
      QwWarning << " " << fHistParams.at(matches.at(i)).name_title << QwLog::endl;
    }
  }

  fDEBUG = 0;
  if (fDEBUG) {
    QwMessage << "Finding histogram defination from: " << histname << QwLog::endl;
    QwMessage << tmpstruct << QwLog::endl;
  }
  if (tmpstruct.name_title == fInvalidName && !fTrimHistoEnable) {
    std::cerr << "GetHistParamsFromList:  We haven't found a match of the histogram name: "
	      << histname << std::endl;
    std::cerr << "                        Please check the input file "
	      << fInputFile << std::endl;
    exit(1);
  }
  return tmpstruct;
}


Bool_t QwHistogramHelper::MatchDeviceParamsFromList(const std::string& devicename)
{
  Int_t matched;
  matched=0;
  if (!fTreeTrimFileLoaded || fTrimDisable){//if file is not loaded or trim tree is disable by cmd flag
    
    return kTRUE;//return true for all devices
  }
  for (size_t i = 0; i < fTreeParams.size(); i++) {
    if (DoesMatch(devicename,fTreeParams.at(i).second)) {
      if (fDEBUG)
	QwMessage << " Branch name found " << fTreeParams.at(i).first << QwLog::endl;
      matched++;
    }
  }

  // Warn when multiple identical matches were found
  if (matched > 1) {
    QwWarning << "Multiple identical matches for branch name " << devicename << ":" << QwLog::endl;
  }
  if (matched)
    return kTRUE;
  else
    return kFALSE;
}

Bool_t QwHistogramHelper::MatchVQWKElementFromList(
    const std::string& subsystemname,
    const std::string& moduletype,
    const std::string& elementname)
{
  Int_t matched = 0;
  if (!fTreeTrimFileLoaded || fTrimDisable){//if file is not loaded or trim tree is disable by cmd flag

    return kTRUE;//return true for all devices
  }
  
  for (size_t j = 0; j < fSubsystemList.size(); j++) {
    //    QwMessage << " Subsystem name "<< subsystemname<< " From List "<<fSubsystemList.at(j) <<  QwLog::endl;
    if (DoesMatch(subsystemname,fSubsystemList.at(j))){
      for (size_t i = 0; i < fModuleList.at(j).size(); i++) {
	if (DoesMatch(moduletype,fModuleList.at(j).at(i))) {
	  for (size_t k = 0; k < fVQWKTrimmedList.at(j).at(i).size(); k++) {
	    if (DoesMatch(elementname,fVQWKTrimmedList.at(j).at(i).at(k))){
	      if (fDEBUG)
		QwMessage << "Subsystem " << fSubsystemList.at(j)
                          << " Module Type " << fModuleList.at(j).at(i)
                          << " Element " << fVQWKTrimmedList.at(j).at(i).at(k)
                          << QwLog::endl;
	      matched++;
	    }
	  }
	  break;
	}
      }
    }
  }

  
  // Warn when multiple identical matches were found
  if (matched > 1) {
    QwWarning << "Multiple identical matches for element name " <<elementname << ":" << QwLog::endl;
  }
  if (matched)
    return kTRUE;
  else
    return kFALSE;
}


const QwHistogramHelper::HistParams QwHistogramHelper::GetHistParamsFromFile(
    const std::string& filename,
    const TString& histname)
{
  // The idea is to look up the input file and get the needed histogram parameters
  // For each histogram we are going to scan the input file once, which
  // is not very efficient. But we only construct histograms once per run ...

  HistParams tmpstruct;
  tmpstruct.name_title = fInvalidName;

  // Open the file
  QwParameterFile mapstr(filename.c_str());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment();    // Remove everything after a comment character
    mapstr.TrimWhitespace(); // Get rid of leading and trailing spaces
    if (mapstr.LineIsEmpty())  continue;

    TString tmpname = mapstr.GetTypedNextToken<TString>();
    TString tmptype = mapstr.GetTypedNextToken<TString>();
    if (tmptype != "TH1F" && tmptype != "TH2F") {
      QwError << "QwHistogramHelper::GetHistParamsFromFile:  Unrecognized histogram type: "
              << tmptype << " (tmpname==" << tmpname <<")"<< QwLog::endl;
      continue;
    }

    if (DoesMatch(histname,tmpname)) {
      // Found a match
      mapstr.RewindToLineStart();
      tmpstruct = GetHistParamsFromLine(mapstr);
      tmpstruct.name_title = histname;
    }
  }

  if (fDEBUG) {
    QwMessage << tmpstruct << QwLog::endl;
  }
  if (tmpstruct.name_title == fInvalidName && !fTrimHistoEnable) {
    QwError << "GetHistParamsFromFile:  We haven't found a match of the histogram name: "
	    << histname << QwLog::endl;
    QwError << "                        Please check the input file "
	    << filename << QwLog::endl;
    exit(1);
  }
  return tmpstruct;
}

Bool_t QwHistogramHelper::DoesMatch(const TString& s, const TRegexp& wildcard)
{
  // A very quick and dirty string matching routine using root
  // TString and TRegExp functions. Require the string and wildcard string
  // to have the SAME length to match (much risky if we don't require this),
  // so the only wildcard you want to use here is ".".

  static Ssiz_t len = 0;
  if (wildcard.Index(s,&len) == 0 && len == s.Length()) {
    // found a match!
    return kTRUE;
  } else
    return kFALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////

TH2F* QwHistogramHelper::Construct2DHist(const TString& name_title)
{
  HistParams tmpstruct = GetHistParamsFromList(name_title);
  return Construct2DHist(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////

TH1F* QwHistogramHelper::Construct1DHist(const TString& name_title)
{
  HistParams tmpstruct = GetHistParamsFromList(name_title);
  return Construct1DHist(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////

TProfile2D* QwHistogramHelper::Construct2DProf(const TString& name_title)
{
  HistParams tmpstruct = GetHistParamsFromList(name_title);
  return Construct2DProf(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////

TProfile* QwHistogramHelper::Construct1DProf(const TString& name_title)
{
  HistParams tmpstruct = GetHistParamsFromList(name_title);
  return Construct1DProf(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////

TH2F* QwHistogramHelper::Construct2DHist(const std::string& inputfile, const TString& name_title)
{
  HistParams tmpstruct = GetHistParamsFromFile(inputfile, name_title);
  return Construct2DHist(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////

TH1F* QwHistogramHelper::Construct1DHist(const std::string& inputfile, const TString& name_title)
{
  HistParams tmpstruct = GetHistParamsFromFile(inputfile, name_title);
  return Construct1DHist(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////

TProfile2D* QwHistogramHelper::Construct2DProf(const std::string& inputfile, const TString& name_title)
{
  HistParams tmpstruct = GetHistParamsFromFile(inputfile, name_title);
  return Construct2DProf(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////

TProfile* QwHistogramHelper::Construct1DProf(const std::string& inputfile, const TString& name_title)
{
  HistParams tmpstruct = GetHistParamsFromFile(inputfile, name_title);
  return Construct1DProf(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////

TH1F* QwHistogramHelper::Construct1DHist(const QwHistogramHelper::HistParams &params)
{
  TH1F* h1 = 0;
  if (fTrimHistoEnable && params.name_title == fInvalidName) {
    return h1;
  }

  h1 = new TH1F(params.name_title,
                params.name_title,
                params.x_nbins,
                Double_t(params.x_min),Double_t(params.x_max));
  h1->SetXTitle(params.xtitle);
  h1->SetYTitle(params.ytitle);
  //  if(params.min!=fInvalidNumber) h1->SetMinimum(params.min);
  //  if(params.max!=fInvalidNumber) h1->SetMinimum(params.max);
  return h1;
}

/////////////////////////////////////////////////////////////////////////////////////////

TH2F* QwHistogramHelper::Construct2DHist(const QwHistogramHelper::HistParams &params)
{
  TH2F* h2 = 0;
  if (fTrimHistoEnable && params.name_title == fInvalidName) {
    return h2;
  }

  h2 = new TH2F(params.name_title,
                params.name_title,
                params.x_nbins,
                Double_t(params.x_min),Double_t(params.x_max),
                params.y_nbins,
                Double_t(params.y_min),Double_t(params.y_max));
  h2->SetXTitle(params.xtitle);
  h2->SetYTitle(params.ytitle);
  //  if (params.min != fInvalidNumber) h2->SetMinimum(params.min);
  //  if (params.max != fInvalidNumber) h2->SetMinimum(params.max);
  return h2;
}

/////////////////////////////////////////////////////////////////////////////////////////

TProfile* QwHistogramHelper::Construct1DProf(const QwHistogramHelper::HistParams &params)
{
  TProfile* h1 = 0;
  if (fTrimHistoEnable && params.name_title == fInvalidName) {
    return h1;
  }

  h1 = new TProfile(params.name_title,
                params.name_title,
                params.x_nbins,
                Double_t(params.x_min),Double_t(params.x_max),
                Double_t(params.y_min),Double_t(params.y_max));
  h1->SetMinimum(params.y_min);
  h1->SetMaximum(params.y_max);
  h1->SetXTitle(params.xtitle);
  h1->SetYTitle(params.ytitle);
  return h1;
}

/////////////////////////////////////////////////////////////////////////////////////////

TProfile2D* QwHistogramHelper::Construct2DProf(const QwHistogramHelper::HistParams &params)
{
  TProfile2D* h2 = 0;
  if (fTrimHistoEnable && params.name_title == fInvalidName) {
    return h2;
  }

  h2 = new TProfile2D(params.name_title,
                params.name_title,
                params.x_nbins,
                Double_t(params.x_min),Double_t(params.x_max),
                params.y_nbins,
                Double_t(params.y_min),Double_t(params.y_max),
                Double_t(params.z_min),Double_t(params.z_max));
  h2->SetMinimum(params.z_min);
  h2->SetMaximum(params.z_max);
  h2->SetXTitle(params.xtitle);
  h2->SetYTitle(params.ytitle);
  return h2;
}

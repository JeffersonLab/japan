#include "QwEPICSEvent.h"

// System headers
#include <iostream>
#include <fstream>
#include <cmath>

// ROOT headers
#include "TObject.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"
#include "TFile.h"
#include "TROOT.h"
#include "TMath.h"

// Qweak headers
#include "QwLog.h"
#include "QwParameterFile.h"
#include "QwTypes.h"

#ifdef __USE_DATABASE__
#define MYSQLPP_SSQLS_NO_STATICS
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif //__USE_DATABASE__


/*************************************
 *  Static definitions
 *************************************/

const int QwEPICSEvent::kDebug  = 0;  /* Debugging flag, set this to 1 to  *
				       * enable debugging output, otherwise 0.*/
const int QwEPICSEvent::kEPICS_Error = -1;
const int QwEPICSEvent::kEPICS_OK    =  1;

const Double_t QwEPICSEvent::kInvalidEPICSData = -999999.0;


// Default autogain list
std::vector<std::string> QwEPICSEvent::fDefaultAutogainList;


/*************************************
 *  Constructors/Destructors.
 *************************************/
QwEPICSEvent::QwEPICSEvent():fNumberEPICSVariables(0),
			     fBlinderReversalForRunTwo(kFALSE),
			     fPrecessionReversal(kFALSE),
			     fNominalWienAngle(30.0)
{
  SetDataLoaded(kFALSE);
  QwEPICSEvent::InitDefaultAutogainList();
  if (kDebug == 1) PrintVariableList();
}


QwEPICSEvent::~QwEPICSEvent()
{
}


/*************************************
 *  Member functions.
 *************************************/

/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwEPICSEvent::DefineOptions(QwOptions &options)
{
#ifdef __USE_DATABASE__
  // Option to disable EPICS database accesses
  options.AddOptions("Default options")
    ("disable-db-epics", 
     po::value<bool>()->default_bool_value(false),
     "disable EPICS database access");
#endif
}


/**
 * Parse the configuration options and store in class fields
 * @param options Options object
 */
void QwEPICSEvent::ProcessOptions(QwOptions &options)
{
#ifdef __USE_DATABASE__
  // Option to disable EPICS database accesses
  fDisableDatabase = options.GetValue<bool>("disable-db-epics");
#endif
}

Int_t QwEPICSEvent::LoadChannelMap(TString mapfile)
{
  Int_t lineread = 0;

  fNumberEPICSVariables = 0;
  fEPICSVariableList.clear();
  fEPICSVariableType.clear();

  fExtraHelicityReversal = 1;
  fBlinderReversalForRunTwo = kFALSE;
  fPrecessionReversal       = kFALSE;

  // Open the file
  QwParameterFile mapstr(mapfile.Data());
  std::string varname, varvalue;
  while (mapstr.ReadNextLine()) {
    lineread++;

    mapstr.TrimComment();      // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
      QwDebug << "QwEPICSEvent::LoadChannelMap:  keyword,value pair:"
	      << varname << "," << varvalue << QwLog::endl;
      if (varname == "NominalWienAngle"){
	fNominalWienAngle = atof(varvalue.c_str());
      } else if (varname == "BlinderReversalForRunTwo"){
	if (! fBlinderReversalForRunTwo){
	  fBlinderReversalForRunTwo = kTRUE;
	  fExtraHelicityReversal = -1 * fExtraHelicityReversal;
	}
      } else if (varname == "PrecessionReversal"){
	if (! fPrecessionReversal){
	  fPrecessionReversal = kTRUE;
	  fExtraHelicityReversal = -1 * fExtraHelicityReversal;
	}
      } else {
	QwError << "QwEPICSEvent::LoadChannelMap: "
		<< "Unknown keyword,variable pair: "
		<< varname << "," << varvalue << QwLog::endl;
	exit(1);
      }
      continue;
    }

    varname = mapstr.GetTypedNextToken<std::string>();
    std::string dbtable = mapstr.GetTypedNextToken<std::string>();
    TString datatype    = mapstr.GetTypedNextToken<TString>();
    datatype.ToLower();

    if (datatype == "") {
      AddEPICSTag(varname,dbtable);
    } else {
      EQwEPICSDataType datatypeid = kEPICSString;
      if (datatype == "string")  datatypeid = kEPICSString;
      if (datatype == "float")   datatypeid = kEPICSFloat;
      if (datatype == "int")     datatypeid = kEPICSInt;
      AddEPICSTag(varname,dbtable,datatypeid);
    }
  }
  if (kDebug == 1) std::cout << "fNumberEPICSVariables = " << fNumberEPICSVariables << std::endl << std::endl;
  if (kDebug == 1) std::cout << "line read in the parameter file =" << lineread << std::endl;

  ResetCounters();
  mapstr.Close(); // Close the file (ifstream)
  return 0;
}


/// \brief Construct the branch and tree vector
void QwEPICSEvent::ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values)
{
  fTreeArrayIndex = values.size();
  Int_t treeindex = fTreeArrayIndex;
  for (size_t tagindex = 0; tagindex < fEPICSVariableType.size(); tagindex++) {
    if (fEPICSVariableType[tagindex] == kEPICSString ||
        fEPICSVariableType[tagindex] == kEPICSFloat ||
        fEPICSVariableType[tagindex] == kEPICSInt) {

    	// Add element to vector
    	values.push_back(0.0);

    	// Determine branch name
    	TString name = fEPICSVariableList[tagindex];
    	name.ReplaceAll(':','_'); // remove colons before creating branch
    	TString name_type = name + "/D";

    	// Create branch
    	tree->Branch(name, &(values[treeindex]), name_type);
        treeindex++;

    } else {

        TString name = fEPICSVariableList[tagindex];
        QwError << "Unrecognized type for EPICS variable " << name << QwLog::endl;

    }
  }
  fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
}

/// \brief Fill the tree vector
void QwEPICSEvent::FillTreeVector(std::vector<Double_t>& values) const
{
  Int_t treeindex = fTreeArrayIndex;
  for (size_t tagindex = 0; tagindex < fEPICSVariableType.size(); tagindex++) {
    switch (fEPICSVariableType[tagindex]) {
      case kEPICSFloat:
      case kEPICSInt: {
        // Add value to vector
        values[treeindex] = fEPICSDataEvent[tagindex].Value;
        treeindex++;
        break;
      }
      case kEPICSString: {
        // Add value to vector
        values[treeindex] = static_cast<Double_t>(fEPICSDataEvent[tagindex].StringValue.Hash());
        treeindex++;
        break;
      }
      default: {
        TString name = fEPICSVariableList[tagindex];
        QwError << "Unrecognized type for EPICS variable " << name << QwLog::endl;
        break;
      }
    }
  }
}


Int_t QwEPICSEvent::AddEPICSTag(
    const string& tag,
    const string& table,
    EQwEPICSDataType datatype)
{
  fEPICSVariableList.push_back(tag);
  fEPICSVariableMap[tag] = fEPICSVariableList.size() - 1;
  fEPICSTableList.push_back(table);
  fEPICSVariableType.push_back(datatype);
  fNumberEPICSVariables++;
  return 0;
}


void QwEPICSEvent::CalculateRunningValues()
{
  if (kDebug == 1) std::cout <<"Here we are in 'CalculateRunningValues'!!"<<std::endl;
  if (kDebug == 1) std::cout<<"fNumberEPICSVariables = "<<fNumberEPICSVariables<<std::endl<<std::endl;

  Bool_t anyFilled = kFALSE;
  for (size_t tagindex = 0; tagindex < fEPICSDataEvent.size(); tagindex++) {
    anyFilled |=  fEPICSDataEvent[tagindex].Filled;
  }
  if (! anyFilled) return;

  for (size_t tagindex = 0; tagindex < fEPICSVariableType.size(); tagindex++) {
    if (fEPICSVariableType[tagindex] == kEPICSFloat ||
        fEPICSVariableType[tagindex] == kEPICSInt) {

      if (fEPICSDataEvent[tagindex].Filled) {

        if (! fEPICSCumulativeData[tagindex].Filled) {
          fEPICSCumulativeData[tagindex].NumberRecords = 0;
          fEPICSCumulativeData[tagindex].Sum           = 0.0;
          fEPICSCumulativeData[tagindex].SquaredSum    = 0.0;
          fEPICSCumulativeData[tagindex].Maximum       =
              fEPICSDataEvent[tagindex].Value;
          fEPICSCumulativeData[tagindex].Minimum       =
              fEPICSDataEvent[tagindex].Value;
          fEPICSCumulativeData[tagindex].Filled        = kTRUE;
        }
        fEPICSCumulativeData[tagindex].NumberRecords++;
        fEPICSCumulativeData[tagindex].Sum +=
            fEPICSDataEvent[tagindex].Value;
        fEPICSCumulativeData[tagindex].SquaredSum +=
            (fEPICSDataEvent[tagindex].Value)*(fEPICSDataEvent[tagindex].Value);
        if (fEPICSDataEvent[tagindex].Value
            > fEPICSCumulativeData[tagindex].Maximum) {
          fEPICSCumulativeData[tagindex].Maximum       =
              fEPICSDataEvent[tagindex].Value;
        }
        if (fEPICSDataEvent[tagindex].Value
            < fEPICSCumulativeData[tagindex].Minimum) {
          fEPICSCumulativeData[tagindex].Minimum       =
              fEPICSDataEvent[tagindex].Value;
        }
        if (kDebug == 1) std::cout << "This event has "<<fEPICSVariableList[tagindex]
                                   << " equal to "<< fEPICSDataEvent[tagindex].Value
				   << " giving a running average of "
				   << fEPICSCumulativeData[tagindex].Sum /
				      fEPICSCumulativeData[tagindex].NumberRecords
				   << std::endl;
      } else {
        if (kDebug == 1) std::cout << fEPICSVariableList[tagindex]
                                   << " seems to be not filled."<<std::endl;
      }
    }

    ////////////////////////////////
    else {
      //  This is a string value.
      //  Just check to see if the EPICS string has changed;
      //  if it is the same, increment the counter.

      if (! fEPICSCumulativeData[tagindex].Filled) {
        fEPICSCumulativeData[tagindex].NumberRecords = 0;
        fEPICSCumulativeData[tagindex].SavedString   =
            fEPICSDataEvent[tagindex].StringValue;
        fEPICSCumulativeData[tagindex].Filled        = kTRUE;
      }
      if (fEPICSCumulativeData[tagindex].SavedString
          == fEPICSDataEvent[tagindex].StringValue ) {
        fEPICSCumulativeData[tagindex].NumberRecords++;
      }
    }
  }
  fNumberEPICSEvents++;
  //////////////////////

  if (kDebug == 1) std::cout << "fNumberEPICSEvents = " << fNumberEPICSEvents << std::endl;
}


void QwEPICSEvent::ExtractEPICSValues(const string& data, int event)
{
  /* This routine will decode the input string, which holds the  *
   * epics buffer and extract the EPICS values from it.          */

  if (kDebug == 1) std::cout <<"Here we are, entering 'ExtractEPICSValues'!!"<<std::endl;

  for (size_t tagindex = 0; tagindex < fEPICSVariableList.size(); tagindex++) {
    fEPICSDataEvent[tagindex].Filled = kFALSE;
  }

  // Convert the input stream into a stringstream so we can
  // do manipulation on it just like reading a file
  std::stringstream ss(data, std::stringstream::in | std::stringstream::out);
  QwParameterFile file(ss);
  while (file.ReadNextLine()) {
    file.TrimWhitespace();
    string varname, varvalue;
    if (file.HasVariablePair(" \t\n", varname, varvalue)) {
      Int_t tagindex = FindIndex(varname);
      if (tagindex != kEPICS_Error) {
        SetDataValue(tagindex, varvalue, event);
        SetDataLoaded(kTRUE);
      }
    }
  }
  if (fIsDataLoaded) {
    //  Determine the WienMode and save it.
    SetDataValue("WienMode",WienModeName(DetermineWienMode()),event);
  }
}


Int_t QwEPICSEvent::FindIndex(const string& tag) const
{
  // Find a match for the tag
  std::map<std::string,Int_t>::const_iterator match = fEPICSVariableMap.find(tag);

  if (match != fEPICSVariableMap.end())
    // A match was found
    return match->second;

  else
    // Otherwise return error
    return kEPICS_Error;
}


Double_t QwEPICSEvent::GetDataValue(const string& tag) const
{
  Double_t data_value = kInvalidEPICSData;
  Int_t tagindex = FindIndex(tag);
  if (tagindex != kEPICS_Error) {
    if (fEPICSDataEvent[tagindex].Filled) {
      data_value = fEPICSDataEvent[tagindex].Value;
    }
  }
  return data_value;
}

TString QwEPICSEvent::GetDataString(const string& tag) const
{
  Int_t tagindex = FindIndex(tag);
  if (tagindex != kEPICS_Error) {
    if (fEPICSVariableType[tagindex]==kEPICSString
	&& fEPICSDataEvent[tagindex].Filled) {
      return(fEPICSDataEvent[tagindex].StringValue);
    }
  }
  return TString("");
}

void QwEPICSEvent::InitDefaultAutogainList()
{
	// Clear the vector first
	fDefaultAutogainList.clear();

	// Add default autogain channels
  fDefaultAutogainList.push_back("IPM3C17.XIFG");
  fDefaultAutogainList.push_back("IPM3C17.YIFG");
  fDefaultAutogainList.push_back("IBC3C17.XIFG");
  fDefaultAutogainList.push_back("IBC3C17.YIFG");
  fDefaultAutogainList.push_back("IPM3C18.XIFG");
  fDefaultAutogainList.push_back("IPM3C18.YIFG");
  fDefaultAutogainList.push_back("IPM3C19.XIFG");
  fDefaultAutogainList.push_back("IPM3C19.YIFG");
  fDefaultAutogainList.push_back("IPM3C03.YIFG");
  fDefaultAutogainList.push_back("IPM3P01.XIFG");
  fDefaultAutogainList.push_back("IPM3P01.YIFG");
  fDefaultAutogainList.push_back("IPM3P02A.XIFG");
  fDefaultAutogainList.push_back("IPM3P02A.YIFG");
  fDefaultAutogainList.push_back("IPM3P03A.XIFG");
  fDefaultAutogainList.push_back("IPM3P03A.YIFG");
  fDefaultAutogainList.push_back("IPM3P02B.XIFG");
  fDefaultAutogainList.push_back("IPM3P02B.YIFG");
  fDefaultAutogainList.push_back("IPM3C20.XIFG");
  fDefaultAutogainList.push_back("IPM3C20.YIFG");
  fDefaultAutogainList.push_back("IPM3C21.XIFG");
  fDefaultAutogainList.push_back("IPM3C21.YIFG");
  fDefaultAutogainList.push_back("IPM3H02.XIFG");
  fDefaultAutogainList.push_back("IPM3H02.YIFG");
  fDefaultAutogainList.push_back("IPM3H04.XIFG");
  fDefaultAutogainList.push_back("IPM3H04.YIFG");
  fDefaultAutogainList.push_back("IPM3H07A.XIFG");
  fDefaultAutogainList.push_back("IPM3H07A.YIFG");
  fDefaultAutogainList.push_back("IPM3H07B.XIFG");
  fDefaultAutogainList.push_back("IPM3H07B.YIFG");
  fDefaultAutogainList.push_back("IPM3H07C.XIFG");
  fDefaultAutogainList.push_back("IPM3H07C.YIFG");
  fDefaultAutogainList.push_back("IPM3H09.XIFG");
  fDefaultAutogainList.push_back("IPM3H09.YIFG");
  fDefaultAutogainList.push_back("IPM3H09B.XIFG");
  fDefaultAutogainList.push_back("IPM3H09B.YIFG");
}

void QwEPICSEvent::SetDefaultAutogainList(std::vector<std::string>& input_list)
{
  fDefaultAutogainList.clear();  //clear the vector first
  fDefaultAutogainList = input_list;
}


int QwEPICSEvent::SetDataValue(const string& tag, const double value, const int event)
{
  Int_t tagindex = FindIndex(tag);
  return (SetDataValue(tagindex, value, event));
}

int QwEPICSEvent::SetDataValue(const string& tag, const string& value, const int event)
{
  Int_t tagindex = FindIndex(tag);
  return (SetDataValue(tagindex, value, event));
}

int QwEPICSEvent::SetDataValue(int index, const double value, const int event)
{
  if (index == kEPICS_Error) return kEPICS_Error;
  if (index < 0)             return kEPICS_Error;

  if (value != kInvalidEPICSData) {
    fEPICSDataEvent[index].EventNumber = event;
    fEPICSDataEvent[index].Value       = value;
    fEPICSDataEvent[index].StringValue = "";
    fEPICSDataEvent[index].Filled      = kTRUE;
    return 0;
  }
  return kEPICS_Error;
}

int QwEPICSEvent::SetDataValue(int index, const string& value, const int event)
{
  if (index == kEPICS_Error) return kEPICS_Error;
  if (index < 0)             return kEPICS_Error;

  Double_t tmpvalue = kInvalidEPICSData;
  switch (fEPICSVariableType[index]) {
  case kEPICSString:
    fEPICSDataEvent[index].EventNumber = event;
    fEPICSDataEvent[index].Value       = 0.0;
    fEPICSDataEvent[index].StringValue = value;
    fEPICSDataEvent[index].Filled      = kTRUE;
    return 0;
    break;

  case kEPICSFloat:
    if (IsNumber(value)) {
      tmpvalue = Double_t(atof(value.c_str()));
    }
    return SetDataValue(index, tmpvalue, event);
    break;

  case kEPICSInt:
    if(IsNumber(value)) {
      tmpvalue = Double_t(atol(value.c_str()));
    }
    return SetDataValue(index, tmpvalue, event);
    break;
  }

  return kEPICS_Error;
}

void QwEPICSEvent::PrintAverages() const
{
  Double_t mean     = 0.0;
  Double_t average_of_squares = 0.0;
  Double_t variance = 0.0;
  Double_t sigma    = 0.0;

  std::cout <<"#####  EPICS Event Summary from PrintAverages() #####\n"
	    <<"Total number of EPICS events in this run == "
	    <<fNumberEPICSEvents <<".\n\n"
	    << std::setw(20) << std::setiosflags(std::ios::left) <<"Name"
	    <<"\tMean        Sigma       "
	    <<"Minimum     Maximum"<<std::endl;
  std::cout << "*****Non-string EPICS Variables*****" << std::endl;
  for (size_t tagindex = 0; tagindex < fEPICSVariableList.size(); tagindex++) {
    if (fEPICSVariableType[tagindex] == kEPICSFloat ||
        fEPICSVariableType[tagindex] == kEPICSInt) {

      mean     = 0.0;
      variance = 0.0;
      sigma    = 0.0;
      if (fEPICSCumulativeData[tagindex].NumberRecords > 0){
	mean     = (fEPICSCumulativeData[tagindex].Sum)/
	  ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
	average_of_squares = (fEPICSCumulativeData[tagindex].SquaredSum)/
	  ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
	variance = average_of_squares - (mean * mean);
	if (variance < 0.0){
	  sigma    = sqrt(-1.0 * variance);
	} else {
	  sigma    = sqrt(variance);
	}

	std::cout << std::setw(20) << std::setiosflags(std::ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
		  << std::setprecision(5) << std::setw(10)
		  <<mean<<"  " << std::setw(10)<<sigma<<"  " << std::setw(10)
		  <<fEPICSCumulativeData[tagindex].Minimum<<"  " << std::setw(10)
		  <<fEPICSCumulativeData[tagindex].Maximum<<std::endl;
      } else {
	std::cout << std::setw(20) << std::setiosflags(std::ios::left)
		  <<fEPICSVariableList[tagindex]<<"\t"
		  << std::setprecision(5) << std::setw(10)
		  << "...No data..."
		  << std::endl;
      }
    }
  }

  std::cout << "*****String EPICS Variables are below, if any*****" << std::endl;
  for (size_t tagindex = 0; tagindex < fEPICSVariableList.size(); tagindex++) {
    if (fEPICSVariableType[tagindex] == kEPICSString) {
      if (fEPICSDataEvent[tagindex].Filled) {
	std::cout << std::setw(20) << std::setiosflags(std::ios::left)
		  <<"tagindex for variable "<< fEPICSVariableList[tagindex]<<"\t is "
		  <<tagindex<<"\tand status is\t\t"
		  <<fEPICSDataEvent[tagindex].StringValue;

	if (fEPICSCumulativeData[tagindex].NumberRecords!= fNumberEPICSEvents){
	  std::cout << "\t*****\tThis variable changed during the run.  Only "
		    << (Double_t(fEPICSCumulativeData[tagindex].NumberRecords)
			*100.0 / Double_t(fNumberEPICSEvents))
		    << "% of the events has this value.";
	}
	std::cout <<std::endl;
      } else {
	std::cout << std::setw(20) << std::setiosflags(std::ios::left)
		  <<fEPICSVariableList[tagindex]<<"\t"
		  <<"No data..."<<std::endl;
      }
    }
  }
}


void QwEPICSEvent::PrintVariableList() const
{
  // Prints all valid Epics variables on the terminal while creating a rootfile.
  for (size_t tagindex = 0; tagindex < fEPICSVariableList.size(); tagindex++) {
    QwMessage << "fEPICSVariableList[" << tagindex << "] == "
              << fEPICSVariableList[tagindex] << QwLog::endl;
  }
}


std::vector<Double_t> QwEPICSEvent::ReportAutogains(std::vector<std::string> tag_list)
{
  std::vector<Double_t> autogain_values;
  std::vector<std::string>::iterator ptr_tag;

  autogain_values.clear(); //clear the vector first

  /*  Iterate through the input vector of the variable names     *
   *  and fill the output vector with the values.                */
  ptr_tag = tag_list.begin();
  while (ptr_tag < tag_list.end()) {
    autogain_values.push_back(GetDataValue(*ptr_tag));
    ptr_tag++;
  }

  return autogain_values;
}



void QwEPICSEvent::ReportEPICSData() const
{
  Double_t mean     = 0.0;
  Double_t average_of_squares = 0.0;
  Double_t variance = 0.0;
  Double_t sigma    = 0.0;


  std::ofstream output;
  TString theEPICSDataFile;
  theEPICSDataFile =  getenv("QW_TMP");
  theEPICSDataFile += "/QwEPICSData.txt";// puts QwEPICSData.txt in  QwAnalysis_DB_01.00.0000/scratch/tmp/ diretory.
  output.open(theEPICSDataFile,std::ofstream::out);

  if (output.is_open()) {


    output <<"#####  EPICS Event Summary  #####\n"
	   <<"Total number of EPICS events in this run == "
	   <<fNumberEPICSEvents <<".\n\n"
	   << std::setw(20) << std::setiosflags(std::ios::left) <<"Name"
	   <<"\tMean        Sigma       "
	   <<"Minimum     Maximum"<<std::endl;
    for (size_t tagindex = 0; tagindex < fEPICSVariableList.size(); tagindex++) {
      if (fEPICSVariableType[tagindex] == kEPICSFloat ||
	  fEPICSVariableType[tagindex] == kEPICSInt) {
	mean     = 0.0;
	variance = 0.0;
	sigma    = 0.0;
	if (fEPICSCumulativeData[tagindex].NumberRecords > 0){
	  mean     = (fEPICSCumulativeData[tagindex].Sum)/
	    ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
	  average_of_squares = (fEPICSCumulativeData[tagindex].SquaredSum)/
	    ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
	  variance = average_of_squares - (mean * mean);
	  if (variance < 0.0){
	    sigma    = sqrt(-1.0 * variance);
	  } else {
	    sigma    = sqrt(variance);
	  }

	  output << std::setw(20) << std::setiosflags(std::ios::left)
		 <<fEPICSVariableList[tagindex]<<"\t"
		 << std::setprecision(5) << std::setw(10)
		 <<mean<<"  " << std::setw(10)<<sigma<<"  " << std::setw(10)
		 <<fEPICSCumulativeData[tagindex].Minimum<<"  " << std::setw(10)
		 <<fEPICSCumulativeData[tagindex].Maximum<<std::endl;
	} else {
	  output << std::setw(20) << std::setiosflags(std::ios::left)
		 <<fEPICSVariableList[tagindex]<<"\t"
		 << std::setprecision(5) << std::setw(10)
		 << "-No data-"
		 << std::endl;
	}
      }

    }
    for (size_t tagindex = 0; tagindex < fEPICSVariableList.size(); tagindex++) {
      if (fEPICSVariableType[tagindex] == kEPICSString) {
	if (fEPICSDataEvent[tagindex].Filled) {
	  output << std::setw(20) << std::setiosflags(std::ios::left)
		 <<fEPICSVariableList[tagindex]<<"\t"
		 <<fEPICSDataEvent[tagindex].StringValue<<std::endl;
	} else {
	  output << std::setw(20) << std::setiosflags(std::ios::left)
		 <<fEPICSVariableList[tagindex]<<"\t"
		 <<"No data..."<<std::endl;
	}
      }
    }

  } output.close();
}

void  QwEPICSEvent::ResetCounters()
{
  fIsDataLoaded = kFALSE;
  /*  Verify that the variable list and variable type vectors
      are the same size, and agree with fNumberEPICSVariables.  */
  Int_t listlength = fEPICSVariableList.size();
  Int_t typelength = fEPICSVariableType.size();
  if (typelength != fNumberEPICSVariables ||
      listlength != fNumberEPICSVariables) {
    std::cerr << "The EPICS variable label and type arrays are not the same size!\n"
	      << "EPICS readback may be corrupted!"<<std::endl;
  }
  /*  Reset the sizes of the fEPICSDataEvent and
      fEPICSCumulativeData vectors.                             */
  fEPICSDataEvent.resize(fNumberEPICSVariables);
  fEPICSCumulativeData.resize(fNumberEPICSVariables);
  for (size_t tagindex = 0; tagindex < fEPICSVariableList.size(); tagindex++) {
    fEPICSDataEvent[tagindex].Filled      = kFALSE;
    fEPICSDataEvent[tagindex].EventNumber = 0;
    fEPICSDataEvent[tagindex].Value       = 0.0;
    fEPICSDataEvent[tagindex].StringValue = "";
    fEPICSCumulativeData[tagindex].Filled        = kFALSE;
    fEPICSCumulativeData[tagindex].NumberRecords = 0;
    fEPICSCumulativeData[tagindex].Sum           = 0.0;
    fEPICSCumulativeData[tagindex].SquaredSum    = 0.0;
    fEPICSCumulativeData[tagindex].Minimum       = 0.0;
    fEPICSCumulativeData[tagindex].Maximum       = 0.0;
  }
  fNumberEPICSEvents = 0;
}

#ifdef __USE_DATABASE__
void QwEPICSEvent::FillDB(QwParityDB *db)
{
  // Sunday, January 16 22:09:16 EST 2011, jhlee
  // don't change disbale database flag
  // just disable FillSlowControlsSettings(db) when fDisableDatabase is off

  bool hold_fDisableDatabase = fDisableDatabase;

   try {
    db->Connect();
    mysqlpp::Query query= db->Query();
    query << "SELECT slow_controls_settings_id FROM slow_controls_settings WHERE";
    query << " runlet_id = " << mysqlpp::quote << db->GetRunletID(); 

    mysqlpp::StoreQueryResult res = query.store();

    if (res.num_rows() != 0) {
      QwError << "This runlet already has slow controls entries in the database!" << QwLog::endl;
      QwError << "The following slow_controls_settings_id values already exist in the database:  ";
      for (size_t i=0; i<res.num_rows(); i++) {
        QwError << res[i][0] << " ";
      }
      QwError << QwLog::endl;
      QwError << "Slow controls values from this replay will NOT be stored in the database."  << QwLog::endl;

      fDisableDatabase=true;
    }

    db->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    QwError << "Unable to determine if there are other slow controls entries in the database for this run.  THERE MAY BE DUPLICATES." << QwLog::endl;
    db->Disconnect();
  }

 
  if (! fDisableDatabase) {
    FillSlowControlsData(db);
    FillSlowControlsStrigs(db);
    FillSlowControlsSettings(db);
  }
  fDisableDatabase=hold_fDisableDatabase;
}


void QwEPICSEvent::FillSlowControlsData(QwParityDB *db)
{

  QwDebug << " -------------------------------------------------------------------------- " << QwLog::endl;
  QwDebug << "                         QwEPICSEvent::FillSlowControlsData(QwParityDB *db) " << QwLog::endl;
  QwDebug << " -------------------------------------------------------------------------- " << QwLog::endl;

  Double_t mean, average_of_squares, variance, sigma;
  Int_t n_records;

  mean     = 0.0;
  average_of_squares = 0.0;
  variance = 0.0;
  sigma    = 0.0;
  n_records = 0;
  //  Figure out if the target table has this runlet_id in it already,
  //  if not, create a new entry with the current runlet_id.

  UInt_t runlet_id = db->GetRunletID();

  std::vector<QwParitySSQLS::slow_controls_data> entrylist;

  UInt_t sc_detector_id;

  string table = "slow_controls_data";

  // QwError << "Step 1 Entering the loop " << QwLog::endl;
  // Loop over EPICS variables
  for (size_t tagindex = 0; tagindex < fEPICSVariableList.size(); tagindex++) {

    // Look for variables to write into this table
    if (fEPICSTableList[tagindex] == table) {
      QwParitySSQLS::slow_controls_data tmp_row(0);

      //  Now get the current sc_detector_id for the above runlet_id.
      sc_detector_id = db->GetSlowControlDetectorID(fEPICSVariableList[tagindex]);

      tmp_row.runlet_id      = runlet_id;
      tmp_row.sc_detector_id = sc_detector_id;

      if (!sc_detector_id) continue;


      // Calculate average and error
      if (fEPICSVariableType[tagindex] == kEPICSFloat
	  || fEPICSVariableType[tagindex] == kEPICSInt) {
	if (fEPICSCumulativeData[tagindex].NumberRecords > 0){

	  mean     = (fEPICSCumulativeData[tagindex].Sum)/
	    ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
          average_of_squares = (fEPICSCumulativeData[tagindex].SquaredSum)/
            ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
          variance = average_of_squares - (mean * mean);
          if (variance < 0.0){
            sigma    = sqrt(-1.0 * variance);
          } else {
            sigma    = sqrt(variance);
          }
          n_records = fEPICSCumulativeData[tagindex].NumberRecords;

	  //  Build the row and submit it to the list
	  tmp_row.n             = n_records;
	  tmp_row.value         = mean;
	  tmp_row.error         = sigma;
	  tmp_row.min_value     = fEPICSCumulativeData[tagindex].Minimum;
	  tmp_row.max_value     = fEPICSCumulativeData[tagindex].Maximum;
	  
	  entrylist.push_back(tmp_row);
	}
      }
    }
  }


  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    QwDebug << "QwEPICSEvent::FillSlowControlsData::Writing to database now" << QwLog::endl;
    mysqlpp::Query query= db->Query();

    //    query.insert(entrylist.begin(), entrylist.end());

    //QwDebug << "Query: " << query.str() << QwLog::endl;

    //    query.execute();

    ///////////////////////////////
    try {
      query.insert(entrylist.begin(), entrylist.end());
      QwDebug << "Query: " << query.str() << QwLog::endl;
      query.execute();
      QwDebug << "Done executing MySQL query" << QwLog::endl;
    } catch (const mysqlpp::Exception &er) {
      QwError << "MySQL exception: " << er.what() << QwLog::endl;
    }
    ///////////////////////////////

  } else {
    QwDebug << "QwEPICSEvent::FillSlowControlsData :: This is the case when the entrylist contains nothing " << QwLog::endl;
  }
  db->Disconnect();
}


void QwEPICSEvent::FillSlowControlsStrigs(QwParityDB *db)
{
  std::vector<QwParitySSQLS::slow_controls_strings> entrylist;
  UInt_t sc_detector_id;
  UInt_t runlet_id = db->GetRunletID();
  string table = "polarized_source";

  // QwError << "Step 1 Entering the loop " << QwLog::endl;
  // Loop over EPICS variables
  for (size_t tagindex = 0; tagindex < fEPICSVariableList.size(); tagindex++) {
    // Look for variables to write into this table

    if (fEPICSTableList[tagindex] == table) {
      QwParitySSQLS::slow_controls_strings tmp_row(0);

      //  Now get the current sc_detector_id for the above runlet_id.
      sc_detector_id = db->GetSlowControlDetectorID(fEPICSVariableList[tagindex]);

      tmp_row.runlet_id      = runlet_id;
      tmp_row.sc_detector_id = sc_detector_id;

      if (!sc_detector_id) continue;

      if (fEPICSVariableType[tagindex] == kEPICSString) {

    	if (fEPICSDataEvent[tagindex].Filled) {
	  if(fEPICSDataEvent[tagindex].StringValue.Contains("***") ){
	    QwWarning<<"fEPICSDataEvent[tagindex].StringValue.Data() is not defined, tmp_row.value is set to an empty string."<<QwLog::endl;
	    tmp_row.value ="";
	  }
	  else {
	    //std::cout<<" Just a test value: "<<fEPICSDataEvent[tagindex].StringValue.Data()<<QwLog::endl;
	    tmp_row.value = fEPICSDataEvent[tagindex].StringValue.Data();
	  }
	  //  Only add rows for filled variables
	  entrylist.push_back(tmp_row);
	}
      }
    }
  }

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query.
  if( entrylist.size() ) {
    QwDebug << "QwEPICSEvent::FillSlowControlsStrigs Writing to database now" << QwLog::endl;
    mysqlpp::Query query= db->Query();
//     query.insert(entrylist.begin(), entrylist.end());

//     QwDebug << "Query: " << query.str() << QwLog::endl;

//     query.execute();


    ///////////////////////////////
        try {
          query.insert(entrylist.begin(), entrylist.end());
          QwDebug << "Query: " << query.str() << QwLog::endl;
          query.execute();
          QwDebug << "Done executing MySQL query for FillSlowControlsStrings"
		  << QwLog::endl;
        } catch (const mysqlpp::Exception &er) {
          QwError << "MySQL exception: " << er.what() << QwLog::endl;
        }
    ///////////////////////////////

  } else {
    QwDebug << "QwEPICSEvent::FillSlowControlsData :: This is the case when the entrylist contains nothing " << QwLog::endl;
  }
  db->Disconnect();
}


void QwEPICSEvent::FillSlowControlsSettings(QwParityDB *db)
{
  QwParitySSQLS::slow_controls_settings  tmp_row(0);
  std::vector<QwParitySSQLS::slow_controls_settings> entrylist;

  tmp_row.slow_helicity_plate = mysqlpp::null;
  tmp_row.wien_reversal = mysqlpp::null;
  tmp_row.helicity_length = mysqlpp::null;
  tmp_row.charge_feedback = mysqlpp::null;
  tmp_row.position_feedback = mysqlpp::null;
  tmp_row.qtor_current = mysqlpp::null;
  tmp_row.target_position = mysqlpp::null;

  UInt_t runlet_id = db->GetRunletID();
  tmp_row.runlet_id      = runlet_id;
  Int_t tagindex;

  // Add as many blocks as needed in the following for all slow_controls_settings.

  ////////////////////////////////////////////////////////////

  // For QTOR current
  tagindex = FindIndex("qw:qt_mps_i_dcct");
  if (tagindex != kEPICS_Error) {
    QwDebug << "tagindex for  = qw:qt_mps_i_dcct" << tagindex << QwLog::endl;
    if (! fEPICSCumulativeData[tagindex].Filled) {
      //  No data for this run.
      tmp_row.qtor_current = mysqlpp::null;
    } else if (fEPICSCumulativeData[tagindex].NumberRecords <= 0) {
      // No events in this variable
      QwWarning << "The value of "
		<< fEPICSVariableList[tagindex]
		<< " had no events during this run.  "
		<< "Send NULL word to the database."
		<< QwLog::endl;
      tmp_row.qtor_current = mysqlpp::null;
    } else {
      Double_t qtorcurrent = (fEPICSCumulativeData[tagindex].Sum)/
	((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
      QwDebug << "Send the value of "
	      << fEPICSVariableList[tagindex]
	      << ", "
	      << qtorcurrent
	      << ", to the database."
	      << QwLog::endl;
      tmp_row.qtor_current = qtorcurrent;
    }
  }

  ////////////////////////////////////////////////////////////

  // For target position
  tagindex = FindIndex("QWtgt_name");
  if (tagindex != kEPICS_Error) {
  QwDebug << "tagindex for  = QWtgt_name" << tagindex << QwLog::endl;
  if (! fEPICSCumulativeData[tagindex].Filled) {
    //  No data for this run.
    tmp_row.target_position = mysqlpp::null;
  } else if (fEPICSCumulativeData[tagindex].NumberRecords
	     != fNumberEPICSEvents) {
    // Target position changed
    QwWarning << "The value of "
	      << fEPICSVariableList[tagindex]
	      << " changed during this run.  "
	      << "Send NULL word to the database."
	      << QwLog::endl;
    tmp_row.target_position = mysqlpp::null;
  }
  if(fEPICSDataEvent[tagindex].StringValue.Contains("***") ){
    QwWarning << "The value of "
	      << fEPICSVariableList[tagindex]
	      << " is not defined."
	      << "Send NULL word to the database."
	      << QwLog::endl;
    tmp_row.target_position =mysqlpp::null;
  }  else {
    // Target position did not change.
    // Store the position as a text string.
    QwDebug << "Send the value of "
	    << fEPICSVariableList[tagindex]
	    << ", "
	    << fEPICSDataEvent[tagindex].StringValue.Data()
	    << ", to the database."
	    << QwLog::endl;
    tmp_row.target_position = fEPICSDataEvent[tagindex].StringValue.Data();
  }
  }

  ////////////////////////////////////////////////////////////

  // For insertable Half Wave Plate Setting
  tagindex = FindIndex("IGL1I00DI24_24M");
  if (tagindex != kEPICS_Error) {
  QwDebug << "tagindex for IGL1I00DI24_24M = " << tagindex << QwLog::endl;

  if (! fEPICSCumulativeData[tagindex].Filled) {
    //  No data for this run.
    tmp_row.slow_helicity_plate = mysqlpp::null;
  } else if (fEPICSCumulativeData[tagindex].NumberRecords
	     != fNumberEPICSEvents) {
    // Insertable Half Wave Plate Setting position changed
    QwWarning << "The value of "
	      << fEPICSVariableList[tagindex]
	      << " changed during the run."
	      << "Send NULL word to the database."
	      << QwLog::endl;
    tmp_row.slow_helicity_plate = mysqlpp::null;
  }
  if(fEPICSDataEvent[tagindex].StringValue.Contains("***") ){
    QwWarning << "The value of "
	      << fEPICSVariableList[tagindex]
	      << " is not defined."
	      << "Send NULL word to the database."
	      << QwLog::endl;
    tmp_row.slow_helicity_plate =mysqlpp::null;
  } else {
    // Insertable Half Wave Plate Setting position did not change
    // Insertable Half Wave Plate Setting setting is stored as a string with possible values
    // "OUT" and "IN".
    QwDebug << "Send the value of "
	    << fEPICSVariableList[tagindex]
	    << ", "
	    << fEPICSDataEvent[tagindex].StringValue.Data()
	    << ", to the database."
	    << QwLog::endl;
    tmp_row.slow_helicity_plate = fEPICSDataEvent[tagindex].StringValue.Data();
  }
  }

  // For IHWP2 Setting
  // IGL1I00DIOFLRD: Passive IHWP readback (13056=IN,8960=OUT)
  tagindex = FindIndex("IGL1I00DIOFLRD");
  if (tagindex != kEPICS_Error) {
    QwDebug << "tagindex for IGL1I00DIOFLRD = " << tagindex << QwLog::endl;
    
    if (! fEPICSCumulativeData[tagindex].Filled) {
      //  No data for this run.
      tmp_row.passive_helicity_plate = mysqlpp::null;
    } else if (fEPICSCumulativeData[tagindex].NumberRecords
	       != fNumberEPICSEvents) {
      // IHWP2 Setting position changed
      QwWarning << "The value of "
		<< fEPICSVariableList[tagindex]
		<< " changed during the run."
		<< "Send NULL word to the database."
		<< QwLog::endl;
      tmp_row.passive_helicity_plate = mysqlpp::null;
    } else {
      Double_t ihwp2_readback = (fEPICSCumulativeData[tagindex].Sum)/
	((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
      if (fabs(ihwp2_readback-13056)<1){
	// IHWP2 is IN
	QwDebug << "Send the value of "
		<< fEPICSVariableList[tagindex]
		<< ", "
		<< fEPICSDataEvent[tagindex].StringValue.Data()
		<< ", to the database."
		<< QwLog::endl;
	tmp_row.passive_helicity_plate = "in";
      } else if (fabs(ihwp2_readback-8960)<1){
	// IHWP2 is OUT
	QwDebug << "Send the value of "
		<< fEPICSVariableList[tagindex]
		<< ", "
		<< fEPICSDataEvent[tagindex].StringValue.Data()
		<< ", to the database."
		<< QwLog::endl;
	tmp_row.passive_helicity_plate = "out";
      } else {
	QwWarning << "The value of "
		  << fEPICSVariableList[tagindex]
		  << " is not defined."
		  << "Send NULL word to the database."
		  << QwLog::endl;
	tmp_row.passive_helicity_plate =mysqlpp::null;
      }
    }
  }

 // For Wien Setting
  tagindex = FindIndex("WienMode");
  if (tagindex != kEPICS_Error) {
    QwDebug << "tagindex for WienMode = " << tagindex << QwLog::endl;

    if (! fEPICSCumulativeData[tagindex].Filled) {
      //  No data for this run.
      tmp_row.wien_reversal = mysqlpp::null;
    } else if (fEPICSCumulativeData[tagindex].NumberRecords
	       != fNumberEPICSEvents) {
      // WienMode changed
      QwWarning << "The value of "
		<< fEPICSVariableList[tagindex]
		<< " changed during the run."
		<< "Send NULL word to the database."
		<< QwLog::endl;
      tmp_row.wien_reversal = mysqlpp::null;
    }
    if(fEPICSDataEvent[tagindex].StringValue.Contains("***") ){
      QwWarning << "The value of "
		<< fEPICSVariableList[tagindex]
		<< " is not defined."
		<< "Send NULL word to the database."
		<< QwLog::endl;
      tmp_row.wien_reversal =mysqlpp::null;
    } else {
      // WienMode is stored as an enum of the following labels:
      TString wien_enum[5] = {"indeterminate",
			      "normal","reverse",
			      "transverse_vertical",
			      "transverse_horizontal"};
      QwDebug << "Send the value of "
	      << fEPICSVariableList[tagindex]
	      << ", "
	      << fEPICSDataEvent[tagindex].StringValue.Data()
	      << ", to the database."
	      << QwLog::endl;
      tmp_row.wien_reversal = wien_enum[WienModeIndex(fEPICSDataEvent[tagindex].StringValue)].Data();
    }
  }
  
  // For the precession reversal
  //   This just uses the flag from the channel map to determine if the precession 
  //   is normal or reversed.
  if (fPrecessionReversal){
    tmp_row.precession_reversal = "reverse";
  } else {
    tmp_row.precession_reversal = "normal";
  }

  // For charge feedback

  tagindex = FindIndex("qw:ChargeFeedback");
  if (tagindex != kEPICS_Error) {
  //std::cout << "tagindex for qw:ChargeFeedback = " << tagindex << std::endl;

  if (! fEPICSCumulativeData[tagindex].Filled) {
    //  No data for this run.
    tmp_row.charge_feedback = mysqlpp::null;

  } else if (fEPICSCumulativeData[tagindex].NumberRecords
	     != fNumberEPICSEvents) {
    // charge feedback status changed
    QwWarning << "The value of "
	      << fEPICSVariableList[tagindex]
	      << " changed during the run."
	      << "Send NULL word to the database."
	      << QwLog::endl;
    tmp_row.charge_feedback = mysqlpp::null;
  }

  if(fEPICSDataEvent[tagindex].StringValue.Contains("***") ){
    QwWarning << "The value of "
	      << fEPICSVariableList[tagindex]
	      << " is not defined."
	      << "Send NULL word to the database."
	      << QwLog::endl;
    tmp_row.charge_feedback =mysqlpp::null;
  }

  else {
    // charge feedback setting did not change
    //   charge feedback setting is stored as a string with possible values
    //   "on" and "off".
    QwDebug << "Send the value of "
	    << fEPICSVariableList[tagindex]
	    << ", "
	    << fEPICSDataEvent[tagindex].StringValue.Data()
	    << ", to the database."
	    << QwLog::endl;
    TString tmpval = fEPICSDataEvent[tagindex].StringValue;
    tmpval.ToLower();
    tmp_row.charge_feedback = tmpval.Data();
  }
  }

  ////////////////////////////////////////////////////////////



  entrylist.push_back(tmp_row);

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    QwDebug << "QwEPICSEvent::FillSlowControlsSettings Writing to database now" << QwLog::endl;
    mysqlpp::Query query= db->Query();
    //   query.insert(entrylist.begin(), entrylist.end());
    //     QwDebug << "Query: " << query.str() <<  QwLog::endl;
    //     query.execute();
    ///////////////////////////////
    try {
      query.insert(entrylist.begin(), entrylist.end());
      QwDebug << "Query: " << query.str() << QwLog::endl;
      query.execute();
      QwDebug << "Done executing MySQL query" << QwLog::endl;
    } catch (const mysqlpp::Exception &er) {
      QwError << "MySQL exception: " << er.what() << QwLog::endl;
    }
    ///////////////////////////////


  } else {
    QwDebug << "QwEPICSEvent::FillSlowControlsSettings :: This is the case when the entrylist contains nothing " << QwLog::endl;
  }
  db->Disconnect();
  QwDebug << "Leaving QwEPICSEvent::FillSlowControlsStrings()" << QwLog::endl;
}
#endif //__USE_DATABASE__

TList *QwEPICSEvent::GetEPICSStringValues()
{
  Bool_t local_debug = false;

  TList *string_list = new TList;
  string_list->SetOwner(true);

  std::size_t tagindex = 0;
  
  for (tagindex=0; tagindex<fEPICSVariableList.size(); tagindex++) 
    {
      if (fEPICSVariableType[tagindex] == kEPICSString) {

	TString epics_string = fEPICSVariableList[tagindex];
	epics_string += "---";

	if (fEPICSDataEvent[tagindex].Filled) {
	  epics_string += fEPICSDataEvent[tagindex].StringValue;
	} 
	else {
	  epics_string += "empty";
	}
	if(local_debug) {
	  std::cout << "QwEPICSEvent::GetEPICSStringValues() "
		    << epics_string
		    << std::endl;
	}
	string_list -> Add(new TObjString(epics_string));
      }
    }
    
  return string_list;
}

void QwEPICSEvent::WriteEPICSStringValues()
{
  QwDebug << "Entering QwEPICSEvent::WriteEPICSStringValues()"  << QwLog::endl;

  Bool_t local_debug = false;

  TSeqCollection *file_list = gROOT->GetListOfFiles();

  if (file_list) {
    
    Int_t size = file_list->GetSize();
    for (Int_t i=0; i<size; i++) 
      {
	TFile *file = (TFile*) file_list->At(i);

	if(local_debug) {
	  std::cout << "QwEPICSEvent::WriteEPICSStringValue()"
		    << file->GetName()
		    << std::endl;
	}
	
	TTree *slow_tree = (TTree*) file->Get("slow");
	
	for (std::size_t tagindex=0; tagindex<fEPICSVariableList.size(); tagindex++) 
	  {
	    // only String 
	    if (fEPICSVariableType[tagindex] == kEPICSString) {
	      
	      TString name = fEPICSVariableList[tagindex];
	      name.ReplaceAll(':','_'); // remove colons before creating branch
	      TString name_type = name + "/C";\

	      Char_t epics_char[128];
	      TString epics_string;

	      TBranch *new_branch = slow_tree->Branch(name, epics_char, name_type);
	      
	      if (fEPICSDataEvent[tagindex].Filled) {
		epics_string = fEPICSDataEvent[tagindex].StringValue;
	      } 
	      else {
		epics_string = "empty";
	      }

	      if(local_debug) {
		std::cout << "QwEPICSEvent::WriteEPICSStringValue()\n";
		std::cout << name << " " << epics_string << std::endl;
		std::cout <<"\n";
	      }

	      sprintf(epics_char, "%s", epics_string.Data());
	      new_branch->Fill();
	    }
	    
	  }

	file -> Write("", TObject::kOverwrite);
      }
  }

  QwDebug << "Leaving QwEPICSEvent::WriteEPICSStringValues() normally"  << QwLog::endl;

  return;
  
}


Int_t QwEPICSEvent::DetermineIHWPPolarity() const{
  Int_t ihwppolarity = 0;
  if (GetDataString("IGL1I00OD16_16")=="OUT"
      || GetDataString("IGL1I00OD16_16")=="0"){
    ihwppolarity = 1;
  } else if (GetDataString("IGL1I00OD16_16")=="IN"
       || GetDataString("IGL1I00OD16_16")=="1"){
    ihwppolarity = -1;
  } else {
    QwWarning << "IHWP state is not well defined: '"
        << GetDataString("IGL1I00OD16_16") << "'"
        << QwLog::endl;
    if (GetDataString("IGL1I00DI24_24M")=="OUT"
        || GetDataString("IGL1I00DI24_24M")=="1"){
      ihwppolarity = 1;
    } else if (GetDataString("IGL1I00DI24_24M")=="IN"
        || GetDataString("IGL1I00DI24_24M")=="0"){
      ihwppolarity = -1;
    } else {
      QwWarning << "IHWP state is not well defined: '"
        << GetDataString("IGL1I00DI24_24M") << "'"
        << QwLog::endl;
    }
  }
  QwDebug << "QwEPICSEvent::DetermineIHWPPolarity: "
	  << "raw IHWP polarity is: "
	  << ihwppolarity << QwLog::endl;
  ihwppolarity = fExtraHelicityReversal * ihwppolarity;
  QwDebug << "QwEPICSEvent::DetermineIHWPPolarity: "
	  << "IHWP polarity after extra reversal is: "
	  << ihwppolarity << QwLog::endl;
  return ihwppolarity;
}

EQwWienMode QwEPICSEvent::DetermineWienMode() const{
  EQwWienMode wienmode = kWienIndeterminate;
  
  Double_t launchangle = 0.0;

  Double_t vwienangle = GetDataValue("VWienAngle");
  Double_t phiangle   = GetDataValue("Phi_FG");
  Double_t hwienangle = GetDataValue("HWienAngle"); 
  Double_t hoffset = 0.0;
  if (fabs(vwienangle)<10.0 && fabs(phiangle)<10.0){
    hoffset = 0.0;
  } else if (fabs(vwienangle)>80.0 && fabs(phiangle)>80.0
	     && fabs(vwienangle+phiangle)<10. ){
    hoffset = -90.0;
  } else if (fabs(vwienangle)>80.0 && fabs(phiangle)>80.0
	     && fabs(vwienangle+phiangle)>170. ){
    hoffset = +90.0;
  } else if (fabs(vwienangle)>80.0 && fabs(phiangle)<10.0) {
    wienmode = kWienVertTrans;
  } 
  if (wienmode == kWienIndeterminate){
    launchangle = hoffset+hwienangle;
    Double_t long_proj = 
      cos((launchangle-fNominalWienAngle)*TMath::DegToRad());
    if (long_proj > 0.5){
      wienmode = kWienForward;
    } else if (long_proj < -0.5){
      wienmode = kWienBackward;
    } else if (fabs(long_proj)<0.25){
      wienmode = kWienHorizTrans;
    }
  }
  return wienmode;
}

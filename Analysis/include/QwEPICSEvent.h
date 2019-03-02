/*
Adopted from G0EPICSEvent class.
*/

#ifndef __QWEPICSEVENT__
#define __QWEPICSEVENT__

// System headers
#include <map>
#include <vector>
#include <string>
using std::string;

// ROOT headers
#include "Rtypes.h"
#include "TString.h"
#include "TTree.h"

// Qweak headers
#include "QwOptions.h"

// Forward declarations
class QwParityDB;


class QwEPICSEvent
{
 public:

  /// EPICS data types
  enum EQwEPICSDataType {kEPICSString, kEPICSFloat, kEPICSInt};


  /// Default constructor
  QwEPICSEvent();
  /// Virtual destructor
  virtual ~QwEPICSEvent();


  /// \brief Define the configuration options
  static void DefineOptions(QwOptions &options);
  /// \brief Process the configuration options
  void ProcessOptions(QwOptions &options);


  // Add a tag to the list
  Int_t AddEPICSTag(const string& tag, const string& table = "",
      EQwEPICSDataType datatype = kEPICSFloat);

  Int_t LoadChannelMap(TString mapfile);

  std::vector<Double_t> ReportAutogains(std::vector<std::string> tag_list = fDefaultAutogainList);

  void ExtractEPICSValues(const string& data, int event);

  /// Find the index of an EPICS variable, or return error
  Int_t FindIndex(const string& tag) const;

  Double_t GetDataValue(const string& tag) const;      // get recent value corr. to tag
  TString  GetDataString(const string& tag) const;


  int SetDataValue(const string& tag, const double value, const int event);
  int SetDataValue(const string& tag, const string& value, const int event);
  int SetDataValue(int index, const double value, const int event);
  int SetDataValue(int index, const string& value, const int event);

  Bool_t HasDataLoaded() const { return fIsDataLoaded; };

  Int_t DetermineIHWPPolarity() const;
  EQwWienMode DetermineWienMode() const;

  void  CalculateRunningValues();

  void  PrintAverages() const;
  void  PrintVariableList() const;
  // void DefineEPICSVariables();
  void  ReportEPICSData() const;

  void  ResetCounters();

  void FillDB(QwParityDB *db);
  void FillSlowControlsData(QwParityDB *db);
  void FillSlowControlsStrigs(QwParityDB *db);
  void FillSlowControlsSettings(QwParityDB *db);


 private:

  // Tree array indices
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  // Flag to indicate that the event contains data
  Bool_t fIsDataLoaded;
  void SetDataLoaded(Bool_t flag) { fIsDataLoaded = flag; };

 public:

  /// \brief Construct the branch and tree vector
  void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values);
  /// \brief Fill the tree vector
  void FillTreeVector(std::vector<Double_t>& values) const;


 public:

  static std::vector<std::string> GetDefaultAutogainList() { return fDefaultAutogainList; };
  static void SetDefaultAutogainList(std::vector<std::string>& input_list);

  void WriteEPICSStringValues();
  
 private:

  /// Default autogain list
  static std::vector<std::string> fDefaultAutogainList;
  /// Initialize the default autogain list
  static void InitDefaultAutogainList();


 private:

  static const int kDebug;
  static const int kEPICS_Error;
  static const int kEPICS_OK;
  static const Double_t kInvalidEPICSData;
  // Int_t maxsize = 300;

  // Flag to disable database accesses for EPICS events
  bool fDisableDatabase;

  // Test whether the string is a number string or not
  Bool_t IsNumber(const string& word) {
    size_t pos;
    pos = word.find_first_not_of("0123456789.+-eE"); // white space not allowed
    if (pos != std::string::npos) return kFALSE;
    else return kTRUE;
  }

  struct EPICSVariableRecord {   //One EPICS variable record.
    Int_t     EventNumber;
    Bool_t    Filled;
    Double_t  Value;
    TString   StringValue;
  };
  std::vector<EPICSVariableRecord> fEPICSDataEvent;


  /*  The next two variables will contain the running sum and sum  *
   *  of squares for use in calculating the average and variance.  */
  struct EPICSCumulativeRecord {
    Bool_t   Filled;
    Int_t    NumberRecords;
    Double_t Sum;
    Double_t SquaredSum;
    Double_t Minimum;
    Double_t Maximum;
    TString  SavedString;
  };
  std::vector<EPICSCumulativeRecord> fEPICSCumulativeData;


  Int_t fNumberEPICSEvents;     // Number of EPICS events in the run
  Int_t fNumberEPICSVariables;  // Number of defined EPICS variables
  std::vector<std::string> fEPICSVariableList;  // List of defined EPICS variables
  std::vector<std::string> fEPICSTableList;     // List of DB tables to write
  std::vector<EQwEPICSDataType> fEPICSVariableType;

  std::map<std::string,Int_t> fEPICSVariableMap;

  TList *GetEPICSStringValues();

  Bool_t fBlinderReversalForRunTwo;
  Bool_t fPrecessionReversal;

  Double_t fNominalWienAngle;
  Int_t    fExtraHelicityReversal;

  
}; // class QwEPICSEvent

#endif // __QWEPICSEVENT__

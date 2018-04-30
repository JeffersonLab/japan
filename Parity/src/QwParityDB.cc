/*!
 * \file   QwParityDB.cc
 * \brief  A class for handling connections to the Qweak database.
 *
 * \author Damon Spayde
 * \date   2010-01-07
 */


#define EXPAND_QWPARITYSSQLS_STATICS
//#define MYSQLPP_SSQLS_NO_STATICS
#include "QwParitySSQLS.h"
#include "QwParityDB.h"

// System headers

// Qweak headers
#include "QwEventBuffer.h"
#include "QwRunCondition.h"
using namespace QwParitySSQLS;

// Specific template instantiations
template void QwDBInterface::AddThisEntryToList<QwParitySSQLS::md_data>(std::vector<QwParitySSQLS::md_data> &list);
template void QwDBInterface::AddThisEntryToList<QwParitySSQLS::lumi_data>(std::vector<QwParitySSQLS::lumi_data> &list);
template void QwDBInterface::AddThisEntryToList<QwParitySSQLS::beam>(std::vector<QwParitySSQLS::beam> &list);

// Definition of static class members in QwParityDB
std::map<string, unsigned int> QwParityDB::fMonitorIDs;
std::map<string, unsigned int> QwParityDB::fMainDetectorIDs;
std::map<string, unsigned int> QwParityDB::fLumiDetectorIDs;
std::vector<string>            QwParityDB::fMeasurementIDs;
std::map<string, unsigned int> QwParityDB::fSlowControlDetectorIDs;// for epics
std::map<string, unsigned char> QwParityDB::fErrorCodeIDs;

class StoreMonitorID {
  public:
    void operator() (QwParitySSQLS::monitor elem) {
      QwDebug << "StoreMonitorID:  monitor_id = " << elem.monitor_id << " quantity = " << elem.quantity << QwLog::endl;
      QwParityDB::fMonitorIDs.insert(std::make_pair(elem.quantity, elem.monitor_id));
    }
};

class StoreMainDetectorID {
  public:
    void operator() (QwParitySSQLS::main_detector elem) {
      QwDebug << "StoreMainDetectorID:  main_detector_id = " << elem.main_detector_id << " quantity = " << elem.quantity << QwLog::endl;
      QwParityDB::fMainDetectorIDs.insert(std::make_pair(elem.quantity, elem.main_detector_id));
    }
};

class StoreLumiDetectorID {
  public:
    void operator() (QwParitySSQLS::lumi_detector elem) {
      QwDebug << "StoreLumiDetectorID:  lumi_detector_id = " << elem.lumi_detector_id << " quantity = " << elem.quantity << QwLog::endl;
      QwParityDB::fLumiDetectorIDs.insert(std::make_pair(elem.quantity, elem.lumi_detector_id));
    }
};

class StoreMeasurementID {
 public:
  void operator() (QwParitySSQLS::measurement_type elem) {
      QwDebug << "StoreMeasurementID:  measurement_type = " << elem.measurement_type_id << QwLog::endl;
      QwParityDB::fMeasurementIDs.push_back( (std::string) elem.measurement_type_id );
    }
};


class StoreSlowControlDetectorID {
  public:
    void operator() (QwParitySSQLS::sc_detector elem) {
      QwDebug << "StoreSlowControlDetectorID: sc_detector_id = " << elem.sc_detector_id << " name = " << elem.name << QwLog::endl;
      QwParityDB::fSlowControlDetectorIDs.insert(std::make_pair(elem.name, elem.sc_detector_id));
    }
};

class StoreErrorCodeID {
  public:
    void operator() (QwParitySSQLS::error_code elem) {
      QwDebug << "StoreErrorCodeID: error_code_id = " << elem.error_code_id << " quantity = " << elem.quantity << QwLog::endl;
      QwParityDB::fErrorCodeIDs.insert(std::make_pair(elem.quantity, elem.error_code_id));
    }
};


/*! The simple constructor initializes member fields.  This class is not
 * used to establish the database connection.  It sets up a
 * mysqlpp::Connection() object that has exception throwing disabled.
 */
//QwDatabase::QwDatabase() : Connection(false)
QwParityDB::QwParityDB() : QwDatabase("01", "04", "0000")
{
  QwDebug << "Greetings from QwParityDB simple constructor." << QwLog::endl;
  // Initialize member fields

  fRunNumber         = 0;
  fRunID             = 0;
  fRunletID          = 0;
  fAnalysisID        = 0;
  fSegmentNumber     = -1;
  fDisableAnalysisCheck = false;
  
}

/*! The constructor initializes member fields using the values in
 *  the QwOptions object.
 * @param options  The QwOptions object.
 */
QwParityDB::QwParityDB(QwOptions &options) : QwDatabase(options, "01", "04", "0000")
{
  QwDebug << "Greetings from QwParityDB extended constructor." << QwLog::endl;

  // Initialize member fields
  fRunNumber         = 0;
  fRunID             = 0;
  fRunletID          = 0;
  fAnalysisID        = 0;
  fSegmentNumber     = -1;
  fDisableAnalysisCheck = false;
  
  ProcessAdditionalOptions(options);

}

/*! The destructor says "Good-bye World!"
 */
QwParityDB::~QwParityDB()
{
  QwDebug << "QwParityDB::~QwParityDB() : Good-bye World from QwParityDB destructor!" << QwLog::endl;
  if( Connected() ) Disconnect();
}

/*!
 * Sets run number for subsequent database interactions.  Makes sure correct
 * entry exists in run table and retrieves run_id.
 */
void QwParityDB::SetupOneRun(QwEventBuffer& qwevt)
{
  if (this->AllowsReadAccess()) {
    UInt_t run_id      = this->GetRunID(qwevt);
    UInt_t runlet_id   = this->GetRunletID(qwevt);
    UInt_t analysis_id = this->GetAnalysisID(qwevt);
    
    //  Write from the datebase
    QwMessage << "QwParityDB::SetupOneRun::"
	      << " Run Number "  << QwColor(Qw::kBoldMagenta) << qwevt.GetRunNumber() << QwColor(Qw::kNormal)
	      << " Run ID "      << QwColor(Qw::kBoldMagenta) << run_id << QwColor(Qw::kNormal)
	      << " Runlet ID "   << QwColor(Qw::kBoldMagenta) << runlet_id << QwColor(Qw::kNormal)
	      << " Analysis ID " << QwColor(Qw::kBoldMagenta) << analysis_id 
	      << QwColor(Qw::kNormal)
	      << QwLog::endl;
  }
}

/*!
 * Sets run number for subsequent database interactions.  Makes sure correct
 * entry exists in run table and retrieves run_id.
 */
bool QwParityDB::SetRunNumber(const UInt_t runnum)
{

  QwDebug << "Made it into QwParityDB::SetRunNumber()" << QwLog::endl;

  try {

    this->Connect();

    mysqlpp::Query query= this->Query();
    query << "SELECT * FROM run WHERE run_number = " << runnum;
    std::vector<run> res;
    query.storein(res);

    QwDebug << "Number of rows returned:  " << res.size() << QwLog::endl;

    if (res.size()!=1) {
      QwError << "Unable to find unique run number " << runnum << " in database." << QwLog::endl;
      QwError << "Run number query returned " << res.size() << "rows." << QwLog::endl;
      QwError << "Please make sure that the database contains one unique entry for this run." << QwLog::endl;
      return false;
    }

    QwDebug << "Run ID = " << res.at(0).run_id << QwLog::endl;

    fRunNumber = runnum;
    fRunID = res.at(0).run_id;

    this->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    return false;
  }

  return true;

}

/*!
 * This function sets the fRunID for the run being replayed as determined by the QwEventBuffer class.
 */
UInt_t QwParityDB::SetRunID(QwEventBuffer& qwevt)
{

  // Check to see if run is already in database.  If so retrieve run ID and exit.
  try
    {
      this->Connect();
      mysqlpp::Query query = this->Query();

      query << "SELECT * FROM run WHERE run_number = " << qwevt.GetRunNumber();
      std::vector<run> res;
      query.storein(res);
      QwDebug << "QwParityDB::SetRunID => Number of rows returned:  " << res.size() << QwLog::endl;

      // If there is more than one run in the DB with the same run number, then there will be trouble later on.  Catch and bomb out.
      if (res.size()>1)
	{
	  QwError << "Unable to find unique run number " << qwevt.GetRunNumber() << " in database." << QwLog::endl;
	  QwError << "Run number query returned " << res.size() << "rows." << QwLog::endl;
	  QwError << "Please make sure that the database contains one unique entry for this run." << QwLog::endl;
	  this->Disconnect();
	  return 0;
	}

      // Run already exists in database.  Pull run_id and move along.
      if (res.size()==1)
	{
	  QwDebug << "QwParityDB::SetRunID => Run ID = " << res.at(0).run_id << QwLog::endl;

	  fRunNumber = qwevt.GetRunNumber();
	  fRunID     = res.at(0).run_id;
	  this->Disconnect();
	  return fRunID;
	}
      this->Disconnect();
    }
  catch (const mysqlpp::Exception& er)
    {

      QwError << er.what() << QwLog::endl;
      this->Disconnect();
      return 0;
    }

  // Run is not in database so insert pertinent data and retrieve run ID
  // Right now this does not insert start/stop times or info on number of events.
  try
    {

      this->Connect();
      run row(0);
      row.run_number      = qwevt.GetRunNumber();
      row.run_type        = "good"; // qwevt.GetRunType(); RunType is the confused name because we have also a CODA run type.
      row.start_time      = mysqlpp::DateTime(qwevt.GetStartUnixTime());
      row.end_time        = mysqlpp::DateTime(qwevt.GetEndUnixTime());
      row.n_mps = 0;
      row.n_qrt	= 0;
      // Set following quantities to 9999 as "uninitialized value".  DTS 8/3/2012
      row.slug = 9999;
      row.wien_slug = 9999;
      row.injector_slug = 9999;
//    row.n_mps=10; // This works
      //    row.start_time = mysqlpp::null; // This works
      //    row.start_time = qwevt.GetStartSQLTime().Data(); // This does not work
        mysqlpp::Query query=this->Query();
        query.insert(row);
       QwDebug<< "QwParityDB::SetRunID() => Run Insert Query = " << query.str() << QwLog::endl;

      query.execute();

      if (query.insert_id()!=0)
	{
	  fRunNumber = qwevt.GetRunNumber();
	  fRunID     = query.insert_id();
	}
      this->Disconnect();
      return fRunID;
    }
  catch (const mysqlpp::Exception& er)
    {
      QwError << er.what() << QwLog::endl;
      this->Disconnect();
      return 0;
    }

}

/*!
 * This is a getter for run_id in the run table.  Should be used in subsequent queries to retain key relationships between tables.
 */
UInt_t QwParityDB::GetRunID(QwEventBuffer& qwevt)
{
  // If the stored run number does not agree with the CODA run number
  // or if fRunID is not set, then retrieve data from database and update if necessary.

  // GetRunNumber() in QwEventBuffer returns Int_t, thus
  // we should convert it to UInt_t here. I think, it is OK.

  if (fRunID == 0 || fRunNumber != (UInt_t) qwevt.GetRunNumber() ) {
     QwDebug << "QwParityDB::GetRunID() set fRunID to " << SetRunID(qwevt) << QwLog::endl;
     fRunletID = 0;
     fAnalysisID = 0;
  }

  return fRunID;

}

/*!
 * This function sets the fRunletID for the run being replayed as determined by the QwEventBuffer class.
 *
 * Runlets are differentiated by file segment number at the moment, not by event range or start/stop time.  This function will need to be altered if we opt to differentiate between runlets in a different way.
 */
UInt_t QwParityDB::SetRunletID(QwEventBuffer& qwevt)
{

  // Make sure 'run' table has been populated and retrieve run_id
  //  UInt_t runid = this->GetRunID(qwevt);

  // Check to see if runlet is already in database.  If so retrieve runlet_id and exit.
  try
    {
      this->Connect();
      mysqlpp::Query query = this->Query();

      // Query is slightly different if file segments are being chained together for replay or not.
      if (qwevt.AreRunletsSplit()) {
        fSegmentNumber = qwevt.GetSegmentNumber();
        query << "SELECT * FROM runlet WHERE run_id = " << fRunID << " AND full_run = 'false' AND segment_number = " << fSegmentNumber;
      } else {
	query << "SELECT * FROM runlet WHERE run_id = " << fRunID << " AND full_run = 'true'";
      }

      std::vector<runlet> res;
      query.storein(res);
      QwDebug << "QwParityDB::SetRunletID => Number of rows returned:  " << res.size() << QwLog::endl;

      // If there is more than one run in the DB with the same runlet number, then there will be trouble later on.  Catch and bomb out.
      if (res.size()>1)
	{
	  QwError << "Unable to find unique runlet number " << qwevt.GetRunNumber() << " in database." << QwLog::endl;
	  QwError << "Run number query returned " << res.size() << "rows." << QwLog::endl;
	  QwError << "Please make sure that the database contains one unique entry for this run." << QwLog::endl;
	  this->Disconnect();
	  return 0;
	}

      // Run already exists in database.  Pull runlet_id and move along.
      if (res.size()==1)
	{
	  QwDebug << "QwParityDB::SetRunletID => Runlet ID = " << res.at(0).runlet_id << QwLog::endl;

	  fRunletID     = res.at(0).runlet_id;
	  this->Disconnect();
	  return fRunletID;
	}
      this->Disconnect();
    }
  catch (const mysqlpp::Exception& er)
    {

      QwError << er.what() << QwLog::endl;
      this->Disconnect();
      return 0;
    }

  // Runlet is not in database so insert pertinent data and retrieve run ID
  // Right now this does not insert start/stop times or info on number of events.
  try
    {

      this->Connect();
      runlet row(0);
      row.run_id      = fRunID;
      row.run_number      = qwevt.GetRunNumber();
      row.start_time      = mysqlpp::null;
      row.end_time        = mysqlpp::null;
      row.first_mps = 0;
      row.last_mps	= 0;
      if (qwevt.AreRunletsSplit()) {
        row.segment_number = fSegmentNumber;
        row.full_run = "false";
      } else {
        row.segment_number  = mysqlpp::null;
        row.full_run = "true";
      }

        mysqlpp::Query query=this->Query();
        query.insert(row);
       QwDebug<< "QwParityDB::SetRunletID() => Run Insert Query = " << query.str() << QwLog::endl;

      query.execute();

      if (query.insert_id()!=0)
	{
	  fRunletID     = query.insert_id();
	}
      this->Disconnect();
      return fRunletID;
    }
  catch (const mysqlpp::Exception& er)
    {
      QwError << er.what() << QwLog::endl;
      this->Disconnect();
      return 0;
    }

}

/*!
 * This is a getter for runlet_id in the runlet table.  Should be used in subsequent queries to retain key relationships between tables.
 */
UInt_t QwParityDB::GetRunletID(QwEventBuffer& qwevt)
{
  // If the stored run number does not agree with the CODA run number
  // or if fRunID is not set, then retrieve data from database and update if necessary.

  if (fRunletID == 0 || (qwevt.AreRunletsSplit() && fSegmentNumber!=qwevt.GetSegmentNumber()) || fRunNumber != (UInt_t) qwevt.GetRunNumber() ) {
     QwDebug << "QwParityDB::GetRunletID() set fRunletID to " << SetRunletID(qwevt) << QwLog::endl;
     fAnalysisID = 0;
  }

  return fRunletID;

}

/*!
 * This is used to set the appropriate analysis_id for this run.  Must be a valid runlet_id in the runlet table before proceeding.  Will insert an entry into the analysis table if necessary.
 */
UInt_t QwParityDB::SetAnalysisID(QwEventBuffer& qwevt)
{

  // If there is already an analysis_id for this run, then let's bomb out.

  try {
    this->Connect();
    mysqlpp::Query query= this->Query();
    query << "SELECT analysis_id FROM analysis WHERE beam_mode=" << mysqlpp::quote << "nbm";
    query << " AND slope_calculation=" << mysqlpp::quote << "off";
    query << " AND slope_correction=" << mysqlpp::quote << "off";
    query << " AND runlet_id = " << mysqlpp::quote << this->GetRunletID(qwevt); 

    mysqlpp::StoreQueryResult res = query.store();

    if (res.num_rows() != 0) {
      QwError << "This runlet has already been analyzed by the engine!" << QwLog::endl;
      QwError << "The following analysis_id values already exist in the database:  ";
      for (size_t i=0; i<res.num_rows(); i++) {
        QwError << res[i][0] << " ";
      }
      QwError << QwLog::endl;

      if (fDisableAnalysisCheck==false) {
        QwError << "Analysis of this run will now be terminated."  << QwLog::endl;

        return 0;
      } else {
        QwWarning << "Analysis will continue.  A duplicate entry with new analysis_id will be added to the analysis table." << QwLog::endl;
      }
    }

    this->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    QwError << "Unable to determine if there are other database entries for this run.  Exiting." << QwLog::endl;
    this->Disconnect();
    return 0;
  }


  try {

    analysis analysis_row(0);

    analysis_row.runlet_id  = GetRunletID(qwevt);
    analysis_row.seed_id = 1;

    std::pair<UInt_t, UInt_t> event_range;
    event_range = qwevt.GetEventRange();

    analysis_row.time        = mysqlpp::DateTime::now();
    analysis_row.bf_checksum = "empty"; // we will match this as a real one later
    analysis_row.beam_mode   = "nbm";   // we will match this as a real one later
    analysis_row.n_mps       = 0;       // we will match this as a real one later
    analysis_row.n_qrt       = 4;       // we will match this as a real one later
    analysis_row.first_event = event_range.first;
    analysis_row.last_event  = event_range.second;
    analysis_row.segment     = 0;       // we will match this as a real one later
    analysis_row.slope_calculation = "off";  // we will match this as a real one later
    analysis_row.slope_correction  = "off"; // we will match this as a real one later

    // Analyzer Information Parsing 
    QwRunCondition run_condition(
      gQwOptions.GetArgc(),
      gQwOptions.GetArgv(),
      "run_condition"
    );

    run_condition.Get()->Print();

    TIter next(run_condition.Get());
    TObjString *obj_str;
    TString str_val, str_var;
    Ssiz_t location;

    // Iterate over each entry in run_condition
    while ((obj_str = (TObjString *) next())) {
      QwMessage << obj_str->GetName() << QwLog::endl; 

      // Store string contents for parsing
      str_var = str_val = obj_str->GetString();
      location = str_val.First(":"); // The first : separates variable from value
      location = location + 2; // Value text starts two characters after :
      str_val.Remove(0,location); //str_val stores value to go in DB

      // Decision tree to figure out which variable to store in
      if (str_var.BeginsWith("ROOT Version")) { 
        analysis_row.root_version = str_val;
      } else if (str_var.BeginsWith("ROOT file creating time")) {
        analysis_row.root_file_time = str_val;
      } else if (str_var.BeginsWith("ROOT file created on Hostname")) {
        analysis_row.root_file_host = str_val;
      } else if (str_var.BeginsWith("ROOT file created by the user")) {
        analysis_row.root_file_user = str_val;
      } else if (str_var.BeginsWith("QwAnalyzer Name")) {
        analysis_row.analyzer_name = str_val;
      } else if (str_var.BeginsWith("QwAnalyzer Options")) {
        analysis_row.analyzer_argv = str_val;
      } else if (str_var.BeginsWith("QwAnalyzer SVN Revision")) {
        analysis_row.analyzer_svn_rev = str_val;
      } else if (str_var.BeginsWith("QwAnalyzer SVN Last Changed Revision")) {
        analysis_row.analyzer_svn_lc_rev = str_val;
      } else if (str_var.BeginsWith("QwAnalyzer SVN URL")) {
        analysis_row.analyzer_svn_url = str_val;
      } else if (str_var.BeginsWith("DAQ ROC flags when QwAnalyzer runs")) {
        analysis_row.roc_flags = str_val;
      } else {
      }
    }
    this->Connect();
    mysqlpp::Query query= this->Query();
    query.insert(analysis_row);
    //QwMessage << "\nQwParityDB::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl<< QwLog::endl;
    query.execute();

    if (query.insert_id()!=0)
      {
	fAnalysisID = query.insert_id();
      }

    this->Disconnect();
    return fAnalysisID;
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    this->Disconnect();
    return 0;
  }


}

void QwParityDB::FillParameterFiles(QwSubsystemArrayParity& subsys){
  TList* param_file_list = subsys.GetParamFileNameList("mapfiles");
  try {
    this->Connect();
    mysqlpp::Query query = this->Query();
    parameter_files parameter_file_row(0);
    parameter_file_row.analysis_id = GetAnalysisID();

    param_file_list->Print();
    TIter next(param_file_list);
    TList *pfl_elem;
    while ((pfl_elem = (TList *) next())) {
      parameter_file_row.filename = pfl_elem->GetName();
      query.insert(parameter_file_row);
      query.execute();
    }

    this->Disconnect();

    delete param_file_list;
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    this->Disconnect();
    delete param_file_list;
  }
}

/*!
 * This is a getter for analysis_id in the analysis table.  Required by all queries on cerenkov, beam, etc. tables.  Will return 0 if fRunID has not been successfully set.  If fAnalysisID is not set, then calls code to insert into analysis table and retrieve analysis_id.
 */
UInt_t QwParityDB::GetAnalysisID(QwEventBuffer& qwevt)
{
  // Sanity check to make sure not calling this before runlet_id has been retrieved.
  if (fRunletID == 0) {
    QwDebug << "QwParityDB::GetAnalysisID() : fRunletID must be set before proceeding.  Check to make sure run exists in database." << QwLog::endl;
    return 0;
  }

  if (fAnalysisID == 0 || fRunNumber != (UInt_t) qwevt.GetRunNumber()
      || (qwevt.AreRunletsSplit() && fSegmentNumber!=qwevt.GetSegmentNumber())) {
    QwDebug << "QwParityDB::GetAnalysisID() set fAnalysisID to " << SetAnalysisID(qwevt) << QwLog::endl;
    if (fAnalysisID==0) {
      QwError << "QwParityDB::SetAnalysisID() unable to set valid fAnalysisID for this run.  Exiting." <<QwLog::endl;
      exit(1);
    }
  }

  return fAnalysisID;
}


/*
 * This function retrieves the monitor table key 'monitor_id' for a given beam monitor.
 */
UInt_t QwParityDB::GetMonitorID(const string& name, Bool_t zero_id_is_error)
{
  if (fMonitorIDs.size() == 0) {
    StoreMonitorIDs();
  }

  UInt_t monitor_id = fMonitorIDs[name];

  if (zero_id_is_error && monitor_id==0) {
    //    monitor_id = 6; // only for QwMockDataAnalysis
    QwError << "QwParityDB::GetMonitorID() => Unable to determine valid ID for beam monitor " << name << QwLog::endl;
  }

  return monitor_id;

}

/*
 * Stores monitor table keys in an associative array indexed by monitor name.
 */
void QwParityDB::StoreMonitorIDs()
{
  try {

    this->Connect();
    mysqlpp::Query query=this->Query();
    query.for_each(monitor(), StoreMonitorID());

//    QwDebug<< "QwParityDB::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;
    this->Disconnect();
  }

  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
}

/*
 * This function retrieves the main_detector table key 'main_detector_id' for a given beam main_detector.
 */
UInt_t QwParityDB::GetMainDetectorID(const string& name, Bool_t zero_id_is_error)
{
  if (fMainDetectorIDs.size() == 0) {
    StoreMainDetectorIDs();
  }

  UInt_t main_detector_id = fMainDetectorIDs[name];

  if (zero_id_is_error && main_detector_id==0) {
    //    main_detector_id = 19; // only for QwMockDataAnalysis
    QwError << "QwParityDB::GetMainDetectorID() => Unable to determine valid ID for beam main_detector " << name << QwLog::endl;
  }

  return main_detector_id;

}


/*
 * Stores main_detector table keys in an associative array indexed by main_detector name.
 */
void QwParityDB::StoreMainDetectorIDs()
{

  try {
    this->Connect();
    mysqlpp::Query query=this->Query();
    query.for_each(main_detector(), StoreMainDetectorID());

//    QwDebug<< "QwParityDB::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    this->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
}


/*
 * This function retrieves the slow control detector table key 'sc_detector_id' for a given epics variable.
 */
UInt_t QwParityDB::GetSlowControlDetectorID(const string& name)
{
  if (fSlowControlDetectorIDs.size() == 0) {
    StoreSlowControlDetectorIDs();
  }

  UInt_t sc_detector_id = fSlowControlDetectorIDs[name];

  if (sc_detector_id==0) {
    QwError << "QwParityDB::GetSlowControlDetectorID() => Unable to determine valid ID for the epics variable " << name << QwLog::endl;
  }

  return sc_detector_id;

}

/*
 * This function retrieves the error code table key 'error_code_id' for a given error code name.
 */
UInt_t QwParityDB::GetErrorCodeID(const string& name)
{
  if (fErrorCodeIDs.size() == 0) {
    StoreErrorCodeIDs();
  }

  UInt_t error_code_id = fErrorCodeIDs[name];

  if (error_code_id==0) {
    QwError << "QwParityDB::GetErrorCodeID() => Unable to determine valid ID for the error code " << name << QwLog::endl;
  }

  return error_code_id;

}

/*
 * Stores slow control detector table keys in an associative array indexed by slow_controls_data name.
 */
void QwParityDB::StoreSlowControlDetectorIDs()
{

  try {
    this->Connect();
    mysqlpp::Query query=this->Query();
    query.for_each(sc_detector(), StoreSlowControlDetectorID());

//    QwDebug<< "QwParityDB::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    this->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
}

/*
 * Stores error_code table keys in an associative array indexed by error_code quantity.
 */
void QwParityDB::StoreErrorCodeIDs()
{

  try {
    this->Connect();
    mysqlpp::Query query=this->Query();
    query.for_each(error_code(), StoreErrorCodeID());

//    QwDebug<< "QwParityDB::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    this->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
}

/*
 * This function retrieves the lumi_detector table key 'lumi_detector_id' for a given beam lumi_detector.
 */
UInt_t QwParityDB::GetLumiDetectorID(const string& name, Bool_t zero_id_is_error)
{
  if (fLumiDetectorIDs.size() == 0) {
    StoreLumiDetectorIDs();
  }

  UInt_t lumi_detector_id = fLumiDetectorIDs[name];

  if (zero_id_is_error && lumi_detector_id==0) {
     QwError << "QwParityDB::GetLumiDetectorID() => Unable to determine valid ID for beam lumi_detector " << name << QwLog::endl;
  }

  return lumi_detector_id;
}

/*
 * Stores lumi_detector table keys in an associative array indexed by lumi_detector name.
 */
void QwParityDB::StoreLumiDetectorIDs()
{

  try {
    this->Connect();

    mysqlpp::Query query=this->Query();
    query.for_each(lumi_detector(), StoreLumiDetectorID());

//    QwDebug<< "QwParityDB::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    this->Disconnect();

  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
}




const string QwParityDB::GetMeasurementID(const Int_t index)
{
  if (fMeasurementIDs.size() == 0) {
    StoreMeasurementIDs();
  }

  string measurement_type = fMeasurementIDs[index];

  if (measurement_type.empty()) {
    QwError << "QwParityDB::GetMeasurementID() => Unable to determine valid ID for measurement type with " << index << QwLog::endl;
  }

  return measurement_type;
}

void QwParityDB::StoreMeasurementIDs()
{

  try {
    this->Connect();

    mysqlpp::Query query=this->Query();
    query.for_each(measurement_type(), StoreMeasurementID());

    // QwDebug<< "QwParityDB::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    this->Disconnect();

  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
}

/*!
 * Defines configuration options for QwParityDB class using QwOptions
 * functionality.
 *
 * Should apparently by called by QwOptions::DefineOptions() in
 * QwParityOptions.h
 */
void QwParityDB::DefineAdditionalOptions(QwOptions& options)
{
  // Specify command line options for use by QwParityDB
  options.AddOptions("Parity Analyzer Database options")
    ("QwParityDB.disable-analysis-check", 
     po::value<bool>()->default_bool_value(false),
     "disable check of pre-existing analysis_id");
}

/*!
 * Loads the configuration options for QwParityDB class into this instance of
 * QwParityDB from the QwOptions object.
 * @param options Options object
 */
void QwParityDB::ProcessAdditionalOptions(QwOptions &options)
{
  if (options.GetValue<bool>("QwParityDB.disable-analysis-check"))  
    fDisableAnalysisCheck=true;

  return;
}



/*!
 * \file   QwDatabase.cc
 * \brief  A class for handling connections to the Qweak database.
 *
 * \author Damon Spayde
 * \date   2010-01-07
 */


#include "QwDatabase.h"

// System headers

// Qweak headers

sql_create_6(db_schema, 1, 2 
  , mysqlpp::sql_int_unsigned , db_schema_id
  , mysqlpp::sql_char , major_release_number
  , mysqlpp::sql_char , minor_release_number
  , mysqlpp::sql_char , point_release_number
  , mysqlpp::sql_timestamp , time
  , mysqlpp::Null<mysqlpp::sql_text> , script_name
) 
 

/*! The simple constructor initializes member fields.  This class is not
 * used to establish the database connection.  It sets up a
 * mysqlpp::Connection() object that has exception throwing disabled.
 */
//QwDatabase::QwDatabase() : Connection(false)
QwDatabase::QwDatabase(const string &major, const string &minor, const string &point) : Connection(), kValidVersionMajor(major), kValidVersionMinor(minor), kValidVersionPoint(point)
{
  QwDebug << "Greetings from QwDatabase simple constructor." << QwLog::endl;

  // Initialize member fields
  fDatabase=fDBServer=fDBUsername=fDBPassword="";
  fVersionMajor=fVersionMinor=fVersionPoint="";

  fAccessLevel = kQwDatabaseOff;

  fDBPortNumber      = 0;
  fValidConnection   = false;

}

/*! The constructor initializes member fields using the values in
 *  the QwOptions object.
 * @param options  The QwOptions object.
 * @param major Major version number
 * @param minor Minor version number
 * @param point Point revision number
 */
QwDatabase::QwDatabase(QwOptions &options, const string &major, const string &minor, const string &point) : Connection(), kValidVersionMajor(major), kValidVersionMinor(minor), kValidVersionPoint(point)
{
  QwDebug << "Greetings from QwDatabase extended constructor." << QwLog::endl;

  // Initialize member fields
  fDatabase=fDBServer=fDBUsername=fDBPassword="";
  fVersionMajor=fVersionMinor=fVersionPoint="";

  fAccessLevel = kQwDatabaseOff;

  fDBPortNumber      = 0;
  fValidConnection   = false;

  ProcessOptions(options);

}

/*! The destructor says "Good-bye World!"
 */
QwDatabase::~QwDatabase()
{
  QwDebug << "QwDatabase::~QwDatabase() : Good-bye World from QwDatabase destructor!" << QwLog::endl;
  if( connected() ) Disconnect();
}

/*! This function is used to load the connection information for the
 * database.  It tests the connection to make sure it is valid and causes a
 * program exit if no valid connection can be formed.
 *
 * It is called the first time Connect() is called.
 */
Bool_t QwDatabase::ValidateConnection()
{
  QwDebug << "Entering QwDatabase::ValidateConnection()..." << QwLog::endl;

  // Bool_t status;
  //
  // Retrieve options if they haven't already been filled.
  //
  //   if (fDatabase.empty()){
  //     status = ProcessOptions(gQwOptions);
  //     if (!status) return status;
  //   }

  //  Check values.
  if (fAccessLevel!=kQwDatabaseOff){
    if (fDatabase.empty()){
      QwError << "QwDatabase::ValidateConnection() : No database supplied.  Unable to connect." << QwLog::endl;
      fValidConnection=false;
    }
    if (fDBUsername.empty()){
      QwError << "QwDatabase::ValidateConnection() : No database username supplied.  Unable to connect." << QwLog::endl;
      fValidConnection=false;
    }
    if (fDBPassword.empty()){
      QwError << "QwDatabase::ValidateConnection() : No database password supplied.  Unable to connect." << QwLog::endl;
      fValidConnection=false;
    }
    if (fDBServer.empty()){
      QwMessage << "QwDatabase::ValidateConnection() : No database server supplied.  Attempting localhost." << QwLog::endl;
      fDBServer = "localhost";
    }
    //
    // Try to connect with given information
    //
    try {
      connect(fDatabase.c_str(), fDBServer.c_str(), fDBUsername.c_str(),
	      fDBPassword.c_str(), (unsigned int) fDBPortNumber);
      //    connect(dbname.Data(), server.Data(), username.Data(), password.Data(), (unsigned int) port);
    } catch (std::exception const& e) {
      QwError << "QwDatabase::ValidateConnection() : " << QwLog::endl;
      QwError << e.what() << " while validating connection" << QwLog::endl;
      QwError << "Database name = " << fDatabase <<QwLog::endl;
      QwError << "Database server = " << fDBServer <<QwLog::endl;
      QwError << "Database username = " << fDBUsername <<QwLog::endl;
      QwError << "Database port = " << fDBPortNumber <<QwLog::endl;
      QwError << "Continuing without database." << QwLog::endl;
      QwWarning << "Might have left database connection dangling..." << QwLog::endl;
      fAccessLevel = kQwDatabaseOff;
      return kFALSE;
    }

    QwDebug << "QwDatabase::ValidateConnection() : Made it past connect() call."  << QwLog::endl;

    // Get database schema version information
    if (StoreDBVersion()) {
      fValidConnection=true;
      // Success!
      QwMessage << "QwDatabase::ValidateConnection() : Successfully connected to requested database." << QwLog::endl;
    } else {
      QwError << "QwDatabase::ValidateConnection() : Unsuccessfully connected to requested database." << QwLog::endl;
      // Connection was bad so clear the member variables
      fValidConnection=false;
      fDatabase.clear();
      fDBServer.clear();
      fDBUsername.clear();
      fDBPassword.clear();
      fDBPortNumber=0;
    }
    disconnect();
  }

  // Check to make sure database and QwDatabase schema versions match up.
  if (fAccessLevel==kQwDatabaseReadWrite && 
      (fVersionMajor != kValidVersionMajor ||
      fVersionMinor != kValidVersionMinor ||
      fVersionPoint < kValidVersionPoint)) {
    fValidConnection = false;
    QwError << "QwDatabase::ValidConnection() : Connected database schema inconsistent with current version of analyzer." << QwLog::endl;
    QwError << "  Database version is " << this->GetVersion() << QwLog::endl;
    QwError << "  Required database version is " << this->GetValidVersion() << QwLog::endl;
    QwError << "  Please connect to a database supporting the required schema version." << QwLog::endl;
    exit(1);
  }

  QwDebug << "QwDatabase::ValidateConnection() : Exiting successfully." << QwLog::endl;
  return fValidConnection;
}


/*! This function is used to initiate a database connection.
 */
bool QwDatabase::Connect()
{
  /* Open a connection to the database using the predefined parameters.
   * Must call QwDatabase::ConnectionInfo() first.
   */

  //  Return flase, if we're not using the DB.
  if (fAccessLevel==kQwDatabaseOff) return false;

  // Make sure not already connected
  if (connected()) return true;

  // If never connected before, then make sure connection parameters form
  // valid connection
  if (!fValidConnection) {
    ValidateConnection();
  }

  if (fValidConnection) {
    return connect(fDatabase.c_str(), fDBServer.c_str(), fDBUsername.c_str(), fDBPassword.c_str(), (unsigned int) fDBPortNumber);
    //   return connect(fDatabase.Data(), fDBServer.Data(), fDBUsername.Data(), fDBPassword.Data(), (unsigned int) fDBPortNumber);
  } else {
    QwError << "QwDatabase::Connect() : Must establish valid connection to database." << QwLog::endl;
    return false;
  }
}

/*!
 * Defines configuration options for QwDatabase class using QwOptions
 * functionality.
 *
 * Should apparently by called by QwOptions::DefineOptions() in
 * QwParityOptions.h
 */
void QwDatabase::DefineOptions(QwOptions& options)
{
  // Specify command line options for use by QwDatabase
  options.AddOptions("Database options")("QwDatabase.accesslevel", po::value<string>(), "database access level (OFF,RO,RW)");
  options.AddOptions("Database options")("QwDatabase.dbname", po::value<string>(), "database name");
  options.AddOptions("Database options")("QwDatabase.dbserver", po::value<string>(), "database server name");
  options.AddOptions("Database options")("QwDatabase.dbusername", po::value<string>(), "database username");
  options.AddOptions("Database options")("QwDatabase.dbpassword", po::value<string>(), "database password");
  options.AddOptions("Database options")("QwDatabase.dbport", po::value<int>()->default_value(0), "database server port number (defaults to standard mysql port)");
}

/*!
 * Loads the configuration options for QwDatabase class into this instance of
 * QwDatabase from the QwOptions object.
 * @param options Options object
 */
void QwDatabase::ProcessOptions(QwOptions &options)
{
  if (options.HasValue("QwDatabase.accesslevel")) {
    string access = options.GetValue<string>("QwDatabase.accesslevel");
    SetAccessLevel(access);
  }
  else {
    QwWarning << "QwDatabase::ProcessOptions : No access level specified; database access is OFF" << QwLog::endl;
    fAccessLevel = kQwDatabaseOff;
  }
  if (options.HasValue("QwDatabase.dbport")) {
    fDBPortNumber = options.GetValue<int>("QwDatabase.dbport");
  }
  if (options.HasValue("QwDatabase.dbname")) {
    fDatabase = options.GetValue<string>("QwDatabase.dbname");
  }
  if (options.HasValue("QwDatabase.dbusername")) {
    fDBUsername = options.GetValue<string>("QwDatabase.dbusername");
  }
  if (options.HasValue("QwDatabase.dbpassword")) {
    fDBPassword = options.GetValue<string>("QwDatabase.dbpassword");
  }
  if (options.HasValue("QwDatabase.dbserver")) {
    fDBServer = options.GetValue<string>("QwDatabase.dbserver");
  }

  return;
}

void QwDatabase::ProcessOptions(const TString& dbname, const TString& username, const TString& passwd, const TString& dbhost, const Int_t dbport, const TString& accesslevel)
{
  SetAccessLevel(static_cast<string>(accesslevel));
  fDatabase = dbname;
  fDBUsername = username;
  fDBPassword = passwd;
  fDBServer = dbhost;
  fDBPortNumber = dbport;
}

void QwDatabase::SetAccessLevel(string accesslevel)
{
  TString level = accesslevel.c_str();
  level.ToLower();
  if (level=="off")     fAccessLevel = kQwDatabaseOff;
  else if (level=="ro") fAccessLevel = kQwDatabaseReadOnly;
  else if (level=="rw") fAccessLevel = kQwDatabaseReadWrite;
  else{
    QwWarning << "QwDatabase::SetAccessLevel  : Unrecognized access level \""
	      << accesslevel << "\"; setting database access OFF"
	      << QwLog::endl;
    fAccessLevel = kQwDatabaseOff;
  }
  return;
}

/*!
 * This function prints the server information.
 */
void QwDatabase::PrintServerInfo()
{
  if (fValidConnection)
    {
      printf("\nQwDatabase MySQL ");
      printf("%s v%s %s -----------------\n", BOLD, GetServerVersion().c_str(), NORMAL);
      printf("Database server : %s%10s%s",    RED,  fDBServer.c_str(),   NORMAL);
      printf(" name   : %s%12s%s",            BLUE, fDatabase.c_str(),   NORMAL);
      printf(" user   : %s%6s%s",             RED,  fDBUsername.c_str(), NORMAL);
      printf(" port   : %s%6d%s\n",           BLUE, fDBPortNumber,       NORMAL);
      printf(" %s\n\n", server_status().c_str());
    }
  else
    {
      printf("There is no connection.\n");
    }

  return;
}


const string QwDatabase::GetVersion(){
  string version = fVersionMajor + "." + fVersionMinor + "." + fVersionPoint;
  return version;
}

const string QwDatabase::GetValidVersion(){
  string version = kValidVersionMajor + "." + kValidVersionMinor + "." + kValidVersionPoint;
  return version;
}

/*
 * Retrieves database schema version information from database.
 * Returns true if successful, false otherwise.
 */
bool QwDatabase::StoreDBVersion()
{
  try
    {
      mysqlpp::Query query = this->Query();

      query << "SELECT * FROM db_schema";
      std::vector<db_schema> res;
      query.storein(res);
      QwDebug << "QwDatabase::StoreDBVersion => Number of rows returned:  " << res.size() << QwLog::endl;

      // If there is more than one run in the DB with the same run number, then there will be trouble later on.  Catch and bomb out.
      if (res.size()>1)
	{
	  QwError << "Unable to find unique schema version in database." << QwLog::endl;
	  QwError << "Schema query returned " << res.size() << "rows." << QwLog::endl;
	  QwError << "Please make sure that db_schema contains one unique." << QwLog::endl;
	  this->Disconnect();
	  return false;
	}

      // Run already exists in database.  Pull run_id and move along.
      if (res.size()==1)
	{
	  QwDebug << "QwDatabase::StoreDBVersion => db_schema_id = " << res.at(0).db_schema_id << QwLog::endl;

	  fVersionMajor=res.at(0).major_release_number;
	  fVersionMinor=res.at(0).minor_release_number;
	  fVersionPoint=res.at(0).point_release_number;
	  this->Disconnect();
	}
    }
  catch (const mysqlpp::Exception& er)
    {
      QwError << er.what() << QwLog::endl;
      disconnect();
      return false;
    }

  return true;

}

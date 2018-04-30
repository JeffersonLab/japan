/*!
 * \file   QwDatabase.h
 * \brief  A class for handling connections to the Qweak database.
 *
 * \author Damon Spayde
 * \date   2010-01-07
 */

#ifndef QWDATABASE_HH
#define QWDATABASE_HH

// System headers
#include <map>
#include <vector>
#include <string>
#include <typeinfo>

// Third Party Headers
#include <mysql++.h>
#include "ssqls.h"


// ROOT headers
#include "TString.h"

// Qweak headers
#include "QwTypes.h"
#include "QwLog.h"
#include "QwColor.h"
#include "QwOptions.h"

// Forward declarations

/**
 *  \class QwDatabase
 *  \ingroup QwAnalysis
 *  \brief A database interface class
 *
 * This class provides the connection to the Qweak database to other objects
 * in the Qweak analyzer.  A static global object gQwDatabase is used to
 * provide these services.
 *
 */

class QwDatabase: private mysqlpp::Connection {
  public:

    QwDatabase(const string &major = "00", const string &minor = "00", const string &point = "0000"); //!< Simple constructor
    QwDatabase(QwOptions &options, const string &major = "00", const string &minor = "00", const string &point = "0000"); //!< Constructor with QwOptions object

    virtual ~QwDatabase(); //!< Destructor

    void         SetAccessLevel(string accesslevel);  //!< Sets the access level flag based on string labels:  "off", "ro", "rw".

    Bool_t       AllowsReadAccess(){return (fAccessLevel==kQwDatabaseReadOnly || fAccessLevel==kQwDatabaseReadWrite);};
    Bool_t       AllowsWriteAccess(){return (fAccessLevel==kQwDatabaseReadWrite);};

    Bool_t       Connect();                    //!< Open a connection to the database using the predefined parameters.
    void         Disconnect() {disconnect();}; //<! Close an open database connection
    Bool_t       Connected() { return connected(); }
    const string GetServerVersion() {return server_version();}; //<! Get database server version
    static void  DefineOptions(QwOptions& options); //!< Defines available class options for QwOptions
    void ProcessOptions(QwOptions &options); //!< Processes the options contained in the QwOptions object.
    void ProcessOptions(const TString& dbname, const TString& username, const TString& passwd, const TString& dbhost="localhost", const Int_t dbport = 0, const TString& accesslevel = "ro"); //!< Processes database options

    mysqlpp::Query Query(const char *qstr=0     ) {return query(qstr);} //<! Generate a query to the database.
    mysqlpp::Query Query(const std::string &qstr) {return query(qstr);} //<! Generate a query to the database.

    
    const string GetVersion();                             //! Return a full version string for the DB schema
    const string GetVersionMajor() {return fVersionMajor;} //<! fVersionMajor getter
    const string GetVersionMinor() {return fVersionMinor;} //<! fVersionMinor getter
    const string GetVersionPoint() {return fVersionPoint;} //<! fVersionPoint getter
    const string GetValidVersion();                        //<! Return a full required version string.

    void         PrintServerInfo();                        //<! Print Server Information

 private:
    enum EQwDBAccessLevel{kQwDatabaseOff, kQwDatabaseReadOnly, kQwDatabaseReadWrite};
 private:

    Bool_t       ValidateConnection();                  //!< Checks that given connection parameters result in a valid connection
    bool StoreDBVersion();  //!< Retrieve database schema version information from database

    // Do not allow compiler to automatically generated these functions
    QwDatabase(const QwDatabase& rhs);  //!< Copy Constructor (not implemented)
    QwDatabase& operator= (const QwDatabase& rhs); //!< Assignment operator (not implemented)
//    QwDatabase* operator& (); //!< Address-of operator (not implemented)
//    const QwDatabase* operator& () const; //!< Address-of operator (not implemented)

    EQwDBAccessLevel fAccessLevel;  //!< Access level of the database instance

    string fDatabase;        //!< Name of database to connect to
    string fDBServer;        //!< Name of server carrying DB to connect to
    string fDBUsername;      //!< Name of account to connect to DB server with
    string fDBPassword;      //!< DB account password
    UInt_t fDBPortNumber;    //!< Port number to connect to on server (mysql default port is 3306)
    Bool_t fValidConnection; //!< True if a valid connection was established using defined connection information

    string fVersionMajor;    //!< Major version number of current DB schema
    string fVersionMinor;    //!< Minor version number of current DB schema
    string fVersionPoint;    //!< Point version number of current DB schema
    const string kValidVersionMajor;
    const string kValidVersionMinor;
    const string kValidVersionPoint;

};

#endif

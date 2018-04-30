/*!
 * \file   QwDatabase.h
 * \brief  A class for handling connections to the Qweak database.
 *
 * \author Damon Spayde
 * \date   2010-01-07
 */

#ifndef QWPARITYDB_HH
#define QWPARITYDB_HH

// System headers
#include <map>
#include <vector>
#include <string>
#include <typeinfo>

// Third Party Headers
#include <mysql++.h>

// ROOT headers
#include "TString.h"

// Qweak headers
#include "QwTypes.h"
#include "QwLog.h"
#include "QwColor.h"
#include "QwOptions.h"
#include "QwDatabase.h"
#include "QwSubsystemArrayParity.h"

//#include "QwParitySSQLS.h"
#include "QwDBInterface.h"

// Forward declarations
class QwEventBuffer;

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

class QwParityDB: public QwDatabase {
  public:

    QwParityDB(); //!< Simple constructor
    QwParityDB(QwOptions &options); //!< Constructor with QwOptions object

    virtual ~QwParityDB(); //!< Destructor

    void         SetupOneRun(QwEventBuffer& qwevt);        //<! Initialize run ID, runlet ID, and analysis ID using data from CODA event buffer
    void         FillParameterFiles(QwSubsystemArrayParity& subsys);

    UInt_t GetMonitorID(const string& name, Bool_t zero_id_is_error=kTRUE);         //<! Get monitor_id for beam monitor name
    UInt_t GetMainDetectorID(const string& name, Bool_t zero_id_is_error=kTRUE);    //<! Get main_detector_id for main detector name
    UInt_t GetLumiDetectorID(const string& name, Bool_t zero_id_is_error=kTRUE);    //<! Get lumi_detector_id for lumi detector name
    const string GetMeasurementID(const Int_t index);
    UInt_t GetSlowControlDetectorID(const string& name);         //<! Get slow_controls_data_id for epics name
    UInt_t GetErrorCodeID(const string& name);         //<! Get error_code_id for error code name
    
    UInt_t GetRunNumber() {return fRunNumber;}       //<! Run number getter
    UInt_t GetSegmentNumber() {return fSegmentNumber;}       //<! CODA File segment number getter
    UInt_t GetRunID()     {return fRunID;}           //<! Run ID getter
    UInt_t GetRunletID()     {return fRunletID;}           //<! Runlet ID getter
    UInt_t GetAnalysisID() {return fAnalysisID;};    //<! Get analysis ID


    UInt_t GetRunID(QwEventBuffer& qwevt);           //<! Get run ID using data from CODA event buffer
    UInt_t GetRunletID(QwEventBuffer& qwevt);      //<! Get runlet ID using data from CODA event buffer
    UInt_t GetAnalysisID(QwEventBuffer& qwevt);      //<! Get analysis ID using data from CODA event buffer
    Bool_t       SetRunNumber(const UInt_t runnum);        //<! Run number setter
    Bool_t       SetSegmentNumber(const UInt_t segment);        //<! CODA file segment number setter
    static void  DefineAdditionalOptions(QwOptions& options); //!< Defines QwParityDB-specific class options for QwOptions
    void ProcessAdditionalOptions(QwOptions &options); //!< Processes the options contained in the QwOptions object.

 private:

    UInt_t SetRunID(QwEventBuffer& qwevt);        //<! Set fRunID using data from CODA event buffer
    UInt_t SetRunletID(QwEventBuffer& qwevt);        //<! Set fRunletID using data from CODA event buffer
    UInt_t SetAnalysisID(QwEventBuffer& qwevt);   //<! Set fAnalysisID using data from CODA event buffer
    void StoreMonitorIDs();                             //<! Retrieve monitor IDs from database and populate fMonitorIDs
    void StoreMainDetectorIDs();                        //<! Retrieve main detector IDs from database and populate fMainDetectorIDs
    void StoreLumiDetectorIDs();                        //<! Retrieve LUMI monitor IDs from database and populate fLumiDetectorIDs
    void StoreMeasurementIDs();
    void StoreSlowControlDetectorIDs();                  //<! Retrieve slow controls data IDs from database and populate fSlow_Controls_DataIDs
    void StoreErrorCodeIDs();                             //<! Retrieve error code IDs from database and populate fErrorCodeIDs

    UInt_t fRunNumber;       //!< Run number of current run
    Int_t fSegmentNumber;    //!< CODA file segment number of current run
    UInt_t fRunID;           //!< run_id of current run
    UInt_t fRunletID;        //!< runlet_id of current run
    UInt_t fAnalysisID;      //!< analysis_id of current analysis pass
    bool fDisableAnalysisCheck; //!< Flag to disable pre-existing analysis_id check

    static std::map<string, unsigned int> fMonitorIDs; //!< Associative array of beam monitor IDs.  This declaration will be a problem if QwDatabase is used to connect to two databases simultaneously.
    static std::map<string, unsigned int> fMainDetectorIDs; //!< Associative array of main detector IDs.  This declaration will be a problem if QwDatabase is used to connect to two databases simultaneously.
    static std::map<string, unsigned int> fLumiDetectorIDs; //!< Associative array of LUMI detector IDs.  This declaration will be a problem if QwDatabase is used to connect to two databases simultaneously.
    static std::map<string, unsigned int> fSlowControlDetectorIDs; //!< Associative array of slow controls data IDs.  This declaration will be a problem if QwDatabase is used to connect to two databases simultaneously.
    static std::map<string, unsigned char> fErrorCodeIDs; //!< Associative array of error code IDs.  This declaration will be a problem if QwDatabase is used to connect to two databases simultaneously.
    static std::vector<string>            fMeasurementIDs;

    friend class StoreMonitorID;
    friend class StoreMainDetectorID;
    friend class StoreLumiDetectorID;
    friend class StoreMeasurementID;
    friend class StoreSlowControlDetectorID;
    friend class StoreErrorCodeID;
};


#endif

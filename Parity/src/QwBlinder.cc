/*!
 * \file   QwBlinder.cc
 * \brief  A class for blinding data, adapted from G0 blinder class.
 *
 * \author Peiqing Wang
 * \date   2010-04-14
 */

#include "QwBlinder.h"

// System headers
#include <string>
#include <limits>

#include "TMath.h"

// Qweak headers
#include "QwLog.h"
#ifdef __USE_DATABASE__
#define MYSQLPP_SSQLS_NO_STATICS
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__
#include "QwVQWK_Channel.h"
#include "QwParameterFile.h"

///  Blinder event counter indices
enum EQwBlinderErrorCounterIndices{
  kBlinderCount_Blindable=0,
  kBlinderCount_NonBlindable,
  kBlinderCount_Transverse,
  kBlinderCount_Disabled,
  kBlinderCount_NoBeam,
  kBlinderCount_UnknownTarget,
  kBlinderCount_ChangedTarget,
  kBlinderCount_UndefinedWien,
  kBlinderCount_ChangedWien,
  kBlinderCount_UndefinedIHWP,
  kBlinderCount_ChangedIHWP,
  kBlinderCount_OtherFailure,
  kBlinderCount_NumCounters
};


//  String names of the blinding and Wien status values
const TString QwBlinder::fStatusName[4] = {"Indeterminate", "NotBlindable",
					   "Blindable", "BlindableFail"};

// Maximum blinding asymmetry for additive blinding
const Double_t QwBlinder::kDefaultMaximumBlindingAsymmetry = 0.150; // ppm
const Double_t QwBlinder::kDefaultMaximumBlindingFactor = 0.0; // [fraction]

// Default seed, associated with seed_id 0
const TString QwBlinder::kDefaultSeed = "Default seed, should not be used!";

//**************************************************//
void QwBlinder::DefineOptions(QwOptions &options){
  options.AddOptions("Blinder")("blinder.force-target-blindable", po::value<bool>()->default_bool_value(false),
		       "Forces the blinder to interpret the target as being in a blindable position");
  options.AddOptions("Blinder")("blinder.force-target-out", po::value<bool>()->default_bool_value(false),
		       "Forces the blinder to interpret the target position as target-out");
  options.AddOptions("Blinder")("blinder.beam-current-threshold", po::value<double>()->default_value(2.5),
		       "Beam current in microamps below which data will not be blinded");
}


/**
 * Default constructor using optional database connection and blinding strategy
 * @param blinding_strategy Blinding strategy
 */
QwBlinder::QwBlinder(const EQwBlindingStrategy blinding_strategy):
  fTargetBlindability_firstread(kIndeterminate),
  fTargetBlindability(kIndeterminate),
  fTargetPositionForced(kFALSE),
  //
  fWienMode_firstread(kWienIndeterminate),
  fWienMode(kWienIndeterminate),
  fIHWPPolarity_firstread(0),
  fIHWPPolarity(0),
  fSpinDirectionForced(kFALSE),
  //
  fBeamCurrentThreshold(1.0),
  fBeamIsPresent(kFALSE),
  fBlindingStrategy(blinding_strategy),
  fBlindingOffset(0.0),
  fBlindingOffset_Base(0.0),
  fBlindingFactor(1.0),
  //
  fMaximumBlindingAsymmetry(kDefaultMaximumBlindingAsymmetry),
  fMaximumBlindingFactor(kDefaultMaximumBlindingFactor)
{
  // Set up the blinder with seed_id 0
  fSeed = kDefaultSeed;
  fSeedID = 0;

  fCREXTargetIndex  = -1;

  Int_t tgt_index;

  // Read parameter file
  QwParameterFile blinder("blinder.map");
  if (blinder.FileHasVariablePair("=", "seed", fSeed))
    QwVerbose << "Using seed from file: " << fSeed << QwLog::endl;
  if (blinder.FileHasVariablePair("=", "max_asymmetry", fMaximumBlindingAsymmetry))
    QwVerbose << "Using blinding box: " << fMaximumBlindingAsymmetry << " ppm" << QwLog::endl;
  if (blinder.FileHasVariablePair("=", "max_factor", fMaximumBlindingFactor))
    QwVerbose << "Using blinding factor: " << fMaximumBlindingFactor << QwLog::endl;
  if (blinder.FileHasVariablePair("=", "crex_target_index", tgt_index)){
    if (tgt_index>=kCREXTgtIndexMin && tgt_index<=kCREXTgtIndexMax){
      fCREXTargetIndex = tgt_index;
    } else {
      QwError << "Invalid CREX target index for blindable events!  Exiting!" 
	      << QwLog::endl;
      exit(100);
    }
  }
  QwMessage << "What is the blindable CREX target position (-1 means we're using the PREX positions)? " << fCREXTargetIndex << QwLog::endl;
  if (fCREXTargetIndex>=kCREXTgtIndexMin){
    fSeed.Prepend(TString("[Using CREX positions!]  "));
    QwMessage << "Updated the seed string: " << fSeed << QwLog::endl;
  }
  std::string strategy;
  if (blinder.FileHasVariablePair("=", "strategy", strategy)) {
    std::transform(strategy.begin(), strategy.end(), strategy.begin(), ::tolower);
    QwVerbose << "Using blinding strategy from file: " << strategy << QwLog::endl;
    if (strategy == "diabled") fBlindingStrategy = kDisabled;
    else if (strategy == "additive") fBlindingStrategy = kAdditive;
    else if (strategy == "multiplicative") fBlindingStrategy = kMultiplicative;
    else if (strategy == "additivemultiplicative") fBlindingStrategy = kAdditiveMultiplicative;
    else QwWarning << "Blinding strategy " << strategy << " not recognized" << QwLog::endl;
  }

  std::string spin_direction;
  if (blinder.FileHasVariablePair("=", "force-spin-direction", spin_direction)) {
    std::transform(spin_direction.begin(), spin_direction.end(), spin_direction.begin(), ::tolower);
    if (spin_direction == "spin-forward"){
      QwWarning << "QwBlinder::QwBlinder:  Spin direction forced with force-spin-direction==spin-forward" << QwLog::endl;
      SetWienState(kWienForward);
      SetIHWPPolarity(+1);
      fSpinDirectionForced = kTRUE;
    } else if (spin_direction == "spin-backward"){
      QwWarning << "QwBlinder::QwBlinder:  Spin direction forced with force-spin-direction==spin-backward" << QwLog::endl;
      SetWienState(kWienBackward);
      SetIHWPPolarity(+1);
      fSpinDirectionForced = kTRUE;
    } else if (spin_direction == "spin-vertical"){
      QwWarning << "QwBlinder::QwBlinder:  Spin direction forced with force-spin-direction==spin-vertical" << QwLog::endl;
      SetWienState(kWienVertTrans);
      SetIHWPPolarity(+1);
      fSpinDirectionForced = kTRUE;
    } else if (spin_direction == "spin-horizontal"){
      QwWarning << "QwBlinder::QwBlinder:  Spin direction forced with force-spin-direction==spin-horizontal" << QwLog::endl;
      SetWienState(kWienHorizTrans);
      SetIHWPPolarity(+1);
      fSpinDirectionForced = kTRUE;
    } else {
      QwError << "QwBlinder::QwBlinder:  Unrecognized option given to force-spin-direction in blinder.map; "
	      << "force-spin-direction==" << spin_direction << ".  Exit and correct the file."
	      << QwLog::endl;
      exit(10);
    } 
  }

  std::string target_type;
  if (blinder.FileHasVariablePair("=", "force-target-type", target_type)) {
    std::transform(target_type.begin(), target_type.end(), target_type.begin(), ::tolower);
    if (target_type == "target-blindable"){
      QwWarning << "QwBlinder::QwBlinder:  Target position forced with force-target-type==target-blindable" << QwLog::endl;
      fTargetPositionForced = kTRUE;
      SetTargetBlindability(QwBlinder::kBlindable);
    } else if (target_type == "target-out"){
      QwWarning << "QwBlinder::QwBlinder:  Target position forced with force-target-type==target-out" << QwLog::endl;
      fTargetPositionForced = kTRUE;
      SetTargetBlindability(QwBlinder::kNotBlindable);
    } else {
      QwError << "QwBlinder::QwBlinder:  Unrecognized option given to force-target-type in blinder.map; "
	      << "force-target-type==" << target_type << ".  Exit and correct the file."
	      << QwLog::endl;
      exit(10);
    } 
  }

  // Initialize blinder from seed
  InitBlinders(0);
  // Calculate set of test values
  InitTestValues(10);

  if (fSpinDirectionForced){
    if (fWienMode == kWienForward){
      fBlindingOffset = fBlindingOffset_Base;
    } else if (fWienMode == kWienBackward){
      fBlindingOffset = -1 * fBlindingOffset_Base;
    } else {
      fBlindingOffset = 0.0;
    }
  }

  // Resize counters
  fPatternCounters.resize(kBlinderCount_NumCounters);
  fPairCounters.resize(kBlinderCount_NumCounters);
}


/**
 * Destructor checks the validity of the blinding and unblinding
 */
QwBlinder::~QwBlinder()
{
  // Check the blinded values
  PrintFinalValues();
}


/**
 * Update the blinder status with new external information
 *
 * @param options Qweak option handler
 */
void QwBlinder::ProcessOptions(QwOptions& options)
{
  if (options.GetValue<bool>("blinder.force-target-out")
      && options.GetValue<bool>("blinder.force-target-blindable")){
    QwError << "QwBlinder::ProcessOptions:  Both blinder.force-target-blindable and blinder.force-target-out are set.  "
	    << "Only one can be in force at one time.  Exit and choose one option."
	    << QwLog::endl;
    exit(10);
  } else if (options.GetValue<bool>("blinder.force-target-blindable")){
    QwWarning << "QwBlinder::ProcessOptions:  Target position forced with blinder.force-target-blindable." << QwLog::endl;
    fTargetPositionForced = kTRUE;
    SetTargetBlindability(QwBlinder::kBlindable);
  } else if (options.GetValue<bool>("blinder.force-target-out")){
    QwWarning << "QwBlinder::ProcessOptions:  Target position forced with blinder.force-target-out." << QwLog::endl;
    fTargetPositionForced = kTRUE;
    SetTargetBlindability(QwBlinder::kNotBlindable);
  }

  fBeamCurrentThreshold = options.GetValue<double>("blinder.beam-current-threshold");
}

/**
 * Update the blinder status with new external information
 *
 * @param db Database connection
 */
#ifdef __USE_DATABASE__
void QwBlinder::Update(QwParityDB* db)
{
  //  Update the seed ID then tell us if it has changed.
  UInt_t old_seed_id = fSeedID;
  ReadSeed(db);
  // If the blinding seed has changed, re-initialize the blinder
  if (fSeedID != old_seed_id ||
      (fSeedID==0 && fSeed!=kDefaultSeed) ) {
    QwWarning << "Changing blinder seed to " << fSeedID
              << " from " << old_seed_id << "." << QwLog::endl;
    InitBlinders(fSeedID);
    InitTestValues(10);
  }
}
#endif // __USE_DATABASE__

/**
 * Update the blinder status using a random number
 *
 */
void QwBlinder::Update()
{
  //  Update the seed ID then tell us if it has changed.
  UInt_t old_seed_id = fSeedID;
  ReadRandomSeed();
  //  Force the target to blindable, Wien to be forward,
  //  and IHWP polarity to be +1
  SetTargetBlindability(QwBlinder::kBlindable);
  SetWienState(kWienForward);
  SetIHWPPolarity(+1);
  // If the blinding seed has changed, re-initialize the blinder
  if (fSeedID != old_seed_id ||
      (fSeedID==0 && fSeed!=kDefaultSeed) ) {
    QwWarning << "Changing blinder seed to " << fSeedID
              << " from " << old_seed_id << "." << QwLog::endl;
    InitBlinders(fSeedID);
    InitTestValues(10);
  }
}

/**
 * Update the blinder status with new external information
 *
 * @param detectors Current subsystem array
 */
void QwBlinder::Update(const QwSubsystemArrayParity& detectors)
{
  static QwVQWK_Channel q_targ("q_targ");
  if (fBlindingStrategy != kDisabled && fTargetBlindability==kBlindable) {
    // Check for the target blindability flag
    

    // Check that the current on target is above acceptable limit
    Bool_t tmp_beam = kFALSE;
    //    if (detectors.RequestExternalValue(q_targ.GetElementName(), &q_targ)) {
    if (detectors.RequestExternalValue("q_targ", &q_targ)) {
      if (q_targ.GetValue() > fBeamCurrentThreshold){
	// 	std::cerr << "q_targ.GetValue()==" 
	// 		  << q_targ.GetValue() << std::endl;
	tmp_beam = kTRUE;
      }
    }
    fBeamIsPresent &= tmp_beam;
  }
}

/**
 * Update the blinder status with new external information
 *
 * @param epics Current EPICS event
 */
void QwBlinder::Update(const QwEPICSEvent& epics)
{
  // Check for the target information
  // Position:
  //     QWtgt_name == "HYDROGEN-CELL" || QWTGTPOS > 350
  // Temperature:
  //     QWT_miA < 22.0 && QWT_moA < 22.0
  // Pressure:
  //     QW_PT3 in [20,35] && QW_PT4 in [20,35]
  if (fBlindingStrategy != kDisabled && !(fTargetPositionForced) ) {
    //TString  position  = epics.GetDataString("QWtgt_name");
    Double_t tgt_pos   = epics.GetDataValue("pcrex90BDSPOS.VAL");
    QwDebug << "Target parameters used by the blinder: "
      //	  << "QWtgt_name=" << position << " "
	    << "QWTGTPOS=" << tgt_pos << " "
	    << QwLog::endl;

    //
    // ****  Target index 1:  Beginning of CREX
    if (fCREXTargetIndex==1 &&
	(tgt_pos>14.5e6 && tgt_pos<18.0e6) ){
      //  Calcium-48 target position
      SetTargetBlindability(QwBlinder::kBlindable);

    } else if (fCREXTargetIndex==1 &&
	       ( (tgt_pos>-1.0e3 && tgt_pos<14.5e6)
		 || (tgt_pos>18.0e6 && tgt_pos<61.e6) ) ){
      //  Reasonable non-calcium-48 target positions
      SetTargetBlindability(QwBlinder::kNotBlindable);

     
      //
      // ****  Target index 2:  After 20January change in target location
    } else if (fCREXTargetIndex==2 &&
	(tgt_pos>11.5e6 && tgt_pos<14.5e6) ){
      //  Calcium-48 target position (old Ca-40 position)
      SetTargetBlindability(QwBlinder::kBlindable);

    } else if (fCREXTargetIndex==2 &&
	       ( (tgt_pos>-1.0e3 && tgt_pos<11.5e6)
		 || (tgt_pos>14.5e6 && tgt_pos<61.e6) ) ){
      //  Reasonable non-calcium-48 target positions
      SetTargetBlindability(QwBlinder::kNotBlindable);

      //
      //  ****  Target index -1:  These are the PREX positions
    } else if ( fCREXTargetIndex==-1 && 
		(/*  Target positions before 1 August 2019.*/
		 ( (tgt_pos > 3e6 && tgt_pos < 6.9e6) 
		   || (tgt_pos > 7.3e6 && tgt_pos < 7.7e6))
		 /*  Target positions after 1 August 2019.*/
		 ||( (tgt_pos>30.e6 && tgt_pos<69e6)
		     || (tgt_pos>73e6 && tgt_pos<78e6)
		     ) ) ){
      //  Lead-208 target positions
      SetTargetBlindability(QwBlinder::kBlindable);

    } else if  ( fCREXTargetIndex==-1 && 
		 (/*  Target positions before 1 August 2019.*/
		  ((tgt_pos > -1e3 && tgt_pos < 3e6) 
		   || (tgt_pos > 6.8e6 && tgt_pos < 7.2e6)
		   || (tgt_pos > 7.7e6 && tgt_pos < 10e6))
		  /*  Target positions after 1 August 2019.*/
		  || ( (tgt_pos>17e6 && tgt_pos<30e6)
		       || (tgt_pos>69e6 && tgt_pos<73e6)
		       || (tgt_pos>78e6 && tgt_pos<90e6)
		       )
		  ) ){
      //  Positions are not lead-208 targets.
      SetTargetBlindability(QwBlinder::kNotBlindable);

    } else {
      SetTargetBlindability(QwBlinder::kIndeterminate);
      QwWarning << "Target parameters used by the blinder are indeterminate: "
	//  << "QWtgt_name=" << position << " "
		<< "QWTGTPOS=" << tgt_pos << " "
		<< QwLog::endl;
    } // End of tests on target positions
  }
  // Check for the beam polarity information
  //     IGL1I00DI24_24M         Beam Half-wave plate Read(off=out)
  //
  if (fBlindingStrategy != kDisabled && !(fSpinDirectionForced) &&
      (fTargetBlindability == QwBlinder::kBlindable) ) {
    //  Use the EPICS class functions to determine the
    //  Wien mode and IHWP polarity.
    SetWienState(epics.DetermineWienMode());
    SetIHWPPolarity(epics.DetermineIHWPPolarity());
    
    if (fWienMode == kWienForward){
      fBlindingOffset = fBlindingOffset_Base * fIHWPPolarity;
    } else if (fWienMode == kWienBackward){
      fBlindingOffset = -1 * fBlindingOffset_Base * fIHWPPolarity;
    } else {
      fBlindingOffset = 0.0;
    }
  }
}

/*!-----------------------------------------------------------
 *------------------------------------------------------------
 * Function to read the seed in from the database.
 *
 * Parameters:
 *
 * Return: Int_t 
 *
 *------------------------------------------------------------
 *------------------------------------------------------------*/
#ifdef __USE_DATABASE__
Int_t QwBlinder::ReadSeed(QwParityDB* db)
{
  // Return unchanged if no database specified
  if (! db) {
    QwWarning << "QwBlinder::ReadSeed(): No database specified" << QwLog::endl;
    fSeedID = 0;
    fSeed   = "Default seed, No database specified";
    return 0;
  }

  // Try to connect to the database
  if (db->Connect()) {

    QwError << "QwBlinder::ReadSeed  db->GetRunNumber() returns "
	    <<  db->GetRunNumber() << QwLog::endl;
    // Build up query
    string s_sql = "SELECT seed_id, seed FROM seeds, run as rf, run as rl WHERE seeds.first_run_id = rf.run_id AND seeds.last_run_id = rl.run_id ";
    s_sql += "AND rf.run_number <= ";
    s_sql += Form("%d",db->GetRunNumber());
    s_sql += " AND rl.run_number >= ";
    s_sql += Form("%d",db->GetRunNumber());
    s_sql += " AND seed_id>2";

    QwError << "QwBlinder::ReadSeed s_sql contains \'"
	    << s_sql  << "\'" << QwLog::endl;

    // Send query
    mysqlpp::Query query = db->Query();
    query << s_sql;
    mysqlpp::StoreQueryResult res = query.store();

    // Store seed_id and seed value in fSeedID and fSeed (want to store actual seed_id in those
    // cases where only the most recent was requested (fSeedID = 0))
    // There should be one and only one seed_id for each seed.
    if (res.size() == 1) {
      // Store seed_id in fSeedID (want to store actual seed_id in those
      // cases where only the most recent was requested (fSeedID = 0))
      fSeedID = res[0]["seed_id"];

      // Store seed value
      if (!res[0]["seed"].is_null()) {
        fSeed = res[0]["seed"].data();
      } else {
        QwError << "QwBlinder::ReadSeed(): Seed value came back NULL from the database." << QwLog::endl;
        fSeedID = 0;
        fSeed = kDefaultSeed;
      }

      std::cout << "QwBlinder::ReadSeed():  Successfully read "
      << Form("the fSeed with ID %d from the database.", fSeedID)
      << std::endl;

    } else {
      // Error Condition.
      // There should be one and only one seed_id for each seed.
      fSeedID = 0;
      fSeed   = Form("ERROR:  There should be one and only one seed_id for each seed, but this had %zu.",
                     res.size());
      std::cerr << "QwBlinder::ReadSeed(): "<<fSeed<<std::endl;
    }

    // Disconnect from database
    db->Disconnect();

  } else {

    //  We were unable to open the connection.
    fSeedID = 0;
    fSeed   = "ERROR:  Unable to open the connection to the database.";
    QwError << "QwBlinder::ReadSeed(): Unable to open connection to database." << QwLog::endl;
  }

  return fSeedID;
}
#endif // __USE_DATABASE__

/*!-----------------------------------------------------------
 *------------------------------------------------------------
 * Function to read the seed string generated utilizing a random number generator
 *
 * Parameters: none
 *
 * Return: Int_t 
 *
 *------------------------------------------------------------
 *------------------------------------------------------------*/
Int_t QwBlinder::ReadRandomSeed()
{
  static const Char_t alphanum[] =
    "0123456789"
    "!@#$%^&*"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

  Int_t strLen = sizeof(alphanum) - 1;
  Char_t randomchar[20];
  // Initialize random number generator.
  srand(time(0));
  //get  a "random" positive integer 
  
  for (int i = 0; i < 20; ++i) {
    randomchar[i] = alphanum[rand() % strLen];
  }
  fSeedID=rand();
  TString frandomSeed(randomchar);
  fSeed=frandomSeed;//a random string
  return fSeedID;
}

/*!-----------------------------------------------------------
 *------------------------------------------------------------
 * Function to read the seed in from the database.
 *
 * Parameters:
 *	seed_id = ID number of seed to use (0 = most recent seed)
 *
 * Return: Int_t
 *
 *------------------------------------------------------------
 *------------------------------------------------------------*/
#ifdef __USE_DATABASE__
Int_t QwBlinder::ReadSeed(QwParityDB* db, const UInt_t seed_id)
{
  // Return unchanged if no database specified
  if (! db) {
    QwWarning << "QwBlinder::ReadSeed(): No database specified" << QwLog::endl;
    fSeedID = 0;
    fSeed   = "Default seed, No database specified";
    return 0;
  }

  // Try to connect to the database
  if (db->Connect()) {

    // Build up query
    string s_sql = "SELECT * FROM seeds ";
    if (fSeedID > 0) {
      // Use specified seed
      Char_t s_seed_id[10];
      sprintf(s_seed_id, "%d", seed_id);
      s_sql += "WHERE seed_id = ";
      s_sql += string(s_seed_id);
    } else {
      // Use most recent seed
      s_sql += "ORDER BY seed_id DESC LIMIT 1";
    }

    // Send query
    mysqlpp::Query query = db->Query();
    query << s_sql;
    std::vector<QwParitySSQLS::seeds> res;
    query.storein(res);

    // Store seed_id and seed value in fSeedID and fSeed (want to store actual seed_id in those
    // cases where only the most recent was requested (fSeedID = 0))
    // There should be one and only one seed_id for each seed.
    if (res.size() == 1) {
      // Store seed_id in fSeedID (want to store actual seed_id in those
      // cases where only the most recent was requested (fSeedID = 0))
      fSeedID = res[0].seed_id;

      // Store seed value
      if (!res[0].seed.is_null) {
        fSeed = res[0].seed.data;
      } else {
        QwError << "QwBlinder::ReadSeed(): Seed value came back NULL from the database." << QwLog::endl;
        fSeedID = 0;
        fSeed = kDefaultSeed;
      }

      std::cout << "QwBlinder::ReadSeed():  Successfully read "
      << Form("the fSeed with ID %d from the database.", fSeedID)
      << std::endl;

    } else {
      // Error Condition.
      // There should be one and only one seed_id for each seed.
      fSeedID = 0;
      fSeed   = Form("ERROR:  There should be one and only one seed_id for each seed, but this had %zu.",
                     res.size());
      std::cerr << "QwBlinder::ReadSeed(): "<<fSeed<<std::endl;
    }

    // Disconnect from database
    db->Disconnect();

  } else {

    //  We were unable to open the connection.
    fSeedID = 0;
    fSeed   = "ERROR:  Unable to open the connection to the database.";
    QwError << "QwBlinder::ReadSeed(): Unable to open connection to database." << QwLog::endl;
  }

  return fSeedID;
}
#endif // __USE_DATABASE__


/**
 * Initialize the blinder parameters
 */
void QwBlinder::InitBlinders(const UInt_t seed_id)
{
  // If the blinding strategy is disabled
  if (fBlindingStrategy == kDisabled) {

      fSeed = kDefaultSeed;
      fSeedID = 0;
      fBlindingFactor = 1.0;
      fBlindingOffset = 0.0;
      fBlindingOffset_Base = 0.0;
      QwWarning << "Blinding parameters have been disabled!"<< QwLog::endl;

  // Else blinding is enabled
  } else {

    Int_t finalseed = UseMD5(fSeed);

    Double_t newtempout;
    if ((finalseed & 0x80000000) == 0x80000000) {
      newtempout = -1.0 * (finalseed & 0x7FFFFFFF);
    } else {
      newtempout =  1.0 * (finalseed & 0x7FFFFFFF);
    }


    /// The blinding constants are determined in two steps.
    ///
    /// First, the blinding asymmetry (offset) is determined.  It is
    /// generated from a signed number between +/- 0.244948974 that
    /// is squared to get a number between +/- 0.06 ppm.
    static Double_t maximum_asymmetry_sqrt = sqrt(fMaximumBlindingAsymmetry);
    Double_t tmp1 = maximum_asymmetry_sqrt * (newtempout / Int_t(0x7FFFFFFF));
    fBlindingOffset = tmp1 * fabs(tmp1) * 0.000001;

    //  Do another little calulation to round off the blinding asymmetry
    Double_t tmp2;
    tmp1 = fBlindingOffset * 4;    // Exactly shifts by two binary places
    tmp2 = tmp1 + fBlindingOffset; // Rounds 5*fBlindingOffset
    fBlindingOffset = tmp2 - tmp1; // fBlindingOffset has been rounded.
    
    //  Set the base blinding offset.
    fBlindingOffset_Base = fBlindingOffset;

    /// Secondly, the multiplicative blinding factor is determined.  This
    /// number is generated from the blinding asymmetry between, say, 0.9 and 1.1
    /// by an oscillating but uniformly distributed sawtooth function.
    fBlindingFactor = 1.0;
    if (fMaximumBlindingAsymmetry > 0.0) {
      /// TODO:  This section of InitBlinders doesn't calculate a reasonable fBlindingFactor but we don't use it for anything.
      fBlindingFactor  = 1.0 + fmod(30.0 * fBlindingOffset, fMaximumBlindingAsymmetry);
      fBlindingFactor /= (fMaximumBlindingAsymmetry > 0.0 ? fMaximumBlindingAsymmetry : 1.0);
    }

    QwMessage << "Blinding parameters have been calculated."<< QwLog::endl;

  }

  // Generate checksum
  TString hex_string;
  hex_string.Form("%.16llx%.16llx", *(ULong64_t*)(&fBlindingFactor), *(ULong64_t*)(&fBlindingOffset));
  fDigest = GenerateDigest(hex_string);
  fChecksum = "";
  for (size_t i = 0; i < fDigest.size(); i++)
    fChecksum += string(Form("%.2x",fDigest[i]));
}

#ifdef __USE_DATABASE__
void  QwBlinder::WriteFinalValuesToDB(QwParityDB* db)
{
  WriteChecksum(db);
  if (! CheckTestValues()) {
    QwError << "QwBlinder::WriteFinalValuesToDB():  "
            << "Blinded test values have changed; may be a problem in the analysis!!!"
            << QwLog::endl;
  }
  WriteTestValues(db);
}
#endif // __USE_DATABASE__


/**
 * Generate a set of test values of similar size as measured asymmetries
 * @param n Number of test values
 */
void QwBlinder::InitTestValues(const int n)
{
  // Use the stored seed to get a pseudorandom number
  Int_t finalseed = UsePseudorandom(fSeed);

  fTestValues.clear();
  fBlindTestValues.clear();
  fUnBlindTestValues.clear();

  Double_t tmp_offset = fBlindingOffset;
  fBlindingOffset = fBlindingOffset_Base;
  // For each test case
  for (int i = 0; i < n; i++) {

    // Generate a pseudorandom number
    for (Int_t j = 0; j < 16; j++) {
      finalseed &= 0x7FFFFFFF;
      if ((finalseed & 0x800000) == 0x800000) {
        finalseed = ((finalseed ^ 0x00000d) << 1) | 0x1;
      } else {
        finalseed <<= 1;
      }
    }

    // Mask out the low digits of the finalseed, multiply by two,
    // divide by the mask value, subtract from 1, and divide result by
    // 1.0e6 to get a range of about -1000 to +1000 ppb.
    Int_t mask = 0xFFFFFF;
    Double_t tempval = (1.0 - 2.0*(finalseed&mask)/mask) / (1.0e6);

    // Store the test values
    fTestValues.push_back(tempval);
    BlindValue(tempval);
    fBlindTestValues.push_back(tempval);
    UnBlindValue(tempval);
    fUnBlindTestValues.push_back(tempval);
  }
  fBlindingOffset = tmp_offset;
  QwMessage << "QwBlinder::InitTestValues(): A total of " << fTestValues.size()
            << " test values have been calculated successfully." << QwLog::endl;
}

/**
 * Use string manipulation to get a number from the seed string
 * @param barestring Seed string
 * @return Integer number
 */
Int_t QwBlinder::UseStringManip(const TString& barestring)
{
  std::vector<UInt_t> choppedwords;
  UInt_t tmpword;
  Int_t finalseed = 0;

  for (Int_t i = 0; i < barestring.Length(); i++)
    {
      if (i % 4 == 0) tmpword = 0;
      tmpword |= (char(barestring[i]))<<(24-8*(i%4));
      if (i%4 == 3 || i == barestring.Length()-1)
        {
          choppedwords.push_back(tmpword);
          finalseed ^= (tmpword);
        }
    }
  for (Int_t i=0; i<64; i++)
    {
      finalseed &= 0x7FFFFFFF;
      if ((finalseed & 0x800000) == 0x800000)
        {
          finalseed = ((finalseed^0x00000d)<<1) | 0x1;
        }
      else
        {
          finalseed<<=1;
        }
    }
  if ((finalseed&0x80000000) == 0x80000000)
    {
      finalseed = -1 * (finalseed&0x7FFFFFFF);
    }
  else
    {
      finalseed = (finalseed&0x7FFFFFFF);
    }
  return finalseed;
}


/**
 * Use pseudo-random number generator to get a number from the seed string
 * @param barestring Seed string
 * @return Integer number
 */
Int_t QwBlinder::UsePseudorandom(const TString& barestring)
{
  ULong64_t finalseed;
  Int_t bitcount;
  Int_t tempout = 0;

  //  This is an attempt to build a portable 64-bit constant
  ULong64_t longmask = (0x7FFFFFFF);
  longmask<<=32;
  longmask|=0xFFFFFFFF;

  finalseed = 0;
  bitcount  = 0;
  for (Int_t i=0; i<barestring.Length(); i++)
    {
      if ( ((barestring[i])&0xC0)!=0 && ((barestring[i])&0xC0)!=0xC0)
        {
          finalseed = ((finalseed&longmask)<<1) | (((barestring[i])&0x40)>>6);
          bitcount++;
        }
      if ( ((barestring[i])&0x30)!=0 && ((barestring[i])&0x30)!=0x30)
        {
          finalseed = ((finalseed&longmask)<<1) | (((barestring[i])&0x10)>>4);
          bitcount++;
        }
      if ( ((barestring[i])&0xC)!=0 && ((barestring[i])&0xC)!=0xC)
        {
          finalseed = ((finalseed&longmask)<<1) | (((barestring[i])&0x4)>>2);
          bitcount++;
        }
      if ( ((barestring[i])&0x3)!=0 && ((barestring[i])&0x3)!=0x3)
        {
          finalseed = ((finalseed&longmask)<<1) | ((barestring[i])&0x1);
          bitcount++;
        }
    }
  for (Int_t i=0; i<(192-bitcount); i++)
    {
      if ((finalseed & 0x800000) == 0x800000)
        {
          finalseed = ((finalseed^0x00000d)<<1) | 0x1;
        }
      else
        {
          finalseed<<=1;
        }
    }
  tempout = (finalseed&0xFFFFFFFF) ^ ((finalseed>>32)&0xFFFFFFFF);
  if ((tempout&0x80000000) == 0x80000000)
    {
      tempout = -1 * (tempout&0x7FFFFFFF);
    }
  else
    {
      tempout =  1 * (tempout&0x7FFFFFFF);
    }
  return tempout;
}


/**
 * Use an MD5 checksum to get a number from the seed string
 * @param barestring Seed string
 * @return Integer number
 */
Int_t QwBlinder::UseMD5(const TString& barestring)
{
  Int_t temp = 0;
  Int_t tempout = 0;

  std::vector<UChar_t> digest = GenerateDigest(barestring);
  for (size_t i = 0; i < digest.size(); i++)
    {
      Int_t j = i%4;
      if (j == 0)
        {
          temp = 0;
        }
      temp |= (digest[i])<<(24-(j*8));
      if ( (j==3) || (i==(digest.size()-1)))
        {
          tempout ^= temp;
        }
    }

  if ((tempout & 0x80000000) == 0x80000000) {
      tempout = -1 * (tempout & 0x7FFFFFFF);
  } else {
      tempout = (tempout & 0x7FFFFFFF);
  }

  return tempout;
}


/*!-----------------------------------------------------------
 *------------------------------------------------------------
 * Function to write the checksum into the analysis table
 *
 * Parameters: void
 *
 * Return: void
 *
 * Note:  This function assumes that the analysis table has already
 *        been filled for the run.
 *------------------------------------------------------------
 *------------------------------------------------------------*/
#ifdef __USE_DATABASE__
void QwBlinder::WriteChecksum(QwParityDB* db)
{
  //----------------------------------------------------------
  // Construct SQL
  //----------------------------------------------------------
  Char_t s_number[20];
  string s_sql = "UPDATE analysis SET seed_id = ";
  sprintf(s_number, "%d", fSeedID);
  s_sql += string(s_number);
  s_sql += ", bf_checksum = ";
  s_sql += "\'" + fChecksum + "\'";
  s_sql += " WHERE analysis_id = ";
  sprintf(s_number, "%d", db->GetAnalysisID());
  s_sql += string(s_number);

  //----------------------------------------------------------
  // Execute SQL
  //----------------------------------------------------------
  db->Connect();
  mysqlpp::Query query = db->Query();
  query <<s_sql;
  query.execute();
  db->Disconnect();
} //End QwBlinder::WriteChecksum

/*!------------------------------------------------------------
 *------------------------------------------------------------
 * Function to write the test values into the database
 *
 * Parameters: void
 *
 * Return: void
 *------------------------------------------------------------
 *------------------------------------------------------------*/
void QwBlinder::WriteTestValues(QwParityDB* db)
{
  //----------------------------------------------------------
  // Construct Initial SQL
  //----------------------------------------------------------
  Char_t s_number[20];

  string s_sql_pre = "INSERT INTO bf_test (analysis_id, test_number, test_value) VALUES (";
  // analysis_id
  sprintf(s_number, "%d", db->GetAnalysisID());
  s_sql_pre += string(s_number);
  s_sql_pre += ", ";

  //----------------------------------------------------------
  // Construct Individual SQL and Execute
  //----------------------------------------------------------
  // Loop over all test values
  for (size_t i = 0; i < fTestValues.size(); i++)
    {
      string s_sql = s_sql_pre;

      // test_number
      sprintf(s_number, "%d", (int) i);
      s_sql += string(s_number);
      s_sql += ", ";

      // test_value
      sprintf(s_number, "%9g", fBlindTestValues[i]);
      s_sql += s_number;
      s_sql += ")";

      // Execute SQL
      db->Connect();
      mysqlpp::Query query = db->Query();
      query <<s_sql;
      query.execute();
      db->Disconnect();
    }
}
#endif // __USE_DATABASE__

/*!--------------------------------------------------------------
 *  This routines checks to see if the stored fBlindTestValues[i]
 *  match a recomputed blinded test value.  The values are cast
 *  into floats, and their difference must be less than a change
 *  of the least-significant-bit of fBlindTestValues[i].
 *--------------------------------------------------------------*/

Bool_t QwBlinder::CheckTestValues()
{
  Bool_t status = kTRUE;

  Double_t tmp_offset = fBlindingOffset;
  fBlindingOffset = fBlindingOffset_Base;
  double epsilon = std::numeric_limits<double>::epsilon();
  for (size_t i = 0; i < fTestValues.size(); i++) {

    /// First test: compare a blinded value with a second computation
    double checkval = fBlindTestValues[i];
    UnBlindValue(checkval);

    double test1 = fTestValues[i];
    double test2 = checkval;
    if ((test1 - test2) <= -epsilon || (test1 - test2) >= epsilon) {
      QwError << "QwBlinder::CheckTestValues():  Unblinded test value "
              << i
              << " does not agree with original test value, "
              << "with a difference of "
              << (test1 - test2) 
	      << " (epsilon==" << epsilon << ")"
	      << "." << QwLog::endl;
      status = kFALSE;
    }

    /// Second test: compare the unblinded value with the original value
    test1 = fUnBlindTestValues[i];
    test2 = fTestValues[i];
    if ((test1 - test2) <= -epsilon || (test1 - test2) >= epsilon) {
      QwError << "QwBlinder::CheckTestValues():  Unblinded test value "
              << i
              << " does not agree with original test value, "
              << "with a difference of "
              << (test1 - test2) << "." << QwLog::endl;
      status = kFALSE;
    }
  }
  fBlindingOffset = tmp_offset;
  return status;
}


/**
 * Generate an MD5 digest of the blinding parameters
 * @param input Input string
 * @return MD5 digest of the input string
 */
std::vector<UChar_t> QwBlinder::GenerateDigest(const TString& input) const
{
  // Initialize MD5 checksum array
  const UInt_t length = 16;
  UChar_t value[length];
  for (UInt_t i = 0; i < length; i++)
    value[i] = 0;

  // Calculate MD5 checksum from input and store in md5_value
  TMD5 md5;
  md5.Update((UChar_t*) input.Data(), input.Length());
  md5.Final(value);

  // Copy the MD5 checksum in an STL vector
  std::vector<UChar_t> output;
  for (UInt_t i = 0; i < length; i++)
    output.push_back(value[i]);

  return output;
}


/**
 * Print a summary of the blinding/unblinding test
 */
void QwBlinder::PrintFinalValues(Int_t kVerbosity)
{
  Int_t total_count = 0;
  for (size_t i=0; i<kBlinderCount_NumCounters; i++){
    total_count += fPatternCounters.at(i);
  }
  if (total_count<=0) return;
  
  QwMessage << "QwBlinder::PrintFinalValues():  Begin summary"    << QwLog::endl;
  QwMessage << "================================================" << QwLog::endl;
  PrintCountersValues(fPatternCounters, "Patterns");
  if(kVerbosity==1){
    QwMessage << "================================================" << QwLog::endl;
    PrintCountersValues(fPairCounters, "Pairs");
  }
  QwMessage << "================================================" << QwLog::endl;
  QwMessage << "The blinding parameters checksum for seed ID "
            << fSeedID << " is:" << QwLog::endl;
  QwMessage << fChecksum << QwLog::endl;
  QwMessage << "================================================" << QwLog::endl;
  CheckTestValues();
  double epsilon = std::numeric_limits<double>::epsilon();
  TString diff;
  QwMessage << "The test results are:" << QwLog::endl;
  QwMessage << std::setw(8)  << "Index"
            << std::setw(16) << "Original value"
            << std::setw(16) << "Blinded value"
            << std::setw(22) << "Orig.-Unblind value"
            << QwLog::endl;
  for (size_t i = 0; i < fTestValues.size(); i++) {
    if ((fTestValues[i]-fUnBlindTestValues[i]) < -epsilon 
	|| (fTestValues[i]-fUnBlindTestValues[i]) > epsilon )
      diff = Form("% 9g ppb", fTestValues[i]-fUnBlindTestValues[i]*1e9);
    else
      diff = "epsilon";
    QwMessage << std::setw(8)  << i
              << std::setw(16) << Form(" [CENSORED]")
	      << std::setw(16) << Form("% 9.3f ppb",fBlindTestValues[i]*1e9)
              << std::setw(22) << diff
              << QwLog::endl;
  }
  QwMessage << "================================================" << QwLog::endl;
  QwMessage << "QwBlinder::PrintFinalValues():  End of summary"   << QwLog::endl;
}

void QwBlinder::PrintCountersValues(std::vector<Int_t> fCounters, TString counter_type)
{
  QwMessage << "Blinder Passed " << counter_type  << QwLog::endl;
  QwMessage << "\t" << counter_type 
	    << " with blinding disabled: " << fCounters.at(kBlinderCount_Disabled) << QwLog::endl;
  QwMessage << "\t" << counter_type 
	    << " on a non-blindable target: " << fCounters.at(kBlinderCount_NonBlindable) << QwLog::endl;
  QwMessage << "\t" << counter_type
	    << " with transverse beam: " << fCounters.at(kBlinderCount_Transverse) << QwLog::endl;
  QwMessage << "\t" << counter_type
	    << " on blindable target with beam present: " << fCounters.at(kBlinderCount_Blindable) << QwLog::endl;
  QwMessage << "Blinder Failed " << counter_type  << QwLog::endl;
  QwMessage << "\t" << counter_type
	    << " with unknown target position: " << fCounters.at(kBlinderCount_UnknownTarget) << QwLog::endl;
  QwMessage << "\t" << counter_type 
	    << " with changed target position: " << fCounters.at(kBlinderCount_ChangedTarget) << QwLog::endl;
  QwMessage << "\t" << counter_type 
	    << " with an undefined Wien setting: " << fCounters.at(kBlinderCount_UndefinedWien) << QwLog::endl;
  QwMessage << "\t" << counter_type
	    << " with a changed Wien setting: " << fCounters.at(kBlinderCount_ChangedWien) << QwLog::endl;
  QwMessage << "\t" << counter_type
	    << " with an undefined IHWP setting: " << fCounters.at(kBlinderCount_UndefinedIHWP) << QwLog::endl;
  QwMessage << "\t" << counter_type
	    << " with a changed IHWP setting: " << fCounters.at(kBlinderCount_ChangedIHWP) << QwLog::endl;
  QwMessage << "\t" << counter_type
	    << " on blindable target with no beam: " << fCounters.at(kBlinderCount_NoBeam) << QwLog::endl;
  QwMessage << "\t" << counter_type
	    << " with other blinding failure: " << fCounters.at(kBlinderCount_OtherFailure) << QwLog::endl;

}


/**
 * Write the blinding parameters to the database
 * @param db Database connection
 * @param datatype Datatype
 *
 * For each analyzed run the database contains a digest of the blinding parameters
 * and a number of blinded test entries.
 */
#ifdef __USE_DATABASE__
void QwBlinder::FillDB(QwParityDB *db, TString datatype)
{
  QwDebug << " --------------------------------------------------------------- " << QwLog::endl;
  QwDebug << "                         QwBlinder::FillDB                       " << QwLog::endl;
  QwDebug << " --------------------------------------------------------------- " << QwLog::endl;

  // Get the analysis ID
  UInt_t analysis_id = db->GetAnalysisID();

  // Fill the rows of the QwParitySSQLS::bf_test table
  if (! CheckTestValues()) {
    QwError << "QwBlinder::FillDB():  "
            << "Blinded test values have changed; "
	    << "may be a problem in the analysis!!!"
            << QwLog::endl;
  }

  QwParitySSQLS::bf_test bf_test_row(0);
  std::vector<QwParitySSQLS::bf_test> bf_test_list;
  for (size_t i = 0; i < fTestValues.size(); i++) {
    bf_test_row.bf_test_id = 0;
    bf_test_row.analysis_id = analysis_id;
    bf_test_row.test_number = i;
    bf_test_row.test_value  = fBlindTestValues[i];
    bf_test_list.push_back(bf_test_row);
  }


  // Connect to the database
  db->Connect();

  // Modify the seed_id and bf_checksum in the analysis table
  try {
    // Get the rows of the QwParitySSQLS::analysis table
    mysqlpp::Query query = db->Query();
    query << "select * from analysis where analysis_id = " 
	  << analysis_id;
    std::vector<QwParitySSQLS::analysis> analysis_res;
    QwDebug << "Query: " << query.str() << QwLog::endl;
    query.storein(analysis_res);
    if (analysis_res.size() == 1) {
      QwParitySSQLS::analysis analysis_row_new  = analysis_res[0];
      // Modify the seed_id and bf_checksum
      analysis_row_new.seed_id = fSeedID;
      analysis_row_new.bf_checksum = fChecksum;
      // Update the analysis table
      query.update(analysis_res[0], analysis_row_new);
      QwDebug << "Query: " << query.str() << QwLog::endl;
      query.execute();
    } else {
      QwError << "Unable to update analysis table with blinder information. "
	      << QwLog::endl;
    }
  } catch (const mysqlpp::Exception& err) {
    QwError << err.what() << QwLog::endl;
  }

  // Add the bf_test rows
  try {
    if (bf_test_list.size()) {
      mysqlpp::Query query = db->Query();
      query.insert(bf_test_list.begin(), bf_test_list.end());
      QwDebug << "Query: " << query.str() << QwLog::endl;
      query.execute();
    } else {
      QwMessage << "QwBlinder::FillDB(): No bf_test entries to write." 
		<< QwLog::endl;
    }
  } catch (const mysqlpp::Exception& err) {
    QwError << err.what() << QwLog::endl;
  }

  // Disconnect from database
  db->Disconnect();

}

void QwBlinder::FillErrDB(QwParityDB *db, TString datatype)
{
  QwDebug << " --------------------------------------------------------------- " << QwLog::endl;
  QwDebug << "                     QwBlinder::FillErrDB                        " << QwLog::endl;
  QwDebug << " --------------------------------------------------------------- " << QwLog::endl;
  QwErrDBInterface row;
  std::vector<QwParitySSQLS::general_errors> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  row.SetAnalysisID(analysis_id);
  row.SetDeviceID(kMaxUInt);
  for (size_t index=0; index<kBlinderCount_NumCounters; index++){
    row.SetErrorCodeId(index+20); 
    row.SetN(fPatternCounters.at(index));
    row.AddThisEntryToList( entrylist );
  }

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(entrylist.begin(), entrylist.end());
    query.execute();
  }
  db->Disconnect();

  return;
};
#endif // __USE_DATABASE__


void QwBlinder::SetTargetBlindability(QwBlinder::EQwBlinderStatus status)
{
  fTargetBlindability = status;
  if (fTargetBlindability_firstread == QwBlinder::kIndeterminate
      && fTargetBlindability != QwBlinder::kIndeterminate){
    fTargetBlindability_firstread = fTargetBlindability;
    QwMessage << "QwBlinder:  First set target blindability to " 
	      << fStatusName[fTargetBlindability] << QwLog::endl;
  }
}

void QwBlinder::SetWienState(EQwWienMode wienmode)
{
  fWienMode = wienmode;
  if (fWienMode_firstread == kWienIndeterminate
      && fWienMode != kWienIndeterminate){
    fWienMode_firstread = fWienMode;
    QwMessage << "QwBlinder:  First set Wien state to " 
	      << WienModeName(fWienMode) << QwLog::endl;
  }
}

void QwBlinder::SetIHWPPolarity(Int_t ihwppolarity)
{
  fIHWPPolarity = ihwppolarity;
  if (fIHWPPolarity_firstread == 0 && fIHWPPolarity != 0){
    fIHWPPolarity_firstread = fIHWPPolarity;
    QwMessage << "QwBlinder:  First set IHWP state to " 
	      << fIHWPPolarity << QwLog::endl;
  }
}


QwBlinder::EQwBlinderStatus QwBlinder::CheckBlindability(std::vector<Int_t> &fCounters)
{
  EQwBlinderStatus status = QwBlinder::kBlindableFail;
  if (fBlindingStrategy == kDisabled) {
    status = QwBlinder::kNotBlindable;
    fCounters.at(kBlinderCount_Disabled)++;
  } else if (fTargetBlindability == QwBlinder::kIndeterminate) {
    QwDebug  << "QwBlinder::CheckBlindability:  The target blindability is not determined.  "
	     << "Fail this pattern." << QwLog::endl;
    status = QwBlinder::kBlindableFail;
    fCounters.at(kBlinderCount_UnknownTarget)++;    
  } else if (fTargetBlindability!=fTargetBlindability_firstread
	     && !fTargetPositionForced) {
    QwDebug << "QwBlinder::CheckBlindability:  The target blindability has changed.  "
	    << "Fail this pattern." << QwLog::endl;
    status = QwBlinder::kBlindableFail;
    fCounters.at(kBlinderCount_ChangedTarget)++;
  } else if (fTargetBlindability==kNotBlindable) {
    //  This isn't a blindable target, so don't do anything.
    status = QwBlinder::kNotBlindable;
    fCounters.at(kBlinderCount_NonBlindable)++;
  } else if (fTargetBlindability==kBlindable &&
	     fWienMode != fWienMode_firstread) {
    //  Wien status changed.  Fail
    status = QwBlinder::kBlindableFail;
    fCounters.at(kBlinderCount_ChangedWien)++;
  } else if (fTargetBlindability==kBlindable &&
	     fIHWPPolarity != fIHWPPolarity_firstread) {
    //  IHWP status changed.  Fail
    status = QwBlinder::kBlindableFail;
    fCounters.at(kBlinderCount_ChangedIHWP)++;
  } else if (fTargetBlindability==kBlindable &&
	     fWienMode == kWienIndeterminate) {
    //  Wien status isn't determined.  Fail.
    status = QwBlinder::kBlindableFail;
    fCounters.at(kBlinderCount_UndefinedWien)++;
  } else if (fTargetBlindability==kBlindable &&
	     fIHWPPolarity==0) {
    //  IHWP status isn't determined.  Fail.
    status = QwBlinder::kBlindableFail;
    fCounters.at(kBlinderCount_UndefinedIHWP)++;
  } else if (fTargetBlindability==kBlindable &&
	    (fWienMode == kWienVertTrans || fWienMode == kWienHorizTrans)) {
    //  We don't have longitudinal beam, so don't blind.
    status = QwBlinder::kNotBlindable;
    fCounters.at(kBlinderCount_Transverse)++;
  } else if (fTargetBlindability==kBlindable 
	     && fBeamIsPresent) {
    //  This is a blindable target and the beam is sufficent.
    status = QwBlinder::kBlindable;
    fCounters.at(kBlinderCount_Blindable)++;
  } else if (fTargetBlindability==kBlindable 
	     && (! fBeamIsPresent) ) {
    //  This is a blindable target but there is insufficent beam present
    status = QwBlinder::kNotBlindable;
    fCounters.at(kBlinderCount_NoBeam)++;
  } else {
    QwError << "QwBlinder::CheckBlindability:  The pattern blindability is unclear.  "
	     << "Fail this pattern." << QwLog::endl;
    status = QwBlinder::kBlindableFail;
    fCounters.at(kBlinderCount_OtherFailure)++;
  }
  //
  fBlinderIsOkay = (status != QwBlinder::kBlindableFail);

  return status;
}

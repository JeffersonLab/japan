/**********************************************************\
* File: QwSubsystemArray.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwSubsystemArray.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "VQwHardwareChannel.h"
#include "QwLog.h"
#include "QwParameterFile.h"

//*****************************************************************

/**
 * Create a subsystem array based on the configuration option 'detectors'
 */
QwSubsystemArray::QwSubsystemArray(QwOptions& options, CanContainFn myCanContain)
: fCleanParameter{0,0,0},fEventTypeMask(0x0),fnCanContain(myCanContain)
{
  ProcessOptionsToplevel(options);
  QwParameterFile detectors(fSubsystemsMapFile.c_str());
  QwMessage << "Loading subsystems from " << fSubsystemsMapFile << "." << QwLog::endl;
  LoadSubsystemsFromParameterFile(detectors);
}

/**
 * Copy constructor by reference
 * @param source Source subsystem array
 */
QwSubsystemArray::QwSubsystemArray(const QwSubsystemArray& source)
: MQwPublishable(source),
  fTreeArrayIndex(source.fTreeArrayIndex),
  fCodaRunNumber(source.fCodaRunNumber),
  fCodaSegmentNumber(source.fCodaSegmentNumber),
  fCodaEventNumber(source.fCodaEventNumber),
  fCodaEventType(source.fCodaEventType),
  fEventTypeMask(source.fEventTypeMask),
  fHasDataLoaded(source.fHasDataLoaded),
  fnCanContain(source.fnCanContain),
  fSubsystemsMapFile(source.fSubsystemsMapFile),
  fSubsystemsDisabledByName(source.fSubsystemsDisabledByName),
  fSubsystemsDisabledByType(source.fSubsystemsDisabledByType)
{
  for (size_t i = 0; i < 3; i++)
    fCleanParameter[i] = source.fCleanParameter[i];

  // Make copies of all subsystems rather than copying just the pointers
  for (const_iterator subsys = source.begin(); subsys != source.end(); ++subsys) {
    this->push_back(subsys->get()->Clone());
    // Instruct the subsystem to publish variables
    if (this->back()->PublishInternalValues() == kFALSE) {
      QwError << "Not all variables for " << this->back()->GetName()
             << " could be published!" << QwLog::endl;
    }
  }
}


/**
 * Fill the subsystem array with the contents of a map file
 * @param detectors Map file
 */
void QwSubsystemArray::LoadSubsystemsFromParameterFile(QwParameterFile& detectors)
{
  // This is how this should work
  QwParameterFile* preamble;
  preamble = detectors.ReadSectionPreamble();
  // Process preamble
  QwVerbose << "Preamble:" << QwLog::endl;
  QwVerbose << *preamble << QwLog::endl;
  if (preamble) delete preamble;

  QwParameterFile* section;
  std::string section_name;
  while ((section = detectors.ReadNextSection(section_name))) {

    // Debugging output of configuration section
    QwVerbose << "[" << section_name << "]" << QwLog::endl;
    QwVerbose << *section << QwLog::endl;

    // Determine type and name of subsystem
    std::string subsys_type = section_name;
    std::string subsys_name;
    if (! section->FileHasVariablePair("=","name",subsys_name)) {
      QwError << "No name defined in section for subsystem " << subsys_type << "." << QwLog::endl;
      delete section; section = 0;
      continue;
    }

    // If subsystem type is explicitly disabled
    bool disabled_by_type = false;
    for (size_t i = 0; i < fSubsystemsDisabledByType.size(); i++)
      if (subsys_type == fSubsystemsDisabledByType.at(i))
        disabled_by_type = true;
    if (disabled_by_type) {
      QwWarning << "Subsystem of type " << subsys_type << " disabled." << QwLog::endl;
      delete section; section = 0;
      continue;
    }

    // If subsystem name is explicitly disabled
    bool disabled_by_name = false;
    for (size_t i = 0; i < fSubsystemsDisabledByName.size(); i++)
      if (subsys_name == fSubsystemsDisabledByName.at(i))
        disabled_by_name = true;
    if (disabled_by_name) {
      QwWarning << "Subsystem with name " << subsys_name << " disabled." << QwLog::endl;
      delete section; section = 0;
      continue;
    }

    // Create subsystem
    QwMessage << "Creating subsystem of type " << subsys_type << " "
              << "with name " << subsys_name << "." << QwLog::endl;
    VQwSubsystem* subsys = 0;
    try {
      subsys =
        VQwSubsystemFactory::Create(subsys_type, subsys_name);
    } catch (QwException_TypeUnknown&) {
      QwError << "No support for subsystems of type " << subsys_type << "." << QwLog::endl;
      // Fall-through to next error for more the psychological effect of many warnings
    }
    if (! subsys) {
      QwError << "Could not create subsystem " << subsys_type << "." << QwLog::endl;
      delete section; section = 0;
      continue;
    }

    // If this subsystem cannot be stored in this array
    if (! fnCanContain(subsys)) {
      QwMessage << "Subsystem " << subsys_name << " cannot be stored in this "
                << "subsystem array." << QwLog::endl;
      QwMessage << "Deleting subsystem " << subsys_name << " again" << QwLog::endl;
      delete section; section = 0;
      delete subsys; subsys = 0;
      continue;
    }

    // Pass detector maps
    subsys->LoadDetectorMaps(*section);
    // Add to array
    this->push_back(subsys);

    // Instruct the subsystem to publish variables
    if (subsys->PublishInternalValues() == kFALSE) {
      QwError << "Not all variables for " << subsys->GetName()
              << " could be published!" << QwLog::endl;
    }

    // Delete parameter file section
    delete section; section = 0;
  }
}

//*****************************************************************

/**
 * Add the subsystem to this array.  Do nothing if the subsystem is null or if
 * there is already a subsystem with that name in the array.
 * @param subsys Subsystem to add to the array
 */
void QwSubsystemArray::push_back(VQwSubsystem* subsys)
{
  if (subsys == NULL) {
    QwError << "QwSubsystemArray::push_back(): NULL subsys"
            << QwLog::endl;
    //  This is an empty subsystem...
    //  Do nothing for now.

  } else if (!this->empty() && GetSubsystemByName(subsys->GetName())){
    //  There is already a subsystem with this name!
    QwError << "QwSubsystemArray::push_back(): subsys " << subsys->GetName()
            << " already exists" << QwLog::endl;

  } else if (!fnCanContain(subsys)) {
    //  There is no support for this type of subsystem
    QwError << "QwSubsystemArray::push_back(): subsys " << subsys->GetName()
            << " is not supported by this subsystem array" << QwLog::endl;

  } else {
    boost::shared_ptr<VQwSubsystem> subsys_tmp(subsys);
    SubsysPtrs::push_back(subsys_tmp);

    // Set the parent of the subsystem to this array
    subsys_tmp->SetParent(this);

    // Update the event type mask
    // Note: Active bits in the mask indicate event types that are accepted
    fEventTypeMask |= subsys_tmp->GetEventTypeMask();
  }
}



/**
 * Define configuration options for global array
 * @param options Options
 */
void QwSubsystemArray::DefineOptions(QwOptions &options)
{
  options.AddOptions()("detectors",
                       po::value<std::string>()->default_value("detectors.map"),
                       "map file with detectors to include");

  options.AddOptions()("bad-event-list",
                       po::value<std::string>()->default_value(""),
                       "map file with bad event ranges");

  // Versions of boost::program_options below 1.39.0 have a bug in multitoken processing
#if BOOST_VERSION < 103900
  options.AddOptions()("disable-by-type",
                       po::value<std::vector <std::string> >(),
                       "subsystem types to disable");
  options.AddOptions()("disable-by-name",
                       po::value<std::vector <std::string> >(),
                       "subsystem names to disable");
#else // BOOST_VERSION >= 103900
  options.AddOptions()("disable-by-type",
                       po::value<std::vector <std::string> >()->multitoken(),
                       "subsystem types to disable");
  options.AddOptions()("disable-by-name",
                       po::value<std::vector <std::string> >()->multitoken(),
                       "subsystem names to disable");
#endif // BOOST_VERSION
}


/**
 * Handle configuration options for the subsystem array itself
 * @param options Options
 */
void QwSubsystemArray::ProcessOptionsToplevel(QwOptions &options)
{
  // Filename to use for subsystem creation (single filename could be expanded
  // to a list)
  fSubsystemsMapFile = options.GetValue<std::string>("detectors");
  // Subsystems to disable
  fSubsystemsDisabledByName = options.GetValueVector<std::string>("disable-by-name");
  fSubsystemsDisabledByType = options.GetValueVector<std::string>("disable-by-type");
}


/**
 * Handle configuration options for all subsystems in the array
 * @param options Options
 */
void QwSubsystemArray::ProcessOptionsSubsystems(QwOptions &options)
{
  LoadAllEventRanges(options);

  for (iterator subsys_iter = begin(); subsys_iter != end(); ++subsys_iter) {
    VQwSubsystem* subsys = dynamic_cast<VQwSubsystem*>(subsys_iter->get());
    subsys->ProcessOptions(options);
  }
}

void QwSubsystemArray::LoadAllEventRanges(QwOptions &options){

  std::string fBadEventListFileName = options.GetValue<std::string>("bad-event-list");
  if (fBadEventListFileName.size() > 0) {
    QwParameterFile fBadEventListFile(fBadEventListFileName);
    // If there is an event list, open the next section
    std::string bad_event_range;
    while (fBadEventListFile.ReadNextLine(bad_event_range)){
      // Find next non-whitespace, non-comment, non-empty line, before EOF
      fBadEventListFile.TrimWhitespace();
      fBadEventListFile.TrimComment('#');
      if (fBadEventListFile.LineIsEmpty()) continue;
      std::pair<UInt_t,UInt_t> aBadEventRange = QwParameterFile::ParseIntRange(":",bad_event_range);
      fBadEventRange.push_back(aBadEventRange);
      QwMessage << "Next Bad event range is " << bad_event_range << QwLog::endl;
    } // end of loop of reading lines.
  }
}

/**
 * Get the subsystem in this array with the spcified name
 * @param name Name of the subsystem
 * @return Pointer to the subsystem
 */
VQwSubsystem* QwSubsystemArray::GetSubsystemByName(const TString& name)
{
  VQwSubsystem* tmp = NULL;
  if (!empty()) {
    // Loop over the subsystems
    for (const_iterator subsys = begin(); subsys != end(); ++subsys) {
      // Check the name of this subsystem
      // std::cout<<"QwSubsystemArray::GetSubsystemByName available name=="<<(*subsys)->GetName()<<"== to be compared to =="<<name<<"==\n";
      if ((*subsys)->GetName() == name) {
        tmp = (*subsys).get();
        //std::cout<<"QwSubsystemArray::GetSubsystemByName found a matching name \n";
      } else {
        // nothing
      }
    }
  }
  return tmp;
}


/**
 * Get the list of subsystems in this array of the specified type
 * @param type Type of the subsystem
 * @return Vector of subsystems
 */
std::vector<VQwSubsystem*> QwSubsystemArray::GetSubsystemByType(const std::string& type)
{
  // Vector of subsystem pointers
  std::vector<VQwSubsystem*> subsys_list;

  // If this array is not empty
  if (!empty()) {

    // Loop over the subsystems
    for (const_iterator subsys = begin(); subsys != end(); ++subsys) {

      // Test to see if the subsystem inherits from the required type
      if (VQwSubsystemFactory::InheritsFrom((*subsys).get(),type)) {
        subsys_list.push_back((*subsys).get());
      }

    } // end of loop over subsystems

  } // end of if !empty()

  return subsys_list;
}


void  QwSubsystemArray::ClearEventData()
{
  if (!empty()) {
    SetDataLoaded(kFALSE);
    SetCodaEventNumber(0);
    SetCodaEventType(0);
    std::for_each(begin(), end(),
		  boost::mem_fn(&VQwSubsystem::ClearEventData));
  }
}

Int_t QwSubsystemArray::ProcessConfigurationBuffer(
  const ROCID_t roc_id,
  const BankID_t bank_id,
  UInt_t* buffer,
  UInt_t num_words)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ProcessConfigurationBuffer(roc_id, bank_id, buffer, num_words);
    }
  return 0;
}

///  QwSubsystemArray::GetMarkerWordList should be called once by QwEventBuffer
///  to build the marker word list as each roc_id and bank_id are reached in
///  the decoding.
void  QwSubsystemArray::GetMarkerWordList(
  const ROCID_t roc_id,
  const BankID_t bank_id,
  std::vector<UInt_t>& marker) const
{
  if (!empty()){
    for (const_iterator subsys = begin(); subsys != end(); ++subsys) {
      (*subsys)->GetMarkerWordList(roc_id, bank_id, marker);
    }
  }
}


Int_t QwSubsystemArray::ProcessEvBuffer(
  const UInt_t event_type,
  const ROCID_t roc_id,
  const BankID_t bank_id,
  UInt_t* buffer,
  UInt_t num_words)
{
  if (!empty()) {
    SetDataLoaded(kTRUE);
    for (iterator subsys = begin(); subsys != end(); ++subsys) {
      (*subsys)->ProcessEvBuffer(event_type, roc_id, bank_id, buffer, num_words);
    }
  }
  return 0;
}


void  QwSubsystemArray::ProcessEvent()
{
  if (!empty() && HasDataLoaded()) {
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::ProcessEvent));
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::ExchangeProcessedData));
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::ProcessEvent_2));
  }
}

void  QwSubsystemArray::AtEndOfEventLoop()
{
  QwDebug << "QwSubsystemArray at end of event loop" << QwLog::endl;
  if (!empty()) {
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::AtEndOfEventLoop));
  }
}

//*****************************************************************
void  QwSubsystemArray::RandomizeEventData(int helicity, double time)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys) {
      (*subsys)->RandomizeEventData(helicity, time);
    }
}

//*****************************************************************
void  QwSubsystemArray::EncodeEventData(std::vector<UInt_t> &buffer)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys) {
      (*subsys)->EncodeEventData(buffer);
    }
}


//*****************************************************************
void  QwSubsystemArray::ConstructObjects(TDirectory *folder, TString &prefix)
{
  if (!empty()) {
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ConstructObjects(folder,prefix);
    }
  }
}

//*****************************************************************
void  QwSubsystemArray::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (!empty()) {
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ConstructHistograms(folder,prefix);
    }
  }
}

void  QwSubsystemArray::FillHistograms()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::FillHistograms));
}

void  QwSubsystemArray::ShareHistograms(const QwSubsystemArray& source)
{
  if (!empty() && !source.empty()) {
    if (this->size() == source.size()) {
      for (size_t i = 0; i < source.size(); ++i) {
        this->at(i)->ShareHistograms(source.at(i).get());
      }
    }
  }
}
//*****************************************************************

/**
 * Construct the tree for this subsystem
 * @param folder Directory where to construct the tree
 * @param prefix Prefix for the name of the tree
 */
void  QwSubsystemArray::ConstructTree(TDirectory *folder, TString &prefix)
{
  if (!empty()) {
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ConstructTree(folder, prefix);
    }
  }
}

/**
 * Fill the tree for this subsystem
 */
void  QwSubsystemArray::FillTree()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::FillTree));
}

/**
 * Delete the tree for this subsystem
 */
void  QwSubsystemArray::DeleteTree()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::DeleteTree));
}

//*****************************************************************

void  QwSubsystemArray::PrintInfo() const
{
  if (!empty()) {
    for (const_iterator subsys = begin(); subsys != end(); ++subsys) {
      (*subsys)->PrintInfo();
    }
  }
}

//*****************************************************************

/**
 * Construct the branch and tree vector
 * @param tree Tree
 * @param prefix Prefix
 * @param values Vector of values
 */
void  QwSubsystemArray::ConstructBranchAndVector(
        TTree *tree,
        TString& prefix,
        std::vector<Double_t>& values)
{
  fTreeArrayIndex = values.size();

  // Each tree should only contain event number and type once, but will
  // still reserve space in the values vector, so we don't need to modify
  // FillTreeVector().
  values.push_back(0.0);
  values.push_back(0.0);
  values.push_back(0.0);
  values.push_back(0.0);
  values.push_back(0.0);
  if (prefix == "" || prefix == "yield_") {
    tree->Branch("CodaEventNumber",&(values[fTreeArrayIndex]),"CodaEventNumber/D");
    tree->Branch("CodaEventType",&(values[fTreeArrayIndex+1]),"CodaEventType/D");
    tree->Branch("Coda_CleanData",&(values[fTreeArrayIndex+2]),"Coda_CleanData/D");
    tree->Branch("Coda_ScanData1",&(values[fTreeArrayIndex+3]),"Coda_ScanData1/D");
    tree->Branch("Coda_ScanData2",&(values[fTreeArrayIndex+4]),"Coda_ScanData2/D");
  }
  
  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystem* subsys_ptr = dynamic_cast<VQwSubsystem*>(subsys->get());
    subsys_ptr->ConstructBranchAndVector(tree, prefix, values);
  }

}


/**
 * Construct the branch for the flat tree
 * @param tree Tree
 * @param prefix Prefix
 */
void QwSubsystemArray::ConstructBranch(TTree *tree, TString& prefix)
{
  // Only MPS tree should contain event number and type
  if (prefix == "" || prefix == "yield_") {
    tree->Branch("CodaEventNumber",&fCodaEventNumber,"CodaEventNumber/I");
    tree->Branch("CodaEventType",&fCodaEventType,"CodaEventType/I");
  }

  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystem* subsys_ptr = dynamic_cast<VQwSubsystem*>(subsys->get());
    subsys_ptr->ConstructBranch(tree, prefix);
  }
}


/**
 * Construct the branch for the flat tree with tree trim files accepted
 * @param tree Tree
 * @param prefix Prefix
 * @param trim_file Trim file
 */
void QwSubsystemArray::ConstructBranch(
        TTree *tree,
        TString& prefix,
        QwParameterFile& trim_file)
{
  QwMessage << " QwSubsystemArray::ConstructBranch " << QwLog::endl;

  QwParameterFile* preamble;
  QwParameterFile* nextsection;
  preamble = trim_file.ReadSectionPreamble();

  // Process preamble
  QwVerbose << "QwSubsystemArrayTracking::ConstructBranch  Preamble:" << QwLog::endl;
  QwVerbose << *preamble << QwLog::endl;

  if (prefix == "" || prefix == "yield_") {
    tree->Branch("CodaEventNumber",&fCodaEventNumber,"CodaEventNumber/I");
    tree->Branch("CodaEventType",&fCodaEventType,"CodaEventType/I");
  }

  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystem* subsys_ptr = dynamic_cast<VQwSubsystem*>(subsys->get());

    TString subsysname = subsys_ptr->GetName();
    //QwMessage << "Tree leaves created for " << subsysname << QwLog::endl;

    if (trim_file.FileHasSectionHeader(subsysname)) {
      // This section contains modules and their channels to be included in the tree
      nextsection = trim_file.ReadUntilNextSection();
      subsys_ptr->ConstructBranch(tree, prefix, *nextsection);
      QwMessage << "Tree leaves created for " << subsysname << QwLog::endl;
    } else
      QwMessage << "No tree leaves created for " << subsysname << QwLog::endl;
  }
}


/**
 * Fill the tree vector
 * @param values Vector of values
 */
void QwSubsystemArray::FillTreeVector(std::vector<Double_t>& values) const
{
  // Fill the event number and event type
  size_t index = fTreeArrayIndex;
  values[index++] = this->GetCodaEventNumber();
  values[index++] = this->GetCodaEventType();
  values[index++] = this->fCleanParameter[0];
  values[index++] = this->fCleanParameter[1];
  values[index++] = this->fCleanParameter[2];


  // Fill the subsystem data
  for (const_iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystem* subsys_ptr = dynamic_cast<VQwSubsystem*>(subsys->get());
    subsys_ptr->FillTreeVector(values);
  }
}




// TList* QwSubsystemArray::GetParamFileNameList(TString name) const
// {
//   if (not empty()) {

//     TList* return_maps_TList = new TList;
//     return_maps_TList->SetOwner(true);
//     return_maps_TList->SetName(name);
    
//     std::vector<TString> mapfiles_vector_subsystem;

//     Int_t num_of_mapfiles_subsystem = 0;

//     for (const_iterator subsys = begin(); subsys != end(); ++subsys) 
//       {
// 	(*subsys)->PrintDetectorMaps(true);
// 	mapfiles_vector_subsystem = (*subsys)->GetParamFileNameList();
// 	num_of_mapfiles_subsystem = (Int_t) mapfiles_vector_subsystem.size();
	
// 	for (Int_t i=0; i<num_of_mapfiles_subsystem; i++) 
// 	  {
// 	    return_maps_TList -> AddLast(new TObjString(mapfiles_vector_subsystem[i]));
// 	  }
	
// 	mapfiles_vector_subsystem.clear();
//       }
//     return return_maps_TList;
//   }
//   else {
//     return NULL;
//   }
// };


void QwSubsystemArray::PrintParamFileList() const
{
  if (not empty()) {
    for (const_iterator subsys = begin(); subsys != end(); ++subsys)
      {
        (*subsys)->PrintDetectorMaps(true);
      }
  }
}

TList* QwSubsystemArray::GetParamFileNameList(TString name) const
{
  if (not empty()) {

    TList* return_maps_TList = new TList;
    return_maps_TList->SetName(name);

    std::map<TString, TString> mapfiles_subsystem;

    for (const_iterator subsys = begin(); subsys != end(); ++subsys) 
      {
	mapfiles_subsystem = (*subsys)->GetDetectorMaps();
	for( std::map<TString, TString>::iterator ii= mapfiles_subsystem.begin(); ii!= mapfiles_subsystem.end(); ++ii)
	  {	
	    TList *test = new TList;
	    test->SetName((*ii).first);
	    test->AddLast(new TObjString((*ii).second));
	    return_maps_TList -> AddLast(test);
	  }
      }

    return return_maps_TList;
  }
  else {
    return NULL;
  }
};



/**
 * Add the subsystem to this array.  Do nothing if the subsystem is null or if
 * there is already a subsystem with that name in the array.
 * @param subsys Subsystem to add to the array
 */
void QwSubsystemArray::push_back(boost::shared_ptr<VQwSubsystem> subsys)
{
  
 if (subsys.get() == NULL) {
   QwError << "QwSubsystemArray::push_back(): NULL subsys"
           << QwLog::endl;
   //  This is an empty subsystem...
   //  Do nothing for now.

 } else if (!this->empty() && GetSubsystemByName(subsys->GetName())){
   //  There is already a subsystem with this name!
   QwError << "QwSubsystemArray::push_back(): subsys " << subsys->GetName()
           << " already exists" << QwLog::endl;

 } else if (!fnCanContain(subsys.get())) {
   //  There is no support for this type of subsystem
   QwError << "QwSubsystemArray::push_back(): subsys " << subsys->GetName()
           << " is not supported by this subsystem array" << QwLog::endl;

 } else {
   boost::shared_ptr<VQwSubsystem> subsys_tmp(subsys);
   SubsysPtrs::push_back(subsys_tmp);

   // Set the parent of the subsystem to this array
   subsys_tmp->SetParent(this);

   // Update the event type mask
   // Note: Active bits in the mask indicate event types that are accepted
   fEventTypeMask |= subsys_tmp->GetEventTypeMask();

   // Instruct the subsystem to publish variables
   if (subsys_tmp->PublishInternalValues() == kFALSE) {
     QwError << "Not all variables for " << subsys_tmp->GetName()
             << " could be published!" << QwLog::endl;
   }
 }
}
  
  
  
  
  

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

/// \todo TODO (wdc) QwVQWK_Channel necessary due to explicit cast in ReturnInternalValue (yuck)
#include "QwVQWK_Channel.h"

//*****************************************************************

/**
 * Create a subsystem array based on the configuration option 'detectors'
 */
QwSubsystemArray::QwSubsystemArray(QwOptions& options, CanContainFn myCanContain)
: fEventTypeMask(0x0),fnCanContain(myCanContain)
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
: fTreeArrayIndex(source.fTreeArrayIndex),
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
  fPublishedValuesDataElement.clear();
  fPublishedValuesSubsystem.clear();
  fPublishedValuesDescription.clear();

  // Make copies of all subsystems rather than copying just the pointers
  for (const_iterator subsys = source.begin(); subsys != source.end(); ++subsys) {
    this->push_back(subsys->get()->Clone());
    // Instruct the subsystem to publish variables
    if (this->back()->PublishInternalValues() == kFALSE) {
      QwError << "Not all variables for " << this->back()->GetSubsystemName()
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
      QwError << "Not all variables for " << subsys->GetSubsystemName()
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

  } else if (!this->empty() && GetSubsystemByName(subsys->GetSubsystemName())){
    //  There is already a subsystem with this name!
    QwError << "QwSubsystemArray::push_back(): subsys " << subsys->GetSubsystemName()
            << " already exists" << QwLog::endl;

  } else if (!fnCanContain(subsys)) {
    //  There is no support for this type of subsystem
    QwError << "QwSubsystemArray::push_back(): subsys " << subsys->GetSubsystemName()
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
  for (iterator subsys_iter = begin(); subsys_iter != end(); ++subsys_iter) {
    VQwSubsystem* subsys = dynamic_cast<VQwSubsystem*>(subsys_iter->get());
    subsys->ProcessOptions(options);
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
      // std::cout<<"QwSubsystemArray::GetSubsystemByName available name=="<<(*subsys)->GetSubsystemName()<<"== to be compared to =="<<name<<"==\n";
      if ((*subsys)->GetSubsystemName() == name) {
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
  const UInt_t roc_id,
  const UInt_t bank_id,
  UInt_t* buffer,
  UInt_t num_words)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ProcessConfigurationBuffer(roc_id, bank_id, buffer, num_words);
    }
  return 0;
}

Int_t QwSubsystemArray::ProcessEvBuffer(
  const UInt_t event_type,
  const UInt_t roc_id,
  const UInt_t bank_id,
  UInt_t* buffer,
  UInt_t num_words)
{
  if (!empty())
    SetDataLoaded(kTRUE);
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ProcessEvBuffer(event_type, roc_id, bank_id, buffer, num_words);
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

    TString subsysname = subsys_ptr->GetSubsystemName();
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

//*****************************************************************

/**
 * Retrieve the variable name from other subsystem arrays
 * @param name Variable name to be retrieved
 * @param value (return) Data element with the variable name
 * @return True if the variable is found, false if not found
 */
Bool_t QwSubsystemArray::RequestExternalValue(const TString& name, VQwHardwareChannel* value) const
{
  //  If this has a parent, we should escalate the call to that object,
  //  but so far we don't have that capability.
  return ReturnInternalValue(name, value);
}

/**
 * Retrieve the variable name from subsystems in this subsystem array
 * @param name Variable name to be retrieved
 * @return Data element with the variable name, null if not found
 */
const VQwHardwareChannel* QwSubsystemArray::ReturnInternalValue(const TString& name) const
{
  //  First try to find the value in the list of published values.
  std::map<TString, const VQwHardwareChannel*>::const_iterator iter1 =
      fPublishedValuesDataElement.find(name);
  if (iter1 != fPublishedValuesDataElement.end()) {
    return iter1->second;
  }
  //  Second, ask the subsystem that has claimed the value
  std::map<TString, const VQwSubsystem*>::const_iterator iter2 =
      fPublishedValuesSubsystem.find(name);
  if (iter2 != fPublishedValuesSubsystem.end()) {
    return (iter2->second)->ReturnInternalValue(name);
  }
  //  If the value is not yet published, try requesting it.
  if (const_cast<QwSubsystemArray*>(this)->PublishByRequest(name)){
    iter1 = fPublishedValuesDataElement.find(name);
    if (iter1 != fPublishedValuesDataElement.end()) {
      return iter1->second;
    }
    QwError << "PublishByRequest succeeded, but can't find the record for "
	    << name << QwLog::endl;
    
  } else {
    QwError << "PublishByRequest failed for " << name << QwLog::endl;
  }
  //  Not found
  return 0;
}

/**
 * Retrieve the variable name from subsystems in this subsystem array
 * @param name Variable name to be retrieved
 * @param value (return) Data element with the variable name
 * @return True if the variable was found, false if not found
 */
Bool_t QwSubsystemArray::ReturnInternalValue(const TString& name, VQwHardwareChannel* value) const
{
  Bool_t foundit = kFALSE;

  // Check for null pointer
  if (! value)
    QwWarning << "QwSubsystemArray::ReturnInternalValue requires that "
              << "'value' be a non-null pointer to a VQwHardwareChannel."
              << QwLog::endl;

  //  Get a const pointer to the internal value
  VQwHardwareChannel* internal_value = const_cast<VQwHardwareChannel*>(ReturnInternalValue(name));
  if (value && internal_value && typeid(value) == typeid(internal_value)) {
    /// \todo TODO (wdc) Remove this ugly statement by redefining
    ///       QwVQWK_Channel::operator= to accept any VQwHardwareChannel.
    //*(dynamic_cast<QwVQWK_Channel*>(value)) = *(dynamic_cast<QwVQWK_Channel*>(internal_value));
    value->AssignValueFrom(internal_value);
    foundit = kTRUE;
  } else
    QwWarning << "QwSubsystemArray::ReturnInternalValue: name \""
              << name << "\" not found in array." << QwLog::endl;

  return foundit;
}

/**
 * Publish the value name with description from a subsystem in this array
 * @param name Name of the variable
 * @param desc Description of the variable
 * @param subsys Subsystem that contains the variable
 * @param element Data element that contains the variable
 * @return True if the variable could be published, false if not published
 */
Bool_t QwSubsystemArray::PublishInternalValue(
    const TString name,
    const TString desc,
    const VQwSubsystem* subsys,
    const VQwHardwareChannel* element)
{
  if (fPublishedValuesSubsystem.count(name) > 0) {
    QwError << "Attempting to publish existing variable key!" << QwLog::endl;
    ListPublishedValues();
    return kFALSE;
  }
  fPublishedValuesSubsystem[name] = subsys;
  fPublishedValuesDescription[name] = desc;
  fPublishedValuesDataElement[name] = element;
  return kTRUE;
}

/**
 * Try to publish an internal variable matching the submitted name
 * @param device_name Name of the desired published variable
 * @return True if the variable could be published, false if not published
 */
Bool_t QwSubsystemArray::PublishByRequest(TString device_name){
  Bool_t status = kFALSE;
  if (fPublishedValuesSubsystem.count(device_name) > 0) {
    QwError << "QwSubsystemArray::PublishByRequest:  Channel "
	    << device_name << " has already been published."
	    << QwLog::endl;
    ListPublishedValues();
    status = kTRUE;
  } else if (not empty()) {
    for (iterator subsys = begin(); subsys != end(); ++subsys)
      {
        status = (*subsys)->PublishByRequest(device_name);
	if (status) break;
      }
    // Report failure to publish
    if (! status) {
      QwError << "QwSubsystemArray::PublishByRequest: Failed to publish channel name: "
              << device_name << QwLog::endl;
    }
  }
  return status;
}


/**
 * List the published values and description in this subsystem array
 */
void QwSubsystemArray::ListPublishedValues() const
{
  QwOut << "List of published values:" << QwLog::endl;
  std::map<TString,TString>::const_iterator iter;
  for (iter  = fPublishedValuesDescription.begin();
       iter != fPublishedValuesDescription.end(); iter++) {
    QwOut << iter->first << ": " << iter->second << QwLog::endl;
  }
}

/**
 * Retrieve the variable name from subsystems in this subsystem array
 * @param name Variable name to be retrieved
 * @return Data element with the variable name
 */
VQwHardwareChannel* QwSubsystemArray::ReturnInternalValueForFriends(const TString& name) const
{
  return const_cast<VQwHardwareChannel*>(ReturnInternalValue(name));
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

 } else if (!this->empty() && GetSubsystemByName(subsys->GetSubsystemName())){
   //  There is already a subsystem with this name!
   QwError << "QwSubsystemArray::push_back(): subsys " << subsys->GetSubsystemName()
           << " already exists" << QwLog::endl;

 } else if (!fnCanContain(subsys.get())) {
   //  There is no support for this type of subsystem
   QwError << "QwSubsystemArray::push_back(): subsys " << subsys->GetSubsystemName()
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
     QwError << "Not all variables for " << subsys_tmp->GetSubsystemName()
             << " could be published!" << QwLog::endl;
   }
 }
}
  
  
  
  
  

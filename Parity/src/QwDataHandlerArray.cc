/**********************************************************\
* File: QwDataHandlerArray.cc                         *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwDataHandlerArray.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "VQwDataHandler.h"
#include "QwParameterFile.h"
#include "QwHelicityPattern.h"

//*****************************************************************//
/**
 * Create a handler array based on the configuration option 'detectors'
 */
QwDataHandlerArray::QwDataHandlerArray(QwOptions& options, QwHelicityPattern& helicitypattern, const TString &run)
  : fHelicityPattern(0),fSubsystemArray(0),fDataHandlersMapFile(""),fArrayScope(kPatternScope)
{
  ProcessOptions(options);
  if (fDataHandlersMapFile != ""){
    QwParameterFile mapfile(fDataHandlersMapFile.c_str());
    QwMessage << "Loading handlers from " << fDataHandlersMapFile << "." << QwLog::endl;
    LoadDataHandlersFromParameterFile(mapfile, helicitypattern, run);
  }
}

/**
 * Create a handler array based on the configuration option 'detectors'
 */
QwDataHandlerArray::QwDataHandlerArray(QwOptions& options, QwSubsystemArrayParity& detectors, const TString &run)
  : fHelicityPattern(0),fSubsystemArray(0),fDataHandlersMapFile(""),fArrayScope(kEventScope)
{
  ProcessOptions(options);
  if (fDataHandlersMapFile != ""){
    QwParameterFile mapfile(fDataHandlersMapFile.c_str());
    QwMessage << "Loading handlers from " << fDataHandlersMapFile << "." << QwLog::endl;
    LoadDataHandlersFromParameterFile(mapfile, detectors, run);
  }
}

/**
 * Copy constructor by reference
 * @param source Source handler array
 */
QwDataHandlerArray::QwDataHandlerArray(const QwDataHandlerArray& source)
: fHelicityPattern(source.fHelicityPattern),
  fSubsystemArray(source.fSubsystemArray),
  fDataHandlersMapFile(source.fDataHandlersMapFile),
  fDataHandlersDisabledByName(source.fDataHandlersDisabledByName),
  fDataHandlersDisabledByType(source.fDataHandlersDisabledByType)
{
  // Make copies of all handlers rather than copying just the pointers
  for (const_iterator handler = source.begin(); handler != source.end(); ++handler) {
    this->push_back(handler->get()->Clone());
    /*
    // Instruct the handler to publish variables
    if (this->back()->PublishInternalValues() == kFALSE) {
      QwError << "Not all variables for " << this->back()->GetName()
             << " could be published!" << QwLog::endl;
    */
  }
}



//*****************************************************************//

/// Destructor
QwDataHandlerArray::~QwDataHandlerArray()
{
  // nothing
}

/**
 * Fill the handler array with the contents of a map file
 * @param detectors Map file
 */
template<class T>
void QwDataHandlerArray::LoadDataHandlersFromParameterFile(
    QwParameterFile& mapfile,
    T& detectors,
    const TString &run)
{
  // Set pointer to this object
  SetPointer(detectors);

  // This is how this should work
  QwParameterFile* preamble;
  preamble = mapfile.ReadSectionPreamble();
  // Process preamble
  QwVerbose << "Preamble:" << QwLog::endl;
  QwVerbose << *preamble << QwLog::endl;
  if (preamble) delete preamble;

  QwParameterFile* section;
  std::string section_name;
  while ((section = mapfile.ReadNextSection(section_name))) {

    // Debugging output of configuration section
    QwVerbose << "[" << section_name << "]" << QwLog::endl;
    QwVerbose << *section << QwLog::endl;

    // Determine type and name of handler
    std::string handler_type = section_name;
    std::string handler_name;
    std::string handler_scope;
    if (! section->FileHasVariablePair("=","name",handler_name)) {
      QwError << "No name defined in section for handler " << handler_type << "." << QwLog::endl;
      delete section; section = 0;
      continue;
    }
    if (section->FileHasVariablePair("=","scope",handler_scope)) {
      if (ScopeMismatch(handler_scope)) continue;
    } else {
      //  Assume the scope of a handler without a scope specifier is
      //  "pattern".
      if (fArrayScope != kPatternScope) continue;
    }

    // If handler type is explicitly disabled
    bool disabled_by_type = false;
    for (size_t i = 0; i < fDataHandlersDisabledByType.size(); i++)
      if (handler_type == fDataHandlersDisabledByType.at(i))
        disabled_by_type = true;
    if (disabled_by_type) {
      QwWarning << "DataHandler of type " << handler_type << " disabled." << QwLog::endl;
      delete section; section = 0;
      continue;
    }

    // If handler name is explicitly disabled
    bool disabled_by_name = false;
    for (size_t i = 0; i < fDataHandlersDisabledByName.size(); i++)
      if (handler_name == fDataHandlersDisabledByName.at(i))
        disabled_by_name = true;
    if (disabled_by_name) {
      QwWarning << "DataHandler with name " << handler_name << " disabled." << QwLog::endl;
      delete section; section = 0;
      continue;
    }

    // Create handler
    QwMessage << "Creating handler of type " << handler_type << " "
              << "with name " << handler_name << "." << QwLog::endl;
    VQwDataHandler* handler = 0;
    
    try {
      handler =
        VQwDataHandlerFactory::Create(handler_type, handler_name);
    } catch (QwException_TypeUnknown&) {
      QwError << "No support for handlers of type " << handler_type << "." << QwLog::endl;
      // Fall-through to next error for more the psychological effect of many warnings
    }
    if (! handler) {
      QwError << "Could not create handler " << handler_type << "." << QwLog::endl;
      delete section; section = 0;
      continue;
    }

    // If this handler cannot be stored in this array
    if (! CanContain(handler)) {
      QwMessage << "DataHandler " << handler_name << " cannot be stored in this "
                << "handler array." << QwLog::endl;
      QwMessage << "Deleting handler " << handler_name << " again" << QwLog::endl;
      delete section; section = 0;
      delete handler; handler = 0;
      continue;
    }

    // Pass detector maps
    handler->SetParent(this);
    handler->SetRunLabel(run);
    handler->SetPointer(&detectors);
    handler->ParseConfigFile(*section);
    handler->LoadChannelMap();
    handler->ConnectChannels(detectors);
    handler->InitRunningSum();

    // Add to array
    this->push_back(handler);
    /*    
    // Instruct the handler to publish variables
    if (handler->PublishInternalValues() == kFALSE) {
      QwError << "Not all variables for " << handler->GetName()
              << " could be published!" << QwLog::endl;
    }
    */
    // Delete parameter file section
    delete section; section = 0;
  }
}
//*****************************************************************

/**
 * Add the handler to this array.  Do nothing if the handler is null or if
 * there is already a handler with that name in the array.
 * @param handler DataHandler to add to the array
 */
void QwDataHandlerArray::push_back(VQwDataHandler* handler)
{
  if (handler == NULL) {
    QwError << "QwDataHandlerArray::push_back(): NULL handler"
            << QwLog::endl;
    //  This is an empty handler...
    //  Do nothing for now.

  } else if (!this->empty() && GetDataHandlerByName(handler->GetName())){
    //  There is already a handler with this name!
    QwError << "QwDataHandlerArray::push_back(): handler " << handler->GetName()
            << " already exists" << QwLog::endl;

  } else if (!CanContain(handler)) {
    //  There is no support for this type of handler
    QwError << "QwDataHandlerArray::push_back(): handler " << handler->GetName()
            << " is not supported by this handler array" << QwLog::endl;

  } else {
    boost::shared_ptr<VQwDataHandler> handler_tmp(handler);
    HandlerPtrs::push_back(handler_tmp);

    // Set the parent of the handler to this array
    //    handler_tmp->SetParent(this);

  }
}

/**
 * Define configuration options for global array
 * @param options Options
 */
void QwDataHandlerArray::DefineOptions(QwOptions &options)
{
  options.AddOptions()("datahandlers",
                       po::value<std::string>(),
                       "map file with datahandlers to include");

  // Versions of boost::program_options below 1.39.0 have a bug in multitoken processing
#if BOOST_VERSION < 103900
  options.AddOptions()("DataHandler.disable-by-type",
                       po::value<std::vector <std::string> >(),
                       "handler types to disable");
  options.AddOptions()("DataHandler.disable-by-name",
                       po::value<std::vector <std::string> >(),
                       "handler names to disable");
#else // BOOST_VERSION >= 103900
  options.AddOptions()("DataHandler.disable-by-type",
                       po::value<std::vector <std::string> >()->multitoken(),
                       "handler types to disable");
  options.AddOptions()("DataHandler.disable-by-name",
                       po::value<std::vector <std::string> >()->multitoken(),
                       "handler names to disable");
#endif // BOOST_VERSION
}


/**
 * Handle configuration options for the handler array itself
 * @param options Options
 */
void QwDataHandlerArray::ProcessOptions(QwOptions &options)
{
  // Filename to use for handler creation (single filename could be expanded
  // to a list)
  if (options.HasValue("datahandlers")){
    fDataHandlersMapFile = options.GetValue<std::string>("datahandlers");
  }
  // DataHandlers to disable
  fDataHandlersDisabledByName = options.GetValueVector<std::string>("DataHandler.disable-by-name");
  fDataHandlersDisabledByType = options.GetValueVector<std::string>("DataHandler.disable-by-type");

  //  Get the globally defined print running sum flag
  fPrintRunningSum = options.GetValue<bool>("print-runningsum");
}

/**
 * Get the handler in this array with the spcified name
 * @param name Name of the handler
 * @return Pointer to the handler
 */
VQwDataHandler* QwDataHandlerArray::GetDataHandlerByName(const TString& name)
{
  VQwDataHandler* tmp = NULL;
  if (!empty()) {
    // Loop over the handlers
    for (const_iterator handler = begin(); handler != end(); ++handler) {
      // Check the name of this handler
      // std::cout<<"QwDataHandlerArray::GetDataHandlerByName available name=="<<(*handler)->GetName()<<"== to be compared to =="<<name<<"==\n";
      if ((*handler)->GetName() == name) {
        tmp = (*handler).get();
        //std::cout<<"QwDataHandlerArray::GetDataHandlerByName found a matching name \n";
      } else {
        // nothing
      }
    }
  }
  return tmp;
}


/**
 * Get the list of handlers in this array of the specified type
 * @param type Type of the handler
 * @return Vector of handlers
 */
std::vector<VQwDataHandler*> QwDataHandlerArray::GetDataHandlerByType(const std::string& type)
{
  // Vector of handler pointers
  std::vector<VQwDataHandler*> handler_list;

  // If this array is not empty
  if (!empty()) {

    // Loop over the handlers
    for (const_iterator handler = begin(); handler != end(); ++handler) {

      // Test to see if the handler inherits from the required type
      if (VQwDataHandlerFactory::InheritsFrom((*handler).get(),type)) {
        handler_list.push_back((*handler).get());
      }

    } // end of loop over handlers

  } // end of if !empty()

  return handler_list;
}

void  QwDataHandlerArray::ClearEventData()
{
  if (!empty()) {
    std::for_each(begin(), end(),
		  boost::mem_fn(&VQwDataHandler::ClearEventData));
  }
}



void  QwDataHandlerArray::ProcessEvent()
{
  if (!empty()){
    std::for_each(begin(), end(), boost::mem_fn(&VQwDataHandler::ProcessData));
  }
}

void  QwDataHandlerArray::ConstructTreeBranches(
    QwRootFile *treerootfile,
    const std::string& treeprefix,
    const std::string& branchprefix)
{
  if (!empty()){
    for (iterator handler = begin(); handler != end(); ++handler) {
      handler->get()->ConstructTreeBranches(treerootfile, treeprefix, branchprefix);
    }
  }
}

void  QwDataHandlerArray::FillTreeBranches(QwRootFile *treerootfile)
{
  if (!empty()){
    for (iterator handler = begin(); handler != end(); ++handler) {
      handler->get()->FillTreeBranches(treerootfile);
    }
  }
}



//*****************************************************************

void  QwDataHandlerArray::ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector <Double_t> &values)
{
  if (!empty()){
    for (iterator handler = begin(); handler != end(); ++handler) {
      VQwDataHandler* handler_parity = dynamic_cast<VQwDataHandler*>(handler->get());
      handler_parity->ConstructBranchAndVector(tree,prefix,values);
    }
  }
}

void  QwDataHandlerArray::FillTreeVector(std::vector <Double_t> &values) const
{
  if (!empty()){
    for (const_iterator handler = begin(); handler != end(); ++handler) {
      VQwDataHandler* handler_parity = dynamic_cast<VQwDataHandler*>(handler->get());
      handler_parity->FillTreeVector(values);
    }
  }
}


//*****************************************************************
void  QwDataHandlerArray::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (!empty()) {
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ConstructHistograms(folder,prefix);
    }
  }
}

void  QwDataHandlerArray::FillHistograms()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwDataHandler::FillHistograms));
}


//*****************************************************************//

void  QwDataHandlerArray::FillDB(QwParityDB *db, TString type)
{
  for (iterator handler = begin(); handler != end(); ++handler) {
    VQwDataHandler* handler_parity = dynamic_cast<VQwDataHandler*>(handler->get());
    handler_parity->FillDB(db, type);
  }
}

/*
void  QwDataHandlerArray::FillErrDB(QwParityDB *db, TString type)
{
  //  for (const_iterator handler = dummy_source->begin(); handler != dummy_source->end(); ++handler) {
  for (iterator handler = begin(); handler != end(); ++handler) {
    VQwDataHandler* handler_parity = dynamic_cast<VQwDataHandler*>(handler->get());
    handler_parity->FillErrDB(db, type);
  }
  return;
}
*/

void QwDataHandlerArray::WritePromptSummary(QwPromptSummary *ps, TString type)
{
  if (!empty()){
    for (const_iterator handler = begin(); handler != end(); ++handler) {
      VQwDataHandler* handler_parity = dynamic_cast<VQwDataHandler*>(handler->get());
      handler_parity->WritePromptSummary(ps, type);
    }
  }
}


//*****************************************************************//

/**
 * Assignment operator
 * @param source DataHandler array to assign to this array
 * @return This handler array after assignment
 */
QwDataHandlerArray& QwDataHandlerArray::operator= (const QwDataHandlerArray &source)
{
  Bool_t localdebug=kFALSE;
  if(localdebug)  std::cout<<"QwDataHandlerArray::operator= \n";
  if (!source.empty()){
    if (this->size() == source.size()){
      for(size_t i=0;i<source.size();i++){
	if (source.at(i)==NULL || this->at(i)==NULL){
	  //  Either the source or the destination handler
	  //  are null
	} else {
	  VQwDataHandler *ptr1 =
	    dynamic_cast<VQwDataHandler*>(this->at(i).get());
	  if (typeid(*ptr1)==typeid(*(source.at(i).get()))){
	    if(localdebug) std::cout<<" here in QwDataHandlerArray::operator= types mach \n";
	    *(ptr1) = *(source.at(i).get());
	  } else {
	    //  DataHandlers don't match
	      QwError << " QwDataHandlerArray::operator= types do not mach" << QwLog::endl;
	      QwError << " typeid(ptr1)=" << typeid(*ptr1).name()
                      << " but typeid(*(source.at(i).get()))=" << typeid(*(source.at(i).get())).name()
                      << QwLog::endl;
	  }
	}
      }
    } else {
      //  Array sizes don't match
    }
  } else {
    //  The source is empty
  }
  return *this;
}

//*****************************************************************
/*
void  QwDataHandlerArray::PrintInfo() const
{
  if (!empty()) {
    for (const_iterator handler = begin(); handler != end(); ++handler) {
      (*handler)->PrintInfo();
    }
  }
}
*/
//*****************************************************************//

void QwDataHandlerArray::PrintValue() const
{
  if (!empty()) {
    for (const_iterator handler = begin(); handler != end(); ++handler) {
      VQwDataHandler* handler_parity = dynamic_cast<VQwDataHandler*>(handler->get());
      handler_parity->PrintValue();
    }
  }
}




//*****************************************************************//

void QwDataHandlerArray::CalculateRunningAverage()
{
  if (!empty()) {
    for (iterator handler = begin(); handler != end(); ++handler) {
      VQwDataHandler* handler_parity = dynamic_cast<VQwDataHandler*>(handler->get());
      handler_parity->CalculateRunningAverage();
    }
  }
}


void QwDataHandlerArray::AccumulateRunningSum(const QwDataHandlerArray& value, Int_t count, Int_t ErrorMask)
{
  if (!value.empty()) {
    if (this->size() == value.size()) {
	for (size_t i = 0; i < value.size(); i++) {
	  if (value.at(i)==NULL || this->at(i)==NULL) {
	    //  Either the value or the destination handler
	    //  are null
	  } else {
	    VQwDataHandler *ptr1 =
	      dynamic_cast<VQwDataHandler*>(this->at(i).get());
	    if (typeid(*ptr1) == typeid(*(value.at(i).get()))) {
	      ptr1->AccumulateRunningSum(*(value.at(i).get()), count, ErrorMask);
	    } else {
	      QwError << "QwDataHandlerArray::AccumulateRunningSum here where types don't match" << QwLog::endl;
	      QwError << " typeid(ptr1)=" << typeid(ptr1).name()
		      << " but typeid(value.at(i)))=" << typeid(value.at(i)).name()
		      << QwLog::endl;
	      //  DataHandlers don't match
	    }
	  }
	}

    } else {
      //  Array sizes don't match

    }
  } else {
    //  The value is empty
  }
}

void QwDataHandlerArray::AccumulateAllRunningSum(const QwDataHandlerArray& value, Int_t count, Int_t ErrorMask)
{
  if (!value.empty()) {
    if (this->size() == value.size()) {
	for (size_t i = 0; i < value.size(); i++) {
	  if (value.at(i)==NULL || this->at(i)==NULL) {
	    //  Either the value or the destination handler
	    //  are null
	  } else {
	    VQwDataHandler *ptr1 =
	      dynamic_cast<VQwDataHandler*>(this->at(i).get());
	    if (typeid(*ptr1) == typeid(*(value.at(i).get()))) {
	      ptr1->AccumulateRunningSum(*(value.at(i).get()), count, ErrorMask);
	    } else {
	      QwError << "QwDataHandlerArray::AccumulateRunningSum here where types don't match" << QwLog::endl;
	      QwError << " typeid(ptr1)=" << typeid(ptr1).name()
		      << " but typeid(value.at(i)))=" << typeid(value.at(i)).name()
		      << QwLog::endl;
	      //  DataHandlers don't match
	    }
	  }
	}
    } else {
      //  Array sizes don't match

    }
  } else {
    //  The value is empty
  }
}



/*
void QwDataHandlerArray::PrintErrorCounters() const{// report number of events failed due to HW and event cut faliure
  const VQwDataHandler *handler_parity;
  if (!empty()){
    for (const_iterator handler = begin(); handler != end(); ++handler){
      handler_parity=dynamic_cast<const VQwDataHandler*>((handler)->get());
      handler_parity->PrintErrorCounters();
    }
  }
}
*/

/**
 * Add the handler to this array.  Do nothing if the handler is null or if
 * there is already a handler with that name in the array.
 * @param handler DataHandler to add to the array
 */
void QwDataHandlerArray::push_back(boost::shared_ptr<VQwDataHandler> handler)
{
  
 if (handler.get() == NULL) {
   QwError << "QwDataHandlerArray::push_back(): NULL handler"
           << QwLog::endl;
   //  This is an empty handler...
   //  Do nothing for now.

 } else if (!this->empty() && GetDataHandlerByName(handler->GetName())){
   //  There is already a handler with this name!
   QwError << "QwDataHandlerArray::push_back(): handler " << handler->GetName()
           << " already exists" << QwLog::endl;

 } else if (!CanContain(handler.get())) {
   //  There is no support for this type of handler
   QwError << "QwDataHandlerArray::push_back(): handler " << handler->GetName()
           << " is not supported by this handler array" << QwLog::endl;

 } else {
   boost::shared_ptr<VQwDataHandler> handler_tmp(handler);
   HandlerPtrs::push_back(handler_tmp);

/*
   // Set the parent of the handler to this array
   handler_tmp->SetParent(this);


   // Instruct the handler to publish variables
   if (handler_tmp->PublishInternalValues() == kFALSE) {
     QwError << "Not all variables for " << handler_tmp->GetName()
             << " could be published!" << QwLog::endl;
   }
*/
 }
}

/*
void QwDataHandlerArray::PrintParamFileList() const
{
  if (not empty()) {
    for (const_iterator handler = begin(); handler != end(); ++handler)
      {
        (*handler)->PrintDetectorMaps(true);
      }
  }
}

TList* QwDataHandlerArray::GetParamFileNameList(TString name) const
{
  if (not empty()) {

    TList* return_maps_TList = new TList;
    return_maps_TList->SetName(name);

    std::map<TString, TString> mapfiles_handler;

    for (const_iterator handler = begin(); handler != end(); ++handler) 
      {
	mapfiles_handler = (*handler)->GetDetectorMaps();
	for( std::map<TString, TString>::iterator ii= mapfiles_handler.begin(); ii!= mapfiles_handler.end(); ++ii)
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

*/

void QwDataHandlerArray::ProcessDataHandlerEntry()
{
  if (!empty()) {
    for(iterator handler = begin(); handler != end(); ++handler){
      (*handler)->ProcessData();
      (*handler)->AccumulateRunningSum();
    }
  }
}

void QwDataHandlerArray::FinishDataHandler()
{
  if (!empty()) {
    for(iterator handler = begin(); handler != end(); ++handler){
      (*handler)->FinishDataHandler();
    }
  }
}
  
  
  
  
  

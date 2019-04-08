/*****************************************************************************
File Name: VQwDataHandler.cc

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This is the implemetation file to the VQwDataHandler class.
              This class acts as a base class to all classes which need
              to access data from multiple subsystems

Last Modified: August 1, 2018 1:39 PM
*****************************************************************************/

#include <iostream>

using namespace std;

//header file
#include "VQwDataHandler.h"

//#include "QwCombiner.h"

#include "QwParameterFile.h"
#include "QwRootFile.h"
#include "QwVQWK_Channel.h"

#define MYSQLPP_SSQLS_NO_STATICS
#ifdef __USE_DATABASE__
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__



VQwDataHandler::~VQwDataHandler() {
  
  for (size_t i = 0; i < fOutputVar.size(); ++i) {
    if (fOutputVar.at(i) != NULL){
       delete fOutputVar.at(i);
    }
  }
  fOutputVar.clear();

}

void VQwDataHandler::LoadDetectorMaps(QwParameterFile& file){
  file.RewindToFileStart();
  file.EnableGreediness();
  while (file.ReadNextLine()) {
    QwMessage << file.GetLine() << QwLog::endl;
  }
  // Check for and process key-value pairs
  file.PopValue("map",fMapFile);
  file.PopValue("priority",fPriority);
  file.PopValue("tree-name",fTreeName);
  file.PopValue("tree-comment",fTreeComment);
}


void VQwDataHandler::CalcOneOutput(const VQwHardwareChannel* dv, VQwHardwareChannel* output,
                                  vector< const VQwHardwareChannel* > &ivs,
                                  vector< Double_t > &sens) {
  
  // if second is NULL, can't do corrector
  if (output == NULL){
    QwError<<"Second is value is NULL, unable to calculate corrector."<<QwLog::endl;
    return;
  }
  // For correct type (asym, diff, mps)
  // if (fDependentType.at(dv) != type) continue;

  // Clear data in second, if first is NULL
  if (dv == NULL){
    output->ClearEventData();
  }else{
    // Update second value
    output->AssignValueFrom(dv);
  }

  // Add corrections
  for (size_t iv = 0; iv < ivs.size(); iv++) {
    output->ScaledAdd(sens.at(iv), ivs.at(iv));
  }
  
}

void VQwDataHandler::ProcessData() {
  
  for (size_t i = 0; i < fDependentVar.size(); ++i) {
    *(fOutputVar.at(i)) = *(fDependentVar[i]);
  }
  for (size_t i = 0; i < fDependentValues.size(); ++i) {
    fOutputValues.at(i) = fDependentValues[i];
  }
  
}


Int_t VQwDataHandler::ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff) {

  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fDependentName.size(); dv++) {
    // Get the dependent variables

    VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    QwVQWK_Channel* vqwk = NULL;
    string name = "";
    string cor = "cor_";
    
    if (fDependentType.at(dv)==kHandleTypeMps) {
      //  Quietly ignore the MPS type when we're connecting the asym & diff
      continue;
    } else if(fDependentName.at(dv).at(0) == '@' ) {
      name = fDependentName.at(dv).substr(1,fDependentName.at(dv).length());
    }else{
      switch (fDependentType.at(dv)) {
        case kHandleTypeAsym:
          dv_ptr = asym.ReturnInternalValueForFriends(fDependentName.at(dv));
          break;
        case kHandleTypeDiff:
          dv_ptr = diff.ReturnInternalValueForFriends(fDependentName.at(dv));
          break;
        default:
          QwWarning << "QwCombiner::ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff):  Dependent variable, "
		                << fDependentName.at(dv)
		                << ", for asym/diff processing does not have proper type, type=="
		                << fDependentType.at(dv) << "."<< QwLog::endl;
          break;
      }

      vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
      name = vqwk->GetElementName().Data();
      name.insert(0, cor);
      new_vqwk = new QwVQWK_Channel(*vqwk, VQwDataElement::kDerived);
      new_vqwk->SetElementName(name);
    }

    // alias
    if(fDependentName.at(dv).at(0) == '@') {
      //QwMessage << "dv: " << name << QwLog::endl;
      new_vqwk = new QwVQWK_Channel(name, VQwDataElement::kDerived);
    }
    // defined type
    else if(dv_ptr!=NULL) {
      //QwMessage << "dv: " << fDependentName.at(dv) << QwLog::endl;
    }else {
      QwWarning << "Dependent variable " << fDependentName.at(dv) << " could not be found, "
                << "or is not a VQWK channel." << QwLog::endl;
      continue; 
    }

    // pair creation
    if(new_vqwk != NULL) {
      fDependentVar.push_back(vqwk);
      fOutputVar.push_back(new_vqwk);
    }
  }
}


pair<VQwDataHandler::EQwHandleType,string> VQwDataHandler::ParseHandledVariable(const string& variable) {
  
  pair<EQwHandleType,string> type_name;
  size_t len = variable.length();
  size_t pos1 = variable.find_first_of(ParseSeparator);
  size_t pos2 = variable.find_first_not_of(ParseSeparator,pos1);
  if (pos1 == string::npos) {
    type_name.first  = kHandleTypeUnknown;
    type_name.second = variable;
  } else {
    string type = variable.substr(0,pos1);
    string name = variable.substr(pos2,len-pos2);
    if (type == "asym")
      {type_name.first = kHandleTypeAsym;}
    else if (type == "diff")
      {type_name.first = kHandleTypeDiff;}
    else if (type == "mps")
      {type_name.first = kHandleTypeMps;}
    else
      {type_name.first = kHandleTypeUnknown;}
    type_name.second = name;
  }
  return type_name;
  
}

void VQwDataHandler::ConstructTreeBranches(QwRootFile *treerootfile)
{
  if (fTreeName.size()>0){
    treerootfile->ConstructTreeBranches(fTreeName, fTreeComment, *this);
  }
}

void VQwDataHandler::ConstructBranchAndVector(
    TTree *tree,
    TString& prefix,
    std::vector<Double_t>& values)
{
  for (size_t i = 0; i < fOutputVar.size(); ++i) {
    fOutputVar.at(i)->ConstructBranchAndVector(tree, prefix, values);
  }
}

void VQwDataHandler::FillTreeBranches(QwRootFile *treerootfile)
{
  if (fTreeName.size()>0){
    treerootfile->FillTreeBranches(*this);
    treerootfile->FillTree(fTreeName);
  }
}


/**
 * Fill the tree vector
 * @param values Vector of values
 */
void VQwDataHandler::FillTreeVector(std::vector<Double_t>& values) const
{
  // Fill the data element
  for (size_t i = 0; i < fOutputVar.size(); ++i) {
    if (fOutputVar.at(i) == NULL) {continue;}
    fOutputVar.at(i)->FillTreeVector(values);
  }
}

    
void VQwDataHandler::AccumulateRunningSum(VQwDataHandler &value)
{
  if (value.fErrorFlag==0){
    for (size_t i = 0; i < fOutputVar.size(); i++){
      this->fOutputVar[i]->AccumulateRunningSum(value.fOutputVar[i]);
    }
  }
}


void VQwDataHandler::CalculateRunningAverage()
{
  for(size_t i = 0; i < fOutputVar.size(); i++) {
    // calling CalculateRunningAverage in scope of VQwHardwareChannel
    fOutputVar[i]->CalculateRunningAverage();
  }
  
  return;
}


void VQwDataHandler::PrintValue() const
{
  QwMessage<<"=== QwCombiner ==="<<QwLog::endl<<QwLog::endl;
  for(size_t i = 0; i < fOutputVar.size(); i++) {
    fOutputVar[i]->PrintValue();
  }
}


#ifdef __USE_DATABASE__
void VQwDataHandler::FillDB(QwParityDB *db, TString datatype)
{

  Bool_t local_print_flag = kTRUE;

  UInt_t analysis_id = db->GetAnalysisID();

  TString measurement_type;
  measurement_type = QwDBInterface::DetermineMeasurementTypeID(datatype);

  std::vector<QwDBInterface> interface;

  std::vector<QwParitySSQLS::beam>      beamlist;
  std::vector<QwParitySSQLS::md_data>   mdlist;
  std::vector<QwParitySSQLS::lumi_data> lumilist;

  QwDBInterface::EQwDBIDataTableType tabletype;


  for(size_t i = 0; i < fOutputVar.size(); i++) {
    interface.clear();
    fOutputVar[i]->AddEntriesToList(interface);
    for(size_t j=0; j<interface.size(); j++) {
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMeasurementTypeID( measurement_type );
      tabletype = interface.at(j).SetDetectorID( db );
      if (tabletype==QwDBInterface::kQwDBI_OtherTable){
	      TString tmp_name = interface.at(j).GetDeviceName();
	      tmp_name.Remove(0,5);
	      interface.at(j).SetDetectorName(tmp_name);
	      tabletype = interface.at(j).SetDetectorID( db );
      }
      if (tabletype==QwDBInterface::kQwDBI_BeamTable){
	      interface.at(j).AddThisEntryToList( beamlist );
      } else if (tabletype==QwDBInterface::kQwDBI_MDTable){
	      interface.at(j).AddThisEntryToList( mdlist );
      } else if (tabletype==QwDBInterface::kQwDBI_LumiTable){
	      interface.at(j).AddThisEntryToList( lumilist );
      } else {
	      QwError << "QwCombiner::FillDB:  Unrecognized detector name:  " 
		            << interface.at(j).GetDeviceName() << QwLog::endl;
      }
      interface.at(j).PrintStatus( local_print_flag);
    }
  }

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( beamlist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(beamlist.begin(), beamlist.end());
    query.execute();
  } else {
    QwMessage << "QwCombiner::FillDB :: This is the case when the beamlist contains nothing for type="<< measurement_type.Data() 
	            << QwLog::endl;
  }
  if( mdlist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(mdlist.begin(), mdlist.end());
    query.execute();
  } else {
    QwMessage << "QwCombiner::FillDB :: This is the case when the mdlist contains nothing for type="<< measurement_type.Data() 
	            << QwLog::endl;
  }
  if( lumilist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(lumilist.begin(), lumilist.end());
    query.execute();
  } else {
    QwMessage << "QwCombiner::FillDB :: This is the case when the lumilist contains nothing for type="<< measurement_type.Data() 
	      << QwLog::endl;
  }
  db->Disconnect();
  return;
}
#endif // __USE_DATABASE__

/*

Bool_t VQwDataHandler::PublishInternalValue(const TString &name, const TString &desc, const VQwHardwareChannel *value) const {
  
  Bool_t status = kTRUE;

  if (desc == "integrationpmt") {
    value = GetIntegrationPMT(name)->GetChannel(name);
  } else if (desc == "combinedpmt") {
    value = GetCombinedPMT(name)->GetChannel(name);
  } else {
    QwError << "VQwDataHandler::PublishInternalValues() error "<< QwLog::endl;
  }

  if (desc == NULL) {
    QwError << "VQwDataHandler::PublishInternalValues(): " << name << " not found" << QwLog::endl;
    status |= kFALSE;
  } else {
    QwDebug << "VQwDataHandler::PublishInternalValues(): " << name << " found" << QwLog::endl;
  }

  return status;

}

Bool_t VQwDataHandler::PublishByRequest(TString device_name) {
  
  Bool_t status = kFALSE;
  
  for(size_t i=0;i<fDependentVar.size();i++) {

    if(device_name.CompareTo(fDependentName.at(i)!=0) continue;
    
    if (fDependentType.at(i) == kQwCombinedPMT){
      status = PublishInternalValue(device_name, "published-by-request", fDependentVar.at(i));
    } else if (fDependentType.at(i) == kQwIntegrationPMT) {
      status = PublishInternalValue(device_name, "published-by-request", fDependentVar.at(i));
    } else {
      QwError << "Unknown channel name:  " << device_name << QwLog::endl;
    }

    break;

  }

  if (!status) {
    QwError << "VQwDataHandler::PublishByRequest:  Failed to publish channel name:  " << device_name << QwLog::endl;
  }

  return status;
  
}

*/


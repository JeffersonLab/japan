/*
 * QwCombinerSubsystem.h
 *
 *  Created on: Aug 11, 2011
 *      Author: meeker
 */


#ifndef __QWCOMBINERSUBSYSTEM__
#define __QWCOMBINERSUBSYSTEM__

// Boost headers
#include <boost/shared_ptr.hpp>

// headers
#include "VQwSubsystemParity.h"
#include "QwSubsystemArrayParity.h"
#include "QwCombiner.h"

class QwParameterFile;

/**
 * \class QwCombinerSubsystem
 *
 * \brief
 *
 */

class QwCombinerSubsystem: public VQwSubsystemParity, 
  public MQwSubsystemCloneable<QwCombinerSubsystem>,
  public QwCombiner
{
  
  public:
      // Constructors
      /// \brief Constructor with just name.
      QwCombinerSubsystem(const TString name)
      : VQwSubsystem(name), VQwSubsystemParity(name), QwCombiner(name) { }

      // Copy Constructor
      QwCombinerSubsystem(const QwCombinerSubsystem &source)
      : VQwSubsystem(source), VQwSubsystemParity(source), QwCombiner(source) { }
	
      // Destructor 
      ~QwCombinerSubsystem();

      boost::shared_ptr<VQwSubsystem> GetSharedPointerToStaticObject();

      /// \brief Update the running sums
      void AccumulateRunningSum(VQwSubsystem* input, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
      void DeaccumulateRunningSum(VQwSubsystem* value, Int_t ErrorMask=0xFFFFFFF);
      /// \brief Calculate the average for all good events
      void CalculateRunningAverage();
      /// \brief Print values for all channels
      void PrintValue() const;

      void ProcessData(){
        QwCombiner::ProcessData();
      }

      /// \brief Overloaded Operators
      VQwSubsystem& operator=  (VQwSubsystem *value);
      VQwSubsystem& operator+= (VQwSubsystem *value);
      VQwSubsystem& operator-= (VQwSubsystem *value);
      VQwSubsystem& operator*= (VQwSubsystem *value);
      VQwSubsystem& operator/= (VQwSubsystem *value);
      void Ratio(VQwSubsystem* value1, VQwSubsystem* value2);
      void Scale(Double_t value);

      void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector <Double_t> &values){
        QwCombiner::ConstructBranchAndVector(tree,prefix,values);
      }
      void FillTreeVector(std::vector<Double_t> &values) const{
        QwCombiner::FillTreeVector(values);
      }

      void ConstructHistograms(TDirectory *folder, TString &prefix);
      void FillHistograms();
      void DeleteHistograms();
      void ConstructBranch(TTree *tree, TString & prefix);
      void ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file);




      //update the error flag in the subsystem level from the top level routines related to stability checks
      // This will uniquely update the errorflag at each channel based on the error flag in the corresponding channel in the ev_error subsystem
      void UpdateErrorFlag(const VQwSubsystem *ev_error);


      /// \brief Derived functions
      // not sure if there should be empty definition, no definition or defined 
      Int_t LoadChannelMap(TString);
      Int_t LoadInputParameters(TString);
      Int_t LoadEventCuts(TString);
      void ClearEventData(){
        for (size_t i = 0; i < fOutputVar.size(); ++i) {
          if (fOutputVar.at(i) != NULL) {
            fOutputVar.at(i)->ClearEventData();
          }
        }
        /*
        Iterator_HdwChan element;
        for (element = fOutputVar.begin(); element != fOutputVar.end(); ++element) {
          if (*element != NULL) {
            (*element)->ClearEventData();
          }
        }
        */
      };
      Int_t ProcessConfigurationBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words);
	Int_t ProcessEvBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words);
      void ProcessEvent(){};

      Bool_t ApplySingleEventCuts();

      Bool_t  CheckForBurpFail(const VQwSubsystem *ev_error){
	      return kFALSE;
      };

      void IncrementErrorCounters();
      void PrintErrorCounters() const;
      UInt_t GetEventcutErrorFlag();


  private: 
       
     /**
      * Default Constructor 
      * 
      * Error: tries to call default constructors of base class, 
      * 	QwCombiner() is private
      */
   //   QwCombinerSubsystem() {};     

      
}; // class QwCombinerSubsystem


#endif // __QWCOMBINERSUBSYSTEM__


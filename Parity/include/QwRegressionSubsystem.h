/*
 * QwRegressionSubsystem.h
 *
 *  Created on: Aug 11, 2011
 *      Author: meeker
 */


#ifndef __QWREGRESSIONSUBSYSTEM__
#define __QWREGRESSIONSUBSYSTEM__

// Boost headers
#include <boost/shared_ptr.hpp>

// headers
#include "QwOptions.h"
#include "VQwSubsystemParity.h"
#include "QwSubsystemArrayParity.h"
#include "QwRegression.h"

class QwParameterFile;

/**
 * \class QwRegressionSubsystem
 *
 * \brief
 *
 */

class QwRegressionSubsystem: public VQwSubsystemParity, 
  public MQwSubsystemCloneable<QwRegressionSubsystem>,
  public QwRegression
{
  
  public:
      // Constructors
      /// \brief Constructor with just name.
      /// (use gQwOptions to initialize the QwRegression baseclass)
      QwRegressionSubsystem(TString name)
      : VQwSubsystem(name), VQwSubsystemParity(name), QwRegression(gQwOptions) { }
      /// \brief Constructor with only options
      QwRegressionSubsystem(QwOptions &options, TString name)
      : VQwSubsystem(name), VQwSubsystemParity(name), QwRegression(options) { }
      /// \brief Constructor with single event
      QwRegressionSubsystem(QwOptions &options, QwSubsystemArrayParity& event, TString name)
      : VQwSubsystem(name), VQwSubsystemParity(name), QwRegression(options, event)
      { QwMessage << "Constructing QwRegressionSubsystem" << QwLog::endl; }

      // Copy Constructor
      QwRegressionSubsystem(const QwRegressionSubsystem &source)
      : VQwSubsystem(source), VQwSubsystemParity(source), QwRegression(source) { }
	
      // Destructor 
      ~QwRegressionSubsystem();

      boost::shared_ptr<VQwSubsystem> GetSharedPointerToStaticObject();

      void ProcessOptions(QwOptions &options){QwRegression::ProcessOptions(options);};

      /// \brief Update the running sums
      void AccumulateRunningSum(VQwSubsystem* input);
      void DeaccumulateRunningSum(VQwSubsystem* value);
      /// \brief Calculate the average for all good events
      void CalculateRunningAverage();
      /// \brief Print values for all channels
      void PrintValue() const;

      void LinearRegression(){
	QwRegression::LinearRegression(QwRegression::kRegTypeMps);
      }


      /// \brief Overloaded Operators
      VQwSubsystem& operator=  (VQwSubsystem *value);
      VQwSubsystem& operator+= (VQwSubsystem *value);
      VQwSubsystem& operator-= (VQwSubsystem *value);
      VQwSubsystem& operator*= (VQwSubsystem *value);
      VQwSubsystem& operator/= (VQwSubsystem *value);
      void Sum(VQwSubsystem *value1, VQwSubsystem *value2);
      void Difference(VQwSubsystem *value1, VQwSubsystem *value2);
      void Ratio(VQwSubsystem* value1, VQwSubsystem* value2);
      void Scale(Double_t value);

      void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector <Double_t> &values){
	QwRegression::ConstructBranchAndVector(tree,prefix,values);
      }
      void FillTreeVector(std::vector<Double_t> &values) const{
	QwRegression::FillTreeVector(values);
      }

      void ConstructHistograms(TDirectory *folder, TString &prefix);
      void FillHistograms();
      void DeleteHistograms();
      void ConstructBranch(TTree *tree, TString & prefix);
      void ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file);




      //update the error flag in the subsystem level from the top level routines related to stability checks. This will uniquely update the errorflag at each channel based on the error flag in the corresponding channel in the ev_error subsystem
      void UpdateErrorFlag(const VQwSubsystem *ev_error);


      /// \brief Derived functions
	// not sure if there should be empty definition, no definition or defined 
      Int_t LoadChannelMap(TString);
      Int_t LoadInputParameters(TString);
      Int_t LoadEventCuts(TString);
      void ClearEventData(){
	PairIterator element;
	for (element = fDependentVar.begin();
	     element != fDependentVar.end(); ++element) {
	  if (element->second != NULL)
	    element->second->ClearEventData();
	}
      };
      Int_t ProcessConfigurationBuffer(UInt_t, UInt_t, UInt_t*, UInt_t);
      Int_t ProcessEvBuffer(UInt_t, UInt_t, UInt_t*, UInt_t);
      void ProcessEvent(){};

      Bool_t ApplySingleEventCuts();
      void IncrementErrorCounters();
      void PrintErrorCounters() const;
      UInt_t GetEventcutErrorFlag();


  private: 
       
     /**
      * Default Constructor 
      * 
      * Error: tries to call default constructors of base class, 
      * 	QwRegression() is private
      */
   //   QwRegressionSubsystem() {};     

      
}; // class QwRegressionSubsystem


#endif // __QWREGRESSIONSUBSYSTEM__


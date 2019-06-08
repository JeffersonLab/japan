/*
 * QwAlarmHandler.h
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 *      Editor: cameronc
 *
 *  Last Modified: June 7, 2019 1:58 PM
 */

#ifndef QWALARMHANDLER_H_
#define QWALARMHANDLER_H_

// Parent Class
#include "VQwDataHandler.h"

class QwAlarmHandler:public VQwDataHandler, public MQwDataHandlerCloneable<QwAlarmHandler>
{
 public:
    typedef std::vector< VQwHardwareChannel* >::iterator Iterator_HdwChan;
    typedef std::vector< VQwHardwareChannel* >::const_iterator ConstIterator_HdwChan;

 public:
    /// \brief Constructor with name
    QwAlarmHandler(const TString& name);

    /// \brief Copy constructor
    QwAlarmHandler(const QwAlarmHandler &source);
    /// Virtual destructor
    virtual ~QwAlarmHandler();

    /// \brief Load the channels and sensitivities
    Int_t LoadChannelMap(const std::string& mapfile);

    /// \brief Connect to Channels (event only)
    Int_t ConnectChannels(QwSubsystemArrayParity& event);
    /// \brief Connect to Channels (asymmetry/difference only)
    Int_t ConnectChannels(
        QwSubsystemArrayParity& yield,
        QwSubsystemArrayParity& asym,
			  QwSubsystemArrayParity& diff);
    

    void ProcessData();
    void CheckAlarms();
    void UpdateAlarmFile();
  
  protected:
  
    /// Default constructor (Protected for child class access)
    QwAlarmHandler() { };

    /// List of channels to use in the combiner
    std::vector< std::vector< EQwHandleType > > fAnalysisEnumType;
    std::vector< std::vector< std::string > > fIndependentName;
    std::vector< std::vector< const VQwHardwareChannel* > > fAnalysisVar;
    std::vector< std::vector< Double_t > > fSensitivity;

    // List of parameters to use in the alarm handler
    // Cameron's Alarm Stuff
    std::vector<std::string> fType;
    std::vector<std::string> fChannel;
    std::vector<std::string> fAna;
    std::vector<std::string> fTree;
    std::vector<VQwDataHandler::EQwHandleType> fAnalysisType;
    std::vector<std::string> fAnalysisName;
    std::vector<std::string> fHighHigh;
    std::vector<std::string> fHigh;
    std::vector<std::string> fLow;
    std::vector<std::string> fLowLow;
    std::vector<std::string> fRingLength;
    std::vector<std::string> fTolerance;

    // List of resultant objects for data handler to update
    std::vector< const VQwHardwareChannel* > fValue;
    std::vector<std::string> fAlarmStatus;
    std::vector<int> fNviolated = {}; // Vector of 0's for history tracking
    std::vector<int> fNsinceLastViolation = {}; // Vector of 0's for history tracking

}; // class QwAlarmHandler

inline std::ostream& operator<< (std::ostream& stream, const QwAlarmHandler::EQwHandleType& i) {
  switch (i){
  case QwAlarmHandler::kHandleTypeMps:  stream << "mps"; break;
  case QwAlarmHandler::kHandleTypeAsym: stream << "asym"; break;
  case QwAlarmHandler::kHandleTypeDiff: stream << "diff"; break;
  default:           stream << "Unknown";
  }
  return stream;
}


#endif // QWALARMHANDLER_H_

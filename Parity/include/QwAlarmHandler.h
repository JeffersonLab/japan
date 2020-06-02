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
    virtual ~QwAlarmHandler() {};

    /// \brief Load the channels and sensitivities
    Int_t LoadChannelMap(const std::string& mapfile);

    /// \brief Connect to Channels (event only)
    //Int_t ConnectChannels(QwSubsystemArrayParity& event);
    /// \brief Connect to Channels (asymmetry/difference only)
    Int_t ConnectChannels(
        QwSubsystemArrayParity& yield,
        QwSubsystemArrayParity& asym,
			  QwSubsystemArrayParity& diff);
    

    void ProcessData();
    void CheckAlarms();
    void UpdateAlarmFile();
    void ParseConfigFile(QwParameterFile&);


  
  protected:
  
    /// Default constructor (Protected for child class access)
    QwAlarmHandler() { };

    std::string fAlarmOutputFile = "adaqfs/home/apar/bin/onlineAlarms.csv"; // The location of outpur file: alarm-output-file=/location/on/disk....
    UInt_t fCounter = 0;
    int fAlarmNupdate = 350;
    int fAlarmActive = 0; // Default to not actually doing the alarm loop unless specified by the user
    std::pair<std::string,std::string> ParseAlarmMapVariable(const string&, char);

    // List of parameters to use in the alarm handler
    // Cameron's Alarm Stuff
    struct alarmObject {
      // To get contents of map do map.at("key");
      // To see if contents in map check map.count("key")!=0
      std::map <std::string,std::string> alarmParameterMapStr;
      std::map <std::string,double> alarmParameterMap;
      VQwDataHandler::EQwHandleType analysisType;
      // List of resultant objects for data handler to update
      const VQwHardwareChannel* value;
      const UInt_t* eventcutErrorFlag;
      std::string alarmStatus;
      int Nviolated; // Vector of 0's for history tracking
      int NsinceLastViolation; // Vector of 0's for history tracking
      /*
      std::string type;
      std::string channel;
      std::string ana;
      std::string tree;
      std::string analysisName;
      std::string highHigh;
      std::string high;
      std::string low;
      std::string lowLow;
      std::string ringLength;
      std::string tolerance;   */
    };

    std::vector<alarmObject> fAlarmObjectList; // Vector pointer of objects

}; // class QwAlarmHandler

inline std::ostream& operator<< (std::ostream& stream, const QwAlarmHandler::EQwHandleType& i) {
  switch (i){
  case QwAlarmHandler::kHandleTypeMps:  stream << "mps"; break;
  case QwAlarmHandler::kHandleTypeAsym: stream << "asym"; break;
  case QwAlarmHandler::kHandleTypeYield: stream << "yield"; break;
  case QwAlarmHandler::kHandleTypeDiff: stream << "diff"; break;
  default:           stream << "Unknown";
  }
  return stream;
}

#endif // QWALARMHANDLER_H_

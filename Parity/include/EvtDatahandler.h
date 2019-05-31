/*
 * EvtDatahandler.h
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 *
 *  Last Modified: August 1, 2018 1:45 PM
 */

#ifndef EvtDatahandler_H_
#define EvtDatahandler_H_

// Parent Class
#include "VQwDataHandler.h"

class EvtDatahandler:public VQwDataHandler, public MQwDataHandlerCloneable<EvtDatahandler>
{
 public:
    typedef std::vector< VQwHardwareChannel* >::iterator Iterator_HdwChan;
    typedef std::vector< VQwHardwareChannel* >::const_iterator ConstIterator_HdwChan;

 public:
    /// \brief Constructor with name
    EvtDatahandler(const TString& name);

    /// \brief Copy constructor
    EvtDatahandler(const EvtDatahandler &source);
    /// Virtual destructor
    virtual ~EvtDatahandler();

    /// \brief Load the channels and sensitivities
    Int_t LoadChannelMap(const std::string& mapfile);

    /// \brief Connect to Channels (event only)
    Int_t ConnectChannels(QwSubsystemArrayParity& event);
    /// \brief Connect to Channels (asymmetry/difference only)
    Int_t ConnectChannels(QwSubsystemArrayParity& asym,
			  QwSubsystemArrayParity& diff);

    void ProcessData();
  
  protected:
  
    /// Default constructor (Protected for child class access)
    EvtDatahandler() { };

    /// List of channels to use in the combiner
    std::vector< std::vector< EQwHandleType > > fIndependentType;
    std::vector< std::vector< std::string > > fIndependentName;
    std::vector< std::vector< const VQwHardwareChannel* > > fIndependentVar;
    std::vector< std::vector< Double_t > > fSensitivity;


}; // class EvtDatahandler

/*inline std::ostream& operator<< (std::ostream& stream, const EvtDatahandler::EQwHandleType& i) {
  switch (i){
  case EvtDatahandler::kHandleTypeMps:  stream << "mps"; break;
  case EvtDatahandler::kHandleTypeAsym: stream << "asym"; break;
  case EvtDatahandler::kHandleTypeDiff: stream << "diff"; break;
  default:           stream << "Unknown";
  }
  return stream;
}*/


#endif // EvtDatahandler_H_

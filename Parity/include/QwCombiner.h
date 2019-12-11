/*
 * QwCombiner.h
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 *
 *  Last Modified: August 1, 2018 1:45 PM
 */

#ifndef QWCOMBINER_H_
#define QWCOMBINER_H_

// Parent Class
#include "VQwDataHandler.h"

class QwCombiner:public VQwDataHandler, public MQwDataHandlerCloneable<QwCombiner>
{
 public:
    typedef std::vector< VQwHardwareChannel* >::iterator Iterator_HdwChan;
    typedef std::vector< VQwHardwareChannel* >::const_iterator ConstIterator_HdwChan;

 public:
    /// \brief Constructor with name
    QwCombiner(const TString& name);

    /// \brief Copy constructor
    QwCombiner(const QwCombiner &source);
    /// Virtual destructor
    virtual ~QwCombiner();

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
    QwCombiner() { };

    /// Error flag mask
    UInt_t fErrorFlagMask;
    const UInt_t* fErrorFlagPointer;

    /// List of channels to use in the combiner
    std::vector< std::vector< EQwHandleType > > fIndependentType;
    std::vector< std::vector< std::string > > fIndependentName;
    std::vector< std::vector< const VQwHardwareChannel* > > fIndependentVar;
    std::vector< std::vector< Double_t > > fSensitivity;


}; // class QwCombiner

inline std::ostream& operator<< (std::ostream& stream, const QwCombiner::EQwHandleType& i) {
  switch (i){
  case QwCombiner::kHandleTypeMps:  stream << "mps"; break;
  case QwCombiner::kHandleTypeAsym: stream << "asym"; break;
  case QwCombiner::kHandleTypeDiff: stream << "diff"; break;
  default:           stream << "Unknown";
  }
  return stream;
}


#endif // QWCOMBINER_H_

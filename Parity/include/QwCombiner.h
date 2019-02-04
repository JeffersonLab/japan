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
#include "LRBCorrector.h"

class QwCombiner : public LRBCorrector {

  public:

    typedef std::vector< VQwHardwareChannel* >::iterator Iterator_HdwChan;
    typedef std::vector< VQwHardwareChannel* >::const_iterator ConstIterator_HdwChan;

  public:

    /// \brief Constructor with single event and helicity pattern
    QwCombiner(QwOptions &options, QwSubsystemArrayParity& event, QwHelicityPattern& helicitypattern);
    /// \brief Constructor with single event
    QwCombiner(QwOptions &options, QwSubsystemArrayParity& event);
    /// \brief Constructor with helicity pattern
    QwCombiner(QwOptions &options, QwHelicityPattern& helicitypattern);
    /// \brief Constructor with only options
    QwCombiner(QwOptions &options);
    /// \brief Copy constructor
    QwCombiner(const QwCombiner &source);
    /// Virtual destructor
    virtual ~QwCombiner();

    /// \brief Define the configuration options
    void static DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);


    /// \brief Load the channels and sensitivities
    Int_t LoadChannelMap(const std::string& mapfile);
    /// \brief Connect to channels
    Int_t ConnectChannels(QwSubsystemArrayParity& event,
        QwSubsystemArrayParity& asym,
        QwSubsystemArrayParity& diff);
    /// \brief Connect to Channels (event only)
    Int_t ConnectChannels(QwSubsystemArrayParity& event);
    /// \brief Connect to Channels (asymmetry/difference only)
    Int_t ConnectChannels(QwSubsystemArrayParity& asym,
			  QwSubsystemArrayParity& diff);

    void ProcessData();
  
  protected:
  
    /// Default constructor (Protected for child class access)
    QwCombiner() { };

    /// List of channels to use in the combiner
    std::vector< std::vector< EQwHandleType > > fIndependentType;
    std::vector< std::vector< std::string > > fIndependentName;

    // std::vector< std::pair< VQwHardwareChannel*, VQwHardwareChannel*> > fDependentVar;
    // std::vector< std::vector< std::pair< Double_t, const VQwHardwareChannel* > > > fIndependentVar;
    std::vector< std::vector< Double_t > > fSensitivity;
    std::vector< std::vector< const VQwHardwareChannel* > > fIndependentVar;

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

#ifndef QWEXTRACTOR_H_
#define QWEXTRACTOR_H_

// Parent Class
#include "VQwDataHandler.h"

class QwExtractor:public VQwDataHandler, public MQwDataHandlerCloneable<QwExtractor>
{
 public:
    /// \brief Constructor with name
    QwExtractor(const TString& name);

    /// \brief Copy constructor
    QwExtractor(const QwExtractor &source);
    /// Virtual destructor
    virtual ~QwExtractor();

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
    QwExtractor() { };

}; // class QwExtractor

#endif // QWEXTRACTOR_H_

/********************************************************\
* File: VQwClock.h                                       *
*                                                        *
* Author: Juan Carlos Cornejo <cornejo@jlab.org>         *
* Time-stamp: <2011-06-16>                               *
\********************************************************/

#include "VQwClock.h"

#include "QwClock.h"

// System headers
#include <stdexcept>

// Qweak database headers
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif // __USE_DATABASE__

//  Qweak types that we want to use in this template
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"


/**
 * \brief A fast way of creating a Clock of specified type
 */
VQwClock* VQwClock::Create(TString subsystemname, TString name, TString type)
{
  Bool_t localDebug = kFALSE;
  type.ToUpper();
  if( localDebug ) QwMessage<<"Creating Clock of type: "<<type<<" with name: "<<
    name<<". Subsystem Name: " <<subsystemname<<"\n";
  // (jc2) As a first try, let's do this the ugly way (but rather very
  // simple), just list out the types of Clock's supported by this code!!!
  if( type == "VQWK") { // (jc2) I don't know why on earth anyone would want
    // a VQWK clock, but it is added here for completion :)
    return new QwClock<QwVQWK_Channel>(subsystemname,name,type);
  } else if ( type == "SIS3801" ) {
    return new QwClock<QwSIS3801_Channel>(subsystemname,name,type);
  } else if ( type == "SCALER" || type == "SIS3801D24" ) {
    return new QwClock<QwSIS3801D24_Channel>(subsystemname,name,type);
  } else { // Unsupported one!
    QwWarning << "Clock of type="<<type<<" is UNSUPPORTED!!\n";
    exit(-1);
  }
}

VQwClock* VQwClock::Create(const VQwClock& source) {
  Bool_t localDebug = kFALSE;
  TString type = source.GetModuleType();
  type.ToUpper();
  if (localDebug) QwMessage<<"Creating Clock of type: " << type << QwLog::endl;
  // (jc2) As a first try, let's do this the ugly way (but rather very
  // simple), just list out the types of Clock's supported by this code!!!
  if( type == "VQWK") { // (jc2) I don't know why on earth anyone would want
    // a VQWK clock, but it is added here for completion :)
    return new QwClock<QwVQWK_Channel>(dynamic_cast<const QwClock<QwVQWK_Channel>&>(source));
  } else if ( type == "SIS3801" ) {
    return new QwClock<QwSIS3801_Channel>(dynamic_cast<const QwClock<QwSIS3801_Channel>&>(source));
  } else if ( type == "SCALER" || type == "SIS3801D24" ) {
    return new QwClock<QwSIS3801D24_Channel>(dynamic_cast<const QwClock<QwSIS3801D24_Channel>&>(source));
  } else { // Unsupported one!
    QwWarning << "Clock of type="<<type<<" is UNSUPPORTED!!\n";
    exit(-1);
  }

}

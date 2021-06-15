/**********************************************************\
* File: VQwBCM.h                                          *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "VQwBCM.h"

#include "QwBCM.h"
#include "QwCombinedBCM.h"

// System headers
#include <stdexcept>

// Qweak database headers
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif // __USE_DATABASE__

//  Qweak types that we want to use in this template
#include "QwVQWK_Channel.h"
#include "QwADC18_Channel.h"
#include "QwScaler_Channel.h"
#include "QwMollerADC_Channel.h"

/**
 * \brief A fast way of creating a BCM of specified type
 */

VQwBCM* VQwBCM::Create(TString subsystemname, TString name, TString type, TString clock)
{
  Bool_t localDebug = kFALSE;
  type.ToUpper();
  if( localDebug ) QwMessage<<"Creating BCM of type: "<<type<<" with name: "<<
    name<<". Subsystem Name: " <<subsystemname<<" and clock name="<<clock<<"\n";
  // (jc2) As a first try, let's do this the ugly way (but rather very
  // simple), just list out the types of BCM's supported by this code!!!
  if( type == "VQWK") {
    return new QwBCM<QwVQWK_Channel>(subsystemname,name,type);
  } else if ( type == "ADC18" ) {
    return new QwBCM<QwADC18_Channel>(subsystemname,name,type,clock);
  } else if ( type == "SIS3801" ) {
    return new QwBCM<QwSIS3801_Channel>(subsystemname,name,type,clock);
  } else if ( type == "SCALER" || type == "SIS3801D24" ) {
    return new QwBCM<QwSIS3801D24_Channel>(subsystemname,name,type,clock);
  } else if ( type == "MOLLERADC" ) {
    return new QwBCM<QwMollerADC_Channel>(subsystemname,name,type,clock);
  } else { // Unsupported one!
    QwWarning << "BCM of type="<<type<<" is UNSUPPORTED!!\n";
    exit(-1);
  }
}

VQwBCM* VQwBCM::Create(const VQwBCM& source)
{
  Bool_t localDebug = kFALSE;
  TString type = source.GetModuleType();
  type.ToUpper();
  if( localDebug ) QwMessage<<"Creating BCM of type: "<<type<<QwLog::endl;
  // (jc2) As a first try, let's do this the ugly way (but rather very
  // simple), just list out the types of BCM's supported by this code!!!
  if( type == "VQWK") {
    return new QwBCM<QwVQWK_Channel>(dynamic_cast<const QwBCM<QwVQWK_Channel>&>(source));
  } else if ( type == "ADC18" ) {
    return new QwBCM<QwADC18_Channel>(dynamic_cast<const QwBCM<QwADC18_Channel>&>(source));
  } else if ( type == "SIS3801" ) {
    return new QwBCM<QwSIS3801_Channel>(dynamic_cast<const QwBCM<QwSIS3801_Channel>&>(source));
  } else if ( type == "SCALER" || type == "SIS3801D24" ) {
    return new QwBCM<QwSIS3801D24_Channel>(dynamic_cast<const QwBCM<QwSIS3801D24_Channel>&>(source));
  } else if ( type == "MOLLERADC" ) {
    return new QwBCM<QwMollerADC_Channel>(dynamic_cast<const QwBCM<QwMollerADC_Channel>&>(source));
  } else { // Unsupported one!
    QwWarning << "BCM of type="<<type<<" is UNSUPPORTED!!\n";
    exit(-1);
  }
}

/**
 * \brief A fast way of creating a combo BCM of specified type
 */
VQwBCM* VQwBCM::CreateCombo(TString subsystemname, TString name, TString type)
{
  Bool_t localDebug = kFALSE;
  type.ToUpper();
  if( localDebug ) QwMessage<<"Creating CombinedBCM of type: "<<type<<" with name: "<<
    name<<". Subsystem Name: " <<subsystemname<<"\n";
  // (jc2) As a first try, let's do this the ugly way (but rather very
  // simple), just list out the types of BCM's supported by this code!!!
  if( type == "VQWK") {
    return new QwCombinedBCM<QwVQWK_Channel>(subsystemname,name,type);
  } else if ( type == "ADC18" ) {
    return new QwCombinedBCM<QwADC18_Channel>(subsystemname,name,type);
  } else if ( type == "SIS3801" ) {
    return new QwCombinedBCM<QwSIS3801_Channel>(subsystemname,name,type);
  } else if ( type == "SCALER" || type == "SIS3801D24" ) {
    return new QwCombinedBCM<QwSIS3801D24_Channel>(subsystemname,name,type);
  } else if ( type == "MOLLERADC" ) {
    return new QwCombinedBCM<QwMollerADC_Channel>(subsystemname,name,type);
  } else { // Unsupported one!
    QwWarning << "BCM of type="<<type<<" is UNSUPPORTED!!\n";
    exit(-1);
  }
}

VQwBCM* VQwBCM::CreateCombo(const VQwBCM& source)
{
  Bool_t localDebug = kFALSE;
  TString type = source.GetModuleType();
  type.ToUpper();
  if( localDebug ) QwMessage<<"Creating CombinedBCM of type: "<<type<< QwLog::endl;
  // (jc2) As a first try, let's do this the ugly way (but rather very
  // simple), just list out the types of BCM's supported by this code!!!
  if( type == "VQWK") {
    return new QwCombinedBCM<QwVQWK_Channel>(dynamic_cast<const QwCombinedBCM<QwVQWK_Channel>&>(source));
  } else if ( type == "ADC18" ) {
    return new QwCombinedBCM<QwADC18_Channel>(dynamic_cast<const QwCombinedBCM<QwADC18_Channel>&>(source));
  } else if ( type == "SIS3801" ) {
    return new QwCombinedBCM<QwSIS3801_Channel>(dynamic_cast<const QwCombinedBCM<QwSIS3801_Channel>&>(source));
  } else if ( type == "SCALER" || type == "SIS3801D24" ) {
    return new QwCombinedBCM<QwSIS3801D24_Channel>(dynamic_cast<const QwCombinedBCM<QwSIS3801D24_Channel>&>(source));
  } else if ( type == "MOLLERADC" ) {
    return new QwCombinedBCM<QwMollerADC_Channel>(dynamic_cast<const QwCombinedBCM<QwMollerADC_Channel>&>(source));
  } else { // Unsupported one!
    QwWarning << "BCM of type="<<type<<" is UNSUPPORTED!!\n";
    exit(-1);
  }
}

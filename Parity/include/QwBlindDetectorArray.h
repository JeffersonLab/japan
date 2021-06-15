/**********************************************************\
* File: QwBlindDetectorArray.h                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

/// \ingroup QwAnalysis_ADC

#ifndef __QWBLINDDETECTORARRAY__
#define __QWBLINDDETECTORARRAY__

// System headers
#include <vector>

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwIntegrationPMT.h"
#include "QwCombinedPMT.h"
#include "VQwDetectorArray.h"

// Forward declarations
class QwBlinder;
class QwBlindDetectorArrayID;


class QwBlindDetectorArray: 
 public VQwDetectorArray, 
 virtual public VQwSubsystemParity,
 public MQwSubsystemCloneable<QwBlindDetectorArray>{

  /******************************************************************
   *  Class: QwBlindDetectorArray
   *
   *
   ******************************************************************/


 private:

  /// Private default constructor (not implemented, will throw linker error on use)
  QwBlindDetectorArray();

 public:

  /// Constructor with name

  QwBlindDetectorArray(const TString& name)
  : VQwSubsystem(name), VQwSubsystemParity(name), VQwDetectorArray(name) {};


  /// Copy constructor
  QwBlindDetectorArray(const QwBlindDetectorArray& source)
  : VQwSubsystem(source),VQwSubsystemParity(source), VQwDetectorArray(source){};


  /// Virtual destructor
  ~QwBlindDetectorArray(){};

  public:

  /// \brief Blind the asymmetry
  void Blind(const QwBlinder *blinder);
  
  /// \brief Blind the difference using the yield
  void Blind(const QwBlinder *blinder, const VQwSubsystemParity* subsys);

};


#endif

/**********************************************************\
* File: QwDetectorArray.h                          *
*                                                          *
* Author: Kevin Ward (Original Code by P. M. King)                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

/// \ingroup QwAnalysis_ADC

#ifndef __QWDETECTORARRAY__
#define __QWDETECTORARRAY__

// System headers
#include <vector>

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwIntegrationPMT.h"
#include "QwCombinedPMT.h"
#include "VQwDetectorArray.h"

// Forward declarations
class QwDetectorArrayID;

class QwDetectorArray: 
 public VQwDetectorArray, 
 virtual public VQwSubsystemParity, 
 public MQwSubsystemCloneable<QwDetectorArray> {

/******************************************************************
*  Class: QwDetectorArray
*
*
******************************************************************/

private:

/// Private default constructor (not implemented, will throw linker error on use)
QwDetectorArray();

public:

/// Constructor with name
QwDetectorArray(const TString& name) 
 :VQwSubsystem(name), VQwSubsystemParity(name), VQwDetectorArray(name){};


/// Copy constructor
QwDetectorArray(const QwDetectorArray& source)
  :VQwSubsystem(source), VQwSubsystemParity(source), VQwDetectorArray(source){};


/// Virtual destructor
~QwDetectorArray() {};

};

#endif

/*!
 * \file   QwOptionsParity.h
 * \brief  Load the options for the parity subsystems
 *
 * To maintain the separation of the tracking and parity subsystems in the
 * libraries, we can only define the options for the parity subsystems in
 * this header.  This header should be loaded in executables that use the
 * parity subsystem options.
 *
 * \author Wouter Deconinck
 * \date   2009-12-10
 */

#ifndef QWOPTIONSPARITY_H
#define QWOPTIONSPARITY_H

// Qweak options header (should be first)
#include "QwOptions.h"

// Qweak headers
#include "QwEventRing.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwDetectorArray.h"
#include "QwBlindDetectorArray.h"
#include "QwDataHandlerArray.h"
#include "QwCorrelator.h"

#ifdef __USE_DATABASE__
#include "QwParityDB.h"
#endif //__USE_DATABASE__
void DefineOptionsParity(QwOptions& options)
{
  /* Define general options */
  QwOptions::DefineOptions(options);

  /* Define parity options */
  VQwDetectorArray::DefineOptions(options);
  //QwBlindDetectorArray::DefineOptions(options);
  QwEventRing::DefineOptions(options);
  QwHelicity::DefineOptions(options);
  QwHelicityPattern::DefineOptions(options);
  QwDataHandlerArray::DefineOptions(options);
  QwCorrelator::DefineOptions(options);
  #ifdef __USE_DATABASE__
  QwParityDB::DefineAdditionalOptions(options);
  #endif //__USE_DATABASE__
}

#endif // QWOPTIONSPARITY_H

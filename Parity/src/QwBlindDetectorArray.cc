/**********************************************************\
* File: QwBlindDetectorArray.cc                          *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#include "QwBlindDetectorArray.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwBlindDetectorArray);


/**
 * Blind the asymmetry
 * @param blinder Blinder
 */
void QwBlindDetectorArray::Blind(const QwBlinder *blinder)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].Blind(blinder);
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].Blind(blinder);
}

/**
 * Blind the difference using the yield
 * @param blinder Blinder
 * @param subsys Subsystem
 */

void QwBlindDetectorArray::Blind(const QwBlinder *blinder, const VQwSubsystemParity* subsys)
{
  /// \todo TODO (wdc) At some point we should introduce const-correctness in
  /// the Compare() routine to ensure nothing funny happens.  This const_casting
  /// is just an ugly stop-gap measure.
  if (Compare(const_cast<VQwSubsystemParity*>(subsys))) {

    const QwBlindDetectorArray* yield = dynamic_cast<const QwBlindDetectorArray*>(subsys);
    if (yield == 0) return;

    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
      fIntegrationPMT[i].Blind(blinder, yield->fIntegrationPMT[i]);
    for (size_t i = 0; i < fCombinedPMT.size(); i++)
      fCombinedPMT[i].Blind(blinder, yield->fCombinedPMT[i]);
  }
}

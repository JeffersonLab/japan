/**
 * \file	QwSIS3320_LogicalAccumulator.cc
 *
 * \brief	Implementation of the SIS3320 sampling ADC accumulator
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwSIS3320_LogicalAccumulator should allow convenient access to the accumulator
 * data collected with the SIS3320 for the Compton photon detector.  This class
 * implements its own sum, difference, and ratio methods inherited from the
 * general VQwDataElement.
 *
 */

#include "QwSIS3320_LogicalAccumulator.h"
#include "QwSIS3320_Accumulator.h"

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"


void QwSIS3320_LogicalAccumulator::ProcessEvent()
{
  for (size_t i = 0; i < fAccumulators.size(); i++) {
    fAccumulatorSum += fAccumulatorWeights[i]*(fAccumulators[i]->GetAccumulatorSum());
  }
}

void QwSIS3320_LogicalAccumulator::AddAccumulatorReference(
    QwSIS3320_Accumulator *accum, Double_t weight)
{
  if (accum) {
    fAccumulators.push_back(accum);
    fAccumulatorWeights.push_back(weight);
  }
}


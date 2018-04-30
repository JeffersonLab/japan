/**
 * \class	QwSIS3320_LogicalAccumulator	QwSIS3320_LogicalAccumulator.h
 *
 * \brief	SIS3320 sampling ADC accumulator
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

#ifndef __QwSIS3320_LogicalAccumulator__
#define __QwSIS3320_LogicalAccumulator__

// System headers
#include <vector>

// Qweak headers
#include "VQwDataElement.h"
#include "QwSIS3320_Accumulator.h"

class QwSIS3320_LogicalAccumulator: public QwSIS3320_Accumulator {
public:

  QwSIS3320_LogicalAccumulator(TString name = "")
  : QwSIS3320_Accumulator(name)
  { };
  virtual ~QwSIS3320_LogicalAccumulator() { };

  void AddAccumulatorReference( QwSIS3320_Accumulator *accum, Double_t weight);

  void  ProcessEvent();
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement = 0) { return 0; };

private:

  std::vector<QwSIS3320_Accumulator*> fAccumulators;
  std::vector<Double_t> fAccumulatorWeights;
};

#endif // __QwSIS3320_LogicalAccumulator__

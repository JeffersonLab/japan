/**********************************************************\
* File: MQwMockable.h                                      *
*                                                          *
* Author: P. King                                          *
* Date:   Tue Mar 29 13:08:12 EDT 2011                     *
\**********************************************************/

#ifndef __MQWMOCKABLE__
#define __MQWMOCKABLE__

// Boost math library for random number generation
#include "boost/random.hpp"

//jpan: Mersenne Twistor: A 623-diminsionally equidistributed
//uniform pseudorandom number generator
#include "TRandom3.h"

class QwParameterFile;

class MQwMockable {
/****************************************************************//**
 *  Class: MQwMockable
 *         Mix-in class to enable mock-data generation for a
 *         data element.
 *         Only the data element classes which actually contain
 *         raw data (such as QwVQWK_Channel, QwScaler_Channel, etc.)
 *         should inherit from this class.
 ******************************************************************/
public:
  MQwMockable(): fUseExternalRandomVariable(false),
                 fCalcMockDataAsDiff(false),
		 fMockAsymmetry(0.0), fMockGaussianMean(0.0),
                 fMockGaussianSigma(0.0)
  {
    // Mock drifts
    fMockDriftAmplitude.clear();
    fMockDriftFrequency.clear();
    fMockDriftPhase.clear();
  }
  virtual ~MQwMockable() {
    fMockDriftAmplitude.clear();
    fMockDriftFrequency.clear();
    fMockDriftPhase.clear();
  }


  /// \name Parity mock data generation
  // @{
  ///  Load the mock data parameters from the current line in the param file.
  virtual void LoadMockDataParameters(QwParameterFile &paramfile);

  /// Set a single set of harmonic drift parameters
  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  /// Add drift parameters to the internal set
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  /// Set the normal random event parameters
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  /// Set the helicity asymmetry
  void  SetRandomEventAsymmetry(Double_t asymmetry);

  /// Return a random value generated either from the internal or
  /// external Random Variable.
  Double_t GetRandomValue();

  /// Internally generate random event data
  virtual void  RandomizeEventData(int helicity = 0, double time = 0.0) = 0;

//-------------------------------------------------------------------------------------------
  virtual void SmearByResolution(double resolution) = 0;
//-------------------------------------------------------------------------------------------

  virtual void  SetRawEventData() = 0;

  /// Encode the event data into a CODA buffer  
  virtual void EncodeEventData(std::vector<UInt_t> &buffer) = 0;

  /// Set the flag to use an externally provided random variable
  void  UseExternalRandomVariable() { fUseExternalRandomVariable = true; };
  /// Set the externally provided random variable
  void  SetExternalRandomVariable(Double_t random_variable) {
    fUseExternalRandomVariable = true;
    fExternalRandomVariable = random_variable;
  };

  // Default the mock data to be calculated with a zero-mean.
  void  SetMockDataAsDiff() {fCalcMockDataAsDiff=true;};
  // @}

 protected:
  /// \name Parity mock data generation
  // @{
  /// Internal randomness generator
  static boost::mt19937 fRandomnessGenerator;
  /// Internal normal probability distribution
  static boost::normal_distribution<double> fNormalDistribution;
  /// Internal normal random variable
  static boost::variate_generator
    < boost::mt19937, boost::normal_distribution<double> > fNormalRandomVariable;
  /// Flag to use an externally provided normal random variable
  bool fUseExternalRandomVariable;
  /// Externally provided normal random variable
  double  fExternalRandomVariable;

  bool fCalcMockDataAsDiff;

  // Parameters of the mock data
  Double_t fMockAsymmetry;     ///< Helicity asymmetry
  Double_t fMockGaussianMean;  ///< Mean of normal distribution
  Double_t fMockGaussianSigma; ///< Sigma of normal distribution
  std::vector<Double_t> fMockDriftAmplitude; ///< Harmonic drift amplitude
  std::vector<Double_t> fMockDriftFrequency; ///< Harmonic drift frequency
  std::vector<Double_t> fMockDriftPhase;     ///< Harmonic drift phase
  // @}
};

#endif

/**
 * \class	QwSIS3320_Channel	QwSIS3320_Channel.h
 *
 * \brief	Class for the decoding of the SIS3320 sampling ADC data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwSIS3320_Channel class is defined to read the integrated and sampled
 * data from the Compton photon detector.  Because the scope of this module is
 * similar the the VQWK ADC module (integration and asymmetries), parts of
 * this class are very similar to QwVQWK_Channel.
 *
 * The main data members of a QwSIS3320_Channel are the vector fSamples of
 * QwSIS3320_Samples where each entry stores a sample event, and the vector of
 * fAccumulators of QwSIS3320_Accumulator where each entry stores an accumulator
 * block.  Both of these data members are derived from their --Raw counterparts
 * by subtraction of pedestals and multiplication with calibration constants.
 *
 */

#ifndef __QwSIS3320_Channel__
#define __QwSIS3320_Channel__

// System headers
#include <iostream>
#include <vector>
#include <numeric>

// ROOT headers
#include <TTree.h>

// Boost math library for random number generation
#include <boost/random.hpp>

// Qweak headers
#include "MQwMockable.h"
#include "VQwDataElement.h"
#include "QwSIS3320_Accumulator.h"
#include "QwSIS3320_LogicalAccumulator.h"
#include "QwSIS3320_Samples.h"

class QwSIS3320_Channel: public VQwDataElement, public MQwMockable {

  public:

    QwSIS3320_Channel(UInt_t channel = 0, TString name = "auto")
    : VQwDataElement(),MQwMockable() {
      InitializeChannel(channel, name);
    };
    QwSIS3320_Channel(const QwSIS3320_Channel& source)
    : VQwDataElement(source),MQwMockable(source),
      fChannel(source.fChannel),
      fHasSamplingData(source.fHasSamplingData),
      fHasAccumulatorData(source.fHasAccumulatorData),
      fPedestal(source.fPedestal),
      fCalibrationFactor(source.fCalibrationFactor),
      fCurrentEvent(source.fCurrentEvent),
      fNumberOfEvents(source.fNumberOfEvents),
      fSampleFormat(source.fSampleFormat),
      fSamples(source.fSamples),
      fSamplesRaw(source.fSamplesRaw),
      fAverageSamples(source.fAverageSamples),
      fTimeWindowAverages(source.fTimeWindowAverages),
      fTimeWindows(source.fTimeWindows),
      fSampleWindowAverages(source.fTimeWindowAverages),
      fSampleWindows(source.fSampleWindows),
      fAccumulatorDAC(source.fAccumulatorDAC),
      fAccumulatorThreshold1(source.fAccumulatorThreshold1),
      fAccumulatorThreshold2(source.fAccumulatorThreshold2),
      fAccumulatorTimingBefore5(source.fAccumulatorTimingBefore5),
      fAccumulatorTimingAfter5(source.fAccumulatorTimingAfter5),
      fAccumulatorTimingBefore6(source.fAccumulatorTimingBefore6),
      fAccumulatorTimingAfter6(source.fAccumulatorTimingAfter6),
      fAccumulators(source.fAccumulators),
      fAccumulatorsRaw(source.fAccumulatorsRaw)
    { }
    virtual ~QwSIS3320_Channel() { };

    void AddLogicalAccumulator(
        const TString name,
        const std::vector<TString> accums,
        const std::vector<Double_t> weights);

    void  InitializeChannel(UInt_t channel, TString name);
    void  ClearEventData();

    virtual void LoadMockDataParameters(QwParameterFile &paramfile) {
      std::cerr << "QwSIS3320_Channel::LoadMockDataParameters is not defined!" << std::endl;
    };
    void  RandomizeEventData(int helicity = 0, double time = 0.0) { };
    void  EncodeEventData(std::vector<UInt_t> &buffer);

    Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0);
    void  ProcessEvent();

    const QwSIS3320_Channel operator+ (const Double_t &value) const;
    const QwSIS3320_Channel operator- (const Double_t &value) const;
    const QwSIS3320_Channel operator+ (const QwSIS3320_Channel &value) const;
    const QwSIS3320_Channel operator- (const QwSIS3320_Channel &value) const;
    QwSIS3320_Channel& operator=  (const QwSIS3320_Channel &value);
    QwSIS3320_Channel& operator+= (const Double_t &value);
    QwSIS3320_Channel& operator-= (const Double_t &value);
    QwSIS3320_Channel& operator+= (const QwSIS3320_Channel &value);
    QwSIS3320_Channel& operator-= (const QwSIS3320_Channel &value);
    void Sum(QwSIS3320_Channel &value1, QwSIS3320_Channel &value2);
    void Difference(QwSIS3320_Channel &value1, QwSIS3320_Channel &value2);
    void Ratio(QwSIS3320_Channel &numer, QwSIS3320_Channel &denom);
    void Offset(Double_t Offset);
    void Scale(Double_t Offset);

    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();

    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  FillTreeVector(std::vector<Double_t> &values) const;

    QwSIS3320_Samples& GetSamples(size_t i) { return fSamples.at(i); };
    QwSIS3320_Samples& GetSamplesRaw(size_t i) { return fSamplesRaw.at(i); };

    size_t GetNumberOfEvents() const { return (fNumberOfEvents); };
    void SetNumberOfEvents(UInt_t nevents) {
      fNumberOfEvents = nevents;
      fSamplesRaw.resize(nevents);
      fSamples.resize(nevents);
    };

    size_t GetNumberOfAccumulators() const { return (fNumberOfAccumulators); };
    void SetNumberOfAccumulators(UInt_t naccumulators) {
      fNumberOfAccumulators = naccumulators;
      fAccumulatorsRaw.resize(naccumulators);
      fAccumulators.resize(naccumulators);
    };

    void SetPedestal(const Double_t ped) { fPedestal = ped; };
    Double_t GetPedestal() const { return fPedestal; };
    void SetCalibrationFactor(const Double_t factor) { fCalibrationFactor = factor; };
    Double_t GetCalibrationFactor() const { return fCalibrationFactor; };

    Bool_t IsGoodEvent();

    void PrintValue() const;
    void PrintInfo() const;

  private:

    static const Bool_t kDEBUG;

    // Identification information
    UInt_t fChannel;
    Bool_t fHasSamplingData;
    Bool_t fHasAccumulatorData;

    /* ADC Calibration */
    static const Double_t kVoltsPerBit;
    static const Double_t kNanoSecondsPerSample;
    Double_t fPedestal;
    Double_t fCalibrationFactor;

    /* In sampling mode we have multiple events in a single data block */
    Int_t fCurrentEvent; //! Current triggered event (allow for negative sentinel)
    UInt_t fNumberOfEvents; //! Number of triggered events

    /* ADC sample data */
    UInt_t fSampleFormat;
    std::vector<QwSIS3320_Samples> fSamples;
    std::vector<QwSIS3320_Samples> fSamplesRaw;
    //
    QwSIS3320_Samples fAverageSamples;
    QwSIS3320_Samples fAverageSamplesRaw;
    std::vector<Double_t> fTimeWindowAverages;
    std::vector<std::pair<UInt_t, UInt_t> > fTimeWindows;
    std::vector<Double_t> fSampleWindowAverages;
    std::vector<std::pair<Double_t, Double_t> > fSampleWindows;

    /* ADC accumulator data */
    Int_t fNumberOfAccumulators;
    Int_t fAccumulatorDAC;
    Int_t fAccumulatorThreshold1, fAccumulatorThreshold2;
    Int_t fAccumulatorTimingBefore5, fAccumulatorTimingAfter5;
    Int_t fAccumulatorTimingBefore6, fAccumulatorTimingAfter6;
    std::vector<QwSIS3320_Accumulator> fAccumulators;
    std::vector<QwSIS3320_Accumulator> fAccumulatorsRaw;
    std::vector<QwSIS3320_LogicalAccumulator> fLogicalAccumulators;

    // Operation mode flags
    static const unsigned int MODE_ACCUM_EVENT;
    static const unsigned int MODE_MULTI_EVENT;
    static const unsigned int MODE_SINGLE_EVENT;
    static const unsigned int MODE_NOTREADY;

    // Data storage format flags
    static const unsigned int FORMAT_ACCUMULATOR;
    static const unsigned int FORMAT_LONG_WORD_SAMPLING;
    static const unsigned int FORMAT_SHORT_WORD_SAMPLING;

};

#endif // __QwSIS3320_Channel__

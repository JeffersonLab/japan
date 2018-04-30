/**
 * \class	QwSIS3320_Samples	QwSIS3320_Samples.h
 *
 * \brief	SIS3320 sampling ADC samples
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwSIS3320_Samples should allow convenient access to the sampling data
 * collected with the SIS3320 for the Compton photon detector.  This class
 * implements its own sum, difference, and ratio methods inherited from the
 * general VQwDataElement.
 *
 */

#ifndef __QwSIS3320_Samples__
#define __QwSIS3320_Samples__

// System headers
#include <iostream>
#include <vector>

// ROOT headers
#include <TObject.h>
#include <TGraph.h>
#include <TTree.h>

/// At this point the samples are hard-coded to be of Float_t data type.
/// Ideally, this should be templated out, so that the channel can have
/// UInt_t raw samples, Float_t processed samples, and Double_t average
/// samples.  Seems to cause too many problems right now and needs some
/// thinking.
typedef Double_t QwSIS3320_Type;

class QwSIS3320_Samples: public TObject {

  public:

    QwSIS3320_Samples(UInt_t nsamples = 256) {
      fGraph = 0;
      SetSamplePointer(0);
      SetSamplesPerWord(2);
      SetNumberOfSamples(nsamples);
    };
    virtual ~QwSIS3320_Samples() {
      if (fGraph) delete fGraph;
    };

    size_t GetMinIndex() const { return GetMin().first; };
    size_t GetMaxIndex() const { return GetMax().first; };
    QwSIS3320_Type GetMinSample() const { return GetMin().second; };
    QwSIS3320_Type GetMaxSample() const { return GetMax().second; };

    QwSIS3320_Type GetSum() const;
    QwSIS3320_Type GetSample(size_t i) const { return fSamples.at(i); };
    QwSIS3320_Type GetPedestal() const { return GetSample(0); };
    QwSIS3320_Type GetSumInTimeWindow(const UInt_t start, const UInt_t stop) const;

    UInt_t GetNumberOfDataWords() const { return fNumberOfDataWords; };
    void SetNumberOfDataWords(const UInt_t &numwords) {
      fNumberOfDataWords = numwords;
    };

    UInt_t GetNumberOfSamples() const { return fSamples.size(); };
    void SetNumberOfSamples(const UInt_t nsamples) {
      // Initialize index vector
      fIndex.resize(nsamples);
      for (size_t i = 0; i < fIndex.size(); i++) fIndex[i] = i;
      // Initialize sample vector
      fSamples.resize(nsamples);
      SetNumberOfDataWords(GetNumberOfSamples() / GetSamplesPerWord());
    };

    UInt_t GetSamplePointer() const { return fSamplePointer; };
    void SetSamplePointer(const UInt_t samplepointer) {
      fSamplePointer = samplepointer;
    };

    UInt_t GetSamplesPerWord() const { return fSamplesPerWord; };
    void SetSamplesPerWord(const UInt_t nsamples) {
      fSamplesPerWord = nsamples;
      SetNumberOfDataWords(GetNumberOfSamples() / GetSamplesPerWord());
    };

    // Return the graph (not const because TGraph::Draw is not const)
    TGraph* GetGraph() const { return fGraph; };

    // Update the graph from the index and value vectors
    void UpdateGraph();


    void  ClearEventData() { fSamples.clear(); };
    Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement = 0);

    QwSIS3320_Samples& operator/= (const Double_t &value);
    QwSIS3320_Samples& operator*= (const Double_t &value);
    QwSIS3320_Samples& operator+= (const Double_t &value);
    QwSIS3320_Samples& operator-= (const Double_t &value);
    const QwSIS3320_Samples operator/ (const Double_t &value) const;
    const QwSIS3320_Samples operator* (const Double_t &value) const;
    const QwSIS3320_Samples operator+ (const Double_t &value) const;
    const QwSIS3320_Samples operator- (const Double_t &value) const;

    QwSIS3320_Samples& operator=  (const QwSIS3320_Samples &value);
    QwSIS3320_Samples& operator+= (const QwSIS3320_Samples &value);
    QwSIS3320_Samples& operator-= (const QwSIS3320_Samples &value);
    const QwSIS3320_Samples operator+ (const QwSIS3320_Samples &value) const;
    const QwSIS3320_Samples operator- (const QwSIS3320_Samples &value) const;

    // Output stream operator<< for an accumulator
    friend std::ostream& operator<< (std::ostream& stream, const QwSIS3320_Samples& s);

  private:

    // Private helper methods for getting minimum and maximum index and samples
    std::pair<size_t,QwSIS3320_Type> GetMin() const; //!
    std::pair<size_t,QwSIS3320_Type> GetMax() const; //!

  private:

    /// Number of 12-bit sample values per data word
    UInt_t fSamplesPerWord;
    /// Number of data words in this data element
    UInt_t fNumberOfDataWords;
    /// Sample position in buffer
    UInt_t fSamplePointer;

    //! Samples index
    static std::vector<QwSIS3320_Type> fIndex; //!
    //! Samples values
    std::vector<QwSIS3320_Type> fSamples;
    //! Graph of samples
    TGraph* fGraph;

    // Ntuple array indices
    size_t fTreeArrayIndex; //!< Index of this data element in tree
    size_t fTreeArrayNumEntries; //!< Number of entries from this data element

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwSIS3320_Samples,1);
  #endif
};

// Output stream operator<< for the samples
inline std::ostream& operator<< (std::ostream& stream, const QwSIS3320_Samples& s)
{
  for (size_t i = 0; i < s.GetNumberOfSamples(); i++)
    stream << s.GetSample(i) << " ";
  return stream;
}

#endif // __QwSIS3320_Samples__

#ifndef __MQWHISTOGRAMS__
#define __MQWHISTOGRAMS__

// System headers
#include <vector>

// Root headers
#include "TH1.h"

// Qweak headers
#include "QwLog.h"

class MQwHistograms {

    /// Regular pointers for the histograms
    typedef TH1* TH1_ptr;
    // Shared pointers (boost::shared_ptr) are not advisable
    // because ROOT keep ownership of all histograms.  They
    // are automatically deleted when ROOT closes the file.
    // If we put them in a shared_ptr here, they would be
    // deleted by the time the shared_ptr goes out of scope.

  protected:
    /// Default constructor
    MQwHistograms() { }
    /// Copy constructor
    MQwHistograms(const MQwHistograms& source)
    : fHistograms(source.fHistograms) { }
    /// Virtual destructor
    virtual ~MQwHistograms() { }

    /// Arithmetic assignment operator:  Should only copy event-based data.
    /// In this particular class, there is no event-based data.
    virtual MQwHistograms& operator=(const MQwHistograms& value) {
      return *this;
    }

    inline void Fill_Pointer(TH1_ptr hist_ptr, Double_t value){
      if (hist_ptr != NULL){
	hist_ptr->Fill(value);
      }
    }

  protected:
    /// Histograms associated with this data element
    std::vector<TH1_ptr> fHistograms;

  protected:
    /// Register a histogram
    void AddHistogram(TH1* h) {
      fHistograms.push_back(TH1_ptr(h));
    }

  public:
    /// Share histogram pointers between objects
    void ShareHistograms(const MQwHistograms* source) {
      if (source) fHistograms = source->fHistograms;
    }


}; // class MQwHistograms

#endif // __MQWHISTOGRAMS__

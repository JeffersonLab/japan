#ifndef LINREGBEVPEB_h
#define LINREGBEVPEB_h
/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Utility class accumulating variances and computing alphas  based on :
 * "Data reduction and error analysis for the physical sciences" / Philip R. Bevington, D. Keith Robinson. Bevington, Philip R., 1933- Boston : McGraw-Hill, c2003.
 * "Formulas for Robust, One-Pass Parallel Computation of Covariances and Arbitrary-Order Statistical Moments" Philippe Peba, SANDIA REPORT SAND2008-6212, Unlimited Release, Printed September 2008
 *********************************************************************/

// System headers
#include <utility>

// ROOT headers
#include <TVectorD.h>
#include <TMatrixD.h>

//-----------------------------------------
class LinRegBevPeb {
  int  nP; // number of independent variables
  int  nY; // number of   dependent variables
 
 private:
  Int_t fErrorFlag;             ///< is information valid
  Long64_t fGoodEventNumber;    ///< accumulated so far  

  /// correlations
  TMatrixD mRPY, mRYP;
  TMatrixD mRPP, mRYY;
  TMatrixD mRYYp;

  /// unnormalized covariances
  TMatrixD mVPY, mVYP;
  TMatrixD mVPP, mVYY;
  TMatrixD mVYYp;
  /// variances
  TVectorD mVP, mVY;
  TVectorD mVYp;

  /// normalized covariances
  TMatrixD mSPY, mSYP;
  TMatrixD mSPP, mSYY;
  TMatrixD mSYYp;
  /// sigmas
  TVectorD mSP, mSY;
  TVectorD mSYp;

  /// mean values
  TVectorD mMP, mMY, mMYp;


  /// slopes
  TMatrixD Axy, Ayx, dAxy, dAyx; // found slopes and their standard errors

 public:

  LinRegBevPeb();
  LinRegBevPeb(const LinRegBevPeb& source);
  virtual ~LinRegBevPeb() { };

  void solve();
  bool failed() { return fGoodEventNumber < nP + 1; }

  // after last event
  void printSummaryP() const;
  void printSummaryY() const;
  void printSummaryYP() const;
  void printSummaryAlphas() const;
  void printSummaryMeansWithUnc() const;
  void printSummaryMeansWithUncCorrected() const;

  void print();
  void init();
  void clear();
  void setDims(int a, int b){ nP = a; nY = b;}

  /// Get mean value of a variable, returns error code
  Int_t getMeanP(const int i, Double_t &mean) const;
  Int_t getMeanY(const int i, Double_t &mean) const;
  Int_t getMeanYprime(const int i, Double_t &mean) const;

  /// Get mean value of a variable, returns error code
  Int_t getSigmaP(const int i, Double_t &sigma) const;
  Int_t getSigmaY(const int i, Double_t &sigma) const;
  Int_t getSigmaYprime(const int i, Double_t &sigma) const;

  /// Get mean value of a variable, returns error code
  Int_t getCovarianceP (int i,  int j,  Double_t &covar) const;
  Int_t getCovariancePY(int ip, int iy, Double_t &covar) const;
  Int_t getCovarianceY (int i,  int j,  Double_t &covar) const;

  double  getUsedEve() const { return fGoodEventNumber; };

  // Addition-assignment
  LinRegBevPeb& operator+=(const std::pair<TVectorD,TVectorD>& rhs);
  LinRegBevPeb& operator+=(const LinRegBevPeb& rhs);
  // Addition using addition-assignment
  friend // friends defined inside class body are inline and are hidden from non-ADL lookup
  LinRegBevPeb operator+(LinRegBevPeb lhs,  // passing lhs by value helps optimize chained a+b+c
                   const LinRegBevPeb& rhs) // otherwise, both parameters may be const references
  {
    lhs += rhs; // reuse compound assignment
    return lhs; // return the result by value (uses move constructor)
  }

  /// \brief Output stream operator
  friend std::ostream& operator<< (std::ostream& stream, const LinRegBevPeb& h);

  /// Friend class with correlator for ROOT tree output
  friend class QwCorrelator;
};

/// Output stream operator
inline std::ostream& operator<< (std::ostream& stream, const LinRegBevPeb& h)
{
  stream << "LRB: " << h.fGoodEventNumber << " events";
  return stream;
}

#endif

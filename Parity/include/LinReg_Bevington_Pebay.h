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

#include <TMatrixD.h>

//-----------------------------------------
class LinRegBevPeb {
  int  par_nP; // number of independent variables
  int  par_nY; // number of   dependent variables
 
 private:
  Long64_t fGoodEventNumber;    ///< accumulated so far  

 public:
  TMatrixD mA, mAsig;  ///< found slopes + their stand errors
  TMatrixD mRjk, mRky, mRyy; ///< found correlations for IV/IV, DV/IV, DV/DV
  TMatrixD mMP, mMY, mMYprime;   ///< mean values accumulators
  TMatrixD mVPP, mVPY, mVYY, mVP2, mVY2; ///< variances accumulators
  TMatrixD covYprime, sigYprime; ///< covariances after correction

  LinRegBevPeb();
  
  virtual ~LinRegBevPeb(){};

  /// processing single events
  void  accumulate(double *P, double *Y);
  void  solve();
  double Alpha(int ip, int iy){ return mA(ip,iy);} //ok
  bool   failed(){ return  fGoodEventNumber<2;}

  // after last event
  void printSummaryP();
  void printSummaryY();
  void printSummaryYP();
  void printSummaryAlphas();

  void print();
  void init();
  void setDims(int a, int b){ par_nP=a; par_nY=b;}

  /// Get mean value of a variable, returns error code
  Int_t getMeanP(const int i, Double_t &mean );
  Int_t getMeanY(const int i, Double_t &mean );
  Int_t getMeanYprime(const int i, Double_t &mean );

  /// Get mean value of a variable, returns error code
  Int_t getSigmaP(const int i, Double_t &sigma );
  Int_t getSigmaY(const int i, Double_t &sigma );

  /// Get mean value of a variable, returns error code
  Int_t getCovarianceP ( int i,  int j,  Double_t &covar );
  Int_t getCovariancePY( int ip, int iy, Double_t &covar );
  Int_t getCovarianceY ( int i,  int j,  Double_t &covar );

  double  getUsedEve(){ return fGoodEventNumber;}


  /// \brief Output stream operator
  friend std::ostream& operator<< (std::ostream& stream, const LinRegBevPeb& h);

};

/// Output stream operator
inline std::ostream& operator<< (std::ostream& stream, const LinRegBevPeb& h) {
  stream << Form("LinRegBevPeb::<< ");
  //  stream << Form("  tag correlation>%.3f ", h.par_highCorr);
  return stream;
}



#endif

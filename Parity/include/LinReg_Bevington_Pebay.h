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

#include <TVectorD.h>
#include <TMatrixD.h>

//-----------------------------------------
class LinRegBevPeb {
  int  nP; // number of independent variables
  int  nY; // number of   dependent variables
 
 private:
  Long64_t fGoodEventNumber;    ///< accumulated so far  

 public:
  TMatrixD mA, mAsig;  ///< found slopes + their stand errors

  /// correlations
  TMatrixD mRPY, mRYP;
  TMatrixD mRPP, mRYY;
  TMatrixD mRYYprime;

  /// unnormalized covariances
  TMatrixD mVPY, mVYP;
  TMatrixD mVPP, mVYY;
  TMatrixD mVYYprime;

  /// normalized covariances
  TMatrixD sigXY, sigYX;
  TMatrixD sigXX, sigYY;
  TMatrixD sigYYprime;

  /// mean values
  TVectorD mMP, mMY, mMYprime;

  /// sigmas
  TVectorD sigX, sigY, sigYprime;

  /// slopes
  TMatrixD Axy, Ayx;

  LinRegBevPeb();
  
  virtual ~LinRegBevPeb(){};

  /// processing single events
  void  accumulate(TVectorD P, TVectorD Y);

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
  void setDims(int a, int b){ nP=a; nY=b;}

  /// Get mean value of a variable, returns error code
  Int_t getMeanP(const int i, Double_t &mean );
  Int_t getMeanY(const int i, Double_t &mean );
  Int_t getMeanYprime(const int i, Double_t &mean );

  /// Get mean value of a variable, returns error code
  Int_t getSigmaP(const int i, Double_t &sigma );
  Int_t getSigmaY(const int i, Double_t &sigma );
  Int_t getSigmaYprime(const int i, Double_t &sigma );

  /// Get mean value of a variable, returns error code
  Int_t getCovarianceP ( int i,  int j,  Double_t &covar );
  Int_t getCovariancePY( int ip, int iy, Double_t &covar );
  Int_t getCovarianceY ( int i,  int j,  Double_t &covar );

  double  getUsedEve(){ return fGoodEventNumber;}

  // Addition-assignment
  LinRegBevPeb& operator+=(const LinRegBevPeb& rhs)
  {
    // If X = A + B, then
    //   Cov[X] = Cov[A] + Cov[B] + (E[x_A] - E[x_B]) * (E[y_A] - E[y_B]) * n_A * n_B / n_X
    // Ref: E. Schubert, M. Gertz (9 July 2018).
    // "Numerically stable parallel computation of (co-)variance".
    // SSDBM '18 Proceedings of the 30th International Conference
    // on Scientific and Statistical Database Management.
    // https://doi.org/10.1145/3221269.3223036

    // Deviations from mean
    TVectorD delta_y(mMY - rhs.mMY);
    TVectorD delta_p(mMP - rhs.mMP);

    // Update covariances
    Double_t alpha = fGoodEventNumber * rhs.fGoodEventNumber
                  / (fGoodEventNumber + rhs.fGoodEventNumber);
    mVYY += rhs.mVYY;
    mVYY.Rank1Update(delta_y, delta_y, alpha);
    mVPY += rhs.mVPY;
    mVPY.Rank1Update(delta_p, delta_y, alpha);
    mVPP += rhs.mVPP;
    mVPP.Rank1Update(delta_p, delta_p, alpha);

    // Update means
    Double_t beta = rhs.fGoodEventNumber / (fGoodEventNumber + rhs.fGoodEventNumber);
    mMY += delta_y * beta;
    mMP += delta_p * beta;

    fGoodEventNumber += rhs.fGoodEventNumber;

    return *this;
  }

  // Addition using addition-assignment
  // friends defined inside class body are inline and are hidden from non-ADL lookup
  friend LinRegBevPeb operator+(LinRegBevPeb lhs,  // passing lhs by value helps optimize chained a+b+c
                   const LinRegBevPeb& rhs) // otherwise, both parameters may be const references
  {
    lhs += rhs; // reuse compound assignment
    return lhs; // return the result by value (uses move constructor)
  }

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

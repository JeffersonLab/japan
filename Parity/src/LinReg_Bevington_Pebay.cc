/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Utility class accumulating variances and computing alphas  based on :
 * "Data reduction and error analysis for the physical sciences" / Philip R. Bevington, D. Keith Robinson. Bevington, Philip R., 1933- Boston : McGraw-Hill, c2003.
 * "Formulas for Robust, One-Pass Parallel Computation of Covariances and Arbitrary-Order Statistical Moments" Philippe Peba, SANDIA REPORT SAND2008-6212, Unlimited Release, Printed September 2008
 *********************************************************************/

#include <assert.h>
#include <math.h>
#include <iostream>
using namespace std;

#include "LinReg_Bevington_Pebay.h"

#include "QwLog.h"

//=================================================
//=================================================
LinRegBevPeb::LinRegBevPeb()
: nP(0),nY(0),
  fGoodEventNumber(0)
{ }

//=================================================
//=================================================
void LinRegBevPeb::init()
{
  printf("Init LinReg dims: nP=%d nY=%d\n",nP,nY);

  mMP.ResizeTo(nP);
  mMY.ResizeTo(nY);
  mMYprime.ResizeTo(nY);

  mVPP.ResizeTo(nP,nP);
  mVPY.ResizeTo(nP,nY);
  mVYP.ResizeTo(nY,nP);
  mVYY.ResizeTo(nY,nY);
  mVYYprime.ResizeTo(nY,nY);

  sigXX.ResizeTo(mVPP);
  sigXY.ResizeTo(mVPY);
  sigYX.ResizeTo(mVYP);
  sigYY.ResizeTo(mVYY);
  sigYYprime.ResizeTo(mVYYprime);

  mA.ResizeTo(nP,nY);
  Axy.ResizeTo(nP,nY);
  Ayx.ResizeTo(nY,nP);
  mAsig.ResizeTo(mA);

  sigX.ResizeTo(nP);
  sigY.ResizeTo(nY);
  sigYprime.ResizeTo(nY);

  mRPP.ResizeTo(mVPP);
  mRPY.ResizeTo(mVPY);
  mRYP.ResizeTo(mVYP);
  mRYY.ResizeTo(mVYY);
  mRYYprime.ResizeTo(mVYYprime);

  fGoodEventNumber=0;

void LinRegBevPeb::clear()
{
  mVPP.Zero();
  mVPY.Zero();
  mVYY.Zero();
  mMP.Zero();
  mMY.Zero();
  fGoodEventNumber = 0;
}

//=================================================
//=================================================
void LinRegBevPeb::print(){
  printf("LinReg dims:  nP=%d nY=%d\n",nP,nY);

  cout<<"MP:"; mMP.Print();
  cout<<"MY:"; mMY.Print();
  cout<<"VPP:"; mVPP.Print();
  cout<<"VPY:"; mVPY.Print();
  cout<<"VYY:"; mVYY.Print();
  cout<<"VYYprime:"; mVYYprime.Print();
}


//==========================================================
//==========================================================
void LinRegBevPeb::accumulate(TVectorD P, TVectorD Y)
{
  // Update number of events
  fGoodEventNumber++;

  if (fGoodEventNumber <= 1) {
    // First event, set covariances to zero and means to first value
    mVPP.Zero();
    mVPY.Zero();
    mVYY.Zero();
    mMP = P;
    mMY = Y;
  } else {
    // Deviations from mean
    TVectorD delta_y(Y - mMY);
    TVectorD delta_p(P - mMP);

    // Update covariances
    Double_t alpha = (fGoodEventNumber - 1.0) / fGoodEventNumber;
    mVPP.Rank1Update(delta_p, delta_p, alpha);
    mVPY.Rank1Update(delta_p, delta_y, alpha);
    mVYY.Rank1Update(delta_y, delta_y, alpha);

    // Update means
    Double_t beta = 1.0 / fGoodEventNumber;
    mMP += delta_p * beta;
    mMY += delta_y * beta;
  }
}


//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getMeanP(const int i, Double_t &mean ){
   mean=-1e50;
   if(i<0 || i >= nP ) return -1;
   if( fGoodEventNumber<1) return -3;
   mean = mMP(i);    return 0;
}


//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getMeanY(const int i, Double_t &mean ){
  mean=-1e50;
  if(i<0 || i >= nY ) return -1;
  if( fGoodEventNumber<1) return -3;
  mean = mMY(i);    return 0;
}


//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getMeanYprime(const int i, Double_t &mean ){
  mean=-1e50;
  if(i<0 || i >= nY ) return -1;
  if( fGoodEventNumber<1) return -3;
  mean = mMYprime(i);    return 0;
}


//==========================================================
//==========================================================
Int_t   LinRegBevPeb::getSigmaP(const int i, Double_t &sigma ){
  sigma=-1e50;
  if(i<0 || i >= nP ) return -1;
  if( fGoodEventNumber<2) return -3;
  sigma=sqrt(mVPP(i,i)/(fGoodEventNumber-1.));
  return 0;
}


//==========================================================
//==========================================================
Int_t   LinRegBevPeb::getSigmaY(const int i, Double_t &sigma ){
  sigma=-1e50;
  if(i<0 || i >= nY ) return -1;
  if( fGoodEventNumber<2) return -3;
  sigma=sqrt(mVYY(i,i)/(fGoodEventNumber-1.));
  return 0;
}

//==========================================================
//==========================================================
Int_t   LinRegBevPeb::getSigmaYprime(const int i, Double_t &sigma ){
  sigma=-1e50;
  if(i<0 || i >= nY ) return -1;
  if( fGoodEventNumber<2) return -3;
  sigma=sqrt(mVYYprime(i,i)/(fGoodEventNumber-1.));
  return 0;
}

//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getCovarianceP( int i, int j, Double_t &covar ){
    covar=-1e50;
    if( i>j) { int k=i; i=j; j=k; }//swap i & j
    //... now we need only upper right triangle
    if(i<0 || i >= nP ) return -11;
    if( fGoodEventNumber<2) return -14;
    covar=mVPP(i,j)/(fGoodEventNumber-1.);
    return 0;
}

//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getCovariancePY(  int ip, int iy, Double_t &covar ){
    covar=-1e50;
    //... now we need only upper right triangle
    if(ip<0 || ip >= nP ) return -11;
    if(iy<0 || iy >= nY ) return -12;
    if( fGoodEventNumber<2) return -14;
    covar=mVPY(ip,iy)/(fGoodEventNumber-1.);
    return 0;
}

//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getCovarianceY( int i, int j, Double_t &covar ){
    covar=-1e50;
    if( i>j) { int k=i; i=j; j=k; }//swap i & j
    //... now we need only upper right triangle
    if(i<0 || i >= nY ) return -11;
    if( fGoodEventNumber<2) return -14;
    covar=mVYY(i,j)/(fGoodEventNumber-1.);
    return 0;
}

//==========================================================
//==========================================================
void LinRegBevPeb::printSummaryP(){
  cout << Form("\nLinRegBevPeb::printSummaryP seen good eve=%lld",fGoodEventNumber)<<endl;

  size_t dim=nP;
  if(fGoodEventNumber>2) { // print full matrix
    cout << Form("\nname:                                               ");
    for (size_t i = 1; i <dim; i++) {
      cout << Form("P%d%11s",(int)i," ");
    }
    cout << Form("\n           mean     sig(distrib)   nSig(mean)       corelation-matrix ....\n");
    for (size_t i = 0; i <dim; i++) {
      Int_t testval;
      double meanI,sigI;
      testval = getMeanP(i,meanI);
      assert(testval==0);
      testval = getSigmaP(i,sigI);
      assert(testval==0);
      double nSig=-1;
      double err=sigI/sqrt(fGoodEventNumber);
      if(sigI>0.) nSig=meanI/err;

      cout << Form("P%d:  %+12.4g  %12.3g      %.1f    ",(int)i,meanI,sigI,nSig);
      for (size_t j = 1; j <dim; j++) {
        if( j<=i) { cout << Form("  %12s","._._._."); continue;}
        Int_t testval;
        double sigJ,cov;
        testval = getSigmaP(j,sigJ);
        assert(testval==0);
        testval = getCovarianceP(i,j,cov);
        assert(testval==0);
        double corel=cov/sigI/sigJ;
        
        cout << Form("  %12.3g",corel);
      }
      cout << Form("\n");
    }
  }
}


//==========================================================
//==========================================================
void LinRegBevPeb::printSummaryY(){
  cout << Form("\nLinRegBevPeb::printSummaryY seen good eve=%lld  (CSV-format)",fGoodEventNumber)<<endl;
  cout << Form("  j,       mean,     sig(mean),   nSig(mean),  sig(distribution)    \n");
  
  for (int i = 0; i <nY; i++) {
    Int_t testval;
    double meanI,sigI;
    testval = getMeanY(i,meanI);
    assert(testval==0);
    testval = getSigmaY(i,sigI);
    assert(testval==0);
    double err=sigI/sqrt(fGoodEventNumber);
    double nSigErr=meanI/err;
    cout << Form("Y%02d, %+11.4g, %12.4g, %8.1f, %12.4g "" ",i,meanI,err,nSigErr,sigI)<<endl;

  }
}



//==========================================================
//==========================================================
void LinRegBevPeb::printSummaryAlphas(){
  cout << Form("\nLinRegBevPeb::printSummaryAlphas seen good eve=%lld",fGoodEventNumber)<<endl;
  cout << Form("\n  j                slope         sigma     mean/sigma\n");
  for (int iy = 0; iy <nY; iy++) {
    cout << Form("dv=Y%d: ",iy)<<endl;
    for (int j = 0; j < nP; j++) {
      double val=mA(j,iy);
      double err=mAsig(j,iy);
      double nSig=val/err;
      char x=' ';
      if(fabs(nSig)>3.) x='*';
      cout << Form("  slope_%d = %11.3g  +/-%11.3g  (nSig=%.2f) %c\n",j,val, err,nSig,x);
    }
  }
}


//==========================================================
//==========================================================
void LinRegBevPeb::printSummaryYP(){
  cout << Form("\nLinRegBevPeb::printSummaryYP seen good eve=%lld",fGoodEventNumber)<<endl;

  if(fGoodEventNumber<2) { cout<<"  too fiew events, skip"<<endl; return;}
  cout << Form("\n         name:             ");
  for (int i = 0; i <nP; i++) {
    cout << Form(" %10sP%d "," ",i);
  }
  cout << Form("\n  j                   meanY         sigY      corelation with Ps ....\n");
  for (int iy = 0; iy <nY; iy++) {
    Int_t testval;
    double meanI,sigI;
    testval = getMeanY(iy,meanI);
    assert(testval==0);
    testval = getSigmaY(iy,sigI);
    assert(testval==0);
    
    cout << Form(" %3d %6sY%d:  %+12.4g  %12.4g ",iy," ",iy,meanI,sigI);
    for (int ip = 0; ip <nP; ip++) {
      Int_t testval;
      double sigJ,cov; 
      testval = getSigmaP(ip,sigJ);
      assert(testval==0);
      testval = getCovariancePY(ip,iy,cov);
      assert(testval==0);
      double corel=cov/sigI/sigJ;      
      cout << Form("  %12.3g",corel);
    }
    cout << Form("\n");
  }
}



//==========================================================
//==========================================================
void LinRegBevPeb::solve() {
  cout << Form("\n********LinRegBevPeb::solve...invert Rjk")<<endl;

  TMatrixD invmVPP(TMatrixD::kUnit, mVPP);
  invmVPP *= TMatrixDDiag(mVPP);
  invmVPP.Invert();
  invmVPP.Sqrt();

  TMatrixD invmVYY(TMatrixD::kUnit, mVYY);
  invmVYY *= TMatrixDDiag(mVYY);
  invmVYY.Invert();
  invmVYY.Sqrt();

  mRPP = invmVPP * mVPP * invmVPP;
  mRYY = invmVYY * mVYY * invmVYY;
  mRPY = invmVPP * mVPY * invmVYY;

  QwMessage << "det=" << mRPP.Determinant() << QwLog::endl;
  TMatrixD invRPP(TMatrixD::kInverted, mRPP);

   TMatrixD Djy; Djy.ResizeTo(mRPY);
   Djy.Mult(invRPP,mRPY);

   for (int iy = 0; iy <nY; iy++) {
    double Sy;
    Int_t testval;
    testval = getSigmaY(iy,Sy);
    assert(testval==0);
    for (int ip = 0; ip <nP; ip++) {
      Int_t testval;
      double Sk;
      testval = getSigmaP(ip,Sk);
      assert(testval==0);
      mA(ip,iy)= Djy(ip,iy)*Sy/Sk;
    }
  }

  /// normalized covariances
  sigYY = mVYY * (1.0 / (fGoodEventNumber - 1.));
  sigXX = mVPP * (1.0 / (fGoodEventNumber - 1.));
  sigXY = mVPY * (1.0 / (fGoodEventNumber - 1.));
  sigYX.Transpose(sigXY);

  // slopes
  Axy = mA;
  Ayx.Transpose(Axy);

  // new means
  mMYprime = mMY - Ayx * mMP;

  // new covariance
  mVYP.Transpose(mVPY);
  mVYYprime = mVYY + Ayx * mVPP * Axy - (Ayx * mVPY + mVYP * Axy);

  // new normalized covariance
  sigYYprime = sigYY + Ayx * sigXX * Axy - (Ayx * sigXY + sigYX * Axy);

  // old sigmas
  sigX = TMatrixDDiag(sigXX);
  sigX.Sqrt();
  sigY = TMatrixDDiag(sigYY);
  sigY.Sqrt();

  // new sigmas
  sigYprime = TMatrixDDiag(sigYYprime);
  sigYprime.Sqrt();

  // invert sigmas and determine correlation matrix
  TMatrixD invsigYprime(TMatrixD::kUnit, sigYYprime);
  invsigYprime *= TMatrixDDiag(sigYYprime);
  invsigYprime.Invert();
  invsigYprime.Sqrt();
  mRYYprime = invsigYprime * sigYYprime * invsigYprime;

  QwMessage << "Uncorrected Y values:" << QwLog::endl;
  QwMessage << "     mean          sig" << QwLog::endl;
  for (int i = 0; i < nY; i++){
    QwMessage << "Y" << i << ":  " << mMY(i) << " +- " << sigY(i) << QwLog::endl;
  }
  QwMessage << QwLog::endl;

  QwMessage << "Corrected Y values:" << endl;
  QwMessage << "     mean          sig" << endl;
  for (int i = 0; i < nY; i++){
    QwMessage << "Y" << i << ":  " << mMYprime(i) << " +- " << sigYprime(i) << QwLog::endl;
  }
  QwMessage << QwLog::endl;

  cout << "Compute errors of alphas ..."<<endl;
  double norm = 1. / (fGoodEventNumber - nP - 1);
  for (int iy = 0; iy < nY; iy++) {
    /* compute s^2= Vy + Vx -2*Vxy 
       where Vy~var(y), Vx~var(x), Vxy~cov(y,x)     */
    double Vx=0,Vxy=0;
    for (int j = 0; j < nP; j++) {
      for (int k = 0; k < nP; k++)
	Vx += sigXX(j,k) * mA(j,iy) * mA(k,iy);
      Vxy += sigXY(j,iy) * mA(j,iy);
    }
    double s2 = sigY(iy) * sigY(iy) + Vx - 2 * Vxy; // consistent w/ Bevington
    for (int j = 0; j < nP; j++) {
      mAsig(j,iy)= sqrt(norm * invRPP(j,j) * s2) / sigX(j);
    }
  }
}


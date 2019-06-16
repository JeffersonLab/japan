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

//=================================================
//=================================================
LinRegBevPeb::LinRegBevPeb() {

}

//=================================================
//=================================================
void LinRegBevPeb::init(){
  printf("Init LinReg dims: nP=%d nY=%d\n",par_nP,par_nY);
  mMY.ResizeTo(par_nY,1); 
  mMP.ResizeTo(par_nP,1);
  mVPP.ResizeTo(par_nP,par_nP);
  mVPY.ResizeTo(par_nP,par_nY);
  mVYY.ResizeTo(par_nY,par_nY);
  mVYYprime.ResizeTo(par_nY,par_nY);
  mA.ResizeTo(par_nP,par_nY);
  mAsig.ResizeTo(mA);
  mRPP.ResizeTo(mVPP);
  mRPY.ResizeTo(mVPY);
  mRYY.ResizeTo(mVYY);
  mRYYprime.ResizeTo(mVYYprime);

  fGoodEventNumber=0;
 
}

//=================================================
//=================================================
void LinRegBevPeb::print(){
  printf("LinReg dims:  nP=%d nY=%d\n",par_nP,par_nY);

  cout<<"MP:"; mMP.Print();
  cout<<"MY:"; mMY.Print();
  cout<<"VPP:"; mVPP.Print();
  cout<<"VPY:"; mVPY.Print();
  cout<<"VYY:"; mVYY.Print();
  cout<<"VYYprime:"; mVYYprime.Print();
}


//==========================================================
//==========================================================
void LinRegBevPeb::accumulate(double *P, double *Y){

  fGoodEventNumber++;

#if 0
  printf("LinRegBevPeb::accumulate eve=%.0f\n",fGoodEventNumber);
  for (int i = 0; i <par_nP; i++)  printf("iv_%d=%g\n",i,P[i]);
  for (int i = 0; i <par_nY; i++)  printf("dv_%d=%g\n",i,Y[i]);

#endif
  // Y-P correlation matrix
  //   Do this first, so the mMY and mMP variables are the previous means, not including this point.
  for (int j = 0; j <par_nY; j++)   {
    double v=Y[j];
    double vdel=v-mMY(j,0);
    for (int i = 0; i <par_nP; i++) {
      double u=P[i];
      double udel=u-mMP(i,0);
      if(fGoodEventNumber<=1) mVPY(i,j)=0;
      else mVPY(i,j)+=udel*vdel*(fGoodEventNumber-1.)/fGoodEventNumber;
    }
  }
  
  //....... P-only matrices ......
  for (int i = 0; i <par_nP; i++) {
    double u=P[i];
    double udel=u-mMP(i,0);
 
    if(fGoodEventNumber<=1) mMP(i,0)=u;
    else mMP(i,0)+=udel/fGoodEventNumber;

    if(fGoodEventNumber<=1)  mVPP(i,i)=0;
    else mVPP(i,i)+=(u-mMP(i,0))*udel; // Note, it uses pre & post incremented means!
    
    for (int j = i+1; j < par_nP; j++) {// only upper triangle
      double v=P[j];
      double vdel=v-mMP(j,0);
      
      if(fGoodEventNumber<=1) mVPP(i,j)=0;
      else mVPP(i,j)+=udel*vdel*(fGoodEventNumber-1.)/fGoodEventNumber;
      }// end of cov-PP
  } // end of P-only


  //....... Y-only matrices
  for (int i = 0; i <par_nY; i++)  {
    double u=Y[i]; 
    double udel=u-mMY(i,0);
 
    if(fGoodEventNumber<=1) mMY(i,0)=u;
    else mMY(i,0)+=udel/fGoodEventNumber;

    if(fGoodEventNumber<=1)  mVYY(i,i)=0;
    else mVYY(i,i)+=(u-mMY(i,0))*udel; // Note, it uses pre & post incremented means!

    for (int j = i+1; j < par_nY; j++) {// only upper triangle
      double v=Y[j];
      double vdel=v-mMY(j,0);

      if(fGoodEventNumber<=1) mVYY(i,j)=0;
      else mVYY(i,j)+=udel*vdel*(fGoodEventNumber-1.)/fGoodEventNumber;
      }// end of cov-YY

  } // end of Y-only


}


//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getMeanP(const int i, Double_t &mean ){
   mean=-1e50;
   if(i<0 || i >= par_nP ) return -1;
   if( fGoodEventNumber<1) return -3;
   mean=mMP(i,0);    return 0;
}


//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getMeanY(const int i, Double_t &mean ){
  mean=-1e50;
  if(i<0 || i >= par_nY ) return -1;
  if( fGoodEventNumber<1) return -3;
  mean=mMY(i,0);    return 0;
}


//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getMeanYprime(const int i, Double_t &mean ){
  mean=-1e50;
  if(i<0 || i >= par_nY ) return -1;
  if( fGoodEventNumber<1) return -3;
  mean=mMYprime(i,0);    return 0;
}


//==========================================================
//==========================================================
Int_t   LinRegBevPeb::getSigmaP(const int i, Double_t &sigma ){
  sigma=-1e50;
  if(i<0 || i >= par_nP ) return -1;
  if( fGoodEventNumber<2) return -3;
  sigma=sqrt(mVPP(i,i)/(fGoodEventNumber-1.));
  return 0;
}


//==========================================================
//==========================================================
Int_t   LinRegBevPeb::getSigmaY(const int i, Double_t &sigma ){
  sigma=-1e50;
  if(i<0 || i >= par_nY ) return -1;
  if( fGoodEventNumber<2) return -3;
  sigma=sqrt(mVYY(i,i)/(fGoodEventNumber-1.));
  return 0;
}

//==========================================================
//==========================================================
Int_t   LinRegBevPeb::getSigmaYprime(const int i, Double_t &sigma ){
  sigma=-1e50;
  if(i<0 || i >= par_nY ) return -1;
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
    if(i<0 || i >= par_nP ) return -11;
    if( fGoodEventNumber<2) return -14;
    covar=mVPP(i,j)/(fGoodEventNumber-1.);
    return 0;
}

//==========================================================
//==========================================================
Int_t  LinRegBevPeb::getCovariancePY(  int ip, int iy, Double_t &covar ){
    covar=-1e50;
    //... now we need only upper right triangle
    if(ip<0 || ip >= par_nP ) return -11;
    if(iy<0 || iy >= par_nY ) return -12;
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
    if(i<0 || i >= par_nY ) return -11;
    if( fGoodEventNumber<2) return -14;
    covar=mVYY(i,j)/(fGoodEventNumber-1.);
    return 0;
}

//==========================================================
//==========================================================
void LinRegBevPeb::printSummaryP(){
  cout << Form("\nLinRegBevPeb::printSummaryP seen good eve=%lld",fGoodEventNumber)<<endl;

  size_t dim=par_nP;
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
  
  for (int i = 0; i <par_nY; i++) {
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
  for (int iy = 0; iy <par_nY; iy++) {
    cout << Form("dv=Y%d: ",iy)<<endl;
    for (int j = 0; j < par_nP; j++) {
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
  for (int i = 0; i <par_nP; i++) {
    cout << Form(" %10sP%d "," ",i);
  }
  cout << Form("\n  j                   meanY         sigY      corelation with Ps ....\n");
  for (int iy = 0; iy <par_nY; iy++) {
    Int_t testval;
    double meanI,sigI;
    testval = getMeanY(iy,meanI);
    assert(testval==0);
    testval = getSigmaY(iy,sigI);
    assert(testval==0);
    
    cout << Form(" %3d %6sY%d:  %+12.4g  %12.4g ",iy," ",iy,meanI,sigI);
    for (int ip = 0; ip <par_nP; ip++) {
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
  TMatrixD S2jk;S2jk.ResizeTo(mVPP);
  for (int j = 0; j < par_nP; j++) {
    Int_t testval;
    double Sj;
    testval = getSigmaP(j,Sj);
    assert(testval==0);
    for (int k = 0; k <par_nP; k++) {
       Int_t testval;
       double Sk,s2jk;
       testval = getSigmaP(k,Sk);
       assert(testval==0);
       testval = getCovarianceP(j,k,s2jk);
       assert(testval==0);
       S2jk(j,k)=s2jk;
       mRPP(j,k)=s2jk/Sj/Sk;
    }
  }
  //cout << "new Rjk:"; mRjk.Print();
  
  TMatrixD invRjk(mRPP); double det;
   //cout<<"0 invRkl:"; invRjk.Print();
   invRjk.Invert(&det);
   cout<<Form("det=%f\n",det); //invRjk.Print();
   /* test
   TMatrixD M; M.ResizeTo(mRjk);//cout<<"0 M:"; M.Print();
   M.Mult(invRjk,mRjk);
   cout<<"check M*invM"; M.Print();
   */
   cout << Form("\n********LinRegBevPeb::solve... alphas ")<<endl;
   for (int iy = 0; iy <par_nY; iy++) {
     Int_t testval;
     double Sy;
     testval = getSigmaY(iy,Sy);
     assert(testval==0);
     for (int ip = 0; ip <par_nP; ip++) {
       Int_t testval;
       double Sk,Syk2;
       testval = getSigmaP(ip,Sk);
       assert(testval==0);
       testval = getCovariancePY(ip,iy,Syk2);
       assert(testval==0);
       mRPY(ip,iy)=Syk2/Sy/Sk;
       //if(ip==0 && iy==0) printf("Syk2=%f  Sy=%f Sk=%f\n", Syk2,Sy,Sk); 
     }
     for (int jy = 0; jy < par_nY; jy++) {
        Int_t testval;
        double Sj,s2ij;
        testval = getSigmaY(jy,Sj);
        assert(testval==0);
        testval = getCovarianceY(iy,jy,s2ij);
        assert(testval==0);
        mRYY(iy,jy)=s2ij/Sy/Sj;
     }
   }
   //cout<<"new Rky:"; Rky.Print();
   TMatrixD Djy; Djy.ResizeTo(mRPY);
   Djy.Mult(invRjk,mRPY);
   //   cout<<"Djy:"; Djy.Print();
   for (int iy = 0; iy <par_nY; iy++) {
    double Sy;
    Int_t testval;
    testval = getSigmaY(iy,Sy);
    assert(testval==0);
    for (int ip = 0; ip <par_nP; ip++) {
      Int_t testval;
      double Sk;
      testval = getSigmaP(ip,Sk);
      assert(testval==0);
      mA(ip,iy)= Djy(ip,iy)*Sy/Sk;
    }
  }

  //define the diagonals of sigYY
  TMatrixD sigYY_diag; sigYY_diag.ResizeTo(par_nY,par_nY);
  for(int iy = 0; iy < par_nY; iy++){
    Int_t testval;
    testval = getSigmaY(iy,sigYY_diag(iy,iy));
    assert(testval==0);
    sigYY_diag(iy,iy) = sigYY_diag(iy,iy)*sigYY_diag(iy,iy);
  }

  //define sigXX
  TMatrixD sigXX; sigXX.ResizeTo(par_nP,par_nP);
  for(int iy = 0; iy < par_nP; iy++){
    for(int ip = 0; ip < par_nP; ip++){
      Int_t testval;
      testval = getCovarianceP(ip,iy,sigXX(ip,iy));
      assert(testval==0);
    }
  }

  //define sigXY
  TMatrixD sigXY(mVPY * (1.0 / (fGoodEventNumber - 1.)));
  //define sigYX
  TMatrixD sigYX(sigXY.T());

  Axy.ResizeTo(par_nP,par_nY);
  Ayx.ResizeTo(par_nY,par_nP);
  Axy = mA;
  Ayx.Transpose(Axy);

  mMYprime.ResizeTo(par_nY,1);
  mMYprime = mMY - Ayx * mMP;

  covYprime.ResizeTo(par_nY,par_nY);
  covYprime = sigYY_diag + Ayx * sigXX * Axy - 2.0 * sigYX * Axy;
  mVYYprime = mVYY + Ayx * mVPP * Axy - 2.0 * Ayx * mVPY;

  sigYprime.ResizeTo(par_nY,1);
  for(int iy = 0; iy < par_nY; iy++){
    sigYprime(iy,0) = sqrt(covYprime(iy,iy));
  }
  
  cout << "Uncorrected Y values:" << endl;
  cout << "     mean          sig" << endl;
  for(int i = 0; i < par_nY; i++){
  	double yold, oldsigy;
  	getMeanY(i,yold);
  	getSigmaY(i,oldsigy);
  	cout << "Y" << i << ":  " << yold << " +- " << oldsigy << endl;
  }
  cout << endl;

  cout << "Corrected Y values:" << endl;
  cout << "     mean          sig" << endl;
  for(int i = 0; i < par_nY; i++){
  	cout << "Y" << i << ":  " << mMYprime(i,0) << " +- " << sigYprime(i,0) << endl;
  }
  cout << endl;

  cout << "Compute errors of alphas ..."<<endl;
  double norm=1./(fGoodEventNumber - par_nP -1);
  for (int iy = 0; iy <par_nY; iy++) {
    
    /* compute s^2= Vy + Vx -2*Vxy 
       where Vy~var(y), Vx~var(x), Vxy~cov(y,x)     */
    Int_t testval;
    double Sy;
    testval = getSigmaY(iy,Sy);
    assert(testval==0);
    
    double Vx=0,Vxy=0;
    for (int j = 0; j < par_nP; j++) {
      for (int k = 0; k <par_nP; k++) 
	Vx+=S2jk(j,k)*mA(j,iy)*mA(k,iy);
      Int_t testval;
      double Syk2;
      testval = getCovariancePY(j,iy,Syk2);
      assert(testval==0);
      Vxy+=Syk2*mA(j,iy);
    }
    //cout<<"iy="<<iy<<"  Vx="<<Vx<<"  Vxy="<<Vxy<<endl;
     //   printf(" errAl:  iy=%d Vy=%f\n",iy,Sy*Sy);
    double s2=Sy*Sy + Vx -2*Vxy; // consistent w/ Bevington
    
    //   cout <<" iy="<<iy<<" s2="<<s2<<endl;
    for (int j = 0; j < par_nP; j++) {
      Int_t testval;
      double Sj;
      testval = getSigmaP(j,Sj);
      assert(testval==0);
      mAsig(j,iy)= sqrt(norm * invRjk(j,j) * s2) / Sj;
    }
  }
  //cout<<"mAsig:"; mAsig.Print();
}


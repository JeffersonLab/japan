/********************************************************************
File Name: QwCorrelator.h

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This is the header file of the QwCorrelator class,
              which is a child of the VQwDataHandler class.  The
              functionality of this class is derived from
              LinRegBlue.

Last Modified: August 1, 2018 1:43 PM
********************************************************************/

#ifndef QWCORRELATOR_H_
#define QWCORRELATOR_H_

// Parent Class
#include "VQwDataHandler.h"

// LinRegBlue Correlator Class
#include "QwkRegBlueCorrelator.h"

class QwCorrelator : public VQwDataHandler {

  public:

		void init(const std::string configFName);
		void FillCorrelator();
		void CalcCorrelations();

  	QwCorrelator(QwOptions &options, QwHelicityPattern& helicitypattern, const TString &run = "0");
  	
  	void readConfig(const char * configFName);
  	
 	  /// \brief Define the configuration options
    void static DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);

    /// \brief Connect to Channels (asymmetry/difference only)
    Int_t ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff);
		
		void unpackEvent();
		
		Int_t LoadChannelMap(const std::string& mapfile);
		
	protected:
	
    bool fEnableCorrelation;
    bool fDisableHistos;

		std::string fCorrelatorMapFile;
    
    std::vector< TString > fIndependentFull;
    std::vector< TString > fDependentFull;
    
	  //  Using the fDependentType and fDependentName from base class, but override the IV arrays
	  std::vector< EQwRegType > fIndependentType;
    std::vector< std::string > fIndependentName;

    std::vector< const VQwHardwareChannel* > fIndependentVar;
    std::vector< Double_t > fIndependentValues;

    std::string fAlphaOutputPath;
    std::string fAliasOutputPath;		


  private:
		
		//Default Constructor
		QwCorrelator():corA("input") { };
		
		QwkRegBlueCorrelator corA;
		
};


#endif //QWCORRELATOR_H_

/*
 * QwRegression.h
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 */

#ifndef QWREGRESSION_H_
#define QWREGRESSION_H_

// Qweak headers
#include "QwHelicityPattern.h"
#include "QwSubsystemArrayParity.h"
#include "VQwHardwareChannel.h"

/**
 * ... info to follow ... really, I promise ...
 */
class QwRegression {

  public:

    /// Type of regression variable
    enum EQwRegType {
      kRegTypeUnknown=0, kRegTypeMps, kRegTypeAsym, kRegTypeDiff
    };

    typedef std::vector< std::pair< VQwHardwareChannel*,VQwHardwareChannel*> >::iterator PairIterator;
    typedef std::vector< std::pair< VQwHardwareChannel*,VQwHardwareChannel*> >::const_iterator PairConstIterator;

  public:

    /// \brief Constructor with single event and helicity pattern
    QwRegression(QwOptions &options,
        QwSubsystemArrayParity& event,
        QwHelicityPattern& helicitypattern);
    /// \brief Constructor with single event
    QwRegression(QwOptions &options, QwSubsystemArrayParity& event);
    /// \brief Constructor with helicity pattern
    QwRegression(QwOptions &options, QwHelicityPattern& helicitypattern);
    /// \brief Constructor with only options
    QwRegression(QwOptions &options);
    /// \brief Copy constructor
    QwRegression(const QwRegression &source);
    /// Virtual destructor
    virtual ~QwRegression();


    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);


    /// \brief Load the channels and sensitivities
    Int_t LoadChannelMap(const std::string& mapfile);
    /// \brief Connect to channels
    Int_t ConnectChannels(QwSubsystemArrayParity& event,
        QwSubsystemArrayParity& asym,
        QwSubsystemArrayParity& diff);
    /// \brief Connect to Channels (event only)
    Int_t ConnectChannels(QwSubsystemArrayParity& event);
    /// \brief Connect to Channels (asymmetry/difference only)
    Int_t ConnectChannels(QwSubsystemArrayParity& asym,
			  QwSubsystemArrayParity& diff);

    /// \brief Linear regression
    void LinearRegression(EQwRegType type);


    /// \name Tree and vector construction and maintenance
    // @{
    /// Construct the tree and vector for this subsystem
    void ConstructBranchAndVector(TTree *tree, std::vector <Double_t> &values) {
      TString tmpstr("");
      ConstructBranchAndVector(tree,tmpstr,values);
    };
    /// \brief Construct a branch and vector for this subsystem with a prefix
    void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector <Double_t> &values);

    /// \brief Fill the vector for this subsystem
    void FillTreeVector(std::vector<Double_t> &values) const;
    // @}

 

    void AccumulateRunningSum(QwRegression value);
    void CalculateRunningAverage();
    void PrintValue() const;
    void FillDB(QwParityDB *db, TString datatype);
  private:

    /// Private default constructor
    QwRegression() { };
  
  protected:
    /// Flag to enable regression
    bool fEnableRegression;

    /// Regression dependent and independent variable map
    std::string fRegressionMapFile;

    /// Single event pointer
    QwSubsystemArrayParity* fSubsystemArray;
    /// Helicity pattern pointer
    QwHelicityPattern* fHelicityPattern;

    UInt_t fErrorFlag;

    /// Parse regression variable in type and name
    std::pair<EQwRegType,std::string> ParseRegressionVariable(const std::string& variable);

    /// List of channels to use in the regression
    std::vector< EQwRegType > fDependentType;
    std::vector< std::string > fDependentName;
    std::vector< std::vector< EQwRegType > > fIndependentType;
    std::vector< std::vector< std::string > > fIndependentName;
    std::vector< std::vector< Double_t> > fSensitivity;

    std::vector< EQwRegType > fDependentVarType;
    std::vector< std::pair< VQwHardwareChannel*, VQwHardwareChannel*> > fDependentVar;
    std::vector< std::vector< std::pair< Double_t, const VQwHardwareChannel* > > > fIndependentVar;


}; // class QwRegression

inline std::ostream& operator<< (std::ostream& stream, const QwRegression::EQwRegType& i) {
  switch (i){
  case QwRegression::kRegTypeMps:  stream << "mps"; break;
  case QwRegression::kRegTypeAsym: stream << "asym"; break;
  case QwRegression::kRegTypeDiff: stream << "diff"; break;
  default:           stream << "Unknown";
  }
  return stream;
}


#endif // QWREGRESSION_H_

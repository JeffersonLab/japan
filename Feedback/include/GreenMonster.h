
#include <iostream>

//#include "TGClient.h"
#include "Rtypes.h"
#include "TString.h"
//#include "TROOT.h"
//#include "TStyle.h"
//#include "TObjectTable.h"
//#include "TGCanvas.h"
//#include "TPad.h"
//#include "TPaveText.h"
//#include "TGTab.h"
//#include "TGIcon.h"
//#include <TExec.h>
//#include "TGFrame.h"
//#include "TGMenu.h"
//#include "TSystem.h"
//#include "TGLayout.h"
//#include "TGLabel.h"
//#include "TGButton.h"
//#include "TGTextBuffer.h"
//#include "TGTextEntry.h"
//#include "TGToolTip.h"
//#include "TFile.h"
//#include "TTimer.h"
//#include "TColor.h"
//#include "TQObject.h"
//#include "TRootHelpDialog.h"
//#include "RQ_OBJECT.h"

#include "cfSock_types.h"
//#include "../bmw/bmw_cf_commands.h"
#include "SCAN_cf_commands.h"
#include "GMSock.h"

#define GM_LOCAL     201

#define GM_BMW_CHANGE 2001
#define GM_BMW_CHECK  2002
#define GM_BMW_TEST_ENABLE 2005
#define GM_BMW_TEST_SET_VALUE 2006
#define GM_BMW_SET_KILL 2007
// the relative value of these button id's labels each modulation object
#define BMW_OBJRADIO1 2101
#define BMW_OBJRADIO2 2102
#define BMW_OBJRADIO3 2103
#define BMW_OBJRADIO4 2104
#define BMW_OBJRADIO5 2105
#define BMW_OBJRADIO6 2106
#define BMW_OBJRADIO7 2107
#define BMW_OBJRADIO8 2108

#define KILL_SERVER_1 5001
#define KILL_SERVER_2 5002
#define KILL_SERVER_3 5003
#define KILL_SERVER_4 5004


#define GM_SCN_CHECK  6002
#define GM_SCN_SET    6003
#define SCN_RADIO_CLN 6101
#define SCN_RADIO_NOT 6102
#define SCN_RADIO_CLN_BT 0
#define SCN_RADIO_NOT_BT 1
#define SCN_INT_CLN 1
#define SCN_INT_NOT 0

class GreenMonster{
public:
  GreenMonster();
  ~GreenMonster();

  void CheckScan();

  void Open();
  void Close();

  void SCNSetStatus(Int_t);
  void SCNSetValue(Int_t,Int_t);

  void PrintScanInfo(){
    switch (statusSCN)
      {
      case SCN_INT_CLN:
	{
	  std::cout << "Scan Status is CLEAN" << std::endl;
	  break;
	}
      case SCN_INT_NOT:
	{
	  std::cout << "Scan Status is NOT_CLEAN" << std::endl;
	  break;
	}
      default:
	std::cout << "ERROR: Unrecognized SCAN status flag" << std::endl;
      }
    std::cout << "Scan data 1: " << setpoint1SCN << std::endl;
    std::cout << "Scan data 2: " << setpoint2SCN << std::endl;
  }

private:
  Bool_t fVerbose;

  Bool_t fUseCrate[5];
  TString* fCrateNames[5];
  Int_t fCrateNumbers[5];

  // objects for scan tabpage
  Int_t fSCN_TABID;

  Int_t statusSCN;
  Int_t setpoint1SCN;
  Int_t setpoint2SCN;

  void SCNUpdateStatus(Int_t);
  Bool_t SCNCheckStatus();
  void SCNCheckValues();
  
};

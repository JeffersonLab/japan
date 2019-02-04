
#include "GreenMonster.h"

//ClassImp(GreenMonster)

GreenMonster::GreenMonster():fVerbose(kTRUE)
{
  fUseCrate[0]= kFALSE;
  fCrateNames[0] = new TString("Qweak Injector");
  fCrateNumbers[0] = Crate_CountingHouse;//injector crate

  fUseCrate[1]= kFALSE;
  fCrateNames[1] = new TString("Injector");
  fCrateNumbers[1] = Crate_Injector;

  fUseCrate[2]= kFALSE;
  fCrateNames[2] = new TString("Left Spect");
  fCrateNumbers[2] = Crate_LeftSpect;

  fUseCrate[3]= kFALSE;
  fCrateNames[3] = new TString("Right Spect");
  fCrateNumbers[3] = Crate_RightSpect;

  
  fUseCrate[4]= kTRUE;
  fCrateNames[4] = new TString("Qw VME TS");
  fCrateNumbers[4] = Crate_Qwvmets;


}

GreenMonster::~GreenMonster() {
};

void GreenMonster::Open() {
  //  cfSockCliOpen(Crate_CountingHouse, 0);
};

void GreenMonster::Close() {
  //  cfSockCliClose();
};

void GreenMonster::CheckScan() {
  SCNCheckStatus(); // this call is actually an important initialization!
  SCNCheckValues(); // this call is actually an important initialization!
};



void GreenMonster::SCNUpdateStatus(Int_t id) {    
  
  switch (id) 
    {
    case SCN_INT_CLN:
      {
	//	fStateRBtSCN[SCN_RADIO_NOT_BT]->SetState(kButtonUp);
	//	fStateRBtSCN[SCN_RADIO_CLN_BT]->SetState(kButtonDown);
	statusSCN=id;
	break;
      }
    case SCN_INT_NOT:
      {
	//	fStateRBtSCN[SCN_RADIO_CLN_BT]->SetState(kButtonUp);
	//	fStateRBtSCN[SCN_RADIO_NOT_BT]->SetState(kButtonDown);
	statusSCN=id;
	break;
      }
    default:
      std::cout << "ERROR: Unrecognized SCAN status flag" << std::endl;
    }
  return;
}


Bool_t GreenMonster::SCNCheckStatus() {
  struct greenRequest gRequest;
  int command, par1, par2, command_type;
  char *msgReq = "SCN status check";
  char *reply = "Y";
  Int_t iclean;
  
  //  printf("SCN Status =");
  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_GET_STATUS;     gRequest.command = command;
  par1 = 0;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_Qwvmets,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    iclean=par1;
    if (iclean != SCN_INT_CLN && iclean != SCN_INT_NOT) {
      printf("UNKNOWN REPLY FOR SCN STATUS: %d  ",iclean);
      return kFALSE;
    }
  } else {
    printf("ERROR accessing socket!");
    return kFALSE;
  }
  
  //  printf("% d  %d\n",par1,par2);
  
  SCNUpdateStatus(iclean);
  return kTRUE;
}

void GreenMonster::SCNSetStatus(Int_t status) {
  struct greenRequest gRequest;
  int command, par1, par2, command_type;
  char *msgReq = "SCN Status Change";
  char *reply = "Y";

  command_type = COMMAND_SCAN;  gRequest.command_type = command_type;
  command = SCAN_SET_STATUS;    gRequest.command = command;
  par1 = status;                gRequest.par1 = par1;
  par2 = 0;                     gRequest.par2 = par2;

  if (fVerbose) std::cout << "Setting SCN status: " << par1 << std::endl;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;

  if (GreenSockCommand(Crate_Qwvmets,&gRequest) == SOCK_OK) {
    if (fVerbose) printf("SCAN status change call is complete\n");
  } else {
    printf("ERROR accessing socket!");
    return;
  }
  // check scan status
  SCNCheckStatus();
}



void GreenMonster::SCNSetValue(Int_t which, Int_t value) {
  struct greenRequest gRequest;
  int command, par1, par2, command_type;
  char *msgReq = "Set SCN Data Value";
  char *reply = "Y";
  
  if (which==1)      setpoint1SCN=value;
  else if (which==2) setpoint2SCN=value;

  if (fVerbose) std::cout << " writing new SCAN set point " << value 
			  << " to data" << which <<std::endl;
  //
  // send set message for obj, value
  //
  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_SET_DATA;       gRequest.command = command;
  par1 = which;                  gRequest.par1 = par1;
  par2 = value;                  gRequest.par2 = par2;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_Qwvmets,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
  } else {
    printf("ERROR accessing socket!"); 
    return;
  }
  return;
}

void GreenMonster::SCNCheckValues() {
  struct greenRequest gRequest;
  int command, par1, par2, command_type;
  char *msgReq = "Check SCN Data Value";
  char *reply = "Y";
  char buff[10];
  int value;

  //
  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_GET_DATA;       gRequest.command = command;
  par1 = 1;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_Qwvmets,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    value = par2;
  } else {
    printf("ERROR accessing socket!"); 
    return;
  }

  //
  // get value from tent
  //
  setpoint1SCN=value;

  // repeat for second data word

  command_type = COMMAND_SCAN;   gRequest.command_type = command_type;
  command = SCAN_GET_DATA;       gRequest.command = command;
  par1 = 2;                      gRequest.par1 = par1;
  par2 = 0;                      gRequest.par2 = par2;
  strcpy(gRequest.message,msgReq);   gRequest.reply = reply;
  if (GreenSockCommand(Crate_Qwvmets,&gRequest) == SOCK_OK) {
    command = gRequest.command;
    par1 = gRequest.par1;
    par2 = gRequest.par2;
    value = par2;
  } else {
    printf("ERROR accessing socket!"); 
    return;
  }

  //
  // get value from tent
  //
  setpoint2SCN=value;

  return;
}


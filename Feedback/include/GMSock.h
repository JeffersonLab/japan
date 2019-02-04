#include "GreenSock.h"

extern "C" int cfSockCommand(int crate_number,
			     int command_type, int command, 
			     int req_param, int req_param_2,
			     char *reply,   char *msg);

extern "C" int GreenSockCommand(int crate_number,
				struct greenRequest *gRequest);

extern "C" int cfSockCliOpen (int crate_number, int keepopen);
extern "C" int cfSockCliSend (int crate_number, struct request *myRequest, 
			      struct request *serverReply);
extern "C" int cfSockCliClose();

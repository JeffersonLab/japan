/* GreenSock.h  - header used by both GreenMonster and cfSockCli */
/* defines */
/* structure for socket requests/replies */


#ifndef SOCK_ERROR
#define SOCK_ERROR (-1)
#endif
#ifndef SOCK_OK
#define SOCK_OK    (0)
#endif

#define GREEN_REQUEST_MSG_SIZE        1024   /* max size of request message */

#ifndef Crate_CountingHouse 
#define Crate_CountingHouse (0)
#endif
#ifndef Crate_LeftSpect 
#define Crate_LeftSpect (1)
#endif	     
#ifndef Crate_RightSpect 
#define Crate_RightSpect  (2)
#endif	     
#ifndef Crate_Injector 
#define Crate_Injector  (3)
#endif
#ifndef Crate_Qwvmets
#define Crate_Qwvmets  (4)
#endif 
struct greenRequest
{ 
   long  command_type;
   long  command;
   long  par1;
   long  par2;
   char  *reply;
   char message[GREEN_REQUEST_MSG_SIZE];     /* message buffer */
};


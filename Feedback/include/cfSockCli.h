/* cfSockCli.h - header used by cfSock client, replacing the
                 missing vxWorks.h  */

#if	!defined(FALSE) || (FALSE!=0)
#define FALSE		0
#endif

#if	!defined(TRUE) || (TRUE!=1)
#define TRUE		1
#endif

/* low-level I/O input, output, error fd's */
#define	STD_IN	0
#define	STD_OUT	1
#define	STD_ERR	2

#include "GreenSock.h"
#include "cfSock.h"

/* Name or IP addresses of the various servers */
char * ServerName_CountingHouse="129.57.206.93"; /* g0 injdaq */ 
char * ServerName_LeftSpect="129.57.164.100"; 
/* char * ServerName_CountingHouse="129.57.164.13"; */
/* char * ServerName_LeftSpect="129.57.192.61"; */
char * ServerName_RightSpect="129.57.192.60";
char * ServerName_Injector="129.57.206.10";   
char * ServerName_QwTSCrate="129.57.168.100";   


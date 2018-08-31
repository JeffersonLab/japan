/* cfSock.h  - header used by both client and server of cfSock */
/* defines */

#define SERVER_PORT_NUM         5001  /* server's port number for bind() */
#define SERVER_WORK_PRIORITY    125    /* priority of server's work task */
#define SERVER_STACK_SIZE       10000  /* stack size of server's work task */
#define SERVER_MAX_CONNECTIONS  4      /* max clients connected at a time */

#define REQUEST_MSG_SIZE        1024   /* max size of request message */
#define REPLY_MSG_SIZE          1024   /* max size of reply message */

#ifndef SOCK_ERROR
#define SOCK_ERROR (-1)
#endif
#ifndef SOCK_OK
#define SOCK_OK    (0)
#endif
#define MAGIC_COOKIE 0xff8b344f /* value = -7654321 */
#define SOCK_ADDR_SIZE sizeof(struct sockaddr_in))



/* structure for requests from clients to server */

struct request
{ 
   long  magic_cookie;
   long  command_type;
   long  command;
   long  par1;
   long  par2;
   long  reply;                        /* TRUE = request reply from server */
   char  message[REQUEST_MSG_SIZE];    /* message buffer */
   long  msgLen;                       // length of message text
};



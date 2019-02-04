
/* cfSockCli - send requests to server over a TCP socket 
               for configuration utility */
/*
  DESCRIPTION
  This file contains the client-side of the VxWorks TCP example code.
*/

/* includes */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <string.h>


#include "cfSockCli.h"


static int   gSocketFd;           /* socket file descriptor */
static int   gSocketKeepOpen = 0; /* indicator to keep the socket open:
				     0 means close it right away;
				     1 means keep it open
				  */

/****************************************************************************
 *
 * cfSockCli - send requests to server over a TCP socket for configuration utility
 *
 * This routine connects over a TCP socket to a server, and sends a
 * user-provided message to the server. Optionally, this routine
 * waits for the server's reply message.
 *
 * This routine may be invoked as follows:
 *       -> cfSockCli "remoteSystem"
 *       Message to send:
 *       Hello out there
 *       Would you like a reply (Y or N):
 *       y
 *       value = 0 = 0x0
 *       -> MESSAGE FROM SERVER:
 *       Server received your message
 *
 * RETURNS: OK, or ERROR if the message could not be sent to the server.
*/


int cfSockCliOpen (int crate_number, int keepopen);
int cfSockCliSend (int crate_number, struct request *myRequest, 
		   struct request *serverReply);
int cfSockCliClose();


int cfSockCli ( 
	       int crate_number,      /* which crate to address */
	       struct request *myRequest,    /* request to send to server */
	       struct request *serverReply   /* reply from server */
 )
{
  int status;
  
  //  printf("cfSockCli: gSocketKeepOpen=%d\n",gSocketKeepOpen);
  //  if (gSocketKeepOpen==0){
  if (1==1){
    status = cfSockCliOpen(crate_number, 0);
    if (status == SOCK_OK) {
      status = cfSockCliSend(crate_number, myRequest, serverReply);
      if (status == SOCK_OK) {
	status = cfSockCliClose();
      }
    }
  } else {
    status = cfSockCliSend(crate_number, myRequest, serverReply);
  }
  return status;
}



int cfSockCliOpen (int crate_number, int keepopen) {
  struct sockaddr_in  serverAddr;    /* server's socket address */
  int                 sockAddrSize;  /* size of socket address structure */
  char optval;
  char * servername;

  /*  printf("cfSockCliOpen: gSocketKeepOpen=%d; keepopen=%d\n",
      gSocketKeepOpen, keepopen);
  */

  gSocketKeepOpen = 0;
  /* create client's socket */
  switch (crate_number) {
  case Crate_CountingHouse:
    servername = ServerName_CountingHouse;
    break;
     case Crate_LeftSpect:
    servername = ServerName_LeftSpect;
    break;
  case Crate_RightSpect:
    servername = ServerName_RightSpect;
    break;
  case Crate_Injector:
    servername = ServerName_Injector;
    break; 
  case Crate_Qwvmets:
    servername = ServerName_QwTSCrate;
    break; 

  default:
    return (SOCK_ERROR);
  }  

  if ((gSocketFd = socket (PF_INET, SOCK_STREAM, 0)) == SOCK_ERROR)
    {
      perror ("socket");
      return (SOCK_ERROR);
    }
  
  optval = 1;
  int test = 0;
  test = setsockopt ( gSocketFd, IPPROTO_TCP, TCP_NODELAY, 
	       (char *) &optval,
	       4 ); /* black magic from Chowdhary's code */


  test = setsockopt(gSocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* bind not required - port number is dynamic */
    /* build server socket address */
  
  sockAddrSize = sizeof (struct sockaddr_in);
  bzero ((char *) &serverAddr, sockAddrSize);
  serverAddr.sin_family = PF_INET;
  serverAddr.sin_port = htons (SERVER_PORT_NUM);
  
  if ((serverAddr.sin_addr.s_addr = inet_addr (servername)) == SOCK_ERROR)
    {
      perror ("unknown server name");
      close (gSocketFd);
      return (SOCK_ERROR);
    }
  
  /* connect to server */  
  if (connect (gSocketFd, (struct sockaddr *) &serverAddr, sockAddrSize) == SOCK_ERROR)
    {
      perror ("connect");
      close (gSocketFd);
      return (SOCK_ERROR);
    }
  gSocketKeepOpen = keepopen;
  return (SOCK_OK);
};

int cfSockCliSend ( 
	       int crate_number,      /* which crate to address */
	       struct request *myRequest,    /* request to send to server */
	       struct request *serverReply   /* reply from server */
 )
{
  int  nWrite;
  int i, j;
  int keepopen = gSocketKeepOpen;

  /*  printf("cfSockCliSend: gSocketKeepOpen=%d; keepopen=%d\n",
      gSocketKeepOpen, keepopen);
  */

  gSocketKeepOpen = 0;
  /*
    buildRequestInteractive(myRequest);
    printf("\n");
    printf("your command type will be: %d \n",ntohl(myRequest->command_type));
    printf("your command will be: %d \n",ntohl(myRequest->command));
    printf("your magic_cookie will be: %d \n",ntohl(myRequest->magic_cookie));
    printf("your request param_1 will be: %d \n",ntohl(myRequest->par1));
    printf("your request param_2 will be: %d \n",ntohl(myRequest->par2));
  */

  /* send request to server */
  nWrite = send (gSocketFd, (char *) myRequest, sizeof (*myRequest), 0);
  if ( nWrite != sizeof(*myRequest) ) 
    {
      printf ( "cfSockCli WARN: nWrite = %d, sizeof (*myRequest) = %d\n",
		     nWrite, sizeof (*myRequest) );
      perror ("Connection aborted on error");
      close(gSocketFd);
      return (SOCK_ERROR);
    }
  
  if (myRequest->reply)        /* if expecting reply, read and display it */
    {
      i = recv ( gSocketFd, (char *) serverReply, sizeof(*serverReply), 0 );

      if ( i == SOCK_ERROR ) 
	{
	  perror ("Error reading result\n");
	  return (SOCK_ERROR);
	}
      /* The reason this while loop exists is that there
       * is a remote possibility of the above recv returning
       * less than sizeof(*serverReply) bytes.  This is because a recv returns
       * as soon as there is some data, and will not wait for
       * all of the requested data to arrive.  Since sizeof(*serverReply) bytes
       * is relatively small compared to the allowed TCP
       * packet sizes, a partial receive is unlikely.  If
       * the reply size were 2048 bytes instead,
       * a partial receive would be far more likely.
       * This loop will keep receiving until all sizeof(*serverReply) bytes
       * have been received, thus guaranteeing that the
       * next recv at the top of the loop will start at
       * the beginning of the next reply.
       */
      while ( i < sizeof(*serverReply) ) {
	j = recv( gSocketFd, ((char *) serverReply)+i, sizeof(*serverReply)-i, 0 );
	if ( j == SOCK_ERROR ) 
	  {
	    perror ("Error reading result\n");
	    close(gSocketFd);
	    return (SOCK_ERROR);
	  }
	i += j;
      }
      //      handleReplyInteractive(serverReply);
    }
  gSocketKeepOpen = keepopen;
  /*  printf("cfSockCliSend: End; gSocketKeepOpen=%d\n", gSocketKeepOpen); */
  return (SOCK_OK);
};

int cfSockCliClose()
{
  gSocketKeepOpen = 0;
  close (gSocketFd);
  /*  printf("cfSockCliClose: gSocketKeepOpen=%d\n", gSocketKeepOpen); */
  return (SOCK_OK);
}

/*

  main (int argc, char *argv[])
  {
    long command = 1, par1 = 1, par2 = 1;
    char *reply="Y";
    char *msg = "one";
    cfSockCommand(Crate_CountingHouse,command,par1,par2,reply,msg);
        printf ("MESSAGE FROM SERVER:  <%s> \n", msg);
        printf("Server reply command: %d \n",command);
        printf("Server reply param_1: %d \n",par1);
        printf("Server reply param_2: %d \n\n",par2);
  }
  main (int argc, char *argv[])
  {
  struct request myRequest;     // request to send to server 
  struct request serverReply;   // reply from server 
  
  
  //    cfSockCli("129.57.164.13");
  buildRequestInteractive(&myRequest);
  printf("\n");
  printf("your command will be: %d \n",ntohl(myRequest.command));
  printf("your magic_cookie will be: %d \n",ntohl(myRequest.magic_cookie));
  printf("your request param_1 will be: %d \n",ntohl(myRequest.par1));
  printf("your request param_2 will be: %d \n",ntohl(myRequest.par2));
  
  cfSockCli(Crate_CountingHouse, &myRequest,&serverReply);
    
  if (myRequest.reply)        // if expecting reply, read and display it
  handleReplyInteractive(&serverReply);
  
  }
*/

int cfSockCommand(int crate_number,
		  long command_type, long command,  
		  long req_param,    long req_param_2, 
		  char *reply,       char *msg  )
{
  struct request myRequest;     // request to send to server 
  struct request serverReply;   // reply from server 
  int mlen;
  int errFlag;
  //  char *reply = "Y";

  myRequest.command_type = htonl(command_type);
  myRequest.command = htonl(command);
  myRequest.magic_cookie = htonl(MAGIC_COOKIE);
  myRequest.par1 = htonl(req_param);
  myRequest.par2 = htonl(req_param_2);
  switch (*reply) 
   {
    case 'y':
    case 'Y': myRequest.reply = TRUE;
      break;
    default: myRequest.reply = FALSE;
      break;
    }
  mlen = strlen(msg);
  strncpy(myRequest.message,msg,REQUEST_MSG_SIZE);
  myRequest.msgLen = htonl(mlen);
  myRequest.message[mlen] = '\0';

  //  printf("\n");
  //  printf ("Message to send:   <%s> \n",myRequest.message);
  //  printf("your command type will be: %d \n",ntohl(myRequest.command_type));
  //  printf("your command will be: %d \n",ntohl(myRequest.command));
  //  printf("your request param_1 will be: %d \n",ntohl(myRequest.par1));
  //  printf("your request param_2 will be: %d \n",ntohl(myRequest.par2));
  //  printf("Requested reply?  : %s \n",reply);
  //  printf("\n");
  
  errFlag = cfSockCli(crate_number,&myRequest,&serverReply);
  if (errFlag != SOCK_OK) return errFlag;
    
  // if expecting reply, read and display it
  if (myRequest.reply) {
	msg = strcpy(serverReply.message,serverReply.message);
        command_type = htonl(serverReply.command_type);
        command = htonl(serverReply.command);
        req_param = htonl(serverReply.par1);
        req_param_2 = htonl(serverReply.par2);
	//	        printf ("MESSAGE FROM SERVER:\n%s\n", msg);
	//	        printf("Server reply command: %d \n",command);
	//	        printf("Server reply param_1: %d \n",req_param);
	//	        printf("Server reply param_2: %d \n\n",req_param_2);
	//        handleReplyInteractive(&serverReply);
  }
  return errFlag;
}

int GreenSockCommand(int crate_number, struct greenRequest *gRequest)
{
  struct request myRequest;     // request to send to server 
  struct request serverReply;   // reply from server 
  //  char *reply;
  int mlen;
  int errFlag;
  
  //  char *reply = "Y";

  myRequest.command_type = htonl(gRequest->command_type);
  myRequest.command = htonl(gRequest->command);
  myRequest.magic_cookie = htonl(MAGIC_COOKIE);
  myRequest.par1 = htonl(gRequest->par1);
  myRequest.par2 = htonl(gRequest->par2);

  switch (*(gRequest->reply))
    {
    case 'y':
    case 'Y': myRequest.reply = TRUE;
      break;
    default: myRequest.reply = FALSE;
      break;
    }
  mlen = strlen(gRequest->message);
  strncpy(myRequest.message,gRequest->message,REQUEST_MSG_SIZE);
  myRequest.msgLen = htonl(mlen);
  myRequest.message[mlen] = '\0';

  //  printf("\n");
  //  printf ("Message to send:   <%s> \n",myRequest.message);
  //  printf("your command will be: %d \n",ntohl(myRequest.command));
  //  printf("your request param_1 will be: %d \n",ntohl(myRequest.par1));
  //  printf("your request param_2 will be: %d \n",ntohl(myRequest.par2));
  //  printf("Requested reply?  : %s \n",gRequest->reply);
  //  printf("\n");
  
  errFlag = cfSockCli(crate_number, &myRequest,&serverReply);

  if (errFlag != SOCK_OK) return errFlag;
    
  // if expecting reply, read and display it
  if (myRequest.reply) {
    //	gRequest->message = strcpy(serverReply.message,serverReply.message);
    strcpy(gRequest->message,serverReply.message);
    gRequest->command_type = htonl(serverReply.command_type);
    gRequest->command = htonl(serverReply.command);
    gRequest->par1 = htonl(serverReply.par1);
    gRequest->par2 = htonl(serverReply.par2);
    
    //    printf ("cfSockCli: MESSAGE FROM SERVER:   %s\n", gRequest->message);
    //    printf("Server reply command: %d \n",gRequest->command);
    //    printf("Server reply param_1: %d \n",gRequest->req_param);
    //    printf("Server reply param_2: %d \n\n",gRequest->req_param_2);
    //        handleReplyInteractive(&serverReply);
  }
  return errFlag;
}


void buildRequestInteractive(struct request *myRequest) {
  /* build request, prompting user for message */
  int                 mlen;          /* length of message */  
  char                reply;         /* if TRUE, expect reply back */
  
  printf ("Message to send: \n");
  mlen = read (STD_IN, myRequest->message, REQUEST_MSG_SIZE);
  myRequest->msgLen = htonl(mlen);
  myRequest->message[mlen - 1] = '\0';
  
  myRequest->command_type = htonl(1);
  printf("your command type will be: %d \n",ntohl(myRequest->command_type));
  myRequest->command = htonl(10);
  printf("your command will be: %d \n",ntohl(myRequest->command));
  myRequest->magic_cookie = htonl(MAGIC_COOKIE);
  printf("your magic_cookie will be: %d \n",ntohl(myRequest->magic_cookie));
  myRequest->par1 = htonl(1);
  printf("your request param_1 will be: %d \n",ntohl(myRequest->par1));
  myRequest->par2 = htonl(2);
  printf("your request param_2 will be: %d \n",ntohl(myRequest->par2));


  printf ("Would you like a reply (Y or N): \n");
  read (STD_IN, &reply, 1);
  switch (reply)
    {
    case 'y':
    case 'Y': myRequest->reply = TRUE;
      break;
    default: myRequest->reply = FALSE;
      break;
    }
}

void handleReplyInteractive(struct request *serverReply) {
  /* build request, prompting user for message */
      printf ("MESSAGE FROM SERVER:\n%s\n", serverReply->message);
      printf("Server reply command type: %d \n",ntohl(serverReply->command_type));
      printf("Server reply command: %d \n",ntohl(serverReply->command));
      printf("Server reply param_1: %d \n",ntohl(serverReply->par1));
      printf("Server reply param_2: %d \n",ntohl(serverReply->par2));
      printf("Server reply msgLen: %d \n",ntohl(serverReply->msgLen));
}








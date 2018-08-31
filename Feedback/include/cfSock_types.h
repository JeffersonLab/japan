/* cfSock_types.h  - header used by both server of cfSock and           */
/* GreenMonster defines codes to specify the types of commands.         */
/* Unique codes here will specify unique calls make by cfSockSer, which */
/* will then be handled by a separate library dedicated to serving the  */
/* config utility calls for that object type. This will make cfSockCli  */
/* and cfSockSer independent of various commands to be issued.          */


#define COMMAND_HAPTB         1000
#define COMMAND_BMW           2000
#define COMMAND_FDBK          3000
#define COMMAND_HAPADC        4000
#define COMMAND_SCAN          5000


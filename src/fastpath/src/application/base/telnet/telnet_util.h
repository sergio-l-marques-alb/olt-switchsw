/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   telnet_util.h
*
* @purpose    Outbound Telnet Implementation
*
* @component  Outbound Telnet Component
*
* @comments   none
*
* @create     02/27/2004
* @modify     08/18/2004
*
* @author     anindya
*
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/
#ifndef _TELNET_UTIL_H_
#define _TELNET_UTIL_H_ 


#define L7_MAX_BUFFER_LENGTH       4096
#define L7_MAX_IAC_BUFFER_LENGTH   128


#define CONST     0x01

#define TELNET_SET_CONFIG_DATA_DIRTY      {pTelnetMapCfgData->cfgHdr.dataChanged = L7_TRUE;} while(0);


/* Outbound Telnet states */
typedef enum
{
  TS_0    = 1,
  TS_IAC  = 2,
  TS_OPT  = 3,
  TS_SUB1 = 4,
  TS_SUB2 = 5
} L7_OBT_STATES_t;


/* Outbound Telnet control context */
typedef struct telnetMapCtrl_s
{
  void *              telnetDataSemId;     /* semaphore object handle */
  L7_uint32           telnetTaskId;
  L7_BOOL             telnetInitialized;
} telnetMapCtrl_t;


/* Outbound Telnet input/output structure */
typedef struct
{
  L7_uint32    clientSock;   /* client socket */
  L7_uint32    serverSock;   /* server socket */
  L7_char8   * ipAddr;

/* global buffer */
  L7_char8   * buf;
  L7_short16   len;
  L7_uchar8    telstate;   /* telnet negotiation state from server host input*/
  L7_uchar8    telwish;    /* DO, DONT, WILL, WONT */
  L7_uchar8    telflags;

/* buffer to handle telnet negotiations */
  L7_char8   * iacbuf;
  L7_short16   iaclen;
  L7_uint32    clientSockWrite;    /* socket to use for output */

} ioHandle_t;


/* Outbound Telnet options */
typedef struct
{
  L7_BOOL       termtype;  /* non-zero if received "DO TERMTYPE" */
  L7_BOOL       rcvbinary;
  L7_BOOL       sndbinary;
  L7_BOOL       sga;
} options_t;


/* Outbound Telnet modes */
typedef struct
{
  L7_BOOL              debugMode;   /* debug mode */
  L7_BOOL              lineMode;    /* linemode */
  L7_BOOL              noechoMode;  /* local echo "OFF" */
} modes_t;


#ifdef _L7_OS_LINUX_
typedef struct
{
  struct termios termios_raw;
  struct termios termios_def;
  struct termios termios_exit;
} term_t;
#endif



/*********************************************************************
* @purpose  Initialize the Outbound Telnet layer application
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    Requires that the system router ID be configured.
*
* @end
*********************************************************************/

L7_RC_t telnetMapAppsInit(void);

/*********************************************************************
* @purpose  Apply Outbound Telnet config data
*
* @param    void
*
* @returns  L7_SUCCESS  Config data applied
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetApplyConfigData(void);

/*********************************************************************
* @purpose  Save Outbound Telnet user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetSave(void);

/*********************************************************************
* @purpose  Restore Outbound Telnet user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetRestoreProcess(void);

/*********************************************************************
* @purpose  Check if Outbound Telnet user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/

L7_BOOL telnetHasDataChanged(void);
void telnetResetDataChanged(void);
/*********************************************************************
* @purpose  Build default Outbound Telnet config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetBuildDefaultConfigData(L7_uint32 ver);


/*********************************************************************
* @purpose purge Outbound Telnet config  and pool data
*
* @param    void
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @end
*********************************************************************/
L7_RC_t telnetPurgeData(void);


/* telnet_main.c */

/*********************************************************************
* @purpose  Initialize all the global and static external variables
*
* @param    telnetParams_t telnetParams  @b((input)) Parameter structure
*
* @returns  void
*
* @notes    none
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @end
*********************************************************************/

L7_RC_t telnetInitialize(telnetParams_t *telnetParams);

/*************************************************************************
* @purpose  Create and allocte a socket on the remote host
*
* @param    telnetParams_t       telnetParams @b((input)) Parameter structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if error
*
* @notes    none
*
* @end
**************************************************************************/

L7_int32 telnetNewCon(telnetParams_t *telnetParams);

/*********************************************************************
* @purpose  Convert the hostname to the corresponding IP address
*
* @param    telnetParams        @b{(input)}  Pointer to parameter list
* @param    pHost               @b{(input)}  Pointer to host
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetServerIpGet(telnetParams_t *telnetParams, struct in_addr *pHost);

/*********************************************************************
* @purpose  Allocate a socket and connect to it
*
* @param    addr         @b{(input)} IP address of the server host
* @param    port         @b{(input)} Port number of the server host
*
* @returns  Socket created
* @returns  L7_ERROR, if error
*
* @notes    none
*
* @end
*********************************************************************/

L7_int32 telnetRemoteConnect(struct in_addr addr, L7_uint32 port);

/*********************************************************************
* @purpose  Set the outbound telnet operational mode
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetConModeSet(ioHandle_t *ioInfo);

/************************************************************************
* @purpose  Set Outbound Telnet operational mode as CHARACTER mode
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    void
*
* @end
************************************************************************/

L7_RC_t telnetCharModeSet(ioHandle_t *ioInfo);

/***********************************************************************
* @purpose  Set Outbound Telnet operational mode as LINE mode
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    void
*
* @end
************************************************************************/

L7_RC_t telnetLineModeSet(ioHandle_t *ioInfo);

/***********************************************************************
* @purpose  Reset the switch mode before exiting Outbound Telnet
*
* @param    clientSock          @b{(input)}  Client socket
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    void
*
* @end
************************************************************************/

L7_RC_t telnetExitModeSet(L7_uint32 clientSock);

/*********************************************************************
* @purpose  Process the data/control information coming from the
*           client host
*
* @param    clientSock          @b{(input)}  Client socket
* @param    serverSock          @b{(input)}  Server socket
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetClientInputHandle(L7_uint32 clientSock, L7_uint32 serverSock);

/*********************************************************************
* @purpose  Process the data/control information coming from the
            server host
*
* @param    clientSock          @b{(input)}  Client socket
* @param    serverSock          @b{(input)}  Server socket
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetServerInputHandle(L7_uint32 clientSock, L7_uint32 serverSock);

/*********************************************************************
* @purpose  Handle the outbound telnet escape sequence commands
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t  telnetConEscape(ioHandle_t *ioInfo);

/*************************************************************************
* @purpose  Process different telnet options
*
* @param    ioInfo          @b{(input)} Pointer to i/o structure
* @param    telopt          @b{(input)} Telnet option
*
* @returns  void
*
* @notes    void
*
* @end
**************************************************************************/

void telnetOptionsMode(ioHandle_t *ioInfo, L7_uchar8 telopt);

/*************************************************************************
* @purpose  Process different telnet suboption negotiation
*
* @param    ioInfo       @b{(input)} Pointer to i/o structure
* @param    tel          @b{(input)} Telnet option/command/state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    void
*
* @end
**************************************************************************/

L7_RC_t telnetSubnegMode(ioHandle_t *ioInfo, L7_uchar8 tel);

/*************************************************************************
* @purpose  Send reply back to the server for telnet BINARY option
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Client sends reply corresponding to WILL/WONT from server host
*
* @end
**************************************************************************/

L7_RC_t telnetReceiveBinary(ioHandle_t *ioInfo);

/*************************************************************************
* @purpose  Send reply back to the server for telnet BINARY option
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Client sends reply corresponding to DO/DONT from server host
*
* @end
**************************************************************************/

L7_RC_t telnetSendBinary(ioHandle_t *ioInfo);

/*********************************************************************
* @purpose  Send reply back to the server for telnet ECHO option
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetEcho(ioHandle_t *ioInfo);

/*********************************************************************
* @purpose  Send reply back to the server for telnet SUPPRESS 
*           GO-AHEAD option
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Server host always sends will/wont, client do/dont
*
* @end
*********************************************************************/

L7_RC_t telnetSga(ioHandle_t *ioInfo);

/*********************************************************************
* @purpose  Send reply back to the server for telnet STATUS option
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetStatus(ioHandle_t *ioInfo);

/*********************************************************************
* @purpose  Send reply back to the server for telnet TERMINAL TYPE
*           option
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Server host always sends do/dont, client will/wont
*
* @end
**********************************************************************/

L7_RC_t telnetTtype(ioHandle_t *ioInfo);

/*********************************************************************
* @purpose  Send the terminal type of the client to the server
*
* @param    *ioInfo             @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetTtypeSend(ioHandle_t *ioInfo);

/*********************************************************************
* @purpose  Send reply back to the server for unsupported telnet options
*
* @param    ioInfo          @b{(input)} Pointer to i/o structure
* @param    telopt          @b{(input)} Telnet option
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetNotSupported(ioHandle_t *ioInfo, L7_char8 telopt);

/*********************************************************************
* @purpose  Add a single character to the buffer
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
* @param    telopt          @b{(input)} Character to add
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetBufferAdd(ioHandle_t *ioInfo, L7_uchar8 character);

/*********************************************************************
* @purpose  Add two characters to the buffer
*
* @param    ioInfo          @b{(input)} Pointer to i/o structure
* @param    tewish          @b{(input)} DO/DONT/WISH/WONT
* @param    telopt          @b{(input)} telnet option
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

void telnetBuffer2Add(ioHandle_t *ioInfo, L7_uchar8 telwish, L7_uchar8 telopt);

/*********************************************************************
* @purpose  Flush the buffer content to the server host
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetBufferFlush(ioHandle_t *ioInfo);

/*********************************************************************
* @purpose  Displays the Outbound Telnet options in effect
*
* @param    ioInfo              @b{(input)}  Pointer to i/o structure
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetDebugShow(ioHandle_t *ioInfo);

/*********************************************************************
* @purpose  Close an Outbound Telnet session
*
* @param    clientSock          @b{(input)}  Client socket
* @param    serverSock          @b{(input)}  Server socket
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetDisconnect(L7_uint32 clientSock, L7_uint32 serverSock);

/*********************************************************************
* @purpose  Delete all stored indexes for Outbound Telnet sessions
*
* @param    void
*
* @returns  L7_TRUE             index added
* @returns  L7_FALSE            index not added
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t telnetSessionRemoveAll(void);

/*********************************************************************
* @purpose  Find index for an Outbound Telnet session
*
* @param    clientSock       	@b{(input)}  Client socket
* @param    sessionIndex        @b{(output)}  Pointer to index
*
* @returns  L7_TRUE             index found
* @returns  L7_FALSE            index not found
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetSessionIndexFind(L7_uint32 clientSock, L7_uint32 *sessionIndex);

/*********************************************************************
* @purpose  Delete index for an Outbound Telnet session
*
* @param    clientSock       	@b{(input)}  Client socket
*
* @returns  L7_TRUE             index found
* @returns  L7_FALSE            index not found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t telnetSessionIndexRemove(L7_uint32 clientSock);

/*********************************************************************
* @purpose  Add index for an Outbound Telnet session
*
* @param    clientSock       	@b{(input)}  Client socket
* @param    sessionIndex        @b{(output)}  Pointer to index
*
* @returns  L7_TRUE             index added
* @returns  L7_FALSE            index not added
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t telnetSessionIndexAdd(L7_uint32 clientSock, L7_uint32 clientSockWrite, L7_uint32 *sessionIndex);

/* telnet_migrate.c */
/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void telnetMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* _TELNET_UTIL_H_*/

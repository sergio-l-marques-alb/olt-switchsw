/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_control.h
*
* @purpose TACACS+ Client control header file
*
* @component tacacs+
*
* @comments none
*
* @create 03/10/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_TACACS_CONTROL_H
#define INCLUDE_TACACS_CONTROL_H

#include "l7_cnfgr_api.h"

#define TACACS_ERROR_SEVERE(format,args...) \
{                                           \
  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TACACS_COMPONENT_ID,format,##args);                   \
}


#define TACACS_TIMER_INTERVAL   1000  /* wake up every second when there are pending requests */

/*
** TACACS+ Various Events
*/
typedef enum tacacsEvents_s
{
  TACACS_EVENT_TIMEOUT = 0,
  TACACS_EVENT_CNFGR_INIT,
  TACACS_EVENT_RESPONSE_RECEIVED,
  TACACS_EVENT_SOCKET_ERROR,
  TACACS_EVENT_AUTH_REQUEST,
  TACACS_EVENT_SERVER_CONNECTED,
  TACACS_EVENT_SERVER_CONNECT_FAILED,
  TACACS_EVENT_SERVER_REMOVE,
  TACACS_EVENT_HOSTNAME_RESOLVE
} tacacsEvents_t;

typedef struct tacacsConnectMsg_s
{
  L7_uint32         ip;       /* server ip */
  L7_uint32         port;     /* server port */
  L7_uint32         socket;   /* socket fd */
} tacacsConnectMsg_t;

/*
** TACACS+ message structure for tacacs_task processing
*/
typedef struct
{
  L7_uint32 event;
  union
  {
    L7_CNFGR_CMD_DATA_t    cmdData;
    tacacsAuthSession_t   *session;
    tacacsConnectMsg_t     connectMsg;
    tacacsRxPacket_t      *packet;
    L7_uchar8             hostName[L7_DNS_HOST_NAME_LEN_MAX];
    L7_uint32              ip;
  } data;
} tacacsMsg_t;

/* TACACS+ Client Registrar Data */
typedef struct
{
  L7_RC_t            (*notifyFunction)(L7_uint32 status,
                                       L7_uint32 correlator,
                                       L7_uint32 accessLevel);
} tacacsNotifyEntry_t;


/******************************************************
**        Internal function prototypes               **
******************************************************/

L7_RC_t tacacsStartTasks();
void tacacsTask(void);
void tacacsRxTask(void);
void tacacsConnectTask(L7_uint32 argc, void *argv);
L7_RC_t tacacsTimeoutHandler(void);
void tacacsTimerAdd(void);
void tacacsTimerDelete(void);
void tacacsServerResolveHostName(L7_uchar8 *hostName);
#endif /* INCLUDE_TACACS_CONTROL_H */

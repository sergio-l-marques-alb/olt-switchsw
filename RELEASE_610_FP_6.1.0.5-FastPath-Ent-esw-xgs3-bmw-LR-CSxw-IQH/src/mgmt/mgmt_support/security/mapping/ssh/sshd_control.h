/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     sshd_control.h
*
* @purpose      SSHD control header
*
* @component    sshd
*
* @comments     none
*
* @create       09/15/2003
*
* @author       dcbii
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_SSHD_CONTROL_H
#define INCLUDE_SSHD_CONTROL_H

#include "commdefs.h"
#include "datatypes.h"

/* Misc. SSHD defines */

#define SSHD_LISTEN_TASK_NAME_LEN 20

/*
SSHD msg event types
*/

typedef enum sshdEvents_s
{
  /* Events from the UI tasks */

  SSHD_ADMIN_MODE_SET,
  SSHD_PROTO_LEVEL_SET,
  SSHD_PORT_NUMBER_SET,
  SSHD_KEY_GENERATE_EVENT,
  /* Configurator event */

  SSHD_CNFGR_EVENT,

  SSHD_EVENTS /* keep this last */

} sshdEvents_t;

/*
SSHD message structure for sshdTask processing
*/

typedef struct
{
  L7_uint32             event;
  union
  {
    L7_uint32           mode;
    L7_uint32           number;
    L7_CNFGR_CMD_DATA_t CmdData;

  } data;

} sshdMsg_t;

/*
Internal function prototypes
*/

L7_RC_t sshdStartTasks(void);
void    sshdTask(void);
L7_RC_t sshdListenTaskStartup(void);
L7_RC_t sshdListenTaskCleanup(void);
L7_RC_t sshdIssueCmd(L7_uint32 event, void *data);
L7_RC_t sshdFillMsg(void *data, sshdMsg_t *msg);
void    sshdDispatchCmd(sshdMsg_t msg);
L7_RC_t sshdEventAdminModeSet(L7_uint32 mode);
L7_RC_t sshdEventProtoLevelSet(L7_uint32 number);
L7_RC_t sshdListenTaskRefresh(void);
L7_RC_t sshdEventPortNumberSet(L7_uint32 port);
#endif /* INCLUDE_SSHD_CONTROL_H */


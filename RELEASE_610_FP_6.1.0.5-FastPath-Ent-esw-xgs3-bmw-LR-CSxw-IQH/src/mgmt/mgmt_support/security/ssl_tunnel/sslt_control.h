/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sslt_control.h
*
* @purpose SSL Tunnel control header
*
* @component sslt
*
* @comments none
*
* @create 07/15/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_SSLT_CONTROL_H
#define INCLUDE_SSLT_CONTROL_H

#include <openssl/ssl.h>
#include "sslt_exports.h"
#include "l7_cnfgr_api.h"

/* Misc. SSLT defines */
#define SSLT_CONTEXT 1
#define SSLT_LISTEN_TASK_NAME_LEN 20
#define SSLT_HOST_PORT_STR_LEN 6

/*
** SSLT msg event types
*/
typedef enum ssltEvents_s
{
  /* Events from the UI tasks */
  SSLT_ADMIN_MODE_SET,
  SSLT_SECURE_PORT_SET,
  SSLT_UNSECURE_SERVER_ADDR_SET,
  SSLT_UNSECURE_PORT_SET,
  SSLT_PEM_PASS_PHRASE_SET,
  SSLT_PROTOCOL_SSL30_SET,
  SSLT_PROTOCOL_TLS10_SET,
  SSLT_CNFGR_EVENT,
  SSLT_CERTIFICATE_ACTIVE_SET,
  SSLT_AUX_MODE_SET,
  SSLT_AUX_SECURE_PORT1_SET,
  SSLT_AUX_SECURE_PORT2_SET,
  SSLT_EVENTS /* keep this last */

} ssltEvents_t;

/*
** SSLT listening port types
*/
typedef enum
{
  /* Up to 3 secure listening types */
  SSLT_SECURE_ADMIN,    /* switch mgmt port */
  SSLT_SECURE_AUX_PORT1,/* auxiliary port */
  SSLT_SECURE_AUX_PORT2 /* auxiliary port */
} ssltSecureTypes_t;

/*
** SSLT message structure for ssltTask processing
*/
typedef struct
{
  L7_uint32             event;
  union
  {
    L7_uint32           mode;
    L7_uint32           number;
    L7_uchar8           string[L7_SSLT_PEM_PASS_PHRASE_MAX];
    L7_CNFGR_CMD_DATA_t CmdData;

  } data;

} ssltMsg_t;


/*
** Internal function prototypes
*/

L7_RC_t ssltStartTasks(void);

void ssltTask(void);

L7_RC_t ssltListenTaskStartup(ssltSecureTypes_t secureType);

void ssltListenTask(int argc, L7_uint32 *argv[]);

void ssltConnectionTask(int argc,
                        L7_uint32 *argv[]);

L7_RC_t ssltIssueCmd(L7_uint32 event,
                     void *data);

L7_RC_t ssltFillMsg(void *data,
                    ssltMsg_t *msg);

void ssltDispatchCmd(ssltMsg_t msg);

L7_RC_t ssltEventAdminModeSet(L7_uint32 mode);

L7_RC_t ssltEventSecurePortSet(L7_uint32 number);

L7_RC_t ssltEventUnSecurePortSet(L7_uint32 number);

L7_RC_t ssltEventUnSecureServerAddrSet(L7_uint32 number);

L7_RC_t ssltEventPemPassPhraseSet(L7_uchar8 *string);

L7_RC_t ssltEventProtocolLevelSet(L7_uint32 protocolId,
                                  L7_uint32 mode);

L7_RC_t ssltEventHttpModeSet(L7_uint32 mode);

L7_RC_t ssltEventCertificateActiveSet(L7_uint32 number);

L7_RC_t ssltEventAuxModeSet(L7_uint32 mode);

L7_RC_t ssltEventSecureAuxPort1Set(L7_uint32 number);

L7_RC_t ssltEventSecureAuxPort2Set(L7_uint32 number);

#endif /* INCLUDE_SSLT_CONTROL_H */


/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename macal.h
*
* @purpose Managemant Access Control and Administration List (MACAL) definitions
*
* @component Managemant Access Control and Administration List (MACAL)
*
* @comments none
*
* @create 05/05/2005
*
* @author stamboli
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#ifndef MACAL_H
#define MACAL_H

#include "l7_cnfgr_api.h"
#include "sysnet_api.h"
#include "defaultconfig.h"
#include "commdefs.h"
#include "macal_exports.h"
#include "macal_api.h"

#define MACAL_QUEUE          "macal_Queue"
#define MACAL_MSG_COUNT      16
#define MACAL_MSG_SIZE        sizeof(macalMgmtMsg_t)

#define MACAL_CFG_FILENAME    "macalCfgData.cfg"
#define MACAL_CFG_VER_1       0x1
#define MACAL_CFG_VER_CURRENT MACAL_CFG_VER_1

#define MACAL_MAX_LISTNAME_LEN 32
#define MACAL_MIN_PRIORITY 1
#define MACAL_MAX_PRIORITY 64
#define ALL_RULES 0

#define MACAL_TELNET_PORT  L7_ACL_L4PORT_TELNET
#define MACAL_HTTP_PORT    L7_ACL_L4PORT_HTTP       /* TODO: replace with a call to SSH/SSL component */
#define MACAL_HTTPS_PORT   L7_SSLT_SECURE_PORT      /* TODO: replace with a call to SSH/SSL component */
#define MACAL_SNMP_PORT    L7_ACL_L4PORT_SNMP
#define MACAL_SNTP_PORT    123
#define MACAL_SSH_PORT     22 /* SSHD_PORT_NUM in ssh_defines.h */
#define MACAL_TFTP_DOWNLOAD_L4PORT 7700 /*Source L4 port used for tftp download*/
#define MACAL_TFTP_UPLOAD_L4PORT 7800 /*Source L4 port used for tftp upload*/

#define MACAL_TCP_SYN_BIT  0x02

typedef enum
{
  MACAL_PHASE_INIT_0 = 0,
  MACAL_PHASE_INIT_1,
  MACAL_PHASE_INIT_2,
  MACAL_PHASE_WMU,
  MACAL_PHASE_INIT_3,
  MACAL_PHASE_EXECUTE,
  MACAL_PHASE_UNCONFIG_1,
  MACAL_PHASE_UNCONFIG_2
} macalCnfgrState_t;

typedef enum
{
  L7_DELETED,
  L7_CREATED
} L7_CREATION_STATUS_t;

typedef enum
{
  L7_DEACTIVATED,
  L7_ACTIVATED
} L7_ACTIVATION_STATUS_t;

typedef struct
{
  L7_uint32       configMask;
  L7_uint32       action;                        /* action: permit/deny      */
  L7_uint32       serviceType;                   /* application protocol     */
  L7_uint32       srcIp;                         /* source ip                */
  L7_uint32       srcMask;                       /* source mask              */
  L7_uint32       ifNum;                         /* interface number:eth/LAG */ /* TODO For release 2: verify it has an IP Addr */
  L7_uint32       vlanId;                        /* VLAN ID                  */
  L7_ACTIVATION_STATUS_t activationStatus;
} macalRule_t;

typedef struct
{
  L7_char8                name[MACAL_MAX_LISTNAME_LEN + 1];
  macalRule_t             rule[MACAL_MAX_NUM_RULES + 1];
  L7_CREATION_STATUS_t    creationStatus;
  L7_ACTIVATION_STATUS_t  activationStatus;
} macalList_t;

typedef struct
{
  L7_fileHdr_t    cfgHdr;
  /* At this point, there is not enough information available about what you
     can and cannot do when this component is enabled/disabled. Also the
     enable/disable capability is available only through SNMP. So even if
     the following variable can store this value, it is not used anywhere in
     the code. */
  L7_BOOL         enabled;   /* if the MACAL feature is enabled. It should always be true. It's here for an SNMP object support. */
  L7_BOOL         consoleOnly; /* if true, prevent all access other than console */
  macalList_t     macalList; /* only one list in the system */
  L7_uint32       checkSum;  /* check sum of config file NOTE: needs to be last entry */
} macalCfgData_t;

/* Management Access Control and Administration List Management Message IDs */
typedef enum
{
  macalCnfgr = 1
} macalMgmtMessages_t;

typedef struct
{
  L7_uint32           msgId;    /* Of type macalMgmtMessages_t */
  L7_CNFGR_CMD_DATA_t CmdData;
} macalMgmtMsg_t;

/*********************************************************************
*
* @purpose  System Initialization for Management Access Control and Administration List component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t macalInit(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  System Init Undo for Management Access Control and Administration List component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void macalInitUndo();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes macalCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void macalCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes macalCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void macalCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes macalCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void macalCnfgrFiniPhase3Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
*
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           None.
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
*
* @purpose  To parse the configurator commands sent to macalTask
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void macalCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  Saves Management Access Control and Administration List file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 macalSave(void);

/*********************************************************************
* @purpose  Checks if Management Access Control and Administration List config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL macalHasDataChanged(void);

/*********************************************************************
* @purpose  Print current Management Access Control and Administration List
*           config values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t macalCfgDump(void);

/*********************************************************************
* @purpose  Apply static Management Access Control and Administration List
*           Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t macalApplyConfigData(void);

/*********************************************************************
* @purpose  Build default Management Access Control and Administration List config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void macalBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Build default Management Access Control and Administration List rules data
*
* @param    ruleNum   (input) rule number
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void macalBuildDefaultRules(L7_uint32 ruleNum);

/*********************************************************************
* @purpose  Start Management Access Control and Administration List task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t macalStartTask();

/*********************************************************************
*
* @purpose Task to handle all Management Access Control and Administration
*          List management messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void macalTask();

/*********************************************************************
*
* @purpose  Register/deregister with sysnet for packet processing
*
* @param    L7_BOOL  bRegister   @b((input)) Register (L7_TRUE)
*                                            Deregister (L7_FALSE)
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t macalSysnetRegisterDeregister(L7_BOOL bRegister);

/*********************************************************************
* @purpose  Intercept IP Management Packets,
*               verify access against the MACAL rules
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_DISCARD  if management is not allowed; drop the packet
* @returns  SYSNET_PDU_RC_IGNORED  if frame has been ignored; continue processing it
*
* @notes    none
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t macalPktIntercept(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc);

/********************* DEBUG Infrastrcture *********************/
#define MACAL_DEBUG_PKT_BUF_SIZE  1023
#define MACAL_DEBUG_PKT_LINES 8

extern L7_uint32 macalDebugLevel;

/* used to format an 32 bit IP address for display in dotted-decimal format */
#define FMTIPADDR(x)  ((x)>>24&0xFF),((x)>>16&0xFF),((x)>>8&0xFF),((x)>>0&0xFF)

#define LOG_MACAL_DEBUG_MSG(__fmt__, __args__... )                                \
          l7_logf(L7_LOG_SEVERITY_DEBUG, L7_MGMT_ACAL_COMPONENT_ID,               \
                  __FILE__, __LINE__, __fmt__, ## __args__)

/*********************************************************************
* @purpose  Dump a packet that has been handed to the management ACL interceptor
*
* @param    *pkt    Pointer to raw frame
*
* @returns  nothing
*
* @end
*********************************************************************/
void macalDebugPktDump(L7_uchar8 *pkt);

#endif /* MACAL_H */

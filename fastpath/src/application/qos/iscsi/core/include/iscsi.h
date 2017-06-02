/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi.h
*
* @purpose iSCSI utility function prototypes
*
* @component iSCSI
*
* @comments none
*
* @create 05/18/2008
*
* @end
*
**********************************************************************/
#ifndef ISCSI_H
#define ISCSI_H

#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "comm_mask.h"
#include "l3_addrdefs.h"
#include "sysnet_api.h"
#include "iscsi_exports.h"
#include "acl_exports.h"
#include "cos_exports.h"

extern L7_uint32       iscsiDebug;

#define ISCSI_DLOG(__fmt__, __args__...) \
  if (iscsiDebug == L7_TRUE) \
  { \
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_QOS_ISCSI_COMPONENT_ID, __fmt__, ## __args__); \
  }

typedef enum iscsiTraceFlags_e
{
  ISCSI_TRACE_INIT            =    0x1,
  ISCSI_TRACE_PDU_INTERCEPT   =    0x2,
  ISCSI_TRACE_PDU_RX          =    0x4,
  ISCSI_TRACE_TASK            =    0x8,
  ISCSI_TRACE_TIMER_LOW       =   0x10,
  ISCSI_TRACE_TIMER_HIGH      =   0x20,
  ISCSI_TRACE_DATABASE        =   0x40,
  ISCSI_TRACE_CHECKPOINT      =   0x80,
  ISCSI_TRACE_CKPT_PACKET     =  0x100,
  ISCSI_TRACE_BIT_9           =  0x200,
  ISCSI_TRACE_BIT_10          =  0x400,
  ISCSI_TRACE_BIT_11          =  0x800,
  ISCSI_TRACE_BIT_12          = 0x1000,
  ISCSI_TRACE_BIT_13          = 0x2000,
  ISCSI_TRACE_BIT_14          = 0x4000,
  ISCSI_TRACE_ALWAYS_ON       = 0x8000    /* used to force output */

} iscsiTraceFlags_t;

#define ISCSI_TRACE(__flags__, __fmt__, __args__...) \
  if (iscsiDebug & __flags__) \
  { \
    sysapiPrintf(__fmt__, ## __args__); \
  } 


typedef enum
{
  ISCSI_MSG_TYPE_PDU_RECEIVED= 1,
  ISCSI_MSG_TYPE_TIMER
} iscsiMsgTypes_t;

typedef struct
{
  L7_uint32       intIfNum;
  L7_uint32       dataLength;
  L7_uchar8       *iscsiPacketBuffer;
} iscsiPduReceiveParms_t;

typedef struct
{
  L7_uint32             msgId;
  union
  { 
    iscsiPduReceiveParms_t iscsiPduReceiveParms;
  } u;
} iscsiMsg_t;

#define ISCSI_PACKET_BUFFER_COUNT      8
#define ISCSI_PACKET_BUFFER_SIZE       L7_MAX_FRAME_SIZE

#define ISCSI_QUEUE_NAME      "iscsi_Queue"
/* reserve spots for control messages by assuring they cannot all contain packet received messages */
#define ISCSI_MSG_COUNT       (ISCSI_PACKET_BUFFER_COUNT + 8)
#define ISCSI_MSG_SIZE        sizeof(iscsiMsg_t)

#define ISCSI_IDLE_TIMEOUT               1000  /* milliseconds */

#define ISCSI_CFG_FILENAME    "iscsiCfgData.cfg"
#define ISCSI_CFG_VER_1       0x1
#define ISCSI_CFG_VER_CURRENT ISCSI_CFG_VER_1

/* The following macro computes whether an entry in the Target Table should be
 * included as "active".  This takes into account flags used to manage the table and 
 * an override variable that can be used to include configured but deactivated
 * default-configuration entries. These entries are required to remain in the table
 * even after they are deleted by user configuration to allow for text based configuration
 * output to be properly generated. 
 *
 * The following equation is derived from the following truth table.  The following variable
 * shorthand is used in the diagrams:
 * 
 * iU = iscsiCfgData->iscsiTargetCfgData[__index__].inUse
 * def = iscsiCfgData->iscsiTargetCfgData[__index__].defaultCfgEntry
 * del = iscsiCfgData->iscsiTargetCfgData[__index__].deletedDefaultCfgEntry
 * inc = __incl_deleted_default_entries__
 *
 * The following truth table shows the desired logic:
 *
 *  iU | def | del | inc | output
 *-----+-----+-----+-----+-------
 *  F  |  -  |  -  |  -  |  F    table entry is empty, nothing else matters --> NOT active
 *  T  |  F  |  -  |  -  |  T    entry in use, not a default config entry --> active
 *  T  |  T  |  F  |  -  |  T    entry in use, is from default config, not deleted --> active
 *  T  |  T  |  T  |  F  |  F    entry in use, is from default config, deleted and user doesn't want deleted entries --> NOT active
 *  T  |  T  |  T  |  F  |  T    entry in use, is from default config, deleted and user DOES want deleted entries --> active
 *
 * Transfer the above to a Karnaugh map for factoring:
 *
 *       \
 *        \  del,inc
 *         \  
 * iU,def   \  00   01   11   10
 *           +----+----+----+----+
 *       00  | 0  | 0  | 0  | 0  |
 *           +----+----+----+----+
 *       01  | 0  | 0  | 0  | 0  |
 *           +----+----+----+----+
 *       11  | 1  | 1  | 1  | 0  |
 *           +----+----+----+----+
 *       10  | 1  | 1  | 1  | 1  |
 *           +----+----+----+----+
 *
 *  Grouping the lower left quadrant, bottom row and bottom half of column 3, we get:
 *  
 *   output = iU*del' + iU*inc + iU*def' = iU * (del' + inc + def')
 *
 */
#define ISCSI_CFG_TARGET_ENTRY_ACTIVE(__index__, __incl_deleted_default_entries__) \
  (iscsiCfgData->iscsiTargetCfgData[__index__].inUse && \
  (!iscsiCfgData->iscsiTargetCfgData[__index__].deletedDefaultCfgEntry || \
   !iscsiCfgData->iscsiTargetCfgData[__index__].defaultCfgEntry || \
   __incl_deleted_default_entries__))

typedef struct
{
  L7_BOOL            inUse;
  L7_uint32          portNumber;
  L7_uint32          ipAddress;
  L7_uchar8          targetName[ISCSI_NAME_LENGTH_MAX+1];  /* save room for terminating '\0' */
  L7_BOOL            defaultCfgEntry;                      /* used to determine whether entry appears in text based config output */
  L7_BOOL            deletedDefaultCfgEntry;               /* flags an entry that has been deleted but is part of default entries */
} iscsiTargetCfgData_t;

#define L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT  (2*L7_ISCSI_MAX_TARGET_TCP_PORTS)
typedef struct
{
  L7_fileHdr_t       cfgHdr;
  L7_uint32          adminMode;                             /* global enable mode */
  L7_uint32          sessionTimeOutInterval;                /* seconds to wait before timing out session */
  L7_QOS_COS_MAP_INTF_MODE_t  tagFieldSelector;             /* select which packet priority mapping is used */
  L7_uint32          vlanPriorityValue;                     /* used to determine queue assignment and to mark packet with when VLAN tag (re)marking */
  L7_uint32          dscpValue;                             /* used to determine queue assignment and to mark packet with when DSCP (re)marking */
  L7_uint32          precValue;                             /* used to determine queue assignment and to mark packet with when precedence (re)marking */
  L7_BOOL            markingEnabled;                        /* if L7_TRUE, (re)marking packet enabled */
  iscsiTargetCfgData_t iscsiTargetCfgData[L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT];  /* need to have room for up to full table of deleted default entries */
  L7_uint32          numActiveTargetEntries;                /* count of target table entries that are active (not deleted) */
  L7_uint32          checkSum;                              /* check sum of config file NOTE: needs to be last entry */
} iscsiCfgData_t;

#define L7_ISCSI_CFG_DATA_SIZE  sizeof(iscsiCfgData_t)

/*********************************************************************
* @purpose  Saves ISCSI file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiSave(void);

/*********************************************************************
* @purpose  Restores ISCSI user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiRestore(void);

/*********************************************************************
* @purpose  Checks if ISCSI user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL iscsiHasDataChanged(void);
void iscsiResetDataChanged(void);

/*********************************************************************
* @purpose  Apply ISCSI Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiApplyConfigData(void);

L7_uint32 iscsiTxtCfgApplyCompletionCallback(L7_uint32 event);

/*********************************************************************
* @purpose  Build default ISCSI config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes   
*
* @end
*********************************************************************/
void iscsiBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Start ISCSI TASk
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
L7_RC_t iscsiStartTask();


L7_RC_t iscsiSemaCreate(void);
void iscsiSemaDelete(void);
void iscsiSemaTake();
void iscsiSemaGive();

L7_RC_t iscsiAdminModeApply(L7_uint32 mode);
L7_RC_t iscsiTimeOutIntervalRangeCheck(L7_uint32 seconds);
L7_RC_t iscsiTimeOutIntervalApply(L7_uint32 seconds);
L7_RC_t iscsiMarkingModeApply(L7_uint32 mode);
L7_RC_t iscsiVlanPriorityRangeCheck(L7_uint32 prioity);
L7_RC_t iscsiVlanPriorityApply(L7_uint32 priority);
L7_RC_t iscsiDscpRangeCheck(L7_uint32 dscp);
L7_RC_t iscsiDscpApply(L7_uint32 dscp);
L7_RC_t iscsiIpPrecRangeCheck(L7_uint32 prec);
L7_RC_t iscsiIpPrecApply(L7_uint32 prec);
L7_RC_t iscsiTagFieldRangeCheck(L7_uint32 selector);
L7_RC_t iscsiTagFieldApply(L7_uint32 selector);
L7_RC_t iscsiImpTargetTcpPortFreeEntryGet(L7_uint32 *entryId);
L7_RC_t iscsiImpTargetTcpPortAdd(L7_uint32 tcpPort, L7_uint32 ipAddress, L7_uint32 cosQueue, 
                                 L7_BOOL taggingEnabled, L7_QOS_COS_MAP_INTF_MODE_t tagMode, L7_uint32 tagData);
L7_RC_t iscsiImpTargetTcpPortDelete(L7_uint32 entryId);
L7_RC_t iscsiImpTargetTableLookUp(L7_uint32 port, L7_uint32 ip_addr, L7_uint32 *index);

L7_RC_t iscsiImpSessionTableIdIsValid(L7_uint32 sessionId);
L7_RC_t iscsiImpSessionFirstGet(L7_uint32 *sessionId);
L7_RC_t iscsiImpSessionNextGet(L7_uint32 sessionId, L7_uint32 *nextSessionId);
L7_RC_t iscsiImpSessionNextByEntryIdGet(L7_uint32 sessionId, L7_uint32 *nextSessionId);
L7_RC_t iscsiImpSessionTargetNameGet(L7_uint32 sessionId, L7_uchar8 *name);
L7_RC_t iscsiImpSessionInitiatorNameGet(L7_uint32 sessionId, L7_uchar8 *name);
L7_RC_t iscsiImpSessionStartTimeGet(L7_uint32 sessionId, L7_clocktime *time);
L7_RC_t iscsiImpSessionSilentTimeGet(L7_uint32 sessionId, L7_uint32 *seconds);
L7_RC_t iscsiImpSessionIsidGet(L7_uint32 sessionId, L7_uchar8 *isid);

L7_RC_t iscsiImpConnectionTableIdIsValid(L7_uint32 connectionId);
L7_RC_t iscsiImpConnectionFirstGet(L7_uint32 sessionId, L7_uint32 *connectionId);
L7_RC_t iscsiImpConnectionNextGet(L7_uint32 sessionId, L7_uint32 connectionId, L7_uint32 *nextConnectionId);
L7_RC_t iscsiImpConnectionNextByEntryIdGet(L7_uint32 connectionId, L7_uint32 *nextConnectionId);
L7_RC_t iscsiImpConnectionTargetIpAddressGet(L7_uint32 connectionId, L7_uint32 *ipAddr);
L7_RC_t iscsiImpConnectionInitiatorIpAddressGet(L7_uint32 connectionId, L7_uint32 *ipAddr);
L7_RC_t iscsiImpConnectionTargetTcpPortGet(L7_uint32 connectionId, L7_uint32 *port);
L7_RC_t iscsiImpConnectionInitiatorTcpPortGet(L7_uint32 connectionId, L7_uint32 *port);
L7_RC_t iscsiImpConnectionCidGet(L7_uint32 connectionId, L7_uint32 *cid);
L7_RC_t iscsiImpConnectionSessionIdGet(L7_uint32 connectionId, L7_uint32 *sessionId);

L7_RC_t iscsiImpCosDataGet(L7_uint32 *cosQueue, L7_QOS_COS_MAP_INTF_MODE_t *tagMode, L7_uint32 *tagData);

L7_RC_t iscsiTimerAdd(void);
void iscsiTimerDelete(void);
void iscsiSessionDelete(L7_uint32 sessionId);
void iscsiSessionDataClear(void);

L7_int32 iscsiConnectionLookup(L7_uint32 targetIpAddress,
                               L7_uint32 targetTcpPort,
                               L7_uint32 initiatorIpAddress,
                               L7_uint32 initiatorTcpPort);
L7_RC_t iscsiConnectionCreate(L7_uchar8 *targetName,
                              L7_uchar8 *initiatorName,
                              L7_uchar8 *isid,
                              L7_uint32 targetIpAddress,
                              L7_uint32 targetTcpPort,
                              L7_uint32 initiatorIpAddress,
                              L7_uint32 initiatorTcpPort,
                              L7_uint32 cid,
                              L7_uint32 targetTcpPortTableId);
void iscsiConnectionDelete(L7_uint32 connectionId);

L7_RC_t iscsiSessionDataBaseCreate(void);
void iscsiSessionDataBaseFree(void);

SYSNET_PDU_RC_t iscsiPktIntercept(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc);
#endif


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename lldp.h
*
* @purpose 802.1AB definitions
*
* @component 802.1AB(LLDP)
*
* @comments none
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#ifndef LLDP_H
#define LLDP_H

#include "l7_common.h"
#include "l7_resources.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "nimapi.h"
#include "lldp_api.h"
#include "l7_cnfgr_api.h"

#define LLDP_QUEUE			"lldp_Queue"
#define LLDP_MSG_SIZE        sizeof(lldpMgmtMsg_t)

#define LLDP_CFG_FILENAME    "lldpCfgData.cfg"
#define LLDP_CFG_VER_1        0x1
#define LLDP_CFG_VER_2		  0x2
#define LLDP_CFG_VER_CURRENT  LLDP_CFG_VER_2

#define LLDP_IS_READY (((lldpCnfgrState == LLDP_PHASE_INIT_3) || \
					    (lldpCnfgrState == LLDP_PHASE_EXECUTE) || \
						(lldpCnfgrState == LLDP_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

#define LLDP_TIMER_INTERVAL         1000 /* milliseconds */

#define LLDP_TX_INTERVAL_MIN          5
#define LLDP_TX_INTERVAL_MAX          32768
#define LLDP_TX_HOLD_MULTIPLIER_MIN   2
#define LLDP_TX_HOLD_MULTIPLIER_MAX   10
#define LLDP_REINIT_DELAY_MIN         1
#define LLDP_REINIT_DELAY_MAX         10
#define LLDP_NOTIFY_INTERVAL_MIN      5
#define LLDP_NOTIFY_INTERVAL_MAX      3600
#define LLDP_TX_DELAY_MIN			  1
#define LLDP_TX_DELAY_MAX			  8192

#define LLDP_TX_TLV_PORT_DESC_BIT   0x01
#define LLDP_TX_TLV_SYS_NAME_BIT    0x02
#define LLDP_TX_TLV_SYS_DESC_BIT    0x04
#define LLDP_TX_TLV_SYS_CAP_BIT     0x08


typedef enum {
  LLDP_PHASE_INIT_0 = 0,
  LLDP_PHASE_INIT_1,
  LLDP_PHASE_INIT_2,
  LLDP_PHASE_WMU,
  LLDP_PHASE_INIT_3,
  LLDP_PHASE_EXECUTE,
  LLDP_PHASE_UNCONFIG_1,
  LLDP_PHASE_UNCONFIG_2,
} lldpCnfgrState_t;

typedef struct
{
  nimConfigID_t        cfgId;
  L7_uchar8            txEnabled;
  L7_uchar8            rxEnabled;
  L7_uchar8			   medEnabled; /* If this is enabled then txEnabled and rxEnabled are forced to be enabled */
  L7_uchar8            notificationEnabled;
  L7_uchar8            optionalTLVsEnabled;
  L7_uchar8            mgmtAddrTxEnabled;
  lldpXMedCapabilities_t medTLVsSupported;
  lldpXMedCapabilities_t medTLVsEnabled;
  L7_uchar8				 medNotifyEnabled;
} lldpIntfCfgData_t;

typedef struct
{
  L7_fileHdr_t         cfgHdr;
  L7_uint32            txInterval;
  L7_uint32            txHoldMultiplier;
  L7_uint32 		   txDelay;
  L7_uint32            reinitDelay;
  L7_uint32            notifyInterval;
  L7_uint32 		   fastStartRepeatCount;
  L7_uint32			   localDeviceClass;
  lldpIntfCfgData_t    intfCfgData[L7_LLDP_INTF_MAX_COUNT];
  L7_uint32            checkSum;
} lldpCfgData_t;

typedef struct
{
  L7_uint32            txFramesTotal;
  L7_uint32            rxFramesTotal;
  L7_uint32            rxFramesDiscarded;
  L7_uint32            rxFramesErrors;
  L7_uint32            rxTLVsDiscarded;
  L7_uint32            rxTLVsUnrecognized;
  L7_uint32            rxAgeouts;
  L7_uint32 		   rxTLVsMED;
  L7_uint32			   rxTLVs8023;
  L7_uint32			   rxTLVs8021;
} lldpIntfStats_t;

typedef struct
{
  L7_uint32            remTblLastChangeTime;  /* last insert, delete, or modify of record */
  L7_uint32            remTblInserts; /* # of complete remote records inserted */
  L7_uint32            remTblDeletes; /* # of complete remote records deleted */
  L7_uint32            remTblDrops;   /* # of times remote info not inserted due to lack of resources */
  L7_uint32            remTblAgeouts; /* # of complete remote records aged */
  lldpIntfStats_t      intfStats[L7_LLDP_INTF_MAX_COUNT];
} lldpStats_t;

/* allocate a pool for each of the following
   structures instead of allocating a fixed amount
   per port, these are all optional */

#define LLDP_MGMT_ADDR_BUF_DESC "LLDPManAddrBufs"  /* 16 char max */

typedef struct lldpMgmtAddrEntry_s
{
  L7_uchar8                   family;
  L7_uchar8                   address[LLDP_MGMT_ADDR_SIZE_MAX];
  L7_uchar8                   length;
  L7_uchar8                   ifSubtype;            /* lldpManAddrIfSubtype_t */
  L7_uint32                   ifId;
  L7_char8                    oid[LLDP_MGMT_ADDR_OID_SIZE_MAX];
  struct lldpMgmtAddrEntry_s *next;
} lldpMgmtAddrEntry_t;

#define LLDP_UNKNOWN_TLV_BUF_DESC "LLDPUnTLVBufs"   /* 16 char max */

typedef struct lldpUnknownTLVEntry_s
{
  L7_uint32                     type;
  L7_uchar8                     info[LLDP_UNKNOWN_TLV_INFO_SIZE_MAX];
  L7_ushort16                   length;
  struct lldpUnknownTLVEntry_s *next;
} lldpUnknownTLVEntry_t;

#define LLDP_ORG_DEF_INFO_BUF_DESC "LLDPOrgDefBufs"   /* 16 char max */

#define LLDP_ORG_DEF_INFO_OUI_SIZE_MAX (3+1)          /* size + NULL byte */

typedef struct lldpOrgDefInfoEntry_s
{
  L7_char8                      oui[LLDP_ORG_DEF_INFO_OUI_SIZE_MAX];
  L7_uchar8                     subtype;
  L7_uchar8                     info[LLDP_ORG_DEF_INFO_SIZE_MAX];
  L7_ushort16                   length;
  struct lldpOrgDefInfoEntry_s *next;
} lldpOrgDefInfoEntry_t;

/* This structure is a just used for the avl tree search for the lldpRemDataEntry_t*/
typedef struct lldpRemDataKey_s
{
  L7_uint32               intIntfNum;           /* Intenal Interface Number */
  L7_uchar8               chassisIdSubtype;     /* lldpChassisIdSubtype_t */
  L7_uchar8               chassisId[LLDP_MGMT_STRING_SIZE_MAX];
  L7_ushort16             chassisIdLength;
  L7_uchar8               portIdSubtype;        /* lldpPortIdSubtype_t */
  L7_uchar8               portId[LLDP_MGMT_STRING_SIZE_MAX];
  L7_ushort16             portIdLength;
}lldpRemDataKey_t;

typedef struct
{
  L7_uint32               intIntfNum;           /* Intenal Interface Number */
  L7_uchar8               chassisIdSubtype;     /* lldpChassisIdSubtype_t */
  L7_uchar8               chassisId[LLDP_MGMT_STRING_SIZE_MAX];
  L7_ushort16             chassisIdLength;
  L7_uchar8               portIdSubtype;        /* lldpPortIdSubtype_t */
  L7_uchar8               portId[LLDP_MGMT_STRING_SIZE_MAX];
  L7_ushort16             portIdLength;
  /* All the above elements form the index for this table */

  L7_char8                portDesc[LLDP_MGMT_STRING_SIZE_MAX];
  L7_char8                sysName[LLDP_MGMT_STRING_SIZE_MAX];
  L7_char8                sysDesc[LLDP_MGMT_STRING_SIZE_MAX];
  L7_ushort16		          sysCapSupported;
  L7_ushort16             sysCapEnabled;
  L7_uchar8				        medCapCurrent[2];
  L7_uchar8				        srcMac[L7_MAC_ADDR_LEN];
  L7_uint32               timestamp;   
  L7_int32                rxTTL; /* TTL in the received PDU, Timer based on this */
  L7_uint32               remIndex;

  void                    *remDBIndex;      /* pointer to the timestamp linked list */
  lldpMgmtAddrEntry_t    *mgmtAddrList;
  lldpUnknownTLVEntry_t  *unknownTLVList;
  lldpOrgDefInfoEntry_t  *orgDefInfoList;

  void                   *next;  /* This needs to be the last element in this structure */
} lldpRemDataEntry_t;

typedef struct
{
  L7_uint32 intIfNum;
  L7_uint32 txTTR;         /* Time To Refresh seconds for intf local data */
  L7_uint32 txDelayWhile;
  L7_uint32 txReinitDelay; /* Time to wait before restarting transmit */
  L7_uint32	medFastStart;
  L7_uchar8 active;
  L7_uchar8 rxFrame;       /* indicates if we registered to receive PDUs */
  L7_uchar8 medTransmitEnabled; /* This is set to enabled if this port has received an MED TLV */
  L7_uchar8 medEndpointDevice; /* This is set to enabled if this port has received an MED TLV */
  L7_uchar8 localDbChanged;
} lldpIntfOprData_t;

typedef struct
{
  L7_uint32        intIfNum;
  L7_uint32        event;
  NIM_CORRELATOR_t correlator;
} lldpIntfChangeParms_t;

typedef struct
{
  L7_uint32       intIfNum;
  L7_netBufHandle bufHandle;
} lldpPduReceiveParms_t;

/* LLDP Management Message IDs */
typedef enum
{
  lldpMsgCnfgr = 1,
  lldpMsgIntfChange,
  lldpMsgTimer,
  lldpMsgPduReceive,
  lldpMsgMedPolicyInfo
} lldpMgmtMessages_t;

typedef struct
{
  L7_uint32        msgId;    /* Of type lldpMgmtMessages_t */
  union {
    L7_CNFGR_CMD_DATA_t   CmdData;
    lldpIntfChangeParms_t lldpIntfChangeParms;
    lldpPduReceiveParms_t lldpPduReceiveParms;
	lldpXMedPolicyInfoParms_t lldpXMedPolicyInfoParms;
  } u;
} lldpMgmtMsg_t;

/*********************************************************************
*
* @purpose  System Initialization for 802.1AB component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpInit(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  System Init Undo for 802.1AB component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void lldpInitUndo();

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
L7_RC_t lldpCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason);

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
L7_RC_t lldpCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason);

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
L7_RC_t lldpCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function undoes lldpCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void lldpCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes lldpCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void lldpCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes lldpCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void lldpCnfgrFiniPhase3Process();

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
L7_RC_t lldpCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason);

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

L7_RC_t lldpCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
*
* @purpose  To parse the configurator commands send to lldpTask
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void lldpCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  Saves 802.1AB configuration file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpSave(void);

/*********************************************************************
* @purpose  Checks if 802.1AB configuration data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL lldpHasDataChanged(void);
void lldpResetDataChanged(void);
/*********************************************************************
* @purpose  Apply static 802.1AB Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpApplyConfigData(void);

/*********************************************************************
* @purpose  Build default 802.1AB interface config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void lldpBuildDefaultIntfConfigData(nimConfigID_t *configId,
                                    lldpIntfCfgData_t *pCfg);

/*********************************************************************
* @purpose  Build default 802.1AB configuration data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void lldpBuildDefaultConfigData(L7_uint32 version);


/*********************************************************************
* @purpose  Start 802.1AB task
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
L7_RC_t lldpTaskStart();

/*********************************************************************
*
* @purpose Task to handle all 802.1AB management messages
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
void lldpTask();

/*********************************************************************
*
* @functions lldpInfoShow
*
* @purpose   Displays all config and nonconfig info for 802.1AB
*
* @param     none
*
* @returns   L7_SUCCESS
*
* @comments  Used for debug
*
* @end
*
*********************************************************************/
L7_RC_t lldpInfoShow();


/* Include the prototyped of the lldp_rdb.c file */
L7_RC_t lldpRemoteDBInit(L7_uint32 nodeCount);
L7_RC_t lldpRemoteDBDeInit(void);
L7_RC_t lldpRemoteDBClear(void);
lldpRemDataEntry_t *lldpRemDataEntryGet(lldpRemDataKey_t *key);
lldpRemDataEntry_t *lldpRemDataEntryGetNext(lldpRemDataKey_t *key);
lldpRemDataEntry_t *lldpRemDataNeighborGetNext(L7_uint32 intIfNum, lldpRemDataEntry_t *prevEntry);
L7_BOOL lldpRemDataNeighborIsLast(L7_uint32 intIfNum);
L7_RC_t  lldpRemDataInterfaceDelete(L7_uint32 intIfNum,L7_BOOL flag);
void lldpRemEntryDelete(lldpRemDataEntry_t *entry);
lldpRemDataEntry_t *lldpRemEntryAdd(lldpRemDataEntry_t *entry);
lldpRemDataEntry_t *lldpRemDataEntryTimestampIndexGet(L7_uint32 timestamp,
                                                      L7_uint32 intIfNum,
                                                      L7_uint32 remIndex);
L7_RC_t lldpRemDataEntryTimestampIndexGetNext(L7_uint32 *timestamp,
                                              L7_uint32 *intIfNum,
                                              L7_uint32 *remIndex);

#endif /* LLDP_H */

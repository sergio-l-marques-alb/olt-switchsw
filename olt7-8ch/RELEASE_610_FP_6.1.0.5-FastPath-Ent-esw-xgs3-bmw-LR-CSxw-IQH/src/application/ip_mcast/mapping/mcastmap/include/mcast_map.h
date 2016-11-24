/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mcast_map.h
*
* @purpose    MCAST Mapping layer internal function prototypes
*
* @component  MCAST Mapping Layer
*
* @comments   none
*
* @create     01/25/2006
*
* @author     gkiran
*
* @end
**********************************************************************/

#ifndef _MCAST_MAP_H_
#define _MCAST_MAP_H_


#include "nimapi.h"
#include "l7_cnfgr_api.h"
#include "mfc_api.h" 
#include "dot1q_api.h"

typedef enum
{
  MCASTMAP_CNFGR_MSG = 101,
  MCASTMAP_INTF_CHANGE_MSG,
  MCASTMAP_ROUTING_EVENT_MSG,
  MCASTMAP_ROUTING6_EVENT_MSG,
  MCASTMAP_VLAN_EVENT_MSG,
  MCASTMAP_CONFIG_SET_MSG,
  MCASTMAP_PROTOCOL_EVENT_MSG,
  MCASTMAP_MRP_EVENT_MSG,
} mcastMapMessages_t;

typedef enum
{
  MCASTMAP_ADMINMODE_SET_EVENT,
  MCASTMAP_THRESHOLD_SET_EVENT,
  MCASTMAP_STATIC_ROUTE_SET_EVENT,
  MCASTMAP_ADMIN_SCOPE_BOUNDARY_SET_EVENT,  
} mcastMapConfigSetEvents_t;

/*--------------------------------------*/
/*  mcast Map Task Messaging             */
/*--------------------------------------*/

/*-------------------------------------------------------------*/
/*  Queue-1 : Apptimer                                         */
/*-------------------------------------------------------------*/
#define MCASTMAP_APP_TMR_QUEUE       "mcastMapAppTmrMsgQueue" 
#define MCASTMAP_APP_TMR_MSG_COUNT   MCAST_APPTIMER_QUEUE_MSG_COUNT  
#define MCASTMAP_APP_TMR_MSG_SIZE    sizeof(mcastMapAppTmrMsg_t)

/*-------------------------------------------------------------*/
/*  Queue-2 : All mapping Events +  MFC events                 */
/*-------------------------------------------------------------*/
#define MCASTMAP_MSG_QUEUE      "mcastMapMsgQueue" 
#define MCASTMAP_MSG_COUNT  (L7_RTR_MAX_RTR_INTERFACES +  L7_MULTICAST_FIB_MAX_ENTRIES) 
#define MCASTMAP_MSG_SIZE   sizeof(mcastMapMsg_t)

/*-------------------------------------------------------------*/
/*  Queue-3 : All ctrl pkt events                              */
/*-------------------------------------------------------------*/
#define MCASTMAP_CTRL_PKT_QUEUE       "mcastMapPktMsgQueue" 
#define MCASTMAP_CTRL_PKT_MSG_COUNT   (L7_MAX_NETWORK_BUFF_PER_BOX + L7_MULTICAST_FIB_MAX_ENTRIES)
#define MCASTMAP_CTRL_PKT_MSG_SIZE    sizeof(mcastMapCtrlPktMsg_t)

#define MCAST_PKT_BUF_COUNT    (L7_MAX_NETWORK_BUFF_PER_BOX + L7_MULTICAST_FIB_MAX_ENTRIES)

/* The following Timer Nodes are for MRPs and MGMD */
#define MCAST_MAX_TIMERS   (L7_MULTICAST_FIB_MAX_ENTRIES * (MCAST_MAX_INTERFACES * 4))

/* Common Heap Pool Size Requirements for all the MCAST Routing Protocols viz.,
 * PIM-SM, PIM-DM, DVMRP, MGMD-Proxy.
 */
#define MCAST_V4_HEAP_SIZE     L7_MULTICAST_V4_HEAP_SIZE
#define MCAST_V6_HEAP_SIZE     L7_MULTICAST_V6_HEAP_SIZE

typedef struct mcastEventMsg_s
{
  union
  {
    mfcEntry_t            mfcEntry;     /* MFC Event Info */
    mfcInterfaceChng_t    intfChange;   /* Interface Change Event Info */
    L7_uint32             ipv6PktBuffer;
  } u;
} mcastEventMsg_t;

typedef struct mcastMapIntfChangeParms_s 
{
  L7_uint32        event;
  L7_uint32        intIfNum;
  NIM_CORRELATOR_t correlator;
} mcastMapIntfChangeParms_t;

typedef struct mcastMapRoutingEventParms_s 
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  void                      *pData;
  L7_BOOL                    asyncResponseRequired;
  ASYNC_EVENT_NOTIFY_INFO_t  eventInfo;
} mcastMapRoutingEventParms_t;

typedef struct mcastMapVlanEventParms_s 
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  dot1qNotifyData_t          vlanData;
} mcastMapVlanEventParms_t;

typedef struct mcastMapTtlEventParms_s
{
  L7_uint32 intIfNum;
  L7_uint32 ttl;
}mcastMapTtlEventParms_t;

typedef struct mcastMapAdminScopeEventParms_s
{
  L7_uint32 intIfNum;
  L7_inet_addr_t grpAddr;
  L7_inet_addr_t grpMask;
  L7_BOOL   mode;
}mcastMapAdminScopeEventParms_t;

typedef struct mcastMapConfigSetEventParms_s
{
  mcastMapConfigSetEvents_t configEvent;
  union
  {
    L7_uint32                adminMode;
    mcastMapTtlEventParms_t  ttlEventParms;
    mcastMapAdminScopeEventParms_t adminScopeEventParms;
  }u; 
}mcastMapConfigSetEventParms_t;

typedef struct mcastMapProtocolEventParms_s
{
  L7_uint32                  event;
  mcastEventMsg_t            msgData;
} mcastMapProtocolEventParms_t;

typedef struct mcastMapAppTmrParms_s
{
  L7_uint32                  event;
} mcastMapAppTmrParms_t;

typedef struct mcastMapMRPEventParms_s
{
  L7_uint32 event;
  L7_uint32 rtrIfNum;

}mcastMapMRPEventParms_t;
typedef struct
{
  L7_uint32   msgId;    
  union 
  {
    L7_CNFGR_CMD_DATA_t         cnfgrCmdData;
    mcastMapIntfChangeParms_t   mcastMapIntfChangeParms;
    mcastMapRoutingEventParms_t mcastMapRoutingEventParms;
    mcastMapVlanEventParms_t    mcastMapVlanEventParms;
    mcastMapConfigSetEventParms_t mcastMapConfigSetEventParms;
    mcastMapMRPEventParms_t     mcastMapMRPEventParms;
  } u;
} mcastMapMsg_t;

typedef struct
{
  L7_uint32   msgId;
  mcastMapProtocolEventParms_t mcastMapProtocolEventParms;
} mcastMapCtrlPktMsg_t;

typedef struct
{
  L7_uint32   msgId;
  mcastMapAppTmrParms_t mcastMapAppTmrParms;
} mcastMapAppTmrMsg_t;


/********************************************************************
*                     Function Prototypes
*********************************************************************/

extern void mcastMapTask();

extern void pktRcvrTask();

extern L7_RC_t mcastMapSave(void);

extern L7_RC_t mcastMapRestore(void);

extern L7_BOOL mcastMapHasDataChanged(void);

extern void mcastMapResetDataChanged(void);

extern L7_RC_t mcastMapIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
                                          NIM_CORRELATOR_t correlator);

extern void mcastMapBuildDefaultConfigData(L7_uint32 ver);

extern L7_RC_t mcastMapApplyConfigData(void);


extern L7_RC_t mcastMapRoutingChangeCallback(L7_uint32 intIfNum, 
                                      L7_uint32 event,
                                      void *pData, 
                                      ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
extern L7_RC_t mcastMapRouting6ChangeCallback(L7_uint32 intIfNum, 
                                      L7_uint32 event,
                                      void *pData, 
                                      ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

/* It looks like this Callback registration is removed with the fast failover 
 * changes.
 * Enable this if we again get back to the VLAN change registration.
 */
#if 0
extern L7_RC_t mcastMapVlanChangeCallback(dot1qNotifyData_t vlanData, L7_uint32 intIfNum, L7_uint32 event);
#endif

extern L7_RC_t mcastMapConfigMsgQueue(mcastMapConfigSetEvents_t eventType,
                                      L7_VOIDPTR pMsg,
                                      L7_uint32 msgLen);
extern L7_RC_t mcastMapMRPEventMsgSend(mcastMapConfigSetEvents_t eventType,
                               L7_VOIDPTR pMsg,
                               L7_uint32 msgLen);

extern L7_RC_t mcastMapProtocolMsgQueue(L7_uint32 eventType, L7_VOIDPTR pMsg,
                                 L7_uint32 msgLen);
extern L7_RC_t mcastMapIntfConfigApply(L7_uint32 intIfNum, L7_uint32 mode);



/**********************DEBUG TRACE ROUTINE PROTOTYPES ***************/

/*********************************************************************
 * @purpose  Saves mcast configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    mcastDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t mcastDebugSave(void);
/*********************************************************************
 * @purpose  Restores mcast debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    mcastDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t mcastDebugRestore(void);
/*********************************************************************
 * @purpose  Checks if mcast debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL mcastDebugHasDataChanged(void);

/*********************************************************************
 * @purpose  Build default mcast config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void mcastDebugBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
 * @purpose  Apply mcast debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t mcastApplyDebugConfigData(void);

#endif /* _MCAST_MAP_H_ */

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename broad_l3_mcast.h
*
* @purpose This file includes all the messages for L3 IP Multicast and prototypes
*
* @component hapi
*
* @comments none
*
* @create
*
* @author
* @end
*
**********************************************************************/
#ifndef INCLUDE_MD_L3_MCAST_H
#define INCLUDE_MD_L3_MCAST_H

#include "dapi.h"
#include "l7_product.h"
/* Number of messages on the async command queue. 
*/
#define HAPI_BROAD_MCAST_ASYNC_CMD_QUEUE_SIZE (L7_MULTICAST_FIB_MAX_ENTRIES + 16)

/* Maximum number of pending RPF failure notifications.
*/
#define HAPI_BROAD_MCAST_ASYNC_RPF_QUEUE_SIZE (L7_MULTICAST_FIB_MAX_ENTRIES)

typedef struct
{
  DAPI_USP_t usp;
  L7_inet_addr_t  srcAddr;
  L7_inet_addr_t  groupIp;
  L7_uint32  vlanID;
} hapi_broad_mcast_rpf_async_t;


/* Command format for multicast route add/delete.
*/
typedef enum 
{
  HAPI_BROAD_MCAST_ASYNC_ADD = 1,    /* Add an entry */
  HAPI_BROAD_MCAST_ASYNC_DELETE = 2, /* Delete an entry */
  HAPI_BROAD_MCAST_ASYNC_SYNC = 3,   /* Release a specified semaphore */
  HAPI_BROAD_MCAST_ASYNC_SNOOP_NOTIFY = 4,  /* L2 notification to L3 MC */
  HAPI_BROAD_MCAST_ASYNC_ADD_FAILURE_NOTIFY = 5,  /* Async notification for failed entry */
} hapi_broad_mcast_async_cmd_t;

typedef struct 
{
  hapi_broad_mcast_async_cmd_t  async_cmd;
  DAPI_USP_t              usp;
  union 
  {

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uchar8                   mcastMacAddr[L7_MAC_ADDR_LEN];
      L7_inet_addr_t              mcastGroupAddr;
      L7_inet_addr_t              srcIpAddr;
      L7_BOOL                     matchSrcAddr;
      L7_BOOL                     rpfCheckEnable;
      DAPI_MCAST_RPF_CHECK_METHOD_t  rpfType;
      DAPI_MCAST_RPF_CHECK_FAIL_ACTION_t rpfCheckFailAction;
      DAPI_USP_t                  outUspList [L7_MAX_INTERFACE_COUNT+1];
      L7_ushort16                 outUspCount;
      L7_BOOL                     outGoingIntfPresent;
      DAPI_USP_t                  ingressPhysicalPort;
     } mcastAdd;

    struct
    {
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uchar8                   mcastMacAddr[L7_MAC_ADDR_LEN];
      L7_inet_addr_t              mcastGroupAddr;
      L7_inet_addr_t              srcIpAddr;
      L7_BOOL                     matchSrcAddr;
      L7_BOOL                     rpfCheckEnable;
      DAPI_MCAST_RPF_CHECK_METHOD_t  rpfType;
      DAPI_MCAST_RPF_CHECK_FAIL_ACTION_t rpfCheckFailAction;
    } mcastDelete;

    struct
    {
      DAPI_CMD_GET_SET_t                 getOrSet;
      DAPI_SNOOP_L3_NOTIFY_DIRECTION_t   outIntfDirection;
      L7_uchar8                          mcastMacAddr[L7_MAC_ADDR_LEN];
      DAPI_USP_t                         outUspList [L7_MAX_INTERFACE_COUNT+1];
      L7_ushort16                        outUspCount;
      L7_BOOL                            snoopVlanOperState;
     } mcastModify;

     struct
     {
       L7_inet_addr_t              mcastGroupAddr;
       L7_inet_addr_t              srcIpAddr;
       L7_ushort16                 vlanId;
       L7_int32                    ipmcIndex;
     } mcastAddFailureNotify;

  } asyncData;
} hapi_broad_mcast_async_msg_t;


/*********************************************************************
*
* @purpose Enter L3 Multicast critical section.
*
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*
*********************************************************************/
void hapiBroadL3McastCritEnter (void);

/*********************************************************************
*
* @purpose Exit L3 Multicast critical section.
*
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*
*********************************************************************/
void hapiBroadL3McastCritExit (void);

/*********************************************************************
*
* @purpose Init software for IP Mcast package
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Init IP Mcast pkg for all ports on this card
*
* @param   L7_ushort16  unitNum - unit number for this card
* @param   L7_ushort16  slotNum - slot number for this card
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastCardInit(L7_ushort16 unitNum,L7_ushort16 slotNum,DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Hook in L3 functionality and initialize any data
*
* @param   DAPI_PORT_t *dapiPortPtr - generic port instance
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose Update stats related to the IP Mcast package
*
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastStatsUpdate(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastFwdConfig
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastFwdConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_ADD
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastAdd
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_DELETE
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastDelete
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastCountQuery
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastForwEntryCtrGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_USE_GET
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastUseQuery
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastForwEntryUseGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastPortAdd
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastPortAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastPortDelete
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastPortDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastVlanPortMemberUpdate
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastVlanPortMemberUpdate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.ttlMcastVal
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastTtlSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_GROUPS_GET
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastGroupsGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastFwdConfig
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingMcastFwdConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG
* @param   void       *data   - DAPI_ROUTING_INTF_MGMT_CMD_t.cmdData.mcastFwdConfig
* @param   DAPI_t     *dapi_g - the driver object    
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingMcastIgmpConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
* @purpose Create a ingress filter to prioratize PIM and DVMRP packets
*
* @param    @b{(input)} enableFilter
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadConfigL3McastFilter(L7_BOOL enableFilter);

/*********************************************************************
*
* @purpose Notify L3 IPMC of updated out port list.
*
* @param   DAPI_USP_t *usp    - unit slot port
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY
* @param   void       *data   - New outport list information
* @param   DAPI_t     *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingSnoopNotify(DAPI_USP_t *usp, DAPI_CMD_t cmd,
                                    void *data, DAPI_t *dapi_g);
/*********************************************************************
*
* @purpose Fine tune IPMC out port list according to the port list
*          maintained by snooping component
*
*
* @param   L7_uchar8 *mcastMacAddr  (Multicast Group MAC Address)
* @param   DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd  (Multicast Group MAC Address)
* @param   DAPI_USP_t *outRtrPortUsp  (Destination Routing interface USP)
* @param   DAPI_t *dapi_g  (DAPI context)
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRoutingIntfMcastSnoopAsyncNotifyProcess(L7_uchar8 *mcastMacAddr,
                                                        DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd,  
                                                        DAPI_USP_t *outRtrPortUsp, 
                                                        DAPI_t *dapi_g);
/*********************************************************************
* @purpose Create a ingress filter to prioratize PIM and DVMRP packets
*
* @param    @b{(input)} enableFilter
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadConfigL3V6McastFilter(L7_BOOL enableFilter);
#endif

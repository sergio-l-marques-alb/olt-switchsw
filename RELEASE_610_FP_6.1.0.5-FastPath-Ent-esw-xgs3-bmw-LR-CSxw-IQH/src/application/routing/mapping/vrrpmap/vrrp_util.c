/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: vrrp_util.c
*
* Purpose: VRRP Utility Functions
*
* Component: Virtual Router redundancy protocol(VRRP) Mapping Layer
*
* Created by: TCS 12/12/2001
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#define L7_MAC_ENET_VRRP                          /* vrrp mac addr      */
#define L7_MAC_IP_VRRP_ADVT            /* VRRP advertisement dest MAC */

/* System include file */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "l7_common.h"
#include "l7_packet.h"

#include "osapi.h"
#include "nimapi.h"

#include "l3_commdefs.h"
#include "l3_comm_structs.h"
#include "default_cnfgr.h"
#include "l3_default_cnfgr.h"

#include "l7_vrrp_api.h"
#include "vrrp_config.h"

#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"

#include "l7_vrrpinclude.h"

#include "l3_defaultconfig.h"
#include "usmdb_mib_vrrp_api.h"
#include "fdb_api.h"


/*global variable*/
extern vrrpList_t* paramHead_g;
extern L7_short16 totalRouters_g;
extern vrrpList_t* paramCurrent_g;
extern vrrpRouterBasicConfiguration_t  vrrpBasicConfig_g;
extern vrrpList_t * listMemPool_g[L7_VRRP_INTF_MAX_COUNT + 1];
extern vrrpRouterInfo_t * routersArray_g[L7_VRRP_INTF_MAX_COUNT + 1];
extern vrrpIntfInformation_t * vrrpIntfInfoTbl;
extern L7_vrrpMapCfg_t     *pVrrpMapCfgData; 
extern void      *VrrpConfigSema;
extern L7_uint32 vrrpQueueIntfChngOverflowCtr;
extern L7_uint32 vrrpQueueRtEventChngOverflowCtr;

extern vrrpCnfgrState_t vrrpCnfgrState;
extern L7_uint32         *vrrpIntfMapTbl;
extern L7_uint32 vrrpBufferPoolId;

static void vrrpMapBestRouteEventPost(void);
/* 1. Arrays listMemPool_g and routersArray_g maintain VRRP information for a particular
   virtual router (identified by its VRId) on a given interface (identified by its intIfNum).
   vrrpVRIdToIndexMapTbl maintains the mapping from VRId to the array index for
   the arrays listMemPool_g and routersArray_g.
   2. The array vrrpVRIdToIndexMapTbl is indexed in the first dimension by the intIfNum. 
   intIfNum is assumed to vary from 1 to L7_VRRP_INTF_MAX_COUNT.   
   Consequently, vrrpVRIdToIndexMapTbl[0][3] corresponds to the entry for a virtual 
   router whose VRId is 4 and resides on the interface whose intIfNum is 1. 
   3. An entry of 'x' in vrrpVRIdToIndexMapTbl implies the application should
   index into the arrays listMemPool_g and routersArray_g, with 'x-1'. An entry of 0
   signifies the VRId hasnt been mapped to an aarray index. */
L7_uchar8 * vrrpVRIdToIndexMapTbl[L7_IPMAP_INTF_MAX_COUNT];
vrrpMapInfo_t *pVrrpMapInfo = L7_NULL;
rtoRouteChange_t *vrrpMapRouteChangeBuf;


/*********************************************************************
* @purpose  to get the vmac corresponding to ip
*
* @param      ipAddress - ip address
* @param      tVMAC   - VMAC address
*                                                          
* @returns  e_Err
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t vrrpGetVMac(L7_uchar8 *ipAddress, L7_uchar8 *tVMAC)
{
  L7_uchar8 vmac[6];
  L7_uint32 i;
#if 0 /* smanders removed for stacking */
  L7_uint32 unit = simGetThisUnit();
#else
  L7_uint32 unit = 1;
#endif
  L7_uint32 ipAddr;
  L7_BOOL mode;

  if (L7_vrrpAdminModeGet(&mode)!= L7_SUCCESS)
    return L7_FAILURE;

  if (mode!=L7_TRUE)
    return L7_FAILURE;

  if (usmDBVrrpNtohl(unit,ipAddress, &ipAddr) != L7_SUCCESS)
    return L7_FAILURE;

  if (L7_vrrpVMacGet(ipAddr,vmac) == L7_SUCCESS)
  {
    for (i =0;i<6;i++)
    {
      tVMAC[i] = vmac[i];
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}           
/*********************************************************************
* @purpose  Function to add the vrrp param structure in the list
*
* @param    vrid   The virtual router id of the router to be configured
* @param    intIfNum   The interface number of the router to be configured
* @param    rtrIntfUp  L7_TRUE if routing interface is up
* @param    ipAddr     primary IP address on local interface
* @param    ipMask     network mask for primary IP address
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t  vrrplistAdd( L7_uchar8 vrid, L7_uint32 intIfNum, L7_BOOL rtrIntfUp,
                      L7_uint32 ipAddr, L7_uint32 ipMask)
{
  vrrpList_t* listTail;
  vrrpList_t* listPosition = L7_NULLPTR;
  vrrpList_t* temp = L7_NULLPTR;
  vrrpRouterInfo_t *  vrrpPkt = L7_NULLPTR;
  L7_short16 intfInfoTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);
  L7_uint32 vridTemp = vrid;
  L7_uint32 ifType;
  L7_uchar8 arrayIdxForVRId;
  L7_RC_t   rc;
  L7_uint32 ipAddrCount;
  L7_uint32 trackIntfCount;
  L7_uint32 trackRoutCount;
 

  if (intfInfoTblIndex == -1)
  {
    /* Invalid Interface Number */
    return L7_FAILURE;
  }

  /* Don't allow more than L7_VRRP_MAX_VIRT_ROUTERS in the system */
  if (totalRouters_g == L7_VRRP_MAX_VIRT_ROUTERS)
  {
    /* Maximum capacity reached */
    return L7_FAILURE;
  }

  if ( intfInfoTblIndex < 0 || vridTemp < L7_ROUTING_VRRP_MIN_VRID || vridTemp > L7_ROUTING_VRRP_MAX_VRID)
  {
    /* Maximum capacity reach */
    return L7_FAILURE;
  }

  if (nimGetIfType(intIfNum, &ifType) != L7_SUCCESS)
    return L7_FAILURE;

  rc = vrrpVRIdToIndexMap(vrid,intIfNum);
  if(rc ==  L7_TABLE_IS_FULL)
      return L7_FAILURE;
  arrayIdxForVRId = vrrpVRIdToIndexGet(vrid, intIfNum);

  /* Find the list element from the linklist pool of memory*/
  temp = &(listMemPool_g[intfInfoTblIndex][arrayIdxForVRId-1]);

  /* Get the pointer to the configuration pkt memory from the 
     array of the configuration*/
  vrrpPkt = &(routersArray_g[intfInfoTblIndex][arrayIdxForVRId-1]);
  memset(vrrpPkt,0, sizeof(vrrpRouterInfo_t));

  if (nimConfigIdGet(intIfNum, &vrrpPkt->vrrpCfgInfo.configId) != L7_SUCCESS)
    return L7_FAILURE;

  /* Increment the configuration count for a particular interface*/
  (vrrpIntfInfoTbl[intfInfoTblIndex].totalConfigured)++;
  vrrpIntfInfoTbl[intfInfoTblIndex].intIfNum = intIfNum;

  /* Assign the default configuration values */
  vrrpPkt->vrrpCfgInfo.vrid = vrid;
  vrrpPkt->vrrpCfgInfo.preempt_mode = L7_TRUE;

  vrrpPkt->vrrpCfgInfo.auth_types = L7_AUTH_TYPE_NONE;
  bzero(vrrpPkt->vrrpCfgInfo.auth_data, VRRP_AUTH_DATA_LEN);
  vrrpPkt->vrrpCfgInfo.priority = FD_VRRP_DEFAULT_PRIORITY;  
  vrrpPkt->vrrpVirtRouterOperInfo.oper_priority = FD_VRRP_DEFAULT_PRIORITY;
  for (ipAddrCount = 0; ipAddrCount < L7_L3_NUM_IP_ADDRS; ipAddrCount++)
  {
    vrrpPkt->vrrpCfgInfo.ipaddress[ipAddrCount] = FD_VRRP_DEFAULT_IP_ADDRESS;
  }
  for (trackIntfCount = 0; trackIntfCount < L7_VRRP_MAX_TRACK_INTF_PER_VR; trackIntfCount++)
  {
    vrrpPkt->vrrpCfgInfo.vr_track_intf[trackIntfCount].intIfNum = 0;
    vrrpPkt->vrrpCfgInfo.vr_track_intf[trackIntfCount].prio_dec = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
    vrrpPkt->vrrpCfgInfo.vr_track_intf[trackIntfCount].stateUp = L7_FALSE;
  }

  for (trackRoutCount = 0; trackRoutCount < L7_VRRP_MAX_TRACK_RT_PER_VR; trackRoutCount++)
  {
    vrrpPkt->vrrpCfgInfo.vr_track_route[trackRoutCount].ipAddr = FD_VRRP_DEFAULT_IP_ADDRESS;
    vrrpPkt->vrrpCfgInfo.vr_track_route[trackRoutCount].subnetMask = FD_VRRP_DEFAULT_IP_ADDRESS;
    vrrpPkt->vrrpCfgInfo.vr_track_route[trackRoutCount].prio_dec =FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
    vrrpPkt->vrrpCfgInfo.vr_track_route[trackRoutCount].reachable = L7_FALSE;
  }
  vrrpPkt->vrrpCfgInfo.adver_int = FD_VRRP_DEFAULT_ADVERTISEMENT_INTERVAL;

  bzero((vrrpPkt->vrrpVirtRouterOperInfo.vmac),L7_MAC_ADDR_LEN);

  vrrpPkt->vrrpVirtRouterOperInfo.intIfNum = intIfNum;

  memcpy( vrrpPkt->vrrpVirtRouterOperInfo.vmac , &(L7_ENET_VRRP_MAC_ADDR) ,L7_MAC_ADDR_LEN - 1);
  vrrpPkt->vrrpVirtRouterOperInfo.vmac[5] = vrid;

  vrrpPkt->vrrpCfgInfo.vr_start_state = L7_VRRP_DOWN;

  vrrpPkt->vrrpVirtRouterOperInfo.ipAddressCount=0;
  vrrpPkt->vrrpVirtRouterOperInfo.vr_state = L7_VRRP_STATE_INIT;
  vrrpPkt->vrrpVirtRouterOperInfo.oper_rowstatus = L7_VRRP_ACTIVE;
  vrrpPkt->vrrpVirtRouterOperInfo.assoc_ip_rowstatus = L7_VRRP_ACTIVE;

  /* Assign the master down interval as described in RFC 2338*/
  vrrpPkt->vrrpVirtRouterOperInfo.master_down_int = 
  3*vrrpPkt->vrrpCfgInfo.adver_int + (255-vrrpPkt->vrrpVirtRouterOperInfo.oper_priority)/255 ;

  vrrpPkt->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds=0;
  vrrpPkt->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds=0;
  vrrpPkt->vrrpVirtRouterOperInfo.protocol_type = L7_VRRP_IP;
  vrrpPkt->vrrpVirtRouterOperInfo.rtrIntfUp = rtrIntfUp;
  vrrpPkt->vrrpVirtRouterOperInfo.localAddr.ipAddr = ipAddr;
  vrrpPkt->vrrpVirtRouterOperInfo.localAddr.ipMask = ipMask;

  temp->object = vrrpPkt;
  paramCurrent_g = temp;
  totalRouters_g++;

  /* Take the semaphore */
  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  if (paramHead_g == L7_NULLPTR)
  {
    paramHead_g = temp;
    paramHead_g->next = paramHead_g->previous = L7_NULLPTR;
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  else
  {
    while ( listTail != L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum<intIfNum )
    {
      listPosition = listTail;
      listTail = listTail->next;
    }

    while (listTail != L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum ==
           intIfNum && (listTail->object)->vrrpCfgInfo.vrid< vrid )
    {
      listPosition = listTail;
      listTail = listTail->next;
    }

    if ( listPosition == L7_NULLPTR)
    {
      /* Add the object in the begining */
      listTail->previous = temp;
      temp->previous = L7_NULLPTR;
      temp->next = listTail;
      paramHead_g = temp;
    }
    else
    {
      temp->next = listTail;      
      temp->previous = listPosition;
      listPosition->next = temp;
      /* if not the last element in the list */
      if ( listTail != L7_NULLPTR)
      {
        listTail->previous = temp;
      }
    }
  } /* endif */
  osapiSemaGive(VrrpConfigSema);
  /* Set data changed flag to true */
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to delete the vrrp param structure from the list 
*
* @param    vrid       The virtual router id.
* @param    intIfNum   Interface Nunmber
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t  vrrplistDelete( L7_uchar8 vrid, L7_uint32 intIfNum)
{
  vrrpList_t* listTail;
  L7_short16 intfInfoTblIndex=0;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  while ( listTail!= L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum<intIfNum )
  {
    listTail = listTail->next;
  }

  while ( listTail != L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum 
          && listTail->object->vrrpCfgInfo.vrid< vrid )
  {
    listTail = listTail->next;
  }

  if (listTail!=L7_NULLPTR && listTail->object->vrrpCfgInfo.vrid == vrid && 
      listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
  {

    if ( listTail->object->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
    {
      osapiSemaGive(VrrpConfigSema);  
      return L7_FAILURE;
    }
    if ( listTail->previous == L7_NULLPTR)
    {
      paramHead_g = listTail->next;    
    }
    else
    {
      listTail->previous->next = listTail->next;
    }
    if ( listTail->next!= L7_NULLPTR)
    {
      listTail->next->previous = listTail->previous;
    }

    listTail->object->vrrpVirtRouterOperInfo.intIfNum = L7_NULL;
    listTail->object->vrrpCfgInfo.vrid = L7_NULL;

    (void)vrrpVRIdToIndexUnmap(vrid,intIfNum);
    
    intfInfoTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);

    if (intfInfoTblIndex == -1)
    {
      osapiSemaGive(VrrpConfigSema);
      /* Invalid Interface Number */
      return L7_FAILURE;
    }


    if(vrrpIntfInfoTbl[intfInfoTblIndex].totalConfigured)
    {
      (vrrpIntfInfoTbl[intfInfoTblIndex].totalConfigured)--;
    }
    totalRouters_g--;
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  else
  {
    osapiSemaGive(VrrpConfigSema);  
    return L7_FAILURE;
  }
}


/*********************************************************************
* @purpose  To map vrrp configuration data from Linked List to 
*           L7_vrrpMapCfg_t structure
*
* @param    None
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void vrrpListMapping()
{
  vrrpList_t* listTail;
  L7_uint32 count = 0;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  while ( listTail!= L7_NULLPTR )
  {
    memcpy(&(pVrrpMapCfgData->virtRouter[count++]),
           &(listTail->object->vrrpCfgInfo), sizeof(vrrpVirtRouterCfg_t));
    listTail = listTail->next;               
  }
  osapiSemaGive(VrrpConfigSema);
  for ( ;count<L7_VRRP_MAX_VIRT_ROUTERS;count++)
  {
    memset(&(pVrrpMapCfgData->virtRouter[count]), 0, sizeof(vrrpVirtRouterCfg_t));  
  }

}

/*********************************************************************
* @purpose  Function to shutdown the virtual router.
*
* @param    vrid       The virtual router id.
* @param    intIfNum   Interface Number
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/

L7_RC_t  L7_vrrpShutdownProcess(L7_uchar8 vrid, L7_uint32 intIfNum )
{
  vrrpRouterInfo_t  *vrrpParam = L7_NULLPTR;

  if ( L7_NULLPTR == (vrrpParam =L7_vrrpParamsFetch(vrid,intIfNum)) )
  {
    return L7_ERROR;
  }
  if (vrrpParam->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
  {
    if (VRRPShutdownVirtualRouter(vrrpParam,intIfNum) != L7_SUCCESS)
    {
      osapiSemaGive(VrrpConfigSema);   
      return L7_ERROR;
    }
    L7_vrrpRouterStop(vrrpParam);


  }
  vrrpParam->vrrpCfgInfo.vr_start_state = L7_VRRP_DOWN;
  osapiSemaGive(VrrpConfigSema);   
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to get the row index for an interface
*
* @param    intIfNum   The interface number 
*
* @returns  row index
* @returns  -1 if Maximum capacity reach
*
* @notes    This function keeps the count of routers memory allocated
*           from the pool and also return the row number to be used 
*           for router configuration.
*       
* @end
*********************************************************************/
L7_short16 vrrpIntfInfoTblIndexGet(L7_uint32 intIfNum)
{
  L7_short16 i, index = -1;

  for (i= L7_VRRP_INTF_MAX_COUNT; i>=1; i--)
  {
    if (vrrpIntfInfoTbl[i].intIfNum == intIfNum)
    {
      index = i;
      i = 0;
    }
    else if (vrrpIntfInfoTbl[i].totalConfigured == 0)
    {
      index = i;
    }
  }
  return index;
}

/*********************************************************************
* @purpose  To Start the vrrp router
*
* @param    vmac       VMAC address of vrrp router
* @param    sourceIP   ip address
* @param    intIfNum   interface Number
* @param    vrid       vrId
* @param    vr_state   state of the router
* @param    priority   router priority
* @param    upTime     uptime of the router
*
* @returns  Pointer to the configuration data
*
* @notes    This function is for the vrrp configuration part only.
*           Function doesn't release semaphore if an object is found.
*       
* @end
*********************************************************************/
void  L7_vrrpRouterStart(vrrpRouterInfo_t *virtualRouter)
{
  L7_clocktime ct;
  L7_IP_ADDR_t ipAddr;
  L7_RC_t rc;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1]; 
  L7_uchar8 *role;
    

  osapiClockTimeRaw(&ct);
  virtualRouter->vrrpVirtRouterOperInfo.vrrp_uptime = ct;
  virtualRouter->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds=0;
  virtualRouter->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds=0;
  virtualRouter->vrrpVirtRouterOperInfo.vrrpSysUpTime = osapiUpTimeRaw();

  /* Ensure priority is set appropriately in case we're in here as a result
   * of an IP address change
   */
  ipAddr = virtualRouter->vrrpVirtRouterOperInfo.localAddr.ipAddr;
  if (ipAddr)
  {
    /* Check if primary vrrp ip address matched with interface Ip */
    if ((virtualRouter->vrrpCfgInfo.ipaddress[0] == ipAddr) &&
        (virtualRouter->vrrpVirtRouterOperInfo.oper_priority != 255))
    {
      virtualRouter->vrrpVirtRouterOperInfo.oper_priority = 255;
    }
    else if ((virtualRouter->vrrpCfgInfo.ipaddress[0] != ipAddr) &&
             (virtualRouter->vrrpVirtRouterOperInfo.oper_priority == 255))
    {
      virtualRouter->vrrpVirtRouterOperInfo.oper_priority = virtualRouter->vrrpCfgInfo.priority;
      (void)vrrpOperationalPriorityCalc(virtualRouter);
    }
  }

  if (virtualRouter->vrrpVirtRouterOperInfo.oper_priority == 255)
  {
    role = "Master";
    rc = VRRPTransitToMaster(virtualRouter,
                             virtualRouter->vrrpVirtRouterOperInfo.vmac,
                             virtualRouter->vrrpVirtRouterOperInfo.intIfNum);
  }
  else
  {
    role = "Backup";
    rc = VRRPTransitToBackup(virtualRouter,
                             virtualRouter->vrrpVirtRouterOperInfo.intIfNum);
  }

  nimGetIntfName(virtualRouter->vrrpVirtRouterOperInfo.intIfNum, 
                 L7_SYSNAME, ifName);
  if (rc == L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
            "VR %d on interface %s started as %s.",
            virtualRouter->vrrpCfgInfo.vrid, ifName, role); 
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
            "Failed to start VR %d on interface %s as %s",
            virtualRouter->vrrpCfgInfo.vrid, ifName, role); 
  }
}                               

/*********************************************************************
* @purpose  To Stop the vrrp router
*
* @param    vmac       VMAC address of vrrp router
* @param    sourceIP   ip address
* @param    intIfNum   interface Number
* @param    vrid       vrId
* @param    addrCount  total address
* @param    auth_type  authentication type
* @param    adver_int  adver interval
* @param    auth_data  auth data
* @param    vr_state   state of the router
* @param    upTime     uptime of the router
*
* @returns  Pointer to the configuration data
*
* @notes    This function is for the vrrp configuration part only.
*           Function doesn't release semaphore if an object is found.
*       
* @end
*********************************************************************/

void  L7_vrrpRouterStop (vrrpRouterInfo_t*  virtualRouter)
{
  if (virtualRouter->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_MASTER)
  {
    dtlIpv4VrrpVridDelete(virtualRouter->vrrpVirtRouterOperInfo.intIfNum,
                          virtualRouter->vrrpCfgInfo.vrid);

    L7_vrrpMgmtVmacAddrDel(virtualRouter->vrrpVirtRouterOperInfo.intIfNum,
                           virtualRouter->vrrpVirtRouterOperInfo.vmac);
  }

  virtualRouter->vrrpVirtRouterOperInfo.vr_state = L7_VRRP_STATE_INIT ;
  virtualRouter->vrrpVirtRouterOperInfo.vrrp_uptime.nanoseconds = 0;
  virtualRouter->vrrpVirtRouterOperInfo.vrrp_uptime.seconds = 0;

}

/*********************************************************************
* @purpose  To Stop all the routers
*
* @param    intIfNum   Interface Number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void L7_vrrpStopAll()
{
  vrrpList_t* listTail;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  /* check if current points to the object to be fetch */

  while (listTail!= L7_NULLPTR)
  {
    if (listTail->object->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
    {
      if (listTail->object->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_MASTER)
        VRRPShutdownVirtualRouter(listTail->object, listTail->object->vrrpVirtRouterOperInfo.intIfNum);
      L7_vrrpRouterStop(listTail->object);
    }
    listTail = listTail->next;
  }
  osapiSemaGive(VrrpConfigSema);
}

/*********************************************************************
* @purpose  To Start all the routers
*
* @param    intIfNum   Interface Number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void L7_vrrpStartAll()
{
  vrrpList_t* listTail;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  /* check if current points to the object to be fetch */

  while (listTail != L7_NULLPTR)
  {
    if ((listTail->object->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_INIT) &&
        (L7_vrrpCheckPrecondition(listTail->object) == L7_SUCCESS))
    {
      L7_vrrpRouterStart(listTail->object);
    }
    listTail = listTail->next;
  }
  osapiSemaGive(VrrpConfigSema);
}

/*********************************************************************
* @purpose  Function to determine if all conditions are met for a VR
*           to become active on an interface.
*
* @param    vrInfo - cfg and state data for a VR
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    The following conditions are required:
*	    1) VRRP is enabled globally
*     2) The virtual router is enabled on the interface
*     3) A VR IP address is configured
*	    4) Routing is enabled (implied if routing interface up)
*	    5) Routing is enabled on the port
*     6) an IP address is configured on the local interface
*
*            No need to check NIM state. Router interface can't be up 
*            unless NIM says interface is active.
*       
* @end
*********************************************************************/
L7_RC_t L7_vrrpCheckPrecondition(vrrpRouterInfo_t *vrInfo)
{
  L7_IP_ADDR_t ipAddr = vrInfo->vrrpVirtRouterOperInfo.localAddr.ipAddr;
  L7_BOOL rtrIntfUp = vrInfo->vrrpVirtRouterOperInfo.rtrIntfUp;
  L7_vrrpStartState_t startState = vrInfo->vrrpCfgInfo.vr_start_state;
  L7_uint32 vrIp = vrInfo->vrrpCfgInfo.ipaddress[0];
  
  if ((pVrrpMapCfgData->rtr.vrrpAdminMode == L7_TRUE) && 
      (startState == L7_VRRP_UP) &&
      vrIp && ipAddr && rtrIntfUp)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Put interface events on the queue to be handled by VRRPDaemonTask
*
* @param    intIfnum    port changing state
* @param    event       new state
* @param    correlator  correlator
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t vrrpMapIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  static L7_BOOL logMsgDisplayed = L7_FALSE;
  L7_RC_t rc;
  vrrpMsg_t msg;
  NIM_EVENT_COMPLETE_INFO_t status;

  if (vrrp_Queue  == L7_NULLPTR )
  {
    return L7_FAILURE;
  }
  /* if this is an event we are not interested in, perform early return*/
  if (event != L7_CREATE &&
      event != L7_DELETE)
  {
    status.intIfNum     = intIfNum;
    status.component    = L7_VRRP_MAP_COMPONENT_ID;
    status.response.rc  = L7_SUCCESS;
    status.event        = event;
    status.correlator   = correlator;
    status.response.rc = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  msg.msg_id = vrrpMsgIntfEvent;
  msg.type.intfEvent.intIfNum = intIfNum;
  msg.type.intfEvent.event = event;
  msg.type.intfEvent.correlator = correlator;
  
  rc = osapiMessageSend( vrrp_Queue, &msg, VRRP_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

  if (rc != L7_SUCCESS)
  {
    if (logMsgDisplayed == L7_FALSE)
    {
      logMsgDisplayed = L7_TRUE;
      LOG_MSG("%s: %d: vrrpMapIntfChangeCallback: osapiMessageSend failed\n", __FILE__, __LINE__);
    }
    vrrpQueueIntfChngOverflowCtr++;
  }

  return rc;
}

/*********************************************************************
* @purpose  Put routing events on the queue to be handled by VRRPDaemonTask
*
* @param    intIfnum    port changing state
* @param    event       new state
* @param    *pData      pointer to event specific data
* @param    *pResponse  for Asynchronous notification of routing event handling to 
*                       IP map
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t vrrpMapRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                          L7_uint32 event,
                                          void *pData, 
                                          ASYNC_EVENT_NOTIFY_INFO_t *response)
{
  static L7_BOOL logMsgDisplayed = L7_FALSE;
  L7_RC_t rc;
  vrrpMsg_t msg;

  if (vrrp_Queue == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset( (void *)&msg, 0, VRRP_MSG_SIZE );
  msg.msg_id                     = vrrpMsgRtrEvent;
  msg.type.rtrEvent.intIfNum    = intIfNum;
  msg.type.rtrEvent.event       = event;
  msg.type.rtrEvent.pData       = pData;    /* Warning: when/if event-specific data is used,
                                                 need to ensure that the information pointed to
                                                 will still be valid (i.e. is fairly persistent
                                                 until all tasks which need to use it can access
                                                 it, or that, for a given event, the information
                                                 is copied. */
    if (response != L7_NULL)
    {
        memcpy(&(msg.type.rtrEvent.response), response, sizeof(ASYNC_EVENT_NOTIFY_INFO_t) );
    }
    
    rc = osapiMessageSend( vrrp_Queue, &msg, VRRP_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM );
    
    if (rc != L7_SUCCESS)
    {
       if (logMsgDisplayed == L7_FALSE)
       {
        logMsgDisplayed = L7_TRUE;
        LOG_MSG("%s: %d: vrrpMapRoutingEventChangeCallBack: osapiMessageSend failed\n", 
                __FILE__, __LINE__);
       }
       vrrpQueueRtEventChngOverflowCtr++;

    }

  return rc;
}

/*********************************************************************
* @purpose  Process interface events
*
* @param    intIfNum    Interface Number
* @param    event -     Event Type
* @param    correlator  NIM correlator waiting for call back
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    No need to listen to LAG or probe events. If interface is 
*           acquired, IP MAP will take routing interface down.
*
*           Don't respond to active/inactive events. Instead, respond
*           to router events.
*
* @end
*********************************************************************/
L7_RC_t vrrpMapIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, 
                                 NIM_CORRELATOR_t correlator)
{
  NIM_EVENT_COMPLETE_INFO_t status;   
  L7_RC_t rc = L7_SUCCESS;

  status.intIfNum     = intIfNum;
  status.component    = L7_VRRP_MAP_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.event        = event;
  status.correlator   = correlator;

  /* Return if VRRP does not care about the interface */
  if (L7_vrrpIntfIsValid(intIfNum) != L7_TRUE)
  {
    nimEventStatusCallback(status);
    return rc;
  }

  if (VRRP_IS_READY == L7_TRUE)
  {
    switch (event)
    {
      case L7_CREATE:
        rc = vrrpIntfCreate(intIfNum); 
        break;

      case (L7_DELETE):
        rc = vrrpIntfDelete(intIfNum);
        break;

      default:
        /* not an event of local interest */
        break;
    } 
  }

  status.response.rc = rc;
  nimEventStatusCallback(status);

  return rc;
}


/*********************************************************************
* @purpose  Process routing events
*
* @param    intIfNum    Interface Number
* @param    event -     Event Type
* @param    *pData      pointer to event specific data
* @param    *pResponse  for Asynchronous notification of routing event handling to 
*                       IP map
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t vrrpMapRoutingEventChangeProcess(L7_uint32 intIfNum,
                                         L7_uint32 event,
                                         void *pData, 
                                         ASYNC_EVENT_NOTIFY_INFO_t *response)

{
  L7_IP_ADDR_t                  ipAddr;
  L7_IP_MASK_t                  mask;
  L7_BOOL                       must_respond;
  L7_BOOL                       event_completed;
  ASYNC_EVENT_COMPLETE_INFO_t   event_completion;

  /*------------------*/
  /* Initialize data  */
  /*------------------*/

  must_respond      = L7_FALSE;
  event_completed   = L7_FALSE;
  memset(&event_completion, 0, sizeof( ASYNC_EVENT_COMPLETE_INFO_t) );

  if  (response != L7_NULLPTR) 
  {
      if ( (response->handlerId != L7_NULL)  && (response->correlator != L7_NULL) )
      {
          must_respond = L7_TRUE; 

          /* Initialize event completion data */
          /* NOTE:  Purposely overloading componentID with L7_IPMAP_REGISTRANTS_t */
          event_completion.componentId  = L7_IPRT_VRRP;
          event_completion.handlerId    = response->handlerId;
          event_completion.correlator   = response->correlator;

          /* Default to return codes  and status.  Only select issues are not
             completed on this thread. */
          event_completion.async_rc.rc      = L7_SUCCESS;
          event_completion.async_rc.reason  = ASYNC_EVENT_REASON_RC_SUCCESS;
          event_completed                   = L7_TRUE;

      } /*  non null handlerId and correlator */

  }       
      
  /*------------------*/
  /* Process event    */
  /*------------------*/


    if (intIfNum == 0 ||
        ((L7_vrrpIntfIsValid(intIfNum) == L7_TRUE)))
    {
      switch (event)
      {
      case L7_RTR_ENABLE:
        /* L7_RTR_ENABLE need not be handled 
             as the interface level events would take care of it*/
        break;          

      case L7_RTR_DISABLE_PENDING:

        /* handle the vrrp interface tracking events here*/
        (void)vrrpTrackingInterfaceDisable(intIfNum);
        
        if (intIfNum == 0 || (vrrpInterfaceAssociate(intIfNum) == L7_TRUE))
        {
          vrrpRoutingIntfDisable(0);    /* consider all routing interfaces down */
          L7_vrrpAllInfRouterStop(intIfNum);
        }
        
        break;          

      case L7_RTR_DISABLE:
        break;

      case L7_RTR_INTF_ENABLE: 
        /* Call to IP MAP must be made w/o holding VRRP semaphore. Don't consider
         * interface up if unnumbered. No need to check NIM state. IP MAP 
         * wouldn't declare the interface up if it were not active. */
        if (intIfNum == 0 || (vrrpInterfaceAssociate(intIfNum) == L7_TRUE))
        {
          if ((ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &mask) == L7_SUCCESS) 
              && (ipAddr != 0))
          {
            vrrpRoutingIntfEnable(intIfNum, ipAddr, mask);
            /* Check if any VR is configured on the interface and if 
             * VRRP is globally enabled. If so, start VRs on this interface. */
            if ((vrrpInterfaceAssociate(intIfNum) == L7_TRUE) && 
                (pVrrpMapCfgData->rtr.vrrpAdminMode == L7_TRUE))
            {
              L7_vrrpAllInfRouterStart(intIfNum);
            }
          }
        }
        /* handle the vrrp interface tracking events here*/
        (void)vrrpTrackingInterfaceEnable(intIfNum);
        break;
       
      case L7_RTR_INTF_DISABLE_PENDING:

        /* handle the vrrp interface tracking events here*/
        (void)vrrpTrackingInterfaceDisable(intIfNum);
        
        if (intIfNum == 0 || (vrrpInterfaceAssociate(intIfNum) == L7_TRUE))
        {
          vrrpRoutingIntfDisable(intIfNum);
          L7_vrrpAllInfRouterStop(intIfNum);
        }
        break;

      case L7_RTR_INTF_DISABLE:
        /* Nothing to do. We cleaned up on the disable pending */
        break;

      case L7_RTR_INTF_IPADDR_ADD:
      case L7_RTR_INTF_IPADDR_DELETE_PENDING:
      case L7_RTR_INTF_IPADDR_DELETE:
        /* deprecated */
        break;                          

      case L7_RTR_INTF_CONFIG_CLEAR:
        /* Clear the interface configuration */
        if (intIfNum == 0 || (vrrpInterfaceAssociate(intIfNum) == L7_TRUE))
        {
          L7_vrrpRouterIntfConfigClear(intIfNum);
        }
        break;

      case L7_RTR_INTF_SECONDARY_IP_ADDR_ADD:
        /* Nothing to do */
        break;
      case L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE:
        /* Nothing to do */
        break;

      default:
        break;
      }
    }

  /*-------------------*/
  /* Event completion? */
  /*-------------------*/

  if ( (must_respond == L7_TRUE) && (event_completed == L7_TRUE) )
  {
     /* Return event completion status  */
      asyncEventCompleteTally(&event_completion);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process an RTO best route change on the VRRP thread.
*
* @param    routeEntry  @b{(input)}  Route that was added/deleted/modified
* @param    route_status  @b{(input)}  one of RTO_ROUTE_EVENT_t
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t vrrpMapRtoRouteChangeProcess(L7_routeEntry_t *routeEntry,
                                       RTO_ROUTE_EVENT_t route_status)
{
  switch (route_status)
  {
    case RTO_ADD_ROUTE    : (void)vrrpMapTrackRouteEntryEnable(routeEntry->ipAddr,routeEntry->subnetMask);
                            break;

    case RTO_DELETE_ROUTE : (void)vrrpMapTrackRouteEntryDisable(routeEntry->ipAddr,routeEntry->subnetMask);
                            break;

    case RTO_CHANGE_ROUTE :
                            break;                       
  }
  
  return L7_SUCCESS;
  
}

/*********************************************************************
* @purpose  RTO calls this function when a best route is added/deleted/changed.
*           VRRP Uses this events for Route Tracking.
*
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
void vrrpMapRouteCallback()
{
  if (!pVrrpMapInfo->bestRouteTimer)
    /* schedule an event to request changes from RTO */
    osapiTimerAdd((void*)vrrpMapBestRouteTimerCb, L7_NULL, L7_NULL,
        L7_VRRP_BEST_ROUTE_DELAY, &pVrrpMapInfo->bestRouteTimer);

  return;
}


/*********************************************************************
*
* @purpose Timer callback to trigger VRRP to get best routes from RTO.
*
* @param void
*
* @returns void
*
* @comments  This callback tells VRRP that best route changes are pending
*            in RTO. VRRP has to go back to RTO and ask for the changes.
*
* @end
*
*********************************************************************/
void vrrpMapBestRouteTimerCb(void)
{
  pVrrpMapInfo->bestRouteTimer = NULL;
  vrrpMapBestRouteEventPost();
}


/*********************************************************************
*
* @purpose Trigger VRRP to request best route changes from RTO.
*
* @param void
*
* @returns void
*
* @comments  Called either when timer expires to retrieve initial set
*            of changes after a notification from RTO or when more
*            changes are pending after getting a set of changes.
*
* @end
*
*********************************************************************/
static void vrrpMapBestRouteEventPost(void)
{
  /* Put event on IP MAP processing thread. */
  L7_RC_t rc;
  vrrpMsg_t msg;
  if (vrrp_Queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
        "vrrp_Queue is NULL");
    return;
  }

  memset( (void *)&msg, 0, sizeof(vrrpMsg_t) );
  msg.msg_id = vrrpMsgBestRouteEvent;

  rc = osapiMessageSend(vrrp_Queue, &msg, sizeof(vrrpMsg_t),
      L7_NO_WAIT, L7_MSG_PRIORITY_NORM );
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
        "osapiMessageSend() failed with rc = %u", rc);
  }

  return;
}

/*********************************************************************
*
* @purpose  Ask RTO for a set of best route changes. If more changes
*           remain, queue up another event to ask for more.
*
* @param void
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t vrrpMapBestRoutesGet(void)
{
  L7_uint32 i;
  L7_uint32 numChanges = 0;
  L7_BOOL moreChanges = L7_FALSE;
  rtoRouteChange_t *routeChange;

  if (rtoRouteChangeRequest(vrrpMapRouteCallback, L7_VRRP_MAX_BEST_ROUTE_CHANGES,
        &numChanges, &moreChanges,
        vrrpMapRouteChangeBuf) != L7_SUCCESS)
  {
    /* Don't schedule another try. */
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
        "Failed to retrieve best route changes from RTO.");
    return L7_FAILURE;
  }

  /* process each best route change */
  for (i = 0; i < numChanges; i++)
  {
    routeChange = &vrrpMapRouteChangeBuf[i];
    vrrpMapRtoRouteChangeProcess(&routeChange->routeEntry, routeChange->changeType);
  }

  if (moreChanges)
  {
    /* post another event to the VRRP message queue to request more
     * changes from RTO */
    vrrpMapBestRouteEventPost();
  }

  return L7_SUCCESS;
}




/*********************************************************************
* @purpose  To increment the the number of packet received with 
*           invalid checksum
*
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpChecksumErrorIncr()
{
  vrrpBasicConfig_g.checksum_err++;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To increment the the number of VRRP packet received with wrong 
*           vrrp version
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpRouterVerErrorIncr()
{
  vrrpBasicConfig_g.version_err++;
  return L7_SUCCESS;
}                              

/*********************************************************************
* @purpose  To increment the the number of packet received with wrong vrrpid
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpVridErrorIncr()
{
  vrrpBasicConfig_g.vrid_error++;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To fetch the configuration data from the list
*
* @param    vrid       The virtual router id.
* @param    intIfNum   Interface Number
*
* @returns  Pointer to the configuration data
*
* @notes    This function is for the vrrp configuration part only.
*           Function doesn't release semaphore if an object is found.
*       
* @end
*********************************************************************/
vrrpRouterInfo_t*  L7_vrrpParamsFetch (L7_uchar8 vrid, 
                                       L7_uint32 intIfNum)

{
  vrrpList_t* listTail;
  vrrpRouterInfo_t* routinfo = L7_NULLPTR;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  /* check if current points to the object to be fetch */
  if (paramCurrent_g != L7_NULLPTR && 
      paramCurrent_g->object->vrrpCfgInfo.vrid == vrid && 
      paramCurrent_g->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
  {
    return(vrrpRouterInfo_t*) paramCurrent_g->object;
  }

  while (listTail!= L7_NULLPTR && 
         listTail->object->vrrpVirtRouterOperInfo.intIfNum < intIfNum)
  {
    listTail = listTail->next;
  }

  while ( listTail != L7_NULLPTR && 
          listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum &&
          listTail->object->vrrpCfgInfo.vrid< vrid )
  {
    listTail = listTail->next;
  }

  if (listTail!=L7_NULLPTR && listTail->object->vrrpCfgInfo.vrid == vrid && 
      listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
  {
    routinfo = listTail->object;
    paramCurrent_g = listTail; 
    return routinfo;
  }
  /* if object not found then free the semaphor and return Null*/
  osapiSemaGive(VrrpConfigSema); 
  return routinfo;
}

/*********************************************************************
* @purpose  To fetch the configuration data from the list
*
* @param    ipAddr     The ip address to check
* @param    intIfNum   Interface Number
*
* @returns  Pointer to the configuration data
*
* @notes    This function is for the vrrp configuration part only.
*           Function doesn't release semaphore if an object is found.
*       
* @end
*********************************************************************/
vrrpRouterInfo_t *L7_vrrpIPParamsFetch(L7_uint32 ipAddr,
									   L7_uint32 intIfNum)

{
	vrrpList_t			*listTail;
	L7_uint32			i;
	vrrpRouterInfo_t	*routinfo = L7_NULLPTR;

	osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

	listTail = paramHead_g;

	while (listTail != L7_NULLPTR)
	{
		if (listTail->object->vrrpVirtRouterOperInfo.intIfNum != intIfNum)
		{
			listTail = listTail->next;
			continue;
		}

		for (i=0; i<L7_L3_NUM_IP_ADDRS; i++)
		{
			if (listTail->object->vrrpCfgInfo.ipaddress[i] == ipAddr)
			{
				/*
				printf("L7_vrrpIPParamsFetch: found object for 0x%08x, intf %d, vrid %d, index %d\n",
					   ipAddr, intIfNum, listTail->object->vrrpCfgInfo.vrid, i);
				*/

				routinfo = listTail->object;
				paramCurrent_g = listTail; 

				/*
				Caller will free semaphore if this succeeds
				*/

				return routinfo;
			}
		}

		listTail = listTail->next;
	}

	/* if object not found then free the semaphore and return Null*/
	osapiSemaGive(VrrpConfigSema); 

	return routinfo;
}

/*********************************************************************
* @purpose  To fetch the configuration data from the list
*
* @param    vrid       The virtual router id.
* @param    intIfNum   Interface Number
*
* @returns  Pointer to the configuration data
*
* @notes    This function is for the vrrp configuration part only.
*           Function doesn't release semaphore if an object is found.
*           This method doesn't use lock 
*       
* @end
*********************************************************************/
vrrpRouterInfo_t*  L7_vrrpVirtualRouterFetch (L7_uchar8 vrid, 
                                              L7_uint32 intIfNum)

{
  vrrpList_t* listTail;
  vrrpRouterInfo_t* routinfo = L7_NULLPTR;


  listTail = paramHead_g;
  /* check if current points to the object to be fetch */
  if (paramCurrent_g != L7_NULLPTR && 
      paramCurrent_g->object->vrrpCfgInfo.vrid == vrid && 
      paramCurrent_g->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
  {
    return(vrrpRouterInfo_t*) paramCurrent_g->object;
  }

  while (listTail!= L7_NULLPTR && 
         listTail->object->vrrpVirtRouterOperInfo.intIfNum < intIfNum)
  {
    listTail = listTail->next;
  }

  while ( listTail != L7_NULLPTR && 
          listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum &&
          listTail->object->vrrpCfgInfo.vrid< vrid )
  {
    listTail = listTail->next;
  }

  if (listTail!=L7_NULLPTR && listTail->object->vrrpCfgInfo.vrid == vrid && 
      listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
  {
    routinfo = listTail->object;
    paramCurrent_g = listTail; 
    return routinfo;
  }
  /* if object not found then free the semaphor and return Null*/

  return routinfo;
}

/*********************************************************************
* @purpose  For each VR on a given interface, note that the routing 
*           interface is up and cache the primary IP address on the
*           interface.
*
* @param    intIfNum   internal interface number
* @param    ipAddr     primary IP address on this interface
* @param    intIfNum   mask for primary IP address on interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    We cache this info from IP MAP so VRRP doesn't have to go back to
*           IP MAP whenever it needs the local IP addr. IP MAP, ARP, and VRRP
*           can deadlock if they all try to call each other.
*
* @end
*********************************************************************/
L7_RC_t vrrpRoutingIntfEnable(L7_uint32 intIfNum, L7_uint32 ipAddr, 
                              L7_uint32 netmask)
{
  vrrpList_t *listTail;
  vrrpVirtRouterOperData_t *vrState;

  if (intIfNum == 0)
    return L7_FAILURE;

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  while ((listTail != L7_NULLPTR) && 
         (listTail->object->vrrpVirtRouterOperInfo.intIfNum < intIfNum))
  {
    listTail = listTail->next;
  }

  while ((listTail != L7_NULLPTR) && 
         (listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum))
  {
    vrState = &listTail->object->vrrpVirtRouterOperInfo;
    vrState->rtrIntfUp = L7_TRUE;
    vrState->localAddr.ipAddr = ipAddr;
    vrState->localAddr.ipMask = netmask;
    
    listTail = listTail->next;
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  For each VR on a given interface, note that the routing 
*           interface is down and clear the cached primary IP address on the
*           interface.
*
* @param    intIfNum   internal interface number. If 0, consider all routing
*                      interfaces down.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    We cache this info from IP MAP so VRRP doesn't have to go back to
*           IP MAP whenever it needs the local IP addr. IP MAP, ARP, and VRRP
*           can deadlock if they all try to call each other.
*
* @end
*********************************************************************/
L7_RC_t vrrpRoutingIntfDisable(L7_uint32 intIfNum)
{
  vrrpList_t *listTail;
  vrrpVirtRouterOperData_t *vrState;

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  listTail = paramHead_g;
  while (intIfNum &&
         (listTail != L7_NULLPTR) && 
         (listTail->object->vrrpVirtRouterOperInfo.intIfNum < intIfNum))
  {
    listTail = listTail->next;
  }

  while ((listTail != L7_NULLPTR) && 
         ((intIfNum == 0) ||
         (listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum)))
  {
    vrState = &listTail->object->vrrpVirtRouterOperInfo;
    vrState->rtrIntfUp = L7_FALSE;
    vrState->localAddr.ipAddr = 0;
    vrState->localAddr.ipMask = 0;
    
    listTail = listTail->next;
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Start all the routers, configured on a particular 
*           interface
*
* @param    intIfNum   Interface Number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAllInfRouterStart(L7_uint32 intIfNum)
{
  vrrpList_t* listTail;  

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  /* check if current points to the object to be fetch */

  if (intIfNum != 0)
  {

    while (listTail!= L7_NULLPTR && 
           listTail->object->vrrpVirtRouterOperInfo.intIfNum < intIfNum)
    {
      listTail = listTail->next;
    }

    while ( listTail != L7_NULLPTR && 
            listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
    {
      if ((listTail->object->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_INIT) &&
          (L7_vrrpCheckPrecondition(listTail->object) == L7_SUCCESS))
      {
        (void)dtlIpv4LocalMulticastAddrAdd(listTail->object->vrrpVirtRouterOperInfo.intIfNum, 
                                           L7_IP_VRRP_ADDR);
        L7_vrrpRouterStart(listTail->object);
      }
      listTail = listTail->next;
    }

  }
  else
  {
    while ( listTail != L7_NULLPTR)
    {
      if ((L7_vrrpCheckPrecondition(listTail->object) == L7_SUCCESS) &&
          (listTail->object->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_INIT))
      {
        (void)dtlIpv4LocalMulticastAddrAdd(listTail->object->vrrpVirtRouterOperInfo.intIfNum, 
                                           L7_IP_VRRP_ADDR);
        L7_vrrpRouterStart((listTail->object));
      }
      listTail = listTail->next;
    }

  }

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Stop all the routers, configured on a particular 
*           interface
*
* @param    intIfNum   Interface Number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAllInfRouterStop(L7_uint32 intIfNum)
{
  vrrpList_t* listTail;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  /* check if current points to the object to be fetch */

  if (intIfNum != 0)
  {
    while (listTail!= L7_NULLPTR && 
           listTail->object->vrrpVirtRouterOperInfo.intIfNum < intIfNum)
    {
      listTail = listTail->next;
    }

    while ( listTail != L7_NULLPTR && 
            listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
    {
      if (listTail->object->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
      {
        if (listTail->object->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_MASTER)
          VRRPShutdownVirtualRouter(listTail->object, listTail->object->vrrpVirtRouterOperInfo.intIfNum);
        L7_vrrpRouterStop(listTail->object);
      }

      listTail = listTail->next;

    }

  }
  else
  {
    while ( listTail != L7_NULLPTR)
    {
      if (listTail->object->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
      {
        if (listTail->object->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_MASTER)
          VRRPShutdownVirtualRouter(listTail->object, listTail->object->vrrpVirtRouterOperInfo.intIfNum);
        L7_vrrpRouterStop(listTail->object);
      }

      listTail = listTail->next;

    }

  }

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To stop and clear the config of all the routers 
*           configured on a particular interface
*
* @param    intIfNum   Interface Number
*
* @returns  L7_SUCCESS  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpRouterIntfConfigClear(L7_uint32 intIfNum)
{
  vrrpList_t *listTail;
  L7_short16 intfInfoTblIndex;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;

  while (listTail != L7_NULLPTR && 
         listTail->object->vrrpVirtRouterOperInfo.intIfNum < intIfNum)
  {
    listTail = listTail->next;
  }

  while ( listTail != L7_NULLPTR && 
          listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
  {
    if (listTail->object->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
    {
      if (listTail->object->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_MASTER)
        VRRPShutdownVirtualRouter(listTail->object, listTail->object->vrrpVirtRouterOperInfo.intIfNum);
      L7_vrrpRouterStop(listTail->object);
    }

    if (listTail->previous == L7_NULLPTR)
      paramHead_g = listTail->next;
    else
      listTail->previous->next = listTail->next;

    if (listTail->next != L7_NULLPTR)
      listTail->next->previous = listTail->previous;

    listTail->object->vrrpVirtRouterOperInfo.intIfNum = L7_NULL;
    listTail->object->vrrpCfgInfo.vrid = L7_NULL;

    intfInfoTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);
    if (intfInfoTblIndex != -1)
    {
      (vrrpIntfInfoTbl[intfInfoTblIndex].totalConfigured)--;
      totalRouters_g--;
      pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    }

    listTail = listTail->next;
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}                                                

/*********************************************************************
* @purpose  Function to check if there is a configuration associated with
*           a given interface number.
*
* @param    intIfNum   The interface number 
*
* @returns  row index
* @returns  -1 if Maximum capacity reach
*
* @notes    This function keeps the count of routers memory allocated
*           from the pool and also return the row number to be used 
*           for router configuration.
*       
* @end
*********************************************************************/
L7_BOOL vrrpInterfaceAssociate(L7_uint32 intIfNum)
{
  L7_short16 i;

  for (i= 1; i<=L7_VRRP_INTF_MAX_COUNT; i++)
  {
    if (vrrpIntfInfoTbl[i].intIfNum == intIfNum && vrrpIntfInfoTbl[i].totalConfigured>0 )
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Function to check if a frame should be dropped due to 
*           VRRP criteria.
*
* @param    hookdId          @b{(input)} Hook ID being called
* @param    bufHandle        @b{(input)} Buffer handle to the frame
* @param    *pduInfo         @b{(input)} Pointer to the PDU info (intIfNum, VLAN ID, etc.)
* @param    continueFunc     @b{(input)} Function to be called to continue processing the frame
*
* @returns  SYSNET_PDU_RC_IGNORED    frame was ignored and should continue to be processed
*           SYSNET_PDU_RC_DISCARD    if frame should be discarded unprocessed
*
* @notes    This function determines whether an IP frame that is 
*           addressed to a VRRP vmac should be handled or dropped.
*           The packets of interest were received because their
*           destination MAC address matched a VRRP VMAC.  However,
*           we should only act on frames with an IP address specifying
*           an IP address we "own".  The RFC states a virtual router
*           in the Master state:
*           'MUST NOT accept packets addressed to the IP address(es)
*           associated with the virtual router if it is not the IP
*           address owner.'
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t vrrpIpFrameFilter(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc)
{
	L7_BOOL				adminMode;
	SYSNET_PDU_RC_t		result = SYSNET_PDU_RC_IGNORED;
	vrrpRouterInfo_t	*vRouterInfo = L7_NULLPTR;
	L7_uchar8			*data;
	L7_ipHeader_t		*ipHeader;
	L7_uint32			destIp;

	SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
	ipHeader = (L7_ipHeader_t *)(data + sysNetDataOffsetGet(data));
	destIp = (L7_uint32)osapiNtohl(ipHeader->iph_dst);

	if ((L7_vrrpAdminModeGet(&adminMode) == L7_SUCCESS) && (adminMode == L7_TRUE))
	{
		/*
		The previous test:

		if (memcmp( vrrpMacTemplate, destMac, 5) == 0)

		does not work on Broadcom, as when a packet gets transferred
		to the system CPU, it gets a new MAC address

		Look for the IP address existing on the VRRP interface instead
		*/

		/*
		printf("vrrpIpFrameFilter: VRRP active, packet to 0x%08x, intf %d\n", destIp,
			   pduInfo->intIfNum);
		*/

		if ((vRouterInfo = L7_vrrpIPParamsFetch(destIp, pduInfo->intIfNum)) != L7_NULLPTR)
		{
			/*
			printf("vrrpIpFrameFilter: 0x%08x is a VRRP address, priority = %d\n",
				   destIp, vRouterInfo->vrrpCfgInfo.priority);
			*/
			if (vRouterInfo->vrrpVirtRouterOperInfo.oper_priority != 255)
			{
				/*
				printf("vrrpIpFrameFilter: packet to 0x%08x will be discarded\n", destIp);
				*/
				result = SYSNET_PDU_RC_DISCARD;
			}

			/* If a router was returned from "fetch" call above, give back the semaphore */
			osapiSemaGive(VrrpConfigSema);
		}
	}
	/*
	else
	{
		printf("vrrpIpFrameFilter: packet to 0x%08x will be retained\n", destIp);
	}
	*/

	return result;
}

/*********************************************************************
* @purpose  Function to check if a frame should be handled by VRRP
*
* @param    hookdId          @b{(input)} Hook ID being called
* @param    bufHandle        @b{(input)} Buffer handle to the frame
* @param    *pduInfo         @b{(input)} Pointer to the PDU info (intIfNum, VLAN ID, etc.)
* @param    continueFunc     @b{(input)} Function to be called to continue processing the frame
*
* @returns  SYSNET_PDU_RC_CONSUMED   frame was consumed by VRRP
*           SYSNET_PDU_RC_DISCARD    if frame should be discarded unprocessed
*           SYSNET_PDU_RC_IGNORED    if frame was ignored and should continue to be processed
*
* @notes    none 
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t vrrpPacketIntercept(L7_uint32 hookId,
                                    L7_netBufHandle bufHandle,
                                    sysnet_pdu_info_t *pduInfo,
                                    L7_FUNCPTR_t continueFunc)
{
  L7_uchar8 *data;
  L7_enetHeader_t *et;
  L7_ipHeader_t *ipHeader;
  SYSNET_PDU_RC_t result = SYSNET_PDU_RC_IGNORED;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  et = (L7_enetHeader_t *) data;
  ipHeader = (L7_ipHeader_t*)(data + sysNetDataOffsetGet(data));

  if ( (ipHeader->iph_versLen >> 4 == L7_IP_VERSION) &&
       ((ipHeader->iph_versLen & 0x0f) == L7_IP_HDR_VER_LEN) &&
       (ipHeader->iph_prot == IP_PROT_VRRP) )
  {
    if (bcmp((caddr_t)et->src.addr, (caddr_t)L7_ENET_VRRP_MAC_ADDR.addr, 
             L7_ENET_MAC_ADDR_LEN - 1) == 0)
    {
      if ( (osapiNtohl(ipHeader->iph_dst) != L7_IP_VRRP_ADDR) ||
           (bcmp((caddr_t)et->dest.addr,
                 (caddr_t)L7_IP_VRRP_ADVT_MAC_ADDR.addr,
                 L7_ENET_MAC_ADDR_LEN) != 0) )
      {
        LOG_MSG("not vrrp multicast packet");
        result = SYSNET_PDU_RC_CONSUMED;
      }
      else if (osapiNtohl(ipHeader->iph_dst)  == L7_IP_VRRP_ADDR)
      {
        if (vrrpPacketProcess(bufHandle, pduInfo->intIfNum) == L7_SUCCESS)
          result = SYSNET_PDU_RC_CONSUMED;
      }
    }
  }

  return result;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
/* L7_BOOL vrrpIntfIsConfigurable(L7_uint32 intIfNum, vrrpVirtRouterCfg_t **pCfg) */
L7_BOOL vrrpIntfIsConfigurable(L7_uint32 intIfNum, vrrpIntfInformation_t **pIntfInfo)
{

  nimConfigID_t configId;
  L7_short16    intfInfoTblIndex;

  if (VRRP_IS_READY != L7_TRUE)
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  /* Why was there a move away from including the following call? */
  if (L7_vrrpIntfIsValid(intIfNum) == L7_FALSE)
      return L7_FALSE;

  intfInfoTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);

  if(intfInfoTblIndex == -1)
  {
      return L7_FALSE;
  }
  
  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
  {
      return L7_FALSE;
  }

  if(pIntfInfo != L7_NULLPTR)
      *pIntfInfo = &(vrrpIntfInfoTbl[intfInfoTblIndex]);

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL vrrpIntfInfoTblEntryGet(L7_uint32 intIfNum, vrrpIntfInformation_t **pIntfInfo)
{
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  L7_short16    intfInfoTblIndex;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (VRRP_IS_READY != L7_TRUE)
    return L7_FALSE;

  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
        /* Retrieve the intfInfoTblIndex for intIfNum */
        intfInfoTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);
        if(intfInfoTblIndex != -1)
        {
            *pIntfInfo = &(vrrpIntfInfoTbl[intfInfoTblIndex]);
            return L7_TRUE;
        }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Initializes the array vrrpVRIdToIndexMapTbl (to 0).
*
* @param    None
*
* @returns  void
*
* @end
*********************************************************************/
void vrrpVRIdToIndexMapTblInit()
{
   L7_uint32 i;

    for (i = 0; i < L7_IPMAP_INTF_MAX_COUNT; i++)
    {
      memset(&(vrrpVRIdToIndexMapTbl[i][0]),0,
             L7_RTR_MAX_VRRP_PER_INTERFACE*sizeof(L7_uchar8));
    }
}

/*********************************************************************
* @purpose  Gets the array index (into the arrays listMemPool_g and 
*           routersArray_g) for the given vrId on the given interface
*           (specified by intIfNum).
*
* @param    vrId        Virtual Router ID
* @param    intIfNum    The internal interface number of the interface 
*                       on which the virtual router resides.
*
* @returns  The array index for the given vrId and intIfNum.
*           Return value of 0 implies the vrId hasnt been mapped to an array 
*           index.
*
* @notes    intIfNum is assumed to be greater than 0. Also, see comment attached
*           with the declaration of the array vrrpVRIdToIndexMapTbl.
*
* @end
*********************************************************************/
L7_uchar8 vrrpVRIdToIndexGet(L7_uchar8 vrId, L7_uint32 intIfNum)
{
    /* L7_ROUTING_VRRP_MIN_VRID is typically 1. However, we allow it to 
    be different. */
    return vrrpVRIdToIndexMapTbl[intIfNum-1][vrId - L7_ROUTING_VRRP_MIN_VRID];
}

/*********************************************************************
* @purpose  Maps the the given vrId on the given interface (specified 
*           by intIfNum) to an available array index (for the arrays 
*           listMemPool_g and routersArray_g).
*
* @param    vrId        Virtual Router ID
* @param    intIfNum    The internal interface number of the interface 
*                       on which the virtual router resides.
*
* @returns  L7_SUCCESS  On successful mapping of (vrId, intIfNum) to an 
*                       array index.
*
*           L7_ALREADY_CONFIGURED If vrId has already been mapped to an
*                                 array index.    
*
*           L7_TABLE_IS_FULL  If there are already L7_VRRP_MAX_VIRT_ROUTERS 
*                       number of virtual routers residing on the given 
*                       interface.
*
* @notes    intIfNum is assumed to be greater than 0. Also, see comment 
*           attached with the declaration of the array vrrpVRIdToIndexMapTbl.
*
* @notes    Invoke vrrpVRIdToIndexGet() to retrieve the mapped array index.
*
* @end
*********************************************************************/
L7_RC_t vrrpVRIdToIndexMap(L7_uchar8 vrId, L7_uint32 intIfNum)
{
    L7_uchar8   arrayIdx, i, j;
    L7_BOOL isArrayIdxInUse;

    if(vrrpVRIdToIndexGet(vrId, intIfNum) != 0)
        return L7_ALREADY_CONFIGURED;
    
    arrayIdx = 1; /* A better alternative is to generate this randomly */
    isArrayIdxInUse = L7_FALSE;

    for(i = 1; i <= L7_VRRP_MAX_VIRT_ROUTERS; i++)
    {
        /* Check if arrayIdx is in use */
        for(j=0; j < L7_RTR_MAX_VRRP_PER_INTERFACE; j++)
        {
            if(vrrpVRIdToIndexMapTbl[intIfNum-1][j] == arrayIdx)
            {
                isArrayIdxInUse = L7_TRUE;
                break;
            }
        }
        if(isArrayIdxInUse == L7_FALSE)
        {
            /* L7_ROUTING_VRRP_MIN_VRID is typically 1. However, we allow it to 
            be different. */
            vrrpVRIdToIndexMapTbl[intIfNum-1][vrId - L7_ROUTING_VRRP_MIN_VRID] = arrayIdx;
            return L7_SUCCESS;
        }
        else
        {
            if(arrayIdx == L7_VRRP_MAX_VIRT_ROUTERS)
                arrayIdx = 1;
            else
                arrayIdx++;
            isArrayIdxInUse = L7_FALSE;
        }
    }

    return L7_TABLE_IS_FULL;
}

/*********************************************************************
* @purpose  Frees the array index (for the arrays listMemPool_g and 
*           routersArray_g) mapped to the given vrId on the given
*           interface (specified by intIfNum).
*
* @param    vrId        Virtual Router ID
* @param    intIfNum    The internal interface number of the interface 
*                       on which the virtual router resides.
*
* @returns  L7_SUCCESS  On successful unmapping of array index.
*
* @notes    intIfNum is assumed to be greater than 0. Also, see comment 
*           attached with the declaration of the array vrrpVRIdToIndexMapTbl.
*
* @end
*********************************************************************/
L7_RC_t vrrpVRIdToIndexUnmap(L7_uchar8 vrId, L7_uint32 intIfNum)
{
    vrrpVRIdToIndexMapTbl[intIfNum-1][vrId - L7_ROUTING_VRRP_MIN_VRID] = 0;
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Notify NIM of an interface event. 
*
* @param    intIfNum  @b{(input)}  internal interface number
* @param    event     @b{(input)}  event from L7_PORT_EVENTS_t
* @parma    callback  @b{(input)}  callback function pointer. If non-NULL, 
*                                  NIM will call this function when the 
*                                  event is complete.
* @param    handle    @b{(output)} identifies the event in case the caller
*                                  requires notification of event completion
*
* @returns  L7_SUCCESS    If event was accepted by NIM
* @returns  L7_FAILURE    If event was not accepted by NIM
*
* @notes    The events generated by VRRP are a virtual router on this 
*           interface transitioning from one state to another (master to 
*           backup or backup to master)
*          
* @end
*********************************************************************/
L7_RC_t vrrpMapIntfChangeNimNotify(L7_uint32 intIfNum, L7_uint32 event,
                                 NIM_NOTIFY_CB_FUNC_t callback,
                                 NIM_HANDLE_t *handle)
{
    NIM_EVENT_NOTIFY_INFO_t eventInfo;
    
    eventInfo.component     = L7_VRRP_MAP_COMPONENT_ID;
    eventInfo.event         = event;
    eventInfo.intIfNum      = intIfNum;
    eventInfo.pCbFunc       = callback;

    return nimEventIntfNotify(eventInfo, handle);
}

/*********************************************************************
* @purpose  To fetch the configuration data from the list
*
* @param    vrid       The virtual router id.
* @param    intIfNum   Interface Number
*
* @returns  Pointer to the configuration data
*
* @notes    
*           
*
* @end
*********************************************************************/
vrrpRouterInfo_t *L7_vrrpParamsFetchLocked (L7_uchar8 vrid, L7_uint32 intIfNum,
                                            L7_BOOL next)
{
  vrrpList_t *listTail;
  vrrpRouterInfo_t *routinfo = L7_NULLPTR;

  listTail = paramHead_g;

  while (listTail != L7_NULLPTR &&
         listTail->object->vrrpVirtRouterOperInfo.intIfNum < intIfNum)
  {
    listTail = listTail->next;
  }

  if (next == L7_FALSE)
  {
    while (listTail != L7_NULLPTR &&
           listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum &&
           listTail->object->vrrpCfgInfo.vrid < vrid)
    {
      listTail = listTail->next;
    }

    if (listTail != L7_NULLPTR && listTail->object->vrrpCfgInfo.vrid == vrid &&
        listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum)
    {
      routinfo = listTail->object;
      return routinfo;
    }
  }
  else
  {
    while (listTail != L7_NULLPTR &&
           listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum &&
           listTail->object->vrrpCfgInfo.vrid <= vrid)
    {
      listTail = listTail->next;
    }
    if (listTail != L7_NULLPTR)
      routinfo = listTail->object;
  }
  
  /* if object not found then free the semaphor and return Null */
  return routinfo;
}

L7_RC_t L7_vrrpMgmtVmacAddrAdd(L7_uint32 intIfNum, L7_uchar8 *vmac)
{   
  L7_uint32 vlanId, sysIntfType;
  fdbMeberInfo_t macInfo;

  /*This is not required for port based routing interfaces*/
  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS && 
      sysIntfType != L7_LOGICAL_VLAN_INTF)
    return L7_SUCCESS;

  /* Doesn't take IP MAP semaphore */
  if (ipMapVlanRtrIntIfNumToVlanId(intIfNum, &vlanId) == L7_SUCCESS)
  {
    macInfo.entryType = L7_FDB_ADDR_FLAG_MANAGEMENT;
    macInfo.intIfNum = intIfNum;
    macInfo.vlanId = vlanId;
    memcpy(macInfo.macAddr, vmac, sizeof(macInfo.macAddr));

    return fdbAddEntry(&macInfo);
  }

  return L7_FAILURE;          
}

L7_RC_t L7_vrrpMgmtVmacAddrDel(L7_uint32 intIfNum, L7_uchar8 *vmac)
{   
  L7_uint32 vlanId, sysIntfType;
  fdbMeberInfo_t macInfo;

  /*This is not required for port based routing interfaces*/
  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS && 
      sysIntfType != L7_LOGICAL_VLAN_INTF)
    return L7_SUCCESS;

  /* Doesn't take IP MAP semaphore */
  if (ipMapVlanRtrIntIfNumToVlanId(intIfNum, &vlanId) == L7_SUCCESS)
  {
    macInfo.entryType = L7_FDB_ADDR_FLAG_MANAGEMENT;
    macInfo.intIfNum = intIfNum;
    macInfo.vlanId = vlanId;
    memcpy(macInfo.macAddr, vmac, sizeof(macInfo.macAddr));

    return fdbDelEntry(&macInfo);
  }

  return L7_FAILURE;          
}

/*********************************************************************
* @purpose  Process an RTO best route change on the VRRP thread.
*
* @param    ipAddr  @b{(input)}  Route that was added/deleted/modified
* @param    subnetMask @b{(input)}  
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t vrrpMapTrackRouteEntryEnable(L7_uint32 ipAddr, L7_uint32 subnetMask)
{

  vrrpList_t *listTail;
  vrrpVirtRouterCfg_t *vrrpCfgInfo;
  vrrpVirtRouterOperData_t *vrState;
  vrrpRouterInfo_t   *my_params;
  L7_ushort16 i;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uchar8 oper_prio;

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  listTail = paramHead_g;

  while ((listTail != L7_NULLPTR))
  {

    vrState = &listTail->object->vrrpVirtRouterOperInfo;
    vrrpCfgInfo = &listTail->object->vrrpCfgInfo;
    my_params =  listTail->object;
    oper_prio = vrState->oper_priority;

      for (i=0; i<L7_VRRP_MAX_TRACK_RT_PER_VR; i++)
      {
        if ((vrrpCfgInfo->vr_track_route[i].ipAddr == ipAddr ) &&
             (vrrpCfgInfo->vr_track_route[i].subnetMask == subnetMask))
        {
          if (vrrpCfgInfo->vr_track_route[i].reachable != L7_TRUE)
          {
            
            vrrpCfgInfo->vr_track_route[i].reachable = L7_TRUE;
            if (oper_prio != L7_VRRP_INTF_PRIORITY_MAX)
            {
            (void) vrrpOperationalPriorityCalc(my_params);
            }
             /* Logging the Event */
            nimGetIntfName(vrState->intIfNum, L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
                "Tracking Ip Route %x/%x  is reachable for VR %d on interface %s."
                " When VRRP Tracking Route Reachable event occurs.",ipAddr,subnetMask,
                vrrpCfgInfo->vrid, ifName);

          } /* end of if state*/

          break;
        } /* end intIfNum */

      }/* end of for loop */

    listTail = listTail->next;
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  Process an RTO best route change on the VRRP thread.
*
* @param    ipAddr  @b{(input)}  Prefix of the Route
                                 that was added/deleted/modified
* @param    subnetMask @b{(input)} Mask of the Route
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t vrrpMapTrackRouteEntryDisable(L7_uint32 ipAddr, L7_uint32 subnetMask)
{
  vrrpList_t *listTail;
  vrrpVirtRouterCfg_t *vrrpCfgInfo;
  vrrpVirtRouterOperData_t *vrState;
  vrrpRouterInfo_t   *my_params;
  L7_uchar8 oper_prio,i;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  listTail = paramHead_g;

  while ((listTail != L7_NULLPTR))
  {

    vrState = &listTail->object->vrrpVirtRouterOperInfo;
    vrrpCfgInfo = &listTail->object->vrrpCfgInfo;
    my_params =  listTail->object;
    oper_prio = vrState->oper_priority;

      for (i=0; i<L7_VRRP_MAX_TRACK_RT_PER_VR; i++)
      {
        if ((vrrpCfgInfo->vr_track_route[i].ipAddr == ipAddr) &&
            (vrrpCfgInfo->vr_track_route[i].subnetMask == subnetMask) )
        {
          if (vrrpCfgInfo->vr_track_route[i].reachable!= L7_FALSE)
          {
            vrrpCfgInfo->vr_track_route[i].reachable = L7_FALSE;
             /* Logging the Event */
            nimGetIntfName(vrState->intIfNum, L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
                "Tracking Ip Route %x/%x  is not reachable for VR %d on interface %s."
                " When VRRP Tracking Route not Reachable event occurs.",ipAddr,subnetMask,
                vrrpCfgInfo->vrid, ifName);

            if (oper_prio != L7_VRRP_INTF_PRIORITY_MAX )
            {
            (void) vrrpOperationalPriorityCalc(my_params);
            }
          }

          break;
        }/* end if intIfNum */

      }/* end for */


    listTail = listTail->next;

  }/* end while */

  osapiSemaGive(VrrpConfigSema);


  return L7_SUCCESS;
  
}

/*********************************************************************
* @purpose  Process interface Enable event which is tracked
*
* @param    intIfNum  @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t vrrpTrackingInterfaceEnable(L7_uint32 intIfNum)
{
  vrrpList_t *listTail;
  vrrpVirtRouterCfg_t *vrrpCfgInfo;
  vrrpVirtRouterOperData_t *vrState;
  vrrpRouterInfo_t   *my_params;
  L7_uchar8 oper_prio,i;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uchar8 trackIfName[L7_NIM_IFNAME_SIZE + 1];

  if (intIfNum == 0)
    return L7_FAILURE;

  nimGetIntfName(intIfNum, L7_SYSNAME, trackIfName);
  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  listTail = paramHead_g;

  while ((listTail != L7_NULLPTR))
  {
    
    vrState = &listTail->object->vrrpVirtRouterOperInfo;
    vrrpCfgInfo = &listTail->object->vrrpCfgInfo;
    my_params  = listTail->object;
    oper_prio = vrState->oper_priority;

      for (i=0; i<L7_VRRP_MAX_TRACK_INTF_PER_VR; i++)
      {
        if (vrrpCfgInfo->vr_track_intf[i].intIfNum == intIfNum)
        {
          if (vrrpCfgInfo->vr_track_intf[i].stateUp != L7_TRUE)
          {
            vrrpCfgInfo->vr_track_intf[i].stateUp = L7_TRUE;
            if (oper_prio != L7_VRRP_INTF_PRIORITY_MAX)
            {
              (void)vrrpOperationalPriorityCalc(my_params);
            }
            /* Logging the Event */
            nimGetIntfName(vrState->intIfNum,
                                 L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
                "Tracking Ip Interface %s  is Up for VR %d on interface %s."
                " When VRRP Tracking Interface UP event occurs.",trackIfName,
                vrrpCfgInfo->vrid, ifName);

          } /* end of if state*/

          break;
        } /* end intIfNum */

      }/* end of for loop */

    listTail = listTail->next;
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process interface disable event which is tracked
*
* @param    intIfNum  @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t vrrpTrackingInterfaceDisable(L7_uint32 intIfNum)
{
  vrrpList_t *listTail;
  vrrpVirtRouterCfg_t *vrrpCfgInfo;
  vrrpVirtRouterOperData_t *vrState;
  vrrpRouterInfo_t   *my_params;
  L7_uchar8 oper_prio,i;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uchar8 trackIfName[L7_NIM_IFNAME_SIZE + 1];
  
  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  listTail = paramHead_g;

  while ((listTail != L7_NULLPTR))
  {

    vrState = &listTail->object->vrrpVirtRouterOperInfo;
    vrrpCfgInfo = &listTail->object->vrrpCfgInfo;
    my_params = listTail->object;
    oper_prio = vrState->oper_priority;

    for (i=0; i<L7_VRRP_MAX_TRACK_INTF_PER_VR; i++)
    {
      if (vrrpCfgInfo->vr_track_intf[i].intIfNum != 0)
      {
        if ((vrrpCfgInfo->vr_track_intf[i].intIfNum == intIfNum) || (intIfNum == 0))
        {
          if (vrrpCfgInfo->vr_track_intf[i].stateUp != L7_FALSE)
          {
            vrrpCfgInfo->vr_track_intf[i].stateUp = L7_FALSE;

            /* Loging the Event */
            nimGetIntfName(vrState->intIfNum, L7_SYSNAME, ifName);
            nimGetIntfName(intIfNum, L7_SYSNAME, trackIfName);
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
                "Tracking Ip Interface %s is Down for VR %d on interface %s."
                " When VRRP Tracking Interface DOWN event occurs.",trackIfName,
                vrrpCfgInfo->vrid, ifName);
            if (oper_prio != L7_VRRP_INTF_PRIORITY_MAX )
            {
              (void) vrrpOperationalPriorityCalc(my_params);
            }
          }
          if (intIfNum != 0)
          {
            break;
          }
        }/* end if intIfNum */
      }

    }/* end for */


    listTail = listTail->next;
    
  }/* end while */

  osapiSemaGive(VrrpConfigSema);


  return L7_SUCCESS;          
}

/*********************************************************************
* To Calculate the Operational priority considering the tracking objects Status 
*
* @param    virtualRouter  @b{input/output}  VRRP information
*
* @returns  none
*
* @end
*********************************************************************/
void vrrpOperationalPriorityCalc(vrrpRouterInfo_t *virtualRouter)
{
  L7_uint32 i;
  L7_uchar8 prio_dec;
  L7_uchar8 oper_priority;
  L7_int32 prio_sum = 0;
  L7_int32 prio_final;

  prio_final = virtualRouter->vrrpCfgInfo.priority;
  oper_priority = virtualRouter->vrrpVirtRouterOperInfo.oper_priority;

  if (oper_priority == L7_VRRP_INTF_PRIORITY_MAX)
  {
    return;
  }
  else
  {
    for (i = 0; i< L7_VRRP_MAX_TRACK_INTF_PER_VR; i++)
    {
      if (virtualRouter->vrrpCfgInfo.vr_track_intf[i].intIfNum != 0)
      {
        prio_dec = virtualRouter->vrrpCfgInfo.vr_track_intf[i].prio_dec;
        if (virtualRouter->vrrpCfgInfo.vr_track_intf[i].stateUp == L7_FALSE)
        {
          prio_sum -=prio_dec;
        }
      }  

      if (virtualRouter->vrrpCfgInfo.vr_track_route[i].ipAddr != 0)
      {
        prio_dec = virtualRouter->vrrpCfgInfo.vr_track_route[i].prio_dec;
        if (virtualRouter->vrrpCfgInfo.vr_track_route[i].reachable == L7_FALSE)
        {
          prio_sum -=prio_dec;
        }
      } 
    }/* end for */
  }
  prio_final +=prio_sum;

  if (prio_final <= 0)
  {
    virtualRouter->vrrpVirtRouterOperInfo.oper_priority = 1;
  }
  else
  {
    virtualRouter->vrrpVirtRouterOperInfo.oper_priority =prio_final;
  }

  return;   
}



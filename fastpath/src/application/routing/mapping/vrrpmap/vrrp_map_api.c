/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: VRRPAPI.c
*
* Purpose: This file contains the VRRP API functions.
*
* Component: Virtual Router redundancy protocol(VRRP)
*
* Commnets:
*
* Created by:TCS 28/11/2001
*
**********************************************************************/
/**********************************************************************

**********************************************************************/
/* System include file */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "l7_common.h"

#include "nimapi.h"

#include "l3_commdefs.h"
#include "l3_comm_structs.h"
#include "default_cnfgr.h"
#include "l3_default_cnfgr.h"
#include "l3_defaultconfig.h"
#include "vrrp_exports.h"

#include "l7_vrrp_api.h"
#include "vrrp_config.h"

#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "vrrp_util.h"

#include "l7_ip_api.h"
#include "l3_defaultconfig.h"

#include "rto_api.h"

/*global variable*/
vrrpList_t* paramHead_g = L7_NULLPTR;
L7_short16 totalRouters_g = 0;
vrrpList_t* paramCurrent_g = L7_NULLPTR;
vrrpRouterBasicConfiguration_t  vrrpBasicConfig_g;
vrrpList_t * listMemPool_g[L7_VRRP_INTF_MAX_COUNT + 1];   /* changes on 12/12 */
vrrpRouterInfo_t * routersArray_g[L7_VRRP_INTF_MAX_COUNT + 1];
vrrpIntfInformation_t * vrrpIntfInfoTbl;
extern L7_vrrpMapCfg_t     *pVrrpMapCfgData;
extern void      *VrrpConfigSema;


/*********************************************************************
* @purpose    Function to add the virtual router structure in the list.
*
* @param    vrid       The virtual router id.
* @param    intIfNum   Interface Nunmber
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVirtualRouterConfigAdd(L7_uchar8 vrid, L7_uint32 intIfNum)
{
  vrrpRouterInfo_t   *temp_params;
  L7_RC_t rc;
  L7_uint32 mode = L7_DISABLE;
  L7_IP_ADDR_t ipAddr = 0;
  L7_IP_MASK_t ipMask = 0;
  L7_BOOL rtrIntfUp = L7_FALSE;

  /* Check with IP MAP before taking VRRP sem */
  if ((ipMapRtrIntfOperModeGet(intIfNum, &mode) == L7_SUCCESS) &&
      (mode == L7_ENABLE))
  {
    rtrIntfUp = L7_TRUE;
  }

  /* ignore return. need to create VR instance anyway. */
  (void)ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask);

  if( (temp_params = L7_vrrpParamsFetch(vrid, intIfNum)) == L7_NULLPTR)
  {
    rc = vrrplistAdd(vrid, intIfNum, rtrIntfUp, ipAddr, ipMask);
    if (rc == L7_SUCCESS)
    {
       /* If the routing interface is numbered and up, start listening
        * to VRRP messages. */
       if ((mode == L7_ENABLE) && (ipAddr != 0))
       {
          rc = dtlIpv4LocalMulticastAddrAdd(intIfNum, L7_IP_VRRP_ADDR);
          if (rc != L7_SUCCESS)
          {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE+1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
                    "Failed to add VRRP multicast address to hardware for interface %s.",
                    ifName);
          }
       }
    }
    return rc;
  }
  /* Already in the list, giveup semaphore and return faliure*/
  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to delete the virtual router structure in the list
*
* @param    vrid       The virtual router id.
* @param    intIfNum   Interface Nunmber
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  L7_vrrpVirtualRouterConfigDelete(L7_uchar8 vrid, L7_uint32 intIfNum )
{
   L7_uint32 mode;
   L7_IP_ADDR_t ipAddr;
   L7_IP_MASK_t ipMask;

   if(vrrplistDelete(vrid,intIfNum) == L7_SUCCESS)
   {
     /* Clean up Process*/

     /* No need to do a VRID delete call to DTL.  If we got this far, the VRID
        was already deleted when this virtual router's status was set to "down" */

      /*
      ** check that intIfNum is a valid and enabled router interface and that an ip address
      ** has been applied before issuing driver call
      */
      if ((ipMapRtrIntfModeGet(intIfNum, &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
      {
         if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask) == L7_SUCCESS && ipAddr != 0)
         {
            dtlIpv4LocalMulticastAddrDelete(intIfNum,L7_IP_VRRP_ADDR);
         }
      }

     return L7_SUCCESS;
   }
   return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to get the VRRP priority
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    priority - pointer to priority (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperPriorityGet(L7_uchar8 routerID,
                               L7_uint32 intIfNum,
                               L7_uchar8* priority)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum) ))
  {
    *priority = my_params->vrrpVirtRouterOperInfo.oper_priority;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Function to get the VRRP priority
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    priority - pointer to priority (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpConfigPriorityGet(L7_uchar8 routerID,
                                 L7_uint32 intIfNum,
                                 L7_uchar8* priority)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum) ))
  {
    *priority = my_params->vrrpCfgInfo.priority;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Configure the priority for a specific virtual router on a given interface.
*
* @param    vrid     - virtual router ID
* @param    intIfNum - internal interface number where virtual router operates
* @param    priority - priority
*
* @returns  L7_SUCCESS
*           L7_REQUEST_DENIED if priority is out of range
*           L7_FAILURE if an error occurs
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpConfigPrioritySet(L7_uchar8 vrid,
                              L7_uint32 intIfNum,
                              L7_uchar8 priority)
{
  vrrpRouterInfo_t *vrData = L7_vrrpParamsFetch(vrid, intIfNum);
  if (!vrData)
  {
    return L7_FAILURE;
  }

  if ((priority <= L7_VRRP_INTF_PRIORITY_MIN) || (priority >= L7_VRRP_INTF_PRIORITY_MAX))
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_REQUEST_DENIED;
  }

  if (vrData->vrrpCfgInfo.priority == priority)
  {
    /* No change */
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }

  vrData->vrrpCfgInfo.priority = priority;
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  /* Update the operational priority */
  vrrpOperationalPriorityCalc(vrData);

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to get the VMAC address
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    vmac - pointer to char (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperVirtualMacAddrGet(L7_uchar8 routerID,
                                     L7_uint32 intIfNum,
                                     L7_uchar8* vmac)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    memcpy(vmac,my_params->vrrpVirtRouterOperInfo.vmac,L7_MAC_ADDR_LEN);
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to get the Ip address count
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    count - pointer to count (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperIpAddrCountGet(L7_uchar8 routerID,
                                  L7_uint32 intIfNum,
                                  L7_uchar8* count)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *count= my_params->vrrpVirtRouterOperInfo.ipAddressCount;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Function to get the Auth type
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    authType - pointer to authType (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAuthTypeGet(L7_uchar8 routerID, L7_uint32 intIfNum,
                               L7_AUTH_TYPES_t * authType)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *authType = my_params->vrrpCfgInfo.auth_types ;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to set the Auth type
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    authType - authType (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAuthTypeSet(L7_uchar8 routerID, L7_uint32 intIfNum,
                               L7_AUTH_TYPES_t  authType)
{
  vrrpRouterInfo_t   *my_params;

  if (authType > L7_AUTH_TYPE_SIMPLE_PASSWORD )
  {
     return L7_FAILURE;
  }

  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    my_params->vrrpCfgInfo.auth_types = authType;

    osapiSemaGive(VrrpConfigSema);
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;

  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to get the Auth Key
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    authKey - authKey pointer (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAuthKeyGet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_uchar8* authKey)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    strncpy(authKey, my_params->vrrpCfgInfo.auth_data,VRRP_AUTH_DATA_LEN);
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to Set the Auth Key
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    authKey - authKey pointer
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAuthKeySet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_uchar8* authKey)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    memcpy(my_params->vrrpCfgInfo.auth_data,authKey, VRRP_AUTH_DATA_LEN);

    osapiSemaGive(VrrpConfigSema);
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to Get the Advertisemnet Interval
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    advIntvl - advInterval  pointer (Out param)
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAdvertisementIntervalGet(L7_uchar8 routerID,
                                            L7_uint32 intIfNum,
                                            L7_uchar8* advIntvl)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *advIntvl = my_params->vrrpCfgInfo.adver_int;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to Set the Advertisemnet Interval
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    advIntvl - advInterval
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAdvertisementIntervalSet(L7_uchar8 routerID,
                                            L7_uint32 intIfNum,
                                            L7_uchar8 advIntvl)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    my_params->vrrpCfgInfo.adver_int = advIntvl;
    my_params->vrrpVirtRouterOperInfo.master_down_int =
           3*advIntvl + (255-my_params->vrrpVirtRouterOperInfo.oper_priority)/255 ;
    my_params->vrrpVirtRouterOperInfo.master_advt_int = advIntvl;

    osapiSemaGive(VrrpConfigSema);
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;

  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to Get the Preempt Mode
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    prmtFlg - pointer to preempt mode
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperPreemptModeGet(L7_uchar8 routerID,
                                  L7_uint32 intIfNum,
                                  L7_BOOL* prmtFlg)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *prmtFlg = my_params->vrrpCfgInfo.preempt_mode;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to Set the Preempt Mode
*
* @param    routerID -  router Id
* @param    intIfNum - Interface Number
* @param    prmtFlg -   preempt mode
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperPreemptModeSet(L7_uchar8 routerID,
                                  L7_uint32 intIfNum,
                                  L7_BOOL prmtFlg)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    my_params->vrrpCfgInfo.preempt_mode = prmtFlg;

    osapiSemaGive(VrrpConfigSema);
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Assign a primary IP address to a virtual router
*
* @param    routerID - VRRP router ID
* @param    intIfNum - interface where virtual router is configured
* @param    ipAddress - virtual router's primary IP address
*
* @returns  L7_SUCCESS if address change is accepted (or ipAddress is already
*                      this virtual router's primary IP address)
*           L7_ALREADY_CONFIGURED - IP address is already a secondary address
*                                   for this virtual router
*           L7_REQUEST_DENIED - IP address is not in the same subnet as any
*                               of the interface addresses
*           L7_FAILURE otherwise
*
* @notes    If the interface supports multiple subnets, the operator may
*           configure a different virtual router to serve each subnet. Thus,
*           the primary address of a virtual router may match the subnet of a
*           secondary IP address on the routing interface. We do check for such
*           a match when the virtual address is configured, but do not enforce
*           this correspondance as interface addresses change.
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAssocIpAddress(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_uint32 ipAddress)
{
  vrrpRouterInfo_t   *my_params;
  L7_uint32           i;

  /* Verify that IP addr being assigned to VR is in same subnet as
   * an address on the local interface. check this before taking VRRP semaphore to
   * avoid deadlock potential. */
  if (!ipMapIpAddrIsLocal(intIfNum, ipAddress))
  {
    return L7_REQUEST_DENIED;
  }

  my_params = L7_vrrpParamsFetch(routerID, intIfNum);
  if (my_params == NULL)
  {
    return L7_FAILURE;
  }

  /* If address already configured, just return */
  if (my_params->vrrpCfgInfo.ipaddress[0] == ipAddress)
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }

  /* If IP address used as a secondary address on the virtual router, return error */
  for (i = 1; i < L7_L3_NUM_IP_ADDRS; i++)
  {
      if (my_params->vrrpCfgInfo.ipaddress[i] == ipAddress)
      {
          osapiSemaGive(VrrpConfigSema);
          return L7_ALREADY_CONFIGURED;
      }
  }
  /* If VR is already running, stop it before changing the virtual IP address */
  if (my_params->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
  {
    L7_vrrpRouterStop(my_params);
  }
  if (my_params->vrrpCfgInfo.ipaddress[0] == 0)
  {
    my_params->vrrpVirtRouterOperInfo.ipAddressCount = 1;
  }
  my_params->vrrpCfgInfo.ipaddress[0] = ipAddress;
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  osapiSemaGive(VrrpConfigSema);

  /* Update the address owner status of this VR before starting the VR.
   * Must give up the VRRP semaphore first. */
  vrrpAddressOwnerUpdateVr(intIfNum, routerID);

  /* Determine if virtual router is ready to run */
  my_params = L7_vrrpParamsFetch(routerID, intIfNum);
  if (my_params == NULL)
  {
    return L7_FAILURE;
  }
  if (L7_SUCCESS == L7_vrrpCheckPrecondition(my_params))
  {
    L7_vrrpRouterStart(my_params);
  }
  osapiSemaGive(VrrpConfigSema);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  To Set the VRRP mode
*
* @param    routerID   Router Id to be set.
* @param    intIfNum  Internal interface number.
* @param    state  Virtual router state.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVrrpModeSet(L7_uchar8 routerID, L7_uint32 intIfNum,
                           L7_vrrpStartState_t state)
{
  vrrpRouterInfo_t   *my_params;
  if( state == L7_VRRP_DOWN)
  {
    return L7_vrrpShutdownProcess(routerID,intIfNum);
  }
  else if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    if(my_params->vrrpCfgInfo.vr_start_state == state)
    {
      osapiSemaGive(VrrpConfigSema);
      return L7_SUCCESS;
    }
    my_params->vrrpCfgInfo.vr_start_state = state;
    if (L7_SUCCESS == L7_vrrpCheckPrecondition(my_params))
    {
      L7_vrrpRouterStart(my_params);
    }
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the VRRP mode
*
* @param    routerID   Router Id to be set.
* @param    intIfNum  Internal interface number.
* @param    state  Virtual router state.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVrrpModeGet(L7_uchar8 routerID,
                           L7_uint32 intIfNum,
                           L7_vrrpStartState_t* state)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *state = my_params->vrrpCfgInfo.vr_start_state;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the vrrp version supported by this node.
*
* @param    Version    Version of the vrrp supported
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
 @end
*********************************************************************/

L7_RC_t L7_vrrpNodeVersionGet(L7_uint32* version)
{
  *version = vrrpBasicConfig_g.vrrp_version;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  To get the status wheather VRRP-enabled router will generate
            SNMP traps for events defined in this MIB
*
* @param    Mode -     SNMP TRAP Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
 @end
*********************************************************************/

L7_RC_t L7_vrrpNotificationCntlGet(L7_uchar8* mode)
{
  *mode = vrrpBasicConfig_g.notif_flag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Set the SNMP traps Flag.
*
* @param    Mode -     SNMP TRAP Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t L7_vrrpNotificationCntlSet(L7_uchar8 mode)
{
  vrrpBasicConfig_g.notif_flag = mode;
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  To Get the state of router(Master/backup or initialised)
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    state -     State of the router(Out param)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpStateGet(L7_uchar8 routerID,
                        L7_uint32 intIfNum,
                        L7_vrrpState_t* state)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *state = my_params->vrrpVirtRouterOperInfo.vr_state;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the  Ip address of virtual router
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    ipAddress - virtual IpAddress(Out param)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpIpAddressGet(L7_uchar8 routerID,
                            L7_uint32 intIfNum,
                            L7_uint32* ipAddress)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *ipAddress = my_params->vrrpCfgInfo.ipaddress[0];
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the  Ip address of the master router
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    ipAddress  Master router's IP Address(Out param)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpMasterIpAddressGet(L7_uchar8 routerID,
                                  L7_uint32 intIfNum,
                                  L7_uint32* ipAddress)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *ipAddress = my_params->vrrpVirtRouterOperInfo.masterIpAddr;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the up time of the virtual Router
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    uptime - up time of the router (Out param)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpUpTimeGet(L7_uchar8 routerID,
                         L7_uint32 intIfNum,
                         L7_uint32* uptime)
{
  vrrpRouterInfo_t   *my_params;

  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *uptime = my_params->vrrpVirtRouterOperInfo.vrrpSysUpTime;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the The particular protocol being controlled by this
*           Virtual Router
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    protocol - protocol type
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpProtocolGet(L7_uchar8 routerID,
                           L7_uint32 intIfNum,
                           L7_vrrpProtocolType_t* protocol)

{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *protocol = my_params->vrrpVirtRouterOperInfo.protocol_type;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  To Set the The particular protocol being controlled by this
*           Virtual Router
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    protocol - protocol type
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpProtocolSet(L7_uchar8 routerID,
                           L7_uint32 intIfNum,
                           L7_vrrpProtocolType_t protocol)

{
  vrrpRouterInfo_t   *my_params;

  /* Currently only support IP protocol */
  if (protocol != L7_VRRP_IP)
    return L7_FAILURE;

  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    my_params->vrrpVirtRouterOperInfo.protocol_type=protocol;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  To Get the the number of packet received with invalid checksum
*
* @param    checkSumError - Checksum error(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpChecksumErrorGet(L7_uint32* checkSumError)
{
  *checkSumError = vrrpBasicConfig_g.checksum_err;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Get the the number of packet received with wrong vrrp version
*
* @param    versionError - version error(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpRouterVerErrorGet(L7_uint32* versionError)
{
  *versionError= vrrpBasicConfig_g.version_err;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Get the the number of packet received with wrong vrrpid
*
* @param    vrIdError - version Id Errors (Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVridErrorGet(L7_uint32* vrIdError)
{
  *vrIdError= vrrpBasicConfig_g.vrid_error;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Get the the number of time state change to master
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    masterState - master state transfer (Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpStateMasterGet(L7_uchar8 routerID,
                              L7_uint32 intIfNum,
                              L7_uint32* masterState)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *masterState = my_params->vrrpVirtRouterOperInfo.num_time_become_master;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  To Get the the number of vrrp advertisement received master
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    addReceived - Number of add received (Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAdverReceived(L7_uchar8 routerID,
                                   L7_uint32 intIfNum,
                                   L7_uint32* addReceived)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *addReceived = my_params->vrrpVirtRouterOperInfo.advert_received;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with wrong Addinterval
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    addIntErrors - Number of wrong interval pkt(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpAddIntErrorGet(L7_uchar8 routerID,
                              L7_uint32 intIfNum,
                              L7_uint32* addIntErrors)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *addIntErrors = my_params->vrrpVirtRouterOperInfo.pkt_advert_interval_error;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received which fails
*           the authentication criteria
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    authFailedPkt - Number of auth failed pkt(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAuthFailedGet(L7_uchar8 routerID,
                             L7_uint32 intIfNum,
                             L7_uint32* authFailedPkt)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *authFailedPkt = my_params->vrrpVirtRouterOperInfo.pkt_auth_failure;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with IP TTL
*           not equal to 255
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    ttlFailedPkt - Number of wrong ttl pkt received pkt(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpIpTTLFailedGet(L7_uchar8 routerID,
                              L7_uint32 intIfNum,
                              L7_uint32* ttlFailedPkt)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *ttlFailedPkt = my_params->vrrpVirtRouterOperInfo.pkt_ttl_incorrect;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with 0 priority
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    zeroPriorRcvPkt - number of zero priority pkt received(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpZeroPriorityRcvGet(L7_uchar8 routerID,
                                  L7_uint32 intIfNum,
                                  L7_uint32* zeroPriorRcvPkt)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *zeroPriorRcvPkt = my_params->vrrpVirtRouterOperInfo.pkt_rcvd_zero_priority;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt sent with 0 priority
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    zeroPriorSentPkt - number of zero priority pkt sent(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpZeroPrioritySentGet(L7_uchar8 routerID,
                                   L7_uint32 intIfNum,
                                   L7_uint32* zeroPriorSentPkt)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *zeroPriorSentPkt = my_params->vrrpVirtRouterOperInfo.pkt_sent_zero_priority;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with an
*           invalid value in the type field
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    invalidTypePkt - number of invalid type pkt received(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpInvldTypePktGet(L7_uchar8 routerID,
                               L7_uint32 intIfNum,
                               L7_uint32* invalidTypePkt)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *invalidTypePkt = my_params->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_type;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with an
*           invalid Ip address.
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    addListerrPkt - number of pkt received
*           with invalid ip address(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAddListErrGet(L7_uchar8 routerID,
                             L7_uint32 intIfNum,
                             L7_uint32* addListerrPkt)
{
   vrrpRouterInfo_t   *my_params;
   if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch(routerID,intIfNum)))
   {
     *addListerrPkt = my_params->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_ip_address_list;
     osapiSemaGive(VrrpConfigSema);
     return L7_SUCCESS;
   }
   return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with an
*           invalid Authentication type.
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    invAuthTypePkt - number of invalid pkt received
*           with an invalid Authentication type(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpInvldAuthTypeGet(L7_uchar8 routerID,
                                L7_uint32 intIfNum,
                                L7_uint32* invAuthTypePkt)
{
  vrrpRouterInfo_t   *my_params;
  if(L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *invAuthTypePkt = my_params->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_auth_type;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  To Get the the number of vrrp pkt received with mismatched
*           Authentication type.
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    authMismatchPkt - number of invalid pkt received
*           with mismatched Authentication data(Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAuthTypeMismatchGet(L7_uchar8 routerID,
                                   L7_uint32 intIfNum,
                                   L7_uint32* authMismatchPkt)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *authMismatchPkt = my_params->vrrpVirtRouterOperInfo.pkt_rcvd_mismatched_auth_type;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  To Get the the number of invalid vrrp pkt received
*
* @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    pktError - number of invalid pkt received (Out Param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpInvldPktGet(L7_uchar8 routerID,
                           L7_uint32 intIfNum,
                           L7_uint32* pktError)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *pktError = my_params->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_length;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

 /*********************************************************************
* @purpose  To Get the Admin Mode
*
* @param    adminMode   Admin mode
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAdminModeGet(L7_BOOL *adminMode)
{
  *adminMode = pVrrpMapCfgData->rtr.vrrpAdminMode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Set the Admin Mode
*
* @param    adminMode   Admin mode
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAdminModeSet(L7_BOOL adminMode)
{
  if (pVrrpMapCfgData->rtr.vrrpAdminMode == adminMode)
    return L7_SUCCESS;

  pVrrpMapCfgData->rtr.vrrpAdminMode = adminMode;

  if( adminMode == L7_TRUE)
  {
    /* Register with hook points that VRRP is interested in */
    if (vrrpSysnetRegister(L7_TRUE) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
              "L7_vrrpAdminModeSet: Error registering with sysnet\n");
      return L7_FAILURE;
    }
    vrrpRtoRegister();

    L7_vrrpStartAll();
  }
  else
  {
    /* Delete registration of VRRP hook points */
    (void)vrrpSysnetRegister(L7_FALSE);

    vrrpRtoRegister();

    L7_vrrpStopAll();
  }
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  To get the configuration struct of the next object from
*           the list.
*
* @param    vrId  Virtual router id
* @param    intIfNum int interface number
*
* @returns  vrrpRouterInfo_t pointer to the next configuration
* @returns  L7_NULLPTR if no parameter is present.
*
* @notes   if configuration is found then it return object with semaphore
*          else return L7_NULLPTR afrer giving the semaphore.
*
* @end
*********************************************************************/
vrrpRouterInfo_t* L7_vrrpNextObjectGet(L7_uchar8 vrId, L7_uint32 intIfNum)
{
  vrrpList_t* listTail;
  vrrpRouterInfo_t* routinfo = L7_NULLPTR;
  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  /* check if current points to the object to be fetch */
  if(paramCurrent_g != L7_NULLPTR && paramCurrent_g->object->vrrpCfgInfo.vrid == vrId &&
     paramCurrent_g->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
  {
    if(paramCurrent_g->next!= L7_NULLPTR)
    {
      paramCurrent_g = paramCurrent_g->next;
      return paramCurrent_g->object;
    }
    else
    {
      osapiSemaGive(VrrpConfigSema);
      return L7_NULLPTR;
    }
  }
  listTail = paramHead_g;
  while( listTail!= L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum<intIfNum )
  {
    listTail = listTail->next;
  }

  while ( listTail != L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum
              && listTail->object->vrrpCfgInfo.vrid< vrId )
  {
    listTail = listTail->next;
  }

  if(listTail!=L7_NULLPTR &&  listTail->object->vrrpCfgInfo.vrid == vrId &&
     listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
  {
    listTail = listTail->next;
    if( listTail != L7_NULLPTR)
    {
      routinfo = listTail->object;
      paramCurrent_g = listTail;
      return routinfo;
    }
  }
  osapiSemaGive(VrrpConfigSema);
  /* routinfo is L7_NULLPTR, release the lock*/
  return routinfo;
}


/*********************************************************************
* @purpose  To get the router id and intif num of the first vrrp
*           configuration from the list.
*
* @param    vrId  Virtual router id (out param)
* @param    intIfNum int interface number (out param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpFirstGet(L7_uchar8* vrId, L7_uint32* intIfNum)
{
  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  if( paramHead_g != L7_NULLPTR)
  {
    *vrId= paramHead_g->object->vrrpCfgInfo.vrid;
    *intIfNum = paramHead_g->object->vrrpVirtRouterOperInfo.intIfNum;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  osapiSemaGive(VrrpConfigSema);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the router id and intif num of the next vrrp
*           configuration from the list.
*
* @param    vrId  Virtual router id
* @param    intIfNum int interface number
* @param    nextVrId  Virtual router id of next config (out param)
* @param    nextInfNum int interface number of next config (out param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpNextGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                       L7_uchar8* nextVrId,
                       L7_uint32* nextInfNum)
{
  vrrpRouterInfo_t* nextConfig = L7_vrrpNextObjectGet(vrId,intIfNum);

  if( nextConfig != L7_NULLPTR)
  {
    *nextVrId = nextConfig->vrrpCfgInfo.vrid;
    *nextInfNum = nextConfig->vrrpVirtRouterOperInfo.intIfNum;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  else
  {
    *nextVrId = 0;
    *nextInfNum = 0;
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  To get the Vmac address associated with an IP
*
* @param    ipAddress  Ip address
* @param    vMac   Vmac address ( Out param)
* @param    vrId   VR Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  The function returns the vmac if router is in master state
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVMacGet(L7_uint32 ipAddress, L7_uchar8* vMac, L7_uchar8 *vrId)
{
  vrrpList_t* listTail;
  L7_uint32 count;
  L7_uint32 ipAddrCount;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  /* check if current points to the object to be fetch */
  if(paramCurrent_g != L7_NULLPTR)
  {
    if(paramCurrent_g->object->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_MASTER)
    {
      for (ipAddrCount = 0; ipAddrCount < L7_L3_NUM_IP_ADDRS; ipAddrCount++)
      {
        if (paramCurrent_g->object->vrrpCfgInfo.ipaddress[ipAddrCount] == ipAddress)
        {
          for(count = 0; count<L7_MAC_ADDR_LEN; count++)
          {
            vMac[count] = paramCurrent_g->object->vrrpVirtRouterOperInfo.vmac[count];
          }
          *vrId = paramCurrent_g->object->vrrpCfgInfo.vrid;
          osapiSemaGive(VrrpConfigSema);
          return L7_SUCCESS;
        }
      }
    }
  }

  listTail = paramHead_g;
  while( listTail!= L7_NULLPTR)
  {
    if (listTail->object->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_MASTER)
    {
      for (ipAddrCount = 0; ipAddrCount < L7_L3_NUM_IP_ADDRS; ipAddrCount++)
      {
        if (listTail->object->vrrpCfgInfo.ipaddress[ipAddrCount] == ipAddress)
        {
          paramCurrent_g = listTail;
          for(count = 0; count<L7_MAC_ADDR_LEN; count++)
          {
            vMac[count] = paramCurrent_g->object->vrrpVirtRouterOperInfo.vmac[count];
          }
          *vrId = paramCurrent_g->object->vrrpCfgInfo.vrid;
          osapiSemaGive(VrrpConfigSema);
          return L7_SUCCESS;
        }
      }
    }

    listTail = listTail->next;
  }
  osapiSemaGive(VrrpConfigSema);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the ip address associated with the VMAC.
*
* @param    vMac  Virtual Mac address
* @param    ipAddress  returned value of ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpIpGet (L7_uchar8* vMac,L7_uint32 *ipAddress)
{
  vrrpList_t* listTail;
  vrrpRouterInfo_t* routinfo = L7_NULLPTR;
  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

  if(paramCurrent_g != L7_NULLPTR)
  {
    routinfo = paramCurrent_g->object;
    if( memcmp(routinfo->vrrpVirtRouterOperInfo.vmac,vMac,L7_MAC_ADDR_LEN) ==0)
    {
      *ipAddress = routinfo->vrrpCfgInfo.ipaddress[0];
      osapiSemaGive(VrrpConfigSema);
      return L7_SUCCESS;
    }
  }
  listTail = paramHead_g;
  while( listTail!= L7_NULLPTR)
  {
    routinfo = listTail->object;
    if( memcmp(routinfo->vrrpVirtRouterOperInfo.vmac,vMac,L7_MAC_ADDR_LEN) ==0)
    {
      *ipAddress = routinfo->vrrpCfgInfo.ipaddress[0];
      paramCurrent_g = listTail;
      osapiSemaGive(VrrpConfigSema);
      return L7_SUCCESS;
    }

    listTail = listTail->next;
  }
  osapiSemaGive(VrrpConfigSema);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the total number of virtual router configuration
*
* @param    count
* @param    intIfNum int interface number
* @param    nextVrId  Virtual router id of next config (out param)
* @param    nextInfNum int interface number of next config (out param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpConfigCountGet(L7_uint32* count)
{
  *count = totalRouters_g;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Check if configuration is present corresponding to a
*           vrid and intnum
*
* @param    vrId  Virtual router id
* @param    intIfNum int interface number
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperEntryGet(L7_uchar8 vrId, L7_uint32 intIfNum)
{
  if( L7_NULLPTR != L7_vrrpParamsFetch (vrId,intIfNum))
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the logical next entry from the configuration list
*
* @param    vrId  Virtual router id
* @param    intIfNum int interface number
* @param    nextVrId  Virtual router id of next config (out param)
* @param    nextInfNum int interface number of next config (out param)
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperEntryNextGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                L7_uchar8* nextVrId,
                                L7_uint32* nextInfNum)
{
  vrrpList_t* listTail;
  vrrpRouterInfo_t* routinfo = L7_NULLPTR;
  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  while( listTail!= L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum<intIfNum )
  {
    listTail = listTail->next;
  }

  while (listTail != L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum== intIfNum
         && listTail->object->vrrpCfgInfo.vrid <= vrId )
  {
    listTail = listTail->next;
  }

  if(listTail!=L7_NULLPTR)
  {
    routinfo = listTail->object;
    paramCurrent_g = listTail;
    *nextVrId= routinfo->vrrpCfgInfo.vrid;
    *nextInfNum = routinfo->vrrpVirtRouterOperInfo.intIfNum;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  osapiSemaGive(VrrpConfigSema);
  *nextVrId= 0;
  *nextInfNum = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Set the state of router(Master/backup or initialised)
*
 * @param    routerID   Router Id.
* @param    intIfNum   Interface Number
* @param    state -     State of the router
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpStateSet(L7_uchar8 routerID,L7_uint32 intIfNum,
                        L7_vrrpState_t state)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    my_params->vrrpVirtRouterOperInfo.vr_state = state;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}



/*********************************************************************
* @purpose  To get the 0 priority advert rcvd flag
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    zeroFlg   zerp priority advert rcvd flag
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpZeroPriorityFlgGet(L7_uchar8 vrId,
                                  L7_uint32 intIfNum,
                                  L7_BOOL *zeroFlg)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (vrId,intIfNum)))
  {
    *zeroFlg = my_params->vrrpVirtRouterOperInfo.zerp_priority_pkt_rcvd_flag;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To set the 0 priority advert rcvd flag
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    zeroFlg   zerp priority advert rcvd flag
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpZeroPriorityFlgSet(L7_uchar8 vrId,
                                  L7_uint32 intIfNum,
                                  L7_BOOL zeroFlg)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (vrId,intIfNum)))
  {
    my_params->vrrpVirtRouterOperInfo.zerp_priority_pkt_rcvd_flag = zeroFlg;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the row status
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    rowStatus  row status
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperRowStatusGet(L7_uchar8  vrId,
                                L7_uint32  intIfNum,
                                L7_uint32* rowStatus)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (vrId,intIfNum)))
  {
    *rowStatus = (L7_uint32) my_params->vrrpVirtRouterOperInfo.oper_rowstatus;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the row status of Associate Ip address.
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    assocIpRowStat  returned value of ip row status
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAssocIpRowStatusGet(L7_uchar8 vrId,
                                   L7_uint32 intIfNum,
                                   L7_uint32* assocIpRowStat)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (vrId,intIfNum)))
  {
    *assocIpRowStat = (L7_uint32) my_params->vrrpVirtRouterOperInfo.assoc_ip_rowstatus;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Set the row status of Associate Ip address.
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    assocIpRowStat   ip row status value to be set
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAssocIpRowStatusSet(L7_uchar8 vrId,
                                   L7_uint32 intIfNum,
                                   L7_uint32 assocIpRowStat)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (vrId,intIfNum)))
  {
    my_params->vrrpVirtRouterOperInfo.assoc_ip_rowstatus =
        (L7_vrrpOperRowStatus_t)assocIpRowStat;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  To add a secondary IP address to the virtual router
*
* @param    routerID  - VRRP router ID
* @param    intIfNum  - Interface Number
* @param    ipAddress - Ip address
*
* @returns  L7_SUCCESS
*           L7_REQUEST_DENIED if virtual address is not in a local subnet
*                             on VR's interface
*           L7_ERROR  if there is no primary address on the VR
*           L7_TABLE_IS_FULL if the max number of secondaries is already configured
*           L7_FAILURE  if other error
*
* @notes    If ipAddress is already configured as a secondary address on this
*           virtual router, just return L7_SUCCESS.
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAssocSecondaryIpAddress(L7_uchar8 routerID,
                                       L7_uint32 intIfNum,
                                       L7_uint32 ipAddress)
{
    L7_uint32           i;

  /* First empty index in virtual address array. 0 is always used by primary
   * virtual address. 0 here indicates that an empty array index has
   * not yet been found. */
  L7_uint32 emptyIndex = 0;

    vrrpRouterInfo_t    *routerInfo;
    L7_BOOL  stopped = L7_FALSE;

  /* Verify that IP addr being assigned to VR is in same subnet as
   * an address on the local interface. check this before taking VRRP semaphore to
   * avoid deadlock potential. */
  if (!ipMapIpAddrIsLocal(intIfNum, ipAddress))
  {
    return L7_REQUEST_DENIED;
  }

  routerInfo = L7_vrrpParamsFetch(routerID, intIfNum);
    if (routerInfo == NULL)
    {
    return L7_FAILURE;
  }

  if (FD_VRRP_DEFAULT_IP_ADDRESS == routerInfo->vrrpCfgInfo.ipaddress[0])
  {
    /* Cannot configure a secondary unless a primary already exists */
    osapiSemaGive(VrrpConfigSema);
    return L7_ERROR;
  }

  /* Find place in address array to store new secondary */
  for (i = 1; i < L7_L3_NUM_IP_ADDRS; i++)
  {
    if (routerInfo->vrrpCfgInfo.ipaddress[i] == FD_VRRP_DEFAULT_IP_ADDRESS)
    {
      if (emptyIndex == 0)
      {
        emptyIndex = i;
      }
      continue;  /* to check for duplicate */
    }

    if (routerInfo->vrrpCfgInfo.ipaddress[i] == ipAddress)
    {
      /* If ipAddress is already configured as a secondary, return success */
      osapiSemaGive(VrrpConfigSema);
      return L7_SUCCESS;
    }
  }

  if (emptyIndex == 0)
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_TABLE_IS_FULL;
  }

  if (routerInfo->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
  {
    L7_vrrpRouterStop(routerInfo);
    stopped = L7_TRUE;
  }

  routerInfo->vrrpCfgInfo.ipaddress[emptyIndex] = ipAddress;
  routerInfo->vrrpVirtRouterOperInfo.ipAddressCount++;
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (stopped == L7_TRUE)
  {
    L7_vrrpRouterStart(routerInfo);
  }

  osapiSemaGive(VrrpConfigSema);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To remove a secondary IP address from the virtual router
*
* @param    routerID  - router Id
* @param    intIfNum  - Interface Number
* @param    ipAddress - Ip address
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpDissocSecondaryIpAddress(L7_uchar8 routerID,
                                        L7_uint32 intIfNum,
                                        L7_uint32 ipAddress)
{
    L7_RC_t             rc = L7_FAILURE;
    L7_uint32           i;
    vrrpRouterInfo_t    *routerInfo;
    L7_BOOL             stopped = L7_FALSE;

    if (L7_NULLPTR != (routerInfo = L7_vrrpParamsFetch (routerID, intIfNum)))
    {
        if (routerInfo->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
        {
            L7_vrrpRouterStop(routerInfo);
            stopped = L7_TRUE;
        }

        for (i = 1; i < L7_L3_NUM_IP_ADDRS; i++)
        {
            if (routerInfo->vrrpCfgInfo.ipaddress[i] == ipAddress)
            {
                routerInfo->vrrpCfgInfo.ipaddress[i] =
                    FD_VRRP_DEFAULT_IP_ADDRESS;
                routerInfo->vrrpVirtRouterOperInfo.ipAddressCount--;
                pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
                rc = L7_SUCCESS;
                break;
            }
        }

        if (stopped == L7_TRUE)
        {
            L7_vrrpRouterStart(routerInfo);
        }

        osapiSemaGive(VrrpConfigSema);
    }

    return rc;
}


/*********************************************************************
* @purpose  To get the next configured Virtual Router IP address
*
* @param    routerID  - router Id
* @param    intIfNum  - Interface Number
* @param    ipAddress  -Ip address
* @param    pIpAddress - Next Ip address
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Function returns primary address (First)- if 0 passed as
*           a
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpIpAddressNextGet(L7_uchar8 routerID, L7_uint32 intIfNum,
                                L7_uint32 ipAddress, L7_uint32 *pIpAddress)
{
    L7_RC_t             rc = L7_FAILURE;
    L7_uint32           i, j;
    vrrpRouterInfo_t    *routerInfo;

    /*
    Always return 0 in pIpAddress if nothing is found
    */

    *pIpAddress = FD_VRRP_DEFAULT_IP_ADDRESS;

    if (L7_NULLPTR != (routerInfo = L7_vrrpParamsFetch (routerID, intIfNum)))
    {
        if (ipAddress == FD_VRRP_DEFAULT_IP_ADDRESS)
        {
            if (routerInfo->vrrpCfgInfo.ipaddress[0] != FD_VRRP_DEFAULT_IP_ADDRESS)
            {
                *pIpAddress = routerInfo->vrrpCfgInfo.ipaddress[0];
                rc = L7_SUCCESS;
                osapiSemaGive(VrrpConfigSema);
                return rc;
            }
        }
        else
        {
            for (i = 0; i < L7_L3_NUM_IP_ADDRS - 1; i++)
            {
                if (routerInfo->vrrpCfgInfo.ipaddress[i] ==
                    FD_VRRP_DEFAULT_IP_ADDRESS)
                {
                    continue;
                }

                if (routerInfo->vrrpCfgInfo.ipaddress[i] == ipAddress)
                {
                    for (j = i+1; j < L7_L3_NUM_IP_ADDRS; j++)
                    {
                        if (routerInfo->vrrpCfgInfo.ipaddress[j] ==
                            FD_VRRP_DEFAULT_IP_ADDRESS)
                        {
                            continue;
                        }

                        *pIpAddress = routerInfo->vrrpCfgInfo.ipaddress[j];
                        rc = L7_SUCCESS;
                        break;
                    }

                    osapiSemaGive(VrrpConfigSema);
                    return rc;
                }
            }
        }

        osapiSemaGive(VrrpConfigSema);
    }

    return rc;
}

/*********************************************************************
* @purpose  Determine whether this router is the master or backup on a given
*           interface for any virtual router.
*
* @param    intIfNum @b{(input)} - Internal Interface Number
* @param    vrrpId @b{(output)} - VRRP vrid of virtual router for which
*              this router is master on interface intIfNum
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL L7_vrrpIsActive(L7_uint32 intIfNum, L7_uchar8 *vrrpId)
{
  L7_uchar8 vrid, nextVrid;
  L7_uint32 vIntIfNum, nextIntIfNum;
  L7_vrrpState_t vState;
  L7_RC_t rc;

  rc = L7_vrrpFirstGet(&vrid, &vIntIfNum);
  while (rc == L7_SUCCESS)
  {
      if (vIntIfNum == intIfNum)
      {
          if ((L7_vrrpStateGet(vrid, vIntIfNum, &vState) == L7_SUCCESS) &&
              (vState != L7_VRRP_STATE_INIT))
          {
              *vrrpId = vrid;
              return L7_TRUE;
          }
      }
      rc = L7_vrrpNextGet(vrid, vIntIfNum, &nextVrid, &nextIntIfNum);
      vrid = nextVrid;
      vIntIfNum = nextIntIfNum;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Checks to see if the interface is valid
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL L7_vrrpIntfIsValid(L7_uint32 intIfNum)
{
    L7_BOOL rv;
    L7_INTF_TYPES_t sysIntfType;

    if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    {
        return L7_FALSE;
    }

    if (sysIntfType == L7_LOOPBACK_INTF || sysIntfType == L7_TUNNEL_INTF)
    {
        return L7_FALSE;
    }

    rv = ipMapIsValidIntf(intIfNum);
    return rv;
}

/*********************************************************************
* @purpose  To get the next configured Assoc Virtual Router IP address
*
* @param    routerID  @b{(input)} - router Id
* @param    intIfNum  @b{(input)} - Interface Number
* @param    ipAddress @b{(input)} - Ip address
* @param    vrIdNext  @b{(output)} - Next Router Id
* @param    intIfNumNext @b{(output)} - Next Interface Number
* @param    ipAddressNext @b{(output)} - Next Ip Address
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Function returns primary address as well as secondary addresses
*
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpAssoIpAddrEntryNextGet (L7_uchar8 vrId,
                                       L7_uint32 intIfNum,
                                       L7_uint32 ipAddress,
                                       L7_uchar8 * vrIdNext,
                                       L7_uint32 * intIfNumNext,
                                       L7_uint32 * ipAddressNext)
{
  vrrpRouterInfo_t *routerInfo;
  L7_uint32 i, tmpIpAddr;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

  /* first try for exact match */
  routerInfo = L7_vrrpParamsFetchLocked (vrId, intIfNum, L7_FALSE);
  while (1)
  {
    if (routerInfo == L7_NULLPTR)
    {
      /* get the next entry */
      routerInfo = L7_vrrpParamsFetchLocked (vrId, intIfNum, L7_TRUE);
      if (routerInfo == L7_NULLPTR)
      {
        /* end of table */
        *vrIdNext = 0;
        *intIfNumNext = 0;
        *ipAddressNext = FD_VRRP_DEFAULT_IP_ADDRESS;
        osapiSemaGive (VrrpConfigSema);
        return L7_FAILURE;
      }
      ipAddress = 0;
    }

    /* get the minumum address greater than the asked for */
    tmpIpAddr = (L7_uint32) (-1);
    for (i = 0; i < routerInfo->vrrpVirtRouterOperInfo.ipAddressCount; i++)
    {
      if (routerInfo->vrrpCfgInfo.ipaddress[i] <= ipAddress)
        continue;
      if (routerInfo->vrrpCfgInfo.ipaddress[i] < tmpIpAddr)
        tmpIpAddr = routerInfo->vrrpCfgInfo.ipaddress[i];
    }

    if (tmpIpAddr != (L7_uint32) (-1))
      break;
    /* no address is just greater than the asked for */
    vrId = routerInfo->vrrpCfgInfo.vrid;
    intIfNum = routerInfo->vrrpVirtRouterOperInfo.intIfNum;
    routerInfo = L7_NULLPTR;
  }

  *vrIdNext = routerInfo->vrrpCfgInfo.vrid;
  *intIfNumNext = routerInfo->vrrpVirtRouterOperInfo.intIfNum;
  *ipAddressNext = tmpIpAddr;
  osapiSemaGive (VrrpConfigSema);
  return L7_SUCCESS;
}

/* starting of vrrp tracking api's */

/*********************************************************************
* @purpose  Add ip interface into the vrrp list of tracking interfaces.
*
* @param    vrId  @b{(input)} Router Id
* @param    intIfNum @b{(input)}  Interface Number
* @param    trackIntIfNum @b{(input)}  Interface Number to be tracked
* @param    prio_dec @b{(input)} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present and
*                       tracking interface table is not full
* @returns  L7_FAILURE  if no configuration found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackIntfAdd(L7_uchar8 vrId, L7_uint32 intIfNum,
                                L7_uint32 trackIntIfNum,
                                L7_uchar8 prio_dec )
{
   vrrpRouterInfo_t   *my_params;
   L7_BOOL entryFound = L7_FALSE;
   L7_int32 emptyPosition = -1;
   L7_uint32 i,ifState;
   L7_RC_t rc = L7_FAILURE;

   if ((prio_dec < L7_VRRP_INTF_PRIORITY_DEF) ||
       (prio_dec > L7_VRRP_INTF_PRIORITY_MAX-1 ))
   {
     return rc;
   }

   osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

   if( L7_NULLPTR != (my_params = L7_vrrpParamsFetchLocked (vrId,intIfNum, L7_FALSE)))
   {
     for (i = 0; i< L7_VRRP_MAX_TRACK_INTF_PER_VR;i++)
     {
       if (my_params->vrrpCfgInfo.vr_track_intf[i].intIfNum == trackIntIfNum)
       {
         if (my_params->vrrpCfgInfo.vr_track_intf[i].prio_dec == prio_dec)
         {
           entryFound = L7_TRUE;
         }
         emptyPosition = i; /* if the enrty already exist then update the priority decrement */
         break;
       }
       else if ((my_params->vrrpCfgInfo.vr_track_intf[i].intIfNum == 0)
           && (emptyPosition == -1))
       {
         emptyPosition = i;
       }
     } /* end of for loop */

     if ( entryFound == L7_TRUE)
     {
       rc = L7_SUCCESS;
     }
     else if (emptyPosition != -1)
     {
       if (my_params->vrrpCfgInfo.vr_track_intf[emptyPosition].intIfNum == 0)
       {
         if (ipMapRtrIntfOperModeGet(trackIntIfNum, &ifState) == L7_SUCCESS)
         {
           if (ifState == L7_ENABLE)
           {
             my_params->vrrpCfgInfo.vr_track_intf[emptyPosition].stateUp = L7_TRUE;
           }
         }
       }
       my_params->vrrpCfgInfo.vr_track_intf[emptyPosition].intIfNum = trackIntIfNum;
       my_params->vrrpCfgInfo.vr_track_intf[emptyPosition].prio_dec = prio_dec;

       if (my_params->vrrpCfgInfo.vr_track_intf[emptyPosition].stateUp != L7_TRUE)
       {
         if ( my_params->vrrpVirtRouterOperInfo.oper_priority != L7_VRRP_INTF_PRIORITY_MAX)
         {
           (void) vrrpOperationalPriorityCalc(my_params);
         }
         my_params->vrrpCfgInfo.vr_track_intf[emptyPosition].stateUp = L7_FALSE;
       } /* end if ip state */

       pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
       rc = L7_SUCCESS;
     } /* end of empty position */
     else
     {
       rc = L7_TABLE_IS_FULL;
     }
   } /* end of parms fetch */

   osapiSemaGive(VrrpConfigSema);
   return rc;

}

/*********************************************************************
* @purpose  Delete ip interface from vrrp list of tracking interfaces.
*
* @param    vrId  @b{(input)} Router Id
* @param    intIfNum @b{(input)}  Interface Number
* @param    trackIntIfNum @b{(input)}  Tracking Interface Number to be deleted
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackIntfDelete( L7_uchar8 vrId, L7_uint32 intIfNum,
                                    L7_uint32 trackIntIfNum)
{
  vrrpRouterInfo_t   *my_params;
  L7_BOOL entryFound = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetchLocked (vrId, intIfNum, L7_FALSE)))
  {
    for (i = 0; i< L7_VRRP_MAX_TRACK_INTF_PER_VR;i++)
    {
      if (my_params->vrrpCfgInfo.vr_track_intf[i].intIfNum == trackIntIfNum)
      {
        entryFound = L7_TRUE;
        my_params->vrrpCfgInfo.vr_track_intf[i].intIfNum = 0;
        if (my_params->vrrpCfgInfo.vr_track_intf[i].stateUp == L7_FALSE)
        {
           if (my_params->vrrpVirtRouterOperInfo.oper_priority != L7_VRRP_INTF_PRIORITY_MAX)
           {
              (void)vrrpOperationalPriorityCalc(my_params);
           }
        }
        my_params->vrrpCfgInfo.vr_track_intf[i].prio_dec = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
        my_params->vrrpCfgInfo.vr_track_intf[i].stateUp = L7_FALSE;
        pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
        break;
      }
    } /* end of for loop */
    if ( entryFound == L7_TRUE)
    {
      rc = L7_SUCCESS;
    }
  }

  osapiSemaGive(VrrpConfigSema);
  return rc;

}

/*********************************************************************
* @purpose  Add IP route to the vrrp list of routes VRRP is tracking.
*
* @param    vrId   @b{(input)} Router Id
* @param    intIfNum @b{(input)}  Interface Number
* @param    netAddr  @b{(input)}  Prefix of the Route
* @param    mask    @b{(input)}  Mask of the Route to be tracked
* @param    prio_dec @b{(input)} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
*           L7_FAILURE  if no configuration found
*           L7_ERROR if the prefix is not a valid unicast destination prefix
*           L7_TABLE_IS_FULL if there is not space in the configuration to track
*                            another route
*
* @notes    There are 2 cases:
*             1. This is a new prefix, not previously tracked
*             2. This route is already being tracked, but the priority decrement
*                is being changed.
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackRouteAdd(L7_uchar8 vrId, L7_uint32 intIfNum,
                                    L7_uint32 netAddr, L7_uint32 mask,
                                    L7_uchar8 prio_dec)
{
  vrrpRouterInfo_t   *my_params;

  /* Index of first unused element in configuration array */
  L7_int32 emptyPosition = -1;

  L7_uint32 i;
  L7_BOOL routeFound = L7_FALSE;
  L7_routeEntry_t routeEntry;

  memset((L7_char8 *)&routeEntry, 0, sizeof(L7_routeEntry_t));

  if ((prio_dec < L7_VRRP_TRACK_ROUTE_DECREMENT_MIN) ||
      (prio_dec > L7_VRRP_TRACK_ROUTE_DECREMENT_MAX))
  {
    return L7_FAILURE;
  }

  if (rtoValidDestAddr(netAddr, mask) != L7_TRUE)
  {
    return L7_ERROR;
  }

  /* Don't need to lookup route if just changing priority decrement, but
   * best to do this before we take VRRP semaphore. */
  if (rtoPrefixFind(netAddr, mask, &routeEntry) == L7_SUCCESS)
  {
    routeFound = L7_TRUE;
  }

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);
  my_params = L7_vrrpParamsFetchLocked(vrId, intIfNum, L7_FALSE);
  if (my_params == NULL)
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_FAILURE;
  }

  /* Found virtual router with matching VRID on given interface. */
  /* Look for exact match for new route track entry */
  for (i = 0; i < L7_VRRP_MAX_TRACK_RT_PER_VR; i++)
  {
    if (my_params->vrrpCfgInfo.vr_track_route[i].inUse)
    {
      if ((my_params->vrrpCfgInfo.vr_track_route[i].ipAddr == netAddr) &&
          (my_params->vrrpCfgInfo.vr_track_route[i].subnetMask == mask))
      {
        /* VRRP already tracking this route. */
        if (my_params->vrrpCfgInfo.vr_track_route[i].prio_dec == prio_dec)
        {
          /* route is already tracked with given priority decrement. Nothing to do. */
          osapiSemaGive(VrrpConfigSema);
          return L7_SUCCESS;
        }
        /* priority decrement has changed. No need to update reachability. */
        my_params->vrrpCfgInfo.vr_track_route[i].prio_dec = prio_dec;
        pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
        if (my_params->vrrpVirtRouterOperInfo.oper_priority != L7_VRRP_INTF_PRIORITY_MAX)
        {
          /* Update operational priority */
          (void) vrrpOperationalPriorityCalc(my_params);
        }
        osapiSemaGive(VrrpConfigSema);
        return L7_SUCCESS;
      }
    }
    else if (emptyPosition == -1)
    {
      emptyPosition = i;
    }
  }

  /* Prefix is not previously tracked. */
  if (emptyPosition == -1)
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_TABLE_IS_FULL;
  }

  /* Have space to track a new prefix. */
      my_params->vrrpCfgInfo.vr_track_route[emptyPosition].ipAddr = netAddr;
      my_params->vrrpCfgInfo.vr_track_route[emptyPosition].subnetMask = mask;
      my_params->vrrpCfgInfo.vr_track_route[emptyPosition].prio_dec = prio_dec;
  my_params->vrrpCfgInfo.vr_track_route[emptyPosition].inUse = L7_TRUE;
  if (routeFound)
  {
    my_params->vrrpCfgInfo.vr_track_route[emptyPosition].reachable = L7_TRUE;
  }
  vrrpOperationalPriorityCalc(my_params);
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete IP route from the vrrp list of tracking routes.
*
* @param    vrId @b{(input)}  Router Id.
* @param    intIfNum @b{(input)}  Interface Number
* @param    netAddr  @b{(input)}  Prefix of the Tracking Route
* @param    mask    @b{(input)}  Mask of the Tracking Route
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackRouteDelete(L7_uchar8 vrId, L7_uint32 intIfNum,
                                       L7_uint32 netAddr, L7_uint32 mask)
{
  vrrpRouterInfo_t *my_params;
  L7_uint32 i;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

  my_params = L7_vrrpParamsFetchLocked(vrId, intIfNum, L7_FALSE);
  if (my_params == NULL)
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_FAILURE;
  }
  for (i = 0; i < L7_VRRP_MAX_TRACK_RT_PER_VR; i++)
  {
    if (my_params->vrrpCfgInfo.vr_track_route[i].inUse &&
        (my_params->vrrpCfgInfo.vr_track_route[i].ipAddr == netAddr) &&
        (my_params->vrrpCfgInfo.vr_track_route[i].subnetMask == mask))
    {
      my_params->vrrpCfgInfo.vr_track_route[i].ipAddr = 0;
        my_params->vrrpCfgInfo.vr_track_route[i].subnetMask = 0;
        my_params->vrrpCfgInfo.vr_track_route[i].prio_dec = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
      my_params->vrrpCfgInfo.vr_track_route[i].inUse = L7_FALSE;
        if (my_params->vrrpCfgInfo.vr_track_route[i].reachable == L7_FALSE)
        {
          if (my_params->vrrpVirtRouterOperInfo.oper_priority != L7_VRRP_INTF_PRIORITY_MAX)
          {
            (void)vrrpOperationalPriorityCalc(my_params);
          }
        }
        my_params->vrrpCfgInfo.vr_track_route[i].reachable = L7_FALSE;
        pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
      osapiSemaGive(VrrpConfigSema);
      return L7_SUCCESS;
    }
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_NOT_EXIST;
}


/*********************************************************************
* @purpose  Get the tracking interface priority.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    trackIntIfNum @b{input}  Interface Number to be tracked
* @param    prio_dec @b{output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackIntfPrioGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                L7_uint32 trackIntIfNum, L7_uchar8 *prio_dec )
{
  vrrpRouterInfo_t   *my_params;
  L7_BOOL entryFound = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetchLocked (vrId,intIfNum, L7_FALSE)))
  {
    for (i = 0; i< L7_VRRP_MAX_TRACK_INTF_PER_VR;i++)
    {
      if (my_params->vrrpCfgInfo.vr_track_intf[i].intIfNum == trackIntIfNum)
      {
        entryFound = L7_TRUE;
        *prio_dec = my_params->vrrpCfgInfo.vr_track_intf[i].prio_dec;
        break;
      }
    }
    if ( entryFound == L7_TRUE)
    {
      rc = L7_SUCCESS;
    }
  }

  osapiSemaGive(VrrpConfigSema);
  return rc;

}

/*********************************************************************
* @purpose  Get the tracking interface state.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number.
* @param    trackIntIfNum @b{input}  Interface Number to be tracked.
* @param    state @b{output} IP state of the Tracked interface.
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackIntfStateGet(L7_uchar8 vrId, L7_uint32 intIfNum,
    L7_uint32 trackIntIfNum, L7_BOOL *state )
{
  vrrpRouterInfo_t   *my_params;
  L7_BOOL entryFound = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetchLocked (vrId,intIfNum, L7_FALSE)))
  {
    for (i = 0; i< L7_VRRP_MAX_TRACK_INTF_PER_VR;i++)
    {
      if (my_params->vrrpCfgInfo.vr_track_intf[i].intIfNum == trackIntIfNum)
      {
        entryFound = L7_TRUE;
        *state = my_params->vrrpCfgInfo.vr_track_intf[i].stateUp;
        break;
      }
    }
    if ( entryFound == L7_TRUE)
    {
      rc = L7_SUCCESS;
    }
  }

  osapiSemaGive(VrrpConfigSema);
  return rc;

}


/*********************************************************************
* @purpose  Get Next tracked interface.
*
* @param    vrId @b{input/output} Router Id.
* @param    intIfNum @b{inpu/outputt} Interface Number
* @param    trackIntIfNum @b{input/output}  Interface Number to be tracked
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackIntfNextGet(L7_uchar8 *vrId, L7_uint32 *intIfNum,
                                    L7_uint32 * trackIntIfNum)
{
  vrrpRouterInfo_t *routerInfo;
  L7_uint32 i, tmpTrackIntIf;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

  /* first try for exact match */
  routerInfo = L7_vrrpParamsFetchLocked (*vrId, *intIfNum, L7_FALSE);
  while (1)
  {
    if (routerInfo == L7_NULLPTR)
    {
      /* get the next entry */
      routerInfo = L7_vrrpParamsFetchLocked (*vrId, *intIfNum, L7_TRUE);
      if (routerInfo == L7_NULLPTR)
      {
        /* end of table */
        *vrId = 0;
        *intIfNum = 0;
        *trackIntIfNum = 0;
        osapiSemaGive (VrrpConfigSema);
        return L7_FAILURE;
      }
      *trackIntIfNum = 0;
    }

    /* get the minumum tracking interface greater than the asked for */
    tmpTrackIntIf = (L7_uint32) (-1);
    for (i = 0; i < L7_VRRP_MAX_TRACK_INTF_PER_VR ; i++)
    {
      if (routerInfo->vrrpCfgInfo.vr_track_intf[i].intIfNum <= *trackIntIfNum)
        continue;
      if (routerInfo->vrrpCfgInfo.vr_track_intf[i].intIfNum < tmpTrackIntIf)
      {
        tmpTrackIntIf = routerInfo->vrrpCfgInfo.vr_track_intf[i].intIfNum;
      }

    }

    if (tmpTrackIntIf != (L7_uint32) (-1))
      break;
    /* no trackIntIfNum is just greater than the asked for */
    *vrId = routerInfo->vrrpCfgInfo.vrid;
    *intIfNum = routerInfo->vrrpVirtRouterOperInfo.intIfNum;
    routerInfo = L7_NULLPTR;
  }

  *vrId = routerInfo->vrrpCfgInfo.vrid;
  *intIfNum = routerInfo->vrrpVirtRouterOperInfo.intIfNum;
  *trackIntIfNum = tmpTrackIntIf;
  osapiSemaGive (VrrpConfigSema);
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the tracked interface for the given vrid and interface.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    trackIntIfNum @b{input}  Interface Number to be tracked
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVrIdIntfTrackIntfGet(L7_uchar8 vrId, L7_uint32 intIfNum,
    L7_uint32  trackIntIfNum)
{
  vrrpRouterInfo_t   *my_params;
  L7_BOOL entryFound = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetchLocked (vrId,intIfNum, L7_FALSE)))
  {
    for (i = 0; i< L7_VRRP_MAX_TRACK_INTF_PER_VR;i++)
    {
      if (my_params->vrrpCfgInfo.vr_track_intf[i].intIfNum == trackIntIfNum)
      {
        entryFound = L7_TRUE;
        break;
      }
    }
    if ( entryFound == L7_TRUE)
    {
      rc = L7_SUCCESS;
    }
  }

 osapiSemaGive (VrrpConfigSema);
  return rc;

}


/*********************************************************************
* @purpose  Get Next tracked interface for the given vrid and interface.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    trackIntIfNum @b{input/output}  Interface Number to be tracked
* @param    priorityDecrement @b{input/output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVrIdIntfTrackIntfNextGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                       L7_uint32 * trackIntIfNum, L7_uchar8 * prio_dec )
{
  vrrpRouterInfo_t *routerInfo;
  L7_uint32 i, tmpTrackIntIf,tmpPrio;
  L7_RC_t rc = L7_FAILURE;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);

  tmpTrackIntIf = (L7_uint32) (-1);
  tmpPrio = 0;
  /* first try for exact match */
  routerInfo = L7_vrrpParamsFetchLocked (vrId, intIfNum, L7_FALSE);
  if (routerInfo == L7_NULLPTR)
  {
    *trackIntIfNum = 0;
    *prio_dec = 0;
  }
  else
  {
    /* get the minumum interface greater than the asked for */
    for (i = 0; i < L7_VRRP_MAX_TRACK_INTF_PER_VR ; i++)
    {
      if (routerInfo->vrrpCfgInfo.vr_track_intf[i].intIfNum != 0)
      {
        if (routerInfo->vrrpCfgInfo.vr_track_intf[i].intIfNum <= *trackIntIfNum)
        {
          continue;
        }
        if (routerInfo->vrrpCfgInfo.vr_track_intf[i].intIfNum < tmpTrackIntIf)
        {
          tmpTrackIntIf = routerInfo->vrrpCfgInfo.vr_track_intf[i].intIfNum;
          tmpPrio = routerInfo->vrrpCfgInfo.vr_track_intf[i].prio_dec;
        }
      }

    }
  }

 if (routerInfo != L7_NULLPTR)
 {
   if (tmpTrackIntIf != (L7_uint32) (-1))
   {
     *trackIntIfNum = tmpTrackIntIf;
     *prio_dec = tmpPrio;
     rc = L7_SUCCESS;
   }
 }
 osapiSemaGive (VrrpConfigSema);
 return rc;

}



/*********************************************************************
* @purpose  Get the tracked route priority.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    ipAddr @b{input}  ip address of tracked route.
* @param    netMask @b{input} subnet mask
* @param    prio_dec @b{output} priority to be decremented
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackRoutePrioGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                     L7_uint32 ipAddr, L7_uint32 netMask,
                                     L7_uchar8 *prio_dec)
{
  vrrpRouterInfo_t *my_params;
  L7_uint32 i;

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  my_params = L7_vrrpParamsFetchLocked(vrId, intIfNum, L7_FALSE);
  if (my_params == NULL)
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_FAILURE;
  }

  for (i = 0; i < L7_VRRP_MAX_TRACK_RT_PER_VR; i++)
  {
    if (my_params->vrrpCfgInfo.vr_track_route[i].inUse &&
        (my_params->vrrpCfgInfo.vr_track_route[i].ipAddr == ipAddr) &&
        (my_params->vrrpCfgInfo.vr_track_route[i].subnetMask == netMask))
    {
        *prio_dec = my_params->vrrpCfgInfo.vr_track_route[i].prio_dec;
      osapiSemaGive(VrrpConfigSema);
      return L7_SUCCESS;
    }
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the tracked route for given VRID and Interface number.
 *
 * @param    vrId     @b{input} VRRP router ID
 * @param    intIfNum @b{input} interface where virtual router is configured
 * @param    ipAddr   @b{input} destinatino prefix of tracked route
 * @param    netMask @b{input} subnet mask
 *
 * @returns  L7_SUCCESS  if router configuraion is present
 * @returns  L7_FAILURE  if prefix is not being tracked
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t L7_vrrpVrIdIntfTrackRouteGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                     L7_uint32 ipAddr, L7_uint32 netMask)
{
  vrrpRouterInfo_t   *my_params;
  L7_uint32 i;

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  my_params = L7_vrrpParamsFetchLocked (vrId,intIfNum, L7_FALSE);
  if (my_params == NULL)
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_FAILURE;
  }

  for (i = 0; i< L7_VRRP_MAX_TRACK_RT_PER_VR;i++)
  {
    if (my_params->vrrpCfgInfo.vr_track_route[i].inUse &&
        (my_params->vrrpCfgInfo.vr_track_route[i].ipAddr == ipAddr) &&
        (my_params->vrrpCfgInfo.vr_track_route[i].subnetMask == netMask))
    {
      osapiSemaGive(VrrpConfigSema);
      return L7_SUCCESS;
    }
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Determine whether a route that VRRP is tracking is currently in
*           the routing table.
*
* @param    vrId @b{input} Router Id.
* @param    intIfNum @b{input} Interface Number
* @param    ipAddr @b{input}  ip address of tracked route.
* @param    netMask @b{input} subnet mask
* @param    reachable @b{output} reachability of the tracked route.
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackRouteReachabilityGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                             L7_uint32 ipAddr,L7_uint32 netMask,
                                             L7_BOOL *reachable)
{
  vrrpRouterInfo_t *my_params;
  L7_uint32 i;

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  my_params = L7_vrrpParamsFetchLocked (vrId,intIfNum, L7_FALSE);
  if (my_params == NULL)
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_FAILURE;
  }

  for (i = 0; i < L7_VRRP_MAX_TRACK_RT_PER_VR; i++)
  {
    if (my_params->vrrpCfgInfo.vr_track_route[i].inUse &&
        (my_params->vrrpCfgInfo.vr_track_route[i].ipAddr == ipAddr) &&
        (my_params->vrrpCfgInfo.vr_track_route[i].subnetMask == netMask))
    {
      *reachable = my_params->vrrpCfgInfo.vr_track_route[i].reachable;
      osapiSemaGive(VrrpConfigSema);
      return L7_SUCCESS;
    }
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the first (lowest numeric prefix) tracked route for a given
*           VR on a given interface.
*
* @param    vrId     @b{input} VRRP router ID
* @param    intIfNum @b{input} interface where virtual router is configured
* @param    ipAddr   @b{output}  Destination prefix of first tracked route
* @param    netmask  @b{output}  Destination network mask of first tracked route
*
* @returns  L7_SUCCESS  if a tracked route is found
* @returns  L7_FAILURE  if no tracked routes on this VR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t vrrpVrTrackedRouteGetFirst(L7_uchar8 vrId, L7_uint32 intIfNum,
                                   L7_uint32 *ipAddr, L7_uint32 *netmask)
{
  L7_uchar8 prio_dec = 0;
  *ipAddr = 0;
  *netmask = 0;

  return L7_vrrpVridIntfTrackRouteNextGet(vrId, intIfNum, ipAddr, netmask, &prio_dec);
}

/*********************************************************************
* @purpose  Iterate through all tracked routes on all virtual routers on all
*           interfaces in get next order.
*
* @param    vrId     @b{input/output} VRRP router ID
* @param    intIfNum @b{inpu/outputt} interface where virtual router is configured
* @param    ipAddr   @b{input/output} IP address of the tracked route
* @param    mask     @b{input/output} network mask
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes    Notice that this function does not return the priority decrement
*           for each tracked route.
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperTrackRouteNextGet(L7_uchar8 *vrId, L7_uint32 *intIfNum,
                                     L7_uint32 *ipAddr, L7_uint32 *mask)
{
  vrrpRouterInfo_t *routerInfo;
  L7_uchar8 prio_dec = 0;

  /* Get the priority decrement of the previous tracked route */
  (void) L7_vrrpOperTrackRoutePrioGet(*vrId, *intIfNum, *ipAddr, *mask, &prio_dec);

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  routerInfo = L7_vrrpParamsFetchLocked(*vrId, *intIfNum, L7_FALSE);
  while (1)
  {
    if (routerInfo == L7_NULLPTR)
    {
      /* get the next VR */
      routerInfo = L7_vrrpParamsFetchLocked(*vrId, *intIfNum, L7_TRUE);
      *ipAddr = 0;
      *mask = 0;
      prio_dec = 0;
      if (routerInfo == L7_NULLPTR)
      {
        /* No more VRs */
        *vrId = 0;
        *intIfNum = 0;
        osapiSemaGive(VrrpConfigSema);
        return L7_FAILURE;
      }
      /* the fetch routine doesn't update vrid and intIfNum. So we have to
       * update the values here. */
      *vrId = routerInfo->vrrpCfgInfo.vrid;
      *intIfNum = routerInfo->vrrpVirtRouterOperInfo.intIfNum;

      /* For new VR, get first tracked route */
      osapiSemaGive(VrrpConfigSema);
      if (vrrpVrTrackedRouteGetFirst(*vrId, *intIfNum, ipAddr, mask) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
      else
      {
        /* No tracked routes on this VR. Continue search with next VR. */
        osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);
        routerInfo = NULL;
        continue;
      }
    }

    /* Look for next tracked route on same VR. */
    osapiSemaGive(VrrpConfigSema);
    if (L7_vrrpVridIntfTrackRouteNextGet(*vrId, *intIfNum, ipAddr, mask,
                                         &prio_dec) == L7_SUCCESS)
    {
      /* Found a next tracked route for this VR */
      return L7_SUCCESS;
    }
    osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

    /* no more tracked routes for this VR on this interface. Trigger
     * lookup of next VR. */
    routerInfo = L7_NULLPTR;
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  For a given virtual router on a given interface, get the
*           next route being tracked.
*
* @param    vrId     @b{input} VRRP router ID
* @param    intIfNum @b{input} interface where virtual router is configured
* @param    ipAddr   @b{input/output}  Destination prefix of the tracked route
* @param    mask     @b{input/output}  Destination network mask
* @param    prio_dec @b{input/output}  Amount priority is decremented when the
*                                      route is not in the routing table
*
* @returns  L7_SUCCESS  if a next entry is returned
* @returns  L7_FAILURE  if no next entry is found
*
* @notes    Assumes the priority decrement can never be configured to 0. To
*           get the first tracked route, set ipAddr, mask, and prio_dec to 0.
*
*           Also assumes that for a given ipAddr/mask, there can only be one
*           priority decrement value.
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpVridIntfTrackRouteNextGet(L7_uchar8 vrId, L7_uint32 intIfNum,
                                         L7_uint32 *ipAddr, L7_uint32 *mask,
                                         L7_uchar8 *prio_dec)
{
  vrrpRouterInfo_t *routerInfo;
  L7_uint32 i, tmpIpAddr,tmpMask,tmpPrio;
  L7_RC_t rc = L7_FAILURE;

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);
  tmpIpAddr = (L7_uint32) (-1);
  tmpPrio = L7_VRRP_TRACK_ROUTE_DECREMENT_MAX + 1;
  tmpMask = (L7_uint32) (-1);

  routerInfo = L7_vrrpParamsFetchLocked(vrId, intIfNum, L7_FALSE);
  if (routerInfo == L7_NULLPTR)
  {
    *ipAddr = 0;
    *mask = 0;
    *prio_dec = 0;
    osapiSemaGive(VrrpConfigSema);
    return L7_FAILURE;
  }

  /* get the tracked route following the one given */
  for (i = 0; i < L7_VRRP_MAX_TRACK_RT_PER_VR; i++)
  {
    if (routerInfo->vrrpCfgInfo.vr_track_route[i].inUse)
    {
      if (routerInfo->vrrpCfgInfo.vr_track_route[i].ipAddr < *ipAddr)
      {
        continue;
      }
      if ((routerInfo->vrrpCfgInfo.vr_track_route[i].ipAddr == *ipAddr) &&
          (routerInfo->vrrpCfgInfo.vr_track_route[i].subnetMask < *mask))
      {
        continue;
      }
      if ((routerInfo->vrrpCfgInfo.vr_track_route[i].ipAddr == *ipAddr) &&
          (routerInfo->vrrpCfgInfo.vr_track_route[i].subnetMask == *mask) &&
          (routerInfo->vrrpCfgInfo.vr_track_route[i].prio_dec <= *prio_dec))
      {
        continue;
      }

      /* This entry is greater than the input entry */
      if (routerInfo->vrrpCfgInfo.vr_track_route[i].ipAddr < tmpIpAddr)
      {
        tmpIpAddr = routerInfo->vrrpCfgInfo.vr_track_route[i].ipAddr;
        tmpMask = routerInfo->vrrpCfgInfo.vr_track_route[i].subnetMask;
        tmpPrio = routerInfo->vrrpCfgInfo.vr_track_route[i].prio_dec;
      }
      else if ((routerInfo->vrrpCfgInfo.vr_track_route[i].ipAddr == tmpIpAddr) &&
               (routerInfo->vrrpCfgInfo.vr_track_route[i].subnetMask < tmpMask))
      {
        tmpIpAddr = routerInfo->vrrpCfgInfo.vr_track_route[i].ipAddr;
        tmpMask  = routerInfo->vrrpCfgInfo.vr_track_route[i].subnetMask;
        tmpPrio = routerInfo->vrrpCfgInfo.vr_track_route[i].prio_dec;
      }
      else if ((routerInfo->vrrpCfgInfo.vr_track_route[i].ipAddr == tmpIpAddr) &&
               (routerInfo->vrrpCfgInfo.vr_track_route[i].subnetMask == tmpMask) &&
               (routerInfo->vrrpCfgInfo.vr_track_route[i].prio_dec < tmpPrio))
      {
        tmpIpAddr = routerInfo->vrrpCfgInfo.vr_track_route[i].ipAddr;
        tmpMask  = routerInfo->vrrpCfgInfo.vr_track_route[i].subnetMask;
        tmpPrio = routerInfo->vrrpCfgInfo.vr_track_route[i].prio_dec;
      }
    }
  } /* end for */

  if (tmpIpAddr != (L7_uint32) (-1))
  {
    *ipAddr = tmpIpAddr;
    *mask = tmpMask;
    *prio_dec = tmpPrio;
    rc = L7_SUCCESS;
  }

  osapiSemaGive (VrrpConfigSema);
  return rc;
}




/*********************************************************************
* @purpose  To Set the VRRP accept mode
*
* @param    routerID    @b{input} Router Id.
* @param    intIfNum    @b{input} Internal interface number.
* @param    accept_mode @b{input} When enabled VRRP master will accept
*                                 packets destined to VRIP and on
*                                 disabling VRRP master will discard
*                                 packets destined to VRIP.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAcceptModeSet(L7_uchar8 routerID,
                                 L7_uint32 intIfNum,
                                 L7_BOOL   accept_mode)
{
  vrrpRouterInfo_t *my_params;

  if (L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    my_params->vrrpCfgInfo.accept_mode = accept_mode;
    osapiSemaGive(VrrpConfigSema);
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get the VRRP accept mode
*
* @param    routerID    @b{input}  Router Id.
* @param    intIfNum    @b{input}  Internal interface number.
* @param    accept_mode @b{output} When enabled VRRP master will accept
*                                  packets destined to VRIP and on
*                                  disabling VRRP master will discard
*                                  packets destined to VRIP.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_vrrpOperAcceptModeGet(L7_uchar8 routerID,
                                 L7_uint32 intIfNum,
                                 L7_BOOL   *accept_mode)
{
  vrrpRouterInfo_t *my_params;

  if (L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *accept_mode = my_params->vrrpCfgInfo.accept_mode;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  To send the gratuitous ARP packets on those interfaces and
*           in those VR groups where the VRRP is in Master mode.
*
* @param    none
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void L7_vrrpMasterGratARPSend(void)
{
  L7_uchar8 vrID;
  L7_uint32 i, intIfNum, checkAvailability;
  vrrpRouterInfo_t* routerInfo ;

  checkAvailability = L7_vrrpFirstGet(&vrID, &intIfNum);

  while (checkAvailability == L7_SUCCESS)
  {
    osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

    if (L7_NULL != (routerInfo = (vrrpRouterInfo_t*)L7_vrrpVirtualRouterFetch(vrID, intIfNum)))
    {
      if (L7_VRRP_STATE_MASTER == routerInfo->vrrpVirtRouterOperInfo.vr_state)
      {
        for (i = 0; i < L7_L3_NUM_IP_ADDRS; i++)
        {
          if (routerInfo->vrrpCfgInfo.ipaddress[i] != FD_VRRP_DEFAULT_IP_ADDRESS)
          {
            vrrpGratuitousARPSend(routerInfo->vrrpVirtRouterOperInfo.vmac,
                                  routerInfo->vrrpCfgInfo.ipaddress[i],
                                  intIfNum);
          }
        }
      }
    }

    osapiSemaGive(VrrpConfigSema);

    checkAvailability = L7_vrrpNextGet(vrID, intIfNum, &vrID, &intIfNum);
  }
}

/*********************************************************************
* @purpose  To Set the description of the virtual router .
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    desc   Description string pointer
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperDescriptionSet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_uchar8* description)

{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    if(osapiStrncmp(my_params->vrrpCfgInfo.description, description, L7_VRRP_MAX_DESCRIPTION)!= 0)
    {
     osapiStrncpy(my_params->vrrpCfgInfo.description, description, L7_VRRP_MAX_DESCRIPTION);
     osapiSemaGive(VrrpConfigSema);
     pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the description of the virtual router .
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    desc   Description string pointer
*
* @returns  L7_SUCCESS or L7_FAILURE.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperDescriptionGet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_uchar8* description)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    strncpy(description, my_params->vrrpCfgInfo.description, L7_VRRP_MAX_DESCRIPTION);
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Set the timer leaning status of the virtual router .
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    mode       to enable/disable the timer leaning on the router
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperTimerLearnSet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_BOOL mode)

{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    if(my_params->vrrpCfgInfo.timer_learn != mode)
    {
     my_params->vrrpCfgInfo.timer_learn = mode;
     osapiSemaGive(VrrpConfigSema);
     pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Function to Get the timer learn state
*
* @param    routerID - router Id
* @param    intIfNum - Interface Number
* @param    mode     - pointer to timer learning status
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperTimerLearnGet(L7_uchar8 routerID,
                                  L7_uint32 intIfNum,
                                  L7_BOOL* mode)
{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *mode = my_params->vrrpCfgInfo.timer_learn;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  To Set a delay for the preemption of the virtual router .
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    delay      to set the delay interval for preemption
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperPreemptDelaySet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_ushort16 delay)

{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
   if(my_params->vrrpCfgInfo.preempt_delay != delay)
   {
    my_params->vrrpCfgInfo.preempt_delay = delay;
    osapiSemaGive(VrrpConfigSema);
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
   }
   return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the configured delay for the preemption.
*
* @param    vrId   Router Id.
* @param    intIfNum   Interface Number
* @param    preemptdelay  to get the delay interval for preemption
*
* @returns  L7_SUCCESS  if router configuraion is present
* @returns  L7_FAILURE  if no configuration found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t L7_vrrpOperPreemptDelayGet(L7_uchar8 routerID, L7_uint32 intIfNum,
                              L7_ushort16* preemptdelay)

{
  vrrpRouterInfo_t   *my_params;
  if( L7_NULLPTR != (my_params = L7_vrrpParamsFetch (routerID,intIfNum)))
  {
    *preemptdelay = my_params->vrrpCfgInfo.preempt_delay;
    osapiSemaGive(VrrpConfigSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

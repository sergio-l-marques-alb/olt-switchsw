/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dvmrp_map_api.c
*
* @purpose   DVMRP API functions
*
* @component DVMRP Mapping Layer
*
* @comments  none
*
* @create    02/18/2002
*
* @author    M Pavan K Chakravarthi
*
* @end
*
**********************************************************************/
#include "l7_common_l3.h"
#include "l3_mcast_commdefs.h"
#include "mcast_wrap.h"
#include "l7_dvmrp_api.h"                
#include "dvmrp_api.h"                   
#include "l7_ip_api.h"                   
#include "trap_layer3_mcast_api.h"
#include "l3_mcast_default_cnfgr.h"
#include "dvmrp_cnfgr.h" 
#include "dvmrp_config.h" 
#include "dvmrp_map_util.h" 
#include "dvmrp_vend_exten.h" 
#include "dvmrp_map_debug.h" 
#include "dvmrp_map.h" 
#include "l7_mcast_api.h" 
/*********************************************************************
* @purpose  Get the DVMRP administrative mode
*
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpAdminModeGet(L7_uint32 *mode)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  if (mode == L7_NULLPTR)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,"mode pointer is NULL\n");
    return L7_FAILURE;
  }

  /*Copy the admin mode */
  *mode = dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the DVMRP administrative mode
*
* @param    mode        @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpAdminModeSet(L7_uint32 mode)
{
  L7_uint32 currentMcastProtocol;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  /* this fails if DVMRP component not started by configurator */
  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Invalid mode %d specified\n",mode);
    return L7_FAILURE;
  }

  /* assumes current admin mode already established, 
   * so nothing more to do here                      
   */

  if (mode == dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode)
  {
    return L7_SUCCESS;
  }

  if (mode == L7_DISABLE)
  {
    if (dvmrpMapUIEventSend (DVMRPMAP_ADMIN_MODE_SET, mode, 0, 0)
        != L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                       "DVMRP Map Event Post Failed for eventType:%d.\n",
                       DVMRPMAP_ADMIN_MODE_SET);
    }
    dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode = mode;
    dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_TRUE;
    mcastMapIpCurrentMcastProtocolSet(L7_AF_INET, L7_MCAST_IANA_MROUTE_UNASSIGNED);
  }
  else
  {
    if ((mcastMapIpCurrentMcastProtocolGet(L7_AF_INET, 
                                           &currentMcastProtocol) == L7_SUCCESS) && 
        currentMcastProtocol == L7_MCAST_IANA_MROUTE_UNASSIGNED)
    {
      if (dvmrpMapMemoryInit(&dvmrpGblVar_g) != L7_SUCCESS)
      {
        DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                         "Another MCAST protocol might be enabled");
        return L7_FAILURE;              
      }
      dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode = mode;
      dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_TRUE;
      mcastMapIpCurrentMcastProtocolSet (L7_AF_INET, 
                                               L7_MCAST_IANA_MROUTE_DVMRP);
      
      if (dvmrpMapUIEventSend (DVMRPMAP_ADMIN_MODE_SET, mode, 0, 0)
          != L7_SUCCESS)
      {
        DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                         "DVMRP Map Event Post Failed for eventType:%d.\n",
                         DVMRPMAP_ADMIN_MODE_SET);
        /*Reset everything back to previous values*/
        dvmrpMapMemoryDeInit(&dvmrpGblVar_g);
        dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode = mode;
        dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_FALSE;
        mcastMapIpCurrentMcastProtocolSet (L7_AF_INET, 
                                                 L7_MCAST_IANA_MROUTE_UNASSIGNED);        
      }
    }
    else
    {
      /* Here L7_ERROR  return status is used by CLI to recognize the failure
         as "other MRP is configured" and other return status for representing 
         other failure states. The return status can be enhanced to capture
         various return states in multicast. */
      DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                       "Another MCast protocol is already enabled\n");
      return L7_ERROR;
    }
  }

  return L7_SUCCESS;
}

/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  INTERFACE ENTRY GET/NEXT
 *---------------------------------------------------------------------
 */

/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE STATS
 *---------------------------------------------------------------------
 */

/*write code to get  the two new statistical parameters for DVMRP.
 *1.Number of neighbors for a specific interface
 *2.Niegbor uptime for a give n nieghbor on a given interfaces
 */
/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Get the administrative mode of a DVMRP routing interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfAdminModeGet(L7_uint32 intIfNum, 
                                 L7_uint32 *mode)
{
  dvmrpCfgCkt_t  *pCfg;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "Interface %d is not ready for dvmrp configuration\n",intIfNum);
    return L7_ERROR;
  }
  *mode = pCfg->intfAdminMode;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the administrative mode of a DVMRP routing interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfAdminModeSet(L7_uint32 intIfNum, 
                                 L7_uint32 mode)
{
  L7_uint32 dvmrpAdminMode;
  dvmrpCfgCkt_t *pCfg;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  /* These pre-conditions has to be removed when the support is provided 
     for unnumbered interfaces for DVMRP */
  if (mcastIpMapIsIntfUnnumbered(L7_AF_INET,intIfNum) == L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "DVMRP cannot be set on unnumbered interface intIfNum(%d)", intIfNum);
    return L7_FAILURE;
  }

  /*return error if mode value is otherthan Disable/enable */
  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Invalid mode %d specified on Interface %d\n",mode,intIfNum);
    return L7_ERROR;
  }

  if (dvmrpMapIntfIsConfigurable(intIfNum, &pCfg) == L7_NULL)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Interface %d is not ready for dvmrp configuration\n",intIfNum);
    return L7_ERROR;
  }

  if (mode == pCfg->intfAdminMode)
  {
    return L7_SUCCESS;
  }

  pCfg->intfAdminMode = mode;
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_TRUE;

  if (mode == L7_DISABLE)
  {
    if (dvmrpMapUIEventSend (DVMRPMAP_INTF_MODE_SET, mode,
                             intIfNum, 0)
        != L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP Map Event Post Failed for eventType:%d.\n",
               DVMRPMAP_INTF_MODE_SET);
    }

  }

  if ((dvmrpMapDvmrpAdminModeGet(&dvmrpAdminMode)== L7_SUCCESS) && 
      (dvmrpAdminMode == L7_ENABLE) &&(mode == L7_ENABLE))
  {
    if (dvmrpMapUIEventSend (DVMRPMAP_INTF_MODE_SET, mode,
                             intIfNum, 0)
        != L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP Map Event Post Failed for eventType:%d.\n",
               DVMRPMAP_INTF_MODE_SET);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the DVMRP Vendor Version 
*
* @param    versionString    @b{(Output)} version string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapVersionStringGet(L7_uchar8 *versionString)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  osapiSnprintf(versionString,DVMRP_MAX_MSG_SIZE,"%d",DVMRP_MAJOR_VERSION);    

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table
*
* @param    numRoutes   @b{(output)} number of routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNumRoutesGet(L7_uint32 *numRoutes)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    *numRoutes = 0;
    return L7_SUCCESS;
  }
  return dvmrpMapExtenNumRoutesGet(numRoutes);
}


/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table with
*           non-infinite metric
*
* @param    reachable    @b{(output)} reachable routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapReachableRoutesGet(L7_uint32 *reachable)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    *reachable = 0;
    return L7_SUCCESS;
  }
  return dvmrpMapExtenReachableRoutesGet(reachable);
}

/*********************************************************************
* @purpose  Gets the IP address of the DVMRP interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipAddr      @b{(output)} Ip Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfLocalAddressGet(L7_uint32 intIfNum, 
                                    L7_inet_addr_t *ipAddr)
{
  L7_IP_MASK_t  myMask;
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     rtrIfNum = L7_NULL;
  L7_uint32     intfIpAddr = L7_NULL;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (inetAddressGet(L7_AF_INET, ipAddr, &intfIpAddr) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to get IPV4 address\n"); 
    return L7_FAILURE;
  }

  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
               Router Interface Number\n");
    return L7_FAILURE;
  }

  if ((rc = ipMapRtrIntfIpAddressGet(intIfNum, 
                                     &intfIpAddr, &myMask)) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Interface %d is not configured as router interface\n",intIfNum); 
    return L7_FAILURE;
  }
  if (inetAddressSet(L7_AF_INET, &intfIpAddr, ipAddr) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to get IPV4 address\n"); 
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets the DVMRP Generation ID used by the router
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    genId       @b{(output)}  Generation Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfGenerationIDGet(L7_uint32 intIfNum, 
                                    L7_ulong32 *genId)
{
  L7_uint32 rtrIfNum = L7_NULL;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapIntfIsOperational(intIfNum)!= L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for this interface\n"); 
    return L7_ERROR;
  }

  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
               Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenIntfGenerationIDGet(rtrIfNum, genId);
}

/*********************************************************************
* @purpose  Gets the DVMRP Interface Metric for the specified interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    metric      @b{(output)}  Metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfMetricGet(L7_uint32 intIfNum,
                              L7_ushort16 *metric)
{
  dvmrpCfgCkt_t  *pCfg;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  if (dvmrpMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Interface %d is not ready for dvmrp configuration\n",intIfNum);
    return L7_ERROR;
  }

  *metric = pCfg->intfMetric;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DVMRP Interface Metric for the specified interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    metric      @b{(input)}  Metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfMetricSet(L7_uint32 intIfNum, 
                              L7_ushort16 metric)
{
  dvmrpCfgCkt_t  *pCfg;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  if (dvmrpMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Interface %d is not ready for dvmrp configuration\n", intIfNum);
    return L7_ERROR;
  }

  pCfg->intfMetric = metric;
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_TRUE;

  if (dvmrpMapUIEventSend (DVMRPMAP_INTF_METRIC_SET, 0,
                           intIfNum, metric)
      != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP Map Event Post Failed for eventType:%d.\n",
             DVMRPMAP_INTF_METRIC_SET);
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the entry status.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    status      @b{(output)}  status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfStatusGet(L7_uint32 intIfNum, 
                              L7_int32 *status)
{
  L7_uint32 rtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  if (dvmrpMapIntfIsOperational(intIfNum) != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }
  return  dvmrpMapExtenIntfStatusGet(rtrIfNum,status);
}

/*********************************************************************
* @purpose  Determine if the DVMRP component has been initialized for
*           the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This is often used to determine whether a configuration
*           value can be applied or not.
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapIntfIsOperational(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  /* return value based on current DVMRP initialization state */
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
  {
    if (dvmrpGblVar_g.DvmrpIntfInfo[rtrIfNum].dvmrpEnabled == L7_TRUE)
    {
      return L7_TRUE;
    }
    else
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized "
                      "for %d interface\n", intIfNum); 
      return L7_FALSE;
    } 
  }

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
           Router Interface Number\n");
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Get the number of invalid packets received on this interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    badPkts    @b{(output)} Bad Packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfRcvBadPktsGet(L7_uint32 intIfNum, 
                                  L7_ulong32 *badPkts)
{
  L7_uint32 rtrIfNum = L7_NULL;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapIntfIsOperational(intIfNum) != L7_TRUE)
  {
    *badPkts = 0;
    return L7_SUCCESS;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenIntfRcvBadPktsGet(rtrIfNum, badPkts);
}

/*********************************************************************
* @purpose  Get the number of invalid routes received on this interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    badRts      @b{(output)} Bad Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfRcvBadRoutesGet(L7_uint32 intIfNum,
                                    L7_ulong32 *badRts)
{
  L7_uint32 rtrIfNum = L7_NULL;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapIntfIsOperational(intIfNum) != L7_TRUE)
  {
    *badRts = 0;
    return L7_SUCCESS;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenIntfRcvBadRoutesGet(rtrIfNum, badRts);
}

/*********************************************************************
* @purpose  Get the number of routes sent on this interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    sent        @b{(output)} Sent Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfSentRoutesGet(L7_uint32 intIfNum, 
                                  L7_ulong32 *sent)
{
  L7_BOOL dvmrpIntialized = L7_FALSE;
  L7_uint32 rtrIfNum = L7_NULL;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  dvmrpIntialized = dvmrpMapIntfIsOperational(intIfNum);
  if (dvmrpIntialized != L7_TRUE)
  {
    *sent = 0;
    return L7_SUCCESS;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenIntfSentRoutesGet(rtrIfNum, sent);
}

/*********************************************************************
* @purpose  Get an entry of Interfaces.
*
* @param    pIntIfNum         @b{(input)} interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfEntryGet(L7_uint32 *pIntIfNum)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIntfEntryGet(pIntIfNum);
}

/*********************************************************************
* @purpose  Get the next entry of interfaces.
*
* @param    pIntIfNum    @b{(inout)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfEntryNextGet(L7_uint32 *pIntIfNum)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIntfEntryNextGet(pIntIfNum);
}


/*********************************************************************
* @purpose  Get the DVMRP Uptime for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrUpTime    @b{(output)} Neighbor Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborUpTimeGet(L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr,
                                  L7_ulong32 *nbrUpTime)
{
  L7_uint32 rtrIfNum;
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  if (dvmrpMapIntfIsOperational(intIfNum) != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborUpTimeGet(rtrIfNum, nbrIpAddr, nbrUpTime);
}

/*********************************************************************
* @purpose  Get the DVMRP Expiry time for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrExpTime   @b{(output)} Neighbor Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborExpiryTimeGet(L7_uint32 intIfNum, 
                                      L7_inet_addr_t *nbrIpAddr, L7_ulong32 *nbrExpTime)
{
  L7_uint32 rtrIfNum;
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  if (dvmrpMapIntfIsOperational(intIfNum) != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n", intIfNum); 
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborExpiryTimeGet(rtrIfNum, 
                                            nbrIpAddr, nbrExpTime);
}


/*********************************************************************
* @purpose  Get the DVMRP Generation ID for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrGenId     @b{(output)} Neighbor Generation ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborGenIdGet(L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr,
                                 L7_ulong32 *nbrGenId)
{
  L7_uint32 rtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapIntfIsOperational(intIfNum) != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborGenIdGet(rtrIfNum, nbrIpAddr, nbrGenId);
}

/*********************************************************************
* @purpose  Get the DVMRP Major Version for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    majorVersion  @b{(output)} Neighbor Major Version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborMajorVersionGet(L7_uint32 intIfNum, 
                                        L7_inet_addr_t *nbrIpAddr, L7_ushort16 *majorVersion)
{
  L7_uint32 rtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapIntfIsOperational(intIfNum) != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborMajorVersionGet(rtrIfNum, 
                                              nbrIpAddr, majorVersion);
}


/*********************************************************************
* @purpose  Get the DVMRP Minor Version for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    minorVersion  @b{(output)} Neighbor Minor Version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborMinorVersionGet(L7_uint32 intIfNum, 
                                        L7_inet_addr_t *nbrIpAddr, L7_ushort16 *minorVersion)
{
  L7_BOOL  dvmrpIntialized = L7_FAILURE;
  L7_uint32 rtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  dvmrpIntialized = dvmrpMapIntfIsOperational(intIfNum);
  if (dvmrpIntialized != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborMinorVersionGet(rtrIfNum, 
                                              nbrIpAddr, minorVersion);

}


/*********************************************************************
* @purpose  Get the DVMRP capabilities for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    capabilities  @b{(output)} Neighbor Capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborCapabilitiesGet(L7_uint32 intIfNum,  
                                        L7_inet_addr_t *nbrIpAddr, L7_ushort16 *capabilities)
{
  L7_BOOL  dvmrpIntialized = L7_FAILURE;
  L7_uint32 rtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  dvmrpIntialized = dvmrpMapIntfIsOperational(intIfNum);
  if (dvmrpIntialized != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }

  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborCapabilitiesGet(rtrIfNum, 
                                              nbrIpAddr, capabilities);
}


/*********************************************************************
* @purpose  Get the number of routes received for the specified neighbour
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    rcvRoutes   @b{(output)} Received Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborRcvRoutesGet(L7_uint32 intIfNum, 
                                     L7_inet_addr_t *nbrIpAddr, L7_ulong32 *rcvRoutes)
{
  L7_BOOL  dvmrpIntialized = L7_FAILURE;
  L7_uint32 rtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  dvmrpIntialized = dvmrpMapIntfIsOperational(intIfNum);
  if (dvmrpIntialized != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborRcvRoutesGet(rtrIfNum, 
                                           nbrIpAddr, rcvRoutes);
}


/*********************************************************************
* @purpose  Get the number of invalid packets received for the specified 
*           neighbour of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    badPkts     @b{(output)} Bad Packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborRcvBadPktsGet(L7_uint32 intIfNum, 
                                      L7_inet_addr_t *nbrIpAddr, L7_ulong32 *badPkts)
{
  L7_BOOL  dvmrpIntialized = L7_FAILURE;
  L7_uint32 rtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  dvmrpIntialized = dvmrpMapIntfIsOperational(intIfNum);
  if (dvmrpIntialized != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborRcvBadPktsGet(rtrIfNum, 
                                            nbrIpAddr, badPkts);
}


/*********************************************************************
* @purpose  Get the number of invalid routes received for the 
*           specified neighbour of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    badRts      @b{(output)} Bad Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  dvmrpMapNeighborRcvBadRoutesGet(L7_uint32 intIfNum, 
                                         L7_inet_addr_t *nbrIpAddr, L7_ulong32 *badRts)
{
  L7_BOOL  dvmrpIntialized = L7_FAILURE;
  L7_uint32 rtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  dvmrpIntialized = dvmrpMapIntfIsOperational(intIfNum);
  if (dvmrpIntialized != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborRcvBadRoutesGet(rtrIfNum, 
                                              nbrIpAddr, badRts);
}


/*********************************************************************
* @purpose  Get the state for the specified neighbour
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    state       @b{(output)} state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborStateGet(L7_uint32 intIfNum, 
                                 L7_inet_addr_t *nbrIpAddr, L7_ulong32 *state)
{
  L7_BOOL  dvmrpIntialized = L7_FAILURE;
  L7_uint32 rtrIfNum = L7_NULL;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  dvmrpIntialized = dvmrpMapIntfIsOperational(intIfNum);
  if (dvmrpIntialized != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized for %d interface\n",intIfNum); 
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborStateGet(rtrIfNum, nbrIpAddr, state);
}

/*********************************************************************
* @purpose  Get if the entry of Neighbors is valid or not.
*
* @param    nbrIfIndex  @b{(input)} Neighbor Interface Index.
* @param    nbrAddr     @b{(output)} IP Address of the neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborGet(L7_uint32 nbrIfIndex, 
                            L7_inet_addr_t *nbrAddr)
{
  L7_uint32 rtrIfNum = L7_NULL;
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }
  if (ipMapIntIfNumToRtrIntf(nbrIfIndex, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborGet(rtrIfNum, nbrAddr);
}

/*********************************************************************
* @purpose  Get an entry of Neighbors.
*
* @param    pIntIfNum   @b{(output)} Internal Interface Number
* @param    nbrIpAddr   @b{(output)} IP Address of the neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborEntryGet(L7_uint32 *pIntIfNum, 
                                 L7_inet_addr_t *nbrIpAddr)
{
  L7_uint32 rtrIfNum = L7_NULL;
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (ipMapIntIfNumToRtrIntf(*pIntIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenNeighborEntryGet(&rtrIfNum, nbrIpAddr);
}

/*********************************************************************
* @purpose  Get the next entry on this interface.
*
* @param    pIntIfNum    @b{(output)} Internal Interface Number
* @param    nbrIpAddr    @b{(output)} IP Address of the neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborEntryNextGet(L7_uint32 *pIntIfNum, 
                                     L7_inet_addr_t *nbrIpAddr)
{
  L7_uint32 rtrIfNum = L7_NULL;
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  if (*pIntIfNum == 0)
  {
    rtrIfNum=L7_NULL;
    if (dvmrpMapExtenNeighborEntryNextGet(&rtrIfNum, nbrIpAddr) !=L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to get the next entry \n");
      return L7_FAILURE;
    }
  }
  else
  {
    if (ipMapIntIfNumToRtrIntf(*pIntIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
      return L7_FAILURE;
    }

    if (dvmrpMapExtenNeighborEntryNextGet(&rtrIfNum, nbrIpAddr) !=L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to get the next entry \n");
      return L7_FAILURE;
    }

  }
  if (ipMapRtrIntfToIntIfNum(rtrIfNum,pIntIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Router interface number to Internal Interface Number \n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the IP address of neighbor which is the source for
*           the packets for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    addr         @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteUpstreamNeighborGet(L7_inet_addr_t *srcIpAddr, 
                                         L7_inet_addr_t *srcMask, L7_inet_addr_t *addr)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }
  return dvmrpMapExtenRouteUpstreamNeighborGet(srcIpAddr, srcMask, addr);
}


/*********************************************************************
* @purpose  Gets the Interface index for a specific route
*           for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    IfIndex      @b{(output)} Interface Index which is passed back 
*                                    to the calling function
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  dvmrpMapRouteIfIndexGet(L7_inet_addr_t *srcIpAddr, 
                                 L7_inet_addr_t *srcMask, L7_uint32 *intIfNum)
{
  L7_uint32 rtrIfNum;
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }
  if (dvmrpMapExtenRouteIfIndexGet(srcIpAddr, srcMask, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Could not get the interface index\n");
    return L7_FAILURE;

  }

  if (rtrIfNum == 0)
  {
    *intIfNum = 0;
    return L7_SUCCESS;
  }

  if (ipMapRtrIntfToIntIfNum(rtrIfNum, intIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the distance in hops for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    metric       @b{(output)} metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteMetricGet(L7_inet_addr_t *srcIpAddr, 
                               L7_inet_addr_t *srcMask, L7_ushort16 *metric)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }
  return dvmrpMapExtenRouteMetricGet(srcIpAddr, srcMask, metric);
}


/*********************************************************************
* @purpose  Gets the route expiry time for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtExpTime    @b{(output)} route expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteExpiryTimeGet(L7_inet_addr_t *srcIpAddr, 
                                   L7_inet_addr_t *srcMask, L7_ulong32 *rtExpTime)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenRouteExpiryTimeGet(srcIpAddr, srcMask, rtExpTime);
}


/*********************************************************************
* @purpose  Gets the time when a route was learnt
*           for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtUpTime     @b{(output)} route up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteUptimeGet(L7_inet_addr_t *srcIpAddr, 
                               L7_inet_addr_t *srcMask, L7_ulong32 *rtUptime)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenRouteUptimeGet(srcIpAddr, srcMask, rtUptime);
}

/*********************************************************************
* @purpose  Get if the entry of Routes is valid or not.
*
* @param    routeSrc          @b{(input)} source IP Address
* @param    routeSrcMask      @b{(input)} source Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteGet(L7_inet_addr_t *routeSrc, 
                         L7_inet_addr_t *routeSrcMask)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenRouteGet(routeSrc, routeSrcMask);
}

/*********************************************************************
* @purpose  Get an entry of Routes.
*
* @param    srcIpAddr     @b{(input)} source IP Address
* @param    srcMask       @b{(input)} source Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteEntryGet(L7_inet_addr_t *srcIpAddr, 
                              L7_inet_addr_t *srcMask)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
    return L7_FAILURE;
  return dvmrpMapExtenRouteEntryGet(srcIpAddr, srcMask);
}

/*********************************************************************
* @purpose  Get the next entry of Routes.
*
* @param    srcIpAddr   @b{(inout)}  source IP Address
* @param    srcMask     @b{(inout)}  source Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteEntryNextGet(L7_inet_addr_t *srcIpAddr, 
                                  L7_inet_addr_t *srcMask)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
    return L7_FAILURE;
  return dvmrpMapExtenRouteEntryNextGet(srcIpAddr, srcMask);
}


/*********************************************************************
* @purpose  Gets the type of next hop router - leaf or branch,
*           for a specified source address and specified interface.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(input)} Next Hop Interface Index
* @param    nextHopType       @b{(output)} Next Hop Type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteNextHopTypeGet(L7_inet_addr_t *nextHopSrc,
                                    L7_inet_addr_t *nextHopSrcMask,
                                    L7_uint32 nextHopIfIndex,
                                    L7_uint32 *nextHopType)
{
  L7_uint32 nextHopRtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }

  if (ipMapIntIfNumToRtrIntf(nextHopIfIndex, &nextHopRtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenRouteNextHopTypeGet(nextHopSrc, 
                                          nextHopSrcMask, nextHopRtrIfNum, nextHopType);
}


/*********************************************************************
* @purpose  Get if the entry of next hops is valid or not.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(input)} Next Hop Interface Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t  dvmrpMapRouteNextHopGet(L7_inet_addr_t *nextHopSrc,
                                 L7_inet_addr_t *nextHopSrcMask,
                                 L7_uint32 nextHopIfIndex)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }
  return dvmrpMapExtenRouteNextHopGet(nextHopSrc, 
                                      nextHopSrcMask, nextHopIfIndex);
}

/*********************************************************************
* @purpose  Get an entry of next hops.
*
* @param    nextHopSrc        @b{(inout)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(inout)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(inout)} Next Hop Interface Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t  dvmrpMapRouteNextHopEntryGet(L7_inet_addr_t *nextHopSrc,
                                      L7_inet_addr_t *nextHopSrcMask,
                                      L7_uint32 *nextHopIfIndex)
{
  L7_uint32 rtrIfNum;
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  if (ipMapIntIfNumToRtrIntf(*nextHopIfIndex, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
               Router Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenRouteNextHopEntryGet(nextHopSrc, 
                                           nextHopSrcMask, &rtrIfNum);
}


/*********************************************************************
* @purpose  Get the next entry of nexthops.
*
* @param    nextHopSrc        @b{(inout)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(inout)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(inout)} Next Hop Interface Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteNextHopEntryNextGet(L7_inet_addr_t *nextHopSrc,
                                         L7_inet_addr_t *nextHopSrcMask,
                                         L7_uint32 *nextHopIfIndex)
{
  L7_uint32 nextHopRtrIfNum;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  if ((inetIsAddressZero (nextHopSrc) == L7_TRUE) &&
      (inetIsAddressZero (nextHopSrcMask) == L7_TRUE))
  {
    if (dvmrpMapExtenRouteNextHopEntryNextGet(nextHopSrc, 
                                              nextHopSrcMask, &nextHopRtrIfNum) != L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to get the next hop entry\n");
      return L7_FAILURE;
    }
  }
  else
  {
    if (ipMapIntIfNumToRtrIntf(*nextHopIfIndex, &nextHopRtrIfNum) != L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Internal Interface Number to \
                 Router Interface Number\n");
      return L7_FAILURE;
    }

    if (dvmrpMapExtenRouteNextHopEntryNextGet(nextHopSrc, 
                                              nextHopSrcMask, &nextHopRtrIfNum) != L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to get the next hop entry\n");
      return L7_FAILURE;
    }

  }

  if (ipMapRtrIntfToIntIfNum(nextHopRtrIfNum,nextHopIfIndex) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to convert Router interface number to Internal Interface Number \n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;


}


/*********************************************************************
* @purpose  Gets the prune expiry time for a group and
*           specified source address.
*
* @param    pruneGrp        @b{(input)} prune group
* @param    pruneSrc        @b{(input)} prune source
* @param    pruneSrcMask    @b{(input)} prune source mask
* @param    pruneExpTime    @b{(output)} prune expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapPruneExpiryTimeGet(L7_inet_addr_t *pruneGrp,
                                   L7_inet_addr_t *pruneSrc,
                                   L7_inet_addr_t *pruneSrcMask,
                                   L7_ulong32 *pruneExpTime)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP component not initialized\n");
    return L7_FAILURE;
  }
  return dvmrpMapExtenPruneExpiryTimeGet(pruneGrp, pruneSrc, 
                                         pruneSrcMask, pruneExpTime);
}

/*********************************************************************
* @purpose  Get an entry of prunes.
*
* @param    pruneGrp        @b{(input)} prune group
* @param    pruneSrc        @b{(input)} prune source
* @param    pruneSrcMask    @b{(input)} prune source mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapPruneEntryGet(L7_inet_addr_t *pruneGrp,
                              L7_inet_addr_t *pruneSrc,
                              L7_inet_addr_t *pruneSrcMask)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenPruneEntryGet(pruneGrp, pruneSrc, pruneSrcMask);
}


/*********************************************************************
* @purpose  Get the next entry of prunes.
*
* @param    pruneGrp        @b{(inout)} prune group
* @param    pruneSrc        @b{(inout)} prune source
* @param    pruneSrcMask    @b{(inout)} prune source mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapPruneEntryNextGet(L7_inet_addr_t *pruneGrp,
                                  L7_inet_addr_t *pruneSrc,
                                  L7_inet_addr_t *pruneSrcMask)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenPruneEntryNextGet(pruneGrp, 
                                        pruneSrc, pruneSrcMask);
}

/*********************************************************************
* @purpose Check whether dvmrp Is Operational.
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapDvmrpIsOperational ()
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpGblVar_g.DvmrpInfo.dvmrpEnabled;
}

/*********************************************************************
* @purpose  Get the Ip Mroute Flags.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    flags                 @b{(output)} Flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteFlagsGet(L7_inet_addr_t *ipMRouteGroup,
                                 L7_inet_addr_t *ipMRouteSource,
                                 L7_inet_addr_t *ipMRouteSourceMask,
                                 L7_uint32 *  flags)
{ 
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteFlagsGet(ipMRouteGroup, ipMRouteSource,
                                       ipMRouteSourceMask, flags);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rtAddr                @b{(output)} rtAddr 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteRtAddressGet(L7_inet_addr_t *ipMRouteGroup,
                                     L7_inet_addr_t *ipMRouteSource,
                                     L7_inet_addr_t  *ipMRouteSourceMask,
                                     L7_inet_addr_t*  rtAddr)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteRtAddrGet(ipMRouteGroup, ipMRouteSource,
                                        ipMRouteSourceMask, rtAddr);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rtMask                @b{(output)} rtMask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteRtMaskGet(L7_inet_addr_t *ipMRouteGroup,
                                  L7_inet_addr_t *ipMRouteSource,
                                  L7_inet_addr_t  *ipMRouteSourceMask,
                                  L7_inet_addr_t*  rtMask)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteRtMaskGet(ipMRouteGroup, ipMRouteSource,
                                        ipMRouteSourceMask, rtMask);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rtType                @b{(output)} rtType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteRtTypeGet(L7_inet_addr_t *ipMRouteGroup,
                                  L7_inet_addr_t *ipMRouteSource,
                                  L7_inet_addr_t  *ipMRouteSourceMask,
                                  L7_uint32 *rtType)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteRtTypeGet(ipMRouteGroup, ipMRouteSource,
                                        ipMRouteSourceMask, rtType);
}
/*********************************************************************
* @purpose  Get the Ip Mroute Expiry Time.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    expire                @b{(output)} expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteExpiryTimeGet(L7_inet_addr_t *ipMRouteGroup,
                                  L7_inet_addr_t *ipMRouteSource,
                                  L7_inet_addr_t  *ipMRouteSourceMask,
                                  L7_uint32 *expire)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteExpiryTimeGet(ipMRouteGroup, ipMRouteSource,
                                        ipMRouteSourceMask, expire);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Address.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rpfAddr               @b{(output)} rpfAddr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteRpfAddrGet(L7_inet_addr_t *ipMRouteGroup,
                                   L7_inet_addr_t *ipMRouteSource,
                                   L7_inet_addr_t  *ipMRouteSourceMask,
                                   L7_inet_addr_t *rpfAddr)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteRpfAddrGet(ipMRouteGroup, ipMRouteSource,
                                         ipMRouteSourceMask, rpfAddr);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count Get.
*
* @param    entryCount     @b{(output)} entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteEntryCountGet(L7_uint32 *entryCount)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteEntryCountGet(entryCount);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count Get.
*
* @param    heCount     @b{(output)} entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteHighestEntryCountGet(L7_uint32 *heCount)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteHighestEntryCountGet(heCount);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Entry
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteEntryGet(L7_inet_addr_t *ipMRouteGroup,
                                 L7_inet_addr_t *ipMRouteSource,
                                 L7_inet_addr_t  *ipMRouteSourceMask)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteEntryGet(ipMRouteGroup, ipMRouteSource,
                                       ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Entry Next
*
* @param    ipMRouteGroup         @b{(inout)} Group Address.
* @param    ipMRouteSource        @b{(inout)} Source Address.
* @param    ipMRouteSourceMask    @b{(inout)} Source Mask.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteEntryNextGet(L7_inet_addr_t *ipMRouteGroup,
                                     L7_inet_addr_t *ipMRouteSource,
                                     L7_inet_addr_t *ipMRouteSourceMask)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpMRouteEntryNextGet(ipMRouteGroup, ipMRouteSource,
                                           ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Get the next out interface
*
* @param    ipMRouteGroup        @b{(inout)}  Group Address.
* @param    ipMRouteSource       @b{(inout)}  Source Address.
* @param    ipMRouteSourceMask   @b{(inout)}  Source Mask.
* @param    outIntf              @b{(inout)}  out interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteOutIntfEntryNextGet(L7_inet_addr_t *ipMRouteGroup,
                                         L7_inet_addr_t *ipMRouteSource,
                                         L7_inet_addr_t *ipMRouteSourceMask,
                                         L7_uint32 *outIntf)
{
  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  return dvmrpMapExtenIpRouteOutIntfEntryNextGet(ipMRouteGroup,
                                                 ipMRouteSource,
                                                 ipMRouteSourceMask, outIntf);
}


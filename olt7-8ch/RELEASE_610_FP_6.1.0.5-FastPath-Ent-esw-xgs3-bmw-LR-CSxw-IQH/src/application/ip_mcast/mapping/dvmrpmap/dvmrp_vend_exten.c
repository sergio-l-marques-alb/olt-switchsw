/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dvmrp_vend_exten.c
*
* @purpose   DVMRP vendor-specific functions
*
* @component DVMRP Mapping Layer
*
* @comments  none
*
* @create    02/17/2002
*
* @author    M Pavan K Chakravarthi
*
* @end
*
**********************************************************************/
#include "l7_common.h"                
#include "l3_mcast_commdefs.h" 
#include "l7_ip_api.h"                    
#include "l3_mcast_default_cnfgr.h" 
#include "l7_dvmrp_api.h"             
#include "dvmrp_api.h"                 
#include "dvmrp_cnfgr.h" 
#include "dvmrp_config.h" 
#include "dvmrp_map_util.h" 
#include "dvmrp_vend_ctrl.h"
#include "dvmrp_map_debug.h" 
#include "mcast_wrap.h" 
#include "l7_mcast_api.h" 

/* private function prototypes */

static L7_RC_t dvmrpMapMetricSetEventSend(L7_uint32 rtrIfNum,
                                          L7_uint32 metric);

static L7_RC_t dvmrpAsyncActivateInterface(L7_uint32 rtrIfNum, 
                                           L7_uint32 mode, 
                                           L7_inet_addr_t *ipAddr, 
                                           L7_inet_addr_t *netMask);

/*
----------------------------------------------------------------------
                     SET FUNCTIONS  -  GLOBAL CONFIG
----------------------------------------------------------------------
*/

/*
----------------------------------------------------------------------
                     SET FUNCTIONS  -  INTERFACE CONFIG
----------------------------------------------------------------------
*/

/************************************************************************
* NOTE:  All intf config 'set' functions return with a successful return
*        code in the event the desired DVMRP interface structure is not
*        found.  This allows for pre-configuration of DVMRP routing
*        interfaces (the config gets picked up in dvmrp_ifinit() when the
*        interface structure is created).
*
* NOTE:  All intf config functions accept a pointer to a DVMRP vendor
*        interface structure, if known.  Otherwise, this p parm must be
*        set to L7_NULL.  The intIfNum parm is required in either case.
*        (This mechanism allows these same functions to be used both when
*        handling config requests from the user interface as well as when
*        the vendor DVMRP code asks for its interface config information.)
*************************************************************************/

/*********************************************************************
* @purpose  Set the DVMRP administrative mode for the router
*
* @param    mode        @b{(input)} Admin mode (L7_ENABLE or L7_DISABLE)
* @param    dvmrpDoInit @b{(input)} Flag to check whether DVMRP
*                                   Memory can be Initialized/
*                                   De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenDvmrpAdminModeSet(L7_uint32 mode,
                                       L7_BOOL dvmrpDoInit)
{
  L7_uint32 mcastMode=L7_DISABLE;
  L7_uint32 rtrIfNum;  
  
  if (mode == L7_ENABLE)
  {
    if (dvmrpGblVar_g.DvmrpInfo.dvmrpEnabled != L7_TRUE)
    {
      if(dvmrpDoInit == L7_TRUE)
      {
        /* Initialize the DVMRP Vendor Layer */
        if (dvmrpCnfgrInitPhase1DynamicProcess (&dvmrpGblVar_g) != L7_SUCCESS)
        {
          DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                           "Vendor Layer Dynamic Init Failed");
          return L7_FAILURE;
        }
      }

      if((ipMapRtrAdminModeGet() == L7_ENABLE) &&(mcastMapMcastAdminModeGet(&mcastMode) ==L7_SUCCESS)
        &&(mcastMode == L7_ENABLE))
      {
      /* Process DVMRP Global Admin Mode for ENABLE*/
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS, 
                      "\nDVMRP_GLOBAL_ADMIN_MODE_SET with the mode\n" 
                      "as ENABLED sent\n");
      if (dvmrp_global_adminmode_set(dvmrpGblVar_g.dvmrpCb, 
                                     mode) != L7_SUCCESS)
      {
        DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Could not Process Event.\n");
        if(dvmrpDoInit == L7_TRUE)
        {
        dvmrpCnfgrFiniPhase1DynamicProcess (&dvmrpGblVar_g);
        }
        return L7_FAILURE;
      }
      dvmrpGblVar_g.DvmrpInfo.dvmrpEnabled = L7_TRUE;

      }
    }
  }
  else
  {
    if (dvmrpGblVar_g.DvmrpInfo.dvmrpEnabled == L7_TRUE)
    {
      /* Process DVMRP Global Admin Mode for DISABLE*/
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,
                      "\nDVMRP_GLOBAL_ADMIN_MODE_SET with the mode\n" 
                      "as DISABLED sent\n");
      if (dvmrp_global_adminmode_set(dvmrpGblVar_g.dvmrpCb, 
                                     mode) != L7_SUCCESS)
      {
        DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Could not Process Event.\n");
        return L7_FAILURE;
      }
      dvmrpGblVar_g.DvmrpInfo.dvmrpEnabled = L7_FALSE;
      /*
         dvmrp_global_adminmode_set funtion disables all the interfaces in the vendor code.
         So in the mapping layer,all the interfaces have to be made operationally disabled.
       */
      for (rtrIfNum= 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
      {
        dvmrpGblVar_g.DvmrpIntfInfo[rtrIfNum].dvmrpEnabled = L7_FALSE;
    }

    }
      /* De-Initialize the DVMRP Vendor Layer */
      if (dvmrpDoInit == L7_TRUE)
      {
        if (dvmrpCnfgrFiniPhase1DynamicProcess (&dvmrpGblVar_g) != L7_SUCCESS)
        {
          DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                           "DVMRP Vendor Layer Dynamic De-Init Failed");
          return L7_FAILURE;
        }
      }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the DVMRP administrative mode for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} Admin mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfAdminModeSet(L7_uint32 rtrIfNum, L7_uint32 mode)
{
  L7_uint32        intIfNum = L7_NULL;
  L7_uint32        state = L7_NULL; 
  L7_inet_addr_t   ipAddr;
  L7_inet_addr_t   netMask;

  inetAddressZeroSet(dvmrpGblVar_g.family, &ipAddr);
  inetAddressZeroSet(dvmrpGblVar_g.family, &netMask);

  if (ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to convert router Interface Number "
                 "to Internal Interface Number\n");
    return L7_FAILURE;
  }

  if (mode == L7_ENABLE)
  {
    if ((nimGetIntfActiveState(intIfNum,&state) == L7_SUCCESS) &&
        state != L7_ACTIVE)
    {
      return L7_SUCCESS;
    }
  }

  if (mcastIpMapRtrIntfIpAddressGet(dvmrpGblVar_g.family, intIfNum, 
                                    &ipAddr) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to retrieve the primary IP Address associated "
                 "with the interface\n");
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfNetMaskGet(dvmrpGblVar_g.family, intIfNum, 
                                  &netMask) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to retrieve the primary IP Address associated "
                 "with the interface\n");
    return L7_FAILURE;
  }


  if (dvmrpAsyncActivateInterface(rtrIfNum, 
                                  mode, &ipAddr, &netMask) == L7_FAILURE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to send admin Mode Set event to DVMRP Task\n"); 
    if (mode != L7_ENABLE)
    {
      dvmrpGblVar_g.DvmrpIntfInfo[rtrIfNum].dvmrpEnabled = L7_FALSE;
    }
    return L7_FAILURE;
  }

  if (mode == L7_ENABLE)
  {
    dvmrpGblVar_g.DvmrpIntfInfo[rtrIfNum].dvmrpEnabled = L7_TRUE;
  }
  else
  {
    dvmrpGblVar_g.DvmrpIntfInfo[rtrIfNum].dvmrpEnabled = L7_FALSE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To send admin Mode Set event to DVMRP task.
*
* @param    rtrIfNum    @b{(input)} Router Interface Number
* @param    mode        @b{(input)} DVMRP vendor interface struct ptr 
*                                   (or L7_NULL)
* @param    ipAddr      @b{(input)} Ip Address.
* @param    netMask     @b{(input)} Net Mask.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t dvmrpAsyncActivateInterface(L7_uint32 rtrIfNum, 
                                           L7_uint32 mode, 
                                           L7_inet_addr_t *ipAddr, 
                                           L7_inet_addr_t *netMask)
{
  dvmrp_adminmode_t  adminMode;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS, "\nInterface Admin Mode.\n");

  adminMode.rtrIfNum = rtrIfNum;
  adminMode.mode =   mode;
  adminMode.ipAddr = ipAddr;
  adminMode.netMask = netMask;

  /*Process Interface Admin Mode Set to DVMRP Module */
  if (dvmrp_interface_adminmode_set (dvmrpGblVar_g.dvmrpCb, 
                                     &adminMode) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Could not Process Interface Admin Mode..\n"); 
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the metric used for default routes in DVMRP updates
*           originated on the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    metric      @b{(output)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfMetricSet(L7_uint32 rtrIfNum, L7_ushort16 metric)
{
  if (dvmrpMapMetricSetEventSend(rtrIfNum, metric) == L7_FAILURE)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to send Metric Set event to DVMRP task\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To send Metric Set event to DVMRP task.
*
* @param    rtrIfNum    @b{(input)} Router Interface Number
* @param    metric      @b{(input)} DVMRP vendor interface struct ptr 
                        (or L7_NULL)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t dvmrpMapMetricSetEventSend(L7_uint32 rtrIfNum, 
                                          L7_uint32 metric)
{
  dvmrp_metric_t  metricData;

  metricData.rtrIfNum = rtrIfNum;
  metricData.metric = metric;

  /* Process Metric Event to DVMRP Module */
  if (dvmrp_interface_metric_set(dvmrpGblVar_g.dvmrpCb, 
                                 &metricData) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Could not Process Event.\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*
----------------------------------------------------------------------
                     GET FUNCTIONS  -  GLOBAL
----------------------------------------------------------------------
*/


/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table
*
* @param    numRoutes    @b{(output)} number of routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNumRoutesGet(L7_uint32 *numRoutes)
{
  return dvmrpNumRoutesGet(numRoutes);
}

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table with
*           non-infinite metric
*
* @param    reachable    @b{(reachable)} reachable routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenReachableRoutesGet(L7_uint32 *reachable)
{
  return dvmrpReachableRoutesGet(reachable);
}

/*********************************************************************
* @purpose  Gets the DVMRP Generation ID used by the router
*
* @param    intIfNum @b{(input)} internal interface number
* @param    genId    @b{(output)} Generation ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfGenerationIDGet(L7_int32 rtrIfNum, L7_ulong32 *genId)
{
  return dvmrpIntfGenerationIDGet(rtrIfNum, genId);
}


/*********************************************************************
* @purpose  Get the entry status.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    status     @b{(output)} status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfStatusGet(L7_uint32 rtrIfNum, L7_int32 *status)
{
  return dvmrpIntfStatusGet(rtrIfNum, status);
}


/*********************************************************************
* @purpose  Get the number of invalid packets received on this interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    badPkts     @b{(output)} bad packets received
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfRcvBadPktsGet(L7_uint32 rtrIfNum, L7_ulong32 *badPkts)
{
  return dvmrpIntfRcvBadPktsGet(rtrIfNum, badPkts);
}


/*********************************************************************
* @purpose  Get the number of invalid routes received on this interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    badRts      @b{(output)} bad routes received
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfRcvBadRoutesGet(L7_uint32 rtrIfNum, L7_ulong32 *badRts)
{
  return dvmrpIntfRcvBadRoutesGet(rtrIfNum, badRts);
}


/*********************************************************************
* @purpose  Get the number of routes sent on this interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    sent      @b{(output)} sent routes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfSentRoutesGet(L7_uint32 rtrIfNum, L7_ulong32 *sent)
{
  return dvmrpIntfSentRoutesGet(rtrIfNum, sent);
}

/*********************************************************************
* @purpose  Get if the entry of Interfaces is valid or not.
*
* @param    rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfGet(L7_uint32 rtrIfNum)
{
  return dvmrpIntfGet(rtrIfNum);
}

/*********************************************************************
* @purpose  Get an entry of Interfaces.
*
* @param    pIntIfNum   @b{(output)}  DVMRP Interface table
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfEntryGet(L7_uint32 *pIntIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 mode;

  rc = dvmrpMapIntfAdminModeGet(*pIntIfNum, &mode);
  if ((rc == L7_SUCCESS) && (mode == L7_ENABLE))
  {
    return L7_SUCCESS;
  }
  DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to get the administrative mode of a DVMRP routing "
             "interface.\n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next entry on this interface.
*
* @param    pIntIfNum   @b{(inout)}  internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfEntryNextGet(L7_uint32 *pIntIfNum)
{
  L7_uint32 rtrIfNum;
  L7_uint32 tmpIntIfNum;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t   retCode = L7_FAILURE;


  for (tmpIntIfNum = *pIntIfNum+1; tmpIntIfNum < L7_MAX_INTERFACE_COUNT; tmpIntIfNum++)
  {
    if (ipMapIntIfNumToRtrIntf(tmpIntIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      retCode = dvmrpMapIntfAdminModeGet(tmpIntIfNum, &mode);
      if ((retCode == L7_SUCCESS) && (mode == L7_ENABLE))
      {
        *pIntIfNum = tmpIntIfNum;
        return L7_SUCCESS;
      }
    }
  }

  DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to get the administrative mode of a DVMRP routing "
           "interface.\n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the DVMRP Uptime for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrUpTime    @b{(output)} Neighbor Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborUpTimeGet(L7_uint32 rtrIfNum,
                                       L7_inet_addr_t *nbrIpAddr,
                                       L7_ulong32 *nbrUpTime)
{
  return dvmrpNeighborUpTimeGet(rtrIfNum, nbrIpAddr, nbrUpTime);
}

/*********************************************************************
* @purpose  Get the DVMRP Expiry time for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrExpTime   @b{(output)} Neighbor Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborExpiryTimeGet(L7_uint32 rtrIfNum,
                                           L7_inet_addr_t *nbrIpAddr,
                                           L7_ulong32 *nbrExpTime)
{
  return dvmrpNeighborExpiryTimeGet(rtrIfNum, nbrIpAddr, nbrExpTime);
}


/*********************************************************************
* @purpose  Get the DVMRP Generation ID for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrGenId     @b{(output)} Neighbor Generation ID
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborGenIdGet(L7_uint32 rtrIfNum,
                                      L7_inet_addr_t *nbrIpAddr,
                                      L7_ulong32 *nbrGenId)
{
  return dvmrpNeighborGenIdGet(rtrIfNum, nbrIpAddr, nbrGenId);
}


/*********************************************************************
* @purpose  Get the DVMRP Major Version for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    majorVersion  @b{(output)} Neighbor Major Version
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborMajorVersionGet(L7_uint32 rtrIfNum,
                                             L7_inet_addr_t *nbrIpAddr,
                                             L7_ushort16 *majorVersion)
{
  return dvmrpNeighborMajorVersionGet(rtrIfNum, nbrIpAddr, majorVersion);
}


/*********************************************************************
* @purpose  Get the DVMRP Minor Version for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)}  IP Address of the neighbour
* @param    minorVersion  @b{(output)} Neighbor Minor Version
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborMinorVersionGet(L7_uint32 rtrIfNum,
                                             L7_inet_addr_t *nbrIpAddr,
                                             L7_ushort16 *minorVersion)
{
  return dvmrpNeighborMinorVersionGet(rtrIfNum, nbrIpAddr, minorVersion);
}


/*********************************************************************
* @purpose  Get the DVMRP capabilities for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    capabilities  @b{(output)} Neighbor Capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborCapabilitiesGet(L7_uint32 rtrIfNum,
                                             L7_inet_addr_t *nbrIpAddr,
                                             L7_ushort16 *capabilities)
{
  return dvmrpNeighborCapabilitiesGet(rtrIfNum, nbrIpAddr, capabilities);
}


/*********************************************************************
* @purpose  Get the number of routes received for the specified neighbour
*           of the specified interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    rcvRoutes   @b{(output)} Received Routes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborRcvRoutesGet(L7_uint32 rtrIfNum,
                                          L7_inet_addr_t *nbrIpAddr,
                                          L7_ulong32 *rcvRoutes)
{
  return dvmrpNeighborRcvRoutesGet(rtrIfNum, nbrIpAddr, rcvRoutes);
}


/*********************************************************************
* @purpose  Get the number of invalid packets received for the specified 
*           neighbour of the specified interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    badPkts     @b{(output)} Bad Packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborRcvBadPktsGet(L7_uint32 rtrIfNum,
                                           L7_inet_addr_t *nbrIpAddr,
                                           L7_ulong32 *badPkts)
{
  return dvmrpNeighborRcvBadPktsGet(rtrIfNum, nbrIpAddr, badPkts);
}

/*********************************************************************
* @purpose  Get the number of invalid routes received for the specified 
*           neighbour of the specified interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    badRts      @b{(output)} Bad Routes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborRcvBadRoutesGet(L7_uint32 rtrIfNum,
                                             L7_inet_addr_t *nbrIpAddr,
                                             L7_ulong32 *badRts)
{
  return dvmrpNeighborRcvBadRoutesGet(rtrIfNum, nbrIpAddr, badRts);
}


/*********************************************************************
* @purpose  Get the state for the specified neighbour
*           of the specified interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    state       @b{(output)} state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborStateGet(L7_uint32 rtrIfNum,
                                      L7_inet_addr_t *nbrIpAddr,
                                      L7_ulong32 *state)
{
  return dvmrpNeighborStateGet(rtrIfNum, nbrIpAddr, state);
}

/*********************************************************************
* @purpose  Get if the entry of Neighbors is valid or not.
*
* @param    nbrIfIndex  @b{(input)}  neighbor interface index
* @param    nbrAddr     @b{(output)} neighbor address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborGet(L7_uint32 nbrIfIndex, L7_inet_addr_t *nbrAddr)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get an entry of Neighbors.
*
* @param    pRtrIfNum   @b{(input)} router interface number
* @param    nbrIpAddr   @b{(output)} neigbor address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborEntryGet(L7_uint32 *pRtrIfNum, 
                                      L7_inet_addr_t *nbrIpAddr)
{
  return dvmrpNeighborEntryGet(pRtrIfNum, nbrIpAddr);
}

/*********************************************************************
* @purpose  Get the next entry on this interface.
*
* @param    pRtrIfNum   @b{(input)} router interface number
* @param    nbrIpAddr    @b{(output)} neighbor address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborEntryNextGet(L7_uint32 *pRtrIfNum, 
                                          L7_inet_addr_t *nbrIpAddr)
{
  return dvmrpNeighborEntryNextGet(pRtrIfNum, nbrIpAddr);
}

/*********************************************************************
* @purpose  Gets the IP address of neighbor which is the source for
*           the packets for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    addr         @b{(output)} neighbor ip address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteUpstreamNeighborGet(L7_inet_addr_t *srcIpAddr,
                                              L7_inet_addr_t *srcMask,
                                              L7_inet_addr_t *addr)
{
  return dvmrpRouteUpstreamNeighborGet(srcIpAddr, srcMask, addr);
}


/*********************************************************************
* @purpose  Gets the Interface index for a specific route
*           for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source ip address
* @param    srcMask      @b{(input)} source Mask
* @param    rtrIfIndex      @b{(output)} rouetr Interface Index which is passed
*                                     back to the calling function
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteIfIndexGet(L7_inet_addr_t *srcIpAddr,
                                     L7_inet_addr_t *srcMask,
                                     L7_uint32 *rtrIfIndex)
{
  return dvmrpRouteIfIndexGet(srcIpAddr, srcMask, rtrIfIndex);
}

/*********************************************************************
* @purpose  Gets the distance in hops for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    metric       @b{(output)} metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteMetricGet(L7_inet_addr_t *srcIpAddr,
                                    L7_inet_addr_t *srcMask, 
                                    L7_ushort16 *metric)
{
  return dvmrpRouteMetricGet(srcIpAddr, srcMask, metric);
}


/*********************************************************************
* @purpose  Gets the route expiry time for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtExpTime    @b{(output)} route expiry time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteExpiryTimeGet(L7_inet_addr_t *srcIpAddr,
                                        L7_inet_addr_t *srcMask,
                                        L7_ulong32 *rtExpTime)
{
  return dvmrpRouteExpiryTimeGet(srcIpAddr, srcMask, rtExpTime);
}


/*********************************************************************
* @purpose  Gets the time when a route was learnt
*           for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtUpTime     @b{(output)} route up time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteUptimeGet(L7_inet_addr_t *srcIpAddr,
                                    L7_inet_addr_t *srcMask,
                                    L7_ulong32 *rtUpTime)
{
  return dvmrpRouteUptimeGet(srcIpAddr, srcMask, rtUpTime);
}

/*********************************************************************
* @purpose  Get if the entry of Routes is valid or not.
*
* @param    routeSrc    @b{(input)} route source
* @param    routeSrcMask    @b{(input)} route source mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteGet(L7_inet_addr_t *routeSrc,
                              L7_inet_addr_t *routeSrcMask)
{
  return L7_NOT_SUPPORTED;

}

/*********************************************************************
* @purpose  Get an entry of Routes.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteEntryGet(L7_inet_addr_t *srcIpAddr, 
                                   L7_inet_addr_t *srcMask)
{
  return dvmrpRouteEntryGet(srcIpAddr, srcMask);
}

/*********************************************************************
* @purpose  Get the next entry of Routes.
*
* @param    srcIpAddr    @b{(inout)} source IP Address
* @param    srcMask      @b{(inout)} source Mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteEntryNextGet(L7_inet_addr_t *srcIpAddr, 
                                       L7_inet_addr_t *srcMask)
{
  return dvmrpRouteEntryNextGet(srcIpAddr, srcMask);
}


/*********************************************************************
* @purpose  Gets the type of next hop router - leaf or branch,
*           for a specified source address and specified interface.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopRtrIfIndex    @b{(input)} Next Hop Interface Index
* @param    nextHopType       @b{(output)} Next Hop Type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteNextHopTypeGet(L7_inet_addr_t *nextHopSrc,
                                         L7_inet_addr_t *nextHopSrcMask,
                                         L7_uint32 nextHopRtrIfIndex,
                                         L7_uint32 *nextHopType)
{
  return dvmrpRouteNextHopTypeGet(nextHopSrc, nextHopSrcMask, 
                                  nextHopRtrIfIndex, nextHopType);
}

/*********************************************************************
* @purpose  Get if the entry of next hops is valid or not.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(input)} Next Hop Interface Index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteNextHopGet(L7_inet_addr_t *nextHopSrc,
                                     L7_inet_addr_t *nextHopSrcMask,
                                     L7_uint32 nextHopIfIndex)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get an entry of next hops.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(output)} Next Hop Interface Index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteNextHopEntryGet(L7_inet_addr_t *nextHopSrc,
                                          L7_inet_addr_t *nextHopSrcMask,
                                          L7_uint32 *nextHopIfIndex)
{
  return dvmrpRouteNextHopEntryGet(nextHopSrc, nextHopSrcMask, 
                                   nextHopIfIndex);
}

/*********************************************************************
* @purpose  Get the next entry of nexthops.
*
* @param    nextHopSrc        @b{(inout)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(inout)} Next Hop source Mask
* @param    nextHopRtrIfIndex    @b{(output)} Next Hop Interface Index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteNextHopEntryNextGet(L7_inet_addr_t *nextHopSrc,
                                              L7_inet_addr_t *nextHopSrcMask,
                                              L7_uint32 *nextHopRtrIfIndex)
{
  return dvmrpRouteNextHopEntryNextGet(nextHopSrc, nextHopSrcMask, 
                                       nextHopRtrIfIndex);
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
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenPruneExpiryTimeGet(L7_inet_addr_t *pruneGrp,
                                        L7_inet_addr_t *pruneSrc,
                                        L7_inet_addr_t *pruneSrcMask,
                                        L7_ulong32 *pruneExpTime)
{
  return dvmrpPruneExpiryTimeGet(pruneGrp, pruneSrc, 
                                 pruneSrcMask, pruneExpTime);
}

/*********************************************************************
* @purpose  Get an entry of prunes.
*
* @param    pruneGrp        @b{(input)} prune group
* @param    pruneSrc        @b{(input)} prune source
* @param    pruneSrcMask    @b{(input)} prune source mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenPruneEntryGet(L7_inet_addr_t *pruneGrp,
                                   L7_inet_addr_t *pruneSrc,
                                   L7_inet_addr_t *pruneSrcMask)
{
  return dvmrpPruneEntryGet(pruneGrp, pruneSrc, pruneSrcMask);
}


/*********************************************************************
* @purpose  Get the next entry of prunes.
*
* @param    pruneGrp        @b{(inout)} prune group
* @param    pruneSrc        @b{(inout)} prune source
* @param    pruneSrcMask    @b{(inout)} prune source mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenPruneEntryNextGet(L7_inet_addr_t *pruneGrp,
                                       L7_inet_addr_t *pruneSrc,
                                       L7_inet_addr_t *pruneSrcMask)
{
  return dvmrpPruneEntryNextGet(pruneGrp, pruneSrc, pruneSrcMask);
}

/*********************************************************************
* @purpose  Get the next entry of nbr on intIfNum.
*
* @param    pIntIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr    @b{(output)} Nbr IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfNbrEntryNextGet(L7_uint32 *pIntIfNum, 
                                         L7_inet_addr_t *nbrIpAddr)
{
  return L7_NOT_SUPPORTED;

}

/*********************************************************************
* @purpose  Chech whether router interface running DVMRP 'intIfNum' is a leaf
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenDvmrpIsInterfaceLeaf(L7_uint32 intIfNum)
{
  return dvmrpIsInterfaceLeaf(intIfNum);
}

/*********************************************************************
* @purpose  Chech whether DVMRP is a leaf router
*
* @param    void
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dvmrpMapExtenDvmrpRouterIsLeaf(void)
{
  return dvmrpRouterIsLeaf();
}

/*********************************************************************
* @purpose  Get the Ip Mroute Flags.
*
* @param    ipMRouteGroup     @b{(input)} Group Address.
* @param    ipMRouteSource    @b{(input)} Source Address.
* @param    ipMRouteMask      @b{(input)} Mask Address.
* @param    flags             @b{(output)} Flags
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteFlagsGet(L7_inet_addr_t *ipMRouteGroup,
                                      L7_inet_addr_t *ipMRouteSource,
                                      L7_inet_addr_t  *ipMRouteSourceMask,
                                      L7_uint32*  flags)
{
  return dvmrpIpMRouteFlagsGet(ipMRouteGroup, 
                               ipMRouteSource, ipMRouteSourceMask, flags);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rpfAddr               @b{(output)} rpfAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteRtAddrGet(L7_inet_addr_t *ipMRouteGroup,
                                       L7_inet_addr_t *ipMRouteSource,
                                       L7_inet_addr_t  *ipMRouteSourceMask,
                                       L7_inet_addr_t*  rpfAddr)
{
  return dvmrpIpMRouteRtAddrGet(ipMRouteGroup, 
                                ipMRouteSource, ipMRouteSourceMask, rpfAddr);
}
/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rtMask                @b{(output)} rtMask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteRtMaskGet(L7_inet_addr_t *ipMRouteGroup,
                                       L7_inet_addr_t *ipMRouteSource,
                                       L7_inet_addr_t  *ipMRouteSourceMask,
                                       L7_inet_addr_t*  rtMask)
{
  return dvmrpIpMRouteRtMaskGet(ipMRouteGroup, 
                                ipMRouteSource, ipMRouteSourceMask, rtMask);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rtType                @b{(output)} rtType
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteRtTypeGet(L7_inet_addr_t *ipMRouteGroup,
                                       L7_inet_addr_t *ipMRouteSource,
                                       L7_inet_addr_t  *ipMRouteSourceMask,
                                       L7_uint32 *rtType)
{
  return dvmrpIpMRouteRtTypeGet(ipMRouteGroup, 
                                ipMRouteSource, ipMRouteSourceMask, rtType);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Addr.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rpfAddr               @b{(output)} rpfAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteRpfAddrGet(L7_inet_addr_t *ipMRouteGroup,
                                        L7_inet_addr_t *ipMRouteSource,
                                        L7_inet_addr_t  *ipMRouteSourceMask,
                                        L7_inet_addr_t *rpfAddr)
{
  return dvmrpIpMRouteRpfAddrGet(ipMRouteGroup, 
                                 ipMRouteSource, ipMRouteSourceMask, rpfAddr);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count.
*
* @param    entryCount      @b{(output)}  rpfAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteEntryCountGet(L7_uint32 *entryCount)
{

  return  dvmrpIpMRouteEntryCountGet(entryCount); 

}

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count.
*
* @param    heCount     @b{(output)}  highest Entry Count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteHighestEntryCountGet(L7_uint32 *heCount)
{
  return L7_NOT_SUPPORTED;

}

/*********************************************************************
* @purpose  Get the Ip Mroute Entry.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteEntryGet(L7_inet_addr_t *ipMRouteGroup,
                                      L7_inet_addr_t *ipMRouteSource,
                                      L7_inet_addr_t  *ipMRouteSourceMask)
{
  return dvmrpIpMRouteEntryGet(ipMRouteGroup,
                               ipMRouteSource, ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Entry.
*
* @param    ipMRouteGroup         @b{(inout)} Group Address.
* @param    ipMRouteSource        @b{(inout)} Source Address.
* @param    ipMRouteSourceMask    @b{(inout)} Source Mask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteEntryNextGet(L7_inet_addr_t *ipMRouteGroup,
                                          L7_inet_addr_t *ipMRouteSource,
                                          L7_inet_addr_t  *ipMRouteSourceMask)
{
  return dvmrpIpMRouteEntryNextGet(ipMRouteGroup,
                                   ipMRouteSource, ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Get the next mroute entry out interface
*
* @param    ipMRouteGroup         @b{(inout)} Group Address.
* @param    ipMRouteSource        @b{(inout)} Source Address.
* @param    ipMRouteSourceMask    @b{(inout)} Source Mask.
* @param    outIfNum              @b{(inout)} interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpRouteOutIntfEntryNextGet(L7_inet_addr_t *ipMRouteGroup,
                                                L7_inet_addr_t *ipMRouteSource,
                                                L7_inet_addr_t  *ipMRouteSourceMask,
                                                L7_uint32 *outIfNum)
{
  return dvmrpRouteOutIntfEntryNextGet(ipMRouteGroup,
                                       ipMRouteSource, ipMRouteSourceMask, outIfNum);
}

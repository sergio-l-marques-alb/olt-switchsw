/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dvmrp_map_util.c
*
* @purpose   DVMRP Utility Functions
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
#include "l7_common_l3.h"
#include "l3_mcast_commdefs.h"
#include "mcast_inet.h"
#include "mcast_wrap.h"
#include "l7_dvmrp_api.h"
#include "dvmrp_api.h"
#include "l7_ip_api.h"
#include "l3_mcast_default_cnfgr.h"
#include "dvmrp_cnfgr.h"
#include "dvmrp_config.h"
#include "dvmrp_map_util.h"
#include "dvmrp_vend_exten.h"
#include "dvmrp_map_debug.h"
#include "l7_mcast_api.h"
/*********************************************************************
* @purpose  Enable the DVMRP Routing Function
*
* @param    dvmrpDoInit      @b{(input)} Flag to check whether DVMRP
*                                        Memory can be Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpAdminModeEnable(L7_BOOL dvmrpDoInit)
{
  L7_uint32 i, intIfNum;
  L7_uint32 mode;
  L7_RC_t rc = L7_SUCCESS;

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    return L7_SUCCESS;
  }
  if (dvmrpMapExtenDvmrpAdminModeSet(L7_ENABLE, dvmrpDoInit) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to set the multicast protocal running on the router.\n");
    return L7_FAILURE;
  }

  if (DVMRP_IS_READY)
  {
    for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
    {
      /* NOTE:  This call fails if router intf not created yet */
      if (ipMapRtrIntfToIntIfNum(i, &intIfNum) == L7_SUCCESS)
      {
        if ((dvmrpMapIntfAdminModeGet(intIfNum, &mode) == L7_SUCCESS) &&
            mode == L7_ENABLE)
        {
          if (dvmrpMapIntfAdminModeApply(intIfNum, L7_ENABLE) != L7_SUCCESS)
          {
            DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                            "\nCould not enable DVMRP interface on %d\n", i);
          }
        }
      }
    }
  }

  return rc;
}


/*********************************************************************
* @purpose  Disable the DVMRP Routing Function
*
* @param    dvmrpDoInit      @b{(input)} Flag to check whether DVMRP
*                                        Memory can be De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpAdminModeDisable(L7_BOOL dvmrpDoInit)
{
  L7_uint32 rtrIfNum, intIfNum;
  dvmrpCfgCkt_t *pCfg;
  L7_RC_t rc = L7_SUCCESS;

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {

  /*-------------------------------------------------------------------------*/
  /* Disable router interfaces configured for DVMRP */
  /*-------------------------------------------------------------------------*/
  if (DVMRP_IS_READY)
  {
    for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
    {
      if (ipMapRtrIntfToIntIfNum(rtrIfNum,&intIfNum) != L7_SUCCESS)
        continue;

      if ((dvmrpMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) &&
          (pCfg->intfAdminMode == L7_ENABLE) &&
          (dvmrpMapIntfIsOperational(intIfNum) == L7_TRUE))
      {
        if (dvmrpMapIntfAdminModeApply(intIfNum, L7_DISABLE) != L7_SUCCESS)
        {
          DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                          "\nCould not disable DVMRP on interface %d\n", intIfNum);
        }
      }
    }
  }

  }

  /* apply the DVMRP configuration change */
  if (dvmrpMapExtenDvmrpAdminModeSet(L7_DISABLE, dvmrpDoInit) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to set the DVMRP administrative mode for the router\n");
    rc = L7_FAILURE;
  }
  return rc;
}


/*---------------------------------------------------------------------
 *                 SUPPORT FUNCTIONS  -  INTERFACE CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Set the DVMRP admin mode for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} Administrative mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes:
*           1. DVMRP is enabled
*           2. Interface has been created and configured for DVMRP at
*              the DVMRP vendor layer
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfAdminModeApply(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_uint32 rtrIfNum;
  L7_uint32 ipAddr, ipMask;
  L7_uint32 mcastMode = L7_DISABLE ;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* These pre-conditions has to be removed when the support is provided
     for unnumbered interfaces for DVMRP */
  if (mcastIpMapIntIfNumToRtrIntf(L7_AF_INET,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "\n Failed to convert to rtrIfNum for intIfNum (%d)", intIfNum);
    return L7_FAILURE;
  }
  if (mcastIpMapIsRtrIntfUnnumbered(L7_AF_INET, rtrIfNum) == L7_TRUE)
  {
   DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS, "\n The rtrIfNum(%d) is unnumbered interface", rtrIfNum);
    return L7_SUCCESS;
  }

  if (mode == L7_DISABLE)
  {
    /* rel K fix */
    if (dvmrpMapIntfIsOperational(intIfNum) == L7_FALSE)
    {
      return L7_SUCCESS;
    }

    /* delete DVMRP router multicast address from NPU */
    if (dtlIpv4LocalMulticastAddrDelete(intIfNum, L7_IP_ALL_DVMRP_RTRS_ADDR)
        != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
              "DVMRP All Routers Address - %x Delete from the DTL Mcast List "
              "Failed for intf - %s"
              " DMVRP All Routers Address deletion from the local multicast list Failed."
              " As a result of this, DVMRP Multicast packets are still received at the"
              " application though DVMRP is disabled.", L7_IP_ALL_DVMRP_RTRS_ADDR, ifName);
      /*return L7_FAILURE;*/
    }

    if (ipMapRtrIntfMcastFwdModeSet(intIfNum, L7_DISABLE) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
              "Mcast Forwarding Mode Disable Failed for intf - %s"
              " The Multicast Forwarding mode Disable Failed for this routing interface.", ifName);
    }
  }

  if (mode == L7_ENABLE)
  {

    /* If the Global Multicast Mode is Enable, then go ahead and enable the
     * the Operational state of DVMRP.
     */
    mcastMapMcastAdminModeGet(&mcastMode);
    if(mcastMode == L7_DISABLE)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                         "Global Multicast Mode is Disabled");
      return L7_FAILURE;
    }

    /* If no IP address has been assigned yet, just return a success.  The
     * enable will be completed when we get notified of an IP address add.
     */
    if ((ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask) != L7_SUCCESS) ||
        (ipAddr == L7_NULL_IP_ADDR))
    {
      return L7_SUCCESS;
    }

    /* set DVMRP router multicast address in NPU */
    if (dtlIpv4LocalMulticastAddrAdd(intIfNum, L7_IP_ALL_DVMRP_RTRS_ADDR)
        != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
              "DVMRP All Routers Address - %x Add to the DTL Mcast List "
              "Failed for intf - %s"
              "DMVRP All Routers Address addition to the local multicast list Failed."
              " As a result of this, DVMRP Multicast packets with this address will not"
              " be received at the application.", L7_IP_ALL_DVMRP_RTRS_ADDR, ifName);
      return L7_FAILURE;
    }

    if (ipMapRtrIntfMcastFwdModeSet(intIfNum, L7_ENABLE) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
              "Mcast Forwarding Mode Enable Failed for intf - %s"
              "The Multicast Forwarding mode Enable Failed for this routing interface."
              " As a result of this, the ability to forward Multicast packets does not"
              " function on this interface.", ifName);
      return L7_FAILURE;
    }
  }

  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "Failed to convert Internal Interface Number to Router "
             "Interface Number\n");
    return L7_FAILURE;
  }

  return dvmrpMapExtenIntfAdminModeSet(rtrIfNum, mode);
}
/*********************************************************************
* @purpose  Set the DVMRP Metric for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    metric      @b{(input)} Administrative mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes:
*           1. DVMRP is enabled
*           2. Interface has been created and configured for DVMRP at
*              the DVMRP vendor layer
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfMetricApply(L7_uint32 intIfNum, L7_ushort16 metric)
{
  L7_uint32 rtrIfNum;
  /* don't apply config unless intf is created
   * (return w/success for pre-config)
   */
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
  {
    if (dvmrpMapIntfIsOperational(intIfNum) != L7_TRUE)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Failed to access if the DVMRP component has been initialized for \
               the specified interface.\n");
      return L7_FAILURE;
    }
  }
  return dvmrpMapExtenIntfMetricSet(rtrIfNum, metric);
}
/*********************************************************************
* @purpose  Apply DVMRP config data
*
* @param    void
*
* @returns  L7_SUCCESS  Config data applied, or not a DVMRP interface
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpApplyConfigData(void)
{
  L7_uint32 intIfNum;
  L7_uint32 dvmrpIfMode;
  L7_uint32 cfgIndex;

  if ((dvmrpMapDvmrpIsOperational() != L7_TRUE) &&
      (ipMapRtrAdminModeGet() == L7_DISABLE))
  {
    return L7_SUCCESS;
  }

  if (dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode == L7_ENABLE)
  {
    dvmrpMapDvmrpAdminModeEnable(L7_TRUE);
  }
  else if (dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode == L7_DISABLE)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                    "\nIn dvmrpApplyConfigData function dvmrpMapDvmrpAdminModeDisable"
                    "called\n");
    dvmrpMapDvmrpAdminModeDisable(L7_TRUE);
  }

  /*-----------------------------------*/
  /* configure per-interface DVMRP parms */
  /*-----------------------------------*/

  /* apply any interface configuration data for interfaces that exist **
  ** at this time */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
  {
    if (nimIntIfFromConfigIDGet(&(dvmrpGblVar_g.DvmrpMapCfgData.
                                  ckt[cfgIndex].configId), &intIfNum) == L7_SUCCESS)
    {
      if (dvmrpMapRouterIntfConfigure(intIfNum)== L7_SUCCESS)
      {
        if (dvmrpMapIntfAdminModeGet(intIfNum, &dvmrpIfMode)== L7_SUCCESS)
        {
          if (dvmrpMapIntfAdminModeApply(intIfNum, dvmrpIfMode) != L7_SUCCESS)
          {
            DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                            "\nFailure to configure DVMRP interface %d\n",
                            intIfNum);
          }
        }
      }
    }
  }
  return L7_SUCCESS;
}

/*---------------------------------------------------------------------
 *                      OTHER UTILITY FUNCTIONS
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Update the DVMRP-MAP Counters
*
* @param    None
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t
dvmrpMapCountersUpdate (DVMRP_MAP_COUNTERS_TYPE_t counterType,
                        DVMRP_MAP_COUNTERS_ACTION_t counterAction)
{
  /* Lock the Table */
  if (osapiSemaTake (dvmrpGblVar_g.eventCountersSemId, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "Failed to take DVMRP-MAP Counters Semaphore.\n");
    return L7_FAILURE;
  }

  switch (counterAction)
  {
    case DVMRP_MAP_COUNTERS_INCREMENT:
      (dvmrpGblVar_g.eventCounters[counterType])++;
      break;

    case DVMRP_MAP_COUNTERS_DECREMENT:
      if ((dvmrpGblVar_g.eventCounters[counterType]) <= 0)
       {
         DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "DVMRP: Invalid operation on counterType %d ",counterType);
         osapiSemaGive (dvmrpGblVar_g.eventCountersSemId);
         return L7_FAILURE;
       }
      (dvmrpGblVar_g.eventCounters[counterType])--;
      break;

    case DVMRP_MAP_COUNTERS_RESET:
      (dvmrpGblVar_g.eventCounters[counterType]) = 0;
      break;

    default:
      break;
  }

  /* Return successful */
  osapiSemaGive (dvmrpGblVar_g.eventCountersSemId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DVMRP-MAP Counters Value
*
* @param    None
*
* @returns  counterValue
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32
dvmrpMapCountersValueGet (DVMRP_MAP_COUNTERS_TYPE_t counterType)
{
  L7_uint32 counterValue = 0;

  /* Lock the Table */
  if (osapiSemaTake (dvmrpGblVar_g.eventCountersSemId, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "Failed to take DVMRP-MAP Counters Semaphore.\n");
    return counterValue;
  }

  counterValue = (dvmrpGblVar_g.eventCounters[counterType]);

  /* Return successful */
  osapiSemaGive(dvmrpGblVar_g.eventCountersSemId);
  return counterValue;
}


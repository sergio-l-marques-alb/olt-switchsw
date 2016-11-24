/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  pimdm_map_util.c
*
* @purpose   PIMDM Utility Functions
*
* @component PIMDM Mapping Layer
*
* @comments  none
*
* @create    02/04/2006
*
* @author    gkiran
*
* @end
*
**********************************************************************/

#include "l7_mcast_api.h"
#include "l7_pimdm_api.h"
#include "l7_ip_api.h"
#include "mcast_wrap.h"
#include "pimdm_map.h"
#include "pimdm_map_util.h"
#include "pimdm_map_rto.h"
#include "pimdm_map_debug.h"
#include "pimdm_map_vend_exten.h"
#include "pimdm_map_v6_wrappers.h"

/*---------------------------------------------------------------------
 *                  SUPPORT FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */
/*********************************************************************
* @purpose  Enable the PIMDM Routing Function
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    pimdmDoInit      @b{(input)} Flag to check whether PIM-DM
*                                        Memory can be Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimdmAdminModeEnable(pimdmMapCB_t *pimdmMapCbPtr,
                                     L7_BOOL pimdmDoInit)
{
    L7_uint32 i,intIfNum = L7_NULL;
    L7_uint32 mode = L7_NULL;
    L7_RC_t rc = L7_SUCCESS;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (!(PIMDM_IS_READY))
    {
      return L7_SUCCESS;
    }

    if (pimdmMapExtenPimdmAdminModeSet (pimdmMapCbPtr, L7_ENABLE, pimdmDoInit)
                                     != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                      "PIMDM Admin Mode Set failed for familyType %d",
                      pimdmMapCbPtr->familyType);
      rc = L7_FAILURE;
    }

    /* enable router interfaces which are configured for routing */
    for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
    {
      /* NOTE:  This call fails if router intf not created yet */
      if (mcastIpMapRtrIntfToIntIfNum(pimdmMapCbPtr->familyType, 
                                      i, &intIfNum) == L7_SUCCESS)
      {
        if ((pimdmMapIntfAdminModeGet(pimdmMapCbPtr->familyType, intIfNum, 
             &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
        {
          if (pimdmMapIntfModeApply(intIfNum, pimdmMapCbPtr,
                                    L7_ENABLE) != L7_SUCCESS)
          {
            PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                            "PIMDM Admin Mode Enable failed on intIfNum(%d) "
                            "for familyType %d",intIfNum,
                            pimdmMapCbPtr->familyType); 
          }
        }
      }
    }

    if (rc == L7_FAILURE)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "PIM-DM Admin Mode Enable Failed");
      return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable the PIMDM Routing Function
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    pimdmDoInit      @b{(input)} Flag to check whether PIM-DM
*                                        Memory can be De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimdmAdminModeDisable(pimdmMapCB_t *pimdmMapCbPtr,
                                      L7_BOOL pimdmDoInit)
{
    L7_uint32 rtrIfNum = L7_NULL;
    L7_uint32 intIfNum = L7_NULL;
    pimdmCfgCkt_t *pCfg =  L7_NULLPTR;
    L7_RC_t rc = L7_SUCCESS;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
   /* NOTE:  Return success if PIMDM has not been initialized.
    *        This allows for dynamic configurability of PIMDM prior to
    *        initializing PIMDM.
    */

    if (pimdmMapPimdmIsInitialized(pimdmMapCbPtr) == L7_TRUE)
    {

    /*----------------------------------------------------------------*/
    /* Disable router interfaces configured for PIM-DM */
    /*----------------------------------------------------------------*/
    if (PIMDM_IS_READY)
    {
      for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
      {
        if (mcastIpMapRtrIntfToIntIfNum(pimdmMapCbPtr->familyType, 
                                             rtrIfNum,&intIfNum) != L7_SUCCESS)
        {
          continue;
        }

        if ( (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, 
              intIfNum, &pCfg) == L7_TRUE) && (pCfg->mode == L7_ENABLE) &&
             (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) == L7_TRUE) )
        {
          if (pimdmMapIntfModeApply(intIfNum, 
                                    pimdmMapCbPtr, L7_DISABLE) != L7_SUCCESS)
          {
            PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, 
                            "Could not disable PIM-DM interface on %d for "
                            "familyType %d", intIfNum,
                             pimdmMapCbPtr->familyType);
          }
        }
      }
    }
    }

    /*The following function has to be called even if the protocol is not 
      operationally enabled for the reason that the heap memory would have 
      been allocated even if the protcol is not operatiopnally enabled,and 
      so that memory has to be freed up.
     */


    /* apply the PIMDM configuration change */
    if (pimdmMapExtenPimdmAdminModeSet(pimdmMapCbPtr, L7_DISABLE, pimdmDoInit)
                                    != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                      "PIMDM Global Admin Mode Disable failed for "
                      "familyType %d",pimdmMapCbPtr->familyType); 
      rc = L7_FAILURE;
    }

    return rc;
}

/*---------------------------------------------------------------------
 *                 SUPPORT FUNCTIONS  -  INTERFACE CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Set the PIMDM admin mode for the specified interface
*
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    mode            @b{(input)} Administrative mode 
*                                       (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes:
*           1. PIMDM is enabled
*           2. Interface has been created and configured for PIMDM at
*              the PIMDM vendor layer
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfModeApply(L7_uint32 intIfNum, 
                              pimdmMapCB_t *pimdmMapCbPtr, L7_uint32 mode)
{
  L7_inet_addr_t  ipAddr;
  L7_uint32 mcastMode;
  L7_uint32 rtrIfNum = 0;
  L7_inet_addr_t allRoutersAddr;   
  L7_uchar8 addr[IPV6_DISP_ADDR_LEN];

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\n Function Entry\n");

  inetAddressReset(&ipAddr);

  /* These pre-conditions has to be removed when the support is provided 
     for unnumbered interfaces for PIMDM */
  if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {  
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, "\n Failed to convert to rtrIfNum for intIfNum (%d)", intIfNum);
    return L7_FAILURE;
  }
  if (mcastIpMapIsRtrIntfUnnumbered(pimdmMapCbPtr->familyType,rtrIfNum) == L7_TRUE)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\n The rtrIfNum(%d) is unnumbered interface", rtrIfNum);
    return L7_SUCCESS;
  }


  switch (mode)
  {
    case L7_ENABLE:
    {
      /* If the Global Multicast Mode is Enable, then go ahead and enable the
       * the Operational state of PIM-DM.
       */
      mcastMapMcastAdminModeGet(&mcastMode);
      if(mcastMode == L7_DISABLE)
      {
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, 
                           "Global Multicast Mode is Disabled");
        return L7_FAILURE;
      }
      /* If no IP address has been assigned yet, just return a success.  
       * The enable will be completed when we get notified of an IP address add.
       */
      if ((mcastIpMapRtrIntfIpAddressGet (pimdmMapCbPtr->familyType, intIfNum, 
                                          &ipAddr) != L7_SUCCESS) ||
          (inetIsAddressZero (&ipAddr) == L7_TRUE))
      {
        return L7_SUCCESS;
      }

      /* Initialize the All Routers Address */
      inetAllPimRouterAddressInit (pimdmMapCbPtr->familyType, &(allRoutersAddr));
      if(mcastLocalMulticastAddrUpdate(pimdmMapCbPtr->familyType,intIfNum,
                                    &(allRoutersAddr),L7_TRUE) != L7_SUCCESS)
      {
        PIMDM_MAP_LOG_MSG (pimdmMapCbPtr->familyType,"PIM All Routers Address - %s "
                           "Add to the DTL Mcast List Failed for intIfNum - %d",
                           inetAddrPrint(&allRoutersAddr,addr), intIfNum);
        return L7_FAILURE;
      }

      if (mcastRtrIntfMcastFwdModeSet(pimdmMapCbPtr->familyType,intIfNum,
                                      L7_ENABLE) != L7_SUCCESS)
      {
        PIMDM_MAP_LOG_MSG (pimdmMapCbPtr->familyType, "Mcast Forwarding Mode "
                           "Enable Failed for intIfNum - %d", intIfNum);
        return L7_FAILURE;
      }
      

      break;
    }

    case L7_DISABLE:
    {
      if (mcastRtrIntfMcastFwdModeSet(pimdmMapCbPtr->familyType,intIfNum,
                                      L7_DISABLE) != L7_SUCCESS)
      {
        PIMDM_MAP_LOG_MSG (pimdmMapCbPtr->familyType, "Mcast Forwarding Mode "
                           "Disable Failed for intIfNum - %d", intIfNum);
      }

        /* Initialize the All Routers Address */
      inetAllPimRouterAddressInit (pimdmMapCbPtr->familyType, &(allRoutersAddr));
      if(mcastLocalMulticastAddrUpdate(pimdmMapCbPtr->familyType,intIfNum,
                                    &(allRoutersAddr),L7_FALSE) != L7_SUCCESS)
      {
        PIMDM_MAP_LOG_MSG (pimdmMapCbPtr->familyType,"PIM All Routers Address - %s "
                           "Delete from the DTL Mcast List Failed for intIfNum - %d",
                           inetAddrPrint(&allRoutersAddr,addr), intIfNum);
      }
      break;
    }

    default:
      break;
  }

  return (pimdmMapExtenIntfAdminModeSet (pimdmMapCbPtr, intIfNum, mode));
}

/*********************************************************************
* @purpose  Set the PIMDM Hello interval for the specified interface
*
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    helloIntvl      @b{(input)} Hello Interval 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes:
*           1. PIMDM is enabled
*           2. Interface has been created and configured for PIMDM at
*              the PIMDM vendor layer
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfHelloIntervalApply(pimdmMapCB_t *pimdmMapCbPtr, 
                           L7_uint32 intIfNum, L7_uint32 helloIntvl)
{
  pimdmCfgCkt_t   *pCfg;
  if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
  { 
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "intIfNum(%d) not valid for PIMDM configuration for \
                        familyType %d",intIfNum,pimdmMapCbPtr->familyType);
    return L7_ERROR;
  }
      

  if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum)!= L7_TRUE)
  {
   PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                   "PIM-DM not operational on intIfNum(%d) for \
                   familyType %d",intIfNum,pimdmMapCbPtr->familyType);
    return L7_ERROR;
  }

  return(pimdmMapExtenIntfHelloIntervalSet(pimdmMapCbPtr, intIfNum, 
                                           helloIntvl));
}

/*---------------------------------------------------------------------
 *                      OTHER UTILITY FUNCTIONS
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Determine if the PIMDM component has been initialized
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This is often used to determine whether a configuration
*           value can be applied or not.
*
* @end
*********************************************************************/
L7_BOOL pimdmMapPimdmIsInitialized(pimdmMapCB_t *pimdmMapCbPtr)
{
  if (pimdmMapCbPtr->pPimdmInfo == L7_NULL)
  {
    /* PIMDM component was not started during system initialization */
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                    "PIMDM component not initialized for familyType %d",
                    pimdmMapCbPtr->familyType);
    return L7_FALSE;
  }

  /* return value based on current PIMDM initialization state */
  return(pimdmMapCbPtr->pPimdmInfo->pimdmInitialized);
}

/*********************************************************************
* @purpose  Determine if the PIMDM component has been initialized for
*           the specified interface.
*
* @param    intIfNum         @b{(input)}  Interface Number.
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This is often used to determine whether a configuration
*           value can be applied or not.
*
* @end
*********************************************************************/
L7_BOOL pimdmMapIntfIsOperational(pimdmMapCB_t *pimdmMapCbPtr, L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum = L7_NULL;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\n Intf Num: %d\n", intIfNum);
  if (pimdmMapCbPtr->pPimdmIntfInfo == L7_NULL)
  {
    /* PIMDM component was not started during system initialization */
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                    "PIMDM component not initialized for familyType %d",
                    pimdmMapCbPtr->familyType);
    return L7_FALSE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType, 
                                       intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                    "Failed to convert intIfNum (%d) to rtrIfNum for \
                    familyType %d",intIfNum,pimdmMapCbPtr->familyType);
    return L7_FALSE;
  }

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, 
                  "\n pimdmInitialized:%d RtrIfNum: %d\n", 
                  pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].pimdmIsOperational, 
                  rtrIfNum);
  /* return value based on current PIMDM initialization state */
  return(pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].pimdmIsOperational);
}

/*********************************************************************
* @purpose  Determine if the interface is valid for PIMDM configuration
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    pimdmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    pCfg          @b{(input)} Output pointer location, 
*                                      or L7_NULL if not needed
*                          @b{(output)} Pointer to PIMDM interface 
*                                       config structure
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the PIMDM component has been
*           started (regardless of whether the PIMDM protocol is enabled
*           or not).
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the
*           interface config data structure.
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL pimdmMapIntfIsConfigurable(pimdmMapCB_t *pimdmMapCbPtr, 
                      L7_uint32 intIfNum, pimdmCfgCkt_t **pCfg)
{
  L7_uint32 index = L7_NULL;
  nimConfigID_t configId;

  if (!(PIMDM_IS_READY))
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                    "PIMDM component not ready for familyType %d",
                     pimdmMapCbPtr->familyType);
    return L7_FALSE;
  }

  index = pimdmMapCbPtr->pPimdmMapCfgMapTbl[intIfNum];

  if (index == 0)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"Invalid Index %d for \
                    familyType %d",index,pimdmMapCbPtr->familyType);
    return L7_FALSE;
  }

  /* verify that the configId in the config data table entry matches the
   * configId that NIM maps to the intIfNum we are considering.
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId,
    	                   &(pimdmMapCbPtr->pPimdmMapCfgData->ckt[index].configId))
    	                == L7_FALSE)
    {
      /* if we get here, there is synchronization
       * issue between NIM and components w.r.t. interface creation/deletion 
       */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, 
                        "Error accessing PIMDM config data for \
                        intIfNum(%d)",intIfNum);
      return L7_FALSE;
    }
  }

  if (pCfg != L7_NULLPTR)
  {
    *pCfg = &(pimdmMapCbPtr->pPimdmMapCfgData->ckt[index]);
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  To get the mapping layer CB based on family Type
*
* @param    familyType      @b{(input)}  Address Family type.
* @param    pimdmMapCbPtr  @b{(output)} Mapping Control Block.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapCtrlBlockGet(L7_uchar8 familyType, 
                                     pimdmMapCB_t **pimdmMapCbPtr)
{
  switch (familyType)
  {
    case L7_AF_INET:
     *pimdmMapCbPtr = &pimdmMapCB_g[PIMDM_MAP_IPV4_CB];
      break;
    case L7_AF_INET6:
     if (pimdmMapV6CtrlBlockGet(pimdmMapCbPtr) != L7_SUCCESS)
     {
       *pimdmMapCbPtr = L7_NULLPTR;
       PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Family Type (%d) Not Supported",familyType);
      return L7_FAILURE;
     }
     break;
    default:
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Invlaid Family Type (%d)",familyType);
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update the PIMSM-MAP Counters
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
pimdmMapCountersUpdate (PIMDM_MAP_COUNTERS_TYPE_t counterType,
                        PIMDM_MAP_COUNTERS_ACTION_t counterAction)
{
  /* Lock the Table */
  if (osapiSemaTake (pimdmGblVariables_g.eventCountersSemId, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, "Failed to take PIMDM-MAP Counters Semaphore.\n");
    return L7_FAILURE;
  }

  switch (counterAction)
  {
    case PIMDM_MAP_COUNTERS_INCREMENT:
      (pimdmGblVariables_g.eventCounters[counterType])++;
      break;

    case PIMDM_MAP_COUNTERS_DECREMENT:
      if ((pimdmGblVariables_g.eventCounters[counterType]) <= 0)
       {
         PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "PIMDM: Invalid operation on counterType %d ",counterType);
         return L7_FAILURE;
       }
      (pimdmGblVariables_g.eventCounters[counterType])--;
      break;

    case PIMDM_MAP_COUNTERS_RESET:
      (pimdmGblVariables_g.eventCounters[counterType]) = 0;
      break;

    default:
      break;
  }

  /* Return successful */
  osapiSemaGive (pimdmGblVariables_g.eventCountersSemId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the PIMDM-MAP Counters Value
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
pimdmMapCountersValueGet (PIMDM_MAP_COUNTERS_TYPE_t counterType)
{
  L7_uint32 counterValue = 0;

  /* Lock the Table */
  if (osapiSemaTake (pimdmGblVariables_g.eventCountersSemId, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, "Failed to take PIMDM-MAP Counters Semaphore.\n");
    return counterValue;
  }

  counterValue = (pimdmGblVariables_g.eventCounters[counterType]);

  /* Return successful */
  osapiSemaGive(pimdmGblVariables_g.eventCountersSemId);
  return counterValue;
}


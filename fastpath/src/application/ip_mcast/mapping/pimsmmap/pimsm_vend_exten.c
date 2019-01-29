/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  pimsm_vend_exten.c
*
* @purpose   PIM-SM vendor-specific functions
*
* @component PIM-SM Mapping Layer
*
* @comments  none
*
* @create    03/09/2002
*
* @author    Ratnakar
*
* @end
*
**********************************************************************/


#include "l7_pimsminclude.h"
#include "mcast_wrap.h"
#include "pimsmuictrl.h"
#include "mcast_wrap.h"
#include "pimsm_vend_exten.h"
#include "intf_bitset.h"
#include "pimsm_map.h"
#include "l7_mcast_api.h" 
#include "pimsm_v6_wrappers.h"
#include "pimsmcontrol.h"

/*********************************************************************
* @purpose  Sets the PIMSM Admin mode
*
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    mode            @b{(input)} L7_ENABLE or L7_DISABLE
* @param    pimsmDoInit     @b{(input)} Flag to check whether PIM-SM
*                                       Memory can be Initialized/
*                                       De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenPimsmAdminModeSet(pimsmMapCB_t *pimsmMapCbPtr, 
                                       L7_uint32 mode,
                                       L7_BOOL pimsmDoInit)
{
  L7_uint32 mcastMode=L7_DISABLE;  
  L7_uint32 rtrIfNum;    

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   if (mode == L7_ENABLE)
   {
     /* initialize vendor stack and apply stored config data now */
     /*---------------------------------------------------------------*/
     /* initialize the PIMSM stack if configured adminmode is enabled */
     /*---------------------------------------------------------------*/
     if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational != L7_TRUE)
     {
       /* Initialize the PIM-SM Vendor Layer */
       if (pimsmDoInit == L7_TRUE)
                  
       {
         if(pimsmCnfgrInitPhase1DynamicProcess (pimsmMapCbPtr) != L7_SUCCESS)
         {
           PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,
                            "Vendor Layer Dynamic Init Failed for Family - %d",
                            pimsmMapCbPtr->familyType);
           return L7_FAILURE;
         }
       }

      if((mcastIpMapRtrAdminModeGet(pimsmMapCbPtr->familyType) == L7_ENABLE) &&
         (mcastMapMcastAdminModeGet(&mcastMode) ==L7_SUCCESS)&&(mcastMode == L7_ENABLE))
      {
       /* Process PIM SM Global INIT */
       if (pimsmGlobalAdminModeProcess(pimsmMapCbPtr->cbHandle,
                                       L7_TRUE) != L7_SUCCESS)
       {
         PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                         "Failed to Process Global Admin Mode Set "
                         "for mode (%d)",mode);
         if (pimsmDoInit == L7_TRUE)
         {
         /* De-Initialize the protocol's Memory & Control Block */
         pimsmCnfgrFiniPhase1DynamicProcess (pimsmMapCbPtr);
         }
         return L7_FAILURE;
       }
       pimsmMapCbPtr->pPimsmInfo->pimsmOperational = L7_TRUE;
      }
     }
   }
   else
   {
     if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_TRUE)
     {
       /* Process PIM SM Global INIT */
       if (pimsmGlobalAdminModeProcess(pimsmMapCbPtr->cbHandle,
                                       L7_FALSE) != L7_SUCCESS)
       {
         PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                           "Failed to Process Global Admin Mode Set \
                           for mode (%d)",mode);
         return L7_FAILURE;
       }
       pimsmMapCbPtr->pPimsmInfo->pimsmOperational = L7_FALSE;
      /* pimsmGlobalAdminModeProcess disables all the interfaces in the vendor code.
         So in the mapping layer,all the interfaces have to be made operationally disabled.
       */
      for (rtrIfNum= 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
      {
        pimsmMapCbPtr->pimsmIntfInfo[rtrIfNum].pimsmOperational= L7_FALSE;
      }
       
     }
       
       /* De-Initialize the protocol's Memory & Control Block */
     if (pimsmDoInit == L7_TRUE)
       {
         if (pimsmCnfgrFiniPhase1DynamicProcess (pimsmMapCbPtr) != L7_SUCCESS)
         {
           PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,
                            "Vendor Layer Dynamic De-Init Failed for Family - %d",
                            pimsmMapCbPtr->familyType);
         }
       }
   }
   return L7_SUCCESS;
}

void pimsmMapExtenRestartFlagClear(pimsmMapCB_t *pimsmMapCbPtr)
{
  if (L7_NULLPTR != pimsmMapCbPtr->cbHandle)
  {
    pimsmRestartFlagClear(pimsmMapCbPtr->cbHandle);
  }
  return;
}

L7_BOOL pimsmMapExtenRestartFlagGet(pimsmMapCB_t *pimsmMapCbPtr)
{
  if (L7_NULLPTR != pimsmMapCbPtr->cbHandle)
  {
    return(pimsmRestartFlagGet(pimsmMapCbPtr->cbHandle));
  }
  else
  {
    return L7_FALSE;
  }
}

/*********************************************************************
* @purpose  Sets the static RP information
*
* @param    pimsmMapCbPtr  @b{(input)}       Mapping Control Block.
* @param    index          @b{(input)}       Index into the config structure 
*                                            for this static RP
* @param    rpIpAddr       @b{(input)}       Ip address of the RP
* @param    rpGrpAddr      @b{(input)}       Group address supported by the RP
* @param    rpGrpMask      @b{(input)}       Group mask for the group address
* @param    conflict       @b{(input)}       conflict
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStaticRPSet(pimsmMapCB_t *pimsmMapCbPtr, 
                          L7_inet_addr_t *rpIpAddr, L7_inet_addr_t *rpGrpAddr, 
                          L7_inet_addr_t *rpGrpMask, L7_BOOL conflict)
{
  pimsmStaticRPdata_t staticRP;
  
  staticRP.operMode = L7_TRUE;
  staticRP.pimsmStaticRPOverrideDynamic = conflict;
  inetMaskToMaskLen(rpGrpMask, &staticRP.pimsmStaticRPGrpPrefixLength);
  staticRP.pPimsmStaticRPRPAddress = rpIpAddr;
  staticRP.pPimsmStaticRPGrpAddress = rpGrpAddr;

 /* Process PIM SM static RP Set  */
  if (pimsmStaticRPProcess(pimsmMapCbPtr->cbHandle, &staticRP) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove the static RP information
*
* @param    pimsmMapCbPtr  @b{(input)}      Mapping Control Block.
* @param    index          @b{(input)}      Index into the config structure 
*                                           for this static RP
* @param    rpIpAddr       @b{(input)}      Ip address of the RP
* @param    rpGrpAddr      @b{(input)}      Group address supported by the RP
* @param    rpGrpMask      @b{(input)}      Group mask for the group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStaticRPRemove(pimsmMapCB_t *pimsmMapCbPtr,
                       L7_inet_addr_t *rpIpAddr, 
             L7_inet_addr_t *rpGrpAddr, L7_inet_addr_t *rpGrpMask)
{
  pimsmStaticRPdata_t staticRP;
  
  staticRP.operMode = L7_FALSE;
  staticRP.pimsmStaticRPOverrideDynamic = L7_FALSE;
  inetMaskToMaskLen(rpGrpMask, &staticRP.pimsmStaticRPGrpPrefixLength);
  staticRP.pPimsmStaticRPRPAddress = rpIpAddr;
  staticRP.pPimsmStaticRPGrpAddress = rpGrpAddr;

  /* Process PIM SM static RP Set  */
  if (pimsmStaticRPProcess(pimsmMapCbPtr->cbHandle, &staticRP) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To send Admin Mode Set event to PIMSM task.
*
* @param    pimsmMapCbPtr  @b{(input)}       Mapping Control Block.
* @param    rtrIfNum       @b{(input)}       Interface Number
* @param    mode           @b{(input)}       Admin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmAsyncActivateInterface(pimsmMapCB_t *pimsmMapCbPtr,
                                    L7_uint32 intIfNum, L7_BOOL mode)
{
  pimsmIntfStatus_t adminMode;
  L7_inet_addr_t    intfAddr;

  inetAddressZeroSet(pimsmMapCbPtr->familyType, &intfAddr);
  
  adminMode.intIfNum = intIfNum;
  adminMode.isEnabled = mode;

  if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType,
                                 intIfNum, &adminMode.rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Error in converting IntIfNum(%d) to Rtr Num",
                      intIfNum);
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfIpAddressGet(pimsmMapCbPtr->familyType, 
                                intIfNum, &intfAddr) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to get ip address on intIfNum(%d)",intIfNum);
    return L7_FAILURE;
  }
  adminMode.intfAddr = &intfAddr;
  /* Process PIMSM interface admin mode */
  if (pimsmIntfStatusProcess(pimsmMapCbPtr->cbHandle,
                             &adminMode) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to process PIMSM Interface Status Change on \
                       intIfNum(%d)",intIfNum);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the administrative mode for the specified interface
*
* @param    pimsmMapCbPtr  @b{(input)}      Mapping Control Block.
* @param    intIfNum       @b{(input)}      Internal Interface Number
* @param    mode           @b{(input)}      Admin mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Interface is made operational according to the admin mode
*           request only if PIM-SM is currently operational
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceModeSet(pimsmMapCB_t *pimsmMapCbPtr, 
                              L7_uint32 intIfNum, L7_uint32 mode)
{
   L7_uint32 state = L7_NULL;
   L7_BOOL operation;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
   if (mode == L7_ENABLE)
      operation = L7_TRUE;
   else
      operation = L7_FALSE;

   if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_FALSE)
   {
       /* PIM-SM is not globally operational */
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,
       "\nPIM-SM not globally operational\n");
       return L7_SUCCESS; /* valid condition for dynamic configurability */
   }

   if (operation == L7_FALSE)
   {
       /* De-activate PIM-SM on the interface   */
      if (pimsmAsyncActivateInterface(pimsmMapCbPtr,
                           intIfNum, L7_FALSE) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                          "Failed to deactivate PIMSM on intIfNum(%d)",
                          intIfNum);
      }

      pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational = L7_FALSE;
   }
   else
   {
     /* Activate PIM-SM on the interface   */
     if ((nimGetIntfActiveState(intIfNum, &state) == L7_SUCCESS) && 
         (state != L7_ACTIVE))
     {
       return L7_SUCCESS;
     }

     if (pimsmAsyncActivateInterface(pimsmMapCbPtr,
                           intIfNum, L7_TRUE) != L7_SUCCESS)
     {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                         "Failed to activate PIMSM on intIfNum(%d)",
                         intIfNum);
       return L7_FAILURE;
     }

     pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational = L7_TRUE;

     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nInterface operational now\n");
   }
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DR IP Address for the specified interface
*
* @param    pimsmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    dsgRtrIpAddr   @b{(output)} IP Address of designated router
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceDRGet(pimsmMapCB_t *pimsmMapCbPtr,
              L7_uint32 intIfNum, L7_inet_addr_t *dsgRtrIpAddr)
{
    L7_uint32  rtrIfNum;

    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                   intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting IntIfNum(%d) to Rtr Num",
                        intIfNum);
      return L7_FAILURE;
    }
    return pimsmInterfaceObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum, 
                        I_pimsmInterfaceDR,(L7_uint32 *) dsgRtrIpAddr);
  
}

/*********************************************************************
* @purpose  Get the uptime for the specified neighbour
*
* @param    pimsmMapCbPtr  @b{(input)}  Mapping Control Block.
* @param    intIfNum       @b{(input)}  Internal Interface Number
* @param    nbrIpAddr      @b{(input)}  IP Address of Neighbor
* @param    nbrUpTime      @b{(output)} Uptime in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenNeighborUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr,
              L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr, 
              L7_uint32 *nbrUpTime)
{
    L7_uint32  rtrIfNum;

    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                   intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting IntIfNum(%d) to Rtr Num",
                        intIfNum);
      return L7_FAILURE;
    }
    return pimsmNeighborObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum,
                         nbrIpAddr, I_pimsmNeighborUpTime, nbrUpTime);
}

/*********************************************************************
* @purpose  Get the expiry time for the specified neighbour
*
* @param    pimsmMapCbPtr @b{(input)}   Mapping Control Block.
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    nbrIpAddr     @b{(input)}   IP Address of Neighbor
* @param    nbrExpiryTime @b{(output)}   Expiry time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenNeighborExpiryTimeGet(pimsmMapCB_t *pimsmMapCbPtr,
                  L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr, 
                  L7_uint32 *nbrExpiryTime)
{
    L7_uint32  rtrIfNum = L7_NULL;

    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                   intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting IntIfNum(%d) to Rtr Num",
                        intIfNum);
      return L7_FAILURE;
    }
    return pimsmNeighborObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum,
                         nbrIpAddr, I_pimsmNeighborExpiryTime, nbrExpiryTime);
}

/*********************************************************************
* @purpose  Get the DR Prioirty for the specified neighbour
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    nbrIpAddr     @b{(input)}  IP Address of Neighbor
* @param    nbrDrPriority @b{(output)} DR Priority. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenNeighborDrPriorityGet(pimsmMapCB_t *pimsmMapCbPtr,
                         L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr, 
                         L7_uint32 *nbrDrPriority)
{
    L7_uint32  rtrIfNum = L7_NULL;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                   intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting IntIfNum(%d) to Rtr Num",
                        intIfNum);
      return L7_FAILURE;
    }
    return pimsmNeighborObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum,
                      nbrIpAddr, I_pimsmNeighborDRPriority, nbrDrPriority);
}
#ifdef PIMSM_MAP_TBD
/*********************************************************************
* @purpose  Get time remaining before the router changes its upstream
*           neighbor back to its RPF neighbor
*
* @param    pimsmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask
* @param    assertTimer         @b{(output)}  in seconds
*
* @returns  L7_SUCCESS            if success
* @returns  L7_FAILURE            if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteUpstreamAssertTimerGet(pimsmMapCB_t *pimsmMapCbPtr,
           L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
           L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertTimer)
{
    return pimsmMRouteEntryObjectGet(pimsmMapCbPtr->cbHandle,
        ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, 
        I_pimsmMRouteUpstreamAssertTimer, assertTimer);
}

/*********************************************************************
* @purpose  Get metric advertised by the assert winner on the upstream
*           interface, or 0 if no such assert is in received
*
* @param    pimsmMapCbPtr      @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup      @b{(input)}   IP multicast group address
* @param    ipMRouteSource     @b{(input)}   Network Address
* @param    ipMRouteSourceMask @b{(input)}   Network Mask (currently not used)
* @param    assertMetric       @b{(output)}  Metric value advertised
*
* @returns  L7_SUCCESS         if success
* @returns  L7_FAILURE         if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteAssertMetricGet(pimsmMapCB_t *pimsmMapCbPtr,
                                             L7_inet_addr_t *ipMRouteGroup,
                                             L7_inet_addr_t *ipMRouteSource,
                                             L7_inet_addr_t *ipMRouteSourceMask,
                                             L7_uint32 *assertMetric)
{
    return pimsmMRouteEntryObjectGet(pimsmMapCbPtr->cbHandle,
           ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, 
           I_pimsmMRouteAssertMetric, assertMetric);
}

/*********************************************************************
* @purpose  Get preference advertised by the assert winner on the upstream
*           interface, or 0 if no such assert is received
*
* @param    pimsmMapCbPtr       @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}   IP multicast group address
* @param    ipMRouteSource      @b{(input)}   Network Address
* @param    ipMRouteSourceMask  @b{(input)}   Network Mask  (currently not used)
* @param    assertMetricPref    @b{(output)}  Metric preference advertised
*
* @returns  L7_SUCCESS         if success
* @returns  L7_FAILURE         if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteAssertMetricPrefGet(pimsmMapCB_t *pimsmMapCbPtr,
                                            L7_inet_addr_t *ipMRouteGroup,
                                            L7_inet_addr_t *ipMRouteSource,
                                            L7_inet_addr_t *ipMRouteSourceMask,
                                            L7_uint32 *assertMetricPref)
{
    return pimsmMRouteEntryObjectGet(pimsmMapCbPtr->cbHandle,
           ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, 
           I_pimsmMRouteAssertMetricPref, assertMetricPref);
}

/*********************************************************************
* @purpose  Get the value of the RPT-bit advertised by the assert winner on
*           the upstream interface, or false if no such assert is in effect
*
* @param    pimsmMapCbPtr       @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}   IP multicast group address
* @param    ipMRouteSource      @b{(input)}   Network Address
* @param    ipMRouteSourceMask  @b{(input)}   Network Mask  (currently not used)
* @param    assertRPTBit        @b{(output)}  Value of RPT bit (1 or 0)
*
* @returns  L7_SUCCESS         if success
* @returns  L7_FAILURE         if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteAssertRPTBitGet(pimsmMapCB_t *pimsmMapCbPtr,
                                             L7_inet_addr_t *ipMRouteGroup,
                                             L7_inet_addr_t *ipMRouteSource,
                                             L7_inet_addr_t *ipMRouteSourceMask,
                                             L7_uint32 *assertRPTBit)
{
    return pimsmMRouteEntryObjectGet(pimsmMapCbPtr->cbHandle,
           ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, 
           I_pimsmMRouteAssertRPTBit, assertRPTBit);
}

/*********************************************************************
* @purpose  Get PIM-specific flags related to a multicast state entry
*
* @param    pimsmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask  (currently not used)
* @param    ipMRouteFlags       @b{(output)} Flag value is 0 or 1
*
* @returns  L7_SUCCESS           if success
* @returns  L7_FAILURE           if failure
*
* @comments Flag is L7_MCAST_PIMSM_FLAG_RPT (0) for RP shared tree,
*                   L7_MCAST_PIMSM_FLAG_SPT (1) for source tree
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteFlagsGet(pimsmMapCB_t *pimsmMapCbPtr,
                                      L7_inet_addr_t *ipMRouteGroup,
                                      L7_inet_addr_t *ipMRouteSource,
                                      L7_inet_addr_t *ipMRouteSourceMask,
                                      L7_uint32 *ipMRouteFlags)
{
    return pimsmMRouteEntryObjectGet(pimsmMapCbPtr->cbHandle,
           ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, 
           I_pimsmMRouteFlags, ipMRouteFlags);
}
#endif
/************************************************************************
* @purpose  Get the holdtime of a Candidate-RP.  If the local router is not
*           the BSR, this value is 0.
*
* @param    pimsmMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    rpSetGroupAddress  @b{(input)}  IP multicast group address
* @param    rpSetGroupMask     @b{(input)}  Multicast group address mask
* @param    rpSetAddress       @b{(input)}  IP address of the Candidate-RP
* @param    rpSetComponent     @b{(input)}  Number uniquely identifying the component
* @param    rpSetHoldTime      @b{(output)} Holdtime of a Candidate-RP in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Our protocol instance is running in only one domain. So there
*           is only one component with index = 1. We ignore the check on
*           component index as there is only one component.
*
* @end
************************************************************************/
L7_RC_t pimsmMapExtenRPSetHoldTimeGet(pimsmMapCB_t *pimsmMapCbPtr,
                                      L7_inet_addr_t *rpSetGroupAddress,
                                      L7_inet_addr_t *rpSetGroupMask,
                                      L7_inet_addr_t *rpSetAddress,
                                      L7_uint32 rpSetComponent,
                                      L7_uint32 *rpSetHoldTime)
{
    L7_uint32 grpPrefixLength = L7_NULL;
    L7_uchar8 origin = L7_NULL;

    return pimsmBsrRPSetEntryObjectGet(pimsmMapCbPtr->cbHandle,
           origin, rpSetGroupAddress, grpPrefixLength, rpSetAddress,
           I_bsrRPSetHoldtime, rpSetHoldTime);
}

/************************************************************************
* @purpose  Get the minimum time remaining before the Candidate-RP will be
*           declared down
*
* @param    pimsmMapCbPtr     @b{(input)} Mapping Control Block.
* @param    rpSetGroupAddress @b{(input)} IP multicast group address
* @param    rpSetGroupMask    @b{(input)} Multicast group address mask
* @param    rpSetAddress      @b{(input)} IP address of the Candidate-RP
* @param    rpSetComponent    @b{(input)}  Number uniquely identifying the component
* @param    rpSetExpiryTime   @b{(output)}  Expiry time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Our protocol instance is running in only one domain. So there
*           is only one component with index = 1. We ignore the check on
*           component index as there is only one component.
*
* @end
************************************************************************/
L7_RC_t pimsmMapExtenRPSetExpiryTimeGet(pimsmMapCB_t *pimsmMapCbPtr,
                                        L7_inet_addr_t *rpSetGroupAddress,
                                        L7_inet_addr_t *rpSetGroupMask,
                                        L7_inet_addr_t *rpSetAddress,
                                        L7_uint32 rpSetComponent,
                                        L7_uint32 *rpSetExpiryTime)
{
    L7_uint32 grpPrefixLength = L7_NULL;
    L7_uchar8 origin = L7_NULL;

    return pimsmBsrRPSetEntryObjectGet(pimsmMapCbPtr->cbHandle,
           origin, rpSetGroupAddress, grpPrefixLength, rpSetAddress,
           I_bsrRPSetExpiryTime, rpSetExpiryTime);

   
}
#if 0 
/*********************************************************************
* @purpose  Gets the Candidate RP Address
*
* @param    pimsmMapCbPtr       @b{(input)}   Mapping Control Block.
* @param    candRPGroupAddress  @b{(input)}   Candidate RP Group Address
* @param    candRPGroupMask     @b{(input)}   Candidate RP Group Mask
* @param    candRPAddr          @b{(output)}  Candidate RP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenCandidateRPAddressGet(pimsmMapCB_t *pimsmMapCbPtr,
                                           L7_inet_addr_t *candRPGroupAddress,
                                           L7_inet_addr_t *candRPGroupMask,
                                           L7_inet_addr_t *candRPAddr)
{
   /* Should be maintained locally in mapping layer.*/
    return L7_NOT_SUPPORTED;
}
#endif
/************************************************************************
* @purpose  Get IP address of the bootstrap router (BSR) for the local
*           PIM region
*
* @param    pimsmMapCbPtr       @b{(input)}   Mapping Control Block.
* @param    elecBSRAddress      @b{(output)}  Elected BSR Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Currently there is no BSR Component table to support multiple
*           domains since only one domain is being supported.
*           Hence there is only index = 1
*
* @end
************************************************************************/
L7_RC_t pimsmMapExtenElectedBSRAddressGet(pimsmMapCB_t *pimsmMapCbPtr, 
                                            L7_inet_addr_t *elecBSRAddress)
{
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle, 
           I_bsrElectedBSRAddress, (L7_uint32 *)elecBSRAddress);
}

/************************************************************************
* @purpose  Get minimum time remaining before the bootstrap router in
*           the local domain will be declared down
*
* @param    pimsmMapCbPtr        @b{(input)}     Mapping Control Block.
* @param    compBSRExpiryTime    @b{(output)}    Expiry time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Currently there is no BSR Component table to support multiple
*           domains since only one domain is being supported.
*           Hence there is only index = 1
*
* @end
************************************************************************/
L7_RC_t pimsmMapExtenElectedBSRExpiryTimeGet(pimsmMapCB_t *pimsmMapCbPtr, 
                                             L7_uint32 *compBSRExpiryTime)
{
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle, 
           I_bsrElectedBSRExpiryTime, (L7_uint32 *)compBSRExpiryTime);

}

/*********************************************************************
* @purpose  Gets the Candidate RP Advertisement time
*
* @param    pimsmMapCbPtr      @b{(input)}    Mapping Control Block.
* @param    elecCRPAdvTime     @b{(output)}    Hold time (in seconds)
*                                             of component when it is C-RP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Currently there is no BSR Component table to support multiple
*           domains since only one domain is being supported.
*           Hence there is only index = 1
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenElectedCRPAdvTimeGet(pimsmMapCB_t *pimsmMapCbPtr, 
                                           L7_uint32 *elecCRPAdvTime)
{
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle, 
           I_bsrElectedBSRCRPAdvTimer, (L7_uint32 *)elecCRPAdvTime);
}

/*********************************************************************
* @purpose  Check whether interface entry exists for the specified interface
*
* @param    pimsmMapCbPtr    @b{(input)}    Mapping Control Block.
* @param    intIfNum         @b{(input)}    Internal Interface Number
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceEntryGet(pimsmMapCB_t *pimsmMapCbPtr,
                                       L7_uint32 intIfNum)
{
    L7_uint32  rtrIfNum;

    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                   intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting IntIfNum(%d) to Rtr Num",
                        intIfNum);
      return L7_FAILURE;
    }
    return pimsmInterfaceEntryGet(pimsmMapCbPtr->cbHandle, rtrIfNum);

}

/*********************************************************************
* @purpose  Get the existing interface entry next to the entry of the
*           specified interface
*
* @param    pimsmMapCbPtr    @b{(output)}   Mapping Control Block.
* @param    intIfNum         @b{(input)}    Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr,
                                           L7_uint32 *intIfNum)
{
  L7_uint32  rtrIfNum = L7_NULL;
  L7_RC_t    retCode = L7_FAILURE;


  if (*intIfNum == 0)
  {
    rtrIfNum = L7_NULL;
    retCode = pimsmInterfaceEntryNextGet(pimsmMapCbPtr->cbHandle, &rtrIfNum);
  }
  else
  {
    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                     *intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting IntIfNum(%d) to Rtr Num",
                        *intIfNum);
      return L7_FAILURE;
    }
    retCode = pimsmInterfaceEntryNextGet(pimsmMapCbPtr->cbHandle, &rtrIfNum);
  }
  if (retCode == L7_SUCCESS)
  {
    if (mcastIpMapRtrIntfToIntIfNum(pimsmMapCbPtr->familyType, 
                                       rtrIfNum, intIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting rtrIfNum(%d) to intIfNum",
                        rtrIfNum);
      return L7_FAILURE;
    }
  }
  return retCode;
}

/*********************************************************************
* @purpose  Check whether neighbor entry exists for the specified IP address
*
* @param    pimsmMapCbPtr    @b{(input)}    Mapping Control Block.
* @param    intIfNum         @b{(input)}    Internal Interface Number
* @param    ipAddress        @b{(input)}    IP Address
*
* @returns  L7_SUCCESS    if neighbor entry exists
* @returns  L7_FAILURE    if neighbor entry does not exist
*
* @comments The PIM neighbors for an interface are sorted in decreasing
*           order of the network addresses (note that to be able to compare
*           them correctly we must translate the addresses in host order.
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenNeighborEntryGet(pimsmMapCB_t *pimsmMapCbPtr, 
                            L7_uint32 intIfNum, L7_inet_addr_t *ipAddress)
{
    L7_uint32 rtrIfNum = L7_NULL; 
    L7_RC_t   retCode = L7_FAILURE;

    if (intIfNum == 0)
    {
      retCode = pimsmNeighborEntryGet(pimsmMapCbPtr->cbHandle, 
                                                     intIfNum, ipAddress);
    }
    else
    {
      if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                           intIfNum, &rtrIfNum) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting IntIfNum(%d) to Rtr Num",
                        intIfNum);
        return L7_FAILURE;
      }
      retCode = pimsmNeighborEntryGet(pimsmMapCbPtr->cbHandle, 
                                                     rtrIfNum, ipAddress);
    }
    return retCode;
}

/*********************************************************************
* @purpose  Get the existing neighbor entry next to the entry of the
*           specified neighbor
*
* @param    pimsmMapCbPtr     @b{(input)}   Mapping Control Block.
* @param    intIfNum          @b{(input)}   Internal Interface Number
* @param    ipAddress         @b{(output)}  IP Address
*
* @returns  L7_SUCCESS    if neighbor entry exists
* @returns  L7_FAILURE    if neighbor entry does not exist
*
* @comments The PIM neighbors for an interface are sorted in decreasing
*           order of the network addresses (note that to be able to compare
*           them correctly we must translate the addresses in host order.
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenNeighborEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr,
                            L7_uint32 *intIfNum, L7_inet_addr_t *ipAddress)
{
    L7_uint32 rtrIfNum = L7_NULL; 
    L7_RC_t   retCode = L7_FAILURE;

    if (*intIfNum == 0)
    {
      rtrIfNum = L7_NULL;
      retCode = pimsmNeighborEntryNextGet(pimsmMapCbPtr->cbHandle, 
                                                      &rtrIfNum, ipAddress);
    }
    else
    {
      if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                         *intIfNum, &rtrIfNum) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting IntIfNum(%d) to Rtr Num",
                        *intIfNum);
        return L7_FAILURE;
      }
      retCode = pimsmNeighborEntryNextGet(pimsmMapCbPtr->cbHandle, 
                                          &rtrIfNum, ipAddress);
    }
    if (retCode == L7_SUCCESS)
    {
      if (mcastIpMapRtrIntfToIntIfNum(pimsmMapCbPtr->familyType, 
                                           rtrIfNum, intIfNum) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting rtrIfNum(%d) to intIfNum",
                        rtrIfNum);
        return L7_FAILURE;
      }
    }
    return retCode;
}

/*********************************************************************
* @purpose  Get the Neighbor count for specified interface
*
* @param    pimsmMapCbPtr  @b{(input)}  Mapping Control Block.
* @param    intIfNum       @b{(input)}  Internal Interface Number
* @param    nbrCount       @b{(output)} Neighbor Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenNeighborCountGet(pimsmMapCB_t *pimsmMapCbPtr,
                          L7_uint32 intIfNum, L7_uint32 *nbrCount)
{
    L7_uint32  rtrIfNum;

    if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                     intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Error in converting IntIfNum(%d) to Rtr Num",
                        intIfNum);
      return L7_FAILURE;
    }

    return pimsmInterfaceNeighborCountGet(pimsmMapCbPtr->cbHandle, 
    rtrIfNum, nbrCount);
}

#ifdef PIMSM_MAP_TBD
/*********************************************************************
* @purpose  Check whether IP Multicast Route entry exists for the specified
*           group, source and mask
*
* @param    pimsmMapCbPtr         @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup         @b{(input)}   IP Multicast route group
* @param    ipMRouteSource        @b{(input)}   IP Multicast route source
* @param    ipMRouteSourceMask    @b{(input)}   IP Multicast route source mask
*                                               (source mask not being used)
*
* @returns  L7_SUCCESS       if entry exists
* @returns  L7_FAILURE       if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteEntryGet(pimsmMapCB_t *pimsmMapCbPtr, 
   L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
   L7_inet_addr_t *ipMRouteSourceMask)
{

    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get the existing IP Multicast Route entry next to the entry of the
*           specified group, source and mask
*
* @param    pimsmMapCbPtr        @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  IP Multicast route group
* @param    ipMRouteSource       @b{(input)}  IP Multicast route source
* @param    ipMRouteSourceMask   @b{(output)} IP Multicast route source mask
*                                             (source mask not being used)
*
* @returns  L7_SUCCESS       if entry exists
* @returns  L7_FAILURE       if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t *ipMRouteSourceMask)
{

    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Check whether PIMSM routing entry exists for the specified
*           group, source and mask
*
* @param    pimsmMapCbPtr          @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup          @b{(input)}   IP Multicast route group
* @param    ipMRouteSource         @b{(input)}   IP Multicast route source
* @param    ipMRouteSourceMask     @b{(input)}   IP Multicast route source mask
*                                                (source mask not being used)
*
* @returns  L7_SUCCESS       if entry exists
* @returns  L7_FAILURE       if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRouteEntryGet(pimsmMapCB_t *pimsmMapCbPtr, 
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t *ipMRouteSourceMask)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get the existing PIMSM routing entry next to the entry of the
*           specified group, source and mask
*
* @param    pimsmMapCbPtr        @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup        @b{(inout)}  IP Multicast route group
* @param    ipMRouteSource       @b{(inout)}  IP Multicast route source
* @param    ipMRouteSourceMask   @b{(inout)}  IP Multicast route source mask
*                                             (source mask not being used)
*
* @returns  L7_SUCCESS       if entry exists
* @returns  L7_FAILURE       if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRouteEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr,
     L7_inet_addr_t *ipMRouteGroup, 
     L7_inet_addr_t *ipMRouteSource, L7_inet_addr_t *ipMRouteSourceMask)
{

    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get expiry time of the routing entry
*
* @param    pimsmMapCbPtr        @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  IP multicast group address
* @param    ipMRouteSource       @b{(input)}  Network Address
* @param    ipMRouteSourceMask   @b{(input)}  Network Mask (currently not used)
* @param    expiryTime           @b{(output)} Expiry time (in seconds)
*
* @returns  L7_SUCCESS         if success
* @returns  L7_FAILURE         if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRouteExpiryTimeGet(pimsmMapCB_t *pimsmMapCbPtr,
                                        L7_inet_addr_t *ipMRouteGroup,
                                        L7_inet_addr_t *ipMRouteSource,
                                        L7_inet_addr_t *ipMRouteSourceMask,
                                        L7_uint32 *expiryTime)
{

    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get up time of the routing entry
*
* @param    pimsmMapCbPtr       @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}   IP multicast group address
* @param    ipMRouteSource      @b{(input)}   Network Address
* @param    ipMRouteSourceMask  @b{(input)}   Network Mask (currently not used)
* @param    upTime              @b{(output)}  Up time (in seconds)
*
* @returns  L7_SUCCESS         if success
* @returns  L7_FAILURE         if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRouteUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr,
                                    L7_inet_addr_t *ipMRouteGroup,
                                    L7_inet_addr_t *ipMRouteSource,
                                    L7_inet_addr_t *ipMRouteSourceMask,
                                    L7_uint32 *upTime)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get RPF address of the routing entry
*
* @param    pimsmMapCbPtr        @b{(input)}    Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}    IP multicast group address
* @param    ipMRouteSource       @b{(input)}    Network Address
* @param    ipMRouteSourceMask   @b{(input)}    Network Mask (currently not used)
* @param    rpfAddr              @b{(output)}   Address of RPF Neighbor
*
* @returns  L7_SUCCESS         if success
* @returns  L7_FAILURE         if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRouteRpfAddrGet(pimsmMapCB_t *pimsmMapCbPtr,
                                     L7_inet_addr_t *ipMRouteGroup,
                                     L7_inet_addr_t *ipMRouteSource,
                                     L7_inet_addr_t *ipMRouteSourceMask,
                                     L7_inet_addr_t *rpfAddr)
{
#ifdef PIMSM_MAP_TBD
    return pimsmRouteRpfAddrGet(pimsmMapCbPtr->cbHandle, ipMRouteGroup, 
                                ipMRouteSource, ipMRouteSourceMask, rpfAddr);

#else
    return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
* @purpose  Get flags of the routing entry
*
* @param    pimsmMapCbPtr         @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup         @b{(input)}   IP multicast group address
* @param    ipMRouteSource        @b{(input)}   Network Address
* @param    ipMRouteSourceMask    @b{(input)}   Network Mask (currently not used)
* @param    flags                 @b{(output)}  RPT or SPT flag
*
* @returns  L7_SUCCESS         if success
* @returns  L7_FAILURE         if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRouteFlagsGet(pimsmMapCB_t *pimsmMapCbPtr,
                                   L7_inet_addr_t *ipMRouteGroup,
                                   L7_inet_addr_t *ipMRouteSource,
                                   L7_inet_addr_t *ipMRouteSourceMask,
                                   L7_uint32 *flags)
{

    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get incoming interface of the routing entry
*
* @param    pimsmMapCbPtr        @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  IP multicast group address
* @param    ipMRouteSource       @b{(input)}  Network Address
* @param    ipMRouteSourceMask   @b{(input)}  Network Mask (currently not used)
* @param    inIfIndex            @b{(output)} Internal Interface Number
*
* @returns  L7_SUCCESS         if success
* @returns  L7_FAILURE         if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRouteIfIndexGet(pimsmMapCB_t *pimsmMapCbPtr,
                                     L7_inet_addr_t *ipMRouteGroup,
                                     L7_inet_addr_t *ipMRouteSource,
                                     L7_inet_addr_t *ipMRouteSourceMask,
                                     L7_uint32 *inIfIndex)
{

    return L7_NOT_SUPPORTED;
}

/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    pimsmMapCbPtr        @b{(input)}    Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapExtenRouteOutIntfEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr,
                                             L7_inet_addr_t *ipMRouteGroup,
                                           L7_inet_addr_t *ipMRouteSource,
                                           L7_inet_addr_t *ipMRouteSourceMask,
                                           L7_uint32 *outIntIfNum)
{
  /* Don't find this mib object in current mib 
    return pimsmRouteOutIntfEntryNextGet(pimsmMapCbPtr->cbHandle,
          ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, outIntIfNum);
  */
  return L7_NOT_SUPPORTED;
}
#endif

/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    pimsmMapCbPtr        @b{(input)}    Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapExtenSGOIFGet(pimsmMapCB_t *pimsmMapCbPtr,
                                           L7_inet_addr_t *ipMRouteGroup,
                                           L7_inet_addr_t *ipMRouteSource,
                                           L7_inet_addr_t *ipMRouteSourceMask,
                                           L7_INTF_MASK_t      *outIntIfMask)
{
   interface_bitset_t rtrOif;
   L7_uint32 i, intIfNum;
   L7_RC_t rc;

   memset(&rtrOif, 0, sizeof(interface_bitset_t));
   rc = pimsmSGEntryOIFGet(pimsmMapCbPtr->cbHandle, ipMRouteGroup, 
         ipMRouteSource,
        &rtrOif);
   if (rc != L7_SUCCESS)
   {
      return rc;
   }
    for (i = 1; i < MCAST_MAX_INTERFACES; i++)
    {
      if (BITX_TEST(&rtrOif, i)!= L7_NULL)
      {
        if (mcastIpMapRtrIntfToIntIfNum(pimsmMapCbPtr->familyType, i, &intIfNum)
            != L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
            " Failed to find internal interface number for rtrIfNum = %d",
             i);
          return L7_FAILURE;
        }
        L7_INTF_SETMASKBIT(*outIntIfMask, intIfNum);
      }
    }
   return L7_SUCCESS;   
}


/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    pimsmMapCbPtr        @b{(input)}    Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapExtenStarGOIFGet(pimsmMapCB_t *pimsmMapCbPtr,
                                           L7_inet_addr_t *ipMRouteGroup,
                                           L7_INTF_MASK_t      *outIntIfMask)
{
   interface_bitset_t rtrOif;
   L7_uint32 i, intIfNum;
   L7_RC_t rc;
   
   memset(&rtrOif, 0, sizeof(interface_bitset_t));
   rc = pimsmStarGEntryOIFGet(pimsmMapCbPtr->cbHandle, ipMRouteGroup,
        &rtrOif);
   if (rc != L7_SUCCESS)
   {
      return rc;
   }
    for (i = 1; i < MCAST_MAX_INTERFACES; i++)
    {
      if (BITX_TEST(&rtrOif, i)!= L7_NULL)
      {
        if (mcastIpMapRtrIntfToIntIfNum(pimsmMapCbPtr->familyType, i, &intIfNum)
            != L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
            " Failed to find internal interface number for rtrIfNum = %d",
             i);
          return L7_FAILURE;
        }
        L7_INTF_SETMASKBIT(*outIntIfMask, intIfNum);
      }
    }
   return L7_SUCCESS;
}

/***************************************************************
* @purpose  Gets number of forwarded packets for the given Index
*           through the router
*
* @param    pimsmMapCbPtr        @b{(input)}    Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    numOfPackets         @b{(output)}   Number of Forwarded Packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
***************************************************************/
L7_RC_t pimsmMapExtenRoutePktsGet(pimsmMapCB_t *pimsmMapCbPtr,
                                  L7_inet_addr_t *ipMRouteGroup,
                                  L7_inet_addr_t *ipMRouteSource,
                                  L7_inet_addr_t *ipMRouteSourceMask,
                                  L7_uint32 *numOfPackets)
{
    return L7_NOT_SUPPORTED;
}
/*********************************************************************
* @purpose  Check whether RP Group entry exists for the specified
*           group address, group mask, address and component number
*
* @param    pimsmMapCbPtr       @b{(input)}    Mapping Control Block.
* @param    origin              @b{(input)}    Origin
* @param    rpGroupAddress      @b{(input)}    IP Multicast group address
* @param    rpGroupMask         @b{(input)}    Multicast group address mask
* @param    rpAddress           @b{(input)}    IP address of candidate RP
*
* @returns  L7_SUCCESS          if entry exists
* @returns  L7_FAILURE          if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRpGroupEntryGet(pimsmMapCB_t   *pimsmMapCbPtr, 
                                   L7_uchar8      origin, 
                                   L7_inet_addr_t *rpGroupAddress,
                                   L7_inet_addr_t *rpGroupMask, 
                                   L7_inet_addr_t *rpAddress)
{
    L7_uchar8 prefixLength = L7_NULL;
    L7_uchar8 rpMask[IPV6_DISP_ADDR_LEN];
 
    if (inetMaskToMaskLen(rpGroupMask, &prefixLength) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Failed to convert Mask %s to MaskLen",
                        inetAddrPrint(rpGroupMask,rpMask));
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: inetMaskToMaskLen failed.\n");
      return L7_FAILURE;
    }
    return pimsmRpGrpMappingEntryGet(pimsmMapCbPtr->cbHandle, origin, 
                    rpGroupAddress, prefixLength, rpAddress);
}

/*********************************************************************
* @purpose  Get the existing RP Set entry next to the entry of the specified
*           group address, group mask, address and component number
*
* @param    pimsmMapCbPtr       @b{(input)}   Mapping Control Block.
* @param    origin              @b{(input)}   Origin
* @param    rpGroupAddress      @b{(input)}   IP Multicast group address
* @param    rpGroupMask         @b{(input)}   Multicast group address mask
* @param    rpAddress           @b{(inout)}   IP address of candidate RP
*
* @returns  L7_SUCCESS           if entry exists
* @returns  L7_FAILURE           if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRpGroupEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr, 
                   L7_uchar8 *origin, L7_inet_addr_t *rpGroupAddress, 
                   L7_inet_addr_t *rpGroupMask, L7_inet_addr_t *rpAddress)
{
    L7_uchar8 prefixLength = L7_NULL;
    L7_RC_t   rc = L7_FAILURE;
    L7_uchar8 rpMask[IPV6_DISP_ADDR_LEN];

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (inetMaskToMaskLen(rpGroupMask, &prefixLength) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Failed to convert Mask %s to MaskLen",
                        inetAddrPrint(rpGroupMask,rpMask));
      return L7_FAILURE;
    }
    rc = pimsmRpGrpMappingEntryNextGet(pimsmMapCbPtr->cbHandle,
                        origin, rpGroupAddress,
                        &prefixLength, rpAddress);
    if (rc == L7_SUCCESS)
    {
      if (inetMaskLenToMask(pimsmMapCbPtr->familyType, 
                            prefixLength, rpGroupMask) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Failed to convert Mask Len %d to Subnet Mask",
                         prefixLength);
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: inetMaskLenToMask failed\n");
        return L7_FAILURE;
      }
    }
    return rc;
}

/*********************************************************************
* @purpose  Get the existing component elected bsr entry next. 
*
* @param    pimsmMapCbPtr     @b{(inout)}   Mapping Control Block.
*
* @returns  L7_SUCCESS   if entry exists
* @returns  L7_FAILURE   if entry does not exist
*
* @comments Currently there is no BSR Component table to support multiple
*           domains since only one domain is being supported.
*           Hence there is only index = 1
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenElectedBSREntryNextGet(pimsmMapCB_t *pimsmMapCbPtr)
{
    return pimsmBsrElectedBSREntryNextGet(pimsmMapCbPtr->cbHandle);
}

/*********************************************************************
* @purpose  Obtain the RP that the specified group will be mapped to.
*
* @param    pimsmMapCbPtr  @b{(input)}  Mapping Control Block.
* @param    grpIpAddr      @b{(input)}  Group ip address
* @param    origin         @b{(input)}  Origin
* @param    rpIpAddr       @b{(output)} RP ip address
*
* @returns  L7_SUCCESS  If group is successfully mapped to RP.
* @returns  L7_FAILURE  If pimsm is not operational or output parm is null.
* @returns  L7_FAILURE  If no RP can be matched to the group.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenGroupToRPMappingGet(pimsmMapCB_t *pimsmMapCbPtr,
                                         L7_inet_addr_t *grpIpAddr,
                                         L7_inet_addr_t *rpIpAddr, 
                                         L7_uchar8 *origin)
{
  return pimsmRpHashGet(pimsmMapCbPtr->cbHandle, 
                                       grpIpAddr, rpIpAddr, origin);
}

/*********************************************************************
* @purpose  Set the RP Candidate for the specified interface
*
* @param    pimsmMapCbPtr     @b{(input)} Mapping Control Block.
* @param    rpAddr            @b{(input)} RP Address.
* @param    rpGrpAddr         @b{(input)} RP Group Address.
* @param    rpGrpMask         @b{(input)} RP Group Mask.
* @param    mode              @b{(input)} mode.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceCandRPSet(pimsmMapCB_t *pimsmMapCbPtr, 
                                        L7_uint32 intIfNum,
                                        L7_inet_addr_t *rpAddr, 
                                        L7_inet_addr_t *rpGrpAddr, 
                                        L7_inet_addr_t *rpGrpMask,
                                        L7_uint32 mode)
{
  pimsmCandRPdata_t  rpCandidate;
  L7_uchar8          maskLen;
  L7_uchar8 rpMask[IPV6_DISP_ADDR_LEN];
  L7_BOOL rpMode;
  L7_uint32 rtrIfNum = 0;
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType,
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"invalid interface number(%d)", intIfNum);
     return L7_FAILURE;  
  }

  if( mode == L7_ENABLE) 
  {
   rpMode = L7_TRUE;
  }
  else if( mode == L7_DISABLE) 
  {
   rpMode = L7_FALSE;
  }
  else
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"wrong rp-mode (%d)", mode);
     return L7_FAILURE;   
  }
  
  rpCandidate.pCandRPAddress = rpAddr;
  rpCandidate.pGrpAddress = rpGrpAddr;
  if (inetMaskToMaskLen(rpGrpMask, &maskLen) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to convert Mask %s to MaskLen",
                      inetAddrPrint(rpGrpMask,rpMask));
    return L7_FAILURE;
  }
  rpCandidate.operMode = rpMode;
  rpCandidate.intIfNum = rtrIfNum;
  rpCandidate.grpPrefixLength = maskLen;

  /* Proces PIMSM RP Candidate Set */
  if (pimsmCRPProcess(pimsmMapCbPtr->cbHandle, &rpCandidate) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the BSR Candidate for the specified interface
*
* @param    pimsmMapCbPtr    @b{(input)}    Mapping Control Block.
* @param    bsrAddr          @b{(input)}    BSR Address
* @param    priority         @b{(input)}    Priority
* @param    mode             @b{(input)}    mode (ENABLE/DISABLE).
* @param    length           @b{(input)}    maskLen
* @param    scope            @b{(input)}    scope Value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceCandBSRSet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 intIfNum,
                 L7_inet_addr_t *bsrAddr, L7_uint32 priority, L7_uint32 mode, 
                 L7_uint32 length, L7_uint32 scope)
{
  pimsmCandBSRdata_t bsrCandidate;
  L7_BOOL bsrMode = L7_FALSE;
  L7_uint32 rtrIfNum = 0;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType,
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"invalid interface number(%d)", intIfNum);
     return L7_FAILURE;  
  }

  if( mode == L7_ENABLE) 
  {
   bsrMode = L7_TRUE;
  }
  else if( mode == L7_DISABLE) 
  {
   bsrMode = L7_FALSE;
  }
  else
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"wrong bsr-mode (%d)", mode);
     return L7_FAILURE;   
  }
  bsrCandidate.operMode = bsrMode;
  bsrCandidate.intIfNum = rtrIfNum;
  bsrCandidate.candBSRPriority = priority;
  bsrCandidate.candBSRHashMaskLength = length;
  bsrCandidate.candBSRScope = scope;
  bsrCandidate.pCandBSRAddress = bsrAddr;

  /* Process PIM SM BSR Candidate */
  if (pimsmCBSRProcess(pimsmMapCbPtr->cbHandle, &bsrCandidate) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the DR Priority for the specified interface
*
* @param    pimsmMapCbPtr     @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    priority          @b{(input)} priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceDRPrioritySet(pimsmMapCB_t *pimsmMapCbPtr,
                                 L7_uint32 intIfNum, L7_uint32 priority)
{
  L7_uint32 rtrIfNum = L7_NULL;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Error in converting IntIfNum(%d) to Rtr Num",
                      intIfNum);
    return L7_FAILURE;
  }

 /*  Process PIM SM DR Priority Set  */
  if (pimsmDRPriorityProcess(pimsmMapCbPtr->cbHandle,
                             rtrIfNum) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the hello interval for the specified interface
*
* @param    pimsmMapCbPtr      @b{(input)} Mapping Control Block.
* @param    intIfNum           @b{(input)} Internal Interface Number
* @param    helloIntvl         @b{(input)} hello interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceHelloIntervalSet(pimsmMapCB_t *pimsmMapCbPtr,
                                               L7_uint32 intIfNum, 
                                               L7_uint32 helloIntvl)
{
  L7_uint32 rtrIfNum = L7_NULL;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered.\n");

  if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Error in converting IntIfNum(%d) to Rtr Num",
                      intIfNum);
    return L7_FAILURE;
  }

  /* Process PIMSM Hello interval Set */
  if (pimsmHelloIntervalProcess(pimsmMapCbPtr->cbHandle,
                                rtrIfNum, helloIntvl) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the join prune interval for the specified interface
*
* @param    pimsmMapCbPtr      Mapping Control Block.
* @param    intIfNum           @b{(input)} Internal Interface Number
* @param    joinPruneIntvl     @b{(input)} join-prune interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceJoinPruneIntervalSet(pimsmMapCB_t *pimsmMapCbPtr,
                                                   L7_uint32 intIfNum, 
                                                   L7_uint32 joinPruneIntvl)
{
  L7_uint32 rtrIfNum = L7_NULL;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "Function Entered\n");

  if (mcastIpMapIntIfNumToRtrIntf(pimsmMapCbPtr->familyType, 
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Error in converting IntIfNum(%d) to Rtr Num",
                      intIfNum);
    return L7_FAILURE;
  }

 /* Process PIM SM Join prune interval Set */
  if (pimsmJoinPruneIntervalProcess(pimsmMapCbPtr->cbHandle,
                       rtrIfNum, joinPruneIntvl) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the Spt Threshold  
*
* @param    pimsmMapCbPtr    @b{(input)}    Mapping Control Block.
* @param    threshold        @b{(input)}    threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenDataThresholdRateSet(pimsmMapCB_t *pimsmMapCbPtr, 
                                                       L7_uint32 threshold)
{
  L7_uint32 sptThreshold = threshold; 

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

 /* Process PIMSM Threshold Rate Set */
  if (pimsmDataThresholdRateSet(pimsmMapCbPtr->cbHandle,
                                sptThreshold) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the register threshold rate.  
*
* @param    pimsmMapCbPtr   @b{(input)}     Mapping Control Block.
* @param    threshold       @b{(input)}     threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenRegisterThresholdRateSet(pimsmMapCB_t *pimsmMapCbPtr, 
                                                       L7_uint32 threshold)
{
  L7_uint32 regThreshold = threshold; 

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

 /* Process  PIMSM Register Threshold Rate Set  */
  if (pimsmRegisterThresholdRateSet(pimsmMapCbPtr->cbHandle,
                                    regThreshold) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}
#ifdef PIMSM_MAP_TBD
/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    pimsmMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}   Group Address.
* @param    ipMRouteSource       @b{(input)}   Source Address.
* @param    ipMRouteSourceMask   @b{(input)}   Source Mask.
* @param    rpfAddr              @b{(output)}  rpfAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteRtAddrGet(pimsmMapCB_t *pimsmMapCbPtr,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rpfAddr)
{
#ifdef PIMSM_MAP_TBD
  return pimsmIpMRouteRtAddrGet(pimsmMapCbPtr->cbHandle, ipMRouteGroup, 
                                ipMRouteSource, ipMRouteSourceMask, rpfAddr);
#endif
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    pimsmMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}   Group Address.
* @param    ipMRouteSource       @b{(input)}   Source Address.
* @param    ipMRouteSourceMask   @b{(input)}   Source Mask.
* @param    rtMask               @b{(output)}  rtMask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteRtMaskGet(pimsmMapCB_t *pimsmMapCbPtr,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rtMask)
{
#ifdef PIMSM_MAP_TBD
  return pimsmIpMRouteRtMaskGet(pimsmMapCbPtr->cbHandle, ipMRouteGroup, 
                                ipMRouteSource, ipMRouteSourceMask, rtMask);
#endif
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Addr.
*
* @param    pimsmMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}   Group Address.
* @param    ipMRouteSource       @b{(input)}   Source Address.
* @param    ipMRouteSourceMask   @b{(input)}   Source Mask.
* @param    rpfAddr              @b{(output)}  rpfAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMRouteRpfAddrGet(pimsmMapCB_t *pimsmMapCbPtr, 
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t *rpfAddr)
{
#ifdef PIMSM_MAP_TBD
  return pimsmIpMRouteRpfAddrGet(pimsmMapCbPtr->cbHandle, ipMRouteGroup,
                                 ipMRouteSource, ipMRouteSourceMask, rpfAddr);
#endif
    return L7_NOT_SUPPORTED;
}

#endif
/*********************************************************************
* @purpose  Gets the Elected BSR Priority. 
*
* @param    pimsmMapCbPtr    @b{(input)}  Mapping Control Block.
* @param    elecBsrPriority  @b{(output)} Elected BSR Priority.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenElectedBsrPriroityGet(pimsmMapCB_t *pimsmMapCbPtr, 
                                           L7_int32 *elecBsrPriority)
{
    return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle,
                         I_bsrElectedBSRPriority, elecBsrPriority);

}

/*********************************************************************
* @purpose  Gets the Elected BSR Hash Mask Length. 
*
* @param    pimsmMapCbPtr    @b{(input)}  Mapping Control Block.
* @param    hashMaskLen      @b{(output)} Elected BSR Hash Mask Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenElectedBsrHashMaskLengthGet(pimsmMapCB_t *pimsmMapCbPtr, 
                                           L7_int32 *hashMaskLen)
{
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle,
                         I_bsrElectedBSRHashMaskLength, hashMaskLen);
}

/* (S,G), (*,G) Tables  functions */


/*********************************************************************
* @purpose  To get the expiry time in SG TABLE.
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pExpiryTime   @b{(output)}expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGExpiryTimeGet(pimsmMapCB_t *pimsmMapCbPtr,  
                                     L7_inet_addr_t *pSGGrpAddr, 
                                     L7_inet_addr_t *pSGSrcAddr,
                                     L7_uint32      *pExpiryTime)
{
    return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pSGGrpAddr, 
                            pSGSrcAddr, I_pimsmSGKeepaliveTimer, pExpiryTime);
}

/*********************************************************************
* @purpose  To get the up time in SG TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pSGGrpAddr    @b{(input)}  Group Address
* @param    pSGSrcAddr    @b{(input)}  Source Address
* @param    pUpTime       @b{(output)} Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr,  
                                 L7_inet_addr_t *pSGGrpAddr, 
                                 L7_inet_addr_t *pSGSrcAddr,
                                 L7_uint32      *pUpTime)
{
    return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pSGGrpAddr, 
                            pSGSrcAddr, I_pimsmSGUpTime, pUpTime);
}

/*********************************************************************
* @purpose  To get the Rpf Address in SG TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pSGGrpAddr    @b{(input)}  Group Address
* @param    pSGSrcAddr    @b{(input)}  Source Address
* @param    pRpfAddr      @b{(output)} RPF Address.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRpfAddrGet(pimsmMapCB_t  *pimsmMapCbPtr,  
                                 L7_inet_addr_t *pSGGrpAddr, 
                                 L7_inet_addr_t *pSGSrcAddr,
                                 L7_inet_addr_t *pRpfAddr)
{
    return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pSGGrpAddr, 
                            pSGSrcAddr, I_pimsmSGUpstreamNeighbor, 
                            (L7_uint32 *)pRpfAddr);
}

/*********************************************************************
* @purpose  To get the Flags in SG TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pSGGrpAddr    @b{(input)}  Group Address
* @param    pSGSrcAddr    @b{(input)}  Source Address
* @param    pSGFalgs      @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGFlagsGet(pimsmMapCB_t  *pimsmMapCbPtr,  
                                L7_inet_addr_t *pSGGrpAddr, 
                                L7_inet_addr_t *pSGSrcAddr,
                                L7_uint32      *pSGFlags)
{
  L7_RC_t rc;
  L7_BOOL sptBit = L7_FALSE;

   *pSGFlags = L7_MCAST_PIMSM_FLAG_RPT;
   rc = pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pSGGrpAddr, 
                            pSGSrcAddr, I_pimsmSGSPTBit, 
                            &sptBit);
   if ( rc == L7_SUCCESS)
   {
      if (sptBit == L7_TRUE) 
      {
         *pSGFlags = L7_MCAST_PIMSM_FLAG_SPT;
      }
   }
   return rc;
}

/*********************************************************************
* @purpose  To get the IfIndex in SG TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pSGGrpAddr    @b{(input)}  Group Address
* @param    pSGSrcAddr    @b{(input)}  Source Address
* @param    pSGIfIndex    @b{(output)} SG If Index.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIfIndexGet(pimsmMapCB_t  *pimsmMapCbPtr,  
                                L7_inet_addr_t *pSGGrpAddr, 
                                L7_inet_addr_t *pSGSrcAddr,
                                L7_uint32      *pSGIfIndex)
{
   L7_uint32 rtrIfNum = 0,intIfNum = 0;
   L7_RC_t rc;
   
    rc = pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pSGGrpAddr, 
                            pSGSrcAddr, I_pimsmSGRPFIfIndex, 
                            &rtrIfNum);
    if (rc == L7_SUCCESS)
    {
      if (rtrIfNum != 0)
      {
        if (mcastIpMapRtrIntfToIntIfNum(pimsmMapCbPtr->familyType, rtrIfNum,
               &intIfNum)
            != L7_SUCCESS)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
            " Failed to find internal interface number for rtrIfNum = %d",
             rtrIfNum);
          return L7_FAILURE;
        }
      }
    }
    *pSGIfIndex = intIfNum;
    return rc;
}

/*********************************************************************
* @purpose  To get the next entry in Star G TABLE.
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    pGrpAddr      @b{(inout)} Group Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr,  
                                       L7_inet_addr_t *pGrpAddr)
{
    return pimsmStarGEntryNextGet(pimsmMapCbPtr->cbHandle, 
                                  pGrpAddr);
}


/*********************************************************************
* @purpose  To get the expiry time in Star G TABLE.
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    pGrpAddr      @b{(input)} Group Address
* @param    pExpiryTime   @b{(output)}expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGExpiryTimeGet(pimsmMapCB_t   *pimsmMapCbPtr,  
                                        L7_inet_addr_t *pGrpAddr, 
                                        L7_uint32      *pExpiryTime)
{
    return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pGrpAddr, 
                               I_pimsmStarGUpstreamJoinTimer, pExpiryTime);
}

/*********************************************************************
* @purpose  To get the up time in Star G TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pUpTime       @b{(output)} Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr,  
                                    L7_inet_addr_t *pGrpAddr, 
                                    L7_uint32      *pUpTime)
{
    return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pGrpAddr, 
                               I_pimsmStarGUpTime, pUpTime);
}

/*********************************************************************
* @purpose  To get the Rpf Address in Star G TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pRpfAddr      @b{(output)} RPF Address.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRpfAddrGet(pimsmMapCB_t   *pimsmMapCbPtr,  
                                     L7_inet_addr_t *pGrpAddr, 
                                     L7_inet_addr_t *pRpfAddr)
{
    return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pGrpAddr, 
                               I_pimsmStarGUpstreamNeighbor, 
                               (L7_uint32 *)pRpfAddr);
}

/*********************************************************************
* @purpose  To get the Flags in Star G TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pFalgs        @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGFlagsGet(pimsmMapCB_t   *pimsmMapCbPtr,  
                                   L7_inet_addr_t *pGrpAddr, 
                                   L7_uint32      *pFlags)
{
  L7_RC_t rc;

   rc = pimsmStarGEntryGet(pimsmMapCbPtr->cbHandle, pGrpAddr);
   if ( rc == L7_SUCCESS)
   {
      *pFlags = L7_MCAST_PIMSM_FLAG_RPT;
   }
   return rc;
}

/*********************************************************************
* @purpose  To get the IfIndex in Star G TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pIfIndex      @b{(output)} If Index.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIfIndexGet(pimsmMapCB_t  *pimsmMapCbPtr,  
                                     L7_inet_addr_t *pGrpAddr, 
                                     L7_uint32      *pIfIndex)
{
  L7_uint32 rtrIfNum = 0,intIfNum = 0;
  L7_RC_t rc;
  
   rc = pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pGrpAddr, 
                            I_pimsmStarGRPFIfIndex,
                           &rtrIfNum);
   if (rc == L7_SUCCESS)
   {
     if (rtrIfNum == L7_PIMSM_REGISTER_INTF)
     {
      intIfNum = 0;
     } 
     else if (mcastIpMapRtrIntfToIntIfNum(pimsmMapCbPtr->familyType, rtrIfNum,
            &intIfNum)
         != L7_SUCCESS)
     {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
         " Failed to find internal interface number for rtrIfNum = %d",
          rtrIfNum);
       return L7_FAILURE;
     }
   }
   *pIfIndex = intIfNum;
   return rc;
    
}
#ifdef PIMSM_MAP_TBD
/* Star Star RP Table API's */
/*********************************************************************
* @purpose  To get the next entry in Star Star RP TABLE.
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    pRpAddr       @b{(inout)} RP Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarStarRPEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr,  
                                            L7_inet_addr_t *pRpAddr)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Obtain the next incoming interface in the Star Star RP TABLE.
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    pRpAddr       @b{(inout)} RP Address
* @param    pIntIfNum     @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarStarRPIEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr,  
                                             L7_inet_addr_t *pRpAddr, 
                                             L7_uint32      *pIntIfNum)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  To get the expiry time in Star Star RP TABLE.
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    pRpAddr       @b{(input)} RP Address
* @param    pExpiryTime   @b{(output)}expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarStarRPExpiryTimeGet(pimsmMapCB_t   *pimsmMapCbPtr,  
                                             L7_inet_addr_t *pRpAddr, 
                                             L7_uint32      *pExpiryTime)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  To get the up time in Star Star RP TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pRpAddr       @b{(input)}  RP Address
* @param    pUpTime       @b{(output)} Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarStarRPUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr,  
                                         L7_inet_addr_t *pRpAddr, 
                                         L7_uint32      *pUpTime)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  To get the Rpf Address in Star Star RP TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pRpAddr       @b{(input)}  RP Address
* @param    pRpfAddr      @b{(output)} RPF Address.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarStarRPRpfAddrGet(pimsmMapCB_t   *pimsmMapCbPtr,  
                                          L7_inet_addr_t *pRpAddr, 
                                          L7_inet_addr_t *pRpfAddr)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  To get the Flags in Star Star RP TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pRpAddr       @b{(input)}  RP Address
* @param    pFalgs        @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarStarRPFlagsGet(pimsmMapCB_t   *pimsmMapCbPtr,  
                                        L7_inet_addr_t *pRpAddr, 
                                        L7_uint32      *pFalgs)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  To get the IfIndex in Star Star RP TABLE.
*
* @param    pimsmMapCbPtr @b{(input)}  Mapping Control Block.
* @param    pRpAddr       @b{(input)}  RP Address
* @param    pIfIndex      @b{(output)} If Index.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarStarRPIfIndexGet(pimsmMapCB_t  *pimsmMapCbPtr,  
                                          L7_inet_addr_t *pRpAddr, 
                                          L7_uint32      *pIfIndex)
{
    return L7_NOT_SUPPORTED;
}

 #endif





  /************************************************************************
 *  New implementation for the draft-ietf-pim-mib-v2-03.txt follows
 ************************************************************************/

/*********************************************************************
* @purpose  Gets the inetAddressType for the specified interface.
*
* @param    intIfNum     Internal Interface number
* @param    familyType    IPV4/V6
* @param    familyType     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceAddressTypeGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 rtrIfNum,  L7_int32 *addrType)
{
  return pimsmInterfaceObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum, I_pimsmInterfaceAddressType, addrType);
}


/*********************************************************************
* @purpose  Gets the Generation ID value for the specified interface.
*
* @param    rtrIfNum @b{(pimsmMapCB_t *pimsmMapCbPtr, input)}  internal Interface number
* @param    familyType    IPV4/V6
* @param    *ipAddr   @b{(pimsmMapCB_t *pimsmMapCbPtr, output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceGenerationIDValueGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 rtrIfNum, L7_uint32 *genIDValue)
{
  return pimsmInterfaceObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum, I_pimsmInterfaceGenerationIDValue, genIDValue);
}

/*********************************************************************
* @purpose  Sets the UseDRPriority option for this interface
*
* @param    rtrIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    useDRPriority   Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceUseDRPrioritySet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 rtrIfNum, L7_int32 useDRPriority)
{

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Gets the UseDRPriority option for this interface
*
* @param    rtrIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    *useDRPriority  Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceUseDRPriorityGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 rtrIfNum, L7_int32 *useDRPriority)
{
  return pimsmInterfaceObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum, I_pimsmInterfaceUseDRPriority, useDRPriority);
}


/*********************************************************************
* @purpose Gets the dRPriority option for this interface
*
* @param    rtrIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    *dRPriority  Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceDRPriorityGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 rtrIfNum, L7_uint32 *dRPriority)
{
  return pimsmInterfaceObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum, I_pimsmInterfaceDRPriority, dRPriority);
}

/*********************************************************************
* @purpose  Sets the UseDRPriority option for this interface
*
* @param    rtrIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    useDRPriority   Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceBSRBorderSet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 rtrIfNum, L7_int32 bSRBorder)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the BSRBorder option for this interface
*
* @param    rtrIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    *bSRBorder      BSRBorder option (pimsmMapCB_t *pimsmMapCbPtr, TRUE/FALSE)
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenInterfaceBSRBorderGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 rtrIfNum, L7_int32 *bSRBorder)
{
  return pimsmInterfaceObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum, I_pimsmInterfaceBSRBorder, bSRBorder);
}

/************ END OF PIMSM INTERFACE TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  To get if the neighbor DR Priority is present.
*
* @param    rtrIfNum    internal Interface number
* @param    familyType   IP Version
* @param    *inetIPAddr   IP Address
* @param    *bNbrDRPrio   Is Nbr Priority Set.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenNeighborDRPriorityPresentGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 rtrIfNum, L7_inet_addr_t *inetIPAddr, L7_uint32 *bNbrDRPrio)
{
  return pimsmNeighborObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum, inetIPAddr, I_pimsmNeighborDRPriorityPresent, bNbrDRPrio);
}

/*********************************************************************
* @purpose  To get the neighbor DR Priority.
*
* @param    rtrIfNum    internal Interface number
* @param    familyType   IP Version
* @param    *inetIPAddr   IP Address
* @param    *nbrDRPrio   Nbr Priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenNeighborDRPriorityGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uint32 rtrIfNum, L7_inet_addr_t *inetIPAddr, L7_uint32 *nbrDRPrio)
{
  return pimsmNeighborObjectGet(pimsmMapCbPtr->cbHandle, rtrIfNum, inetIPAddr, I_pimsmNeighborDRPriority, nbrDRPrio);
}

/************ END OF PIMSM NEIGHBOUR TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SG entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGEntryGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
  return pimsmSGEntryGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SG TABLE
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
  return pimsmSGEntryNextGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr);
}


/*********************************************************************
* @purpose  To get the SG PimMode.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGPimModeGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGpimMode, sgUpTime);
}

/*********************************************************************
* @purpose  To get the SG UpstreamJoinState
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGUpstreamJoinStateGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGUpstreamJoinState, sgUpTime);
}

/*********************************************************************
* @purpose  To get the SG UpstreamJoinTimer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGUpstreamJoinTimerGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGUpstreamJoinTimer, sgUpTime);
}
/*********************************************************************
* @purpose  To get the SG RPF UpstreamNeighbor.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGUpstreamNeighborGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGUpstreamNeighbor, (L7_uint32*)rpfNextHop);
}

/*********************************************************************
* @purpose  To get the SG RPFIfIndex.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPFIfIndexGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRPFIfIndex, sgSPTBit);
}

/*********************************************************************
* @purpose  To get the SG RPFRouteProtocol.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPFRouteProtocolGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRPFRouteProtocol, sgSPTBit);
}

/*********************************************************************
* @purpose  To get the SG RPF RPFRouteAddress.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPFRouteAddressGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRPFRouteAddress, (L7_uint32*)rpfNextHop);
}

/*********************************************************************
* @purpose  To get the SG RPFRoutePrefixLength.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPFRoutePrefixLengthGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRPFRoutePrefixLength, sgSPTBit);
}

/*********************************************************************
* @purpose  To get the SG RPFRouteMetricPref.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPFRouteMetricPrefGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRPFRouteMetricPref, sgSPTBit);
}

/*********************************************************************
* @purpose  To get the SG RPFRouteMetric.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPFRouteMetricGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRPFRouteMetric, sgSPTBit);
}

/*********************************************************************
* @purpose  To get the SG KeepaliveTimer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGKeepaliveTimerGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGKeepaliveTimer, sgSPTBit);
}

/*********************************************************************
* @purpose  To get the SG DRRegisterState.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGDRRegisterStateGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGDRRegisterState, sgSPTBit);
}

/*********************************************************************
* @purpose  To get the SG DRRegisterStopTimer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGDRRegisterStopTimerGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGDRRegisterStopTimer, sgSPTBit);
}

/*********************************************************************
* @purpose  To get the SG RPF RPRegisterPMBRAddress
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPRegisterPMBRAddressGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRPRegisterPMBRAddress, (L7_uint32*)rpfNextHop);
}

/*********************************************************************
* @purpose  To get the SG RPF nextHop.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPFNextHopGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRPFNextHop, (L7_uint32*)rpfNextHop);
}

/*********************************************************************
* @purpose  To get the SG SPTBit.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGSPTBitGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  return pimsmSGObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGSPTBit, sgSPTBit);
}

/************ END OF PIMSM SG TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SGRpt entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRptEntryGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
  return pimsmSGRptEntryGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SGRpt TABLE
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRptEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
  return pimsmSGRptEntryNextGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr);
}

/*********************************************************************
* @purpose  To get the SGRPT uptime.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRptUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgRPTUpTime)
{
  return pimsmSGRptObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRptUpTime, sgRPTUpTime);
}

/*********************************************************************
* @purpose  To get the SGRPT UpstreamPruneState.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPTUpstreamPruneStateGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_int32 *sgRPTUpTime)
{
  return pimsmSGRptObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRptUpstreamPruneState, sgRPTUpTime);
}

/*********************************************************************
* @purpose  To get the SGRPT UpstreamOverride Timer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRPTUpstreamOverrideTimerGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgRPTUpTime)
{
  return pimsmSGRptObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, I_pimsmSGRptUpstreamOverrideTimer, sgRPTUpTime);
}
/************ END OF PIMSM SGRPT TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether Group mapping entry exists for the specified index.
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenGroupMappingEntryGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uchar8 origin,  L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr)
{
  return pimsmRpGrpMappingEntryGet(pimsmMapCbPtr->cbHandle, origin, pimMappingGrpAddr, length, pimMappingRPAddr);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the Group mapping TABLE
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenGroupMappingEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uchar8 *origin, L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 *length, L7_inet_addr_t *pimMappingRPAddr)
{
  return pimsmRpGrpMappingEntryNextGet(pimsmMapCbPtr->cbHandle,origin, pimMappingGrpAddr, length, pimMappingRPAddr);
}

/*********************************************************************
* @purpose  To get the groupMapping pim mode
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    pimGroupMappingMode  Group mapping mode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenGroupMappingPimModeGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uchar8 origin,  L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr, L7_uint32 *pimGroupMappingMode)
{
  return pimsmRpGrpMappingEntryObjectGet(pimsmMapCbPtr->cbHandle, origin, pimMappingGrpAddr, length, pimMappingRPAddr, I_pimsmGroupMappingpimMode, pimGroupMappingMode);
}

/************ END OF PIMSM GROUP MAPPING TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether bsrCandidateBSR entry exists for the specified index.
*
* @param    familyType             whether IPV4/V6
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrElectedBSREntryGet(pimsmMapCB_t *pimsmMapCbPtr) 
{
  return pimsmBsrElectedBSREntryGet(pimsmMapCbPtr->cbHandle);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the bsrCandidateRP TABLE
*
* @param    familyType             whether IPV4/V6
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrElectedBSREntryNextGet(pimsmMapCB_t *pimsmMapCbPtr)
{
  return pimsmBsrElectedBSREntryNextGet(pimsmMapCbPtr->cbHandle);
}

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrElectedBSRAddr    To get the bsrElectedBSR Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrElectedBSRAddrGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_uint32 *bsrElectedBSRAddr)
{
  return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle, I_bsrElectedBSRAddress, bsrElectedBSRAddr);
}

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrElectedBSRPriority  To get the bsrElectedBSR Priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrElectedBSRPriorityGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_uint32 *bsrElectedBSRPriority)
{
  return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle, I_bsrElectedBSRPriority, bsrElectedBSRPriority);
}
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    hashMaskLen  To get the bsrElectedBSR hashMaskLen
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrElectedBSRHashMaskLengthGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_uint32 *hashMaskLen)
{
  return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle, I_bsrElectedBSRHashMaskLength, hashMaskLen);
}
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrElectedBSRExpiryTime  To get the bsrElectedBSR ExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrElectedBSRExpiryTimeGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_uint32 *bsrElectedBSRExpiryTime)
{
  return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle, I_bsrElectedBSRExpiryTime, bsrElectedBSRExpiryTime);
}
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrCRPAdvTimer  To get the bsrElectedBSR AdvTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrElectedBSRCRPAdvTimerGet(pimsmMapCB_t *pimsmMapCbPtr,  L7_uint32 *bsrCRPAdvTimer)
{
  return pimsmBsrElectedBSRObjectGet(pimsmMapCbPtr->cbHandle, I_bsrElectedBSRCRPAdvTimer, bsrCRPAdvTimer);
}

/************ END OF PIMSM BSR Elected BSR TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether StarG entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGEntryGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr) 
{
  return pimsmStarGEntryGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the StarG TABLE
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
/*L7_RC_t pimsmMapExtenStarGEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr) 
{
  return pimsmStarGEntryNextGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr);
}*/

/*********************************************************************
* @purpose  To get the StarG uptime
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGUpTime    To get the pimStarGUpTime 
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
/*L7_RC_t pimsmMapExtenStarGUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGUpTime)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGUpTime, pimStarGUpTime);
}*/

/*********************************************************************
* @purpose  To get the StarG pimMode
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGpimMode   To get the pimStarGpimMode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGpimModeGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGpimMode)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGpimMode, pimStarGpimMode);
}

/*********************************************************************
* @purpose  To get the starG RP address.
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPAddr  To get the pimStarGRPAddr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPAddrGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *pimStarGRPAddr)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPAddress, (L7_uint32 *)pimStarGRPAddr);
}

/*********************************************************************
* @purpose  To get the StarGRPOrigin
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPOrigin  To get the StarGRPOrigin
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPOriginGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGRPOrigin)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPOrigin, pimStarGRPOrigin);
}
/*********************************************************************
* @purpose  To get the pimStarGRPIsLocal
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPIsLocal  To get the pimStarGRPIsLocal
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPIsLocalGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGRPIsLocal)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPIsLocal, pimStarGRPIsLocal);
}
/*********************************************************************
* @purpose  To get the JoinState
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    JoinState  To get the JoinState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGUpstreamJoinStateGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *JoinState)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGUpstreamJoinState, JoinState);
}

/*********************************************************************
* @purpose  To get the UpstreamJoinTimer
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGUpstreamJoinTimerGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *JoinTimer)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGUpstreamJoinTimer, JoinTimer);
}

/*********************************************************************
* @purpose  To get the UpstreamNeighbor
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    UpstreamNbr  To get the UpstreamNbr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGUpstreamNbrGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *UpstreamNbr)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGUpstreamNeighbor, (L7_uint32 *)UpstreamNbr);
}

/*********************************************************************
* @purpose  To get the RPFIfIndex
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFIfIndex  To get the RPFIfIndex
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPFIfIndexGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFIfIndex)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPFIfIndex, RPFIfIndex);
}

/*********************************************************************
* @purpose  To get the RPFNextHop
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFNextHop  To get the RPFNextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPFNextHopGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *RPFNextHop)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPFNextHop, (L7_uint32 *)RPFNextHop);
}

/*********************************************************************
* @purpose  To get the RPFRouteProtocol
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteProtocol  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPFRouteProtocolGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RouteProtocol)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPFRouteProtocol, RouteProtocol);
}

/*********************************************************************
* @purpose  To get the RPFRouteAddress
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteAddress  To get the RPFRouteAddress
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPFRouteAddrGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *RPFRouteAddr)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPFRouteAddress, (L7_uint32 *)RPFRouteAddr);
}

/*********************************************************************
* @purpose  To get the RPFRoutePrefixLength
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRoutePrefixLength  To get the RPFRoutePrefixLength
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPFRoutePrefixLenGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFRoutePrefixLen)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPFRoutePrefixLength, RPFRoutePrefixLen);
}

/*********************************************************************
* @purpose  To get the RPFRouteMetricPref
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteMetricPref  To get the RPFRouteMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPFRouteMetricPrefGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFRouteMetricPref)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPFRouteMetricPref, RPFRouteMetricPref);
}

/*********************************************************************
* @purpose  To get the RPFRouteMetric
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteMetric  To get the RPFRouteMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGRPFRouteMetricGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFRouteMetric)
{
  return pimsmStarGObjectGet(pimsmMapCbPtr->cbHandle, pimStarGGrpAddr, I_pimsmStarGRPFRouteMetric, RPFRouteMetric);
}
/************ END OF PIMSM STARG TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether StarGI entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr      StarGI group address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIEntryGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex) 
{
  return pimsmStarGIEntryGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr,IfIndex);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the StarGII TABLE
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGII group address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 *IfIndex) 
{
  return pimsmStarGIEntryNextGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, IfIndex);
}

/*********************************************************************
* @purpose  To get the StarGII uptime
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGII group address
* @param    pimStarGIUpTime    To get the pimStarGIUpTime 
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *pimStarGIUpTime)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, IfIndex, I_pimsmStarGIUpTime, pimStarGIUpTime);
}

/*********************************************************************
* @purpose  To get the starG LocalMembership
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGII group address
* @param    pimStarGIRPAddr  To get the pimStarGIRPAddr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGILocalMembershipGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *localMemShip)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, IfIndex, I_pimsmStarGILocalMembership, localMemShip);
}

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGII group address
* @param    JoinState  To get the JoinState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIJoinPruneStateGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *JoinState)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, IfIndex, I_pimsmStarGIJoinPruneState, JoinState);
}

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGII group address
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIPrunePendingTimerGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *JoinTimer)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, IfIndex, I_pimsmStarGIPrunePendingTimer, JoinTimer);
}

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGII group address
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIJoinExpiryTimerGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *JoinExpiry)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, IfIndex, I_pimsmStarGIJoinExpiryTimer, JoinExpiry);
}

/*********************************************************************
* @purpose  To get the AssertState
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGII group address
* @param    AssertState  To get the AssertState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIAssertStateGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *AssertState)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, IfIndex, I_pimsmStarGIAssertState, AssertState);
}

/*********************************************************************
* @purpose  To get the AssertTimer
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGII group address
* @param    AssertTimer  To get the AssertTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIAssertTimerGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *AssertTimer)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, IfIndex, I_pimsmStarGIAssertTimer, AssertTimer);
}

/*********************************************************************
* @purpose  To get the AssertWinnerAddr
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGII group address
* @param    AssertWinnerAddr  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIAssertWinnerAddrGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_inet_addr_t *AssertWinner)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, IfIndex, I_pimsmStarGIAssertWinnerAddress, (L7_uint32 *)AssertWinner);
}

/*********************************************************************
* @purpose  To get the AssertWinnerMetricPref
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr      StarGI group address
* @param    AssertWinnerMetricPref  To get the AssertWinnerMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIRPFRouteMetricPrefGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 ifindex, L7_uint32 *AssertWinnerMetricPref)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, ifindex, I_pimsmStarGIAssertWinnerMetricPref, AssertWinnerMetricPref);
}

/*********************************************************************
* @purpose  To get the AssertWinnerMetric
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr      StarGI group address
* @param    AssertWinnerMetric  To get the AssertWinnerMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenStarGIAssertWinnerMetricGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 ifindex, L7_uint32 *AssertWinnerMetric)
{
  return pimsmStarGIObjectGet(pimsmMapCbPtr->cbHandle, pimStarGIGrpAddr, ifindex, I_pimsmStarGIAssertWinnerMetric, AssertWinnerMetric);
}

/************ END OF PIMSM StarGI TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SGI entry exists for the specified index.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIEntryGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg)
{
  return pimsmSGIEntryGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SG TABLE
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 *ifindexg)
{
  return pimsmSGIEntryNextGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg);
}

/*********************************************************************
* @purpose  To get the SG uptime.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_uint32 *sgUpTime)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg, I_pimsmSGIUpTime, sgUpTime);
}

/*********************************************************************
* @purpose  To get the LocalMembership
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    LocalMembership    LocalMembership
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGILocalMembershipGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_int32 *LocalMembership)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg, I_pimsmSGILocalMembership, LocalMembership);
}

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinPruneState JoinPruneState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIJoinPruneStateGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_uint32 *JoinPruneState)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg, I_pimsmSGIJoinPruneState, JoinPruneState);
}

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIPrunePendingTimerGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                    L7_uint32 *JoinTimer)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, IfIndex, I_pimsmSGIPrunePendingTimer, JoinTimer);
}

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIJoinExpiryTimerGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,
                                   L7_uint32 *JoinExpiry)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, IfIndex, I_pimsmSGIJoinExpiryTimer, JoinExpiry);
}

/*********************************************************************
* @purpose  To get the AssertState
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertState  To get the AssertState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIAssertStateGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                   L7_uint32 *AssertState)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, IfIndex, I_pimsmSGIAssertState, AssertState);
}

/*********************************************************************
* @purpose  To get the AssertTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertTimer  To get the AssertTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIAssertTimerGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                      L7_uint32 *AssertTimer)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, IfIndex, I_pimsmSGIAssertTimer, AssertTimer);
}

/*********************************************************************
* @purpose  To get the AssertWinnerAddr
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerAddr  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIAssertWinnerAddrGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                    L7_inet_addr_t *AssertWinner)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, IfIndex, I_pimsmSGIAssertWinnerAddress, (L7_uint32 *)AssertWinner);
}

/*********************************************************************
* @purpose  To get the AssertWinnerMetricPref
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerMetricPref  To get the AssertWinnerMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIRPFRouteMetricPrefGet(pimsmMapCB_t *pimsmMapCbPtr,L7_inet_addr_t *pimSGGrpAddr,L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                   L7_uint32 *AssertWinnerMetricPref)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, IfIndex, I_pimsmSGIAssertWinnerMetricPref, AssertWinnerMetricPref);
}

/*********************************************************************
* @purpose  To get the AssertWinnerMetric
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerMetric  To get the AssertWinnerMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGIAssertWinnerMetricGet(pimsmMapCB_t *pimsmMapCbPtr,L7_inet_addr_t *pimSGGrpAddr,L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                           L7_uint32 *AssertWinnerMetric)
{
  return pimsmSGIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, IfIndex, I_pimsmSGIAssertWinnerMetric, AssertWinnerMetric);
}

/************ END OF PIMSM SGI TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SGRptI entry exists for the specified index.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRptIEntryGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg)
{
  return pimsmSGRptIEntryGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SGRptI TABLE
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRptIEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 *ifindexg)
{
  return pimsmSGRptIEntryNextGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg);
}

/*********************************************************************
* @purpose  To get the SG uptime.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRptIUpTimeGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_uint32 *sgUpTime)
{
  return pimsmSGRptIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg, I_pimsmSGRptIUpTime, sgUpTime);
}

/*********************************************************************
* @purpose  To get the LocalMembership
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    LocalMembership    LocalMembership
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRptILocalMembershipGet(pimsmMapCB_t *pimsmMapCbPtr,L7_inet_addr_t *pimSGGrpAddr,L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_int32 *LocalMembership)
{
  return pimsmSGRptIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg, I_pimsmSGRptILocalMembership, LocalMembership);
}

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinPruneState JoinPruneState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRptIJoinPruneStateGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_uint32 *JoinPruneState)
{
  return pimsmSGRptIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, ifindexg, I_pimsmSGRptIJoinPruneState, JoinPruneState);
}

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t 
pimsmMapExtenSGRptIPrunePendingTimerGet(pimsmMapCB_t *pimsmMapCbPtr,L7_inet_addr_t *pimSGGrpAddr,L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                    L7_uint32 *JoinTimer)
{
  return pimsmSGRptIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, IfIndex, I_pimsmSGRptIPrunePendingTimer, JoinTimer);
}

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenSGRptIJoinExpiryTimerGet(pimsmMapCB_t *pimsmMapCbPtr, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,
                                   L7_uint32 *JoinExpiry)
{
  return pimsmSGRptIObjectGet(pimsmMapCbPtr->cbHandle, pimSGGrpAddr, pimSGSrcAddr, IfIndex, I_pimsmSGRptIPruneExpiryTimer, JoinExpiry);
}

/*********************************************************************
* @purpose  Check whether bsrRPSet mapping entry exists for the specified index.
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrRPSetEntryGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uchar8 origin,  L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr)
{
  return pimsmBsrRPSetEntryGet(pimsmMapCbPtr->cbHandle, origin, pimMappingGrpAddr, length, pimMappingRPAddr);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the bsrRPSet mapping TABLE
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrRPSetEntryNextGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uchar8 *origin, L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 *length, L7_inet_addr_t *pimMappingRPAddr)
{
  return pimsmBsrRPSetEntryNextGet(pimsmMapCbPtr->cbHandle, origin, pimMappingGrpAddr, length, pimMappingRPAddr);
}

/*********************************************************************
* @purpose  To get the RPSetPriority
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetPriority  RPSetPriority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrRPSetPriorityGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uchar8 origin,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetPriority)
{
  return pimsmBsrRPSetEntryObjectGet(pimsmMapCbPtr->cbHandle, origin, pimMappingGrpAddr, length, pimMappingRPAddr, I_bsrRPSetPriority, RPSetPriority);
}

/*********************************************************************
* @purpose  To get the RPSetHoldtime
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetHoldtime        RPSetHoldtime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrRPSetHoldtimeGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uchar8 origin,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetHoldtime)
{
  return pimsmBsrRPSetEntryObjectGet(pimsmMapCbPtr->cbHandle, origin, pimMappingGrpAddr, length, pimMappingRPAddr, I_bsrRPSetHoldtime, RPSetHoldtime);
}

/*********************************************************************
* @purpose  To get the RPSetExpiryTime
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetExpiryTime        RPSetExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenbsrRPSetExpiryTimeGet(pimsmMapCB_t *pimsmMapCbPtr, L7_uchar8 origin,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetExpiryTime)
{
  return pimsmBsrRPSetEntryObjectGet(pimsmMapCbPtr->cbHandle, origin, pimMappingGrpAddr, length, pimMappingRPAddr, I_bsrRPSetExpiryTime, RPSetExpiryTime);
}
/*********************************************************************
* @purpose  To get the PIMSM MRT table entry count
*
* @param    pimsmMapCbPtr       @b{(input)} pimsm CB Handle
* @param    entryCount          @b{(input)} number of MRT entries
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE 
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapExtenIpMrouteEntryCountGet(pimsmMapCB_t *pimsmMapCbPtr,
                                           L7_uint32 *entryCount)
{
  return pimsmIpMrouteEntryCountGet(pimsmMapCbPtr->cbHandle, entryCount);
}

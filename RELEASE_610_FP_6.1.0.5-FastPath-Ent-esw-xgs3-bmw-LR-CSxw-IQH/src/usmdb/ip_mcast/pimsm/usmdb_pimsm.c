/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   usmdb_pimsm.c
*
* @purpose    Provide interface to pimsm API's for unitmgr components
*
* @component  usmdb
*
* @comments   none
*
* @create     04/05/2002
*
* @author     Nitish
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_pimsm_api.h"
#endif


#include "osapi.h"
#include "l3_mcast_commdefs.h"
#include "l7_pimsm_api.h"
#include "pimsm_debug_api.h"

/*********************************************************************
* @purpose  Sets the PIM-SM Source Specific Multicast (SSM)
*
* @param    UnitIndex        @b{(input)} unit Index. 
* @param    family           @b{(input)} Address Family type
* @param    ssmMode          @b{(input)} ssm Mode.
* @param    groupAddr        @b{(input)} Group Address.
* @param    groupMask        @b{(input)} Group Mask.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSsmRangeSet(L7_uint32 UnitIndex, L7_uchar8 family, 
                              L7_uint32 ssmMode, L7_inet_addr_t *groupAddr, 
                              L7_inet_addr_t *groupMask)
{
    return pimsmMapSsmRangeSet(family, ssmMode, groupAddr, groupMask);
}

/*********************************************************************
* @purpose  Sets the PIM-SM Admin mode
*
* @param    UnitIndex       @b{(input)}  unix Index.
* @param    family          @b{(input)}  family Type. 
* @param    mode            @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmAdminModeSet(L7_uint32 UnitIndex, 
                               L7_uchar8 family, L7_uint32 mode)
{
    return pimsmMapPimsmAdminModeSet(family, mode);
}

/*********************************************************************
* @purpose  Gets the PIM-SM Admin mode
*
* @param    UnitIndex       @b{(input)}  unix Index.
* @param    family          @b{(input)}  family Type. 
* @param    mode            @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmAdminModeGet(L7_uint32 UnitIndex, 
                               L7_uchar8 family, L7_uint32 *mode)
{
    return pimsmMapPimsmAdminModeGet(family, mode);
}

/*********************************************************************
* @purpose  Sets the data threshold rate
*
* @param    UnitIndex         @b{(input)}  unix Index.
* @param    family            @b{(input)}  family Type. 
* @param    dataThresholdRate @b{(input)}  threshold rate in seconds.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmDataThresholdRateSet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                       L7_uint32 dataThresholdRate)
{
    return pimsmMapDataThresholdRateSet(family, dataThresholdRate);
}

/*********************************************************************
* @purpose  Gets the data threshold rate
*
* @param    UnitIndex         @b{(input)}  unix Index.
* @param    family            @b{(input)}  family Type. 
* @param    dataThresholdRate @b{(output)}  threshold rate in seconds.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmDataThresholdRateGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                       L7_uint32 *dataThresholdRate)
{
    return pimsmMapDataThresholdRateGet(family, dataThresholdRate);
}

/*********************************************************************
* @purpose  Sets the register threshold rate
*
* @param    UnitIndex         @b{(input)}  unix Index.
* @param    family            @b{(input)}  family Type. 
* @param    regThresholdRate  @b{(input)}  register Threshold in seconds.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmRegisterThresholdRateSet(L7_uint32 UnitIndex, 
                                           L7_uchar8 family,
                                           L7_uint32 regThresholdRate)
{
    return pimsmMapRegisterThresholdRateSet(family, regThresholdRate);
}

/*********************************************************************
* @purpose  Gets the register threshold rate
*
* @param    UnitIndex         @b{(input)}  unix Index.
* @param    family            @b{(input)}  family Type. 
* @param    regThresholdRate  @b{(output)} register Threshold in seconds.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmRegisterThresholdRateGet(L7_uint32 UnitIndex,
                                           L7_uchar8 family,
                                           L7_uint32 *regThresholdRate)
{
    return pimsmMapRegisterThresholdRateGet(family, regThresholdRate);
}


/*********************************************************************
* @purpose  Gets the next static RP information after the supplied info
*
* @param    family            @b{(input)} Address Family type
* @param    rpGrpAddr         @b{(inout)}  Group address supported by the RP
* @param    prefixLen         @b{(inout)}   prefix Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPNextGet(L7_uchar8 family, L7_inet_addr_t *rpGrpAddr, 
                                  L7_uchar8 *prefixLen)
{
    return pimsmMapStaticRPNextGet(family, rpGrpAddr, prefixLen);
}

/*********************************************************************
* @purpose  Gets the number of static RP entries
*
* @param    family            @b{(input)} Address Family type
* @param    num               @b{(output)} Number of entries.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNumStaticRPEntriesGet(L7_uchar8 family, L7_uint32 *num)
{
  /*This API should not be called by the UI to get the NUm of Static RP entries */
  #if 0
    return pimsmMapNumStaticRPEntriesGet(family, num);
  #endif
    return L7_SUCCESS;
    
}

/*********************************************************************
* @purpose  Get the Interface Operational State of PIMSM in the router.
*
* @param    UnitIndex       @b{(input)} unit Index.
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} internal Interface number
*
* @returns  L7_TRUE   if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbPimsmInterfaceOperationalStateGet(L7_uint32 UnitIndex, 
                                  L7_uchar8 family, L7_uint32 intIfNum)
{
    return pimsmMapIntfIsOperational(family, intIfNum);
}

/*********************************************************************
* @purpose  Get the Neighbor count for specified interface
*
* @param    UnitIndex       @b{(input)} unit Index.
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} Internal Interface number
* @param    nbrCount        @b{(output)} Neighbor Count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNeighborCountGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                   L7_uint32 intIfNum, L7_uint32 *nbrCount)
{
    return pimsmMapNeighborCountGet(family, intIfNum, nbrCount);
}

/************************************************************************
* @purpose  Get minimum time Spec remaining before the bootstrap router in
*           the local domain will be declared down.
*
* @param    UnitIndex       @b{(input)} unit Index.
* @param    family          @b{(input)} Address Family type
* @param    val             @b{(output)} Expiry Time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmElectedBSRExpiryTimeSpecGet(L7_uint32 UnitIndex,
                                              L7_uchar8 family,
                                              usmDbTimeSpec_t* val)
{
    L7_uint32 compBSRExpiryTime;
    if (L7_SUCCESS == pimsmMapElectedBSRExpiryTimeGet(family,
                                                      &compBSRExpiryTime))
    {
        osapiConvertRawUpTime(compBSRExpiryTime,(L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}


/************************************************************************
* @purpose  Get the hold time Spec of the elected bsr.
*
* @param    UnitIndex       @b{(input)} unit Index.
* @param    family          @b{(input)} Address Family type
* @param    val             @b{(output)} CRP Adv Time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmElectedCRPAdvTimeSpecGet(L7_uint32 UnitIndex,
                                           L7_uchar8 family,
                                           usmDbTimeSpec_t* val)
{
    L7_uint32 elecCRPAdvTime = L7_NULL;

    if (L7_SUCCESS == pimsmMapElectedCRPAdvTimeGet(family, &elecCRPAdvTime))
    {
        osapiConvertRawUpTime(elecCRPAdvTime, (L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Uptime Spec for the specified neighbour
*
* @param    UnitIndex       @b{(input)} unit Index.
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} Internal Interface number
* @param    nbrIpAddr       @b{(input)} IP Address of Neighbor
* @param    val            @b{(output)}Up Time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        if neighbor does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNeighborUpTimeSpecGet(L7_uint32 UnitIndex, 
                                        L7_uchar8 family,
                                        L7_uint32 intIfNum,
                                        L7_inet_addr_t *nbrIpAddr,
                                        usmDbTimeSpec_t* val)
{
    L7_uint32 nbrUpTime = L7_NULL;

    if (L7_SUCCESS == pimsmMapNeighborUpTimeGet(family, intIfNum, 
                                                nbrIpAddr, &nbrUpTime))
    {
        osapiConvertRawUpTime(nbrUpTime,(L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Expiry time Spec for the specified neighbour
*
* @param    UnitIndex       @b{(input)} unit Index.
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} Internal Interface number
* @param    nbrIpAddr       @b{(input)} IP Address of Neighbor
* @param    val             @b{(output)}Expiry Time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        if neighbor does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNeighborExpiryTimeSpecGet(L7_uint32 UnitIndex,
                                            L7_uchar8 family,
                                            L7_uint32 intIfNum,
                                            L7_inet_addr_t *nbrIpAddr,
                                            usmDbTimeSpec_t* val)
{
    L7_uint32 nbrExpiryTime = L7_NULL;
    if (L7_SUCCESS == pimsmMapNeighborExpiryTimeGet(family, 
                                  intIfNum, nbrIpAddr, &nbrExpiryTime))
    {
        osapiConvertRawUpTime(nbrExpiryTime,(L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/************************************************************************
* @purpose  Get the holdtime Spec of a Candidate-RP.  If the local
*           router is not the BSR, this value is 0.
*
* @param    UnitIndex         @b{(input)} unit Index.
* @param    family            @b{(input)} Address Family type
* @param    rpSetGroupAddress @b{(input)} GroupAddress.
* @param    rpSetGroupMask    @b{(input)} GroupMask.
* @param    rpSetAddress      @b{(input)} SetAddress.
* @param    rpSetComponent    @b{(input)} SetComponent.
* @param    val              @b{(output)} seconds.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmRPSetHoldTimeSpecGet(L7_uint32 UnitIndex,
                                       L7_uchar8 family,
                                       L7_inet_addr_t *rpSetGroupAddress,
                                       L7_inet_addr_t *rpSetGroupMask,
                                       L7_inet_addr_t *rpSetAddress,
                                       L7_uint32 rpSetComponent,
                                       usmDbTimeSpec_t* val)
{
    L7_uint32 rpSetHoldTime = L7_NULL;

    if (L7_SUCCESS == pimsmMapRPSetHoldTimeGet(family, rpSetGroupAddress,
                                              rpSetGroupMask,
                                              rpSetAddress,
                                              rpSetComponent,
                                              &rpSetHoldTime))
    {
        osapiConvertRawUpTime(rpSetHoldTime,(L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/************************************************************************
* @purpose  Get the minimum time remaining before the Candidate-RP will be
*           declared down
*
* @param    UnitIndex         @b{(input)} unit Index.
* @param    family            @b{(input)} Address Family type
* @param    rpSetGroupAddress @b{(input)} GroupAddress.
* @param    rpSetGroupMask    @b{(input)} GroupMask.
* @param    rpSetAddress      @b{(input)} SetAddress.
* @param    rpSetComponent    @b{(input)} SetComponent.
* @param    val               @b{(output)} seconds.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmRPSetExpiryTimeSpecGet(L7_uint32 UnitIndex,
                                         L7_uchar8 family,
                                         L7_inet_addr_t *rpSetGroupAddress,
                                         L7_inet_addr_t *rpSetGroupMask,
                                         L7_inet_addr_t *rpSetAddress,
                                         L7_uint32 rpSetComponent,
                                         usmDbTimeSpec_t* val)
{
    L7_uint32 rpSetExpiryTime = L7_NULL;

    if (L7_SUCCESS == pimsmMapRPSetExpiryTimeGet(family, rpSetGroupAddress,
                                                 rpSetGroupMask,
                                                 rpSetAddress,
                                                 rpSetComponent,
                                                 &rpSetExpiryTime))
    {
        osapiConvertRawUpTime(rpSetExpiryTime,(L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Removes the static RP information
*
* @param    family            @b{(input)} Address Family type
* @param    rpIpAddr          @b{(input)} Ip address of the RP
* @param    rpGrpAddr         @b{(input)} Group address supported by the RP
* @param    prefixLen         @b{(input)} prefix Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPRemove(L7_uchar8 family, L7_inet_addr_t *rpIpAddr, 
                            L7_inet_addr_t *rpGrpAddr, L7_uchar8 prefixLen)
{
    return pimsmMapStaticRPRemove(family, rpIpAddr, 
                                               rpGrpAddr, prefixLen);
}

/*********************************************************************
* @purpose  Sets the static RP information
*
* @param    family            @b{(input)} Address Family type
* @param    rpIpAddr          @b{(input)} Ip address of the RP
* @param    rpGrpAddr         @b{(input)} Group address supported by the RP
* @param    prefixLen         @b{(input)} prefix Length.
* @param    conflict          @b{(input)} conflict in configuration 
*                                         (override option)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPSet(L7_uchar8 family, L7_inet_addr_t *rpIpAddr, 
     L7_inet_addr_t *rpGrpAddr, L7_uchar8 prefixLen, L7_BOOL conflict)
{
    return pimsmMapStaticRPSet(family, rpIpAddr, 
                                rpGrpAddr, prefixLen, conflict);
}

/*********************************************************************
* @purpose  Sets the RP Candidate for specified interface
*
* @param    family            @b{(input)} Address Family type
* @param    intIfNum          @b{(input)} interface Number.
* @param    rpGrpAddr         @b{(input)} Group address supported by the RP
* @param    rpGrpMask         @b{(input)} Group Mask.
* @param    mode              @b{(input)} Enable/Disable.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandRPSet(L7_uchar8 family, 
                                 L7_uint32      intIfNum, 
                                 L7_inet_addr_t *rpGrpAddr,
                                 L7_inet_addr_t *rpGrpMask, 
                                 L7_uint32 mode)
{
    return pimsmMapCandRPSet(family, intIfNum, rpGrpAddr, rpGrpMask, mode);
}
/*********************************************************************
* @purpose  Gets the interface on which Candidate Rp is configured
*
* @param    familyType   @b{(input)}        Address Family type
* @param    intIfNum     @b{(output)}       interface Number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmMapCandRPConfiguredIntfGet(L7_uchar8 familyType, L7_uint32 *intIfNum)
{
  return pimsmMapCandRPConfiguredIntfGet(familyType, intIfNum);
}

/************************************************************************
* @purpose  Set the candidate BSR of the router.
*
* @param    UnitIndex         @b{(input)} unit Index.
* @param    family            @b{(input)} Address Family type
* @param    intIfNum          @b{(input)} interface Number.
* @param    priorty           @b{(input)} priority.
* @param    mode              @b{(input)} mode.
* @param    maskLen           @b{(input)} Hash Mask Length.
* @param    scopeVal          @b{(input)} scope Value.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmCandBSRSet(L7_uint32 UnitIndex, L7_uchar8 family,
        L7_uint32 intIfNum, L7_uint32 priority, 
        L7_uint32 mode, L7_uint32 maskLen, L7_uint32 scopeVal)
{
    return pimsmMapCandBSRSet(family, intIfNum, 
                                   priority, mode, maskLen, scopeVal);
}


/*********************************************************************
* @purpose  Clears routes, neighbors and related in Pimsm
*
* @param    UnitIndex  @b{(input)} the unit for this operation
*
* @returns 
*
* @comments    none
*
* @end
*********************************************************************/
void usmDbPimsmClearRoutes(L7_uint32 UnitIndex)
{
   /* Dont do anything here as it is not per standard CLI*/
    return;
}

/*********************************************************************
* @purpose  Get the Operational State of PIMSM in the router.
*
* @param    UnitIndex       @b{(input)} unit Index.
* @param    family          @b{(input)} Address Family type
*
* @returns  L7_TRUE   if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbPimsmIsOperational(L7_uint32 UnitIndex, L7_uchar8 family)
{
    return pimsmMapPimsmIsOperational(family);
}

/*********************************************************************
* @purpose  Finds whether V6 entries are present in the PIMSM starG table.
*
* @param    none
*
* @returns  L7_TRUE   if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbV6StarGEntriesExists()
{
  L7_inet_addr_t mRouteGroup;
  inetAddressZeroSet(L7_AF_INET6, &mRouteGroup);

  if((pimsmMapPimsmIsOperational(L7_AF_INET6) == L7_TRUE)
      && (pimsmMapStarGEntryNextGet(L7_AF_INET6, &mRouteGroup) == L7_SUCCESS))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Finds whether V6 entries are not present in the PIMSM starG table.
*
* @param    none
*
* @returns  L7_TRUE   if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbV6StarGEntriesNotExists()
{
  return !usmDbV6StarGEntriesExists();
}

/*********************************************************************
* @purpose  Finds whether V4 entries are present in the PIMSM starG table.
*
* @param    none
*
* @returns  L7_TRUE   if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbV4StarGEntriesExists()
{
  L7_inet_addr_t mRouteGroup;
  inetAddressZeroSet(L7_AF_INET, &mRouteGroup);

  if((pimsmMapPimsmIsOperational(L7_AF_INET) == L7_TRUE)
      && (pimsmMapStarGEntryNextGet(L7_AF_INET, &mRouteGroup) == L7_SUCCESS))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Finds whether V4 entries are present in the PIMSM starG table.
*
* @param    none
*
* @returns  L7_TRUE   if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbV4StarGEntriesNotExists()
{
  return !usmDbV4StarGEntriesExists();
}

/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType           @b{(input)}     Address Family type
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
L7_RC_t usmDbPimsmMapOIFGet(L7_uchar8 familyType,
                                           L7_inet_addr_t *ipMRouteGroup,
                                           L7_inet_addr_t *ipMRouteSource,
                                           L7_inet_addr_t *ipMRouteSourceMask,
                                           L7_INTF_MASK_t     * outIntIfMask)
{
   return pimsmMapOIFGet(familyType, ipMRouteGroup, ipMRouteSource,
                         ipMRouteSourceMask, outIntIfMask);
}
/*********************************************************************
* @purpose  Turns on/off the displaying of pimsm packet debug info
*            
* @param    family     @b{(input)} Address Family type
* @param    rxFlag     @b{(input)} Receive Trace Flag
* @param    txFlag     @b{(input)} Transmit Trace Flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmPacketDebugTraceFlagSet(L7_uchar8 family, L7_BOOL rxFlag,
                                          L7_BOOL txFlag)
{
  if (pimsmDebugTraceFlagSet(family, PIMSM_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pimsmDebugTraceFlagSet(family, PIMSM_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the pimsm packet debug info
*            
* @param    family     @b{(input)} Address Family type
* @param    rxFlag     @b{(output)} Receive Trace Flag
* @param    txFlag     @b{(output)} Transmit Trace Flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmPacketDebugTraceFlagGet(L7_uchar8 family, L7_BOOL *rxFlag,
                                          L7_BOOL *txFlag)
{
  if (pimsmDebugTraceFlagGet(family, PIMSM_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pimsmDebugTraceFlagGet(family, PIMSM_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/************************************************************************
* @purpose  Set the candidate BSR of the router.
*
* @param    zoneIndex         @b{(input)} Scope Zone Index
* @param    intIfNum          @b{(input)} interface Number.
* @param    priorty           @b{(input)} priority.
* @param    mode              @b{(input)} mode.
* @param    maskLen           @b{(input)} Hash Mask Length.
* @param    scopeVal          @b{(input)} scope Value.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRSet (L7_uchar8 zoneIndex,
                         L7_uint32 intIfNum,
                         L7_uint32 priority, 
                         L7_uint32 mode,
                         L7_uint32 maskLen,
                         L7_uint32 scopeVal)
{
  return pimsmMapCandBSRSet (zoneIndex,
                             intIfNum, 
                             priority,
                             mode,
                             maskLen,
                             scopeVal);
}

/************************************************************************
* @purpose  Get minimum time Spec remaining before the bootstrap router in
*           the local domain will be declared down.
*
* @param    zoneIndex       @b{(input)} Scope Zone Index
* @param    val             @b{(output)} Expiry Time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t
usmDbPimsmXUIElectedBSRExpiryTimeSpecGet (L7_uchar8 zoneIndex,
                                          usmDbTimeSpec_t* val)
{
  L7_uint32 compBSRExpiryTime;

  if (L7_SUCCESS == pimsmMapElectedBSRExpiryTimeGet(zoneIndex, &compBSRExpiryTime))
  {
    osapiConvertRawUpTime (compBSRExpiryTime,(L7_timespec *)val);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/************************************************************************
* @purpose  Get the hold time Spec of the elected bsr.
*
* @param    zoneIndex       @b{(input)} Scope Zone Index
* @param    val             @b{(output)} CRP Adv Time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t
usmDbPimsmXUIElectedCRPAdvTimeSpecGet (L7_uchar8 zoneIndex,
                                       usmDbTimeSpec_t* val)
{
  L7_uint32 elecCRPAdvTime = L7_NULL;

  if (L7_SUCCESS == pimsmMapElectedCRPAdvTimeGet (zoneIndex, &elecCRPAdvTime))
  {
    osapiConvertRawUpTime (elecCRPAdvTime, (L7_timespec *)val);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


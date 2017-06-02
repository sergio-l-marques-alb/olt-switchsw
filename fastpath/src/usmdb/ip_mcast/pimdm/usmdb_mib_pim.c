/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    usmdb_mib_pim.c
*
* @purpose      Imlementation of USMDB layer for PIM
*
* @component    USMDB
*
* @comments
*
* @create      11/27/2001
*
* @author   Abdul Shareef
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pim_api.h"
#endif

#include "osapi.h"
#include "nimapi.h"

#include "pimdm_debug_api.h"
#include "trap_layer3_mcast_api.h"

/*********************************************************************
* @purpose  Gets the PIM interface mask
*
* @param    UnitIndex   @b{(input)}  Unit Index.
* @param    family      @b{(input)}  family Type. 
* @param    ifIndex     @b{(input)}  PIM interface index
* @param    netMask     @b{(output)} PIM interface IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceNetMaskGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                    L7_uint32 ifIndex, L7_inet_addr_t* netMask)
{
    return pimdmMapIntfSubnetMaskGet(family, ifIndex,netMask);
}

/*********************************************************************
* @purpose  Gets the PIM Interface DR
*
* @param    UnitIndex    @b{(input)}  Unit Index.
* @param    family       @b{(input)}  family Type. 
* @param    ifIndex      @b{(input)}  PIM interface index
* @param    dsgRtrIpAddr @b{(output)} PIM Interface DR
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceDRGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                       L7_uint32 ifIndex, L7_inet_addr_t* dsgRtrIpAddr)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the PIM Interface Hello Interval
*
* @param    UnitIndex    @b{(input)}  Unit Index.
* @param    family       @b{(input)}  family Type. 
* @param    ifIndex      @b{(input)}  PIM interface index
* @param    helloIntvl   @b{(output)} PIM Interface Hello Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceHelloIntervalGet(L7_uint32 UnitIndex, 
               L7_uchar8 family, L7_uint32 ifIndex,L7_uint32* helloIntvl)
{
    return pimdmMapIntfHelloIntervalGet(family, ifIndex, helloIntvl);
}

/*********************************************************************
* @purpose  Gets the PIM Interface CBSR Preference
*
* @param    UnitIndex    @b{(input)}  Unit Index.
* @param    family       @b{(input)}  family Type. 
* @param    ifIndex      @b{(input)}  PIM interface index
* @param    val          @b{(output)} PIM Interface CBSR Preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceCBSRPreferenceGet(L7_uint32 UnitIndex, 
                   L7_uchar8 family, L7_uint32 ifIndex, L7_uint32* val)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Sets the PIM Interface Hello Interval
*
* @param    UnitIndex    @b{(input)}  Unit Index.
* @param    family       @b{(input)}  family Type. 
* @param    ifIndex      @b{(input)}  PIM interface index
* @param    helloIntvl   @b{(input)}  PIM Interface Hello Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceHelloIntervalSet(L7_uint32 UnitIndex, 
              L7_uchar8 family, L7_uint32 ifIndex,L7_uint32 helloIntvl)
{
    return pimdmMapIntfHelloIntervalSet(family, ifIndex, helloIntvl);
}

/*********************************************************************
* @purpose  Sets the PIM Interface Join/Prune Interval
*
* @param    UnitIndex    @b{(input)}  Unit Index.
* @param    family       @b{(input)}  family Type. 
* @param    ifIndex      @b{(input)}  PIM interface index
* @param    val          @b{(input)}  PIM Interface Join/Prune Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceJoinPruneIntervalSet(L7_uint32 UnitIndex, 
                       L7_uchar8 family, L7_uint32 ifIndex, L7_uint32 val)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Sets the PIM Interface CBSR Preference
*
* @param    UnitIndex    @b{(input)}  Unit Index.
* @param    family       @b{(input)}  family Type. 
* @param    ifIndex      @b{(input)}  PIM interface index
* @param    val          @b{(input)}  PIM Interface CBSR Preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceCBSRPreferenceSet(L7_uint32 UnitIndex, 
                     L7_uchar8 family, L7_uint32 ifIndex, L7_uint32 val)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets next valid PIM Neighbor Address By Interface Index
*
* @param    UnitIndex           @b{(input)} Unit Index.
* @param    family              @b{(input)} family Type. 
* @param    pimNeighborAddress  @b{(inout)} PIM Neighbor Address
* @param    pimNeighborAddress  @b{(inout)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborEntryByIfIndexNextGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                              L7_inet_addr_t* pimNeighborAddress,
                                              L7_uint32 *intIfNum)
{
    return pimdmMapNbrEntryByIfIndexNextGet(family, intIfNum,pimNeighborAddress);
}

/*********************************************************************
* @purpose  Gets PIM IfIndex for the Neighbor
*
* @param    UnitIndex           @b{(input)} Unit Index.
* @param    family              @b{(input)} family Type. 
* @param    pimNeighborAddress  @b{(input)} PIM Neighbor Address
* @param    val                 @b{(output)} PIM IfIndex
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborIfIndexGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                        L7_inet_addr_t* pimNeighborAddress, L7_uint32* val)
{
    return pimdmMapNbrIfIndexGet(family, pimNeighborAddress,val);
}

/*********************************************************************
* @purpose  Gets PIM UpTime for the Neighbor By Interface Index
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    pimNeighborAddress  @b{(input)}  PIM Neighbor Address
* @param    pimNeighborAddress  @b{(input)}  internal interface number
* @param    val                 @b{(output)} PIM UpTime
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborUpTimeByIfIndexGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                           L7_inet_addr_t* pimNeighborAddress, 
                                           L7_uint32 intIfNum,
                                           L7_uint32* val)
{
    return pimdmMapNbrUpTimeByIfIndexGet(family, pimNeighborAddress,intIfNum, val);
}

/*********************************************************************
* @purpose  Gets PIM Expiry Time for the Neighbor By Interface Index
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    pimNeighborAddress  @b{(input)}  PIM Neighbor Address
* @param    pimNeighborAddress  @b{(input)}   internal interface number
* @param    val                 @b{(output)} PIM Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborExpiryTimeByIfIndexGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                               L7_inet_addr_t* pimNeighborAddress,
                                               L7_uint32 intIfNum,
                                               L7_uint32* val)
{
    return pimdmMapNbrExpiryTimeByIfIndexGet(family, pimNeighborAddress,intIfNum, val);
}

/*********************************************************************
* @purpose  Gets PIM Mode for the Neighbor
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    pimNeighborAddress  @b{(input)}  PIM Neighbor Address
* @param    val                 @b{(output)} PIM Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborModeGet(L7_uint32 UnitIndex, L7_uchar8 family,
                     L7_inet_addr_t* pimNeighborAddress, L7_uint32* val)
{
    return pimdmMapNbrModeGet(family, pimNeighborAddress, val);
}

/*********************************************************************
* @purpose  Checks whether PIM IPMRoute entry is valid
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    ipMRouteGroup       @b{(input)}  PIM IPMRoute Group
* @param    ipMRouteSource      @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteSourceMask  @b{(input)}  PIM IPMRoute Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteEntryGet(L7_uint32 UnitIndex, L7_uchar8 family, 
             L7_inet_addr_t* ipMRouteGroup, L7_inet_addr_t* ipMRouteSource,
             L7_inet_addr_t*  ipMRouteSourceMask)
{
    return pimdmMapIpMRouteEntryGet(family, ipMRouteGroup, ipMRouteSource,
                                    ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Gets next valid PIM IPMRoute entry
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    ipMRouteGroup       @b{(inout)} PIM IPMRoute Group
* @param    ipMRouteSource      @b{(inout)} PIM IPMRoute Source
* @param    ipMRouteSourceMask  @b{(inout)} PIM IPMRoute Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteEntryNextGet(L7_uint32 UnitIndex, L7_uchar8 family, 
             L7_inet_addr_t* ipMRouteGroup, L7_inet_addr_t* ipMRouteSource, 
             L7_inet_addr_t* ipMRouteSourceMask)
{
    return pimdmMapIpMRouteEntryNextGet(family, ipMRouteGroup, 
                                        ipMRouteSource, ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Gets PIM IPMRoute Upstream Assert Timer
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    ipMRouteGroup       @b{(input)}  PIM IPMRoute Group
* @param    ipMRouteSource      @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteSourceMask  @b{(input)}  PIM IPMRoute Mask
* @param    val                 @b{(output)} PIM IPMRoute Upstream Assert Timer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteUpstreamAssertTimerGet(L7_uint32 UnitIndex,
                       L7_uchar8 family, L7_inet_addr_t* ipMRouteGroup, 
                       L7_inet_addr_t* ipMRouteSource, 
                       L7_inet_addr_t*  ipMRouteSourceMask, L7_uint32*  val)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets PIM IPMRoute Upstream Assert Metric
*
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    ipMRouteGroup       @b{(input)}  PIM IPMRoute Group
* @param    ipMRouteSource      @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteSourceMask  @b{(input)}  PIM IPMRoute Mask
* @param    val                 @b{(output)} PIM IPMRoute Assert Metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteAssertMetricGet(L7_uint32 UnitIndex,  L7_uchar8 family,
                 L7_inet_addr_t* ipMRouteGroup, L7_inet_addr_t* ipMRouteSource,
                 L7_inet_addr_t*  ipMRouteSourceMask, L7_uint32*  val)
{
    return pimdmMapIpMRouteAssertMetricGet(family, ipMRouteGroup, 
                                    ipMRouteSource,ipMRouteSourceMask,val);
}

/*********************************************************************
* @purpose  Gets PIM IPMRoute Assert Metric Pref
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    ipMRouteGroup       @b{(input)}  PIM IPMRoute Group
* @param    ipMRouteSource      @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteSourceMask  @b{(input)}  PIM IPMRoute Mask
* @param    val                 @b{(output)} PIM IPMRoute Assert Metric Pref
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteAssertMetricPrefGet(L7_uint32 UnitIndex, 
                    L7_uchar8 family, L7_inet_addr_t* ipMRouteGroup, 
                    L7_inet_addr_t* ipMRouteSource,
                    L7_inet_addr_t*  ipMRouteSourceMask, L7_uint32*  val)
{
    return pimdmMapIpMRouteAssertMetricPrefGet(family, ipMRouteGroup, 
                              ipMRouteSource, ipMRouteSourceMask, val);
}

/*********************************************************************
* @purpose  Gets PIM IPMRoute Assert RPT Bit
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    ipMRouteGroup       @b{(input)}  PIM IPMRoute Group
* @param    ipMRouteSource      @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteSourceMask  @b{(input)}  PIM IPMRoute Mask
* @param    val                 @b{(output)} PIM IPMRoute Assert RPT Bit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteAssertRPTBitGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                L7_inet_addr_t* ipMRouteGroup, L7_inet_addr_t* ipMRouteSource,
                L7_inet_addr_t*  ipMRouteSourceMask, L7_uint32*  val)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets PIM IPMRoute Flags
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    ipMRouteGroup       @b{(input)}  PIM IPMRoute Group
* @param    ipMRouteSource      @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteSourceMask  @b{(input)}  PIM IPMRoute Mask
* @param    val                 @b{(output)} PIM IPMRoute Flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteFlagsGet(L7_uint32 UnitIndex, L7_uchar8 family, 
         L7_inet_addr_t* ipMRouteGroup, L7_inet_addr_t* ipMRouteSource,
         L7_inet_addr_t*  ipMRouteSourceMask, L7_uint32*  val)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Checks whether PIM IPMRoute Next Hop entry is valid
*
* @param    UnitIndex                  @b{(input)}  Unit Index.
* @param    family                     @b{(input)}  family Type. 
* @param    ipMRouteNextHopGroup       @b{(input)}  PIM IPMRoute Group
* @param    ipMRouteNextHopSource      @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteNextHopSourceMask  @b{(input)}  PIM IPMRoute Mask
* @param    ipMRouteNextHopIfIndex     @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteNextHopAddress     @b{(input)}  PIM IPMRoute Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteNextHopEntryGet (L7_uint32 UnitIndex, L7_uchar8 family,
  L7_inet_addr_t* ipMRouteNextHopGroup, L7_inet_addr_t* ipMRouteNextHopSource,
  L7_inet_addr_t*  ipMRouteNextHopSourceMask, L7_uint32 ipMRouteNextHopIfIndex,
  L7_inet_addr_t* ipMRouteNextHopAddress)
{
  return pimdmMapIpMRouteNextHopEntryGet(family, ipMRouteNextHopGroup, 
         ipMRouteNextHopSource, ipMRouteNextHopSourceMask, 
         ipMRouteNextHopIfIndex, ipMRouteNextHopAddress);
}

/*********************************************************************
* @purpose  Get next valid PIM IPMRoute Next Hop entry is valid
*
* @param    UnitIndex                  @b{(input)}  Unit Index.
* @param    family                     @b{(input)}  family Type. 
* @param    ipMRouteNextHopGroup       @b{(inout)} PIM IPMRoute Group
* @param    ipMRouteNextHopSource      @b{(inout)} PIM IPMRoute Source
* @param    ipMRouteNextHopSourceMask  @b{(inout)} PIM IPMRoute Mask
* @param    ipMRouteNextHopIfIndex     @b{(inout)} PIM IPMRoute Source
* @param    ipMRouteNextHopAddress     @b{(inout)} PIM IPMRoute Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteNextHopEntryNextGet(L7_uint32 UnitIndex, 
         L7_uchar8 family, L7_inet_addr_t* ipMRouteNextHopGroup, 
         L7_inet_addr_t* ipMRouteNextHopSource, 
         L7_inet_addr_t* ipMRouteNextHopSourceMask, 
         L7_uint32* ipMRouteNextHopIfIndex, 
         L7_inet_addr_t* ipMRouteNextHopAddress)
{
  return pimdmMapIpMRouteNextHopEntryNextGet(family, ipMRouteNextHopGroup, 
         ipMRouteNextHopSource, ipMRouteNextHopSourceMask, 
         ipMRouteNextHopIfIndex, ipMRouteNextHopAddress);
}

/*********************************************************************
* @purpose  Gets PIM IPMRoute Next Hop Prune Reason
*
*
* @param    UnitIndex                  @b{(input)}  Unit Index.
* @param    family                     @b{(input)}  family Type. 
* @param    ipMRouteNextHopGroup       @b{(input)}  PIM IPMRoute Group
* @param    ipMRouteNextHopSource      @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteNextHopSourceMask  @b{(input)}  PIM IPMRoute Mask
* @param    ipMRouteNextHopIfIndex     @b{(input)}  PIM IPMRoute Source
* @param    ipMRouteNextHopAddress     @b{(input)}  PIM IPMRoute Mask
* @param    val                        @b{(output)} PIM IPMRoute Next Hop 
*                                                   Prune Reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteNextHopPruneReasonGet(L7_uint32 UnitIndex, 
                L7_uchar8 family, L7_inet_addr_t* ipMRouteNextHopGroup, 
                L7_inet_addr_t* ipMRouteNextHopSource, 
                L7_inet_addr_t*  ipMRouteNextHopSourceMask, 
                L7_uint32 ipMRouteNextHopIfIndex, 
                L7_inet_addr_t* ipMRouteNextHopAddress, L7_uint32* val)
{
    return pimdmMapIpMRouteNextHopPruneReasonGet(family, ipMRouteNextHopGroup, 
           ipMRouteNextHopSource, ipMRouteNextHopSourceMask, 
           ipMRouteNextHopIfIndex, ipMRouteNextHopAddress, val);
}

/*********************************************************************
* @purpose  Gets PIM Admin Mode
*
* @param    UnitIndex     @b{(input)}  Unit Index.
* @param    family        @b{(input)}  family Type. 
* @param    ifIndex       @b{(input)}  PIM Interface Index.
* @param    adminMode     @b{(output)} PIM Admin mode.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimAdminModeGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                             L7_uint32 ifIndex, L7_uint32* adminMode)
{
    return pimdmMapIntfAdminModeGet(family, ifIndex, adminMode);
}

/*********************************************************************
* @purpose  Sets PIM Admin Mode
*
* @param    UnitIndex     @b{(input)}  Unit Index.
* @param    family        @b{(input)}  family Type. 
* @param    ifIndex       @b{(input)}  PIM Interface Index.
* @param    adminMode     @b{(input)}  PIM Admin mode.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimAdminModeSet(L7_uint32 UnitIndex, L7_uchar8 family, 
                             L7_uint32 ifIndex, L7_uint32 adminMode)
{
    return pimdmMapIntfAdminModeSet(family, ifIndex, adminMode);
}

/*********************************************************************
* @purpose  Gets PIM Interface Neighbor Count.
*
* @param    UnitIndex     @b{(input)}  Unit Index.
* @param    family        @b{(input)}  family Type. 
* @param    ifIndex       @b{(input)}  PIM Interface Index.
* @param    nbrCount      @b{(output)} Neighbor count.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceNeighborCountGet(L7_uint32 UnitIndex, 
               L7_uchar8 family, L7_uint32 ifIndex, L7_uint32* nbrCount)
{
    return pimdmMapNbrCountGet(family, ifIndex, nbrCount);
}

/*********************************************************************
* @purpose  Gets Next PIM Interface Neighbor Address.
*
* @param    UnitIndex     @b{(input)}  Unit Index.
* @param    family        @b{(input)}  family Type. 
* @param    ifIndex       @b{(input)}  PIM Interface Index.
* @param    val           @b{(output)} Neighbor Address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceNeigborAddressGetNext(L7_uint32 UnitIndex, 
                    L7_uchar8 family, L7_uint32 ifIndex, L7_uint32* val)
{
  return L7_FAILURE; 
}

/*********************************************************************
* @purpose  Gets PIM UpTime Spec for the Neighbor
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    pimNeighborAddress  @b{(input)}  PIM Neighbor Address
* @param    val                 @b{(output)} PIM UpTime Spec
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborUpTimeSpecGet(L7_uint32 UnitIndex, L7_uchar8 family,
                    L7_inet_addr_t* pimNeighborAddress, usmDbTimeSpec_t* val)
{
    L7_uint32 time = L7_NULL;
    if (L7_SUCCESS == pimdmMapNbrUpTimeGet(family, pimNeighborAddress, &time))
    {
        osapiConvertRawUpTime(time,(L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets PIM Expiry Time Spec for the Neighbor
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    pimNeighborAddress  @b{(input)}  PIM Neighbor Address
* @param    val                 @b{(output)} PIM Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborExpiryTimeSpecGet(L7_uint32 UnitIndex, L7_uchar8 family,
              L7_inet_addr_t* pimNeighborAddress, usmDbTimeSpec_t* val)
{
    L7_uint32 time = L7_NULL;

    if (pimdmMapNbrExpiryTimeGet(family, 
                                 pimNeighborAddress, &time) == L7_SUCCESS)
    {
        osapiConvertRawUpTime(time,(L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets PIM UpTime Spec for the Neighbor
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    pimNeighborAddress  @b{(input)}  PIM Neighbor Address
* @param    pimNeighborAddress  @b{(input)}  interbal interface number
* @param    val                 @b{(output)} PIM UpTime Spec
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborUpTimeSpecByIfIndexGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                               L7_inet_addr_t* pimNeighborAddress,
                                               L7_uint32 intIfNum, 
                                               usmDbTimeSpec_t* val)
{
    L7_uint32 time = L7_NULL;
    if (L7_SUCCESS == pimdmMapNbrUpTimeByIfIndexGet(family, pimNeighborAddress,intIfNum, &time))
    {
        osapiConvertRawUpTime(time,(L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets PIM Expiry Time Spec for the Neighbor
*
* @param    UnitIndex           @b{(input)}  Unit Index.
* @param    family              @b{(input)}  family Type. 
* @param    pimNeighborAddress  @b{(input)}  PIM Neighbor Address
* @param    pimNeighborAddress  @b{(input)}  internal interface number
* @param    val                 @b{(output)} PIM Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborExpiryTimeSpecByIfIndexGet(L7_uint32 UnitIndex, L7_uchar8 family,
              L7_inet_addr_t* pimNeighborAddress, L7_uint32 intIfNum,usmDbTimeSpec_t* val)
{
    L7_uint32 time = L7_NULL;

    if (pimdmMapNbrExpiryTimeByIfIndexGet(family, 
                                 pimNeighborAddress,intIfNum, &time) == L7_SUCCESS)
    {
        osapiConvertRawUpTime(time,(L7_timespec *)val);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets next PIM IPMRoute Entry Outgoing Interface
*
* @param    UnitIndex           @b{(input)} Unit Index.
* @param    family              @b{(input)} family Type. 
* @param    ipMRouteGroup       @b{(input)} PIM IPMRoute Group
* @param    ipMRouteSource      @b{(input)} PIM IPMRoute Source
* @param    ipMRouteSourceMask  @b{(input)} PIM IPMRoute Mask
* @param    ifIndex             @b{(inout)} Outgoing interface
* @param    state               @b{(inout)} Outgoing interface state
* @param    mode                @b{(inout)} Outgoing interface mode
* @param    upTime              @b{(inout)} Outgoing interface uptime
* @param    expiryTime          @b{(inout)} Outgoing interface expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    if ifIndex are -1, then first entry is returned
*
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIpMRouteOutgoingInterfaceGetNext(L7_uint32 UnitIndex, 
    L7_uchar8 family, L7_inet_addr_t *ipMRouteGroup, 
    L7_inet_addr_t *ipMRouteSource, L7_inet_addr_t *ipMRouteSourceMask, 
    L7_uint32* ifIndex, L7_uint32* state, L7_uint32* mode, 
    L7_uint32*  upTime, L7_uint32* expiryTime)
{
    return pimdmMapIpMRouteOIFNextGet (family, ipMRouteGroup,
        ipMRouteSource, ipMRouteSourceMask, ifIndex, state, mode, 
        upTime, expiryTime);
}

/*********************************************************************
* @purpose  Gets PIM Admin Mode for router
*
* @param    UnitIndex           @b{(input)} Unit Index.
* @param    family              @b{(input)} family Type. 
* @param    val                 @b{(output)} PIM Admin mode.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimRouterAdminModeGet(L7_uint32 UnitIndex, 
                                   L7_uchar8 family, L7_uint32* val)
{
    return pimdmMapPimdmAdminModeGet(family, val);
}

/*********************************************************************
* @purpose  Sets PIM Admin Mode
*
* @param    UnitIndex           @b{(input)} Unit Index.
* @param    family              @b{(input)} family Type. 
* @param    val                 @b{(input)} PIM Admin mode.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimRouterAdminModeSet(L7_uint32 UnitIndex, 
                                   L7_uchar8 family, L7_uint32 val)
{
    return pimdmMapPimdmAdminModeSet(family, val);
}

/*********************************************************************
* @purpose  Sets PIM Trap Mode
*
* @param    unitIndex           @b{(input)} Unit Index.
* @param    mode                @b{(input)} enable/disable
* @param    trapType            @b{(input)} trap
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimTrapModeSet(L7_uint32 unitIndex, 
                            L7_uint32 mode, L7_uint32 trapType)
{
  return trapMgrPimTrapModeSet(mode, trapType);
}

/*********************************************************************
* @purpose  Gets PIM Trap Mode
*
* @param    trapType        @b{(input)}  trap
* @param    mode            @b{(output)} enable/disable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimTrapModeGet(L7_uint32 trapType, L7_uint32 *mode)
{
  return trapMgrPimTrapModeGet(mode, trapType);
}

/*********************************************************************
* @purpose  Checks whether an interface is valid for PIM-DM
*
* @param    unitIndex         @b{(input)} Unit Index.
* @param    intIfNum          @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbPimdmIsValidIntf(L7_uint32 unitIndex, L7_uint32 intIfNum)
{
    return pimdmIntfIsValid(intIfNum);
}
/*********************************************************************
* @purpose  Turns on/off the displaying of pimdm packet debug info
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
L7_RC_t usmDbPimdmPacketDebugTraceFlagSet(L7_uchar8 family, L7_BOOL rxFlag,
                                          L7_BOOL txFlag)
{
  if (pimdmDebugTraceFlagSet(family, PIMDM_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pimdmDebugTraceFlagSet(family, PIMDM_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the pimdm packet debug info
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
L7_RC_t usmDbPimdmPacketDebugTraceFlagGet(L7_uchar8 family, L7_BOOL *rxFlag,
                                          L7_BOOL *txFlag)
{
  if (pimdmDebugTraceFlagGet(family, PIMDM_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (pimdmDebugTraceFlagGet(family, PIMDM_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets PIMDM Interface Operational state
*
* @param    UnitIndex     @b{(input)}  Unit Index.
* @param    family        @b{(input)}  family Type. 
* @param    ifIndex       @b{(input)}  PIM Interface Index.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbPimdmInterfaceOperationalStateGet (L7_uint32 UnitIndex, 
                                                L7_uchar8 family,
                                                L7_uint32 ifIndex)
{
  if(pimdmMapIntfOperationalStateGet(family, ifIndex) == L7_SUCCESS)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check whether interface entry exists for the specified interface
*
* @param    UnitIndex    @b{(input)}  UnitIndex.
* @param    family       @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal interface number  
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimdmInterfaceEntryGet(L7_uint32 UnitIndex,
                                    L7_uchar8 family, 
                                    L7_uint32 intIfNum)
{
  return pimdmMapIntfEntryGet(family, intIfNum);
}

/*********************************************************************
* @purpose  Get the existing interface entry next to the entry of the
*           specified interface
*
* @param    UnitIndex    @b{(input)} UnitIndex.
* @param    family       @b{(input)} Address Family type
* @param    intIfNum     @b{(inout)} internal Interface number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimdmInterfaceEntryNextGet(L7_uint32 UnitIndex, 
                                        L7_uchar8 family,
                                        L7_uint32 *intIfNum)
{
  return pimdmMapIntfEntryNextGet(family, intIfNum);
}

/*********************************************************************
* @purpose  Get the existing neighbor entry next to the entry of the
*           specified neighbor
*
* @param    UnitIndex    @b{(input)}  UnitIndex.
* @param    family       @b{(input)}  Address Family type
* @param    intIfNum     @b{(inout)}  internal Interface number
* @param    ipAddress    @b{(inout)}  IP Address
*
* @returns  L7_SUCCESS    if neighbor entry exists
* @returns  L7_FAILURE    if neighbor entry does not exist
*
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimdmNeighborEntryNextGet(L7_uint32 UnitIndex,
                                       L7_uchar8 family,
                                       L7_uint32 *intIfNum,
                                       L7_inet_addr_t *ipAddress)
{
  return pimdmMapNbrEntryByIfIndexNextGet (family, intIfNum, ipAddress);
}


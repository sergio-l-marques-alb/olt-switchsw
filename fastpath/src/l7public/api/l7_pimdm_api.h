/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l7_pimdm_api.h
*
* @purpose    PIMDM Mapping Layer APIs
*
* @component  PIMDM Mapping Layer
*
* @comments   none
*
* @create     02/06/2002
*
* @author     vidhumouli
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef _L7_PIMDM_API_H_
#define _L7_PIMDM_API_H_

#include "l3_comm_structs.h"
#include "l3_mcast_commdefs.h"

/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */
/*********************************************************************
* @purpose  Get the PIMDM administrative mode
*
* @param    familyType  @b{(input)} Address Family type
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimdmAdminModeGet(L7_uchar8 familyType,
                                  L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the PIMDM administrative mode
*
* @param    familyType  @b{(input)} Address Family type
* @param    mode        @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimdmAdminModeSet(L7_uchar8 familyType,
                                  L7_uint32 mode);

/*********************************************************************
* @purpose  Check whether the routing internal interface number is valid*
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(output)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfEntryGet(L7_uchar8 familyType, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain the next sequential routing internal interface number
*           after the interface corresponding to the specified IP address
*
* @param    familyType  @b{(input)}  Address Family type
* @param    pIntIfNum   @b{(output)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfEntryNextGet(L7_uchar8 familyType, L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the Neighbor count specified interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Interface Number
* @param    nbrCount     @b{(output)} neighbour count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrCountGet(L7_uchar8 familyType,
                            L7_uint32 intIfNum, L7_uint32 *nbrCount);

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Uptime for the specified neighbour
*           of the specified interface
*
* @param    familyType  @b{(input)} Address Family type.
* @param    ngbrIpAddr  @b{(input)} Neighbhour IP Address.
* @param    ngbrUpTime  @b{(output)} The time since the neighbour is up
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrUpTimeGet(L7_uchar8 familyType,
                             L7_inet_addr_t *ngbrIpAddr, L7_uint32 *ngbrUpTime);

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Uptime for the specified neighbour
*           of the specified interface
*
* @param    familyType  @b{(input)} Address Family type.
* @param    ngbrIpAddr   @b{(input)} Neighbhour IP Address.
* @param    ngbrIpAddr   @b{(input)} internal interface number
* @param    ngbrUpTime  @b{(output)} The time since the neighbour is up
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrUpTimeByIfIndexGet(L7_uchar8 familyType,L7_inet_addr_t *ngbrIpAddr,
                             L7_uint32 intIfNum,
                             L7_uint32 *ngbrUpTime);

/*********************************************************************
* @purpose  Set the administrative mode of a PIMDM routing interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfAdminModeSet(L7_uchar8 familyType,
                                 L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the administrative mode of a PIMDM routing interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    mode          @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfAdminModeGet(L7_uchar8 familyType,
                                 L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the operational state of the specified interface.
*
* @param    familyType      @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfOperationalStateGet(L7_uchar8 familyType,
                                        L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the Hello interval of a PIMDM routing interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)}  Interface Number
* @param    helloIntvl  @b{(output)} Hello interval value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments The frequency at which PIM Hello messages are transmitted
*           on this interface
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfHelloIntervalGet(L7_uchar8 familyType,
       L7_uint32 intIfNum, L7_uint32 *helloIntvl);

/*********************************************************************
* @purpose  Set the Hello interval of a PIMDM routing interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    helloIntvl  @b{(input)} Hello interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments The frequency at which PIM Hello messages are transmitted 
*           on this interface
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfHelloIntervalSet(L7_uchar8 familyType,L7_uint32 intIfNum,  
           L7_uint32 helloIntvl);
/*********************************************************************
* @purpose  Get the IP address of the specified PIMDM interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    ipAddr        @b{(output)} Interface IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfIPAddrGet(L7_uchar8 familyType,
                              L7_uint32 intIfNum, L7_inet_addr_t *ipAddr);

/*********************************************************************
* @purpose  Get the subnet mask of the specified PIMDM interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    netMask     @b{(output)} Interface subnet mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfSubnetMaskGet(L7_uchar8 familyType, L7_uint32 intIfNum, 
                                  L7_inet_addr_t *netMask);

/*********************************************************************
* @purpose  Get the DR IP address for the specified specified interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    dsgRtrIpAddr  @b{(output)}IP Address of the Designated Router
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfDRGet(L7_uchar8 familyType,L7_uint32 intIfNum, 
                          L7_inet_addr_t *dsgRtrIpAddr);

/*********************************************************************
* @purpose  Get the PIMDM mode on the specified interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)}  Interface Number
* @param    mode          @b{(output)}  PIM protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfModeGet(L7_uchar8 familyType,L7_uint32 intIfNum, 
                            L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the PIMDM mode on the specified interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    mode          @b{(input)} PIM protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfModeSet(L7_uchar8 familyType,L7_uint32 intIfNum, 
                            L7_uint32 mode);

/*********************************************************************
* @purpose  Set the PIMDM join prune interval on the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
/*L7_RC_t pimdmMapInterfaceJoinPruneIntervalSet( L7_uint32 ifIndex,
                                                 L7_uint32 jpIntval );*/

/*********************************************************************
* @purpose  Get the PIMDM neighbor entry on the specified interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    ngbrIpAddr   @b{(output)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrEntryGet(L7_uchar8 familyType, L7_inet_addr_t *nbrIpAddr);

/*********************************************************************
* @purpose  Get the PIMDM Next neighbor entry on the specified interface
*
* @param    familyType    @b{(input)}  Address Family type
* @param    ngbrIpAddr    @b{(output)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrEntryNextGet(L7_uchar8 familyType, L7_inet_addr_t* nbrIpAddr);

/*********************************************************************
* @purpose  Check whether neighbor entry exists for the specified IP address
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    nbrIpAddr   @b{(input)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrEntryByIfIndexGet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                     L7_inet_addr_t* nbrIpAddr);

/*********************************************************************
* @purpose  Get the PIMDM Next neighbor entry By interface index 
*           on the specified interface
*
* @param    familyType    @b{(input)}  Address Family type
* @param    ngbrIpAddr    @b{(in/out)} Internal Interface Number
* @param    ngbrIpAddr    @b{(output)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrEntryByIfIndexNextGet(L7_uchar8 familyType,L7_uint32 *outIntIfNum,
                                L7_inet_addr_t* nbrIpAddr);

/*********************************************************************
* @purpose  Get the Pimdm Interface index of the specified interface
*

* @param    familyType   @b{(input)} Address Family type
* @param    ngbrIpAddr   @b{(output)} Neighbor Ip address
* @param    pIntIfNum     @b{(output)} L7_uint32 interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrIfIndexGet(L7_uchar8 familyType, 
                              L7_inet_addr_t *nbrIpAddr, L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Expire time for the specified neighbour
*           of the specified interface
*
* @param    familyType     @b{(input)} Address Family type
* @param    ngbrIpAddr     @b{(input)}Neighbor Ip address
* @param    nbrExpireTime  @b{(output)} Get the neighbor expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Time before this neighbor entry status expires
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrExpiryTimeGet(L7_uchar8 familyType, 
                                 L7_inet_addr_t *nbrIpAddr, 
                                 L7_uint32 *nbrExpireTime);

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Expire time for the specified neighbour
*           of the specified interface
*
* @param    familyType     @b{(input)} Address Family type
* @param    ngbrIpAddr     @b{(input)}Neighbor Ip address
* @param    ngbrIpAddr     @b{(input)}internal interface number
* @param    nbrExpireTime  @b{(output)} Get the neighbor expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Time before this neighbor entry status expires
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrExpiryTimeByIfIndexGet(L7_uchar8 familyType,
             L7_inet_addr_t *nbrIpAddr, L7_uint32 intIfNum,L7_uint32 *nbrExpireTime);


/*********************************************************************
* @purpose  Get the PIMDM  neighbor mode on the specified interface
*
* @param    familyType     @b{(input)} Address Family type
* @param    ngbrIpAddr     @b{(input)}Neighbor Ip address
* @param    mode           @b{(output)} PIM protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrModeGet(L7_uchar8 familyType,
                           L7_inet_addr_t *nbrIpAddr, 
                           L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the assert metric
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    assertMetric         @b{(output)} Assert metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteAssertMetricGet(L7_uchar8 familyType,
   L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
   L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetric);

/*********************************************************************
* @purpose  Get the assert metric preference
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    assertMetricPref     @b{(output)} Assert metric preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteAssertMetricPrefGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetricPref);

/*********************************************************************
* @purpose  Get the RPT bit status
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    RPTBitStatus         @b{(output)} RPT bit status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteAssertRPTBitGet(L7_uchar8 familyType,
   L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
   L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *RPTBitStatus);

/*********************************************************************
* @purpose  Get the Upstream assert timer status
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    upstreamAssertTimer  @b{(output)}  Upstream assert timer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteUpstreamAssertTimerGet(L7_uchar8 familyType, 
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
  L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *upstreamAssertTimer);

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast route table entry
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteEntryGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask);

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast route table Next entry
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(inout)}  Multicast Group address
* @param    ipMRouteSource       @b{(inout)}  Source address
* @param    ipMRouteSourceMask   @b{(inout)}  Mask Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteEntryNextGet(L7_uchar8 familyType,
   L7_inet_addr_t* ipMRouteGroup, L7_inet_addr_t* ipMRouteSource, 
   L7_inet_addr_t* ipMRouteSourceMask);

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast Flags
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    flags                @b{(output)} flags.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteFlagsGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32*  flags);

/*********************************************************************
* @purpose  Get the PIMDM  Next Hop table entry
*
* @param    familyType                 @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteNextHopSource      @b{(input)}  Source address
* @param    ipMRouteNextHopSourceMask  @b{(input)}  Mask Address
* @param    ipMRouteNextHopIfIndex     @b{(input)}  Next Hop If Index
* @param    ipMRouteNextHopAddress     @b{(input)}  Next Hop Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteNextHopEntryGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *ipMRouteNextHopGroup, 
                                   L7_inet_addr_t *ipMRouteNextHopSource, 
                                   L7_inet_addr_t  *ipMRouteNextHopSourceMask, 
                                   L7_uint32 ipMRouteNextHopIfIndex,
                                   L7_inet_addr_t *ipMRouteNextHopAddress);

/*********************************************************************
* @purpose  Get the PIMDM  Next Hop table Next entry
*
* @param    familyType                 @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteNextHopSource      @b{(input)}  Source address
* @param    ipMRouteNextHopSourceMask  @b{(input)}  Mask Address
* @param    ipMRouteNextHopIfIndex     @b{(input)}  Next Hop If Index
* @param    ipMRouteNextHopAddress     @b{(input)}  Next Hop Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteNextHopEntryNextGet
 (L7_uchar8 familyType, L7_inet_addr_t* ipMRouteNextHopGroup, 
  L7_inet_addr_t* ipMRouteNextHopSource, 
  L7_inet_addr_t* ipMRouteNextHopSourceMask, 
  L7_uint32* ipMRouteNextHopIfIndex,
  L7_inet_addr_t* ipMRouteNextHopAddress );

/*********************************************************************
* @purpose  Get the Next Hop Prune reason on the specified interface
*
* @param    familyType                 @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteNextHopSource      @b{(input)}  Source address
* @param    ipMRouteNextHopSourceMask  @b{(input)}  Mask Address
* @param    ipMRouteNextHopIfIndex     @b{(input)}  Next Hop If Index
* @param    ipMRouteNextHopAddress     @b{(input)}  Next Hop Address
* @param    PruneReason                @b{(output)} Prune reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments Prune is generated in router durig assert/flood-prune 
*           and IGMP no member cases
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteNextHopPruneReasonGet
 (L7_uchar8 familyType, L7_inet_addr_t *ipMRouteNextHopGroup, 
  L7_inet_addr_t *ipMRouteNextHopSource, 
  L7_inet_addr_t *ipMRouteNextHopSourceMask, L7_uint32 ipMRouteNextHopIfIndex,
  L7_inet_addr_t *ipMRouteNextHopAddress, L7_uint32 *pruneReason);

/*********************************************************************
* @purpose  Get the PIMDM  IP multicast table  Out going interface list
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    ifIndex             @b{(input)}  Input interface index
* @param    state               @b{(input)}  interface status
* @param    mode                @b{(output)} PIM mode
* @param    upTime              @b{(output)} entry uptime
* @param    expiryTime          @b{(output)} entry expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteOIFNextGet (L7_uchar8 familyType,
                                    L7_inet_addr_t *ipMRouteGroup,
                                    L7_inet_addr_t *ipMRouteSource,
                                    L7_inet_addr_t *ipMRouteSourceMask,
                                    L7_uint32* ifIndex,
                                    L7_uint32* state,
                                    L7_uint32* mode,
                                    L7_uint32*  upTime, 
                                    L7_uint32* expiryTime);

/*********************************************************************
* @purpose  Get IP Mapping Layer tracing mode
*
* @param    familyType    @b{(input)} Address Family type
*
* @return    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapTraceModeGet(L7_uchar8 familyType);


/*********************************************************************
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    familyType @b{(input)} Address Family type
* @param    rtrIntNum  @b{(input)} Router interface on which neighbor is lost.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimTrapNeighborLoss (L7_uchar8 familyType, 
                                     L7_uint32 rtrIntNum);

/*********************************************************************
* @purpose  Determine if the interface type is valid in PIMDM
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL pimdmIntfIsValidType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in PIMDM
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL pimdmIntfIsValid(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the Interface index for a specific route
*           for a specified source address.
*
* @param    familyType   @b{(input)} Address Family type
* @param    grpIpAddr    @b{(input)} Group IP Address
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    IfIndex      @b{(output)}Interface Index which is passed back 
*                                    to the calling function
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  pimdmMapRouteIfIndexGet(L7_uchar8 familyType,
    L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, 
    L7_inet_addr_t *srcMask, L7_uint32 *IfIndex);

/*********************************************************************
* @purpose  Gets the time when a route was learnt
*           for a specified source address.
*
* @param    familyType   @b{(input)} Address Family type
* @param    grpIpAddr    @b{(input)} Group IP Address
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtUptime     @b{(output)}route up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapRouteUptimeGet(L7_uchar8 familyType,
         L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, 
         L7_inet_addr_t *srcMask, L7_uint32 *rtUptime);

/*********************************************************************
* @purpose  Gets the route expiry time for a specified source address.
*
* @param    familyType   @b{(input)} Address Family type
* @param    grpIpAddr    @b{(input)} Group IP Address
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
L7_RC_t pimdmMapRouteExpiryTimeGet(L7_uchar8 familyType,
     L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, 
     L7_inet_addr_t *srcMask, L7_uint32 *rtExpTime);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtAddr              @b{(output)}  rt Address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRtAddressGet(L7_uchar8 familyType,
   L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
   L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rtAddr);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtMask              @b{(output)}  rt mask.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRtMaskGet(L7_uchar8 familyType,
                                  L7_inet_addr_t *ipMRouteGroup, 
                                  L7_inet_addr_t *ipMRouteSource, 
                                  L7_inet_addr_t  *ipMRouteSourceMask, 
                                  L7_inet_addr_t*  rtMask);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtType              @b{(output)}  rt Type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRtTypeGet(L7_uchar8 familyType,
     L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
     L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtType);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Proto.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtproto             @b{(output)} Route Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRtProtoGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtProto);

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Address.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rpfAddr             @b{(output)} rpfAddr.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRpfAddrGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t *rpfAddr);

/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count Get.
*
* @param    familyType         @b{(input)}   Address Family type
* @param    entryCount         @b{(output)}  entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteEntryCountGet(L7_uchar8 familyType,
                                      L7_uint32 *entryCount);

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count Get.
*
* @param    familyType         @b{(input)}   Address Family type
* @param    heCount            @b{(output)}  entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteHighestEntryCountGet(L7_uchar8 familyType, 
                                             L7_uint32 *heCount);
/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Source Mask
* @param    outIntf             @b{(input)} Out interface. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapRouteOutIntfEntryNextGet(L7_uchar8 familyType,
 L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
 L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *outIntf);

/*********************************************************************
* @purpose To check PIMDM is operational.
*
* @param    familyType      @b{(input)} Address Family type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimdmMapPimDmIsOperational (L7_uchar8 familyType);
/*********************************************************************
* @purpose  Returns whether PIM-SM is operational on interface or not
*
*
* @param    familyType      Address Family type
* @param    intIfNum        Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimdmMapInterfaceIsOperational (L7_uchar8 familyType, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Callback function for RTO4 Best Route Changes
*
* @param    none
*                            
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapRto4BestRouteClientCallback (void);

/*********************************************************************
*
* @purpose  Callback function for RTO4 Best Route Changes
*
* @param    none
*                            
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapRto6BestRouteClientCallback (void);

/*********************************************************************
* @purpose  Send event to PIMDM Vendor
*
* @param    familyType   @b{(input)} Address Family type
* @param    eventType    @b{(input)} Event Type 
* @param    msgLen       @b{(input)} Message Length
* @param    eventMsg     @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapEventChangeCallback (L7_uchar8 familyType,
                                     L7_uint32  eventType, 
                                     L7_uint32  msgLen, 
                                     void *eventMsg);

/*********************************************************************
* @purpose  Send AdminScope Boundary event to PIMDM Vendor
*
* @param    eventType    @b{(input)} Event Type 
* @param    msgLen       @b{(input)} Message Length
* @param    eventMsg     @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapAdminScopeEventChangeCallback (L7_uint32 eventType, 
                                               L7_uint32 msgLen, 
                                               void *eventMsg);
#endif /* _L7_PIMDM_API_H_ */

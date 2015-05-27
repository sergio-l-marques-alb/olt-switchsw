/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_api.h
*
* @purpose    Contains prototypes to APIs used by USMDB and snoop
*             component
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#ifndef SNOOPING_API_H
#define SNOOPING_API_H

#include "comm_mask.h"
#include "l3_addrdefs.h"
#include "default_cnfgr.h"
#include "snooping_exports.h"
#include "ping_api.h"
#include "ptin_structs.h"

/* PTin added: IGMP snooping */
#if 1

/**
 * Get a list of channels consumed by a particular vlan and 
 * client index 
 * 
 * @param vlanId       : Group Vlan
 * @param client_index : Client index
 * @param channels     : Channels array
 * @param nChannels    : number of channels
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE 
 */
L7_RC_t ptin_snoop_activeChannels_get(L7_uint16 vlanId, L7_uint32 intIfNum,L7_uint16 client_index, ptin_igmpChannelInfo_t *channels, L7_uint16 *nChannels);

/**
 * Get a list of clients (client indexes) watching a particular 
 * channel 
 * 
 * @param channelIP         : channel
 * @param sVlan             : group vlan
 * @param client_list_bmp   : client list
 * @param number_of_clients : number of clients
 * 
 * @return L7_RC_t L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_snoop_clientsList_get(L7_inet_addr_t *channelIP, L7_uint16 sVlan, L7_uint32 *client_list_bmp, L7_uint16 *number_of_clients);

/**
 * Remove a client from all channels allocated in a particular 
 * interface of a channel group 
 * 
 * @param vlanId       : Service vlan (0 to not be considered)
 * @param client_index : client index
 * @param intIfNum     : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_snoop_client_remove(L7_uint16 vlanId, L7_uint16 client_index, L7_uint32 intIfNum);

/**
 * Add a static channel
 * 
 * @param vlanId    : Service vlan
 * @param channel   : Channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_snoop_static_channel_add(L7_uint16 vlanId, L7_inet_addr_t *channel);

/**
 * Remove a channel
 * 
 * @param vlanId    : Service vlan
 * @param channel   : Channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_snoop_channel_remove(L7_uint16 vlanId, L7_inet_addr_t *channel);

/**
 * Remove all channels associated to a particualr vlan
 * 
 * @param vlanId    : Service vlan (0 to not be considered)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_snoop_channel_removeAll(L7_uint16 vlanId);

#endif

/*********************************************************************
* @purpose  Gets the Snooping Admin mode for the specified snooping
*           instance
*
* @param    adminMode  @b{(output)} Snooping admin mode
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopAdminModeGet(L7_uint32 *adminMode, L7_uchar8 family);
/*********************************************************************
* @purpose  Sets the Snooping Admin mode for the specified snooping
*           instance
*
* @param    adminMode  @b{(input)} Snooping admin mode
* @param    family     @b{(input)} L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopAdminModeSet(L7_uint32 adminMode, L7_uchar8 family);

/* PTin added: IGMP */
#if 1
/*********************************************************************
* @purpose  Sets the Snooping Priority for the specified snooping
*           instance
*
* @param    prio       @b{(input)} Snooping priority
* @param    family     @b{(input)} L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopPrioModeSet(L7_uint8 prio, L7_uchar8 family);
#endif

/******************************************************************************
* @purpose  Gets the interface snoop operational mode of a specified interface
*           and VLAN for a specified snooping instance
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    vlanId    @b{(input)} VLAN ID
* @param    mode      @b{(output)} Snooping interface operational mode
* @param    family    @b{(input)} L7_AF_INET  => IGMP Snooping
*                                 L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopIntfModeGet(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 *mode,
                         L7_uchar8 family);
/*********************************************************************
* @purpose  Sets the Snooping interface configuration mode for the
*           specified interface and snoop instance
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    mode      @b{(input)} Snooping intf mode
* @param    family    @b{(input)} L7_AF_INET  => IGMP Snooping
*                                 L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS If mode was set successfully
* @returns  L7_FAILURE If interface was invalid or if invalid mode
*                      was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfCfgModeSet(L7_uint32 intIfNum, L7_uint32 mode,
                            L7_uchar8 family);
/*********************************************************************
* @purpose  Gets the Snooping interface configuration mode for the
*           specified interface and snoop instance
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    mode      @b{(output)} Snooping intf mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS If mode was set successfully
* @returns  L7_FAILURE If interface was invalid or if invalid mode
*                      was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfCfgModeGet(L7_uint32 intIfNum, L7_uint32 *mode,
                            L7_uchar8 family);
/*********************************************************************
* @purpose  Gets list of interfaces that are enabled for snooping for
*           a specified snoop instance
*
* @param    numIntf   @b{(output)} Number of interfaces
* @param    intfList  @b{(output)} Densely packed array of interfaces
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
*
* @notes    numIntf will be returned as zero if there is no interfaces enabled.
*           Only the first numIntf entries in the intfList have valid
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t snoopIntfEnabledListGet(L7_uint32 *numIntf, L7_uint32 intfList[],
                                L7_uchar8 family);
/*********************************************************************
*
* @purpose  Set the IGMP group membership interval for the specified interface
*           of a snoop instance
*
* @param    intIfNum                  @b{(input)} Internal interface number
* @param    groupMembershipInterval   @b{(input)} New Group Membership Interval
* @param    family                    @b{(input)} L7_AF_INET  => IGMP Snooping
*                                                 L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE   If the specified interval is out of range
*
* @notes    This will set the configured interval time, but will not change
*           the current running timers of any existing Snooping entries.
*
* @end
*********************************************************************/
L7_RC_t snoopIntfApiGroupMembershipIntervalSet(L7_uint32 intIfNum,
                                               L7_uint32 groupMembershipInterval,
                                               L7_uchar8 family);
/*********************************************************************
* @purpose  Get the group membership interval for the specified interface
*           of a snoop instance
*
* @param    intIfNum                 @b{(input)}  Internal interface number
* @param    groupMembershipInterval  @b{(output)} Group Membership Interval
* @param    family                   @b{(input)} L7_AF_INET  => IGMP Snooping
*                                                L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfGroupMembershipIntervalGet(L7_uint32 intIfNum,
                                            L7_uint32 *groupMembershipInterval,
                                            L7_uchar8 family);
/*********************************************************************
* @purpose  Set the query response time for the specified interface
*           of a snoop instance
*
* @param    intIfNum      @b{(input)} Internal interface number
* @param    responseTime  @b{(input)} New response time
* @param    family        @b{(input)} L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If the response time was set
* @returns  L7_FAILURE  If the specified response time is out of range
*
* @notes    This will set the configured response time, but, will not update
*           the response time of any existing Snooping entries.
*
* @end
*********************************************************************/
L7_RC_t snoopIntfApiResponseTimeSet(L7_uint32 intIfNum, L7_uint32 responseTime,
                                    L7_uchar8 family);
/*********************************************************************
* @purpose  Get the query response time for the specified interface of
*           a snoop instance
*
* @param    intIfNum     @b{(input)}  Internal interface number
* @param    responseTime @b{(output)} Response time
* @param    family       @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfResponseTimeGet(L7_uint32 intIfNum, L7_uint32 *responseTime,
                                 L7_uchar8 family);
/*********************************************************************
* @purpose  Get the mcast router expiration time for the specified interface
*
* @param    intIfNum     @b{(input)}  Internal interface number
* @param    expiryTime   @b{(output)} Expiry time
* @param    family       @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @notes    An expiryTime of zero indicates infinite timeout
*
* @end
*********************************************************************/
L7_RC_t snoopIntfMcastRtrExpiryTimeGet(L7_uint32 intIfNum, L7_uint32 *expiryTime,
                                       L7_uchar8 family);
/*********************************************************************
* @purpose  Set the mcast router expiration time for the specified interface
*           of a snoop instance
*
* @param    intIfNum     @b{(input)}  Internal interface number
* @param    expiryTime   @b{(input)}  Expiry time
* @param    family       @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE  If time value out of range
*
* @notes    An expiryTime of zero indicates infinite timeout
*
* @end
*********************************************************************/
L7_RC_t snoopIntfMcastRtrExpiryTimeSet(L7_uint32 intIfNum, L7_uint32 expiryTime,
                                       L7_uchar8 family);
/*********************************************************************
* @purpose  Gets the Fast-Leave Admin mode for the specified interface
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    mode      @b{(output)} Fast-Leave intf admin mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was retrieved successfully
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfFastLeaveAdminModeGet(L7_uint32 intIfNum, L7_uint32 *mode,
                                       L7_uchar8 family);
/**************************************************************************
* @purpose  Sets the Fast-Leave Admin mode for the specified interface
*           for a snoop instance
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    mode      @b{(input)} Fast-Leave intf admin mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was set successfully
* @returns  L7_FAILURE  If interface was invalid or if invalid mode was
*                       specified
*
* @notes    none
*
* @end
**************************************************************************/
L7_RC_t snoopIntfFastLeaveAdminModeSet(L7_uint32 intIfNum, L7_uint32 mode,
                                       L7_uchar8 family);
/*********************************************************************
* @purpose  Sets the specified interface as a multicast router interface
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    mode      @b{(input)}  Mrouter intf admin mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was set successfully
* @returns  L7_FAILURE  If interface was invalid or if invalid mode was
*                       specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfMrouterSet(L7_uint32 intIfNum, L7_uint32 mode,
                            L7_uchar8 family);
/*************************************************************************
* @purpose  Gets the operational Multicast Router Detected mode for the
*           specified interface of a snoop instance
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    vlanId    @b{(input)}  VLAN Id
* @param    mode      @b{(output)} Multicast Router Detected mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was retrieved successfully
* @returns  L7_FAILURE  If interface out of range
*
* @notes    none
*
* @end
*************************************************************************/
L7_RC_t snoopIntfMrouterGet(L7_uint32 intIfNum, L7_uint32 vlanId,
                            L7_uint32 *mode, L7_uchar8 family);
/*****************************************************************************
* @purpose  Gets the configured Multicast Router Detected mode for the
*           specified interface of a snoop Instance
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    mode      @b{(output)} Multicast Router Detected mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was retrieved successfully
* @returns  L7_FAILURE  If interface out of range
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopIntMrouterStatusGet(L7_uint32 intIfNum, L7_uint32 *mode,
                                 L7_uchar8 family);
/*********************************************************************
* @purpose  Flush Snooping Table of a snoop instance
*
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopEntriesFlush(L7_uchar8 family);
/*********************************************************************
* @purpose  Notify Snooping about a L3 Mcast Entry addition
*
* @param    mcastGroupAddr     @b{(input)}  IP Address of mcast group
* @param    mcastSrcAddr       @b{(input)}  IP Address of data source
* @param    srcVlan            @b{(input)}  Ingress VLAN Interface
* @param   *outVlanRtrIntfMask @b{(input)}  Rtr Interface Mask
* @param    flag               @b{(input)}  flag = L7_TRUE for add notify
*                                           flag = L7_FALSE for delete notify
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopMcastNotify(L7_inet_addr_t *mcastGroupAddr,
                         L7_inet_addr_t *mcastSrcAddr,
                         L7_int32        srcVlan,
                         L7_VLAN_MASK_t *outVlanRtrIntfMask,
                         L7_uchar8 flag);

/*****************************************************************************
* @purpose  Get the number of multicast control frames processed by the CPU
*           of a snoop instance
*
* @param    controlFramesProcessed  @b{(output)} Number of control frames
*                                                processed
* @param    family                  @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                                L7_AF_INET6 => MLD Snooping
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopControlFramesProcessed(L7_uint32 *controlFramesProcessed,
                                    L7_uchar8 family);
/*****************************************************************************
* @purpose  Get the number of multicast data frames forwarded by the CPU of
*           a snoop instance
*
* @param    dataFramesForwarded  @b{(output)} Number of data frames forwarded
* @param    family               @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                             L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This API is for reference only and is currently not used.
*
* @end
*****************************************************************************/
L7_RC_t snoopDataFramesForwarded(L7_uint32 *dataFramesForwarded, L7_uchar8 family);
/***************************************************************************
* @purpose  Gets the configured Snooping mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId     @b{(input)}  vlan Id
* @param    vlanMode   @b{(output)} Configured Snooping vlan mode
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS If mode was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
***************************************************************************/
L7_RC_t snoopVlanModeGet(L7_uint32 vlanId, L7_uint32 *vlanMode,
                         L7_uchar8 family);
/******************************************************************************
* @purpose  Sets the configured Snooping mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId     @b{(input)}  vlan Id
* @param    vlanMode   @b{(output)} Configured Snooping vlan mode
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was set successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopVlanModeSet(L7_uint32 vlanId, L7_uint32 vlanMode, L7_uchar8 family);
/******************************************************************************
* @purpose  Sets the configured Snooping fast leave mode for the specified VLAN
*           of a snoop instance
*
* @param    vlanId          @b{(input)}  vlan Id
* @param    fastLeaveMode   @b{(input)}  Configured Snooping Fast Leave Mode
* @param    family          @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                        L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS If mode was set successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopVlanFastLeaveModeSet(L7_uint32 vlanId, L7_uint32 fastLeaveMode,
                                  L7_uchar8 family);
/*****************************************************************************
* @purpose  Gets the configured Snooping fast leave mode for the
*           specified VLAN of a snoop instance
*
* @param    vlanId         @b{(input)}  vlan Id
* @param    fastLeaveMode  @b{(output)} Configured Snooping Fast Leave Mode
* @param    family         @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopVlanFastLeaveModeGet(L7_uint32 vlanId, L7_BOOL *fastLeaveMode,
                                  L7_uchar8 family);
/*********************************************************************
* @purpose  Sets the configured Snooping Group Membership Interval
*           for the specified VLAN of a snoop instance
*
* @param    vlanId                    @b{(input)}  vlan Id
* @param    groupMembershipInterval   @b{(input)}  Configured IGMP Snooping
*                                                  Fast Leave Mode
* @param    family                    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanGroupMembershipIntervalSet(L7_uint32 vlanId,
                                            L7_ushort16 groupMembershipInterval,
                                            L7_uchar8 family);
/*********************************************************************
* @purpose  Gets the configured Snooping Group Membership Interval
*           for the specified VLAN of a snoop instance
*
* @param    vlanId                    @b{(input)}  vlan Id
* @param    groupMembershipInterval   @b{(output)} Configured Snooping Group
*                                                  Membership Interval
* @param    family                    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanGroupMembershipIntervalGet(L7_uint32 vlanId,
                                            L7_ushort16 *groupMembershipInterval,
                                            L7_uchar8 family);
/*****************************************************************************
* @purpose  Sets the configured Snooping Maximum Response Time for the
*           specified VLAN of a snoop instance
*
* @param    vlanId           @b{(input)} vlan Id
* @param    responseTime     @b{(input)} Configured Snooping Maximum
*                                        response Time
* @param    family           @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                         L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopVlanMaximumResponseTimeSet(L7_uint32 vlanId,
                                        L7_ushort16 responseTime,
                                        L7_uchar8 family);
/*********************************************************************
* @purpose  Gets the configured Snooping Maximum Response Time
*           for the specified VLAN of a snoop instance
*
* @param    vlanId           @b{(input)}  vlan Id
* @param    responseTime     @b{(output)} Configured Snooping Maximum
*                                         response Time
* @param    family           @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                         L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanMaximumResponseTimeGet(L7_uint32 vlanId,
                                        L7_ushort16 *responseTime,
                                        L7_uchar8 family);
/*********************************************************************
* @purpose  Sets the configured Snooping Mcast Router Expiry Time
*           for the specified VLAN of a snoop instance
*
* @param    vlanId               @b{(input)}  vlan Id
* @param    mcastRtrExpiryTime   @b{(input)}  Configured Snooping Maximum
*                                             response Time
*
* @param    family               @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                             L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanMcastRtrExpiryTimeSet(L7_uint32 vlanId,
                                       L7_ushort16 mcastRtrExpiryTime,
                                       L7_uchar8 family);
/***************************************************************************
* @purpose  Sets the configured Snooping Mcast Router Expiry Time
*           for the specified VLAN of a snoop instance
*
* @param    vlanId               @b{(input)}  vlan Id
* @param    mcastRtrExpiryTime   @b{(output)} Configured Snooping Maximum
*                                             response Time
* @param    family               @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                             L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
***************************************************************************/
L7_RC_t snoopVlanMcastRtrExpiryTimeGet(L7_uint32 vlanId,
                                       L7_ushort16 *mcastRtrExpiryTime,
                                       L7_uchar8 family);
/*****************************************************************************
* @purpose  Set the static mcast router attached status for the specified
*           interface/vlan of a snoop instance
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    vlanId      @b{(input)}  vlan Id
* @param    status      @b{(input)}  Mcast router attached mode
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t snoopIntfApiVlanStaticMcastRtrSet(L7_uint32 intIfNum, L7_uint32 vlanId,
                                          L7_uchar8 status, L7_uchar8 family);
/*****************************************************************************
* @purpose  Get the static mcast router attached status for the specified
*           interface/vlan of a snoop instance
*
* @param    intIfNum    @b{(input)}   Internal interface number
* @param    vlanId      @b{(input)}   vlan Id
* @param    status      @b{(output)}  Mcast router attached mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t snoopIntfApiVlanStaticMcastRtrGet(L7_uint32 intIfNum, L7_uint32 vlanId,
                                          L7_uchar8 *status, L7_uchar8 family);
/*********************************************************************
* @purpose  Validate whether a snoop instance is supported or not
*
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_TRUE    Corresponding snoop instance exists
* @returns  L7_FALSE   Corresponding snoop instance not supported
*
*
* @end
*********************************************************************/
L7_BOOL snoopProtocolGet(L7_uchar8 family);
/*********************************************************************
* @purpose  Get next supported snoop instance
*
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
* @param    nextFamily  @b{(output)}  Next valid snoop instance
*                                     L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_TRUE   Found a valid snoop instance
* @returns  L7_FALSE  No more snoop instances are present
*
*
* @end
*********************************************************************/
L7_BOOL snoopProtocolNextGet(L7_uchar8 family, L7_uchar8 *nextFamily);
/* Snooping Querier APIs */
/**********************************************************************
* @purpose  Sets the Snooping Querier Admin mode of a snoop instance
*
* @param    adminMode   @b{(input)}   Snooping Querier admin mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t snoopQuerierAdminModeSet(L7_uint32 adminMode, L7_uchar8 family);
/***********************************************************************
* @purpose  Gets the Snooping Querier Admin mode of a snoop instance
*
* @param    adminMode   @b{(output)}  Snooping Querier admin mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
***********************************************************************/
L7_RC_t snoopQuerierAdminModeGet(L7_uint32 *adminMode, L7_uchar8 family);
/*********************************************************************
* @purpose  Gets the Snooping Querier Configured Address of a snoop
*           instance
*
* @param    snoopQuerierAddr @b{(output)}  Querier Address
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierAddressGet(void *snoopQuerierAddr,
                               L7_uchar8 family);
/*********************************************************************
* @purpose  Sets the Snooping Querier Address of a snoop instance
*
* @param    snoopQuerierAddr @b{(input)}   Querier Address
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierAddressSet(void *snoopQuerierAddr,
                               L7_uchar8 family);
/*********************************************************************
* @purpose  Gets the Snooping Querier Configured version of a snoop
*           instance
*
* @param    version     @b{(output)}  Configured querier version
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierVersionGet(L7_uint32 *version, L7_uchar8 family);
/*********************************************************************
* @purpose  Sets the Snooping Querier version of a snoop instance
*
* @param    version     @b{(input)}   Configured querier version
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierVersionSet(L7_uint32 version, L7_uchar8 family);
/*****************************************************************************
* @purpose  Gets the Snooping Querier Query Interval of a snoop instance
*
* @param    queryInterval @b{(output)}  Interval for sending periodic queries
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierQueryIntervalGet(L7_uint32 *queryInterval,
                                     L7_uchar8 family);
/*****************************************************************************
* @purpose  Sets the Snooping Querier Query Interval of a snoop instance
*
* @param    queryInterval @b{(input)}   Interval for sending periodic queries
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierQueryIntervalSet(L7_uint32 queryInterval, L7_uchar8 family);
/*****************************************************************************
* @purpose  Gets the other Querier expiry Interval of a snoop instance
*
* @param    queryInterval @b{(output)}  Interval for declaring last querier
*                                       as expired
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierExpiryIntervalGet(L7_uint32 *expiryInterval,
                                      L7_uchar8 family);
/*****************************************************************************
* @purpose  Sets the other Querier expiry Interval of a snoop instance
*
* @param    queryInterval @b{(input)}   Interval for declaring last querier
*                                       as expired
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierExpiryIntervalSet(L7_uint32 expiryInterval,
                                      L7_uchar8 family);
/*****************************************************************************
* @purpose  Gets the configured Snooping Querier mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(output)} Configured Snooping Querier vlan mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierVlanModeGet(L7_uint32 vlanId, L7_uint32 *vlanMode,
                                L7_uchar8 family);
/*****************************************************************************
* @purpose  Sets the configured Snooping Querier mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(input)}  Configured Snooping Querier vlan mode
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierVlanModeSet(L7_uint32 vlanId, L7_uint32 vlanMode,
                                L7_uchar8 family);
/*****************************************************************************
* @purpose  Gets the configured Snooping Querier election mode for the
*           specified vlanId of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(output)} Configured Snooping Querier vlan election
*                                    participate mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierVlanElectionModeGet(L7_uint32 vlanId, L7_uint32 *vlanMode,
                                        L7_uchar8 family);
/*****************************************************************************
* @purpose  Sets the configured Snooping Querier election mode for the
*           specified vlanId of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(input)}  Configured Snooping Querier vlan election
*                                    participate mode
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierVlanElectionModeSet(L7_uint32 vlanId, L7_uint32 vlanMode,
                                        L7_uchar8 family);
/******************************************************************************
* @purpose  Gets the Snooping Querier Configured Address for specified vlan Id
*           of a snoop instance
*
* @param    vlanId                @b{(input)}   vlan Id
* @param    snoopQuerierAddr      @b{(output)}  Querier vlan address
* @param    family                @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                               L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierVlanAddressGet(L7_uint32 vlanId,
                                   void *snoopQuerierAddr,
                                   L7_uchar8 family);
/******************************************************************************
* @purpose  Gets the Snooping Querier Configured Address for specified vlan Id
*           of a snoop instance
*
* @param    vlanId                @b{(input)}  vlan Id
* @param    snoopQuerierAddr      @b{(input)}  Querier vlan address
* @param    family                @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                              L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierVlanAddressSet(L7_uint32 vlanId,
                                   void *snoopQuerierAddr,
                                   L7_uchar8 family);
/******************************************************************************
* @purpose  Gets the detected last Querier Address for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    querierAddr      @b{(output)}  last Querier vlan address
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierLastQuerierAddressGet(L7_uint32 vlanId,
                                          void *querierAddr,
                                          L7_uchar8 family);
/******************************************************************************
* @purpose  Gets the detected last Querier's version for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    querierVersion   @b{(output)}  last Querier protocol version
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierLastQuerierVersionGet(L7_uint32 vlanId,
                                          L7_uint32 *querierVersion,
                                          L7_uchar8 family);
/******************************************************************************
* @purpose  Gets the Operationa version of Querier  for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    version          @b{(output)}  Operational Version
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierOperVersionGet(L7_uint32 vlanId, L7_uint32 *version,
                                   L7_uchar8 family);
/******************************************************************************
* @purpose  Gets the Operational State of Querier  for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    state            @b{(output)}  Operational State
*                                          L7_SNOOP_QUERIER_STATE_t
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierOperStateGet(L7_uint32 vlanId, L7_uint32 *state,
                                 L7_uchar8 family);
/******************************************************************************
* @purpose  Gets the Operational value of max response time for specified
*           vlan Id of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    maxRespTime      @b{(output)}  Operational Max Response Time value
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    When success, if maxRespTime == 0, it means that the max response
*           time is less than 1 sec
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierOperMaxRespTimeGet(L7_uint32 vlanId, L7_uint32 *maxRespTime,
                                       L7_uchar8 family);
/*********************************************************************
* @purpose  Determine if the interface is valid for Snooping
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE   Supported interface
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
extern L7_BOOL snoopIsValidIntf(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Sets whether the optional Router Alert field is required.
*
*
* @param    checkRtrAlert   @b{(input)}   L7_TRUE/L7_FALSE to enable/disable
*                                          router alert checking in IGMP frames
* @param    family          @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                         L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    Family field is not used currently
*
* @end
*********************************************************************/
extern L7_RC_t snoopRouterAlertMandatorySet(L7_BOOL checkRtrAlert, L7_uchar8 family);
/*********************************************************************
* @purpose  Indicates whether the optional Router Alert field is required.
*
* @param    checkRtrAlert   @b{(output)}   L7_TRUE/L7_FALSE to enable/disable
*                                          router alert checking in IGMP frames
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    Family field is not used currently
*
* @end
*********************************************************************/
extern L7_RC_t snoopRouterAlertMandatoryGet(L7_BOOL *checkRtrAlert, L7_uchar8 family);
/*****************************************************************************
* @purpose  Get the static mcast router attached status for the specified
*           interface of a snoop instance. List of VLANs on which
*           static mrouter is enabled are turned on in the vlanStaticMcastRtr
*           mask.
*
* @param    intIfNum    @b{(input)}   Internal interface number
* @param    vlanMask    @b{(output)}  Mcast router attached mode mask
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t snoopIntfApiVlanStaticMcastRtrMaskGet(L7_uint32 intIfNum,
                                              L7_VLAN_MASK_t *vlanStaticMcastRtr,
                                              L7_uchar8 family);

L7_RC_t snoopPortOpen(L7_uint32 serviceId, L7_uint32 intIfNum, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr, L7_BOOL isStatic, L7_BOOL isProtection);
L7_RC_t snoopPortClose(L7_uint32 serviceId, L7_uint32 intIfNum, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr, L7_BOOL isProtection);

#endif /* SNOOPING_API_H */



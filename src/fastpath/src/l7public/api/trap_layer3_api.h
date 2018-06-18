/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename trapapi_layer3.h
*
* @purpose Trap Manager Layer 3 functions
*
* @component trapapi_layer3.h
*
* @comments none
*
* @created 04/18/2001
*
* @author kdesai
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef TRAP_LAYER3_API_H
#define TRAP_LAYER3_API_H
#include "l3_addrdefs.h"
#define TRAPMGR_GENERIC_NONE                                          0

#define TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_CHANGE                     0
#define TRAPMGR_SPECIFIC_NEIGHBOR_STATE_CHANGE                        1
#define TRAPMGR_SPECIFIC_INTERFACE_CONFIG_ERROR                       2
#define TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_CONFIG_ERROR               3
#define TRAPMGR_SPECIFIC_INTERFACE_AUTHFAILURE                        4
#define TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_AUTHFAILURE                5
#define TRAPMGR_SPECIFIC_RECEIVED_BADPACKET                           6
#define TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_RECEIVED_BADPACKET         7
#define TRAPMGR_SPECIFIC_RETRANSMIT                                   8
#define TRAPMGR_SPECIFIC_VIRTUAL_RETRANSMIT                           9
#define TRAPMGR_SPECIFIC_ORIGINATE_LSA                               10
#define TRAPMGR_SPECIFIC_MAX_AGE_LSA                                 11
#define TRAPMGR_SPECIFIC_LSDB_OVERFLOW                               12
#define TRAPMGR_SPECIFIC_LSDB_APPROACHING_OVERFLOW                   13
#define TRAPMGR_SPECIFIC_INTERFACE_STATE_CHANGE                      14
#define TRAPMGR_SPECIFIC_INTERFACE_RECEIVED_PACKET                   15
#define TRAPMGR_SPECIFIC_ROUNDTABLE_ENTRY_INFO                       16
#define TRAPMGR_SPECIFIC_VRRP_NEW_MASTER                             17
#define TRAPMGR_SPECIFIC_VRRP_AUTH_FAILURE                           18
#define TRAPMGR_SPECIFIC_BGP_ESTABLISHED                             22
#define TRAPMGR_SPECIFIC_BGP_BACKWARD_TRANSITION                     23
#include "trapmgr_exports.h"

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfTrapModeSet(L7_uint32 mode, L7_uint32 trapType);

/*********************************************************************
*
* @purpose  Allows a user to determine whether the specified trap
*           is enabled or disabled
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfTrapModeGet(L7_uint32 *mode, L7_uint32 trapType);

/*********************************************************************
* @purpose  Allows a user to determine configured ospf trap flags
*
* @param    trapFlags      bit mask indicating ospf traps that are 
*                          enabled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfTrapFlagsGet(L7_uint32 *trapFlags);

/*********************************************************************
*
* @purpose  Signifies a change in the state of an OSPF virtual interface.
*
* @param  Routerid        The ip address of the router originating the trap.
*         VirtIfAreaId    The area ID of the Virtual Interface to which
*                         this RTO is attached.
*         VirtIfNeighbor  The Neighbor Router ID to which this interface
*                         is connected.
*         VirtIfState     The Virtual Interface Hello protocol State machine state.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the interface state regresses or
*         progresses to a terminal state.
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtIfStateChange(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor, L7_uint32 virtIfState);

/*********************************************************************
*
* @purpose  Signifies a change in the state of a non-virtual OSPF neighbor.
*
* @param    routerId    The ip address of the router originating the trap.
*           nbrIpAddr   The IP address this neighbor is using as this
*                       IP Source Address.
*           intIfNum    On an interface having and IP Address, "0".
*                                On an interface without an address, the
*                                corresponding value of ifIndex for this
*                                interface in the Internet Standard MIB
*                                ifTable.  This index is usually used in
*                                an unnumbered interface as a reference to
*                                the local router's control application
*                                interface table.
*           nbrRtrId     A 32-bit integer uniquely indentifying the neighboring
*                        router in the AS (Autonomous System).
*           nbrState     The new NBR state.  NBR states are defined in the
*                        OSPF MIB.(pg. 126)
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the neighbor state regresses or
*         progresses to a terminal state.  When a neighbor transitions
*         from or to Full on non-broadcast multi-access and broadcast
*         networks, the trap should be generated by the designated router.
*         A designated router transitioning to Down will be noted by
*         ospfIfStateChange.
*
* @end
*********************************************************************/
L7_RC_t trapMgrNbrStateChange(L7_uint32 routerId, L7_uint32 nbrIpAddr,
                              L7_uint32 intIfNum,
                              L7_uint32 nbrRtrId, L7_uint32 nbrState);

/*********************************************************************
*
* @purpose  Signifies that there  has been a change in the state of a
*           virtual OSPF neighbor.
*
* @param    routerId      The originator of the trap
*           virtNbrArea
*           virtNbrRtrId
*           virtNbrState  The new state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This  trap  should  be generated  when  the  neighbor
*          state regresses (e.g., goes from Attempt or Full
*          to  1-Way  or Down)  or progresses to a terminal
*          state (e.g., 2-Way or Full).  When an  neighbor
*          transitions from  or  to Full on non-broadcast
*          multi-access and broadcast networks, the trap should
*          be generated  by the designated router.  A designated
*          router transitioning to Down will be  noted  by
*          ospfIfStateChange."
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtNbrStateChange(L7_uint32 routerId, L7_uint32 virtNbrArea,
                                  L7_uint32 virtNbrRtrId, L7_uint32 virtNbrState);

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a non-virtual
*           interface from a router whose configuration parameters  conflict
*           with this router's configuration parameters.
*
* @param    routerId          The originator of the trap.
*           ifIpAddress
*           intIfNum          internal interface number
*           packetSrc         The source IP Address
*           configErrorType   Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming.
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfConfigError(L7_uint32 routerId, L7_uint32 ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 packetSrc,
                             L7_uint32 configErrorType, L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a virtual
*           interface from a router whose configuration parameters  conflict
*           with this router's configuration parameters.
*
* @param    routerId        The originator of the Trap
*           virtIfAreaId
*           virtIfNeighbor
*           configErrorType Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming.
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtIfConfigError(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor,
                                 L7_uint32 configErrorType,
                                 L7_uint32 packetType);

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a non-virtual
*          interface from a router whose authentication key or
*          authentication type conflicts with this router's
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          ifIpAddress
*          intIfNum         internal interface number
*          packetSrc        The source IP Address
*          configErrorType  authType mismatch or auth failure
*          packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfAuthFailure(L7_uint32 routerId, L7_uint32 ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 packetSrc,
                             L7_uint32 configErrorType, L7_uint32 packetType);

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a virtual
*          interface from a router whose authentication key or
*          authentication type conflicts with this router's
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          virtIfAreaId
*          virtIfNeighbor
*          configErrorType  Auth type mismatch or auth failure
*          packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtIfAuthFailure(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor,
                                 L7_uint32 configErrorType,
                                 L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a
*           non-virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the Trap
*           ifIpAddress
*           intIfNum        internal interface number
*           packetSrc       The source ip address
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfRxBadPacket(L7_uint32 routerId, L7_uint32 ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 packetSrc,
                             L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a
*           virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtIfRxBadPacket(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor, L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a
*           non-virtual interface.
*
* @param    routerId        The originator of the trap
*           ifIpAddress
*           intIfNum        internal interface number
*           nbrRtrId        Destination
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated
*           with an LSDB entry.  The LS type, LS ID, and Router ID
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t trapMgrTxRetransmit(L7_uint32 routerId, L7_uint32 ifIpAddress,
                            L7_uint32 intIfNum, L7_uint32 nbrRtrId,
                            L7_uint32 packetType, L7_uint32 lsdbType,
                            L7_uint32 lsdbLsid, L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a
*           virtual interface.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated
*           with an LSDB entry.  The LS type, LS ID, and Router ID
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtTxRetransmit(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                L7_uint32 virtIfNeighbor, L7_uint32 packetType,
                                L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                                L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies that a  new LSA  has  been originated by this router.
*
* @param    routerId        The originator of the trap.
*           lsdbAreaId      0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This trap should not be invoked for simple refreshes of
*           LSAs  (which happesn every 30 minutes), but
*           instead will only be invoked  when  an  LSA  is
*           (re)originated due to a topology change.  Addi-
*           tionally, this trap does not include LSAs  that
*           are  being  flushed  because  they have reached
*           MaxAge.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOriginateLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                            L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                            L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies that one of the LSA in the router's link-state
*           database has aged to MaxAge.
*
* @param    routerId         The originator of the trap
*           lsdbAreaId       0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrMaxAgeLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                         L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                         L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies that the number of LSAs in the router's link-state
*           data-base has exceeded ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrLsdbOverflow(L7_uint32 routerId, L7_uint32 extLsdbLimit);


/*********************************************************************
*
* @purpose  Signifies that  the  number of LSAs in the router's link-
*           state database has exceeded ninety  percent  of
*           ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrLsdbApproachingOverflow(L7_uint32 routerId,
                                       L7_uint32 extLsdbLimit);


/*********************************************************************
*
* @purpose  Signifies that there has been a change in the state of a
*           non-virtual OSPF interface.
*
* @param    routerId      The originator of the trap
*           ifIpAddress
*           intIfNum      internal interface number
*           ifState       New state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This trap should  be  generated when  the interface state
*          regresses (e.g., goes from Dr to Down) or progresses  to  a  terminal
*          state  (i.e.,  Point-to-Point, DR Other, Dr, or Backup).
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfStateChange(L7_uint32 routerId, L7_uint32 ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 ifState);


/*********************************************************************
*
* @purpose  Signifies that an OSPF packet was received on a non-virtual interface.
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfRxPacket();


/*********************************************************************
*
* @purpose  Signifies that an entry has been made in the OSPF routing table.
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ATIC's proprietary Routing Table Entry Information trap.
*
* @end
*********************************************************************/
L7_RC_t trapMgrRtbEntryInfo();

/*********************************************************************
* @purpose  Returns Trap Manager's trap VRRP state
*
* @param    *mode     L7_ENABLE/L7_DISABLE
* @param    trapType  VRRP trap type specified
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrGetTrapVrrp(L7_uint32 *mode, L7_uint32 trapType);

/*********************************************************************
* @purpose  Sets Trap Manager's VRRP state
*
*
* @param    mode      L7_ENABLE/L7_DISABLE
* @param    trapType  VRRP trap type specified
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrSetTrapVrrp(L7_uint32 mode, L7_uint32 trapType);

/*********************************************************************
*
* @purpose  Signifies that the sending agent has transitioned to the
*           'Master' state.
*
* @param    vrrpOperMasterIpAddr   The master router's real (primary) IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t trapMgrVrrpTrapNewMasterTrap(L7_uint32 vrrpOperMasterIpAddr);

/*********************************************************************
*
* @purpose  Signifies that a packet has been received from a router
*           whose authentication key or authentication type conflicts
*           with this router's authentication key or authentication type.
*
* @param    vrrpTrapPacketSrc       The address of an inbound VRRP packet
* @param    vrrpTrapAuthErrorType   The type of configuration conflict
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t trapMgrVrrpTrapAuthFailureTrap(L7_uint32 vrrpTrapPacketSrc, L7_int32 vrrpTrapAuthErrorType);

/*********************************************************************
*
* @purpose  Allows a user to know whether the specified trap
*           is enabled or disabled
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  BGP trap type specified in L7_BGP_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrBgpTrapModeGet(L7_uint32 *mode, L7_uint32 trapType);

/*********************************************************************
*
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  BGP trap type specified in L7_BGP_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrBgpTrapModeSet(L7_uint32 mode, L7_uint32 trapType);

/*********************************************************************
*
* @purpose  Signifies that the BGP FSM  has transitioned to the
*           Established state.
*
* @param    peerId    Peer Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t trapMgrBgpTrapBgpEstablishedTrap(L7_uint32 peerRemoteAddr, L7_char8* peerLastError, L7_uint32 peerState);

/*********************************************************************
*
* @purpose  Signifies that BGP FSM moves from a higher numbered state
*           to a lower numbered state.
*
* @param    peerId              Peer Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t trapMgrBgpTrapBgpBackwardTransitionTrap(L7_uint32 peerRemoteAddr, L7_char8* peerLastError, L7_uint32 peerState);

/*********************************************************************
*
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3TrapModeSet(L7_uint32 mode, L7_uint32 trapType);

/*********************************************************************
*
* @purpose  Allows a user to determine whether the specified trap
*           is enabled or disabled
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3TrapModeGet(L7_uint32 *mode, L7_uint32 trapType);

/*********************************************************************
* @purpose  Allows a user to determine configured ospf trap flags
*
* @param    trapFlags      bit mask indicating ospf traps that are 
*                          enabled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3TrapFlagsGet(L7_uint32 *trapFlags);

/*********************************************************************
*
* @purpose  Signifies a change in the state of an OSPF virtual interface.
*
* @param  Routerid        The ip address of the router originating the trap.
*         VirtIfAreaId    The area ID of the Virtual Interface to which
*                         this RTO is attached.
*         VirtIfNeighbor  The Neighbor Router ID to which this interface
*                         is connected.
*         VirtIfState     The Virtual Interface Hello protocol State machine state.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the interface state regresses or
*         progresses to a terminal state.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtIfStateChange(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor, L7_uint32 virtIfState);

/*********************************************************************
*
* @purpose  Signifies a change in the state of a non-virtual OSPF neighbor.
*
* @param    routerId    The ip address of the router originating the trap.
*           nbrIpAddr   The IP address this neighbor is using as this
*                       IP Source Address.
*           intIfNum    On an interface having and IP Address, "0".
*                                On an interface without an address, the
*                                corresponding value of ifIndex for this
*                                interface in the Internet Standard MIB
*                                ifTable.  This index is usually used in
*                                an unnumbered interface as a reference to
*                                the local router's control application
*                                interface table.
*           nbrRtrId     A 32-bit integer uniquely indentifying the neighboring
*                        router in the AS (Autonomous System).
*           nbrState     The new NBR state.  NBR states are defined in the
*                        OSPF MIB.(pg. 126)
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the neighbor state regresses or
*         progresses to a terminal state.  When a neighbor transitions
*         from or to Full on non-broadcast multi-access and broadcast
*         networks, the trap should be generated by the designated router.
*         A designated router transitioning to Down will be noted by
*         ospfIfStateChange.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3NbrStateChange(L7_uint32 routerId, L7_in6_addr_t nbrIpAddr,
                              L7_uint32 intIfNum,
                              L7_uint32 nbrRtrId, L7_uint32 nbrState);

/*********************************************************************
*
* @purpose  Signifies that there  has been a change in the state of a
*           virtual OSPF neighbor.
*
* @param    routerId      The originator of the trap
*           virtNbrArea
*           virtNbrRtrId
*           virtNbrState  The new state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This  trap  should  be generated  when  the  neighbor
*          state regresses (e.g., goes from Attempt or Full
*          to  1-Way  or Down)  or progresses to a terminal
*          state (e.g., 2-Way or Full).  When an  neighbor
*          transitions from  or  to Full on non-broadcast
*          multi-access and broadcast networks, the trap should
*          be generated  by the designated router.  A designated
*          router transitioning to Down will be  noted  by
*          ospfIfStateChange."
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtNbrStateChange(L7_uint32 routerId, L7_uint32 virtNbrArea,
                                  L7_uint32 virtNbrRtrId, L7_uint32 virtNbrState);

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a non-virtual
*           interface from a router whose configuration parameters  conflict
*           with this router's configuration parameters.
*
* @param    routerId          The originator of the trap.
*           ifIpAddress
*           intIfNum          internal interface number
*           packetSrc         The source IP Address
*           configErrorType   Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfConfigError(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                             L7_uint32 intIfNum, L7_in6_addr_t packetSrc,
                             L7_uint32 configErrorType, L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a virtual
*           interface from a router whose configuration parameters  conflict
*           with this router's configuration parameters.
*
* @param    routerId        The originator of the Trap
*           virtIfAreaId
*           virtIfNeighbor
*           configErrorType Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtIfConfigError(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor,
                                 L7_uint32 configErrorType,
                                 L7_uint32 packetType);

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a non-virtual
*          interface from a router whose authentication key or
*          authentication type conflicts with this router's
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          ifIpAddress
*          intIfNum         internal interface number
*          packetSrc        The source IP Address
*          configErrorType  authType mismatch or auth failure
*          packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfAuthFailure(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                             L7_uint32 intIfNum, L7_in6_addr_t packetSrc,
                             L7_uint32 configErrorType, L7_uint32 packetType);

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a virtual
*          interface from a router whose authentication key or
*          authentication type conflicts with this router's
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          virtIfAreaId
*          virtIfNeighbor
*          configErrorType  Auth type mismatch or auth failure
*          packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtIfAuthFailure(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor,
                                 L7_uint32 configErrorType,
                                 L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a
*           non-virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the Trap
*           ifIpAddress
*           intIfNum        internal interface number
*           packetSrc       The source ip address
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfRxBadPacket(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                             L7_uint32 intIfNum, L7_in6_addr_t packetSrc,
                             L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a
*           virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtIfRxBadPacket(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor, L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a
*           non-virtual interface.
*
* @param    routerId        The originator of the trap
*           ifIpAddress
*           intIfNum        internal interface number
*           nbrRtrId        Destination
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated
*           with an LSDB entry.  The LS type, LS ID, and Router ID
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3TxRetransmit(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                            L7_uint32 intIfNum, L7_uint32 nbrRtrId,
                            L7_uint32 packetType, L7_uint32 lsdbType,
                            L7_uint32 lsdbLsid, L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a
*           virtual interface.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated
*           with an LSDB entry.  The LS type, LS ID, and Router ID
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtTxRetransmit(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                L7_uint32 virtIfNeighbor, L7_uint32 packetType,
                                L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                                L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies that a  new LSA  has  been originated by this router.
*
* @param    routerId        The originator of the trap.
*           lsdbAreaId      0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This trap should not be invoked for simple refreshes of
*           LSAs  (which happesn every 30 minutes), but
*           instead will only be invoked  when  an  LSA  is
*           (re)originated due to a topology change.  Addi-
*           tionally, this trap does not include LSAs  that
*           are  being  flushed  because  they have reached
*           MaxAge.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3OriginateLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                            L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                            L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies that one of the LSA in the router's link-state
*           database has aged to MaxAge.
*
* @param    routerId         The originator of the trap
*           lsdbAreaId       0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3MaxAgeLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                         L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                         L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies that the number of LSAs in the router's link-state
*           data-base has exceeded ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3LsdbOverflow(L7_uint32 routerId, L7_uint32 extLsdbLimit);


/*********************************************************************
*
* @purpose  Signifies that  the  number of LSAs in the router's link-
*           state database has exceeded ninety  percent  of
*           ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3LsdbApproachingOverflow(L7_uint32 routerId,
                                       L7_uint32 extLsdbLimit);


/*********************************************************************
*
* @purpose  Signifies that there has been a change in the state of a
*           non-virtual OSPF interface.
*
* @param    routerId      The originator of the trap
*           ifIpAddress
*           intIfNum      internal interface number
*           ifState       New state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This trap should  be  generated when  the interface state
*          regresses (e.g., goes from Dr to Down) or progresses  to  a  terminal
*          state  (i.e.,  Point-to-Point, DR Other, Dr, or Backup).
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfStateChange(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 ifState);


/*********************************************************************
*
* @purpose  Signifies that an OSPF packet was received on a non-virtual interface.
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfRxPacket();


/*********************************************************************
*
* @purpose  Signifies that an entry has been made in the OSPF routing table.
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ATIC's proprietary Routing Table Entry Information trap.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3RtbEntryInfo();

/* End Function Prototypes */

#endif /* TRAP_LAYER3_API_H */

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: snmp_trap_api_l3.c
*
* Purpose: API interface for SNMP Agent Traps
*
* Created by: Colin Verne 06/07/2001
*
* Component: SNMP
*
*********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "trap_api_routing.h"

#ifdef L7_OSPF_PACKAGE
#include "privatetrap_ospf.h"
#endif

#ifdef L7_VRRP_PACKAGE
#include "privatetrap_vrrp.h"
#endif

#include "snmp_api.h"
#include "usmdb_util_api.h"
#include "dtlapi.h"
#include "osapi.h"


typedef enum
{
  L7_SNMP_TRAP_ROUTING_OspfIfStateChange = 0,
  L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange,
  L7_SNMP_TRAP_ROUTING_OspfNbrStateChange,
  L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange,
  L7_SNMP_TRAP_ROUTING_OspfIfConfigError,
  L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError,
  L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure,
  L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure,
  L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket,
  L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket,
  L7_SNMP_TRAP_ROUTING_OspfTxRetransmit,
  L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit,
  L7_SNMP_TRAP_ROUTING_OspfOriginateLsa,
  L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa,
  L7_SNMP_TRAP_ROUTING_OspfLsdbOverflow,
  L7_SNMP_TRAP_ROUTING_OspfLsdbApproachingOverflow,
  L7_SNMP_TRAP_ROUTING_VrrpTrapNewMaster,
  L7_SNMP_TRAP_ROUTING_VrrpTrapAuthFailure
} L7_SNMP_TRAP_TYPE_ROUTING_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfIfIpAddress;
  L7_uint32 ospfAddressLessIf;
  L7_uint32 ospfIfState;
} L7_SNMP_TRAP_ROUTING_OspfIfStateChange_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfVirtIfAreaId;
  L7_uint32 ospfVirtIfNeighbor;
  L7_uint32 ospfVirtIfState;
} L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfNbrIpAddr;
  L7_uint32 ospfNbrAddressLessIndex;
  L7_uint32 ospfNbrRtrId;
  L7_uint32 ospfNbrState;
} L7_SNMP_TRAP_ROUTING_OspfNbrStateChange_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfVirtNbrArea;
  L7_uint32 ospfVirtNbrRtrId;
  L7_uint32 ospfVirtNbrState;
} L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfIfIpAddress;
  L7_uint32 ospfAddressLessIf;
  L7_uint32 ospfPacketSrc;
  L7_uint32 ospfConfigErrorType;
  L7_uint32 ospfPacketType;
} L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfVirtIfAreaId;
  L7_uint32 ospfVirtIfNeighbor;
  L7_uint32 ospfConfigErrorType;
  L7_uint32 ospfPacketType;
} L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfIfIpAddress;
  L7_uint32 ospfAddressLessIf;
  L7_uint32 ospfPacketSrc;
  L7_uint32 ospfConfigErrorType;
  L7_uint32 ospfPacketType;
} L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfVirtIfAreaId;
  L7_uint32 ospfVirtIfNeighbor;
  L7_uint32 ospfConfigErrorType;
  L7_uint32 ospfPacketType;
} L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfIfIpAddress;
  L7_uint32 ospfAddressLessIf;
  L7_uint32 ospfPacketSrc;
  L7_uint32 ospfPacketType;
} L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfVirtIfAreaId;
  L7_uint32 ospfVirtIfNeighbor;
  L7_uint32 ospfPacketType;
} L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfIfIpAddress;
  L7_uint32 ospfAddressLessIf;
  L7_uint32 ospfNbrRtrId;
  L7_uint32 ospfPacketType;
  L7_uint32 ospfLsdbType;
  L7_uint32 ospfLsdbLsid;
  L7_uint32 ospfLsdbRouterId;
} L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfVirtIfAreaId;
  L7_uint32 ospfVirtIfNeighbor;
  L7_uint32 ospfPacketType;
  L7_uint32 ospfLsdbType;
  L7_uint32 ospfLsdbLsid;
  L7_uint32 ospfLsdbRouterId;
} L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfLsdbAreaId;
  L7_uint32 ospfLsdbType;
  L7_uint32 ospfLsdbLsid;
  L7_uint32 ospfLsdbRouterId;
} L7_SNMP_TRAP_ROUTING_OspfOriginateLsa_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfLsdbAreaId;
  L7_uint32 ospfLsdbType;
  L7_uint32 ospfLsdbLsid;
  L7_uint32 ospfLsdbRouterId;
} L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfExtLsdbLimit;
} L7_SNMP_TRAP_ROUTING_OspfLsdbOverflow_t;

typedef struct
{
  L7_uint32 ospfRouterId;
  L7_uint32 ospfExtLsdbLimit;
} L7_SNMP_TRAP_ROUTING_OspfLsdbApproachingOverflow_t;

typedef struct
{
  L7_uint32 vrrpOperMasterIpAddr;
} L7_SNMP_TRAP_ROUTING_VrrpTrapNewMaster_t;

typedef struct
{
  L7_uint32 vrrpTrapPacketSrc;
  L7_int32 vrrpTrapAuthErrorType;
} L7_SNMP_TRAP_ROUTING_VrrpTrapAuthFailure_t;


/*********************************************************************
*
* @purpose  Handles calling the trap code to send traps
*
* @param    trap_id   enumeration of the trap to send
* @param    data      data structure holding trap information
*
* @returns  void
*
* @notes    The datastructure is determined by the trap_id indicated.
*
* @end
*********************************************************************/
void SnmpTrapSendCallback_routing(L7_uint32 trap_id, void *data)
{
  switch (trap_id)
  {
  #ifdef L7_OSPF_PACKAGE
  case L7_SNMP_TRAP_ROUTING_OspfIfStateChange:
    (void)snmp_ospfIfStateChangeTrapSend(((L7_SNMP_TRAP_ROUTING_OspfIfStateChange_t*)data)->ospfRouterId,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfStateChange_t*)data)->ospfIfIpAddress,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfStateChange_t*)data)->ospfAddressLessIf,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfStateChange_t*)data)->ospfIfState);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange:
    (void)snmp_ospfVirtIfStateChangeTrapSend(((L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange_t*)data)->ospfRouterId,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange_t*)data)->ospfVirtIfAreaId,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange_t*)data)->ospfVirtIfNeighbor,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange_t*)data)->ospfVirtIfState);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfNbrStateChange:
    (void)snmp_ospfNbrStateChangeTrapSend(((L7_SNMP_TRAP_ROUTING_OspfNbrStateChange_t*)data)->ospfRouterId,
                                          ((L7_SNMP_TRAP_ROUTING_OspfNbrStateChange_t*)data)->ospfNbrIpAddr,
                                          ((L7_SNMP_TRAP_ROUTING_OspfNbrStateChange_t*)data)->ospfNbrAddressLessIndex,
                                          ((L7_SNMP_TRAP_ROUTING_OspfNbrStateChange_t*)data)->ospfNbrRtrId,
                                          ((L7_SNMP_TRAP_ROUTING_OspfNbrStateChange_t*)data)->ospfNbrState);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange:
    (void)snmp_ospfVirtNbrStateChangeTrapSend(((L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange_t*)data)->ospfRouterId,
                                              ((L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange_t*)data)->ospfVirtNbrArea,
                                              ((L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange_t*)data)->ospfVirtNbrRtrId,
                                              ((L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange_t*)data)->ospfVirtNbrState);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfIfConfigError:
    (void)snmp_ospfIfConfigErrorTrapSend(((L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t*)data)->ospfRouterId,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t*)data)->ospfIfIpAddress,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t*)data)->ospfAddressLessIf,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t*)data)->ospfPacketSrc,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t*)data)->ospfConfigErrorType,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t*)data)->ospfPacketType);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError:
    (void)snmp_ospfVirtIfConfigErrorTrapSend(((L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError_t*)data)->ospfRouterId,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError_t*)data)->ospfVirtIfAreaId,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError_t*)data)->ospfVirtIfNeighbor,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError_t*)data)->ospfConfigErrorType,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError_t*)data)->ospfPacketType);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure:
    (void)snmp_ospfIfAuthFailureTrapSend(((L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t*)data)->ospfRouterId,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t*)data)->ospfIfIpAddress,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t*)data)->ospfAddressLessIf,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t*)data)->ospfPacketSrc,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t*)data)->ospfConfigErrorType,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t*)data)->ospfPacketType);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure:
    (void)snmp_ospfVirtIfAuthFailureTrapSend(((L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure_t*)data)->ospfRouterId,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure_t*)data)->ospfVirtIfAreaId,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure_t*)data)->ospfVirtIfNeighbor,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure_t*)data)->ospfConfigErrorType,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure_t*)data)->ospfPacketType);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket:
    (void)snmp_ospfIfRxBadPacketTrapSend(((L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket_t*)data)->ospfRouterId,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket_t*)data)->ospfIfIpAddress,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket_t*)data)->ospfAddressLessIf,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket_t*)data)->ospfPacketSrc,
                                         ((L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket_t*)data)->ospfPacketType);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket:
    (void)snmp_ospfVirtIfRxBadPacketTrapSend(((L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket_t*)data)->ospfRouterId,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket_t*)data)->ospfVirtIfAreaId,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket_t*)data)->ospfVirtIfNeighbor,
                                             ((L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket_t*)data)->ospfPacketType);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfTxRetransmit:
    (void)snmp_ospfTxRetransmitTrapSend(((L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t*)data)->ospfRouterId,
                                        ((L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t*)data)->ospfIfIpAddress,
                                        ((L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t*)data)->ospfAddressLessIf,
                                        ((L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t*)data)->ospfNbrRtrId,
                                        ((L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t*)data)->ospfPacketType,
                                        ((L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t*)data)->ospfLsdbType,
                                        ((L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t*)data)->ospfLsdbLsid,
                                        ((L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t*)data)->ospfLsdbRouterId);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit:
    (void)snmp_ospfVirtTxRetransmitTrapSend(((L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t*)data)->ospfRouterId,
                                            ((L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t*)data)->ospfVirtIfAreaId,
                                            ((L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t*)data)->ospfVirtIfNeighbor,
                                            ((L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t*)data)->ospfPacketType,
                                            ((L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t*)data)->ospfLsdbType,
                                            ((L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t*)data)->ospfLsdbLsid,
                                            ((L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t*)data)->ospfLsdbRouterId);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfOriginateLsa:
    (void)snmp_ospfOriginateLsaTrapSend(((L7_SNMP_TRAP_ROUTING_OspfOriginateLsa_t*)data)->ospfRouterId,
                                        ((L7_SNMP_TRAP_ROUTING_OspfOriginateLsa_t*)data)->ospfLsdbAreaId,
                                        ((L7_SNMP_TRAP_ROUTING_OspfOriginateLsa_t*)data)->ospfLsdbType,
                                        ((L7_SNMP_TRAP_ROUTING_OspfOriginateLsa_t*)data)->ospfLsdbLsid,
                                        ((L7_SNMP_TRAP_ROUTING_OspfOriginateLsa_t*)data)->ospfLsdbRouterId);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa:
    (void)snmp_ospfMaxAgeLsaTrapSend(((L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa_t*)data)->ospfRouterId,
                                     ((L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa_t*)data)->ospfLsdbAreaId,
                                     ((L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa_t*)data)->ospfLsdbType,
                                     ((L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa_t*)data)->ospfLsdbLsid,
                                     ((L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa_t*)data)->ospfLsdbRouterId);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfLsdbOverflow:
    (void)snmp_ospfLsdbOverflowTrapSend(((L7_SNMP_TRAP_ROUTING_OspfLsdbOverflow_t*)data)->ospfRouterId,
                                        ((L7_SNMP_TRAP_ROUTING_OspfLsdbOverflow_t*)data)->ospfExtLsdbLimit);
    break;

  case L7_SNMP_TRAP_ROUTING_OspfLsdbApproachingOverflow:
    (void)snmp_ospfLsdbApproachingOverflowTrapSend(((L7_SNMP_TRAP_ROUTING_OspfLsdbApproachingOverflow_t*)data)->ospfRouterId,
                                                   ((L7_SNMP_TRAP_ROUTING_OspfLsdbApproachingOverflow_t*)data)->ospfExtLsdbLimit);
    break;
#endif
#ifdef L7_VRRP_PACKAGE
  case L7_SNMP_TRAP_ROUTING_VrrpTrapNewMaster:
    (void)snmp_vrrpTrapNewMasterTrapSend(((L7_SNMP_TRAP_ROUTING_VrrpTrapNewMaster_t*)data)->vrrpOperMasterIpAddr);
    break;

  case L7_SNMP_TRAP_ROUTING_VrrpTrapAuthFailure:
    (void)snmp_vrrpTrapAuthFailureTrapSend(((L7_SNMP_TRAP_ROUTING_VrrpTrapAuthFailure_t*)data)->vrrpTrapPacketSrc,
                                           ((L7_SNMP_TRAP_ROUTING_VrrpTrapAuthFailure_t*)data)->vrrpTrapAuthErrorType);
    break;
#endif
  default:
    /* unknown trap */
    break;
  }
}

/* Begin Function Declarations: snmp_trap_api_l3.h */
#ifdef L7_OSPF_PACKAGE
/*********************************************************************
*
* @purpose  Signifies that there has been a change in the state of a 
*           non-virtual OSPF interface.
*
* @param    ospfRouterId      The originator of the trap
*           ospfIfIpAddress
*           ospfAddressLessIf
*           ospfIfState       New state
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
L7_RC_t SnmpOspfIfStateChangeTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfIfIpAddress, L7_uint32 ospfAddressLessIf, L7_uint32 ospfIfState)
{
  L7_SNMP_TRAP_ROUTING_OspfIfStateChange_t *trap;
  L7_uint32 ospfAddressLessIf_ext;

  if (usmDbExtIfNumFromIntIfNum(ospfAddressLessIf, &ospfAddressLessIf_ext) != L7_SUCCESS)
    return L7_FAILURE;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfIfStateChange_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfIfStateChange_t));

  trap->ospfAddressLessIf = ospfAddressLessIf_ext;
  trap->ospfIfIpAddress = ospfIfIpAddress;
  trap->ospfIfState = ospfIfState;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfIfStateChange, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies a change in the state of an OSPF virtual interface.
*
* @param  ospfRouterid        The ip address of the router originating the trap.
*         ospfVirtIfAreaId    The area ID of the Virtual Interface to which
*                             this RTO is attached.
*         ospfVirtIfNeighbor  The Neighbor Router ID to which this interface
*                             is connected.
*         ospfVirtIfState     The Virtual Interface Hello protocol State machine state.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the interface state regresses or 
*         progresses to a terminal state. 
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfVirtIfStateChangeTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfVirtIfAreaId, L7_uint32 ospfVirtIfNeighbor, L7_uint32 ospfVirtIfState)
{
  L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange_t *trap;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange_t));

  if ( SnmpStatusGet() != L7_ENABLE )
    return L7_FAILURE;

  trap->ospfRouterId = ospfRouterId;
  trap->ospfVirtIfAreaId = ospfVirtIfAreaId;
  trap->ospfVirtIfNeighbor = ospfVirtIfNeighbor;
  trap->ospfVirtIfState = ospfVirtIfState;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfVirtIfStateChange, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies a change in the state of a non-virtual OSPF neighbor.
*
* @param    ospfRouterId    The ip address of the router originating the trap.
*           ospfNbrIpAddr   The IP address this neighbor is using as this 
*                           IP Source Address.
*           ospfNbrAddressLessIndex  On an interface having and IP Address, "0".  
*                                    On an interface without an address, the 
*                                    corresponding value of ifIndex for this
*                                    interface in the Internet Standard MIB 
*                                    ifTable.  This index is usually used in 
*                                    an unnumbered interface as a reference to
*                                    the local router's control application 
*                                    interface table.
*           ospfNbrRtrId     A 32-bit integer uniquely indentifying the neighboring
*                            router in the AS (Autonomous System).
*           ospfNbrState     The new NBR state.  NBR states are defined in the 
*                            OSPF MIB.(pg. 126)
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
L7_RC_t SnmpOspfNbrStateChangeTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfNbrIpAddr, L7_uint32 ospfNbrAddressLessIndex, L7_uint32 ospfNbrRtrId, L7_uint32 ospfNbrState)
{
  L7_SNMP_TRAP_ROUTING_OspfNbrStateChange_t *trap = L7_NULLPTR;
  L7_uint32 ospfNbrAddressLessIndex_ext;

  if (usmDbExtIfNumFromIntIfNum(ospfNbrAddressLessIndex, &ospfNbrAddressLessIndex_ext) != L7_SUCCESS)
    return L7_FAILURE;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfNbrStateChange_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfNbrStateChange_t));

  trap->ospfNbrAddressLessIndex = ospfNbrAddressLessIndex_ext;
  trap->ospfNbrIpAddr = ospfNbrIpAddr;
  trap->ospfNbrRtrId = ospfNbrRtrId;
  trap->ospfNbrState = ospfNbrState;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfNbrStateChange, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies that there  has been a change in the state of a 
*           virtual OSPF neighbor.
*
* @param    ospfRouterId      The originator of the trap
*           ospfVirtNbrArea
*           ospfVirtNbrRtrId
*           ospfVirtNbrState  The new state
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
L7_RC_t SnmpOspfVirtNbrStateChangeTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfVirtNbrArea, L7_uint32 ospfVirtNbrRtrId, L7_uint32 ospfVirtNbrState)
{
  L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange_t));

  trap->ospfRouterId = ospfRouterId;
  trap->ospfVirtNbrArea = ospfVirtNbrArea;
  trap->ospfVirtNbrRtrId = ospfVirtNbrRtrId;
  trap->ospfVirtNbrState = ospfVirtNbrState;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfVirtNbrStateChange, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a non-virtual 
*           interface from a router whose configuration parameters  conflict  
*           with this router's configuration parameters.
*
* @param    ospfRouterId          The originator of the trap.
*           ospfIfIpAddress
*           ospfAddressLessIf
*           ospfPacketSrc         The source IP Address
*           ospfConfigErrorType   Type of error
*           ospfPacketType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming. 
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfIfConfigErrorTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfIfIpAddress, L7_uint32 ospfAddressLessIf, L7_uint32 ospfPacketSrc, L7_uint32 ospfConfigErrorType, L7_uint32 ospfPacketType)
{
  L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t *trap = L7_NULLPTR;
  L7_uint32 ospfAddressLessIf_ext;

  if (usmDbExtIfNumFromIntIfNum(ospfAddressLessIf, &ospfAddressLessIf_ext) != L7_SUCCESS)
    return L7_FAILURE;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfIfConfigError_t));

  trap->ospfAddressLessIf = ospfAddressLessIf_ext;
  trap->ospfConfigErrorType = ospfConfigErrorType;
  trap->ospfIfIpAddress = ospfIfIpAddress;
  trap->ospfPacketSrc = ospfPacketSrc;
  trap->ospfPacketType = ospfPacketType;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfIfConfigError, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a virtual 
*           interface from a router whose configuration parameters  conflict  
*           with this router's configuration parameters.
*
* @param    ospfRouterId        The originator of the Trap
*           ospfVirtIfAreaId
*           ospfVirtIfNeighbor
*           ospfConfigErrorType Type of error
*           ospfPacketType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming. 
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfVirtIfConfigErrorTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfVirtIfAreaId, L7_uint32 ospfVirtIfNeighbor, L7_uint32 ospfConfigErrorType, L7_uint32 ospfPacketType)
{
  L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError_t));

  trap->ospfConfigErrorType = ospfConfigErrorType;
  trap->ospfPacketType = ospfPacketType;
  trap->ospfRouterId = ospfRouterId;
  trap->ospfVirtIfAreaId = ospfVirtIfAreaId;
  trap->ospfVirtIfNeighbor = ospfVirtIfNeighbor;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfVirtIfConfigError, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a non-virtual
*          interface from a router whose authentication key or 
*          authentication type conflicts with this router's 
*          authentication key or authentication type.
*
* @param   ospfRouterId         The originator of the trap.
*          ospfIfIpAddress
*          ospfAddressLessIf
*          ospfPacketSrc        The source IP Address
*          ospfConfigErrorType  authType mismatch or auth failure
*          ospfPacketType 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfIfAuthFailureTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfIfIpAddress, L7_uint32 ospfAddressLessIf, L7_uint32 ospfPacketSrc, L7_uint32 ospfConfigErrorType, L7_uint32 ospfPacketType)
{
  L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t *trap = L7_NULLPTR;
  L7_uint32 ospfAddressLessIf_ext;

  if (usmDbExtIfNumFromIntIfNum(ospfAddressLessIf, &ospfAddressLessIf_ext) != L7_SUCCESS)
    return L7_FAILURE;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure_t));

  trap->ospfAddressLessIf = ospfAddressLessIf_ext;
  trap->ospfConfigErrorType = ospfConfigErrorType;
  trap->ospfIfIpAddress = ospfIfIpAddress;
  trap->ospfPacketSrc = ospfPacketSrc;
  trap->ospfPacketType = ospfPacketType;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfIfAuthFailure, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a virtual
*          interface from a router whose authentication key or 
*          authentication type conflicts with this router's 
*          authentication key or authentication type.
*
* @param   ospfRouterId         The originator of the trap.
*          ospfVirtIfAreaId
*          ospfVirtIfNeighbor
*          ospfConfigErrorType  Auth type mismatch or auth failure
*          ospfPacketType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfVirtIfAuthFailureTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfVirtIfAreaId, L7_uint32 ospfVirtIfNeighbor, L7_uint32 ospfConfigErrorType, L7_uint32 ospfPacketType)
{
  L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure_t));

  trap->ospfConfigErrorType = ospfConfigErrorType;
  trap->ospfPacketType = ospfPacketType;
  trap->ospfRouterId = ospfRouterId;
  trap->ospfVirtIfAreaId = ospfVirtIfAreaId;
  trap->ospfVirtIfNeighbor = ospfVirtIfNeighbor;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfVirtIfAuthFailure, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a 
*           non-virtual interface that cannot be parsed.
*
* @param    ospfRouterId        The originator of the Trap
*           ospfIfIpAddress
*           ospfAddressLessIf
*           ospfPacketSrc       The source ip address
*           ospfPacketType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfIfRxBadPacketTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfIfIpAddress, L7_uint32 ospfAddressLessIf, L7_uint32 ospfPacketSrc, L7_uint32 ospfPacketType)
{
  L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket_t *trap = L7_NULLPTR;
  L7_uint32 ospfAddressLessIf_ext;

  if (usmDbExtIfNumFromIntIfNum(ospfAddressLessIf, &ospfAddressLessIf_ext) != L7_SUCCESS)
    return L7_FAILURE;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket_t));

  trap->ospfAddressLessIf = ospfAddressLessIf_ext;
  trap->ospfIfIpAddress = ospfIfIpAddress;
  trap->ospfPacketSrc = ospfPacketSrc;
  trap->ospfPacketType = ospfPacketType;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfIfRxBadPacket, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a 
*           virtual interface that cannot be parsed.
*
* @param    ospfRouterId        The originator of the trap
*           ospfVirtIfAreaId
*           ospfVirtIfNeighbor
*           ospfPacketType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfVirtIfRxBadPacketTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfVirtIfAreaId, L7_uint32 ospfVirtIfNeighbor, L7_uint32 ospfPacketType)
{
  L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket_t));

  trap->ospfPacketType = ospfPacketType;
  trap->ospfRouterId = ospfRouterId;
  trap->ospfVirtIfAreaId = ospfVirtIfAreaId;
  trap->ospfVirtIfNeighbor = ospfVirtIfNeighbor;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfVirtIfRxBadPacket, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a 
*           non-virtual interface.
*
* @param    ospfRouterId        The originator of the trap
*           ospfIfIpAddress
*           ospfAddressLessIf
*           ospfNbrRtrId        Destination
*           ospfPacketType
*           ospfLsdbType
*           ospfLsdbLsid
*           ospfLsdbRouterId
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
L7_RC_t SnmpOspfTxRetransmitTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfIfIpAddress, L7_uint32 ospfAddressLessIf, L7_uint32 ospfNbrRtrId, L7_uint32 ospfPacketType, L7_uint32 ospfLsdbType, L7_uint32 ospfLsdbLsid, L7_uint32 ospfLsdbRouterId)
{
  L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t *trap = L7_NULLPTR;
  L7_uint32 ospfAddressLessIf_ext;

  if (usmDbExtIfNumFromIntIfNum(ospfAddressLessIf, &ospfAddressLessIf_ext) != L7_SUCCESS)
    return L7_FAILURE;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfTxRetransmit_t));

  trap->ospfAddressLessIf = ospfAddressLessIf_ext;
  trap->ospfIfIpAddress = ospfIfIpAddress;
  trap->ospfLsdbLsid = ospfLsdbLsid;
  trap->ospfLsdbRouterId = ospfLsdbRouterId;
  trap->ospfLsdbType = ospfLsdbType;
  trap->ospfNbrRtrId = ospfNbrRtrId;
  trap->ospfPacketType = ospfPacketType;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfTxRetransmit, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a 
*           virtual interface.
*
* @param    ospfRouterId        The originator of the trap
*           ospfVirtIfAreaId
*           ospfVirtIfNeighbor
*           ospfPacketType
*           ospfLsdbType
*           ospfLsdbLsid
*           ospfLsdbRouterId
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
L7_RC_t SnmpOspfVirtTxRetransmitTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfVirtIfAreaId, L7_uint32 ospfVirtIfNeighbor, L7_uint32 ospfPacketType, L7_uint32 ospfLsdbType, L7_uint32 ospfLsdbLsid, L7_uint32 ospfLsdbRouterId)
{
  L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit_t));

  trap->ospfLsdbLsid = ospfLsdbLsid;
  trap->ospfLsdbRouterId = ospfLsdbRouterId;
  trap->ospfLsdbType = ospfLsdbType;
  trap->ospfPacketType = ospfPacketType;
  trap->ospfRouterId = ospfRouterId;
  trap->ospfVirtIfAreaId = ospfVirtIfAreaId;
  trap->ospfVirtIfNeighbor = ospfVirtIfNeighbor;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfVirtTxRetransmit, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies that a  new LSA  has  been originated by this router.
*
* @param    ospfRouterId        The originator of the trap.
*           ospfLsdbAreaId      0.0.0.0 for AS Externals
*           ospfLsdbType
*           ospfLsdbLsid
*           ospfLsdbRouterId
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
L7_RC_t SnmpOspfOriginateLsaTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfLsdbAreaId, L7_uint32 ospfLsdbType, L7_uint32 ospfLsdbLsid, L7_uint32 ospfLsdbRouterId)
{
  L7_SNMP_TRAP_ROUTING_OspfOriginateLsa_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfOriginateLsa_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfOriginateLsa_t));

  trap->ospfLsdbAreaId = ospfLsdbAreaId;
  trap->ospfLsdbLsid = ospfLsdbLsid;
  trap->ospfLsdbRouterId = ospfLsdbRouterId;
  trap->ospfLsdbType = ospfLsdbType;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfOriginateLsa, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies that one of the LSA in the router's link-state 
*           database has aged to MaxAge.
*
* @param    ospfRouterId         The originator of the trap
*           ospfLsdbAreaId       0.0.0.0 for AS Externals
*           ospfLsdbType
*           ospfLsdbLsid
*           ospfLsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfMaxAgeLsaTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfLsdbAreaId, L7_uint32 ospfLsdbType, L7_uint32 ospfLsdbLsid, L7_uint32 ospfLsdbRouterId)
{
  L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa_t));

  trap->ospfLsdbAreaId = ospfLsdbAreaId;
  trap->ospfLsdbLsid = ospfLsdbLsid;
  trap->ospfLsdbRouterId = ospfLsdbRouterId;
  trap->ospfLsdbType = ospfLsdbType;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfMaxAgeLsa, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies that the number of LSAs in the router's link-state 
*           data-base has exceeded ospfExtLsdbLimit.
*
* @param    ospfRouterId      The originator of the trap
*           ospfExtLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfLsdbOverflowTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfExtLsdbLimit)
{
  L7_SNMP_TRAP_ROUTING_OspfLsdbOverflow_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfLsdbOverflow_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfLsdbOverflow_t));

  trap->ospfExtLsdbLimit = ospfExtLsdbLimit;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfLsdbOverflow, (void*)trap, &SnmpTrapSendCallback_routing);
}

/*********************************************************************
*
* @purpose  Signifies that  the  number of LSAs in the router's link-
*           state database has exceeded ninety  percent  of      
*           ospfExtLsdbLimit.
*
* @param    ospfRouterId      The originator of the trap
*           ospfExtLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t SnmpOspfLsdbApproachingOverflowTrapSend(L7_uint32 ospfRouterId, L7_uint32 ospfExtLsdbLimit)
{
  L7_SNMP_TRAP_ROUTING_OspfLsdbApproachingOverflow_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_OspfLsdbApproachingOverflow_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_OspfLsdbApproachingOverflow_t));

  trap->ospfExtLsdbLimit = ospfExtLsdbLimit;
  trap->ospfRouterId = ospfRouterId;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_OspfLsdbApproachingOverflow, (void*)trap, &SnmpTrapSendCallback_routing);
}
#endif
#ifdef L7_VRRP_PACKAGE
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
L7_RC_t SnmpVrrpTrapNewMasterTrapSend(L7_uint32 vrrpOperMasterIpAddr)
{
  L7_SNMP_TRAP_ROUTING_VrrpTrapNewMaster_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_VrrpTrapNewMaster_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_VrrpTrapNewMaster_t));

  trap->vrrpOperMasterIpAddr = vrrpOperMasterIpAddr;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_VrrpTrapNewMaster, (void*)trap, &SnmpTrapSendCallback_routing);
}

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
L7_RC_t SnmpVrrpTrapAuthFailureTrapSend(L7_uint32 vrrpTrapPacketSrc, L7_int32 vrrpTrapAuthErrorType)
{
  L7_SNMP_TRAP_ROUTING_VrrpTrapAuthFailure_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_ROUTING_VrrpTrapAuthFailure_t));
  if (trap == L7_NULLPTR)
    return L7_ERROR;

  memset(trap, 0, sizeof(L7_SNMP_TRAP_ROUTING_VrrpTrapAuthFailure_t));

  trap->vrrpTrapAuthErrorType = vrrpTrapAuthErrorType;
  trap->vrrpTrapPacketSrc = vrrpTrapPacketSrc;

  return snmpTrapSend(L7_SNMP_TRAP_ROUTING_VrrpTrapAuthFailure, (void*)trap, &SnmpTrapSendCallback_routing);
}
#endif
void SnmpTestTraps_routing()
{
  L7_RC_t rc;
  rc = L7_FAILURE;
#ifdef L7_OSPF_PACKAGE
  rc = SnmpOspfIfStateChangeTrapSend(1, 2, 3, 4);
  sysapiPrintf("SnmpOspfIfStateChangeTrapSend -> %d\n", rc);

  rc = SnmpOspfVirtIfStateChangeTrapSend(1, 2, 3, 4);
  sysapiPrintf("SnmpOspfVirtIfStateChangeTrapSend -> %d\n", rc);

  rc = SnmpOspfNbrStateChangeTrapSend(1, 2, 3, 4, 5);
  sysapiPrintf("SnmpOspfNbrStateChangeTrapSend -> %d\n", rc);

  rc = SnmpOspfVirtNbrStateChangeTrapSend(1, 2, 3, 4);
  sysapiPrintf("SnmpOspfVirtNbrStateChangeTrapSend -> %d\n", rc);

  rc = SnmpOspfIfConfigErrorTrapSend(1, 2, 3, 4, 5, 6);
  sysapiPrintf("SnmpOspfIfConfigErrorTrapSend -> %d\n", rc);

  rc = SnmpOspfVirtIfConfigErrorTrapSend(1, 2, 3, 4, 5);
  sysapiPrintf("SnmpOspfVirtIfConfigErrorTrapSend -> %d\n", rc);

  rc = SnmpOspfIfAuthFailureTrapSend(1, 2, 3, 4, 5, 6);
  sysapiPrintf("SnmpOspfIfAuthFailureTrapSend -> %d\n", rc);

  rc = SnmpOspfVirtIfAuthFailureTrapSend(1, 2, 3, 4, 5);
  sysapiPrintf("SnmpOspfVirtIfAuthFailureTrapSend -> %d\n", rc);

  rc = SnmpOspfIfRxBadPacketTrapSend(1, 2, 3, 4, 5);
  sysapiPrintf("SnmpOspfIfRxBadPacketTrapSend -> %d\n", rc);

  rc = SnmpOspfVirtIfRxBadPacketTrapSend(1, 2, 3, 4);
  sysapiPrintf("SnmpOspfVirtIfRxBadPacketTrapSend -> %d\n", rc);

  rc = SnmpOspfTxRetransmitTrapSend(1, 2, 3, 4, 5, 6, 7, 8);
  sysapiPrintf("SnmpOspfTxRetransmitTrapSend -> %d\n", rc);

  rc = SnmpOspfVirtTxRetransmitTrapSend(1, 2, 3, 4, 5, 6, 7);
  sysapiPrintf("SnmpOspfVirtTxRetransmitTrapSend -> %d\n", rc);

  rc = SnmpOspfOriginateLsaTrapSend(1, 2, 3, 4, 5);
  sysapiPrintf("SnmpOspfOriginateLsaTrapSend -> %d\n", rc);

  rc = SnmpOspfMaxAgeLsaTrapSend(1, 2, 3, 4, 5);
  sysapiPrintf("SnmpOspfMaxAgeLsaTrapSend -> %d\n", rc);

  rc = SnmpOspfLsdbOverflowTrapSend(1, 2);
  sysapiPrintf("SnmpOspfLsdbOverflowTrapSend -> %d\n", rc);

  rc = SnmpOspfLsdbApproachingOverflowTrapSend(1, 2);
  sysapiPrintf("SnmpOspfLsdbApproachingOverflowTrapSend -> %d\n", rc);
#endif
#ifdef L7_VRRP_PACKAGE

  rc = SnmpVrrpTrapNewMasterTrapSend(1);
  sysapiPrintf("SnmpVrrpTrapNewMasterTrapSend -> %d\n", rc);

  rc = SnmpVrrpTrapAuthFailureTrapSend(1, 2);
  sysapiPrintf("SnmpVrrpTrapAuthFailureTrapSend -> %d\n", rc);
#endif 
  return;
}
/* End Function Declarations */

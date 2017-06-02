/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap_ospf.c
*
* Purpose: OSPF trap functions
*
* Created by: Colin Verne 07/03/2001
*
* Component: SNMP
*
*********************************************************************/
/*********************************************************************
                         
**********************************************************************
*********************************************************************/
#include "sr_conf.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif /* HAVE_MEMORY_H */
#include "sr_snmp.h"
#include "diag.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "getvar.h"
#include "sr_ntfy.h"

#include "l7_common.h"
#include "privatetrap_ospf.h"
        
        
/* Begin Function Declarations: privatetrap_ospf.h */
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
L7_RC_t snmp_ospfIfStateChangeTrapSend(L7_uint32 ospfRouterId, 
                                       L7_uint32 ospfIfIpAddress,
                                       L7_uint32 ospfAddressLessIf, 
                                       L7_uint32 ospfIfState)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.16");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfIfIpAddress = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.1");
  OID *oid_ospfAddressLessIf = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.2");
  OID *oid_ospfIfState = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.12");
  OID *oid_instance = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfIfIpAddress = MakeOctetString((char*) &ospfIfIpAddress, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfIfIpAddress = NULL;
  VarBind *var_ospfAddressLessIf = NULL;
  VarBind *var_ospfIfState = NULL;

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfIfIpAddress)[0];
  buffer[index++] = ((char*)&ospfIfIpAddress)[1];
  buffer[index++] = ((char*)&ospfIfIpAddress)[2];
  buffer[index++] = ((char*)&ospfIfIpAddress)[3];
  buffer[index++] = ospfAddressLessIf;
  oid_instance = MakeOID(buffer, index);

  var_ospfIfIpAddress = MakeVarBindWithValue(oid_ospfIfIpAddress, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfIfIpAddress);
  var_ospfAddressLessIf = MakeVarBindWithValue(oid_ospfAddressLessIf, oid_instance, INTEGER_TYPE, &ospfAddressLessIf);
  var_ospfIfState = MakeVarBindWithValue(oid_ospfIfState, oid_instance, INTEGER_TYPE, &ospfIfState);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfIfIpAddress);
  FreeOID(oid_ospfAddressLessIf);
  FreeOID(oid_ospfIfState);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfIfIpAddress == NULL || var_ospfAddressLessIf == NULL || var_ospfIfState == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfIfIpAddress);
    FreeVarBind(var_ospfAddressLessIf);
    FreeVarBind(var_ospfIfState);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfIfIpAddress;
  var_ospfIfIpAddress->next_var = var_ospfAddressLessIf;
  var_ospfAddressLessIf->next_var = var_ospfIfState;
  var_ospfIfState->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfVirtIfStateChangeTrapSend(L7_uint32 ospfRouterId, 
                                           L7_uint32 ospfVirtIfAreaId,
                                           L7_uint32 ospfVirtIfNeighbor, 
                                           L7_uint32 ospfVirtIfState)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.1");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfVirtIfAreaId = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.1");
  OID *oid_ospfVirtIfNeighbor = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.2");
  OID *oid_ospfVirtIfState = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.7");
  OID *oid_instance = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfVirtIfAreaId = MakeOctetString((char*) &ospfVirtIfAreaId, 4);
  OctetString *os_ospfVirtIfNeighbor = MakeOctetString((char*) &ospfVirtIfNeighbor, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfVirtIfAreaId = NULL;
  VarBind *var_ospfVirtIfNeighbor = NULL;
  VarBind *var_ospfVirtIfState = NULL;

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[0];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[1];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[2];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[3];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[0];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[1];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[2];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[3];
  oid_instance = MakeOID(buffer, index);

  var_ospfVirtIfAreaId = MakeVarBindWithValue(oid_ospfVirtIfAreaId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtIfAreaId);
  var_ospfVirtIfNeighbor = MakeVarBindWithValue(oid_ospfVirtIfNeighbor, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtIfNeighbor);
  var_ospfVirtIfState = MakeVarBindWithValue(oid_ospfVirtIfState, oid_instance, INTEGER_TYPE, &ospfVirtIfState);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfVirtIfAreaId);
  FreeOID(oid_ospfVirtIfNeighbor);
  FreeOID(oid_ospfVirtIfState);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfVirtIfAreaId == NULL || var_ospfVirtIfNeighbor == NULL || var_ospfVirtIfState == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfVirtIfAreaId);
    FreeVarBind(var_ospfVirtIfNeighbor);
    FreeVarBind(var_ospfVirtIfState);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfVirtIfAreaId;
  var_ospfVirtIfAreaId->next_var = var_ospfVirtIfNeighbor;
  var_ospfVirtIfNeighbor->next_var = var_ospfVirtIfState;
  var_ospfVirtIfState->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfNbrStateChangeTrapSend(L7_uint32 ospfRouterId, 
                                        L7_uint32 ospfNbrIpAddr,
                                        L7_uint32 ospfNbrAddressLessIndex,
                                        L7_uint32 ospfNbrRtrId, 
                                        L7_uint32 ospfNbrState)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.2");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfNbrIpAddr = MakeOIDFromDot("1.3.6.1.2.1.14.10.1.1");
  OID *oid_ospfNbrAddressLessIndex = MakeOIDFromDot("1.3.6.1.2.1.14.10.1.2");
  OID *oid_ospfNbrRtrId = MakeOIDFromDot("1.3.6.1.2.1.14.10.1.3");
  OID *oid_ospfNbrState = MakeOIDFromDot("1.3.6.1.2.1.14.10.1.6");
  OID *oid_instance = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfNbrIpAddr = MakeOctetString((char*) &ospfNbrIpAddr, 4);
  OctetString *os_ospfNbrRtrId = MakeOctetString((char*) &ospfNbrRtrId, 4);
  
  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfNbrIpAddr = NULL;
  VarBind *var_ospfNbrAddressLessIndex = NULL;
  VarBind *var_ospfNbrRtrId = NULL;
  VarBind *var_ospfNbrState = NULL;
  
  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfNbrIpAddr)[0];
  buffer[index++] = ((char*)&ospfNbrIpAddr)[1];
  buffer[index++] = ((char*)&ospfNbrIpAddr)[2];
  buffer[index++] = ((char*)&ospfNbrIpAddr)[3];
  buffer[index++] = ospfNbrAddressLessIndex;
  oid_instance = MakeOID(buffer, index);

  var_ospfNbrIpAddr = MakeVarBindWithValue(oid_ospfNbrIpAddr, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfNbrIpAddr);
  var_ospfNbrAddressLessIndex = MakeVarBindWithValue(oid_ospfNbrAddressLessIndex, oid_instance, INTEGER_TYPE, &ospfNbrAddressLessIndex);
  var_ospfNbrRtrId = MakeVarBindWithValue(oid_ospfNbrRtrId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfNbrRtrId);
  var_ospfNbrState = MakeVarBindWithValue(oid_ospfNbrState, oid_instance, INTEGER_TYPE, &ospfNbrState);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfNbrIpAddr);
  FreeOID(oid_ospfNbrAddressLessIndex);
  FreeOID(oid_ospfNbrRtrId);
  FreeOID(oid_ospfNbrState);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfNbrIpAddr == NULL || var_ospfNbrAddressLessIndex == NULL || var_ospfNbrRtrId == NULL || var_ospfNbrState == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfNbrIpAddr);
    FreeVarBind(var_ospfNbrAddressLessIndex);
    FreeVarBind(var_ospfNbrRtrId);
    FreeVarBind(var_ospfNbrState);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfNbrIpAddr;
  var_ospfNbrIpAddr->next_var = var_ospfNbrAddressLessIndex;
  var_ospfNbrAddressLessIndex->next_var = var_ospfNbrRtrId;
  var_ospfNbrRtrId->next_var = var_ospfNbrState;
  var_ospfNbrState->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfVirtNbrStateChangeTrapSend(L7_uint32 ospfRouterId, 
                                            L7_uint32 ospfVirtNbrArea,
                                            L7_uint32 ospfVirtNbrRtrId, 
                                            L7_uint32 ospfVirtNbrState)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.3");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfVirtNbrArea = MakeOIDFromDot("1.3.6.1.2.1.14.11.1.1");
  OID *oid_ospfVirtNbrRtrId = MakeOIDFromDot("1.3.6.1.2.1.14.11.1.2");
  OID *oid_ospfVirtNbrState = MakeOIDFromDot("1.3.6.1.2.1.14.11.1.5");
  OID *oid_instance = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfVirtNbrArea = MakeOctetString((char*) &ospfVirtNbrArea, 4);
  OctetString *os_ospfVirtNbrRtrId = MakeOctetString((char*) &ospfVirtNbrRtrId, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfVirtNbrArea = NULL;
  VarBind *var_ospfVirtNbrRtrId = NULL;
  VarBind *var_ospfVirtNbrState = MakeVarBindWithValue(oid_ospfVirtNbrState, oid_instance, INTEGER_TYPE, &ospfVirtNbrState);
  
  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfVirtNbrArea)[0];
  buffer[index++] = ((char*)&ospfVirtNbrArea)[1];
  buffer[index++] = ((char*)&ospfVirtNbrArea)[2];
  buffer[index++] = ((char*)&ospfVirtNbrArea)[3];
  buffer[index++] = ((char*)&ospfVirtNbrRtrId)[0];
  buffer[index++] = ((char*)&ospfVirtNbrRtrId)[1];
  buffer[index++] = ((char*)&ospfVirtNbrRtrId)[2];
  buffer[index++] = ((char*)&ospfVirtNbrRtrId)[3];
  oid_instance = MakeOID(buffer, index);

  var_ospfVirtNbrArea = MakeVarBindWithValue(oid_ospfVirtNbrArea, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtNbrArea);
  var_ospfVirtNbrRtrId = MakeVarBindWithValue(oid_ospfVirtNbrRtrId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtNbrRtrId);
  var_ospfVirtNbrState = MakeVarBindWithValue(oid_ospfVirtNbrState, oid_instance, INTEGER_TYPE, &ospfVirtNbrState);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfVirtNbrArea);
  FreeOID(oid_ospfVirtNbrRtrId);
  FreeOID(oid_ospfVirtNbrState);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfVirtNbrArea == NULL || var_ospfVirtNbrRtrId == NULL || var_ospfVirtNbrState == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfVirtNbrArea);
    FreeVarBind(var_ospfVirtNbrRtrId);
    FreeVarBind(var_ospfVirtNbrState);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfVirtNbrArea;
  var_ospfVirtNbrArea->next_var = var_ospfVirtNbrRtrId;
  var_ospfVirtNbrRtrId->next_var = var_ospfVirtNbrState;
  var_ospfVirtNbrState->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfIfConfigErrorTrapSend(L7_uint32 ospfRouterId, 
                                       L7_uint32 ospfIfIpAddress,
                                       L7_uint32 ospfAddressLessIf, 
                                       L7_uint32 ospfPacketSrc,
                                       L7_uint32 ospfConfigErrorType, 
                                       L7_uint32 ospfPacketType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.4");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfIfIpAddress = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.1");
  OID *oid_ospfAddressLessIf = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.2");
  OID *oid_ospfPacketSrc = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.4.0");
  OID *oid_ospfConfigErrorType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.2.0");
  OID *oid_ospfPacketType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.3.0");
  OID *oid_instance = NULL;
  
  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfIfIpAddress = MakeOctetString((char*) &ospfIfIpAddress, 4);
  OctetString *os_ospfPacketSrc = MakeOctetString((char*) &ospfPacketSrc, 4);
  
  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfIfIpAddress = NULL;
  VarBind *var_ospfAddressLessIf = NULL;
  VarBind *var_ospfPacketSrc = MakeVarBindWithValue(oid_ospfPacketSrc, NULL, IP_ADDR_PRIM_TYPE, os_ospfPacketSrc);
  VarBind *var_ospfConfigErrorType = MakeVarBindWithValue(oid_ospfConfigErrorType, NULL, INTEGER_TYPE, &ospfConfigErrorType);
  VarBind *var_ospfPacketType = MakeVarBindWithValue(oid_ospfPacketType, NULL, INTEGER_TYPE, &ospfPacketType);

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfIfIpAddress)[0];
  buffer[index++] = ((char*)&ospfIfIpAddress)[1];
  buffer[index++] = ((char*)&ospfIfIpAddress)[2];
  buffer[index++] = ((char*)&ospfIfIpAddress)[3];
  buffer[index++] = ospfAddressLessIf;
  oid_instance = MakeOID(buffer, index);

  var_ospfIfIpAddress = MakeVarBindWithValue(oid_ospfIfIpAddress, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfIfIpAddress);
  var_ospfAddressLessIf = MakeVarBindWithValue(oid_ospfAddressLessIf, oid_instance, INTEGER_TYPE, &ospfAddressLessIf);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfIfIpAddress);
  FreeOID(oid_ospfAddressLessIf);
  FreeOID(oid_ospfPacketSrc);
  FreeOID(oid_ospfConfigErrorType);
  FreeOID(oid_ospfPacketType);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfIfIpAddress == NULL || var_ospfAddressLessIf == NULL || var_ospfPacketSrc == NULL || var_ospfConfigErrorType == NULL || var_ospfPacketType == NULL)
  {
    FreeOID(snmpTrapOID);

    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfIfIpAddress);
    FreeVarBind(var_ospfAddressLessIf);
    FreeVarBind(var_ospfPacketSrc);
    FreeVarBind(var_ospfConfigErrorType);
    FreeVarBind(var_ospfPacketType);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfIfIpAddress;
  var_ospfIfIpAddress->next_var = var_ospfAddressLessIf;
  var_ospfAddressLessIf->next_var = var_ospfPacketSrc;
  var_ospfPacketSrc->next_var = var_ospfConfigErrorType;
  var_ospfConfigErrorType->next_var = var_ospfPacketType;
  var_ospfPacketType->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfVirtIfConfigErrorTrapSend(L7_uint32 ospfRouterId, 
                                           L7_uint32 ospfVirtIfAreaId,
                                           L7_uint32 ospfVirtIfNeighbor,
                                           L7_uint32 ospfConfigErrorType,
                                           L7_uint32 ospfPacketType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.5");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfVirtIfAreaId = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.1");
  OID *oid_ospfVirtIfNeighbor = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.2");
  OID *oid_ospfConfigErrorType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.2.0");
  OID *oid_ospfPacketType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.3.0");
  OID *oid_instance = NULL;
  
  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfVirtIfAreaId = MakeOctetString((char*) &ospfVirtIfAreaId, 4);
  OctetString *os_ospfVirtIfNeighbor = MakeOctetString((char*) &ospfVirtIfNeighbor, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfVirtIfAreaId = NULL;
  VarBind *var_ospfVirtIfNeighbor = NULL;
  VarBind *var_ospfConfigErrorType = MakeVarBindWithValue(oid_ospfConfigErrorType, NULL, INTEGER_TYPE, &ospfConfigErrorType);
  VarBind *var_ospfPacketType = MakeVarBindWithValue(oid_ospfPacketType, NULL, INTEGER_TYPE, &ospfPacketType);
  
  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[0];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[1];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[2];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[3];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[0];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[1];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[2];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[3];
  oid_instance = MakeOID(buffer, index);

  var_ospfVirtIfAreaId = MakeVarBindWithValue(oid_ospfVirtIfAreaId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtIfAreaId);
  var_ospfVirtIfNeighbor = MakeVarBindWithValue(oid_ospfVirtIfNeighbor, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtIfNeighbor);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfVirtIfAreaId);
  FreeOID(oid_ospfVirtIfNeighbor);
  FreeOID(oid_ospfConfigErrorType);
  FreeOID(oid_ospfPacketType);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfVirtIfAreaId == NULL || var_ospfVirtIfNeighbor == NULL || var_ospfConfigErrorType == NULL || var_ospfPacketType == NULL)
  {
    FreeOID(snmpTrapOID);

    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfVirtIfAreaId);
    FreeVarBind(var_ospfVirtIfNeighbor);
    FreeVarBind(var_ospfConfigErrorType);
    FreeVarBind(var_ospfPacketType);
    return L7_FAILURE;
  }
  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfVirtIfAreaId;
  var_ospfVirtIfAreaId->next_var = var_ospfVirtIfNeighbor;
  var_ospfVirtIfNeighbor->next_var = var_ospfConfigErrorType;
  var_ospfConfigErrorType->next_var = var_ospfPacketType;
  var_ospfPacketType->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfIfAuthFailureTrapSend(L7_uint32 ospfRouterId, 
                                       L7_uint32 ospfIfIpAddress,
                                       L7_uint32 ospfAddressLessIf, 
                                       L7_uint32 ospfPacketSrc,
                                       L7_uint32 ospfConfigErrorType, 
                                       L7_uint32 ospfPacketType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.6");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfIfIpAddress = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.1");
  OID *oid_ospfAddressLessIf = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.2");
  OID *oid_ospfPacketSrc = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.4.0");
  OID *oid_ospfConfigErrorType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.2.0");
  OID *oid_ospfPacketType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.3.0");
  OID *oid_instance = NULL;
  
  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfIfIpAddress = MakeOctetString((char*) &ospfIfIpAddress, 4);
  OctetString *os_ospfPacketSrc = MakeOctetString((char*) &ospfPacketSrc, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfIfIpAddress = NULL;
  VarBind *var_ospfAddressLessIf = NULL;
  VarBind *var_ospfPacketSrc = MakeVarBindWithValue(oid_ospfPacketSrc, NULL, IP_ADDR_PRIM_TYPE, os_ospfPacketSrc);
  VarBind *var_ospfConfigErrorType = MakeVarBindWithValue(oid_ospfConfigErrorType, NULL, INTEGER_TYPE, &ospfConfigErrorType);
  VarBind *var_ospfPacketType = MakeVarBindWithValue(oid_ospfPacketType, NULL, INTEGER_TYPE, &ospfPacketType);
  
  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfIfIpAddress)[0];
  buffer[index++] = ((char*)&ospfIfIpAddress)[1];
  buffer[index++] = ((char*)&ospfIfIpAddress)[2];
  buffer[index++] = ((char*)&ospfIfIpAddress)[3];
  buffer[index++] = ospfAddressLessIf;
  oid_instance = MakeOID(buffer, index);

  var_ospfIfIpAddress = MakeVarBindWithValue(oid_ospfIfIpAddress, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfIfIpAddress);
  var_ospfAddressLessIf = MakeVarBindWithValue(oid_ospfAddressLessIf, oid_instance, INTEGER_TYPE, &ospfAddressLessIf);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfIfIpAddress);
  FreeOID(oid_ospfAddressLessIf);
  FreeOID(oid_ospfPacketSrc);
  FreeOID(oid_ospfConfigErrorType);
  FreeOID(oid_ospfPacketType);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfIfIpAddress == NULL || var_ospfAddressLessIf == NULL || var_ospfPacketSrc == NULL || var_ospfConfigErrorType == NULL || var_ospfPacketType == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfIfIpAddress);
    FreeVarBind(var_ospfAddressLessIf);
    FreeVarBind(var_ospfPacketSrc);
    FreeVarBind(var_ospfConfigErrorType);
    FreeVarBind(var_ospfPacketType);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfIfIpAddress;
  var_ospfIfIpAddress->next_var = var_ospfAddressLessIf;
  var_ospfAddressLessIf->next_var = var_ospfPacketSrc;
  var_ospfPacketSrc->next_var = var_ospfConfigErrorType;
  var_ospfConfigErrorType->next_var = var_ospfPacketType;
  var_ospfPacketType->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfVirtIfAuthFailureTrapSend(L7_uint32 ospfRouterId, 
                                           L7_uint32 ospfVirtIfAreaId,
                                           L7_uint32 ospfVirtIfNeighbor,
                                           L7_uint32 ospfConfigErrorType,
                                           L7_uint32 ospfPacketType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.7");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfVirtIfAreaId = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.1");
  OID *oid_ospfVirtIfNeighbor = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.2");
  OID *oid_ospfConfigErrorType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.2.0");
  OID *oid_ospfPacketType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.3.0");
  OID *oid_instance = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfVirtIfAreaId = MakeOctetString((char*) &ospfVirtIfAreaId, 4);
  OctetString *os_ospfVirtIfNeighbor = MakeOctetString((char*) &ospfVirtIfNeighbor, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfVirtIfAreaId = NULL;
  VarBind *var_ospfVirtIfNeighbor = NULL;
  VarBind *var_ospfConfigErrorType = MakeVarBindWithValue(oid_ospfConfigErrorType, NULL, INTEGER_TYPE, &ospfConfigErrorType);
  VarBind *var_ospfPacketType = MakeVarBindWithValue(oid_ospfPacketType, NULL, INTEGER_TYPE, &ospfPacketType);

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[0];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[1];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[2];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[3];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[0];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[1];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[2];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[3];
  oid_instance = MakeOID(buffer, index);

  var_ospfVirtIfAreaId = MakeVarBindWithValue(oid_ospfVirtIfAreaId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtIfAreaId);
  var_ospfVirtIfNeighbor = MakeVarBindWithValue(oid_ospfVirtIfNeighbor, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtIfNeighbor);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfVirtIfAreaId);
  FreeOID(oid_ospfVirtIfNeighbor);
  FreeOID(oid_ospfConfigErrorType);
  FreeOID(oid_ospfPacketType);
  FreeOID(oid_instance);
  
  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfVirtIfAreaId == NULL || var_ospfVirtIfNeighbor == NULL || var_ospfConfigErrorType == NULL || var_ospfPacketType == NULL)
  {
    FreeOID(snmpTrapOID);

    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfVirtIfAreaId);
    FreeVarBind(var_ospfVirtIfNeighbor);
    FreeVarBind(var_ospfConfigErrorType);
    FreeVarBind(var_ospfPacketType);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfVirtIfAreaId;
  var_ospfVirtIfAreaId->next_var = var_ospfVirtIfNeighbor;
  var_ospfVirtIfNeighbor->next_var = var_ospfConfigErrorType;
  var_ospfConfigErrorType->next_var = var_ospfPacketType;
  var_ospfPacketType->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfIfRxBadPacketTrapSend(L7_uint32 ospfRouterId, 
                                       L7_uint32 ospfIfIpAddress,
                                       L7_uint32 ospfAddressLessIf, 
                                       L7_uint32 ospfPacketSrc,
                                       L7_uint32 ospfPacketType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.8");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfIfIpAddress = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.1");
  OID *oid_ospfAddressLessIf = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.2");
  OID *oid_ospfPacketSrc = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.4.0");
  OID *oid_ospfPacketType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.3.0");
  OID *oid_instance = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfIfIpAddress = MakeOctetString((char*) &ospfIfIpAddress, 4);
  OctetString *os_ospfPacketSrc = MakeOctetString((char*) &ospfPacketSrc, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfIfIpAddress = NULL;
  VarBind *var_ospfAddressLessIf = NULL;
  VarBind *var_ospfPacketSrc = MakeVarBindWithValue(oid_ospfPacketSrc, NULL, IP_ADDR_PRIM_TYPE, os_ospfPacketSrc);
  VarBind *var_ospfPacketType = MakeVarBindWithValue(oid_ospfPacketType, NULL, INTEGER_TYPE, &ospfPacketType);

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfIfIpAddress)[0];
  buffer[index++] = ((char*)&ospfIfIpAddress)[1];
  buffer[index++] = ((char*)&ospfIfIpAddress)[2];
  buffer[index++] = ((char*)&ospfIfIpAddress)[3];
  buffer[index++] = ospfAddressLessIf;
  oid_instance = MakeOID(buffer, index);
  
  var_ospfIfIpAddress = MakeVarBindWithValue(oid_ospfIfIpAddress, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfIfIpAddress);
  var_ospfAddressLessIf = MakeVarBindWithValue(oid_ospfAddressLessIf, oid_instance, INTEGER_TYPE, &ospfAddressLessIf);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfIfIpAddress);
  FreeOID(oid_ospfAddressLessIf);
  FreeOID(oid_ospfPacketSrc);
  FreeOID(oid_ospfPacketType);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfIfIpAddress == NULL || var_ospfAddressLessIf == NULL || var_ospfPacketSrc == NULL || var_ospfPacketType == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfIfIpAddress);
    FreeVarBind(var_ospfAddressLessIf);
    FreeVarBind(var_ospfPacketSrc);
    FreeVarBind(var_ospfPacketType);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfIfIpAddress;
  var_ospfIfIpAddress->next_var = var_ospfAddressLessIf;
  var_ospfAddressLessIf->next_var = var_ospfPacketSrc;
  var_ospfPacketSrc->next_var = var_ospfPacketType;
  var_ospfPacketType->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfVirtIfRxBadPacketTrapSend(L7_uint32 ospfRouterId, 
                                           L7_uint32 ospfVirtIfAreaId,
                                           L7_uint32 ospfVirtIfNeighbor, 
                                           L7_uint32 ospfPacketType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.9");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfVirtIfAreaId = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.1");
  OID *oid_ospfVirtIfNeighbor = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.2");
  OID *oid_ospfPacketType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.3.0");
  OID *oid_instance = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfVirtIfAreaId = MakeOctetString((char*) &ospfVirtIfAreaId, 4);
  OctetString *os_ospfVirtIfNeighbor = MakeOctetString((char*) &ospfVirtIfNeighbor, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfVirtIfAreaId = NULL;
  VarBind *var_ospfVirtIfNeighbor = NULL;
  VarBind *var_ospfPacketType = MakeVarBindWithValue(oid_ospfPacketType, NULL, INTEGER_TYPE, &ospfPacketType);

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[0];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[1];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[2];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[3];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[0];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[1];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[2];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[3];
  oid_instance = MakeOID(buffer, index);

  var_ospfVirtIfAreaId = MakeVarBindWithValue(oid_ospfVirtIfAreaId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtIfAreaId);
  var_ospfVirtIfNeighbor = MakeVarBindWithValue(oid_ospfVirtIfNeighbor, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfVirtIfNeighbor);
  
  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfVirtIfAreaId);
  FreeOID(oid_ospfVirtIfNeighbor);
  FreeOID(oid_ospfPacketType);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfVirtIfAreaId == NULL || var_ospfVirtIfNeighbor == NULL || var_ospfPacketType == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfVirtIfAreaId);
    FreeVarBind(var_ospfVirtIfNeighbor);
    FreeVarBind(var_ospfPacketType);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfVirtIfAreaId;
  var_ospfVirtIfAreaId->next_var = var_ospfVirtIfNeighbor;
  var_ospfVirtIfNeighbor->next_var = var_ospfPacketType;
  var_ospfPacketType->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfTxRetransmitTrapSend(L7_uint32 ospfRouterId, 
                                      L7_uint32 ospfIfIpAddress,
                                      L7_uint32 ospfAddressLessIf, 
                                      L7_uint32 ospfNbrRtrId,
                                      L7_uint32 ospfPacketType, 
                                      L7_uint32 ospfLsdbType,
                                      L7_uint32 ospfLsdbLsid, 
                                      L7_uint32 ospfLsdbRouterId)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.10");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfIfIpAddress = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.1");
  OID *oid_ospfAddressLessIf = MakeOIDFromDot("1.3.6.1.2.1.14.7.1.2");
  OID *oid_ospfNbrRtrId = MakeOIDFromDot("1.3.6.1.2.1.14.10.1.3");
  OID *oid_ospfPacketType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.3.0");
  OID *oid_ospfLsdbType = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.2");
  OID *oid_ospfLsdbLsid = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.3");
  OID *oid_ospfLsdbRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.4");

  OID *oid_instance1 = NULL;
  OID *oid_instance2 = NULL;
  
  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfIfIpAddress = MakeOctetString((char*) &ospfIfIpAddress, 4);
  OctetString *os_ospfNbrRtrId = MakeOctetString((char*) &ospfNbrRtrId, 4);
  OctetString *os_ospfLsdbLsid = MakeOctetString((char*) &ospfLsdbLsid, 4);
  OctetString *os_ospfLsdbRouterId = MakeOctetString((char*) &ospfLsdbRouterId, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfIfIpAddress = NULL;
  VarBind *var_ospfAddressLessIf = NULL;
  VarBind *var_ospfNbrRtrId = NULL;
  VarBind *var_ospfPacketType = MakeVarBindWithValue(oid_ospfPacketType, NULL, INTEGER_TYPE, &ospfPacketType);
  VarBind *var_ospfLsdbType = NULL;
  VarBind *var_ospfLsdbLsid = NULL;
  VarBind *var_ospfLsdbRouterId = NULL;

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfIfIpAddress)[0];
  buffer[index++] = ((char*)&ospfIfIpAddress)[1];
  buffer[index++] = ((char*)&ospfIfIpAddress)[2];
  buffer[index++] = ((char*)&ospfIfIpAddress)[3];
  buffer[index++] = ospfAddressLessIf;
  oid_instance1 = MakeOID(buffer, index);

  var_ospfIfIpAddress = MakeVarBindWithValue(oid_ospfIfIpAddress, oid_instance1, IP_ADDR_PRIM_TYPE, os_ospfIfIpAddress);
  var_ospfAddressLessIf = MakeVarBindWithValue(oid_ospfAddressLessIf, oid_instance1, INTEGER_TYPE, &ospfAddressLessIf);
  var_ospfNbrRtrId = MakeVarBindWithValue(oid_ospfNbrRtrId, oid_instance1, IP_ADDR_PRIM_TYPE, os_ospfNbrRtrId);

  /* create instance OID */
  index = 0;
  buffer[index++] = ospfLsdbType;
  buffer[index++] = ((char*)&ospfLsdbLsid)[0];
  buffer[index++] = ((char*)&ospfLsdbLsid)[1];
  buffer[index++] = ((char*)&ospfLsdbLsid)[2];
  buffer[index++] = ((char*)&ospfLsdbLsid)[3];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[0];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[1];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[2];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[3];
  oid_instance2 = MakeOID(buffer, index);

  var_ospfLsdbType = MakeVarBindWithValue(oid_ospfLsdbType, oid_instance2, INTEGER_TYPE, &ospfLsdbType);
  var_ospfLsdbLsid = MakeVarBindWithValue(oid_ospfLsdbLsid, oid_instance2, IP_ADDR_PRIM_TYPE, os_ospfLsdbLsid);
  var_ospfLsdbRouterId = MakeVarBindWithValue(oid_ospfLsdbRouterId, oid_instance2, IP_ADDR_PRIM_TYPE, os_ospfLsdbRouterId);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfIfIpAddress);
  FreeOID(oid_ospfAddressLessIf);
  FreeOID(oid_ospfNbrRtrId);
  FreeOID(oid_ospfPacketType);
  FreeOID(oid_ospfLsdbType);
  FreeOID(oid_ospfLsdbLsid);
  FreeOID(oid_ospfLsdbRouterId);
  FreeOID(oid_instance1);
  FreeOID(oid_instance2);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfIfIpAddress == NULL || var_ospfAddressLessIf == NULL || var_ospfNbrRtrId == NULL || var_ospfPacketType == NULL || var_ospfLsdbType == NULL || var_ospfLsdbLsid == NULL || var_ospfLsdbRouterId == NULL)
  {
    FreeOID(snmpTrapOID);

    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfIfIpAddress);
    FreeVarBind(var_ospfAddressLessIf);
    FreeVarBind(var_ospfNbrRtrId);
    FreeVarBind(var_ospfPacketType);
    FreeVarBind(var_ospfLsdbType);
    FreeVarBind(var_ospfLsdbLsid);
    FreeVarBind(var_ospfLsdbRouterId);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfIfIpAddress;
  var_ospfIfIpAddress->next_var = var_ospfAddressLessIf;
  var_ospfAddressLessIf->next_var = var_ospfNbrRtrId;
  var_ospfNbrRtrId->next_var = var_ospfPacketType;
  var_ospfPacketType->next_var = var_ospfLsdbType;
  var_ospfLsdbType->next_var = var_ospfLsdbLsid;
  var_ospfLsdbLsid->next_var = var_ospfLsdbRouterId;
  var_ospfLsdbRouterId->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfVirtTxRetransmitTrapSend(L7_uint32 ospfRouterId, 
                                          L7_uint32 ospfVirtIfAreaId,
                                          L7_uint32 ospfVirtIfNeighbor, 
                                          L7_uint32 ospfPacketType,
                                          L7_uint32 ospfLsdbType, 
                                          L7_uint32 ospfLsdbLsid,
                                          L7_uint32 ospfLsdbRouterId)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.11");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfVirtIfAreaId = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.1");
  OID *oid_ospfVirtIfNeighbor = MakeOIDFromDot("1.3.6.1.2.1.14.9.1.2");
  OID *oid_ospfPacketType = MakeOIDFromDot("1.3.6.1.2.1.14.16.1.3.0");
  OID *oid_ospfLsdbType = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.2");
  OID *oid_ospfLsdbLsid = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.3");
  OID *oid_ospfLsdbRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.4");
  OID *oid_instance1 = NULL;
  OID *oid_instance2 = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfVirtIfAreaId = MakeOctetString((char*) &ospfVirtIfAreaId, 4);
  OctetString *os_ospfVirtIfNeighbor = MakeOctetString((char*) &ospfVirtIfNeighbor, 4);
  OctetString *os_ospfLsdbLsid = MakeOctetString((char*) &ospfLsdbLsid, 4);
  OctetString *os_ospfLsdbRouterId = MakeOctetString((char*) &ospfLsdbRouterId, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfVirtIfAreaId = NULL;
  VarBind *var_ospfVirtIfNeighbor = NULL;
  VarBind *var_ospfPacketType = MakeVarBindWithValue(oid_ospfPacketType, NULL, INTEGER_TYPE, &ospfPacketType);
  VarBind *var_ospfLsdbType = NULL;
  VarBind *var_ospfLsdbLsid = NULL;
  VarBind *var_ospfLsdbRouterId = NULL;

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[0];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[1];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[2];
  buffer[index++] = ((char*)&ospfVirtIfAreaId)[3];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[0];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[1];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[2];
  buffer[index++] = ((char*)&ospfVirtIfNeighbor)[3];
  oid_instance1 = MakeOID(buffer, index);

  var_ospfVirtIfAreaId = MakeVarBindWithValue(oid_ospfVirtIfAreaId, oid_instance1, IP_ADDR_PRIM_TYPE, os_ospfVirtIfAreaId);
  var_ospfVirtIfNeighbor = MakeVarBindWithValue(oid_ospfVirtIfNeighbor, oid_instance1, IP_ADDR_PRIM_TYPE, os_ospfVirtIfNeighbor);

  /* create instance OID */
  index = 0;
  buffer[index++] = ospfLsdbType;
  buffer[index++] = ((char*)&ospfLsdbLsid)[0];
  buffer[index++] = ((char*)&ospfLsdbLsid)[1];
  buffer[index++] = ((char*)&ospfLsdbLsid)[2];
  buffer[index++] = ((char*)&ospfLsdbLsid)[3];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[0];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[1];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[2];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[3];
  oid_instance2 = MakeOID(buffer, index);

  var_ospfLsdbType = MakeVarBindWithValue(oid_ospfLsdbType, oid_instance2, INTEGER_TYPE, &ospfLsdbType);
  var_ospfLsdbLsid = MakeVarBindWithValue(oid_ospfLsdbLsid, oid_instance2, IP_ADDR_PRIM_TYPE, os_ospfLsdbLsid);
  var_ospfLsdbRouterId = MakeVarBindWithValue(oid_ospfLsdbRouterId, oid_instance2, IP_ADDR_PRIM_TYPE, os_ospfLsdbRouterId);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfVirtIfAreaId);
  FreeOID(oid_ospfVirtIfNeighbor);
  FreeOID(oid_ospfPacketType);
  FreeOID(oid_ospfLsdbType);
  FreeOID(oid_ospfLsdbLsid);
  FreeOID(oid_ospfLsdbRouterId);
  FreeOID(oid_instance1);
  FreeOID(oid_instance2);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfVirtIfAreaId == NULL || var_ospfVirtIfNeighbor == NULL || var_ospfPacketType == NULL || var_ospfLsdbType == NULL || var_ospfLsdbLsid == NULL || var_ospfLsdbRouterId == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfVirtIfAreaId);
    FreeVarBind(var_ospfVirtIfNeighbor);
    FreeVarBind(var_ospfPacketType);
    FreeVarBind(var_ospfLsdbType);
    FreeVarBind(var_ospfLsdbLsid);
    FreeVarBind(var_ospfLsdbRouterId);
    return L7_FAILURE;
  }
  
  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfVirtIfAreaId;
  var_ospfVirtIfAreaId->next_var = var_ospfVirtIfNeighbor;
  var_ospfVirtIfNeighbor->next_var = var_ospfPacketType;
  var_ospfPacketType->next_var = var_ospfLsdbType;
  var_ospfLsdbType->next_var = var_ospfLsdbLsid;
  var_ospfLsdbLsid->next_var = var_ospfLsdbRouterId;
  var_ospfLsdbRouterId->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfOriginateLsaTrapSend(L7_uint32 ospfRouterId, 
                                      L7_uint32 ospfLsdbAreaId,
                                      L7_uint32 ospfLsdbType, 
                                      L7_uint32 ospfLsdbLsid,
                                      L7_uint32 ospfLsdbRouterId)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.12");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfLsdbAreaId = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.1");
  OID *oid_ospfLsdbType = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.2");
  OID *oid_ospfLsdbLsid = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.3");
  OID *oid_ospfLsdbRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.4");
  OID *oid_instance = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfLsdbAreaId = MakeOctetString((char*) &ospfLsdbAreaId, 4);
  OctetString *os_ospfLsdbLsid = MakeOctetString((char*) &ospfLsdbLsid, 4);
  OctetString *os_ospfLsdbRouterId = MakeOctetString((char*) &ospfLsdbRouterId, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfLsdbAreaId = NULL;
  VarBind *var_ospfLsdbType = NULL;
  VarBind *var_ospfLsdbLsid = NULL;
  VarBind *var_ospfLsdbRouterId = NULL;

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfLsdbAreaId)[0];
  buffer[index++] = ((char*)&ospfLsdbAreaId)[1];
  buffer[index++] = ((char*)&ospfLsdbAreaId)[2];
  buffer[index++] = ((char*)&ospfLsdbAreaId)[3];
  buffer[index++] = ospfLsdbType;
  buffer[index++] = ((char*)&ospfLsdbLsid)[0];
  buffer[index++] = ((char*)&ospfLsdbLsid)[1];
  buffer[index++] = ((char*)&ospfLsdbLsid)[2];
  buffer[index++] = ((char*)&ospfLsdbLsid)[3];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[0];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[1];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[2];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[3];
  oid_instance = MakeOID(buffer, index);

  var_ospfLsdbAreaId = MakeVarBindWithValue(oid_ospfLsdbAreaId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfLsdbAreaId);
  var_ospfLsdbType = MakeVarBindWithValue(oid_ospfLsdbType, oid_instance, INTEGER_TYPE, &ospfLsdbType);
  var_ospfLsdbLsid = MakeVarBindWithValue(oid_ospfLsdbLsid, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfLsdbLsid);
  var_ospfLsdbRouterId = MakeVarBindWithValue(oid_ospfLsdbRouterId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfLsdbRouterId);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfLsdbAreaId);
  FreeOID(oid_ospfLsdbType);
  FreeOID(oid_ospfLsdbLsid);
  FreeOID(oid_ospfLsdbRouterId);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfLsdbAreaId == NULL || var_ospfLsdbType == NULL || var_ospfLsdbLsid == NULL || var_ospfLsdbRouterId == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfLsdbAreaId);
    FreeVarBind(var_ospfLsdbType);
    FreeVarBind(var_ospfLsdbLsid);
    FreeVarBind(var_ospfLsdbRouterId);

    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfLsdbAreaId;
  var_ospfLsdbAreaId->next_var = var_ospfLsdbType;
  var_ospfLsdbType->next_var = var_ospfLsdbLsid;
  var_ospfLsdbLsid->next_var = var_ospfLsdbRouterId;
  var_ospfLsdbRouterId->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfMaxAgeLsaTrapSend(L7_uint32 ospfRouterId, 
                                   L7_uint32 ospfLsdbAreaId,
                                   L7_uint32 ospfLsdbType, 
                                   L7_uint32 ospfLsdbLsid,
                                   L7_uint32 ospfLsdbRouterId)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.13");
  VarBind *temp_vb = NULL;
  L7_uint32 buffer[52];
  L7_uint32 index;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfLsdbAreaId = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.1");
  OID *oid_ospfLsdbType = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.2");
  OID *oid_ospfLsdbLsid = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.3");
  OID *oid_ospfLsdbRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.4.1.4");
  OID *oid_instance = NULL;

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);
  OctetString *os_ospfLsdbAreaId = MakeOctetString((char*) &ospfLsdbAreaId, 4);
  OctetString *os_ospfLsdbLsid = MakeOctetString((char*) &ospfLsdbLsid, 4);
  OctetString *os_ospfLsdbRouterId = MakeOctetString((char*) &ospfLsdbRouterId, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfLsdbAreaId = NULL;
  VarBind *var_ospfLsdbType = NULL;
  VarBind *var_ospfLsdbLsid = NULL;
  VarBind *var_ospfLsdbRouterId = NULL;

  /* create instance OID */
  index = 0;
  buffer[index++] = ((char*)&ospfLsdbAreaId)[0];
  buffer[index++] = ((char*)&ospfLsdbAreaId)[1];
  buffer[index++] = ((char*)&ospfLsdbAreaId)[2];
  buffer[index++] = ((char*)&ospfLsdbAreaId)[3];
  buffer[index++] = ospfLsdbType;
  buffer[index++] = ((char*)&ospfLsdbLsid)[0];
  buffer[index++] = ((char*)&ospfLsdbLsid)[1];
  buffer[index++] = ((char*)&ospfLsdbLsid)[2];
  buffer[index++] = ((char*)&ospfLsdbLsid)[3];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[0];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[1];
  buffer[index++] = ((char*)&ospfLsdbRouterId)[2];
  buffer[index++] = (((char*)&ospfLsdbRouterId))[3];
  oid_instance = MakeOID(buffer, index);

  var_ospfLsdbAreaId = MakeVarBindWithValue(oid_ospfLsdbAreaId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfLsdbAreaId);
  var_ospfLsdbType = MakeVarBindWithValue(oid_ospfLsdbType, oid_instance, INTEGER_TYPE, &ospfLsdbType);
  var_ospfLsdbLsid = MakeVarBindWithValue(oid_ospfLsdbLsid, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfLsdbLsid);
  var_ospfLsdbRouterId = MakeVarBindWithValue(oid_ospfLsdbRouterId, oid_instance, IP_ADDR_PRIM_TYPE, os_ospfLsdbRouterId);

  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfLsdbAreaId);
  FreeOID(oid_ospfLsdbType);
  FreeOID(oid_ospfLsdbLsid);
  FreeOID(oid_ospfLsdbRouterId);
  FreeOID(oid_instance);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfLsdbAreaId == NULL || var_ospfLsdbType == NULL || var_ospfLsdbLsid == NULL || var_ospfLsdbRouterId == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfLsdbAreaId);
    FreeVarBind(var_ospfLsdbType);
    FreeVarBind(var_ospfLsdbLsid);
    FreeVarBind(var_ospfLsdbRouterId);

    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfLsdbAreaId;
  var_ospfLsdbAreaId->next_var = var_ospfLsdbType;
  var_ospfLsdbType->next_var = var_ospfLsdbLsid;
  var_ospfLsdbLsid->next_var = var_ospfLsdbRouterId;
  var_ospfLsdbRouterId->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfLsdbOverflowTrapSend(L7_uint32 ospfRouterId, 
                                      L7_uint32 ospfExtLsdbLimit)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.14");
  VarBind *temp_vb = NULL;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfExtLsdbLimit = MakeOIDFromDot("1.3.6.1.2.1.14.1.11.0");

  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfExtLsdbLimit = MakeVarBindWithValue(oid_ospfExtLsdbLimit, NULL, INTEGER_TYPE, &ospfExtLsdbLimit);
  
  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfExtLsdbLimit);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfExtLsdbLimit == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfExtLsdbLimit);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfExtLsdbLimit;
  var_ospfExtLsdbLimit->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_ospfLsdbApproachingOverflowTrapSend(L7_uint32 ospfRouterId,
                                                 L7_uint32 ospfExtLsdbLimit)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("ospfTraps.15");
  VarBind *temp_vb = NULL;

  OID *oid_ospfRouterId = MakeOIDFromDot("1.3.6.1.2.1.14.1.1.0");
  OID *oid_ospfExtLsdbLimit = MakeOIDFromDot("1.3.6.1.2.1.14.1.11.0");
  
  OctetString *os_ospfRouterId = MakeOctetString((char*) &ospfRouterId, 4);

  VarBind *var_ospfRouterId = MakeVarBindWithValue(oid_ospfRouterId, NULL, IP_ADDR_PRIM_TYPE, os_ospfRouterId);
  VarBind *var_ospfExtLsdbLimit = MakeVarBindWithValue(oid_ospfExtLsdbLimit, NULL, INTEGER_TYPE, &ospfExtLsdbLimit);
  
  FreeOID(oid_ospfRouterId);
  FreeOID(oid_ospfExtLsdbLimit);

  if (snmpTrapOID == NULL || var_ospfRouterId == NULL || var_ospfExtLsdbLimit == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_ospfRouterId);
    FreeVarBind(var_ospfExtLsdbLimit);
    return L7_FAILURE;
  }

  temp_vb = var_ospfRouterId;
  var_ospfRouterId->next_var = var_ospfExtLsdbLimit;
  var_ospfExtLsdbLimit->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}
/* End Function Declarations*/

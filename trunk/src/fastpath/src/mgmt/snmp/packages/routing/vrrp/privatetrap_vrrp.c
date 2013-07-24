
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: privatetrap_vrrp.c
*
* Purpose: VRRP trap functions
*
* Created by: Mike Fiorito 10/26/2001
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
#include "privatetrap_vrrp.h"
        
/* Begin Function Declarations: privatetrap_vrrp.c */

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
L7_RC_t snmp_vrrpTrapNewMasterTrapSend(L7_uint32 vrrpOperMasterIpAddr)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("vrrpNotifications.1");

  OID *oid_vrrpOperMasterIpAddr = MakeOIDFromDot("1.3.6.1.2.1.68.1.3.1.7");
  
  OctetString *os_vrrpOperMasterIpAddr = MakeOctetString((char*) &vrrpOperMasterIpAddr, 4);
  VarBind *var_vrrpOperMasterIpAddr = MakeVarBindWithValue(oid_vrrpOperMasterIpAddr, NULL, IP_ADDR_PRIM_TYPE, os_vrrpOperMasterIpAddr);

  FreeOID(oid_vrrpOperMasterIpAddr);

  if (snmpTrapOID == NULL || var_vrrpOperMasterIpAddr == NULL)
  {
    FreeOID(snmpTrapOID);
    FreeVarBind(var_vrrpOperMasterIpAddr);
    return L7_FAILURE;
  }

  var_vrrpOperMasterIpAddr->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, var_vrrpOperMasterIpAddr, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(var_vrrpOperMasterIpAddr);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
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
L7_RC_t snmp_vrrpTrapAuthFailureTrapSend(L7_uint32 vrrpTrapPacketSrc, 
                                         L7_int32 vrrpTrapAuthErrorType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("vrrpNotifications.2");
  VarBind *temp_vb = NULL;

  OID *oid_vrrpTrapPacketSrc = MakeOIDFromDot("1.3.6.1.2.1.68.1.5");
  OID *oid_vrrpTrapAuthErrorType = MakeOIDFromDot("1.3.6.1.2.1.68.1.6");
  
  OctetString *os_vrrpTrapPacketSrc = MakeOctetString((char*) &vrrpTrapPacketSrc, 4);
  VarBind *var_vrrpTrapPacketSrc = MakeVarBindWithValue(oid_vrrpTrapPacketSrc, NULL, IP_ADDR_PRIM_TYPE, os_vrrpTrapPacketSrc);
  VarBind *var_vrrpTrapAuthErrorType = MakeVarBindWithValue(oid_vrrpTrapAuthErrorType, NULL, INTEGER_TYPE, &vrrpTrapAuthErrorType);

  FreeOID(oid_vrrpTrapPacketSrc);
  FreeOID(oid_vrrpTrapAuthErrorType);

  if (snmpTrapOID == NULL || var_vrrpTrapPacketSrc == NULL || var_vrrpTrapAuthErrorType == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_vrrpTrapPacketSrc);
    FreeVarBind(var_vrrpTrapAuthErrorType);
    return L7_FAILURE;
  }

  temp_vb = var_vrrpTrapPacketSrc;
  var_vrrpTrapPacketSrc->next_var = var_vrrpTrapAuthErrorType;
  var_vrrpTrapAuthErrorType->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

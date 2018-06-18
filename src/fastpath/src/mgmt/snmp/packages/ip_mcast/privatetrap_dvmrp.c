/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: privatetrap_dvmrp.c
*
* Purpose: DVMRP trap functions
*
* Created by: Mahe Korukonda (BRI) 03/19/2002
*
* Component: SNMP
*
*********************************************************************/
/********************************************************************
 *                    
 *******************************************************************/
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
#include "privatetrap_dvmrp.h"


L7_RC_t snmp_dvmrpNeighborLossTrapSend(L7_uint32 dvmrpInterfaceLocalAddress, L7_uint32 dvmrpNeighborState)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("dvmrpTraps.1");
  VarBind *temp_vb = NULL;

  OID *oid_dvmrpInterfaceLocalAddress = MakeOIDFromDot("dvmrpInterfaceLocalAddress");
  OID *oid_dvmrpNeighborState = MakeOIDFromDot("dvmrpNeighborState");

  OctetString *os_dvmrpInterfaceLocalAddress = MakeOctetString((char*) &dvmrpInterfaceLocalAddress, 4);
  VarBind *var_dvmrpInterfaceLocalAddress = MakeVarBindWithValue(oid_dvmrpInterfaceLocalAddress, NULL, IP_ADDR_PRIM_TYPE, os_dvmrpInterfaceLocalAddress);
  VarBind *var_dvmrpNeighborState = MakeVarBindWithValue(oid_dvmrpNeighborState, NULL, INTEGER_TYPE, &dvmrpNeighborState);

  FreeOID(oid_dvmrpInterfaceLocalAddress);
  FreeOID(oid_dvmrpNeighborState);

  if (snmpTrapOID == NULL || var_dvmrpInterfaceLocalAddress == NULL || var_dvmrpNeighborState == NULL)
  {
    FreeOID(snmpTrapOID);
    FreeVarBind(var_dvmrpInterfaceLocalAddress);
    FreeVarBind(var_dvmrpNeighborState);
    return L7_FAILURE;
  }

  temp_vb = var_dvmrpInterfaceLocalAddress;
  var_dvmrpInterfaceLocalAddress->next_var = var_dvmrpNeighborState;
  var_dvmrpNeighborState->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}


L7_RC_t snmp_dvmrpNeighborNotPruningTrapSend(L7_uint32 dvmrpInterfaceLocalAddress, L7_uint32 dvmrpNeighborCapabilities)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("dvmrpTraps.2");
  VarBind *temp_vb = NULL;
  L7_uchar8 val8 = (L7_uchar8)(dvmrpNeighborCapabilities & 0x000000FF);

  OID *oid_dvmrpInterfaceLocalAddress = MakeOIDFromDot("dvmrpInterfaceLocalAddress");
  OID *oid_dvmrpNeighborCapabilities = MakeOIDFromDot("dvmrpNeighborCapabilities");

  OctetString *os_dvmrpInterfaceLocalAddress = MakeOctetString((char*) &dvmrpInterfaceLocalAddress, 4);
  VarBind *var_dvmrpInterfaceLocalAddress = MakeVarBindWithValue(oid_dvmrpInterfaceLocalAddress, NULL, IP_ADDR_PRIM_TYPE, os_dvmrpInterfaceLocalAddress);
  OctetString *os_dvmrpNeighborCapabilities = MakeOctetString((char *)&val8, sizeof(L7_uchar8));
  VarBind *var_dvmrpNeighborCapabilities = MakeVarBindWithValue(oid_dvmrpNeighborCapabilities, NULL, BITS_TYPE, os_dvmrpNeighborCapabilities);

  FreeOID(oid_dvmrpInterfaceLocalAddress);
  FreeOID(oid_dvmrpNeighborCapabilities);

  if (snmpTrapOID == NULL || var_dvmrpInterfaceLocalAddress == NULL || var_dvmrpNeighborCapabilities == NULL)
  {
    FreeOID(snmpTrapOID);
    FreeVarBind(var_dvmrpInterfaceLocalAddress);
    FreeVarBind(var_dvmrpNeighborCapabilities);
    return L7_FAILURE;
  }

  temp_vb = var_dvmrpInterfaceLocalAddress;
  var_dvmrpInterfaceLocalAddress->next_var = var_dvmrpNeighborCapabilities;
  var_dvmrpNeighborCapabilities->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: privatetrap_pim.c
*
* Purpose: PIM trap functions
*
* Created by: Abdul Shareef (BRI) 02/05/2002
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
#include "privatetrap_pim.h"
        


/*********************************************************************
*
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    pimNeighborIfIndex      The lost neighbor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t snmp_pimNeighborLossTrapSend(L7_uint32 pimNeighborIfIndex)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("pimTraps.15");
  VarBind *temp_vb = NULL;

  /* 1.3.6.1.3.61.1.1.3.1.2 = pimNeighborIfIndex */
  OID *oid_pimNeighborIfIndex = MakeOIDFromDot("1.3.6.1.3.61.1.1.3.1.2");
  VarBind *var_pimNeighborIfIndex = MakeVarBindWithValue(oid_pimNeighborIfIndex, NULL, INTEGER_TYPE, &pimNeighborIfIndex);
  FreeOID(oid_pimNeighborIfIndex);

  if (snmpTrapOID == NULL || var_pimNeighborIfIndex == NULL)
  {
    FreeOID(snmpTrapOID);
    FreeVarBind(var_pimNeighborIfIndex);
    return L7_FAILURE;
  }

  temp_vb = var_pimNeighborIfIndex;
  var_pimNeighborIfIndex->next_var = NULL;
  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    pimNeighborUpTime      The UpTime of the lost neighbor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snmp_pimSmNeighborLossTrapSend(L7_uint32 pimSmNeighborUpTime)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("pimSmTraps.15");
  VarBind *temp_vb = NULL;

  /* 1.3.6.1.2.1.203.1.1.2.1.4 = oid_pimSmNeighborUpTime */
  OID *oid_pimSmNeighborUpTime = MakeOIDFromDot("1.3.6.1.2.1.203.1.1.2.1.4");
  VarBind *var_pimSmNeighborUpTime = MakeVarBindWithValue(oid_pimSmNeighborUpTime, NULL, TIME_TICKS_TYPE, &pimSmNeighborUpTime);
  FreeOID(oid_pimSmNeighborUpTime);

  if (snmpTrapOID == NULL || var_pimSmNeighborUpTime == NULL)
  {
    FreeOID(snmpTrapOID);
    FreeVarBind(var_pimSmNeighborUpTime);
    return L7_FAILURE;
  }

  temp_vb = var_pimSmNeighborUpTime;
  var_pimSmNeighborUpTime->next_var = NULL;
  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}


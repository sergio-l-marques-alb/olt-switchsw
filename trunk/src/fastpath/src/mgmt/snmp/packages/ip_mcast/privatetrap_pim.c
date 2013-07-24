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
* @param    pimNeighborUpTime      The UpTime of the lost neighbor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snmp_pimNeighborLossTrapSend(L7_uint32 pimNeighborUpTime)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("pimTraps.1");
  VarBind *temp_vb = NULL;

  /* 1.3.6.1.2.1.157.1.1.2.1.6 = oid_pimNeighborUpTime */
  OID *oid_pimNeighborUpTime = MakeOIDFromDot("1.3.6.1.2.1.157.1.1.2.1.6");
  VarBind *var_pimNeighborUpTime = MakeVarBindWithValue(oid_pimNeighborUpTime, NULL, TIME_TICKS_TYPE, &pimNeighborUpTime);
  FreeOID(oid_pimNeighborUpTime);

  if (snmpTrapOID == NULL || var_pimNeighborUpTime == NULL)
  {
    FreeOID(snmpTrapOID);
    FreeVarBind(var_pimNeighborUpTime);
    return L7_FAILURE;
  }

  temp_vb = var_pimNeighborUpTime;
  var_pimNeighborUpTime->next_var = NULL;
  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}


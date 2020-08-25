/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap_entity.c
*
* Purpose: Entity-related trap functions
*
* Created by: cpverne 06/26/2003
*
* Component: SNMP
*
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
#include "privatetrap_entity.h"

/* Begin Function Definitions: privatetrap_entity.h */

/*********************************************************************
*
* @purpose  Sends a trap associated with inserting a card into a slot
*           which is not supported by the slot.
*
* @param    unitIndex       Unit Index
* @param    slotIndex       Slot Index
* @param    insertedType    Card type inserted into the 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t snmp_entConfigChangeTrapSend()
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("entConfigChange");

  if (snmpTrapOID == NULL)
  {
    return L7_FAILURE;
  }

  status = SendNotificationsSMIv2Params(snmpTrapOID, NULL, NULL);

  FreeOID(snmpTrapOID);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/* End Function Definitions */

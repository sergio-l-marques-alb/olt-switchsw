/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmptrap.c
*
* Purpose: API interface for sending traps via the SNMP agent
*
* Created by: Colin Verne 12/06/2000
*
* Component: SNMP
*
*********************************************************************/
/*************************************************************

*************************************************************/



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
#include "trap.h"
#include "ntfy_oid.h"
#include "sr_ntfy.h"

#include "snmptrap.h"

/* Begin Function Declarations: snmptrap.h */

/*********************************************************************
*
* @purpose  Send a Cold Start Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
ColdStartTrapSend ()
{
   if (SendNotificationsSMIv2Params(coldStartTrap, NULL, NULL) == 0)
      return L7_SUCCESS;

   return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Send a Warm Start Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
WarmStartTrapSend ()
{
   if (SendNotificationsSMIv2Params(warmStartTrap, NULL, NULL) == 0)
      return L7_SUCCESS;

   return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Send a Link Down Trap
*
* @param    ifIndex     External ifIndex of link that went down.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
LinkDownTrapSend ( L7_uint32 ifIndex )
{
   L7_uint32 snmp_int_buffer[1];
   OID *object;
   OID *instance;
   VarBind *vb_ptr;

   if ((object = MakeOIDFromDot("ifIndex")) == NULL)
      return L7_FAILURE;

   snmp_int_buffer[0] = ifIndex;
   if ((instance = MakeOID(snmp_int_buffer, 1)) == NULL)
      return L7_FAILURE;

   vb_ptr = MakeVarBindWithNull(object, instance);
   FreeOID(object);
   FreeOID(instance);

   if (vb_ptr == NULL)
      return L7_FAILURE;

   if (SendNotificationsSMIv2Params(linkDownTrap, vb_ptr, NULL) == 0)
   {
      FreeVarBindList(vb_ptr);
      return L7_SUCCESS;
   }

   FreeVarBindList(vb_ptr);
   return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  Send a Link Up Trap
*
* @param    ifIndex     External ifIndex of link that came up.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
LinkUpTrapSend ( L7_uint32 ifIndex )
{
   L7_uint32 snmp_int_buffer[1];
   OID *object;
   OID *instance;
   VarBind *vb_ptr;

   if ((object = MakeOIDFromDot("ifIndex")) == NULL)
      return L7_FAILURE;

   snmp_int_buffer[0] = ifIndex;
   if ((instance = MakeOID(snmp_int_buffer, 1)) == NULL)
      return L7_FAILURE;

   vb_ptr = MakeVarBindWithNull(object, instance);
   FreeOID(object);
   FreeOID(instance);

   if (vb_ptr == NULL)
      return L7_FAILURE;

   if (SendNotificationsSMIv2Params(linkUpTrap, vb_ptr, NULL) == 0)
   {
      FreeVarBindList(vb_ptr);
      return L7_SUCCESS;
   }

   FreeVarBindList(vb_ptr);
   return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Send Authentication failure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
AuthenFailureTrapSend ()
{
   if (SendNotificationsSMIv2Params(authenFailureTrap, NULL, NULL) == 0)
      return L7_SUCCESS;

   return L7_FAILURE;
}

/* End Function Declarations */

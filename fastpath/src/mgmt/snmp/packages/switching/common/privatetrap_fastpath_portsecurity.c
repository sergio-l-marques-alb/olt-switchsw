/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap_fastpath_portsecurity.c
*
* Purpose: Port Security trap functions
*
* Created by: kmanish 06/18/2004
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
#include "k_private_base.h" 
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "privatetrap_fastpath_portsecurity.h"
#include "k_mib_fastpath_portsecurity_api.h"



/* Begin Function Definitions: privatetrap_fastpath_portsecurity.h */

/*********************************************************************
*
* @purpose  Sends when a packet is received on a locked port with a 
*           source MAC address that is not allowed.
*
* @param    intIfNum       Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentPortSecurityViolationTrapSend(L7_uint32 intIfNum)
{
  L7_uint32 status;
  L7_uint32 extIfNum;
  VarBind *temp_vb = NULL;
  OID *snmpTrapOID;
  OID *oid_ifIndex;
  OID *oid_agentPortSecurityLastDiscardedMAC;
  OctetString *os_agentPortSecurityLastDiscardedMAC;
  VarBind *var_ifIndex;
  VarBind *var_agentPortSecurityLastDiscardedMAC;
  
  L7_uchar8 discardedMAC[SNMP_BUFFER_LEN];
  bzero(discardedMAC, SNMP_BUFFER_LEN);
  
  /* Making OIDs for agentPortSecurityViolation,ifIndex and agentPortSecurityLastDiscardedMAC */
  snmpTrapOID = MakeOIDFromDot("1.3.6.1.4.1.6132.1.1.20.2.1");
  
  oid_ifIndex = MakeOIDFromDot("ifIndex");
  oid_agentPortSecurityLastDiscardedMAC  = MakeOIDFromDot("agentPortSecurityLastDiscardedMAC");
 
  /* Converting external interface to internal interface */ 
  if(L7_SUCCESS != usmDbExtIfNumFromIntIfNum( intIfNum, &extIfNum))
     return L7_FAILURE; 
  
  /* Getting discarded MAC address for the internal interface number */
  if(L7_SUCCESS != snmpAgentPortSecurityLastDiscardedMACGet(intIfNum,
			  discardedMAC))
    return L7_FAILURE;
 
  os_agentPortSecurityLastDiscardedMAC = MakeOctetString(discardedMAC,
                                                      strlen( discardedMAC));
  
  /* Creating Var Binds */
  var_ifIndex = MakeVarBindWithValue(oid_ifIndex, NULL, 
		                     INTEGER_TYPE, &extIfNum);
  var_agentPortSecurityLastDiscardedMAC =
	  MakeVarBindWithValue(oid_agentPortSecurityLastDiscardedMAC, 
                               NULL, OCTET_PRIM_TYPE,
			       os_agentPortSecurityLastDiscardedMAC);
  FreeOID(oid_ifIndex);
  FreeOID(oid_agentPortSecurityLastDiscardedMAC);
  
  if (snmpTrapOID == NULL || var_ifIndex == NULL 
      || var_agentPortSecurityLastDiscardedMAC == NULL)
  {
    FreeOID(snmpTrapOID);
    FreeVarBind(var_ifIndex);
    FreeVarBind(var_agentPortSecurityLastDiscardedMAC);
    return L7_FAILURE;
  }

  temp_vb = var_ifIndex;
  var_ifIndex->next_var = var_agentPortSecurityLastDiscardedMAC;
  var_agentPortSecurityLastDiscardedMAC->next_var = NULL;
  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/* End Function Definitions */

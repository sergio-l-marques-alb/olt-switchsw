/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap_vlan.c
*
* Purpose: VLAN-related trap functions
*
* Created by: Mike Fiorito 09/19/2001
*
* Component: SNMP
*
*********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
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
#include "usmdb_util_api.h"
#include "privatetrap_vlan.h"

/* Begin Function Declarations: privatetrap_vlan.c */
/*********************************************************************
*
* @purpose  Creates an SNMP trap OID and a VarBind structure and sends
*           them as a VLAN-related trap.
*
* @param    vlanIndex       VLAN Index
* @param    snmpTrapOIDDot  character string form of an OID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t snmp_VlanTrapSend(L7_int32 instanceID, L7_uchar8 *snmpTrapOIDDot)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot(snmpTrapOIDDot);
  
  OID *oid_instanceID = MakeOIDFromDot("1.3.6.1.2.1.17.7.1.4.2.1.2");

  VarBind *var_instanceID = MakeVarBindWithValue(oid_instanceID, NULL, INTEGER_TYPE, &instanceID);

  FreeOID(oid_instanceID);

  if (snmpTrapOID == NULL || var_instanceID == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_instanceID);
    return L7_FAILURE;
  }

  var_instanceID->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, var_instanceID, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(var_instanceID);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Signifies that there has been an error processing a VLAN
*           configuration request.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t snmp_VlanRequestFailureTrapSend(L7_int32 vlanIndex)
{
  return snmp_VlanTrapSend(vlanIndex, (L7_uchar8 *)"vlanRequestFailureTrap");
}

/*********************************************************************
*
* @purpose  Signifies that the last (or Default) VLAN is being deleted.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t snmp_VlanDeleteLastTrapSend(L7_int32 vlanIndex)
{
  return snmp_VlanTrapSend(vlanIndex, (L7_uchar8 *)"vlanDeleteLastTrap");
}

/*********************************************************************
*
* @purpose  Signifies that the default VLAN configuration has failed.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t snmp_VlanDefaultCfgFailureTrapSend(L7_int32 vlanIndex)
{
  return snmp_VlanTrapSend(vlanIndex, (L7_uchar8 *)"vlanDefaultCfgFailureTrap");
}

/*********************************************************************
*
* @purpose  Signifies that there has been an error restoring the prior
*           VLAN configuration
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t snmp_VlanRestoreFailureTrapSend(L7_int32 vlanIndex)
{
  return snmp_VlanTrapSend(vlanIndex, (L7_uchar8 *)"vlanRestoreFailureTrap");
}

/*********************************************************************
*
* @purpose  Signifies that there is a new root bridge for the STP instance
*           associated with a particular VLAN.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t snmp_StpInstanceNewRootTrapSend(L7_int32 instanceID)
{
  return snmp_VlanTrapSend(instanceID, (L7_uchar8 *)"stpInstanceNewRootTrap");
}

/*********************************************************************
*
* @purpose  Signifies that this port in this MSTP instance enters
            loop inconsistent state upon failure to receive a BPDU.
*
* @param    instanceID     MST Instance Index
* @param    intIfNum       Internal interface number          
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t snmp_StpInstanceLoopInconsistentStartTrapSend(L7_int32 instanceID, L7_uint32 intIfNum)
{
  L7_uint32 status;
  L7_uint32 extIfNum;  
  OID *snmpTrapOID = L7_NULLPTR;
  OID *oid_agentStpMstId = L7_NULLPTR;
  OID *oid_ifIndex = L7_NULLPTR;
  VarBind *var_agentStpMstId = L7_NULLPTR;
  VarBind *var_ifIndex = L7_NULLPTR;
  VarBind *temp_vb = L7_NULLPTR;
  
  snmpTrapOID = MakeOIDFromDot("stpInstanceLoopInconsistentStartTrap");
  oid_agentStpMstId = MakeOIDFromDot("agentStpMstId");
  oid_ifIndex = MakeOIDFromDot("ifIndex");

  if(L7_SUCCESS != usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum))
     return L7_FAILURE; 
  
  var_agentStpMstId = MakeVarBindWithValue(oid_agentStpMstId, L7_NULLPTR, INTEGER_TYPE, &instanceID);
  var_ifIndex = MakeVarBindWithValue(oid_ifIndex, L7_NULLPTR, INTEGER_TYPE, &extIfNum);

  FreeOID(oid_agentStpMstId);
  FreeOID(oid_ifIndex);

  if ((snmpTrapOID == L7_NULLPTR) || (var_agentStpMstId == L7_NULLPTR) || (var_ifIndex == L7_NULLPTR))
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_agentStpMstId);
    FreeVarBind(var_ifIndex);
    return L7_FAILURE;
  }

  temp_vb = var_agentStpMstId;
  var_agentStpMstId->next_var = var_ifIndex;
  var_ifIndex->next_var = L7_NULLPTR;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, L7_NULLPTR);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Signifies that this port in this MSTP instance exits
            loop inconsistent state upon reception of a BPDU.
*
* @param    instanceID     MST Instance Index
* @param    intIfNum       Internal interface number          
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t snmp_StpInstanceLoopInconsistentEndTrapSend(L7_int32 instanceID, L7_uint32 intIfNum)
{
  L7_uint32 status;
  L7_uint32 extIfNum;  
  OID *snmpTrapOID = L7_NULLPTR;
  OID *oid_agentStpMstId = L7_NULLPTR;
  OID *oid_ifIndex = L7_NULLPTR;
  VarBind *var_agentStpMstId = L7_NULLPTR;
  VarBind *var_ifIndex = L7_NULLPTR;
  VarBind *temp_vb = L7_NULLPTR;

  snmpTrapOID = MakeOIDFromDot("stpInstanceLoopInconsistentEndTrap");
  oid_agentStpMstId = MakeOIDFromDot("agentStpMstId");
  oid_ifIndex = MakeOIDFromDot("ifIndex");

  if(L7_SUCCESS != usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum))
     return L7_FAILURE; 
  
  var_agentStpMstId = MakeVarBindWithValue(oid_agentStpMstId, L7_NULLPTR, INTEGER_TYPE, &instanceID);
  var_ifIndex = MakeVarBindWithValue(oid_ifIndex, L7_NULLPTR, INTEGER_TYPE, &extIfNum);

  FreeOID(oid_agentStpMstId);
  FreeOID(oid_ifIndex);

  if ((snmpTrapOID == L7_NULLPTR) || (var_agentStpMstId == L7_NULLPTR) || (var_ifIndex == L7_NULLPTR))
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_agentStpMstId);
    FreeVarBind(var_ifIndex);
    return L7_FAILURE;
  }

  temp_vb = var_agentStpMstId;
  var_agentStpMstId->next_var = var_ifIndex;
  var_ifIndex->next_var = L7_NULLPTR;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, L7_NULLPTR);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Signifies that there has been a topology change for the
*           STP instance associated with a particular VLAN.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t snmp_StpInstanceTopologyChangeTrapSend(L7_int32 instanceID)
{
  return snmp_VlanTrapSend(instanceID, (L7_uchar8 *)"stpInstanceTopologyChangeTrap");
}

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap_fastpathinventory.c
*
* Purpose: Inventory-related trap functions
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
#include "privatetrap_fastpathinventory.h"
#include "sysapi.h"

/* Begin Function Definitions: privatetrap_fastpathinventory.h */

/*********************************************************************
*
* @purpose  Sends a trap associated with inserting a card into a slot
*           which is configured for a card of a different type.
*
* @param    unitIndex       Unit Index
* @param    slotIndex       Slot Index
* @param    insertedType    Card type inserted
* @param    configuredType  Card the slot is configured for
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentInventoryCardMismatchTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 insertedType, L7_uint32 configuredType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("agentInventoryCardMismatch");
  VarBind *temp_vb = NULL;

  OID *oid_agentInventoryUnitNumber = MakeOIDFromDot("agentInventoryUnitNumber");
  OID *oid_agentInventorySlotNumber = MakeOIDFromDot("agentInventorySlotNumber");
  OID *oid_agentInventorySlotInsertedType = MakeOIDFromDot("agentInventorySlotInsertedCardType");
  OID *oid_agentInventorySlotConfiguredType = MakeOIDFromDot("agentInventorySlotConfiguredCardType");
  
  VarBind *var_agentInventoryUnitNumber = MakeVarBindWithValue(oid_agentInventoryUnitNumber, NULL, INTEGER_TYPE, &unitIndex);
  VarBind *var_agentInventorySlotNumber = MakeVarBindWithValue(oid_agentInventorySlotNumber, NULL, INTEGER_TYPE, &slotIndex);
  VarBind *var_agentInventorySlotInsertedType = MakeVarBindWithValue(oid_agentInventorySlotInsertedType, NULL, INTEGER_TYPE, &insertedType);
  VarBind *var_agentInventorySlotConfiguredType = MakeVarBindWithValue(oid_agentInventorySlotConfiguredType, NULL, INTEGER_TYPE, &configuredType);

  FreeOID(oid_agentInventoryUnitNumber);
  FreeOID(oid_agentInventorySlotNumber);
  FreeOID(oid_agentInventorySlotInsertedType);
  FreeOID(oid_agentInventorySlotConfiguredType);

  if (snmpTrapOID == NULL || var_agentInventoryUnitNumber == NULL || var_agentInventorySlotNumber == NULL || var_agentInventorySlotInsertedType == NULL || var_agentInventorySlotConfiguredType == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_agentInventoryUnitNumber);
    FreeVarBind(var_agentInventorySlotNumber);
    FreeVarBind(var_agentInventorySlotInsertedType);
    FreeVarBind(var_agentInventorySlotConfiguredType);
    return L7_FAILURE;
  }

  temp_vb = var_agentInventoryUnitNumber;
  var_agentInventoryUnitNumber->next_var = var_agentInventorySlotNumber;
  var_agentInventorySlotNumber->next_var = var_agentInventorySlotInsertedType;
  var_agentInventorySlotInsertedType->next_var = var_agentInventorySlotConfiguredType;
  var_agentInventorySlotConfiguredType->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}


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
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentInventoryCardUnsupportedTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 insertedType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("agentInventoryCardUnsupported");
  VarBind *temp_vb = NULL;

  OID *oid_agentInventoryUnitNumber = MakeOIDFromDot("agentInventoryUnitNumber");
  OID *oid_agentInventorySlotNumber = MakeOIDFromDot("agentInventorySlotNumber");
  OID *oid_agentInventorySlotInsertedType = MakeOIDFromDot("agentInventorySlotInsertedCardType");
  
  VarBind *var_agentInventoryUnitNumber = MakeVarBindWithValue(oid_agentInventoryUnitNumber, NULL, INTEGER_TYPE, &unitIndex);
  VarBind *var_agentInventorySlotNumber = MakeVarBindWithValue(oid_agentInventorySlotNumber, NULL, INTEGER_TYPE, &slotIndex);
  VarBind *var_agentInventorySlotInsertedType = MakeVarBindWithValue(oid_agentInventorySlotInsertedType, NULL, INTEGER_TYPE, &insertedType);

  FreeOID(oid_agentInventoryUnitNumber);
  FreeOID(oid_agentInventorySlotNumber);
  FreeOID(oid_agentInventorySlotInsertedType);

  if (snmpTrapOID == NULL || var_agentInventoryUnitNumber == NULL || var_agentInventorySlotNumber == NULL || var_agentInventorySlotInsertedType == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_agentInventoryUnitNumber);
    FreeVarBind(var_agentInventorySlotNumber);
    FreeVarBind(var_agentInventorySlotInsertedType);
    return L7_FAILURE;
  }

  temp_vb = var_agentInventoryUnitNumber;
  var_agentInventoryUnitNumber->next_var = var_agentInventorySlotNumber;
  var_agentInventorySlotNumber->next_var = var_agentInventorySlotInsertedType;
  var_agentInventorySlotInsertedType->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Sends a trap associated with the link status of a Front Panel Stack Port going up.
*
* @param    agentInventoryStackPortIndex  Stack Port Index
* @param    agentInventoryStackPortUnit   Unit Index of the Unit the Stack Port is on
* @param    agentInventoryStackPortTag    Tag associated with that Stack Port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentInventoryStackPortLinkUpTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("agentInventoryStackPortLinkUp");
  VarBind *temp_vb = NULL;
  SR_UINT32 instance_sid[1] = { agentInventoryStackPortIndex };

  OID *oid_instance = MakeOID(instance_sid, 1);
  OID *oid_agentInventoryStackPortUnit = MakeOIDFromDot("agentInventoryStackPortUnit");
  OID *oid_agentInventoryStackPortTag = MakeOIDFromDot("agentInventoryStackPortTag");
  
  /* Freed with the varbind */
  OctetString *os_agentInventoryStackPortTag = MakeOctetStringFromText(agentInventoryStackPortTag);

  VarBind *var_agentInventoryStackPortUnit = MakeVarBindWithValue(oid_agentInventoryStackPortUnit, oid_instance, INTEGER_TYPE, &agentInventoryStackPortUnit);
  VarBind *var_agentInventoryStackPortTag = MakeVarBindWithValue(oid_agentInventoryStackPortTag, oid_instance, OCTET_PRIM_TYPE, os_agentInventoryStackPortTag);

  FreeOID(oid_instance);
  FreeOID(oid_agentInventoryStackPortUnit);
  FreeOID(oid_agentInventoryStackPortTag);

  if (snmpTrapOID == NULL || var_agentInventoryStackPortUnit == NULL || var_agentInventoryStackPortTag == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_agentInventoryStackPortUnit);
    if (var_agentInventoryStackPortTag == NULL)
    {
      FreeOctetString(os_agentInventoryStackPortTag);
    }
    else
    {
      FreeVarBind(var_agentInventoryStackPortTag);
    }
    return L7_FAILURE;
  }

  temp_vb = var_agentInventoryStackPortUnit;
  var_agentInventoryStackPortUnit->next_var = var_agentInventoryStackPortTag;
  var_agentInventoryStackPortTag->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Sends a trap associated with the link status of a Front Panel Stack Port going down.
*
* @param    agentInventoryStackPortIndex  Stack Port Index
* @param    agentInventoryStackPortUnit   Unit Index of the Unit the Stack Port is on
* @param    agentInventoryStackPortTag    Tag associated with that Stack Port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentInventoryStackPortLinkDownTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("agentInventoryStackPortLinkDown");
  VarBind *temp_vb = NULL;
  SR_UINT32 instance_sid[1] = { agentInventoryStackPortIndex };

  OID *oid_instance = MakeOID(instance_sid, 1);
  OID *oid_agentInventoryStackPortUnit = MakeOIDFromDot("agentInventoryStackPortUnit");
  OID *oid_agentInventoryStackPortTag = MakeOIDFromDot("agentInventoryStackPortTag");
  
  /* Freed with the varbind */
  OctetString *os_agentInventoryStackPortTag = MakeOctetStringFromText(agentInventoryStackPortTag);

  VarBind *var_agentInventoryStackPortUnit = MakeVarBindWithValue(oid_agentInventoryStackPortUnit, oid_instance, INTEGER_TYPE, &agentInventoryStackPortUnit);
  VarBind *var_agentInventoryStackPortTag = MakeVarBindWithValue(oid_agentInventoryStackPortTag, oid_instance, OCTET_PRIM_TYPE, os_agentInventoryStackPortTag);

  FreeOID(oid_instance);
  FreeOID(oid_agentInventoryStackPortUnit);
  FreeOID(oid_agentInventoryStackPortTag);

  if (snmpTrapOID == NULL || var_agentInventoryStackPortUnit == NULL || var_agentInventoryStackPortTag == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_agentInventoryStackPortUnit);
    if (var_agentInventoryStackPortTag == NULL)
    {
      FreeOctetString(os_agentInventoryStackPortTag);
    }
    else
    {
      FreeVarBind(var_agentInventoryStackPortTag);
    }
    return L7_FAILURE;
  }

  temp_vb = var_agentInventoryStackPortUnit;
  var_agentInventoryStackPortUnit->next_var = var_agentInventoryStackPortTag;
  var_agentInventoryStackPortTag->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/* End Function Definitions */

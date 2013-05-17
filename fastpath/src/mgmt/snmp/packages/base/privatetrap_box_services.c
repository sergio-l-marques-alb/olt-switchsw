/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2008
*
**********************************************************************
*
* Name: privatetrap_box_services.c
*
* Purpose: Box Services trap functions
*
* Created by: vkozlov
*
* Component: SNMP
*
*********************************************************************/
#include "commdefs.h"


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

#include "privatetrap_box_services.h"
        
/* Begin Function Declarations: privatetrap_box_services.h */


L7_RC_t snmp_boxsTemperatureChangeTrapSend( L7_uint32 sensorNum, L7_uint32  tempEventType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("boxsTemperatureChange");
  VarBind *temp_vb = NULL;
  SR_UINT32 instance_sid[1] = { 0 };

  OID *oid_instance = MakeOID(instance_sid, 1);
  OID *oid_boxServicesTempSensorIndex = MakeOIDFromDot("boxServicesTempSensorIndex");
  OID *oid_boxsTemperatureChangeEvent = MakeOIDFromDot("boxsTemperatureChangeEvent");
  
  /* Freed with the varbind */
  VarBind *var_boxServicesTempSensorIndex = MakeVarBindWithValue(oid_boxServicesTempSensorIndex, oid_instance, INTEGER_TYPE, &sensorNum);
  VarBind *var_boxsTemperatureChangeEvent = MakeVarBindWithValue(oid_boxsTemperatureChangeEvent, oid_instance, INTEGER_TYPE, &tempEventType);

  FreeOID(oid_instance);
  FreeOID(oid_boxServicesTempSensorIndex);
  FreeOID(oid_boxsTemperatureChangeEvent);

  if (snmpTrapOID == NULL || var_boxServicesTempSensorIndex == NULL || var_boxsTemperatureChangeEvent == NULL)
  {
    FreeOID(snmpTrapOID);

    if (var_boxServicesTempSensorIndex != NULL)
      FreeVarBind(var_boxServicesTempSensorIndex);
    if (var_boxsTemperatureChangeEvent != NULL)
      FreeVarBind(var_boxsTemperatureChangeEvent);
    return L7_FAILURE;
  }

  temp_vb = var_boxServicesTempSensorIndex;
  var_boxServicesTempSensorIndex->next_var = var_boxsTemperatureChangeEvent;
  var_boxsTemperatureChangeEvent->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}


L7_RC_t snmp_boxsFanStateChangeTrapSend( L7_uint32 fanNum, L7_uint32  fanEventType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("boxsFanStateChange");
  VarBind *temp_vb = NULL;
  SR_UINT32 instance_sid[1] = { 0 };

  OID *oid_instance = MakeOID(instance_sid, 1);
  OID *oid_boxServicesFansIndex = MakeOIDFromDot("boxServicesFansIndex");
  OID *oid_boxsItemStateChangeEvent = MakeOIDFromDot("boxsItemStateChangeEvent");

  /* Freed with the varbind */
  VarBind *var_boxServicesFansIndex = MakeVarBindWithValue(oid_boxServicesFansIndex, oid_instance, INTEGER_TYPE, &fanNum);
  VarBind *var_boxsItemStateChangeEven = MakeVarBindWithValue(oid_boxsItemStateChangeEvent, oid_instance, INTEGER_TYPE, &fanEventType);

  FreeOID(oid_instance);
  FreeOID(oid_boxServicesFansIndex);
  FreeOID(oid_boxsItemStateChangeEvent);

  if (snmpTrapOID == NULL || var_boxServicesFansIndex == NULL || var_boxsItemStateChangeEven == NULL)
  {
    FreeOID(snmpTrapOID);

    if (var_boxServicesFansIndex != NULL)
      FreeVarBind(var_boxServicesFansIndex);
    if (var_boxsItemStateChangeEven != NULL)
      FreeVarBind(var_boxsItemStateChangeEven);
    return L7_FAILURE;
  }

  temp_vb = var_boxServicesFansIndex;
  var_boxServicesFansIndex->next_var = var_boxsItemStateChangeEven;
  var_boxsItemStateChangeEven->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}


L7_RC_t snmp_boxsPowSupplyStateChangeTrapSend( L7_uint32 psmNum, L7_uint32  psmEventType)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("boxsPowSupplyStateChange");
  VarBind *temp_vb = NULL;
  SR_UINT32 instance_sid[1] = { 0 };

  OID *oid_instance = MakeOID(instance_sid, 1);
  OID *oid_boxServicesPowSupplyIndex = MakeOIDFromDot("boxServicesPowSupplyIndex");
  OID *oid_boxsItemStateChangeEvent = MakeOIDFromDot("boxsItemStateChangeEvent");

  /* Freed with the varbind */
  VarBind *var_boxServicesPowSupplyIndex = MakeVarBindWithValue(oid_boxServicesPowSupplyIndex, oid_instance, INTEGER_TYPE, &psmNum);
  VarBind *var_boxsItemStateChangeEvent = MakeVarBindWithValue(oid_boxsItemStateChangeEvent, oid_instance, INTEGER_TYPE, &psmEventType);

  FreeOID(oid_instance);
  FreeOID(oid_boxServicesPowSupplyIndex);
  FreeOID(oid_boxsItemStateChangeEvent);

  if (snmpTrapOID == NULL || var_boxServicesPowSupplyIndex == NULL || var_boxsItemStateChangeEvent == NULL)
  {
    FreeOID(snmpTrapOID);

    if (var_boxServicesPowSupplyIndex != NULL)
      FreeVarBind(var_boxServicesPowSupplyIndex);
    if (var_boxsItemStateChangeEvent != NULL)
      FreeVarBind(var_boxsItemStateChangeEvent);
    return L7_FAILURE;
  }

  temp_vb = var_boxServicesPowSupplyIndex;
  var_boxServicesPowSupplyIndex->next_var = var_boxsItemStateChangeEvent;
  var_boxsItemStateChangeEvent->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/* End Function Declarations*/

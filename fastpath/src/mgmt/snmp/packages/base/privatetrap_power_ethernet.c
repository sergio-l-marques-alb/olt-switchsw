/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: privatetrap_power_ethernet.c
*
* Purpose: Power Ethernet trap functions
*
* Created by: cpverne
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
#include "usmdb_util_api.h"

#include "privatetrap_power_ethernet.h"
#include "k_mib_power_ethernet_api.h"
        
/* Begin Function Declarations: privatetrap_power_ethernet.h */

/*********************************************************************
*
* @purpose  This Notification indicates if Pse Port is delivering or
*           not power to the PD. This Notification SHOULD be sent on
*           every status change except in the searching mode.
*           At least 500 msec must elapse between notifications
*           being emitted by the same object instance.
*
* @param    intIfNum      The Pse port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmp_pethPsePortOnOffNotificationTrapSend(L7_uint32 unitIndex, 
                                                  L7_uint32 intIfNum, 
                                                  L7_uint32 pethPsePortDetectionStatus)
{
  L7_uint32 status = 0;
  SR_UINT32 pethPsePortDetectionStatus_instance[2];
  OID *snmpTrapOID = MakeOIDFromDot("pethNotifications.1");
  VarBind *temp_vb = NULL;
  OID *oid_pethPsePortDetectionStatus;
  OID *oid_pethPsePortDetectionStatus_instance;
  VarBind *var_pethPsePortDetectionStatus;

  pethPsePortDetectionStatus_instance[0] = unitIndex;
  if (usmDbExtIfNumFromIntIfNum(intIfNum, &pethPsePortDetectionStatus_instance[1]) != L7_SUCCESS)
    return L7_FAILURE;

  oid_pethPsePortDetectionStatus = MakeOIDFromDot("pethPsePortDetectionStatus");
  oid_pethPsePortDetectionStatus_instance = MakeOID(pethPsePortDetectionStatus_instance, 2);
  
  var_pethPsePortDetectionStatus = MakeVarBindWithValue(oid_pethPsePortDetectionStatus, 
                                                                 oid_pethPsePortDetectionStatus_instance, 
                                                                 INTEGER_TYPE, &pethPsePortDetectionStatus);

  FreeOID(oid_pethPsePortDetectionStatus);
  FreeOID(oid_pethPsePortDetectionStatus_instance);

  if (snmpTrapOID == NULL || var_pethPsePortDetectionStatus == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_pethPsePortDetectionStatus);
    return L7_FAILURE;
  }

  temp_vb = var_pethPsePortDetectionStatus;
  var_pethPsePortDetectionStatus->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  This Notification indicate PSE Threshold usage
*           indication is on, the usage power is above the
*           threshold. At least 500 msec must elapse between
*           notifications being emitted by the same object
*           instance.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmp_pethMainPowerUsageOnNotificationTrapSend(L7_uint32 unitIndex, 
                                                      L7_uint32 pethMainPseConsumptionPower)
{
  L7_uint32 status = 0;
  OID *snmpTrapOID = MakeOIDFromDot("pethNotifications.2");
  VarBind *temp_vb = NULL;
  OID *oid_pethMainPseConsumptionPower;
  OID *oid_pethMainPseConsumptionPower_instance;
  VarBind *var_pethMainPseConsumptionPower;
  L7_uchar8 unitStr[4];

  oid_pethMainPseConsumptionPower = MakeOIDFromDot("pethMainPseConsumptionPower");
  sprintf(unitStr, "%u", unitIndex); 
  oid_pethMainPseConsumptionPower_instance = MakeOIDFromDot(unitStr);
  
  var_pethMainPseConsumptionPower = MakeVarBindWithValue(oid_pethMainPseConsumptionPower, 
                                                                  oid_pethMainPseConsumptionPower_instance, 
                                                                  INTEGER_TYPE, &pethMainPseConsumptionPower);

  FreeOID(oid_pethMainPseConsumptionPower);
  FreeOID(oid_pethMainPseConsumptionPower_instance);

  if (snmpTrapOID == NULL || var_pethMainPseConsumptionPower == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_pethMainPseConsumptionPower);
    return L7_FAILURE;
  }

  temp_vb = var_pethMainPseConsumptionPower;
  var_pethMainPseConsumptionPower->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  This Notification indicates PSE Threshold usage indication
*           off, the usage power is below the threshold.
*           At least 500 msec must elapse between notifications being
*           emitted by the same object instance.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmp_pethMainPowerUsageOffNotificationTrapSend(L7_uint32 unitIndex, 
                                                       L7_uint32 pethMainPseConsumptionPower)
{
  L7_uint32 status = 0;
  OID *snmpTrapOID = MakeOIDFromDot("pethNotifications.3");
  VarBind *temp_vb = NULL;
  OID *oid_pethMainPseConsumptionPower;
  OID *oid_pethMainPseConsumptionPower_instance;
  VarBind *var_pethMainPseConsumptionPower;
  L7_uchar8 unitStr[4];

  oid_pethMainPseConsumptionPower = MakeOIDFromDot("pethMainPseConsumptionPower");
  sprintf(unitStr, "%u", unitIndex); 
  oid_pethMainPseConsumptionPower_instance = MakeOIDFromDot(unitStr);
  
  var_pethMainPseConsumptionPower = MakeVarBindWithValue(oid_pethMainPseConsumptionPower, 
                                                                  oid_pethMainPseConsumptionPower_instance, 
                                                                  INTEGER_TYPE, &pethMainPseConsumptionPower);

  FreeOID(oid_pethMainPseConsumptionPower);
  FreeOID(oid_pethMainPseConsumptionPower_instance);

  if (snmpTrapOID == NULL || var_pethMainPseConsumptionPower == NULL)
  {
    FreeOID(snmpTrapOID);
    
    FreeVarBind(var_pethMainPseConsumptionPower);
    return L7_FAILURE;
  }

  temp_vb = var_pethMainPseConsumptionPower;
  var_pethMainPseConsumptionPower->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/* End Function Declarations*/

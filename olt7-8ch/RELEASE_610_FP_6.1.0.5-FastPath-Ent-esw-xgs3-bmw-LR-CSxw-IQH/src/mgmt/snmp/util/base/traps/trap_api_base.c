/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: trap_api_base.c
*
* Purpose: API functions to initiate sending of SNMP traps
*
* Created by: cpverne
*
* Component: SNMP
*
*********************************************************************/

#include "l7_common.h"
#include "dtlapi.h"
#include "osapi.h"
#include "snmp_api.h"
#include "trapmgr_exports.h"
#include "traputil_api.h"
#include "basetrap.h"
#include "privatetrap_power_ethernet.h"
#include "privatetrap_lldp.h"
#include "privatetrap_entity.h"
#include "snmp_trap_api_base.h"
#include "lldp_api.h"
#include "string.h"
        
/* Begin Function Declarations: snmp_trap_api_base.h */

/*********************************************************************
*
* @purpose  Receives a trap request for base traps.
*
* @param    L7_TRAP_t  trap  trap request
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t BaseTrapSend(L7_TRAP_t *trap)
{
  switch (trap->trapId)
  {
  case TRAP_EDB_CONFIG_CHANGE:
    return snmp_entConfigChangeTrapSend();
    break;

  case TRAP_LINK_DOWN:
    return snmp_LinkDownTrapSend(trap->u.linkStatus.intIfNum);
    break;

  case TRAP_LINK_UP:
    return snmp_LinkUpTrapSend(trap->u.linkStatus.intIfNum);
    break;

  default:
    /* unknown trap*/
    return L7_ERROR;
    break;
  }

  return L7_ERROR;
}


void SnmpTrapRegistrationBase(void)
{
  trapMgrFunctionList_t trapMgrFunctionList;

  trapMgrFunctionList.sendTrap = BaseTrapSend;

  trapMgrFunctionList.registrar_ID = TRAP_EDB_CONFIG_CHANGE;
  trapMgrRegister(&trapMgrFunctionList);

  trapMgrFunctionList.registrar_ID = TRAP_LINK_DOWN;
  trapMgrRegister(&trapMgrFunctionList);

  trapMgrFunctionList.registrar_ID = TRAP_LINK_UP;
  trapMgrRegister(&trapMgrFunctionList);
}


void SnmpTrapSendCallback_base(L7_uint32 trap_id, void *data);
/*********************************************************************/
typedef enum
{
  L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected=0,
  L7_SNMP_TRAP_BASE_pethMainPowerUsageOffNotification,
  L7_SNMP_TRAP_BASE_pethMainPowerUsageOnNotification,
  L7_SNMP_TRAP_BASE_pethPsePortOnOffNotification
} L7_SNMP_TRAP_TYPE_BASE_t;

typedef struct
{
  L7_uint32 pethPsePortGroupIndex;
  L7_uint32 pethPsePort;
  L7_uint32 pethPsePortDetectionStatus;
} L7_SNMP_TRAP_BASE_pethPsePort_t;

typedef struct
{
  L7_uint32 pethMainPseGroupIndex;
  L7_uint32 pethMainPseConsumptionPower;
} L7_SNMP_TRAP_BASE_pethMainPseGroupIndex_t;

/*********************************************************************
*
* @purpose  This Notification indicates if Pse Port is delivering or
*           not power to the PD. This Notification SHOULD be sent on
*           every status change except in the searching mode.
*           At least 500 msec must elapse between notifications
*           being emitted by the same object instance.
*
* @param    unitIndex     The Pse Group Index
* @param    intIfNum      The Pse port
* @param    detection     The Pse port detection status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t SnmpPethPsePortOnOffNotificationTrapSend(L7_uint32 unitIndex,
                                                 L7_uint32 intIfNum,
                                                 L7_uint32 detection)
{
  L7_SNMP_TRAP_BASE_pethPsePort_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, 
                     sizeof(L7_SNMP_TRAP_BASE_pethPsePort_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->pethPsePort = intIfNum;
  trap->pethPsePortGroupIndex = unitIndex;
  trap->pethPsePortDetectionStatus = detection;

  return snmpTrapSend(L7_SNMP_TRAP_BASE_pethPsePortOnOffNotification, 
                      (void*)trap, &SnmpTrapSendCallback_base);
}

/*********************************************************************
*
* @purpose  This Notification indicate PSE Threshold usage
*           indication is on, the usage power is above the
*           threshold. At least 500 msec must elapse between
*           notifications being emitted by the same object
*           instance.
*
* @param    unitIndex     The Pse Group Index
* @param    allocPower    The Power being delivered by the pse
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t SnmpPethMainPowerUsageOnNotificationTrapSend(L7_uint32 unitIndex, 
                                                     L7_uint32 allocPower)
{
  L7_SNMP_TRAP_BASE_pethMainPseGroupIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID,
                     sizeof(L7_SNMP_TRAP_BASE_pethMainPseGroupIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->pethMainPseGroupIndex = unitIndex;
  trap->pethMainPseConsumptionPower = allocPower;

  return snmpTrapSend(L7_SNMP_TRAP_BASE_pethMainPowerUsageOnNotification, 
                      (void *)trap, &SnmpTrapSendCallback_base);
}

/*********************************************************************
*
* @purpose  This Notification indicates PSE Threshold usage indication
*           off, the usage power is below the threshold.
*           At least 500 msec must elapse between notifications being
*           emitted by the same object instance.
*
* @param    unitIndex     The Pse Group Index
* @param    allocPower    The Power being delivered by the pse
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t SnmpPethMainPowerUsageOffNotificationTrapSend(L7_uint32 unitIndex,
                                                     L7_uint32 allocPower)
{
  L7_SNMP_TRAP_BASE_pethMainPseGroupIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID,
                     sizeof(L7_SNMP_TRAP_BASE_pethMainPseGroupIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->pethMainPseGroupIndex = unitIndex;
  trap->pethMainPseConsumptionPower = allocPower;

  return snmpTrapSend(L7_SNMP_TRAP_BASE_pethMainPowerUsageOffNotification,
                      (void *)trap, &SnmpTrapSendCallback_base);
}

/* End Function Declarations*/

typedef struct
{
  L7_uint32 lldpRemChassisIdSubtype;
  L7_char8 lldpRemChassisId[LLDP_MGMT_STRING_SIZE_MAX];
  L7_uint32 lldpRemChassisId_len;
  L7_uint32 lldpXMedRemDeviceClass;
} L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected_t;


/*********************************************************************
*
* @purpose  Handles calling the trap code to send traps
*
* @param    trap_id   enumeration of the trap to send
* @param    data      data structure holding trap information
*
* @returns  void
*
* @notes    The datastructure is determined by the trap_id indicated.
*
* @end
*********************************************************************/
void SnmpTrapSendCallback_base(L7_uint32 trap_id, void *data)
{
  switch (trap_id)
  {
  case L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected:
    (void)snmp_lldpXMedTopologyChangeDetectedTrapSend(((L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected_t*)data)->lldpRemChassisIdSubtype,
                                                      ((L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected_t*)data)->lldpRemChassisId,
                                                      ((L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected_t*)data)->lldpRemChassisId_len,
                                                      ((L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected_t*)data)->lldpXMedRemDeviceClass);
    break;
 case L7_SNMP_TRAP_BASE_pethMainPowerUsageOffNotification: 
    (void)snmp_pethMainPowerUsageOffNotificationTrapSend(((L7_SNMP_TRAP_BASE_pethMainPseGroupIndex_t *)data)->pethMainPseGroupIndex, ((L7_SNMP_TRAP_BASE_pethMainPseGroupIndex_t *)data)->pethMainPseConsumptionPower);
    break;
 case L7_SNMP_TRAP_BASE_pethMainPowerUsageOnNotification:
    (void)snmp_pethMainPowerUsageOnNotificationTrapSend(((L7_SNMP_TRAP_BASE_pethMainPseGroupIndex_t *)data)->pethMainPseGroupIndex, ((L7_SNMP_TRAP_BASE_pethMainPseGroupIndex_t *)data)->pethMainPseConsumptionPower);
    break;
 case L7_SNMP_TRAP_BASE_pethPsePortOnOffNotification:
    (void)snmp_pethPsePortOnOffNotificationTrapSend(((L7_SNMP_TRAP_BASE_pethPsePort_t *)data)->pethPsePortGroupIndex, 
                                                    ((L7_SNMP_TRAP_BASE_pethPsePort_t *)data)->pethPsePort,
                                                    ((L7_SNMP_TRAP_BASE_pethPsePort_t *)data)->pethPsePortDetectionStatus);
    break;
  default:
    break;
  }

  return;
}
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
L7_RC_t SnmpLldpXMedTopologyChangeDetectedTrapSend(L7_uint32 lldpRemChassisIdSubtype,
                                                   L7_uchar8 *lldpRemChassisId,
                                                   L7_uint32 lldpRemChassisId_len,
                                                   L7_uint32 lldpXMedRemDeviceClass)
{
  L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->lldpRemChassisIdSubtype = lldpRemChassisIdSubtype;
  memcpy(trap->lldpRemChassisId, lldpRemChassisId, lldpRemChassisId_len);
  trap->lldpRemChassisId_len = lldpRemChassisId_len;
  trap->lldpXMedRemDeviceClass = lldpXMedRemDeviceClass;

  return snmpTrapSend(L7_SNMP_TRAP_BASE_lldpXMedTopologyChangeDetected, (void*)trap, &SnmpTrapSendCallback_base);
}


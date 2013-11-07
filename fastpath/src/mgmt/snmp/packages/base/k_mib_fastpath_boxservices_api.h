/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename k_mib_fastpath_boxservices_api.h 
*
* @purpose  Provide API functions for box services component objects
*
* @component SNMP
*
* @comments
*
* @create 22/02/2008
*
* @author vkozlov
* @end
*
**********************************************************************/

#ifndef __K_MIB_FASTPATH_BOXSERVICES_API__
#define __K_MIB_FASTPATH_BOXSERVICES_API__



L7_RC_t snmpBoxsFanIndexNextGet(L7_uint32 UnitIndex, L7_uint32 *index);
L7_RC_t snmpBoxsFanIndexGet(L7_uint32 UnitIndex, L7_uint32 index);
L7_RC_t snmpBoxsFanTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *fanType);
L7_RC_t snmpBoxsFanStateGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *fanState);
L7_RC_t snmpBoxsFanSpeedGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *fanSpeed);
L7_RC_t snmpBoxsFanDutyGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *fanDuty);
L7_RC_t snmpBoxsPowerSupplyIndexNextGet(L7_uint32 UnitIndex, L7_uint32 *index);
L7_RC_t snmpBoxsPowerSupplyIndexGet(L7_uint32 UnitIndex, L7_uint32 index);
L7_RC_t snmpBoxsPowerSupplyTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *powSupplyType);
L7_RC_t snmpBoxsPowerSupplyStateGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *powSupplyState);
L7_RC_t snmpBoxsTempSensorIndexNextGet(L7_uint32 UnitIndex, L7_uint32 *index);
L7_RC_t snmpBoxsTempSensorIndexGet(L7_uint32 UnitIndex, L7_uint32 index);
L7_RC_t snmpBoxsTempSensorTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *tempSensorType);
L7_RC_t snmpBoxsTempSensorStateGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *tempSensorState);
L7_RC_t snmpBoxsTempSensorTemperatureGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *tempSensorTemperature);

L7_RC_t snmpBoxsPowSupplyStatusTrapEnableSet(L7_uint32 UnitIndex, L7_uint32 enable);
L7_RC_t snmpBoxsTempStatusTrapEnableSet(L7_uint32 UnitIndex, L7_uint32 enable);
L7_RC_t snmpBoxsFanStatusTrapEnableSet(L7_uint32 UnitIndex, L7_uint32 enable);

#endif /* __K_MIB_FASTPATH_BOXSERVICES_API__ */

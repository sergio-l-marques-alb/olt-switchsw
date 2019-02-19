/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmp_vct_api.h
*
* Purpose: API interface for SNMP Cable Test
*
* Created by: Colin Verne 09/27/2002
*
* Component: SNMP
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/



#ifndef SNMP_CT_API_H
#define SNMP_CT_API_H

#include "l7_common.h"

typedef enum
{
  SNMP_CT_STATUS_ACTIVE = 1,
  SNMP_CT_STATUS_SUCCESS,
  SNMP_CT_STATUS_FAILURE,
  SNMP_CT_STATUS_UNINITIALIZED
} snmpCtStatus_t;

typedef enum
{
  SNMP_CT_CABLE_STATUS_NORMAL = 1,
  SNMP_CT_CABLE_STATUS_OPEN,
  SNMP_CT_CABLE_STATUS_SHORT,
  SNMP_CT_CABLE_STATUS_UNKNOWN
} snmpCtCableStatus_t;

typedef struct
{
  snmpCtStatus_t ctStatus;
  L7_uint32 ctPortIndex;
  snmpCtCableStatus_t ctCableStatus;
  L7_uint32 ctMinLength;
  L7_uint32 ctMaxLength;
  L7_uint32 ctFailLocation;
} snmpCtCfg_t;

#define SNMP_CT_PORTINDEX_DEFAULT 0
#define SNMP_CT_MINLENGTH_DEFAULT 0
#define SNMP_CT_MAXLENGTH_DEFAULT 0
#define SNMP_CT_FAILLOCATION_DEFAULT 0

/*********************************************************************
*
* @purpose  Returns the current status of the SNMP Cable Test Task
*
* @param    ctStatus   snmpCtStatus_t*
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCableTesterStatusGet(snmpCtStatus_t *ctStatus);

/*********************************************************************
*
* @purpose  Returns the currently configured interface to test
*
* @param    ctPortIndex   L7_uint32*
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCableTesterPortIndexGet(L7_uint32 *ctPortIndex);

/*********************************************************************
*
* @purpose  Returns the status of the cable tested
*
* @param    ctCableStatus   snmpCtCableStatus_t*
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCableTesterCableStatusGet(snmpCtCableStatus_t *ctCableStatus);

/*********************************************************************
*
* @purpose  Returns the minimum estimated lengh of the cable test
*
* @param    ctMinLength   L7_uint32*
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCableTesterMinLengthGet(L7_uint32 *ctMinLength);

/*********************************************************************
*
* @purpose  Returns the maximum estimated length of the cable tested
*
* @param    ctMaxLength   L7_uint32*
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCableTesterMaxLengthGet(L7_uint32 *ctMaxLength);

/*********************************************************************
*
* @purpose  Returns the estimated failure location in meters from the
*           end of the cable
*
* @param    ctFailLocation   *L7_uint32
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCableTesterFailLocationGet(L7_uint32 *ctFailLocation);

/*********************************************************************
*
* @purpose  Sets the status of the SNMP Cable Tester task
*
* @param    ctStatus   snmpCtStatus_t*
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCableTesterStatusSet(snmpCtStatus_t ctStatus);

/*********************************************************************
*
* @purpose  Sets the configured port to test
*
* @param    ctPortIndex   L7_uint32
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCableTesterPortIndexSet(L7_uint32 ctPortIndex);

/* End Function Prototypes */
#endif /* SNMP_CT_API_H */






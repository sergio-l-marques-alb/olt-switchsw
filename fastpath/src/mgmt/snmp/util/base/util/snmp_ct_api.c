/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmp_ct_api.c
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



#include "l7_common.h"
#include "usmdb_nim_api.h"
#include "snmptask_ct.h"
#include "snmp_ct_api.h"


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
SnmpCableTesterStatusGet(snmpCtStatus_t *ctStatus)
{
  if (snmpCtReady == L7_TRUE)
  {
    *ctStatus = snmpCtCfg.ctStatus;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

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
SnmpCableTesterPortIndexGet(L7_uint32 *ctPortIndex)
{
  if (snmpCtReady == L7_TRUE)
  {
    *ctPortIndex = snmpCtCfg.ctPortIndex;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

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
SnmpCableTesterCableStatusGet(snmpCtCableStatus_t *ctCableStatus)
{
  if (snmpCtReady == L7_TRUE)
  {
    *ctCableStatus = snmpCtCfg.ctCableStatus;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

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
SnmpCableTesterMinLengthGet(L7_uint32 *ctMinLength)
{
  if (snmpCtReady == L7_TRUE)
  {
    *ctMinLength = snmpCtCfg.ctMinLength;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

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
SnmpCableTesterMaxLengthGet(L7_uint32 *ctMaxLength)
{
  if (snmpCtReady == L7_TRUE)
  {
    *ctMaxLength = snmpCtCfg.ctMaxLength;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

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
SnmpCableTesterFailLocationGet(L7_uint32 *ctFailLocation)
{
  if (snmpCtReady == L7_TRUE)
  {
    *ctFailLocation = snmpCtCfg.ctFailLocation;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Sets the status of the SNMP CT task
*
* @param    ctStatus   snmpCtStatus_t
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCableTesterStatusSet(snmpCtStatus_t ctStatus)
{
  L7_uint32 connType;
  L7_RC_t rc = L7_FAILURE;

  if (( snmpCtReady == L7_TRUE ) &&
      ( snmpCtCfg.ctStatus != SNMP_CT_STATUS_ACTIVE ) &&
      ( ctStatus == SNMP_CT_STATUS_ACTIVE ) &&
      ( usmDbIntfConnectorTypeGet(snmpCtCfg.ctPortIndex, &connType) == L7_SUCCESS) &&
      ( connType == L7_RJ45 ))
  {
    snmpCtCfg.ctStatus = ctStatus;
    rc = L7_SUCCESS;
  }

  return rc;
}

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
SnmpCableTesterPortIndexSet(L7_uint32 ctPortIndex)
{
  L7_uint32 connType;
  L7_RC_t rc = L7_FAILURE;

  if (( snmpCtReady == L7_TRUE ) &&
      ( snmpCtCfg.ctStatus != SNMP_CT_STATUS_ACTIVE ) &&
      ( usmDbIntfConnectorTypeGet(ctPortIndex, &connType) == L7_SUCCESS ) &&
      ( connType == L7_RJ45 ) )
  {
    snmpCtCfg.ctPortIndex = ctPortIndex;
    rc = L7_SUCCESS;
  }

  return rc;
}


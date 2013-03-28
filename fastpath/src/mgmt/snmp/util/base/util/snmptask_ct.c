/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: snmptask.c
*
* Purpose: API interface for starting the SNMP Agent task
* 	
*
* Created by: Colin Verne 07/05/2000
*
* Component: SNMP
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "usmdb_status.h"
#include "snmptask_ct.h"
#include "snmp_ct_api.h"

#include "snmp_api.h"

snmpCtCfg_t snmpCtCfg;
L7_BOOL snmpCtReady = L7_FALSE;

/*********************************************************************
*
* @purpose  Starts SNMP Cable Test task function
*
* @notes    none 
*
* @end
*********************************************************************/
void 
L7_snmp_ct_task ()
{
  L7_RC_t rc;
  L7_uint32 cableStatus;
  L7_BOOL lengthKnown;
  L7_uint32 shortestLength;
  L7_uint32 longestLength;
  L7_uint32 cableFailureLength;
  L7_clocktime timestamp;

  /* initialize configuration */

  snmpCtCfg.ctStatus = SNMP_CT_STATUS_UNINITIALIZED;
  snmpCtCfg.ctPortIndex = SNMP_CT_PORTINDEX_DEFAULT;
  snmpCtCfg.ctCableStatus = SNMP_CT_CABLE_STATUS_UNKNOWN;
  snmpCtCfg.ctMinLength = SNMP_CT_MINLENGTH_DEFAULT;
  snmpCtCfg.ctMaxLength = SNMP_CT_MAXLENGTH_DEFAULT;
  snmpCtCfg.ctFailLocation = SNMP_CT_FAILLOCATION_DEFAULT;

  SnmpWaitUntilRun();

  /* check to see that the Cable Test component exists first */
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_CABLE_TEST_COMPONENT_ID) != L7_TRUE)
    return;

  /* allow access to CT information */
  snmpCtReady = L7_TRUE;

  while (snmpCtReady == L7_TRUE)
  {
    if (snmpCtCfg.ctStatus == SNMP_CT_STATUS_ACTIVE &&
        snmpCtCfg.ctPortIndex != 0)
    {
      rc = usmDbStatusCableGet(snmpCtCfg.ctPortIndex, &cableStatus, &lengthKnown, &shortestLength, 
                               &longestLength, &cableFailureLength, &timestamp );

      if (rc == L7_SUCCESS && cableStatus != L7_CABLE_TEST_FAIL)
      {
        snmpCtCfg.ctStatus = SNMP_CT_STATUS_SUCCESS;

        switch (cableStatus)
        {
        case L7_CABLE_NORMAL:
          snmpCtCfg.ctCableStatus = SNMP_CT_CABLE_STATUS_NORMAL;
          break;

        case L7_CABLE_OPEN:
          snmpCtCfg.ctCableStatus = SNMP_CT_CABLE_STATUS_OPEN;
          break;

        case L7_CABLE_SHORT:
          snmpCtCfg.ctCableStatus = SNMP_CT_CABLE_STATUS_SHORT;
          break;

        default:
          snmpCtCfg.ctCableStatus = SNMP_CT_CABLE_STATUS_UNKNOWN;
          break;
        }

        if (lengthKnown == L7_TRUE)
        {
          snmpCtCfg.ctMinLength = shortestLength;
          snmpCtCfg.ctMaxLength = longestLength;
        }
        else
        {
          snmpCtCfg.ctMinLength = SNMP_CT_MINLENGTH_DEFAULT;
          snmpCtCfg.ctMaxLength = SNMP_CT_MAXLENGTH_DEFAULT;
        }

        snmpCtCfg.ctFailLocation = cableFailureLength;
      }
      else
      {
        snmpCtCfg.ctStatus = SNMP_CT_STATUS_FAILURE;
        snmpCtCfg.ctCableStatus = SNMP_CT_CABLE_STATUS_UNKNOWN;
        snmpCtCfg.ctMinLength = SNMP_CT_MINLENGTH_DEFAULT;
        snmpCtCfg.ctMaxLength = SNMP_CT_MAXLENGTH_DEFAULT;
        snmpCtCfg.ctFailLocation = SNMP_CT_FAILLOCATION_DEFAULT;
      }
    }

    osapiSleep(1);
  }
}

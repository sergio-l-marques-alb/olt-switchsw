
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_bridge_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @comments This file is included at the top of the k_mib_bridge.c and
* @comments k_mib_vlan.c files.
*
* @create 04/09/2001
*
* @author cpverne
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "snmpapi.h"
#include "osapi_support.h"
#include "usmdb_dot1d_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"

/* lvl7_@p1299 start */
L7_RC_t
snmpDot1dBasePortEntryGet ( L7_uint32 UnitIndex, L7_uint32 dot1dBasePort)
{
  L7_RC_t rc = L7_FAILURE;
  L7_INTF_TYPES_t sysIntfType;

  /* interfaces must be valid and visible */
  rc = usmDbDot1dBasePortEntryGet(UnitIndex, dot1dBasePort);

  if ((rc == L7_SUCCESS) && (usmDbVisibleIntIfNumberCheck(UnitIndex, dot1dBasePort) == L7_SUCCESS) &&
      (usmDbIntfTypeGet(dot1dBasePort, &sysIntfType) == L7_SUCCESS) && (sysIntfType != L7_CPU_INTF))
  {
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpDot1dBasePortEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *dot1dBasePort)
{
  /* cycle through valid internal interfaces */
  while (usmDbGetNextVisibleIntIfNumber(*dot1dBasePort, dot1dBasePort) == L7_SUCCESS)
  {
    /* only return success if it's a valid interface for this table */
    if (usmDbDot1dIsValidIntf(UnitIndex, *dot1dBasePort) == L7_TRUE &&
        snmpDot1dBasePortEntryGet(UnitIndex, *dot1dBasePort) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }

  /* if no more valid interfaces exist, return failure */
  return L7_FAILURE;
}
/* lvl7_@p1299 end */

L7_RC_t
snmpDot1dBaseTypeGet ( L7_uint32 UnitIndex, L7_int32 *Type )
{                       
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbDot1dBaseTypeGet ( UnitIndex, &temp_val );

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case 1:
        *Type = D_dot1dBaseType_unknown;
        break;
      
      case 2:
        *Type = D_dot1dBaseType_transparent_only;
        break;
      
      case 3:
        *Type = D_dot1dBaseType_sourceroute_only;
        break;
      
      case 4:
        *Type = D_dot1dBaseType_srt;
        break;
      
      default:
        *Type = 0;
        rc = L7_FAILURE;
      }
    }

    return rc;
}

L7_RC_t
snmpDot1dStpPortEnableGet ( L7_uint32 UnitIndex, L7_uint32 Port, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbIfAdminStateGet ( UnitIndex, Port, &temp_val );

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_ENABLE:
        *val = D_dot1dStpPortEnable_enabled;
        break;
      case L7_DISABLE:
        *val = D_dot1dStpPortEnable_disabled;
        break;
      default:
        *val = 0;
        rc = L7_FAILURE;
      }
    }

    return rc;
}
/*#define usmDbDot1dStpPortEnableGet              snmpDot1dStpPortEnableGet*/

L7_RC_t
snmpDot1dStpPortEnableSet ( L7_uint32 UnitIndex, L7_uint32 Port, L7_int32 val )
{
    L7_int32 temp_val = 0;
    L7_RC_t rc;

    rc = L7_SUCCESS;

    switch (val)
    {
    case D_dot1dStpPortEnable_enabled:
      temp_val = L7_ENABLE;
      break;
    case D_dot1dStpPortEnable_disabled:
      temp_val = L7_DISABLE;
      break;
    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbIfAdminStateSet ( UnitIndex, Port, temp_val );
    }

    return rc;
}
/*#define usmDbDot1dStpPortEnableSet              snmpDot1dStpPortEnableSet*/

L7_RC_t
snmpDot1dTpFdbPortGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbDot1dTpFdbPortGet ( UnitIndex, Address, &temp_val );

    if (rc == L7_SUCCESS)
    {
      rc = usmDbExtIfNumFromIntIfNum(temp_val, val);
    }

    return rc;
}
/*#define usmDbDot1dTpFdbPortGet                snmpDot1dTpFdbPortGet*/

/*
  "STATIC",
  "LEARNED",
  "MGMT",
  "GMRP_LEARNED",
  "OTHER
  */
L7_RC_t
snmpDot1dTpFdbStatusGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbDot1dTpFdbStatusGet ( UnitIndex, Address, &temp_val );

    if (rc == L7_SUCCESS)
    {
      switch(temp_val)
      {
      case 1:
        *val = D_dot1dTpFdbStatus_learned;
        break;

      case 2:
        *val = D_dot1dTpFdbStatus_mgmt;
        break;

      case 4:
        *val = D_dot1dTpFdbStatus_other;
        break;

      default:
        *val = D_dot1dTpFdbStatus_invalid;
      }
    }

    return rc;
}
/*#define usmDbDot1dTpFdbStatusGet                snmpDot1dTpFdbStatusGet*/

L7_RC_t
snmpDot1dStaticStatusGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbDot1dStaticStatusGet ( UnitIndex, Address, Port, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}
/*#define usmDbDot1dStaticStatusGet               snmpDot1dStaticStatusGet*/


/* lvl7_@p1656 end */




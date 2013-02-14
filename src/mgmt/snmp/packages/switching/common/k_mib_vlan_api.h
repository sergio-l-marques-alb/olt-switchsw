/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_vlan_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @comments This file is included at the top of the k_mib_vlan.c
*
* @create 04/09/2001
*
* @author cpverne
*
* @end
*             
**********************************************************************/

#include "dot1q_exports.h"
#include "fdb_exports.h"
#include "usmdb_common.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_garp.h"
#include "usmdb_mib_vlan_api.h"


/*************************************************************
                    
*************************************************************/


L7_RC_t
snmpDot1dPortGmrpStatusGet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbGarpGmrpPortEnableGet(UnitIndex, intIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_dot1dPortGmrpStatus_enabled;
      break;
    case L7_DISABLE:
      *val = D_dot1dPortGmrpStatus_disabled;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpDot1dPortGmrpStatusSet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_dot1dPortGmrpStatus_enabled:
    temp_val = L7_ENABLE;
    break;
  case D_dot1dPortGmrpStatus_disabled:
    temp_val = L7_DISABLE;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbGarpGmrpPortEnableSet(UnitIndex, intIfIndex, temp_val);
  }
  return rc;
}

/* lvl7_@p0802 start */
L7_RC_t
snmpDot1qPortAcceptableFrameTypesGet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbQportsAcceptFrameTypeGet(UnitIndex, intIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1Q_ADMIT_ALL:
      *val = D_dot1qPortAcceptableFrameTypes_admitAll;
      break;

    case L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED:
      *val = D_dot1qPortAcceptableFrameTypes_admitOnlyVlanTagged;
      break;

    case L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED:      /*this option is not supported in the standard mib */ 
      *val = D_dot1qPortAcceptableFrameTypes_admitAll;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpDot1qPortAcceptableFrameTypesSet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val)
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_dot1qPortAcceptableFrameTypes_admitAll:
    temp_val = L7_DOT1Q_ADMIT_ALL;
    break;

  case D_dot1qPortAcceptableFrameTypes_admitOnlyVlanTagged:
    temp_val = L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED;
    break;

  default:
    temp_val = 0;
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQportsAcceptFrameTypeSet(UnitIndex, intIfIndex, temp_val);
  }
  return rc;
}
/* lvl7_@p0802 end */


/* lvl7_@p0803 start */
L7_RC_t
snmpDot1qPortIngressFilteringGet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbQportsEnableIngressFilteringGet(UnitIndex, intIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_dot1qPortIngressFiltering_true;
      break;

    case L7_DISABLE:
      *val = D_dot1qPortIngressFiltering_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpDot1qPortIngressFilteringSet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val)
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_dot1qPortIngressFiltering_true:
    temp_val = L7_ENABLE;
    break;

  case D_dot1qPortIngressFiltering_false:
    temp_val = L7_DISABLE;
    break;

  default:
    temp_val = 0;
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQportsEnableIngressFilteringSet(UnitIndex, intIfIndex, temp_val);
  }
  return rc;
}
/* lvl7_@p0803 end */

/* lvl7_@p0804 start */
L7_RC_t
snmpDot1qPortGvrpStatusGet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbQportsEnableGVRPGet(UnitIndex, intIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_dot1qPortGvrpStatus_enabled;
      break;

    case L7_DISABLE:
      *val = D_dot1qPortGvrpStatus_disabled;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpDot1qPortGvrpStatusSet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val)
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_dot1qPortGvrpStatus_enabled:
    temp_val = L7_ENABLE;
    break;

  case D_dot1qPortGvrpStatus_disabled:
    temp_val = L7_DISABLE;
    break;

  default:
    temp_val = 0;
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQportsEnableGVRPSet(UnitIndex, intIfIndex, temp_val);
  }
  return rc;
}
/* lvl7_@p0804 start */

L7_RC_t
snmpDot1dTrafficClassesEnabledGet ( L7_uint32 UnitIndex, L7_int32 *val)
{
  L7_BOOL temp_val;
  L7_RC_t rc;

  rc = usmDbDot1dTrafficClassesEnabledGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_dot1dTrafficClassesEnabled_true;
      break;

    case L7_FALSE:
      *val = D_dot1dTrafficClassesEnabled_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpDot1dGmrpStatusGet ( L7_uint32 UnitIndex, L7_int32 *val)
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = usmDbGarpGmrpEnableGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_dot1dGmrpStatus_enabled;
      break;

    case L7_DISABLE:
      *val = D_dot1dGmrpStatus_disabled;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpDot1dTrafficClassesEnabledSet ( L7_uint32 UnitIndex, L7_int32 val)
{
  L7_BOOL temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_dot1dTrafficClassesEnabled_true:
    temp_val = L7_TRUE;
    break;

  case D_dot1dTrafficClassesEnabled_false:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot1dTrafficClassesEnabledSet(UnitIndex, temp_val);
  }

  return rc;
}


L7_RC_t
snmpDot1dGmrpStatusSet ( L7_uint32 UnitIndex, L7_int32 val)
{
  L7_int32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_dot1dGmrpStatus_enabled:
    temp_val = L7_ENABLE;
    break;

  case D_dot1dGmrpStatus_disabled:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbGarpGmrpEnableSet(UnitIndex, temp_val);
  }
  return rc;
}


L7_RC_t
snmpDot1qVlanVersionNumberGet (L7_uint32 UnitIndex, L7_int32 *val)
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1qVlanVersionNumberGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case 1:
      *val = D_dot1qVlanVersionNumber_version1;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1qGvrpStatusGet (L7_uint32 UnitIndex, L7_int32 *val)
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1qGvrpStatusGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_dot1qGvrpStatus_enabled;
      break;

    case L7_DISABLE:
      *val = D_dot1qGvrpStatus_disabled;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}
L7_RC_t
snmpDot1qGvrpStatusSet ( L7_uint32 UnitIndex, L7_int32 val)
{
  L7_int32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_dot1qGvrpStatus_enabled:
    temp_val = L7_ENABLE;
    break;

  case D_dot1qGvrpStatus_disabled:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot1qGvrpStatusSet(UnitIndex, temp_val);
  }
  return rc;
}

L7_RC_t 
snmpDot1qTpFdbStatusGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress, L7_int32 *val )
{

  L7_int32 temp_val;
  L7_RC_t rc;
  
  rc = usmDbDot1qTpFdbStatusGet ( UnitIndex, FdbId, FdbAddress, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_FDB_ADDR_FLAG_LEARNED:
    case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
      *val = D_dot1qTpFdbStatus_learned;
      break;
    case L7_FDB_ADDR_FLAG_MANAGEMENT:
    case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
      *val = D_dot1qTpFdbStatus_mgmt;
      break;
    case L7_FDB_ADDR_FLAG_STATIC:
      *val = D_dot1qTpFdbStatus_other;
      break;
    case L7_FDB_ADDR_FLAG_SELF:
      *val = D_dot1qTpFdbStatus_self;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t 
snmpDot1qStaticUnicastStatusGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 *val )
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1qStaticUnicastStatusGet ( UnitIndex, FdbId, Address, ReceivePort, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case 1:
      *val = D_dot1qStaticUnicastStatus_deleteOnReset;
      break;
    case 2:
      *val = D_dot1qStaticUnicastStatus_deleteOnTimeout;
      break;
    case 3:
      *val = D_dot1qStaticUnicastStatus_invalid;
      break;
    case 4:
      *val = D_dot1qStaticUnicastStatus_other;
      break;
    case 5:
      *val = D_dot1qStaticUnicastStatus_permanent;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t 
snmpDot1qStaticMulticastStatusGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 *val )
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1qStaticMulticastStatusGet ( UnitIndex, VlanIndex, Address, ReceivePort, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case 1:
      *val = D_dot1qStaticMulticastStatus_deleteOnReset;
      break;
    case 2:
      *val = D_dot1qStaticMulticastStatus_deleteOnTimeout;
      break;
    case 3:
      *val = D_dot1qStaticMulticastStatus_invalid;
      break;
    case 4:
      *val = D_dot1qStaticMulticastStatus_other;
      break;
    case 5:
      *val = D_dot1qStaticMulticastStatus_permanent;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1qVlanStatusGet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_int32 *val)
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1qVlanStatus(UnitIndex, TimeMark, VlanIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case 1:
      *val = D_dot1qVlanStatus_other;
      break;

    case 2:
      *val = D_dot1qVlanStatus_permanent;
      break;

    case 3:
      *val = D_dot1qVlanStatus_dynamicGvrp;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpDot1qVlanCreationTimeGet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_uint32 *val)
{
  usmDbTimeSpec_t ts;
  L7_RC_t rc;

  rc = usmDbDot1qVlanCreationTime( UnitIndex, TimeMark, VlanIndex, &ts );

  if (rc == L7_SUCCESS)
  {
    *val = ts.seconds;
    *val += ts.minutes * 60;
    *val += ts.hours * 60 * 60;
    *val += ts.days * 60 * 60 * 24;
    *val = *val * 100;
  }
  return rc;
}


L7_RC_t 
snmpDot1qConstraintTypeGet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set, L7_int32 *val )
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1qConstraintTypeGet ( UnitIndex, Vlan, Set, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case 1:
      *val = D_dot1qConstraintType_independent;
      break;

    case 2:
      *val = D_dot1qConstraintType_shared;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpDot1qConstraintTypeDefaultGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1qConstraintTypeDefaultGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case 1:
      *val = D_dot1qConstraintTypeDefault_independent;
      break;

    case 2:
      *val = D_dot1qConstraintTypeDefault_shared;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/* lvl7_@p0837 start */
L7_RC_t
snmpDot1qVlanStaticRowStatusSet ( L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_dot1qVlanStaticRowStatus_active:
  case D_dot1qVlanStaticRowStatus_createAndGo:
    break;

  case D_dot1qVlanStaticRowStatus_destroy:
    rc = usmDbVlanDelete(UnitIndex, vlanIndex);
    break;

  case D_dot1qVlanStaticRowStatus_createAndWait:
    rc = usmDbVlanDelete(UnitIndex, vlanIndex);
    break;
  default:
    rc = L7_FAILURE;
  }

  return rc;
}
/* lvl7_@p0837 end */

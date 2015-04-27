/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_fastpathmulticast_api.h
*
* @purpose Wrapper functions for FASTPATH Multicast Private MIB
*
* @component SNMP
*
* @comments
*
* @create
*
* @author cpverne
*
* @end
*
**********************************************************************/

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_igmp_api.h"
#include "usmdb_mib_dvmrp_api.h"
#include "usmdb_mib_mcast_api.h"
#include "usmdb_mib_pim_api.h"
#include "usmdb_pimsm_api.h"
#endif




L7_RC_t
snmpAgentMulticastIGMPAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbMgmdAdminModeGet(UnitIndex, L7_AF_INET, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentMulticastIGMPAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentMulticastIGMPAdminMode_disable;
      break;

    default:
      rc = L7_FAILURE;
      *val = 0;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentMulticastIGMPAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;


  switch (val)
  {
  case D_agentMulticastIGMPAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentMulticastIGMPAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbMgmdAdminModeSet(UnitIndex,L7_AF_INET,temp_val);

  return rc;
}


/**********************************************************************/

L7_RC_t
snmpAgentMulticastPIMSMAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbPimsmAdminModeGet(UnitIndex, L7_AF_INET, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentMulticastPIMSMAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentMulticastPIMSMAdminMode_disable;
      break;

    default:
      rc = L7_FAILURE;
      *val = 0;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentMulticastPIMSMAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;


  switch (val)
  {
  case D_agentMulticastPIMSMAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentMulticastPIMSMAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbPimsmAdminModeSet(UnitIndex, L7_AF_INET, temp_val);

  return rc;
}

/**********************************************************************/

L7_RC_t
snmpAgentMulticastPIMDMAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbPimRouterAdminModeGet(UnitIndex, L7_AF_INET, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentMulticastPIMDMAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentMulticastPIMDMAdminMode_disable;
      break;

    default:
      rc = L7_FAILURE;
      *val = 0;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentMulticastPIMDMAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;


  switch (val)
  {
  case D_agentMulticastPIMDMAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentMulticastPIMDMAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbPimRouterAdminModeSet(UnitIndex,L7_AF_INET,temp_val);

  return rc;
}



/**********************************************************************/

L7_RC_t
snmpAgentMulticastDVMRPAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbDvmrpAdminModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentMulticastDVMRPAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentMulticastDVMRPAdminMode_disable;
      break;

    default:
      rc = L7_FAILURE;
      *val = 0;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentMulticastDVMRPAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;


  switch (val)
  {
  case D_agentMulticastDVMRPAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentMulticastDVMRPAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDvmrpAdminModeSet(UnitIndex, temp_val);

  return rc;
}

/* APIs Added to be compatible with the Rel_k */

/* PIM TRAPS */
L7_RC_t
snmpAgentPIMTrapModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbPimTrapModeGet(L7_PIM_TRAP_ALL, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSnmpPIMTrapFlag_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSnmpPIMTrapFlag_disable;
      break;

    default:
      rc = L7_FAILURE;
      *val = 0;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPIMTrapModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;


  switch (val)
  {
  case D_agentSnmpPIMTrapFlag_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSnmpPIMTrapFlag_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbPimTrapModeSet(UnitIndex, temp_val, L7_PIM_TRAP_ALL);

  return rc;
}

/* ****************** DVMRP TRAPS ***************/
L7_RC_t
snmpAgentDVMRPTrapModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbDvmrpTrapModeGet(UnitIndex, L7_DVMRP_TRAP_ALL, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSnmpDVMRPTrapFlag_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSnmpDVMRPTrapFlag_disable;
      break;

    default:
      rc = L7_FAILURE;
      *val = 0;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentDVMRPTrapModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentSnmpDVMRPTrapFlag_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSnmpDVMRPTrapFlag_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDvmrpTrapModeSet(UnitIndex, temp_val, L7_DVMRP_TRAP_ALL);

  return rc;
}

/*  API to Enable Mcast Globally */
L7_RC_t
snmpAgentMulticastRoutingAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;


  switch (val)
  {
  case D_agentMulticastRoutingAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentMulticastRoutingAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbMcastAdminModeSet(UnitIndex, temp_val);

  return rc;
}

L7_RC_t
snmpAgentMulticastRoutingAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbMcastAdminModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentMulticastRoutingAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentMulticastRoutingAdminMode_disable;
      break;

    default:
      rc = L7_FAILURE;
      *val = 0;
      break;
    }
  }

  return rc;
}




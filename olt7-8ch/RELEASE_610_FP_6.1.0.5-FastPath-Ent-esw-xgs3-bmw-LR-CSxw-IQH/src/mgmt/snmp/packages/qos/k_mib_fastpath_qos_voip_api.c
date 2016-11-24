/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_qos_voip_api.c
*
* @purpose    Wrapper functions for Fastpath QOS AUTO VOIP MIB
*
* @component  SNMP
*
* @comments
*
* @create     11/23/2007
*
* @author     Amitabha Sen
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_fastpath_qos_voip_api.h"

#include "usmdb_qos_voip_api.h"
#include "voip_exports.h"
#include "usmdb_util_api.h"

L7_RC_t
snmpAgentAutoVoIPModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 mode;

  rc = usmDbQosVoIPIntfModeGet(UnitIndex, intIfNum, &mode);

  if (rc == L7_SUCCESS)
  {
    switch (mode)
    {
    case L7_ENABLE:
      *val = D_agentAutoVoIPMode_enable;
      break;
    case L7_DISABLE:
      *val = D_agentAutoVoIPMode_disable;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAutoVoIPModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 mode = L7_DISABLE;

  switch (val)
  {
  case D_agentAutoVoIPMode_enable:
    mode = L7_ENABLE;
    break;
  case D_agentAutoVoIPMode_disable:
    mode = L7_DISABLE;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosVoIPIntfModeSet(UnitIndex, intIfNum, mode);
  }

  return rc;
}

#if COSQUEUE_BUCKET_SUPPORT /* Min Bandwidth not supported yet */
L7_RC_t
snmpAgentAutoVoIPMinBandwidthGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 bandwidth;

  rc = usmDbQosVoIPIntfMinBandwidthGet(UnitIndex, intIfNum, &bandwidth);

  if (rc == L7_SUCCESS)
  {
    switch (bandwidth)
    {
    case VOIPBANDWIDTH_0:
      *val = D_agentAutoVoIPMinBandwidth_minbw0;
      break;
    case VOIPBANDWIDTH_64:
      *val = D_agentAutoVoIPMinBandwidth_minbw64;
      break;
    case VOIPBANDWIDTH_128:
      *val = D_agentAutoVoIPMinBandwidth_minbw128;
      break;
    case VOIPBANDWIDTH_256:
      *val = D_agentAutoVoIPMinBandwidth_minbw256;
      break;
    case VOIPBANDWIDTH_512:
      *val = D_agentAutoVoIPMinBandwidth_minbw512;
      break;
    case VOIPBANDWIDTH_1024:
      *val = D_agentAutoVoIPMinBandwidth_minbw1024;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAutoVoIPMinBandwidthSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 bandwidth = VOIPBANDWIDTH_0;

  if (intIfNum != L7_ALL_INTERFACES)
  {
    return L7_FAILURE;
  }
  
  switch (val)
  {
  case D_agentAutoVoIPMinBandwidth_minbw0:
    bandwidth= VOIPBANDWIDTH_0;
    break;
  case D_agentAutoVoIPMinBandwidth_minbw64:
    bandwidth = VOIPBANDWIDTH_64;
    break;
  case D_agentAutoVoIPMinBandwidth_minbw128:
    bandwidth = VOIPBANDWIDTH_128;
    break;
  case D_agentAutoVoIPMinBandwidth_minbw256:
    bandwidth = VOIPBANDWIDTH_256;
    break;
  case D_agentAutoVoIPMinBandwidth_minbw512:
    bandwidth = VOIPBANDWIDTH_512;
    break;
  case D_agentAutoVoIPMinBandwidth_minbw1024:
    bandwidth = VOIPBANDWIDTH_1024;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosVoIPIntfMinBandwidthSet(UnitIndex, intIfNum, bandwidth);
  }

  return rc;
}
#endif

L7_RC_t
snmpAgentAutoVoIPCosQueueGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *CosQueue)
{
  L7_RC_t rc;

  rc = usmDbQosVoIPIntfCosQueueGet(UnitIndex, intIfNum, CosQueue);

  return rc;
}

/**********************************************************************/

L7_RC_t
snmpAgentAutoVoIPEntryGet(L7_uint32 UnitIndex, 
                          L7_uint32 agentAutoVoIPIntfIndex, 
                          L7_uint32 *intIfNum)
{

  /* if all interfaces */
  if (agentAutoVoIPIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    return L7_SUCCESS;
  }
  /* check if interface is of valid type */
  else if (usmDbIntIfNumFromExtIfNum(agentAutoVoIPIntfIndex, intIfNum) == L7_SUCCESS)
  {
    if(usmdbQoSVoIPIsValidIntf(*intIfNum)==L7_TRUE)    
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentAutoVoIPEntryNextGet(L7_uint32 UnitIndex, 
                              L7_uint32 *agentAutoVoIPIntfIndex, 
                              L7_uint32 *intIfNum)
{
  /* if all interfaces */
  if (*agentAutoVoIPIntfIndex == 0)
  {
    *intIfNum = 1;
    if (usmDbExtIfNumFromIntIfNum(*intIfNum, agentAutoVoIPIntfIndex) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  else
  {
    if (usmDbNextPhysicalExtIfNumberGet(UnitIndex, *agentAutoVoIPIntfIndex, agentAutoVoIPIntfIndex) 
                                        == L7_SUCCESS &&
        usmDbIntIfNumFromExtIfNum(*agentAutoVoIPIntfIndex, intIfNum) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }

  /* no more interfaces available */
  return L7_FAILURE;
}


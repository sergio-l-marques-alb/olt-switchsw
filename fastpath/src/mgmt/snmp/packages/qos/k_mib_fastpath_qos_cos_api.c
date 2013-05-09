/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_qos_cos_api.c
*
* @purpose    Wrapper functions for Fastpath QOS COS MIB
*
* @component  SNMP
*
* @comments
*
* @create     5/04/2004
*
* @author     cpverne
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_fastpath_qos_cos_api.h"
#include "usmdb_util_api.h"
#include "cos_exports.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_cos_api.h"
#endif



L7_RC_t
snmpAgentCosMapIntfTrustModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_QOS_COS_MAP_INTF_MODE_t temp_val;

  rc = usmDbQosCosMapTrustModeGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
      *val = D_agentCosMapIntfTrustMode_untrusted;
      break;
    case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
      *val = D_agentCosMapIntfTrustMode_trustDot1p;
      break;
    case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
      *val = D_agentCosMapIntfTrustMode_trustIpPrecedence;
      break;
    case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
      *val = D_agentCosMapIntfTrustMode_trustIpDscp;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentCosMapIntfTrustModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_QOS_COS_MAP_INTF_MODE_t temp_val = 0;

  switch (val)
  {
  case D_agentCosMapIntfTrustMode_untrusted:
    temp_val = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    break;
  case D_agentCosMapIntfTrustMode_trustDot1p:
    temp_val = L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P;
    break;
  case D_agentCosMapIntfTrustMode_trustIpPrecedence:
    temp_val = L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC;
    break;
  case D_agentCosMapIntfTrustMode_trustIpDscp:
    temp_val = L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosCosMapTrustModeSet(UnitIndex, intIfNum, temp_val);
  }

  return rc;
}

/**********************************************************************/

L7_RC_t
snmpAgentCosMapIpPrecEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosMapIpPrecIntfIndex, L7_uint32 agentCosMapIpPrecValue, L7_uint32 *intIfNum)
{
  /* if all interfaces */
  if (agentCosMapIpPrecIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    if (usmDbQosCosMapIpPrecIndexGet(UnitIndex, agentCosMapIpPrecValue) == L7_SUCCESS)
      return L7_SUCCESS;

    return L7_FAILURE;
  }

  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(agentCosMapIpPrecIntfIndex, intIfNum) == L7_SUCCESS &&
      usmDbQosCosMapIpPrecIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS &&
      usmDbQosCosMapIpPrecIndexGet(UnitIndex, agentCosMapIpPrecValue) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpAgentCosMapIpPrecEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosMapIpPrecIntfIndex, L7_uint32 *agentCosMapIpPrecValue, L7_uint32 *intIfNum)
{
  /* if all interfaces */
  if (*agentCosMapIpPrecIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    /* iterate through agentCosMapIpPrecValue */
    if (usmDbQosCosMapIpPrecIndexGet(UnitIndex, *agentCosMapIpPrecValue) == L7_SUCCESS ||
        usmDbQosCosMapIpPrecIndexGetNext(UnitIndex, *agentCosMapIpPrecValue, agentCosMapIpPrecValue) == L7_SUCCESS)
      return L7_SUCCESS;

    *agentCosMapIpPrecValue = 0;
  }

  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(*agentCosMapIpPrecIntfIndex, intIfNum) != L7_SUCCESS)
  {
    *agentCosMapIpPrecValue = 0;
    if (usmDbGetNextVisibleExtIfNumber(*agentCosMapIpPrecIntfIndex, agentCosMapIpPrecIntfIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*agentCosMapIpPrecIntfIndex, intIfNum) != L7_SUCCESS)
    {
      /* no more interfaces available */
      return L7_FAILURE;
    }
  }
  
  do {

    /* iterate through agentCosMapIpPrecValue */
    if (usmDbQosCosMapIpPrecIndexGet(UnitIndex, *agentCosMapIpPrecValue) == L7_SUCCESS ||
        usmDbQosCosMapIpPrecIndexGetNext(UnitIndex, *agentCosMapIpPrecValue, agentCosMapIpPrecValue) == L7_SUCCESS)
    {
      if (usmDbExtIfNumFromIntIfNum(*intIfNum, agentCosMapIpPrecIntfIndex) == L7_SUCCESS)
        return L7_SUCCESS;

      /* could not translate to external index */
      return L7_FAILURE;
    }

    /* no more agentCosMapIpPrecValue */
    *agentCosMapIpPrecValue = 0;
  } while (usmDbQosCosMapIpPrecIntfIndexGetNext(UnitIndex, *intIfNum, intIfNum) == L7_SUCCESS);

  /* no more interfaces with agentCosMapIpPrecValue */
  return L7_FAILURE;
}

/**********************************************************************/

L7_RC_t
snmpAgentCosMapIpDscpEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosMapIpDscpIntfIndex, L7_uint32 agentCosMapIpDscpValue, L7_uint32 *intIfNum)
{
  /* if all interfaces */
  if (agentCosMapIpDscpIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    if (usmDbQosCosMapIpDscpIndexGet(UnitIndex, agentCosMapIpDscpValue) == L7_SUCCESS)
      return L7_SUCCESS;

    return L7_FAILURE;
  }

  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(agentCosMapIpDscpIntfIndex, intIfNum) == L7_SUCCESS &&
      usmDbQosCosMapIpDscpIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS &&
      usmDbQosCosMapIpDscpIndexGet(UnitIndex, agentCosMapIpDscpValue) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpAgentCosMapIpDscpEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosMapIpDscpIntfIndex, L7_uint32 *agentCosMapIpDscpValue, L7_uint32 *intIfNum)
{
  /* if all interfaces */
  if (*agentCosMapIpDscpIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    /* iterate through agentCosMapIpDscpValue */
    if (usmDbQosCosMapIpDscpIndexGet(UnitIndex, *agentCosMapIpDscpValue) == L7_SUCCESS ||
        usmDbQosCosMapIpDscpIndexGetNext(UnitIndex, *agentCosMapIpDscpValue, agentCosMapIpDscpValue) == L7_SUCCESS)
      return L7_SUCCESS;

    *agentCosMapIpDscpValue = 0;
  }

  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(*agentCosMapIpDscpIntfIndex, intIfNum) != L7_SUCCESS)
  {
    *agentCosMapIpDscpValue = 0;
    if (usmDbGetNextVisibleExtIfNumber(*agentCosMapIpDscpIntfIndex, agentCosMapIpDscpIntfIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*agentCosMapIpDscpIntfIndex, intIfNum) != L7_SUCCESS)
    {
      /* no more interfaces available */
      return L7_FAILURE;
    }
  }
  
  do {

    /* iterate through agentCosMapIpDscpValue */
    if (usmDbQosCosMapIpDscpIndexGet(UnitIndex, *agentCosMapIpDscpValue) == L7_SUCCESS ||
        usmDbQosCosMapIpDscpIndexGetNext(UnitIndex, *agentCosMapIpDscpValue, agentCosMapIpDscpValue) == L7_SUCCESS)
    {
      if (usmDbExtIfNumFromIntIfNum(*intIfNum, agentCosMapIpDscpIntfIndex) == L7_SUCCESS)
        return L7_SUCCESS;

      /* could not translate to external index */
      return L7_FAILURE;
    }

    /* no more agentCosMapIpDscpValue */
    *agentCosMapIpDscpValue = 0;
  } while (usmDbQosCosMapIpDscpIntfIndexGetNext(UnitIndex, *intIfNum, intIfNum) == L7_SUCCESS);

  /* no more interfaces with agentCosMapIpDscpValue */
  return L7_FAILURE;
}

/**********************************************************************/

L7_RC_t
snmpAgentCosMapIntfTrustEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosMapIntfTrustIntfIndex, L7_uint32 *intIfNum)
{
  /* if all interfaces */
  if (agentCosMapIntfTrustIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    return L7_SUCCESS;
  }

  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(agentCosMapIntfTrustIntfIndex, intIfNum) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpAgentCosMapIntfTrustEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosMapIntfTrustIntfIndex, L7_uint32 *intIfNum)
{
  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(*agentCosMapIntfTrustIntfIndex, intIfNum) != L7_SUCCESS)
  {
    if (usmDbGetNextVisibleExtIfNumber(*agentCosMapIntfTrustIntfIndex, agentCosMapIntfTrustIntfIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*agentCosMapIntfTrustIntfIndex, intIfNum) != L7_SUCCESS)
    {
      /* no more interfaces available */
      return L7_FAILURE;
    }
    /* check and see if this new index is valid */
    else if (usmDbQosCosQueueIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS)
    {
      return usmDbExtIfNumFromIntIfNum(*intIfNum, agentCosMapIntfTrustIntfIndex);
    }
  }
  
  if (usmDbQosCosMapTrustModeIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    if (usmDbQosCosMapTrustModeIntfIndexGetNext(UnitIndex, *intIfNum, intIfNum) == L7_SUCCESS)
      return usmDbExtIfNumFromIntIfNum(*intIfNum, agentCosMapIntfTrustIntfIndex);
  }

  /* no more interfaces */
  return L7_FAILURE;
}

/**********************************************************************/

L7_RC_t
snmpAgentCosQueueControlEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosQueueIntfIndex, L7_uint32 *intIfNum)
{
  /* if all interfaces */
  if (agentCosQueueIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    return L7_SUCCESS;
  }

  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(agentCosQueueIntfIndex, intIfNum) == L7_SUCCESS &&
      usmDbQosCosQueueIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpAgentCosQueueControlEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosQueueIntfIndex, L7_uint32 *intIfNum)
{
  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(*agentCosQueueIntfIndex, intIfNum) != L7_SUCCESS)
  {
    if (usmDbGetNextVisibleExtIfNumber(*agentCosQueueIntfIndex, agentCosQueueIntfIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*agentCosQueueIntfIndex, intIfNum) != L7_SUCCESS)
    {
      /* no more interfaces available */
      return L7_FAILURE;
    }
    /* check and see if this new index is valid */
    else if (usmDbQosCosQueueIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS)
    {
      return usmDbExtIfNumFromIntIfNum(*intIfNum, agentCosQueueIntfIndex);
    }
  }
  
  if (usmDbQosCosQueueIntfIndexGetNext(UnitIndex, *intIfNum, intIfNum) == L7_SUCCESS)
  {
    return usmDbExtIfNumFromIntIfNum(*intIfNum, agentCosQueueIntfIndex);
  }
  return L7_FAILURE;
}


L7_RC_t
snmpAgentCosQueueMgmtTypeIntfGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_QOS_COS_QUEUE_MGMT_TYPE_t temp_val;

  rc = usmDbQosCosQueueMgmtTypePerIntfGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP:
      *val = D_agentCosQueueMgmtTypeIntf_taildrop;
      break;

    case L7_QOS_COS_QUEUE_MGMT_TYPE_WRED:
      *val = D_agentCosQueueMgmtTypeIntf_wred;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtTypeIntfSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_QOS_COS_QUEUE_MGMT_TYPE_t temp_val = 0;

  switch (val)
  {
  case D_agentCosQueueMgmtTypeIntf_taildrop:
    temp_val = L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP;
    break;

  case D_agentCosQueueMgmtTypeIntf_wred:
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_QOS_COS_QUEUE_MGMT_TYPE_WRED;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosCosQueueMgmtTypePerIntfSet(UnitIndex, intIfNum, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueDefaultsRestoreSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentCosQueueDefaultsRestore_disable:
    break;

  case D_agentCosQueueDefaultsRestore_enable:
    rc = usmDbQosCosQueueDefaultsRestore(UnitIndex,intIfNum);
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/**********************************************************************/

L7_RC_t
snmpAgentCosQueueEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosQueueIntfIndex, L7_uint32 agentCosQueueIndex, L7_uint32 *intIfNum)
{
  /* if all interfaces, verify other indexes */
  if (agentCosQueueIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;

    if (usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex) == L7_SUCCESS)
      return L7_SUCCESS;

    return L7_FAILURE;
  }

  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(agentCosQueueIntfIndex, intIfNum) == L7_SUCCESS &&
      usmDbQosCosQueueIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS &&
      usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpAgentCosQueueEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosQueueIntfIndex, L7_uint32 *agentCosQueueIndex, L7_uint32 *intIfNum)
{
  /* if all interfaces, verify other indexes, else go on to first actual interface */
  if (*agentCosQueueIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    /* iterate through queues */
    if (usmDbQosCosQueueIdIndexGet(UnitIndex, *agentCosQueueIndex) == L7_SUCCESS ||
        usmDbQosCosQueueIdIndexGetNext(UnitIndex, *agentCosQueueIndex, agentCosQueueIndex) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }

    /* no more queues */
    *agentCosQueueIndex = 0;
  }

  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(*agentCosQueueIntfIndex, intIfNum) != L7_SUCCESS)
  {
    *agentCosQueueIndex = 0;
    if (usmDbGetNextVisibleExtIfNumber(*agentCosQueueIntfIndex, agentCosQueueIntfIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*agentCosQueueIntfIndex, intIfNum) != L7_SUCCESS)
    {
      /* no more interfaces available */
      return L7_FAILURE;
    }
  }
  
  do {

    /* iterate through queues */
    if (usmDbQosCosQueueIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS &&
        (usmDbQosCosQueueIdIndexGet(UnitIndex, *agentCosQueueIndex) == L7_SUCCESS ||
         usmDbQosCosQueueIdIndexGetNext(UnitIndex, *agentCosQueueIndex, agentCosQueueIndex) == L7_SUCCESS))
    {
      if (usmDbExtIfNumFromIntIfNum(*intIfNum, agentCosQueueIntfIndex) == L7_SUCCESS)
        return L7_SUCCESS;

      /* could not translate to external index */
      return L7_FAILURE;
    }

    /* no more queues */
    *agentCosQueueIndex = 0;
  } while (usmDbQosCosQueueIntfIndexGetNext(UnitIndex, *intIfNum, intIfNum) == L7_SUCCESS);

  /* no more interfaces with queues */
  return L7_FAILURE;
}

L7_RC_t
snmpAgentCosQueueSchedulerTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_qosCosQueueSchedTypeList_t temp_list;
  L7_uint32 listIndex = agentCosQueueIndex - L7_QOS_COS_QUEUE_ID_MIN;

  rc = usmDbQosCosQueueSchedulerTypeListGet(UnitIndex, intIfNum, &temp_list);

  if (rc == L7_SUCCESS &&
      usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex) == L7_SUCCESS)
  {
    switch (temp_list.schedType[listIndex])
    {
    case L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT:
      *val = D_agentCosQueueSchedulerType_strict;
      break;

    case L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED:
      *val = D_agentCosQueueSchedulerType_weighted;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueSchedulerTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  rc = usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex);

  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
    case D_agentCosQueueSchedulerType_strict:
      temp_val = L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT;
      break;

    case D_agentCosQueueSchedulerType_weighted:
      temp_val = L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosCosQueueSchedulerTypeSet(UnitIndex, intIfNum, agentCosQueueIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMinBandwidthGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_qosCosQueueBwList_t temp_list;
  L7_uint32 listIndex = agentCosQueueIndex - L7_QOS_COS_QUEUE_ID_MIN;

  rc = usmDbQosCosQueueMinBandwidthListGet(UnitIndex, intIfNum, &temp_list);

  if (rc == L7_SUCCESS &&
      usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex) == L7_SUCCESS)
  {
    *val = temp_list.bandwidth[listIndex];
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMinBandwidthSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 val)
{
  L7_RC_t rc = 0;

  rc = usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosCosQueueMinBandwidthSet(UnitIndex, intIfNum, agentCosQueueIndex, val);
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMaxBandwidthGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_qosCosQueueBwList_t temp_list;
  L7_uint32 listIndex = agentCosQueueIndex - L7_QOS_COS_QUEUE_ID_MIN;

  rc = usmDbQosCosQueueMaxBandwidthListGet(UnitIndex, intIfNum, &temp_list);

  if (rc == L7_SUCCESS &&
      usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex) == L7_SUCCESS)
  {
    *val = temp_list.bandwidth[listIndex];
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMaxBandwidthSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 val)
{
  L7_RC_t rc = 0;

  rc = usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosCosQueueMaxBandwidthSet(UnitIndex, intIfNum, agentCosQueueIndex, val);
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_qosCosQueueMgmtTypeList_t temp_list;
  L7_uint32 listIndex = agentCosQueueIndex - L7_QOS_COS_QUEUE_ID_MIN;

  rc = usmDbQosCosQueueMgmtTypeListGet(UnitIndex,intIfNum, &temp_list);

  if (rc == L7_SUCCESS &&
      usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex) == L7_SUCCESS)
  {
    switch(temp_list.mgmtType[listIndex])
    {
    case L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP:
      *val = D_agentCosQueueMgmtType_taildrop;
      break;

    case L7_QOS_COS_QUEUE_MGMT_TYPE_WRED:
      *val = D_agentCosQueueMgmtType_wred;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  rc = usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex);

  if (rc == L7_SUCCESS)
  {
    switch(val)
    {
    case D_agentCosQueueMgmtType_taildrop:
      temp_val = L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP;
      break;

    case D_agentCosQueueMgmtType_wred:
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE)
      {
        temp_val = L7_QOS_COS_QUEUE_MGMT_TYPE_WRED;
      }
      else
      {
        rc = L7_FAILURE;
      }
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosCosQueueMgmtTypeSet(UnitIndex, intIfNum, agentCosQueueIndex, temp_val);
  }

  return rc;
}


/**********************************************************************/


L7_RC_t
snmpAgentCosQueueMgmtEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosQueueIntfIndex, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *intIfNum)
{
  /* if all interfaces, verify other indexes */
  if (agentCosQueueIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    if (usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex) == L7_SUCCESS &&
        usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS)
      return L7_SUCCESS;

    return L7_FAILURE;
  }

  if (usmDbIntIfNumFromExtIfNum(agentCosQueueIntfIndex, intIfNum) == L7_SUCCESS &&
      usmDbQosCosQueueIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS &&
      usmDbQosCosQueueIdIndexGet(UnitIndex, agentCosQueueIndex) == L7_SUCCESS &&
      usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpAgentCosQueueMgmtEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosQueueIntfIndex, L7_uint32 *agentCosQueueIndex, L7_uint32 *agentCosQueueDropPrecIndex, L7_uint32 *intIfNum)
{
  /* if all interfaces, verify other indexes, else go on to first actual interface */
  if (*agentCosQueueIntfIndex == 0)
  {
    *intIfNum = L7_ALL_INTERFACES;
    /* iterate through queues */
    if (usmDbQosCosQueueIdIndexGet(UnitIndex, *agentCosQueueIndex) == L7_SUCCESS ||
        usmDbQosCosQueueIdIndexGetNext(UnitIndex, *agentCosQueueIndex, agentCosQueueIndex) == L7_SUCCESS)
    {
      do {
        /* iterate through drop precedences */
        if (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, *agentCosQueueDropPrecIndex) == L7_SUCCESS ||
            usmDbQosCosQueueDropPrecIndexGetNext(UnitIndex, *agentCosQueueDropPrecIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS)
          return L7_SUCCESS;

        /* no more drop precedences */
        *agentCosQueueDropPrecIndex = 0;
      } while (usmDbQosCosQueueIdIndexGetNext(UnitIndex, *agentCosQueueIndex, agentCosQueueIndex) == L7_SUCCESS);
    }

    /* no more queues */
    *agentCosQueueIndex = 0;
    *agentCosQueueDropPrecIndex = 0;
  }

  *intIfNum = 0;

  if (usmDbIntIfNumFromExtIfNum(*agentCosQueueIntfIndex, intIfNum) != L7_SUCCESS)
  {
    *agentCosQueueIndex = 0;
    *agentCosQueueDropPrecIndex = 0;
    if (usmDbGetNextVisibleExtIfNumber(*agentCosQueueIntfIndex, agentCosQueueIntfIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*agentCosQueueIntfIndex, intIfNum) != L7_SUCCESS)
    {
      /* no more interfaces available */
      return L7_FAILURE;
    }
  }
  
  do {

    /* iterate through queues */
    if (usmDbQosCosQueueIntfIndexGet(UnitIndex, *intIfNum) == L7_SUCCESS &&
        (usmDbQosCosQueueIdIndexGet(UnitIndex, *agentCosQueueIndex) == L7_SUCCESS ||
         usmDbQosCosQueueIdIndexGetNext(UnitIndex, *agentCosQueueIndex, agentCosQueueIndex) == L7_SUCCESS))
    {
      do {
        /* iterate through drop precedences */
        if (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, *agentCosQueueDropPrecIndex) == L7_SUCCESS ||
            usmDbQosCosQueueDropPrecIndexGetNext(UnitIndex, *agentCosQueueDropPrecIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS)
        {
          if (usmDbExtIfNumFromIntIfNum(*intIfNum, agentCosQueueIntfIndex) == L7_SUCCESS)
            return L7_SUCCESS;
          /* could not translate to external index */
          return L7_FAILURE;
        }

        /* no more drop precedences */
        *agentCosQueueDropPrecIndex = 0;
      } while (usmDbQosCosQueueIdIndexGetNext(UnitIndex, *agentCosQueueIndex, agentCosQueueIndex) == L7_SUCCESS);
    }

    /* no more queues */
    *agentCosQueueIndex = 0;
    *agentCosQueueDropPrecIndex = 0;
  } while (usmDbQosCosQueueIntfIndexGetNext(UnitIndex, *intIfNum, intIfNum) == L7_SUCCESS);

  /* no more interfaces with queues */
  return L7_FAILURE;

}

L7_RC_t
snmpAgentCosQueueMgmtTailDropThresholdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_qosCosDropParmsList_t temp_list;
  L7_uint32 listIndex = agentCosQueueDropPrecIndex - L7_QOS_COS_DROP_PREC_LEVEL_MIN;

  rc = usmDbQosCosQueueDropParmsListGet(UnitIndex, intIfNum, &temp_list);
  if ((rc == L7_SUCCESS) && 
      (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS) && 
      (agentCosQueueIndex < L7_MAX_CFG_QUEUES_PER_PORT))
  {
      *val = temp_list.queue[agentCosQueueIndex].tailDropMaxThreshold[listIndex];
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtTailDropThresholdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 val)
{
    L7_RC_t rc;
    L7_qosCosDropParmsList_t temp_list;
    L7_uint32 listIndex = agentCosQueueDropPrecIndex - L7_QOS_COS_DROP_PREC_LEVEL_MIN;

    rc = usmDbQosCosQueueDropParmsListGet(UnitIndex, intIfNum, &temp_list);

    if ((rc == L7_SUCCESS) &&
        (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS) && 
        (agentCosQueueIndex < L7_MAX_CFG_QUEUES_PER_PORT))
    {
      temp_list.queue[agentCosQueueIndex].tailDropMaxThreshold[listIndex] = val;
      rc = usmDbQosCosQueueDropParmsListSet(UnitIndex, intIfNum, &temp_list);
    }
    else
    {
      rc = L7_FAILURE;
    }

    return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtWredMinThresholdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *val)
{
    L7_RC_t rc;
    L7_qosCosDropParmsList_t temp_list;
    L7_uint32 listIndex = agentCosQueueDropPrecIndex - L7_QOS_COS_DROP_PREC_LEVEL_MIN;

    rc = usmDbQosCosQueueDropParmsListGet(UnitIndex, intIfNum, &temp_list);
    if ((rc == L7_SUCCESS) && 
        (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS) && 
        (agentCosQueueIndex < L7_MAX_CFG_QUEUES_PER_PORT))
    {
        *val = temp_list.queue[agentCosQueueIndex].minThreshold[listIndex];
    }
    else
    {
      rc = L7_FAILURE;
    }

    return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtWredMinThresholdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_qosCosDropParmsList_t temp_list;
  L7_uint32 listIndex = agentCosQueueDropPrecIndex - L7_QOS_COS_DROP_PREC_LEVEL_MIN;

  rc = usmDbQosCosQueueDropParmsListGet(UnitIndex, intIfNum, &temp_list);

  if ((rc == L7_SUCCESS) &&
      (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS) && 
      (agentCosQueueIndex < L7_MAX_CFG_QUEUES_PER_PORT))
  {
    temp_list.queue[agentCosQueueIndex].minThreshold[listIndex] = val;
    rc = usmDbQosCosQueueDropParmsListSet(UnitIndex, intIfNum, &temp_list);
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtWredMaxThresholdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *val)
{
    L7_RC_t rc;
    L7_qosCosDropParmsList_t temp_list;
    L7_uint32 listIndex = agentCosQueueDropPrecIndex - L7_QOS_COS_DROP_PREC_LEVEL_MIN;

    rc = usmDbQosCosQueueDropParmsListGet(UnitIndex, intIfNum, &temp_list);
    if ((rc == L7_SUCCESS) && 
        (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS) && 
        (agentCosQueueIndex < L7_MAX_CFG_QUEUES_PER_PORT))
    {
        *val = temp_list.queue[agentCosQueueIndex].wredMaxThreshold[listIndex];
    }
    else
    {
      rc = L7_FAILURE;
    }

    return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtWredMaxThresholdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_qosCosDropParmsList_t temp_list;
  L7_uint32 listIndex = agentCosQueueDropPrecIndex - L7_QOS_COS_DROP_PREC_LEVEL_MIN;

  rc = usmDbQosCosQueueDropParmsListGet(UnitIndex, intIfNum, &temp_list);

  if ((rc == L7_SUCCESS) &&
      (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS) && 
      (agentCosQueueIndex < L7_MAX_CFG_QUEUES_PER_PORT))
  {
    temp_list.queue[agentCosQueueIndex].wredMaxThreshold[listIndex] = val;
    rc = usmDbQosCosQueueDropParmsListSet(UnitIndex, intIfNum, &temp_list);
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtWredDropProbabilityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *val)
{
    L7_RC_t rc;
    L7_qosCosDropParmsList_t temp_list;
    L7_uint32 listIndex = agentCosQueueDropPrecIndex - L7_QOS_COS_DROP_PREC_LEVEL_MIN;

    rc = usmDbQosCosQueueDropParmsListGet(UnitIndex, intIfNum, &temp_list);
    if ((rc == L7_SUCCESS) && 
        (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS) && 
        (agentCosQueueIndex < L7_MAX_CFG_QUEUES_PER_PORT))
    {
        *val = temp_list.queue[agentCosQueueIndex].dropProb[listIndex];
    }
    else
    {
      rc = L7_FAILURE;
    }

    return rc;
}

L7_RC_t
snmpAgentCosQueueMgmtWredDropProbabilitySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_qosCosDropParmsList_t temp_list;
  L7_uint32 listIndex = agentCosQueueDropPrecIndex - L7_QOS_COS_DROP_PREC_LEVEL_MIN;

  rc = usmDbQosCosQueueDropParmsListGet(UnitIndex, intIfNum, &temp_list);

  if ((rc == L7_SUCCESS) &&
      (usmDbQosCosQueueDropPrecIndexGet(UnitIndex, agentCosQueueDropPrecIndex) == L7_SUCCESS) && 
      (agentCosQueueIndex < L7_MAX_CFG_QUEUES_PER_PORT))
  {
    temp_list.queue[agentCosQueueIndex].dropProb[listIndex] = val;
    rc = usmDbQosCosQueueDropParmsListSet(UnitIndex, intIfNum, &temp_list);
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

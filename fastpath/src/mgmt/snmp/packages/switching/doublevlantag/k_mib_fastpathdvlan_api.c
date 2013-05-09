/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdvlan_api.c
*
* @purpose    Wrapper functions for Fastpath DVLAN
*
* @component  SNMP
*
* @comments
*
* @create     3/6/2007
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_fastpathdvlan_api.h"
#include "usmdb_dvlantag_api.h"
#include "usmdb_util_api.h"

/**************************************************************************************************************/

L7_RC_t snmpAgentSwitchDvlanTagEthertypeCurrentValueGet(L7_uint32 unit,
                                                        L7_uint32 *etherType)
{
  L7_uint32 i = L7_NULL;

  if (etherType == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  
  for(i = L7_NULL; i < L7_DVLANTAG_MAX_TPIDS; ++i)
  {
    if (usmDbDvlantagEthertypeGet(unit, etherType, i) == L7_SUCCESS)
    {
      if(*etherType == L7_NULL)
      {
        continue;
      }

      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentSwitchDvlanTagEthertypeGet(L7_uint32 UnitIndex, L7_uint32 etherType)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i, tpidValue;

  if (etherType == 0)
  {
    return L7_FAILURE;
  }
  
  for(i =0; i<L7_DVLANTAG_MAX_TPIDS; i++)
  {
    if (usmDbDvlantagEthertypeGet(UnitIndex, &tpidValue, i) == L7_SUCCESS)
    {
      if(tpidValue == etherType)
      {
        return L7_SUCCESS;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDvlanTagEthertypeNextGet(L7_uint32 UnitIndex, L7_uint32 etherType, 
                                                   L7_uint32 *nextEtherType)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i, j, tpidValue;

  if (etherType == 0)
  {
    if(usmDbDvlantagEthertypeGet(UnitIndex, &tpidValue, 0) == L7_SUCCESS)
    {
      *nextEtherType = tpidValue;
      return L7_SUCCESS;
    }
    return L7_FAILURE;
  }
  
  for(i =0; i<L7_DVLANTAG_MAX_TPIDS; i++)
  {
    if (usmDbDvlantagEthertypeGet(UnitIndex, &tpidValue, i) == L7_SUCCESS)
    {
      if(tpidValue == etherType)
      {
        for(j=i+1; j<L7_DVLANTAG_MAX_TPIDS; j++)
        {
          if(usmDbDvlantagEthertypeGet(UnitIndex, &tpidValue, j) == L7_SUCCESS)
          {
            if(tpidValue == 0)
              continue;

            *nextEtherType = tpidValue;
            return L7_SUCCESS;
          }
        }
        return L7_FAILURE;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDvlanTagIsPrimaryTPid(L7_uint32 UnitIndex, L7_uint32 etherType)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 tpidValue;

  if (etherType == 0)
  {
    return L7_FAILURE;
  }
  
  if (usmDbDvlantagEthertypeGet(UnitIndex, &tpidValue, 0) == L7_SUCCESS)
  {
    if(tpidValue == etherType)
    {
      return L7_SUCCESS;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortDVlanTagModeGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDvlantagIntfModeGet(UnitIndex, intIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
#ifdef I_agentPortDVlanTagMode 
    case L7_ENABLE:
      *val = D_agentPortDVlanTagMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentPortDVlanTagMode_disable;
      break;
#endif

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortDVlanTagModeSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
#ifdef I_agentPortDVlanTagMode 
  case D_agentPortDVlanTagMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentPortDVlanTagMode_disable:
    temp_val = L7_DISABLE;
    break;
#endif

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDvlantagIntfModeSet(UnitIndex, intIfIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchPortDvlanTagEntryGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 etherType)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i, tpidValue;

  if(usmDbDvlantagIntfValidCheck(intIfIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (etherType == 0)
  {
    return L7_FAILURE;
  }
  
  for(i =0; i<L7_DVLANTAG_MAX_TPIDS; i++)
  {
    if (usmDbDvlantagIntfEthertypeGet(UnitIndex, intIfIndex, &tpidValue, i) == L7_SUCCESS)
    {
      if(tpidValue == etherType)
      {
        return L7_SUCCESS;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchPortDvlanTagNextEntryGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, 
                                                  L7_uint32 *nextIntfIndex, L7_uint32 etherType,
                                                  L7_uint32 *nextEtherType)
{
  if(usmDbDvlantagIntfValidCheck(intIfIndex) == L7_SUCCESS)
  {
    /* Get the next valid Ethertype on this interface */
    if(snmpAgentSwitchDvlanTagIntfEthertypeNextGet(UnitIndex, intIfIndex, etherType, nextEtherType) == L7_SUCCESS)
    {
      *nextIntfIndex = intIfIndex;
      return L7_SUCCESS;
    }
  }
  while(usmDbValidIntIfNumNext(intIfIndex, nextIntfIndex) == L7_SUCCESS)
  {
    intIfIndex = *nextIntfIndex;
    if(usmDbDvlantagIntfValidCheck(*nextIntfIndex) == L7_SUCCESS)
    {
      /* Get the first valid Ethertype on this new interface */
      etherType = 0;
      if(snmpAgentSwitchDvlanTagIntfEthertypeNextGet(UnitIndex, *nextIntfIndex, etherType, nextEtherType) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentSwitchDvlanTagIntfEthertypeNextGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, 
                                                      L7_uint32 etherType, L7_uint32 *nextEtherType)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i, j, tpidValue;
  if (etherType == 0)
  {
    if(usmDbDvlantagIntfEthertypeGet(UnitIndex, intIfIndex, &tpidValue, 0) == L7_SUCCESS)
    {
      *nextEtherType = tpidValue;
      return L7_SUCCESS;
    }
    return L7_FAILURE;
  }
  
  for(i =0; i<L7_DVLANTAG_MAX_TPIDS; i++)
  {
    if (usmDbDvlantagIntfEthertypeGet(UnitIndex, intIfIndex, &tpidValue, i) == L7_SUCCESS)
    {
      if(tpidValue == etherType)
      {
        for(j=i+1; j<L7_DVLANTAG_MAX_TPIDS; j++)
        {
          if(usmDbDvlantagIntfEthertypeGet(UnitIndex, intIfIndex, &tpidValue, j) == L7_SUCCESS)
          {
            if(tpidValue == 0)
              continue;

            *nextEtherType = tpidValue;
            return L7_SUCCESS;
          }
        }
        return L7_FAILURE;
      }
    }
  }

  return rc;
}

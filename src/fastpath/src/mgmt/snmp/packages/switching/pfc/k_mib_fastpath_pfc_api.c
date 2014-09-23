/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2009
*
**********************************************************************
*
* @filename   k_mib_fastpath_pfc_api.c
*
* @purpose    Wrapper functions for Fastpath PFC Private MIB
*
* @component  SNMP
*
* @comments
*
* @create     06/02/2009
*
* @author     Visakha Erina
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_fastpath_pfc_api.h"

#include "pfc_exports.h"
#include "usmdb_pfc.h"
#include "usmdb_util_api.h"
#include "usmdb_common.h"

L7_RC_t
snmpAgentPfcIntfAdminModeGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 mode;

  rc = usmDbIfPfcModeGet(intIfNum, &mode);

  if (rc == L7_SUCCESS)
  {
    switch (mode)
    {
      case L7_ENABLE:
        *val = D_agentPfcIntfAdminMode_enable;
        break;
      case L7_DISABLE:
        *val = D_agentPfcIntfAdminMode_disable;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}
   
L7_RC_t
snmpAgentPfcIntfAdminModeSet(L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 mode = L7_DISABLE;

  switch (val)
  {
    case D_agentPfcIntfAdminMode_enable:
      mode = L7_ENABLE;
      break;
    case D_agentPfcIntfAdminMode_disable:
      mode = L7_DISABLE;
      break;
    default:
      rc = L7_FAILURE;
      break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIfPfcModeSet(intIfNum, mode);
  }

  return rc;
}

L7_RC_t
snmpAgentPfcIntfPfcStatusGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 status = L7_INACTIVE;

  rc = usmDbIfPfcStatusGet(intIfNum, &status);

  if (rc == L7_SUCCESS)
  {
    switch (status)
    {
      case L7_ACTIVE:
        *val = D_agentPfcIntfPfcStatus_active;
        break;
      case L7_INACTIVE:
        *val = D_agentPfcIntfPfcStatus_inactive;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}
 
L7_RC_t
snmpAgentPfcEntryGet(L7_uint32 agentPfcIntfIndex, L7_uint32 *intIfNum)
{
  *intIfNum =0;
  /* check if interface is of valid type */
  if (usmDbIntIfNumFromExtIfNum(agentPfcIntfIndex, intIfNum) == L7_SUCCESS)
  {
    if(usmDbPfcIntfIndexGet(*intIfNum) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentPfcEntryNextGet(L7_uint32 *agentPfcIntfIndex, L7_uint32 *intIfNum)
{
  *intIfNum = 0;
  if (usmDbIntIfNumFromExtIfNum(*agentPfcIntfIndex, intIfNum) != L7_SUCCESS)
  {
    if (usmDbGetNextVisibleExtIfNumber(*agentPfcIntfIndex, agentPfcIntfIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*agentPfcIntfIndex, intIfNum) != L7_SUCCESS)
    {
      /* no more interfaces available */
      return L7_FAILURE;
    }
    /* check and see if this new index is valid */
    else if (usmDbPfcIntfIndexGet(*intIfNum) == L7_SUCCESS)
    {
      return usmDbExtIfNumFromIntIfNum(*intIfNum, agentPfcIntfIndex);
    }
  }

  if (usmDbPfcIntfIndexGet(*intIfNum) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    if (usmDbPfcIntfIndexGetNext(*intIfNum, intIfNum)== L7_SUCCESS)
      return usmDbExtIfNumFromIntIfNum(*intIfNum, agentPfcIntfIndex);
  }

  /* no more interfaces */
  return L7_FAILURE;
}

L7_RC_t
snmpAgentPfcActionGet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 mode;
  
  rc = usmDbPfcPriorityIndexGet(priority);
  if (rc == L7_SUCCESS)
  {
    rc = usmDbIfPfcPriorityModeGet(intIfNum, priority, &mode); 
    if (rc == L7_SUCCESS)
    {
      switch (mode)
      {
        case L7_PFC_ACTION_DROP:
          *val = D_agentPfcAction_drop;
          break;
        case L7_PFC_ACTION_NO_DROP:
          *val = D_agentPfcAction_nodrop;
          break;
        default:
          rc = L7_FAILURE;
          break;
      }/*end-of-switch*/
    }    
  }
  else
  {
    rc = L7_FAILURE;
  } 

  return rc;
}
   
L7_RC_t
snmpAgentPfcActionSet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 mode = L7_PFC_ACTION_DROP;

  rc = usmDbPfcPriorityIndexGet(priority);
  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
      case D_agentPfcAction_drop:
        mode = L7_PFC_ACTION_DROP;
        break;
      case D_agentPfcAction_nodrop:
        mode = L7_PFC_ACTION_NO_DROP;
        break;
      default:
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
    rc = usmDbIfPfcPriorityModeSet(intIfNum, priority, mode);
  }

  return rc;
}

L7_RC_t
snmpAgentPfcActionEntryGet(L7_uint32 agentPfcIntfIndex, 
                           L7_uint32 agentPfcPriority, 
                           L7_uint32 *intIfNum)
{
  *intIfNum = 0;
  if (usmDbIntIfNumFromExtIfNum(agentPfcIntfIndex, intIfNum) == L7_SUCCESS &&
      usmDbPfcIntfIndexGet(*intIfNum) == L7_SUCCESS &&
      usmDbPfcPriorityIndexGet(agentPfcPriority) == L7_SUCCESS)
  { 
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentPfcActionEntryNextGet(L7_uint32 *agentPfcIntfIndex, 
                               L7_uint32 *agentPfcPriorityIndex, 
                               L7_uint32 *intIfNum)
{
  *intIfNum = 0;
  if (usmDbIntIfNumFromExtIfNum(*agentPfcIntfIndex, intIfNum) != L7_SUCCESS)
  {
    *agentPfcPriorityIndex = 0;
    if (usmDbGetNextVisibleExtIfNumber(*agentPfcIntfIndex, agentPfcIntfIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*agentPfcIntfIndex, intIfNum) != L7_SUCCESS)
    {
      /* no more interfaces available */
      return L7_FAILURE;
    }
  }

  do 
  {
    /* iterate through queues */
    if (usmDbPfcIntfIndexGet(*intIfNum) == L7_SUCCESS &&
        (usmDbPfcPriorityIndexGet(*agentPfcPriorityIndex) == L7_SUCCESS ||
         usmDbPfcPriorityIndexGetNext(*agentPfcPriorityIndex, agentPfcPriorityIndex) == L7_SUCCESS))
    {
      if (usmDbExtIfNumFromIntIfNum(*intIfNum, agentPfcIntfIndex) == L7_SUCCESS)
        return L7_SUCCESS;

      /* could not translate to external index */
      return L7_FAILURE;
    }

    /* no more priorities */
    *agentPfcPriorityIndex = 0;
  }while (usmDbPfcIntfIndexGetNext(*intIfNum, intIfNum) == L7_SUCCESS);

  /* no more interfaces with queues */
  return L7_FAILURE;
}

L7_RC_t
snmpAgentPfcIntfStatsPerPriorityEntryGet(L7_uint32 agentPfcIntfIndex, 
                                         L7_uint32 agentPfcPriority, 
                                         L7_uint32 *intIfNum)
{
  *intIfNum = 0;
  if (usmDbIntIfNumFromExtIfNum(agentPfcIntfIndex, intIfNum) == L7_SUCCESS &&
      usmDbPfcIntfIndexGet(*intIfNum) == L7_SUCCESS &&
      usmDbPfcPriorityIndexGet(agentPfcPriority) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpAgentPfcIntfStatsPerPriorityEntryNextGet(L7_uint32 *agentPfcIntfIndex, 
                                             L7_uint32 *agentPfcPriorityIndex, 
                                             L7_uint32 *intIfNum)
{
  *intIfNum = 0;
  if (usmDbIntIfNumFromExtIfNum(*agentPfcIntfIndex, intIfNum) != L7_SUCCESS)
  {
    *agentPfcPriorityIndex = 0;
    if (usmDbGetNextVisibleExtIfNumber(*agentPfcIntfIndex, agentPfcIntfIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*agentPfcIntfIndex, intIfNum) != L7_SUCCESS)
    {
      /* no more interfaces available */
      return L7_FAILURE;
    }
  }

  do 
  {
    /* iterate through queues */
    if (usmDbPfcIntfIndexGet(*intIfNum) == L7_SUCCESS &&
        (usmDbPfcPriorityIndexGet(*agentPfcPriorityIndex) == L7_SUCCESS ||
         usmDbPfcPriorityIndexGetNext(*agentPfcPriorityIndex, agentPfcPriorityIndex) == L7_SUCCESS))
    {
      if (usmDbExtIfNumFromIntIfNum(*intIfNum, agentPfcIntfIndex) == L7_SUCCESS)
        return L7_SUCCESS;

      /* could not translate to external index */
      return L7_FAILURE;
    }

    /* no more priorities */
    *agentPfcPriorityIndex = 0;
  } while (usmDbPfcIntfIndexGetNext(*intIfNum, intIfNum) == L7_SUCCESS);

  /* no more interfaces with queues */
  return L7_FAILURE;
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_fastpathloopback_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create 10/28/2005
*
* @author nramesh
*
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include <l7_common.h>
#include <l3_comm_structs.h>

#include <usmdb_l3.h>
#include <defaultconfig.h>
#include <sysapi.h>
#include "usmdb_rlim_api.h"
#include "usmdb_util_api.h"

#include "defaultconfig.h"
#include "sysapi.h"

/**************************************************************************************************************/


L7_RC_t
snmpAgentLoopbackEntryGet ( L7_uint32 UnitIndex, L7_uint32 loopbackID)
{
  L7_uint32 pLoopbackId;
  L7_uint32 pNextLoopbackId;
  L7_RC_t  rc = L7_FAILURE;

  rc = usmDbRlimLoopbackIdFirstGet( &pLoopbackId);
  pNextLoopbackId = pLoopbackId;

  do
  {
    pLoopbackId = pNextLoopbackId;
    if( pLoopbackId == loopbackID)
      return L7_SUCCESS;
  }while (usmDbRlimLoopbackIdNextGet(pLoopbackId, &pNextLoopbackId) == L7_SUCCESS);

  return L7_FAILURE;
	
}


L7_RC_t
snmpAgentLoopbackEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 loopbackID, L7_uint32 *nextLoopbackID)
{

  if (usmDbRlimLoopbackIdNextGet(loopbackID, nextLoopbackID) == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE; 
}


L7_RC_t
snmpLoopbackCreate(L7_uint32 UnitIndex, L7_uint32 loopbackIfIndex)
{

  if (usmDbRlimLoopbackCreate(loopbackIfIndex) == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

L7_RC_t
snmpLoopbackDelete(L7_uint32 UnitIndex, L7_uint32 intIfIndex)
{

  if (usmDbRlimLoopbackDelete(UnitIndex,intIfIndex) == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

L7_RC_t
snmpLoopbackStatusSet(L7_uint32 UnitIndex, L7_uint32 loopbackID, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIfIndex;

  switch (val)
  {
  case D_agentLoopbackStatus_createAndGo:
    rc =usmDbRlimLoopbackCreate(loopbackID);
    break;

  case D_agentLoopbackStatus_active:
    rc = L7_SUCCESS;
    break;

  case D_agentLoopbackStatus_destroy:
    rc = usmDbRlimLoopbackIntIfNumGet(loopbackID, &intIfIndex);
    rc = usmDbRlimLoopbackDelete(UnitIndex, intIfIndex);
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpLoopbackIfIndexGet (L7_uint32 UnitIndex, L7_uint32 loopbackID, L7_uint32 *loopbackIfIndex)
{

  L7_uint32 intIfIndex;

  if ((usmDbRlimLoopbackIntIfNumGet(loopbackID, &intIfIndex) == L7_SUCCESS) &&
      (usmDbExtIfNumFromIntIfNum(intIfIndex, loopbackIfIndex)  == L7_SUCCESS))
    return L7_SUCCESS;
 else
   return L7_FAILURE;
}


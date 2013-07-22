/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_qos_diffserv_extensions_api.h
*
* @purpose  Provide interface to DiffServ Standard MIB
*                  
* @component SNMP
*
* @comments 
*
* @create 07/12/2002
*
* @author cpverne
* @end
*
**********************************************************************/
#include "usmdb_mib_diffserv_api.h"

L7_RC_t
snmpAgentDiffServAuxMFClfrStorageGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServAuxMFClfrStorageGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServAuxMfClfrStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServAuxMfClfrStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServAuxMfClfrStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServAuxMfClfrStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServAuxMfClfrStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServAuxMFClfrStatusGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServAuxMFClfrStatusGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServAuxMfClfrStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServAuxMfClfrStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServAuxMfClfrStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServAuxMfClfrStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServAuxMfClfrStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServAuxMfClfrStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpAgentDiffServAssignQueueStorageGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServAssignQueueStorageGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServAssignQueueStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServAssignQueueStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServAssignQueueStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServAssignQueueStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServAssignQueueStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServAssignQueueStatusGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServAssignQueueStatusGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServAssignQueueStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServAssignQueueStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServAssignQueueStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServAssignQueueStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServAssignQueueStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServAssignQueueStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentDiffServRedirectStorageGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServRedirectStorageGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServRedirectStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServRedirectStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServRedirectStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServRedirectStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServRedirectStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServRedirectStatusGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServRedirectStatusGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServRedirectStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServRedirectStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServRedirectStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServRedirectStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServRedirectStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServRedirectStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentDiffServMirrorStorageGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServMirrorStorageGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServMirrorStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServMirrorStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServMirrorStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServMirrorStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServMirrorStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServMirrorStatusGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServMirrorStatusGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServMirrorStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServMirrorStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServMirrorStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServMirrorStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServMirrorStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServMirrorStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_qos_diffserv_api.h
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
snmpDiffServTableRowPointer(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t rowPointer, L7_uchar8 *buf)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_uint = 0;

  switch (rowPointer.tableId)
  {
  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_NONE:
    sprintf(buf, "0.0");
    break;

  /* Private MIB tables */
  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLASS:
    sprintf(buf, "agentDiffServClassEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLASS_RULE:
    sprintf(buf, "agentDiffServClassRuleEntry.1.%d.%d", rowPointer.rowIndex1, rowPointer.rowIndex2);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY:
    sprintf(buf, "agentDiffServPolicyEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_INST:
    sprintf(buf, "agentDiffServPolicyInstEntry.1.%d.%d", rowPointer.rowIndex1, rowPointer.rowIndex2);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_ATTR:
    sprintf(buf, "agentDiffServPolicyAttrEntry.1.%d.%d.%d", rowPointer.rowIndex1, rowPointer.rowIndex2, rowPointer.rowIndex3);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_PERF_IN:
    sprintf(buf, "agentDiffServPolicyPerfInEntry.1.%d.%d.%d", rowPointer.rowIndex1, rowPointer.rowIndex2, rowPointer.rowIndex3);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_PERF_OUT:
    sprintf(buf, "agentDiffServPolicyPerfOutEntry.1.%d.%d.%d", rowPointer.rowIndex1, rowPointer.rowIndex2, rowPointer.rowIndex3);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_SERVICE:
    
    switch (rowPointer.rowIndex2)
    {
    case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN:
      temp_uint = D_agentDiffServServiceIfDirection_in;
      break;
    case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT:
      temp_uint = D_agentDiffServServiceIfDirection_out;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }

    if (rc == L7_SUCCESS)
      sprintf(buf, "agentDiffServServiceEntry.1.%d.%d", rowPointer.rowIndex1, temp_uint);

    break;

  /* Draft MIB tables */
  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_DATA_PATH:

    switch (rowPointer.rowIndex2)
    {
    case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN:
      temp_uint = D_agentDiffServServiceIfDirection_in;
      break;
    case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT:
      temp_uint = D_agentDiffServServiceIfDirection_out;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  
    if (rc == L7_SUCCESS)
      sprintf(buf, "diffServDataPath.1.%d.%d", rowPointer.rowIndex1, temp_uint);

    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLFR:
    sprintf(buf, "diffServClfrEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLFR_ELEMENT:
    sprintf(buf, "diffServClfrElementEntry.1.%d.%d", rowPointer.rowIndex1, rowPointer.rowIndex2);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_MULTI_FIELD_CLFR:
    sprintf(buf, "diffServMultiFieldClfrEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_AUX_MF_CLFR:
    sprintf(buf, "agentDiffServAuxMfClfrEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_METER:
    sprintf(buf, "diffServMeterEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_TB_PARAM:
    sprintf(buf, "diffServTBParamEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_ACTION:
    sprintf(buf, "diffServActionEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_COS_ACT:
    sprintf(buf, "agentDiffServCosMarkActEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_COS_AS_COS2_ACT:
    sprintf(buf, "agentDiffServCosAsCos2MarkActEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_IPDSCP_ACT:
    sprintf(buf, "diffServDscpMarkActEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_IPPREC_ACT:
    sprintf(buf, "agentDiffServIpPrecMarkActEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_COUNT_ACT:
    sprintf(buf, "diffServCountActEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_ALG_DROP:
    sprintf(buf, "diffServAlgDropEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_RANDOM_DROP:
    sprintf(buf, "diffServRandomDropEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_QUEUE:
    sprintf(buf, "diffServQEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_SCHEDULER:
    sprintf(buf, "diffServSchedulerEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_MIN_RATE:
    sprintf(buf, "diffServMinRateEntry.1.%d", rowPointer.rowIndex1);
    break;

  case L7_USMDB_MIB_DIFFSERV_TABLE_ID_MAX_RATE:
    sprintf(buf, "diffServMaxRateEntry.1.%d.%d", rowPointer.rowIndex1, rowPointer.rowIndex2);
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpDiffServDataPathGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 direction)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t temp_direction = 0;

  switch (direction)
  {
  case D_diffServDataPathIfDirection_inbound:
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
    break;

  case D_diffServDataPathIfDirection_outbound:
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServDataPathGet(UnitIndex, index, temp_direction);
  }

  return rc;
}

L7_RC_t
snmpDiffServDataPathGetNext(L7_uint32 UnitIndex, L7_uint32 *index, L7_uint32 *direction)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t temp_direction;

  /* find the right ifDirection starting value for the GetNext invocation */
  /* NOTE: assumes _DIRECTION_OUT > _DIRECTION_IN (don't use switch stmt for this) */
  if (*direction >= D_diffServDataPathIfDirection_outbound)
  {
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
  }
  else if (*direction >= D_diffServDataPathIfDirection_inbound)
  {
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
  }
  else
  {
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
  }

  rc = usmDbDiffServDataPathGetNext(UnitIndex, *index, temp_direction, index, &temp_direction);

  if (rc == L7_SUCCESS)
  {
    switch (temp_direction)
    {
    case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN:
      *direction = D_diffServDataPathIfDirection_inbound;
      break;

    case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT:
      *direction = D_diffServDataPathIfDirection_outbound;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServDataPathStartGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 direction, L7_uchar8 *buf)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t temp_direction = 0;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  switch (direction)
  {
  case D_diffServDataPathIfDirection_inbound:
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
    break;

  case D_diffServDataPathIfDirection_outbound:
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServDataPathStartGet(UnitIndex, index, temp_direction, &temp_val);

    if (rc == L7_SUCCESS)
    {
      rc = snmpDiffServTableRowPointer(temp_val,buf);
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServDataPathStorageTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 direction, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t temp_direction = 0;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  switch (direction)
  {
  case D_diffServDataPathIfDirection_inbound:
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
    break;

  case D_diffServDataPathIfDirection_outbound:
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServDataPathStorageGet(UnitIndex, index, temp_direction, &temp_val);

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
        *val = D_diffServDataPathStorage_other;
        break;

      case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
        *val = D_diffServDataPathStorage_volatile;
        break;

      case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
        *val = D_diffServDataPathStorage_nonVolatile;
        break;

      case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
        *val = D_diffServDataPathStorage_permanent;
        break;

      case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
        *val = D_diffServDataPathStorage_readOnly;
        break;

      default:
        rc = L7_FAILURE;
        break;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServDataPathRowStatusGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 direction, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t temp_direction = 0;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  switch (direction)
  {
  case D_diffServDataPathIfDirection_inbound:
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
    break;

  case D_diffServDataPathIfDirection_outbound:
    temp_direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServDataPathStatusGet(UnitIndex, index, temp_direction, &temp_val);
    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
        *val = D_diffServDataPathStatus_active;
        break;

      case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
        *val = D_diffServDataPathStatus_notInService;
        break;

      case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
        *val = D_diffServDataPathStatus_notReady;
        break;

      case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
        *val = D_diffServDataPathStatus_createAndGo;
        break;

      case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
        *val = D_diffServDataPathStatus_createAndWait;
        break;

      case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
        *val = D_diffServDataPathStatus_destroy;
        break;

      default:
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServClfrStorageGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServClfrStorageGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_diffServClfrStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_diffServClfrStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_diffServClfrStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_diffServClfrStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_diffServClfrStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServClfrStatusGet(L7_uint32 UnitIndex, L7_uint32 clfrId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServClfrStatusGet(UnitIndex, clfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_diffServClfrStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_diffServClfrStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_diffServClfrStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_diffServClfrStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_diffServClfrStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_diffServClfrStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServClfrElemNextGet(L7_uint32 UnitIndex, L7_uint32 ClfrId, L7_uint32 ClfrElementId, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServClfrElemNextGet(UnitIndex, ClfrId, ClfrElementId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServClfrElemSpecificGet(L7_uint32 UnitIndex, L7_uint32 ClfrId, L7_uint32 ClfrElementId, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServClfrElemSpecificGet(UnitIndex, ClfrId, ClfrElementId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServClfrElemStorageGet(L7_uint32 UnitIndex, L7_uint32 ClfrId, L7_uint32 ClfrElementId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServClfrElemStorageGet(UnitIndex, ClfrId, ClfrElementId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_diffServClfrElementStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_diffServClfrElementStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_diffServClfrElementStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_diffServClfrElementStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_diffServClfrElementStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServClfrElemStatusGet(L7_uint32 UnitIndex, L7_uint32 ClfrId, L7_uint32 ClfrElementId, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServClfrElemStatusGet(UnitIndex, ClfrId, ClfrElementId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_diffServClfrElementStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_diffServClfrElementStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_diffServClfrElementStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_diffServClfrElementStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_diffServClfrElementStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_diffServClfrElementStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServMFClfrAddrTypeGet(L7_uint32 UnitIndex, L7_uint32 mfClfrId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_t temp_val;

  rc = usmDbDiffServMFClfrAddrTypeGet(UnitIndex, mfClfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_IPV4:
      *val = D_diffServMultiFieldClfrAddrType_ipv4;
      break;

    case L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_IPV6:
      *val = D_diffServMultiFieldClfrAddrType_ipv6;
      break;

    default:
      *val = D_diffServMultiFieldClfrAddrType_unknown;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServMFClfrStorageGet(L7_uint32 UnitIndex, L7_uint32 mfClfrId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServMFClfrStorageGet(UnitIndex, mfClfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_diffServMultiFieldClfrStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_diffServMultiFieldClfrStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_diffServMultiFieldClfrStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_diffServMultiFieldClfrStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_diffServMultiFieldClfrStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServMFClfrStatusGet(L7_uint32 UnitIndex, L7_uint32 mfClfrId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServMFClfrStatusGet(UnitIndex, mfClfrId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_diffServMultiFieldClfrStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_diffServMultiFieldClfrStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_diffServMultiFieldClfrStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_diffServMultiFieldClfrStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_diffServMultiFieldClfrStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_diffServMultiFieldClfrStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServMeterSucceedNextGet(L7_uint32 UnitIndex, L7_uint32 meterId, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServMeterSucceedNextGet(UnitIndex, meterId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServMeterFailNextGet(L7_uint32 UnitIndex, L7_uint32 meterId, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServMeterFailNextGet(UnitIndex, meterId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServMeterSpecificGet(L7_uint32 UnitIndex, L7_uint32 meterId, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServMeterSpecificGet(UnitIndex, meterId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServMeterStorageGet(L7_uint32 UnitIndex, L7_uint32 meterId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServMeterStorageGet(UnitIndex, meterId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_diffServMeterStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_diffServMeterStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_diffServMeterStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_diffServMeterStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_diffServMeterStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServMeterStatusGet(L7_uint32 UnitIndex, L7_uint32 meterId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServMeterStatusGet(UnitIndex, meterId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_diffServMeterStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_diffServMeterStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_diffServMeterStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_diffServMeterStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_diffServMeterStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_diffServMeterStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServColorAwareModeGet(L7_uint32 UnitIndex, L7_uint32 meterId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_COLOR_MODE_t temp_val;

  rc = usmDbDiffServColorAwareModeGet(UnitIndex,meterId,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_COLOR_MODE_BLIND:
      *val = D_agentDiffServColorAwareMode_blind;
      break;

    case L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_COS:
      *val = D_agentDiffServColorAwareMode_awarecos;
      break;

    case L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_COS2:
      *val = D_agentDiffServColorAwareMode_awarecos2;
      break;

    case L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_IPDSCP:
      *val = D_agentDiffServColorAwareMode_awareipdscp;
      break;

    case L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_IPPREC:
      *val = D_agentDiffServColorAwareMode_awareipprec;
      break;
      
    case L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_UNUSED:
      *val = D_agentDiffServColorAwareMode_awareunused;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServColorAwareLevelGet(L7_uint32 UnitIndex, L7_uint32 meterId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_t temp_val;
  
  rc = usmDbDiffServColorAwareLevelGet(UnitIndex,meterId,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_CONFORM:
      *val = D_agentDiffServColorAwareLevel_conform;
      break;

    case L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_EXCEED:
      *val = D_agentDiffServColorAwareLevel_exceed;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpDiffServTBParamTypeGet(L7_uint32 UnitIndex, L7_uint32 tbParamId, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_t temp_val;

  rc = usmDbDiffServTBParamTypeGet(UnitIndex, tbParamId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_SIMPLE_TOKEN_BUCKET:
      sprintf(buf, "diffServTBParamSimpleTokenBucket.0");
      break;

    case L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_AVG_RATE:
      sprintf(buf, "diffServTBParamAvgRate.0");
      break;

    case L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_SRTCM_BLIND:
      sprintf(buf, "diffServTBParamSrTCMBlind.0");
      break;

    case L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_SRTCM_AWARE:
      sprintf(buf, "diffServTBParamSrTCMAware.0");
      break;

    case L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_TRTCM_BLIND:
      sprintf(buf, "diffServTBParamTrTCMBlind.0");
      break;

    case L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_TRTCM_AWARE:
      sprintf(buf, "diffServTBParamTrTCMAware.0");
      break;

    case L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_TSWTCM:
      sprintf(buf, "diffServTBParamTswTCM.0");
      break;

    default:
      sprintf(buf, "%d", temp_val);
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServTBParamStorageGet(L7_uint32 UnitIndex, L7_uint32 tbParamId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServTBParamStorageGet(UnitIndex, tbParamId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_diffServTBParamStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_diffServTBParamStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_diffServTBParamStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_diffServTBParamStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_diffServTBParamStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServTBParamStatusGet(L7_uint32 UnitIndex, L7_uint32 tbParamId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServTBParamStatusGet(UnitIndex, tbParamId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_diffServTBParamStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_diffServTBParamStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_diffServTBParamStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_diffServTBParamStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_diffServTBParamStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_diffServTBParamStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpDiffServActionNextGet(L7_uint32 UnitIndex, L7_uint32 actionId, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServActionNextGet(UnitIndex, actionId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServActionSpecificGet(L7_uint32 UnitIndex, L7_uint32 actionId, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServActionSpecificGet(UnitIndex, actionId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServActionStorageGet(L7_uint32 UnitIndex, L7_uint32 actionId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServActionStorageGet(UnitIndex, actionId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_diffServActionStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_diffServActionStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_diffServActionStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_diffServActionStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_diffServActionStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServActionStatusGet(L7_uint32 UnitIndex, L7_uint32 actionId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServActionStatusGet(UnitIndex, actionId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_diffServActionStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_diffServActionStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_diffServActionStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_diffServActionStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_diffServActionStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_diffServActionStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServCountActStorageGet(L7_uint32 UnitIndex, L7_uint32 countActId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServCountActStorageGet(UnitIndex, countActId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_diffServCountActStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_diffServCountActStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_diffServCountActStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_diffServCountActStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_diffServCountActStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServCountActStatusGet(L7_uint32 UnitIndex, L7_uint32 countActId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServCountActStatusGet(UnitIndex, countActId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_diffServCountActStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_diffServCountActStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_diffServCountActStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_diffServCountActStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_diffServCountActStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_diffServCountActStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpDiffServAlgDropTypeGet(L7_uint32 UnitIndex, L7_uint32 algDropId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_t temp_val;

  rc = usmDbDiffServAlgDropTypeGet(UnitIndex, algDropId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_OTHER:
      *val = D_diffServAlgDropType_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_TAIL_DROP:
      *val = D_diffServAlgDropType_tailDrop;
      break;

    case L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_HEAD_DROP:
      *val = D_diffServAlgDropType_headDrop;
      break;

    case L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_RANDOM_DROP:
      *val = D_diffServAlgDropType_randomDrop;
      break;

    case L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_ALWAYS_DROP:
      *val = D_diffServAlgDropType_alwaysDrop;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServAlgDropNextGet(L7_uint32 UnitIndex, L7_uint32 algDropId, L7_char8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServAlgDropNextGet(UnitIndex, algDropId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServAlgDropQMeasureGet(L7_uint32 UnitIndex, L7_uint32 algDropId, L7_char8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServAlgDropQMeasureGet(UnitIndex, algDropId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServAlgDropSpecificGet(L7_uint32 UnitIndex, L7_uint32 algDropId, L7_char8 *buf)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t temp_val;

  rc = usmDbDiffServAlgDropSpecificGet(UnitIndex, algDropId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = snmpDiffServTableRowPointer(temp_val,buf);
  }

  return rc;
}

L7_RC_t
snmpDiffServAlgDropStorageGet(L7_uint32 UnitIndex, L7_uint32 algDropId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServAlgDropStorageGet(UnitIndex, algDropId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_diffServAlgDropStorage_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_diffServAlgDropStorage_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_diffServAlgDropStorage_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_diffServAlgDropStorage_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_diffServAlgDropStorage_readOnly;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServAlgDropStatusGet(L7_uint32 UnitIndex, L7_uint32 algDropId, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServAlgDropStatusGet(UnitIndex, algDropId, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_diffServAlgDropStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_diffServAlgDropStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_diffServAlgDropStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_diffServAlgDropStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_diffServAlgDropStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_diffServAlgDropStatus_destroy;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**************************************************************************************************************/

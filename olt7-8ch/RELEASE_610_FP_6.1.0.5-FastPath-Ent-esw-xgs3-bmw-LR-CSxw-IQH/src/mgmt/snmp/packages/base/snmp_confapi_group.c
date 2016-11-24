
#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "v2clssc.h"
#include "lookup.h"
#include "v2table.h"            /* for SnmpV2Table */
#include "pw2key.h"             /* for SrTextPasswordToLocalizedKey */
#include "snmpid.h"             /* for SrGetSnmpID  */
#include "snmpv3/v3type.h"      /* for snmpTargetAddrEntry_t */
#include "scan.h"
#include "diag.h"
#include "sr_ip.h"		/* for ConvToken_IP_TAddr */
#include "snmpv3/i_usm.h"       /* for usmUserEntryTypeTable */
#include "snmpv3/v3defs.h"      /* for usmUserTable constant macros */
#include "snmpv3/v3supp.h"      /* for usmUserTable constant macros */
#include "snmpv3/i_tgt.h"       /* for snmpTargetParamsEntryTypeTable */
#include "snmpv3/i_notif.h"     /* for snmpNotifyEntryTypeTable */
#include "coex/i_comm.h"        /* for snmpCommunityEntryTypeTable */
#include "coex/coextype.h"      /* for snmpCommunityEntry_t definition */
#include "coex/coexsupp.h"      /* for I_snmpCommunity* definition */
#include "coex/coexdefs.h"      /* for snmpCommunityTable constant macros */
#include "context.h"
#include "method.h"

#include "l7_common.h"
#include "snmp_exports.h"
#include "sysapi.h"
#include "snmp_confapi.h"

extern SnmpV2Table agt_vacmSecurityToGroupTable;     /* vacmSecurityToGroupEntry_t */

extern int writeConfigFileFlag;

/*******************************************************************************************/

/*
        SR_INT32        vacmSecurityModel;
        OctetString     *vacmSecurityName;
        OctetString     *vacmGroupName;
        SR_INT32        vacmSecurityToGroupStorageType;
        SR_INT32        vacmSecurityToGroupStatus;
*/

/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to vacmSecurityToGroupEntry_t object
*           
* @notes This table maps a combination of securityModel and
*        securityName into a groupName which is used to define
*        an access control policy for a group of principals.
*
* @end
*
*********************************************************************/
static vacmSecurityToGroupEntry_t* snmp_vacmSecurityToGroupTable_get(int searchType, l7_snmpSecurityModel_t vacmSecurityModel, L7_char8 *vacmSecurityName)
{
  L7_int32 index = -1;
  vacmSecurityToGroupEntry_t *row = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || vacmSecurityName == NULL)
    return NULL;

  /* build the search structure */
  agt_vacmSecurityToGroupTable.tip[0].value.uint_val = vacmSecurityModel;
  agt_vacmSecurityToGroupTable.tip[1].value.octet_val = MakeOctetStringFromText(vacmSecurityName);
  if (agt_vacmSecurityToGroupTable.tip[1].value.octet_val == NULL)
  {
    return NULL;
  }

  index = SearchTable(&agt_vacmSecurityToGroupTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (vacmSecurityToGroupEntry_t*)agt_vacmSecurityToGroupTable.tp[index];

    if (searchType == NEXT &&
        vacmSecurityModel == row->vacmSecurityModel &&
        strncmp(vacmSecurityName, 
                row->vacmSecurityName->octet_ptr,
                row->vacmSecurityName->length) == 0)
    index = SearchTable(&agt_vacmSecurityToGroupTable, NEXT_SKIP);
  }


  FreeOctetString(agt_vacmSecurityToGroupTable.tip[1].value.octet_val);

  if (index >= 0)
  {
    return(vacmSecurityToGroupEntry_t*)agt_vacmSecurityToGroupTable.tp[index];
  }

  return NULL;
}

extern OctetString *agt_local_snmpID; 

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  NO_ERROR                     if defaults were set
*           RESOURCE_UNAVAILABLE_ERROR   if memory was not sufficient
*           
* @end
*
*********************************************************************/
static int l7_vacmSecurityToGroupEntry_set_defaults(vacmSecurityToGroupEntry_t *data)
{
  if ((data->vacmGroupName = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->vacmSecurityToGroupStorageType = D_vacmSecurityToGroupStorageType_nonVolatile;
  data->RowStatusTimerId = -1;


  SET_ALL_VALID(data->valid);
  /*
   * clear valid bits for non-index objects that do not have
   * a DEFVAL clause
   */
  CLR_VALID(I_vacmGroupName, data->valid);
  CLR_VALID(I_vacmSecurityToGroupStatus, data->valid);
  return NO_ERROR;
}

/*********************************************************************
*
* @purpose Sets the row associated with the index for the given request
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*           
* @notes This table maps a combination of securityModel and
*        securityName into a groupName which is used to define
*        an access control policy for a group of principals.
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_vacmSecurityToGroupTable_set(vacmSecurityToGroupEntry_t *data)
{
  L7_int32 index = -1;
  vacmSecurityToGroupEntry_t * vacmSecurityToGroupEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_vacmSecurityModel, data->valid) ||
      !VALID(I_vacmSecurityName, data->valid))
    return L7_ERROR;

  /* build the search structure */
  agt_vacmSecurityToGroupTable.tip[0].value.uint_val = data->vacmSecurityModel;
  agt_vacmSecurityToGroupTable.tip[1].value.octet_val = CloneOctetString(data->vacmSecurityName);
  if (agt_vacmSecurityToGroupTable.tip[1].value.octet_val == NULL)
  {
    return L7_ERROR;
  }

  index = SearchTable(&agt_vacmSecurityToGroupTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_vacmSecurityToGroupStatus, data->valid) && 
        (data->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_createAndGo ||
         data->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_createAndWait))
    {
      index = NewTableEntry(&agt_vacmSecurityToGroupTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_vacmSecurityToGroupEntry_set_defaults((vacmSecurityToGroupEntry_t*)agt_vacmSecurityToGroupTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_createAndGo)
      {
        ((vacmSecurityToGroupEntry_t*)agt_vacmSecurityToGroupTable.tp[index])->vacmSecurityToGroupStatus = D_vacmSecurityToGroupStatus_active;
      }
      else
      {
        ((vacmSecurityToGroupEntry_t*)agt_vacmSecurityToGroupTable.tp[index])->vacmSecurityToGroupStatus = D_vacmSecurityToGroupStatus_notInService;
      }
      SET_VALID(I_vacmSecurityToGroupStatus, ((vacmSecurityToGroupEntry_t*)agt_vacmSecurityToGroupTable.tp[index])->valid);

      CLR_VALID(I_vacmSecurityToGroupStatus, data->valid);
    }
  }

  FreeOctetString(agt_vacmSecurityToGroupTable.tip[1].value.octet_val);

  if (index >= 0)
  {
    if (VALID(I_vacmSecurityToGroupStatus, data->valid) && 
        data->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_destroy)
    {
      RemoveTableEntry(&agt_vacmSecurityToGroupTable, index);
      return L7_SUCCESS;
    }

    vacmSecurityToGroupEntryData = (vacmSecurityToGroupEntry_t*)agt_vacmSecurityToGroupTable.tp[index];

    if (VALID(I_vacmGroupName, data->valid))
    {
      if (vacmSecurityToGroupEntryData->vacmGroupName != NULL)
      {
        FreeOctetString(vacmSecurityToGroupEntryData->vacmGroupName);
        CLR_VALID(I_vacmGroupName, vacmSecurityToGroupEntryData->valid);
      }

      vacmSecurityToGroupEntryData->vacmGroupName = CloneOctetString(data->vacmGroupName);

      if (vacmSecurityToGroupEntryData->vacmGroupName != NULL)
      {
        SET_VALID(I_vacmGroupName, vacmSecurityToGroupEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_vacmSecurityToGroupStorageType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(vacmSecurityToGroupEntryData->vacmSecurityToGroupStorageType,
                                        data->vacmSecurityToGroupStorageType) != L7_SUCCESS)
        return L7_ERROR;

      vacmSecurityToGroupEntryData->vacmSecurityToGroupStorageType = data->vacmSecurityToGroupStorageType;
      SET_VALID(I_vacmSecurityToGroupStorageType, vacmSecurityToGroupEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_vacmSecurityToGroupStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(vacmSecurityToGroupEntryData->vacmSecurityToGroupStatus,
                                      data->vacmSecurityToGroupStatus) != L7_SUCCESS)
        return L7_ERROR;

      vacmSecurityToGroupEntryData->vacmSecurityToGroupStatus = data->vacmSecurityToGroupStatus;
      SET_VALID(I_vacmSecurityToGroupStatus, vacmSecurityToGroupEntryData->valid);
      writeConfigFileFlag = TRUE;
    }
  }
  else
  {
    return L7_ERROR;
  }

  return rc;
}

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t l7_vacmSecurityToGroupTableCheckValid(l7_snmpSecurityModel_t vacmSecurityModel, L7_char8 *vacmSecurityName)
{
  vacmSecurityToGroupEntry_t * vacmSecurityToGroupEntryData = snmp_vacmSecurityToGroupTable_get(EXACT, vacmSecurityModel, vacmSecurityName);

  if (vacmSecurityToGroupEntryData != NULL)
    return L7_SUCCESS;
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
#if 1
L7_RC_t l7_vacmSecurityToGroupTableNextGet(L7_uint32 *vacmSecurityModel, L7_char8 *vacmSecurityName)
#else
L7_RC_t l7_vacmSecurityToGroupTableNextGet(l7_snmpSecurityModel_t *vacmSecurityModel, L7_char8 *vacmSecurityName)
#endif
{
  vacmSecurityToGroupEntry_t * vacmSecurityToGroupEntryData = snmp_vacmSecurityToGroupTable_get(NEXT, *vacmSecurityModel, vacmSecurityName);

  if (vacmSecurityToGroupEntryData != NULL)
  {
    *vacmSecurityModel = vacmSecurityToGroupEntryData->vacmSecurityModel;
    strncpy(vacmSecurityName, vacmSecurityToGroupEntryData->vacmSecurityName->octet_ptr,
            vacmSecurityToGroupEntryData->vacmSecurityName->length);
    vacmSecurityName[vacmSecurityToGroupEntryData->vacmSecurityName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmGroupName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The name of the group to which this entry (e.g., the
*        combination of securityModel and securityName)
*        belongs.
*        
*        This groupName is used as index into the
*        vacmAccessTable to select an access control policy.
*        However, a value in this table does not imply that an
*        instance with the value exists in table vacmAccesTable.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmGroupNameGet(l7_snmpSecurityModel_t vacmSecurityModel, L7_char8 *vacmSecurityName, L7_char8 *val)
{
  vacmSecurityToGroupEntry_t * vacmSecurityToGroupEntryData = snmp_vacmSecurityToGroupTable_get(EXACT, vacmSecurityModel, vacmSecurityName);

  if (vacmSecurityToGroupEntryData != NULL && 
      VALID(I_vacmGroupName, vacmSecurityToGroupEntryData->valid))
  {
    strncpy(val, vacmSecurityToGroupEntryData->vacmGroupName->octet_ptr,
            vacmSecurityToGroupEntryData->vacmGroupName->length);
    val[vacmSecurityToGroupEntryData->vacmGroupName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmGroupName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The name of the group to which this entry (e.g., the
*        combination of securityModel and securityName)
*        belongs.
*        
*        This groupName is used as index into the
*        vacmAccessTable to select an access control policy.
*        However, a value in this table does not imply that an
*        instance with the value exists in table vacmAccesTable.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmGroupNameSet(l7_snmpSecurityModel_t vacmSecurityModel, L7_char8 *vacmSecurityName, L7_char8 *val)
{
  vacmSecurityToGroupEntry_t vacmSecurityToGroupEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 1, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&vacmSecurityToGroupEntryData, 0, sizeof(vacmSecurityToGroupEntryData));

  vacmSecurityToGroupEntryData.vacmSecurityModel = vacmSecurityModel;
  SET_VALID(I_vacmSecurityModel, vacmSecurityToGroupEntryData.valid);
  vacmSecurityToGroupEntryData.vacmSecurityName = MakeOctetStringFromText(vacmSecurityName);
  SET_VALID(I_vacmSecurityName, vacmSecurityToGroupEntryData.valid);

  vacmSecurityToGroupEntryData.vacmGroupName = MakeOctetStringFromText(val);
  SET_VALID(I_vacmGroupName, vacmSecurityToGroupEntryData.valid);

  rc = snmp_vacmSecurityToGroupTable_set(&vacmSecurityToGroupEntryData);

  FreeOctetString(vacmSecurityToGroupEntryData.vacmSecurityName);
  FreeOctetString(vacmSecurityToGroupEntryData.vacmGroupName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmSecurityToGroupStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmSecurityToGroupStorageTypeGet(l7_snmpSecurityModel_t vacmSecurityModel, L7_char8 *vacmSecurityName, l7_snmpStorageType_t *val)
{
  vacmSecurityToGroupEntry_t * vacmSecurityToGroupEntryData = snmp_vacmSecurityToGroupTable_get(EXACT, vacmSecurityModel, vacmSecurityName);

  if (vacmSecurityToGroupEntryData != NULL && 
      VALID(I_vacmSecurityToGroupStorageType, vacmSecurityToGroupEntryData->valid))
  {
    *val = vacmSecurityToGroupEntryData->vacmSecurityToGroupStorageType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmSecurityToGroupStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmSecurityToGroupStorageTypeSet(l7_snmpSecurityModel_t vacmSecurityModel, L7_char8 *vacmSecurityName, l7_snmpStorageType_t val)
{
  vacmSecurityToGroupEntry_t vacmSecurityToGroupEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&vacmSecurityToGroupEntryData, 0, sizeof(vacmSecurityToGroupEntryData));

  vacmSecurityToGroupEntryData.vacmSecurityModel = vacmSecurityModel;
  SET_VALID(I_vacmSecurityModel, vacmSecurityToGroupEntryData.valid);
  vacmSecurityToGroupEntryData.vacmSecurityName = MakeOctetStringFromText(vacmSecurityName);
  SET_VALID(I_vacmSecurityName, vacmSecurityToGroupEntryData.valid);

  vacmSecurityToGroupEntryData.vacmSecurityToGroupStorageType = val;
  SET_VALID(I_vacmSecurityToGroupStorageType, vacmSecurityToGroupEntryData.valid);

  rc = snmp_vacmSecurityToGroupTable_set(&vacmSecurityToGroupEntryData);

  FreeOctetString(vacmSecurityToGroupEntryData.vacmSecurityName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmSecurityToGroupStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the vacmSecurityToGroupStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until a value has been set for vacmGroupName.
*        
*        The  RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmSecurityToGroupStatusGet(l7_snmpSecurityModel_t vacmSecurityModel, L7_char8 *vacmSecurityName, l7_snmpRowStatus_t *val)
{
  vacmSecurityToGroupEntry_t * vacmSecurityToGroupEntryData = snmp_vacmSecurityToGroupTable_get(EXACT, vacmSecurityModel, vacmSecurityName);

  if (vacmSecurityToGroupEntryData != NULL && 
      VALID(I_vacmSecurityToGroupStatus, vacmSecurityToGroupEntryData->valid))
  {
    *val = vacmSecurityToGroupEntryData->vacmSecurityToGroupStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmSecurityToGroupStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the vacmSecurityToGroupStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until a value has been set for vacmGroupName.
*        
*        The  RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmSecurityToGroupStatusSet(l7_snmpSecurityModel_t vacmSecurityModel, L7_char8 *vacmSecurityName, l7_snmpRowStatus_t val)
{
  vacmSecurityToGroupEntry_t vacmSecurityToGroupEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&vacmSecurityToGroupEntryData, 0, sizeof(vacmSecurityToGroupEntryData));

  if (vacmSecurityModel < 1 || 
      vacmSecurityModel > 2147483647 ||
      l7_snmpStringSetValidate(vacmSecurityName, 1, 32) != L7_SUCCESS)
    return rc;

  vacmSecurityToGroupEntryData.vacmSecurityModel = vacmSecurityModel;
  SET_VALID(I_vacmSecurityModel, vacmSecurityToGroupEntryData.valid);
  vacmSecurityToGroupEntryData.vacmSecurityName = MakeOctetStringFromText(vacmSecurityName);
  SET_VALID(I_vacmSecurityName, vacmSecurityToGroupEntryData.valid);

  vacmSecurityToGroupEntryData.vacmSecurityToGroupStatus = val;
  SET_VALID(I_vacmSecurityToGroupStatus, vacmSecurityToGroupEntryData.valid);

  rc = snmp_vacmSecurityToGroupTable_set(&vacmSecurityToGroupEntryData);

  FreeOctetString(vacmSecurityToGroupEntryData.vacmSecurityName);

  return rc;
}

void
debug_vacmSecurityToGroupTable_walk()
{
  l7_snmpSecurityModel_t vacmSecurityModel=0;
  L7_char8 vacmSecurityName[256];
  L7_char8 temp_string[256];
  L7_uint32 temp_val;

  memset((void*)vacmSecurityName, 0, sizeof(vacmSecurityName));

  sysapiPrintf("[vacmSecurityToGroupTable start]\n");

  while(l7_vacmSecurityToGroupTableNextGet(&vacmSecurityModel, vacmSecurityName) == L7_SUCCESS)
  {
    sysapiPrintf("vacmSecurityModel.%d.\"%s\" = %d\n", vacmSecurityModel, vacmSecurityName, vacmSecurityModel);
    sysapiPrintf("vacmSecurityName.%d.\"%s\" = \"%s\"\n", vacmSecurityModel, vacmSecurityName, vacmSecurityName);

    if (l7_vacmGroupNameGet(vacmSecurityModel, vacmSecurityName, temp_string) == L7_SUCCESS)
      sysapiPrintf("vacmGroupName.%d.\"%s\" = \"%s\"\n", vacmSecurityModel, vacmSecurityName, temp_string);
    else
      sysapiPrintf("vacmGroupName.%d.\"%s\" = L7_FAILURE\n", vacmSecurityModel, vacmSecurityName);

    if (l7_vacmSecurityToGroupStorageTypeGet(vacmSecurityModel, vacmSecurityName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("vacmSecurityToGroupStorageType.%d.\"%s\" = %d\n", vacmSecurityModel, vacmSecurityName, temp_val);
    else
      sysapiPrintf("vacmSecurityToGroupStorageType.%d.\"%s\" = L7_FAILURE\n", vacmSecurityModel, vacmSecurityName);

    if (l7_vacmSecurityToGroupStatusGet(vacmSecurityModel, vacmSecurityName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("vacmSecurityToGroupStatus.%d.\"%s\" = %d\n", vacmSecurityModel, vacmSecurityName, temp_val);
    else
      sysapiPrintf("vacmSecurityToGroupStatus.%d.\"%s\" = L7_FAILURE\n", vacmSecurityModel, vacmSecurityName);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_vacmSecurityToGroupTable_create(l7_snmpSecurityModel_t vacmSecurityModel, L7_char8 *vacmSecurityName, L7_char8 *vacmGroupName)
{
  L7_RC_t rc;

  rc = l7_vacmSecurityToGroupStatusSet(vacmSecurityModel, vacmSecurityName, snmpRowStatus_createAndWait);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmSecurityToGroupStatusSet 1 failed!\n");
    return;
  }

  rc = l7_vacmGroupNameSet(vacmSecurityModel, vacmSecurityName, vacmGroupName);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmGroupNameSet 1 failed!\n");
    return;
  }

  rc = l7_vacmSecurityToGroupStatusSet(vacmSecurityModel, vacmSecurityName, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmSecurityToGroupStatusSet 2 failed!\n");
    return;
  }
  sysapiPrintf("debug_vacmSecurityToGroupTable_create finished!\n");
}

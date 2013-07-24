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
#include "coex/i_comm.h"        /* for snmpNotifyFilterProfileEntryTypeTable */
#include "coex/coextype.h"      /* for snmpNotifyFilterProfileEntry_t definition */
#include "coex/coexsupp.h"      /* for I_snmpNotifyFilterProfile* definition */
#include "coex/coexdefs.h"      /* for snmpNotifyFilterProfileTable constant macros */
#include "context.h"
#include "method.h"
#include "oid_lib.h"
#include "frmt_lib.h"

#include "l7_common.h"
#include "snmp_exports.h"
#include "sysapi.h"
#include "snmp_confapi.h"

extern SnmpV2Table agt_snmpNotifyFilterProfileTable; /* snmpNotifyFilterProfileEntry_t */
extern SnmpV2Table agt_snmpNotifyFilterTable;        /* snmpNotifyFilterEntry_t */
extern SnmpV2Table agt_snmpNotifyTable;              /* snmpNotifyEntry_t */

extern int writeConfigFileFlag;

/*******************************************************************************************/

/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to snmpNotifyFilterProfileEntry_t object
*           
* @notes This table is used to associate a notification filter
*        profile with a particular set of target parameters.
*
* @end
*
*********************************************************************/
static snmpNotifyFilterProfileEntry_t* snmp_snmpNotifyFilterProfileTable_get(int searchType, L7_char8 *snmpTargetParamsName)
{
  L7_int32 index = -1;
  snmpNotifyFilterProfileEntry_t *row = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || snmpTargetParamsName == NULL)
    return NULL;

  /* build the search structure */
  agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val = MakeOctetStringFromText(snmpTargetParamsName);
  if (agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val == NULL)
  {
    return NULL;
  }

  index = SearchTable(&agt_snmpNotifyFilterProfileTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (snmpNotifyFilterProfileEntry_t*)agt_snmpNotifyFilterProfileTable.tp[index];

    if (searchType == NEXT &&
        strncmp(snmpTargetParamsName, 
                row->snmpTargetParamsName->octet_ptr,
                row->snmpTargetParamsName->length) == 0)
    index = SearchTable(&agt_snmpNotifyFilterProfileTable, NEXT_SKIP);
  }

  FreeOctetString(agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    return(snmpNotifyFilterProfileEntry_t*)agt_snmpNotifyFilterProfileTable.tp[index];
  }

  return NULL;
}

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
static int l7_snmpNotifyFilterProfileEntry_set_defaults(snmpNotifyFilterProfileEntry_t *data)
{
    if ((data->snmpNotifyFilterProfileName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->snmpNotifyFilterProfileStorType = 
        D_snmpNotifyFilterProfileStorType_nonVolatile;
    data->RowStatusTimerId = -1;


    SET_ALL_VALID(data->valid);
    /*
     * clear valid bits for non-index objects that do not have
     * a DEFVAL clause
     */
    CLR_VALID(I_snmpNotifyFilterProfileName, data->valid);
    CLR_VALID(I_snmpNotifyFilterProfileRowStatus, data->valid);
    return NO_ERROR;
}

/*********************************************************************
*
* @purpose Sets the row associated with the index for the given request
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*           
* @notes This table is used to associate a notification filter
*        profile with a particular set of target parameters.
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_snmpNotifyFilterProfileTable_set(snmpNotifyFilterProfileEntry_t *data)
{
  L7_int32 index = -1;
  snmpNotifyFilterProfileEntry_t * snmpNotifyFilterProfileEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName, data->valid))
    return L7_ERROR;

  /* build the search structure */
  agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val = CloneOctetString(data->snmpTargetParamsName);
  if (agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val == NULL)
  {
    return L7_ERROR;
  }

  index = SearchTable(&agt_snmpNotifyFilterProfileTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_snmpNotifyFilterProfileRowStatus, data->valid) && 
        (data->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_createAndGo ||
         data->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_createAndWait))
    {
      index = NewTableEntry(&agt_snmpNotifyFilterProfileTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_snmpNotifyFilterProfileEntry_set_defaults((snmpNotifyFilterProfileEntry_t*)agt_snmpNotifyFilterProfileTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_createAndGo)
      {
        ((snmpNotifyFilterProfileEntry_t*)agt_snmpNotifyFilterProfileTable.tp[index])->snmpNotifyFilterProfileRowStatus = D_snmpNotifyFilterProfileRowStatus_active;
      }
      else
      {
        ((snmpNotifyFilterProfileEntry_t*)agt_snmpNotifyFilterProfileTable.tp[index])->snmpNotifyFilterProfileRowStatus = D_snmpNotifyFilterProfileRowStatus_notInService;
      }
      SET_VALID(I_snmpNotifyFilterProfileRowStatus, ((snmpNotifyFilterProfileEntry_t*)agt_snmpNotifyFilterProfileTable.tp[index])->valid);

      CLR_VALID(I_snmpNotifyFilterProfileRowStatus, data->valid);
    }
  }

  FreeOctetString(agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    if (VALID(I_snmpNotifyFilterProfileRowStatus, data->valid) && 
        data->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_destroy)
    {
      RemoveTableEntry(&agt_snmpNotifyFilterProfileTable, index);
      return L7_SUCCESS;
    }

    snmpNotifyFilterProfileEntryData = (snmpNotifyFilterProfileEntry_t*)agt_snmpNotifyFilterProfileTable.tp[index];

    if (VALID(I_snmpNotifyFilterProfileName, data->valid))
    {
      if (snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileName != NULL)
      {
        FreeOctetString(snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileName);
        CLR_VALID(I_snmpNotifyFilterProfileName, snmpNotifyFilterProfileEntryData->valid);
      }

      snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileName = CloneOctetString(data->snmpNotifyFilterProfileName);

      if (snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileName != NULL)
      {
        SET_VALID(I_snmpNotifyFilterProfileName, snmpNotifyFilterProfileEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpNotifyFilterProfileStorType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileStorType,
                                        data->snmpNotifyFilterProfileStorType) != L7_SUCCESS)
        return L7_ERROR;

      snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileStorType = data->snmpNotifyFilterProfileStorType;
      SET_VALID(I_snmpNotifyFilterProfileStorType, snmpNotifyFilterProfileEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpNotifyFilterProfileRowStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileRowStatus,
                                      data->snmpNotifyFilterProfileRowStatus) != L7_SUCCESS)
        return L7_ERROR;

      snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileRowStatus = data->snmpNotifyFilterProfileRowStatus;
      SET_VALID(I_snmpNotifyFilterProfileRowStatus, snmpNotifyFilterProfileEntryData->valid);
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
L7_RC_t l7_snmpNotifyFilterProfileTableCheckValid(L7_char8 *snmpTargetParamsName)
{
  snmpNotifyFilterProfileEntry_t * snmpNotifyFilterProfileEntryData = snmp_snmpNotifyFilterProfileTable_get(EXACT, snmpTargetParamsName);

  if (snmpNotifyFilterProfileEntryData != NULL)
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
L7_RC_t l7_snmpNotifyFilterProfileTableNextGet(L7_char8 *snmpTargetParamsName)
{
  snmpNotifyFilterProfileEntry_t * snmpNotifyFilterProfileEntryData = snmp_snmpNotifyFilterProfileTable_get(NEXT, snmpTargetParamsName);

  if (snmpNotifyFilterProfileEntryData != NULL)
  {
    strncpy(snmpTargetParamsName, snmpNotifyFilterProfileEntryData->snmpTargetParamsName->octet_ptr,
            snmpNotifyFilterProfileEntryData->snmpTargetParamsName->length);
    snmpTargetParamsName[snmpNotifyFilterProfileEntryData->snmpTargetParamsName->length] = '\0';

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The name of the filter profile to be used when generating
*        notifications using the corresponding entry in the
*        snmpTargetAddrTable.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterProfileNameGet(L7_char8 *snmpTargetParamsName, L7_char8 *val)
{
  snmpNotifyFilterProfileEntry_t * snmpNotifyFilterProfileEntryData = snmp_snmpNotifyFilterProfileTable_get(EXACT, snmpTargetParamsName);

  if (snmpNotifyFilterProfileEntryData != NULL && 
      VALID(I_snmpNotifyFilterProfileName, snmpNotifyFilterProfileEntryData->valid))
  {
    strncpy(val, snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileName->octet_ptr,
            snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileName->length);
    val[snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The name of the filter profile to be used when generating
*        notifications using the corresponding entry in the
*        snmpTargetAddrTable.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterProfileNameSet(L7_char8 *snmpTargetParamsName, L7_char8 *val)
{
  snmpNotifyFilterProfileEntry_t snmpNotifyFilterProfileEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpNotifyFilterProfileEntryData, 0, sizeof(snmpNotifyFilterProfileEntryData));

  snmpNotifyFilterProfileEntryData.snmpTargetParamsName = MakeOctetStringFromText(snmpTargetParamsName);
  SET_VALID(I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName, snmpNotifyFilterProfileEntryData.valid);

  snmpNotifyFilterProfileEntryData.snmpNotifyFilterProfileName = MakeOctetStringFromText(val);
  SET_VALID(I_snmpNotifyFilterProfileName, snmpNotifyFilterProfileEntryData.valid);

  rc = snmp_snmpNotifyFilterProfileTable_set(&snmpNotifyFilterProfileEntryData);

  FreeOctetString(snmpNotifyFilterProfileEntryData.snmpTargetParamsName);
  FreeOctetString(snmpNotifyFilterProfileEntryData.snmpNotifyFilterProfileName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileStorType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type of this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterProfileStorTypeGet(L7_char8 *snmpTargetParamsName, l7_snmpStorageType_t *val)
{
  snmpNotifyFilterProfileEntry_t * snmpNotifyFilterProfileEntryData = snmp_snmpNotifyFilterProfileTable_get(EXACT, snmpTargetParamsName);

  if (snmpNotifyFilterProfileEntryData != NULL && 
      VALID(I_snmpNotifyFilterProfileStorType, snmpNotifyFilterProfileEntryData->valid))
  {
    *val = snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileStorType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileStorType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type of this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterProfileStorTypeSet(L7_char8 *snmpTargetParamsName, l7_snmpStorageType_t val)
{
  snmpNotifyFilterProfileEntry_t snmpNotifyFilterProfileEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpNotifyFilterProfileEntryData, 0, sizeof(snmpNotifyFilterProfileEntryData));

  snmpNotifyFilterProfileEntryData.snmpTargetParamsName = MakeOctetStringFromText(snmpTargetParamsName);
  SET_VALID(I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName, snmpNotifyFilterProfileEntryData.valid);

  snmpNotifyFilterProfileEntryData.snmpNotifyFilterProfileStorType = val;
  SET_VALID(I_snmpNotifyFilterProfileStorType, snmpNotifyFilterProfileEntryData.valid);

  rc = snmp_snmpNotifyFilterProfileTable_set(&snmpNotifyFilterProfileEntryData);

  FreeOctetString(snmpNotifyFilterProfileEntryData.snmpTargetParamsName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the
*        snmpNotifyFilterProfileRowStatus column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding instance of
*        snmpNotifyFilterProfileName has been set.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterProfileRowStatusGet(L7_char8 *snmpTargetParamsName, l7_snmpRowStatus_t *val)
{
  snmpNotifyFilterProfileEntry_t * snmpNotifyFilterProfileEntryData = snmp_snmpNotifyFilterProfileTable_get(EXACT, snmpTargetParamsName);

  if (snmpNotifyFilterProfileEntryData != NULL && 
      VALID(I_snmpNotifyFilterProfileRowStatus, snmpNotifyFilterProfileEntryData->valid))
  {
    *val = snmpNotifyFilterProfileEntryData->snmpNotifyFilterProfileRowStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the
*        snmpNotifyFilterProfileRowStatus column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding instance of
*        snmpNotifyFilterProfileName has been set.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterProfileRowStatusSet(L7_char8 *snmpTargetParamsName, l7_snmpRowStatus_t val)
{
  snmpNotifyFilterProfileEntry_t snmpNotifyFilterProfileEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpNotifyFilterProfileEntryData, 0, sizeof(snmpNotifyFilterProfileEntryData));

  if (l7_snmpStringSetValidate(snmpTargetParamsName, 1, 32) != L7_SUCCESS)
    return rc;

  snmpNotifyFilterProfileEntryData.snmpTargetParamsName = MakeOctetStringFromText(snmpTargetParamsName);
  SET_VALID(I_snmpNotifyFilterProfileEntryIndex_snmpTargetParamsName, snmpNotifyFilterProfileEntryData.valid);

  snmpNotifyFilterProfileEntryData.snmpNotifyFilterProfileRowStatus = val;
  SET_VALID(I_snmpNotifyFilterProfileRowStatus, snmpNotifyFilterProfileEntryData.valid);

  rc = snmp_snmpNotifyFilterProfileTable_set(&snmpNotifyFilterProfileEntryData);

  FreeOctetString(snmpNotifyFilterProfileEntryData.snmpTargetParamsName);

  return rc;
}

void
debug_snmpNotifyFilterProfileTable_walk()
{
  L7_char8 snmpTargetParamsName[256];
  L7_char8 temp_string[256];
  L7_uint32 temp_val;

  memset((void*)snmpTargetParamsName, 0, sizeof(snmpTargetParamsName));

  sysapiPrintf("[snmpNotifyFilterProfileTable start]\n");

  while(l7_snmpNotifyFilterProfileTableNextGet(snmpTargetParamsName) == L7_SUCCESS)
  {
    sysapiPrintf("snmpTargetParamsName.\"%s\" = \"%s\"\n", snmpTargetParamsName, snmpTargetParamsName);

    if (l7_snmpNotifyFilterProfileNameGet(snmpTargetParamsName, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpNotifyFilterProfileName.\"%s\" = \"%s\"\n", snmpTargetParamsName, temp_string);
    else
      sysapiPrintf("snmpNotifyFilterProfileName.\"%s\" = L7_FAILURE\n", snmpTargetParamsName);

    if (l7_snmpNotifyFilterProfileStorTypeGet(snmpTargetParamsName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpNotifyFilterProfileStorType.\"%s\" = %d\n", snmpTargetParamsName, temp_val);
    else
      sysapiPrintf("snmpNotifyFilterProfileStorType.\"%s\" = L7_FAILURE\n", snmpTargetParamsName);

    if (l7_snmpNotifyFilterProfileRowStatusGet(snmpTargetParamsName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpNotifyFilterProfileRowStatus.\"%s\" = %d\n", snmpTargetParamsName, temp_val);
    else
      sysapiPrintf("snmpNotifyFilterProfileRowStatus.\"%s\" = L7_FAILURE\n", snmpTargetParamsName);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_snmpNotifyFilterProfileTable_create(L7_char8 *snmpTargetParamsName, L7_char8 *snmpNotifyFilterProfileName)
{
  L7_RC_t rc;

  rc = l7_snmpNotifyFilterProfileRowStatusSet(snmpTargetParamsName, snmpRowStatus_createAndWait);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyFilterProfileRowStatusSet 1 failed!\n");
    return;
  }

  rc = l7_snmpNotifyFilterProfileNameSet(snmpTargetParamsName, snmpNotifyFilterProfileName);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyFilterProfileNameSet 1 failed!\n");
    return;
  }

  rc = l7_snmpNotifyFilterProfileRowStatusSet(snmpTargetParamsName, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyFilterProfileRowStatusSet 2 failed!\n");
    return;
  }

  sysapiPrintf("debug_snmpNotifyFilterProfileTable_create finished!\n");
}

/*******************************************************************************************/

/*
        snmpNotifyFilterTable
        snmpNotifyFilterEntry_t

        OID             *snmpNotifyFilterSubtree; (index 2)
        OctetString     *snmpNotifyFilterMask;
        SR_INT32        snmpNotifyFilterType;
        SR_INT32        snmpNotifyFilterStorageType;
        SR_INT32        snmpNotifyFilterRowStatus;
        OctetString *   snmpNotifyFilterProfileName; (index 1)
*/



/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to snmpNotifyFilterEntry_t object
*           
* @notes The table of filter profiles.  Filter profiles are used
*        to determine whether particular management targets should
*        receive particular notifications.
*        
*        When a notification is generated, it must be compared
*        with the filters associated with each management target
*        which is configured to receive notifications, in order to
*        determine whether it may be sent to each such management
*        target.
*        
*        A more complete discussion of notification filtering
*        can be found in section 6. of [RFC2573].
*
* @end
*
*********************************************************************/
static snmpNotifyFilterEntry_t* snmp_snmpNotifyFilterTable_get(int searchType, L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree)
{
  L7_int32 index = -1;
  snmpNotifyFilterEntry_t *row = NULL;
  OID *oid_snmpNotifyFilterSubtree = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || snmpNotifyFilterProfileName == NULL)
    return NULL;

  /* build the search structure */
  agt_snmpNotifyFilterTable.tip[0].value.octet_val = MakeOctetStringFromText(snmpNotifyFilterProfileName);
  if (searchType == NEXT && strcmp(snmpNotifyFilterSubtree, "") == 0)
  {
    oid_snmpNotifyFilterSubtree = MakeOIDFromDot("0.0");
  }
  else
  {
    oid_snmpNotifyFilterSubtree = MakeOIDFromDot(snmpNotifyFilterSubtree);
  }
  agt_snmpNotifyFilterTable.tip[1].value.oid_val = CloneOID(oid_snmpNotifyFilterSubtree);

  if (agt_snmpNotifyFilterTable.tip[0].value.octet_val == NULL ||
      oid_snmpNotifyFilterSubtree == NULL ||
      agt_snmpNotifyFilterTable.tip[1].value.oid_val == NULL)
  {
    FreeOctetString (agt_snmpNotifyFilterTable.tip[0].value.octet_val);
    FreeOID (agt_snmpNotifyFilterTable.tip[1].value.oid_val);
    return NULL;
  }

  index = SearchTable(&agt_snmpNotifyFilterTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (snmpNotifyFilterEntry_t*)agt_snmpNotifyFilterTable.tp[index];

    if (searchType == NEXT &&
        strncmp(snmpNotifyFilterProfileName, 
                row->snmpNotifyFilterProfileName->octet_ptr,
                row->snmpNotifyFilterProfileName->length) == 0 &&
        CmpOID(row->snmpNotifyFilterSubtree, oid_snmpNotifyFilterSubtree) == 0)
      index = SearchTable(&agt_snmpNotifyFilterTable, NEXT_SKIP);
  }


  FreeOctetString(agt_snmpNotifyFilterTable.tip[0].value.octet_val);
  FreeOID(oid_snmpNotifyFilterSubtree);
  FreeOID(agt_snmpNotifyFilterTable.tip[1].value.oid_val);

  if (index >= 0)
    return(snmpNotifyFilterEntry_t*)agt_snmpNotifyFilterTable.tp[index];

  return NULL;
}

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
static int l7_snmpNotifyFilterEntry_set_defaults(snmpNotifyFilterEntry_t *data)
{
  if ((data->snmpNotifyFilterMask = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->snmpNotifyFilterType = D_snmpNotifyFilterType_included;
  data->snmpNotifyFilterStorageType = 
      D_snmpNotifyFilterStorageType_nonVolatile;
  data->RowStatusTimerId = -1;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

/*********************************************************************
*
* @purpose Sets the row associated with the index for the given request
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*           
* @notes The table of filter profiles.  Filter profiles are used
*        to determine whether particular management targets should
*        receive particular notifications.
*        
*        When a notification is generated, it must be compared
*        with the filters associated with each management target
*        which is configured to receive notifications, in order to
*        determine whether it may be sent to each such management
*        target.
*        
*        A more complete discussion of notification filtering
*        can be found in section 6. of [RFC2573].
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_snmpNotifyFilterTable_set(snmpNotifyFilterEntry_t *data)
{
  L7_int32 index = -1;
  snmpNotifyFilterEntry_t * snmpNotifyFilterEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName, data->valid))
    return L7_ERROR;

  /* build the search structure */
  agt_snmpNotifyFilterTable.tip[0].value.octet_val = CloneOctetString(data->snmpNotifyFilterProfileName);
  agt_snmpNotifyFilterTable.tip[1].value.oid_val = CloneOID(data->snmpNotifyFilterSubtree);
  if (agt_snmpNotifyFilterTable.tip[0].value.octet_val == NULL ||
      agt_snmpNotifyFilterTable.tip[1].value.oid_val == NULL)
  {
    return L7_ERROR;
  }

  index = SearchTable(&agt_snmpNotifyFilterTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_snmpNotifyFilterRowStatus, data->valid) && 
        (data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_createAndGo ||
         data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_createAndWait))
    {
      index = NewTableEntry(&agt_snmpNotifyFilterTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_snmpNotifyFilterEntry_set_defaults((snmpNotifyFilterEntry_t*)agt_snmpNotifyFilterTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_createAndGo)
      {
        ((snmpNotifyFilterEntry_t*)agt_snmpNotifyFilterTable.tp[index])->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_active;
      }
      else
      {
        ((snmpNotifyFilterEntry_t*)agt_snmpNotifyFilterTable.tp[index])->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_notInService;
      }
      SET_VALID(I_snmpNotifyFilterRowStatus, ((snmpNotifyFilterEntry_t*)agt_snmpNotifyFilterTable.tp[index])->valid);

      CLR_VALID(I_snmpNotifyFilterRowStatus, data->valid);
    }
  }

  FreeOctetString(agt_snmpNotifyFilterTable.tip[0].value.octet_val);
  FreeOID(agt_snmpNotifyFilterTable.tip[1].value.oid_val);

  if (index >= 0)
  {
    if (VALID(I_snmpNotifyFilterRowStatus, data->valid) && 
        data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_destroy)
    {
      RemoveTableEntry(&agt_snmpNotifyFilterTable, index);
      return L7_SUCCESS;
    }

    snmpNotifyFilterEntryData = (snmpNotifyFilterEntry_t*)agt_snmpNotifyFilterTable.tp[index];

    if (VALID(I_snmpNotifyFilterMask, data->valid))
    {
      if (snmpNotifyFilterEntryData->snmpNotifyFilterMask != NULL)
      {
        FreeOctetString(snmpNotifyFilterEntryData->snmpNotifyFilterMask);
        CLR_VALID(I_snmpNotifyFilterMask, snmpNotifyFilterEntryData->valid);
      }

      snmpNotifyFilterEntryData->snmpNotifyFilterMask = CloneOctetString(data->snmpNotifyFilterMask);

      if (snmpNotifyFilterEntryData->snmpNotifyFilterMask != NULL)
      {
        SET_VALID(I_snmpNotifyFilterMask, snmpNotifyFilterEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpNotifyFilterType, data->valid))
    {
      if (data->snmpNotifyFilterType != D_snmpNotifyFilterType_included &&
          data->snmpNotifyFilterType != D_snmpNotifyFilterType_excluded)
        return L7_ERROR;

      snmpNotifyFilterEntryData->snmpNotifyFilterType = data->snmpNotifyFilterType;
      SET_VALID(I_snmpNotifyFilterType, snmpNotifyFilterEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpNotifyFilterStorageType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(snmpNotifyFilterEntryData->snmpNotifyFilterStorageType,
                                        data->snmpNotifyFilterStorageType) != L7_SUCCESS)
        return L7_ERROR;

      snmpNotifyFilterEntryData->snmpNotifyFilterStorageType = data->snmpNotifyFilterStorageType;
      SET_VALID(I_snmpNotifyFilterStorageType, snmpNotifyFilterEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpNotifyFilterRowStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(snmpNotifyFilterEntryData->snmpNotifyFilterRowStatus,
                                      data->snmpNotifyFilterRowStatus) != L7_SUCCESS)
        return L7_ERROR;

      snmpNotifyFilterEntryData->snmpNotifyFilterRowStatus = data->snmpNotifyFilterRowStatus;
      SET_VALID(I_snmpNotifyFilterRowStatus, snmpNotifyFilterEntryData->valid);
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
L7_RC_t l7_snmpNotifyFilterTableCheckValid(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree)
{
  snmpNotifyFilterEntry_t * snmpNotifyFilterEntryData = snmp_snmpNotifyFilterTable_get(EXACT, snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

  if (snmpNotifyFilterEntryData != NULL)
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
L7_RC_t l7_snmpNotifyFilterTableNextGet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree)
{
  snmpNotifyFilterEntry_t * snmpNotifyFilterEntryData = snmp_snmpNotifyFilterTable_get(NEXT, snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

  if (snmpNotifyFilterEntryData != NULL)
  {
    strncpy(snmpNotifyFilterProfileName, snmpNotifyFilterEntryData->snmpNotifyFilterProfileName->octet_ptr,
            snmpNotifyFilterEntryData->snmpNotifyFilterProfileName->length);
    snmpNotifyFilterProfileName[snmpNotifyFilterEntryData->snmpNotifyFilterProfileName->length] = '\0';

    SPrintOID(snmpNotifyFilterEntryData->snmpNotifyFilterSubtree, snmpNotifyFilterSubtree);

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterMask
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The bit mask which, in combination with the corresponding
*        instance of snmpNotifyFilterSubtree, defines a family of
*        subtrees which are included in or excluded from the
*        filter profile.
*        
*        Each bit of this bit mask corresponds to a
*        sub-identifier of snmpNotifyFilterSubtree, with the
*        most significant bit of the i-th octet of this octet
*        string value (extended if necessary, see below)
*        corresponding to the (8*i - 7)-th sub-identifier, and
*        the least significant bit of the i-th octet of this
*        octet string corresponding to the (8*i)-th
*        sub-identifier, where i is in the range 1 through 16.
*        
*        Each bit of this bit mask specifies whether or not
*        the corresponding sub-identifiers must match when
*        determining if an OBJECT IDENTIFIER matches this
*        family of filter subtrees; a '1' indicates that an
*        exact match must occur; a '0' indicates 'wild card',
*        i.e., any sub-identifier value matches.
*        
*        Thus, the OBJECT IDENTIFIER X of an object instance
*        is contained in a family of filter subtrees if, for
*        each sub-identifier of the value of
*        snmpNotifyFilterSubtree, either:
*        
*          the i-th bit of snmpNotifyFilterMask is 0, or
*        
*          the i-th sub-identifier of X is equal to the i-th
*          sub-identifier of the value of
*          snmpNotifyFilterSubtree.
*        
*        If the value of this bit mask is M bits long and
*        there are more than M sub-identifiers in the
*        corresponding instance of snmpNotifyFilterSubtree,
*        then the bit mask is extended with 1's to be the
*        required length.
*        
*        Note that when the value of this object is the
*        zero-length string, this extension rule results in
*        a mask of all-1's being used (i.e., no 'wild card'),
*        and the family of filter subtrees is the one
*        subtree uniquely identified by the corresponding
*        instance of snmpNotifyFilterSubtree.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterMaskGet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, L7_char8 *val, L7_uint32 *len)
{
  snmpNotifyFilterEntry_t * snmpNotifyFilterEntryData = snmp_snmpNotifyFilterTable_get(EXACT, snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

  if (snmpNotifyFilterEntryData != NULL && 
      VALID(I_snmpNotifyFilterMask, snmpNotifyFilterEntryData->valid))
  {
    strncpy(val, snmpNotifyFilterEntryData->snmpNotifyFilterMask->octet_ptr,
            snmpNotifyFilterEntryData->snmpNotifyFilterMask->length);
    *len = snmpNotifyFilterEntryData->snmpNotifyFilterMask->length;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterMask
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The bit mask which, in combination with the corresponding
*        instance of snmpNotifyFilterSubtree, defines a family of
*        subtrees which are included in or excluded from the
*        filter profile.
*        
*        Each bit of this bit mask corresponds to a
*        sub-identifier of snmpNotifyFilterSubtree, with the
*        most significant bit of the i-th octet of this octet
*        string value (extended if necessary, see below)
*        corresponding to the (8*i - 7)-th sub-identifier, and
*        the least significant bit of the i-th octet of this
*        octet string corresponding to the (8*i)-th
*        sub-identifier, where i is in the range 1 through 16.
*        
*        Each bit of this bit mask specifies whether or not
*        the corresponding sub-identifiers must match when
*        determining if an OBJECT IDENTIFIER matches this
*        family of filter subtrees; a '1' indicates that an
*        exact match must occur; a '0' indicates 'wild card',
*        i.e., any sub-identifier value matches.
*        
*        Thus, the OBJECT IDENTIFIER X of an object instance
*        is contained in a family of filter subtrees if, for
*        each sub-identifier of the value of
*        snmpNotifyFilterSubtree, either:
*        
*          the i-th bit of snmpNotifyFilterMask is 0, or
*        
*          the i-th sub-identifier of X is equal to the i-th
*          sub-identifier of the value of
*          snmpNotifyFilterSubtree.
*        
*        If the value of this bit mask is M bits long and
*        there are more than M sub-identifiers in the
*        corresponding instance of snmpNotifyFilterSubtree,
*        then the bit mask is extended with 1's to be the
*        required length.
*        
*        Note that when the value of this object is the
*        zero-length string, this extension rule results in
*        a mask of all-1's being used (i.e., no 'wild card'),
*        and the family of filter subtrees is the one
*        subtree uniquely identified by the corresponding
*        instance of snmpNotifyFilterSubtree.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterMaskSet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, L7_char8 *val, L7_uint32 len)
{
  snmpNotifyFilterEntry_t snmpNotifyFilterEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (len > 16)
    return rc;

  memset((void*)&snmpNotifyFilterEntryData, 0, sizeof(snmpNotifyFilterEntryData));

  snmpNotifyFilterEntryData.snmpNotifyFilterProfileName = MakeOctetStringFromText(snmpNotifyFilterProfileName);
  SET_VALID(I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName, snmpNotifyFilterEntryData.valid);

  snmpNotifyFilterEntryData.snmpNotifyFilterSubtree = MakeOIDFromDot(snmpNotifyFilterSubtree);
  SET_VALID(I_snmpNotifyFilterSubtree, snmpNotifyFilterEntryData.valid);

  snmpNotifyFilterEntryData.snmpNotifyFilterMask = MakeOctetString(val, len);
  SET_VALID(I_snmpNotifyFilterMask, snmpNotifyFilterEntryData.valid);

  rc = snmp_snmpNotifyFilterTable_set(&snmpNotifyFilterEntryData);

  FreeOctetString(snmpNotifyFilterEntryData.snmpNotifyFilterProfileName);
  FreeOID(snmpNotifyFilterEntryData.snmpNotifyFilterSubtree);
  FreeOctetString(snmpNotifyFilterEntryData.snmpNotifyFilterMask);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object indicates whether the family of filter subtrees
*        defined by this entry are included in or excluded from a
*        filter.  A more detailed discussion of the use of this
*        object can be found in section 6. of [RFC2573].
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterTypeGet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpNotifyFilterType_t *val)
{
  snmpNotifyFilterEntry_t * snmpNotifyFilterEntryData = snmp_snmpNotifyFilterTable_get(EXACT, snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

  if (snmpNotifyFilterEntryData != NULL && 
      VALID(I_snmpNotifyFilterType, snmpNotifyFilterEntryData->valid))
  {
    *val = snmpNotifyFilterEntryData->snmpNotifyFilterType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object indicates whether the family of filter subtrees
*        defined by this entry are included in or excluded from a
*        filter.  A more detailed discussion of the use of this
*        object can be found in section 6. of [RFC2573].
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterTypeSet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpNotifyFilterType_t val)
{
  snmpNotifyFilterEntry_t snmpNotifyFilterEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpNotifyFilterEntryData, 0, sizeof(snmpNotifyFilterEntryData));

  snmpNotifyFilterEntryData.snmpNotifyFilterProfileName = MakeOctetStringFromText(snmpNotifyFilterProfileName);
  SET_VALID(I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName, snmpNotifyFilterEntryData.valid);

	snmpNotifyFilterEntryData.snmpNotifyFilterSubtree = MakeOIDFromDot(snmpNotifyFilterSubtree);
	SET_VALID(I_snmpNotifyFilterSubtree, snmpNotifyFilterEntryData.valid);

  snmpNotifyFilterEntryData.snmpNotifyFilterType = val;
  SET_VALID(I_snmpNotifyFilterType, snmpNotifyFilterEntryData.valid);

  rc = snmp_snmpNotifyFilterTable_set(&snmpNotifyFilterEntryData);

  FreeOctetString(snmpNotifyFilterEntryData.snmpNotifyFilterProfileName);
	FreeOID(snmpNotifyFilterEntryData.snmpNotifyFilterSubtree);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type of this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterStorageTypeGet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpStorageType_t *val)
{
  snmpNotifyFilterEntry_t * snmpNotifyFilterEntryData = snmp_snmpNotifyFilterTable_get(EXACT, snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

  if (snmpNotifyFilterEntryData != NULL && 
      VALID(I_snmpNotifyFilterStorageType, snmpNotifyFilterEntryData->valid))
  {
    *val = snmpNotifyFilterEntryData->snmpNotifyFilterStorageType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type of this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterStorageTypeSet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpStorageType_t val)
{
  snmpNotifyFilterEntry_t snmpNotifyFilterEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpNotifyFilterEntryData, 0, sizeof(snmpNotifyFilterEntryData));

  snmpNotifyFilterEntryData.snmpNotifyFilterProfileName = MakeOctetStringFromText(snmpNotifyFilterProfileName);
  SET_VALID(I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName, snmpNotifyFilterEntryData.valid);

	snmpNotifyFilterEntryData.snmpNotifyFilterSubtree = MakeOIDFromDot(snmpNotifyFilterSubtree);
	SET_VALID(I_snmpNotifyFilterSubtree, snmpNotifyFilterEntryData.valid);

  snmpNotifyFilterEntryData.snmpNotifyFilterStorageType = val;
  SET_VALID(I_snmpNotifyFilterStorageType, snmpNotifyFilterEntryData.valid);

  rc = snmp_snmpNotifyFilterTable_set(&snmpNotifyFilterEntryData);

  FreeOctetString(snmpNotifyFilterEntryData.snmpNotifyFilterProfileName);
	FreeOID(snmpNotifyFilterEntryData.snmpNotifyFilterSubtree);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterRowStatusGet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpRowStatus_t *val)
{
  snmpNotifyFilterEntry_t *snmpNotifyFilterEntryData = snmp_snmpNotifyFilterTable_get(EXACT, snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

  if (snmpNotifyFilterEntryData != NULL && 
      VALID(I_snmpNotifyFilterRowStatus, snmpNotifyFilterEntryData->valid))
  {
    *val = snmpNotifyFilterEntryData->snmpNotifyFilterRowStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyFilterRowStatusSet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpRowStatus_t val)
{
  snmpNotifyFilterEntry_t snmpNotifyFilterEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpNotifyFilterEntryData, 0, sizeof(snmpNotifyFilterEntryData));

  if (l7_snmpStringSetValidate(snmpNotifyFilterProfileName, 1, 32) != L7_SUCCESS)
    return rc;

  snmpNotifyFilterEntryData.snmpNotifyFilterProfileName = MakeOctetStringFromText(snmpNotifyFilterProfileName);
  SET_VALID(I_snmpNotifyFilterEntryIndex_snmpNotifyFilterProfileName, snmpNotifyFilterEntryData.valid);

	snmpNotifyFilterEntryData.snmpNotifyFilterSubtree = MakeOIDFromDot(snmpNotifyFilterSubtree);
	SET_VALID(I_snmpNotifyFilterSubtree, snmpNotifyFilterEntryData.valid);

  snmpNotifyFilterEntryData.snmpNotifyFilterRowStatus = val;
  SET_VALID(I_snmpNotifyFilterRowStatus, snmpNotifyFilterEntryData.valid);

  rc = snmp_snmpNotifyFilterTable_set(&snmpNotifyFilterEntryData);

  FreeOctetString(snmpNotifyFilterEntryData.snmpNotifyFilterProfileName);
	FreeOID(snmpNotifyFilterEntryData.snmpNotifyFilterSubtree);

  return rc;
}

void
debug_snmpNotifyFilterTable_walk()
{
  L7_char8 snmpNotifyFilterProfileName[256];
	L7_char8 snmpNotifyFilterSubtree[256];
  L7_char8 temp_string[256];
  L7_uint32 temp_len;
  L7_uint32 temp_val;

  memset((void*)snmpNotifyFilterProfileName, 0, sizeof(snmpNotifyFilterProfileName));
	memset((void*)snmpNotifyFilterSubtree, 0, sizeof(snmpNotifyFilterSubtree));

  sysapiPrintf("[snmpNotifyFilterTable start]\n");

  while(l7_snmpNotifyFilterTableNextGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree) == L7_SUCCESS)
  {
    sysapiPrintf("snmpNotifyFilterProfileName.\"%s\".\"%s\" = \"%s\"\n", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, snmpNotifyFilterProfileName);
		sysapiPrintf("snmpNotifyFilterSubtree.\"%s\".\"%s\" = \"%s\"\n", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, snmpNotifyFilterSubtree);

    if (l7_snmpNotifyFilterMaskGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, temp_string, &temp_len) == L7_SUCCESS)
    {
      sysapiPrintf("snmpNotifyFilterMask.\"%s\".\"%s\" = \"", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);
      debug_snmp_printHexString(temp_string, temp_len);
      sysapiPrintf("\"\n");
    }
    else
      sysapiPrintf("snmpNotifyFilterMask.\"%s\".\"%s\" = L7_FAILURE\n", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

		if (l7_snmpNotifyFilterTypeGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, &temp_val) == L7_SUCCESS)
			sysapiPrintf("snmpNotifyFilterType.\"%s\".\"%s\" = %d\n", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, temp_val);
		else
			sysapiPrintf("snmpNotifyFilterType.\"%s\".\"%s\" = L7_FAILURE\n", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

		if (l7_snmpNotifyFilterStorageTypeGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpNotifyFilterStorageType.\"%s\".\"%s\" = %d\n", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, temp_val);
    else
      sysapiPrintf("snmpNotifyFilterStorageType.\"%s\".\"%s\" = L7_FAILURE\n", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

    if (l7_snmpNotifyFilterRowStatusGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpNotifyFilterRowStatus.\"%s\".\"%s\" = %d\n", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, temp_val);
    else
      sysapiPrintf("snmpNotifyFilterRowStatus.\"%s\".\"%s\" = L7_FAILURE\n", snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_snmpNotifyFilterTable_create(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpNotifyFilterType_t snmpNotifyFilterType)
{
  L7_RC_t rc;
  L7_char8 temp_str[128];

  rc = l7_snmpNotifyFilterRowStatusSet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, snmpRowStatus_createAndWait);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyFilterRowStatusSet 1 failed!\n");
    return;
  }
  memset(temp_str, 10, sizeof(temp_str));

  rc = l7_snmpNotifyFilterMaskSet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, temp_str, 4);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyFilterMaskSet 1 failed!\n");
    return;
  }

  rc = l7_snmpNotifyFilterTypeSet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, snmpNotifyFilterType);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyFilterTypeSet 1 failed!\n");
    return;
  }

  rc = l7_snmpNotifyFilterRowStatusSet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyFilterRowStatusSet 2 failed!\n");
    return;
  }

  sysapiPrintf("debug_snmpNotifyFilterTable_create finished!\n");
}

/*******************************************************************************************/

/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to snmpNotifyEntry_t object
*           
* @notes This table is used to select management targets which should
*        receive notifications, as well as the type of notification
*        which should be sent to each selected management target.
*
* @end
*
*********************************************************************/
static snmpNotifyEntry_t* snmp_snmpNotifyTable_get(int searchType, L7_char8 *snmpNotifyName)
{
  L7_int32 index = -1;
  snmpNotifyEntry_t *row = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || snmpNotifyName == NULL)
    return NULL;

  /* build the search structure */
  agt_snmpNotifyTable.tip[0].value.octet_val = MakeOctetStringFromText(snmpNotifyName);
  if (agt_snmpNotifyTable.tip[0].value.octet_val == NULL)
  {
    return NULL;
  }

  index = SearchTable(&agt_snmpNotifyTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (snmpNotifyEntry_t*)agt_snmpNotifyTable.tp[index];

    if (searchType == NEXT &&
        strncmp(snmpNotifyName, 
                row->snmpNotifyName->octet_ptr,
                row->snmpNotifyName->length) == 0)
    index = SearchTable(&agt_snmpNotifyTable, NEXT_SKIP);
  }


  FreeOctetString(agt_snmpNotifyTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    return(snmpNotifyEntry_t*)agt_snmpNotifyTable.tp[index];
  }

  return NULL;
}

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
static int l7_snmpNotifyEntry_set_defaults(snmpNotifyEntry_t *data)
{
  if ((data->snmpNotifyTag = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->snmpNotifyType = D_snmpNotifyType_trap;
  data->RowStatusTimerId = -1;
  data->snmpNotifyStorageType = D_snmpNotifyStorageType_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

/*********************************************************************
*
* @purpose Sets the row associated with the index for the given request
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*           
* @notes This table is used to select management targets which should
*        receive notifications, as well as the type of notification
*        which should be sent to each selected management target.
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_snmpNotifyTable_set(snmpNotifyEntry_t *data)
{
  L7_int32 index = -1;
  snmpNotifyEntry_t * snmpNotifyEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_snmpNotifyName, data->valid))
    return L7_ERROR;

  /* build the search structure */
  agt_snmpNotifyTable.tip[0].value.octet_val = CloneOctetString(data->snmpNotifyName);
  if (agt_snmpNotifyTable.tip[0].value.octet_val == NULL)
  {
    return L7_ERROR;
  }

  index = SearchTable(&agt_snmpNotifyTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_snmpNotifyRowStatus, data->valid) && 
        (data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_createAndGo ||
         data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_createAndWait))
    {
      index = NewTableEntry(&agt_snmpNotifyTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_snmpNotifyEntry_set_defaults((snmpNotifyEntry_t*)agt_snmpNotifyTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_createAndGo)
      {
        ((snmpNotifyEntry_t*)agt_snmpNotifyTable.tp[index])->snmpNotifyRowStatus = D_snmpNotifyRowStatus_active;
      }
      else
      {
        ((snmpNotifyEntry_t*)agt_snmpNotifyTable.tp[index])->snmpNotifyRowStatus = D_snmpNotifyRowStatus_notInService;
      }
      SET_VALID(I_snmpNotifyRowStatus, ((snmpNotifyEntry_t*)agt_snmpNotifyTable.tp[index])->valid);

      CLR_VALID(I_snmpNotifyRowStatus, data->valid);
    }
  }

  FreeOctetString(agt_snmpNotifyTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    if (VALID(I_snmpNotifyRowStatus, data->valid) && 
        data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_destroy)
    {
      RemoveTableEntry(&agt_snmpNotifyTable, index);
      return L7_SUCCESS;
    }

    snmpNotifyEntryData = (snmpNotifyEntry_t*)agt_snmpNotifyTable.tp[index];

    if (VALID(I_snmpNotifyTag, data->valid))
    {
      if (snmpNotifyEntryData->snmpNotifyTag != NULL)
      {
        FreeOctetString(snmpNotifyEntryData->snmpNotifyTag);
        CLR_VALID(I_snmpNotifyTag, snmpNotifyEntryData->valid);
      }

      snmpNotifyEntryData->snmpNotifyTag = CloneOctetString(data->snmpNotifyTag);

      if (snmpNotifyEntryData->snmpNotifyTag != NULL)
      {
        SET_VALID(I_snmpNotifyTag, snmpNotifyEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpNotifyType, data->valid))
    {
      if ((data->snmpNotifyType != snmpNotifyType_inform) &&
          (data->snmpNotifyType != snmpNotifyType_trap))
        return L7_ERROR;

      snmpNotifyEntryData->snmpNotifyType = data->snmpNotifyType;
      SET_VALID(I_snmpNotifyType, snmpNotifyEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpNotifyStorageType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(snmpNotifyEntryData->snmpNotifyStorageType,
                                        data->snmpNotifyStorageType) != L7_SUCCESS)
        return L7_ERROR;

      snmpNotifyEntryData->snmpNotifyStorageType = data->snmpNotifyStorageType;
      SET_VALID(I_snmpNotifyStorageType, snmpNotifyEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpNotifyRowStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(snmpNotifyEntryData->snmpNotifyRowStatus,
                                      data->snmpNotifyRowStatus) != L7_SUCCESS)
        return L7_ERROR;

      snmpNotifyEntryData->snmpNotifyRowStatus = data->snmpNotifyRowStatus;
      SET_VALID(I_snmpNotifyRowStatus, snmpNotifyEntryData->valid);
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
L7_RC_t l7_snmpNotifyTableCheckValid(L7_char8 *snmpNotifyName)
{
  snmpNotifyEntry_t * snmpNotifyEntryData = snmp_snmpNotifyTable_get(EXACT, snmpNotifyName);

  if (snmpNotifyEntryData != NULL)
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
L7_RC_t l7_snmpNotifyTableNextGet(L7_char8 *snmpNotifyName)
{
  snmpNotifyEntry_t * snmpNotifyEntryData = snmp_snmpNotifyTable_get(NEXT, snmpNotifyName);

  if (snmpNotifyEntryData != NULL)
  {
    strncpy(snmpNotifyName, snmpNotifyEntryData->snmpNotifyName->octet_ptr,
            snmpNotifyEntryData->snmpNotifyName->length);
    snmpNotifyName[snmpNotifyEntryData->snmpNotifyName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyTag
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object contains a single tag value which is used
*        to select entries in the snmpTargetAddrTable.  Any entry
*        in the snmpTargetAddrTable which contains a tag value
*        which is equal to the value of an instance of this
*        object is selected.  If this object contains a value
*        of zero length, no entries are selected.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyTagGet(L7_char8 *snmpNotifyName, L7_char8 *val)
{
  snmpNotifyEntry_t * snmpNotifyEntryData = snmp_snmpNotifyTable_get(EXACT, snmpNotifyName);

  if (snmpNotifyEntryData != NULL && 
      VALID(I_snmpNotifyTag, snmpNotifyEntryData->valid))
  {
    strncpy(val, snmpNotifyEntryData->snmpNotifyTag->octet_ptr,
            snmpNotifyEntryData->snmpNotifyTag->length);
    val[snmpNotifyEntryData->snmpNotifyTag->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyTag
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object contains a single tag value which is used
*        to select entries in the snmpTargetAddrTable.  Any entry
*        in the snmpTargetAddrTable which contains a tag value
*        which is equal to the value of an instance of this
*        object is selected.  If this object contains a value
*        of zero length, no entries are selected.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyTagSet(L7_char8 *snmpNotifyName, L7_char8 *val)
{
  snmpNotifyEntry_t snmpNotifyEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 0, 255) != L7_SUCCESS)
    return rc;

  memset((void*)&snmpNotifyEntryData, 0, sizeof(snmpNotifyEntryData));

  snmpNotifyEntryData.snmpNotifyName = MakeOctetStringFromText(snmpNotifyName);
  SET_VALID(I_snmpNotifyName, snmpNotifyEntryData.valid);

  snmpNotifyEntryData.snmpNotifyTag = MakeOctetStringFromText(val);
  SET_VALID(I_snmpNotifyTag, snmpNotifyEntryData.valid);

  rc = snmp_snmpNotifyTable_set(&snmpNotifyEntryData);

  FreeOctetString(snmpNotifyEntryData.snmpNotifyName);
  FreeOctetString(snmpNotifyEntryData.snmpNotifyTag);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object determines the type of notification to
*        be generated for entries in the snmpTargetAddrTable
*        selected by the corresponding instance of
*        snmpNotifyTag.  This value is only used when
*        generating notifications, and is ignored when
*        using the snmpTargetAddrTable for other purposes.
*        
*        If the value of this object is trap(1), then any
*        messages generated for selected rows will contain
*        Unconfirmed-Class PDUs.
*        
*        If the value of this object is inform(2), then any
*        messages generated for selected rows will contain
*        Confirmed-Class PDUs.
*        
*        Note that if an SNMP entity only supports
*        generation of Unconfirmed-Class PDUs (and not
*        Confirmed-Class PDUs), then this object may be
*        read-only.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyTypeGet(L7_char8 *snmpNotifyName, l7_snmpNotifyType_t *val)
{
  snmpNotifyEntry_t * snmpNotifyEntryData = snmp_snmpNotifyTable_get(EXACT, snmpNotifyName);

  if (snmpNotifyEntryData != NULL && 
      VALID(I_snmpNotifyType, snmpNotifyEntryData->valid))
  {
    *val = snmpNotifyEntryData->snmpNotifyType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object determines the type of notification to
*        be generated for entries in the snmpTargetAddrTable
*        selected by the corresponding instance of
*        snmpNotifyTag.  This value is only used when
*        generating notifications, and is ignored when
*        using the snmpTargetAddrTable for other purposes.
*        
*        If the value of this object is trap(1), then any
*        messages generated for selected rows will contain
*        Unconfirmed-Class PDUs.
*        
*        If the value of this object is inform(2), then any
*        messages generated for selected rows will contain
*        Confirmed-Class PDUs.
*        
*        Note that if an SNMP entity only supports
*        generation of Unconfirmed-Class PDUs (and not
*        Confirmed-Class PDUs), then this object may be
*        read-only.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyTypeSet(L7_char8 *snmpNotifyName, l7_snmpNotifyType_t val)
{
  snmpNotifyEntry_t snmpNotifyEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpNotifyEntryData, 0, sizeof(snmpNotifyEntryData));

  snmpNotifyEntryData.snmpNotifyName = MakeOctetStringFromText(snmpNotifyName);
  SET_VALID(I_snmpNotifyName, snmpNotifyEntryData.valid);

  snmpNotifyEntryData.snmpNotifyType = val;
  SET_VALID(I_snmpNotifyType, snmpNotifyEntryData.valid);

  rc = snmp_snmpNotifyTable_set(&snmpNotifyEntryData);

  FreeOctetString(snmpNotifyEntryData.snmpNotifyName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyStorageTypeGet(L7_char8 *snmpNotifyName, l7_snmpStorageType_t *val)
{
  snmpNotifyEntry_t * snmpNotifyEntryData = snmp_snmpNotifyTable_get(EXACT, snmpNotifyName);

  if (snmpNotifyEntryData != NULL && 
      VALID(I_snmpNotifyStorageType, snmpNotifyEntryData->valid))
  {
    *val = snmpNotifyEntryData->snmpNotifyStorageType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyStorageTypeSet(L7_char8 *snmpNotifyName, l7_snmpStorageType_t val)
{
  snmpNotifyEntry_t snmpNotifyEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpNotifyEntryData, 0, sizeof(snmpNotifyEntryData));

  snmpNotifyEntryData.snmpNotifyName = MakeOctetStringFromText(snmpNotifyName);
  SET_VALID(I_snmpNotifyName, snmpNotifyEntryData.valid);

  snmpNotifyEntryData.snmpNotifyStorageType = val;
  SET_VALID(I_snmpNotifyStorageType, snmpNotifyEntryData.valid);

  rc = snmp_snmpNotifyTable_set(&snmpNotifyEntryData);

  FreeOctetString(snmpNotifyEntryData.snmpNotifyName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyRowStatusGet(L7_char8 *snmpNotifyName, l7_snmpRowStatus_t *val)
{
  snmpNotifyEntry_t * snmpNotifyEntryData = snmp_snmpNotifyTable_get(EXACT, snmpNotifyName);

  if (snmpNotifyEntryData != NULL && 
      VALID(I_snmpNotifyRowStatus, snmpNotifyEntryData->valid))
  {
    *val = snmpNotifyEntryData->snmpNotifyRowStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpNotifyRowStatusSet(L7_char8 *snmpNotifyName, l7_snmpRowStatus_t val)
{
  snmpNotifyEntry_t snmpNotifyEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpNotifyEntryData, 0, sizeof(snmpNotifyEntryData));

  if (l7_snmpStringSetValidate(snmpNotifyName, 1, 32) != L7_SUCCESS)
    return rc;

  snmpNotifyEntryData.snmpNotifyName = MakeOctetStringFromText(snmpNotifyName);
  SET_VALID(I_snmpNotifyName, snmpNotifyEntryData.valid);

  snmpNotifyEntryData.snmpNotifyRowStatus = val;
  SET_VALID(I_snmpNotifyRowStatus, snmpNotifyEntryData.valid);

  rc = snmp_snmpNotifyTable_set(&snmpNotifyEntryData);

  FreeOctetString(snmpNotifyEntryData.snmpNotifyName);

  return rc;
}

void
debug_snmpNotifyTable_walk()
{
  L7_char8 snmpNotifyName[256];
  L7_char8 temp_string[256];
  L7_uint32 temp_val;

  memset((void*)snmpNotifyName, 0, sizeof(snmpNotifyName));

  sysapiPrintf("[snmpNotifyTable start]\n");

  while(l7_snmpNotifyTableNextGet(snmpNotifyName) == L7_SUCCESS)
  {
    sysapiPrintf("snmpNotifyName.\"%s\" = \"%s\"\n", snmpNotifyName, snmpNotifyName);

    if (l7_snmpNotifyTagGet(snmpNotifyName, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpNotifyTag.\"%s\" = \"%s\"\n", snmpNotifyName, temp_string);
    else
      sysapiPrintf("snmpNotifyTag.\"%s\" = L7_FAILURE\n", snmpNotifyName);

    if (l7_snmpNotifyTypeGet(snmpNotifyName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpNotifyType.\"%s\" = %d\n", snmpNotifyName, temp_val);
    else
      sysapiPrintf("snmpNotifyType.\"%s\" = L7_FAILURE\n", snmpNotifyName);

    if (l7_snmpNotifyStorageTypeGet(snmpNotifyName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpNotifyStorageType.\"%s\" = %d\n", snmpNotifyName, temp_val);
    else
      sysapiPrintf("snmpNotifyStorageType.\"%s\" = L7_FAILURE\n", snmpNotifyName);

    if (l7_snmpNotifyRowStatusGet(snmpNotifyName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpNotifyRowStatus.\"%s\" = %d\n", snmpNotifyName, temp_val);
    else
      sysapiPrintf("snmpNotifyRowStatus.\"%s\" = L7_FAILURE\n", snmpNotifyName);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_snmpNotifyTable_create(L7_char8 *snmpNotifyName, L7_char8 *snmpNotifyTag, l7_snmpNotifyType_t snmpNotifyType)
{
  L7_RC_t rc;

  rc = l7_snmpNotifyRowStatusSet(snmpNotifyName, snmpRowStatus_createAndWait);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyRowStatusSet 1 failed!\n");
    return;
  }
  rc = l7_snmpNotifyTagSet(snmpNotifyName, snmpNotifyTag);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyTagSet 1 failed!\n");
    return;
  }

  rc = l7_snmpNotifyTypeSet(snmpNotifyName, snmpNotifyType);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyTypeSet 1 failed!\n");
    return;
  }

  rc = l7_snmpNotifyRowStatusSet(snmpNotifyName, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpNotifyRowStatusSet 2 failed!\n");
    return;
  }

}

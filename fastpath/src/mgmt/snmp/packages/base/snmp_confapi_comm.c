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
#include "snmp_util.h"
#include "snmp_util_api.h"

extern SnmpV2Table agt_snmpCommunityTable;           /* snmpCommunityEntry_t */

extern int writeConfigFileFlag;

/*******************************************************************************************/

/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to snmpCommunityEntry_t object
*           
* @notes The table of community strings configured in the SNMP
*        engine's Local Configuration Datastore (LCD).
*
* @end
*
*********************************************************************/
static snmpCommunityEntry_t* snmp_snmpCommunityTable_get(int searchType, L7_char8 *snmpCommunityIndex)
{
  L7_int32 index = -1;
  snmpCommunityEntry_t *row = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || snmpCommunityIndex == NULL)
    return NULL;

  /* build the search structure */
  agt_snmpCommunityTable.tip[0].value.octet_val = MakeOctetStringFromText(snmpCommunityIndex);
  if (agt_snmpCommunityTable.tip[0].value.octet_val == NULL)
  {
    return NULL;
  }

  index = SearchTable(&agt_snmpCommunityTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (snmpCommunityEntry_t*)agt_snmpCommunityTable.tp[index];

    if (searchType == NEXT &&
        strncmp(snmpCommunityIndex, 
                row->snmpCommunityIndex->octet_ptr,
                row->snmpCommunityIndex->length) == 0)
    index = SearchTable(&agt_snmpCommunityTable, NEXT_SKIP);
  }


  FreeOctetString(agt_snmpCommunityTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    return(snmpCommunityEntry_t*)agt_snmpCommunityTable.tp[index];
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
static int l7_snmpCommunityEntry_set_defaults(snmpCommunityEntry_t *data)
{
    if ((data->snmpCommunityName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpCommunitySecurityName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpCommunityContextEngineID = SrGetSnmpID(SR_SNMPID_ALGORITHM_SIMPLE_DEFAULT,NULL)) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpCommunityContextName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpCommunityTransportTag = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->snmpCommunityStorageType = D_snmpCommunityStorageType_nonVolatile;
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
* @notes The table of community strings configured in the SNMP
*        engine's Local Configuration Datastore (LCD).
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_snmpCommunityTable_set(snmpCommunityEntry_t *data)
{
  L7_int32 index = -1;
  snmpCommunityEntry_t * snmpCommunityEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_snmpCommunityIndex, data->valid))
    return L7_ERROR;

  /* build the search structure */
  agt_snmpCommunityTable.tip[0].value.octet_val = CloneOctetString(data->snmpCommunityIndex);
  if (agt_snmpCommunityTable.tip[0].value.octet_val == NULL)
  {
    return L7_ERROR;
  }

  index = SearchTable(&agt_snmpCommunityTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_snmpCommunityStatus, data->valid) && 
        (data->snmpCommunityStatus == D_snmpCommunityStatus_createAndGo ||
         data->snmpCommunityStatus == D_snmpCommunityStatus_createAndWait))
    {
      index = NewTableEntry(&agt_snmpCommunityTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_snmpCommunityEntry_set_defaults((snmpCommunityEntry_t*)agt_snmpCommunityTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->snmpCommunityStatus == D_snmpCommunityStatus_createAndGo)
      {
        ((snmpCommunityEntry_t*)agt_snmpCommunityTable.tp[index])->snmpCommunityStatus = D_snmpCommunityStatus_active;
      }
      else
      {
        ((snmpCommunityEntry_t*)agt_snmpCommunityTable.tp[index])->snmpCommunityStatus = D_snmpCommunityStatus_notInService;
      }
      SET_VALID(I_snmpCommunityStatus, ((snmpCommunityEntry_t*)agt_snmpCommunityTable.tp[index])->valid);

      CLR_VALID(I_snmpCommunityStatus, data->valid);
    }
  }

  FreeOctetString(agt_snmpCommunityTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    if (VALID(I_snmpCommunityStatus, data->valid) && 
        data->snmpCommunityStatus == D_snmpCommunityStatus_destroy)
    {
      RemoveTableEntry(&agt_snmpCommunityTable, index);
      return L7_SUCCESS;
    }

    snmpCommunityEntryData = (snmpCommunityEntry_t*)agt_snmpCommunityTable.tp[index];

    if (VALID(I_snmpCommunityName, data->valid))
    {
      if (snmpCommunityEntryData->snmpCommunityName != NULL)
      {
        FreeOctetString(snmpCommunityEntryData->snmpCommunityName);
        CLR_VALID(I_snmpCommunityName, snmpCommunityEntryData->valid);
      }

      snmpCommunityEntryData->snmpCommunityName = CloneOctetString(data->snmpCommunityName);

      if (snmpCommunityEntryData->snmpCommunityName != NULL)
      {
        SET_VALID(I_snmpCommunityName, snmpCommunityEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpCommunitySecurityName, data->valid))
    {
      if (snmpCommunityEntryData->snmpCommunitySecurityName != NULL)
      {
        FreeOctetString(snmpCommunityEntryData->snmpCommunitySecurityName);
        CLR_VALID(I_snmpCommunitySecurityName, snmpCommunityEntryData->valid);
      }

      snmpCommunityEntryData->snmpCommunitySecurityName = CloneOctetString(data->snmpCommunitySecurityName);

      if (snmpCommunityEntryData->snmpCommunitySecurityName != NULL)
      {
        SET_VALID(I_snmpCommunitySecurityName, snmpCommunityEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpCommunityContextEngineID, data->valid))
    {
      if (snmpCommunityEntryData->snmpCommunityContextEngineID != NULL)
      {
        FreeOctetString(snmpCommunityEntryData->snmpCommunityContextEngineID);
        CLR_VALID(I_snmpCommunityContextEngineID, snmpCommunityEntryData->valid);
      }

      snmpCommunityEntryData->snmpCommunityContextEngineID = CloneOctetString(data->snmpCommunityContextEngineID);

      if (snmpCommunityEntryData->snmpCommunityContextEngineID != NULL)
      {
        SET_VALID(I_snmpCommunityContextEngineID, snmpCommunityEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpCommunityContextName, data->valid))
    {
      if (snmpCommunityEntryData->snmpCommunityContextName != NULL)
      {
        FreeOctetString(snmpCommunityEntryData->snmpCommunityContextName);
        CLR_VALID(I_snmpCommunityContextName, snmpCommunityEntryData->valid);
      }

      snmpCommunityEntryData->snmpCommunityContextName = CloneOctetString(data->snmpCommunityContextName);

      if (snmpCommunityEntryData->snmpCommunityContextName != NULL)
      {
        SET_VALID(I_snmpCommunityContextName, snmpCommunityEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpCommunityTransportTag, data->valid))
    {
      if (snmpCommunityEntryData->snmpCommunityTransportTag != NULL)
      {
        FreeOctetString(snmpCommunityEntryData->snmpCommunityTransportTag);
        CLR_VALID(I_snmpCommunityTransportTag, snmpCommunityEntryData->valid);
      }

      snmpCommunityEntryData->snmpCommunityTransportTag = CloneOctetString(data->snmpCommunityTransportTag);

      if (snmpCommunityEntryData->snmpCommunityTransportTag != NULL)
      {
        SET_VALID(I_snmpCommunityTransportTag, snmpCommunityEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpCommunityStorageType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(snmpCommunityEntryData->snmpCommunityStorageType,
                                        data->snmpCommunityStorageType) != L7_SUCCESS)
        return L7_ERROR;

      snmpCommunityEntryData->snmpCommunityStorageType = data->snmpCommunityStorageType;
      SET_VALID(I_snmpCommunityStorageType, snmpCommunityEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpCommunityStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(snmpCommunityEntryData->snmpCommunityStatus,
                                      data->snmpCommunityStatus) != L7_SUCCESS)
        return L7_ERROR;

      snmpCommunityEntryData->snmpCommunityStatus = data->snmpCommunityStatus;
      SET_VALID(I_snmpCommunityStatus, snmpCommunityEntryData->valid);
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
L7_RC_t l7_snmpCommunityTableCheckValid(L7_char8 *snmpCommunityIndex)
{
  snmpCommunityEntry_t * snmpCommunityEntryData = snmp_snmpCommunityTable_get(EXACT, snmpCommunityIndex);

  if (snmpCommunityEntryData != NULL)
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
L7_RC_t l7_snmpCommunityTableNextGet(L7_char8 *snmpCommunityIndex)
{
  snmpCommunityEntry_t * snmpCommunityEntryData = snmp_snmpCommunityTable_get(NEXT, snmpCommunityIndex);

  if (snmpCommunityEntryData != NULL)
  {
    strncpy(snmpCommunityIndex, snmpCommunityEntryData->snmpCommunityIndex->octet_ptr,
            snmpCommunityEntryData->snmpCommunityIndex->length);
    snmpCommunityIndex[snmpCommunityEntryData->snmpCommunityIndex->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Retrieves the row index for a given community name
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityTableCheckCommunity(L7_char8 *snmpCommunityName, L7_char8 *snmpCommunityIndex)
{
  L7_char8 temp_index[128];
  L7_char8 temp_community[128];

  memset(temp_index, 0, sizeof(temp_index));
  memset(temp_community, 0, sizeof(temp_community));

  while (l7_snmpCommunityTableNextGet(temp_index) == L7_SUCCESS)
  {
    if (l7_snmpCommunityNameGet(temp_index, temp_community) == L7_SUCCESS)
    {
      if (strcmp(temp_community, snmpCommunityName) == 0)
      {
        strcpy(snmpCommunityIndex, temp_index);
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The community string for which a row in this table
*        represents a configuration.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityNameGet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
  snmpCommunityEntry_t * snmpCommunityEntryData = snmp_snmpCommunityTable_get(EXACT, snmpCommunityIndex);

  if (snmpCommunityEntryData != NULL && 
      VALID(I_snmpCommunityName, snmpCommunityEntryData->valid))
  {
    strncpy(val, snmpCommunityEntryData->snmpCommunityName->octet_ptr,
            snmpCommunityEntryData->snmpCommunityName->length);
    val[snmpCommunityEntryData->snmpCommunityName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The community string for which a row in this table
*        represents a configuration.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityNameSet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
  snmpCommunityEntry_t snmpCommunityEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 1, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&snmpCommunityEntryData, 0, sizeof(snmpCommunityEntryData));

  snmpCommunityEntryData.snmpCommunityIndex = MakeOctetStringFromText(snmpCommunityIndex);
  SET_VALID(I_snmpCommunityIndex, snmpCommunityEntryData.valid);

  snmpCommunityEntryData.snmpCommunityName = MakeOctetStringFromText(val);
  SET_VALID(I_snmpCommunityName, snmpCommunityEntryData.valid);

  rc = snmp_snmpCommunityTable_set(&snmpCommunityEntryData);

  FreeOctetString(snmpCommunityEntryData.snmpCommunityIndex);
  FreeOctetString(snmpCommunityEntryData.snmpCommunityName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunitySecurityName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes A human readable string representing the corresponding
*        value of snmpCommunityName in a Security Model
*        independent format.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunitySecurityNameGet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
  snmpCommunityEntry_t * snmpCommunityEntryData = snmp_snmpCommunityTable_get(EXACT, snmpCommunityIndex);

  if (snmpCommunityEntryData != NULL && 
      VALID(I_snmpCommunitySecurityName, snmpCommunityEntryData->valid))
  {
    strncpy(val, snmpCommunityEntryData->snmpCommunitySecurityName->octet_ptr,
            snmpCommunityEntryData->snmpCommunitySecurityName->length);
    val[snmpCommunityEntryData->snmpCommunitySecurityName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunitySecurityName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes A human readable string representing the corresponding
*        value of snmpCommunityName in a Security Model
*        independent format.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunitySecurityNameSet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
  snmpCommunityEntry_t snmpCommunityEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 1, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&snmpCommunityEntryData, 0, sizeof(snmpCommunityEntryData));

  snmpCommunityEntryData.snmpCommunityIndex = MakeOctetStringFromText(snmpCommunityIndex);
  SET_VALID(I_snmpCommunityIndex, snmpCommunityEntryData.valid);

  snmpCommunityEntryData.snmpCommunitySecurityName = MakeOctetStringFromText(val);
  SET_VALID(I_snmpCommunitySecurityName, snmpCommunityEntryData.valid);

  rc = snmp_snmpCommunityTable_set(&snmpCommunityEntryData);

  FreeOctetString(snmpCommunityEntryData.snmpCommunityIndex);
  FreeOctetString(snmpCommunityEntryData.snmpCommunitySecurityName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityContextEngineID
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The contextEngineID indicating the location of the
*        context in which management information is accessed
*        when using the community string specified by the
*        corresponding instance of snmpCommunityName.
*        
*        The default value is the snmpEngineID of the entity in
*        which this object is instantiated.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityContextEngineIDGet(L7_char8* snmpCommunityIndex, L7_char8 *val, L7_uint32 *len)
{
  snmpCommunityEntry_t * snmpCommunityEntryData = snmp_snmpCommunityTable_get(EXACT, snmpCommunityIndex);

  if (snmpCommunityEntryData != NULL && 
      VALID(I_snmpCommunityContextEngineID, snmpCommunityEntryData->valid))
  {
    memcpy(val, snmpCommunityEntryData->snmpCommunityContextEngineID->octet_ptr,
           snmpCommunityEntryData->snmpCommunityContextEngineID->length);
    *len = snmpCommunityEntryData->snmpCommunityContextEngineID->length;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityContextEngineID
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The contextEngineID indicating the location of the
*        context in which management information is accessed
*        when using the community string specified by the
*        corresponding instance of snmpCommunityName.
*        
*        The default value is the snmpEngineID of the entity in
*        which this object is instantiated.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityContextEngineIDSet(L7_char8* snmpCommunityIndex, L7_char8 *val, L7_uint32 len)
{
  snmpCommunityEntry_t snmpCommunityEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 5, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&snmpCommunityEntryData, 0, sizeof(snmpCommunityEntryData));

  snmpCommunityEntryData.snmpCommunityIndex = MakeOctetStringFromText(snmpCommunityIndex);
  SET_VALID(I_snmpCommunityIndex, snmpCommunityEntryData.valid);

  snmpCommunityEntryData.snmpCommunityContextEngineID = MakeOctetString(val, len);
  SET_VALID(I_snmpCommunityContextEngineID, snmpCommunityEntryData.valid);

  rc = snmp_snmpCommunityTable_set(&snmpCommunityEntryData);

  FreeOctetString(snmpCommunityEntryData.snmpCommunityIndex);
  FreeOctetString(snmpCommunityEntryData.snmpCommunityContextEngineID);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityContextName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The context in which management information is accessed
*        when using the community string specified by the corresponding
*        instance of snmpCommunityName.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityContextNameGet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
  snmpCommunityEntry_t * snmpCommunityEntryData = snmp_snmpCommunityTable_get(EXACT, snmpCommunityIndex);

  if (snmpCommunityEntryData != NULL && 
      VALID(I_snmpCommunityContextName, snmpCommunityEntryData->valid))
  {
    strncpy(val, snmpCommunityEntryData->snmpCommunityContextName->octet_ptr,
            snmpCommunityEntryData->snmpCommunityContextName->length);
    val[snmpCommunityEntryData->snmpCommunityContextName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityContextName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The context in which management information is accessed
*        when using the community string specified by the corresponding
*        instance of snmpCommunityName.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityContextNameSet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
  snmpCommunityEntry_t snmpCommunityEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 0, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&snmpCommunityEntryData, 0, sizeof(snmpCommunityEntryData));

  snmpCommunityEntryData.snmpCommunityIndex = MakeOctetStringFromText(snmpCommunityIndex);
  SET_VALID(I_snmpCommunityIndex, snmpCommunityEntryData.valid);

  snmpCommunityEntryData.snmpCommunityContextName = MakeOctetStringFromText(val);
  SET_VALID(I_snmpCommunityContextName, snmpCommunityEntryData.valid);

  rc = snmp_snmpCommunityTable_set(&snmpCommunityEntryData);

  FreeOctetString(snmpCommunityEntryData.snmpCommunityIndex);
  FreeOctetString(snmpCommunityEntryData.snmpCommunityContextName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityTransportTag
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object specifies a set of transport endpoints
*        from which an agent will accept management requests.
*        If a management request containing this community
*        is received on a transport endpoint other than the
*        transport endpoints identified by this object, the
*        request is deemed unauthentic.
*        
*        The transports identified by this object are specified
*        in the snmpTargetAddrTable.  Entries in that table
*        whose snmpTargetAddrTagList contains this tag value
*        are identified.
*        
*        If the value of this object has zero-length, transport
*        endpoints are not checked when authenticating messages
*        containing this community string.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityTransportTagGet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
  snmpCommunityEntry_t * snmpCommunityEntryData = snmp_snmpCommunityTable_get(EXACT, snmpCommunityIndex);

  if (snmpCommunityEntryData != NULL && 
      VALID(I_snmpCommunityTransportTag, snmpCommunityEntryData->valid))
  {
    strncpy(val, snmpCommunityEntryData->snmpCommunityTransportTag->octet_ptr,
            snmpCommunityEntryData->snmpCommunityTransportTag->length);
    val[snmpCommunityEntryData->snmpCommunityTransportTag->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityTransportTag
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object specifies a set of transport endpoints
*        from which an agent will accept management requests.
*        If a management request containing this community
*        is received on a transport endpoint other than the
*        transport endpoints identified by this object, the
*        request is deemed unauthentic.
*        
*        The transports identified by this object are specified
*        in the snmpTargetAddrTable.  Entries in that table
*        whose snmpTargetAddrTagList contains this tag value
*        are identified.
*        
*        If the value of this object has zero-length, transport
*        endpoints are not checked when authenticating messages
*        containing this community string.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityTransportTagSet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
  snmpCommunityEntry_t snmpCommunityEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 0, 255) != L7_SUCCESS)
    return rc;

  memset((void*)&snmpCommunityEntryData, 0, sizeof(snmpCommunityEntryData));

  snmpCommunityEntryData.snmpCommunityIndex = MakeOctetStringFromText(snmpCommunityIndex);
  SET_VALID(I_snmpCommunityIndex, snmpCommunityEntryData.valid);

  snmpCommunityEntryData.snmpCommunityTransportTag = MakeOctetStringFromText(val);
  SET_VALID(I_snmpCommunityTransportTag, snmpCommunityEntryData.valid);

  rc = snmp_snmpCommunityTable_set(&snmpCommunityEntryData);

  FreeOctetString(snmpCommunityEntryData.snmpCommunityIndex);
  FreeOctetString(snmpCommunityEntryData.snmpCommunityTransportTag);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row in the
*        snmpCommunityTable.  Conceptual rows having the value
*        columnar object in the row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityStorageTypeGet(L7_char8* snmpCommunityIndex, l7_snmpStorageType_t *val)
{
  snmpCommunityEntry_t * snmpCommunityEntryData = snmp_snmpCommunityTable_get(EXACT, snmpCommunityIndex);

  if (snmpCommunityEntryData != NULL && 
      VALID(I_snmpCommunityStorageType, snmpCommunityEntryData->valid))
  {
    *val = snmpCommunityEntryData->snmpCommunityStorageType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row in the
*        snmpCommunityTable.  Conceptual rows having the value
*        columnar object in the row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityStorageTypeSet(L7_char8* snmpCommunityIndex, l7_snmpStorageType_t val)
{
  snmpCommunityEntry_t snmpCommunityEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpCommunityEntryData, 0, sizeof(snmpCommunityEntryData));

  snmpCommunityEntryData.snmpCommunityIndex = MakeOctetStringFromText(snmpCommunityIndex);
  SET_VALID(I_snmpCommunityIndex, snmpCommunityEntryData.valid);

  snmpCommunityEntryData.snmpCommunityStorageType = val;
  SET_VALID(I_snmpCommunityStorageType, snmpCommunityEntryData.valid);

  rc = snmp_snmpCommunityTable_set(&snmpCommunityEntryData);

  FreeOctetString(snmpCommunityEntryData.snmpCommunityIndex);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row in the snmpCommunityTable.
*        
*        An entry in this table is not qualified for activation
*        until instances of all corresponding columns have been
*        initialized, either through default values, or through
*        Set operations.  The snmpCommunityName and
*        snmpCommunitySecurityName objects must be explicitly set.
*        
*        There is no restriction on setting columns in this table
*        when the value of snmpCommunityStatus is active(1).
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityStatusGet(L7_char8* snmpCommunityIndex, l7_snmpRowStatus_t *val)
{
  snmpCommunityEntry_t * snmpCommunityEntryData = snmp_snmpCommunityTable_get(EXACT, snmpCommunityIndex);

  if (snmpCommunityEntryData != NULL && 
      VALID(I_snmpCommunityStatus, snmpCommunityEntryData->valid))
  {
    *val = snmpCommunityEntryData->snmpCommunityStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row in the snmpCommunityTable.
*        
*        An entry in this table is not qualified for activation
*        until instances of all corresponding columns have been
*        initialized, either through default values, or through
*        Set operations.  The snmpCommunityName and
*        snmpCommunitySecurityName objects must be explicitly set.
*        
*        There is no restriction on setting columns in this table
*        when the value of snmpCommunityStatus is active(1).
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpCommunityStatusSet(L7_char8* snmpCommunityIndex, l7_snmpRowStatus_t val)
{
  snmpCommunityEntry_t snmpCommunityEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpCommunityEntryData, 0, sizeof(snmpCommunityEntryData));

  if (l7_snmpStringSetValidate(snmpCommunityIndex, 1, 32) != L7_SUCCESS)
    return rc;

  snmpCommunityEntryData.snmpCommunityIndex = MakeOctetStringFromText(snmpCommunityIndex);
  SET_VALID(I_snmpCommunityIndex, snmpCommunityEntryData.valid);

  snmpCommunityEntryData.snmpCommunityStatus = val;
  SET_VALID(I_snmpCommunityStatus, snmpCommunityEntryData.valid);

  rc = snmp_snmpCommunityTable_set(&snmpCommunityEntryData);

  FreeOctetString(snmpCommunityEntryData.snmpCommunityIndex);

  return rc;
}

void
debug_snmpCommunityTable_walk()
{
  L7_char8 snmpCommunityIndex[256];
  L7_char8 temp_string[256];
  L7_uint32 temp_len;
  L7_uint32 temp_val;

  memset((void*)snmpCommunityIndex, 0, sizeof(snmpCommunityIndex));

  sysapiPrintf("[snmpCommunityTable start]\n");

  while(l7_snmpCommunityTableNextGet(snmpCommunityIndex) == L7_SUCCESS)
  {
    sysapiPrintf("snmpCommunityIndex.\"%s\" = \"%s\"\n", snmpCommunityIndex, snmpCommunityIndex);

    if (l7_snmpCommunityNameGet(snmpCommunityIndex, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpCommunityName.\"%s\" = \"%s\"\n", snmpCommunityIndex, temp_string);
    else
      sysapiPrintf("snmpCommunityName.\"%s\" = L7_FAILURE\n", snmpCommunityIndex);

    if (l7_snmpCommunitySecurityNameGet(snmpCommunityIndex, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpCommunitySecurityName.\"%s\" = \"%s\"\n", snmpCommunityIndex, temp_string);
    else
      sysapiPrintf("snmpCommunitySecurityName.\"%s\" = L7_FAILURE\n", snmpCommunityIndex);

    if (l7_snmpCommunityContextEngineIDGet(snmpCommunityIndex, temp_string, &temp_len) == L7_SUCCESS)
    {
      sysapiPrintf("snmpCommunityContextEngineID.\"%s\" = \"", snmpCommunityIndex);
      debug_snmp_printHexString(temp_string, temp_len);
      sysapiPrintf("\"\n");
    }
    else
      sysapiPrintf("snmpCommunityContextEngineID.\"%s\" = L7_FAILURE\n", snmpCommunityIndex);

    if (l7_snmpCommunityContextNameGet(snmpCommunityIndex, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpCommunityContextName.\"%s\" = \"%s\"\n", snmpCommunityIndex, temp_string);
    else
      sysapiPrintf("snmpCommunityContextName.\"%s\" = L7_FAILURE\n", snmpCommunityIndex);

    if (l7_snmpCommunityTransportTagGet(snmpCommunityIndex, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpCommunityTransportTag.\"%s\" = \"%s\"\n", snmpCommunityIndex, temp_string);
    else
      sysapiPrintf("snmpCommunityTransportTag.\"%s\" = L7_FAILURE\n", snmpCommunityIndex);

    if (l7_snmpCommunityStorageTypeGet(snmpCommunityIndex, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpCommunityStorageType.\"%s\" = %d\n", snmpCommunityIndex, temp_val);
    else
      sysapiPrintf("snmpCommunityStorageType.\"%s\" = L7_FAILURE\n", snmpCommunityIndex);

    if (l7_snmpCommunityStatusGet(snmpCommunityIndex, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpCommunityStatus.\"%s\" = %d\n", snmpCommunityIndex, temp_val);
    else
      sysapiPrintf("snmpCommunityStatus.\"%s\" = L7_FAILURE\n", snmpCommunityIndex);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_snmpCommunityTable_create(L7_char8* snmpCommunityIndex, L7_char8* snmpCommunityName)
{
  L7_RC_t rc;

  rc = l7_snmpCommunityStatusSet(snmpCommunityIndex, snmpRowStatus_createAndWait);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpCommunityStatusSet 1 failed!\n");
    return;
  }

  rc = l7_snmpCommunityNameSet(snmpCommunityIndex, snmpCommunityName);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpCommunityNameSet 1 failed!\n");
    return;
  }

  rc = l7_snmpCommunitySecurityNameSet(snmpCommunityIndex, snmpCommunityIndex);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpCommunitySecurityNameSet 1 failed!\n");
    return;
  }

  rc = l7_snmpCommunityTransportTagSet(snmpCommunityIndex, "");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpCommunityTransportTagSet 1 failed!\n");
    return;
  }

  rc = l7_snmpCommunityStatusSet(snmpCommunityIndex, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpCommunityStatusSet 1 failed!\n");
    return;
  }

  sysapiPrintf("debug_snmpCommunityTable_create finished!\n");
}


void
debug_snmpCommunityTableCheckCommunity(L7_char8* snmpCommunityName)
{
  L7_char8 temp_index[128];
  L7_RC_t rc;
  memset(temp_index, 0, sizeof(temp_index));

  rc = l7_snmpCommunityTableCheckCommunity(snmpCommunityName, temp_index);

  if (rc == L7_SUCCESS)
  {
    sysapiPrintf("Name: %s -> Index: %s\n", snmpCommunityName, temp_index);
  }
  return;
}

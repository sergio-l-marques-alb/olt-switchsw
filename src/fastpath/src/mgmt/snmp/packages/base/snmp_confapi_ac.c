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

extern SnmpV2Table agt_vacmAccessTable;              /* vacmAccessEntry_t */

extern int writeConfigFileFlag;

/*******************************************************************************************/

/*
        OctetString     *vacmAccessContextPrefix;  (index)
        SR_INT32        vacmAccessSecurityModel;   (index)
        SR_INT32        vacmAccessSecurityLevel;   (index)
        SR_INT32        vacmAccessContextMatch;
        OctetString     *vacmAccessReadViewName;
        OctetString     *vacmAccessWriteViewName;
        OctetString     *vacmAccessNotifyViewName;
        SR_INT32        vacmAccessStorageType;
        SR_INT32        vacmAccessStatus;
        OctetString *   vacmGroupName;              (index)
*/

/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to vacmAccessEntry_t object
*           
* @notes The table of access rights for groups.
*        
*        Each entry is indexed by a groupName, a contextPrefix,
*        a securityModel and a securityLevel.  To determine
*        whether access is allowed, one entry from this table
*        needs to be selected and the proper viewName from that
*        entry must be used for access control checking.
*        
*        To select the proper entry, follow these steps:
*        
*        1) the set of possible matches is formed by the
*           intersection of the following sets of entries:
*             the set of entries with identical vacmGroupName
*             the union of these two sets:
*                value of 'prefix' and matching
*                vacmAccessContextPrefix
*             intersected with the union of these two sets:
*                vacmSecurityModel
*                value of 'any'
*             intersected with the set of entries with
*             vacmAccessSecurityLevel value less than or equal
*             to the requested securityLevel
*        
*        2) if this set has only one member, we're done
*           otherwise, it comes down to deciding how to weight
*           the preferences between ContextPrefixes,
*           SecurityModels, and SecurityLevels as follows:
*           a) if the subset of entries with securityModel
*              matching the securityModel in the message is
*              not empty, then discard the rest.
*           b) if the subset of entries with
*              vacmAccessContextPrefix matching the contextName
*              in the message is not empty,
*              then discard the rest
*           c) discard all entries with ContextPrefixes shorter
*              than the longest one remaining in the set
*           d) select the entry with the highest securityLevel
*        
*        Please note that for securityLevel noAuthNoPriv, all
*        groups are really equivalent since the assumption that
*        the securityName has been authenticated does not hold.
*
* @end
*
*********************************************************************/
static vacmAccessEntry_t* snmp_vacmAccessTable_get(int searchType, L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel)
{
  L7_int32 index = -1;
  vacmAccessEntry_t *row = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || vacmGroupName == NULL || vacmGroupName == NULL)
    return NULL;

  /* build the search structure */
  agt_vacmAccessTable.tip[0].value.octet_val = MakeOctetStringFromText(vacmGroupName);
  if (agt_vacmAccessTable.tip[0].value.octet_val == NULL)
    return NULL;

  agt_vacmAccessTable.tip[1].value.octet_val = MakeOctetStringFromText(vacmAccessContextPrefix);
  if (agt_vacmAccessTable.tip[1].value.octet_val == NULL)
  {
    FreeOctetString(agt_vacmAccessTable.tip[0].value.octet_val);
    return NULL;
  }
  agt_vacmAccessTable.tip[2].value.uint_val = vacmAccessSecurityModel;
  agt_vacmAccessTable.tip[3].value.uint_val = vacmAccessSecurityLevel;

  index = SearchTable(&agt_vacmAccessTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (vacmAccessEntry_t*)agt_vacmAccessTable.tp[index];

    if (searchType == NEXT &&
        strncmp(vacmGroupName, 
                row->vacmGroupName->octet_ptr,
                row->vacmGroupName->length) == 0 &&
        strncmp(vacmAccessContextPrefix, 
                row->vacmAccessContextPrefix->octet_ptr,
                row->vacmAccessContextPrefix->length) == 0 &&
        row->vacmAccessSecurityModel == vacmAccessSecurityModel &&
        row->vacmAccessSecurityLevel == vacmAccessSecurityLevel)
    index = SearchTable(&agt_vacmAccessTable, NEXT_SKIP);
  }

  FreeOctetString(agt_vacmAccessTable.tip[0].value.octet_val);
  FreeOctetString(agt_vacmAccessTable.tip[1].value.octet_val);

  if (index >= 0)
  {
    return(vacmAccessEntry_t*)agt_vacmAccessTable.tp[index];
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
static int l7_vacmAccessEntry_set_defaults(vacmAccessEntry_t *data)
{
  if ((data->vacmAccessReadViewName = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->vacmAccessWriteViewName = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->vacmAccessNotifyViewName = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->vacmAccessContextMatch = D_vacmAccessContextMatch_exact;
  data->vacmAccessStorageType = D_vacmAccessStorageType_nonVolatile;
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
* @notes The table of access rights for groups.
*        
*        Each entry is indexed by a groupName, a contextPrefix,
*        a securityModel and a securityLevel.  To determine
*        whether access is allowed, one entry from this table
*        needs to be selected and the proper viewName from that
*        entry must be used for access control checking.
*        
*        To select the proper entry, follow these steps:
*        
*        1) the set of possible matches is formed by the
*           intersection of the following sets of entries:
*             the set of entries with identical vacmGroupName
*             the union of these two sets:
*                value of 'prefix' and matching
*                vacmAccessContextPrefix
*             intersected with the union of these two sets:
*                vacmSecurityModel
*                value of 'any'
*             intersected with the set of entries with
*             vacmAccessSecurityLevel value less than or equal
*             to the requested securityLevel
*        
*        2) if this set has only one member, we're done
*           otherwise, it comes down to deciding how to weight
*           the preferences between ContextPrefixes,
*           SecurityModels, and SecurityLevels as follows:
*           a) if the subset of entries with securityModel
*              matching the securityModel in the message is
*              not empty, then discard the rest.
*           b) if the subset of entries with
*              vacmAccessContextPrefix matching the contextName
*              in the message is not empty,
*              then discard the rest
*           c) discard all entries with ContextPrefixes shorter
*              than the longest one remaining in the set
*           d) select the entry with the highest securityLevel
*        
*        Please note that for securityLevel noAuthNoPriv, all
*        groups are really equivalent since the assumption that
*        the securityName has been authenticated does not hold.
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_vacmAccessTable_set(vacmAccessEntry_t *data)
{
  L7_int32 index = -1;
  vacmAccessEntry_t * vacmAccessEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_vacmAccessEntryIndex_vacmGroupName, data->valid) ||
      !VALID(I_vacmAccessContextPrefix, data->valid))
    return L7_ERROR;

  /* build the search structure */
  agt_vacmAccessTable.tip[0].value.octet_val = CloneOctetString(data->vacmGroupName);
  if (agt_vacmAccessTable.tip[0].value.octet_val == NULL)
    return L7_ERROR;

  agt_vacmAccessTable.tip[1].value.octet_val = CloneOctetString(data->vacmAccessContextPrefix);
  if (agt_vacmAccessTable.tip[1].value.octet_val == NULL)
  {
    FreeOctetString(agt_vacmAccessTable.tip[0].value.octet_val);
    return L7_ERROR;
  }
  agt_vacmAccessTable.tip[2].value.uint_val = data->vacmAccessSecurityModel;
  agt_vacmAccessTable.tip[3].value.uint_val = data->vacmAccessSecurityLevel;

  index = SearchTable(&agt_vacmAccessTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_vacmAccessStatus, data->valid) && 
        (data->vacmAccessStatus == D_vacmAccessStatus_createAndGo ||
         data->vacmAccessStatus == D_vacmAccessStatus_createAndWait))
    {
      index = NewTableEntry(&agt_vacmAccessTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_vacmAccessEntry_set_defaults((vacmAccessEntry_t*)agt_vacmAccessTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->vacmAccessStatus == D_vacmAccessStatus_createAndGo)
      {
        ((vacmAccessEntry_t*)agt_vacmAccessTable.tp[index])->vacmAccessStatus = D_vacmAccessStatus_active;
      }
      else
      {
        ((vacmAccessEntry_t*)agt_vacmAccessTable.tp[index])->vacmAccessStatus = D_vacmAccessStatus_notInService;
      }
      SET_VALID(I_vacmAccessStatus, ((vacmAccessEntry_t*)agt_vacmAccessTable.tp[index])->valid);

      CLR_VALID(I_vacmAccessStatus, data->valid);
    }
  }

  FreeOctetString(agt_vacmAccessTable.tip[0].value.octet_val);
  FreeOctetString(agt_vacmAccessTable.tip[1].value.octet_val);

  if (index >= 0)
  {
    if (VALID(I_vacmAccessStatus, data->valid) && 
        data->vacmAccessStatus == D_vacmAccessStatus_destroy)
    {
      RemoveTableEntry(&agt_vacmAccessTable, index);
      return L7_SUCCESS;
    }

    vacmAccessEntryData = (vacmAccessEntry_t*)agt_vacmAccessTable.tp[index];

    if (VALID(I_vacmAccessContextMatch, data->valid))
    {
      vacmAccessEntryData->vacmAccessContextMatch = data->vacmAccessContextMatch;
      SET_VALID(I_vacmAccessContextMatch, vacmAccessEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_vacmAccessReadViewName, data->valid))
    {
      if (vacmAccessEntryData->vacmAccessReadViewName != NULL)
      {
        FreeOctetString(vacmAccessEntryData->vacmAccessReadViewName);
        CLR_VALID(I_vacmAccessReadViewName, vacmAccessEntryData->valid);
      }

      vacmAccessEntryData->vacmAccessReadViewName = CloneOctetString(data->vacmAccessReadViewName);

      if (vacmAccessEntryData->vacmAccessReadViewName != NULL)
      {
        SET_VALID(I_vacmAccessReadViewName, vacmAccessEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_vacmAccessWriteViewName, data->valid))
    {
      if (vacmAccessEntryData->vacmAccessWriteViewName != NULL)
      {
        FreeOctetString(vacmAccessEntryData->vacmAccessWriteViewName);
        CLR_VALID(I_vacmAccessWriteViewName, vacmAccessEntryData->valid);
      }

      vacmAccessEntryData->vacmAccessWriteViewName = CloneOctetString(data->vacmAccessWriteViewName);

      if (vacmAccessEntryData->vacmAccessWriteViewName != NULL)
      {
        SET_VALID(I_vacmAccessWriteViewName, vacmAccessEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_vacmAccessNotifyViewName, data->valid))
    {
      if (vacmAccessEntryData->vacmAccessNotifyViewName != NULL)
      {
        FreeOctetString(vacmAccessEntryData->vacmAccessNotifyViewName);
        CLR_VALID(I_vacmAccessNotifyViewName, vacmAccessEntryData->valid);
      }

      vacmAccessEntryData->vacmAccessNotifyViewName = CloneOctetString(data->vacmAccessNotifyViewName);

      if (vacmAccessEntryData->vacmAccessNotifyViewName != NULL)
      {
        SET_VALID(I_vacmAccessNotifyViewName, vacmAccessEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_vacmAccessStorageType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(vacmAccessEntryData->vacmAccessStorageType,
                                        data->vacmAccessStorageType) != L7_SUCCESS)
        return L7_ERROR;

      vacmAccessEntryData->vacmAccessStorageType = data->vacmAccessStorageType;
      SET_VALID(I_vacmAccessStorageType, vacmAccessEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_vacmAccessStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(vacmAccessEntryData->vacmAccessStatus,
                                      data->vacmAccessStatus) != L7_SUCCESS)
        return L7_ERROR;

      vacmAccessEntryData->vacmAccessStatus = data->vacmAccessStatus;
      SET_VALID(I_vacmAccessStatus, vacmAccessEntryData->valid);
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
L7_RC_t l7_vacmAccessTableCheckValid(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel)
{
  vacmAccessEntry_t * vacmAccessEntryData = snmp_vacmAccessTable_get(EXACT, vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

  if (vacmAccessEntryData != NULL)
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
L7_RC_t l7_vacmAccessTableNextGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 *vacmAccessSecurityModel, L7_uint32 *vacmAccessSecurityLevel)
#else
L7_RC_t l7_vacmAccessTableNextGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t *vacmAccessSecurityModel, L7_uint32 *vacmAccessSecurityLevel)
#endif
{
  vacmAccessEntry_t * vacmAccessEntryData = snmp_vacmAccessTable_get(NEXT, vacmGroupName, vacmAccessContextPrefix, *vacmAccessSecurityModel, *vacmAccessSecurityLevel);

  if (vacmAccessEntryData != NULL)
  {
    strncpy(vacmGroupName, vacmAccessEntryData->vacmGroupName->octet_ptr,
            vacmAccessEntryData->vacmGroupName->length);
    vacmGroupName[vacmAccessEntryData->vacmGroupName->length] = '\0';

    strncpy(vacmAccessContextPrefix, vacmAccessEntryData->vacmAccessContextPrefix->octet_ptr,
            vacmAccessEntryData->vacmAccessContextPrefix->length);
    vacmAccessContextPrefix[vacmAccessEntryData->vacmAccessContextPrefix->length] = '\0';

    *vacmAccessSecurityModel = vacmAccessEntryData->vacmAccessSecurityModel;

    *vacmAccessSecurityLevel = vacmAccessEntryData->vacmAccessSecurityLevel;

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessContextMatch
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes If the value of this object is exact(1), then all
*        rows where the contextName exactly matches
*        vacmAccessContextPrefix are selected.
*        
*        If the value of this object is prefix(2), then all
*        rows where the contextName whose starting octets
*        exactly match vacmAccessContextPrefix are selected.
*        This allows for a simple form of wildcarding.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessContextMatchGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, l7_vacmAccessContextMatch_t *val)
{
  vacmAccessEntry_t * vacmAccessEntryData = snmp_vacmAccessTable_get(EXACT, vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

  if (vacmAccessEntryData != NULL && 
      VALID(I_vacmAccessContextMatch, vacmAccessEntryData->valid))
  {
    *val = vacmAccessEntryData->vacmAccessContextMatch;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessContextMatch
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes If the value of this object is exact(1), then all
*        rows where the contextName exactly matches
*        vacmAccessContextPrefix are selected.
*        
*        If the value of this object is prefix(2), then all
*        rows where the contextName whose starting octets
*        exactly match vacmAccessContextPrefix are selected.
*        This allows for a simple form of wildcarding.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessContextMatchSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, l7_vacmAccessContextMatch_t val)
{
  vacmAccessEntry_t vacmAccessEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (val != vacmAccessContextMatch_exact &&
      val != vacmAccessContextMatch_prefix)
    return L7_FAILURE;

  memset((void*)&vacmAccessEntryData, 0, sizeof(vacmAccessEntryData));

  vacmAccessEntryData.vacmGroupName = MakeOctetStringFromText(vacmGroupName);
  SET_VALID(I_vacmAccessEntryIndex_vacmGroupName, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessContextPrefix = MakeOctetStringFromText(vacmAccessContextPrefix);
  SET_VALID(I_vacmAccessContextPrefix, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityModel = vacmAccessSecurityModel;
  SET_VALID(I_vacmAccessSecurityModel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityLevel = vacmAccessSecurityLevel;
  SET_VALID(I_vacmAccessSecurityLevel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessContextMatch = val;
  SET_VALID(I_vacmAccessContextMatch, vacmAccessEntryData.valid);

  rc = snmp_vacmAccessTable_set(&vacmAccessEntryData);

  FreeOctetString(vacmAccessEntryData.vacmGroupName);
  FreeOctetString(vacmAccessEntryData.vacmAccessContextPrefix);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessReadViewName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes read access.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessReadViewNameGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, L7_char8 *val)
{
  vacmAccessEntry_t * vacmAccessEntryData = snmp_vacmAccessTable_get(EXACT, vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

  if (vacmAccessEntryData != NULL && 
      VALID(I_vacmAccessReadViewName, vacmAccessEntryData->valid))
  {
    strncpy(val, vacmAccessEntryData->vacmAccessReadViewName->octet_ptr,
            vacmAccessEntryData->vacmAccessReadViewName->length);
    val[vacmAccessEntryData->vacmAccessReadViewName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessReadViewName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes read access.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessReadViewNameSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, L7_char8 *val)
{
  vacmAccessEntry_t vacmAccessEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 0, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&vacmAccessEntryData, 0, sizeof(vacmAccessEntryData));

  vacmAccessEntryData.vacmGroupName = MakeOctetStringFromText(vacmGroupName);
  SET_VALID(I_vacmAccessEntryIndex_vacmGroupName, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessContextPrefix = MakeOctetStringFromText(vacmAccessContextPrefix);
  SET_VALID(I_vacmAccessContextPrefix, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityModel = vacmAccessSecurityModel;
  SET_VALID(I_vacmAccessSecurityModel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityLevel = vacmAccessSecurityLevel;
  SET_VALID(I_vacmAccessSecurityLevel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessReadViewName = MakeOctetStringFromText(val);
  SET_VALID(I_vacmAccessReadViewName, vacmAccessEntryData.valid);

  rc = snmp_vacmAccessTable_set(&vacmAccessEntryData);

  FreeOctetString(vacmAccessEntryData.vacmGroupName);
  FreeOctetString(vacmAccessEntryData.vacmAccessContextPrefix);
  FreeOctetString(vacmAccessEntryData.vacmAccessReadViewName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessWriteViewName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes write access.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessWriteViewNameGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, L7_char8 *val)
{
  vacmAccessEntry_t * vacmAccessEntryData = snmp_vacmAccessTable_get(EXACT, vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

  if (vacmAccessEntryData != NULL && 
      VALID(I_vacmAccessWriteViewName, vacmAccessEntryData->valid))
  {
    strncpy(val, vacmAccessEntryData->vacmAccessWriteViewName->octet_ptr,
            vacmAccessEntryData->vacmAccessWriteViewName->length);
    val[vacmAccessEntryData->vacmAccessWriteViewName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessWriteViewName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes write access.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessWriteViewNameSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, L7_char8 *val)
{
  vacmAccessEntry_t vacmAccessEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 0, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&vacmAccessEntryData, 0, sizeof(vacmAccessEntryData));

  vacmAccessEntryData.vacmGroupName = MakeOctetStringFromText(vacmGroupName);
  SET_VALID(I_vacmAccessEntryIndex_vacmGroupName, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessContextPrefix = MakeOctetStringFromText(vacmAccessContextPrefix);
  SET_VALID(I_vacmAccessContextPrefix, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityModel = vacmAccessSecurityModel;
  SET_VALID(I_vacmAccessSecurityModel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityLevel = vacmAccessSecurityLevel;
  SET_VALID(I_vacmAccessSecurityLevel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessWriteViewName = MakeOctetStringFromText(val);
  SET_VALID(I_vacmAccessWriteViewName, vacmAccessEntryData.valid);

  rc = snmp_vacmAccessTable_set(&vacmAccessEntryData);

  FreeOctetString(vacmAccessEntryData.vacmGroupName);
  FreeOctetString(vacmAccessEntryData.vacmAccessContextPrefix);
  FreeOctetString(vacmAccessEntryData.vacmAccessWriteViewName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessNotifyViewName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes access for notifications.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessNotifyViewNameGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, L7_char8 *val)
{
  vacmAccessEntry_t * vacmAccessEntryData = snmp_vacmAccessTable_get(EXACT, vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

  if (vacmAccessEntryData != NULL && 
      VALID(I_vacmAccessNotifyViewName, vacmAccessEntryData->valid))
  {
    strncpy(val, vacmAccessEntryData->vacmAccessNotifyViewName->octet_ptr,
            vacmAccessEntryData->vacmAccessNotifyViewName->length);
    val[vacmAccessEntryData->vacmAccessNotifyViewName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessNotifyViewName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes access for notifications.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessNotifyViewNameSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, L7_char8 *val)
{
  vacmAccessEntry_t vacmAccessEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 0, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&vacmAccessEntryData, 0, sizeof(vacmAccessEntryData));

  vacmAccessEntryData.vacmGroupName = MakeOctetStringFromText(vacmGroupName);
  SET_VALID(I_vacmAccessEntryIndex_vacmGroupName, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessContextPrefix = MakeOctetStringFromText(vacmAccessContextPrefix);
  SET_VALID(I_vacmAccessContextPrefix, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityModel = vacmAccessSecurityModel;
  SET_VALID(I_vacmAccessSecurityModel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityLevel = vacmAccessSecurityLevel;
  SET_VALID(I_vacmAccessSecurityLevel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessNotifyViewName = MakeOctetStringFromText(val);
  SET_VALID(I_vacmAccessNotifyViewName, vacmAccessEntryData.valid);

  rc = snmp_vacmAccessTable_set(&vacmAccessEntryData);

  FreeOctetString(vacmAccessEntryData.vacmGroupName);
  FreeOctetString(vacmAccessEntryData.vacmAccessContextPrefix);
  FreeOctetString(vacmAccessEntryData.vacmAccessNotifyViewName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessStorageTypeGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, l7_snmpStorageType_t *val)
{
  vacmAccessEntry_t * vacmAccessEntryData = snmp_vacmAccessTable_get(EXACT, vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

  if (vacmAccessEntryData != NULL && 
      VALID(I_vacmAccessStorageType, vacmAccessEntryData->valid))
  {
    *val = vacmAccessEntryData->vacmAccessStorageType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmAccessStorageTypeSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, l7_snmpStorageType_t val)
{
  vacmAccessEntry_t vacmAccessEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&vacmAccessEntryData, 0, sizeof(vacmAccessEntryData));

  vacmAccessEntryData.vacmGroupName = MakeOctetStringFromText(vacmGroupName);
  SET_VALID(I_vacmAccessEntryIndex_vacmGroupName, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessContextPrefix = MakeOctetStringFromText(vacmAccessContextPrefix);
  SET_VALID(I_vacmAccessContextPrefix, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityModel = vacmAccessSecurityModel;
  SET_VALID(I_vacmAccessSecurityModel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityLevel = vacmAccessSecurityLevel;
  SET_VALID(I_vacmAccessSecurityLevel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessStorageType = val;
  SET_VALID(I_vacmAccessStorageType, vacmAccessEntryData.valid);

  rc = snmp_vacmAccessTable_set(&vacmAccessEntryData);

  FreeOctetString(vacmAccessEntryData.vacmGroupName);
  FreeOctetString(vacmAccessEntryData.vacmAccessContextPrefix);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
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
L7_RC_t l7_vacmAccessStatusGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, l7_snmpRowStatus_t *val)
{
  vacmAccessEntry_t * vacmAccessEntryData = snmp_vacmAccessTable_get(EXACT, vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

  if (vacmAccessEntryData != NULL && 
      VALID(I_vacmAccessStatus, vacmAccessEntryData->valid))
  {
    *val = vacmAccessEntryData->vacmAccessStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
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
L7_RC_t l7_vacmAccessStatusSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel, l7_snmpRowStatus_t val)
{
  vacmAccessEntry_t vacmAccessEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&vacmAccessEntryData, 0, sizeof(vacmAccessEntryData));

  if (l7_snmpStringSetValidate(vacmGroupName, 1, 32) != L7_SUCCESS ||
      l7_snmpStringSetValidate(vacmAccessContextPrefix, 0, 32) != L7_SUCCESS)
    return rc;

  vacmAccessEntryData.vacmGroupName = MakeOctetStringFromText(vacmGroupName);
  SET_VALID(I_vacmAccessEntryIndex_vacmGroupName, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessContextPrefix = MakeOctetStringFromText(vacmAccessContextPrefix);
  SET_VALID(I_vacmAccessContextPrefix, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityModel = vacmAccessSecurityModel;
  SET_VALID(I_vacmAccessSecurityModel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessSecurityLevel = vacmAccessSecurityLevel;
  SET_VALID(I_vacmAccessSecurityLevel, vacmAccessEntryData.valid);

  vacmAccessEntryData.vacmAccessStatus = val;
  SET_VALID(I_vacmAccessStatus, vacmAccessEntryData.valid);

  rc = snmp_vacmAccessTable_set(&vacmAccessEntryData);

  FreeOctetString(vacmAccessEntryData.vacmGroupName);
  FreeOctetString(vacmAccessEntryData.vacmAccessContextPrefix);

  return rc;
}

void
debug_vacmAccessTable_walk()
{
  L7_char8 vacmGroupName[256];
  L7_char8 vacmAccessContextPrefix[256];
  l7_snmpSecurityModel_t vacmAccessSecurityModel=0;
  l7_snmpSecurityLevel_t vacmAccessSecurityLevel=0;
  L7_char8 temp_string[256];
  L7_uint32 temp_val;

  memset((void*)vacmGroupName, 0, sizeof(vacmGroupName));
  memset((void*)vacmAccessContextPrefix, 0, sizeof(vacmAccessContextPrefix));

  sysapiPrintf("[vacmAccessTable start]\n");

  while(l7_vacmAccessTableNextGet(vacmGroupName, vacmAccessContextPrefix, &vacmAccessSecurityModel, &vacmAccessSecurityLevel) == L7_SUCCESS)
  {
    sysapiPrintf("vacmGroupName.\"%s\".\"%s\".%d.%d = \"%s\"\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, vacmGroupName);
    sysapiPrintf("vacmAccessContextPrefix.\"%s\".\"%s\".%d.%d = \"%s\"\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, vacmAccessContextPrefix);
    sysapiPrintf("vacmAccessSecurityModel.\"%s\".\"%s\".%d.%d = %d\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, vacmAccessSecurityModel);
    sysapiPrintf("vacmAccessSecurityLevel.\"%s\".\"%s\".%d.%d = %d\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, vacmAccessSecurityLevel);

    if (l7_vacmAccessContextMatchGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, &temp_val) == L7_SUCCESS)
      sysapiPrintf("vacmAccessContextMatch.\"%s\".\"%s\".%d.%d = %d\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, temp_val);
    else
      sysapiPrintf("vacmAccessContextMatch.\"%s\".\"%s\".%d.%d = L7_FAILURE\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

    if (l7_vacmAccessReadViewNameGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, temp_string) == L7_SUCCESS)
      sysapiPrintf("vacmAccessReadViewName.\"%s\".\"%s\".%d.%d = \"%s\"\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, temp_string);
    else
      sysapiPrintf("vacmAccessReadViewName.\"%s\".\"%s\".%d.%d = L7_FAILURE\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

    if (l7_vacmAccessWriteViewNameGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, temp_string) == L7_SUCCESS)
      sysapiPrintf("vacmAccessWriteViewName.\"%s\".\"%s\".%d.%d = \"%s\"\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, temp_string);
    else
      sysapiPrintf("vacmAccessWriteViewName.\"%s\".\"%s\".%d.%d = L7_FAILURE\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

    if (l7_vacmAccessNotifyViewNameGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, temp_string) == L7_SUCCESS)
      sysapiPrintf("vacmAccessNotifyViewName.\"%s\".\"%s\".%d.%d = \"%s\"\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, temp_string);
    else
      sysapiPrintf("vacmAccessNotifyViewName.\"%s\".\"%s\".%d.%d = L7_FAILURE\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

    if (l7_vacmAccessStorageTypeGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, &temp_val) == L7_SUCCESS)
      sysapiPrintf("vacmAccessStorageType.\"%s\".\"%s\".%d.%d = %d\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, temp_val);
    else
      sysapiPrintf("vacmAccessStorageType.\"%s\".\"%s\".%d.%d = L7_FAILURE\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

    if (l7_vacmAccessStatusGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, &temp_val) == L7_SUCCESS)
      sysapiPrintf("vacmAccessStatus.\"%s\".\"%s\".%d.%d = %d\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, temp_val);
    else
      sysapiPrintf("vacmAccessStatus.\"%s\".\"%s\".%d.%d = L7_FAILURE\n", vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_vacmAccessTable_create(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, l7_snmpSecurityModel_t vacmAccessSecurityModel, l7_snmpSecurityLevel_t vacmAccessSecurityLevel)
{
  L7_RC_t rc;

  rc =  l7_vacmAccessStatusSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, snmpRowStatus_createAndWait);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmAccessStatusSet 1 failed!");
    return;
  }

  rc = l7_vacmAccessContextMatchSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, vacmAccessContextMatch_exact);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmAccessContextMatchSet 1 failed!");
    return;
  }

  rc = l7_vacmAccessContextMatchSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, vacmAccessContextMatch_prefix);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmAccessContextMatchSet 2 failed!");
    return;
  }

  rc = l7_vacmAccessReadViewNameSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, "All");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmAccessReadViewNameSet failed!");
    return;
  }
  
  rc = l7_vacmAccessWriteViewNameSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, "All");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmAccessWriteViewNameSet failed!");
    return;
  }
  
  rc = l7_vacmAccessNotifyViewNameSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, "");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmAccessNotifyViewNameSet failed!");
    return;
  }

  rc =  l7_vacmAccessStatusSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmAccessStatusSet 2 failed!");
    return;
  }

  sysapiPrintf("debug_vacmAccessTable_create finished!");
}


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
#include "sr_ip.h"    /* for ConvToken_IP_TAddr */
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
#include "oid_lib.h"
#include "frmt_lib.h"

#include "l7_common.h"
#include "snmp_exports.h"
#include "sysapi.h"
#include "snmp_confapi.h"

extern SnmpV2Table agt_vacmViewTreeFamilyTable;      /* vacmViewTreeFamilyEntry_t */

extern int writeConfigFileFlag;

/*******************************************************************************************/
/*
        OctetString     *vacmViewTreeFamilyViewName;
        OID             *vacmViewTreeFamilySubtree;
        OctetString     *vacmViewTreeFamilyMask;
        SR_INT32        vacmViewTreeFamilyType;
        SR_INT32        vacmViewTreeFamilyStorageType;
        SR_INT32        vacmViewTreeFamilyStatus;
*/

/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to vacmViewTreeFamilyEntry_t object
*           
* @notes Locally held information about families of subtrees
*        within MIB views.
*        
*        Each MIB view is defined by two sets of view subtrees:
*        Every such view subtree, both the included and the
*        excluded ones, is defined in this table.
*        
*        To determine if a particular object instance is in
*        a particular MIB view, compare the object instance's
*        OBJECT IDENTIFIER with each of the MIB view's active
*        entries in this table.  If none match, then the
*        object instance is not in the MIB view.  If one or
*        more match, then the object instance is included in,
*        or excluded from, the MIB view according to the
*        value of vacmViewTreeFamilyType in the entry whose
*        value of vacmViewTreeFamilySubtree has the most
*        sub-identifiers.  If multiple entries match and have
*        the same number of sub-identifiers (when wildcarding
*        is specified with the value of vacmViewTreeFamilyMask),
*        then the lexicographically greatest instance of
*        vacmViewTreeFamilyType determines the inclusion or
*        exclusion.
*        
*        An object instance's OBJECT IDENTIFIER X matches an
*        active entry in this table when the number of
*        sub-identifiers in X is at least as many as in the
*        value of vacmViewTreeFamilySubtree for the entry,
*        and each sub-identifier in the value of
*        vacmViewTreeFamilySubtree matches its corresponding
*        sub-identifier in X.  Two sub-identifiers match
*        either if the corresponding bit of the value of
*        vacmViewTreeFamilyMask for the entry is zero (the
*        
*        A 'family' of subtrees is the set of subtrees defined
*        by a particular combination of values of
*        vacmViewTreeFamilySubtree and vacmViewTreeFamilyMask.
*        In the case where no 'wild card' is defined in the
*        vacmViewTreeFamilyMask, the family of subtrees reduces
*        to a single subtree.
*        
*        When creating or changing MIB views, an SNMP Command
*        Generator application should utilize the
*        vacmViewSpinLock to try to avoid collisions. See
*        DESCRIPTION clause of vacmViewSpinLock.
*        
*        When creating MIB views, it is strongly advised that
*        first the 'excluded' vacmViewTreeFamilyEntries are
*        created and then the 'included' entries.
*        
*        When deleting MIB views, it is strongly advised that
*        first the 'included' vacmViewTreeFamilyEntries are
*        deleted and then the 'excluded' entries.
*        
*        If a create for an entry for instance-level access
*        control is received and the implementation does not
*        support instance-level granularity, then an
*        inconsistentName error must be returned.
*
* @end
*
*********************************************************************/
static vacmViewTreeFamilyEntry_t* snmp_vacmViewTreeFamilyTable_get(int searchType, L7_char8 *vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree)
{
  L7_int32 index = -1;
  vacmViewTreeFamilyEntry_t *row = NULL;
  OID *vacmViewTreeFamilySubtree_oid = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || vacmViewTreeFamilyViewName == NULL || vacmViewTreeFamilySubtree == NULL)
    return NULL;

  /* build the search structure */
  agt_vacmViewTreeFamilyTable.tip[0].value.octet_val = MakeOctetStringFromText(vacmViewTreeFamilyViewName);
  if (agt_vacmViewTreeFamilyTable.tip[0].value.octet_val == NULL)
  {
    return NULL;
  }

  /* if empty string, make a valid OID */
  if (strcmp(vacmViewTreeFamilySubtree, "") == 0)
    agt_vacmViewTreeFamilyTable.tip[1].value.oid_val = MakeOID(NULL, 0);
  else
    agt_vacmViewTreeFamilyTable.tip[1].value.oid_val = MakeOIDFragFromDot(vacmViewTreeFamilySubtree);

  if (agt_vacmViewTreeFamilyTable.tip[1].value.oid_val == NULL)
  {
    FreeOctetString(agt_vacmViewTreeFamilyTable.tip[0].value.octet_val);
    return NULL;
  }
  vacmViewTreeFamilySubtree_oid = CloneOID(agt_vacmViewTreeFamilyTable.tip[1].value.oid_val);
  if (vacmViewTreeFamilySubtree_oid == NULL)
  {
    FreeOctetString(agt_vacmViewTreeFamilyTable.tip[0].value.octet_val);
    FreeOID(agt_vacmViewTreeFamilyTable.tip[1].value.oid_val);
    return NULL;
  }

  index = SearchTable(&agt_vacmViewTreeFamilyTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (vacmViewTreeFamilyEntry_t*)agt_vacmViewTreeFamilyTable.tp[index];

    if (searchType == NEXT &&
        strncmp(vacmViewTreeFamilyViewName, 
                row->vacmViewTreeFamilyViewName->octet_ptr,
                row->vacmViewTreeFamilyViewName->length) == 0 &&
        CmpOID(row->vacmViewTreeFamilySubtree, vacmViewTreeFamilySubtree_oid) == 0)
    index = SearchTable(&agt_vacmViewTreeFamilyTable, NEXT_SKIP);
  }

  FreeOctetString(agt_vacmViewTreeFamilyTable.tip[0].value.octet_val);
  FreeOID(agt_vacmViewTreeFamilyTable.tip[1].value.oid_val);
  FreeOID(vacmViewTreeFamilySubtree_oid);

  if (index >= 0)
  {
    return(vacmViewTreeFamilyEntry_t*)agt_vacmViewTreeFamilyTable.tp[index];
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
static int l7_vacmViewTreeFamilyEntry_set_defaults(vacmViewTreeFamilyEntry_t *data)
{
  if ((data->vacmViewTreeFamilyMask = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->vacmViewTreeFamilyType = D_vacmViewTreeFamilyType_included;
  data->vacmViewTreeFamilyStorageType = D_vacmViewTreeFamilyStorageType_nonVolatile;
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
* @notes Locally held information about families of subtrees
*        within MIB views.
*        
*        Each MIB view is defined by two sets of view subtrees:
*        Every such view subtree, both the included and the
*        excluded ones, is defined in this table.
*        
*        To determine if a particular object instance is in
*        a particular MIB view, compare the object instance's
*        OBJECT IDENTIFIER with each of the MIB view's active
*        entries in this table.  If none match, then the
*        object instance is not in the MIB view.  If one or
*        more match, then the object instance is included in,
*        or excluded from, the MIB view according to the
*        value of vacmViewTreeFamilyType in the entry whose
*        value of vacmViewTreeFamilySubtree has the most
*        sub-identifiers.  If multiple entries match and have
*        the same number of sub-identifiers (when wildcarding
*        is specified with the value of vacmViewTreeFamilyMask),
*        then the lexicographically greatest instance of
*        vacmViewTreeFamilyType determines the inclusion or
*        exclusion.
*        
*        An object instance's OBJECT IDENTIFIER X matches an
*        active entry in this table when the number of
*        sub-identifiers in X is at least as many as in the
*        value of vacmViewTreeFamilySubtree for the entry,
*        and each sub-identifier in the value of
*        vacmViewTreeFamilySubtree matches its corresponding
*        sub-identifier in X.  Two sub-identifiers match
*        either if the corresponding bit of the value of
*        vacmViewTreeFamilyMask for the entry is zero (the
*        
*        A 'family' of subtrees is the set of subtrees defined
*        by a particular combination of values of
*        vacmViewTreeFamilySubtree and vacmViewTreeFamilyMask.
*        In the case where no 'wild card' is defined in the
*        vacmViewTreeFamilyMask, the family of subtrees reduces
*        to a single subtree.
*        
*        When creating or changing MIB views, an SNMP Command
*        Generator application should utilize the
*        vacmViewSpinLock to try to avoid collisions. See
*        DESCRIPTION clause of vacmViewSpinLock.
*        
*        When creating MIB views, it is strongly advised that
*        first the 'excluded' vacmViewTreeFamilyEntries are
*        created and then the 'included' entries.
*        
*        When deleting MIB views, it is strongly advised that
*        first the 'included' vacmViewTreeFamilyEntries are
*        deleted and then the 'excluded' entries.
*        
*        If a create for an entry for instance-level access
*        control is received and the implementation does not
*        support instance-level granularity, then an
*        inconsistentName error must be returned.
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_vacmViewTreeFamilyTable_set(vacmViewTreeFamilyEntry_t *data)
{
  L7_int32 index = -1;
  vacmViewTreeFamilyEntry_t * vacmViewTreeFamilyEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_vacmViewTreeFamilyViewName, data->valid))
    return L7_ERROR;

  /* build the search structure */
  agt_vacmViewTreeFamilyTable.tip[0].value.octet_val = CloneOctetString(data->vacmViewTreeFamilyViewName);
  if (agt_vacmViewTreeFamilyTable.tip[0].value.octet_val == NULL)
  {
    return L7_ERROR;
  }
  agt_vacmViewTreeFamilyTable.tip[1].value.oid_val = CloneOID(data->vacmViewTreeFamilySubtree);
  if (agt_vacmViewTreeFamilyTable.tip[1].value.oid_val == NULL)
  {
    FreeOctetString(agt_vacmViewTreeFamilyTable.tip[0].value.octet_val);
    return L7_ERROR;
  }

  index = SearchTable(&agt_vacmViewTreeFamilyTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_vacmViewTreeFamilyStatus, data->valid) && 
        (data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_createAndGo ||
         data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_createAndWait))
    {
      index = NewTableEntry(&agt_vacmViewTreeFamilyTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_vacmViewTreeFamilyEntry_set_defaults((vacmViewTreeFamilyEntry_t*)agt_vacmViewTreeFamilyTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_createAndGo)
      {
        ((vacmViewTreeFamilyEntry_t*)agt_vacmViewTreeFamilyTable.tp[index])->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_active;
      }
      else
      {
        ((vacmViewTreeFamilyEntry_t*)agt_vacmViewTreeFamilyTable.tp[index])->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_notInService;
      }
      SET_VALID(I_vacmViewTreeFamilyStatus, ((vacmViewTreeFamilyEntry_t*)agt_vacmViewTreeFamilyTable.tp[index])->valid);

      CLR_VALID(I_vacmViewTreeFamilyStatus, data->valid);
    }
  }

  FreeOctetString(agt_vacmViewTreeFamilyTable.tip[0].value.octet_val);
  FreeOID(agt_vacmViewTreeFamilyTable.tip[1].value.oid_val);

  if (index >= 0)
  {
    if (VALID(I_vacmViewTreeFamilyStatus, data->valid) && 
        data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_destroy)
    {
      RemoveTableEntry(&agt_vacmViewTreeFamilyTable, index);
      return L7_SUCCESS;
    }

    vacmViewTreeFamilyEntryData = (vacmViewTreeFamilyEntry_t*)agt_vacmViewTreeFamilyTable.tp[index];

    if (VALID(I_vacmViewTreeFamilyMask, data->valid))
    {
      if (vacmViewTreeFamilyEntryData->vacmViewTreeFamilyMask != NULL)
      {
        FreeOctetString(vacmViewTreeFamilyEntryData->vacmViewTreeFamilyMask);
        CLR_VALID(I_vacmViewTreeFamilyMask, vacmViewTreeFamilyEntryData->valid);
      }

      vacmViewTreeFamilyEntryData->vacmViewTreeFamilyMask = CloneOctetString(data->vacmViewTreeFamilyMask);

      if (vacmViewTreeFamilyEntryData->vacmViewTreeFamilyMask != NULL)
      {
        SET_VALID(I_vacmViewTreeFamilyMask, vacmViewTreeFamilyEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_vacmViewTreeFamilyType, data->valid))
    {
      vacmViewTreeFamilyEntryData->vacmViewTreeFamilyType = data->vacmViewTreeFamilyType;
      SET_VALID(I_vacmViewTreeFamilyType, vacmViewTreeFamilyEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_vacmViewTreeFamilyStorageType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(vacmViewTreeFamilyEntryData->vacmViewTreeFamilyStorageType,
                                        data->vacmViewTreeFamilyStorageType) != L7_SUCCESS)
        return L7_ERROR;

      vacmViewTreeFamilyEntryData->vacmViewTreeFamilyStorageType = data->vacmViewTreeFamilyStorageType;
      SET_VALID(I_vacmViewTreeFamilyStorageType, vacmViewTreeFamilyEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_vacmViewTreeFamilyStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(vacmViewTreeFamilyEntryData->vacmViewTreeFamilyStatus,
                                      data->vacmViewTreeFamilyStatus) != L7_SUCCESS)
        return L7_ERROR;

      vacmViewTreeFamilyEntryData->vacmViewTreeFamilyStatus = data->vacmViewTreeFamilyStatus;
      SET_VALID(I_vacmViewTreeFamilyStatus, vacmViewTreeFamilyEntryData->valid);
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
L7_RC_t l7_vacmViewTreeFamilyTableCheckValid(L7_char8 *vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree)
{
  vacmViewTreeFamilyEntry_t * vacmViewTreeFamilyEntryData = snmp_vacmViewTreeFamilyTable_get(EXACT, vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);

  if (vacmViewTreeFamilyEntryData != NULL)
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
L7_RC_t l7_vacmViewTreeFamilyTableNextGet(L7_char8 *vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree)
{
  vacmViewTreeFamilyEntry_t * vacmViewTreeFamilyEntryData = snmp_vacmViewTreeFamilyTable_get(NEXT, vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);

  if (vacmViewTreeFamilyEntryData != NULL)
  {
    strncpy(vacmViewTreeFamilyViewName, vacmViewTreeFamilyEntryData->vacmViewTreeFamilyViewName->octet_ptr,
            vacmViewTreeFamilyEntryData->vacmViewTreeFamilyViewName->length);
    vacmViewTreeFamilyViewName[vacmViewTreeFamilyEntryData->vacmViewTreeFamilyViewName->length] = '\0';

    SPrintOID(vacmViewTreeFamilyEntryData->vacmViewTreeFamilySubtree, vacmViewTreeFamilySubtree);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyMask
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The bit mask which, in combination with the
*        corresponding instance of vacmViewTreeFamilySubtree,
*        defines a family of view subtrees.
*        
*        Each bit of this bit mask corresponds to a
*        sub-identifier of vacmViewTreeFamilySubtree, with the
*        most significant bit of the i-th octet of this octet
*        string value (extended if necessary, see below)
*        corresponding to the (8*i - 7)-th sub-identifier, and
*        the least significant bit of the i-th octet of this
*        octet string corresponding to the (8*i)-th
*        sub-identifier, where i is in the range 1 through 16.
*        
*        Each bit of this bit mask specifies whether or not
*        the corresponding sub-identifiers must match when
*        determining if an OBJECT IDENTIFIER is in this
*        family of view subtrees; a '1' indicates that an
*        exact match must occur; a '0' indicates 'wild card',
*        i.e., any sub-identifier value matches.
*        
*        Thus, the OBJECT IDENTIFIER X of an object instance
*        is contained in a family of view subtrees if, for
*        each sub-identifier of the value of
*        vacmViewTreeFamilySubtree, either:
*        
*          the i-th bit of vacmViewTreeFamilyMask is 0, or
*        
*          the i-th sub-identifier of X is equal to the i-th
*          sub-identifier of the value of
*          vacmViewTreeFamilySubtree.
*        
*        If the value of this bit mask is M bits long and
*        there are more than M sub-identifiers in the
*        corresponding instance of vacmViewTreeFamilySubtree,
*        then the bit mask is extended with 1's to be the
*        required length.
*        
*        Note that when the value of this object is the
*        zero-length string, this extension rule results in
*        a mask of all-1's being used (i.e., no 'wild card'),
*        and the family of view subtrees is the one view
*        subtree uniquely identified by the corresponding
*        instance of vacmViewTreeFamilySubtree.
*        
*        Note that masks of length greater than zero length
*        do not need to be supported. In this case this
*        object is made read-only.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmViewTreeFamilyMaskGet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, L7_char8 *val, L7_uint32 *len)
{
  vacmViewTreeFamilyEntry_t * vacmViewTreeFamilyEntryData = snmp_vacmViewTreeFamilyTable_get(EXACT, vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);

  if (vacmViewTreeFamilyEntryData != NULL && 
      VALID(I_vacmViewTreeFamilyMask, vacmViewTreeFamilyEntryData->valid))
  {
    strncpy(val, vacmViewTreeFamilyEntryData->vacmViewTreeFamilyMask->octet_ptr,
            vacmViewTreeFamilyEntryData->vacmViewTreeFamilyMask->length);
    *len = vacmViewTreeFamilyEntryData->vacmViewTreeFamilyMask->length;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyMask
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The bit mask which, in combination with the
*        corresponding instance of vacmViewTreeFamilySubtree,
*        defines a family of view subtrees.
*        
*        Each bit of this bit mask corresponds to a
*        sub-identifier of vacmViewTreeFamilySubtree, with the
*        most significant bit of the i-th octet of this octet
*        string value (extended if necessary, see below)
*        corresponding to the (8*i - 7)-th sub-identifier, and
*        the least significant bit of the i-th octet of this
*        octet string corresponding to the (8*i)-th
*        sub-identifier, where i is in the range 1 through 16.
*        
*        Each bit of this bit mask specifies whether or not
*        the corresponding sub-identifiers must match when
*        determining if an OBJECT IDENTIFIER is in this
*        family of view subtrees; a '1' indicates that an
*        exact match must occur; a '0' indicates 'wild card',
*        i.e., any sub-identifier value matches.
*        
*        Thus, the OBJECT IDENTIFIER X of an object instance
*        is contained in a family of view subtrees if, for
*        each sub-identifier of the value of
*        vacmViewTreeFamilySubtree, either:
*        
*          the i-th bit of vacmViewTreeFamilyMask is 0, or
*        
*          the i-th sub-identifier of X is equal to the i-th
*          sub-identifier of the value of
*          vacmViewTreeFamilySubtree.
*        
*        If the value of this bit mask is M bits long and
*        there are more than M sub-identifiers in the
*        corresponding instance of vacmViewTreeFamilySubtree,
*        then the bit mask is extended with 1's to be the
*        required length.
*        
*        Note that when the value of this object is the
*        zero-length string, this extension rule results in
*        a mask of all-1's being used (i.e., no 'wild card'),
*        and the family of view subtrees is the one view
*        subtree uniquely identified by the corresponding
*        instance of vacmViewTreeFamilySubtree.
*        
*        Note that masks of length greater than zero length
*        do not need to be supported. In this case this
*        object is made read-only.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmViewTreeFamilyMaskSet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, L7_char8 *val, L7_uint32 len)
{
  vacmViewTreeFamilyEntry_t vacmViewTreeFamilyEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (len > 16)
    return L7_ERROR;

  memset((void*)&vacmViewTreeFamilyEntryData, 0, sizeof(vacmViewTreeFamilyEntryData));

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilyViewName = MakeOctetStringFromText(vacmViewTreeFamilyViewName);
  SET_VALID(I_vacmViewTreeFamilyViewName, vacmViewTreeFamilyEntryData.valid);

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilySubtree = MakeOIDFromDot(vacmViewTreeFamilySubtree);
  SET_VALID(I_vacmViewTreeFamilySubtree, vacmViewTreeFamilyEntryData.valid);

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilyMask = MakeOctetString(val, len);
  SET_VALID(I_vacmViewTreeFamilyMask, vacmViewTreeFamilyEntryData.valid);

  rc = snmp_vacmViewTreeFamilyTable_set(&vacmViewTreeFamilyEntryData);

  FreeOctetString(vacmViewTreeFamilyEntryData.vacmViewTreeFamilyViewName);
  FreeOctetString(vacmViewTreeFamilyEntryData.vacmViewTreeFamilyMask);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyStorageType
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
L7_RC_t l7_vacmViewTreeFamilyStorageTypeGet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_snmpStorageType_t *val)
{
  vacmViewTreeFamilyEntry_t * vacmViewTreeFamilyEntryData = snmp_vacmViewTreeFamilyTable_get(EXACT, vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);

  if (vacmViewTreeFamilyEntryData != NULL && 
      VALID(I_vacmViewTreeFamilyStorageType, vacmViewTreeFamilyEntryData->valid))
  {
    *val = vacmViewTreeFamilyEntryData->vacmViewTreeFamilyStorageType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes Indicates whether the corresponding instances of
*        vacmViewTreeFamilySubtree and vacmViewTreeFamilyMask
*        define a family of view subtrees which is included in
*        or excluded from the MIB view.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmViewTreeFamilyTypeGet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_vacmViewTreeFamilyType_t *val)
{
  vacmViewTreeFamilyEntry_t * vacmViewTreeFamilyEntryData = snmp_vacmViewTreeFamilyTable_get(EXACT, vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);

  if (vacmViewTreeFamilyEntryData != NULL && 
      VALID(I_vacmViewTreeFamilyType, vacmViewTreeFamilyEntryData->valid))
  {
    *val = vacmViewTreeFamilyEntryData->vacmViewTreeFamilyType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Sets the value of the object vacmViewTreeFamilyType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes Indicates whether the corresponding instances of
*        vacmViewTreeFamilySubtree and vacmViewTreeFamilyMask
*        define a family of view subtrees which is included in
*        or excluded from the MIB view.
*
* @end
*
*********************************************************************/
L7_RC_t l7_vacmViewTreeFamilyTypeSet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_vacmViewTreeFamilyType_t val)
{
  vacmViewTreeFamilyEntry_t vacmViewTreeFamilyEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&vacmViewTreeFamilyEntryData, 0, sizeof(vacmViewTreeFamilyEntryData));

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilyViewName = MakeOctetStringFromText(vacmViewTreeFamilyViewName);
  SET_VALID(I_vacmViewTreeFamilyViewName, vacmViewTreeFamilyEntryData.valid);

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilySubtree = MakeOIDFromDot(vacmViewTreeFamilySubtree);
  SET_VALID(I_vacmViewTreeFamilySubtree, vacmViewTreeFamilyEntryData.valid);

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilyType = val;
  SET_VALID(I_vacmViewTreeFamilyType, vacmViewTreeFamilyEntryData.valid);

  rc = snmp_vacmViewTreeFamilyTable_set(&vacmViewTreeFamilyEntryData);

  FreeOctetString(vacmViewTreeFamilyEntryData.vacmViewTreeFamilyViewName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyStorageType
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
L7_RC_t l7_vacmViewTreeFamilyStorageTypeSet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_snmpStorageType_t val)
{
  vacmViewTreeFamilyEntry_t vacmViewTreeFamilyEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&vacmViewTreeFamilyEntryData, 0, sizeof(vacmViewTreeFamilyEntryData));

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilyViewName = MakeOctetStringFromText(vacmViewTreeFamilyViewName);
  SET_VALID(I_vacmViewTreeFamilyViewName, vacmViewTreeFamilyEntryData.valid);

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilySubtree = MakeOIDFromDot(vacmViewTreeFamilySubtree);
  SET_VALID(I_vacmViewTreeFamilySubtree, vacmViewTreeFamilyEntryData.valid);

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilyStorageType = val;
  SET_VALID(I_vacmViewTreeFamilyStorageType, vacmViewTreeFamilyEntryData.valid);

  rc = snmp_vacmViewTreeFamilyTable_set(&vacmViewTreeFamilyEntryData);

  FreeOctetString(vacmViewTreeFamilyEntryData.vacmViewTreeFamilyViewName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
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
L7_RC_t l7_vacmViewTreeFamilyStatusGet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_snmpRowStatus_t *val)
{
  vacmViewTreeFamilyEntry_t * vacmViewTreeFamilyEntryData = snmp_vacmViewTreeFamilyTable_get(EXACT, vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);

  if (vacmViewTreeFamilyEntryData != NULL && 
      VALID(I_vacmViewTreeFamilyStatus, vacmViewTreeFamilyEntryData->valid))
  {
    *val = vacmViewTreeFamilyEntryData->vacmViewTreeFamilyStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
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
L7_RC_t l7_vacmViewTreeFamilyStatusSet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_snmpRowStatus_t val)
{
  vacmViewTreeFamilyEntry_t vacmViewTreeFamilyEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&vacmViewTreeFamilyEntryData, 0, sizeof(vacmViewTreeFamilyEntryData));

  if (l7_snmpStringSetValidate(vacmViewTreeFamilyViewName, 1, 32) != L7_SUCCESS)
    return rc;

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilyViewName = MakeOctetStringFromText(vacmViewTreeFamilyViewName);
  SET_VALID(I_vacmViewTreeFamilyViewName, vacmViewTreeFamilyEntryData.valid);

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilySubtree = MakeOIDFromDot(vacmViewTreeFamilySubtree);
  SET_VALID(I_vacmViewTreeFamilySubtree, vacmViewTreeFamilyEntryData.valid);

  vacmViewTreeFamilyEntryData.vacmViewTreeFamilyStatus = val;
  SET_VALID(I_vacmViewTreeFamilyStatus, vacmViewTreeFamilyEntryData.valid);

  rc = snmp_vacmViewTreeFamilyTable_set(&vacmViewTreeFamilyEntryData);

  FreeOctetString(vacmViewTreeFamilyEntryData.vacmViewTreeFamilyViewName);

  return rc;
}

void
debug_vacmViewTreeFamilyTable_walk()
{
  L7_char8 vacmViewTreeFamilyViewName[256];
  L7_char8 vacmViewTreeFamilySubtree[256];
  L7_char8 temp_string[256];
  L7_uint32 temp_len;
  L7_uint32 temp_val;

  memset((void*)vacmViewTreeFamilyViewName, 0, sizeof(vacmViewTreeFamilyViewName));
  memset((void*)vacmViewTreeFamilySubtree, 0, sizeof(vacmViewTreeFamilySubtree));

  sysapiPrintf("[vacmViewTreeFamilyTable start]\n");

  while(l7_vacmViewTreeFamilyTableNextGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree) == L7_SUCCESS)
  {
    sysapiPrintf("vacmViewTreeFamilyViewName.\"%s\".%s = \"%s\"\n", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, vacmViewTreeFamilyViewName);
    sysapiPrintf("vacmViewTreeFamilySubtree.\"%s\".%s = %s\n", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, vacmViewTreeFamilySubtree);

    if (l7_vacmViewTreeFamilyMaskGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, temp_string, &temp_len) == L7_SUCCESS)
    {
      sysapiPrintf("vacmViewTreeFamilyMask.\"%s\".%s = \"", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);
      debug_snmp_printHexString(temp_string, temp_len);
      sysapiPrintf("\"\n");
    }
    else
    {
      sysapiPrintf("vacmViewTreeFamilyMask.\"%s\".%s = L7_FAILURE\n", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);
    }

    if (l7_vacmViewTreeFamilyTypeGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, &temp_val) == L7_SUCCESS)
      sysapiPrintf("vacmViewTreeFamilyType.\"%s\".%s = %d\n", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, temp_val);
    else
      sysapiPrintf("vacmViewTreeFamilyType.\"%s\".%s = L7_FAILURE\n", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);

    if (l7_vacmViewTreeFamilyStorageTypeGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, &temp_val) == L7_SUCCESS)
      sysapiPrintf("vacmViewTreeFamilyStorageType.\"%s\".%s = %d\n", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, temp_val);
    else
      sysapiPrintf("vacmViewTreeFamilyStorageType.\"%s\".%s = L7_FAILURE\n", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);

    if (l7_vacmViewTreeFamilyStatusGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, &temp_val) == L7_SUCCESS)
      sysapiPrintf("vacmViewTreeFamilyStatus.\"%s\".%s = %d\n", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, temp_val);
    else
      sysapiPrintf("vacmViewTreeFamilyStatus.\"%s\".%s = L7_FAILURE\n", vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_vacmViewTreeFamilyTable_create(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_vacmViewTreeFamilyType_t vacmViewTreeFamilyType)
{
  L7_RC_t rc;

  rc = l7_vacmViewTreeFamilyStatusSet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, snmpRowStatus_createAndWait);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmViewTreeFamilyStatusSet 1 failed!\n");
    return;
  }

  rc = l7_vacmViewTreeFamilyTypeSet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, vacmViewTreeFamilyType);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmViewTreeFamilyTypeSet 1 failed!\n");
    return;
  }

  rc = l7_vacmViewTreeFamilyStatusSet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_vacmViewTreeFamilyStatusSet 2 failed!\n");
    return;
  }
  sysapiPrintf("debug_vacmViewTreeFamilyTable_create finished!\n");
}


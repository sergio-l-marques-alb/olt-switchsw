/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_entity.c
*
* @purpose    System-Specific code for Entity MIB - RFC 2737
*
* @component  SNMP
*
* @comments
*
* @create     6/04/2003
*
* @author     cpverne
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "k_mib_entity_api.h"
#include "usmdb_edb_api.h"


entPhysicalEntry_t *
k_entPhysicalEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 entPhysicalIndex)
{
  static entPhysicalEntry_t entPhysicalEntryData;
  
  ZERO_VALID(entPhysicalEntryData.valid);
  entPhysicalEntryData.entPhysicalIndex = entPhysicalIndex;
  SET_VALID(I_entPhysicalIndex, entPhysicalEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpEntPhysicalEntryGet(&entPhysicalEntryData, nominator) != L7_SUCCESS) :
      ((snmpEntPhysicalEntryGet(&entPhysicalEntryData, nominator) != L7_SUCCESS) &&
       (snmpEntPhysicalEntryNextGet(&entPhysicalEntryData, nominator) != L7_SUCCESS)))
    return(NULL);

  if (nominator >= 0 && !VALID(nominator, entPhysicalEntryData.valid))
    return(NULL);

  return(&entPhysicalEntryData);
}

#ifdef SETS
int
k_entPhysicalEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_entPhysicalEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_entPhysicalEntry_set_defaults(doList_t *dp)
{
    entPhysicalEntry_t *data = (entPhysicalEntry_t *) (dp->data);

    if ((data->entPhysicalDescr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalVendorType = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalHardwareRev = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalFirmwareRev = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalSoftwareRev = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalSerialNum = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalMfgName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalModelName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalAlias = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->entPhysicalAssetID = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_entPhysicalEntry_set(entPhysicalEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_entPhysicalSerialNum, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->entPhysicalSerialNum->octet_ptr, data->entPhysicalSerialNum->length);
    if (usmDbEdbPhysicalSerialNumSet(data->entPhysicalIndex, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_entPhysicalSerialNum, tempValid);
    }
  }

  if (VALID(I_entPhysicalAlias, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->entPhysicalAlias->octet_ptr, data->entPhysicalAlias->length);
    if (usmDbEdbPhysicalAliasSet(data->entPhysicalIndex, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_entPhysicalAlias, tempValid);
    }
  }

  if (VALID(I_entPhysicalAssetID, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->entPhysicalAssetID->octet_ptr, data->entPhysicalAssetID->length);
    if (usmDbEdbPhysicalAssetIDSet(data->entPhysicalIndex, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_entPhysicalEntry_UNDO
/* add #define SR_entPhysicalEntry_UNDO in sitedefs.h to
 * include the undo routine for the entPhysicalEntry family.
 */
int
entPhysicalEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
  entPhysicalEntry_t *data = (entPhysicalEntry_t *) doCur->data;
  entPhysicalEntry_t *undodata = (entPhysicalEntry_t *) doCur->undodata;
  entPhysicalEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_entPhysicalEntry_set(setdata, contextInfo, function) == NO_ERROR)
    return NO_ERROR;

  return UNDO_FAILED_ERROR;

}
#endif /* SR_entPhysicalEntry_UNDO */

#endif /* SETS */

entLogicalEntry_t *
k_entLogicalEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_INT32 entLogicalIndex)
{
#ifdef NOT_YET
   static entLogicalEntry_t entLogicalEntryData;

   /*
    * put your code to retrieve the information here
    */

   entLogicalEntryData.entLogicalIndex = ;
   entLogicalEntryData.entLogicalDescr = ;
   entLogicalEntryData.entLogicalType = ;
   entLogicalEntryData.entLogicalCommunity = ;
   entLogicalEntryData.entLogicalTAddress = ;
   entLogicalEntryData.entLogicalTDomain = ;
   entLogicalEntryData.entLogicalContextEngineID = ;
   entLogicalEntryData.entLogicalContextName = ;
   SET_ALL_VALID(entLogicalEntryData.valid);
   return(&entLogicalEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

entLPMappingEntry_t *
k_entLPMappingEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_INT32 entLogicalIndex,
                        SR_INT32 entLPPhysicalIndex)
{
#ifdef NOT_YET
   static entLPMappingEntry_t entLPMappingEntryData;

   /*
    * put your code to retrieve the information here
    */

   entLPMappingEntryData.entLPPhysicalIndex = ;
   entLPMappingEntryData.entLogicalIndex = ;
   SET_ALL_VALID(entLPMappingEntryData.valid);
   return(&entLPMappingEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

entAliasMappingEntry_t *
k_entAliasMappingEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 entPhysicalIndex,
                           SR_INT32 entAliasLogicalIndexOrZero)
{
#ifdef NOT_YET
   static entAliasMappingEntry_t entAliasMappingEntryData;

   /*
    * put your code to retrieve the information here
    */

   entAliasMappingEntryData.entAliasLogicalIndexOrZero = ;
   entAliasMappingEntryData.entAliasMappingIdentifier = ;
   entAliasMappingEntryData.entPhysicalIndex = ;
   SET_ALL_VALID(entAliasMappingEntryData.valid);
   return(&entAliasMappingEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

entPhysicalContainsEntry_t *
k_entPhysicalContainsEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 entPhysicalIndex,
                               SR_INT32 entPhysicalChildIndex)
{
  static entPhysicalContainsEntry_t entPhysicalContainsEntryData;
  
  ZERO_VALID(entPhysicalContainsEntryData.valid);
  
  entPhysicalContainsEntryData.entPhysicalIndex = entPhysicalIndex;
  SET_VALID(I_entPhysicalIndex, entPhysicalContainsEntryData.valid);
  
  entPhysicalContainsEntryData.entPhysicalChildIndex = entPhysicalChildIndex;
  SET_VALID(I_entPhysicalChildIndex, entPhysicalContainsEntryData.valid);
  
  if ((searchType == EXACT) ?
      (usmDbEdbPhysicalContainsEntryGet(entPhysicalContainsEntryData.entPhysicalIndex,
                                      entPhysicalContainsEntryData.entPhysicalChildIndex) != L7_SUCCESS) :
      ((usmDbEdbPhysicalContainsEntryGet(entPhysicalContainsEntryData.entPhysicalIndex,
                                       entPhysicalContainsEntryData.entPhysicalChildIndex) != L7_SUCCESS) &&
       (usmDbEdbPhysicalContainsEntryNextGet(&entPhysicalContainsEntryData.entPhysicalIndex,
                                           &entPhysicalContainsEntryData.entPhysicalChildIndex) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case I_entPhysicalContainsEntryIndex_entPhysicalIndex:
  case I_entPhysicalChildIndex:
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  return(&entPhysicalContainsEntryData);
}

entityGeneral_t *
k_entityGeneral_get(int serialNum, ContextInfo *contextInfo,
                    int nominator)
{
  static entityGeneral_t entityGeneralData;

  ZERO_VALID(entityGeneralData.valid);
  
  switch (nominator)
  {
  case I_entLastChangeTime:
    if (snmpEntLastChangeTimeGet(&entityGeneralData.entLastChangeTime) == L7_SUCCESS)
      SET_VALID(nominator, entityGeneralData.valid);
  break;
  
  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, entityGeneralData.valid))
    return(NULL);

  return(&entityGeneralData);
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_dot3ad.c
*
* @purpose System specific code for 802.3ad MIBs
*
* @component SNMP
*
* @create 06/21/2001
*
* @author Mike Fiorito
*
* @end
*             
*********************************************************************/ 

/*********************************************************************
                         
*********************************************************************/

#include "k_private_base.h"
#include "k_mib_dot3ad_api.h"
#include "usmdb_common.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_util_api.h"

lagMIBObjects_t *
k_lagMIBObjects_get(int serialNum, ContextInfo *contextInfo,
                    int nominator)
{
   static lagMIBObjects_t lagMIBObjectsData;

   ZERO_VALID(lagMIBObjectsData.valid);

   switch (nominator)
   {
   case -1:
     break;

   case I_dot3adTablesLastChanged:
     if (snmpDot3adTablesLastChangedGet(USMDB_UNIT_CURRENT, &lagMIBObjectsData.dot3adTablesLastChanged) == L7_SUCCESS)
     {
       SET_VALID(I_dot3adTablesLastChanged, lagMIBObjectsData.valid);
     }
     break;
   }

   if ( nominator >= 0 && !VALID(nominator, lagMIBObjectsData.valid) )
     return(NULL);

   return(&lagMIBObjectsData);
}

dot3adAggEntry_t *
k_dot3adAggEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_INT32 dot3adAggIndex)
{
   static dot3adAggEntry_t dot3adAggEntryData;
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
   L7_uint32 intIfIndex;
   static L7_BOOL firstTime = L7_TRUE;

   if (firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;

     dot3adAggEntryData.dot3adAggMACAddress = MakeOctetString(NULL, 0);
     dot3adAggEntryData.dot3adAggActorSystemID = MakeOctetString(NULL, 0);
     dot3adAggEntryData.dot3adAggPartnerSystemID = MakeOctetString(NULL, 0);
   }

   ZERO_VALID(dot3adAggEntryData.valid);
   dot3adAggEntryData.dot3adAggIndex = dot3adAggIndex;
   SET_VALID(I_dot3adAggIndex, dot3adAggEntryData.valid);

   if ( ( searchType == EXACT ?
        ( snmpDot3adAggEntryGet(USMDB_UNIT_CURRENT, dot3adAggEntryData.dot3adAggIndex) != L7_SUCCESS ) :
        ( snmpDot3adAggEntryGet(USMDB_UNIT_CURRENT, dot3adAggEntryData.dot3adAggIndex) != L7_SUCCESS &&
          snmpDot3adAggEntryGetNext(USMDB_UNIT_CURRENT, dot3adAggEntryData.dot3adAggIndex, &dot3adAggEntryData.dot3adAggIndex) != L7_SUCCESS ) ) ||
        usmDbIntIfNumFromExtIfNum(dot3adAggEntryData.dot3adAggIndex, &intIfIndex) != L7_SUCCESS )
   {
     ZERO_VALID(dot3adAggEntryData.valid);
     return(NULL);
   }

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality.
 */ 
   switch (nominator)
   {
   case -1:
   case I_dot3adAggIndex:
     if( nominator != -1) break;
     /* else pass through */
   case I_dot3adAggMACAddress:
     CLR_VALID(I_dot3adAggMACAddress, dot3adAggEntryData.valid);
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     if ( usmDbDot3adAggPhysAddressGet(USMDB_UNIT_CURRENT, intIfIndex,
                                       snmp_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggEntryData.dot3adAggMACAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
         SET_VALID(I_dot3adAggMACAddress, dot3adAggEntryData.valid);
     }
     if( nominator != -1) break;
     /* else pass through */

   case I_dot3adAggActorSystemPriority:
     SET_VALID(I_dot3adAggActorSystemPriority, dot3adAggEntryData.valid);
     if ( usmDbDot3adAggActorSystemPriorityGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggEntryData.dot3adAggActorSystemPriority) != L7_SUCCESS )
       CLR_VALID(I_dot3adAggActorSystemPriority, dot3adAggEntryData.valid);
     if( nominator != -1) break;
     /* else pass through */

   case I_dot3adAggActorSystemID:
     CLR_VALID(I_dot3adAggActorSystemID, dot3adAggEntryData.valid);
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     if ( usmDbDot3adAggActorSystemIDGet(USMDB_UNIT_CURRENT, intIfIndex,
                                         snmp_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggEntryData.dot3adAggActorSystemID, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
         SET_VALID(I_dot3adAggActorSystemID, dot3adAggEntryData.valid);
     }
     if( nominator != -1) break;
     /* else pass through */

   case I_dot3adAggAggregateOrIndividual:
     SET_VALID(I_dot3adAggAggregateOrIndividual, dot3adAggEntryData.valid);
     if ( snmpDot3adAggAggregateOrIndividualGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggEntryData.dot3adAggAggregateOrIndividual) != L7_SUCCESS )
       CLR_VALID(I_dot3adAggAggregateOrIndividual, dot3adAggEntryData.valid);
     if( nominator != -1) break;
     /* else pass through */

   case I_dot3adAggActorAdminKey:
     SET_VALID(I_dot3adAggActorAdminKey, dot3adAggEntryData.valid);
     if ( usmDbDot3adAggActorAdminKeyGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggEntryData.dot3adAggActorAdminKey) != L7_SUCCESS )
       CLR_VALID(I_dot3adAggActorAdminKey, dot3adAggEntryData.valid);
     if( nominator != -1) break;
     /* else pass through */

   case I_dot3adAggActorOperKey:
     SET_VALID(I_dot3adAggActorOperKey, dot3adAggEntryData.valid);
     if ( usmDbDot3adAggActorOperKeyGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggEntryData.dot3adAggActorOperKey) != L7_SUCCESS )
       CLR_VALID(I_dot3adAggActorOperKey, dot3adAggEntryData.valid);
     if( nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPartnerSystemID:
     CLR_VALID(I_dot3adAggPartnerSystemID, dot3adAggEntryData.valid);
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     if ( usmDbDot3adAggPartnerSystemIDGet(USMDB_UNIT_CURRENT, intIfIndex,
                                           snmp_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggEntryData.dot3adAggPartnerSystemID, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
         SET_VALID(I_dot3adAggPartnerSystemID, dot3adAggEntryData.valid);
     }
     if( nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPartnerSystemPriority:
     SET_VALID(I_dot3adAggPartnerSystemPriority, dot3adAggEntryData.valid);
     if ( usmDbDot3adAggPartnerSystemPriorityGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggEntryData.dot3adAggPartnerSystemPriority) != L7_SUCCESS )
       CLR_VALID(I_dot3adAggPartnerSystemPriority, dot3adAggEntryData.valid);
     if( nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPartnerOperKey:
     SET_VALID(I_dot3adAggPartnerOperKey, dot3adAggEntryData.valid);
     if ( usmDbDot3adAggPartnerOperKeyGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggEntryData.dot3adAggPartnerOperKey) != L7_SUCCESS )
       CLR_VALID(I_dot3adAggPartnerOperKey, dot3adAggEntryData.valid);
     if( nominator != -1) break;
     /* else pass through */

   case I_dot3adAggCollectorMaxDelay:
     SET_VALID(I_dot3adAggCollectorMaxDelay, dot3adAggEntryData.valid);
     if ( usmDbDot3adAggCollectorMaxDelayGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggEntryData.dot3adAggCollectorMaxDelay) != L7_SUCCESS )
       CLR_VALID(I_dot3adAggCollectorMaxDelay, dot3adAggEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if ( nominator >= 0 && !VALID(nominator, dot3adAggEntryData.valid) )
     return(NULL);

   return(&dot3adAggEntryData);
}

#ifdef SETS
int
k_dot3adAggEntry_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dot3adAggEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dot3adAggEntry_set_defaults(doList_t *dp)
{
    dot3adAggEntry_t *data = (dot3adAggEntry_t *) (dp->data);

    if ((data->dot3adAggMACAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->dot3adAggActorSystemID = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->dot3adAggPartnerSystemID = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->dot3adAggActorSystemPriority = (SR_INT32)0;
    data->dot3adAggAggregateOrIndividual = (SR_INT32)0;
    data->dot3adAggActorAdminKey = (SR_INT32)0;
    data->dot3adAggActorOperKey = (SR_INT32)0;
    data->dot3adAggPartnerSystemPriority = (SR_INT32)0;
    data->dot3adAggPartnerOperKey = (SR_INT32)0;
    data->dot3adAggCollectorMaxDelay = (SR_INT32)0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_dot3adAggEntry_set(dot3adAggEntry_t *data,
                     ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfIndex;

  if ( snmpDot3adAggEntryGet(USMDB_UNIT_CURRENT, data->dot3adAggIndex) != L7_SUCCESS ||
       usmDbIntIfNumFromExtIfNum(data->dot3adAggIndex, &intIfIndex) != L7_SUCCESS )
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggActorSystemPriority, data->valid) &&
       usmDbDot3adAggActorSystemPrioritySet(USMDB_UNIT_CURRENT, intIfIndex,
                                            data->dot3adAggActorSystemPriority) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggActorSystemPriority, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggActorAdminKey, data->valid) &&
       usmDbDot3adAggActorAdminKeySet(USMDB_UNIT_CURRENT, intIfIndex,
                                      data->dot3adAggActorAdminKey) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggActorAdminKey, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggCollectorMaxDelay, data->valid) &&
       usmDbDot3adAggCollectorMaxDelaySet(USMDB_UNIT_CURRENT, intIfIndex,
                                          data->dot3adAggCollectorMaxDelay) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggCollectorMaxDelay, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}

  #ifdef SR_dot3adAggEntry_UNDO
/* add #define SR_dot3adAggEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot3adAggEntry family.
 */
int
dot3adAggEntry_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{

  dot3adAggEntry_t *data = (dot3adAggEntry_t *) doCur->data;
  dot3adAggEntry_t *undodata = (dot3adAggEntry_t *) doCur->undodata;
  dot3adAggEntry_t *setdata = NULL;
  int function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_dot3adAggEntry_set(setdata, contextInfo, function) == NO_ERROR) 
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_dot3adAggEntry_UNDO */

#endif /* SETS */

dot3adAggPortListEntry_t *
k_dot3adAggPortListEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 dot3adAggIndex)
{
   static dot3adAggPortListEntry_t dot3adAggPortListEntryData;
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
   L7_uint32 snmp_buffer_len;
   L7_uint32 intIfIndex;
   static L7_BOOL firstTime = L7_TRUE;

   if (firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;

     dot3adAggPortListEntryData.dot3adAggPortListPorts = MakeOctetString(NULL, 0);
   }

   ZERO_VALID(dot3adAggPortListEntryData.valid);
   dot3adAggPortListEntryData.dot3adAggIndex = dot3adAggIndex;
   SET_VALID(I_dot3adAggPortListEntryIndex_dot3adAggIndex, dot3adAggPortListEntryData.valid);
   
   if ( ( searchType == EXACT ?
        ( snmpDot3adAggEntryGet(USMDB_UNIT_CURRENT, dot3adAggPortListEntryData.dot3adAggIndex) != L7_SUCCESS ) :
        ( snmpDot3adAggEntryGet(USMDB_UNIT_CURRENT, dot3adAggPortListEntryData.dot3adAggIndex) != L7_SUCCESS &&
          snmpDot3adAggEntryGetNext(USMDB_UNIT_CURRENT, dot3adAggPortListEntryData.dot3adAggIndex, &dot3adAggPortListEntryData.dot3adAggIndex) != L7_SUCCESS ) ) ||
        usmDbIntIfNumFromExtIfNum(dot3adAggPortListEntryData.dot3adAggIndex, &intIfIndex) != L7_SUCCESS )
   {
     ZERO_VALID(dot3adAggPortListEntryData.valid);
     return(NULL);
   }

   switch (nominator)
   {
   case -1:
   case I_dot3adAggPortListEntryIndex_dot3adAggIndex:
      break;

   case I_dot3adAggPortListPorts:
     CLR_VALID(I_dot3adAggPortListPorts, dot3adAggPortListEntryData.valid);
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     snmp_buffer_len = SNMP_BUFFER_LEN;
     if ( usmDbDot3adAggPortListGet(USMDB_UNIT_CURRENT, intIfIndex, snmp_buffer, &snmp_buffer_len) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggPortListEntryData.dot3adAggPortListPorts, snmp_buffer, snmp_buffer_len) == L7_TRUE)
         SET_VALID(I_dot3adAggPortListPorts, dot3adAggPortListEntryData.valid);
     }
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if ( nominator >= 0 && !VALID(nominator, dot3adAggPortListEntryData.valid) )
     return(NULL);

   return(&dot3adAggPortListEntryData);
}

dot3adAggPortEntry_t *
k_dot3adAggPortEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 dot3adAggPortIndex)
{
   static dot3adAggPortEntry_t dot3adAggPortEntryData;
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
   L7_uchar8 snmp_state_buffer;
   L7_uint32 intIfIndex;
   static L7_BOOL firstTime = L7_TRUE;

   if (firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;

     dot3adAggPortEntryData.dot3adAggPortActorSystemID = MakeOctetString(NULL, 0);
     dot3adAggPortEntryData.dot3adAggPortPartnerAdminSystemID = MakeOctetString(NULL, 0);
     dot3adAggPortEntryData.dot3adAggPortPartnerOperSystemID = MakeOctetString(NULL, 0);
     dot3adAggPortEntryData.dot3adAggPortActorAdminState = MakeOctetString(NULL, 0);
     dot3adAggPortEntryData.dot3adAggPortActorOperState = MakeOctetString(NULL, 0);
     dot3adAggPortEntryData.dot3adAggPortPartnerAdminState = MakeOctetString(NULL, 0);
     dot3adAggPortEntryData.dot3adAggPortPartnerOperState = MakeOctetString(NULL, 0);
   }

   ZERO_VALID(dot3adAggPortEntryData.valid);
   dot3adAggPortEntryData.dot3adAggPortIndex = dot3adAggPortIndex;
   SET_VALID(I_dot3adAggPortIndex, dot3adAggPortEntryData.valid);

   if ( ( searchType == EXACT ?
        ( snmpDot3adAggPortEntryGet(USMDB_UNIT_CURRENT, dot3adAggPortEntryData.dot3adAggPortIndex) != L7_SUCCESS ) :
        ( snmpDot3adAggPortEntryGet(USMDB_UNIT_CURRENT, dot3adAggPortEntryData.dot3adAggPortIndex) != L7_SUCCESS &&
          snmpDot3adAggPortEntryGetNext(USMDB_UNIT_CURRENT, dot3adAggPortEntryData.dot3adAggPortIndex,
                                         &dot3adAggPortEntryData.dot3adAggPortIndex) != L7_SUCCESS ) ) ||
        usmDbIntIfNumFromExtIfNum(dot3adAggPortEntryData.dot3adAggPortIndex, &intIfIndex) != L7_SUCCESS )
   {
     ZERO_VALID(dot3adAggPortEntryData.valid);
     return(NULL);
   }

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality.
 */ 
   switch (nominator)
   {
   case -1:
   case I_dot3adAggPortIndex:
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortActorSystemPriority:
     if ( usmDbDot3adAggPortActorSystemPriorityGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortActorSystemPriority) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortActorSystemPriority, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortActorSystemID:
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     if ( usmDbDot3adAggPortActorSystemIDGet(USMDB_UNIT_CURRENT, intIfIndex,
                                             snmp_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggPortEntryData.dot3adAggPortActorSystemID, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
         SET_VALID(I_dot3adAggPortActorSystemID, dot3adAggPortEntryData.valid);
     }
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortActorAdminKey:
     if ( usmDbDot3adAggPortActorAdminKeyGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortActorAdminKey) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortActorAdminKey, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortActorOperKey:
     if ( usmDbDot3adAggPortActorOperKeyGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortActorOperKey) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortActorOperKey, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerAdminSystemPriority:
     if ( usmDbDot3adAggPortPartnerAdminSystemPriorityGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortPartnerAdminSystemPriority) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortPartnerAdminSystemPriority, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerOperSystemPriority:
     if ( usmDbDot3adAggPortPartnerOperSystemPriorityGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortPartnerOperSystemPriority) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortPartnerOperSystemPriority, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerAdminSystemID:
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     if ( usmDbDot3adAggPortPartnerAdminSystemIDGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                    snmp_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggPortEntryData.dot3adAggPortPartnerAdminSystemID, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
         SET_VALID(I_dot3adAggPortPartnerAdminSystemID, dot3adAggPortEntryData.valid);
     }
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerOperSystemID:
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     if ( usmDbDot3adAggPortPartnerOperSystemIDGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                   snmp_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggPortEntryData.dot3adAggPortPartnerOperSystemID, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
         SET_VALID(I_dot3adAggPortPartnerOperSystemID, dot3adAggPortEntryData.valid);
     }
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerAdminKey:
     if ( usmDbDot3adAggPortPartnerAdminKeyGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortPartnerAdminKey) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortPartnerAdminKey, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerOperKey:
     if ( usmDbDot3adAggPortPartnerOperKeyGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortPartnerOperKey) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortPartnerOperKey, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortSelectedAggID:
     if ( usmDbDot3adAggPortSelectedAggIDGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortSelectedAggID) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortSelectedAggID, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;

   case I_dot3adAggPortAttachedAggID:
     if ( usmDbDot3adAggPortAttachedAggIDGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortAttachedAggID) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortAttachedAggID, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortActorPort:
     if ( usmDbDot3adAggPortActorPortGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortActorPort) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortActorPort, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortActorPortPriority:
     if ( usmDbDot3adAggPortActorPortPriorityGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortActorPortPriority) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortActorPortPriority, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerAdminPort:
     if ( usmDbDot3adAggPortPartnerAdminPortGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortPartnerAdminPort) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortPartnerAdminPort, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerOperPort:
     if ( usmDbDot3adAggPortPartnerOperPortGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortPartnerOperPort) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortPartnerOperPort, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerAdminPortPriority:
     if ( usmDbDot3adAggPortPartnerAdminPortPriorityGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortPartnerAdminPortPriority) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortPartnerAdminPortPriority, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerOperPortPriority:
     if ( usmDbDot3adAggPortPartnerOperPortPriorityGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortPartnerOperPortPriority) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortPartnerOperPortPriority, dot3adAggPortEntryData.valid);
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortActorAdminState:
     if ( usmDbDot3adAggPortActorAdminStateGet(USMDB_UNIT_CURRENT, intIfIndex,
                                               &snmp_state_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggPortEntryData.dot3adAggPortActorAdminState, &snmp_state_buffer, 1) == L7_TRUE)
         SET_VALID(I_dot3adAggPortActorAdminState, dot3adAggPortEntryData.valid);
     }
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortActorOperState:
     if ( usmDbDot3adAggPortActorOperStateGet(USMDB_UNIT_CURRENT, intIfIndex,
                                              &snmp_state_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggPortEntryData.dot3adAggPortActorOperState, &snmp_state_buffer, 1) == L7_TRUE)
         SET_VALID(I_dot3adAggPortActorOperState, dot3adAggPortEntryData.valid);
     }
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerAdminState:
     if ( usmDbDot3adAggPortPartnerAdminStateGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                 &snmp_state_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggPortEntryData.dot3adAggPortPartnerAdminState, &snmp_state_buffer, 1) == L7_TRUE)
         SET_VALID(I_dot3adAggPortPartnerAdminState, dot3adAggPortEntryData.valid);
     }
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortPartnerOperState:
     if ( usmDbDot3adAggPortPartnerOperStateGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                &snmp_state_buffer) == L7_SUCCESS )
     {
       if (SafeMakeOctetString(&dot3adAggPortEntryData.dot3adAggPortPartnerOperState, &snmp_state_buffer, 1) == L7_TRUE)
         SET_VALID(I_dot3adAggPortPartnerOperState, dot3adAggPortEntryData.valid);
     }
     if (nominator != -1) break;
     /* else pass through */

   case I_dot3adAggPortAggregateOrIndividual:
     if ( snmpDot3adAggPortAggregateOrIndividualGet(USMDB_UNIT_CURRENT, intIfIndex,
                     &dot3adAggPortEntryData.dot3adAggPortAggregateOrIndividual) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortAggregateOrIndividual, dot3adAggPortEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if ( nominator >= 0 && !VALID(nominator, dot3adAggPortEntryData.valid) )
     return(NULL);

   return(&dot3adAggPortEntryData);
}

#ifdef SETS
int
k_dot3adAggPortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dot3adAggPortEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dot3adAggPortEntry_set_defaults(doList_t *dp)
{
    dot3adAggPortEntry_t *data = (dot3adAggPortEntry_t *) (dp->data);

    if ((data->dot3adAggPortActorSystemID = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->dot3adAggPortPartnerAdminSystemID = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->dot3adAggPortPartnerOperSystemID = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->dot3adAggPortActorAdminState = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->dot3adAggPortActorOperState = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->dot3adAggPortPartnerAdminState = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->dot3adAggPortPartnerOperState = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->dot3adAggPortActorSystemPriority = (SR_INT32)0;
    data->dot3adAggPortActorAdminKey = (SR_INT32)0;
    data->dot3adAggPortActorOperKey = (SR_INT32)0;
    data->dot3adAggPortPartnerAdminSystemPriority = (SR_INT32)0;
    data->dot3adAggPortPartnerOperSystemPriority = (SR_INT32)0;
    data->dot3adAggPortPartnerAdminKey = (SR_INT32)0;
    data->dot3adAggPortPartnerOperKey = (SR_INT32)0;
    data->dot3adAggPortSelectedAggID = (SR_INT32)0;
    data->dot3adAggPortAttachedAggID = (SR_INT32)0;
    data->dot3adAggPortActorPort = (SR_INT32)0;
    data->dot3adAggPortActorPortPriority = (SR_INT32)0;
    data->dot3adAggPortPartnerAdminPort = (SR_INT32)0;
    data->dot3adAggPortPartnerOperPort = (SR_INT32)0;
    data->dot3adAggPortPartnerAdminPortPriority = (SR_INT32)0;
    data->dot3adAggPortPartnerOperPortPriority = (SR_INT32)0;
    data->dot3adAggPortAggregateOrIndividual = (SR_INT32)0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_dot3adAggPortEntry_set(dot3adAggPortEntry_t *data,
                         ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfIndex;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];

  if ( snmpDot3adAggPortEntryGet(USMDB_UNIT_CURRENT, data->dot3adAggPortIndex) != L7_SUCCESS ||
       usmDbIntIfNumFromExtIfNum(data->dot3adAggPortIndex, &intIfIndex) != L7_SUCCESS )
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggPortActorSystemPriority, data->valid) &&
       usmDbDot3adAggPortActorSystemPrioritySet(USMDB_UNIT_CURRENT, intIfIndex,
                     data->dot3adAggPortActorSystemPriority) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggPortActorSystemPriority, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggPortActorAdminKey, data->valid) &&
       usmDbDot3adAggPortActorAdminKeySet(USMDB_UNIT_CURRENT, intIfIndex,
                     data->dot3adAggPortActorAdminKey) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggPortActorAdminKey, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  /* this object is RO */
  if ( VALID(I_dot3adAggPortActorOperKey, data->valid))
  {
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggPortPartnerAdminSystemPriority, data->valid) &&
       usmDbDot3adAggPortPartnerAdminSystemPrioritySet(USMDB_UNIT_CURRENT, intIfIndex,
                     data->dot3adAggPortPartnerAdminSystemPriority) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggPortPartnerAdminSystemPriority, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggPortPartnerAdminSystemID, data->valid) )
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->dot3adAggPortPartnerAdminSystemID->octet_ptr,
                         data->dot3adAggPortPartnerAdminSystemID->length);
    if ( usmDbDot3adAggPortPartnerAdminSystemIDSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                   snmp_buffer) != L7_SUCCESS )
    {
      CLR_VALID(I_dot3adAggPortPartnerAdminSystemID, data->valid);
      return(COMMIT_FAILED_ERROR);
    }
  }

  if ( VALID(I_dot3adAggPortPartnerAdminKey, data->valid) &&
       usmDbDot3adAggPortPartnerAdminKeySet(USMDB_UNIT_CURRENT, intIfIndex,
                     data->dot3adAggPortPartnerAdminKey) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggPortPartnerAdminKey, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggPortActorPortPriority, data->valid) &&
       usmDbDot3adAggPortActorPortPrioritySet(USMDB_UNIT_CURRENT, intIfIndex,
                     data->dot3adAggPortActorPortPriority) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggPortActorPortPriority, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggPortPartnerAdminPort, data->valid) &&
       usmDbDot3adAggPortPartnerAdminPortSet(USMDB_UNIT_CURRENT, intIfIndex,
                     data->dot3adAggPortPartnerAdminPort) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggPortPartnerAdminPort, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggPortPartnerAdminPortPriority, data->valid) &&
       usmDbDot3adAggPortPartnerAdminPortPrioritySet(USMDB_UNIT_CURRENT, intIfIndex,
                     data->dot3adAggPortPartnerAdminPortPriority) != L7_SUCCESS )
  {
    CLR_VALID(I_dot3adAggPortPartnerAdminPortPriority, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_dot3adAggPortActorAdminState, data->valid) )
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->dot3adAggPortActorAdminState->octet_ptr,
                         data->dot3adAggPortActorAdminState->length);
    if ( usmDbDot3adAggPortActorAdminStateSet(USMDB_UNIT_CURRENT, intIfIndex,
                                              snmp_buffer) != L7_SUCCESS )
    {
      CLR_VALID(I_dot3adAggPortActorAdminState, data->valid);
      return(COMMIT_FAILED_ERROR);
    }
  }

  if ( VALID(I_dot3adAggPortPartnerAdminState, data->valid) )
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->dot3adAggPortPartnerAdminState->octet_ptr,
                         data->dot3adAggPortPartnerAdminState->length);
    if ( usmDbDot3adAggPortPartnerAdminStateSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                snmp_buffer) != L7_SUCCESS )
    {
      CLR_VALID(I_dot3adAggPortPartnerAdminState, data->valid);
      return(COMMIT_FAILED_ERROR);
    }
  }

  return(NO_ERROR);
}

  #ifdef SR_dot3adAggPortEntry_UNDO
/* add #define SR_dot3adAggPortEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot3adAggPortEntry family.
 */
int
dot3adAggPortEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{

  dot3adAggPortEntry_t *data = (dot3adAggPortEntry_t *) doCur->data;
  dot3adAggPortEntry_t *undodata = (dot3adAggPortEntry_t *) doCur->undodata;
  dot3adAggPortEntry_t *setdata = NULL;
  int function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_dot3adAggPortEntry_set(setdata, contextInfo, function) == NO_ERROR) 
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_dot3adAggPortEntry_UNDO */

#endif /* SETS */

dot3adAggPortStatsEntry_t *
k_dot3adAggPortStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_INT32 dot3adAggPortIndex)
{
   static dot3adAggPortStatsEntry_t dot3adAggPortStatsEntryData;
   L7_uint32 intIfIndex;

   ZERO_VALID(dot3adAggPortStatsEntryData.valid);
   dot3adAggPortStatsEntryData.dot3adAggPortIndex = dot3adAggPortIndex;
   SET_VALID(I_dot3adAggPortStatsEntryIndex_dot3adAggPortIndex, dot3adAggPortStatsEntryData.valid);

   if ( ( searchType == EXACT ?
        ( snmpDot3adAggPortEntryGet(USMDB_UNIT_CURRENT, dot3adAggPortStatsEntryData.dot3adAggPortIndex) != L7_SUCCESS ) :
        ( snmpDot3adAggPortEntryGet(USMDB_UNIT_CURRENT, dot3adAggPortStatsEntryData.dot3adAggPortIndex) != L7_SUCCESS &&
          snmpDot3adAggPortEntryGetNext(USMDB_UNIT_CURRENT, dot3adAggPortStatsEntryData.dot3adAggPortIndex,
                                         &dot3adAggPortStatsEntryData.dot3adAggPortIndex) != L7_SUCCESS ) ) ||
        usmDbIntIfNumFromExtIfNum(dot3adAggPortStatsEntryData.dot3adAggPortIndex, &intIfIndex) != L7_SUCCESS )
   {
     ZERO_VALID(dot3adAggPortStatsEntryData.valid);
     return(NULL);
   }


   switch (nominator)
   {
   case -1:
   case I_dot3adAggPortStatsEntryIndex_dot3adAggPortIndex:
     break;

   case I_dot3adAggPortStatsLACPDUsRx:
     if ( usmDbDot3adAggPortStatsLACPDUsRxGet(USMDB_UNIT_CURRENT, intIfIndex,
                       &dot3adAggPortStatsEntryData.dot3adAggPortStatsLACPDUsRx) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortStatsLACPDUsRx, dot3adAggPortStatsEntryData.valid);
     break;

   case I_dot3adAggPortStatsMarkerPDUsRx:
     if ( usmDbDot3adAggPortStatsMarkerPDUsRxGet(USMDB_UNIT_CURRENT, intIfIndex,
                       &dot3adAggPortStatsEntryData.dot3adAggPortStatsMarkerPDUsRx) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortStatsMarkerPDUsRx, dot3adAggPortStatsEntryData.valid);
     break;

#ifdef I_dot3adAggPortStatsMarkerResponsePDUsRx
   case I_dot3adAggPortStatsMarkerResponsePDUsRx:
     if ( usmDbDot3adAggPortStatsMarkerResponsePDUsRxGet(USMDB_UNIT_CURRENT, intIfIndex,
                       &dot3adAggPortStatsEntryData.dot3adAggPortStatsMarkerResponsePDUsRx) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortStatsMarkerResponsePDUsRx, dot3adAggPortStatsEntryData.valid);
     break;
#endif

   case I_dot3adAggPortStatsUnknownRx:
     if ( usmDbDot3adAggPortStatsUnknownRxGet(USMDB_UNIT_CURRENT, intIfIndex,
                       &dot3adAggPortStatsEntryData.dot3adAggPortStatsUnknownRx) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortStatsUnknownRx, dot3adAggPortStatsEntryData.valid);
     break;

   case I_dot3adAggPortStatsIllegalRx:
     if ( usmDbDot3adAggPortStatsIllegalRxGet(USMDB_UNIT_CURRENT, intIfIndex,
                       &dot3adAggPortStatsEntryData.dot3adAggPortStatsIllegalRx) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortStatsIllegalRx, dot3adAggPortStatsEntryData.valid);
     break;

   case I_dot3adAggPortStatsLACPDUsTx:
     if ( usmDbDot3adAggPortStatsLACPDUsTxGet(USMDB_UNIT_CURRENT, intIfIndex,
                       &dot3adAggPortStatsEntryData.dot3adAggPortStatsLACPDUsTx) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortStatsLACPDUsTx, dot3adAggPortStatsEntryData.valid);
     break;

#ifdef I_dot3adAggPortStatsMarkerPDUsTx
   case I_dot3adAggPortStatsMarkerPDUsTx:
     if ( usmDbDot3adAggPortStatsMarkerPDUsTxGet(USMDB_UNIT_CURRENT, intIfIndex,
                       &dot3adAggPortStatsEntryData.dot3adAggPortStatsMarkerPDUsTx) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortStatsMarkerPDUsTx, dot3adAggPortStatsEntryData.valid);
     break;
#endif

   case I_dot3adAggPortStatsMarkerResponsePDUsTx:
     if ( usmDbDot3adAggPortStatsMarkerResponsePDUsTxGet(USMDB_UNIT_CURRENT, intIfIndex,
                       &dot3adAggPortStatsEntryData.dot3adAggPortStatsMarkerResponsePDUsTx) == L7_SUCCESS )
       SET_VALID(I_dot3adAggPortStatsMarkerResponsePDUsTx, dot3adAggPortStatsEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if ( nominator >= 0 && !VALID(nominator, dot3adAggPortStatsEntryData.valid) )
     return(NULL);

   return(&dot3adAggPortStatsEntryData);
}

dot3adAggPortDebugEntry_t *
k_dot3adAggPortDebugEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_INT32 dot3adAggPortIndex)
{
#ifdef NOT_SUPPORTED  /* Debug table not supported */
   static dot3adAggPortDebugEntry_t dot3adAggPortDebugEntryData;

   /*
    * put your code to retrieve the information here
    */

   dot3adAggPortDebugEntryData.dot3adAggPortDebugRxState = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugLastRxTime = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugMuxState = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugMuxReason = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugActorChurnState = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugPartnerChurnState = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugActorChurnCount = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugPartnerChurnCount = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugActorSyncTransitionCount = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugPartnerSyncTransitionCount = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugActorChangeCount = ;
   dot3adAggPortDebugEntryData.dot3adAggPortDebugPartnerChangeCount = ;
   dot3adAggPortDebugEntryData.dot3adAggPortIndex = ;
   SET_ALL_VALID(dot3adAggPortDebugEntryData.valid);
   return(&dot3adAggPortDebugEntryData);
#else /* NOT_SUPPORTED */
   return(NULL);
#endif /* NOT_SUPPORTED */
}


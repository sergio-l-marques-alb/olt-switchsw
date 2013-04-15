/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\snmp\snmp_sr\snmpb\unix\k_mib_ripv2.c
*
* @purpose  rip version 2 routines
*                  
* @component 
*
* @comments 
*
* @create 05/10/2001
*
* @author soma
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#include <k_private_base.h>
#include "k_mib_ripv2_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_common.h"
#include "usmdb_iputil_api.h"
#include "usmdb_mib_ripv2_api.h"

rip2Globals_t *     
   k_rip2Globals_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
  static rip2Globals_t rip2GlobalsData;
  
  ZERO_VALID(rip2GlobalsData.valid);
  
  switch(nominator)
  {
  case -1:
    break;
  
  case I_rip2GlobalRouteChanges:
    if ( usmDbRip2GlobalRouteChangesGet(USMDB_UNIT_CURRENT, &rip2GlobalsData.rip2GlobalRouteChanges) == L7_SUCCESS )
      SET_VALID(I_rip2GlobalRouteChanges, rip2GlobalsData.valid);
    break;
           
  case I_rip2GlobalQueries: 
    if ( usmDbRip2GlobalQueriesGet(USMDB_UNIT_CURRENT, &rip2GlobalsData.rip2GlobalQueries) == L7_SUCCESS )
      SET_VALID(I_rip2GlobalQueries, rip2GlobalsData.valid);
    break;
  } /* end of switch(nominator)  */

  if ( nominator >= 0 && !VALID(nominator, rip2GlobalsData.valid) )
    return(NULL);
    
  return(&rip2GlobalsData);
}

rip2IfStatEntry_t *
   k_rip2IfStatEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_UINT32 rip2IfStatAddress)
{
  static rip2IfStatEntry_t rip2IfStatEntryData;
  L7_uint32 intIfNum;

/* lvl7_@p0971 start */
  ZERO_VALID(rip2IfStatEntryData.valid);
  rip2IfStatEntryData.rip2IfStatAddress = rip2IfStatAddress;
  SET_VALID(I_rip2IfStatAddress, rip2IfStatEntryData.valid);
/* lvl7_@p0971 end */

/* lvl7_@p1707 start */
  if ( (searchType == EXACT) ?
       (snmpRip2IfStatEntryGet(USMDB_UNIT_CURRENT, rip2IfStatEntryData.rip2IfStatAddress) != L7_SUCCESS) :
       ( (snmpRip2IfStatEntryGet(USMDB_UNIT_CURRENT, rip2IfStatEntryData.rip2IfStatAddress) != L7_SUCCESS) &&
         (snmpRip2IfStatEntryNextGet(USMDB_UNIT_CURRENT, &rip2IfStatEntryData.rip2IfStatAddress) != L7_SUCCESS )))
  {
/* lvl7_@p1707 start */
    ZERO_VALID(rip2IfStatEntryData.valid);
    return(NULL);
  }

   /* Conversion to internal interface number for usmdb calls */
  if (usmDbRip2IfIPAddrToIntIf(USMDB_UNIT_CURRENT, rip2IfStatEntryData.rip2IfStatAddress, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(rip2IfStatEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_rip2IfStatAddress:
    break;

  case I_rip2IfStatRcvBadPackets:
    if ( usmDbRip2IfStatRcvBadPacketsGet(USMDB_UNIT_CURRENT, intIfNum,
                                         &rip2IfStatEntryData.rip2IfStatRcvBadPackets) == L7_SUCCESS )
      SET_VALID(I_rip2IfStatRcvBadPackets, rip2IfStatEntryData.valid);
    break;

  case I_rip2IfStatRcvBadRoutes:
    if ( usmDbRip2IfStatRcvBadRoutesGet(USMDB_UNIT_CURRENT, intIfNum,
                                        &rip2IfStatEntryData.rip2IfStatRcvBadRoutes) == L7_SUCCESS )
      SET_VALID(I_rip2IfStatRcvBadRoutes, rip2IfStatEntryData.valid);
    break;

  case I_rip2IfStatSentUpdates:
    if ( usmDbRip2IfStatSentUpdatesGet(USMDB_UNIT_CURRENT, intIfNum,
                                       &rip2IfStatEntryData.rip2IfStatSentUpdates) == L7_SUCCESS )
      SET_VALID(I_rip2IfStatSentUpdates, rip2IfStatEntryData.valid);
    break;

  case I_rip2IfStatStatus:
/* lvl7_@p0971 start */
    if ( snmpRip2IfStatStatusGet(USMDB_UNIT_CURRENT, intIfNum,
                                 &rip2IfStatEntryData.rip2IfStatStatus) == L7_SUCCESS )
      SET_VALID(I_rip2IfStatStatus, rip2IfStatEntryData.valid);
/* lvl7_@p0971 end */
    break;
  }  /* end of switch(nominator)  */

  if ( nominator >= 0 && !VALID(nominator, rip2IfStatEntryData.valid) )
    return(NULL);

  return(&rip2IfStatEntryData);
}

#ifdef SETS
int
   k_rip2IfStatEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
   k_rip2IfStatEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
   k_rip2IfStatEntry_set_defaults(doList_t *dp)
{
   rip2IfStatEntry_t *data = (rip2IfStatEntry_t *) (dp->data);

   data->rip2IfStatRcvBadPackets = (SR_UINT32) 0;
   data->rip2IfStatRcvBadRoutes = (SR_UINT32) 0;
   data->rip2IfStatSentUpdates = (SR_UINT32) 0;
/* lvl7_@p0971 start */
   data->rip2IfStatStatus = D_rip2IfStatStatus_createAndGo;

   ZERO_VALID(data->valid);
   SET_VALID(I_rip2IfStatStatus, data->valid);
/*   SET_ALL_VALID(data->valid);*/
/* lvl7_@p0971 end */
   return(NO_ERROR);
}

int
   k_rip2IfStatEntry_set(rip2IfStatEntry_t *data,
                         ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

/* lvl7_@p0971 start */
/* check to see if there is an interface associated with this ip address */
/*   if (usmDbRip2IfStatEntryGet(USMDB_UNIT_CURRENT, &data->rip2IfStatAddress) != L7_SUCCESS)*/
  if (usmDbRip2IfIPAddrToIntIf(USMDB_UNIT_CURRENT, data->rip2IfStatAddress, &intIfNum) != L7_SUCCESS)
  {
/* lvl7_@p0971 end */
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

/* lvl7_@p0971 start */
  if ( VALID(I_rip2IfStatStatus, data->valid) && 
       snmpRip2IfStatStatusSet(USMDB_UNIT_CURRENT, intIfNum,
                               data->rip2IfStatStatus) != L7_SUCCESS )
  {
/* lvl7_@p0971 end */
    CLR_VALID(I_rip2IfStatStatus, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}

#ifdef SR_rip2IfStatEntry_UNDO
/* add #define SR_rip2IfStatEntry_UNDO in sitedefs.h to
 * include the undo routine for the rip2IfStatEntry family.
 */
int
   rip2IfStatEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_rip2IfStatEntry_UNDO */

#endif /* SETS */

rip2IfConfEntry_t *
   k_rip2IfConfEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_UINT32 rip2IfConfAddress)
{
  static rip2IfConfEntry_t rip2IfConfEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    rip2IfConfEntryData.rip2IfConfDomain  = MakeOctetString(NULL, 0);
    rip2IfConfEntryData.rip2IfConfAuthKey = MakeOctetString(NULL, 0);

    firstTime = L7_FALSE;
  }

/* lvl7_@p0971 start */
  ZERO_VALID(rip2IfConfEntryData.valid);
/* lvl7_@p0971 end */
  rip2IfConfEntryData.rip2IfConfAddress = rip2IfConfAddress;
  SET_VALID(I_rip2IfConfAddress, rip2IfConfEntryData.valid);

  if ( ((searchType == EXACT) ?
        (snmpRip2IfConfEntryGet(USMDB_UNIT_CURRENT, rip2IfConfEntryData.rip2IfConfAddress) != L7_SUCCESS) :
        ((snmpRip2IfConfEntryGet(USMDB_UNIT_CURRENT, rip2IfConfEntryData.rip2IfConfAddress) != L7_SUCCESS) &&
         (usmDbRip2IfConfEntryNext(USMDB_UNIT_CURRENT, &rip2IfConfEntryData.rip2IfConfAddress) != L7_SUCCESS)))  ||
       (usmDbRip2IfIPAddrToIntIf(USMDB_UNIT_CURRENT, rip2IfConfEntryData.rip2IfConfAddress, &intIfNum)  != L7_SUCCESS))
  {
    ZERO_VALID(rip2IfConfEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_rip2IfConfAddress:
    break;

/* lvl7_@p0975 start */
#ifdef NOT_SUPPORTED
  case I_rip2IfConfDomain:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ( usmDbRip2IfConfDomainGet(USMDB_UNIT_CURRENT, intIfNum, snmp_buffer) == L7_SUCCESS &&
         (SafeMakeOctetStringFromText(&rip2IfConfEntryData.rip2IfConfDomain, snmp_buffer) == L7_TRUE ))
      SET_VALID(I_rip2IfConfDomain, rip2IfConfEntryData.valid);
    break;
#endif /* NOT_SUPPORTED */

/* lvl7_@p0975 end */

  case I_rip2IfConfAuthType:
    if ( snmpRip2IfConfAuthTypeGet(USMDB_UNIT_CURRENT, intIfNum,
                                   &rip2IfConfEntryData.rip2IfConfAuthType) == L7_SUCCESS )
      SET_VALID(I_rip2IfConfAuthType, rip2IfConfEntryData.valid);
    break;

  case I_rip2IfConfAuthKey: 
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
/* lvl7_@p0973 start */
    if ( usmDbRip2IfConfAuthKeyGet(USMDB_UNIT_CURRENT, intIfNum, snmp_buffer) == L7_SUCCESS  &&
         ( SafeMakeOctetStringFromText(&rip2IfConfEntryData.rip2IfConfAuthKey, snmp_buffer) == L7_TRUE))
/* lvl7_@p0973 end */
      SET_VALID(I_rip2IfConfAuthKey, rip2IfConfEntryData.valid);
    break;

  case I_rip2IfConfSend:
    if ( snmpRip2IfConfSendGet(USMDB_UNIT_CURRENT, intIfNum,
                               &rip2IfConfEntryData.rip2IfConfSend) == L7_SUCCESS )
      SET_VALID(I_rip2IfConfSend, rip2IfConfEntryData.valid);
    break;

  case I_rip2IfConfReceive:
    if ( snmpRip2IfConfReceiveGet(USMDB_UNIT_CURRENT, intIfNum,
                                  &rip2IfConfEntryData.rip2IfConfReceive) == L7_SUCCESS )
      SET_VALID(I_rip2IfConfReceive, rip2IfConfEntryData.valid);
    break;

  case I_rip2IfConfDefaultMetric:
    if ( usmDbRip2IfConfDefaultMetricGet(USMDB_UNIT_CURRENT, intIfNum,
                                         &rip2IfConfEntryData.rip2IfConfDefaultMetric) == L7_SUCCESS )
      SET_VALID(I_rip2IfConfDefaultMetric, rip2IfConfEntryData.valid);
    break;

  case I_rip2IfConfStatus:
/* lvl7_@p0971 start */
    if ( snmpRip2IfConfStatusGet(USMDB_UNIT_CURRENT, intIfNum,
                                 &rip2IfConfEntryData.rip2IfConfStatus) == L7_SUCCESS )
      SET_VALID(I_rip2IfConfStatus, rip2IfConfEntryData.valid);
/* lvl7_@p0971 end */
    break;

  case I_rip2IfConfSrcAddress:
    if ( usmDbRip2IfConfSrcIpAddrGet(USMDB_UNIT_CURRENT, intIfNum,
                                     &rip2IfConfEntryData.rip2IfConfSrcAddress) == L7_SUCCESS )
      SET_VALID(I_rip2IfConfSrcAddress, rip2IfConfEntryData.valid);
    break;

  case I_agentRip2IfConfAuthKeyId:
    if ( usmDbRip2IfConfAuthKeyIdGet(USMDB_UNIT_CURRENT, intIfNum,
                                     &rip2IfConfEntryData.agentRip2IfConfAuthKeyId) == L7_SUCCESS )
      SET_VALID(I_agentRip2IfConfAuthKeyId, rip2IfConfEntryData.valid);
    break;

  } /* end of switch(nominator)  */

  if ( nominator >= 0 && !VALID(nominator, rip2IfConfEntryData.valid) )
    return(NULL);

  return(&rip2IfConfEntryData);
}

#ifdef SETS
int
   k_rip2IfConfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
   k_rip2IfConfEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
   k_rip2IfConfEntry_set_defaults(doList_t *dp)
{
   rip2IfConfEntry_t *data = (rip2IfConfEntry_t *) (dp->data);

   if ( (data->rip2IfConfDomain = MakeOctetStringFromText("")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   data->rip2IfConfAuthType = D_rip2IfConfAuthType_noAuthentication;
   if ( (data->rip2IfConfAuthKey = MakeOctetStringFromText("")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   data->agentRip2IfConfAuthKeyId = 0;
   data->rip2IfConfSend = D_rip2IfConfSend_rip1Compatible;
   data->rip2IfConfReceive = D_rip2IfConfReceive_rip1OrRip2;
/* lvl7_@p0971 start */
   data->rip2IfConfStatus = D_rip2IfConfStatus_createAndGo;
   ZERO_VALID(data->valid);
   SET_VALID(I_rip2IfConfStatus, data->valid);
/*   SET_ALL_VALID(data->valid);*/
/* lvl7_@p0971 end */
   return(NO_ERROR);
}

int
   k_rip2IfConfEntry_set(rip2IfConfEntry_t *data,
                         ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  char snmp_buffer[SNMP_BUFFER_LEN];

/* lvl7_@p0971 start */
  if ( usmDbRip2IfIPAddrToIntIf(USMDB_UNIT_CURRENT, data->rip2IfConfAddress, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
/* lvl7_@p0971 end */
  }
/* lvl7_@p0975 start */
#ifdef NOT_SUPPORTED
  if (data->rip2IfConfDomain != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->rip2IfConfDomain->octet_ptr, data->rip2IfConfDomain->length);
    if (VALID(I_rip2IfConfDomain, data->valid) &&
        usmDbRip2IfConfDomainSet(USMDB_UNIT_CURRENT, intIfNum,
                                 snmp_buffer) != L7_SUCCESS)
    {
      CLR_VALID(I_rip2IfConfDomain, data->valid);
      return(COMMIT_FAILED_ERROR);
    }
  }
#endif /* NOT_SUPPORTED */
/* lvl7_@p0975 end */

  if ( VALID(I_rip2IfConfAuthType, data->valid) && 
       snmpRip2IfConfAuthTypeSet(USMDB_UNIT_CURRENT, intIfNum,
                                 data->rip2IfConfAuthType) != L7_SUCCESS )
  {
    CLR_VALID(I_rip2IfConfAuthType, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if (data->rip2IfConfAuthKey != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->rip2IfConfAuthKey->octet_ptr, data->rip2IfConfAuthKey->length);
    if (VALID(I_rip2IfConfAuthKey, data->valid) &&
        (usmDbValidateRtrIntf(USMDB_UNIT_CURRENT, intIfNum) != L7_SUCCESS ||
         usmDbRip2IfConfAuthKeySet(USMDB_UNIT_CURRENT, intIfNum, snmp_buffer) != L7_SUCCESS))
    {
      CLR_VALID(I_rip2IfConfAuthKey, data->valid);
      return(COMMIT_FAILED_ERROR);
    }
  }

  if ( VALID(I_rip2IfConfSend, data->valid) && 
       snmpRip2IfConfSendSet(USMDB_UNIT_CURRENT, intIfNum,
                             data->rip2IfConfSend) != L7_SUCCESS )
  {
    CLR_VALID(I_rip2IfConfSend, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_rip2IfConfReceive, data->valid) && 
       snmpRip2IfConfReceiveSet(USMDB_UNIT_CURRENT, intIfNum,
                                data->rip2IfConfReceive) != L7_SUCCESS )
  {
    CLR_VALID(I_rip2IfConfReceive, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_rip2IfConfDefaultMetric, data->valid) && 
       (usmDbValidateRtrIntf(USMDB_UNIT_CURRENT, intIfNum) != L7_SUCCESS ||
        usmDbRip2IfConfDefaultMetricSet(USMDB_UNIT_CURRENT, intIfNum,
                                       data->rip2IfConfDefaultMetric) != L7_SUCCESS ))
  {
    CLR_VALID(I_rip2IfConfDefaultMetric, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

/* lvl7_@p0971 start */
  if ( VALID(I_rip2IfConfStatus, data->valid) && 
       snmpRip2IfConfStatusSet(USMDB_UNIT_CURRENT, intIfNum,
                               data->rip2IfConfStatus) != L7_SUCCESS )
  {
/* lvl7_@p0971 end */
    CLR_VALID(I_rip2IfConfStatus, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_rip2IfConfSrcAddress, data->valid) && 
       usmDbRip2IfConfSrcIpAddrSet(USMDB_UNIT_CURRENT, intIfNum,
                                   data->rip2IfConfSrcAddress) != L7_SUCCESS )
  {
    CLR_VALID(I_rip2IfConfSrcAddress, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_agentRip2IfConfAuthKeyId, data->valid) && 
       usmDbRip2IfConfAuthKeyIdSet(USMDB_UNIT_CURRENT, intIfNum,
                                   data->agentRip2IfConfAuthKeyId) != L7_SUCCESS )
  {
    CLR_VALID(I_agentRip2IfConfAuthKeyId, data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}

#ifdef SR_rip2IfConfEntry_UNDO
/* add #define SR_rip2IfConfEntry_UNDO in sitedefs.h to
 * include the undo routine for the rip2IfConfEntry family.
 */
int
   rip2IfConfEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_rip2IfConfEntry_UNDO */

#endif /* SETS */

rip2PeerEntry_t *
   k_rip2PeerEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_UINT32 rip2PeerAddress,
                       OctetString * rip2PeerDomain)
{
  static rip2PeerEntry_t rip2PeerEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  ZERO_VALID(rip2PeerEntryData.valid);
  rip2PeerEntryData.rip2PeerAddress = rip2PeerAddress;
  SET_VALID(I_rip2PeerAddress, rip2PeerEntryData.valid);

  if (rip2PeerDomain->length > SNMP_BUFFER_LEN)
    return(NULL);
  memcpy(snmp_buffer, rip2PeerDomain->octet_ptr, rip2PeerDomain->length);

  SET_VALID(I_rip2PeerDomain, rip2PeerEntryData.valid);

  if ( ( ( searchType == EXACT ) ?
         ( usmDbRip2PeerEntryGet(USMDB_UNIT_CURRENT, rip2PeerEntryData.rip2PeerAddress, snmp_buffer) != L7_SUCCESS ) :
         (( usmDbRip2PeerEntryGet(USMDB_UNIT_CURRENT, rip2PeerEntryData.rip2PeerAddress, snmp_buffer) != L7_SUCCESS ) &&
          ( usmDbRip2PeerEntryNext(USMDB_UNIT_CURRENT, &rip2PeerEntryData.rip2PeerAddress,  snmp_buffer) != L7_SUCCESS ))) || 
       (SafeMakeOctetStringFromText(&rip2PeerEntryData.rip2PeerDomain, snmp_buffer) == L7_FALSE ))
  {
    ZERO_VALID(rip2PeerEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_rip2PeerAddress:
  case I_rip2PeerDomain:
    break;

  case I_rip2PeerLastUpdate:
    if ( usmDbRip2PeerLastUpdateGet(USMDB_UNIT_CURRENT, rip2PeerEntryData.rip2PeerAddress,
                                    rip2PeerEntryData.rip2PeerDomain->octet_ptr,
                                    &rip2PeerEntryData.rip2PeerLastUpdate) == L7_SUCCESS )
      SET_VALID(I_rip2PeerLastUpdate, rip2PeerEntryData.valid);
   break;

  case I_rip2PeerVersion:
    if ( usmDbRip2PeerVersionGet(USMDB_UNIT_CURRENT, rip2PeerEntryData.rip2PeerAddress,
                                 rip2PeerEntryData.rip2PeerDomain->octet_ptr,
                                 &rip2PeerEntryData.rip2PeerVersion) == L7_SUCCESS )
      SET_VALID(I_rip2PeerVersion, rip2PeerEntryData.valid);
   break;

  case I_rip2PeerRcvBadPackets:
    if ( usmDbRip2PeerRcvBadPacketsGet(USMDB_UNIT_CURRENT, rip2PeerEntryData.rip2PeerAddress,
                                       rip2PeerEntryData.rip2PeerDomain->octet_ptr,
                                       &rip2PeerEntryData.rip2PeerRcvBadPackets) == L7_SUCCESS )
      SET_VALID(I_rip2PeerRcvBadPackets, rip2PeerEntryData.valid);
   break;

  case I_rip2PeerRcvBadRoutes:
    if ( usmDbRip2PeerRcvBadRoutesGet(USMDB_UNIT_CURRENT, rip2PeerEntryData.rip2PeerAddress,
                                      rip2PeerEntryData.rip2PeerDomain->octet_ptr,
                                      &rip2PeerEntryData.rip2PeerRcvBadRoutes) == L7_SUCCESS )
      SET_VALID(I_rip2PeerRcvBadRoutes, rip2PeerEntryData.valid);
    break;
  } /* end of switch(nominator)  */

  if ( nominator >= 0 && !VALID(nominator, rip2PeerEntryData.valid) )
    return(NULL);

  return(&rip2PeerEntryData);
}

/***************************************************************************************/
agentRouterRipConfigGroup_t *
k_agentRouterRipConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                int nominator)
{
   static agentRouterRipConfigGroup_t agentRouterRipConfigGroupData;

   ZERO_VALID(agentRouterRipConfigGroupData.valid);

   switch (nominator)
   {
   case I_agentRouterRipAdminState:
     if (snmpAgentRouterRipAdminStateGet(USMDB_UNIT_CURRENT, &agentRouterRipConfigGroupData.agentRouterRipAdminState) == L7_SUCCESS)
       SET_VALID(I_agentRouterRipAdminState, agentRouterRipConfigGroupData.valid);
     break;

   case I_agentRouterRipSplitHorizonMode:
     if (snmpAgentRouterRipSplitHorizonModeGet(USMDB_UNIT_CURRENT, &agentRouterRipConfigGroupData.agentRouterRipSplitHorizonMode) == L7_SUCCESS)
       SET_VALID(I_agentRouterRipSplitHorizonMode, agentRouterRipConfigGroupData.valid);
     break;

   case I_agentRouterRipAutoSummaryMode:
     if (snmpAgentRouterRipAutoSummaryModeGet(USMDB_UNIT_CURRENT, &agentRouterRipConfigGroupData.agentRouterRipAutoSummaryMode) == L7_SUCCESS)
       SET_VALID(I_agentRouterRipAutoSummaryMode, agentRouterRipConfigGroupData.valid);
     break;

   case I_agentRouterRipHostRoutesAcceptMode:
     if (snmpAgentRouterRipHostRoutesAcceptModeGet(USMDB_UNIT_CURRENT, &agentRouterRipConfigGroupData.agentRouterRipHostRoutesAcceptMode) == L7_SUCCESS)
       SET_VALID(I_agentRouterRipHostRoutesAcceptMode, agentRouterRipConfigGroupData.valid);
     break;

   case I_agentRouterRipDefaultMetric:
     if (snmpAgentRipDefaultMetricGet(USMDB_UNIT_CURRENT, &agentRouterRipConfigGroupData.agentRouterRipDefaultMetric) == L7_SUCCESS)
     SET_VALID(I_agentRouterRipDefaultMetric, agentRouterRipConfigGroupData.valid);
     break;


   case I_agentRouterRipDefaultMetricConfigured:
    if (snmpAgentRipDefaultMetricConfiguredGet(USMDB_UNIT_CURRENT,
            &agentRouterRipConfigGroupData.agentRouterRipDefaultMetricConfigured) == L7_SUCCESS)
    SET_VALID(I_agentRouterRipDefaultMetricConfigured, agentRouterRipConfigGroupData.valid);
    break;

   case I_agentRouterRipDefaultInfoOriginate:
    if (snmpAgentRipDefaultInfoOriginateGet(USMDB_UNIT_CURRENT,
            &agentRouterRipConfigGroupData.agentRouterRipDefaultInfoOriginate) == L7_SUCCESS)
    SET_VALID(I_agentRouterRipDefaultInfoOriginate, agentRouterRipConfigGroupData.valid);
    break;

   case I_agentRouterRipRoutePref:
    if (snmpAgentRouterRipRoutePrefGet(USMDB_UNIT_CURRENT,
            &agentRouterRipConfigGroupData.agentRouterRipRoutePref) == L7_SUCCESS)
    SET_VALID(I_agentRouterRipRoutePref, agentRouterRipConfigGroupData.valid);
    break;


   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   return(&agentRouterRipConfigGroupData);
}

#ifdef SETS
int
k_agentRouterRipConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentRouterRipConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentRouterRipConfigGroup_set_defaults(doList_t *dp)
{
    agentRouterRipConfigGroup_t *data = (agentRouterRipConfigGroup_t *) (dp->data);

    data->agentRouterRipSplitHorizonMode = D_agentRouterRipSplitHorizonMode_simple;
    SET_VALID(I_agentRouterRipSplitHorizonMode, data->valid);
    data->agentRouterRipAutoSummaryMode = D_agentRouterRipAutoSummaryMode_enable;
    SET_VALID(I_agentRouterRipAutoSummaryMode, data->valid);
    data->agentRouterRipHostRoutesAcceptMode = D_agentRouterRipHostRoutesAcceptMode_enable;
    SET_VALID(I_agentRouterRipHostRoutesAcceptMode, data->valid);

    return NO_ERROR;
}

int
k_agentRouterRipConfigGroup_set(agentRouterRipConfigGroup_t *data,
                                ContextInfo *contextInfo, int function)
{
  /* lvl7_@p1515 start */

  if (VALID(I_agentRouterRipAdminState, data->valid) &&
      snmpAgentRouterRipAdminStateSet(USMDB_UNIT_CURRENT, data->agentRouterRipAdminState) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterRipAdminState, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRouterRipSplitHorizonMode, data->valid) &&
      snmpAgentRouterRipSplitHorizonModeSet(USMDB_UNIT_CURRENT, data->agentRouterRipSplitHorizonMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterRipSplitHorizonMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRouterRipAutoSummaryMode, data->valid) &&
      snmpAgentRouterRipAutoSummaryModeSet(USMDB_UNIT_CURRENT, data->agentRouterRipAutoSummaryMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterRipAutoSummaryMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRouterRipHostRoutesAcceptMode, data->valid) &&
      snmpAgentRouterRipHostRoutesAcceptModeSet(USMDB_UNIT_CURRENT, data->agentRouterRipHostRoutesAcceptMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterRipHostRoutesAcceptMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRouterRipDefaultMetric, data->valid) &&
    snmpAgentRipDefaultMetricSet(USMDB_UNIT_CURRENT, L7_ENABLE,
                              data->agentRouterRipDefaultMetric)!= L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterRipDefaultMetric, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRouterRipDefaultMetricConfigured, data->valid) &&
    snmpAgentRipDefaultMetricConfiguredSet(USMDB_UNIT_CURRENT,
                              data->agentRouterRipDefaultMetricConfigured)!= L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterRipDefaultMetricConfigured, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRouterRipDefaultInfoOriginate, data->valid) &&
    snmpAgentRipDefaultInfoOriginateSet(USMDB_UNIT_CURRENT,
                          data->agentRouterRipDefaultInfoOriginate) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterRipDefaultInfoOriginate, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRouterRipRoutePref, data->valid) &&
    snmpAgentRouterRipRoutePrefSet(USMDB_UNIT_CURRENT,
                                   data->agentRouterRipRoutePref) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterRipRoutePref, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
  /* lvl7_@p1515 end */
}

#ifdef SR_agentRouterRipConfigGroup_UNDO
/* add #define SR_agentRouterRipConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentRouterRipConfigGroup family.
 */
int
agentRouterRipConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentRouterRipConfigGroup_UNDO */

#endif /* SETS */

agentRipRouteRedistEntry_t *
k_agentRipRouteRedistEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 agentRipRouteRedistSource)
{
   static agentRipRouteRedistEntry_t agentRipRouteRedistEntryData;

   ZERO_VALID(agentRipRouteRedistEntryData.valid);
   agentRipRouteRedistEntryData.agentRipRouteRedistSource =   agentRipRouteRedistSource;
   SET_VALID(I_agentRipRouteRedistSource, agentRipRouteRedistEntryData.valid);

   if ( (searchType == EXACT) ?
        (snmpAgentRipRouteRedistributeSourceGet (USMDB_UNIT_CURRENT, agentRipRouteRedistEntryData.agentRipRouteRedistSource) != L7_SUCCESS) :
        ( (snmpAgentRipRouteRedistributeSourceGet(USMDB_UNIT_CURRENT, agentRipRouteRedistEntryData.agentRipRouteRedistSource) != L7_SUCCESS) &&
          (snmpAgentRipRouteRedistributeSourceGetNext (USMDB_UNIT_CURRENT, &agentRipRouteRedistEntryData.agentRipRouteRedistSource) != L7_SUCCESS) ) )
   {
     ZERO_VALID(agentRipRouteRedistEntryData.valid);
     return(NULL);
   }


   switch (nominator)
   {
   case -1:

   case I_agentRipRouteRedistSource :
     break;

   case I_agentRipRouteRedistMode :
     if (snmpAgentRipRedistributeModeGet(USMDB_UNIT_CURRENT,
                                   agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                                   &agentRipRouteRedistEntryData.agentRipRouteRedistMode) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistMode, agentRipRouteRedistEntryData.valid);
     break;

   case I_agentRipRouteRedistMetric :
     if (snmpAgentRipRouteRedistributeMetricGet(USMDB_UNIT_CURRENT,
                                   agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                                   &agentRipRouteRedistEntryData.agentRipRouteRedistMetric) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistMetric, agentRipRouteRedistEntryData.valid);
     break;

   case I_agentRipRouteRedistMetricConfigured :
     if (snmpAgentRipRedistMetricConfiguredGet(USMDB_UNIT_CURRENT,
                                             agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                                             &agentRipRouteRedistEntryData.agentRipRouteRedistMetricConfigured) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistMetricConfigured, agentRipRouteRedistEntryData.valid);
     break;

   case I_agentRipRouteRedistMatchInternal :
     if (snmpAgentRipRedistMatchInternalGet(USMDB_UNIT_CURRENT,
                                          agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                                          &agentRipRouteRedistEntryData.agentRipRouteRedistMatchInternal) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistMatchInternal, agentRipRouteRedistEntryData.valid);
     break;

   case I_agentRipRouteRedistMatchExternal1 :
     if (snmpAgentRipRedistMatchExternal1Get(USMDB_UNIT_CURRENT,
                                           agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                                           &agentRipRouteRedistEntryData.agentRipRouteRedistMatchExternal1) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistMatchExternal1, agentRipRouteRedistEntryData.valid);
     break;

   case I_agentRipRouteRedistMatchExternal2 :
     if (snmpAgentRipRedistMatchExternal2Get(USMDB_UNIT_CURRENT,
                                           agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                                           &agentRipRouteRedistEntryData.agentRipRouteRedistMatchExternal2) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistMatchExternal2, agentRipRouteRedistEntryData.valid);
     break;

   case I_agentRipRouteRedistMatchNSSAExternal1 :
     if (snmpAgentRipRedistMatchNSSAExternal1Get(USMDB_UNIT_CURRENT,
                                               agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                                               &agentRipRouteRedistEntryData.agentRipRouteRedistMatchNSSAExternal1) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistMatchNSSAExternal1, agentRipRouteRedistEntryData.valid);
     break;

   case I_agentRipRouteRedistMatchNSSAExternal2 :
     if (snmpAgentRipRedistMatchNSSAExternal2Get(USMDB_UNIT_CURRENT,
                                               agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                                               &agentRipRouteRedistEntryData.agentRipRouteRedistMatchNSSAExternal2) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistMatchNSSAExternal2, agentRipRouteRedistEntryData.valid);
     break;

   case I_agentRipRouteRedistDistList :
     if (snmpAgentRipRouteRedistributeFilterGet(USMDB_UNIT_CURRENT,
                                     agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                                     &agentRipRouteRedistEntryData.agentRipRouteRedistDistList) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistDistList, agentRipRouteRedistEntryData.valid);
     break;

   case I_agentRipRouteRedistDistListConfigured :
     if (snmpRipRouteRedistDistListConfiguredGet(USMDB_UNIT_CURRENT,
                                     agentRipRouteRedistEntryData.agentRipRouteRedistSource,
                      &agentRipRouteRedistEntryData.agentRipRouteRedistDistListConfigured) == L7_SUCCESS)
     SET_VALID(I_agentRipRouteRedistDistListConfigured, agentRipRouteRedistEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
    break;
   }


   if (nominator >= 0 && !VALID(nominator, agentRipRouteRedistEntryData.valid))
     return(NULL);

   return(&agentRipRouteRedistEntryData);
}

#ifdef SETS
int
k_agentRipRouteRedistEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentRipRouteRedistEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentRipRouteRedistEntry_set_defaults(doList_t *dp)
{
    agentRipRouteRedistEntry_t *data = (agentRipRouteRedistEntry_t *) (dp->data);

    data->agentRipRouteRedistMode = D_agentRipRouteRedistMode_disable;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentRipRouteRedistEntry_set(agentRipRouteRedistEntry_t *data,
                               ContextInfo *contextInfo, int function)
{

   if (VALID(I_agentRipRouteRedistMode, data->valid) &&
    snmpAgentRipRedistributeSet(USMDB_UNIT_CURRENT,
                                data->agentRipRouteRedistSource,
                                data->agentRipRouteRedistMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRipRouteRedistMetric, data->valid) &&
    snmpAgentRipRouteRedistributeMetricSet(USMDB_UNIT_CURRENT,
                             data->agentRipRouteRedistSource,
                             L7_ENABLE,
                             data->agentRipRouteRedistMetric) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistMetric, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRipRouteRedistMetricConfigured, data->valid) &&
    snmpAgentRipRedistMetricConfiguredSet(USMDB_UNIT_CURRENT,
                             data->agentRipRouteRedistSource,
                             data->agentRipRouteRedistMetricConfigured) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistMetricConfigured, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRipRouteRedistMatchInternal, data->valid) &&
    snmpAgentRipRedistMatchInternalSet(USMDB_UNIT_CURRENT,
                                       data->agentRipRouteRedistSource,
                                       data->agentRipRouteRedistMatchInternal) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistMatchInternal, data->valid);
    return COMMIT_FAILED_ERROR;
  }


  if (VALID(I_agentRipRouteRedistMatchExternal1, data->valid) &&
    snmpAgentRipRedistMatchExternal1Set(USMDB_UNIT_CURRENT,
                                        data->agentRipRouteRedistSource,
                                        data->agentRipRouteRedistMatchExternal1) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistMatchExternal1, data->valid);
    return COMMIT_FAILED_ERROR;
  }


  if (VALID(I_agentRipRouteRedistMatchExternal2, data->valid) &&
    snmpAgentRipRedistMatchExternal2Set(USMDB_UNIT_CURRENT,
                                        data->agentRipRouteRedistSource,
                                        data->agentRipRouteRedistMatchExternal2) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistMatchExternal2, data->valid);
    return COMMIT_FAILED_ERROR;
  }


  if (VALID(I_agentRipRouteRedistMatchNSSAExternal1, data->valid) &&
    snmpAgentRipRedistMatchNSSAExternal1Set(USMDB_UNIT_CURRENT,
                                            data->agentRipRouteRedistSource,
                                            data->agentRipRouteRedistMatchNSSAExternal1) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistMatchNSSAExternal1, data->valid);
    return COMMIT_FAILED_ERROR;
  }


  if (VALID(I_agentRipRouteRedistMatchNSSAExternal2, data->valid) &&
    snmpAgentRipRedistMatchNSSAExternal2Set(USMDB_UNIT_CURRENT,
                                            data->agentRipRouteRedistSource,
                                            data->agentRipRouteRedistMatchNSSAExternal2) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistMatchNSSAExternal2, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRipRouteRedistDistList, data->valid) &&
    snmpAgentRipRouteRedistributeFilterSet(USMDB_UNIT_CURRENT,
                                        data->agentRipRouteRedistSource,
                                        L7_ENABLE,
                                  data->agentRipRouteRedistDistList) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistDistList, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRipRouteRedistDistListConfigured, data->valid) &&
    snmpRipRouteRedistDistListConfiguredSet(USMDB_UNIT_CURRENT,
                                        data->agentRipRouteRedistSource,
                                        data->agentRipRouteRedistDistListConfigured) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRipRouteRedistDistListConfigured, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;

}


#ifdef SR_agentRipRouteRedistEntry_UNDO
/* add #define SR_agentRipRouteRedistEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentRipRouteRedistEntry family.
 */
int
agentRipRouteRedistEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentRipRouteRedistEntry_UNDO */

#endif /* SETS */


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_ospf.c
*
* @purpose  Provide interface to hardware API's OSPF rfc1850 MIB components
*                  
* @component unitmgr
*
* @comments 
*
* @create 05/15/2001
*
* @author soma
* @end
*
**********************************************************************/
/*********************************************************************
 *              
 *********************************************************************/
#include <k_private_base.h>
#include "k_mib_ospf_api.h"
#include "usmdb_common.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_ospf_api.h"
#include "usmdb_util_api.h"

#define SNMP_OSPF_SETTRAP_LEN 4

#undef NOT_SUPPORTED

ospfGeneralGroup_t *
k_ospfGeneralGroup_get(int serialNum, ContextInfo *contextInfo,
                       int nominator)
{
   static ospfGeneralGroup_t ospfGeneralGroupData;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   switch(nominator)
   {

   case -1:
     break;
   
   case I_ospfRouterId:
     if (usmDbOspfRouterIdGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfRouterId) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfAdminStat:
     if (snmpOspfAdminStatGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfAdminStat) == L7_SUCCESS)
     /*if (usmDbOspfAdminStatGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfAdminStat) == L7_SUCCESS)*/
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfVersionNumber:
     if (snmpOspfVersionNumberGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfVersionNumber) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfAreaBdrRtrStatus:
     if (snmpOspfAreaBdrRtrStatusGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfAreaBdrRtrStatus) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfASBdrRtrStatus:
     if (snmpOspfASBdrRtrStatusGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfASBdrRtrStatus) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfExternLsaCount:
     if (usmDbOspfExternalLSACountGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfExternLsaCount) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfExternLsaCksumSum:
     if (usmDbOspfExternalLSAChecksumGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfExternLsaCksumSum) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfTOSSupport:
     if (snmpOspfTOSSupportGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfTOSSupport) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

    case I_ospfOriginateNewLsas:
     if (usmDbOspfNewLSAOrigGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfOriginateNewLsas) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfRxNewLsas:
     if (usmDbOspfNumLSAReceivedGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfRxNewLsas) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfExtLsdbLimit:
     if (usmDbOspfExtLsdbLimitGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfExtLsdbLimit) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfMulticastExtensions:
     if (usmDbOspfMulticastExtensionsGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfMulticastExtensions) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfExitOverflowInterval:
     if (usmDbOspfExitOverflowIntervalGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfExitOverflowInterval) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;

   case I_ospfDemandExtensions:
     CLR_VALID(I_ospfDemandExtensions, ospfGeneralGroupData.valid);
     if (snmpOspfDemandExtensionsGet(USMDB_UNIT_CURRENT, &ospfGeneralGroupData.ospfDemandExtensions) == L7_SUCCESS)
        SET_VALID(nominator, ospfGeneralGroupData.valid);
     break;
   
   default:
     /* unknown nominator */
     return(NULL);
     break;
   }
   
   if (nominator >= 0 && !VALID(nominator, ospfGeneralGroupData.valid))
      return(NULL);

   return(&ospfGeneralGroupData);
}

#ifdef SETS
int
k_ospfGeneralGroup_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfGeneralGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfGeneralGroup_set(ospfGeneralGroup_t *data,
                       ContextInfo *contextInfo, int function)
{

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfRouterId, data->valid) && 
       usmDbOspfRouterIdSet(USMDB_UNIT_CURRENT, data->ospfRouterId) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfAdminStat, data->valid) &&
       snmpOspfAdminStatSet(USMDB_UNIT_CURRENT, data->ospfAdminStat) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
   
   if (VALID(I_ospfASBdrRtrStatus, data->valid) &&
       snmpOspfASBdrRtrStatusSet(USMDB_UNIT_CURRENT, data->ospfASBdrRtrStatus) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfTOSSupport, data->valid) &&
       snmpOspfTOSSupportSet(USMDB_UNIT_CURRENT, data->ospfTOSSupport) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfExtLsdbLimit, data->valid) &&
       usmDbOspfExtLsdbLimitSet(USMDB_UNIT_CURRENT, data->ospfExtLsdbLimit) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfMulticastExtensions, data->valid) &&
       usmDbOspfMulticastExtensionsSet(USMDB_UNIT_CURRENT, data->ospfMulticastExtensions) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfExitOverflowInterval, data->valid) &&
       usmDbOspfExitOverflowIntervalSet(USMDB_UNIT_CURRENT, data->ospfExitOverflowInterval) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfDemandExtensions, data->valid) &&
       snmpOspfDemandExtensionsSet(USMDB_UNIT_CURRENT, data->ospfDemandExtensions) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

   return(NO_ERROR);
}

#ifdef SR_ospfGeneralGroup_UNDO
/* add #define SR_ospfGeneralGroup_UNDO in sitedefs.h to
 * include the undo routine for the ospfGeneralGroup family.
 */
int
ospfGeneralGroup_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfGeneralGroup_UNDO */

#endif /* SETS */

ospfAreaEntry_t *
k_ospfAreaEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_UINT32 ospfAreaId)
{
   static ospfAreaEntry_t ospfAreaEntryData;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

/* lvl7_@p1239 start */
   ZERO_VALID(ospfAreaEntryData.valid);
/* lvl7_@p1239 end */

   ospfAreaEntryData.ospfAreaId = ospfAreaId;
   SET_VALID(I_ospfAreaId, ospfAreaEntryData.valid);

   if ((searchType == EXACT) ?
        (usmDbOspfAreaEntryGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId) != L7_SUCCESS) :
        ((usmDbOspfAreaEntryGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId) != L7_SUCCESS) &&
        (usmDbOspfAreaEntryNext(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId, &ospfAreaEntryData.ospfAreaId) != L7_SUCCESS)))
      return(NULL);

/* lvl7_@p1239 start */
   switch(nominator)
   {       
   case -1:
   case I_ospfAreaId:
     break;

#ifdef OBSOLETE
   case I_ospfAuthType:
     if (usmDbOspfAuthTypeGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId,
                            &ospfAreaEntryData.ospfAuthType) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaEntryData.valid);
     break;
#endif

   case I_ospfImportAsExtern:
     if (snmpOspfImportAsExternGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId,
                                  &ospfAreaEntryData.ospfImportAsExtern) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaEntryData.valid);
     break;

   case I_ospfSpfRuns:
     if (usmDbOspfNumSPFRunsGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId,
                           &ospfAreaEntryData.ospfSpfRuns) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaEntryData.valid);
     break;
                  
   case I_ospfAreaBdrRtrCount:
     if (usmDbOspfAreaBorderRtrCountGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId,
                                   &ospfAreaEntryData.ospfAreaBdrRtrCount) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaEntryData.valid);
     break;

   case I_ospfAsBdrRtrCount:
     if (usmDbOspfAsBdrRtrCountGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId,
                                 &ospfAreaEntryData.ospfAsBdrRtrCount) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaEntryData.valid);
     break;
   
   case I_ospfAreaLsaCount:
     if (usmDbOspfAreaLSACountGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId,
                                &ospfAreaEntryData.ospfAreaLsaCount) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaEntryData.valid);
     break;

   case I_ospfAreaLsaCksumSum:
     if (usmDbOspfAreaLSACksumSumGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId,
                                   &ospfAreaEntryData.ospfAreaLsaCksumSum) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaEntryData.valid);
     break;

   case I_ospfAreaSummary:
     if (snmpOspfAreaSummaryGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId,
                               &ospfAreaEntryData.ospfAreaSummary) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaEntryData.valid);
     break;

   case I_ospfAreaStatus:
     if (snmpOspfAreaStatusGet(USMDB_UNIT_CURRENT, ospfAreaEntryData.ospfAreaId,
                              &ospfAreaEntryData.ospfAreaStatus) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaEntryData.valid);
     break;
   
   default:
     /* unknown nominator */
     return(NULL);
     break;
   }
/* lvl7_@p1239 end */

   if (nominator >= 0 && !VALID(nominator, ospfAreaEntryData.valid))
      return(NULL);

   return(&ospfAreaEntryData);
}

#ifdef SETS
int
k_ospfAreaEntry_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfAreaEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                      doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfAreaEntry_set_defaults(doList_t *dp)
{
   ospfAreaEntry_t *data = (ospfAreaEntry_t *) (dp->data);

   data->ospfAuthType = 0;
   data->ospfImportAsExtern = D_ospfImportAsExtern_importExternal;
   data->ospfSpfRuns = (SR_UINT32) 0;
   data->ospfAreaLsaCksumSum = 0;
   data->ospfAreaSummary = D_ospfAreaSummary_noAreaSummary;
   data->ospfAreaStatus = D_ospfAreaStatus_active;
   
   /* component provides default configuration */
   ZERO_VALID(data->valid);

   return(NO_ERROR);
}

int
k_ospfAreaEntry_set(ospfAreaEntry_t *data,
                    ContextInfo *contextInfo, int function)
{
   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);
 
#ifdef NOT_SUPPORTED
   if (VALID(I_ospfAuthType, data->valid) &&
       usmDbOspfAuthTypeSet(USMDB_UNIT_CURRENT, data->ospfAreaId,
                            data->ospfAuthType) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
#else
   if (VALID(I_ospfAuthType, data->valid))
      return(COMMIT_FAILED_ERROR);
#endif

   if (VALID(I_ospfImportAsExtern, data->valid) &&
       (snmpOspfImportAsExternSet(USMDB_UNIT_CURRENT, data->ospfAreaId,
                                  data->ospfImportAsExtern) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfAreaSummary, data->valid) &&
       (snmpOspfAreaSummarySet(USMDB_UNIT_CURRENT, data->ospfAreaId,
                               data->ospfAreaSummary) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfAreaStatus, data->valid))
      return(COMMIT_FAILED_ERROR);

   
   return(NO_ERROR);
}

#ifdef SR_ospfAreaEntry_UNDO
/* add #define SR_ospfAreaEntry_UNDO in sitedefs.h to
 * include the undo routine for the ospfAreaEntry family.
 */
int
ospfAreaEntry_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfAreaEntry_UNDO */

#endif /* SETS */

ospfStubAreaEntry_t *
k_ospfStubAreaEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_UINT32 ospfStubAreaId,
                        SR_INT32 ospfStubTOS)
{

   static ospfStubAreaEntry_t ospfStubAreaEntryData;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   ZERO_VALID(ospfStubAreaEntryData.valid);

   ospfStubAreaEntryData.ospfStubAreaId = ospfStubAreaId;
   ospfStubAreaEntryData.ospfStubTOS = ospfStubTOS;
   SET_VALID(I_ospfStubAreaId, ospfStubAreaEntryData.valid);
   SET_VALID(I_ospfStubTOS, ospfStubAreaEntryData.valid);
   
   if ((searchType == EXACT) ?
        (usmDbOspfStubAreaEntryGet(USMDB_UNIT_CURRENT, ospfStubAreaEntryData.ospfStubAreaId,
                                  ospfStubAreaEntryData.ospfStubTOS) != L7_SUCCESS) :
       ((usmDbOspfStubAreaEntryGet(USMDB_UNIT_CURRENT, ospfStubAreaEntryData.ospfStubAreaId,
                                 ospfStubAreaEntryData.ospfStubTOS) != L7_SUCCESS) && 
       (usmDbOspfStubAreaEntryNext(USMDB_UNIT_CURRENT, &ospfStubAreaEntryData.ospfStubAreaId,
                                   &ospfStubAreaEntryData.ospfStubTOS) != L7_SUCCESS)))
      return(NULL);

   switch(nominator)
   {

   case -1:
   case I_ospfStubAreaId:
   case I_ospfStubTOS:
     break;
       
   case I_ospfStubMetric:
     if (usmDbOspfStubMetricGet(USMDB_UNIT_CURRENT, ospfStubAreaEntryData.ospfStubAreaId,
                              ospfStubAreaEntryData.ospfStubTOS,
                              &ospfStubAreaEntryData.ospfStubMetric) == L7_SUCCESS)
       SET_VALID(nominator, ospfStubAreaEntryData.valid);
     break;

   case I_ospfStubStatus:
     if (snmpOspfStubStatusGet(USMDB_UNIT_CURRENT, ospfStubAreaEntryData.ospfStubAreaId,
                              ospfStubAreaEntryData.ospfStubTOS,
                              &ospfStubAreaEntryData.ospfStubStatus) == L7_SUCCESS)
       SET_VALID(nominator, ospfStubAreaEntryData.valid);
     break;

   case I_ospfStubMetricType:
     if (snmpOspfStubMetricTypeGet(USMDB_UNIT_CURRENT, ospfStubAreaEntryData.ospfStubAreaId,
                                  ospfStubAreaEntryData.ospfStubTOS,
                                  &ospfStubAreaEntryData.ospfStubMetricType) == L7_SUCCESS)
       SET_VALID(nominator, ospfStubAreaEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, ospfStubAreaEntryData.valid))
      return(NULL);

   return(&ospfStubAreaEntryData);
}

#ifdef SETS
int
k_ospfStubAreaEntry_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfStubAreaEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfStubAreaEntry_set_defaults(doList_t *dp)
{
   ospfStubAreaEntry_t *data = (ospfStubAreaEntry_t *) (dp->data);

   data->ospfStubMetricType = D_ospfStubMetricType_ospfMetric;
   data ->ospfStubStatus = D_ospfStubStatus_active;

   /* component provides default configuration */
   ZERO_VALID(data->valid);
   return(NO_ERROR);
}

int
k_ospfStubAreaEntry_set(ospfStubAreaEntry_t *data,
                        ContextInfo *contextInfo, int function)
{
   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);
    
   if (usmDbOspfStubAreaEntryGet(USMDB_UNIT_CURRENT, data->ospfStubAreaId,
                                 data->ospfStubTOS) != L7_SUCCESS)
   {
     if (!VALID(I_ospfStubStatus, data->valid) ||
         snmpOspfStubStatusSet(USMDB_UNIT_CURRENT, data->ospfStubAreaId,
                               data->ospfStubTOS,
                               data->ospfStubStatus) != L7_SUCCESS)
      {
          return(COMMIT_FAILED_ERROR);
      }
      else
      {
        CLR_VALID(I_ospfStubStatus, data->valid);
      }
   }

   if (VALID(I_ospfStubMetric, data->valid) &&
       (usmDbOspfStubMetricSet(USMDB_UNIT_CURRENT, data->ospfStubAreaId,
                               data->ospfStubTOS,
                               data->ospfStubMetric) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfStubMetricType, data->valid) &&
       (snmpOspfStubMetricTypeSet(USMDB_UNIT_CURRENT, data->ospfStubAreaId,
                                  data->ospfStubTOS,
                                  data->ospfStubMetricType) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfStubStatus, data->valid) &&
       (snmpOspfStubStatusSet(USMDB_UNIT_CURRENT, data->ospfStubAreaId,
                              data->ospfStubTOS,
                              data->ospfStubStatus) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   return(NO_ERROR);
}

#ifdef SR_ospfStubAreaEntry_UNDO
/* add #define SR_ospfStubAreaEntry_UNDO in sitedefs.h to
 * include the undo routine for the ospfStubAreaEntry family.
 */
int
ospfStubAreaEntry_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfStubAreaEntry_UNDO */

#endif /* SETS */

ospfLsdbEntry_t *
k_ospfLsdbEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_UINT32 ospfLsdbAreaId,
                    SR_INT32 ospfLsdbType,
                    SR_UINT32 ospfLsdbLsid,
                    SR_UINT32 ospfLsdbRouterId)
{

   static ospfLsdbEntry_t ospfLsdbEntryData;
   L7_char8 *snmp_buffer = NULL;
   L7_uint32 snmp_buffer_len;
   static L7_BOOL firstTime = L7_TRUE;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   if (firstTime == L7_TRUE)
   {
     ospfLsdbEntryData.ospfLsdbAdvertisement = MakeOctetString(NULL, 0);

     firstTime = L7_FALSE;
   }

   ZERO_VALID(ospfLsdbEntryData.valid);

   ospfLsdbEntryData.ospfLsdbAreaId = ospfLsdbAreaId;
   ospfLsdbEntryData.ospfLsdbType = ospfLsdbType;
   ospfLsdbEntryData.ospfLsdbLsid = ospfLsdbLsid;
   ospfLsdbEntryData.ospfLsdbRouterId = ospfLsdbRouterId;

   SET_VALID(I_ospfLsdbAreaId, ospfLsdbEntryData.valid);
   SET_VALID(I_ospfLsdbType, ospfLsdbEntryData.valid);
   SET_VALID(I_ospfLsdbLsid, ospfLsdbEntryData.valid);
   SET_VALID(I_ospfLsdbRouterId, ospfLsdbEntryData.valid);

   if ((searchType == EXACT) ?
        (snmpOspfLsdbEntryGet(USMDB_UNIT_CURRENT, ospfLsdbEntryData.ospfLsdbAreaId,
                              ospfLsdbEntryData.ospfLsdbType,
                              ospfLsdbEntryData.ospfLsdbLsid,
                              ospfLsdbEntryData.ospfLsdbRouterId) != L7_SUCCESS) :
       ((snmpOspfLsdbEntryGet(USMDB_UNIT_CURRENT, ospfLsdbEntryData.ospfLsdbAreaId,
                             ospfLsdbEntryData.ospfLsdbType,
                             ospfLsdbEntryData.ospfLsdbLsid,
                             ospfLsdbEntryData.ospfLsdbRouterId) != L7_SUCCESS) &&
        (snmpOspfLsdbEntryNext(USMDB_UNIT_CURRENT, &ospfLsdbEntryData.ospfLsdbAreaId,
                               &ospfLsdbEntryData.ospfLsdbType,
                               &ospfLsdbEntryData.ospfLsdbLsid,
                               &ospfLsdbEntryData.ospfLsdbRouterId) != L7_SUCCESS)))
      return(NULL);


   switch(nominator)
   {

   case -1:
   case I_ospfLsdbAreaId:
   case I_ospfLsdbType:
   case I_ospfLsdbLsid:
   case I_ospfLsdbRouterId:
     break;
       
   case I_ospfLsdbSequence:
     if (usmDbOspfLsdbSequenceGet(USMDB_UNIT_CURRENT, ospfLsdbEntryData.ospfLsdbAreaId, 0,
                                ospfLsdbEntryData.ospfLsdbType,
                                ospfLsdbEntryData.ospfLsdbLsid,
                                ospfLsdbEntryData.ospfLsdbRouterId,
                                &ospfLsdbEntryData.ospfLsdbSequence) == L7_SUCCESS)
        SET_VALID(nominator, ospfLsdbEntryData.valid);
     break;

   case I_ospfLsdbAge:
     if (usmDbOspfLsdbAgeGet(USMDB_UNIT_CURRENT, ospfLsdbEntryData.ospfLsdbAreaId, 0,
                           ospfLsdbEntryData.ospfLsdbType,
                           ospfLsdbEntryData.ospfLsdbLsid,
                           ospfLsdbEntryData.ospfLsdbRouterId,
                           &ospfLsdbEntryData.ospfLsdbAge) == L7_SUCCESS)
        SET_VALID(nominator, ospfLsdbEntryData.valid);
     break;

   case I_ospfLsdbChecksum:
     if (usmDbOspfLsdbChecksumGet(USMDB_UNIT_CURRENT, ospfLsdbEntryData.ospfLsdbAreaId, 0,
                                ospfLsdbEntryData.ospfLsdbType,
                                ospfLsdbEntryData.ospfLsdbLsid,
                                ospfLsdbEntryData.ospfLsdbRouterId,
                                &ospfLsdbEntryData.ospfLsdbChecksum) == L7_SUCCESS)
        SET_VALID(nominator, ospfLsdbEntryData.valid);
     break;

   case I_ospfLsdbAdvertisement:
     if (usmDbOspfLsdbAdvertisementGet(USMDB_UNIT_CURRENT, ospfLsdbEntryData.ospfLsdbAreaId, 0,
                                        ospfLsdbEntryData.ospfLsdbType, ospfLsdbEntryData.ospfLsdbLsid,
                           ospfLsdbEntryData.ospfLsdbRouterId, &snmp_buffer, &snmp_buffer_len)
                           == L7_SUCCESS)
     {
         /* The SNMP Agent is not sending OCTETSTRINGS
	  * whose size > SNMP_AGENT_MAX_PKT_LEN ( = 2048 ) */
         if(snmp_buffer_len > SNMP_AGENT_MAX_PKT_LEN)
	 {
             snmp_buffer_len = SNMP_AGENT_MAX_PKT_LEN;
	 }
         if (SafeMakeOctetString(&ospfLsdbEntryData.ospfLsdbAdvertisement,
                                 snmp_buffer, snmp_buffer_len) == L7_TRUE )
         {
             SET_VALID(nominator, ospfLsdbEntryData.valid);
	 }
         osapiFree(L7_OSPF_MAP_COMPONENT_ID, snmp_buffer);
         snmp_buffer = NULL;
     }
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, ospfLsdbEntryData.valid))
      return(NULL);

   return(&ospfLsdbEntryData);
}

ospfAreaRangeEntry_t *
k_ospfAreaRangeEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_UINT32 ospfAreaRangeAreaId,
                         SR_UINT32 ospfAreaRangeNet)
{
  /* Obsolete */
  return(NULL);
}

#ifdef SETS
int
k_ospfAreaRangeEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfAreaRangeEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfAreaRangeEntry_set_defaults(doList_t *dp)
{
   ospfAreaRangeEntry_t *data = (ospfAreaRangeEntry_t *) (dp->data);

   data->ospfAreaRangeEffect = D_ospfAreaRangeEffect_advertiseMatching;
   data->ospfAreaRangeStatus = D_ospfAreaRangeStatus_active;

   /* component provides default configuration */
   ZERO_VALID(data->valid);
   return(NO_ERROR);
}

int
k_ospfAreaRangeEntry_set(ospfAreaRangeEntry_t *data,
                         ContextInfo *contextInfo, int function)
{
  /* Obsolete */
  return(COMMIT_FAILED_ERROR);
}

#ifdef SR_ospfAreaRangeEntry_UNDO
/* add #define SR_ospfAreaRangeEntry_UNDO in sitedefs.h to
 * include the undo routine for the ospfAreaRangeEntry family.
 */
int
ospfAreaRangeEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfAreaRangeEntry_UNDO */

#endif /* SETS */

ospfHostEntry_t *
k_ospfHostEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_UINT32 ospfHostIpAddress,
                    SR_INT32 ospfHostTOS)
{

   static ospfHostEntry_t ospfHostEntryData;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   ZERO_VALID(ospfHostEntryData.valid);

   ospfHostEntryData.ospfHostIpAddress = ospfHostIpAddress;
   ospfHostEntryData.ospfHostTOS = ospfHostTOS;

   SET_VALID(I_ospfHostIpAddress, ospfHostEntryData.valid);
   SET_VALID(I_ospfHostTOS, ospfHostEntryData.valid);

   if ((searchType == EXACT) ?
        (usmDbOspfHostEntryGet(USMDB_UNIT_CURRENT, ospfHostEntryData.ospfHostIpAddress,
                              ospfHostEntryData.ospfHostTOS) != L7_SUCCESS) :
        ((usmDbOspfHostEntryGet(USMDB_UNIT_CURRENT, ospfHostEntryData.ospfHostIpAddress,
                             ospfHostEntryData.ospfHostTOS)  != L7_SUCCESS) &&
        (usmDbOspfHostEntryNext(USMDB_UNIT_CURRENT, &ospfHostEntryData.ospfHostIpAddress,
                               &ospfHostEntryData.ospfHostTOS) != L7_SUCCESS)))
      return(NULL);

   switch(nominator)
   {

   case -1:
   case I_ospfHostIpAddress:
   case I_ospfHostTOS:
     break;
       
   case I_ospfHostMetric:
     if (usmDbOspfHostMetricGet(USMDB_UNIT_CURRENT, ospfHostEntryData.ospfHostIpAddress,
                              ospfHostEntryData.ospfHostTOS,
                              &ospfHostEntryData.ospfHostMetric) == L7_SUCCESS)
        SET_VALID(nominator, ospfHostEntryData.valid);
     break;

   case I_ospfHostStatus:
     if (snmpOspfHostStatusGet(USMDB_UNIT_CURRENT, ospfHostEntryData.ospfHostIpAddress,
                               ospfHostEntryData.ospfHostTOS,
                               &ospfHostEntryData.ospfHostStatus) == L7_SUCCESS)
        SET_VALID(nominator, ospfHostEntryData.valid);
     break;

   case I_ospfHostAreaID:
     if (usmDbOspfHostAreaIDGet(USMDB_UNIT_CURRENT, ospfHostEntryData.ospfHostIpAddress,
                              ospfHostEntryData.ospfHostTOS,
                              &ospfHostEntryData.ospfHostAreaID) == L7_SUCCESS)
        SET_VALID(nominator, ospfHostEntryData.valid);
     break;  
   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */
   
   if (nominator >= 0 && !VALID(nominator, ospfHostEntryData.valid))
      return(NULL);

   return(&ospfHostEntryData);
}

 
#ifdef SETS
int
k_ospfHostEntry_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfHostEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                      doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfHostEntry_set_defaults(doList_t *dp)
{
   ospfHostEntry_t *data = (ospfHostEntry_t *) (dp->data);
   data->ospfHostStatus = D_ospfHostStatus_active;

   /* component provides default configuration */
   ZERO_VALID(data->valid);
   return(NO_ERROR);
}

int
k_ospfHostEntry_set(ospfHostEntry_t *data,
                    ContextInfo *contextInfo, int function)
{
   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);
    
   if (usmDbOspfHostEntryGet(USMDB_UNIT_CURRENT, data->ospfHostIpAddress,
                             data->ospfHostTOS) != L7_SUCCESS)
   {
      
     if (!VALID(I_ospfHostStatus, data->valid) ||
         data->ospfHostStatus != D_ospfHostStatus_createAndWait ||
         snmpOspfHostStatusSet(USMDB_UNIT_CURRENT, data->ospfHostIpAddress,
                               data->ospfHostTOS,
                               data->ospfHostStatus) != L7_SUCCESS)
     {
       return(COMMIT_FAILED_ERROR);
     }
     else
     {
       CLR_VALID(I_ospfHostStatus, data->valid);
     }
   }

   if (VALID(I_ospfHostMetric, data->valid) &&
       (usmDbOspfHostMetricSet(USMDB_UNIT_CURRENT, data->ospfHostIpAddress,
                               data->ospfHostTOS,
                               data->ospfHostMetric) != L7_SUCCESS))
   {
      CLR_VALID(I_ospfHostMetric, data->valid);
      return(COMMIT_FAILED_ERROR);
   }

   if (VALID(I_ospfHostStatus, data->valid) &&
       snmpOspfHostStatusSet(USMDB_UNIT_CURRENT, data->ospfHostIpAddress,
                              data->ospfHostTOS,
                              data->ospfHostStatus) != L7_SUCCESS)
   {
      CLR_VALID(I_ospfHostMetric, data->valid);
      return(COMMIT_FAILED_ERROR);
   }

   return(NO_ERROR);
}

#ifdef SR_ospfHostEntry_UNDO
/* add #define SR_ospfHostEntry_UNDO in sitedefs.h to
 * include the undo routine for the ospfHostEntry family.
 */
int
ospfHostEntry_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfHostEntry_UNDO */

#endif /* SETS */

ospfIfEntry_t *
k_ospfIfEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_UINT32 ospfIfIpAddress,
                  SR_INT32 ospfAddressLessIf)
{

   static ospfIfEntry_t ospfIfEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   L7_uint32 intIfNum;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   if (firstTime == L7_TRUE)
   {
     ospfIfEntryData.ospfIfAuthKey = MakeOctetString(NULL, 0);

     firstTime = L7_FALSE;
   }
/* lvl7_@p1079 start */
   ZERO_VALID(ospfIfEntryData.valid);
/* lvl7_@p1079 end */

   ospfIfEntryData.ospfIfIpAddress = ospfIfIpAddress;
   ospfIfEntryData.ospfAddressLessIf = ospfAddressLessIf;
   SET_VALID(I_ospfIfIpAddress, ospfIfEntryData.valid);
   SET_VALID(I_ospfAddressLessIf, ospfIfEntryData.valid);

/* lvl7_@p1244 start */
   if (((searchType == EXACT) ?
        (snmpOspfIfEntryGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                            ospfIfEntryData.ospfAddressLessIf) != L7_SUCCESS) :
        ((snmpOspfIfEntryGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                             ospfIfEntryData.ospfAddressLessIf) != L7_SUCCESS) &&
         (snmpOspfIfEntryNextGet(USMDB_UNIT_CURRENT, &ospfIfEntryData.ospfIfIpAddress,
                                 &ospfIfEntryData.ospfAddressLessIf) != L7_SUCCESS))) ||
       (usmDbIntIfNumFromExtIfNum(ospfIfEntryData.ospfAddressLessIf, &intIfNum) != L7_SUCCESS))
/* lvl7_@p1244 end */
      return(NULL);

   switch(nominator)
   {
   case -1:
   case I_ospfIfIpAddress:
   case I_ospfAddressLessIf:
     break;

/* lvl7_@p1244 start */
   case I_ospfIfAreaId:
     if (usmDbOspfIfAreaIdGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                            intIfNum, &ospfIfEntryData.ospfIfAreaId) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfType:
     if (snmpOspfIfTypeGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                          intIfNum, &ospfIfEntryData.ospfIfType) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfAdminStat:
     if (snmpOspfIfAdminStatGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                               intIfNum, &ospfIfEntryData.ospfIfAdminStat) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfRtrPriority:
     if (usmDbOspfIfRtrPriorityGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                                 intIfNum, &ospfIfEntryData.ospfIfRtrPriority) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfTransitDelay:
     if (usmDbOspfIfTransitDelayGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                                  intIfNum, &ospfIfEntryData.ospfIfTransitDelay) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfRetransInterval:
     if (usmDbOspfIfRetransIntervalGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                                intIfNum, &ospfIfEntryData.ospfIfRetransInterval) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfHelloInterval:
     if (usmDbOspfIfHelloIntervalGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                                intIfNum, &ospfIfEntryData.ospfIfHelloInterval) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfRtrDeadInterval:
     if (usmDbOspfIfRtrDeadIntervalGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                                intIfNum, &ospfIfEntryData.ospfIfRtrDeadInterval) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfPollInterval:
#ifdef NOT_SUPPORTED
     if (usmDbOspfIfPollIntervalGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                               intIfNum, &ospfIfEntryData.ospfIfPollInterval) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
#endif
     break;

   case I_ospfIfState:
     if (snmpOspfIfStateGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                         intIfNum, &ospfIfEntryData.ospfIfState) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfDesignatedRouter:
     if (usmDbOspfIfDesignatedRouterGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                              intIfNum, &ospfIfEntryData.ospfIfDesignatedRouter) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfBackupDesignatedRouter:
     if (usmDbOspfIfBackupDesignatedRouterGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                         intIfNum, &ospfIfEntryData.ospfIfBackupDesignatedRouter) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfEvents:
     if (usmDbOspfIfEventsGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                            intIfNum, &ospfIfEntryData.ospfIfEvents) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfAuthKey:
     /* this value should always be a null string, so just use the default value */
     SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_ospfIfStatus:
/* lvl7_@p1174 start */
     if (snmpOspfIfStatusGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                            intIfNum, &ospfIfEntryData.ospfIfStatus) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
/* lvl7_@p1174 end */
     break;

   case I_ospfIfMulticastForwarding:
     if (snmpOspfIfMulticastForwardingGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                          intIfNum, &ospfIfEntryData.ospfIfMulticastForwarding) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;
/* lvl7_@p1081 start */
   case I_ospfIfDemand:
     if (snmpOspfIfDemandGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                            intIfNum, &ospfIfEntryData.ospfIfDemand) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;
/* lvl7_@p1081 end */

   case I_ospfIfAuthType:
     if (snmpOspfIfAuthTypeGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                              intIfNum, &ospfIfEntryData.ospfIfAuthType) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_agentOspfIfAuthKeyId:
     if (usmDbOspfIfAuthKeyIdGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                                 intIfNum, &ospfIfEntryData.agentOspfIfAuthKeyId) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

   case I_agentOspfIfIpMtuIgnoreFlag:
     if (snmpAgentOspfIfIpMtuIgnoreFlagGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                                           intIfNum, &ospfIfEntryData.agentOspfIfIpMtuIgnoreFlag) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfEntryData.valid);
     break;

  case I_agentOspfIfPassiveMode:
    if (snmpAgentOspfIfPassiveModeGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                                      intIfNum, &ospfIfEntryData.agentOspfIfPassiveMode) == L7_SUCCESS)
    SET_VALID(nominator, ospfIfEntryData.valid);
    break;
  case I_agentOspfIfAdvertiseSecondaries:
    if (snmpAgentOspfIfAdvertiseSecondariesGet(USMDB_UNIT_CURRENT, ospfIfEntryData.ospfIfIpAddress,
                                      intIfNum, &ospfIfEntryData.agentOspfIfAdvertiseSecondaries) == L7_SUCCESS)
    SET_VALID(nominator, ospfIfEntryData.valid);
    break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
/* lvl7_@p1244 end */
/* lvl7_@p1079 end */
   }  /* end of switch(nominator)  */
   
   if (nominator >= 0 && !VALID(nominator, ospfIfEntryData.valid))
     return(NULL);

   return(&ospfIfEntryData);
}

#ifdef SETS
int
k_ospfIfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                   doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfIfEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                    doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfIfEntry_set_defaults(doList_t *dp)
{
   ospfIfEntry_t *data = (ospfIfEntry_t *) (dp->data);

   data->ospfIfAdminStat = D_ospfIfAdminStat_enabled;
   data->ospfIfState = D_ospfIfState_down;
   data->ospfIfEvents = (SR_UINT32) 0;
   if ((data->ospfIfAuthKey = MakeOctetStringFromText("")) == 0)
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   data->ospfIfMulticastForwarding = D_ospfIfMulticastForwarding_blocked;
   data->ospfIfDemand = D_ospfIfDemand_false;
   data->ospfIfAuthType = 0;
   data->agentOspfIfAuthKeyId = 0;
   data->ospfIfStatus = D_ospfIfStatus_active;

   /* component provides default configuration */
   ZERO_VALID(data->valid);
   return(NO_ERROR);
}

int
k_ospfIfEntry_set(ospfIfEntry_t *data,
                  ContextInfo *contextInfo, int function)
{
   char snmp_buffer[SNMP_BUFFER_LEN];
   L7_uint32 intIfNum;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);

   if (usmDbIntIfNumFromExtIfNum(data->ospfAddressLessIf, &intIfNum) != L7_SUCCESS)
     return(COMMIT_FAILED_ERROR);

   if (snmpOspfIfEntryGet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                          data->ospfAddressLessIf) != L7_SUCCESS)
   {
     if (!VALID(I_ospfIfStatus, data->valid) ||
         snmpOspfIfStatusSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                             intIfNum,
                             data->ospfIfStatus) != L7_SUCCESS)
     {
          return(COMMIT_FAILED_ERROR);
     }
     else
     {
       CLR_VALID(I_ospfIfStatus, data->valid);
     }
   }

   if (VALID(I_ospfIfAreaId, data->valid) &&
       (usmDbOspfIfAreaIdSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                             intIfNum,
                             data->ospfIfAreaId) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfIfType, data->valid) &&
       (snmpOspfIfTypeSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                          intIfNum,
                          data->ospfIfType) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfIfAdminStat, data->valid) &&
       (snmpOspfIfAdminStatSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                               intIfNum,
                               data->ospfIfAdminStat) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfIfRtrPriority, data->valid) &&
       (usmDbOspfIfRtrPrioritySet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                  intIfNum,
                                  data->ospfIfRtrPriority) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfIfTransitDelay, data->valid) &&
       (usmDbOspfIfTransitDelaySet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                   intIfNum,
                                   data->ospfIfTransitDelay) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfIfRetransInterval, data->valid) &&
       (usmDbOspfIfRetransIntervalSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                      intIfNum,
                                      data->ospfIfRetransInterval) != L7_SUCCESS))
     return(COMMIT_FAILED_ERROR);
   
   if (VALID(I_ospfIfHelloInterval, data->valid) &&
       (usmDbOspfIfHelloIntervalSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                    intIfNum,
                                    data->ospfIfHelloInterval) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfIfRtrDeadInterval, data->valid) &&
       (usmDbOspfIfRtrDeadIntervalSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                      intIfNum,
                                      data->ospfIfRtrDeadInterval) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

#ifdef NOT_SUPPORTED
   if (VALID(I_ospfIfPollInterval, data->valid) &&
       (usmDbOspfIfPollIntervalSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                   intIfNum,
                                   data->ospfIfPollInterval) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);
#else
   if (VALID(I_ospfIfPollInterval, data->valid))
       return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

   if (data->ospfIfAuthKey != NULL)
   {
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     memcpy(snmp_buffer, data->ospfIfAuthKey->octet_ptr, data->ospfIfAuthKey->length);
     if (VALID(I_ospfIfAuthKey, data->valid) &&
         (usmDbOspfIfAuthKeySet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                intIfNum,
                                snmp_buffer) != L7_SUCCESS))
       return(COMMIT_FAILED_ERROR);
   }            
 
/* lvl7_@p1174 start */
   if (VALID(I_ospfIfStatus, data->valid) &&
       (snmpOspfIfStatusSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                            intIfNum,
                            data->ospfIfStatus) != L7_SUCCESS))
/* lvl7_@p1174 end */
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfIfMulticastForwarding, data->valid))
      return(COMMIT_FAILED_ERROR);

/* lvl7_@p1081 start */
#ifdef NOT_SUPPORTED
   if (VALID(I_ospfIfDemand, data->valid) &&
       (snmpOspfIfDemandSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                            intIfNum,
                            data->ospfIfDemand) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);
#else
   if (VALID(I_ospfIfDemand, data->valid))
      return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */
/* lvl7_@p1081 end */

/* lvl7_@p1079 start */
   if (VALID(I_ospfIfAuthType, data->valid) &&
       (snmpOspfIfAuthTypeSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                              intIfNum,
                              data->ospfIfAuthType) != L7_SUCCESS))
/* lvl7_@p1079 end */
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_agentOspfIfAuthKeyId, data->valid) &&
       (usmDbOspfIfAuthKeyIdSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                intIfNum,
                                data->agentOspfIfAuthKeyId) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_agentOspfIfPassiveMode, data->valid) &&
       snmpAgentOspfIfPassiveModeSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                     intIfNum,
                                     data->agentOspfIfPassiveMode) != L7_SUCCESS)
   {
     CLR_VALID(I_agentOspfIfPassiveMode, data->valid);
     return COMMIT_FAILED_ERROR;
   }
   if (VALID(I_agentOspfIfAdvertiseSecondaries, data->valid) &&
       snmpAgentOspfIffAdvertiseSecondariesSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
                                     intIfNum,
                                     data->agentOspfIfAdvertiseSecondaries) != L7_SUCCESS)
   {
     CLR_VALID(I_agentOspfIfAdvertiseSecondaries, data->valid);
     return COMMIT_FAILED_ERROR;
   }

   if (VALID(I_agentOspfIfIpMtuIgnoreFlag, data->valid) &&
      (snmpAgentOspfIfIpMtuIgnoreFlagSet(USMDB_UNIT_CURRENT, data->ospfIfIpAddress,
       intIfNum,
       data->agentOspfIfIpMtuIgnoreFlag) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   return(NO_ERROR);
}

#ifdef SR_ospfIfEntry_UNDO
/* add #define SR_ospfIfEntry_UNDO in sitedefs.h to
 * include the undo routine for the ospfIfEntry family.
 */
int
ospfIfEntry_undo(doList_t *doHead, doList_t *doCur,
                 ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfIfEntry_UNDO */

#endif /* SETS */

ospfIfMetricEntry_t *
k_ospfIfMetricEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_UINT32 ospfIfMetricIpAddress,
                        SR_INT32 ospfIfMetricAddressLessIf,
                        SR_INT32 ospfIfMetricTOS)
{

   static ospfIfMetricEntry_t ospfIfMetricEntryData;
   L7_uint32 intIfNum;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

/* lvl7_@p1265 start */
   ZERO_VALID(ospfIfMetricEntryData.valid);
/* lvl7_@p1265 end */

   ospfIfMetricEntryData.ospfIfMetricIpAddress = ospfIfMetricIpAddress;
   ospfIfMetricEntryData.ospfIfMetricAddressLessIf = ospfIfMetricAddressLessIf;
   ospfIfMetricEntryData.ospfIfMetricTOS = ospfIfMetricTOS;
   SET_VALID(I_ospfIfMetricIpAddress, ospfIfMetricEntryData.valid);
   SET_VALID(I_ospfIfMetricAddressLessIf, ospfIfMetricEntryData.valid);
   SET_VALID(I_ospfIfMetricTOS, ospfIfMetricEntryData.valid);

   if (((searchType == EXACT) ?
        (snmpOspfIfMetricEntryGet(USMDB_UNIT_CURRENT, ospfIfMetricEntryData.ospfIfMetricIpAddress,
                                  ospfIfMetricEntryData.ospfIfMetricAddressLessIf,
                                  ospfIfMetricEntryData.ospfIfMetricTOS) != L7_SUCCESS) :
        (snmpOspfIfMetricEntryGet(USMDB_UNIT_CURRENT, ospfIfMetricEntryData.ospfIfMetricIpAddress,
                                  ospfIfMetricEntryData.ospfIfMetricAddressLessIf,
                                  ospfIfMetricEntryData.ospfIfMetricTOS) != L7_SUCCESS &&
         snmpOspfIfMetricEntryNextGet(USMDB_UNIT_CURRENT, &ospfIfMetricEntryData.ospfIfMetricIpAddress,
                                      &ospfIfMetricEntryData.ospfIfMetricAddressLessIf,
                                      &ospfIfMetricEntryData.ospfIfMetricTOS) != L7_SUCCESS)) ||
        usmDbIntIfNumFromExtIfNum(ospfIfMetricEntryData.ospfIfMetricAddressLessIf, &intIfNum))
     return(NULL);

/* lvl7_@p1265 start */
   switch(nominator)
   {
   case -1:
   case I_ospfIfMetricIpAddress:
   case I_ospfIfMetricAddressLessIf:
   case I_ospfIfMetricTOS:
     break;

   case I_ospfIfMetricValue:
     if (usmDbOspfIfMetricValueGet(USMDB_UNIT_CURRENT, intIfNum, ospfIfMetricEntryData.ospfIfMetricTOS,
                                   &ospfIfMetricEntryData.ospfIfMetricValue) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfMetricEntryData.valid);
     break;

   case I_ospfIfMetricStatus:
     if (snmpOspfIfMetricStatusGet(USMDB_UNIT_CURRENT, ospfIfMetricEntryData.ospfIfMetricIpAddress,
                                   intIfNum, ospfIfMetricEntryData.ospfIfMetricTOS,
                                   &ospfIfMetricEntryData.ospfIfMetricStatus) == L7_SUCCESS)
        SET_VALID(nominator, ospfIfMetricEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */
/* lvl7_@p1265 end */

   if (nominator >= 0 && !VALID(nominator, ospfIfMetricEntryData.valid))
      return(NULL);

   return(&ospfIfMetricEntryData);
}

#ifdef SETS
int
k_ospfIfMetricEntry_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfIfMetricEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfIfMetricEntry_set_defaults(doList_t *dp)
{
   ospfIfMetricEntry_t *data = (ospfIfMetricEntry_t *) (dp->data);

   data->ospfIfMetricStatus = D_ospfIfMetricStatus_active;

   /* component provides default configuration */
   ZERO_VALID(data->valid);
   return(NO_ERROR);
}

int
k_ospfIfMetricEntry_set(ospfIfMetricEntry_t *data,
                        ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);
    
  if (usmDbIntIfNumFromExtIfNum(data->ospfIfMetricAddressLessIf, &intIfNum) != L7_SUCCESS)
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_ospfIfMetricValue, data->valid) &&
       (usmDbOspfIfMetricValueSet(USMDB_UNIT_CURRENT, data->ospfIfMetricIpAddress,
                                  intIfNum,
                                  data->ospfIfMetricTOS,
                                  data->ospfIfMetricValue) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

  if (VALID(I_ospfIfMetricStatus, data->valid))
     return(COMMIT_FAILED_ERROR);

  return(NO_ERROR); 
}

#ifdef SR_ospfIfMetricEntry_UNDO
/* add #define SR_ospfIfMetricEntry_UNDO in sitedefs.h to
 * include the undo routine for the ospfIfMetricEntry family.
 */
int
ospfIfMetricEntry_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfIfMetricEntry_UNDO */

#endif /* SETS */

ospfVirtIfEntry_t *
k_ospfVirtIfEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_UINT32 ospfVirtIfAreaId,
                      SR_UINT32 ospfVirtIfNeighbor)
{

   static ospfVirtIfEntry_t ospfVirtIfEntryData;
   char snmp_buffer[SNMP_BUFFER_LEN];
   static L7_BOOL firstTime = L7_TRUE;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   if (firstTime == L7_TRUE)
   {
     ospfVirtIfEntryData.ospfVirtIfAuthKey = MakeOctetString(NULL, 0);

     firstTime = L7_FALSE;
   }
   
   ZERO_VALID(ospfVirtIfEntryData.valid);

   ospfVirtIfEntryData.ospfVirtIfAreaId = ospfVirtIfAreaId;
   ospfVirtIfEntryData.ospfVirtIfNeighbor = ospfVirtIfNeighbor;

   SET_VALID(I_ospfVirtIfAreaId, ospfVirtIfEntryData.valid);
   SET_VALID(I_ospfVirtIfNeighbor, ospfVirtIfEntryData.valid);

   if ((searchType == EXACT) ?
        (usmDbOspfVirtIfEntryGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                ospfVirtIfEntryData.ospfVirtIfNeighbor) != L7_SUCCESS) :
        ((usmDbOspfVirtIfEntryGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                               ospfVirtIfEntryData.ospfVirtIfNeighbor) != L7_SUCCESS) &&
        (usmDbOspfVirtIfEntryNext(USMDB_UNIT_CURRENT, &ospfVirtIfEntryData.ospfVirtIfAreaId,
                                 &ospfVirtIfEntryData.ospfVirtIfNeighbor) != L7_SUCCESS)))
      return(NULL);

   switch(nominator)
   {
   case -1:
   case I_ospfVirtIfAreaId:
   case I_ospfVirtIfNeighbor:
     break;
       
   case I_ospfVirtIfTransitDelay:
     if (usmDbOspfVirtIfTransitDelayGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                      ospfVirtIfEntryData.ospfVirtIfNeighbor,
                                      &ospfVirtIfEntryData.ospfVirtIfTransitDelay) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   case I_ospfVirtIfRetransInterval:
     if (usmDbOspfVirtIfRetransIntervalGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                         ospfVirtIfEntryData.ospfVirtIfNeighbor,
                                         &ospfVirtIfEntryData.ospfVirtIfRetransInterval) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   case I_ospfVirtIfHelloInterval:
     if (usmDbOspfVirtIfHelloIntervalGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                       ospfVirtIfEntryData.ospfVirtIfNeighbor,
                                       &ospfVirtIfEntryData.ospfVirtIfHelloInterval) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   case I_ospfVirtIfRtrDeadInterval:
     if (usmDbOspfVirtIfRtrDeadIntervalGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                         ospfVirtIfEntryData.ospfVirtIfNeighbor,
                                         &ospfVirtIfEntryData.ospfVirtIfRtrDeadInterval) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   case I_ospfVirtIfState:
     if (snmpOspfVirtIfStateGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                               ospfVirtIfEntryData.ospfVirtIfNeighbor,
                               &ospfVirtIfEntryData.ospfVirtIfState) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   case I_ospfVirtIfEvents:
     if (usmDbOspfVirtIfEventsGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                ospfVirtIfEntryData.ospfVirtIfNeighbor,
                                &ospfVirtIfEntryData.ospfVirtIfEvents) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   case I_ospfVirtIfAuthKey:
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     if ((usmDbOspfVirtIfAuthKeyGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                 ospfVirtIfEntryData.ospfVirtIfNeighbor, snmp_buffer) == L7_SUCCESS) &&
         (SafeMakeOctetStringFromText(&ospfVirtIfEntryData.ospfVirtIfAuthKey, snmp_buffer) == L7_TRUE))
             SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   case I_ospfVirtIfStatus:
     if (snmpOspfVirtIfStatusGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                 ospfVirtIfEntryData.ospfVirtIfNeighbor,
                                 &ospfVirtIfEntryData.ospfVirtIfStatus) == L7_SUCCESS)
       SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   case I_ospfVirtIfAuthType:
     if (usmDbOspfVirtIfAuthTypeGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                  ospfVirtIfEntryData.ospfVirtIfNeighbor,
                                  &ospfVirtIfEntryData.ospfVirtIfAuthType) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   case I_agentOspfVirtIfAuthKeyId:
     if (usmDbOspfVirtIfAuthKeyIdGet(USMDB_UNIT_CURRENT, ospfVirtIfEntryData.ospfVirtIfAreaId,
                                     ospfVirtIfEntryData.ospfVirtIfNeighbor,
                                     &ospfVirtIfEntryData.agentOspfVirtIfAuthKeyId) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtIfEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, ospfVirtIfEntryData.valid))
      return(NULL);

   return(&ospfVirtIfEntryData);
}

#ifdef SETS
int
k_ospfVirtIfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfVirtIfEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfVirtIfEntry_set_defaults(doList_t *dp)
{
   ospfVirtIfEntry_t *data = (ospfVirtIfEntry_t *) (dp->data);

   data->ospfVirtIfState = D_ospfVirtIfState_down;
   data->ospfVirtIfEvents = (SR_UINT32) 0;
   if ((data->ospfVirtIfAuthKey = MakeOctetStringFromText("")) == 0)
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   data->ospfVirtIfAuthType = 0;
   data->agentOspfVirtIfAuthKeyId = 0;
   data->ospfVirtIfStatus = D_ospfVirtIfStatus_active;

   /* component provides default configuration */
   ZERO_VALID(data->valid);
   return(NO_ERROR);
}

int
k_ospfVirtIfEntry_set(ospfVirtIfEntry_t *data,
                      ContextInfo *contextInfo, int function)
{
   char snmp_buffer[SNMP_BUFFER_LEN];

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);

   if (usmDbOspfVirtIfEntryGet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                               data->ospfVirtIfNeighbor) != L7_SUCCESS)
   {
     if (!VALID(I_ospfVirtIfStatus, data->valid) ||
         snmpOspfVirtIfStatusSet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                                 data->ospfVirtIfNeighbor,
                                 data->ospfVirtIfStatus) != L7_SUCCESS)
     {
       return(COMMIT_FAILED_ERROR);
     }
     else
     {
       CLR_VALID(I_ospfVirtIfStatus, data->valid);
     }
   }

   if (VALID(I_ospfVirtIfTransitDelay, data->valid) &&
       (usmDbOspfVirtIfTransitDelaySet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                                       data->ospfVirtIfNeighbor,
                                       data->ospfVirtIfTransitDelay) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfVirtIfRetransInterval, data->valid) &&
       (usmDbOspfVirtIfRetransIntervalSet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                                          data->ospfVirtIfNeighbor,
                                          data->ospfVirtIfRetransInterval) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfVirtIfHelloInterval, data->valid) &&
       (usmDbOspfVirtIfHelloIntervalSet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                                        data->ospfVirtIfNeighbor,
                                        data->ospfVirtIfHelloInterval) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfVirtIfRtrDeadInterval, data->valid) &&
       (usmDbOspfVirtIfRtrDeadIntervalSet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                                          data->ospfVirtIfNeighbor,
                                          data->ospfVirtIfRtrDeadInterval) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (data->ospfVirtIfAuthKey != NULL)
   {
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     memcpy(snmp_buffer, data->ospfVirtIfAuthKey->octet_ptr, data->ospfVirtIfAuthKey->length);
     if (VALID(I_ospfVirtIfAuthKey, data->valid) &&
         (usmDbOspfVirtIfAuthKeySet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                                    data->ospfVirtIfNeighbor,
                                    snmp_buffer) != L7_SUCCESS))
       return(COMMIT_FAILED_ERROR);
   }

   if (VALID(I_ospfVirtIfStatus, data->valid) &&
       (snmpOspfVirtIfStatusSet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                                data->ospfVirtIfNeighbor,
                                data->ospfVirtIfStatus) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfVirtIfAuthType, data->valid) &&
       (usmDbOspfVirtIfAuthTypeSet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                                   data->ospfVirtIfNeighbor,
                                   data->ospfVirtIfAuthType) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_agentOspfVirtIfAuthKeyId, data->valid) &&
       (usmDbOspfVirtIfAuthKeyIdSet(USMDB_UNIT_CURRENT, data->ospfVirtIfAreaId,
                                    data->ospfVirtIfNeighbor,
                                    data->agentOspfVirtIfAuthKeyId) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   return(NO_ERROR);
}

#ifdef SR_ospfVirtIfEntry_UNDO
/* add #define SR_ospfVirtIfEntry_UNDO in sitedefs.h to
 * include the undo routine for the ospfVirtIfEntry family.
 */
int
ospfVirtIfEntry_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfVirtIfEntry_UNDO */

#endif /* SETS */

ospfNbrEntry_t *
k_ospfNbrEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_UINT32 ospfNbrIpAddr,
                   SR_INT32 ospfNbrAddressLessIndex)
{

   static ospfNbrEntry_t ospfNbrEntryData;
   L7_uint32 intIfNum;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   ZERO_VALID(ospfNbrEntryData.valid);

   ospfNbrEntryData.ospfNbrIpAddr = ospfNbrIpAddr;
   ospfNbrEntryData.ospfNbrAddressLessIndex = ospfNbrAddressLessIndex;

   SET_VALID(I_ospfNbrIpAddr, ospfNbrEntryData.valid);
   SET_VALID(I_ospfNbrAddressLessIndex, ospfNbrEntryData.valid);

   if (((searchType == EXACT) ?
        (snmpOspfNbrEntryGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                             ospfNbrEntryData.ospfNbrAddressLessIndex) != L7_SUCCESS) :
        (snmpOspfNbrEntryGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                             ospfNbrEntryData.ospfNbrAddressLessIndex) != L7_SUCCESS &&
         snmpOspfNbrEntryNextGet(USMDB_UNIT_CURRENT, &ospfNbrEntryData.ospfNbrIpAddr,
                                 &ospfNbrEntryData.ospfNbrAddressLessIndex) != L7_SUCCESS)) ||
       usmDbIntIfNumFromExtIfNum(ospfNbrEntryData.ospfNbrAddressLessIndex, &intIfNum))
     return(NULL);
   
   switch(nominator)
   {
   case -1:
   case I_ospfNbrIpAddr:
   case I_ospfNbrAddressLessIndex:
     break;
       
   case I_ospfNbrRtrId:
     if (usmDbOspfNbrRtrIdGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                            intIfNum, &ospfNbrEntryData.ospfNbrRtrId) == L7_SUCCESS)
        SET_VALID(nominator, ospfNbrEntryData.valid);
     break;

   case I_ospfNbrOptions:
     if (usmDbOspfNbrOptionsGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                              intIfNum, &ospfNbrEntryData.ospfNbrOptions) == L7_SUCCESS)
        SET_VALID(nominator, ospfNbrEntryData.valid);
     break;

   case I_ospfNbrPriority:
     if (usmDbOspfNbrPriorityGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                               intIfNum, &ospfNbrEntryData.ospfNbrPriority) == L7_SUCCESS)
        SET_VALID(nominator, ospfNbrEntryData.valid);
     break;

   case I_ospfNbrState:
     if (snmpOspfNbrStateGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                            intIfNum, &ospfNbrEntryData.ospfNbrState) == L7_SUCCESS)
        SET_VALID(nominator, ospfNbrEntryData.valid);
     break;

   case I_ospfNbrEvents:
     if (usmDbOspfNbrEventsGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                             intIfNum, &ospfNbrEntryData.ospfNbrEvents) == L7_SUCCESS)
        SET_VALID(nominator, ospfNbrEntryData.valid);
     break;

   case I_ospfNbrLsRetransQLen:
     if (usmDbOspfNbrLsRetransQLenGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                              intIfNum, &ospfNbrEntryData.ospfNbrLsRetransQLen) == L7_SUCCESS)
        SET_VALID(nominator, ospfNbrEntryData.valid);
     break;

   case I_ospfNbmaNbrStatus:
     if (snmpOspfNbmaNbrStatusGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                                  intIfNum, &ospfNbrEntryData.ospfNbmaNbrStatus) == L7_SUCCESS)
        SET_VALID(nominator, ospfNbrEntryData.valid);
     break;

   case I_ospfNbmaNbrPermanence:
     if (snmpOspfNbmaNbrPermanenceGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                              intIfNum, &ospfNbrEntryData.ospfNbmaNbrPermanence) == L7_SUCCESS)
        SET_VALID(nominator, ospfNbrEntryData.valid);
     break;

   case I_ospfNbrHelloSuppressed:
     if (snmpOspfNbrHelloSuppressedGet(USMDB_UNIT_CURRENT, ospfNbrEntryData.ospfNbrIpAddr,
                             intIfNum, &ospfNbrEntryData.ospfNbrHelloSuppressed) == L7_SUCCESS)
        SET_VALID(nominator, ospfNbrEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, ospfNbrEntryData.valid))
      return(NULL);

   return(&ospfNbrEntryData);
}

#ifdef SETS
int
k_ospfNbrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfNbrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfNbrEntry_set_defaults(doList_t *dp)
{
   ospfNbrEntry_t *data = (ospfNbrEntry_t *) (dp->data);

   data->ospfNbrOptions = 0;
   data->ospfNbrState = D_ospfNbrState_down;
   data->ospfNbrEvents = (SR_UINT32) 0;
   data->ospfNbmaNbrPermanence = D_ospfNbmaNbrPermanence_permanent;
   data->ospfNbmaNbrStatus = D_ospfNbmaNbrStatus_active;

   /* component provides default configuration */
   ZERO_VALID(data->valid);
   return(NO_ERROR);
}

int
k_ospfNbrEntry_set(ospfNbrEntry_t *data,
                   ContextInfo *contextInfo, int function)
{
	/* all the entries are read-only, hence return error in such a case */
    return(COMMIT_FAILED_ERROR);
}

#ifdef SR_ospfNbrEntry_UNDO
/* add #define SR_ospfNbrEntry_UNDO in sitedefs.h to
 * include the undo routine for the ospfNbrEntry family.
 */
int
ospfNbrEntry_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfNbrEntry_UNDO */

#endif /* SETS */

ospfVirtNbrEntry_t *
k_ospfVirtNbrEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_UINT32 ospfVirtNbrArea,
                       SR_UINT32 ospfVirtNbrRtrId)
{

   static ospfVirtNbrEntry_t ospfVirtNbrEntryData;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   ZERO_VALID(ospfVirtNbrEntryData.valid);

   ospfVirtNbrEntryData.ospfVirtNbrArea = ospfVirtNbrArea;
   ospfVirtNbrEntryData.ospfVirtNbrRtrId = ospfVirtNbrRtrId;

   SET_VALID(I_ospfVirtNbrArea, ospfVirtNbrEntryData.valid);
   SET_VALID(I_ospfVirtNbrRtrId, ospfVirtNbrEntryData.valid);

   if ((searchType == EXACT) ?
        (usmDbOspfVirtNbrEntryGet(USMDB_UNIT_CURRENT, ospfVirtNbrEntryData.ospfVirtNbrArea,
                                 ospfVirtNbrEntryData.ospfVirtNbrRtrId) != L7_SUCCESS) :
        ((usmDbOspfVirtNbrEntryGet(USMDB_UNIT_CURRENT, ospfVirtNbrEntryData.ospfVirtNbrArea,
                                ospfVirtNbrEntryData.ospfVirtNbrRtrId) != L7_SUCCESS) &&
        (usmDbOspfVirtNbrEntryNext(USMDB_UNIT_CURRENT, &ospfVirtNbrEntryData.ospfVirtNbrArea,
                                  &ospfVirtNbrEntryData.ospfVirtNbrRtrId) != L7_SUCCESS)))
      return(NULL);

   switch(nominator)
   {
   case -1:
   case I_ospfVirtNbrArea:
   case I_ospfVirtNbrRtrId:
     break;
       
   case I_ospfVirtNbrIpAddr:
     if (usmDbOspfVirtNbrIpAddrGet(USMDB_UNIT_CURRENT, ospfVirtNbrEntryData.ospfVirtNbrArea,
                                 ospfVirtNbrEntryData.ospfVirtNbrRtrId,
                                 &ospfVirtNbrEntryData.ospfVirtNbrIpAddr) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtNbrEntryData.valid);
     break;

   case I_ospfVirtNbrOptions:
     if (usmDbOspfVirtNbrOptionsGet(USMDB_UNIT_CURRENT, ospfVirtNbrEntryData.ospfVirtNbrArea,
                                  ospfVirtNbrEntryData.ospfVirtNbrRtrId,
                                  &ospfVirtNbrEntryData.ospfVirtNbrOptions) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtNbrEntryData.valid);
     break;

   case I_ospfVirtNbrState:
     if (snmpOspfVirtNbrStateGet(USMDB_UNIT_CURRENT, ospfVirtNbrEntryData.ospfVirtNbrArea,
                                ospfVirtNbrEntryData.ospfVirtNbrRtrId,
                                &ospfVirtNbrEntryData.ospfVirtNbrState) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtNbrEntryData.valid);
     break;

   case I_ospfVirtNbrEvents:
     if (usmDbOspfVirtNbrEventsGet(USMDB_UNIT_CURRENT, ospfVirtNbrEntryData.ospfVirtNbrArea,
                                 ospfVirtNbrEntryData.ospfVirtNbrRtrId,
                                 &ospfVirtNbrEntryData.ospfVirtNbrEvents) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtNbrEntryData.valid);
     break;

   case I_ospfVirtNbrLsRetransQLen:
     if (usmDbOspfVirtNbrLsRetransQLenGet(USMDB_UNIT_CURRENT, ospfVirtNbrEntryData.ospfVirtNbrArea,
                                        ospfVirtNbrEntryData.ospfVirtNbrRtrId,
                                        &ospfVirtNbrEntryData.ospfVirtNbrLsRetransQLen) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtNbrEntryData.valid);
     break;

   case I_ospfVirtNbrHelloSuppressed:
     if (usmDbOspfVirtNbrHelloSuppressedGet(USMDB_UNIT_CURRENT, ospfVirtNbrEntryData.ospfVirtNbrArea,
                                          ospfVirtNbrEntryData.ospfVirtNbrRtrId,
                                          &ospfVirtNbrEntryData.ospfVirtNbrHelloSuppressed) == L7_SUCCESS)
        SET_VALID(nominator, ospfVirtNbrEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */
   
   if (nominator >= 0 && !VALID(nominator, ospfVirtNbrEntryData.valid))
      return(NULL);

   return(&ospfVirtNbrEntryData);
}

ospfExtLsdbEntry_t *
k_ospfExtLsdbEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 ospfExtLsdbType,
                       SR_UINT32 ospfExtLsdbLsid,
                       SR_UINT32 ospfExtLsdbRouterId)
{

   static ospfExtLsdbEntry_t ospfExtLsdbEntryData;
   L7_uint32 snmp_buffer_len;
   L7_char8 *snmp_buffer = NULL;
   static L7_BOOL firstTime = L7_TRUE;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   if (firstTime == L7_TRUE)
   {
     ospfExtLsdbEntryData.ospfExtLsdbAdvertisement = MakeOctetString(NULL, 0);

     firstTime = L7_FALSE;
   }

   ZERO_VALID(ospfExtLsdbEntryData.valid);

   ospfExtLsdbEntryData.ospfExtLsdbType = ospfExtLsdbType;
   ospfExtLsdbEntryData.ospfExtLsdbLsid = ospfExtLsdbLsid;
   ospfExtLsdbEntryData.ospfExtLsdbRouterId = ospfExtLsdbRouterId;

   SET_VALID(I_ospfExtLsdbType, ospfExtLsdbEntryData.valid);
   SET_VALID(I_ospfExtLsdbLsid, ospfExtLsdbEntryData.valid);
   SET_VALID(I_ospfExtLsdbRouterId, ospfExtLsdbEntryData.valid);

   if ((searchType == EXACT) ?
        (snmpOspfExtLsdbEntryGet(USMDB_UNIT_CURRENT, ospfExtLsdbEntryData.ospfExtLsdbType,
                                 ospfExtLsdbEntryData.ospfExtLsdbLsid,
                                 ospfExtLsdbEntryData.ospfExtLsdbRouterId) != L7_SUCCESS) :
        ((snmpOspfExtLsdbEntryGet(USMDB_UNIT_CURRENT, ospfExtLsdbEntryData.ospfExtLsdbType,
                                ospfExtLsdbEntryData.ospfExtLsdbLsid,
                                ospfExtLsdbEntryData.ospfExtLsdbRouterId) != L7_SUCCESS) &&
        (snmpOspfExtLsdbEntryNext(USMDB_UNIT_CURRENT, &ospfExtLsdbEntryData.ospfExtLsdbType,
                                  &ospfExtLsdbEntryData.ospfExtLsdbLsid,
                                  &ospfExtLsdbEntryData.ospfExtLsdbRouterId) != L7_SUCCESS)))
      return(NULL);

   switch(nominator)
   {
   case -1:
   case I_ospfExtLsdbType:
   case I_ospfExtLsdbLsid:
   case I_ospfExtLsdbRouterId:
     break;

   case I_ospfExtLsdbSequence:
     if (usmDbOspfExtLsdbSequenceGet(USMDB_UNIT_CURRENT, ospfExtLsdbEntryData.ospfExtLsdbType,
                                   ospfExtLsdbEntryData.ospfExtLsdbLsid,
                                   ospfExtLsdbEntryData.ospfExtLsdbRouterId,
                                   &ospfExtLsdbEntryData.ospfExtLsdbSequence) == L7_SUCCESS)
        SET_VALID(nominator, ospfExtLsdbEntryData.valid);
     break;

   case I_ospfExtLsdbAge:
     if (usmDbOspfExtLsdbAgeGet(USMDB_UNIT_CURRENT, ospfExtLsdbEntryData.ospfExtLsdbType,
                              ospfExtLsdbEntryData.ospfExtLsdbLsid,
                              ospfExtLsdbEntryData.ospfExtLsdbRouterId,
                              &ospfExtLsdbEntryData.ospfExtLsdbAge) == L7_SUCCESS)
        SET_VALID(nominator, ospfExtLsdbEntryData.valid);
     break;

   case I_ospfExtLsdbChecksum:
     if (usmDbOspfExtLsdbChecksumGet(USMDB_UNIT_CURRENT, ospfExtLsdbEntryData.ospfExtLsdbType,
                                   ospfExtLsdbEntryData.ospfExtLsdbLsid,
                                   ospfExtLsdbEntryData.ospfExtLsdbRouterId,
                                   &ospfExtLsdbEntryData.ospfExtLsdbChecksum) == L7_SUCCESS)
        SET_VALID(nominator, ospfExtLsdbEntryData.valid);
     break;

   case I_ospfExtLsdbAdvertisement:
     if (usmDbOspfExtLsdbAdvertisementGet(USMDB_UNIT_CURRENT, 
                    ospfExtLsdbEntryData.ospfExtLsdbType, ospfExtLsdbEntryData.ospfExtLsdbLsid,
                    ospfExtLsdbEntryData.ospfExtLsdbRouterId, &snmp_buffer, &snmp_buffer_len)
                    == L7_SUCCESS)
     {
         /* The SNMP Agent is not sending OCTETSTRINGS
	  * whose size > SNMP_AGENT_MAX_PKT_LEN ( = 2048 ) */
         if(snmp_buffer_len > SNMP_AGENT_MAX_PKT_LEN)
	 {
             snmp_buffer_len = SNMP_AGENT_MAX_PKT_LEN;
	 }
         if (SafeMakeOctetString(&ospfExtLsdbEntryData.ospfExtLsdbAdvertisement,
                                 snmp_buffer, snmp_buffer_len) == L7_TRUE)
	 {
             SET_VALID(nominator, ospfExtLsdbEntryData.valid);
	 }
         osapiFree(L7_OSPF_MAP_COMPONENT_ID, snmp_buffer);
         snmp_buffer = NULL;
     }
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, ospfExtLsdbEntryData.valid))
      return(NULL);

   return(&ospfExtLsdbEntryData);
}

ospfAreaAggregateEntry_t *
k_ospfAreaAggregateEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 ospfAreaAggregateAreaID,
                             SR_INT32 ospfAreaAggregateLsdbType,
                             SR_UINT32 ospfAreaAggregateNet,
                             SR_UINT32 ospfAreaAggregateMask)
{

   static ospfAreaAggregateEntry_t ospfAreaAggregateEntryData;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   ZERO_VALID(ospfAreaAggregateEntryData.valid);

   ospfAreaAggregateEntryData.ospfAreaAggregateAreaID = ospfAreaAggregateAreaID;
   ospfAreaAggregateEntryData.ospfAreaAggregateLsdbType = ospfAreaAggregateLsdbType;
   ospfAreaAggregateEntryData.ospfAreaAggregateNet = ospfAreaAggregateNet;
   ospfAreaAggregateEntryData.ospfAreaAggregateMask = ospfAreaAggregateMask;

   SET_VALID(I_ospfAreaAggregateAreaID, ospfAreaAggregateEntryData.valid);
   SET_VALID(I_ospfAreaAggregateLsdbType, ospfAreaAggregateEntryData.valid);
   SET_VALID(I_ospfAreaAggregateNet, ospfAreaAggregateEntryData.valid);
   SET_VALID(I_ospfAreaAggregateMask, ospfAreaAggregateEntryData.valid);

   if ((searchType == EXACT) ?
        (snmpOspfAreaAggregateEntryGet(USMDB_UNIT_CURRENT, ospfAreaAggregateEntryData.ospfAreaAggregateAreaID,
                                       ospfAreaAggregateEntryData.ospfAreaAggregateLsdbType,
                                       ospfAreaAggregateEntryData.ospfAreaAggregateNet,
                                       ospfAreaAggregateEntryData.ospfAreaAggregateMask) != L7_SUCCESS) :
        ((snmpOspfAreaAggregateEntryGet(USMDB_UNIT_CURRENT, ospfAreaAggregateEntryData.ospfAreaAggregateAreaID,
                                      ospfAreaAggregateEntryData.ospfAreaAggregateLsdbType,
                                      ospfAreaAggregateEntryData.ospfAreaAggregateNet,
                                      ospfAreaAggregateEntryData.ospfAreaAggregateMask) != L7_SUCCESS) &&
        (snmpOspfAreaAggregateEntryNext(USMDB_UNIT_CURRENT, &ospfAreaAggregateEntryData.ospfAreaAggregateAreaID,
                                        &ospfAreaAggregateEntryData.ospfAreaAggregateLsdbType,
                                        &ospfAreaAggregateEntryData.ospfAreaAggregateNet,
                                        &ospfAreaAggregateEntryData.ospfAreaAggregateMask) != L7_SUCCESS)))
      return(NULL);

   switch(nominator)
   {
   case -1:
   case I_ospfAreaAggregateAreaID:
   case I_ospfAreaAggregateLsdbType:
   case I_ospfAreaAggregateNet:
   case I_ospfAreaAggregateMask:
     break;
   
   case I_ospfAreaAggregateStatus:
     if (snmpOspfAreaAggregateStatusGet(USMDB_UNIT_CURRENT, ospfAreaAggregateEntryData.ospfAreaAggregateAreaID,
                                        ospfAreaAggregateEntryData.ospfAreaAggregateLsdbType,
                                        ospfAreaAggregateEntryData.ospfAreaAggregateNet,
                                        ospfAreaAggregateEntryData.ospfAreaAggregateMask,
                                        &ospfAreaAggregateEntryData.ospfAreaAggregateStatus) == L7_SUCCESS)
       SET_VALID(nominator, ospfAreaAggregateEntryData.valid);
     break;

   case I_ospfAreaAggregateEffect:
     if (snmpOspfAreaAggregateEffectGet(USMDB_UNIT_CURRENT, ospfAreaAggregateEntryData.ospfAreaAggregateAreaID,
                                        ospfAreaAggregateEntryData.ospfAreaAggregateLsdbType,
                                        ospfAreaAggregateEntryData.ospfAreaAggregateNet,
                                        ospfAreaAggregateEntryData.ospfAreaAggregateMask,
                                        &ospfAreaAggregateEntryData.ospfAreaAggregateEffect) == L7_SUCCESS)
        SET_VALID(nominator, ospfAreaAggregateEntryData.valid);
     break;
   
   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, ospfAreaAggregateEntryData.valid))
      return(NULL);

   return(&ospfAreaAggregateEntryData);
}

#ifdef SETS
int
k_ospfAreaAggregateEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
k_ospfAreaAggregateEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
k_ospfAreaAggregateEntry_set_defaults(doList_t *dp)
{
   ospfAreaAggregateEntry_t *data = (ospfAreaAggregateEntry_t *) (dp->data);

   data->ospfAreaAggregateEffect = D_ospfAreaAggregateEffect_advertiseMatching;
   data->ospfAreaAggregateStatus = D_ospfAreaAggregateStatus_active;

   /* component provides default configuration */
   ZERO_VALID(data->valid);
   return(NO_ERROR);
}

int
k_ospfAreaAggregateEntry_set(ospfAreaAggregateEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfAreaAggregateEffect, data->valid) &&
       (snmpOspfAreaAggregateEffectSet(USMDB_UNIT_CURRENT, data->ospfAreaAggregateAreaID,
                                       data->ospfAreaAggregateLsdbType,
                                       data->ospfAreaAggregateNet,
                                       data->ospfAreaAggregateMask,
                                       data->ospfAreaAggregateEffect) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_ospfAreaAggregateStatus, data->valid))
   {
     if (data->ospfAreaAggregateStatus == D_ospfAreaAggregateStatus_createAndGo )
     {
       if ( usmDbOspfAreaAggregateStatusSet(USMDB_UNIT_CURRENT, data->ospfAreaAggregateAreaID, data->ospfAreaAggregateLsdbType, data->ospfAreaAggregateNet,data->ospfAreaAggregateMask, L7_CREATE)!= L7_SUCCESS )
       {
         return(COMMIT_FAILED_ERROR);
       }
     }
     else  if (data->ospfAreaAggregateStatus == D_ospfAreaAggregateStatus_destroy )
     {
       if ( usmDbOspfAreaAggregateStatusSet(USMDB_UNIT_CURRENT, data->ospfAreaAggregateAreaID, data->ospfAreaAggregateLsdbType, data->ospfAreaAggregateNet,data->ospfAreaAggregateMask, L7_DELETE)!= L7_SUCCESS )
       { 
         return(COMMIT_FAILED_ERROR);
       }
     } 
     else 
     {
       return(COMMIT_FAILED_ERROR); 
     } 
   }


   return(NO_ERROR);
}

#ifdef SR_ospfAreaAggregateEntry_UNDO
/* add #define SR_ospfAreaAggregateEntry_UNDO in sitedefs.h to
 * include the undo routine for the ospfAreaAggregateEntry family.
 */
int
ospfAreaAggregateEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return(UNDO_FAILED_ERROR);
}
#endif /* SR_ospfAreaAggregateEntry_UNDO */

#endif /* SETS */


ospfTrapControl_t *
k_ospfTrapControl_get(int serialNum, ContextInfo *contextInfo,
                      int nominator)
{
   static ospfTrapControl_t ospfTrapControlData;
   char snmp_buffer[SNMP_BUFFER_LEN];

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(NULL);

   switch(nominator)
   {

   case -1:
     break;

   case I_ospfSetTrap:
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     if((snmpOspfSetTrapGet(USMDB_UNIT_CURRENT,snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetString(&ospfTrapControlData.ospfSetTrap, snmp_buffer, SNMP_OSPF_SETTRAP_LEN) == L7_TRUE))
           SET_VALID(nominator, ospfTrapControlData.valid);
     break;

#ifdef NOT_SUPPORTED
   case I_ospfConfigErrorType:
     if (usmDbOspfConfigErrorTypeGet(USMDB_UNIT_CURRENT, 
                                       &ospfTrapControlData.ospfConfigErrorType) == L7_SUCCESS)
        SET_VALID(nominator, ospfTrapControlData.valid);
     break;

   case I_ospfPacketType:
     if (usmDbOspfPacketTypeGet(USMDB_UNIT_CURRENT, 
                                       &ospfTrapControlData.ospfPacketType) == L7_SUCCESS)
        SET_VALID(nominator, ospfTrapControlData.valid);
     break;

   case I_ospfPacketSrc:
     if (usmDbOspfPacketSrcGet(USMDB_UNIT_CURRENT, 
                                       &ospfTrapControlData.ospfPacketSrc) == L7_SUCCESS)
        SET_VALID(nominator, ospfTrapControlData.valid);
#endif /* NOT_SUPPORTED */

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, ospfTrapControlData.valid))
      return(NULL);

   return(&ospfTrapControlData);
}

#ifdef SETS
int
k_ospfTrapControl_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_ospfTrapControl_ready(ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_ospfTrapControl_set(ospfTrapControl_t *data,
                      ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
      if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
           return(COMMIT_FAILED_ERROR);

  if (VALID(I_ospfSetTrap, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->ospfSetTrap->octet_ptr, data->ospfSetTrap->length);
    if (snmpOspfSetTrapSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
  return(NO_ERROR);
}

#ifdef SR_ospfTrapControl_UNDO
/* add #define SR_ospfTrapControl_UNDO in sitedefs.h to
 * include the undo routine for the ospfTrapControl family.
 */
int
ospfTrapControl_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ospfTrapControl_UNDO */

#endif /* SETS */


/************************************************************************************/
agentOspfRouteRedistEntry_t *
k_agentOspfRouteRedistEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_INT32 agentOspfRouteRedistSource)
{


  static agentOspfRouteRedistEntry_t agentOspfRouteRedistEntryData;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);
 
  ZERO_VALID(agentOspfRouteRedistEntryData.valid);
  agentOspfRouteRedistEntryData.agentOspfRouteRedistSource =   agentOspfRouteRedistSource;
  SET_VALID(I_agentOspfRouteRedistSource, agentOspfRouteRedistEntryData.valid);

  if ( (searchType == EXACT) ?
     (snmpAgentOspfRouteRedistributeSourceGet (USMDB_UNIT_CURRENT, agentOspfRouteRedistEntryData.agentOspfRouteRedistSource) != L7_SUCCESS) :
     ( (snmpAgentOspfRouteRedistributeSourceGet(USMDB_UNIT_CURRENT, agentOspfRouteRedistEntryData.agentOspfRouteRedistSource) != L7_SUCCESS) &&
       (snmpAgentOspfRouteRedistributeSourceGetNext(USMDB_UNIT_CURRENT,
                                          &agentOspfRouteRedistEntryData.agentOspfRouteRedistSource) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentOspfRouteRedistEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
    case -1:

    case I_agentOspfRouteRedistSource :
      break;

    case I_agentOspfRouteRedistMode :
    if (snmpAgentOspfRedistributeModeGet(USMDB_UNIT_CURRENT,
                                agentOspfRouteRedistEntryData.agentOspfRouteRedistSource,
                                &agentOspfRouteRedistEntryData.agentOspfRouteRedistMode) == L7_SUCCESS)
    SET_VALID(I_agentOspfRouteRedistMode, agentOspfRouteRedistEntryData.valid);
      break;

    case I_agentOspfRouteRedistMetric :
    if (snmpAgentOspfRedistMetricGet(USMDB_UNIT_CURRENT,
                                 agentOspfRouteRedistEntryData.agentOspfRouteRedistSource,
                                 &agentOspfRouteRedistEntryData.agentOspfRouteRedistMetric) == L7_SUCCESS)
    SET_VALID(I_agentOspfRouteRedistMetric, agentOspfRouteRedistEntryData.valid);
      break;

    case I_agentOspfRouteRedistMetricConfigured :
    if (snmpAgentOspfRedistMetricConfiguredGet(USMDB_UNIT_CURRENT,
                                               agentOspfRouteRedistEntryData.agentOspfRouteRedistSource,
                                               &agentOspfRouteRedistEntryData.agentOspfRouteRedistMetricConfigured) == L7_SUCCESS)
    SET_VALID(I_agentOspfRouteRedistMetricConfigured, agentOspfRouteRedistEntryData.valid);
      break;

    case I_agentOspfRouteRedistMetricType :
    if (snmpAgentOspfRedistMetricTypeGet(USMDB_UNIT_CURRENT,
                                         agentOspfRouteRedistEntryData.agentOspfRouteRedistSource,
                                         &agentOspfRouteRedistEntryData.agentOspfRouteRedistMetricType) == L7_SUCCESS)
    SET_VALID(I_agentOspfRouteRedistMetricType, agentOspfRouteRedistEntryData.valid);
      break;

    case I_agentOspfRouteRedistTag :
    if (snmpAgentOspfTagGet(USMDB_UNIT_CURRENT,
                        agentOspfRouteRedistEntryData.agentOspfRouteRedistSource,
                        &agentOspfRouteRedistEntryData.agentOspfRouteRedistTag) == L7_SUCCESS)
    SET_VALID(I_agentOspfRouteRedistTag, agentOspfRouteRedistEntryData.valid);
      break;

    case I_agentOspfRouteRedistSubnets :
    if (snmpAgentOspfRedistSubnetsGet(USMDB_UNIT_CURRENT,
                    agentOspfRouteRedistEntryData.agentOspfRouteRedistSource,
                    &agentOspfRouteRedistEntryData.agentOspfRouteRedistSubnets) == L7_SUCCESS)
    SET_VALID(I_agentOspfRouteRedistSubnets, agentOspfRouteRedistEntryData.valid);
      break;

    case I_agentOspfRouteRedistDistList :
    if (snmpAgentOspfDistListGet(USMDB_UNIT_CURRENT,
                             agentOspfRouteRedistEntryData.agentOspfRouteRedistSource,
                             &agentOspfRouteRedistEntryData.agentOspfRouteRedistDistList) == L7_SUCCESS)
    SET_VALID(I_agentOspfRouteRedistDistList, agentOspfRouteRedistEntryData.valid);
      break;

    case I_agentOspfRouteRedistDistListConfigured :
    if (snmpAgentOspfDistListConfiguredGet(USMDB_UNIT_CURRENT,
                             agentOspfRouteRedistEntryData.agentOspfRouteRedistSource,
                             &agentOspfRouteRedistEntryData.agentOspfRouteRedistDistListConfigured) == L7_SUCCESS)
    SET_VALID(I_agentOspfRouteRedistDistListConfigured, agentOspfRouteRedistEntryData.valid);
      break;


    default:
    /* unknown nominator */
    return(NULL);
      break;
  }


  if (nominator >= 0 && !VALID(nominator, agentOspfRouteRedistEntryData.valid))
    return(NULL);

  return(&agentOspfRouteRedistEntryData);
}

#ifdef SETS
int
k_agentOspfRouteRedistEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentOspfRouteRedistEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentOspfRouteRedistEntry_set_defaults(doList_t *dp)
{
    agentOspfRouteRedistEntry_t *data = (agentOspfRouteRedistEntry_t *) (dp->data);

    data->agentOspfRouteRedistMode = D_agentOspfRouteRedistMode_disable;
    data->agentOspfRouteRedistMetricType = D_agentOspfRouteRedistMetricType_externalType2;
    data->agentOspfRouteRedistSubnets = D_agentOspfRouteRedistSubnets_false;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentOspfRouteRedistEntry_set(agentOspfRouteRedistEntry_t *data,
                                ContextInfo *contextInfo, int function)
{
   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
        return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentOspfRouteRedistMode, data->valid) &&
      snmpAgentOspfRedistributeModeSet(USMDB_UNIT_CURRENT,
                                   data->agentOspfRouteRedistSource,
                                   data->agentOspfRouteRedistMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRouteRedistMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }


  if (VALID(I_agentOspfRouteRedistMetric, data->valid) &&
      snmpAgentOspfRedistMetricSet(USMDB_UNIT_CURRENT,
                               data->agentOspfRouteRedistSource,
                               data->agentOspfRouteRedistMetric) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRouteRedistMetric, data->valid);
    return COMMIT_FAILED_ERROR;
  }


  if (VALID(I_agentOspfRouteRedistMetricConfigured, data->valid) &&
      snmpAgentOspfRedistMetricConfiguredSet(USMDB_UNIT_CURRENT,
                               data->agentOspfRouteRedistSource,
                               data->agentOspfRouteRedistMetricConfigured) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRouteRedistMetricConfigured, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfRouteRedistMetricType, data->valid) &&
      snmpAgentOspfRedistMetricTypeSet(USMDB_UNIT_CURRENT,
                                       data->agentOspfRouteRedistSource,
                                       data->agentOspfRouteRedistMetricType) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRouteRedistMetricType, data->valid);
    return COMMIT_FAILED_ERROR;
  }


  if (VALID(I_agentOspfRouteRedistTag, data->valid) &&
      snmpAgentOspfTagSet(USMDB_UNIT_CURRENT,
                      data->agentOspfRouteRedistSource,
                      data->agentOspfRouteRedistTag) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRouteRedistTag, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfRouteRedistSubnets, data->valid) &&
      snmpAgentOspfRedistSubnetsSet(USMDB_UNIT_CURRENT,
                                  data->agentOspfRouteRedistSource,
                                  data->agentOspfRouteRedistSubnets) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRouteRedistSubnets, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfRouteRedistDistList, data->valid) &&
      snmpAgentOspfDistListSet(USMDB_UNIT_CURRENT,
                                  data->agentOspfRouteRedistSource,
                                  data->agentOspfRouteRedistDistList) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRouteRedistDistList, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfRouteRedistDistListConfigured, data->valid) &&
      snmpAgentOspfRedistDistListConfiguredSet(USMDB_UNIT_CURRENT,
                               data->agentOspfRouteRedistSource,
                               data->agentOspfRouteRedistDistListConfigured) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRouteRedistDistListConfigured, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_agentOspfRouteRedistEntry_UNDO
/* add #define SR_agentOspfRouteRedistEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentOspfRouteRedistEntry family.
 */
int
agentOspfRouteRedistEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentOspfRouteRedistEntry_UNDO */

#endif /* SETS */

L7_RC_t
snmpOspfAsOpaqueLsdbEntryNext(L7_int32 *type, L7_uint32 *lsId, L7_int32 *rtrId)
{
  L7_uint32 temp_Type = L7_NULL;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if (*type == D_agentOspfAsLsdbType_asOpaqueLink)
  {
    temp_Type = L7_S_AS_OPAQUE_LSA;
  }
  if(usmDbOspfAsOpaqueLsdbEntryNext(&temp_Type, lsId, rtrId, &temp_Lsa) 
                                                         == L7_SUCCESS)
  {
    switch (temp_Type)
    {
      case L7_S_ILLEGAL_LSA:
      case L7_S_AS_OPAQUE_LSA:
        *type = D_agentOspfAsLsdbType_asOpaqueLink;
      break;
      default:
        return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpOspfAsOpaqueLsdbEntryGet(L7_int32 type, L7_uint32 lsId, L7_int32 rtrId)
{
  L7_uint32 temp_Type = L7_NULL;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if (type == D_agentOspfAsLsdbType_asOpaqueLink)
  {
    temp_Type = L7_S_AS_OPAQUE_LSA;
  }
  return usmDbOspfAsOpaqueLsdbEntryGet(temp_Type, lsId, rtrId, &temp_Lsa);
}

L7_RC_t
snmpOspfLocalLsdbEntryNext(L7_uint32 *ipAddr, L7_int32 *ifIndex, 
                           L7_int32 *type, L7_uint32 *lsId, 
                           L7_int32 *rtrId)
{
  L7_uint32 temp_Type = L7_NULL;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if (*type == D_agentOspfLocalLsdbType_localOpaqueLink)
  {
    temp_Type = L7_S_LINK_OPAQUE_LSA;
  }
  if(usmDbOspfLinkOpaqueLsdbEntryNext(ipAddr, ifIndex, &temp_Type,
                               lsId, rtrId, &temp_Lsa) == L7_SUCCESS)
  {
    switch (temp_Type)
    {
      case L7_S_ILLEGAL_LSA:
      case L7_S_LINK_OPAQUE_LSA:
        *type = D_agentOspfLocalLsdbType_localOpaqueLink;
      break;
      default:
        return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpOspfLocalLsdbEntryGet(L7_uint32 ipAddr, L7_int32 ifIndex, 
                          L7_int32 type, L7_uint32 lsId, 
                          L7_int32 rtrId)
{
  L7_uint32 temp_Type = L7_NULL;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if (type == D_agentOspfLocalLsdbType_localOpaqueLink)
  {
    temp_Type = L7_S_LINK_OPAQUE_LSA;
  }
  return usmDbOspfLinkOpaqueLsdbEntryGet(ipAddr, ifIndex, temp_Type,
                               lsId, rtrId, &temp_Lsa);
}

L7_RC_t
snmpAgentOspfAreaOpaqueLsdbEntryNext( L7_uint32 *areaID,
                                     L7_int32 *type,
                                     L7_uint32 *lsid,
                                     L7_uint32 *routerId )
{
  L7_uint32 temp_Type = L7_NULL;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if (*type == D_agentOspfAreaOpaqueLsdbType_areaOpaqueLink)
  {
    temp_Type = L7_S_AREA_OPAQUE_LSA;
  }
  if(usmDbOspfAreaOpaqueLsdbEntryNext(areaID, &temp_Type,
                               lsid, routerId, &temp_Lsa) == L7_SUCCESS)
  {
    switch (temp_Type)
    {
      case L7_S_ILLEGAL_LSA:
      case L7_S_AREA_OPAQUE_LSA:
        *type = D_agentOspfAreaOpaqueLsdbType_areaOpaqueLink;
      break;
      default:
        return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentOspfAreaOpaqueLsdbEntryGet( L7_uint32 areaID,
                                     L7_int32 type,
                                     L7_uint32 lsid,
                                     L7_uint32 routerId )
{
  L7_uint32 temp_Type = L7_NULL;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if (type == D_agentOspfAreaOpaqueLsdbType_areaOpaqueLink)
  {
    temp_Type = L7_S_AREA_OPAQUE_LSA;
  }
  return usmDbOspfAreaOpaqueLsdbEntryGet(areaID, temp_Type,
                               lsid, routerId, &temp_Lsa);
}

agentOspfAreaOpaqueLsdbEntry_t *
k_agentOspfAreaOpaqueLsdbEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_UINT32 agentOspfAreaOpaqueLsdbAreaId,
                                   SR_INT32 agentOspfAreaOpaqueLsdbType,
                                   SR_UINT32 agentOspfAreaOpaqueLsdbLsid,
                                   SR_UINT32 agentOspfAreaOpaqueLsdbRouterId)
{
   static agentOspfAreaOpaqueLsdbEntry_t agentOspfAreaOpaqueLsdbEntryData;
   L7_char8 *snmp_buffer = NULL;
   L7_uint32 snmp_buffer_len;
   static L7_BOOL firstTime = L7_TRUE;


   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
     return(NULL);

   if (firstTime == L7_TRUE)
   {
     agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAdvertisement = 
                                                       MakeOctetString(NULL, 0);
     firstTime = L7_FALSE;
   }

   ZERO_VALID(agentOspfAreaOpaqueLsdbEntryData.valid);

   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAreaId = 
                                                  agentOspfAreaOpaqueLsdbAreaId;
   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbType = 
                                                    agentOspfAreaOpaqueLsdbType;
   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbLsid = 
                                                    agentOspfAreaOpaqueLsdbLsid;
   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbRouterId = 
                                                agentOspfAreaOpaqueLsdbRouterId;

   SET_VALID(I_agentOspfAreaOpaqueLsdbAreaId, 
                                        agentOspfAreaOpaqueLsdbEntryData.valid);
   SET_VALID(I_agentOspfAreaOpaqueLsdbType, 
                                        agentOspfAreaOpaqueLsdbEntryData.valid);
   SET_VALID(I_agentOspfAreaOpaqueLsdbLsid, 
                                        agentOspfAreaOpaqueLsdbEntryData.valid);
   SET_VALID(I_agentOspfAreaOpaqueLsdbRouterId, 
                                        agentOspfAreaOpaqueLsdbEntryData.valid);

   if ((searchType == EXACT) ?
        (snmpAgentOspfAreaOpaqueLsdbEntryGet(
                 agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAreaId,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbType,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbLsid,
               agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbRouterId)
        != L7_SUCCESS) :
       ((snmpAgentOspfAreaOpaqueLsdbEntryGet(
                 agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAreaId,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbType,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbLsid,
               agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbRouterId) 
        != L7_SUCCESS) &&
        (snmpAgentOspfAreaOpaqueLsdbEntryNext(
                &agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAreaId,
                  &agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbType,
                  &agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbLsid,
              &agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbRouterId) 
        != L7_SUCCESS)))
      return(NULL);

   switch(nominator)
   {

   case -1:
   case I_agentOspfAreaOpaqueLsdbAreaId:
   case I_agentOspfAreaOpaqueLsdbType:
   case I_agentOspfAreaOpaqueLsdbLsid:
   case I_agentOspfAreaOpaqueLsdbRouterId:
     break;

   case I_agentOspfAreaOpaqueLsdbSequence:
     if (usmDbOspfAreaOpaqueLsdbSequenceGet(
                 agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAreaId,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbType,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbLsid,
               agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbRouterId,
              &agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbSequence) 
        == L7_SUCCESS)
        SET_VALID(nominator, agentOspfAreaOpaqueLsdbEntryData.valid);
     break;

   case I_agentOspfAreaOpaqueLsdbAge:
     if (usmDbOspfAreaOpaqueLsdbAgeGet(
                 agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAreaId,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbType,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbLsid,
               agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbRouterId,
                   &agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAge) 
        == L7_SUCCESS)
        SET_VALID(nominator, agentOspfAreaOpaqueLsdbEntryData.valid);
     break;

   case I_agentOspfAreaOpaqueLsdbChecksum:
     if (usmDbOspfAreaOpaqueLsdbChecksumGet(
                 agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAreaId,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbType,
                   agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbLsid,
               agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbRouterId,
              &agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbChecksum)
        == L7_SUCCESS)
        SET_VALID(nominator, agentOspfAreaOpaqueLsdbEntryData.valid);
     break;

   case I_agentOspfAreaOpaqueLsdbAdvertisement:
     if (usmDbOspfAreaOpaqueLsdbAdvertisementGet(
                  agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAreaId,
                    agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbType, 
                    agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbLsid,
                agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbRouterId,
                                      &snmp_buffer, &snmp_buffer_len)
                           == L7_SUCCESS)
     {
       /* The SNMP Agent is not sending OCTETSTRINGS
        * whose size > SNMP_AGENT_MAX_PKT_LEN ( = 2048 ) */
       if(snmp_buffer_len > SNMP_AGENT_MAX_PKT_LEN)
       {
           snmp_buffer_len = SNMP_AGENT_MAX_PKT_LEN;
       }
       if (SafeMakeOctetString(
         &agentOspfAreaOpaqueLsdbEntryData.agentOspfAreaOpaqueLsdbAdvertisement,
                               snmp_buffer, snmp_buffer_len) == L7_TRUE )
       {
           SET_VALID(nominator, agentOspfAreaOpaqueLsdbEntryData.valid);
       }
       osapiFree(L7_OSPF_MAP_COMPONENT_ID, snmp_buffer);
       snmp_buffer = NULL;
     }
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, agentOspfAreaOpaqueLsdbEntryData.valid))
      return(NULL);

   return(&agentOspfAreaOpaqueLsdbEntryData);
}

agentOspfLocalLsdbEntry_t *
k_agentOspfLocalLsdbEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 agentOspfLocalLsdbIpAddress,
                              SR_INT32 agentOspfLocalLsdbAddressLessIf,
                              SR_INT32 agentOspfLocalLsdbType,
                              SR_UINT32 agentOspfLocalLsdbLsid,
                              SR_UINT32 agentOspfLocalLsdbRouterId)
{
   static agentOspfLocalLsdbEntry_t agentOspfLocalLsdbEntryData;
   L7_char8 *snmp_buffer = NULL;
   L7_uint32 snmp_buffer_len;
   static L7_BOOL firstTime = L7_TRUE;


   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
     return(NULL);

   if (firstTime == L7_TRUE)
   {
     agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAdvertisement = 
                                                       MakeOctetString(NULL, 0);
     firstTime = L7_FALSE;
   }

   ZERO_VALID(agentOspfLocalLsdbEntryData.valid);

   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbIpAddress = 
                                                  agentOspfLocalLsdbIpAddress;
   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAddressLessIf = 
                                              agentOspfLocalLsdbAddressLessIf;
   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbType = 
                                                    agentOspfLocalLsdbType;
   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbLsid = 
                                                agentOspfLocalLsdbLsid;
   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbRouterId = 
                                                agentOspfLocalLsdbRouterId;

   SET_VALID(I_agentOspfLocalLsdbIpAddress,
                                        agentOspfLocalLsdbEntryData.valid);
   SET_VALID(I_agentOspfLocalLsdbAddressLessIf, 
                                        agentOspfLocalLsdbEntryData.valid);
   SET_VALID(I_agentOspfLocalLsdbType, 
                                        agentOspfLocalLsdbEntryData.valid);
   SET_VALID(I_agentOspfLocalLsdbLsid, 
                                        agentOspfLocalLsdbEntryData.valid);
   SET_VALID(I_agentOspfLocalLsdbRouterId, 
                                        agentOspfLocalLsdbEntryData.valid);

   if ((searchType == EXACT) ?
        (snmpOspfLocalLsdbEntryGet(
                 agentOspfLocalLsdbEntryData.agentOspfLocalLsdbIpAddress,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAddressLessIf,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbType,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbLsid,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbRouterId) 
        != L7_SUCCESS) :
       ((snmpOspfLocalLsdbEntryGet(
                 agentOspfLocalLsdbEntryData.agentOspfLocalLsdbIpAddress,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAddressLessIf,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbType,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbLsid,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbRouterId) 
        != L7_SUCCESS) &&
        (snmpOspfLocalLsdbEntryNext(
                &agentOspfLocalLsdbEntryData.agentOspfLocalLsdbIpAddress,
                  &agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAddressLessIf,
                  &agentOspfLocalLsdbEntryData.agentOspfLocalLsdbType,
              &agentOspfLocalLsdbEntryData.agentOspfLocalLsdbLsid,
              &agentOspfLocalLsdbEntryData.agentOspfLocalLsdbRouterId) 
        != L7_SUCCESS)))
      return(NULL);

   switch(nominator)
   {

   case -1:
   case I_agentOspfLocalLsdbIpAddress:
   case I_agentOspfLocalLsdbAddressLessIf:
   case I_agentOspfLocalLsdbType:
   case I_agentOspfLocalLsdbLsid:
   case I_agentOspfLocalLsdbRouterId:
     break;

   case I_agentOspfLocalLsdbSequence:
     if (usmDbOspfLinkOpaqueLsdbSequenceGet(
                 agentOspfLocalLsdbEntryData.agentOspfLocalLsdbIpAddress,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAddressLessIf,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbType,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbLsid,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbRouterId,
              &agentOspfLocalLsdbEntryData.agentOspfLocalLsdbSequence) 
        == L7_SUCCESS)
        SET_VALID(nominator, agentOspfLocalLsdbEntryData.valid);
     break;

   case I_agentOspfLocalLsdbAge:
     if (usmDbOspfLinkOpaqueLsdbAgeGet(
                 agentOspfLocalLsdbEntryData.agentOspfLocalLsdbIpAddress,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAddressLessIf,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbType,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbLsid,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbRouterId,
                   &agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAge) 
        == L7_SUCCESS)
        SET_VALID(nominator, agentOspfLocalLsdbEntryData.valid);
     break;

   case I_agentOspfLocalLsdbChecksum:
     if (usmDbOspfLinkOpaqueLsdbChecksumGet(
                 agentOspfLocalLsdbEntryData.agentOspfLocalLsdbIpAddress,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAddressLessIf,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbType,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbLsid,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbRouterId,
              &agentOspfLocalLsdbEntryData.agentOspfLocalLsdbChecksum)
        == L7_SUCCESS)
        SET_VALID(nominator, agentOspfLocalLsdbEntryData.valid);
     break;

   case I_agentOspfLocalLsdbAdvertisement:
     if (usmDbOspfLinkOpaqueLsdbAdvertisementGet(
                 agentOspfLocalLsdbEntryData.agentOspfLocalLsdbIpAddress,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAddressLessIf,
                   agentOspfLocalLsdbEntryData.agentOspfLocalLsdbType,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbLsid,
               agentOspfLocalLsdbEntryData.agentOspfLocalLsdbRouterId,
                                      &snmp_buffer, &snmp_buffer_len)
                           == L7_SUCCESS)
     {
       /* The SNMP Agent is not sending OCTETSTRINGS
        * whose size > SNMP_AGENT_MAX_PKT_LEN ( = 2048 ) */
       if(snmp_buffer_len > SNMP_AGENT_MAX_PKT_LEN)
       {
           snmp_buffer_len = SNMP_AGENT_MAX_PKT_LEN;
       }
       if (SafeMakeOctetString(
         &agentOspfLocalLsdbEntryData.agentOspfLocalLsdbAdvertisement,
                               snmp_buffer, snmp_buffer_len) == L7_TRUE )
       {
           SET_VALID(nominator, agentOspfLocalLsdbEntryData.valid);
       }
       osapiFree(L7_OSPF_MAP_COMPONENT_ID, snmp_buffer);
       snmp_buffer = NULL;
     }
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, agentOspfLocalLsdbEntryData.valid))
      return(NULL);

   return(&agentOspfLocalLsdbEntryData);
}

agentOspfAsLsdbEntry_t *
k_agentOspfAsLsdbEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 agentOspfAsLsdbType,
                           SR_UINT32 agentOspfAsLsdbLsid,
                           SR_UINT32 agentOspfAsLsdbRouterId)
{
   static agentOspfAsLsdbEntry_t agentOspfAsLsdbEntryData;
   L7_char8 *snmp_buffer = NULL;
   L7_uint32 snmp_buffer_len;
   static L7_BOOL firstTime = L7_TRUE;


   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
      return(NULL);

   if (firstTime == L7_TRUE)
   {
     agentOspfAsLsdbEntryData.agentOspfAsLsdbAdvertisement = 
                                                       MakeOctetString(NULL, 0);
     firstTime = L7_FALSE;
   }

   ZERO_VALID(agentOspfAsLsdbEntryData.valid);

   agentOspfAsLsdbEntryData.agentOspfAsLsdbType = agentOspfAsLsdbType;
   agentOspfAsLsdbEntryData.agentOspfAsLsdbLsid = agentOspfAsLsdbLsid;
   agentOspfAsLsdbEntryData.agentOspfAsLsdbRouterId = agentOspfAsLsdbRouterId;

   SET_VALID(I_agentOspfAsLsdbType, agentOspfAsLsdbEntryData.valid);
   SET_VALID(I_agentOspfAsLsdbLsid, agentOspfAsLsdbEntryData.valid);
   SET_VALID(I_agentOspfAsLsdbRouterId, agentOspfAsLsdbEntryData.valid);

   if ((searchType == EXACT) ?
        (snmpOspfAsOpaqueLsdbEntryGet(
                   agentOspfAsLsdbEntryData.agentOspfAsLsdbType,
                   agentOspfAsLsdbEntryData.agentOspfAsLsdbLsid,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbRouterId) 
        != L7_SUCCESS) :
       ((snmpOspfAsOpaqueLsdbEntryGet(
                   agentOspfAsLsdbEntryData.agentOspfAsLsdbType,
                   agentOspfAsLsdbEntryData.agentOspfAsLsdbLsid,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbRouterId) 
        != L7_SUCCESS) &&
        (snmpOspfAsOpaqueLsdbEntryNext(
                  &agentOspfAsLsdbEntryData.agentOspfAsLsdbType,
                  &agentOspfAsLsdbEntryData.agentOspfAsLsdbLsid,
              &agentOspfAsLsdbEntryData.agentOspfAsLsdbRouterId) 
        != L7_SUCCESS)))
      return(NULL);

   switch(nominator)
   {

   case -1:
   case I_agentOspfAsLsdbType:
   case I_agentOspfAsLsdbLsid:
   case I_agentOspfAsLsdbRouterId:
     break;

   case I_agentOspfAsLsdbSequence:
     if (usmDbOspfAsOpaqueLsdbSequenceGet(
               agentOspfAsLsdbEntryData.agentOspfAsLsdbType,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbLsid,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbRouterId,
              &agentOspfAsLsdbEntryData.agentOspfAsLsdbSequence) 
        == L7_SUCCESS)
        SET_VALID(nominator, agentOspfAsLsdbEntryData.valid);
     break;

   case I_agentOspfAsLsdbAge:
     if (usmDbOspfAsOpaqueLsdbAgeGet( 
               agentOspfAsLsdbEntryData.agentOspfAsLsdbType,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbLsid,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbRouterId,
              &agentOspfAsLsdbEntryData.agentOspfAsLsdbAge) 
        == L7_SUCCESS)
        SET_VALID(nominator, agentOspfAsLsdbEntryData.valid);
     break;

   case I_agentOspfAsLsdbChecksum:
     if (usmDbOspfAsOpaqueLsdbChecksumGet(
               agentOspfAsLsdbEntryData.agentOspfAsLsdbType,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbLsid,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbRouterId,
              &agentOspfAsLsdbEntryData.agentOspfAsLsdbChecksum)
        == L7_SUCCESS)
        SET_VALID(nominator, agentOspfAsLsdbEntryData.valid);
     break;

   case I_agentOspfAsLsdbAdvertisement:
     if (usmDbOspfAsOpaqueLsdbAdvertisementGet(
               agentOspfAsLsdbEntryData.agentOspfAsLsdbType,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbLsid,
               agentOspfAsLsdbEntryData.agentOspfAsLsdbRouterId,
              &snmp_buffer, &snmp_buffer_len)
                           == L7_SUCCESS)
     {
       /* The SNMP Agent is not sending OCTETSTRINGS
        * whose size > SNMP_AGENT_MAX_PKT_LEN ( = 2048 ) */
       if(snmp_buffer_len > SNMP_AGENT_MAX_PKT_LEN)
       {
         snmp_buffer_len = SNMP_AGENT_MAX_PKT_LEN;
       }
       if (SafeMakeOctetString(
         &agentOspfAsLsdbEntryData.agentOspfAsLsdbAdvertisement,
                               snmp_buffer, snmp_buffer_len) == L7_TRUE )
       {
         SET_VALID(nominator, agentOspfAsLsdbEntryData.valid);
       }
       osapiFree(L7_OSPF_MAP_COMPONENT_ID, snmp_buffer);
       snmp_buffer = NULL;
     }
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }  /* end of switch(nominator)  */

   if (nominator >= 0 && !VALID(nominator, agentOspfAsLsdbEntryData.valid))
      return(NULL);

   return(&agentOspfAsLsdbEntryData);
}


agentRouterOspfConfigGroup_t *
k_agentRouterOspfConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator)
{
  static agentRouterOspfConfigGroup_t agentRouterOspfConfigGroupData;

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  switch (nominator)
  {

  case I_agentOspfDefaultMetric:
    if (snmpAgentOspfDefaultMetricGet(USMDB_UNIT_CURRENT,
                                &agentRouterOspfConfigGroupData.agentOspfDefaultMetric) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfDefaultMetricConfigured:
    if (snmpAgentOspfDefaultMetricConfiguredGet(USMDB_UNIT_CURRENT,
                 &agentRouterOspfConfigGroupData.agentOspfDefaultMetricConfigured) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfDefaultInfoOriginate:
    if (snmpAgentOspfDefaultInfoOriginateGet(USMDB_UNIT_CURRENT,
               &agentRouterOspfConfigGroupData.agentOspfDefaultInfoOriginate) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfDefaultInfoOriginateAlways:
    if (snmpAgentOspfDefaultInfoOriginateAlwaysGet(USMDB_UNIT_CURRENT,
             &agentRouterOspfConfigGroupData.agentOspfDefaultInfoOriginateAlways) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfDefaultInfoOriginateMetric:
    if (snmpAgentOspfDefaultRouteMetricGet(USMDB_UNIT_CURRENT,
           &agentRouterOspfConfigGroupData.agentOspfDefaultInfoOriginateMetric) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfDefaultInfoOriginateMetricConfigured:
    if (snmpAgentOspfDefaultInfoOrigMetricConfiguredGet(USMDB_UNIT_CURRENT,
         &agentRouterOspfConfigGroupData.agentOspfDefaultInfoOriginateMetricConfigured) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfDefaultInfoOriginateMetricType:
    if (snmpAgentOspfDefaultInfoOrigMetricTypeGet(USMDB_UNIT_CURRENT,
         &agentRouterOspfConfigGroupData.agentOspfDefaultInfoOriginateMetricType) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentRouterOspfRFC1583CompatibilityMode:
    if (snmpAgentRouterOspfRFC1583CompatibilityModeGet(USMDB_UNIT_CURRENT,
                                                       &agentRouterOspfConfigGroupData.agentRouterOspfRFC1583CompatibilityMode) == L7_SUCCESS)
      SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfSpfDelayTime:
    if (usmDbOspfspfDelayTimeGet(USMDB_UNIT_CURRENT, &agentRouterOspfConfigGroupData.agentOspfSpfDelayTime) == L7_SUCCESS)
      SET_VALID(nominator, agentRouterOspfConfigGroupData .valid);
    break;

  case I_agentOspfSpfHoldTime:
    if (usmDbOspfspfHoldTimeGet(USMDB_UNIT_CURRENT, &agentRouterOspfConfigGroupData.agentOspfSpfHoldTime) == L7_SUCCESS)
      SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;
  case I_agentOspfAutoCostRefBw:
    if (usmDbOspfAutoCostRefBwGet(&agentRouterOspfConfigGroupData.agentOspfAutoCostRefBw) == L7_SUCCESS)
      SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break; 
 case I_agentOspfOpaqueLsaSupport:
    if (snmpAgentOspfOpaqueLsaSupportGet(&agentRouterOspfConfigGroupData.agentOspfOpaqueLsaSupport) == L7_SUCCESS)
      SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfDefaultPassiveMode:
    if (snmpAgentOspfDefaultPassiveModeGet(USMDB_UNIT_CURRENT,
                 &agentRouterOspfConfigGroupData.agentOspfDefaultPassiveMode) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfRoutePrefIntraArea:
    if (snmpAgentOspfRoutePrefIntraAreaGet(USMDB_UNIT_CURRENT,
                 &agentRouterOspfConfigGroupData.agentOspfRoutePrefIntraArea) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfRoutePrefInterArea:
    if (snmpAgentOspfRoutePrefInterAreaGet(USMDB_UNIT_CURRENT,
                 &agentRouterOspfConfigGroupData.agentOspfRoutePrefInterArea) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

  case I_agentOspfRoutePrefExternal:
    if (snmpAgentOspfRoutePrefExternalGet(USMDB_UNIT_CURRENT,
                 &agentRouterOspfConfigGroupData.agentOspfRoutePrefExternal) == L7_SUCCESS)
    SET_VALID(nominator, agentRouterOspfConfigGroupData.valid);
    break;

default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentRouterOspfConfigGroupData.valid))
    return(NULL);

  return(&agentRouterOspfConfigGroupData);
}


#ifdef SETS
int
k_agentRouterOspfConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentRouterOspfConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentRouterOspfConfigGroup_set(agentRouterOspfConfigGroup_t *data,
                                 ContextInfo *contextInfo, int function)
{

   /* To check whether OSPF component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_OSPF_MAP_COMPONENT_ID) != L7_TRUE)
        return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentOspfDefaultMetric, data->valid) &&
      snmpAgentOspfDefaultMetricSet(USMDB_UNIT_CURRENT,
                                  data->agentOspfDefaultMetric) != L7_SUCCESS)
  {
      CLR_VALID(I_agentOspfDefaultMetric, data->valid);
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfDefaultMetricConfigured, data->valid) &&
      snmpAgentOspfDefaultMetricConfiguredSet(USMDB_UNIT_CURRENT,
                            data->agentOspfDefaultMetricConfigured)!= L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfDefaultMetricConfigured, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfDefaultInfoOriginate, data->valid) &&
      snmpAgentOspfDefaultInfoOriginateSet(USMDB_UNIT_CURRENT,
                                data->agentOspfDefaultInfoOriginate) != L7_SUCCESS)
  {
      CLR_VALID(I_agentOspfDefaultInfoOriginate, data->valid);
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfDefaultInfoOriginateAlways, data->valid) &&
      snmpAgentOspfDefaultInfoOriginateAlwaysSet(USMDB_UNIT_CURRENT,
                              data->agentOspfDefaultInfoOriginateAlways) != L7_SUCCESS)
  {
      CLR_VALID(I_agentOspfDefaultInfoOriginateAlways, data->valid);
      return COMMIT_FAILED_ERROR;
  }


  if (VALID(I_agentOspfDefaultInfoOriginateMetric, data->valid) &&
      snmpAgentOspfDefaultRouteMetricSet(USMDB_UNIT_CURRENT,
                                data->agentOspfDefaultInfoOriginateMetric) != L7_SUCCESS)
  {
      CLR_VALID(I_agentOspfDefaultInfoOriginateMetric, data->valid);
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfDefaultInfoOriginateMetricConfigured, data->valid) &&
    snmpAgentOspfDefaultInfoOrigMetricConfiguredSet(USMDB_UNIT_CURRENT,
                          data->agentOspfDefaultInfoOriginateMetricConfigured)!= L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfDefaultInfoOriginateMetricConfigured, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfDefaultInfoOriginateMetricType, data->valid) &&
      snmpAgentOspfDefaultInfoOrigMetricTypeSet(USMDB_UNIT_CURRENT,
                                data->agentOspfDefaultInfoOriginateMetricType) != L7_SUCCESS)
  {
      CLR_VALID(I_agentOspfDefaultInfoOriginateMetricType, data->valid);
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRouterOspfRFC1583CompatibilityMode, data->valid) &&
      snmpAgentRouterOspfRFC1583CompatibilityModeSet(USMDB_UNIT_CURRENT,
                                                     data->agentRouterOspfRFC1583CompatibilityMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterOspfRFC1583CompatibilityMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfSpfDelayTime, data->valid) &&
      usmDbOspfspfDelaySet(USMDB_UNIT_CURRENT, data->agentOspfSpfDelayTime) != L7_SUCCESS)
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentOspfSpfHoldTime, data->valid) &&
      usmDbOspfspfHoldTimeSet(USMDB_UNIT_CURRENT, data->agentOspfSpfHoldTime) != L7_SUCCESS)
    return(COMMIT_FAILED_ERROR);
  
  if (VALID(I_agentOspfAutoCostRefBw, data->valid) &&
      usmDbOspfAutoCostRefBwSet(USMDB_UNIT_CURRENT, data->agentOspfAutoCostRefBw) != L7_SUCCESS)
    return(COMMIT_FAILED_ERROR);
if (VALID(I_agentOspfOpaqueLsaSupport, data->valid) &&
      snmpAgentOspfOpaqueLsaSupportSet(data->agentOspfOpaqueLsaSupport) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfOpaqueLsaSupport, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfDefaultPassiveMode, data->valid) &&
      snmpAgentOspfDefaultPassiveModeSet(USMDB_UNIT_CURRENT,
                                         data->agentOspfDefaultPassiveMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfDefaultPassiveMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfRoutePrefIntraArea, data->valid) &&
      snmpAgentOspfRoutePrefIntraAreaSet(USMDB_UNIT_CURRENT,
                                         data->agentOspfRoutePrefIntraArea) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRoutePrefIntraArea, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfRoutePrefInterArea, data->valid) &&
      snmpAgentOspfRoutePrefInterAreaSet(USMDB_UNIT_CURRENT,
                                         data->agentOspfRoutePrefInterArea) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRoutePrefInterArea, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOspfRoutePrefExternal, data->valid) &&
      snmpAgentOspfRoutePrefExternalSet(USMDB_UNIT_CURRENT,
                                         data->agentOspfRoutePrefExternal) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOspfRoutePrefExternal, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_agentRouterOspfConfigGroup_UNDO
/* add #define SR_agentRouterOspfConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentRouterOspfConfigGroup family.
 */
int
agentRouterOspfConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentRouterOspfConfigGroup_UNDO */

#endif /* SETS */

agentECMPGroup_t *
k_agentECMPGroup_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
   static agentECMPGroup_t agentECMPGroupData;

   ZERO_VALID(agentECMPGroupData.valid);

   switch (nominator)
   {
   case -1:
     break;

   case I_agentECMPOspfMaxPaths :
     if (usmDbOspfMaxPathsGet(&agentECMPGroupData.agentECMPOspfMaxPaths) == L7_SUCCESS)
       SET_VALID(I_agentECMPOspfMaxPaths, agentECMPGroupData.valid);
     break;


   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, agentECMPGroupData.valid))
     return(NULL);

   return(&agentECMPGroupData);
}


#ifdef SETS
int
k_agentECMPGroup_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentECMPGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentECMPGroup_set(agentECMPGroup_t *data,
                     ContextInfo *contextInfo, int function)
{

    if (VALID(I_agentECMPOspfMaxPaths, data->valid) &&
     usmDbOspfMaxPathsSet(USMDB_UNIT_CURRENT, data->agentECMPOspfMaxPaths) != L7_SUCCESS)
   {
     CLR_VALID(I_agentECMPOspfMaxPaths, data->valid);
     return COMMIT_FAILED_ERROR;
   }
   return(NO_ERROR);
}

#ifdef SR_agentECMPGroup_UNDO
/* add #define SR_agentECMPGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentECMPGroup family.
 */
int
agentECMPGroup_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentECMPGroup_UNDO */

#endif /* SETS */


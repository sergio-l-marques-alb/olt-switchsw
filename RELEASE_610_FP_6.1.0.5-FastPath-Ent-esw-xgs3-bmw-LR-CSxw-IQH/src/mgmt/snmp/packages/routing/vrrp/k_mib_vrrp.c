/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_vrrp.c
*
* @purpose System specific code for VRRP MIBs
*
* @component SNMP
*
* @create 10/26/2001
*
* @author Kumar Manish 
*
* @end
*             
*********************************************************************/ 

/*********************************************************************
                         
*********************************************************************/

#include <k_private_base.h>
#include "k_mib_vrrp_api.h"
#include "usmdb_common.h"
#include "usmdb_mib_vrrp_api.h"
#include "usmdb_util_api.h"

vrrpOperations_t *
k_vrrpOperations_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
  static vrrpOperations_t vrrpOperationsData;

   /* To check whether VRRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
                 return(NULL);
  
  ZERO_VALID(vrrpOperationsData.valid);
  
  switch(nominator)
  {
  
  case -1:
    break;
  
  case I_vrrpNodeVersion:
    CLR_VALID(I_vrrpNodeVersion, vrrpOperationsData.valid);
    if (usmDbVrrpNodeVersionGet(USMDB_UNIT_CURRENT, &vrrpOperationsData.vrrpNodeVersion) == L7_SUCCESS)
      SET_VALID(I_vrrpNodeVersion, vrrpOperationsData.valid);
    break;
  
  case I_vrrpNotificationCntl:
    CLR_VALID(I_vrrpNotificationCntl, vrrpOperationsData.valid);
    if (snmpVrrpNotificationCntlGet(USMDB_UNIT_CURRENT, &vrrpOperationsData.vrrpNotificationCntl) == L7_SUCCESS)
      SET_VALID(I_vrrpNotificationCntl, vrrpOperationsData.valid);
    break;

  default:
    return(NULL);
    break;
  }
   
  if (nominator >= 0 && !VALID(nominator, vrrpOperationsData.valid))
    return(NULL);

  return(&vrrpOperationsData);

}

#ifdef SETS
int
k_vrrpOperations_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_vrrpOperations_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_vrrpOperations_set(vrrpOperations_t *data,
                     ContextInfo *contextInfo, int function)
{
   /* To check whether VRRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
                 return(COMMIT_FAILED_ERROR);

  if (VALID(I_vrrpNotificationCntl, data->valid) && 
      snmpVrrpNotificationCntlSet(USMDB_UNIT_CURRENT, data->vrrpNotificationCntl) != L7_SUCCESS)
  {
     CLR_VALID(I_vrrpNotificationCntl, data->valid);
     return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}


#ifdef SR_vrrpOperations_UNDO
/* add #define SR_vrrpOperations_UNDO in sitedefs.h to
 * include the undo routine for the vrrpOperations family.
 */
int
vrrpOperations_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_vrrpOperations_UNDO */

#endif /* SETS */

vrrpOperEntry_t *
k_vrrpOperEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 ifIndex,
                    SR_INT32 vrrpOperVrId)
{
  static vrrpOperEntry_t vrrpOperEntryData;

  char snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 routerUpTimeinSec=0;
  L7_uint32 intIfNum;
  static L7_BOOL firstTime = L7_TRUE;

   /* To check whether VRRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    vrrpOperEntryData.vrrpOperVirtualMacAddr = MakeOctetString(NULL, 0);
    vrrpOperEntryData.vrrpOperAuthKey = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(vrrpOperEntryData.valid);
  vrrpOperEntryData.ifIndex = ifIndex;
  SET_VALID(I_vrrpOperEntryIndex_ifIndex, vrrpOperEntryData.valid);
  vrrpOperEntryData.vrrpOperVrId = vrrpOperVrId;
  SET_VALID(I_vrrpOperVrId, vrrpOperEntryData.valid);

  if (((searchType == EXACT) ?
       (snmpVrrpOperEntryGet(USMDB_UNIT_CURRENT, 
                             vrrpOperEntryData.vrrpOperVrId,
                             vrrpOperEntryData.ifIndex) != L7_SUCCESS) :
       ((snmpVrrpOperEntryGet(USMDB_UNIT_CURRENT,
                              vrrpOperEntryData.vrrpOperVrId,
                              vrrpOperEntryData.ifIndex) != L7_SUCCESS) && 
        (snmpVrrpOperEntryNextGet(USMDB_UNIT_CURRENT, 
                                  vrrpOperEntryData.vrrpOperVrId,
                                  vrrpOperEntryData.ifIndex, 
                                  &vrrpOperEntryData.vrrpOperVrId, 
                                  &vrrpOperEntryData.ifIndex) != L7_SUCCESS)))
      || (usmDbIntIfNumFromExtIfNum(vrrpOperEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(vrrpOperEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_vrrpOperEntryIndex_ifIndex:
  case I_vrrpOperVrId:
     break;

  case I_vrrpOperVirtualMacAddr:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbVrrpOperVirtualMacAddrGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                        intIfNum, snmp_buffer) == L7_SUCCESS)
        &&(SafeMakeOctetString(&vrrpOperEntryData.vrrpOperVirtualMacAddr, 
                               snmp_buffer,L7_MAC_ADDR_LEN) == L7_TRUE ))
      SET_VALID(I_vrrpOperVirtualMacAddr, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperState:
    if (snmpVrrpOperStateGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                             intIfNum, &vrrpOperEntryData.vrrpOperState) == L7_SUCCESS)
      SET_VALID(I_vrrpOperState, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperAdminState:
    if (snmpVrrpOperAdminStateGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                  intIfNum, &vrrpOperEntryData.vrrpOperAdminState) == L7_SUCCESS)
      SET_VALID(I_vrrpOperAdminState, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperPriority:
    if (snmpVrrpOperPriorityGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                intIfNum, &vrrpOperEntryData.vrrpOperPriority) == L7_SUCCESS)
      SET_VALID(I_vrrpOperPriority, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperIpAddrCount:
    if (snmpVrrpOperIpAddrCountGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                   intIfNum, &vrrpOperEntryData.vrrpOperIpAddrCount) == L7_SUCCESS)
      SET_VALID(I_vrrpOperIpAddrCount, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperMasterIpAddr:
    if (snmpVrrpOperMasterIpAddrGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                    intIfNum, &vrrpOperEntryData.vrrpOperMasterIpAddr) == L7_SUCCESS)
      SET_VALID(I_vrrpOperMasterIpAddr, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperPrimaryIpAddr:
    if (usmDbVrrpIpAddressNextGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                          intIfNum,0, &vrrpOperEntryData.vrrpOperPrimaryIpAddr) == L7_SUCCESS)
      SET_VALID(I_vrrpOperPrimaryIpAddr, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperAuthType:
    if (snmpVrrpOperAuthTypeGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                intIfNum, &vrrpOperEntryData.vrrpOperAuthType) == L7_SUCCESS)
      SET_VALID(I_vrrpOperAuthType, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperAuthKey:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbVrrpOperAuthKeyGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                 intIfNum,snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&vrrpOperEntryData.vrrpOperAuthKey, snmp_buffer) == L7_TRUE ))
      SET_VALID(I_vrrpOperAuthKey, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperAdvertisementInterval:
    if (snmpVrrpOperAdvertisementIntervalGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                             intIfNum, &vrrpOperEntryData.vrrpOperAdvertisementInterval) == L7_SUCCESS)
      SET_VALID(I_vrrpOperAdvertisementInterval, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperPreemptMode:
    if (snmpVrrpOperPreemptModeGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                   intIfNum, &vrrpOperEntryData.vrrpOperPreemptMode) == L7_SUCCESS)
      SET_VALID(I_vrrpOperPreemptMode, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperVirtualRouterUpTime:
    if (snmpVrrpOperUpTimeGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                              intIfNum, &routerUpTimeinSec) == L7_SUCCESS)
    {
      vrrpOperEntryData.vrrpOperVirtualRouterUpTime= (100 * routerUpTimeinSec); 
      SET_VALID(I_vrrpOperVirtualRouterUpTime, vrrpOperEntryData.valid);
    }
    break;

  case I_vrrpOperProtocol:
    if (snmpVrrpOperProtocolGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                intIfNum, &vrrpOperEntryData.vrrpOperProtocol) == L7_SUCCESS)
      SET_VALID(I_vrrpOperProtocol, vrrpOperEntryData.valid);
    break;

  case I_vrrpOperRowStatus:
    if (usmDbVrrpOperRowStatusGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                  intIfNum, &vrrpOperEntryData.vrrpOperRowStatus) == L7_SUCCESS)
      SET_VALID(I_vrrpOperRowStatus, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsBecomeMaster:
    if (usmDbVrrpStatsBecomeMaster(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                   intIfNum, &vrrpOperEntryData.vrrpStatsBecomeMaster) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsBecomeMaster, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsAdvertiseRcvd:
    if (usmDbVrrpStatsAdvertiseRcvd(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                    intIfNum, &vrrpOperEntryData.vrrpStatsAdvertiseRcvd) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsAdvertiseRcvd, vrrpOperEntryData.valid);
    break;
  case I_vrrpStatsAdvertiseIntervalErrors:
    if (usmDbVrrpStatsAdvertiseIntervalErrorsGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                                 intIfNum, &vrrpOperEntryData.vrrpStatsAdvertiseIntervalErrors) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsAdvertiseIntervalErrors, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsAuthFailures:
    if (usmDbVrrpStatsAuthFailuresGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                      intIfNum, &vrrpOperEntryData.vrrpStatsAuthFailures) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsAuthFailures, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsIpTtlErrors:
    if (usmDbVrrpStatsIpTTLFailedGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                     intIfNum, &vrrpOperEntryData.vrrpStatsIpTtlErrors) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsIpTtlErrors, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsPriorityZeroPktsRcvd:
    if (usmDbVrrpStatsPriorityZeroPktsRcvdGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                              intIfNum, &vrrpOperEntryData.vrrpStatsPriorityZeroPktsRcvd) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsPriorityZeroPktsRcvd, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsPriorityZeroPktsSent:
    if (usmDbVrrpStatsPriorityZeroPktsSentGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                              intIfNum, &vrrpOperEntryData.vrrpStatsPriorityZeroPktsSent) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsPriorityZeroPktsSent, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsInvalidTypePktsRcvd:
    if (usmDbVrrpStatsInvalidTypePktsRcvdGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                             intIfNum, &vrrpOperEntryData.vrrpStatsInvalidTypePktsRcvd) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsInvalidTypePktsRcvd, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsAddressListErrors:
    if (usmDbVrrpStatsAddressListErrorsGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                           intIfNum, &vrrpOperEntryData.vrrpStatsAddressListErrors) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsAddressListErrors, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsInvalidAuthType:
    if (usmDbVrrpStatsInvalidAuthTypeGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                         intIfNum, &vrrpOperEntryData.vrrpStatsInvalidAuthType) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsInvalidAuthType, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsAuthTypeMismatch:
    if (usmDbVrrpStatsAuthTypeMismatchGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                          intIfNum, &vrrpOperEntryData.vrrpStatsAuthTypeMismatch) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsAuthTypeMismatch, vrrpOperEntryData.valid);
    break;

  case I_vrrpStatsPacketLengthErrors:
    if (usmDbVrrpStatsPktLengthGet(USMDB_UNIT_CURRENT, vrrpOperEntryData.vrrpOperVrId,
                                   intIfNum, &vrrpOperEntryData.vrrpStatsPacketLengthErrors) == L7_SUCCESS)
      SET_VALID(I_vrrpStatsPacketLengthErrors, vrrpOperEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }  /* end of switch(nominator)  */

  if (nominator >= 0 && !VALID(nominator, vrrpOperEntryData.valid))
    return(NULL);

  return(&vrrpOperEntryData);
}

#ifdef SETS
int
k_vrrpOperEntry_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_vrrpOperEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_vrrpOperEntry_set_defaults(doList_t *dp)
{
  vrrpOperEntry_t *data = (vrrpOperEntry_t *) (dp->data);

  if ((data->vrrpOperVirtualMacAddr = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->vrrpOperState =  D_vrrpOperState_initialize;
  data->vrrpOperAdminState = D_vrrpOperAdminState_down;
  data->vrrpOperPriority = 100;
  data->vrrpOperIpAddrCount = 1;
  data->vrrpOperMasterIpAddr =  (SR_UINT32) 0;
  data->vrrpOperPrimaryIpAddr =  (SR_UINT32) 0;
  data->vrrpOperAuthType = D_vrrpOperAuthType_noAuthentication;
  if ((data->vrrpOperAuthKey = MakeOctetStringFromText("")) == 0) 
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->vrrpOperAdvertisementInterval = 1;
  data->vrrpOperPreemptMode = D_vrrpOperPreemptMode_true;
  data->vrrpOperVirtualRouterUpTime = (SR_UINT32) 0;
  data->vrrpOperProtocol = D_vrrpOperProtocol_ip;
  data->vrrpOperRowStatus = D_vrrpOperRowStatus_active; 

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_vrrpOperEntry_set(vrrpOperEntry_t *data,
                    ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

  char snmp_buffer[SNMP_BUFFER_LEN];

   /* To check whether VRRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
        return(COMMIT_FAILED_ERROR);

  /* Convert the ifIndex to internal interface number */
  if (usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return(WRONG_VALUE_ERROR);
  }

  /* If entry doesn't exist, then try to create it */
  if (snmpVrrpOperEntryGet(USMDB_UNIT_CURRENT, data->vrrpOperVrId, data->ifIndex) != L7_SUCCESS)
  {
    /* creation is only allowed when setting row status to createAndGo or createAndWait */
    if (VALID(I_vrrpOperRowStatus, data->valid) && 
        (data->vrrpOperRowStatus == D_vrrpOperRowStatus_createAndGo ||
         data->vrrpOperRowStatus == D_vrrpOperRowStatus_createAndWait))
    {
      if (usmDbVrrpRouterIdCreate(USMDB_UNIT_CURRENT, data->vrrpOperVrId, intIfNum) != L7_SUCCESS)
        return(NO_CREATION_ERROR);

      CLR_VALID(I_vrrpOperRowStatus, data->valid);
    }
    else
    {
      return(NO_CREATION_ERROR);
    }
  }
  
  if (VALID(I_vrrpOperAdminState, data->valid) &&
      (snmpVrrpOperAdminStateSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                 intIfNum,data->vrrpOperAdminState) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);
  

  if (VALID(I_vrrpOperPriority, data->valid) &&
      (snmpVrrpOperPrioritySet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                               intIfNum,data->vrrpOperPriority) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);
    
  if (VALID(I_vrrpOperPrimaryIpAddr, data->valid) &&
      (usmDbVrrpAssocIpAddress(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                    intIfNum,data->vrrpOperPrimaryIpAddr) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

  if (VALID(I_vrrpOperAuthType, data->valid) &&
      (snmpVrrpOperAuthTypeSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                               intIfNum,data->vrrpOperAuthType) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

  if (data->vrrpOperAuthKey != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->vrrpOperAuthKey->octet_ptr, data->vrrpOperAuthKey->length);
    if (VALID(I_vrrpOperAuthKey, data->valid) &&
        (snmpVrrpOperAuthKeySet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                intIfNum,snmp_buffer) != L7_SUCCESS))
        return(COMMIT_FAILED_ERROR);
  }
 
  if (VALID(I_vrrpOperAdvertisementInterval, data->valid) &&
      (usmDbVrrpOperAdvertisementIntervalSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                             intIfNum,data->vrrpOperAdvertisementInterval) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);
  
  if (VALID(I_vrrpOperPreemptMode, data->valid) &&
      (snmpVrrpOperPreemptModeSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                  intIfNum,data->vrrpOperPreemptMode) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);
  
  if (VALID(I_vrrpOperProtocol, data->valid) &&
      (snmpVrrpOperProtocolSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                intIfNum,data->vrrpOperProtocol) != L7_SUCCESS))
      return(COMMIT_FAILED_ERROR);

  if (VALID(I_vrrpOperRowStatus, data->valid) &&
       (snmpVrrpOperRowStatusSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                 intIfNum,data->vrrpOperRowStatus) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);

  return(NO_ERROR);
}

#ifdef SR_vrrpOperEntry_UNDO
/* add #define SR_vrrpOperEntry_UNDO in sitedefs.h to
 * include the undo routine for the vrrpOperEntry family.
 */
int
vrrpOperEntry_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_vrrpOperEntry_UNDO */

#endif /* SETS */

vrrpAssoIpAddrEntry_t *
k_vrrpAssoIpAddrEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 ifIndex,
                          SR_INT32 vrrpOperVrId,
                          SR_UINT32 vrrpAssoIpAddr)
{
  static vrrpAssoIpAddrEntry_t vrrpAssoIpAddrEntryData;
  L7_uint32 intIfNum;

   /* To check whether VRRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE
    */
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  ZERO_VALID(vrrpAssoIpAddrEntryData.valid);

  vrrpAssoIpAddrEntryData.vrrpOperVrId = vrrpOperVrId;
  vrrpAssoIpAddrEntryData.ifIndex = ifIndex;
  vrrpAssoIpAddrEntryData.vrrpAssoIpAddr = vrrpAssoIpAddr;

  SET_VALID(I_vrrpAssoIpAddrEntryIndex_vrrpOperVrId, vrrpAssoIpAddrEntryData.valid);
  SET_VALID(I_vrrpAssoIpAddrEntryIndex_ifIndex, vrrpAssoIpAddrEntryData.valid);
  SET_VALID(I_vrrpAssoIpAddr, vrrpAssoIpAddrEntryData.valid);

  if (((searchType == EXACT) ?
       (snmpVrrpAssoIpAddrEntryGet(USMDB_UNIT_CURRENT, 
                                   vrrpAssoIpAddrEntryData.vrrpOperVrId,
                                   vrrpAssoIpAddrEntryData.ifIndex,
                                   vrrpAssoIpAddrEntryData.vrrpAssoIpAddr) != L7_SUCCESS) :
       ((snmpVrrpAssoIpAddrEntryGet(USMDB_UNIT_CURRENT, 
                                    vrrpAssoIpAddrEntryData.vrrpOperVrId,
                                    vrrpAssoIpAddrEntryData.ifIndex,
                                    vrrpAssoIpAddrEntryData.vrrpAssoIpAddr) != L7_SUCCESS) && 
        (snmpVrrpAssoIpAddrEntryNextGet(USMDB_UNIT_CURRENT,
                                        vrrpAssoIpAddrEntryData.vrrpOperVrId,
                                        vrrpAssoIpAddrEntryData.ifIndex,
                                        vrrpAssoIpAddrEntryData.vrrpAssoIpAddr, 
                                        &vrrpAssoIpAddrEntryData.vrrpOperVrId,
                                        &vrrpAssoIpAddrEntryData.ifIndex,
                                        &vrrpAssoIpAddrEntryData.vrrpAssoIpAddr) != L7_SUCCESS)))
      || (usmDbIntIfNumFromExtIfNum(vrrpAssoIpAddrEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(vrrpAssoIpAddrEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
    case -1:
      /* fall through when nominator is -1 */
    
    case I_vrrpAssoIpAddrRowStatus:
      vrrpAssoIpAddrEntryData.vrrpAssoIpAddrRowStatus = D_vrrpAssoIpAddrRowStatus_active;
      SET_VALID(I_vrrpAssoIpAddrRowStatus, vrrpAssoIpAddrEntryData.valid);
      break;

    default:
      return(NULL);
      break;
    
  }/* end of switch(nominator)  */

  if (nominator >= 0 && !VALID(nominator, vrrpAssoIpAddrEntryData.valid))
    return(NULL);

  return(&vrrpAssoIpAddrEntryData);
}

#ifdef SETS
int
k_vrrpAssoIpAddrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_vrrpAssoIpAddrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_vrrpAssoIpAddrEntry_set_defaults(doList_t *dp)
{
  vrrpAssoIpAddrEntry_t *data = (vrrpAssoIpAddrEntry_t *) (dp->data);
  
  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_vrrpAssoIpAddrEntry_set(vrrpAssoIpAddrEntry_t *data,
                          ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_BOOL isNewRowCreated = L7_FALSE;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  /* To check whether VRRP component is present or not.
   * If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE
   */
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
    return(COMMIT_FAILED_ERROR);

  if (usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum) != L7_SUCCESS)
    return (COMMIT_FAILED_ERROR);

  if ((snmpVrrpAssoIpAddrEntryGet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                  data->ifIndex, data->vrrpAssoIpAddr) != L7_SUCCESS))
      
  {
    if (VALID(I_vrrpAssoIpAddrRowStatus, data->valid) && 
         ((data->vrrpAssoIpAddrRowStatus == D_vrrpAssoIpAddrRowStatus_createAndGo)|| 
          (data->vrrpAssoIpAddrRowStatus == D_vrrpAssoIpAddrRowStatus_createAndWait)))
    {
      if ((usmDbVrrpAssocSecondaryIpAddress(USMDB_UNIT_CURRENT,data->vrrpOperVrId,
                                   intIfNum,data->vrrpAssoIpAddr) != L7_SUCCESS))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR); 
      }
      else
      {
        isNewRowCreated = L7_TRUE;
      }
    }
  }

  if (VALID(I_vrrpAssoIpAddrRowStatus, data->valid) && (isNewRowCreated != L7_TRUE))
  {
    if(((data->vrrpAssoIpAddrRowStatus == D_vrrpAssoIpAddrRowStatus_destroy || 
         data->vrrpAssoIpAddrRowStatus == D_vrrpAssoIpAddrRowStatus_notInService) &&
        usmDbVrrpDissocSecondaryIpAddress(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                   intIfNum, data->vrrpAssoIpAddr) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
   
  return(NO_ERROR);
}

#ifdef SR_vrrpAssoIpAddrEntry_UNDO
/* add #define SR_vrrpAssoIpAddrEntry_UNDO in sitedefs.h to
 * include the undo routine for the vrrpAssoIpAddrEntry family.
 */
int
vrrpAssoIpAddrEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_vrrpAssoIpAddrEntry_UNDO */

#endif /* SETS */

vrrpStatistics_t *
k_vrrpStatistics_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
  static vrrpStatistics_t vrrpStatisticsData;

   /* To check whether VRRP component is present or not.
      If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
        return(NULL);

  switch (nominator)
  {
  case -1:
    break;

  case I_vrrpRouterChecksumErrors:
    if (usmDbVrrpRouterChecksumErrorGet(USMDB_UNIT_CURRENT, &vrrpStatisticsData.vrrpRouterChecksumErrors) == L7_SUCCESS)
      SET_VALID(I_vrrpRouterChecksumErrors, vrrpStatisticsData.valid);
    break;

  case I_vrrpRouterVersionErrors:
    if (usmDbVrrpRouterVerErrorGet(USMDB_UNIT_CURRENT, &vrrpStatisticsData.vrrpRouterVersionErrors) == L7_SUCCESS)
      SET_VALID(I_vrrpRouterVersionErrors, vrrpStatisticsData.valid);
    break;

  case I_vrrpRouterVrIdErrors:
    if (usmDbVrrpRouterVridErrorGet(USMDB_UNIT_CURRENT, &vrrpStatisticsData.vrrpRouterVrIdErrors) == L7_SUCCESS)
      SET_VALID(I_vrrpRouterVrIdErrors, vrrpStatisticsData.valid);
    break;

  default:
    return(NULL);
    break;
  } /* end of switch(nominator)  */

  if (nominator >= 0 && !VALID(nominator, vrrpStatisticsData.valid))
    return(NULL);

  return(&vrrpStatisticsData);
}

/*******************************************************************************/

agentRouterVrrpConfigGroup_t *
k_agentRouterVrrpConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator)
{
  static agentRouterVrrpConfigGroup_t agentRouterVrrpConfigGroupData;
  ZERO_VALID(agentRouterVrrpConfigGroupData.valid);

  switch(nominator)
  {
   case -1:
     break;

   case I_agentRouterVrrpAdminState:
     if (snmpAgentVrrpAdminStateGet(&agentRouterVrrpConfigGroupData.agentRouterVrrpAdminState) 
                                                                             == L7_SUCCESS)
      SET_VALID(I_agentRouterVrrpAdminState,agentRouterVrrpConfigGroupData.valid);
     break;
   
   default:
     /* unknown nominator */
     return(NULL);
     break;   
  }
  
  if (nominator >= 0 && !VALID(nominator, agentRouterVrrpConfigGroupData.valid))
     return(NULL);

  return(&agentRouterVrrpConfigGroupData);
}

#ifdef SETS
int
k_agentRouterVrrpConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentRouterVrrpConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentRouterVrrpConfigGroup_set(agentRouterVrrpConfigGroup_t *data,
                                 ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentRouterVrrpAdminState, data->valid) &&
    snmpAgentVrrpAdminStateSet(data->agentRouterVrrpAdminState) != L7_SUCCESS)
  {
    CLR_VALID(I_agentRouterVrrpAdminState, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  return(NO_ERROR);
}

#ifdef SR_agentRouterVrrpConfigGroup_UNDO
/* add #define SR_agentRouterVrrpConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentRouterVrrpConfigGroup family.
 */
int
agentRouterVrrpConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentRouterVrrpConfigGroup_UNDO */

#endif /* SETS */

agentSnmpTrapFlagsConfigGroupLayer3_t *
k_agentSnmpTrapFlagsConfigGroupLayer3_get(int serialNum, ContextInfo *contextInfo,
                                          int nominator)
{
   static agentSnmpTrapFlagsConfigGroupLayer3_t agentSnmpTrapFlagsConfigGroupLayer3Data;

   ZERO_VALID(agentSnmpTrapFlagsConfigGroupLayer3Data.valid);

   switch (nominator)
   {
   case -1:
     break;

   case I_agentSnmpVRRPNewMasterTrapFlag :
     if (snmpAgentSnmpVrrpNewMasterTrapFlagGet(USMDB_UNIT_CURRENT, &agentSnmpTrapFlagsConfigGroupLayer3Data.agentSnmpVRRPNewMasterTrapFlag) == L7_SUCCESS)
       SET_VALID(I_agentSnmpVRRPNewMasterTrapFlag, agentSnmpTrapFlagsConfigGroupLayer3Data.valid);
     break;

   case I_agentSnmpVRRPAuthFailureTrapFlag :
     if (snmpAgentSnmpVrrpAuthFailureTrapFlagGet(USMDB_UNIT_CURRENT, &agentSnmpTrapFlagsConfigGroupLayer3Data.agentSnmpVRRPAuthFailureTrapFlag) == L7_SUCCESS)
       SET_VALID(I_agentSnmpVRRPAuthFailureTrapFlag, agentSnmpTrapFlagsConfigGroupLayer3Data.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if ( nominator >= 0 && !VALID(nominator, agentSnmpTrapFlagsConfigGroupLayer3Data.valid) )
     return(NULL);

   return(&agentSnmpTrapFlagsConfigGroupLayer3Data);
}

#ifdef SETS
int
k_agentSnmpTrapFlagsConfigGroupLayer3_test(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSnmpTrapFlagsConfigGroupLayer3_ready(ObjectInfo *object, ObjectSyntax *value,
                                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSnmpTrapFlagsConfigGroupLayer3_set(agentSnmpTrapFlagsConfigGroupLayer3_t *data,
                                          ContextInfo *contextInfo, int function)
{
  if ( VALID(I_agentSnmpVRRPNewMasterTrapFlag, data->valid) &&
       snmpAgentSnmpVrrpNewMasterTrapFlagSet(USMDB_UNIT_CURRENT,
                                             data->agentSnmpVRRPNewMasterTrapFlag) != L7_SUCCESS )
    return(COMMIT_FAILED_ERROR);

  if ( VALID(I_agentSnmpVRRPAuthFailureTrapFlag, data->valid) &&
       snmpAgentSnmpVrrpAuthFailureTrapFlagSet(USMDB_UNIT_CURRENT,
                                               data->agentSnmpVRRPAuthFailureTrapFlag) != L7_SUCCESS )
    return(COMMIT_FAILED_ERROR);

  return NO_ERROR;
}

#ifdef SR_agentSnmpTrapFlagsConfigGroupLayer3_UNDO
/* add #define SR_agentSnmpTrapFlagsConfigGroupLayer3_UNDO in sitedefs.h to
 * include the undo routine for the agentSnmpTrapFlagsConfigGroupLayer3 family.
 */
int
agentSnmpTrapFlagsConfigGroupLayer3_undo(doList_t *doHead, doList_t *doCur,
                                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSnmpTrapFlagsConfigGroupLayer3_UNDO */

#endif /* SETS */

/* To get the Operational priority */
agentRouterVrrpOperEntry_t *
k_agentRouterVrrpOperEntry_get(int serialNum, ContextInfo *contextInfo, 
                              int nominator, int searchType, 
                              SR_INT32 ifIndex,
                              SR_INT32 vrrpOperVrId)
{
    return(NULL); /* Obsolete */
}

/* To get the Configured Priority */
agentRouterVrrpConfiguredEntry_t *
k_agentRouterVrrpConfiguredEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator, int searchType,
                              SR_INT32 ifIndex,
                              SR_INT32 vrrpOperVrId)
{
  static agentRouterVrrpConfiguredEntry_t agentRouterVrrpConfiguredEntryData;

  L7_uint32 intIfNum;

  /* To check whether VRRP component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
    return(NULL);

  ZERO_VALID(agentRouterVrrpConfiguredEntryData.valid);
  agentRouterVrrpConfiguredEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentRouterVrrpConfiguredEntryIndex_ifIndex, agentRouterVrrpConfiguredEntryData.valid);
  agentRouterVrrpConfiguredEntryData.vrrpOperVrId = vrrpOperVrId;
  SET_VALID(I_agentRouterVrrpConfiguredEntryIndex_vrrpOperVrId, agentRouterVrrpConfiguredEntryData.valid);

  if (((searchType == EXACT) ?
        (snmpVrrpOperEntryGet(USMDB_UNIT_CURRENT,
                              agentRouterVrrpConfiguredEntryData.vrrpOperVrId,
                              agentRouterVrrpConfiguredEntryData.ifIndex) != L7_SUCCESS) :
        ((snmpVrrpOperEntryGet(USMDB_UNIT_CURRENT,
                               agentRouterVrrpConfiguredEntryData.vrrpOperVrId,
                               agentRouterVrrpConfiguredEntryData.ifIndex) != L7_SUCCESS) &&
         (snmpVrrpOperEntryNextGet(USMDB_UNIT_CURRENT,
                                   agentRouterVrrpConfiguredEntryData.vrrpOperVrId,
                                   agentRouterVrrpConfiguredEntryData.ifIndex,
                                   &agentRouterVrrpConfiguredEntryData.vrrpOperVrId,
                                   &agentRouterVrrpConfiguredEntryData.ifIndex) != L7_SUCCESS)))
      || (usmDbIntIfNumFromExtIfNum(agentRouterVrrpConfiguredEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentRouterVrrpConfiguredEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
    case -1:
    case I_agentRouterVrrpConfiguredEntryIndex_ifIndex:
    case I_agentRouterVrrpConfiguredEntryIndex_vrrpOperVrId:
      break;

    case I_agentRouterVrrpConfiguredPriority:
      if (snmpVrrpConfigPriorityGet(USMDB_UNIT_CURRENT, agentRouterVrrpConfiguredEntryData.vrrpOperVrId,
                                   intIfNum, &agentRouterVrrpConfiguredEntryData.agentRouterVrrpConfiguredPriority)
                                   == L7_SUCCESS)
        SET_VALID(I_agentRouterVrrpConfiguredPriority, agentRouterVrrpConfiguredEntryData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentRouterVrrpConfiguredEntryData.valid) )
    return(NULL);

  return(&agentRouterVrrpConfiguredEntryData);
}


agentRouterVrrpTrackIntfEntry_t *
k_agentRouterVrrpTrackIntfEntry_get(int serialNum, ContextInfo *contextInfo, int nominator, 
                                    int searchType, 
                                    SR_INT32 ifIndex, 
                                    SR_INT32 vrrpOperVrId,
                                    SR_INT32 agentRouterVrrpTrackIntf)
{
  static agentRouterVrrpTrackIntfEntry_t agentRouterVrrpTrackIntfEntryData;

  L7_uint32 intIfNum;

  /* To check whether VRRP component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
    return(NULL);

  ZERO_VALID(agentRouterVrrpTrackIntfEntryData.valid);

  agentRouterVrrpTrackIntfEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentRouterVrrpTrackIntfEntryIndex_ifIndex, agentRouterVrrpTrackIntfEntryData.valid);
  
  agentRouterVrrpTrackIntfEntryData.vrrpOperVrId = vrrpOperVrId;
  SET_VALID(I_agentRouterVrrpTrackIntfEntryIndex_vrrpOperVrId, agentRouterVrrpTrackIntfEntryData.valid);

  agentRouterVrrpTrackIntfEntryData.agentRouterVrrpTrackIntf = agentRouterVrrpTrackIntf;
  SET_VALID(I_agentRouterVrrpTrackIntf,agentRouterVrrpTrackIntfEntryData.valid);

  if (((searchType == EXACT) ?
        (snmpVrrpTrackIntfEntryGet(USMDB_UNIT_CURRENT,
                              agentRouterVrrpTrackIntfEntryData.vrrpOperVrId,
                              agentRouterVrrpTrackIntfEntryData.ifIndex,
                              agentRouterVrrpTrackIntfEntryData.agentRouterVrrpTrackIntf) != L7_SUCCESS) :
        ((snmpVrrpTrackIntfEntryGet(USMDB_UNIT_CURRENT,
                               agentRouterVrrpTrackIntfEntryData.vrrpOperVrId,
                               agentRouterVrrpTrackIntfEntryData.ifIndex,
                               agentRouterVrrpTrackIntfEntryData.agentRouterVrrpTrackIntf) != L7_SUCCESS) &&
         (snmpVrrpTrackIntfEntryNextGet(USMDB_UNIT_CURRENT,
                                   &agentRouterVrrpTrackIntfEntryData.vrrpOperVrId,
                                   &agentRouterVrrpTrackIntfEntryData.ifIndex,
                                   &agentRouterVrrpTrackIntfEntryData.agentRouterVrrpTrackIntf)
                                    != L7_SUCCESS)))
      || (usmDbIntIfNumFromExtIfNum(agentRouterVrrpTrackIntfEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentRouterVrrpTrackIntfEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
    case -1:
    case I_agentRouterVrrpTrackIntfEntryIndex_ifIndex:
    case I_agentRouterVrrpTrackIntfEntryIndex_vrrpOperVrId:
    case I_agentRouterVrrpTrackIntf:
      break;

   case I_agentRouterVrrpTrackIfPrioDec:
      
       if (snmpVrrpOperTrackIntfPrioGet(USMDB_UNIT_CURRENT,
             agentRouterVrrpTrackIntfEntryData.vrrpOperVrId,intIfNum,
             agentRouterVrrpTrackIntfEntryData.agentRouterVrrpTrackIntf,
             &agentRouterVrrpTrackIntfEntryData.agentRouterVrrpTrackIfPrioDec) == L7_SUCCESS)
        SET_VALID(I_agentRouterVrrpTrackIfPrioDec, agentRouterVrrpTrackIntfEntryData.valid);
      break;
   case I_agentRouterVrrpTrackIfState:
       if (snmpVrrpOperTrackIntfStateGet(USMDB_UNIT_CURRENT,
                                         agentRouterVrrpTrackIntfEntryData.vrrpOperVrId,intIfNum,
                                         agentRouterVrrpTrackIntfEntryData.agentRouterVrrpTrackIntf,
                                         &agentRouterVrrpTrackIntfEntryData.agentRouterVrrpTrackIfState) == L7_SUCCESS)
        SET_VALID(I_agentRouterVrrpTrackIfState, agentRouterVrrpTrackIntfEntryData.valid);
      break;
   case I_agentRouterVrrpTrackIfStatus:
        agentRouterVrrpTrackIntfEntryData.agentRouterVrrpTrackIfStatus =        
          D_agentRouterVrrpTrackIfStatus_active;
        SET_VALID(I_agentRouterVrrpTrackIfStatus, agentRouterVrrpTrackIntfEntryData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

   if ( nominator >= 0 && !VALID(nominator, agentRouterVrrpTrackIntfEntryData.valid) )
         return(NULL);

     return(&agentRouterVrrpTrackIntfEntryData);

}

int
k_agentRouterVrrpTrackIntfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                               doList_t *dp, ContextInfo *contextInfo)
{

      return NO_ERROR;
}

int 
k_agentRouterVrrpTrackIntfEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
      return NO_ERROR;
}

  int
k_agentRouterVrrpTrackIntfEntry_set_defaults(doList_t *dp)
{
    agentRouterVrrpTrackIntfEntry_t *data = (agentRouterVrrpTrackIntfEntry_t *) (dp->data);
    data->agentRouterVrrpTrackIfPrioDec = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;

      ZERO_VALID(data->valid);
        return NO_ERROR;
}

int
k_agentRouterVrrpTrackIntfEntry_set(agentRouterVrrpTrackIntfEntry_t *data,
                                   ContextInfo *contextInfo,
                                  int function)
{
  L7_uint32 intIfNum;

  /* To check whether VRRP component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
    return(COMMIT_FAILED_ERROR);

  /* Convert the ifIndex to internal interface number */
  if (usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return(WRONG_VALUE_ERROR);
  }

  /* If entry doesn't exist, then return error */
  if (snmpVrrpOperEntryGet(USMDB_UNIT_CURRENT, data->vrrpOperVrId, data->ifIndex) == L7_SUCCESS)
  {
    if (snmpVrrpTrackIntfEntryGet(USMDB_UNIT_CURRENT, data->vrrpOperVrId, data->ifIndex,
          data->agentRouterVrrpTrackIntf) != L7_SUCCESS)
    {
      /* creation is only allowed when setting row status to createAndGo or createAndWait */
      if (VALID(I_agentRouterVrrpTrackIfStatus, data->valid) &&
          (data->agentRouterVrrpTrackIfStatus == D_agentRouterVrrpTrackIfStatus_createAndGo ||
           data->agentRouterVrrpTrackIfStatus == D_agentRouterVrrpTrackIfStatus_createAndWait))
      {
        if (snmpVrrpTrackIntfAdd(USMDB_UNIT_CURRENT, data->vrrpOperVrId, intIfNum,data->agentRouterVrrpTrackIntf,
              data->agentRouterVrrpTrackIfPrioDec) != L7_SUCCESS)
          return(NO_CREATION_ERROR);

      }
      else
      {
        return(NO_CREATION_ERROR);
      }
    }
  }
  else
    return(NO_CREATION_ERROR);

  if (VALID(I_agentRouterVrrpTrackIfPrioDec, data->valid) &&
      (snmpVrrpTrackIfPrioSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                              intIfNum,data->agentRouterVrrpTrackIntf,
                              data->agentRouterVrrpTrackIfPrioDec) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentRouterVrrpTrackIfStatus, data->valid) &&
      (snmpVrrpTrackIfRowStatusSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                intIfNum,data->agentRouterVrrpTrackIntf,
                                data->agentRouterVrrpTrackIfStatus) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);

  return NO_ERROR;
}

/* Track Route APis */
agentRouterVrrpTrackRouteEntry_t *
k_agentRouterVrrpTrackRouteEntry_get(int serialNum, ContextInfo *contextInfo, int nominator,
                                     int searchType,
                                     SR_INT32 ifIndex,
                                     SR_INT32 vrrpOperVrId,
                                     SR_UINT32       agentRouterVrrpTrackRtPfx,
                                     SR_INT32        agentRouterVrrpTrackRtPfxLen)
{
  static agentRouterVrrpTrackRouteEntry_t agentRouterVrrpTrackRouteEntryData;

  L7_uint32 intIfNum;
  L7_uint32 mask;

  /* To check whether VRRP component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
    return(NULL);

  ZERO_VALID(agentRouterVrrpTrackRouteEntryData.valid);

  agentRouterVrrpTrackRouteEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentRouterVrrpTrackRouteEntryIndex_ifIndex, agentRouterVrrpTrackRouteEntryData.valid);

  agentRouterVrrpTrackRouteEntryData.vrrpOperVrId = vrrpOperVrId;
  SET_VALID(I_agentRouterVrrpTrackRouteEntryIndex_vrrpOperVrId, agentRouterVrrpTrackRouteEntryData.valid);

  agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtPfx = agentRouterVrrpTrackRtPfx;
  SET_VALID(I_agentRouterVrrpTrackRtPfx,agentRouterVrrpTrackRouteEntryData.valid);
  
  agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtPfxLen = agentRouterVrrpTrackRtPfxLen;
  SET_VALID(I_agentRouterVrrpTrackRtPfxLen,agentRouterVrrpTrackRouteEntryData.valid);

  if (((searchType == EXACT) ?
        (snmpVrrpTrackRouteEntryGet(USMDB_UNIT_CURRENT,
                                   agentRouterVrrpTrackRouteEntryData.vrrpOperVrId,
                                   agentRouterVrrpTrackRouteEntryData.ifIndex,
                                   agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtPfx,
                                   agentRouterVrrpTrackRouteEntryData. agentRouterVrrpTrackRtPfxLen) != L7_SUCCESS) :
        ((snmpVrrpTrackRouteEntryGet(USMDB_UNIT_CURRENT,
                                    agentRouterVrrpTrackRouteEntryData.vrrpOperVrId,
                                    agentRouterVrrpTrackRouteEntryData.ifIndex,
                                    agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtPfx,
                                    agentRouterVrrpTrackRouteEntryData. agentRouterVrrpTrackRtPfxLen) != L7_SUCCESS) &&
         (snmpVrrpTrackRouteEntryNextGet(USMDB_UNIT_CURRENT,
                                        &agentRouterVrrpTrackRouteEntryData.vrrpOperVrId,
                                        &agentRouterVrrpTrackRouteEntryData.ifIndex,
                                        &agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtPfx,
                                        &agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtPfxLen
                                        )!= L7_SUCCESS)))
      || (usmDbIntIfNumFromExtIfNum(agentRouterVrrpTrackRouteEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentRouterVrrpTrackRouteEntryData.valid);
    return(NULL);
  }
  mask = agentRouterVrrpTrackRouteEntryData. agentRouterVrrpTrackRtPfxLen;

  switch (nominator)
  {
    case -1:
    case I_agentRouterVrrpTrackRouteEntryIndex_ifIndex:
    case I_agentRouterVrrpTrackRouteEntryIndex_vrrpOperVrId:
    case I_agentRouterVrrpTrackRtPfx:
    case I_agentRouterVrrpTrackRtPfxLen:
      break;

    case I_agentRouterVrrpTrackRtPrioDec:
      if (snmpVrrpOperTrackRoutePrioGet(USMDB_UNIT_CURRENT,
            agentRouterVrrpTrackRouteEntryData.vrrpOperVrId, intIfNum,
            agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtPfx,
            mask,&agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtPrioDec) == L7_SUCCESS)
        SET_VALID(I_agentRouterVrrpTrackRtPrioDec, agentRouterVrrpTrackRouteEntryData.valid);
       break;
    case I_agentRouterVrrpTrackRtReachable:
       if (snmpVrrpOperTrackRoutReachabilityGet(USMDB_UNIT_CURRENT,
             agentRouterVrrpTrackRouteEntryData.vrrpOperVrId, intIfNum,
             agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtPfx,
             mask,&agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtReachable) == L7_SUCCESS)
         SET_VALID(I_agentRouterVrrpTrackRtReachable, agentRouterVrrpTrackRouteEntryData.valid);
       break;
    case I_agentRouterVrrpTrackRtStatus:
       agentRouterVrrpTrackRouteEntryData.agentRouterVrrpTrackRtStatus= D_agentRouterVrrpTrackRtStatus_active;
        SET_VALID(I_agentRouterVrrpTrackRtStatus, agentRouterVrrpTrackRouteEntryData.valid);
       break;
    default:
       /* unknown nominator */
       return(NULL);
       break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentRouterVrrpTrackRouteEntryData.valid) )
    return(NULL);

  return(&agentRouterVrrpTrackRouteEntryData);


}

int
k_agentRouterVrrpTrackRouteEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                                   doList_t *dp, ContextInfo *contextInfo)
{

        return NO_ERROR;
}

int
k_agentRouterVrrpTrackRouteEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *doHead, doList_t *dp)
{
      dp->state = SR_ADD_MODIFY;
            return NO_ERROR;
}

  int
  k_agentRouterVrrpTrackRouteEntry_set_defaults(doList_t *dp)
{
      agentRouterVrrpTrackRouteEntry_t *data = (agentRouterVrrpTrackRouteEntry_t *) (dp->data);
      data->agentRouterVrrpTrackRtPrioDec = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT; 

            ZERO_VALID(data->valid);
                    return NO_ERROR;
}

int
k_agentRouterVrrpTrackRouteEntry_set(agentRouterVrrpTrackRouteEntry_t *data,
                                     ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

  /* To check whether VRRP component is present or not.
     If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_VRRP_MAP_COMPONENT_ID) != L7_TRUE)
    return(COMMIT_FAILED_ERROR);

  /* Convert the ifIndex to internal interface number */
  if (usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return(WRONG_VALUE_ERROR);
  }

  /* If entry doesn't exist, then try to create it */
  if (snmpVrrpOperEntryGet(USMDB_UNIT_CURRENT, data->vrrpOperVrId, data->ifIndex) == L7_SUCCESS)
  {
    if (snmpVrrpTrackRouteEntryGet(USMDB_UNIT_CURRENT, data->vrrpOperVrId, data->ifIndex, 
          data->agentRouterVrrpTrackRtPfx, data->agentRouterVrrpTrackRtPfxLen) != L7_SUCCESS)
    {
      /* creation is only allowed when setting row status to createAndGo or createAndWait */
      if (VALID(I_agentRouterVrrpTrackRtStatus, data->valid) &&
          (data->agentRouterVrrpTrackRtStatus == D_agentRouterVrrpTrackRtStatus_createAndGo ||
           data->agentRouterVrrpTrackRtStatus == D_agentRouterVrrpTrackRtStatus_createAndWait))
      {
        if (snmpVrrpTrackRtAdd(USMDB_UNIT_CURRENT, data->vrrpOperVrId, intIfNum,data->agentRouterVrrpTrackRtPfx,
              data->agentRouterVrrpTrackRtPfxLen,data->agentRouterVrrpTrackRtPrioDec) != L7_SUCCESS)
          return(NO_CREATION_ERROR);

      }
      else
      {
        return(NO_CREATION_ERROR);
      }
    }
  }
  else
    return(NO_CREATION_ERROR);

  if (VALID(I_agentRouterVrrpTrackRtPrioDec, data->valid) &&
      (snmpVrrpTrackRtPrioSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                               intIfNum,data->agentRouterVrrpTrackRtPfx, data->agentRouterVrrpTrackRtPfxLen,
                               data->agentRouterVrrpTrackRtPrioDec) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentRouterVrrpTrackRtStatus, data->valid) &&
      (snmpVrrpTrackRtRowStatusSet(USMDB_UNIT_CURRENT, data->vrrpOperVrId,
                                intIfNum,data->agentRouterVrrpTrackRtPfx, data->agentRouterVrrpTrackRtPfxLen,
                                data->agentRouterVrrpTrackRtStatus) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);



    return NO_ERROR;
}


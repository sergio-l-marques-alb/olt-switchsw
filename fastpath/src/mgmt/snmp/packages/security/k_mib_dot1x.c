/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_dot1x.c
*
* Purpose: System-specific code for IEEE8021-PAE-MIB
*
* Created by: Colin Verne 04/17/2003
*
* Component: SNMP
*
*********************************************************************/ 

#include <k_private_base.h>
#include <k_mib_dot1x_api.h>
#include "usmdb_common.h"
#include "dot1x_exports.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_util_api.h"

dot1xPaeSystem_t *
k_dot1xPaeSystem_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
  static dot1xPaeSystem_t dot1xPaeSystemData;

  ZERO_VALID(dot1xPaeSystemData.valid);

  switch (nominator)
  {
  case -1:
    break;

  case I_dot1xPaeSystemAuthControl:
    if (snmpDot1xPaeSystemAuthControlGet(USMDB_UNIT_CURRENT,
                                         &dot1xPaeSystemData.dot1xPaeSystemAuthControl) == L7_SUCCESS)
      SET_VALID(nominator, dot1xPaeSystemData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

  if ((nominator >= 0) && !VALID(nominator, dot1xPaeSystemData.valid))
    return(NULL);
  return(&dot1xPaeSystemData);
}

#ifdef SETS
int
k_dot1xPaeSystem_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dot1xPaeSystem_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dot1xPaeSystem_set(dot1xPaeSystem_t *data,
                     ContextInfo *contextInfo, int function)
{

  if (VALID(I_dot1xPaeSystemAuthControl, data->valid) &&
      snmpDot1xPaeSystemAuthControlSet(USMDB_UNIT_CURRENT,
                                       data->dot1xPaeSystemAuthControl) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_dot1xPaeSystem_UNDO
/* add #define SR_dot1xPaeSystem_UNDO in sitedefs.h to
 * include the undo routine for the dot1xPaeSystem family.
 */
int
dot1xPaeSystem_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dot1xPaeSystem_UNDO */

#endif /* SETS */

dot1xPaePortEntry_t *
k_dot1xPaePortEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_INT32 dot1xPaePortNumber)
{
  static dot1xPaePortEntry_t dot1xPaePortEntryData;
  L7_uint32 intIfNum;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    dot1xPaePortEntryData.dot1xPaePortCapabilities = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }
  
  ZERO_VALID(dot1xPaePortEntryData.valid);
  dot1xPaePortEntryData.dot1xPaePortNumber = dot1xPaePortNumber;
  SET_VALID(I_dot1xPaePortNumber, dot1xPaePortEntryData.valid);
  
  if (((searchType == EXACT) ?
      (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xPaePortEntryData.dot1xPaePortNumber) != L7_SUCCESS) :
      (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xPaePortEntryData.dot1xPaePortNumber) != L7_SUCCESS &&
       usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT, dot1xPaePortEntryData.dot1xPaePortNumber,
                                       &dot1xPaePortEntryData.dot1xPaePortNumber) != L7_SUCCESS)) ||
     usmDbIntIfNumFromExtIfNum(dot1xPaePortEntryData.dot1xPaePortNumber, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(dot1xPaePortEntryData.valid);
    return(NULL);
  }
  
  switch (nominator)
  {
  case -1:
    break;

  case I_dot1xPaePortNumber:
    break;

  case I_dot1xPaePortProtocolVersion:
    if (usmDbDot1xPortProtocolVersionGet(USMDB_UNIT_CURRENT, intIfNum,
                                         &dot1xPaePortEntryData.dot1xPaePortProtocolVersion) == L7_SUCCESS)
      SET_VALID(I_dot1xPaePortProtocolVersion, dot1xPaePortEntryData.valid);
    break;
  
  case I_dot1xPaePortCapabilities:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpDot1xPaePortCapabilitiesGet(USMDB_UNIT_CURRENT, intIfNum,
                                    snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        SafeMakeOctetString(&dot1xPaePortEntryData.dot1xPaePortCapabilities,
                            snmp_buffer, snmp_buffer_len) == L7_TRUE)
      SET_VALID(I_dot1xPaePortCapabilities, dot1xPaePortEntryData.valid);
    break;
  
  case I_dot1xPaePortInitialize:
    dot1xPaePortEntryData.dot1xPaePortInitialize = D_dot1xPaePortInitialize_false;
    SET_VALID(I_dot1xPaePortInitialize, dot1xPaePortEntryData.valid);
    break;
  
  case I_dot1xPaePortReauthenticate:
    dot1xPaePortEntryData.dot1xPaePortReauthenticate = D_dot1xPaePortReauthenticate_false;
    SET_VALID(I_dot1xPaePortReauthenticate, dot1xPaePortEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return (NULL);
  }


  if ((nominator >= 0) && !VALID(nominator, dot1xPaePortEntryData.valid))
    return(NULL);

  return(&dot1xPaePortEntryData);
}

#ifdef SETS
int
k_dot1xPaePortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dot1xPaePortEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dot1xPaePortEntry_set_defaults(doList_t *dp)
{
    dot1xPaePortEntry_t *data = (dot1xPaePortEntry_t *) (dp->data);

    if ((data->dot1xPaePortCapabilities = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    return NO_ERROR;
}

int
k_dot1xPaePortEntry_set(dot1xPaePortEntry_t *data,
                        ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

  if ((usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, data->dot1xPaePortNumber) != L7_SUCCESS) ||
      usmDbIntIfNumFromExtIfNum(data->dot1xPaePortNumber, &intIfNum) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xPaePortInitialize, data->valid) &&
      snmpDot1xPaePortInitializeSet(USMDB_UNIT_CURRENT, intIfNum,
                                    data->dot1xPaePortInitialize) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xPaePortReauthenticate, data->valid) &&
      snmpDot1xPaePortReauthenticateSet(USMDB_UNIT_CURRENT, intIfNum,
                                    data->dot1xPaePortReauthenticate) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_dot1xPaePortEntry_UNDO
/* add #define SR_dot1xPaePortEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1xPaePortEntry family.
 */
int
dot1xPaePortEntry_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dot1xPaePortEntry_UNDO */

#endif /* SETS */

dot1xAuthConfigEntry_t *
k_dot1xAuthConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 dot1xPaePortNumber)
{
  static dot1xAuthConfigEntry_t dot1xAuthConfigEntryData;
  L7_uint32 intIfNum;
  
  ZERO_VALID(dot1xAuthConfigEntryData.valid);
  dot1xAuthConfigEntryData.dot1xPaePortNumber = dot1xPaePortNumber;
  SET_VALID(I_dot1xAuthConfigEntryIndex_dot1xPaePortNumber, dot1xAuthConfigEntryData.valid);
  
  if (((searchType == EXACT) ?
      (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xAuthConfigEntryData.dot1xPaePortNumber) != L7_SUCCESS) :
      (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xAuthConfigEntryData.dot1xPaePortNumber) != L7_SUCCESS &&
       usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT, dot1xAuthConfigEntryData.dot1xPaePortNumber,
                                       &dot1xAuthConfigEntryData.dot1xPaePortNumber) != L7_SUCCESS)) ||
     usmDbIntIfNumFromExtIfNum(dot1xAuthConfigEntryData.dot1xPaePortNumber, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(dot1xAuthConfigEntryData.valid);
    return(NULL);
  }
  
  switch (nominator)
  {
  case -1:
    break;

  case I_dot1xAuthConfigEntryIndex_dot1xPaePortNumber:
    break;

  case I_dot1xAuthPaeState:
    if (snmpDot1xAuthPaeStateGet(USMDB_UNIT_CURRENT, intIfNum,
                                 &dot1xAuthConfigEntryData.dot1xAuthPaeState) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthPaeState, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthBackendAuthState:
    if (snmpDot1xAuthBackendAuthStateGet(USMDB_UNIT_CURRENT, intIfNum,
                                         &dot1xAuthConfigEntryData.dot1xAuthBackendAuthState) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthBackendAuthState, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthAdminControlledDirections:
    if (snmpDot1xAuthAdminControlledDirectionsGet(USMDB_UNIT_CURRENT, intIfNum,
                                                  &dot1xAuthConfigEntryData.dot1xAuthAdminControlledDirections) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAdminControlledDirections, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthOperControlledDirections:
    if (snmpDot1xAuthOperControlledDirectionsGet(USMDB_UNIT_CURRENT, intIfNum,
                                                 &dot1xAuthConfigEntryData.dot1xAuthOperControlledDirections) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthOperControlledDirections, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthAuthControlledPortStatus:
    if (snmpDot1xAuthAuthControlledPortStatusGet(USMDB_UNIT_CURRENT, intIfNum,
                                                 &dot1xAuthConfigEntryData.dot1xAuthAuthControlledPortStatus) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthControlledPortStatus, dot1xAuthConfigEntryData.valid);
    break;

#ifdef OBSOLETE
  case I_dot1xAuthAuthControlledPortControl:
    if (snmpDot1xAuthAuthControlledPortControlGet(USMDB_UNIT_CURRENT, intIfNum,
                                                  &dot1xAuthConfigEntryData.dot1xAuthAuthControlledPortControl) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthControlledPortControl, dot1xAuthConfigEntryData.valid);
    break;
#endif

  case I_dot1xAuthQuietPeriod:
    if (usmDbDot1xPortQuietPeriodGet(USMDB_UNIT_CURRENT, intIfNum,
                                     &dot1xAuthConfigEntryData.dot1xAuthQuietPeriod) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthQuietPeriod, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthTxPeriod:
    if (usmDbDot1xPortTxPeriodGet(USMDB_UNIT_CURRENT, intIfNum,
                                  &dot1xAuthConfigEntryData.dot1xAuthTxPeriod) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthTxPeriod, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthSuppTimeout:
    if (usmDbDot1xPortSuppTimeoutGet(USMDB_UNIT_CURRENT, intIfNum,
                                     &dot1xAuthConfigEntryData.dot1xAuthSuppTimeout) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthSuppTimeout, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthServerTimeout:
    if (usmDbDot1xPortServerTimeoutGet(USMDB_UNIT_CURRENT, intIfNum,
                                  &dot1xAuthConfigEntryData.dot1xAuthServerTimeout) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthServerTimeout, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthMaxReq:
    if (usmDbDot1xPortMaxReqGet(USMDB_UNIT_CURRENT, intIfNum,
                                &dot1xAuthConfigEntryData.dot1xAuthMaxReq) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthMaxReq, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthReAuthPeriod:
    if (usmDbDot1xPortReAuthPeriodGet(USMDB_UNIT_CURRENT, intIfNum,
                                      &dot1xAuthConfigEntryData.dot1xAuthReAuthPeriod) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthReAuthPeriod, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthReAuthEnabled:
    if (snmpDot1xAuthReAuthEnabledGet(USMDB_UNIT_CURRENT, intIfNum,
                                      &dot1xAuthConfigEntryData.dot1xAuthReAuthEnabled) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthReAuthEnabled, dot1xAuthConfigEntryData.valid);
    break;

  case I_dot1xAuthKeyTxEnabled:
    if (snmpDot1xAuthKeyTxEnabledGet(USMDB_UNIT_CURRENT, intIfNum,
                                     &dot1xAuthConfigEntryData.dot1xAuthKeyTxEnabled) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthKeyTxEnabled, dot1xAuthConfigEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

  if ((nominator >= 0) && !VALID(nominator, dot1xAuthConfigEntryData.valid))
    return(NULL);

  return(&dot1xAuthConfigEntryData);
}

#ifdef SETS
int
k_dot1xAuthConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dot1xAuthConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dot1xAuthConfigEntry_set_defaults(doList_t *dp)
{
    dot1xAuthConfigEntry_t *data = (dot1xAuthConfigEntry_t *) (dp->data);

    data->dot1xAuthReAuthEnabled = D_dot1xAuthReAuthEnabled_false;

    return NO_ERROR;
}

int
k_dot1xAuthConfigEntry_set(dot1xAuthConfigEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

  if ((usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, data->dot1xPaePortNumber) != L7_SUCCESS) ||
      (usmDbIntIfNumFromExtIfNum(data->dot1xPaePortNumber, &intIfNum) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xAuthAdminControlledDirections, data->valid) &&
      snmpDot1xAuthAdminControlledDirectionsSet(USMDB_UNIT_CURRENT, intIfNum,
                                                data->dot1xAuthAdminControlledDirections) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

#ifdef NOT_SUPPORTED
  if (VALID(I_dot1xAuthAuthControlledPortControl, data->valid) &&
      snmpDot1xAuthAuthControlledPortControlSet(USMDB_UNIT_CURRENT, intIfNum,
                                                data->dot1xAuthAuthControlledPortControl) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#else
  if (VALID(I_dot1xAuthAuthControlledPortControl, data->valid))
      return COMMIT_FAILED_ERROR;
#endif

  if (VALID(I_dot1xAuthQuietPeriod, data->valid) &&
      (data->dot1xAuthQuietPeriod < L7_DOT1X_PORT_MIN_QUIET_PERIOD ||
       data->dot1xAuthQuietPeriod > L7_DOT1X_PORT_MAX_QUIET_PERIOD ||
       usmDbDot1xPortQuietPeriodSet(USMDB_UNIT_CURRENT, intIfNum,
                                    data->dot1xAuthQuietPeriod) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xAuthTxPeriod, data->valid) &&
      (data->dot1xAuthTxPeriod > L7_DOT1X_PORT_MAX_TX_PERIOD ||
       data->dot1xAuthTxPeriod < L7_DOT1X_PORT_MIN_TX_PERIOD ||
       usmDbDot1xPortTxPeriodSet(USMDB_UNIT_CURRENT, intIfNum,
                                 data->dot1xAuthTxPeriod) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xAuthSuppTimeout, data->valid) &&
      (data->dot1xAuthSuppTimeout > L7_DOT1X_PORT_MAX_SUPP_TIMEOUT ||
       data->dot1xAuthSuppTimeout < L7_DOT1X_PORT_MIN_SUPP_TIMEOUT ||
       usmDbDot1xPortSuppTimeoutSet(USMDB_UNIT_CURRENT, intIfNum,
                                    data->dot1xAuthSuppTimeout) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xAuthServerTimeout, data->valid) &&
      (data->dot1xAuthServerTimeout > L7_DOT1X_PORT_MAX_SERVER_TIMEOUT ||
       data->dot1xAuthServerTimeout < L7_DOT1X_PORT_MIN_SERVER_TIMEOUT ||
       usmDbDot1xPortServerTimeoutSet(USMDB_UNIT_CURRENT, intIfNum,
                                      data->dot1xAuthServerTimeout) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xAuthMaxReq, data->valid) &&
      (data->dot1xAuthMaxReq > L7_DOT1X_PORT_MAX_MAX_REQ ||
       data->dot1xAuthMaxReq < L7_DOT1X_PORT_MIN_MAX_REQ ||
       usmDbDot1xPortMaxReqSet(USMDB_UNIT_CURRENT, intIfNum,
                               data->dot1xAuthMaxReq) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xAuthReAuthPeriod, data->valid) &&
      (data->dot1xAuthReAuthPeriod > L7_DOT1X_PORT_MAX_REAUTH_PERIOD ||
       data->dot1xAuthReAuthPeriod < L7_DOT1X_PORT_MIN_REAUTH_PERIOD ||
       usmDbDot1xPortReAuthPeriodSet(USMDB_UNIT_CURRENT, intIfNum,
                                     data->dot1xAuthReAuthPeriod) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xAuthReAuthEnabled, data->valid) &&
      snmpDot1xAuthReAuthEnabledSet(USMDB_UNIT_CURRENT, intIfNum,
                                    data->dot1xAuthReAuthEnabled) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xAuthKeyTxEnabled, data->valid) &&
      (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1X_COMPONENT_ID, L7_DOT1X_SET_KEYTXENABLED_FEATURE_ID) == L7_FALSE ||
       snmpDot1xAuthKeyTxEnabledSet(USMDB_UNIT_CURRENT, intIfNum,
                                   data->dot1xAuthKeyTxEnabled) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_dot1xAuthConfigEntry_UNDO
/* add #define SR_dot1xAuthConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1xAuthConfigEntry family.
 */
int
dot1xAuthConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dot1xAuthConfigEntry_UNDO */

#endif /* SETS */

dot1xAuthStatsEntry_t *
k_dot1xAuthStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 dot1xPaePortNumber)
{
  static dot1xAuthStatsEntry_t dot1xAuthStatsEntryData;
  L7_uint32 intIfNum;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  
  ZERO_VALID(dot1xAuthStatsEntryData.valid);
  dot1xAuthStatsEntryData.dot1xPaePortNumber = dot1xPaePortNumber;
  SET_VALID(I_dot1xAuthStatsEntryIndex_dot1xPaePortNumber, dot1xAuthStatsEntryData.valid);
  
  if (((searchType == EXACT) ?
     (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xAuthStatsEntryData.dot1xPaePortNumber) != L7_SUCCESS) :
     (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xAuthStatsEntryData.dot1xPaePortNumber) != L7_SUCCESS &&
      usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT, dot1xAuthStatsEntryData.dot1xPaePortNumber,
                                      &dot1xAuthStatsEntryData.dot1xPaePortNumber) != L7_SUCCESS)) ||
    usmDbIntIfNumFromExtIfNum(dot1xAuthStatsEntryData.dot1xPaePortNumber, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(dot1xAuthStatsEntryData.valid);
    return(NULL);
  }
  
  switch (nominator)
  {
  case -1:
    break;

  case I_dot1xAuthStatsEntryIndex_dot1xPaePortNumber:
    break;

  case I_dot1xAuthEapolFramesRx:
    if (usmDbDot1xPortEapolFramesRxGet(USMDB_UNIT_CURRENT, intIfNum,
                                       &dot1xAuthStatsEntryData.dot1xAuthEapolFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapolFramesRx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthEapolFramesTx:
    if (usmDbDot1xPortEapolFramesTxGet(USMDB_UNIT_CURRENT, intIfNum,
                                       &dot1xAuthStatsEntryData.dot1xAuthEapolFramesTx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapolFramesTx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthEapolStartFramesRx:
    if (usmDbDot1xPortEapolStartFramesRxGet(USMDB_UNIT_CURRENT, intIfNum,
                                            &dot1xAuthStatsEntryData.dot1xAuthEapolStartFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapolStartFramesRx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthEapolLogoffFramesRx:
    if (usmDbDot1xPortEapolLogoffFramesRxGet(USMDB_UNIT_CURRENT, intIfNum,
                                             &dot1xAuthStatsEntryData.dot1xAuthEapolLogoffFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapolLogoffFramesRx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthEapolRespIdFramesRx:
    if (usmDbDot1xPortEapRespIdFramesRxGet(USMDB_UNIT_CURRENT, intIfNum,
                                           &dot1xAuthStatsEntryData.dot1xAuthEapolRespIdFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapolRespIdFramesRx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthEapolRespFramesRx:
    if (usmDbDot1xPortEapResponseFramesRxGet(USMDB_UNIT_CURRENT, intIfNum,
                                             &dot1xAuthStatsEntryData.dot1xAuthEapolRespFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapolRespFramesRx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthEapolReqIdFramesTx:
    if (usmDbDot1xPortEapReqIdFramesTxGet(USMDB_UNIT_CURRENT, intIfNum,
                                          &dot1xAuthStatsEntryData.dot1xAuthEapolReqIdFramesTx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapolReqIdFramesTx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthEapolReqFramesTx:
    if (usmDbDot1xPortEapReqFramesTxGet(USMDB_UNIT_CURRENT, intIfNum,
                                        &dot1xAuthStatsEntryData.dot1xAuthEapolReqFramesTx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapolReqFramesTx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthInvalidEapolFramesRx:
    if (usmDbDot1xPortInvalidEapolFramesRxGet(USMDB_UNIT_CURRENT, intIfNum,
                                              &dot1xAuthStatsEntryData.dot1xAuthInvalidEapolFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthInvalidEapolFramesRx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthEapLengthErrorFramesRx:
    if (usmDbDot1xPortEapLengthErrorFramesRxGet(USMDB_UNIT_CURRENT, intIfNum,
                                                &dot1xAuthStatsEntryData.dot1xAuthEapLengthErrorFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapLengthErrorFramesRx, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthLastEapolFrameVersion:
    if (usmDbDot1xPortLastEapolFrameVersionGet(USMDB_UNIT_CURRENT, intIfNum,
                                               &dot1xAuthStatsEntryData.dot1xAuthLastEapolFrameVersion) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthLastEapolFrameVersion, dot1xAuthStatsEntryData.valid);
    break;

  case I_dot1xAuthLastEapolFrameSource:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbDot1xPortLastEapolFrameSourceGet(USMDB_UNIT_CURRENT, intIfNum, snmp_buffer) == L7_SUCCESS) &&
        SafeMakeOctetString(&dot1xAuthStatsEntryData.dot1xAuthLastEapolFrameSource,
                            snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
      SET_VALID(I_dot1xAuthLastEapolFrameSource, dot1xAuthStatsEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

   if ((nominator >= 0) && !VALID(nominator, dot1xAuthStatsEntryData.valid))
     return(NULL);
   
   return(&dot1xAuthStatsEntryData);
}

dot1xAuthDiagEntry_t *
k_dot1xAuthDiagEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 dot1xPaePortNumber)
{
  static dot1xAuthDiagEntry_t dot1xAuthDiagEntryData;
  L7_uint32 intIfNum;
  
  ZERO_VALID(dot1xAuthDiagEntryData.valid);
  dot1xAuthDiagEntryData.dot1xPaePortNumber = dot1xPaePortNumber;
  SET_VALID(I_dot1xAuthDiagEntryIndex_dot1xPaePortNumber, dot1xAuthDiagEntryData.valid);
  
  if (((searchType == EXACT) ?
     (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xAuthDiagEntryData.dot1xPaePortNumber) != L7_SUCCESS) :
     (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xAuthDiagEntryData.dot1xPaePortNumber) != L7_SUCCESS &&
      usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT, dot1xAuthDiagEntryData.dot1xPaePortNumber,
                                      &dot1xAuthDiagEntryData.dot1xPaePortNumber) != L7_SUCCESS)) ||
    usmDbIntIfNumFromExtIfNum(dot1xAuthDiagEntryData.dot1xPaePortNumber, &intIfNum) != L7_SUCCESS)
  {
   ZERO_VALID(dot1xAuthDiagEntryData.valid);
   return(NULL);
  }
  
  switch (nominator)
  {
  case -1:
    break;

  case I_dot1xAuthDiagEntryIndex_dot1xPaePortNumber:
    break;

  case I_dot1xAuthEntersConnecting:
    if (usmDbDot1xPortAuthEntersConnectingGet(USMDB_UNIT_CURRENT, intIfNum,
                                              &dot1xAuthDiagEntryData.dot1xAuthEntersConnecting) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEntersConnecting, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthEapLogoffsWhileConnecting:
    if (usmDbDot1xPortAuthEapLogoffsWhileConnectingGet(USMDB_UNIT_CURRENT, intIfNum,
                                                       &dot1xAuthDiagEntryData.dot1xAuthEapLogoffsWhileConnecting) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEapLogoffsWhileConnecting, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthEntersAuthenticating:
    if (usmDbDot1xPortAuthEntersAuthenticatingGet(USMDB_UNIT_CURRENT, intIfNum,
                                                  &dot1xAuthDiagEntryData.dot1xAuthEntersAuthenticating) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthEntersAuthenticating, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthAuthSuccessWhileAuthenticating:
    if (usmDbDot1xPortAuthAuthSuccessWhileAuthenticatingGet(USMDB_UNIT_CURRENT, intIfNum,
                                                            &dot1xAuthDiagEntryData.dot1xAuthAuthSuccessWhileAuthenticating) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthSuccessWhileAuthenticating, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthAuthTimeoutsWhileAuthenticating:
    if (usmDbDot1xPortAuthAuthTimeoutsWhileAuthenticatingGet(USMDB_UNIT_CURRENT, intIfNum,
                                                             &dot1xAuthDiagEntryData.dot1xAuthAuthTimeoutsWhileAuthenticating) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthTimeoutsWhileAuthenticating, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthAuthFailWhileAuthenticating:
    if (usmDbDot1xPortAuthAuthFailWhileAuthenticatingGet(USMDB_UNIT_CURRENT, intIfNum,
                                                         &dot1xAuthDiagEntryData.dot1xAuthAuthFailWhileAuthenticating) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthFailWhileAuthenticating, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthAuthReauthsWhileAuthenticating:
    if (usmDbDot1xPortAuthAuthReauthsWhileAuthenticatingGet(USMDB_UNIT_CURRENT, intIfNum,
                                                            &dot1xAuthDiagEntryData.dot1xAuthAuthReauthsWhileAuthenticating) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthReauthsWhileAuthenticating, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthAuthEapStartsWhileAuthenticating:
    if (usmDbDot1xPortAuthAuthEapStartsWhileAuthenticatingGet(USMDB_UNIT_CURRENT, intIfNum,
                                                              &dot1xAuthDiagEntryData.dot1xAuthAuthEapStartsWhileAuthenticating) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthEapStartsWhileAuthenticating, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthAuthEapLogoffWhileAuthenticating:
    if (usmDbDot1xPortAuthAuthEapLogoffWhileAuthenticatingGet(USMDB_UNIT_CURRENT, intIfNum,
                                                              &dot1xAuthDiagEntryData.dot1xAuthAuthEapLogoffWhileAuthenticating) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthEapLogoffWhileAuthenticating, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthAuthReauthsWhileAuthenticated:
    if (usmDbDot1xPortAuthAuthReauthsWhileAuthenticatedGet(USMDB_UNIT_CURRENT, intIfNum,
                                                           &dot1xAuthDiagEntryData.dot1xAuthAuthReauthsWhileAuthenticated) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthReauthsWhileAuthenticated, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthAuthEapStartsWhileAuthenticated:
    if (usmDbDot1xPortAuthAuthEapStartsWhileAuthenticatedGet(USMDB_UNIT_CURRENT, intIfNum,
                                                             &dot1xAuthDiagEntryData.dot1xAuthAuthEapStartsWhileAuthenticated) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthEapStartsWhileAuthenticated, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthAuthEapLogoffWhileAuthenticated:
    if (usmDbDot1xPortAuthAuthEapLogoffWhileAuthenticatedGet(USMDB_UNIT_CURRENT, intIfNum,
                                                             &dot1xAuthDiagEntryData.dot1xAuthAuthEapLogoffWhileAuthenticated) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthAuthEapLogoffWhileAuthenticated, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthBackendResponses:
    if (usmDbDot1xPortAuthBackendResponsesGet(USMDB_UNIT_CURRENT, intIfNum,
                                              &dot1xAuthDiagEntryData.dot1xAuthBackendResponses) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthBackendResponses, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthBackendAccessChallenges:
    if (usmDbDot1xPortAuthBackendAccessChallengesGet(USMDB_UNIT_CURRENT, intIfNum,
                                                     &dot1xAuthDiagEntryData.dot1xAuthBackendAccessChallenges) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthBackendAccessChallenges, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthBackendOtherRequestsToSupplicant:
    if (usmDbDot1xPortAuthBackendOtherRequestsToSupplicantGet(USMDB_UNIT_CURRENT, intIfNum,
                                                              &dot1xAuthDiagEntryData.dot1xAuthBackendOtherRequestsToSupplicant) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthBackendOtherRequestsToSupplicant, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthBackendNonNakResponsesFromSupplicant:
    if (usmDbDot1xPortAuthBackendNonNakResponsesFromSupplicantGet(USMDB_UNIT_CURRENT, intIfNum,
                                                                  &dot1xAuthDiagEntryData.dot1xAuthBackendNonNakResponsesFromSupplicant) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthBackendNonNakResponsesFromSupplicant, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthBackendAuthSuccesses:
    if (usmDbDot1xPortAuthBackendAuthSuccessesGet(USMDB_UNIT_CURRENT, intIfNum,
                                                  &dot1xAuthDiagEntryData.dot1xAuthBackendAuthSuccesses) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthBackendAuthSuccesses, dot1xAuthDiagEntryData.valid);
    break;

  case I_dot1xAuthBackendAuthFails:
    if (usmDbDot1xPortAuthBackendAuthFailsGet(USMDB_UNIT_CURRENT, intIfNum,
                                              &dot1xAuthDiagEntryData.dot1xAuthBackendAuthFails) == L7_SUCCESS)
      SET_VALID(I_dot1xAuthBackendAuthFails, dot1xAuthDiagEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }
   
   if ((nominator >= 0) && !VALID(nominator, dot1xAuthDiagEntryData.valid))
     return(NULL);

   return(&dot1xAuthDiagEntryData);
}

dot1xAuthSessionStatsEntry_t *
k_dot1xAuthSessionStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator,
                                 int searchType,
                                 SR_INT32 dot1xPaePortNumber)
{
  /* Authentication Session Stats not supported */
  return(NULL);
}

dot1xSuppConfigEntry_t *
k_dot1xSuppConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 dot1xPaePortNumber)
{
  static dot1xSuppConfigEntry_t dot1xSuppConfigEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(dot1xSuppConfigEntryData.valid);
  dot1xSuppConfigEntryData.dot1xPaePortNumber = dot1xPaePortNumber;
  SET_VALID(I_dot1xSuppConfigEntryIndex_dot1xPaePortNumber, dot1xSuppConfigEntryData.valid);

  if (((searchType == EXACT) ?
      (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xSuppConfigEntryData.dot1xPaePortNumber) != L7_SUCCESS) :
      (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xSuppConfigEntryData.dot1xPaePortNumber) != L7_SUCCESS &&
       usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT, dot1xSuppConfigEntryData.dot1xPaePortNumber,
                                       &dot1xSuppConfigEntryData.dot1xPaePortNumber) != L7_SUCCESS)) ||
     usmDbIntIfNumFromExtIfNum(dot1xSuppConfigEntryData.dot1xPaePortNumber, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(dot1xSuppConfigEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
    break;

  case I_dot1xSuppConfigEntryIndex_dot1xPaePortNumber:
    break;
  case I_dot1xSuppPaeState:
    if (usmDbDot1xSupplicantPortPaeStateGet(USMDB_UNIT_CURRENT,intIfNum,
                                ( L7_DOT1X_SPM_STATES_t *) &dot1xSuppConfigEntryData.dot1xSuppPaeState) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppPaeState, dot1xSuppConfigEntryData.valid);
    break;

  case I_dot1xSuppHeldPeriod:
    if (usmDbDot1xSupplicantPortHeldPeriodGet(intIfNum,
                                     &dot1xSuppConfigEntryData.dot1xSuppHeldPeriod) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppHeldPeriod, dot1xSuppConfigEntryData.valid);
    break;
  case I_dot1xSuppAuthPeriod:
    if (usmDbDot1xSupplicantPortAuthPeriodGet(intIfNum,
                                     &dot1xSuppConfigEntryData.dot1xSuppAuthPeriod) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppAuthPeriod, dot1xSuppConfigEntryData.valid);
    break;
  case I_dot1xSuppStartPeriod:
    if (usmDbDot1xSupplicantPortStartPeriodGet(intIfNum,
                                     &dot1xSuppConfigEntryData.dot1xSuppStartPeriod) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppStartPeriod, dot1xSuppConfigEntryData.valid);
    break;
  case I_dot1xSuppMaxStart:
    if (usmDbDot1xSupplicantPortMaxStartGet(intIfNum,
                                &dot1xSuppConfigEntryData.dot1xSuppMaxStart) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppMaxStart, dot1xSuppConfigEntryData.valid);
    break;
  case I_dot1xSuppControlledPortStatus:
    if (usmDbDot1xSupplicantPortControlModeGet(intIfNum,
                                (L7_uint32 *)&dot1xSuppConfigEntryData.dot1xSuppControlledPortStatus) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppControlledPortStatus, dot1xSuppConfigEntryData.valid);
    break;
  case I_dot1xBackendSupplicantState:
    if (usmDbDot1xSupplicantPortSbmStateGet(USMDB_UNIT_CURRENT,intIfNum,
                                (L7_DOT1X_SBM_STATES_t *)&dot1xSuppConfigEntryData.dot1xBackendSupplicantState) == L7_SUCCESS)
      SET_VALID(I_dot1xBackendSupplicantState, dot1xSuppConfigEntryData.valid);
    break;
  /* We are not supporting this Attribute as we don't allow Supplicant & Authenticator on the 
     Same port at a time. So It is always Inactive */
  case I_dot1xSupplicantAccessControlWithAuthenticator:
      dot1xSuppConfigEntryData.dot1xSupplicantAccessControlWithAuthenticator = 0;  
      SET_VALID(I_dot1xSupplicantAccessControlWithAuthenticator, dot1xSuppConfigEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

  if ((nominator >= 0) && !VALID(nominator, dot1xSuppConfigEntryData.valid))
    return(NULL);

  return(&dot1xSuppConfigEntryData);

}

#ifdef SETS
int
k_dot1xSuppConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dot1xSuppConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dot1xSuppConfigEntry_set_defaults(doList_t *dp)
{
    return NO_ERROR;
}

int
k_dot1xSuppConfigEntry_set(dot1xSuppConfigEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

  if ((usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, data->dot1xPaePortNumber) != L7_SUCCESS) ||
      (usmDbIntIfNumFromExtIfNum(data->dot1xPaePortNumber, &intIfNum) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xSuppHeldPeriod, data->valid) &&
      (data->dot1xSuppHeldPeriod < L7_DOT1X_PORT_MIN_HELD_PERIOD ||
       data->dot1xSuppHeldPeriod > L7_DOT1X_PORT_MAX_HELD_PERIOD ||
       usmDbDot1xSupplicantPortHeldPeriodSet (intIfNum,
                                    data->dot1xSuppHeldPeriod) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xSuppAuthPeriod, data->valid) &&
      (data->dot1xSuppAuthPeriod < L7_DOT1X_PORT_MIN_AUTH_PERIOD ||
       data->dot1xSuppAuthPeriod > L7_DOT1X_PORT_MAX_AUTH_PERIOD ||
       usmDbDot1xSupplicantPortAuthPeriodSet (intIfNum,
                                    data->dot1xSuppAuthPeriod) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xSuppStartPeriod, data->valid) &&
      (data->dot1xSuppStartPeriod < L7_DOT1X_PORT_MIN_START_PERIOD ||
       data->dot1xSuppStartPeriod > L7_DOT1X_PORT_MAX_START_PERIOD ||
       usmDbDot1xSupplicantPortStartPeriodSet (intIfNum,
                                    data->dot1xSuppStartPeriod) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_dot1xSuppMaxStart, data->valid) &&
      (data->dot1xSuppMaxStart > L7_DOT1X_PORT_MAX_MAX_START ||
       data->dot1xSuppMaxStart < L7_DOT1X_PORT_MIN_MAX_START ||
       usmDbDot1xSupplicantPortMaxStartSet(intIfNum,
                               data->dot1xSuppMaxStart) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;
 return NO_ERROR;

}

#ifdef SR_dot1xSuppConfigEntry_UNDO
/* add #define SR_dot1xSuppConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1xSuppConfigEntry family.
 */
int
dot1xSuppConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dot1xSuppConfigEntry_UNDO */

#endif /* SETS */

dot1xSuppStatsEntry_t *
k_dot1xSuppStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 dot1xPaePortNumber)
{
  static dot1xSuppStatsEntry_t dot1xSuppStatsEntryData;
  L7_uint32 intIfNum;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_enetMacAddr_t lastEapolFrameSource;

  ZERO_VALID(dot1xSuppStatsEntryData.valid);
  dot1xSuppStatsEntryData.dot1xPaePortNumber = dot1xPaePortNumber;
  SET_VALID(I_dot1xSuppStatsEntryIndex_dot1xPaePortNumber, dot1xSuppStatsEntryData.valid);

  if (((searchType == EXACT) ?
     (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xSuppStatsEntryData.dot1xPaePortNumber) != L7_SUCCESS) :
     (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, dot1xSuppStatsEntryData.dot1xPaePortNumber) != L7_SUCCESS &&
      usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT, dot1xSuppStatsEntryData.dot1xPaePortNumber,
                                      &dot1xSuppStatsEntryData.dot1xPaePortNumber) != L7_SUCCESS)) ||
    usmDbIntIfNumFromExtIfNum(dot1xSuppStatsEntryData.dot1xPaePortNumber, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(dot1xSuppStatsEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
    break;

  case I_dot1xSuppStatsEntryIndex_dot1xPaePortNumber:
    break;

  case I_dot1xSuppEapolFramesRx:
    if (usmDbDot1xSupplicantPortEapolFramesRxGet(intIfNum,
                                       &dot1xSuppStatsEntryData.dot1xSuppEapolFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppEapolFramesRx, dot1xSuppStatsEntryData.valid);
    break;
  case I_dot1xSuppEapolFramesTx:
    if (usmDbDot1xSupplicantPortEapolFramesTxGet(intIfNum,
                                       &dot1xSuppStatsEntryData.dot1xSuppEapolFramesTx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppEapolFramesTx, dot1xSuppStatsEntryData.valid);
    break;

  case I_dot1xSuppEapolStartFramesTx:
    if (usmDbDot1xSupplicantPortEapolStartFramesTxGet(intIfNum,
                                            &dot1xSuppStatsEntryData.dot1xSuppEapolStartFramesTx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppEapolStartFramesTx, dot1xSuppStatsEntryData.valid);
    break;

  case I_dot1xSuppEapolLogoffFramesTx:
    if (usmDbDot1xSupplicantPortEapolLogoffFramesTxGet(intIfNum,
                                             &dot1xSuppStatsEntryData.dot1xSuppEapolLogoffFramesTx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppEapolLogoffFramesTx, dot1xSuppStatsEntryData.valid);
    break;

  case I_dot1xSuppEapolRespIdFramesTx:
    if (usmDbDot1xSupplicantPortEapolRespIdFramesTxGet(intIfNum,
                                           &dot1xSuppStatsEntryData.dot1xSuppEapolRespIdFramesTx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppEapolRespIdFramesTx, dot1xSuppStatsEntryData.valid);
    break;

  case I_dot1xSuppEapolRespFramesTx:
    if (usmDbDot1xSupplicantPortEapolResponseFramesTxGet(intIfNum,
                                             &dot1xSuppStatsEntryData.dot1xSuppEapolRespFramesTx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppEapolRespFramesTx, dot1xSuppStatsEntryData.valid);
    break;


  case I_dot1xSuppEapolReqIdFramesRx:
    if (usmDbDot1xSupplicantPortEapolReqIdFramesRxGet(intIfNum,
                                          &dot1xSuppStatsEntryData.dot1xSuppEapolReqIdFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppEapolReqIdFramesRx, dot1xSuppStatsEntryData.valid);
    break;

  case I_dot1xSuppEapolReqFramesRx:
    if (usmDbDot1xSupplicantPortEapolRequestFramesRxGet(intIfNum,
                                        &dot1xSuppStatsEntryData.dot1xSuppEapolReqFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppEapolReqFramesRx, dot1xSuppStatsEntryData.valid);
    break;

  case I_dot1xSuppInvalidEapolFramesRx:
    if (usmDbDot1xSupplicantPortInvalidEapolFramesRxGet(intIfNum,
                                              &dot1xSuppStatsEntryData.dot1xSuppInvalidEapolFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppInvalidEapolFramesRx, dot1xSuppStatsEntryData.valid);
    break;

  case I_dot1xSuppEapLengthErrorFramesRx:
    if (usmDbDot1xSupplicantPortEapLengthErrorFramesRxGet(intIfNum,
                                                &dot1xSuppStatsEntryData.dot1xSuppEapLengthErrorFramesRx) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppEapLengthErrorFramesRx, dot1xSuppStatsEntryData.valid);
    break;

  case I_dot1xSuppLastEapolFrameVersion:
    if (usmDbDot1xSupplicantPortLastEapolFrameVersionGet(intIfNum,
                                               &dot1xSuppStatsEntryData.dot1xSuppLastEapolFrameVersion) == L7_SUCCESS)
      SET_VALID(I_dot1xSuppLastEapolFrameVersion, dot1xSuppStatsEntryData.valid);
    break;
  case I_dot1xSuppLastEapolFrameSource:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbDot1xSupplicantPortLastEapolFrameSourceGet(intIfNum, &lastEapolFrameSource) == L7_SUCCESS))
    {
         memcpy(snmp_buffer,lastEapolFrameSource.addr,
         L7_MAC_ADDR_LEN);
        if(SafeMakeOctetString(&dot1xSuppStatsEntryData.dot1xSuppLastEapolFrameSource,
                            snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
          SET_VALID(I_dot1xSuppLastEapolFrameSource, dot1xSuppStatsEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

   if ((nominator >= 0) && !VALID(nominator, dot1xSuppStatsEntryData.valid))
     return(NULL);

   return(&dot1xSuppStatsEntryData);
}


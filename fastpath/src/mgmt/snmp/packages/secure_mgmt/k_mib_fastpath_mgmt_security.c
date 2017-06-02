/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_mgmt_security.c
*
* Purpose: System-specific code for FASTPATH Mgmt Security MIB
*
* Created by: Colin Verne 09/17/2003
*
* Component: SNMP
*
*********************************************************************/

#include "k_private_base.h"
#include "k_mib_fastpath_mgmt_security_api.h"
#include "usmdb_common.h"
#include "usmdb_sshd_api.h"
#include "usmdb_sslt_api.h"

agentSSLConfigGroup_t *
k_agentSSLConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
  static agentSSLConfigGroup_t agentSSLConfigGroupData;

  ZERO_VALID(agentSSLConfigGroupData.valid);

  switch (nominator)
  {
  case I_agentSSLAdminMode:
    if (snmpAgentSSLAdminModeGet(USMDB_UNIT_CURRENT,
                                 &agentSSLConfigGroupData.agentSSLAdminMode) == L7_SUCCESS)
      SET_VALID(nominator, agentSSLConfigGroupData.valid);
    break;
  case I_agentSSLSecurePort:
    if (usmDbssltSecurePortGet(USMDB_UNIT_CURRENT,
                               &agentSSLConfigGroupData.agentSSLSecurePort) == L7_SUCCESS)
      SET_VALID(nominator, agentSSLConfigGroupData.valid);
    break;
  case I_agentSSLProtocolLevel:
    if (snmpAgentSSLProtocolLevelGet(USMDB_UNIT_CURRENT,
                                     &agentSSLConfigGroupData.agentSSLProtocolLevel) == L7_SUCCESS)
      SET_VALID(nominator, agentSSLConfigGroupData.valid);
    break;
  case I_agentSSLMaxSessions:
    if (usmDbssltNumSessionsGet(&agentSSLConfigGroupData.agentSSLMaxSessions) == L7_SUCCESS)
      SET_VALID(nominator, agentSSLConfigGroupData.valid);
    break;
  case I_agentSSLHardTimeout:
    if (usmDbssltSessionHardTimeOutGet(&agentSSLConfigGroupData.agentSSLHardTimeout) == L7_SUCCESS)
      SET_VALID(nominator, agentSSLConfigGroupData.valid);
    break;
  case I_agentSSLSoftTimeout:
    if (usmDbssltSessionSoftTimeOutGet(&agentSSLConfigGroupData.agentSSLSoftTimeout) == L7_SUCCESS)
      SET_VALID(nominator, agentSSLConfigGroupData.valid);
    break;
  case I_agentSSLCertificatePresent:
    if (usmDbssltCertificateExists(1) == L7_SUCCESS)
    {
      agentSSLConfigGroupData.agentSSLCertificatePresent = D_agentSSLCertificatePresent_true;
    }
    else
    {
      agentSSLConfigGroupData.agentSSLCertificatePresent = D_agentSSLCertificatePresent_false;
    }
    SET_VALID(nominator, agentSSLConfigGroupData.valid);
    break;
  case I_agentSSLCertificateGenerationStatus:
    if (usmDbssltCertificateExists(1) == L7_FAILURE)
    {
      agentSSLConfigGroupData.agentSSLCertificateGenerationStatus = D_agentSSLCertificateGenerationStatus_true;
    }
    else
    {
      agentSSLConfigGroupData.agentSSLCertificateGenerationStatus = D_agentSSLCertificateGenerationStatus_false;
    }
    SET_VALID(nominator, agentSSLConfigGroupData.valid);
    break;
  case I_agentSSLCertificateControl:
    agentSSLConfigGroupData.agentSSLCertificateControl = D_agentSSLCertificateControl_noop;
    SET_VALID(nominator, agentSSLConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSSLConfigGroupData.valid))
    return (NULL);

  return(&agentSSLConfigGroupData);
}

#ifdef SETS
int
k_agentSSLConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSSLConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSSLConfigGroup_set(agentSSLConfigGroup_t *data,
                          ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentSSLAdminMode, data->valid) &&
      snmpAgentSSLAdminModeSet(USMDB_UNIT_CURRENT,
                               data->agentSSLAdminMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSSLSecurePort, data->valid) &&
      usmDbssltSecurePortSet(USMDB_UNIT_CURRENT,
                             data->agentSSLSecurePort) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSSLProtocolLevel, data->valid) &&
      snmpAgentSSLProtocolLevelSet(USMDB_UNIT_CURRENT,
                               data->agentSSLProtocolLevel) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSSLMaxSessions, data->valid) &&
      usmDbssltNumSessionsSet(data->agentSSLMaxSessions) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSSLHardTimeout, data->valid) &&
      usmDbssltSessionHardTimeOutSet(data->agentSSLHardTimeout) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSSLSoftTimeout, data->valid) &&
      usmDbssltSessionSoftTimeOutSet(data->agentSSLSoftTimeout) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSSLCertificateControl, data->valid) &&
      snmpAgentSSLCertificateControlSet(data->agentSSLCertificateControl) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentSSLConfigGroup_UNDO
/* add #define SR_agentSSLConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSSLConfigGroup family.
 */
int
agentSSLConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSSLConfigGroup_UNDO */

#endif /* SETS */

agentSSHConfigGroup_t *
k_agentSSHConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
  static agentSSHConfigGroup_t agentSSHConfigGroupData;

  ZERO_VALID(agentSSHConfigGroupData.valid);

  switch (nominator)
  {
  case I_agentSSHAdminMode:
    if (snmpAgentSSHAdminModeGet(USMDB_UNIT_CURRENT,
                                 &agentSSHConfigGroupData.agentSSHAdminMode) == L7_SUCCESS)
      SET_VALID(nominator, agentSSHConfigGroupData.valid);
    break;

  case I_agentSSHProtocolLevel:
    if (snmpAgentSSHProtocolLevelGet(USMDB_UNIT_CURRENT,
                                     &agentSSHConfigGroupData.agentSSHProtocolLevel) == L7_SUCCESS)
      SET_VALID(nominator, agentSSHConfigGroupData.valid);
    break;

  case I_agentSSHSessionsCount:
    if (usmDbsshdNumSessionsGet(USMDB_UNIT_CURRENT,
                                &agentSSHConfigGroupData.agentSSHSessionsCount) == L7_SUCCESS)
      SET_VALID(nominator, agentSSHConfigGroupData.valid);
    break;

  /* SSH TELNET */

  case I_agentSSHMaxSessionsCount:
    if (usmDbSshdMaxNumSessionsGet(USMDB_UNIT_CURRENT, &agentSSHConfigGroupData.agentSSHMaxSessionsCount) == L7_SUCCESS)
       SET_VALID(nominator, agentSSHConfigGroupData.valid);
    break;

  case I_agentSSHSessionTimeout:
    if (usmDbsshdTimeoutGet(USMDB_UNIT_CURRENT, &agentSSHConfigGroupData.agentSSHSessionTimeout) == L7_SUCCESS)
       SET_VALID(nominator, agentSSHConfigGroupData.valid);
    break;
  /* SSH TELNET */

  case I_agentSSHKeysPresent:
    if (snmpAgentSSHKeysPresentGet(&agentSSHConfigGroupData.agentSSHKeysPresent) == L7_SUCCESS)
       SET_VALID(nominator, agentSSHConfigGroupData.valid);
    break;

  case I_agentSSHKeyGenerationStatus:
    if (snmpAgentSSHKeyGenerationStatusGet(&agentSSHConfigGroupData.agentSSHKeyGenerationStatus) == L7_SUCCESS)
       SET_VALID(nominator, agentSSHConfigGroupData.valid);
    break;

  case I_agentSSHRSAKeyControl:
       agentSSHConfigGroupData.agentSSHRSAKeyControl = D_agentSSHRSAKeyControl_noop;
       SET_VALID(nominator, agentSSHConfigGroupData.valid);
    break;

  case I_agentSSHDSAKeyControl:
       agentSSHConfigGroupData.agentSSHDSAKeyControl = D_agentSSHDSAKeyControl_noop;
       SET_VALID(nominator, agentSSHConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSSHConfigGroupData.valid))
    return(NULL);

  return(&agentSSHConfigGroupData);
}

#ifdef SETS
int
k_agentSSHConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSSHConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSSHConfigGroup_set(agentSSHConfigGroup_t *data,
                          ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentSSHAdminMode, data->valid) &&
      snmpAgentSSHAdminModeSet(USMDB_UNIT_CURRENT, data->agentSSHAdminMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSSHProtocolLevel, data->valid) &&
      snmpAgentSSHProtocolLevelSet(USMDB_UNIT_CURRENT, data->agentSSHProtocolLevel) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* SSH TELNET */

  if (VALID(I_agentSSHMaxSessionsCount, data->valid) &&
      snmpAgentMaxSSHSessionsSet(USMDB_UNIT_CURRENT, data->agentSSHMaxSessionsCount) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSSHSessionTimeout, data->valid) &&
      snmpAgentSSHTimeoutSet(USMDB_UNIT_CURRENT, data->agentSSHSessionTimeout) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* SSH TELNET */
  if (VALID(I_agentSSHRSAKeyControl, data->valid) &&
      snmpAgentSSHRSAKeyControlSet(data->agentSSHRSAKeyControl) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSSHDSAKeyControl, data->valid) &&
      snmpAgentSSHDSAKeyControlSet(data->agentSSHDSAKeyControl) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
  return NO_ERROR;
}

#ifdef SR_agentSSHConfigGroup_UNDO
/* add #define SR_agentSSHConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSSHConfigGroup family.
 */
int
agentSSHConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSSHConfigGroup_UNDO */

#endif /* SETS */


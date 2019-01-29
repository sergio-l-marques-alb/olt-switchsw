
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_dot1xAdvanced.c
*
* Purpose: System-specific code for FASTPATH-DOT1x-ADVANCED-FEATURES-MIB
*
* Created by: 
*
* Component: SNMP
*
*********************************************************************/
#include <k_private_base.h>
#include <k_mib_dot1xAdvanced_api.h>
#include "usmdb_common.h"
#include "dot1q_exports.h"
#include "dot1x_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_util_api.h"

agentDot1xEnhancementConfigGroup_t *
k_agentDot1xEnhancementConfigGroup_get(int serialNum, ContextInfo *contextInfo,
    int nominator)
{
  static agentDot1xEnhancementConfigGroup_t agentDot1xEnhancementConfigGroupData;

  ZERO_VALID(agentDot1xEnhancementConfigGroupData.valid);

  switch (nominator)
  {
    case I_agentDot1xRadiusVlanAssignment:
      if (usmDbFeaturePresentCheck (USMDB_UNIT_CURRENT,L7_DOT1X_COMPONENT_ID, 
                                   L7_DOT1X_VLANASSIGN_FEATURE_ID) != L7_TRUE)
          return (NULL);

      if (snmpagentDot1xVlanAssignmentModeGet(USMDB_UNIT_CURRENT,
            &agentDot1xEnhancementConfigGroupData.agentDot1xRadiusVlanAssignment) == L7_SUCCESS)
        SET_VALID(I_agentDot1xRadiusVlanAssignment, agentDot1xEnhancementConfigGroupData.valid);

      if(nominator != -1)
      break;

#ifdef I_agentDot1xDynamicVlanCreationMode
      case I_agentDot1xDynamicVlanCreationMode:
	/* Add feature present cheque if required */	
      if(snmpagentDot1xDynamicVlanCreationModeGet(USMDB_UNIT_CURRENT,
            &agentDot1xEnhancementConfigGroupData.agentDot1xDynamicVlanCreationMode) == L7_SUCCESS)
        SET_VALID(I_agentDot1xDynamicVlanCreationMode, agentDot1xEnhancementConfigGroupData.valid);
      break;
#endif
    default :
      /* unknown nominator */
      return(NULL);
  }

  if ((nominator >= 0) && !VALID(nominator, agentDot1xEnhancementConfigGroupData.valid))
    return(NULL);

  return(&agentDot1xEnhancementConfigGroupData);
}

#ifdef SETS
  int
k_agentDot1xEnhancementConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

  int
k_agentDot1xEnhancementConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_agentDot1xEnhancementConfigGroup_set(agentDot1xEnhancementConfigGroup_t *data,
    ContextInfo *contextInfo, int function)
{

#ifdef I_agentDot1xDynamicVlanCreationMode
  if (VALID(I_agentDot1xDynamicVlanCreationMode, data->valid) &&
      usmDbDot1xDynamicVlanCreationModeSet(USMDB_UNIT_CURRENT,
        data->agentDot1xDynamicVlanCreationMode) != L7_SUCCESS)

    return COMMIT_FAILED_ERROR;
#endif

  if (usmDbFeaturePresentCheck (USMDB_UNIT_CURRENT,L7_DOT1X_COMPONENT_ID, 
                                   L7_DOT1X_VLANASSIGN_FEATURE_ID) != L7_TRUE)
       return COMMIT_FAILED_ERROR;
         
  if (VALID(I_agentDot1xRadiusVlanAssignment, data->valid) &&
      usmDbDot1xVlanAssignmentModeSet(USMDB_UNIT_CURRENT,
        data->agentDot1xRadiusVlanAssignment) != L7_SUCCESS)

    return COMMIT_FAILED_ERROR;
  
  return NO_ERROR;
}

#ifdef SR_agentDot1xEnhancementConfigGroup_UNDO
/* add #define SR_agentDot1xEnhancementConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDot1xEnhancementConfigGroup family.
 */
  int
agentDot1xEnhancementConfigGroup_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDot1xEnhancementConfigGroup_UNDO */

#endif /* SETS */


agentDot1xPortConfigEntry_t *
k_agentDot1xPortConfigEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    SR_INT32 dot1xPaePortNumber)
{
  static agentDot1xPortConfigEntry_t agentDot1xPortConfigEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(agentDot1xPortConfigEntryData.valid);

  agentDot1xPortConfigEntryData.dot1xPaePortNumber = dot1xPaePortNumber; 
  SET_VALID(I_agentDot1xPortConfigEntryIndex_dot1xPaePortNumber, agentDot1xPortConfigEntryData.valid);

  if (((searchType == EXACT) ?
        (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, agentDot1xPortConfigEntryData.dot1xPaePortNumber) != L7_SUCCESS) :
        (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, agentDot1xPortConfigEntryData.dot1xPaePortNumber) != L7_SUCCESS &&
         usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT, agentDot1xPortConfigEntryData.dot1xPaePortNumber,
           &agentDot1xPortConfigEntryData.dot1xPaePortNumber) != L7_SUCCESS)) ||
      usmDbIntIfNumFromExtIfNum(agentDot1xPortConfigEntryData.dot1xPaePortNumber, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(agentDot1xPortConfigEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {

    case -1:
      break;

    case I_agentDot1xPortControlMode:
       if (snmpagentDot1xPortControlModeGet(USMDB_UNIT_CURRENT,intIfNum,
           &agentDot1xPortConfigEntryData.agentDot1xPortControlMode)==L7_SUCCESS) 
            SET_VALID(I_agentDot1xPortControlMode, agentDot1xPortConfigEntryData.valid);
       break;
    case I_agentDot1xGuestVlanId:
      if(usmDbDot1xAdvancedGuestPortsCfgGet(USMDB_UNIT_CURRENT,intIfNum,
            &agentDot1xPortConfigEntryData.agentDot1xGuestVlanId) == L7_SUCCESS)
        SET_VALID(I_agentDot1xGuestVlanId, agentDot1xPortConfigEntryData.valid);
      break;

    case I_agentDot1xGuestVlanPeriod:
      if(usmDbDot1xAdvancedPortGuestVlanPeriodGet(USMDB_UNIT_CURRENT,intIfNum,
            &agentDot1xPortConfigEntryData.agentDot1xGuestVlanPeriod) == L7_SUCCESS)
        SET_VALID(I_agentDot1xGuestVlanPeriod, agentDot1xPortConfigEntryData.valid);
      break;

    case I_agentDot1xUnauthenticatedVlan:
       if (usmDbDot1xPortUnauthenticatedVlanGet(USMDB_UNIT_CURRENT,intIfNum,
           &agentDot1xPortConfigEntryData.agentDot1xUnauthenticatedVlan) == L7_SUCCESS) 
           SET_VALID(I_agentDot1xUnauthenticatedVlan, agentDot1xPortConfigEntryData.valid);
      break;

    case I_agentDot1xMaxUsers:
        if (usmDbDot1xPortMaxUsersGet(USMDB_UNIT_CURRENT,intIfNum,
           &agentDot1xPortConfigEntryData.agentDot1xMaxUsers) == L7_SUCCESS) 
           SET_VALID(I_agentDot1xMaxUsers, agentDot1xPortConfigEntryData.valid);
        break;

    case I_agentDot1xPortVlanAssigned:
        if (snmpagentDot1xPortVlanAssignedGet(USMDB_UNIT_CURRENT,intIfNum,
           &agentDot1xPortConfigEntryData.agentDot1xPortVlanAssigned) == L7_SUCCESS) 
           SET_VALID(I_agentDot1xPortVlanAssigned, agentDot1xPortConfigEntryData.valid);
        break;
      

    case I_agentDot1xPortVlanAssignedReason:
        if (snmpagentDot1xPortVlanAssignedReasonGet(USMDB_UNIT_CURRENT,intIfNum,
            &agentDot1xPortConfigEntryData.agentDot1xPortVlanAssignedReason)==L7_SUCCESS) 
               SET_VALID(I_agentDot1xPortVlanAssignedReason, agentDot1xPortConfigEntryData.valid);
      break;

    case I_agentDot1xPortSessionTimeout:
         if (snmpagentDot1xPortSessionTimeoutGet(USMDB_UNIT_CURRENT,intIfNum,
           &agentDot1xPortConfigEntryData.agentDot1xPortSessionTimeout) == L7_SUCCESS) 
             SET_VALID(I_agentDot1xPortSessionTimeout, agentDot1xPortConfigEntryData.valid);
        break;
      break;

    case I_agentDot1xPortTerminationAction:
         if (snmpagentDot1xPortTerminationActionGet(USMDB_UNIT_CURRENT,intIfNum,
            &agentDot1xPortConfigEntryData.agentDot1xPortTerminationAction)==L7_SUCCESS) 
               SET_VALID(I_agentDot1xPortTerminationAction, agentDot1xPortConfigEntryData.valid);
      break;

#ifdef NOT_YET
    case I_agentDot1xPortMABenabled:
         if (snmpagentDot1xPortMABenabledGet(USMDB_UNIT_CURRENT,intIfNum,
            &agentDot1xPortConfigEntryData.agentDot1xPortMABenabled)==L7_SUCCESS) 
               SET_VALID(I_agentDot1xPortMABenabled, agentDot1xPortConfigEntryData.valid);
      break;

     case I_agentDot1xPortMABenabledOperational:
         if (snmpagentDot1xPortMABenabledOperationalGet(USMDB_UNIT_CURRENT,intIfNum,
            &agentDot1xPortConfigEntryData.agentDot1xPortMABenabledOperational)==L7_SUCCESS) 
               SET_VALID(I_agentDot1xPortMABenabledOperational, agentDot1xPortConfigEntryData.valid);
      break;
#endif /* NOT_YET */

    default :
      /* unknown nominator */
      return(NULL);
  }  

  if (nominator != -1 && !VALID(nominator, agentDot1xPortConfigEntryData.valid))
    return NULL;

  return(&agentDot1xPortConfigEntryData);
}

#ifdef SETS
int
k_agentDot1xPortConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDot1xPortConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDot1xPortConfigEntry_set_defaults(doList_t *dp)
{
  agentDot1xPortConfigEntry_t *data = (agentDot1xPortConfigEntry_t *) (dp->data);

  data->agentDot1xPortControlMode = D_agentDot1xPortControlMode_auto;
  data->agentDot1xGuestVlanId = 0;
  data->agentDot1xGuestVlanPeriod = 90;
  data->agentDot1xUnauthenticatedVlan = 0;
  data->agentDot1xMaxUsers = FD_DOT1X_PORT_MAX_USERS;
  data->agentDot1xPortVlanAssigned = 0;
  data->agentDot1xPortVlanAssignedReason = D_agentDot1xPortVlanAssignedReason_notAssigned;
  data->agentDot1xPortSessionTimeout = 0;
  data->agentDot1xPortTerminationAction = D_agentDot1xPortTerminationAction_default;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentDot1xPortConfigEntry_set(agentDot1xPortConfigEntry_t *data,
    ContextInfo *contextInfo, int function)
{

  L7_uint32 intIfNum;

  if ((usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, data->dot1xPaePortNumber) != L7_SUCCESS) ||
      usmDbIntIfNumFromExtIfNum(data->dot1xPaePortNumber, &intIfNum) != L7_SUCCESS)

    return COMMIT_FAILED_ERROR;

  /*port control mode*/
  if (VALID(I_agentDot1xPortControlMode, data->valid) &&
      (snmpagentDot1xPortControlModeSet(USMDB_UNIT_CURRENT,data->dot1xPaePortNumber,data->agentDot1xPortControlMode)!=L7_SUCCESS)) 
       return COMMIT_FAILED_ERROR; 

  /*guest vlan */
  if (VALID(I_agentDot1xGuestVlanId, data->valid) &&
      ((data->agentDot1xGuestVlanId != 0 &&
      (data->agentDot1xGuestVlanId > L7_DOT1Q_MAX_VLAN_ID ||
       data->agentDot1xGuestVlanId < L7_DOT1Q_MIN_VLAN_ID )) ||
       (usmDbDot1xAdvancedGuestPortsCfgSet(USMDB_UNIT_CURRENT, intIfNum, data->agentDot1xGuestVlanId) != L7_SUCCESS)))

    return COMMIT_FAILED_ERROR;

  /*guest vlan period*/
  if (VALID(I_agentDot1xGuestVlanPeriod, data->valid) &&
      (data->agentDot1xGuestVlanPeriod > L7_DOT1X_PORT_MAX_GUESTVLAN_PERIOD ||
       data->agentDot1xGuestVlanPeriod < L7_DOT1X_PORT_MIN_GUESTVLAN_PERIOD ||
       usmDbDot1xAdvancedPortGuestVlanPeriodSet(USMDB_UNIT_CURRENT, intIfNum,
         data->agentDot1xGuestVlanPeriod) != L7_SUCCESS))

    return COMMIT_FAILED_ERROR;

  /*unauthenticated vlan */
   if (VALID(I_agentDot1xUnauthenticatedVlan, data->valid) &&
      ((data->agentDot1xUnauthenticatedVlan !=0 &&
        (data->agentDot1xUnauthenticatedVlan > L7_DOT1Q_MAX_VLAN_ID ||
        data->agentDot1xUnauthenticatedVlan < L7_DOT1Q_MIN_VLAN_ID) )||
       usmDbDot1xPortUnauthenticatedVlanSet(USMDB_UNIT_CURRENT, intIfNum,
         data->agentDot1xUnauthenticatedVlan) != L7_SUCCESS))

    return COMMIT_FAILED_ERROR;

  /*max users*/
   if (VALID(I_agentDot1xMaxUsers, data->valid) &&
      (data->agentDot1xMaxUsers > L7_DOT1X_PORT_MAX_MAC_USERS||
       data->agentDot1xMaxUsers < L7_DOT1X_PORT_MIN_MAC_USERS ||
       usmDbDot1xPortMaxUsersSet(USMDB_UNIT_CURRENT, intIfNum,
         data->agentDot1xMaxUsers) != L7_SUCCESS))

    return COMMIT_FAILED_ERROR;

#ifdef NOT_YET
   if (VALID(I_agentDot1xPortMABenabled, data->valid) &&
       snmpagentDot1xPortMABenabledSet(USMDB_UNIT_CURRENT, intIfNum, data->agentDot1xPortMABenabled)!=L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
#endif /* NOT_YET */

  return NO_ERROR;

}

#ifdef SR_agentDot1xGuestVlanPortConfigEntry_UNDO
/* add #define SR_agentDot1xGuestVlanPortConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDot1xGuestVlanPortConfigEntry family.
 */
int
agentDot1xPortConfigEntry_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDot1xGuestVlanPortConfigEntry_UNDO */

#endif /* SETS */

agentDot1xClientConfigEntry_t *
k_agentDot1xClientConfigEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    OctetString * agentDot1xClientMacAddress)
{
  static agentDot1xClientConfigEntry_t agentDot1xClientConfigEntryData;
  L7_uint32 lIntfNum;
  static L7_BOOL firstTime=L7_TRUE;
  L7_char8 snmp_buffer_ClientMacAddress[SNMP_BUFFER_LEN];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
   
  if (firstTime == L7_TRUE)
  {
   firstTime = L7_FALSE;

   agentDot1xClientConfigEntryData.agentDot1xClientMacAddress = MakeOctetString(NULL, 0);
   agentDot1xClientConfigEntryData.agentDot1xClientUserName = MakeOctetString(NULL,0);
   agentDot1xClientConfigEntryData.agentDot1xClientFilterID = MakeOctetString(NULL,0);
  }

  ZERO_VALID(agentDot1xClientConfigEntryData.valid);

  bzero(snmp_buffer_ClientMacAddress, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_ClientMacAddress, agentDot1xClientMacAddress->octet_ptr, agentDot1xClientMacAddress->length);
  /*agentDot1xPortConfigEntryData.dot1xPaePortNumber = dot1xPaePortNumber; */

  if ((searchType == EXACT) ?
        (usmDbDot1xClientMacAddressGet(USMDB_UNIT_CURRENT, snmp_buffer_ClientMacAddress,&lIntfNum) != L7_SUCCESS) :
        (usmDbDot1xClientMacAddressGet(USMDB_UNIT_CURRENT, snmp_buffer_ClientMacAddress,&lIntfNum) != L7_SUCCESS &&
         usmDbDot1xClientMacAddressNextGet(USMDB_UNIT_CURRENT, snmp_buffer_ClientMacAddress,&lIntfNum) != L7_SUCCESS)) 
  {
    return(NULL);
  }

  
  /* copy the new MacAddress to the outgoing data structure, return failure if it fails */
  if (SafeMakeOctetString(&agentDot1xClientConfigEntryData.agentDot1xClientMacAddress, snmp_buffer_ClientMacAddress, L7_MAC_ADDR_LEN) != L7_TRUE)
    return(NULL);

  SET_VALID(I_agentDot1xClientMacAddress, agentDot1xClientConfigEntryData.valid);

  agentDot1xClientConfigEntryData.agentDot1xLogicalPort = lIntfNum;
  SET_VALID(I_agentDot1xLogicalPort, agentDot1xClientConfigEntryData.valid);

  switch (nominator)
  {

    case -1:
      break;

    case I_agentDot1xClientMacAddress:
    case I_agentDot1xLogicalPort:
      break;

    case I_agentDot1xInterface:
       if (usmDbDot1xPhysicalPortGet(USMDB_UNIT_CURRENT,lIntfNum,
           &agentDot1xClientConfigEntryData.agentDot1xInterface)==L7_SUCCESS) 
            SET_VALID(I_agentDot1xPortControlMode, agentDot1xClientConfigEntryData.valid);
       break;

    case I_agentDot1xClientAuthPAEstate:
        if (snmpagentDot1xClientAuthPAEstateGet(lIntfNum,
            &agentDot1xClientConfigEntryData.agentDot1xClientAuthPAEstate) == L7_SUCCESS) 
            SET_VALID(I_agentDot1xClientAuthPAEstate, agentDot1xClientConfigEntryData.valid);
   break;

   case I_agentDot1xClientBackendState:
        if (snmpagentDot1xClientBackendStateGet(lIntfNum,
            &agentDot1xClientConfigEntryData.agentDot1xClientBackendState) == L7_SUCCESS) 
            SET_VALID(I_agentDot1xClientBackendState, agentDot1xClientConfigEntryData.valid);
   break;

   case I_agentDot1xClientUserName:
        memset(snmp_buffer, 0,SNMP_BUFFER_LEN);
        if ((snmpagentDot1xClientUserNameGet(USMDB_UNIT_CURRENT,lIntfNum,snmp_buffer)==L7_SUCCESS)&&
             SafeMakeOctetStringFromTextExact(&agentDot1xClientConfigEntryData.agentDot1xClientUserName,snmp_buffer)==L7_TRUE) 
             SET_VALID(I_agentDot1xClientUserName, agentDot1xClientConfigEntryData.valid);
      break;

    case I_agentDot1xClientSessionTime:
         if (snmpagentDot1xClientSessionTimeGet(USMDB_UNIT_CURRENT,lIntfNum,&agentDot1xClientConfigEntryData.agentDot1xClientSessionTime)==L7_SUCCESS) 
             SET_VALID(I_agentDot1xClientSessionTime, agentDot1xClientConfigEntryData.valid);
      break;

    case I_agentDot1xClientFilterID:
        memset(snmp_buffer, 0,SNMP_BUFFER_LEN);
        if ((snmpagentDot1xClientFilterIDGet(USMDB_UNIT_CURRENT,lIntfNum,snmp_buffer)==L7_SUCCESS)&&
             SafeMakeOctetStringFromTextExact(&agentDot1xClientConfigEntryData.agentDot1xClientFilterID,snmp_buffer)==L7_TRUE) 
             SET_VALID(I_agentDot1xClientFilterID, agentDot1xClientConfigEntryData.valid);
      break;

    case I_agentDot1xClientVlanAssigned:
        if (snmpagentDot1xClientVlanAssignedGet(lIntfNum,&agentDot1xClientConfigEntryData.agentDot1xClientVlanAssigned)==L7_SUCCESS) 
             SET_VALID(I_agentDot1xClientVlanAssigned, agentDot1xClientConfigEntryData.valid);
        break;

    case I_agentDot1xClientVlanAssignedReason:
        if (snmpagentDot1xClientVlanAssignedReasonGet(lIntfNum,
            &agentDot1xClientConfigEntryData.agentDot1xClientVlanAssignedReason) == L7_SUCCESS) 
            SET_VALID(I_agentDot1xClientVlanAssignedReason, agentDot1xClientConfigEntryData.valid);
        break;
      

    case I_agentDot1xClientSessionTimeout:
        if (snmpagentDot1xClientSessionTimeoutGet(USMDB_UNIT_CURRENT,lIntfNum,
              &agentDot1xClientConfigEntryData.agentDot1xClientSessionTimeout)==L7_SUCCESS) 
             SET_VALID(I_agentDot1xClientSessionTimeout, agentDot1xClientConfigEntryData.valid);
      break;

    case I_agentDot1xClientTerminationAction:
         if (snmpagentDot1xClientTerminationActionGet(USMDB_UNIT_CURRENT,lIntfNum,
            &agentDot1xClientConfigEntryData.agentDot1xClientTerminationAction) == L7_SUCCESS) 
            SET_VALID(I_agentDot1xClientTerminationAction, agentDot1xClientConfigEntryData.valid);
        break;
      break;

     default :
      /* unknown nominator */
      return(NULL);
  }  

  if (nominator != -1 && !VALID(nominator, agentDot1xClientConfigEntryData.valid))
    return NULL;

  return(&agentDot1xClientConfigEntryData);
}


/**********************************************************************************
 *                                                                                *
 *                               DOT1X MONITOR MODE                               *
 *                                                                                * 
 **********************************************************************************/                                   

agentDot1xMonitorModeConfigGroup_t *
k_agentDot1xMonitorModeConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator)
{
  static agentDot1xMonitorModeConfigGroup_t agentDot1xMonitorModeConfigGroupData;
  ZERO_VALID(agentDot1xMonitorModeConfigGroupData.valid);
  L7_BOOL enable; 

  switch (nominator)
  {
    case I_agentDot1xMonitorModeEnabled:
      if(usmDbDot1xMonitorModeGet(USMDB_UNIT_CURRENT, &enable) == L7_SUCCESS)
      {
        if(enable == L7_DISABLE)
        {
          enable = L7_DIAG_DISABLE;
        }
        agentDot1xMonitorModeConfigGroupData.agentDot1xMonitorModeEnabled = enable;
        SET_VALID(I_agentDot1xMonitorModeEnabled, agentDot1xMonitorModeConfigGroupData.valid);
      }
      break;

    case I_agentDot1xMonitorModeClients:
      if(usmDbDot1xMonitorModeNumClientsGet(USMDB_UNIT_CURRENT, 
                                         &agentDot1xMonitorModeConfigGroupData.agentDot1xMonitorModeClients) == L7_SUCCESS)
      {
        SET_VALID(I_agentDot1xMonitorModeClients, agentDot1xMonitorModeConfigGroupData.valid);
      }
      break;

    case I_agentDot1xNonMonitorModeClients:
      if(usmDbDot1xNonMonitorModeNumClientsGet(USMDB_UNIT_CURRENT, 
                                            &agentDot1xMonitorModeConfigGroupData.agentDot1xNonMonitorModeClients) == L7_SUCCESS)
      {
        SET_VALID(I_agentDot1xNonMonitorModeClients, agentDot1xMonitorModeConfigGroupData.valid);
      }
      break;


    default :
      /* unknown nominator */
      return(NULL);
  }

  if ((nominator >= 0) && !VALID(nominator, agentDot1xMonitorModeConfigGroupData.valid))
  { 
    return(NULL);
  }

  return(&agentDot1xMonitorModeConfigGroupData);
}

#ifdef SETS
int
k_agentDot1xMonitorModeConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{
#ifdef NOT_YET
    agentDot1xMonitorModeConfigGroup_t *data = (agentDot1xMonitorModeConfigGroup_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * Perform checks on "object values in isolation".
     */

    if (object->nominator == I_agentDot1xMonitorModeEnabled) {
        /*
         * Examine value->sl_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_agentDot1xMonitorModeEnabled, data->mod);
    }
#endif /* NOT_YET */
    return NO_ERROR;
}

int
k_agentDot1xMonitorModeConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                         doList_t *doHead, doList_t *dp)
{
#ifdef NOT_YET
    agentDot1xMonitorModeConfigGroup_t *data = (agentDot1xMonitorModeConfigGroup_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * The proposed new values are in data.
     * Perform checks on "object value relationships".
     */

    if (VALID(I_agentDot1xMonitorModeEnabled, data->valid)) {
        /*
         * Examine data->agentDot1xMonitorModeEnabled in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
#endif /* NOT_YET */
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDot1xMonitorModeConfigGroup_set(agentDot1xMonitorModeConfigGroup_t *data,
                                       ContextInfo *contextInfo, int function)
{
  L7_BOOL enable;

  if (VALID(I_agentDot1xMonitorModeEnabled, data->valid))
  {
    if(data->agentDot1xMonitorModeEnabled == L7_DIAG_DISABLE)
    {
      enable = L7_DISABLE;
    }      
    else if(data->agentDot1xMonitorModeEnabled == L7_ENABLE)
    {
      enable = L7_ENABLE;
    }      
    else
    {
      return COMMIT_FAILED_ERROR;
    }
    
    if(usmDbDot1xMonitorModeSet(USMDB_UNIT_CURRENT, enable) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentDot1xMonitorModeConfigGroup_UNDO
/* add #define SR_agentDot1xMonitorModeConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDot1xMonitorModeConfigGroup family.
 */
int
agentDot1xMonitorModeConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDot1xMonitorModeConfigGroup_UNDO */

#endif /* SETS */

agentDot1xPortAuthHistoryResultEntry_t *
k_agentDot1xPortAuthHistoryResultEntry_get(int serialNum, ContextInfo *contextInfo,
                                           int nominator,
                                           int searchType,
                                           SR_UINT32 agentDot1xAuthHistoryResultIfaceIndex,
                                           SR_UINT32 agentDot1xAuthHistoryResultIndex)
{
  static agentDot1xPortAuthHistoryResultEntry_t agentDot1xPortAuthHistoryResultEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8  filterName[L7_DOT1X_FILTER_NAME_LEN]={0};
  L7_uint32  len, temp_unsigned;
  L7_enetMacAddr_t macAddr;
  L7_ushort16 vlanId;
  L7_uint8  assignedType;

  if (firstTime == L7_TRUE)
  {
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultIfaceIndex = 0;
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultIndex = 0;
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultTimeStamp = 0;
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultAge = 0;
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultMacAddress = MakeOctetString(NULL, 0);
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultVlanId = 0;
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultAuthStatus = 0;
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultAccessStatus = 0;
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultReasonCode = 0;
    agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultFilterID = MakeOctetString(NULL, 0);
  }
  ZERO_VALID(agentDot1xPortAuthHistoryResultEntryData.valid);

  if(searchType == EXACT)
  {
    if(usmDbDot1xAuthHistoryLogEntryIsValid(USMDB_UNIT_CURRENT,
                                            agentDot1xAuthHistoryResultIfaceIndex,
                                            agentDot1xAuthHistoryResultIndex) == L7_FAILURE)
    {
      ZERO_VALID(agentDot1xPortAuthHistoryResultEntryData.valid); 
      return (NULL);
    }
  }
  else
  {
    if(agentDot1xAuthHistoryResultIndex != 0)
    {
      agentDot1xAuthHistoryResultIndex = agentDot1xAuthHistoryResultIndex-1; /* NextGet returns entryIndex incremented by one */
    }
      
    if(usmDbDot1xAuthHistoryLogReverseIndexNextGet(USMDB_UNIT_CURRENT,
                                                   &agentDot1xAuthHistoryResultIfaceIndex,
                                                   &agentDot1xAuthHistoryResultIndex) != L7_SUCCESS)
    {
      ZERO_VALID(agentDot1xPortAuthHistoryResultEntryData.valid);
      return (NULL);
    }
  }                                   
  agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultIfaceIndex = agentDot1xAuthHistoryResultIfaceIndex;
  agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultIndex = agentDot1xAuthHistoryResultIndex;

  SET_VALID(I_agentDot1xAuthHistoryResultIfaceIndex, agentDot1xPortAuthHistoryResultEntryData.valid);
  SET_VALID(I_agentDot1xAuthHistoryResultIndex, agentDot1xPortAuthHistoryResultEntryData.valid);

  switch (nominator)
  {
    case -1:
    case I_agentDot1xAuthHistoryResultIfaceIndex:
    case I_agentDot1xAuthHistoryResultIndex:
         if (nominator != -1) break;
 
    case I_agentDot1xAuthHistoryResultTimeStamp:
         if(usmDbDot1xAuthHistoryLogTimeStampGet(USMDB_UNIT_CURRENT, 
                       agentDot1xAuthHistoryResultIfaceIndex,
                       agentDot1xAuthHistoryResultIndex, 
                       &temp_unsigned) == L7_SUCCESS)
         {
           if (SafeMakeDateAndTime(&agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultTimeStamp, temp_unsigned) == L7_TRUE)
           {
             SET_VALID(I_agentDot1xAuthHistoryResultTimeStamp, agentDot1xPortAuthHistoryResultEntryData.valid);
           }
         }
         if (nominator != -1) break; 

    case I_agentDot1xAuthHistoryResultAge:
         if(usmDbDot1xAuthHistoryLogTimeStampGet(USMDB_UNIT_CURRENT, 
                       agentDot1xAuthHistoryResultIfaceIndex,
                       agentDot1xAuthHistoryResultIndex, 
                       &temp_unsigned) == L7_SUCCESS)
         {
           L7_uint32 currentTime=0, ageTime=0;

           currentTime = simAdjustedTimeGet();
           ageTime = currentTime-temp_unsigned;

           /* convert Agetime to timeticks (100th of a second) as it is returned in seconds */
           SafeMakeTimeTicksFromSeconds(ageTime,
                             &agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultAge);
           SET_VALID(I_agentDot1xAuthHistoryResultAge, agentDot1xPortAuthHistoryResultEntryData.valid);
         }
         if (nominator != -1) break; 


    case I_agentDot1xAuthHistoryResultMacAddress:
         memset(&macAddr, 0x00, sizeof(macAddr));
         if(usmDbDot1xAuthHistoryLogSupplicantMacAddressGet(USMDB_UNIT_CURRENT, 
                           agentDot1xAuthHistoryResultIfaceIndex,
                           agentDot1xAuthHistoryResultIndex,
                           &macAddr) == L7_SUCCESS)
         {
           if (SafeMakeOctetString(&agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultMacAddress, 
                                   macAddr.addr, L7_MAC_ADDR_LEN) != L7_TRUE)
           {
             return(NULL);
           }
           SET_VALID(I_agentDot1xAuthHistoryResultMacAddress, agentDot1xPortAuthHistoryResultEntryData.valid);
         }
         if (nominator != -1) break; 

    case I_agentDot1xAuthHistoryResultVlanId:
         if(usmDbDot1xAuthHistoryLogVlanIdGet(USMDB_UNIT_CURRENT,
                           agentDot1xAuthHistoryResultIfaceIndex,
                           agentDot1xAuthHistoryResultIndex,
                           &vlanId) == L7_SUCCESS) 
         {
           agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultVlanId = vlanId;
           SET_VALID(I_agentDot1xAuthHistoryResultVlanId, agentDot1xPortAuthHistoryResultEntryData.valid);
         }
         if (nominator != -1) break; 

    case I_agentDot1xAuthHistoryResultAuthStatus:
         if(usmDbDot1xAuthHistoryLogAuthStatusGet(USMDB_UNIT_CURRENT, 
                            agentDot1xAuthHistoryResultIfaceIndex, 
                            agentDot1xAuthHistoryResultIndex,
                            &agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultAuthStatus) == L7_SUCCESS)
         {
           SET_VALID(I_agentDot1xAuthHistoryResultAuthStatus, agentDot1xPortAuthHistoryResultEntryData.valid);
         }
         if (nominator != -1) break; 

    case I_agentDot1xAuthHistoryResultAccessStatus:
         if(usmDbDot1xAuthHistoryLogAccessStatusGet(USMDB_UNIT_CURRENT,
                            agentDot1xAuthHistoryResultIfaceIndex,
                            agentDot1xAuthHistoryResultIndex,
                            &agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultAccessStatus) == L7_SUCCESS)
         {
           SET_VALID(I_agentDot1xAuthHistoryResultAccessStatus, agentDot1xPortAuthHistoryResultEntryData.valid);
         }
         if (nominator != -1) break; 

    case I_agentDot1xAuthHistoryResultReasonCode:
         if(usmDbDot1xAuthHistoryLogReasonCodeGet(USMDB_UNIT_CURRENT,
                            agentDot1xAuthHistoryResultIfaceIndex, 
                            agentDot1xAuthHistoryResultIndex,
                            &agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultReasonCode) == L7_SUCCESS)
         {
           SET_VALID(I_agentDot1xAuthHistoryResultReasonCode, agentDot1xPortAuthHistoryResultEntryData.valid);
         }
         if (nominator != -1) break; 

    case I_agentDot1xAuthHistoryResultFilterID:
         memset(filterName, 0, sizeof(filterName));
         len = sizeof(filterName);
         if(usmDbDot1xAuthHistoryLogFilterIdGet(USMDB_UNIT_CURRENT,
                           agentDot1xAuthHistoryResultIfaceIndex,
                           agentDot1xAuthHistoryResultIndex,
                           filterName, &len) == L7_SUCCESS)
         {
           if(SafeMakeOctetStringFromTextExact(&agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultFilterID,
                                               filterName) != L7_TRUE)
           {
             return NULL;
           }
           SET_VALID(I_agentDot1xAuthHistoryResultFilterID, agentDot1xPortAuthHistoryResultEntryData.valid);
         }
         if (nominator != -1) break; 

     case I_agentDot1xAuthHistoryResultVlanAssigned:
         if(usmDbDot1xAuthHistoryLogVlanIdAssignedGet(USMDB_UNIT_CURRENT,
                                                      agentDot1xAuthHistoryResultIfaceIndex,
                                                      agentDot1xAuthHistoryResultIndex,
                                                      &vlanId) == L7_SUCCESS) 
         {
           agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultVlanAssigned = vlanId;
           SET_VALID(I_agentDot1xAuthHistoryResultVlanAssigned, agentDot1xPortAuthHistoryResultEntryData.valid);
         }
         if (nominator != -1) break; 

     case I_agentDot1xAuthHistoryResultVlanAssignedType:
         if(usmDbDot1xAuthHistoryLogVlanAssignedTypeGet(USMDB_UNIT_CURRENT,
                                                        agentDot1xAuthHistoryResultIfaceIndex,
                                                        agentDot1xAuthHistoryResultIndex,
                                                        &assignedType) == L7_SUCCESS) 
         {
           agentDot1xPortAuthHistoryResultEntryData.agentDot1xAuthHistoryResultVlanAssignedType = assignedType;
           SET_VALID(I_agentDot1xAuthHistoryResultVlanAssignedType, agentDot1xPortAuthHistoryResultEntryData.valid);
         }
         break;

    default:
      /* unknown nominator */
      return(NULL);
  }


  if (nominator >= 0 && !VALID(nominator, agentDot1xPortAuthHistoryResultEntryData.valid))
  {
    return(NULL);
  }

  return(&agentDot1xPortAuthHistoryResultEntryData);
}




agentDot1xPortAuthHistoryResultClearEntry_t *
k_agentDot1xPortAuthHistoryResultClearEntry_get(int serialNum, ContextInfo *contextInfo,
                                                int nominator,
                                                int searchType,
                                                SR_UINT32 agentDot1xAuthHistoryResultIfIndex)
{
   static agentDot1xPortAuthHistoryResultClearEntry_t agentDot1xPortAuthHistoryResultClearEntryData;
   L7_uint32 ifIndex=L7_NULL, entryIndex=L7_NULL;

   if(searchType == EXACT)
   {  
     if(usmDbDot1xAuthHistoryLogEntryIndexNextGet(USMDB_UNIT_CURRENT, 
                                                  &agentDot1xAuthHistoryResultIfIndex, 
                                                  &entryIndex) != L7_SUCCESS)
     {
       ZERO_VALID(agentDot1xPortAuthHistoryResultClearEntryData.valid); 
       return (NULL);
     }
   }
   else
   {
     if(agentDot1xAuthHistoryResultIfIndex != 0)
     {
       agentDot1xAuthHistoryResultIfIndex = agentDot1xAuthHistoryResultIfIndex-1;
     }
     ifIndex = agentDot1xAuthHistoryResultIfIndex;
      
     if(usmDbDot1xAuthHistoryLogIfIndexNextGet(USMDB_UNIT_CURRENT, &ifIndex) != L7_SUCCESS)
     {
       ZERO_VALID(agentDot1xPortAuthHistoryResultClearEntryData.valid); 
       return (NULL);
     }
     agentDot1xAuthHistoryResultIfIndex = ifIndex;
   }

   ZERO_VALID(agentDot1xPortAuthHistoryResultClearEntryData.valid);
   agentDot1xPortAuthHistoryResultClearEntryData.agentDot1xAuthHistoryResultIfIndex = agentDot1xAuthHistoryResultIfIndex;
   SET_VALID(I_agentDot1xAuthHistoryResultIfaceIndex, agentDot1xPortAuthHistoryResultClearEntryData.valid);

   switch (nominator)
   {
     case -1:
     case I_agentDot1xAuthHistoryResultIfaceIndex:
          if (nominator != -1) break;

     case I_agentDot1xPortAuthHistoryResultsClear:
          agentDot1xPortAuthHistoryResultClearEntryData.agentDot1xPortAuthHistoryResultsClear 
                                        = D_agentDot1xPortAuthHistoryResultsClear_disable;
          SET_VALID(I_agentDot1xPortAuthHistoryResultsClear, agentDot1xPortAuthHistoryResultClearEntryData.valid);
          break;

    default:
      /* unknown nominator */
      return(NULL);
  }


  if (nominator >= 0 && !VALID(nominator, agentDot1xPortAuthHistoryResultClearEntryData.valid))
  {
    return(NULL);
  }

  return(&agentDot1xPortAuthHistoryResultClearEntryData);
}


int
k_agentDot1xPortAuthHistoryResultClearEntry_set_defaults(doList_t *dp)
{
    agentDot1xPortAuthHistoryResultClearEntry_t *data = (agentDot1xPortAuthHistoryResultClearEntry_t *) (dp->data);

    data->agentDot1xPortAuthHistoryResultsClear = 2;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDot1xPortAuthHistoryResultClearEntry_set(agentDot1xPortAuthHistoryResultClearEntry_t *data,
                                                ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
   L7_char8 tempValid[sizeof(data->valid)];
   bzero(tempValid, sizeof(tempValid));

   if( VALID(I_agentDot1xPortAuthHistoryResultsClear, data->valid))
   {
     if(usmDbDot1xAuthHistoryLogInterfacePurge(USMDB_UNIT_CURRENT, data->agentDot1xAuthHistoryResultIfIndex) == L7_SUCCESS)
     {
       SET_VALID(I_agentDot1xPortAuthHistoryResultsClear, data->valid);
     }
     else
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
     }
   }

  return NO_ERROR;
}

#ifdef SETS
int
k_agentDot1xPortAuthHistoryResultClearEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                                 doList_t *dp, ContextInfo *contextInfo)
{
#ifdef NOT_YET
    agentDot1xPortAuthHistoryResultClearEntry_t *data = (agentDot1xPortAuthHistoryResultClearEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * Perform checks on "object values in isolation".
     */

    if (object->nominator == I_agentDot1xPortAuthHistoryResultsClear) {
        /*
         * Examine value->sl_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_agentDot1xPortAuthHistoryResultsClear, data->mod);
    }
#endif /* NOT_YET */
    return NO_ERROR;
}

int
k_agentDot1xPortAuthHistoryResultClearEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                                  doList_t *doHead, doList_t *dp)
{
#ifdef NOT_YET
    agentDot1xPortAuthHistoryResultClearEntry_t *data = (agentDot1xPortAuthHistoryResultClearEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * The proposed new values are in data.
     * Perform checks on "object value relationships".
     */

    if (VALID(I_agentDot1xPortAuthHistoryResultsClear, data->valid)) {
        /*
         * Examine data->agentDot1xPortAuthHistoryResultsClear in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
#endif /* NOT_YET */
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

#endif /* SETS */

agentDot1xAuthHistoryResultsGroup_t *
k_agentDot1xAuthHistoryResultsGroup_get(int serialNum, ContextInfo *contextInfo,
                                        int nominator)
{
   static agentDot1xAuthHistoryResultsGroup_t agentDot1xAuthHistoryResultsGroupData;

   ZERO_VALID(agentDot1xAuthHistoryResultsGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
    case -1:
      break;
      /* else pass through */

    case I_agentDot1xAuthHistoryResultsClear:
      agentDot1xAuthHistoryResultsGroupData.agentDot1xAuthHistoryResultsClear = D_agentDot1xAuthHistoryResultsClear_disable;
      SET_VALID(I_agentDot1xAuthHistoryResultsClear, agentDot1xAuthHistoryResultsGroupData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentDot1xAuthHistoryResultsGroupData.valid))
    return(NULL);

  return(&agentDot1xAuthHistoryResultsGroupData);
}

int
k_agentDot1xAuthHistoryResultsGroup_set(agentDot1xAuthHistoryResultsGroup_t *data,
                                        ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
   L7_char8 tempValid[sizeof(data->valid)];
   bzero(tempValid, sizeof(tempValid));

   if( VALID(I_agentDot1xAuthHistoryResultsClear, data->valid))
   {
     if(usmDbDot1xAuthHistoryLogPurgeAll(USMDB_UNIT_CURRENT) == L7_SUCCESS)
     {
       SET_VALID(I_agentDot1xAuthHistoryResultsClear, data->valid);
     }
     else
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
     }
   }

  return NO_ERROR;
}


#ifdef SETS
int
k_agentDot1xAuthHistoryResultsGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *dp, ContextInfo *contextInfo)
{
#ifdef NOT_YET
    agentDot1xAuthHistoryResultsGroup_t *data = (agentDot1xAuthHistoryResultsGroup_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * Perform checks on "object values in isolation".
     */

    if (object->nominator == I_agentDot1xAuthHistoryResultsClear) {
        /*
         * Examine value->sl_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_agentDot1xAuthHistoryResultsClear, data->mod);
    }
#endif /* NOT_YET */
    return NO_ERROR;
}

int
k_agentDot1xAuthHistoryResultsGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                          doList_t *doHead, doList_t *dp)
{
#ifdef NOT_YET
    agentDot1xAuthHistoryResultsGroup_t *data = (agentDot1xAuthHistoryResultsGroup_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * The proposed new values are in data.
     * Perform checks on "object value relationships".
     */

    if (VALID(I_agentDot1xAuthHistoryResultsClear, data->valid)) {
        /*
         * Examine data->agentDot1xAuthHistoryResultsClear in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
#endif /* NOT_YET */
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}
#endif /* SETS */


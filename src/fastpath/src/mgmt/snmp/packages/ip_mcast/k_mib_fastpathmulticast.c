/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_fastpathmulticast.c
*
* @purpose FASTPATH Multicast Private MIB
*
* @component SNMP
*
* @comments
*
* @create
*
* @author cpverne
*
* @end
*
**********************************************************************/
/********************************************************************
 *                    
 *******************************************************************/

#include "k_private_base.h"
#include "k_mib_pim_config.h"
#include "k_mib_fastpathmulticast_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_pimsm_api.h"
#endif


agentMulticastIGMPConfigGroup_t *
k_agentMulticastIGMPConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator)
{
  static agentMulticastIGMPConfigGroup_t agentMulticastIGMPConfigGroupData;

  ZERO_VALID(agentMulticastIGMPConfigGroupData.valid);

  switch (nominator)
  {
  case I_agentMulticastIGMPAdminMode:
    if (snmpAgentMulticastIGMPAdminModeGet(USMDB_UNIT_CURRENT, 
                                           &agentMulticastIGMPConfigGroupData.agentMulticastIGMPAdminMode) == L7_SUCCESS)
    {
      SET_VALID(nominator, agentMulticastIGMPConfigGroupData.valid);
    }
    break;

  default:

    /* unknown nominator */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentMulticastIGMPConfigGroupData.valid))
    return(NULL);

  return(&agentMulticastIGMPConfigGroupData);
}

#ifdef SETS
int
k_agentMulticastIGMPConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentMulticastIGMPConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentMulticastIGMPConfigGroup_set(agentMulticastIGMPConfigGroup_t *data,
                                    ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentMulticastIGMPAdminMode, data->valid) &&
      snmpAgentMulticastIGMPAdminModeSet(USMDB_UNIT_CURRENT,
                                         data->agentMulticastIGMPAdminMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentMulticastIGMPConfigGroup_UNDO
/* add #define SR_agentMulticastIGMPConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentMulticastIGMPConfigGroup family.
 */
int
agentMulticastIGMPConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentMulticastIGMPConfigGroup_UNDO */

#endif /* SETS */

agentMulticastIGMPInterfaceEntry_t *
k_agentMulticastIGMPInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_INT32 agentMulticastIGMPInterfaceIfIndex)
{
   return(NULL);
}

#ifdef SETS
int
k_agentMulticastIGMPInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentMulticastIGMPInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentMulticastIGMPInterfaceEntry_set_defaults(doList_t *dp)
{
    return NO_ERROR;
}

int
k_agentMulticastIGMPInterfaceEntry_set(agentMulticastIGMPInterfaceEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{
   return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentMulticastIGMPInterfaceEntry_UNDO
/* add #define SR_agentMulticastIGMPInterfaceEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentMulticastIGMPInterfaceEntry family.
 */
int
agentMulticastIGMPInterfaceEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentMulticastIGMPInterfaceEntry_UNDO */

#endif /* SETS */

agentMulticastPIMConfigGroup_t *
k_agentMulticastPIMConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{
  return(NULL);
}

#ifdef SETS
int
k_agentMulticastPIMConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentMulticastPIMConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentMulticastPIMConfigGroup_set(agentMulticastPIMConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{
  return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentMulticastPIMConfigGroup_UNDO
/* add #define SR_agentMulticastPIMConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentMulticastPIMConfigGroup family.
 */
int
agentMulticastPIMConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentMulticastPIMConfigGroup_UNDO */

#endif /* SETS */

agentMulticastPIMSMConfigGroup_t *
k_agentMulticastPIMSMConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{
  static agentMulticastPIMSMConfigGroup_t agentMulticastPIMSMConfigGroupData;

  ZERO_VALID(agentMulticastPIMSMConfigGroupData.valid);

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
    return(NULL);

  switch (nominator)
  {
  case I_agentMulticastPIMSMAdminMode:
    if (snmpAgentMulticastPIMSMAdminModeGet(USMDB_UNIT_CURRENT, 
                                           &agentMulticastPIMSMConfigGroupData.agentMulticastPIMSMAdminMode) == L7_SUCCESS)
    {
      SET_VALID(nominator, agentMulticastPIMSMConfigGroupData.valid);
    }
    break;
  case I_agentMulticastPIMSMDataThresholdRate:
    if (usmDbPimsmDataThresholdRateGet(USMDB_UNIT_CURRENT, L7_AF_INET,
                                       &agentMulticastPIMSMConfigGroupData.agentMulticastPIMSMDataThresholdRate) == L7_SUCCESS)
    {
      SET_VALID(nominator, agentMulticastPIMSMConfigGroupData.valid);
    }
    break;

  case I_agentMulticastPIMSMRegThresholdRate:
    if (usmDbPimsmRegisterThresholdRateGet(USMDB_UNIT_CURRENT, L7_AF_INET,
                                           &agentMulticastPIMSMConfigGroupData.agentMulticastPIMSMRegThresholdRate) == L7_SUCCESS)
    {
      SET_VALID(nominator, agentMulticastPIMSMConfigGroupData.valid);
    }
    break;

  default:

    /* unknown nominator */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentMulticastPIMSMConfigGroupData.valid))
    return(NULL);

  return(&agentMulticastPIMSMConfigGroupData);
}

#ifdef SETS
int
k_agentMulticastPIMSMConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentMulticastPIMSMConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentMulticastPIMSMConfigGroup_set(agentMulticastPIMSMConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_PIMSM_MAP_COMPONENT_ID) != L7_TRUE)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentMulticastPIMSMAdminMode, data->valid) &&
      snmpAgentMulticastPIMSMAdminModeSet(USMDB_UNIT_CURRENT, 
                                          data->agentMulticastPIMSMAdminMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
  
  if (VALID(I_agentMulticastPIMSMDataThresholdRate, data->valid) &&
      usmDbPimsmDataThresholdRateSet(USMDB_UNIT_CURRENT, L7_AF_INET,
                                     data->agentMulticastPIMSMDataThresholdRate) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentMulticastPIMSMRegThresholdRate, data->valid) &&
      usmDbPimsmRegisterThresholdRateSet(USMDB_UNIT_CURRENT, L7_AF_INET,
                                         data->agentMulticastPIMSMRegThresholdRate) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;


       return NO_ERROR;

}

#ifdef SR_agentMulticastPIMSMConfigGroup_UNDO
/* add #define SR_agentMulticastPIMSMConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentMulticastPIMSMConfigGroup family.
 */
int
agentMulticastPIMSMConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentMulticastPIMSMConfigGroup_UNDO */

#endif /* SETS */

agentMulticastPIMSMStaticRPEntry_t *
k_agentMulticastPIMSMStaticRPEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_UINT32 agentMulticastPIMSMStaticRPIpAddr,
                                       SR_UINT32 agentMulticastPIMSMStaticRPGroupIpAddr,
                                       SR_UINT32 agentMulticastPIMSMStaticRPGroupIpMask)
{
   return(NULL);
}

#ifdef SETS
int
k_agentMulticastPIMSMStaticRPEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentMulticastPIMSMStaticRPEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                         doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentMulticastPIMSMStaticRPEntry_set_defaults(doList_t *dp)
{
    return NO_ERROR;
}

int
k_agentMulticastPIMSMStaticRPEntry_set(agentMulticastPIMSMStaticRPEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{
   return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentMulticastPIMSMStaticRPEntry_UNDO
/* add #define SR_agentMulticastPIMSMStaticRPEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentMulticastPIMSMStaticRPEntry family.
 */
int
agentMulticastPIMSMStaticRPEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentMulticastPIMSMStaticRPEntry_UNDO */

#endif /* SETS */

agentMulticastPIMSMInterfaceEntry_t *
k_agentMulticastPIMSMInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
                                        int nominator,
                                        int searchType,
                                        SR_UINT32 agentMulticastPIMSMInterfaceIndex)
{
    return(NULL);
}

#ifdef SETS
int
k_agentMulticastPIMSMInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentMulticastPIMSMInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                          doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentMulticastPIMSMInterfaceEntry_set_defaults(doList_t *dp)
{
    return NO_ERROR;
}

int
k_agentMulticastPIMSMInterfaceEntry_set(agentMulticastPIMSMInterfaceEntry_t *data,
                                        ContextInfo *contextInfo, int function)
{
   return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentMulticastPIMSMInterfaceEntry_UNDO
/* add #define SR_agentMulticastPIMSMInterfaceEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentMulticastPIMSMInterfaceEntry family.
 */
int
agentMulticastPIMSMInterfaceEntry_undo(doList_t *doHead, doList_t *doCur,
                                       ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentMulticastPIMSMInterfaceEntry_UNDO */

#endif /* SETS */

agentMulticastPIMDMConfigGroup_t *
k_agentMulticastPIMDMConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{
  static agentMulticastPIMDMConfigGroup_t agentMulticastPIMDMConfigGroupData;

  ZERO_VALID(agentMulticastPIMDMConfigGroupData.valid);

    /* To check whether PIMDM component is present or not.
       If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_PIMDM_MAP_COMPONENT_ID) != L7_TRUE)
      return (NULL);

  switch (nominator)
  {
  case I_agentMulticastPIMDMAdminMode:
    if (snmpAgentMulticastPIMDMAdminModeGet(USMDB_UNIT_CURRENT, 
                                           &agentMulticastPIMDMConfigGroupData.agentMulticastPIMDMAdminMode) == L7_SUCCESS)
    {
      SET_VALID(nominator, agentMulticastPIMDMConfigGroupData.valid);
    }
    break;

  default:

    /* unknown nominator */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentMulticastPIMDMConfigGroupData.valid))
    return(NULL);

  return(&agentMulticastPIMDMConfigGroupData);
}

#ifdef SETS
int
k_agentMulticastPIMDMConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentMulticastPIMDMConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentMulticastPIMDMConfigGroup_set(agentMulticastPIMDMConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{
    /* To check whether PIMDM component is present or not.
       If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_PIMDM_MAP_COMPONENT_ID) != L7_TRUE)
      return COMMIT_FAILED_ERROR;

  if (VALID(I_agentMulticastPIMDMAdminMode, data->valid) &&
      snmpAgentMulticastPIMDMAdminModeSet(USMDB_UNIT_CURRENT, 
                                          data->agentMulticastPIMDMAdminMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentMulticastPIMDMConfigGroup_UNDO
/* add #define SR_agentMulticastPIMDMConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentMulticastPIMDMConfigGroup family.
 */
int
agentMulticastPIMDMConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentMulticastPIMDMConfigGroup_UNDO */

#endif /* SETS */

agentMulticastRoutingConfigGroup_t *
k_agentMulticastRoutingConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator)
{
  static agentMulticastRoutingConfigGroup_t agentMulticastRoutingConfigGroupData;

  ZERO_VALID(agentMulticastRoutingConfigGroupData.valid);

  switch (nominator)
  {
  case I_agentMulticastRoutingAdminMode:
    if (snmpAgentMulticastRoutingAdminModeGet(USMDB_UNIT_CURRENT,
                                           &agentMulticastRoutingConfigGroupData.agentMulticastRoutingAdminMode) == L7_SUCCESS)
    {
      SET_VALID(nominator, agentMulticastRoutingConfigGroupData.valid);
    }
    break;

  default:

    /* unknown nominator */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentMulticastRoutingConfigGroupData.valid))
    return(NULL);

  return(&agentMulticastRoutingConfigGroupData);

}

#ifdef SETS
int
k_agentMulticastRoutingConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentMulticastRoutingConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                         doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentMulticastRoutingConfigGroup_set(agentMulticastRoutingConfigGroup_t *data,
                                       ContextInfo *contextInfo, int function)
{
    if (VALID(I_agentMulticastRoutingAdminMode, data->valid) &&
      snmpAgentMulticastRoutingAdminModeSet(USMDB_UNIT_CURRENT,
                                          data->agentMulticastRoutingAdminMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;


    return NO_ERROR;
}

#ifdef SR_agentMulticastRoutingConfigGroup_UNDO
/* add #define SR_agentMulticastRoutingConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentMulticastRoutingConfigGroup family.
 */
int
agentMulticastRoutingConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentMulticastRoutingConfigGroup_UNDO */

#endif /* SETS */

agentMulticastDVMRPConfigGroup_t *
k_agentMulticastDVMRPConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{
  static agentMulticastDVMRPConfigGroup_t agentMulticastDVMRPConfigGroupData;

  ZERO_VALID(agentMulticastDVMRPConfigGroupData.valid);

     /* To check whether DVMRP component is present or not.
       If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
     if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_DVMRP_MAP_COMPONENT_ID) != L7_TRUE)
          return(NULL);

  switch (nominator)
  {
  case I_agentMulticastDVMRPAdminMode:
    if (snmpAgentMulticastDVMRPAdminModeGet(USMDB_UNIT_CURRENT, 
                                           &agentMulticastDVMRPConfigGroupData.agentMulticastDVMRPAdminMode) == L7_SUCCESS)
    {
      SET_VALID(nominator, agentMulticastDVMRPConfigGroupData.valid);
    }
    break;

  default:

    /* unknown nominator */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentMulticastDVMRPConfigGroupData.valid))
    return(NULL);

  return(&agentMulticastDVMRPConfigGroupData);
}

#ifdef SETS
int
k_agentMulticastDVMRPConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentMulticastDVMRPConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentMulticastDVMRPConfigGroup_set(agentMulticastDVMRPConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{
     /* To check whether DVMRP component is present or not.
       If a valid key is entered, usmDbComponentPresentCheck() returns L7_TRUE*/
     if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_DVMRP_MAP_COMPONENT_ID) != L7_TRUE)
          return COMMIT_FAILED_ERROR;

  if (VALID(I_agentMulticastDVMRPAdminMode, data->valid) &&
      snmpAgentMulticastDVMRPAdminModeSet(USMDB_UNIT_CURRENT, 
                                          data->agentMulticastDVMRPAdminMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentMulticastDVMRPConfigGroup_UNDO
/* add #define SR_agentMulticastDVMRPConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentMulticastDVMRPConfigGroup family.
 */
int
agentMulticastDVMRPConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentMulticastDVMRPConfigGroup_UNDO */

#endif /* SETS */

agentSnmpTrapFlagsConfigGroupMulticast_t *
k_agentSnmpTrapFlagsConfigGroupMulticast_get(int serialNum, ContextInfo *contextInfo,
                                             int nominator)
{
   static agentSnmpTrapFlagsConfigGroupMulticast_t agentSnmpTrapFlagsConfigGroupMulticastData;

  ZERO_VALID(agentSnmpTrapFlagsConfigGroupMulticastData.valid);

  switch (nominator)
  {
  case I_agentSnmpDVMRPTrapFlag:
    if (snmpAgentDVMRPTrapModeGet(USMDB_UNIT_CURRENT,
                                  &agentSnmpTrapFlagsConfigGroupMulticastData.agentSnmpDVMRPTrapFlag) == L7_SUCCESS)
    {
      SET_VALID(nominator, agentSnmpTrapFlagsConfigGroupMulticastData.valid);
    }
    break;

  case I_agentSnmpPIMTrapFlag:
    if (snmpAgentPIMTrapModeGet(USMDB_UNIT_CURRENT,
                                &agentSnmpTrapFlagsConfigGroupMulticastData.agentSnmpPIMTrapFlag) == L7_SUCCESS)
    {
      SET_VALID(nominator, agentSnmpTrapFlagsConfigGroupMulticastData.valid);
    }
    break;

    /* unknown nominator */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentSnmpTrapFlagsConfigGroupMulticastData.valid))
    return(NULL);

  return(&agentSnmpTrapFlagsConfigGroupMulticastData);
 
}

#ifdef SETS
int
k_agentSnmpTrapFlagsConfigGroupMulticast_test(ObjectInfo *object, ObjectSyntax *value,
                                              doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentSnmpTrapFlagsConfigGroupMulticast_ready(ObjectInfo *object, ObjectSyntax *value, 
                                               doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSnmpTrapFlagsConfigGroupMulticast_set(agentSnmpTrapFlagsConfigGroupMulticast_t *data,
                                             ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentSnmpDVMRPTrapFlag, data->valid) &&
      snmpAgentDVMRPTrapModeSet(USMDB_UNIT_CURRENT,
                                data->agentSnmpDVMRPTrapFlag) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSnmpPIMTrapFlag, data->valid) &&
      snmpAgentPIMTrapModeSet(USMDB_UNIT_CURRENT,
                              data->agentSnmpPIMTrapFlag) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

   return NO_ERROR;

}

#ifdef SR_agentSnmpTrapFlagsConfigGroupMulticast_UNDO
/* add #define SR_agentSnmpTrapFlagsConfigGroupMulticast_UNDO in sitedefs.h to
 * include the undo routine for the agentSnmpTrapFlagsConfigGroupMulticast family.
 */
int
agentSnmpTrapFlagsConfigGroupMulticast_undo(doList_t *doHead, doList_t *doCur,
                                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSnmpTrapFlagsConfigGroupMulticast_UNDO */

#endif /* SETS */

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_telnet_private.c
*
* @purpose  Provide interface to Outbound Telnet Private MIB
*
* @component SNMP
*
* @comments
*
* @create 29/03/2004
*
* @author Anindya Sarkar
*
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "k_mib_fastpath_telnet_api.h"
#include "cnfgr.h"
#include "usmdb_common.h"
#include "usmdb_telnet_api.h"

agentOutboundTelnetGroup_t *
k_agentOutboundTelnetGroup_get(int serialNum, ContextInfo *contextInfo,
                           int nominator)
{

   static agentOutboundTelnetGroup_t agentOutboundTelnetGroupData;

   switch (nominator)
  {
  case -1:
    break;

  case I_agentOutboundTelnetAdminMode:
    if (snmpAgentOutboundTelnetAdminModeGet(USMDB_UNIT_CURRENT, &agentOutboundTelnetGroupData.agentOutboundTelnetAdminMode) == L7_SUCCESS)
      SET_VALID(I_agentOutboundTelnetAdminMode, agentOutboundTelnetGroupData.valid);
    break;

  case I_agentOutboundTelnetMaxNoOfSessions:
    if (usmDbTelnetMaxSessionsGet(USMDB_UNIT_CURRENT, &agentOutboundTelnetGroupData.agentOutboundTelnetMaxNoOfSessions) == L7_SUCCESS)
      SET_VALID(I_agentOutboundTelnetMaxNoOfSessions, agentOutboundTelnetGroupData.valid);
    break;

   case I_agentOutboundTelnetTimeout:
    if (usmDbTelnetTimeoutGet(USMDB_UNIT_CURRENT, &agentOutboundTelnetGroupData.agentOutboundTelnetTimeout) == L7_SUCCESS)
      SET_VALID(I_agentOutboundTelnetTimeout, agentOutboundTelnetGroupData.valid);
    break;

  default:
     /* unknown nominator */
     return(NULL);
     break;
   }
   if (nominator >= 0 && !VALID(nominator, agentOutboundTelnetGroupData.valid))
     return(NULL);
   return(&agentOutboundTelnetGroupData);

}


#ifdef SETS
int
k_agentOutboundTelnetGroup_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentOutboundTelnetGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentOutboundTelnetGroup_set(agentOutboundTelnetGroup_t *data,
                           ContextInfo *contextInfo, int function)
{

   if (VALID(I_agentOutboundTelnetAdminMode, data->valid) &&
      snmpAgentOutboundTelnetAdminModeSet(USMDB_UNIT_CURRENT, data->agentOutboundTelnetAdminMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOutboundTelnetAdminMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOutboundTelnetMaxNoOfSessions, data->valid) &&
     snmpAgentOutboundTelnetMaxNoOfSessionsSet(USMDB_UNIT_CURRENT, data->agentOutboundTelnetMaxNoOfSessions) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOutboundTelnetMaxNoOfSessions, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentOutboundTelnetTimeout, data->valid) &&
     snmpAgentOutboundTelnetTimeoutSet(USMDB_UNIT_CURRENT, data->agentOutboundTelnetTimeout) != L7_SUCCESS)
  {
    CLR_VALID(I_agentOutboundTelnetTimeout, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;

}

#ifdef SR_agentOutboundTelnetGroup_UNDO
/* add #define SR_agentOutboundTelnetGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentOutboundTelnetGroup family.
 */
int
agentOutboundTelnetGroup_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentOutboundTelnetGroup_UNDO */

#endif /* SETS */

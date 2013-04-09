/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2009
*
**********************************************************************
*
* @filename   k_mib_fastpath_pfc_api.h
*
* @purpose    Wrapper functions for Fastpath PFC Private MIB
*
* @component  SNMP
*
* @comments
*
* @create     6/02/2009
*
* @author     Visakha Erina
* @end
*
**********************************************************************/

#include "l7_common.h"


L7_RC_t
snmpAgentPfcIntfAdminModeGet(L7_uint32 intIfNum, L7_uint32 *val);
   
L7_RC_t
snmpAgentPfcIntfAdminModeSet(L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpAgentPfcIntfPfcStatusGet(L7_uint32 intIfNum, L7_uint32 *val);
 
L7_RC_t
snmpAgentPfcEntryGet(L7_uint32 agentPfcIntfIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentPfcEntryNextGet(L7_uint32 *agentPfcIntfIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentPfcActionGet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 *val);
   
L7_RC_t
snmpAgentPfcActionSet(L7_uint32 intIfNum, L7_uint32 priority, L7_uint32 val);

L7_RC_t
snmpAgentPfcActionEntryGet(L7_uint32 agentPfcIntfIndex, 
                           L7_uint32 agentPfcPriority, 
                           L7_uint32 *intIfNum);

L7_RC_t
snmpAgentPfcActionEntryNextGet(L7_uint32 *agentPfcIntfIndex, 
                               L7_uint32 *agentPfcPriorityIndex, 
                               L7_uint32 *intIfNum);

L7_RC_t
snmpAgentPfcIntfStatsPerPriorityEntryGet(L7_uint32 agentPfcIntfIndex, 
                                         L7_uint32 agentPfcPriority, 
                                         L7_uint32 *intIfNum);

L7_RC_t
snmpAgentPfcIntfStatsPerPriorityEntryNextGet(L7_uint32 *agentPfcIntfIndex, 
                                             L7_uint32 *agentPfcPriorityIndex, 
                                             L7_uint32 *intIfNum);


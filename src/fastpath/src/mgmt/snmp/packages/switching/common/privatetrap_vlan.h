/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap_vlan.h
*
* Purpose: VLAN-related trap functions
*
* Created by: Mike Fiorito 09/19/2001
*
* Component: SNMP
*
*********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef PRIVATETRAP_VLAN_H
#define PRIVATETRAP_VLAN_H

#include "l7_common.h"

/* Begin Function Declarations: privatetrap_vlan.h */
/*********************************************************************
*
* @purpose  Creates an SNMP trap OID and a VarBind structure and sends
*           them as a VLAN-related trap.
*
* @param    vlanIndex       VLAN Index
* @param    snmpTrapOIDDot  character string form of an OID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t snmp_VlanTrapSend(L7_int32 instanceID, L7_uchar8 *snmpTrapOIDDot);

/*********************************************************************
*
* @purpose  Signifies that there has been an error processing a VLAN
*           configuration request.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t snmp_VlanRequestFailureTrapSend(L7_int32 vlanIndex);

/*********************************************************************
*
* @purpose  Signifies that the last (or Default) VLAN is being deleted.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t snmp_VlanDeleteLastTrapSend( L7_int32 vlanIndex );

/*********************************************************************
*
* @purpose  Signifies that the default VLAN configuration has failed.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t snmp_VlanDefaultCfgFailureTrapSend( L7_int32 vlanIndex );

/*********************************************************************
*
* @purpose  Signifies that there has been an error restoring the prior
*           VLAN configuration
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t snmp_VlanRestoreFailureTrapSend( L7_int32 vlanIndex );

/*********************************************************************
*
* @purpose  Signifies that there is a new root bridge for the STP instance
*           associated with a particular VLAN.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t snmp_StpInstanceNewRootTrapSend(L7_int32 instanceID);

/*********************************************************************
*
* @purpose  Signifies that this port in this MSTP instance enters
            loop inconsistent state upon failure to receive a BPDU.
*
* @param    instanceID     MST Instance Index
* @param    intIfNum       Internal interface number          
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t snmp_StpInstanceLoopInconsistentStartTrapSend(L7_int32 instanceID, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Signifies that this port in this MSTP instance exits
            loop inconsistent state upon reception of a BPDU.
*
* @param    instanceID     MST Instance Index
* @param    intIfNum       Internal interface number          
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t snmp_StpInstanceLoopInconsistentEndTrapSend(L7_int32 instanceID, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Signifies that there has been a topology change for the
*           STP instance associated with a particular VLAN.
*
* @param    vlanIndex      VLAN Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t snmp_StpInstanceTopologyChangeTrapSend(L7_int32 instanceID);

#endif /* PRIVATETRAP_VLAN_H */

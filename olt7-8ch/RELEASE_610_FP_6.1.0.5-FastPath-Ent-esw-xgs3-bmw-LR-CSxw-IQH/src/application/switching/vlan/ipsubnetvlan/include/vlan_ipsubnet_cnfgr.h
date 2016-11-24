/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_cnfgr.h
*
* @purpose   IP Subnet Vlan configurator
*
* @component vlanIpSubnet
*
* @comments 
*
* @create    5/5/2005
*
* @author    tsrikanth
*
* @end
*             
**********************************************************************/

#ifndef VLAN_IPSUBNET_CNFGR_H
#define VLAN_IPSUBNET_CNFGR_H

#include "l7_cnfgr_api.h"

#define VLAN_IPSUBNET_IS_READY \
    (((vlanIpSubnetCnfgrState == VLAN_IPSUBNET_PHASE_WMU) || \
    (vlanIpSubnetCnfgrState == VLAN_IPSUBNET_PHASE_INIT_3) || \
    (vlanIpSubnetCnfgrState == VLAN_IPSUBNET_PHASE_EXECUTE) || \
    (vlanIpSubnetCnfgrState == VLAN_IPSUBNET_PHASE_UNCONFIG_1)) \
    ? (L7_TRUE) : (L7_FALSE))

typedef enum
{
    VLAN_IPSUBNET_PHASE_INIT_0 = 0,
    VLAN_IPSUBNET_PHASE_INIT_1,
    VLAN_IPSUBNET_PHASE_INIT_2,
    VLAN_IPSUBNET_PHASE_WMU,
    VLAN_IPSUBNET_PHASE_INIT_3,
    VLAN_IPSUBNET_PHASE_EXECUTE,
    VLAN_IPSUBNET_PHASE_UNCONFIG_1,
    VLAN_IPSUBNET_PHASE_UNCONFIG_2,
} vlanIpSubnetCnfgrState_t;


/* vlan_ipsubnet_cnfgr.h */

void vlanIpSubnetApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t vlanIpSubnetCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t vlanIpSubnetCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t vlanIpSubnetCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason );
void vlanIpSubnetCnfgrFiniPhase1Process();
void vlanIpSubnetCnfgrFiniPhase2Process();
void vlanIpSubnetCnfgrFiniPhase3Process();
L7_RC_t vlanIpSubnetCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t vlanIpSubnetCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason );


#endif /* VLAN_IPSUBNET_CNFGR_H */

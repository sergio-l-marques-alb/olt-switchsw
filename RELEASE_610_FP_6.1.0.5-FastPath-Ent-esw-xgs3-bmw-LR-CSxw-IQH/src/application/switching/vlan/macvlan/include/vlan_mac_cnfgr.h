/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_cnfgr.h
*
* @purpose   Mac Vlan Configurator file
*
* @component vlanMac
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

#ifndef VLAN_MAC_CNFGR_H
#define VLAN_MAC_CNFGR_H

#include "l7_cnfgr_api.h"

#define macvlanTree_t          avlTree_t
#define macvlanTreeTables_t    avlTreeTables_t

#define VLAN_MAC_IS_READY \
    (((vlanMacCnfgrState == VLAN_MAC_PHASE_WMU) || \
    (vlanMacCnfgrState == VLAN_MAC_PHASE_INIT_3) || \
    (vlanMacCnfgrState == VLAN_MAC_PHASE_EXECUTE) || \
    (vlanMacCnfgrState == VLAN_MAC_PHASE_UNCONFIG_1)) \
    ? (L7_TRUE) : (L7_FALSE))

typedef enum
{
    VLAN_MAC_PHASE_INIT_0 = 0,
    VLAN_MAC_PHASE_INIT_1,
    VLAN_MAC_PHASE_INIT_2,
    VLAN_MAC_PHASE_WMU,
    VLAN_MAC_PHASE_INIT_3,
    VLAN_MAC_PHASE_EXECUTE,
    VLAN_MAC_PHASE_UNCONFIG_1,
    VLAN_MAC_PHASE_UNCONFIG_2,
} vlanMacCnfgrState_t;



/*  Function Prototypes */

void vlanMacApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t vlanMacCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t vlanMacCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                      L7_CNFGR_ERR_RC_t * pReason);
L7_RC_t vlanMacCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t *pReason);
void vlanMacCnfgrFiniPhase1Process();
void vlanMacCnfgrFiniPhase2Process();
void vlanMacCnfgrFiniPhase3Process();
L7_RC_t vlanMacCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t vlanMacCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t *pReason);


#endif /* VLAN_MAC_CNFGR_H */

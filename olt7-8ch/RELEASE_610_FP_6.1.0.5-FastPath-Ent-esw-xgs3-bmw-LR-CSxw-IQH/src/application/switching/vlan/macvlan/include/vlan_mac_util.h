/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_util.h
*
* @purpose   MAC Vlan utility file
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

#ifndef VLAN_MAC_UTIL_H
#define VLAN_MAC_UTIL_H

                             
/* vlan_mac_cfg.h */

void vlanMacBuildDefaultConfigData(L7_uint32 ver);
L7_BOOL vlanMacHasDataChanged();
void vlanMacResetDataChanged(void);
L7_RC_t vlanMacRestore();
L7_RC_t vlanMacSave();
L7_RC_t vlanMacApplyConfigData();
void vlanMacCfgRemove();

                             
/* vlan_mac_data.h */

void *vlanMacDataSearch(void *pTree, L7_enetMacAddr_t mac);
L7_RC_t vlanMacEntryAdd(avlTree_t *pTree, void *pData);
L7_RC_t vlanMacEntryDelete(avlTree_t *pTree, L7_enetMacAddr_t mac);
L7_uint32 vlanMacDataCount(avlTree_t * pTree);
void * vlanMacFirstDataEntry(avlTree_t *pTree);
void * vlanMacNextDataEntry(avlTree_t *pTree, L7_enetMacAddr_t mac);
void *  vlanMacDataEntry(avlTree_t *pTree, L7_enetMacAddr_t mac);

/* vlan_mac_data.h */
L7_RC_t vlanMacCfgDump();
L7_RC_t vlanMacInfoDump();
void vlanMacBuildTestVlanData(vlanMacCfgData_t *pCfg, L7_uint32 seed);
void vlanMacBuildTestConfigData(void);
void vlanMacConfigDataTestShow(void);

/* vlan_mac_util.h */
L7_RC_t vlanMacAddApply(L7_enetMacAddr_t mac, L7_uint32 vlanId);
void vlanMacDeleteApply(L7_enetMacAddr_t mac, L7_uint32 vlanId);
void vlanMacCfgEntryAdd(L7_enetMacAddr_t mac, L7_uint32 vlanId);
void vlanMacCfgEntryDelete(L7_enetMacAddr_t mac, L7_uint32 vlanId);
L7_RC_t vlanMacVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
                                  L7_uint32 event);
#endif /* VLAN_MAC_UTIL_H */

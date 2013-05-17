/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_util.h
*
* @purpose   IP Subnet Vlan utility file
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

#ifndef VLAN_IPSUBNET_UTIL_H
#define VLAN_IPSUBNET_UTIL_H

#define VLAN_IPSUBNET_IPADDR_CLASSA_BEG       0x00000000      /* 0.0.0.0 */
#define VLAN_IPSUBNET_IPADDR_CLASSA_END       0x7fffffff      /* 127.255.255.255 */
#define VLAN_IPSUBNET_IPADDR_CLASSB_BEG       0x80000000      /* 128.0.0.0 */
#define VLAN_IPSUBNET_IPADDR_CLASSB_END       0xbfffffff      /* 191.255.255.255 */
#define VLAN_IPSUBNET_IPADDR_CLASSC_BEG       0xc0000000      /* 192.0.0.0 */
#define VLAN_IPSUBNET_IPADDR_CLASSC_END       0xdfffffff      /* 223.255.255.255 */
#define VLAN_IPSUBNET_ISBITON(word, pos)   (((word)>>(pos) & 0x01) == 0x01)
#define VLAN_IPSUBNET_ISBITOFF(word, pos)  (((word)>>(pos) & 0x01) == 0x00)




/* vlan_ipsubnet_cfg.h */

void vlanIpSubnetBuildDefaultConfigData(L7_uint32 ver);
L7_BOOL vlanIpSubnetHasDataChanged();
void vlanIpSubnetResetDataChanged(void);
L7_RC_t vlanIpSubnetRestore();
L7_RC_t vlanIpSubnetSave();
L7_RC_t vlanIpSubnetApplyConfigData();
void vlanIpSubnetCfgRemove();

/* vlan_ipsubnet_data.h */

void *vlanIpSubnetDataSearch(void *pTree, L7_uint32 subnet, L7_uint32 netmask);
L7_RC_t vlanIpSubnetEntryAdd(avlTree_t *pTree, void *pData);
L7_RC_t vlanIpSubnetEntryDelete(avlTree_t *pTree, L7_uint32 subnet, L7_uint32 netmask);
L7_uint32 vlanIpSubnetDataCount(avlTree_t *pTree);
void * vlanIpSubnetFirstDataEntry(avlTree_t *pTree);
void *  vlanIpSubnetNextDataEntry(avlTree_t *pTree, L7_uint32 subnet, L7_uint32 netmask);
void *  vlanIpSubnetDataEntry(avlTree_t *pTree, L7_uint32 subnet, L7_uint32 netmask);

/* vlan_ipsubnet_debug.h */

L7_RC_t vlanIpSubnetCfgDump();
L7_RC_t vlanIpSubnetInfoDump();
void vlanIpSubnetBuildTestVlanData(vlanIpSubnetCfgData_t *pCfg, L7_uint32 seed);
void vlanIpSubnetBuildTestConfigData(void);
void vlanIpSubnetConfigDataTestShow(void);


/* vlan_ipsubnet_util.h */

void vlanIpSubnetSubnetAddApply(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId);
void vlanIpSubnetSubnetDeleteApply(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId);
void vlanIpSubnetCfgEntryAdd(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId);
void vlanIpSubnetCfgEntryDelete(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId);
L7_RC_t vlanIpSubnetVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event);
L7_uint32 vlanIpTxtCfgApplyCompletionCallback(L7_uint32 event);
L7_BOOL vlanIpSubnetIsNetMaskContig(L7_uint32 netmask, L7_uint32 *numOfOnes);
L7_BOOL vlanIpSubnetSubnetValid(L7_uint32 subnetip, L7_uint32 netmask);
L7_uint32 vlanIpSubnetFindSubnet(L7_uint32 ipaddr, L7_uint32 netmask);
L7_BOOL vlanIpSubnetFindSubnetOverlap(L7_uint32 subnet1, L7_uint32 netmask1,
                                      L7_uint32 subnet2, L7_uint32 netmask2);


#endif /* VLAN_IPSUBNET_UTIL_H */

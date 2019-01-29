/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_cfg.h
*
* @purpose   Mac   Vlan  Configuration file
*
* @component vlanMac
*
* @comments
*
* @create    05/20/2005
*
* @author    
*
* @end
*
**********************************************************************/

#ifndef VLAN_MAC_CFG_H
#define VLAN_MAC_CFG_H


#define VLAN_MAC_CFG_FILENAME            "ipMacVlan.cfg"
#define VLAN_MAC_CFG_VER_1               0x1
#define VLAN_MAC_CFG_VER_CURRENT         VLAN_MAC_CFG_VER_1


/* Configuration data */
typedef struct vlanMacCfgData_s
{
    L7_enetMacAddr_t mac;
    L7_uint32        vlanId;
} vlanMacCfgData_t;

typedef struct
{
    L7_fileHdr_t     cfgHdr;
    vlanMacCfgData_t macVlanInfo[L7_VLAN_MAC_MAX_VLANS];
    L7_uint32        checkSum;
} vlanMacCfg_t;

typedef struct vlanMacOperData_s
{
    L7_enetMacAddr_t	mac;
    L7_uint32        	vlanId;
    L7_uchar8       	inUse;
	/* This field is needed by the AVL Library */
	void         		*avlNext;
} vlanMacOperData_t;


#endif /* VLAN_MAC_CFG_H */

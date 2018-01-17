/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_cfg.h
*
* @purpose   IP Subnet Vlan configuration  file
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


#ifndef VLAN_IPSUBNET_CFG_H
#define VLAN_IPSUBNET_CFG_H


#define VLAN_IPSUBNET_CFG_FILENAME       "ipSubnetVlan.cfg"
#define VLAN_IPSUBNET_CFG_VER_1          0x1
#define VLAN_IPSUBNET_CFG_VER_CURRENT    VLAN_IPSUBNET_CFG_VER_1


#define VLAN_IPSUBNET_KEYSIZE            8

/* Configuration data */

typedef struct vlanIpSubnetCfgData_s
{
    L7_IP_ADDR_t ipSubnet;
    L7_IP_MASK_t netMask;
    L7_uint32    vlanId;
} vlanIpSubnetCfgData_t;
typedef struct vlanIpSubnetOperData_s
{
    L7_IP_ADDR_t ipSubnet;
    L7_IP_MASK_t netMask;
    L7_uint32    vlanId;
	/* This field is needed by the AVL Library */
	void         *avlNext;
} vlanIpSubnetOperData_t;

typedef struct vlanIpSubnetCfg_s
{
    L7_fileHdr_t          cfgHdr;
    vlanIpSubnetCfgData_t vlanIpSubnetCfgData[L7_VLAN_IPSUBNET_MAX_VLANS];
    L7_uint32             checkSum;
} vlanIpSubnetCfg_t;



#endif /* VLAN_IPSUBNET_CFG_H */

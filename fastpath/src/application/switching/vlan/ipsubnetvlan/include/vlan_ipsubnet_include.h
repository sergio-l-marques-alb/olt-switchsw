/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_include.h
*
* @purpose   IP Subnet VLAN include header
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
#ifndef INCLUDE_IPSUBNET_VLAN_INCLUDE_H
#define INCLUDE_IPSUBNET_VLAN_INCLUDE_H

/*
 ***********************************************************************
 *                            COMMON INCLUDES
 ***********************************************************************
 */

#include "l7_common.h"
#include "osapi_support.h"
#include "dot1q_api.h"
#include "log.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "avl_api.h"

/*
 **********************************************************************
 *                            HEADER FILES
 **********************************************************************
 */
#include "vlan_ipsubnet_cfg.h"
#include "vlan_ipsubnet_cnfgr.h"
#include "vlan_ipsubnet_util.h"
#include "vlan_ipsubnet_migrate.h"

/*
 **********************************************************************
 *                            EXTERNAL DATA STRUCTURES
 **********************************************************************
 */
extern avlTree_t *vlanIpSubnetTreeData;
extern osapiRWLock_t vlanIpSubnetCfgRWLock;
extern vlanIpSubnetCfg_t *vlanIpSubnetCfg;
extern vlanIpSubnetCnfgrState_t vlanIpSubnetCnfgrState;

#endif/*INCLUDE_IPSUBNET_VLAN_INCLUDE_H*/


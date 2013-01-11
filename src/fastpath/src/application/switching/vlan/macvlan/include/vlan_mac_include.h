/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_include.h
*
* @purpose   MAC Vlan Include file
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

#ifndef INCLUDE_MAC_VLAN_INCLUDE_H
#define INCLUDE_MAC_VLAN_INCLUDE_H

/*
 ***********************************************************************
 *                            COMMON INCLUDES
 ***********************************************************************
 */

#include "l7_common.h"
#include "osapi.h"
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
#include "vlan_mac_cfg.h"
#include "vlan_mac_cnfgr.h"
#include "vlan_mac_util.h"
#include "vlan_mac_migrate.h"

/*
 **********************************************************************
 *                            EXTERNAL DATA STRUCTURES
 **********************************************************************
*/

extern osapiRWLock_t macVlanCfgRWLock;
extern vlanMacCfg_t *vlanMacCfg;
extern vlanMacCnfgrState_t vlanMacCnfgrState;


#endif /* VLAN_MAC_INCLUDE_H */

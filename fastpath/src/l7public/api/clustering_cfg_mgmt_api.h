/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2008
*
**********************************************************************
*
* @filename     cluster_cfg_mgmt_api.h
*
* @purpose      clustering cfg management utility functions
*
* @component    wireless (for now)
*
* @comments     This file will, at some point, disappear.  Today,
*               however, we need a away to allow the wireless 
*               component to notify the nascent clustering component
*               about new clustering config application.  But, we 
*               don't want to expose a slew of utility functions either.  
*               Weirder still, FastPath requires us to export this as 
*               an API, even though we really don't want other
*               components to use it.
*
*               Once clustering becomes a component in its own
*               right, the function prototypes below will be migrated 
*               to the appropriate util.h function, and no other
*               component will need to call them directly. 
*   
*               (If you have to ask whether or not you need to invoke
*                these functions yourself, the answer is: 
*                NO, YOU DON'T.)
*
* @create       3/24/2008
*
* @author       dcaugherty
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_CLUSTER_CFG_MGMT_API_H
#define INCLUDE_CLUSTER_CFG_MGMT_API_H

#include "l7_common.h"
#include "clustering_commdefs.h"


/*********************************************************************
*
* @purpose  Return a mask of clustering component configuration
*           that is known to have been applied
*
* @params   none
*
* @returns  L7_uint32 - the mask value
*
* @comments 
*
* @end
*
*********************************************************************/
L7_uint32 clusterConfigApplied(void);

/*********************************************************************
*
* @purpose  To denote that a received configuration has been applied
*
* @params   clusterCfgID_t @b{(input)} cfgID - of app in question
*
* @returns  nothing
*
* @comments 
*
* @end
*
*********************************************************************/

void      clusterConfigAppliedSet(clusterCfgID cfgID);

/*********************************************************************
*
* @purpose  To mark all configuration as unapplied 
*
* @params   none
*
* @returns  nothing
*
* @comments 
*
* @end
*
*********************************************************************/

void      clusterConfigAppliedClear(void);


#endif

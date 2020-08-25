/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cluster_notify_util.h
*
* @purpose      clustering notification utility function
*
* @component    wireless (for now)
*
* @comments     This file will, at some point, disappear.  Today,
*               however, we need a away to allow the wireless 
*               component to notify the nascent clustering component
*               about new clustering events.  But, we don't want to
*               expose a slew of utility functions either.  Weirder
*               still, FastPath requires us to export this as an API
*               function, even though we really don't want other
*               components to use it.
*
*               Once clustering becomes a component in its own
*               right, the function prototypes below will be migrated 
*               to the appropriate util.h function, and no other
*               component will need to call them directly. 
*   
*               (If you have to ask whether or not you need to invoke
*                these functions yourself, the answer is: 
*                No, You Don't.)
*
* @create       12/11/2007
*
* @author       dcaugherty
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_CLUSTER_NOTIFY_API_H
#define INCLUDE_CLUSTER_NOTIFY_API_H

#include "l7_common.h"
#include "clustering_commdefs.h"


/*********************************************************************
*
* @purpose  Notify observers about clustering events
*
* @params   clusterEvent evt @b{(input)} cluster event 
* @params   clusterMemberID_t id @b{(input)} responsbile member
*
* @returns  nothing
*
* @comments Notifies all "observer" callbacks about an event
*
* @end
*
*********************************************************************/
void clusterObserverNotify(clusterEvent evt, 
		           clusterMemberID_t * id);



/*********************************************************************
*
* @purpose  Get the cluster controller ID for this switch
*
* @params   clusterMemberID_t pID @b{(output)} found ID
*
* @returns  L7_TRUE if controller established within a cluster
* @returns  L7_FALSE otherwise
*
* @end
*
*********************************************************************/
L7_BOOL clusterControllerIDGet(clusterMemberID_t *);


#endif

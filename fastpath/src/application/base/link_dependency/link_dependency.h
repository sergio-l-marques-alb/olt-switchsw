/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2006-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename link_dependency.h
*
* @purpose application level APIs
*
* @component link dependency
*
* @comments none
*
* @create 12/06/2006
*
* @author dflint
* @end
*
**********************************************************************/

#ifndef LINK_DEPENDENCY_H
#define LINK_DEPENDENCY_H

#include "commdefs.h"
#include "nimapi.h"
#include "l7_cnfgr_api.h"

/*********************************************************************
* @purpose  This function returns the phase that linkDependency believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until linkDependency has completed phase 1, the value will be L7_CNFGR_STATE_NULL
*
* @end
*********************************************************************/
L7_CNFGR_STATE_t linkDependencyCnfgrPhaseGet(void);

/*********************************************************************
* @purpose  Handle text config apply events
*
* @param    The event type
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_uint32 linkDepTxtCfgApplyCompletionCallback(L7_uint32 event);

/*********************************************************************
* @purpose  Save the binary configuration
*
* @param    No parameters
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t linkDependencyCfgSave( void );

/*********************************************************************
* @purpose  Indicate whether the configuration has changed since the last save.
*
* @param    No parameters
*
* @returns  L7_TRUE                If the configuration is not saved.
* @returns  L7_FALSE               If the configuration is saved.
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL linkDependencyHasDataChanged( void );
void linkDependencyResetDataChanged( void );

/*********************************************************************
* @purpose  Updates the state of an interface.
*
* @param    intIfNum     @b{(input)}  the internal interface number
*
* @returns  None
*
* @comments Only updates the state in execute phase
*
* @end
*********************************************************************/
L7_RC_t linkDependencyIntfStateUpdate( L7_uint32 intIfNum );

/*********************************************************************
* @purpose  Updates the state of an group.
*
* @param    groupId      @b{(input)}  the group ID
*
* @returns  None
*
* @comments Only updates the state in execute phase
*
* @end
*********************************************************************/
L7_RC_t linkDependencyGroupStateUpdate( L7_uint32 groupId );

/*********************************************************************
* @purpose  Handle L7_UP/L7_DOWN events from NIM.
*
* @param    intIfNum     @b{(input)}  the internal interface number
* @param    event        @b{(input)}  the interface event
* @param    correlator   @b{(input)}  the correlator used by NIM for tallying
*
* @returns  L7_SUCCESS             Always, but ignored by NIM
*
* @comments This is where the main work of this component is performed.
*
* @notes    This function runs in NIM's thread. This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_uint32 linkDependencyIntfChangeCallBack( L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator );

#endif  /* LINK_DEPENDENCY_H */

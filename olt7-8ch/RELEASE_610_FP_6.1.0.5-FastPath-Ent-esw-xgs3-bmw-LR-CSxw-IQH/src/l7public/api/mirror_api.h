/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    mirror_api.h
* @purpose     Port mirroring API function prototype
* @component   mirroring
* @comments    none
* @create      11/21/2001
* @author      skalyanam
* @end
*             
**********************************************************************/

#ifndef MIRROR_API_H
#define MIRROR_API_H
#include "comm_mask.h"
#include "mirror_exports.h"

/*********************************************************************
* @purpose  Determine if the interface is attached
*
* @param  intIfNum - internal interface number
*
* @returns  L7_TRUE     interface is attached/attaching
* @returns  L7_FALSE    interface is not attached/attaching
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_BOOL mirrorIntfAttached(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the list of source ports configured for the specified 
*           session
*
* @param  sessionNum   @b{(input))  Session number
*                              Valid range: 1 - L7_MIRRORING_MAX_SESSIONS
* @param  *intIfMask   @b{(output)} Internal mask for source interfaces
*
* @returns  L7_SUCCESS,    result is available in *intIfMask
* @returns  L7_NOT_EXIST, session "sessionNum" does not exists
*
* @notes    It is valid for a port not to be configured in the 
*            source port list 
* @end
*********************************************************************/

extern L7_RC_t mirrorSourcePortsGet(L7_uint32 sessionNum,
                                    L7_INTF_MASK_t *intIfMask);


/*********************************************************************
* @purpose  Get the direction to be mirrored of a source port
*
* @param  intIfNum  (input) internal interface number
* @param  probeType (output) direction of traffic to be mirrored
*
* @returns  L7_SUCCESS     
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mirrorSourcePortDirectionGet(L7_uint32 sessionNum,L7_uint32 intIfNum, L7_MIRROR_DIRECTION_t *probeType);

/*********************************************************************
* @purpose  Add interface to the mirrored ports of session "sessionNum"
*
* @param  sessionNum @b{(input)) Session number 
*                          Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum   @b{(input)} Internal interface number to be added
*
* @returns  L7_SUCCESS        intIfNum added to the source interface list 
* @returns  L7_NOT_EXIST      session "sessionNum" does not exists
* @returns  L7_REQUEST_DENIED maximum limit of source ports reached
* @returns  L7_FAILURE        intIfNum is not valid 
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t mirrorSourcePortAdd(L7_uint32 sessionNum, L7_uint32 intIfNum,
                            L7_MIRROR_DIRECTION_t probeType);

/*********************************************************************
* @purpose  Remove interface from list of mirrored ports in session "sessionNum"
*
* @param  sessionNum @b{(input)) Session number
*                                Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum   @b{(input)} The internal interface number to remove
*
* @returns  L7_SUCCESS, intIfNum is removed from source list
* @returns  L7_NOT_EXIST, session does not exists
* @returns  L7_FAILURE, intIfNum is not memeber of source list of sessionNum
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t mirrorSourcePortRemove(L7_uint32 sessionNum,L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if an interface is an active probe port
*
* @param    intIfNum  @b((input)) Interface number
*
* @returns  L7_TRUE, intIfNum is configured as active probe port
* @returns  L7_FALSE, intIfNum is either not configured as probe port
*                       or it is not active
*
* @notes    none
*
* @end
*********************************************************************/

extern L7_BOOL mirrorIsActiveProbePort(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Returns Port Monitoring Destination Port for session "sessionNum"
*
* @param  sessionNum @b{(input)) Session number
*                                Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  *intIfNum  @b((output)) Interface number of destination port
*
* @returns  L7_SUCCESS, destination interface is available in *intIfNum
* @returns  L7_NOT_EXIST, session "sessionNum" does not exist
* @returns  L7_FAILURE, nim could not find AVL entry for destination ID
*                       of this session
*
* @notes    none
*
* @end
*********************************************************************/

extern L7_RC_t mirrorDestPortGet(L7_uint32 sessionNum,L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Sets Port Monitoring Destination Port for session "sessionNum"
*
* @param  sessionNum   @b{(input)) Session number
*                            Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum     @b{(input)} The internal interface number to set
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXIST, session "sessionNum" is not valid
* @returns  L7_FAILURE, intIfNum is not valid
* @returns  L7_ALREADY_CONFIGURED, destination port must be removed before
*                          re-configuring it
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t mirrorDestPortSet(L7_uint32 sessionNum,L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Remove Port Monitoring Destination Port for session "sessionNum"
*
* @param  sessionNum @b{(input))  Session number
*                             Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
*
* @returns  L7_SUCCESS, destination port is removed
* @returns  L7_NOT_EXIST, session "sessionNum" is not valid
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t mirrorDestPortRemove(L7_uint32 sessionNum);

/*********************************************************************
* @purpose  Returns the Port Monitoring Mode for session "sessionNum"
*
* @param    sessionNum   @b{(input))  Session number
*                      Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param    *mode       @b{(output)} mode of the session
*
* @returns  L7_SUCCESS, enable/disable status is available in *mode
* @returns  L7_NOT_EXIST, sessionNum does not exists
*
* @notes    none
*
* @end
*********************************************************************/

extern L7_RC_t mirrorModeGet(L7_uint32 sessionNum,L7_BOOL *mode);

/*********************************************************************
* @purpose  Sets the Unit's Port Monitoring Mode
*
* @param    sessionNum  @b{(input)) Session number
*                           Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param    mode        @b{(input)} Monitoring Mode (enable/disable)
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/

extern L7_RC_t mirrorModeSet(L7_uint32 sessionNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Returns count of Unit's Port Monitoring sessions
*
* @param   none
*
* @returns  Count of configured Port Monitoring sessions
*
* @notes  It returns maximum number of sessions that can be configured.
*
* @end
*********************************************************************/

extern L7_uint32 mirrorSessionCountGet();

/*********************************************************************
* @purpose  Removes port monitoring configuration
*
* @param    void
*
* @returns  L7_SUCCESS mirroring config data is removed successfully
*
* @notes    disables any current port monitoring setup, removes current
*           values for src and dest ports, replaces them with zeros,
*           the default configuration.
*
* @end
*********************************************************************/

extern L7_RC_t mirrorConfigRemove(L7_uint32 sessionNum);

/*********************************************************************
* @purpose  Removes port monitoring configuration for all sessions
*
* @param    void
*
* @returns  L7_SUCCESS, mirroring config data is removed successfully
*
* @notes    disables any current port monitoring setup, removes current 
*           values for src and dest ports, replaces them with zeros, 
*           the default configuration. 
*            
* @end
*********************************************************************/

extern L7_RC_t mirrorConfigRemoveAll();

/*********************************************************************
* @purpose  Removes port monitoring configuration for sessionNum
*
* @param    sessionNum   session number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST, session is not valid
*
* @notes    disables any current port monitoring setup, removes current values
*           for src and dest ports, replaces them with zeros,
*           the default configuration.
*
* @end
*********************************************************************/

extern L7_RC_t mirrorSessionConfig(L7_uint32 sessionNum);

/*********************************************************************
* @purpose  Check if intIfNum is configured as probe port in any session
*
* @param  intIfNum  @b{(input)}   The internal interface number being validated
* @param  *sessionNum @b{(output)} Session number in which this interface is
*                                    configured as probe port
*
* @returns  L7_TRUE   intIfNum is configured as probe port in some of the sessions
* @returns  L7_FALSE  intIfNum is not configured as probe port in any of the sessions
*
* @notes  sessionNum is valid only if return value is L7_TRUE
*
* @end
*********************************************************************/

extern L7_BOOL mirrorIsDestConfigured(L7_uint32 intIfNum, L7_uint32 *sessionNum);

/*********************************************************************
* @purpose  whether the interface is configured as source in any of the sessions
*
* @param    intIfNum  @b{(input)} The internal interface number being validated
* @param    *sessionNum  @b{(input)} Pointer to where session is stored
*
* @returns  L7_TRUE   intIfNum is configured as sourc port in some of the sessions
* @returns  L7_FALSE  intIfNum is not configured as source port in any of the sessions
*
* @notes   None
*
* @end
*********************************************************************/
extern L7_BOOL mirrorIsSrcConfigured(L7_uint32 intIfNum, L7_uint32 *sessionNum);

/*********************************************************************
* @purpose  check validity of interface for a sourceport 
*
* @param    intIfNum     @b{(input)} The internal interface number being validated
* 
* @returns  L7_TRUE   intIfNum can be configured as source port 
* @returns  L7_FALSE  intIfNum cannot be configured as source port 
*
* @notes   checks if the interface is a physical port or 
*          CPU port when feature is present 
*
* @end
*********************************************************************/
extern L7_BOOL mirrorValidSrcIntfCheck(L7_uint32 intfNum);

/*********************************************************************
* @purpose  check validity of interface for a destination port 
*
* @param    intIfNum     @b{(input)} The internal interface number being validated
* 
* @returns  L7_TRUE   intIfNum can be configured as source port in some of the sessions
* @returns  L7_FALSE  intIfNum cannot be configured as source port in any of the sessions
*
* @notes   checks if the interface is a physical port 
*
* @end
*********************************************************************/
extern L7_BOOL mirrorValidDestIntfCheck(L7_uint32 intfNum);

/*********************************************************************
* @purpose  get first valid interface that can be configured as source
*
* @param    intIfNum     @b{(output)} The internal interface number 
*                                     of first valid source interface
* 
* @returns  L7_SUCCESS  first valid source interface is found
* @returns  L7_FAILURE  no valid source interfaces can be found
*
* @notes    
* 
*
* @end
*********************************************************************/
extern L7_BOOL mirrorValidSrcIntfFirstGet(L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  get next valid interface that can be configured as source 
*
* @param     intIfNum     @b{(input)}  Internal Interface Number of 
*                                      a valid source port
* @param    nextIfNum @b{(output)} Internal Interface Number of 
*                                     next valid source port,
* 
* @returns  L7_SUCCESS  next valid source interface is found
* @returns  L7_FAILURE  no valid source interfaces can be found
*
* @notes  nextIfNum value is 0 when no more source interfaces can 
*        be found   
* 
*
* @end
*********************************************************************/
extern L7_BOOL mirrorValidSrcIntfNextGet(L7_uint32 intIfNum, L7_uint32 *nextIfNum);

/*********************************************************************
* @purpose  get first valid interface that can be configured as destination
*
* @param    intIfNum     @b{(output)} The internal interface number 
*                                     of first valid destination interface
* 
* @returns  L7_SUCCESS  first valid source interface is found
* @returns  L7_FAILURE  no valid source interfaces can be found
*
* @notes    
* 
*
* @end
*********************************************************************/
extern L7_BOOL mirrorValidDestIntfFirstGet(L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  get next valid interface that can be configured as destination
*
*@param     intIfNum     @b{(input)}  Internal Interface Number of 
*                                      a valid destination port
* @param    nextIfNum @b{(output)} Internal Interface Number of 
*                                     next valid destination port,
* 
* @returns  L7_SUCCESS  next valid destination interface is found
* @returns  L7_FAILURE  no valid destination interfaces can be found or
*                       any other error 
*
* @notes  nextIfNum value is 0 when no more destination interfaces can 
*        be found  
*
* @end
*********************************************************************/
extern L7_BOOL mirrorValidDestIntfNextGet(L7_uint32 intIfNum, L7_uint32 *nextIfNum);


#endif /*MIRROR_API_H*/

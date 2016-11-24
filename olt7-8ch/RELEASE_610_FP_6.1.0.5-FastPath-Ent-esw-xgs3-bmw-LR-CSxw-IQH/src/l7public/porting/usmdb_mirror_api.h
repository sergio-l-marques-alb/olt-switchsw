/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_mirror_api.h
*
* @purpose externs for USMDB layer
*
* @component unitmgr
*
* @comments none
*
* @create 11/21/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

#ifndef USMDB_MIRROR_API_H
#define USMDB_MIRROR_API_H

#include "l7_common.h"
#include "comm_mask.h"
#include "mirror_exports.h"

/*********************************************************************
*
* @purpose  Returns Source Ports for session "sessionNum"
*
* @param  UnitIndex    @b((input))  The unit for this operation
* @param  sessionNum   @b{(input))  Session number
*                              Valid range: 1 - L7_MIRRORING_MAX_SESSIONS
* @param  *intIfMask   @b{(output)} Internal mask for source interfaces
*
* @returns  L7_SUCCESS,    result is available in *intIfMask
* @returns  L7_NOT_EXIST, session "sessionNum" does not exists
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorSourcePortsGet(L7_uint32 UnitIndex, 
                      L7_uint32 sessionNum, L7_INTF_MASK_t *intIfNum);

/*********************************************************************
*
* @purpose  Add interface to the mirrored ports of session "sessionNum"
*
* @param  UnitIndex   @b((input))  The unit for this operation
* @param  sessionNum  @b{(input)) Session number
*                             Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum    @b{(input)} The internal interface number to be added
* @param  probeType   @b{(input)} direction of traffic to be mirrored
*
* @returns  L7_SUCCESS, intIfNum added to the source interface list
* @returns  L7_NOT_EXIST, session "sessionNum" does not exists
* @returns  L7_FAILURE,  intIfNum is not valid
*
* @notes none
*
* @end
*********************************************************************/
 extern L7_RC_t usmDbSwPortMonitorSourcePortAdd(L7_uint32 UnitIndex, 
                                               L7_uint32 sessionNum,
                                               L7_uint32 intIfNum, 
                                               L7_MIRROR_DIRECTION_t probeType);

/*********************************************************************
*
* @purpose  Remove interface from mirrored ports of session "sessionNum"
*
* @param  UnitIndex   @b((input))  The unit for this operation
* @param  sessionNum  @b{(input)) Session number
*                             Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum    @b{(input)} The internal interface number to be
*                                 removed
*
* @returns  L7_SUCCESS, intIfNum removed from source interface list
* @returns  L7_NOT_EXIST, session "sessionNum" does not exists
* @returns  L7_FAILURE, intIfNum is not in source list of sessionNum
*
* @notes none
*
* @end
*********************************************************************/

extern L7_RC_t usmDbSwPortMonitorSourcePortRemove(L7_uint32 UnitIndex, 
                          L7_uint32 sessionNum, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Returns Destination Port for session "sessionNum"
*
* @param UnitIndex @b((input))  L7_uint32 the unit for this operation
* @param sessionNum @b((input)) session number
*                                 Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param intIfNum  @b((output)) The internal destination interface number
*
* @returns  L7_SUCCESS, destination interface is available in *intIfNum
* @returns  L7_NOT_EXIST, session "sessionNum" does not exist
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorDestPortGet(L7_uint32 UnitIndex, 
                                L7_uint32 sessionNum, L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose  Sets Destination Port for session "sessionNum"
*
* @param  UnitIndex    @b((input))  The unit for this operation
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
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorDestPortSet(L7_uint32 UnitIndex, 
                    L7_uint32 sessionNum, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Remove Destination Port for session "sessionNum"
*
* @param  UnitIndex    @b((input))  The unit for this operation
* @param  sessionNum   @b{(input)) Session number
*                            Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
*
* @returns  L7_SUCCESS, destination port is cleared
* @returns  L7_NOT_EXIST, session "sessionNum" is not valid
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorDestPortRemove(L7_uint32 UnitIndex,
                              L7_uint32 sessionNum);


/*********************************************************************
*
* @purpose  Returns the Port Monitoring Mode for session "sessionNum"
*
* @param    UnitIndex    @b((input))  The unit for this operation
* @param    sessionNum   @b{(input))  Session number
*                      Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param    *mode        @b((output)) mode of the session
*
* @returns  L7_SUCCESS, enable/disable status is available in *mode
* @returns  L7_NOT_EXIST, sessionNum does not exists
*
* @notes Valid Modes returned:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorModeGet(L7_uint32 UnitIndex, L7_uint32 sessionNum,
                            L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Sets the Port Monitoring Mode for session "sessionNum"
*
* @param    UnitIndex   @b((input)) The unit for this operation
* @param    sessionNum  @b{(input)) Session number
*                           Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param    mode        @b{(input)} Monitoring Mode (enable/disable)
*
* @returns L7_SUCCESS, mirroring mode for session is set successfully
* @returns L7_NOT_EXIST, sessionNum does not exists
*
* @notes Valid Modes:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorModeSet(L7_uint32 UnitIndex,L7_uint32 sessionNum,
                 L7_uint32 mode);

/*********************************************************************
*
* @purpose  Returns count of Port Monitoring sessions
*
* @param UnitIndex @b((input))  The unit for this operation
*
* @returns  Count of the sessions
*
* @notes It returns maximum number of sessions that can be configured.
*
* @end
*********************************************************************/
extern L7_uint32 usmDbSwPortMonitorSessionCount(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose  Removes port monitoring configuration for all sessions
*
* @param    UnitIndex @b((input))  The unit for this operation
*
* @returns  L7_SUCCESS, mirroring config data is removed successfully
* @returns  L7_FAILURE, mirroring data could not be removed
*
* @notes    disables any current port monitoring setup, removes current
*           values for src and dest ports, replaces them with zeros,
*           the default configuration.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorConfigRemove(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose  Removes port monitoring configuration for sessionNum
*
* @param    UnitIndex   @b((input))  The unit for this operation
* @param    sessionNum  @b((input)) session number
*
* @returns  L7_SUCCESS, session removed successfully
* @returns  L7_FAILURE, session data could not be removed
* @returns  L7_NOT_EXIST, session is not valid
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorSessionRemove(L7_uint32 UnitIndex, 
                             L7_uint32 sessionNum);

/*********************************************************************
*
* @purpose  Determine if intIfNum is configured as destination Port
*
* @param  UnitIndex    @b((input))  The unit for this operation
* @param  intIfNum     @b{(input)} The internal interface number
* @param *sessionNum   @b{(output)) Session number
*                            Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
*
* @returns  L7_TRUE, intIfNum is configured as destination port for
*                    *sessionNum
* @returns L7_FALSE, intIfNum is not configured as destination port
*                    for any of the sessions
*
* @notes
*
* @end
*********************************************************************/
extern L7_BOOL usmDbSwPortMonitorIsDestConfigured(L7_uint32 UnitIndex,
                        L7_uint32 intIfNum, L7_uint32 *sessionNum);

/*********************************************************************
*
* @purpose  Determine if intIfNum is configured as source Port
*
* @param  UnitIndex    @b((input))  The unit for this operation
* @param  intIfNum     @b{(input)} The internal interface number
*
* @returns  L7_TRUE, intIfNum is configured as source port
* @returns L7_FALSE, intIfNum is not configured as source port
*
* @notes
*
* @end
*********************************************************************/
extern L7_BOOL usmDbSwPortMonitorIsSrcConfigured(L7_uint32 UnitIndex,
                                   L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the direction to be mirrored for a source port
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
L7_RC_t usmDbMirrorSourcePortDirectionGet(L7_uint32 sessionNum,L7_uint32 intIfNum, L7_MIRROR_DIRECTION_t *probeType);

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
extern L7_BOOL usmDbSwPortMonitorIsValidSrcPort(L7_uint32 intIfNum);

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
extern L7_BOOL usmDbSwPortMonitorIsValidDestPort(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  get first valid interface that can be configured as source
*
* @param    interface     @b{(output)} The internal interface number 
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
extern L7_RC_t usmDbSwPortMonitorValidSrcIntfFirstGet(L7_uint32 *interface);

/*********************************************************************
* @purpose  get next valid interface that can be configured as source 
*
* @param    interface     @b{(input)}  Internal Interface Number of 
*                                      a valid source port
* @param    nextInterface @b{(output)} Internal Interface Number of 
*                                     next valid source port,
* 
* @returns  L7_SUCCESS  next valid source interface is found
* @returns  L7_FAILURE  no more valid source interface can be found or
*                       any other error 
*
* @notes nextInterface  value is 0 when no more destination interfaces can 
*        be found 
* 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorValidSrcIntfNextGet(L7_uint32 interface,
                                                     L7_uint32 *nextInterface);

/*********************************************************************
* @purpose  get first valid interface that can be configured as destination
*
* @param    interace     @b{(output)} The internal interface number 
*                                     of first valid destination interface
* 
* @returns  L7_SUCCESS  first valid source interface is found
* @returns  L7_FAILURE  no more valid source interfaces can be found
*
* @notes    
* 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorValidDestIntfFirstGet(L7_uint32 *interface);

/*********************************************************************
* @purpose  get next valid interface that can be configured as destination
*
* @param     interface     @b{(input)}  Internal Interface Number of 
*                                      a valid destination port
* @param    nextInterface @b{(output)} Internal Interface Number of 
*                                     next valid destination port,
* 
* @returns  L7_SUCCESS  next valid destination interface is found
* @returns  L7_FAILURE  no more valid destination interfaces can be found or
*                       any other error
*
* @notes nextInterface  value is 0 when no more destination interfaces can 
*        be found 
* 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortMonitorValidDestIntfNextGet(L7_uint32 interface, 
                                                      L7_uint32 *nextInterface);
#endif /* USMDB_MIRROR_API_H*/

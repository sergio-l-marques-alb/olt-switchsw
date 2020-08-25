/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  usmdb_dot1s_api.h
*
* @purpose   USMDB API's for IEEE 802.1s Multiple Spanning Tree (MSTP)
*
* @component dot1s
*
* @component unitmgr
*
* @create    09/16/2002
*
* @author    spetriccione
*
* @end
**********************************************************************/   

#ifndef USMDB_DOT1S_API_H
#define USMDB_DOT1S_API_H
#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_dot1d_api.h"
#include "dot1s_exports.h"

/*********************************************************************
* @purpose  Set the MSTP Operational Mode 
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(input)} the new value of the MSTP mode  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
**********************************************************************/
extern L7_RC_t usmDbDot1sModeSet(L7_uint32 unitIndex, 
                          L7_BOOL val); 

/*********************************************************************
* @purpose  Get the MSTP Operational Mode 
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP mode  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sModeGet(L7_uint32 unitIndex, 
                          L7_BOOL *val); 

/*********************************************************************
* @purpose  Set the MSTP Force Protocol Version parameter	
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(input)} the new value of the MSTP parameter  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sForceVersionSet(L7_uint32 unitIndex, 
                                  DOT1S_FORCE_VERSION_t val); 

/*********************************************************************
* @purpose  Get the value of the MSTP Force Protocol Version parameter	
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter  
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sForceVersionGet(L7_uint32 unitIndex, 
                                  DOT1S_FORCE_VERSION_t *val); 

/*********************************************************************
* @purpose  Set the MSTP Configuration Format Selector 
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sConfigFormatSelectorSet(L7_uint32 unitIndex, 
                                          L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Configuration Format Selector 
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sConfigFormatSelectorGet(L7_uint32 unitIndex, 
                                          L7_uint32 *val); 

/*********************************************************************
* @purpose  Set the MSTP Configuration Name
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    buf       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sConfigNameSet(L7_uint32 unitIndex, 
                                L7_uchar8 *buf); 

/*********************************************************************
* @purpose  Get the MSTP Configuration Name
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    buf       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sConfigNameGet(L7_uint32 unitIndex, 
                                L7_uchar8 *buf); 

/*********************************************************************
* @purpose  Get the MSTP Configuration Digest Key
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    buf       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sConfigDigestKeyGet(L7_uint32 unitIndex, 
                                     L7_uchar8 *buf); 

/*********************************************************************
* @purpose  Set the MSTP Configuration Revision
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sConfigRevisionSet(L7_uint32 unitIndex, 
                                    L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Configuration Revision
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sConfigRevisionGet(L7_uint32 unitIndex, 
                                    L7_uint32 *val); 

/*********************************************************************
* @purpose  Set the MSTP Bridge Max Age parameter for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistBridgeMaxAgeSet(L7_uint32 unitIndex, 
                                      L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Bridge Max Age parameter for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistBridgeMaxAgeGet(L7_uint32 unitIndex, 
                                      L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Root Port Max Age parameter for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistMaxAgeGet(L7_uint32 unitIndex, 
                                L7_uint32 *val); 

/*********************************************************************
* @purpose  Set the MSTP Bridge HelloTime parameter for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistBridgeHelloTimeSet(L7_uint32 unitIndex, 
                                         L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Bridge HelloTime parameter for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistBridgeHelloTimeGet(L7_uint32 unitIndex, 
                                         L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Root Bridge HelloTime parameter
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments This value will override any local settings if different
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistHelloTimeGet(L7_uint32 unitIndex, 
                                   L7_uint32 *val); 

/*********************************************************************
* @purpose  Set the MSTP Bridge Forward Delay for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistBridgeFwdDelaySet(L7_uint32 unitIndex, 
                                        L7_uint32 val);

/*********************************************************************
* @purpose  Set the MSTP Bridge parameters(ForwardDelay ,Hello time
            and Max age for the CIST
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    fwdDelay,maxAge,helloTime  @b{(input)} the new paramters
                                               of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistBridgeParametersSet(L7_uint32 unitIndex,
                                         L7_uint32 fwdDelay,
                                         L7_uint32 maxAge,
                                         L7_uint32 helloTime);

/*********************************************************************
* @purpose  Get the MSTP Bridge Forward Delay for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistBridgeFwdDelayGet(L7_uint32 unitIndex, 
                                        L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Root Port Forward Delay for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistRootFwdDelayGet(L7_uint32 unitIndex, 
                                      L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Bridge Hold Time for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistBridgeHoldTimeGet(L7_uint32 unitIndex, 
                                        L7_uint32 *val); 

/*********************************************************************
* @purpose  Set the MSTP Bridge Priority parameter in the specific 
*           instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    val       @b{(output)} actual value used for the MSTP parameter 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiBridgePrioritySet(L7_uint32 unitIndex, 
                                        L7_uint32 mstID, 
                                        L7_uint32 *val);

/*********************************************************************
* @purpose  Get the MSTP Bridge Priority parameter in the specific
*           instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiBridgePriorityGet(L7_uint32 unitIndex, 
                                        L7_uint32 mstID, 
                                        L7_ushort16 *val); 

/*********************************************************************
* @purpose  Set the MSTP Administrative Edge Port parameter of a 
*           specific port for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortEdgeSet(L7_uint32 unitIndex, 
                                  L7_uint32 intIfNum, 
                                  L7_BOOL val); 

/*********************************************************************
* @purpose  Get the MSTP Administrative Edge Port parameter of a 
*           specific port for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortEdgeGet(L7_uint32 unitIndex, 
                                  L7_uint32 intIfNum, 
                                  L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Operational Edge Port parameter of a 
*           specific port for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortOperEdgeGet(L7_uint32 unitIndex, 
                                      L7_uint32 intIfNum, 
                                      L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Operational Point to Point Mac parameter 
*           of a specific port for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortOperPointToPointGet(L7_uint32 unitIndex, 
                                              L7_uint32 intIfNum, 
                                              L7_BOOL *val); 

/*********************************************************************
* @purpose  Force the specified port to transmit RSTP or MSTP BPDU's
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortForceMigrationCheck(L7_uint32 unitIndex, 
                                          L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the MSTP Administrative Bridge Port State
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments This is called the Port Administrative Mode in the dot1s
*           code.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStateSet(L7_uint32 unitIndex, 
                               L7_uint32 intIfNum, 
                               L7_BOOL val); 

/*********************************************************************
* @purpose  Get the MSTP Administrative Bridge Port State
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments This is called the Port Administrative Mode in the dot1s
*           code.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStateGet(L7_uint32 unitIndex, 
                               L7_uint32 intIfNum, 
                               L7_BOOL *val); 

/*********************************************************************
* @purpwse  Get the MSTP Forwarding State of the port in a specific
*           instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current forwarding state
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortForwardingStateGet(L7_uint32 unitIndex, 
                                             L7_uint32 mstID, 
                                             L7_uint32 intIfNum, 
                                             L7_uint32 *val); 

/*********************************************************************
* @purpwse  Get the MSTP Port Role in a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current port role
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortRoleGet(L7_uint32 unitIndex, 
                                  L7_uint32 mstID, 
                                  L7_uint32 intIfNum, 
                                  L7_uint32 *val); 

/*********************************************************************
* @purpwse  Get the MSTP Up Time of the port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the uptime
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortUpTimeGet(L7_uint32 unitIndex, 
                                L7_uint32 intIfNum, 
                                usmDbTimeSpec_t *val); 

/*********************************************************************
* @purpwse  Get the MTSI associated with the specified VID
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    VID       @b{(input)} the desired VLAN ID
* @param    val       @b{(output)} the MSTID of the associated instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sVlanToMstiGet(L7_uint32 unitIndex, 
                                L7_uint32 VID, 
                                L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the configured MSTI - VID association
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    VID       @b{(input)} the desired VLAN ID
* @param    val       @b{(output)} the MSTID of the associated instance
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sVlanToMstiConfiguredGet(L7_uint32 unitIndex,
                                                 L7_uint32 VID, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the MSTP Port Path Cost in a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortPathCostSet(L7_uint32 unitIndex, 
                                      L7_uint32 mstID, 
                                      L7_uint32 intIfNum, 
                                      L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Port Path Cost in a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortPathCostGet(L7_uint32 unitIndex, 
                                      L7_uint32 mstID, 
                                      L7_uint32 intIfNum, 
                                      L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Port Path Cost Mode in a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortPathCostModeGet(L7_uint32 unitIndex, 
                                          L7_uint32 mstID, 
                                          L7_uint32 intIfNum, 
                                          L7_BOOL *val); 

/*********************************************************************
* @purpose  Set the MSTP Port Priority in a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} actual value used for the MSTP parameter 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortPrioritySet(L7_uint32 unitIndex, 
                                      L7_uint32 mstID, 
                                      L7_uint32 intIfNum, 
                                      L7_uint32 *val);

/*********************************************************************
* @purpose  Get the MSTP Port Priority in a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortPriorityGet(L7_uint32 unitIndex, 
                                      L7_uint32 mstID, 
                                      L7_uint32 intIfNum, 
                                      L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the default MSTP port priority for the port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the default MSTP port priority
*
* @comments 
*
*
* @end
*********************************************************************/
extern L7_uint32 usmDbDot1sDefaultPortPriorityGet(L7_uint32 unitIndex,
                                                  L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Port Identifier of a specific port in a
*           specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the port ID
*
* @returns  L7_SUCCESS  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortIDGet(L7_uint32 unitIndex, 
                                L7_uint32 mstID, 
                                L7_uint32 intIfNum, 
                                L7_uint32 *val); 

/*********************************************************************
* @purpose  Set the MSTP Administrative Auto Edge Port parameter of a
*           specific port. 
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortAutoEdgeSet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_BOOL val);

/*********************************************************************
* @purpose  Get the MSTP Administrative Auto Edge Port parameter of a
*           specific port.
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortAutoEdgeGet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_uint32 *val);

/*********************************************************************
* @purpose  Set the MSTP Administrative Restricted Role parameter of a
*           specific port.
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortRestrictedRoleSet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_BOOL val);

/*********************************************************************
* @purpose  Get the MSTP Administrative Restricted Role parameter of a
*           specific port
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortRestrictedRoleGet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_uint32 *val);

/*********************************************************************
* @purpose  Get the MSTP Administrative LoopGuard parameter of a specific port
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortLoopGuardGet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_uint32 *val);

/*********************************************************************
* @purpose  Set the MSTP Administrative LoopGuard parameter of a
*           specific port.
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortLoopGuardSet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_BOOL val);

/*********************************************************************
* @purpose  Get the MSTP Loop Inconsistent State of the port in a specific
*           instance
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MSTI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current loop inconsistent state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortLoopInconsistentStateGet(L7_uint32 unitIndex,
                                                          L7_uint32 mstID,
                                                          L7_uint32 intIfNum,
                                                          L7_uint32 *val);

/*********************************************************************
* @purpose  Set the MSTP Administrative Restricted Tcn parameter of a
*           specific port
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortRestrictedTcnSet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_BOOL val);

/*********************************************************************
* @purpose  Get the MSTP Administrative Restricted Tcn parameter of a
*           specific port
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortRestrictedTcnGet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_uint32 *val);

/*********************************************************************
* @purpose  Create a MSTP instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    @b{(output)}  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments total created should not exceed L7_MAX_MULTIPLE_STP_INSTANCES
*           0 is not allowed as an mstID
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiCreate(L7_uint32 unitIndex, 
                             L7_uint32 mstID); 

/*********************************************************************
* @purpose  Delete a MSTP instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    @b{(output)}  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 0 is not allowed as an mstID
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiDelete(L7_uint32 unitIndex, 
                             L7_uint32 mstID); 


/*********************************************************************
* @purpose  Add an association between a VLAN and an MSTP instance
*
* @param    unitIndex  @b{(input)} the unit for this operation
* @param    mstID      @b{(input)} the MSTID for the desired MTSI
* @param    vlan_mask  @b{(input)} the set of vlans to be mapped 
*                                  to the mst instance
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The VLAN ID(s) will no longer be associated with the CIST.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1sMstiVlanAdd(L7_uint32 unitIndex,
                              L7_uint32 mstID,
                              L7_VLAN_MASK_t *vlan_mask);

/*********************************************************************
* @purpose  Remove an association between a VLAN and an MSTP instance
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MSTI
* @param    vlan_mask  @b{(input)} the set of vlans to be mapped 
*                                  to the mst instance
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The VLAN ID(s) will again be associated with the CIST.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1sMstiVlanRemove(L7_uint32 unitIndex,
                                 L7_uint32 mstID,
                                 L7_VLAN_MASK_t *vlan_mask);
                                 
/*********************************************************************
* @purpose  Get or Check if the MSTP instance ID exists
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID in question
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an L7_FAILURE return indicates the MSTID not found 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sInstanceGet(L7_uint32 unitIndex, 
                              L7_uint32 mstID);

/*********************************************************************
* @purpose  Get the first of the created MSTP instance IDs
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(output)} the first of the MSTIDs
*
* @returns  L7_SUCCESS  
*
* @comments The first MSTP instance will always be the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sInstanceFirstGet(L7_uint32 unitIndex, 
                                   L7_uint32 *mstID);

/*********************************************************************
* @purpose  Get the next of the created MSTP instance IDs
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    prevMstID @b{(output)} the previous of the MSTIDs
* @param    mstID     @b{(output)} the next of the MSTIDs
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an L7_FAILURE return indicates no next ID found
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sInstanceNextGet(L7_uint32 unitIndex, 
                                  L7_uint32 prevMstID, 
                                  L7_uint32 *mstID);

/*********************************************************************
* @purpose  List all of the created MSTP instance IDs
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    buf @b{(output)} pointer to the current list of MSTIDs
* @param    val       @b{(input)} maximum number of MSTIDs to put in the list
* @param    val       @b{(output)} actual count of MSTIDs in the list
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sInstanceList(L7_uint32 unitIndex, 
                               L7_uint32 *buf, 
                               L7_uint32 *val);

/*********************************************************************
* @purpose  Get the first of the Forwarding Database ID's associated 
*           with the specified MSTP instance ID
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTP instance in question
* @param    FID       @b{(output)} the first FID or L7_NULL if none found
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiFIDFirstGet(L7_uint32 unitIndex, 
                                  L7_uint32 mstID, 
                                  L7_uint32 *FID);

/*********************************************************************
* @purpose  Get the next of the Forwarding Database ID's associated 
*           with the specified MSTP instance ID
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTP instance in question
* @param    prevFID   @b{(input)} previous FID
* @param    FID       @b{(output)} the next FID or L7_NULL if none found
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if next FID not found
*
* @comments VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiFIDNextGet(L7_uint32 unitIndex, 
                                 L7_uint32 mstID, 
                                 L7_uint32 prevFID, 
                                 L7_uint32 *FID);

/*********************************************************************
* @purpose  List all of the Forwarding Database ID's associated with 
*           an MSTP instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MSTI
* @param    buf       @b{(output)} pointer to the current list of FIDs
* @param    val       @b{(output)} actual count of FIDs in the list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*           VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiFIDList(L7_uint32 unitIndex, 
                              L7_uint32 mstID, 
                              L7_uint32 *buf, 
                              L7_uint32 *val);

/*********************************************************************
* @purpose  Get the first of the VLAN ID's associated 
*           with the specified MSTP instance ID
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTP instance in question
* @param    VID       @b{(output)} the first VID or L7_NULL if none found
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if first VID not found
*
* @comments an MSTID of zero will represent the CIST
*           VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiVIDFirstGet(L7_uint32 unitIndex, 
                                  L7_uint32 mstID, 
                                  L7_uint32 *VID);

/*********************************************************************
* @purpose  Get the next of the VLAN ID's associated with the 
*           specified MSTP instance ID
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTP instance in question
* @param    prevVID   @b{(input)} previous VID
* @param    VID       @b{(output)} the next VID or L7_NULL if none found
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if next VID not found
*
* @comments VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiVIDNextGet(L7_uint32 unitIndex, 
                                 L7_uint32 mstID, 
                                 L7_uint32 prevVID, 
                                 L7_uint32 *VID);

/*********************************************************************
* @purpose  Check if the MSTP instance and VLAN ID association exists
*
* @param    unitIndex @b{(input)} unit for this operation
* @param    mstID     @b{(input)} MSTP instance ID
* @param    VID       @b{(input)} Vlan ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sInstanceVlanGet(L7_uint32 unitIndex,
                                         L7_uint32 mstID, L7_uint32 VID);

/*********************************************************************
* @purpose  Get the next MSTP instance and VLAN ID 
*           given an MSTP instance and VLAN ID as reference.
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    prevMstID @b{(input)} the previous MSTP instance ID
* @param    mstID     @b{(output)} the next MSTP instance ID
* @param    prevVID @b{(input)} the previous VID
* @param    VID @b{(output)} the next VID or L7_NULL if none found
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function returns the next 'mstID-VID' pair using the 
*           specified 'prevMstID-prevVID' pair as reference.
*           An input of zero (0) for both 'prevMstID' and 'prevVID' 
*           returns the first VLAN ID that is associated with an MSTP 
*           instance.
*           When multiple VLANs are associated with an MSTP instance, 
*           the next 'mstID' is same as 'prevMstID'.
*
* @comments VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sInstanceVlanNextGet(L7_uint32 unitIndex, 
                                      L7_uint32 prevMstID, 
                                      L7_uint32 *mstID, 
                                      L7_uint32 prevVID, 
                                      L7_uint32 *VID);

/*********************************************************************
* @purpose  Get or Check if the MSTP instance ID has the specificed VID
*           associated with it.
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID in question
* @param    VID       @b{(input)} the VLAN ID in question
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments a L7_FAILURE return indicates the assocation of MSTI to
*           VID does not exist.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiVIDGet(L7_uint32 unitIndex, 
                             L7_uint32 mstID,
                             L7_uint32 VID);

/*********************************************************************
* @purpose  List all of the VLAN ID's associated with an MSTP instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    buf       @b{(output)} pointer to the current list of FIDs
* @param    val       @b{(output)} actual count of FIDs in the list
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiVIDList(L7_uint32 unitIndex, 
                              L7_uint32 mstID, 
                              L7_uint32 *buf, 
                              L7_uint32 *val);

/*********************************************************************
* @purpose  Get the MSTP Bridge Identifier in a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    val       @b{(output)} Bridge Identifier
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiBridgeIdentifierGet(L7_uint32 unitIndex, 
                                          L7_uint32 mstID, 
                                          L7_uchar8 * buf, 
                                          L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP CIST Regional Root Identifier
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    buf @b{(output)} Bridge Identifier
* @param    len @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
********************************************************************/
extern L7_RC_t usmDbDot1sCistRegionalRootIDGet(L7_uint32 unitIndex, 
                                        L7_uchar8 *buf, 
                                        L7_uint32 *len);

/*********************************************************************
* @purpose  Get the MSTP Regional Root Bridge Identifier in a specific 
*           instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    buf       @b{(output)} Bridge Identifier
* @param    len       @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE    
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiRegionalRootIDGet(L7_uint32 unitIndex, 
                                        L7_uint32 mstID,
                                        L7_uchar8 *buf, 
                                        L7_uint32 *len);

/*********************************************************************
* @purpose  Get the MSTP CIST Regional Root Path Cost
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} current value of the Path Cost
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistRegionalRootPathCostGet(L7_uint32 unitIndex, 
                                              L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the MSTP Regional Root Path Cost for a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    val       @b{(output)} current value of the Path Cost
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiRegionalRootPathCostGet(L7_uint32 unitIndex, 
                                              L7_uint32 mstID,
                                              L7_uint32 *val);

/*********************************************************************
* @purpose  Get the MSTP Time since the last Topology Change in a
*           specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    ts        @b{(output)} time since last TCN
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiTimeSinceTopologyChangeGet(L7_uint32 unitIndex, 
                                                 L7_uint32 mstID, 
                                                 usmDbTimeSpec_t *ts); 

/*********************************************************************
* @purpose  Get the MSTP count of Topology Changes in a specific 
*           instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    val       @b{(output)} number of TCNs received
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiTopologyChangeCountGet(L7_uint32 unitIndex, 
                                             L7_uint32 mstID, 
                                             L7_uint32 *countTCN); 

/*********************************************************************
* @purpose  Get the MSTP Topology Change parameter in a specific
*           instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    val       @b{(output)} the current value of the TCN parm
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiTopologyChangeParmGet(L7_uint32 unitIndex, 
                                            L7_uint32 mstID, 
                                            L7_BOOL *val); 

/*********************************************************************
* @purpose  Get the MSTP Topology Change Acknowledge parameter for a
*           specific port in the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP parm
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortTopologyChangeAckGet(L7_uint32 unitIndex, 
                                               L7_uint32 intIfNum, 
                                               L7_BOOL *val); 

/*********************************************************************
* @purpose  Get the MSTP Root Port ID in a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    val       @b{(output)} Port ID of the Root Port
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiRootPortIDGet(L7_uint32 unitIndex, 
                                    L7_uint32 mstID, 
                                    L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Designated Root Identifier in a specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    buf       @b{(output)} Bridge Identifier
* @param    len       @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @comments According to the IEEE 802.1s-2002 spec., the "Designated Root"
*           Identifies the CIST Root Bridge for the CIST and the
*           MSTI Regional Root Bridge for an instance. Therefore this
*           function is somewhat redundant to the function 
*           usmDbDot1sMstiRegionalRootIDGet for an instance.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiDesignatedRootIDGet(L7_uint32 unitIndex, 
                                          L7_uint32 mstID, 
                                          L7_uchar8 *buf, 
                                          L7_uint32 *len);

/*********************************************************************
* @purpose  Get the MSTP Designated Root Path Cost parameter in a specific
*           instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    val       @b{(output)} current value of the Path Cost
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @comments According to the IEEE 802.1s-2002 spec., the "Designated Root"
*           Identifies the CIST Root Bridge for the CIST and the
*           MSTI Regional Root Bridge for an instance. Therefore this
*           function is somewhat redundant to the function 
*           usmDbDot1sMstiRegionalRootPathCostGet for an instance.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiRootPathCostGet(L7_uint32 unitIndex, 
                                      L7_uint32 mstID, 
                                      L7_uint32 *val);

/*********************************************************************
* @purpose  Get the MSTP Designated Cost of a specific port in a 
*           specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} current value of the Path Cost
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiDesignatedCostGet(L7_uint32 unitIndex, 
                                        L7_uint32 mstID, 
                                        L7_uint32 intIfNum, 
                                        L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Designated Bridge ID of a specific port in a 
*           specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    buf       @b{(output)} Bridge Identifier
* @param    len       @b{(output)} length of the Bridge Identifer
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiDesignatedBridgeIDGet(L7_uint32 unitIndex, 
                                            L7_uint32 mstID, 
                                            L7_uint32 intIfNum, 
                                            L7_uchar8 *buf, 
                                            L7_uint32 *len);

/*********************************************************************
* @purpose  Get the MSTP Designated Port ID of a specific port in a 
*           specific instance
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the value of the port ID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiDesignatedPortIDGet(L7_uint32 unitIndex, 
                                          L7_uint32 mstID, 
                                          L7_uint32 intIfNum, 
                                          L7_ushort16 *portID); 

/*********************************************************************
* @purpose  Get the next port participating in MSTP associated with 
*           the specified MSTP instance ID.
*          
* @param    unitIndex    @b{(input)} the unit for this operation
* @param    mstID        @b{(input)} the MSTID of the desired MSTI
* @param    prevIntIfNum @b{(input)} the previous interface number
* @param    intIfNum     @b{(output)} the next interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments a L7_FAILURE return indicates the next MSTID/port combination 
*           is not found and intIfNum is returned as L7_NULL
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortNextGet(L7_uint32 unitIndex, 
                              L7_uint32 mstID,
                              L7_uint32 prevIntIfNum,
                              L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get the next port participating in MSTP associated with 
*           the specified MSTP instance ID.
*          
* @param    unitIndex    @b{(input)} the unit for this operation
* @param    prevMstID    @b{(input)} the previous MSTID
* @param    mstID        @b{(output)} the next MSTID
* @param    prevIntIfNum @b{(input)} the previous interface number
* @param    intIfNum     @b{(output)} the next interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments L7_FAILURE return indicates the next MSTID/port combination 
*           is not found. The intIfNum is returned as L7_NULL and
*           the mstID is returned as L7_DOT1S_MSTID_ALL.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortNextGet(L7_uint32 unitIndex, 
                                  L7_uint32 prevMstID,
                                  L7_uint32 *mstID,
                                  L7_uint32 prevIntIfNum,
                                  L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get or Check if the port is usable by MSTP and is 
*           associated with the specified MSTP instance ID.
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum  @b{(input)} the interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments a L7_FAILURE return indicates the MSTID/port combination 
*           is not found to be participating in MSTP.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortGet(L7_uint32 unitIndex, 
                              L7_uint32 mstID,
                              L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the MSTP Statistics for STP BPDU's Received on a 
*           specific port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP stat  
*
* @returns  L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStatsSTPBPDUsReceivedGet(L7_uint32 unitIndex, 
                                               L7_uint32 intIfNum, 
                                               L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for RSTP BPDU's Received on a 
*           specific port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP stat  
*
* @returns  L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStatsRSTPBPDUsReceivedGet(L7_uint32 unitIndex, 
                                                L7_uint32 intIfNum, 
                                                L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for MSTP BPDU's Received on a 
*           specific port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP stat  
*
* @returns  L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStatsMSTPBPDUsReceivedGet(L7_uint32 unitIndex, 
                                                L7_uint32 intIfNum, 
                                                L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for STP BPDU's Sent on a specific
*           port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP stat  
*
* @returns  L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStatsSTPBPDUsSentGet(L7_uint32 unitIndex, 
                                           L7_uint32 intIfNum, 
                                           L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for RSTP BPDU's Sent on a specific
*           port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP stat  
*
* @returns  L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStatsRSTPBPDUsSentGet(L7_uint32 unitIndex, 
                                            L7_uint32 intIfNum, 
                                            L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for MSTP BPDU's Sent on a specific
*           port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} the current value of the MSTP stat  
*
* @returns  L7_SUCCESS  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStatsMSTPBPDUsSentGet(L7_uint32 unitIndex, 
                                            L7_uint32 intIfNum, 
                                            L7_uint32 *val);
/*********************************************************************
* @purpose  Get the MSTP Statistics for the number of times this interface 
*           has transitioned into loop inconsistent state
*
* @param    unitIndex @b{(input)}  the unit for this operation
* @param    mstID     @b((input))  MST Instance ID 
* @param    intIfNum  @b{(input)}  interface number
* @param    val       @b{(output)} the current value of the Loop Inconsistent State Start stat
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStatsTransitionsIntoLoopInconsistentStateGet(L7_uint32 unitIndex,
                                                                          L7_uint32 mstID,
                                                                          L7_uint32 intIfNum,
                                                                          L7_uint32 *val);
/*********************************************************************
* @purpose  Get the MSTP Statistics for the number of times this interface 
*           has transitioned out of loop inconsistent state
*
* @param    unitIndex @b{(input)}  the unit for this operation
* @param    mstID     @b((input))  MST Instance ID 
* @param    intIfNum  @b{(input)}  interface number
* @param    val       @b{(output)} the current value of the Loop Inconsistent State End stat
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStatsTransitionsOutOfLoopInconsistentStateGet(L7_uint32 unitIndex,
                                                                           L7_uint32 mstID,
                                                                           L7_uint32 intIfNum,
                                                                           L7_uint32 *val);
/*********************************************************************
* @purpose  Clears the port stats
* 
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sPortStatsClear(L7_uint32 unitIndex, 
                                 L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the MSTP Designated Root Bridge Identifier in a specific 
*           instance for a particular interface
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstID     @b{(input)} the MSTID for the desired MTSI
* @param	intIfNum  @b{(input)} Internal Interface Number
* @param    buf       @b{(output)} Designated Root Identifier
* @param    len       @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sMstiPortDesignatedRootIDGet(L7_uint32 unitIndex, 
                                                     L7_uint32 mstID,
                                                     L7_uint32 intIfNum,
											         L7_uchar8 *buf, 
                                                     L7_uint32 *len);
/*********************************************************************
* @purpose  Get the CIST Regional Root Bridge Identifier for a particular interface
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param	intIfNum  @b{(input)} Internal Interface Number
* @param    buf       @b{(output)} Designated Root Identifier
* @param    len       @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortRegionalRootIDGet(L7_uint32 unitIndex, 
                                                   L7_uint32 intIfNum,
								                   L7_uchar8 *buf, 
                                                   L7_uint32 *len);
/*********************************************************************
* @purpose  Get the MSTP Port Hello Time parameter of a 
*           specific port for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param	intIfNum  @b{(input)} Internal Interface Number
* @param    val       @b{(output)} pointer to hello time value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Hello time is stored in multiples of 256
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortHelloTimeGet(L7_uint32 unitIndex, 
											  L7_uint32 intIfNum, 
											  L7_uint32 *val);
/*********************************************************************
* @purpose  Get the MSTP Port Path Cost parameter of a 
*           specific port for the CIST
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} pointer to CIST Path Cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortPathCostGet(L7_uint32 unitIndex, 
											 L7_uint32 intIfNum, 
											 L7_uint32 *val);
/*********************************************************************
* @purpose  Get the MSTP Max Hop count parameter for the device
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} pointer to Max Hop Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sBridgeMaxHopGet(L7_uint32 unitIndex, 
										 L7_uint32 *val);
/*********************************************************************
* @purpose  Set the MSTP Max Hop count parameter for the device
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(input)} Max Hop Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sBridgeMaxHopSet(L7_uint32 unitIndex, 
									     L7_uint32 val);
/*********************************************************************
* @purpose  Get the MSTP Tx Hold count parameter for the device
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(output)} pointer to Tx Hold Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sBridgeTxHoldCountGet(L7_uint32 unitIndex, 
										 L7_uint32 *val);
/*********************************************************************
* @purpose  Set the MSTP Tx Hold count parameter for the device
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    val       @b{(input)} Tx Hold Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sBridgeTxHoldCountSet(L7_uint32 unitIndex, 
									     L7_uint32 val);
/*********************************************************************
* @purpose  Get the Admin Hello Time for this port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} pointer to the adminHelloTime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortAdminHelloTimeGet(L7_uint32 unitIndex, 
											       L7_uint32 intIfNum, 
											       L7_uint32 *val);
/*********************************************************************
* @purpose  Set the Admin Hello Time for this port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} hello time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortAdminHelloTimeSet(L7_uint32 unitIndex, 
											       L7_uint32 intIfNum, 
											       L7_uint32 val);
/*********************************************************************
* @purpose  Get the CIST External Path Cost for this port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} pointer to path cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Use usmDbDot1sMstiPortPathCostGet with mstid 0 to get the 
*           internal port path cost for the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortExternalPathCostGet(L7_uint32 unitIndex, 
											         L7_uint32 intIfNum,
											         L7_uint32 *val);
/*********************************************************************
* @purpose  Set the CIST External Path Cost for this port
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} path cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Use usmDbDot1sMstiPortPathCostSet with mstid 0 to set the 
*           internal port path cost for the CIST
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sCistPortExternalPathCostSet(L7_uint32 unitIndex, 
											         L7_uint32 intIfNum,
											         L7_uint32 val);
/*********************************************************************
* @purpose  Get CIST External Port Path Cost Mode 
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments	returns true is Auto calculate is set else returns false
*
* @end
*********************************************************************/
extern L7_BOOL usmDbDot1sCistExtPortPathCostModeGet(L7_uint32 unitIndex, 
										            L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if the interface is valid in dot1s
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
extern L7_BOOL usmDbDot1sIsValidIntf(L7_uint32 unitIndex, L7_uint32 intIfNum);

/***************************************************************************
* @purpose  Set BPDU Guard setting
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    guard     @b{{input}} boolean guard value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
****************************************************************************/
extern L7_RC_t usmDbDot1sBpduGuardSet(L7_uint32 unitIndex, L7_BOOL guard);

/***************************************************************************
* @purpose  Get BPDU Guard setting for the bridge
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    guard     @b{{output}} boolean guard value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
****************************************************************************/
extern L7_RC_t usmDbDot1sBpduGuardGet(L7_uint32 unitIndex, L7_BOOL *guard);

/*********************************************************************
* @purpose  Get the BPDU Guard Effect for this port
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} boolean guard effect value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1sBpduGuardEffectGet(L7_uint32 unitIndex,
                                            L7_uint32 intIfNum,
                                            L7_BOOL   *val);

/***************************************************************************
* @purpose  Set BPDU filter for the bridge
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    guard     @b{{input}} boolean filter value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1sBpduFilterSet(L7_uint32 unitIndex,
                                L7_BOOL filter);

/***************************************************************************
* @purpose  Set BPDU filter for the bridge
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    guard     @b{{input}} boolean filter value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1sIntfBpduFilterSet(L7_uint32 unitIndex,
                                    L7_uint32 intIfNum,
                                    L7_BOOL filter);

/***************************************************************************
* @purpose  Get BPDU filter mode for the switch
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    guard     @b{{input}} boolean filter value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1sBpduFilterGet(L7_uint32 unitIndex,
                                    L7_BOOL *filter);

/***************************************************************************
* @purpose  Get BPDU filter mode for the interface
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    guard     @b{{input}} boolean filter value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1sIntfBpduFilterGet(L7_uint32 unitIndex,
                                    L7_uint32 intIfNum,
                                    L7_BOOL *filter);

/***************************************************************************
* @purpose  Set BPDU flood for the port
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    guard     @b{{input}} boolean filter value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1sIntfBpduFloodSet(L7_uint32 unitIndex,
                                   L7_uint32 intIfNum,
                                   L7_BOOL filter);

/***************************************************************************
* @purpose  Get BPDU flood mode for the interface
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
* @param    guard     @b{{input}} boolean filter value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1sIntfBpduFloodGet(L7_uint32 unitIndex,
                                    L7_uint32 intIfNum,
                                    L7_BOOL *filter);

/*********************************************************************
* @purpose  Get the current status of displaying Dot1s packet debug info
*            
* @param    none
*                      
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
extern void usmDbDot1sPacketDebugTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag);

/*********************************************************************
* @purpose  Turns on/off the displaying of Dot1s packet debug info
*            
* @param    flag         new value of the Packet Debug flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1sPacketDebugTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag);

/*********************************************************************
* @purpose  Determine if the mstid is in use
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstid  @b{(input)} mst instance  number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbdot1sInstCheckInUse(L7_uint32 unitIndex,L7_uint32 mstId);

/*********************************************************************
* @purpose  Confirm if the mstid is in use
*
* @param    unitIndex @b{(input)} the unit for this operation
* @param    mstid  @b{(input)} mst instance  number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This call will wait till the dot1s queue is processed to ensure
*           the instance create message is not on the queue.
*
* @end
*********************************************************************/
L7_RC_t usmDbdot1sInstCheckInUseConfirm(L7_uint32 unitIndex,
                                            L7_uint32 mstId);

/*********************************************************************
* @purpose  Get the base Mac Address
*
* @param    unitIndex @b{(input)}  the unit for this operation
* @param    buf       @b{(output)} Base Mac Address
*
* @comments
*
* @end
**********************************************************************/
void usmDbDot1sBaseMacAddrGet(L7_uint32 unitIndex,
                              L7_uchar8 *buf);



#endif /* USMDB_DOT1D_API_H */




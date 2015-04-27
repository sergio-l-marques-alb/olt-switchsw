/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\l7public\porting\usmdb_policy_api.h
*
* @purpose externs for USMDB layer policy APIs
*
* @component unitmgr
*
* @comments none
*
* @create 01/10/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_POLICY_API_H
#define USMDB_POLICY_API_H

/* R2 */

/*********************************************************************
*
* @purpose check to see if intIfNum is a valid policy Interface
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 intf       @b((input)) Interface Number
*
* @returns L7_TRUE     If valid interface
* @returns L7_FALSE    If not valid interface 
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_BOOL usmDbPolicyIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Set the Flow Control mode of an interface.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 intf  @b((input)) Physical or logical interface
*                        to be configured or MAX_INTERFACE_COUNT
*                        to apply configuration to all interfaces
* @param L7_uint32 mode  @b((input)) Admin mode (@b{L7_ENABLE or L7_DISABLE})
*
* @returns L7_SUCCESS  If success
* @returns L7_ERROR    If interface does not exist
* @returns L7_FAILURE  If other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbIfFlowCtrlModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode);


/*********************************************************************
*
* @purpose Get the Flow Control mode of an interface.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 intf   @b((input))  Physical or logical interface
*                         to be configured or MAX_INTERFACE_COUNT
*                         to apply configuration to all interfaces
* @param L7_uint32 *mode  @b((output)) Admin mode (@b{L7_ENABLE or L7_DISABLE})
*
* @returns L7_SUCCESS  If success
* @returns L7_ERROR    If interface does not exist
* @returns L7_FAILURE  If other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbIfFlowCtrlModeGet(L7_uint32 UnitIndex, L7_uint32 intf, L7_uint32 *mode);


/*********************************************************************
*
* @purpose Set the Flow Control mode of the system.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Mode (L7_ENABLE or L7_DISABLE)  
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlFlowControlModeSet(L7_uint32 UnitIndex, L7_uint32 mode);


/*********************************************************************
*
* @purpose Get the Broadcast Storm mode of the system. 
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlBcastStormModeGet(L7_uint32 UnitIndex, L7_uint32 *mode);


/*********************************************************************
*
* @purpose Get the Broadcast Storm threshold of the system. 
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_int32 *threshold  @b((output)) Threshold
* @param L7_RATE_UNIT_t *rate_unit  @b((output)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlBcastStormThresholdGet(L7_uint32 UnitIndex, L7_uint32 *threshold,
                                                    L7_RATE_UNIT_t *rate_unit);

/*********************************************************************
*
* @purpose Get the Multicast Storm mode of the system. 
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMcastStormModeGet(L7_uint32 UnitIndex, L7_uint32 *mode);

/*********************************************************************
*
* @purpose Get the Multicast Storm threshold of the system. 
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_int32 *threshold  @b((output)) Threshold
* @param L7_RATE_UNIT_t *rate_unit  @b((output)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMcastStormThresholdGet(L7_uint32 UnitIndex, L7_uint32 *threshold,
                                                    L7_RATE_UNIT_t *rate_unit);

/*********************************************************************
*
* @purpose Get the Destination lookup failure Storm mode of the system. 
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlUcastStormModeGet(L7_uint32 UnitIndex, L7_uint32 *mode);


/*********************************************************************
*
* @purpose Get the Destination lookup failure Storm threshold of the system. 
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_int32 *threshold  @b((output)) Threshold
* @param L7_RATE_UNIT_t *rate_unit  @b((output)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlUcastStormThresholdGet(L7_uint32 UnitIndex, L7_uint32 *threshold,
                                                    L7_RATE_UNIT_t *rate_unit);


/*********************************************************************
*
* @purpose Set the Broadcast Storm mode of the system.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlBcastStormModeSet(L7_uint32 UnitIndex, L7_uint32 mode);


/*********************************************************************
*
* @purpose Set the Broadcast Storm threshold of the system.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_int32  threshold  @b((input)) Threshold
* @param L7_RATE_UNIT_t rate_unit  @b((input)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlBcastStormThresholdSet(L7_uint32 UnitIndex, L7_uint32 threshold,
                                                    L7_RATE_UNIT_t rate_unit);


/*********************************************************************
*
* @purpose Set the Multicast Storm mode of the system.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMcastStormModeSet(L7_uint32 UnitIndex, L7_uint32 mode);


/*********************************************************************
*
* @purpose Set the Multicast Storm threshold of the system.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_int32  threshold  @b((input)) Threshold
* @param L7_RATE_UNIT_t rate_unit  @b((input)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMcastStormThresholdSet(L7_uint32 UnitIndex, L7_uint32 threshold,
                                                    L7_RATE_UNIT_t rate_unit);

/*********************************************************************
*
* @purpose Set the Destination lookup failure Storm mode of the system.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlUcastStormModeSet(L7_uint32 UnitIndex, L7_uint32 mode);


/*********************************************************************
*
* @purpose Set the Destination lookup failure Storm threshold of the system.
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_int32  threshold  @b((input)) Threshold
* @param L7_RATE_UNIT_t rate_unit  @b((input)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlUcastStormThresholdSet(L7_uint32 UnitIndex, L7_uint32 threshold,
                                                    L7_RATE_UNIT_t rate_unit);

/*********************************************************************
*
* @purpose Set the Broadcast Storm mode of an interface.
*
* @param L7_uint32 interface  @b((input)) The interface for this operation
* @param L7_uint32 val        @b((input)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlBcastStormModeIntfSet(L7_uint32 interface, L7_uint32 mode);


/*********************************************************************
*
* @purpose Set the Broadcast Storm threshold of an interface.
*
* @param L7_uint32 interface  @b((input)) The interface for this operation
* @param L7_int32  threshold  @b((input)) Threshold
* @param L7_RATE_UNIT_t rate_unit  @b((input)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlBcastStormThresholdIntfSet(L7_uint32 interface, L7_uint32 threshold,
                                                        L7_RATE_UNIT_t rate_unit);

/*********************************************************************
*
* @purpose Set the Multicast Storm mode of an interface.
*
* @param L7_uint32 interface  @b((input)) The interface for this operation
* @param L7_uint32 val        @b((input)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMcastStormModeIntfSet(L7_uint32 interface, L7_uint32 mode);


/*********************************************************************
*
* @purpose Set the Multicast Storm threshold of an interface.
*
* @param L7_uint32 interface  @b((input)) The interface for this operation
* @param L7_int32  threshold  @b((input)) Threshold
* @param L7_RATE_UNIT_t rate_unit  @b((input)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMcastStormThresholdIntfSet(L7_uint32 interface, L7_uint32 threshold,
                                                        L7_RATE_UNIT_t rate_unit);

/*********************************************************************
*
* @purpose Set the Destination lookup failure Storm mode of an interface.
*
* @param L7_uint32 interface  @b((input)) The interface for this operation
* @param L7_uint32 val        @b((input)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlUcastStormModeIntfSet(L7_uint32 interface, L7_uint32 mode);


/*********************************************************************
*
* @purpose Set the Destination lookup failure Storm threshold of an interface.
*
* @param L7_uint32 interface  @b((input)) The interface for this operation
* @param L7_int32  threshold  @b((input)) Threshold
* @param L7_RATE_UNIT_t rate_unit  @b((input)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlUcastStormThresholdIntfSet(L7_uint32 interface, L7_uint32 threshold,
                                                        L7_RATE_UNIT_t rate_unit);

/*********************************************************************
*
* @purpose Get the Broadcast Storm mode of an interface. 
*
* @param L7_uint32 interface  @b((input))  The interface for this operation
* @param L7_uint32 *val       @b((output)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlBcastStormModeIntfGet(L7_uint32 interface, L7_uint32 *mode);


/*********************************************************************
*
* @purpose Get the Broadcast Storm threshold of an interface. 
*
* @param L7_uint32 interface  @b((input))  The interface for this operation
* @param L7_int32 *threshold  @b((output)) Threshold
* @param L7_RATE_UNIT_t *rate_unit  @b((output)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlBcastStormThresholdIntfGet(L7_uint32 interface, L7_uint32 *threshold,
                                                        L7_RATE_UNIT_t *rate_unit);

/*********************************************************************
*
* @purpose Get the Multicast Storm mode of an interface. 
*
* @param L7_uint32 interface  @b((input))  The interface for this operation
* @param L7_uint32 *val       @b((output)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMcastStormModeIntfGet(L7_uint32 interface, L7_uint32 *mode);


/*********************************************************************
*
* @purpose Get the Multicast Storm threshold of an interface. 
*
* @param L7_uint32 interface  @b((input))  The interface for this operation
* @param L7_uint32 *threshold @b((output)) Threshold
* @param L7_RATE_UNIT_t *rate_unit  @b((output)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMcastStormThresholdIntfGet(L7_uint32 interface, L7_uint32 *threshold,
                                                        L7_RATE_UNIT_t *rate_unit);

/*********************************************************************
*
* @purpose Get the Destination lookup failure Storm mode of an interface. 
*
* @param L7_uint32 interface  @b((input))  The interface for this operation
* @param L7_uint32 *val       @b((output)) Mode (L7_ENABLE or L7_DISABLE) 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlUcastStormModeIntfGet(L7_uint32 interface, L7_uint32 *mode);


/*********************************************************************
*
* @purpose Get the Destination lookup failure Storm threshold of an interface. 
*
* @param L7_uint32 interface  @b((input))  The interface for this operation
* @param L7_int32 *threshold  @b((output)) Threshold
* @param L7_RATE_UNIT_t *rate_unit  @b((output)) Units of PERCENT or PPS 
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlUcastStormThresholdIntfGet(L7_uint32 interface, L7_uint32 *threshold,
                                                        L7_RATE_UNIT_t *rate_unit);


/*********************************************************************
*
* @purpose Get the Flow Control mode of the system.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Mode (L7_ENABLE or L7_DISABLE)  
*
* @returns L7_SUCCESS  If success
* @returns L7_FAILURE  If failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlFlowControlModeGet(L7_uint32 UnitIndex, L7_uint32 *mode);


#endif

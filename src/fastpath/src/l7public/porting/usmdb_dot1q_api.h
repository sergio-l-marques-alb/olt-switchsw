/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_dot1q_api.h
*
* @purpose externs for USMDB layer
*
* @component unitmgr
*
* @comments LVL7 extensions to VLAN MIB support
*
* @create 01/10/2001
*
* @author rjindal
* @end
*
**********************************************************************/


#ifndef USMDB_DOT1Q_API_H
#define USMDB_DOT1Q_API_H
                             
#include "nimapi.h"
#include "dot1q_api.h"
#include "dot1q_exports.h"

/*********************************************************************
*
* @purpose Returns the number of miscellaneous discards
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRxMiscDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose Returns the number of discards due to vlan mismatches
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRxVlanMismatchDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the number of dynamic VLANs.
*
* @param    UnitIndex   Unit
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDynamicVlanEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the max VLAN entries allowed.
*
* @param    UnitIndex   Unit
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbMostVlanEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the number of static VLANs.
*
* @param    UnitIndex   Unit
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbStaticVlanEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Returns the number of discards due to vlan mismatches
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTxVlanMismatchDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose Returns the number of discards due to unacceptable frames
* @purpose type mismatch for 802.1Q VLAN classification
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbUnacceptableFrameDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Create a new VLAN.
*
* @param UnitIndex the unit for this operation
* @param vlanID    the desired ID of the new VLAN
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanCreate(L7_uint32 UnitIndex, L7_uint32 vlanID);


/* will be replaced with VLAN MIB definitions?? */
/*********************************************************************
*
* @purpose  Set VLAN port membership
*
* @param    UnitIndex   L7_uint32 the unit for this operation,
*                       or L7_ALL_INTERFACES for all interfaces
* @param    vlanIndex   vlan ID
* @param    val         mode of participation
*                       (@b{  DOT1Q_FIXED, DOT1Q_FORBIDDEN, or
*                             DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanAllMemberSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_uint32 val,DOT1Q_SWPORT_MODE_t swport_mode);

/*********************************************************************
*
* @purpose  Set the tagging configuration for a member port
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    vlanIndex   vlan ID
* @param    mode        tagging mode
*                       (@b{  DOT1Q_UNTAGGED or 
*                             DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    mode
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanAllTaggedSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Delete a VLAN.
*
* @param UnitIndex the unit for this operation
* @param val       to be deleted VLAN ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanDelete(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose  Check to see if a VLAN exists based on a
*           VLAN ID.
*
* @param    UnitIndex   Unit
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    added 10/4/00 djohnson
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanIDGet(L7_uint32 UnitIndex, L7_uint32 index);

/*********************************************************************
*
* @purpose  Get a vlan ID from a vlan name
*
* @param    *vlanName   name of a VLAN
* @param    *vlanId     VLAN ID
*
* @returns  L7_SUCCESS, if name found
* @returns  L7_FAILURE, if no VLAN matches the name
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanIdFromNameGet(const L7_char8 *vlanName, L7_uint32 *vlanId);

/*********************************************************************
*
* @purpose  Get VLAN port membership
*
* @param    UnitIndex   unit
* @param    vindex      vlan ID
* @param    index       physical or logical interface to be add
* @param    *val        pointer to mode of participation
*                       (@b{  DOT1Q_FIXED, DOT1Q_FORBIDDEN, or
*                             DOT1Q_NORMAL_REGISTRATION})
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanMemberSetGet(L7_uint32 UnitIndex, L7_uint32 vindex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get current VLAN port membership
*
* @param    UnitIndex   unit
* @param    vindex      vlan ID
* @param    index       physical or logical interface to be add
* @param    *val        pointer to mode of participation
*                       (@b{  DOT1Q_FIXED, DOT1Q_FORBIDDEN, or
*                             DOT1Q_NORMAL_REGISTRATION})
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
*
* @end
*********************************************************************/
extern L7_RC_t usmDbOperVlanMemberSetGet(L7_uint32 UnitIndex, L7_uint32 vindex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set VLAN port membership
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    vlanIndex   vlan ID
* @param    interface   physical or logical interface to be add
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    val         mode of participation
*                       (@b{  DOT1Q_FIXED, DOT1Q_FORBIDDEN, or
*                             DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanMemberSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_uint32 interface, L7_uint32 val,DOT1Q_SWPORT_MODE_t swport_mode);

/*********************************************************************
*
* @purpose  Reset VLANS to factory defaults.
*
* @param    UnitIndex   Unit
* @param    val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanResetToDefaultSet(L7_uint32 UnitIndex, L7_uint32 val);
 
/*********************************************************************
*
* @purpose  Get the VLAN type 
*
* @param    UnitIndex   unit
* @param    vid         vlan ID
* @param    *val        pointer to mode of participation
*                       L7_DOT1Q_DEFAULT,
*                       L7_DOT1Q_ADMIN
*                       L7_DOT1Q_DYNAMIC
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This information is LVL7 status only. 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanTypeGet(L7_uint32 UnitIndex, L7_uint32 vid, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Convert a dynamically learned VLAN into an 
*           administratively configured one.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    vlanIndex   vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanMakeStaticSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex);

/*********************************************************************
*
* @purpose Determine if an interface has been aquired by a LAG
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *status    L7_TRUE: aquired, L7_FALSE: released
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE  if intf doesn't exist
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbIntfAquiredGet(L7_uint32 UnitIndex, L7_uint32 intf, L7_uint32 *status);

/*********************************************************************
* @purpose  Gets the component ID which has acquired the interface
*
* @param    intIfNum        interface
* @param    *compId         the ID of the acquiring component
*
* @returns  L7_SUCCESS     if success
* @returns  L7_NOT_EXISTS  if VLAN does not exist
*
* @notes    Storage must be allocated by the caller
*           The component ID returned is either valid or L7_FIRST_COMPONENT_ID
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIntfAcquiredCompGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_COMPONENT_IDS_t *compId);

/*********************************************************************
* @purpose  Get a list of VLANs an interface is a member of
*
* @param    unitIndex   Unit Index
* @param    intfNum     interface
* @param    *data       pointer to data
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Storage must be allocated by the caller, 2*4*(L7_MAX_VLANS+1)
* @notes    Uses an array of dot1qVidList_t to store data
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanVIDListGet(L7_uint32 unitIndex, L7_uint32 intfNum, L7_uint32 *data, L7_uint32 *numOfElements);

/*********************************************************************
* @purpose  Get a list of VLANs an interface is a statically configured in
*
* @param    unitIndex   Unit Index
* @param    intfNum     interface
* @param    *data       pointer to data
* @param    *numOfElements length of list (vid,tag)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    Storage must be allocated by the caller, 2*4*(L7_MAX_VLANS+1)
* @notes    Uses an array of dot1qVidList_t to store data
* @end
*********************************************************************/
L7_RC_t usmDbVlanVIDListCfgGet(L7_uint32 unitIndex, L7_uint32 intfNum, L7_uint32 *data, L7_uint32 *numOfElements);

/*********************************************************************
* @purpose  Check to see if the port is the appropriate type on which
*			the parm can be set
*
* @param    unitIndex           Unit Index 
* @param 	intIfNum			Internal interface number
*
* @returns  L7_SUCCESS			The parm can be set on this port type
* @returns  L7_NOT_SUPPORTED	The port does not support this parm to be set
*
* @notes    presently this function tests only for logical vlan intf.
*                                 
* @end
*********************************************************************/
L7_RC_t usmDbQportsPortParmCanSet(L7_uint32 unitIndex, L7_uint32 intIfNum);
/*********************************************************************
* @purpose  For a given interface set vlan membership for a range of vlans
*
* @param    unitIndex	Unit Number
* @param    intIfNum    Internal Interface Number
* @param    vidMask     Pointer Vlan ID Mask
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the vlans,
*           whose membership is changing, in other words only the delta
*           This routine will set the membership for all valid vlans and
*           not abort in the case of a non existant or dynamic vlan
*
*
* @end
*********************************************************************/
L7_RC_t usmDbQportsVlanMembershipMaskSet(L7_uint32 unitIndex, 
										 L7_uint32 intIfNum, 
										 L7_VLAN_MASK_t *vidMask, 
										 L7_uint32 mode,
                                         DOT1Q_SWPORT_MODE_t swport_mode);
/*********************************************************************
* @purpose  Set the tagging configuration for a interface on a range of vlans
*
* @param    unitIndex	Unit Number
* @param    intIfNum    Internal Interface Number
* @param    vidMask     Pointer Vlan ID Mask
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or 
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    This API expects the mask bits turned on for only the vlans,
*           whose tagging is changing, in other words only the delta
*           This routine will set the tagging for all valid vlans and
*           not abort in the case of a non existant or dynamic vlan
*
*
* @end
*********************************************************************/
L7_RC_t usmDbQportsVlanTagMaskSet(L7_uint32 unitIndex, 
								  L7_uint32 intIfNum, 
								  L7_VLAN_MASK_t *vidMask, 
								  L7_uint32 tagMode);

/*********************************************************************
* @purpose  Create a range of vlans
*
* @param    unitIndex	Unit Number
* @param    vidMask     Pointer Vlan ID Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    The vlan mask need not be contiguous
*
*
* @end
*********************************************************************/
L7_RC_t usmDbVlanCreateMaskSet(L7_uint32 unitIndex, L7_VLAN_MASK_t *vidMask);
/*********************************************************************
* @purpose  Delete a range of vlans
*
* @param    unitIndex	Unit Number
* @param    vidMask     Vlan ID Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    The vlan mask need not be contiguous
*
*
* @end
*********************************************************************/
L7_RC_t usmDbVlanDeleteMaskSet(L7_uint32 unitIndex, L7_VLAN_MASK_t *vidMask);

/*********************************************************************
* @purpose Is a valid vlan
*
* @param    unitIndex   Unit Number
* @param    vid         Vlan ID 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    
*
*
* @end
*********************************************************************/
L7_RC_t usmDbVlanIsValid(L7_uint32 unitIndex, L7_uint32 vid);

/*********************************************************************
* @purpose  Set the SwitchPort Mode configuration for a port
*
* @param    intf        physical or logical interface to be configured
* @param    L7_uint32   new_switchport_mode  New switchportmode to be set
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if error in input or other error
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qSwPortModeSet(L7_uint32 unit, NIM_INTF_MASK_t *mask, L7_uint32 mode);

/*********************************************************************
* @purpose  Set the Access vlan for a port
*
* @param    intf        physical or logical interface to be configured
* @param    L7_uint32   vlanId  VlanId to be set
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if error in input or other error
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qSwPortAccessVlanSet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 vlanId);

/*********************************************************************
* @purpose  Get the SwitchPort Mode configuration for a port
*
* @param    intf        physical or logical interface to be configured
* @param    L7_uint32   new_switchport_mode  New switchportmode to be set
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if error in input or other error
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qSwPortModeGet(L7_uint32 unit,L7_uint32 interface, L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the Access vlan for a port
*
* @param    intf        physical or logical interface to be configured
* @param    L7_uint32   vlanId  VlanId to be set
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if error in input or other error
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qSwPortAccessVlanGet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32* vlanId);

/*********************************************************************
* @purpose  Is the vlan access vlan for any port?
*
* @param    vid         vlan ID
*
* @returns  L7_TRUE, if yes L7_FALSE otherwise
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL usmDbDot1qIsAccessVlanForAnyPort(L7_uint32 unit, L7_uint32 vlanId);


/*********************************************************************
* @purpose  Set the direction from the base VLAN ID when looking for an
*           unused VLAN ID to assign to a port-based routing interface. 
*
* @param    policy  (input)   up or down 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbDot1qInternalVlanPolicySet(DOT1Q_INTERNAL_VLAN_POLICY policy);

/*********************************************************************
* @purpose  Get the direction from the base VLAN ID when looking for an
*           unused VLAN ID to assign to a port-based routing interface. 
*
* @param    policy  (input)   up or down 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
DOT1Q_INTERNAL_VLAN_POLICY usmDbDot1qInternalVlanPolicyGet(void);

/*********************************************************************
* @purpose  Set the VLAN ID used as a starting point to internal VLAN assignments.
*
* @param    baseVlanId  (input)   VLAN ID 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbDot1qBaseInternalVlanIdSet(L7_uint32 baseVlanId);

/*********************************************************************
* @purpose  Get the VLAN ID used as a starting point to internal VLAN assignments.
*
* @param    void
*
* @returns  VLAN ID
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_uint32 usmDbDot1qBaseInternalVlanIdGet(void);

/*********************************************************************
* @purpose  Find the first VLAN ID (lowest numerical ID) claimed for internal use
*
* @param    vlanId          @b{(output)} pointer to first internal vlan 
*
* @returns  L7_SUCCESS if a VLAN ID found
* @returns  L7_FAILURE if no VLAN IDs used internally                         
*
* @notes    
*                                                   
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qInternalVlanFindFirst(L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Find the next VLAN ID (in numeric order) claimed for internal use
*
* @param    vid      @b{(input)} previous VLAN ID
* @param    nextvid  @b{(output)} next vlanId 
*
* @returns  L7_SUCCESS if next VLAN ID found
* @returns  L7_FAILURE if no more VLAN IDs used internally                         
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbDot1qInternalVlanFindNext(L7_uint32 vid, L7_uint32 *nextvid);


/*********************************************************************
* @purpose  Find the usage description for the given internal vlan
*
* @param    vid          @b{(input)} vlan ID
* @param    *descr       @b{(output)} pointer to usage description 
*
* @returns  L7_SUCCESS if VLAN ID is found and is internal
* @returns  L7_FAILURE if the VLAN ID is not listed as internal                        
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbDot1qInternalVlanUsageDescrGet(L7_uint32 vid, L7_char8 *descr);

#endif

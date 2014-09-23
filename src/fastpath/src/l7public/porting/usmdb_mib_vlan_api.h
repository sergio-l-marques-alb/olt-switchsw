/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\public\usmdb_mib_vlan.h
*
* @purpose usmdb support for LVL7 extensions to VLAN support
*
* @component unitmgr
*
* @comments tba
*
* @create 01/10/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_MIB_VLAN_API_H
#define USMDB_MIB_VLAN_API_H

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_garp.h"
#include "dot1q_exports.h"

/* dot1dBase */
/*********************************************************************
*
* @purpose get the optional parts of IEEE 802.1D that are
*          implemented and are manageable through this MIB.   
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *buf       port capabilities
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
extern L7_RC_t usmDbDot1dPortCapabilitiesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_char8 *buf );

extern L7_RC_t usmDbDot1dPortCapabilitiesSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the default ingress User Priority for a specific
*           port.  This only has effect on media, such as Ethernet,
*           that do not support native User Priority
*
* @param    UnitIndex   Unit
* @param    BasePort    interface number       
* @param    *val        returned value
*
* @returns  L7_SUCCESS  If the value was successfully obtained
* @returns  L7_FAILURE  Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dPortDefaultUserPriorityGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  Set the default ingress User Priority for a specific
*           port.  This only has effect on media, such as Ethernet,
*           that do not support native User Priority
*
* @param    UnitIndex   the unit for this operation
* @param    intIfNum    Internal Interface Number 
* @param    priority    priority, from 0-7
*
* @returns  L7_SUCCESS  If the value was successfully set
* @returns  L7_FAILURE  Other failure
*
* @notes    none
*       
* @end
**********************************************************************/
extern L7_RC_t usmDbDot1dPortDefaultUserPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 priority);

/*********************************************************************
*
* @purpose  Get the number of traffic classes
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *val        Number of traffic classes supported, in range of 1-8
*                       optionally readonly
*
* @returns  L7_SUCCESS        If the value was successfully set 
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dPortNumTrafficClassesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
* @purpose  Set dot1d number of traffic classes
*
* @param    intIfNum             Internal Interface Number 
* @param    numTrafficClasses    number of traffic classes, from 1-8
*
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
*
* @notes    This API is no longer supported.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dPortNumTrafficClassesSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 numTrafficClasses);

/*********************************************************************
*
* @purpose  get GARP Join time, in centiseconds
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *val       returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Other failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dPortGarpJoinTimeGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

extern L7_RC_t usmDbDot1dPortGarpJoinTimeSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  get GARP Leave time, in centiseconds
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
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
extern L7_RC_t usmDbDot1dPortGarpLeaveTimeGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

extern L7_RC_t usmDbDot1dPortGarpLeaveTimeSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  get GARP LeaveAll time, in centiseconds
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
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
extern L7_RC_t usmDbDot1dPortGarpLeaveAllTimeGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

extern L7_RC_t usmDbDot1dPortGarpLeaveAllTimeSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  get the administrative state of GMRP operation on this port.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    enabled == 1
* @notes    disabled == 2
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dPortGmrpStatusGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

extern L7_RC_t usmDbDot1dPortGmrpStatusSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  get the total number of failed GMRP registrations, for any
*           reason, in all VLANs, on this port
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
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
extern L7_RC_t usmDbDot1dPortGmrpFailedRegistrationsGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

extern L7_RC_t usmDbDot1dPortGmrpFailedRegistrationsSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  get the Source MAC Address of the last GMRP message
*          received on this port
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *buf        returned value of counter
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
extern L7_RC_t usmDbDot1dPortGmrpLastPduOriginGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_char8 *buf );

extern L7_RC_t usmDbDot1dPortGmrpLastPduOriginSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_char8 *buf );

/*********************************************************************
*
* @purpose  get the PVID, the VLAN ID assigned to untagged frames or
*           Priority-Tagged frames received on this port
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
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
extern L7_RC_t usmDbDot1qPvid ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the acceptable frame type for a specific port.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    admitAll == 1
* @notes    admitOnlyVlanTagged == 2
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qPortAcceptableFrameTypes ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get VLAN ingress filtering status per port.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes true == 1
* @notes false == 2 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qPortIngressFiltering ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the state of GVRP operation on this port.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
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
extern L7_RC_t usmDbDot1qPortGvrpStatus ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the total number of failed GVRP registrations, for any
*           reason, on this port.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
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
extern L7_RC_t usmDbDot1qPortGvrpFailedRegistrations ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the source MAC address of the last GVRP PDU received
*           on this port
*
* @param    intIfNum    internal interface number
* @param    *macAddr    pointer to a structure in which to store the mac
*                       address
*
* @returns  L7_SUCCESS,  if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
*
* @notes     
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qPortGvrpLastPduOrigin ( L7_uint32 UnitIndex, L7_uint32 BasePort, L7_uchar8 *buf );

/* dot1dTpHCPort */
/*********************************************************************
*
* @purpose  Get statistics information for each high capacity port of a
*           transparent bridge
*
* @param    UnitIndex   Unit
* @param    Port        port number       
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
extern L7_RC_t dot1dTpHCPortEntryGet ( L7_uint32 UnitIndex, L7_int32 Port );

extern L7_RC_t dot1dTpHCPortEntrySet ( L7_uint32 UnitIndex, L7_int32 Port );

/*********************************************************************
*
* @purpose  get the number of frames that have been received by this
*           transparnet high capacity port from its segment
*
* @param    UnitIndex       Unit
* @param    Port            port number       
* @param    *high           returned high value of counter
* @param    *low            returned low value of counter
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
extern L7_RC_t dot1dTpHCPortInFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high, L7_uint32 *low );

extern L7_RC_t dot1dTpHCPortInFramesSet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high, L7_uint32 *low );

/*********************************************************************
*
* @purpose  get the number of valid frames that have been received by this
*           transparnet high capacity port from its segment which were
*           discarded (i.e., filtered) by the Forwarding Process.
*
* @param    UnitIndex       Unit
* @param    Port            port number       
* @param    *high           returned high value of counter
* @param    *low            returned low value of counter
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
extern L7_RC_t dot1dTpHCPortInDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high, L7_uint32 *low );

extern L7_RC_t dot1dTpHCPortInDiscardsSet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high, L7_uint32 *low );

/*********************************************************************
*
* @purpose  get the number of frames that have been transmited by this
*           transparnet high capacity port to its segment
*
* @param    UnitIndex       Unit
* @param    Port            port number       
* @param    *high           returned high value of counter
* @param    *low            returned low value of counter
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
extern L7_RC_t dot1dTpHCPortOutFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high, L7_uint32 *low );

extern L7_RC_t dot1dTpHCPortOutFramesSet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high, L7_uint32 *low );

/*********************************************************************
*
* @purpose  get the next transparnet high capacity port entry
*
* @param    UnitIndex   Unit
* @param    Port        port number       
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
extern L7_RC_t dot1dTpHCPortEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *Port );

/* dot1dTpPortOverflow */
/*********************************************************************
*
* @purpose  Get the most significant bits of statistics counters for a high
*           capacity interface of a transparent bridge
*
* @param    UnitIndex   Unit
* @param    Port   port number       
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
extern L7_RC_t dot1dTpPortOverflowEntryGet ( L7_uint32 UnitIndex, L7_int32 Port );

extern L7_RC_t dot1dTpPortOverflowEntrySet ( L7_uint32 UnitIndex, L7_int32 Port );

/*********************************************************************
*
* @purpose  Get the net entry of most significant bits of statistics
*           counters for a high capacity interface of a transparent bridge
*
* @param    UnitIndex   Unit
* @param    Port   port number       
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
extern L7_RC_t dot1dTpPortOverflowEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *Port );

/*********************************************************************
*
* @purpose  Get the number of times the associated dot1dTpPortInFrames
*           counter has overflowed.
*
* @param    UnitIndex   Unit
* @param    BasePort    interface number       
* @param    *high       returned high value of counter
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
extern L7_RC_t dot1dTpPortInOverflowFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high );

extern L7_RC_t dot1dTpPortInOverflowFramesSet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high );

/*********************************************************************
*
* @purpose  Get the number of times the associated dot1dTpPortInDiscards
*           counter has overflowed.
*
* @param    UnitIndex   Unit
* @param    BasePort    interface number       
* @param    *high       returned high value of counter
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
extern L7_RC_t dot1dTpPortInOverflowDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high );

extern L7_RC_t dot1dTpPortInOverflowDiscardsSet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high );

/*********************************************************************
*
* @purpose  Get the number of times the associated dot1dTpPortOutFrames
*           counter has overflowed.
*
* @param    UnitIndex   Unit
* @param    BasePort    interface number       
* @param    *high       returned high value of counter
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
extern L7_RC_t dot1dTpPortOutOverflowFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high );

extern L7_RC_t dot1dTpPortOutOverflowFramesSet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high );

/* dot1dExt */
/*********************************************************************
*
* @purpose  Get device capabilities
*
* @param    none
*
* @returns  deviceCapabilities   bitmask mapping of functionality
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dDeviceCapabilitiesGet ( L7_uint32 UnitIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Set device capabilities
*
* @param    UnitIndex   Unit
* @param    *buf        value to set
*
* @returns  L7_SUCCESS      If the value was successfully set 
* @returns  L7_ERROR        error 
* @returns  L7_FAILURE      Catastrophic failure 
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dDeviceCapabilitiesSet ( L7_uint32 UnitIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get traffic class capability
*
* @param    UnitIndex   Unit
* @param    *val        returned status value
*
* @returns  L7_SUCCESS      If the value was successfully obtained
* @returns  L7_ERROR        error 
* @returns  L7_FAILURE      Catastrophic failure
*
* @notes    If successful, the *val parameter indicates L7_TRUE if
*           more than one priority level is enabled, or L7_FALSE if a 
*           single priority level is enabled.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTrafficClassesEnabledGet ( L7_uint32 UnitIndex, L7_BOOL *val );

/*********************************************************************
*
* @purpose  Set traffic class capability
*
* @param    UnitIndex   Unit
* @param    val         status value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS        If the value was successfully set 
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    A val of L7_TRUE indicates more than one priority level
*           is enabled, while L7_FALSE indicates a single priority level
*           is enabled.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTrafficClassesEnabledSet ( L7_uint32 UnitIndex, L7_BOOL val );

/*********************************************************************
*
* @purpose  Get the administrative status requested by management for
*           GMRP
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
extern L7_RC_t usmDbDot1dGmrpStatusGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
*
* @purpose  Set the administrative status requested by management for
*           GMRP
*
* @param    UnitIndex   Unit
* @param    *val        value to set
*
* @returns  L7_SUCCESS      If the value was successfully set 
* @returns  L7_ERROR        error 
* @returns  L7_FAILURE      Catastrophic failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dGmrpStatusSet ( L7_uint32 UnitIndex, L7_int32 val );

/* dot1dUserPriorityRegen */
/*********************************************************************
*
* @purpose  Validate the Regenerated User Priority the incoming User
*           Priority is mapped to for this port.
*
* @param    UnitIndex       Unit
* @param    BasePort        interface number       
* @param    UserPriority    user priority
*
* @returns  L7_SUCCESS      If the value was successfully validated
* @returns  L7_ERROR        error 
* @returns  L7_FAILURE      Catastrophic failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dUserPriorityRegenEntryGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 UserPriority );

/*********************************************************************
*
* @purpose  Get the next Regenerated User Priority the incoming User
*           Priority is mapped to for this port.
*
* @param    UnitIndex       Unit
* @param    *BasePort        interface number       
* @param    *UserPriority    returned value of counter 
*
* @returns  L7_SUCCESS      If the value was successfully obtained
* @returns  L7_ERROR        error 
* @returns  L7_FAILURE      Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dUserPriorityRegenEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *BasePort, L7_int32 *UserPriority );

/*********************************************************************
*
* @purpose  Get the Regenerated User Priority the incoming User
*           Priority is mapped to for this port
*
* @param    UnitIndex       Unit
* @param    BasePort        interface number       
* @param    UserPriority    user priority
* @param    *val            returned value
*
* @returns  L7_SUCCESS      If the value was successfully obtained
* @returns  L7_ERROR        error 
* @returns  L7_FAILURE      Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dRegenUserPriorityGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 UserPriority, L7_int32 *val );

/*********************************************************************
*
* @purpose  Set the Regenerated User Priority the incoming User
*           Priority is mapped to for this port.
*
* @param    UnitIndex       Unit
* @param    BasePort        port number       
* @param    UserPriority    user priority
* @param    val             value to set
*
* @returns  L7_SUCCESS      If the value was successfully set
* @returns  L7_ERROR        error 
* @returns  L7_FAILURE      Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dRegenUserPrioritySet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 UserPriority, L7_int32 val );

/* dot1dTrafficClass */
/*********************************************************************
*
* @purpose  Validate the User Priority to Traffic Class mapping.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    Priority   priority
*
* @returns  L7_SUCCESS        If the value was successfully validated
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTrafficClassEntryGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 Priority );

/*********************************************************************
*
* @purpose  Get the next User Priority to Traffic Class mapping.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    Priority   priority
*
* @returns  L7_SUCCESS        If the value was successfully obtained
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTrafficClassEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *BasePort, L7_int32 *Priority );

/*********************************************************************
*
* @purpose  Get the next User Priority to Traffic Class mapping for
*           Traffic Class per switch not per interface
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    Priority   priority
*
* @returns  L7_SUCCESS        If the value was successfully obtained
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTrafficClassPerSwitchEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *Priority );


/*********************************************************************
*
* @purpose  Get the User Priority to Traffic Class mapping.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    Priority   priority
* @param    *val       value to get
*
* @returns  L7_SUCCESS        If the value was successfully set
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTrafficClassGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 Priority, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the default User Priority to Traffic Class mapping.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    Priority   priority
* @param    *val       value to get
*
* @returns  L7_SUCCESS        If the value was successfully set
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dDefaultTrafficClassGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 Priority, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the User Priority to Traffic Class mapping.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    Priority   priority
* @param    *val       value to get
*
* @returns  L7_SUCCESS        If the value was successfully set
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTrafficClassSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 Priority, L7_int32 val );

/* dot1dPortOutboundAccessPriority */
/*********************************************************************
*
* @purpose  Get a Regenerated User Priority to Outbound Access Priority
*           mapping.
*
* @param    UnitIndex           Unit
* @param    BasePort            interface number       
* @param    RegenUserPriority   priority
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Other failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dPortOutboundAccessPriorityEntryGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 RegenUserPriority );

extern L7_RC_t usmDbDot1dPortOutboundAccessPriorityEntrySet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 RegenUserPriority );

/*********************************************************************
*
* @purpose  Get the Outbound Access Priority the received frame is
*          mapped to.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number 
* @param    RegenUserPriority   priority
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
extern L7_RC_t usmDbDot1dPortOutboundAccessPriorityGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 RegenUserPriority, L7_int32 *val );

extern L7_RC_t usmDbDot1dPortOutboundAccessPrioritySet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 RegenUserPriority, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the next Regenerated User Priority to Outbound Access Priority
*           mapping.
*
* @param    UnitIndex           Unit
* @param    *BasePort            interface number       
* @param    *RegenUserPriority   priority
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
extern L7_RC_t usmDbDot1dPortOutboundAccessPriorityEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *BasePort, L7_int32 *RegenUserPriority );

/* dot1qBase */
/*********************************************************************
*
* @purpose  Get dot1q Vlan Version Number
*
* @param    none
*
* @returns  IEEE 802.1Q version number supported by this device
*
* @notes     
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qVlanVersionNumberGet ( L7_uint32 UnitIndex, L7_int32 *val );

extern L7_RC_t usmDbDot1qVlanVersionNumberSet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
*
* @purpose  Get dot1q Max VLAN ID        
*
* @param    none
*
* @returns  Maximum VLAN ID supported by this device
*
* @notes     
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qMaxVlanIdGet ( L7_uint32 UnitIndex, L7_int32 *val );

extern L7_RC_t usmDbDot1qMaxVlanIdSet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
*
* @purpose  Get dot1q Max Supported VLANs
*
* @param    none
*
* @returns  Maximum number of VLANs supported by this device
*
* @notes     
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qMaxSupportedVlansGet ( L7_uint32 UnitIndex, L7_uint32 *val );

extern L7_RC_t usmDbDot1qMaxSupportedVlansSet ( L7_uint32 UnitIndex, L7_uint32 val );

/*********************************************************************
*
* @purpose  Get dot1q Number Vlans
*
* @param    none
*
* @returns  number of VLANs currently configured on this device
*
* @notes     
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qNumVlansGet ( L7_uint32 UnitIndex, L7_uint32 *val );

extern L7_RC_t usmDbDot1qNumVlansSet ( L7_uint32 UnitIndex, L7_uint32 val );

/*********************************************************************
*
* @purpose  Get the administrative status for GVRP.
*
* @param    UnitIndex   Unit
* @param    *val        L7_ENABLE, L7_DISABLE
*
* @returns  L7_SUCCESS     
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qGvrpStatusGet ( L7_uint32 UnitIndex, L7_int32 *val );

extern L7_RC_t usmDbDot1qGvrpStatusSet ( L7_uint32 UnitIndex, L7_int32 val );

/* dot1qFdb */
/*********************************************************************
*
* @purpose  Get information about a specific Filtering Database.
*
* @param    UnitIndex   Unit
* @param    *FdbId        returned value of counter
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
extern L7_RC_t usmDbDot1qFdbEntryGet ( L7_uint32 UnitIndex, L7_uint32 FdbId );

extern L7_RC_t usmDbDot1qFdbEntrySet ( L7_uint32 UnitIndex, L7_uint32 FdbId );

/*********************************************************************
*
* @purpose  Get the current number of dynamic entries in this
*           Filtering Database.
*
* @param    UnitIndex   Unit
* @param    FdbId       forwarding DB ID       
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
extern L7_RC_t usmDbDot1qFdbDynamicCountGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_uint32 *val );

extern L7_RC_t usmDbDot1qFdbDynamicCountSet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_uint32 *val );

/*********************************************************************
*
* @purpose  Get information about the next Filtering Database
*
* @param    UnitIndex   Unit
* @param    *FdbId        returned value of counter
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
extern L7_RC_t usmDbDot1qFdbEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *FdbId );

/* dot1qTpFdb */
/*********************************************************************
*
* @purpose  Get information about a specific unicast MAC address for
*           which the device has some forwarding and/or filtering
*            information.
*
* @param    UnitIndex       Unit
* @param    FdbId           forwarding DB ID       
* @param    *FdbAddress     returned value of counter
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
extern L7_RC_t usmDbDot1qTpFdbEntryGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress );

extern L7_RC_t usmDbDot1qTpFdbEntrySet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress );

/*********************************************************************
*
* @purpose  Get information about the next unicast MAC address for
*           which the device has some forwarding and/or filtering
*            information.
*
* @param    UnitIndex       Unit
* @param    *FdbId          forwarding DB ID       
* @param    *FdbAddress     returned value of counter
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
extern L7_RC_t usmDbDot1qTpFdbEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *FdbId, L7_char8 *FdbAddress );

/*********************************************************************
*
* @purpose  Get either the value '0', or the port number of the port on
*           which a frame having a source address equal to the value
*           of the corresponding instance of dot1qTpFdbAddress has
*           been seen.
*
* @param    UnitIndex   Unit
* @param    FdbId       forwarding db id 
* @param    *FdbAddress fdb address      
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
extern L7_RC_t usmDbDot1qTpFdbPortGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress, L7_int32 *val );

extern L7_RC_t usmDbDot1qTpFdbPortSet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the status of this entry.
*
* @param    UnitIndex   Unit
* @param    FdbId       forwarding db id 
* @param    *FdbAddress fdb address      
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    other(1) - none of the following.  This may include
*                the case where some other MIB object (not the
*                corresponding instance of dot1qTpFdbPort, nor an
*                entry in the dot1qStaticUnicastTable) is being
*                used to determine if and how frames addressed to
*                the value of the corresponding instance of
*                dot1qTpFdbAddress are being forwarded.
* @notes    invalid(2) - this entry is no longer valid (e.g., it
*                was learned but has since aged out), but has not
*                yet been flushed from the table.
* @notes    learned(3) - the value of the corresponding instance
*                of dot1qTpFdbPort was learned and is being used.
* @notes    self(4) - the value of the corresponding instance of
*                dot1qTpFdbAddress represents one of the device's
*                addresses.  The corresponding instance of
*                dot1qTpFdbPort indicates which of the device's
*                ports has this address.
* @notes    mgmt(5) - the value of the corresponding instance of
*                dot1qTpFdbAddress is also the value of an
*                existing instance of dot1qStaticAddress."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qTpFdbStatusGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress, L7_int32 *val );

extern L7_RC_t usmDbDot1qTpFdbStatusSet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress, L7_int32 *val );

/* dot1qTpGroup */
/*********************************************************************
*
* @purpose  Get a filtering information entry for VLANs configured
*           into the bridge.
*
* @param    UnitIndex       Unit
* @param    VlanIndex       vlan index       
* @param    *GroupAddress   group address
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
extern L7_RC_t usmDbDot1qTpGroupEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *GroupAddress );

extern L7_RC_t usmDbDot1qTpGroupEntrySet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *GroupAddress );

/*********************************************************************
*
* @purpose  Get the next filtering information entry for VLANs configured
*           into the bridge.
*
* @param    UnitIndex       Unit
* @param    VlanIndex       vlan index       
* @param    *GroupAddress   group address
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
extern L7_RC_t usmDbDot1qTpGroupEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex, L7_char8 *GroupAddress );

/*********************************************************************
*
* @purpose  Get complete set of ports, in this VLAN, to which
*           frames destined for this Group MAC address are currently
*           being explicitly forwarded.
*
* @param    UnitIndex       Unit
* @param    VlanIndex       vlan index      
* @param    *GroupAddress   group address
* @param    *buf            returned value
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
extern L7_RC_t usmDbDot1qTpGroupEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *GroupAddress, L7_char8 *buf, L7_uint32 *buf_length );

/*********************************************************************
*
* @purpose  Get the subset of ports in dot1qTpGroupEgressPorts which
*           were learnt by GMRP or some other dynamic mechanism, in
*           this Filtering database.
*
* @param    UnitIndex       Unit
* @param    VlanIndex       vlan index      
* @param    *GroupAddress   group address
* @param    *buf            returned value
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
extern L7_RC_t usmDbDot1qTpGroupLearntGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *GroupAddress, L7_char8 *buf, L7_uint32 *buf_length );

/* dot1qForwardAll */
/*********************************************************************
*
* @purpose  Get forwarding information for a VLAN, specifying the set
*           of ports to which all multicasts should be forwarded,
*           configured statically by management or dynamically by
*           GMRP.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   interface number       
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
extern L7_RC_t usmDbDot1qForwardAllEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex );

extern L7_RC_t usmDbDot1qForwardAllEntrySet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex );

/*********************************************************************
*
* @purpose  Get forwarding information for the next VLAN, specifying
*           the set of ports to which all multicasts should be forwarded,
*           configured statically by management or dynamically by
*           GMRP.
*
* @param    UnitIndex   Unit
* @param    *VlanIndex  vlan index       
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
extern L7_RC_t usmDbDot1qForwardAllEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex );

/*********************************************************************
*
* @purpose  Get the complete set of ports in this VLAN to which all
*           multicast group-addressed frames are to be forwarded.
*
* @param    UnitIndex   Unit
* @param    VlanIndex  vlan index      
* @param    *buf       returned value of counter
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
extern L7_RC_t usmDbDot1qForwardAllPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Set the complete set of ports in this VLAN to which all
*           multicast group-addressed frames are to be forwarded.
*
* @param    UnitIndex   Unit
* @param    VlanIndex  vlan index      
* @param    *buf       returned value of counter
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
extern L7_RC_t usmDbDot1qForwardAllPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the manually configured set of ports in this VLAN to
*           which all multicast group-addressed frames are to be forwarded.
*
* @param    UnitIndex   Unit
* @param    VlanIndex  vlan index      
* @param    *buf       returned value of counter
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
extern L7_RC_t usmDbDot1qForwardAllStaticPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Set the manually configured set of ports in this VLAN to
*           which all multicast group-addressed frames are to be forwarded.
*
* @param    UnitIndex   Unit
* @param    VlanIndex  vlan index      
* @param    *buf       returned value of counter
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
extern L7_RC_t usmDbDot1qForwardAllStaticPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the set of ports configured by management in this VLAN
*           for which the Service Requirement attribute Forward All
*           Multicast Groups may not be dynamically registered by
*           GMRP.
*
* @param    UnitIndex   Unit
* @param    VlanIndex  vlan index      
* @param    *buf       returned value of counter
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
extern L7_RC_t usmDbDdot1qForwardAllForbiddenPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Set the set of ports configured by management in this VLAN
*           for which the Service Requirement attribute Forward All
*           Multicast Groups may not be dynamically registered by
*           GMRP.
*
* @param    UnitIndex   Unit
* @param    VlanIndex  vlan index      
* @param    *buf       returned value of counter
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
extern L7_RC_t usmDbDdot1qForwardAllForbiddenPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/* dot1qForwardUnregistered */
/*********************************************************************
*
* @purpose  Get the Forwarding information for a VLAN, specifying the set
*           of ports to which all multicasts for which there is no
*           more specific forwarding information shall be forwarded.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index       
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
extern L7_RC_t usmDbDot1qForwardUnregisteredEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex );

extern L7_RC_t usmDbDot1qForwardUnregisteredEntrySet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex );

/*********************************************************************
*
* @purpose  Get the Forwarding information for the next VLAN, specifying the set
*           of ports to which all multicasts for which there is no
*           more specific forwarding information shall be forwarded.
*
* @param    UnitIndex   Unit
* @param    *VlanIndex   vlan index       
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
extern L7_RC_t usmDbDot1qForwardUnregisteredEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex );

/*********************************************************************
*
* @purpose  Get the complete set of ports in this VLAN to which
*           multicast group-addressed frames for which there is no
*           more specific forwarding information will be forwarded.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index       
* @param    *buf        returned value of counter
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
extern L7_RC_t usmDbDot1qForwardUnregisteredPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the complete set of ports in this VLAN to which
*           multicast group-addressed frames for which there is no
*           more specific forwarding information will be forwarded.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index       
* @param    *buf        returned value of counter
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
extern L7_RC_t usmDbDot1qForwardUnregisteredPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the complete set of ports in this VLAN to which
*           multicast group-addressed frames for which there is no
*           more specific forwarding information will be forwarded.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index       
* @param    *buf        returned value of counter
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
extern L7_RC_t usmDbDot1qForwardUnregisteredStaticPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the complete set of ports in this VLAN to which
*           multicast group-addressed frames for which there is no
*           more specific forwarding information will be forwarded.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index       
* @param    *buf        returned value of counter
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
extern L7_RC_t usmDbDot1qForwardUnregisteredStaticPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the set of ports configured by management in this VLAN
*           for which the Service Requirement attribute Forward
*           Unregistered Multicast Groups may not be dynamically
*           registered by GMRP.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index       
* @param    *buf        returned value of counter

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
extern L7_RC_t usmDbDdot1qForwardUnregisteredForbiddenPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the set of ports configured by management in this VLAN
*           for which the Service Requirement attribute Forward
*           Unregistered Multicast Groups may not be dynamically
*           registered by GMRP.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index       
* @param    *buf        returned value of counter
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
extern L7_RC_t usmDbDdot1qForwardUnregisteredForbiddenPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/* dot1qStaticUnicast */
/*********************************************************************
*
* @purpose  Get the Filtering information configured into the device by
*           (local or network) management specifying the set of
*           ports to which frames received from a specific port and
*           containing a specific unicast destination address are
*           allowed to be forwarded.
*
* @param    UnitIndex       Unit
* @param    FdbId           forwarding DB ID      
* @param    *Address        address
* @param    ReceivePort    receive port
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
extern L7_RC_t usmDbDot1qStaticUnicastEntryGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort );

extern L7_RC_t usmDbDot1qStaticUnicastEntrySet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort );

/*********************************************************************
*
* @purpose  Get the next Filtering information configured into the device by
*           (local or network) management specifying the set of
*           ports to which frames received from a specific port and
*           containing a specific unicast destination address are
*           allowed to be forwarded.
*
* @param    UnitIndex       Unit
* @param    FdbId           forwarding DB ID      
* @param    *Address        address
* @param    *ReceivePort    receive port

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
extern L7_RC_t usmDbDot1qStaticUnicastEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *FdbId, L7_char8 *Address, L7_int32 *ReceivePort );

/*********************************************************************
*
* @purpose  Get the set of ports for which a frame with a specific
*           unicast address will be flooded in the event that it
*           has not been learned.
*
* @param    UnitIndex       Unit
* @param    FdbId           forwarding DB ID      
* @param    *Address        address
* @param    ReceivePort    receive port
* @param    *buf            buffer
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
extern L7_RC_t usmDbDot1qStaticUnicastAllowedToGoToGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Set the set of ports for which a frame with a specific
*           unicast address will be flooded in the event that it
*           has not been learned.
*
* @param    UnitIndex       Unit
* @param    FdbId           forwarding DB ID      
* @param    *Address        address
* @param    ReceivePort    receive port
* @param    *buf            buffer*
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
extern L7_RC_t usmDbDot1qStaticUnicastAllowedToGoToSet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the status of static unicast.
*
* @param    UnitIndex       Unit
* @param    FdbId           forwarding DB ID      
* @param    *Address        address
* @param    ReceivePort    receive port
* @param    *val            value returned
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    other(1) - this entry is currently in use but
*                the conditions under which it will remain
*                so differ from the following values.
* @notes    invalid(2) - writing this value to the object
*                removes the corresponding entry.
* @notes     permanent(3) - this entry is currently in use
*                and will remain so after the next reset of
*                the bridge.
* @notes     deleteOnReset(4) - this entry is currently in
*                use and will remain so until the next
*                reset of the bridge.
* @notes    deleteOnTimeout(5) - this entry is currently in
*               use and will remain so until it is aged out.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qStaticUnicastStatusGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  Set the status of static unicast.
*
* @param    UnitIndex       Unit
* @param    FdbId           forwarding DB ID      
* @param    *Address        address
* @param    ReceivePort    receive port
* @param    val            value returned
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    other(1) - this entry is currently in use but
*                the conditions under which it will remain
*                so differ from the following values.
* @notes    invalid(2) - writing this value to the object
*                removes the corresponding entry.
* @notes     permanent(3) - this entry is currently in use
*                and will remain so after the next reset of
*                the bridge.
* @notes     deleteOnReset(4) - this entry is currently in
*                use and will remain so until the next
*                reset of the bridge.
* @notes    deleteOnTimeout(5) - this entry is currently in
*               use and will remain so until it is aged out.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qStaticUnicastStatusSet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 val );

/* dot1qStaticMulticast */
/*********************************************************************
*
* @purpose  Get Filtering information configured into the device by
*           (local or network) management specifying the set of
*           ports to which frames received from this specific port
*           for this VLAN and containing this Multicast or Broadcast
*           destination address are allowed to be forwarded.
*
* @param    UnitIndex       Unit
* @param    VlanIndex       vlan index      
* @param    *Address        address
* @param    ReceivePort    receive port
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
extern L7_RC_t usmDbDot1qStaticMulticastEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort );

extern L7_RC_t usmDbDot1qStaticMulticastEntrySet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort );

/*********************************************************************
*
* @purpose  Get the next Filtering information configured into the device by
*           (local or network) management specifying the set of
*           ports to which frames received from this specific port
*           for this VLAN and containing this Multicast or Broadcast
*           destination address are allowed to be forwarded.
*
* @param    UnitIndex       Unit
* @param    *VlanIndex       vlan index      
* @param    *Address        address
* @param    *ReceivePort    receive port
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
extern L7_RC_t usmDbDot1qStaticMulticastEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex, L7_char8 *Address, L7_int32 *ReceivePort );

/*********************************************************************
*
* @purpose  Get the set of ports to which frames received from a
*           specific port and destined for a specific Multicast or
*           Broadcast MAC address must be forwarded, regardless of
*           any dynamic information e.g. from GMRP.
*
* @param    UnitIndex       Unit
* @param    VlanIndex      vlan index      
* @param    *Address        address
* @param    ReceivePort     receive port       
* @param    *buf            buffer
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
extern L7_RC_t usmDbDot1qStaticMulticastStaticEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Set the set of ports to which frames received from a
*           specific port and destined for a specific Multicast or
*           Broadcast MAC address must be forwarded, regardless of
*           any dynamic information e.g. from GMRP.
*
* @param    UnitIndex       Unit
* @param    VlanIndex      vlan index      
* @param    *Address        address
* @param    ReceivePort     receive port       
* @param    *buf            buffer
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
extern L7_RC_t usmDbDot1qStaticMulticastStaticEgressPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the set of ports to which frames received from a
*           specific port and destined for a specific Multicast or
*           Broadcast MAC address must not be forwarded, regardless
*           of any dynamic information e.g. from GMRP. 
*
* @param    UnitIndex       Unit
* @param    VlanIndex      vlan index      
* @param    *Address        address
* @param    ReceivePort     receive port       
* @param    *buf            buffer
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
extern L7_RC_t usmDbDot1qStaticMulticastForbiddenEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Set the set of ports to which frames received from a
*           specific port and destined for a specific Multicast or
*           Broadcast MAC address must not be forwarded, regardless
*           of any dynamic information e.g. from GMRP. 
*
* @param    UnitIndex       Unit
* @param    VlanIndex      vlan index      
* @param    *Address        address
* @param    ReceivePort     receive port       
* @param    *buf            buffer

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
extern L7_RC_t usmDbDot1qStaticMulticastForbiddenEgressPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the static multicast status.
*
* @param    UnitIndex       Unit
* @param    VlanIndex      vlan index      
* @param    *Address        address
* @param    ReceivePort     receive port       
* @param    *val            value returned
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    other(1) - this entry is currently in use but
*                the conditions under which it will remain
*                so differ from the following values.
* @notes     invalid(2) - writing this value to the object
*                removes the corresponding entry.
* @notes     permanent(3) - this entry is currently in use
*                and will remain so after the next reset of
*                the bridge.
* @notes     deleteOnReset(4) - this entry is currently in
*                use and will remain so until the next
*                reset of the bridge.
* @notes     deleteOnTimeout(5) - this entry is currently in
*                use and will remain so until it is aged out.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qStaticMulticastStatusGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the static multicast status.
*
* @param    UnitIndex       Unit
* @param    VlanIndex      vlan index      
* @param    *Address        address
* @param    ReceivePort     receive port       
* @param    *val            value returned
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
extern L7_RC_t usmDbDot1qStaticMulticastStatusSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 *val );

/* dot1qVlan */
/*********************************************************************
*
* @purpose  Get the number of times a VLAN entry has been deleted from
*           the dot1qVlanCurrentTable (for any reason).
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
extern L7_RC_t usmDbDot1qVlanNumDeletesGet ( L7_uint32 UnitIndex, L7_uint32 *val );

/*********************************************************************
*
* @purpose  Determine if a new VLAN can be supported
*
* @param    none
*
* @returns  0, if another VLAN cannot be supported
* @returns  >=4096 if another VLAN can be supported

*
* @notes     
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qNextFreeLocalVlanIndexGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
*
* @purpose  Get the dentity of the constraint set to which a VLAN
*           belongs, if there is not an explicit entry for that VLAN
*           in dot1qLearningConstraintsTable.
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
extern L7_RC_t usmDbDot1qConstraintSetDefaultGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
*
* @purpose  Set the dentity of the constraint set to which a VLAN
*           belongs, if there is not an explicit entry for that VLAN
*           in dot1qLearningConstraintsTable.
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
extern L7_RC_t usmDbDot1qConstraintSetDefaultSet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
*
* @purpose  Get the type of constraint set to which a VLAN belongs, if
*           there is not an explicit entry for that VLAN in
*           dot1qLearningConstraintsTable.
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
extern L7_RC_t usmDbDot1qConstraintTypeDefaultGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
*
* @purpose  Set the type of constraint set to which a VLAN belongs, if
*           there is not an explicit entry for that VLAN in
*           dot1qLearningConstraintsTable. 
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
extern L7_RC_t usmDbDot1qConstraintTypeDefaultSet ( L7_uint32 UnitIndex, L7_int32 val );

/* dot1qVlanCurrent */
/*********************************************************************
*
* @purpose  Get the next Information for a VLAN configured into the device by
*           (local or network) management, or dynamically created
*           as a result of GVRP requests received.
*
* @param    UnitIndex   Unit
* @param    *TimeMark   time mark       
* @param    VlanIndex  Vlan Index
*
* @returns  L7_SUCCESS   If the next VLAN can be found    
*			              
* @returns  L7_ERROR      
* @returns  L7_FAILURE    
*			    		  
*
* @notes    If 0 is passed, next=1
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qVlanCurrentEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *TimeMark, L7_uint32 *VlanIndex );

/*********************************************************************
*
* @purpose  Get Information for a VLAN configured into the device by
*           (local or network) management, or dynamically created
*           as a result of GVRP requests received.
*
* @param    UnitIndex   Unit
* @param    TimeMark   time mark       
* @param    VlanIndex  Vlan Index
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
extern L7_RC_t usmDbDot1qVlanCurrentEntryGet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex );

extern L7_RC_t usmDbDot1qVlanCurrentEntrySet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex );

/*********************************************************************
*
* @purpose  Get the VLANs forwarding database ID.
*
* @param    UnitIndex   Unit
* @param    TimeMark   time mark
* @param    VlanIndex   Vlan Index       
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
extern L7_RC_t usmDbDot1qVlanFdbIdGet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_uint32 *val );

extern L7_RC_t usmDbDot1qVlanFdbIdSet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_uint32 *val );

/*********************************************************************
*
* @purpose  Get the set of ports which are transmitting traffic for
*           this VLAN as either tagged or untagged frames.
*
* @param    UnitIndex   Unit
* @param    TimeMark    time mark
* @param    VlanIndex   Vlan Index       
* @param    *buf        buffer
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
extern L7_RC_t usmDbDot1qVlanCurrentEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *size );

extern L7_RC_t usmDbDot1qVlanCurrentEgressPortsSet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the set of ports which are transmitting traffic for
*           this VLAN as untagged frames.
*
* @param    UnitIndex   Unit
* @param    TimeMark    time mark
* @param    VlanIndex   Vlan Index       
* @param    *buf        buffer
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
extern L7_RC_t usmDbDot1qVlanCurrentUntaggedPortsGet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *size );

extern L7_RC_t usmDbDot1qVlanCurrentUntaggedPortsSet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the status of a vlan entry.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    other(1) - this entry is currently in use but the
*                conditions under which it will remain so differ
*                from the following values.
* @notes     permanent(2) - this entry, corresponding to an entry
*                in dot1qVlanStaticTable, is currently in use and
*                will remain so after the next reset of the
*                device.  The port lists for this entry include
*                ports from the equivalent dot1qVlanStaticTable
*                entry and ports learnt dynamically.
* @notes     dynamicGvrp(3) - this entry is currently in use
*                and will remain so until removed by GVRP.  There
*                is no static entry for this VLAN and it will be
*                removed when the last port leaves the VLAN.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qVlanStatus ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_int32 *val);
/*********************************************************************
*
* @purpose  Get the value of sysUpTime when this VLAN was created.
*
* @param    UnitIndex   Unit
* @param    TimeMark    time mark
* @param    VlanIndex   Vlan Index       
* @param    *time       time value
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
extern L7_RC_t usmDbDot1qVlanCreationTime ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, usmDbTimeSpec_t *time);

/*********************************************************************
*
* @purpose  Set the given set of ports as members of this vlan(based on mode)
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to mask of ports. Bits which are set
*                       correspond to ports which are members of the vlan
* @param    val         mode of participation
*                       (@b{  DOT1Q_FIXED, DOT1Q_FORBIDDEN, or
*                             DOT1Q_NORMAL_REGISTRATION})
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure

*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanMemberMaskSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 mode ,DOT1Q_SWPORT_MODE_t swport_mode2);

/*********************************************************************
*
* @purpose  Set the given set of ports to transmit tagged/untagged frames
*           in this vlan (based on the mode)
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to mask of ports. Bits which are set
*                       correspond to ports which are members of the vlan
* @param    mode        tagging mode
*                       (@b{  DOT1Q_UNTAGGED or
*                             DOT1Q_TAGGED})
*
*
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanTaggedMemberMaskSet (L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf,L7_uint32 tagMode );
/* dot1qVlanStatic */
/*********************************************************************
*
* @purpose  Get the Static information entry for a VLAN configured into the
*           device by (local or network) management.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index       
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
extern L7_RC_t usmDbDot1qVlanStaticEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex );

extern L7_RC_t usmDbDot1qVlanStaticEntrySet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex );

/*********************************************************************
*
* @purpose  Get the next  Static information entry for a VLAN configured
*           into the device by (local or network) management.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index
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
extern L7_RC_t usmDbDot1qVlanStaticEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex );

/*********************************************************************
*
* @purpose  Get the administratively assigned string used
*           to identify the VLAN.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index
* @param    *buf        buffer
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
extern L7_RC_t usmDbDot1qVlanStaticNameGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Set the administratively assigned string used
*           to identify the VLAN.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index
* @param    *buf        buffer
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
extern L7_RC_t usmDbDot1qVlanStaticNameSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );
/*********************************************************************
*
* @purpose  Get the VLAN static egress ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set 
*                       correspond to ports which are members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qVlanStaticEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *length);

/*********************************************************************
*
* @purpose  Set the VLAN static egress ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set 
*                       correspond to ports which are members of the vlan
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
extern L7_RC_t usmDbDot1qVlanStaticEgressPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the VLAN Forbidden Egress Ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set 
*                       correspond to ports which cannot be members of the vlan
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
extern L7_RC_t usmDbDot1qVlanForbiddenEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *length );

/*********************************************************************
*
* @purpose  Set the VLAN Forbidden Egress Ports
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set 
*                       correspond to ports which cannot be members of the vlan
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
extern L7_RC_t usmDbDot1qVlanForbiddenEgressPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get set of ports which are transmitting untagged frames 
*           in this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned 
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure

*
* @notes     
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qVlanStaticUntaggedPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *length );

/*********************************************************************
*
* @purpose  Set the set of ports which are transmitting untagged frames 
*           in this vlan
*
* @param    vlanID      VLAN ID
* @param    *mask       pointer to a storage area to contain returned 
*                       interface bitmask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure

*
* @notes     
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qVlanStaticUntaggedPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Gets the status of the specified entry in the 
*           dot1qVlanStaticTable
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    " This object indicates the status of this entry."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qVlanStaticRowStatusGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_int32 *val );

/*********************************************************************
*
* @purpose  Sets the status of the specified entry in the 
*           dot1qVlanStaticTable
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    " This object indicates the status of this entry."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qVlanStaticRowStatusSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_int32 *val );

/* dot1qPortVlanStatistics */
/*********************************************************************
*
* @purpose  Get the next Traffic statistics entry for a VLAN on an interface.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qPortVlanStatisticsEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *BasePort, L7_uint32 *VlanIndex );

/*********************************************************************
*
* @purpose  Get a Traffic statistics entry for a VLAN on an interface.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qPortVlanStatisticsEntryGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex );

extern L7_RC_t usmDbDot1qPortVlanStatisticsEntrySet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex );

/*********************************************************************
*
* @purpose  Get the number of valid frames received by this port from
*           its segment which were classified as belonging to this VLAN.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qTpVlanPortInFramesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

extern L7_RC_t usmDbDot1qTpVlanPortInFramesSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

/*********************************************************************
*
* @purpose  Get the number of valid frames transmitted by this port to
*           its segment from the local forwarding process for this VLAN.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qTpVlanPortOutFramesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

extern L7_RC_t usmDbDot1qTpVlanPortOutFramesSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

/*********************************************************************
*
* @purpose  Get the number of valid frames received by this port from
*           its segment which were classified as belonging to this
*           VLAN which were discarded due to VLAN related reasons.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qTpVlanPortInDiscardsGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

extern L7_RC_t usmDbDot1qTpVlanPortInDiscardsSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

/*********************************************************************
*
* @purpose  Get the number of times the associated
*           dot1qTpVlanPortInFrames counter has overflowed.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qTpVlanPortInOverflowFramesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

extern L7_RC_t usmDbDot1qTpVlanPortInOverflowFramesSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

/*********************************************************************
*
* @purpose  Get the number of times the associated
*           dot1qTpVlanPortOutFrames counter has overflowed.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qTpVlanPortOutOverflowFramesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

extern L7_RC_t usmDbDot1qTpVlanPortOutOverflowFramesSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

/*********************************************************************
*
* @purpose  Get the number of times the associated
*           dot1qTpVlanPortInDiscards counter has overflowed.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qTpVlanPortInOverflowDiscardsGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

extern L7_RC_t usmDbDot1qTpVlanPortInOverflowDiscardsSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val );

/* dot1qPortVlanHCStatistics */
/*********************************************************************
*
* @purpose  Get Traffic statistics for a VLAN on a high capacity
*           interface.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qPortVlanHCStatisticsEntryGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex );

extern L7_RC_t usmDbDot1qPortVlanHCStatisticsEntrySet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex );

/*********************************************************************
*
* @purpose  Get the nextTraffic statistics entry for a VLAN on a
*           high capacity interface.
*
* @param    UnitIndex   Unit
* @param    *BasePort   interface number
* @param    *VlanIndex  vlan index
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
extern L7_RC_t usmDbDot1qPortVlanHCStatisticsEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *BasePort, L7_uint32 *VlanIndex );

/*********************************************************************
*
* @purpose  Get the number of valid frames received by this port from
*           its segment which were classified as belonging to this VLAN.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    VlanIndex  vlan index
* @param    *big_end        ??
* @param    *little_end     ??
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
extern L7_RC_t usmDbDot1qTpVlanPortHCInFramesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *big_end, L7_uint32 *little_end );

extern L7_RC_t usmDbDot1qTpVlanPortHCInFramesSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *big_end, L7_uint32 *little_end );

/*********************************************************************
*
* @purpose  Get the number of valid frames transmitted by this port to
*           its segment from the local forwarding process for this VLAN.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    VlanIndex  vlan index
* @param    *big_end        ??
* @param    *little_end     ??
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
extern L7_RC_t usmDbDot1qTpVlanPortHCOutFramesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *big_end, L7_uint32 *little_end );

extern L7_RC_t usmDbDot1qTpVlanPortHCOutFramesSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *big_end, L7_uint32 *little_end );

/*********************************************************************
*
* @purpose  Get the number of valid frames received by this port from
*           its segment which were classified as belonging to this
*           VLAN which were discarded due to VLAN related reasons.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number       
* @param    VlanIndex  vlan index
* @param    *big_end        ??
* @param    *little_end     ??
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
extern L7_RC_t usmDbDot1qTpVlanPortHCInDiscardsGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *big_end, L7_uint32 *little_end );

extern L7_RC_t usmDbDot1qTpVlanPortHCInDiscardsSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *big_end, L7_uint32 *little_end );

/* dot1qLearningContstraints */
/*********************************************************************
*
* @purpose  Get a learning constraint defined for a VLAN.
*
* @param    UnitIndex   Unit
* @param    Vlan        vlan         
* @param    set         set
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
extern L7_RC_t usmDbDot1qLearningConstraintsEntryGet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set );

extern L7_RC_t usmDbDot1qLearningConstraintsEntrySet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set );

/*********************************************************************
*
* @purpose  Get the next learning constraint defined for a VLAN.
*
* @param    UnitIndex   Unit
* @param    Vlan        vlan         
* @param    *set         set
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
extern L7_RC_t usmDbDot1qLearningConstraintsEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *Vlan, L7_int32 *Set );

/*********************************************************************
*
* @purpose  Gets the type of constraint this entry defines.
*
* @param    UnitIndex   Unit
* @param    Vlan        vlan         
* @param    *set         set       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
* 
* @notes    Types of constraints:
*             independent(1)
*             shared(2)
*
*           " The type of constraint this entry defines.  independent(1)
*             - the VLAN, dot1qConstraintVlan, uses an independent
*             filtering database from all other VLANs in the same
*             set, defined by dot1qConstraintSet."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qConstraintTypeGet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set, L7_int32 *val );

/*********************************************************************
*
* @purpose  Sets the type of constraint this entry defines.
*
* @param    UnitIndex   Unit
* @param    Vlan        vlan         
* @param    *set         set       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			                found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    Types of constraints:
*             independent(1)
*             shared(2)
*
*           " The type of constraint this entry defines.  independent(1)
*             - the VLAN, dot1qConstraintVlan, uses an independent
*             filtering database from all other VLANs in the same
*             set, defined by dot1qConstraintSet."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1qConstraintTypeSet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set, L7_int32 val );

/*********************************************************************
*
* @purpose  Get the constraint status.
*
* @param    UnitIndex   Unit
* @param    Vlan        vlan         
* @param    *set         set       
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
extern L7_RC_t usmDbDot1qConstraintStatusGet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set, L7_int32 *val );

/*********************************************************************
*
* @purpose  Set the constraint status.
*
* @param    UnitIndex   Unit
* @param    Vlan        vlan         
* @param    set         set       
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
extern L7_RC_t usmDbDot1qConstraintStatusSet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set, L7_int32 val );

/*********************************************************************
*
* @purpose  Determine if the given interface is valid for dot1q
*
* @param    UnitIndex   Unit
* @param    intIfNum    Internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbDot1qIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get the GVRP configuration for an interface
*
* @param    UnitIndex   unit
* @param    index       physical or logical interface to be configured
* @param    *val        L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, always
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsEnableGVRPGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);
/*********************************************************************
*
* @purpose  Get the GMRP configuration for an interface
*
* @param    UnitIndex   unit
* @param    index       physical or logical interface to be configured
* @param    *val        L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, always
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpGmrpPortEnableGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Enable or Disable GVRP for an interface
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    mode        L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsEnableGVRPSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 val);
/*********************************************************************
*
* @purpose  Enable or Disable GMRP for an interface
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    mode        L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpGmrpPortEnableSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the VID configuration for a port
*
* @param    UnitIndex   unit
* @param    index       physical or logical interface to be configured
* @param    *val        DOT1Q_DEFAULT_VID, DOT1Q_IP_VID, DOT1Q_IPX_VID,
*                       DOT1Q_NETBIOS_VID
*
* @returns  vid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsPVIDGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Cuurent VID configuration for a port
*
* @param    UnitIndex   unit
* @param    index       physical or logical interface to be configured
* @param    *val        DOT1Q_DEFAULT_VID, DOT1Q_IP_VID, DOT1Q_IPX_VID,
*                       DOT1Q_NETBIOS_VID
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsCurrentPVIDGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Set the VID configuration for a port
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    vlanID      vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist(configuration is applied)
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsPVIDSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 vlanID);

/*********************************************************************
*
* @purpose  Get the Acceptable Frame Type configuration for a port
*
* @param    UnitIndex   unit
* @param    index       physical or logical interface to be configured
* @param    *val        DOT1Q_ADMIT_ALL, DOT1Q_ADMIT_UNTAGGED_ONLY
*
* @returns  L7_SUCCESS, always
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsAcceptFrameTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the Acceptable Frame Type configuration for a port
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    type        DOT1Q_ADMIT_ALL, DOT1Q_ADMIT_UNTAGGED_ONLY
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsAcceptFrameTypeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 type);

/*********************************************************************
*
* @purpose  Get the Ingress Filtering configuration for a port
*
* @param    UnitIndex   unit
* @param    index       physical or logical interface to be configured
* @param    val         L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, always
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsEnableIngressFilteringGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Cuurent Ingress Filtering configuration for a port
*
* @param    UnitIndex   unit
* @param    index       physical or logical interface to be configured
* @param    val         L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, always
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsCurrentIngressFilteringGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the Ingress Filtering configuration for a port
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    mode        L7_ENABLE  or L7_DISABLE
*

* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbQportsEnableIngressFilteringSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 model);

/*********************************************************************
*
* @purpose  Get a vlan name
*
* @param    UnitIndex   unit
* @param    index       vlan ID
* @param    *buf        pointer to string in which to store vlan name
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
extern L7_RC_t usmDbVlanNameGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf);

/*********************************************************************
*
* @purpose  Configure an optional VLAN name
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param vlanID    L7_uint32 the VLAN ID to configure
* @param *name     L7_char8  the desired name, max length=MAX_VLAN_NAME
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN doesn't exist
* @returns  L7_ERROR, if failure
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanNameSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose  Get the tagging configuration for a member port
*
* @param    UnitIndex   unit
* @param    vindex      vlan ID
* @param    index       physical or logical interface to be configured
* @param    *val        Pointer to tagging mode
*                       (@b{  DOT1Q_UNTAGGED or 
*                             DOT1Q_TAGGED})
*

* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    Use dot1q_set_vlanCfg() instead of this routine when possible
*
* @end
*********************************************************************/
extern L7_RC_t usmDbVlanTaggedSetGet(L7_uint32 UnitIndex, L7_uint32 vindex, L7_uint32 index, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the tagging configuration for a member port of a dynamic vlan 
*
* @param    UnitIndex   unit
* @param    vindex      vlan ID
* @param    index       physical or logical interface to be configured
* @param    *val        Pointer to tagging mode
*                       (@b{  DOT1Q_UNTAGGED or
*                             DOT1Q_TAGGED})
*

* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    Use dot1q_set_vlanCfg() instead of this routine when possible
*	        This is required for dynamic vlans. 
*
* @end
*********************************************************************/
 L7_RC_t usmDbDot1qOperVlanTaggedMemberGet(L7_uint32 UnitIndex, L7_uint32 vindex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set the tagging configuration for a member port
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    vlanIndex   vlan ID
* @param    interface   physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    mode        tagging mode
*                       (@b{  DOT1Q_UNTAGGED or 
*                             DOT1Q_TAGGED})
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
extern L7_RC_t usmDbVlanTaggedSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_uint32 interface, L7_uint32 mode);

extern L7_RC_t usmDbVlanTaggedGet(L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_uint32 interface, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets the next VLAN ID
*
* @param    UnitIndex   unit
* @param    vid         current VLAN ID
* @param    *nextvid    pointer to next vid 
*                       (@b{  1-4094,   or         
                             DOT1Q_ADMIN_VLAN})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbNextVlanGet(L7_uint32 UnitIndex, L7_uint32 vid, L7_uint32 *nextvid);

extern L7_RC_t usmDbNextVlanSet(L7_uint32 UnitIndex, L7_uint32 vid, L7_uint32 *nextvid);

/*********************************************************************
*
* @purpose  Get GVRP status for the switch
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        L7_ENABLE or L7_DISABLE
*
* @notes    nonde
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpGvrpEnableGet(L7_uint32 UnitIndex, L7_uint32 *val);
/*********************************************************************
*
* @purpose  Get GMRP status for the switch
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        L7_ENABLE or L7_DISABLE
*
* @notes    nonde
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpGmrpEnableGet(L7_uint32 UnitIndex, L7_uint32 *val);
/*********************************************************************
*
* @purpose  Enable or Disable GVRP for the switch
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    status      L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpGvrpEnableSet(L7_uint32 UnitIndex, L7_uint32 mode);
/*********************************************************************
*
* @purpose  Enable or Disable GMRP for the switch
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    status      L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpGmrpEnableSet(L7_uint32 UnitIndex, L7_uint32 mode);


/*********************************************************************
*
* @purpose  Get GARP Join Time
*
* @param    UnitIndex   unit
* @param    interface   interface number
* @param    *val        Time (in centiseconds)
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpJoinTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set GARP Join Time
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   interface number
* @param    tempInt     Time (in centiseconds)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpJoinTimeSet(L7_uint32 UnitIndex, L7_uint32 interface,L7_uint32 tempInt);

/*********************************************************************
*
* @purpose  Get GARP Leave All Time
*
* @param    UnitIndex   unit
* @param    interface   interface number
* @param    *val        Time (in centiseconds)
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpLeaveAllTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set GARP Leave All Time
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   interface number
* @param    tempInt     Time (in centiseconds)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpLeaveAllTimeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 tempInt);

/*********************************************************************
*
* @purpose  Get GARP Leave Time
*
* @param    UnitIndex   unit
* @param    interface   interface number
* @param    *val        Time (in centiseconds)
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpLeaveTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Set Leave Time
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   interface number
* @param    tempInt  Time (in centiseconds)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGarpLeaveTimeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 tempInt);


/*********************************************************************
*
* @purpose Determine if an interface has been GVRP enabled
*          
* @param   unitIndex  Unit Index
* @param   interface  interface number
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE  if intf doesn't exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbGarpisIntfGVRPenabled(L7_uint32 unitIndex, L7_uint32 interface);

/*********************************************************************
* @purpose  Get the GARP protocol related statistic
*
* @param    interface   interface number
* @param    counterType the type of statistic counter 
* @param    val         the statistic 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarily
*           a complex way.
*       
* @end
*********************************************************************/
L7_RC_t usmDbGarpStatisticGet(L7_uint32 interface, GARP_STAT_COUNTER_TYPE_t counterType, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the GARP protocol related Received packets statistic
*
* @param    interface   interface number
* @param    counterType the type of statistic counter
* @param    val         the statistic
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarily
*           a complex way.
*
* @end
*********************************************************************/
L7_RC_t usmDbGarpRxStatisticGet(L7_uint32 interface, L7_GARP_ATTRIBUTE_TYPE_t garpAttrType, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the GARP protocol related Transmit Packets statistic
*
* @param    interface   interface number
* @param    counterType the type of statistic counter
* @param    val         the statistic
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarily
*           a complex way.
*
* @end
*********************************************************************/
L7_RC_t usmDbGarpTxStatisticGet(L7_uint32 interface, L7_GARP_ATTRIBUTE_TYPE_t garpAttrType, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the GARP protocol related error statistic
*
* @param    interface   interface number
* @param    counterType the type of error statistic counter 
* @param    val         the statistic 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarily
*           a complex way.
*       
* @end
*********************************************************************/
L7_RC_t usmDbGarpErrorStatisticGet(L7_uint32 interface, GARP_ERROR_STAT_COUNTER_TYPE_t counterType, L7_uint32 *val);

/*********************************************************************
* @purpose  Clear the GARP protocol related statistics
*
* @param    interface   interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarily
*           a complex way.
*       
* @end
*********************************************************************/
L7_RC_t usmDbGarpStatisticsClear(L7_uint32 interface);

/*********************************************************************
* @purpose  Clear the GARP protocol related error statistics
*
* @param    interface   interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarily
*           a complex way.
*       
* @end
*********************************************************************/
L7_RC_t usmDbGarpErrorStatisticsClear(L7_uint32 interface);

/*********************************************************************
* @purpose  Get the value of vlan-creation-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (output) vlan-creation-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usmDbGvrpVlanCreationForbidGet(L7_uint32 interface, L7_BOOL *forbid);

/*********************************************************************
* @purpose  Set the value of vlan-creation-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (input) vlan-creation-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usmDbGvrpPortVlanCreationForbidSet(L7_uint32 interface, L7_BOOL forbid);

/*********************************************************************
* @purpose  Get the value of registration-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (output) VLAN registration-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usmDbGvrpPortRegistrationForbidGet(L7_uint32 interface, L7_BOOL *forbid);

/*********************************************************************
* @purpose  Set the value of registration-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (input) VLAN registration-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usmDbGvrpPortRegistrationForbidSet(L7_uint32 interface, L7_BOOL forbid);
#endif

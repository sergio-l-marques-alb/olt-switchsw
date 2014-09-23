/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename usmdb_garp.c
*
* @purpose Provide interface to API's for GARP component
*
* @component unitmgr
*
* @comments tba
*
* @create 2/27/2007
*
* @author akulkarni
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_mib_vlan_api.h"
#include "garp_dot1q_api.h"
#include "garpapi.h"



/*********************************************************************
*
* @purpose  get GARP Join time, in centiseconds
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    *val       returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortGarpJoinTimeGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  get GARP Leave time, in centiseconds
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortGarpLeaveTimeGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  get GARP LeaveAll time, in centiseconds
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortGarpLeaveAllTimeGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

/*********************************************************************
*
* @purpose  get the administrative state of GMRP operation on this port.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    enabled == 1
* @notes    disabled == 2
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortGmrpStatusGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortGmrpFailedRegistrationsGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortGmrpLastPduOriginGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_char8 *buf );

/*********************************************************************
*
* @purpose  Get the state of GVRP operation on this port.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qPortGvrpStatus ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qPortGvrpFailedRegistrations ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val );

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
L7_RC_t usmDbDot1qPortGvrpLastPduOrigin ( L7_uint32 UnitIndex, L7_uint32 BasePort, L7_uchar8 *buf );

/*********************************************************************
*
* @purpose  Get the administrative status requested by management for
*           GMRP
*
* @param    UnitIndex   Unit
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dGmrpStatusGet ( L7_uint32 UnitIndex, L7_int32 *val );

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
L7_RC_t usmDbDot1dGmrpStatusSet ( L7_uint32 UnitIndex, L7_int32 val );

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
L7_RC_t usmDbDot1qGvrpStatusGet ( L7_uint32 UnitIndex, L7_int32 *val );


/*********************************************************************
*
* @purpose  Set the administrative status for GVRP.
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
L7_RC_t usmDbDot1qGvrpStatusSet ( L7_uint32 UnitIndex, L7_int32 val );


/*********************************************************************
*
* @purpose  Get the number of recieved GVRP PDUs.
*
* @param    UnitIndex   Unit
* @param    interface   interface number
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbRxGvrpPdusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the number of trasmited GVRP PDUs.
*
* @param    UnitIndex   Unit
* @param    interface   interface number
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS     If the counter was successfully
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbTxGvrpPdusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

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
L7_RC_t usmDbQportsEnableGVRPGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);
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
L7_RC_t usmDbGarpGmrpPortEnableGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);

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
L7_RC_t usmDbGarpJoinTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

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
L7_RC_t usmDbGarpLeaveTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

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
L7_RC_t usmDbGarpLeaveAllTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

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
L7_RC_t usmDbQportsEnableGVRPSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 mode);

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
L7_RC_t usmDbGarpGvrpEnableSet(L7_uint32 UnitIndex, L7_uint32 mode);
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
L7_RC_t usmDbGarpGmrpEnableSet(L7_uint32 UnitIndex, L7_uint32 mode);
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
L7_RC_t usmDbGarpGmrpPortEnableSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 mode);
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
L7_RC_t usmDbGarpJoinTimeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 tempInt);

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
L7_RC_t usmDbGarpLeaveAllTimeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 tempInt);

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
L7_RC_t usmDbGarpLeaveTimeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 tempInt);

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
L7_RC_t usmDbGarpGvrpEnableGet(L7_uint32 UnitIndex, L7_uint32 *val);
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
L7_RC_t usmDbGarpGmrpEnableGet(L7_uint32 UnitIndex, L7_uint32 *val);
/*********************************************************************
*
* @purpose Determine if an interface has been GVRP enabled
*
* @param   UnitIndex  Unit Index
* @param   interface  interface number
*
* @returns L7_RC_t  L7_SUCCESS;
* @returns L7_RC_t  L7_FAILURE  if intf doesn't exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbGarpisIntfGVRPenabled(L7_uint32 UnitIndex, L7_uint32 interface);

/*********************************************************************
* @purpose Determine if an interface can support GARP
*
* @param  UnitIndex     Unit Index
* @param  intIfNum      Internal interface number
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes none
*
* @end
*********************************************************************/
L7_BOOL usmDbGarpIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);


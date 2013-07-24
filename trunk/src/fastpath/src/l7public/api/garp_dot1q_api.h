/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename garp_dot1q_api.h
*
* @purpose garp dot1q APIs
*
* @component dot1Q
*
* @comments none
*
* @create 08/14/2000
*
* @author wjacobs
* @end
*
**********************************************************************/

#ifndef _GARP_DOT1Q_API_H_
#define _GARP_DOT1Q_API_H_

#include "l7_common.h"
#include "dot1q_api.h"
#include    <nimapi.h>
#include    <dtlapi.h>


/*********************************************************************
* @purpose  Get GVRP status for the switch
*
* @param    mode    L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t garpGVRPEnableGet(L7_uint32 *mode);


/*********************************************************************
* @purpose  Get GVRP status for the interface
*
* @param    intIfNum    Internal Interface Numer
* @param    mode        L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL garpGVRPIntfEnableGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get GMRP status for the switch
*
* @param    mode    L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t garpGMRPEnableGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Get GMRP status for the interface
*
* @param    intIfNum    Internal Interface Numer
* @param    mode        L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL garpGMRPIntfEnableGet(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Enable or Disable GVRP for an interface
*
* @param    intf        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    status      L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpIntfGVRPModeSet(L7_uint32 intf, L7_uint32 status);

/*********************************************************************
* @purpose  Get the GVRP configuration for an interface
*
* @param    intf        physical or logical interface to be configured
* @param    status      L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpQportsGVRPCfgGet(L7_uint32 intf, L7_uint32 *status);

/*********************************************************************
* @purpose  Enable or Disable GMRP for an interface
*
* @param    intf        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    status      L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpIntfGMRPModeSet(L7_uint32 intf, L7_uint32 status);

/*********************************************************************
* @purpose  Get the GMRP configuration for an interface
*
* @param    intf        physical or logical interface to be configured
* @param    status      L7_ENABLE or L7_DISABLE
*
* @returns  status      L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpQportsGMRPCfgGet(L7_uint32 intf, L7_uint32 *status);

/*********************************************************************
* @purpose  Set GARP Join Time
*
* @param    time     Time (in centiseconds)
* @param    intreface  interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpJoinTimeSet(L7_uint32 intf, L7_uint32 time);

/*********************************************************************
* @purpose  Set Leave Join Time
*
* @param    time     Time (in centiseconds)
* @param    intreface  interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpLeaveTimeSet(L7_uint32 intf, L7_uint32 time);

/*********************************************************************
* @purpose  Set GARP Leave All Time
*
* @param    time     Time (in centiseconds)
* @param    intreface  interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpLeaveAllTimeSet(L7_uint32 intf, L7_uint32 time);

/*********************************************************************
* @purpose  Get GARP Join Time
*
* @param    time     Time (in centiseconds)
* @param    interface interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpJoinTimeGet(L7_uint32 UnitIndex, L7_uint32 intf, L7_uint32 *time);

/*********************************************************************
* @purpose  Get GARP Leave Time
*
* @param    time     Time (in centiseconds)
* @param    interface interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpLeaveTimeGet(L7_uint32 UnitIndex, L7_uint32 intf, L7_uint32 *time);

/*********************************************************************
* @purpose  Get GARP Leave All Time
*
* @param    time     Time (in centiseconds)
* @param    interface interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpLeaveAllTimeGet(L7_uint32 UnitIndex, L7_uint32 intf, L7_uint32 *time);

/*********************************************************************
* @purpose  Enable or Disable GVRP for the switch
*
* @param    status      L7_ENABLE  or L7_DISABLE
*

* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpGVRPModeSet(L7_uint32 status);

/*********************************************************************
* @purpose  Enable or Disable GMRP for the switch
*
* @param    status      L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
 L7_RC_t garpGMRPModeSet(L7_uint32 status);

/*********************************************************************
* @purpose  Get GMRP configuration for the switch
*
* @param    status      L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
#define garpGMRPCfgGet garpGMRPEnableGet 


/*********************************************************************
* @purpose  See if an interface type is of the right type for GARP
*
* @param    sysIntfType              @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL garpIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  See if an interface is of the right type for VLANs
*
* @param    intifNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL garpIsValidIntf(L7_uint32 intIfNum);

#endif

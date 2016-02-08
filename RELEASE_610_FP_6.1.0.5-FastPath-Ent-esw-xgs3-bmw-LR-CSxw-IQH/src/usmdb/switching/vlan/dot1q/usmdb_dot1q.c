/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename usmdb_dot1q.c
*
* @purpose Provide interface to hardware API's for BRIDGE and VLAN MIB components
*
* @component unitmgr
*
* @comments tba
*
* @create 10/10/2000
*
* @author cpverne
* @end
*
**********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_mfdb_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"
#include "osapi.h"

#include "dot1q_api.h"
#include "garp_dot1q_api.h"
#include "statsapi.h"
#include "usmdb_counters_api.h"
#include "garpapi.h"
#include "fdb_api.h"

#include "osapi_trace.h"
#include "osapi_support.h"

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortCapabilitiesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_char8 *buf )
{
  *buf = dot1dPortCapabilitiesGet(BasePort);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbDot1dPortDefaultUserPriorityGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  *val = dot1dPortDefaultUserPriorityGet(BasePort);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbDot1dPortNumTrafficClassesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  *val = dot1dPortNumTrafficClassesGet(BasePort);
  return L7_SUCCESS;
}


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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qPvid ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the acceptable frame type for a specific port.
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
* @notes    admitAll == 1
* @notes    admitOnlyVlanTagged == 2
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qPortAcceptableFrameTypes ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get VLAN ingress filtering status per port.
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
* @notes true == 1
* @notes false == 2
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qPortIngressFiltering ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return dot1qQportsIngressFilterGet(BasePort, val);
}


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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dTpHCPortEntryGet ( L7_uint32 UnitIndex, L7_int32 Port )
{
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  get the next transparnet high capacity port entry
*
* @param    UnitIndex   Unit
* @param    Port        port number
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
L7_RC_t dot1dTpHCPortEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *Port )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dTpHCPortInFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high, L7_uint32 *low )
{
  return usmDbStatGet64(UnitIndex, L7_CTR_RX_TOTAL_FRAMES, Port, high, low);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dTpHCPortInDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high, L7_uint32 *low )
{
  return usmDbStatGet64(UnitIndex, L7_CTR_POLICY_RX_TOTAL_DISCARDS, Port, high, low);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dTpHCPortOutFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high, L7_uint32 *low )
{
  return usmDbStatGet64(UnitIndex, L7_CTR_TX_TOTAL_FRAMES, Port, high, low);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dTpPortOverflowEntryGet ( L7_uint32 UnitIndex, L7_int32 Port )
{
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the net entry of most significant bits of statistics
*           counters for a high capacity interface of a transparent bridge
*
* @param    UnitIndex   Unit
* @param    Port   port number
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
L7_RC_t dot1dTpPortOverflowEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *Port )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dTpPortInOverflowFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high )
{
  L7_uint32 low;
  return usmDbStatGet64(UnitIndex, L7_CTR_RX_TOTAL_FRAMES, Port, high, &low);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dTpPortInOverflowDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high )
{
  L7_uint32 low;
  return usmDbStatGet64(UnitIndex, L7_CTR_POLICY_RX_TOTAL_DISCARDS, Port, high, &low);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dTpPortOutOverflowFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *high )
{
  L7_uint32 low;
  return usmDbStatGet64(UnitIndex, L7_CTR_TX_TOTAL_FRAMES, Port, high, &low);
}

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
L7_RC_t usmDbDot1dDeviceCapabilitiesGet ( L7_uint32 UnitIndex, L7_char8 *buf )
{
  *buf = (L7_char8)dot1dDeviceCapabilitiesGet();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get traffic class capability
*
* @param    UnitIndex   Unit
* @param    *val        returned status value
*
* @returns  L7_SUCCESS      If the value was successfully obtained
* @returns  L7_ERROR        error 
* @returns  L7_FAILURE      Other failure
*
* @notes    If successful, the *val parameter indicates L7_TRUE if
*           more than one priority level is enabled, or L7_FALSE if a 
*           single priority level is enabled.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dTrafficClassesEnabledGet ( L7_uint32 UnitIndex, L7_BOOL *val )
{
  return dot1dTrafficClassesEnabledGet(val);
}


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
L7_RC_t usmDbDot1dDeviceCapabilitiesSet ( L7_uint32 UnitIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot1dTrafficClassesEnabledSet ( L7_uint32 UnitIndex, L7_BOOL val )
{
  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return dot1dTrafficClassesEnabledSet(val);
}


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
* @returns  L7_FAILURE      Other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dUserPriorityRegenEntryGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 UserPriority )
{
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot1dUserPriorityRegenEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *BasePort, L7_int32 *UserPriority )
{
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot1dRegenUserPriorityGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 UserPriority, L7_int32 *val )
{
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot1dRegenUserPrioritySet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 UserPriority, L7_int32 val )
{
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot1dTrafficClassEntryGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 Priority )
{
  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ( (usmDbIntIfNumTypeCheckValid(UnitIndex, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, BasePort) == L7_SUCCESS) &&
       (Priority >= 0) && (Priority <= DOT1D_MAX_TRAFFIC_CLASS_PRIORITY)
     )
    return L7_SUCCESS;

  return L7_FAILURE;
}

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
L7_RC_t usmDbDot1dTrafficClassEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *BasePort, L7_int32 *Priority )
{
  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((BasePort == L7_NULLPTR) || (Priority == L7_NULLPTR))
    return L7_FAILURE;

  if (*BasePort == 0 && *Priority == 0)
    /*return usmDbGetNextPhysicalIntIfNumber(*BasePort, BasePort);*/
    return usmDbIntIfNumTypeFirstGet(UnitIndex, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, BasePort);

  if (*Priority >= 0 && *Priority < DOT1D_MAX_TRAFFIC_CLASS_PRIORITY)
  {
    (*Priority)++;
    return usmDbIntIfNumTypeCheckValid( UnitIndex,  USM_PHYSICAL_INTF | USM_LAG_INTF, 0, *BasePort);
    /* return usmDbPhysicalIntIfNumberCheck(UnitIndex, *BasePort); */
  }

  *Priority = 0;
  /* return usmDbGetNextPhysicalIntIfNumber(*BasePort, BasePort); */
  return usmDbIntIfNumTypeNextGet(UnitIndex, USM_PHYSICAL_INTF | USM_LAG_INTF, 0,*BasePort, BasePort);
}

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
L7_RC_t usmDbDot1dTrafficClassPerSwitchEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *Priority )
{
  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (Priority == L7_NULLPTR)
    return L7_FAILURE;

  if (*Priority >= 0 && *Priority < DOT1D_MAX_TRAFFIC_CLASS_PRIORITY)
  {
    (*Priority)++;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the User Priority to Traffic Class mapping.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    Priority   priority
* @param    *val       value to get
*
* @returns  L7_SUCCESS        If the value was successfully obtained
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dTrafficClassGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 Priority, L7_int32 *val )
{
  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((BasePort != L7_ALL_INTERFACES) &&
      (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) != L7_TRUE))
    return L7_NOT_SUPPORTED;

  return dot1dPortTrafficClassGet(BasePort, Priority, val);
}

/*********************************************************************
*
* @purpose  Get the User Priority to Traffic Class mapping.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    Priority   priority
* @param    *val       value to get
*
* @returns  L7_SUCCESS        If the value was successfully obtained
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dDefaultTrafficClassGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 Priority, L7_int32 *val )
{
  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((BasePort != L7_ALL_INTERFACES) &&
      (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) != L7_TRUE))
    return L7_NOT_SUPPORTED;

  return dot1dPortDefaultTrafficClassGet(BasePort, Priority, val);
}

/*********************************************************************
*
* @purpose  Set the User Priority to Traffic Class mapping.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    Priority   priority
* @param    val       value to set
*
* @returns  L7_SUCCESS        If the value was successfully set
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
* @returns  L7_FAILURE        Other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dTrafficClassSet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 Priority, L7_int32 val )
{
  L7_RC_t  rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (BasePort == L7_ALL_INTERFACES)
    rc = dot1dPortTrafficClassGlobalSet(Priority, val);

  else if (cnfgrIsFeaturePresent(L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
    rc = dot1dPortTrafficClassSet(BasePort, Priority, val);

  return rc;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortOutboundAccessPriorityEntryGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 RegenUserPriority )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortOutboundAccessPriorityEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *BasePort, L7_int32 *RegenUserPriority )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortOutboundAccessPriorityGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 RegenUserPriority, L7_int32 *val )
{
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot1qVlanVersionNumberGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  *val = dot1qVlanVersionGet();
  return L7_SUCCESS;
}

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
L7_RC_t usmDbDot1qMaxVlanIdGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  *val = dot1qMaxVlanIDGet();
  return L7_SUCCESS;
}

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
L7_RC_t usmDbDot1qMaxSupportedVlansGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  *val = dot1qMaxSupportedVlanGet();
  return L7_SUCCESS;
}

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
L7_RC_t usmDbDot1qNumVlansGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  *val = dot1qCurrentVlanCountGet();
  return L7_SUCCESS;
}


/* dot1qFdb */

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qFdbDynamicCountGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_uint32 *val )
{
  L7_ushort16 fdbId, curFdbId;
  usmdbFdbEntry_t fdbEntry;
  L7_uint32 count = 0;

  fdbId = (L7_ushort16)FdbId;
  bzero(fdbEntry.usmdbFdbMac, L7_FDB_KEY_SIZE);

  if (usmDbFDBEntryNextGet(UnitIndex, &fdbEntry) != L7_SUCCESS)
    return L7_FAILURE;

  do
  {
    curFdbId = *(L7_ushort16 *)&(fdbEntry.usmdbFdbMac[0]);
    if ( (fdbId == curFdbId) && (fdbEntry.usmdbFdbEntryType == L7_FDB_ADDR_FLAG_LEARNED) )
      count++;
  } while (usmDbFDBEntryNextGet(UnitIndex, &fdbEntry) == L7_SUCCESS);

  *val = count;
  return L7_SUCCESS;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpFdbEntryGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress )
{
  usmdbFdbEntry_t fdbEntry;
  L7_ushort16 fdbId;

  if (FdbAddress != NULL)
  {
    fdbId = osapiNtohs((L7_ushort16)FdbId);

    bzero(fdbEntry.usmdbFdbMac, L7_FDB_KEY_SIZE);
    memcpy(fdbEntry.usmdbFdbMac, &fdbId, L7_FDB_IVL_ID_LEN);
    memcpy(&fdbEntry.usmdbFdbMac[L7_FDB_IVL_ID_LEN], FdbAddress, L7_FDB_MAC_ADDR_LEN);

    return usmDbFDBEntryGet(UnitIndex, &fdbEntry);
  }

  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpFdbEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *FdbId, L7_char8 *FdbAddress )
{
  usmdbFdbEntry_t fdbEntry;
  L7_ushort16 fdbId;
  L7_RC_t rc;

  if (FdbAddress != NULL)
  {
    fdbId = osapiNtohs((L7_ushort16)(*FdbId));

    bzero(fdbEntry.usmdbFdbMac, L7_FDB_KEY_SIZE);
    memcpy(fdbEntry.usmdbFdbMac, &fdbId, L7_FDB_IVL_ID_LEN);
    memcpy(&fdbEntry.usmdbFdbMac[L7_FDB_IVL_ID_LEN], FdbAddress, L7_FDB_MAC_ADDR_LEN);

    rc = usmDbFDBEntryNextGet(UnitIndex, &fdbEntry);

    if (rc == L7_SUCCESS)
    {
      fdbId = *(L7_ushort16 *)&fdbEntry.usmdbFdbMac[0];

      *FdbId = (L7_uint32)(osapiHtons(fdbId));
      memcpy(FdbAddress, &fdbEntry.usmdbFdbMac[L7_FDB_IVL_ID_LEN], L7_FDB_MAC_ADDR_LEN);
    }

    return rc;
  }

  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpFdbPortGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress, L7_int32 *val )
{
  usmdbFdbEntry_t fdbEntry;
  L7_ushort16 fdbId;
  L7_RC_t rc;

  if (FdbAddress != NULL)
  {
    fdbId = osapiNtohs((L7_ushort16)FdbId);

    bzero(fdbEntry.usmdbFdbMac, L7_FDB_KEY_SIZE);
    memcpy(fdbEntry.usmdbFdbMac, &fdbId, L7_FDB_IVL_ID_LEN);
    memcpy(&fdbEntry.usmdbFdbMac[L7_FDB_IVL_ID_LEN], FdbAddress, L7_FDB_MAC_ADDR_LEN);

    rc = usmDbFDBEntryGet(UnitIndex, &fdbEntry);

    if (rc == L7_SUCCESS)
    {
      *val = fdbEntry.usmdbFdbIntIfNum;
    }

    return rc;
  }

  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
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
L7_RC_t usmDbDot1qTpFdbStatusGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *FdbAddress, L7_int32 *val )
{
  usmdbFdbEntry_t fdbEntry;
  L7_ushort16 fdbId;
  L7_RC_t rc;

  if (FdbAddress != NULL)
  {
    fdbId = osapiNtohs((L7_ushort16)FdbId);

    bzero(fdbEntry.usmdbFdbMac, L7_FDB_KEY_SIZE);
    memcpy(fdbEntry.usmdbFdbMac, &fdbId, L7_FDB_IVL_ID_LEN);
    memcpy(&fdbEntry.usmdbFdbMac[L7_FDB_IVL_ID_LEN], FdbAddress, L7_FDB_MAC_ADDR_LEN);

    rc = usmDbFDBEntryGet(UnitIndex, &fdbEntry);

    if (rc == L7_SUCCESS)
    {
      *val = fdbEntry.usmdbFdbEntryType;
    }

    return rc;
  }

  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpGroupEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *GroupAddress )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];
  L7_INTF_MASK_t absMask;

  if (usmDbEntryVidMacCombine(VlanIndex, GroupAddress, vidMac) == L7_SUCCESS &&
      usmDbMfdbEntryFwdIntfGetNext(UnitIndex, vidMac, L7_MATCH_EXACT, &absMask) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpGroupEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex, L7_char8 *GroupAddress )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];
  L7_INTF_MASK_t absMask;

  if (usmDbEntryVidMacCombine(*VlanIndex, GroupAddress, vidMac) == L7_SUCCESS &&
      usmDbMfdbEntryFwdIntfGetNext(UnitIndex, vidMac, L7_MATCH_GETNEXT, &absMask) == L7_SUCCESS &&
      usmDbEntryVidMacSeparate(vidMac, VlanIndex, GroupAddress) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpGroupEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *GroupAddress, L7_char8 *buf, L7_uint32 *buf_length )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];
  L7_INTF_MASK_t absMask;

  if (usmDbEntryVidMacCombine(VlanIndex, GroupAddress, vidMac) == L7_SUCCESS &&
      usmDbMfdbEntryFwdIntfGetNext(UnitIndex, vidMac, L7_MATCH_EXACT, &absMask) == L7_SUCCESS &&
      usmDbReverseMask(absMask, &absMask) == L7_SUCCESS)
  {
    memcpy(buf, absMask.value, L7_INTF_INDICES);
    *buf_length = L7_INTF_INDICES;
    rc = L7_SUCCESS;
  }

  return rc;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpGroupLearntGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *GroupAddress, L7_char8 *buf, L7_uint32 *buf_length )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];
  L7_INTF_MASK_t absMask;

  if (usmDbEntryVidMacCombine(VlanIndex, GroupAddress, vidMac) == L7_SUCCESS &&
      usmDbMfdbEntryLearntIntfGetNext(UnitIndex, vidMac, L7_MATCH_EXACT, &absMask) == L7_SUCCESS &&
      usmDbReverseMask(absMask, &absMask) == L7_SUCCESS)
  {
    memcpy(buf, absMask.value, L7_INTF_INDICES);
    *buf_length = L7_INTF_INDICES;
    rc = L7_SUCCESS;
  }

  return rc;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardAllEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardAllEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardAllPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardAllStaticPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDdot1qForwardAllForbiddenPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardAllPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardAllStaticPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDdot1qForwardAllForbiddenPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardUnregisteredEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardUnregisteredEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardUnregisteredPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardUnregisteredStaticPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDdot1qForwardUnregisteredForbiddenPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardUnregisteredPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qForwardUnregisteredStaticPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDdot1qForwardUnregisteredForbiddenPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticUnicastEntryGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticUnicastEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *FdbId, L7_char8 *Address, L7_int32 *ReceivePort )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticUnicastAllowedToGoToGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
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
L7_RC_t usmDbDot1qStaticUnicastStatusGet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 *val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticUnicastAllowedToGoToSet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
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
L7_RC_t usmDbDot1qStaticUnicastStatusSet ( L7_uint32 UnitIndex, L7_uint32 FdbId, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticMulticastEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticMulticastEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex, L7_char8 *Address, L7_int32 *ReceivePort )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticMulticastStaticEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticMulticastForbiddenEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
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
L7_RC_t usmDbDot1qStaticMulticastStatusGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 *val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticMulticastStaticEgressPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticMulticastForbiddenEgressPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_char8 *buf )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qStaticMulticastStatusSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *Address, L7_int32 ReceivePort, L7_int32 *val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanNumDeletesGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  return dot1qVlanNumDeletesGet(val);
}

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
L7_RC_t usmDbDot1qNextFreeLocalVlanIndexGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  *val = dot1qNextFreeLocalVlanIndexGet();
  return L7_SUCCESS;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qConstraintSetDefaultGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qConstraintTypeDefaultGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qConstraintSetDefaultSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qConstraintTypeDefaultSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  return L7_FAILURE;
}

/* dot1qVlanCurrent */
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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanCurrentEntryGet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex )
{
  return dot1qVlanCheckValid(VlanIndex);
}

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
L7_RC_t usmDbDot1qVlanCurrentEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *TimeMark, L7_uint32 *VlanIndex )
{
  return dot1qNextVlanGet(*VlanIndex, VlanIndex);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanFdbIdGet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_uint32 *val )
{
  return fdbFdbIDGet(VlanIndex, val);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanCurrentEgressPortsGet(L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *size)
{
  return dot1qVlanCurrentEgressPortsGet(VlanIndex, (NIM_INTF_MASK_t*)buf, size);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanCurrentUntaggedPortsGet ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *size)
{
  return dot1qVlanCurrentUntaggedPortsGet(VlanIndex, (NIM_INTF_MASK_t*)buf, size);
}

/*********************************************************************
*
* @purpose  Get the status of a vlan entry.
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
L7_RC_t usmDbDot1qVlanStatus ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, L7_int32 *val )
{
  return dot1qVlanStatusGet(VlanIndex, val);
}

/*********************************************************************
*
* @purpose  Get the value of sysUpTime when this VLAN was created.
*
* @param    UnitIndex   Unit
* @param    TimeMark    time mark
* @param    VlanIndex   Vlan Index
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
L7_RC_t usmDbDot1qVlanCreationTime ( L7_uint32 UnitIndex, L7_uint32 TimeMark, L7_uint32 VlanIndex, usmDbTimeSpec_t *time)
{
  return dot1qOperVlanCreationTimeGet(VlanIndex, (L7_timespec*) time);
}

/* dot1qVlanStatic */
/*********************************************************************
*
* @purpose  Check the VID provided, return L7_SUCCESS if it is valid
*
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index
*
* @returns  L7_SUCCESS
*     
* @returns  L7_ERROR
* @returns  L7_FAILURE
*             
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanStaticEntryGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex )
{
  L7_uint32 val;

  if ( dot1qVlanCheckValid(VlanIndex) == L7_SUCCESS )
  {
    if (dot1qOperVlanTypeGet(VlanIndex, &val) == L7_SUCCESS && val != L7_DOT1Q_DYNAMIC && 
        val != L7_DOT1Q_WS_DYNAMIC)
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot1qVlanMemberMaskSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 mode ,DOT1Q_SWPORT_MODE_t swport_mode)
{
  return dot1qVlanMemberMaskSet(VlanIndex, (NIM_INTF_MASK_t*)buf, mode, swport_mode);
}

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
L7_RC_t usmDbDot1qVlanTaggedMemberMaskSet (L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf,L7_uint32 tagMode )
{
  return dot1qVlanTaggedMemberMaskSet(VlanIndex, (NIM_INTF_MASK_t*)buf, tagMode);
}

/*********************************************************************
*
* @purpose  Get the next  Static information entry for a VLAN configured
*           into the device by (local or network) management.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index
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
L7_RC_t usmDbDot1qVlanStaticEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *VlanIndex )
{
  return dot1qNextStaticVlanGet(*VlanIndex, VlanIndex);
}

/*********************************************************************
*
* @purpose  Get the administratively assigned string used
*           to identify the VLAN.
*
* @param    UnitIndex   Unit
* @param    VlanIndex   vlan index
* @param    *buf        buffer
*
* @returns  L7_SUCCESS
*     
* @returns  L7_ERROR
* @returns  L7_FAILURE
*             
*
* @notes    Storage must be allocated by the caller (name)
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanStaticNameGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return dot1qVlanNameGet(VlanIndex, buf);
}

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
L7_RC_t usmDbDot1qVlanStaticEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *length )
{
  return dot1qVlanStaticEgressPortsGet(VlanIndex, (NIM_INTF_MASK_t*)buf, length);
}

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
L7_RC_t usmDbDot1qVlanForbiddenEgressPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *length )
{
  return dot1qVlanForbiddenEgressPortsGet(VlanIndex, (NIM_INTF_MASK_t*)buf, length);
}

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
L7_RC_t usmDbDot1qVlanStaticUntaggedPortsGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf, L7_uint32 *length )
{
  return dot1qVlanStaticUntaggedPortsGet(VlanIndex, (NIM_INTF_MASK_t*)buf, length);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    " This object indicates the status of this entry."
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanStaticRowStatusGet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_int32 *val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanStaticNameSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return dot1qVlanNameSet(VlanIndex, buf);
}

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
L7_RC_t usmDbDot1qVlanStaticEgressPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return dot1qVlanStaticEgressPortsSet(VlanIndex, (NIM_INTF_MASK_t*)buf);
}

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
L7_RC_t usmDbDot1qVlanForbiddenEgressPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return dot1qVlanForbiddenEgressPortsSet(VlanIndex, (NIM_INTF_MASK_t*)buf);
}

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
L7_RC_t usmDbDot1qVlanStaticUntaggedPortsSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_char8 *buf )
{
  return dot1qVlanStaticUntaggedPortsSet(VlanIndex, (NIM_INTF_MASK_t*)buf);
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    " This object indicates the status of this entry."
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qVlanStaticRowStatusSet ( L7_uint32 UnitIndex, L7_uint32 VlanIndex, L7_int32 *val )
{
  return L7_FAILURE;
}

/* dot1qPortVlanStatistics */
/*********************************************************************
*
* @purpose  Get a Traffic statistics entry for a VLAN on an interface.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    VlanIndex  vlan index
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
L7_RC_t usmDbDot1qPortVlanStatisticsEntryGet(L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose  Get the next Traffic statistics entry for a VLAN on an interface.
*
* @param    UnitIndex   Unit
* @param    BasePort   interface number
* @param    VlanIndex  vlan index
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
L7_RC_t usmDbDot1qPortVlanStatisticsEntryNextGet(L7_uint32 UnitIndex, L7_int32 *BasePort, L7_uint32 *VlanIndex)
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpVlanPortInFramesGet(L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val)
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    initially use hardware counters for vlan in and out hardware
*           counters for FE ports
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpVlanPortOutFramesGet(L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val)
{
  return L7_NOT_SUPPORTED;

}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpVlanPortInDiscardsGet(L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val)
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpVlanPortInOverflowFramesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val )
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpVlanPortOutOverflowFramesGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val )
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpVlanPortInOverflowDiscardsGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *val )
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qPortVlanHCStatisticsEntryGet(L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex)
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qPortVlanHCStatisticsEntryNextGet(L7_uint32 UnitIndex, L7_int32 *BasePort, L7_uint32 *VlanIndex)
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpVlanPortHCInFramesGet(L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *big_end, L7_uint32 *little_end)
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpVlanPortHCOutFramesGet(L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *big_end, L7_uint32 *little_end)
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qTpVlanPortHCInDiscardsGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_uint32 VlanIndex, L7_uint32 *big_end, L7_uint32 *little_end )
{
  return L7_NOT_SUPPORTED;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qLearningConstraintsEntryGet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set )
{
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the next learning constraint defined for a VLAN.
*
* @param    UnitIndex   Unit
* @param    Vlan        vlan
* @param    *set         set
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
L7_RC_t usmDbDot1qLearningConstraintsEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *Vlan, L7_int32 *Set )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
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
L7_RC_t usmDbDot1qConstraintTypeGet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set, L7_int32 *val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qConstraintStatusGet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set, L7_int32 *val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
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
L7_RC_t usmDbDot1qConstraintTypeSet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set, L7_int32 val )
{
  return L7_FAILURE;
}

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
*                     found and stored in the space provided
* @returns  L7_ERROR      If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                 values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1qConstraintStatusSet ( L7_uint32 UnitIndex, L7_uint32 Vlan, L7_int32 Set, L7_int32 val )
{
  return L7_FAILURE;
}

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
L7_BOOL usmDbDot1qIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t ifType;

  if (nimGetIntfType(intIfNum, &ifType) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  return dot1qIntfIsValidType(ifType);
}

/*********************************************************************
*
* @purpose  Reset VLANS to factory defaults.
*
* @param    UnitIndex   Unit
* @param    val        returned value of counter
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
L7_RC_t usmDbVlanResetToDefaultSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return dot1qVlanCfgClear();
}

/*********************************************************************
*
* @purpose  Returns the number of discards due to vlan mismatches
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
L7_RC_t usmDbTxVlanMismatchDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_ERROR;
}


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
L7_RC_t usmDbRxMiscDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_ERROR;
}

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
L7_RC_t usmDbRxVlanMismatchDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_ERROR;
}


/*********************************************************************
*
* @purpose  Get the max VLAN entries allowed.
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
L7_RC_t usmDbMostVlanEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = dot1qMostVlanCountGet();
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Get the number of static VLANs.
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
L7_RC_t usmDbStaticVlanEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_uint32 vid, nextvid;
  L7_uint32 count=0;

  vid = 0;
  while (dot1qNextStaticVlanGet(vid,&nextvid) != L7_FAILURE)
  {
    vid = nextvid;
    count++;
  }
  *val = count;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the number of dynamic VLANs.
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
L7_RC_t usmDbDynamicVlanEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_uint32 vid, nextvid;
  L7_uint32 count=0;

  vid = 0;
  while (dot1qNextDynamicVlanGet(vid,&nextvid) != L7_FAILURE)
  {
    vid = nextvid;
    count++;
  }
  *val = count;
  return L7_SUCCESS;
}

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
L7_RC_t usmDbVlanIDGet(L7_uint32 UnitIndex, L7_uint32 index)
{
  return dot1qVlanQueueSyncCheckValid(index);
}

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
L7_RC_t usmDbVlanIdFromNameGet(const L7_char8 *vlanName, L7_uint32 *vlanId)
{
  return dot1qVlanIdFromNameGet(vlanName, vlanId);
}

/*********************************************************************
*
* @purpose  Gets the next VLAN ID
*
* @param    UnitIndex   unit
* @param    vid         current VLAN ID
* @param    *nextvid    pointer to next vid
*                       (@b{  1-4094,   or
                             ADMIN_VLAN})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbNextVlanGet(L7_uint32 UnitIndex, L7_uint32 vid, L7_uint32 *nextvid)
{
  return dot1qNextVlanGet(vid, nextvid);
}

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
L7_RC_t usmDbVlanNameGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  return dot1qVlanNameGet(index, buf);
}

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
L7_RC_t usmDbVlanTypeGet(L7_uint32 UnitIndex, L7_uint32 vid, L7_uint32 *val)
{
  return dot1qOperVlanTypeGet(vid, val);
}

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
L7_RC_t usmDbVlanMemberSetGet(L7_uint32 UnitIndex, L7_uint32 vindex, L7_uint32 index, L7_uint32 *val)
{
  return dot1qVlanMemberGet(vindex, index, val);
}

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
L7_RC_t usmDbOperVlanMemberSetGet(L7_uint32 UnitIndex, L7_uint32 vindex, L7_uint32 index, L7_uint32 *val)
{
  return dot1qOperVlanMemberGet(vindex, index, val);
}

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
L7_RC_t usmDbVlanTaggedSetGet(L7_uint32 UnitIndex, L7_uint32 vindex, L7_uint32 index, L7_uint32 *val)
{
  return dot1qVlanTaggedMemberGet(vindex, index, val);
}

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
 L7_RC_t usmDbDot1qOperVlanTaggedMemberGet(L7_uint32 UnitIndex, L7_uint32 vindex, L7_uint32 index, L7_uint32 *val)
 {
   return dot1qOperVlanTaggedMemberGet(vindex,index,val);
 }

/*********************************************************************
*
* @purpose  Get the VID configuration for a port
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
L7_RC_t usmDbQportsPVIDGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  return dot1qQportsVIDGet(index, L7_DOT1Q_DEFAULT_VID_INDEX, val);
}


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
L7_RC_t usmDbQportsCurrentPVIDGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  return dot1qQportsCurrentVIDGet(index, L7_DOT1Q_DEFAULT_VID_INDEX, val);
}


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
L7_RC_t usmDbQportsAcceptFrameTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  return dot1qQportsAcceptFrameTypeGet(index, val);
}

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
L7_RC_t usmDbQportsEnableIngressFilteringGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  return dot1qQportsIngressFilterGet(index, val);
}

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
L7_RC_t usmDbQportsCurrentIngressFilteringGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  return dot1qQportsCurrentIngressFilterGet(index, val);
}



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
L7_RC_t usmDbVlanCreate(L7_uint32 UnitIndex, L7_uint32 vlanID)
{
  L7_RC_t rc;

  OSAPI_TRACE_TRIGGER (L7_TRACE_TRIGGER_USMDB_VLAN_ADD_ENTER);
  OSAPI_TRACE_EVENT (L7_TRACE_EVENT_USMDB_VLAN_ADD_ENTER,
                     (L7_uchar8 *)&vlanID, sizeof (vlanID));

  rc = dot1qVlanCreate(vlanID, DOT1Q_ADMIN);

  OSAPI_TRACE_EVENT (L7_TRACE_EVENT_USMDB_VLAN_ADD_EXIT, (L7_uchar8 *)&rc, sizeof(rc));

  return rc;
}

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
L7_RC_t usmDbVlanDelete(L7_uint32 UnitIndex, L7_uint32 val)
{
  if(val == L7_DOT1Q_DEFAULT_VLAN)
    return L7_FAILURE;
  else
    return dot1qVlanDelete(val, DOT1Q_ADMIN);
}

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
L7_RC_t usmDbVlanNameSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_char8 *buf)
{
  return dot1qVlanNameSet(vlanIndex, buf);
}

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
L7_RC_t usmDbVlanMakeStaticSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex)
{
  return dot1qDynamicToStaticVlanConvert(vlanIndex);
}

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
L7_RC_t usmDbVlanMemberSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_uint32 interface, L7_uint32 val,DOT1Q_SWPORT_MODE_t swport_mode)
{
  L7_RC_t rc;

  if (interface != L7_ALL_INTERFACES)
  {
  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;
  }
  return dot1qVlanMemberSet(vlanIndex, interface, val, DOT1Q_ADMIN,swport_mode);
}

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
L7_RC_t usmDbVlanTaggedSet(L7_uint32 UnitIndex, L7_uint32 vlanIndex, L7_uint32 interface, L7_uint32 mode)
{
  L7_RC_t rc;

  if (interface != L7_ALL_INTERFACES)
  {
  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;
  }
  return dot1qVlanTaggedMemberSet(vlanIndex, interface, mode);
}

/*********************************************************************
* @purpose  Find the first VLAN ID (lowest numerical ID) claimed for internal use.
*
* @param    vlanId          vlanId 
* @param    intIfNum        corresponding interface
*
* @returns  L7_SUCCESS if a VLAN ID found
* @returns  L7_FAILURE if no VLAN IDs used internally                         
*
* @notes    Not very efficient.
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbInternalVlanFindFirst(L7_uint32 *vlanId, L7_uint32 *intIfNum)
{
  return dot1qInternalVlanFindFirst(vlanId, intIfNum);
}

/*********************************************************************
* @purpose  Find the next VLAN ID (in numeric order) claimed for internal use.
*
* @param    prevVid      previous VLAN ID
* @param    vlanId       next vlanId 
* @param    intIfNum     interface corresponding to next VLAN ID
*
* @returns  L7_SUCCESS if next VLAN ID found
* @returns  L7_FAILURE if no more VLAN IDs used internally                         
*
* @notes    Not very efficient.
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbInternalVlanFindNext(L7_uint32 prevVid, L7_uint32 *vlanId, L7_uint32 *intIfNum)
{
  return dot1qInternalVlanFindNext(prevVid, vlanId, intIfNum);
}

/*********************************************************************
* @purpose  Get the routing interface associated with given Internal VLAN ID.
*
* @param    vlanId VLAN ID
*
* @returns  L7_SUCCESS Routing Interface
* @returns  L7_FAILURE                          
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_uint32 usmDbInternalVlanRoutingInterfaceGet(L7_uint32 vlanId)
{
  return dot1qInternalVlanRoutingInterfaceGet(vlanId);
}

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
L7_RC_t usmDbQportsPVIDSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 vlanID)
{
  L7_RC_t rc;

  if (interface != L7_ALL_INTERFACES)
  {
  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;
  }
  return dot1qQportsVIDSet(interface, vlanID, L7_DOT1Q_DEFAULT_VID_INDEX,DOT1Q_ADMIN);
}

/*********************************************************************
*
* @purpose  Set the Acceptable Frame Type configuration for a port
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    type        L7_DOT1Q_ADMIT_ALL, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbQportsAcceptFrameTypeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 type)
{
  L7_RC_t rc;

  if (interface != L7_ALL_INTERFACES)
  {
  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;
  }
  return dot1qQportsAcceptFrameTypeSet(interface, type);
}

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
L7_RC_t usmDbQportsEnableIngressFilteringSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 mode)
{
  L7_RC_t rc;

  if (interface != L7_ALL_INTERFACES)
  {
  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;
  }
  return dot1qQportsIngressFilterSet(interface,mode,DOT1Q_ADMIN);
}


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
*********************************************************************/
L7_RC_t usmDbDot1dPortDefaultUserPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 priority)
{
  L7_RC_t   rc = L7_FAILURE;

  if (intIfNum == L7_ALL_INTERFACES)
    rc = dot1dPortDefaultUserPriorityGlobalSet(priority);

  else
    rc = dot1dPortDefaultUserPrioritySet(intIfNum, priority);

  return rc;
}

/*********************************************************************
* @purpose  Set dot1d number of traffic classes
*
* @param    UnitIndex            the unit for this operation
* @param    intIfNum             Internal Interface Number
* @param    numTrafficClasses    number of traffic classes, from 1-8
*
* @returns  L7_NOT_SUPPORTED  This feature is not supported 
*
* @notes    This API is no longer supported.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1dPortNumTrafficClassesSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 numTrafficClasses)
{
  /* Setting the number of traffic classes via USMDB is no longer supported.
   * This specifies how many port egress queues are available, which 
   * is not normally a user configurable item.  Allowing the number
   * of traffic classes to change during run time can cause unintended
   * side effects for some of the mapping tables and QOS configuration 
   * parameters.
   */
  return L7_NOT_SUPPORTED;
}

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
L7_RC_t usmDbUnacceptableFrameDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_ERROR;
}


/*********************************************************************
*
* @purpose Determine if an interface has been acquired by a LAG
*
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *status    L7_TRUE: acquired, L7_FALSE: released
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE  if intf doesn't exist
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIntfAquiredGet(L7_uint32 UnitIndex, L7_uint32 intf, L7_uint32 *status)
{
  return dot1qIntfAcquiredGet(intf, status);
}

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
L7_RC_t usmDbIntfAcquiredCompGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_COMPONENT_IDS_t *compId)
{
  return dot1qIntfAcquiredCompGet(intIfNum, compId);
}

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
L7_RC_t usmDbVlanVIDListCfgGet(L7_uint32 unitIndex, L7_uint32 intfNum, L7_uint32 *data, L7_uint32 *numOfElements)
{
  return dot1qVIDListCfgGet(intfNum, 0, data, numOfElements);
}

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
L7_RC_t usmDbVlanVIDListGet(L7_uint32 unitIndex, L7_uint32 intfNum, L7_uint32 *data, L7_uint32 *numOfElements)
{
  return dot1qVIDListGet(intfNum, 0, data, numOfElements);
}

/*********************************************************************
* @purpose  Check to see if the port is the appropriate type on which
*     the parm can be set
*
* @param    UnitIndex           Unit Index
* @param  intIfNum      Internal interface number
*
* @returns  L7_SUCCESS      The parm can be set on this port type
* @returns  L7_NOT_SUPPORTED  The port does not support this parm to be set
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbQportsPortParmCanSet(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t ifType;

  if (nimGetIntfType(intIfNum,&ifType) == L7_SUCCESS)
  {
    if (dot1qIntfIsValidType(ifType) != L7_TRUE)
    {
      return L7_NOT_SUPPORTED;
    }
  }
  return L7_SUCCESS;
}
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
                                         DOT1Q_SWPORT_MODE_t swport_mode)
{
  return dot1qQportsVlanMembershipMaskSet(intIfNum, vidMask, mode, swport_mode);
}
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
								  L7_uint32 tagMode)
{
  return dot1qQportsVlanTagMaskSet(intIfNum, vidMask, tagMode);
}
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
L7_RC_t usmDbDot1qSwPortModeSet(L7_uint32 unit, NIM_INTF_MASK_t *mask, L7_uint32 mode)
{
  return dot1qSwitchPortModeSet(mask, mode, DOT1Q_ADMIN);
}
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
L7_RC_t usmDbDot1qSwPortAccessVlanSet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 vlanId)
{
  return dot1qSwitchPortAccessVlanSet(intIfNum, vlanId, DOT1Q_ADMIN);
}
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
L7_RC_t usmDbDot1qSwPortModeGet(L7_uint32 unit,L7_uint32 interface, L7_uint32 *mode)
{
    return dot1qSwitchPortModeGet(interface, mode);
}

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
L7_RC_t usmDbDot1qSwPortAccessVlanGet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32* vlanId)
{
  return dot1qSwitchPortAccessVlanGet(intIfNum, vlanId);
}

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
L7_BOOL usmDbDot1qIsAccessVlanForAnyPort(L7_uint32 unit, L7_uint32 vlanId)
{
  return dot1qIsAccessVlanForAnyPort(vlanId );
}

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
L7_RC_t usmDbVlanCreateMaskSet(L7_uint32 unitIndex, L7_VLAN_MASK_t *vidMask)
{
  return dot1qVlanCreateMask(vidMask);
}
/*********************************************************************
* @purpose  Delete a range of vlans
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
L7_RC_t usmDbVlanDeleteMaskSet(L7_uint32 unitIndex, L7_VLAN_MASK_t *vidMask)
{
  return dot1qVlanDeleteMask(vidMask);
}


/*********************************************************************
* @purpose  Is a Valid Vlan
*
* @param    unitIndex   Unit Number
* @param    vid     Vlan ID 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    
*
*
* @end
*********************************************************************/
L7_RC_t usmDbVlanIsValid(L7_uint32 unitIndex, L7_uint32 vid)
{
  return dot1qVlanCheckValid(vid);
}


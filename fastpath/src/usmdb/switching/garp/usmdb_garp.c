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
#include "usmdb_common.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_garp.h"
#include "usmdb_mib_vlan_api.h"
#include "garp_dot1q_api.h"
#include "garpapi.h"
#include "usmdb_counters_api.h"




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
L7_RC_t usmDbDot1dPortGarpJoinTimeGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return garpJoinTimeGet(UnitIndex,BasePort,val);
}

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
L7_RC_t usmDbDot1dPortGarpLeaveTimeGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return garpLeaveTimeGet(UnitIndex,BasePort,val);
}

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
L7_RC_t usmDbDot1dPortGarpLeaveAllTimeGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return garpLeaveAllTimeGet(UnitIndex,BasePort,val);
}

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
L7_RC_t usmDbDot1dPortGmrpStatusGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return garpQportsGMRPCfgGet(BasePort,val);
}

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
L7_RC_t usmDbDot1dPortGmrpFailedRegistrationsGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_GMRP_FAILED_REGISTRATIONS, BasePort, val);
}

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
L7_RC_t usmDbDot1dPortGmrpLastPduOriginGet ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_char8 *buf )
{
  /*do i need to check for port gmrp enable?*/
  return garpPortGmrpLastPduOriginGet(BasePort,buf);
}

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
L7_RC_t usmDbDot1qPortGvrpStatus ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return garpQportsGVRPCfgGet(BasePort, val);
}

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
L7_RC_t usmDbDot1qPortGvrpFailedRegistrations ( L7_uint32 UnitIndex, L7_int32 BasePort, L7_int32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_GVRP_FAILED_REGISTRATIONS, BasePort, val);
}

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
L7_RC_t usmDbDot1qPortGvrpLastPduOrigin ( L7_uint32 UnitIndex, L7_uint32 BasePort, L7_uchar8 *buf )
{
  if (usmDbGarpisIntfGVRPenabled(UnitIndex, BasePort) == L7_SUCCESS)
  {
    return garpPortGvrpLastPduOriginGet(BasePort, buf);
  }
  else
  {
    bzero(buf, L7_MAC_ADDR_LEN);
    return L7_SUCCESS;
  }
}

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
L7_RC_t usmDbDot1dGmrpStatusGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  return garpGMRPCfgGet(val);
}

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
L7_RC_t usmDbDot1dGmrpStatusSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  return garpGMRPModeSet(val);
}

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
L7_RC_t usmDbDot1qGvrpStatusGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  return garpGVRPEnableGet(val);
}


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
L7_RC_t usmDbDot1qGvrpStatusSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  return garpGVRPModeSet(val);
}



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
L7_RC_t usmDbRxGvrpPdusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_ERROR;
}

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
L7_RC_t usmDbTxGvrpPdusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_ERROR;
}

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
L7_RC_t usmDbQportsEnableGVRPGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  return garpQportsGVRPCfgGet(index, val);
}
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
L7_RC_t usmDbGarpGmrpPortEnableGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  return garpQportsGMRPCfgGet(index, val);
}

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
L7_RC_t usmDbGarpJoinTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return garpJoinTimeGet(UnitIndex,interface,val);
}

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
L7_RC_t usmDbGarpLeaveTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return garpLeaveTimeGet(UnitIndex,interface,val);
}

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
L7_RC_t usmDbGarpLeaveAllTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return garpLeaveAllTimeGet(UnitIndex,interface,val);
}

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
L7_RC_t usmDbQportsEnableGVRPSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 mode)
{
  L7_RC_t rc;

  if (interface != L7_ALL_INTERFACES)
  {
  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;
  }
  return garpIntfGVRPModeSet(interface, mode);
}

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
L7_RC_t usmDbGarpGvrpEnableSet(L7_uint32 UnitIndex, L7_uint32 mode)
{
  return garpGVRPModeSet(mode);
}
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
L7_RC_t usmDbGarpGmrpEnableSet(L7_uint32 UnitIndex, L7_uint32 mode)
{
  return garpGMRPModeSet(mode);
}
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
L7_RC_t usmDbGarpGmrpPortEnableSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 mode)
{
  L7_RC_t rc;

  if (interface != L7_ALL_INTERFACES)
  {
  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;
  }
  return garpIntfGMRPModeSet(interface, mode);
}
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
L7_RC_t usmDbGarpJoinTimeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 tempInt)
{
  L7_RC_t rc;

  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;

  if (/*(usmDbGarpisIntfGVRPenabled(interface) == L7_FAILURE) &&*/ (tempInt == 0))
  {
    return L7_SUCCESS;
  }
  else if (tempInt < 10 || tempInt > 100)
  {
    return L7_FAILURE;
  }
  else return garpJoinTimeSet(interface,tempInt);

}

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
L7_RC_t usmDbGarpLeaveAllTimeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 tempInt)
{
  L7_RC_t rc;

  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;

  if (/*(usmDbGarpisIntfGVRPenabled(interface) == L7_FAILURE) &&*/ (tempInt == 0))
  {
    return L7_SUCCESS;
  }
  else if ((tempInt < 200) || (tempInt > 6000))
  {
    return L7_FAILURE;
  }
  else return garpLeaveAllTimeSet(interface,tempInt);
}

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
L7_RC_t usmDbGarpLeaveTimeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 tempInt)
{
  L7_RC_t rc;

  rc = usmDbQportsPortParmCanSet(UnitIndex, interface);
  if (rc != L7_SUCCESS)
    return rc;

  if (/*(usmDbGarpisIntfGVRPenabled(interface) == L7_FAILURE) &&*/ (tempInt == 0))
  {
    return L7_SUCCESS;
  }
  else if (tempInt < 20 || tempInt > 600)
  {
    return L7_FAILURE;
  }
  else return garpLeaveTimeSet(interface,tempInt);
}

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
L7_RC_t usmDbGarpGvrpEnableGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return garpGVRPEnableGet(val);
}
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
L7_RC_t usmDbGarpGmrpEnableGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return garpGMRPEnableGet(val);
}
/*********************************************************************
*
* @purpose Determine if an interface has been GVRP enabled
*
* @param   UnitIndex  Unit Index
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
L7_RC_t usmDbGarpisIntfGVRPenabled(L7_uint32 UnitIndex, L7_uint32 interface)
{
  L7_uint32 status;
  L7_RC_t   returnVal = L7_FAILURE;

  garpQportsGVRPCfgGet(interface, &status);
  if (status == L7_ENABLE)
  {
    returnVal =  L7_SUCCESS;
  }
  return returnVal;
}

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
L7_BOOL usmDbGarpIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t ifType;

  if (nimGetIntfType(intIfNum, &ifType) != L7_SUCCESS)
    return L7_FALSE;

  return garpIsValidIntfType(ifType);
}

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
L7_RC_t usmDbGarpStatisticGet(L7_uint32 interface, GARP_STAT_COUNTER_TYPE_t counterType, L7_uint32 *val)
{
  return garpStatisticGet(interface, counterType, val);
}

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
L7_RC_t usmDbGarpRxStatisticGet(L7_uint32 interface, L7_GARP_ATTRIBUTE_TYPE_t garpAttrType, L7_uint32 *val)
{
  GARP_STAT_COUNTER_TYPE_t counterType;

  switch(garpAttrType)
  {
    case L7_GARP_JOIN_EMPTY:
      counterType = GARP_JOIN_EMPTY_RECEIVED; 
      break;
    case L7_GARP_JOIN_IN:
      counterType = GARP_JOIN_IN_RECEIVED; 
      break;
    case L7_GARP_EMPTY:
      counterType = GARP_EMPTY_RECEIVED; 
      break;
    case L7_GARP_LEAVE_IN:
      counterType = GARP_LEAVE_IN_RECEIVED; 
      break;
    case L7_GARP_LEAVE_EMPTY:
      counterType = GARP_LEAVE_EMPTY_RECEIVED; 
      break;
    case L7_GARP_LEAVEALL:
      counterType = GARP_LEAVEALL_RECEIVED; 
      break;
    default:
      return L7_FAILURE; 
  }
  return garpStatisticGet(interface, counterType, val);
}
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
L7_RC_t usmDbGarpTxStatisticGet(L7_uint32 interface, L7_GARP_ATTRIBUTE_TYPE_t garpAttrType, L7_uint32 *val)
{
  GARP_STAT_COUNTER_TYPE_t counterType;

  switch(garpAttrType)
  {
    case L7_GARP_JOIN_EMPTY:
      counterType = GARP_JOIN_EMPTY_SENT;
      break;
    case L7_GARP_JOIN_IN:
      counterType = GARP_JOIN_IN_SENT;
      break;
    case L7_GARP_EMPTY:
      counterType = GARP_EMPTY_SENT;
      break;
    case L7_GARP_LEAVE_IN:
      counterType = GARP_LEAVE_IN_SENT;
      break;
    case L7_GARP_LEAVE_EMPTY:
      counterType = GARP_LEAVE_EMPTY_SENT;
      break;
    case L7_GARP_LEAVEALL:
      counterType = GARP_LEAVEALL_SENT;
      break;
    default:
      return L7_FAILURE;
  }
  return garpStatisticGet(interface, counterType, val);
}

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
L7_RC_t usmDbGarpErrorStatisticGet(L7_uint32 interface, GARP_ERROR_STAT_COUNTER_TYPE_t counterType, L7_uint32 *val)
{
  return garpErrorStatisticGet(interface, counterType, val);
}

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
L7_RC_t usmDbGarpStatisticsClear(L7_uint32 interface)
{
  return garpStatisticsClear(interface);
}

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
L7_RC_t usmDbGarpErrorStatisticsClear(L7_uint32 interface)
{
  return garpErrorStatisticsClear(interface);
}

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
L7_RC_t usmDbGvrpVlanCreationForbidGet(L7_uint32 interface, L7_BOOL *forbid)
{
  return gvrpVlanCreationForbidGet(interface, forbid);
}

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
L7_RC_t usmDbGvrpPortVlanCreationForbidSet(L7_uint32 interface, L7_BOOL forbid)
{
  return gvrpPortVlanCreationForbidSet(interface, forbid);
}

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
L7_RC_t usmDbGvrpPortRegistrationForbidGet(L7_uint32 interface, L7_BOOL *forbid)
{
  return gvrpPortRegistrationForbidGet(interface, forbid);
}

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
L7_RC_t usmDbGvrpPortRegistrationForbidSet(L7_uint32 interface, L7_BOOL forbid)
{
  return gvrpPortRegistrationForbidSet(interface, forbid);
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename usmdb_snooping.c
*
* @purpose Provide interface to hardware API's for unitmgr components
*
* @component IGMP Snooping
*
* @comments none
*
* @create 04/16/2002
*
* @author mfiorito
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include "l7_common.h"
#include "usmdb_snooping_api.h"

#include "snooping_api.h"
#include "nimapi.h"
#include "snooping_debug_api.h"

/*********************************************************************
*
* @purpose  Gets the IGMP Snooping Admin mode
*
* @param    L7_uint32  UnitIndex    @b((input)) The unit for this operation
* @param    L7_uchar8  *adminMode   @b((output)) IGMP Snooping admin mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *adminMode,
                               L7_uchar8 family)
{
  return snoopAdminModeGet(adminMode, family);
}

/*********************************************************************
*
* @purpose  Gets the IGMP Snooping Priority
*
* @param    L7_uint32  UnitIndex    @b((input)) The unit for this operation
* @param    L7_uchar8  *prio        @b((output)) IGMP Snooping priority
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopPrioModeGet(L7_uint32 UnitIndex, L7_uint8 *prio, L7_uchar8 family)
{
  return snoopPrioModeGet(prio, family);
}

// PTin added
L7_RC_t usmDbSnoopVlanAdminModeSet(L7_uint32 UnitIndex, L7_uint32 adminMode,
                                   L7_uint32 intIfNum, L7_uint16 vlanId, L7_uchar8 family)
{
  return snoopVlanAdminModeSet(adminMode, intIfNum, vlanId, family);
}
// PTin end

/*********************************************************************
*
* @purpose  Sets the IGMP Snooping Admin mode
*
* @param    L7_uint32  UnitIndex   @b((input)) The unit for this operation
* @param    L7_uchar8  adminMode   @b((input)) IGMP Snooping admin mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopAdminModeSet(L7_uint32 UnitIndex, L7_uint32 adminMode,
                               L7_uint32 intIfNum, L7_uint16 vlanId, L7_uchar8 family)
{
  return snoopAdminModeSet(adminMode, intIfNum, vlanId, family);
}

/*********************************************************************
*
* @purpose  Sets the IGMP Snooping Priority
*
* @param    L7_uint32  UnitIndex   @b((input)) The unit for this operation
* @param    L7_uchar8  prio        @b((input)) IGMP Snooping priority
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopPrioModeSet(L7_uint32 UnitIndex, L7_uint8 prio, L7_uchar8 family)
{
  return snoopPrioModeSet(prio, family);
}

/*********************************************************************
*
* @purpose  Gets the IGMP Snooping mode for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *mode     @b((output)) IGMP Snooping intf mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                              L7_uint32 *mode, L7_uchar8 family)
{
  if (snoopIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return snoopIntfCfgModeGet(intIfNum, mode, family);
}

/*********************************************************************
*
* @purpose  Sets the IGMP Snooping mode for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
* @param    L7_uint32  intIfNum  @b((output)) Internal interface number
* @param    L7_uchar8  mode      @b((input))  IGMP Snooping intf mode
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                              L7_uint32 mode, L7_uchar8 family)
{
  if (snoopIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return snoopIntfCfgModeSet(intIfNum, mode, family);
}

/*********************************************************************
*
* @purpose  Gets interface mask of interfaces enabled for IGMP Snooping
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @param    L7_uchar8   *mask      @b{(output)} Bit Mask of the interfaces
*                                               1 or Set indicates this interface is enabled
*                                               0 indicates this interface is not enabled
* @param    L7_uint32   *maskLen   @b{(input/output)} Size of mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if bit mask buffer is not big enough
*
* @notes    Mask is written into and is returned with bits set to represent interfaces that are
*           in the intf list, in the manner SNMP wants. MSb represents interface 1
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfEnabledMaskGet(L7_uint32 UnitIndex, L7_uchar8 *mask,
                                     L7_uint32 *maskLen, L7_uchar8 family)
{
  NIM_INTF_MASK_t currentMask;
  L7_uint32 currentList[L7_IGMP_SNOOPING_MAX_INTF];
  L7_uint32 numList;
  L7_RC_t rc;

  if (*maskLen < NIM_INTF_INDICES)
    return L7_FAILURE;

  rc = snoopIntfEnabledListGet(&numList, currentList, family);
  if (rc == L7_FAILURE)
    return L7_FAILURE;

  *maskLen = NIM_INTF_INDICES;

  nimListToMask(currentList, numList, &currentMask);
  return nimReverseMask(currentMask, (NIM_INTF_MASK_t *)mask);
}

/*********************************************************************
*
* @purpose  Sets IGMP Snooping interface mode via a bit mask
*
* @param    L7_uint32   UnitIndex   @b((input))  The unit for this operation
* @param    L7_uchar8   *maskOctets @b{(input)}  Bit Mask of the interfaces
*                                                1 or Set indicates enable this interface
*                                                0 indicates disable this interface
* @param    L7_uint32   *maskLen    @b{(input/output)} Size of mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if mode could not be set or bit mask buffer is not big enough
*
* @notes    All set-able interfaces will be set.  If any one fails, a failure will
* @notes    be returned.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfEnabledMaskSet(L7_uint32 UnitIndex,
                                     L7_uchar8 *maskOctets,
                                     L7_uint32 *maskLen, L7_uchar8 family)
{
  /*take care of the fact that this mask may include enabling and
    disabling of interfaces. So need to compare this mask with
    the old list of interfaces*/
  NIM_INTF_MASK_t *mask;
  NIM_INTF_MASK_t oldMask;
  NIM_INTF_MASK_t newMask;
  NIM_INTF_MASK_t disableMask;
  NIM_INTF_MASK_t enableMask;

  L7_uint32 oldList[L7_IGMP_SNOOPING_MAX_INTF];
  L7_uint32 oldNum;
  L7_uint32 newList[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 newNum;
  L7_uint32 enableList[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 enableNum;
  L7_uint32 disableList[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 disableNum;

  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i;

  if (*maskLen < NIM_INTF_INDICES)
    return L7_FAILURE;

  mask = (NIM_INTF_MASK_t *)maskOctets;

  /*reverse the mask*/
  nimReverseMask(*mask, &newMask);
  /*generate the new intf list*/
  nimMaskToList(&newMask, newList, &newNum);
  /*get the old list of enabled interfaces*/
  rc = snoopIntfEnabledListGet(&oldNum, oldList, family);
  if (rc==L7_FAILURE)
    return L7_FAILURE;

  /*generate old mask*/
  nimListToMask(oldList, oldNum, &oldMask);
  /*calculate 1s to 0s i.e. those bits that were set in old but are now disabled*/
  /*algorithm is all bitwise operation
    old exor new -> result
    result and old -> a mask of 1s to 0s w.r.t. old mask
    i.e.mask contains list of interface to be disabled*/
  /*copy newmask into disableMask*/
  NIM_INTF_MASKEQ(disableMask, newMask);
  /*do the exor with oldMask  and write back into disableMask*/
  NIM_INTF_MASKEXOREQ(disableMask, oldMask);
  /*do the and with oldMask and write back to disableMask*/
  NIM_INTF_MASKANDEQ(disableMask, oldMask);
  /*convert the mask into the disabled list*/
  nimMaskToList(&disableMask, disableList, &disableNum);
  /*disable these interfaces*/
  for (i=0;i<disableNum;i++)
  {
    if (snoopIntfCfgModeSet(disableList[i], L7_DISABLE, family) != L7_SUCCESS)
      rc = L7_FAILURE;
  }


  /*calculate 0s to 1s i.e. those bits that were set to 0 in old but are now enabled*/
  /*algorithm is all bitwise operation
    old exor new -> result
    result and new -> a mask of 0s to 1s w.r.t. old mask
    i.e. mask contains list of interfaces to be enabled*/
  /*copy newmask into enableMask*/
  NIM_INTF_MASKEQ(enableMask, newMask);
  /*do the exor with oldMask and write back into enableMask*/
  NIM_INTF_MASKEXOREQ(enableMask, oldMask);
  /*do the and with the newmask and write back to enableMask*/
  NIM_INTF_MASKANDEQ(enableMask, newMask);
  /*convert the mask into the enabled list*/
  nimMaskToList(&enableMask, enableList, &enableNum);
  /*enable these interfaces*/
  for (i=0;i<enableNum;i++)
  {
    if (snoopIntfCfgModeSet(enableList[i], L7_ENABLE, family) != L7_SUCCESS)
      rc = L7_FAILURE;
  }

  /*if we reach here all is good and we can return a success*/
  *maskLen = NIM_INTF_INDICES;
  return rc;
}

/*********************************************************************
*
* @purpose  Get the IGMP group membership interval for the specified interface
*
* @param    L7_uint32  UnitIndex                  @b((input))  The unit for this operation
* @param    L7_uint32  intIfNum                   @b((input))  Internal interface number
* @param    L7_uint32  *groupMembershipInterval   @b((output)) Query time
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfGroupMembershipIntervalGet(L7_uint32 UnitIndex,
                                                 L7_uint32 intIfNum,
                                                 L7_uint32 *groupMembershipInterval
                                                 , L7_uchar8 family)
{
  return snoopIntfGroupMembershipIntervalGet(intIfNum, groupMembershipInterval,
                                             family);
}

/*********************************************************************
*
* @purpose  Set the IGMP group membership interval for the specified interface
*
* @param    L7_uint32  UnitIndex                 @b((input)) The unit for this operation
* @param    L7_uint32  intIfNum                  @b((output))Internal interface number
* @param    L7_uint32  groupMembershipInterval   @b((input)) New group membership interval
*
* @returns  L7_SUCCESS, if the group membership interval was set
* @returns  L7_FAILURE, if the specified group membership interval is out of range
*
* @notes    This will set the configured query time, but, will not change
* @notes    the expiration times of any existing Snooping entries.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfGroupMembershipIntervalSet(L7_uint32 UnitIndex,
                                                 L7_uint32 intIfNum,
                                                 L7_uint32 queryTime,
                                                 L7_uchar8 family)
{
  return snoopIntfApiGroupMembershipIntervalSet(intIfNum, queryTime, family);
}

/*********************************************************************
*
* @purpose  Get the IGMP query response time for the specified interface
*
* @param    L7_uint32  UnitIndex      @b((input))  The unit for this operation
* @param    L7_uint32  intIfNum       @b((input))  Internal interface number
* @param    L7_uint32  *responseTime  @b((output)) Response time
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfResponseTimeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                      L7_uint32 *responseTime, L7_uchar8 family)
{
  return snoopIntfResponseTimeGet(intIfNum, responseTime, family);
}

/*********************************************************************
*
* @purpose  Set the IGMP query response time for the specified interface
*
* @param    L7_uint32  UnitIndex     @b((input)) The unit for this operation
* @param    L7_uint32  intIfNum      @b((output))Internal interface number
* @param    L7_uint32  responseTime  @b((input)) New response time
*
* @returns  L7_SUCCESS, if the response time was set
* @returns  L7_FAILURE, if the specified response time is out of range
*
* @notes    This will set the configured response time as well as update the
* @notes    timers of all snooping entries if necessary.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfResponseTimeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                      L7_uint32 responseTime, L7_uchar8 family)
{
  return snoopIntfApiResponseTimeSet(intIfNum, responseTime, family);
}

/*********************************************************************
*
* @purpose  Get the mcast router expiration time for the specified interface
*
* @param    L7_uint32  UnitIndex    @b((input))  The unit for this operation
* @param    L7_uint32  intIfNum     @b((input))  Internal interface number
* @param    L7_uint32  *expiryTime  @b((output)) Expiry time
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfMcastRtrExpiryTimeGet(L7_uint32 UnitIndex,
                                            L7_uint32 intIfNum,
                                            L7_uint32 *expiryTime,
                                            L7_uchar8 family)
{
  return snoopIntfMcastRtrExpiryTimeGet(intIfNum, expiryTime, family);
}

/*********************************************************************
*
* @purpose  Set the mcast router expiration time for the specified interface
*
* @param    L7_uint32  UnitIndex    @b((input)) The unit for this operation
* @param    L7_uint32  intIfNum     @b((output))Internal interface number
* @param    L7_uint32  expiryTime   @b((input)) Expiry time
*
* @returns  L7_SUCCESS, if time set successfully
* @returns  L7_FAILURE, if time value out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfMcastRtrExpiryTimeSet(L7_uint32 UnitIndex,
                                            L7_uint32 intIfNum,
                                            L7_uint32 expiryTime,
                                            L7_uchar8 family)
{
  return snoopIntfMcastRtrExpiryTimeSet(intIfNum, expiryTime, family);
}

/*********************************************************************
*
* @purpose  Gets the Fast-Leave Admin mode for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input))  The unit for this operation
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uint32  *mode     @b((output)) Fast-Leave intf admin mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfFastLeaveAdminModeGet(L7_uint32 UnitIndex,
                                            L7_uint32 intIfNum,
                                            L7_uint32 *mode, L7_uchar8 family)
{
  return snoopIntfFastLeaveAdminModeGet(intIfNum, mode, family);
}

/*********************************************************************
*
* @purpose  Sets the Fast-Leave Admin mode for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
* @param    L7_uint32  intIfNum  @b((output))Internal interface number
* @param    L7_uint32  mode      @b((input)) Fast-Leave intf admin mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfFastLeaveAdminModeSet(L7_uint32 UnitIndex,
                                            L7_uint32 intIfNum, L7_uint32 mode,
                                            L7_uchar8 family)
{
  return snoopIntfFastLeaveAdminModeSet(intIfNum, mode, family);
}

/*********************************************************************
*
* @purpose  Gets the Multicast Router Detected mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uint32  vlanId    @b((input))  VLAN Id
* @param    L7_uint32  *mode     @b((output)) Multicast Router Detected mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfMrouterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                 L7_uint32 vlanId, L7_uint32 *mode,
                                 L7_uchar8 family)
{
  return snoopIntfMrouterGet(intIfNum, vlanId, mode, family);
}
/*********************************************************************
*
* @purpose  Gets the Multicast Router Detected mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uint32  *mode     @b((output)) Multicast Router Detected mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfMrouterStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                       L7_uint32 *mode, L7_uchar8 family)
{
  return snoopIntMrouterStatusGet(intIfNum, mode, family);
}
/*********************************************************************
*
* @purpose  Sets the specified interface as a multicast router interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uint32  mode      @b((input)) Mrouter intf admin mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopIntfMrouterSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                 L7_uint32 mode, L7_uchar8 family)
{
  return snoopIntfMrouterSet(intIfNum, mode, family);
}
/*********************************************************************
*
* @purpose  Flush all Snooping entries and multicast router entries
*
* @param    unitIndex      Unit Index
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopEntriesFlush(L7_uint32 unitIndex, L7_uchar8 family)
{
  return snoopEntriesFlush(family);
}

/*********************************************************************
*
* @purpose  Get the number of multicast control (i.e. IGMP) frames processed by the CPU
*
* @param    L7_uint32  UnitIndex                @b((input)) The unit for this operation
* @param    L7_uint32  *controlFramesProcessed  @b((output)) Number of control frames processed
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopControlFramesProcessed(L7_uint32 UnitIndex,
                                         L7_uint32 *controlFramesProcessed,
                                         L7_uchar8 family)
{
  return snoopControlFramesProcessed(controlFramesProcessed, family);
}

/*********************************************************************
*
* @purpose  Get the number of multicast data frames forwarded by the CPU
*
* @param    L7_uint32  UnitIndex              @b((input)) The unit for this operation
* @param    L7_uint32  *dataFramesForwarded   @b((output)) Number of data frames forwarded
*
* @returns  L7_SUCCESS
*
* @notes    This API is for reference only and is currently not used.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopDataFramesForwarded(L7_uint32 UnitIndex,
                                      L7_uint32 *dataFramesForwarded,
                                      L7_uchar8 family)
{
  return snoopDataFramesForwarded(dataFramesForwarded, family);
}

/*********************************************************************
*
* @purpose check to see if intIfNum is a valid igmp snooping Interface
*
* @param    UnitIndex       Unit Number
* @param    intIfNum        Interface Number
*
* @returns L7_TRUE     If valid interface
* @returns L7_FALSE    If not valid interface
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbSnoopIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return snoopIsValidIntf(intIfNum);
}

/*********************************************************************
* @purpose  Get the IGMP snooping Mode corresponding to the vlanId
*
* @param    UnitIndex       Unit Number
* @param    vlanid          id of the vlan
* @param    *vlanMode       IGMP snooping Mode for the vlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanModeGet(L7_uint32 unitIndex, L7_uint32 vlanId,
                              L7_uint32 *snoopVlanMode, L7_uchar8 family)
{
  return (snoopVlanModeGet(vlanId, snoopVlanMode, family));
}

/*********************************************************************
* @purpose  Set the IGMP snooping Mode corresponding to the vlanId
*
* @param    UnitIndex       Unit Number
* @param    vlanid          id of the vlan
* @param    vlanMode        IGMP snooping Mode for the vlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanModeSet(L7_uint32 unitIndex, L7_uint32 vlanId,
                              L7_uint32 snoopVlanMode, L7_uchar8 family)
{
  return (snoopVlanModeSet(vlanId, snoopVlanMode, family));
}

/*********************************************************************
*
* @purpose  Sets the configured IGMP Snooping fast leave mode for the specified VLAN
*
* @param    L7_uint32  vlanId             @b((input))  vlan Id
* @param    L7_uint32  fastLeaveMode      @b((input)) Configured IGMP Snooping Fast Leave
*                                         Mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanFastLeaveModeSet(L7_uint32 unitIndex, L7_uint32 vlanId,
                                       L7_BOOL fastLeaveMode,
                                       L7_uchar8 family)
{
  return (snoopVlanFastLeaveModeSet(vlanId, fastLeaveMode, family));
}

/*********************************************************************
*
* @purpose  Gets the configured IGMP Snooping fast leave mode for the specified VLAN
*
* @param    L7_uint32  vlanId             @b((input))  vlan Id
* @param    L7_uint32  fastLeaveMode      @b((output)) Configured IGMP Snooping Fast Leave
*                                         Mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanFastLeaveModeGet(L7_uint32 unitIndex, L7_uint32 vlanId,
                                       L7_BOOL *fastLeaveMode, L7_uchar8 family)
{
  return (snoopVlanFastLeaveModeGet(vlanId, fastLeaveMode, family));
}

/*********************************************************************
*
* @purpose  Sets the configured IGMP Snooping Group Membership Interval
*           for the specified VLAN
*
* @param    L7_uint32  vlanId                    @b((input))  vlan Id
* @param    L7_uint32  groupMembershipInterval   @b((input)) Configured IGMP Snooping Fast Leave
*                                                Mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanGroupMembershipIntervalSet(L7_uint32 unitIndex,
                                                 L7_uint32 vlanId,
                                                 L7_ushort16 groupMembershipInterval,
                                                 L7_uchar8 family)
{
  return (snoopVlanGroupMembershipIntervalSet(vlanId, groupMembershipInterval, family));
}

/*********************************************************************
*
* @purpose  Gets the configured IGMP Snooping Group Membership Interval
*           for the specified VLAN
*
* @param    L7_uint32  vlanId                    @b((input))  vlan Id
* @param    L7_uint32  groupMembershipInterval   @b((output)) Configured IGMP Snooping Group
*                                                Membership Interval
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanGroupMembershipIntervalGet(L7_uint32 unitIndex,
                                                 L7_uint32 vlanId,
                                                 L7_ushort16 *groupMembershipInterval,
                                                 L7_uchar8 family)
{
  return (snoopVlanGroupMembershipIntervalGet(vlanId, groupMembershipInterval, family));
}

/*********************************************************************
*
* @purpose  Sets the configured IGMP Snooping Maximum Response Time
*           for the specified VLAN
*
* @param    L7_uint32  vlanId                @b((input))  vlan Id
* @param    L7_uint32  responseTime          @b((input)) Configured IGMP Snooping Maximum
*                                            response Time
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanMaximumResponseTimeSet(L7_uint32 unitIndex,
                                             L7_uint32 vlanId,
                                             L7_ushort16 responseTime,
                                             L7_uchar8 family)
{
  return (snoopVlanMaximumResponseTimeSet(vlanId, responseTime, family));
}

/*********************************************************************
*
* @purpose  Gets the configured IGMP Snooping Maximum Response Time
*           for the specified VLAN
*
* @param    L7_uint32  vlanId                @b((input))  vlan Id
* @param    L7_uint32  responseTime          @b((output)) Configured IGMP Snooping Maximum
*                                            response Time
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanMaximumResponseTimeGet(L7_uint32 unitIndex,
                                             L7_uint32 vlanId,
                                             L7_ushort16 *responseTime,
                                             L7_uchar8 family)
{
  return (snoopVlanMaximumResponseTimeGet(vlanId, responseTime, family));
}

/*********************************************************************
*
* @purpose  Sets the configured IGMP Snooping Mcast Router Expiry Time
*           for the specified VLAN
*
* @param    L7_uint32  vlanId                      @b((input))  vlan Id
* @param    L7_uint32  mcastRtrExpiryTime          @b((input)) Configured IGMP Snooping Maximum
*                                                  response Time
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanMcastRtrExpiryTimeSet(L7_uint32 unitIndex,
                                            L7_uint32 vlanId,
                                            L7_ushort16 mcastRtrExpiryTime,
                                            L7_uchar8 family)
{
  return (snoopVlanMcastRtrExpiryTimeSet(vlanId, mcastRtrExpiryTime, family));
}

/*********************************************************************
*
* @purpose  Sets the configured IGMP Snooping Mcast Router Expiry Time
*           for the specified VLAN
*
* @param    L7_uint32  vlanId                      @b((input))  vlan Id
* @param    L7_uint32  mcastRtrExpiryTime          @b((input)) Configured IGMP Snooping Maximum
*                                                  response Time
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopVlanMcastRtrExpiryTimeGet(L7_uint32 unitIndex,
                                            L7_uint32 vlanId,
                                            L7_ushort16 *mcastRtrExpiryTime,
                                            L7_uchar8 family)
{
  return (snoopVlanMcastRtrExpiryTimeGet(vlanId, mcastRtrExpiryTime, family));
}

/*********************************************************************
*
* @purpose  Set the IGMP static mcast router attached status for the specified interface/vlan
*
* @param    L7_uint32  intIfNum          @b((input)) Internal interface number
* @param    L7_uint32  responseTime      @b((input)) New response time
*
* @returns  L7_SUCCESS, if the response time was set
* @returns  L7_FAILURE, if the specified response time is out of range
*
* @notes    This will set the configured response time, but, will not update
* @notes    the response time of any existing Snooping entries.
*
* @end
*********************************************************************/
L7_RC_t usmDbsnoopIntfApiVlanStaticMcastRtrSet(L7_uint32 unitIndex,
                                               L7_uint32 intIfNum,
                                               L7_uint32 vlanId,
                                               L7_uchar8 status,
                                               L7_uchar8 family)
{
  return (snoopIntfApiVlanStaticMcastRtrSet(intIfNum, vlanId, status, family));
}

/*********************************************************************
*
* @purpose  Get the IGMP static mcast router attached status for the specified interface/vlan
*
* @param    L7_uint32  intIfNum          @b((input)) Internal interface number
* @param    L7_uint32  responseTime      @b((input)) New response time
*
* @returns  L7_SUCCESS, if the response time was set
* @returns  L7_FAILURE, if the specified response time is out of range
*
* @notes    This will set the configured response time, but, will not update
* @notes    the response time of any existing Snooping entries.
*
* @end
*********************************************************************/
L7_RC_t usmDbsnoopIntfApiVlanStaticMcastRtrGet(L7_uint32 unitIndex,
                                               L7_uint32 intIfNum,
                                               L7_uint32 vlanId,
                                               L7_uchar8 *status,
                                               L7_uchar8 family)
{
  return (snoopIntfApiVlanStaticMcastRtrGet(intIfNum, vlanId, status, family));
}

/*********************************************************************
* @purpose  Get the current status of displaying snoop packet debug info
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
L7_RC_t usmDbSnoopPacketDebugTraceFlagGet(L7_BOOL *transmitFlag,
                                       L7_BOOL *receiveFlag, L7_uchar8 family)
{
  return snoopDebugPacketTraceFlagGet(transmitFlag,receiveFlag, family);
}

/*********************************************************************
* @purpose  Turns on/off the displaying of snoop packet debug info
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
L7_RC_t usmDbSnoopPacketDebugTraceFlagSet(L7_BOOL transmitFlag,
                                          L7_BOOL receiveFlag, L7_uchar8 family)
{
  return snoopDebugPacketTraceFlagSet(transmitFlag,receiveFlag, family);
}

/*********************************************************************
* @purpose  Validates if the passed family type snooping is supported
*           or not
*
* @param    family
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL usmDbSnoopProtocolGet(L7_uchar8 family)
{
  return snoopProtocolGet(family);
}
/*********************************************************************
* @purpose  Validates if the passed family type snooping is supported
*           or not
*
* @param    family
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL usmDbSnoopProtocolNextGet(L7_uchar8 family, L7_uchar8 *nextFamily)
{
  return snoopProtocolNextGet(family, nextFamily);
}

/**********************************************************************
* @purpose  Sets the Snooping Querier Admin mode of a snoop instance
*
* @param    adminMode   @b{(input)}   Snooping Querier admin mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbSnoopQuerierAdminModeSet(L7_uint32 adminMode, L7_uchar8 family)
{
   return snoopQuerierAdminModeSet(adminMode, family);
}
/***********************************************************************
* @purpose  Gets the Snooping Querier Admin mode of a snoop instance
*
* @param    adminMode   @b{(output)}  Snooping Querier admin mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
***********************************************************************/
L7_RC_t usmDbSnoopQuerierAdminModeGet(L7_uint32 *adminMode, L7_uchar8 family)
{
   return snoopQuerierAdminModeGet(adminMode, family);
}
/*********************************************************************
* @purpose  Gets the Snooping Querier Configured Address of a snoop
*           instance
*
* @param    snoopQuerierAddr @b{(output)}  Pointer to Querier Address
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopQuerierAddressGet(void *snoopQuerierAddr,
                                    L7_uchar8 family)
{
  return snoopQuerierAddressGet(snoopQuerierAddr,family);
}

/*********************************************************************
* @purpose  Sets the Snooping Querier Address of a snoop instance
*
* @param    snoopQuerierAddr @b{(input)}   Querier Address
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopQuerierAddressSet(void *snoopQuerierAddr,
                                    L7_uchar8 family)
{
  return snoopQuerierAddressSet(snoopQuerierAddr, family);
}
/*********************************************************************
* @purpose  Gets the Snooping Querier Configured version of a snoop
*           instance
*
* @param    version     @b{(output)}  Configured querier version
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopQuerierVersionGet(L7_uint32 *version, L7_uchar8 family)
{
  return snoopQuerierVersionGet(version, family);
}
/*********************************************************************
* @purpose  Sets the Snooping Querier version of a snoop instance
*
* @param    version     @b{(input)}   Configured querier version
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnoopQuerierVersionSet(L7_uint32 version, L7_uchar8 family)
{
  return snoopQuerierVersionSet(version, family);
}

/*****************************************************************************
* @purpose  Gets the Snooping Querier Query Interval of a snoop instance
*
* @param    queryInterval @b{(output)}  Interval for sending periodic queries
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbSnoopQuerierQueryIntervalGet(L7_uint32 *queryInterval,
                                          L7_uchar8 family)
{
  return snoopQuerierQueryIntervalGet(queryInterval, family);
}
/*****************************************************************************
* @purpose  Sets the Snooping Querier Query Interval of a snoop instance
*
* @param    queryInterval @b{(input)}   Interval for sending periodic queries
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbSnoopQuerierQueryIntervalSet(L7_uint32 queryInterval,
                                          L7_uchar8 family)
{
  return snoopQuerierQueryIntervalSet(queryInterval, family);
}
/*****************************************************************************
* @purpose  Gets the other Querier expiry Interval of a snoop instance
*
* @param    queryInterval @b{(output)}  Interval for declaring last querier
*                                       as expired
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbSnoopQuerierExpiryIntervalGet(L7_uint32 *expiryInterval,
                                           L7_uchar8 family)
{
  return snoopQuerierExpiryIntervalGet(expiryInterval, family);
}
/*****************************************************************************
* @purpose  Sets the other Querier expiry Interval of a snoop instance
*
* @param    queryInterval @b{(input)}   Interval for declaring last querier
*                                       as expired
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbSnoopQuerierExpiryIntervalSet(L7_uint32 expiryInterval,
                                           L7_uchar8 family)
{
  return snoopQuerierExpiryIntervalSet(expiryInterval, family);
}

/*****************************************************************************
* @purpose  Gets the configured Snooping Querier mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(output)} Configured Snooping Querier vlan mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbSnoopQuerierVlanModeGet(L7_uint32 vlanId, L7_uint32 *vlanMode,
                                     L7_uchar8 family)
{
  return snoopQuerierVlanModeGet(vlanId, vlanMode, family);
}

/*****************************************************************************
* @purpose  Sets the configured Snooping Querier mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(input)}  Configured Snooping Querier vlan mode
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbSnoopQuerierVlanModeSet(L7_uint32 vlanId, L7_uint32 vlanMode,
                                     L7_uchar8 family)
{
  return snoopQuerierVlanModeSet(vlanId, vlanMode, family);
}

/*****************************************************************************
* @purpose  Gets the configured Snooping Querier election mode for the
*           specified vlanId of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(output)} Configured Snooping Querier vlan election
*                                    participate mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbSnoopQuerierVlanElectionModeGet(L7_uint32 vlanId, L7_uint32 *vlanMode,
                                             L7_uchar8 family)
{
  return snoopQuerierVlanElectionModeGet(vlanId, vlanMode, family);
}

/*****************************************************************************
* @purpose  Sets the configured Snooping Querier election mode for the
*           specified vlanId of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(input)}  Configured Snooping Querier vlan election
*                                    participate mode
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbSnoopQuerierVlanElectionModeSet(L7_uint32 vlanId, L7_uint32 vlanMode,
                                             L7_uchar8 family)
{
  return snoopQuerierVlanElectionModeSet(vlanId, vlanMode, family);
}
/******************************************************************************
* @purpose  Gets the Snooping Querier Configured Address for specified vlan Id
*           of a snoop instance
*
* @param    vlanId                @b{(input)}   vlan Id
* @param    snoopQuerierAddr      @b{(output)}  Querier vlan address
* @param    family                @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                               L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t usmDbSnoopQuerierVlanAddressGet(L7_uint32 vlanId,
                                        void *snoopQuerierAddr,
                                        L7_uchar8 family)
{
  return snoopQuerierVlanAddressGet(vlanId, snoopQuerierAddr, family);
}

/******************************************************************************
* @purpose  Gets the Snooping Querier Configured Address for specified vlan Id
*           of a snoop instance
*
* @param    vlanId                @b{(input)}  vlan Id
* @param    snoopQuerierAddr      @b{(input)}  Querier vlan address
* @param    family                @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                              L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t usmDbSnoopQuerierVlanAddressSet(L7_uint32 vlanId,
                                        void *snoopQuerierAddr,
                                        L7_uchar8 family)
{
  return snoopQuerierVlanAddressSet(vlanId, snoopQuerierAddr, family);
}


/******************************************************************************
* @purpose  Gets the detected last Querier Address for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    querierAddr      @b{(output)}  last Querier vlan address
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t usmDbSnoopQuerierLastQuerierAddressGet(L7_uint32 vlanId,
                                               void *querierAddr,
                                               L7_uchar8 family)
{
  return snoopQuerierLastQuerierAddressGet(vlanId, querierAddr, family);
}
/******************************************************************************
* @purpose  Gets the detected last Querier's version for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    version          @b{(output)}  last Querier protocol version
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t usmDbSnoopQuerierLastQuerierVersionGet(L7_uint32  vlanId,
                                               L7_uint32 *version,
                                               L7_uchar8  family)
{
  return snoopQuerierLastQuerierVersionGet(vlanId, version, family);
}

/******************************************************************************
* @purpose  Gets the Operationa version of Querier  for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    version          @b{(output)}  Operational Version
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t usmDbSnoopQuerierOperVersionGet(L7_uint32 vlanId, L7_uint32 *version,
                                   L7_uchar8 family)
{
  return snoopQuerierOperVersionGet(vlanId, version, family);
}
/******************************************************************************
* @purpose  Gets the Operational State of Querier  for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    state            @b{(output)}  Operational State
*                                          L7_SNOOP_QUERIER_STATE_t
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t usmDbSnoopQuerierOperStateGet(L7_uint32 vlanId, L7_uint32 *state,
                                      L7_uchar8 family)
{
  return snoopQuerierOperStateGet(vlanId, state,family);
}

/******************************************************************************
* @purpose  Gets the Operational value of max response time for specified
*           vlan Id of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    maxRespTime      @b{(output)}  Operational Max Response Time value
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    When success, if maxRespTime == 0, it means that the max response
*           time is less than 1 sec
*
* @end
******************************************************************************/
L7_RC_t usmDbSnoopQuerierOperMaxRespTimeGet(L7_uint32 vlanId,
                                            L7_uint32 *maxRespTime,
                                            L7_uchar8 family)
{
  return snoopQuerierOperMaxRespTimeGet(vlanId, maxRespTime, family);
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename usmdb_lldp.c
*
* @purpose Provides the USMDB interface for 802.1AB component
*
* @component 802.1AB
*
* @comments none
*
* @create 02/14/2005
*
* @author dfowler
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include <string.h>
#include <stdio.h>

#include "l7_common.h"
#include "usmdb_lldp_api.h"
#include "usmdb_util_api.h"
#include "osapi_support.h"

#include "lldp_api.h"
#include "cnfgr.h"


/*********************************************************************
 *                802.1AB Configuration APIs
 *********************************************************************/

/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit interval
*
* @param    L7_uint32  interval  @b((input))  interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpTxIntervalSet(L7_uint32 interval)
{
  return lldpTxIntervalSet(interval);
}
/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit interval
*
* @param    L7_uint32  *interval @b((output)) interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpTxIntervalGet(L7_uint32 *interval)
{
  return lldpTxIntervalGet(interval);
}
/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit delay
*
* @param    L7_uint32  delay  @b((input))  delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpTxDelaySet(L7_uint32 interval)
{
  return lldpTxDelaySet(interval);
}
/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit delay
*
* @param    L7_uint32  *delay @b((output)) delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpTxDelayGet(L7_uint32 *delay)
{
  return lldpTxDelayGet(delay);
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit hold muliplier
*
* @param    L7_uint32  hold      @b((input))  hold multiplier
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid hold value
*
* @notes    hold mulitplier must be between 2-10 seconds
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpTxHoldSet(L7_uint32 hold)
{
  return lldpTxHoldSet(hold);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit hold muliplier
*
* @param    L7_uint32  *hold     @b((output)) hold multiplier
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpTxHoldGet(L7_uint32 *hold)
{
  return lldpTxHoldGet(hold);
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit reinit delay
*
* @param    L7_uint32  delay     @b((input))  delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid delay value
*
* @notes    delay must be between 1-10 seconds
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpTxReinitDelaySet(L7_uint32 delay)
{
  return lldpTxReinitDelaySet(delay);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit reinit delay
*
* @param    L7_uint32  *delay     @b((output)) delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpTxReinitDelayGet(L7_uint32 *delay)
{
  return lldpTxReinitDelayGet(delay);
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB global notification interval limit
*
* @param    L7_uint32  interval  @b((input))  interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpNotificationIntervalSet(L7_uint32 interval)
{
  return lldpNotificationIntervalSet(interval);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB global notification interval limit
*
* @param    L7_uint32  *interval @b((output)) interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpNotificationIntervalGet(L7_uint32 *interval)
{
  return lldpNotificationIntervalGet(interval);
}

/*********************************************************************
* @purpose  Determine if the interface is valid for 802.1AB
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS, if interface is valid for 802.1AB
* @returns  L7_FAILURE, otherwise
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIsValidIntf(L7_uint32 intIfNum)
{
  if (lldpIsValidIntf(intIfNum) == L7_TRUE)
    return L7_SUCCESS;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the first interface that is valid for 802.1AB
*
* @param    *intIfNum  @b{(output)} internal interface number
*
* @returns  L7_SUCCESS, if a valid interface is found
* @returns  L7_FAILURE, otherwise
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpValidIntfFirstGet(L7_uint32 *intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 interface;

  rc = usmDbValidIntIfNumFirstGet(&interface);
  while (rc == L7_SUCCESS)
  {
    if (lldpIsValidIntf(interface) == L7_TRUE)
    {
      *intIfNum = interface;
      return L7_SUCCESS;
    }
    else
    {
      rc = usmDbValidIntIfNumNext(interface, &interface); 
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next interface that is valid for 802.1AB
*
* @param     prevIfNum  @b{(input)} internal interface number
* @param    *intIfNum  @b{(output)} internal interface number
*
* @returns  L7_SUCCESS, if a valid interface is found
* @returns  L7_FAILURE, otherwise
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpValidIntfNextGet(L7_uint32 prevIfNum, L7_uint32 *intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 interface;

  rc = usmDbValidIntIfNumNext(prevIfNum, &interface); 
  while (rc == L7_SUCCESS)
  {
    if (lldpIsValidIntf(interface) == L7_TRUE)
    {
      *intIfNum = interface;
      return L7_SUCCESS;
    }
    else
    {
      rc = usmDbValidIntIfNumNext(interface, &interface); 
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_uint32  mode      @b((input))  802.1AB transmit mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfTxModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  return lldpIntfTxModeSet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  *mode     @b((output)) configured 802.1AB intf mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfTxModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  return lldpIntfTxModeGet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB receive mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  mode      @b((input))  intf receive mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfRxModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  return lldpIntfRxModeSet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB receive mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  *mode     @b((output)) configured intf receive mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfRxModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  return lldpIntfRxModeGet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB notify mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_uint32  mode      @b((input)) intf notify mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfNotificationModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  return lldpIntfNotificationModeSet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB notify mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  *mode     @b((output)) intf notify mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfNotificationModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  return lldpIntfNotificationModeGet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB optional transmit TLVs for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_BOOL    portDesc  @b((input))  transmit port desc TLV
* @param    L7_BOOL    sysName   @b((input))  transmit system name TLV
* @param    L7_BOOL    sysDesc   @b((input))  transmit system desc TLV
* @param    L7_BOOL    sysCap    @b((input))  transmit system capability TLV
*
* @returns  L7_SUCCESS, if optional TLVs are set successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfTxTLVsSet(L7_uint32 intIfNum,
                               L7_BOOL   portDesc,
                               L7_BOOL   sysName,
                               L7_BOOL   sysDesc,
                               L7_BOOL   sysCap)
{
  return lldpIntfTxTLVsSet(intIfNum, portDesc, sysName, sysDesc, sysCap);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB optional transmit TLVs for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_BOOL    portDesc  @b((output))  transmit port desc TLV
* @param    L7_BOOL    sysName   @b((output))  transmit system name TLV
* @param    L7_BOOL    sysDesc   @b((output))  transmit system desc TLV
* @param    L7_BOOL    sysCap    @b((output))  transmit system capability TLV
*
* @returns  L7_SUCCESS, if values retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfTxTLVsGet(L7_uint32 intIfNum,
                               L7_BOOL   *portDesc,
                               L7_BOOL   *sysName,
                               L7_BOOL   *sysDesc,
                               L7_BOOL   *sysCap)
{
  return lldpIntfTxTLVsGet(intIfNum, portDesc, sysName, sysDesc, sysCap);
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB mgmt addr transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_BOOL    enabled   @b((input))  transmit mgmt addr TLV
*
* @returns  L7_SUCCESS, if mgmt addr transmit is set successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfTxMgmtAddrSet(L7_uint32 intIfNum, L7_BOOL enabled)
{
  return lldpIntfTxMgmtAddrSet(intIfNum, enabled);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB mgmt addr transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_BOOL    enabled   @b((output))  transmit mgmt addr TLV
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpIntfTxMgmtAddrGet(L7_uint32 intIfNum, L7_BOOL *enabled)
{
  return lldpIntfTxMgmtAddrGet(intIfNum, enabled);
}

/*********************************************************************
 *                802.1AB Statistics APIs
 *********************************************************************/

/*********************************************************************
*
* @purpose  Clears all 802.1AB statistics
*
* @returns  L7_SUCCESS, if statistics cleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsClear()
{
  return lldpStatsClear();
}

/*********************************************************************
*
* @purpose  Gets the last update time for 802.1AB remote data table
*
* @param    L7_uint32  lastUpate  @b((output))  time of last update
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRemoteLastUpdateGet(L7_uint32 *lastUpdate)
{
  return lldpStatsRemoteLastUpdateGet(lastUpdate);
}

/*********************************************************************
*
* @purpose  Gets the insert count for 802.1AB remote data table
*
* @param    L7_uint32  inserts   @b((output))   insert count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRemoteInsertsGet(L7_uint32 *inserts)
{
  return lldpStatsRemoteInsertsGet(inserts);
}

/*********************************************************************
*
* @purpose  Gets the delete count for 802.1AB remote data table
*
* @param    L7_uint32  deletes   @b((output))  delete count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRemoteDeletesGet(L7_uint32 *deletes)
{
  return lldpStatsRemoteDeletesGet(deletes);
}

/*********************************************************************
*
* @purpose  Gets the drop count for 802.1AB remote data table
*
* @param    L7_uint32  drops     @b((output))  drop count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRemoteDropsGet(L7_uint32 *drops)
{
  return lldpStatsRemoteDropsGet(drops);
}

/*********************************************************************
*
* @purpose  Gets the ageout count for 802.1AB remote data table
*
* @param    L7_uint32  ageouts   @b((output))  ageout count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRemoteAgeoutsGet(L7_uint32 *ageouts)
{
  return lldpStatsRemoteAgeoutsGet(ageouts);
}

/*********************************************************************
*
* @purpose  Gets the LLDP transmit frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  transmit frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsTxPortFramesTotalGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsTxPortFramesTotalGet(intIfNum, total);
}

/*********************************************************************
*
* @purpose  Gets the LLDP receive frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  receive frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRxPortFramesTotalGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsRxPortFramesTotalGet(intIfNum, total);
}

/*********************************************************************
*
* @purpose  Gets the LLDP discarded frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  discarded frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRxPortFramesDiscardedGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsRxPortFramesDiscardedGet(intIfNum, total);
}

/*********************************************************************
*
* @purpose  Gets the LLDP errors frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  errors frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRxPortFramesErrorsGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsRxPortFramesErrorsGet(intIfNum, total);
}

/*********************************************************************
*
* @purpose  Gets the LLDP ageouts count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  ageouts count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRxPortAgeoutsGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsRxPortAgeoutsGet(intIfNum, total);
}

/*********************************************************************
*
* @purpose  Gets the LLDP TLV discarded count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  discarded count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRxPortTLVsDiscardedGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsRxPortTLVsDiscardedGet(intIfNum, total);
}

/*********************************************************************
*
* @purpose  Gets the LLDP TLV unrecognized count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRxPortTLVsUnrecognizedGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsRxPortTLVsUnrecognizedGet(intIfNum, total);
}
/*********************************************************************
*
* @purpose  Gets the LLDP TLV unrecognized count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRxPortTLVsMEDGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsRxPortTLVsMEDGet(intIfNum, total);
}
/*********************************************************************
*
* @purpose  Gets the LLDP TLV unrecognized count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRxPortTLVs8023Get(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsRxPortTLVs8023Get(intIfNum, total);
}
/*********************************************************************
*
* @purpose  Gets the LLDP TLV 8021 count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpStatsRxPortTLVs8021Get(L7_uint32 intIfNum, L7_uint32 *total)
{
  return lldpStatsRxPortTLVs8021Get(intIfNum, total);
}

/*********************************************************************
 *                802.1AB Local Data APIs
 *********************************************************************/

/*********************************************************************
*
* @purpose  Gets the 802.1AB local chassis id subtype
*
* @param    lldpChassisIdSubtype_t  subtype   @b((output))  Chassis ID Subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocChassisIdSubtypeGet(lldpChassisIdSubtype_t *subtype)
{
  return lldpLocChassisIdSubtypeGet(subtype);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local chassis id
*
* @param    L7_uchar8   *chassisId  @b((output))  Chassis ID
* @param    L7_ushort16 *length     @b((output))  Chassis ID length
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    chassisId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           chassisId is encoded based on the ChassisIdSubtype
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocChassisIdGet(L7_uchar8 *chassisId, L7_ushort16 *length)
{
  return lldpLocChassisIdGet(chassisId, length);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system name
*
* @param    L7_char8   sysName   @b((output))  system name
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    sysName buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocSysNameGet(L7_char8 *sysName)
{
  return lldpLocSysNameGet(sysName);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system description
*
* @param    L7_char8   sysDesc   @b((output))  system description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    sysDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocSysDescGet(L7_char8 *sysDesc)
{
  return lldpLocSysDescGet(sysDesc);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system capabilities supported
*
* @param    L7_ushort16  sysCap  @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocSysCapSupportedGet(L7_ushort16 *sysCap)
{
  return lldpLocSysCapSupportedGet(sysCap);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system capabilities enabled
*
* @param    L7_ushort16  sysCap  @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocSysCapEnabledGet(L7_ushort16 *sysCap)
{
  return lldpLocSysCapEnabledGet(sysCap);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local port subtype for the specified interface
*
* @param    L7_uint32            intIfNum @b((input))   internal interface number
* @param    lldpPortIdSubtype_t  subtype  @b((output))  Port ID Subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocPortIdSubtypeGet(L7_uint32 intIfNum,
                                     lldpPortIdSubtype_t *subtype)
{
  return lldpLocPortIdSubtypeGet(intIfNum, subtype);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local port id for the specified interface
*
* @param    L7_uint32   intIfNum  @b((input))   internal interface number
* @param    L7_uchar8   *portId   @b((output))  Port ID
* @param    L7_ushort16 *length   @b((output))  Port ID length
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    portId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           portId is encoded based on the ChassisIdSubtype
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocPortIdGet(L7_uint32   intIfNum,
                              L7_uchar8   *portId,
                              L7_ushort16 *length)
{
  return lldpLocPortIdGet(intIfNum, portId, length);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local port description for the specified interface
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_char8  *portDesc  @b((output))  Port Description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    portDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocPortDescGet(L7_uint32 intIfNum,
                                L7_char8 *portDesc)
{
  return lldpLocPortDescGet(intIfNum, portDesc);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address
*
* @param    lldpIANAAddrFamilyNumber_t *family   @b((output))   IANA address family
* @param    L7_uchar8                  *address  @b((output))   management address
* @param    L7_uchar8                  *length   @b((output))   address length
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    address is encoded based on family
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocManAddrGet(lldpIANAAddrFamilyNumber_t *family,
                               L7_uchar8                  *address,
                               L7_uchar8                  *length)
{
  return lldpLocManAddrGet(family, address, length);
}


/*********************************************************************
*
* @purpose  Gets the 802.1AB management address interface subtype
*
* @param    lldpManAddrIfSubtype_t *subtype  @b((output))  interface subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocManAddrIfSubtypeGet(lldpManAddrIfSubtype_t *subtype)
{
  return lldpLocManAddrIfSubtypeGet(subtype);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address interface id
*
* @param    L7_uint32  *ifId     @b((output))  interface id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocManAddrIfIdGet(L7_uint32 *ifId)
{
  return lldpLocManAddrIfIdGet(ifId);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address OID
*
* @param    L7_char8  *addrOID   @b((output))  management address OID
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    addrOID buffer should be at least LLDP_MGMT_ADDR_OID_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpLocManAddrOIDGet(L7_char8 *addrOID)
{
  return lldpLocManAddrOIDGet(addrOID);
}


/*********************************************************************
 *                802.1AB Remote Data APIs
 *********************************************************************/


/*********************************************************************
*
* @purpose  Clears all 802.1AB remote data
*
* @returns  L7_SUCCESS, if datacleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemTableClear()
{
  return lldpRemTableClear();
}

/*********************************************************************
*
* @purpose  Get the unique values to identify an interface remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 *remIndex  @b((output))  remote data index
* @param    L7_uint32 *timestamp @b((output))  remote data timestamp
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    each interface can only have one remote entry.
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemEntryGet(L7_uint32 intIfNum,
                             L7_uint32 *remIndex,
                             L7_uint32 *timestamp)
{
  return lldpRemEntryGet(intIfNum, remIndex, timestamp);
}

/*********************************************************************
*
* @purpose  Get the next unique values to identify an interface remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 *remIndex  @b((output))  remote data index
* @param    L7_uint32 *timestamp @b((output))  remote data timestamp
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    each interface can only have one remote entry.
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemEntryGetNext(L7_uint32 intIfNum,
                                 L7_uint32 *remIndex,
                                 L7_uint32 *timestamp)
{
  return lldpRemEntryGetNext(intIfNum, remIndex, timestamp);
}

/*********************************************************************
*
* @purpose  Get the unique values to identify an interface remote entry
*
* @param    L7_uint32 timestamp @b((input))  remote data timestamp
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex  @b((input))  remote data index
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemTimestampIndexEntryGet(L7_uint32 timestamp,
                                           L7_uint32 intIfNum,
                                           L7_uint32 remIndex)
{
  return lldpRemTimestampIndexEntryGet(timestamp,intIfNum, remIndex);
}

/*********************************************************************
*
* @purpose  Get the Next unique values to identify an interface remote entry
*
* @param    L7_uint32 timestamp @b((output))  remote data timestamp
* @param    L7_uint32 intIfNum   @b((output))   internal interface number
* @param    L7_uint32 remIndex  @b((output))  remote data index
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemTimestampIndexEntryGetNext(L7_uint32 *timestamp,
                                               L7_uint32 *intIfNum,
                                               L7_uint32 *remIndex)
{
  return lldpRemTimestampIndexEntryGetNext(timestamp,intIfNum, remIndex);
}

/*********************************************************************
*
* @purpose  Get the time to live for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 *ttl       @b((output)) Time to Live seconds
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemTTLGet(L7_uint32 intIfNum,
                           L7_uint32 remIndex,
                           L7_uint32 timestamp,
                           L7_uint32 *ttl)
{
  return lldpRemTTLGet(intIfNum, remIndex, timestamp, ttl);
}

/*********************************************************************
*
* @purpose  Get the chassis subtype ID for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    lldpChassisIdSubtype_t subtype  @b((output))  remote chassis subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemChassisIdSubtypeGet(L7_uint32 intIfNum,
                                        L7_uint32 remIndex,
                                        L7_uint32 timestamp,
                                        lldpChassisIdSubtype_t *subtype)
{
  return lldpRemChassisIdSubtypeGet(intIfNum, remIndex, timestamp, subtype);
}

/*********************************************************************
*
* @purpose  Get the chassis ID for a remote entry
*
* @param    L7_uint32    intIfNum    @b((input))   internal interface number
* @param    L7_uint32    remIndex    @b((input))   remote data index
* @param    L7_uint32    timestamp   @b((input))   remote data timestamp
* @param    L7_uchar8    *chassisId  @b((output))  buffer to store chassis id
* @param    L7_ushort16  *length     @b((output))  length of chassis id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    chassisId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           chassisId is encoded based on ChassisIdSubtype
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemChassisIdGet(L7_uint32    intIfNum,
                                 L7_uint32    remIndex,
                                 L7_uint32    timestamp,
                                 L7_uchar8   *chassisId,
                                 L7_ushort16 *length)
{
  return lldpRemChassisIdGet(intIfNum, remIndex, timestamp, chassisId, length);
}

/*********************************************************************
*
* @purpose  Get the port id subtype for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   remote data index
* @param    L7_uint32 timestamp  @b((input))   remote data timestamp
* @param    lldpPortIdSubtype_t *subtype @b((output))  remote port id subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemPortIdSubtypeGet(L7_uint32 intIfNum,
                                     L7_uint32 remIndex,
                                     L7_uint32 timestamp,
                                     lldpPortIdSubtype_t *subtype)
{
  return lldpRemPortIdSubtypeGet(intIfNum, remIndex, timestamp, subtype);
}

/*********************************************************************
*
* @purpose  Get the port ID for a remote entry
*
* @param    L7_uint32    intIfNum   @b((input))   internal interface number
* @param    L7_uint32    remIndex   @b((input))   remote data index
* @param    L7_uint32    timestamp  @b((input))   remote data timestamp
* @param    L7_uchar8    *portId    @b((output))  remote port id
* @param    L7_ushort16  *length    @b((output))  length of port id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    portId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           portId buffer is encoded based on portIdSubtype
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemPortIdGet(L7_uint32    intIfNum,
                              L7_uint32    remIndex,
                              L7_uint32    timestamp,
                              L7_uchar8   *portId,
                              L7_ushort16 *length)
{
  return lldpRemPortIdGet(intIfNum, remIndex, timestamp, portId, length);
}

/*********************************************************************
*
* @purpose  Get the port description for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_char8  *portDesc  @b((output)) remote port description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    portDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemPortDescGet(L7_uint32 intIfNum,
                                L7_uint32 remIndex,
                                L7_uint32 timestamp,
                                L7_char8 *portDesc)
{
  return lldpRemPortDescGet(intIfNum, remIndex, timestamp, portDesc);
}

/*********************************************************************
*
* @purpose  Get the system name for the remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_char8  *sysName   @b((output)) remote system name
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    sysName buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemSysNameGet(L7_uint32 intIfNum,
                               L7_uint32 remIndex,
                               L7_uint32 timestamp,
                               L7_char8 *sysName)
{
  return lldpRemSysNameGet(intIfNum, remIndex, timestamp, sysName);
}

/*********************************************************************
*
* @purpose  Get the system description for the remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   remote data index
* @param    L7_uint32 timestamp  @b((input))   remote data timestamp
* @param    L7_char8  *sysDesc   @b((output))  remote system description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    sysDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemSysDescGet(L7_uint32 intIfNum,
                               L7_uint32 remIndex,
                               L7_uint32 timestamp,
                               L7_char8 *sysDesc)
{
  return lldpRemSysDescGet(intIfNum, remIndex, timestamp, sysDesc);
}

/*********************************************************************
*
* @purpose  Get the system capabilities supported for a remote entry
*
* @param    L7_uint32   intIfNum   @b((input))   internal interface number
* @param    L7_uint32   remIndex   @b((input))   remote data index
* @param    L7_uint32   timestamp  @b((input))   remote data timestamp
* @param    L7_ushort16 sysCap     @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemSysCapSupportedGet(L7_uint32    intIfNum,
                                       L7_uint32    remIndex,
                                       L7_uint32    timestamp,
                                       L7_ushort16 *sysCap)
{
  return lldpRemSysCapSupportedGet(intIfNum, remIndex, timestamp, sysCap);
}

/*********************************************************************
*
* @purpose  Get the system capabilities enabled for a remote entry
*
* @param    L7_uint32   intIfNum   @b((input))  internal interface number
* @param    L7_uint32   remIndex   @b((input))  remote data index
* @param    L7_uint32   timestamp  @b((input))  remote data timestamp
* @param    L7_ushort16 sysCap     @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemSysCapEnabledGet(L7_uint32    intIfNum,
                                     L7_uint32    remIndex,
                                     L7_uint32    timestamp,
                                     L7_ushort16 *sysCap)
{
  return lldpRemSysCapEnabledGet(intIfNum, remIndex, timestamp, sysCap);
}

/*********************************************************************
*
* @purpose  Get the next valid management address entry for the
*           indicated remote data entry.
*
* @param    L7_uint32                   intIfNum   @b((input))  internal interface number
* @param    L7_uint32                   remIndex   @b((input))  remote data index
* @param    L7_uint32                   timestamp  @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t *family     @b((input/output))   IANA address family
* @param    L7_uchar8                  *addr       @b((input/output))   remote mgmt address
* @param    L7_uchar8                  *length     @b((input/output))   address length
*
* @returns  L7_SUCCESS, entry found, addrIndex is valid
* @returns  L7_FAILURE, no more entries
*
* @notes    if *length == 0, this will return first entry.
*           addr buffer must be at least LLDP_MGMT_ADDR_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemManAddrEntryNextGet(L7_uint32                    intIfNum,
                                        L7_uint32                    remIndex,
                                        L7_uint32                    timestamp,
                                        lldpIANAAddrFamilyNumber_t  *family,
                                        L7_uchar8                   *addr,
                                        L7_uchar8                   *length)
{
  return lldpRemManAddrEntryNextGet(intIfNum, remIndex, timestamp, family, addr, length);
}

/*********************************************************************
*
* @purpose  Determine if a remote management address entry exists
*
* @param    L7_uint32                  intIfNum  @b((input))  internal interface number
* @param    L7_uint32                  remIndex  @b((input))  remote data index
* @param    L7_uint32                  timestamp @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family    @b((input))   IANA address family
* @param    L7_uchar8                  *addr     @b((input))   remote mgmt address
* @param    L7_uchar8                  length    @b((input))   address length
*
* @returns  L7_SUCCESS, entry exists
* @returns  L7_FAILURE, entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemManAddrEntryGet(L7_uint32                    intIfNum,
                                    L7_uint32                    remIndex,
                                    L7_uint32                    timestamp,
                                    lldpIANAAddrFamilyNumber_t   family,
                                    L7_uchar8                   *addr,
                                    L7_uchar8                    length)
{
  return lldpRemManAddrEntryGet(intIfNum, remIndex, timestamp, family, addr, length);
}

/*********************************************************************
*
* @purpose  Get interface subtype for a remote management address entry
*
* @param    L7_uint32                  intIfNum   @b((input))  internal interface number
* @param    L7_uint32                  remIndex   @b((input))  remote data index
* @param    L7_uint32                  timestamp  @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family     @b((input))   IANA address family
* @param    L7_uchar8                  *addr      @b((input))   remote mgmt address
* @param    L7_uchar8                  length     @b((input))   address length
* @param    lldpManAddrIfSubtype_t     *subtype   @b((output))  interface subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemManAddrIfSubtypeGet(L7_uint32                    intIfNum,
                                        L7_uint32                    remIndex,
                                        L7_uint32                    timestamp,
                                        lldpIANAAddrFamilyNumber_t   family,
                                        L7_uchar8                   *addr,
                                        L7_uchar8                    length,
                                        lldpManAddrIfSubtype_t      *subtype)
{
  return lldpRemManAddrIfSubtypeGet(intIfNum, remIndex, timestamp, family, addr, length, subtype);
}

/*********************************************************************
*
* @purpose  Get interface id for a remote management address entry
*
* @param    L7_uint32                  intIfNum   @b((input))  internal interface number
* @param    L7_uint32                  remIndex   @b((input))  remote data index
* @param    L7_uint32                  timestamp  @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family     @b((input))   IANA address family
* @param    L7_uchar8                  *addr      @b((input))   remote mgmt address
* @param    L7_uchar8                  length     @b((input))   address length
* @param    L7_uint32                  *id        @b((output)) interface id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemManAddrIfIdGet(L7_uint32                    intIfNum,
                                   L7_uint32                    remIndex,
                                   L7_uint32                    timestamp,
                                   lldpIANAAddrFamilyNumber_t   family,
                                   L7_uchar8                   *addr,
                                   L7_uchar8                    length,
                                   L7_uint32                   *id)
{
  return lldpRemManAddrIfIdGet(intIfNum, remIndex, timestamp, family, addr, length, id);
}

/*********************************************************************
*
* @purpose  Get OID for the remote management address entry
*
* @param    L7_uint32                  intIfNum   @b((input))   internal interface number
* @param    L7_uint32                  remIndex   @b((input))   remote data index
* @param    L7_uint32                  timestamp  @b((input))   remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family     @b((input))   IANA address family
* @param    L7_uchar8                  *addr      @b((input))   remote mgmt address
* @param    L7_uchar8                  length     @b((input))   address length
* @param    L7_char8                   *oid       @b((output))  management address OID
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    oid buffer must be at least LLDP_MGMT_ADDR_OID_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemManAddrOIDGet(L7_uint32                    intIfNum,
                                  L7_uint32                    remIndex,
                                  L7_uint32                    timestamp,
                                  lldpIANAAddrFamilyNumber_t   family,
                                  L7_uchar8                   *addr,
                                  L7_uchar8                    length,
                                  L7_char8                    *oid)
{
  return lldpRemManAddrOIDGet(intIfNum, remIndex, timestamp, family, addr, length, oid);
}

/*********************************************************************
*
* @purpose  Get the next unknown TLV entry for the indicated remote data entry.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 *tlvType   @b((input/output)) tlv type
*
* @returns  L7_SUCCESS, next entry found, tlvType is valid
* @returns  L7_FAILURE, no more entries
*
* @notes    if *tlvType == 0, this will return first entry.
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemUnknownTLVEntryNextGet(L7_uint32  intIfNum,
                                           L7_uint32  remIndex,
                                           L7_uint32  timestamp,
                                           L7_uint32 *tlvType)
{
  return lldpRemUnknownTLVEntryNextGet(intIfNum, remIndex, timestamp, tlvType);
}

/*********************************************************************
*
* @purpose  Determine if unknown TLV entry exists.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 tlvType    @b((input))  tlv type
*
* @returns  L7_SUCCESS, entry valid
* @returns  L7_FAILURE, entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemUnknownTLVEntryGet(L7_uint32  intIfNum,
                                       L7_uint32  remIndex,
                                       L7_uint32  timestamp,
                                       L7_uint32  tlvType)
{
  return lldpRemUnknownTLVEntryGet(intIfNum, remIndex, timestamp, tlvType);
}

/*********************************************************************
*
* @purpose  Get the TLV info for the specified remote entry
*
* @param    L7_uint32   intIfNum   @b((input))  internal interface number
* @param    L7_uint32   remIndex   @b((input))  remote data index
* @param    L7_uint32   timestamp  @b((input))  remote data timestamp
* @param    L7_uint32   tlvType    @b((input))  tlv type
* @param    L7_uchar8   *info      @b((output))  organization defined info
* @param    L7_ushort16 *length    @b((output))  length of info field
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    tlvInfo buffer must be at least LLDP_UNKNOWN_TLV_INFO_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemUnknownTLVInfoGet(L7_uint32    intIfNum,
                                      L7_uint32    remIndex,
                                      L7_uint32    timestamp,
                                      L7_uint32    tlvType,
                                      L7_uchar8   *info,
                                      L7_ushort16 *length)
{
  return lldpRemUnknownTLVInfoGet(intIfNum, remIndex, timestamp, tlvType, info, length);
}

/*********************************************************************
*
* @purpose  Get the next organizationally defined info entry for
*           the indicated remote data entry.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 *infoIndex @b((output)) info index
*
* @returns  L7_SUCCESS, entry found, tlvIndex is valid
* @returns  L7_FAILURE, no more entries
*
* @notes    if *infoIndex = 0, this will return first index.
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemOrgDefInfoEntryNextGet(L7_uint32  intIfNum,
                                           L7_uint32  remIndex,
                                           L7_uint32  timestamp,
                                           L7_uint32 *infoIndex)
{
  return lldpRemOrgDefInfoEntryNextGet(intIfNum, remIndex, timestamp, infoIndex);
}


/*********************************************************************
*
* @purpose  Determine if an organizationally defined info entry exists.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemOrgDefInfoEntryGet(L7_uint32    intIfNum,
                                       L7_uint32    remIndex,
                                       L7_uint32    timestamp,
                                       L7_uint32    infoIndex)
{
  return lldpRemOrgDefInfoEntryGet(intIfNum, remIndex, timestamp, infoIndex);
}

/*********************************************************************
*
* @purpose  Get the organizationally defined info for specified entry.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 infoIndex  @b((input))  info index
* @param    L7_uchar8   *info      @b((output))  organization defined info
* @param    L7_ushort16 *length    @b((output))  length of info field
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    orgDefInfo buffer must be at least LLDP_ORG_DEF_INFO_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpRemOrgDefInfoGet(L7_uint32    intIfNum,
                                  L7_uint32    remIndex,
                                  L7_uint32    timestamp,
                                  L7_uint32    infoIndex,
                                  L7_uchar8   *info,
                                  L7_ushort16 *length)
{
  return lldpRemOrgDefInfoGet(intIfNum, remIndex, timestamp, infoIndex, info, length);
}
/*********************************************************************
*
* @purpose  Get the organizationally defined subtype for specified entry.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
* @param    L7_uint32   *subtype    @b((output))  organization defined info
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    subtype buffer must be at least 1 char size
*
* @end
*********************************************************************/
L7_RC_t usmdbLldpRemOrgDefSubtypeGet(L7_uint32    intIfNum,
                             L7_uint32    remIndex,
                             L7_uint32    timestamp,
                             L7_uint32    infoIndex,
                             L7_uint32   *subtype)
{
  return lldpRemOrgDefSubtypeGet(intIfNum, remIndex, timestamp, infoIndex, subtype);
}
/*********************************************************************
*
* @purpose  Get the organizationally defined OUI for specified entry.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
* @param    L7_uchar8   *oui       @b((output))  organization defined info
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    oui buffer must be at least LLDP_ORG_DEF_INFO_OUI_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t usmdbLldpRemOrgDefOUIGet(L7_uint32    intIfNum,
                             L7_uint32    remIndex,
                             L7_uint32    timestamp,
                             L7_uint32    infoIndex,
                             L7_uchar8   *oui)
{
  return lldpRemOrgDefOUIGet(intIfNum, remIndex,timestamp, infoIndex, oui);
}
/*********************************************************************
*
* @purpose  Get a string representation for Chassis ID Subtype
*
* @param    lldpChassisIdSubtype_t type   @b((input))  Chassis ID Subtype
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpChassisIdSubtypeString(lldpChassisIdSubtype_t subtype)
{
  switch (subtype)
  {
    case LLDP_CHASSIS_ID_SUBTYPE_MAC_ADDR:
      return "MAC Address";
    case LLDP_CHASSIS_ID_SUBTYPE_CHASSIS_COMP:
      return "Chassis Component";
    case LLDP_CHASSIS_ID_SUBTYPE_INTF_ALIAS:
      return "Interface Alias";
    case LLDP_CHASSIS_ID_SUBTYPE_PORT_COMP:
      return "Port Component";
    case LLDP_CHASSIS_ID_SUBTYPE_NET_ADDR:
      return "Network Address";
    case LLDP_CHASSIS_ID_SUBTYPE_INTF_NAME:
      return "Interface Name";
    case LLDP_CHASSIS_ID_SUBTYPE_LOCAL:
      return "Local";
    default:
      return "Unknown";
  }
}

/*********************************************************************
*
* @purpose  Get a formatted Chassis ID string
*
* @param    L7_char8              *buffer    @b((output)) formatted chassis Id
* @param    L7_uint32              buflength @b((input))  maximum buffer length
* @param    lldpChassisIdSubtype_t subtype   @b((input))  chassis Id subtype
* @param    L7_uchar8             *chassisId @b((input))  chassis Id 
* @param    L7_ushort16            length    @b((input))  chassis Id length
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void usmDbLldpChassisIdFormat(L7_char8              *buffer,
                              L7_uint32              buflength,
                              lldpChassisIdSubtype_t subtype, 
                              L7_uchar8             *chassisId,
                              L7_ushort16            length)
{
  L7_uint32 i = 0;
  L7_uint32 netTypeIndex = 0;
  L7_uint32 addrFamily = L7_AF_INET6;
  
  memset(buffer, 0, buflength);
  switch (subtype)
  {
    case LLDP_CHASSIS_ID_SUBTYPE_MAC_ADDR:
      sprintf(buffer, "%02X", chassisId[0]);
      for (i=1; i<length; i++)
      {
        sprintf(buffer, "%s:%02X", buffer, chassisId[i]); 
      }
      break;
    case LLDP_CHASSIS_ID_SUBTYPE_NET_ADDR:
     
       switch (chassisId[netTypeIndex])
       {
         case LLDP_CHASSIS_ID_SUBTYPE_NET_IPV4:/* 4 Bytes */
           addrFamily = L7_AF_INET;
           /* fall thru*/
         case LLDP_CHASSIS_ID_SUBTYPE_NET_IPV6:
          osapiInetNtop(addrFamily, &chassisId[1], buffer, buflength);
          break;
         case LLDP_CHASSIS_ID_SUBTYPE_NET_ASNUMBER:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_XTPOVERIPV4:
           sprintf(buffer, "%d", chassisId[netTypeIndex+1]);
           for (i=netTypeIndex+2; i<=length-1; i++)
           {
             sprintf(buffer, "%s.%d", buffer, chassisId[i]);
           }
           break;
         case LLDP_CHASSIS_ID_SUBTYPE_NET_XTPOVERIPV6:
           
           sprintf(buffer, "%02X%02X", chassisId[netTypeIndex+1], chassisId[netTypeIndex+2]);
           for (i=netTypeIndex+3; i<=length-1; i+=2)
           {
             sprintf(buffer, "%s:%02X%02X", buffer, chassisId[i],chassisId[i+1]);
           }
           break;
         case LLDP_CHASSIS_ID_SUBTYPE_NET_NSAP:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_E164WITHNSAP:
           sprintf(buffer, "%02X%02X", chassisId[netTypeIndex+1], chassisId[netTypeIndex+2]);
           for (i=netTypeIndex+3; i<=length-1; i+=2)
           {
             sprintf(buffer, "%s%02X%02X", buffer, chassisId[i],chassisId[i+1]);
           }
           break;
         case LLDP_CHASSIS_ID_SUBTYPE_NET_HDLC: /* The address consists of three parts: A Service Access Point (SAP)
                                                   A Command/Response bit 
                                                   An address extension bit which is usually set to true to indicate 
                                                   that the address is of length one byte. 
                                                   When set to false it indicates an additional byte follows.  */
         case LLDP_CHASSIS_ID_SUBTYPE_NET_BBN1822: /* 24 bits  8 bits host number 16 bits IMP number*/
         case LLDP_CHASSIS_ID_SUBTYPE_NET_AFI:
            sprintf(buffer, "%02X",chassisId[netTypeIndex+1]);
           for (i=netTypeIndex+2; i<=length-1; i+=1)
           {
             sprintf(buffer,"%s%02X",buffer,chassisId[i]);
           }
          break;

         case LLDP_CHASSIS_ID_SUBTYPE_NET_ALL802:
           sprintf(buffer, "%02X",chassisId[netTypeIndex+1]);
           for (i=netTypeIndex+2; i<=length-1; i+=1)
           {
             sprintf(buffer,"%s:%02X",buffer,chassisId[i]);
           }
           break;
         case LLDP_CHASSIS_ID_SUBTYPE_NET_E163:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_E164:
           sprintf(buffer, "%d",chassisId[netTypeIndex+1]);
           for (i=netTypeIndex+2; i<=length-1; i+=1)
           {
             sprintf(buffer,"%s%d",buffer,chassisId[i]);
           }
           break;
         case LLDP_CHASSIS_ID_SUBTYPE_NET_F69:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_X121:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_BANYANVINES:
           sprintf(buffer, "%02X",chassisId[netTypeIndex+1]);
           for (i=netTypeIndex+2; i<=length-1; i+=1)
           {
             sprintf(buffer,"%s%02X",buffer,chassisId[i]);
           }
           break;
         case LLDP_CHASSIS_ID_SUBTYPE_NET_IPX: /* 80 bit address network:
                                                  An IPX address is hexidecimal with the format of network.node.node.node
                                                */
            sprintf(buffer, "%02X%02X%02X%02X.%02X%02X.%02X%02X.%02X%02X", 
            chassisId[netTypeIndex+1], chassisId[netTypeIndex+2],chassisId[netTypeIndex+3],
            chassisId[netTypeIndex+4], chassisId[netTypeIndex+5],chassisId[netTypeIndex+6],
            chassisId[netTypeIndex+7],chassisId[netTypeIndex+8],chassisId[netTypeIndex+9],
            chassisId[netTypeIndex+10]);
           break;

         case LLDP_CHASSIS_ID_SUBTYPE_NET_APPLETALK:/* 4 bytes This consisted of a two-byte network number,
                                                       a one-byte node number, and a one-byte socket number. */
         case LLDP_CHASSIS_ID_SUBTYPE_NET_DECNETIV:
           sprintf(buffer, "%d", chassisId[netTypeIndex+1]);
           for (i=netTypeIndex+2; i<=length-1; i++)
           {
             sprintf(buffer, "%s.%d", buffer, chassisId[i]);
           }
           break;
         case LLDP_CHASSIS_ID_SUBTYPE_NET_DNS:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_DISTINGUISHEDNAME:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_XTPNATIVEMODEXTP:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_FIBRECHANNELWWPN:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_FIBRECHANNELWWNN:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_GWID:
         case LLDP_CHASSIS_ID_SUBTYPE_NET_OTHER:
         default:
           if (length + 1 > buflength)
           {
             memcpy(buffer, chassisId+1, buflength - 5); /* Skipping first byte as it contains the type of NET ADDR */
             strcat(buffer, " ...");
           }
           else
           {
              memcpy(buffer, chassisId+1, length);/* Skipping first byte as it contains the type of NET ADDR */
           }
           break;
       }
      break;

    case LLDP_CHASSIS_ID_SUBTYPE_CHASSIS_COMP:
    case LLDP_CHASSIS_ID_SUBTYPE_INTF_ALIAS:
    case LLDP_CHASSIS_ID_SUBTYPE_PORT_COMP:
    case LLDP_CHASSIS_ID_SUBTYPE_INTF_NAME:
    case LLDP_CHASSIS_ID_SUBTYPE_LOCAL:
    default:
      if (length + 1 > buflength)
      {
        memcpy(buffer, chassisId, buflength - 5);
        strcat(buffer, " ...");
      }
      memcpy(buffer, chassisId, length);
      break;
  }
  return;
}

/*********************************************************************
*
* @purpose  Get a string representation for Port ID Subtype
*
* @param    lldpPortIdSubtype_t type   @b((input))  Port ID Subtype
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpPortIdSubtypeString(lldpPortIdSubtype_t subtype)
{
  switch (subtype)
  {
    case LLDP_PORT_ID_SUBTYPE_MAC_ADDR:
      return "MAC Address";
    case LLDP_PORT_ID_SUBTYPE_INTF_ALIAS:
      return "Interface Alias";
    case LLDP_PORT_ID_SUBTYPE_PORT_COMP:
      return "Port Component";
    case LLDP_PORT_ID_SUBTYPE_NET_ADDR:
      return "Network Address";
    case LLDP_PORT_ID_SUBTYPE_INTF_NAME:
      return "Interface Name";
    case LLDP_PORT_ID_SUBTYPE_AGENT_ID:
      return "Agent Circuit ID";
    case LLDP_PORT_ID_SUBTYPE_LOCAL:
      return "Local";
    default:
      return "Unknown";
  }
}

/*********************************************************************
*
* @purpose  Get a formatted Port Id string
*
* @param    L7_char8              *buffer    @b((output)) formatted port Id
* @param    L7_uint32              buflength @b((input))  maximum buffer length
* @param    lldpChassisIdSubtype_t subtype   @b((input))  port Id subtype
* @param    L7_uchar8             *portId    @b((input))  port Id 
* @param    L7_ushort16            length    @b((input))  port Id length
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void usmDbLldpPortIdFormat(L7_char8           *buffer,
                           L7_uint32           buflength,
                           lldpPortIdSubtype_t subtype, 
                           L7_uchar8          *portId,
                           L7_ushort16         length)
{
  L7_uint32 i = 0;
  memset(buffer, 0, buflength);
  switch (subtype)
  {
    case LLDP_PORT_ID_SUBTYPE_MAC_ADDR:
      sprintf(buffer, "%02X", portId[0]);
      for (i=1; i<length; i++)
      {
        sprintf(buffer, "%s:%02X", buffer, portId[i]); 
      }
      break;
    case LLDP_PORT_ID_SUBTYPE_INTF_ALIAS:
    case LLDP_PORT_ID_SUBTYPE_PORT_COMP:
    case LLDP_PORT_ID_SUBTYPE_NET_ADDR:
    case LLDP_PORT_ID_SUBTYPE_INTF_NAME:
    case LLDP_PORT_ID_SUBTYPE_AGENT_ID:
    case LLDP_PORT_ID_SUBTYPE_LOCAL:
    default:
      if (buflength < length + 1)
      {
        memcpy(buffer, portId, buflength - 5);
        strcat(buffer, " ...");
      }
      memcpy(buffer, portId, length);
      break;
  }
  return;
}

/*********************************************************************
*
* @purpose  Get a formatted string for system capabilties
*
* @param    L7_char8       *buffer    @b((output)) formatted string
* @param    L7_uint32       buflength @b((input))  maximum buffer length
* @param    L7_ushort16     sysCap    @b((input))  system capabilties mask
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void usmDbLldpSysCapFormat(L7_char8   *buffer,
                           L7_uint32   buflength,
                           L7_ushort16 sysCap)
{
  const L7_char8 *sep = "";
  memset(buffer, 0, buflength);
  while (sysCap != 0)
  {
    L7_ushort16     mask = 0; 
    const L7_char8 *str = L7_NULLPTR;
    if (sysCap & LLDP_SYS_CAP_MASK_OTHER)
    {
      mask = LLDP_SYS_CAP_MASK_OTHER;
      str = "other"; 
    }
    else if (sysCap & LLDP_SYS_CAP_MASK_REPEATER)
    {
      mask = LLDP_SYS_CAP_MASK_REPEATER;
      str = "repeater";
    }
    else if (sysCap & LLDP_SYS_CAP_MASK_BRIDGE)
    {
      mask = LLDP_SYS_CAP_MASK_BRIDGE;
      str = "bridge";
    }
    else if (sysCap & LLDP_SYS_CAP_MASK_WLAN)
    {
      mask = LLDP_SYS_CAP_MASK_WLAN;
      str = "WLAN access point"; 
    }
    else if (sysCap & LLDP_SYS_CAP_MASK_ROUTER)
    {
      mask = LLDP_SYS_CAP_MASK_ROUTER;
      str = "router";
    }
    else if (sysCap & LLDP_SYS_CAP_MASK_TELEPHONE)
    {
      mask = LLDP_SYS_CAP_MASK_TELEPHONE;
      str = "telephone";
    }
    else if (sysCap & LLDP_SYS_CAP_MASK_CABLE)
    {
      mask = LLDP_SYS_CAP_MASK_CABLE;
      str = "DOCSIS cable device";
    }
    else if (sysCap & LLDP_SYS_CAP_MASK_STATION)
    {
      mask = LLDP_SYS_CAP_MASK_STATION;
      str = "station only";
    }
    else if (sysCap & LLDP_SYS_CAP_MASK_RESERVED)
    {
      mask = LLDP_SYS_CAP_MASK_RESERVED;
      str = "reserved";
    }

    if (str != L7_NULLPTR)
    {
      if (strlen(buffer) + strlen(sep) + strlen(str) + 1 > buflength)
      {
        break;
      }
      sysCap &= ~mask;
      strcat(buffer, sep);
      strcat(buffer, str);
      if (strlen(sep) == 0)
      {
        sep = ", ";
      }
    }
  }

  if (sysCap != 0)
  {
    sprintf(buffer+(buflength-5), " ...");
  }

  return;
}

/*********************************************************************
*
* @purpose  Get a string representation for Management Address Family
*
* @param    lldpIANAAddrFamilyNumber_t family  @b((input))  address family
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpManAddrFamilyString(lldpIANAAddrFamilyNumber_t family)
{
  switch (family)
  {
    case LLDP_IANA_ADDR_FAMILY_NUMBER_RESERVED:
      return "reserved";
    case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4:
      return "IPv4";
    case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV6:
      return "IPv6";
    case LLDP_IANA_ADDR_FAMILY_NUMBER_802:
      return "802";
    default:
      return "other";
  }
}

/*********************************************************************
*
* @purpose  Get a formatted string for a management address
*
* @param    L7_char8                  *buffer    @b((output)) formatted string
* @param    L7_uint32                  buflength @b((input))  maximum buffer length
* @param    lldpIANAAddrFamilyNumber_t family    @b((input))  address family
* @param    L7_uchar8                 *address   @b((input))  management address
* @param    L7_uchar8                  length    @b((input))  address length
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void usmDbLldpManAddrFormat(L7_char8                  *buffer,
                            L7_uint32                  buflength,
                            lldpIANAAddrFamilyNumber_t family,
                            L7_uchar8                 *address,
                            L7_uchar8                  length)
{
  L7_uint32 i = 0;
  memset(buffer, 0, buflength);
  switch (family)
  {
    case LLDP_IANA_ADDR_FAMILY_NUMBER_802:
      sprintf(buffer, "%02X", address[0]);
      for (i=1; i<length; i++)
      {
        sprintf(buffer, "%s:%02X", buffer, address[i]); 
      }
      break;
    case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4:
    case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV6:
    case LLDP_IANA_ADDR_FAMILY_NUMBER_RESERVED:
    default:
      sprintf(buffer, "%u", address[0]);
      for (i=1; i<length; i++)
      {
        sprintf(buffer, "%s.%u", buffer, address[i]); 
      }
      break;
  }
  return;
}
/*********************************************************************
 *                LLDP-MED Configuration APIs
 *********************************************************************/
/*********************************************************************
*
* @purpose  Sets the MED admin mode for this interface
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  value @b((input))  admin mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedPortAdminModeSet(L7_uint32 intIfNum, L7_uint32 value)
{
  return lldpXMedPortAdminModeSet(intIfNum, value);
}
/*********************************************************************
*
* @purpose  Gets the MED admin mode for this interface
*
* @param    L7_uint32                  intIfNum @b((input)) Internal Interface Number
* @param    L7_uint32                  *value	@b((output)) configured admin mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedPortAdminModeGet(L7_uint32 intIfNum, L7_uint32 *value)
{
  return lldpXMedPortAdminModeGet (intIfNum, value);
}
/*********************************************************************
*
* @purpose  Gets whether we have seen a MED TLV and we are transmitting MED TLVs
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                   *status@b((input))  (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The port will transmit only if it has received an MED TLV
*			So even if we are admin mode enabled, we may not neccessarily transmit MED TLVs
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedPortTransmitEnabledGet(L7_uint32 intIfNum, L7_uint32 *status)
{
  return lldpXMedPortTransmitEnabledGet(intIfNum, status);
}
/*********************************************************************
*
* @purpose  Gets the local device's (This device) MED Classification
*
* @param    L7_uint32                *deviceClass @b((output))  endpoint C I, CII, CII or network connectivity
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocDeviceClassGet(L7_uint32 *deviceClass)
{
  return lldpXMedLocDeviceClassGet(deviceClass);
}
/*********************************************************************
*
* @purpose  Gets the supported capabilities that could be tranamitted in MED TLVs
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    lldpXMedCapabilities_t     *capSup @b((output))  bit mask of the capabilities supported
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedPortCapSupportedGet(L7_uint32 intIfNum, lldpXMedCapabilities_t *capSup)
{
  return lldpXMedPortCapSupportedGet(intIfNum, capSup);
}
/*********************************************************************
*
* @purpose  Sets the TLVs that need to be transmitted in the LLDP PDU
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    lldpXMedCapabilities_t     *capSet @b((output))  Bit mask of the TLVs that will be transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedPortConfigTLVsEnabledSet(L7_uint32 intIfNum, lldpXMedCapabilities_t *capSet)
{
  return lldpXMedPortConfigTLVsEnabledSet(intIfNum, capSet);
}
/*********************************************************************
*
* @purpose  Gets the TLVs that need to be transmitted in the LLDP PDU
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    lldpXMedCapabilities_t     *capGet@b((input))  Bit mask of TLVs that are set for transmision
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedPortConfigTLVsEnabledGet(L7_uint32 intIfNum, lldpXMedCapabilities_t *capGet)
{
  return lldpXMedPortConfigTLVsEnabledGet(intIfNum, capGet);
}
/*********************************************************************
*
* @purpose  Sets the Topology Change notification for this interface
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL                   notif @b((output))  Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedPortConfigNotifEnableSet(L7_uint32 intIfNum, L7_BOOL notif)
{
  return lldpXMedPortConfigNotifEnableSet(intIfNum, notif);
}
/*********************************************************************
*
* @purpose  Gets the Topology Change notification for this interface
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL                   *notif @b((input))  Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedPortConfigNotifEnableGet(L7_uint32 intIfNum, L7_BOOL *notif)
{
  return lldpXMedPortConfigNotifEnableGet(intIfNum, notif);
}
/*********************************************************************
*
* @purpose  Sets the fast start repeat count 
*
* @param    L7_uint32                  count @b((input))  fast start repeat count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedFastStartRepeatCountSet(L7_uint32 count)
{
  return lldpXMedFastStartRepeatCountSet(count);
}/*********************************************************************
*
* @purpose  Gets the fast start repeat count 
*
* @param    L7_uint32                   *count @b((output)) fast start repeat count 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedFastStartRepeatCountGet(L7_uint32 *count)
{
  return lldpXMedFastStartRepeatCountGet(count);
}
/*********************************************************************
 *                LLDP-MED Local Device APIs
 *********************************************************************/
/*********************************************************************
*
* @purpose  Gets the vlan id associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_uint32                  *vlanId @b((output))  Vlan ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocMediaPolicyVlanIDGet(L7_uint32 intIfNum, 
											 L7_uint32 policyAppType, 
											 L7_uint32 *vlanId)
{
  return lldpXMedLocMediaPolicyVlanIDGet(intIfNum, policyAppType, vlanId);
}
/*********************************************************************
*
* @purpose  Gets the priority associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_uint32                  *priority @b((output))  Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocMediaPolicyPriorityGet(L7_uint32 intIfNum, 
											   L7_uint32 policyAppType, 
											   L7_uint32 *priority )
{
  return lldpXMedLocMediaPolicyPriorityGet(intIfNum, policyAppType, priority);
}
/*********************************************************************
*
* @purpose  Gets the DSCP associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_uint32                  *dscp @b((output))  dscp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocMediaPolicyDscpGet(L7_uint32 intIfNum, 
										   L7_uint32 policyAppType, 
										   L7_uint32 *dscp )
{
  return lldpXMedLocMediaPolicyDscpGet(intIfNum, policyAppType, dscp);
}
/*********************************************************************
*
* @purpose  Gets the Unknown bit associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_BOOL                  *unknown @b((output))  unknown bit value (0/1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocMediaPolicyUnknownGet(L7_uint32 intIfNum, 
											  L7_uint32 policyAppType, 
											  L7_BOOL *unknown )
{
  return lldpXMedLocMediaPolicyUnknownGet(intIfNum, policyAppType, unknown);
}
/*********************************************************************
*
* @purpose  Gets the tagged bit associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_BOOL                  *tagged @b((output))  tagged bit value (0/1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocMediaPolicyTaggedGet(L7_uint32 intIfNum, 
											 L7_uint32 policyAppType, 
											 L7_BOOL *tagged )
{
  return lldpXMedLocMediaPolicyTaggedGet(intIfNum, policyAppType, tagged);
}
/*********************************************************************
*
* @purpose  Gets the Hardware revision
*
* @param    L7_uchar8                    *hardwareRev @b((output)) Hardware revision as a string 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocHardwareRevGet(L7_uchar8 *hardwareRev)
{
  return lldpXMedLocHardwareRevGet(hardwareRev);
}
/*********************************************************************
*
* @purpose  Gets the Firmware revision
*
* @param    L7_uchar8                    *firmwareRev @b((output)) Firmware revision as a string 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocFirmwareRevGet(L7_uchar8 *firmwareRev)
{
  return lldpXMedLocFirmwareRevGet(firmwareRev);
}
/*********************************************************************
*
* @purpose  Gets the Software revision
*
* @param    L7_uchar8                    *SoftwareRev @b((output)) Software revision as a string 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocSoftwareRevGet(L7_uchar8 *softwareRev)
{
  return lldpXMedLocSoftwareRevGet(softwareRev);
}
/*********************************************************************
*
* @purpose  Gets the Serial Number
*
* @param    L7_uchar8                    *serialNum @b((output)) Serial Number as a string 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocSerialNumGet(L7_uchar8 *serialNum)
{
  return lldpXMedLocSerialNumGet(serialNum);
}
/*********************************************************************
*
* @purpose  Gets the Manufacturer's Name 
*
* @param    L7_uchar8                    *mfgName @b((output)) Manufacturer's Name as a string 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocMfgNameGet(L7_uchar8 *mfgName)
{
  return lldpXMedLocMfgNameGet(mfgName);
}
/*********************************************************************
*
* @purpose  Gets the Model Name
*
* @param    L7_uchar8                    *modelName @b((output)) Model Name as a string 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocModelNameGet(L7_uchar8 *modelName)
{
  return lldpXMedLocModelNameGet(modelName);
}
/*********************************************************************
*
* @purpose  Gets the Asset ID
*
* @param    L7_uchar8                    *assetID @b((output)) Asset ID as a string 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocAssetIDGet(L7_uchar8 *assetID)
{
  return lldpXMedLocAssetIDGet(assetID);
}
/*********************************************************************
*
* @purpose  Sets the location information as a string for given type of Location ID
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  subType  @b((input)) Type of location information (unkbown, ELIN, Co-ord, civic address)
* @param    L7_uint32                  *info @b((input)) Actual location information 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocLocationInfoSet(L7_uint32 intIfNum, 
										L7_uint32 subType, 
										L7_uchar8 *info)
{
  return lldpXMedLocLocationInfoSet(intIfNum, subType, info);
}
/*********************************************************************
*
* @purpose  Gets the location information as a string for given type of Location ID
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  subType  @b((input)) Type of location information (unkbown, ELIN, Co-ord, civic address)
* @param    L7_uint32                  *info @b((output)) Actual location information 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocLocationInfoGet(L7_uint32 intIfNum, 
										L7_uint32 subType, 
										L7_uchar8 *info)
{
  return lldpXMedLocLocationInfoGet(intIfNum, subType, info);
}
/*********************************************************************
*
* @purpose  Gets the PoE device type. 
*
* @param    L7_uint32                *deviceType   @b((output))  Type of power device
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocXPoeDeviceTypeGet(L7_uint32 intIfNum,L7_uint32 *deviceType)
{
  return lldpXMedLocXPoeDeviceTypeGet(intIfNum,deviceType);
}
/*********************************************************************
*
* @purpose  Gets the PSE power value in tenths of watts
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  *powerAv @b((output))  Power value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is the amount of power available by the power source device.
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocXPoePSEPortPowerAvGet(L7_uint32 intIfNum, L7_uint32 *powerAv)
{
  return lldpXMedLocXPoePSEPortPowerAvGet(intIfNum, powerAv);
}
/*********************************************************************
*
* @purpose  Gets the PSE port power priority
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  *priority @b((output)) priority 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocXPoePSEPortPDPriorityGet(L7_uint32 intIfNum, L7_uint32 *priority)
{
  return lldpXMedLocXPoePSEPortPriorityGet(intIfNum, priority);
}
/*********************************************************************
*
* @purpose  Gets the power source of this port
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  *powerSource @b((output)) type of power source 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocXPoePSEPowerSourceGet(L7_uint32 *powerSource)
{
  return lldpXMedLocXPoePSEPowerSourceGet(powerSource);
}
/*********************************************************************
*
* @purpose  Gets this devices PD power requirement
*
* @param    L7_uint32                  *pdPowerReq @b((output))  If this device is a PD then its requirement
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocXPoePDPowerReqGet(L7_uint32 intIfNum,L7_uint32 *pdPowerReq)
{
  return lldpXMedLocXPoePDPowerReqGet(intIfNum,pdPowerReq);
}
/*********************************************************************
*
* @purpose   Gets this device's PD source
*
* @param    L7_uint32                  *powerSource @b((output))  Source of the PD
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocXPoePDPowerSouceGet(L7_uint32 intIfNum,L7_uint32 *powerSource)
{
  return lldpXMedLocXPoePDPowerSourceGet(intIfNum,powerSource);
}
/*********************************************************************
*
* @purpose  Gets this device's PD priority
*
* @param    L7_uint32                  *priority @b((output))  Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedLocXPoePDPowerPriorityGet(L7_uint32 intIfNum,L7_uint32 *priority)
{
  return lldpXMedLocXPoePDPowerPriorityGet(intIfNum,priority);
}
/*********************************************************************
*
* @purpose  Gets this port's auto nego capabilities
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL                   *support @b((output))  Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPortAutoNegSupportedGet(L7_uint32 intIfNum, L7_BOOL *support)
{
  return lldpXdot3LocPortAutoNegSupportedGet(intIfNum, support);
}
/*********************************************************************
*
* @purpose   Gets this port's auto nego status
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL                  *enabled @b((output))  Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPortAutoNegEnabledGet(L7_uint32 intIfNum, L7_BOOL *enabled)
{
  return lldpXdot3LocPortAutoNegEnabledGet(intIfNum, enabled);
}
/*********************************************************************
*
* @purpose  Gets this port's auto nego advertized capability
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                   *advrtCap @b((output))  String representation of the capability
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPortAutoNegAdvertizedCapGet(L7_uint32 intIfNum, L7_uchar8 *advrtCap)
{
  return lldpXdot3LocPortAutoNegAdvertizedCapGet(intIfNum, advrtCap);
}
/*********************************************************************
*
* @purpose  Gets this port's auto nego operational MAU type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  *type @b((output))  MAU Type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPortAutoNegOperMauTypeGet(L7_uint32 intIfNum, L7_uint32 *type)
{
  return lldpXdot3LocPortAutoNegOperMauTypeGet(intIfNum, type);
}

/* 802.3 Power MDI Local Data API */
/*********************************************************************
*
* @purpose  Gets Local Device Power Class 
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                   portClass @b((input)) Pointer to Port Class
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerPortClassGet(L7_uint32 intIfNum, L7_BOOL *portClass)
{
  return lldpXdot3LocPowerPortClassGet(intIfNum,portClass);
}

/*********************************************************************
*
* @purpose  Gets Local Device MDISupported value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                 mdiSupported @b((input)) Pointer to MDI Supported 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerMDISupportedGet(L7_uint32 intIfNum, L7_BOOL *mdiSupported)
{
  return lldpXdot3LocPowerMDISupportedGet(intIfNum,mdiSupported);
}

/*********************************************************************
*
* @purpose  Gets Local Device MDI Enable value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                 mdiEnabled @b((input)) Pointer to MDI Enabled 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerMDIEnabledGet(L7_uint32 intIfNum, L7_BOOL *mdiEnabled)
{
  return lldpXdot3LocPowerMDIEnabledGet(intIfNum,mdiEnabled);
}

/*********************************************************************
*
* @purpose  Gets Local Device Power Pair contrallable value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                 pairControlable @b((input)) pairControlable 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerPairControlableGet(L7_uint32 intIfNum, L7_BOOL *pairControlable)
{
  return lldpXdot3LocPowerPairControlableGet(intIfNum,pairControlable);
}

/*********************************************************************
*
* @purpose  Gets Local Device Power Pairs Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               pairs@b((input)) Pointer to power pairs 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerPairsGet(L7_uint32 intIfNum, L7_uint32 *pairs)
{
  return lldpXdot3LocPowerPairsGet(intIfNum,pairs);
}

/*********************************************************************
*
* @purpose  Gets Local Device Detected Power Class Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerClass @b((input)) Pointer to powerClass 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerClassGet(L7_uint32 intIfNum, L7_uint32 *powerClass)
{
  return lldpXdot3LocPowerClassGet(intIfNum,powerClass);
}

/*********************************************************************
*
* @purpose  Gets Local Device Power Type Value
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerType @b((input)) Pointer to power type 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerTypeGet(L7_uint32 intIfNum, L7_uint32 *powerType)
{
  return lldpXdot3LocPowerTypeGet(intIfNum,powerType);
}

/*********************************************************************
*
* @purpose  Gets Local Device Power Source Value
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerSource @b((input)) Pointer to powerSource 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerSourceGet(L7_uint32 intIfNum, L7_uint32 *powerSource)
{
  return lldpXdot3LocPowerSourceGet(intIfNum,powerSource);
}

/*********************************************************************
*
* @purpose  Gets Local Device Power Priority Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerPriority @b((input)) Pointer to power priority 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerPriorityGet(L7_uint32 intIfNum, L7_uint32 *powerPriority)
{
  return lldpXdot3LocPowerPriorityGet(intIfNum,powerPriority);
}

/*********************************************************************
*
* @purpose  Gets Local Device PD Requested power value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerValue @b((input)) Pointer to PD Requested power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPDRequestedPowerValueGet(L7_uint32 intIfNum, L7_uint32 *powerValue)
{
  return lldpXdot3LocPDRequestedPowerValueGet(intIfNum,powerValue);
}

/*********************************************************************
*
* @purpose  Gets Local Device PSE Allocated Power Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerValue @b((input)) Pointer to PSE allocated power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPSEAllocatedPowerValueGet(L7_uint32 intIfNum, L7_uint32 *powerValue)
{
  return lldpXdot3LocPSEAllocatedPowerValueGet(intIfNum,powerValue);
}

/*********************************************************************
*
* @purpose  Gets Local Device Response Time 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               responseTime @b((input)) Pointer to Response time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocResponseTime(L7_uint32 intIfNum, L7_uint32 *responseTime)
{
  return lldpXdot3LocResponseTime(intIfNum,responseTime);
}

/*********************************************************************
*
* @purpose  Gets Local device readiness 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                 ready @b((input)) Pointer to readiness 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocReadyGet(L7_uint32 intIfNum, L7_BOOL *ready)
{
  return lldpXdot3LocReadyGet(intIfNum,ready);
}

/*********************************************************************
*
* @purpose  Gets Local Device Reduced Operation Power Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               reducedPowerValue @b((input)) Pointer to reduced power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocReducedOperationPowerValueGet(L7_uint32 intIfNum, L7_uint32 *reducedPowerValue)
{
  return lldpXdot3LocReducedOperationPowerValueGet(intIfNum,reducedPowerValue);
}

/*********************************************************************
*
* @purpose  Sets the Local Device Power Priority 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uchar8                powerPriority@b((input)) Power priority  value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3LocPowerPrioritySet(L7_uint32 intIfNum, L7_uint32 powerPriority)
{
  return lldpXdot3LocPowerPrioritySet(intIfNum,powerPriority);
}

/*********************************************************************
 *                LLDP-MED Remote Device APIs
 *********************************************************************/
/*********************************************************************
*
* @purpose  Gets the supported capabilities that was received in MED TLV on this port
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *cap@b((output))  Bit mask of the capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemCapSupportedGet(L7_uint32 intIfNum, 
										L7_uint32 remIndex, 
										L7_uint32 timestamp, 
										lldpXMedCapabilities_t *cap)
{
  return lldpXMedRemCapSupportedGet(intIfNum, remIndex, timestamp, cap);
} 
/*********************************************************************
*
* @purpose  Gets the enabled capabilities that was received in MED TLV on this port
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *cap@b((output))  Bit mask of capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemCapCurrentGet(L7_uint32 intIfNum, 
									  L7_uint32 remIndex, 
									  L7_uint32 timestamp, 
									  lldpXMedCapabilities_t *cap)
{
  return lldpXMedRemCapCurrentGet(intIfNum, remIndex, timestamp, cap);
} 
/*********************************************************************
*
* @purpose  Gets the remote devices MED class
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *deviceClass @b((output))  MED class of the remote device
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemDeviceClassGet(L7_uint32 intIfNum, 
									   L7_uint32 remIndex, 
									   L7_uint32 timestamp, 
									   L7_uint32 *deviceClass)
{
  return lldpXMedRemDeviceClassGet(intIfNum, remIndex, timestamp, deviceClass);
} 
/*********************************************************************
*
* @purpose  Gets the vlan id associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_uint32                  *vlanId @b((output))  Vlan ID
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemMediaPolicyVlanIdGet(L7_uint32 intIfNum, 
											 L7_uint32 remIndex, 
											 L7_uint32 timestamp, 
											 L7_uint32 policyAppType, 
											 L7_uint32 *vlanId)
{
  return lldpXMedRemMediaPolicyVlanIdGet(intIfNum, remIndex, timestamp, policyAppType, vlanId);
} 
/*********************************************************************
*
* @purpose  Gets the priority associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_uint32                  *priority @b((output))  Priority
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemMediaPolicyPriorityGet(L7_uint32 intIfNum, 
											   L7_uint32 remIndex, 
											   L7_uint32 timestamp, 
											   L7_uint32 policyAppType, 
											   L7_uint32 *priority)
{
  return lldpXMedRemMediaPolicyPriorityGet(intIfNum, remIndex, timestamp, policyAppType, priority);
} 
/*********************************************************************
*
* @purpose  Gets the DSCP associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_uint32                  *dscp @b((output))  dscp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemMediaPolicyDscpGet(L7_uint32 intIfNum, 
										   L7_uint32 remIndex, 
										   L7_uint32 timestamp, 
										   L7_uint32 policyAppType, 
										   L7_uint32 *dscp)
{
  return lldpXMedRemMediaPolicyDscpGet(intIfNum, remIndex, timestamp, policyAppType, dscp);
}
/*********************************************************************
*
* @purpose  Gets the Unknown bit associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_BOOL                  *unknown @b((output))  unknown bit value (0/1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemMediaPolicyUnknownGet(L7_uint32 intIfNum, 
											  L7_uint32 remIndex, 
											  L7_uint32 timestamp, 
											  L7_uint32 policyAppType, 
											  L7_BOOL *unknown)
{
  return lldpXMedRemMediaPolicyUnknownGet(intIfNum, remIndex, timestamp, policyAppType, unknown);
} 
/*********************************************************************
*
* @purpose  Gets the tagged bit associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param	L7_uint32				   policyAppType @b((input)) type application policy
* @param    L7_BOOL                  *tagged @b((output))  tagged bit value (0/1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemMediaPolicyTaggedGet(L7_uint32 intIfNum, 
											 L7_uint32 remIndex, 
											 L7_uint32 timestamp, 
											 L7_uint32 policyAppType, 
											 L7_BOOL *tagged)
{
  return lldpXMedRemMediaPolicyTaggedGet(intIfNum, remIndex, timestamp, policyAppType, tagged);
} 
/*********************************************************************
*
* @purpose  Gets the remote device's Hardware Revision
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *hardwareRev @b((output))  Hardware revision as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemHardwareRevGet(L7_uint32 intIfNum, 
									   L7_uint32 remIndex, 
									   L7_uint32 timestamp, 
									   L7_uchar8 *hardwareRev)
{
  return lldpXMedRemHardwareRevGet(intIfNum, remIndex, timestamp, hardwareRev);
}
/*********************************************************************
*
* @purpose  Gets the remote device's Firmware Revision
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *firmwareRev @b((output))  Firmware revision as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemFirmwareRevGet(L7_uint32 intIfNum, 
									   L7_uint32 remIndex, 
									   L7_uint32 timestamp, 
									   L7_uchar8 *firmwareRev)
{
  return lldpXMedRemFirmwareRevGet(intIfNum, remIndex, timestamp, firmwareRev);
}
/*********************************************************************
*
* @purpose  Gets the remote device's Softdware Revision
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *softwareRev @b((output))  Software revision as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemSoftwareRevGet(L7_uint32 intIfNum, 
									   L7_uint32 remIndex, 
									   L7_uint32 timestamp, 
									   L7_uchar8 *softwareRev)
{
  return lldpXMedRemSoftwareRevGet(intIfNum, remIndex, timestamp, softwareRev);
}
/*********************************************************************
*
* @purpose  Gets the remote device's Hardware Revision
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *serialNum @b((output))  Serial Number as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemSerialNumGet(L7_uint32 intIfNum, 
									 L7_uint32 remIndex, 
									 L7_uint32 timestamp, 
									 L7_uchar8 *serialNum)
{
  return lldpXMedRemSerialNumGet(intIfNum, remIndex, timestamp, serialNum);
}
/*********************************************************************
*
* @purpose  Gets the remote device's Manufacturer's Name
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *mfgName @b((output))  Manufacturer's name as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemMfgNameGet(L7_uint32 intIfNum, 
								   L7_uint32 remIndex, 
								   L7_uint32 timestamp, 
								   L7_uchar8 *mfgName)
{
  return lldpXMedRemMfgNameGet(intIfNum, remIndex, timestamp, mfgName);
}
/*********************************************************************
*
* @purpose  Gets the remote device's Model Name
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *modelName @b((output))  Model Name as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemModelNameGet(L7_uint32 intIfNum, 
									 L7_uint32 remIndex, 
									 L7_uint32 timestamp, 
									 L7_uchar8 *modelName)
{
  return lldpXMedRemModelNameGet(intIfNum, remIndex, timestamp, modelName);
}
/*********************************************************************
*
* @purpose  Gets the remote device's Asset ID
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *assetID @b((output))  Asset ID as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemAssetIdGet(L7_uint32 intIfNum, 
								   L7_uint32 remIndex, 
								   L7_uint32 timestamp, 
								   L7_uchar8 *assetId)
{
  return lldpXMedRemAssetIdGet(intIfNum, remIndex, timestamp, assetId);
}
/*********************************************************************
*
* @purpose  Gets the location info received on this port for the given location subtype
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   subType @b((input))  Loaction subtype 
* @param    L7_uchar8                  *info @b((output)) Location information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemLocationInfoGet(L7_uint32 intIfNum, 
										L7_uint32 remIndex, 
										L7_uint32 timestamp, 
										L7_uint32 subType, 
										L7_uchar8 *info)
{
  return lldpXMedRemLocationInfoGet(intIfNum, remIndex, timestamp, subType, info);
}
/*********************************************************************
*
* @purpose  Gets the remote device's PoE device type connected to this port
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *deviceType @b((output))  PoE device type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemXPoeDeviceTypeGet(L7_uint32 intIfNum, 
										  L7_uint32 remIndex, 
										  L7_uint32 timestamp, 
										  L7_uint32 *deviceType)
{
  return lldpXMedRemXPoeDeviceTypeGet(intIfNum, remIndex, timestamp, deviceType);
}
/*********************************************************************
*
* @purpose  Gets the remote ports PSE power value in tenths of watts
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *powerAv @b((output))  Power Available
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemXPoePSEPowerAvGet(L7_uint32 intIfNum, 
										  L7_uint32 remIndex, 
										  L7_uint32 timestamp, 
										  L7_uint32 *powerAv)
{
  return lldpXMedRemXPoePSEPowerAvGet(intIfNum, remIndex, timestamp, powerAv);
}
/*********************************************************************
*
* @purpose  Gets the remote ports PSE power source
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *powerSrc@b((output)) Power Soruce type 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemXPoePSEPowerSrcGet(L7_uint32 intIfNum, 
										   L7_uint32 remIndex, 
										   L7_uint32 timestamp, 
										   L7_uint32 *powerSrc)
{
  return lldpXMedRemXPoePSEPowerSrcGet(intIfNum, remIndex, timestamp, powerSrc);
}
/*********************************************************************
*
* @purpose  Gets the remote ports PSE power priority
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *powerPri	@b((output)) priority  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemXPoePSEPowerPriGet(L7_uint32 intIfNum, 
										   L7_uint32 remIndex, 
										   L7_uint32 timestamp, 
										   L7_uint32 *powerPri)
{
  return lldpXMedRemXPoePSEPowerPriGet(intIfNum, remIndex, timestamp, powerPri);
}
/*********************************************************************
*
* @purpose  Gets the remote port's PD power requirement 
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *powerReq @b((output)) Power requirement 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemXPoePDPowerReqGet(L7_uint32 intIfNum, 
										  L7_uint32 remIndex, 
										  L7_uint32 timestamp, 
										  L7_uint32 *powerReq)
{
  return lldpXMedRemXPoePDPowerReqGet(intIfNum, remIndex, timestamp, powerReq);
}
/*********************************************************************
*
* @purpose  Gets the remote port's PD power Source
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *powerSrc @b((output))  Power source type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemXPoePDPowerSrcGet(L7_uint32 intIfNum, 
										  L7_uint32 remIndex, 
										  L7_uint32 timestamp, 
										  L7_uint32 *powerSrc)
{
  return lldpXMedRemXPoePDPowerSrcGet(intIfNum, remIndex, timestamp, powerSrc);
}
/*********************************************************************
*
* @purpose  Gets the remote port's PD power priority
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *powerPri @b((output))  power priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXMedRemXPoePDPowerPriGet(L7_uint32 intIfNum, 
										  L7_uint32 remIndex, 
										  L7_uint32 timestamp, 
										  L7_uint32 *powerPri)
{
  return lldpXMedRemXPoePDPowerPriGet(intIfNum, remIndex, timestamp, powerPri);
}
/*********************************************************************
 *                LLDP 802.3 Extension Configuration APIs
 *********************************************************************/
/*********************************************************************
*
* @purpose  Gets remote port's auto nego capability
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL                   *support@b((output)) Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPortAutoNegSupportedGet(L7_uint32 intIfNum, 
												 L7_uint32 remIndex, 
												 L7_uint32 timestamp, 
												 L7_BOOL *support)
{
  return lldpXdot3RemPortAutoNegSupportedGet(intIfNum, remIndex, timestamp, support);
}
/*********************************************************************
*
* @purpose  Gets remote port's auto nego enabled status
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL                   *enabled@b((output)) Truth value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPortAutoNegEnabledGet(L7_uint32 intIfNum, 
											   L7_uint32 remIndex, 
											   L7_uint32 timestamp, 
											   L7_BOOL *enabled)
{
  return lldpXdot3RemPortAutoNegEnabledGet(intIfNum, remIndex, timestamp, enabled);
}
/*********************************************************************
*
* @purpose  Gets remote ports auto nego advertized capability as a string
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *advrtCap @b((output))  Advertized capability
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPortAutoNegAdvertizedCapGet(L7_uint32 intIfNum, 
													 L7_uint32 remIndex, 
													 L7_uint32 timestamp, 
													 L7_uchar8 *advrtCap)
{
  return lldpXdot3RemPortAutoNegAdvertizedCapGet(intIfNum, remIndex, timestamp, advrtCap);
}
/*********************************************************************
*
* @purpose  Gets the remote port's Operational MAU Type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex	 @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *type @b((output))  MAU Type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPortAutoNegOperMauTypeGet(L7_uint32 intIfNum, 
												   L7_uint32 remIndex, 
												   L7_uint32 timestamp, 
												   L7_uint32 *type)
{
  return lldpXdot3RemPortAutoNegOperMauTypeGet(intIfNum, remIndex, timestamp, type);
}

/* 802.3 Power MDI Remote Data API */
/*********************************************************************
*
* @purpose  Gets Remote Device Power Class 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL*                 portClass @b((input)) Pointer to Port Class
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPowerPortClassGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp,L7_BOOL *portClass)
{
  return lldpXdot3RemPowerPortClassGet(intIfNum,remIndex,timestamp,portClass);
}

/*********************************************************************
*
* @purpose  Gets Remote Device MDISupported value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL*                 mdiSupported @b((input)) Pointer to MDI Supported 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPowerMDISupportedGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_BOOL *mdiSupported)
{
  return lldpXdot3RemPowerMDISupportedGet(intIfNum,remIndex,timestamp,mdiSupported);
}

/*********************************************************************
*
* @purpose  Gets Remote Device MDI Enable value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL*                 mdiEnabled @b((input)) Pointer to MDI Enabled 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPowerMDIEnabledGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_BOOL *mdiEnabled)
{
  return lldpXdot3RemPowerMDIEnabledGet(intIfNum,remIndex,timestamp,mdiEnabled);
}

/*********************************************************************
*
* @purpose  Gets Remote Device Power Pair contrallable value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL*                 pairControlable @b((input)) pairControlable 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPowerPairControlableGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_BOOL *pairControlable)
{
  return lldpXdot3RemPowerPairControlableGet(intIfNum,remIndex,timestamp,pairControlable);
}

/*********************************************************************
*
* @purpose  Gets Remote Device Power Pairs Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               pairs@b((input)) Pointer to power pairs 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPowerPairsGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_uint32 *pairs)
{
  return lldpXdot3RemPowerPairsGet(intIfNum,remIndex,timestamp,pairs);
}

/*********************************************************************
*
* @purpose  Gets Remote Device Detected Power Class Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerClass @b((input)) Pointer to powerClass 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPowerClassGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_uint32 *powerClass)
{
  return lldpXdot3RemPowerClassGet(intIfNum,remIndex,timestamp,powerClass);
}

/*********************************************************************
*
* @purpose  Gets Remote Device Power Type Value
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerType @b((input)) Pointer to power type 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPowerTypeGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                                 L7_uint32 timestamp,L7_uint32 *powerType)
{
  return lldpXdot3RemPowerTypeGet(intIfNum,remIndex,timestamp,powerType);
}

/*********************************************************************
*
* @purpose  Gets Remote Device Power Source Value
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerSource @b((input)) Pointer to powerSource 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPowerSourceGet(L7_uint32 intIfNum,L7_uint32 remIndex,
                                 L7_uint32 timestamp, L7_uint32 *powerSource)
{
  return lldpXdot3RemPowerSourceGet(intIfNum,remIndex,timestamp,powerSource);
}

/*********************************************************************
*
* @purpose  Gets Remote Device Power Priority Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerPriority @b((input)) Pointer to power priority 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPowerPriorityGet(L7_uint32 intIfNum,L7_uint32 remIndex,
                                 L7_uint32 timestamp, L7_uint32 *powerPriority)
{
  return lldpXdot3RemPowerPriorityGet(intIfNum,remIndex,timestamp,powerPriority);
}

/*********************************************************************
*
* @purpose  Gets Remote Device PD Requested power value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerValue @b((input)) Pointer to PD Requested power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPDRequestedPowerValueGet(L7_uint32 intIfNum,L7_uint32 remIndex,
                                 L7_uint32 timestamp, L7_uint32 *powerValue)
{
  return lldpXdot3RemPDRequestedPowerValueGet(intIfNum,remIndex,timestamp,powerValue);
}

/*********************************************************************
*
* @purpose  Gets Remote Device PSE Allocated Power Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerValue @b((input)) Pointer to PSE allocated power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpXdot3RemPSEAllocatedPowerValueGet(L7_uint32 intIfNum,L7_uint32 remIndex,
                                 L7_uint32 timestamp, L7_uint32 *powerValue)
{
  return lldpXdot3RemPSEAllocatedPowerValueGet(intIfNum,remIndex,timestamp,powerValue);
}


/*********************************************************************
*
* @purpose  Get a string representation for Device Class
*
* @param    lldpXMedDeviceClass_t type   @b((input))  Device Class
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpXMedDeviceClassString(lldpXMedDeviceClass_t type)
{
  switch (type)
  {
	case notDefined:
	  return "Not Defined";
	case endpointClass1:
	  return "Endpoint Class I";
	case endpointClass2:
	  return "Endpoint Class II";
	case endpointClass3:
	  return "Endpoint Class III";
	case networkConnectivity:
	  return "Network Connectivity";
	default:
	  return "Error";
  }
}
/*********************************************************************
*
* @purpose  Get a string representation for Media Capabilities
*
* @param    lldpXMedCapabilitiesValue_t value   @b((input))  Media Capabilities
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpXMedMediaCapabilitiesString(lldpXMedCapabilitiesValue_t value)
{
  switch (value)
  {
	case capabilities:
	  return "Capabilities";
	case networkPolicy:
	  return "Network Policy";
	case location:
	  return "Location";
	case extendedPSE:
	  return "Extended PSE";
	case extendedPD:
	  return "Extended PD";
	case inventory:
	  return "Inventory";
	default:
	  return "Error";
  }
}
/*********************************************************************
*
* @purpose  Get a string representation for Location Subtype
*
* @param    lldpXMedLocationSubtype_t type   @b((input))  Location subtype
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpXMedLocationSubtypeString(lldpXMedLocationSubtype_t type)
{
  switch (type)
  {
	case unknown_locsubtype:
	  return "Unknown";
	case coordinateBased_locsubtype:
	  return "Coordinate Based";
	case civicAddress_locsubtype:
	  return "Civic Address";
	case elin_locsubtype:
	  return "ELIN";
	default:
	  return "Error";
  }
}
/*********************************************************************
*
* @purpose  Get a string representation for Network Policy Application type
*
* @param    lldpXMedPolicyAppTypeValue_t type   @b((input))  Policy Application Type
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpXMedPolicyAppTypesString(lldpXMedPolicyAppTypeValue_t type)
{
  switch (type)
  {
	case unknown_policyapptype:
	  return "Unknown";
	case voice_policyapptype:
	  return "Voice";
	case voiceSignaling_policyapptype:
	  return "Voice Signaling";
	case guestVoice_policyapptype:
	  return "Guest Voice";
	case guestVoiceSignaling_policyapptype:
	  return "Guest Voice Signaling";
	case softPhoneVoice_policyapptype:
	  return "Soft Phone Voice";
	case videoConferencing_policyapptype:
	  return "Video Conferencing";
	case streamingVideo_policyapptype:
	  return "Streaming Video";
	case videoSignaling_policyapptype:
	  return "Video Signaling";
	default:
	  return "Error";
  }
}
/*********************************************************************
*
* @purpose  Get a string representation for PoE Decive Type
*
* @param    lldpXMedPoeDeviceType_t type   @b((input))  Device Type
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpXMedPoeDeviceTypeString(lldpXMedPoeDeviceType_t type)
{
  switch (type)
  {
	case unknown_poedevicetype:
	  return "Unknown";
	case pseDevice:
	  return "PSE";
	case pdDevice:
	  return "PD";
	case none_poedevicetype:
	  return "None";
	default:
	  return "Error";
  }
}
/*********************************************************************
*
* @purpose  Get a string representation for PoE Power Priority
*
* @param    lldpXMedPoePowerPriority_t type   @b((input))  Power Priority
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpXMedPoePowerPriorityString(lldpXMedPoePowerPriority_t type)
{
  switch (type)
  {
	case unknown_pwpri:
	  return "Unknown";
	case critical_pwpri:
	  return "Critical";
	case high_pwpri:
	  return "High";
	case low_pwpri:
	  return "Low";
	default:
	  return "Error";
  }
}
/*********************************************************************
*
* @purpose  Get a string representation for PoE PSE Power Source
*
* @param    lldpXMedPsePowerSource_t type   @b((input))  PSE Source
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpXMedPoePsePowerSourceString(lldpXMedPsePowerSource_t type)
{
  switch (type)
  {
	case unknown_psesrc:
	  return "Unknown";
	case primary_psesrc:
	  return "Primary";
	case backup_psesrc:
	  return "Backup";
	default:
	  return "Error";
  }
}
/*********************************************************************
*
* @purpose  Get a string representation for PoE PD Power Source
*
* @param    lldpXMedPdPowerSource_t type   @b((input))  PD Source
*
* @returns  const L7_char8 *
*
* @notes    
*
* @end
*********************************************************************/
const L7_char8 *usmDbLldpXMedPoePdPowerSourceString(lldpXMedPdPowerSource_t type)
{
  switch (type)
  {
	case unknown_pdsrc:
	  return "Unknown";
	case fromPSE_pdsrc:
	  return "From PSE";
	case local_pdsrc:
	  return "Local";
	case localAndPSE_pdsrc:
	  return "Local And PSE";
	default:
	  return "Error";
  }
}
/*********************************************************************
*
* @purpose  Gets whether a particular policy type is supported on this interface
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param	L7_uint32				   policyAppType @b((input)) type application policy
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL usmDbLldpXMedLocMediaPolicyAppSupportedGet(L7_uint32 intIfNum, 
												   L7_uint32 policyAppType)
{
  return lldpXMedLocMediaPolicyAppSupportedGet(intIfNum, policyAppType);
}

/*********************************************************************
*
* @purpose To get the next valid interface enabled for LLDP receives
*
*
* @param   {{input}}unit  unit number
*
* @param   {{input}}intIfNum  interface number
*
* @param   {{output}}nextIntf - pointer to the next valid interface intIfNum
*
* @returns L7_FAILURE
*          L7_SUCCESS when next valid interface found
*
* @end
*
*********************************************************************/
L7_RC_t usmDbLldpRemoteIntfNextGet(L7_uint32 intIfNum, L7_uint32 *nextIntf)
{
  L7_uint32 tempintIfNum, rxMode;

  tempintIfNum = intIfNum;
  /*rc = usmDbLldpValidIntfNextGet(tempintIfNum, &tempintIfNum); */
  while (usmDbLldpValidIntfNextGet(tempintIfNum, &tempintIfNum) == L7_SUCCESS)
  {
    if ((usmDbLldpIntfRxModeGet(tempintIfNum, &rxMode) == L7_SUCCESS)
      && (rxMode == L7_ENABLE))
    {
      *nextIntf = tempintIfNum;
      return L7_SUCCESS;
      /*rc = L7_SUCCESS;
      break;*/
    }
    /*rc = usmDbLldpValidIntfNextGet(tempintIfNum, &tempintIfNum); */
  }
  return L7_FAILURE;
  /*return rc; */
}
/*********************************************************************
*
* @purpose To get the first interface enable for LLDP receives
*
*
* @param   {{input}}unit  unit number
*
* @param   {{output}}firstIntf - pointer to the first valid interface
*
*
* @returns L7_FAILURE
*          L7_SUCCESS when first valid interface found
*
* @end
*
*********************************************************************/
L7_RC_t usmDbLldpRemoteIntfFirstGet(L7_uint32 * firstIntf)
{
  L7_uint32 tempintIfNum, rxMode=0;
  L7_RC_t rc = L7_FAILURE;

  if(usmDbLldpValidIntfFirstGet(&tempintIfNum) == L7_SUCCESS)
  {
    if((usmDbLldpIntfRxModeGet(tempintIfNum, &rxMode) == L7_SUCCESS)
       && (rxMode == L7_ENABLE))
    {
      *firstIntf = tempintIfNum;
      rc = L7_SUCCESS;
    }
    else
    {
      rc = usmDbLldpRemoteIntfNextGet(tempintIfNum, firstIntf);
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose To get the next valid interface enabled for LLDP receives
*
*
*
* @param    L7_uint32  intIfNum  @b((output))  interface number
* @param    L7_uint32 *remIndex  @b((output))  remote data index
* @param    L7_uint32 *timestamp @b((output))  remote data timestamp
*
*
* @returns L7_FAILURE
*          L7_SUCCESS when next valid interface found
*
* @end
*
*********************************************************************/
L7_RC_t usmDbLldpRemoteDbNextGet(L7_uint32 *intIfNum, L7_uint32 *remIndex,L7_uint32 *timestamp)
{
  L7_uint32 tempIntf = *intIfNum;
  
  if(lldpRemEntryGetNext(tempIntf,remIndex,timestamp) == L7_FAILURE)
  {
    *remIndex = 0;
    *timestamp = 0;
    if(usmDbLldpRemoteIntfNextGet(tempIntf,intIfNum) == L7_SUCCESS)
    {
      /* Get the next remIndex */
      lldpRemEntryGetNext(*intIfNum,remIndex,timestamp);
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose To find first valid remote entry for any entry 
*
* @param   none
* @param   {{output}}nextIntf - pointer to the next valid interface intIfNum
*
* @returns L7_FAILURE or L7_SUCCESS if remote entry is present
*
* @end
*
*********************************************************************/
L7_RC_t usmDbLldpRemoteEntryFirstGet()
{
   L7_uint32 intIfNum=0, remIndex=0, timeStamp=0;

   if(usmDbLldpRemoteIntfFirstGet(&intIfNum) == L7_SUCCESS)
   {
     if(usmDbLldpRemEntryGet(intIfNum, &remIndex, &timeStamp) == L7_SUCCESS)
     {
       return L7_SUCCESS;
     }
   }
   return L7_FAILURE;
}
/*********************************************************************
*
* @purpose Check to see if PoE is supported on this platform.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    pointer to intIfNum,
*                       (@b{Returns: intIfNum})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIsLldpPoeSupported(L7_uint32 UnitIndex)
{
  return ((cnfgrIsComponentPresent(L7_POE_COMPONENT_ID))? L7_SUCCESS:L7_FAILURE);
}
/*********************************************************************
* @purpose  Determine if the interface is valid and capable of supporting poe
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpPoeIsValidIntf(L7_uint32 intIfNum)
{
  return lldpPoeIsValidIntf(intIfNum);
}
/*********************************************************************
* @purpose  Determine if the interface is valid and capable of supporting poe PSE
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbLldpPoeIsValidPSEIntf(L7_uint32 intIfNum)
{
  return lldpPoeIsValidPSEIntf(intIfNum);
}
/*********************************************************************
* @purpose  Determine if the interface is capable of supporting poe PD.
*
* @param    intIfNum    Internal    Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
* @end
*********************************************************************/
L7_RC_t usmDbLldpPoeIsValidPDIntf(L7_uint32 intIfNum)
{
  return lldpPoeIsValidPDIntf(intIfNum);
}
/*********************************************************************
*
* @purpose  Get next Info associated with this port given a specific OUI and Subtype 
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   Remote Index
* @param    L7_uint32 timestamp  @b((input))   Timestamp for this entry
* @param    L7_uint32 *infoIndex  @b((input))  Pointer to InfoIndex for this entry
* @param    L7_uchar8     *oui   @b((input))   Pointer to the OUI
* @param    L7_uint32 *subtype   @b((input))   Pointer to the Subtype
* @param    L7_uchar8    *info   @b((output))   Pointer To the Info
* @param    L7_uint32 *length    @b((output))   Pointer To the Length
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    Starting with the incoming OUI and Subtype this routine returns the next 
*           numerically largest OUI/Subtype combination.  
*
* @end
*********************************************************************/
L7_RC_t usmdbLldpRemOrgDefEntryInfoGetNext(L7_uint32    intIfNum,
                                      L7_uint32    remIndex,
                                      L7_uint32    timestamp,
                                      L7_uint32    *infoIndex,
                                      L7_uchar8     *oui,
                                      L7_uint32    *subtype,
                                      L7_uchar8   *info,
                                      L7_uint32 *length)
{
  return lldpRemOrgDefEntryInfoGetNext(intIfNum,remIndex,timestamp,infoIndex,oui,subtype,info,length);
}
/*********************************************************************
*
* @purpose  Get Info associated with this port with a specific OUI and Subtype 
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   Remote Index
* @param    L7_uint32 timestamp  @b((input))   Timestamp for this entry
* @param    L7_uint32 infoIndex  @b((input))   InfoIndex for this entry
* @param    L7_uchar8     *oui   @b((input))   Pointer to the OUI
* @param    L7_uint32 *subtype   @b((input))   Pointer to the Subtype
* @param    L7_uchar8    *info   @b((output))   Pointer To the Info
* @param    L7_uint32 *length    @b((output))   Pointer To the Length
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    If incoming OUI and subtype is zero then this routine gets the 
*           numerically smallest OUI/Subtype combination.  
*
* @end
*********************************************************************/
L7_RC_t usmdbLldpRemOrgDefEntryInfoGet(L7_uint32    intIfNum,
                                  L7_uint32    remIndex,
                                  L7_uint32    timestamp,
                                  L7_uint32    infoIndex,
                                  L7_uchar8     *oui,
                                  L7_uint32    *subtype,
                                  L7_uchar8    *info,
                                  L7_uint32  *length)
{
  return lldpRemOrgDefEntryInfoGet(intIfNum,remIndex,timestamp,infoIndex,oui,subtype,info,length);
}

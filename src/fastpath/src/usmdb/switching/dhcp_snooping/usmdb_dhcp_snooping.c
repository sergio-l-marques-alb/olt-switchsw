/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  usmdb_dhcp_snooping.c
*
* @purpose   DHCP snooping APIs
*
* @component DHCP snooping
*
* @comments 
*
* @create 3/16/2007
*
* @author Rob Rice (rrice)
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "usmdb_dhcp_snooping.h"

#include "dhcp_snooping_api.h"
#ifdef L7_IPSG_PACKAGE
#include "ds_ipsg.h"
#endif
#include "ds_util.h"


/*********************************************************************
* @purpose  Gets the DHCP Snooping Admin mode
*
* @param    adminMode   @b((output)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsAdminModeGet(L7_uint32 *adminMode)
{
  return dsAdminModeGet(adminMode);
}

/*********************************************************************
* @purpose  Sets the DHCP Snooping admin mode
*
* @param    adminMode   @b((input)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsAdminModeSet(L7_uint32 adminMode)
{
  return dsAdminModeSet(adminMode);
}

#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
* @purpose  Gets the DHCP L2 Relay Admin mode
*
* @param    adminMode   @b((output)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayAdminModeGet(L7_uint32 *adminMode)
{
  return dsL2RelayAdminModeGet(adminMode);
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay admin mode
*
* @param    adminMode   @b((input)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayAdminModeSet(L7_uint32 adminMode)
{
  return dsL2RelayAdminModeSet(adminMode);
}


/*********************************************************************
* @purpose  Gets the DHCP L2 Relay interface mode
*
* @param    intIfNum    @b((input)) internal interface number
* @param    intfMode    @b((output)) DHCP L2 Relay interface mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayIntfModeGet(L7_uint32 intIfNum, L7_uint32 *intfMode)
{
  return dsL2RelayIntfModeGet(intIfNum, intfMode);
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay interface mode
*
* @param    intIfNum    @b((input)) internal interface number
* @param    intfMode    @b((input)) DHCP L2 Relay interface mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayIntfModeSet(L7_uint32 intIfNum, L7_uint32 intfMode)
{
  return dsL2RelayIntfModeSet(intIfNum, intfMode);
}

/*********************************************************************
* @purpose  Get API for whether DHCP L2 relay considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((output)) L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS, if mode was set
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayIntfTrustGet(L7_uint32 intIfNum, L7_BOOL *trust)
{
  return dsL2RelayIntfTrustGet(intIfNum, trust);
}

/*********************************************************************
* @purpose  Set API for whether DHCP l2relay considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((input))  L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS, if mode was set
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayIntfTrustSet(L7_uint32 intIfNum, L7_BOOL trust)
{
  return dsL2RelayIntfTrustSet(intIfNum, trust);
}

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay for a VLAN.
*
* @param    vlanNum       @b((input)) VLAN number
* @param    l2RelayMode   @b((output)) DHCP L2 Relay  mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayVlanModeGet(L7_uint32 vlanNum, L7_uint32 *l2RelayMode)
{
  return dsL2RelayVlanModeGet(vlanNum, l2RelayMode);
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay mode for a vlan range
*
* @param    vlanStart       @b((input)) Starting Vlan number
* @param    vlanEnd         @b((input)) Ending Vlan number
* @param    l2RelayMode        @b((input)) L2 DHCP Relay Agent mode

*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayVlanModeSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                             L7_uint32 l2RelayMode)
{
  return dsL2RelayVlanModeSet(vlanStart, vlanEnd, l2RelayMode);
}

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay circuit-id mode for a vlan
*
* @param    vlanNum        @b((input))  VLAN number
* @param    circuitIdMode  @b((output)) DHCP L2 Relay Circuit-Id mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayCircuitIdGet(L7_uint32 vlanNum, L7_uint32 *circuitIdMode)
{
  return dsL2RelayCircuitIdGet(vlanNum, circuitIdMode);
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay  circuit-Id mode for a vlan range
*
* @param    vlanStart       @b((input)) Starting Vlan number
* @param    vlanEnd         @b((input)) Ending Vlan number
* @param    circuitIdMode   @b((input)) L2 DHCP Relay Agent mode

*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayCircuitIdSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                              L7_uint32 circuitId)
{
  return dsL2RelayCircuitIdSet(vlanStart, vlanEnd, circuitId);
}

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay remote-id string for a vlan
*
* @param    vlanNum   @b((input))  VLAN number
* @param    remoteId  @b((output)) DHCP L2 Relay remote Id string
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayRemoteIdGet(L7_uint32 vlanNum, L7_uchar8 *remoteId)
{
  return dsL2RelayRemoteIdGet(vlanNum, remoteId);
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay remote-Id string for a vlan range
*
* @param    vlanStart    @b((input)) Starting Vlan number
* @param    vlanEnd      @b((input)) Ending Vlan number
* @param    remoteId     @b((input)) L2 DHCP Relay Agent remote Id string

*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    The remote-id is expected to be string with null character
*           when disabling, but it should not Null pointer.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayRemoteIdSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                             L7_uchar8 *remoteId)
{
  return dsL2RelayRemoteIdSet(vlanStart, vlanEnd, remoteId);
}

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
/*********************************************************************
* @purpose  Gets the DHCP L2 Relay for a given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    l2RelayMode      @b((output)) DHCP L2 Relay  mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDsL2RelaySubscripionModeGet(L7_uint32 intIfNum,
                                        L7_uchar8 *subscriptionName,
                                        L7_uint32 *l2RelayMode)
{
  return dsL2RelaySubscripionModeGet(intIfNum, subscriptionName, l2RelayMode);
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay for a given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    l2RelayMode      @b((input)) DHCP L2 Relay  mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDsL2RelaySubscripionModeSet(L7_uint32 intIfNum,
                                    L7_uchar8 *subscriptionName,
                                    L7_uint32 l2RelayMode)
{
  return dsL2RelaySubscripionModeSet(intIfNum, subscriptionName, l2RelayMode);
}


/*********************************************************************
* @purpose  Gets the DHCP L2 Relay Circuit-id mode for a
*           given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    cIdMode          @b((output)) DHCP L2 Relay circuit-id mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDsL2RelaySubscripionCircuitIdModeGet(L7_uint32 intIfNum,
                                        L7_uchar8 *subscriptionName,
                                        L7_uint32 *cIdMode)
{
  return dsL2RelaySubscripionCircuitIdModeGet(intIfNum,
                                              subscriptionName, cIdMode);
}
/*********************************************************************
* @purpose  Sets the DHCP L2 Relay Circuit-id mode for a
*           given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    cIdMode          @b((input)) DHCP L2 Relay circuit-id mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDsL2RelaySubscripionCircuitIdModeSet(L7_uint32 intIfNum,
                                    L7_uchar8 *subscriptionName,
                                    L7_uint32 cIdMode)
{
  return dsL2RelaySubscripionCircuitIdModeSet(intIfNum,
                                              subscriptionName, cIdMode);
}
/*********************************************************************
* @purpose  Gets the DHCP L2 Relay Remote-id string for a
*           given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    rIdStr           @b((output)) DHCP L2 Relay remote-id string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDsL2RelaySubscripionRemoteIdModeGet(L7_uint32 intIfNum,
                                        L7_uchar8 *subscriptionName,
                                        L7_uchar8 *rIdStr)
{
  return dsL2RelaySubscripionRemoteIdModeGet(intIfNum,
                                              subscriptionName, rIdStr);
}

/*********************************************************************
* @purpose  Sets the DHCP L2 Relay Remote-id string for a
*           given subscription on an interface.
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subsripion name
* @param    rIdstr           @b((input)) DHCP L2 Relay remote-id string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDsL2RelaySubscripionRemoteIdModeSet(L7_uint32 intIfNum,
                                    L7_uchar8 *subscriptionName,
                                    L7_uchar8 *rIdStr)
{
  return dsL2RelaySubscripionRemoteIdModeSet(intIfNum,
                                              subscriptionName, rIdStr);
}

/*********************************************************************
* @purpose  Get the next DHCP L2 Relay Subscription entry on an interface
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subscription name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDsL2RelayIntfSubscriptionGet(L7_uint32 intIfNum, L7_uchar8 *subscriptionName)
{
  return dsL2RelayIntfSubscriptionGet(intIfNum, subscriptionName);
}
/*********************************************************************
* @purpose  Get the next DHCP L2 Relay Subscription entry on an interface
*
* @param    intIfNum         @b((inout)) internal interface number
* @param    subscriptionName @b((inout)) subscription name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDsL2RelayIntfSubscriptionNextGet(L7_uint32 *intIfNum,
                                           L7_uchar8 *subscriptionName)
{
  return dsL2RelayIntfSubscriptionNextGet(intIfNum, subscriptionName);
}
#endif
/*********************************************************************
* @purpose  Gets the DHCP L2 Relay interface specific statistics.
*
* @param    intIfNum     @b((input)) internal interface number
* @param    intfStats    @b((ouput)) L2 Relay statstics.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayIntfStatsGet(L7_uint32 intIfNum, 
                              dsL2RelayIntfStats_t *intfStats)
{
  return dsL2RelayIntfStatsGet(intIfNum, intfStats);
}
/*********************************************************************
* @purpose  Clears the DHCP L2 Relay interface specific statistics.
*
* @param    intIfNum     @b((input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayIntfStatsClear(L7_uint32 intIfNum)
{
  return dsL2RelayIntfStatsClear(intIfNum);
}
/*********************************************************************
* @purpose  Gives the next interface on which DHCP L2 Relay is enabled.
*
* @param    intIfNum     @b((input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayIntfEnabledNextGet(L7_uint32 *intIfNext)
{
  return dsL2RelayIntfEnabledNextGet(intIfNext);
}
/*********************************************************************
* @purpose  Gives the next vlan range on which DHCP L2 Relay is enabled.
*
* @param    vlanStart     @b((input)) starting VLAN Id
* @param    vlanEnd       @b((input)) Ending VLAN Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayVlanRangeNextGet(L7_uint32 *vlanStart, L7_uint32 *vlanEnd)
{
  return dsL2RelayVlanRangeNextGet(vlanStart, vlanEnd);
}
/*********************************************************************
* @purpose  Gives the next vlan range on which DHCP CircuitId is enabled.
*
* @param    vlanStart     @b((input)) starting VLAN Id
* @param    vlanEnd       @b((input)) Ending VLAN Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayCircuitIdVlanRangeNextGet(L7_uint32 *vlanStart, L7_uint32 *vlanEnd)
{
  return dsL2RelayCircuitIdVlanRangeNextGet(vlanStart, vlanEnd);
}
/*********************************************************************
* @purpose  Gives the next vlan on which DHCP RemoteId is enabled.
*
* @param    vlanNext      @b((input)) VLAN Id
* @param    remoteIdStr   @b((input)) Remote Id string confiured on that VLAN.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsL2RelayRemoteIdVlanNextGet(L7_uint32 *vlanNext, L7_uchar8 *remoteIdStr)
{
  return dsL2RelayRemoteIdVlanNextGet(vlanNext, remoteIdStr);
}

#endif
/*********************************************************************
* @purpose  Get API for whether DHCP snooping verifies source MAC addresses.
*
* @param    verifyMac   @b((output)) Verify MAC option
*
* @returns  L7_SUCCESS, if mode was set
*
* @notes    When MAC verify is enabled (and port security is enabled 
*           on the interface), DHCP snooping tells the hardware to 
*           drop any incoming frames on the interface if the source MAC
*           is not the MAC address for a DHCP client who has been leased
*           an IP address.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsVerifyMacGet(L7_BOOL *verifyMac)
{
  return dsVerifyMacGet(verifyMac);
}

/*********************************************************************
* @purpose  Set API for whether DHCP snooping verifies source MAC addresses.
*
* @param    verifyMac   @b((input)) L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    see dsVerifyMacGet().
*
* @end
*********************************************************************/
L7_RC_t usmDbDsVerifyMacSet(L7_BOOL verifyMac)
{
  return dsVerifyMacSet(verifyMac);
}

/*********************************************************************
* @purpose  Get API for whether DHCP snooping is enabled or disabled on a VLAN.
*
* @param    vlanId @b((input)) Low end of VLAN range
* @param    val    @b((output)) L7_ENABLE if DHCP snooping is enabled on the VLAN
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDsVlanConfigGet(L7_uint32 vlanId, L7_uint32 *val)
{
  return dsVlanConfigGet(vlanId, val);
}

/*********************************************************************
* @purpose  Enable or disable DHCP snooping on a VLAN.
*
* @param    vlanStart @b((input)) Low end of VLAN range
* @param    vlanEnd   @b((input)) High end of VLAN range
* @param    val       @b((input)) L7_ENABLE if DHCP snooping is enabled 
*                                 on all VLANs in the range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDsVlanConfigSet(L7_uint32 vlanStart, L7_uint32 vlanEnd, L7_uint32 val)
{
  return dsVlanConfigSet(vlanStart, vlanEnd, val);
}

/*********************************************************************
* @purpose  Given a VLAN ID, verify whether the DHCP snooping is
*           enabled or not.
*
* @param    vlanId    @b((input)) Low end of VLAN range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDsVlanEnableModeGet(L7_uint32 vlanId)
{
  return dsVlanEnableModeGet(vlanId);
}

/*********************************************************************
* @purpose  Given a VLAN ID, find the next vlanId where  DHCP snooping
*           is enabled.
*
* @param    vlanId    @b((input/ouput))
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDsNextEnabledVlanGet(L7_uint32 *vlanId)
{
  return dsNextEnabledVlanGet(vlanId);
}

/*********************************************************************
* @purpose  Given a VLAN ID, return the endpoints of the next range of 
*           VLANs enabled for DHCP snooping.
*
* @param    vlanStart @b((input/output)) Low end of VLAN range
* @param    vlanEnd   @b((input/output)) High end of VLAN range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API intended to be used by show run, so that show run
*           doesn't have to list each enabled VLAN individually. Search
*           for next range starts at vlanEnd + 1. 
*
*           If no VLANs are enabled for DHCP snooping beyond vlanIn, 
*           returns L7_SUCCESS with vlanStart = 0.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsNextEnabledVlanRangeGet(L7_uint32 *vlanStart,
                                       L7_uint32 *vlanEnd)
{
  return dsNextEnabledVlanRangeGet(vlanStart, vlanEnd);
}
/*********************************************************************
* @purpose  Checks to see if the interface is valid for DHCP snooping
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL usmDbDsIntfIsValid(L7_uint32 intIfNum)
{
  return dsIntfIsValid(intIfNum);
}
 

/*********************************************************************
* @purpose  Determines whether DHCP snooping is enabled on a port.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((output)) L7_TRUE if DHCP snooping is enabled on port
*
* @returns  L7_SUCCESS, if mode was set
*           L7_FAILURE
*
* @notes    DHCP snooping is considered enabled on a port if DHCP snooping
*           is enabled on one or more VLANs the port participates in.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsPortEnabledGet(L7_uint32 intIfNum, L7_BOOL *enabled)
{
  return dsPortEnabledGet(intIfNum, enabled);
}

/*********************************************************************
* @purpose  Get API for whether DHCP snooping considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((output)) L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS, if mode was set
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfTrustGet(L7_uint32 intIfNum, L7_BOOL *trust)
{
  return dsIntfTrustGet(intIfNum, trust);
}

/*********************************************************************
* @purpose  Set API for whether DHCP snooping considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((input))  L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS, if mode was set
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfTrustSet(L7_uint32 intIfNum, L7_BOOL trust)
{
  return dsIntfTrustSet(intIfNum, trust);
}

/*********************************************************************
* @purpose  Get API for whether DHCP snooping logs invalid packets.
*
* @param    intIfNum @b((input))  internal interface number
* @param    log      @b((output)) L7_TRUE logging enabled on this interface
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfLogInvalidGet(L7_uint32 intIfNum, L7_BOOL *log)
{
  return dsIntfLogInvalidGet(intIfNum, log);
}



/*********************************************************************
* @purpose  API to get the rate limit value of incoming DHCP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    rate            @b((output)) upper limit for number of
*                           incoming packets processed per second
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfRateLimitGet(L7_uint32 intIfNum, L7_int32 *rate)
{
  return dsIntfRateLimitGet(intIfNum, rate);
}

/*********************************************************************
* @purpose  API to get the burst interval value of incoming DHCP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    burst_interval  @b((output)) the consecutive interval in
*                           seconds, over which the interface is
*                           monitored for a high rate of DHCP packets
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfBurstIntervalGet(L7_uint32 intIfNum, L7_uint32 *burst_interval)
{
  return dsIntfBurstIntervalGet(intIfNum, burst_interval);
}


/*********************************************************************
* @purpose  API to set the rate limit value of incoming DHCP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    rate            @b((input)) upper limit for number of
*                           incoming packets processed per second
*                           If passed as -1, it means no upper limit
*                           is set for the rate.
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfRateLimitSet(L7_uint32 intIfNum, L7_int32 rate)
{
  return dsIntfRateLimitSet(intIfNum, rate);
}

/*********************************************************************
* @purpose  API to set the burst interval value of incoming DHCP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    burst_interval  @b((input)) the consecutive interval in
*                           seconds, over which the interface is
*                           monitored for a high rate of ARP packets
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfBurstIntervalSet(L7_uint32 intIfNum, L7_uint32 burst_interval)
{
  return dsIntfBurstIntervalSet(intIfNum, burst_interval);
}


/*********************************************************************
* @purpose  Set API for whether DHCP snooping logs invalid packets.
*
* @param    intIfNum @b((input))  internal interface number
* @param    log      @b((input))  L7_TRUE if invalid packets should be logged
*
* @returns  L7_SUCCESS, if mode was set
*           L7_NOT_SUPPORTED, if DHCP snooping is not supported on this 
*                             type of interface
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfLogInvalidSet(L7_uint32 intIfNum, L7_BOOL log)
{
  return dsIntfLogInvalidSet(intIfNum, log);
}

/*********************************************************************
* @purpose  Get API for Remote file name to store and restore the
*           DHCP SNOOPING database.
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsDbFileNameGet (L7_uchar8 *fileName)
{
  return dsDbFileNameGet(fileName);
}

/*********************************************************************
* @purpose  Get API for Remote IpAddr to store and restore the
*           DHCP SNOOPING database.
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsDbIpAddrGet (L7_uint32 *ipAddr)
{
  return dsDbIpAddrGet(ipAddr);
}

/*********************************************************************
* @purpose  Get API for write delay interval to store the
*           DHCP SNOOPING database.
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsDbStoreIntervalGet (L7_uint32 *storeInterval)
{
  return dsDbStoreIntervalGet(storeInterval);
}



/*********************************************************************
* @purpose  Set API for Remote file name to store and restore the
*           DHCP SNOOPING database.
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsDbFileNameSet (L7_uchar8 *fileName)
{
  return dsDbFileNameSet(fileName);
}


/*********************************************************************
* @purpose  Set API for Remote Ip Addr to store and restore the
*           DHCP SNOOPING database.
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsDbIpAddrSet (L7_uint32 ipAddr)
{
  return dsDbIpAddrSet(ipAddr);
}


/*********************************************************************
* @purpose  Set API for write delay interval to store the
*           DHCP SNOOPING database.
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDsDbStoreIntervalSet (L7_uint32 storeInterval)
{
  return dsDbStoreIntervalSet(storeInterval);
}



/*********************************************************************
* @purpose  Add a static entry to the DHCP snooping bindings database.
*
* @param    macAddr  @b((input))  client MAC address. 
* @param    ipAddr   @b((input))  client IP address. 
* @param    vlanId   @b((input))  client VLAN ID. 
* @param    intIfNum @b((input))  internal interface number. 
*
* @returns  L7_SUCCESS if entry added.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDsStaticBindingAdd(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr,
                                L7_ushort16 vlanId, L7_ushort16 innerVlanId /*PTin modified: DHCP */, L7_uint32 intIfNum)
{
  return dsStaticBindingAdd(macAddr, ipAddr, vlanId, innerVlanId /*PTin modified: DHCP */, intIfNum);
}

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
* @purpose  Add a static entry to the IPSG entries database.
*
* @param    intIfNum @b((input))  internal interface number.
* @param    vlanId   @b((input))  client VLAN ID.
* @param    ipv4Addr @b((input))  client IP address.
* @param    macAddr  @b((input))  client MAC address.
*
* @returns  L7_SUCCESS if entry added.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpsgStaticEntryAdd(L7_uint32 intIfNum,
                           L7_ushort16 vlanId,
                           L7_enetMacAddr_t *macAddr,
                           L7_uint32 ipv4Addr)
{
#if 0//Added to implement IPv6 support
  L7_inet_addr_t ipAddr;

  inetAddressSet(L7_AF_INET, &ipv4Addr, &ipAddr);
#endif

  return ipv4sgStaticEntryAdd(intIfNum,
                               vlanId,
                               macAddr,
                               ipv4Addr);
}
#endif

/*********************************************************************
* @purpose  Remove a static entry from the DHCP snooping bindings database.
*
* @param    macAddr  @b((input))  client MAC address. 
*
* @returns  L7_SUCCESS if entry removed.
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t usmDbDsStaticBindingRemove(L7_enetMacAddr_t *macAddr)
{
  return dsStaticBindingRemove(macAddr);
}

/* PTin added: DHCP snooping */
#if 1
/*********************************************************************
* @purpose  Remove an entry from the DHCP snooping bindings database.
*
* @param    macAddr  @b((input))  client MAC address. 
*
* @returns  L7_SUCCESS if entry removed.
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t usmDbDsBindingRemove(L7_enetMacAddr_t *macAddr , L7_uint8 type)
{
  dsBindingTreeKey_t key;
  memset(&key, 0x00, sizeof(key));
  memcpy(&key.macAddr.addr, &macAddr->addr, L7_ENET_MAC_ADDR_LEN);
  key.ipType = type;
  return dsBindingRemove(&key);
}
#endif

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
* @purpose  Add a static entry to the IPSG entries database.
*
* @param    intIfNum @b((input))  internal interface number.
* @param    vlanId   @b((input))  client VLAN ID.
* @param    ipv4Addr @b((input))  client IP address.
* @param    macAddr  @b((input))  client MAC address.
*
* @returns  L7_SUCCESS if entry added.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpsgStaticEntryRemove(L7_uint32 intIfNum,
                           L7_ushort16 vlanId,
                           L7_enetMacAddr_t *macAddr,
                           L7_uint32 ipv4Addr)
{
#if 0//Added to implement IPv6 support
  L7_inet_addr_t ipAddr;

  inetAddressSet(L7_AF_INET, &ipv4Addr, &ipAddr);
#endif

  return ipv4sgStaticEntryRemove(intIfNum,
                               vlanId,
                               macAddr,
                               ipv4Addr);
}
#endif

/*********************************************************************
* @purpose  Get the DHCP snooping binding for a given client.
*
* @param    dsBinding  @b((input))  DHCP snooping binding table entry
*
* @returns  L7_SUCCESS if client binding found
*
* @notes    The MAC address uniquely identifies the client.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsBindingGet(dhcpSnoopBinding_t *dsBinding)
{
  return dsBindingGet(dsBinding);
}

/*********************************************************************
* @purpose  Get the DHCP snooping binding for a given client based on
*           binding type.
*
* @param    dsBinding  @b((input))  DHCP snooping binding table entry
*
* @returns  L7_SUCCESS if client binding found
*
* @notes    The MAC address uniquely identifies the client.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsBindingGetByType(dhcpSnoopBinding_t *dsBinding, 
                                L7_uint32 bindingType)
{
  L7_RC_t rc = L7_FAILURE;
  
  rc = dsBindingGet(dsBinding);
  if ( (rc == L7_SUCCESS) && (dsBinding->bindingType == bindingType))
  return rc;
  else
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Given a DHCP snooping binding, get the next one.
*
* @param    dsBinding  @b((input))  DHCP snooping binding table entry
*
* @returns  L7_SUCCESS if another binding is found
*
* @notes    The MAC address uniquely identifies the client.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsBindingGetNext(dhcpSnoopBinding_t *dsBinding)
{
  return dsBindingGetNext(dsBinding);
}


/*********************************************************************
* @purpose  Given a DHCP snooping binding, get the next one.
*
* @param    dsBinding  @b((input))  DHCP snooping binding table entry
*
* @returns  L7_SUCCESS if another binding is found
*
* @notes    The MAC address uniquely identifies the client.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsBindingGetNextByType(dhcpSnoopBinding_t *dsBinding,
                              L7_uint32 bindingType)
{
  L7_RC_t rc = L7_FAILURE;
  
  while(1)
  {
   rc = dsBindingGetNext(dsBinding);
   if ( rc == L7_FAILURE)
   {
     rc = L7_FAILURE;
     break;
   }
   else if ( (rc == L7_SUCCESS) && 
             (bindingType == dsBinding->bindingType))
   {
     rc = L7_SUCCESS;
     break;
   }
  }
  return rc;
}


/*********************************************************************
* @purpose  Get the nth Entry from the DHCP snooping table.
*
* @param    dsBinding  @b((input))  DHCP snooping binding table entry
*
* @returns  L7_SUCCESS if another binding is found
*
* @notes    The MAC address uniquely identifies the client.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsBindingNthEntryGet(dhcpSnoopBinding_t *dsBinding,
                               L7_uint32 n,
                               dsBindingType_t type )
{
  return dsBindingNthEntryGet(dsBinding,n,type);
}

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
* @purpose  Get the nth Entry from the IPSG table.
*
* @param    ipsgBinding  @b((input))  IPSG binding table entry
*
* @returns  L7_SUCCESS if another binding is found
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpsgBindingNthEntryGet(ipsgBinding_t *ipsgBinding,
                               L7_uint32 n,
                               ipsgEntryType_t type)
{
  return ipv4sgBindingNthEntryGet(ipsgBinding,n,type);
}

#endif

/*********************************************************************
* @purpose  Get the number of DHCP snooping bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes    
*
*
* @end
*********************************************************************/
L7_uint32 usmDbDsBindingsCount(void)
{
  return dsBindingsCount();
}

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
* @purpose  Get the number of  IPSG bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
*
* @end
*********************************************************************/
L7_uint32 usmDbIpsgBindingsCount(void)
{
  return ipsgBindingsCount();
}
#endif


/*********************************************************************
* @purpose  Get the number of DHCP snooping static bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
*
* @end
*********************************************************************/
L7_uint32 usmDbDsStaticBindingsCount(void)
{
  return dsStaticBindingsCount();
}
#ifdef L7_IPSG_PACKAGE

/*********************************************************************
* @purpose  Get the number of IPSG static bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
*
* @end
*********************************************************************/
L7_uint32 usmDbIpsgStaticBindingsCount(void)
{
  return ipsgStaticBindingsCount();
}
#endif


/*********************************************************************
* @purpose  Clear DHCP snooping bindings.
*
* @param    intIfNum  @b((input))  If non-zero, only bindings associated
*                                  with a specific interface are cleared. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDsBindingClear(L7_uint32 intIfNum)
{
  return dsBindingClear(intIfNum);
}

/*********************************************************************
* @purpose  Get DHCP snooping statistics.
*
* @param    intIfNum  @b((input))  internal interface number
* @param    intfStats @b((output)) stats for given interface 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfStatsGet(L7_uint32 intIfNum, dhcpSnoopIntfStats_t *intfStats)
{
  return dsIntfStatsGet(intIfNum, intfStats);
}

/*********************************************************************
* @purpose  Clear DHCP snooping interface statistics for all interfaces.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDsIntfStatsClear(void)
{
  return dsIntfStatsClear();
}

#ifdef L7_IPSG_PACKAGE
/*********************************************************************
* @purpose  Get the IPSG configuration for an interface.
*
* @param    intIfNum   @b((input)) internal interface number
* @param    verifyIp   @b((output)) whether IPSG verifies the source 
*                                   IP address on this interface
* @param    verifyMac  @b((output)) whether IPSG verifies the source
*                                   MAC address on this interface
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpsgVerifySourceGet(L7_uint32 intIfNum, 
                                 L7_BOOL *verifyIp,
                                 L7_BOOL *verifyMac)
{
  return ipsgVerifySourceGet(intIfNum, verifyIp, verifyMac);
}

/*********************************************************************
* @purpose  Set the IPSG configuration for an interface.
*
* @param    intIfNum   @b((input)) internal interface number
* @param    verifyIp   @b((input)) whether IPSG verifies the source 
*                                   IP address on this interface
* @param    verifyMac  @b((input)) whether IPSG verifies the source
*                                   MAC address on this interface
*
* @returns  L7_SUCCESS
*           L7_NOT_SUPPORTED, if IPSG is not supported on this 
*                             type of interface
*           L7_DEPENDENCY_NOT_MET, if attempting to enable IPSG port security
*                                  without enabling IPSG
*           L7_REQUEST_DENIED, if attempting to disable IPSG port security
*                              while leaving IPSG enabled
*           L7_FAILURE, other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpsgVerifySourceSet(L7_uint32 intIfNum, 
                                 L7_BOOL verifyIp,
                                 L7_BOOL verifyMac)
{
  return ipsgVerifySourceSet(intIfNum, verifyIp, verifyMac);
}



/*********************************************************************
* @purpose  Get the next source binding for IP source guard.
*
* @param    intIfNum   @b((input/output)) internal interface number
* @param    vlanId     @b((input/output)) VLAN ID
* @param    ipAddr     @b((input/output)) authorized source IP address
* @param    macAddr    @b((input/output)) authorized source MAC address
*
* @returns  L7_SUCCESS
*           L7_NOT_IMPLEMENTED_YET when a IPv6 entry is found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpsgBindingGetNext(L7_uint32 *intIfNum, L7_ushort16 *vlanId, 
                                L7_uint32 *ipv4Addr, L7_enetMacAddr_t *macAddr,
                                L7_uint32 *entryType)
{
  L7_inet_addr_t ipAddr;
  L7_RC_t        rc;

  rc=ipsgBindingGetNext(intIfNum, vlanId, &ipAddr, macAddr,
                            entryType,L7_MATCH_GETNEXT);

  if(ipAddr.family == L7_AF_INET)
  {
    *ipv4Addr=ipAddr.addr.ipv4.s_addr;
     return rc;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_SNOOPING_COMPONENT_ID,
              "INET_ADDR: FamilyType Not Implemented Yet- %d", ipAddr.family);
    return L7_NOT_IMPLEMENTED_YET;
  }    
}

/*********************************************************************
* @purpose  Get the source binding for IP source guard.
*
* @param    intIfNum   @b((input/output)) internal interface number
* @param    vlanId     @b((input/output)) VLAN ID
* @param    ipAddr     @b((input/output)) authorized source IP address
* @param    macAddr    @b((input/output)) authorized source MAC address
*
* @returns  L7_SUCCESS
*           L7_NOT_IMPLEMENTED_YET when a IPv6 entry is found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpsgBindingGet(L7_uint32 *intIfNum, L7_ushort16 *vlanId,
                                L7_uint32 *ipv4Addr, L7_enetMacAddr_t *macAddr,
                                L7_uint32 *entryType)
{  
  L7_inet_addr_t ipAddr;
  L7_RC_t        rc;

  rc=ipsgBindingGetNext(intIfNum, vlanId, &ipAddr, macAddr,
                            entryType,L7_MATCH_EXACT);

  if(ipAddr.family == L7_AF_INET)
  {
    *ipv4Addr=ipAddr.addr.ipv4.s_addr;
     return rc;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_SNOOPING_COMPONENT_ID,
              "INET_ADDR: FamilyType Not Implemented Yet- %d", ipAddr.family);
    return L7_NOT_IMPLEMENTED_YET;
  }    
}

/*********************************************************************
* @purpose  Get the source binding for IP source guard based on 
*           entry type.
*
* @param    intIfNum   @b((input/output)) internal interface number
* @param    vlanId     @b((input/output)) VLAN ID
* @param    ipAddr     @b((input/output)) authorized source IP address
* @param    macAddr    @b((input/output)) authorized source MAC address
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpsgBindingGetByType(L7_uint32 *intIfNum, L7_ushort16 *vlanId,
                                L7_uint32 *ipv4Addr, L7_enetMacAddr_t *macAddr,
                                L7_uint32 entryType)
{
  L7_RC_t        rc;            
  L7_uint32      entryTypeIn = entryType;
  L7_inet_addr_t ipAddr;
  
  while (1)
  {
    rc=ipsgBindingGetNext(intIfNum, vlanId, &ipAddr, macAddr,
                            &entryTypeIn, L7_MATCH_EXACT);

    if (rc != L7_SUCCESS)
    {
      break;
    }

    if( (rc == L7_SUCCESS) && (entryTypeIn == entryType) && (ipAddr.family == L7_AF_INET))
    {
      *ipv4Addr=ipAddr.addr.ipv4.s_addr;
      break;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Get the next source binding for IP source guard based on
*           entry type.
*
* @param    intIfNum   @b((input/output)) internal interface number
* @param    vlanId     @b((input/output)) VLAN ID
* @param    ipAddr     @b((input/output)) authorized source IP address
* @param    macAddr    @b((input/output)) authorized source MAC address
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpsgBindingGetNextByType(L7_uint32 *intIfNum, L7_ushort16 *vlanId,
                                L7_uint32 *ipv4Addr, L7_enetMacAddr_t *macAddr,
                                L7_uint32 entryType)
{
  L7_RC_t        rc;
  L7_uint32      entryTypeIn;
  L7_inet_addr_t ipAddr;

  while (1)
  {
    rc = ipsgBindingGetNext(intIfNum, vlanId, &ipAddr, macAddr,
                            &entryTypeIn, L7_MATCH_GETNEXT);
    if (rc != L7_SUCCESS)
    {
       break;
    }
    if ( (rc == L7_SUCCESS)&& (entryTypeIn == entryType)  && (ipAddr.family == L7_AF_INET) )
    {
      *ipv4Addr = ipAddr.addr.ipv4.s_addr;
      break;
    }
  }
  return rc;
}
#endif

/*********************************************************************
* @purpose  To notify the DHCP Snooping application about the
*           download status of the DHCP Snooping bindings.
*
* @param    downloadStatus  @b((input))  status of the tftp process
*
* @returns  L7_SUCCESS
*
* @notes    This functionality should be through call back.
*
* @end
*********************************************************************/
L7_RC_t usmDbDsTftpDownloadStatus(L7_uint32 direction,L7_BOOL downloadStatus)
{

  return dsTftpStatusCb(direction,downloadStatus);
}








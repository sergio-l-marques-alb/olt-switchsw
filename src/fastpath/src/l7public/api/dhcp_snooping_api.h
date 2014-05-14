/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  dhcp_snooping_api.h
*
* @purpose   DHCP snooping APIs
*
* @component DHCP snooping
*
* @comments Replaces the DHCP filtering component. 
*           See functional spec "DHCP Snooping and IP Source Guard FS"
*
*           DHCP Snooping is a security feature that monitors traffic between a 
*           DHCP client and DHCP server to build a bindings database of 
*           {MAC address, IP address, port} tuples that are considered authorized. 
*           The network administrator enables DHCP snooping globally and on specific 
*           VLANs, and configures ports within the VLAN to be trusted or untrusted.  
*           DHCP servers must be reached through trusted ports. DHCP snooping 
*           enforces the following security rules:
*
*           - DHCP packets from a DHCP server (DHCPOFFER, DHCPACK, DHCPNAK, 
*             DHCPRELEASEQUERY) are dropped if received on an untrusted port.
*
*           - DHCPRELEASE and DHCPDECLINE messages are dropped if for a MAC address 
*             in the snooping database, but the binding's interface is other than 
*             the interface where the message was received.
*
*           If port security (aka port MAC locking) is enabled on an untrusted 
*           interface, then DHCP snooping provides an additional security mechanism:
*
*           - On untrusted interfaces, the switch drops packets whose source MAC 
*             address is not in the list of DHCP clients for that interface.
*
*           IP Source Guard and Dynamic ARP Inspection use the DHCP snooping 
*           bindings database. When IP Source Guard is enabled, the switch drops 
*           incoming packets whose source IP address does not match an IP address 
*           in the bindings database. Dynamic ARP Inspection uses the bindings 
*           database to validate ARP packets.
*
*
* @create 3/15/2007
*
* @author Rob Rice (rrice)
*
* @end
*             
**********************************************************************/

#ifndef DHCP_SNOOPING_API_H
#define DHCP_SNOOPING_API_H

#include "comm_structs.h"
#include "defaultconfig.h"
/* DHCP snooping is applied only on physical ports. */
#define DS_MAX_INTF_COUNT   L7_MAX_INTERFACE_COUNT
#define DS_MAX_REMOTE_ID_STRING  FD_DS_MAX_REMOTE_ID_STRING  
#include "ping_api.h"
#include "snooping_api.h"

/* L2 Relay per subscription configuration.*/
typedef struct dsL2RelaySubscrptionCfg_s
{
  L7_uchar8 subscriptionName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX + 1];
  L7_uint32 l2relay;
  L7_uint32 circuitId;
  L7_uchar8 remoteId[DS_MAX_REMOTE_ID_STRING];
}dsL2RelaySubscriptionCfg_t;

 /************************************************************************
* DHCP Snooping funtion table that contains the APIs that are available
* for the external components when this component exists.
************************************************************************/
typedef struct dsFuncTable_s
{
  L7_RC_t (*dsBindingGet)(dhcpSnoopBinding_t *dsBinding);
  L7_RC_t (*dsAdminModeGet)(L7_uint32 *adminMode);
  L7_RC_t (*dsVlanConfigGet)(L7_uint32 vlanId, L7_uint32 *val);
  L7_RC_t (*dsIntfTrustGet)(L7_uint32 intIfNum, L7_BOOL *trust);
  L7_RC_t (*dsPortEnabledGet)(L7_uint32 intIfNum, L7_BOOL *enabled);

} dsFuncTable_t;

typedef struct dsL2RelayIntfStats_s
{
  L7_uint32 untrustedSrvMsgsWithOpt82;
  L7_uint32 untrustedCliMsgsWithOpt82;
  L7_uint32 trustedSrvMsgsWithoutOpt82;
  L7_uint32 trustedCliMsgsWithoutOpt82;
}dsL2RelayIntfStats_t;

extern dsFuncTable_t dsFuncTable; 

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
L7_RC_t dsAdminModeGet(L7_uint32 *adminMode);

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
L7_RC_t dsAdminModeSet(L7_uint32 adminMode);


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
#ifdef L7_DHCP_L2_RELAY_PACKAGE
L7_RC_t dsL2RelayAdminModeGet(L7_uint32 *adminMode);
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
L7_RC_t dsL2RelayAdminModeSet(L7_uint32 adminMode);

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
L7_RC_t dsL2RelayIntfModeGet(L7_uint32 intIfNum, L7_uint32 *intfMode);

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
L7_RC_t dsL2RelayIntfModeSet(L7_uint32 intIfNum, L7_uint32 intfMode);

/*********************************************************************
* @purpose  Get API for whether DHCP L@ Relay considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((output)) L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfTrustGet(L7_uint32 intIfNum, L7_BOOL *trust);

/*********************************************************************
* @purpose  Set API for whether DHCP L2 Relay considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((input))  L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS, if mode was set
*           L7_NOT_SUPPORTED, if DHCP L2 Relay is not supported on this 
*                             type of interface
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfTrustSet(L7_uint32 intIfNum, L7_BOOL trust);

/*********************************************************************
* @purpose  Gets the DHCP L2 Relay for a VLAN
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
L7_RC_t dsL2RelayVlanModeGet(L7_uint32 vlanNum, L7_uint32 *l2RelayMode);
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
L7_RC_t dsL2RelayVlanModeSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                             L7_uint32 l2RelayMode);

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
L7_RC_t dsL2RelayCircuitIdGet(L7_uint32 vlanNum, L7_uint32 *circuitIdMode);
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
L7_RC_t dsL2RelayCircuitIdSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                             L7_uint32 l2RelayMode);


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
L7_RC_t dsL2RelayRemoteIdGet(L7_uint32 vlanNum, L7_uchar8 *remoteId);
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
L7_RC_t dsL2RelayRemoteIdSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                             L7_uchar8 *remoteId);


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
L7_RC_t dsL2RelaySubscripionModeGet(L7_uint32 intIfNum,
                                        L7_uchar8 *subscriptionName,
                                        L7_uint32 *l2RelayMode);

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
L7_RC_t dsL2RelaySubscripionModeSet(L7_uint32 intIfNum,
                                    L7_uchar8 *subscriptionName,
                                    L7_uint32 l2RelayMode);

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
L7_RC_t dsL2RelaySubscripionCircuitIdModeGet(L7_uint32 intIfNum,
                                        L7_uchar8 *subscriptionName,
                                        L7_uint32 *cIdMode);
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
L7_RC_t dsL2RelaySubscripionCircuitIdModeSet(L7_uint32 intIfNum,
                                    L7_uchar8 *subscriptionName,
                                    L7_uint32 cIdMode);
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
L7_RC_t dsL2RelaySubscripionRemoteIdModeGet(L7_uint32 intIfNum,
                                        L7_uchar8 *subscriptionName,
                                        L7_uchar8 *rIdStr);

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
L7_RC_t dsL2RelaySubscripionRemoteIdModeSet(L7_uint32 intIfNum,
                                    L7_uchar8 *subscriptionName,
                                    L7_uchar8 *rIdStr);



/*********************************************************************
* @purpose  Get the next DHCP L2 Relay Subscription entry on an interface
*
* @param    intIfNum         @b((input)) internal interface number
* @param    subscriptionName @b((input)) subscription name
* @param    subsCfg          @b((input)) DHCP L2 Relay subscription entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfSubscriptionEntryNextGet(L7_uint32 *intIfNum,
                                           L7_uchar8 *subscriptionName,
                                           dsL2RelaySubscriptionCfg_t **subsCfg);
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
L7_RC_t dsL2RelayIntfSubscriptionGet(L7_uint32 intIfNum, L7_uchar8 *subscriptionName);
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
L7_RC_t dsL2RelayIntfSubscriptionNextGet(L7_uint32 *intIfNum,
                                           L7_uchar8 *subscriptionName);
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
L7_RC_t dsL2RelayIntfStatsGet(L7_uint32 intIfNum, 
                              dsL2RelayIntfStats_t *intfStats);
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
L7_RC_t dsL2RelayIntfStatsClear(L7_uint32 intIfNum);


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
L7_RC_t dsL2RelayIntfEnabledNextGet(L7_uint32 *intIfNum);

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
L7_RC_t dsL2RelayVlanRangeNextGet(L7_uint32 *vlanStart, L7_uint32 *vlanEnd);

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
L7_RC_t dsL2RelayCircuitIdVlanRangeNextGet(L7_uint32 *vlanStart, L7_uint32 *vlanEnd);

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
L7_RC_t dsL2RelayRemoteIdVlanNextGet(L7_uint32 *vlanNext, L7_uchar8 *remoteIdStr);

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
L7_RC_t dsVerifyMacGet(L7_BOOL *verifyMac);

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
L7_RC_t dsVerifyMacSet(L7_BOOL verifyMac);

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
L7_RC_t dsVlanConfigGet(L7_uint32 vlanId, L7_uint32 *val);

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
L7_RC_t dsVlanConfigSet(L7_uint32 vlanStart, L7_uint32 vlanEnd, L7_uint32 val);

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
L7_RC_t dsVlanEnableModeGet(L7_uint32 vlanId);


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
L7_RC_t dsNextEnabledVlanGet(L7_uint32 *vlanId);


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
L7_RC_t dsNextEnabledVlanRangeGet(L7_uint32 *vlanStart,
                                  L7_uint32 *vlanEnd);

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
L7_RC_t dsPortEnabledGet(L7_uint32 intIfNum, L7_BOOL *enabled);

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
L7_RC_t dsIntfTrustGet(L7_uint32 intIfNum, L7_BOOL *trust);

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
L7_RC_t dsIntfTrustSet(L7_uint32 intIfNum, L7_BOOL trust);

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
L7_RC_t dsIntfLogInvalidGet(L7_uint32 intIfNum, L7_BOOL *log);

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
L7_RC_t dsIntfLogInvalidSet(L7_uint32 intIfNum, L7_BOOL log);



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
L7_RC_t dsIntfBurstIntervalGet(L7_uint32 intIfNum, L7_uint32 *burst_interval);


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
L7_RC_t dsIntfRateLimitSet(L7_uint32 intIfNum, L7_int32 rate);


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
L7_RC_t dsIntfRateLimitGet(L7_uint32 intIfNum, L7_int32 *rate);


/*********************************************************************
* @purpose  API to set the burst interval value of incoming DHCP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    burst_interval  @b((input)) the consecutive interval in
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
L7_RC_t dsIntfBurstIntervalSet(L7_uint32 intIfNum, L7_uint32 burst_interval);

/*********************************************************************
* @purpose  Sets the remote file name to be used to store/restore
*           the DHCP SNOOPING database
*
* @param    fileName  @b((input)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS, if fileName  was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbFileNameSet(L7_uchar8* fileName);


/*********************************************************************
* @purpose  Sets the remote IP address to be used to store/restore
*           the DHCP SNOOPING database
*
* @param    ipAddress  @b((input)) ip address
*
* @returns  L7_SUCCESS, if ipAddress  was set
* @returns  L7_FAILURE, if invalid ipAddress was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbIpAddrSet(L7_uint32 ipAddr);

/*********************************************************************
* @purpose  Sets the store interval to be used to store
*           the DHCP SNOOPING database
*
* @param    storeInterval  @b((input)) store interval
*
* @returns  L7_SUCCESS, if store interval was set
* @returns  L7_FAILURE, if invalid store interval was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbStoreIntervalSet(L7_uint32 ipAddr);


/*********************************************************************
* @purpose  Get file name that will be used for storing/restoring the
*           DHCP SNOOPING.
*
* @param    fileName  @b((output)) fileName
*
* @returns  L7_SUCCESS if value successfully retrieved
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsDbFileNameGet(L7_uchar8* fileName);


/*********************************************************************
* @purpose  Get the remote IP address to be used to store/restore
*           the DHCP SNOOPING database
*
* @param    ipAddress  @b((input)) ip address
*
* @returns  L7_SUCCESS, if ipAddress  was set
* @returns  L7_FAILURE, if invalid ipAddress was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbIpAddrGet(L7_uint32* ipAddr);

/*********************************************************************
* @purpose  Gets the store interval to be used to store
*           the DHCP SNOOPING database
*
* @param    storeInterval  @b((output)) store interval
*
* @returns  L7_SUCCESS, if store interval was Get
* @returns  L7_FAILURE, if invalid store interval was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDbStoreIntervalGet(L7_uint32 *storeInterval);



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
L7_RC_t dsStaticBindingAdd(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr,
                           L7_ushort16 vlanId, L7_ushort16 innerVlanId /*PTin modified: DHCP */, L7_uint32 intIfNum);

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
L7_RC_t dsStaticBindingRemove(L7_enetMacAddr_t *macAddr);

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
L7_RC_t dsBindingClear(L7_uint32 intIfNum);

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
L7_uint32 dsBindingsCount(void);

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
L7_uint32 ipsgBindingsCount (void);


/*********************************************************************
* @purpose  Get the number of  IPSG static bindings.
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
L7_uint32 ipsgStaticBindingsCount (void);


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
L7_uint32 dsStaticBindingsCount(void);


/*********************************************************************
* @purpose  Get the DHCP snooping binding for a given client.
*
* @param    dsBinding  @b((input/output))  DHCP snooping binding table entry
*
* @returns  L7_SUCCESS if client binding found
*
* @notes    The MAC address uniquely identifies the client.
*
* @end
*********************************************************************/
L7_RC_t dsBindingGet(dhcpSnoopBinding_t *dsBinding);

/*********************************************************************
* @purpose  Given a DHCP snooping binding, get the next one.
*
* @param    dsBinding  @b((input/output))  DHCP snooping binding table entry
*
* @returns  L7_SUCCESS if another binding is found
*
* @notes    The MAC address uniquely identifies the client.
*
* @end
*********************************************************************/
L7_RC_t dsBindingGetNext(dhcpSnoopBinding_t *dsBinding);

/*********************************************************************
* @purpose  Find out if a binding exists for a given MAC address,
*           IP address, and VLAN.
*
* @param    macAddr  @b((input))  client MAC address. 
* @param    ipAddr   @b((input))  client IP address. 
* @param    vlanId   @b((input))  client VLAN ID. 
*
* @returns  L7_TRUE if binding exists
*
* @notes    Expecting dynamic ARP inspection and pml to use this.
*
* @end
*********************************************************************/
L7_BOOL dsClientKnown(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr,
                      L7_ushort16 vlanId);

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
L7_RC_t dsIntfStatsGet(L7_uint32 intIfNum, dhcpSnoopIntfStats_t *intfStats);

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
L7_RC_t dsIntfStatsClear(void);

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
L7_RC_t ipsgVerifySourceGet(L7_uint32 intIfNum, 
                            L7_BOOL *verifyIp,
                            L7_BOOL *verifyMac);

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
*           L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipsgVerifySourceSet(L7_uint32 intIfNum, 
                            L7_BOOL verifyIp,
                            L7_BOOL verifyMac);

/*********************************************************************
* @purpose  Get the next source binding for IP source guard.
*
* @param    intIfNum   @b((input/output)) internal interface number
* @param    vlanId     @b((input/output)) VLAN ID
* @param    ipAddr     @b((input/output)) authorized source IP address
* @param    macAddr    @b((input/output)) authorized source MAC address
*
* @returns  L7_SUCCESS
*           L7_FAILURE when no more entries
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipsgBindingGetNext(L7_uint32 *intIfNum, L7_ushort16 *vlanId, 
                           L7_uint32 *ipAddr, L7_enetMacAddr_t *macAddr,
                           L7_uint32 *entryType, L7_uint32 matchType);



/*********************************************************************
* @purpose  Query IPSG to find out if a source MAC address is valid. 
*
* @param    macAddr    @b((input)) authorized source MAC address
* @param    vlanId     @b((input)) VLAN ID
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_TRUE if source MAC is valid on given VLAN/port
*           L7_FALSE otherwise
*
* @notes    If IPSG port-security is not enabled on the given interface,
*           consider all clients authorized. 
*
*           Binding must match MAC, vlan ID, and port.
*
* @end
*********************************************************************/
L7_BOOL ipsgClientAuthorized(L7_enetMacAddr_t *macAddr,
                             L7_ushort16 vlanId, L7_uint32 intIfNum);

#endif /* DHCP_SNOOPING_API_H */

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  dai_api.h
*
* @purpose   Dynamic ARP Inspection APIs
*
* @component Dynamic ARP Inspection
*
* @comments 
*
* @create 09/12/2007
*
* @author Kiran Kumar Kella
*
* @end
*             
**********************************************************************/

#ifndef DAI_API_H
#define DAI_API_H

#include "comm_structs.h"

/*********************************************************************
* @purpose  Get API for whether DAI verifies source MAC addresses.
*
* @param    verifySMac   @b((output)) Verify Source MAC option
*
* @returns  L7_SUCCESS, if mode was set
*
* @notes    When SMAC verify is enabled, check the source MAC address
*           in the Ethernet header against the sender MAC address in the
*           ARP body of both ARP requests and responses. Packets with
*           different MAC addresses are classified as invalid and are
*           dropped.
*
* @end
*********************************************************************/
L7_RC_t daiVerifySMacGet(L7_BOOL *verifySMac);

/*********************************************************************
* @purpose  Set API for whether DAI verifies source MAC addresses.
*
* @param    verifySMac   @b((input)) L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    see daiVerifySMacGet().
*
* @end
*********************************************************************/
L7_RC_t daiVerifySMacSet(L7_BOOL verifySMac);

/*********************************************************************
* @purpose  Get API for whether DAI verifies destination MAC addresses.
*
* @param    verifyDMac   @b((output)) Verify destination MAC option
*
* @returns  L7_SUCCESS, if mode was set
*
* @notes    When DMAC verify is enabled, check the destination MAC address
*           in the Ethernet header against the target MAC address in the
*           ARP body of ARP responses. Packets with different MAC addresses
*           are classified as invalid and are dropped.
*
* @end
*********************************************************************/
L7_RC_t daiVerifyDMacGet(L7_BOOL *verifyDMac);

/*********************************************************************
* @purpose  Set API for whether DAI verifies destination MAC addresses.
*
* @param    verifyDMac   @b((input)) L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    see daiVerifyDMacGet().
*
* @end
*********************************************************************/
L7_RC_t daiVerifyDMacSet(L7_BOOL verifyDMac);

/*********************************************************************
* @purpose  Get API for whether DAI verifies IP address.
*
* @param    verifyIP   @b((output)) Verify IP address
*
* @returns  L7_SUCCESS, if mode was set
*
* @notes    When IP verify is enabled, check the ARP body for invalid and
*           unexpected IP addresses. Addresses include 0.0.0.0, 255.255.255.255,
*           and all IP multicast addresses. Sender IP addresses are checked
*           in all ARP requests and responses, and target IP addresses are
*           checked only in ARP responses.
*
* @end
*********************************************************************/
L7_RC_t daiVerifyIPGet(L7_BOOL *verifyIP);

/*********************************************************************
* @purpose  Set API for whether DAI verifies IP address.
*
* @param    verifyIP   @b((input)) L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    see daiVerifyIPGet().
*
* @end
*********************************************************************/
L7_RC_t daiVerifyIPSet(L7_BOOL verifyIP);

/*********************************************************************
*
* @purpose check to see if intIfNum is a valid DAI Interface
*
* @param L7_uint32 intf       @b((input)) Interface Number
*
* @returns L7_TRUE     If valid interface
* @returns L7_FALSE    If not valid interface
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL daiIntfIsValid(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get API for whether Dynamic ARP Inspection is enabled
*           or disabled on a VLAN.
*
* @param    vlanId @b((input)) VLAN Id
* @param    val    @b((output)) L7_ENABLE if DAI is enabled on the VLAN
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanEnableGet(L7_uint32 vlanId, L7_uint32 *val);

/*********************************************************************
* @purpose  Set API for whether Dynamic ARP Inspection is enabled
*           or disabled on a VLAN.
*
* @param    vlanId @b((input)) VLAN Id
* @param    val    @b((input)) L7_ENABLE if DAI is enabled on the VLAN
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanEnableSet(L7_uint32 vlanId, L7_uint32 val);

/*********************************************************************
* @purpose  Get the next vlan id eligible for DAI configuration, given
*           the current vlan id
*
* @param    vlanId   @b((input)) VLAN Id
* @param    nextVlan @b((output)) next vlan id
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanNextGet(L7_uint32 vlanId, L7_uint32 *nextVlan);

/*********************************************************************
* @purpose  Enable or disable Dynamic ARP Inspection on a
*           VLAN/VLAN range
*
* @param    vlanStart @b((input)) Low end of VLAN range
* @param    vlanEnd   @b((input)) High end of VLAN range
* @param    val       @b((input)) L7_ENABLE if DAI is enabled 
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
L7_RC_t daiVlanEnableRangeSet(L7_uint32 vlanStart, L7_uint32 vlanEnd, L7_uint32 val);

/*********************************************************************
* @purpose  Given a VLAN ID, return the endpoints of the next range of 
*           VLANs enabled for Dynamic ARP Inspection.
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
*           If no VLANs are enabled for DAI beyond vlanIn, 
*           returns L7_SUCCESS with vlanStart = 0.
*
* @end
*********************************************************************/
L7_RC_t daiNextEnabledVlanRangeGet(L7_uint32 *vlanStart,
                                   L7_uint32 *vlanEnd);

/*********************************************************************
* @purpose  Get API for whether DAI considers a port trusted.
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
L7_RC_t daiIntfTrustGet(L7_uint32 intIfNum, L7_BOOL *trust);

/*********************************************************************
* @purpose  Set API for whether DAI considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((input))  L7_TRUE if port is trusted
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
L7_RC_t daiIntfTrustSet(L7_uint32 intIfNum, L7_BOOL trust);

/*********************************************************************
* @purpose  Determines whether DAI is enabled on a port.
*
* @param    intIfNum @b((input))  internal interface number
* @param    enabled  @b((output)) L7_TRUE if DAI is enabled on port
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_FAILURE
*
* @notes    DAI is considered enabled on a port if DAI
*           is enabled on one or more VLANs the port participates in.
*
* @end
*********************************************************************/
L7_RC_t daiPortEnabledGet(L7_uint32 intIfNum, L7_BOOL *enabled);

/*********************************************************************
* @purpose  Get API for whether DAI logging is enabled
*           or disabled on a VLAN.
*
* @param    vlanId @b((input)) VLAN Id
* @param    val    @b((output)) L7_ENABLE if DAI logging is enabled on the VLAN
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanLoggingEnableGet(L7_uint32 vlanId, L7_uint32 *val);

/*********************************************************************
* @purpose  Set API for whether DAI logging is enabled
*           or disabled on a VLAN.
*
* @param    vlanId @b((input)) VLAN Id
* @param    val    @b((input)) L7_ENABLE if DAI logging is enabled on the VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanLoggingEnableSet(L7_uint32 vlanId, L7_uint32 val);

/*********************************************************************
* @purpose  Enable or disable DAI logging on a VLAN/VLAN range
*
* @param    vlanStart @b((input)) Low end of VLAN range
* @param    vlanEnd   @b((input)) High end of VLAN range
* @param    val       @b((input)) L7_ENABLE if DAI logging is enabled 
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
L7_RC_t daiVlanLoggingEnableRangeSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                                     L7_uint32 val);

/*********************************************************************
* @purpose  Given a VLAN ID, return the endpoints of the next range of 
*           VLANs enabled for DAI logging.
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
*           If no VLANs are enabled for DAI beyond vlanIn, 
*           returns L7_SUCCESS with vlanStart = 0.
*
* @end
*********************************************************************/
L7_RC_t daiVlanNextLoggingEnableRangeGet(L7_uint32 *vlanStart,
                                         L7_uint32 *vlanEnd);

/*********************************************************************
* @purpose  Get the ARP ACL Name for a vlan
*
* @param    vlanId    @b((input)) vlan Id
* @param    aclName   @b((output)) ARP ACL name for this VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API is intended to be used by WEB/SNMP
*
*           aclName should have memory allocated by the caller already.
*
* @end
*********************************************************************/
L7_RC_t daiVlanArpAclGet(L7_uint32 vlanId, L7_uchar8 *aclName);

/*********************************************************************
* @purpose  Set the ARP ACL Name for a vlan
*
* @param    vlanId    @b((input)) vlan Id
* @param    aclName   @b((input)) ARP ACL name for this VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API is intended to be used by WEB/SNMP
*
*           aclName should have memory allocated by the caller already.
*
* @end
*********************************************************************/
L7_RC_t daiVlanArpAclSet(L7_uint32 vlanId, L7_uchar8 *aclName);

/*********************************************************************
* @purpose  Get the ARP ACL static flag for a vlan
*
* @param    vlanId      @b((input))  vlan Id
* @param    staticFlag @b((output)) static flag for the VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API is intended to be used by WEB/SNMP
*
* @end
*********************************************************************/
L7_RC_t daiVlanArpAclStaticFlagGet(L7_uint32 vlanId, L7_uint32 *staticFlag);

/*********************************************************************
* @purpose  Set the ARP ACL static flag for the vlan
*
* @param    vlanId     @b((input)) vlan Id
* @param    staticFlag @b((input)) static flag for the VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API is intended to be used by WEB/SNMP
*
* @end
*********************************************************************/
L7_RC_t daiVlanArpAclStaticFlagSet(L7_uint32 vlanId, L7_uint32 staticFlag);

/*********************************************************************
* @purpose  Given a VLAN ID, return the endpoints of the next range of 
*           VLANs enabled for ARP ACLs.
*
* @param    vlanStart @b((input/output)) Low end of VLAN range
* @param    vlanEnd   @b((input/output)) High end of VLAN range
* @param    aclName   @b((output)) ARP ACL name for this VLAN range
* @param    staticFlag @b((output)) static Flag for this ACL filter
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API intended to be used by show run, so that show run
*           doesn't have to list each enabled VLAN individually. Search
*           for next range starts at vlanEnd + 1. 
*
*           If no VLANs are enabled for DAI beyond vlanIn, 
*           returns L7_SUCCESS with vlanStart = 0.
*
*           aclName should have memory allocated by the caller already.
*
* @end
*********************************************************************/
L7_RC_t daiVlanNextArpAclRangeGet(L7_uint32 *vlanStart,
                                  L7_uint32 *vlanEnd,
                                  L7_uchar8 *aclName,
                                  L7_uint32 *staticFlag);

/*********************************************************************
* @purpose  Apply the ARP ACL to a range of VLANs
*
* @param    aclName   @b((input))  ACL name
* @param    vlanStart @b((input)) Low end of VLAN range
* @param    vlanEnd   @b((input)) High end of VLAN range
* @param    val       @b((input)) L7_ENABLE if ARP ACL is enabled
*                                 on all VLANs in the range
* @param    staticFlag @b((input)) If static is enabled, packets that do
*                      not match any permit statements of ARP ACL are
*                      dropped without consulting the DHCP snooping bindings.
*
* @returns  L7_SUCCESS if entry added.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t daiVlanARPAclRangeSet(L7_uchar8 *aclName, L7_uint32 vlanStart,
                              L7_uint32 vlanEnd, L7_uint32 val, L7_uint32 staticFlag);

/*********************************************************************
* @purpose  API to get the rate limit value of incoming ARP packets on
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
L7_RC_t daiIntfRateLimitGet(L7_uint32 intIfNum, L7_int32 *rate);

/*********************************************************************
* @purpose  API to get the burst interval value of incoming ARP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    burst_interval  @b((output)) the consecutive interval in
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
L7_RC_t daiIntfBurstIntervalGet(L7_uint32 intIfNum, L7_uint32 *burst_interval);

/*********************************************************************
* @purpose  API to set the rate limit value of incoming ARP packets on
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
L7_RC_t daiIntfRateLimitSet(L7_uint32 intIfNum, L7_int32 rate);

/*********************************************************************
* @purpose  API to set the burst interval value of incoming ARP packets on
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
L7_RC_t daiIntfBurstIntervalSet(L7_uint32 intIfNum, L7_uint32 burst_interval);

/*********************************************************************
* @purpose  Get DAI statistics on a VLAN
*
* @param    vlanId    @b((input))  VLAN Id
* @param    stats     @b((output)) stats for given Vlan 
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*           L7_ERROR  if Vlan is invalid
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t daiVlanStatsGet(L7_uint32 vlanId, daiVlanStats_t *stats);

/*********************************************************************
* @purpose  Clear DAI statistics on all vlans
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*           L7_ERROR  if Vlan is invalid
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t daiVlanStatsClear(void);

/********************************************************************/
/*                   ARP ACL Related APIs                           */
/*                                                                  */
/* ARP ACLs are part of Dynamic ARP Inspection as they are used     */
/* only by DAI.                                                     */
/* Later when it becomes something common in Fastpath               */
/* we can move these APIs to different files                        */
/********************************************************************/

/*********************************************************************
* @purpose  API to create ARP ACL
*
* @param    aclName @b((input))  ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if the max number of ARP ACLs reached already
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclCreate(L7_uchar8 *aclName);

/*********************************************************************
* @purpose  API to delete ARP ACL
*
* @param    aclName @b((input))  ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if no ARP ACL exists with that name
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclDelete(L7_uchar8 *aclName);

/*********************************************************************
* @purpose  API to Check if an ARP ACL exists
*
* @param    aclName @b((input))  ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclGet(L7_uchar8 *aclName);

/*********************************************************************
* @purpose  API to get the next ARP ACL entry
*
* @param    aclName     @b((input))  ACL Name
* @param    nextAclName @b((output)) Next ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*           memory for nextAclName should have been allocated by
*           the caller of this function
*
* @end
*********************************************************************/
L7_RC_t arpAclNextGet(L7_uchar8 *aclName, L7_uchar8 *nextAclName);

/*********************************************************************
* @purpose  API to add rule to ARP ACL
*
* @param    aclName @b((input))  ACL Name
* @param    ipAddr  @b((input))  Sender's IP address
* @param    macAddr @b((input))  Sender's MAC address in 
*                                xx:xx:xx:xx:xx:xx string format 
*
* @returns  L7_SUCCESS
*           L7_FAILURE if the max number of rules in ARP ACL have
*                      already reached
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleAdd(L7_uchar8 *aclName, L7_uint32 ipAddr, L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  API to remove rule from ARP ACL
*
* @param    aclName @b((input))  ACL Name
* @param    ipAddr  @b((input))  Sender's IP address
* @param    macAddr @b((input))  Sender's MAC address in 
*                                xx:xx:xx:xx:xx:xx string format 
*
* @returns  L7_SUCCESS
*           L7_FAILURE if no ARP ACL exists or if the rule doesn't exist
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleDelete(L7_uchar8 *aclName, L7_uint32 ipAddr, L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  API to Check if an ARP ACL Rule exists
*
* @param    aclName @b((input))  ACL Name
* @param    ipAddr  @b((input))  Sender's IP address
* @param    macAddr @b((input))  Sender's MAC address in 
*                                xx:xx:xx:xx:xx:xx string format 
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleGet(L7_uchar8 *aclName, L7_uint32 ipAddr, L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  API to get the next ARP ACL Rule
*
* @param    aclNameIn @b((input))  ACL Name
* @param    ipAddrIn  @b((input))  Sender's IP address
* @param    macAddrIn @b((input))  Sender's MAC address in
*                                  xx:xx:xx:xx:xx:xx string format
* @param    aclNameOut @b((output))  ACL Name
* @param    ipAddrOut  @b((output))  Sender's IP address
* @param    macAddrOut @b((output))  Sender's MAC address in
*                                    xx:xx:xx:xx:xx:xx string format
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleNextGet(L7_uchar8 *aclNameIn, L7_uint32 ipAddrIn, L7_uchar8 *macAddrIn,
                          L7_uchar8 *aclNameOut, L7_uint32 *ipAddrOut, L7_uchar8 *macAddrOut);

/*********************************************************************
* @purpose  API to get the next ARP ACL Rule in the given ACL
*
* @param    aclNameIn @b((input))  ACL Name
* @param    ipAddrIn  @b((input))  Sender's IP address
* @param    macAddrIn @b((input))  Sender's MAC address in
*                                  xx:xx:xx:xx:xx:xx string format
* @param    ipAddrOut  @b((output))  Sender's IP address
* @param    macAddrOut @b((output))  Sender's MAC address in
*                                    xx:xx:xx:xx:xx:xx string format
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    This API would be used by CLI and WEB
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleInAclNextGet(L7_uchar8 *aclName, L7_uint32 ipAddrIn, L7_uchar8 *macAddrIn,
                               L7_uint32 *ipAddrOut, L7_uchar8 *macAddrOut);

#endif /* DAI_API_H */

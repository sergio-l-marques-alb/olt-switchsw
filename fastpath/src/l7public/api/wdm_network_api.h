/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_network_api.h
*
* @purpose      Wireless Data Manager Network API header
*
* @component    WDM
*
* @comments     none
*
* @create       10/20/2005
*
* @author       dfowler
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WDM_NETWORK_API_H
#define INCLUDE_WDM_NETWORK_API_H

#include "datatypes.h"
#include "comm_structs.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "acl_exports.h"
#include "acl_exports.h"

/*********************************************************************
*
* @purpose  Add an entry to the network configuration table.
*
* @param    L7_uint32 networkId @b{(input)} Network ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkEntryAdd(L7_uint32 networkId);

/*********************************************************************
*
* @purpose  Check whether the specified network is assigned to any VAPs.
*
* @param    L7_uint32 networkId @b{(input)} Network ID
* @param    L7_BOOL inUse @b{(output)} Flag indicating whether the network
*		    is assigned to a VAP.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkUsageCheck(L7_uint32 networkId, L7_BOOL *inUse);

/*********************************************************************
*
* @purpose  Delete an entry from the network table.
*
* @param    L7_uint32 networkId @b{(input)} Network ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkEntryDelete(L7_uint32 networkId);

/*********************************************************************
*
* @purpose  Verify an entry in the network table.
*
* @param    L7_uint32 networkId @b{(input)} Network ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkEntryGet(L7_uint32 networkId);

/*********************************************************************
*
* @purpose  Get the next entry from the network table.
*
* @param    L7_uint32 *networkId @b{(input/output)} Network ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkEntryNextGet(L7_uint32 *networkId);

/*********************************************************************
*
* @purpose  Determine if a network is applied to one or more enabled VAPs.
*
* @param    L7_uint32 networkId @b{(input)}  Network ID
*
* @returns  L7_SUCCESS, network is applied.
* @returns  L7_FAILURE, invalid network, WS is disabled, or network not applied.
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkVAPEnabled(L7_uint32 networkId);

/*********************************************************************
*
* @purpose  Restore default settings for a network entry.
*
* @param    L7_uint32 networkId @b{(input)} Network ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkEntryClear(L7_uint32 networkId);

/*********************************************************************
* @purpose  Set the Network SSID.
*
* @param    L7_uint32  networkId    @b{(input)} Network ID 
* @param    L7_char8  *ssid         @b{(input)} SSID 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkSSIDSet(L7_uint32  networkId,
                          L7_char8  *ssid);

/*********************************************************************
* @purpose  Set the Network default VLAN ID.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 vlanId       @b{(input)} Default VLAN
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkDefaultVlanIdSet(L7_uint32 networkId,
                                   L7_uint32 vlanId);

/*********************************************************************
* @purpose  Set the Network hide SSID mode.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 mode         @b{(input)} Hide SSID mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkHideSSIDModeSet(L7_uint32 networkId,
                                  L7_uint32 mode);

/*********************************************************************
* @purpose  Sets Network deny broadcast mode.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 mode         @b{(input)} deny broadcast mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkDenyBroadcastModeSet(L7_uint32 networkId,
                                       L7_uint32 mode);

/*********************************************************************
* @purpose  Sets Network L3 tunneling mode.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 mode         @b{(input)} tunneling mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkL3TunnelingModeSet(L7_uint32 networkId,
                                     L7_uint32 mode);

/*********************************************************************
* @purpose  Sets Network L3 tunneling subnet IP address and mask.
*
* @param    L7_uint32    networkId    @b{(input)} Network ID 
* @param    L7_IP_ADDR_t ip           @b{(input)} tunneling subnet IP
* @param    L7_IP_ADDR_t mask         @b{(input)} tunneling subnet mask
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkDefaultL3TunnelIPAndMaskSet(L7_uint32    networkId,
                                              L7_IP_ADDR_t ip,
                                              L7_IP_ADDR_t mask);

/*********************************************************************
* @purpose  Sets Network L3 tunneling subnet IP address.
*
* @param    L7_uint32    networkId    @b{(input)} Network ID 
* @param    L7_IP_ADDR_t ip           @b{(input)} tunneling subnet IP
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkDefaultL3TunnelIPSet(L7_uint32    networkId,
                                       L7_IP_ADDR_t ip);

/*********************************************************************
* @purpose  Sets Network L3 tunneling subnet mask.
*
* @param    L7_uint32    networkId    @b{(input)} Network ID 
* @param    L7_IP_ADDR_t mask         @b{(input)} tunneling subnet mask
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkDefaultL3TunnelMaskSet(L7_uint32    networkId,
                                         L7_IP_ADDR_t mask);

/*********************************************************************
* @purpose  Set the Network security mode.
*
* @param    L7_uint32              networkId    @b{(input)} Network ID 
* @param    L7_WDM_SECURITY_MODE_t mode         @b{(input)} Security Mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkSecurityModeSet(L7_uint32              networkId,
                                  L7_WDM_SECURITY_MODE_t mode);

/*********************************************************************
*
* @purpose  Set the Network Authentication Mode for Static(Shared Key) WEP
*
* @param    L7_uint32              networkId    @b{(input)} Network ID 
* @param    L7_WDM_WEP_AUTH_TYPE_t mode         @b{(input)} Authentication Mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPAuthModeSet(L7_uint32 networkId,
                            L7_WDM_WEP_AUTH_TYPE_t mode);

/*********************************************************************
* @purpose  Sets Network WPA versions supported.
*
* @param    L7_uint32                       networkId    @b{(input)} Network ID 
* @param    L7_WDM_WPA_SUPPORTED_VERSIONS_t version      @b{(input)} WPA versions 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPAVersionsSupportedSet(L7_uint32 networkId,
                                          L7_WDM_WPA_SUPPORTED_VERSIONS_t version);

/*********************************************************************
* @purpose  Sets Network cipher suites supported.
*
* @param    L7_uint32                  networkId  @b{(input)} Network ID 
* @param    L7_WDM_WPA_CIPHER_SUITES_t cipher     @b{(input)} cipher suites 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPACipherSuitesSet(L7_uint32 networkId,
                                     L7_WDM_WPA_CIPHER_SUITES_t cipher);

/*********************************************************************
* @purpose  Set the Network MAC authentication mode.
*
* @param    L7_uint32              networkId    @b{(input)} Network ID 
* @param    L7_WDM_MAC_AUTH_MODE_t mode         @b{(input)} MAC authentication mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkMACAuthModeSet(L7_uint32 networkId,
                                 L7_WDM_MAC_AUTH_MODE_t mode);

/*********************************************************************
* @purpose  Set Network client QoS mode.
*
* @param    networkId   @b{(input)} network id 
* @param    mode        @b{(input)} client QoS mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkClientQosModeSet(L7_uint32 networkId,
                                   L7_uint32 mode);

/*********************************************************************
* @purpose  Set the Network client default ACL for the specified direction.
*
* @param    networkId   @b{(input)} network id 
* @param    dir         @b{(input)} direction 
* @param    aclType     @b{(input)} ACL type 
* @param    aclName     @b{(input)} ACL name string
*
* returns   L7_SUCCESS
* returns   L7_TABLE_IS_FULL      Too many different ACLs being used for wireless
* returns   L7_REQUEST_DENIED     ACL is incompatible with wireless use
* returns   L7_FAILURE
*
* @comments The aclName is passed as a string, even if it is an IPv4 ACL number.
*
* @comments The ACL entry is reset to default by passing an aclType of 
*           L7_ACL_TYPE_NONE (the aclName parameter is ignored).
*
* @comments An L7_TABLE_IS_FULL response means too many different ACLs
*           are assigned to wireless networks for client QoS.  For a given
*           AP profile, wireless imposes a finite upper limit on the number
*           of access list definitions that the AP can support.
*
* @comments An L7_REQUEST_DENIED response means the ACL definition is not
*           valid for use with a wireless network.  This usually means that
*           it contains more rules than wireless is able to support.
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkClientQosAclSet(L7_uint32 networkId,
                                  L7_WDM_CLIENT_QOS_DIR_t dir,
                                  L7_ACL_TYPE_t aclType,
                                  L7_char8 *aclName);

/*********************************************************************
* @purpose  Set the Network client default bandwidth maximum rate for the specified direction.
*
* @param    networkId   @b{(input)} network id 
* @param    dir         @b{(input)} direction 
* @param    maxRate     @b{(input)} maximum rate (in bits-per-second)
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments Setting a maxRate value of L7_WDM_NETWORK_CLIENT_QOS_BW_NONE 
*           turns off rate limiting in the specified direction for this
*           network.
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkClientQosBandwidthSet(L7_uint32 networkId,
                                        L7_WDM_CLIENT_QOS_DIR_t dir,
                                        L7_uint32 maxRate);

/*********************************************************************
* @purpose  Set the Network client default DiffServ policy for the specified direction.
*
* @param    networkId   @b{(input)} network id 
* @param    dir         @b{(input)} direction 
* @param    policyType  @b{(input)} DiffServ policy type 
* @param    *policyName @b{(input)} DiffServ policy name string
*
* returns   L7_SUCCESS
* returns   L7_TABLE_IS_FULL      Too many different policies being used for wireless
* returns   L7_REQUEST_DENIED     Policy is incompatible with wireless use
* returns   L7_FAILURE
*
* @comments The DiffServ policy entry is reset to default by passing a policyType of 
*           L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE (the policyName parameter is ignored).
*
* @comments An L7_TABLE_IS_FULL response means too many different policies or classes
*           are assigned to wireless networks for client QoS.  For a given
*           AP profile, wireless imposes a finite upper limit on the number
*           of policy and class definitions that the AP can support.
*
* @comments An L7_REQUEST_DENIED response means the policy definition is not
*           valid for use with a wireless network.  This usually means that
*           it contains more instances than wireless is able to support.
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkClientQosPolicySet(L7_uint32 networkId,
                                     L7_WDM_CLIENT_QOS_DIR_t dir,
                                     L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType,
                                     L7_char8 *policyName);

/*********************************************************************
* @purpose  Set the Network RADIUS accounting mode.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 mode         @b{(input)} radius accounting mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkRadiusAccountingModeSet(L7_uint32 networkId,
                                          L7_uint32 mode);

/*********************************************************************
* @purpose  Set the Network WEP key length.
*
* @param    L7_uint32                networkId    @b{(input)} Network ID 
* @param    L7_WDM_WEP_KEY_LENGTH_t  length       @b{(input)} WEP key length
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPKeyLengthSet(L7_uint32               networkId,
                                  L7_WDM_WEP_KEY_LENGTH_t length);

/*********************************************************************
* @purpose  Set the Network WEP key type.
*
* @param    L7_uint32                 networkId @b{(input)} Network ID 
* @param    L7_WDM_WEP_WPA_KEY_TYPE_t type      @b{(input)} WEP key type 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPKeyTypeSet(L7_uint32 networkId,
                                L7_WDM_WEP_WPA_KEY_TYPE_t type);

/*********************************************************************
* @purpose  Set the Network WEP key index to be used.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 index        @b{(input)} WEP key index
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPTransferKeyIndexSet(L7_uint32 networkId,
                                         L7_uint32 index);

/*********************************************************************
* @purpose  Sets Network WEP key.
*
* @param    L7_uint32  networkId    @b{(input)} Network ID 
* @param    L7_uint32  index        @b{(input)} WEP key index
* @param    L7_char8  *key          @b{(input)} WEP key
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPKeySet(L7_uint32  networkId,
                            L7_uint32  index,
                            L7_char8  *key);

/*********************************************************************
* @purpose  Set the Network WPA key type.
*
* @param    L7_uint32                 networkId    @b{(input)} Network ID 
* @param    L7_WDM_WEP_WPA_KEY_TYPE_t type         @b{(input)} WPA key type
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPAKeyTypeSet(L7_uint32 networkId,
                                L7_WDM_WEP_WPA_KEY_TYPE_t type);

/*********************************************************************
* @purpose  Set the Network WPA key.
*
* @param    L7_uint32  networkId    @b{(input)} Network ID 
* @param    L7_char8  *key          @b{(input)} WPA key 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPAKeySet(L7_uint32  networkId,
                            L7_char8  *key);

/*********************************************************************
* @purpose  Set the Network WPA2 pre-authentication mode.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 mode         @b{(input)} WPA2 pre-authentication mode.
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPA2PreAuthModeSet(L7_uint32 networkId,
                                     L7_uint32 mode);

/*********************************************************************
* @purpose  Sets Network  WPA2 pre-authentication limit.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 limit        @b{(input)} WPA2 pre-authentication limit.
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPA2PreAuthLimitSet(L7_uint32 networkId,
                                      L7_uint32 limit);

/*********************************************************************
* @purpose  Sets Network WPA2 dynamic key forwarding mode.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 mode         @b{(input)} key forwarding mode.
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPA2DynamicKeyFwModeSet(L7_uint32 networkId,
                                          L7_uint32 mode);

/*********************************************************************
* @purpose  Set the Network WPA2 roam-back key caching holdtime.
*
* @param    L7_uint32 networkId    @b{(input)} Network ID 
* @param    L7_uint32 time         @b{(input)} key caching holdtime.
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPA2RoambackKeyCacheHoldTimeSet(L7_uint32 networkId,
                                                  L7_uint32 time);

/*********************************************************************
* @purpose  Get the Network SSID. 
*
* @param    L7_uint32   networkId    @b{(input)}  Network ID 
* @param    L7_char8   *ssid         @b{(output)} Network SSID 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkSSIDGet(L7_uint32   networkId,
                          L7_char8   *ssid);

/*********************************************************************
* @purpose  Verify an SSID entry in the network table.
*
* @param    L7_char8   *ssid         @b{(output)} Network SSID 
* @param    L7_char8   *networkId    @b{(output)} Network ID for this SSID
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkSSIDEntryGet(L7_char8 *ssid, L7_uint32 *networkId);

/*********************************************************************
* @purpose  Get Network default VLAN ID. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *vlanId       @b{(output)} Default VLAN ID  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkDefaultVlanIdGet(L7_uint32  networkId,
                                   L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Get Network hide SSID mode. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *mode         @b{(output)} hide SSID mode  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkHideSSIDModeGet(L7_uint32  networkId,
                                  L7_uint32 *mode);

/*********************************************************************
* @purpose  Get Network deny broadcast mode. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *mode         @b{(output)} deny broadcast mode  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkDenyBroadcastModeGet(L7_uint32  networkId,
                                       L7_uint32 *mode);

/*********************************************************************
* @purpose  Get Network L3 tunneling mode. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *mode         @b{(output)} tunneling mode  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkL3TunnelingModeGet(L7_uint32  networkId,
                                     L7_uint32 *mode);

/*********************************************************************
* @purpose  Get Network L3 default tunnel subnet IP address. 
*
* @param    L7_uint32     networkId    @b{(input)}  Network ID 
* @param    L7_IP_ADDR_t *ip           @b{(output)} tunneling subnet ip  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkDefaultL3TunnelIPGet(L7_uint32     networkId,
                                       L7_IP_ADDR_t *ip);

/*********************************************************************
* @purpose  Get Network L3 default tunneling subnet mask. 
*
* @param    L7_uint32     networkId    @b{(input)}  Network ID 
* @param    L7_IP_ADDR_t *mask         @b{(output)} tunneling subnet mask  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkDefaultL3TunnelMaskGet(L7_uint32     networkId,
                                         L7_IP_ADDR_t *mask);

/*********************************************************************
* @purpose  Get Network security mode. 
*
* @param    L7_uint32               networkId    @b{(input)} Network ID 
* @param    L7_WDM_SECURITY_MODE_t *mode         @b{(output)} Security Mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkSecurityModeGet(L7_uint32 networkId,
                                  L7_WDM_SECURITY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get the Network Authentication Mode for Static(Shared Key) WEP
*
* @param    L7_uint32              networkId    @b{(input)} Network ID 
* @param    L7_WDM_WEP_AUTH_TYPE_t *mode        @b{(output)} Authentication Mode
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPAuthModeGet(L7_uint32 networkId,
                                  L7_WDM_WEP_AUTH_TYPE_t *mode);

/*********************************************************************
* @purpose  Get Network WPA versions supported. 
*
* @param    L7_uint32                        networkId    @b{(input)} Network ID 
* @param    L7_WDM_WPA_SUPPORTED_VERSIONS_t *version      @b{(input)} WPA versions 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPAVersionsSupportedGet(L7_uint32 networkId,
                                          L7_WDM_WPA_SUPPORTED_VERSIONS_t *version);

/*********************************************************************
* @purpose  Get Network WPA cipher suites supported. 
*
* @param    L7_uint32                   networkId    @b{(input)}  Network ID 
* @param    L7_WDM_WPA_CIPHER_SUITES_t *cipher       @b{(output)} WPA cipher suites  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPACipherSuitesGet(L7_uint32 networkId,
                                     L7_WDM_WPA_CIPHER_SUITES_t *cipher);

/*********************************************************************
* @purpose  Get Network MAC authentication mode. 
*
* @param    L7_uint32               networkId    @b{(input)} Network ID 
* @param    L7_WDM_MAC_AUTH_MODE_t *mode         @b{(output)} MAC authentication mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkMACAuthModeGet(L7_uint32               networkId,
                                 L7_WDM_MAC_AUTH_MODE_t *mode);

/*********************************************************************
* @purpose  Get Network client QoS mode. 
*
* @param    networkId   @b{(input)}  network id 
* @param    *mode       @b{(output)} client QoS mode ptr
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkClientQosModeGet(L7_uint32  networkId,
                                   L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the Network client default ACL for the specified direction.
*
* @param    networkId   @b{(input)}  network id 
* @param    dir         @b{(input)}  direction 
* @param    *aclType    @b{(output)} ACL type ptr
* @param    *aclName    @b{(output)} ACL name string output ptr
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments Caller must supply an aclName output buffer of at least
*           L7_ACL_NAME_LEN_MAX+1 bytes in length.
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkClientQosAclGet(L7_uint32 networkId,
                                  L7_WDM_CLIENT_QOS_DIR_t dir,
                                  L7_ACL_TYPE_t *aclType,
                                  L7_char8 *aclName);

/*********************************************************************
* @purpose  Get the Network client default bandwidth maximum rate for the specified direction.
*
* @param    networkId   @b{(input)}  network id 
* @param    dir         @b{(input)}  direction 
* @param    *maxRate    @b{(output)} maximum rate output ptr (in bits-per-second)
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments A maxRate value of L7_WDM_NETWORK_CLIENT_QOS_BW_NONE means
*           rate limiting is turned off in the specified direction for this
*           network.
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkClientQosBandwidthGet(L7_uint32 networkId,
                                        L7_WDM_CLIENT_QOS_DIR_t dir,
                                        L7_uint32 *maxRate);

/*********************************************************************
* @purpose  Get the Network client default DiffServ policy for the specified direction.
*
* @param    networkId   @b{(input)}  network id 
* @param    dir         @b{(input)}  direction 
* @param    *policyType @b{(output)} DiffServ policy type ptr
* @param    *policyName @b{(output)} DiffServ policy name output ptr
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments Caller must supply a policyName output buffer of at least
*           L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1 bytes in length.
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkClientQosPolicyGet(L7_uint32 networkId,
                                     L7_WDM_CLIENT_QOS_DIR_t dir,
                                     L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t *policyType,
                                     L7_char8 *policyName);

/*********************************************************************
* @purpose  Get Network AP Radius Accounting mode. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *mode         @b{(output)} Accounting mode  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkRadiusAccountingModeGet(L7_uint32  networkId,
                                          L7_uint32 *mode);

/*********************************************************************
* @purpose  Get Network WEP key length. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *length       @b{(output)} WEP key length  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPKeyLengthGet(L7_uint32  networkId,
                                  L7_uint32 *length);

/*********************************************************************
*
* @purpose  Get Network WEP key character length.
*
* @param    L7_WDM_WEP_WPA_KEY_TYPE_t type @b{(input)} Key Type
* @param    L7_WDM_WEP_KEY_LENGTH_t keyLength @b{(input)} Key Length chosen
* @param    L7_uint32 *length       @b{(output)} WEP key character length
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPKeyCharacterLengthGet (L7_WDM_WEP_WPA_KEY_TYPE_t keyType,
                                  L7_WDM_WEP_KEY_LENGTH_t keyLength,
                                  L7_uint32 *charLength);

/*********************************************************************
* @purpose  Get Network WEP key type. 
*
* @param    L7_uint32                  networkId @b{(input)} Network ID 
* @param    L7_WDM_WEP_WPA_KEY_TYPE_t *type      @b{(output)} WEP key type 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPKeyTypeGet(L7_uint32 networkId,
                                L7_WDM_WEP_WPA_KEY_TYPE_t *type);

/*********************************************************************
* @purpose  Get Network WEP key index used. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *index        @b{(output)} WEP key index
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPTransferKeyIndexGet(L7_uint32  networkId,
                                         L7_uint32 *index);

/*********************************************************************
* @purpose  Get Network WPA key type. 
*
* @param    L7_uint32                  networkId  @b{(input)}  Network ID 
* @param    L7_WDM_WEP_WPA_KEY_TYPE_t *type       @b{(output)} WPA key type
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPAKeyTypeGet(L7_uint32 networkId,
                                L7_WDM_WEP_WPA_KEY_TYPE_t *type);

/*********************************************************************
* @purpose  Get Network WPA2 pre-authentication mode. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *mode         @b{(output)} pre-authentication mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPA2PreAuthModeGet(L7_uint32  networkId,
                                     L7_uint32 *mode);

/*********************************************************************
* @purpose  Get Network WPA2 pre-authentication limit. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *limit        @b{(output)} pre-authentication limit 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPA2PreAuthLimitGet(L7_uint32  networkId,
                                      L7_uint32 *limit);

/*********************************************************************
* @purpose  Get Network WPA2 dynamic key forwarding mode. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *mode         @b{(output)} key forwarding mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPA2DynamicKeyFwModeGet(L7_uint32  networkId,
                                          L7_uint32 *mode);

/*********************************************************************
* @purpose  Get Network WPA2 roam-back key caching holdtime. 
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID 
* @param    L7_uint32 *time         @b{(output)} holdtime 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPA2RoambackKeyCacheHoldTimeGet(L7_uint32  networkId,
                                                  L7_uint32 *time);

/*********************************************************************
* @purpose  Get Network WPA key. 
*
* @param    L7_uint32 networkId    @b{(input)}   Network ID 
* @param    L7_char8 *key          @b{(output)}  WPA key
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWPAKeyGet(L7_uint32 networkId,
                            L7_char8 *key);

/*********************************************************************
* @purpose  Get Network WEP key. 
*
* @param    L7_uint32 networkId    @b{(input)}  Network ID 
* @param    L7_uint32 index        @b{(input)}  WEP key index
* @param    L7_char8 *key          @b{(output)} WEP key 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkWEPKeyGet(L7_uint32 networkId,
                            L7_uint32 index, 
                            L7_char8 *key);

/*********************************************************************
* @purpose  Sets network tunnel configuration status.
*
* @param    L7_uint32               networkId @b{(input)}  Network ID 
* @param    L7_WDM_TUNNEL_STATUS_t  status    @b{(input)}  tunnel status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkTunnelStatusSet(L7_uint32              networkId,
                                  L7_WDM_TUNNEL_STATUS_t status);

/*********************************************************************
* @purpose  Get network tunnel configuration status
*
* @param    L7_uint32               networkId @b{(input)}  Network ID 
* @param    L7_WDM_TUNNEL_STATUS_t *status    @b{(output)} tunnel status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkTunnelStatusGet(L7_uint32               networkId,
                                  L7_WDM_TUNNEL_STATUS_t *status);


/*********************************************************************
* @purpose  Set the Network redirect mode.
*
* @param    L7_uint32              networkId    @b{(input)} Network ID 
* @param    L7_WDM_REDIRECT_MODE_t mode         @b{(input)} Redirect Mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkRedirectModeSet(L7_uint32              networkId,
                                  L7_WDM_REDIRECT_MODE_t mode);


/*********************************************************************
* @purpose  Set the Network redirect URL.
*
* @param    L7_uint32              networkId    @b{(input)} Network ID 
* @param    L7_char8 mode         @b{(input)} Redirect URL 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkRedirectURLSet(L7_uint32  networkId,
                                 L7_char8   *URL);


/*********************************************************************
* @purpose  Set the Network redirect IP Address.
*
* @param    L7_uint32     networkId    @b{(input)} Network ID 
* @param    L7_IP_ADDR_t  IP         @b{(input)} Redirect IP
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkRedirectIPSet(L7_uint32     networkId,
                                L7_IP_ADDR_t  IP);


/*********************************************************************
* @purpose  Get Network redirect mode. 
*
* @param    L7_uint32               networkId    @b{(input)} Network ID 
* @param    L7_WDM_REDIRECT_MODE_t *mode         @b{(output)} Redirect Mode 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkRedirectModeGet(L7_uint32 networkId,
                                  L7_WDM_REDIRECT_MODE_t *mode);

/*********************************************************************
* @purpose  Get Network Redirect URL. 
*
* @param    L7_uint32               networkId    @b{(input)} Network ID 
* @param    L7_char8 *mode         @b{(output)} Redirect URL 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkRedirectURLGet(L7_uint32 networkId,
                                 L7_char8 *URL);

/*********************************************************************
* @purpose  Get Network redirect IP. 
*
* @param    L7_uint32               networkId    @b{(input)} Network ID 
* @param    L7_IP_ADDR_t IP         @b{(output)} Redirect IP 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkRedirectIPGet(L7_uint32 networkId,
                                L7_IP_ADDR_t *IP);

/*********************************************************************
* @purpose  Get Network Interface Number. 
*
* @param    L7_uint32               networkId    @b{(input)} Network ID 
* @param    L7_uint32               @b{(output)} Network Interface Number 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmIfNumberGet(L7_uint32 networkId, L7_uint32 *ifNumber);

/*********************************************************************
* @purpose  Get Network Interface Number. 
*
* @param    L7_uint32     ifNumber    @b{(input)}  Network Interface Number 
* @param    L7_uint32     networkId   @b{(output)} Network ID
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkIDGet(L7_uint32 ifNumber, L7_uint32 *networkId);

/*********************************************************************
* @purpose  Get pointer to Captive Portal call back functions from WDM
* @param    void *cpCbPtr       pointer to a pointer of structure of  
*                               wireless CP call back function pointers
*
* @notes    none
*            
*       
* @end
*********************************************************************/
void wdmCpCallbackGet(void **cpCbPtr);

/*********************************************************************
* @purpose  Sets Network RADIUS auth server name.
*
* @param    L7_uint32  networkId    @b{(input)} Network ID 
* @param    L7_char8   *name        @b{(input)} radius server name 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkAuthRadiusServerNameSet(L7_uint32  networkId, 
                                          L7_char8  *name );

/*********************************************************************
* @purpose  Get Network RADIUS auth server name.
*
* @param    L7_uint32  networkId    @b{(input)} Network ID 
* @param    L7_char8   *name        @b{(output)} radius server name 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkAuthRadiusServerNameGet(L7_uint32  networkId, 
                                          L7_char8  *name );

/*********************************************************************
* @purpose  Sets Network RADIUS acct server name.
*
* @param    L7_uint32  networkId    @b{(input)} Network ID 
* @param    L7_char8   *name        @b{(input)} radius server name 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkAcctRadiusServerNameSet(L7_uint32  networkId, 
                                          L7_char8  *name );

/*********************************************************************
* @purpose  Get Network RADIUS acct server name.
*
* @param    L7_uint32  networkId    @b{(input)} Network ID 
* @param    L7_char8   *name        @b{(output)} radius server name 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkAcctRadiusServerNameGet(L7_uint32  networkId, 
                                          L7_char8  *name );

/*********************************************************************
* @purpose  Enable/Disable Network RADIUS server configuration usage.
*
* @param    L7_uint32  networkId    @b{(input)} Network ID 
* @param    L7_uint32  mode         @b{(input)} mode enable/disable 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkUseRadiusConfigSet(L7_uint32  networkId, 
                                     L7_uint32  mode );

/*********************************************************************
* @purpose  Get Network RADIUS server configuration usage mode.
*
* @param    L7_uint32  networkId    @b{(input)} Network ID 
* @param    L7_uint32  *mode        @b{(output)} mode  
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkUseRadiusConfigGet(L7_uint32  networkId, 
                                     L7_uint32  *mode );

/*********************************************************************
*
* @purpose Get the network authentication server configuration status.
*
* @param    L7_uint32   networkId    @b{(input)} Network ID 
* @param    L7_uint32   *value       @b{(output)} pointer to store the status.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*       
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkAuthRadiusConfigurationGet(L7_uint32 networkId,
                                             L7_uint32 *value);

/*********************************************************************
*
* @purpose Get the network accounting server configuration status.
*
* @param    L7_uint32   networkId    @b{(input)} Network ID 
* @param    L7_uint32   *value       @b{(output)} pointer to store the status.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*       
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkAcctRadiusConfigurationGet(L7_uint32 networkId, 
                                             L7_uint32 *value);

/*********************************************************************
* @purpose  Get Network L2 Distributed tunneling mode.
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID
* @param    L7_uint32 *mode         @b{(output)} tunneling mode
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkL2DistTunnelModeGet(L7_uint32 networkId, 
                                       L7_uint32 *mode);
/*********************************************************************
* @purpose  Set Network L2 Distributed tunneling mode.
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID
* @param    L7_uint32  mode         @b{(input)}  tunneling mode
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkL2DistTunnelModeSet(L7_uint32 networkId, 
                                       L7_uint32 mode);

/*********************************************************************
* @purpose  Set Network Bcast Key Refresh Rate
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID
* @param    L7_uint32  rate         @b{(input)}  Bcast key Refresh rate
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkBcastKeyRefreshRateSet (L7_uint32 networkId, L7_uint32 rate);
/*********************************************************************
* @purpose  Set Network Session Key Refresh Rate
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID
* @param    L7_uint32  rate         @b{(input)}  Session key Refresh rate
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkSessionKeyRefreshRateSet (L7_uint32 networkId, L7_uint32 rate);

/*********************************************************************
* @purpose  Get Network Bcast Key Refresh Rate
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID
* @param    L7_uint32  rate         @b{(output)}  Bcast key Refresh rate
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkBcastKeyRefreshRateGet (L7_uint32 networkId, L7_uint32 *rate);
/*********************************************************************
* @purpose  Get Network Session Key Refresh Rate
*
* @param    L7_uint32  networkId    @b{(input)}  Network ID
* @param    L7_uint32  rate         @b{(output)}  Session key Refresh rate
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmNetworkSessionKeyRefreshRateGet (L7_uint32 networkId, L7_uint32 *rate);

/*********************************************************************
*
* @purpose  Get the network Wireless ARP Suppression Mode.
*
* @param    L7_uint32   networkId   @b{(input)} Network ID 
* @param    L7_uint32   *value      @b{(output)} pointer to store the mode.
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @notes    none
*       
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkWirelessARPSuppModeGet (L7_uint32 networkId, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the network Wireless ARP Suppression Mode.
*
* @param    L7_uint32    networkId  @b{(input)} Network ID 
* @param    L7_uint32    value      @b{(input)} Wireless ARP Supp. Mode.
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @notes    none
*       
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkWirelessARPSuppModeSet (L7_uint32 networkId, L7_uint32 value);

#endif /* INCLUDE_WDM_NETWORK_API_H */

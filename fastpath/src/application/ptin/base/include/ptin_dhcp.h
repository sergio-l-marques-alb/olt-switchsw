/**
 * ptin_dhcp.h
 *  
 * Implements the DHCP snooping interface module
 *
 * Created on: 2012/01/06
 * Author: Milton Ruas (milton-r-silva@ext.ptinovacao.pt)
 *  
 * Notes:
 */

#ifndef _PTIN_DHCP_H
#define _PTIN_DHCP_H

#include "ptin_include.h"

/*********************************************************** 
 * Typedefs
 ***********************************************************/

typedef enum  {
  DHCP_STAT_FIELD_RX_INTERCEPTED=0,
  DHCP_STAT_FIELD_RX,
  DHCP_STAT_FIELD_RX_FILTERED,
  DHCP_STAT_FIELD_TX_FORWARDED,
  DHCP_STAT_FIELD_TX_FAILED,

  DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTIONS,
  DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82,
  DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION37,
  DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION18,
  DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTIONS,
  DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82,
  DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION37,
  DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION18,

  DHCP_STAT_FIELD_RX_CLIENT_PKTS_ON_TRUSTED_INTF,
  DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOPS_ON_UNTRUSTED_INTF,
  DHCP_STAT_FIELD_RX_SERVER_PKTS_ON_UNTRUSTED_INTF,
  DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOUTOPS_ON_TRUSTED_INTF,

  DHCP_STAT_FIELD_ALL
} ptin_dhcp_stat_enum_t;

typedef struct _ptin_DHCP_Statistics_t
{
  L7_uint32 dhcp_rx_intercepted;
  L7_uint32 dhcp_rx;                                     //dhcp_rx_valid
  L7_uint32 dhcp_rx_filtered;
  L7_uint32 dhcp_tx_forwarded;
  L7_uint32 dhcp_tx_failed;

  L7_uint32 dhcp_rx_client_requests_without_options;
  L7_uint32 dhcp_tx_client_requests_with_option82;
  L7_uint32 dhcp_tx_client_requests_with_option37;
  L7_uint32 dhcp_tx_client_requests_with_option18;
  L7_uint32 dhcp_rx_server_replies_with_option82;
  L7_uint32 dhcp_rx_server_replies_with_option37;
  L7_uint32 dhcp_rx_server_replies_with_option18;
  L7_uint32 dhcp_tx_server_replies_without_options;

  L7_uint32 dhcp_rx_client_pkts_onTrustedIntf;
  L7_uint32 dhcp_rx_client_pkts_withOps_onUntrustedIntf;
  L7_uint32 dhcp_rx_server_pkts_onUntrustedIntf;
  L7_uint32 dhcp_rx_server_pkts_withoutOps_onTrustedIntf;
} __attribute__ ((packed)) ptin_DHCP_Statistics_t;

extern L7_BOOL ptin_debug_dhcp_snooping;

/*********************************************************** 
 * External function prototypes
 ***********************************************************/

/**
 * Initialize circuitId+remoteId database
 *  
 * @return none
 */
extern L7_RC_t ptin_dhcp_init(void);

/**
 * Set Global enable for DHCP packet trapping
 * 
 * @param enable : L7_ENABLE/L7_DISABLE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_enable(L7_BOOL enable);

/**
 * Check if a EVC is being used in an DHCP instance
 * 
 * @param evcId : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
extern L7_RC_t ptin_dhcp_is_evc_used(L7_uint16 evcId);

/**
 * Creates a DHCP instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_instance_add(L7_uint16 UcastEvcId);

/**
 * Removes a DHCP instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_instance_remove(L7_uint16 UcastEvcId);

/**
 * Update DHCP entries, when EVCs are deleted
 * 
 * @param evcId : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_instance_destroy(L7_uint16 evcId);

/**
 * Get DHCP client data (circuit and remote ids)
 * 
 * @param UcastEvcId  : Unicast evc id
 * @param client      : client identification parameters
 * @param circuitId   : circuit id (output)
 * @param remoteId    : remote id (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_client_get(L7_uint16 UcastEvcId, ptin_client_id_t *client, L7_uint16 *options, L7_char8 *circuitId, L7_char8 *remoteId);

/**
 * Add a new DHCP client
 * 
 * @param UcastEvcId  : Unicast evc id
 * @param client      : client identification parameters
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_client_add(L7_uint16 UcastEvcId, ptin_client_id_t *client, L7_uint16 options, L7_char8 *circuitId, L7_char8 *remoteId);

/**
 * Remove a DHCP client
 * 
 * @param UcastEvcId  : Unicast evc id
 * @param client      : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_client_delete(L7_uint16 UcastEvcId, ptin_client_id_t *client);

/**
 * Get DHCP Binding table
 * 
 * @param table       : Bin table
 * @param max_entries : Size of table
 * 
 * @return L7_RC_t : L7_FAILURE/L7_SUCCESS
 */
L7_RC_t ptin_dhcp82_bindtable_get(ptin_DHCP_bind_entry *table, L7_uint16 *max_entries);

/**
 * Get DHCP Binding table
 *
 * @param table       : Bin table
 * @param max_entries : Size of table
 *
 * @notes   IPv6 compatible
 *
 * @return L7_RC_t : L7_FAILURE/L7_SUCCESS
 */
L7_RC_t ptin_dhcpv4v6_bindtable_get(ptin_DHCPv4v6_bind_entry *table, L7_uint16 *max_entries);

/**
 * Removes an entry from the DHCP binding table
 * 
 * @param ptr : DHCP bind table entry
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp82_bindtable_remove(dhcpSnoopBinding_t *dsBinding);



/**
 * Get global DHCP statistics
 * 
 * @param intIfNum    : interface
 * @param stat_port_g : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_intf_get(ptin_intf_t *ptin_intf, ptin_DHCP_Statistics_t *stat_port_g);

/**
 * Get DHCP statistics of a particular DHCP instance and 
 * interface 
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param intIfNum    : interface
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcp_stat_instanceIntf_get(L7_uint16 UcastEvcId, ptin_intf_t *ptin_intf, ptin_DHCP_Statistics_t *stat_port);

/**
 * Get DHCP statistics of a particular DHCP instance and 
 * client
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param client      : client reference
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_stat_client_get(L7_uint16 UcastEvcId, ptin_client_id_t *client, ptin_DHCP_Statistics_t *stat_client);

/**
 * Clear all DHCP statistics
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_stat_clearAll(void);

/**
 * Clear all statistics of one DHCP instance
 * 
 * @param UcastEvcId : Unicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_stat_instance_clear(L7_uint16 UcastEvcId);

/**
 * Clear interface DHCP statistics
 * 
 * @param intIfNum    : interface 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_stat_intf_clear(ptin_intf_t *ptin_intf);

/**
 * Clear statistics of a particular DHCP instance and interface
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param intIfNum    : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_stat_instanceIntf_clear(L7_uint16 UcastEvcId, ptin_intf_t *ptin_intf);

/**
 * Clear DHCP statistics of a particular DHCP instance and 
 * client
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_stat_client_clear(L7_uint16 UcastEvcId, ptin_client_id_t *client);


/*********************************************************** 
 * Internal functions (for Fastpath usage)
 ***********************************************************/

/**
 * Validate ingress interface for a DHCP packet 
 * 
 * @param intIfNum    : interface
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_dhcp82_intf_validate(L7_uint32 intIfNum);

/**
 * Validate internal vlan in a DHCP Packet 
 * 
 * @param intVlanId   : internal vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_dhcp82_vlan_validate(L7_uint16 intVlanId);

/**
 * Validate interface, internal vlan and innervlan received in a 
 * DHCP packet 
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_dhcp82_intfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlanId /*, L7_uint16 innerVlanId*/);

/**
 * Check if a particular interface of one EVC is trusted
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_dhcp82_is_intfTrusted(L7_uint32 intIfNum, L7_uint16 intVlanId);

/**
 * Get DHCP client data (circuit and remote ids)
 * 
 * @param intIfNum    : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan 
 * @param circuitId   : circuit id (output) 
 * @param remoteId    : remote id (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_stringIds_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_uchar8 *macAddr,
                                       L7_char8 *circuitId, L7_char8 *remoteId);

/**
 * Get DHCP client data (DHCP Options)
 *
 * @param intIfNum    : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan
 * @param isActiveOp82: L7_TRUE if op82 is active for this client
 * @param isActiveOp37: L7_TRUE if op37 is active for this client
 * @param isActiveOp18: L7_TRUE if op18 is active for this client
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_client_options_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_BOOL *isActiveOp82,
                                            L7_BOOL *isActiveOp37, L7_BOOL *isActiveOp18);

/**
 * Get the client index associated to a DHCP client 
 * 
 * @param intIfNum      : interface number
 * @param intVlan       : internal vlan
 * @param client        : Client information parameters
 * @param client_index  : Client index to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_clientIndex_get(L7_uint32 intIfNum, L7_uint16 intVlan, ptin_client_id_t *client, L7_uint *client_index);

/**
 * Get client information from its index. 
 * 
 * @param intVlan       : internal vlan
 * @param client_index  : Client index
 * @param client        : Client information (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_clientData_get(L7_uint16 intVlan, L7_uint client_idx, ptin_client_id_t *client);

/**
 * Update DHCP snooping configuration, when interfaces are 
 * added/removed 
 * 
 * @param evcId     : EVC id 
 * @param ptin_intf : interface 
 * @param enable    : L7_TRUE when interface is added 
 *                    L7_FALSE when interface is removed
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_snooping_trap_interface_update(L7_uint16 evcId, ptin_intf_t *ptin_intf, L7_BOOL enable);

/**
 * Increment DHCP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp_stat_increment_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_dhcp_stat_enum_t field);

#if 0
/**
 * Read DHCP Option 82 entry
 * 
 * @param evc_idx     : EVC id
 * @param ptin_intf   : Interface type and id
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address  
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp82_get(L7_uint evc_idx, ptin_intf_t *ptin_intf, L7_uint16 innerVlanId, L7_uint8 *macAddr,
                               L7_char8 *circuitId, L7_char8 *remoteId);

/**
 * Configure DHCP Option 82
 * 
 * @param evc_idx     : EVC id
 * @param ptin_intf   : Interface type and id
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address  
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp82_config(L7_uint evc_idx, ptin_intf_t *ptin_intf, L7_uint16 innerVlanId, L7_uint8 *macAddr,
                                  L7_char8 *circuitId, L7_char8 *remoteId);

/**
 * Unconfigure DHCP Option 82
 * 
 * @param evc_idx     : EVC id
 * @param ptin_intf   : Interface type and id
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp82_unconfig(L7_uint evc_idx, ptin_intf_t *ptin_intf, L7_uint16 innerVlanId, L7_uint8 *macAddr);

/**
 * Get a circuitId+remoteId entry in database
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan 
 * @param macAddr     : MAC Address 
 * @param circuitId   : circuit id
 * @param remoteId    : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_dhcp82_database_get(L7_uint32 intIfNum, L7_uint16 intVlanId, L7_uint16 innerVlanId, L7_uint8 *macAddr,
                                        L7_char8 *circuitId, L7_char8 *remoteId);
#endif

#endif /* _PTIN_DHCP_H */

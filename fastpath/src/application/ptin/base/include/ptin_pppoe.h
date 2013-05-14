/**
 * ptin_pppoe.h
 *  
 * Implements the PPPOE snooping interface module
 *
 * Created on: 2012/01/06
 * Author: Milton Ruas (milton-r-silva@ext.ptinovacao.pt)
 *  
 * Notes:
 */

#ifndef _PTIN_PPPOE_H
#define _PTIN_PPPOE_H

#include "ptin_include.h"

/*********************************************************** 
 * Defines
 ***********************************************************/

#define CIRCUITID_ACCESSNODEID          0x0001
#define CIRCUITID_CHASSIS               0x0002
#define CIRCUITID_RACK                  0x0004
#define CIRCUITID_FRAME                 0x0008
#define CIRCUITID_ETHERNETPRIORITY      0x0010
#define CIRCUITID_SVID                  0x0020
#define CIRCUITID_ONUID                 0x0040
#define CIRCUITID_SLOT                  0x0080
#define CIRCUITID_PORT                  0x0100
#define CIRCUITID_QVID                  0x0200
#define CIRCUITID_CVID                  0x0400

#define CIRCUITID_ACCESSNODEID_STR      "$accessnodeid"   
#define CIRCUITID_CHASSIS_STR           "$subrack"   
#define CIRCUITID_RACK_STR              "$rack"
#define CIRCUITID_FRAME_STR             "$shelf"  
#define CIRCUITID_ETHERNETPRIORITY_STR  "$ethprty"
#define CIRCUITID_S_VID_STR             "$svid"  
#define CIRCUITID_ONUID_STR             "$onuid"   
#define CIRCUITID_SLOT_STR              "$slot"   
#define CIRCUITID_PORT_STR              "$port"   
#define CIRCUITID_Q_VID_STR             "$qvid"   
#define CIRCUITID_C_VID_STR             "$cvid"   

/***********************************************************
 * Typedefs
 ***********************************************************/

typedef enum  {
  PPPOE_STAT_FIELD_RX_INTERCEPTED=0,
  PPPOE_STAT_FIELD_RX,
  PPPOE_STAT_FIELD_RX_FILTERED,
  PPPOE_STAT_FIELD_TX_FORWARDED,
  PPPOE_STAT_FIELD_TX_FAILED,

  PPPOE_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTIONS,
  PPPOE_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82,
  PPPOE_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION37,
  PPPOE_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION18,
  PPPOE_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTIONS,
  PPPOE_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82,
  PPPOE_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION37,
  PPPOE_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION18,

  PPPOE_STAT_FIELD_RX_CLIENT_PKTS_ON_TRUSTED_INTF,
  PPPOE_STAT_FIELD_RX_CLIENT_PKTS_WITHOPS_ON_UNTRUSTED_INTF,
  PPPOE_STAT_FIELD_RX_SERVER_PKTS_ON_UNTRUSTED_INTF,
  PPPOE_STAT_FIELD_RX_SERVER_PKTS_WITHOUTOPS_ON_TRUSTED_INTF,

  PPPOE_STAT_FIELD_ALL
} ptin_pppoe_stat_enum_t;

typedef struct _ptin_PPPOE_Statistics_t
{
  L7_uint32 pppoe_rx_intercepted;
  L7_uint32 pppoe_rx;                                     //pppoe_rx_valid
  L7_uint32 pppoe_rx_filtered;
  L7_uint32 pppoe_tx_forwarded;
  L7_uint32 pppoe_tx_failed;

  L7_uint32 pppoe_rx_client_requests_without_options;
  L7_uint32 pppoe_tx_client_requests_with_option82;
  L7_uint32 pppoe_tx_client_requests_with_option37;
  L7_uint32 pppoe_tx_client_requests_with_option18;
  L7_uint32 pppoe_rx_server_replies_with_option82;
  L7_uint32 pppoe_rx_server_replies_with_option37;
  L7_uint32 pppoe_rx_server_replies_with_option18;
  L7_uint32 pppoe_tx_server_replies_without_options;

  L7_uint32 pppoe_rx_client_pkts_onTrustedIntf;
  L7_uint32 pppoe_rx_client_pkts_withOps_onUntrustedIntf;
  L7_uint32 pppoe_rx_server_pkts_onUntrustedIntf;
  L7_uint32 pppoe_rx_server_pkts_withoutOps_onTrustedIntf;
} __attribute__ ((packed)) ptin_PPPOE_Statistics_t;

extern L7_BOOL ptin_debug_pppoe_snooping;

/*********************************************************** 
 * External function prototypes
 ***********************************************************/

/**
 * Initialize circuitId+remoteId database
 *  
 * @return none
 */
extern L7_RC_t ptin_pppoe_init(void);

/**
 * Set Global enable for PPPOE packet trapping
 * 
 * @param enable : L7_ENABLE/L7_DISABLE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_enable(L7_BOOL enable);

/**
 * Check if a EVC is being used in an PPPOE instance
 * 
 * @param evcId : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
extern L7_RC_t ptin_pppoe_is_evc_used(L7_uint16 evcId);

/**
 * Creates a PPPOE instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_instance_add(L7_uint16 UcastEvcId);

/**
 * Removes a PPPOE instance
 * 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_instance_remove(L7_uint16 UcastEvcId);

/**
 * Update PPPOE entries, when EVCs are deleted
 * 
 * @param evcId : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_instance_destroy(L7_uint16 evcId);

/**
 * Reconfigure global PPPOE EVC
 *
 * @param evcId         : evc index
 * @param pppoe_flag     : PPPOE flag (not used)
 * @param options       : options
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_evc_reconf(L7_uint16 evcId, L7_uint8 pppoe_flag, L7_uint32 options);

/**
 * Set PPPOE circuit-id global data
 *
 * @param evcId           : evc index
 * @param template_str    : Circuit-id template string
 * @param mask            : Circuit-id mask
 * @param access_node_id  : Access Node ID
 * @param chassis         : Access Node Chassis
 * @param rack            : Access Node Rack
 * @param frame           : Access Node Frame
 * @param slot            : Access Node Chassis/Rack/Frame Slot
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_circuitid_set(L7_uint16 evcId, L7_char8 *template_str, L7_uint32 mask, L7_char8 *access_node_id, L7_uint8 chassis,
                                       L7_uint8 rack, L7_uint8 frame, L7_uint8 ethernet_priority, L7_uint16 s_vid);

/**
 * Get PPPOE circuit-id global data
 *
 * @param evcId           : evc index
 * @param template_str    : Circuit-id template string (output)
 * @param mask            : Circuit-id mask (output)
 * @param access_node_id  : Access Node ID (output)
 * @param chassis         : Access Node Chassis (output)
 * @param rack            : Access Node Rack (output)
 * @param frame           : Access Node Frame (output)
 * @param slot            : Access Node Chassis/Rack/Frame Slot (output)
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_circuitid_get(L7_uint16 evcId, L7_char8 *template_str, L7_uint32 *mask, L7_char8 *access_node_id, L7_uint8 *chassis,
                                       L7_uint8 *rack, L7_uint8 *frame, L7_uint8 *ethernet_priority, L7_uint16 *s_vid);

/**
 * Get PPPOE client data (circuit and remote ids)
 * 
 * @param UcastEvcId        : Unicast evc id
 * @param client            : client identification parameters
 * @param options           : PPPOE options (output)
 * @param onuid             : ONU ID (output)
 * @param sub_slot          : Client Sub-Slot (output)
 * @param port              : Client Port (output)
 * @param q_vid             : Q VID (output)
 * @param ethernet_priority : Ethernet Priority (output)
 * @param remoteId          : remote id (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_client_get(L7_uint16 UcastEvcId, ptin_client_id_t *client, L7_uint16 *options, L7_uint16 *onuid, L7_uint8 *slot,
                                    L7_uint16 *port, L7_uint16 *q_vid, L7_uint16 *c_vid,
                                    L7_char8 *circuitId, L7_char8 *remoteId);

/**
 * Add a new PPPOE client
 * 
 * @param UcastEvcId        : Unicast evc id
 * @param client            : client identification parameters
 * @param options           : PPPOE options
 * @param onuid             : ONU ID
 * @param sub_slot          : Client Sub-Slot
 * @param port              : Client Port
 * @param q_vid             : Q VID
 * @param ethernet_priority : Ethernet Priority
 * @param remoteId          : remote id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_client_add(L7_uint16 UcastEvcId, ptin_client_id_t *client, L7_uint16 options, L7_uint16 onuid, L7_uint8 slot,
                                    L7_uint16 port, L7_uint16 q_vid, L7_uint16 c_vid, L7_char8 *remoteId);

/**
 * Remove a PPPOE client
 * 
 * @param UcastEvcId  : Unicast evc id
 * @param client      : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_client_delete(L7_uint16 UcastEvcId, ptin_client_id_t *client);

#if 0
/**
 * Get PPPOE Binding table
 * 
 * @param table       : Bin table
 * @param max_entries : Size of table
 * 
 * @return L7_RC_t : L7_FAILURE/L7_SUCCESS
 */
L7_RC_t ptin_pppoe82_bindtable_get(ptin_PPPOE_bind_entry *table, L7_uint16 *max_entries);

/**
 * Get PPPOE Binding table
 *
 * @param table       : Bin table
 * @param max_entries : Size of table
 *
 * @notes   IPv6 compatible
 *
 * @return L7_RC_t : L7_FAILURE/L7_SUCCESS
 */
L7_RC_t ptin_pppoev4v6_bindtable_get(ptin_PPPOEv4v6_bind_entry *table, L7_uint16 *max_entries);

/**
 * Removes an entry from the PPPOE binding table
 * 
 * @param ptr : PPPOE bind table entry
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe82_bindtable_remove(pppoeSnoopBinding_t *dsBinding);
#endif


/**
 * Get global PPPOE statistics
 * 
 * @param intIfNum    : interface
 * @param stat_port_g : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_intf_get(ptin_intf_t *ptin_intf, ptin_PPPOE_Statistics_t *stat_port_g);

/**
 * Get PPPOE statistics of a particular PPPOE instance and 
 * interface 
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param intIfNum    : interface
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_stat_instanceIntf_get(L7_uint16 UcastEvcId, ptin_intf_t *ptin_intf, ptin_PPPOE_Statistics_t *stat_port);

/**
 * Get PPPOE statistics of a particular PPPOE instance and 
 * client
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param client      : client reference
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_stat_client_get(L7_uint16 UcastEvcId, ptin_client_id_t *client, ptin_PPPOE_Statistics_t *stat_client);

/**
 * Clear all PPPOE statistics
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_stat_clearAll(void);

/**
 * Clear all statistics of one PPPOE instance
 * 
 * @param UcastEvcId : Unicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_stat_instance_clear(L7_uint16 UcastEvcId);

/**
 * Clear interface PPPOE statistics
 * 
 * @param intIfNum    : interface 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_stat_intf_clear(ptin_intf_t *ptin_intf);

/**
 * Clear statistics of a particular PPPOE instance and interface
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param intIfNum    : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_stat_instanceIntf_clear(L7_uint16 UcastEvcId, ptin_intf_t *ptin_intf);

/**
 * Clear PPPOE statistics of a particular PPPOE instance and 
 * client
 * 
 * @param UcastEvcId  : Unicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_stat_client_clear(L7_uint16 UcastEvcId, ptin_client_id_t *client);


/*********************************************************** 
 * Internal functions (for Fastpath usage)
 ***********************************************************/

/**
 * Get the root vlan associated to the internal vlan
 * 
 * @param intVlan  : internal vlan
 * @param rootVlan : root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoe_rootVlan_get(L7_uint16 intVlan, L7_uint16 *rootVlan);

/**
 * Validate ingress interface for a PPPOE packet 
 * 
 * @param intIfNum    : interface
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_intf_validate(L7_uint32 intIfNum);

/**
 * Validate internal vlan in a PPPOE Packet 
 * 
 * @param intVlanId   : internal vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_vlan_validate(L7_uint16 intVlanId);

/**
 * Validate interface, internal vlan and innervlan received in a 
 * PPPOE packet 
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * @param innerVlanId : client vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_intfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlanId /*, L7_uint16 innerVlanId*/);

/**
 * Check if a particular interface of one EVC is trusted
 * 
 * @param intIfNum    : interface
 * @param intVlanId   : internal vlan
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
L7_BOOL ptin_pppoe_is_intfTrusted(L7_uint32 intIfNum, L7_uint16 intVlanId);

/**
 * Get PPPOE client data (circuit and remote ids)
 * 
 * @param intIfNum    : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan 
 * @param circuitId   : circuit id (output) 
 * @param remoteId    : remote id (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_stringIds_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_uchar8 *macAddr,
                                       L7_char8 *circuitId, L7_char8 *remoteId);

/**
 * Get PPPOE EVC ethPrty
 * 
 * @param intIfNum    : FP interface
 * @param intVlan     : internal vlan
 * @param innerVlan   : inner/client vlan 
 * @param ethPrty     : ethernet priority (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_ethPrty_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_uint8 *ethPrty);

/**
 * Get PPPOE client data (PPPOE Options)
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
extern L7_RC_t ptin_pppoe_client_options_get(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 innerVlan, L7_BOOL *isActiveOp82,
                                            L7_BOOL *isActiveOp37, L7_BOOL *isActiveOp18);

/**
 * Get the client index associated to a PPPOE client 
 * 
 * @param intIfNum      : interface number
 * @param intVlan       : internal vlan
 * @param client        : Client information parameters
 * @param client_index  : Client index to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_clientIndex_get(L7_uint32 intIfNum, L7_uint16 intVlan, ptin_client_id_t *client, L7_uint *client_index);

/**
 * Get client information from its index. 
 * 
 * @param intVlan       : internal vlan
 * @param client_index  : Client index
 * @param client        : Client information (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_clientData_get(L7_uint16 intVlan, L7_uint client_idx, ptin_client_id_t *client);

/**
 * Update PPPOE snooping configuration, when interfaces are 
 * added/removed 
 * 
 * @param evcId     : EVC id 
 * @param ptin_intf : interface 
 * @param enable    : L7_TRUE when interface is added 
 *                    L7_FALSE when interface is removed
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_snooping_trap_interface_update(L7_uint16 evcId, ptin_intf_t *ptin_intf, L7_BOOL enable);

/**
 * Increment PPPOE statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_pppoe_stat_increment_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_pppoe_stat_enum_t field);

#endif /* _PTIN_PPPOE_H */

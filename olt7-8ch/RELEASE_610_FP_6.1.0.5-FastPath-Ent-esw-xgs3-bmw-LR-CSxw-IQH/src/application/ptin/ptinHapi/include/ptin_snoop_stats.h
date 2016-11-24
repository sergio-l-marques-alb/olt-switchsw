#ifndef _PTIN_SNOOP_STATS__H
#define _PTIN_SNOOP_STATS__H

#include "snooping_exports.h"
#include "ptin_snoop_stat_api.h"
#include "dtlinclude.h"

#include "ptin/globaldefs.h"

#define IGMP_MAX_NUMBER_OF_FLOWS      8
#define DHCP_MAX_NUMBER_OF_FLOWS      8

extern L7_RC_t ptin_igmp_stat_get(L7_uint16 evcId, L7_uint16 cvlan, L7_uint16 port, L7_IGMP_Statistics_t *stat_client, L7_IGMP_Statistics_t *stat_port, L7_IGMP_Statistics_t *stat_port_g);
extern L7_RC_t ptin_igmp_stat_port_clear(L7_uint16 evcId, L7_uint16 port);
extern L7_RC_t ptin_igmp_stat_client_clear(L7_uint16 evcId, L7_uint16 cvlan);

extern void ptin_igmp_evcClient_init(void);
extern L7_RC_t ptin_igmp_evcClient_clear(L7_uint16 evcId);
extern L7_RC_t ptin_igmp_evcClient_isEmpty(L7_uint16 evcId, L7_BOOL *state);
extern L7_RC_t ptin_igmp_evcClient_isFull(L7_uint16 evcId, L7_BOOL *state);
extern L7_RC_t ptin_igmp_evcClient_add(L7_uint16 evcId, L7_uint16 cvlan);
extern L7_RC_t ptin_igmp_evcClient_remove(L7_uint16 evcId, L7_uint16 cvlan);
extern L7_RC_t ptin_igmp_evcClient_add_fromIntfVlan(L7_uint32 intf, L7_uint16 vlanId, L7_uint16 cvlan);
extern L7_RC_t ptin_igmp_evcClient_remove_fromIntfVlan(L7_uint32 intf, L7_uint16 vlanId, L7_uint16 cvlan);

extern void ptin_igmp_flow_init(void);
extern L7_RC_t ptin_igmp_flow_isEmpty(L7_BOOL *state);
extern L7_RC_t ptin_igmp_flow_isFull(L7_BOOL *state);
extern L7_RC_t ptin_igmp_flow_add(L7_uint16 evcId);
extern L7_RC_t ptin_igmp_flow_remove(L7_uint16 evcId);

#ifdef IGMP_WHITELIST_FEATURE
/**
 * Add a new entry in white list. 
 * 
 * @param channel_group   : Group channel
 * @param channel_grpMask : Number of masked bits
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t igmp_wlist_channel_add(L7_uint16 evc_idx, L7_inet_addr_t *channel_group , L7_uint16 channel_grpMask);

/**
 * Remove an entry from the white list. 
 * 
 * @param channel_group   : Group channel
 * @param channel_grpMask : Number of masked bits
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t igmp_wlist_channel_remove(L7_uint16 evc_idx, L7_inet_addr_t *channel_group, L7_uint16 channel_grpMask);
#endif

extern L7_RC_t ptin_dhcp_stat_get(L7_uint16 evcId, L7_uint16 cvlan, L7_uint16 port, L7_DHCP_Statistics_t *stat_client, L7_DHCP_Statistics_t *stat_port, L7_DHCP_Statistics_t *stat_port_g);
extern L7_RC_t ptin_dhcp_stat_port_clear(L7_uint16 evcId, L7_uint16 port);
extern L7_RC_t ptin_dhcp_stat_client_clear(L7_uint16 evcId, L7_uint16 cvlan);

extern void ptin_dhcp_evcClient_init(void);
extern L7_RC_t ptin_dhcp_evcClient_clear(L7_uint16 evcId);
extern L7_RC_t ptin_dhcp_evcClient_isEmpty(L7_uint16 evcId, L7_BOOL *state);
extern L7_RC_t ptin_dhcp_evcClient_isFull(L7_uint16 evcId, L7_BOOL *state);
extern L7_RC_t ptin_dhcp_evcClient_add(L7_uint16 evcId, L7_uint16 cvlan);
extern L7_RC_t ptin_dhcp_evcClient_remove(L7_uint16 evcId, L7_uint16 cvlan);
extern L7_RC_t ptin_dhcp_evcClient_add_fromIntfVlan(L7_uint32 intf, L7_uint16 vlanId, L7_uint16 cvlan);
extern L7_RC_t ptin_dhcp_evcClient_remove_fromIntfVlan(L7_uint32 intf, L7_uint16 vlanId, L7_uint16 cvlan);

extern void ptin_dhcp_flow_init(void);
extern L7_RC_t ptin_dhcp_flow_isEmpty(L7_BOOL *state);
extern L7_RC_t ptin_dhcp_flow_isFull(L7_BOOL *state);
extern L7_RC_t ptin_dhcp_flow_add(L7_uint16 evcId);
extern L7_RC_t ptin_dhcp_flow_remove(L7_uint16 evcId);

#endif // _PTIN_SNOOP_STATS__H


#ifndef _PTIN_SNOOP_STAT_API__H
#define _PTIN_SNOOP_STAT_API__H

#include "datatypes.h"
#include "dtlinclude.h"
#include "ptin/globaldefs.h"

// The values below must be in the same order as in L7_IGMP_Statistics_t structure
typedef enum  {
  SNOOP_STAT_FIELD_ACTIVE_GROUPS=0,
  SNOOP_STAT_FIELD_ACTIVE_CLIENTS,
  SNOOP_STAT_FIELD_IGMP_SENT,
  SNOOP_STAT_FIELD_IGMP_TX_FAILED,
  SNOOP_STAT_FIELD_IGMP_INTERCEPTED,
  SNOOP_STAT_FIELD_IGMP_DROPPED,
  SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID,
  SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID,
  SNOOP_STAT_FIELD_JOINS_SENT,
  SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS,
  SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED,
  SNOOP_STAT_FIELD_LEAVES_SENT,
  SNOOP_STAT_FIELD_LEAVES_RECEIVED,
  SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_V3,
  SNOOP_STAT_FIELD_GENERAL_QUERIES_SENT,
  SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED,
  SNOOP_STAT_FIELD_SPECIFIC_QUERIES_SENT,
  SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED,
  SNOOP_STAT_FIELD_ALL                        // This must be the last element
} snoop_stat_field_t;

typedef enum  {
  DHCP_STAT_FIELD_RX_INTERCEPTED=0,
  DHCP_STAT_FIELD_RX,
  DHCP_STAT_FIELD_RX_FILTERED,
  DHCP_STAT_FIELD_TX_FORWARDED,
  DHCP_STAT_FIELD_TX_FAILED,
  DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITH_OPTION82,
  DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82,
  DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82,
  DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITH_OPTION82,
  DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTION82,
  DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITHOUT_OPTION82,
  DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITHOUT_OPTION82,
  DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTION82,
  DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOUTOP82_ON_TRUSTED_INTF,
  DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOP82_ON_UNTRUSTED_INTF,
  DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOP82_ON_UNTRUSTED_INTF,
  DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOUTOP82_ON_TRUSTED_INTF,
  DHCP_STAT_FIELD_ALL
} dhcp_stat_field_t;

#ifdef IGMP_WHITELIST_FEATURE
/**
 * Check if a groupAddress exists in White List.
 * 
 * @param channel_group   : Group address
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_BOOL igmp_wlist_channel_exists(L7_uint32 intIfNum, L7_uint16 vlanId, L7_inet_addr_t *channel_group);
#endif

extern void ptin_igmp_stat_init(void);
extern L7_BOOL ptin_igmp_stat_client_exists(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan);
extern L7_RC_t ptin_igmp_stat_clear_field(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, snoop_stat_field_t field);
extern L7_RC_t ptin_igmp_stat_increment_field(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, snoop_stat_field_t field);

extern void ptin_dhcp_stat_init(void);
extern L7_RC_t ptin_dhcp_stat_clear_field(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, dhcp_stat_field_t field);
extern L7_RC_t ptin_dhcp_stat_increment_field(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, dhcp_stat_field_t field);

#endif // _PTIN_SNOOP_STAT_API__H


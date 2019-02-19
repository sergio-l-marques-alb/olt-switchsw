#include "ptin_snoop_stats.h"
#include "ptin_flow.h"
#include "usmdb_util_api.h"
#include "ptin/logger.h"

// Tables used to manage client indexes associated to an inner vlan
typedef struct _st_igmpflow_index_table {
  L7_int index_free_ptr;
  L7_int index_free[IGMP_MAX_NUMBER_OF_FLOWS];
  L7_int flow_index[FLOWS_MAX];
} st_igmpflow_index_table;
st_igmpflow_index_table igmpflow_table;

typedef struct _st_dhcpflow_index_table {
  L7_int index_free_ptr;
  L7_int index_free[DHCP_MAX_NUMBER_OF_FLOWS];
  L7_int flow_index[FLOWS_MAX];
} st_dhcpflow_index_table;
st_dhcpflow_index_table dhcpflow_table;

// Tables used to manage client indexes associated to an inner vlan
typedef struct _st_client_index_table {
  L7_int index_free_ptr;
  L7_int index_free[SNOOP_MAX_NUMBER_OF_CLIENTS];
  L7_int client_index[4096];
} st_client_index_table;
st_client_index_table igmp_client_table[IGMP_MAX_NUMBER_OF_FLOWS];
st_client_index_table dhcp_client_table[DHCP_MAX_NUMBER_OF_FLOWS];

static L7_IGMP_Statistics_t L7_IGMP_Statistics_Intf_Global[L7_SYSTEM_N_INTERF];
static L7_IGMP_Statistics_t L7_IGMP_Statistics_Intf[IGMP_MAX_NUMBER_OF_FLOWS][L7_SYSTEM_N_INTERF];
static L7_IGMP_Statistics_t L7_IGMP_Statistics_Clients[IGMP_MAX_NUMBER_OF_FLOWS][SNOOP_MAX_NUMBER_OF_CLIENTS];

static L7_DHCP_Statistics_t L7_DHCP_Statistics_Intf_Global[L7_SYSTEM_N_INTERF];
static L7_DHCP_Statistics_t L7_DHCP_Statistics_Intf[DHCP_MAX_NUMBER_OF_FLOWS][L7_SYSTEM_N_INTERF];
static L7_DHCP_Statistics_t L7_DHCP_Statistics_Clients[DHCP_MAX_NUMBER_OF_FLOWS][SNOOP_MAX_NUMBER_OF_CLIENTS];

static L7_RC_t ptin_igmp_stat_get_indexes(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index, L7_int *port_index);
static L7_RC_t ptin_igmp_client_getIndex(L7_uint16 port, L7_uint16 vlan, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index);
static L7_RC_t ptin_igmp_evcClient_getIndex(L7_uint16 evcId, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index);
static L7_RC_t ptin_igmp_flow_getIndex(L7_uint16 evcId, L7_int *flow_index);

static L7_RC_t ptin_dhcp_stat_get_indexes(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index, L7_int *port_index);
static L7_RC_t ptin_dhcp_client_getIndex(L7_uint16 port, L7_uint16 vlan, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index);
static L7_RC_t ptin_dhcp_evcClient_getIndex(L7_uint16 evcId, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index);
static L7_RC_t ptin_dhcp_flow_getIndex(L7_uint16 evcId, L7_int *flow_index);

#ifdef IGMP_WHITELIST_FEATURE

#define IGMP_WHITELIST_CHANNELS_MAX   4096

typedef struct
{
  L7_uint16       evc_id;
  L7_inet_addr_t  channel_group;
} ptinIgmpWListDataKey_t;

typedef struct
{
  ptinIgmpWListDataKey_t     igmpWListDataKey;
  void *next;
} ptinIgmpWListInfoData_t;

typedef struct {
    L7_uint16 number_of_entries;

    avlTree_t               igmpWListAvlTree;
    avlTreeTables_t         *igmpWListTreeHeap;
    ptinIgmpWListInfoData_t *igmpWListDataHeap;
} ptinIgmpWListAvlTree_t;

/* List of all IGMP associations */
ptinIgmpWListAvlTree_t igmpWListDB;

static L7_RC_t igmp_wlist_channelIP_prepare( L7_inet_addr_t *channel_in, L7_uint16 channel_mask,
                                             L7_inet_addr_t *channel_out, L7_uint32 *number_of_channels);
static L7_RC_t igmp_wlist_avlTree_insert( ptinIgmpWListInfoData_t *node );
static L7_RC_t igmp_wlist_avlTree_remove( ptinIgmpWListDataKey_t *avl_key );
#if 0
static L7_RC_t igmp_wlist_avlTree_purge ( void );
#endif
#endif

L7_int igmp_stat_debug = 0;
L7_int dhcp_stat_debug = 0;

void igmp_stat_debugger(L7_int active)
{
  igmp_stat_debug = (active!=0);
}

void dhcp_stat_debugger(L7_int active)
{
  dhcp_stat_debug = (active!=0);
}


void ptin_igmp_stat_init(void)
{
  // Clear management database
  ptin_igmp_flow_init();

  // Clear statistics database
  memset(L7_IGMP_Statistics_Intf_Global,0x00,sizeof(L7_IGMP_Statistics_t)*L7_SYSTEM_N_INTERF);
  memset(L7_IGMP_Statistics_Intf,0x00,sizeof(L7_IGMP_Statistics_t)*IGMP_MAX_NUMBER_OF_FLOWS*L7_SYSTEM_N_INTERF);
  memset(L7_IGMP_Statistics_Clients,0x00,sizeof(L7_IGMP_Statistics_t)*IGMP_MAX_NUMBER_OF_FLOWS*SNOOP_MAX_NUMBER_OF_CLIENTS);

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > ptin_snoop_stat_init executed\n", __FUNCTION__, __LINE__);
}


L7_BOOL ptin_igmp_stat_client_exists(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan)
{
  L7_int client_index = -1;

  // Get client index
  ptin_igmp_stat_get_indexes(intf,vlan,cvlan, L7_NULLPTR, &client_index, L7_NULLPTR);

  // Check if this client already exists
  if (client_index>=0 && client_index<SNOOP_MAX_NUMBER_OF_CLIENTS)
    return L7_TRUE;

  return L7_FALSE;
}


L7_RC_t ptin_igmp_stat_clear_field(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, snoop_stat_field_t field)
{
  L7_int flow_index=-1, client_index=-1, port_index=-1;
  L7_IGMP_Statistics_t *stat_port_g=L7_NULLPTR;
  L7_IGMP_Statistics_t *stat_port=L7_NULLPTR;
  L7_IGMP_Statistics_t *stat_client=L7_NULLPTR;

  //return L7_SUCCESS;

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > Going to clear field: intf=%u,vlan=%u,cvlan=%u, field=%u\n", __FUNCTION__, __LINE__,intf,vlan,cvlan,field);

  ptin_igmp_stat_get_indexes(intf,vlan,cvlan, &flow_index, &client_index, &port_index);
//if (ptin_igmp_stat_get_indexes(intf,vlan,cvlan, &flow_index, &client_index, &port_index)!=L7_SUCCESS &&
//    flow_index==-1)  {
//  if (igmp_stat_debug)
//    printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_igmp_stat_increment_field (intf=%u,vlan=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,intf,vlan,cvlan);
//  return L7_FAILURE;
//}

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > intf=%u,vlan=%u,cvlan=%u => flow_index=%d,client_index=%d,port_index=%d\n", __FUNCTION__, __LINE__,
           intf,vlan,cvlan,flow_index,client_index,port_index);

  // Global port statistics
  if (port_index>=0 && port_index<L7_SYSTEM_N_INTERF)
    stat_port_g = &L7_IGMP_Statistics_Intf_Global[port_index];
  else
    stat_port_g = L7_NULLPTR;

  // EVC port statistics
  if (flow_index>=0 && flow_index<IGMP_MAX_NUMBER_OF_FLOWS &&
      port_index>=0 && port_index<L7_SYSTEM_N_INTERF)
    stat_port   = &L7_IGMP_Statistics_Intf[flow_index][port_index];
  else
    stat_port   = L7_NULLPTR;

  // EVC client statistics
  if (flow_index>=0 && flow_index<IGMP_MAX_NUMBER_OF_FLOWS &&
      client_index>=0 && client_index<SNOOP_MAX_NUMBER_OF_CLIENTS)
    stat_client = &L7_IGMP_Statistics_Clients[flow_index][client_index];
  else
    stat_client = L7_NULLPTR;

  if (stat_port_g==L7_NULLPTR && stat_port==L7_NULLPTR && stat_client==L7_NULLPTR) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > No reference found to one port or one client!\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  switch (field) {
  case SNOOP_STAT_FIELD_ACTIVE_GROUPS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->active_groups = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->active_groups = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->active_groups = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_ACTIVE_GROUPS field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_ACTIVE_CLIENTS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->active_clients = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->active_clients = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->active_clients = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_ACTIVE_CLIENTS field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_sent = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_sent = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_sent = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_IGMP_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_TX_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_tx_failed = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_tx_failed = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_tx_failed = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_IGMP_TX_FAILED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_INTERCEPTED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_intercepted = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_intercepted = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_intercepted = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_IGMP_INTERCEPTED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_dropped = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_dropped = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_dropped = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_IGMP_DROPPED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_received_valid = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_received_valid = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_received_valid = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_received_invalid = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_received_invalid = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_received_invalid = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_JOINS_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_sent = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_sent  = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_sent = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_JOINS_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_success = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_success = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_success = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_failed = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_failed = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_failed = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_LEAVES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_sent = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_sent = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_sent = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_LEAVES_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_received = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_received = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_received = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_LEAVES_RECEIVED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_V3:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->membership_report_v3 = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->membership_report_v3 = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->membership_report_v3 = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_V3 field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERIES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->general_queries_sent = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->general_queries_sent = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->general_queries_sent = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_GENERAL_QUERIES_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->general_queries_received = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->general_queries_received = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->general_queries_received = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_SPECIFIC_QUERIES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->specific_queries_sent = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->specific_queries_sent = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->specific_queries_sent = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_SPECIFIC_QUERIES_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->specific_queries_received = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port->specific_queries_received = 0;
    if (stat_client!=L7_NULLPTR)  stat_client->specific_queries_received = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_ALL:
    if (stat_port_g!=L7_NULLPTR) {
      stat_port_g->active_groups = 0;
      stat_port_g->active_clients = 0;
      stat_port_g->igmp_sent = 0;
      stat_port_g->igmp_tx_failed = 0;
      stat_port_g->igmp_intercepted = 0;
      stat_port_g->igmp_dropped = 0;
      stat_port_g->igmp_received_valid = 0;
      stat_port_g->igmp_received_invalid = 0;
      stat_port_g->joins_sent  = 0;
      stat_port_g->joins_received_success = 0;
      stat_port_g->joins_received_failed = 0;
      stat_port_g->leaves_sent = 0;
      stat_port_g->leaves_received = 0;
      stat_port_g->membership_report_v3 = 0;
      stat_port_g->general_queries_sent = 0;
      stat_port_g->general_queries_received = 0;
      stat_port_g->specific_queries_sent = 0;
      stat_port_g->specific_queries_received = 0;
    }
    if (stat_port!=L7_NULLPTR) {
      stat_port->active_groups = 0;
      stat_port->active_clients = 0;
      stat_port->igmp_sent = 0;
      stat_port->igmp_tx_failed = 0;
      stat_port->igmp_intercepted = 0;
      stat_port->igmp_dropped = 0;
      stat_port->igmp_received_valid = 0;
      stat_port->igmp_received_invalid = 0;
      stat_port->joins_sent  = 0;
      stat_port->joins_received_success = 0;
      stat_port->joins_received_failed = 0;
      stat_port->leaves_sent = 0;
      stat_port->leaves_received = 0;
      stat_port->membership_report_v3 = 0;
      stat_port->general_queries_sent = 0;
      stat_port->general_queries_received = 0;
      stat_port->specific_queries_sent = 0;
      stat_port->specific_queries_received = 0;
    }
    if (stat_client!=L7_NULLPTR) {
      stat_client->active_groups = 0;
      stat_client->active_clients = 0;
      stat_client->igmp_sent = 0;
      stat_client->igmp_tx_failed = 0;
      stat_client->igmp_intercepted = 0;
      stat_client->igmp_dropped = 0;
      stat_client->igmp_received_valid = 0;
      stat_client->igmp_received_invalid = 0;
      stat_client->joins_sent = 0;
      stat_client->joins_received_success = 0;
      stat_client->joins_received_failed = 0;
      stat_client->leaves_sent = 0;
      stat_client->leaves_received = 0;
      stat_client->membership_report_v3 = 0;
      stat_client->general_queries_sent = 0;
      stat_client->general_queries_received = 0;
      stat_client->specific_queries_sent = 0;
      stat_client->specific_queries_received = 0;
    }
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Cleared SNOOP_STAT_FIELD_ALL field\n", __FUNCTION__, __LINE__);
    break;

  default:
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid field reference (field=%u)\n", __FUNCTION__, __LINE__,field);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_igmp_stat_increment_field(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, snoop_stat_field_t field)
{
  L7_int flow_index=-1, client_index=-1, port_index=-1;
  L7_IGMP_Statistics_t *stat_port_g=L7_NULLPTR;
  L7_IGMP_Statistics_t *stat_port=L7_NULLPTR;
  L7_IGMP_Statistics_t *stat_client=L7_NULLPTR;

  //return L7_SUCCESS;

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > Going to increment field: intf=%u,vlan=%u,cvlan=%u, field=%u\n", __FUNCTION__, __LINE__,intf,vlan,cvlan,field);

  ptin_igmp_stat_get_indexes(intf,vlan,cvlan, &flow_index, &client_index, &port_index);
//if (ptin_igmp_stat_get_indexes(intf,vlan,cvlan, &flow_index, &client_index, &port_index)!=L7_SUCCESS &&
//    flow_index==-1)  {
//  if (igmp_stat_debug)
//    printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_igmp_stat_increment_field (intf=%u,vlan=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,intf,vlan,cvlan);
//  return L7_FAILURE;
//}

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > intf=%u,vlan=%u,cvlan=%u => flow_index=%d,client_index=%d,port_index=%d\n", __FUNCTION__, __LINE__,
           intf,vlan,cvlan,flow_index,client_index,port_index);

  // Global port statistics
  if (port_index>=0 && port_index<L7_SYSTEM_N_INTERF)
    stat_port_g = &L7_IGMP_Statistics_Intf_Global[port_index];
  else
    stat_port_g = L7_NULLPTR;

  // EVC port statistics
  if (flow_index>=0 && flow_index<IGMP_MAX_NUMBER_OF_FLOWS &&
      port_index>=0 && port_index<L7_SYSTEM_N_INTERF)
    stat_port   = &L7_IGMP_Statistics_Intf[flow_index][port_index];
  else
    stat_port   = L7_NULLPTR;

  // EVC client statistics
  if (flow_index>=0 && flow_index<IGMP_MAX_NUMBER_OF_FLOWS &&
      client_index>=0 && client_index<SNOOP_MAX_NUMBER_OF_CLIENTS)
    stat_client = &L7_IGMP_Statistics_Clients[flow_index][client_index];
  else
    stat_client = L7_NULLPTR;

  if (stat_port_g==L7_NULLPTR && stat_port==L7_NULLPTR && stat_client==L7_NULLPTR) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > No reference found to one port or one client!\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  switch (field) {
  case SNOOP_STAT_FIELD_ACTIVE_GROUPS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->active_groups++;
    if (stat_port  !=L7_NULLPTR)  stat_port->active_groups++;
    if (stat_client!=L7_NULLPTR)  stat_client->active_groups++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_ACTIVE_GROUPS field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_ACTIVE_CLIENTS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->active_clients++;
    if (stat_port  !=L7_NULLPTR)  stat_port->active_clients++;
    if (stat_client!=L7_NULLPTR)  stat_client->active_clients++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_ACTIVE_CLIENTS field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_sent++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_IGMP_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_TX_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_tx_failed++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_tx_failed++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_tx_failed++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_IGMP_TX_FAILED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_INTERCEPTED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_intercepted++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_intercepted++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_intercepted++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_IGMP_INTERCEPTED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_DROPPED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_dropped++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_dropped++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_dropped++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_IGMP_DROPPED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_received_valid++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_received_valid++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_received_valid++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->igmp_received_invalid++;
    if (stat_port  !=L7_NULLPTR)  stat_port->igmp_received_invalid++;
    if (stat_client!=L7_NULLPTR)  stat_client->igmp_received_invalid++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_JOINS_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_sent++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_JOINS_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_success++;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_success++;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_success++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->joins_received_failed++;
    if (stat_port  !=L7_NULLPTR)  stat_port->joins_received_failed++;
    if (stat_client!=L7_NULLPTR)  stat_client->joins_received_failed++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_LEAVES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_sent++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_LEAVES_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_LEAVES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->leaves_received++;
    if (stat_port  !=L7_NULLPTR)  stat_port->leaves_received++;
    if (stat_client!=L7_NULLPTR)  stat_client->leaves_received++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_LEAVES_RECEIVED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_V3:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->membership_report_v3++;
    if (stat_port  !=L7_NULLPTR)  stat_port->membership_report_v3++;
    if (stat_client!=L7_NULLPTR)  stat_client->membership_report_v3++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_V3 field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERIES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->general_queries_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->general_queries_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->general_queries_sent++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_GENERAL_QUERIES_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->general_queries_received++;
    if (stat_port  !=L7_NULLPTR)  stat_port->general_queries_received++;
    if (stat_client!=L7_NULLPTR)  stat_client->general_queries_received++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_SPECIFIC_QUERIES_SENT:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->specific_queries_sent++;
    if (stat_port  !=L7_NULLPTR)  stat_port->specific_queries_sent++;
    if (stat_client!=L7_NULLPTR)  stat_client->specific_queries_sent++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_SPECIFIC_QUERIES_SENT field\n", __FUNCTION__, __LINE__);
    break;

  case SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g->specific_queries_received++;
    if (stat_port  !=L7_NULLPTR)  stat_port->specific_queries_received++;
    if (stat_client!=L7_NULLPTR)  stat_client->specific_queries_received++;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > Incremented SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED field\n", __FUNCTION__, __LINE__);
    break;

  default:
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid field reference (field=%u)\n", __FUNCTION__, __LINE__,field);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


L7_RC_t ptin_igmp_stat_get(L7_uint16 evcId, L7_uint16 cvlan, L7_uint16 port, L7_IGMP_Statistics_t *stat_client, L7_IGMP_Statistics_t *stat_port, L7_IGMP_Statistics_t *stat_port_g)
{
  L7_int flow_index, client_index, port_index;

  if (stat_port_g!=L7_NULLPTR) {
    if (port>=L7_SYSTEM_N_INTERF) {
      if (igmp_stat_debug)
        printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid port reference (port=%u)\n", __FUNCTION__, __LINE__,port);
      return L7_FAILURE;
    }
    port_index = port;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > port=%u => port_index=%u\n", __FUNCTION__, __LINE__,port,port_index);
    memcpy(stat_port_g,&L7_IGMP_Statistics_Intf_Global[port_index],sizeof(L7_IGMP_Statistics_t));
  }

  // evcId==0, corresponds to read general port statistics
//if (evcId==0) {
//  if (igmp_stat_debug)
//    printf("{igmp_stat_debug} [ info  ] %s(%d) > Only global port statistics will be read\n", __FUNCTION__, __LINE__);
//  return L7_SUCCESS;
//}

  // Validate evcId
  if (evcId>=FLOWS_MAX) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  if (stat_port!=L7_NULLPTR) {
    if (port>=L7_SYSTEM_N_INTERF) {
      if (igmp_stat_debug)
        printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid port reference (port=%u)\n", __FUNCTION__, __LINE__,port);
      return L7_FAILURE;
    }
    if (ptin_igmp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
      if (igmp_stat_debug)
        printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_igmp_flow_getIndex (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
      return L7_FAILURE;
    }
    port_index = port;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > port=%u => port_index=%u; evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,port,port_index,evcId,flow_index);
    memcpy(stat_port,&L7_IGMP_Statistics_Intf[flow_index][port_index],sizeof(L7_IGMP_Statistics_t));
  }

  if (stat_client!=L7_NULLPTR) {
    if (cvlan>4095) {
      if (igmp_stat_debug)
        printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid cvlan value (cvlan=%u)\n", __FUNCTION__, __LINE__,cvlan);
      return L7_FAILURE;
    }
    if (ptin_igmp_evcClient_getIndex(evcId,cvlan,&flow_index,&client_index)!=L7_SUCCESS) {
      if (igmp_stat_debug)
        printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_igmp_evcClient_getIndex (evcId=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
      return L7_FAILURE;
    }
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u,cvlan=%u => flow_index=%d,client_index=%d\n", __FUNCTION__, __LINE__,evcId,cvlan,flow_index,client_index);
    memcpy(stat_client,&L7_IGMP_Statistics_Clients[flow_index][client_index],sizeof(L7_IGMP_Statistics_t));
  }

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > ptin_snoop_stat_get executed\n", __FUNCTION__, __LINE__);

  return L7_SUCCESS;
}


L7_RC_t ptin_igmp_stat_port_clear(L7_uint16 evcId, L7_uint16 port)
{
  L7_int flow_index=-1, port_index=-1;

  //return L7_SUCCESS;

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > Going to clear structure: evcId=%u,port=%u\n", __FUNCTION__, __LINE__,evcId,port);

  // Check if all port statistics will be cleared
  if (evcId==(L7_uint16)-1) {
    if (port==(L7_uint16)-1)
      memset(L7_IGMP_Statistics_Intf_Global,0x00,sizeof(L7_IGMP_Statistics_t)*L7_SYSTEM_N_INTERF);
    memset(L7_IGMP_Statistics_Intf,0x00,sizeof(L7_IGMP_Statistics_t)*IGMP_MAX_NUMBER_OF_FLOWS*L7_SYSTEM_N_INTERF);
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > All port statistics of all evcs cleared\n", __FUNCTION__, __LINE__);
    // Everything is done
    return L7_SUCCESS;
  }

  // Get igmpflow and port indexes
  if (evcId<FLOWS_MAX) {
    if (ptin_igmp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
      if (igmp_stat_debug)
        printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_igmp_flow_getIndex (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
      return L7_FAILURE;
    }
  }
  port_index = (port<L7_SYSTEM_N_INTERF) ? port : -1;

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u,port=%u => flow_index=%d,port_index=%u\n", __FUNCTION__, __LINE__,
           evcId,port,flow_index,port_index);

  // Check if all port statistics (of one EVC) should be cleared
  if (port==(L7_uint16)-1) {
    if (flow_index!=-1)
      memset(L7_IGMP_Statistics_Intf[flow_index],0x00,sizeof(L7_IGMP_Statistics_t)*L7_SYSTEM_N_INTERF);
    else
      memset(L7_IGMP_Statistics_Intf_Global,0x00,sizeof(L7_IGMP_Statistics_t)*L7_SYSTEM_N_INTERF);

    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > All port statistics of evc=%u cleared\n", __FUNCTION__, __LINE__,evcId);
  } else if (port_index>=0 && port_index<L7_SYSTEM_N_INTERF) {
    if (flow_index!=-1)
      memset(&L7_IGMP_Statistics_Intf[flow_index][port_index],0x00,sizeof(L7_IGMP_Statistics_t));
    else
      memset(&L7_IGMP_Statistics_Intf_Global[port_index],0x00,sizeof(L7_IGMP_Statistics_t));

    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Port=%u statistics of evc=%u cleared\n", __FUNCTION__, __LINE__,port,evcId);
  } else {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid Port Index: (flow_index=%d,port_index=%d)\n", __FUNCTION__, __LINE__,flow_index,port_index);
    return L7_FAILURE;
  }

  return L7_SUCCESS;  
}

L7_RC_t ptin_igmp_stat_client_clear(L7_uint16 evcId, L7_uint16 cvlan)
{
  L7_int flow_index=-1, client_index=-1;

  //return L7_SUCCESS;

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > Going to clear structure: evcId=%u,cvlan=%u\n", __FUNCTION__, __LINE__,evcId,cvlan);

  // Check if all client statistics should be cleared
  if (evcId==(L7_uint16)-1) {
    memset(L7_IGMP_Statistics_Clients,0x00,sizeof(L7_IGMP_Statistics_t)*IGMP_MAX_NUMBER_OF_FLOWS*SNOOP_MAX_NUMBER_OF_CLIENTS);
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > All client statistics of all evcs cleared\n", __FUNCTION__, __LINE__);
    // Everything is done
    return L7_SUCCESS;
  }

  if (ptin_igmp_evcClient_getIndex(evcId, cvlan, &flow_index, &client_index)!=L7_SUCCESS &&
      flow_index==-1) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_igmp_evcClient_getIndex (evcId=%u, cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
    return L7_FAILURE;
  }

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u,cvlan=%u => flow_index=%d,client_index=%d\n", __FUNCTION__, __LINE__,
           evcId,cvlan,flow_index,client_index);

  // Check if all client statistics of one EVC will be cleared
  if (cvlan==(L7_uint16)-1) {
    memset(L7_IGMP_Statistics_Clients[flow_index],0x00,sizeof(L7_IGMP_Statistics_t)*SNOOP_MAX_NUMBER_OF_CLIENTS);
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > All client=%u statistics of evc=%u cleared\n", __FUNCTION__, __LINE__,cvlan,evcId);
  }
  // Check if one client of EVC will be cleared
  else if (client_index!=-1) {
    memset(&L7_IGMP_Statistics_Clients[flow_index][client_index],0x00,sizeof(L7_IGMP_Statistics_t));
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Client=%u statistics of evc=%u cleared\n", __FUNCTION__, __LINE__,cvlan,evcId);
  } else {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Client non existent: (flow_index=%d,client_index=%d)\n", __FUNCTION__, __LINE__,flow_index,client_index);
    return L7_FAILURE;
  }

  return L7_SUCCESS;  
}


void ptin_igmp_evcClient_init(void)
{
  st_client_index_table *igmpflow;
  L7_int flow_index;
  L7_int index;

  for (flow_index=0; flow_index<IGMP_MAX_NUMBER_OF_FLOWS; flow_index++) {

    igmpflow = &igmp_client_table[flow_index];

    for (index=0; index<SNOOP_MAX_NUMBER_OF_CLIENTS; index++)
      igmpflow->index_free[index]=index;
    igmpflow->index_free_ptr = 0;
    memset(igmpflow->client_index,0xFF,sizeof(L7_int)*4096);
  }

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > ptin_evcClient_resetAll executed\n", __FUNCTION__, __LINE__);
}

L7_RC_t ptin_igmp_evcClient_clear(L7_uint16 evcId)
{
  st_client_index_table *igmpflow;
  L7_int flow_index;
  L7_int index;

  if (evcId>=FLOWS_MAX) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Get igmpflow index
  if (ptin_igmp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);

  igmpflow = &igmp_client_table[flow_index];

  for (index=0; index<SNOOP_MAX_NUMBER_OF_CLIENTS; index++)
    igmpflow->index_free[index]=index;
  igmpflow->index_free_ptr = 0;
  memset(igmpflow->client_index,0xFF,sizeof(L7_int)*4096);

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > ptin_evcClient_init executed\n", __FUNCTION__, __LINE__);

  return L7_SUCCESS;
}

L7_RC_t ptin_igmp_evcClient_isEmpty(L7_uint16 evcId, L7_BOOL *state)
{
  L7_int flow_index;

  if (evcId>=FLOWS_MAX) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Get igmpflow index
  if (ptin_igmp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (evcId%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);

  // Check if this client does not exist
  // If so, there is nothing to do. Return success
  if (state!=L7_NULLPTR) {
    *state = ( igmp_client_table[flow_index].index_free_ptr == 0 );
  }

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > evcId=%u => state=%u\n", __FUNCTION__, __LINE__,evcId,( igmp_client_table[flow_index].index_free_ptr == 0 ));

  return L7_SUCCESS;
}

L7_RC_t ptin_igmp_evcClient_isFull(L7_uint16 evcId, L7_BOOL *state)
{
  L7_int flow_index;

  if (evcId>=FLOWS_MAX) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) >Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Get igmpflow index
  if (ptin_igmp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);

  // Check if this client does not exist
  // If so, there is nothing to do. Return success
  if (state!=L7_NULLPTR) {
    *state = ( igmp_client_table[flow_index].index_free_ptr >= SNOOP_MAX_NUMBER_OF_CLIENTS );
  }
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > evcId=%u => state=%u\n", __FUNCTION__, __LINE__,evcId,( igmp_client_table[flow_index].index_free_ptr >= SNOOP_MAX_NUMBER_OF_CLIENTS ));

  return L7_SUCCESS;
}


L7_RC_t ptin_igmp_evcClient_add(L7_uint16 evcId, L7_uint16 cvlan)
{
  st_client_index_table *igmpflow;
  L7_int flow_index;
  L7_int client_index;

  if (evcId>=FLOWS_MAX || cvlan>4095) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
    return L7_FAILURE;
  }

  // Get igmpflow index
  if (ptin_igmp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);

  // Check if this client already exists
  // If so, there is nothing to do. Return success
  if (igmp_client_table[flow_index].client_index[cvlan]!=-1) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > This client already exists: cvlan=%u => client_index=%d\n", __FUNCTION__, __LINE__,cvlan,igmp_client_table[flow_index].client_index[cvlan]);
    return L7_SUCCESS;
  }

  igmpflow = &igmp_client_table[flow_index];

  // Check if there is no more free clients to add
  if ( igmpflow->index_free_ptr >= SNOOP_MAX_NUMBER_OF_CLIENTS ) {
    igmpflow->index_free_ptr = SNOOP_MAX_NUMBER_OF_CLIENTS;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > There is no space to add clients in evc %u (flow_index=%d)\n", __FUNCTION__, __LINE__,evcId,flow_index);
    return L7_FAILURE;
  }

  // Attrib index to this vlan
  client_index = igmpflow->index_free[igmpflow->index_free_ptr];
  igmpflow->client_index[cvlan] = client_index;
  // Clear this index, and increment index pointer to the next position
  igmpflow->index_free[igmpflow->index_free_ptr++] = -1;

  // Clear client statistics
  memset(&L7_IGMP_Statistics_Clients[flow_index][client_index],0x00,sizeof(L7_IGMP_Statistics_t));

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > evcId=%u,cvlan=%u => client_index=%d, free_ptr=%u\n", __FUNCTION__, __LINE__,
           evcId,
           cvlan,
           igmpflow->client_index[cvlan],
           igmpflow->index_free_ptr);

  return L7_SUCCESS;
}

L7_RC_t ptin_igmp_evcClient_remove(L7_uint16 evcId, L7_uint16 cvlan)
{
  st_client_index_table *igmpflow;
  L7_int flow_index;

  // If EVC id is null, clear all clients database (not of flows)
  if (evcId==(L7_uint16)-1) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > All clients of all EVC's will be removed\n", __FUNCTION__, __LINE__);
    (void)ptin_igmp_evcClient_init();
    return L7_SUCCESS;
  }

  // If only cvid is null, clear all clients of one evc in database
  if (cvlan==(L7_uint16)-1) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > All clients of EVC %u will be removed\n", __FUNCTION__, __LINE__,evcId);
    return ptin_igmp_evcClient_clear(evcId);
  }

  // Validate arguments
  if (evcId>=FLOWS_MAX || cvlan>4095) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
    return L7_FAILURE;
  }

  // Get igmpflow index
  if (ptin_igmp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);


  // Check if this client does not exist
  // If so, there is nothing to do. Return success
  if ( igmp_client_table[flow_index].client_index[cvlan] == -1 ) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > This client does not exist in evcId=%u\n", __FUNCTION__, __LINE__,evcId);
    return L7_SUCCESS;
  }

  igmpflow = &igmp_client_table[flow_index];

  // Check if there is any client
  if ( igmpflow->index_free_ptr <= 0 ) {
    igmpflow->index_free_ptr = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > There is no clients in evc %u (flow_index=%d)\n", __FUNCTION__, __LINE__,evcId,flow_index);
    return L7_FAILURE;
  }

  // Decrement the pointer of free indexes, and assign the index of the given vlan as free
  igmpflow->index_free[--(igmpflow->index_free_ptr)] = igmpflow->client_index[cvlan];
  // Clear index of the given vlan
  igmpflow->client_index[cvlan] = -1;

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > evcId=%u,cvlan=%u => free_ptr=%u\n", __FUNCTION__, __LINE__,evcId,cvlan,igmpflow->index_free_ptr);

  return L7_SUCCESS;
}

L7_RC_t ptin_igmp_evcClient_add_fromIntfVlan(L7_uint32 intf, L7_uint16 vlanId, L7_uint16 cvlan)
{
  L7_int port;
  L7_uint16 flow_id;

  if (ptin_translate_intf2port(intf, &port)!=L7_SUCCESS || port>=L7_SYSTEM_N_INTERF)
    return L7_FAILURE;

  if (ptin_flow_getId(port,vlanId,&flow_id)!=L7_SUCCESS)
    return L7_FAILURE;

  return ptin_igmp_evcClient_add(flow_id, cvlan);
}

L7_RC_t ptin_igmp_evcClient_remove_fromIntfVlan(L7_uint32 intf, L7_uint16 vlanId, L7_uint16 cvlan)
{
  L7_int port;
  L7_uint16 flow_id;

  if (ptin_translate_intf2port(intf, &port)!=L7_SUCCESS || port>=L7_SYSTEM_N_INTERF)
    return L7_FAILURE;

  if (ptin_flow_getId(port,vlanId,&flow_id)!=L7_SUCCESS)
    return L7_FAILURE;

  return ptin_igmp_evcClient_remove(flow_id, cvlan);
}

/*
 * MC flow management functions
 *
 */

void ptin_igmp_flow_init(void)
{
  L7_int index;

  // Clear igmpflows list
  for (index=0; index<IGMP_MAX_NUMBER_OF_FLOWS; index++)
    igmpflow_table.index_free[index]=index;
  igmpflow_table.index_free_ptr = 0;
  memset(igmpflow_table.flow_index,0xFF,sizeof(L7_int)*FLOWS_MAX);

  // Clear all clients list
  (void)ptin_igmp_evcClient_init();

  /* IGMP associaations */
  #ifdef IGMP_WHITELIST_FEATURE

  memset(&igmpWListDB, 0x00, sizeof(igmpWListDB));

  igmpWListDB.number_of_entries = 0;

  igmpWListDB.igmpWListTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PTINHAPI_COMPONENT_ID, IGMP_WHITELIST_CHANNELS_MAX * sizeof(avlTreeTables_t)); 
  igmpWListDB.igmpWListDataHeap = (ptinIgmpWListInfoData_t *)osapiMalloc(L7_PTINHAPI_COMPONENT_ID, IGMP_WHITELIST_CHANNELS_MAX * sizeof(ptinIgmpWListInfoData_t)); 

  if ((igmpWListDB.igmpWListTreeHeap == L7_NULLPTR) ||
      (igmpWListDB.igmpWListDataHeap == L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error allocating data for IGMP WList AVL Trees\n");
    return;
  }

  /* Initialize the storage for all IGMP associations */
  memset (&igmpWListDB.igmpWListAvlTree , 0x00, sizeof(avlTree_t));
  memset ( igmpWListDB.igmpWListTreeHeap, 0x00, sizeof(avlTreeTables_t)*IGMP_WHITELIST_CHANNELS_MAX);
  memset ( igmpWListDB.igmpWListDataHeap, 0x00, sizeof(ptinIgmpWListInfoData_t)*IGMP_WHITELIST_CHANNELS_MAX);

  // AVL Tree creations - snoopIpAvlTree
  avlCreateAvlTree(&(igmpWListDB.igmpWListAvlTree),
                   igmpWListDB.igmpWListTreeHeap,
                   igmpWListDB.igmpWListDataHeap,
                   IGMP_WHITELIST_CHANNELS_MAX, 
                   sizeof(ptinIgmpWListInfoData_t),
                   0x10,
                   sizeof(ptinIgmpWListDataKey_t));
  #endif

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > ptin_igmp_flow_init executed\n", __FUNCTION__, __LINE__);
}

L7_RC_t ptin_igmp_flow_isEmpty(L7_BOOL *state)
{
  if (state!=L7_NULLPTR) {
    *state = ( igmpflow_table.index_free_ptr == 0 );

    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [Success] %s(%d) > => state=%u, freeptr=%u\n", __FUNCTION__, __LINE__,( igmpflow_table.index_free_ptr == 0 ),igmpflow_table.index_free_ptr);
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_igmp_flow_isFull(L7_BOOL *state)
{
  if (state!=L7_NULLPTR) {
    *state = ( igmpflow_table.index_free_ptr >= IGMP_MAX_NUMBER_OF_FLOWS );

    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [Success] %s(%d) > => state=%u, freeptr=%u\n", __FUNCTION__, __LINE__,( igmpflow_table.index_free_ptr >= IGMP_MAX_NUMBER_OF_FLOWS ),igmpflow_table.index_free_ptr);
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_igmp_flow_add(L7_uint16 evcId)
{
  L7_BOOL evc_active;
  L7_int  flow_index;

  if (evcId>=FLOWS_MAX) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Check if this flow already exists
  // If so, there is nothing to do. Return success
  if (igmpflow_table.flow_index[evcId]!=-1) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > This EVC already exists: evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,igmpflow_table.flow_index[evcId]);
    return L7_SUCCESS;
  }

  // Check if there is no more free clients to add
  if ( igmpflow_table.index_free_ptr >= IGMP_MAX_NUMBER_OF_FLOWS ) {
    igmpflow_table.index_free_ptr = IGMP_MAX_NUMBER_OF_FLOWS;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > There is no space to add EVCs\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Check if EVC is active
  if (ptin_flow_inUse(evcId, &evc_active)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error getting evc active state (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (!evc_active) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > EVC %u does not exist\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Check if EVC is a multicast type
#if 0
  {
    L7_uint8 evc_type;

    if (ptin_flow_getType(evcId, &evc_type)!=L7_SUCCESS) {
      if (igmp_stat_debug)
        printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error getting evc type value (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
      return L7_FAILURE;
    }
    if (evc_type!=FLOW_TYPE_MULTICAST) {
      if (igmp_stat_debug)
        printf("{igmp_stat_debug} [ ERROR ] %s(%d) > EVC %u is not a multicast flow\n", __FUNCTION__, __LINE__,evcId);
      return L7_FAILURE;
    }
  }
#endif

  // Attrib index to this vlan
  flow_index = igmpflow_table.index_free[igmpflow_table.index_free_ptr];
  igmpflow_table.flow_index[evcId] = flow_index;
  // Clear this index, and increment index pointer to the next position
  igmpflow_table.index_free[igmpflow_table.index_free_ptr++] = -1;

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d, free_ptr=%u\n", __FUNCTION__, __LINE__,
           evcId,
           igmpflow_table.flow_index[evcId],
           igmpflow_table.index_free_ptr);

  // CLEAR LIST OF CLIENTS
  if (ptin_igmp_evcClient_clear(evcId)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Cannot init list of clients of EVC %u\n", __FUNCTION__, __LINE__,evcId);
    ptin_igmp_flow_remove(evcId);
    return L7_FAILURE;
  }

  // Clear EVC port statistics
  memset(L7_IGMP_Statistics_Intf[flow_index],0x00,sizeof(L7_IGMP_Statistics_t)*L7_SYSTEM_N_INTERF);

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > ptin_evcClient_init(evcId=%u) executed\n", __FUNCTION__, __LINE__,evcId);

  return L7_SUCCESS;
}

L7_RC_t ptin_igmp_flow_remove(L7_uint16 evcId)
{
  // If EVC id is null, clear all database
  if (evcId==(L7_uint16)-1) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ info  ] %s(%d) > All clients of all EVC's will be removed\n", __FUNCTION__, __LINE__);
    (void)ptin_igmp_flow_init();
    return L7_SUCCESS;
  }

  if (evcId>=FLOWS_MAX) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Check if this flow does not exist
  // If so, there is nothing to do. Return success
  if ( igmpflow_table.flow_index[evcId] == -1 ) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > There is no igmpflow associated to this evcId=%u\n", __FUNCTION__, __LINE__,evcId);
    return L7_SUCCESS;
  }

  // Check if there is any flow
  if ( igmpflow_table.index_free_ptr <= 0 ) {
    igmpflow_table.index_free_ptr = 0;
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > There is no MC flows\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Decrement the pointer of free indexes, and assign the index of the given vlan as free
  igmpflow_table.index_free[--(igmpflow_table.index_free_ptr)] = igmpflow_table.flow_index[evcId];
  // Clear index of the given vlan
  igmpflow_table.flow_index[evcId] = -1;

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > evcId=%u => free_ptr=%u\n", __FUNCTION__, __LINE__,
           evcId,
           igmpflow_table.index_free_ptr);

  return L7_SUCCESS;
}

static L7_RC_t ptin_igmp_stat_get_indexes(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index, L7_int *port_index)
{
  L7_int port;

  // GET PORT INDEX

  // Get port information
  if (ptin_translate_intf2port(intf,&port)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error getting port information from intf %u\n", __FUNCTION__, __LINE__,intf);
    return L7_FAILURE;
  }
  // Validate port value
  if (port<0 || port>=L7_SYSTEM_N_INTERF) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Port is invalid (port=%u)\n", __FUNCTION__, __LINE__,port);
    return L7_FAILURE;
  }

  if (port_index!=L7_NULLPTR) *port_index = port;

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > intf=%u => port=%d\n", __FUNCTION__, __LINE__,intf,port);

  // GET MCFLOW AND CLIENT INDEXES

  // Nothing to do
  if (flow_index==L7_NULLPTR && client_index==L7_NULLPTR) {
    return L7_SUCCESS;
  }

  if (ptin_igmp_client_getIndex(port, vlan, cvlan, flow_index, client_index)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Flow and/or client indexes are invalid (port=%u,vlan=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,port,vlan,cvlan);
    return L7_FAILURE;
  }

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > port=%u,vlan=%u,cvlan=%u => flow_index=%d,client_index=%d\n", __FUNCTION__, __LINE__,
           port,vlan,cvlan,((flow_index!=L7_NULLPTR) ? *flow_index : -1),((client_index!=L7_NULLPTR) ? *client_index : -1));

  return L7_SUCCESS;
}


static L7_RC_t ptin_igmp_client_getIndex(L7_uint16 port, L7_uint16 vlan, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index)
{
  L7_uint16 evcId;
  L7_uint8 type;

  if (ptin_flow_getId(port,snoop_mcast_vlan[vlan],&evcId)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error getting EVC id from port %u and vlan %u\n", __FUNCTION__, __LINE__,port,vlan);
    return L7_FAILURE;
  }
  if (ptin_flow_getType(evcId,&type)!=L7_SUCCESS || type!=FLOW_TYPE_MULTICAST) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > EVC %u is not multicast\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > port=%u,vlan=%u => evcId=%u\n", __FUNCTION__, __LINE__,port,vlan,evcId);

  if (ptin_igmp_evcClient_getIndex(evcId, cvlan, flow_index, client_index)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_igmp_evcClient_getIndex (evcId=%u, cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
    return L7_FAILURE;
  }

  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > ptin_igmp_client_getIndex executed\n", __FUNCTION__, __LINE__);

  return L7_SUCCESS;
}

static L7_RC_t ptin_igmp_evcClient_getIndex(L7_uint16 evcId, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index)
{
  st_client_index_table *igmpflow;
  L7_int  igmpflowIndex;

  // Validate EVC id
  if (evcId>=FLOWS_MAX) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Get igmpflow index
  if (ptin_igmp_flow_getIndex(evcId, &igmpflowIndex)!=L7_SUCCESS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // igmpflow index found
  if (flow_index!=L7_NULLPTR)  *flow_index = igmpflowIndex;
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,igmpflowIndex);

  // Validate CVlan
  if (cvlan>4095) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (cvlan=%u)\n", __FUNCTION__, __LINE__,cvlan);
    return L7_FAILURE;
  }

  igmpflow = &igmp_client_table[igmpflowIndex];

  // Check if this vlan is an added client, if not, exit with error
  if (igmpflow->client_index[cvlan]<0 || igmpflow->client_index[cvlan]>=SNOOP_MAX_NUMBER_OF_CLIENTS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid client index (client_index=%d, cvlan=%u, flow_index=%d\n", __FUNCTION__, __LINE__,
             igmpflow->client_index[cvlan],cvlan,igmpflowIndex);
    return L7_FAILURE;
  }

  // client index found
  if (client_index!=L7_NULLPTR)  *client_index = igmpflow->client_index[cvlan];
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > evcId=%u,cvlan=%u => flow_index=%d,client_index=%d\n", __FUNCTION__, __LINE__,evcId,cvlan,igmpflowIndex,igmpflow->client_index[cvlan]);

  return L7_SUCCESS;
}

static L7_RC_t ptin_igmp_flow_getIndex(L7_uint16 evcId, L7_int *flow_index)
{
  if (evcId>=FLOWS_MAX) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Check if this vlan is an added client, if not, exit with error
  if (igmpflow_table.flow_index[evcId]<0 || igmpflow_table.flow_index[evcId]>=IGMP_MAX_NUMBER_OF_FLOWS) {
    if (igmp_stat_debug)
      printf("{igmp_stat_debug} [ ERROR ] %s(%d) > Invalid flow_index (flow_index=%d) for evcId=%u\n", __FUNCTION__, __LINE__,igmpflow_table.flow_index[evcId],evcId);
    return L7_FAILURE;
  }

  // igmpflow index found
  if (flow_index!=L7_NULLPTR)  *flow_index = igmpflow_table.flow_index[evcId];
  if (igmp_stat_debug)
    printf("{igmp_stat_debug} [Success] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,igmpflow_table.flow_index[evcId]);

  return L7_SUCCESS;
}


























void ptin_dhcp_stat_init(void)
{
  // Clear management database
  ptin_dhcp_flow_init();

  // Clear statistics database
  memset(L7_DHCP_Statistics_Intf_Global,0x00,sizeof(L7_DHCP_Statistics_t)*L7_SYSTEM_N_INTERF);
  memset(L7_DHCP_Statistics_Intf,0x00,sizeof(L7_DHCP_Statistics_t)*DHCP_MAX_NUMBER_OF_FLOWS*L7_SYSTEM_N_INTERF);
  memset(L7_DHCP_Statistics_Clients,0x00,sizeof(L7_DHCP_Statistics_t)*DHCP_MAX_NUMBER_OF_FLOWS*SNOOP_MAX_NUMBER_OF_CLIENTS);

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > ptin_snoop_stat_init executed\n", __FUNCTION__, __LINE__);
}

L7_RC_t ptin_dhcp_stat_clear_field(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, dhcp_stat_field_t field)
{
  L7_int flow_index=-1, client_index=-1, port_index=-1;
  L7_DHCP_Statistics_t *stat_port_g=L7_NULLPTR;
  L7_DHCP_Statistics_t *stat_port=L7_NULLPTR;
  L7_DHCP_Statistics_t *stat_client=L7_NULLPTR;

  //return L7_SUCCESS;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > Going to clear field: intf=%u,vlan=%u,cvlan=%u, field=%u\n", __FUNCTION__, __LINE__,intf,vlan,cvlan,field);

  ptin_dhcp_stat_get_indexes(intf,vlan,cvlan, &flow_index, &client_index, &port_index);
//if (ptin_dhcp_stat_get_indexes(intf,vlan,cvlan, &flow_index, &client_index, &port_index)!=L7_SUCCESS &&
//    flow_index==-1)  {
//  if (dhcp_stat_debug)
//    printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_dhcp_stat_increment_field (intf=%u,vlan=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,intf,vlan,cvlan);
//  return L7_FAILURE;
//}

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > intf=%u,vlan=%u,cvlan=%u => flow_index=%d,client_index=%d,port_index=%d\n", __FUNCTION__, __LINE__,
           intf,vlan,cvlan,flow_index,client_index,port_index);

  // Global port statistics
  if (port_index>=0 && port_index<L7_SYSTEM_N_INTERF)
    stat_port_g = &L7_DHCP_Statistics_Intf_Global[port_index];
  else
    stat_port_g = L7_NULLPTR;

  // EVC port statistics
  if (flow_index>=0 && flow_index<DHCP_MAX_NUMBER_OF_FLOWS &&
      port_index>=0 && port_index<L7_SYSTEM_N_INTERF)
    stat_port   = &L7_DHCP_Statistics_Intf[flow_index][port_index];
  else
    stat_port   = L7_NULLPTR;

  // EVC client statistics
  if (flow_index>=0 && flow_index<DHCP_MAX_NUMBER_OF_FLOWS &&
      client_index>=0 && client_index<SNOOP_MAX_NUMBER_OF_CLIENTS)
    stat_client = &L7_DHCP_Statistics_Clients[flow_index][client_index];
  else
    stat_client = L7_NULLPTR;

  if (stat_port_g==L7_NULLPTR && stat_port==L7_NULLPTR && stat_client==L7_NULLPTR) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > No reference found to one port or one client!\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  switch (field) {
  case DHCP_STAT_FIELD_RX_INTERCEPTED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_intercepted = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_intercepted = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_intercepted = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_INTERCEPTED field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_FILTERED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_filtered = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_filtered = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_filtered = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_FILTERED field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_FORWARDED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_tx_forwarded = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_tx_forwarded = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_tx_forwarded = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_TX_FORWARDED field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_tx_failed = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_tx_failed = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_tx_failed = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_TX_FAILED field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITH_OPTION82:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITH_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_tx_client_requests_with_option82 = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_tx_client_requests_with_option82 = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_tx_client_requests_with_option82 = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_server_replies_with_option82 = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_server_replies_with_option82 = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_server_replies_with_option82 = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_SERVER_REQUESTS_WITH_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITH_OPTION82:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_TX_SERVER_REQUESTS_WITH_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_client_requests_without_option82 = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_client_requests_without_option82 = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_client_requests_without_option82 = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITHOUT_OPTION82:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITHOUT_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITHOUT_OPTION82:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_SERVER_REQUESTS_WITHOUT_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_tx_server_replies_without_option82 = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_tx_server_replies_without_option82 = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_tx_server_replies_without_option82 = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_TX_SERVER_REQUESTS_WITHOUT_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOUTOP82_ON_TRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOUTOP82_ON_TRUSTED_INTF field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOP82_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_client_pkts_withOp82_onUntrustedIntf = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_client_pkts_withOp82_onUntrustedIntf = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_client_pkts_withOp82_onUntrustedIntf = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOP82_ON_UNTRUSTED_INTF field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOP82_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_server_pkts_withOp82_onUntrustedIntf = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_server_pkts_withOp82_onUntrustedIntf = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_server_pkts_withOp82_onUntrustedIntf = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOP82_ON_UNTRUSTED_INTF field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOUTOP82_ON_TRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf = 0;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf = 0;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOUTOP82_ON_TRUSTED_INTF field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_ALL:
    if (stat_port_g!=L7_NULLPTR) {
      stat_port_g->dhcp_rx_intercepted = 0;
      stat_port_g->dhcp_rx = 0;
      stat_port_g->dhcp_rx_filtered = 0;
      stat_port_g->dhcp_tx_forwarded = 0;
      stat_port_g->dhcp_tx_failed = 0;
      stat_port_g->dhcp_rx_client_requests_without_option82 = 0;
      stat_port_g->dhcp_tx_client_requests_with_option82 = 0;
      stat_port_g->dhcp_rx_server_replies_with_option82 = 0;
      stat_port_g->dhcp_tx_server_replies_without_option82 = 0;
      stat_port_g->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf = 0;
      stat_port_g->dhcp_rx_client_pkts_withOp82_onUntrustedIntf = 0;
      stat_port_g->dhcp_rx_server_pkts_withOp82_onUntrustedIntf = 0;
      stat_port_g->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf = 0;
    }
    if (stat_port!=L7_NULLPTR) {
      stat_port->dhcp_rx_intercepted = 0;
      stat_port->dhcp_rx = 0;
      stat_port->dhcp_rx_filtered = 0;
      stat_port->dhcp_tx_forwarded = 0;
      stat_port->dhcp_tx_failed = 0;
      stat_port->dhcp_rx_client_requests_without_option82 = 0;
      stat_port->dhcp_tx_client_requests_with_option82 = 0;
      stat_port->dhcp_rx_server_replies_with_option82 = 0;
      stat_port->dhcp_tx_server_replies_without_option82 = 0;
      stat_port->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf = 0;
      stat_port->dhcp_rx_client_pkts_withOp82_onUntrustedIntf = 0;
      stat_port->dhcp_rx_server_pkts_withOp82_onUntrustedIntf = 0;
      stat_port->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf = 0;
    }
    if (stat_client!=L7_NULLPTR) {
      stat_client->dhcp_rx_intercepted = 0;
      stat_client->dhcp_rx = 0;
      stat_client->dhcp_rx_filtered = 0;
      stat_client->dhcp_tx_forwarded = 0;
      stat_client->dhcp_tx_failed = 0;
      stat_client->dhcp_rx_client_requests_without_option82 = 0;
      stat_client->dhcp_tx_client_requests_with_option82 = 0;
      stat_client->dhcp_rx_server_replies_with_option82 = 0;
      stat_client->dhcp_tx_server_replies_without_option82 = 0;
      stat_client->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf = 0;
      stat_client->dhcp_rx_client_pkts_withOp82_onUntrustedIntf = 0;
      stat_client->dhcp_rx_server_pkts_withOp82_onUntrustedIntf = 0;
      stat_client->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf = 0;
    }
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Cleared DHCP_STAT_FIELD_ALL field\n", __FUNCTION__, __LINE__);
    break;

  default:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid field reference (field=%u)\n", __FUNCTION__, __LINE__,field);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_stat_increment_field(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, dhcp_stat_field_t field)
{
  L7_int port;
  L7_int flow_index=-1, client_index=-1, port_index=-1;
  L7_DHCP_Statistics_t *stat_port_g=L7_NULLPTR;
  L7_DHCP_Statistics_t *stat_port=L7_NULLPTR;
  L7_DHCP_Statistics_t *stat_client=L7_NULLPTR;

  //return L7_SUCCESS;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > Going to increment field: intf=%u,vlan=%u,cvlan=%u, field=%u\n", __FUNCTION__, __LINE__,intf,vlan,cvlan,field);

  // Get port information
  if (ptin_translate_intf2port(intf,&port)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error getting port information from intf %u\n", __FUNCTION__, __LINE__,intf);
    return L7_FAILURE;
  }

  ptin_dhcp_stat_get_indexes(intf,vlan,cvlan, &flow_index, &client_index, &port_index);
//if (ptin_dhcp_stat_get_indexes(intf,vlan,cvlan, &flow_index, &client_index, &port_index)!=L7_SUCCESS &&
//    flow_index==-1)  {
//  if (dhcp_stat_debug)
//    printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_dhcp_stat_increment_field (intf=%u,vlan=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,intf,vlan,cvlan);
//  return L7_FAILURE;
//}

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > intf=%u,vlan=%u,cvlan=%u => flow_index=%d,client_index=%d,port_index=%d\n", __FUNCTION__, __LINE__,
           intf,vlan,cvlan,flow_index,client_index,port_index);

  // Global port statistics
  if (port_index>=0 && port_index<L7_SYSTEM_N_INTERF)
    stat_port_g = &L7_DHCP_Statistics_Intf_Global[port_index];
  else
    stat_port_g = L7_NULLPTR;

  // EVC port statistics
  if (flow_index>=0 && flow_index<DHCP_MAX_NUMBER_OF_FLOWS &&
      port_index>=0 && port_index<L7_SYSTEM_N_INTERF)
    stat_port   = &L7_DHCP_Statistics_Intf[flow_index][port_index];
  else
    stat_port   = L7_NULLPTR;

  // EVC client statistics
  if (flow_index>=0 && flow_index<DHCP_MAX_NUMBER_OF_FLOWS &&
      client_index>=0 && client_index<SNOOP_MAX_NUMBER_OF_CLIENTS &&
      port_index>=0 && port_index<L7_SYSTEM_N_INTERF /*L7_SYSTEM_PON_PORTS*/)
    stat_client = &L7_DHCP_Statistics_Clients[flow_index][client_index];
  else
    stat_client = L7_NULLPTR;

  if (stat_port_g==L7_NULLPTR && stat_port==L7_NULLPTR && stat_client==L7_NULLPTR) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > No reference found to one port or one client!\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  switch (field) {
  case DHCP_STAT_FIELD_RX_INTERCEPTED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_intercepted++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_intercepted++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_intercepted++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_INTERCEPTED field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_FILTERED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_filtered++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_filtered++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_filtered++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_FILTERED field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_FORWARDED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_tx_forwarded++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_tx_forwarded++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_tx_forwarded++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_TX_FORWARDED field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_FAILED:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_tx_failed++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_tx_failed++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_tx_failed++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_TX_FAILED field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITH_OPTION82:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITH_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_tx_client_requests_with_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_tx_client_requests_with_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_tx_client_requests_with_option82++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_server_replies_with_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_server_replies_with_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_server_replies_with_option82++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_SERVER_REQUESTS_WITH_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITH_OPTION82:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_TX_SERVER_REQUESTS_WITH_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_client_requests_without_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_client_requests_without_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_client_requests_without_option82++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITHOUT_OPTION82:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITHOUT_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITHOUT_OPTION82:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_SERVER_REQUESTS_WITHOUT_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTION82:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_tx_server_replies_without_option82++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_tx_server_replies_without_option82++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_tx_server_replies_without_option82++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_TX_SERVER_REQUESTS_WITHOUT_OPTION82 field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOUTOP82_ON_TRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_client_pkts_withoutOp82_onTrustedIntf++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOUTOP82_ON_TRUSTED_INTF field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOP82_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_client_pkts_withOp82_onUntrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_client_pkts_withOp82_onUntrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_client_pkts_withOp82_onUntrustedIntf++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOP82_ON_UNTRUSTED_INTF field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOP82_ON_UNTRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_server_pkts_withOp82_onUntrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_server_pkts_withOp82_onUntrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_server_pkts_withOp82_onUntrustedIntf++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOP82_ON_UNTRUSTED_INTF field\n", __FUNCTION__, __LINE__);
    break;

  case DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOUTOP82_ON_TRUSTED_INTF:
    if (stat_port_g!=L7_NULLPTR)  stat_port_g ->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf++;
    if (stat_port  !=L7_NULLPTR)  stat_port   ->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf++;
    if (stat_client!=L7_NULLPTR)  stat_client ->dhcp_rx_server_pkts_withoutOp82_onTrustedIntf++;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > Incremented DHCP_STAT_FIELD_RX_SERVER_PKTS_WITHOUTOP82_ON_TRUSTED_INTF field\n", __FUNCTION__, __LINE__);
    break;

  default:
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid field reference (field=%u)\n", __FUNCTION__, __LINE__,field);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


L7_RC_t ptin_dhcp_stat_get(L7_uint16 evcId, L7_uint16 cvlan, L7_uint16 port, L7_DHCP_Statistics_t *stat_client, L7_DHCP_Statistics_t *stat_port, L7_DHCP_Statistics_t *stat_port_g)
{
  L7_int flow_index, client_index, port_index;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > Starting\n", __FUNCTION__, __LINE__);

  if (stat_port_g!=L7_NULLPTR) {
    if (port>=L7_SYSTEM_N_INTERF) {
      if (dhcp_stat_debug)
        printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid port reference (port=%u)\n", __FUNCTION__, __LINE__,port);
      return L7_FAILURE;
    }
    port_index = port;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > port=%u => port_index=%u\n", __FUNCTION__, __LINE__,port,port_index);
    memcpy(stat_port_g,&L7_DHCP_Statistics_Intf_Global[port_index],sizeof(L7_DHCP_Statistics_t));
  }

  // evcId==0, corresponds to read general port statistics
//if (evcId==0) {
//  if (dhcp_stat_debug)
//    printf("{dhcp_stat_debug} [ info  ] %s(%d) > Only global port statistics will be read\n", __FUNCTION__, __LINE__);
//  return L7_SUCCESS;
//}

  // Validate evcId
  if (evcId>=FLOWS_MAX) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  if (stat_port!=L7_NULLPTR) {
    if (port>=L7_SYSTEM_N_INTERF) {
      if (dhcp_stat_debug)
        printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid port reference (port=%u)\n", __FUNCTION__, __LINE__,port);
      return L7_FAILURE;
    }
    if (ptin_dhcp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
      if (dhcp_stat_debug)
        printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_dhcp_flow_getIndex (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
      return L7_FAILURE;
    }
    port_index = port;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > port=%u => port_index=%u; evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,port,port_index,evcId,flow_index);
    memcpy(stat_port,&L7_DHCP_Statistics_Intf[flow_index][port_index],sizeof(L7_DHCP_Statistics_t));
  }

  if (stat_client!=L7_NULLPTR) {
    if (cvlan>4095) {
      if (dhcp_stat_debug)
        printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid cvlan value (cvlan=%u)\n", __FUNCTION__, __LINE__,cvlan);
      return L7_FAILURE;
    }
    if (ptin_dhcp_evcClient_getIndex(evcId,cvlan,&flow_index,&client_index)!=L7_SUCCESS) {
      if (dhcp_stat_debug)
        printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_dhcp_evcClient_getIndex (evcId=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
      return L7_FAILURE;
    }
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u,cvlan=%u => flow_index=%d,client_index=%d\n", __FUNCTION__, __LINE__,evcId,cvlan,flow_index,client_index);
    memcpy(stat_client,&L7_DHCP_Statistics_Clients[flow_index][client_index],sizeof(L7_DHCP_Statistics_t));
  }

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > ptin_snoop_stat_get executed\n", __FUNCTION__, __LINE__);

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_stat_port_clear(L7_uint16 evcId, L7_uint16 port)
{
  L7_int flow_index=-1, port_index=-1;

  //return L7_SUCCESS;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > Going to clear structure: evcId=%u,port=%u\n", __FUNCTION__, __LINE__,evcId,port);

  // Check if all port statistics will be cleared
  if (evcId==(L7_uint16)-1) {
    if (port==(L7_uint16)-1)
      memset(L7_DHCP_Statistics_Intf_Global,0x00,sizeof(L7_DHCP_Statistics_t)*L7_SYSTEM_N_INTERF);
    memset(L7_DHCP_Statistics_Intf,0x00,sizeof(L7_DHCP_Statistics_t)*DHCP_MAX_NUMBER_OF_FLOWS*L7_SYSTEM_N_INTERF);
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > All port statistics of all evcs cleared\n", __FUNCTION__, __LINE__);
    // Everything is done
    return L7_SUCCESS;
  }

  // Get dhcpflow and port indexes
  if (evcId<FLOWS_MAX) {
    if (ptin_dhcp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
      if (dhcp_stat_debug)
        printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_dhcp_flow_getIndex (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
      return L7_FAILURE;
    }
  }
  port_index = (port<L7_SYSTEM_N_INTERF) ? port : -1;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u,port=%u => flow_index=%d,port_index=%u\n", __FUNCTION__, __LINE__,
           evcId,port,flow_index,port_index);

  // Check if all port statistics (of one EVC) should be cleared
  if (port==(L7_uint16)-1) {
    if (flow_index!=-1)
      memset(L7_DHCP_Statistics_Intf[flow_index],0x00,sizeof(L7_DHCP_Statistics_t)*L7_SYSTEM_N_INTERF);
    else
      memset(L7_DHCP_Statistics_Intf_Global,0x00,sizeof(L7_DHCP_Statistics_t)*L7_SYSTEM_N_INTERF);

    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > All port statistics of evc=%u cleared\n", __FUNCTION__, __LINE__,evcId);
  } else if (port_index>=0 && port_index<L7_SYSTEM_N_INTERF) {
    if (flow_index!=-1)
      memset(&L7_DHCP_Statistics_Intf[flow_index][port_index],0x00,sizeof(L7_DHCP_Statistics_t));
    else
      memset(&L7_DHCP_Statistics_Intf_Global[port_index],0x00,sizeof(L7_DHCP_Statistics_t));

    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Port=%u statistics of evc=%u cleared\n", __FUNCTION__, __LINE__,port,evcId);
  } else {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid Port Index: (flow_index=%d,port_index=%d)\n", __FUNCTION__, __LINE__,flow_index,port_index);
    return L7_FAILURE;
  }

  return L7_SUCCESS;  
}

L7_RC_t ptin_dhcp_stat_client_clear(L7_uint16 evcId, L7_uint16 cvlan)
{
  L7_int flow_index=-1, client_index=-1;

  //return L7_SUCCESS;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > Going to clear structure: evcId=%u,cvlan=%u\n", __FUNCTION__, __LINE__,evcId,cvlan);

  // Check if all client statistics should be cleared
  if (evcId==(L7_uint16)-1) {
    memset(L7_DHCP_Statistics_Clients,0x00,sizeof(L7_DHCP_Statistics_t)*DHCP_MAX_NUMBER_OF_FLOWS*SNOOP_MAX_NUMBER_OF_CLIENTS);
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > All client statistics of all evcs cleared\n", __FUNCTION__, __LINE__);
    // Everything is done
    return L7_SUCCESS;
  }

  if (ptin_dhcp_evcClient_getIndex(evcId, cvlan, &flow_index, &client_index)!=L7_SUCCESS &&
      flow_index==-1) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_dhcp_evcClient_getIndex (evcId=%u, cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
    return L7_FAILURE;
  }

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u,cvlan=%u => flow_index=%d,client_index=%d\n", __FUNCTION__, __LINE__,
           evcId,cvlan,flow_index,client_index);

  // Check if all client statistics of one EVC will be cleared
  if (cvlan==(L7_uint16)-1) {
    memset(L7_DHCP_Statistics_Clients[flow_index],0x00,sizeof(L7_DHCP_Statistics_t)*SNOOP_MAX_NUMBER_OF_CLIENTS);
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > All client=%u statistics of evc=%u cleared\n", __FUNCTION__, __LINE__,cvlan,evcId);
  }
  // Check if one client of EVC will be cleared
  else if (client_index!=-1) {
    memset(&L7_DHCP_Statistics_Clients[flow_index][client_index],0x00,sizeof(L7_DHCP_Statistics_t));
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Client=%u statistics of evc=%u cleared\n", __FUNCTION__, __LINE__,cvlan,evcId);
  } else {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Client non existent: (flow_index=%d,client_index=%d)\n", __FUNCTION__, __LINE__,flow_index,client_index);
    return L7_FAILURE;
  }

  return L7_SUCCESS;  
}


void ptin_dhcp_evcClient_init(void)
{
  st_client_index_table *dhcpflow;
  L7_int flow_index;
  L7_int index;

  for (flow_index=0; flow_index<DHCP_MAX_NUMBER_OF_FLOWS; flow_index++) {

    dhcpflow = &dhcp_client_table[flow_index];

    for (index=0; index<SNOOP_MAX_NUMBER_OF_CLIENTS; index++)
      dhcpflow->index_free[index]=index;
    dhcpflow->index_free_ptr = 0;
    memset(dhcpflow->client_index,0xFF,sizeof(L7_int)*4096);
  }

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > ptin_evcClient_resetAll executed\n", __FUNCTION__, __LINE__);
}

L7_RC_t ptin_dhcp_evcClient_clear(L7_uint16 evcId)
{
  st_client_index_table *dhcpflow;
  L7_int flow_index;
  L7_int index;

  if (evcId>=FLOWS_MAX) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Get dhcpflow index
  if (ptin_dhcp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);

  dhcpflow = &dhcp_client_table[flow_index];

  for (index=0; index<SNOOP_MAX_NUMBER_OF_CLIENTS; index++)
    dhcpflow->index_free[index]=index;
  dhcpflow->index_free_ptr = 0;
  memset(dhcpflow->client_index,0xFF,sizeof(L7_int)*4096);

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > ptin_evcClient_init executed\n", __FUNCTION__, __LINE__);

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_evcClient_isEmpty(L7_uint16 evcId, L7_BOOL *state)
{
  L7_int flow_index;

  if (evcId>=FLOWS_MAX) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Get dhcpflow index
  if (ptin_dhcp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (evcId%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);

  // Check if this client does not exist
  // If so, there is nothing to do. Return success
  if (state!=L7_NULLPTR) {
    *state = ( dhcp_client_table[flow_index].index_free_ptr == 0 );
  }

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > evcId=%u => state=%u\n", __FUNCTION__, __LINE__,evcId,( dhcp_client_table[flow_index].index_free_ptr == 0 ));

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_evcClient_isFull(L7_uint16 evcId, L7_BOOL *state)
{
  L7_int flow_index;

  if (evcId>=FLOWS_MAX) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) >Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Get dhcpflow index
  if (ptin_dhcp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);

  // Check if this client does not exist
  // If so, there is nothing to do. Return success
  if (state!=L7_NULLPTR) {
    *state = ( dhcp_client_table[flow_index].index_free_ptr >= SNOOP_MAX_NUMBER_OF_CLIENTS );
  }
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > evcId=%u => state=%u\n", __FUNCTION__, __LINE__,evcId,( dhcp_client_table[flow_index].index_free_ptr >= SNOOP_MAX_NUMBER_OF_CLIENTS ));

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_evcClient_add(L7_uint16 evcId, L7_uint16 cvlan)
{
  st_client_index_table *dhcpflow;
  L7_int flow_index;
  L7_int client_index;

  if (evcId>=FLOWS_MAX || cvlan>4095) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
    return L7_FAILURE;
  }

  // Get dhcpflow index
  if (ptin_dhcp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);

  // Check if this client already exists
  // If so, there is nothing to do. Return success
  if (dhcp_client_table[flow_index].client_index[cvlan]!=-1) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > This client already exists: cvlan=%u => client_index=%d\n", __FUNCTION__, __LINE__,cvlan,dhcp_client_table[flow_index].client_index[cvlan]);
    return L7_SUCCESS;
  }

  dhcpflow = &dhcp_client_table[flow_index];

  // Check if there is no more free clients to add
  if ( dhcpflow->index_free_ptr >= SNOOP_MAX_NUMBER_OF_CLIENTS ) {
    dhcpflow->index_free_ptr = SNOOP_MAX_NUMBER_OF_CLIENTS;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > There is no space to add clients in evc %u (flow_index=%d)\n", __FUNCTION__, __LINE__,evcId,flow_index);
    return L7_FAILURE;
  }

  // Attrib index to this vlan
  client_index = dhcpflow->index_free[dhcpflow->index_free_ptr];
  dhcpflow->client_index[cvlan] = client_index;
  // Clear this index, and increment index pointer to the next position
  dhcpflow->index_free[dhcpflow->index_free_ptr++] = -1;

  // Clear client statistics
  memset(&L7_DHCP_Statistics_Clients[flow_index][client_index],0x00,sizeof(L7_DHCP_Statistics_t));

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > evcId=%u,cvlan=%u => client_index=%d, free_ptr=%u\n", __FUNCTION__, __LINE__,
           evcId,
           cvlan,
           dhcpflow->client_index[cvlan],
           dhcpflow->index_free_ptr);

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_evcClient_remove(L7_uint16 evcId, L7_uint16 cvlan)
{
  st_client_index_table *dhcpflow;
  L7_int flow_index;

  // If EVC id is null, clear all clients database (not of flows)
  if (evcId==(L7_uint16)-1) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > All clients of all EVC's will be removed\n", __FUNCTION__, __LINE__);
    (void)ptin_dhcp_evcClient_init();
    return L7_SUCCESS;
  }

  // If only cvid is null, clear all clients of one evc in database
  if (cvlan==(L7_uint16)-1) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > All clients of EVC %u will be removed\n", __FUNCTION__, __LINE__,evcId);
    return ptin_dhcp_evcClient_clear(evcId);
  }

  // Validate arguments
  if (evcId>=FLOWS_MAX || cvlan>4095) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
    return L7_FAILURE;
  }

  // Get dhcpflow index
  if (ptin_dhcp_flow_getIndex(evcId, &flow_index)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,flow_index);


  // Check if this client does not exist
  // If so, there is nothing to do. Return success
  if ( dhcp_client_table[flow_index].client_index[cvlan] == -1 ) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > This client does not exist in evcId=%u\n", __FUNCTION__, __LINE__,evcId);
    return L7_SUCCESS;
  }

  dhcpflow = &dhcp_client_table[flow_index];

  // Check if there is any client
  if ( dhcpflow->index_free_ptr <= 0 ) {
    dhcpflow->index_free_ptr = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > There is no clients in evc %u (flow_index=%d)\n", __FUNCTION__, __LINE__,evcId,flow_index);
    return L7_FAILURE;
  }

  // Decrement the pointer of free indexes, and assign the index of the given vlan as free
  dhcpflow->index_free[--(dhcpflow->index_free_ptr)] = dhcpflow->client_index[cvlan];
  // Clear index of the given vlan
  dhcpflow->client_index[cvlan] = -1;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > evcId=%u,cvlan=%u => free_ptr=%u\n", __FUNCTION__, __LINE__,evcId,cvlan,dhcpflow->index_free_ptr);

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_evcClient_add_fromIntfVlan(L7_uint32 intf, L7_uint16 vlanId, L7_uint16 cvlan)
{
  L7_int port;
  L7_uint16 flow_id;

  if (ptin_translate_intf2port(intf, &port)!=L7_SUCCESS || port>=L7_SYSTEM_N_INTERF) {
    printf("%s(%d) intf=%u\r\n", __FUNCTION__, __LINE__,intf);
    return L7_FAILURE;
  }

  if (ptin_flow_getId(port,vlanId,&flow_id)!=L7_SUCCESS) {
    printf("%s(%d) port=%u, vlanId=%u\r\n", __FUNCTION__, __LINE__,port,vlanId);
    return L7_FAILURE;
  }

  return ptin_dhcp_evcClient_add(flow_id, cvlan);
}

L7_RC_t ptin_dhcp_evcClient_remove_fromIntfVlan(L7_uint32 intf, L7_uint16 vlanId, L7_uint16 cvlan)
{
  L7_int port;
  L7_uint16 flow_id;

  if (ptin_translate_intf2port(intf, &port)!=L7_SUCCESS || port>=L7_SYSTEM_N_INTERF)
    return L7_FAILURE;

  if (ptin_flow_getId(port,vlanId,&flow_id)!=L7_SUCCESS)
    return L7_FAILURE;

  return ptin_dhcp_evcClient_remove(flow_id, cvlan);
}

/*
 * MC flow management functions
 *
 */

void ptin_dhcp_flow_init(void)
{
  L7_int index;

  // Clear dhcpflows list
  for (index=0; index<DHCP_MAX_NUMBER_OF_FLOWS; index++)
    dhcpflow_table.index_free[index]=index;
  dhcpflow_table.index_free_ptr = 0;
  memset(dhcpflow_table.flow_index,0xFF,sizeof(L7_int)*FLOWS_MAX);

  // Clear all clients list
  (void)ptin_dhcp_evcClient_init();

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > ptin_dhcp_flow_init executed\n", __FUNCTION__, __LINE__);
}

L7_RC_t ptin_dhcp_flow_isEmpty(L7_BOOL *state)
{
  if (state!=L7_NULLPTR) {
    *state = ( dhcpflow_table.index_free_ptr == 0 );

    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [Success] %s(%d) > => state=%u, freeptr=%u\n", __FUNCTION__, __LINE__,( dhcpflow_table.index_free_ptr == 0 ),dhcpflow_table.index_free_ptr);
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_flow_isFull(L7_BOOL *state)
{
  if (state!=L7_NULLPTR) {
    *state = ( dhcpflow_table.index_free_ptr >= DHCP_MAX_NUMBER_OF_FLOWS );

    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [Success] %s(%d) > => state=%u, freeptr=%u\n", __FUNCTION__, __LINE__,( dhcpflow_table.index_free_ptr >= DHCP_MAX_NUMBER_OF_FLOWS ),dhcpflow_table.index_free_ptr);
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_flow_add(L7_uint16 evcId)
{
  L7_BOOL evc_active;
  L7_int  flow_index;

  if (evcId>=FLOWS_MAX) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Check if this flow already exists
  // If so, there is nothing to do. Return success
  if (dhcpflow_table.flow_index[evcId]!=-1) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > This EVC already exists: evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,dhcpflow_table.flow_index[evcId]);
    return L7_SUCCESS;
  }

  // Check if there is no more free clients to add
  if ( dhcpflow_table.index_free_ptr >= DHCP_MAX_NUMBER_OF_FLOWS ) {
    dhcpflow_table.index_free_ptr = DHCP_MAX_NUMBER_OF_FLOWS;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > There is no space to add EVCs\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Check if EVC is active
  if (ptin_flow_inUse(evcId, &evc_active)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error getting evc active state (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }
  if (!evc_active) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > EVC %u does not exist\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Check if EVC is a multicast type
#if 0
  {
    L7_uint8 evc_type;

    if (ptin_flow_getType(evcId, &evc_type)!=L7_SUCCESS) {
      if (dhcp_stat_debug)
        printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error getting evc type value (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
      return L7_FAILURE;
    }
    if (evc_type!=FLOW_TYPE_MULTICAST) {
      if (dhcp_stat_debug)
        printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > EVC %u is not a multicast flow\n", __FUNCTION__, __LINE__,evcId);
      return L7_FAILURE;
    }
  }
#endif

  // Attrib index to this vlan
  flow_index = dhcpflow_table.index_free[dhcpflow_table.index_free_ptr];
  dhcpflow_table.flow_index[evcId] = flow_index;
  // Clear this index, and increment index pointer to the next position
  dhcpflow_table.index_free[dhcpflow_table.index_free_ptr++] = -1;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d, free_ptr=%u\n", __FUNCTION__, __LINE__,
           evcId,
           dhcpflow_table.flow_index[evcId],
           dhcpflow_table.index_free_ptr);

  // CLEAR LIST OF CLIENTS
  if (ptin_dhcp_evcClient_clear(evcId)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Cannot init list of clients of EVC %u\n", __FUNCTION__, __LINE__,evcId);
    ptin_dhcp_flow_remove(evcId);
    return L7_FAILURE;
  }

  // Clear EVC port statistics
  memset(L7_DHCP_Statistics_Intf[flow_index],0x00,sizeof(L7_DHCP_Statistics_t)*L7_SYSTEM_N_INTERF);

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > ptin_evcClient_init(evcId=%u) executed\n", __FUNCTION__, __LINE__,evcId);

  return L7_SUCCESS;
}

L7_RC_t ptin_dhcp_flow_remove(L7_uint16 evcId)
{
  // If EVC id is null, clear all database
  if (evcId==(L7_uint16)-1) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ info  ] %s(%d) > All clients of all EVC's will be removed\n", __FUNCTION__, __LINE__);
    (void)ptin_dhcp_flow_init();
    return L7_SUCCESS;
  }

  if (evcId>=FLOWS_MAX) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Check if this flow does not exist
  // If so, there is nothing to do. Return success
  if ( dhcpflow_table.flow_index[evcId] == -1 ) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > There is no dhcpflow associated to this evcId=%u\n", __FUNCTION__, __LINE__,evcId);
    return L7_SUCCESS;
  }

  // Check if there is any flow
  if ( dhcpflow_table.index_free_ptr <= 0 ) {
    dhcpflow_table.index_free_ptr = 0;
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > There is no MC flows\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Decrement the pointer of free indexes, and assign the index of the given vlan as free
  dhcpflow_table.index_free[--(dhcpflow_table.index_free_ptr)] = dhcpflow_table.flow_index[evcId];
  // Clear index of the given vlan
  dhcpflow_table.flow_index[evcId] = -1;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > evcId=%u => free_ptr=%u\n", __FUNCTION__, __LINE__,
           evcId,
           dhcpflow_table.index_free_ptr);

  return L7_SUCCESS;
}

static L7_RC_t ptin_dhcp_stat_get_indexes(L7_uint32 intf, L7_uint16 vlan, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index, L7_int *port_index)
{
  L7_int port;

  // GET PORT INDEX

  // Get port information
  if (ptin_translate_intf2port(intf,&port)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error getting port information from intf %u\n", __FUNCTION__, __LINE__,intf);
    return L7_FAILURE;
  }
  // Validate port value
  if (port<0 || port>=L7_SYSTEM_N_INTERF) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Port is invalid (port=%u)\n", __FUNCTION__, __LINE__,port);
    return L7_FAILURE;
  }

  if (port_index!=L7_NULLPTR) *port_index = port;

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > intf=%u => port=%d\n", __FUNCTION__, __LINE__,intf,port);

  // GET MCFLOW AND CLIENT INDEXES

  // Nothing to do
  if (flow_index==L7_NULLPTR && client_index==L7_NULLPTR) {
    return L7_SUCCESS;
  }

  if (ptin_dhcp_client_getIndex(port, vlan, cvlan, flow_index, client_index)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Flow and/or client indexes are invalid (port=%u,vlan=%u,cvlan=%u)\n", __FUNCTION__, __LINE__,port,vlan,cvlan);
    return L7_FAILURE;
  }

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > port=%u,vlan=%u,cvlan=%u => flow_index=%d,client_index=%d\n", __FUNCTION__, __LINE__,
           port,vlan,cvlan,((flow_index!=L7_NULLPTR) ? *flow_index : -1),((client_index!=L7_NULLPTR) ? *client_index : -1));

  return L7_SUCCESS;
}


static L7_RC_t ptin_dhcp_client_getIndex(L7_uint16 port, L7_uint16 vlan, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index)
{
  L7_uint16 evcId;

  if (ptin_flow_getId(port,vlan,&evcId)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error getting EVC id from port %u and vlan %u\n", __FUNCTION__, __LINE__,port,vlan);
    return L7_FAILURE;
  }
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > port=%u,vlan=%u => evcId=%u\n", __FUNCTION__, __LINE__,port,vlan,evcId);

  if (ptin_dhcp_evcClient_getIndex(evcId, cvlan, flow_index, client_index)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Error with ptin_dhcp_evcClient_getIndex (evcId=%u, cvlan=%u)\n", __FUNCTION__, __LINE__,evcId,cvlan);
    return L7_FAILURE;
  }

  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > ptin_dhcp_client_getIndex executed\n", __FUNCTION__, __LINE__);

  return L7_SUCCESS;
}

static L7_RC_t ptin_dhcp_evcClient_getIndex(L7_uint16 evcId, L7_uint16 cvlan, L7_int *flow_index, L7_int *client_index)
{
  st_client_index_table *dhcpflow;
  L7_int  dhcpflowIndex;

  // Validate EVC id
  if (evcId>=FLOWS_MAX) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Get dhcpflow index
  if (ptin_dhcp_flow_getIndex(evcId, &dhcpflowIndex)!=L7_SUCCESS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid EVC (%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // dhcpflow index found
  if (flow_index!=L7_NULLPTR)  *flow_index = dhcpflowIndex;
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [ info  ] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,dhcpflowIndex);

  // Validate CVlan
  if (cvlan>4095) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (cvlan=%u)\n", __FUNCTION__, __LINE__,cvlan);
    return L7_FAILURE;
  }

  dhcpflow = &dhcp_client_table[dhcpflowIndex];

  // Check if this vlan is an added client, if not, exit with error
  if (dhcpflow->client_index[cvlan]<0 || dhcpflow->client_index[cvlan]>=SNOOP_MAX_NUMBER_OF_CLIENTS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid client index (client_index=%d, cvlan=%u, flow_index=%d\n", __FUNCTION__, __LINE__,
             dhcpflow->client_index[cvlan],cvlan,dhcpflowIndex);
    return L7_FAILURE;
  }

  // client index found
  if (client_index!=L7_NULLPTR)  *client_index = dhcpflow->client_index[cvlan];
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > evcId=%u,cvlan=%u => flow_index=%d,client_index=%d\n", __FUNCTION__, __LINE__,evcId,cvlan,dhcpflowIndex,dhcpflow->client_index[cvlan]);

  return L7_SUCCESS;
}

static L7_RC_t ptin_dhcp_flow_getIndex(L7_uint16 evcId, L7_int *flow_index)
{
  if (evcId>=FLOWS_MAX) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid arguments (evcId=%u)\n", __FUNCTION__, __LINE__,evcId);
    return L7_FAILURE;
  }

  // Check if this vlan is an added client, if not, exit with error
  if (dhcpflow_table.flow_index[evcId]<0 || dhcpflow_table.flow_index[evcId]>=DHCP_MAX_NUMBER_OF_FLOWS) {
    if (dhcp_stat_debug)
      printf("{dhcp_stat_debug} [ ERROR ] %s(%d) > Invalid flow_index (flow_index=%d) for evcId=%u\n", __FUNCTION__, __LINE__,dhcpflow_table.flow_index[evcId],evcId);
    return L7_FAILURE;
  }

  // dhcpflow index found
  if (flow_index!=L7_NULLPTR)  *flow_index = dhcpflow_table.flow_index[evcId];
  if (dhcp_stat_debug)
    printf("{dhcp_stat_debug} [Success] %s(%d) > evcId=%u => flow_index=%d\n", __FUNCTION__, __LINE__,evcId,dhcpflow_table.flow_index[evcId]);

  return L7_SUCCESS;
}

#ifdef IGMP_WHITELIST_FEATURE

/**
 * Add a new entry in white list. 
 * 
 * @param channel_group   : Group channel
 * @param channel_grpMask : Number of masked bits
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_wlist_channel_add(L7_uint16 evc_idx, L7_inet_addr_t *channel_group , L7_uint16 channel_grpMask )
{
  L7_inet_addr_t  group;
  L7_int32        i, n_groups=1;
  ptinIgmpWListInfoData_t avl_node;
  L7_RC_t rc;
  L7_BOOL inUse;

  /* Validate EVC index */
  if (evc_idx >= FLOWS_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u not valid", evc_idx);
    return L7_FAILURE;
  }
  if (ptin_flow_inUse(evc_idx, &inUse)!=L7_SUCCESS || !(inUse))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u not active", evc_idx);
    return L7_FAILURE;
  }

  /* Validate and prepare channel group Address*/
  if (igmp_wlist_channelIP_prepare( channel_group, channel_grpMask, &group, &n_groups)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error preparing groupAddr");
    //return L7_FAILURE;
    return L7_SUCCESS;
  }
  /* Validate output ip address */
  if ( n_groups == 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Group address is not valid!");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to add group 0x%08x (%u addresses)", group.addr.ipv4.s_addr, n_groups);

  /* Validate number of channels */
  if ( (n_groups > IGMP_WHITELIST_CHANNELS_MAX) ||
       (igmpWListDB.number_of_entries + n_groups) > IGMP_WHITELIST_CHANNELS_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot add more than %u channels (already present: %u)", IGMP_WHITELIST_CHANNELS_MAX, igmpWListDB.number_of_entries);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Maximum addresses to be added: %u", n_groups);

  memset( &avl_node, 0x00, sizeof(ptinIgmpWListInfoData_t));

  /* Add channels */
  i = 0;
  rc = L7_SUCCESS;

  /* Run all group addresses */
  while ((rc==L7_SUCCESS && i<n_groups) || (rc!=L7_SUCCESS && i>=0))
  {
    avl_node.igmpWListDataKey.evc_id = evc_idx;
    memcpy(&avl_node.igmpWListDataKey.channel_group, &group, sizeof(L7_inet_addr_t));

    /* In case of success, continue adding nodes into avl tree */
    if (rc == L7_SUCCESS)
    {
      if (igmp_wlist_avlTree_insert( &avl_node ) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error inserting group channel 0x%08x",
                group.addr.ipv4.s_addr);
        rc = L7_FAILURE;
      }
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Added group 0x%08x", group.addr.ipv4.s_addr);
      }
    }
    /* If one error ocurred, remove previously added nodes */
    else
    {
      if (igmp_wlist_avlTree_remove( &avl_node.igmpWListDataKey ) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group channel 0x%08x",group.addr.ipv4.s_addr);
      }
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed group 0x%08x", group.addr.ipv4.s_addr);
      }
    }

    /* Next group address */
    if (group.family != L7_AF_INET6)
    {
      if (rc==L7_SUCCESS)
      {
        group.addr.ipv4.s_addr++;   i++;
      }
      else
      {
        group.addr.ipv4.s_addr--;   i--;
      }
    }
    else
      break;
  }

  return rc;
}

/**
 * Remove an entry from the white list. 
 * 
 * @param channel_group   : Group channel
 * @param channel_grpMask : Number of masked bits
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t igmp_wlist_channel_remove(L7_uint16 evc_idx, L7_inet_addr_t *channel_group, L7_uint16 channel_grpMask )
{
  L7_inet_addr_t  group;
  L7_uint32       i, n_groups=1;
  ptinIgmpWListDataKey_t avl_key;
  L7_RC_t rc;
  L7_BOOL inUse;

  /* Validate EVC index */
  if (evc_idx >= FLOWS_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u not valid", evc_idx);
    return L7_FAILURE;
  }
  if (ptin_flow_inUse(evc_idx, &inUse)!=L7_SUCCESS || !(inUse))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u not active", evc_idx);
    return L7_FAILURE;
  }

  /* Validate and prepare channel group Address*/
  if (igmp_wlist_channelIP_prepare( channel_group, channel_grpMask, &group, &n_groups)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error preparing groupAddr");
    //return L7_FAILURE;
    return L7_SUCCESS;
  }
  /* Validate output ip address */
  if ( n_groups == 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Group address is not valid!");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to remove group 0x%08x (%u addresses)", group.addr.ipv4.s_addr, n_groups);

  /* Validate number of channels */
  if ( n_groups > IGMP_WHITELIST_CHANNELS_MAX )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot add more than %u channels", IGMP_WHITELIST_CHANNELS_MAX);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Maximum addresses to be removed: %u", n_groups);

  rc = L7_SUCCESS;

  memset( &avl_key, 0x00, sizeof(ptinIgmpWListDataKey_t));

  /* Remove channels */
  for (i=0; i<n_groups; i++)
  {
    avl_key.evc_id = evc_idx;
    memcpy(&avl_key.channel_group, &group, sizeof(L7_inet_addr_t));

    /* Add node into avl tree */
    if (igmp_wlist_avlTree_remove( &avl_key ) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group channel 0x%08x (evc %u)", group.addr.ipv4.s_addr,evc_idx);
      rc = L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removed group 0x%08x (evc %u)", group.addr.ipv4.s_addr, evc_idx);

    /* Next group address */
    if (group.family != L7_AF_INET6)
      group.addr.ipv4.s_addr++;
    else
      break;
  }

  return rc;
}


/**
 * Check if a groupAddress exists in White List.
 *  
 * @param intIfNum      : Source interface 
 * @param vlanId        : Multicast vlan
 * @param channel_group : Group address
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_BOOL igmp_wlist_channel_exists( L7_uint32 intIfNum, L7_uint16 vlanId, L7_inet_addr_t *channel_group )
{
  L7_int port;
  L7_uint16 evc_idx;
  ptinIgmpWListDataKey_t  avl_key;
  ptinIgmpWListInfoData_t *avl_infoData;
  L7_inet_addr_t group_address;

  /* Get EVC using port and vlan */
  if (ptin_translate_intf2port(intIfNum, &port)!=L7_SUCCESS)
  {
    if (igmp_stat_debug)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid intIfNum %u",intIfNum);
    return L7_FALSE;
  }
  if (ptin_flow_getId(port, vlanId, &evc_idx)!=L7_SUCCESS)
  {
    if (igmp_stat_debug)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error getting evc from port %u, vlan%u",port,vlanId);
    return L7_FALSE;
  }

  /* Validate group address */
  if (igmp_wlist_channelIP_prepare(channel_group, 32, &group_address, L7_NULLPTR)!=L7_SUCCESS)
  {
    if (igmp_stat_debug)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid group address");
    return L7_FALSE;
  }  

  if (igmp_stat_debug)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Channel_group=0x%08x, group_address=0x%08x!",
              channel_group->addr.ipv4.s_addr, group_address.addr.ipv4.s_addr);
  

  /* Prepare key */
  memset( &avl_key, 0x00, sizeof(ptinIgmpWListDataKey_t) );
  avl_key.evc_id = evc_idx;
  memcpy(&avl_key.channel_group, &group_address, sizeof(L7_inet_addr_t));

  /* Check if this key does not exist */
  if ((avl_infoData=(ptinIgmpWListInfoData_t *) avlSearchLVL7( &(igmpWListDB.igmpWListAvlTree), (void *)&avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
    if (igmp_stat_debug)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"EVC %u, Group channel 0x%08x does not exist!",evc_idx, group_address.addr.ipv4.s_addr);
    return L7_FALSE;
  }
  
  return L7_TRUE;
}

/**
 * Prepare an ip address to be used for the AVL trees
 * 
 * @param channel_in  : ip address (in) 
 * @param channel_mask : number of bits to be masked (in)
 * @param channel_out : ip address to be returned (out)
 * @param number_of_channels: number of used channels (out)
 * 
 * @return L7_RC_t 
 */
static L7_RC_t igmp_wlist_channelIP_prepare( L7_inet_addr_t *channel_in, L7_uint16 channel_mask,
                                             L7_inet_addr_t *channel_out, L7_uint32 *number_of_channels)
{
  L7_uint16 mask_inv;

  /* Initialize output variables */
  if ( channel_out != L7_NULLPTR )
    memset(channel_out, 0x00, sizeof(L7_inet_addr_t));
  if ( number_of_channels != L7_NULLPTR )
    *number_of_channels = 0;

  /* Validate channel IP */
  if ( channel_in == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null pointer of provided address");
    return L7_FAILURE;
  }

  /* Only IPv4 is supported */
  if ( channel_in->family == L7_AF_INET6 )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Not supported IPv6");
    return L7_FAILURE;
  }

  /* Invalid IP value */
  if ( channel_in->addr.ipv4.s_addr == 0 || channel_in->addr.ipv4.s_addr == 0xffffffff )
  {
    if (igmp_stat_debug)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Not valid address: 0x%08x",channel_in->addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* Limit number of bits to be used */
  if ( channel_mask > sizeof(L7_uint32)*8 )  channel_mask = sizeof(L7_uint32)*8;

  mask_inv = sizeof(L7_uint32)*8 - channel_mask;

  /* Channel IP to be returned */
  if ( channel_out != L7_NULLPTR )
  {
    /* IP Address */
    channel_out->family = L7_AF_INET;
    channel_out->addr.ipv4.s_addr = channel_in->addr.ipv4.s_addr;

    /* Clear bits not covered by the mask */
    channel_out->addr.ipv4.s_addr >>= mask_inv;
    channel_out->addr.ipv4.s_addr <<= mask_inv;
  }

  /* Number of channels */
  if ( number_of_channels != L7_NULLPTR )
  {
    L7_uint16 i, n;

    n = 1;
    for (i=0; i<mask_inv; i++)
    {
      n *= 2;
      if (n > IGMP_WHITELIST_CHANNELS_MAX)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Mask too small (%u bits)",channel_mask);
        return L7_FAILURE;
      }
    }
    *number_of_channels = n;
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Bit mask = %u, number of = %u", channel_mask, n);
  }

  return L7_SUCCESS;
}

/**
 * Insert a node to the IGMP white list tree
 * 
 * @param node : node information to be added
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t igmp_wlist_avlTree_insert( ptinIgmpWListInfoData_t *node )
{
  ptinIgmpWListDataKey_t  avl_key;
  ptinIgmpWListInfoData_t *avl_infoData;

  /* Check if there is enough room for one more channels */
  if (igmpWListDB.number_of_entries >= IGMP_WHITELIST_CHANNELS_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"No more free entries!");
    return L7_FAILURE;
  }

  /* Prepare key */
  memset(&avl_key, 0x00, sizeof(ptinIgmpWListDataKey_t));
  avl_key.evc_id = node->igmpWListDataKey.evc_id;
  memcpy(&avl_key.channel_group, &node->igmpWListDataKey.channel_group, sizeof(L7_inet_addr_t));

  /* Check if this key already exists */
  if ((avl_infoData=(ptinIgmpWListInfoData_t *) avlSearchLVL7( &(igmpWListDB.igmpWListAvlTree), (void *)&avl_key, AVL_EXACT)) != L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Group channel 0x%08x already exists",
                node->igmpWListDataKey.channel_group.addr.ipv4.s_addr);
    return L7_SUCCESS;
  }

  /* Add key */
  if (avlInsertEntry(&(igmpWListDB.igmpWListAvlTree), (void *)&avl_key) != L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error inserting group channel 0x%08x",
            node->igmpWListDataKey.channel_group.addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* Search for inserted key */
  if ((avl_infoData=(ptinIgmpWListInfoData_t *) avlSearchLVL7(&(igmpWListDB.igmpWListAvlTree),(void *)&avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Group channel 0x%08x was added, but does not exist",
            node->igmpWListDataKey.channel_group.addr.ipv4.s_addr);

    return L7_FAILURE;
  }

  /* One more entry */
  igmpWListDB.number_of_entries++;

  return L7_SUCCESS;
}

/**
 * Remove a node to the IGMP white list tree
 * 
 * @param key : key information to be removed
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t igmp_wlist_avlTree_remove( ptinIgmpWListDataKey_t *avl_key )
{
  ptinIgmpWListInfoData_t *avl_infoData;
  L7_uint16 i;

  /* Check if this key does not exists */
  if ((avl_infoData=(ptinIgmpWListInfoData_t *) avlSearchLVL7( &(igmpWListDB.igmpWListAvlTree), (void *) avl_key, AVL_EXACT)) == L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"EVC %u, Group channel 0x%08x does not exist",
                avl_key->evc_id, avl_key->channel_group.addr.ipv4.s_addr);
    return L7_SUCCESS;
  }

  printf("Printing provided key:");
  for (i=0; i<sizeof(ptinIgmpWListDataKey_t); i++)
  {
    if (i%16==0)  printf("\r\n0x%04x:",i);
    printf(" %02x",*(((L7_uchar8 *) avl_key)+i) );
  }
  printf("\r\ndone!\r\n");

  /* Remove key */
  if (avlDeleteEntry(&(igmpWListDB.igmpWListAvlTree), (void *) avl_key) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group channel 0x%08x (EVC %u)",
            avl_key->channel_group.addr.ipv4.s_addr, avl_key->evc_id);
    return L7_FAILURE;
  }

  /* One less entry */
  if (igmpWListDB.number_of_entries>0)
    igmpWListDB.number_of_entries--;

  return L7_SUCCESS;
}

#if 0
/**
 * Remove all nodes from the IGMP white list tree. 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE;
 */
static L7_RC_t igmp_wlist_avlTree_purge ( void )
{
  /* Purge all AVL tree, but the root node */
  avlPurgeAvlTree( &igmpWListDB.igmpWListAvlTree, IGMP_WHITELIST_CHANNELS_MAX );

  /* No entries */
  igmpWListDB.number_of_entries = 0;

  return L7_SUCCESS;
}
#endif

/**
 * Dumps all IGMP associations 
 */
void ptin_igmp_wlist_dump(void)
{
  ptinIgmpWListDataKey_t avl_key;
  ptinIgmpWListInfoData_t *avl_info;
  L7_uint16 n_entries;

  /* Run all cells in AVL tree */
  memset(&avl_key,0x00,sizeof(ptinIgmpWListDataKey_t));

  n_entries = 0;

  while ( ( avl_info = (ptinIgmpWListInfoData_t *)
                        avlSearchLVL7(&igmpWListDB.igmpWListAvlTree, (void *)&avl_key, AVL_NEXT)
          ) != L7_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->igmpWListDataKey, sizeof(ptinIgmpWListDataKey_t));

    printf("EVC %u: groupAddr=%03u.%03u.%03u.%03u\r\n",
           avl_info->igmpWListDataKey.evc_id,
           (avl_info->igmpWListDataKey.channel_group.addr.ipv4.s_addr>>24) & 0xff,
            (avl_info->igmpWListDataKey.channel_group.addr.ipv4.s_addr>>16) & 0xff,
             (avl_info->igmpWListDataKey.channel_group.addr.ipv4.s_addr>>8) & 0xff,
              avl_info->igmpWListDataKey.channel_group.addr.ipv4.s_addr & 0xff );
    n_entries++;
  }

  printf("Done! %u entries displayed.\r\n",n_entries);
}
#endif


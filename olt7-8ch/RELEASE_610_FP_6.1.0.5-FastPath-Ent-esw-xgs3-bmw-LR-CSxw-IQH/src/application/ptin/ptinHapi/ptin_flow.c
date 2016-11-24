#include <unistd.h>
#include "ptin_flow.h"
#include "ptinHapi_cnfgr.h"
#include "ptin_snoop_stats.h"
#include "ptin_dhcp_database.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_dhcp_snooping.h"
#include "dhcp_snooping_api.h"
#include "snooping_api.h"
#include "ptin_msg.h"
#include "fdb_api.h"
#include "filter_api.h"
#include "traces.h"
#include "IPC.h"
#include "usmdb_nim_api.h"
#include "usmdb_filter_api.h"

#define L7_SYSTEM_FIRST_ROOT_PORT L7_SYSTEM_PON_PORTS

typedef struct {
  L7_uint32  lag_id;
  L7_uint16  ceVid;
} L7_EVCLagEntry;

typedef struct {
  L7_int     id;
  L7_uint16  cvid;
} L7_fpentry;

// EVC database structure
typedef struct {
  L7_uint8       inUse;
  L7_uint8       dhcp_enable;
  L7_uint8       mc_flood;
  L7_uint8       type;
  L7_HwEthernetMef10CeVidMap port_original[L7_SYSTEM_N_INTERF];
  L7_HwEthernetMef10CeVidMap port[L7_SYSTEM_N_INTERF];
  L7_uint16      vid_xlate[L7_SYSTEM_N_INTERF];
  L7_uint16      vid_root;
  L7_uint8       number_of_roots, number_of_leafs;
  L7_uint32      lag_pbmp;
  L7_fpentry_data *fpentry_ptr[SNOOP_MAX_NUMBER_OF_CLIENTS];
} L7_flow_struct;

L7_flow_struct flow_database[FLOWS_MAX];

L7_uint8 vlan_id_usage[4096];

L7_int EVCid_table[4096][L7_SYSTEM_N_INTERF];
L7_int EVCid_table_internalVlans[4096][L7_SYSTEM_N_INTERF];

#define FLOW_COMPARE_DIFFERENT          0
#define FLOW_COMPARE_PORT_STAG_MISMATCH 1
#define FLOW_COMPARE_EQUAL              2

static L7_int flow_compare(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port);
static L7_RC_t flow_remove_translation_entries(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port);
static L7_RC_t flow_add_translation_entries(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port);

static L7_RC_t ptin_flow_validate2(L7_HwEthernetMef10CeVidMap *uni);
static L7_RC_t ptin_flow_build(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port);
static L7_RC_t ptin_flow_unbuild(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port);

static L7_RC_t ptin_fpentry_read(L7_bw_profile *profile, L7_fpentry_data *fpentry_ptr);
static L7_RC_t ptin_fpentry_create(L7_bw_profile *profile, L7_fpentry_data **fpentry_ptr);
static L7_RC_t ptin_fpentry_destroy(L7_fpentry_data *fpentry_ptr);

static L7_RC_t ptin_flow_activate_snooping(L7_int port, L7_uint16 flow_id, L7_uint16 vlan_root);
static L7_RC_t ptin_flow_deactivate_snooping(L7_int port, L7_uint16 flow_id);

static L7_BOOL ptin_flow_validate(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port);
static L7_BOOL ptin_flow_create_is_possible(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port);


L7_int igmp_firstRootPort = L7_SYSTEM_FIRST_ROOT_PORT;

L7_RC_t hapiPtinDebug_IgmpFirstRootPort(L7_int port)
{
  if (port<0 || port>=L7_SYSTEM_N_INTERF)
    igmp_firstRootPort = L7_SYSTEM_FIRST_ROOT_PORT;
  else
    igmp_firstRootPort = port;

  return L7_SUCCESS;
}

static L7_int32 l2_dynamic_address_learned = 0;
static L7_int32 l2_dynamic_address_deleted = 0;
static L7_int32 l2_dynamic_delta = 0;

static L7_int32 l2_static_address_learned = 0;
static L7_int32 l2_static_address_deleted = 0;
static L7_int32 l2_static_delta = 0;

void hapiPtinDebug_l2_clear_counters(void)
{
  osapiSemaTake(ptin_module_l2_sem,L7_WAIT_FOREVER);

  l2_dynamic_address_learned = 0;
  l2_dynamic_address_deleted = 0;
  l2_dynamic_delta = 0;

  l2_static_address_learned = 0;
  l2_static_address_deleted = 0;
  l2_static_delta = 0;

  osapiSemaGive(ptin_module_l2_sem);
}

void hapiPtinDebug_l2_show_counters(void)
{
  osapiSemaTake(ptin_module_l2_sem,L7_WAIT_FOREVER);

  printf("l2_dynamic_address_learned=%d\r\n",l2_dynamic_address_learned);
  printf("l2_dynamic_address_deleted=%d\r\n",l2_dynamic_address_deleted);
  printf("l2_dynamic_delta=%d\r\n",l2_dynamic_delta);

  printf("l2_static_address_learned=%d\r\n",l2_static_address_learned);
  printf("l2_static_address_deleted=%d\r\n",l2_static_address_deleted);
  printf("l2_static_delta=%d\r\n",l2_static_delta);

  osapiSemaGive(ptin_module_l2_sem);
}



/*
 * Initialization functions 
 *  
 */

void ptin_flow_init(void)
{
  // Clear database
  memset(flow_database,0,sizeof(L7_flow_struct)*FLOWS_MAX);
  memset(vlan_id_usage,0,sizeof(L7_uint8)*4096);
  memset(EVCid_table,0xFF,sizeof(L7_int)*4096*L7_SYSTEM_N_INTERF);
  memset(EVCid_table_internalVlans,0xFF,sizeof(L7_int)*4096*L7_SYSTEM_N_INTERF);
  vlan_id_usage[0]=1;
  vlan_id_usage[1]=1;
  vlan_id_usage[4095]=1;

  // Initialize DHCP database
  ptin_dhcp_database_init();

  // Initialize translations
  ptin_vlan_translate_init();
}

L7_RC_t ptin_flow_getVlan(L7_uint8 port, L7_uint16 flowId, L7_uint16 *vlanId)
{
  L7_uint16 vlan;

  // Validate arguments
  if (port>=L7_SYSTEM_N_INTERF || flowId>=FLOWS_MAX) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments (port=%u, flowId=%u)\n", __FUNCTION__, __LINE__,port,flowId);
    return L7_FAILURE;
  }
  // Validate EVC: is should exist
  if (!flow_database[flowId].inUse) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC %u does not exist\n", __FUNCTION__, __LINE__,flowId);
    return L7_FAILURE;
  }
  // Check if port is included in EVC
  if (flow_database[flowId].port[port].type==MEF10_EVC_UNITYPE_UNUSED) {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Port %u in EVC %u does not exist\n", __FUNCTION__, __LINE__,port,flowId);
    return L7_FAILURE;
  }
  // Extract vlan
  vlan = (L7_uint16) flow_database[flowId].port[port].ceVid;

  // Validate vlan
  if (vlan<=L7_DOT1Q_MIN_VLAN_ID || vlan>L7_DOT1Q_MAX_VLAN_ID) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Vlan associated to Port %u in EVC %u is invalid (%u)\n", __FUNCTION__, __LINE__,port,flowId,vlan);
    return L7_FAILURE;
  }
  // Return vlan
  if (vlanId!=L7_NULLPTR)  *vlanId = vlan;
  return L7_SUCCESS;
}

L7_RC_t ptin_flow_get_originalVlan(L7_uint8 port, L7_uint16 flowId, L7_uint16 *vlanId)
{
  L7_uint16 vlan;

  // Validate arguments
  if (port>=L7_SYSTEM_N_INTERF || flowId>=FLOWS_MAX) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments (port=%u, flowId=%u)\n", __FUNCTION__, __LINE__,port,flowId);
    return L7_FAILURE;
  }
  // Validate EVC: is should exist
  if (!flow_database[flowId].inUse) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC %u does not exist\n", __FUNCTION__, __LINE__,flowId);
    return L7_FAILURE;
  }
  // Check if port is included in EVC
  if (flow_database[flowId].port[port].type==MEF10_EVC_UNITYPE_UNUSED) {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Port %u in EVC %u does not exist\n", __FUNCTION__, __LINE__,port,flowId);
    return L7_FAILURE;
  }
  // Extract vlan
  vlan = (L7_uint16) flow_database[flowId].port_original[port].ceVid;

  // Validate vlan
  if (vlan<=L7_DOT1Q_MIN_VLAN_ID || vlan>L7_DOT1Q_MAX_VLAN_ID) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Vlan associated to Port %u in EVC %u is invalid (%u)\n", __FUNCTION__, __LINE__,port,flowId,vlan);
    return L7_FAILURE;
  }
  // Return vlan
  if (vlanId!=L7_NULLPTR)  *vlanId = vlan;
  return L7_SUCCESS;
}

L7_RC_t ptin_flow_get_internalVlan(L7_uint8 port, L7_uint16 flowId, L7_uint16 *vlanId)
{
  L7_uint16 vlan;

  // Validate arguments
  if (port>=L7_SYSTEM_N_INTERF || flowId>=FLOWS_MAX) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments (port=%u, flowId=%u)\n", __FUNCTION__, __LINE__,port,flowId);
    return L7_FAILURE;
  }
  // Validate EVC: is should exist
  if (!flow_database[flowId].inUse) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC %u does not exist\n", __FUNCTION__, __LINE__,flowId);
    return L7_FAILURE;
  }
  // Check if port is included in EVC
  if (flow_database[flowId].port[port].type==MEF10_EVC_UNITYPE_UNUSED) {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Port %u in EVC %u does not exist\n", __FUNCTION__, __LINE__,port,flowId);
    return L7_FAILURE;
  }
  // Extract vlan
  vlan = (L7_uint16) flow_database[flowId].vid_xlate[port];

  // Validate vlan
  if (vlan<=L7_DOT1Q_MIN_VLAN_ID || vlan>L7_DOT1Q_MAX_VLAN_ID) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Vlan associated to Port %u in EVC %u is invalid (%u)\n", __FUNCTION__, __LINE__,port,flowId,vlan);
    return L7_FAILURE;
  }
  // Return vlan
  if (vlanId!=L7_NULLPTR)  *vlanId = vlan;
  return L7_SUCCESS;
}

/*
 * EVC flows management functions
 *  
 */

L7_RC_t ptin_flow_getId(L7_uint8 port, L7_uint16 vlan, L7_uint16 *flow_id)
{
  L7_int flowId;

  if (port>=L7_SYSTEM_N_INTERF || vlan<=L7_DOT1Q_MIN_VLAN_ID || vlan>L7_DOT1Q_MAX_VLAN_ID) {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments (port=%u, vlan=%u)\n", __FUNCTION__, __LINE__,port,vlan);
    return L7_FAILURE;
  }

  flowId = EVCid_table[vlan][port];

  if (flowId<0 || flowId>=FLOWS_MAX)  {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC id is not consistent (vlan=%u,port=%u => flowId=%u)\n", __FUNCTION__, __LINE__,vlan,port,flowId);
    return L7_FAILURE;
  }

  if (!flow_database[flowId].inUse) {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC %u does not exist\n", __FUNCTION__, __LINE__,flowId);
    return L7_FAILURE;
  }

  if (flow_id!=L7_NULLPTR)  *flow_id = (L7_uint16) flowId;
  return L7_SUCCESS;
}

L7_RC_t ptin_flow_getId_fromInternalVlan(L7_uint8 port, L7_uint16 vlan, L7_uint16 *flow_id)
{
  L7_int flowId;

  if (port>=L7_SYSTEM_N_INTERF || vlan<=L7_DOT1Q_MIN_VLAN_ID || vlan>L7_DOT1Q_MAX_VLAN_ID) {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments (port=%u, vlan=%u)\n", __FUNCTION__, __LINE__,port,vlan);
    return L7_FAILURE;
  }

  flowId = EVCid_table_internalVlans[vlan][port];

  if (flowId<0 || flowId>=FLOWS_MAX)  {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC id is not consistent (vlan=%u,port=%u => flowId=%u)\n", __FUNCTION__, __LINE__,vlan,port,flowId);
    return L7_FAILURE;
  }

  if (!flow_database[flowId].inUse) {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC %u does not exist\n", __FUNCTION__, __LINE__,flowId);
    return L7_FAILURE;
  }

  if (flow_id!=L7_NULLPTR)  *flow_id = (L7_uint16) flowId;
  return L7_SUCCESS;
}


L7_RC_t ptin_flow_inUse(L7_uint16 flow_id, L7_BOOL *inUse)
{
  // Validate flow_id
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if (inUse!=L7_NULLPTR)  *inUse = flow_database[flow_id].inUse;
  return L7_SUCCESS;
}

L7_RC_t ptin_flow_getType(L7_uint16 flow_id, L7_uint8 *type)
{
  // Validate flow_id
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if (type!=L7_NULLPTR)  *type = flow_database[flow_id].type;
  return L7_SUCCESS;
}


L7_RC_t ptin_flow_getPortVlanList(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *plist)
{
  L7_flow_struct *flow;

  // Validate flow_id
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  flow = &flow_database[flow_id];

  // EVC must exist
  if (!flow->inUse)  return L7_FAILURE;

  // Copy port list
  memcpy(plist, flow->port, sizeof(L7_HwEthernetMef10CeVidMap)*L7_SYSTEM_N_INTERF);

  return L7_SUCCESS;
}


L7_RC_t ptin_flow_read(L7_HwEthernetMef10EvcBundling *evcCfg)
{
  L7_uint16 flow_id;
  L7_uint8 i;
  L7_flow_struct *flow;

  flow_id = (L7_uint16) (evcCfg->index & 0xFFFF);

  // Validate flow_id
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  flow = &flow_database[flow_id];

  // Check if flow exists
  if (!flow->inUse)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: flow %u does not exist\n",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }

  // Physical ports
  memcpy(evcCfg->uni,flow->port,sizeof(L7_HwEthernetMef10CeVidMap)*L7_SYSTEM_N_PORTS);
  // Lag interface
  for (i=L7_SYSTEM_N_PORTS; i<L7_SYSTEM_N_INTERF; i++) {
    if (flow->port[i].type!=MEF10_EVC_UNITYPE_UNUSED) {
      evcCfg->index |= (L7_uint64) ((i-L7_SYSTEM_N_PORTS+1) & 0xFF)<<32;
      evcCfg->outerTag = flow->port[i].ceVid;
      break;
    }
  }

  // EVC type
  evcCfg->type = flow->type;

  // DHCP enable
  evcCfg->index |= ((L7_uint64) (flow->dhcp_enable & 1)<<56) | ((L7_uint64) (flow->mc_flood & 1)<<57);

  return L7_SUCCESS;
}


L7_RC_t ptin_flow_create(L7_HwEthernetMef10EvcBundling *evcCfg)
{
  L7_uint32 lag_intf/*, intf*/;
  L7_uint8 lag_id, dhcp_enable, mc_flood;
  L7_uint8 flow_compare_result;
  L7_uint16 p/*, vlanId*/;
  L7_uint8 number_of_roots, number_of_leafs;
  L7_HwEthernetMef10CeVidMap port[L7_SYSTEM_N_INTERF], source_ports[L7_SYSTEM_N_INTERF];
  L7_uint16 /*index,*/ flow_id, vlan_root=0;
  //L7_FILTER_VLAN_FILTER_MODE_t filter_mode=0xFF;
  L7_flow_struct *flow;
  L7_RC_t rc;

  // Validate given flow
//if (ptin_flow_validate(evcCfg)!=L7_SUCCESS)  {
//  return L7_FAILURE;
//}

  // Extract EVC ID
  flow_id = (L7_uint16) (evcCfg->index & 0xFFFF);
  // Extract LAG id, if used
  lag_id = (L7_uint8) ((evcCfg->index>>32) & 0xFF);
  // DHCP enable?
  dhcp_enable = (L7_uint8) ((evcCfg->index>>56) & 1);
  // MC flood type
  mc_flood    = (L7_uint8) ((evcCfg->index>>57) & 1);

  /* Force flags for some service types */
  if (evcCfg->type==FLOW_TYPE_BITSTREAM_WITH_MACLEARN)
  {
    dhcp_enable = L7_FALSE;
    mc_flood    = L7_TRUE;
  }

  // Validate flow_id and lag_id
  if (flow_id>=FLOWS_MAX || lag_id>=L7_SYSTEM_N_LAGS)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: invalid arguments (flow_id=%u, lag_id=%u)\n",__FUNCTION__,flow_id,lag_id);
    return L7_FAILURE;
  }

//// Check if flow exists
//if (flow_database[flow_id].inUse)  {
//  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: flow already created\n",__FUNCTION__);
//  return L7_FAILURE;
//}

  // List of port/vlan
  memcpy(port,evcCfg->uni,sizeof(L7_HwEthernetMef10CeVidMap)*L7_SYSTEM_N_PORTS);
  for (p=L7_SYSTEM_N_PORTS; p<L7_SYSTEM_N_INTERF; p++) {
    port[p].type  = MEF10_EVC_UNITYPE_UNUSED;
    port[p].ceVid = (L7_uint16)-1;
  }
  if (lag_id>0) {
    if (ptin_translate_port2intf(L7_SYSTEM_N_PORTS+lag_id-1,&lag_intf)!=L7_SUCCESS || lag_intf==0)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Lag %u does not exist!\n",__FUNCTION__,lag_id);
      return L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Lag %u is configured!\n",__FUNCTION__,lag_id);
    port[L7_SYSTEM_N_PORTS+lag_id-1].type  = MEF10_EVC_UNITYPE_ROOT;
    port[L7_SYSTEM_N_PORTS+lag_id-1].ceVid = evcCfg->outerTag;
  }

  // Validate given flow
  if (ptin_flow_validate2(port)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error converting physical ports to Lags\n",__FUNCTION__);
    return L7_FAILURE;
  }

  // Check arguments: at least two ports used
  for (p=0,number_of_roots=0,number_of_leafs=0; p<L7_SYSTEM_N_INTERF; p++)  {
    if (port[p].type!=(L7_uint8)-1 && port[p].ceVid<4096)  {
      if (port[p].type==MEF10_EVC_UNITYPE_ROOT)  number_of_roots++;
      else  number_of_leafs++;
    }
  }

  if (number_of_roots==0 || (number_of_roots+number_of_leafs)<2)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: invalid flow schematic\n",__FUNCTION__);
    return L7_FAILURE;
  }
  // If there is only 1 leaf port involved, all ports will be roots
  #if 0
  if (number_of_leafs==1)  {
    for (p=0; p<L7_SYSTEM_N_INTERF; p++)  {
      if (port[p].type==(L7_uint8)-1)  continue;
      port[p].type=MEF10_EVC_UNITYPE_ROOT;
    }
  }
  #endif

  // Save EVC data before adaptation if it is multicast
  memcpy(source_ports,port,sizeof(L7_HwEthernetMef10CeVidMap)*L7_SYSTEM_N_INTERF);

  if (evcCfg->type==FLOW_TYPE_MULTICAST)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: This is a MC flow (flow_id=%u)",__FUNCTION__,flow_id);
    // Save port list
    // Root vlan should be the sabe as the client vlans.
    // Search for the first root vlan
    for (p=igmp_firstRootPort; p<L7_SYSTEM_N_INTERF && port[p].type!=MEF10_EVC_UNITYPE_ROOT; p++);
    if (p>=L7_SYSTEM_N_INTERF)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: There are no root ports in this EVC (flow_id=%u)",__FUNCTION__,flow_id);
      return L7_FAILURE;
    }
    // This is the root vlan
    vlan_root = port[p].ceVid;
    // For all client interfaces, attrib the root vlan
    for (p=0; p<igmp_firstRootPort; p++) {
      if (port[p].type!=MEF10_EVC_UNITYPE_UNUSED) {
        port[p].type=MEF10_EVC_UNITYPE_ROOT;
        port[p].ceVid=vlan_root;
      }
    }
  }
  else  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Flow type=%u (flow_id=%u)",__FUNCTION__,evcCfg->type,flow_id);
  }

  flow = &flow_database[flow_id];

  // Validate flow
  if (flow->inUse && evcCfg->type==flow->type && dhcp_enable==flow->dhcp_enable)
  {
    flow_compare_result=flow_compare(flow_id, port);
    if (flow_compare_result==FLOW_COMPARE_PORT_STAG_MISMATCH)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: stag mismatch error\n",__FUNCTION__);
      return L7_FAILURE;
    }
    if (flow_compare_result==FLOW_COMPARE_EQUAL && mc_flood==flow->mc_flood)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: there is an equal flow\n",__FUNCTION__);
      return L7_SUCCESS;
    }
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Creating flow!\n",__FUNCTION__);
  }
  else
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Killing flow!\n",__FUNCTION__);
    // EVC type or DHCP change, will delete EVC
    if (ptin_flow_kill(flow_id)!=L7_SUCCESS)  {
      return L7_FAILURE;
    }
  }

  // Wait until dot1q and Snoop Queue is empty
  while (!dot1qQueueEmpty() || !snoopQueueEmpty())
    usleep(1000);

  /* Validate EVC data */
  if (!ptin_flow_validate(flow_id, port))
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: EVC %u not valid\n",__FUNCTION__, flow_id);
    return L7_FAILURE;
  }

  /* Check if there are enough resources */
  if (!ptin_flow_create_is_possible(flow_id, port))
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Not enough resources to (re)create flow\n",__FUNCTION__);
    return L7_TABLE_IS_FULL;
  }

  // Update flow
  flow->type = evcCfg->type;
  flow->dhcp_enable = dhcp_enable;
  flow->mc_flood    = mc_flood;
  flow->inUse = 1;   // TRUE

  // Different configurations at the level port, will result on its remotion
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Unbuilding flow!\n",__FUNCTION__);
  if (ptin_flow_unbuild(flow_id, port)!=L7_SUCCESS)  {
    return L7_FAILURE;
  }
  // Correct type and vlan of the client interfaces, in order to reflect the original values
  memcpy(flow->port_original,source_ports,sizeof(L7_HwEthernetMef10CeVidMap)*L7_SYSTEM_N_INTERF);

  // Add not configured ports
  if ((rc=ptin_flow_build(flow_id, port))!=L7_SUCCESS)  {
    ptin_flow_kill(flow_id);
    return rc;
  }

  /*
  // For multicast EVCs, correct PON vlans (internal) related to the original source vlans
  if (flow->type==FLOW_TYPE_UNICAST || flow->type==FLOW_TYPE_MULTICAST)  {
    for (index=1; index<FLOWS_MAX; index++) {
      // EVC should be in use, and UNICAST type
      if (index==flow_id || !flow_database[index].inUse ||
          (flow->type==FLOW_TYPE_MULTICAST && flow_database[index].type!=FLOW_TYPE_UNICAST) ||
          (flow->type==FLOW_TYPE_UNICAST && flow_database[index].type!=FLOW_TYPE_MULTICAST))
        continue;
      for (p=0; p<igmp_firstRootPort; p++) {
        // Port must be in use
        if (flow->port_original[p].type==MEF10_EVC_UNITYPE_UNUSED ||
            flow_database[index].port_original[p].type==MEF10_EVC_UNITYPE_UNUSED ||
            flow_database[index].port_original[p].ceVid!=flow->port_original[p].ceVid)
          continue;

        vlanId = (flow->type==FLOW_TYPE_MULTICAST) ? flow_database[index].port_original[p].ceVid : flow->port_original[p].ceVid;

        // Determine intIfNum
        if (ptin_translate_port2intf(p,&intf)!=L7_SUCCESS) intf = 0;

        if (vlanId>=2 && vlanId<=4093 &&
            usmDbSnoopAdminModeSet( 1, L7_TRUE, intf, vlanId, L7_AF_INET)!=L7_SUCCESS) {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopAdminModeSet for vlan=%u",__FUNCTION__,vlanId);
          return L7_FAILURE;
        }
      }
    }
  }
*/
  // Multicast services processing
  if (flow->type==FLOW_TYPE_MULTICAST)  {
    // Update static MC channels
    //snoopStaticIpRefreshAll();

    if (ptin_igmp_flow_add(flow_id)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error adding MC flow for stats (flow_id=%u)",__FUNCTION__,flow_id);
      ptin_flow_kill(flow_id);
      return L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: MC flow added successfully for stats (flow_id=%u)",__FUNCTION__,flow_id);
  }

  // DHCP statistics
  if (dhcp_enable)  {
    if (ptin_dhcp_flow_add(flow_id)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error adding flow for DHCP stats (flow_id=%u)",__FUNCTION__,flow_id);
      ptin_flow_kill(flow_id);
      return L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Flow added successfully for DHCP stats (flow_id=%u)",__FUNCTION__,flow_id);
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_flow_kill(L7_uint16 flow_id)
{
//L7_uint32 intIfNum;
//L7_uint16 vlanId;
//L7_uint16 index;
//L7_uint8 p;
  L7_uint16 c;
  L7_flow_struct *flow;
  st_ptin_flow_counters flow_counters;

  // Validate flow_id
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  if (!flow_database[flow_id].inUse)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: flow not used\n",__FUNCTION__);
    return L7_SUCCESS;
  }

  flow = &flow_database[flow_id];

/*
  // For multicast EVCs, correct PON vlans (internal) related to the original source vlans
  if (flow->type==FLOW_TYPE_UNICAST || flow->type==FLOW_TYPE_MULTICAST)
  {
    for (index=1; index<FLOWS_MAX; index++)
    {
      // EVC should be in use, and UNICAST type
      if (index==flow_id || !flow_database[index].inUse ||
          (flow->type==FLOW_TYPE_MULTICAST && flow_database[index].type!=FLOW_TYPE_UNICAST) ||
          (flow->type==FLOW_TYPE_UNICAST && flow_database[index].type!=FLOW_TYPE_MULTICAST))
        continue;
      for (p=0; p<igmp_firstRootPort; p++)
      {
        // Port must be in use
        if (flow->port_original[p].type==MEF10_EVC_UNITYPE_UNUSED ||
            flow_database[index].port_original[p].type==MEF10_EVC_UNITYPE_UNUSED ||
            flow_database[index].port_original[p].ceVid!=flow->port_original[p].ceVid)
          continue;

        // Determine intIfNum
        if (ptin_translate_port2intf(p,&intIfNum)!=L7_SUCCESS) intIfNum = 0;
        vlanId = (flow->type==FLOW_TYPE_MULTICAST) ? flow_database[index].port_original[p].ceVid : flow->port_original[p].ceVid;
        
        // Only remove this Vlan, if is not used in any other UC EVC!
        if (vlanId>=2 && vlanId<=4093 &&
            usmDbSnoopAdminModeSet( 1, L7_FALSE, intIfNum, vlanId, L7_AF_INET)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopAdminModeSet for vlan=%u",__FUNCTION__,vlanId);
          return L7_FAILURE;
        }
      }
    }
  }
*/

  ptin_flow_unbuild(flow_id, L7_NULLPTR);

  // Multicast services processing
  if (flow->type==FLOW_TYPE_MULTICAST)  {
    ptin_igmp_flow_remove(flow_id);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Flow removed from IGMP statistics (flow_id=%u)",__FUNCTION__,flow_id);
  }
  // DHCP statistics
  if (flow->dhcp_enable)  {
    ptin_dhcp_flow_remove(flow_id);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Flow removed from DHCP statistics (flow_id=%u)",__FUNCTION__,flow_id);
  }

  /* Only clear counters, for regular EVCs */
  if (flow_id!=0)
  {
    memset(&flow_counters, 0x00, sizeof(st_ptin_flow_counters));
    flow_counters.index = flow_id;
    flow_counters.flow_type = (L7_uint8)-1;
    flow_counters.c_id.client_vlan = 0;

    ptin_msg_removeFlowCounters( &flow_counters );
  }

  // Remove BWP's
  for (c=0; c<SNOOP_MAX_NUMBER_OF_CLIENTS; c++)  {
    // If fpentry is not defined, skip
    if (flow->fpentry_ptr[c]==L7_NULLPTR)  continue;

    if (ptin_fpentry_destroy(flow->fpentry_ptr[c]))  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with flow_destroy_fpentry\n",__FUNCTION__);
      //return L7_FAILURE;
    }
  }

  // Wait until dot1q and Snoop Queue is empty
  while (!dot1qQueueEmpty() || !snoopQueueEmpty())
    usleep(1000);

//for (p=0; p<L7_SYSTEM_N_INTERF; p++) {
//  if (flow->vid_stat[p]<=0)  continue;
//  if (ptin_fpentry_clean(&flow->vid_stat[p]))  {
//    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with ptin_fpentry_clean\n",__FUNCTION__);
//    //return L7_FAILURE;
//  }
//}

  // Clear database
  memset(flow,0,sizeof(L7_flow_struct));

  // Restore L2 learn operation
  //bcm_l2_addr_thaw(ptin_hapi_unit);
  
  return L7_SUCCESS;
}


L7_RC_t ptin_flow_reset(void)
{
  L7_uint16 flow_id;

  // Remove all flows
  for (flow_id=1; flow_id<FLOWS_MAX; flow_id++) {
    if (!flow_database[flow_id].inUse)  continue;
    // Remove flow
    ptin_flow_kill(flow_id);
  }

  /* Clear DHCP database */
  ptin_dhcp_database_init();

  return L7_SUCCESS;
}


L7_RC_t ptin_flow_set_profile(L7_uint16 flow_id, L7_uint8 port, L7_bw_profile *bw_profile)
{
  L7_uint8 i;
  L7_flow_struct *flow;
  L7_fpentry_data *fpentry_ptr, *fpentry_profile_ptr=L7_NULLPTR, *fpentry_redirect_ptr=L7_NULLPTR;
  L7_int client_index, client_index_free;
  L7_bw_profile  profile_case;
  L7_RC_t rc = L7_SUCCESS;

  // Validate arguments
  if (flow_id>=FLOWS_MAX || bw_profile==NULL)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Pointer for the EVC
  flow = &flow_database[flow_id];

  // Check if evc is defined
  if (!flow->inUse)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Flow is not defined\n",__FUNCTION__);
    return L7_FAILURE;
  }

  // For multicast services, only add the client for IGMP statistic purposes
  if (flow->type==FLOW_TYPE_MULTICAST) {
    // Add client for IGMP snooping
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: This is a MC flow (%d)",__FUNCTION__,flow_id);
    if (ptin_igmp_evcClient_add(flow_id,bw_profile->ctag.vid_value)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error adding Client (%d) for IGMP stats (flow %d)",__FUNCTION__,bw_profile->ctag.vid_value,flow_id);
      return L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Client (%d) added for IGMP stats (flow %d)",__FUNCTION__,bw_profile->ctag.vid_value,flow_id);
  }
  // Otherwise, define a bandwidth profile
  else
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Normal flow (%d)",__FUNCTION__,flow_id);

    // Correct bandwidth parameters
    bw_profile->port_bmp           = 0x3ff00;
    bw_profile->stag.vid_value     = flow->vid_root;
    bw_profile->stag.vid_mask      = (L7_uint16)-1;

    // Validate ctag info
    if ( (flow->type!=FLOW_TYPE_BITSTREAM) ||
         (bw_profile->ctag.vid_value<1 || bw_profile->ctag.vid_value>4095 || bw_profile->ctag.vid_mask==0)) {
      // port_output field will identify if packets should be redirected to a certain destination port, bypassing the L2 FWD block, as required in business services
      // Bitstream flows identifies business services!
      bw_profile->port_output = (L7_uint8)-1;
    }
    /*
    // For invalid cvid info: IT IS REQUIRED A VALID VALUE FOR THIS FIELD
    else {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid cVid: %u/%u\n",__FUNCTION__,__LINE__,bw_profile->ctag.vid_value,bw_profile->ctag.vid_mask);
      return L7_FAILURE;
      //bw_profile->ctag.vid_value = 0;
      //bw_profile->ctag.vid_mask  = 0;
    } 
    */ 
  
    // Can i add one more client?
  
    // At this point client_index, and fpentry_ptr have valid values

    // Profile and redirect rules definition
    for (i=0; i<2; i++)
    {
      // Save profile structure
      profile_case = *bw_profile;

      // bandwidth profile
      if (i==0)
      {
        // S-Vlan and CIR must be valid
        if (bw_profile->stag.vid_mask==0 /*|| bw_profile->cir==(L7_uint32)-1*/)
          continue;
        if (bw_profile->cir==(L7_uint32)-1)
          profile_case.port_output = (L7_uint8)-1;  // No redirection
      }
      // Redirect profile
      else if (i==1)
      {
        // C-Vlan and output port must be valid
        if (bw_profile->ctag.vid_mask==0 || bw_profile->port_output>=igmp_firstRootPort)
          continue;
        profile_case.stag.vid_value = 0;          // No Stag classification
        profile_case.stag.vid_mask  = 0;
        profile_case.cir = (L7_uint32)-1;         // No profile
      }
      else
      {
        continue;
      }

      // Search for an existent client for the profile
      for (client_index=0,client_index_free=-1; client_index<SNOOP_MAX_NUMBER_OF_CLIENTS; client_index++)  {
        fpentry_ptr = flow->fpentry_ptr[client_index];
        // Free entry... use this entry
        if (fpentry_ptr==L7_NULLPTR || !fpentry_ptr->inUse)  {
          if (client_index_free==-1)  client_index_free=client_index;
          continue;
        }
        // Entry is equal... redefine it
        if ((fpentry_ptr->stag.vid_value==profile_case.stag.vid_value && fpentry_ptr->stag.vid_mask==profile_case.stag.vid_mask) &&
            (fpentry_ptr->ctag.vid_value==profile_case.ctag.vid_value && fpentry_ptr->ctag.vid_mask==profile_case.ctag.vid_mask) )  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: client_id match (fpentry_id=%d)\n",__FUNCTION__,fpentry_ptr->fpentry_id);
          break;
        }
      }
      // No clients available
      if (client_index>=SNOOP_MAX_NUMBER_OF_CLIENTS)  {
        if (client_index_free==-1)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: No more clients to be added\n",__FUNCTION__);
          if (i>0)  ptin_fpentry_destroy(fpentry_profile_ptr);
          return L7_FAILURE;
        }
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: New client to be added (client_idx_free=%u)\n",__FUNCTION__,client_index_free);
        client_index = client_index_free;
        fpentry_ptr  = flow->fpentry_ptr[client_index_free];
      }

      // Save fpentry pointer
      switch (i)
      {
      case 0:
        fpentry_profile_ptr   = fpentry_ptr;
        break;
      default:
        fpentry_redirect_ptr  = fpentry_ptr;
        break;
      }

      // Create rule
      if ((rc=ptin_fpentry_create(&profile_case, &fpentry_ptr))!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with flow_create_fpentry\n",__FUNCTION__);
        if (i>0)  ptin_fpentry_destroy(fpentry_profile_ptr);
        return rc;
      }
      flow->fpentry_ptr[client_index] = fpentry_ptr;
    }
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_flow_remove_profile(L7_uint16 flow_id, L7_uint8 port, L7_bw_profile *bw_profile)
{
  L7_uint8 i;
  L7_flow_struct *flow;
  L7_fpentry_data *fpentry_ptr, *fpentry_redirect_ptr;
  L7_int client_index, client_index_redirect;
  L7_vid ctag;
  L7_uint32 intIfNum;
  L7_bw_profile profile_case;
  L7_RC_t rc;

  // Validate arguments
  if (flow_id>=FLOWS_MAX || bw_profile==NULL)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Pointer for the EVC
  flow = &flow_database[flow_id];

  // Check if evc is defined
  if (!flow->inUse)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Flow is not defined\n",__FUNCTION__);
    return L7_FAILURE;
  }

  // For multicast services, only remove the client for IGMP statistic purposes
  if (flow->type==FLOW_TYPE_MULTICAST) {
    // Add client for IGMP snooping
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: This is a MC flow (%d)",__FUNCTION__,flow_id);
    if (ptin_igmp_evcClient_remove(flow_id,bw_profile->ctag.vid_value)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error removing Client (%d) for IGMP stats (flow %d)",__FUNCTION__,bw_profile->ctag.vid_value,flow_id);
      return L7_FAILURE;
    }
    // Remove this client from AVL trees (MCvlan must be converted to UNIvlan)
    if (ptin_translate_port2intf(port,&intIfNum)!=L7_SUCCESS || intIfNum==0)  intIfNum=L7_ALL_INTERFACES;
    if (snoopClientRemove( snoop_mcast_vlan_inv[bw_profile->stag.vid_value], bw_profile->ctag.vid_value, intIfNum )!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error removing Client (%d) from AVL trees (flow %d)",__FUNCTION__,bw_profile->stag.vid_value,flow_id);
      return L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Client (%d) removed for IGMP stats (flow %d)",__FUNCTION__,bw_profile->ctag.vid_value,flow_id);
  }
  // Otherwise, remove the bandwidth profile
  else {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Normal flow (flow %d)",__FUNCTION__,flow_id);

    // Correct bandwidth parameters
    bw_profile->port_bmp           = 0x3ff00;
    bw_profile->stag.vid_value     = flow->vid_root;
    bw_profile->stag.vid_mask      = (L7_uint16)-1;
  
    // Search for Client
    if (bw_profile->ctag.vid_value!=(L7_uint16)-1) {
      // When CVid is null, mask must be so
      if (bw_profile->ctag.vid_value==0)  bw_profile->ctag.vid_mask=0;
  
      // Remove profile rule and redirection rule
      for (i=0; i<2; i++)
      {
        // Remove bandwidth profile
        profile_case = *bw_profile;

        switch (i)
        {
        case 0:
          break;
        default:
          profile_case.stag.vid_value = 0;    // No stag classification
          profile_case.stag.vid_mask  = 0;
          break;
        }

        for (client_index=0; client_index<SNOOP_MAX_NUMBER_OF_CLIENTS; client_index++)  {
          fpentry_ptr = flow->fpentry_ptr[client_index];
          if (fpentry_ptr==L7_NULLPTR || !fpentry_ptr->inUse)  continue;
          // Entry is equal... redefine it
          if (profile_case.stag.vid_value==fpentry_ptr->stag.vid_value && profile_case.stag.vid_mask==fpentry_ptr->stag.vid_mask &&
              profile_case.ctag.vid_value==fpentry_ptr->ctag.vid_value && profile_case.ctag.vid_mask==fpentry_ptr->ctag.vid_mask)
          {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: client_id match (fpentry_id=%d)\n",__FUNCTION__,fpentry_ptr->fpentry_id);
            break;
          }
        }
        // No clients available
        if (client_index>=SNOOP_MAX_NUMBER_OF_CLIENTS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Client not found\n",__FUNCTION__);
          continue;
        }

        // Remove fp entry
        if ((rc=ptin_fpentry_destroy(fpentry_ptr))!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error executing ptin_fpentry_destroy\n",__FUNCTION__);
          return rc;
        }
        flow->fpentry_ptr[client_index] = L7_NULLPTR;   // Free client table
      }
    }
    // Otherwise, read all clients
    else  {
      // This section is used to remove all rules associated to a certain EVC
      // So, all rules should be destroyed immediately (counter=0)

      // Run all clients
      for (client_index=0; client_index<SNOOP_MAX_NUMBER_OF_CLIENTS; client_index++)  {
        fpentry_ptr = flow->fpentry_ptr[client_index];
        // Rules not defined or not in use, are skipped
        if (fpentry_ptr==L7_NULLPTR || !fpentry_ptr->inUse)  continue;

        // Entry is equal... remove it
        if ( fpentry_ptr->stag.vid_value == bw_profile->stag.vid_value &&
             fpentry_ptr->stag.vid_mask  == bw_profile->stag.vid_mask )
        {
          // Save ctag (will be used to remove later the redirection rules)
          ctag = fpentry_ptr->ctag;

          // Remove profile rule
          fpentry_ptr->counter = 0;   // Remove immediately
          if ((rc=ptin_fpentry_destroy(fpentry_ptr))!=L7_SUCCESS)  {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error executing ptin_fpentry_destroy\n",__FUNCTION__);
            return rc;
          }
          flow->fpentry_ptr[client_index] = L7_NULLPTR;   // Free client table

          // If this entry has a valid ctag, remove all redirection rules, with (only) this ctag
          if (ctag.vid_mask!=0)
          {
            // Run all clients
            for (client_index_redirect=0; client_index_redirect<SNOOP_MAX_NUMBER_OF_CLIENTS; client_index_redirect++)
            {
              fpentry_redirect_ptr = flow->fpentry_ptr[client_index_redirect];
              // If it is the client removed earlier, or it is not a valid client, skip
              if (client_index==client_index_redirect || fpentry_redirect_ptr==L7_NULLPTR || !fpentry_redirect_ptr->inUse)  continue;
              // Only rules with no defined stag are compared
              if (fpentry_redirect_ptr->stag.vid_value == 0 &&
                  fpentry_redirect_ptr->stag.vid_mask  == 0 &&
                  fpentry_redirect_ptr->ctag.vid_value == ctag.vid_value &&
                  fpentry_redirect_ptr->ctag.vid_mask  == ctag.vid_mask)
              {
                // Remove redirect rule
                fpentry_redirect_ptr->counter = 0;   // Remove immediately
                if ((rc=ptin_fpentry_destroy(fpentry_redirect_ptr))!=L7_SUCCESS)  {
                  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error executing ptin_fpentry_destroy\n",__FUNCTION__);
                  return rc;
                }
                flow->fpentry_ptr[client_index_redirect] = L7_NULLPTR;   // Free client table
              }
            }
          }
        }
      }
    }
  }
  
  return L7_SUCCESS;
}


L7_RC_t ptin_flow_get_profile(L7_uint16 flow_id, L7_uint8 port, L7_bw_profile *bw_profile, L7_uint16 *nProfiles)
{
  L7_flow_struct *flow;
  L7_fpentry_data *fpentry_ptr;
  L7_int client_index;
  L7_RC_t rc;

  // Validate arguments
  if (flow_id>=FLOWS_MAX || bw_profile==NULL)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Pointer for the EVC
  flow = &flow_database[flow_id];

  // Check if evc is defined
  if (!flow->inUse)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Flow is not defined\n",__FUNCTION__);
    return L7_FAILURE;
  }

  bw_profile->port_bmp           = (L7_uint32) 0x3ff00;
  bw_profile->stag.vid_value     = flow->vid_root;
  bw_profile->stag.vid_mask      = (L7_uint16)-1;

  // Can i add one more client?

  // If Ctag is provided, Search for that client
  if (bw_profile->ctag.vid_value!=(L7_uint16)-1) {
    // When CVid is null, mask must be so
    if (bw_profile->ctag.vid_value==0)  bw_profile->ctag.vid_mask=0;

    printf("%s(%d)\r\n",__FUNCTION__,__LINE__);
    for (client_index=0; client_index<SNOOP_MAX_NUMBER_OF_CLIENTS; client_index++)  {
      fpentry_ptr = flow->fpentry_ptr[client_index];
      if (fpentry_ptr==L7_NULLPTR || !fpentry_ptr->inUse)  continue;
      printf("%s(%d) stag={%u,%u}, ctag={%u,%u}\r\n",__FUNCTION__,__LINE__,fpentry_ptr->stag.vid_value,fpentry_ptr->stag.vid_mask,fpentry_ptr->ctag.vid_value,fpentry_ptr->stag.vid_mask);
      // Entry is equal... redefine it
      if (fpentry_ptr->stag.vid_value==bw_profile->stag.vid_value && fpentry_ptr->stag.vid_mask==bw_profile->stag.vid_mask &&
          fpentry_ptr->ctag.vid_value==bw_profile->ctag.vid_value && fpentry_ptr->ctag.vid_mask==bw_profile->ctag.vid_mask)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: client_id match (fpentry_id=%d)\n",__FUNCTION__,fpentry_ptr->fpentry_id);
        break;
      }
    }
    // No clients available
    if (client_index>=SNOOP_MAX_NUMBER_OF_CLIENTS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Client not found\n",__FUNCTION__);
      return L7_FAILURE;
    }
  
    // At this point client_index, and fpentry_ptr have valid values
  
    // Create fp entry
    if ((rc=ptin_fpentry_read(bw_profile, fpentry_ptr))!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error executing ptin_fpentry_read\n",__FUNCTION__);
      return rc;
    }

    if (nProfiles!=L7_NULLPTR)  *nProfiles=1;
  }
  // Otherwise, read all clients
  else  {
    L7_uint16 index=0;

    for (client_index=0; client_index<SNOOP_MAX_NUMBER_OF_CLIENTS; client_index++)  {
      fpentry_ptr = flow->fpentry_ptr[client_index];
      if (fpentry_ptr==L7_NULLPTR)  continue;
      // Entry is equal... redefine it
      if (fpentry_ptr->inUse && memcmp(&(fpentry_ptr->stag),&(bw_profile[0].stag),sizeof(L7_vid))==0)  {
        if (index>0) {
          memcpy(&(bw_profile[index].stag),&(bw_profile[0].stag),sizeof(L7_vid));
        }
        memcpy(&(bw_profile[index].ctag),&(fpentry_ptr->ctag),sizeof(L7_vid));
        // Create fp entry
        if ((rc=ptin_fpentry_read(&bw_profile[index++], fpentry_ptr))!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error executing ptin_fpentry_read\n",__FUNCTION__);
          return rc;
        }
      }
    }
    // No clients available
    if (index==0)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Clients not found\n",__FUNCTION__);
      return L7_FAILURE;
    }
    if (nProfiles!=L7_NULLPTR)  *nProfiles=index;
  }
  
  return L7_SUCCESS;
}

//static L7_RC_t ptin_flow_bcast_ratelimit_set(L7_uint16 flow_id)
//{
//  return L7_SUCCESS;
//}
//
//static L7_RC_t ptin_flow_bcast_ratelimit_unset(L7_uint16 flow_id)
//{
//  return L7_SUCCESS;
//}

static L7_RC_t ptin_flow_build(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port)
{
  //L7_uint16 p;
  //L7_uint16 vlan_root;
  L7_RC_t rc;
  L7_flow_struct *flow;
  L7_vlan_defs vlan_defs = { 1, 1};

  // Validate flow_id
  if (flow_id>=FLOWS_MAX || port==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: invalid arguments (flow_id=%u\n",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }
  // Check if flow exists
  if (!flow_database[flow_id].inUse)  {
    return L7_FAILURE;
  }

  flow = &flow_database[flow_id];

  // Create translation entries
  if (flow->type==FLOW_TYPE_BITSTREAM || flow->type==FLOW_TYPE_UNIVOIP)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Bitstream/VoIP flow (flow_id=%u)\n",__FUNCTION__,flow_id);
    vlan_defs.include_cpu_intf = L7_FALSE;
    ptin_filter_vlan_mode = L7_FILTER_FORWARD_ALL;
  }
  else if (flow->type==FLOW_TYPE_MULTICAST)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: MC flow (flow_id=%u)\n",__FUNCTION__,flow_id);
    vlan_defs.include_cpu_intf = L7_TRUE;
    ptin_filter_vlan_mode = FD_VLAN_FILTERING_MODE;
  }
  else if (flow->type==FLOW_TYPE_BITSTREAM_WITH_MACLEARN)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Bitstream w/MAC Learn (flow_id=%u)\n",__FUNCTION__,flow_id);
    vlan_defs.include_cpu_intf = L7_FALSE;
    ptin_filter_vlan_mode = (flow->mc_flood) ? L7_FILTER_FORWARD_ALL : FD_VLAN_FILTERING_MODE;
  }
  else  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Other flow (flow_id=%u)\n",__FUNCTION__,flow_id);
    vlan_defs.include_cpu_intf = L7_TRUE;
    ptin_filter_vlan_mode = (flow->mc_flood) ? L7_FILTER_FORWARD_ALL : FD_VLAN_FILTERING_MODE;
  }

  if (dtlPtinVlanDefs(&vlan_defs)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) ERROR\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if ((rc=flow_add_translation_entries(flow_id,port)))  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with flow_add_translation_entries\n",__FUNCTION__);
    return rc;
  }

//// Wait until dot1q and snoop Queue is empty
//do {
//  usleep(1000);
//} while (!dot1qQueueEmpty());
//
//// Check if all vlans were added to Snooping module
//if (ptin_flow_snoop_is_ready(flow->port)!=L7_SUCCESS)  {
//  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Vlans were not added to Snooping module (flow_id=%u)",__FUNCTION__,flow_id);
//  return L7_FAILURE;
//}

//// If DHCP is enabled, configure it
//if (flow->dhcp_enable) {
//  if (ptin_flow_activate_dhcpOp82(-1,flow_id)!=L7_SUCCESS)  {
//    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with ptin_flow_activate_dhcpOp82(flow_id=%u)",__FUNCTION__,flow_id);
//    return L7_FAILURE;
//  }
//}
//
//// Multicast services processing
//if (flow->type==FLOW_TYPE_MULTICAST)  {
//
//  for (p=igmp_firstRootPort; p<L7_SYSTEM_N_INTERF && port[p].type!=MEF10_EVC_UNITYPE_ROOT; p++);
//  if (p<L7_SYSTEM_N_INTERF)  {
//    // This is the root vlan
//    vlan_root = port[p].ceVid;
//
//    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: IGMP Snooping processing (flow_id=%u)",__FUNCTION__,flow_id);
//    if (ptin_flow_activate_snooping(-1,flow_id,vlan_root)!=L7_SUCCESS) {
//      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error configuring IGMP snooping (flow_id=%u)",__FUNCTION__,flow_id);
//      return L7_FAILURE;
//    }
//    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: IGMP Snooping configured successfully (flow_id=%u)",__FUNCTION__,flow_id);
//  }
//}

  // Wait until dot1q and snoop Queue is empty
//do {
//  usleep(1000);
//} while (!snoopQueueEmpty());

  // Restore L2 learn operation
  //bcm_l2_addr_thaw(ptin_hapi_unit);
  return L7_SUCCESS;
}

static L7_RC_t ptin_flow_unbuild(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port)
{
  L7_flow_struct *flow;

  // Validate flow_id
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  if (!flow_database[flow_id].inUse)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: flow not used\n",__FUNCTION__);
    return L7_SUCCESS;
  }

  flow = &flow_database[flow_id];

  // Freeze L2 learning
  //bcm_l2_addr_freeze(ptin_hapi_unit);

//// Deactivate DHCP
//ptin_flow_deactivate_dhcpOp82(-1,flow_id);
//
//// Multicast services processing
//if (flow->type==FLOW_TYPE_MULTICAST)  {
//  ptin_flow_deactivate_snooping(-1,flow_id);
//  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: IGMP Snooping configuration removed (flow_id=%u)",__FUNCTION__,flow_id);
//}

  // Remove translation entries
  if (flow_remove_translation_entries(flow_id,port))  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with flow_remove_translation_entries\n",__FUNCTION__);
  }
  
  // Wait until dot1q and snoop Queue is empty
//do {
//  usleep(1000);
//} while (!snoopQueueEmpty());
//
//// Wait until dot1q and snoop Queue is empty
//do {
//  usleep(1000);
//} while (!dot1qQueueEmpty());
  
  return L7_SUCCESS;
}


static L7_RC_t flow_add_translation_entries(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port)
{
  L7_uint8  p, i;
  L7_flow_struct *flow;
  L7_uint16 vid_start, vid, vlan_root=0;
  L7_uint32 intIfNum, interface;
  L7_RC_t rc;
  L7_BOOL new_port[L7_SYSTEM_N_INTERF];

  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Determine vlan root, for igmp snooping processing
  vlan_root=0;
  for (p=igmp_firstRootPort; p<L7_SYSTEM_N_INTERF && port[p].type!=MEF10_EVC_UNITYPE_ROOT; p++);
  if (p<L7_SYSTEM_N_INTERF)  {
    // This is the root vlan
    vlan_root = port[p].ceVid;
  }

  flow = &flow_database[flow_id];

  memset(new_port,0x00,sizeof(L7_BOOL)*L7_SYSTEM_N_INTERF);

  // Run all ports
  for (p=0; p<L7_SYSTEM_N_INTERF; p++)  {

    // Initialize entry (necessary for first time inits)
    if (flow->port[p].type!=MEF10_EVC_UNITYPE_UNUSED && flow->port[p].ceVid==0 && flow->vid_xlate[p]==0) {
      flow->port[p].type = MEF10_EVC_UNITYPE_UNUSED;
    }

    // Do nothing for already existent entries
    if (flow->port[p].type!=MEF10_EVC_UNITYPE_UNUSED)  continue;

    // Skip not used ports
    if (port[p].type==MEF10_EVC_UNITYPE_UNUSED)  continue;

    // Skip if port already has translation entries
    //if (flow->vid_xlate[p]!=0)  continue;

    // Search for the vid to use
    if (flow->vid_xlate[p]!=0) {
      new_port[p] = L7_FALSE;
      vid=flow->vid_xlate[p];
    }
    else if (port[p].type==MEF10_EVC_UNITYPE_ROOT && flow->vid_root!=0)  {
      new_port[p] = L7_TRUE;
      vid=flow->vid_root;
      vlan_id_usage[vid]++;
    }
    else  {
      new_port[p] = L7_TRUE;
      if (flow_id==0) {
        // Inband flow, uses vlan 4093
        vid = L7_DOT1Q_MAX_VLAN_ID;
      }
      else  {
        // For other EVCs use only vlan below or equal than 4092
        // Search for a non-used Vlan
        vid_start = flow_id*16;
        for (vid=vid_start; vid<vid_start+16; vid++)  {
          if ((rc=usmDbVlanIDGet(1,vid))==L7_FAILURE)  {
            return L7_FAILURE;
          }
          if (vlan_id_usage[vid]==0 && rc!=L7_SUCCESS)  break;
        }
        // Not found situation
        if (vid>=vid_start+16)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d Not found vlan!\n",__FILE__, __FUNCTION__, __LINE__);
          return L7_FAILURE;
        }
      }
      vlan_id_usage[vid]++;
      // Only create vlan after adding translation entries!
      // That's because, callbacks generated after vlan creation, will use translation tables!
    }

    // Ingress entry
    if (port[p].ceVid<4096)  {
      if ((rc=ptin_vlan_translate_add(p, port[p].ceVid, vid)))  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error using bcm_vlan_translate_add\n",__FUNCTION__);
        return rc;
      }
      /* Only add egress entry, if port is root */
      if (port[p].type==MEF10_EVC_UNITYPE_ROOT)
      {
        // Egress entry
        if ((rc=ptin_vlan_translate_egress_add(p,vid,port[p].ceVid)))  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error using bcm_vlan_translate_egress_add\n",__FUNCTION__);
          return rc;
        }
      }
    }

    // Update root vid
    if (port[p].type==MEF10_EVC_UNITYPE_ROOT && flow->vid_root==0)  {
      flow->vid_root=vid;
    }

    // Signalize Ingress translation!
    flow->vid_xlate[p]=vid;
  }

  // Run all ports
  for (p=0; p<L7_SYSTEM_N_INTERF; p++)  {

    // Do nothing for already existent entries (it may only change filetring mode)
    if (flow->port[p].type!=MEF10_EVC_UNITYPE_UNUSED)
    {
      if (flow->vid_xlate[p]>0 && flow->vid_xlate[p]<=4095)
      {
        /* Set multicast flooding mode */
        if (usmdbFilterVlanFilteringModeSet(1, flow->vid_xlate[p], ptin_filter_vlan_mode)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d Error setting filtering mode %u to vlan %u\n",
                     __FILE__, __FUNCTION__, __LINE__,ptin_filter_vlan_mode,flow->vid_xlate[p]);
          return L7_FAILURE;
        }
        else
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d Success setting filtering mode %u to vlan %u\n",
                      __FILE__, __FUNCTION__, __LINE__,ptin_filter_vlan_mode,flow->vid_xlate[p]);
        }
      }
      continue;
    }

    // Skip not used ports (or not added - it was already there)
    if (port[p].type==MEF10_EVC_UNITYPE_UNUSED)
    {
      continue;
    }

    // Skip if port already has translation entries    
    if (flow->vid_xlate[p]==0 || flow->vid_xlate[p]>4095)  continue;

    // Get vid translated
    vid = flow->vid_xlate[p];

    // Only create vlan after adding translation entries! At this point, translation entries already had benn added
    // That's because, callbacks generated after vlan creation, will use translation tables!
    if (port[p].type!=MEF10_EVC_UNITYPE_ROOT || flow->number_of_roots==0) {
      // Create vlan
      if ((rc=usmDbVlanCreate(1,vid))!=L7_SUCCESS && rc!=L7_ALREADY_CONFIGURED)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d => vid=%u, rc=%d\n",__FILE__, __FUNCTION__, __LINE__,vid,rc);
        return L7_FAILURE;
      }

      if (usmDbVlanMemberSet(0,vid,L7_ALL_INTERFACES,L7_DOT1Q_FORBIDDEN,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
      if (usmDbVlanTaggedSet(0,vid,L7_ALL_INTERFACES,L7_DOT1Q_TAGGED)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
    }

    // Open vlan mask
    if (ptin_translate_port2intf(p,&intIfNum)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    if (usmDbVlanMemberSet(0,vid,intIfNum,L7_DOT1Q_FIXED,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    if (usmDbVlanTaggedSet(0,vid,intIfNum,L7_DOT1Q_TAGGED)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u added to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,intIfNum,vid);

    // Porta � um ROOT e existem leafs?
    if (port[p].type==MEF10_EVC_UNITYPE_ROOT && flow->number_of_leafs>0)  {
      // Percorrer todas as leafs
      for (i=0; i<L7_SYSTEM_N_INTERF; i++)  {
        if (port[i].type!=MEF10_EVC_UNITYPE_LEAF || flow->vid_xlate[i]==0)  continue;
        // Ao egresso do porto de root, adicionar tradu��o de vlan de cada leaf
        if ((rc=ptin_vlan_translate_egress_add(p,flow->vid_xlate[i],port[p].ceVid)))  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error using bcm_vlan_translate_egress_add 2\n",__FUNCTION__);
          return rc;
        }
        // Open vlan mask
        if (ptin_translate_port2intf(p,&interface)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
          return L7_FAILURE;
        }
        if (usmDbVlanMemberSet(0,flow->vid_xlate[i],interface,L7_DOT1Q_FIXED,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
          return L7_FAILURE;
        }
        if (usmDbVlanTaggedSet(0,flow->vid_xlate[i],interface,L7_DOT1Q_TAGGED)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
          return L7_FAILURE;
        }
        //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u added to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,interface,flow->vid_xlate[i]);

        // Este � o primeiro root?
        if (flow->number_of_roots==0)  {
          flow->number_of_roots=0;
          if ((rc=ptin_vlan_translate_egress_add(i,vid,port[i].ceVid)))  {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error using bcm_vlan_translate_egress_add 3\n",__FUNCTION__);
            return rc;
          }
          // Open vlan mask
          if (ptin_translate_port2intf(i,&interface)!=L7_SUCCESS)  {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
            return L7_FAILURE;
          }
          if (usmDbVlanMemberSet(0,vid,interface,L7_DOT1Q_FIXED,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
            return L7_FAILURE;
          }
          if (usmDbVlanTaggedSet(0,vid,interface,L7_DOT1Q_TAGGED)!=L7_SUCCESS)  {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
            return L7_FAILURE;            
          }
          //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u added to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,interface,vid);
        }
      }
    }
    // Porta � uma LEAF e existem roots?
    else if (port[p].type==MEF10_EVC_UNITYPE_LEAF && flow->vid_root!=0 && flow->number_of_roots>0)  {
      // Ao egresso da leaf, adicionar vlan de root
      if ((rc=ptin_vlan_translate_egress_add(p,flow->vid_root,port[p].ceVid)))  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error using bcm_vlan_translate_egress_add 4\n",__FUNCTION__);
        return rc;
      }
      // Open vlan mask
      if (ptin_translate_port2intf(p,&interface)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
      if (usmDbVlanMemberSet(0,flow->vid_root,interface,L7_DOT1Q_FIXED,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
      if (usmDbVlanTaggedSet(0,flow->vid_root,interface,L7_DOT1Q_TAGGED)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
        return L7_FAILURE;            
      }
      //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u added to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,interface,flow->vid_root);

      // Percorrer todas as roots
      for (i=0; i<L7_SYSTEM_N_INTERF; i++)  {
        if (port[i].type!=MEF10_EVC_UNITYPE_ROOT || flow->vid_xlate[i]==0)  continue;
        if ((rc=ptin_vlan_translate_egress_add(i,vid,port[i].ceVid)))  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error using bcm_vlan_translate_egress_add 5\n",__FUNCTION__);
          return rc;
        }
        // Open vlan mask
        if (ptin_translate_port2intf(i,&interface)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
          return L7_FAILURE;
        }
        if (usmDbVlanMemberSet(0,vid,interface,L7_DOT1Q_FIXED,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
          return L7_FAILURE;
        }
        if (usmDbVlanTaggedSet(0,vid,interface,L7_DOT1Q_TAGGED)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__, __FUNCTION__, __LINE__);
          return L7_FAILURE;
        }
        //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u added to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,interface,vid);
      }
    }

    // Actualizar contabilidade de roots e leafs
    if (new_port[p]) {
      if (port[p].type==MEF10_EVC_UNITYPE_ROOT)      flow->number_of_roots++;
      else if (port[p].type==MEF10_EVC_UNITYPE_LEAF) flow->number_of_leafs++;
    }

    if (port[p].ceVid > 0 && port[p].ceVid < 4096)
    {
      EVCid_table[port[p].ceVid][p] = flow_id;
    }
    if (flow->vid_xlate[p] > 0 && flow->vid_xlate[p] < 4096)
    {
      EVCid_table_internalVlans[flow->vid_xlate[p]][p] = flow_id;
    }

    flow->port[p].type = port[p].type;
    flow->port[p].ceVid = port[p].ceVid;
    // Update original EVC data
    if (p>=igmp_firstRootPort) {
      flow->port_original[p].type = port[p].type;
      flow->port_original[p].ceVid = port[p].ceVid;
    }

    // Wait until all vlan callbacks are executed
    while (!dot1qQueueEmpty()) usleep(1000);

    // Save translation entries for snooping module
    if (flow->type==FLOW_TYPE_MULTICAST) {
      ptin_mcflow_translate_save(flow->port[p].ceVid, flow->vid_xlate[p]);
    }

    // If DHCP is enabled, configure it
    if (flow->dhcp_enable) {
      if ((rc=ptin_flow_activate_dhcpOp82(p,flow_id))!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with ptin_flow_activate_dhcpOp82 for port %u (flow_id=%u)",__FUNCTION__,p,flow_id);
        return rc;
      }
    }
    // Multicast services processing
    if (flow->type==FLOW_TYPE_MULTICAST)  {
      if (vlan_root!=0)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: IGMP Snooping processing for port %u (flow_id=%u)",__FUNCTION__,p,flow_id);
        if ((rc=ptin_flow_activate_snooping(p,flow_id,vlan_root))!=L7_SUCCESS) {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error configuring IGMP snooping for port %u (flow_id=%u)",__FUNCTION__,p,flow_id);
          return rc;
        }
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: IGMP Snooping configured successfully for port %u (flow_id=%u)",__FUNCTION__,p,flow_id);
      }
    }
    while (!snoopQueueEmpty())  usleep(1000);

    if (flow_id>0)
    {
      if (flow->type==FLOW_TYPE_UNICAST)
      {
        // Add rate limiter only to UNICAST flows
        if ((rc=dtlPtinBcastPktLimit(L7_TRUE, intIfNum, flow->vid_xlate[p] /*flow->port[p].ceVid*/))!=L7_SUCCESS)
          return rc;
      }
      else if (flow->type==FLOW_TYPE_BITSTREAM)
      {
        if ((rc=dtlPtinBitStreamUpFwd(intIfNum,L7_TRUE,flow_id,flow->vid_xlate[p] /*flow->port[p].ceVid*/))!=L7_SUCCESS)
          return rc;
      }
    }
  }

//// Only update entries, which didn't exist before
//for (p=0; p<L7_SYSTEM_N_INTERF; p++)  {
//  if (flow->port[p].type==MEF10_EVC_UNITYPE_UNUSED) {
//    flow->port[p] = port[p];
//  }
//}
  //memcpy(flow->port,port,sizeof(L7_HwEthernetMef10CeVidMap)*L7_SYSTEM_N_INTERF);
  //memcpy(flow->usp,usp_map,sizeof(DAPI_USP_t)*L7_SYSTEM_N_INTERF);

  return L7_SUCCESS;
}

static L7_RC_t flow_remove_translation_entries(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port)
{
  L7_uint8 p, i;
  L7_flow_struct *flow;
  L7_uint16 vid;
  L7_uint32 interface;
  L7_RC_t   rc;

  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  flow = &flow_database[flow_id];

  // Initialize entry (necessary for first time inits)
  for (p=0; p<L7_SYSTEM_N_INTERF; p++)  {
    if (flow->port[p].type!=MEF10_EVC_UNITYPE_UNUSED && flow->port[p].ceVid==0) {
      flow->port[p].type = MEF10_EVC_UNITYPE_UNUSED;
    }
  }

  // VERIFICAR PARA REMO��O DE ENTRADAS DE TRADU��O
  for (p=0; p<L7_SYSTEM_N_INTERF; p++)  {

    // N�o processar portas n�o existentes no EVC (velho)
    if (flow->port[p].type==MEF10_EVC_UNITYPE_UNUSED)  continue;

    // Se a porta agora n�o est� a ser usada, se mudou a vlan associada, se o tipo da porta mudou, ou se o numero de classes de servi�o mudou,
    // destruir todas as entradas de tradu��o
    if (port!=NULL &&
        port[p].type==flow->port[p].type &&
        flow->port[p].ceVid!=0 && port[p].ceVid==flow->port[p].ceVid)
      continue;

    // Entradas j� foram todas removidas
    if (flow->vid_xlate[p]==0)  continue;

    // Deactivate DHCP
    if (flow->dhcp_enable) {
      ptin_flow_deactivate_dhcpOp82(p,flow_id);
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: DHCP configuration removed for port %u (flow_id=%u)",__FUNCTION__,p,flow_id);
    }
  
    // Multicast services processing
    if (flow->type==FLOW_TYPE_MULTICAST)  {
      ptin_flow_deactivate_snooping(p,flow_id);
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: IGMP Snooping configuration removed for port %u (flow_id=%u)",__FUNCTION__,p,flow_id);
    }
    // Wait until dot1q and Snoop Queue is empty
    while (!snoopQueueEmpty())  usleep(1000);

    // Close vlan mask
    if (ptin_translate_port2intf(p,&interface)!=L7_SUCCESS)  {
      return L7_FAILURE;
    }
    if (flow_id>0)
    {
      if (flow->type==FLOW_TYPE_UNICAST)
      {
        // Add rate limiter only to UNICAST flows
        if ((rc=dtlPtinBcastPktLimit(L7_FALSE, interface, flow->vid_xlate[p] /*flow->port[p].ceVid*/))!=L7_SUCCESS)
          return rc;
      }
      else if (flow->type==FLOW_TYPE_BITSTREAM)
      {
        if ((rc=dtlPtinBitStreamUpFwd(interface,L7_FALSE,flow_id,flow->vid_xlate[p] /*flow->port[p].ceVid*/))!=L7_SUCCESS)
          return rc;
      }
    }

    //if (hapiPortPtr[p]->hapiModeparm.physical.phySemaphore == L7_NULL)  LOG_ERROR(0);
    //if (osapiSemaTake(hapiPortPtr[p]->hapiModeparm.physical.phySemaphore, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);

    // Vid a remover no ingress e no egresso
    vid = flow->vid_xlate[p];

    // Eliminar entrada de ingresso
    if (flow->port[p].ceVid!=0)  {
      if (ptin_vlan_translate_delete(p,flow->port[p].ceVid))  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with bcm_vlan_translate_delete (port=%u,oldvlan=%u)\n",__FUNCTION__,p,flow->port[p].ceVid);
        //return L7_FAILURE;
      }
      if (flow->port[p].type==MEF10_EVC_UNITYPE_ROOT)
      {
        // Eliminar entrada de egresso
        if (ptin_vlan_translate_egress_delete(p,vid))  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with bcm_vlan_translate_egress_delete 1 (port=%u,oldvlan=%u)\n",__FUNCTION__,p,vid);
          //return L7_FAILURE;
        }
      }
    }
    if (usmDbVlanMemberSet(0,vid,interface,L7_DOT1Q_FORBIDDEN,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
      return L7_FAILURE;
    }
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u removed to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,interface,vid);
    //pbm.pbits[0] = (1<<usp_map[p].port);
    //bcm_vlan_port_remove(usp_map[p].unit,vid,pbm);

    // Porta � um ROOT e existem leafs?
    if (flow->port[p].type==MEF10_EVC_UNITYPE_ROOT && flow->number_of_leafs>0)  {
      // Percorrer todas as leafs
      for (i=0; i<L7_SYSTEM_N_INTERF; i++)  {
        if (flow->port[i].type!=MEF10_EVC_UNITYPE_LEAF || flow->vid_xlate[i]==0)  continue;
        // Ao egresso do porto de root, remover tradu��o de vlan de cada leaf
        if (ptin_vlan_translate_egress_delete(p,flow->vid_xlate[i]))  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with bcm_vlan_translate_egress_delete 2 (port=%u,oldvlan=%u)\n",__FUNCTION__,p,flow->vid_xlate[i]);
          //return L7_FAILURE;
        }
        // Close vlan mask
        if (ptin_translate_port2intf(p,&interface)!=L7_SUCCESS)  {
          return L7_FAILURE;
        }
        if (usmDbVlanMemberSet(0,flow->vid_xlate[i],interface,L7_DOT1Q_FORBIDDEN,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
          return L7_FAILURE;
        }
        //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u removed to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,interface,flow->vid_xlate[i]);
        //pbm.pbits[0] = (1<<usp_map[p].port);
        //bcm_vlan_port_remove(usp_map[p].unit,flow->vid_xlate[i],pbm);

        // Este � o �nico root?
        if (flow->number_of_roots==1)  {
          //if (hapiPortPtr[i]->hapiModeparm.physical.phySemaphore == L7_NULL)  LOG_ERROR(0);
          //if (osapiSemaTake(hapiPortPtr[i]->hapiModeparm.physical.phySemaphore, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);
          // ao egresso de cada leaf remover a vlan de root
          if (ptin_vlan_translate_egress_delete(i,flow->vid_root))  {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with bcm_vlan_translate_egress_delete 3 (port=%u,oldvlan=%u)\n",__FUNCTION__,i,flow->vid_root);
            //return L7_FAILURE;
          }
          // Close vlan mask
          if (ptin_translate_port2intf(i,&interface)!=L7_SUCCESS)  {
            return L7_FAILURE;
          }
          if (usmDbVlanMemberSet(0,flow->vid_root,interface,L7_DOT1Q_FORBIDDEN,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
            return L7_FAILURE;
          }
          //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u removed to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,interface,flow->vid_root);
          //pbm.pbits[0] = (1<<usp_map[i].port);
          //bcm_vlan_port_remove(usp_map[i].unit,flow->vid_root,pbm);
          //if (osapiSemaGive(hapiPortPtr[i]->hapiModeparm.physical.phySemaphore) != L7_SUCCESS)  LOG_ERROR(0);
        }
      }
    }
    // Porta � uma LEAF e existem roots?
    else if (flow->port[p].type==MEF10_EVC_UNITYPE_LEAF && flow->vid_root!=0 && flow->number_of_roots>0)  {
      // Ao egresso da leaf, remover vlan de root
      if (ptin_vlan_translate_egress_delete(p,flow->vid_root))  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with bcm_vlan_translate_egress_delete 4 (port=%u,oldvlan=%u)\n",__FUNCTION__,p,flow->vid_root);
        //return L7_FAILURE;
      }
      // Close vlan mask
      if (ptin_translate_port2intf(p,&interface)!=L7_SUCCESS)  {
        return L7_FAILURE;
      }
      if (usmDbVlanMemberSet(0,flow->vid_root,interface,L7_DOT1Q_FORBIDDEN,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
        return L7_FAILURE;
      }
      //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u removed to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,interface,flow->vid_root);
      //pbm.pbits[0] = (1<<usp_map[p].port);
      //bcm_vlan_port_remove(usp_map[p].unit,flow->vid_root,pbm);

      // Percorrer todas as roots
      for (i=0; i<L7_SYSTEM_N_INTERF; i++)  {
        if (flow->port[i].type!=MEF10_EVC_UNITYPE_ROOT || flow->vid_xlate[i]==0)  continue;
        //if (hapiPortPtr[i]->hapiModeparm.physical.phySemaphore == L7_NULL)  LOG_ERROR(0);
        //if (osapiSemaTake(hapiPortPtr[i]->hapiModeparm.physical.phySemaphore, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);
        // Adicionar vlan de leaf ao egresso de cada root
        if (ptin_vlan_translate_egress_delete(i,vid))  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with bcm_vlan_translate_egress_delete 5 (port=%u,oldvlan=%u)\n",__FUNCTION__,i,vid);
          //return L7_FAILURE;
        }
        // Close vlan mask
        if (ptin_translate_port2intf(i,&interface)!=L7_SUCCESS)  {
          return L7_FAILURE;
        }
        if (usmDbVlanMemberSet(0,vid,interface,L7_DOT1Q_FORBIDDEN,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
          return L7_FAILURE;
        }
        //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Interface %u removed to vlan %u (L7_DOT1Q_FIXED)", __FUNCTION__, __LINE__,interface,vid);
        //pbm.pbits[0] = (1<<usp_map[i].port);
        //bcm_vlan_port_remove(usp_map[i].unit,vid,pbm);
        //if (osapiSemaGive(hapiPortPtr[i]->hapiModeparm.physical.phySemaphore) != L7_SUCCESS)  LOG_ERROR(0);
      }
    }

/*    // Destroy vlan?
    if ((evc->port[port].type==MEF10_EVC_UNITYPE_ROOT && evc->number_of_roots==1) || (evc->port[port].type==MEF10_EVC_UNITYPE_LEAF))  {
      if ((status=bcm_vlan_destroy(0,vid))!=BCM_E_NONE)  {
        printf("evc_route_clean: error with bcm_vlan_destroy (%s)\n",bcm_errmsg(status));
        return BRDCOM_ERROR_HW;
      }
      //xlate_vlan_utilization[vid]=1;
    }*/
    if (vlan_id_usage[vid]>0)  vlan_id_usage[vid]--;
    // This vlan will be cleared
    if (vlan_id_usage[vid]==0)
    {
      usmDbVlanDelete(1,vid);
    }

    // Actualizar contabilidade de roots e leafs
    if (flow->port[p].type==MEF10_EVC_UNITYPE_ROOT && flow->number_of_roots==1)  {
      flow->vid_root=0;
    }

    // Only dynamic entries will be removed from L2 table
    // For statis entries, we have to do it manually
    {
      L7_uchar8         keyNext[L7_FDB_KEY_SIZE];
      dot1dTpFdbData_t  fdbEntry;
      fdbMeberInfo_t    fdbMemberInfo;
      L7_uint16         vlan;

      memset(keyNext,0x00,sizeof(L7_uchar8)*L7_FDB_KEY_SIZE);
      while (fdbFind(keyNext,L7_MATCH_GETNEXT,&fdbEntry)==L7_SUCCESS) {
        // Prepare for the next cycle
        memcpy(keyNext, fdbEntry.dot1dTpFdbAddress, L7_FDB_KEY_SIZE);
        // Entry should be static
        if (fdbEntry.dot1dTpFdbEntryType!=L7_FDB_ADDR_FLAG_STATIC)
          continue;
        // And vlan must match
        vlan = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
        if (vlan!=flow->vid_xlate[p])
          continue;
        // Now, it's ok to be removed
        fdbMemberInfo.entryType = L7_FDB_ADDR_FLAG_STATIC;
        fdbMemberInfo.intIfNum  = fdbEntry.dot1dTpFdbPort;
        fdbMemberInfo.vlanId    = vlan;
        memcpy(fdbMemberInfo.macAddr,&fdbEntry.dot1dTpFdbAddress[2],L7_MAC_ADDR_LEN);
        fdbDelEntry(&fdbMemberInfo);
      }
    }

    // Signalize Ingress translation!
    flow->vid_xlate[p]=0;

    // Actualizar contabilidade de roots e leafs
    if (flow->port[p].type==MEF10_EVC_UNITYPE_ROOT)  {
      if (flow->number_of_roots>0)  flow->number_of_roots--;
    }
    else if (flow->port[p].type==MEF10_EVC_UNITYPE_LEAF)  {
      if (flow->number_of_leafs>0)  flow->number_of_leafs--;
    }

    // Clear translation entries for snooping module
    ptin_mcflow_translate_clear(flow->port[p].ceVid);

    // Update EVC data
    EVCid_table[flow->port[p].ceVid][p] = -1;
    EVCid_table_internalVlans[flow->vid_xlate[p]][p] = -1;
    flow->port[p].type  = MEF10_EVC_UNITYPE_UNUSED;
    flow->port[p].ceVid = (L7_uint16)-1;
    // Update original EVC data
    //if (p>=igmp_firstRootPort) {
      flow->port_original[p].type = MEF10_EVC_UNITYPE_UNUSED;
      flow->port_original[p].ceVid = (L7_uint16)-1;
    //}
    //if (osapiSemaGive(hapiPortPtr[p]->hapiModeparm.physical.phySemaphore) != L7_SUCCESS)  LOG_ERROR(0);

    while (!dot1qQueueEmpty()) usleep(1000);
  }

  //memset(flow->usp,0,sizeof(DAPI_USP_t)*L7_SYSTEM_N_INTERF);

  return L7_SUCCESS;
}

static L7_int flow_compare(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port)
{
  L7_uint16 flow_index;
  L7_uint8  p;
  L7_HwEthernetMef10CeVidMap *port_ref;

  // Now, check if there is an STAG mismatch relative to other EVCs
  for (flow_index=0; flow_index<FLOWS_MAX; flow_index++)  {

    // Only evaluate other valid EVCs
    if (flow_index==flow_id || !flow_database[flow_index].inUse)  continue;

    // database entry to be compared
    port_ref = flow_database[flow_index].port;

    // Run all ports
    for (p=0; p<L7_SYSTEM_N_INTERF; p++)  {
      // Do not consider unused ports
      if (port[p].type==MEF10_EVC_UNITYPE_UNUSED || port_ref[p].type==MEF10_EVC_UNITYPE_UNUSED)
        continue;

      // Check if vlan is equal
      if (port[p].ceVid==port_ref[p].ceVid)
        return FLOW_COMPARE_PORT_STAG_MISMATCH;
    }
  }

  // At this point there is no STAG mismatches
  // Check if EVC is equal

  if (flow_id<FLOWS_MAX && flow_database[flow_id].inUse)  {
    // database entry to be compared
    port_ref = flow_database[flow_id].port;
    
    // Check if proposed EVC is equal or different!
    for (p=0; p<L7_SYSTEM_N_INTERF; p++)  {
      // If type and svid differs, flow is different
      if ( port[p].type!=port_ref[p].type || 
          (port[p].type!=MEF10_EVC_UNITYPE_UNUSED && port[p].ceVid!=port_ref[p].ceVid))  {
        return FLOW_COMPARE_DIFFERENT;
      }
    }
  }
  // in case this EVC is to be created, return different
  else {
    return FLOW_COMPARE_DIFFERENT;
  }

  // Flow is equal
  return FLOW_COMPARE_EQUAL;
}


static L7_RC_t ptin_fpentry_read(L7_bw_profile *profile, L7_fpentry_data *fpentry_ptr)
{
  L7_fpentry_oper fp_oper;

  fp_oper.operation = L7_FPENTRY_OPER_READ;
  fp_oper.profile = profile;
  fp_oper.fpentry_ptr = fpentry_ptr;

  return dtlPtinFPentry(&fp_oper);
}

static L7_RC_t ptin_fpentry_create(/*L7_int *stat_id,*/ L7_bw_profile *profile, L7_fpentry_data **fpentry_ptr)
{
  L7_fpentry_oper fp_oper;
  L7_RC_t rc;

  fp_oper.operation = L7_FPENTRY_OPER_CREATE;
  fp_oper.profile = profile;
  fp_oper.fpentry_ptr = *fpentry_ptr;   // fp_oper.fpentry_ptr can be changed after dtlPtinFPentry call

  rc = dtlPtinFPentry(&fp_oper);

  // If operation was successfull, update *fpentry_ptr pointer (this pointer could be modified)
  if (rc==L7_SUCCESS) {
    *fpentry_ptr = fp_oper.fpentry_ptr;
  }
  return rc;
}

static L7_RC_t ptin_fpentry_destroy(L7_fpentry_data *fpentry_ptr)
{
  L7_fpentry_oper fp_oper;

  fp_oper.operation = L7_FPENTRY_OPER_DESTROY;
  fp_oper.profile = L7_NULLPTR;
  fp_oper.fpentry_ptr = fpentry_ptr;

  return dtlPtinFPentry(&fp_oper);
}

static L7_RC_t ptin_fpentry_counters(L7_HWEth_VlanStatistics_Block *counters, L7_fpentry_data *fpentry_ptr)
{
  L7_fpentry_oper fp_oper;

  fp_oper.operation   = L7_FPENTRY_OPER_COUNTERS;
  fp_oper.counters    = counters;
  fp_oper.profile     = L7_NULLPTR;
  fp_oper.fpentry_ptr = fpentry_ptr;

  return dtlPtinFPentry(&fp_oper);
}

L7_RC_t ptin_readVlanCounters(L7_uint16 FlowId, L7_uint16 svid, L7_HWEth_VlanStat *counters, L7_uint16 *nClients)
{
  L7_flow_struct *flow;
  L7_fpentry_data *fpentry_ptr;
  L7_int client_index;
  L7_RC_t rc;

  // Validate arguments
  if (FlowId>=FLOWS_MAX || counters==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Pointer for the EVC
  flow = &flow_database[FlowId];

  // Check if evc is defined
  if (!flow->inUse)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Flow is not defined\n",__FUNCTION__);
    return L7_FAILURE;
  }

  // SVlan is internal Vid root
  svid = flow->vid_root;

  // Stag definition
  if (svid<=L7_DOT1Q_MIN_VLAN_ID || svid>L7_DOT1Q_MAX_VLAN_ID)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid parameters\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Can i add one more client?

  // If Ctag is provided, Search for that client
  if (counters->CVid!=(L7_uint16)-1) {
    for (client_index=0; client_index<SNOOP_MAX_NUMBER_OF_CLIENTS; client_index++)  {
      fpentry_ptr = flow->fpentry_ptr[client_index];
      if (fpentry_ptr==L7_NULLPTR)  continue;
      // Entry is equal... redefine it
      if (fpentry_ptr->inUse && fpentry_ptr->stag.vid_value==svid && fpentry_ptr->ctag.vid_value==counters->CVid)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: client_id match (fpentry_id=%d)\n",__FUNCTION__,fpentry_ptr->fpentry_id);
        break;
      }
    }
    // No clients available
    if (client_index>=SNOOP_MAX_NUMBER_OF_CLIENTS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Client not found\n",__FUNCTION__);
      return L7_FAILURE;
    }

    // At this point client_index, and fpentry_ptr have valid values

    // Create fp entry
    if ((rc=ptin_fpentry_counters(&counters->stat, fpentry_ptr)))  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error executing ptin_fpentry_counters\n",__FUNCTION__);
      return rc;
    }

    if (nClients!=L7_NULLPTR)  *nClients=1;
  }
  // Otherwise, read all clients
  else  {
    L7_uint16 index=0;

    for (client_index=0; client_index<SNOOP_MAX_NUMBER_OF_CLIENTS; client_index++)  {
      fpentry_ptr = flow->fpentry_ptr[client_index];
      if (fpentry_ptr==L7_NULLPTR)  continue;
      // Entry is equal... redefine it
      if (fpentry_ptr->inUse && fpentry_ptr->stag.vid_value==svid)  {
        counters[index].CVid = fpentry_ptr->ctag.vid_value;
        if ((rc=ptin_fpentry_counters(&counters[index++].stat, fpentry_ptr)))  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error executing ptin_fpentry_counters\n",__FUNCTION__);
          return rc;
        }
      }
    }
    // No clients available
    if (index==0)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Clients not found\n",__FUNCTION__);
      return L7_FAILURE;
    }
    if (nClients!=L7_NULLPTR)  *nClients=index;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*********************************************************************/
L7_RC_t dtlPTinL2Callback(DAPI_USP_t *ddusp, 
                          DAPI_FAMILY_t family, 
                          DAPI_CMD_t cmd, 
                          DAPI_EVENT_t event,
                          void *dapiEventInfo)
{
  ptinL2Msg_t l2_msg;
  DAPI_ADDR_MGMT_CMD_t *dapiAddrEventInfo = (DAPI_ADDR_MGMT_CMD_t *) dapiEventInfo;
  L7_int port;
  L7_INTF_TYPES_t sysIntfType;
  L7_uint8 flags;
  L7_uint16 vlanId;
  L7_uint32 intIfNum;
  L7_uchar8 *macAddr;

  // Process only learned or aged addresses
  if (event!=DAPI_EVENT_ADDR_LEARNED_ADDRESS && event!=DAPI_EVENT_ADDR_AGED_ADDRESS) {
    return L7_SUCCESS;
  }

  macAddr= dapiAddrEventInfo->cmdData.unsolLearnedAddress.macAddr.addr;
  vlanId = dapiAddrEventInfo->cmdData.unsolLearnedAddress.vlanID;
  flags  = dapiAddrEventInfo->cmdData.unsolLearnedAddress.flags;

  // Only dynamic entries should be processed
  if ( flags != DAPI_ADDR_FLAG_LEARNED )
    return L7_SUCCESS;

  // Validate vlan
  if (vlanId<=1 || vlanId>=4095)
    return L7_SUCCESS;

  /* Validate interface */
  if (ddusp->slot!=0 && ddusp->slot!=1 && ddusp->port>=L7_SYSTEM_N_PORTS)
    return L7_SUCCESS;
  if (usmDbIntIfNumFromUSPGet(ddusp->unit, ddusp->slot, ddusp->port+1, &intIfNum)!=L7_SUCCESS)
    return L7_SUCCESS;
  if (ptin_translate_intf2port(intIfNum,&port)!=L7_SUCCESS)
    return L7_SUCCESS;
  /* Interface must not be CPU */
  if ((nimGetIntfType(intIfNum, &sysIntfType)!=L7_SUCCESS) || (sysIntfType==L7_CPU_INTF))
    return L7_SUCCESS;

  l2_msg.msgsType  = event;
  memcpy(l2_msg.mac_addr,dapiAddrEventInfo->cmdData.unsolLearnedAddress.macAddr.addr,sizeof(L7_uchar8)*L7_ENET_MAC_ADDR_LEN);
  l2_msg.vlanId    = vlanId;
  l2_msg.entryType = flags;
  l2_msg.intIfNum  = intIfNum;

  if (ptinL2Process(event,intIfNum,vlanId,macAddr,flags)!=L7_SUCCESS)
  {
    printf("L2 address not processed: port %u, vid %u: mac=%02X:%02X:%02X:%02X:%02X:%02X\n",port,l2_msg.vlanId,
           l2_msg.mac_addr[0],l2_msg.mac_addr[1],l2_msg.mac_addr[2],l2_msg.mac_addr[3],l2_msg.mac_addr[4],l2_msg.mac_addr[5]);
    return L7_FAILURE;
  }

  /*
  if (osapiMessageSend(ptinL2Queue, &l2_msg, sizeof(ptinL2Msg_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM ) != L7_SUCCESS)
  {
    printf("L2 address not processed: port %u, vid %u: mac=%02X:%02X:%02X:%02X:%02X:%02X\n",port,l2_msg.vlanId,
           l2_msg.mac_addr[0],l2_msg.mac_addr[1],l2_msg.mac_addr[2],l2_msg.mac_addr[3],l2_msg.mac_addr[4],l2_msg.mac_addr[5]);
  }
  */

  return L7_SUCCESS;
}

static L7_BOOL l2_show_not = 0;

void ptin_l2_show_not(L7_BOOL enable)
{
  l2_show_not = enable;
}

L7_RC_t ptinL2Process(L7_uint8 event, L7_uint32 intIfNum, L7_uint16 vid, L7_uchar8 *macAddr, L7_uint8 flags)
{
  L7_flow_struct *flow;
  //L7_uint32 intf2;
  L7_int    port, p;
  L7_uint16 flow_id;
  L7_RC_t status=L7_SUCCESS;

  dot1dTpFdbData_t  pData;
  L7_uchar8         keySearch[L7_FDB_KEY_SIZE] = {0};
  L7_RC_t           result;
  L7_BOOL           duplicated_mac;
  L7_int            param1, param2;

  fdbMeberInfo_t fdbMemberInfo;

  // Process only learned or aged addresses
  if (event!=DAPI_EVENT_ADDR_LEARNED_ADDRESS && event!=DAPI_EVENT_ADDR_AGED_ADDRESS) {
    return L7_SUCCESS;
  }

  // If static entry, do nothing
  if (flags == DAPI_ADDR_FLAG_STATIC)  {
    return L7_SUCCESS;
  }

  /* Validate interface, and get port number */
  if (intIfNum==0 || ptin_translate_intf2port(intIfNum,&port)!=L7_SUCCESS)
    return L7_SUCCESS;

  if (l2_show_not)
  {
    printf("I received a notification:\n");
    printf("flags=%u\n",flags);
    printf("mac_addr=%02x:%02x:%02x:%02x:%02x:%02x\n",macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
    printf("vid=%u\n",vid);
    printf("port=%u\n",port);
    printf("operation=%u\n",event);
  }

  /* If address was added, check if it was really added */
  if (event==DAPI_EVENT_ADDR_LEARNED_ADDRESS)
  {
    // Check if this Vid(Root)+MAC exists
    memcpy(&keySearch[0],&vid,sizeof(L7_uint16));
    memcpy(&keySearch[2],macAddr,sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);
    if (fdbFind(keySearch,L7_MATCH_EXACT,&pData)==L7_SUCCESS)
    {
      if (l2_show_not)
      {
        printf("Duplicated notification received!\r\n");
      }
      return L7_SUCCESS;
    }

    l2_dynamic_address_learned++;
    l2_dynamic_delta++;
  }
  else
  {
    l2_dynamic_address_deleted++;
    l2_dynamic_delta--;
  }

  // Run all flows
  for (flow_id=0; flow_id<FLOWS_MAX; flow_id++)  {
    // Flow must be in use, and port must be valid
    if (!flow_database[flow_id].inUse)  continue;

    // Don't add static entries for bitstream services (business)
    if (flow_database[flow_id].type==FLOW_TYPE_BITSTREAM)  continue;

    if (flow_database[flow_id].port[port].type==(L7_uint8)-1 ||
        flow_database[flow_id].vid_xlate[port]!=vid)
      continue;

    flow = &flow_database[flow_id];

    memset(&fdbMemberInfo,0,sizeof(fdbMemberInfo));
    fdbMemberInfo.entryType = L7_FDB_ADDR_FLAG_STATIC;                      // static entry
    memcpy(fdbMemberInfo.macAddr,macAddr,L7_MAC_ADDR_LEN);  // mac address
    fdbMemberInfo.intIfNum = intIfNum;                                   // port

    // Data for alarm situations
    param1 = (((L7_uint32) flow_id & 0xFFFF)<<16) | ((L7_uint32) *((L7_uint16 *) &macAddr[0]) & 0xFFFF);
    param2 = (L7_uint32) *((L7_uint32 *) &macAddr[2]);

    if (flow->port[port].type==MEF10_EVC_UNITYPE_LEAF && flow->vid_root>1 && flow->vid_root<4095)
    {
      // Vid to add is root vid
      fdbMemberInfo.vlanId = flow->vid_root;

      // Check if this Vid(Root)+MAC exists
      memcpy(&keySearch[0],&flow->vid_root,sizeof(L7_uint16));
      memcpy(&keySearch[2],macAddr,sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);
      result=fdbFind(keySearch,L7_MATCH_EXACT,&pData);

      // Check if is a duplicated MAC
      if (result==L7_SUCCESS && pData.dot1dTpFdbPort!=intIfNum)
      {
        duplicated_mac = L7_TRUE;
      }
      else
      {
        duplicated_mac = L7_FALSE;
      }
      
      if (event==DAPI_EVENT_ADDR_LEARNED_ADDRESS)
      {
        // Only add static entry, if this entry does not exist!
        if (result==L7_FAILURE) {
          if ((status=fdbAddEntry(&fdbMemberInfo))==L7_SUCCESS)  {
            l2_static_address_learned++;
            l2_static_delta++;
            //printf("L2 address added to leaf port %u, vid %u: %d\n",port,flow->vid_root,status);
          }
          else {
            printf("L2 address NOT added to leaf port %u, vid %u: %d\n",port,flow->vid_root,status);
            return L7_FAILURE;
          }
        }
        // Only consider duplicated MAC, if the existent entry is in another port
        else if (pData.dot1dTpFdbPort!=intIfNum) {
          // Duplicated MAC situation
          send_trap_gen_alarm(port,TRAP_ALARM_MAC_DUPLICATED,TRAP_ALARM_STATUS_EVENT,param1,param2);
          if (l2_show_not)
          {
            printf("(%d) MAC duplicated detected at port %u: flowId=%u, vlan=%u, mac=%02X:%02X:%02X:%02X:%02X:%02X (original port=%u,vlan=%u)\r\n",__LINE__,
                   port,flow_id,fdbMemberInfo.vlanId,macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],port,vid);
          }
        }
      }
      else if (event==DAPI_EVENT_ADDR_AGED_ADDRESS)
      {
        // If it exists, in the current leaf port, delete it...
        if (result==L7_SUCCESS)
        {
          if (pData.dot1dTpFdbPort==intIfNum)  {
            if (fdbMemberInfo.entryType>4)
            {
              printf("%s(%d) dtlFlag has an invalid value: %u",__FUNCTION__,__LINE__,fdbMemberInfo.entryType);
            }
            if ((status=fdbDelEntry(&fdbMemberInfo))==L7_SUCCESS)
            {
              l2_static_address_deleted++;
              l2_static_delta--;
              //printf("L2 address removed to leaf port %u, vid %u: %d\n",port,flow->vid_root,status);
            }
            else
            {
              printf("L2 address NOT removed to leaf port %u, vid %u: %d\n",port,flow->vid_root,status);
              return L7_FAILURE;
            }
          }
          else {
            // Duplicated MAC has gone
            //send_trap_alarm(port,TRAP_ALARM_MAC_DUPLICATED,TRAP_ALARM_STATUS_END,param1,param2);
          }
        }
        // If it didn't existed, or existing, was deleted in the previous step...
//      if (result!=L7_SUCCESS || pData.dot1dTpFdbPort==intIfNum)
//      {
//        // Run all leaf ports, except the current one, and check if there is the same MAC
//        for (p=0; p<L7_SYSTEM_N_INTERF; p++)
//        {
//          // Only other leaf ports will be processed
//          if (p==port || flow->port[p].type!=MEF10_EVC_UNITYPE_LEAF ||
//              flow->vid_xlate[p]==0 || flow->vid_xlate[p]>4095 ||
//              ptin_translate_port2intf(p,&intf2)!=L7_SUCCESS || intf2==0)
//            continue;
//          // Search for the entry Vid(leaf)+MAC
//          // If found, add Vid(Root)+MAC entry to this leaf!
//          memcpy(&keySearch[0],&flow->vid_xlate[p],sizeof(L7_uint16));
//          memcpy(&keySearch[2],macAddr,sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);
//          if (fdbFind(keySearch,L7_MATCH_EXACT,&pData)==L7_SUCCESS) {
//            fdbMemberInfo.intIfNum = intf2;
//            if ((status=fdbAddEntry(&fdbMemberInfo))==L7_SUCCESS)  {
//              l2_static_address_learned++;
//              l2_static_delta++;
//              //printf("L2 address added to leaf port %u, vid %u: %d\n",p,flow->vid_root,status);
//              // Duplicated MAC has gone
//              //send_trap_alarm(p,TRAP_ALARM_MAC_DUPLICATED,TRAP_ALARM_STATUS_END,param1,param2);
//            }
//            else {
//              printf("L2 address NOT added to leaf port %u, vid %u: %d\n",p,flow->vid_root,status);
//              return L7_FAILURE;
//            }
//          }
//        }
//      }
      }
    }
    else if (flow->port[port].type==MEF10_EVC_UNITYPE_ROOT)
    {
      // At the beginning there is no duplicated MACs
      duplicated_mac = L7_FALSE;

      for (p=0; p<L7_SYSTEM_N_INTERF; p++)  {
        if (flow->port[p].type!=MEF10_EVC_UNITYPE_LEAF || flow->vid_xlate[p]<=1 || flow->vid_xlate[p]>=4095)  continue;

        // Check if this Vid(Root)+MAC exists
        memcpy(&keySearch[0],&flow->vid_xlate[p],sizeof(L7_uint16));
        memcpy(&keySearch[2],macAddr,sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);
        result=fdbFind(keySearch,L7_MATCH_EXACT,&pData);

        // Check for duplicated MACs
        if (result==L7_SUCCESS && pData.dot1dTpFdbPort!=intIfNum)
        {
          duplicated_mac = L7_TRUE;
        }

        // Vid to add is leaf vid
        fdbMemberInfo.vlanId = flow->vid_xlate[p];

        if (event==DAPI_EVENT_ADDR_LEARNED_ADDRESS)
        {
          // Only add entry, if it doesn't exist
          if ((status=fdbAddEntry(&fdbMemberInfo))==L7_SUCCESS)  {
            l2_static_address_learned++;
            l2_static_delta++;
            //printf("L2 address added to root %u, vid=%u (p=%u): %d\n",port,flow->vid_xlate[p],p,status);
          }
          else {
            printf("L2 address NOT added to root %u, vid=%u (p=%u): %d\n",port,flow->vid_xlate[p],p,status);
            return L7_FAILURE;
          }
        }
        else if (event==DAPI_EVENT_ADDR_AGED_ADDRESS)
        {
          // Only remove entry, if it exists in the current interface
          if (result==L7_SUCCESS)
          {
            if (pData.dot1dTpFdbPort==intIfNum) {
              if (fdbMemberInfo.entryType>4)
              {
                printf("%s(%d) dtlFlag has an invalid value: %u",__FUNCTION__,__LINE__,fdbMemberInfo.entryType);
              }
              if ((status=fdbDelEntry(&fdbMemberInfo))==L7_SUCCESS)
              {
                l2_static_address_deleted++;
                l2_static_delta--;
                //printf("L2 address removed to root %u, vid=%u (p=%u): %d\n",port,flow->vid_xlate[p],p,status);
              }
              else {
                printf("L2 address NOT removed to root %u, vid=%u (p=%u): %d\n",port,flow->vid_xlate[p],p,status);
                return L7_FAILURE;
              }
            }
            else {
              // Duplicated MAC has gone
              //send_trap_alarm(port,TRAP_ALARM_MAC_DUPLICATED,TRAP_ALARM_STATUS_END,param1,param2);
            }
          }
        }
      }

      // If there was an identical entry we have a Duplicated MAC
      if (duplicated_mac)
      {
        // Duplicated MAC situation
        send_trap_gen_alarm(port,TRAP_ALARM_MAC_DUPLICATED,TRAP_ALARM_STATUS_EVENT,param1,param2);
        if (l2_show_not)
        {
          printf("(%d) MAC duplicated detected at port %u: flowId=%u, vlan=%u, mac=%02X:%02X:%02X:%02X:%02X:%02X (original port=%u,vlan=%u)\r\n",__LINE__,
                 port,flow_id,fdbMemberInfo.vlanId,macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],port,vid);
        }
      }
    }
    // Tere is only one flow for one vid
    break;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_flow_L2StaticEntry_add(L7_uint16 flow_id, L7_uint8 port, L7_uint8 *mac_address)
{
  L7_uchar8 keyNext[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t   fdbEntry;
  fdbMeberInfo_t     entry;
  L7_uint32          intIfNum;
  L7_uint16          vlanId;
  L7_flow_struct    *flow;
  L7_uint8           p;

  // Validate flow id
  if (/*flow_id==0 ||*/ flow_id>FLOWS_MAX)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid EVC (%u)\n",__FUNCTION__,__LINE__,flow_id);
    return L7_FAILURE;
  }
  // Validate Interface
  if (port>=L7_SYSTEM_N_INTERF || ptin_translate_port2intf(port,&intIfNum)!=L7_SUCCESS || intIfNum==0)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid port (%u)\n",__FUNCTION__,__LINE__,port);
    return L7_FAILURE;
  }

  flow = &flow_database[flow_id];
  // Validate EVC data
  if (!flow->inUse)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC %u is not used\n",__FUNCTION__,__LINE__,flow_id);
    return L7_FAILURE;
  }
  if (flow->port[port].type==MEF10_EVC_UNITYPE_UNUSED)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Port %u is not in use inside EVC %u\n",__FUNCTION__,__LINE__,port,flow_id);
    return L7_FAILURE;
  }
  if (flow->vid_xlate[port]<L7_DOT1Q_MIN_VLAN_ID || flow->vid_xlate[port]>=L7_DOT1Q_MAX_VLAN_ID)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Internal vlan %u is not valid (EVC %u)\n",__FUNCTION__,__LINE__,flow->vid_xlate[port],flow_id);
    return L7_FAILURE;
  }
  vlanId = flow->vid_xlate[port];

  // Prepare key
  memcpy(&keyNext[0],&vlanId,sizeof(L7_uint16));
  memcpy(&keyNext[2],mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

  // Find This entry
  // If this entry exists, remove it
  if (fdbFind(keyNext,L7_MATCH_EXACT,&fdbEntry)==L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) This entry exists\n",__FUNCTION__,__LINE__);

    // Prepare entry remotion
    memcpy(entry.macAddr,&fdbEntry.dot1dTpFdbAddress[2],sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    entry.vlanId    = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
    entry.intIfNum  = fdbEntry.dot1dTpFdbPort;
    entry.entryType = fdbEntry.dot1dTpFdbEntryType;

    // Remove this entry
    if (fdbDelEntry(&entry)!=L7_SUCCESS) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error removing entry\n",__FUNCTION__,__LINE__);
    }
  }

  // Add this entry
  // Prepare entry remotion
  memcpy(entry.macAddr,mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  entry.vlanId    = vlanId;
  entry.intIfNum  = intIfNum;
  entry.entryType = L7_FDB_ADDR_FLAG_STATIC;

  if (fdbAddEntry(&entry)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error adding entry\n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // The following piece of code, only applies to E-trees
  // If this port is a root port, add the same entry but for all leaf vlans
  if (flow->port[port].type==MEF10_EVC_UNITYPE_ROOT)
  {
    for (p=0; p<L7_SYSTEM_N_INTERF; p++)
    {
      if (p==port || flow->port[p].type!=MEF10_EVC_UNITYPE_LEAF)  continue;

      // If this entry exists, remove it
      memcpy(&keyNext[0],&flow->vid_xlate[p],sizeof(L7_uint16));
      memcpy(&keyNext[2],mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
      if (fdbFind(keyNext,L7_MATCH_EXACT,&fdbEntry)==L7_SUCCESS) {
        // Remove this entry
        entry.vlanId    = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
        entry.intIfNum  = fdbEntry.dot1dTpFdbPort;
        entry.entryType = fdbEntry.dot1dTpFdbEntryType;
        if (fdbDelEntry(&entry)!=L7_SUCCESS) {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error removing entry (port=%u,vlan=%u)\n",__FUNCTION__,__LINE__,port,entry.vlanId);
        }
      }
      // Add entry
      entry.vlanId    = flow->vid_xlate[p];
      entry.intIfNum  = intIfNum;
      entry.entryType = L7_FDB_ADDR_FLAG_STATIC;
      if (fdbAddEntry(&entry)!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error adding entry (port=%u, vlan=%u)\n",__FUNCTION__,__LINE__,port,entry.vlanId);
        return L7_FAILURE;
      }
    }
  }
  else if (flow->port[port].type==MEF10_EVC_UNITYPE_LEAF)
  {
    // If this entry exists, remove it
    memcpy(&keyNext[0],&flow->vid_root,sizeof(L7_uint16));
    memcpy(&keyNext[2],mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    if (fdbFind(keyNext,L7_MATCH_EXACT,&fdbEntry)==L7_SUCCESS) {
      // Remove this entry
      entry.vlanId    = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
      entry.intIfNum  = fdbEntry.dot1dTpFdbPort;
      entry.entryType = fdbEntry.dot1dTpFdbEntryType;
      if (fdbDelEntry(&entry)!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error removing entry (port=%u,vlan=%u)\n",__FUNCTION__,__LINE__,port,entry.vlanId);
      }
    }
    // Add entry
    entry.vlanId    = flow->vid_root;
    entry.intIfNum  = intIfNum;
    entry.entryType = L7_FDB_ADDR_FLAG_STATIC;
    if (fdbAddEntry(&entry)!=L7_SUCCESS) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error adding entry (port=%u, vlan=%u)\n",__FUNCTION__,__LINE__,port,entry.vlanId);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_flow_L2StaticEntry_delete(L7_uint16 flow_id, L7_uint8 port, L7_uint8 *mac_address)
{
  L7_uchar8 keyNext[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t   fdbEntry;
  fdbMeberInfo_t     entry;
  L7_uint32          intIfNum;
  L7_uint16          vlanId;
  L7_flow_struct    *flow;
  L7_uint8           p;
  L7_RC_t            result = L7_SUCCESS;

  // Validate arguments
  if (/*flow_id==0 ||*/ flow_id>FLOWS_MAX)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid EVC (%u)\n",__FUNCTION__,__LINE__,flow_id);
    return L7_FAILURE;
  }
  if (port>=L7_SYSTEM_N_INTERF || ptin_translate_port2intf(port,&intIfNum)!=L7_SUCCESS || intIfNum==0)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid port (%u)\n",__FUNCTION__,__LINE__,port);
    return L7_FAILURE;
  }

  flow = &flow_database[flow_id];

  // Validate EVC data
  if (!flow->inUse)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC %u is not used\n",__FUNCTION__,__LINE__,flow_id);
    return L7_FAILURE;
  }
  if (flow->port[port].type==MEF10_EVC_UNITYPE_UNUSED)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Port %u is not in use inside EVC %u\n",__FUNCTION__,__LINE__,port,flow_id);
    return L7_FAILURE;
  }
  if (flow->vid_xlate[port]<L7_DOT1Q_MIN_VLAN_ID || flow->vid_xlate[port]>=L7_DOT1Q_MAX_VLAN_ID)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Internal vlan %u is not valid (EVC %u)\n",__FUNCTION__,__LINE__,flow->vid_xlate[port],flow_id);
    return L7_FAILURE;
  }
  vlanId = flow->vid_xlate[port];

  // Find This entry. If not found, there is nothing to do
  memcpy(&keyNext[0],&vlanId,sizeof(L7_uint16));
  memcpy(&keyNext[2],mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  if (fdbFind(keyNext,L7_MATCH_EXACT,&fdbEntry)!=L7_SUCCESS)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) This entry does not exist (port,vlan=%u)\n",__FUNCTION__,__LINE__,port,vlanId);
    return L7_SUCCESS;
  }

  // Remove this entry
  memcpy(entry.macAddr,&fdbEntry.dot1dTpFdbAddress[2],sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  entry.vlanId    = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
  entry.intIfNum  = fdbEntry.dot1dTpFdbPort;
  entry.entryType = fdbEntry.dot1dTpFdbEntryType;
  if (fdbDelEntry(&entry)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error removing entry\n",__FUNCTION__,__LINE__);
    result = L7_FAILURE;
  }

  // The following piece of code, only applies to E-trees, and only if previous entry was static
  // If this port is a root port, remove the same entry but for all leaf vlans
  if (fdbEntry.dot1dTpFdbEntryType==L7_FDB_ADDR_FLAG_STATIC)
  {
    if (flow->port[port].type==MEF10_EVC_UNITYPE_ROOT)
    {
      for (p=0; p<L7_SYSTEM_N_INTERF; p++)
      {
        // Do nothing for the same port, and for non leaf ports
        if (p==port || flow->port[p].type!=MEF10_EVC_UNITYPE_LEAF)  continue;
  
        // Prepare key
        memcpy(&keyNext[0],&flow->vid_xlate[p],sizeof(L7_uint16));
        memcpy(&keyNext[2],mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  
        // if this entry is not found, or if is not static, or it belongs to another interface, do nothing
        if (fdbFind(keyNext,L7_MATCH_EXACT,&fdbEntry)!=L7_SUCCESS ||
            fdbEntry.dot1dTpFdbEntryType!=L7_FDB_ADDR_FLAG_STATIC ||
            fdbEntry.dot1dTpFdbPort!=intIfNum)
          continue;
  
        // Prepare entry remotion (only changes vlan and entry type)
        entry.vlanId    = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
        entry.intIfNum  = fdbEntry.dot1dTpFdbPort;
        entry.entryType = fdbEntry.dot1dTpFdbEntryType;
  
        // Remove this entry
        if (fdbDelEntry(&entry)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error removing entry (port=%u, vlan=%u)\n",__FUNCTION__,__LINE__,port,entry.vlanId);
          result = L7_FAILURE;
        }
      }
    }
    // If this port is a leaf port, remove the same entry but for the root vlan
    else if (flow->port[port].type==MEF10_EVC_UNITYPE_LEAF)
    {
      // Prepare key
      memcpy(&keyNext[0],&flow->vid_root,sizeof(L7_uint16));
      memcpy(&keyNext[2],mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  
      // if this entry is not found, or if is not static, or it belongs to another interface, do nothing
      if (fdbFind(keyNext,L7_MATCH_EXACT,&fdbEntry)==L7_SUCCESS &&
          fdbEntry.dot1dTpFdbEntryType==L7_FDB_ADDR_FLAG_STATIC &&
          fdbEntry.dot1dTpFdbPort==intIfNum)
      {    
        // Prepare entry remotion (only changes vlan and entry type)
        entry.vlanId    = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
        entry.intIfNum  = fdbEntry.dot1dTpFdbPort;
        entry.entryType = fdbEntry.dot1dTpFdbEntryType;
    
        // Remove this entry
        if (fdbDelEntry(&entry)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error removing entry (port=%u, vlan=%u)\n",__FUNCTION__,__LINE__,port,entry.vlanId);
          result = L7_FAILURE;
        }
      }
    }
  }
  
  return result;
}


L7_RC_t ptin_flow_activate_snooping(L7_int port, L7_uint16 flow_id, L7_uint16 vlan_root)
{
  L7_uint32 intIfNum;
  L7_uint16 p;
  L7_flow_struct *flow;
  L7_uint32 state;
  st_snooping_intf snoop_intf;
  st_snooping_vlan snoop_vlan;
  st_snooping_querier_vlan querier_vlan;
  L7_RC_t rc;

  // Validate arguments
  if ((port!=-1 && port>=L7_SYSTEM_N_INTERF) ||
      vlan_root<=L7_DOT1Q_MIN_VLAN_ID || vlan_root>L7_DOT1Q_MAX_VLAN_ID) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid parameters",__FUNCTION__);
    return L7_FAILURE;
  }
  // Validate flow
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: flow id out of range (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }
  flow = &flow_database[flow_id];
  if (!flow->inUse || flow->type!=FLOW_TYPE_MULTICAST)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid flow id (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }

  // Wait until snoop Queue is empty
  while (!snoopQueueEmpty())  usleep(10000);

  // Snooping interfaces processing
  for (p=0; p<L7_SYSTEM_N_INTERF; p++) {
    // Do nothing for non selected ports, and if interface type in evc is unused
    if ((port!=-1 && port!=p) ||
        flow->port_original[p].type==MEF10_EVC_UNITYPE_UNUSED)  {
      snoop_intf.intf_type[p]=PTIN_SNOOP_INTF_NONE;
    }
    else if (p>=igmp_firstRootPort) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Intf %u will be added as router",__FUNCTION__,p);
      snoop_intf.intf_type[p]=PTIN_SNOOP_INTF_ROUTER;
    }
    else  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Intf %u will be added as client",__FUNCTION__,p);
      snoop_intf.intf_type[p]=PTIN_SNOOP_INTF_CLIENT;
    }
    snoop_intf.intf_config[p].admin = L7_ENABLE;
    snoop_intf.intf_config[p].fastleave_admin_mode = L7_ENABLE;
    snoop_intf.intf_config[p].group_membership_interval = 260;
    snoop_intf.intf_config[p].max_response_time = 10;
    snoop_intf.intf_config[p].mrouter_present_expiration_time = 0;
  }
  if (ptin_msg_snooping_intf_config(&snoop_intf)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_intf_config",__FUNCTION__);
    return L7_FAILURE;
  }

  // Snooping vlans processing
  // Clients:
  for (p=0; p<L7_SYSTEM_N_INTERF; p++) {

    snoop_vlan.port.intf = p;
    if ((port!=-1 && port!=p) ||
        flow->port_original[p].type==MEF10_EVC_UNITYPE_UNUSED)  {
      snoop_vlan.port.type = PTIN_SNOOP_INTF_NONE;
    }
    else  {
      snoop_vlan.port.type = PTIN_SNOOP_INTF_CLIENT;
    }

    if (snoop_vlan.port.type!=PTIN_SNOOP_INTF_NONE) {
      snoop_vlan.snoop_vlan = flow->port_original[p].ceVid;
      snoop_vlan.snoop_vlan_xlate = vlan_root;
      snoop_vlan.snoop_vlan_config.admin = L7_ENABLE;
      snoop_vlan.snoop_vlan_config.fastleave_admin_mode = L7_ENABLE;
      snoop_vlan.snoop_vlan_config.group_membership_interval = 260;
      snoop_vlan.snoop_vlan_config.max_response_time = 10;
      snoop_vlan.snoop_vlan_config.mrouter_present_expiration_time = 0;

      // Determine intIfNum
      if (ptin_translate_port2intf(p,&intIfNum)!=L7_SUCCESS) intIfNum = 0;

      // Activate vlan snooping processing
      if (flow->port_original[p].ceVid>=2 && flow->port_original[p].ceVid<=4093 &&
          (rc=usmDbSnoopVlanAdminModeSet( 1, L7_TRUE, intIfNum, flow->port_original[p].ceVid, L7_AF_INET))!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopAdminModeSet for vlan=%u",__FUNCTION__,flow->port_original[p].ceVid);
        return rc;
      }

      if (ptin_msg_snooping_vlan_config(&snoop_vlan, 1)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_vlan_config",__FUNCTION__);
        return L7_FAILURE;
      }
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Vlan %u added as client",__FUNCTION__,snoop_vlan.snoop_vlan);
    }
  }
  // Routers:
  for (p=igmp_firstRootPort; p<L7_SYSTEM_N_INTERF; p++) {

    snoop_vlan.port.intf = p;
    if ((port!=-1 && port!=p) || 
        flow->port_original[p].type==MEF10_EVC_UNITYPE_UNUSED)  {
      snoop_vlan.port.type = PTIN_SNOOP_INTF_NONE;
    }
    else  {
      snoop_vlan.port.type = PTIN_SNOOP_INTF_ROUTER;
    }

    if (snoop_vlan.port.type!=PTIN_SNOOP_INTF_NONE) {
      snoop_vlan.snoop_vlan = flow->port_original[p].ceVid;
      snoop_vlan.snoop_vlan_config.admin = L7_ENABLE;
      
//    // Determine intIfNum
//    if (ptin_translate_port2intf(p,&intIfNum)!=L7_SUCCESS) intIfNum = 0;
//
//    // Activate vlan snooping processing
//    if (flow->vid_xlate[p]>=2 && flow->vid_xlate[p]<=4093 &&
//        usmDbSnoopAdminModeSet( 1, L7_TRUE, intIfNum, flow->port_original[p].ceVid, L7_AF_INET)!=L7_SUCCESS) {
//      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopAdminModeSet for vlan=%u",__FUNCTION__,flow->port_original[p].ceVid);
//      return L7_FAILURE;
//    }

      if (ptin_msg_snooping_vlan_config(&snoop_vlan, 1)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_vlan_config",__FUNCTION__);
        return L7_FAILURE;
      }
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Vlan %u added as router",__FUNCTION__,snoop_vlan.snoop_vlan);
    }
  }

  // Querier vlans processing
  for (p=igmp_firstRootPort; p<L7_SYSTEM_N_INTERF; p++) {

    if ((port==-1 || port==p) &&
        flow->port_original[p].type!=MEF10_EVC_UNITYPE_UNUSED)  {
      querier_vlan.querier_vlan_admin = L7_ENABLE;
      querier_vlan.querier_vlan = flow->port_original[p].ceVid;
      querier_vlan.querier_election_participate_mode = L7_ENABLE;
      querier_vlan.querier_ipaddress = 0x01020304;

      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Configuring querier with vlan %u",__FUNCTION__,querier_vlan.querier_vlan);
      if (ptin_msg_snooping_querier_vlan_config(&querier_vlan,1)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_querier_vlan_config",__FUNCTION__);
        return L7_FAILURE;
      }
      if (usmDbSnoopQuerierOperStateGet(querier_vlan.querier_vlan, &state, L7_AF_INET)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopQuerierOperStateGet",__FUNCTION__);
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_flow_deactivate_snooping(L7_int port, L7_uint16 flow_id)
{
  L7_uint32 intIfNum;
  L7_flow_struct *flow;
  L7_int p, p2, flow_index;
  st_snooping_intf snoop_intf;
  st_snooping_vlan snoop_vlan;
  st_snooping_querier_vlan querier_vlan;
  L7_RC_t rc;

  // Validate arguments
  if (port!=-1 && port>=L7_SYSTEM_N_INTERF) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid parameters",__FUNCTION__);
    return L7_FAILURE;
  }

  // Validate flow id
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: flow id out of range (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }

  flow = &flow_database[flow_id];
  if (!flow->inUse || flow->type!=FLOW_TYPE_MULTICAST)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid flow id (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }
  
  // Snooping interfaces processing
  for (p=0; p<L7_SYSTEM_N_INTERF; p++) {
    // Interface is router or client?
    if ((port!=-1 && port!=p) ||
        flow->port_original[p].type==MEF10_EVC_UNITYPE_UNUSED) {
      // Do not touch interfaces not included in this EVC
      snoop_intf.intf_type[p]=PTIN_SNOOP_INTF_NONE;
    }
    else if (p>=igmp_firstRootPort)  {
      snoop_intf.intf_type[p]=PTIN_SNOOP_INTF_ROUTER;
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: router port %u",__FUNCTION__,p);
    }
    else  {
      snoop_intf.intf_type[p]=PTIN_SNOOP_INTF_CLIENT;
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: client port %u",__FUNCTION__,p);
    }
    // Check if this interface is used in others multicast services,
    // And do nothing if is used
    for (flow_index=1; flow_index<FLOWS_MAX; flow_index++) {
      if (flow_id==flow_index || !flow_database[flow_index].inUse || flow_database[flow_index].type!=FLOW_TYPE_MULTICAST)
        continue;
      if (flow_database[flow_index].port_original[p].type!=MEF10_EVC_UNITYPE_UNUSED)  break;
    }
    if (flow_index<FLOWS_MAX)  {
      snoop_intf.intf_type[p]=PTIN_SNOOP_INTF_NONE;
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Nothing will be done for port %u",__FUNCTION__,p);
    }
    // Disabled
    snoop_intf.intf_config[p].admin = L7_DISABLE;
  }
  if (ptin_msg_snooping_intf_config(&snoop_intf)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_intf_config",__FUNCTION__);
    return L7_FAILURE;
  }

  // Snooping vlans processing
  // Clients:
  for (p=0; p<L7_SYSTEM_N_INTERF; p++) {
    // Selective processing
    if (port!=-1 && port!=p)  continue;

    snoop_vlan.port.intf = p;
    // Interface is router or client?
    if (flow->port_original[p].type==MEF10_EVC_UNITYPE_UNUSED)  {
      snoop_vlan.port.type = PTIN_SNOOP_INTF_NONE;
    }
    else  {
      snoop_vlan.port.type = PTIN_SNOOP_INTF_CLIENT;
    }

    if (snoop_vlan.port.type!=PTIN_SNOOP_INTF_NONE) {
      // Check if this vlan is used in others multicast services,
      // And do nothing if is used
      for (flow_index=1; flow_index<FLOWS_MAX; flow_index++) {
        if (flow_id==flow_index || !flow_database[flow_index].inUse || flow_database[flow_index].type!=FLOW_TYPE_MULTICAST)
          continue;
        for (p2=0; p2<L7_SYSTEM_N_INTERF; p2++) {
          if ((flow_id!=flow_index || p2!=p) &&
              flow_database[flow_index].port_original[p2].type!=MEF10_EVC_UNITYPE_UNUSED &&
              flow_database[flow_index].port_original[p2].ceVid==flow->port_original[p].ceVid)
            break;
        }
        if (p2<L7_SYSTEM_N_INTERF)
          break;
      }
      // If vlan is in use, do not touch it
      if (flow_index<FLOWS_MAX)  snoop_vlan.port.type = PTIN_SNOOP_INTF_NONE;
    }
    // Only delete vlan, if it isn't used in other MC services
    if (snoop_vlan.port.type!=PTIN_SNOOP_INTF_NONE) {
      snoop_vlan.snoop_vlan = flow->port_original[p].ceVid;
      snoop_vlan.snoop_vlan_config.admin = L7_DISABLE;

      // Determine intIfNum
      if (ptin_translate_port2intf(p,&intIfNum)!=L7_SUCCESS) intIfNum = 0;

      // Deactivate vlan snooping processing
      if (flow->port_original[p].ceVid>=2 && flow->port_original[p].ceVid<=4093 &&
          (rc=usmDbSnoopVlanAdminModeSet( 1, L7_FALSE, intIfNum, flow->port_original[p].ceVid, L7_AF_INET))!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopAdminModeSet for vlan=%u",__FUNCTION__,flow->port_original[p].ceVid);
        return rc;
      }

      if (ptin_msg_snooping_vlan_config(&snoop_vlan, 1)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_vlan_config",__FUNCTION__);
        return L7_FAILURE;
      }
    }
  }

  // Routers:
  for (p=igmp_firstRootPort; p<L7_SYSTEM_N_INTERF; p++) {
    // Selective processing
    if (port!=-1 && port!=p)  continue;

    snoop_vlan.port.intf = p;
    // Interface is router or client?
    if (flow->port_original[p].type==MEF10_EVC_UNITYPE_UNUSED)  {
      snoop_vlan.port.type = PTIN_SNOOP_INTF_NONE;
    }
    else  {
      snoop_vlan.port.type = PTIN_SNOOP_INTF_ROUTER;
    }

    if (snoop_vlan.port.type!=PTIN_SNOOP_INTF_NONE) {
      // Check if this vlan is used in others multicast services,
      // And do nothing if is used
      for (flow_index=1; flow_index<FLOWS_MAX; flow_index++) {
        if (/*flow_id==flow_index ||*/ !flow_database[flow_index].inUse || flow_database[flow_index].type!=FLOW_TYPE_MULTICAST)
          continue;
        for (p2=0; p2<L7_SYSTEM_N_INTERF; p2++) {
          if ((flow_id!=flow_index || p2!=p) &&
              flow_database[flow_index].port_original[p2].type!=MEF10_EVC_UNITYPE_UNUSED &&
              flow_database[flow_index].port_original[p2].ceVid==flow->port_original[p].ceVid)
            break;
        }
        if (p2<L7_SYSTEM_N_INTERF)
          break;
      }
      // If vlan is in use, do not touch it
      if (flow_index<FLOWS_MAX)  snoop_vlan.port.type = PTIN_SNOOP_INTF_NONE;
    }

    // Only delete vlan, if it isn't used in other MC services
    if (snoop_vlan.port.type!=PTIN_SNOOP_INTF_NONE) {
      snoop_vlan.snoop_vlan = flow->port_original[p].ceVid;
      snoop_vlan.snoop_vlan_config.admin = L7_DISABLE;

//    // Determine intIfNum
//    if (ptin_translate_port2intf(p,&intIfNum)!=L7_SUCCESS) intIfNum = 0;
//
//    // Deactivate vlan snooping processing
//    if (flow->port_original[p].ceVid>=2 && flow->port_original[p].ceVid<=4093 &&
//        usmDbSnoopAdminModeSet( 1, L7_FALSE, intIfNum, flow->port_original[p].ceVid, L7_AF_INET)!=L7_SUCCESS) {
//      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopAdminModeSet for vlan=%u",__FUNCTION__,flow->port_original[p].ceVid);
//      return L7_FAILURE;
//    }

      if (ptin_msg_snooping_vlan_config(&snoop_vlan, 1)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_vlan_config",__FUNCTION__);
        return L7_FAILURE;
      }
    }
  }

  // Querier vlans processing
  for (p=igmp_firstRootPort; p<L7_SYSTEM_N_INTERF; p++) {
    // Selective processing
    if (port!=-1 && port!=p)  continue;

    if (flow->port_original[p].type!=MEF10_EVC_UNITYPE_UNUSED)  {
      // Check if the client vlans isn't used in other MC services
      for (flow_index=1; flow_index<FLOWS_MAX; flow_index++) {
        if (/*flow_id==flow_index ||*/ !flow_database[flow_index].inUse || flow_database[flow_index].type!=FLOW_TYPE_MULTICAST)
          continue;
        for (p2=igmp_firstRootPort; p2<L7_SYSTEM_N_INTERF; p2++) {
          if ((flow_id!=flow_index || p2!=p) &&
              flow_database[flow_index].port_original[p2].type!=MEF10_EVC_UNITYPE_UNUSED &&
              flow_database[flow_index].port_original[p2].ceVid==flow->port_original[p].ceVid)
            break;
        }
        if (p2<L7_SYSTEM_N_INTERF)
          break;
      }
      // Only disable this vlan if it isn't used in other MC services
      if (flow_index>=FLOWS_MAX) {
        querier_vlan.querier_vlan_admin = L7_DISABLE;
        querier_vlan.querier_vlan = flow->port_original[p].ceVid;

        if (ptin_msg_snooping_querier_vlan_config(&querier_vlan,1)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_querier_vlan_config",__FUNCTION__);
          return L7_FAILURE;
        }
      }
    }
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_flow_update_snooping(st_snooping *snoop)
{
  L7_int p, flow_index;
  L7_uint32 intf;
  st_snooping_querier_vlan querier_vlan;
  L7_RC_t rc=L7_SUCCESS;

  // Validate arguments
  if (snoop==L7_NULLPTR || snoop->querier_ipaddress==0) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: Invalid arguments",__FUNCTION__);
    return L7_FAILURE;
  }

  // Run all flows, and router ports
  for (flow_index=1; flow_index<FLOWS_MAX; flow_index++)  {

    // Update group membership intervals
    for (p=0; p<igmp_firstRootPort; p++) {
      if (flow_database[flow_index].inUse &&
          flow_database[flow_index].type==FLOW_TYPE_MULTICAST &&
          flow_database[flow_index].port_original[p].type!=MEF10_EVC_UNITYPE_UNUSED)
      {
        // Update group membership interval
        // Interface membership
        if (ptin_translate_port2intf(p,&intf)==L7_SUCCESS && intf>0 &&
            usmDbSnoopIntfGroupMembershipIntervalSet(1,intf,snoop->group_membership_interval,L7_AF_INET)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopIntfGroupMembershipIntervalSet (flow_id=%u, port=%u, intf=%u)",
                      __FUNCTION__,flow_index,p,intf);
        }
        // Vlan membership
        if (flow_database[flow_index].port_original[p].ceVid>=2 && flow_database[flow_index].port_original[p].ceVid<=4093 &&
            usmDbSnoopVlanGroupMembershipIntervalSet(1,flow_database[flow_index].port_original[p].ceVid,snoop->group_membership_interval,L7_AF_INET)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopVlanGroupMembershipIntervalSet (flow_id=%u, port=%u, vlan=%u)",
                      __FUNCTION__,flow_index,p,flow_database[flow_index].port_original[p].ceVid);
        }
      }
    }

    // Update querier data
    for (p=igmp_firstRootPort; p<L7_SYSTEM_N_INTERF; p++) {
      // If flow is created, and is a Multicast flow, and the router port is in use, process it
      if (flow_database[flow_index].inUse &&
          flow_database[flow_index].type==FLOW_TYPE_MULTICAST &&
          flow_database[flow_index].port_original[p].type!=MEF10_EVC_UNITYPE_UNUSED)
      {
        // Vlan membership
        if (flow_database[flow_index].port_original[p].ceVid>=2 && flow_database[flow_index].port_original[p].ceVid<=4093 &&
            usmDbSnoopVlanGroupMembershipIntervalSet(1,flow_database[flow_index].port_original[p].ceVid,snoop->group_membership_interval,L7_AF_INET)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using usmDbSnoopVlanGroupMembershipIntervalSet (flow_id=%u, port=%u, vlan=%u)",
                      __FUNCTION__,flow_index,p,flow_database[flow_index].port_original[p].ceVid);
        }

        // Extract the vlan
        querier_vlan.querier_vlan = flow_database[flow_index].port_original[p].ceVid;
  
        // Read the current querier configurations
        if (ptin_msg_snooping_querier_vlan_getconfig(&querier_vlan,1)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_querier_vlan_getconfig (flow_id=%u, port=%u, vlan=%u)",
                      __FUNCTION__,flow_index,p,querier_vlan.querier_vlan);
          rc = L7_FAILURE;
          continue;
        }
        // Change the ip address
        querier_vlan.querier_ipaddress = snoop->querier_ipaddress;
        // And apply the new ip address
        if (ptin_msg_snooping_querier_vlan_config(&querier_vlan,1)!=L7_SUCCESS)  {
           DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error using ptin_msg_snooping_querier_vlan_config (flow_id=%u, port=%u, vlan=%u)",
                       __FUNCTION__,flow_index,p,querier_vlan.querier_vlan);
           rc = L7_FAILURE;
           continue;
        }
      }
    }
  }
  return rc;
}


L7_RC_t ptin_flow_activate_dhcpOp82(L7_int port, L7_uint16 flow_id)
{
  L7_uint16 p;
  L7_uint32 intIfNum;
  L7_flow_struct *flow;
  L7_RC_t rc, result=L7_SUCCESS;

  // Validate port
  if (port!=-1 && port>=L7_SYSTEM_N_INTERF)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid port (%d)",__FUNCTION__,port);
    return L7_FAILURE;
  }
  // Validate flow
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: flow id out of range (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }
  flow = &flow_database[flow_id];
  if (!flow->inUse)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid flow id (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }
  if (!flow->dhcp_enable) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: DHCP is not enable in this EVC (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }

  if (usmDbDsAdminModeSet(L7_ENABLE)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsAdminModeSet",__FUNCTION__);
    return L7_FAILURE;
  }
  if (usmDbDsL2RelayAdminModeSet(L7_ENABLE)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayAdminModeSet",__FUNCTION__);
    return L7_FAILURE;
  }

  for (p=0; p<L7_SYSTEM_N_INTERF; p++) {
    // Selective processing
    if (port!=-1 && port!=p)  continue;

    if (flow->port[p].type==MEF10_EVC_UNITYPE_UNUSED || flow->port[p].ceVid<=L7_DOT1Q_MIN_VLAN_ID || flow->port[p].ceVid>L7_DOT1Q_MAX_VLAN_ID)
      continue;

    if (ptin_translate_port2intf(p, &intIfNum)!=L7_SUCCESS || intIfNum==0)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid interface (port=%u)",__FUNCTION__,p);
      continue;
    }

    if (p<igmp_firstRootPort) {
      if (usmDbDsL2RelayCircuitIdSet(flow->port[p].ceVid,flow->port[p].ceVid,L7_ENABLE)!=L7_SUCCESS)
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayVlanModeSet(%u,...,ENABLE)",__FUNCTION__,flow->port[p].ceVid);
    }

    if (usmDbDsL2RelayIntfModeSet(intIfNum,L7_ENABLE)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayCircuitIdSet(%u,...,ENABLE)",__FUNCTION__,p);
    }
    if (p>=igmp_firstRootPort) {
      if (usmDbDsIntfTrustSet(intIfNum,L7_TRUE)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsIntfTrustSet(%u,TRUE)",__FUNCTION__,p);
      }
      if (usmDbDsL2RelayIntfTrustSet(intIfNum,L7_TRUE)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayIntfTrustSet(%u,TRUE)",__FUNCTION__,p);
      }
    }
    if (usmDbDsVlanConfigSet(flow->port[p].ceVid,flow->port[p].ceVid,L7_ENABLE)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsVlanConfigSet(%u,...,ENABLE)",__FUNCTION__,flow->port[p].ceVid);
    }
    if (usmDbDsL2RelayVlanModeSet(flow->port[p].ceVid,flow->port[p].ceVid,L7_ENABLE)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayVlanModeSet(%u,...,ENABLE)",__FUNCTION__,flow->port[p].ceVid);
    }
    // Add this vlan to packet filtering
    if ((rc=usmDbDsVlanFilterAdmin(L7_TRUE,intIfNum,flow->vid_xlate[p] /*flow->port[p].ceVid*/))!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsVlanFilterAdmin(L7_TRUE,%u)",__FUNCTION__,flow->vid_xlate[p]);
      result = rc;
    }
  }

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: DHCP configured successfully",__FUNCTION__);

  return result;
}

L7_RC_t ptin_flow_deactivate_dhcpOp82(L7_int port, L7_uint16 flow_id)
{
  L7_uint16 p, flow_index, p2;
  L7_uint32 intIfNum;
  L7_flow_struct *flow;
  L7_RC_t rc, result=L7_SUCCESS;

  // Validate port
  if (port!=-1 && port>=L7_SYSTEM_N_INTERF)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid port (%d)",__FUNCTION__,port);
    return L7_FAILURE;
  }
  // Validate flow
  if (flow_id>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: flow id out of range (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }
  flow = &flow_database[flow_id];
  if (!flow->inUse)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid flow id (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }
  if (!flow->dhcp_enable) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: DHCP is not enable in this EVC (%d)",__FUNCTION__,flow_id);
    return L7_FAILURE;
  }

  // Clear DHCP Binding table
  if (usmDbDsBindingClear(0)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Can't clear binding table",__FUNCTION__);
    return L7_FAILURE;
  }

  for (p=0; p<L7_SYSTEM_N_INTERF; p++) {
    // Selective processing
    if (port!=-1 && port!=p)  continue;

    if (flow->port[p].type==MEF10_EVC_UNITYPE_UNUSED || flow->port[p].ceVid<=L7_DOT1Q_MIN_VLAN_ID || flow->port[p].ceVid>L7_DOT1Q_MAX_VLAN_ID)
      continue;

    if (ptin_translate_port2intf(p, &intIfNum)!=L7_SUCCESS || intIfNum==0)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Invalid interface (port=%u)",__FUNCTION__,p);
      continue;
    }

    // Is this vlan being used in other EVCs?
    for (flow_index=1; flow_index<FLOWS_MAX; flow_index++) {
      if (/*flow_index==flow_id ||*/ !flow_database[flow_index].inUse || !flow_database[flow_index].dhcp_enable)  continue;
      for (p2=0; p2<L7_SYSTEM_N_INTERF; p2++) {
        if ((flow_index!=flow_id || p2!=p) &&
            flow_database[flow_index].port[p2].type!=MEF10_EVC_UNITYPE_UNUSED && flow_database[flow_index].port[p2].ceVid==flow->port[p].ceVid)
          break;
      }
      if (p2<L7_SYSTEM_N_INTERF) break;
    }
    // Only in the case where this vlan is not used anywhere else, delete it
    if (flow_index>=FLOWS_MAX) {
      if (p<igmp_firstRootPort) {
        if (usmDbDsL2RelayCircuitIdSet(flow->port[p].ceVid,flow->port[p].ceVid,L7_DISABLE)!=L7_SUCCESS)
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayVlanModeSet(%u,...,DISABLE)",__FUNCTION__,flow->port[p].ceVid);
      }
      if (usmDbDsL2RelayVlanModeSet(flow->port[p].ceVid,flow->port[p].ceVid,L7_DISABLE)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayVlanModeSet(%u,...,DISABLE)",__FUNCTION__,flow->port[p].ceVid);
      }
      if (usmDbDsVlanConfigSet(flow->port[p].ceVid,flow->port[p].ceVid,L7_DISABLE)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsVlanConfigSet(%u,...,DISABLE)",__FUNCTION__,flow->port[p].ceVid);
      }
    }

    // Is this vlan being used in other EVCs?
    for (flow_index=1; flow_index<FLOWS_MAX; flow_index++) {
      if (flow_index==flow_id || !flow_database[flow_index].inUse || !flow_database[flow_index].dhcp_enable)  continue;
      for (p2=0; p2<L7_SYSTEM_N_INTERF; p2++) {
        if (flow_database[flow_index].port[p2].type!=MEF10_EVC_UNITYPE_UNUSED && p2==p)
          break;
      }
      if (p2<L7_SYSTEM_N_INTERF) break;
    }
    if (flow_index>=FLOWS_MAX) {
      if (p>=igmp_firstRootPort) {
        if (usmDbDsL2RelayIntfTrustSet(intIfNum,L7_FALSE)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayIntfTrustSet(%u,FALSE)",__FUNCTION__,p);
        }
        if (usmDbDsIntfTrustSet(intIfNum,L7_FALSE)!=L7_SUCCESS)  {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsIntfTrustSet(%u,FALSE)",__FUNCTION__,p);
        }
      }
      if (usmDbDsL2RelayIntfModeSet(intIfNum,L7_DISABLE)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayIntfModeSet(%u,DISABLE)",__FUNCTION__,p);
      }
    }

    // Check if there is one EVC with DHCP enabled
    for (flow_index=1; flow_index<FLOWS_MAX; flow_index++) {
      if (flow_index==flow_id || !flow_database[flow_index].inUse)  continue;
      if (flow_database[flow_index].dhcp_enable)  break;
    }
    if (flow_index>=FLOWS_MAX) {
      if (usmDbDsL2RelayAdminModeSet(L7_DISABLE)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsL2RelayAdminModeSet",__FUNCTION__);
        return L7_FAILURE;
      }
      if (usmDbDsAdminModeSet(L7_DISABLE)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsAdminModeSet",__FUNCTION__);
        return L7_FAILURE;
      }
    }
    // Remove this vlan from packet filtering
    if ((rc=usmDbDsVlanFilterAdmin(L7_FALSE,intIfNum,flow->vid_xlate[p] /*flow->port[p].ceVid*/))!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Error with usmDbDsVlanFilterAdmin(L7_FALSE,%u)",__FUNCTION__,flow->vid_xlate[p]);
      result = rc;
    }
  }

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: DHCP removed successfully",__FUNCTION__);

  return result;
}


L7_RC_t ptin_flow_lag_port_add(L7_uint8 lag_port, L7_uint8 port)
{
  L7_uint16 flow_id, i, lag_vlan=1;
  L7_flow_struct *flow;
  L7_RC_t rc;

  // Validate lag port
  if (lag_port<L7_SYSTEM_N_PORTS || lag_port>=L7_SYSTEM_N_INTERF ||
      port<igmp_firstRootPort || port>=L7_SYSTEM_N_PORTS)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid port (lag_port=%u, port=%u)", __FUNCTION__, __LINE__,lag_port,port);
    return L7_FAILURE;
  }

  // Run all EVCs
  for (flow_id=1; flow_id<FLOWS_MAX; flow_id++) {
    // If EVC is not in use, skip to the next one
    if (!flow_database[flow_id].inUse)  continue;

    flow = &flow_database[flow_id];

    // Lag interface must be in use
    if (flow->port[lag_port].type==MEF10_EVC_UNITYPE_UNUSED ||
        flow->port[lag_port].ceVid==0 || flow->port[lag_port].ceVid>4095 ||
        flow->vid_xlate[lag_port]==0 || flow->vid_xlate[lag_port]>4095)
      continue;

    // Save lag vlan
    lag_vlan = flow->port[lag_port].ceVid;

    // Add ingress translation entry
    if ((rc=ptin_vlan_translate_add(port,lag_vlan,flow->vid_xlate[lag_port]))!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) error using ptin_vlan_translate_add(%u,%u,%u)\n",__FUNCTION__,__LINE__,port,lag_vlan,flow->vid_xlate[lag_port]);
      return rc;
    }

    // Add egress translation entry
    if ((rc=ptin_vlan_translate_egress_add(port,flow->vid_xlate[lag_port],lag_vlan))!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) error using ptin_vlan_translate_add(%u,%u,%u)\n",__FUNCTION__,__LINE__,port,flow->vid_xlate[lag_port],lag_vlan);
      return rc;
    }

    // Add egress translation entries for leaf vlans

    // Run all possible leafs
    for (i=0; i<igmp_firstRootPort; i++)
    {
      // Only for active leafs
      if (flow->port[i].type==MEF10_EVC_UNITYPE_LEAF &&
          flow->vid_xlate[i]>0 && flow->vid_xlate[i]<4096)
      {
        if ((rc=ptin_vlan_translate_egress_add(port,flow->vid_xlate[i],lag_vlan))!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) error using ptin_vlan_translate_add(%u,%u,%u)\n",__FUNCTION__,__LINE__,port,flow->vid_xlate[i],lag_vlan);
          return rc;
        }
      }
    }
  }
  
  return L7_SUCCESS;
}

L7_RC_t ptin_flow_lag_port_delete(L7_uint8 lag_port, L7_uint8 port)
{
  L7_uint16 flow_id, i, lag_vlan=1;
  L7_flow_struct *flow;

  // Validate lag port
  if (lag_port<L7_SYSTEM_N_PORTS || lag_port>=L7_SYSTEM_N_INTERF ||
      port<igmp_firstRootPort || port>=L7_SYSTEM_N_PORTS)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid port (lag_port=%u, port=%u)", __FUNCTION__, __LINE__,lag_port,port);
    return L7_FAILURE;
  }

  // Run all EVCs
  for (flow_id=1; flow_id<FLOWS_MAX; flow_id++) {
    // If EVC is not in use, skip to the next one
    if (!flow_database[flow_id].inUse)  continue;

    flow = &flow_database[flow_id];

    // Lag interface must be in use
    if (flow->port[lag_port].type==MEF10_EVC_UNITYPE_UNUSED ||
        flow->port[lag_port].ceVid==0 || flow->port[lag_port].ceVid>4095 ||
        flow->vid_xlate[lag_port]==0 || flow->vid_xlate[lag_port]>4095)
      continue;

    // Save lag vlan
    lag_vlan = flow->port[lag_port].ceVid;

    // Remove ingress translation entry
    if (ptin_vlan_translate_delete(port,lag_vlan)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) error using ptin_vlan_translate_delete(%u,%u)\n",__FUNCTION__,__LINE__,port,lag_vlan);
      return L7_FAILURE;
    }

    // Remove egress translation entry
    if (ptin_vlan_translate_egress_delete(port,flow->vid_xlate[lag_port])!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) error using ptin_vlan_translate_delete(%u,%u)\n",__FUNCTION__,__LINE__,port,flow->vid_xlate[lag_port]);
      return L7_FAILURE;
    }

    // Remove egress translation entries for leaf vlans

    // Run all possible leafs
    for (i=0; i<igmp_firstRootPort; i++)
    {
      // Only for active leafs
      if (flow->port[i].type==MEF10_EVC_UNITYPE_LEAF &&
          flow->vid_xlate[i]>0 && flow->vid_xlate[i]<4096)
      {
        if (ptin_vlan_translate_egress_delete(port,flow->vid_xlate[i])!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) error using ptin_vlan_translate_delete(%u,%u)\n",__FUNCTION__,__LINE__,port,flow->vid_xlate[i]);
          return L7_FAILURE;
        }
      }
    }
  }
  
  return L7_SUCCESS;
}

// This function, is responsible for correcting the EVC information,
// in order to remove the physical ports, which are part of lags,
// and add the correspondent lags.
//
static L7_RC_t ptin_flow_validate2(L7_HwEthernetMef10CeVidMap *uni)
{
  L7_uint8 lag_port;
  L7_uint16 p;
  L7_uint32 port_bmp, lag_bmp, and_bmp;

  if (uni==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Get physical port bitmap
  // Discard PON ports as possible lag part
  port_bmp = 0;
  for (p=igmp_firstRootPort; p<L7_SYSTEM_N_PORTS; p++) {
    if (uni[p].type!=MEF10_EVC_UNITYPE_UNUSED)
      port_bmp |= (L7_uint32) 1<<p;
  }

  // Run all lags, and check if there are physical ports part of each lag
  for (lag_port=L7_SYSTEM_N_PORTS; lag_port<L7_SYSTEM_N_INTERF; lag_port++) {
    if (ptin_lag_memberList_get(lag_port, &lag_bmp)!=L7_SUCCESS)  continue;

    // No matching port situation
    if ((and_bmp=(port_bmp & lag_bmp))==0)  continue;

    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) We have a port match: 0x%08X and 0x%08X = 0x%08X", __FUNCTION__, __LINE__,port_bmp,lag_bmp,and_bmp);

    // Run all ports, to find the matching ports
    and_bmp>>=igmp_firstRootPort;
    for (p=igmp_firstRootPort; p<L7_SYSTEM_N_PORTS; p++,and_bmp>>=1) {
      // If no match port, skip
      if (!(and_bmp & 1))  continue;

      // Activate Lag port, with the same information as the physical port
      uni[lag_port].type  = uni[p].type;
      uni[lag_port].ceVid = uni[p].ceVid;

      // Deactivate the physical port
      uni[p].type  = MEF10_EVC_UNITYPE_UNUSED;
      uni[p].ceVid = (L7_uint16)-1;
    }
  }

  return L7_SUCCESS;
}

/* LAG utils */
L7_BOOL ptin_flow_port_exists(L7_int port)
{
  L7_uint16 flow_id;
  L7_uint32 intIfNum;
  L7_flow_struct *flow;

  // Validate port
  if (port>=L7_SYSTEM_N_INTERF) {
    return L7_FALSE;
  }
  // If Lag, it should exist
  else if (port>=L7_SYSTEM_N_PORTS) {
    if (ptin_translate_port2intf(port,&intIfNum)!=L7_SUCCESS || intIfNum==0 ||
        !usmDbDot3adValidIntfCheck(1,intIfNum))
      return L7_FALSE;
  }
  
  // Run all EVCs
  for (flow_id=1; flow_id<FLOWS_MAX; flow_id++) {
    // If EVC is not in use, skip to the next one
    if (!flow_database[flow_id].inUse)  continue;

    flow = &flow_database[flow_id];

    if (flow->port[port].type!=MEF10_EVC_UNITYPE_UNUSED)
      return L7_TRUE;
  }

  return L7_FALSE;
}
/* LAG utils - end */

/* InBand management */
L7_BOOL ptin_flow_inBand_exists(L7_uint16 *vlanId)
{
  L7_int p;

  if (flow_database[0].inUse)
  {
    // Find vlan (original)
    for (p=0; p<L7_SYSTEM_N_INTERF && flow_database[0].port[p].type==MEF10_EVC_UNITYPE_UNUSED; p++);
    if (p>=L7_SYSTEM_N_INTERF || flow_database[0].port[p].ceVid==0)
      return L7_FALSE;

    if (vlanId!=L7_NULLPTR) *vlanId=flow_database[0].port[p].ceVid;
    return L7_TRUE;
  }

  return L7_FALSE;
}

L7_RC_t ptin_flow_create_inBand(L7_uint16 vlanId)
{
  L7_int p;
  L7_HwEthernetMef10EvcBundling evc;
  L7_RC_t rc;

  // Add EVC 0
  evc.err_code = 0;
  evc.index    = 0;      // EVC 0
  evc.admin    = L7_TRUE;
  evc.type     = FLOW_TYPE_UNICAST;
  evc.bundling = 0;
  evc.outerTag = 0;
  // PON ports are not to be used
  for (p=0; p<L7_SYSTEM_N_PORTS; p++) {
    evc.uni[p].type  = MEF10_EVC_UNITYPE_UNUSED;
    evc.uni[p].ceVid = (L7_uint16) -1;
  }
  // All external Eth ports to be grouped
  for (p=igmp_firstRootPort; p<L7_SYSTEM_N_PORTS; p++) {
    evc.uni[p].type  = MEF10_EVC_UNITYPE_ROOT;
    evc.uni[p].ceVid = vlanId;
  }
  if ((rc=ptin_flow_create(&evc))!=L7_SUCCESS)
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error creating EVC 0",__FUNCTION__,__LINE__);
    return rc;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_flow_destroy_inBand(void)
{
  return ptin_flow_kill(0);
}
/* InBand management - end */


static L7_BOOL ptin_flow_validate(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port)
{
  L7_int p, id;

  if (flow_id>=FLOWS_MAX || port==L7_NULLPTR)
  {
    return L7_FALSE;
  }

  /* Run all ports */
  for (p = 0; p < L7_SYSTEM_N_INTERF; p++)
  {
    if (port[p].type != MEF10_EVC_UNITYPE_ROOT && port[p].type != MEF10_EVC_UNITYPE_LEAF)
      continue;

    /* Validate VLAN */
    if (port[p].ceVid == 0 || port[p].ceVid >= 4096)
    {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid vlan %u at port %u",__FUNCTION__,__LINE__,
                  port[p].ceVid, p);
      return L7_FALSE;
    }

    /* Check if VLAN already exists */
    id = EVCid_table[port[p].ceVid][p];
    if (id >= 0 && id < FLOWS_MAX && id != flow_id)
    {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Flow %u already has vlan %u at port %u",__FUNCTION__,__LINE__,
                  id, port[p].ceVid, p);
      return L7_FALSE;
    }
  }

  return L7_TRUE;
}


static L7_BOOL ptin_flow_create_is_possible(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *port)
{
  L7_uint32 port_bmp;
  L7_flow_struct *flow;
  L7_int p, i, n_ports, n_roots[2], n_leafs[2];
  L7_int xlate_entries_ing, xlate_entries_egr;
  st_ptin_hw_resources resources;

  if (flow_id>=FLOWS_MAX || port==L7_NULLPTR)
  {
    return L7_FALSE;
  }

  flow = &flow_database[flow_id];

  n_roots[0] = n_roots[1] = 0;
  n_leafs[0] = n_leafs[1] = 0;

  /* Run all ports */
  for (p=0; p<L7_SYSTEM_N_INTERF; p++)
  {
    /* Number of physical ports */
    if (p<L7_SYSTEM_N_PORTS)  n_ports = 1;
    else if (ptin_lag_memberList_get(p,&port_bmp)==L7_SUCCESS)
    {
      n_ports = 0;
      for (i=0; i<(sizeof(L7_uint32)*8) && port_bmp!=0; i++,port_bmp>>=1)
      {
        if (port_bmp & 1)  n_ports++;
      }
      if (n_ports==0)  continue;
      //printf("intf %u has %u members\r\n",p,n_ports);
    }
    else  continue;

    /* Source port structure */
    if (flow->inUse)
    {
      if (flow->port[p].type==MEF10_EVC_UNITYPE_ROOT)
        n_roots[0]+=n_ports;
      else if (flow->port[p].type==MEF10_EVC_UNITYPE_LEAF)
        n_leafs[0]+=n_ports;
    }

    /* Final port structure */
    if (port[p].type==MEF10_EVC_UNITYPE_ROOT)
      n_roots[1]+=n_ports;
    else if (port[p].type==MEF10_EVC_UNITYPE_LEAF)
      n_leafs[1]+=n_ports;
  }

  /* Necessary ingress and egress entries */
  xlate_entries_ing = (n_roots[1]-n_roots[0]) + (n_leafs[1]-n_leafs[0]);
  xlate_entries_egr = xlate_entries_ing + ((n_roots[1]*n_leafs[1])-(n_roots[0]*n_leafs[0]));

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) %+d ingress translations",__FUNCTION__,__LINE__,xlate_entries_ing);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) %+d eggress translations",__FUNCTION__,__LINE__,xlate_entries_egr);

  /* Only consider positive values */
  if (xlate_entries_ing<0)  xlate_entries_ing=0;
  if (xlate_entries_egr<0)  xlate_entries_egr=0;

  memset(&resources,0x00,sizeof(st_ptin_hw_resources));
  resources.vlanXlate_available_entries.ing_stag = xlate_entries_ing;
  resources.vlanXlate_available_entries.egr_stag = xlate_entries_egr;

  return ptin_flow_resources_check(&resources);
}

/* Check available resources */
L7_BOOL ptin_flow_resources_check(st_ptin_hw_resources *necessary_resources)
{
  st_ptin_hw_resources available_resources;

  /* Get available resources */
  if (dtlPtinHwResources(&available_resources)!=L7_SUCCESS)
  {
    return L7_FALSE;
  }

  /* Evaluate if available resources are enough */
  if ( (available_resources.vlanXlate_available_entries.ing_stag <= necessary_resources->vlanXlate_available_entries.ing_stag) ||
       (available_resources.vlanXlate_available_entries.ing_dtag <= necessary_resources->vlanXlate_available_entries.ing_dtag) ||
       (available_resources.vlanXlate_available_entries.egr_stag <= necessary_resources->vlanXlate_available_entries.egr_stag) ||
       (available_resources.vlans_available.bcastLim < necessary_resources->vlans_available.bcastLim) ||
       (available_resources.vlans_available.igmp     < necessary_resources->vlans_available.igmp    ) ||
       (available_resources.vlans_available.dhcp     < necessary_resources->vlans_available.dhcp    ) ||
       (available_resources.flowCounters_available_entries < necessary_resources->flowCounters_available_entries) )
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}


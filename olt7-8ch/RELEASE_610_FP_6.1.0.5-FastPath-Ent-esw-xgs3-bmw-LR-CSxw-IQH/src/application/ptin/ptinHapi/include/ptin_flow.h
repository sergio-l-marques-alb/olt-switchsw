#ifndef _PTIN_FLOW__H
#define _PTIN_FLOW__H

#include "datatypes.h"
#include "dtlinclude.h"
#include "ptin_translate_api.h"

#define FLOWS_MAX     512

#define FLOW_TYPE_UNICAST   0
#define FLOW_TYPE_BITSTREAM_WITH_MACLEARN 10
#define FLOW_TYPE_MULTICAST 11
#define FLOW_TYPE_UNIVOIP   12
#define FLOW_TYPE_BITSTREAM 13

#define MEF10_EVC_UNITYPE_ROOT    0
#define MEF10_EVC_UNITYPE_LEAF    1
#define MEF10_EVC_UNITYPE_UNUSED ((L7_uint8) -1)

extern L7_int igmp_firstRootPort;

extern void ptin_flow_init(void);

extern L7_RC_t ptin_flow_getVlan(L7_uint8 port, L7_uint16 flowId, L7_uint16 *vlanId);

extern L7_RC_t ptin_flow_get_originalVlan(L7_uint8 port, L7_uint16 flowId, L7_uint16 *vlanId);

extern L7_RC_t ptin_flow_get_internalVlan(L7_uint8 port, L7_uint16 flowId, L7_uint16 *vlanId);

extern L7_RC_t ptin_flow_getId(L7_uint8 port, L7_uint16 vlan, L7_uint16 *flow_id);

extern L7_RC_t ptin_flow_getId_fromInternalVlan(L7_uint8 port, L7_uint16 vlan, L7_uint16 *flow_id);

extern L7_RC_t ptin_flow_getType(L7_uint16 flow_id, L7_uint8 *type);

extern L7_RC_t ptin_flow_inUse(L7_uint16 flow_id, L7_BOOL *inUse);

extern L7_RC_t ptin_flow_getPortVlanList(L7_uint16 flow_id, L7_HwEthernetMef10CeVidMap *plist);

extern L7_RC_t ptin_flow_read(L7_HwEthernetMef10EvcBundling *evcCfg);

extern L7_RC_t ptin_flow_create(L7_HwEthernetMef10EvcBundling *evcCfg);

extern L7_RC_t ptin_flow_kill(L7_uint16 flow_id);

extern L7_RC_t ptin_flow_reset(void);

extern L7_RC_t ptin_flow_get_profile(L7_uint16 flow_id, L7_uint8 port, L7_bw_profile *bw_profile, L7_uint16 *nProfiles);

extern L7_RC_t ptin_flow_set_profile(L7_uint16 flow_id, L7_uint8 port, L7_bw_profile *bw_profile);

extern L7_RC_t ptin_flow_remove_profile(L7_uint16 flow_id, L7_uint8 port, L7_bw_profile *bw_profile);

extern L7_RC_t ptin_readVlanCounters(L7_uint16 FlowId, L7_uint16 svid, L7_HWEth_VlanStat *counters, L7_uint16 *nClients);

/*
extern L7_RC_t dtlPTinL2Callback(DAPI_USP_t *ddusp, 
                          DAPI_FAMILY_t family, 
                          DAPI_CMD_t cmd, 
                          DAPI_EVENT_t event,
                          void *dapiEventInfo);
*/
extern L7_RC_t ptinL2Process(L7_uint8 event, L7_uint32 intIfNum, L7_uint16 vid, L7_uchar8 *macAddr, L7_uint8 flags);


extern L7_RC_t ptin_flow_L2StaticEntry_add(L7_uint16 flow_id, L7_uint8 port, L7_uint8 *mac_address);

extern L7_RC_t ptin_flow_L2StaticEntry_delete(L7_uint16 flow_id, L7_uint8 port, L7_uint8 *mac_address);

extern L7_RC_t ptin_flow_update_snooping(st_snooping *snoop);

extern L7_RC_t ptin_flow_activate_dhcpOp82(L7_int port, L7_uint16 flow_id);

extern L7_RC_t ptin_flow_deactivate_dhcpOp82(L7_int port, L7_uint16 flow_id);

/* LAG utils */
extern L7_BOOL ptin_flow_port_exists(L7_int port);
extern L7_RC_t ptin_flow_lag_port_add(L7_uint8 lag_port, L7_uint8 port);
extern L7_RC_t ptin_flow_lag_port_delete(L7_uint8 lag_port, L7_uint8 port);
/* LAG utils - end */

/* InBand management */
extern L7_BOOL ptin_flow_inBand_exists(L7_uint16 *vlanId);
extern L7_RC_t ptin_flow_create_inBand(L7_uint16 vlanId);
extern L7_RC_t ptin_flow_destroy_inBand(void);
/* InBand management - end*/

extern L7_BOOL ptin_flow_resources_check(st_ptin_hw_resources *necessary_resources);

#endif // _PTIN_FLOW__H

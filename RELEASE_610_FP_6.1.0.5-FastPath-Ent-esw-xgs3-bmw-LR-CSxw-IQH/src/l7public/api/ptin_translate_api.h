#ifndef _PTIN_TRANSLATE_API__H
#define _PTIN_TRANSLATE_API__H

#include "datatypes.h"

// Table with vlans to use in multicast channels
extern L7_uint32 snoop_mcast_vlan[4096];        /* UniVlan to MCVlan*/
extern L7_uint32 snoop_mcast_vlan_inv[4096];    /* MCVlan to UniVlan*/

extern L7_RC_t usmDbSnoopQuerierVlanXlateGet(L7_uint32 vlanId, L7_uint32 *xlateVlanId);
extern L7_RC_t usmDbSnoopQuerierVlanXlateSet(L7_uint32 vlanId, L7_uint32 xlateVlanId);
extern L7_RC_t ptin_translate_dhcpVlanGet(L7_uint32 src_intf, L7_uint16 src_vlanId, L7_uint32 dst_intf, L7_uint16 *dst_vlanId);

extern L7_RC_t ptin_lag_memberList_get(L7_int port, L7_uint32 *port_pbmp);
extern L7_RC_t ptin_lag_activeMemberList_get(L7_int port, L7_uint32 *port_pbmp);
extern L7_RC_t ptin_translate_intf2port(L7_uint32 intf, L7_int *port_ref);
extern L7_RC_t ptin_translate_port2intf(L7_int port, L7_uint32 *intf_ref);
extern L7_RC_t ptin_translate_LagPortIntfSet(L7_int port, L7_uint32 intf_ref);
extern L7_RC_t ptin_translate_LagPortIntfClear(L7_int port);

extern void    ptin_vlan_translate_init(void);

//extern L7_RC_t ptin_translate_get_realVlans(L7_int port, L7_uint16 oldvlan, L7_uint16 *realVlans);

extern L7_RC_t ptin_intif_oldvlan_translate_get(L7_uint32 intif, L7_uint16 *vlan_old, L7_uint16 vlan_new);

extern L7_RC_t ptin_oldvlan_translate_get(L7_int port, L7_uint16 *vlan_old, L7_uint16 vlan_new);

extern L7_RC_t ptin_intif_vlan_translate_get(L7_uint32 intif, L7_uint16 vlan_old, L7_uint16 *vlan_new);

extern L7_RC_t ptin_intif_vlan_translate_egress_get(L7_uint32 intif, L7_uint16 vlan_old, L7_uint16 *vlan_new);

extern L7_RC_t ptin_vlan_translate_get(L7_int port, L7_uint16 vlan_old, L7_uint16 *vlan_new);

extern L7_RC_t ptin_vlan_translate_egress_get(L7_int port, L7_uint16 vlan_old, L7_uint16 *vlan_new);

extern L7_RC_t ptin_vlan_translate_add(L7_int port, L7_uint16 vlan_old, L7_uint16 vlan_new);

extern L7_RC_t ptin_vlan_translate_delete(L7_int port, L7_uint16 vlan_old);

extern L7_RC_t ptin_vlan_translate_egress_add(L7_int port, L7_uint16 vlan_old, L7_uint16 vlan_new);

extern L7_RC_t ptin_vlan_translate_egress_delete(L7_int port, L7_uint16 vlan_old);

extern void ptin_vlan_translate_delete_flush(void);

extern L7_RC_t ptin_mcflow_translate_save(L7_uint16 vlan_dot1q, L7_uint16 vlan_snoop);
extern L7_RC_t ptin_mcflow_translate_clear(L7_uint16 vlan_dot1q);
extern L7_RC_t ptin_mcflow_translate_dot1qvlan_get(L7_uint16 vlan_snoop, L7_uint16 *vlan_dot1q);
extern L7_RC_t ptin_mcflow_translate_snoopvlan_get(L7_uint16 vlan_dot1q, L7_uint16 *vlan_snoop);

#endif

/*
 * $Id: compat_6512.h,v 2.0 2017/10/14
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * RPC Compatibility with sdk-6.5.12 routines
 */

#ifndef _COMPAT_6512_H_
#define _COMPAT_6512_H_

#ifdef	BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/field.h>

#if defined(INCLUDE_L3)
#include <bcm/vxlan.h>

/* MPLS ECN Map Structure. */
typedef struct bcm_compat6512_vxlan_vpn_config_s {
    uint32 flags;                       /* BCM_VXLAN_VPN_xxx. */
    bcm_vpn_t vpn;                      /* VXLAN VPN */
    uint32 vnid;                        /* VNID */
    uint8 pkt_pri;                      /* Packet Priority */
    uint8 pkt_cfi;                      /* Packet CFI */
    uint16 egress_service_tpid;         /* Service TPID */
    bcm_vlan_t egress_service_vlan;     /* Service VLAN */
    bcm_multicast_t broadcast_group; 
    bcm_multicast_t unknown_unicast_group; 
    bcm_multicast_t unknown_multicast_group; 
    bcm_vlan_protocol_packet_ctrl_t protocol_pkt; 
    bcm_vlan_t vlan;                    /* Outer VLAN */
    bcm_gport_t match_port_class;       /* local port vlan domain */
} bcm_compat6512_vxlan_vpn_config_t;

/* Get vxlan vpn */
extern int bcm_compat6512_vxlan_vpn_get(int unit, bcm_vpn_t l2vpn,
                                     bcm_compat6512_vxlan_vpn_config_t *info);

/* Create vxlan vpn */
extern int bcm_compat6512_vxlan_vpn_create(int unit,
                                     bcm_compat6512_vxlan_vpn_config_t *info);
#endif
#endif
#endif	/* !_COMPAT_6512_H */

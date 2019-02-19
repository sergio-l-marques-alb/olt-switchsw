/* 
 * $Id: xgs5.h,v 1.6 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        xgs5.h
 * Purpose:     Definitions for XGS5 systems.
 */

#ifndef   _BCM_INT_XGS5_H_
#define   _BCM_INT_XGS5_H_

#include <bcm/qos.h>
#include <bcm/switch.h>
#include <bcm_int/esw/subport.h>
#ifdef BCM_MPLS_SUPPORT
#include <bcm/mpls.h>
#endif

#if defined(INCLUDE_BFD)

#include <soc/mcm/allenum.h>
#include <soc/tnl_term.h>
#include <bcm/tunnel.h>
#include <bcm/bfd.h>
#include <bcm_int/esw/bfd.h>

/*
 * Local RX DMA channel
 *
 * This is the channel number local to the uC (0..3).
 * Each uController application needs to use a different RX DMA channel.
 */
#define BCM_XGS5_BFD_RX_CHANNEL    1


/*
 * Device Specific HW Definitions
 */

/* Device programming routines */
typedef struct bcm_xgs5_bfd_hw_calls_s {
    int (*l3_tnl_term_entry_init)(int unit, 
                                  bcm_tunnel_terminator_t *tnl_info,
                                  soc_tunnel_term_t *entry);
    int (*mpls_lock)(int unit);
    void (*mpls_unlock)(int unit);
} bcm_xgs5_bfd_hw_calls_t;

/* L2 Table */
typedef struct bcm_xgs5_bfd_l2_table_s {
    soc_mem_t    mem;
    soc_field_t  key_type;
    uint32       bfd_key_type;
    soc_field_t  valid;
    soc_field_t  static_bit;
    soc_field_t  session_id_type;
    soc_field_t  your_discr;
    soc_field_t  label;
    soc_field_t  session_index;
    soc_field_t  cpu_queue_class;
    soc_field_t  remote;
    soc_field_t  dst_module;
    soc_field_t  dst_port;
    soc_field_t  int_pri;
} bcm_xgs5_bfd_l2_table_t;

/* L3 IPv4 Unicast Table */
typedef struct bcm_xgs5_bfd_l3_ipv4_table_s {
    soc_mem_t    mem;
    soc_field_t  vrf_id;
    soc_field_t  ip_addr;
    soc_field_t  key_type;
    soc_field_t  local_address;
    soc_field_t  bfd_enable;
} bcm_xgs5_bfd_l3_ipv4_table_t;

/* L3 IPv6 Unicast Table */
typedef struct bcm_xgs5_bfd_l3_ipv6_table_s {
    soc_mem_t    mem;
    soc_field_t  ip_addr_lwr_64;
    soc_field_t  ip_addr_upr_64;
    soc_field_t  key_type_0;
    soc_field_t  key_type_1;
    soc_field_t  vrf_id;
    soc_field_t  local_address;
    soc_field_t  bfd_enable;
} bcm_xgs5_bfd_l3_ipv6_table_t;

/* L3 Tunnel Table */
typedef struct bcm_xgs5_bfd_l3_tunnel_table_s {
    soc_mem_t    mem;
    soc_field_t  bfd_enable;
} bcm_xgs5_bfd_l3_tunnel_table_t;

/* MPLS Table */
typedef struct bcm_xgs5_bfd_mpls_table_s {
    soc_mem_t    mem;
    soc_field_t  valid;
    soc_field_t  key_type;
    uint32       key_type_value;
    soc_field_t  mpls_label;
    soc_field_t  session_id_type;
    soc_field_t  bfd_enable;
    soc_field_t  cw_check_ctrl;
    soc_field_t  pw_cc_type;
    soc_field_t  mpls_action_if_bos;
    soc_field_t  l3_iif;
    soc_field_t  decap_use_ttl;
} bcm_xgs5_bfd_mpls_table_t;

/* HW Definitions */
typedef struct bcm_xgs5_bfd_hw_defs_t {
    bcm_xgs5_bfd_hw_calls_t         *hw_call;    /* Chip programming */
    bcm_xgs5_bfd_l2_table_t         *l2;         /* L2 Memory Table */
    bcm_xgs5_bfd_l3_ipv4_table_t    *l3_ipv4;    /* L3 IPv4 UC Table */
    bcm_xgs5_bfd_l3_ipv6_table_t    *l3_ipv6;    /* L3 IPv6 UC Table */
    bcm_xgs5_bfd_l3_tunnel_table_t  *l3_tunnel;  /* L3 Tunnel Table */
    bcm_xgs5_bfd_mpls_table_t       *mpls;       /* MPLS Table */
} bcm_xgs5_bfd_hw_defs_t;


/* Functions */
extern
int bcmi_xgs5_bfd_init(int unit,
                       bcm_esw_bfd_drv_t *drv,
                       bcm_xgs5_bfd_hw_defs_t *hw_defs);
extern
int bcmi_xgs5_bfd_detach(int unit);
extern
int bcmi_xgs5_bfd_endpoint_create(int unit,
                                  bcm_bfd_endpoint_info_t *endpoint_info);
extern
int bcmi_xgs5_bfd_endpoint_get(int unit, bcm_bfd_endpoint_t endpoint, 
                               bcm_bfd_endpoint_info_t *endpoint_info);
extern
int bcmi_xgs5_bfd_tx_start(int unit);
extern
int bcmi_xgs5_bfd_tx_stop(int unit);
extern
int bcmi_xgs5_bfd_endpoint_destroy(int unit,
                                   bcm_bfd_endpoint_t endpoint);
extern
int bcmi_xgs5_bfd_endpoint_destroy_all(int unit);
extern
int bcmi_xgs5_bfd_endpoint_poll(int unit, bcm_bfd_endpoint_t endpoint);
extern
int bcmi_xgs5_bfd_event_register(int unit,
                                 bcm_bfd_event_types_t event_types, 
                                 bcm_bfd_event_cb cb, void *user_data);
extern
int bcmi_xgs5_bfd_event_unregister(int unit,
                                   bcm_bfd_event_types_t event_types, 
                                   bcm_bfd_event_cb cb);
extern
int bcmi_xgs5_bfd_endpoint_stat_get(int unit,
                                    bcm_bfd_endpoint_t endpoint, 
                                    bcm_bfd_endpoint_stat_t *ctr_info,
                                    uint8 clear);
extern
int bcmi_xgs5_bfd_auth_sha1_set(int unit, int index,
                                bcm_bfd_auth_sha1_t *sha1);
extern
int bcmi_xgs5_bfd_auth_sha1_get(int unit, int index,
                                bcm_bfd_auth_sha1_t *sha1);
extern
int bcmi_xgs5_bfd_auth_simple_password_set(int unit, int index, 
                                           bcm_bfd_auth_simple_password_t *sp);
extern
int bcmi_xgs5_bfd_auth_simple_password_get(int unit, int index, 
                                           bcm_bfd_auth_simple_password_t *sp);

#ifdef BCM_WARM_BOOT_SUPPORT
extern
int bcmi_xgs5_bfd_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern
void bcmi_xgs5_bfd_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif /* INCLUDE_BFD */

/* ING Port Table */
typedef struct bcmi_xgs5_subport_coe_ing_port_table_s {
    soc_mem_t    mem;
    soc_field_t  port_type;
} bcmi_xgs5_subport_coe_ing_port_table_t;

/* EGR Port Table */
typedef struct bcmi_xgs5_subport_coe_egr_port_table_s {
    soc_mem_t    mem;
    soc_field_t  port_type;
} bcmi_xgs5_subport_coe_egr_port_table_t;

/* Subport Tag SGPP Memory Table */
typedef struct bcmi_xgs5_subport_coe_subport_tag_sgpp_table_s {
    soc_mem_t    mem;
    soc_field_t  valid;
    soc_field_t  subport_tag;
    soc_field_t  subport_tag_mask;
    soc_field_t  subport_tag_namespace;
    soc_field_t  subport_tag_namespace_mask;
    soc_field_t  src_modid;
    soc_field_t  src_port;
    soc_field_t  phb_enable;
    soc_field_t  int_pri;
    soc_field_t  cng;
} bcmi_xgs5_subport_coe_subport_tag_sgpp_table_t;

/* Modport map subport Memory Table */
typedef struct bcmi_xgs5_subport_coe_modport_map_subport_table_s {
    soc_mem_t    mem;
    soc_field_t  dest;
    soc_field_t  is_trunk;
    soc_field_t  enable;
} bcmi_xgs5_subport_coe_modport_map_subport_table_t;

/* Egress subport tag dot1p Table */
typedef struct bcmi_xgs5_subport_coe_egr_subport_tag_dot1p_table_s {
    soc_mem_t    mem;
    soc_field_t  subport_tag_priority;
    soc_field_t  subport_tag_color;
} bcmi_xgs5_subport_coe_egr_subport_tag_dot1p_table_t;

typedef struct bcmi_xgs5_subport_coe_hw_defs_s {
     /* Ingress Port Memory Table */
    bcmi_xgs5_subport_coe_ing_port_table_t              *igr_port;
    /* Egress Port Memory Table */     
    bcmi_xgs5_subport_coe_egr_port_table_t              *egr_port;
    /* Subport Tag SGPP Memory Table */
    bcmi_xgs5_subport_coe_subport_tag_sgpp_table_t      *subport_tag_sgpp;
    /* Modport map subport Memory Table */
    bcmi_xgs5_subport_coe_modport_map_subport_table_t   *modport_map_subport; 
  /* Egress subport tag dot1p Table */
    bcmi_xgs5_subport_coe_egr_subport_tag_dot1p_table_t *egr_subport_tag_dot1p;
} bcmi_xgs5_subport_coe_hw_defs_t;


/* Functions */
extern int bcmi_xgs5_subport_init(int unit,
                           bcm_esw_subport_drv_t *drv,
                           bcmi_xgs5_subport_coe_hw_defs_t *hw_defs);
extern int _bcmi_xgs5_subport_reinit(int unit);
extern int bcmi_xgs5_subport_coe_cleanup(int unit);
extern int bcmi_xgs5_subport_coe_group_create(int unit,
                                     bcm_subport_group_config_t *config,
                                     bcm_gport_t *group);
extern int bcmi_xgs5_subport_coe_group_destroy(int unit, bcm_gport_t group);
extern int bcmi_xgs5_subport_coe_group_get(int unit, bcm_gport_t group,
                                     bcm_subport_group_config_t *config);
extern int bcmi_xgs5_subport_coe_group_traverse(int unit, bcm_gport_t group,
                                     bcm_subport_port_traverse_cb cb,
                                     void *user_data);
extern int bcmi_xgs5_subport_coe_port_add(int unit, bcm_subport_config_t 
                                          *config, bcm_gport_t *port);
extern int bcmi_xgs5_subport_coe_port_delete(int unit, bcm_gport_t port);
extern int bcmi_xgs5_subport_coe_port_get(int unit, bcm_gport_t port,
                                   bcm_subport_config_t *config);
extern int bcmi_xgs5_subport_coe_port_traverse(int unit,
                                      bcm_subport_port_traverse_cb cb,
                                        void *user_data);

extern int bcmi_xgs5_subport_egr_subtag_dot1p_map_add(int unit,
                                                      bcm_qos_map_t *map);
extern int bcmi_xgs5_subport_egr_subtag_dot1p_map_delete(int unit,
                                                         bcm_qos_map_t *map);

extern int bcmi_xgs5_subport_subtag_port_tpid_set(int unit, bcm_gport_t gport,
                                                  uint16 tpid);

extern int bcmi_xgs5_subport_subtag_port_tpid_delete(int unit, 
                                                     bcm_gport_t gport,
                                                     uint16 tpid);

extern int bcmi_xgs5_subport_subtag_port_tpid_get(int unit, bcm_gport_t gport,
                                                  uint16 *tpid);

extern int bcmi_xgs5_port_control_subtag_status_set(int unit,
                                                    bcm_port_t port, 
                                                    int value);

extern int bcmi_xgs5_port_control_subtag_status_get(int unit, 
                                                    bcm_port_t port, 
                                                    int *value);

extern int _bcmi_xgs5_subport_group_resolve(int unit,
                                           bcm_gport_t subport_group_gport,
                                           bcm_module_t *modid, 
                                           bcm_port_t *port,
                                           bcm_trunk_t *trunk_id, 
                                           int *id);

extern int _bcm_xgs5_subport_port_resolve(int unit, 
                                          bcm_gport_t subport_port_gport,
                                          bcm_module_t *modid,
                                          bcm_port_t *port,
                                          bcm_trunk_t *trunk_id,
                                          int *id);

extern int _bcmi_coe_subport_physical_port_get(int unit, bcm_gport_t subport,
                                               int *local_port);

extern int _bcmi_coe_subport_mod_port_physical_port_get(int unit, bcm_module_t modid, 
                                                        bcm_port_t portid,
                                                        int *local_port);

extern int _bcm_xgs5_subport_coe_mod_local(int unit, int modid);

extern int _bcm_xgs5_subport_gport_validate(int unit, bcm_port_t port_in,
                                            bcm_port_t *port_out);

extern int _bcm_xgs5_subport_coe_mod_port_local(int unit, int modid, 
                                                bcm_port_t port);

extern int _bcm_xgs5_subport_coe_gport_local(int unit, bcm_gport_t gport);

extern int _bcmi_coe_multicast_subport_encap_get(int unit, 
                                                 bcm_gport_t subport_port,
                                                 bcm_if_t *encap_id);

extern int _bcmi_coe_subport_tcam_idx_get(int unit, bcm_gport_t subport,
                                          int *hw_idx);

extern int bcmi_xgs5_subport_coe_ether_type_size_set(int unit, 
                                              bcm_switch_control_t type, 
                                              int arg);

extern int bcmi_xgs5_subport_coe_ether_type_size_get(int unit, 
                                              bcm_switch_control_t type, 
                                              int *arg);

extern int bcmi_xgs5_subport_subtag_port_override_set(int unit, int port, int arg);

extern int bcmi_xgs5_subport_subtag_port_override_get(int unit, int port, int *arg);

#ifdef INCLUDE_L3
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))

#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_INVALID    0x0
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_POP        0x1
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI    0x2
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI   0x3
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP  0x4
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP   0x5

#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_INVALID        0x0
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L2_SVP         0x1
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_IIF         0x2
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI       0x3
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI         0x4
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP        0x5
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP      0x6

/* MPLS functions */
extern int bcmi_xgs5_mpls_tunnel_switch_get(int unit,
        bcm_mpls_tunnel_switch_t *info);
extern int bcmi_xgs5_mpls_tunnel_switch_delete_all(int unit);
extern int bcmi_xgs5_mpls_tunnel_switch_delete(int unit,
        bcm_mpls_tunnel_switch_t *info);
extern int bcmi_xgs5_mpls_tunnel_switch_add(int unit,
        bcm_mpls_tunnel_switch_t *info);
extern int bcmi_xgs5_mpls_tunnel_switch_traverse(int unit,
        bcm_mpls_tunnel_switch_traverse_cb cb,
        void *user_data);
/* End of MPLS functions */
#endif /* (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)) */
#endif /* INCLUDE_L3 */

#endif /* _BCM_INT_XGS5_H_ */

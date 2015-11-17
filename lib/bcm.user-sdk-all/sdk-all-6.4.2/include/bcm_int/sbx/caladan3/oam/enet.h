/*
 * $Id: enet.h,v 1.10 Broadcom SDK $
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
 */
#ifndef _BCM_INT_SBX_CALADAN3_OAM_ENET_H_
#define _BCM_INT_SBX_CALADAN3_OAM_ENET_H_
#include <bcm/vlan.h>
#include <bcm_int/sbx/caladan3/vlan.h>

#ifdef BCM_CALADAN3_G3P1_SUPPORT
#include <soc/sbx/g3p1/g3p1.h>
#include <soc/sbx/g3p1/g3p1_int.h>
#include <soc/sbx/caladan3.h>
#endif

#define BCM_C3_OAM_ENET_CCM_MESSAGE_TYPE 1

/* ivid(2) ovid (2), gport(4), vid_mode (1), direction (4) */
#define BCM_C3_OAM_ENET_SERVICE_HASH_KEY_SIZE  (13)
typedef uint8 bcm_c3_oam_enet_service_hash_key_t[BCM_C3_OAM_ENET_SERVICE_HASH_KEY_SIZE];

typedef struct bcm_c3_oam_enet_service_info_s {
    uint32 id;
    uint32 ref_cnt;
} bcm_c3_oam_enet_service_info_t;

typedef struct bcm_c3_oam_enet_endpoint_state_s {
    bcm_vlan_t                     vlan;                      /* from bcm_oam_endpoint_info_t VLAN ID                        */
    bcm_c3_oam_enet_service_info_t service_info;              /* This is allocated per ivid/ovid/port/vid_mode/direction     */
                                                              /* to uniquely identify a service - called svc_hdl in ucode    */
    unsigned int                   peer_idx;                  /* This is allocated to uniquely associate maidmep to oam_peer */
    bcm_mac_t                      src_mac_address;           /* from bcm_oam_endpoint_info_t source mac address             */
} bcm_c3_oam_enet_endpoint_state_t;

typedef struct bcm_c3_oam_enet_state_s {
    shr_idxres_list_handle_t          peer_idx_pool;       /* 64k max */

    shr_idxres_list_handle_t          service_pool;        /* service pool */
    shr_htb_hash_table_t              service_htbl;        /* service hash handle */
    bcm_c3_oam_enet_service_info_t   *service_info;        /* service info array */
    uint32                            ete_base;            /* base address of allocated group of ETEs    */
    uint32                            oi2e_reserved_id;
    uint32                            max_endpoints;
    bcm_c3_oam_enet_endpoint_state_t *enet_endpoint_state;
} bcm_c3_oam_enet_state_t;


#define  BCM_C3_OAM_ENET_ALLOC_ETE_BIT         0x01 
 
     
#define BCM_C3_OAM_ENET_ALLOC_GET(type, alloc_state)  ((alloc_state) & BCM_C3_OAM_ENET_ALLOC_##type##_BIT)   
#define BCM_C3_OAM_ENET_ALLOC_SET(type, alloc_state)   (alloc_state) |= BCM_C3_OAM_ENET_ALLOC_##type##_BIT 
#define BCM_C3_OAM_ENET_ALLOC_CLEAR(type, alloc_state) (alloc_state) &= (~BCM_C3_OAM_ENET_ALLOC_##type##_BIT) 
/*
 * Tracking of the egress resources for logical ports
 * ete_index is the bubble_idx which is the endpoint_id
 */
typedef struct bcm_c3_oam_enet_egress_path_desc_s {
    uint8                    alloc_state;
    uint32                   ete_index;
    soc_sbx_g3p1_ete_t       ete;
    bcm_gport_t              port;
} bcm_c3_oam_enet_egress_path_desc_t;

int bcm_c3_oam_enet_init(int unit, uint32 max_endpoints);
int bcm_c3_oam_enet_cleanup(int unit, uint32 max_endpoints);
int bcm_c3_oam_enet_validate_endpoint(int unit, bcm_oam_endpoint_info_t *endpoint_info);
int bcm_c3_oam_enet_endpoint_create(int unit, bcm_oam_endpoint_info_t *endpoint_info);
int bcm_c3_oam_enet_endpoint_destroy(int unit, bcm_oam_endpoint_info_t *endpoint_info);
int bcm_c3_oam_enet_endpoint_get(int unit, bcm_oam_endpoint_t endpoint_id, bcm_oam_endpoint_info_t *endpoint_info);
int bcm_c3_oam_enet_endpoint_info_state_get(int unit, bcm_oam_endpoint_info_t *endpoint_info, bcm_oam_endpoint_t endpoint_id);
int bcm_c3_oam_enet_sdk_ccm_period_to_packet_ccm_period(int unit, int ccm_period, int *packet_ccm_period);
#endif  /* _BCM_INT_SBX_CALADAN3_OAM_ENET_H_  */

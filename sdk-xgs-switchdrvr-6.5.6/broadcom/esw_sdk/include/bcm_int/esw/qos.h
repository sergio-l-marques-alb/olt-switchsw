/* $Id: qos.h,v 1.2 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * This file contains QOS module definitions internal to the BCM library.
 */

#ifndef _BCM_INT_QOS_H
#define _BCM_INT_QOS_H

#include <bcm/qos.h>

#define _BCM_QOS_MAP_TYPE_ING_VFT_PRI_MAP    6
#define _BCM_QOS_MAP_TYPE_EGR_VFT_PRI_MAP    7
#define _BCM_QOS_MAP_TYPE_EGR_VSAN_PRI_MAP   8
#define _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP   9
#define _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP   10
#define _BCM_QOS_VFT_MAP_SHIFT               11
#ifdef BCM_HURRICANE3_SUPPORT
#define _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP    12
#define _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS      13
#endif

#define QOS_MAP_IS_FCOE(map_id)                                                \
  (  ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_ING_VFT_PRI_MAP) \
  || ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_VFT_PRI_MAP) \
  || ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_VSAN_PRI_MAP)\
  )

#define QOS_MAP_IS_L2_VLAN_ETAG(map_id)                                        \
  (  ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) \
  || ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP) \
  )

#define QOS_FLAGS_ARE_FCOE(flags) \
  ( (flags & BCM_QOS_MAP_VFT) || (flags & BCM_QOS_MAP_VSAN) )

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_qos_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_esw_qos_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef BCM_TRIDENT2_SUPPORT
extern int bcm_td2_qos_get_egr_vsan_hw_idx(int unit, int egr_map_id, 
                                            int *hw_index);
extern int bcm_td2_vsan_profile_to_qos_id(int unit, int profile_num, 
                                          int *egr_map);
extern int bcm_td2_qos_egr_etag_id2profile(int unit, int map_id, int *profile_num);
extern int bcm_td2_qos_egr_etag_profile2id(int unit, int profile_num, int *map_id);
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
extern void
_bcm_tr2_qos_id_update(int unit, uint32 dscp_map_id, uint32 new_dscp_map_id);
#endif

#if defined(BCM_HGPROXY_COE_SUPPORT)
extern int bcmi_xgs5_subport_egr_subtag_dot1p_map_add(int unit,
                                                      bcm_qos_map_t *map);
extern int bcmi_xgs5_subport_egr_subtag_dot1p_map_delete(int unit,
                                                         bcm_qos_map_t *map);
#endif /* BCM_HGPROXY_COE_SUPPORT */

#endif	/* !_BCM_INT_QOS_H */

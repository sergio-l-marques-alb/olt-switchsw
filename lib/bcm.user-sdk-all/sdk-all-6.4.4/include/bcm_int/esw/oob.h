/*
 * $Id: bst.h,v 1.0 Broadcom SDK $
 *
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

#ifndef _BCM_OOB_H
#define _BCM_OOB_H

#include <soc/defs.h>
#include <soc/types.h>

#define _BCM_OOB_UNIT_DRIVER(u)   _bcm_oob_unit_driver[(u)]

typedef int (*_bcm_oob_fc_tx_config_set_f)(int, bcm_oob_fc_tx_config_t*);
typedef int (*_bcm_oob_fc_tx_config_get_f)(int, bcm_oob_fc_tx_config_t*);
typedef int (*_bcm_oob_fc_tx_info_get_f)(int, bcm_oob_fc_tx_info_t*);

typedef int (*_bcm_oob_fc_rx_port_tc_mapping_multi_set_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                          bcm_gport_t, int, uint32*,
                                                          uint32*);
typedef int (*_bcm_oob_fc_rx_port_tc_mapping_multi_get_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                          bcm_gport_t, int, uint32*,
                                                          uint32*, int*);
typedef int (*_bcm_oob_fc_rx_port_tc_mapping_set_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                    bcm_gport_t, uint32, uint32);
typedef int (*_bcm_oob_fc_rx_port_tc_mapping_get_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                    bcm_gport_t, uint32, uint32*);
typedef int (*_bcm_oob_fc_rx_config_set_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                bcm_oob_fc_rx_config_t*, int,
                                                bcm_gport_t*);
typedef int (*_bcm_oob_fc_rx_config_get_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                bcm_oob_fc_rx_config_t*, int,
                                                bcm_gport_t*, int*);
typedef int (*_bcm_oob_fc_rx_port_offset_get_f)(int, bcm_oob_fc_rx_intf_id_t,
                                                bcm_gport_t, uint32*);

typedef int (*_bcm_oob_stats_config_set_f)(int unit, bcm_oob_stats_config_t *config);
typedef int (*_bcm_oob_stats_config_get_f)(int unit, bcm_oob_stats_config_t *config);
typedef int (*_bcm_oob_stats_pool_mapping_multi_set_f)(int unit, int array_count,
                                                       int *offset_array, uint8 *dir_array,
                                                       bcm_service_pool_id_t *pool_array);
typedef int (*_bcm_oob_stats_pool_mapping_multi_get_f)(int unit, int array_max,
                                                       int *offset_array, uint8 *dir_array,
                                                       bcm_service_pool_id_t *pool_array,
                                                       int *array_count);
typedef int (*_bcm_oob_stats_pool_mapping_set_f)(int unit, int offset, uint8 dir,
                                                bcm_service_pool_id_t pool);
typedef int (*_bcm_oob_stats_pool_mapping_get_f)(int unit, int offset, uint8 *dir,
                                                bcm_service_pool_id_t *pool);
typedef int (*_bcm_oob_stats_queue_mapping_multi_set_f)(int unit, int array_count,
                                                     int *offset_array,
                                                     bcm_gport_t *gport_array);
typedef int (*_bcm_oob_stats_queue_mapping_multi_get_f)(int unit, int array_max,
                                                       int *offset_array,
                                                       bcm_gport_t *gport_array,
                                                       int *array_count);
typedef int (*_bcm_oob_stats_queue_mapping_set_f)(int unit, int offset,
                                                  bcm_gport_t gport);
typedef int (*_bcm_oob_stats_queue_mapping_get_f)(int unit, int offset,
                                                  bcm_gport_t *gport);

typedef struct _bcm_oob_unit_driver_s {
    _bcm_oob_fc_tx_config_set_f                     fc_tx_config_set;
    _bcm_oob_fc_tx_config_get_f                     fc_tx_config_get;
    _bcm_oob_fc_tx_info_get_f                       fc_tx_info_get;
    _bcm_oob_fc_rx_port_tc_mapping_multi_set_f      fc_rx_port_tc_mapping_multi_set;
    _bcm_oob_fc_rx_port_tc_mapping_multi_get_f      fc_rx_port_tc_mapping_multi_get;
    _bcm_oob_fc_rx_port_tc_mapping_set_f            fc_rx_port_tc_mapping_set;
    _bcm_oob_fc_rx_port_tc_mapping_get_f            fc_rx_port_tc_mapping_get;
    _bcm_oob_fc_rx_config_set_f                     fc_rx_config_set;
    _bcm_oob_fc_rx_config_get_f                     fc_rx_config_get;
    _bcm_oob_fc_rx_port_offset_get_f                fc_rx_port_offset_get;
    _bcm_oob_stats_config_set_f                     stats_config_set;
    _bcm_oob_stats_config_get_f                     stats_config_get;
    _bcm_oob_stats_pool_mapping_multi_set_f         stats_pool_mapping_multi_set;
    _bcm_oob_stats_pool_mapping_multi_get_f         stats_pool_mapping_multi_get;
    _bcm_oob_stats_pool_mapping_set_f               stats_pool_mapping_set;
    _bcm_oob_stats_pool_mapping_get_f               stats_pool_mapping_get;
    _bcm_oob_stats_queue_mapping_multi_set_f        stats_queue_mapping_multi_set;
    _bcm_oob_stats_queue_mapping_multi_get_f        stats_queue_mapping_multi_get;
    _bcm_oob_stats_queue_mapping_set_f              stats_queue_mapping_set;
    _bcm_oob_stats_queue_mapping_get_f              stats_queue_mapping_get;
} _bcm_oob_unit_driver_t;

extern int _bcm_oob_init(int unit);
extern void _bcm_oob_deinit(int unit);

#if defined(BCM_TOMAHAWK_SUPPORT)
extern int bcm_oob_th_init(int unit);
#endif /* BCM_TOMAHAWK_SUPPORT */

#endif /* _BCM_OOB_H */

/*
 * $Id: wb_db_port.h,v 1.6 Broadcom SDK $
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
 * File:        wb_db_port.h
 * Purpose:     WarmBoot - Level 2 support (PORT Module)
 */

#ifndef _BCM_DPP_WB_DB_PORT_H_
#define _BCM_DPP_WB_DB_PORT_H_

#include <bcm_int/dpp/port.h>

typedef struct bcm_dpp_wb_port_config_s {

    int map_tbl_use[_bcm_dpp_port_map_type_count][_BCM_DPP_PORT_MAP_MAX_NOF_TBLS];

    /* for each learn trap what is the usage */
    int trap_to_flag[_BCM_PETRA_PORT_LEARN_NOF_TRAPS];

    _bcm_petra_port_tpid_info *_bcm_port_tpid_info;

} bcm_dpp_wb_port_config_t;

extern int
_bcm_dpp_wb_port_tpid_is_allocated(int unit, uint8 *is_alloc);

extern int
_bcm_dpp_wb_port_tpid_allocate(int unit);

extern int
_bcm_dpp_wb_port_tpid_free(int unit);

extern int
_bcm_dpp_wb_port_map_tbl_use_set(int unit, int map_tbl_use, uint32 indx1, uint32 indx2);

extern int
_bcm_dpp_wb_port_map_tbl_use_get(int unit, int *map_tbl_use, uint32 indx1, uint32 indx2);

extern int
_bcm_dpp_wb_port_trap_to_flag_set(int unit, int trap_to_flag, uint32 indx1);

extern int
_bcm_dpp_wb_port_trap_to_flag_get(int unit, int *trap_to_flag, uint32 indx1);

extern int
_bcm_dpp_wb_port_tpid_set(int unit, uint16 tpid, uint32 indx1, uint32 indx2);

extern int
_bcm_dpp_wb_port_tpid_get(int unit, uint16 *tpid, uint32 indx1, uint32 indx2);

extern int
_bcm_dpp_wb_port_tpid_count_set(int unit, int tpid_count, uint32 indx1, uint32 indx2);

extern int
_bcm_dpp_wb_port_tpid_count_get(int unit, int *tpid_count, uint32 indx1, uint32 indx2);

#if defined(BCM_WARM_BOOT_SUPPORT)

/*
 * runtime information
 */
typedef struct bcm_dpp_wb_port_info_s {
    int                      init_done;
    bcm_dpp_wb_port_config_t *port_config;

    int                      is_dirty;

    uint16                   version;
    uint8                   *scache_ptr;
    int                      size;

    uint32                   map_tbl_use_num;
    uint32                   map_tbl_use_off;

    uint32                   trap_to_flag_num;
    uint32                   trap_to_flag_off;

    uint32                   tpid_num;
    uint32                   tpid_off;

    uint32                   tpid_count_num;
    uint32                   tpid_count_off;

} bcm_dpp_wb_port_info_t;

extern bcm_dpp_wb_port_info_t   _dpp_wb_port_info_p[BCM_MAX_NUM_UNITS];
extern bcm_dpp_wb_port_config_t bcm_dpp_wb_port_config[BCM_MAX_NUM_UNITS];
/*
 * Functions
 */
extern int
_bcm_dpp_wb_port_state_init(int unit);

extern int
_bcm_dpp_wb_port_state_deinit(int unit);

extern int
_bcm_dpp_wb_port_sync(int unit);

extern int
_bcm_dpp_wb_port_update_map_tbl_use_state(int unit, uint32 arr_indx1, uint32 arr_indx2);

extern int
_bcm_dpp_wb_port_update_trap_to_flag_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_port_update_tpid_state(int unit, uint32 arr_indx1, uint32 arr_indx2);

extern int
_bcm_dpp_wb_port_update_tpid_count_state(int unit, uint32 arr_indx1, uint32 arr_indx2);

extern int
_bcm_dpp_wb_port_state_is_initialized(int unit);


#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _BCM_DPP_WB_DB_PORT_H_ */

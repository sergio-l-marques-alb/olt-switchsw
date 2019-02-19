/* 
 * $Id: init_deinit.c,v 1.0 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:        init_deinit.h
 * Purpose:     DCMN initialization sequence header file.
 *
 */

#ifndef _APPL_DCMN_INIT_DEINIT_H
#define _APPL_DCMN_INIT_DEINIT_H

/* appl_traffic_enable_stage options: */
#define TRAFFIC_EN_STAGE_DISABLE     0
#define TRAFFIC_EN_STAGE_AFTER_STK   0x1

typedef struct appl_dcmn_init_param_s {
    uint32 unit;
    int32 nof_devices;
    int32 repeat;
    int32 no_init;
    int32 no_deinit;
    int32 no_attach;
    int32 no_bcm;
    int32 no_soc;
    int32 no_intr;
    int32 no_rx_los;
    int32 modid;
    int32 base_modid;
    int32 flags;   
#if (defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
    int32 partial_init;
    int32 fake_bcm_start;
    int32 fake_bcm_range;
    int32 fake_soc_start;
    int32 fake_soc_range; 
#endif
    int32 no_linkscan;
    int32 warmboot;
    int32 engine_dump;
    int32 no_sync;
    int32 no_elk_appl;
    int32 no_elk_device;
    int32 no_elk_second_device;
    uint32 elk_ilkn_rev;
    uint32 elk_mdio_id;
    uint32 elk_mdio_second_id;

    int32 no_packet_rx;
    int32 packet_rx_cosq;

    int32 no_appl;
    int32 no_appl_stk;
    int32 appl_traffic_enable_stage;
    uint32 cosq_disable;
    uint32 appl_flags;
    int32 no_itmh_prog_mode;
    int32 rc_load;
    uint32 is_resources_check;
    uint32 l2_mode;
} appl_dcmn_init_param_t;

cmd_result_t appl_dcmn_init_usage_parse(int unit, args_t *a, appl_dcmn_init_param_t *init_param);

int appl_dcmn_init(int unit, appl_dcmn_init_param_t* init_param);

int appl_dcmn_deinit(int unit, appl_dcmn_init_param_t* init_param);

#endif /* _APPL_DCMN_INIT_DEINIT_H */

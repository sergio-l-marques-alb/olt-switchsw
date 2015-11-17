/*
 * $Id: fabric.h,v 1.6 Broadcom SDK $
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
 * File:        fabric.h
 * Purpose:     FABRIC internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_FABRIC_H_
#define   _BCM_INT_DPP_FABRIC_H_

#include <bcm/debug.h>

#include <soc/dpp/dpp_config_defs.h>

/*
 * Defines
 */
#define BCM_FABRIC_MODID_IS_VALID(unit, modid)  \
        ((modid >= 0) && (modid < SOC_DPP_DEFS_GET(unit, modid_max_valid))) /* True if modid is between 0 and the max 
                                                                              limit of FAP's in the device  */
#define BCM_FABRIC_NUM_OF_LINKS_IS_VALID(unit, num_of_links)  \
        ((num_of_links >= 0) && (num_of_links < SOC_DPP_DEFS_GET(unit, nof_fabric_links))) /* True if num_of_links is between 0 and the max 
                                                                                            fabric links in the device  */
#define BCM_FABRIC_PCP_MODE_IS_VALID(pcp_mode)  \
        ((pcp_mode >= 0) && (pcp_mode <= 2))        /* True if pcp_mode is one of the 
                                                        three supported pcp_modes (0-2) */
#define BCM_FABRIC_MC_ID_IS_VALID(unit, mc_id)  \
        ((mc_id >= 0) && (mc_id <= (64*1024)/*SOC_DPP_DEFS_GET(unit, mc_id_max_valid)*/))    /* True if mc_id is between 0 and the max 
                                                                                               limit of mc id's in the device */
/*
 * External functions
 */
int
bcm_petra_fabric_enhance_ports_get(int unit, int *nbr_ports, int *ports);
int
bcm_petra_fabric_multicast_scheduler_mode_get(int unit, int *mode);
int
_bcm_petra_fabric_credit_watchdog_range_set(int unit, bcm_fabric_control_t control_type, int queue_id, int common_message_time);
#endif /* _BCM_INT_DPP_FABRIC_H_ */

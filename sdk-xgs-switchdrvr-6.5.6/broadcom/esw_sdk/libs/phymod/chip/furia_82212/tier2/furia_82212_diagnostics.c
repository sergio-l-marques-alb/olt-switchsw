/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 * 
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
 *     
 */

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>

#ifdef PHYMOD_FURIA_SUPPORT

#include <phymod/chip/furia_diagnostics.h>
#include "furia_82212_helper.h"

int furia_82212_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_config_set(&phy_cpy, flags, prbs));
        }
    }
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_config_get(&phy_cpy, flags, prbs));
            break;
        }
    }
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_enable_set(&phy_cpy, flags, enable));
        }
    }
    return PHYMOD_E_NONE;
}

int furia_82212_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_enable_get(&phy_cpy, flags, enable));
            break;
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    phymod_prbs_status_t status;
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
    prbs_status->prbs_lock = 1;
    prbs_status->prbs_lock_loss = 1;
    prbs_status->error_count = 0;

    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_status_get(&phy_cpy, flags, &status));
            prbs_status->prbs_lock &= status.prbs_lock;
            prbs_status->prbs_lock_loss &= status.prbs_lock_loss ;
            prbs_status->error_count |= status.error_count;
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_diagnostics_get(&phy_cpy, diag));
            break;
        }
    }
       
    return PHYMOD_E_NONE;
}


int furia_82212_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_eyescan_run(&phy_cpy, flags, mode, eyescan_options));
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_link_mon_enable_set(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_link_mon_enable_set(&phy_cpy, link_mon_mode, enable));
        }
    }
       
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_link_mon_enable_get(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t* enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_link_mon_enable_get(&phy_cpy, link_mon_mode, enable));
            break;
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_link_mon_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint32_t lock= 1, lock_lost = 1, err_cnt = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
    
    *lock_status = 0xFFFF;
    *lock_lost_lh = 0xFFFF;
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_link_mon_status_get(&phy_cpy, &lock, &lock_lost, &err_cnt));
            *lock_status &= lock;
            *lock_lost_lh &= lock_lost;
            *error_count |= err_cnt;
        }
    }
       
        
    return PHYMOD_E_NONE;
    
}


#endif /* PHYMOD_FURIA_SUPPORT */

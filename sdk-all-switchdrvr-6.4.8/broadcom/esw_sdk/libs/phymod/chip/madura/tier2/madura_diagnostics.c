/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 * 
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
 *     
 */

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>

#ifdef PHYMOD_MADURA_SUPPORT
#include "../tier1/madura_cfg_seq.h"
#include "../tier1/madura_diag_seq.h"

int madura_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
        
    return (_madura_phy_prbs_config_set (phy, flags, prbs)); 
    
}

int madura_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
        
    return (_madura_phy_prbs_config_get(phy, flags, prbs)); 
    
}


int madura_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
        
    return _madura_phy_prbs_enable_set(phy, flags, enable);
    
}

int madura_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
        
    return _madura_phy_prbs_enable_get(phy, flags, enable);
    
}


int madura_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    return _madura_phy_prbs_status_get(phy, flags, prbs_status);
}


int madura_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
        
    return _madura_phy_diagnostics_get(&phy->access, diag);
    
}


int madura_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{
    if (PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        switch(mode) {
        case phymodEyescanModeFast:
          return _madura_phy_display_eyescan(&phy->access);
          /*
        case phymodEyescanModeBERProj:
          return _madura_ber_proj(&phy->access, eyescan_options);
          */
        default:
          return _madura_phy_display_eyescan(&phy->access);
          break;
        }
    } else {
        return PHYMOD_E_NONE;
    }
    
}


#endif /* PHYMOD_MADURA_SUPPORT */

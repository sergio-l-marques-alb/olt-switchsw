/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
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
 *         
 *     
 */

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>


#ifdef PHYMOD_QUADRA28_SUPPORT

#include <phymod/chip/quadra28.h>

/* To be defined */
#include "../tier1/quadra28_types.h"
#include "../tier1/quadra28_cfg_seq.h"
#include "../tier1/quadra28_reg_access.h"
#include "../tier1/bcmi_quadra28_defs.h"


/**   Convert PHYMOD poly to falcon poly type     
 *    This function converts PHYMOD polynomial type to falcon polynomial type  
 *
 *    @param phymod_poly        phymod polynomial type
 *    @param falcon_poly        falcon polynomial type
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
STATIC
int _quadra28_prbs_poly_phymod_to_merlin(phymod_prbs_poly_t phymod_poly,
                                      enum srds_prbs_polynomial_enum *merlin_poly)
{
    switch(phymod_poly) {
        case phymodPrbsPoly7:
            *merlin_poly= SRDS_PRBS_7;
        break;
        case phymodPrbsPoly9:
            *merlin_poly= SRDS_PRBS_9;
        break;
        case phymodPrbsPoly11:
            *merlin_poly= SRDS_PRBS_11;
        break;
        case phymodPrbsPoly15:
            *merlin_poly= SRDS_PRBS_15;
        break;
        case phymodPrbsPoly23:
            *merlin_poly= SRDS_PRBS_23;
        break;
        case phymodPrbsPoly31:
            *merlin_poly= SRDS_PRBS_31;
        break;
        case phymodPrbsPoly58:
            *merlin_poly= SRDS_PRBS_58;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_PARAM,\
                 (_PHYMOD_MSG("unsupported poly for q28")));
        break;
    }
    return PHYMOD_E_NONE;
}

/**   Convert falcon poly to PHYMOD poly type     
 *    This function converts falcon polynomial type to PHYMOD polynomial type  
 *
 *    @param falcon_poly        falcon polynomial type
 *    @param phymod_poly        phymod polynomial type
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
STATIC
int _quadra28_prbs_poly_merlin_to_phymod(enum srds_prbs_polynomial_enum merlin_poly,
                                      phymod_prbs_poly_t *phymod_poly)
{
    switch(merlin_poly) {
        case SRDS_PRBS_7:
            *phymod_poly = phymodPrbsPoly7;
        break;
        case SRDS_PRBS_9:
            *phymod_poly = phymodPrbsPoly9;
        break;
        case SRDS_PRBS_11:
            *phymod_poly = phymodPrbsPoly11;
        break;
        case SRDS_PRBS_15:
            *phymod_poly = phymodPrbsPoly15;
        break;
        case SRDS_PRBS_23:
            *phymod_poly = phymodPrbsPoly23;
        break;
        case SRDS_PRBS_31:
            *phymod_poly = phymodPrbsPoly31;
        break;
        case SRDS_PRBS_58:
            *phymod_poly = phymodPrbsPoly58;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_INTERNAL,\
                 (_PHYMOD_MSG("uknown poly")));
        break;
    }
    return PHYMOD_E_NONE;
}

/** PRBS Polynomial Enum */
int quadra28_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum merlin_poly;
    
    PHYMOD_IF_ERR_RETURN
        (_quadra28_prbs_poly_phymod_to_merlin(prbs->poly, &merlin_poly)); 
    PHYMOD_IF_ERR_RETURN(
        quara28_prbs_config_set(&phy->access,
                        flags,
                        merlin_poly,
                        prbs->invert));
    return PHYMOD_E_NONE;
    
}

int quadra28_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum merlin_poly;
    
    PHYMOD_IF_ERR_RETURN(
        quadra28_prbs_config_get(&phy->access,
                        flags,
                        &merlin_poly, &prbs->invert));
    PHYMOD_IF_ERR_RETURN
        (_quadra28_prbs_poly_merlin_to_phymod(merlin_poly, &prbs->poly));
    
    return PHYMOD_E_NONE;
    
}


int quadra28_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{

    PHYMOD_IF_ERR_RETURN(
        quadra28_prbs_enable_set(&phy->access, flags, enable));
    
    return PHYMOD_E_NONE;
    
}

int quadra28_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN(
        quadra28_prbs_enable_get(&phy->access, flags, enable));
    
    return PHYMOD_E_NONE;
    
}


int quadra28_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    PHYMOD_IF_ERR_RETURN(
        quadra28_prbs_status_get(&phy->access,
                             &prbs_status->prbs_lock,
                             &prbs_status->prbs_lock_loss,
                             &prbs_status->error_count));

    PHYMOD_DIAG_OUT(("PRBSSTATUS:: lock:%d ERR Cnt:%d lock loss:%d\n", prbs_status->prbs_lock, prbs_status->error_count, prbs_status->prbs_lock_loss));
    return PHYMOD_E_NONE;
    
}


int quadra28_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
        
    
    PHYMOD_IF_ERR_RETURN(
      _quadra28_phy_diagnostics_get(&phy->access, diag));
    return PHYMOD_E_NONE;
    
}


int quadra28_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{
    /* Added condition to solve printing eye trice from PHYMOD layer*/
    if (PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        return quadra28_display_eye_scan(&phy->access);
    } else {
        return PHYMOD_E_NONE;
    }
    
}


#endif /* PHYMOD_QUADRA28_SUPPORT */

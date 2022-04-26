/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 * 
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_config.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/chip/jupiter_diagnostics.h>

#include "jupiter/tier1/jupiter_dependencies.h"
#include "jupiter/tier1/jupiter_core.h"

#ifdef PHYMOD_JUPITER_SUPPORT

STATIC
int _jupiter_prbs_poly_phymod_to_jupiter(phymod_prbs_poly_t phymod_poly, jupiter_bist_pattern_t *jupiter_poly)
{
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *jupiter_poly = JUPITER_PRBS7;
        break;
    case phymodPrbsPoly9:
        *jupiter_poly = JUPITER_PRBS9;
        break;
    case phymodPrbsPoly11:
        *jupiter_poly = JUPITER_PRBS11;
        break;
    case phymodPrbsPoly13:
        *jupiter_poly = JUPITER_PRBS13;
        break;
    case phymodPrbsPoly15:
        *jupiter_poly = JUPITER_PRBS15;
        break;
    case phymodPrbsPoly23:
        *jupiter_poly = JUPITER_PRBS23;
        break;
    case phymodPrbsPoly31:
        *jupiter_poly = JUPITER_PRBS31;
        break;
    case phymodPrbsPoly58:
    case phymodPrbsPoly49:
    case phymodPrbsPoly10:
    case phymodPrbsPoly20:
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for Jupiter %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _jupiter_prbs_poly_jupiter_to_phymod(jupiter_bist_pattern_t jupiter_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch(jupiter_poly){
    case JUPITER_PRBS7:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case JUPITER_PRBS9:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case JUPITER_PRBS11:
        *phymod_poly = phymodPrbsPoly11;
        break;
    case JUPITER_PRBS13:
        *phymod_poly = phymodPrbsPoly13;
        break;
    case JUPITER_PRBS15:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case JUPITER_PRBS23:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case JUPITER_PRBS31:
        *phymod_poly = phymodPrbsPoly31;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown poly %u"), jupiter_poly));
    }
    return PHYMOD_E_NONE;
}


#define JUPITER_PRBS_LOCK_THRESHOLD 10
#define JUPITER_PRBS_TIMER_THRESHOLD 10

int jupiter_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{        
    int start_lane, num_lane, i;
    uint64_t udp;
    uint32_t lock_thresh = JUPITER_PRBS_LOCK_THRESHOLD, timer_thresh = JUPITER_PRBS_TIMER_THRESHOLD;
    phymod_phy_access_t phy_copy;
    jupiter_bist_pattern_t jupiter_poly;
    jupiter_bist_mode_t mode = JUPITER_DWELL;

    COMPILER_64_ZERO(udp);
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN(_jupiter_prbs_poly_phymod_to_jupiter(prbs->poly, &jupiter_poly));

    if(prbs->invert) {
        return PHYMOD_E_PARAM;
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_rx_chk_config_set(&phy_copy.access, jupiter_poly, mode,  udp, lock_thresh, timer_thresh));
        } else if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_tx_gen_config_set(&phy_copy.access, jupiter_poly, udp));
        } else {
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_rx_chk_config_set(&phy_copy.access, jupiter_poly, mode,  udp, lock_thresh, timer_thresh));
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_tx_gen_config_set(&phy_copy.access, jupiter_poly, udp));
        }
    }

    return PHYMOD_E_NONE;
}

int jupiter_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{        
    uint64_t udp;
    uint32_t lock_thresh, timer_thresh;
    phymod_phy_access_t phy_copy;
    jupiter_bist_pattern_t jupiter_poly;
    jupiter_bist_mode_t mode = JUPITER_DWELL;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_gen_config_get(&phy_copy.access, &jupiter_poly, &udp));
        PHYMOD_IF_ERR_RETURN(_jupiter_prbs_poly_jupiter_to_phymod(jupiter_poly, &prbs->poly));
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_rx_chk_config_get(&phy_copy.access,
                                                           &jupiter_poly,
                                                           &mode,
                                                           &udp,
                                                           &lock_thresh,
                                                           &timer_thresh));
        PHYMOD_IF_ERR_RETURN(_jupiter_prbs_poly_jupiter_to_phymod(jupiter_poly, &prbs->poly));
    } else {
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_gen_config_get(&phy_copy.access, &jupiter_poly, &udp));
        PHYMOD_IF_ERR_RETURN(_jupiter_prbs_poly_jupiter_to_phymod(jupiter_poly, &prbs->poly));
    }

    prbs->invert = 0;
    return PHYMOD_E_NONE;
}


int jupiter_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{        
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_gen_en_set(&phy_copy.access, enable));
        } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN(jupiter_pmd_rx_chk_en_set(&phy_copy.access, enable));
        } else {
            PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_gen_en_set(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(jupiter_pmd_rx_chk_en_set(&phy_copy.access, enable));
        }
    }

    return PHYMOD_E_NONE;
}

int jupiter_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{        
    uint32_t enable_tmp;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_gen_en_get(&phy_copy.access, &enable_tmp));
        *enable = enable_tmp;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_rx_chk_en_get(&phy_copy.access, &enable_tmp));
        *enable = enable_tmp;
    } else {
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_gen_en_get(&phy_copy.access, &enable_tmp));
        *enable = enable_tmp;
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_rx_chk_en_get(&phy_copy.access, &enable_tmp));
        *enable &= enable_tmp;
    }

    return PHYMOD_E_NONE;
}



int jupiter_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{        
    int i, start_lane, num_lane;
    uint32_t rx_bist_lock;
    uint64_t err_count;
    uint32_t err_count_done = 0, err_count_overflown = 0;
    phymod_phy_access_t phy_copy;



    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    prbs_status->error_count = 0;
    prbs_status->prbs_lock_loss = 0;
    prbs_status->prbs_lock = 1;

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);

        PHYMOD_IF_ERR_RETURN(jupiter_pmd_rx_chk_lock_state_get(&phy_copy.access, &rx_bist_lock));
        if (rx_bist_lock) {
                PHYMOD_IF_ERR_RETURN
                    (jupiter_pmd_rx_chk_err_count_state_get(&phy_copy.access, &err_count, &err_count_done,
                                                            &err_count_overflown));
                //PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS Error count :: %llu\n", i, err_count));
                if (err_count_done &&
                    !err_count_overflown) {
                    
                    prbs_status->error_count += COMPILER_64_LO(err_count);
                }
                
                PHYMOD_IF_ERR_RETURN
                    (jupiter_pmd_rx_chk_err_count_state_clear(&phy_copy.access));
        } else {
                PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS not locked\n", i ));
                prbs_status->prbs_lock = 0;
        }
    }

    return PHYMOD_E_NONE;
}


int jupiter_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int jupiter_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int jupiter_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int jupiter_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int jupiter_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{        
    int start_lane, num_lane, i;
    phymod_phy_access_t phy_copy;
    phymod_diag_type_t cmd_type;
    jupiter_uc_diag_regs_t uc_diag;

    if (!type) {
        cmd_type = phymod_diag_DSC;
    } else if ((!PHYMOD_STRCMP(type, "ber")) ||
            (!PHYMOD_STRCMP(type, "Ber")) ||
            (!PHYMOD_STRCMP(type, "BER"))) {
        cmd_type = phymod_diag_BER;
    } else if ((!PHYMOD_STRCMP(type, "config")) ||
            (!PHYMOD_STRCMP(type, "Config")) ||
            (!PHYMOD_STRCMP(type, "CONFIG"))) {
        cmd_type = phymod_diag_CFG;
    } else if ((!PHYMOD_STRCMP(type, "cl72")) ||
            (!PHYMOD_STRCMP(type, "Cl72")) ||
            (!PHYMOD_STRCMP(type, "CL72"))) {
        cmd_type = phymod_diag_CL72;
    } else if ((!PHYMOD_STRCMP(type, "debug")) ||
            (!PHYMOD_STRCMP(type, "Debug")) ||
            (!PHYMOD_STRCMP(type, "DEBUG"))) {
        cmd_type = phymod_diag_DEBUG;
    } else if ((!PHYMOD_STRCMP(type, "state")) ||
            (!PHYMOD_STRCMP(type, "State")) ||
            (!PHYMOD_STRCMP(type, "STATE"))) {
        cmd_type = phymod_diag_STATE;
    } else if ((!PHYMOD_STRCMP(type, "state_eye")) ||
            (!PHYMOD_STRCMP(type, "State_Eye")) ||
            (!PHYMOD_STRCMP(type, "STATE_EYE"))) {
        cmd_type = phymod_diag_STATE_EYE;
    } else if ((!PHYMOD_STRCMP(type, "dfe")) ||
            (!PHYMOD_STRCMP(type, "Dfe")) ||
            (!PHYMOD_STRCMP(type, "DFE"))) {
        cmd_type = phymod_diag_DFE_TAPS;
    } else if ((!PHYMOD_STRCMP(type, "verbose")) ||
            (!PHYMOD_STRCMP(type, "Verbose")) ||
            (!PHYMOD_STRCMP(type, "VERBOSE"))) {
        cmd_type = phymod_diag_ALL;
    } else if (!PHYMOD_STRCMP(type, "STD")) {
        cmd_type = phymod_diag_DSC_STD;
    } else {
        cmd_type = phymod_diag_STATE;
    }

    PHYMOD_DEBUG_ERROR((" %s:%d type = %d laneMask  = 0x%X\n", __func__, __LINE__, cmd_type, phy->access.lane_mask));


    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    if (cmd_type == phymod_diag_DSC) {
       for (i = start_lane; i < start_lane + num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << i ;

            PHYMOD_DIAG_OUT(("\n+--------------------------------------------------------------------+\n"));
            PHYMOD_DIAG_OUT(("Lane mask = 0x%x\n", phy_copy.access.lane_mask));
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_uc_diag_reg_dump(&phy_copy.access, &uc_diag));
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_read_status(&phy_copy.access, -1));
       }
    } else {
        PHYMOD_DEBUG_ERROR(("Not supported type = %d\n", cmd_type));
    }

    return PHYMOD_E_NONE;

}


int jupiter_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int jupiter_phy_PAM4_tx_pattern_enable_set(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t enable)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int jupiter_phy_PAM4_tx_pattern_enable_get(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t* enable)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int jupiter_phy_tx_pattern_set(const phymod_phy_access_t* phy, phymod_phy_tx_pattern_t tx_pattern)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int jupiter_phy_tx_pattern_get(const phymod_phy_access_t* phy, phymod_phy_tx_pattern_t* tx_pattern)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int jupiter_phy_ber_proj(const phymod_phy_access_t* phy, phymod_ber_proj_mode_t mode, const phymod_phy_ber_proj_options_t* options)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int jupiter_phy_fast_ber_proj_get(const phymod_phy_access_t* phy, uint32_t* ber_proj_data)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int jupiter_phy_linkcat(const phymod_phy_access_t* phy, phymod_linkcat_config_t linkcat_config)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int jupiter_phy_pmd_lane_diag_debug_level_set(const phymod_phy_access_t* phy, uint32_t level)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int jupiter_phy_pmd_lane_diag_debug_level_get(const phymod_phy_access_t* phy, uint32_t* level)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


#endif /* PHYMOD_JUPITER_SUPPORT */

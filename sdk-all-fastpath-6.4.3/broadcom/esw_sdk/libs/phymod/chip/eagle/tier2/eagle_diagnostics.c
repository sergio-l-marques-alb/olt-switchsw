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

#ifdef PHYMOD_EAGLE_SUPPORT

#include "../../eagle/tier1/eagle_tsc_enum.h"
#include "../../eagle/tier1/eagle_tsc_common.h"
#include "../../eagle/tier1/eagle_tsc_interface.h"
#include "../../eagle/tier1/eagle_tsc_dependencies.h"
#include "../../eagle/tier1/eagle_tsc_debug_functions.h"

int eagle_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

STATIC err_code_t eagle_phy_meas_lowber_eye (const phymod_access_t *pa, 
                                             const phymod_phy_eyescan_options_t *eyescan_options,
                                             uint32_t *buffer) 
{
    struct eagle_tsc_eyescan_options_st e_options;

    e_options.linerate_in_khz = eyescan_options->linerate_in_khz;
    e_options.timeout_in_milliseconds = eyescan_options->timeout_in_milliseconds;
    e_options.horz_max = eyescan_options->horz_max;
    e_options.horz_min = eyescan_options->horz_min;
    e_options.hstep = eyescan_options->hstep;
    e_options.vert_max = eyescan_options->vert_max;
    e_options.vert_min = eyescan_options->vert_min;
    e_options.vstep = eyescan_options->vstep;
    e_options.mode = eyescan_options->mode;

    return (eagle_tsc_meas_lowber_eye(pa, e_options, buffer));
}

STATIC err_code_t eagle_phy_display_lowber_eye (const phymod_access_t *pa, 
                                                const phymod_phy_eyescan_options_t *eyescan_options,
                                                uint32_t *buffer) 
{
    struct eagle_tsc_eyescan_options_st e_options;

    e_options.linerate_in_khz = eyescan_options->linerate_in_khz;
    e_options.timeout_in_milliseconds = eyescan_options->timeout_in_milliseconds;
    e_options.horz_max = eyescan_options->horz_max;
    e_options.horz_min = eyescan_options->horz_min;
    e_options.hstep = eyescan_options->hstep;
    e_options.vert_max = eyescan_options->vert_max;
    e_options.vert_min = eyescan_options->vert_min;
    e_options.vstep = eyescan_options->vstep;
    e_options.mode = eyescan_options->mode;

    return(eagle_tsc_display_lowber_eye (pa, e_options, buffer)); 
} 


static void _eagle_diag_uc_reg_dump(const phymod_access_t *pa)
{
    err_code_t errc;

    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|    MICRO CODE USR CTRL CONFIGURATION REGISTERS  |\n"));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|    config_word              [0x00]: 0x%04X      |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x0)));
    PHYMOD_DEBUG_ERROR(("|    retune_after_restart     [0x02]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x2)));
    PHYMOD_DEBUG_ERROR(("|    clk90_offset_adjust      [0x03]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x3)));
    PHYMOD_DEBUG_ERROR(("|    clk90_offset_override    [0x04]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x4)));
    PHYMOD_DEBUG_ERROR(("|    lane_event_log_level     [0x05]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x5)));
    PHYMOD_DEBUG_ERROR(("|    disable_startup          [0x06]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x6)));
    PHYMOD_DEBUG_ERROR(("|    disable_startup_dfe      [0x07]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x7)));
    PHYMOD_DEBUG_ERROR(("|    disable_steady_state     [0x08]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x8)));
    PHYMOD_DEBUG_ERROR(("|    disable_steady_state_dfe [0x09]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x9)));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|         MICRO CODE USER STATUS REGISTERS        |\n"));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|    restart_counter           [0x0A]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xa)));
    PHYMOD_DEBUG_ERROR(("|    reset_counter             [0x0B]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xb)));
    PHYMOD_DEBUG_ERROR(("|    pmd_lock_counter          [0x0C]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xc)));
    PHYMOD_DEBUG_ERROR(("|    heye_left                 [0x0D]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xd)));
    PHYMOD_DEBUG_ERROR(("|    heye_left_lsbyte          [0x0E]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xe)));
    PHYMOD_DEBUG_ERROR(("|    heye_right                [0x0F]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xf)));
    PHYMOD_DEBUG_ERROR(("|    heye_right_lsbyte         [0x10]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x10)));
    PHYMOD_DEBUG_ERROR(("|    veye_upper                [0x11]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x11)));
    PHYMOD_DEBUG_ERROR(("|    veye_upper_lsbyte         [0x12]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x12)));
    PHYMOD_DEBUG_ERROR(("|    veye_lower                [0x13]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x13)));
    PHYMOD_DEBUG_ERROR(("|    veye_lower_lsbyte         [0x14]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x14)));
    PHYMOD_DEBUG_ERROR(("|    micro_stopped             [0x15]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x15)));
    PHYMOD_DEBUG_ERROR(("|    link_time                 [0x16]: 0x%04X     |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x16)));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|            MICRO CODE MISC REGISTERS            |\n"));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|    usr_diag_status           [0x18]: 0x%04X     |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x18)));
    PHYMOD_DEBUG_ERROR(("|    usr_diag_rd_ptr           [0x1A]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x1a)));
    PHYMOD_DEBUG_ERROR(("|    usr_diag_mode             [0x1B]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x1b)));
    PHYMOD_DEBUG_ERROR(("|    usr_var_msb               [0x1C]: 0x%04X     |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x1c)));
    PHYMOD_DEBUG_ERROR(("|    usr_var_lsb               [0x1E]: 0x%04X     |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x1e)));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
}

STATIC int eagle_diagnostics_eyescan_run_uc(const phymod_phy_access_t* phy, uint32 flags)   
{
    int                 ii, rc = PHYMOD_E_NONE;
    uint32_t            stripe[64];
    uint16_t            status;

    if(PHYMOD_EYESCAN_F_ENABLE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_meas_eye_scan_start(&(phy->access), 0));

        if(!PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_GET(flags)) {
            PHYMOD_USLEEP(100000);
        }
    }

    if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {

        for (ii = 31; ii >= -31; ii--) { 
            if (ii == 31) {
                PHYMOD_DIAG_OUT(("\n\n\n"));
                PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                PHYMOD_DIAG_OUT(("    | EYESCAN Phy: %02d                                            |\n", phy->access.addr));
                PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                /*display eyescan header*/
                PHYMOD_IF_ERR_RETURN(eagle_tsc_display_eye_scan_header(&(phy->access), 1));
            }

            rc  = eagle_tsc_read_eye_scan_stripe(&(phy->access), stripe, &status);
            if(rc != PHYMOD_E_NONE) {
                _eagle_diag_uc_reg_dump(&(phy->access));
                PHYMOD_IF_ERR_RETURN(rc);
            }
            PHYMOD_IF_ERR_RETURN(eagle_tsc_display_eye_scan_stripe(&(phy->access), ii, &stripe[0]));

            PHYMOD_DIAG_OUT(("\n"));
        }

        if (rc == PHYMOD_E_NONE) {
            PHYMOD_IF_ERR_RETURN(eagle_tsc_display_eye_scan_footer(&(phy->access), 1));
            PHYMOD_DIAG_OUT(("\n"));
        }
    }
    
    if(PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_meas_eye_scan_done(&(phy->access)));
    }

    return PHYMOD_E_NONE;
}

STATIC int eagle_diagnostics_eyescan_run_lowber( 
    const phymod_phy_access_t* phy,
    uint32 flags,
    const phymod_phy_eyescan_options_t* eyescan_options
    )   
{
    uint32_t            buffer[64*64];

    /*enable eyescan*/
    if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {

        PHYMOD_IF_ERR_RETURN(eagle_phy_meas_lowber_eye (&(phy->access), eyescan_options, buffer));

        PHYMOD_IF_ERR_RETURN(eagle_phy_display_lowber_eye (&(phy->access), eyescan_options, buffer));
    }

    if(PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_pmd_uc_cmd(&(phy->access), CMD_CAPTURE_BER_END, 0, 2000));
    }

    return PHYMOD_E_NONE;
}

int eagle_phy_eyescan_run(const phymod_phy_access_t* phy, 
                           uint32_t flags, 
                           phymod_eyescan_mode_t mode, 
                           const phymod_phy_eyescan_options_t* eyescan_options)
{
    /* If stage isn't set - perform all stages*/
    if(!PHYMOD_EYESCAN_F_ENABLE_GET(flags)
       && !PHYMOD_EYESCAN_F_PROCESS_GET(flags)
       && !PHYMOD_EYESCAN_F_DONE_GET(flags)) 
    {
        PHYMOD_EYESCAN_F_ENABLE_SET(flags);
        PHYMOD_EYESCAN_F_PROCESS_SET(flags);
        PHYMOD_EYESCAN_F_DONE_SET(flags);
    }


    switch(mode) {
        case phymodEyescanModeFast:
            return eagle_diagnostics_eyescan_run_uc(phy, flags);
        case phymodEyescanModeLowBER:
            return eagle_diagnostics_eyescan_run_lowber(phy, flags, eyescan_options);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }
}



#endif /* PHYMOD_EAGLE_SUPPORT */

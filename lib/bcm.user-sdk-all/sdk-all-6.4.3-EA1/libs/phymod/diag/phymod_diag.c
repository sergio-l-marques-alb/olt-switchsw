/* 
 * $Id: phymod_diag.c $ 
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
#include <phymod/phymod_diag.h>

#ifdef PHYMOD_QSGMIIE_SUPPORT
#include <phymod/chip/bcmi_qsgmiie_serdes_sym.h>
#endif /*PHYMOD_QSGMIIE_SUPPORT  */
#ifdef PHYMOD_TSCE_SUPPORT
#include <phymod/chip/bcmi_tsce_xgxs_sym.h>
#endif /*PHYMOD_TSCE_SUPPORT  */
#ifdef PHYMOD_TSCF_SUPPORT
#include <phymod/chip/bcmi_tscf_xgxs_sym.h>
#endif /*PHYMOD_TSCF_SUPPORT  */
#ifdef PHYMOD_EAGLE_SUPPORT
#include <phymod/chip/bcmi_eagle_xgxs_sym.h>
#endif /*PHYMOD_EAGLE_SUPPORT  */
#ifdef PHYMOD_FALCON_SUPPORT
#include <phymod/chip/bcmi_falcon_xgxs_sym.h>
#endif /*PHYMOD_FALCON_SUPPORT  */


#define _PHYMOD_PHY_MSG(phy, string) ("Phy 0x%x lanes 0x%02x: " string), (phy)->access.addr, (phy)->access.lane_mask
#define _PHYMOD_CORE_MSG(core, string) ("Core 0x%x: " string), (core)->access.addr

#define PHYMOD_DIAG_PRINT_FUNC_VALIDATE if(phymod_diag_print_func == NULL) return PHYMOD_E_IO

print_func_f phymod_diag_print_func = NULL;


int phymod_diag_symbols_table_get(phymod_phy_access_t *phy, phymod_symbols_t **symbols)
{
    switch(phy->type){
#ifdef PHYMOD_QSGMIIE_SUPPORT
    case phymodDispatchTypeQsgmiie:
        *symbols = &bcmi_qsgmiie_serdes_symbols;
        break;
#endif /*PHYMOD_QSGMIIE_SUPPORT  */
#ifdef PHYMOD_TSCE_SUPPORT
    case phymodDispatchTypeTsce:
        *symbols = &bcmi_tsce_xgxs_symbols;
        break;
#endif /*PHYMOD_TSCE_SUPPORT  */
#ifdef PHYMOD_TSCF_SUPPORT
    case phymodDispatchTypeTscf:   
        *symbols = &bcmi_tscf_xgxs_symbols;
        break;
#endif /*PHYMOD_TSCF_SUPPORT  */
#ifdef PHYMOD_EAGLE_SUPPORT
    case phymodDispatchTypeEagle:
        *symbols = &bcmi_eagle_xgxs_symbols;
        break;
#endif /*PHYMOD_EAGLE_SUPPORT  */
#ifdef PHYMOD_FALCON_SUPPORT
    case phymodDispatchTypeFalcon:
        *symbols = &bcmi_falcon_xgxs_symbols;
        break;
#endif /*PHYMOD_FALCON_SUPPORT  */
    default:
        phymod_diag_print_func(_PHYMOD_PHY_MSG(phy,"unsupported SerDes type(%d) for symbolic access \n"), phy->type);
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}

/******************************************************************************
 Firmware load
******************************************************************************/

int phymod_diag_firmware_load(phymod_core_access_t *cores, int array_size, char *firwmware_file){
    int i = 0;

    /*unsigned short firmware_crc;
    unsigned short firmware_verision;
    unsigned short firmware_length;*/
    unsigned short phy_type = -1;
    
    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;
    
    
      
    for(i = 0 ; i < array_size ; i++){
        if(cores[i].type != phy_type){
            phymod_diag_print_func(_PHYMOD_CORE_MSG(&cores[i],"The firmware is not compatible with the core\n"));
            return PHYMOD_E_FAIL;
        }
    }
    return PHYMOD_E_NONE;
}


/******************************************************************************
 Register read/write
******************************************************************************/
int phymod_diag_reg_read(phymod_phy_access_t *phys, int array_size, uint32_t reg_addr){
    uint32_t val = 0;
    int rv;
    int i;

    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;
    
    for(i = 0 ; i < array_size ; i++){
       rv = phymod_phy_reg_read(&phys[i], reg_addr, &val);
       if(rv == PHYMOD_E_NONE){
           phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Reg 0x%08x: 0x%04x\n"), reg_addr, val);
       }
       else{
           phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Reg 0x%08x: Error read register!\n"), reg_addr);
       }
    }
    return PHYMOD_E_NONE;
}


int phymod_diag_reg_write(phymod_phy_access_t *phys, int array_size, uint32_t reg_addr, uint32_t val){
    int i;
    int rv;

    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;
    
    for(i = 0 ; i < array_size ; i++){
       rv = phymod_phy_reg_write(&phys[i], reg_addr, val);
       if(rv != PHYMOD_E_NONE){
           phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Reg 0x%08x: Error write register!\n"), reg_addr);
       }
    }
    return PHYMOD_E_NONE;
}


/******************************************************************************
 PRBS
******************************************************************************/
STATIC
void phymod_diag_prbs_get_results_print(phymod_phy_access_t *phy, phymod_prbs_status_t *status) {
    if(status->prbs_lock == 0){
        phymod_diag_print_func(_PHYMOD_PHY_MSG(phy, "prbs unlocked\n"));
    }
    if(status->prbs_lock_loss == 1){
        phymod_diag_print_func(_PHYMOD_PHY_MSG(phy, "prbs unlocked during the PRBS test\n"));
    }
    if(status->error_count > 0){
        phymod_diag_print_func(_PHYMOD_PHY_MSG(phy, "%d errors during PRBS test\n"), status->error_count);
    }
    else{
        phymod_diag_print_func(_PHYMOD_PHY_MSG(phy, "PRBS OK!\n"));
    }
}


STATIC
int phymod_diag_prbs_set(phymod_phy_access_t *phys, int array_size, phymod_diag_prbs_set_args_t *params){
    int i = 0;
    int rv;
    
    phymod_prbs_t_validate(&params->prbs_options);
    
       
    if(params->loopback == 1){
        for(i = 0 ; i < array_size ; i++){
            rv = phymod_phy_loopback_set(&phys[i], phymodLoopbackGlobal, 1);
            if(rv != PHYMOD_E_NONE){
                phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed set loopback\n"));
                return rv;
            }
        }
    }    
    for(i = 0 ; i < array_size ; i++){
        if(phymod_phy_prbs_config_set(&phys[i], params->flags, &params->prbs_options) != PHYMOD_E_NONE){
            phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed set PRBS\n"));
            return PHYMOD_E_FAIL;
        }
        if(phymod_phy_prbs_enable_set(&phys[i], params->flags,  params->enable) != PHYMOD_E_NONE){
            phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed enable PRBS\n"));
            return PHYMOD_E_FAIL;
        }

    }
    return PHYMOD_E_NONE;
}



STATIC
int phymod_diag_prbs_get(phymod_phy_access_t *phys, int array_size, phymod_diag_prbs_get_args_t *params){
    int i = 0;
    int rv;
    phymod_prbs_status_t status;
    uint32_t enable;
    uint32_t flags = 0;

    
    for(i = 0 ; i < array_size ; i++){

        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        rv = phymod_phy_prbs_enable_get(&phys[i], flags, &enable);
        if(rv != PHYMOD_E_NONE){
            phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed get PRBS configuration\n"));
            return rv;
        }
        if(enable == 0){
            phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"PRBS status get can not be called when rx PRBS is disabled\n"));
            return PHYMOD_E_FAIL;
        }
        /*clear the status*/
        rv = phymod_phy_prbs_status_get(&phys[i], PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ, &status);
        if(rv != PHYMOD_E_NONE){
            phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"PRBS clear status failed\n"));
            return rv;
        }
    }
    /*wait*/
    PHYMOD_SLEEP(params->time);

    /*read the status*/
    for(i = 0 ; i < array_size ; i++){
        rv = phymod_phy_prbs_status_get(&phys[i], PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ, &status);
        if(rv != PHYMOD_E_NONE){
            phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"PRBS status read failed\n"));
            return rv;
        }
        phymod_diag_prbs_get_results_print(&phys[i], &status);
    }
    return PHYMOD_E_NONE;
}


STATIC
int phymod_diag_prbs_clear(phymod_phy_access_t *phys, int array_size, phymod_diag_prbs_clear_args_t *params){
    int i = 0;
    int rv;
    
    for(i = 0 ; i < array_size ; i++){
        rv = phymod_phy_prbs_enable_set(&phys[i], params->flags, 0);
        if(rv  != PHYMOD_E_NONE){
            phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed clear prbs\n"));
            return rv;
        }
    }
    return PHYMOD_E_NONE;
}


int phymod_diag_prbs(phymod_phy_access_t *phys, int array_size, phymod_diag_prbs_args_t *prbs_diag_args){
    int rv = PHYMOD_E_NONE;

    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;

    switch(prbs_diag_args->prbs_cmd){
    case PhymodDiagPrbsClear:
        rv = phymod_diag_prbs_clear(phys, array_size, &prbs_diag_args->args.clear_params);
        break;
    case PhymodDiagPrbsGet:
        rv = phymod_diag_prbs_get(phys, array_size, &prbs_diag_args->args.get_params);
        break;
    case PhymodDiagPrbsSet:
        rv = phymod_diag_prbs_set(phys, array_size, &prbs_diag_args->args.set_params);
        break;
    default:
        phymod_diag_print_func("Failed parsing PRBS command type\n");
        rv = PHYMOD_E_FAIL;
    }
    return rv;
}


/******************************************************************************
DSC
******************************************************************************/
int phymod_diag_dsc(phymod_phy_access_t *phys, int array_size){
    phymod_phy_diagnostics_t phy_diag;
    phymod_core_diagnostics_t core_diag;
    phymod_rx_t rx;
    phymod_tx_t tx;
    phymod_firmware_lane_config_t fw_lane_config;
    phymod_firmware_core_config_t fw_core_config;

    phymod_core_access_t core;
    int i = 0;
    int lane;
    int lane_index;
    int rv;
    char *fw_mode_str;
    char *osr;
    char *pmd_mode;

    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;
    
    for(i = 0 ; i < array_size ; i++){
        PHYMOD_MEMCPY(&core.access, &phys[i].access, sizeof(core.access));
        core.access.lane_mask = 0xf;
        core.type = phys[i].type;
        rv = phymod_core_diagnostics_get(&core, &core_diag);
        if(rv  != PHYMOD_E_NONE){
            phymod_diag_print_func(_PHYMOD_CORE_MSG(&core, "Failed get core diagnostics data\n"));
            return rv;
        }
        phymod_diag_print_func(_PHYMOD_CORE_MSG(&core, "temprature: %uc  pll range: %u\n") ,core_diag.temperature, core_diag.pll_range);
    }
    

    phymod_diag_print_func("LN   AD (CDRxN,UC_CFG) ");
    phymod_diag_print_func("SD LOCK RXPPM CLK90 CLKP1  PF(M,L) VGA P1_mV M1_mV     DFE(1,2,3,4,5,dcd1,dcd2)   SLICER(ze,zo,pe,po,me,mo) ");
    phymod_diag_print_func("TXPPM TXEQ(n1,m,p1,p2) EYE(L,R,U,D)\n");
    for(i = 0 ; i < array_size ; i++){
        lane = phys[i].access.lane_mask;
        for( lane_index = 0 ; lane_index < PHYMOD_MAX_LANES_PER_CORE; lane_index++){
            /*if lane is not selected pass*/
            if( (lane & (1<<lane_index)) == 0){
                continue;
            }
            phys[i].access.lane_mask = lane & (1<<lane_index);
            /*collect info*/
            rv = phymod_phy_diagnostics_get(&phys[i], &phy_diag);
            if(rv  != PHYMOD_E_NONE){
                phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed get phy diagnostics data\n"));
                return rv;
            }
            rv = phymod_phy_rx_get(&phys[i], &rx);
            if(rv  != PHYMOD_E_NONE){
                phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed get rx data\n"));
                return rv;
            }
            rv = phymod_phy_tx_get(&phys[i], &tx);
            if(rv  != PHYMOD_E_NONE){
                phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed get tx data\n"));
                return rv;
            }
            rv = phymod_phy_firmware_lane_config_get(&phys[i], &fw_lane_config);
            if(rv  != PHYMOD_E_NONE){
                phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed get fw lane mode data\n"));
                return rv;
            }

            rv = phymod_phy_firmware_core_config_get(&phys[i], &fw_core_config);
            if(rv  != PHYMOD_E_NONE){
                phymod_diag_print_func(_PHYMOD_PHY_MSG(&phys[i],"Failed get fw core data\n"));
                return rv;
            }
            
            /*enums trnaslation*/
            if (fw_lane_config.DfeOn) {
                fw_mode_str = "dfe";
            } else if (fw_lane_config.ForceBrDfe) {
                fw_mode_str = "brdfe";
            } else {
                fw_mode_str = "unknown";
            }

            switch(phy_diag.osr_mode){
            case phymodOversampleMode1:
                osr = "x1";
                break;
            case phymodOversampleMode2:
                osr = "x2";
                break;
            case phymodOversampleMode3:
                osr = "x3";
                break;
            case phymodOversampleMode3P3:
                osr = "x3.3";
                break;
            case phymodOversampleMode4:
                osr = "x4";
                break;
            case phymodOversampleMode5:
                osr = "x5";
                break;
            case phymodOversampleMode8:
                osr = "x8";
                break;
            case phymodOversampleMode8P25:
                osr = "x8.25";
                break;
           case phymodOversampleMode10:
                osr = "x10";
                break;
            default:
                osr = "unknown";
            }

            switch(phy_diag.pmd_mode){
            case phymodPmdModeBrDfe:
                pmd_mode = "BR";
                break;
            case phymodPmdModeOs:
                pmd_mode = "OS";
                break;
            case phymodPmdModeOsDfe:
                pmd_mode = "DFE";
                break;

            default:
                pmd_mode = "unknown";

            }

           /*printing*/
           phymod_diag_print_func("%2d 0x%02x ", lane_index, phys[i].access.addr);
           phymod_diag_print_func("(%3s%-5s,%4s)", pmd_mode, osr, fw_mode_str);
           phymod_diag_print_func(" %1d   %1d  ", phy_diag.signal_detect, phy_diag.rx_lock);
           phymod_diag_print_func(" %4d ", phy_diag.rx_ppm);
           phymod_diag_print_func(" %3d   %3d ", phy_diag.clk90_offset, phy_diag.clkp1_offset);
           phymod_diag_print_func("   %2d,%d ", rx.peaking_filter.value, rx.low_freq_peaking_filter.value);
           phymod_diag_print_func("   %2d ", rx.vga.value); 
           phymod_diag_print_func("%4d   %3d", phy_diag.p1_lvl, phy_diag.m1_lvl);
           phymod_diag_print_func("    %2d,%3d,%3d,%3d,%3d,%3d,%3d  ", rx.dfe[0].value, rx.dfe[1].value, rx.dfe[2].value, rx.dfe[3].value, rx.dfe[4].value, phy_diag.dfe1_dcd, phy_diag.dfe2_dcd);
           phymod_diag_print_func("  %3d,%3d,%3d,%3d,%3d,%3d   ", phy_diag.slicer_offset.offset_ze, phy_diag.slicer_offset.offset_zo, phy_diag.slicer_offset.offset_pe, phy_diag.slicer_offset.offset_po, phy_diag.slicer_offset.offset_me, phy_diag.slicer_offset.offset_mo);
           phymod_diag_print_func("%4d ", phy_diag.tx_ppm);
           phymod_diag_print_func("  %2d,%2d,%2d,%2d ", tx.pre, tx.main, tx.post, tx.post2); 
           phymod_diag_print_func("   %2d,%2d,%2d,%2d ", phy_diag.eyescan.heye_left, phy_diag.eyescan.heye_right, phy_diag.eyescan.veye_upper, phy_diag.eyescan.veye_lower); 
           phymod_diag_print_func("\n");
        }

    }
    return PHYMOD_E_NONE;
}

/******************************************************************************
Eyescan
******************************************************************************/
STATIC
void _phymod_display_eye_scan_header(void) 
{
    phymod_diag_print_func(("\n"));
    phymod_diag_print_func(("  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30"));
    phymod_diag_print_func(("\n"));
    phymod_diag_print_func(("         : -|----|----|----|----|----|----|----|----|----|----|----|----|-"));
    phymod_diag_print_func(("\n"));
}

STATIC
int _phymod_display_eye_scan_stripe(const phymod_phy_access_t *pa, int8_t y, uint32_t *buffer) 
{
    const uint32_t limits[7] = {1835008, 183501, 18350, 1835, 184, 18, 2};
    int8_t x,i;
    int16_t level;
    int rv;

    rv = phymod_phy_ladder_setting_to_mV(pa, y, &level);
    PHYMOD_IF_ERR_RETURN(rv);

    phymod_diag_print_func("%6dmV : " ,level);
 
    for (x=-31;x<32;x++) {
      for (i=0;i<7;i++) 
        if (buffer[x+31]>=limits[i]) {
            phymod_diag_print_func("%c", '0'+i+1);
            break;
        }
        if (i==7) {
            if ((x%5)==0 && (y%5)==0) {
                phymod_diag_print_func(("+"));
            } else if ((x%5)!=0 && (y%5)==0) {
                phymod_diag_print_func(("-"));
            } else if ((x%5)==0 && (y%5)!=0) {
                phymod_diag_print_func((":"));
            } else {
                phymod_diag_print_func((" "));
            }
        }
    }

    return PHYMOD_E_NONE;
}

STATIC 
void _phymod_display_eye_scan_footer(void)
{
    phymod_diag_print_func(("         : -|----|----|----|----|----|----|----|----|----|----|----|----|-"));
    phymod_diag_print_func(("\n"));
    phymod_diag_print_func(("  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30"));
    phymod_diag_print_func(("\n"));
}

int phymod_diag_eyescan_run_uc( 
    phymod_phy_access_t *phys,
    int num_phys
    )   
{
    const phymod_phy_access_t    *pa;
    int                 idx, ii, rc = PHYMOD_E_NONE, rv;
    uint32_t            stripe[64];
    uint16_t            status;

    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;

    /*enable eyescan*/
    for (idx = 0; idx < num_phys; idx++) {

        pa = &(phys[idx]);

        rc = phymod_phy_meas_eye_scan_start(pa, 0); /* 0 = Horiz, 1 = Vertical */
        if (rc != PHYMOD_E_NONE) {
            phymod_diag_print_func(_PHYMOD_PHY_MSG(pa,"Failed in phymod_phy_meas_eye_scan_start \n"));
            goto exit;
        }
    }

    PHYMOD_USLEEP(100000);

    /* Read Scan Stripe */
    for (idx = 0; idx < num_phys; idx++) {

        pa = &(phys[idx]);

        for (ii = 31; ii >= -31; ii--) { 
            if (ii == 31) {
                phymod_diag_print_func("\n\n\n");
                phymod_diag_print_func("    +--------------------------------------------------------------------+\n");
                phymod_diag_print_func("    | EYESCAN Phy: %02d Idx: %02d                                           |\n", pa->access.addr, idx);
                phymod_diag_print_func("    +--------------------------------------------------------------------+\n");
                /*display eyescan header*/
                _phymod_display_eye_scan_header();
            }

            PHYMOD_USLEEP(100000);

            rc = phymod_phy_read_eye_scan_stripe(pa, stripe, &status); 
            if (rc != PHYMOD_E_NONE) {
                phymod_phy_eye_scan_debug_info_dump(pa);
                phymod_diag_print_func(_PHYMOD_PHY_MSG(pa,"Failed in phymod_phy_eye_scan_debug_info_dump, idx: %d, rc: %d \n"), idx, rc);
                break;
            }
            rc = _phymod_display_eye_scan_stripe(pa, ii, &stripe[0]);
            if (rc != PHYMOD_E_NONE) {
                phymod_diag_print_func(_PHYMOD_PHY_MSG(pa,"Failed in _phymod_display_eye_scan_stripe, idx: %d, rc: %d \n"), idx, rc);
                break;
            }
            phymod_diag_print_func("\n");
        }

        if (rc == PHYMOD_E_NONE) {
            _phymod_display_eye_scan_footer();
            phymod_diag_print_func("\n");
        }
    }
    

exit:
    /*disable eyescan mode*/
    for (idx = 0; idx < num_phys; idx++) {

        pa = &(phys[idx]);

        rv = phymod_phy_meas_eye_scan_done(pa);
        if (rv != PHYMOD_E_NONE) {
            phymod_diag_print_func(_PHYMOD_PHY_MSG(pa,"Failed in phymod_phy_meas_eye_scan_done, idx: %d, rc: %d \n"), idx, rc);
            rc = rv;
        }
    }

    return rc;
}


int phymod_diag_eyescan_run_lowber( 
    phymod_phy_access_t *phys,
    int* line_rates, 
    int num_phys,
    phymod_phy_eyescan_options_t* eyescan_options
    )   
{
    const phymod_phy_access_t    *pa;
    int                 idx, rc = PHYMOD_E_NONE;
    uint32_t            buffer[64*64];
    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;

    phymod_diag_print_func(" Timeout = %d ms\n",  eyescan_options->timeout_in_milliseconds);
    phymod_diag_print_func(" Hmax    = %d\n",     eyescan_options->horz_max);
    phymod_diag_print_func(" Hmin    = %d\n",     eyescan_options->horz_min);
    phymod_diag_print_func(" Hstep   = %d\n",     eyescan_options->hstep);
    phymod_diag_print_func(" Vmax    = %d\n",     eyescan_options->vert_max);
    phymod_diag_print_func(" Vmin    = %d\n",     eyescan_options->vert_min);
    phymod_diag_print_func(" Vstep   = %d\n",     eyescan_options->vstep);
    phymod_diag_print_func(" mode    = %d\n",     eyescan_options->mode);

    /*enable eyescan*/
    for (idx = 0; idx < num_phys; idx++) {

        pa = &(phys[idx]);

        eyescan_options->linerate_in_khz = line_rates[idx];

        rc = phymod_phy_meas_lowber_eye(pa, *eyescan_options, buffer); 
        if (rc != PHYMOD_E_NONE) {
            phymod_diag_print_func(_PHYMOD_PHY_MSG(pa,"Failed in phymod_phy_meas_lowber_eye \n"));
            goto exit;
        }

        rc = phymod_phy_display_lowber_eye(pa, *eyescan_options, buffer);
        if (rc != PHYMOD_E_NONE) {
            phymod_diag_print_func(_PHYMOD_PHY_MSG(pa,"Failed in phymod_phy_display_lowber_eye \n"));
            goto exit;
        }
    }

exit:    
    /*disable eyescan mode*/
    for (idx = 0; idx < num_phys; idx++) {

        pa = &(phys[idx]);

        rc = phymod_phy_pmd_ber_end_cmd(pa, 0, 2000);
        if (rc != PHYMOD_E_NONE) {
            phymod_diag_print_func(_PHYMOD_PHY_MSG(pa,"Failed in phymod_phy_pmd_ber_end_cmd, idx: %d, rc: %d \n"), idx, rc);
        }
    }

    return rc;
}

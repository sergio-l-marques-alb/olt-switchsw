/*
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
 */

#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/drv.h>

#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "../../../phy8481.h"
#include <soc/phyreg.h>
#include <soc/phy.h>
#include "../../../phyreg.h"

#define  KOI_MERLIN_DEPENDENCIES

#include "merlin_koi_dependencies.h"

/* Change XGP table to point to XFI */
#define XGP_POINT_TO_XFI(unit, pc)\
    do {\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4110, 0x2004));\
    } while(0)

/* Restore XGP table*/
#define RESTORE_XGP(unit, pc)\
    do {\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4110, 0x3));\
    } while(0)

#define WRITE_KOI_PMAD_REG(unit, pc, addr, val)\
    do {\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4110, 0x2004));\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_PMAD_REG(unit, pc, addr, val));\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4110, 0x3));\
    } while(0)

static phy_ctrl_t *pc_int = NULL;

void set_pc(phy_ctrl_t *pc) {
    pc_int = pc;
}

err_code_t merlin_koi_pmd_mwr_reg(uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {

    uint16_t tmp, otmp;
    uint16_t reg_val;
    val = val << lsb;
    val = val & mask ;
 
    XGP_POINT_TO_XFI(pc_int->unit, pc_int);
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PMAD_REG(pc_int->unit, pc_int, addr, &reg_val)); 
    RESTORE_XGP(pc_int->unit, pc_int);

    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= val;

    if (otmp != tmp) {
          WRITE_KOI_PMAD_REG(pc_int->unit, pc_int, addr, tmp);
    }

  return(0);
}

int logger_write(int message_verbose_level, const char *format, ...) {

	return ( 0 );
}

err_code_t merlin_koi_pmd_rdt_reg(uint16_t address, uint16_t *data){
   
   if(pc_int) { 
        uint16_t val=0;
        XGP_POINT_TO_XFI(pc_int->unit, pc_int);
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(pc_int->unit, pc_int, address, &val));        
        RESTORE_XGP(pc_int->unit, pc_int);
        *data = (uint16_t)val;
   }
    return ( 0 );
}

err_code_t merlin_koi_pmd_wr_reg(uint16_t address, uint16_t val){
   
    if(pc_int) {
        WRITE_KOI_PMAD_REG(pc_int->unit, pc_int, address, val);    
    }
 
    return(0);
}

err_code_t merlin_koi_delay_us(uint32_t delay_us){

    sal_usleep(delay_us);
    return ( 0 );
}

err_code_t merlin_koi_delay_ns(uint16_t delay_ns) {

    if (delay_ns > 1000) {
        return(0);
    }
    return ( 0 );
}

uint8_t merlin_koi_get_lane(void) {
    return ( 0 );
}

uint8_t merlin_koi_get_core(void) {
	return ( 0 );
}

err_code_t merlin_koi_uc_lane_idx_to_system_id(char string[16], uint8_t uc_lane_idx) {
     return(0);
}

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
 *  $Id$
 */

/***************************************************************************
****************************************************************************
*  File Name     :  falcon_furia_dependencies.c                            *
*  Created On    :  05/29/2014                                             *
*  Created By    :  Sharad Sharma                                          *
*  Description   :  Version of Dependency APIs used in Eval                *
*  Revision      :  $Id: $                                                 *
*                                                                          *
*  Copyright 2013  Broadcom Corporation                                    *
*  All Rights Reserved                                                     *
*  No portions of this material may be reproduced in any form without      *
*  the written permission of:                                              *
*      Broadcom Corporation                                                *
*      5300 California Avenue                                              *
*      Irvine, CA  92617                                                   *
*                                                                          *
*  All information contained in this document is Broadcom Corporation      *
*  company private proprietary, and trade secret.                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <phymod/phymod.h>
#include "merlin_dino_interface.h"
#include "merlin_dino_dependencies.h"
#include "../common/srds_api_err_code.h"
#include "../../bcmi_dino_defs.h"
#include "../../dino_cfg_seq.h"

err_code_t merlin_dino_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val) {
    uint32_t data = 0;
    
    READ_DINO_PMA_PMD_REG(pa, (uint32_t)address, data);
    *val = (uint16_t)data;
    return ( ERR_CODE_NONE );
}

err_code_t merlin_dino_pmd_wr_reg(const phymod_access_t *pa, uint16_t address, uint16_t val) {
    uint32_t data = 0xffff & val;

    WRITE_DINO_PMA_PMD_REG(pa, (uint32_t)address, data);
    return(ERR_CODE_NONE);
}

err_code_t merlin_dino_pmd_mwr_reg(const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {
    uint16_t tmp, otmp;
    uint32_t reg_val;
    val = val << lsb;
    
    val = val & mask ;
    /* Use clause 45 access if supported */
    READ_DINO_PMA_PMD_REG(pa, addr, reg_val);
    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= val;

    if (otmp != tmp) {
        WRITE_DINO_PMA_PMD_REG(pa, addr, tmp);
    }
    return(ERR_CODE_NONE);
}

err_code_t merlin_dino_delay_ns(uint16_t delay_ns) {
    if (delay_ns > 1000) {
        return (ERR_CODE_SERDES_DELAY);
    }

    return (ERR_CODE_NONE);
} 


err_code_t merlin_dino_delay_us(uint32_t delay_us) {
    PHYMOD_USLEEP(delay_us);

    return (ERR_CODE_NONE);
}

err_code_t merlin_dino_delay_ms(uint32_t delay_ms) {
    PHYMOD_USLEEP(delay_ms*1000);

    return (ERR_CODE_NONE);
}

uint8_t merlin_dino_get_lane(const phymod_access_t *pa) {
    uint8_t lane_mask = 0, lane = 0; 
    uint32_t reg_val = 0;
    
    READ_DINO_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, reg_val);
    /* lane from 0-3 */
    lane_mask = (reg_val & 0xF);
    switch (lane_mask) {
        case 1:
            lane = 0;
        break;
        case 2:
            lane = 1;
        break;
        case 4:
            lane = 2;
        break;
        case 8:
            lane = 3;
        break;
        default:
        break;
    }

    return lane;
}

uint8_t merlin_dino_get_core(const phymod_access_t *pa)
{
    uint32_t reg_val;
    uint8_t core_mask = 0, core = 0; 
    
    READ_DINO_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, reg_val);

    core_mask = (reg_val & 0x70) >> 4;

    switch (core_mask) {
        case 1:
            core = 0;
        break;
        case 2:
            core = 1;
        break;
        case 4:
            core = 2;
        break;
        default:
        break;
    }

    return (core);
}

err_code_t merlin_dino_uc_lane_idx_to_system_id(char string[16], uint8_t uc_lane_idx)
{
    string[0]= '\0';
    return(0);
}


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
#include "falcon2_madura_interface.h"
#include "falcon2_madura_dependencies.h"
#include "../common2/srds_api_err_code.h"
#include "../../bcmi_madura_defs.h"
#include "../../madura_cfg_seq.h"
/*
#include "falcon2_madura_ram_regs.h"
#include "falcon2_madura_core_lane_access.h"
*/

static const uint16_t M0_DATA_RAM_LENGTH    = 0x1400;     /* fixed by hardware design */

err_code_t falcon2_madura_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val) {
    uint32_t data = 0;

    READ_MADURA_PMA_PMD_REG(pa, (uint32_t)address, data);
    *val = (uint16_t)data;
    return ( ERR_CODE_NONE );
}

err_code_t falcon2_madura_pmd_wr_reg(const phymod_access_t *pa, uint16_t address, uint16_t val) {
    uint32_t data = 0xffff & val;

    WRITE_MADURA_PMA_PMD_REG(pa, (uint32_t) address, data);
    return(ERR_CODE_NONE);
}

err_code_t falcon2_madura_pmd_mwr_reg(const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {
    uint16_t tmp, otmp;
    uint32_t reg_val;
    val = val << lsb;
    
    val = val & mask ;
    /* Use clause 45 access if supported */
    READ_MADURA_PMA_PMD_REG(pa, addr, reg_val);
    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= val;

    if (otmp != tmp) {
        WRITE_MADURA_PMA_PMD_REG(pa, addr, tmp);
    }
    return(ERR_CODE_NONE);
}

err_code_t falcon2_madura_delay_ns(uint16_t delay_ns) {

  if (delay_ns > 1000) {
    return (ERR_CODE_SERDES_DELAY);
  }
  return (ERR_CODE_NONE);
} 


err_code_t falcon2_madura_delay_us(uint32_t delay_us) {
  PHYMOD_USLEEP(delay_us);
  return (ERR_CODE_NONE);
}

uint8_t falcon2_madura_get_lane(const phymod_access_t *pa) {
    uint8_t i = 0 , lane = -1 ; 
    uint32_t reg_val,val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, reg_val);
    val = reg_val;
    val = val & 0xF; /*lane from 0-3*/
    for(i=0;i<4;i++)
    {
         if((1<<i)& val)/*considering that only one lane is selected*/
	 {
	      lane=i;
  	      break;
	 }
    } 

    return (lane);
}
err_code_t falcon2_madura_set_lane(const phymod_access_t *pa, uint8_t lane_index)
{
    uint32_t reg_val;
    
    if (lane_index > 3) {
        return  ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }

    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, reg_val);
    reg_val &= 0xFFF0; /*lane from 0-3*/
    reg_val |= (1<< lane_index);

    WRITE_MADURA_PMA_PMD_REG(pa, (uint32_t) DEV1_SLICE_SLICE_ADR, reg_val);
  return(0);
}

uint8_t falcon2_madura_get_core(const phymod_access_t *pa)
{
    uint32_t val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, val);
    val = (val >> 8) & 0xF; 

    if( (val & 0x3) == 0x3 ||  (val & 0x3) == 0)
    { 
        /* both cores or none */
	  return ERR_CODE_INVALID_RAM_ADDR;
    }
    if( (val & 0xC) == 0xC ||  (val & 0xC) == 0)
    { 
        /* both sys/line or none */
	  return ERR_CODE_INVALID_RAM_ADDR;
    }
    if( (val & 0x6) == 0x6 )
    { 
        /* both sys core 1  */
	  return ERR_CODE_INVALID_RAM_ADDR;
    }

    switch (val) {
        case 0x5:
            return 0; /* sys core 0 */
        case 0x9:
            return 2; /* line core 0 */
        case 0xA:
            return 3; /* line core 1 */
    }
 return 0;
}

err_code_t falcon2_madura_uc_lane_idx_to_system_id(const phymod_access_t *pa, char string[16], uint8_t uc_lane_idx)
{
  string[0]= '\0';
  return(0);
}

uint8_t falcon2_madura_get_pll(const phymod_access_t *pa)
{
    uint32_t val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, val);

    return (val & 0x1000) ? 1 : 0;
}

err_code_t falcon2_madura_set_pll(const phymod_access_t *pa, uint8_t pll_index) 
{
    uint32_t val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, val);

    val  |=   (0x1000) ;

    if (pll_index == 0) {
        val ^= 0x1000;
    }

    WRITE_MADURA_PMA_PMD_REG(pa, (uint32_t) DEV1_SLICE_SLICE_ADR, val);

    return(0);
}

err_code_t falcon2_madura_set_core(const phymod_access_t *pa, uint8_t core_id)
{
    uint32_t val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, val);
    val &=  0xF0FF;  /* clear core / side slection */

    switch (core_id) {
        case 0x0:
            val |= 0x0500; /* sys core 0 */
            break;
        case 0x2:
            val |= 0x0900; /* line core 0 */
            break;
        case 0x3:
            val |= 0x0A00; /* line core 1 */
            break;
        default:
            return  ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }
    WRITE_MADURA_PMA_PMD_REG(pa, (uint32_t) DEV1_SLICE_SLICE_ADR, val);
    return 0;
}


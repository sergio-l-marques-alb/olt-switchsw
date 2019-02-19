/*
 * $Id: jer_regs.h,v 1.0 Broadcom SDK $
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
 */
#ifndef _JER_REGS_H_
#define _JER_REGS_H_


int soc_jer_regs_eci_access_check(int unit);

/*
 * Function:
 *      soc_jer_regs_blocks_access_check_regs
 * Purpose:
 *      performs a perliminary sanity check for access to few registers of different sizes.
 * Parameters:
 *      unit            - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_regs_blocks_access_check_regs(int unit);

/*
 * Function:
 *      soc_jer_regs_blocks_access_check_mems
 * Purpose:
 *      performs a perliminary sanity check for access a memory.
 *      SHOULD NOT BE USED ON WIDE MEMORIES!
 * Parameters:
 *      unit            - Device Number
 *      test_mem        - Tested memory
 *      block_num       - Block number, should be filled using <block name>_BLOCK(unit, instance) MACRO
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_regs_blocks_access_check_mem(int unit, soc_mem_t test_mem, int block_num);

/*
 * Function:
 *      soc_jer_regs_blocks_access_check_dma
 * Purpose:
 *      performs a perliminary sanity check for access via dma.
 * Parameters:
 *      unit            - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_regs_blocks_access_check_dma(int unit);

/*
 * Function:
 *      soc_jer_regs_blocks_access_check
 * Purpose:
 *      performs a perliminary sanity check for few registers and memories of differnet size, and checks dma access
 * Parameters:
 *      unit            - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_regs_blocks_access_check(int unit);

#endif /* _JER_REGS_H_ */

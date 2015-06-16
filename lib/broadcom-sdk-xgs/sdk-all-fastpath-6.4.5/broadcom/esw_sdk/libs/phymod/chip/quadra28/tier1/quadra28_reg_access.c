/*
 *
 * $Id: quadra28_reg_access.c 2014/12/18 Aman Exp $
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
 *
*/

/*
 * Includes
 */
#include <phymod/phymod_acc.h>
#include <phymod/phymod_reg.h>

/*
 *  Defines
 */

/**   Read Register
 *    This function is used to read the register content of given
 *    register address
 *
 *    @param pa                 Pointer to phymod access structure
 *    @param addr               Register address supplied by user
 *    @param data               Regiser content/value of the given register
 *
 *    @return ioerr             Error return
 *                              0 - Success
 *                              any other value - Error
 */
int phymod_raw_iblk_read(const phymod_access_t *pa, uint32_t reg_addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t reg_val = 0;
    int32_t dev_add = ((reg_addr>>16) & 0x1f);

    if (pa == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("NULL parameter")));
    }
    if (dev_add == 0) {
        ioerr = PHYMOD_BUS_READ(pa, ((1<<16) | reg_addr), &reg_val);
    } else {
        ioerr = PHYMOD_BUS_READ(pa, reg_addr, &reg_val);
    }
    *data = reg_val;
    return ioerr;
}


/**   Write Register
 *    This function is used to write the content to given register
 *
 *    @param pa                 Pointer to phymod access structure
 *    @param addr               Register address supplied by user
 *    @param data               Regiser content/value to be written to register
 *
 *    @return ioerr             Error return
 *                              0 - Success
 *                              any other value - Error
 */
int phymod_raw_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{

    int ioerr = 0;
    int32_t dev_add = ((addr>>16) & 0x1f);
    if (pa == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("NULL parameter")));
    }
    data &= 0xffff;
    if (dev_add == 0) {
        ioerr = PHYMOD_BUS_WRITE(pa, ((1<<16) | addr), data);
    } else {
        ioerr = PHYMOD_BUS_WRITE(pa, addr, data);
    }

    return ioerr;
}


/**   Modify Register
 *    This function is used to modify the content without overwriting it
 *
 *    @param pa                 Pointer to phymod access structure
 *    @param addr               Register address supplied by user
 *    @param reg_data           Regiser data for modified portion
 *    @param reg_mask           Register mask for the data bits to be modified
 *
 *    @return ioerr             Error return
 *                              0 - Success
 *                              any other value - Error
 */
int quadra28_reg_modify(const phymod_access_t *pa,
                     uint32_t addr,
                     uint16_t reg_data,
                     uint16_t reg_mask)
{

    int ioerr = 0;
	uint16_t tmp, otmp;
	uint32_t reg_val;
   
    if (pa == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("NULL parameter")));
    }

    reg_data = reg_data & reg_mask;

    /* Use clause 45 access if supported */
    ioerr += PHYMOD_BUS_READ(pa, addr, &reg_val);
        
    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(reg_mask);
    tmp |= reg_data;
    if (otmp != tmp) {
        ioerr += PHYMOD_BUS_WRITE(pa, addr, tmp);
    }	
    return ioerr;


}

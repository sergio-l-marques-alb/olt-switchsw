/*
 * $Id: $
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

#include <shbde_mdio.h>

/* iProc MDIO register offset */
#define MII_MGMT_CTRL                       0x0
#define MII_MGMT_CMD_DATA                   0x4

/* iProc MII register with fields definition */
#define MII_MGMT_CTRLr_MDCDIVf_SHFT         0
#define MII_MGMT_CTRLr_MDCDIVf_MASK         0x7f
#define MII_MGMT_CTRLr_BSYf_SHFT            8
#define MII_MGMT_CTRLr_BSYf_MASK            0x1

#define MII_MGMT_CMD_DATAr_DATAf_SHFT       0
#define MII_MGMT_CMD_DATAr_DATAf_MASK       0xffff
#define MII_MGMT_CMD_DATAr_TAf_SHFT         16
#define MII_MGMT_CMD_DATAr_TAf_MASK         0x3
#define MII_MGMT_CMD_DATAr_RAf_SHFT         18
#define MII_MGMT_CMD_DATAr_RAf_MASK         0x1f
#define MII_MGMT_CMD_DATAr_PAf_SHFT         23
#define MII_MGMT_CMD_DATAr_PAf_MASK         0x1f
#define MII_MGMT_CMD_DATAr_OPf_SHFT         28
#define MII_MGMT_CMD_DATAr_OPf_MASK         0x3
#define MII_MGMT_CMD_DATAr_SBf_SHFT         30
#define MII_MGMT_CMD_DATAr_SBf_MASK         0x3

/* Register field value set/get */
#define REG_FIELD_SET(_r, _f, _r_val, _f_val) \
        _r_val = ((_r_val) & ~(_r##_##_f##_MASK << _r##_##_f##_SHFT)) | \
                 (((_f_val) & _r##_##_f##_MASK) << _r##_##_f##_SHFT)
#define REG_FIELD_GET(_r, _f, _r_val) \
        (((_r_val) >> _r##_##_f##_SHFT) & _r##_##_f##_MASK)

#define LOG_OUT(_shbde, _lvl, _str, _prm)             \
    if ((_shbde)->log_func) {                         \
        (_shbde)->log_func(_lvl, _str, _prm);         \
    }
#define LOG_ERR(_shbde, _str, _prm)     LOG_OUT(_shbde, SHBDE_ERR, _str, _prm)
#define LOG_WARN(_shbde, _str, _prm)    LOG_OUT(_shbde, SHBDE_WARN, _str, _prm)
#define LOG_DBG(_shbde, _str, _prm)     LOG_OUT(_shbde, SHBDE_DBG, _str, _prm)

static unsigned int
mdio32_read(shbde_mdio_ctrl_t *smc, unsigned int offset)
{
    if (!smc || !smc->io32_read) {
        return 0;
    }
    return smc->io32_read(smc->shbde, smc->regs, smc->base_addr + offset);
}

static void
mdio32_write(shbde_mdio_ctrl_t *smc, unsigned int offset, unsigned int data)
{
    if (!smc || !smc->io32_read) {
        return;
    }
    smc->io32_write(smc->shbde, smc->regs, smc->base_addr + offset, data);
}

static void
wait_usec(shbde_mdio_ctrl_t *smc, int usec)
{
    shbde_hal_t *shbde = smc->shbde;

    if (shbde && shbde->usleep) {
        shbde->usleep(usec);
    } else {
        int idx;
        volatile int count;
        for (idx = 0; idx < usec; idx++) {
            for (count = 0; count < 100; count++);
        }
    }
}

static int
iproc_mdio_wait_for_busy(shbde_mdio_ctrl_t *smc)
{
    int mii_busy;
    unsigned int reg_val;
    int count = 1000;

    /* Wait until MII is not busy */
    do {
        reg_val = mdio32_read(smc, MII_MGMT_CTRL);
        mii_busy = REG_FIELD_GET(MII_MGMT_CTRLr, BSYf, reg_val);
        if (!mii_busy) {
            break;
        }
        wait_usec(smc, 10);
        count --;
    } while (count > 0);

    return mii_busy;
}

int
shbde_iproc_mdio_init(shbde_mdio_ctrl_t *smc)
{
    shbde_hal_t *shbde = smc->shbde;
    unsigned int reg_val = 0;

    /* Enable the iProc internal MDIO interface */
    REG_FIELD_SET(MII_MGMT_CTRLr, MDCDIVf, reg_val, 0x7f);
    mdio32_write(smc, MII_MGMT_CTRL, reg_val);

    if (shbde && !shbde->usleep) {
        LOG_DBG(shbde, "shbde_mdio: no registration of usleep vector", 0);
    }

    wait_usec(smc, 100);

    return 0;
}

int
shbde_iproc_mdio_read(shbde_mdio_ctrl_t *smc, unsigned int phy_addr,
                      unsigned int reg, unsigned int *val)
{
    unsigned int reg_val = 0;

    REG_FIELD_SET(MII_MGMT_CMD_DATAr, SBf, reg_val, 0x1);
    REG_FIELD_SET(MII_MGMT_CMD_DATAr, TAf, reg_val, 0x2);
    REG_FIELD_SET(MII_MGMT_CMD_DATAr, OPf, reg_val, 0x2);
    REG_FIELD_SET(MII_MGMT_CMD_DATAr, PAf, reg_val, phy_addr);
    REG_FIELD_SET(MII_MGMT_CMD_DATAr, RAf, reg_val, reg);
    mdio32_write(smc, MII_MGMT_CMD_DATA, reg_val);

    if (iproc_mdio_wait_for_busy(smc)) {
        *val = 0;
        LOG_DBG(smc->shbde, "shbde_iproc_mdio_read busy", reg);
        return -1;
    }

    reg_val = mdio32_read(smc, MII_MGMT_CMD_DATA);
    *val = REG_FIELD_GET(MII_MGMT_CMD_DATAr, DATAf, reg_val);

    return 0;
}

int
shbde_iproc_mdio_write(shbde_mdio_ctrl_t *smc, unsigned int phy_addr,
                       unsigned int reg, unsigned int val)
{
    unsigned int reg_val = 0;

    REG_FIELD_SET(MII_MGMT_CMD_DATAr, SBf, reg_val, 0x1);
    REG_FIELD_SET(MII_MGMT_CMD_DATAr, TAf, reg_val, 0x2);
    REG_FIELD_SET(MII_MGMT_CMD_DATAr, OPf, reg_val, 0x1);
    REG_FIELD_SET(MII_MGMT_CMD_DATAr, PAf, reg_val, phy_addr);
    REG_FIELD_SET(MII_MGMT_CMD_DATAr, RAf, reg_val, reg);
    REG_FIELD_SET(MII_MGMT_CMD_DATAr, DATAf, reg_val, val);
    mdio32_write(smc, MII_MGMT_CMD_DATA, reg_val);

    if (iproc_mdio_wait_for_busy(smc)) {
        LOG_DBG(smc->shbde, "shbde_iproc_mdio_write busy", reg);
        return -1;
    }

    /* Wait for some time for the write to take effect */
    wait_usec(smc, 100);

    return 0;
}


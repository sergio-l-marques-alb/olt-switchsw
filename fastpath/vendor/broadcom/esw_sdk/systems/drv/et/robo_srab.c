/*
 * $Id: robo_srab.c 1.7 Broadcom SDK $
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
 * BCM5301X SRAB utility functions
 */

/* #define EXPORT_SYMTAB */
#include <linux-bde.h>
#include <typedefs.h>
#include <shared/et/linux_osl.h>
#include <aiutils.h>
#include <aiutils_priv.h>
#include <sbchipc.h>
#include <bcmutils.h>
#include <bcmendian.h>
#include <soc/devids.h>
#include "robo_srab.h"
#include "nssrabreg.h"
#include <shared/et/bcmdevs.h>

#ifdef	BCMDBG
#define	SRAB_ERR(args)	printk args
#else
#define	SRAB_ERR(args)
#endif

#define SI_CC_IDX   0

/* Private state per RoboSwitch */
typedef struct {
    void *sbh;        /* SiliconBackplane handle */

    nssrabregs_t *regs;      /* pointer to chip registers */

#ifdef _KERNEL_
    spinlock_t lock;        /* per-device perimeter lock */    
#endif
} robo_info_t;

/* Forward declarations */

void *robo_attach(void *sbh);
void robo_detach(void *robo);
void robo_wreg(void *rinfo, uint8 cid, uint8 page, uint8 addr, uint8 *buf, uint len);
void robo_rreg(void *rinfo, uint8 cid, uint8 page, uint8 addr, uint8 *buf, uint len);

static void *robo_sbh = NULL;

#define ROBO_POLL_TIMEOUT 1000
#define ROBO_IF_TIMEOUT 10000

#define CCB_SRAB_BASE 0x18007000
#define NSP_CCB_SRAB_BASE 0x18036000
#define CORE_SIZE 0x1000

/* Get access to the RoboSwitch */
void *
robo_attach(void *sih)
{
    robo_info_t *robo;
    si_info_t *sii;
    nssrabregs_t *regs;      /* pointer to chip registers */
	struct si_pub *sii_pub = NULL;
	uint32	base_addr;

    sii = SI_INFO((si_t*)sih);

    /* Allocate private state */
    if (!(robo = MALLOC(sii->osh, sizeof(robo_info_t)))) {
        SRAB_ERR(("robo_attach: out of memory"));
        return NULL;
    } else {
        robo_sbh = (void *)robo;
    }
    bzero((char *) robo, sizeof(robo_info_t));

    robo->sbh = sih;
	sii_pub = &sii->pub;

	if (sii_pub->chip == BCM53020_CHIP_ID) {
		base_addr = NSP_CCB_SRAB_BASE;
	} else {
		base_addr = CCB_SRAB_BASE;
	}
    if ((regs = (void*)REG_MAP(base_addr, CORE_SIZE)) == NULL) {
        SRAB_ERR(("robo_attach: can't get base address"));
        return NULL;
    }
/*    printk("%d: robo_attach: regs=0x%x\n",__LINE__,(uint32)regs);*/
    robo->regs = regs;

#ifdef _KERNEL_
    /* Initialize lock */
    spin_lock_init(&robo->lock);
#endif
    return robo;
}

/* Release access to the RoboSwitch */
void
robo_detach(void *rinfo)
{
    robo_info_t *robo = (robo_info_t *)rinfo;
    si_info_t *sii;

    sii = SI_INFO((si_t*)robo->sbh);

	if (robo->regs) {
		REG_UNMAP(robo->regs);
		robo->regs = NULL;
	}

    COMPILER_REFERENCE(sii);

    /* Free private state */
    MFREE(sii->osh, robo, sizeof(robo_info_t));
}

static
void _switch_interface_reset(void *rinfo)
{
    uint32 timeout;
    robo_info_t *robo = (robo_info_t *)rinfo;
    si_info_t *sii;
    osl_t       *osh;
    nssrabregs_t *regs;      /* pointer to chip registers */

    sii = SI_INFO((si_t*)robo->sbh);
    osh = sii->osh;
    regs = robo->regs;

    COMPILER_REFERENCE(osh);

    /* Wait for switch initialization complete */
    timeout = ROBO_IF_TIMEOUT;
    while (!(R_REG(osh, &regs->chipcommonb_srab_sw_if) & CHIPCOMMONB_SRAB_SW_IF_SW_INIT_DONE_MASK)) {
        if (!timeout--) {
            SRAB_ERR(("srab reset switch interface: timeout"));
            break;
        }
    }

    /* Set the SRAU reset bit */
    W_REG(osh, &regs->chipcommonb_srab_cmdstat, CHIPCOMMONB_SRAB_CMDSTAT_SRA_RST_MASK);
    /* Wait for it to auto-clear */
    timeout = ROBO_IF_TIMEOUT;
    while (R_REG(osh, &regs->chipcommonb_srab_cmdstat) & CHIPCOMMONB_SRAB_CMDSTAT_SRA_RST_MASK) {
        if (!timeout--) {
            SRAB_ERR(("srab reset switch interface: timeout sra_rst"));
            return;
        }
    }
}

static
void _switch_request_grant(void *rinfo)
{
    uint32 regval;
    uint32 timeout = ROBO_IF_TIMEOUT;
    robo_info_t *robo = (robo_info_t *)rinfo;
    si_info_t *sii;
    osl_t       *osh;
    nssrabregs_t *regs;      /* pointer to chip registers */

    sii = SI_INFO((si_t*)robo->sbh);
    osh = sii->osh;
    regs = robo->regs;

    COMPILER_REFERENCE(osh);

	regval = R_REG(osh, &regs->chipcommonb_srab_sw_if);
    regval |= CHIPCOMMONB_SRAB_SW_IF_RCAREQ_MASK;

    W_REG(osh, &regs->chipcommonb_srab_sw_if, regval);
    while (!(R_REG(osh, &regs->chipcommonb_srab_sw_if) & CHIPCOMMONB_SRAB_SW_IF_RCAGNT_MASK)) {
        if (!timeout--) {
            SRAB_ERR(("srab request grant: timeout"));
            return;
        }
    }
}

static
void _switch_release_grant(void *rinfo)
{
    uint32 regval;
    robo_info_t *robo = (robo_info_t *)rinfo;
    si_info_t *sii;
    osl_t       *osh;
    nssrabregs_t *regs;      /* pointer to chip registers */

    sii = SI_INFO((si_t*)robo->sbh);
    osh = sii->osh;
    regs = robo->regs;

    COMPILER_REFERENCE(osh);

	regval = R_REG(osh, &regs->chipcommonb_srab_sw_if);
    regval &= ~CHIPCOMMONB_SRAB_SW_IF_RCAREQ_MASK;
    W_REG(osh, &regs->chipcommonb_srab_sw_if, regval);
}

static
uint64 _switch_reg_read(void *rinfo, uint8 page, uint8 offset)
{
    uint64 value = ~(uint64)0;
    uint32 regval;
    uint32 timeout = ROBO_POLL_TIMEOUT;

    robo_info_t *robo = (robo_info_t *)rinfo;
    si_info_t *sii;
    osl_t       *osh;
    nssrabregs_t *regs;      /* pointer to chip registers */

    sii = SI_INFO((si_t*)robo->sbh);
    osh = sii->osh;
    regs = robo->regs;

    COMPILER_REFERENCE(osh);

    /* Assemble read command */
    _switch_request_grant(rinfo);
    regval = ((page << CHIPCOMMONB_SRAB_CMDSTAT_SRA_PAGE_SHIFT)
              | (offset << CHIPCOMMONB_SRAB_CMDSTAT_SRA_OFFSET_SHIFT)
              | CHIPCOMMONB_SRAB_CMDSTAT_SRA_GORDYN_MASK);
    W_REG(osh, &regs->chipcommonb_srab_cmdstat, regval);

    /* Wait for command complete */
    while (R_REG(osh, &regs->chipcommonb_srab_cmdstat) & CHIPCOMMONB_SRAB_CMDSTAT_SRA_GORDYN_MASK) {
        if (!--timeout) {
			SRAB_ERR(("robo_read: timeout"));
            _switch_interface_reset(rinfo);
            break;
        }
    }
    if (timeout) {
        /* Didn't time out, read and return the value */
        value = (((uint64)R_REG(osh, &regs->chipcommonb_srab_rdh)) << 32)
                        | R_REG(osh, &regs->chipcommonb_srab_rdl);
    }

    _switch_release_grant(rinfo);
    return value;
}

static
void _switch_reg_write(void *rinfo, uint8 page, uint8 offset, uint64 value)
{
    uint32 regval;
    uint32 timeout = ROBO_POLL_TIMEOUT;
    robo_info_t *robo = (robo_info_t *)rinfo;
    si_info_t *sii;
    osl_t       *osh;
    nssrabregs_t *regs;      /* pointer to chip registers */

    sii = SI_INFO((si_t*)robo->sbh);
    osh = sii->osh;
    regs = robo->regs;

    COMPILER_REFERENCE(osh);

    _switch_request_grant(rinfo);
    /* Load the value to write */
    W_REG(osh, &regs->chipcommonb_srab_wdh, (uint32)(value >> 32));
    W_REG(osh, &regs->chipcommonb_srab_wdl, (uint32)(value));

    /* Issue the write command */
    regval = ((page << CHIPCOMMONB_SRAB_CMDSTAT_SRA_PAGE_SHIFT)
              | (offset << CHIPCOMMONB_SRAB_CMDSTAT_SRA_OFFSET_SHIFT)
              | CHIPCOMMONB_SRAB_CMDSTAT_SRA_GORDYN_MASK
              | CHIPCOMMONB_SRAB_CMDSTAT_SRA_WRITE_MASK);
    W_REG(osh, &regs->chipcommonb_srab_cmdstat, regval);
    /* Wait for command complete */
    while (R_REG(osh, &regs->chipcommonb_srab_cmdstat) & CHIPCOMMONB_SRAB_CMDSTAT_SRA_GORDYN_MASK) {
        if (!--timeout) {
			SRAB_ERR(("robo_write: timeout"));
            _switch_interface_reset(rinfo);
            break;
        }
    }
    _switch_release_grant(rinfo);

}


/* Write chip register */
void
robo_wreg(void *rinfo, uint8 cid, uint8 page, uint8 addr, uint8 *buf, uint len)
{
	uint64 regval;
    uint8  *data8_ptr;

#ifdef _KERNEL_
    unsigned long flags;
    spin_lock_irqsave(&robo->lock, flags);
#endif

    regval = 0;
    data8_ptr = (uint8 *)&regval;
    memcpy(data8_ptr, buf, len);

    _switch_reg_write(rinfo, page, addr, regval);

#ifdef _KERNEL_
    spin_unlock_irqrestore(&robo->lock, flags);
#endif
}

/* Read chip register */
void
robo_rreg(void *rinfo, uint8 cid, uint8 page, uint8 addr, uint8 *buf, uint len)
{
	uint64 regval;
    uint8  *data8_ptr;

#ifdef _KERNEL_
    unsigned long flags;
    spin_lock_irqsave(&robo->lock, flags);
#endif

    regval = _switch_reg_read(rinfo, page, addr);

    data8_ptr = (uint8_t *)&regval;
    memcpy(buf, data8_ptr, len);

#ifdef _KERNEL_
    spin_unlock_irqrestore(&robo->lock, flags);
#endif
}


/*
 * Bsafe uHSM Strata wrapper function s
 *
 * $Copyright: Copyright 2009 Broadcom Corporation.
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

#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <sal/appl/io.h>

#include "bsafe_core_if.h"
#include "bsafe_regs.h"

sal_mutex_t	bsafe_lock[BCM_MAX_NUM_UNITS];

int bsafe_strata_initialize(int fd, int reset)
{
	bsafe_initialize(&fd, TRUE);
	return 0;
}

int bsafe_strata_open(int unit, int reset)
{
	int fd = unit;
	bsafe_lock[fd] = sal_mutex_create("bsafe");
	bsafe_strata_initialize(fd, reset);
	return fd;
}

int bsafe_strata_close(int fd)
{
	int unit = fd;
	bsafe_clean(&fd);
	sal_mutex_destroy(bsafe_lock[unit]);
	bsafe_lock[unit] = NULL;
	return 0;
}

int bsafe_strata_isr_callback(void *param)
{
	bsafe_isr_callback(param);
	return 0;
}

#define LOCK(fd)	sal_mutex_take(bsafe_lock[fd], sal_mutex_FOREVER)
#define UNLOCK(fd)	sal_mutex_give(bsafe_lock[fd])

int bsafe_strata_ioctl(int fd, int command, void *cmdio)
{
	int ret=0;

	LOCK(fd);
		/* here fd is the context */
		ret= bsafe_ioctl((void *)&fd, command, cmdio);
	UNLOCK(fd);
	return ret;
}

int bsafe_write_uint32(void * dev_context, int address, uint32 *data_in)
{
	int unit = *((int *)dev_context);
	uint32 addr;

	addr = (soc_reg_addr(unit, BSAFE_GLB_TIMERr, REG_PORT_ANY, 0) +
		(address - 0x200));

	SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, *data_in));

    	debugk(DK_VERBOSE,
	       "bsafe_write_uint32: write %08x: %08x\n", address, *data_in);

	return 4;
}

int bsafe_read_uint32(void * dev_context, int address, uint32 *data_out)
{
	int unit = *((int *)dev_context);
	uint32 addr;

	addr = (soc_reg_addr(unit, BSAFE_GLB_TIMERr, REG_PORT_ANY, 0) +
		(address - 0x200));

	SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, data_out));

    	debugk(DK_VERBOSE,
	       "bsafe_read_uint32: read %08x: %08x\n", address, *data_out);

	return 4;
}

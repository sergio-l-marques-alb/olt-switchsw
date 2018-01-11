/*
 * $Id: diag_cint_data.c,v 1.47 Broadcom SDK $
 *
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
#include <appl/diag/system.h>

#include <shared/bsl.h>
#include <sdk_config.h>

#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/register.h>
#include <soc/mem.h>

#if defined(INCLUDE_LIB_CINT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

static void diag_printk(char *str)
{
    cli_out("%s", str);
}

#define DIAG_PRINTK_FLAGS_HEX 0x1

static void diag_printk_int(int num, int flags)
{
    if(flags & DIAG_PRINTK_FLAGS_HEX) {
        cli_out("%x",(uint32)num);
    } else {
        cli_out("%d",num);
    }
}

static void diag_pcie_read(int unit, uint32 addr, uint32 *val, int swap)
{

    *val = CMVEC(unit).read(&CMDEV(unit).dev, addr);

    if(swap) {
        *val = _shr_swap32(*val);
    }

    /* cli_out("%s(): unit=%d, addr=0x%x, swap=%d. after swap: *val=0x%x",
               FUNCTION_NAME(), unit, addr, swap, *val); */
}

static void diag_pcie_write(int unit, uint32 addr, uint32 val, int swap)
{
    /* cli_out("%s(): unit=%d, addr=0x%x, val=0x%x, swap=%d",
               FUNCTION_NAME(), unit, addr, val, swap); */

    if(swap) {
        val = _shr_swap32(val);
    }

    CMVEC(unit).write(&CMDEV(unit).dev, addr, val);

}

CINT_FWRAPPER_CREATE_VP1(diag_printk,
                         char*, char, str, 1, 0);

CINT_FWRAPPER_CREATE_VP2(diag_printk_int,
                         int, int, num, 0, 0,
                         int, int, flags, 0, 0);

CINT_FWRAPPER_CREATE_VP4(diag_pcie_read,
                         int,int,unit,0,0,
                         uint32,uint32,addr,0,0,
                         uint32*,uint32,val,1,0,
                         int,int,swap,0,0);

CINT_FWRAPPER_CREATE_VP4(diag_pcie_write,
                         int,int,unit,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,val,0,0,
                         int,int,swap,0,0);


static cint_function_t __cint_diag_functions[] =
{
    CINT_FWRAPPER_ENTRY(diag_printk),
    CINT_FWRAPPER_ENTRY(diag_printk_int),
    CINT_FWRAPPER_ENTRY(diag_pcie_read),
    CINT_FWRAPPER_ENTRY(diag_pcie_write),

    CINT_ENTRY_LAST
};

static cint_constants_t __cint_diag_constants[] =
{
   { "DIAG_PRINTK_FLAGS_HEX", DIAG_PRINTK_FLAGS_HEX },
   { NULL }
};

cint_data_t diag_cint_data =
{
    NULL,
    __cint_diag_functions,
    NULL,
    NULL,
    NULL,
    __cint_diag_constants,
    NULL
};

#endif /* INCLUDE_LIB_CINT*/


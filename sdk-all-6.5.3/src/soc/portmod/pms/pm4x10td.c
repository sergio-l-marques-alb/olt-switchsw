/*
 *
 * $Id:$
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
 *         
 *     
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_common.h>
#include <soc/mcm/memregs.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X10TD_SUPPORT

#include <soc/portmod/xlmac.h>
#include <soc/portmod/pm4x10.h>

int pm4x10td_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return (READ_PGW_CNTMAXSIZEr(unit, port, (uint32_t *)val));
}

int pm4x10td_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    int rv = 0;
    uint32 reg_val;

    rv = READ_PGW_CNTMAXSIZEr(unit, port, &reg_val);
    if (rv) return (rv);

    soc_reg_field_set(unit, PGW_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, val);

    return(WRITE_PGW_CNTMAXSIZEr(unit, port, reg_val));
}

int pm4x10td_port_mac_rsv_mask_set(int unit, int port, pm_info_t pm_info, uint32 rsv_mask)
{
        return(WRITE_PGW_MAC_RSV_MASKr(unit, port, rsv_mask));
}

int pm4x10td_port_mib_reset_toggle(int unit, int port, pm_info_t pm_info, int port_index)
{
    /* PGW_MIB_RESET needs to be controlled in portctrl layer
       Do nothing in Portmod */
    return 0;
}

#endif /* PORTMOD_PM4X10TD_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

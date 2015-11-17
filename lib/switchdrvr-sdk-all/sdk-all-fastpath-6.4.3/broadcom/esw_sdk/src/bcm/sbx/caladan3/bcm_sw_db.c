/*
 * $Id: bcm_sw_db.c,v 1.20 Broadcom SDK $
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
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BCM_DBG_NORMAL
#include <shared/bsl.h>
#include <soc/sbx/caladan3/soc_sw_db.h>
#include <soc/scache.h>
#include <bcm/module.h>
#include <bcm_int/sbx/caladan3/bcm_sw_db.h>

#ifdef BCM_WARM_BOOT_SUPPORT

char *g_bcm_sbx_caladan3_module_names[] = BCM_CALADAN3_MODULE_NAMES_INITIALIZER;

char *
bcm_caladan3_module_name(int unit, int module_num)
{
    if (module_num < BCM_MODULE__COUNT)
    {
        return bcm_module_name(unit, module_num);
    }

    if (sizeof(g_bcm_sbx_caladan3_module_names) / sizeof(g_bcm_sbx_caladan3_module_names[0])
                                                    != (BCM_CALADAN3_MODULE__COUNT - BCM_MODULE__COUNT)) {
        int i;

        i = sizeof(g_bcm_sbx_caladan3_module_names) / sizeof(g_bcm_sbx_caladan3_module_names[0]) - 1;

        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "bcm_module_name: BCM_MODULE_NAMES_INITIALIZER(%d) and BCM_MODULE__COUNT(%d) mis-match\n"), i, BCM_MODULE__COUNT));
        for(;i >= 0;i--) {
            LOG_CLI((BSL_META_U(unit,
                                "%2d. module_name %s module_num %d\n"), i, g_bcm_sbx_caladan3_module_names[i], (i+BCM_MODULE__COUNT)));
        }
    }

    if (module_num >= BCM_MODULE__COUNT && module_num < BCM_CALADAN3_MODULE__COUNT) 
    {
        return g_bcm_sbx_caladan3_module_names[(module_num - BCM_MODULE__COUNT)];
    }
    else
    {
        return "UNKNOWN";
    }

}

int
bcm_caladan3_scache_ptr_get(int unit, soc_scache_handle_t handle, soc_caladan3_scache_oper_t oper,
                                 int flags, uint32 *size, uint8 **scache_ptr,
                                 uint16 version, uint16 *recovered_ver, int *already_exists)
{
    LOG_CLI((BSL_META_U(unit,
                        "MODULE %s  MODULE ID %d: "), 
             bcm_caladan3_module_name(unit, SOC_SCACHE_HANDLE_MODULE_GET(handle)), SOC_SCACHE_HANDLE_MODULE_GET(handle)));

    return soc_caladan3_scache_ptr_get(unit, handle, oper,
                                 flags, size, scache_ptr,
                                 version, recovered_ver, already_exists);

}

#endif /*BCM_WARM_BOOT_SUPPORT*/

#undef _ERR_MSG_MODULE_NAME

/*need to add flag mechanism to sw db*/

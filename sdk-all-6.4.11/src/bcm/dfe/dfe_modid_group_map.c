/*
 * $Id: dfe_modid_group_map.c,v 1.4 Broadcom SDK $
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
 * SOC DFE MODID GROUP MAP
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FABRIC
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/dfe/dfe_modid_group_map.h>

#include <shared/bitop.h>

#include <sal/core/libc.h>

#include <soc/dfe/cmn/dfe_warm_boot.h>

/*
 * Clear modid group id map
 */
int bcm_dfe_modid_group_map_clear(int unit)
{
    int i;
    _bcm_dfe_modid_group_map_t group_map;
    bcm_module_t modid_to_group_id;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    
    sal_memset(group_map.raw,0, SOC_DFE_MODID_NOF_UINT32_SIZE * sizeof(uint32));
    for(i=0 ; i< DFE_MODID_GROUP_MAP_ROWS(unit) ; i++)
    {
        rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_GROUP_MAP, i, &group_map);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    modid_to_group_id = DFE_MODID_GROUP_INVALID;
    for(i=0 ; i< SOC_DFE_MODID_NOF ; i++)
    {
        rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_MODID_TO_GROUP_MAP, i, &modid_to_group_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Clear modid single group 
 */
int bcm_dfe_modid_group_map_clear_group(int unit, bcm_module_t group)
{
    int i,
        rv;
    _bcm_dfe_modid_group_map_t group_map;
    bcm_module_t modid_to_group_id;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(group_map.raw,0,SOC_DFE_MODID_NOF_UINT32_SIZE * sizeof(uint32));
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_GROUP_MAP, group, &group_map);
    BCMDNX_IF_ERR_EXIT(rv);

    for(i=0 ; i < SOC_DFE_MODID_NOF ; i++)
    {
        rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_MODID_TO_GROUP_MAP, i, &modid_to_group_id);
        BCMDNX_IF_ERR_EXIT(rv);
        if(group == modid_to_group_id) {
             modid_to_group_id = DFE_MODID_GROUP_INVALID;
             rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_MODID_TO_GROUP_MAP, i, &modid_to_group_id);
             BCMDNX_IF_ERR_EXIT(rv);
        } 
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Add module to a group 
 */
int bcm_dfe_modid_group_map_add(int unit, bcm_module_t group, bcm_module_t module)
{  
    _bcm_dfe_modid_group_map_t group_map;
    bcm_module_t modid_to_group_id;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_GROUP_MAP, group, &group_map);
    BCMDNX_IF_ERR_EXIT(rv);
    SHR_BITSET(group_map.raw, module);
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_GROUP_MAP, group, &group_map);
    BCMDNX_IF_ERR_EXIT(rv);

    modid_to_group_id = group;
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_MODID_TO_GROUP_MAP, module, &modid_to_group_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get list of modules for a group
 */
int bcm_dfe_modid_group_map_get_list(int unit, bcm_module_t group, int modid_max_count, bcm_module_t *moidid_array, int *modid_count)
{ 
    int i;
    _bcm_dfe_modid_group_map_t group_map;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(moidid_array);
    BCMDNX_NULL_CHECK(modid_count);
     
    *modid_count = 0;
    
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_GROUP_MAP, group, &group_map);
    BCMDNX_IF_ERR_EXIT(rv);

    for(i = 0 ; i < SOC_DFE_MODID_NOF ; i++)
    {
        if(SHR_BITGET(group_map.raw, i))
        {
            if(*modid_count >= modid_max_count) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Array is too small")));  
            }
              
            moidid_array[*modid_count] = i;
            (*modid_count)++;
        }
    }
    
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Get group id of a module
 */
int bcm_dfe_modid_group_map_get_group(int unit, bcm_module_t module, bcm_module_t* group)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(group);

    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_MODID_TO_GROUP_MAP, module, group);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN; 
}

#undef _ERR_MSG_MODULE_NAME


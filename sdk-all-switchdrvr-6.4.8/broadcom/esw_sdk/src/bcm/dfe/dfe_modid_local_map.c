/*
 * $Id: dfe_modid_local_map.c,v 1.3 Broadcom SDK $
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
 * SOC DFE MODID LOCAL MAP
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FABRIC
 #include <shared/bsl.h>
#include <bcm_int/dfe/dfe_modid_local_map.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

#include <shared/bitop.h>

#include <soc/drv.h>

#include <soc/dfe/cmn/dfe_warm_boot.h>

/*
 * Clear modid local map
 */
int bcm_dfe_modid_local_map_clear(int unit)
{
    int i, rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    for(i=0 ; i < DFE_MODID_LOCAL_MAP_ROWS(unit) ; i++)
    {
        rc = bcm_dfe_modid_local_map_module_clear(unit, i);
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Remove local module from the map
 */
int bcm_dfe_modid_local_map_module_clear(int unit, bcm_module_t local_module_id)
{
    int rv;
    _bcm_dfe_modid_local_map_t local_modid_map;
    BCMDNX_INIT_FUNC_DEFS;
    
    local_modid_map.valid = 0;
    local_modid_map.module_id = 0;
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_LOCAL_MAP, local_module_id, &local_modid_map);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Check whether a local module is mapped
 */
int bcm_dfe_modid_local_map_is_valid(int unit, bcm_module_t local_module_id, int* is_valid)
{  
    int rv;
    _bcm_dfe_modid_local_map_t local_modid_map;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_valid);

    if(!DFE_MODID_LOCAL_MAP_ROW_VALIDATE(unit, local_module_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local_module_id is out of range")));  
    }
    
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_LOCAL_MAP, local_module_id, &local_modid_map);
    BCMDNX_IF_ERR_EXIT(rv);

    *is_valid = local_modid_map.valid;

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Attach module id with local module id
 */
int bcm_dfe_modid_local_map_add(int unit, bcm_module_t local_module_id, bcm_module_t module_id, int allow_override)
{ 
    int rv;
    _bcm_dfe_modid_local_map_t local_modid_map;
    BCMDNX_INIT_FUNC_DEFS;
    
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_LOCAL_MAP, local_module_id, &local_modid_map);
    BCMDNX_IF_ERR_EXIT(rv);


    if(!DFE_MODID_LOCAL_MAP_ROW_VALIDATE(unit, local_module_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local_module_id is out of range")));
    }
        
    if(0 == allow_override && local_modid_map.valid)  {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local_module_id alerady exist")));
    }
      
    local_modid_map.valid = 1;
    local_modid_map.module_id = module_id;

    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_LOCAL_MAP, local_module_id, &local_modid_map);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Get module id for local module id
 */
int bcm_dfe_modid_local_map_get(int unit, bcm_module_t local_module_id, bcm_module_t* module_id)
{  
    int rv;
    _bcm_dfe_modid_local_map_t local_modid_map;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(module_id);

    if(!DFE_MODID_LOCAL_MAP_ROW_VALIDATE(unit, local_module_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local_module_id is out of range")));
    }
    
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_LOCAL_MAP, local_module_id, &local_modid_map);
    BCMDNX_IF_ERR_EXIT(rv);

    if(!local_modid_map.valid) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local_module_id doesn't exist")));
    }
      
    *module_id = local_modid_map.module_id;
    
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Get local module id for a module id
 */
int bcm_dfe_modid_local_map_modid_to_local(int unit, bcm_module_t module_id, bcm_module_t* local_module_id)
{  
    int i,
        rv;
    _bcm_dfe_modid_local_map_t local_modid_map;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(local_module_id);
    
    for(i = 0; i<DFE_MODID_LOCAL_MAP_ROWS(unit) ; i++)
    {

        rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_LOCAL_MAP, i, &local_modid_map);
        BCMDNX_IF_ERR_EXIT(rv);

        if(local_modid_map.valid == 0)
          continue;

        if(local_modid_map.module_id == module_id)
        {
          *local_module_id = i;
          BCM_EXIT;
        }
    }
    
    /*module not found*/
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("module_id wasn't found")));

exit:
    BCMDNX_FUNC_RETURN; 
}

#undef _ERR_MSG_MODULE_NAME


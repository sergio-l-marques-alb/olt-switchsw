/*
 * $Id: dfe_multicast_id_map.c,v 1.4 Broadcom SDK $
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
 * SOC DFE MULTICAST ID MAP
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MCAST

#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/dfe/dfe_multicast_id_map.h>

#include <shared/bitop.h>

#include <sal/core/alloc.h>

#include <soc/dfe/cmn/dfe_warm_boot.h>
#include <soc/dfe/cmn/mbcm.h>

/*
 * Clear multicast id map
 */
STATIC int bcm_dfe_multicst_id_map_clear(int unit)
{
    bcm_multicast_t i;
    int rc;
    uint32 max_mc_id;
    BCMDNX_INIT_FUNC_DEFS;
    
    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_size_get, (unit, &max_mc_id));
    BCMDNX_IF_ERR_EXIT(rc);

    for(i=0 ; i < max_mc_id ; i++)
    {
        rc = bcm_dfe_multicst_id_map_clear_id(unit, i);
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Initialize multicast id map
 */
int bcm_dfe_multicst_id_map_init(int unit, soc_dfe_multicast_table_mode_t mc_mode)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Clear map*/
    rc = SOC_DFE_WARM_BOOT_VAR_SET(unit, MC_MODE, &mc_mode);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Clear map*/
    rc = bcm_dfe_multicst_id_map_clear(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Clear multicast id
 */
int bcm_dfe_multicst_id_map_clear_id(int unit, bcm_multicast_t id)
{
    int entry_id,
        inner_bit,
        rv;
    uint32 mc_map_entry[1];
    uint32 max_ids;
    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_size_get, (unit, &max_ids));
    BCMDNX_IF_ERR_EXIT(rv);
    
    if(id < max_ids) {
        /*Each warm boot entry is uint32*/
        entry_id = id / 32;
        inner_bit = id % 32;

        rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MC_ID_MAP, entry_id, mc_map_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        SHR_BITCLR(mc_map_entry, inner_bit);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Multicast id is out of range"))); 
    }

    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MC_ID_MAP, entry_id, mc_map_entry);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Select multicast id
 */
int bcm_dfe_multicst_id_map_mark_id(int unit, bcm_multicast_t id)
{
    int entry_id,
        inner_bit,
        rv;
    uint32 mc_map_entry[1];
    uint32 max_ids;
    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_size_get, (unit, &max_ids));
    BCMDNX_IF_ERR_EXIT(rv);

    if(id < max_ids) {
        /*Each warm boot entry is uint32*/
        entry_id = id / 32;
        inner_bit = id % 32;

        rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MC_ID_MAP, entry_id, mc_map_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        SHR_BITSET(mc_map_entry, inner_bit);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Milticast id is out of range"))); 
    }

    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MC_ID_MAP, entry_id, mc_map_entry);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Check whether mc id is legal
 */
int bcm_dfe_multicst_id_map_is_legal_id(int unit, bcm_multicast_t id, int* is_legal)
{
    uint32 max_ids;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(is_legal);

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_size_get, (unit, &max_ids));
    BCMDNX_IF_ERR_EXIT(rv);

    *is_legal = (id < max_ids);

exit:
    BCMDNX_FUNC_RETURN;  
}

/*
 * Check whether mc id is free
 */
int bcm_dfe_multicst_id_map_is_free_id(int unit, bcm_multicast_t id, int* is_free)
{
    uint32 max_ids;
    int entry_id,
        inner_bit,
        rv;
    uint32 mc_map_entry[1];
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_free);

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_size_get, (unit, &max_ids));
    BCMDNX_IF_ERR_EXIT(rv);

    if(id < max_ids) {
        /*Each warm boot entry is uint32*/
        entry_id = id / 32;
        inner_bit = id % 32;

        rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MC_ID_MAP, entry_id, mc_map_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        *is_free = (SHR_BITGET(mc_map_entry, inner_bit) ? 0 :1);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Milticast id is out of range"))); 
    }

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Get free mc id
 */
int bcm_dfe_multicst_id_map_get_free_id(int unit, bcm_multicast_t* id)
{
    bcm_multicast_t i;
    int is_free, rc;
    uint32 max_ids;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(id);
    
    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_size_get, (unit, &max_ids));
    BCMDNX_IF_ERR_EXIT(rc);

    for(i=0 ; i< max_ids; i++)
    {
        rc = bcm_dfe_multicst_id_map_is_free_id(unit, i, &is_free);
        BCMDNX_IF_ERR_EXIT(rc);
        if(is_free)
        {
            *id = i;
            BCM_EXIT;
        }
    }

    /*id not found*/
     BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Milticast free id not found"))); 
    
exit:
    BCMDNX_FUNC_RETURN; 
}

#undef _ERR_MSG_MODULE_NAME


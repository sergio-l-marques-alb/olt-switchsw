/*
 * $Id: alloc_mngr.c,v 1.312 Broadcom SDK $
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
 * File:        alloc_mngr.c
 * Purpose:     Resource allocation manager for SOC_SAND chips.
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>

/*#include <soc/error.h>*/
#include <bcm_int/common/debug.h>
#include <shared/shr_resmgr.h>
#include <shared/shr_template.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/dpp/wb_db_alloc.h>
#endif

#include <soc/dpp/dpp_wb_engine.h>

/* If set, copies the warmboot array before performing binary search to find resource id by pool id. */
#define DPP_AM_UTILS_COPY_WB_ARRAY 1

/* Debug test defines.
 *
 * If ALLOC_MNGR_DEBUG_TEST is set, resource #DEBUG_TEST_TESTED_RES will be have DEBUG_TEST_NOF_RES pools
 * and template #DEBUG_TEST_TESTED_RES will have DEBUG_TEST_NOF_TEMPLATE pools.
 */ 
#define ALLOC_MNGR_DEBUG_TEST   0

#define DEBUG_TEST_NOF_RES 1

#define DEBUG_TEST_NOF_TEMPLATE 2

#define DEBUG_TEST_TESTED_RES  000

/*
 * Debug test defines - end. 
 */

/* Since lif resources and cosq resources use the wrapper functions,
   but don't use alloc_warmboot (cosq never uses alloc_warmboot and lif
   doesn't when a special compilation flag is used), the regular warmboot
   check should be expanded. */
#define DPP_AM_RES_USE_WARMBOOT(_unit, _res_id)                                             \
    (!SOC_WARM_BOOT(_unit) /* Don't use warmboot during init */                             \
    && !DPP_AM_RES_ID_IS_COSQ(_unit, _res_id) /* Cosq WB resources are saved elsewhere */   \
    && !DPP_AM_RES_LIF_SKIP_WARMBOOT(_unit, _res_id)) /* LIF memory optimization mode. */


STATIC int
_bcm_dpp_resources_destroy(int unit);

STATIC int 
dpp_am_pp_resource_setup_in_range_by_core_id(int unit, int first_res, int last_res, int *pool_id, bcm_dpp_am_pool_info_t *p_info);

STATIC int
_bcm_dpp_resources_create(int unit, int nof_resource_pools, int nof_cosq_pools, int nof_lif_pools);

STATIC int
_bcm_dpp_templates_create(int unit, int nof_template_pools);

STATIC int
_bcm_dpp_config_am_get(int unit, int *nof_resource_pools, int *nof_cosq_pools, int *nof_lif_pools, int *nof_template_pools);

STATIC int 
bcm_dpp_am_calculate_nof_resource_pools(int unit, int *nof_resource_pools, int *nof_cosq_pools, int *nof_lif_pools);

STATIC int 
bcm_dpp_am_calculate_nof_template_pools(int unit, int *nof_template_pools);


bcm_dpp_am_pool_info_t *_bcm_dpp_pool_info[BCM_MAX_NUM_UNITS] = {NULL};
bcm_dpp_am_template_info_t *_bcm_dpp_template_info[BCM_MAX_NUM_UNITS] = {NULL};

/*************************************/
/*************************************/
/**** ALLOC MANAGER INIT - START  ****/  
/*************************************/
/*************************************/

/*
 * Function:
 *     bcm_dpp_am_attach
 * Purpose:
 *     INIT alloc manager state
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_dpp_am_attach(int unit)
{
    int rv = BCM_E_NONE;
    int nof_resource_pools  = 0, 
        nof_cosq_pools      = 0, 
        nof_lif_pools       = 0, 
        nof_template_pools  = 0;
#ifdef BCM_WARM_BOOT_SUPPORT
    bcm_dpp_am_pool_info_t *pool_info;
    bcm_dpp_am_template_info_t *template_info;
    int num_pools;
#endif
    BCMDNX_INIT_FUNC_DEFS;

    /* Initialise the resource/template mapping buffer. */
    rv = _bcm_dpp_config_am_get(unit, &nof_resource_pools, &nof_cosq_pools, &nof_lif_pools, &nof_template_pools);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR_MULTI_CORE);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_resources_create(unit, nof_resource_pools, nof_cosq_pools, nof_lif_pools);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_templates_create(unit, nof_template_pools);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    num_pools = nof_resource_pools;

#ifdef BCM_ARAD_SUPPORT
#if (!BCM_ARAD_OPTIMIZE_MEMORY_ALLOCATION)
    if (SOC_IS_ARAD(unit)) {
        num_pools += nof_lif_pools;
    }
#endif
#endif

    pool_info =     _bcm_dpp_pool_info[unit];
    template_info = _bcm_dpp_template_info[unit];

    rv = _bcm_dpp_wb_alloc_state_init(unit, pool_info, num_pools, template_info, nof_template_pools);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("failed warmboot initialization of alloc_mngr rv = %d"), rv));
    }

#endif

#ifdef BCM_ARAD_SUPPORT
    /* Ingress LIF & Egress encapsulation initalization */
    if (SOC_IS_ARAD(unit) && SOC_DPP_PP_ENABLE(unit)) {
        rv = _bcm_dpp_am_sync_lif_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_am_egress_encap_sw_state_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_am_ingress_lif_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
     }
#endif /* BCM_ARAD_SUPPORT */
   
    /* warmboot recovery for am buffer containing ingress_lif and egress_encap info*/
    rv = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_ARAD_SUPPORT
    /* Ingress LIF & Egress encapsulation initalization */
    if (SOC_IS_ARAD(unit) && SOC_DPP_PP_ENABLE(unit)) {
        rv = _bcm_dpp_am_egress_encap_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /* BCM_ARAD_SUPPORT */

    if (SOC_IS_JERICHO(unit)) {
        rv = _bcm_dpp_am_global_lif_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Setup warmboot variables for FEC allocations (bank groups). */
        BCMDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_FEC_BANK_GROUPS));
    }

exit:
    BCM_FREE(_bcm_dpp_pool_info[unit]);
    BCM_FREE(_bcm_dpp_template_info[unit]);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_dpp_am_clear
 * Purpose:
 *     Restart alloc manager state.
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_dpp_am_clear(int unit)
{
        int rv = BCM_E_NONE;

        BCMDNX_INIT_FUNC_DEFS;

        /* first destroy, then re-create */
        rv = _bcm_dpp_resources_destroy(unit);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = shr_template_detach(unit);
        BCMDNX_IF_ERR_EXIT(rv);


        rv = bcm_dpp_am_attach(unit);
        BCMDNX_IF_ERR_EXIT(rv);

exit:
        BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_resources_destroy
 *   Purpose
 *      Create resource management information for the unit
 *   Parameters
 *      (IN) unit = unit number of the device
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 *   Notes
 *      This function needs to be called very early during bcm_petra_init().
 */
STATIC int
_bcm_dpp_resources_destroy(int unit)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, Starting resources destroy\n"), unit));
    BCM_FREE(_bcm_dpp_pool_info[unit]);
    BCM_FREE(_bcm_dpp_template_info[unit]);

#ifdef BCM_ARAD_SUPPORT
    /* Destroy Ingress LIF & Egress encapsulation */
    if (SOC_IS_ARAD(unit) && SOC_DPP_PP_ENABLE(unit)) {
      rv = _bcm_dpp_am_ingress_lif_deinit(unit);
      BCMDNX_IF_ERR_EXIT(rv);

      rv = _bcm_dpp_am_egress_encap_deinit(unit);
      BCMDNX_IF_ERR_EXIT(rv);

      rv = _bcm_dpp_am_sync_lif_deinit(unit);
      BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = shr_template_detach(unit);
    BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_ARAD_SUPPORT */

    rv = shr_res_detach(unit);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_dpp_am_attach
 * Purpose:
 *     DEINIT alloc manager state
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_dpp_am_detach(
    int unit)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_resources_destroy(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_detach(unit);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      dpp_am_pp_resource_setup_in_range_by_core_id
 *   Purpose
 *      Given a unit, a range of resources (Not including cosq), an initialized pool id pointer and a pool info array pointer,
 *      inits all the resources in range with their appropriate number of pools, saves the base pool number for the resource
 *      and saves the relevant data in the pool array.
 *   Parameters
 *      (IN) unit           unit number of the device
 *      (IN) first_res      Start of the range to allocate.
 *      (IN) last_res       End of the range to allocate.
 *      (INOUT) pool_id     Should contain the base pool for the first resource. Will be returned with the first unused pool.
 *      (OUT) p_info        For every allocated pool, will be filled with the pool's information.
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
STATIC int 
dpp_am_pp_resource_setup_in_range_by_core_id(int unit, int first_res, int last_res, int *pool_id, bcm_dpp_am_pool_info_t *p_info){
    int res_id, core_id, rv, adj_pool_id;
    uint8 nof_pools_for_res;
    BCMDNX_INIT_FUNC_DEFS;

    for (res_id = first_res; res_id < last_res ; res_id++)   {

        /* First, save the index of this resource's first pool. */
        rv = bcm_dpp_am_resource_base_pool_id_set(unit, res_id, *pool_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Get how many pools to allocate for the resource. */
        rv = bcm_dpp_am_resource_to_nof_pools(unit, res_id, &nof_pools_for_res);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Allocate the pools. */
        for (core_id = 0 ; core_id < nof_pools_for_res ; core_id++) {
            adj_pool_id = *pool_id + core_id;
            rv = _bcm_dpp_pp_resource_setup(unit, res_id, core_id, adj_pool_id, &p_info[adj_pool_id]);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* The next resource will start from this index. */
        *pool_id += nof_pools_for_res;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *   Function
 *      _bcm_dpp_resources_create
 *   Purpose
 *      Initializes the resource manager, and the regular and lif resources for the unit.
 *   Parameters
 *      (IN) unit                   unit number of the device
 *      (IN) nof_resource_pools     Number of resource pools to allocate.
 *      (IN) nof_cosq_pools         Number of cosq resource pools to allocate.
 *      (IN) nof_lif_pools          Number of lif resource pools to allocate.
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
STATIC int
_bcm_dpp_resources_create(int unit, int nof_resource_pools, int nof_cosq_pools, int nof_lif_pools)
{
    int rv;
    int pool_id, num_pools;
#if (BCM_ARAD_OPTIMIZE_MEMORY_ALLOCATION)
    int res_id;
#endif
    bcm_dpp_am_pool_info_t *p_info;
    BCMDNX_INIT_FUNC_DEFS;

    /* The number of pools has already been calculated on device. */
    num_pools = nof_resource_pools + nof_cosq_pools + nof_lif_pools;

    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, Starting resources create\n"), unit));

    rv = shr_res_init(unit,
                      num_pools, /* number of types is the same as the number of pools */
                      num_pools /* number of resource pools */);

    num_pools = nof_resource_pools;

#ifdef BCM_ARAD_SUPPORT
#if (!BCM_ARAD_OPTIMIZE_MEMORY_ALLOCATION)
    /* If we're in optimize memory allocation mode mode, or petrab, we don't allocate the lif pools. */
    if (SOC_IS_ARAD(unit)) {
        num_pools += nof_lif_pools;
    }
#endif
#endif

    BCMDNX_ALLOC(_bcm_dpp_pool_info[unit], (sizeof(bcm_dpp_am_pool_info_t) * num_pools), "am pool info");
    if (_bcm_dpp_pool_info[unit] == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    
    p_info = _bcm_dpp_pool_info[unit];

    pool_id = 0; /* Start from the first pool. */
    /* Allocate the regular resources */
    rv = dpp_am_pp_resource_setup_in_range_by_core_id(unit, 0, dpp_am_res_count, &pool_id, p_info);
    if (rv != BCM_E_NONE) {
        goto err;
    }

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
#if (!BCM_ARAD_OPTIMIZE_MEMORY_ALLOCATION)

        /* Allocate the lif resources. */
        rv = dpp_am_pp_resource_setup_in_range_by_core_id(unit, dpp_res_arad_pool_egress_encap_bank_start, dpp_res_arad_pool_ingress_lif_bank_end, &pool_id, p_info);
        if (rv != BCM_E_NONE) {
            goto err;
        }
#else
        /* Even if optimizing memory allocation, the resource pool mapping should still be kept. */
        for (res_id = dpp_res_arad_pool_egress_encap_bank_start ; res_id < dpp_res_arad_pool_ingress_lif_bank_end ; res_id++) {
            rv = bcm_dpp_am_resource_base_pool_id_set(unit, res_id, pool_id);
            BCMDNX_IF_ERR_EXIT(rv);
            pool_id++;
        }
#endif  /* (!BCM_ARAD_OPTIMIZE_MEMORY_ALLOCATION) */
    }
#endif /*BCM_ARAD_SUPPORT*/

    /* Allocate global lif pools. Don't allocate egress pools in the illegal range (rif banks) */
    if (SOC_IS_JERICHO(unit)) {

        rv = dpp_am_pp_resource_setup_in_range_by_core_id(unit, _BCM_DPP_AM_GLOBAL_LIF_EGRESS_RES_ID_START(unit), 
                                                          _BCM_DPP_AM_GLOBAL_LIF_EGRESS_RES_ID_END(unit), &pool_id, p_info);
        if (rv != BCM_E_NONE) {
            goto err;
        }
        rv = dpp_am_pp_resource_setup_in_range_by_core_id(unit, _BCM_DPP_AM_GLOBAL_LIF_INGRESS_RES_ID_START(unit), 
                                                          _BCM_DPP_AM_GLOBAL_LIF_INGRESS_RES_ID_END(unit), &pool_id, p_info);
        if (rv != BCM_E_NONE) {
            goto err;
        }
    }

    BCM_EXIT;   /* Skip the error label. */

err:
    BCM_FREE(_bcm_dpp_pool_info[unit]);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_templates_create
 *   Purpose
 *      Create template management information for the unit
 *   Parameters
 *      (IN) unit = unit number of the device
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 *   Notes
 *      This function needs to be called very early during bcm_petra_init().
 */
STATIC int
_bcm_dpp_templates_create(int unit, int nof_template_pools)
{
    int template_id, pool_id, core_id;
    uint8 nof_pools_for_template;
    int rv;
    bcm_dpp_am_template_info_t *t_info;

    BCMDNX_INIT_FUNC_DEFS;
    /* account for all the template types/template pools */
    /* pool resources for all modules are allocated in this function */
    
    rv = shr_template_init(unit,
                      nof_template_pools /* number of template types (depecrated, equals nof_pools) */,
                      nof_template_pools /* number of template pools */);

   BCMDNX_ALLOC(_bcm_dpp_template_info[unit], (sizeof(bcm_dpp_am_template_info_t) * nof_template_pools), "am template info");
    if (_bcm_dpp_template_info[unit] == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate template memory")));
    }    

    t_info = _bcm_dpp_template_info[unit];
    pool_id = 0;
    for (template_id = 0 ; template_id < SOC_DPP_CONFIG(unit)->am.nof_am_template_ids ; template_id++)   {

        /* First, save the index of this template's first pool. */
        rv = bcm_dpp_am_template_base_pool_id_set(unit, template_id, pool_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Get how many pools to allocate for the resource. */
        rv = bcm_dpp_am_template_to_nof_pools(unit, template_id, &nof_pools_for_template);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Allocate the pools. */
        for (core_id = 0 ; core_id < nof_pools_for_template ; core_id++) {
            rv = _bcm_dpp_template_setup(unit, template_id, core_id, pool_id, &t_info[pool_id]);
            if (rv != BCM_E_NONE) {
                goto err;
            }
            pool_id++;
        }
    }



    BCM_EXIT;

err:
    BCM_FREE(_bcm_dpp_template_info[unit]);
exit:
    BCMDNX_FUNC_RETURN;
}

/*******************************************/
/** Number of pools calculation functions.**/
/*******************************************/
/*
 *   Function
 *      bcm_dpp_am_calculate_nof_resource_pools
 *      bcm_dpp_am_calculate_nof_template_pools
 *      _bcm_dpp_config_am_get
 *  
 *  
 *   Purpose
 *      _bcm_dpp_config_am_get is the wrapper function for the other two functions.
 *      Those functions fill the number of pools per resource 
 *   Parameters
 *      (IN) unit               - unit number of the device
 *  
 * In bcm_dpp_am_calculate_nof_resource_pools:
 *      (OUT)nof_resource_pools - Will be filled with the number of regular resource pools to allocate. 
 *      (OUT)nof_cosq_pools     - Will be filled with the number of lif resource pools to allocate. 
 *      (OUT)nof_lif_pools      - Will be filled with the number of cosq resource pools to allocate.
 *  
 * In bcm_dpp_am_calculate_nof_template_pools:
 *      (OUT)nof_template_pools - Will be filled with the number of template pools to allocate. 
 *  
 *  
 *      SOC_DPP_CONFIG(unit)->am
 * In bcm_dpp_am_calculate_nof_resource_pools:
 *                                  ->nof_am_resource_ids   - Will be filled with the number of regular resource ids.
 *                                  ->nof_am_cosq_ids       - Will be filled with the number of cosq resource ids.
 *                                  ->nof_am_lif_ids        - Will be filled with the number of lif resource ids.
 * In bcm_dpp_am_calculate_nof_template_pools: 
 *                                  ->nof_am_template_ids   - Will be filled with the number of template ids.
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */


/* See function description above. */
STATIC int
_bcm_dpp_config_am_get(int unit, int *nof_resource_pools, int *nof_cosq_pools, int *nof_lif_pools, int *nof_template_pools){
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_calculate_nof_resource_pools(unit, nof_resource_pools, nof_cosq_pools, nof_lif_pools);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = bcm_dpp_am_calculate_nof_template_pools(unit, nof_template_pools);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;    
}


/* See function description above. */
STATIC int 
bcm_dpp_am_calculate_nof_resource_pools(int unit, int *nof_resource_pools, int *nof_cosq_pools, int *nof_lif_pools){
    int rv;
    int res_id;
    uint8 nof_pools;
    _dpp_res_pool_cosq_t *dpp_res_cosq_pool = NULL;
    _dpp_res_type_cosq_t *dpp_res_cosq_type = NULL;
    soc_dpp_config_am_t *am_config;
    int nof_cosq_res;
    int cosq_res_start;

    BCMDNX_INIT_FUNC_DEFS;

    am_config = &SOC_DPP_CONFIG(unit)->am;

    /*
     * GENERAL RESOURCES
     * All resources that are not lif or cosq (in the _dpp_am_res_t enum).
     * 
     */

    /* Calcualte the number of pools for resources that are not lif or cosq. */
    am_config->nof_am_resource_ids = dpp_am_res_count;

    /* Reset the global number of resources. */
    *nof_resource_pools = 0;

    /* For each resource type, get the number of pools, and add it to the total.*/
    for (res_id = 0 ; res_id < am_config->nof_am_resource_ids ; res_id++) {
        rv = bcm_dpp_am_resource_to_nof_pools(unit, res_id, &nof_pools);
        BCMDNX_IF_ERR_EXIT(rv);
        *nof_resource_pools += nof_pools;
    }


    /* Calculate cosq pools and types. */
    BCMDNX_ALLOC(dpp_res_cosq_pool, (sizeof(_dpp_res_pool_cosq_t)), "res cos type");
    if (dpp_res_cosq_pool == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    BCMDNX_ALLOC(dpp_res_cosq_type, (sizeof(_dpp_res_type_cosq_t)), "res cos type");
    if (dpp_res_cosq_type == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    /*
     * 
     * COSQ RESOURCES 
     * All resources mapped by the cosq allocation functions. 
     *  
     */
    
    /* Reset the global number of resources to indicate that we want to read the number of resources.
       (See documentation in _bcm_dpp_am_cosq_get_first_base_pool_id for details). */
    am_config->nof_am_cosq_resource_ids = 0;
    
    /* Fill cosq resources */
    rv = _bcm_dpp_resources_fill_pool_cosq(unit, SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores - 1, dpp_res_cosq_pool);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_resources_fill_type_cosq(unit, SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores - 1, dpp_res_cosq_type);
    BCMDNX_IF_ERR_EXIT(rv);


    /* Calculate cosq number of pools and types. Cosq allocation doesn't use type_id == pool_id,
       so there are more pool pointers than resources allocated.
       Type id is used as res_id in this case. */
    rv = _bcm_dpp_am_cosq_get_first_base_pool_id(unit, &cosq_res_start);
    BCMDNX_IF_ERR_EXIT(rv);

    am_config->nof_am_cosq_resource_ids = 
        dpp_res_cosq_type->dpp_res_type_cosq_queue_dynamic_end - cosq_res_start;

    nof_cosq_res = am_config->nof_am_cosq_resource_ids;

    *nof_cosq_pools = 0;
    /* For each resource type, get the number of pools, and add it to the total.*/
    for (res_id = DPP_AM_RES_COSQ_START(unit) ; res_id < DPP_AM_RES_COSQ_START(unit) + nof_cosq_res ; res_id++) {
        rv = bcm_dpp_am_resource_to_nof_pools(unit, res_id, &nof_pools);
        BCMDNX_IF_ERR_EXIT(rv);
        *nof_cosq_pools += nof_pools;
    }

    /*
     * 
     * LIF RESOURCES 
     * All resources mapped by the lif allocation functions. 
     *  
     */

    /* Calculate lif  number of resources. */
    am_config->nof_am_lif_resource_ids = dpp_res_arad_type_ingress_lif_bank_end - dpp_res_arad_type_egress_encap_bank_start; 

    if (SOC_IS_JERICHO(unit)) {
        /* If soc is jericho, add global lif resources to the lif pool. */
        am_config->nof_am_lif_resource_ids +=  _BCM_DPP_AM_GLOBAL_LIF_INGRESS_RES_ID_END(unit) - _BCM_DPP_AM_GLOBAL_LIF_EGRESS_RES_ID_START(unit);
    }

    /* Lif does not use different mapping for different cores. It's simply 1 core each. */
    *nof_lif_pools = am_config->nof_am_lif_resource_ids;


exit:
    BCM_FREE(dpp_res_cosq_type);
    BCM_FREE(dpp_res_cosq_pool);
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
STATIC int 
bcm_dpp_am_calculate_nof_template_pools(int unit, int *nof_template_pools){
    int rv, template_id;
    soc_dpp_config_am_t *am_config;
    uint8 nof_pools;
    BCMDNX_INIT_FUNC_DEFS;

    /* Update global am template information. */
    am_config = &SOC_DPP_CONFIG(unit)->am;
    am_config->nof_am_template_ids = dpp_am_template_count;
    
    /* Iterate over the pools and sum the number of pools each template use. */
    *nof_template_pools = 0;
    for (template_id = 0 ; template_id < am_config->nof_am_template_ids ; template_id++) {
        rv = bcm_dpp_am_template_to_nof_pools(unit, template_id, &nof_pools);
        BCMDNX_IF_ERR_EXIT(rv);
        *nof_template_pools += nof_pools;
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*************************************/
/*************************************/
/***** ALLOC MANAGER INIT - END  *****/  
/*************************************/
/*************************************/


/********************************************/
/********************************************/
/**** GENERAL CORE HANDLING UTILS - START ***/  
/********************************************/
/********************************************/

/**
 * Most of the functions in the general core handling utils 
 * block have one function for templates and resources and both 
 * call an inner generic function.  
 *  
 */

/*************************************************/
/** Resource / template to nof_pools functions. **/
/*************************************************/

/*
 *   Function
 *      bcm_dpp_am_resource_to_nof_pools
 *      bcm_dpp_am_template_to_nof_pools
 *   Purpose
 *      Given a resource id or template id, returns the number of pools that should be allocated fot this resource or template.
 *      The default is 1. For all chips below ARADPLUS, this is always 1. 
 *  
 *   Parameters
 *      (IN) unit                   - unit number of the device
 *      (IN) res_id | template_id   - Resource id or template id to be checked.    
 *      (OUT)nof_pools_per_res      - Will be filled with the number of pools for this resource.
 *  
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
int 
bcm_dpp_am_resource_to_nof_pools(int unit, int res_id, uint8 *nof_pools_per_res){
    BCMDNX_INIT_FUNC_DEFS;

    /* Only one core in aradplus and below. */
    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        *nof_pools_per_res = BCM_DPP_AM_DEFAULT_NOF_POOLS;
        BCM_EXIT;
    }
    
    /* Enter your resource id as a case here, and handle it as needed. */
    switch (res_id) {

#if ALLOC_MNGR_DEBUG_TEST
        /* Do not insert code here, this is for debug purposes only. */
        /* Example: */
        case DEBUG_TEST_TESTED_RES:
            *nof_pools_per_res = DEBUG_TEST_NOF_RES;
            break;
#endif
        case dpp_am_res_fec_global:
            *nof_pools_per_res = SOC_DPP_DEFS_GET(unit, nof_fec_banks);
            break;
        default:
            *nof_pools_per_res = BCM_DPP_AM_DEFAULT_NOF_POOLS;
    }

    /*
     *  If your resource is within a certain range (cosq or lif), then add an if block here for the range.
     *
     */
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int 
bcm_dpp_am_template_to_nof_pools(int unit, int template_id, uint8 *nof_pools){
    BCMDNX_INIT_FUNC_DEFS;

    /* Only one core in aradplus and below. */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        *nof_pools = BCM_DPP_AM_DEFAULT_NOF_POOLS;
        BCM_EXIT;
    }

    /* Enter your template id as a case here, and handle it as needed. */
    switch (template_id) {
    
    case dpp_am_template_egress_queue_mapping:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_queue_discount_cls:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_egress_port_discount_cls_type_raw:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_egress_port_discount_cls_type_cpu:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_egress_port_discount_cls_type_eth:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;

    case dpp_am_template_egress_port_discount_cls_type_tm:
        *nof_pools = SOC_DPP_DEFS_GET(unit, nof_cores);
        break;
#if ALLOC_MNGR_DEBUG_TEST
    /* Example: */
    case DEBUG_TEST_TESTED_RES:
        *nof_pools = DEBUG_TEST_NOF_TEMPLATE;
        break;
#endif

    default:
        *nof_pools = BCM_DPP_AM_DEFAULT_NOF_POOLS;
    }

    BCM_EXIT;    
exit:
    BCMDNX_FUNC_RETURN;
}

/****************************/
/** Base pool id functions **/
/****************************/
/*
 *   Function
 *      _bcm_dpp_am_base_pool_id_set
 *      bcm_dpp_am_resource_base_pool_id_set
 *      bcm_dpp_am_template_base_pool_id_set
 *   Purpose
 *      Given a resource id or a template id and a pool id, sets the pool to be the base pool of this resource.
 *      _bcm_dpp_am_base_pool_id_set is the internal implementation of the two other functions.
 *  
 *  
 *   Parameters
 *      (IN) unit                                       - unit number of the device
 *      (IN) res_id | template_id | res_template_id     - Resource id or template id to be mapped.
 *      (IN) base_pool_id                               - Base pool id to set. 
 *  
 * _bcm_dpp_am_base_pool_id_set only: 
 *      (IN) engine_id                                  - WB engine where the mapping should be saved
 *                                                          (either  template or resource)
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
STATIC int
_bcm_dpp_am_base_pool_id_set(int unit, int engine_id, int res_template_id, int base_pool_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, engine_id, &base_pool_id, res_template_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int
bcm_dpp_am_resource_base_pool_id_set(int unit, int resource_id, int base_pool_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_base_pool_id_set(unit, SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_MULTI_CORE_RESOURCE_TO_BASE_POOL_MAP, resource_id, base_pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int
bcm_dpp_am_template_base_pool_id_set(int unit, int template_id, int base_pool_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_base_pool_id_set(unit, SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_MULTI_CORE_TEMPLATE_TO_BASE_POOL_MAP, template_id, base_pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/****************************/
/** Base pool id functions **/
/****************************/
/*
 *   Function
 *      _bcm_dpp_am_id_to_pool_id_get
 *      bcm_dpp_am_resource_id_to_pool_id_get
 *      bcm_dpp_am_template_id_to_pool_id_get
 *   Purpose
 *      Given a resource id or a template id and a core id, returns the pool id for this <resource/template , core> tuple.
 *      _bcm_dpp_am_id_to_pool_id_get is the internal implementation of the two other functions.
 *  
 *  
 *   Parameters
 *      (IN) unit                                       - unit number of the device
 *      (IN) core_id                                    - core id for the resource. If core id is BCM_DPP_AM_INVALID_CORE_ID,
 *                                                          the mapping will be direct mapping (i.e., the pool id will be the
 *                                                          resource/template id and not according to the alloc manager's mapping)
 *      (IN) res_id | template_id | res_template_id     - Resource id or template id.
 *      (OUT)pool_id                                    - Pool id for the tuple.
 *  
 * _bcm_dpp_am_base_pool_id_set only: 
 *      (IN) engine_id                                  - WB engine where the mapping should be saved
 *                                                          (either  template or resource)
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
int
_bcm_dpp_am_id_to_pool_id_get(int unit, int core_id, int engine_id, int res_template_id, int *pool_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    if (core_id == BCM_DPP_AM_INVALID_CORE_ID) {
        *pool_id = res_template_id;
    } else {
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, engine_id, pool_id, res_template_id);
        BCMDNX_IF_ERR_EXIT(rv);

        *pool_id += core_id;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int
bcm_dpp_am_resource_id_to_pool_id_get(int unit, int core_id, int resource_id, int *pool_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_id_to_pool_id_get(unit, core_id, SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_MULTI_CORE_RESOURCE_TO_BASE_POOL_MAP, resource_id, pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int
bcm_dpp_am_template_id_to_pool_id_get(int unit, int core_id, int template_id, int *pool_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_id_to_pool_id_get(unit, core_id, SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_MULTI_CORE_TEMPLATE_TO_BASE_POOL_MAP, template_id, pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/****************************/
/** Base pool id functions **/
/****************************/
/*
 *   Function
 *      _bcm_dpp_am_pool_id_to_id_get
 *      bcm_dpp_am_pool_id_to_resource_id_get
 *      bcm_dpp_am_pool_id_to_template_id_get
 *   Purpose
 *      Given a pool id, returns the resource id and the core id mapped to this pool. 
 *      _bcm_dpp_am_pool_id_to_id_get is the internal implementation of the two other functions.
 *      It uses binary search in the base_pool_id mapping array to find the the closest base_pool_id.
 *  
 *   Parameters
 *      (IN) unit                                       - unit number of the device
 *      (IN) pool_id                                    - Pool id to be retrieved.
 *      (OUT)core_id                                    - Core id mapped to this pool.
 *      (OUT)res_id | template_id | res_template_id     - Resource id or template id mapped to this pool. 
 *  
 * _bcm_dpp_am_base_pool_id_set only: 
 *      (IN) engine_id                                  - WB engine where the mapping should be saved
 *                                                          (either  template or resource)
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
int 
_bcm_dpp_am_pool_id_to_id_get(int unit, int engine_id, int nof_pools, int pool_id, int *res_template_id, int *core_id){
    int rv;
#if DPP_AM_UTILS_COPY_WB_ARRAY
    int *map_array = NULL;
    int alloc_size = sizeof(int) * nof_pools;
#endif
    int imin, imax, imid;
    int current;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(res_template_id);
    BCMDNX_NULL_CHECK(core_id);

#if DPP_AM_UTILS_COPY_WB_ARRAY
    /* Copy the WB array to save the calls to individual vars. */
    BCMDNX_ALLOC(map_array, (alloc_size), "binary search array");
    if (map_array == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    rv = SOC_DPP_WB_ENGINE_MEMCPY_ARR_GET(unit, engine_id, map_array, 0, nof_pools);
    BCMDNX_IF_ERR_EXIT(rv);
#endif

    /* Binary search for the pool_id. */
    imax = nof_pools - 1;
    imin = 0;

    /* continue searching while [imin,imax] is not empty */
    while (imax >= imin)
        {
        /* calculate the midpoint for roughly equal partition */
        imid = imin + ((imax - imin) / 2);

#if DPP_AM_UTILS_COPY_WB_ARRAY
        current = map_array[imid];
#else
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, engine_id, &current, imid);
        BCMDNX_IF_ERR_EXIT(rv);
#endif
        if(current == pool_id){
            /* pool_id found at index imid */
            *res_template_id = imid;
            break;
        } else if (imin == imax) {
            /* We're one off from the pool_id. It's either the current, or the one below. */
            if (current < pool_id) {
                *res_template_id = imid;
            } else {
                *res_template_id = imid - 1;
            }
            break;
        }
        /* determine which subarray to search */
        else if (current < pool_id){
            /* change min index to search upper subarray */
            imin = imid + 1;
        } else {
            /* change max index to search lower subarray */
            imax = imid - 1;
        }
    }

    /* Set core id */

  
exit:
#if DPP_AM_UTILS_COPY_WB_ARRAY
    BCM_FREE(map_array);
#endif
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int 
bcm_dpp_am_pool_id_to_resource_id_get(int unit, int pool_id, int *res_id, int *core_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_pool_id_to_id_get(unit, SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_MULTI_CORE_RESOURCE_TO_BASE_POOL_MAP, 
                                       DPP_AM_RES_COSQ_START(unit), pool_id, res_id, core_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* See function description above. */
int 
bcm_dpp_am_pool_id_to_template_id_get(int unit, int pool_id, int *template_id, int *core_id){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_pool_id_to_id_get(unit, SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_MULTI_CORE_TEMPLATE_TO_BASE_POOL_MAP, 
                                       dpp_am_template_count, pool_id, template_id, core_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/********************************************/
/********************************************/
/**** GENERAL CORE HANDLING UTILS - END  ****/  
/********************************************/
/********************************************/


/********************************************/
/********************************************/
/** RESOURCE MANAGER ENCAPSULATION - START **/
/********************************************/
/********************************************/



int 
dpp_am_res_free_and_status(int unit, int core_id, int res_id, int count, int elem, uint32 *flags){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_free_and_status(unit,pool_id,count,elem,flags);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* if we aren't recovering data, update the warmboot state */
    if (DPP_AM_RES_USE_WARMBOOT(unit, res_id)) {
        rv = bcm_dpp_wb_dealloc_update(unit, pool_id, count, elem);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif    
exit:
    BCMDNX_FUNC_RETURN;
}

int
dpp_am_res_free(int unit, int core_id, int res_id, int count, int elem){
    int rv;
    uint32 flags;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_free_and_status(unit, core_id, res_id, count, elem, &flags);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int 
dpp_am_res_alloc(int unit, int core_id, int res_id, uint32 flags, int count, int *elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_alloc(unit, pool_id, flags, count, elem);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* if we aren't recovering data, update the warmboot state */
    if (DPP_AM_RES_USE_WARMBOOT(unit, res_id)) {
        rv = bcm_dpp_wb_alloc_update(unit, pool_id, count, *elem);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif

exit:
    BCMDNX_FUNC_RETURN;
}

int
dpp_am_res_check(int unit, int core_id, int res_id, int count, int elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_check(unit,pool_id,count,elem);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
dpp_am_res_used_count(int unit, int core_id, int res_id, int *used_count){
    int rv, pool_id;
    shr_res_type_info_t info;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_type_info_get(unit, pool_id, &info);
    BCMDNX_IF_ERR_EXIT(rv);

    *used_count = info.used;

exit:
    BCMDNX_FUNC_RETURN;
}



/*******/

/*******/

int 
dpp_am_res_alloc_align(int unit, int core_id, int res_id, uint32 flags, int align, int offset, int count, int *elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_alloc_align(unit, pool_id, flags, align, offset, count, elem);
    if(rv == BCM_E_RESOURCE) {
        BCM_ERR_EXIT_NO_MSG(rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* if we aren't recovering data, update the warmboot state */
    if (DPP_AM_RES_USE_WARMBOOT(unit, res_id)) {
        rv = bcm_dpp_wb_alloc_update(unit, pool_id, count, *elem);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif


exit:
    BCMDNX_FUNC_RETURN;
}


/*******/

int
dpp_am_res_alloc_align_tag(int unit, int core_id, int res_id, uint32 flags, int align, int offset, const void *tag, int count, int *elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_alloc_align_tag(unit, res_id, flags, align, offset, tag, count, elem);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* if we aren't recovering data, update the warmboot state */
    if (DPP_AM_RES_USE_WARMBOOT(unit, res_id)) {
        rv = bcm_dpp_wb_alloc_update(unit, pool_id, count, *elem);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif

exit:
    BCMDNX_FUNC_RETURN;
}


/*******/

int
dpp_am_res_check_all(int unit, int core_id, int res_id, int count, int elem){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_check_all(unit,pool_id,count,elem);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_res_set_pool(int unit, int core_id, int res_id, shr_res_allocator_t manager, 
                                 int low_id, int count, const void *extras, const char *name){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_pool_set(unit, pool_id, manager, low_id, count, extras, name);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_type_set(unit, pool_id, pool_id, 1, name);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
/*******/
int
dpp_am_res_unset_pool(int unit, int core_id, int res_id){
    int rv, pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_resource_id_to_pool_id_get(unit, core_id, res_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_type_unset(unit, pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_res_pool_unset(unit, pool_id);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/********************************************/
/********************************************/
/*** RESOURCE MANAGER ENCAPSULATION - END ***/
/********************************************/
/********************************************/


/********************************************/
/********************************************/
/** TEMPLATE MANAGER ENCAPSULATION - START **/
/********************************************/
/********************************************/

int 
dpp_am_template_allocate(int unit, int core_id, int template_id, uint32 flags, const void *data, int *is_allocated, int *template)
{
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, core_id, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_allocate(unit, pool_id, flags, data, is_allocated, template);
    BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_WARM_BOOT_SUPPORT
        if (!SOC_WARM_BOOT(unit))  {
            rv = bcm_dpp_wb_template_alloc_update(unit, pool_id, data, *template, 1 /* ref_cnt */);
            BCMDNX_IF_ERR_EXIT(rv);
        }
#endif
exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int 
dpp_am_template_allocate_group(int unit, int core_id, int template_id, 
                                            uint32 flags, const void *data, int nof_additions, int *is_allocated, int *template)
{
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, core_id, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_allocate_group(unit, pool_id, flags, data, nof_additions, is_allocated, template);
    BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_WARM_BOOT_SUPPORT
        if (!SOC_WARM_BOOT(unit))  {
            rv = bcm_dpp_wb_template_alloc_update(unit, pool_id, data, *template, nof_additions);
            BCMDNX_IF_ERR_EXIT(rv);
        }
#endif
exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_exchange(int unit, int core_id, int template_id, uint32 flags, const void *data, int old_template,
                                      int *is_last, int *template, int *is_allocated){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, core_id, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_exchange(unit, pool_id, flags, data, old_template, is_last, template, is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        rv = bcm_dpp_wb_template_dealloc_update(unit, pool_id, old_template, FALSE);
        BCMDNX_IF_ERR_EXIT(rv);
        
        rv = bcm_dpp_wb_template_alloc_update(unit, pool_id, data, *template, 1 /*ref_cnt */);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif
exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_free(int unit, int core_id, int template_id, int template, int *is_last){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, core_id, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_free(unit, pool_id, template, is_last);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        rv = bcm_dpp_wb_template_dealloc_update(unit, pool_id, template, FALSE);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif
exit:
    BCMDNX_FUNC_RETURN;
}

/*******/
int 
dpp_am_template_clear(int unit, int core_id, int template_id){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, core_id, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_clear(unit, pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        rv = bcm_dpp_wb_template_clear_update(unit, pool_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif
       

exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_exchange_test(int unit, int core_id, int template_id, uint32 flags, const void *data, int old_template,
                                      int *is_last, int *template, int *is_allocated){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, core_id, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_exchange_test(unit, pool_id, flags, data, old_template, is_last, template, is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_data_get(int unit, int core_id, int template_id, int template, void *data){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;
    
    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, core_id, template_id, &pool_id);
    if (rv != BCM_E_NONE) {
        BCM_RETURN_VAL_EXIT(rv);
    }

    rv = shr_template_data_get(unit, pool_id, template, data);
    if (rv != BCM_E_NONE) {
        BCM_RETURN_VAL_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*******/

int
dpp_am_template_template_get(int unit, int core_id, int template_id, const void *data, int *template){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, core_id, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_template_get(unit, pool_id, data, template);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*********/

int
dpp_am_template_ref_count_get(int unit, int core_id, int template_id, int template, uint32 *ref_count){
    int rv, pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, core_id, template_id, &pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_template_ref_count_get(unit, pool_id, template, ref_count);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}



/********************************************/
/********************************************/
/*** TEMPLATE MANAGER ENCAPSULATION - END ***/
/********************************************/
/********************************************/


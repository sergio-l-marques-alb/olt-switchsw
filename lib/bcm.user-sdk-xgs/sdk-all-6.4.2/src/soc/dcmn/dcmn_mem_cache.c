/*
 * $Id: soc_dcmn_mem_cache.c, v1 16/06/2014 09:55:39 azarrin $
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
 *
 * Purpose:    Implement soc cache memories.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/dpp/drv.h>

#include <soc/dcmn/error.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

/*************
 * FUNCTIONS *
 *************/
int
dcmn_mem_cache_attach(int unit, soc_reg_t mem, int block_instance)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;
   
    if (soc_mem_is_valid(unit, mem) &&
        ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {

       SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
       rc = soc_mem_cache_set(unit, mem, block_instance, 1);
       SOCDNX_IF_ERR_EXIT(rc);

    } else {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
   
exit:
    SOCDNX_FUNC_RETURN;
}

int
dcmn_mem_cache_detach(int unit, soc_reg_t mem, int block_instance)
{
    int rc;
  
    SOCDNX_INIT_FUNC_DEFS;
  
    if (soc_mem_is_valid(unit, mem) &&
            ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {
    rc = soc_mem_cache_set(unit, mem, block_instance, 0);
    SOCDNX_IF_ERR_EXIT(rc);

    SOC_MEM_INFO(unit, mem).flags &= (~SOC_MEM_FLAG_CACHABLE);
    }
    else {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
dcmn_mem_cache_could_be_cached(int unit, soc_mem_t cached_mem, int* cache_enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cache_enable);

    /* ask if table is dynamic */
    if (soc_mem_is_valid(unit, cached_mem) &&
        ((SOC_MEM_INFO(unit, cached_mem).blocks | SOC_MEM_INFO(unit, cached_mem).blocks_hi) != 0)) {
            *cache_enable = TRUE;
    } else {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
dcmn_mem_cache_init(int unit)
{
    int rc;
    int cache_enable;
	soc_mem_t mem;

    SOCDNX_INIT_FUNC_DEFS;

    /* attach cache memory */
	mem = 0;
/*  go over all cache memory soc properyes and cache them all*/
	{
		rc = dcmn_mem_cache_could_be_cached(unit, mem, &cache_enable); 
		SOCDNX_IF_ERR_EXIT(rc);

		if (TRUE == cache_enable) {
			rc = dcmn_mem_cache_attach(unit, mem, COPYNO_ALL);
			SOCDNX_IF_ERR_EXIT(rc);
		}
	}

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * $Id: $
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

#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/scache.h>


#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_utils.h>


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_INIT

/* in the future, scache init/deinit will be called from these functions */
int
soc_sw_state_utils_init(unit){
    return 0;
}

int
soc_sw_state_utils_deinit(unit){
    return 0;
}

/* 
 * this function does the interaction with scache module,
 * fetching\creating\modifying an scache buffer and return a pointer to it
 */
int
soc_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, soc_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists)
{
    int        rc = SOC_E_NONE;  
    uint32     allocated_size;
    int        alloc_size;
#ifdef BCM_WARM_BOOT_SUPPORT
    int        incr_size;
#endif

    if (scache_ptr == NULL) {
        return(SOC_E_PARAM);
    }

    if (oper == socSwStateScacheCreate) {
        if (size == NULL) {
            return(SOC_E_PARAM);
        }
        if (already_exists == NULL) {
            return(SOC_E_PARAM);
        }

        SOC_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size) + SOC_SW_STATE_SCACHE_CONTROL_SIZE;

#ifdef BCM_WARM_BOOT_SUPPORT
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if ((rc != SOC_E_NONE) && (rc != SOC_E_NOT_FOUND) ) {
            return(rc);
        }

        if (rc == SOC_E_NONE) { /* already exists */
            (*already_exists) = TRUE;
            return(SOC_E_PARAM);
        }
        else { /* need to create */
            (*already_exists) = FALSE;
            rc = soc_scache_alloc(unit, handle, alloc_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }

            rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }
            if ((*scache_ptr) == NULL) {
                return(SOC_E_MEMORY);
            }
        }

        if (alloc_size != allocated_size) {
            return(SOC_E_INTERNAL);
        }

#else /*BCM_WARM_BOOT_SUPPORT*/
        /* alloc buffer here instead of in scache */
        /*   (for buffers that store their original data on the buffer itself)*/
        *scache_ptr = sal_alloc(alloc_size, "scache buffer replacement");
        if ( *scache_ptr == NULL) {
            return SOC_E_MEMORY;
        }
        allocated_size = alloc_size;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    else if (oper == socSwStateScacheRetreive) {
        if (size == NULL) {
            return(SOC_E_PARAM);
        }

        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
        (*size) = allocated_size;
        
        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
    }
#endif /*BCM_WARM_BOOT_SUPPORT*/

    else if (oper == socSwStateScacheRealloc) {
        if (size == NULL) {
            return(SOC_E_PARAM);
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        /* get current size */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
#endif /*BCM_WARM_BOOT_SUPPORT*/

        /* allocate new size */
        SOC_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size) + SOC_SW_STATE_SCACHE_CONTROL_SIZE;
#ifdef BCM_WARM_BOOT_SUPPORT
        incr_size = alloc_size - allocated_size;

        rc = soc_scache_realloc(unit, handle, incr_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
#else /*BCM_WARM_BOOT_SUPPORT*/
        
        /* remove old buffer (supplied by caller) */
        if (*scache_ptr==NULL) {
            return SOC_E_INTERNAL;
        }
        else{
            sal_free(*scache_ptr - SOC_SW_STATE_SCACHE_CONTROL_SIZE);
        }
        /* alloc buffer here instead of in scache */
        *scache_ptr = sal_alloc(alloc_size, "soc_sw_state_buffer_info");
        if (*scache_ptr == NULL) {
            return SOC_E_MEMORY;
        }
        allocated_size = alloc_size;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    }
    else {
        return(SOC_E_PARAM);
    }

    /* Advance over scache control info */
    (*scache_ptr) += SOC_SW_STATE_SCACHE_CONTROL_SIZE;
    (*size) = (allocated_size - SOC_SW_STATE_SCACHE_CONTROL_SIZE); /* update size */

    return(rc);  
}

int soc_sw_state_scache_sync(int unit, soc_scache_handle_t handle, int offset, int size)
{
    SOC_INIT_FUNC_DEFS;
#ifdef BCM_WARM_BOOT_SUPPORT
    _SOC_IF_ERR_EXIT(soc_scache_partial_commit(unit, handle, offset + SOC_SW_STATE_SCACHE_CONTROL_SIZE, size));
#endif
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

/*
 * $Id: $
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
 */

#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/scache.h>


#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_utils.h>
#include <shared/swstate/sw_state_defs.h>


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_INIT

/* in the future, scache init/deinit will be called from these functions */
int
shr_sw_state_utils_init(unit){
    return 0;
}

int
shr_sw_state_utils_deinit(unit){
    return 0;
}

/* 
 * this function does the interaction with scache module,
 * fetching\creating\modifying an scache buffer and return a pointer to it
 */
int
shr_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, shr_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists)
{
    int        rc = SOC_E_NONE;  
    uint32     allocated_size;
    int        alloc_size;

#ifdef BCM_WARM_BOOT_SUPPORT
    int        incr_size;
#endif

    SOC_INIT_FUNC_DEFS; /* added for _SOC_EXIT_WITH_ER */

    if (scache_ptr == NULL) {
     _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (BSL_META_U(unit,
                  "Unit:%d scache_ptr is null.\n"), unit));

          /* return(SOC_E_PARAM); */
    }

    if (oper == socSwStateScacheCreate) {
        if (size == NULL) {
         _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d size is null.\n"), unit));
           /* return(SOC_E_PARAM); */
        }
        if (already_exists == NULL) {
         _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d already_exist is null.\n"), unit));
          /* return(SOC_E_PARAM); */
        }

        SHR_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size) + SHR_SW_STATE_SCACHE_CONTROL_SIZE;

#ifdef BCM_WARM_BOOT_SUPPORT
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if ((rc != SOC_E_NONE) && (rc != SOC_E_NOT_FOUND) ) {
            return(rc);
        }

        if (rc == SOC_E_NONE) { /* already exists */
            (*already_exists) = TRUE;
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d scache already_exist.\n"), unit));
            /* return(SOC_E_PARAM); */
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
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d Memory failure.\n"), unit));
             /* return(SOC_E_MEMORY); */
            }
        }

        if (alloc_size != allocated_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                       (BSL_META_U(unit,
                          "Unit:%d Allocation size is not enough.\n"), unit));
             /* return(SOC_E_INTERNAL); */
        }

#else /*BCM_WARM_BOOT_SUPPORT*/
        /* alloc buffer here instead of in scache */
        /*   (for buffers that store their original data on the buffer itself)*/
        *scache_ptr = sal_alloc(alloc_size, "scache buffer replacement");
        if ( *scache_ptr == NULL) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d Memory allocation failure.\n"), unit));
            return SOC_E_MEMORY;
        }
        allocated_size = alloc_size;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    else if (oper == socSwStateScacheRetreive) {
        if (size == NULL) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d Size is null.\n"), unit));
            /* return(SOC_E_PARAM); */
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
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (BSL_META_U(unit,
                      "Unit:%d Size is null.\n"), unit));
            /* return(SOC_E_PARAM); */
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        /* get current size */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
#endif /*BCM_WARM_BOOT_SUPPORT*/

        /* allocate new size */
        SHR_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size) + SHR_SW_STATE_SCACHE_CONTROL_SIZE;
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
             _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "Unit:%d scache_ptr is null.\n"), unit));
            return SOC_E_INTERNAL;
        }
        else{
            sal_free(*scache_ptr - SHR_SW_STATE_SCACHE_CONTROL_SIZE);
        }
        /* alloc buffer here instead of in scache */
        *scache_ptr = sal_alloc(alloc_size, "shr_sw_state_buffer_info");
        if (*scache_ptr == NULL) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (BSL_META_U(unit,
                  "Unit:%d Memory allocation failure.\n"), unit));
            /* return SOC_E_MEMORY; */
        }
        allocated_size = alloc_size;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    }
    else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
           (BSL_META_U(unit,
              "Unit:%d Invalid parameter.\n"), unit));
       /* return(SOC_E_PARAM); */
    }

    /* Advance over scache control info */
    (*scache_ptr) += SHR_SW_STATE_SCACHE_CONTROL_SIZE;
    (*size) = (allocated_size - SHR_SW_STATE_SCACHE_CONTROL_SIZE); /* update size */

    return(rc);  

exit:
SOC_FUNC_RETURN; /* added for _SOC_EXIT_WITH_ERR */

}

int shr_sw_state_scache_sync(int unit, soc_scache_handle_t handle, int offset, int size)
{
    SOC_INIT_FUNC_DEFS;
#ifdef BCM_WARM_BOOT_SUPPORT
    _SOC_IF_ERR_EXIT(soc_scache_partial_commit(unit, handle, offset + SHR_SW_STATE_SCACHE_CONTROL_SIZE, size));
#endif
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}





int shr_sw_state_ds_layout_node_t_clear(shr_sw_state_ds_layout_node_t *node) {
    SOC_INIT_FUNC_DEFS;
    sal_memset(node, 0x0, sizeof(shr_sw_state_ds_layout_node_t)); 


    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_ds_layout_node_set(int unit, 
                                    int node_id, 
                                    char* name, 
                                    int size, 
                                    int nof_pointer, 
                                    int array_size_0, 
                                    int array_size_1) {

    shr_sw_state_ds_layout_node_t* ds_layout_node = &(shr_sw_state_data_block_header[unit]->ds_layout_nodes[node_id]);
    SOC_INIT_FUNC_DEFS;
    SW_STATE_NODE_ID_CHECK(unit, node_id); 

    /* clear the current node */
    /*init the data to be all zeros*/
    sal_memset(ds_layout_node, 0x0, sizeof(shr_sw_state_ds_layout_node_t));

    if (sal_strlen(name) < sizeof(ds_layout_node->name)) {
        sal_strncpy(ds_layout_node->name, name, sizeof(ds_layout_node->name)); 
    } 
    /* name size is higher than node name array.
       Make sure we have a null character (don't copy the last character), so it's stay 0 */
    else {
        sal_strncpy(ds_layout_node->name, name, sizeof(ds_layout_node->name) -1 ); 
    }
   ds_layout_node->size = size;                   
   ds_layout_node->nof_pointer = nof_pointer;     
   ds_layout_node->array_sizes[0] = array_size_0; 
   ds_layout_node->array_sizes[1] = array_size_1; 
                                                               
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_ds_layout_add_brother(int unit, int older_brother_node_id, int younger_brother_node_id) {

    shr_sw_state_ds_layout_node_t* ds_layout_older_brother_node; 

    SOC_INIT_FUNC_DEFS;


    SW_STATE_NODE_ID_CHECK(unit, older_brother_node_id);  
    SW_STATE_NODE_ID_CHECK(unit, younger_brother_node_id);

    /* get older brother node */
     ds_layout_older_brother_node = &(shr_sw_state_data_block_header[unit]->ds_layout_nodes[older_brother_node_id]);  
    /* set the younger brother */
     ds_layout_older_brother_node->next_brother_node_index = younger_brother_node_id;  

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_ds_layout_add_child(int unit, int parent_node_id, int child_node_id) {

    shr_sw_state_ds_layout_node_t* ds_layout_parent_node; 

    SOC_INIT_FUNC_DEFS;


    SW_STATE_NODE_ID_CHECK(unit, parent_node_id); 
    SW_STATE_NODE_ID_CHECK(unit, child_node_id);  

    /* get parent node */
    ds_layout_parent_node = &(shr_sw_state_data_block_header[unit]->ds_layout_nodes[parent_node_id]);   

    /* set the elder child */
    if (ds_layout_parent_node->first_child_node_index == 0) {
        ds_layout_parent_node->first_child_node_index = child_node_id;  
    } 
    /* add brother to the last child */
    else {
        _SOC_IF_ERR_EXIT(shr_sw_state_ds_layout_add_brother(unit, ds_layout_parent_node->last_child_node_index, child_node_id)); 
    }
    /* set the last child */
    ds_layout_parent_node->last_child_node_index = child_node_id; 

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}



int shr_sw_state_ds_layout_update_offset(int unit, int node_id, int offset) {

    shr_sw_state_ds_layout_node_t* ds_layout_node; 

    SOC_INIT_FUNC_DEFS;

    SW_STATE_NODE_ID_CHECK(unit, node_id); 

    /* get node */
    ds_layout_node = &(shr_sw_state_data_block_header[unit]->ds_layout_nodes[node_id]); 
    /* update offset */
    ds_layout_node->offset = offset; 

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}



#undef _ERR_MSG_MODULE_NAME

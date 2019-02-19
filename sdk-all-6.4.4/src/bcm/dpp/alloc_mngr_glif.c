/*
 * $Id: alloc_mngr_glif.c,v 1.312 Broadcom SDK $
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
 * File:        alloc_mngr_glif.c
 * Purpose:     Global lif allocation manager for DNX chips.
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>
#include <shared/swstate/sw_state_access.h>

#include <bcm_int/common/debug.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/module.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_wb_engine.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/dpp/wb_db_alloc.h>
#endif

#include <bcm_int/dpp/gport_mgmt.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#define _BCM_DPP_GLOBAL_LIF_MAPPING_HASH_SIZE               (32 * 1024) /* We need a power of 2, but it doesn't have to be 
                                                                            the same as the number of entries we'll use */


#define _BCM_DPP_GLOBAL_LIF_MAPPING_UNMAPPED                (-1)

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_MASK    (0X1f)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SHIFT   (27)

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_MASK     (0Xffffff)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_SHIFT    (0)

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SET(_global_lif, _type)     \
    ((_global_lif) | ((_type) << _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SHIFT))

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_GET(_global_lif)     \
    (((_global_lif) & _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_MASK) >> _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SHIFT)

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(_global_lif)     \
    (((_global_lif) & _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_MASK) >> _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_SHIFT)

#define GLIF_INGRESS_GLOBAL_LIF_TO_LIF_HTB 0
#define GLIF_EGRESS_GLOBAL_LIF_TO_LIF_HTB  1

#define ALLOC_MNGR_GLIF_ACCESS                          sw_state_access[unit].dpp.bcm.alloc_mngr_glif

typedef enum _bcm_dpp_global_lif_mapping_type_s {
    _bcm_dpp_global_lif_mapping_type_none,
    _bcm_dpp_global_lif_mapping_type_count
} _bcm_dpp_global_lif_mapping_type_t;


typedef struct {
    int     is_htb;
    int     max_entries;    /* How many entries can be in this mapping. */
    char    name[50];       /* Name of the hash table (if valid) */
    int     max_key;        /* Maximum key of the hash table */
    int     max_val;        /* Maimum value to map to */
    int     (*get_function)(int unit, int index, int *value); /* Wb variable get function */
    int     (*set_function)(int unit, int index, int value); /* Wb variable set function */
    int     htb_type; 
} _bcm_dpp_global_lif_mapping_data_t;

typedef int (*_bcm_dpp_global_lif_map_iterate_cb)(int unit, uint32 flags);

int _lif_egress_global_lif_to_local_htb_idx[BCM_MAX_NUM_UNITS];
int _lif_ingress_global_lif_to_local_htb_idx[BCM_MAX_NUM_UNITS];

STATIC int
_bcm_dpp_global_lif_map_iterate(int unit, _bcm_dpp_global_lif_map_iterate_cb cb);

STATIC int 
_bcm_dpp_global_lif_map_create(int unit, uint32 flags);

STATIC int 
_bcm_dpp_global_lif_map_destroy(int unit, uint32 flags);

STATIC int 
_bcm_dpp_global_lif_map_get(int unit, uint32 flags, _bcm_dpp_global_lif_mapping_data_t *data);

STATIC int
_bcm_dpp_global_lif_mapping_find_internal(int unit, uint32 flags, int key, int *data, uint8 remove);

STATIC int 
_bcm_dpp_global_lif_mapping_insert_internal(int unit, uint32 flags, int key, int data);

/* Decode global lif flags */
STATIC int
_bcm_dpp_global_lif_mapping_decode(uint32 flags, uint8 *is_global_to_local, _bcm_dpp_global_lif_mapping_type_t *type);


/* 
 *  GLOBAL LIF MAPPING - START
 */ 


/*
 *  GLOBAL LIF WRAPPER FUNCTIONS - START
 */

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_decode
 * Purpose:
 *      Given global lif mapping flags, decodes them and returns the relevant flags.
 *  
 * Parameters:
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_*
 *     is_global_to_local-(OUT) Optional (can be null).
 *                                  0: local to global. 1: global to local
 *     type             - (OUT) Optional (can be null). The global lif type.
 * Returns: 
 *     BCM_E_NONE       - If all flags are valid 
 *     BCM_E_INTERNAL   - If no direction flag was selected for ingress/egress or global->local/local->global
 */
STATIC int
_bcm_dpp_global_lif_mapping_decode(uint32 flags, uint8 *is_global_to_local, _bcm_dpp_global_lif_mapping_type_t *type){
    uint8 is_local_to_global;
    uint8 is_ingress, is_egress;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    is_ingress = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS;
    is_egress = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS;
    
    /* Check that at least one of is_ingress or is_egress is set */
    if ((!is_ingress && !is_egress)
        || (is_ingress && is_egress)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Global lif mapping flags must contain either ingress or egress.")));
    }

    /* Check that at least one of global_to_local or local_to_global is set.
       This argument is optional because not all functions use it. */
    if (is_global_to_local) {
        *is_global_to_local = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL;
        is_local_to_global = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL;
        if ((!*is_global_to_local && !is_local_to_global)
            || (*is_global_to_local && is_local_to_global)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Global lif mapping flags must contain either global_to_local or local_to_global.")));
        }
    }

    /* When there are types available, add them here */
    DPP_IF_NOT_NULL_FILL(type, _bcm_dpp_global_lif_mapping_type_none);

exit:
    BCMDNX_FUNC_RETURN_NO_UNIT;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_create
 * Purpose:
 *      Create mapping between a global lif, and a local in lif and/or local out lif.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS and/or _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS
 *     global lif       - (IN) Global lif to be mapped. 
 *     local_lif        - (IN) Local lif to be mapped.
 * Returns: 
 *     BCM_E_NONE       - If the mapping was successful.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_create(int unit, uint32 flags, int global_lif, int local_lif){
    int rv;
    _bcm_dpp_global_lif_mapping_type_t type;
    uint32 internal_flags;
    int global_lif_with_type;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get global lif mapping type. */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, NULL, &type);
    BCMDNX_IF_ERR_EXIT(rv);

    global_lif_with_type = _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SET(global_lif, type);

    internal_flags = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_IN_EG_DIRECTIONS;
    
    /* Global to local */
    rv = _bcm_dpp_global_lif_mapping_insert_internal(unit, internal_flags | _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL, global_lif_with_type, local_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Local to global */
    rv = _bcm_dpp_global_lif_mapping_insert_internal(unit, internal_flags | _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL, local_lif, global_lif_with_type);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_remove
 * Purpose:
 *      Remove mapping between a global lif, and a local in lif and/or local out lif.
 *      Use the _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL flag and provide the global lif argument 
 *      or use the _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL with INGRESS/EGRESS flag and provide
 *      ingress/egress local lif.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_*
 *     global lif       - (IN) Global lif to be unmapped (with _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL flag). 
 *     local_lif        - (IN) Local lif to be unmapped (with _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL flag).
 * Returns: 
 *     BCM_E_NONE       - If the unmapping was succesful.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_remove(int unit, uint32 flags, int global_lif, int local_lif){
    int rv;
    uint8 is_global_to_local;
    int given_lif, mapped_lif;
    _bcm_dpp_global_lif_mapping_type_t type;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get type and global/local direction */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, &is_global_to_local, &type);
    BCMDNX_IF_ERR_EXIT(rv);

    given_lif = (is_global_to_local) ? _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SET(global_lif, type) : local_lif;
    
    /* Remove */
    /* Find the mapping for the given global/local lif, and remove it. */
    rv = _bcm_dpp_global_lif_mapping_find_internal(unit, flags, given_lif, &mapped_lif, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Use the mapped lif we found above and remove the opposite mapping. */
    rv = _bcm_dpp_global_lif_mapping_find_internal(unit, flags ^ _BCM_DPP_GLOBAL_LIF_MAPPING_GLOB_LOC_DIRECTIONS /* Will return the opposite direction */, mapped_lif, &given_lif, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_local_to_global_get
 * Purpose:
 *      Find mapping from local lif to global lif.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS OR _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS
 *     local_lif        - (IN) Local lif to be matched.
 *     global_lif       - (OUT) Found global lif.
 * Returns: 
 *     BCM_E_NONE       - If the value was retrieved successfully..
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_local_to_global_get(int unit, uint32 flags, int local_lif, int *global_lif){
    int rv;
    uint8 is_mapped;

    BCMDNX_INIT_FUNC_DEFS;

    /* Check and analyze input */
    BCMDNX_NULL_CHECK(global_lif); 

    /* In case lif is invalid return */
    if (local_lif == BCM_GPORT_INVALID) {
        *global_lif = BCM_GPORT_INVALID;
        BCM_EXIT;
    }

    /* Verify flags */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    /* If lif is a direct lif, just return it. */
    rv = _bcm_dpp_lif_id_is_mapped(unit, flags & _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_lif, &is_mapped);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_mapped) {
        *global_lif = local_lif;
        BCM_EXIT;
    }


    /* Find mapping */
    rv = _bcm_dpp_global_lif_mapping_find_internal(unit, flags | _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL, 
                                                   local_lif, global_lif, FALSE);

    /* A not found result doesn't necessarily indicate an error, and therefore shouldn't print an error. */
    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(rv);

    *global_lif = _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(*global_lif);
    BCM_RETURN_VAL_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_global_to_local_get
 * Purpose:
 *      Find mapping from global lif to local lif.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS OR _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS
 *     global_lif       - (IN) Global lif to be matched.
 *     local_lif        - (OUT) Found local lif.
 * Returns: 
 *     BCM_E_NONE       - If the value was retrieved successfully.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_global_to_local_get(int unit, int flags, int global_lif, int *local_lif){
    int rv;
    _bcm_dpp_global_lif_mapping_type_t type;
    uint8 is_mapped;
    int global_lif_with_type;
    BCMDNX_INIT_FUNC_DEFS;

    /* Check and analyze input. */

    BCMDNX_NULL_CHECK(local_lif); 

    /* In case lif is invalid return */
    if (global_lif == BCM_GPORT_INVALID) {
        *local_lif = BCM_GPORT_INVALID;
        BCM_EXIT;
    }

    /* Verify input */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, NULL, &type);
    BCMDNX_IF_ERR_EXIT(rv);


    /* If lif is not a global lif, just return it. */
    rv = _bcm_dpp_lif_id_is_mapped(unit, flags & _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_lif, &is_mapped);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_mapped) {
        *local_lif = global_lif;
        BCM_EXIT;
    }
    
    global_lif_with_type = _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SET(global_lif, type);

    /* Get local lif */
    rv = _bcm_dpp_global_lif_mapping_find_internal(unit, flags | _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL, 
                                                   global_lif_with_type, local_lif, FALSE);
    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_init
 * Purpose:
 *      Initiates all global-local lif mappings on the device.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_init(int unit){
    int rv;
    BCMDNX_INIT_FUNC_DEFS

    /* Init array buffer - must be done after initializing the arrays. */
    rv = ALLOC_MNGR_GLIF_ACCESS.alloc(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Iterate over all maps and create them.*/
    rv = _bcm_dpp_global_lif_map_iterate(unit, _bcm_dpp_global_lif_map_create);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_deinit
 * Purpose:
 *      Destroys all global-local lif mappings on the device.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_deinit(int unit){
    int rv;
    BCMDNX_INIT_FUNC_DEFS

    rv = ALLOC_MNGR_GLIF_ACCESS.free(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Iterate over all maps and destroy them.*/
    rv = _bcm_dpp_global_lif_map_iterate(unit, _bcm_dpp_global_lif_map_destroy);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_map_iterate
 * Purpose:
 *      Iterates over all ingress/egress and global_to_local/local_to_global mappings
 *      and applies a callback to each of them.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     cb               - (IN) Callback to be applied to the mapping.
 * Returns: 
 *     BCM_E_NONE       - If the iteration and all the callbacks were succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC int
_bcm_dpp_global_lif_map_iterate(int unit, _bcm_dpp_global_lif_map_iterate_cb cb){
    int rv;
    uint32  in_eg_flags[2] = {_BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS};
    uint32  mapping_flags[2] = {_BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL, _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL};
    uint32 i, j;
    BCMDNX_INIT_FUNC_DEFS

    /* Iterate over the flags and apply the callback. */
    for (i = 0 ; i < 2 ; i++) {
        for (j = 0 ; j < 2 ; j++) {
            rv = cb(unit, in_eg_flags[i] | mapping_flags[j]);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_map_get
 * Purpose:
 *      Given an INGRESS/EGRESS direction flag and a local_to_global/global_to_local
 *      direction flag, returns the relevant map data: Array or hash, maximum entries, maximum value, etc.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* flags.
 *     data             - (OUT) Metadata for the table / array.
 * Returns: 
 *     BCM_E_NONE       - If the data was retrieved successfully.
 *     BCM_E_*          - Otherwise.
 */
STATIC int 
_bcm_dpp_global_lif_map_get(int unit, uint32 flags, _bcm_dpp_global_lif_mapping_data_t *data){
    int rv;
    uint8 is_ingress, is_global_to_local;
    BCMDNX_INIT_FUNC_DEFS;

    /* Decode the direction flags */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, &is_global_to_local, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    is_ingress = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS;


    if (is_ingress) {
        if (is_global_to_local) { /* Ingress global to local - use hash table. */
            data->max_entries = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
            data->max_key = SOC_DPP_DEFS_GET(unit, nof_global_lifs);
            data->max_val = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
            sal_sprintf(data->name, "_bcm_dpp_global_lif_ingress_global_lif_to_local");
            data->htb_type =  GLIF_INGRESS_GLOBAL_LIF_TO_LIF_HTB;
            data->is_htb = 1;
        } else { /* Ingress local to global - use WB array. */
            data->max_entries = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
            data->max_key = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
            data->max_val = SOC_DPP_DEFS_GET(unit, nof_global_lifs);
            data->get_function = ALLOC_MNGR_GLIF_ACCESS.ingress_lif_to_global_lif.get;
            data->set_function = ALLOC_MNGR_GLIF_ACCESS.ingress_lif_to_global_lif.set;
            data->is_htb = 0;
        }
    } else { /* Egress */
        if (is_global_to_local) { /* Egress global to local - use hash table. */
            data->max_entries = SOC_DPP_DEFS_GET(unit, nof_eedb_lines);
            data->max_key = _BCM_DPP_MAX_VIRTUAL_LIF(unit);
            data->max_val  = SOC_DPP_DEFS_GET(unit, nof_eedb_lines);
            sal_sprintf(data->name, "_bcm_dpp_global_lif_egress_global_lif_to_local");
            data->htb_type =  GLIF_EGRESS_GLOBAL_LIF_TO_LIF_HTB;
            data->is_htb = 1;

        } else { /* Egress local to global - use WB array. */
            data->max_entries = SOC_DPP_DEFS_GET(unit, nof_eedb_lines);
            data->max_key  = SOC_DPP_DEFS_GET(unit, nof_eedb_lines);
            data->max_val = _BCM_DPP_MAX_VIRTUAL_LIF(unit);
            data->get_function = ALLOC_MNGR_GLIF_ACCESS.egress_lif_to_global_lif.get;
            data->set_function = ALLOC_MNGR_GLIF_ACCESS.egress_lif_to_global_lif.set;
            data->is_htb = 0;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_map_create
 * Purpose:
 *      Initiate a single hash table/ array according to the direction flags.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* to indicate the mapping direction.
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC int 
_bcm_dpp_global_lif_map_create(int unit, uint32 flags){
    int rv;
    int htb_idx;
    _bcm_dpp_global_lif_mapping_data_t mapping_data;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get mapping data. */
    rv = _bcm_dpp_global_lif_map_get(unit, flags, &mapping_data);
    BCMDNX_IF_ERR_EXIT(rv);

    /* If map is a hash table, initiate it. */
    if (mapping_data.is_htb) {
        rv = sw_state_htb_create(unit, &htb_idx, _BCM_DPP_GLOBAL_LIF_MAPPING_HASH_SIZE, sizeof(int), sizeof(int),
               mapping_data.name);
        BCMDNX_IF_ERR_EXIT(rv);

        if (mapping_data.htb_type == GLIF_INGRESS_GLOBAL_LIF_TO_LIF_HTB) {
            _lif_ingress_global_lif_to_local_htb_idx[unit] = htb_idx;
        }
        else if (mapping_data.htb_type == GLIF_EGRESS_GLOBAL_LIF_TO_LIF_HTB) {
            _lif_egress_global_lif_to_local_htb_idx[unit] = htb_idx;
        }
        else {
            BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);
        }

    } else if (!SOC_WARM_BOOT(unit)) {
        /*If map is an array, and we're not in warmboot, then set all entries to unmapped. */
        int i;
        int illegal = _BCM_DPP_GLOBAL_LIF_MAPPING_UNMAPPED;

        for (i = 0 ; i < mapping_data.max_entries ; i++) {
            rv = mapping_data.set_function(unit, i, illegal);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_map_destroy
 * Purpose:
 *      Destroys a single hash table/ array according to the direction flags.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* to indicate the mapping direction.
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC int 
_bcm_dpp_global_lif_map_destroy(int unit, uint32 flags){
    int rv;
    int htb_idx;
    _bcm_dpp_global_lif_mapping_data_t mapping_data;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get mapping. */
    rv = _bcm_dpp_global_lif_map_get(unit, flags, &mapping_data);
    BCMDNX_IF_ERR_EXIT(rv);

    if (mapping_data.is_htb) {

        if (mapping_data.htb_type == GLIF_INGRESS_GLOBAL_LIF_TO_LIF_HTB) {
            htb_idx = _lif_ingress_global_lif_to_local_htb_idx[unit];
        }
        else if (mapping_data.htb_type == GLIF_EGRESS_GLOBAL_LIF_TO_LIF_HTB) {
            htb_idx = _lif_egress_global_lif_to_local_htb_idx[unit];
        }
        else {
            BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);
        }
        rv = sw_state_htb_destroy(unit, htb_idx);
        BCMDNX_IF_ERR_EXIT(rv);
    } 
    /* If not hash table (array) then nothing to do here */

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_find_internal
 * Purpose:
 *      Find mapped lif by direction flags.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* flags.
 *     key              - (IN) Lif to be looked up.
 *     data             - (OUT) Found lif.
 *     remove           - (IN) Delete found key.
 * Returns: 
 *     BCM_E_NONE       - If the value was found or removed successfully.
 *     BCM_E_*          - Otherwise.
 */
STATIC int
_bcm_dpp_global_lif_mapping_find_internal(int unit, uint32 flags, int key, int *data, uint8 remove){
    int rv;
    _bcm_dpp_global_lif_mapping_data_t mapping_data;
    int htb_idx = -1;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get mapping info */
    rv = _bcm_dpp_global_lif_map_get(unit, flags, &mapping_data);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check if the key is legal */
    if (_BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(key) > mapping_data.max_key || key < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given lif id is illegal: %d"), key));
    }

    /* Find (and delete, if required) the entry. */ 
    if (mapping_data.is_htb) {

        if (mapping_data.htb_type == GLIF_INGRESS_GLOBAL_LIF_TO_LIF_HTB) {
            htb_idx = _lif_ingress_global_lif_to_local_htb_idx[unit];
        }
        else if (mapping_data.htb_type == GLIF_EGRESS_GLOBAL_LIF_TO_LIF_HTB) {
            htb_idx = _lif_egress_global_lif_to_local_htb_idx[unit];
        }
        else {
            BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);
        }

        /* If the map is a hash table, find the entry, remove if required. */
        rv = sw_state_htb_find(unit, htb_idx, (sw_state_htb_key_t) &key, (sw_state_htb_data_t) (uint8*) data, remove);
        if (rv != BCM_E_NONE) {
            BCM_RETURN_VAL_EXIT(rv);
        }

    } else {
        /* If the map is an array, find it in the WB array. */
        rv = mapping_data.get_function(unit, key, data);
        BCMDNX_IF_ERR_EXIT(rv);

        /* If entry is not initialized, return not found */
        if (*data == _BCM_DPP_GLOBAL_LIF_MAPPING_UNMAPPED) {
            BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
        }

        /* If remove is set, set the entry to unmapped */
        if (remove) {
            int illegal = _BCM_DPP_GLOBAL_LIF_MAPPING_UNMAPPED;
            rv = mapping_data.set_function(unit, key, illegal);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_insert_internal
 * Purpose:
 *      Insert mapped lif by direction flags.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* flags.
 *     key              - (IN) Lif to be mapped from.
 *     data             - (IN) Lif to be mapped to.
 *  
 * Returns: 
 *     BCM_E_NONE       - If the mapping was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC int 
_bcm_dpp_global_lif_mapping_insert_internal(int unit, uint32 flags, int key, int data){
    int rv;
    _bcm_dpp_global_lif_mapping_data_t mapping_data;
    int htb_idx;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get mapping info */
    rv = _bcm_dpp_global_lif_map_get(unit, flags, &mapping_data);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Verify that both key and data are in legal range */
    if (key < 0 || data < 0 
        || _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(key) > mapping_data.max_key 
        || _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(data) > mapping_data.max_val) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given lif id is illegal")));
    }

    /* Map hash table or insert to WB array. */
    if (mapping_data.is_htb) {

        if (mapping_data.htb_type == GLIF_INGRESS_GLOBAL_LIF_TO_LIF_HTB) {
            htb_idx = _lif_ingress_global_lif_to_local_htb_idx[unit];
        }
        else if (mapping_data.htb_type == GLIF_EGRESS_GLOBAL_LIF_TO_LIF_HTB) {
            htb_idx = _lif_egress_global_lif_to_local_htb_idx[unit];
        }
        else {
            BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);
        }

        rv = sw_state_htb_insert(unit, htb_idx, (sw_state_htb_key_t) &key, (sw_state_htb_data_t) &data);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = mapping_data.set_function(unit, key, data);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 *  MAPPING MODULE UTILITIES - END
 */ 

/*
 * GLOBAL LIF MAPPING - END
 */

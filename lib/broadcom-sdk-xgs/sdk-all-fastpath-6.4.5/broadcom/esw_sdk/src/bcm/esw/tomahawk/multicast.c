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
 *
 * File:        multicast.c
 * Purpose:     Tomahawk multicast functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/esw/tomahawk.h>

#define TH_NUM_PIPES 4
#define TH_PORTS_PIPE 34
#define TH_NUM_TRUNKS 1024

typedef struct _bcm_th_aggid_info_s {
    uint8 free_aggid[TH_PORTS_PIPE];
} _bcm_th_aggid_info_t;

typedef struct _bcm_th_trunk_aggid_map_s {
    uint8 agg_id;
} _bcm_th_trunk_aggid_map_t;

typedef struct _bcm_th_trunk_aggid_info_s {
    _bcm_th_trunk_aggid_map_t trunk_aggid_map[TH_NUM_TRUNKS];
} _bcm_th_trunk_aggid_info_t;


STATIC _bcm_th_aggid_info_t *free_aggid[BCM_MAX_NUM_UNITS];
STATIC _bcm_th_trunk_aggid_info_t *trunk_aggid_info[BCM_MAX_NUM_UNITS];


/*
 * Function:
 *      _bcm_th_aggregation_id_list_init
 * Purpose:
 *      Initialize aggregation ID list.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_th_aggregation_id_list_init(int unit)
{
    int   rv = BCM_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (free_aggid[unit] != NULL) {
        rv = _bcm_th_aggregation_id_list_detach(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    free_aggid[unit] = sal_alloc((TH_NUM_PIPES * 
                                 sizeof(_bcm_th_aggid_info_t)), "free_aggid");
    if (free_aggid[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(free_aggid[unit], 0xFF, TH_NUM_PIPES * 
               sizeof(_bcm_th_aggid_info_t));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_aggregation_id_list_detach
 * Purpose:
 *      Free aggregation ID list.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_aggregation_id_list_detach(int unit)
{
	if (free_aggid[unit] != NULL) {
		sal_free(free_aggid[unit]);
		free_aggid[unit] = NULL;
	}
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_aggid_trunk_map_init
 * Purpose:
 *      Initialize aggregation ID to trunk map array.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_th_aggid_trunk_map_init(int unit)
{
    int   rv = BCM_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (trunk_aggid_info[unit] != NULL) {
        rv = _bcm_th_aggid_trunk_map_detach(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    trunk_aggid_info[unit] = sal_alloc((TH_NUM_PIPES * 
                                       sizeof (_bcm_th_trunk_aggid_map_t) *
                                       sizeof(_bcm_th_trunk_aggid_info_t)),
                                       "trunk_aggid_info");
    if (trunk_aggid_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(trunk_aggid_info[unit], 0xFF, TH_NUM_PIPES * 
               sizeof (_bcm_th_trunk_aggid_map_t) *
               sizeof(_bcm_th_trunk_aggid_info_t));


    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_aggid_trunk_map_detach
 * Purpose:
 *      Free aggregation ID list.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_aggid_trunk_map_detach(int unit)
{
	if (trunk_aggid_info[unit] != NULL) {
		sal_free(trunk_aggid_info[unit]);
		trunk_aggid_info[unit] = NULL;
	}
    return BCM_E_NONE;
}
/*
 * Function:
 *      get_free_aggregation_id
 * Purpose:
 *      Recover software state for warm boot.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int get_free_aggregation_id(int unit, int pipe) {
    int aggid = 0xFF;
    _bcm_th_aggid_info_t *aggid_info;
    int id;

    aggid_info = &free_aggid[unit][pipe];

    for (id = 0; id < TH_PORTS_PIPE; id++) {
        aggid = aggid_info->free_aggid[id];        
        if (aggid != 0xFF) {
            /* Free aggId exists
             * return this aggId and mark it unavailable for other ports
             */
            aggid_info->free_aggid[id] = 0xFF;
            break;
        }
    }
    
    return aggid;

}

/*
 * Function:
 *      set_free_aggregation_id
 * Purpose:
 *      Recover software state for warm boot.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */

static int set_free_aggregation_id(int unit, int pipe, uint8 agg_id) {

    _bcm_th_aggid_info_t *aggid_info;
    uint8 id;

    aggid_info = &free_aggid[unit][pipe];

    for (id = 0; id < TH_PORTS_PIPE; id++) {
        if (aggid_info->free_aggid[id] == 0xFF) {
            aggid_info->free_aggid[id] = agg_id;
            break;
        };
    }

    if ( id == TH_PORTS_PIPE) {
        return BCM_E_FULL;
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      get_aggid_from_trunkid
 * Purpose:
 *      Get aggregation ID for a given trunk for a pipe
 * Parameters:
 *      unit  - (IN) Unit number.
 *      pipe  - (IN) Pipe number
 *      tgid  - (IN) Trunk ID
 * Returns:
 *      Aggregation ID
 */

static uint8 get_aggid_from_trunkid(int unit, int pipe, int tgid) {
    
     uint8 aggid;
    _bcm_th_trunk_aggid_info_t *aggid_map;
    
    aggid_map = &trunk_aggid_info[unit][pipe];
    aggid = aggid_map->trunk_aggid_map[tgid].agg_id;
    
    return aggid;
}


/*
 * Function:
 *      _bcm_th_set_repl_port_agg_map
 * Purpose:
 *      Set Port Aggregration ID for members of a trunk
 * Parameters:
 *      unit                - (IN) Unit number.
 *      local_member_array  - (IN) Array of local trunk members
 *      local_member_count  - (IN) Number of local trunk members
 * Returns:
 *      Aggregation ID
 */

int _bcm_th_set_repl_port_agg_map(int unit, const bcm_port_t *local_member_array,
                                  int local_member_count, bcm_trunk_t tgid) {

    uint32 regval = 0;
    uint8 agg_id = 0xFF;
    uint8 prev_agg_id = 0xFF;
    int ltp_idx;    /* Local trunk port index      */
    int port;       /* Local trunk port            */
    int mmu_port;   /* MMU port number        */
    int pipe;       /* Pipe to which port belongs  */
    int prev_pipe = -1;
    _bcm_th_trunk_aggid_info_t *aggid_map;
    int t_aggid;
    soc_info_t *si;
    si = &SOC_INFO(unit);    
    
    if ((local_member_count == 0) || (NULL == local_member_array)) {
        return BCM_E_PARAM;
    }

    /*
     * Tomahawk introduced REPL_PORT_AGG_MAP table.
     * For all members of a trunk belonging to same pipe, REPL_PORT_AGG_MAP table
     * will be programmed with same L3MC-port-Agg-ID. This ID will be used with
     * L3MC bitmap and will eventually result in single entry in REPL_HEAD table
     * representing all trunk memebers.
     */

    for (ltp_idx = 0; ltp_idx < local_member_count; ltp_idx++) {
        port = local_member_array[ltp_idx];                
        pipe = si->port_pipe[port];                
        mmu_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[mmu_port];

        if (pipe != prev_pipe) {
            t_aggid = get_aggid_from_trunkid(unit, pipe, tgid);
            if (t_aggid != 0xFF) {
                agg_id = t_aggid;                    
            } else {
                agg_id = mmu_port % 64;
                soc_reg_field_set(unit, MMU_DQS_REPL_PORT_AGG_MAPr, &regval,
                                  L3MC_PORT_AGG_IDf, agg_id);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, MMU_DQS_REPL_PORT_AGG_MAPr,
                                   port, 0, regval));
            }
            /* L3mC-Port-Agg-ID is unique for each trunk for a given pipe. 
            * Mark this ID unavailable for other trunks on same pipe. 
            */
            aggid_map = &trunk_aggid_info[unit][pipe];
            aggid_map->trunk_aggid_map[tgid].agg_id = agg_id;    
        } else {
            soc_reg_field_set(unit, MMU_DQS_REPL_PORT_AGG_MAPr, &regval,
                              L3MC_PORT_AGG_IDf, prev_agg_id);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_DQS_REPL_PORT_AGG_MAPr,
                                port, 0, regval));
            /* put this port in free aggregation ID list */
            BCM_IF_ERROR_RETURN
                (set_free_aggregation_id(unit, pipe, (mmu_port % 64)));
        }

        prev_pipe = pipe;
        prev_agg_id = agg_id;
    }
        
    return BCM_E_NONE;

}

#endif

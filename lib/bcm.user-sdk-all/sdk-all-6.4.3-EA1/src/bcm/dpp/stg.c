/*

 * $Id: stg.c,v 1.35 Broadcom SDK $
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
 * File:        stg.c
 * Purpose:     Spanning tree group support
 *
 * Multiple spanning trees (MST) is supported on this chipset
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_STG

#include <shared/bsl.h>

#include <bcm_int/common/debug.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stg.h>
#include <bcm/vlan.h>

#include <bcm_int/control.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/stg.h>
#include <bcm_int/dpp/wb_db_stg.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/state.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/alloc_mngr.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_vsi.h>
#include <soc/dpp/PPD/ppd_api_port.h>


bcm_stg_info_t  stg_info[BCM_MAX_NUM_UNITS];


#define STG_DB_LOCK(unit) \
        sal_mutex_take(stg_info[(unit)].lock, sal_mutex_FOREVER)

#define STG_DB_UNLOCK(unit) sal_mutex_give(stg_info[(unit)].lock)

#define STG_CNTL(unit)  stg_info[(unit)]

#define STG_CHECK_INIT(unit)                                           \
    do {                                                               \
        BCM_DPP_UNIT_CHECK(unit);                                      \
        if (unit >= BCM_MAX_NUM_UNITS) return BCM_E_UNIT;              \
        if (STG_CNTL(unit).init == FALSE) return BCM_E_INIT;           \
        if (STG_CNTL(unit).init != TRUE) return STG_CNTL(unit).init;   \
    } while (0);

#define STG_CHECK_STG(si, stg)                            \
    if ((stg) < (si)->stg_min || (stg) > (si)->stg_max) return BCM_E_BADID;

#define STG_CHECK_PORT(unit, port)                      \
    if ((port < 0)                                      \
        || (!IS_E_PORT((unit), (port))                  \
            && !IS_HG_PORT((unit), (port))              \
            && !IS_SPI_SUBPORT_PORT((unit), (port)))) { \
        return BCM_E_PORT;                              \
    }

/*
 * Allocation bitmap macros
 */
#define STG_BITMAP_TST(si, stg)     SHR_BITGET(si->stg_bitmap, stg)
#define STG_BITMAP_SET(si, stg)     SHR_BITSET(si->stg_bitmap, stg)
#define STG_BITMAP_CLR(si, stg)     SHR_BITCLR(si->stg_bitmap, stg)

#define BCM_PETRA_STG_MIN           (BCM_DPP_STG_FROM_TOPOLOGY_ID(SOC_PB_PP_STP_TOPOLOGY_MIN))


static bcm_vlan_t _stg_vlan_min     = BCM_VLAN_MIN + 1;
static bcm_vlan_t _stg_vlan_max     = BCM_VLAN_MAX;
static bcm_stg_t  _stg_default      = BCM_STG_DEFAULT;
static bcm_stg_t  _stg_min          = BCM_PETRA_STG_MIN;


void
_bcm_petra_stg_sw_dump(int unit)
{
    bcm_stg_info_t *si;
    bcm_stg_t       stg;
    bcm_vlan_t      vid;
    int             cnt, num_display_vids;

    num_display_vids = 8;

    si = &STG_CNTL(unit);
    if (FALSE == si->init) {
        LOG_CLI((BSL_META_U(unit,
                            "Unit %d STG not initialized\n"), unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "stg_min=%d stg_max=%d stg_default=%d allocated STGs=%d\n"),
             si->stg_min, si->stg_max, si->stg_defl, si->stg_count));
    LOG_CLI((BSL_META_U(unit,
                        "STG list:\nSTG :  VID list\n")));

    for (stg = si->stg_min; stg <= si->stg_max; stg++) {

        if (STG_BITMAP_TST(si, stg)) {
            LOG_CLI((BSL_META_U(unit,
                                "%4d: "), stg));

            cnt = 0;
            for (vid = si->vlan_first[stg];
                 vid != BCM_VLAN_NONE;
                 vid = si->vlan_next[vid]) {
                
                if (cnt < num_display_vids) {
                    if (cnt==0) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%d"), vid));
                    } else {
                        LOG_CLI((BSL_META_U(unit,
                                            ", %d"), vid));
                    }
                }

                cnt++;
            }

            if (cnt > num_display_vids) {
                LOG_INFO(BSL_LS_BCM_STG,
                         (BSL_META_U(unit,
                                     "), ... %d more"), cnt - num_display_vids));
            }

            LOG_INFO(BSL_LS_BCM_STG,
                     (BSL_META_U(unit,
                                 "\n")));
        }
    }
    LOG_INFO(BSL_LS_BCM_STG,
             (BSL_META_U(unit,
                         "\n")));
    
}


STATIC int
_bcm_petra_stg_map_store(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;
    /* Same logic as in vlan module */
    rv = _bcm_petra_vlan_stg_set(unit, vid, stg);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_stg_map_add
 * Purpose:
 *      Add VLAN to STG linked list
 */
STATIC void
_bcm_petra_stg_map_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t *si;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

    assert(BCM_VLAN_NONE != vid);

    si = &STG_CNTL(unit);

    si->vlan_next[vid] = si->vlan_first[stg];
    si->vlan_first[stg] = vid;
#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dpp_wb_stg_update_vlan_f_state(unit, stg);
    _bcm_dpp_wb_stg_update_vlan_n_state(unit, vid);
#endif

    _bcm_petra_stg_map_store(unit, stg, vid);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit\n"), FUNCTION_NAME(), unit, stg, vid));
}

/*
 * Function:
 *      _bcm_petra_stg_map_delete
 * Purpose:
 *      Remove VLAN from STG linked list. No action if VLAN is not in list.
 */
STATIC void
_bcm_petra_stg_map_delete(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t *si;
    bcm_vlan_t     *vp;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

    assert(BCM_VLAN_NONE != vid);

    si = &STG_CNTL(unit);

    vp = &si->vlan_first[stg];

    while (BCM_VLAN_NONE != *vp) {
        if (*vp == vid) {
            *vp = si->vlan_next[*vp];
        } else {
            vp = &si->vlan_next[*vp];
        }
    }
    
    _bcm_petra_stg_map_store(unit, stg, vid);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit\n"), FUNCTION_NAME(), unit, stg, vid));
}

/*
 * Function:
 *      _bcm_petra_stg_map_get
 * Purpose:
 *      Get STG that a VLAN is mapped to.
 * Parameters:
 *      unit - device unit number.
 *      vid  - VLAN id to search for
 *      *stg - Spanning tree group id if found
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_petra_stg_map_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg)
{
    bcm_stg_info_t *si;
    int             result = BCM_E_NOT_FOUND;
    int             index;
    bcm_vlan_t      vlan;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, *) - Enter\n"), FUNCTION_NAME(), unit, vid));

    /* this "internal" function is public so check parms */
    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);

    assert(BCM_VLAN_NONE != vid);
    *stg = 0;

    for (index = si->stg_min; index <= si->stg_max; index++) {
        vlan = si->vlan_first[index];

        while (BCM_VLAN_NONE != vlan) {
            if (vlan  == vid) {
                /* since a vlan may exist in only one STG, safe to exit */
                *stg = index;
                result = BCM_E_NONE;
                break;
            }

            vlan = si->vlan_next[vlan];
        }

        if (BCM_E_NONE == result) {
            break;
        }
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, vid, *stg, bcm_errmsg(result)));

    if (result != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(result);
    } else {
        BCM_RETURN_VAL_EXIT(result);
    }
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      _bcm_petra_stg_vid_compar
 * Purpose:
 *      Compare routine for sorting on VLAN ID.
 */
STATIC int
_bcm_petra_stg_vid_compare(void *a, void *b)
{
    uint16 a16, b16;

    a16 = *(uint16 *)a;
    b16 = *(uint16 *)b;

    return a16 - b16;
}

/*
 * Function:
 *      _bcm_ppd_stg_stp_port_set
 * Purpose:
 *      Set the spanning tree state for a port in specified STP topology id.
 * Parameters:
 *      unit      - device unit number.
 *      vid       - VLAN id.
 *      port      - device port number.
 *      stp_state - Port STP state.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_ppd_stg_stp_port_set(int unit, bcm_stg_t stg_id, bcm_port_t port, int stp_state)
{
    SOC_PPD_PORT_STP_STATE
      port_stp_state;
    uint32
      soc_sand_rv,
      stp_topology_id;
    SOC_PPD_PORT
      soc_ppd_port;
    int core;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    switch(stp_state) {
    case BCM_STG_STP_BLOCK:
    case BCM_STG_STP_LISTEN:
        port_stp_state = SOC_PPD_PORT_STP_STATE_BLOCK;
        break;
    case BCM_STG_STP_LEARN:
        port_stp_state = SOC_PPD_PORT_STP_STATE_LEARN;
        break;
    case BCM_STG_STP_DISABLE: /* STP is disabled for this port - all traffic should go through */
    case BCM_STG_STP_FORWARD:
        port_stp_state = SOC_PPD_PORT_STP_STATE_FORWARD;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("stp_state is invalid")));
    }

    stp_topology_id = BCM_DPP_STG_TO_TOPOLOGY_ID(stg_id);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

    soc_sand_rv = soc_ppd_port_stp_state_set(unit, soc_ppd_port, stp_topology_id, port_stp_state);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_ppd_stg_stp_port_get(int unit, bcm_stg_t stg_id, bcm_port_t port, int *stp_state)
{
    SOC_PPD_PORT_STP_STATE
      port_stp_state;
    uint32
      soc_sand_rv,
      stp_topology_id;
    
    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);
    
    stp_topology_id = BCM_DPP_STG_TO_TOPOLOGY_ID(stg_id);

    soc_sand_rv = soc_ppd_port_stp_state_get(unit, port, stp_topology_id, &port_stp_state);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch(port_stp_state) {
    case SOC_PPD_PORT_STP_STATE_BLOCK:
        *stp_state = BCM_STG_STP_BLOCK;
        break;
    case SOC_PPD_PORT_STP_STATE_LEARN:
        *stp_state = BCM_STG_STP_LEARN;
        break;
    case SOC_PPD_PORT_STP_STATE_FORWARD:
        *stp_state = BCM_STG_STP_FORWARD;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("port stp_state is invalid")));
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_stg_stp_get
 * Purpose:
 *      Retrieve the spanning tree state for a port in specified STG.
 * Parameters:
 *      unit      - device unit number.
 *      stg       - Spanning tree group id.
 *      port      - device port number.
 *      stp_state - (OUT)Port STP state int the group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_petra_stg_stp_get(int unit, bcm_stg_t stg, bcm_port_t port, int *stp_state)
{
    bcm_stg_info_t *si;
    int             state = 0;
    
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, *) - Enter\n"), FUNCTION_NAME(), unit, stg, port));

    /* Input parameters check. */
    STG_CHECK_PORT(unit, port);
    si = &STG_CNTL(unit);

    if (BCM_PBMP_MEMBER(si->stg_enable[stg], port)) {
        if (BCM_PBMP_MEMBER(si->stg_state_h[stg], port)) {
            state |= 0x2;
        }
        if (BCM_PBMP_MEMBER(si->stg_state_l[stg], port)) {
            state |= 0x1;
        }
        switch (state) {
        case 0:  *stp_state = BCM_STG_STP_BLOCK;
            break;
        case 1:  *stp_state = BCM_STG_STP_LISTEN;
            break;
        case 2:  *stp_state = BCM_STG_STP_LEARN;
            break;
        case 3:  *stp_state = BCM_STG_STP_FORWARD;
            break;
        /* must default. Otherwise compilation error */
        /* coverity[dead_error_begin:FALSE] */
        default: *stp_state = BCM_STG_STP_DISABLE;
        }
    } else {
        *stp_state = BCM_STG_STP_DISABLE;
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, port, *stp_state, bcm_errmsg(BCM_E_NONE)));
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_stg_stp_set
 * Purpose:
 *      Set the spanning tree state for a port in specified STG.
 * Parameters:
 *      unit      - device unit number.
 *      stg       - Spanning tree group id.
 *      port      - device port number.
 *      stp_state - (OUT)Port STP state int the group.
 * Returns:
 *      BCM_E_XXX
 */


#define _STG_VLAN_COUNT BCM_VLAN_COUNT

STATIC int
_bcm_petra_stg_stp_set(int unit, bcm_stg_t stg, bcm_port_t port, int stp_state)
{
    bcm_stg_info_t *si;
    int             result = BCM_E_NOT_FOUND; /* local result code */
    
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, port, stp_state));

    /* Input parameters check. */
    STG_CHECK_PORT(unit, port);

    si = &STG_CNTL(unit);

    /* Update port with new STG */
    result = _bcm_ppd_stg_stp_port_set(unit, stg, port, stp_state);

    if (BCM_E_NONE == result) {
        if (stp_state == BCM_STG_STP_DISABLE) {
            BCM_PBMP_PORT_REMOVE(si->stg_enable[stg], port);

        } else {
            BCM_PBMP_PORT_ADD(si->stg_enable[stg], port);
            
            if ((stp_state == BCM_STG_STP_LEARN) ||
                (stp_state == BCM_STG_STP_FORWARD)) {
                BCM_PBMP_PORT_ADD(si->stg_state_h[stg], port);
            } else {
                BCM_PBMP_PORT_REMOVE(si->stg_state_h[stg], port);
            }
            if ((stp_state == BCM_STG_STP_LISTEN) ||
                (stp_state == BCM_STG_STP_FORWARD)) {
                BCM_PBMP_PORT_ADD(si->stg_state_l[stg], port);
            } else {
                BCM_PBMP_PORT_REMOVE(si->stg_state_l[stg], port);
            }
        }
#ifdef BCM_WARM_BOOT_SUPPORT
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_stg_update_enable_state(unit, stg));
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_stg_update_state_h_state(unit, stg));
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_stg_update_state_l_state(unit, stg));
#endif
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, port, stp_state, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_stg_vlan_add
 * Purpose:
 *      Main part of bcm_petra_stg_vlan_add; assumes locks already acquired.
 * Parameters:
 *      unit    - device unit number.
 *      stg     - spanning tree group id.
 *      vid     - vlan to add
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The vlan is removed from it's current STG if necessary.
 */
STATIC int
_bcm_petra_stg_vlan_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t *si;
    bcm_stg_t       stg_cur;
    int             result = BCM_E_NOT_FOUND; /* local result code */
    int exists;
    
    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

    si = &STG_CNTL(unit); 
    
    exists = (bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vid) == BCM_E_EXISTS);   
    if (!exists) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("vid is not exist")));
    }

    if ((_stg_vlan_min > vid) || (_stg_vlan_max < vid)) {
        result = BCM_E_PARAM;    

    } else if (STG_BITMAP_TST(si, stg)) { /* STG must already exist */

        /* Get the STG the VLAN is currently associated to */
        result = _bcm_petra_stg_map_get(unit, vid, &stg_cur);
        if ((BCM_E_NONE == result) || (BCM_E_NOT_FOUND == result)) {

            /* iff found, delete it */
            if (BCM_E_NONE == result) {
                _bcm_petra_stg_map_delete(unit, stg_cur, vid);
            }
        }

        /* Set the new STG */
        _bcm_petra_stg_map_add(unit, stg, vid);

        result = BCM_E_NONE;
    } else {
        /* Not found in the specified STG */
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not found in the specified STG")));
	}

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, vid, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_stg_vlan_remove
 * Purpose:
 *      Main part of bcm_petra_stg_vlan_remove; assumes lock already acquired.
 * Parameters:
 *      unit    - device unit number.
 *      stg     - spanning tree group id.
 *      vid     - vlan id to remove
 *      destroy - boolean flag indicating the VLAN is being destroyed and is
 *                not to be added to the default STG. Also used internally
 *                to supress default STG assignment during transition.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_petra_stg_vlan_remove(int unit, bcm_stg_t stg, bcm_vlan_t vid, int destroy)
{
    bcm_stg_info_t *si;
    int             stg_cur;
    int             result = BCM_E_NOT_FOUND; /* local result code */

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid, destroy));

    si = &STG_CNTL(unit);

    /* STG must already exist */
    if (STG_BITMAP_TST(si, stg)) {

        /* Get the STG the VLAN is currently associated to */
        result = _bcm_petra_stg_map_get(unit, vid, &stg_cur);
        if ((BCM_E_NONE == result) && ((stg == stg_cur))) {
            _bcm_petra_stg_map_delete(unit, stg, vid);

            /* If the VLAN is not being destroyed, set the VLAN to the default STG */
            if (FALSE == destroy) {
                _bcm_petra_stg_map_add(unit, si->stg_defl, vid);
            }
        }
        else if (BCM_E_NONE == result && (stg_cur == si->stg_defl)) {
            /* Not found in the specified STG */
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not found in the specified STG")));
        } else {
            BCMDNX_IF_ERR_EXIT(result);
            BCM_EXIT;
        }
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, vid, destroy, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_stg_vlan_destroy
 * Purpose:
 *      Remove a VLAN from a spanning tree group.
 *      The VLAN is NOT placed in the default spanning tree group.
 *      Intended for use when the VLAN is destroyed.
 * Parameters:
 *      unit - device unit number
 *          stg - STG ID to use
 *      vid - VLAN id to be removed from STG
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_petra_stg_vlan_destroy(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t      *si = &stg_info[unit];
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    STG_CHECK_INIT(unit);
    STG_CHECK_STG(si, stg);
    BCM_DPP_VLAN_CHK_ID(unit,vid);

    STG_DB_LOCK(unit);

    rv = _bcm_petra_stg_vlan_remove(unit, stg, vid, TRUE);

    STG_DB_UNLOCK(unit);

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_stg_vlan_port_add
 * Purpose:
 *      Callout for vlan code to get a port that has been added to a vlan into
 *      the proper STP state
 * Parameters:
 *      unit      - device unit number.
 *      vid       - VLAN id.
 *      port      - device port number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_petra_stg_vlan_port_add(int unit, bcm_vlan_t vid, bcm_port_t port)
{
    int         result = BCM_E_FAIL; /* local result code */
    bcm_stg_t   stg;
    int         state;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, vid, port));

    result = _bcm_petra_stg_map_get(unit, vid, &stg);

    if (BCM_E_NONE == result) {
        result = _bcm_petra_stg_stp_get(unit, stg, port, &state);
    }

    if (BCM_E_NONE == result) {
        result = _bcm_ppd_stg_stp_port_set(unit, stg, port, state);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, vid, port, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_stg_vlan_port_remove
 * Purpose:
 *      Callout for vlan code to get a port that has been removed from a vlan
 *      into the proper STP state
 * Parameters:
 *      unit      - device unit number.
 *      vid       - VLAN id.
 *      port      - device port number.
 * Returns:
 *      BCM_E_NONE - This function is best effort
 */
int
_bcm_petra_stg_vlan_port_remove(int unit, bcm_vlan_t vid, bcm_port_t port)
{
    int         result = BCM_E_FAIL; /* local result code */
    bcm_stg_t   stg;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, vid, port));

    /* Get STG ID from vid */
    result = _bcm_petra_stg_map_get(unit, vid, &stg);

    if (BCM_E_NONE == result) {
        /* Remove Port from STG ID*/
        (void)_bcm_ppd_stg_stp_port_set(unit, stg, port, BCM_STG_STP_BLOCK);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, vid, port, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_stg_stp_init
 * Purpose:
 *      Initialize spanning tree group on a single device.
 * Parameters:
 *      unit - SOC unit number.
 *      stg  - Spanning tree group id.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_petra_stg_stp_init(int unit, bcm_stg_t stg)
{
    bcm_stg_info_t *si;
    bcm_port_t      port;
    pbmp_t          ports_map;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));

    si = &STG_CNTL(unit);

    BCM_PBMP_ASSIGN(ports_map, PBMP_ALL(unit));
    BCM_PBMP_REMOVE(ports_map, PBMP_SFI_ALL(unit));
    BCM_PBMP_ITER(ports_map, port) {
        /* place port in forward */
        BCM_PBMP_PORT_ADD(si->stg_enable[stg], port);

        BCM_PBMP_PORT_ADD(si->stg_state_h[stg], port);
        BCM_PBMP_PORT_ADD(si->stg_state_l[stg], port);      
#ifdef BCM_WARM_BOOT_SUPPORT
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_stg_update_bitmap_state(unit, stg));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_stg_update_state_h_state(unit, stg));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_stg_update_state_l_state(unit, stg));
#endif
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_list
 * Purpose:
 *      Return a list of VLANs in a specified Spanning Tree Group (STG).
 * Parameters:
 *      unit  - device unit number
 *      stg   - STG ID to list
 *      list  - Place where pointer to return array will be stored.
 *              Will be NULL if there are zero VLANs returned.
 *      count - Place where number of entries in array will be stored.
 *              Will be 0 if there are zero VLANs associated to the STG.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the Spanning Tree Group (STG) requested is not defined in the
 *      system, call returns NULL list and count of zero (0). The caller is
 *      responsible for freeing the memory that is returned, using
 *      bcm_petra_stg_vlan_list_destroy().
 */
int
bcm_petra_stg_vlan_list(int unit, bcm_stg_t stg, bcm_vlan_t **list,
                         int *count)
{
    bcm_stg_info_t *si;
    bcm_vlan_t      vlan;
    int             index;
    int             result = BCM_E_NOT_FOUND;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, *, *) - Enter\n"), FUNCTION_NAME(), unit, stg));
    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);
    STG_CHECK_STG(si, stg);

    *list = NULL;
    *count = 0;

    STG_DB_LOCK(unit);

    if (STG_BITMAP_TST(si, stg)) {
        /* Traverse list once just to get an allocation count */
        vlan = si->vlan_first[stg];

        while (BCM_VLAN_NONE != vlan) {
            (*count)++;
            vlan = si->vlan_next[vlan];
        }

        if (0 == *count) {
            result = BCM_E_NONE;
        }
        else {
            BCMDNX_ALLOC(*list, *count * sizeof (bcm_vlan_t), "bcm_petra_stg_vlan_list");

            if (NULL == *list) {
                result = BCM_E_MEMORY;
            }
            else {
                /* Traverse list a second time to record the VLANs */
                vlan = si->vlan_first[stg];
                index = 0;

                while (BCM_VLAN_NONE != vlan) {
                    (*list)[index++] = vlan;
                    vlan = si->vlan_next[vlan];
                }

                /* Sort the vlan list */
                _shr_sort(*list, *count, sizeof (bcm_vlan_t), _bcm_petra_stg_vid_compare);

                result = BCM_E_NONE;
            }
        }

    }
    STG_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, *, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, *count, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_list_destroy
 * Purpose:
 *      Destroy a list returned by bcm_petra_stg_vlan_list.
 * Parameters:
 *      unit  - device unit number
 *      list  - Pointer to VLAN array to be destroyed.
 *      count - Number of entries in array.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_vlan_list_destroy(int unit, bcm_vlan_t *list, int count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Enter\n"), FUNCTION_NAME(), unit, count));

    if (NULL != list) {
       BCM_FREE(list);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, count, bcm_errmsg(BCM_E_NONE)));
    BCM_EXIT;
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_default_get
 * Purpose:
 *      Returns the default STG for the device.
 * Parameters:
 *      unit    - device unit number.
 *      stg_ptr - STG ID for default.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_default_get(int unit, bcm_stg_t *stg_ptr)
{
    bcm_stg_info_t *si;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);

    *stg_ptr = si->stg_defl;

    STG_CHECK_STG(si, *stg_ptr);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, *stg_ptr, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_default_set
 * Purpose:
 *      Changes the default STG for the device.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *      stg  - STG ID to become default.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The specified STG must already exist.
 */
int
bcm_petra_stg_default_set(int unit, bcm_stg_t stg)
{
    bcm_stg_info_t *si;
    int             result = BCM_E_NOT_FOUND;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);
    STG_CHECK_STG(si, stg);

    STG_DB_LOCK(unit);

    if (STG_BITMAP_TST(si, stg)) {
        si->stg_defl = stg;
        result = BCM_E_NONE;
    }

    STG_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);

#ifdef BCM_WARM_BOOT_SUPPORT
    result = _bcm_dpp_wb_stg_update_defl_state(unit);
    BCMDNX_IF_ERR_EXIT(result);
#endif

exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_add
 * Purpose:
 *      Add a VLAN to a spanning tree group.
 * Parameters:
 *      unit - device unit number
 *      stg  - STG ID to use
 *      vid  - VLAN id to be added to STG
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Spanning tree group ID must have already been created. The
 *      VLAN is removed from the STG it is currently in.
 */
int
bcm_petra_stg_vlan_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t *si;
    int             result = BCM_E_BADID;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);
    STG_CHECK_STG(si, stg);

    if (BCM_VLAN_VALID(vid)) {

        STG_DB_LOCK(unit);
        result = _bcm_petra_stg_vlan_add(unit, stg, vid);
        STG_DB_UNLOCK(unit);

    } else {
        BCM_DPP_VLAN_CHK_ID(unit,vid);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, vid, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_remove
 * Purpose:
 *      Remove a VLAN from a spanning tree group. The VLAN is placed in the
 *      default spanning tree group.
 * Parameters:
 *      unit - device unit number
 *      stg  - STG ID to use
 *      vid  - VLAN id to be removed from STG
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_vlan_remove(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_info_t *si;
    int             result = BCM_E_BADID;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);
    STG_CHECK_STG(si, stg);

    if (BCM_VLAN_VALID(vid)) {

        STG_DB_LOCK(unit);
        result = _bcm_petra_stg_vlan_remove(unit, stg, vid, FALSE);
        STG_DB_UNLOCK(unit);

    } else {
        BCM_DPP_VLAN_CHK_ID(unit,vid);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, vid, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_remove_all
 * Purpose:
 *      Remove all VLANs from a spanning tree group. The VLANs are placed in
 *      the default spanning tree group.
 * Parameters:
 *      unit - device unit number
 *      stg  - STG ID to clear
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_vlan_remove_all(int unit, bcm_stg_t stg)
{
    bcm_stg_info_t *si;
    int             result = BCM_E_NONE;
    bcm_vlan_t      vid;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);
    STG_CHECK_STG(si, stg);

    STG_DB_LOCK(unit);

    if (stg != si->stg_defl) {

        if (!STG_BITMAP_TST(si, stg)) {   /* STG must already exist */
            result = BCM_E_NOT_FOUND;
        } else {
            vid = si->vlan_first[stg];
            while (BCM_VLAN_NONE != vid) {
                result = _bcm_petra_stg_vlan_remove(unit, stg, vid, FALSE);
                if (BCM_E_NONE != result) {
                    break;
                }

                result = _bcm_petra_stg_vlan_add(unit, si->stg_defl, vid);
                if (BCM_E_NONE != result) {
                    break;
                }

                /*
                 * Iterate through list. The vid just removed was popped from
                 * the list. Use the new first.
                 */
                vid = si->vlan_first[stg];
            }
        }
    }

    STG_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_clear
 * Description:
 *      Destroy all STGs
 * Parameters:
 *      unit - device unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_clear(int unit)
{
    bcm_stg_info_t *si;
    bcm_stg_t       stg;
    int             result = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);

    STG_DB_LOCK(unit);

    for (stg = si->stg_min; stg <= si->stg_max; stg++) {
        if (STG_BITMAP_TST(si, stg)) {
            STG_DB_UNLOCK(unit);

            /* ignore error code as unit will be 'init'ed later. */
            bcm_petra_stg_destroy(unit, stg);
            
            STG_DB_LOCK(unit);
        }
    }

    STG_DB_UNLOCK(unit);

    result = bcm_petra_stg_init(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_create_id
 * Description:
 *      Create a STG, using a specified ID.
 * Parameters:
 *      unit - Device unit number
 *      stg -  STG to create
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      In the new STG, all ports are in the DISABLED state.
 */
int
bcm_petra_stg_create_id(int unit, bcm_stg_t stg)
{
    bcm_stg_info_t *si;
    int             result  = BCM_E_EXISTS;
#ifdef BCM_WARM_BOOT_SUPPORT
    int             result2 = BCM_E_EXISTS;
#endif

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);
    STG_CHECK_STG(si, stg);

    STG_DB_LOCK(unit);

    if (!STG_BITMAP_TST(si, stg)) {
        /* No device action needed */
        result = _bcm_petra_stg_stp_init(unit, stg);
        if (BCM_E_NONE == result) {
            STG_BITMAP_SET(si, stg);
            si->stg_count++;
#ifdef BCM_WARM_BOOT_SUPPORT
            result2 = _bcm_dpp_wb_stg_update_count_state(unit);
            if (BCM_E_NONE != result2) {
                STG_DB_UNLOCK(unit);
                BCMDNX_IF_ERR_EXIT(result2);
            }
            result2 = _bcm_dpp_wb_stg_update_bitmap_state(unit, stg);
            if (BCM_E_NONE != result2) {
                STG_DB_UNLOCK(unit);
                BCMDNX_IF_ERR_EXIT(result2);
            }
#endif            
        }
    }

    STG_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_create
 * Description:
 *      Create a STG, picking an unused ID and returning it.
 * Parameters:
 *      unit - device unit number
 *      stg_ptr - (OUT) the STG ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_create(int unit, bcm_stg_t *stg_ptr)
{
    bcm_stg_info_t *si;
    bcm_stg_t       stg = BCM_STG_INVALID;
    int             result = BCM_E_FULL;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);

    STG_DB_LOCK(unit);

    for (stg = si->stg_min; stg <= si->stg_max; stg++) {
        if (!STG_BITMAP_TST(si, stg)) {
            break;          /* free id found */
        }
    }

    if (si->stg_max >= stg) {
        result = bcm_petra_stg_create_id(unit, stg);
    }  

    STG_DB_UNLOCK(unit);

    *stg_ptr = stg;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, *stg_ptr, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_destroy
 * Description:
 *      Destroy an STG.
 * Parameters:
 *      unit - device unit number.
 *      stg  - The STG ID to be destroyed.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The default STG may not be destroyed.
 */
int
bcm_petra_stg_destroy(int unit, bcm_stg_t stg)
{
    bcm_stg_info_t *si;
    int             result  = BCM_E_PARAM;    
#ifdef BCM_WARM_BOOT_SUPPORT
    int             result2 = BCM_E_PARAM;
#endif

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);
    STG_CHECK_STG(si, stg);

    STG_DB_LOCK(unit);

    if (si->stg_defl != stg) {
        /* The next call checks if STG exists as well as removing all VLANs */
        result = bcm_petra_stg_vlan_remove_all(unit, stg);

        if (BCM_E_NONE == result) {
            STG_BITMAP_CLR(si, stg);
            si->stg_count--;
#ifdef BCM_WARM_BOOT_SUPPORT
            result2 = _bcm_dpp_wb_stg_update_count_state(unit);
            if (BCM_E_NONE != result2) {
                STG_DB_UNLOCK(unit);
                BCMDNX_IF_ERR_EXIT(result2);
            }
            result2 = _bcm_dpp_wb_stg_update_bitmap_state(unit, stg);
            if (BCM_E_NONE != result2) {
                STG_DB_UNLOCK(unit);
                BCMDNX_IF_ERR_EXIT(result2);
            }
#endif
        }
    } else {
        result = BCM_E_NOT_FOUND;
    }

    STG_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_list
 * Purpose:
 *      Return a list of defined Spanning Tree Groups
 * Parameters:
 *      unit  - device unit number
 *      list  - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero STGs returned.
 *      count - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero STGs returned.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The caller is responsible for freeing the memory that is returned,
 *      using bcm_petra_stg_list_destroy().
 */
int
bcm_petra_stg_list(int unit, bcm_stg_t **list, int *count)
{
    bcm_stg_info_t *si;
    bcm_stg_t       stg;
    int             index = 0;
    int             result = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, *) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);

    STG_DB_LOCK(unit);

    *list = NULL;
    if (0 == si->stg_count) {
        *count = 0;
    } else {
        *count = si->stg_count;
        BCMDNX_ALLOC(*list, si->stg_count * sizeof (bcm_stg_t), "bcm_petra_stg_list");

        if (NULL == *list) {
            result = BCM_E_MEMORY;
        } else {
            for (stg = si->stg_min; stg <= si->stg_max; stg++) {
                if (STG_BITMAP_TST(si, stg)) {
                    assert(index < *count);
                    (*list)[index++] = stg;
                }
            }
        }
    }

    STG_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, *count, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_list_destroy
 * Purpose:
 *      Destroy a list returned by bcm_petra_stg_list.
 * Parameters:
 *      unit  - device unit number
 *      list  - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero STGs returned.
 *      count - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero STGs returned.
 * Returns:
 *      BCM_E_NONE
 */
int
bcm_petra_stg_list_destroy(int unit, bcm_stg_t *list, int count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Enter\n"), FUNCTION_NAME(), unit, count));

    if (NULL != list) {
       BCM_FREE(list);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, count, bcm_errmsg(BCM_E_NONE)));
    BCM_EXIT;
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_stp_set
 * Purpose:
 *      Set the Spanning tree state for a port in specified STG.
 * Parameters:
 *      unit      - device unit number.
 *      stg       - STG ID.
 *      port      - device port number.
 *      stp_state - Spanning Tree State of port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_stp_set(int unit, bcm_stg_t stg, bcm_port_t port, int stp_state)
{
    bcm_stg_info_t *si;
    int             result = BCM_E_NOT_FOUND;
    bcm_port_t local_port;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, port, stp_state));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);
    STG_CHECK_STG(si, stg);

        
    result = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCM_IF_ERROR_RETURN(result);

    PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
        STG_CHECK_PORT(unit, local_port);

        if ((stp_state < BCM_STG_STP_DISABLE) || (stp_state > BCM_STG_STP_FORWARD))
            return BCM_E_PARAM;

        STG_DB_LOCK(unit);

        if (STG_BITMAP_TST(si, stg)) {
            result = _bcm_petra_stg_stp_set(unit, stg, local_port, stp_state);
            if (result != BCM_E_NONE) {
                STG_DB_UNLOCK(unit);
                BCM_IF_ERROR_RETURN(result);
            }
        } else {
            /* Not found in the specified STG */
            STG_DB_UNLOCK(unit);
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not found in the specified STG")));
        }

        STG_DB_UNLOCK(unit);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, port, stp_state, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_stp_get
 * Purpose:
 *      Get the Spanning tree state for a port in specified STG.
 * Parameters:
 *      unit      - device unit number.
 *      stg       - STG ID.
 *      port      - device port number.
 *      stp_state - (Out) Pointer to where Spanning Tree State is stored.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */
int
bcm_petra_stg_stp_get(int unit, bcm_stg_t stg, bcm_port_t port, int *stp_state)
{
    bcm_stg_info_t *si;
    int             result = BCM_E_NOT_FOUND;
    bcm_port_t local_port;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, *) - Enter\n"), FUNCTION_NAME(), unit, stg, port));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);
    STG_CHECK_STG(si, stg);

    if (BCM_GPORT_IS_SET(port)) {
        result = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
        BCM_IF_ERROR_RETURN(result);

        local_port = gport_info.local_port;
    } else {
        local_port = port;
    }

    STG_CHECK_PORT(unit, local_port);

    STG_DB_LOCK(unit);

    if (STG_BITMAP_TST(si, stg)) {
        result = _bcm_petra_stg_stp_get(unit, stg, local_port, stp_state);
        if (result != BCM_E_NONE) {
            STG_DB_UNLOCK(unit);
            BCM_IF_ERROR_RETURN(result);
        }
    } else {
        /* Not found in the specified STG */
        STG_DB_UNLOCK(unit);    
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not found in the specified STG")));
    }

    STG_DB_UNLOCK(unit);    

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, local_port, *stp_state, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_petra_stg_count_get
 * Purpose:
 *     Get the maximum number of STG groups the device supports
 * Parameters:
 *     unit    - device unit number.
 *     max_stg - max number of STG groups supported by this unit
 * Returns:
 *     BCM_E_xxx
 */
int
bcm_petra_stg_count_get(int unit, int *max_stg)
{
    bcm_stg_info_t *si;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);
    si = &STG_CNTL(unit);

    *max_stg = si->stg_max - si->stg_min + 1;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, *max_stg, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_stg_detach(int unit)
{
    bcm_stg_info_t     *si;
#ifdef BCM_WARM_BOOT_SUPPORT
    int result = BCM_E_NONE;
#endif
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));

    si = &STG_CNTL(unit);

    STG_DB_LOCK(unit); 
    if (si->stg_bitmap) {
       BCM_FREE(si->stg_bitmap);
        si->stg_bitmap = NULL;
    }

    if (si->stg_enable) {
       BCM_FREE(si->stg_enable);
        si->stg_enable = NULL;
    } 

    if (si->stg_state_h) {
       BCM_FREE(si->stg_state_h);
        si->stg_state_h = NULL;
    }

    if (si->stg_state_l) {
       BCM_FREE(si->stg_state_l);
        si->stg_state_l = NULL;
    }

    if (si->vlan_first) {
       BCM_FREE(si->vlan_first); 
        si->vlan_first = NULL;
    }

    if (si->vlan_next) {
       BCM_FREE(si->vlan_next);
        si->vlan_next = NULL;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    result = _bcm_dpp_wb_stg_state_deinit(unit);
    if (result != BCM_E_NONE) {
      STG_DB_UNLOCK(unit);
      BCMDNX_IF_ERR_EXIT(result);
    }
#endif

    STG_DB_UNLOCK(unit);

    BCM_EXIT;
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_stg_init(int unit)
{
    SOC_PPD_TRAP_CODE trap_code;
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO trap_info;
    uint32 soc_sand_rv = 0;

    BCMDNX_INIT_FUNC_DEFS;
    /* init drop trap to drop packets*/

    /* for block state */
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&trap_info);
    trap_code = SOC_PPD_TRAP_CODE_STP_STATE_BLOCK;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_code,&trap_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trap_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    trap_info.processing_info.enable_learning = FALSE;
    trap_info.dest_info.frwrd_dest.dest_id = 0;
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    /* for learn state */
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&trap_info);
    trap_code = SOC_PPD_TRAP_CODE_STP_STATE_LEARN;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_code,&trap_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trap_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    trap_info.processing_info.enable_learning = TRUE;
    trap_info.dest_info.frwrd_dest.dest_id = 0;
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_stg_init
 * Description:
 *      Initialize the STG module according to Initial Configuration.
 * Parameters:
 *      unit - Device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_init(int unit)
{
    bcm_stg_info_t     *si;
    int                 sz_bmp, sz_e, sz_sh, sz_sl, sz_vfirst, sz_vnext;
    bcm_port_t          port;
    int                 result = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));

    SOC_DPP_STATE(unit)->stg_info = si = &STG_CNTL(unit);

    if (NULL == si->lock) {
        if (NULL == (si->lock = sal_mutex_create("soc_petra_stg_lock"))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("memory allocation failed")));
        }
    }

    /* Set the device properties */
    si->stg_count = 0;                  /* no STGs currently defined */
    si->stg_min   = _stg_min;
    si->stg_max   = SOC_DPP_DEFS_GET(unit, nof_topology_ids);;
    si->stg_defl  = _stg_default;       /* The default STG is always 1 */

    assert(si->stg_defl >= si->stg_min && si->stg_defl <= si->stg_max &&
           si->stg_min <= si->stg_max);

    /* alloc memory and/or clear */
    sz_bmp = SHR_BITALLOCSIZE(si->stg_max+1);
    if (NULL == si->stg_bitmap) {
        BCMDNX_ALLOC(si->stg_bitmap, sz_bmp, "STG-bitmap");
    }

    /* array of port bitmaps indicating whether the port+stg has STP enabled,
     * 0 = BCM_STG_STP_DISABLE, 1= Enabled.
     */
    sz_sh = sz_sl = sz_e = sizeof(bcm_pbmp_t) * (si->stg_max + 1);
    if (NULL == si->stg_enable) {
        BCMDNX_ALLOC(si->stg_enable, sz_e, "STG-enable");
    }

    /* array of port bitmaps indicating STP state for the port+stg combo.
     * Only valid if stg_enable = TRUE. Use high (_h) and low (_l) for
     * each port to represent one of four states in bcm_stg_stp_t, i.e
     * BLOCK(_h,_l = 0,0), LISTEN(0,1), LEARN(1,0), FORWARD(1,1).
     */
    if (NULL == si->stg_state_h) {
        BCMDNX_ALLOC(si->stg_state_h, sz_sh, "STG-state_h");
    }

    if (NULL == si->stg_state_l) {
        BCMDNX_ALLOC(si->stg_state_l, sz_sl, "STG-state_l");
    }

    sz_vfirst = (si->stg_max + 1) * sizeof (bcm_vlan_t);
    if (NULL == si->vlan_first) {
        BCMDNX_ALLOC(si->vlan_first, sz_vfirst, "STG-vfirst");
    }

    sz_vnext = BCM_VLAN_COUNT * sizeof (bcm_vlan_t);
    if (NULL == si->vlan_next) {
        BCMDNX_ALLOC(si->vlan_next, sz_vnext, "STG-vnext");
    }

    if (si->stg_bitmap == NULL || si->stg_enable == NULL || 
        si->stg_state_h == NULL || si->stg_state_l == NULL ||
        si->vlan_first == NULL || si->vlan_next == NULL) {

        result = bcm_petra_stg_detach(unit);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_STG,
                      (BSL_META_U(unit,
                                  "Failed to clean up data\n")));
        }
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("STG detach failed")));
    }

    sal_memset(si->stg_bitmap, 0, sz_bmp);
    sal_memset(si->stg_enable, 0, sz_e);
    sal_memset(si->stg_state_h, 0, sz_sh);
    sal_memset(si->stg_state_l, 0, sz_sl);
    sal_memset(si->vlan_first, 0, sz_vfirst);
    sal_memset(si->vlan_next, 0, sz_vnext);

    if (!SOC_WARM_BOOT(unit)) {
        /* Set trap configuration */
        result = _bcm_petra_stg_init(unit);
        BCMDNX_IF_ERR_EXIT(result);
    }

    /*
     * Create default STG and add all VLANs to it.  Use private calls.
     * This creates a slight maintenance issue but allows delayed setting
     * of the init flag. This will prevent any public API functions
     * from executing.
     */
     if (!SOC_WARM_BOOT(unit)) {
	    result = _bcm_petra_stg_stp_init(unit, si->stg_defl);
	    BCMDNX_IF_ERR_EXIT(result);

	    STG_BITMAP_SET(si, si->stg_defl);
	    si->stg_count++;


        _bcm_petra_stg_map_add(unit, si->stg_defl, BCM_VLAN_DEFAULT);

        
        PBMP_E_ITER(unit, port) {
            (void)_bcm_ppd_stg_stp_port_set(unit, si->stg_defl, port, BCM_STG_STP_FORWARD);
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    result = _bcm_dpp_wb_stg_state_init(unit);
    if (result != BCM_E_NONE) {
      BCMDNX_IF_ERR_EXIT(result);
    }
#endif

    si->init = TRUE;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "bcm_petra_stg_init: unit=%d rv=%d(%s)\n"),
                          unit, BCM_E_NONE, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


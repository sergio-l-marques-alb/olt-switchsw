/*
 * $Id: trunk.c,v 1.58 Broadcom SDK $
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
 * File:    trunk.c
 * Purpose: BCM level APIs for Link Aggregation (a.k.a Trunking)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TRUNK

#include <shared/bsl.h>

#include <soc/types.h>
#include <sal/appl/io.h>
#include <sal/core/alloc.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/PPD/ppd_api_lag.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>
#include <soc/dpp/PPD/ppd_api_slb.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/trunk.h>
#include <bcm/vlan.h>
#include <bcm/stack.h>
#include <bcm/pkt.h>
#include <bcm/cosq.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/common/trunk.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/trunk.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/wb_db_trunk.h>
#include <bcm_int/dpp/state.h>
#include <bcm_int/dpp/switch.h>

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_ports.h>
#endif


int _bcm_petra_gport_to_sys_port(int unit, bcm_gport_t gport, uint32 *sys_port);
#ifdef BCM_ARAD_SUPPORT
int soc_dpp_arad_trunk_groups_ports_num_get(int unit, int *ngroups, int *nports);
#endif

/*
 * Check if port is in LAG (this function return correct value only if port is local).
 */
int
_bcm_petra_trunk_member_in_other_trunk_check(int unit,  bcm_gport_t gport, bcm_trunk_t tid);

/*
 * One trunk control entry for each SOC device containing trunk book keeping
 * info for that device.
 */
trunk_state_t bcm_trunk_control[BCM_MAX_NUM_UNITS];

#define TRUNK_MIN_MEMBERS   0   /* Minimum number of ports in a trunk */

#define TRUNK_CNTL(unit)        bcm_trunk_control[unit]

#define _BCM_TRUNK_RESILIENT_MATCH_FLAGS   (BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID | BCM_TRUNK_RESILIENT_MATCH_HASH_KEY | BCM_TRUNK_RESILIENT_MATCH_MEMBER)
#define _BCM_TRUNK_RESILIENT_ACTION_FLAGS  (BCM_TRUNK_RESILIENT_REPLACE | BCM_TRUNK_RESILIENT_COUNT | BCM_TRUNK_RESILIENT_DELETE)

#define _DPP_TRUNK_TRUNK_ID_GET(unit, idx, trunk_id)                    \
    _rv = _bcm_dpp_wb_trunk_trunk_id_get(unit, &trunk_id, idx); \
    if (_rv != BCM_E_NONE) { \
        BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: get trunk_id failed, unit %d"),FUNCTION_NAME(), unit)); \
    }
   
#define _DPP_TRUNK_TRUNK_ID_SET(unit, idx, trunk_id)   \
      _rv = _bcm_dpp_wb_trunk_trunk_id_set(unit, trunk_id, idx); \
      if (_rv != BCM_E_NONE) { \
        BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: set trunk_id failed, unit %d"),FUNCTION_NAME(), unit)); \
    }

#define _DPP_TRUNK_IN_USE_GET(unit, idx, in_use)                    \
    _rv = _bcm_dpp_wb_trunk_in_use_get(unit, &in_use, idx); \
    if (_rv != BCM_E_NONE) { \
        BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: get in_use failed, unit %d"),FUNCTION_NAME(), unit)); \
    }
   
#define _DPP_TRUNK_IN_USE_SET(unit, idx, in_use)   \
      _rv = _bcm_dpp_wb_trunk_in_use_set(unit, in_use, idx); \
      if (_rv != BCM_E_NONE) { \
        BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: set in_use failed, unit %d"),FUNCTION_NAME(), unit)); \
    }


#define TRUNK_DB_LOCK(unit)                                                 \
        do {                                                                \
            if (NULL != TRUNK_CNTL(unit).lock)                              \
                sal_mutex_take(TRUNK_CNTL(unit).lock, sal_mutex_FOREVER);   \
        } while (0);

#define TRUNK_DB_UNLOCK(unit)                           \
        do {                                            \
            if (NULL != TRUNK_CNTL(unit).lock)          \
                sal_mutex_give(TRUNK_CNTL(unit).lock);  \
        } while (0);

/*
 * Cause a routine to return BCM_E_INIT if trunking subsystem is not
 * initialized to an acceptable initialization level (il_).
 */
#define TRUNK_CHECK_INIT(u_, il_)                                 \
    do {                                                          \
        BCM_DPP_UNIT_CHECK(u_);                                   \
        if (TRUNK_CNTL(u_).init_state < (il_)) return BCM_E_INIT; \
    } while (0);

/*
 * Make sure TID is within valid range.
 */
#define TRUNK_CHECK_TID(unit, tid) \
     if (((tid) < 0) || ((tid) >= (TRUNK_CNTL(unit).ngroups)))  \
        return BCM_E_BADID;

#define TRUNK_CHECK_STK_TID(unit, tid) \
     if (((tid) < 0) || ((tid) >= (TRUNK_CNTL(unit).ngroups + TRUNK_CNTL(unit).stk_ngroups)))  \
        return BCM_E_BADID;

/*
 * TID is in range, check to make sure it is actually in use.
 */
#define TRUNK_TID_VALID(trunk_id)                          \
    (trunk_id != BCM_TRUNK_INVALID)
    

#define TRUNK_PORTCNT_VALID(unit, port_cnt)                  \
    (((/* not really needed. just to aviod coverity defect */int)port_cnt >= TRUNK_MIN_MEMBERS) && (port_cnt <= TRUNK_CNTL(unit).nports))

/* 
 *  Trunk Staking TID=TM-domain
 */
#define TRUNK_TID_STACKING_TID_SET(unit, tid) ((tid) += TRUNK_CNTL(unit).ngroups + 1)
#define TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid) (stk_tid = tid - TRUNK_CNTL(unit).ngroups - 1)
#define TRUNK_TID_IS_STACKING_TID(unit, tid)  (((tid) > TRUNK_CNTL(unit).ngroups) && ((tid) < TRUNK_CNTL(unit).ngroups + TRUNK_CNTL(unit).stk_ngroups))
#define BCM_TRUNK_STACKING_TID_VALID(stk_tid)    ((stk_tid < 0) || (stk_tid >= BCM_DPP_MAX_STK_TRUNKS))
#define BCM_TRUNK_IS_STACKING_TID(tid)  (tid & (1 << BCM_TRUNK_STACKING_TID_BIT) ? 1 : 0)

static int        _ngroups          = BCM_DPP_MAX_TRUNKS; 
static int        _stk_ngroups      = BCM_DPP_MAX_STK_TRUNKS;
static int        _nports           = BCM_TRUNK_MAX_PORTCNT; 

int
_bcm_petra_trunk_psc_to_lb_type(int unit, int psc, SOC_PPD_LAG_LB_TYPE *lb_type)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (lb_type == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("_bcm_petra_trunk_psc_to_lb_type errm lb_type == NULL")));
    }

    switch (psc) {
    case BCM_TRUNK_PSC_ROUND_ROBIN:
        *lb_type = SOC_PPD_LAG_LB_TYPE_ROUND_ROBIN;
        break;
    case BCM_TRUNK_PSC_PORTFLOW:
        *lb_type = SOC_PPD_LAG_LB_TYPE_HASH;
        break;
    case BCM_TRUNK_PSC_SMOOTH_DIVISION:
        if (SOC_IS_JERICHO(unit)) {
            *lb_type = SOC_PPD_LAG_LB_TYPE_SMOOTH_DIVISION;
        } else {
            rv = BCM_E_PARAM;
        }
        break;
#ifdef BCM_88660_A0
    case BCM_TRUNK_PSC_DYNAMIC_RESILIENT:
        if (SOC_IS_ARADPLUS(unit) && (soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0) == 1)) {
            *lb_type = SOC_PPD_LAG_LB_TYPE_HASH;
        } else {
            rv = BCM_E_PARAM;
        }
        break;
#endif /* BCM_88660_A0 */
    default:
        rv = BCM_E_PARAM;
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_trunk_lb_type_to_psc(int unit, SOC_PPD_LAG_LB_TYPE lb_type, int *psc)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (psc == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("_bcm_petra_trunk_lb_type_to_psc err,  psc == NULL")));
    }

    switch (lb_type) {
    case SOC_PPD_LAG_LB_TYPE_ROUND_ROBIN:
        *psc = BCM_TRUNK_PSC_ROUND_ROBIN;
        break;
    case SOC_PPD_LAG_LB_TYPE_HASH:
        *psc = BCM_TRUNK_PSC_PORTFLOW;
        break;
    case SOC_PPD_LAG_LB_TYPE_SMOOTH_DIVISION:
        if (SOC_IS_JERICHO(unit)) {
            *psc = BCM_TRUNK_PSC_SMOOTH_DIVISION;
        } else {
            rv = BCM_E_PARAM;
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_trunk_stacking_tid_to_local_tid(int unit, bcm_trunk_t *tid)
{
    int rv = BCM_E_NONE;
    bcm_trunk_t stk_tid;

    BCMDNX_INIT_FUNC_DEFS;

    if (tid == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("_bcm_petra_trunk_stacking_tid_to_local_tid err, tid == NULL")));
    }

    if (BCM_TRUNK_IS_STACKING_TID(*tid)) {

        if (!SOC_IS_ARAD(unit)) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Arad Stacking. invalid Device")));
        }

        stk_tid = BCM_TRUNK_STACKING_TID_GET(*tid);
        if (BCM_TRUNK_STACKING_TID_VALID(stk_tid)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Stacking tid not in range.")));
        }
        TRUNK_TID_STACKING_TID_SET(unit, stk_tid);
        *tid = stk_tid;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_stacking_trunk_set(int unit,
                     bcm_trunk_t tid,
                     bcm_trunk_info_t *trunk_info,
                     int member_count,
                     bcm_trunk_member_t *member_array)
{
    int                 rv = BCM_E_NONE;
    int soc_sand_rc = SOC_SAND_OK;

    int                     index, entry;
    bcm_gport_t      gport, gport_modport = 0x0;
    uint8                 is_dest_valid = FALSE;
    uint32               dest_base_queue, local_port = 0;
    int                     system_port;
    bcm_trunk_t      stk_tid = 0x0;
    uint32                  tm_port;
    int core = SOC_CORE_ALL;
    uint8 is_sw_only = FALSE;
    BCMDNX_INIT_FUNC_DEFS;

    TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
 
    LOG_VERBOSE(BSL_LS_BCM_TRUNK,
                (BSL_META_U(unit,
                            "%s(),%d: adding Stacking Trunk  tid=%d(%d(."), FUNCTION_NAME(), unit, tid, stk_tid));

    if(member_count > SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Error: member_count=0x%x - maximum ports allowed in stacking trunk is 64"), member_count));
    }

    index = 0x0;
    for (entry = 0; entry < SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX; entry++) {

        rv = bcm_petra_stk_gport_sysport_get(unit, member_array[index].gport, &gport);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to get gport_system_port from gport=0x%x"), member_array[index].gport));
        }   

        system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);

        rv = bcm_petra_stk_sysport_gport_get(unit, gport, &gport_modport);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to get gport_modport from gport=0x%x"), gport));
        }
        local_port = BCM_GPORT_MODPORT_PORT_GET(gport_modport);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, local_port, &tm_port, &core)));


        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_stacking_info_set,(unit, core, tm_port, 0x1, stk_tid)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed setting Stacking info. local_port=%d, stk_tid=%d,"), local_port, stk_tid));
        }

        /* get stk port voq */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_destination_id_packets_base_queue_id_get,(unit, core, system_port, &is_dest_valid, &is_sw_only, &dest_base_queue)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("retrieving base queue for dest port(%d) failed, soc_sand error (0x%x),"), system_port, soc_sand_rc));
        }
        if (is_dest_valid == 0x0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("Failed to get queue for local_stk_sysport=%d"), system_port));
        }

        /* Write entries to Stack trunk resolve table  */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_stack_lag_packets_base_queue_id_set,(unit, stk_tid, entry, dest_base_queue)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed setting stack_lag_packets_base_queue_id. stk_tid=%d, entry=%d, dest_base_queue=%d"), stk_tid, entry, dest_base_queue));
        }

        index = (index + 1) % member_count;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    _bcm_petra_trunk_set
 * Purpose:
 *      Set ports in a trunk group.
 * Parameters:
 *      unit       - Device unit number.
 *      tid        - The trunk ID to be affected.
 *      t_add_info - Information on the trunk group.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_PARAM     - Invalid ports specified.
 *      BCM_E_XXXXX     - As set by lower layers of software
 * Notes:
 *      the following fields of the bcm_trunk_add_info_t structure are ignored
 *      on DPP: 
 *          flags
 *          ipmc_psc
 *          dlf_index
 *          mc_index
 *          ipmc_index
 *          member_flags
 *          dynamic_size
 *          dynamic_age
 */
STATIC int
_bcm_petra_trunk_set(int unit,
                     bcm_trunk_t tid,
                     bcm_trunk_info_t *trunk_info,
                     int member_count,
                     bcm_trunk_member_t *member_array)
{
    int                 index;
    int                 rv = BCM_E_NONE;
    bcm_gport_t         gport;
    SOC_PPD_LAG_INFO       *lag_info = NULL;
    SOC_PPD_LAG_LB_TYPE     lb_type = SOC_PPD_LAG_LB_TYPE_HASH;
#ifdef BCM_88660_A0
    uint8               is_stateful = 0;
#endif /* BCM_88660_A0 */

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *, [n=%d - "), FUNCTION_NAME(), unit, tid,
                          member_count));
    for (index = 0; index < member_count; index++) {
        LOG_DEBUG(BSL_LS_BCM_TRUNK,
                  (BSL_META_U(unit,
                              " %08X flags:%08X"), member_array[index].gport,
                              member_array[index].flags));
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "]) - Enter\n")));

    /* Validate PSC */
    if (trunk_info->psc <= 0) {
        /* Arad: default to Round Robin
           Jericho: error - invalid input */
        if (SOC_IS_JERICHO(unit)) {
            rv = BCM_E_PARAM;
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: trunk_info->psc is invalid (%d)"),
                                     FUNCTION_NAME(), trunk_info->psc));
        }

        lb_type = SOC_PPD_LAG_LB_TYPE_ROUND_ROBIN;

#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit)) {
          is_stateful = 0;
        }
#endif /* BCM_88660_A0 */
    } else {
        rv = _bcm_petra_trunk_psc_to_lb_type(unit, trunk_info->psc, &lb_type);
        if (BCM_FAILURE(rv)) {
          BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: Invalid trunk psc (%d) specified"),
                           FUNCTION_NAME(), trunk_info->psc));
        }

#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit)) {
          is_stateful = (trunk_info->psc == BCM_TRUNK_PSC_DYNAMIC_RESILIENT) ? 1 : 0;
        }
#endif /* BCM_88660_A0 */
    }

    /* check validation of the trunk members */

    if (TRUNK_TID_IS_STACKING_TID(unit, tid)) { 
        rv = _bcm_petra_stacking_trunk_set(unit, tid, trunk_info, member_count, member_array);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Stacking trunk Set Failed."), FUNCTION_NAME()));
        }
        TRUNK_CHECK_STK_TID(unit, tid);
    } 

    /* Make sure the ports/nodes supplied are valid. */
    BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "aggregate workspace");
    if (!lag_info) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: unable to allocate %u bytes workspace"),
                      FUNCTION_NAME(),
                      (uint32)sizeof(*lag_info)));
    }
    SOC_PPD_LAG_INFO_clear(lag_info);
    
    for (index = 0; (BCM_E_NONE == rv) && (index < member_count); index++) {

        gport = member_array[index].gport;

        rv = _bcm_petra_gport_to_sys_port(unit, gport, &lag_info->members[index].sys_port);
        if (BCM_FAILURE(rv)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("%s: Invalid trunk member (%d) specified"), FUNCTION_NAME(), index));
        }        
        lag_info->members[index].member_id = index;
        
        /* Set ingress disable flags */
        if((member_array[index].flags & BCM_TRUNK_MEMBER_INGRESS_DISABLE) != 0) {
            lag_info->members[index].flags = SOC_PPC_LAG_MEMBER_INGRESS_DISABLE;
        } 
        
        /* Check that ports of the lag are not memners of other LAG */
        rv = _bcm_petra_trunk_member_in_other_trunk_check(unit, gport, tid);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: port ox%x is other lag member"), FUNCTION_NAME(), lag_info->members[index].sys_port));
        }

    }

    /* for (all members as long as no error) */
    if (BCM_E_NONE == rv) {
        lag_info->nof_entries = member_count;
        lag_info->lb_type = lb_type;


#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit)) {
            lag_info->is_stateful = is_stateful;
        }
#endif /* BCM_88660_A0 */

        if (TRUNK_TID_IS_STACKING_TID(unit, tid)) { 

            rv = soc_ppd_lag_lb_key_range_set(unit, lag_info);
            rv = handle_sand_result(rv);

        } else {

            rv = soc_ppd_lag_set(unit, tid, lag_info);
            rv = handle_sand_result(rv);
        }
    } /* if (BCM_E_NONE == result) */
    
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *, %d, *) - Exit(%s)\n"),
                          FUNCTION_NAME(),
               unit,
               tid,
               member_count,
               bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lag_info);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_gport_to_sys_port(int unit, bcm_gport_t gport, uint32 *sys_port)
{
    int         rv = BCM_E_NONE;
    bcm_gport_t sys_gport;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(sys_port);
    *sys_port = BCM_GPORT_INVALID;

    if (BCM_GPORT_IS_MODPORT(gport) || BCM_GPORT_IS_LOCAL(gport)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_stk_gport_sysport_get(unit, gport, 
                                                      &sys_gport));
    } else if (BCM_GPORT_IS_SYSTEM_PORT(gport)) {
        sys_gport = gport;
    } else {
        rv = BCM_E_PARAM;
    }
    if (BCM_SUCCESS(rv)) {
        *sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(sys_gport);
    }

    if(*sys_port == BCM_GPORT_INVALID) {
        rv = BCM_E_PARAM; 
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_trunk_member_set(int unit, bcm_trunk_t tid,
                           int member_count, bcm_trunk_member_t *member_array)
{
    int             rv = BCM_E_NONE, index;
    uint32          sys_port;
    SOC_PPD_LAG_INFO   *lag_info = NULL;
    bcm_trunk_t     trunk_id;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (member_array == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_set err,  member_array == NULL")));
    }
    if ((member_count <= 0) || (member_count > BCM_TRUNK_MAX_PORTCNT)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_set err,(member_count <= 0) || (member_count > BCM_TRUNK_MAX_PORTCNT) ")));
    }

    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
       BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
       TRUNK_CHECK_STK_TID(unit, tid);
    }
    BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "aggregate workspace");
    if (!lag_info) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: unable to allocate %u bytes workspace"),
                      FUNCTION_NAME(),
                      (uint32)sizeof(*lag_info)));
    }
    TRUNK_DB_LOCK(unit);
    _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);
    if (TRUNK_TID_VALID(trunk_id)) {
        SOC_PPD_LAG_INFO_clear(lag_info);
        for (index = 0; index < member_count; index++) {
            rv = _bcm_petra_gport_to_sys_port(unit, member_array[index].gport,
                                              &sys_port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Invalid gport(%x) in member_array at "
                                      "index:%d \n"), FUNCTION_NAME(),
                           member_array[index].gport, index));
                break;
            } else {
                lag_info->members[index].sys_port = sys_port;
                lag_info->members[index].member_id = index;
            }
        }
        if (BCM_SUCCESS(rv)) {
            lag_info->nof_entries = member_count;
            lag_info->lb_type = SOC_PPD_LAG_LB_TYPE_HASH; /* default to hash */

            rv = soc_ppd_lag_set(unit, tid, lag_info);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Failed(%s) to set members on trunk:%d \n"),
                                      FUNCTION_NAME(), bcm_errmsg(rv), tid));
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    TRUNK_DB_UNLOCK(unit);
   BCM_FREE(lag_info);

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid,
                          bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_trunk_member_get(int unit, bcm_trunk_t tid, int member_max,
                           bcm_trunk_member_t *member_array, int *member_count)
{
    int             index;
    bcm_gport_t     gport;
    SOC_PPD_LAG_INFO   *lag_info = NULL;
    int             rv = BCM_E_NONE;
    bcm_trunk_t     trunk_id;

    bcm_trunk_t stk_tid;
    bcm_trunk_info_t trunk_info;

    BCMDNX_INIT_FUNC_DEFS;
    TRUNK_CHECK_INIT(unit, ts_init);
     if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }
    if (member_count == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_get err,  member_count == NULL")));
    }
    if ((member_max > 0) && (member_array == NULL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_get err, (member_max > 0) && (member_array == NULL)")));
    }

    if (TRUNK_TID_IS_STACKING_TID(unit, tid)) {

            TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
            BCM_TRUNK_STACKING_TID_SET(tid, stk_tid);
               
            sal_memset(&trunk_info, 0x0, sizeof(bcm_trunk_info_t));
            rv = bcm_petra_trunk_get(unit, tid, &trunk_info, BCM_DPP_MAX_STK_TRUNKS, member_array, member_count);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Get trunk. tid=0x%x."), FUNCTION_NAME(), tid));
            }
    } else {

        BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "aggregate workspace");
        if (!lag_info) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: unable to allocate %u bytes workspace"),
                          FUNCTION_NAME(),
                          (uint32)sizeof(*lag_info)));
        }
        TRUNK_DB_LOCK(unit);
        _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);
        if (TRUNK_TID_VALID(trunk_id)) {
            SOC_PPD_LAG_INFO_clear(lag_info);
            rv = soc_ppd_lag_get(unit, tid, lag_info);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Failed (%s) to read trunk (id:%d) info \n"),
                                      FUNCTION_NAME(), bcm_errmsg(rv), tid));
            } else {
                if (member_max > 0) {
                    for (index = 0;
                         ((index < lag_info->nof_entries) && (index < member_max));
                         index++) {


                        BCM_GPORT_SYSTEM_PORT_ID_SET(gport,
                                                     lag_info->members[index].sys_port);
                         member_array[index].gport = gport;
                         member_array[index].flags = 0;

                    }
                    *member_count = index;
                } else {
                    *member_count = lag_info->nof_entries;
                }
            }
        } else {
            rv = BCM_E_NOT_FOUND;
        }
        TRUNK_DB_UNLOCK(unit);
       BCM_FREE(lag_info);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_trunk_member_add(int unit, bcm_trunk_t tid,
                           bcm_trunk_member_t *member)
{
    int             rv = BCM_E_NONE;
    SOC_PPD_LAG_INFO   *lag_info = NULL;
    SOC_PPD_LAG_MEMBER  lag_member;
    uint32          soc_sand_rv, sys_port;
    bcm_trunk_t     trunk_id;

    bcm_trunk_t stk_tid;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t member_array[BCM_DPP_MAX_STK_TRUNKS];
    int member_count = 0,i;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    TRUNK_CHECK_INIT(unit, ts_init);
     if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }
    if (member == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_add err,  member == NULL")));
    }
    rv = _bcm_petra_gport_to_sys_port(unit, member->gport, &sys_port); 
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Invalid member port type (not modport | sysport) "
                       "specified."), FUNCTION_NAME()));
    }
        
    /* check if the port is al ready member of a trunk */
    rv = _bcm_petra_trunk_member_in_other_trunk_check(unit, member->gport, tid);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: error port 0x%x lag member"), FUNCTION_NAME(), sys_port));
    }

    if (TRUNK_TID_IS_STACKING_TID(unit, tid)) {

            TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
            BCM_TRUNK_STACKING_TID_SET(tid, stk_tid);
               
            sal_memset(&trunk_info, 0x0, sizeof(bcm_trunk_info_t));
            sal_memset(member_array, 0x0, BCM_DPP_MAX_STK_TRUNKS * sizeof(bcm_trunk_member_t));
            rv = bcm_petra_trunk_get(unit, tid, &trunk_info, BCM_DPP_MAX_STK_TRUNKS, member_array, &member_count);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Get trunk. tid=0x%x."), FUNCTION_NAME(), tid));
            }

            if (member_count >= BCM_DPP_MAX_STK_TRUNKS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG_STR("%s: Stacking Trunk already contains max number of supported members (%d)\n"), FUNCTION_NAME(),BCM_DPP_MAX_STK_TRUNKS));
            }

            for (i=0 ; i < member_count; i++) {
                rv = bcm_petra_stk_gport_sysport_get(unit, member_array[i].gport, &(member_array[i].gport));
                if (BCM_FAILURE(rv)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed convert trunk member from gport modport to gport sysport. member_array[%d].gport=0x%x."), FUNCTION_NAME(), i, member_array[i].gport));
                }
                if (member_array[i].gport == member->gport) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: New member already exist in stacking trunk. member_array[%d].gport=0x%x."), FUNCTION_NAME(), i, member_array[i].gport));
                }
            }
            sal_memcpy(&member_array[member_count], member, sizeof(bcm_trunk_member_t));

            rv = bcm_petra_trunk_set(unit, tid, &trunk_info, member_count + 1, member_array);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Set trunk. tid=0x%x."), FUNCTION_NAME(), tid));
            }

    } else {

        BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "aggregate workspace");
        if (!lag_info) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: unable to allocate %u bytes workspace"),
                          FUNCTION_NAME(),
                          (uint32)sizeof(*lag_info)));
        }
        TRUNK_DB_LOCK(unit);
        _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);
        if (TRUNK_TID_VALID(trunk_id)) {

                SOC_PPD_LAG_INFO_clear(lag_info);
                rv = soc_ppd_lag_get(unit, tid, lag_info);
                rv = handle_sand_result(rv);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_TRUNK,
                              (BSL_META_U(unit,
                                          "%s: Failed (%s) to read trunk (id:%d) info \n"),
                                          FUNCTION_NAME(), bcm_errmsg(rv), tid));
                } else if (!TRUNK_PORTCNT_VALID(unit, lag_info->nof_entries + 1)) {
                    LOG_ERROR(BSL_LS_BCM_TRUNK,
                              (BSL_META_U(unit,
                                          "%s: Trunk already contains max number of supported"
                                          "members (%d)\n"), FUNCTION_NAME(),
                               BCM_TRUNK_MAX_PORTCNT));
                    rv = BCM_E_RESOURCE;
                } else {
                    SOC_PPD_LAG_MEMBER_clear(&lag_member);
                    lag_member.sys_port = sys_port;
                    lag_member.member_id = lag_info->nof_entries;  
                    /* Set ingress disable flags */
                    if((member->flags & BCM_TRUNK_MEMBER_INGRESS_DISABLE) != 0) {
                        lag_member.flags = SOC_PPC_LAG_MEMBER_INGRESS_DISABLE;
                    }             

                    rv = soc_ppd_lag_member_add(unit, tid, &lag_member, &soc_sand_rv);
                    rv = handle_sand_result(soc_sand_rv); /* override the result ? */
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_TRUNK,
                                  (BSL_META_U(unit,
                                              "%s: Error(%s) adding member (gport:%x) to trunk"
                                              " (id:%d) \n"), FUNCTION_NAME(), bcm_errmsg(rv),
                                   member->gport, tid));
                    } else {
                        LOG_ERROR(BSL_LS_BCM_TRUNK,
                                  (BSL_META_U(unit,
                                              "%s: Added member (gport:%x) to trunk (id:%d) "
                                              "\n"), FUNCTION_NAME(), member->gport, tid));
                        _DPP_TRUNK_IN_USE_SET(unit, tid, TRUE); /* set trunk in use for a case it dosn't set yet */
                    }
                }
            
        } else {
            rv = BCM_E_NOT_FOUND;
        }
        TRUNK_DB_UNLOCK(unit);
       BCM_FREE(lag_info);
    }

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid,
                          bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lag_info);
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_stacking_trunk_member_delete(int unit, bcm_trunk_t tid, bcm_trunk_member_t *member)
{
    int         rv = BCM_E_NONE;
    int             soc_sand_rc = SOC_SAND_OK;
    bcm_trunk_t stk_tid;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t member_array[BCM_DPP_MAX_STK_TRUNKS];
    SOC_PPD_LAG_INFO   *lag_info = NULL;
    int member_count = 0;
    int i, member_del_idx = 0xffffffff;
    bcm_gport_t         gport_modport = 0x0;
    uint32                  local_port = 0;
    uint32                  tm_port;
    int                     core;

    BCMDNX_INIT_FUNC_DEFS;

    TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
    BCM_TRUNK_STACKING_TID_SET(tid, stk_tid);
       
    sal_memset(&trunk_info, 0x0, sizeof(bcm_trunk_info_t));
    sal_memset(member_array, 0x0, BCM_DPP_MAX_STK_TRUNKS * sizeof(bcm_trunk_member_t));
    rv = bcm_petra_trunk_get(unit, tid, &trunk_info, BCM_DPP_MAX_STK_TRUNKS, member_array, &member_count);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Get trunk. tid=0x%x."), FUNCTION_NAME(), tid));
    }

    if (member_count == 0x0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG_STR("%s: Stacking Trunk already empty (member_count=%d)\n"), FUNCTION_NAME(),member_count));
    }

    rv = bcm_petra_stk_sysport_gport_get(unit, member->gport, &gport_modport);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to get gport_modport from gport=0x%x"), member->gport));
    }

    for (i=0 ; i < member_count; i++) {
        if (member_array[i].gport == gport_modport) {
            member_del_idx = i;
        }
    }

    if (member_del_idx == 0xffffffff) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG_STR("%s: Input gport member (0x%x), are not in Stacking trunk (%d)\n"), FUNCTION_NAME(),member->gport, tid));
    }

    /* Update port stacking info (is_stacking, peer) */

    local_port = BCM_GPORT_MODPORT_PORT_GET(gport_modport);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, local_port, &tm_port, &core)));

    soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_stacking_info_set,(unit, core, tm_port, 0x0, 0x0)));
    if (SOC_SAND_FAILURE(soc_sand_rc)) {
        BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed setting Stacking info. local_port=%d, stk_tid=%d,"), local_port, stk_tid));
    }

    /* Clear port lb-key range */
    BCMDNX_ALLOC(lag_info, sizeof(SOC_PPD_LAG_INFO), "bcm_petra_stacking_trunk_member_delete.lag_info");
    if (!lag_info) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }
    SOC_PPD_LAG_INFO_clear(lag_info);
    lag_info->members[0x0].sys_port = _SHR_GPORT_IS_MODPORT(member->gport) ? BCM_GPORT_MODPORT_PORT_GET(member->gport) : BCM_GPORT_SYSTEM_PORT_ID_GET(member->gport);
    lag_info->members[0x0].member_id = 0x0;
    lag_info->nof_entries = 0x1;
    lag_info->lb_type = SOC_PPD_LAG_LB_TYPE_HASH;
    rv = soc_ppd_lag_lb_key_range_set(unit, lag_info);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to Clear lag lb-key range.")));
    }

    if (member_count == 1) {

        /* Write entries to Stack trunk resolve table  */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_stack_lag_packets_base_queue_id_set,(unit, stk_tid, SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_ALL, 0x0)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed setting stack_lag_packets_base_queue_id. stk_tid=%d,"), stk_tid));
        }

    } else {
        if (member_del_idx != member_count) {
            sal_memcpy(&member_array[member_del_idx], &member_array[member_count - 1], sizeof(bcm_trunk_member_t));
        }

        rv = bcm_petra_trunk_set(unit, tid, &trunk_info, member_count -1, member_array);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Set trunk. tid=0x%x."), FUNCTION_NAME(), tid));
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lag_info);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_trunk_member_delete(int unit, bcm_trunk_t tid, bcm_trunk_member_t *member)
{
    int         rv = BCM_E_NONE;
    uint32      sys_port;
    bcm_trunk_t trunk_id;
    SOC_PPD_LAG_MEMBER  lag_member;
    
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    TRUNK_CHECK_INIT(unit, ts_init);
     if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }
    if (member == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_delete err,  member == NULL")));
    }

    rv = _bcm_petra_gport_to_sys_port(unit, member->gport, &sys_port);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Invalid member port type (not modport | sysport) specified."), FUNCTION_NAME()));
    } 

    if (member->flags) {
        LOG_WARN(BSL_LS_BCM_TRUNK,
                 (BSL_META_U(unit,
                             "%s: member->flags not supported. \n"), 
                             FUNCTION_NAME()));
    }

    if (TRUNK_TID_IS_STACKING_TID(unit, tid)) { 

        rv = bcm_petra_stacking_trunk_member_delete(unit, tid, member);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Stacking trunk delete Failed."), FUNCTION_NAME()));
        }

    } else {
        /* Before removing the member, get some data for later. */

#ifdef BCM_88660_A0
        SOC_PPD_LAG_INFO lag_info;
        int remove_idx = -1;
        unsigned int soc_sand_unit = (unit);
        uint32 sys_port_to_remove;
        unsigned int lag_member_idx;
        uint32 soc_sand_rv;
        
        SOC_PPD_LAG_INFO_clear(&lag_info);
#endif /* BCM_88660_A0 */

        TRUNK_DB_LOCK(unit);
        _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);

        if (TRUNK_TID_VALID(trunk_id)) {

#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
                 
                /* Get the LAG info. */
                soc_sand_rv = soc_ppd_lag_get(soc_sand_unit, trunk_id, &lag_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                if (lag_info.is_stateful) {
                    /* Get the sysport to remove. */
                    rv = _bcm_petra_gport_to_sys_port(unit, member->gport, &sys_port_to_remove);
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* Get the first index matching the sysport. */
                    for (lag_member_idx = 0; lag_member_idx < lag_info.nof_entries; lag_member_idx++) {
                        if (lag_info.members[lag_member_idx].sys_port == sys_port_to_remove) {
                            break;
                        }
                    }   

                    if (lag_member_idx >= lag_info.nof_entries) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LAG group %d does not contain sysport %d.\n"), trunk_id, sys_port_to_remove));
                    }

                    remove_idx = lag_member_idx; 
                }
            }

#endif /* BCM_88660_A0 */

            /* Set ingress disable flags */
            SOC_PPD_LAG_MEMBER_clear(&lag_member);
            lag_member.sys_port = sys_port;	
            if ((member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) != 0) {
                lag_member.flags = SOC_PPC_LAG_MEMBER_EGRESS_DISABLE;
            } 
            rv = soc_ppd_lag_member_remove(unit, tid, &lag_member);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Error(%s) deleting member(gport:%x) from trunk"
                                      "(id:%d) \n"), FUNCTION_NAME(), bcm_errmsg(rv), 
                           member->gport, tid));
            } else {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Deleted member(gport:%x) from trunk (id:%d)\n"),
                                      FUNCTION_NAME(), member->gport, tid));
            }
        } else {
            rv = BCM_E_NOT_FOUND;
        }
        TRUNK_DB_UNLOCK(unit);

/* For SLB delete all flows pointing to the deleted member and transplant all flows pointing to the last member to its new index. */
/* WARNING - If this fails there will be no rollback (the lag member is still deleted). */
/* This is why we do not expect anything to fail here. */
#ifdef BCM_88660_A0
        if (lag_info.is_stateful) {

            /* Note: The lag_info is still of the LAG group before the removal. */

            /* Remove all flows pointing to the removed idx. */
            {
                SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG match_lag;
                SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG match_lag_member;
                const SOC_PPC_SLB_TRAVERSE_MATCH_RULE *match_rules[2];
                SOC_PPC_SLB_TRAVERSE_ACTION_REMOVE remove_action;

                SOC_PPC_SLB_CLEAR(&match_lag, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG);
                SOC_PPC_SLB_CLEAR(&match_lag_member, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG);
                SOC_PPC_SLB_CLEAR(&remove_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_REMOVE);

                match_lag.lag_ndx = trunk_id;
                match_lag_member.lag_member_ndx = remove_idx;

                match_rules[0] = SOC_PPC_SLB_DOWNCAST(&match_lag, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
                match_rules[1] = SOC_PPC_SLB_DOWNCAST(&match_lag_member, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);

                soc_sand_rv = soc_ppd_slb_traverse(soc_sand_unit, match_rules, 2, SOC_PPC_SLB_DOWNCAST(&remove_action, SOC_PPC_SLB_TRAVERSE_ACTION), NULL);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            /* If the member being removed is not the last member, then transplant all flows pointing to the last idx to point to the removed member idx. */
            if (remove_idx != (lag_info.nof_entries - 1)) {
                SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG match_lag;
                SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG match_lag_member;
                const SOC_PPC_SLB_TRAVERSE_MATCH_RULE *match_rules[2];
                SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE update_action;
                SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER update_lag_member;

                SOC_PPC_SLB_CLEAR(&match_lag, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG);
                SOC_PPC_SLB_CLEAR(&match_lag_member, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG);
                SOC_PPC_SLB_CLEAR(&update_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_UPDATE);
                SOC_PPC_SLB_CLEAR(&update_lag_member, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_UPDATE_VALUE_LAG_MEMBER);

                match_lag.lag_ndx = trunk_id;
                match_lag_member.lag_member_ndx = lag_info.nof_entries - 1;

                match_rules[0] = SOC_PPC_SLB_DOWNCAST(&match_lag, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
                match_rules[1] = SOC_PPC_SLB_DOWNCAST(&match_lag_member, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
                update_action.traverse_update_value = SOC_PPC_SLB_DOWNCAST(&update_lag_member, SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE);
                update_lag_member.new_lag_member_ndx = remove_idx;

                soc_sand_rv = soc_ppd_slb_traverse(soc_sand_unit, match_rules, 2, SOC_PPC_SLB_DOWNCAST(&update_action, SOC_PPC_SLB_TRAVERSE_ACTION), NULL);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
#endif /* BCM_88660_A0 */

    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, 
                          bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_trunk_member_delete_all(int unit, bcm_trunk_t tid)
{
    int                 rv, index, count;
    bcm_trunk_member_t *members = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    BCMDNX_ALLOC(members, (sizeof(bcm_trunk_member_t) * BCM_TRUNK_MAX_PORTCNT), "trunk mem");
    if (members == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("bcm_petra_trunk_member_delete_all err, member == NULL")));
    } 

    rv =  bcm_petra_trunk_member_get(unit, tid, BCM_TRUNK_MAX_PORTCNT, members, 
                                     &count);
    if (BCM_SUCCESS(rv)) {
        for (index = 0; index < count; index++) {

            if (BCM_TRUNK_IS_STACKING_TID(tid))  {
                rv = bcm_petra_stk_gport_sysport_get(unit, members[index].gport, &(members[index].gport));
                if (BCM_FAILURE(rv)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed convert trunk member from gport modport to gport sysport. members[%d].gport=0x%x."), FUNCTION_NAME(), index, members[index].gport));
                }    
            }

             rv = bcm_petra_trunk_member_delete(unit, tid, &members[index]);
             if (BCM_FAILURE(rv))  {
                 break;
             }
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(members);
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_trunk_member_in_other_trunk_check(int unit,  bcm_gport_t gport, bcm_trunk_t tid)
{
    int                 rv=BCM_E_NONE;
    uint32 lag_id;
    uint8 is_in_lag = FALSE;
    uint32 local_port;
    bcm_module_t modid; 
    int         core;
    uint32      tm_port;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get local port */
    rv = bcm_petra_stk_sysport_gport_get(unit, gport, &gport);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to get gport_modport from gport=0x%x"), gport));
    }

    local_port = BCM_GPORT_MODPORT_PORT_GET(gport);
    rv = bcm_petra_stk_my_modid_get(unit, &modid);

    /* check if port is local */
    BCMDNX_IF_ERR_EXIT(rv); 
    if(modid == BCM_GPORT_MODPORT_MODID_GET(gport)) {

        int   soc_sand_rc = SOC_SAND_OK;
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, local_port, &tm_port, &core)));

        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_is_port_lag_member,(unit, core, tm_port, &is_in_lag, &lag_id)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {   
                BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed getting port lag information. local_port=%d"), local_port)); 
        }
        
        if(is_in_lag && lag_id != tid) {
            rv = BCM_E_PARAM;
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_trunk_create_id
 * Purpose:
 *      Create the software data structure for this trunk ID and program the
 *      hardware for this TID. User must call bcm_petra_trunk_set() to finish setting
 *      up this trunk.
 * Parameters:
 *      unit - Device unit number.
 *      tid - The trunk ID.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_EXISTS    - TID already used
 *      BCM_E_BADID     - TID out of range
 */

STATIC int
_bcm_petra_trunk_create_id(int unit, bcm_trunk_t tid, int is_stacking)
{
    int             rv = BCM_E_EXISTS;
    bcm_trunk_t     trunk_id;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, tid));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (is_stacking) {
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    TRUNK_DB_LOCK(unit);

    _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);
    if (trunk_id == BCM_TRUNK_INVALID) {
        rv = BCM_E_NONE;
        _DPP_TRUNK_TRUNK_ID_SET(unit, tid, tid);
        _DPP_TRUNK_IN_USE_SET(unit, tid, FALSE);	
    }

    TRUNK_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, 
                          bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_create
 * Purpose:
 *      Allocate an available Trunk ID from the pool
 *      bcm_trunk_create_id.
 * Parameters:
 *      unit - Device unit number.
 *      flags - Flags.
 *      tid - (IN/Out) The trunk ID, IN if BCM_TRUNK_FLAG_WITH_ID flag is set.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_FULL      - Trunk table full, no more trunks available.
 *      BCM_E_XXXXX     - As set by lower layers of software
 */
int
bcm_petra_trunk_create(int unit, uint32 flags, bcm_trunk_t *tid)
{
    trunk_state_t  *tc;
    int             rv = BCM_E_FULL;
    int             index;
    bcm_trunk_t     trunk_id;
    bcm_trunk_t stk_tid;
    int is_stacking = FALSE;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    if (tid == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Param Error: tid == NULL.")));
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    if (flags & BCM_TRUNK_FLAG_WITH_ID) {
        if (BCM_TRUNK_IS_STACKING_TID(*tid)) {
            is_stacking = TRUE;
            stk_tid = *tid;
            BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &stk_tid));
            BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_create_id(unit, stk_tid, is_stacking));
        } else {
            is_stacking = FALSE;
            BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_create_id(unit, *tid, is_stacking));
        }
        BCM_EXIT;
    }

    TRUNK_CHECK_INIT(unit, ts_init);
    TRUNK_DB_LOCK(unit);

    tc = &TRUNK_CNTL(unit);

    for (index = 0; index < tc->ngroups; index++) {
        _DPP_TRUNK_TRUNK_ID_GET(unit, index, trunk_id);
        if (trunk_id == BCM_TRUNK_INVALID) {
            rv = _bcm_petra_trunk_create_id(unit, index,is_stacking);
            if (BCM_SUCCESS(rv)) {
                *tid = index;
            }
            break;
        }
    }

    TRUNK_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, *tid, 
                          bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_psc_set
 * Purpose:
 *      Set the trunk selection criteria.
 * Parameters:
 *      unit - Device unit number.
 *      tid  - The trunk ID to be affected.
 *      psc  - Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_BADID     - TID out of range
 *      BCM_E_RESOURCE -  psc set to a different value than is currently set
 *      BCM_E_PARAM     - psc value specified is not supported
 * Notes:
 *      On this platform, port selection criteria is global and cannot be
 *      configured per trunk group. The rule is, last psc_set wins and affects
 *      EVERY trunk group!
 */

int
bcm_petra_trunk_psc_set(int unit, bcm_trunk_t tid, int psc)
{
    int             rv;
    SOC_PPD_LAG_INFO   *lag_info = NULL;
    SOC_PPD_LAG_LB_TYPE lb_type = SOC_PPD_LAG_LB_TYPE_HASH;
    bcm_trunk_t     trunk_id;
#ifdef BCM_88660_A0
    uint8 is_stateful = 0;
#endif /* BCM_88660_A0 */

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, tid,
                          psc));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        is_stateful = (psc == BCM_TRUNK_PSC_DYNAMIC_RESILIENT) ? 1 : 0;
    }
#endif /* BCM_88660_A0 */
    rv = _bcm_petra_trunk_psc_to_lb_type(unit, psc, &lb_type);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TRUNK,
                  (BSL_META_U(unit,
                              "%s(%d, %d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid,
                              psc, bcm_errmsg(rv)));
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: Failed (%s) to read trunk (id:%d) info"),
                       FUNCTION_NAME(), bcm_errmsg(rv), tid));
    }

    BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "aggregate workspace");
    if (!lag_info) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: unable to allocate %u bytes workspace"),
                      FUNCTION_NAME(),
                      (uint32)sizeof(*lag_info)));
    }
    TRUNK_DB_LOCK(unit);
    _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);
    if (TRUNK_TID_VALID(trunk_id)) {
        SOC_PPD_LAG_INFO_clear(lag_info);
        rv = soc_ppd_lag_get(unit, tid, lag_info);
        rv = handle_sand_result(rv);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TRUNK,
                      (BSL_META_U(unit,
                                  "%s: Failed (%s) to read trunk (id:%d) info \n"),
                                  FUNCTION_NAME(), bcm_errmsg(rv), tid));
        } else if (lag_info->lb_type == lb_type
#ifdef BCM_88660_A0
                   && (SOC_IS_ARADPLUS(unit) && lag_info->is_stateful == is_stateful)
#endif /* BCM_88660_A0 */
                   ) {
            /* nothing TBD */
            LOG_VERBOSE(BSL_LS_BCM_TRUNK,
                        (BSL_META_U(unit,
                                    "%s: Trunk (%d) psc already set to same(%d)\n"),
                                    FUNCTION_NAME(), tid, psc));
        } else {

#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit)) {
                lag_info->is_stateful = is_stateful;
            }
#endif /* BCM_88660_A0 */
            lag_info->lb_type = lb_type;
            rv = soc_ppd_lag_set(unit, tid, lag_info);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Failed(%s) to set trunk (id:%d) PSC to"
                                      " %d \n"), FUNCTION_NAME(), bcm_errmsg(rv),
                           tid, psc));
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    TRUNK_DB_UNLOCK(unit);
    BCM_FREE(lag_info);

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid,
                          psc, bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_psc_get
 * Purpose:
 *      Get the trunk selection criteria.
 * Parameters:
 *      unit - Device unit number.
 *      tid  - The trunk ID to be used.
 *      psc  - (OUT) Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_BADID     - TID out of range
 */

int
bcm_petra_trunk_psc_get(int unit, bcm_trunk_t tid, int *psc)
{
    int             rv;
    SOC_PPD_LAG_INFO   *lag_info = NULL;
    bcm_trunk_t     trunk_id;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *) - Enter\n"), FUNCTION_NAME(), unit, tid));
    if (psc == NULL) {
        LOG_DEBUG(BSL_LS_BCM_TRUNK,
                  (BSL_META_U(unit,
                              "%s(%d, %d, NULL) - Exit (null param)\n"),
                              FUNCTION_NAME(), unit, tid));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_psc_get err,  (psc == NULL)")));
    }

    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "aggregate workspace");
    if (!lag_info) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: unable to allocate %u bytes workspace"),
                      FUNCTION_NAME(),
                      (uint32)sizeof(*lag_info)));
    }
    TRUNK_DB_LOCK(unit);
    _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);
    if (TRUNK_TID_VALID(trunk_id)) {
        SOC_PPD_LAG_INFO_clear(lag_info);
        rv = soc_ppd_lag_get(unit, tid, lag_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TRUNK,
                      (BSL_META_U(unit,
                                  "%s: Failed (%s) to read trunk (id:%d) info \n"),
                                  FUNCTION_NAME(), bcm_errmsg(rv), tid));
        } else {
            rv = _bcm_petra_trunk_lb_type_to_psc(unit, lag_info->lb_type, psc);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Invalid PSC found on trunk (id:%d) \n"),
                                      FUNCTION_NAME(), tid));
            }
#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit)) {
                if (lag_info->is_stateful) {
                    /* Verify that the PSC is not round robin. */
                    BCMDNX_VERIFY(*psc == BCM_TRUNK_PSC_PORTFLOW);
                    *psc = BCM_TRUNK_PSC_DYNAMIC_RESILIENT;
                }
            }
#endif /* BCM_88660_A0 */
            
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    TRUNK_DB_UNLOCK(unit);
   BCM_FREE(lag_info);

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid,
                          *psc, bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_chip_info_get
 * Purpose:
 *      Get device specific trunking information.
 * Parameters:
 *      unit    - Device unit number.
 *      ta_info - (OUT) Chip specific Trunk information.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 */
int
bcm_petra_trunk_chip_info_get(int unit, bcm_trunk_chip_info_t *ta_info)
{
    trunk_state_t *tc;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    TRUNK_CHECK_INIT(unit, ts_init);
    tc = &TRUNK_CNTL(unit);

    ta_info->trunk_group_count = tc->ngroups;
    ta_info->trunk_id_min = 0;
    ta_info->trunk_id_max = tc->ngroups - 1;
    ta_info->trunk_ports_max = tc->nports ;
    ta_info->trunk_fabric_id_min = -1;
    ta_info->trunk_fabric_id_max = -1;
    ta_info->trunk_fabric_ports_max = -1;

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit,
                          bcm_errmsg(BCM_E_NONE)));
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trunk_set
 * Purpose:
 *      Adds ports to a trunk group.
 * Parameters:
 *      unit - Device unit number.
 *      tid - The trunk ID to be affected.
 *      trunk_info - Information on the trunk group.
 *      member_count - Number of trunk members.
 *      member_array - Array of trunk members.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 * Notes:
 *
 */
int
bcm_petra_trunk_set(int unit,
                    bcm_trunk_t tid,
                    bcm_trunk_info_t *trunk_info,
                    int member_count,
                    bcm_trunk_member_t *member_array)
{
    int                 index;
    int                 rv = BCM_E_NONE;
    bcm_trunk_t         trunk_id;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *, [n=%d - "), FUNCTION_NAME(), unit, tid,
                          member_count));
    for (index = 0; index < member_count; index++) {
        LOG_DEBUG(BSL_LS_BCM_TRUNK,
                  (BSL_META_U(unit,
                              " %08X flags:%08X"), member_array[index].gport,
                              member_array[index].flags));
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "]) - Enter\n")));

    TRUNK_CHECK_INIT(unit, ts_init);
     if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    TRUNK_DB_LOCK(unit);

    /* make sure trunk is in use */
    _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);
    if (TRUNK_TID_VALID(trunk_id)) {
        /* Check number of ports in trunk group */
        if (TRUNK_PORTCNT_VALID(unit, member_count)) {
            rv = _bcm_petra_trunk_set(unit,
                                      tid,
                                      trunk_info,
                                      member_count,
                                      member_array);
            if (BCM_SUCCESS(rv)) {
                _DPP_TRUNK_IN_USE_SET(unit, tid, TRUE); 
           }
        } else {
            rv = BCM_E_PARAM;
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    TRUNK_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid,
                          bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_stacking_trunk_get(int unit,
                    bcm_trunk_t tid,
                    bcm_trunk_info_t *trunk_info,
                    int member_max,
                    bcm_trunk_member_t *member_array,
                    int *member_count)
{
    int             index;
    int             rv = BCM_E_NOT_FOUND;
    int             soc_sand_rc = SOC_SAND_OK;

    bcm_trunk_t stk_tid = 0x0;
    bcm_gport_t     gport_qid, physical_port;
    int num_cos_levels = 0x0;
    uint32                       dest_base_queue = 0x0, first_dest_base_queue = 0x0;
    uint32 flags = 0x0;

    BCMDNX_INIT_FUNC_DEFS;

    TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
            
    for (index = 0; index < SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX; index++) {

        if (index >= member_max) {
            break;
        }

        /* Read entries from Stack trunk resolve table  */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_stack_lag_packets_base_queue_id_get,(unit, stk_tid, index, &dest_base_queue)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed getting stack_lag_packets_base_queue_id. stk_tid=%d, index=%d, dest_base_queue=%d"), stk_tid, index, dest_base_queue));
        }

        if (dest_base_queue == 0x0) {
            LOG_VERBOSE(BSL_LS_BCM_TRUNK,
                        (BSL_META_U(unit,
                                    "%s(): Stacking:  Empty stack_lag. dest_base_queue=0x%x\n"), FUNCTION_NAME(), dest_base_queue));
            rv = BCM_E_NONE;
            break;
        }

        if (index == 0) {
            first_dest_base_queue = dest_base_queue;
        } else {
            if (dest_base_queue == first_dest_base_queue) {
                LOG_VERBOSE(BSL_LS_BCM_TRUNK,
                            (BSL_META_U(unit,
                                        "%s(): Stacking:  wrap around DB. index=%d, dest_base_queue=0x%x\n"), FUNCTION_NAME(), index, dest_base_queue));
                break;
            }
        }

        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport_qid, dest_base_queue);

        LOG_VERBOSE(BSL_LS_BCM_TRUNK,
                    (BSL_META_U(unit,
                                "%s(): Stacking:  tid=%d, stk_tid=%d, index=%d, dest_base_queue=0x%x,  gport_qid=0x%x\n"), FUNCTION_NAME(), tid, stk_tid, index, dest_base_queue, gport_qid));

         rv = bcm_petra_cosq_gport_get(unit, gport_qid, &physical_port, &num_cos_levels, &flags);
         if (rv != BCM_E_NONE) {
             LOG_ERROR(BSL_LS_BCM_TRUNK,
                       (BSL_META_U(unit,
                                   "%s(): failure in getting gport(0x%x) info, error 0x%x\n"),FUNCTION_NAME(), gport_qid, rv));
             rv = BCM_E_INTERNAL;
         }

         member_array[index].gport = physical_port;

         LOG_VERBOSE(BSL_LS_BCM_TRUNK,
                     (BSL_META_U(unit,
                                 "%s(): Stacking:  member_array[index].gport=0x%x, physical_port=0x%x\n"), FUNCTION_NAME(), member_array[index].gport, physical_port));
    }

    *member_count = index;

    sal_memset(trunk_info, 0, sizeof(*trunk_info));
    trunk_info->dlf_index = trunk_info->mc_index = -1;
    trunk_info->ipmc_index = -1;
    trunk_info->psc = SOC_PPD_LAG_LB_TYPE_HASH;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trunk_get
 * Purpose:
 *      Return a port information of given trunk ID.
 * Parameters:
 *      unit - Device unit number.
 *      tid - Trunk ID.
 *      trunk_info   - (OUT) Place to store returned trunk info.
 *      member_max   - (IN) Size of member_array.
 *      member_array - (OUT) Place to store returned trunk members.
 *      member_count - (OUT) Place to store returned number of trunk members.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */
int
bcm_petra_trunk_get(int unit,
                    bcm_trunk_t tid,
                    bcm_trunk_info_t *trunk_info,
                    int member_max,
                    bcm_trunk_member_t *member_array,
                    int *member_count)
{
    int             index;
    int             rv = BCM_E_NOT_FOUND;
    int             psc;
    bcm_gport_t     modport, sys_gport;
    SOC_PPD_LAG_INFO   *lag_info = NULL;
    bcm_trunk_t     trunk_id;

    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *, %d, *, *) - Enter\n"),
                          FUNCTION_NAME(),
               unit,
               tid,
               member_max));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    if ((!trunk_info) || (!member_count)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_get err, ((!trunk_info) || (!member_count))")));
    }
    if (member_max && (!member_array)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_get err,  (member_max && (!member_array))")));
    }

    BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "aggregate workspace");
    if (!lag_info) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: unable to allocate %u bytes workspace"),
                      FUNCTION_NAME(),
                      (uint32)sizeof(*lag_info)));
    }
    TRUNK_DB_LOCK(unit);

    _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);
    if (!TRUNK_TID_VALID(trunk_id)) {
        LOG_ERROR(BSL_LS_BCM_TRUNK,
                  (BSL_META_U(unit,
                              "%s: Trunk(%d) not initialized\n"), FUNCTION_NAME(),
                              tid));
        rv = BCM_E_NOT_FOUND;
    } else {
        SOC_PPD_LAG_INFO_clear(lag_info);

        if (TRUNK_TID_IS_STACKING_TID(unit, tid)) { 
            rv = bcm_petra_stacking_trunk_get(unit, tid, trunk_info, member_max, member_array, member_count);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Stacking trunk Get Failed."), FUNCTION_NAME()));
            }
        } else {

            rv = soc_ppd_lag_get(unit, tid, lag_info);
            rv = handle_sand_result(rv);
            
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Failed (%s) to read trunk (id:%d) info \n"),
                                      FUNCTION_NAME(), bcm_errmsg(rv), tid));
            } else {
                rv = _bcm_petra_trunk_lb_type_to_psc(unit, lag_info->lb_type, &psc);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_TRUNK,
                              (BSL_META_U(unit,
                                          "%s: Invalid PSC on trunk (id:%d) info \n"),
                                          FUNCTION_NAME(), tid));
                    rv = BCM_E_INTERNAL;
                }
#ifdef BCM_88660_A0
                if (SOC_IS_ARADPLUS(unit)) {
                    if (lag_info->is_stateful) {
                        /* Verify that the PSC is not round robin. */
                        BCMDNX_VERIFY(psc == BCM_TRUNK_PSC_PORTFLOW);
                        psc = BCM_TRUNK_PSC_DYNAMIC_RESILIENT;
                    }
                }
#endif /* BCM_88660_A0 */
            }
            if (!BCM_FAILURE(rv)) {
                sal_memset(trunk_info, 0, sizeof(*trunk_info));
                trunk_info->dlf_index = trunk_info->mc_index = -1;
                trunk_info->ipmc_index = -1;
                trunk_info->psc = psc;
                if (!member_max) {
                    /* no member space means querying size of aggregate */
                    *member_count = lag_info->nof_entries;
                } else {
                    /* otherwise fill available space or to end of aggregate */
                    for (index = 0;
                          (index < member_max) &&
                          (index < lag_info->nof_entries) &&
                          (BCM_SUCCESS(rv));
                          index++) {
                        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport,
                                                     lag_info->members[index].sys_port);
                        rv = bcm_petra_stk_sysport_gport_get(unit, sys_gport, &modport);
                        sal_memset(&(member_array[index]),
                                   0x00,
                                   sizeof(member_array[index]));
                        member_array[index].gport = modport;
                    }
                *member_count = index;
                }
            }
        }
    }

    TRUNK_DB_UNLOCK(unit);

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, *, %d, [n=%d - "),
                          FUNCTION_NAME(),
               unit,
               tid,
               member_max,
               *member_count));
    for (index = 0; index < member_max; index++) {
        LOG_DEBUG(BSL_LS_BCM_TRUNK,
                  (BSL_META_U(unit,
                              " %08X flags:%08X"),
                              member_array[index].gport,
                   member_array[index].flags));
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "]) - Exit(%s)\n"), bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lag_info);
	BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_destroy
 * Purpose:
 *      Removes a trunk group. Performs hardware steps neccessary to tear
 *      down a created trunk.
 * Parameters:
 *      unit - Device unit number.
 *      tid  - Trunk Id.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_BADID     - TID out of range
 *      BCM_E_NOT_FOUND - Trunk does not exist
 *      BCM_E_XXXXX     - As set by lower layers of software
 * Notes:
 *      The return code of the trunk_set call is purposely ignored.
 */

int
bcm_petra_trunk_destroy(int unit, bcm_trunk_t tid)
{
    bcm_trunk_info_t        trunkInfo;
    int                     rv = BCM_E_NOT_FOUND;
    bcm_trunk_t     trunk_id;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, tid));
    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    TRUNK_DB_LOCK(unit);

    _DPP_TRUNK_TRUNK_ID_GET(unit, tid, trunk_id);
    if (trunk_id != BCM_TRUNK_INVALID) {

        if (TRUNK_TID_IS_STACKING_TID(unit, tid)) {
            TRUNK_TID_STACKING_TID_GET(unit, tid, tid);
            BCM_TRUNK_STACKING_TID_SET(tid, tid);
            rv = bcm_petra_trunk_member_delete_all(unit, tid);
            BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        } else {
            sal_memset((void *)&trunkInfo, 0, sizeof(trunkInfo));
            rv = bcm_petra_trunk_psc_get(unit, tid, &trunkInfo.psc);
            if (BCM_SUCCESS(rv)) {
                    
             BCMDNX_IF_ERR_EXIT(bcm_petra_trunk_member_delete_all(unit, tid));
             rv = _bcm_petra_trunk_set(unit, tid, &trunkInfo, 0, NULL);

            }
        }
        if (BCM_SUCCESS(rv)) {
            _DPP_TRUNK_TRUNK_ID_SET(unit, tid, BCM_TRUNK_INVALID);
            _DPP_TRUNK_IN_USE_SET(unit, tid, FALSE);
        }
    }

    TRUNK_DB_UNLOCK(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, 
                          bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_trunk_detach
 * Purpose:     Shuts down the trunk module.
 * Parameters:  unit - Device unit number.
 * Returns:     BCM_E_NONE              Success.
 *              BCM_E_XXX
 */
int
bcm_petra_trunk_detach(int unit)
{
    trunk_state_t  *tc;
    int             rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));

    if (TRUNK_CNTL(unit).init_state == ts_none) {
        BCM_EXIT;
    }

    tc = &TRUNK_CNTL(unit);

    TRUNK_DB_LOCK(unit);

    tc->init_state = ts_none;

    /* Free trunk_private_t (t_info) structures */
    rv = _bcm_dpp_wb_trunk_control_free(unit);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: failed to free trunk info"),
                           FUNCTION_NAME()));
        }

    /* Set number of ports and groups to zero */
    tc->ngroups = 0;
    tc->stk_ngroups = 0;
    tc->nports  = 0;

    TRUNK_DB_UNLOCK(unit);

    /* Destroy LOCK (no more data to protect */
    if (tc->lock != NULL) {
        sal_mutex_destroy(tc->lock);
        tc->lock = NULL;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_trunk_state_deinit(unit);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_TRUNK,
                  (BSL_META_U(unit,
                              "unit %d, failed warmboot deinit, error 0x%x\n"),
                              unit,
                   rv));
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d) - Exit(%s)\n"), FUNCTION_NAME(), unit, 
                          bcm_errmsg(rv)));

#ifdef BCM_WARM_BOOT_SUPPORT    
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_trunk_state_deinit(unit));
#endif /*BCM_WARM_BOOT_SUPPORT*/

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_bitmap_expand
 * Purpose:
 *      Given a port bitmap, if any of the ports are in a trunk,
 *      add all the trunk member ports to the bitmap.
 * Parameters:
 *      unit     - Device unit number.
 *      pbmp_ptr - Input/output port bitmap
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 * Notes:
 */
int
bcm_petra_trunk_bitmap_expand(int unit, bcm_pbmp_t *pbmp_ptr)
{
    trunk_state_t          *tc;
    bcm_trunk_t             tid;
    int                     index;
    bcm_pbmp_t              pbmp, t_pbmp, o_pbmp;
    int                     rv = BCM_E_NONE;
    bcm_module_t            modid;
    bcm_gport_t             modport, sys_gport;
    SOC_PPD_LAG_INFO           *lag_info = NULL;
    int                     in_use;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit,
                          SOC_PBMP_WORD_GET(*pbmp_ptr,0)));

    rv = bcm_petra_stk_my_modid_get(unit, &modid);
    BCMDNX_IF_ERR_EXIT(rv);

    TRUNK_CHECK_INIT(unit, ts_init);
    BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "aggregate workspace");
    if (!lag_info) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: unable to allocate %u bytes workspace"),
                      FUNCTION_NAME(),
                      (uint32)sizeof(*lag_info)));
    }
    TRUNK_DB_LOCK(unit);

    tc = &TRUNK_CNTL(unit);
    for (tid = 0; tid < tc->ngroups; tid++) {
        _DPP_TRUNK_IN_USE_GET(unit, tid, in_use);
        if (in_use == TRUE) {
            BCM_PBMP_CLEAR(t_pbmp);
            BCM_PBMP_CLEAR(o_pbmp);
            BCM_PBMP_CLEAR(pbmp);
            rv = soc_ppd_lag_get(unit, tid, lag_info);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Failed (%s) to read trunk (id:%d) info \n"),
                                      FUNCTION_NAME(), bcm_errmsg(rv), tid));
            } else {
                for (index = 0; ((index < lag_info->nof_entries) &&
                                 (BCM_SUCCESS(rv))); index++) {
                    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport,
                                                 lag_info->members[index].sys_port);
                    rv = bcm_petra_stk_sysport_gport_get(unit, sys_gport, &modport);
                    if (BCM_SUCCESS(rv) &&
                        (modid == BCM_GPORT_MODPORT_MODID_GET(modport))) {
                        BCM_PBMP_PORT_ADD(t_pbmp, BCM_GPORT_MODPORT_PORT_GET(modport));
                    }
                }
            }
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Failed (%s) to get trunk (id:%d) info \n"),
                                      FUNCTION_NAME(), bcm_errmsg(rv), tid));
                break;
            }
            BCM_PBMP_ASSIGN(pbmp, t_pbmp);      /* save a copy */
            BCM_PBMP_AND(t_pbmp, *pbmp_ptr);    /* find common ports */

            /* if lists have common member */
            if (BCM_PBMP_NOT_NULL(t_pbmp) == TRUE) {
                BCM_PBMP_OR(o_pbmp, pbmp);   /* add saved member set */
            }
        }
    }
    TRUNK_DB_UNLOCK(unit);
   BCM_FREE(lag_info);
    if (BCM_E_NONE == rv) {
        /* add all found members to the caller's pbmp */
        BCM_PBMP_OR(*pbmp_ptr, o_pbmp);
    }

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit,
                          SOC_PBMP_WORD_GET(*pbmp_ptr,0), bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_mcast_join
 * Purpose:
 *    Add the trunk group to existing MAC multicast entry.
 * Parameters:
 *      unit - Device unit number.
 *      tid - Trunk Id.
 *      vid - Vlan ID.
 *      mac - MAC address.
 * Returns:
 *    BCM_E_XXX
 * Notes:
 *      Applications have to remove the MAC multicast entry and re-add in with
 *      new port bitmap to remove the trunk group from MAC multicast entry.
 */

int
bcm_petra_trunk_mcast_join(int unit, bcm_trunk_t tid, bcm_vlan_t vid, bcm_mac_t mac)
{
    BCMDNX_INIT_FUNC_DEFS;
    LOG_VERBOSE(BSL_LS_BCM_TRUNK,
                (BSL_META_U(unit,
                            "%s(unit %d, tid %d) - This API is not available.\n"),
                            FUNCTION_NAME(), unit, tid));

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_mcast_join err , not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_find
 * Description:
 *      Get trunk id that contains the given system port
 * Parameters:
 *      unit    - Device unit number
 *      modid   - Module ID
 *      port    - Port number
 *      tid     - (OUT) Trunk id
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_NOT_FOUND - The module:port combo was not found in a trunk.
 */
int
bcm_petra_trunk_find(int unit, bcm_module_t modid, bcm_port_t port,
                      bcm_trunk_t *tid)
{
    trunk_state_t     *tc;
    bcm_trunk_t         t;
    int                 index, found;
    int                 rv = BCM_E_NONE;
    SOC_PPD_LAG_INFO       *lag_info = NULL;
    bcm_gport_t         modport, sys_gport;
    int                 in_use;
    bcm_trunk_t         trunk_id;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, *) - Enter\n"), FUNCTION_NAME(), unit,
                          modid, port));

    TRUNK_CHECK_INIT(unit, ts_init);

    BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "aggregate workspace");
    if (!lag_info) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: unable to allocate %u bytes workspace"),
                      FUNCTION_NAME(),
                      (uint32)sizeof(*lag_info)));
    }

    TRUNK_DB_LOCK(unit);

    tc = &TRUNK_CNTL(unit);
    *tid = BCM_TRUNK_INVALID;

    found = 0;
    for (t = 0; t < tc->ngroups; t++) {
        _DPP_TRUNK_IN_USE_GET(unit, t, in_use);
        if (TRUE == in_use) {
            SOC_PPD_LAG_INFO_clear(lag_info);
            _DPP_TRUNK_TRUNK_ID_GET(unit, t, trunk_id);
            rv = soc_ppd_lag_get(unit, trunk_id, lag_info);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK,
                          (BSL_META_U(unit,
                                      "%s: Failed (%s) to read trunk (id:%d) info \n"),
                                      FUNCTION_NAME(), bcm_errmsg(rv), trunk_id));
            } else {
                for (index = 0; index < lag_info->nof_entries; index++) {
                    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport,
                                             lag_info->members[index].sys_port);
                    rv = bcm_petra_stk_sysport_gport_get(unit, sys_gport, &modport);
                    if (BCM_SUCCESS(rv)) {
                        
                      if ((modid == BCM_GPORT_MODPORT_MODID_GET(modport)) &&
                            (port == BCM_GPORT_MODPORT_PORT_GET(modport))) { 
                            found = 1;
                            *tid = trunk_id;
                            break;
                        }
                    } else {
                        LOG_ERROR(BSL_LS_BCM_TRUNK,
                                  (BSL_META_U(unit,
                                              "%s: Failed to map sys_gport(%d) to "
                                              "modport\n"), FUNCTION_NAME(), sys_gport));
                        break;
                    }
                }
            }
        }
        if (found || (BCM_FAILURE(rv))) {
            break;
        }
    }

    TRUNK_DB_UNLOCK(unit);
   BCM_FREE(lag_info);

    if (found == 0) {
        rv = BCM_E_NOT_FOUND;
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit,
                          modid, port, *tid, bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:    _bcm_petra_trunk_recover
 * Purpose:     recover the internal trunk software state
 * Parameters:  unit - Device unit number.
 * Returns:     BCM_E_*
 */
STATIC int
_bcm_petra_trunk_recover(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    /* Implement when warmboot feature is added */
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("_bcm_petra_trunk_recover err, not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}
#endif

/* 
 * Function:    bcm_petra_trunk_init
 * Purpose:     Initializes the trunk module. The hardware and the software 
 *              data structures are both set to their initial states with no 
 *              trunks configured.
 * Parameters:  unit - Device unit number.
 * Returns:     BCM_E_NONE      - Success.
 *              BCM_E_MEMORY    - Out of memory
 *              BCM_E_XXXXX     - As set by lower layers of software
 */
int
bcm_petra_trunk_init(int unit)
{
    trunk_state_t  *tc;
    bcm_trunk_t     tid;
    int             rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));

    tc = &TRUNK_CNTL(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) && (tc->init_state != ts_none)) {
        if (tc->init_state == ts_recovering) { 
            BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_recover(unit));
            BCM_EXIT;
        }
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_trunk_init err,(SOC_WARM_BOOT(unit) && (tc->init_state != ts_none)) ")));
    }
#endif

    if (tc->lock != NULL) {
        rv = bcm_petra_trunk_detach(unit);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: failed to detach trunk module"),
                           FUNCTION_NAME()));
        }
        tc->lock = NULL;        
    }
 
    if (tc->lock == NULL) {
        if (NULL == (tc->lock = sal_mutex_create("soc_petra_trunk_lock"))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("bcm_petra_trunk_init err,  cannot create mutex tc->lock")));
        }
    }
    tc->stk_ngroups = _stk_ngroups;
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        rv = soc_dpp_arad_trunk_groups_ports_num_get(unit, &tc->ngroups, &tc->nports);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: failed to get correct groups & ports number of trunk"),
                           FUNCTION_NAME()));
        }
    } else 
#endif
    {
        tc->ngroups = _ngroups;
        tc->nports = _nports;
    }

    /* Free trunk_private_t (t_info) structures if allocated */
    rv = _bcm_dpp_wb_trunk_control_free(unit);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: failed to free trunk info"),
                           FUNCTION_NAME()));
        }

    /* alloc memory and clear */
    if ((tc->ngroups > 0) || (tc->stk_ngroups > 0)) {
        rv = _bcm_dpp_wb_trunk_control_allocate(unit, (tc->ngroups + tc->stk_ngroups));
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: failed to allocate trunk info"),
                           FUNCTION_NAME()));
        }        
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bcm_petra_trunk_init err, ((tc->ngroups <= 0) && (tc->stk_ngroups <= 0)) ")));
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* in WB initialization this values are soon being initialized from scache,
           so there is no need to set them here */
        /* Init internal structures */
        for (tid = 0; tid < (tc->ngroups + tc->stk_ngroups); tid++) {
            /* disable all trunk group */
            _DPP_TRUNK_TRUNK_ID_SET(unit, tid, BCM_TRUNK_INVALID);
            _DPP_TRUNK_IN_USE_SET(unit, tid, FALSE);
        }
    }


#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_trunk_state_init(unit);  
    if (rv != BCM_E_NONE) { 
        LOG_ERROR(BSL_LS_BCM_TRUNK,
                  (BSL_META_U(unit,
                              "unit %d, failed warmboot init, error 0x%x\n"), unit, rv)); 	 
        BCMDNX_IF_ERR_EXIT(rv);
    } 
#endif /* BCM_WARM_BOOT_SUPPORT */ 	 

    /* check if any HW init is required for each group */

    if (rv == BCM_E_NONE) {
        tc->init_state = ts_init;
    }

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "bcm_petra_trunk_init: unit=%d rv=%d(%s)\n"),
                          unit, rv, bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if(rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_TRUNK,
                  (BSL_META_U(unit,
                              " Failed while executing the macro SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX.\n")));
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_ARAD_SUPPORT
int soc_dpp_arad_trunk_groups_ports_num_get(int unit, int *ngroups, int *nports)
{
  ARAD_PORT_LAG_MODE     lag_mode;
  uint32                 sys_lag_port_id_nof_bits;
    
    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));
    arad_ports_lag_mode_get_unsafe(unit, &lag_mode, &sys_lag_port_id_nof_bits);

    switch (lag_mode) 
    {
        case SOC_TMC_PORT_LAG_MODE_1K_16:
            *ngroups = 1024;
            *nports = 16;
            break;
        case SOC_TMC_PORT_LAG_MODE_512_32:
            *ngroups = 512;
            *nports = 32;
            break;
        case SOC_TMC_PORT_LAG_MODE_256_64:
            *ngroups = 256;
            *nports = 64;
            break;
        case SOC_TMC_PORT_LAG_MODE_128_128:
            *ngroups = 128;
            *nports = 128;
            break;
        case SOC_TMC_PORT_LAG_MODE_64_256:
            *ngroups = 64;
#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit)) {
                *nports = 256; /* Fixed */
            } else 
#endif
            {
                *nports = 255; /* Should be 256 but there is bug in HW (will be fixed in arad+) */
                
            }            

            break;
        default:
            BCMDNX_ERR_EXIT_MSG(SOC_E_FAIL, (_BSL_BCM_MSG("%s: failed to get correct lag mode"), FUNCTION_NAME()));
            break;

    }

exit:
    BCMDNX_FUNC_RETURN;
}
#endif

/*
 * Function:
 *      bcm_petra_trunk_failover_set
 * Purpose:
 *      Assign the failover port list for a specific trunk port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk id.
 *      failport - (IN) Port in trunk for which to specify failover port list.
 *      psc - (IN) Port selection criteria for failover port list.
 *      flags - (IN) BCM_TRUNK_FLAG_FAILOVER_xxx.
 *      count - (IN) Number of ports in failover port list.
 *      fail_to_array - (IN) Failover port list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_petra_trunk_failover_set(int unit, bcm_trunk_t tid, bcm_gport_t failport, 
                             int psc, uint32 flags, int count, 
                             bcm_gport_t *fail_to_array)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_failover_set unsupported"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_trunk_trunk_failover_get
 * Purpose:
 *      Retrieve the failover port list for a specific trunk port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk id.
 *      failport - (IN) Port in trunk for which to retrieve failover port list.
 *      psc - (OUT) Port selection criteria for failover port list.
 *      flags - (OUT) BCM_TRUNK_FLAG_FAILOVER_xxx.
 *      array_size - (IN) Maximum number of ports in provided failover port list.
 *      fail_to_array - (OUT) Failover port list.
 *      array_count - (OUT) Number of ports in returned failover port list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_petra_trunk_failover_get(int unit, bcm_trunk_t tid, bcm_gport_t failport, 
                             int *psc, uint32 *flags, int array_size, 
                             bcm_gport_t *fail_to_array, int *array_count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_failover_get unsupported"))); 
exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP      
int      
_bcm_dpp_trunk_sw_dump(int unit)      
{
    trunk_state_t  *tc;
    uint32 i;
    int in_use;
    bcm_trunk_t     trunk_id;

    BCMDNX_INIT_FUNC_DEFS;

    /* Make sure the trunk module is initialized */
    TRUNK_CHECK_INIT(unit, ts_init);


    LOG_CLI((BSL_META_U(unit,
                        "\nTRUNK:")));
    LOG_CLI((BSL_META_U(unit,
                        "\n------")));

    tc = &TRUNK_CNTL(unit);

    LOG_CLI((BSL_META_U(unit,
                        "\n\ntrunks:\n")));

    for (i = 0; i < tc->ngroups; i++) {
        _DPP_TRUNK_IN_USE_GET(unit, i, in_use);
        _DPP_TRUNK_TRUNK_ID_GET(unit, i, trunk_id);
        if(BCM_TRUNK_INVALID != trunk_id) {
            LOG_CLI((BSL_META_U(unit,
                                "  (%3d) trunk_id %3d in_use %c\n"),
                     i, 
                     trunk_id,
                     in_use ? 'T' : 'F'));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */


#ifdef BCM_88660_A0

/** 
 * PURPOSE 
 *   Traverse all SLB entries and execute the callback (trav_fn) 
 *   on each entry that matchs the rule specified in match_entry.
 *  
 * PARAMETERS 
 *   unit [IN]
 *   flags [IN]                                   - Specify options. Flag combinations are allowed.
 *     BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID [FLAG]    - Match the trunk (LAG) group (specified in match_entry).
 *     BCM_TRUNK_RESILIENT_MATCH_MEMBER [FLAG]      - Match the trunk (LAG) member (specified in match_entry).
 *   match_entry [IN]                             - Specify the rule to use when matching entries.
 *     tid [IN]                                     - The trunk (LAG) to match (in case MATCH_TRUNK_ID is specified).
 *     member [IN]                                  - The trunk (LAG) member to match (in case MATCH_MEMBER is specified).
 *                                                    [ In by_index, the gport should be the INDEX and NOT the gport ]
 *   trav_fn [IN]                                 - A callback to be called for each matching entry.
 *                                                  If the callback returns a non-zero result, then
 *                                                  traverse stops, and BCM_E_FAIL is returned.
 *   user_data [IN]                               - Opaque user data to be passed back to the callback.
 *  
 * RETURNS 
 *   BCM_E_PARAM     - Error in parameters.
 *   BCM_E_INTERNAL  - Internal error or error in parameters.
 *   BCM_E_FAIL      - The traverse callback returned a non-zero result. 
 *  
 * NOTES 
 *   This function is only available for Arad+. 
 *   If no match flag is specified then all entries are matched. 
 */
int 
  _bcm_petra_trunk_resilient_traverse_by_index(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    bcm_trunk_resilient_traverse_cb trav_fn, 
    void *user_data
  )
{
  int rv = BCM_E_NONE;
  uint32 soc_sand_rv;

  uint32 nof_match_rules = 0;
  const uint32 possible_flags = _BCM_TRUNK_RESILIENT_MATCH_FLAGS;
  uint32 nof_scanned_entries;

  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_LAG all_lag_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG group_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG member_match_rule;
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE * match_rules[2];

  SOC_SAND_TABLE_BLOCK_RANGE block_range;

  SOC_PPC_SLB_ENTRY_KEY   *slb_keys = NULL;
  SOC_PPC_SLB_ENTRY_VALUE *slb_values = NULL;

  BCMDNX_INIT_FUNC_DEFS;
  /* _BCM_DPP_SWITCH_API_START(unit); */

  TRUNK_CHECK_INIT(unit, ts_init);

  /* Only Arad+ and above is supported. */
  if (!SOC_IS_ARADPLUS(unit)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_l3_egress_ecmp_resilient_replace is unsupported for this device.\n")));
  }

  /* Check flags */
  if ((flags & ~possible_flags) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags specified (0x%x).\n"), flags & ~possible_flags));
  }

  /* We currently do not support matching the key. */
  if ((flags & BCM_TRUNK_RESILIENT_MATCH_HASH_KEY) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY (0x%x) is unsupported.\n"), BCM_TRUNK_RESILIENT_MATCH_HASH_KEY));
  }

  /* If we have some match flag, then check match_entry. */
  if (flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) {
    BCMDNX_NULL_CHECK(match_entry);
  } 

  /* Check the callback is not NULL. */
  BCMDNX_NULL_CHECK(trav_fn);

  unit = (unit);

  BCMDNX_ALLOC(slb_keys, sizeof(slb_keys[0]) * SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, "slb_keys");
  BCMDNX_ALLOC(slb_values, sizeof(slb_values[0]) * SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, "slb_values");

  if ((slb_keys == NULL) || (slb_values == NULL)) {
    BCM_FREE(slb_keys);
    BCM_FREE(slb_values);
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Could not allocate enough memory for this operation.\n")));
  }

  SOC_PPC_SLB_CLEAR(&all_lag_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_LAG);
  SOC_PPC_SLB_CLEAR(&group_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG);
  SOC_PPC_SLB_CLEAR(&member_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG);

  /* If there are no match flags then match all. */
  if ((flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) == 0) {
    match_rules[0] = SOC_PPC_SLB_DOWNCAST(&all_lag_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules = 1;
  }

  if (flags & BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID) {
    group_match_rule.lag_ndx = match_entry->tid;

    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&group_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  if (flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) {
    member_match_rule.lag_member_ndx = match_entry->member->gport;
    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&member_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  /* Prepare the block range. */
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

  SOC_SAND_TBL_ITER_SET_BEGIN(&block_range.iter);
  
  block_range.entries_to_act = SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK;
  block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

  /* Loop over all entries. */
  while (!SOC_SAND_TBL_ITER_IS_END(&block_range.iter)) {
    unsigned int entry_idx;

    TRUNK_DB_LOCK(unit); /* { */

      soc_sand_rv = soc_ppd_slb_get_block(
        unit,
        match_rules,
        nof_match_rules,
        &block_range,
        slb_keys,
        slb_values,
        &nof_scanned_entries
      );

    /* } */ TRUNK_DB_UNLOCK(unit);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    for (entry_idx = 0; entry_idx < nof_scanned_entries; entry_idx++) {
      bcm_trunk_resilient_entry_t entry;
      bcm_trunk_member_t lag_member;
      uint32 lag_member_ndx;
      SOC_PPD_LAG_INFO lag_info;

      BCMDNX_VERIFY(!(slb_keys[entry_idx].is_fec));
      BCMDNX_VERIFY(slb_values[entry_idx].lag_valid);
      BCMDNX_VERIFY(slb_values[entry_idx].lag_ndx == slb_keys[entry_idx].lb_group.lag_ndx);

      lag_member_ndx = slb_values[entry_idx].lag_member_ndx;
      
      SOC_PPD_LAG_INFO_clear(&lag_info);

      entry.tid = slb_values[entry_idx].lag_ndx;

      rv = soc_ppd_lag_get(unit, entry.tid, &lag_info);
      BCM_SAND_IF_ERR_EXIT(rv);

      BCMDNX_VERIFY(lag_info.is_stateful);
      BCMDNX_VERIFY(lag_info.nof_entries > lag_member_ndx);

      bcm_trunk_member_t_init(&lag_member);

      /* See above for an exaplanation about this. */
      if (lag_member_ndx >= lag_info.nof_entries) {
        lag_member.gport = lag_member_ndx;
      } else{
        rv = bcm_petra_stk_sysport_gport_get(unit, lag_info.members[lag_member_ndx].sys_port, &lag_member.gport);
        BCMDNX_IF_ERR_EXIT(rv);
      }
      lag_member.flags = 0;
      
      entry.hash_key = slb_keys[entry_idx].flow_label_id;
      entry.member = &lag_member;

      rv = trav_fn(unit, &entry, user_data);
      if (rv != 0) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_FAIL);
      }
    }
  }

exit:
  /* _BCM_DPP_SWITCH_API_END(unit); */
  BCMDNX_FUNC_RETURN;
}

/**
 * Wrapper around _bcm_petra_trunk_resilient_traverse_by_index to convert from gport to index. 
 * gport as match rule -> match all indices containing this gport. 
 */
int 
  bcm_petra_trunk_resilient_traverse(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    bcm_trunk_resilient_traverse_cb trav_fn, 
    void *user_data
  )
{
  int rv;
  uint32 lag_member_ndx;
  SOC_PPD_LAG_INFO lag_info;
  bcm_trunk_resilient_entry_t match_entry_internal;
  bcm_trunk_member_t match_member;

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  _BCM_DPP_SWITCH_API_START(unit);

  /* If a match flag or replace is present then check match_entry. */
  /* For matching, we have to have he match_entry. */
  /* For replace we need the match_entry to specify the group we replace the member to. */
  if ((flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) != 0) {
    BCMDNX_NULL_CHECK(match_entry);

    SOC_PPD_LAG_INFO_clear(&lag_info);

    /* Remark: replace_entry->tid is ignored! We assume the gport in replace_entry is relevant to the LAG match_entry->tid. */
    /*         Otherwise we might end up trying to replace the member of LAG A with the member of LAG B. */
    rv = soc_ppd_lag_get(unit, match_entry->tid, &lag_info);
    BCM_SAND_IF_ERR_EXIT(rv);
  }

  /* We currently do not support MEMBER without TRUNK_ID. */
  if (((flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) != 0) && ((flags & BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID) == 0)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
                     (_BSL_BCM_MSG("BCM_TRUNK_RESILIENT_MATCH_MEMBER is only allowed in combination with BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID.\n")));
  }

  TRUNK_CHECK_INIT(unit, ts_init);

  if (flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) {
    uint32 match_sys_port;

    rv = _bcm_petra_gport_to_sys_port(unit, match_entry->member->gport, &match_sys_port);
    BCMDNX_IF_ERR_EXIT(rv);

    match_entry_internal = *match_entry;
    match_entry_internal.member = &match_member;

    /* We need to traverse all member indices with a matching gport. */
    for (lag_member_ndx = 0; lag_member_ndx < lag_info.nof_entries; lag_member_ndx++) {
      if (lag_info.members[lag_member_ndx].sys_port == match_sys_port) {
        match_entry_internal.member->gport = lag_member_ndx;

        rv = _bcm_petra_trunk_resilient_traverse_by_index(unit, flags, &match_entry_internal, trav_fn, user_data);
        BCMDNX_IF_ERR_EXIT(rv);
      }
    }
  } else {
    rv = _bcm_petra_trunk_resilient_traverse_by_index(unit, flags, match_entry, trav_fn, user_data);
    BCMDNX_IF_ERR_EXIT(rv);
  }

exit:
  _BCM_DPP_SWITCH_API_END(unit);
  BCMDNX_FUNC_RETURN;
}

/** 
 * PURPOSE 
 *   Perform an action on matching SLB ECMP entries.
 *  
 * PARAMETERS 
 *   flags [IN]                                   - Specify options. Flag combinations are allowed.
 *                                                  Any combination of match flags (MATCH_XXX) is allowed.
 *                                                  However exactly one action (REPLACE/DELETE/COUNT) flag
 *                                                  must be specified.
 *     BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID [FLAG]    - Match the trunk (LAG) group (specified in match_entry).
 *     BCM_TRUNK_RESILIENT_MATCH_MEMBER [FLAG]      - Match the trunk (LAG) member (specified in match_entry).
 *     BCM_TRUNK_RESILIENT_REPLACE [FLAG]           - Replace matching entries with the data specified
 *                                                    in replace_entry.
 *     BCM_TRUNK_RESILIENT_DELETE [FLAG]            - Delete matching entries.
 *     BCM_TRUNK_RESILIENT_COUNT [FLAG]             - Count matching entries (no action).
 *   match_entry [IN]                             - Specify the rule to use when matching entries.
 *     tid [IN]                                     - The trunk (LAG) to match (in case MATCH_TRUNK_ID is specified).
 *     member [IN]                                  - The trunk (LAG) member to match (in case MATCH_MEMBER is specified).
 *                                                    [ In by_index, the gport should be the INDEX and NOT the gport ]
 *   num_entries [OUT]                            - If non-NULL then the no. of entries that were matched is
 *                                                  returned.
 *   replace_entry [IN]                           - In case the action is REPLACE, then this will determine
 *                                                  the values that matching entries will be replaced with.
 *     member [IN]                                  - The LAG member will be replaced to this.
 *                                                    [ In by_index, the gport should be the INDEX and NOT the gport ]
 *    
 * RETURNS 
 *   BCM_E_PARAM     - Error in parameters.
 *   BCM_E_INTERNAL  - Internal error or error in parameters.
 *  
 * NOTES 
 *   This function is only available for Arad+. 
 *   If no match flag is specified then all entries are matched. 
 */
int 
  _bcm_petra_trunk_resilient_replace_by_index(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    int *num_entries, 
    bcm_trunk_resilient_entry_t *replace_entry
  )
{
  uint32 soc_sand_rv;

  uint32 nof_match_rules = 0;
  uint32 num_entries_internal;
  const uint32 possible_flags = _BCM_TRUNK_RESILIENT_MATCH_FLAGS | _BCM_TRUNK_RESILIENT_ACTION_FLAGS;
  
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_LAG all_lag_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG group_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG member_match_rule;
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE *match_rules[2];
  SOC_PPC_SLB_TRAVERSE_ACTION *action;
  SOC_PPC_SLB_TRAVERSE_ACTION_COUNT count_action;
  SOC_PPC_SLB_TRAVERSE_ACTION_REMOVE remove_action;
  SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE update_action;
  SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER update_value_lag_member;

  BCMDNX_INIT_FUNC_DEFS;
  /* _BCM_DPP_SWITCH_API_START(unit); */

  TRUNK_CHECK_INIT(unit, ts_init);

  /* This is only supported for Arad+ and above. */
  if (!SOC_IS_ARADPLUS(unit)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_resilient_replace is unsupported for this device.\n")));
  }

  /* Check possible flags. */
  if ((flags & ~possible_flags) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags specified (0x%x).\n"), flags & ~possible_flags));
  }

  /* If a match flag is present then check match_entry. */
  if ((flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) != 0) {
    BCMDNX_NULL_CHECK(match_entry);
  }

  /* An action must be specified. */
  if ((flags & _BCM_TRUNK_RESILIENT_ACTION_FLAGS) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No action specified.\n")));
  }

  /* We currently do not support matching the key. */
  if ((flags & BCM_TRUNK_RESILIENT_MATCH_HASH_KEY) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_TRUNK_MATCH_HASH_KEY (0x%x) is unsupported.\n"), BCM_TRUNK_RESILIENT_MATCH_HASH_KEY));
  }

  /* If the action is REPLACE then check replace_entry. */
  if (flags & BCM_TRUNK_RESILIENT_REPLACE) {
    BCMDNX_NULL_CHECK(replace_entry);
  }

  unit = (unit);

  SOC_PPC_SLB_CLEAR(&all_lag_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_LAG);
  SOC_PPC_SLB_CLEAR(&group_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG);
  SOC_PPC_SLB_CLEAR(&member_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG);
  SOC_PPC_SLB_CLEAR(&count_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_COUNT);
  SOC_PPC_SLB_CLEAR(&remove_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_REMOVE);
  SOC_PPC_SLB_CLEAR(&update_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_UPDATE);
  SOC_PPC_SLB_CLEAR(&update_value_lag_member, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_UPDATE_VALUE_LAG_MEMBER);

  /* If there are no match flags then match all. */
  if ((flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) == 0) {
    match_rules[0] = SOC_PPC_SLB_DOWNCAST(&all_lag_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules = 1;
  }

  if (flags & BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID) {
    group_match_rule.lag_ndx = match_entry->tid;

    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&group_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  if (flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) {
    member_match_rule.lag_member_ndx = (uint32)match_entry->member->gport;
    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&member_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  /* COUNT action. */
  if (flags & BCM_TRUNK_RESILIENT_COUNT) {
    action = SOC_PPC_SLB_DOWNCAST(&count_action, SOC_PPC_SLB_TRAVERSE_ACTION);

  /* DELETE action. */
  } else if (flags & BCM_TRUNK_RESILIENT_DELETE) {
    action = SOC_PPC_SLB_DOWNCAST(&remove_action, SOC_PPC_SLB_TRAVERSE_ACTION);

  /* REPLACE action. */
  } else if (flags & BCM_TRUNK_RESILIENT_REPLACE) {
    action = SOC_PPC_SLB_DOWNCAST(&update_action, SOC_PPC_SLB_TRAVERSE_ACTION);
    update_action.traverse_update_value = SOC_PPC_SLB_DOWNCAST(&update_value_lag_member, SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE);

    update_value_lag_member.new_lag_member_ndx = (uint32)replace_entry->member->gport;
  } else {
    BCMDNX_VERIFY(FALSE);
    action = NULL;
  }
  
  TRUNK_DB_LOCK(unit); /* { */

  soc_sand_rv = soc_ppd_slb_traverse(
      unit,
      match_rules,
      nof_match_rules,
      action,
      &num_entries_internal
    );

  /* } */ TRUNK_DB_UNLOCK(unit);

  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (num_entries) {
    *num_entries = num_entries_internal;
  }

exit:
  /* _BCM_DPP_SWITCH_API_END(unit); */
  BCMDNX_FUNC_RETURN;
}

/**
 * Wrapper around _bcm_petra_trunk_resilient_replace_by_index to convert from gport to index. 
 * gport as match rule -> match all indices containing this gport. 
 * gport as replace value -> replace all matches to the first member containing this gport. 
 */
int 
  bcm_petra_trunk_resilient_replace(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    int *num_entries, 
    bcm_trunk_resilient_entry_t *replace_entry
  )
{
  int rv;
  uint32 lag_member_ndx;
  SOC_PPD_LAG_INFO lag_info;
  bcm_trunk_resilient_entry_t match_entry_internal;
  bcm_trunk_resilient_entry_t replace_entry_internal;
  bcm_trunk_resilient_entry_t *replace_entry_ptr = NULL;
  bcm_trunk_member_t replace_member;
  bcm_trunk_member_t match_member;


  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  _BCM_DPP_SWITCH_API_START(unit);

  /* If a match flag or replace is present then check match_entry. */
  /* For matching, we have to have he match_entry. */
  /* For replace we need the match_entry to specify the group we replace the member to. */
  if ((flags & (_BCM_TRUNK_RESILIENT_MATCH_FLAGS | BCM_TRUNK_RESILIENT_REPLACE)) != 0) {
    BCMDNX_NULL_CHECK(match_entry);

    SOC_PPD_LAG_INFO_clear(&lag_info);

    /* Remark: replace_entry->tid is ignored! We assume the gport in replace_entry is relevant to the LAG match_entry->tid. */
    /*         Otherwise we might end up trying to replace the member of LAG A with the member of LAG B. */
    rv = soc_ppd_lag_get(unit, match_entry->tid, &lag_info);
    BCM_SAND_IF_ERR_EXIT(rv);

  }

  if (flags & BCM_TRUNK_RESILIENT_REPLACE) {
    BCMDNX_NULL_CHECK(replace_entry);
    replace_entry_internal = *replace_entry;
    replace_entry_ptr = &replace_entry_internal;
  }

  /* We currently do not support MEMBER without TRUNK_ID. */
  if (((flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) != 0) && ((flags & BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID) == 0)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
                     (_BSL_BCM_MSG("BCM_TRUNK_RESILIENT_MATCH_MEMBER is only allowed in combination with BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID.\n")));
  }

  TRUNK_CHECK_INIT(unit, ts_init);

  /* First find the index of the replace_entry gport and prepare replace_entry_internal if needed. */
  if (flags & BCM_TRUNK_RESILIENT_REPLACE) {
    uint32 replace_sys_port;

    rv = _bcm_petra_gport_to_sys_port(unit, replace_entry->member->gport, &replace_sys_port);
    BCMDNX_IF_ERR_EXIT(rv);

    for (lag_member_ndx = 0; lag_member_ndx < lag_info.nof_entries; lag_member_ndx++) {
      if (lag_info.members[lag_member_ndx].sys_port == replace_sys_port) {
        break;
      }
    }
    if (lag_member_ndx == lag_info.nof_entries) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No member of the LAG group %d contains gport %d.\n"), match_entry->tid, replace_entry->member->gport));
    }

    replace_entry_internal.member = &replace_member;
    replace_entry_internal.member->gport = lag_member_ndx;
  }

  if (flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) {
    uint32 match_sys_port;

    rv = _bcm_petra_gport_to_sys_port(unit, match_entry->member->gport, &match_sys_port);
    BCMDNX_IF_ERR_EXIT(rv);

    match_entry_internal = *match_entry;
    match_entry_internal.member = &match_member;
    if (num_entries) {
      *num_entries = 0;
    }

    /* We need to traverse all member indices with a matching gport. */
    for (lag_member_ndx = 0; lag_member_ndx < lag_info.nof_entries; lag_member_ndx++) {
      if (lag_info.members[lag_member_ndx].sys_port == match_sys_port) {
        int num_entries_internal;
        
        match_entry_internal.member->gport = lag_member_ndx;

        rv = _bcm_petra_trunk_resilient_replace_by_index(unit, flags, &match_entry_internal, &num_entries_internal, replace_entry_ptr);
        BCMDNX_IF_ERR_EXIT(rv);

        if (num_entries) {
          *num_entries += num_entries_internal;
        }
      }
    }
  } else {
    rv = _bcm_petra_trunk_resilient_replace_by_index(unit, flags, match_entry, num_entries, replace_entry_ptr);
    BCMDNX_IF_ERR_EXIT(rv);
  }

exit:
  _BCM_DPP_SWITCH_API_END(unit);
  BCMDNX_FUNC_RETURN;
}
#else
int 
  bcm_petra_trunk_resilient_traverse(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    bcm_trunk_resilient_traverse_cb trav_fn, 
    void *user_data
  )
{
  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  _BCM_DPP_SWITCH_API_START(unit);

  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_resilient_traverse is unsupported for this device.\n")));

exit:
  _BCM_DPP_SWITCH_API_END(unit);
  BCMDNX_FUNC_RETURN;
}

int 
  bcm_petra_trunk_resilient_replace(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    int *num_entries, 
    bcm_trunk_resilient_entry_t *replace_entry
  )
{
  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  _BCM_DPP_SWITCH_API_START(unit);

  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_resilient_replace is unsupported for this device.\n")));

exit:
  _BCM_DPP_SWITCH_API_END(unit);
  BCMDNX_FUNC_RETURN;
}
#endif /* BCM_88660_A0 */



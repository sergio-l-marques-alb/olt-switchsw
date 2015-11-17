/*
 * $Id: wb_db_cosq.c,v 1.59 Broadcom SDK $
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
 * Warmboot - Level 2 support (COSQ Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm/cosq.h>
#include <bcm/fabric.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/wb_db_cosq.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <soc/dpp/soc_sw_db.h>
#include <shared/shr_resmgr.h>
#include <shared/shr_template.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/petra_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <soc/dpp/mbcm.h>


#if defined(BCM_WARM_BOOT_SUPPORT)

static bcm_dpp_wb_cosq_info_t   *_dpp_wb_cosq_info_p[BCM_MAX_NUM_UNITS] = {0};


/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_cosq_layout_init(int unit, int version)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t  *wb_info; 
    int                      entry_size, total_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    wb_info->version = version;

    switch (version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            entry_size = sizeof(bcm_dpp_wb_cosq_voq_config_t) * BCM_DPP_WB_COSQ_MAX_VOQ_GROUPS(unit);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_voqs = BCM_DPP_WB_COSQ_MAX_VOQ_GROUPS(unit);
            wb_info->voq_off = 0;
            total_size += entry_size;

            entry_size = sizeof(bcm_dpp_wb_cosq_connector_config_t) * BCM_DPP_WB_COSQ_MAX_CONN_GROUPS(unit);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_conns = BCM_DPP_WB_COSQ_MAX_CONN_GROUPS(unit);
            wb_info->conn_off = total_size;
            total_size += entry_size;

            entry_size = sizeof(bcm_dpp_wb_cosq_se_config_t) * BCM_DPP_WB_COSQ_MAX_SE(unit);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_ses = BCM_DPP_WB_COSQ_MAX_SE(unit);
            wb_info->se_off = total_size;
            total_size += entry_size;

            entry_size = sizeof(bcm_dpp_wb_cosq_flow_config_t) * BCM_DPP_WB_COSQ_MAX_FLOW(unit);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_flows = BCM_DPP_WB_COSQ_MAX_FLOW(unit);
            wb_info->flow_off = total_size;
            total_size += entry_size;

            /* E2E Flow Control Templates */
            entry_size = sizeof(bcm_dpp_wb_cosq_e2e_fc_data_t) * SOC_TMC_SCH_LOW_FC_NOF_AVAIL_CONFS;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_e2e_fc_data = SOC_TMC_SCH_LOW_FC_NOF_AVAIL_CONFS;
            wb_info->e2e_fc_data_off = total_size;
            total_size += entry_size;

            /* CL Class Templates*/
            entry_size = sizeof(bcm_dpp_wb_cosq_class_data_t) * SOC_TMC_SCH_NOF_CLASS_TYPES;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_class_data = SOC_TMC_SCH_NOF_CLASS_TYPES;
            wb_info->class_data_off = total_size;
            total_size += entry_size;

            /* Ingress Rate Class Templates*/
            entry_size = sizeof(bcm_dpp_wb_cosq_ingr_rate_class_data_t) * (SOC_TMC_ITM_RATE_CLASS_MAX + 1);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_ingr_rate_class_data = (SOC_TMC_ITM_RATE_CLASS_MAX + 1);
            wb_info->ingr_rate_class_data_off = total_size;
            total_size += entry_size;

            /* VSQ Rate Class Templates */
            entry_size = sizeof(bcm_dpp_wb_cosq_vsq_rate_class_data_t) * (SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX + 1);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_vsq_rate_class_data = (SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX + 1);
            wb_info->vsq_rate_class_data_off = total_size;
            total_size += entry_size;

            /* Ingress Discount Class Templates */
            entry_size = sizeof(bcm_dpp_wb_cosq_ingr_discount_class_data_t) * (SOC_TMC_ITM_NOF_CR_DISCNT_CLS_NDXS);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_ingr_discount_class_data = (SOC_TMC_ITM_NOF_CR_DISCNT_CLS_NDXS);
            wb_info->ingr_discount_class_data_off = total_size;
            total_size += entry_size;

            /* Global State */
            entry_size = sizeof(bcm_dpp_wb_cosq_gbl_data_t);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->gbl_data_off = total_size;
            total_size += entry_size;

            /* IPF data*/
            entry_size = sizeof(bcm_dpp_wb_cosq_ipf_data_t);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->ipf_data_off = total_size;
            total_size += entry_size;

            /* PFC RX TYPE data*/
            entry_size = sizeof(bcm_dpp_cosq_pfc_rx_type_t) * SOC_MAX_NUM_PORTS;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->rx_pfc_map_info_off = total_size;
            total_size += entry_size;
            wb_info->size = total_size;            

            break;

        default:
            /* no other version supported */
            rc = BCM_E_INTERNAL;
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_voq_state(int unit)
{
    int                              rc = BCM_E_NONE;
    int                              nbr_entry;
    bcm_dpp_wb_cosq_info_t          *wb_info; 
    bcm_dpp_wb_cosq_voq_config_t    *voq_state;
    int                              base_qid = 0, is_non_contiguous, is_reserved;
    bcm_dpp_cosq_voq_config_t       *voq_config;
    bcm_gport_t                      voq_gport, physical_port;
    int                              is_multicast, is_unicast, is_isq, num_cos_levels, queue_type;
    bcm_dpp_cosq_config_t           *cosq_config;
    uint32                           flags;


    BCMDNX_INIT_FUNC_DEFS;
    /* NOTE: This function has to handle conditions where the s/w state is already intialized */
    /*       (e.g. default state that is allocated during init).                              */
    /*       One approach is during warmboot cosq_init() does no initialize the s/w state     */
    /*       and depends on restore for update of the state. State that never changes does    */
    /*       not have to be stored.                                                           */

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            voq_state = (bcm_dpp_wb_cosq_voq_config_t *)(wb_info->scache_ptr + wb_info->voq_off);

            /* read all voq entries */
            for (nbr_entry = 0; nbr_entry < wb_info->max_voqs;
                           nbr_entry++, voq_state++, base_qid += DPP_DEVICE_COSQ_COS_ALLOC_SZ) {

                /* check if voq group exists */
                if (voq_state->num_cos == 0) {
                    continue;
                }

                /* update s/w state of cosq module */
                voq_config = (bcm_dpp_cosq_voq_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->voq_hd), base_qid);
                if (voq_config == NULL) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("VoQ Config element cannot allocate memory\n")));
                }
                voq_config->num_cos = voq_state->num_cos;

                /* update s/w state of resource manager */
                rc = _bcm_petra_cosq_qid_type_get(unit, base_qid, &voq_gport,
                                                  &is_multicast, &is_unicast, &is_isq, &queue_type);
                BCMDNX_IF_ERR_EXIT(rc);

                /* retreive voq attributes */
                rc = bcm_petra_cosq_gport_get(unit, voq_gport, &physical_port, &num_cos_levels, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                is_non_contiguous = (flags & BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR) ? TRUE : FALSE;
                _bcm_petra_cosq_is_queue_reserved(unit, base_qid, &is_reserved);
                if (is_reserved == FALSE) {
                    rc = bcm_dpp_am_ingress_voq_allocate(unit, SHR_RES_ALLOC_WITH_ID,
                                  is_non_contiguous, voq_state->num_cos, queue_type, &base_qid);
                    BCMDNX_IF_ERR_EXIT(rc);
                }
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_conn_state(int unit)
{
    int                                    rc = BCM_E_NONE;
    int                                    nbr_entry, num_cos;
    bcm_dpp_wb_cosq_info_t                *wb_info; 
    bcm_dpp_wb_cosq_connector_config_t    *conn_state;
    int                                    base_cid = 0, is_non_contiguous, is_composite, conn_id;
    int                                    actual_base_cid;
    int                                    is_reserved;
    bcm_dpp_cosq_connector_config_t       *conn_config;
    bcm_gport_t                            conn_gport, physical_port;
    int                                    num_cos_levels;
    bcm_dpp_cosq_config_t                 *cosq_config;
    uint32                                 flags;
    uint8                                 src_modid = 0;
    bcm_dpp_wb_cosq_flow_config_t         *flow_state;
    bcm_dpp_cosq_flow_config_t            *flow_config;
    int                                   flow_region, is_odd_even;
    uint8                                 is_non_contiguous_uint8;


    BCMDNX_INIT_FUNC_DEFS;
    /* NOTE: This function has to handle conditions where the s/w state is already intialized */
    /*       (e.g. default state that is allocated during init).                              */
    /*       One approach is during warmboot cosq_init() does no initialize the s/w state     */
    /*       and depends on restore for update of the state. State that never changes does    */
    /*       not have to be stored.                                                           */
    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            conn_state = (bcm_dpp_wb_cosq_connector_config_t *)(wb_info->scache_ptr + wb_info->conn_off);
            flow_state = (bcm_dpp_wb_cosq_flow_config_t *)(wb_info->scache_ptr + wb_info->flow_off);

            /* read all connector entries */
            for (nbr_entry = 0; nbr_entry < wb_info->max_conns;
                           nbr_entry++, conn_state++, base_cid += DPP_DEVICE_COSQ_COS_ALLOC_SZ) {

                /* check if connector group exists */
                if (conn_state->num_cos == 0) {
                    continue;
                }

                /*Add an extra offset in non contiguous region*/
                flow_region = base_cid / 1024;
                rc = _bcm_petra_flow_region_config_get(unit, flow_region, &is_non_contiguous_uint8, &is_odd_even);
                BCMDNX_IF_ERR_EXIT(rc);
                actual_base_cid = is_non_contiguous_uint8 ? base_cid+2 : base_cid;

                /* update s/w state of cosq module */
                conn_config = (bcm_dpp_cosq_connector_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->connector_hd), actual_base_cid);
                if (conn_config == NULL) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Connection element cannot allocate memory\n")));
                }

                /* update connector s/w state */
                conn_config->num_cos = conn_state->num_cos; 
#ifdef BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION
                src_modid = conn_config->src_modid = conn_state->src_modid; 
#endif /* BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION */

                /* retreive connector attributes */
                BCM_COSQ_GPORT_VOQ_CONNECTOR_SET(conn_gport, actual_base_cid);
                rc = bcm_petra_cosq_gport_get(unit, conn_gport, &physical_port, &num_cos_levels, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                is_non_contiguous = (flags & BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR) ? TRUE : FALSE;
                is_composite = (flags & BCM_COSQ_GPORT_COMPOSITE) ? TRUE : FALSE;

                _bcm_petra_cosq_is_conn_reserved(unit, actual_base_cid, &is_reserved);
                if (is_reserved == FALSE) {
                    /* update s/w state of resource manager */
                    rc = bcm_dpp_am_cosq_scheduler_allocate(unit, SHR_RES_ALLOC_WITH_ID,
                                            is_composite, FALSE, FALSE, is_non_contiguous,
                                            conn_config->num_cos, SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR, &src_modid, &actual_base_cid);
                    BCMDNX_IF_ERR_EXIT(rc);
                }

                /* update flow state */
                for (num_cos = 0; num_cos < conn_config->num_cos; num_cos++) {

                    rc = _bcm_petra_flow_id_get(unit, actual_base_cid, num_cos, is_non_contiguous, is_composite, FALSE,&conn_id);
                    BCMDNX_IF_ERR_EXIT(rc);

                    flow_config = (bcm_dpp_cosq_flow_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->flow_hd), conn_id);
                    if (flow_config == NULL) {
                        BCM_EXIT;
                    }

                    flow_config->weight = (int) ((int16) _bcm_dpp_wb_load16(((uint8 *)&(flow_state + conn_id)->weight)));
                    flow_config->mode = (int) ((int16) _bcm_dpp_wb_load16(((uint8 *)&(flow_state + conn_id)->mode)));

                    if (is_composite) {
                        rc = _bcm_petra_composite_flow_id_get(unit, actual_base_cid, num_cos, is_non_contiguous, is_composite, &conn_id);
                        BCMDNX_IF_ERR_EXIT(rc);

                        flow_config = (bcm_dpp_cosq_flow_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->flow_hd), conn_id);
                        if (flow_config == NULL) {
                            BCM_EXIT;
                        }

                        flow_config->weight = (int)((int16)_bcm_dpp_wb_load16(((uint8 *)&(flow_state + conn_id)->weight)));
                        flow_config->mode = (int)((int16)_bcm_dpp_wb_load16(((uint8 *)&(flow_state + conn_id)->mode)));
                    }
                }

            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_se_state(int unit)
{
    int                                    rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                *wb_info; 
    bcm_dpp_cosq_config_t                 *cosq_config;
    bcm_dpp_wb_cosq_se_config_t           *se_state;
    bcm_dpp_cosq_se_config_t              *se_config;
    bcm_dpp_wb_cosq_flow_config_t         *flow_state;
    bcm_dpp_cosq_flow_config_t            *flow_config;
    int                                    se_id, dual_se_id = 0, flow_id, flow_id2;
    int                                    is_composite, is_dual, is_enhanced, is_reserved;
    bcm_gport_t                            se_gport, physical_port;
    uint8                                 *se_info = NULL;
    int                                    num_cos_levels;
    uint32                                 flags;


    BCMDNX_INIT_FUNC_DEFS;

    /* NOTE: This function has to handle conditions where the s/w state is already intialized */
    /*       (e.g. default state that is allocated during init).                              */
    /*       One approach is during warmboot cosq_init() does no initialize the s/w state     */
    /*       and depends on restore for update of the state. State that never changes does    */
    /*       not have to be stored.                                                           */
    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    BCMDNX_ALLOC(se_info, sizeof(uint8) * ((wb_info->max_ses / 8) + 1), "temp se info");
    if (se_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate temp memory")));
    }
    sal_memset(se_info, 0, sizeof(uint8) * ((wb_info->max_ses / 8) + 1));

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            se_state = (bcm_dpp_wb_cosq_se_config_t *)(wb_info->scache_ptr + wb_info->se_off);
            flow_state = (bcm_dpp_wb_cosq_flow_config_t *)(wb_info->scache_ptr + wb_info->flow_off);

            /* read all se entries */
            for (se_id = 0; se_id < wb_info->max_ses; se_id++) {

                /* check that the element is not already processed */
                if (se_info[(se_id / 8)] & (1 << (se_id % 8))) {
                    continue;
                }

                /* check if connector group exists */
                if ((se_state + se_id)->ref_cnt == 0) {
                    continue;
                }

                /* determine flow corresponding to this SE */
                flow_id = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_sch_se2flow_id,( se_id)));
                /* retreive SE attributes */
                BCM_GPORT_SCHEDULER_SET(se_gport, flow_id);

                rc = bcm_petra_cosq_gport_get(unit, se_gport, &physical_port, &num_cos_levels, &flags);
                if (rc != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "unit %d, failure in getting gport(0x%x) info, error 0x%x\n"), unit, se_gport, rc));
                    goto err;
                }

                is_composite = (flags & BCM_COSQ_GPORT_COMPOSITE) ? TRUE : FALSE;
                is_dual = (flags & BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER) ? TRUE : FALSE;

                /* update s/w state of resource manager */
                if (flags & DPP_DEVICE_COSQ_CL_MASK) {
                    if (flags & DPP_DEVICE_COSQ_HR_MASK) { /* determines dual {CL, HR} mode */
                        rc = bcm_dpp_am_cosq_scheduler_allocate(unit, SHR_RES_ALLOC_WITH_ID,
                                            is_composite, FALSE, is_dual, TRUE,
                                            1, SOC_TMC_AM_SCH_FLOW_TYPE_HR, NULL, &flow_id);
                    }
                    else {
                        is_enhanced = (flags & DPP_DEVICE_COSQ_CL_ENHANCED_MASK) ? TRUE: FALSE;
                        rc = bcm_dpp_am_cosq_scheduler_allocate(unit, SHR_RES_ALLOC_WITH_ID,
                                            is_composite, is_enhanced, is_dual, TRUE,
                                            1, SOC_TMC_AM_SCH_FLOW_TYPE_CL, NULL, &flow_id);
                    }
                }
                else if (flags & DPP_DEVICE_COSQ_HR_MASK) {
                    _bcm_petra_cosq_is_se_reserved(unit, se_id, &is_reserved);
                    if (is_reserved == FALSE) {
                        rc = bcm_dpp_am_cosq_scheduler_allocate(unit, SHR_RES_ALLOC_WITH_ID,
                                            is_composite, FALSE, is_dual, TRUE,
                                            1, SOC_TMC_AM_SCH_FLOW_TYPE_HR, NULL, &flow_id);
                    }
                }
                else if (flags & DPP_DEVICE_COSQ_FQ_MASK) {
                    rc = bcm_dpp_am_cosq_scheduler_allocate(unit, SHR_RES_ALLOC_WITH_ID,
                                            is_composite, FALSE, is_dual, TRUE,
                                            1, SOC_TMC_AM_SCH_FLOW_TYPE_FQ, NULL, &flow_id);
                }
                else {
                    rc = BCM_E_INTERNAL;
                }

                if (rc != BCM_E_NONE) {
                    goto err;
                }


                /* update s/w state of cosq module */
                if (is_composite) {
                    rc = _bcm_petra_se_composite_flow_id_get(unit, flow_id, SOC_TMC_SCH_SE_TYPE_NONE, TRUE, &flow_id2);

                    if (rc != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_INIT,
                                  (BSL_META_U(unit,
                                              "Unit(%d) failed to get sub-flow 2 for flow_id(%d)\n"), unit, flow_id));
                        goto err;
                    }
                }

                if (is_dual) {
                    rc = _bcm_petra_se_dual_flow_id_get(unit, flow_id, SOC_TMC_SCH_SE_TYPE_CL, TRUE, &flow_id2);
                    if (rc != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_INIT,
                                  (BSL_META_U(unit,
                                              "unit %d, cl se failure in getting dual flow(%d), error 0x%x\n"), unit, flow_id, rc));
                        goto err;
                    }

                    dual_se_id = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_sch_flow2se_id,( flow_id2)));
                    if (dual_se_id == DPP_COSQ_SCH_SE_ID_INVALID(unit)) {
                        rc = BCM_E_INTERNAL;
                        LOG_ERROR(BSL_LS_BCM_INIT,
                                  (BSL_META_U(unit,
                                              "unit %d, Invalid flow 0x%x specified - does not correspond to SE\n"), unit, flow_id2));
                        goto err;
                    }
                }

                se_config = (bcm_dpp_cosq_se_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->se_hd), se_id);
                if (se_config == NULL) {
                    goto err;
                }

                se_config->ref_cnt = (se_state + se_id)->ref_cnt;

                /* update temp state to indicate element processed */
                se_info[(se_id / 8)] |= (1 << (se_id % 8));

                flow_config = (bcm_dpp_cosq_flow_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->flow_hd), flow_id);
                if (flow_config == NULL) {
                    goto err;
                }

                flow_config->weight = (int) ((int16) _bcm_dpp_wb_load16(((uint8 *)&(flow_state + flow_id)->weight)));
                flow_config->mode = (int) ((int16)_bcm_dpp_wb_load16(((uint8 *)&(flow_state + flow_id)->mode)));

                if (is_dual) {
                    se_config = (bcm_dpp_cosq_se_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->se_hd), dual_se_id);
                    if (se_config == NULL) {
                        goto err;
                    }

                    se_config->ref_cnt = (se_state + dual_se_id) ->ref_cnt;

                    /* update temp state to indicate element processed */
                    se_info[(dual_se_id / 8)] |= (1 << (dual_se_id % 8));
                }

                if ( (is_composite) || (is_dual) ) {
                    flow_config = (bcm_dpp_cosq_flow_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->flow_hd), flow_id2);
                    if (flow_config == NULL) {
                        goto err;
                    }

                    flow_config->weight = (int) ((int16) _bcm_dpp_wb_load16(((uint8 *)&(flow_state + flow_id2)->weight)));
                    flow_config->mode = (int) ((int16) _bcm_dpp_wb_load16(((uint8 *)&(flow_state + flow_id2)->mode)));
                }
            }

            break;

        default:
            rc = BCM_E_INTERNAL;
            goto err;
            break;
    }

   BCM_FREE(se_info);

    BCMDNX_IF_ERR_EXIT(rc);
    BCM_EXIT;

err:

    if (se_info != NULL) {
       BCM_FREE(se_info);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_e2e_fc_data(int unit)
{
    int                             rc = BCM_E_NONE;
    uint32                          soc_sand_rc;
    int                             fap_port;
    SOC_TMC_SCH_PORT_INFO               port_info;


    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    /* populate other template related data structures */
    if (!SOC_DPP_CONFIG(unit)->tm.is_port_tc_enable) {
        SOC_TMC_SCH_PORT_INFO_clear(&port_info);
        for (fap_port = 0; fap_port <= _BCM_PETRA_NOF_TM_PORTS(unit); fap_port++) {
            soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_port_sched_get,(unit, fap_port, &port_info)));
            if (SOC_SAND_FAILURE(soc_sand_rc)) {
                rc = translate_sand_success_failure(soc_sand_rc);
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d, error in retreiving scheduler info port %d, soc_sand error 0x%x, error 0x%x\n"), unit, fap_port, soc_sand_rc, rc));
                BCMDNX_IF_ERR_EXIT(rc);
                BCM_EXIT;
            }
         
            rc = _bcm_dpp_am_template_cosq_port_hr_fc_profile_mapping_set(unit, fap_port, port_info.lowest_hp_class);
            BCMDNX_IF_ERR_EXIT(rc);
        } 
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_class_data(int unit)
{
    int                              rc = BCM_E_NONE;
    SOC_TMC_SCH_SE_CL_CLASS_TABLE       *cl_class_table = NULL;
    uint32                           soc_sand_rc, flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    int                              weights[SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS];
    int                              template, is_allocated, ref_count, i;
    bcm_dpp_wb_cosq_info_t          *wb_info; 
    bcm_dpp_wb_cosq_class_data_t    *class_state;
    bcm_dpp_cosq_sched_class_data_t  class_data;
    SOC_TMC_SCH_SE_CL_CLASS_INFO        *class_type;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    BCMDNX_ALLOC(cl_class_table, sizeof(SOC_TMC_SCH_SE_CL_CLASS_TABLE), "class table");
    if (cl_class_table == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, resource memory allocation failure"), unit));
    }

    SOC_TMC_SCH_SE_CL_CLASS_TABLE_clear(cl_class_table);
    sal_memset(weights, 0, (sizeof(int) * SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS));

    soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_class_type_params_table_get,(unit, cl_class_table)));
    if(SOC_SAND_FAILURE(soc_sand_rc)) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d, error in retreivng cl param table\n"), unit));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
    }

    /* read all template entries */
    for (template = 0; template < wb_info->max_class_data; template++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                class_state = (bcm_dpp_wb_cosq_class_data_t *)(wb_info->scache_ptr + wb_info->class_data_off);
                ref_count = (class_state + template)->ref_cnt;
                if (ref_count == 0) {
                    continue;
                }

                class_type = &cl_class_table->class_types[template];

                sal_memset(weights, 0, (sizeof(int) * SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS));
                for (i = 0; i < SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS; i++) {
                    weights[i] = class_type->weight[i];
                }

                DPP_DEVICE_COSQ_CL_CLASS_DATA_INIT(class_data, class_type->mode, class_type->enhanced_mode, class_type->weight_mode, weights);

                break;

            default:
                rc = BCM_E_INTERNAL;
                BCM_EXIT;
                break;
        }

        rc = _bcm_dpp_am_template_cosq_sched_class_allocate_group(unit, flags, (int *)&class_data, ref_count, &is_allocated, &template);
        BCMDNX_IF_ERR_EXIT(rc);

    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:

    BCM_FREE(cl_class_table);

    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_ingr_rate_class_data(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                   *wb_info;
    bcm_dpp_wb_cosq_ingr_rate_class_data_t   *ingr_rate_state;
    bcm_dpp_cosq_ingress_rate_class_info_t    ingress_rate_info, *ingress_rate_state;
    int                                       template, is_allocated, ref_count;
    uint32                                    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    for (template = 0; template < wb_info->max_ingr_rate_class_data; template++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ingr_rate_state = (bcm_dpp_wb_cosq_ingr_rate_class_data_t *)(wb_info->scache_ptr + wb_info->ingr_rate_class_data_off);

                ref_count = (ingr_rate_state + template)->ref_cnt;
                if (ref_count == 0) {
                    continue;
                }

                
                /*        mantained in state table and persistent storage.                   */
                /*        Following is a quick fix. The long term solution is to retreive    */
                /*        as much state from from h/w.                                       */
                ingress_rate_state = (bcm_dpp_cosq_ingress_rate_class_info_t *) &((ingr_rate_state + template)->ingress_rate_class_info);
                ingress_rate_info = (*ingress_rate_state);

                break;

            default:
                rc = BCM_E_INTERNAL;
                BCM_EXIT;
                break;
        }

        /* restore state  */
        rc = _bcm_dpp_am_template_queue_rate_cls_allocate_group(unit, flags, &ingress_rate_info, ref_count, &is_allocated, &template);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_vsq_rate_class_data(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                   *wb_info;
    bcm_dpp_wb_cosq_vsq_rate_class_data_t    *vsq_rate_state;
    bcm_dpp_cosq_vsq_rate_class_info_t        vsq_rate_class_info, *vsq_rate_class_state;
    int                                       template, is_allocated, ref_count;
    uint32                                    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    for (template = 0; template < wb_info->max_vsq_rate_class_data; template++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                vsq_rate_state = (bcm_dpp_wb_cosq_vsq_rate_class_data_t *)(wb_info->scache_ptr + wb_info->vsq_rate_class_data_off);

                ref_count = (vsq_rate_state + template)->ref_cnt;
                if (ref_count == 0) {
                    continue;
                }

                
                /*        mantained in state table and persistent storage.                   */
                /*        Following is a quick fix. The long term solution is to retreive    */
                /*        as much state from from h/w.                                       */
                vsq_rate_class_state = (bcm_dpp_cosq_vsq_rate_class_info_t *) &((vsq_rate_state + template)->wred_enable[0][0]);
                vsq_rate_class_info = (*vsq_rate_class_state);

                break;

            default:
                rc = BCM_E_INTERNAL;
                BCM_EXIT;
                break;
        }

        /* restore state  */
        rc = _bcm_dpp_am_template_vsq_rate_cls_allocate_group(unit, flags, &vsq_rate_class_info, ref_count, &is_allocated, &template);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_cosq_restore_ingr_discount_class_data(int unit)
{
    int                                          rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                      *wb_info;
    bcm_dpp_wb_cosq_ingr_discount_class_data_t  *ingr_discount_state;
    SOC_TMC_ITM_CR_DISCOUNT_INFO                 ingr_discnt_info, *ingr_discnt_state;
    int                                          template, is_allocated, ref_count;
    uint32                                       flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    for (template = 0; template < wb_info->max_ingr_discount_class_data; template++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ingr_discount_state = (bcm_dpp_wb_cosq_ingr_discount_class_data_t *)(wb_info->scache_ptr + wb_info->ingr_discount_class_data_off);

                ref_count = (ingr_discount_state + template)->ref_cnt;
                if (ref_count == 0) {
                    continue;
                }

                
                /*        mantained in state table and persistent storage.                   */
                /*        Following is a quick fix. The long term solution is to retreive    */
                /*        as much state from from h/w.                                       */
                ingr_discnt_state = (SOC_TMC_ITM_CR_DISCOUNT_INFO *) &((ingr_discount_state + template)->discount_class);
                ingr_discnt_info = (*ingr_discnt_state);

                break;

            default:
                rc = BCM_E_INTERNAL;
                BCM_EXIT;
                break;
        }

        /* restore state  */
        rc = _bcm_dpp_am_template_queue_discount_cls_allocate_group(unit, flags, &ingr_discnt_info, ref_count, &is_allocated, &template);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/* ARAD Only */
STATIC int
_bcm_dpp_wb_cosq_restore_tc_map_data(int unit)
{
    int                                    rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;



    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_gbl_state(int unit)
{
    int                                    rc = BCM_E_NONE, sand_rc;
    bcm_dpp_wb_cosq_info_t                *wb_info;
    bcm_dpp_wb_cosq_gbl_data_t            *gbl_state;
    int                                    vsq_category, ucast_qid_start, ucast_qid_end;
    SOC_SAND_U32_RANGE                     mcast_queue_range;
    SOC_TMC_ITM_INGRESS_SHAPE_INFO           isp_info;
    int                                   *queue_region_config = NULL;
    int                                    region, region_start, region_end;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    SOC_TMC_ITM_INGRESS_SHAPE_INFO_clear(&isp_info);

    /* Allocation manager configuration */
    BCMDNX_ALLOC(queue_region_config, sizeof(int) * DPP_DEVICE_ARAD_COSQ_CONFIG_QUEUE_REGIONS, "queue_region");
    if (queue_region_config == NULL) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d, resource memory allocation failure\n"), unit));
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unable to allocate memory for temp state")));
    }

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            gbl_state = (bcm_dpp_wb_cosq_gbl_data_t *)(wb_info->scache_ptr + wb_info->gbl_data_off);

            vsq_category = gbl_state->vsq_category_mode;
            ucast_qid_start = gbl_state->ucast_qid_start;
            ucast_qid_end = gbl_state->ucast_qid_end;

            break;

        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }

    /* set cosq state */
    rc = _bcm_petra_cosq_ucast_qid_range_set(unit, ucast_qid_start, ucast_qid_end);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_petra_cosq_fmq_vsq_category_mode_set(unit, (bcm_fabric_vsq_category_mode_t)vsq_category);
    BCMDNX_IF_ERR_EXIT(rc);

    /*
     * set resource manager state
     */
    sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_enhanced_get,(unit, &mcast_queue_range)));
    if (SOC_SAND_FAILURE(sand_rc)) {
        rc = translate_sand_success_failure(sand_rc);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_ingress_shape_get,(unit, &isp_info)));
    if (SOC_SAND_FAILURE(sand_rc)) {
        rc = translate_sand_success_failure(sand_rc);
        BCM_SAND_IF_ERR_EXIT(sand_rc);
    }

    /* unicast */
    if ( (ucast_qid_start >= 0) && (ucast_qid_end > 0) && (ucast_qid_end > ucast_qid_start) ) {
        rc = bcm_dpp_am_cosq_queue_region_config_get(unit, queue_region_config, DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST);
        BCMDNX_IF_ERR_EXIT(rc);

        region_start = ucast_qid_start / 1024;
        region_end = (ucast_qid_end + 1) / 1024;
        for (region = region_start; region < region_end; region++) {
            queue_region_config[region] = DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST;
        }

        rc = _bcm_dpp_am_cosq_process_queue_region(unit, queue_region_config);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    /* multicast */

    /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
    /* coverity[uninit_use:FALSE] */
    if ( !((mcast_queue_range.start == 0) && (mcast_queue_range.end == 0))  &&
          ((mcast_queue_range.start != DPP_DEVICE_COSQ_FMQ_NON_ENHANCED_QID_MIN) ||
           (mcast_queue_range.end != DPP_DEVICE_COSQ_FMQ_NON_ENHANCED_QID_MAX)) ) {
        rc = bcm_dpp_am_cosq_queue_region_config_get(unit, queue_region_config, DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST);
        BCMDNX_IF_ERR_EXIT(rc);

        region_start = mcast_queue_range.start / 1024;
        region_end = (mcast_queue_range.end + 1) / 1024;
        for (region = region_start; region < region_end; region++) {
            queue_region_config[region] = DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST;
        }
        rc = _bcm_dpp_am_cosq_process_queue_region(unit, queue_region_config);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    /* ISQ */ 
    if (isp_info.enable) {
        rc = bcm_dpp_am_cosq_queue_region_config_get(unit, queue_region_config, DPP_DEVICE_COSQ_QUEUE_REGION_ISQ);
        BCMDNX_IF_ERR_EXIT(rc);

        region_start = isp_info.q_range.q_num_low / 1024;
        region_end = ( isp_info.q_range.q_num_high + 1) / 1024;
        for (region = region_start; region < region_end; region++) {
            queue_region_config[region] = DPP_DEVICE_COSQ_QUEUE_REGION_ISQ;
        }
        rc = _bcm_dpp_am_cosq_process_queue_region(unit, queue_region_config);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if (queue_region_config != NULL) {
       BCM_FREE(queue_region_config);
    }

    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_cosq_restore_ipf_data(int unit)
{
    int                                  rc = BCM_E_NONE;
    uint32                               sand_rc;
    bcm_dpp_wb_cosq_info_t               *wb_info;
    bcm_dpp_wb_cosq_ipf_data_t           *ipf_data;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            ipf_data = (bcm_dpp_wb_cosq_ipf_data_t *)(wb_info->scache_ptr + wb_info->ipf_data_off);
            break;

        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    BCMDNX_IF_ERR_EXIT(rc);

    sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_ipf_mode_set, (unit, ipf_data->ipf_mode));
    BCM_SAND_IF_ERR_EXIT(sand_rc);  

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_pfc_rx_type_data(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t          *wb_info;
    bcm_dpp_cosq_config_t           *cosq_config;
    int                              size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            size = sizeof(bcm_dpp_cosq_pfc_rx_type_t) * SOC_MAX_NUM_PORTS;
            sal_memcpy(cosq_config->rx_pfc_map_info, wb_info->scache_ptr + wb_info->rx_pfc_map_info_off, size);

            break;

        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_cosq_restore_gbl_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_restore_voq_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_restore_conn_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_restore_ipf_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_restore_se_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_restore_e2e_fc_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_restore_class_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    /* rc = _bcm_dpp_wb_cosq_restore_egr_thres_data(unit); */
    /* BCMDNX_IF_ERR_EXIT(rc); */

    rc = _bcm_dpp_wb_cosq_restore_ingr_rate_class_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_restore_vsq_rate_class_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_restore_ingr_discount_class_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    /* rc = _bcm_dpp_wb_cosq_restore_egr_discount_class_data(unit); */
    /* BCMDNX_IF_ERR_EXIT(rc); */

    rc = _bcm_dpp_wb_cosq_restore_tc_map_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_restore_pfc_rx_type_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_voq_state(int unit)
{
    int                              rc = BCM_E_NONE;
    int                              nbr_entry;
    bcm_dpp_wb_cosq_info_t          *wb_info; 
    bcm_dpp_wb_cosq_voq_config_t    *voq_state;
    int                              base_qid = 0;
    bcm_dpp_cosq_voq_config_t       *voq_config;
    bcm_dpp_cosq_config_t           *cosq_config;


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nVoQ State\n")));

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            voq_state = (bcm_dpp_wb_cosq_voq_config_t *)(wb_info->scache_ptr + wb_info->voq_off);

            /* read all voq entries */
            for (nbr_entry = 0; nbr_entry < wb_info->max_voqs;
                             nbr_entry++, voq_state++, base_qid += DPP_DEVICE_COSQ_COS_ALLOC_SZ) {
                voq_config = (bcm_dpp_cosq_voq_config_t*)_bcm_petra_cosq_element_get(unit,
                                         &(cosq_config->voq_hd), base_qid);
                if (voq_config == NULL) {
                    voq_state->num_cos = 0;
                }
                else {
                    voq_state->num_cos = voq_config->num_cos;

                    if (voq_config->num_cos != 0) {
                        LOG_DEBUG(BSL_LS_BCM_INIT,
                                  (BSL_META_U(unit,
                                              "  baseVoq(%d), numCos(%d)\n"), base_qid, voq_state->num_cos));
                    }
                }
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_conn_state(int unit)
{
    int                                  rc = BCM_E_NONE;
    int                                  nbr_entry, num_cos;
    int                                  base_cid = 0, is_non_contiguous, is_composite, conn_id;
    int                                 actual_base_cid;
    bcm_dpp_wb_cosq_info_t              *wb_info; 
    bcm_dpp_cosq_config_t               *cosq_config;
    bcm_gport_t                          conn_gport, physical_port;
    bcm_dpp_wb_cosq_connector_config_t  *conn_state;
    bcm_dpp_cosq_connector_config_t     *conn_config;
    bcm_dpp_wb_cosq_flow_config_t       *flow_state;
    bcm_dpp_cosq_flow_config_t          *flow_config;
    int                                  num_cos_levels;
    uint32                               flags;
    int                                  flow_region, is_odd_even;
    uint8                               is_non_contiguous_uint8;


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nConnector State\n")));

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            conn_state = (bcm_dpp_wb_cosq_connector_config_t *)(wb_info->scache_ptr + wb_info->conn_off);
            flow_state = (bcm_dpp_wb_cosq_flow_config_t *)(wb_info->scache_ptr + wb_info->flow_off);

            /* read all connector entries */
            for (nbr_entry = 0; nbr_entry < wb_info->max_conns;
                         nbr_entry++,  conn_state++, base_cid += DPP_DEVICE_COSQ_COS_ALLOC_SZ) {


                /*Add an extra offset in non contiguous region*/
                flow_region = base_cid / 1024;
                rc = _bcm_petra_flow_region_config_get(unit, flow_region, &is_non_contiguous_uint8, &is_odd_even);
                BCMDNX_IF_ERR_EXIT(rc);
                actual_base_cid = is_non_contiguous_uint8 ? base_cid+2 : base_cid;

                conn_config = (bcm_dpp_cosq_connector_config_t *)_bcm_petra_cosq_element_get(unit,
                                                      &(cosq_config->connector_hd), actual_base_cid);
                if (conn_config == NULL) {
                    conn_state->num_cos = 0;
#ifdef BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION
                    conn_state->src_modid = 0;
#endif /* BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION */

                    continue;
                }

                conn_state->num_cos = conn_config->num_cos;
#ifdef BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION
                conn_state->src_modid = conn_config->src_modid;
#endif /* BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION */

                if (conn_config->num_cos != 0) {
                    LOG_DEBUG(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "  baseConn(%d), numCos(%d)\n"), actual_base_cid, conn_state->num_cos));
                }
                else {
                    continue;
                }

                /* retreive voq attributes */
                BCM_COSQ_GPORT_VOQ_CONNECTOR_SET(conn_gport, actual_base_cid);
                rc = bcm_petra_cosq_gport_get(unit, conn_gport, &physical_port, &num_cos_levels, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                is_non_contiguous = (flags & BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR) ? TRUE : FALSE;
                is_composite = (flags & BCM_COSQ_GPORT_COMPOSITE) ? TRUE : FALSE;

                /* update flow state */
                for (num_cos = 0; num_cos < conn_config->num_cos; num_cos++) {

                    rc = _bcm_petra_flow_id_get(unit, actual_base_cid, num_cos, is_non_contiguous, is_composite, FALSE,&conn_id);
                    BCMDNX_IF_ERR_EXIT(rc);

                    flow_config = (bcm_dpp_cosq_flow_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->flow_hd), conn_id);
                    if (flow_config == NULL) {
                        BCM_EXIT;
                    }

                    _bcm_dpp_wb_store16((int16)flow_config->weight, (uint8 *)&((flow_state + conn_id)->weight));
                    _bcm_dpp_wb_store16((int16)flow_config->mode, (uint8 *)&((flow_state + conn_id)->mode));

                    LOG_DEBUG(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "    cos(%d), flow(%d), weight(%d)\n"), num_cos, conn_id, _bcm_dpp_wb_load16((uint8 *)&((flow_state + conn_id)->weight)) ));

                    if (is_composite) {
                        rc = _bcm_petra_composite_flow_id_get(unit, actual_base_cid, num_cos, is_non_contiguous, is_composite, &conn_id);
                        BCMDNX_IF_ERR_EXIT(rc);

                        flow_config = (bcm_dpp_cosq_flow_config_t *)_bcm_petra_cosq_element_alloc(unit, &(cosq_config->flow_hd), conn_id);
                        if (flow_config == NULL) {
                            BCM_EXIT;
                        }

                        _bcm_dpp_wb_store16((int16)flow_config->weight, (uint8 *)&((flow_state + conn_id)->weight));
                        _bcm_dpp_wb_store16((int16)flow_config->mode, (uint8 *)&((flow_state + conn_id)->mode));

                        LOG_DEBUG(BSL_LS_BCM_INIT,
                                  (BSL_META_U(unit,
                                              "    cos(%d), composite-flow(%d), weight(%d)\n"), num_cos, conn_id, _bcm_dpp_wb_load16((uint8 *)&((flow_state + conn_id)->weight)) ));
                    }
                }
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_se_state(int unit)
{
    int                                  rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t              *wb_info;
    bcm_dpp_cosq_config_t               *cosq_config;
    bcm_dpp_wb_cosq_se_config_t         *se_state;
    bcm_dpp_cosq_se_config_t            *se_config;
    bcm_dpp_wb_cosq_flow_config_t       *flow_state;
    bcm_dpp_cosq_flow_config_t          *flow_config;
    int                                  se_id, dual_se_id=0, flow_id, flow_id2;
    int                                  is_composite, is_dual;
    bcm_gport_t                          se_gport, physical_port;
    uint8                               *se_info = NULL;
    int                                  num_cos_levels;
    uint32                               flags;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    BCMDNX_ALLOC(se_info, sizeof(uint8) * ((wb_info->max_ses / 8) + 1), "temp se info");
    if (se_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate temp memory")));
    }
    sal_memset(se_info, 0, sizeof(uint8) * ((wb_info->max_ses / 8) + 1));

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nSE State\n")));

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            se_state = (bcm_dpp_wb_cosq_se_config_t *)(wb_info->scache_ptr + wb_info->se_off);
            flow_state = (bcm_dpp_wb_cosq_flow_config_t *)(wb_info->scache_ptr + wb_info->flow_off);

            /* read all se entries */
            for (se_id = 0; se_id < wb_info->max_ses; se_id++) {

                /* check that the element is not already processed */
                if (se_info[(se_id / 8)] & (1 << (se_id % 8))) {
                    continue;
                }

                se_config = (bcm_dpp_cosq_se_config_t *)_bcm_petra_cosq_element_get(unit,
                                                            &(cosq_config->se_hd), se_id);

                /* check if connector group exists */
                if ( (se_config == NULL) || (se_config->ref_cnt == 0) ) {
                    (se_state + se_id)->ref_cnt = 0;
                    continue;
                }

                /* determine flow corresponding to this SE */
                flow_id = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_sch_se2flow_id,( se_id)));
                /* retreive SE attributes */
                BCM_GPORT_SCHEDULER_SET(se_gport, flow_id);

                rc = bcm_petra_cosq_gport_get(unit, se_gport, &physical_port, &num_cos_levels, &flags);
                if (rc != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "unit %d, failure in getting gport(0x%x) info, error 0x%x\n"), unit, se_gport, rc));
                    goto err;
                }

                is_composite = (flags & BCM_COSQ_GPORT_COMPOSITE) ? TRUE : FALSE;
                is_dual = (flags & BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER) ? TRUE : FALSE;

                if (is_composite) {
                    rc = _bcm_petra_se_composite_flow_id_get(unit, flow_id, SOC_TMC_SCH_SE_TYPE_NONE, TRUE, &flow_id2);

                    if (rc != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_INIT,
                                  (BSL_META_U(unit,
                                              "Unit(%d) failed to get sub-flow 2 for flow_id(%d)\n"), unit, flow_id));
                        goto err;
                    }
                }

                if (is_dual) {
                    rc = _bcm_petra_se_dual_flow_id_get(unit, flow_id, SOC_TMC_SCH_SE_TYPE_CL, TRUE, &flow_id2);
                    if (rc != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_INIT,
                                  (BSL_META_U(unit,
                                              "unit %d, cl se failure in getting dual flow(%d), error 0x%x\n"), unit, flow_id, rc));
                        goto err;
                    }

                    dual_se_id = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_sch_flow2se_id,( flow_id2)));
                    if (dual_se_id == DPP_COSQ_SCH_SE_ID_INVALID(unit)) {
                        rc = BCM_E_INTERNAL;
                        LOG_ERROR(BSL_LS_BCM_INIT,
                                  (BSL_META_U(unit,
                                              "unit %d, Invalid flow 0x%x specified - does not correspond to SE\n"), unit, flow_id2));
                        goto err;
                    }
                }

                (se_state + se_id)->ref_cnt = se_config->ref_cnt;

                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "  SE(%d), ref_cnt(%d)\n"), se_id,
                                      ((se_state + se_id)->ref_cnt - DPP_DEVICE_COSQ_RESOURCE_ALLOC_NREF_VALUE)));

                /* update temp state to indicate element processed */
                se_info[(se_id / 8)] |= (1 << (se_id % 8));

                flow_config = (bcm_dpp_cosq_flow_config_t *)_bcm_petra_cosq_element_get(unit,
                                                            &(cosq_config->flow_hd), flow_id);
                if (flow_config == NULL) {
                    goto err;
                }

                _bcm_dpp_wb_store16((int16)flow_config->weight, (uint8 *)&((flow_state + flow_id)->weight));
                _bcm_dpp_wb_store16((int16)flow_config->mode, (uint8 *)&((flow_state + flow_id)->mode));

                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "    flow(%d), weight(%d) mode(%d)\n"), flow_id, 
                                      (int) ((int16)_bcm_dpp_wb_load16((uint8 *)&((flow_state + flow_id)->weight))),
                           (int) ((int16)_bcm_dpp_wb_load16((uint8 *)&((flow_state + flow_id)->mode)))));


                if (is_dual) {
                    se_config = (bcm_dpp_cosq_se_config_t *)_bcm_petra_cosq_element_get(unit,
                                                            &(cosq_config->se_hd), dual_se_id);
                    if (se_config == NULL) {
                        goto err;
                    }
                    (se_state + dual_se_id)->ref_cnt =  se_config->ref_cnt;

                    LOG_DEBUG(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "  SE-Dual(%d), ref_cnt(%d)\n"), dual_se_id,
                                          ((se_state + dual_se_id)->ref_cnt - DPP_DEVICE_COSQ_RESOURCE_ALLOC_NREF_VALUE)));

                    /* update temp state to indicate element processed */
                    se_info[(dual_se_id / 8)] |= (1 << (dual_se_id % 8));
                }

                if ( (is_dual) || (is_composite) ) {
                    flow_config = (bcm_dpp_cosq_flow_config_t *)_bcm_petra_cosq_element_get(unit,
                                                            &(cosq_config->flow_hd), flow_id2);
                    if (flow_config == NULL) {
                        goto err;
                    }

                    _bcm_dpp_wb_store16((int16)flow_config->weight, (uint8 *)&((flow_state + flow_id2)->weight));
                    _bcm_dpp_wb_store16((int16)flow_config->mode, (uint8 *)&((flow_state + flow_id2)->mode));

                    LOG_DEBUG(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "    flow-2(%d), weight(%d) mode(%d)\n"), flow_id2, 
                                          (int) ((int16)_bcm_dpp_wb_load16((uint8 *)&((flow_state + flow_id2)->weight))),
                               (int) ((int16) _bcm_dpp_wb_load16((uint8 *)&((flow_state + flow_id2)->mode)))));

                }
            }

            break;

        default:
            rc = BCM_E_INTERNAL;
            goto err;
            break;
    }

    BCM_FREE(se_info);

    BCMDNX_IF_ERR_EXIT(rc);
    BCM_EXIT;

err:

    if (se_info != NULL) {
       BCM_FREE(se_info);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_e2e_fc_data(int unit)
{
    int                                  rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t              *wb_info;
    bcm_dpp_wb_cosq_e2e_fc_data_t       *e2e_fc_state;
    int                                  nbr_templates;
    uint32                               ref_count;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    /* read all se entries */
    for (nbr_templates = 0; nbr_templates < wb_info->max_e2e_fc_data; nbr_templates++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ref_count = 0;
                rc = _bcm_dpp_am_template_cosq_hr_fc_ref_get(unit, nbr_templates, &ref_count);
                if (rc != BCM_E_NONE) {
                    ref_count = 0;
                    rc = BCM_E_NONE;
                }

                e2e_fc_state = (bcm_dpp_wb_cosq_e2e_fc_data_t *)(wb_info->scache_ptr + wb_info->e2e_fc_data_off);
                (e2e_fc_state + nbr_templates)->ref_cnt = ref_count;

                break;

            default:
                rc = BCM_E_INTERNAL;
                BCM_EXIT;
                break;
        }
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_class_data(int unit)
{
    int                                  rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t              *wb_info;
    bcm_dpp_wb_cosq_class_data_t        *class_state;
    int                                  nbr_templates;
    uint32                               ref_count;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    for (nbr_templates = 0; nbr_templates < wb_info->max_class_data; nbr_templates++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ref_count = 0;
                rc = _bcm_dpp_am_template_cosq_sched_class_ref_get(unit, nbr_templates, &ref_count);
                if (rc != BCM_E_NONE) {
                    ref_count = 0;
                    rc = BCM_E_NONE;
                }

                class_state = (bcm_dpp_wb_cosq_class_data_t *)(wb_info->scache_ptr + wb_info->class_data_off);
                (class_state + nbr_templates)->ref_cnt = ref_count;

                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
                break;
        }

    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_ingr_rate_class_data(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                   *wb_info;
    bcm_dpp_wb_cosq_ingr_rate_class_data_t   *ingr_rate_state;
    bcm_dpp_cosq_ingress_rate_class_info_t    ingress_rate_info, *ingress_rate_state;
    int                                       nbr_templates;
    uint32                                    ref_count;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    for (nbr_templates = 0; nbr_templates < wb_info->max_ingr_rate_class_data; nbr_templates++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ref_count = 0;
                rc = _bcm_dpp_am_template_queue_rate_cls_ref_get(unit, nbr_templates, &ref_count);
                if (rc != BCM_E_NONE) {
                    ref_count = 0;
                    rc = BCM_E_NONE;
                }

                ingr_rate_state = (bcm_dpp_wb_cosq_ingr_rate_class_data_t *)(wb_info->scache_ptr + wb_info->ingr_rate_class_data_off);
                (ingr_rate_state + nbr_templates)->ref_cnt = ref_count;

                if (ref_count != 0) {
                    rc = _bcm_dpp_am_template_queue_rate_cls_tdata_get(unit, nbr_templates, &ingress_rate_info);
                    BCMDNX_IF_ERR_EXIT(rc);

                    
                    /*        mantained in state table and persistent storage.                   */
                    /*        Following is a quick fix. THe long term solution is to store only  */
                    /*        the state that cannot be easily re-created from h/w.               */
                    ingress_rate_state = (bcm_dpp_cosq_ingress_rate_class_info_t *) &((ingr_rate_state + nbr_templates)->ingress_rate_class_info);
                    (*ingress_rate_state) = ingress_rate_info;
                }

                break;

            default:
                rc = BCM_E_INTERNAL;
                BCM_EXIT;
                break;
        }
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_vsq_rate_class_data(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                   *wb_info;
    bcm_dpp_wb_cosq_vsq_rate_class_data_t    *vsq_rate_state;
    bcm_dpp_cosq_vsq_rate_class_info_t        vsq_rate_class_info, *vsq_rate_class_state;
    int                                       nbr_templates;
    uint32                                    ref_count;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    for (nbr_templates = 0; nbr_templates < wb_info->max_vsq_rate_class_data; nbr_templates++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ref_count = 0;
                rc = _bcm_dpp_am_template_vsq_rate_cls_ref_get(unit, nbr_templates, &ref_count);
                if (rc != BCM_E_NONE) {
                    ref_count = 0;
                    rc = BCM_E_NONE;
                }

                vsq_rate_state = (bcm_dpp_wb_cosq_vsq_rate_class_data_t *)(wb_info->scache_ptr + wb_info->vsq_rate_class_data_off);
                (vsq_rate_state + nbr_templates)->ref_cnt = ref_count;

                if (ref_count != 0) {
                    rc = _bcm_dpp_am_template_vsq_rate_cls_tdata_get(unit, nbr_templates, &vsq_rate_class_info);
                    BCMDNX_IF_ERR_EXIT(rc);

                    
                    /*        mantained in state table and persistent storage.                   */
                    /*        Following is a quick fix. THe long term solution is to store only  */
                    /*        the state that cannot be easily re-created from h/w.               */
                    vsq_rate_class_state = (bcm_dpp_cosq_vsq_rate_class_info_t *) &((vsq_rate_state + nbr_templates)->wred_enable[0][0]);
                    (*vsq_rate_class_state) = vsq_rate_class_info;
                }

                break;

            default:
                rc = BCM_E_INTERNAL;
                BCM_EXIT;
                break;
        }
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_cosq_save_ingr_discount_class_data(int unit)
{
    int                                         rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                     *wb_info;
    bcm_dpp_wb_cosq_ingr_discount_class_data_t *ingr_discount_state;
    SOC_TMC_ITM_CR_DISCOUNT_INFO                ingr_discnt_info, *ingr_discnt_state;
    int                                         nbr_templates;
    uint32                                      ref_count;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    for (nbr_templates = 0; nbr_templates < wb_info->max_ingr_discount_class_data; nbr_templates++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ref_count = 0;
                rc = _bcm_dpp_am_template_queue_discount_cls_ref_get(unit, nbr_templates, &ref_count);
                if (rc != BCM_E_NONE) {
                    ref_count = 0;
                    rc = BCM_E_NONE;
                }

                ingr_discount_state = (bcm_dpp_wb_cosq_ingr_discount_class_data_t *)(wb_info->scache_ptr + wb_info->ingr_discount_class_data_off);
                (ingr_discount_state + nbr_templates)->ref_cnt = ref_count;

                if (ref_count != 0) {
                    rc = _bcm_dpp_am_template_queue_discount_cls_tdata_get(unit, nbr_templates, &ingr_discnt_info);
                    BCMDNX_IF_ERR_EXIT(rc);

                    
                    /*        mantained in state table and persistent storage.                   */
                    /*        Following is a quick fix. THe long term solution is to store only  */
                    /*        the state that cannot be easily re-created from h/w.               */
                    ingr_discnt_state = (SOC_TMC_ITM_CR_DISCOUNT_INFO *) &((ingr_discount_state + nbr_templates)->discount_class);
                    (*ingr_discnt_state) = ingr_discnt_info;
                }

                break;

            default:
                rc = BCM_E_INTERNAL;
                BCM_EXIT;
                break;
        }
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/* ARAD Only */
STATIC int
_bcm_dpp_wb_cosq_save_tc_map_data(int unit)
{
    int                                    rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;



    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_gbl_state(int unit)
{
    int                                    rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                *wb_info;
    bcm_dpp_wb_cosq_gbl_data_t            *gbl_state;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    gbl_state = (bcm_dpp_wb_cosq_gbl_data_t *)(wb_info->scache_ptr + wb_info->gbl_data_off);

    rc = _bcm_petra_cosq_ucast_qid_range_get(unit, &(gbl_state->ucast_qid_start), &(gbl_state->ucast_qid_end));
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_petra_cosq_fmq_vsq_category_mode_get(unit, (bcm_fabric_vsq_category_mode_t *)&(gbl_state->vsq_category_mode));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_ipf_data(int unit)
{
    uint32                          soc_sand_rc;
    bcm_dpp_wb_cosq_info_t                *wb_info;
    bcm_dpp_wb_cosq_ipf_data_t            *ipf_data;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    ipf_data = (bcm_dpp_wb_cosq_ipf_data_t *)(wb_info->scache_ptr + wb_info->ipf_data_off);

    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_ipf_mode_get, (unit, &(ipf_data->ipf_mode)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_save_pfc_rx_type_data(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t          *wb_info;
    bcm_dpp_cosq_config_t           *cosq_config;
    int                              size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            size = sizeof(bcm_dpp_cosq_pfc_rx_type_t) * SOC_MAX_NUM_PORTS;
            sal_memcpy(wb_info->scache_ptr + wb_info->rx_pfc_map_info_off, cosq_config->rx_pfc_map_info, size);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_cosq_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_cosq_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_cosq_info_p[unit], sizeof(bcm_dpp_wb_cosq_info_t), "wb cosq");
        if (_dpp_wb_cosq_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate WB Cosq info memory\n")));
        }
    }

    sal_memset(_dpp_wb_cosq_info_p[unit], 0x00, sizeof(bcm_dpp_wb_cosq_info_t));

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_cosq_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_cosq_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_cosq_info_p[unit]);
        _dpp_wb_cosq_info_p[unit] = NULL;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Global functions
 *
 * The save functionality has been implemented
 *   - some consistency checks rather then blindly coping data structures
 *     Thus easier to debug, catch errors.
 *   - The above implementation aslo make its semetric with the the per
 *     API update.
 */

int
_bcm_dpp_wb_cosq_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t  *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if ( !(BCM_DPP_WB_COSQ_IS_DIRTY(unit)) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_cosq_save_voq_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_save_conn_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_save_se_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_save_e2e_fc_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_save_class_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    /* rc = _bcm_dpp_wb_cosq_save_egr_thres_data(unit); */
    /* BCMDNX_IF_ERR_EXIT(rc); */

    rc = _bcm_dpp_wb_cosq_save_ingr_rate_class_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_save_vsq_rate_class_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_save_ingr_discount_class_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    /* rc = _bcm_dpp_wb_cosq_save_egr_discount_class_data(unit); */
    /* BCMDNX_IF_ERR_EXIT(rc); */

    rc = _bcm_dpp_wb_cosq_save_tc_map_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_save_gbl_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_save_ipf_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_cosq_save_pfc_rx_type_data(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_COSQ_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_state_init(int unit, bcm_dpp_cosq_config_t *cosq_config)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_COSQ_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    bcm_dpp_wb_cosq_info_t  *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    rc = _bcm_dpp_wb_cosq_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    wb_info->cosq_config = cosq_config;

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetreive, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_cosq_layout_init(unit, recovered_ver);
        if (rc != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rc);
        }

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_cosq_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_cosq_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_cosq_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_cosq_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        if (already_exists == TRUE) {
            /* state should have been removed by Host. Stale State */
            rc = _bcm_dpp_wb_cosq_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
        else {
            /* Initialize the state to know value */
            rc = _bcm_dpp_wb_cosq_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }

    wb_info->init_done = TRUE;

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX(rc);
    if(rc != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              " Failed while executing the macro SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX.\n")));
    }
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_cosq_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_voq_state(int unit, int base_qid)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t          *wb_info; 
    bcm_dpp_wb_cosq_voq_config_t    *voq_state;
    bcm_dpp_cosq_voq_config_t       *voq_config;
    bcm_dpp_cosq_config_t           *cosq_config;
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            /* update framework cache and peristent storage                       */
            voq_config = (bcm_dpp_cosq_voq_config_t *)_bcm_petra_cosq_element_get(unit,
                                         &(cosq_config->voq_hd), base_qid);
            voq_state = (bcm_dpp_wb_cosq_voq_config_t *)(wb_info->scache_ptr + wb_info->voq_off);
            voq_state += (base_qid / DPP_DEVICE_COSQ_COS_ALLOC_SZ);

            if (voq_config == NULL) {
                voq_state->num_cos = 0;
            }
            else {
                voq_state->num_cos = voq_config->num_cos;
            }

            data_size = sizeof(bcm_dpp_wb_cosq_voq_config_t);
            data = (uint8 *)voq_state;
            offset = (uint32)(data - wb_info->scache_ptr);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * calling function passes parameters required for h/w configuration but not cached in s/w.
 * This avoids doing another h/w lookup.
 */
int
_bcm_dpp_wb_cosq_update_conn_all_state(int unit, int base_conn, int is_non_contiguous, int is_composite, int src_modid)
{
    int                                  rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t              *wb_info; 
    bcm_dpp_cosq_config_t               *cosq_config;
    soc_scache_handle_t                  wb_handle;
    bcm_dpp_wb_cosq_connector_config_t  *conn_state;
    bcm_dpp_cosq_connector_config_t     *conn_config;
    uint32                               data_size;
    uint8                               *data;
    int                                  offset;
    bcm_dpp_wb_cosq_flow_config_t       *flow_state;
    uint32                               flow_data_size = 0;
    uint8                              **flow_data = NULL;
    int                                  num_cos, index, nbr_flow_offsets = 0, *flow_offset = NULL;
    int                                  conn_id;

    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    /* allocate offsets for maximum flows. account for composite attribute */
    BCMDNX_ALLOC(flow_offset, sizeof(int) * DPP_DEVICE_COSQ_MAX_COS * 2, "flow offsets");
    if (flow_offset == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate flow offset memory\n")));
    }
    BCMDNX_ALLOC(flow_data, sizeof(uint8 *) * DPP_DEVICE_COSQ_MAX_COS * 2, "flow data");
    if ( flow_data == NULL ) {
        BCM_FREE(flow_offset);
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate flow data memory\n")));
    }

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            /* update framework cache and peristent storage                       */
            conn_config = (bcm_dpp_cosq_connector_config_t *)_bcm_petra_cosq_element_get(unit,
                                         &(cosq_config->connector_hd), base_conn);
            conn_state = (bcm_dpp_wb_cosq_connector_config_t *)(wb_info->scache_ptr + wb_info->conn_off);
            conn_state += (base_conn / DPP_DEVICE_COSQ_COS_ALLOC_SZ);

            if (conn_config == NULL) {
                conn_state->num_cos = 0;
#ifdef  BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION 
                conn_state->src_modid = 0;
#endif /* BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION */
            }
            else {
                conn_state->num_cos = conn_config->num_cos;
#ifdef BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION 
                conn_state->src_modid = src_modid;
#endif /* BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION */
            }

            data_size = sizeof(bcm_dpp_wb_cosq_connector_config_t);
            data = (uint8 *)conn_state;
            offset = (uint32)(data - wb_info->scache_ptr);

            /* for delete functionality flows are not updated. Flows are only updated */
            /* during creation of connector group. Thus flows are valid only if the   */
            /* resource (connector group / SE) exits.                                 */

            flow_data_size = sizeof(bcm_dpp_wb_cosq_flow_config_t);
            flow_state = (bcm_dpp_wb_cosq_flow_config_t *)(wb_info->scache_ptr + wb_info->flow_off);

            for (num_cos = 0; num_cos < conn_state->num_cos; num_cos++) {
                rc = _bcm_petra_flow_id_get(unit, base_conn, num_cos, is_non_contiguous, is_composite, FALSE,&conn_id);
                if (rc != BCM_E_NONE) {
                    goto err;
                }

                _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + conn_id)->weight));
                _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + conn_id)->mode));
                flow_data[nbr_flow_offsets] = (uint8 *)(flow_state + conn_id);
                flow_offset[nbr_flow_offsets] = (uint32)(flow_data[nbr_flow_offsets] - wb_info->scache_ptr);
                nbr_flow_offsets++;

                if (is_composite) {
                    rc = _bcm_petra_composite_flow_id_get(unit, base_conn, num_cos, is_non_contiguous, is_composite, &conn_id);
                    if (rc != BCM_E_NONE) {
                        goto err;
                    }

                    _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + conn_id)->weight));
                    _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + conn_id)->mode));
                    flow_data[nbr_flow_offsets] = (uint8 *)(flow_state + conn_id);
                    flow_offset[nbr_flow_offsets] = (uint32)(flow_data[nbr_flow_offsets] - wb_info->scache_ptr);
                    nbr_flow_offsets++;
                }

            }
            break;

        default:
            rc = BCM_E_INTERNAL;
            goto err;
            break;
    }

    /* connector group update */
    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    if (rc != SOC_E_NONE) {
        goto err;
    }

    /* flow update */
    for (index = 0; index < nbr_flow_offsets; index++) {
        rc = soc_scache_commit_specific_data(unit, wb_handle, flow_data_size, flow_data[index], flow_offset[index]);
        if (rc != SOC_E_NONE) {
            goto err;
        }
    }

    BCM_FREE(flow_offset);
    BCM_FREE(flow_data);

    BCMDNX_IF_ERR_EXIT(rc);
    BCM_EXIT;

err:
    if (flow_offset != NULL) {
       BCM_FREE(flow_offset);
    }
    if (flow_data != NULL) {
       BCM_FREE(flow_data);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_flow_state(int unit, int flow_id)
{
    int                                  rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t              *wb_info; 
    bcm_dpp_cosq_config_t               *cosq_config;
    soc_scache_handle_t                  wb_handle;
    bcm_dpp_wb_cosq_flow_config_t       *flow_state;
    uint32                               data_size;
    uint8                               *data;
    int                                  offset;
    bcm_dpp_cosq_flow_config_t          *flow_config;


    BCMDNX_INIT_FUNC_DEFS;
    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            flow_config = (bcm_dpp_cosq_flow_config_t *)_bcm_petra_cosq_element_get(unit,
                                                            &(cosq_config->flow_hd), flow_id);
            if (flow_config == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unable to get flow config element\n")));
            }

            data_size = sizeof(bcm_dpp_wb_cosq_flow_config_t);
            flow_state = (bcm_dpp_wb_cosq_flow_config_t *)(wb_info->scache_ptr + wb_info->flow_off);
            data = (uint8 *)(flow_state + flow_id);
            offset = (uint32)(data - wb_info->scache_ptr);

            _bcm_dpp_wb_store16((int16)flow_config->weight, (uint8 *)&((flow_state + flow_id)->weight));
            _bcm_dpp_wb_store16((int16)flow_config->mode, (uint8 *)&((flow_state + flow_id)->mode));
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown SCACHE Version\n")));
            break;
    }

    /* flow update */
    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_se_all_state(int unit, int se_id, int is_composite, int is_dual)
{   
    int                                  rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t              *wb_info; 
    bcm_dpp_cosq_config_t               *cosq_config; 
    soc_scache_handle_t                  wb_handle;
    bcm_dpp_wb_cosq_se_config_t         *se_state;
    uint32                               data_size;
    uint8                              **data = NULL;
    int                                  nbr_se_offsets = 0, *offset = NULL, dual_se_id = 0;
    bcm_dpp_cosq_se_config_t            *se_config;
    bcm_dpp_wb_cosq_flow_config_t       *flow_state;
    uint32                               flow_data_size = 0;
    uint8                              **flow_data = NULL;
    int                                  nbr_flow_offsets = 0, *flow_offset = NULL, flow_id, flow_id2;
    int                                  is_delete = FALSE, index;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    /* allocate offsets for maximum flows. account for composite attribute */
    BCMDNX_ALLOC(offset, sizeof(int) * 2, "se offsets");
    BCMDNX_ALLOC(data, sizeof(uint32 *) * 2, "se data");
    BCMDNX_ALLOC(flow_offset, sizeof(int) * 2, "flow offsets");
    BCMDNX_ALLOC(flow_data, sizeof(uint8 *) * 2, "flow data");

    if ( (offset == NULL) || (data == NULL) || (flow_offset == NULL) || (flow_data == NULL) ) {
        rc = BCM_E_MEMORY;
        goto err;
    }

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            se_config = (bcm_dpp_cosq_se_config_t *)_bcm_petra_cosq_element_get(unit,
                                                            &(cosq_config->se_hd), se_id);
            if ( (se_config == NULL) || (se_config->ref_cnt == 0) ) {
                is_delete = TRUE;
            }

            flow_id = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_sch_se2flow_id,( se_id)));

            if (is_dual) {
                rc = _bcm_petra_se_dual_flow_id_get(unit, flow_id, SOC_TMC_SCH_SE_TYPE_NONE, TRUE, &flow_id2);
                if (rc != BCM_E_NONE) {
                   goto err;
                }

                dual_se_id = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_sch_flow2se_id,( flow_id2)));
                if (dual_se_id == DPP_COSQ_SCH_SE_ID_INVALID(unit)) {
                    rc = BCM_E_INTERNAL;
                    LOG_ERROR(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "unit %d, Invalid flow 0x%x specified - does not correspond to SE\n"), unit, flow_id2));
                    goto err;
                }
            }

            if (is_composite) {
                rc = _bcm_petra_se_composite_flow_id_get(unit, flow_id, SOC_TMC_SCH_SE_TYPE_NONE, TRUE, &flow_id2);

                if (rc != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "Unit(%d) failed to get sub-flow 2 for flow_id(%d)\n"), unit, flow_id));
                    goto err;
                }
            }

            /* save se state */
            data_size = sizeof(bcm_dpp_wb_cosq_se_config_t);

            se_state = (bcm_dpp_wb_cosq_se_config_t *)(wb_info->scache_ptr + wb_info->se_off);
            (se_state + se_id)->ref_cnt = (is_delete == TRUE) ? 0 : DPP_DEVICE_COSQ_RESOURCE_ALLOC_NREF_VALUE;
            data[nbr_se_offsets] = (uint8 *)(se_state + se_id);
            offset[nbr_se_offsets] = (uint32)(data[nbr_se_offsets] - wb_info->scache_ptr);
            nbr_se_offsets++;

            if (is_dual) {
                (se_state + dual_se_id)->ref_cnt = (is_delete == TRUE) ? 0 : DPP_DEVICE_COSQ_RESOURCE_ALLOC_NREF_VALUE;
                data[nbr_se_offsets] = (uint8 *)(se_state + dual_se_id);
                offset[nbr_se_offsets] = (uint32)(data[nbr_se_offsets] - wb_info->scache_ptr);
                nbr_se_offsets++;
            }

            /* save corresponding flow state */
            if (is_delete == TRUE) {
                /* no flow upate required for delete operation */
                break;
            }

            flow_data_size = sizeof(bcm_dpp_wb_cosq_flow_config_t);
            flow_state = (bcm_dpp_wb_cosq_flow_config_t *)(wb_info->scache_ptr + wb_info->flow_off);

            _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + flow_id)->weight));
            _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + flow_id)->mode));
            flow_data[nbr_flow_offsets] = (uint8 *)(flow_state + flow_id);
            flow_offset[nbr_flow_offsets] = (uint32)(flow_data[nbr_flow_offsets] - wb_info->scache_ptr);
            nbr_flow_offsets++;

            if (is_dual) {
                _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + flow_id2)->weight));
                _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + flow_id2)->mode));
                flow_data[nbr_flow_offsets] = (uint8 *)(flow_state + flow_id2);
                flow_offset[nbr_flow_offsets] = (uint32)(flow_data[nbr_flow_offsets] - wb_info->scache_ptr);
                nbr_flow_offsets++;
            }

            if (is_composite) {
                _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + flow_id2)->weight));
                _bcm_dpp_wb_store16(0, (uint8 *)&((flow_state + flow_id2)->mode));
                flow_data[nbr_flow_offsets] = (uint8 *)(flow_state + flow_id2);
                flow_offset[nbr_flow_offsets] = (uint32)(flow_data[nbr_flow_offsets] - wb_info->scache_ptr);
                nbr_flow_offsets++;
            }

            break;

        default:
            rc = BCM_E_INTERNAL;
            goto err;
            break;
    }

    /* update persistent storage */
    for (index = 0; index < nbr_se_offsets; index++) {
        rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data[index], offset[index]);
        if (rc != SOC_E_NONE) {
            goto err;
        }
    }

    for (index = 0; index < nbr_flow_offsets; index++) {
        rc = soc_scache_commit_specific_data(unit, wb_handle, flow_data_size, flow_data[index], flow_offset[index]);
        if (rc != SOC_E_NONE) {
            goto err;
        }
    }

    BCM_FREE(offset);
    BCM_FREE(data);
    BCM_FREE(flow_offset);
    BCM_FREE(flow_data);

    BCMDNX_IF_ERR_EXIT(rc);
    BCM_EXIT;

err:
    if (offset != NULL) {
       BCM_FREE(offset);
    }
    if (data != NULL) {
       BCM_FREE(data);
    }
    if (flow_offset != NULL) {
       BCM_FREE(flow_offset);
    }
    if (flow_data != NULL) {
       BCM_FREE(flow_data);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_se_state(int unit, int se_id, uint32 ref_cnt)
{
    int                                  rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t              *wb_info;
    bcm_dpp_cosq_config_t               *cosq_config;
    soc_scache_handle_t                  wb_handle;
    bcm_dpp_wb_cosq_se_config_t         *se_state;
    uint32                               data_size;
    uint8                               *data;
    int                                  offset;
    bcm_dpp_cosq_se_config_t            *se_config;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;


    se_config = (bcm_dpp_cosq_se_config_t *)_bcm_petra_cosq_element_get(unit,
                                                            &(cosq_config->se_hd), se_id);
    if (se_config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("error retreiving se (%d) info"), se_id));
    }

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:

            data_size = sizeof(bcm_dpp_wb_cosq_se_config_t);
            se_state = (bcm_dpp_wb_cosq_se_config_t *)(wb_info->scache_ptr + wb_info->se_off);
            se_state += se_id;
            se_state->ref_cnt = ref_cnt; /* se_config->ref_cnt */

            data = (uint8 *)(se_state);
            offset = (uint32)(data - wb_info->scache_ptr);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
            break;
    }

    /* se update */
    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_e2e_fc_data(int unit, int flags, int id1, int id2)
{
    int                                  rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t              *wb_info;
    soc_scache_handle_t                  wb_handle;
    int                                  id[2], no_ids = 0, index;
    bcm_dpp_wb_cosq_e2e_fc_data_t       *e2e_fc_state;
    uint32                               data_size, ref_count;
    uint8                               *data;
    int                                  offset;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    /* process id1 */
    id[no_ids++] = id1;

    /* if required process id2 */
    if (id2 != id1) {
        id[no_ids++] = id2;
    }

    for (index = 0; index < no_ids; index++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ref_count = 0;
                rc = _bcm_dpp_am_template_cosq_hr_fc_ref_get(unit, id[index], &ref_count);
                if (rc != BCM_E_NONE) {
                    ref_count = 0;
                    rc = BCM_E_NONE;
                }

                e2e_fc_state = (bcm_dpp_wb_cosq_e2e_fc_data_t *)(wb_info->scache_ptr + wb_info->e2e_fc_data_off);
                (e2e_fc_state + id[index])->ref_cnt = ref_count;

                data = (uint8 *)(e2e_fc_state + id[index]);
                offset = (uint32)(data - wb_info->scache_ptr);
                data_size = sizeof(bcm_dpp_wb_cosq_e2e_fc_data_t);

                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
                break;
        }

        rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_class_data(int unit, int flags, int id1, int id2)
{
    int                                  rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t              *wb_info;
    soc_scache_handle_t                  wb_handle;
    int                                  id[2], no_ids = 0, index;
    bcm_dpp_wb_cosq_class_data_t        *class_state;
    uint32                               data_size, ref_count;
    uint8                               *data;
    int                                  offset;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    /* process id1 */
    id[no_ids++] = id1;

    /* if required process id2 */
    if (id2 != id1) {
        id[no_ids++] = id2;
    }

    for (index = 0; index < no_ids; index++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ref_count = 0;
                rc = _bcm_dpp_am_template_cosq_sched_class_ref_get(unit, id[index], &ref_count);
                if (rc != BCM_E_NONE) {
                    ref_count = 0;
                    rc = BCM_E_NONE;
                }

                class_state = (bcm_dpp_wb_cosq_class_data_t *)(wb_info->scache_ptr + wb_info->class_data_off);
                (class_state + id[index])->ref_cnt = ref_count;

                data = (uint8 *)(class_state + id[index]);
                offset = (uint32)(data - wb_info->scache_ptr);
                data_size = sizeof(bcm_dpp_wb_cosq_class_data_t);

                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
                break;
        }

        rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_ingr_rate_class_data(int unit, int flags, int id1, int id2)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                   *wb_info;
    bcm_dpp_wb_cosq_ingr_rate_class_data_t   *ingr_rate_state;
    bcm_dpp_cosq_ingress_rate_class_info_t    ingress_rate_info, *ingress_rate_state;
    soc_scache_handle_t                       wb_handle;
    int                                       id[2], no_ids = 0, index;
    uint32                                    data_size, ref_count;
    uint8                                    *data;
    int                                       offset;
#ifdef BCM_ARAD_SUPPORT
    uint8                                     queue_to_rate_class_mapping_is_simple = TRUE;
#endif /*BCM_ARAD_SUPPORT*/

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_ARAD_SUPPORT
    BCM_SAND_IF_ERR_EXIT(arad_sw_db_tm_queue_to_rate_class_mapping_is_simple_get(unit, &queue_to_rate_class_mapping_is_simple));
    if (SOC_IS_ARAD(unit) && queue_to_rate_class_mapping_is_simple) /*template mngr is relevent only for simples queue to rate cls mapping mode*/
#endif /*BCM_ARAD_SUPPORT*/
    {
        if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
            BCM_EXIT;
        }

        /* check if immediate sync not enabled  */
        if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
            /* mark dirty state. Currently no other processing done           */
            /* if required framework cache could be updated without updating  */
            /* the persistent storage                                         */
            BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
            BCM_EXIT;
        }

        SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

        wb_info = BCM_DPP_WB_COSQ_INFO(unit);

        /* process id1 */
        id[no_ids++] = id1;

        /* if required process id2 */
        if (id2 != id1) {
            id[no_ids++] = id2;
        }

        for (index = 0; index < no_ids; index++) {
            switch(wb_info->version) {
                case BCM_DPP_WB_COSQ_VERSION_1_0:
                    ref_count = 0;
                    rc = _bcm_dpp_am_template_queue_rate_cls_ref_get(unit, id[index], &ref_count);
                    if (rc != BCM_E_NONE) {
                        ref_count = 0;
                        rc = BCM_E_NONE;
                    }

                    ingr_rate_state = (bcm_dpp_wb_cosq_ingr_rate_class_data_t *)(wb_info->scache_ptr + wb_info->ingr_rate_class_data_off);
                    (ingr_rate_state + id[index])->ref_cnt = ref_count;

                    if (ref_count != 0) {
                        rc = _bcm_dpp_am_template_queue_rate_cls_tdata_get(unit, id[index], &ingress_rate_info);
                        BCMDNX_IF_ERR_EXIT(rc);

                        
                        /*        mantained in state table and persistent storage.                   */
                        /*        Following is a quick fix. THe long term solution is to store only  */
                        /*        the state that cannot be easily re-created from h/w.               */
                        ingress_rate_state = (bcm_dpp_cosq_ingress_rate_class_info_t *) &((ingr_rate_state + id[index])->ingress_rate_class_info);
                        (*ingress_rate_state) = ingress_rate_info;
                    }

                    data = (uint8 *)(ingr_rate_state + id[index]);
                    offset = (uint32)(data - wb_info->scache_ptr);
                    data_size = sizeof(bcm_dpp_wb_cosq_ingr_rate_class_data_t);

                    break;

                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
                    break;
            }

            rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_vsq_rate_class_data(int unit, int flags, int id1, int id2)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                   *wb_info;
    bcm_dpp_wb_cosq_vsq_rate_class_data_t    *vsq_rate_state;
    bcm_dpp_cosq_vsq_rate_class_info_t        vsq_rate_class_info, *vsq_rate_class_state;
    soc_scache_handle_t                       wb_handle;
    int                                       id[2], no_ids = 0, index;
    uint32                                    data_size, ref_count;
    uint8                                    *data;
    int                                       offset;



    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    /* process id1 */
    id[no_ids++] = id1;

    /* if required process id2 */
    if (id2 != id1) {
        id[no_ids++] = id2;
    }

    for (index = 0; index < no_ids; index++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ref_count = 0;
                rc = _bcm_dpp_am_template_vsq_rate_cls_ref_get(unit, id[index], &ref_count);
                if (rc != BCM_E_NONE) {
                    ref_count = 0;
                    rc = BCM_E_NONE;
                }

                vsq_rate_state = (bcm_dpp_wb_cosq_vsq_rate_class_data_t *)(wb_info->scache_ptr + wb_info->vsq_rate_class_data_off);
                (vsq_rate_state + id[index])->ref_cnt = ref_count;

                if (ref_count != 0) {
                    rc = _bcm_dpp_am_template_vsq_rate_cls_tdata_get(unit, id[index], &vsq_rate_class_info);
                    BCMDNX_IF_ERR_EXIT(rc);

                    
                    /*        mantained in state table and persistent storage.                   */
                    /*        Following is a quick fix. THe long term solution is to store only  */
                    /*        the state that cannot be easily re-created from h/w.               */
                    vsq_rate_class_state = (bcm_dpp_cosq_vsq_rate_class_info_t *) &((vsq_rate_state + id[index])->wred_enable[0][0]);
                    (*vsq_rate_class_state) = vsq_rate_class_info;
                }

                data = (uint8 *)(vsq_rate_state + id[index]);
                offset = (uint32)(data - wb_info->scache_ptr);
                data_size = sizeof(bcm_dpp_wb_cosq_vsq_rate_class_data_t);

                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
                break;
        }

        rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_cosq_update_ingr_discount_class_data(int unit, int flags, int id1, int id2)
{
    int                                         rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                     *wb_info;
    bcm_dpp_wb_cosq_ingr_discount_class_data_t *ingr_discount_state;
    SOC_TMC_ITM_CR_DISCOUNT_INFO                ingr_discnt_info, *ingr_discnt_state;
    soc_scache_handle_t                         wb_handle;
    int                                         id[2], no_ids = 0, index;
    uint32                                      data_size, ref_count;
    uint8                                      *data;
    int                                         offset;


    BCMDNX_INIT_FUNC_DEFS;


    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    /* process id1 */
    id[no_ids++] = id1;

    /* if required process id2 */
    if (id2 != id1) {
        id[no_ids++] = id2;
    }

    for (index = 0; index < no_ids; index++) {
        switch(wb_info->version) {
            case BCM_DPP_WB_COSQ_VERSION_1_0:
                ref_count = 0;
                rc = _bcm_dpp_am_template_queue_discount_cls_ref_get(unit, id[index], &ref_count);
                if (rc != BCM_E_NONE) {
                    ref_count = 0;
                    rc = BCM_E_NONE;
                }

                ingr_discount_state = (bcm_dpp_wb_cosq_ingr_discount_class_data_t *)(wb_info->scache_ptr + wb_info->ingr_discount_class_data_off);
                (ingr_discount_state+ id[index])->ref_cnt = ref_count;

                if (ref_count != 0) {
                    rc = _bcm_dpp_am_template_queue_discount_cls_tdata_get(unit, id[index], &ingr_discnt_info);
                    BCMDNX_IF_ERR_EXIT(rc);

                    
                    /*        mantained in state table and persistent storage.                   */
                    /*        Following is a quick fix. THe long term solution is to store only  */
                    /*        the state that cannot be easily re-created from h/w.               */
                    ingr_discnt_state = (SOC_TMC_ITM_CR_DISCOUNT_INFO*) &((ingr_discount_state + id[index])->discount_class);
                    (*ingr_discnt_state) = ingr_discnt_info;
                }

                data = (uint8 *)(ingr_discount_state + id[index]);
                offset = (uint32)(data - wb_info->scache_ptr);
                data_size = sizeof(bcm_dpp_wb_cosq_ingr_discount_class_data_t);

                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
                break;
        }

        rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/* ARAD Only */
int
_bcm_dpp_wb_cosq_update_tc_map_data(int unit, int flags, int id1, int id2)
{
    int                                    rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;



    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_vsq_data(int unit, int category_mode)
{
    int                                    rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                *wb_info;
    bcm_dpp_wb_cosq_gbl_data_t            *gbl_state;
    soc_scache_handle_t                    wb_handle;
    uint32                                 data_size;
    uint8                                 *data;
    int                                    offset;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            gbl_state = (bcm_dpp_wb_cosq_gbl_data_t *)(wb_info->scache_ptr + wb_info->gbl_data_off);
            gbl_state->vsq_category_mode = category_mode;

            data = (uint8 *)&(gbl_state->vsq_category_mode);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(int32);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_cosq_update_ipf_mode(int unit, SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE ipf_mode)
{
    int                                    rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                *wb_info;
    bcm_dpp_wb_cosq_ipf_data_t            *ipf_data;
    soc_scache_handle_t                    wb_handle;
    uint32                                 data_size;
    uint8                                 *data;
    int                                    offset;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            ipf_data = (bcm_dpp_wb_cosq_ipf_data_t *)(wb_info->scache_ptr + wb_info->ipf_data_off);
            ipf_data->ipf_mode = ipf_mode;

            data = (uint8 *)&(ipf_data->ipf_mode);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(ipf_data->ipf_mode);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_cosq_update_ucast_data(int unit, int qid_start, int qid_end)
{
    int                                    rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                *wb_info;
    bcm_dpp_wb_cosq_gbl_data_t            *gbl_state;
    soc_scache_handle_t                    wb_handle;
    uint32                                 data_size;
    uint8                                 *data;
    int                                    offset;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            gbl_state = (bcm_dpp_wb_cosq_gbl_data_t *)(wb_info->scache_ptr + wb_info->gbl_data_off);
            gbl_state->ucast_qid_start = qid_start;
            gbl_state->ucast_qid_end = qid_end;

            data = (uint8 *)&(gbl_state->ucast_qid_start);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(int32) + sizeof(int32);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_cosq_update_pfc_rx_type_data(int unit, bcm_port_t port)
{
    int                                    rc = BCM_E_NONE;
    bcm_dpp_wb_cosq_info_t                *wb_info;
    bcm_dpp_cosq_config_t                 *cosq_config;
    bcm_dpp_cosq_pfc_rx_type_t            *pfc_rx_type;
    soc_scache_handle_t                    wb_handle;
    uint32                                 data_size;
    uint8                                 *data;
    int                                    offset;


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COSQ_INFO(unit);
    cosq_config = wb_info->cosq_config;

    if (!(BCM_DPP_WB_COSQ_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COSQ, 0);

    switch(wb_info->version) {
        case BCM_DPP_WB_COSQ_VERSION_1_0:
            pfc_rx_type = (bcm_dpp_cosq_pfc_rx_type_t *)(wb_info->scache_ptr + wb_info->rx_pfc_map_info_off);
            *pfc_rx_type = cosq_config->rx_pfc_map_info[port];

            data = (uint8 *)pfc_rx_type;
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(bcm_dpp_cosq_pfc_rx_type_t);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unsupported version (0x%x)"), wb_info->version));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Routines to make code Independent of Host processor alignment constaints
 */  

uint16
_bcm_dpp_wb_load16(uint8 *ptr)
{
    return((ptr[1]<<8) + ptr[0]);
}

uint32
_bcm_dpp_wb_load32(uint8 *ptr)
{
    return((ptr[3] << 24) + (ptr[2] << 16) + (ptr[1] << 8) + ptr[0]);
}

void
_bcm_dpp_wb_store16(uint16 value, uint8 *ptr)
{
    ptr[0] = ((value >> 0) & 0xFF);
    ptr[1] = ((value >> 8) & 0xFF);
}

void
_bcm_dpp_wb_store32(uint32 value, uint8 *ptr)
{
    ptr[0] = ((value >> 0) & 0xFF);
    ptr[1] = ((value >> 8) & 0xFF);
    ptr[2] = ((value >> 16) & 0xFF);
    ptr[3] = ((value >> 24) & 0xFF);
}

#endif /* BCM_WARM_BOOT_SUPPORT */


/* 
 * $Id: rx.c,v 1.131 Broadcom SDK $
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
 * File:    tx.c
 * Purpose: Implementation of bcm_rx* API for dune devices
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_RX

#include <shared/bsl.h>


#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>

#include <bcm_int/control.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/common/tx.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/oam.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/common/debug.h>

#include <shared/shr_resmgr.h>

#include <soc/dcmn/dcmn_wb.h>

#include <soc/dpp/mbcm.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/headers.h>
#include <soc/dpp/TMC/tmc_api_stack.h>
#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lag.h>

#include <soc/dpp/Petra/PB_TM/pb_api_action_cmd.h>

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_action_cmd.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_ports.h>

#endif

#include <shared/shr_template.h>

/* #define ARAD_RX_COSQ_MODE_ENABLE */


#define DPP_RX_START_CHECK(unit) \
        BCM_DPP_UNIT_CHECK(unit);\
        if(!RX_UNIT_STARTED(unit)) { return BCM_E_INIT; }

/* DPP_RX_INIT_CHECK is different from RX_INIT_CHECK (which calls bcm_petra_rx_init 
 * when RX module is not initialized yet)
 */
#define DPP_RX_INIT_CHECK(unit) \
        BCM_DPP_UNIT_CHECK(unit);\
        if(!RX_INIT_DONE(unit)) { return BCM_E_INIT; }

/* Default data for configuring RX system */
#define DPP_RX_PKT_SIZE_DEFAULT     (16 * 1024) /* 16K pkts */
#define DPP_RX_PPC_DEFAULT          (1)         /* no chains */
#define DPP_RX_PPS_DEFAULT          (1000)      /* 1000 pkts/sec */
#define DPP_RX_FREE_LIST_NEXT(data) (((void **)data)[0])
#define DPP_RX_QUEUE(unit)          (&(rx_ctl[unit]->pkt_queue[0])) /* cos 0 */
#define DPP_RX_PKT(unit, idx)       (rx_ctl[unit]->all_pkts[idx])
#define DPP_RX_PKT_USED(unit, idx)  (*(rx_ctl[unit]->pkt_load_ids + idx))
#define DPP_RX_BUF_CHECK_RETRY_COUNT    (10)
#define DPP_RX_PKT_USED_SET_LOCK(unit, idx, val)\
        do {                             \
            RX_LOCK(unit);                      \
            DPP_RX_PKT_USED(unit, idx) = val;   \
            RX_UNLOCK(unit);                    \
        } while (0)

#define DPP_RX_CPU_ASYNC_TCMD_SOP       (0)
#define DPP_RX_CPU_ASYNC_TCMD_EOP_2BV   (3) /* EOP with 2Bytes valid */
#define DPP_RX_CPU_ASYNC_TCMD_EOP_1BV   (4) /* EOP with 1Byte valid */

#define DPP_RX_DEFAULT_ALLOC    bcm_rx_pool_alloc
#define DPP_RX_DEFAULT_FREE     bcm_rx_pool_free

/*create egress bit*/         
#define _BCM_TRAP_EG_TYPE_TO_ID(_eg_trap)  ((_BCM_DPP_RX_TRAP_EGRESS << _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) | (((_eg_trap) <= SOC_PPC_TRAP_EG_TYPE_CFM_TRAP)? (_bcm_dpp_rx_trap_egrr_calc(_eg_trap)) : (_eg_trap>>_BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT) + _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT))

#define _BCM_TRAP_VIRTUAL_TO_ID(_virtual_trap)  ((_BCM_DPP_RX_TRAP_VIRTUAL << _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) | _virtual_trap)


#define _BCM_TRAP_CODE_VIRTUAL_BASE (_BCM_TRAP_VIRTUAL_TO_ID(0))

/* MACRO returns new trap id that was allocated for user defined and virtual traps*/

#define _BCM_TRAP_EG_MASK (0x7ff)

#define MAX_ITTR_COUNT (10)

/* Number of snoop and traps reserved for ITMH parsing in Aard at init */
#define _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT   (SOC_PPC_TRAP_CODE_NOF_USER_DEFINED_SNOOPS_FOR_TM)


#define _BCM_RX_SNOOP_CONFIG_PROB_TO_TMC_ACTION_CMD_SNOOP_INFO_PROB(snoop_config_prob, tmc_action_command_prob) \
    do {\
    if (SOC_IS_PETRAB(unit)) {\
        tmc_action_command_prob = ((snoop_config_prob)*1023)/100000;\
    } else {\
    uint64 prob_res;\
    /* the math is  x ---> x* (2^16 -1)/ 100,000. Unfortunately for this we need a uint64*/\
        COMPILER_64_SET(prob_res, 0, snoop_config_prob);\
        COMPILER_64_UMUL_32(prob_res, 64*1024-1);\
        /* we would like now to divide by 100000.*/ \
        /*   in prob_res_tmp only bit 32 may be set bits 33-63 must be reset because config->probability */\
        /*doesn't exceed 100000.*/\
        /* we will shift right by 1 and then divide by 50000 */\
        COMPILER_64_SHR(prob_res, 1);\
        tmc_action_command_prob = COMPILER_64_LO(prob_res) / 50000;\
    }\
} while (0)


#define _BCM_RX_TMC_ACTION_CMD_SNOOP_INFO_PROB_TO_SNOOP_CONFIG_PROB(snoop_config_prob, tmc_action_command_prob) \
    do {\
    if (SOC_IS_PETRAB(unit)) {\
        snoop_config_prob = ((tmc_action_command_prob) * 100000)/1023;\
    } else {\
    uint64 prob_res;\
    /* the math is x ---> x* (2^100,000)/(2^16-1). Unfortunately for this we need a uint64, but there is no compiler64 division, so a compromise is needed.*/\
        COMPILER_64_SET(prob_res, 0, tmc_action_command_prob);\
        COMPILER_64_UMUL_32(prob_res, 50000);\
        snoop_config_prob = COMPILER_64_LO(prob_res) / ((SOC_IS_JERICHO(unit) ? 64 :1)*32*1024-1);\
            if (snoop_config_prob > 100000 -4) {\
            /* close enough... */\
                snoop_config_prob = 100000;\
            }\
    }\
} while (0)


#define SOC_SAND_FAILURE(_sand_ret) \
    ((handle_sand_result(_sand_ret)) < 0)

/**
 * return one of three available snoop sizes: 64 192 and -1
 */
#define _BCM_RX_TMC_ACTION_SIZE_TO_SNOOP_SIZE(tmc_action_size)  (tmc_action_size==SOC_TMC_ACTION_CMD_SIZE_BYTES_64)? 64 : \
(tmc_action_size==SOC_TMC_ACTION_CMD_SIZE_BYTES_192)? 192 : -1 ;


/*
 * Function
 *      _bcm_dpp_rx_virtual_trap_get _rx_virtual_traps array.
 * Purpose
 *      find the specific struct in the 
 * Parameters
 *      (in)  unit                  = unit number
 *      (in)  prm_profile_info    = trap date
 *      (out) config              = config field
 * Returns
 *      rv = error code.
 *                    
 */
int _bcm_dpp_rx_virtual_trap_get(int unit,int virtual_trap_id,bcm_dpp_rx_virtual_traps_t *virtual_trap_str)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
        
    if ( (virtual_trap_id>=0) && (virtual_trap_id<_BCM_NUMBER_OF_VIRTUAL_TRAPS) ) {
        /*returns pointer access to _rx_virtual_traps array*/
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_RX_VIRTUAL_TRAPS, virtual_trap_str, virtual_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /*Wrong input value*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Wrong argument,function _bcm_dpp_rx_virtual_trap_get, on unit:%d \n"), unit));
    }

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_rx_virtual_trap_set(int unit,int virtual_trap_id,bcm_dpp_rx_virtual_traps_t *virtual_trap_str)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    if ( (virtual_trap_id>=0) && (virtual_trap_id<_BCM_NUMBER_OF_VIRTUAL_TRAPS) ) {
        /*returns pointer access to _rx_virtual_traps array*/
        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_RX_VIRTUAL_TRAPS, virtual_trap_str, virtual_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /*Wrong input value*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Wrong argument,function _bcm_dpp_rx_virtual_trap_set, on unit:%d \n"), unit));
    }

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_dpp_rx_fwd_action_to_config
 * Purpose
 *      Fill config field with trap data(prm_profile_info).
 * Parameters
 *      (in)  unit                  = unit number
 *      (in)  prm_profile_info    = trap date
 *      (out) config              = config field
 * Returns
 *      rv = error code.
 *                    
 */
STATIC int _bcm_dpp_rx_fwd_action_to_config(
    int unit,SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO* prm_profile_info,bcm_rx_trap_config_t *config)
{
    int rv= BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;

    if(config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL")));         
    }

    if(prm_profile_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("prm_profile_info pointer is NULL")));         
    }

    config->trap_strength = prm_profile_info->strength;

    if (prm_profile_info->bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST) {        
        if (prm_profile_info->dest_info.frwrd_dest.type == SOC_PPD_FRWRD_DECISION_TYPE_MC) {
            config->dest_group = prm_profile_info->dest_info.frwrd_dest.dest_id;
            config->flags |= BCM_RX_TRAP_DEST_MULTICAST;
        }else{
            rv= _bcm_dpp_gport_from_fwd_decision(unit, &config->dest_port, &(prm_profile_info->dest_info.frwrd_dest));
            BCMDNX_IF_ERR_EXIT(rv);

            config->flags |= BCM_RX_TRAP_UPDATE_DEST;
        }
    }

    if (prm_profile_info->bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_TC) {
        config->prio = prm_profile_info->cos_info.tc;
        config->flags |= BCM_RX_TRAP_UPDATE_PRIO;
    }

    if (prm_profile_info->bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP) {
        config->color = prm_profile_info->cos_info.dp;
        config->flags |= BCM_RX_TRAP_UPDATE_COLOR;
    }

    if (prm_profile_info->processing_info.is_control == TRUE) {
        config->flags |= BCM_RX_TRAP_BYPASS_FILTERS;
    }

    if (prm_profile_info->processing_info.enable_learning == FALSE) {
        config->flags |= BCM_RX_TRAP_LEARN_DISABLE;
    }

    if (prm_profile_info->dest_info.add_vsi == TRUE) {
        config->flags |= BCM_RX_TRAP_UPDATE_ADD_VLAN;
    }
    if (prm_profile_info->bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET) {
         config->flags          |= BCM_RX_TRAP_UPDATE_FORWARDING_TYPE;
         _bcm_dpp_rx_ppd_to_forwarding_type(prm_profile_info->processing_info.frwrd_type, &config->forwarding_type);
     }

    
    config->policer_id = prm_profile_info->meter_info.meter_id;
    config->flags |= BCM_RX_TRAP_UPDATE_POLICER;

    if (prm_profile_info->bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_POLICER) {
        /* Check if ethernet_police_id is correct*/
        config->ether_policer_id = prm_profile_info->policing_info.ethernet_police_id;
        config->flags |= BCM_RX_TRAP_UPDATE_ETHERNET_POLICER;
    }

    
    config->counter_num = prm_profile_info->count_info.counter_id;
    
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function
 *      _bcm_dpp_rx_fwd_action_to_config
 * Purpose
 *      Fill config field with trap data(prm_profile_info).
 * Parameters
 *      (in)  unit                  = unit number
 *      (in)  prm_profile_info    = trap date
 *      (out) config              = config field
 * Returns
 *      rv = error code.
 *                    
 */
STATIC int _bcm_dpp_rx_clear_virtual_traps_array(
    int unit)
{
    int rv= BCM_E_NONE;

    int j;
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO      default_info;
    bcm_dpp_rx_virtual_traps_t tmp_virt_trap;

    BCMDNX_INIT_FUNC_DEFS;

    /* Clear traps */
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&default_info);

    if (unit >= SOC_MAX_NUM_DEVICES) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit out of range")));                 
    }

    /*Clear hardware if needed*/
    for(j=0;j<_BCM_NUMBER_OF_VIRTUAL_TRAPS;j++) {

        /*get a copy of the rx virtual traps info from sw_db */
        rv = _bcm_dpp_rx_virtual_trap_get(unit,j,&tmp_virt_trap);
        BCM_SAND_IF_ERR_EXIT(rv);

        /*Clear software array*/
        SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&tmp_virt_trap.trap_info);
        SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&tmp_virt_trap.snoop_info);
        tmp_virt_trap.gport_trap_id = _BCM_DPP_TRAP_INVALID_VIRTUAL_ID;

        if (tmp_virt_trap.soc_ppd_reserved_mc !=0) {
            if (!SOC_WARM_BOOT(unit)) {
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,tmp_virt_trap.soc_ppd_reserved_mc,&default_info);
                BCM_SAND_IF_ERR_EXIT(rv);
            }
            tmp_virt_trap.soc_ppd_reserved_mc = 0;
        }
        if (tmp_virt_trap.soc_ppd_prog !=0) {
            if (!SOC_WARM_BOOT(unit)) {
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,tmp_virt_trap.soc_ppd_prog,&default_info);
                BCM_SAND_IF_ERR_EXIT(rv);
            }
            tmp_virt_trap.soc_ppd_prog = 0;
        }

        /*write rx virtual traps info back to sw_db */
        rv = _bcm_dpp_rx_virtual_trap_get(unit,j,&tmp_virt_trap);
        BCM_SAND_IF_ERR_EXIT(rv);
    }
            
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}










/*
 * Function
 *      _bcm_dpp_rx_egress_set
 * Purpose
 *       Maps egress trap type to a cofigured egress action profile
 * Parameters
 *      (in) trap_id                 = The ppd trap code
 *      (in) prm_egr_profile_info    = Egress action profile 
 * Returns
 *      rv = Error code
 *                    
 */

int _bcm_dpp_rx_egress_set(int unit,int trap_id, SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO *prm_egr_profile_info)
{
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO               default_info;
    int                                           old_template,is_last,is_allocated,curr_template;
    int                                           flags=0;    

    int                                           rv= BCM_E_NONE;
    uint32                                       soc_sand_rv;
    uint32 trap_index; /* for the allocation manager only, not PPD calls */

    BCMDNX_INIT_FUNC_DEFS;
    
    if(prm_egr_profile_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("prm_egr_profile_info pointer is NULL")));         
    }

    unit = (unit); 

    /* Translate Trap id to trap index for the allocation manager for egress traps */
    rv = _bcm_dpp_am_eg_trap_index_get(unit, 
                                       trap_id, /* from the enum */
                                       &trap_index); /* 0...NOF_TRAPS */
    BCMDNX_IF_ERR_EXIT(rv);

    /*Egress handle*/        
    rv = _bcm_dpp_am_template_trap_egress_exchange(unit,flags, trap_id, trap_index,
        prm_egr_profile_info,&old_template,&is_last,&curr_template,&is_allocated);
    BCM_SAND_IF_ERR_EXIT(rv);


    if (old_template != curr_template) {
        if(curr_template == 0) {
            curr_template = SOC_PPD_TRAP_EG_NO_ACTION;
        }
        soc_sand_rv = soc_ppd_trap_to_eg_action_map_set(unit,trap_id,curr_template);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    if (is_last) {

        SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(&default_info);

        /*Call to old template with default profile*/
        soc_sand_rv = soc_ppd_trap_eg_profile_info_set(unit,old_template,&default_info); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    if (is_allocated) {
        if(curr_template == SOC_PPD_TRAP_EG_NO_ACTION) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("cannot update default no-action.")));
        }
        soc_sand_rv = soc_ppd_trap_eg_profile_info_set(unit,curr_template,prm_egr_profile_info); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }



    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_rx_egress_set_to_drop_info(int unit, SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO *eg_drop_info)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(eg_drop_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("eg_drop_info pointer is NULL")));
    }

    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(eg_drop_info);
    eg_drop_info->bitmap_mask = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    eg_drop_info->out_tm_port = SOC_PPD_TRAP_ACTION_PKT_DISCARD_ID;

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_rx_egress_set_to_cpu_drop_info(int unit, SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO *eg_cpu_drop_info)
{
#ifdef BCM_ARAD_SUPPORT
    uint32          base_q_pair;
    unsigned int   soc_sand_dev_id;
#endif     

    BCMDNX_INIT_FUNC_DEFS;


    if(eg_cpu_drop_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("eg_cpu_drop_info pointer is NULL")));
    }

    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(eg_cpu_drop_info);

    eg_cpu_drop_info->bitmap_mask = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    eg_cpu_drop_info->out_tm_port = SOC_PETRA_FRST_CPU_PORT_ID;
#ifdef BCM_ARAD_SUPPORT
    soc_sand_dev_id = (unit);

    if (SOC_IS_ARAD(unit)) {

        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(soc_sand_dev_id, CMIC_PORT(unit), &base_q_pair));

        eg_cpu_drop_info->out_tm_port = base_q_pair; 
    }
#endif     

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;

}


/*
 * Function
 *      _bcm_dpp_trap_mate_get
 * Purpose
 *      Get the trap mates for each trap code if exist.
 * Parameters
 *      (in) trap_id    = the ppd trap code
 * Returns
 *      rv = ppd trap mate
 *                    
 */
STATIC int
_bcm_dpp_trap_mate_get(int unit, int trap_id)
{
    int rv = -1;
    
    BCMDNX_INIT_FUNC_DEFS;
    switch(trap_id) 
    {
    /* For example: SOC_PPD_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14 work with a link list style that follows first 3 cases*/
    case SOC_PPD_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14:
        rv = SOC_PPD_TRAP_CODE_MPLS_PWE_NO_BOS;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_PWE_NO_BOS:
        rv = SOC_PPD_TRAP_CODE_MPLS_VRF_NO_BOS;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_VRF_NO_BOS:
        rv = SOC_PPD_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS:
        rv = -1;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_TTL0:
        rv = SOC_PPD_TRAP_CODE_MPLS_TERM_TTL_0;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_TTL1:
        rv = SOC_PPD_TRAP_CODE_MPLS_TERM_TTL_1;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_TERM_TTL_1:
        rv = -1;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_CONTROL_WORD_TRAP:
        rv = SOC_PPD_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP:
        rv = SOC_PPD_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP:
        rv = -1;
        break;
    case SOC_PPD_TRAP_CODE_IPV4_SIP_EQUAL_DIP:
        if (SOC_IS_PETRAB(unit)) {
            rv = SOC_PPD_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP;
        }
        else{
            rv = -1;
        }
        break;
    case SOC_PPD_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP:
        rv = -1;
        break;
    case SOC_PPD_TRAP_CODE_IPV4_DIP_ZERO:
        if (SOC_IS_PETRAB(unit)) {
            rv = SOC_PPD_TRAP_CODE_IPV4_TERM_DIP_ZERO;
        }
        else{
            rv = -1;
        }
        break;
    case SOC_PPD_TRAP_CODE_IPV4_TERM_DIP_ZERO:
        rv = -1;
        break;
    case SOC_PPD_TRAP_CODE_IPV4_SIP_IS_MC:
        if (SOC_IS_PETRAB(unit)) {
            rv = SOC_PPD_TRAP_CODE_IPV4_TERM_SIP_IS_MC;
        }
        else{
            rv = -1;
        }
        break;
    case SOC_PPD_TRAP_CODE_IPV4_TERM_SIP_IS_MC:
        rv = -1;
        break;
    case SOC_PPD_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL:
        if (SOC_IS_PETRAB(unit)) {
            rv = SOC_PPD_TRAP_CODE_MC_USE_SIP_RPF_FAIL;
        }
        else{
            rv = -1;
        }
        break;
    case SOC_PPD_TRAP_CODE_MC_USE_SIP_RPF_FAIL:
        rv = -1;
        break;
    case SOC_PPD_TRAP_CODE_UNKNOWN_DA_0:
        rv = SOC_PPD_TRAP_CODE_UNKNOWN_DA_1;
        break;
    case SOC_PPD_TRAP_CODE_UNKNOWN_DA_1:
        rv = SOC_PPD_TRAP_CODE_UNKNOWN_DA_2;
        break;
    case SOC_PPD_TRAP_CODE_UNKNOWN_DA_2:
        rv = SOC_PPD_TRAP_CODE_UNKNOWN_DA_3;
        break;
    case SOC_PPD_TRAP_CODE_UNKNOWN_DA_3:
        rv = SOC_PPD_TRAP_CODE_UNKNOWN_DA_4;
        break;
    case SOC_PPD_TRAP_CODE_UNKNOWN_DA_4:
        rv = SOC_PPD_TRAP_CODE_UNKNOWN_DA_5;
        break;
    case SOC_PPD_TRAP_CODE_UNKNOWN_DA_5:
        rv = SOC_PPD_TRAP_CODE_UNKNOWN_DA_6;
        break;
    case SOC_PPD_TRAP_CODE_UNKNOWN_DA_6:
        rv = SOC_PPD_TRAP_CODE_UNKNOWN_DA_7;
        break;
    case SOC_PPD_TRAP_CODE_UNKNOWN_DA_7:
        rv = -1;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_LSP_BOS:
        rv = SOC_PPD_TRAP_CODE_MPLS_UNEXPECTED_BOS;
        break;
    case SOC_PPD_TRAP_CODE_MPLS_UNEXPECTED_BOS:
        rv = -1;
        break;

    default:
        rv = -1;
    }
    BCM_RETURN_VAL_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}




#ifdef BCM_PETRAB_SUPPORT
STATIC bcm_rx_cfg_t _bcm_petra_default_rx_cfg = {
    DPP_RX_PKT_SIZE_DEFAULT,/* packet alloc size */
    DPP_RX_PPC_DEFAULT,     /* Packets per chain */
    DPP_RX_PPS_DEFAULT,     /* Default pkt rate, global (all COS, one unit) */
    0,                      /* Burst */
    {                       /* Not applicable for DPP */
        {0},                
        {                   
            0, 
            0,
            0,
            0
        }
    },
    DPP_RX_DEFAULT_ALLOC,   /* alloc function */
    DPP_RX_DEFAULT_FREE,    /* free function */
    0                       /* flags */
};
#endif /* BCM_PETRAB_SUPPORT */   
#ifdef BCM_ARAD_SUPPORT
/* Default data for configuring RX system */
CONST STATIC bcm_rx_cfg_t _rx_arad_dflt_cfg = {
    DPP_RX_PKT_SIZE_DEFAULT,       /* packet alloc size */
    RX_PPC_DFLT,            /* Packets per chain */
    RX_PPS_DFLT,            /* Default pkt rate, global (all COS, one unit) */
    0,                      /* Burst */
    {                       /* Just configure channel 1 */
        {0},                /* Channel 0 is usually TX */
        {                   /* Channel 1, default RX */
            RX_CHAINS_DFLT, /* DV count (number of chains) */
            1000,           /* Default pkt rate, DEPRECATED */
            0,              /* No flags */
            0xff            /* COS bitmap channel to receive */
        }
    },
    DPP_RX_DEFAULT_ALLOC,       /* alloc function */
    DPP_RX_DEFAULT_FREE,        /* free function */
    0                       /* flags */
};
#endif


STATIC SOC_PPD_TRAP_CODE _bcm_rx_trap_to_ppd[bcmRxTrapCount];

/*
 * Function
 *      _bcm_dpp_rx_trap_egrr_calc
 * Purpose
 *      convert bit map trap id to value trap id.
 * Parameters
 *      (in)  egrr_trap    			= bit map trap id
 * Returns
 *      rv = value trap id.
 *                    
 */
int
_bcm_dpp_rx_trap_egrr_calc(int egrr_trap)
{
    int counter=0;

    while (egrr_trap) {
        egrr_trap = egrr_trap>>1;
        counter++;
    }
    return (counter-1);
}

/*
 * Function
 *      _bcm_dpp_rx_trap_egrr_reverse_calc
 * Purpose
 *      convert value trap id to bit map trap id.
 * Parameters
 *      (in)  egrr_trap    			= value trap id
 * Returns
 *      rv = bit map trap id.
 *                    
 */
int
_bcm_dpp_rx_trap_egrr_reverse_calc(int egrr_trap)
{

    int counter=1;

    while (egrr_trap) {
        counter = counter<<1;
        egrr_trap--;
    }
    return counter;
}

/*
 * Function
 *      _bcm_dpp_rx_trap_id_to_egress
 * Purpose
 *      convert value trap id to bit map trap id,support 2 cases.
 * Parameters
 *      (in)  egrr_trap    			= value trap id
 * Returns
 *      rv = bit map trap id.
 *                    
 */
int _bcm_dpp_rx_trap_id_to_egress(int _eg_trap)
{

    return ( ( ((_eg_trap&(_BCM_TRAP_EG_MASK)) <= _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT)? (_bcm_dpp_rx_trap_egrr_reverse_calc(_eg_trap&(_BCM_TRAP_EG_MASK))) : (((_eg_trap&(_BCM_TRAP_EG_MASK))- _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT)<<_BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT) ) ); /*set bit 9 to low*/
}

/*
 * Function
 *      _bcm_dpp_rx_trap_to_ppd_init
 * Purpose
 *      init _bcm_rx_trap_to_ppd array.
 * Parameters
 *      (in)  unit    		= unit number
 *                    
 */
void _bcm_dpp_rx_trap_to_ppd_init(int unit)
{
    _bcm_rx_trap_to_ppd[bcmRxTrapMimDiscardMacsaFwd]            =    SOC_PPD_TRAP_CODE_PBP_SA_DROP_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimDiscardMacsaDrop]           =    SOC_PPD_TRAP_CODE_PBP_SA_DROP_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimDiscardMacsaTrap]           =    SOC_PPD_TRAP_CODE_PBP_SA_DROP_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimDiscardMacsaSnoop]          =    SOC_PPD_TRAP_CODE_PBP_SA_DROP_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimTeSaMove]                   =    SOC_PPD_TRAP_CODE_PBP_TE_TRANSPLANT;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimTeSaUnknown]                =    SOC_PPD_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimSaMove]                     =    SOC_PPD_TRAP_CODE_PBP_TRANSPLANT;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimSaUnknown]                  =    SOC_PPD_TRAP_CODE_PBP_LEARN_SNOOP;
    _bcm_rx_trap_to_ppd[bcmRxTrapAuthSaLookupFail]              =    SOC_PPD_TRAP_CODE_SA_AUTHENTICATION_FAILED;
    _bcm_rx_trap_to_ppd[bcmRxTrapAuthSaPortFail]                =    SOC_PPD_TRAP_CODE_PORT_NOT_PERMITTED;
    _bcm_rx_trap_to_ppd[bcmRxTrapAuthSaVlanFail]                =    SOC_PPD_TRAP_CODE_UNEXPECTED_VID;
    _bcm_rx_trap_to_ppd[bcmRxTrapSaMulticast]                   =    SOC_PPD_TRAP_CODE_SA_MULTICAST;
    _bcm_rx_trap_to_ppd[bcmRxTrapSaEqualsDa]                    =    SOC_PPD_TRAP_CODE_SA_EQUALS_DA;
    _bcm_rx_trap_to_ppd[bcmRxTrap8021xFail]                     =    SOC_PPD_TRAP_CODE_8021X;
    _bcm_rx_trap_to_ppd[bcmRxTrapArpMyIp]                       =    SOC_PPD_TRAP_CODE_MY_ARP;
    _bcm_rx_trap_to_ppd[bcmRxTrapArp]                           =    SOC_PPD_TRAP_CODE_ARP;
    _bcm_rx_trap_to_ppd[bcmRxTrapIgmpMembershipQuery]           =    SOC_PPD_TRAP_CODE_IGMP_MEMBERSHIP_QUERY;
    _bcm_rx_trap_to_ppd[bcmRxTrapIgmpReportLeaveMsg]            =    SOC_PPD_TRAP_CODE_IGMP_REPORT_LEAVE_MSG;
    _bcm_rx_trap_to_ppd[bcmRxTrapIgmpUndefined]                 =    SOC_PPD_TRAP_CODE_IGMP_UNDEFINED;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpv6MldMcListenerQuery]      =    SOC_PPD_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Cache]                       =    _BCM_TRAP_CODE_VIRTUAL_BASE;
    _bcm_rx_trap_to_ppd[bcmRxTrapDhcpv6Server]                  =    SOC_PPD_TRAP_CODE_DHCPV6_SERVER;
    _bcm_rx_trap_to_ppd[bcmRxTrapDhcpv6Client]                  =    SOC_PPD_TRAP_CODE_DHCPV6_CLIENT;  
    _bcm_rx_trap_to_ppd[bcmRxTrapPortNotVlanMember]             =    SOC_PPD_TRAP_CODE_PORT_NOT_VLAN_MEMBER;
    _bcm_rx_trap_to_ppd[bcmRxTrapHeaderSizeErr]                 =    SOC_PPD_TRAP_CODE_HEADER_SIZE_ERR;
    _bcm_rx_trap_to_ppd[bcmRxTrapMyBmacUknownTunnel]            =    SOC_PPD_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMyBmacUnknownISid]             =    SOC_PPD_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID;
    _bcm_rx_trap_to_ppd[bcmRxTrapStpStateBlock]                 =    SOC_PPD_TRAP_CODE_STP_STATE_BLOCK;
    _bcm_rx_trap_to_ppd[bcmRxTrapStpStateLearn]                 =    SOC_PPD_TRAP_CODE_STP_STATE_LEARN;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpCompMcInvalidIp]             =    SOC_PPD_TRAP_CODE_IP_COMP_MC_INVALID_IP;  
    _bcm_rx_trap_to_ppd[bcmRxTrapMyMacAndIpDisabled]            =    SOC_PPD_TRAP_CODE_MY_MAC_AND_IP_DISABLE;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillVersion]                  =    SOC_PPD_TRAP_CODE_TRILL_VERSION;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillTtl0]                     =    SOC_PPD_TRAP_CODE_TRILL_INVALID_TTL;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillChbh]                     =    SOC_PPD_TRAP_CODE_TRILL_CHBH;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillUnknonwnIngressNickname]  =    SOC_PPD_TRAP_CODE_TRILL_NO_REVERSE_FEC;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillCite]                     =    SOC_PPD_TRAP_CODE_TRILL_CITE;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillIllegalInnerMc]           =    SOC_PPD_TRAP_CODE_TRILL_ILLEGAL_INNER_MC;
    _bcm_rx_trap_to_ppd[bcmRxTrapMyMacAndMplsDisable]           =    SOC_PPD_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE;
    _bcm_rx_trap_to_ppd[bcmRxTrapArpReply]                      =    SOC_PPD_TRAP_CODE_MY_MAC_AND_ARP;
    _bcm_rx_trap_to_ppd[bcmRxTrapMyMacAndUnknownL3]             =    SOC_PPD_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabel0150]                 =    SOC_PPD_TRAP_CODE_MPLS_LABEL_VALUE_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabel0151]                 =    SOC_PPD_TRAP_CODE_MPLS_LABEL_VALUE_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabel0152]                 =    SOC_PPD_TRAP_CODE_MPLS_LABEL_VALUE_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabel0153]                 =    SOC_PPD_TRAP_CODE_MPLS_LABEL_VALUE_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsTerminationFail]           =    SOC_PPD_TRAP_CODE_MPLS_NO_RESOURCES;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsUnexpectedBos]             =    SOC_PPD_TRAP_CODE_MPLS_UNEXPECTED_BOS;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsUnexpectedNoBos]           =    SOC_PPD_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS;
    _bcm_rx_trap_to_ppd[bcmRxTrapCfmAcceleratedIngress]         =    SOC_PPD_TRAP_CODE_CFM_ACCELERATED_INGRESS;
    _bcm_rx_trap_to_ppd[bcmRxTrapIllegelPacketFormat]           =    SOC_PPD_TRAP_CODE_ILLEGEL_PFC;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DiscardMacsaFwd]             =    SOC_PPD_TRAP_CODE_SA_DROP_0;   
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DiscardMacsaDrop]            =    SOC_PPD_TRAP_CODE_SA_DROP_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DiscardMacsaTrap]            =    SOC_PPD_TRAP_CODE_SA_DROP_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DiscardMacsaSnoop]           =    SOC_PPD_TRAP_CODE_SA_DROP_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Learn0]                      =    SOC_PPD_TRAP_CODE_SA_NOT_FOUND_0; 
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Learn1]                      =    SOC_PPD_TRAP_CODE_SA_NOT_FOUND_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Learn2]                      =    SOC_PPD_TRAP_CODE_SA_NOT_FOUND_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2Learn3]                      =    SOC_PPD_TRAP_CODE_SA_NOT_FOUND_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapVlanUnknownDa]                 =    SOC_PPD_TRAP_CODE_UNKNOWN_DA_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapExternalLookupError]           =    SOC_PPD_TRAP_CODE_ELK_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DlfFwd]                      =    SOC_PPD_TRAP_CODE_DA_NOT_FOUND_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DlfDrop]                     =    SOC_PPD_TRAP_CODE_DA_NOT_FOUND_1;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DlfTrap]                     =    SOC_PPD_TRAP_CODE_DA_NOT_FOUND_2;
    _bcm_rx_trap_to_ppd[bcmRxTrapL2DlfSnoop]                    =    SOC_PPD_TRAP_CODE_DA_NOT_FOUND_3;
    _bcm_rx_trap_to_ppd[bcmRxTrapExtendedP2pLookupFail]         =    SOC_PPD_TRAP_CODE_P2P_MISCONFIGURATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapSameInterface]                 =    SOC_PPD_TRAP_CODE_SAME_INTERFACE;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillUnknownUc]                =    SOC_PPD_TRAP_CODE_TRILL_UNKNOWN_UC;
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillUnknownMc]                =    SOC_PPD_TRAP_CODE_TRILL_UNKNOWN_MC;
    _bcm_rx_trap_to_ppd[bcmRxTrapUcLooseRpfFail]                =    SOC_PPD_TRAP_CODE_UC_LOOSE_RPF_FAIL;
    _bcm_rx_trap_to_ppd[bcmRxTrapDefaultUcv6]                   =    SOC_PPD_TRAP_CODE_DEFAULT_UCV6;
    _bcm_rx_trap_to_ppd[bcmRxTrapDefaultMcv6]                   =    SOC_PPD_TRAP_CODE_DEFAULT_MCV6;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsP2pNoBos]                  =    SOC_PPD_TRAP_CODE_MPLS_P2P_NO_BOS;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsControlWordTrap]           =    SOC_PPD_TRAP_CODE_MPLS_CONTROL_WORD_TRAP;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsControlWordDrop]           =    SOC_PPD_TRAP_CODE_MPLS_CONTROL_WORD_DROP;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsUnknownLabel]              =    SOC_PPD_TRAP_CODE_MPLS_UNKNOWN_LABEL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsExtendP2pMplsx4]           =    SOC_PPD_TRAP_CODE_MPLS_P2P_MPLSX4;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIgmpMembershipQuery]      =    SOC_PPD_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIgmpReportLeaveMsg]       =    SOC_PPD_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIgmpUndefined]            =    SOC_PPD_TRAP_CODE_ETH_FL_IGMP_UNDEFINED;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIcmpv6MldMcListenerQuery] =    SOC_PPD_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIcmpv6MldReportDone]      =    SOC_PPD_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE;
    _bcm_rx_trap_to_ppd[bcmRxTrapFrwrdIcmpv6MldUndefined]       =    SOC_PPD_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4VersionError]              =    SOC_PPD_TRAP_CODE_IPV4_VERSION_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4ChecksumError]             =    SOC_PPD_TRAP_CODE_IPV4_CHECKSUM_ERROR; 
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4HeaderLengthError]         =    SOC_PPD_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4TotalLengthError]          =    SOC_PPD_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR; 
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4Ttl0]                      =    SOC_PPD_TRAP_CODE_IPV4_TTL0;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4HasOptions]                =    SOC_PPD_TRAP_CODE_IPV4_HAS_OPTIONS;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4Ttl1]                      =    SOC_PPD_TRAP_CODE_IPV4_TTL1;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4SipEqualDip]               =    SOC_PPD_TRAP_CODE_IPV4_SIP_EQUAL_DIP;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4DipZero]                   =    SOC_PPD_TRAP_CODE_IPV4_DIP_ZERO;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4SipIsMc]                   =    SOC_PPD_TRAP_CODE_IPV4_SIP_IS_MC;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv4TunnelTerminationAndFragmented] = SOC_PPD_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6VersionError]                   = SOC_PPD_TRAP_CODE_IPV6_VERSION_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6HopCount0]                      = SOC_PPD_TRAP_CODE_IPV6_HOP_COUNT0;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6HopCount1]                      = SOC_PPD_TRAP_CODE_IPV6_HOP_COUNT1;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6UnspecifiedDestination]         = SOC_PPD_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LoopbackAddress]                = SOC_PPD_TRAP_CODE_IPV6_LOOPBACK_ADDRESS;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6MulticastSource]                = SOC_PPD_TRAP_CODE_IPV6_MULTICAST_SOURCE;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6NextHeaderNull]                 = SOC_PPD_TRAP_CODE_IPV6_NEXT_HEADER_NULL;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6UnspecifiedSource]              = SOC_PPD_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LocalLinkDestination]           = SOC_PPD_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LocalSiteDestination]           = SOC_PPD_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LocalLinkSource]                = SOC_PPD_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6LocalSiteSource]                = SOC_PPD_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6Ipv4CompatibleDestination]      = SOC_PPD_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6Ipv4MappedDestination]          = SOC_PPD_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapIpv6MulticastDestination]           = SOC_PPD_TRAP_CODE_IPV6_MULTICAST_DESTINATION;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsTtl0]                           = SOC_PPD_TRAP_CODE_MPLS_TTL0; 
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsTtl1]                           = SOC_PPD_TRAP_CODE_MPLS_TTL1;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpSnFlagsZero]                     = SOC_PPD_TRAP_CODE_TCP_SN_FLAGS_ZERO;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpSnZeroFlagsSet]                  = SOC_PPD_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpSynFin]                          = SOC_PPD_TRAP_CODE_TCP_SYN_FIN;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpEqualPorts]                      = SOC_PPD_TRAP_CODE_TCP_EQUAL_PORTS;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpFragmentIncompleteHeader]        = SOC_PPD_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER;
    _bcm_rx_trap_to_ppd[bcmRxTrapTcpFragmentOffsetLt8]               = SOC_PPD_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8;
    _bcm_rx_trap_to_ppd[bcmRxTrapUdpEqualPorts]                      = SOC_PPD_TRAP_CODE_UDP_EQUAL_PORTS;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpDataGt576]                      = SOC_PPD_TRAP_CODE_ICMP_DATA_GT_576;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpFragmented]                     = SOC_PPD_TRAP_CODE_ICMP_FRAGMENTED;
    _bcm_rx_trap_to_ppd[bcmRxTrapFailoverFacilityInvalid]            = SOC_PPD_TRAP_CODE_FACILITY_INVALID;
    _bcm_rx_trap_to_ppd[bcmRxTrapUcStrictRpfFail]                    = SOC_PPD_TRAP_CODE_UC_STRICT_RPF_FAIL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMcExplicitRpfFail]                  = SOC_PPD_TRAP_CODE_MC_EXPLICIT_RPF_FAIL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMcUseSipRpfFail]                    = SOC_PPD_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL;
    _bcm_rx_trap_to_ppd[bcmRxTrapMcUseSipMultipath]                  = SOC_PPD_TRAP_CODE_MC_USE_SIP_ECMP; 
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpRedirect]                       = SOC_PPD_TRAP_CODE_ICMP_REDIRECT;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamp]                               = SOC_PPD_TRAP_CODE_USER_OAMP;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamEthAccelerated]                  = SOC_PPD_TRAP_CODE_TRAP_ETH_OAM;        
    _bcm_rx_trap_to_ppd[bcmRxTrapOamMplsAccelerated]                 = SOC_PPD_TRAP_CODE_USER_MPLS_OAM_ACCELERATED;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdIpTunnelAccelerated]          = SOC_PPD_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED;    
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdPweAccelerated]               = SOC_PPD_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamEthUpAccelerated]                = SOC_PPD_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED;
    
    _bcm_rx_trap_to_ppd[bcmRxTrapEgPortNotVlanMember]                = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_VSI_MEMBERSHIP);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgHairPinFilter]                    = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_HAIR_PIN);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgSplitHorizonFilter]               = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_SPLIT_HORIZON);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgUnknownDa]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_UNKNOWN_DA); 
    _bcm_rx_trap_to_ppd[bcmRxTrapEgMtuFilter]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_MTU_VIOLATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgPvlanFilter]                      = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_PRIVATE_VLAN);   
    _bcm_rx_trap_to_ppd[bcmRxTrapEgDiscardFrameTypeFilter]           = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_ACC_FRM);   
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTrillHairPinFilter]               = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_TRILL_SAME_INTERFACE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTrillBounceBack]                  = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_TRILL_BOUNCE_BACK);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgTrillTtl0]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_TRILL_TTL_0);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpmcTtlErr]                       = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_TTL_SCOPE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgCfmAccelerated]                   = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_CFM_TRAP);
    _bcm_rx_trap_to_ppd[bcmRxTrapDssStacking]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_DSS_STACKING);
    _bcm_rx_trap_to_ppd[bcmRxTrapLagMulticast]                       = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_LAG_MULTICAST);
    _bcm_rx_trap_to_ppd[bcmRxTrapExEgCnm]                            = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_CNM_PACKET);

    /*arad only:*/ 
    _bcm_rx_trap_to_ppd[bcmRxTrapTrillDesignatedVlanNoMymac]        = SOC_PPD_TRAP_CODE_TRILL_DISABLE_BRIDGE_IF_DESIGNATED;
    _bcm_rx_trap_to_ppd[bcmRxTrapAdjacentCheckFail]                 = SOC_PPD_TRAP_CODE_TRILL_NO_ADJACENT;                             
    _bcm_rx_trap_to_ppd[bcmRxTrapFcoeSrcIdMismatchSa]               = SOC_PPD_TRAP_CODE_FCOE_SRC_SA_MISMATCH;
    _bcm_rx_trap_to_ppd[bcmRxTrapSipMove]                           = -1;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamY1731MplsTp]                    = SOC_PPD_TRAP_CODE_TRAP_Y1731_O_MPLS_TP;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamY1731Pwe]                       = SOC_PPD_TRAP_CODE_TRAP_Y1731_O_PWE;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdIpv4]                        = SOC_PPD_TRAP_CODE_TRAP_BFD_O_IPV4;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdMpls]                        = SOC_PPD_TRAP_CODE_TRAP_BFD_O_MPLS;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdPwe]                         = SOC_PPD_TRAP_CODE_TRAP_BFD_O_PWE;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdCcMplsTp]                    = SOC_PPD_TRAP_CODE_TRAP_BFDCC_O_MPLS_TP;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamBfdCvMplsTp]                     = SOC_PPD_TRAP_CODE_TRAP_BFDCV_O_MPLS_TP;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamLevel]                          = SOC_PPD_TRAP_CODE_OAM_LEVEL;
    _bcm_rx_trap_to_ppd[bcmRxTrapOamPassive]                        = SOC_PPD_TRAP_CODE_OAM_PASSIVE;
    _bcm_rx_trap_to_ppd[bcmRxTrap1588]                              = SOC_PPD_TRAP_CODE_1588_PACKET_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapMplsLabelIndexNoSupport]           = SOC_PPD_TRAP_CODE_MPLS_P2P_MPLSX4;
    _bcm_rx_trap_to_ppd[bcmRxTrapDhcpv4Client]                      = SOC_PPD_TRAP_CODE_DHCP_CLIENT;
    _bcm_rx_trap_to_ppd[bcmRxTrapDhcpv4Server]                      = SOC_PPD_TRAP_CODE_DHCP_SERVER;
    _bcm_rx_trap_to_ppd[bcmRxTrapEtherIpVersion]                    = SOC_PPD_TRAP_CODE_ETHER_IP_VERSION_ERROR;
    _bcm_rx_trap_to_ppd[bcmRxTrapEgressObjectAccessed]              = SOC_PPD_TRAP_CODE_FEC_ENTRY_ACCESSED;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpv6MldReportDone]               = SOC_PPD_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG;
    _bcm_rx_trap_to_ppd[bcmRxTrapIcmpv6MldUndefined]                = SOC_PPD_TRAP_CODE_ICMPV6_MLD_UNDEFINED;
    _bcm_rx_trap_to_ppd[bcmRxTrapFailover1Plus1Fail]                = SOC_PPD_TRAP_CODE_LIF_PROTECT_PATH_INVALID;

    _bcm_rx_trap_to_ppd[bcmRxTrapEgInvalidDestPort]                 = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_INVALID_OUT_PORT);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4VersionError]                = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV4_VERSION_ERROR);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4ChecksumError]               = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV4_CHECKSUM_ERROR);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4HeaderLengthError]           = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV4_HEADER_LENGTH_ERROR);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4TotalLengthError]            = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV4_TOTAL_LENGTH_ERROR);       
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4Ttl0]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IP_TTL0);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4HasOptions]                  = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IP_HAS_OPTIONS);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4Ttl1]                        = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IP_TTL1);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4SipEqualDip]                 = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV4_SIP_EQUAL_DIP);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4DipZero]                     = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV4_DIP_ZERO);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv4SipIsMc]                     = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV4_SIP_IS_MC);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6UnspecifiedDestination]      = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_UNSPECIFIED_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LoopbackAddress]             = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_LOOPBACK_ADDRESS);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6MulticastSource]             = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_MULTICAST_SOURCE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6UnspecifiedSource]           = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_UNSPECIFIED_SOURCE);
        
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LocalLinkDestination]       = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_LOCAL_LINK_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LocalSiteDestination]       = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_LOCAL_SITE_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LocalLinkSource]            = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_LOCAL_LINK_SOURCE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6LocalSiteSource]            = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_LOCAL_SITE_SOURCE);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6Ipv4CompatibleDestination]  = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_IPV4_COMPATIBLE_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6Ipv4MappedDestination]      = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_IPV4_MAPPED_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6MulticastDestination]       = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_MULTICAST_DESTINATION);
    _bcm_rx_trap_to_ppd[bcmRxTrapEgIpv6NextHeaderNull]             = _BCM_TRAP_EG_TYPE_TO_ID(SOC_PPD_TRAP_EG_TYPE_IPV6_HOP_BY_HOP);
         
    _bcm_rx_trap_to_ppd[bcmRxTrapUserDefine]                        = SOC_PPD_TRAP_CODE_USER_DEFINED_0;
    _bcm_rx_trap_to_ppd[bcmRxTrapArpIPUnknown]                   = -1;  
    _bcm_rx_trap_to_ppd[bcmRxTrapFcoeZoneCheckFail]              = SOC_PPD_TRAP_CODE_FCOE_FCF_FLP_LOOKUP_FAIL; /* for all fcf lookup fail zone + forwarding*/
    _bcm_rx_trap_to_ppd[bcmRxTrapArplookupFail]                     = SOC_PPD_TRAP_CODE_ARP_FLP_FAIL;     
    _bcm_rx_trap_to_ppd[bcmRxTrapL2cpPeer]                          = SOC_PPD_TRAP_CODE_ETH_L2CP_PEER; 
    _bcm_rx_trap_to_ppd[bcmRxTrapL2cpDrop]                          = SOC_PPD_TRAP_CODE_ETH_L2CP_DROP;
    _bcm_rx_trap_to_ppd[bcmRxTrapMimMyBmacMulticastContinue]        = SOC_PPD_TRAP_CODE_MY_B_MAC_MC_CONTINUE;                                                                                                                                                                                                                    
}



STATIC sal_thread_t rx_poll_tid;

#if (defined BCM_PETRA_SUPPORT) || (defined BCM_PETRA_SUPPORT)

/*
 * Function:
 *      bcm_petra_rx_sched_register
 * Purpose:
 *      Rx scheduler registration function. 
 * Parameters:
 *      unit       - (IN) Unused. 
 *      sched_cb   - (IN) Rx scheduler routine.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_rx_sched_register(int unit, bcm_rx_sched_cb sched_cb)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_sched_register(unit, sched_cb));
     } 
#endif
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {    
        return BCM_E_UNAVAIL;
    }      
#endif
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_petra_rx_sched_unregister
 * Purpose:
 *      Rx scheduler de-registration function. 
 * Parameters:
 *      unit  - (IN) Unused. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_rx_sched_unregister(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_sched_unregister(unit));
     } 
#endif
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {    
        return BCM_E_UNAVAIL;
    }      
#endif
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_petra_rx_unit_next_get
 * Purpose:
 *      Rx started units iteration routine.
 * Parameters:
 *      unit       - (IN)  BCM device number. 
 *      unit_next  - (OUT) Next attached unit with started rx.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_rx_unit_next_get(int unit, int *unit_next)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_unit_next_get(unit, unit_next));
     } 
#endif
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {    
        return BCM_E_UNAVAIL;
    }      
#endif
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN; 
}

int 
bcm_petra_rx_channels_running(
    int unit, 
    uint32 *channels)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_channels_running(unit, channels));
     } 
#endif
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {    
        return BCM_E_UNAVAIL;
    }      
#endif
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;        
}

/* Function:    bcm_petra_rx_active
 * Purpose:     Indicates if RX is active on this unit
 * Parameters:  unit (IN)   - Device Number
 * Returns:     1 if RX is running, 0 otherwise
 */
int
bcm_petra_rx_active(int unit)
{
    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT

    if (SOC_IS_ARAD(unit)) {
         return (_bcm_common_rx_active(unit));
     } 
#endif     
     if (SOC_IS_PETRAB(unit)) {

        BCM_DPP_UNIT_CHECK(unit);

        return (RX_UNIT_STARTED(unit) != 0);
    }
    return (BCM_E_NONE);
}

/* Function:    bcm_petra_rx_alloc
 * Purpose:     Allocate an RX buffer using configured allocator
 * Parameters:  unit (IN)       - Device Number
 *              pkt_size (IN)   - size of packet to allocate
 *              flags (IN)      - flags passed to allocator
 *              buf (OUT)       - allocated buffer
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_alloc(int unit, int pkt_size, uint32 flags, void **buf)
{
    int rv;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         return (_bcm_common_rx_alloc(unit,pkt_size,flags, buf));
     } 
#endif
    if (SOC_IS_PETRAB(unit)) {     
#ifdef BCM_PETRAB_SUPPORT
      
        DPP_RX_START_CHECK(unit);

    if (pkt_size <= 0) {
        pkt_size = rx_ctl[unit]->user_cfg.pkt_size;
    }

    rv = rx_ctl[unit]->user_cfg.rx_alloc(unit, pkt_size, flags, buf);
#endif
        return rv;
    }
    
    return (BCM_E_UNAVAIL);
}

/* Function:    bcm_petra_rx_cfg_get
 * Purpose:     Returns the current RX configuration
 * Parameters:  unit (IN)       - Device Number
 *              cfg (OUT)       - Current RX configuration
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cfg_get(int unit, bcm_rx_cfg_t *cfg)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT(_bcm_common_rx_cfg_get(unit,cfg));
     } 
#endif
#ifdef BCM_PETRAB_SUPPORT

    if (SOC_IS_PETRAB(unit)){ 


        RX_INIT_CHECK(unit);
        BCMDNX_NULL_CHECK(cfg);

        RX_LOCK(unit);
        sal_memcpy(cfg, &rx_ctl[unit]->user_cfg, sizeof(bcm_rx_cfg_t));
        RX_UNLOCK(unit);
        BCM_EXIT;
    }
#endif    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_cfg_init
 * Purpose:     Initialize user level RX configuration to default values
 * Parameters:  unit (IN)       - Device Number
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cfg_init(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
        /*    BCMDNX_IF_ERR_EXIT(_bcm_common_rx_cfg_init(unit)); */
        RX_INIT_CHECK(unit);
        if (RX_UNIT_STARTED(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("RX already started on unit:%d \n"), unit));
        } 
        sal_memcpy(&rx_ctl[unit]->user_cfg, &_rx_arad_dflt_cfg, sizeof(bcm_rx_cfg_t));
    } 
#endif    
#ifdef BCM_PETRAB_SUPPORT
    /* There is a check that the unit is ok in the macro BCM_DPP_UNIT_CHECK  - no overrun */
    /* coverity[overrun-local:FALSE] */
    if (SOC_IS_PETRAB(unit)) { 

        RX_INIT_CHECK(unit);

        if (RX_UNIT_STARTED(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("RX already started on unit:%d \n"), unit));
        }

        RX_LOCK(unit);
        sal_memcpy(&rx_ctl[unit]->user_cfg, &_bcm_petra_default_rx_cfg, 
                   sizeof(bcm_rx_cfg_t));
        RX_UNLOCK(unit);
    
        BCM_EXIT;
    }
#endif    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_free
 * Description: 
 * Parameters:  unit (IN)       - Device number
 *              pkt_data (IN)   - pointer to packet data
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_free(int unit, void *pkt_data)
{
#ifdef BCM_PETRAB_SUPPORT
    int rv;
#endif    
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_free(unit,pkt_data));
     } 
#endif
#ifdef BCM_PETRAB_SUPPORT

    if (SOC_IS_PETRAB(unit)) { 

 
    DPP_RX_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(pkt_data);

    RX_LOCK(unit);
    rv = rx_ctl[unit]->user_cfg.rx_free(unit, pkt_data);
    RX_UNLOCK(unit);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d error(%s) freeing packet:%p\n"), 
                              unit, bcm_errmsg(rv), pkt_data));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
    }
#endif
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_rx_free_queued
 * Purpose:     Free all buffers listed in pending free list 
 * Parameters:  unit (IN)       - Device Number
 */
void
_bcm_petra_rx_free_queued(int unit)
{
    void *free_list, *next_free;

    /* Steal list of pkts to be freed for unit */
    RX_INTR_LOCK;
    free_list = (bcm_pkt_t *)rx_ctl[unit]->free_list;
    rx_ctl[unit]->free_list = NULL;
    RX_INTR_UNLOCK;

    while (free_list) {
        next_free = DPP_RX_FREE_LIST_NEXT(free_list);
        bcm_petra_rx_free(unit, free_list);
        free_list = next_free;
    }
}

/* Function:    _bcm_petra_rx_queue_cleanup
 * Purpose:     Remove all packets in queue
 * Parameters:  unit (IN)       - Device Number
 * Returns:     BCM_E_XXX
 */
void
_bcm_petra_rx_queue_cleanup(int unit)
{
    rx_queue_t *queue;

    /* empty the queue first */
    _bcm_petra_rx_free_queued(unit);

    /* mark the queue as empty */
    queue = DPP_RX_QUEUE(unit);
    if (!_Q_EMPTY(queue)) {
        queue->count = 0;
        queue->head = NULL;
        queue->tail = NULL;
    }
}
/* Function:    _bcm_petra_rx_cleanup
 * Purpose:     Remove all packets in queue and cleanup resources
 * Parameters:  unit (IN)       - Device Number
 * Returns:     BCM_E_XXX
 */
void
_bcm_petra_rx_cleanup(int unit)
{
    int i;
    bcm_pkt_t *pkt_block = NULL;
    bcm_pkt_t *pkt = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    
    /* cleanup the queue */
    _bcm_petra_rx_queue_cleanup(unit);

    /* Packets are allocated in a single contiguous block, packet index 0
     * points to the single block, don't free each individual packet
     */
    if (rx_ctl[unit]->all_pkts && rx_ctl[unit]->all_pkts[0]) {
        sal_memset(rx_ctl[unit]->all_pkts[0], 0, RX_PPC(unit) * sizeof(bcm_pkt_t));
    }

    pkt_block = rx_ctl[unit]->all_pkts[0];

    for (i=0; i < RX_PPC(unit); i++) {
        pkt = DPP_RX_PKT(unit, i) = pkt_block++;            
        pkt->unit = unit;
        pkt->pkt_data = &pkt->_pkt_data;
        pkt->blk_count = 1;
        pkt->_idx = i;
    }

    if (rx_ctl[unit]->pkt_load_ids) {
        sal_memset(rx_ctl[unit]->pkt_load_ids, 0, RX_PPC(unit) * sizeof(int));
    }    

    BCMDNX_FUNC_RETURN_VOID;
}

/*
 * Function:
 *      _bcm_petra_rx_shutdown
 * Purpose:
 *      Shutdown threads, free up resources without touching
 *      hardware
 * Parameters:
 *      unit - Unit reference
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_petra_rx_shutdown(int unit)
{
    if (0 == RX_IS_SETUP(unit)) {
        return (BCM_E_NONE);
    }

    /* Packets are allocated in a single contiguous block, packet index 0
     * points to the single block, don't free each individual packet
     */
    if (rx_ctl[unit]->all_pkts) {
        BCM_FREE(DPP_RX_PKT(unit, 0));
        BCM_FREE(rx_ctl[unit]->all_pkts);
        rx_ctl[unit]->all_pkts = NULL;            
    }

    if (rx_ctl[unit]->pkt_load_ids) {
        BCM_FREE(rx_ctl[unit]->pkt_load_ids);
        rx_ctl[unit]->pkt_load_ids = NULL;
    }

    /* cleanup dynamically allocated resources */
    if (rx_ctl[unit]->pkt_queue) {
        BCM_FREE(rx_ctl[unit]->pkt_queue);
        rx_ctl[unit]->pkt_queue = NULL;
    }
    
    if (rx_ctl[unit]->rc_callout) {
        BCM_FREE(rx_ctl[unit]->rc_callout);
        rx_ctl[unit]->rc_callout = NULL;
    }

    if (rx_ctl[unit]->rx_mutex) {
        BCM_FREE(rx_ctl[unit]->rx_mutex);
        rx_ctl[unit]->rx_mutex = NULL;
    }
    BCM_FREE(rx_ctl[unit]);
    rx_ctl[unit] = NULL;
    
    /* free up the rx pool */
    bcm_rx_pool_cleanup();
    
    return BCM_E_NONE;
}

/*
 * Function:    bcm_petra_rx_stop
 * Purpose:     Stop RX for the given unit; saves current configuration
 * Parameters:  unit (IN)   - device number top stop RX on
 *              cfg (IN)    - OUT Configuration copied to this parameter
 * Returns:     BCM_E_XXX
 * Notes:       This signals the thread to exit if this is the only unit 
 *              on which RX is started.
 */
int
bcm_petra_rx_stop(int unit, bcm_rx_cfg_t *cfg)
{
#ifdef BCM_PETRAB_SUPPORT
    int i;
#endif
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);   

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_stop(unit, cfg));
     }
#endif
#ifdef BCM_PETRAB_SUPPORT

    if (SOC_IS_PETRAB(unit)) {
        RX_INIT_CHECK(unit);

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Unit:%d Stopping RX\n"), unit));

    if (cfg != NULL) {    /* Save configuration */
        sal_memcpy(cfg, &rx_ctl[unit]->user_cfg, sizeof(bcm_rx_cfg_t));
    }

    /*
     * If no units are active, signal thread to stop and wait to
     * see it exit; simple semaphore
     */
    RX_INTR_LOCK;
    for (i = 0; i < BCM_CONTROL_MAX; i++) {
        if (!RX_IS_SETUP(i) || i == unit) {
            continue;
        }
        if (rx_ctl[i]->flags & BCM_RX_F_STARTED) {
            /* Some other unit is active */
            rx_ctl[unit]->flags &= ~BCM_RX_F_STARTED;
            RX_INTR_UNLOCK;
            _BCM_RX_SYSTEM_LOCK;
            rx_control.system_flags |= BCM_RX_CTRL_ACTIVE_UNITS_UPDATE;
            _BCM_RX_SYSTEM_UNLOCK;
            BCM_EXIT;
        }
    }

    /* RX not started on any other units.  Kill RX thread */
    if (rx_control.thread_running) {
        rx_control.thread_exit_complete = FALSE;
        rx_control.thread_running = FALSE;
        RX_INTR_UNLOCK;
        RX_THREAD_NOTIFY(unit);
        for (i = 0; i < 10; i++) {
            if (rx_control.thread_exit_complete) {
                break;
            }
            /* Sleep a bit to allow thread to stop */
            sal_usleep(500000);
        }
        if (!rx_control.thread_exit_complete) {
            LOG_WARN(BSL_LS_BCM_RX,
                     (BSL_META_U(unit,
                                 "Unit:%d RX Thread %p running after signaled to stop\n"),
                                 unit, (void *)rx_control.rx_tid));
        } else {
            rx_control.rx_tid = NULL;
        }
    } else {
        RX_INTR_UNLOCK;
    }

    rx_ctl[unit]->flags &= ~BCM_RX_F_STARTED;
    if (rx_control.system_lock != 0) {
        _BCM_RX_SYSTEM_LOCK;
        rx_control.system_flags |= BCM_RX_CTRL_ACTIVE_UNITS_UPDATE;
        _BCM_RX_SYSTEM_UNLOCK;
    }

    BCM_EXIT;
    }
#endif    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_clear
 * Purpose:     Stop RX on all modules and cleanup any resources
 * Parameters:  unit (IN)       - Device Number
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_clear(int unit)
{
#ifdef BCM_PETRAB_SUPPORT
    int rv;
#endif
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_clear(unit));

         /* free up the rx pool */
         bcm_rx_pool_cleanup();
     } 
#endif
#ifdef BCM_PETRAB_SUPPORT
   
    if (SOC_IS_PETRAB(unit)){ 
    
    if (RX_IS_SETUP(unit)) {
        rv = bcm_petra_rx_stop(unit, NULL);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Failed (%s) to stop RX on unit:%d. Ignoring and "
                                  "continuing to cleanup other units. \n"), 
                       bcm_errmsg(rv), unit));
        }

        rv = _bcm_petra_rx_shutdown(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Failed (%s) to shutdown RX on unit:%d. Ignoring and "
                                  "continuing to shutdown RX on other units. \n"), 
                       bcm_errmsg(rv), unit));
        }
    }

    /*Clear hardware if needed*/
    _bcm_dpp_rx_clear_virtual_traps_array(unit);


    BCM_EXIT;
    }
#endif    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_dpp_rx_tm_snoop_trap_reserve
 * Purpose
 *      Reserve the traps for ITMH snoop parsing in Arad
 * Parameters
 *      (in)  unit          = unit number
 *      (in)  num_snoops    = number of allocated snoop commands (and traps)
 * Returns
 *      rv = error code.
 *                    
 */
STATIC int _bcm_dpp_rx_tm_snoop_trap_reserve(
            int unit,
            uint8 is_set, /* 0: get function, 1: set function */
            int *num_snoops)
{
    int rv= BCM_E_NONE;
    int snoop_ndx;
    int trap_id;
    bcm_rx_trap_config_t config_get, config_set;
    
    BCMDNX_INIT_FUNC_DEFS;

    if (num_snoops == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL input pointer num_snoops")));
    }
    
    if(is_set && (*num_snoops > _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Too large required number (%d) of ITMH snoop commands to reserve."), *num_snoops));         
    }

    if (is_set == 0) {
        /* For get function, assume the maximum */
        *num_snoops = _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT;
    }

    /* Reserve / free the respective trap and snoop commands */
    for (snoop_ndx = 0; snoop_ndx < _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT; snoop_ndx++) {
        /* Reserve always the last traps */
        trap_id = SOC_PPC_TRAP_CODE_USER_DEFINED_LAST - _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT + 1 + snoop_ndx;

        /* Build the trap configuration */
        bcm_rx_trap_config_t_init(&config_set);
        config_set.snoop_cmnd = snoop_ndx;
        config_set.snoop_strength = PPC_TRAP_MGMT_SNP_STRENGTH_MAX;

        /*
         * See if the trap exists
         * If so, compare if same configuration, otherwise return error
         */
        if (_bcm_dpp_am_trap_is_alloced(unit, trap_id) == BCM_E_EXISTS) {
            bcm_rx_trap_config_t_init(&config_get);
            rv = bcm_petra_rx_trap_get(unit, trap_id, &config_get);
            BCMDNX_IF_ERR_EXIT(rv);
            /* Assumption that the config-port or config-group will be changed if allocated */
            if ((config_get.dest_group != 0) || (config_get.dest_port != 0) || (config_get.trap_strength != 0)) {
                if (is_set == 0) {
                    /* For get function, 1st trap not TM */
                    *num_snoops = snoop_ndx;
                    break;
                }
                else {
                    /* Error in set only if under the required number of snoops */
                    if (snoop_ndx < (*num_snoops)) {
                        LOG_ERROR(BSL_LS_BCM_RX,
                                  (BSL_META_U(unit,
                                              "Unit %d: Error (%s) Try to reserve a TM snoop traps for snoop %d "
                                              "and trap-id %d when already allocated \n"), 
                                   unit, bcm_errmsg(rv), snoop_ndx, trap_id));
                    }
                }
            }
            else { /* TM configuration on this snoop */
                if (is_set && (snoop_ndx >= (*num_snoops))) {
                    /* Free the TM configurations above the required number of snoops */
                    bcm_rx_trap_config_t_init(&config_set);
                    rv = bcm_petra_rx_trap_type_destroy(unit, trap_id);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
        else { /* trap not alloced */
            if (is_set == 0) {
                /* For get function, 1st trap not TM */
                *num_snoops = snoop_ndx;
                break;
            }
            /* Allocate the trap when not already reserved */
            rv = bcm_petra_rx_trap_type_create(
                    unit, 
                    BCM_RX_TRAP_WITH_ID, 
                    bcmRxTrapUserDefine,
                    &trap_id);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = bcm_petra_rx_trap_set(unit, trap_id, &config_set);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }


    
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}


/* Function:    bcm_petra_rx_control_get
 * Description: Get the status of specified RX feature.
 * Parameters:  unit (IN)   - Device number
 *              type (IN)   - RX control parameter
 *              value (OUT) - Current value of control parameter
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_control_get(int unit, bcm_rx_control_t type, int *value)
{
    BCMDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(type);
    COMPILER_REFERENCE(value);

    switch (type) {
    case bcmRxControlTmSnoopCount:
        if (SOC_IS_ARAD(unit)) {
            int rv;
            int num_snoops = 0;
            rv = _bcm_dpp_rx_tm_snoop_trap_reserve(unit, FALSE /*is_set*/,  &num_snoops);
            BCMDNX_IF_ERR_EXIT(rv);
            *value = num_snoops;
        } else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable rx control type:%d"), type));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable rx control type:%d"), type));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_control_set
 * Description: Set the status of specified RX feature.
 * Parameters:  unit (IN)   - Device number
 *              type (IN)   - RX control parameter
 *              value (IN)  - Value of control parameter to set to
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_control_set(int unit, bcm_rx_control_t type, int value)
{
    BCMDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(type);
    COMPILER_REFERENCE(value);

    switch (type) {
    case bcmRxControlTmSnoopCount:
        if (SOC_IS_ARAD(unit)) {
            int num_snoops = value;
            int rv;
            rv = _bcm_dpp_rx_tm_snoop_trap_reserve(unit, TRUE /*is_set*/,  &num_snoops);
            BCMDNX_IF_ERR_EXIT(rv);
        } else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable rx control type:%d"), type));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable rx control type:%d"), type));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_cosq_mapping_get
 * Description: Get the COS Queue mapping details for the index specified
 * Parameters:  unit (IN)           - Device number
 *              index (IN)          - Index into COSQ mapping table 
 *                                      0 - lowest match priority
 *              reasons (OUT)       - packet "reasons" bitmap
 *              reasons_mask (OUT)  - mask for packet "reasons" bitmap
 *              int_prio (OUT)      - internal priority value
 *              int_prio_mask (OUT) - mask for internal priority value
 *              packet_type (OUT)   - packet type bitmap (BCM_RX_COSQ_PACKET_TYPE_*)
 *              packet_type_mask(OUT)- mask for packet type bitmap
 *              cosq (OUT)          - CPU cos queue
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cosq_mapping_get(int unit, int index, bcm_rx_reasons_t *reasons, 
                              bcm_rx_reasons_t *reasons_mask, 
                              uint8 *int_prio, uint8 *int_prio_mask,
                              uint32 *packet_type, uint32 *packet_type_mask,
                              bcm_cos_queue_t *cosq)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_rx_cosq_mapping_get is not available yet")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_cosq_mapping_set
 * Description: Set the COS Queue mapping details for the specified index
 * Parameters:  unit (IN)           - Device number
 *              index (IN)          - Index into COSQ mapping table
 *                                      0 - lowest match priority
 *              reasons (IN)        - packet "reasons" bitmap
 *              reasons_mask (IN)   - mask for packet "reasons" bitmap
 *              int_prio (IN)       - internal priority value
 *              int_prio_mask (IN)  - mask for internal priority value
 *              packet_type (IN)    - packet type bitmap (BCM_RX_COSQ_PACKET_TYPE_*)
 *              packet_type_mask(IN)- mask for packet type bitmap
 *              cosq (IN)           - CPU cos queue
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cosq_mapping_set(int unit, int index, bcm_rx_reasons_t reasons, 
                              bcm_rx_reasons_t reasons_mask, 
                              uint8 int_prio, uint8 int_prio_mask,
                              uint32 packet_type, uint32 packet_type_mask,
                              bcm_cos_queue_t cosq)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_rx_cosq_mapping_set is not available yet")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_cpu_rate_get
 * Description: Get the CPU RX rate in packets per second
 * Parameters:  unit (IN)   - Device number
 *              pps (OUT)   - Rate in packets per second
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_cpu_rate_get(int unit, int *pps)
{
    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_ARAD_SUPPORT
  
    if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_cpu_rate_get(unit, pps));
    } 
#endif
#ifdef BCM_PETRAB_SUPPORT

    if (SOC_IS_PETRAB(unit)) { 

        
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_rx_cpu_rate_get is not available yet")));
    }
#endif     
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_rx_free_enqueue
 * Purpose:     Queue a packet to be freed by the RX thread.
 * Parameters:  unit (IN)   - Unit reference
 *              pkt (IN)    - pointer to packet data to be enqueued for free
 * Returns:     BCM_E_XXX
 * Notes:       This may be called in interrupt context to queue a packet to 
 *              be freed later.
 *              Assumes pkt_data is 32-bit aligned. Uses the first word of 
 *              the freed data as a "next" pointer for the free list.
 */
int
bcm_petra_rx_free_enqueue(int unit, void *pkt_data)
{
    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_free_enqueue(unit,pkt_data));
     }  
#endif
#ifdef BCM_PETRAB_SUPPORT

      if (SOC_IS_PETRAB(unit)) {
    DPP_RX_INIT_CHECK(unit);
    if (rx_control.rx_tid == NULL) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d RX thread not started yet. \n"), unit));
        BCM_RETURN_VAL_EXIT(BCM_E_INIT);
    }
    BCMDNX_NULL_CHECK(pkt_data);

    RX_INTR_LOCK;
    DPP_RX_FREE_LIST_NEXT(pkt_data) = (void *)(rx_ctl[unit]->free_list);
    rx_ctl[unit]->free_list = pkt_data;
    RX_INTR_UNLOCK;

    RX_THREAD_NOTIFY(unit);

    BCM_EXIT;
    }
#endif      
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    _bcm_petra_rx_discard_packet
 * Purpose:     Lowest priority registered handler that discards packet.
 * Parameters:  unit (IN)   - StrataSwitch Unit number.
 *              pkt (IN)    - The packet to handle
 *              cookie (IN) - Not used
 * Returns:     BCM_RX_*
 */

STATIC bcm_rx_t
_bcm_petra_rx_discard_packet(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int                 idx;
    DPP_HDR_hdr_type_t  hdr_type, hdr_len;
    char                buf[64];
    
    COMPILER_REFERENCE(cookie);
    if (pkt == NULL) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d invalid RX. pkt is null \n"), unit));
        return BCM_RX_INVALID;
    }
    hdr_type = pkt->_dpp_hdr_type;
    hdr_len = soc_dpp_hdr_len_get(unit, hdr_type);

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Unit:%d CPU RX'ed a packet. \n"), unit));
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Len:%5d   Header-type:%s \n"), 
                            pkt->tot_len, soc_dpp_hdr_type_to_name(unit, hdr_type)));
    if (hdr_type != DPP_HDR_none) {
        for (idx = 0; ((idx < hdr_len) && (idx < DPP_HDR_MAX_LEN)); idx++) {
            sal_sprintf((buf + ((idx % 10) * 3)), "%02x ", pkt->_dpp_hdr[idx]);
            if (!((idx + 1) % 10) || (idx == (hdr_len - 1))) {
                LOG_VERBOSE(BSL_LS_BCM_RX,
                            (BSL_META_U(unit,
                                        "Header[%02d-%02d]:      %s\n"), 
                                        (((idx - 9) > 0)? (idx - 9) : 0), idx, buf));
            }
        }
    }

    if ((pkt->_pkt_data.data != NULL) && (pkt->pkt_len > 0)) {
        for (idx = 0; idx < pkt->pkt_len; idx++) {
            sal_sprintf((buf + ((idx % 10) * 3)), "%02x ", 
                        pkt->_pkt_data.data[idx]);
            if (!((idx + 1) % 10) || (idx == (pkt->pkt_len - 1))) {
                LOG_VERBOSE(BSL_LS_BCM_RX,
                            (BSL_META_U(unit,
                                        "Data[%05d-%05d]:  %s\n"), (idx - 9), idx, buf));
            }
        }
    } else {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "Invalid data \n")));
    }

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Unit:%d No registered RX handler. Discarding packet.\n"), unit));
    
    ++(rx_ctl[unit]->dpkt);
    rx_ctl[unit]->dbyte += pkt->tot_len;
    
    return BCM_RX_HANDLED;
}



/* Function:    bcm_petra_rx_init
 * Purpose:     Initialize RX module on the specified unit
 * Parameters:  unit (IN)   - Unit reference
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_init(int unit)
{
    int rv;
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO default_info;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_RX));

    BCM_DPP_UNIT_CHECK(unit);
#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_init(unit));
     } 
#endif    

#ifdef BCM_PETRAB_SUPPORT
     if (SOC_IS_PETRAB(unit)) {
        if (rx_ctl[unit] != NULL) {
            /* re-initializing. Stop if RX is currently active */
            if (bcm_petra_rx_active(unit)) {
                LOG_VERBOSE(BSL_LS_BCM_RX,
                            (BSL_META_U(unit,
                                        "Unit:%d RX module already initialized. "
                                        "Stopping \n"), unit));
                rv = bcm_petra_rx_stop(unit, NULL);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_RX,
                              (BSL_META_U(unit,
                                          "Unit %d: Error (%s) stopping RX module\n"), 
                                          unit, bcm_errmsg(rv)));
                }
            }
        } else {
            BCMDNX_ALLOC(rx_ctl[unit], sizeof(rx_ctl_t), "rx_ctl");
            if (rx_ctl[unit] == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("sal_alloc failed")));
            }
            sal_memset(rx_ctl[unit], 0, sizeof(rx_ctl_t));

            rx_ctl[unit]->rx_mutex = sal_mutex_create("RX_MUTEX");
            if(rx_ctl[unit]->rx_mutex == NULL) {
               BCM_FREE(rx_ctl[unit]);
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("sal_alloc failed")));
            }
            BCMDNX_ALLOC(rx_ctl[unit]->rc_callout, sizeof(rx_callout_t), "rx_callout");
            if (rx_ctl[unit]->rc_callout == NULL) {
                BCM_FREE(rx_ctl[unit]->rx_mutex);
                BCM_FREE(rx_ctl[unit]);
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("sal_alloc failed")));
            }
            BCMDNX_ALLOC(rx_ctl[unit]->pkt_queue, (sizeof(rx_queue_t) * BCM_RX_COS), 
                      "rx_ctl--pkt_queue");
            if (rx_ctl[unit]->pkt_queue == NULL) {
                BCM_FREE(rx_ctl[unit]->rc_callout);
                BCM_FREE(rx_ctl[unit]->rx_mutex);
                BCM_FREE(rx_ctl[unit]);
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("sal_alloc failed")));
            }
            sal_memset(rx_ctl[unit]->pkt_queue, 0,
                       sizeof(rx_queue_t) * BCM_RX_COS);
            RX_LOCK(unit);
            /* set user config to defaults */
            sal_memcpy(&rx_ctl[unit]->user_cfg, &_bcm_petra_default_rx_cfg, 
                       sizeof(bcm_rx_cfg_t));
            /* Initialize the RX queue */
            (DPP_RX_QUEUE(unit))->head = NULL;
            (DPP_RX_QUEUE(unit))->tail = NULL;
            (DPP_RX_QUEUE(unit))->count = 0;
            (DPP_RX_QUEUE(unit))->max_len = RX_Q_MAX_DFLT;

            /* setup the discard handler */
            SETUP_RCO(rx_ctl[unit]->rc_callout, "Discard", 
                     _bcm_petra_rx_discard_packet, BCM_RX_PRIO_MIN, NULL, NULL, 0);
            rx_ctl[unit]->hndlr_cnt++;

            rx_ctl[unit]->rx_thread_pri = soc_property_get(unit, 
                                                           spn_BCM_RX_THREAD_PRI, 
                                                           RX_THREAD_PRI_DFLT);

            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "Unit:%d RX initialized. \n"), unit));
            RX_UNLOCK(unit);
        }
     }
#endif /* soc_petra-B */

    /* A.M initliaztion - Start*/
    _bcm_dpp_rx_trap_to_ppd_init(unit);
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(&default_info);

    soc_sand_dev_id = (unit);
    if (!SOC_WARM_BOOT(unit)) {
        soc_sand_rv =  soc_ppd_trap_to_eg_action_map_set(soc_sand_dev_id,SOC_PPD_TRAP_EG_TYPE_ALL,SOC_PPD_TRAP_EG_NO_ACTION);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /*set invalid out tm port to discard pkts*/
        soc_sand_rv =  soc_ppd_trap_to_eg_action_map_set(soc_sand_dev_id,SOC_PPD_TRAP_EG_TYPE_INVALID_OUT_PORT,SOC_PPD_TRAP_ACTION_PKT_DISCARD_ID);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /*points all ports to default profile= NO ACTION*/
    /*SW config*/

    rv = _bcm_dpp_am_template_trap_egress_init(unit,_BCM_DPP_TRAP_EG_NO_ACTION, &default_info);/*profile = 0, trap-info = default */   
    BCMDNX_IF_ERR_EXIT(rv);
    
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_PETRAB(unit)) {    
        BCM_SAND_IF_ERR_EXIT(_bcm_dpp_rx_clear_virtual_traps_array(unit));
    }
#endif 
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        /*
         * Allocate the last 16 User-defined traps for TM Snoop field parsing:
         * - The Arad.ITMH has a snoop field that must be mapped through PMF
         * - the PMF maps this field to a snooop command (8b) that is mapped to 
         * a snoop code (4b) afterwards
         * - These snoop codes are in the same space than the traps, thus these
         * user-defined traps have an invalid snoop mapping
         * - Thus we allocate the 16 last User-defined traps for TM 
         * at init. The user can reduce the size with bcmRxControlTmSnoopCount
         */ 
        if (!SOC_WARM_BOOT(unit)) {
            int num_snoops = _BCM_RX_NUM_SNOOPS_FOR_TM_AT_INIT;
            rv = _bcm_dpp_rx_tm_snoop_trap_reserve(unit, TRUE /*is_set*/,  &num_snoops);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

#endif /* BCM_ARAD_SUPPORT */


    /* Do not Change!
     * Capture a User Defined SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS trap
     * Trap destination is to the recycle and is used for IPv6-UC-with-RPF FLP Program
     * Since FLP is initialized first need to make sure to capture this trap code before the application 
     */ 
    if (SOC_DPP_PP_ENABLE(unit)) 
    {
        if (!SOC_WARM_BOOT(unit))
        {
            if (SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists) 
            {
                int trap_code;
                /*
                 * Capture the SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS trap 
                 */
                trap_code = SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS;
                rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code);
                if (rv != BCM_E_NONE) 
                {
                    /* Trap is already captured */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("IPv6 UC with RPF 2pass trap:0x% already captured, error %d unit %d"), trap_code,rv,unit));
                }
            }
        }
    }

exit:
   BCMDNX_FUNC_RETURN;
}
    

/* Function:    bcm_petra_rx_partial_init
 * Purpose:     Initialize RX module on the specified unit without thread creating 
 * Parameters:  unit (IN)   - Unit reference
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_rx_partial_init(int unit)
{
    int rv;
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO default_info;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_RX));

    soc_sand_dev_id = (unit);

    /* A.M initliaztion - Start*/
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(&default_info);

    soc_sand_dev_id = (unit);
    if (!SOC_WARM_BOOT(unit)) {
        soc_sand_rv =  soc_ppd_trap_to_eg_action_map_set(soc_sand_dev_id,SOC_PPD_TRAP_EG_TYPE_ALL,SOC_PPD_TRAP_EG_NO_ACTION);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /*set invalid out tm port to discard pkts*/
        soc_sand_rv =  soc_ppd_trap_to_eg_action_map_set(soc_sand_dev_id,SOC_PPD_TRAP_EG_TYPE_INVALID_OUT_PORT,SOC_PPD_TRAP_ACTION_PKT_DISCARD_ID);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /*points all ports to default profile= NO ACTION*/
    /*SW config*/
    rv = _bcm_dpp_am_template_trap_egress_init(unit,_BCM_DPP_TRAP_EG_NO_ACTION, &default_info);/*profile = 0, trap-info = default */   
    BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_PETRAB(unit)) {        
        BCM_SAND_IF_ERR_EXIT(_bcm_dpp_rx_clear_virtual_traps_array(unit));
    }
#endif 
exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_rx_queue_max_get
 * Purpose:     Get maximum cos queue number for the device.
 * Parameters:  unit (IN)   - device number. 
 *              cosq (OUT)  - Maximum queue priority.
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_queue_max_get(int unit, bcm_cos_queue_t *cosq)
{
    BCMDNX_INIT_FUNC_DEFS;
    
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_max_get(unit, cosq));
    } 
#endif
#ifdef BCM_PETRAB_SUPPORT

    if (SOC_IS_PETRAB(unit)) {

        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_rx_queue_max_get is not available")));
    }
#endif    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_rx_queue_packet_count_get
 * Purpose:     Get number of packets awaiting processing in the specific 
 *              device/queue.
 * Parameters:  unit (IN)           - device number. 
 *              cosq (IN)           - Queue priority
 *              packet_count (OUT)  - Number of packets awaiting processing
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_queue_packet_count_get(int unit, bcm_cos_queue_t cosq, 
                                    int *packet_count)
{
    BCMDNX_INIT_FUNC_DEFS;
    
#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_packet_count_get(unit, cosq, packet_count));
    }  
#endif    

#ifdef BCM_PETRAB_SUPPORT

    if (SOC_IS_PETRAB(unit)) {

        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_rx_queue_packet_count_get is not available")));
    }
#endif    

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_rx_rate_get
 * Purpose:     Get the configured CPU RX rate in pps
 * Parameters:  unit (IN)   - device number. 
 *              pps (OUT)   - RX rate in packets per second rate
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_rate_get(int unit, int *pps)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_rate_get(unit, pps));
     } 
#endif
  #ifdef BCM_PETRAB_SUPPORT

    if (SOC_IS_PETRAB(unit)) {
    RX_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(pps);

    *pps = RX_PPS(unit);
    BCM_EXIT;
    }
 #endif   
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_rx_reasons_policer_set
 * Purpose:     Set the policer for the specified rx reasons
 * Parameters:  unit (IN)       - device number. 
 *              rx_reasons(IN)  - RX reasons for which to set the policer
 *              pol_id (IN)     - Policer ID to set to
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_reasons_policer_set(int unit, bcm_rx_reasons_t reasons, 
                                 bcm_policer_t pol_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_rx_reasons_policer_set is not available yet")));
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:    bcm_petra_rx_register
 * Purpose:     Register an upper layer driver
 * Parameters:  unit (IN)   - device number.
 *              name (IN)   - constant character string for debug purposes.
 *              cb_f (IN)   - callback function when packet arrives
 *              pri (IN)    - priority of handler in list (0:lowest priority)
 *              cookie (IN) - cookie passed to driver when packet arrives.
 *              flags (IN)  - Register for interrupt or non-interrupt callback
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_register(int unit, const char *name, bcm_rx_cb_f cb_f, uint8 pri, 
                      void *cookie, uint32 flags)
{
#ifdef BCM_PETRAB_SUPPORT
    volatile rx_callout_t   *rco = NULL, *list_rco, *prev_rco;
#endif
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT

    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_register(unit, name, cb_f, pri, cookie, flags));
     }  
#endif
#ifdef BCM_PETRAB_SUPPORT

     if (SOC_IS_PETRAB(unit)) {   


    RX_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(name);
    BCMDNX_NULL_CHECK(cb_f);
    if (SOC_IS_PETRAB(unit)){

        /* check for any un-supported flags */
        if (flags & (~BCM_RCO_F_INTR)) {
            LOG_WARN(BSL_LS_BCM_RX,
                     (BSL_META_U(unit,
                                 "Unit:%d bcm_petra_rx_register: Unsupported flags (%x)\n"),
                                 unit, (flags & (~BCM_RCO_F_INTR))));
        }
    }
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Registering %s on %d, flags 0x%x%s\n"),
                            name, unit, flags, flags & BCM_RCO_F_INTR ? "(intr)" : ""));

    /* First check if the same is already registered */
    for (list_rco = rx_ctl[unit]->rc_callout; list_rco;
         list_rco = list_rco->rco_next) {
        if ((list_rco->rco_function == cb_f) && 
            (list_rco->rco_priority == pri)) {
            if ((list_rco->rco_flags == flags) && 
                (list_rco->rco_cookie == cookie)) {
                /* registering the same CB @ same priority along with same 
                 * cookie and flags. Do nothing.
                 */
                BCM_EXIT;
            }
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d has %s already registered with diff params\n"), 
                                  unit, name));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }
    }

    BCMDNX_ALLOC(rco, sizeof(*rco), "rx_callout");
    if (rco == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    SETUP_RCO(rco, name, cb_f, pri, cookie, NULL, flags);

    RX_LOCK(unit);
    RX_INTR_LOCK;
    /*
     * Find correct place to insert handler, this code assumes that
     * the discard handler has been registered on init.  Handlers
     * of the same priority are placed in the list in the order
     * they are registered
     */
    prev_rco = NULL;
    for (list_rco = rx_ctl[unit]->rc_callout; list_rco;
         list_rco = list_rco->rco_next) {
        if (list_rco->rco_priority < pri) {
            break;
        }
        prev_rco = list_rco;
    }

    if (prev_rco) { 
        /* Insert after prev_rco */
        rco->rco_next = prev_rco->rco_next;
        prev_rco->rco_next = rco;
    } else { 
        /* Insert first */
        rco->rco_next = list_rco;
        rx_ctl[unit]->rc_callout = rco;
    }

    if (flags & BCM_RCO_F_INTR) {
        rx_ctl[unit]->hndlr_intr_cnt++;
    } else {
        rx_ctl[unit]->hndlr_cnt++;
    }
    RX_INTR_UNLOCK;
    RX_UNLOCK(unit);

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Unit:%d %s registered %s%s.\n"),
                            unit, name, prev_rco ? "after " : "first", 
                 prev_rco ? prev_rco->rco_name : ""));

    BCM_EXIT;
    }
#endif     
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:    bcm_petra_rx_unregister
 * Purpose:     Un-register an upper layer driver
 * Parameters:  unit (IN)   - device number.
 *              cb_f (IN)   - callback function to unregister
 *              pri (IN)    - priority of handler in list (0:lowest priority)
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_unregister(int unit, bcm_rx_cb_f cb_f, uint8 pri)
{
#ifdef BCM_PETRAB_SUPPORT
    volatile rx_callout_t   *rco;
    volatile rx_callout_t   *prev_rco = NULL;
    const char              *name;
    uint32                  flags;
#endif

    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_unregister(unit, cb_f, pri));
     } 
#endif
#ifdef BCM_PETRAB_SUPPORT

if (SOC_IS_PETRAB(unit)) {   
    DPP_RX_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(cb_f);

    RX_LOCK(unit);
    RX_INTR_LOCK;
    for (rco = rx_ctl[unit]->rc_callout; rco; rco = rco->rco_next) {
        if ((rco->rco_function == cb_f) && (rco->rco_priority == pri)) {
            break;
        }
        prev_rco = rco;
    }
    if (!rco) {
        RX_INTR_UNLOCK;
        RX_UNLOCK(unit);
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("rco - not found")));
    }
    name = rco->rco_name;
    flags = rco->rco_flags;
    if (!prev_rco) {  /* First elt on list */
        rx_ctl[unit]->rc_callout = rco->rco_next;
    } else {          /* skip current */
        prev_rco->rco_next = rco->rco_next;
    }
    if (flags & BCM_RCO_F_INTR) {
        rx_ctl[unit]->hndlr_intr_cnt--;
    } else {
        rx_ctl[unit]->hndlr_cnt--;
    }
    RX_INTR_UNLOCK;
    RX_UNLOCK(unit);

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Unit:%d RX(%s) unregistered\n"), 
                            unit, name));
   BCM_FREE(rco);
    
    BCM_EXIT;
    }
#endif
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

#if defined(BROADCOM_DEBUG)
/*
 * Function:    bcm_petra_rx_show
 * Purpose:     Un-register an upper layer driver
 * Parameters:  unit (IN)   - device number.
 *              cb_f (IN)   - callback function to unregister
 *              pri (IN)    - priority of handler in list (0:lowest priority)
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_show(int unit)
{
    volatile rx_callout_t   *rco;
    sal_usecs_t             cur_time;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    if (!RX_INIT_DONE(unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "Unit:%d RX not initialized. \n"), unit));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("TODO err message")));
    }

    cur_time = sal_time_usecs();
    LOG_CLI((BSL_META_U(unit,
                        "RX Info @ time=%u: %sstarted. Thread is %srunning.\n"
             "    +verbose for more info\n"),cur_time, 
             RX_UNIT_STARTED(unit) ? "" : "not ",
             rx_control.thread_running ? "" : "not "));
    LOG_CLI((BSL_META_U(unit,
                        "    Pkt Size %d. All COS PPS %d \n"),
             RX_PKT_SIZE(unit), RX_PPS(unit)));
    LOG_CLI((BSL_META_U(unit,
                        "    Cntrs:  Pkts %d. Last start %d. Tunnel %d. Owned %d.\n"
             "        Bad Hndlr %d. No Hndlr %d. Not Running %d.\n"
                        "        Thrd Not Running %d.\n"),
             rx_ctl[unit]->tot_pkts, rx_ctl[unit]->pkts_since_start,
             rx_ctl[unit]->tunnelled, rx_ctl[unit]->pkts_owned,
             rx_ctl[unit]->bad_hndlr_rv, rx_ctl[unit]->no_hndlr,
             rx_ctl[unit]->not_running, rx_ctl[unit]->thrd_not_running));

    LOG_CLI((BSL_META_U(unit,
                        "  Registered callbacks:\n")));
    /* Display callouts and priority in order */
    for (rco = rx_ctl[unit]->rc_callout; rco; rco = rco->rco_next) {
        LOG_CLI((BSL_META_U(unit,
                            "        %-10s Priority=%3d%s. "
                 "Argument=0x%x. COS 0x%x%08x.\n"),
                 rco->rco_name, (uint32)rco->rco_priority,
                 (rco->rco_flags & BCM_RCO_F_INTR) ? " Interrupt" : "",
                 PTR_TO_INT(rco->rco_cookie),
                 rco->rco_cos[1], rco->rco_cos[0]));
        LOG_CLI((BSL_META_U(unit,
                            "        %10s Packets handled %u, owned %u.\n"), " ",
                 rco->rco_pkts_handled, rco->rco_pkts_owned));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BROADCOM_DEBUG */

/*
 * Function:    _bcm_petra_rx_thread_pkt_process
 * Purpose:     Process a single RX packet. Registered callbacks are called
 * Parameters:  param (IN)  - device number
 *              pkt (IN)    - packet structure to process
 */
void
_bcm_petra_rx_thread_pkt_process(int unit, bcm_pkt_t *pkt)
{
    volatile rx_callout_t   *rco;
    bcm_rx_t                handler_rc;
    int                     handled;

    if (pkt == NULL) {
        return;
    }
    if (rx_ctl[unit]->hndlr_cnt == 0) {
        /* No callbacks...free the packet and return */
        rx_ctl[unit]->no_hndlr++;
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d RX thread Error. No handlers, returning\n"), unit));
        bcm_petra_rx_free(unit, pkt->pkt_data->data);
        pkt->pkt_data->data = NULL;
        DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
        return;
    }

    handled = FALSE;
    RX_LOCK(unit);  /* Can't modify handler list while doing callbacks */
    /* Loop through registered drivers until packet consumed */
    for (rco = rx_ctl[unit]->rc_callout; rco; rco = rco->rco_next) {
        if (rco->rco_flags & BCM_RCO_F_INTR) { /* Non interrupt context */
            continue;
        }
        handler_rc = rco->rco_function(unit, pkt, rco->rco_cookie);

        switch (handler_rc) {
        case BCM_RX_NOT_HANDLED:
            break;                      /* Next callout */
        case BCM_RX_HANDLED:
            handled = TRUE;
            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "Unit:%d pkt handled "
                                    "by %s\n"), unit, rco->rco_name));
            rco->rco_pkts_handled++;
            break;
        case BCM_RX_HANDLED_OWNED:
            handled = TRUE;
            pkt->_pkt_data.data = NULL;
            pkt->alloc_ptr = NULL;
            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "Unit:%d pkt handled "
                                    "and owned by %s\n"), unit, rco->rco_name));
            rx_ctl[unit]->pkts_owned++;
            rco->rco_pkts_owned++;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d RX packet invalid callback return value=%d\n"), 
                                  unit, handler_rc));
            break;
        }

        if (handled) {
            break;
        }
    }
    RX_UNLOCK(unit);

    if (pkt->pkt_data->data) {
        bcm_petra_rx_free(unit, pkt->pkt_data->data);
        pkt->pkt_data->data = NULL;
    }
    DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);

    if (!handled) {
        /* Internal error as discard should have handled packet */
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d RX Error, no handler processed packet. Port %d\n"),
                              unit, pkt->rx_port));
    }
}

/*
 * Function:    _bcm_petra_rx_thread_pkts_process
 * Purpose:     Handles the RX pkt queue on the unit
 * Parameters:  param (IN)  - device number
 * Returns:     BCM_E_XXX
 * Notes:       Processes all the packets in the queue.
 */
int
_bcm_petra_rx_thread_pkts_process(int unit)
{
    int         count;
    rx_queue_t  *queue;
    bcm_pkt_t   *pkt_list = NULL;
    bcm_pkt_t   *next_pkt;

    BCMDNX_INIT_FUNC_DEFS;
    queue = DPP_RX_QUEUE(unit);
    if (queue == NULL) {
        BCM_EXIT; /* Should not hit this. Some internal error */
    }
    count = queue->count;
    if (count == 0) {
        BCM_EXIT;  /* queue empty */
    }
    _Q_STEAL(queue, pkt_list, count);
    if (pkt_list == NULL) {
        /* no packets. but queue count does not say so? */
        BCM_EXIT;   
    }
    /* Process all packets */
    while (pkt_list) {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "Unit:%d RX dequeued "
                                "packet (%p)\n"), unit, pkt_list));
        next_pkt = pkt_list->_next;

        /* No rate limiting imposed */
        _bcm_petra_rx_thread_pkt_process(unit, pkt_list);
        pkt_list = next_pkt;

        _BCM_RX_CHECK_THREAD_DONE;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Sleep .01 seconds when running. */
#define RUNNING_SLEEP 10000

/* Sleep .5 seconds when not running. */

/* Check if given quantity is < cur sleep secs; set to that value if so */
#define BASE_SLEEP_VAL RUNNING_SLEEP

/* Set sleep to base value */
#define INIT_SLEEP    rx_control.sleep_cur = BASE_SLEEP_VAL

/* Sleep .1 seconds before polling again */
#define RX_POLL_SLEEP 100000

/* sleep .05 seconds before checking for another packet again */
#define RX_POLL_WAIT_SLEEP 50000

/* sleep for 0.01 seconds before retrying for buffer available */
#define RX_POLL_WAIT_BUFFER 10000

/* Lower sleep time if val is < current */
#define SLEEP_MIN_SET(val)                                           \
    (rx_control.sleep_cur = ((val) < rx_control.sleep_cur) ?         \
     (val) : rx_control.sleep_cur)

#define CHECK_RX_THREAD_DONE    \
    if (!rx_control.thread_running) goto rx_thread_done

/*
 * Function:    _bcm_petra_rx_pkt_thread
 * Purpose:     User level thread that handles packets
 * Parameters:  param (IN)  - unused
 * Notes:   
 *      This thread can get awoken for the following reasons:
 *      1.  Packets ready to process
 *      2.  RX stop is called ending processing
 *      3.  Interrupt processing has handled packets 
 *      4.  Periodic check in case memory has freed up
 */
STATIC void
_bcm_petra_rx_pkt_thread(void *param)
{
    int unit;
    COMPILER_REFERENCE(param);

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META("RX Packet thread starting\n")));

    INIT_SLEEP;

    /* Sleep on sem */
    while (rx_control.thread_running) {
        CHECK_RX_THREAD_DONE;
        for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
            if (!RX_UNIT_STARTED(unit)) {
                continue;
            }
            CHECK_RX_THREAD_DONE;

            while (_bcm_petra_rx_thread_pkts_process(unit)) {
                CHECK_RX_THREAD_DONE;

                /* free pkt data on the free list */
                _bcm_petra_rx_free_queued(unit);
            }

            /* Free queued packets */
            _bcm_petra_rx_free_queued(unit);
        }

        SLEEP_MIN_SET(BASE_SLEEP_VAL);
        LOG_DEBUG(BSL_LS_BCM_RX,
                  (BSL_META("RX thread Sleeping %d "
"at %u\n"), rx_control.sleep_cur, sal_time_usecs()));
    
        sal_sem_take(rx_control.pkt_notify, rx_control.sleep_cur);
        rx_control.pkt_notify_given = FALSE;

        LOG_DEBUG(BSL_LS_BCM_RX,
                  (BSL_META("RX thread Woke %u\n"),
sal_time_usecs()));
        INIT_SLEEP;
    }

rx_thread_done:
    for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
        if (RX_IS_SETUP(unit)) {
            _bcm_petra_rx_cleanup(unit);
        }
    }
    rx_control.thread_exit_complete = TRUE;
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META("RX: Packet thread exitting\n")));

    sal_thread_exit(0);
}


/* Function:    _bcm_petra_rx_init_buffer_mgt
 * Purpose:     Initialize the pkt buffer pool
 * Parameters:  unit (IN)   - device number
 * Returns:     BCM_E_XXX
 * Notes:       Should only be called once...If not, overrides current buffer
 */
int
_bcm_petra_rx_init_buffer_mgt(int unit)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_pkt_t *pkt_block = NULL;
    bcm_pkt_t *pkt = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    RX_LOCK(unit);
    if (BCM_SUCCESS(rv)) {
        BCMDNX_ALLOC(rx_ctl[unit]->all_pkts, RX_PPC(unit) * sizeof(bcm_pkt_t*), "Rx pkt array");
        if (rx_ctl[unit]->all_pkts == NULL) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d Failed(%s) to alloc packet descriptors\n"), 
                                  unit, bcm_errmsg(rv)));
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(rv)) {
        BCMDNX_ALLOC(rx_ctl[unit]->pkt_load_ids, RX_PPC(unit) * sizeof(int), "RxPktLoadIDs");
        if (rx_ctl[unit]->pkt_load_ids == NULL) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d Failed(%s) to alloc RX load IDs\n"), 
                                  unit, bcm_errmsg(rv)));
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(rv)) {
        BCMDNX_ALLOC(pkt_block, RX_PPC(unit) * sizeof(bcm_pkt_t), "Rx pkt block");
        if (pkt_block == NULL) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d failed to alloc packet descriptor block\n"), 
                                  unit));
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_FAILURE(rv)) {
        BCM_FREE(rx_ctl[unit]->all_pkts);
        BCM_FREE(rx_ctl[unit]->pkt_load_ids);

        RX_UNLOCK(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    sal_memset(pkt_block, 0, RX_PPC(unit) * sizeof(bcm_pkt_t));
    /* pkt_load_ids is being used to keep track of pkt_block usage */
    sal_memset(rx_ctl[unit]->pkt_load_ids, 0, RX_PPC(unit) * sizeof(int));
    rx_ctl[unit]->all_pkts[0] = pkt_block; /* to appease coverity */

    for (i=0; i < RX_PPC(unit); i++) {
        pkt = DPP_RX_PKT(unit, i) = pkt_block++;            
        pkt->unit = unit;
        pkt->pkt_data = &pkt->_pkt_data;
        pkt->blk_count = 1;
        pkt->_idx = i;
    }

    RX_UNLOCK(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_rx_check_buffer_level
 * Purpose:     Checks the buffer pool and allocates if needed.
 * Parameters:  unit (IN)   - device number
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_rx_check_buffer_level(int unit)
{
    int rv = BCM_E_NONE;
    bcm_pkt_t *pkt = NULL;
    int i;

    BCMDNX_INIT_FUNC_DEFS;
    /* allocate packet descriptors and packet buffers for RX */
    if (rx_ctl[unit]->all_pkts == NULL) {
        rv = _bcm_petra_rx_init_buffer_mgt(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    RX_LOCK(unit);
    /* fill in packet data buffers if needed */
    for (i=0; i < RX_PPC(unit); i++) {
        void *pkt_data = NULL;
        pkt = DPP_RX_PKT(unit, i);

        if ((pkt->pkt_data->data == NULL) && (DPP_RX_PKT_USED(unit, i) == 0)) {
            rv = bcm_petra_rx_alloc(unit, rx_ctl[unit]->user_cfg.pkt_size, 
                                    0, &pkt_data);

            if ((pkt_data == NULL) || (BCM_FAILURE(rv))) {
                LOG_ERROR(BSL_LS_BCM_RX,
                          (BSL_META_U(unit,
                                      "Unit:%d Failed (%s) to allocate pkt buffer\n"), 
                                      unit, bcm_errmsg(rv)));
                RX_UNLOCK(unit);
                BCM_RETURN_VAL_EXIT(BCM_E_MEMORY);
            }

            pkt->_pkt_data.data = pkt_data;
            pkt->_pkt_data.len  = rx_ctl[unit]->user_cfg.pkt_size;
            pkt->alloc_ptr = pkt_data;
        }
    }

    RX_UNLOCK(unit);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_rx_pkt_buffer_get
 * Purpose:     Returns a bcm_pkt_t buffer
 * Parameters:  unit (IN)   - device number
 *              pkt (out)   - ptr to ptr to pkt structure
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_rx_pkt_buffer_get(int unit, bcm_pkt_t **p_pkt)
{
    int rv, idx;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(p_pkt);

    rv = _bcm_petra_rx_check_buffer_level(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d Error (%s) filling RX buffers\n"), 
                              unit, bcm_errmsg(rv)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = BCM_E_RESOURCE;
    RX_LOCK(unit);
    for (idx=0; ((idx < RX_PPC(unit)) && (BCM_FAILURE(rv))); idx++) {
        if (DPP_RX_PKT_USED(unit, idx) == 0) {
            /* bcm_pkt_t struct is free. return it */
            *p_pkt = DPP_RX_PKT(unit, idx);
            DPP_RX_PKT_USED(unit, idx) = 1;
            rv = BCM_E_NONE;
        }
    }
    RX_UNLOCK(unit);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    _bcm_petra_rx_pkt_available
 * Purpose:     Check if a packet is in CPU RX path for Async interface
 * Parameters:  param (IN)  - unused
 * Returns:     1 if pkt is to be RX'ed from HW, 0 otherwise
 */
int
_bcm_petra_rx_pkt_available(int unit, uint32 *val)
{
    int     avail = 0;

#if defined(BCM_88640_A0) || defined (BCM_88650_A0)
#ifdef  BCM_88640_A0       
    int     rv;
    uint32  regval = 0;
#endif    

    /* assume not available */
    avail = 0;
    
    if (val == NULL) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d Internal error. Null input param \n"), unit));
        return avail;
    }

    if (SOC_IS_PETRAB(unit)) {

#ifdef  BCM_88640_A0       
        rv = READ_CPUASYNCHRONOUSPACKETDATAr(unit, &regval);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d RX poll thread failed (%s) to read "
                                  "CPUASYNCHRONOUSPACKETDATA reg\n"), unit, bcm_errmsg(rv)));
        } else {
            if ((soc_reg_field_get(unit,CPUASYNCHRONOUSPACKETDATAr,regval,TDVf)) && 
                (soc_reg_field_get(unit,CPUASYNCHRONOUSPACKETDATAr,regval,TCMDf) == 
                 DPP_RX_CPU_ASYNC_TCMD_SOP)) {
                LOG_VERBOSE(BSL_LS_BCM_RX,
                            (BSL_META_U(unit,
                                        "Unit:%d RX poll "
                                        "thread found a packet waiting for CPU\n"), unit));
                avail = 1;
                *val = regval;
            }
        }
#endif  
        }
    
    
#endif /*defined(BCM_88640_A0)*/
    return avail;
}

/*
 * Function:    _bcm_petra_rx_pkt_enqueue
 * Purpose:     Grab a packet destined for CPU RX from chip's async interface
 * Parameters:  param (IN)  - unused
 */
void
_bcm_petra_rx_pkt_enqueue(int unit, uint32 regval)
{
#if defined(BCM_88640_A0)

    int                         rv=0, eop, tcmd, tdv, num_bytes, tdata;
    int                         soc_sand_rv, hdr_len, check_mc;
    int                         hdr_len_read, is_mc, retry_count;
    bcm_pkt_t                   *pkt = NULL;
    rx_queue_t                  *queue;
    SOC_TMC_STACK_GLBL_INFO          stk_info;
    SOC_TMC_PORTS_FTMH_EXT_OUTLIF   ext_info = SOC_TMC_PORTS_FTMH_NOF_EXT_OUTLIFS;
    uint8                       buff[2048]; 
    int                         buff_len = 0;
    SOC_PPD_TRAP_PACKET_INFO        packet_info;
    bcm_module_t                mod_id;
    bcm_port_t                  mod_port;

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Unit:%d RX enqueuing "
                            "a packet\n"), unit));

    retry_count = 0;
    while ((retry_count++ < DPP_RX_BUF_CHECK_RETRY_COUNT)) {
        rv = _bcm_petra_rx_pkt_buffer_get(unit, &pkt);
        if (BCM_SUCCESS(rv)) {
            break;
        }
        sal_usleep(RX_POLL_WAIT_BUFFER);
    }
    if (BCM_FAILURE(rv) || (pkt == NULL)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d RX buffer pool full. \n"), unit));
        return;
    }

    /* FTMH base header is always present */
    hdr_len = DPP_HDR_FTMH_BASE_LEN;

    /* check if OUTLIF extension header is present */    
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_ftmh_extension_get,(unit, &ext_info)));
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d failed to get FTMH outlif extension info. Error:%d "
                              "(%s) \n"), unit, rv, bcm_errmsg(rv)));
        DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
        return;
    }
    is_mc = 0;
    if (ext_info == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC) {
        check_mc = 1;
    } else {
        /* SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER & 
           SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS */
        check_mc = 0;
        if (ext_info == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS) {
            hdr_len += 2;
        }
    }
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "FTMH Outlif ext: %d \n"), 
                            ext_info));

    SOC_TMC_STACK_GLBL_INFO_clear(&stk_info);

    /* check if LB-Key extension header is present */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_stack_global_info_get,(unit, &stk_info)));
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d failed to get FTMH LB-key ext header info. Error:%d"
                              " (%s) \n"), unit, rv, bcm_errmsg(rv)));
        DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
        return;
    }

    if ((stk_info.max_nof_tm_domains == SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_8) || 
        (stk_info.max_nof_tm_domains == SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_16)) {
        /* LB extension is enabled when 8 or 16 TM domains are present */
        hdr_len += 2;
    }
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "FTMH hdr len:%d \n"), hdr_len));

    
    /* following code is for Asynchronous CPU RX interface */
    eop = 0;
    pkt->tot_len = 0;
    pkt->pkt_len = 0;
    hdr_len_read = 0;
    while ((!eop) && (BCM_SUCCESS(rv))) {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "Regval: %08x \n"), regval));
        tcmd = soc_reg_field_get(unit, CPUASYNCHRONOUSPACKETDATAr, regval,
                                 TCMDf);
        tdv = soc_reg_field_get(unit, CPUASYNCHRONOUSPACKETDATAr, regval, 
                                TDVf);
        if (tdv) {
            num_bytes = 2;
            if (tcmd == DPP_RX_CPU_ASYNC_TCMD_EOP_1BV) {
                num_bytes = 1;
                eop = 1;
            } else if (tcmd == DPP_RX_CPU_ASYNC_TCMD_EOP_2BV) {
                eop = 1;
            }
        } else {
            /* some error ? */
            eop = 1;
            num_bytes = 0;
            rv = BCM_E_INTERNAL;
        }
        if (num_bytes) {
            tdata = soc_reg_field_get(unit, CPUASYNCHRONOUSPACKETDATAr, 
                                      regval, TDATAf);
            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "Packet content: "
                                    "%04x. num_bytes:%d \n"), tdata, num_bytes));
                                                 
            sal_memcpy((buff + buff_len), 
                           (((char *)(&tdata)) + 2), num_bytes);
            buff_len += num_bytes;

            if (hdr_len_read < hdr_len) {
                LOG_VERBOSE(BSL_LS_BCM_RX,
                            (BSL_META_U(unit,
                                        "Header:%04x "
                                        "hdr_len_read:%d \n"), tdata, hdr_len_read));
                if (num_bytes < 2) {
                    LOG_ERROR(BSL_LS_BCM_RX,
                              (BSL_META_U(unit,
                                          "Unit:%d Error rx'ing packet. Packet size"
                                          "< FTMH header length \n"), unit));
                    rv = BCM_E_INTERNAL;
                } else {
                    sal_memcpy((pkt->_dpp_hdr + hdr_len_read), 
                               (((char *)(&tdata)) + 2), num_bytes);
                    hdr_len_read += num_bytes;
                    if (check_mc &&
                        (hdr_len_read == DPP_HDR_FTMH_BASE_LEN)) {
                        /* check if packet is system multicast */
                        if (tdata & 0x1) {
                            /* SYS_MC, FTMH-OUTLIF(CUD) ext hdr present */
                            hdr_len += 2;
                            is_mc = 1;
                        }
                    }
                }
            } else {
                sal_memcpy((pkt->_pkt_data.data + pkt->pkt_len), 
                           (((char *)(&tdata)) + 2), num_bytes);
                pkt->pkt_len += num_bytes;
            }
        }
        if (!eop) {
            rv = READ_CPUASYNCHRONOUSPACKETDATAr(unit, &regval);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_RX,
                          (BSL_META_U(unit,
                                      "Unit:%d RX poll thread failed (%s) to read "
                                      "CPUASYNCHRONOUSPACKETDATA reg\n"), unit, bcm_errmsg(rv)));
            }
        }
    }
    {
    int rc = soc_ppd_trap_packet_parse(unit, buff, buff_len, &packet_info);
    if (rc < 0) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "RX buffer translate err. \n")));
        rv = BCM_E_INTERNAL;
    }
    }
    if (BCM_SUCCESS(rv)) {
        switch (hdr_len) {
        case DPP_HDR_FTMH_BASE_LEN:
            pkt->_dpp_hdr_type = DPP_HDR_ftmh_base;
            break;
        case DPP_HDR_FTMH_OUTLIF_EXT_LEN: /* or DPP_HDR_FTMH_MC_LB_EXT_LEN */
            if (check_mc && is_mc) {
                pkt->_dpp_hdr_type = DPP_HDR_ftmh_mc_lb_ext;
            } else {
                pkt->_dpp_hdr_type = DPP_HDR_ftmh_outlif_ext;
            }
            break;
        case DPP_HDR_FTMH_OUTLIF_MC_LB_EXT_LEN:
            pkt->_dpp_hdr_type = DPP_HDR_ftmh_outlif_mc_lb_ext;
            break;
        default:
            /* should not hit this */
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d Invalid header length (%d) on RX'ed packet. \n"), 
                                  unit, hdr_len));
            DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
            return;
        }

        /* Remove PPH from pkt_len */
        pkt->pkt_len -= (packet_info.ntwrk_header_ptr - hdr_len);
        pkt->tot_len = pkt->pkt_len + hdr_len;
        sal_memcpy((pkt->_pkt_data.data ), 
                   (((char *)(buff)) + packet_info.ntwrk_header_ptr), 
                   pkt->pkt_len);
        pkt->_pkt_data.len = pkt->pkt_len;
        rv = _bcm_dpp_mod_port_from_sys_port(unit, &mod_id, &mod_port, 
                                             packet_info.src_sys_port);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d _bcm_dpp_mod_port_from_sys_port failed"), unit));
            return;
        }
        pkt->src_mod = mod_id;
        pkt->src_port = mod_port;
        pkt->rx_unit = unit;
        pkt->rx_port = pkt->src_port;
        pkt->rx_reason = packet_info.cpu_trap_code;

        queue = DPP_RX_QUEUE(unit);
        if (queue == NULL) {
            /* Should not hit this. Some internal error */
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d Invalid RX queue state. \n"), unit));
            DPP_RX_PKT_USED_SET_LOCK(unit, pkt->_idx, 0);
        } else {
            rx_ctl[unit]->tot_pkts++;
            rx_ctl[unit]->pkts_since_start++;

            _Q_ENQUEUE_LOCK(queue, pkt);

            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "Unit:%d RX enqueued "
                                    "a packet (%p) successfully\n"), unit, pkt));
            RX_THREAD_NOTIFY(unit);
        }
    }
#endif /*defined(BCM_88640_A0)*/
}
/*
 * Function:    _bcm_petra_rx_poll_pkt_thread
 * Purpose:     User level thread that polls for packets
 * Parameters:  param (IN)  - unused
 * Notes:       This thread wakes up periodically and polls for any available 
 *              packets in the CPU RX path. Packets are pushed into queue to 
 *              be picked up by RX thread.
 *              This RX Poll thread exit is controlled by RX thread controls 
 *              (rx_control.thread_running)
 */
#define CHECK_RX_POLL_THREAD_DONE    \
    if (!rx_control.thread_running) goto rx_poll_thread_done

STATIC void
_bcm_petra_rx_poll_pkt_thread(void *param)
{
    int     unit;
    uint32  val;
    COMPILER_REFERENCE(param);

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META("RX packet poll thread "
"starting\n")));

    while (rx_control.thread_running) {
        CHECK_RX_POLL_THREAD_DONE;
        for (unit = 0; unit < BCM_LOCAL_UNITS_MAX; unit++) {
            if (!RX_UNIT_STARTED(unit)) {
                continue;
            }
            CHECK_RX_POLL_THREAD_DONE;

                while (_bcm_petra_rx_pkt_available(unit, &val)) {
                CHECK_RX_POLL_THREAD_DONE;
                if (SOC_IS_PETRAB(unit)) {
                    /* free pkt data on the free list */
                    _bcm_petra_rx_pkt_enqueue(unit, val);
                }
                
                sal_usleep(RX_POLL_WAIT_SLEEP);
            }
        }

        LOG_DEBUG(BSL_LS_BCM_RX,
                  (BSL_META("RX poll thread Sleeping"
" %d at %u\n"), RX_POLL_SLEEP, sal_time_usecs()));
        sal_usleep(RX_POLL_SLEEP);
        LOG_DEBUG(BSL_LS_BCM_RX,
                  (BSL_META("RX poll thread Woke up"
"at %u\n"), sal_time_usecs()));
        INIT_SLEEP;
    }

rx_poll_thread_done:
    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META("RX: Packet poll thread exitting\n")));
    sal_thread_exit(0);
}


/*
 * Function:    _bcm_petra_rx_thread_start
 * Purpose:     Start the RX thread and RX polling thread when using 
 *              Async CPU interface
 * Parameters:  unit (IN)   - device number
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_rx_thread_start(int unit)
{
    int priority;

    BCMDNX_INIT_FUNC_DEFS;
    /* Timer/Event semaphore thread sleeping on. */
    if (rx_control.pkt_notify == NULL) {
        rx_control.pkt_notify = sal_sem_create("RX pkt ntfy", sal_sem_BINARY, 
                                               0);
        if (rx_control.pkt_notify == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }
        rx_control.pkt_notify_given = FALSE;
    }

    /* RX start/stop on one of the units protection mutex. */
    if (rx_control.system_lock == NULL) {
        rx_control.system_lock = sal_mutex_create("RX system lock");
        if (rx_control.system_lock == NULL) {
            sal_sem_destroy(rx_control.pkt_notify);
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }
    }

    priority = rx_ctl[unit]->rx_thread_pri;

    /* Start rx thread. */
    rx_control.rx_tid = sal_thread_create("bcmRX", SAL_THREAD_STKSZ,
                                     priority, _bcm_petra_rx_pkt_thread, NULL);

    /* Start rx polling thread */
    rx_poll_tid = sal_thread_create("bcmRxPoll", SAL_THREAD_STKSZ,
                                priority, _bcm_petra_rx_poll_pkt_thread, NULL);

    /* Thread creation error handling. */
    if ((rx_control.rx_tid == NULL) || (rx_poll_tid == NULL)) {
        sal_sem_destroy(rx_control.pkt_notify);
        sal_mutex_destroy(rx_control.system_lock);
        rx_control.pkt_notify = NULL;
        rx_control.system_lock = NULL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    _bcm_petra_rx_user_cfg_check
 * Purpose:     Validate and adjust user specified cfg to reasonable values
 * Parameters:  unit (IN)   - device number
 */
void
_bcm_petra_rx_user_cfg_check(int unit)
{
    if (RX_PPS(unit) < 0) {
        RX_PPS(unit) = 0;
    }

    
}



/*
 * Function:    bcm_petra_rx_start
 * Purpose:     Starts bcm_rx thread for Soc_petra device. Allocates packet 
 *              descriptors, packet buffers, and initializes HW to recveive
 *              data into buffers.
 * Parameters:  unit (IN)   - device number
 *              cfg (IN)    - rx config parameters, if null, default is used
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_rx_start(int unit, bcm_rx_cfg_t *cfg)
{
#ifdef BCM_PETRAB_SUPPORT
    int rv;
#endif

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_ARAD_SUPPORT
    
    if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT(_bcm_common_rx_start(unit, cfg));  
        BCMDNX_IF_ERR_EXIT(soc_dpp_arad_dma_init(unit));              
    } 
#endif
#ifdef BCM_PETRAB_SUPPORT
   
   if (SOC_IS_PETRAB(unit)) {   
    RX_INIT_CHECK(unit);

    if (RX_UNIT_STARTED(unit)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "Unit:%d RX already started. \n"), unit));
        BCM_RETURN_VAL_EXIT(BCM_E_BUSY);
    }

    RX_LOCK(unit);
    if (cfg != NULL) {
        if (cfg->pkt_size == 0) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d RX start error. Invalid (%d) pkt_size"
                                  " specified\n"), unit, cfg->pkt_size));
            RX_UNLOCK(unit);
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }
        sal_memcpy(&rx_ctl[unit]->user_cfg, cfg, sizeof(bcm_rx_cfg_t));
        if (cfg->rx_alloc == NULL) {
            rx_ctl[unit]->user_cfg.rx_alloc = DPP_RX_DEFAULT_ALLOC;
        }
        if (cfg->rx_free == NULL) {
            rx_ctl[unit]->user_cfg.rx_free = DPP_RX_DEFAULT_FREE;
        }
        _bcm_petra_rx_user_cfg_check(unit);
    }

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Unit:%d Starting rx thread\n"),
                            unit));

    rv = bcm_rx_pool_setup_done();
    if (!rv) {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "Unit:%d setting up rx "
                                "pool\n"), unit));
        rv = bcm_rx_pool_setup(-1, 
                             rx_ctl[unit]->user_cfg.pkt_size + sizeof(void *));
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Unit:%d Error(%s) setting up rx pool\n"), unit, 
                                  bcm_errmsg(rv)));
            RX_UNLOCK(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* reset the counters */
    rx_ctl[unit]->pkts_since_start = 0;
    rx_ctl[unit]->pkts_owned = 0;

    /* start the RX thread */
    RX_INTR_LOCK;
    if (!rx_control.thread_running) {
        rx_control.thread_running = TRUE;
        RX_INTR_UNLOCK;
        rv = _bcm_petra_rx_thread_start(unit);
        if (BCM_FAILURE(rv)) {
            rx_control.thread_running = FALSE;
            _bcm_petra_rx_cleanup(unit);
            RX_UNLOCK(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        RX_INTR_UNLOCK;
    }

    rx_ctl[unit]->flags |= BCM_RX_F_STARTED;
    RX_UNLOCK(unit);
    BCM_EXIT;
    }
#endif  
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get the PPD Trap code according to the BCM Trap code
 */
void _bcm_dpp_rx_ppd_trap_get(int unit,
                                 bcm_rx_trap_t trap_type,
                                 int *soc_ppd_trap_id){

    *soc_ppd_trap_id = _bcm_rx_trap_to_ppd[trap_type];

}

/*
 * Get the BCM Trap type according to the PPD Trap ID
 */
void _bcm_dpp_rx_trap_type_get(int unit, 
                               int soc_ppd_trap_id, 
                               uint8 *found,
                               bcm_rx_trap_t *trap_type)
{
    bcm_rx_trap_t trap_type_index = 0;
    int soc_ppd_trap_id_cmp = soc_ppd_trap_id;

    /* Map all user defined trap codes to bcmRxTrapUserDefine */
    if (SOC_IS_ARAD(unit)) {
        if ((SOC_PPD_TRAP_CODE_USER_DEFINED_0 <= soc_ppd_trap_id) && (soc_ppd_trap_id <= _BCM_LAST_USER_DEFINE)) {
            soc_ppd_trap_id_cmp = SOC_PPD_TRAP_CODE_USER_DEFINED_0;
        }
        else if (((SOC_PPD_TRAP_CODE_PROG_TRAP_0 <= soc_ppd_trap_id) && (soc_ppd_trap_id <= SOC_PPD_TRAP_CODE_PROG_TRAP_3))
                 || ((SOC_PPD_TRAP_CODE_RESERVED_MC_0 <= soc_ppd_trap_id) && (soc_ppd_trap_id <= SOC_PPD_TRAP_CODE_RESERVED_MC_7))) {
            soc_ppd_trap_id_cmp = _BCM_TRAP_CODE_VIRTUAL_BASE;
        }
    }

    *found = FALSE;
    for (trap_type_index = 0; trap_type_index < sizeof(_bcm_rx_trap_to_ppd) / sizeof(SOC_PPD_TRAP_CODE); trap_type_index ++) {
        if (_bcm_rx_trap_to_ppd[trap_type_index] == soc_ppd_trap_id_cmp) {
            *found = TRUE;
            *trap_type = trap_type_index;
            break;
        }
    }
}

int _bcm_rx_ppd_trap_code_from_trap_id(int unit, int trap_id, SOC_PPD_TRAP_CODE *ppd_trap_code) {
     bcm_dpp_rx_virtual_traps_t p_virtual_traps;

     BCMDNX_INIT_FUNC_DEFS;

     if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id)) {
        if (_bcm_dpp_rx_virtual_trap_get(unit,_BCM_TRAP_ID_TO_VIRTUAL(trap_id),&p_virtual_traps) != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Function _bcm_dpp_rx_virtual_trap_get failed\n")));
        }

        if (p_virtual_traps.soc_ppd_prog != 0) {
            *ppd_trap_code = p_virtual_traps.soc_ppd_prog;
        } else if (p_virtual_traps.soc_ppd_reserved_mc != 0) {
            *ppd_trap_code = p_virtual_traps.soc_ppd_reserved_mc;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Expected trap_id %d to be mapped to a programable trap or reserved mc trap"), trap_id));
        }
    } else {
        *ppd_trap_code = trap_id;
    }

exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_rx_trap_type_create(
    int unit, 
    int flags, 
    bcm_rx_trap_t trap_type,
    int *trap_id)
{
    int rv = BCM_E_NONE; 
    int epp =0;
    int alloc_flags=0;
    uint32 oamp_error_trap_id;

    BCMDNX_INIT_FUNC_DEFS;
    
    if(trap_id == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("trap_id  pointer is NULL"))); 
    }

    /*Check if enum is valid and in range*/
    if(trap_type >= bcmRxTrapCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("trap_type not valid"))); 
    }


    if ((trap_type >= bcmRxTrapOampTrapErr) && (trap_type <= bcmRxTrapOampProtection)) {
    	if (!SOC_IS_ARADPLUS(unit)) {
    		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
    	}

        /* OAMP trap - taken from a separate pool, must be created with_id */
        if (!(flags & BCM_RX_TRAP_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OAMP Error traps must be created with WITH_ID flag"))); 
        }
        if (!_BCM_TRAP_ID_IS_OAM(*trap_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OAMP Error traps must be in range 0x400-0x4FF"))); 
        }
        oamp_error_trap_id = _BCM_TRAP_ID_TO_OAM(*trap_id);
        rv = _bcm_dpp_oam_error_trap_allocate(unit, trap_type, oamp_error_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

     if ((flags & BCM_RX_TRAP_WITH_ID) && ( (trap_type == bcmRxTrapUserDefine)||(trap_type == bcmRxTrapL2Cache) )) {
        alloc_flags = SHR_RES_ALLOC_WITH_ID;
        if (trap_type == bcmRxTrapL2Cache) {
            *trap_id = _BCM_TRAP_VIRTUAL_TO_ID(*trap_id);
        }
    }else {    
        alloc_flags = 0;
        _bcm_dpp_rx_ppd_trap_get(unit,
                                     trap_type,
                                     trap_id);
    }


    /*Check if trap type is valid*/
    if(*trap_id == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("trap_id value is -1"))); 
    }

    rv = _bcm_dpp_am_trap_alloc(unit,alloc_flags,*trap_id,&epp);
    BCMDNX_IF_ERR_EXIT(rv);

    if(trap_type == bcmRxTrapUserDefine)
    {
        /*User define allocation, without ID*/
         *trap_id = epp;    
    }else if (trap_type == bcmRxTrapL2Cache) {
        /*Set bcmRxTrapL2Cache trap id with bit 30 high*/
        *trap_id = _BCM_TRAP_VIRTUAL_TO_ID(epp);
    }                              
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}


/* Destroy user defined trap code. */
int bcm_petra_rx_trap_type_destroy(
    int unit,  
    int trap_id)
{
    int                                         elem=0,flags=0,ittr_counter,current_trap_id=0;
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO      default_info;
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO         egrr_default_info;

    int                                         rv= BCM_E_NONE, virtual_trap_id;
    bcm_dpp_rx_virtual_traps_t                  virtual_traps_p;
    uint32                                      oamp_error_trap_id;

    BCMDNX_INIT_FUNC_DEFS;

    if (_BCM_TRAP_ID_IS_OAM(trap_id)) {
    	if (!SOC_IS_ARADPLUS(unit)) {
    		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
    	}
        oamp_error_trap_id = _BCM_TRAP_ID_TO_OAM(trap_id);
        rv = _bcm_dpp_oam_error_trap_destroy(unit, oamp_error_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&default_info);
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(&egrr_default_info);
    default_info.dest_info.frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_DROP;
    default_info.processing_info.frwrd_type = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
    unit = (unit);

    if (!_BCM_TRAP_ID_IS_EGRESS(trap_id)) {    
        if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id)) {
            virtual_trap_id = _BCM_TRAP_ID_TO_VIRTUAL(trap_id);

            if (_bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_get function returned error"))); 
            }
            if (virtual_traps_p.soc_ppd_reserved_mc != 0) {
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,virtual_traps_p.soc_ppd_reserved_mc,&default_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                virtual_traps_p.soc_ppd_reserved_mc = 0;
                if (_bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_set function returned error"))); 
                    }
                }
            if (virtual_traps_p.soc_ppd_prog != 0) {
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,virtual_traps_p.soc_ppd_prog,&default_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                virtual_traps_p.soc_ppd_prog = 0;
                if (_bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_set function returned error"))); 
                }
            }
        }else
        {
            ittr_counter=0;
            current_trap_id = trap_id;
            do { 
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,current_trap_id,&default_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                /* Check if there are other actions*/
                rv = _bcm_dpp_trap_mate_get(unit, current_trap_id);
                if (rv != -1)
                {
                    /* Update trap id only if there is a mate.*/
                    current_trap_id = rv;
                }
                 ittr_counter++;

            } while ( (rv != -1) && (ittr_counter < MAX_ITTR_COUNT) );
            rv = BCM_E_NONE; /* clear rv */
            if(ittr_counter == MAX_ITTR_COUNT) {                
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Reached max iteration while destroying single instance trap : %d \n"), MAX_ITTR_COUNT));                                             
            }
        }
    } 
    else {
        /* Additional care for egress trap*/
        rv = _bcm_dpp_rx_egress_set(unit,_bcm_dpp_rx_trap_id_to_egress(trap_id),&egrr_default_info);
        BCM_IF_ERROR_RETURN(rv);                
    }

    /* egress trap is traeted as single instance
       For ingress case:if there is a mate-> dealloc only the first trap_id */
    rv = _bcm_dpp_am_trap_dealloc(unit,flags,trap_id,elem); 
    BCM_IF_ERROR_RETURN(rv);

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;

}

int bcm_petra_rx_trap_type_get(int unit, int flags, bcm_rx_trap_t type, int *trap_id)
{

    BCMDNX_INIT_FUNC_DEFS;

    
    if(trap_id == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("trap_id  pointer is NULL"))); 
    }

    /*Check if enum is valid and in range*/
    if(type >= bcmRxTrapCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("type is not valid"))); 
    }


    if ((type >= bcmRxTrapOampTrapErr) && (type <= bcmRxTrapOampProtection))  /* When creating an OAM trap - the "BCM_RX_TRAP_WITH_ID" flag must be transfarred as a param.
              Since we don't support getting the trap type with the flag: "BCM_RX_TRAP_WITH_ID" - We don't support getting the type of an OAM trap.*/
    {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Getting the trap_id with the flag: BCM_RX_TRAP_WITH_ID is unsupported")));    
    }

    if (flags & BCM_RX_TRAP_WITH_ID)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Getting the trap_id with the flag: BCM_RX_TRAP_WITH_ID is unsupported")));    
    }
    else 
    {    
        _bcm_dpp_rx_ppd_trap_get(unit, type, trap_id);

    }


    /*Check if trap type is valid*/
    if(*trap_id == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("trap_id value is -1"))); 
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* Set how to handle packets according to trap type. */
int bcm_petra_rx_trap_set(
    int unit, 
    int trap_id, 
    bcm_rx_trap_config_t *config)
{
    int rv= BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_rx_trap_set(unit, trap_id, config, 0);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/* Set how to handle packets according to trap type - including offset index. */
int _bcm_dpp_rx_trap_set(
    int unit, 
    int trap_id, 
    bcm_rx_trap_config_t *config,
    uint8 trap_fwd_offset_index)
{
    int                                        rv= BCM_E_NONE;
    int                                        virtual_trap_id;
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO     prm_profile_info;
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO        prm_egr_profile_info,default_info;
    int                                        ittr_counter;

    SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO     snoop_profile_info;
    bcm_dpp_rx_virtual_traps_t                 virtual_traps_p;
    uint32                                     oamp_error_trap_id;

    
    BCMDNX_INIT_FUNC_DEFS;
    if(config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL"))); 
    }

    if (_BCM_TRAP_ID_IS_OAM(trap_id)) {
    	if (!SOC_IS_ARADPLUS(unit)) {
    		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
    	}
		if (config->color || config->counter_num || config->dest_group || config->ether_policer_id || config->policer_id || config->prio || config->snoop_cmnd ||
			config->snoop_strength || config->trap_strength) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only dest_port field should be set for OAMP trap."))); 
		}
        oamp_error_trap_id = _BCM_TRAP_ID_TO_OAM(trap_id);
        rv = _bcm_dpp_oam_error_trap_set(unit, oamp_error_trap_id, config->dest_port);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&prm_profile_info);
    prm_profile_info.dest_info.frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_DROP;
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(&default_info);   
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(&prm_egr_profile_info); 
    SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&snoop_profile_info);

    if (trap_fwd_offset_index != 0) {
        prm_profile_info.bitmap_mask  |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET;
        prm_profile_info.processing_info.frwrd_offset_index = trap_fwd_offset_index;
    }
    
    if (BCM_E_EXISTS != _bcm_dpp_am_trap_is_alloced(unit,trap_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trap id %d is in not allocated, on unit:%d \n"), trap_id,unit));
    }

     if (_BCM_TRAP_ID_IS_EGRESS(trap_id) && 
         ((config->flags & BCM_RX_TRAP_UPDATE_POLICER) ||
          (config->flags & BCM_RX_TRAP_UPDATE_ETHERNET_POLICER) ||
          (config->flags & BCM_RX_TRAP_UPDATE_COUNTER) ||
          (config->flags & BCM_RX_TRAP_DEST_MULTICAST) ||
          (config->flags & BCM_RX_TRAP_BYPASS_FILTERS) ||
          (config->flags & BCM_RX_TRAP_LEARN_DISABLE) ||
          (config->flags & BCM_RX_TRAP_WITH_ID) ||
          (config->flags & BCM_RX_TRAP_REPLACE) ||
          (config->flags & BCM_RX_TRAP_TRAP) ||
          (config->flags & BCM_RX_TRAP_UPDATE_ADD_VLAN)||
          (config->flags & BCM_RX_TRAP_UPDATE_FORWARDING_TYPE))) {

         BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Egress profile doesn't support those features")));
     }
    unit = (unit); 

    /*Insert snoop profile data*/
    snoop_profile_info.snoop_cmnd = config->snoop_cmnd;
    snoop_profile_info.strength = config->snoop_strength;  

    if (!_BCM_TRAP_ID_IS_EGRESS(trap_id)) {

        /*User define traps must have trap_strength =0*/
        if ( (SOC_PPD_TRAP_CODE_USER_DEFINED_0 <= trap_id) && (trap_id <= _BCM_LAST_USER_DEFINE) ) { 
            if( config->trap_strength != 0)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("User define traps must have trap_strength =0"))); 
            }

        }
        /*Ingress trap handle*/
        prm_profile_info.strength = config->trap_strength; 

        if (config->flags & (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_DEST_MULTICAST)) {
            prm_profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
            if (config->flags & BCM_RX_TRAP_DEST_MULTICAST) {
                prm_profile_info.dest_info.frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_MC;
                prm_profile_info.dest_info.frwrd_dest.dest_id = config->dest_group;
            }   
            else{
                rv= _bcm_dpp_gport_to_fwd_decision(unit, config->dest_port, &prm_profile_info.dest_info.frwrd_dest);
                BCMDNX_IF_ERR_EXIT(rv);

            }
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_PRIO) {
             prm_profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_TC;
            prm_profile_info.cos_info.tc = config->prio;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_COLOR) {
            prm_profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP;
            prm_profile_info.cos_info.dp = config->color;
        }

        if (config->flags & BCM_RX_TRAP_BYPASS_FILTERS) {
            prm_profile_info.processing_info.is_control = TRUE;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_POLICER) {
           /*There is no need for bitmap mask, called meter*/ 
            prm_profile_info.meter_info.meter_id = config->policer_id;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_ETHERNET_POLICER) {
            prm_profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_POLICER;
            prm_profile_info.policing_info.ethernet_police_id = config->ether_policer_id;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_COUNTER) {
            /*There is no need for bitmap mask*/
            prm_profile_info.bitmap_mask  |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_0;
            prm_profile_info.count_info.counter_id = config->counter_num;
        } 

        if (config->flags & BCM_RX_TRAP_LEARN_DISABLE) {
            /*There is no need for bitmap mask*/
            /*prm_profile_info.bitmap_mask = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;*/
            
            prm_profile_info.processing_info.enable_learning = FALSE;
        }    

        if (config->flags & BCM_RX_TRAP_WITH_ID) {
            /*!!! How should this be used ? */
        }    


        if (config->flags & BCM_RX_TRAP_TRAP) {
            /*!!! Should update a bitmap ?*/
            prm_profile_info.processing_info.is_trap = TRUE;
            /*config->dest_port = CPU*/
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_ADD_VLAN) {
            prm_profile_info.dest_info.add_vsi = TRUE;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_FORWARDING_TYPE) {
            prm_profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET;
            _bcm_dpp_rx_forwarding_type_to_ppd(config->forwarding_type, &prm_profile_info.processing_info.frwrd_type);
        }

        /* Check if virtual bit is marked */
        if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id)) {
        
            /*Clears virtual bit before allocation,range 0-11*/
            virtual_trap_id = _BCM_TRAP_ID_TO_VIRTUAL(trap_id);
            /*Save config data in _rx_virtual_traps array*/

            /* get a local copy of rx virtual traps info */
            if (_bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_get function failed"))); 
            }

            sal_memcpy(&(virtual_traps_p.trap_info), &prm_profile_info, sizeof(prm_profile_info));
            sal_memcpy(&(virtual_traps_p.snoop_info),&snoop_profile_info, sizeof(snoop_profile_info));

            /* update sw_db */
            if (_bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_set function failed"))); 
            }

            if (virtual_traps_p.soc_ppd_reserved_mc != 0) {
                
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,virtual_traps_p.soc_ppd_reserved_mc,&prm_profile_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                /*Connect snoop to trap*/
                rv =  soc_ppd_trap_snoop_profile_info_set(unit,virtual_traps_p.soc_ppd_reserved_mc,&snoop_profile_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                
            }
            if (virtual_traps_p.soc_ppd_prog != 0) {
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,virtual_traps_p.soc_ppd_prog,&prm_profile_info);
                BCM_SAND_IF_ERR_EXIT(rv);
                /*Connect snoop to trap*/
                rv = soc_ppd_trap_snoop_profile_info_set(unit,virtual_traps_p.soc_ppd_prog,&snoop_profile_info);
                BCM_SAND_IF_ERR_EXIT(rv);            
            }

        } else {
            /*Trap is not virtual*/
            ittr_counter = 0;
            do { 
                rv =  soc_ppd_trap_frwrd_profile_info_set(unit,trap_id,&prm_profile_info);
                BCM_SAND_IF_ERR_EXIT(rv);

                /*Connect snoop to trap*/
                rv = soc_ppd_trap_snoop_profile_info_set(unit,trap_id,&snoop_profile_info);
                BCM_SAND_IF_ERR_EXIT(rv);
            
                /*check if there are other actions*/
                rv = _bcm_dpp_trap_mate_get(unit, trap_id);
                if (rv != -1)
                {
                    /*update trap id only if there is a mate.*/
                    trap_id = rv;
                }


                ittr_counter++;

            } while ( (rv != -1) && (ittr_counter < MAX_ITTR_COUNT) );
            rv = BCM_E_NONE; /* clear rv */
            if(ittr_counter == MAX_ITTR_COUNT) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("ittr_counter reached its max value, on unit:%d \n"), unit));
            }
        }
    }else {
        if (config->flags & BCM_RX_TRAP_UPDATE_DEST) {
            prm_egr_profile_info.bitmap_mask = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
            rv= _bcm_dpp_gport_to_fwd_decision(unit, config->dest_port, &prm_profile_info.dest_info.frwrd_dest);
            BCM_IF_ERROR_RETURN(rv);

            if (prm_profile_info.dest_info.frwrd_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT) {
                BCMDNX_IF_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("overriding our port is not supported")));                
            } else if (prm_profile_info.dest_info.frwrd_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_DROP) {
                prm_egr_profile_info.out_tm_port = SOC_PPD_TRAP_ACTION_PKT_DISCARD_ID;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported. Has to be a Local port or Drop"))); 
            }
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_PRIO) {
             prm_egr_profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_TC;
             prm_egr_profile_info.cos_info.tc = config->prio;
        }

        if (config->flags & BCM_RX_TRAP_UPDATE_COLOR) {
            prm_egr_profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP;
            prm_egr_profile_info.cos_info.dp = config->color;
        }


        /*Egress handle*/ 
        rv = _bcm_dpp_rx_egress_set(unit,_bcm_dpp_rx_trap_id_to_egress(trap_id),&prm_egr_profile_info);               
        BCM_IF_ERROR_RETURN(rv); 
    }
      
    BCM_EXIT; 

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_rx_trap_get(
    int unit, 
    int trap_id, 
    bcm_rx_trap_config_t *config)
{
    int                                         rv=BCM_E_NONE;
    int                                         virtual_trap_id=0;
    uint32                                      soc_sand_rv;
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO      prm_profile_info;
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO         prm_egr_profile_info;

    SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO      snoop_profile_info;
    bcm_dpp_rx_virtual_traps_t                  virtual_traps_p;
    uint32                                      trap_index; /* for the allocation manager only, not PPD calls */

    BCMDNX_INIT_FUNC_DEFS;

    if(config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Pointer is NULL ")));         
    }

    if (_bcm_dpp_am_trap_is_alloced(unit,trap_id) != BCM_E_EXISTS ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trap id not alloced"))); 
    }

    unit = (unit);

    if (!_BCM_TRAP_ID_IS_EGRESS(trap_id)) {    
        /* Check if virtual bit is marked */
        if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id)) {

            /*Clears virtual bit before allocation,range 0-11*/
            virtual_trap_id = _BCM_TRAP_ID_TO_VIRTUAL(trap_id);
            /*Save config data in _rx_virtual_traps array*/
             if (_bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE) {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_virtual_trap_get function failed"))); 
             }

             if (_bcm_dpp_rx_fwd_action_to_config(unit,&(virtual_traps_p.trap_info),config) != BCM_E_NONE)
             {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_fwd_action_to_config failed, on unit:%d \n"), unit));                              
             }
     
             config->snoop_cmnd =  virtual_traps_p.snoop_info.snoop_cmnd;
             config->snoop_strength = virtual_traps_p.snoop_info.strength;

        } else {

            soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_id,&prm_profile_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
            if (_bcm_dpp_rx_fwd_action_to_config(unit,&prm_profile_info,config) != BCM_E_NONE)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("_bcm_dpp_rx_fwd_action_to_config failed, on unit:%d \n"), unit));                                                           
            }

            /*Connect snoop to trap*/
            soc_sand_rv = soc_ppd_trap_snoop_profile_info_get(unit,trap_id,&snoop_profile_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            config->snoop_cmnd = snoop_profile_info.snoop_cmnd;
            config->snoop_strength = snoop_profile_info.strength;
        }
    }else {

        /* Translate the indexing between enum and int for egress traps */
        rv = _bcm_dpp_am_eg_trap_index_get(unit, 
                                           _bcm_dpp_rx_trap_id_to_egress(trap_id), /* from the enum */
                                           &trap_index); /* 0...NOF_TRAPS */
        BCMDNX_IF_ERR_EXIT(rv);
        rv =_bcm_dpp_am_template_trap_egress_data_get(unit, trap_id, trap_index, &prm_egr_profile_info);
        BCM_IF_ERROR_RETURN(rv);

        if (prm_egr_profile_info.bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_TC) {
            config->prio = prm_egr_profile_info.cos_info.tc;
            config->flags |= BCM_RX_TRAP_UPDATE_PRIO;
        }

        if (prm_egr_profile_info.bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP) {
            config->color = prm_egr_profile_info.cos_info.dp;
            config->flags |= BCM_RX_TRAP_UPDATE_COLOR;
        }

        if (prm_egr_profile_info.bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST) {
            
            config->dest_port = prm_egr_profile_info.out_tm_port;
            config->flags |= BCM_RX_TRAP_UPDATE_DEST;
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported. Should convert tm port to gport"))); 
        }
    }
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

/* Create snoop command. */
int bcm_petra_rx_snoop_create(
    int unit, 
    int flags, 
    int *snoop_cmnd)
{
    int rv = BCM_E_NONE; 
    int epp =0;
    
    BCMDNX_INIT_FUNC_DEFS;

    if(snoop_cmnd == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("snoop_cmnd pointer is NULL"))); 
    }

    rv = _bcm_dpp_am_snoop_alloc(unit,flags,*snoop_cmnd,&epp);
    BCMDNX_IF_ERR_EXIT(rv);

    /*if use creat without id ,update snoop_cmnd with alloction number. range: 2-15(0=no snoop,1=saved snoop)*/
    if(!(flags & BCM_RX_SNOOP_WITH_ID)) {
        *snoop_cmnd = epp; 
    }

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;  
}

/* Destroy snoop command. */
int bcm_petra_rx_snoop_destroy(
    int unit, 
    int flags, 
    int snoop_cmnd)
{
    int elem=0;
    int rv= BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_snoop_dealloc(unit,flags,snoop_cmnd,elem); 
    BCM_IF_ERROR_RETURN(rv);

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;

}



int bcm_petra_rx_snoop_set(
    int unit, 
    int snoop_cmnd, 
    bcm_rx_snoop_config_t *config)
{



    uint32 soc_sand_rv;    
    /*SOC_PB_ACTION_CMD_SNOOP_INFO prm_profile_info;*/
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO prm_profile_info;
    int rv;
    
    BCMDNX_INIT_FUNC_DEFS;

    SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(&prm_profile_info);
    
    /*Only supported for soc_petra b*/
    

    if(config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL"))); 
    }

    /* check size one of valid options, -1: for all packet, 128 not supported HW bug */
    switch(config->size) {
    case 64:
        prm_profile_info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_64;
        break;
    case 192:
        prm_profile_info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_192;
        break;
    case -1:
        prm_profile_info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config->size not valid"))); 
    }

    _BCM_RX_SNOOP_CONFIG_PROB_TO_TMC_ACTION_CMD_SNOOP_INFO_PROB(config->probability,prm_profile_info.prob );  
 

    if (config->flags & BCM_RX_SNOOP_UPDATE_DEST) {        
        
        if (config->flags & BCM_RX_SNOOP_DEST_MULTICAST) {        
            prm_profile_info.cmd.dest_id.type = SOC_TMC_DEST_TYPE_MULTICAST;
            prm_profile_info.cmd.dest_id.id = config->dest_group;
        }else
        {
            rv = _bcm_dpp_gport_to_tm_dest_info(unit,config->dest_port,&prm_profile_info.cmd.dest_id);
            BCM_SAND_IF_ERR_EXIT(rv);
        }
    }

    if (config->flags & BCM_RX_SNOOP_UPDATE_PRIO) {
       prm_profile_info.cmd.tc.value = config->prio;  
       
    }

    prm_profile_info.cmd.tc.enable = SOC_SAND_NUM2BOOL(config->flags & BCM_RX_SNOOP_UPDATE_PRIO);


    if (config->flags & BCM_RX_SNOOP_UPDATE_COLOR) {
        prm_profile_info.cmd.dp.value = config->color;
    }
    prm_profile_info.cmd.dp.enable = SOC_SAND_NUM2BOOL(config->flags & BCM_RX_SNOOP_UPDATE_COLOR);

    if (config->flags & BCM_RX_SNOOP_UPDATE_POLICER) {
        /*prm_profile_info.cmd.dp = config.policer_id;*/
    }

    if (config->flags & BCM_RX_SNOOP_UPDATE_ETHERNET_POLICER) {

    }

    if (config->flags & BCM_RX_SNOOP_UPDATE_COUNTER) {
    } 


    if (config->flags & BCM_RX_SNOOP_DEST_MULTICAST) {
    }   
    if (prm_profile_info.cmd.dest_id.type == SOC_TMC_DEST_TYPE_MULTICAST) {
        /* mark prm_profile_info.cmd.is_ing_mc true if prm_profile_info.cmd.dest_id.id is an ingress MC group */ 
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_group_exists(unit, prm_profile_info.cmd.dest_id.id, &prm_profile_info.cmd.is_ing_mc));
    }
    /**prm_profile_info.cmd.outlif.value = snoop_cmnd;  consider inserting this in the future*/ 


    unit = (unit);
    if(SOC_IS_PETRAB(unit)){
        /*soc_ppd_trap_snoop_profile_info_set(unit,snoop_cmnd,&prm_profile_info);*/
        soc_sand_rv = soc_pb_action_cmd_snoop_set(unit,snoop_cmnd,&prm_profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
#ifdef BCM_ARAD_SUPPORT
    else if (SOC_IS_ARAD(unit)) {
        soc_sand_rv = arad_action_cmd_snoop_set_unsafe(unit,snoop_cmnd,&prm_profile_info,SOC_TMC_CMD_TYPE_SNOOP);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
#endif

    
    BCM_EXIT;    
    
exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_rx_snoop_get(
    int unit, 
    int snoop_cmnd, 
    bcm_rx_snoop_config_t *config)
{

    uint32 soc_sand_rv;
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO prm_profile_info;
    int rv;


    BCMDNX_INIT_FUNC_DEFS;
    
    
    if(config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("config pointer is NULL")));
    }

    if (_bcm_dpp_am_snoop_is_alloced(unit,snoop_cmnd) != BCM_E_EXISTS ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("snoop_cmnd not alloced"))); 
    }
    unit = (unit);

    if(SOC_IS_PETRAB(unit)){
        /*soc_ppd_trap_snoop_profile_info_set(unit,snoop_cmnd,&prm_profile_info);*/
        soc_sand_rv =  soc_pb_action_cmd_snoop_get(unit,snoop_cmnd,&prm_profile_info);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
    }
#ifdef BCM_ARAD_SUPPORT
    else if (SOC_IS_ARAD(unit)) {
        soc_sand_rv = arad_action_cmd_snoop_get(unit,snoop_cmnd,&prm_profile_info);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
    }
#endif
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("supporting only ARAD and PETRAB devices"))); 
    }

    if(prm_profile_info.cmd.dest_id.type == SOC_TMC_DEST_TYPE_MULTICAST) {
        config->dest_group = prm_profile_info.cmd.dest_id.id;
        config->flags |= BCM_RX_SNOOP_DEST_MULTICAST;
    }else
    {
        rv = _bcm_dpp_gport_from_tm_dest_info(unit,&config->dest_port,&prm_profile_info.cmd.dest_id);
        BCM_SAND_IF_ERR_EXIT(rv);
        config->flags |= BCM_RX_SNOOP_UPDATE_DEST;
    }

    if(prm_profile_info.cmd.tc.enable) {
        config->prio = prm_profile_info.cmd.tc.value;  
        config->flags |= BCM_RX_SNOOP_UPDATE_PRIO;
    }

    if(prm_profile_info.cmd.dp.enable) {
        config->color = prm_profile_info.cmd.dp.value;
        config->flags |= BCM_RX_SNOOP_UPDATE_COLOR;
    }
        
    config->size  = _BCM_RX_TMC_ACTION_SIZE_TO_SNOOP_SIZE(prm_profile_info.size);
    _BCM_RX_TMC_ACTION_CMD_SNOOP_INFO_PROB_TO_SNOOP_CONFIG_PROB(config->probability,prm_profile_info.prob );


    /*config.flags  = ??? */

    /*config->policer_id = ??? */
    /*config->dest_group  = ??? */
    /*config->dest_group  = ??? */
    /*config->counter_num  = ??? */
    
    BCM_EXIT;     
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_rx_queue_channel_get(
    int unit, 
    bcm_cos_queue_t queue_id, 
    bcm_rx_chan_t *chan_id)
{
BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_channel_get(unit,queue_id, chan_id));
    } 
#endif
#ifdef BCM_PETRAB_SUPPORT

   if (SOC_IS_PETRAB(unit)){ 
       return BCM_E_UNAVAIL;
   }
#endif
   BCM_EXIT;    
exit:
   BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_rx_queue_channel_set(
    int unit, 
    bcm_cos_queue_t queue_id, 
    bcm_rx_chan_t chan_id)
{
BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_channel_set(unit,queue_id, chan_id));
    } 
#endif
#ifdef BCM_PETRAB_SUPPORT

   if (SOC_IS_PETRAB(unit)){ 
       return BCM_E_UNAVAIL;
   }
#endif
   BCM_EXIT;    
exit:
   BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_rx_queue_rate_limit_status_get(
    int unit, 
    bcm_cos_queue_t cosq, 
    int *packet_tokens)
{
BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_rate_limit_status_get(unit, cosq, packet_tokens));
    } 
#endif
#ifdef BCM_PETRAB_SUPPORT

   if (SOC_IS_PETRAB(unit)){ 
       return BCM_E_UNAVAIL;
   }
#endif
   BCM_EXIT;    
exit:
   BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_rx_queue_register(
    int unit, 
    const char *name, 
    bcm_cos_queue_t cosq, 
    bcm_rx_cb_f callback, 
    uint8 priority, 
    void *cookie, 
    uint32 flags)
{
 BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
         BCMDNX_IF_ERR_EXIT (_bcm_common_rx_queue_register(unit,name,cosq,callback,priority, cookie,flags));
     } 
#endif
#ifdef BCM_PETRAB_SUPPORT
   
    if (SOC_IS_PETRAB(unit)){ 
        return BCM_E_UNAVAIL;
    }
#endif
    BCM_EXIT;    
exit:
    BCMDNX_FUNC_RETURN;

}

int 
bcm_petra_rx_queue_unregister(
    int unit, 
    bcm_cos_queue_t cosq, 
    bcm_rx_cb_f callback, 
    uint8 priority)
{
    return BCM_E_UNAVAIL;
}

void
_bcm_dpp_rx_packet_cos_parse(
    int unit, 
    bcm_pkt_t *pkt)
{
#ifdef BCM_ARAD_SUPPORT
    bcm_pbmp_t  bmp;
    bcm_port_t  port;
    uint32 queue_base, queue_offset;
    bcm_error_t rv;
    int core=0; 
    
    if (SOC_IS_ARAD(unit)) {
        /* Extracting port and TC */ 
        unit = (unit); 
        bmp = PBMP_CMIC(unit);
        PBMP_ITER(bmp, port){
            if(port >= ARAD_NOF_FAP_PORTS) {
                break;
            }
            rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core,  port, &queue_base);
            if(BCM_FAILURE(rv)) {
                return;
            }
            rv = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core, port, &queue_offset);
            if(BCM_FAILURE(rv)) {
                return;
            }

            if ((pkt->rx_port >= queue_base) && (pkt->rx_port < (queue_base+queue_offset))) {
                pkt->cos = (pkt->rx_port - queue_base);
                break;
            }
        }
        return;
    }
#endif
}
    /*
     * This function dumps the packet contents, then returns that we have handled and 
     * we are done with the packet. 
     *
     * The bcm_pkt_t structure is defined in orion/include/bcm/pkt.h
     * The RX API supports only one packet buffer per packet. 
     * There is no scatter/gather RX support.
     * 
     * In order to fill pkt->src_port and pkt->src_mod fields we have to access the device,
     * which is not interrupt safe. The flag device_access_allowed should be set
     * when using this function ountside interrupt context in order to enable
     * calculation of these fields. Otherwise they are set to 0.
     */
int
_bcm_dpp_rx_packet_parse(
    int unit, 
    bcm_pkt_t *pkt,
    uint8 device_access_allowed)
{
#ifdef BROADCOM_DEBUG
    int i;
#endif /*BROADCOM_DEBUG*/
#ifdef BCM_ARAD_SUPPORT
    SOC_PPD_TRAP_PACKET_INFO packetInfo;
    int rv = BCM_E_NONE;
    bcm_gport_t mod_port, gport;

    uint32 sand_rv;
    uint8 found, is_lag ;
    uint32 lag_id, lag_member_id, sys_port,
            port_i, tm_port = SOC_MAX_NUM_PORTS; 
    ARAD_PORT_HEADER_TYPE header_type_out;
    bcm_pbmp_t  bmp;
    bcm_port_t  port;
    uint32 queue_base, queue_offset;
    SOC_PPD_LAG_INFO lag_info;
    int core = 0; 
#endif /*BCM_ARAD_SUPPORT*/
    BCMDNX_INIT_FUNC_DEFS;

#if defined(BROADCOM_DEBUG)
    if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityVerbose, unit)) {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "*************************************RX: (Egress) port %d, cos %d, tot len %d, reason %04x\n"), 
                                pkt->rx_port, pkt->cos, pkt->tot_len, pkt->rx_reason));
    
        {
            char *packet_ptr, *ch_ptr;
            packet_ptr = sal_alloc(3 * pkt->pkt_len, "Packet print");
            if (packet_ptr != NULL) {
                ch_ptr = packet_ptr;
                    /* Dump the packet data block */
                for(i = 0; i < pkt->pkt_len; i++) {
                        if ((i % 4) == 0) {
                            sal_sprintf(ch_ptr," ");
                            ch_ptr++;
                        }
                        if ((i % 32) == 0) {
                            sal_sprintf(ch_ptr,"\n");
                            ch_ptr++;
                        }
                        
                        sal_sprintf(ch_ptr,"%02x", (pkt->_pkt_data.data)[i]); 
                        ch_ptr++;
                        ch_ptr++;
                }
                sal_sprintf(ch_ptr,"\n");
                LOG_VERBOSE(BSL_LS_BCM_RX,
                            (BSL_META_U(unit,
                                        "%s"),packet_ptr));
                sal_free(packet_ptr);
            }
        }
    }
#endif/*BROADCOM_DEBUG*/
   
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        /* Extracting port and TC */ 
        unit = (unit);
        found = 0;    
        bmp = PBMP_CMIC(unit);
        PBMP_ITER(bmp, port){
            if(port >= ARAD_NOF_FAP_PORTS) {
                break;
            }
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, port, &queue_base));
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core, port, &queue_offset));

            if ((pkt->rx_port >= queue_base) && (pkt->rx_port < (queue_base+queue_offset))) {
                found = 1;
                pkt->cos = (pkt->rx_port - queue_base);
                BCM_GPORT_LOCAL_SET(pkt->dst_gport, port);
                pkt->src_gport = -1;
                break;
            }
        }
        if (!found) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Error in rx_port extraction! Unit=%d, queue_id=%d\n"), unit, pkt->rx_port));
        }

        found = 0; 
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &bmp));
        PBMP_ITER(bmp,port_i){ 
              if (port_i != SOC_DPP_PORT_INTERNAL_ERP(0)) {
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
                if(tm_port == port) {
                    found = 1;
                    break;
                }
            }
        }

        if (!found) {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "Internal Error in finding port header type! Unit=%d, port=%d\n"), unit, port));
        }
        
        /* Checking header type */
        if (found) {
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_get(unit, tm_port, &header_type_out)); 
            if ((header_type_out == SOC_TMC_PORT_HEADER_TYPE_CPU)
                || 
                (header_type_out == SOC_TMC_PORT_HEADER_TYPE_STACKING) )
            {
                sand_rv = soc_ppd_trap_packet_parse(unit, pkt->_pkt_data.data, pkt->tot_len, &packetInfo);
                BCM_SAND_IF_ERR_EXIT(sand_rv);

                pkt->cos = packetInfo.tc;
                pkt->prio_int = packetInfo.dp;
                pkt->pkt_len = packetInfo.size_in_bytes-packetInfo.ntwrk_header_ptr;
                if (packetInfo.is_trapped) {

                    _bcm_dpp_rx_trap_type_get(unit, packetInfo.cpu_trap_code, &found, &pkt->rx_reason);        
                    if (!found) {
                        LOG_VERBOSE(BSL_LS_BCM_RX,
                                    (BSL_META_U(unit,
                                                "No trap code found! Unit %d\n"), unit));
                    }
                    if (pkt->rx_reason == bcmRxTrapUserDefine) {
                        pkt->rx_trap_data = packetInfo.cpu_trap_code;
                    }
                    else {
                        pkt->rx_trap_data = packetInfo.cpu_trap_qualifier;
                    }
                }

                if (device_access_allowed) {
                    sand_rv = arad_ports_logical_sys_id_parse(unit, packetInfo.src_sys_port, 
                                                              &is_lag, &lag_id, &lag_member_id, 
                                                              &sys_port);
                     BCM_SAND_IF_ERR_EXIT(sand_rv);

                     if (is_lag) {
                         pkt->src_trunk = lag_id;

                        SOC_PPD_LAG_INFO_clear(&lag_info);

                        /* Get LAG info */
                        rv = soc_ppd_lag_get(unit, lag_id, &lag_info);
                        BCM_SAND_IF_ERR_EXIT(rv);

                        /* Get the gport of index lag_member_ndx into gport */
                        rv = bcm_petra_stk_sysport_gport_get(unit, lag_info.members[lag_member_id].sys_port, &gport);
                        BCMDNX_IF_ERR_EXIT(rv);

                        
                        /*and set to src_gport after the problem with src_gport/dest_gport cunfusion is fixed. */
                        pkt->src_port = BCM_GPORT_MODPORT_PORT_GET(gport);
                        pkt->src_mod = BCM_GPORT_MODPORT_MODID_GET(gport);


                     }
                     else {
                         pkt->src_trunk = -1;
                         BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, packetInfo.src_sys_port);
                         rv = bcm_petra_stk_sysport_gport_get(unit, sys_port, &mod_port);
                         if (BCM_E_NONE != rv) {
                             LOG_VERBOSE(BSL_LS_BCM_RX,
                                         (BSL_META_U(unit,
                                                     "rx_intr_process_packet: system gport unvalid: "
                                                     "Unit %d Port %d\n"),
                                          unit, sys_port));
                         } else {
                             pkt->src_port = BCM_GPORT_MODPORT_PORT_GET(mod_port);
                             pkt->src_mod = BCM_GPORT_MODPORT_MODID_GET(mod_port);
                         }

                     }
    #ifdef BROADCOM_DEBUG
                     if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityVerbose, unit)) {
                         SOC_PPD_TRAP_PACKET_INFO_print(&packetInfo);
                         LOG_VERBOSE(BSL_LS_BCM_RX,
                                     (BSL_META_U(unit,
                                                 "*************************************RX: is_lag %d, lag_id %d, src_port %d, src_mod %d\n"), 
                                                 is_lag,pkt->src_trunk, pkt->src_port, pkt->src_mod));

                 }
#endif/*BROADCOM_DEBUG*/
                 if ((int8)(packetInfo.otm_port) == -1) {
                     pkt->multicast_group =  packetInfo.outlif_mcid;
                 } 
                 pkt->dest_port =  packetInfo.otm_port;
                 pkt->stk_load_balancing_number =  packetInfo.lb_key;
                 pkt->stk_dst_gport = packetInfo.dsp_ext;
                 pkt->stk_route_tm_domains= packetInfo.stacking_ext;
                 pkt->dst_vport = packetInfo.outlif_mcid;
                 pkt->src_vport = packetInfo.pph_inlif_inrif;

                 if (packetInfo.pph_unknown_addr) {
                     pkt->flags2 |= BCM_PKT_F2_UNKNOWN_DEST;
                 }
                 if (packetInfo.frwrd_type == SOC_PPD_TRAP_MGMT_PKT_FRWRD_TYPE_SNOOP) {
                     pkt->flags2 |= BCM_PKT_F2_SNOOPED;
                 }
                
                 switch (packetInfo.pph_fwd_code) {
                     case SOC_TMC_PKT_FRWRD_TYPE_MPLS:
                         pkt->stk_forward = BCM_PKT_STK_FORWARD_MPLS;
                         break;
                     case SOC_TMC_PKT_FRWRD_TYPE_TRILL:
                         pkt->stk_forward = BCM_PKT_STK_FORWARD_TRILL;
                         break;
                     case SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1:
                         pkt->stk_forward =  BCM_PKT_STK_FORWARD_FCOE;
                         break;
                     case SOC_TMC_PKT_FRWRD_TYPE_SNOOP:
                         pkt->stk_forward =  BCM_PKT_STK_FORWARD_SNOOP;
                         break;
                     case SOC_TMC_PKT_FRWRD_TYPE_TM:
                         pkt->stk_forward =  BCM_PKT_STK_FORWARD_TRAFFIC_MANAGEMENT;
                         break;
                     default:
                         break;
                 }

                }
                else {
                    pkt->src_port = 0;
                    pkt->src_mod = 0;
                }
#if defined(BROADCOM_DEBUG)
                if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityVerbose, unit)) {
                    LOG_VERBOSE(BSL_LS_BCM_RX,
                                (BSL_META_U(unit,
                                            "*************************************RX: (Egress) packet len %d\n"), pkt->pkt_len));
                }
#endif/*BROADCOM_DEBUG*/
            }
        }
        else {
            LOG_WARN(BSL_LS_BCM_RX,
                     (BSL_META_U(unit,
                                 "RX: skipping packet parsing because of non-cpu port header type\n")));
        }
    }
#else
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available for this unit")));
#endif /*BCM_ARAD_SUPPORT*/
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_rx_forwarding_type_to_ppd(bcm_forwarding_type_t bcmForwardingType,
                                          SOC_TMC_PKT_FRWRD_TYPE *ppdForwardingType)
{
    switch(bcmForwardingType) {
    case bcmForwardingTypeL2:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
        break;
    case bcmForwardingTypeIp4Ucast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC;
        break;
    case bcmForwardingTypeIp4Mcast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC;
        break;
    case bcmForwardingTypeIp6Ucast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC;
        break;
    case bcmForwardingTypeIp6Mcast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC;
        break;
    case bcmForwardingTypeMpls:
         *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_MPLS;
        break;
    case bcmForwardingTypeTrill:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_TRILL;
        break;
    case bcmForwardingTypeRxReason:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP;
        break;
    case bcmForwardingTypeFCoE:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1;
        break;
    case bcmForwardingTypeTrafficManagement:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_TM;
        break;
    case bcmForwardingTypeSnoop:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_SNOOP;
        break;
    default:
        return BCM_E_PARAM;
    };
    return BCM_E_NONE;
}

int
_bcm_dpp_rx_ppd_to_forwarding_type(SOC_TMC_PKT_FRWRD_TYPE ppdForwardingType,
									bcm_forwarding_type_t *bcmForwardingType)
{
    switch(ppdForwardingType) {
    case SOC_TMC_PKT_FRWRD_TYPE_BRIDGE :
        *bcmForwardingType = bcmForwardingTypeL2;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC:
        *bcmForwardingType = bcmForwardingTypeIp4Ucast;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC :
        *bcmForwardingType = bcmForwardingTypeIp4Mcast;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC :
        *bcmForwardingType = bcmForwardingTypeIp6Ucast;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC :
        *bcmForwardingType = bcmForwardingTypeIp6Mcast;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_MPLS :
         *bcmForwardingType = bcmForwardingTypeMpls;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_TRILL :
        *bcmForwardingType = bcmForwardingTypeTrill;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP :
        *bcmForwardingType = bcmForwardingTypeRxReason;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1 :
        *bcmForwardingType = bcmForwardingTypeFCoE;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_TM :
        *bcmForwardingType = bcmForwardingTypeTrafficManagement;
        break;
    case SOC_TMC_PKT_FRWRD_TYPE_SNOOP :
        *bcmForwardingType = bcmForwardingTypeSnoop;
        break;
    default:
        return BCM_E_PARAM;
    };
    return BCM_E_NONE;
}

#endif /* BCM_PETRA_SUPPORT */

